"""Character-port key census: every table key a June class script uses, who reads it in
June's exe, and whether March's C++ reads it.  (CHARACTER_PORT_CHECKLIST section 5.)

    python june2017_re/charport_keys.py <ClassScript> [-o out.tsv]

<ClassScript> is a name such as Arme_VioletMage (resolved case-insensitively to
june_src_fixed/data036_kom/<name>.src.lua) or a path to a .lua file.

Steps
  1. Tokenise the June source (comments and strings stripped properly) and collect every
     `NAME =` / `["NAME"] =` table key (a key written inside a table constructor).  Top-level
     table names (`AVSI_START = {`, `INIT_SYSTEM = {`) are not keys; they are counted in the
     summary.  Context = `state` (any top-level table that is not INIT_*), `INIT_<name>`, or
     `damage-data` (the key sits somewhere under a DAMAGE_DATA* key).  Keys ending in digits
     also get a normalised form (`SOUND_PLAY0` -> `SOUND_PLAY%d`, `FOO_1` -> `FOO_%d`).
  2. June reader: exact + `%d` forms looked up in xorstrings.tsv (plaintext -> func_va, joined
     to funcmap.tsv for a name) and searched in unpacked_x2.exe as a plain NUL-terminated
     ASCII / UTF-16LE string.  june_src = xor | plain | both | none.  Readers are known only
     for xor strings; a plain-only hit has no reader (no xref search is done).
  3. March reader: every string literal in X2Lib/ and KTDXLIB/ *.cpp *.h (comments skipped,
     read as latin-1 bytes) is indexed; a key is a site when `"KEY"` / `L"KEY"` / the `%d`
     forms occur as a whole literal.  Sites inside a literal `#if 0` are marked (if0).
     SITES ARE GREP-BASED: other #ifdefs are not resolved; a port phase must confirm the
     branch with `cl /P`.  Short generic names (X, Y, NAME ...) match unrelated literals.
  4. damage-data keys are joined to census.tsv (root DE.DAMAGE_DATA) by (sub-path, key),
     falling back to key alone (shown with a leading `~`).
  5. verdict: both | june-only | march-only-site | neither (sorted in that order, then key).
"""
import bisect
import csv
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..'))
SRC_DIR = os.path.join(TRUNK, 'june_src_fixed', 'data036_kom')
JUNE_EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
MARCH_DIRS = ['X2Lib', 'KTDXLIB']
VERDICT_ORDER = ['june-only', 'neither', 'march-only-site', 'both']

# ---------------------------------------------------------------- Lua tokenising
TOK = re.compile(r'''
    (?P<lcom>--\[(?P<lc>=*)\[.*?\](?P=lc)\]) |
    (?P<com>--[^\n]*) |
    (?P<lstr>\[(?P<ls>=*)\[.*?\](?P=ls)\]) |
    (?P<str>"(?:[^"\\\n]|\\.)*"|'(?:[^'\\\n]|\\.)*') |
    (?P<id>[A-Za-z_][A-Za-z_0-9]*) |
    (?P<num>0[xX][0-9a-fA-F]+|\d+\.?\d*(?:[eE][+-]?\d+)?|\.\d+) |
    (?P<op>==|~=|<=|>=|\.\.\.|\.\.|[{}\[\]=,;.:()+\-*/%^#<>]) |
    (?P<ws>\s+)
''', re.X | re.S)


def lua_tokens(text):
    """-> list of (kind, text, line); comments/whitespace dropped, strings kept as 'str'."""
    out, line, pos = [], 1, 0
    while pos < len(text):
        m = TOK.match(text, pos)
        if not m:
            pos += 1
            continue
        k = m.lastgroup
        if k in ('lc', 'ls'):
            k = 'lcom' if m.group('lcom') else 'lstr'
        s = m.group(0)
        if k in ('id', 'num', 'op', 'str', 'lstr'):
            out.append(('str' if k == 'lstr' else k, s, line))
        line += s.count('\n')
        pos = m.end()
    return out


