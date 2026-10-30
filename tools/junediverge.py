#!/usr/bin/env python
"""junediverge.py - the June-vs-March gate for client source (JUNE_MARCH_DIVERGENCE_PHASES.md, Phase D2).

    python tools/junediverge.py --check FILE [FILE ...]   # PASS / FAIL for the files a phase changed (exit 1 on FAIL)
    python tools/junediverge.py --check-all               # every client source file that holds a SERV_IRUHADEV_JUNE_* flag (about a minute)
    python tools/junediverge.py --list                    # names to avoid and what to write instead, the idiom rows, the open replace rows
    python tools/junediverge.py --open-rows               # every `replace` row of june_march_divergent.tsv that is not done, with its allow-list state
    python tools/junediverge.py --selftest                # pinned revisions and fixtures; exit 1 when a rule stops firing

--check looks at June code only: the ON branch of a defined SERV_IRUHADEV_JUNE_* flag (an `#ifndef` flag is on in its `#else`; the
`#else` of an `#ifdef` is March and is never looked at). Three rules:
  a  a June-named, address-named or phase-tag identifier is DEFINED in the file (function, member, local, constant, namespace, class,
     Lua-bound name). Flag names `SERV_IRUHADEV_*` and the rule-form namespace `_CONST_<CLASS>_CH<n><x>_` are fine.
  b  an idiom row of tools/data/june_march_divergent.tsv written the March way inside June code. Mechanical today: a game-type test
     that is GT_PVP only (June 322cc1a0 tests GT_PVP or GT_PVE: write `GetIsNowPVPCorrectionGame()`). The other idiom rows
     (322d0350, 32d4ce60, 32630d20, 322ece70) have no textual shape; they stay a read of the June body.
  c  a June-code call to a function of a `replace` row that is not done. Done = the row's source names the
     SERV_IRUHADEV_JUNE_DIVERGE_* flag it was ported under, or says "no code", or the tree's definition of that function holds a
     SERV_IRUHADEV_JUNE_DIVERGE_* block. `--open-rows` lists the rest.
Allow-list: tools/data/june_diverge_allow.tsv, one row per line, tab separated, `#` starts a comment:
    file<TAB>function<TAB>name<TAB>reason        (file: basename or path suffix; function: `Name` or `Class::Name`; `*` = any)
`name` is the identifier (rule a), `GT_PVP` (rule b) or the called function (rule c). The reason must be filled in.
Output: one `HIT <rule> file:line [function] name: what to do` line per hit, then `RESULT: PASS` or `RESULT: FAIL, N hit(s)`.
"""
import argparse
import collections
import csv
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import junecensus as jc                                   # noqa: E402  (the same scanner as the census, so both tools agree)

TRUNK = jc.TRUNK
DIVERGENT = os.path.join(HERE, 'data', 'june_march_divergent.tsv')
ALLOW = os.path.join(HERE, 'data', 'june_diverge_allow.tsv')
SCRATCH = os.path.join(TRUNK, '.build', 'junediverge')
DIVERGE_FLAG = 'SERV_IRUHADEV_JUNE_DIVERGE_'
RX_RULE_NS = re.compile(r'^_CONST_\w+_CH\w+_$')
RX_IDENT = re.compile(r'^~?[A-Za-z_]\w*$')

# rule b: (allow-list name, regex on the masked text, what to write instead)
GT_PVP_TEST = [
    re.compile(r'\bGT_PVP\b\s*[!=]=\s*[\w\->.():\s]{0,60}?GetGameType\s*\(\s*\)'),
    re.compile(r'GetGameType\s*\(\s*\)\s*[!=]=\s*(?:CX2Game\s*::\s*)?GT_PVP\b'),
]
GT_PVP_FIX = 'June 322cc1a0 tests GT_PVP or GT_PVE: write GetIsNowPVPCorrectionGame() (X2Game.h), or allow-list it when June tests type 1 only'
GT_PVP_SIBLING = re.compile(r'\bGT_PVE\b|GetIsNowPVPCorrectionGame')