def collect_keys(toks):
    """-> (keys, tables).  keys: name -> dict(count, tables:set, first, ctx:set, subpaths:set)."""
    keys, tables = {}, []
    stack = []          # frames: name of the table opened ('' = anonymous)
    prev = ('op', ';', 0)
    n = len(toks)
    for i, (k, s, ln) in enumerate(toks):
        nxt = toks[i + 1] if i + 1 < n else ('', '', 0)
        if k == 'op' and s == '{':
            nm = ''
            if i >= 2 and toks[i - 1][1] == '=' and toks[i - 2][0] == 'id':
                nm = toks[i - 2][1]
            elif i >= 4 and toks[i - 1][1] == '=' and toks[i - 2][1] == ']' and toks[i - 3][0] == 'str':
                nm = toks[i - 3][1][1:-1]
            stack.append(nm)
            if len(stack) == 1:
                tables.append(nm)
        elif k == 'op' and s == '}':
            if stack:
                stack.pop()
        elif stack:
            key = None
            if k == 'id' and nxt[1] == '=' and prev[1] in ('{', ',', ';'):
                key = s
            elif (k == 'op' and s == '[' and nxt[0] == 'str' and i + 3 < n
                  and toks[i + 2][1] == ']' and toks[i + 3][1] == '=' and prev[1] in ('{', ',', ';')
                  and nxt[1][0] in '"\''):
                key = nxt[1][1:-1]
            if key is not None:
                top = stack[0]
                dd = [j for j, f in enumerate(stack) if f.startswith('DAMAGE_DATA')]
                if dd:
                    ctx = 'damage-data'
                    sub = '/'.join(f if f else '[]' for f in stack[dd[-1] + 1:]) or '(top)'
                else:
                    ctx = top if top.startswith('INIT_') else 'state'
                    sub = None
                e = keys.setdefault(key, dict(count=0, tables=set(), first=ln, ctx=set(), subpaths=set()))
                e['count'] += 1
                e['tables'].add(top or '?')
                e['ctx'].add(ctx)
                if sub is not None:
                    e['subpaths'].add(sub)
        prev = (k, s, ln)
    return keys, tables


def norm_forms(key):
    """-> (norm or '', [(form, kind)]) with kind 'x' (exact), 'n' (%d form) or 'b' (bare base).

    The last digit run of the key is the index: SOUND_PLAY0 -> SOUND_PLAY%d (and SOUND_PLAY_%d),
    FOO_1 -> FOO_%d (and FOO%d), SOUND_PLAY0_JP -> SOUND_PLAY%d_JP.  The bare base (`SOUND_PLAY`,
    for `BeginTable( "SOUND_PLAY", idx )`-style readers) is a weaker 'b' form."""
    forms = [(key, 'x')]
    ms = list(re.finditer(r'\d+', key))
    if not ms or ms[-1].start() == 0:
        return '', forms
    m = ms[-1]
    pre, post = key[:m.start()], key[m.end():]
    norm = pre + '%d' + post
    forms.append((norm, 'n'))
    if pre.endswith('_') and len(pre) > 1:
        forms.append((pre[:-1] + '%d' + post, 'n'))
    else:
        forms.append((pre + '_%d' + post, 'n'))
    bases = [pre + post, pre.rstrip('_') + post]
    if re.match(r'^_[A-Z]{2}$', post):          # nation suffix (SOUND_PLAY0_JP): the reader appends it
        bases.append(pre.rstrip('_'))
    for b in bases:
        if len(b) >= 3 and (b, 'b') not in forms:
            forms.append((b, 'b'))
    return norm, forms


# ---------------------------------------------------------------- June side
def load_xor():
    rd = {}
    p = os.path.join(HERE, 'xorstrings.tsv')
    with open(p, encoding='utf-8', errors='replace', newline='') as f:
        for r in csv.DictReader(f, delimiter='\t'):
            rd.setdefault(r['plaintext'], []).append(r['func_va'].lower())
    fm = {}
    with open(os.path.join(HERE, 'funcmap.tsv'), encoding='utf-8', errors='replace', newline='') as f:
        rows = csv.reader(f, delimiter='\t')
        next(rows, None)
        for r in rows:
            if len(r) >= 2:
                fm[r[0].lower()] = r[1]
    return rd, fm


def load_exe_strings():
    data = open(JUNE_EXE, 'rb').read()
    asc = set(m.group(0).decode('ascii') for m in re.finditer(rb'[\x20-\x7e]{2,}(?=\x00)', data))
    wide = set(m.group(0)[::2].decode('ascii') for m in re.finditer(rb'(?:[\x20-\x7e]\x00){2,}(?=\x00\x00)', data))
    return asc, wide


# ---------------------------------------------------------------- March side
C_TOK = re.compile(r'//[^\n]*|/\*.*?\*/|L?"((?:[^"\\\n]|\\.)*)"|\'(?:[^\'\\\n]|\\.)*\'', re.S)


def dead_ranges(lines):
    """line numbers (1-based) inside a literal `#if 0` (until its #else/#elif/#endif)."""
    dead, stack, pre = set(), [], re.compile(r'^\s*#\s*(if|ifdef|ifndef|else|elif|endif)\b(.*)')
    for ln, l in enumerate(lines, 1):
        m = pre.match(l)
        if m:
            d, rest = m.group(1), m.group(2).strip()
            if d in ('if', 'ifdef', 'ifndef'):
                stack.append([d == 'if' and re.match(r'^0\b', rest) is not None, False])
            elif d in ('else', 'elif') and stack:
                stack[-1][0] = False
            elif d == 'endif' and stack:
                stack.pop()
        elif any(s[0] for s in stack):
            dead.add(ln)
    return dead


def march_index():
    idx = {}
    for d in MARCH_DIRS:
        for root, _, files in os.walk(os.path.join(TRUNK, d)):
            for fn in files:
                if not fn.lower().endswith(('.cpp', '.h')):
                    continue
                p = os.path.join(root, fn)
                try:
                    t = open(p, 'rb').read().decode('latin-1')
                except OSError:
                    continue
                if '"' not in t:
                    continue
                nl = [m.start() for m in re.finditer('\n', t)]
                dead = None
                rel = os.path.relpath(p, TRUNK).replace('\\', '/')
                for m in C_TOK.finditer(t):
                    lit = m.group(1)
                    if lit is None or not lit or len(lit) > 80:
                        continue
                    ln = bisect.bisect_left(nl, m.start()) + 1
                    if dead is None:
                        dead = dead_ranges(t.split('\n'))
                    idx.setdefault(lit, []).append((rel, ln, ln in dead))
    return idx


# ---------------------------------------------------------------- census
def load_census():
    sp, by_key = {}, {}
    with open(os.path.join(HERE, 'census.tsv'), encoding='utf-8', errors='replace', newline='') as f:
        for r in csv.DictReader(f, delimiter='\t'):
            if r['root'] != 'DE.DAMAGE_DATA':
                continue
            st = r['status'] + (('/' + r['phase']) if r['phase'] else '')
            sp[(r['subpath'], r['key'])] = st
            by_key.setdefault(r['key'], set()).add(st)
    return sp, by_key


def census_status(key, e, sp, by_key):
    out = set()
    for s in e['subpaths']:
        v = sp.get((s, key))
        if v:
            out.add(v)
    if out:
        return '|'.join(sorted(out))
    if key in by_key:
        return '~' + '|'.join(sorted(by_key[key]))
    return 'not-in-census'


def cap(items, n=4):
    items = list(items)
    s = ', '.join(items[:n])
    return s + (' +%d more' % (len(items) - n) if len(items) > n else '')


# ---------------------------------------------------------------- main
def resolve(arg):
    if os.path.isfile(arg):
        return arg
    want = arg.lower()
    for suf in ('', '.src.lua', '.lua'):
        for fn in os.listdir(SRC_DIR):
            if fn.lower() == want + suf:
                return os.path.join(SRC_DIR, fn)
    sys.exit('cannot resolve %r (looked in %s)' % (arg, SRC_DIR))