AVOID = [
    ('a  June / june / JUNE in an identifier', 'drop it; June body goes into the March function of that name (#ifdef flag June #else March #endif), or a new name in the class pattern'),
    ('a  Ghidra address in a name (June32481fa0)', 'name what it is (bOberonGuardMemoEquipped, fElectronBallRangeRate); the address goes in the one-line comment'),
    ('a  phase tag in a name (JUNE_CH1I_..., CH4c_..., bCh4Memo)', 'the Enum member (never a renamed alias), or a constant named for its role; `_CONST_<CLASS>_CH4C_` namespaces are the rule form'),
    ('b  GT_PVP == GetGameType() in June code', 'GetIsNowPVPCorrectionGame() when June calls 322cc1a0 (type 1 or 4)'),
    ('c  a call to a replace-row function that is not replaced', 'replace March\'s definition with June\'s whole body under SERV_IRUHADEV_JUNE_DIVERGE_<NAME> first (D1 shape)'),
]


def rel_of(path):
    p = os.path.abspath(path)
    try:
        return os.path.relpath(p, TRUNK).replace('\\', '/')
    except ValueError:
        return p.replace('\\', '/')


# ----------------------------------------------------------------------------------------------------------------------
# tables
# ----------------------------------------------------------------------------------------------------------------------
def read_rows():
    rows = []
    if os.path.exists(DIVERGENT):
        for r in csv.reader(open(DIVERGENT, encoding='utf-8'), delimiter='\t'):
            if r and not r[0].startswith('#') and len(r) > 3:
                r = r + [''] * (10 - len(r))
                rows.append(dict(zip(['name', 'kind', 'june_va', 'verdict', 'june_does', 'march_does', 'write_instead',
                                      'march_callers', 'guard', 'source'], r)))
    return rows


def load_allow():
    out = []
    if os.path.exists(ALLOW):
        for ln, line in enumerate(open(ALLOW, encoding='utf-8'), 1):
            line = line.rstrip('\r\n')
            if not line.strip() or line.startswith('#'):
                continue
            c = line.split('\t')
            if len(c) < 4 or not c[3].strip():
                sys.stderr.write('june_diverge_allow.tsv:%d: needs file, function, name and a reason\n' % ln)
                continue
            out.append((c[0].strip(), c[1].strip(), c[2].strip(), c[3].strip()))
    return out


def allowed(allow, rel, func, name):
    base = rel.rsplit('/', 1)[-1]
    for (f, fn, nm, _) in allow:
        if nm != name:
            continue
        if f != '*' and not (rel.endswith(f) or base == f):
            continue
        if fn != '*' and fn != func and fn != func.rsplit('::', 1)[-1]:
            continue
        return True
    return False


def callable_names(row):
    """the function names a table row talks about (`Class::f / g`, `A/B/C`)"""
    out = []
    for tok in re.split(r'[\s/,;]+', row['name']):
        tok = tok.split('::')[-1].strip('()')
        if RX_IDENT.match(tok):
            out.append(tok)
    return out


# ----------------------------------------------------------------------------------------------------------------------
# preprocessor walk: which lines are June code
# ----------------------------------------------------------------------------------------------------------------------
RX_MASK = re.compile(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'', re.S)


def mask(text):
    """comments and string / char literals -> spaces, newlines kept (so offsets and lines survive)"""
    return RX_MASK.sub(lambda m: re.sub(r'[^\n]', ' ', m.group(0)), text)


def june_lines(masked, defs):
    """-> (set of June-live line numbers, list of (start, end) lines of every JUNE_DIVERGE block)"""
    live = set()
    frames = []                      # [names, jb, val, branch, start_line]
    diverge = []
    lines = masked.split('\n')
    for ln, line in enumerate(lines, 1):
        m = re.match(r'\s*#\s*(ifdef|ifndef|if|elif|else|endif)\b(.*)$', line)
        if m:
            d, rest = m.group(1), m.group(2).strip()
            if d in ('ifdef', 'ifndef'):
                nm = rest.split(' ')[0] if rest else ''
                names = [nm] if nm.startswith(jc.FLAG_PREFIX) else []
                frames.append([names, (0 if d == 'ifdef' else 1) if names else None, (nm in defs) if names else None, 0, ln])
            elif d == 'if':
                v, names, positive = jc.eval_cond(rest, defs)
                frames.append([names, (0 if positive else 1) if names else None, v if names else None, 0, ln])
            elif d in ('elif', 'else'):
                if frames:
                    frames[-1][3] += 1
                    if d == 'elif':
                        v, names, positive = jc.eval_cond(rest, defs)
                        if names and frames[-1][1] is not None:
                            frames[-1][0] = list(set(frames[-1][0]) | set(names))
            elif d == 'endif':
                if frames:
                    f = frames.pop()
                    if any(n.startswith(DIVERGE_FLAG) for n in f[0]):
                        diverge.append((f[4], ln))
            continue
        for f in frames:
            if any(n.startswith(jc.JUNE_FLAG_PREFIX) for n in f[0]) and f[1] is not None and f[3] == f[1] and f[2] is not False:
                live.add(ln)
                break
    return live, diverge