def main(argv):
    out = None
    args = []
    i = 0
    while i < len(argv):
        if argv[i] == '-o':
            out = argv[i + 1]
            i += 2
        else:
            args.append(argv[i])
            i += 1
    if len(args) != 1:
        sys.exit(__doc__)
    path = resolve(args[0])
    text = open(path, encoding='utf-8-sig', errors='replace').read()
    keys, tables = collect_keys(lua_tokens(text))

    xor, fm = load_xor()
    asc, wide = load_exe_strings()
    marc = march_index()
    csp, cby = load_census()

    rows = []
    for key, e in keys.items():
        norm, forms = norm_forms(key)
        readers, xs, ps = [], set(), set()
        for f, kd in forms:
            if f in xor:
                xs.add(kd)
                if kd != 'b':
                    for va in xor[f]:
                        r = '%s:%s' % (va, fm.get(va, '?'))
                        if r not in readers:
                            readers.append(r)
                else:
                    for va in xor[f]:
                        r = 'base[%s]%s:%s' % (f, va, fm.get(va, '?'))
                        if r not in readers:
                            readers.append(r)
            if f in asc or f in wide:
                ps.add(kd)
        # a hit on the bare base only counts when nothing stronger hit; it is tagged (base)
        def strong(sset):
            return bool(sset - {'b'})
        xs_ok, ps_ok = bool(xs), bool(ps)
        jsrc = 'both' if xs_ok and ps_ok else 'xor' if xs_ok else 'plain' if ps_ok else 'none'
        if jsrc != 'none' and not strong(xs | ps):
            jsrc += '(base)'
        sites = []
        for f, kd in forms:
            for rel, ln, d in marc.get(f, ()):
                s_ = '%s%s:%d%s' % ('base:' if kd == 'b' else '', rel, ln, '(if0)' if d else '')
                if s_ not in sites:
                    sites.append(s_)
        live = [x for x in sites if not x.endswith('(if0)')]
        # a site in #if 0 does not count as a reader
        mn = len(live)
        if jsrc != 'none' and mn:
            v = 'both'
        elif jsrc != 'none':
            v = 'june-only'
        elif mn:
            v = 'march-only-site'
        else:
            v = 'neither'
        cs = ''
        if 'damage-data' in e['ctx']:
            cs = census_status(key, e, csp, cby)
        rows.append(dict(key=key, norm=norm, count=e['count'], tables=len(e['tables']), first=e['first'],
                         context=','.join(sorted(e['ctx'])), june_src=jsrc, june_readers=cap(readers) or 'none',
                         march_n=mn, march_sites=cap(sites) or 'none', census_status=cs, verdict=v))
    rows.sort(key=lambda r: (VERDICT_ORDER.index(r['verdict']), r['key']))

    cols = ['key', 'norm', 'count', 'tables', 'first_line', 'context', 'june_src', 'june_readers',
            'march_n', 'march_sites', 'census_status', 'verdict']
    keymap = dict(first_line='first')
    lines = ['\t'.join(cols)]
    for r in rows:
        lines.append('\t'.join(str(r[keymap.get(c, c)]) for c in cols))
    if out:
        os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
        with open(out, 'w', encoding='utf-8', newline='\n') as f:
            f.write('\n'.join(lines) + '\n')

    cnt = {}
    for r in rows:
        cnt[r['verdict']] = cnt.get(r['verdict'], 0) + 1
    print('script: %s' % os.path.relpath(path, TRUNK).replace('\\', '/'))
    print('top-level tables: %d (INIT_*: %d)   distinct keys: %d   key occurrences: %d' % (
        len(tables), sum(1 for t in tables if t.startswith('INIT_')), len(rows), sum(r['count'] for r in rows)))
    print('verdicts: ' + '  '.join('%s=%d' % (v, cnt.get(v, 0)) for v in VERDICT_ORDER))
    print('june-only keys (a port item):')
    for r in rows:
        if r['verdict'] == 'june-only':
            print('  %-40s x%-5d %-22s june=%s readers=%s %s' % (
                r['key'], r['count'], r['context'], r['june_src'], r['june_readers'],
                ('census=' + r['census_status']) if r['census_status'] else ''))
    print('neither (no reader found on either side):')
    for r in rows:
        if r['verdict'] == 'neither':
            print('  %-40s x%-5d %s' % (r['key'], r['count'], r['context']))
    print('note: March sites are grep-based literals; confirm the #ifdef branch with cl /P in the port phase.')
    if out:
        print('wrote %s' % out)


if __name__ == '__main__':
    main(sys.argv[1:])