def body_at(bodies, line):
    best = None
    for b in bodies:                                 # (cls, name, june, line)
        if b[3] <= line and (best is None or b[3] >= best[3]):
            best = b
    return ('%s::%s' % (best[0], best[1]) if best[0] else best[1]) if best else ''


# ----------------------------------------------------------------------------------------------------------------------
# the open replace rows
# ----------------------------------------------------------------------------------------------------------------------
_diverge_fn_cache = {}


def diverge_functions():
    """names of the functions whose definition holds a SERV_IRUHADEV_JUNE_DIVERGE_* block (the done test, third form)"""
    if 'v' in _diverge_fn_cache:
        return _diverge_fn_cache['v']
    defs = jc.defined_flags()
    jc.worker_init(defs)
    out = set()
    for path in jc.source_files():
        try:
            raw = open(path, 'rb').read()
        except OSError:
            continue
        if DIVERGE_FLAG.encode() not in raw:
            continue
        res = jc.scan_one(path)
        masked = mask(jc.read_text(path))
        _, blocks = june_lines(masked, defs | {m.group(0) for m in re.finditer(r'SERV_IRUHADEV_\w+', masked)})
        for (a, b) in blocks:
            enclosing = [x for x in res['bodies'] if x[3] <= a]
            if enclosing:
                out.add(max(enclosing, key=lambda x: x[3])[1])
            out.update(x[1] for x in res['bodies'] if a <= x[3] <= b)
    _diverge_fn_cache['v'] = out
    return out


def row_done(row, fns):
    src = row['source'] + ' ' + row['write_instead']
    if DIVERGE_FLAG in src or re.search(r'(?i)\bno code\b', src):
        return True
    return any(n in fns for n in callable_names(row))


def open_rows(rows):
    fns = diverge_functions()
    return [r for r in rows if r['verdict'] == 'replace' and not row_done(r, fns)]


# ----------------------------------------------------------------------------------------------------------------------
# the check
# ----------------------------------------------------------------------------------------------------------------------
def check_text_file(path, defs, open_names, allow, rel=None):
    """-> (hits, n_allowed); a hit is (rule, line, func, name, message)"""
    rel = rel or rel_of(path)
    jc.worker_init(defs)
    res = jc.scan_file(path)
    bodies = res['bodies']
    masked = mask(jc.read_text(path))
    live, _ = june_lines(masked, defs)
    hits, n_allow = [], 0

    def add(rule, ln, name, msg, func=None):
        nonlocal n_allow
        func = func if func is not None else body_at(bodies, ln)
        if allowed(allow, rel, func, name):
            n_allow += 1
        else:
            hits.append((rule, ln, func, name, msg))

    # a: June-named definitions (a flag name is not an identifier; the rule-form namespace is the house form)
    seen = set()
    for (nm, kind, ln, scope, func, flag, addr, note) in res['defs']:
        if not jc.is_june_name(nm) or (kind == 'namespace' and RX_RULE_NS.match(nm)) or (nm, ln) in seen or flag.startswith('!'):
            continue
        seen.add((nm, ln))
        add('a', ln, nm, '%s `%s` carries June / an address / a phase tag: name what it is (see --list)' % (kind, nm), func=func or None)
    for (nm, ln) in res['lua']:
        if (nm, ln) not in seen:
            seen.add((nm, ln))
            add('a', ln, nm, 'Lua-bound name `%s` carries June: name what it is' % nm)
    # b: GT_PVP-only tests in June code
    nlpos = [m.start() for m in re.finditer('\n', masked)]
    import bisect
    for rx in GT_PVP_TEST:
        for m in rx.finditer(masked):
            ln = bisect.bisect_left(nlpos, m.start()) + 1
            if ln not in live or (ln, 'GT_PVP') in seen:
                continue
            s = max(masked.rfind(';', 0, m.start()), masked.rfind('{', 0, m.start()), masked.rfind('}', 0, m.start()), m.start() - 600)
            e = [x for x in (masked.find(';', m.end()), masked.find('{', m.end())) if x >= 0]
            stmt = masked[s + 1: min(e) if e else m.end() + 200]
            if GT_PVP_SIBLING.search(stmt):
                continue
            seen.add((ln, 'GT_PVP'))
            add('b', ln, 'GT_PVP', GT_PVP_FIX)
    # c: calls to a replace-row function that is not replaced yet
    for (callee, via, qual, cls, fn, reason), (cnt, lines) in res['calls'].items():
        if callee in open_names and callee != fn:
            for ln in lines[:1]:
                add('c', ln, callee, 'calls `%s` (%d site%s in this function), whose replace row is not done: replace it first, or allow-list it with the D1 disposition'
                    % (callee, cnt, '' if cnt == 1 else 's'), func=('%s::%s' % (cls, fn)) if cls else fn)
    return hits, n_allow


def run_check(files, defs=None, open_names=None, quiet=False):
    defs = defs if defs is not None else jc.defined_flags()
    if open_names is None:
        open_names = set(n for r in open_rows(read_rows()) for n in callable_names(r))
    allow = load_allow()
    total, n_allow = 0, 0
    for f in files:
        path = f if os.path.isabs(f) or os.path.exists(f) else os.path.join(TRUNK, f)
        if not os.path.exists(path):
            print('MISSING %s' % f)
            total += 1
            continue
        hits, na = check_text_file(os.path.abspath(path), defs, open_names, allow)
        n_allow += na
        for (rule, ln, func, name, msg) in sorted(hits, key=lambda h: h[1]):
            print('HIT %s %s:%d [%s] %s: %s' % (rule, rel_of(path), ln, func, name, msg))
        total += len(hits)
    if not quiet:
        print('allow-listed: %d' % n_allow)
        print('RESULT: PASS' if not total else 'RESULT: FAIL, %d hit(s)' % total)
    return total


# ----------------------------------------------------------------------------------------------------------------------
# selftest
# ----------------------------------------------------------------------------------------------------------------------
FIXTURE_FLAG = 'SERV_IRUHADEV_JUNE_SELFTEST'
FIXTURE = '''#ifdef SERV_IRUHADEV_JUNE_SELFTEST
void CX2Fix::AlphaFrameMove()
{
	if( CX2Game::GT_PVP == g_pX2Game->GetGameType() ) { }
	if( CX2Game::GT_PVP == g_pX2Game->GetGameType() || CX2Game::GT_PVE == g_pX2Game->GetGameType() ) { }
	if( true == g_pX2Game->GetIsNowPVPCorrectionGame() ) { }
}
void CX2Fix::JuneBetaFrameMove() { int bJuneMemo = 0; }
#else
void CX2Fix::AlphaFrameMove()
{
	if( CX2Game::GT_PVP == g_pX2Game->GetGameType() ) { }
	int bJuneMarchLeftover = 0;
}
#endif SERV_IRUHADEV_JUNE_SELFTEST
#ifndef SERV_IRUHADEV_JUNE_SELFTEST
void CX2Fix::GammaFrameMove() { if( CX2Game::GT_PVP != g_pX2Game->GetGameType() ) { } }
#else
void CX2Fix::GammaFrameMove() { if( CX2Game::GT_PVP != g_pX2Game->GetGameType() ) { } }
#endif SERV_IRUHADEV_JUNE_SELFTEST
'''
PINNED = 'd4e8edc6:X2Lib/X2GUArme_VioletMage.cpp'


def selftest():
    fails = []
    os.makedirs(SCRATCH, exist_ok=True)
    defs = jc.defined_flags() | {FIXTURE_FLAG}
    fx = os.path.join(SCRATCH, 'fixture.cpp')
    with open(fx, 'wb') as f:
        f.write(FIXTURE.encode('ascii'))
    hits, _ = check_text_file(fx, defs, set(), [], rel='fixture.cpp')
    b_lines = sorted(h[1] for h in hits if h[0] == 'b')
    a_names = sorted(h[3] for h in hits if h[0] == 'a')
    # June branch: line 4 hits, 5 (GT_PVE beside it) and 6 do not; March #else (line 14) does not; #ifndef's #else (line 19) does
    if b_lines != [4, 19]:
        fails.append('fixture rule b lines %s, expected [4, 19]' % b_lines)
    if a_names != ['JuneBetaFrameMove', 'bJuneMemo']:
        fails.append('fixture rule a names %s, expected JuneBetaFrameMove + bJuneMemo (the #else bJuneMarchLeftover is March)' % a_names)
    hits2, na = check_text_file(fx, defs, set(), [('fixture.cpp', 'CX2Fix::AlphaFrameMove', 'GT_PVP', 'selftest')], rel='fixture.cpp')
    if [h[1] for h in hits2 if h[0] == 'b'] != [19] or na != 1:
        fails.append('allow-list row did not remove exactly the AlphaFrameMove hit (b=%s allowed=%d)' % ([h[1] for h in hits2 if h[0] == 'b'], na))
    # pinned revision
    try:
        blob = subprocess.check_output(['git', '-C', TRUNK, 'show', PINNED])
        pf = os.path.join(SCRATCH, 'pinned_VioletMage.cpp')
        with open(pf, 'wb') as f:
            f.write(blob)
        h3, _ = check_text_file(pf, jc.defined_flags(), set(), [], rel='X2Lib/X2GUArme_VioletMage.cpp')
        ra = sum(1 for h in h3 if h[0] == 'a')
        rb = sum(1 for h in h3 if h[0] == 'b')
        print('pinned %s: rule a %d hit(s), rule b %d hit(s)' % (PINNED, ra, rb))
        if not ra:
            fails.append('%s: the June-named rule no longer fires' % PINNED)
        if not rb:
            fails.append('%s: the GT_PVP idiom no longer fires' % PINNED)
    except (subprocess.CalledProcessError, OSError) as e:
        fails.append('cannot read %s: %s' % (PINNED, e))
    for f in fails:
        print('SELFTEST FAIL: ' + f)
    print('SELFTEST: PASS' if not fails else 'SELFTEST: FAIL, %d' % len(fails))
    return 1 if fails else 0


# ----------------------------------------------------------------------------------------------------------------------
def cmd_list():
    print('Names to avoid, and what to write instead (tools/junediverge.py --check enforces them in June code):')
    for a, b in AVOID:
        print('  %-52s -> %s' % (a, b))
    rows = read_rows()
    print('\nIdiom rows (verdict idiom): write the June test at the call site')
    for r in rows:
        if r['verdict'] == 'idiom':
            print('  %s %s: %s | March: %s' % (r['june_va'], r['name'], r['june_does'], r['march_does']))
    print('\nOpen replace rows (not done): replace March\'s definition first, or allow-list with the D1 disposition')
    for r in open_rows(rows):
        print('  %-40s %-22s %s' % (r['name'][:40], r['june_va'][:22], r['write_instead'][:90]))


def cmd_open_rows():
    rows = read_rows()
    fns = diverge_functions()
    allow = load_allow()
    n_done = n_open = 0
    for r in rows:
        if r['verdict'] != 'replace':
            continue
        if row_done(r, fns):
            n_done += 1
            continue
        n_open += 1
        names = callable_names(r)
        al = [a[3] for a in allow if a[2] in names]
        print('OPEN %-36s %-20s allow: %s | %s' % (r['name'][:36], r['june_va'][:20], '; '.join(al)[:100] if al else 'none', r['source'][-110:]))
    print('replace rows: %d done, %d open' % (n_done, n_open))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--check', nargs='+', metavar='FILE')
    ap.add_argument('--check-all', action='store_true')
    ap.add_argument('--list', action='store_true')
    ap.add_argument('--open-rows', action='store_true')
    ap.add_argument('--selftest', action='store_true')
    a = ap.parse_args()
    if a.selftest:
        return selftest()
    if a.list:
        cmd_list()
        return 0
    if a.open_rows:
        cmd_open_rows()
        return 0
    if a.check_all:
        files = [f for f in jc.source_files() if b'SERV_IRUHADEV_JUNE_' in open(f, 'rb').read()]
        return 1 if run_check(files) else 0
    if a.check:
        return 1 if run_check(a.check) else 0
    ap.print_help()
    return 2


if __name__ == '__main__':
    sys.exit(main())
