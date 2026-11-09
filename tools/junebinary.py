#!/usr/bin/env python
"""junebinary.py - coverage census of the June 2017 binary (plan june-census, Phase V1).

    python tools/junebinary.py              # build june2017_re/census/coverage.tsv + coverage_by_file.tsv, print the summary
    python tools/junebinary.py --check      # RESULT: PASS when the buckets add up to functions.tsv (and no duplicate / missing row)
    python tools/junebinary.py --sample 50  # print 50 random june_guarded rows (seed 7) for the hand check
    python tools/junebinary.py --summary    # print the summary from the written coverage.tsv
    python tools/junebinary.py --holdout    # precision of the `block` rule: hide each evidenced function, predict it from its neighbours

(Not `junecensus.py`: that name is the D0a identifier census of the june-march-divergence plan.)
Inputs: june2017_re/census/functions.tsv (V0), june2017_re/funcmap.tsv, X2Lib / KTDXLIB / X2 sources, JUNE_PORT_BACKLOG_ORDER.md,
the *_PHASES.md plan docs and kickoffs/. Read-only everywhere except the two output tsv files. Python 3, standard library only.

bucket   studio | library | crt-stl | unknown;  basis says why: funcmap | string | name | block | none
         `block`: no evidence of its own, but the nearest evidenced function on each side (by address) has the same bucket (and,
         for studio, the same class) within MAX_GAP functions. Never for crt-stl (hold-out precision 42%). It is an inference, reported separately from the others.
status   (studio only) mapped = funcmap.tsv has a row for it; march_has = no row but Class::Method resolves in the March tree; none
june_guarded  the March definition holds a SERV_IRUHADEV_JUNE_* block (cpp definitions only; header-inline bodies count as no: lower bound)
owner    stage / plan that names the address (owner_basis addr) or the class (owner_basis class)
"""
import bisect, collections, csv, os, random, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CEN = os.path.join(ROOT, 'june2017_re', 'census')
FUNCS = os.path.join(CEN, 'functions.tsv')
FMAP = os.path.join(ROOT, 'june2017_re', 'funcmap.tsv')
OUT = os.path.join(CEN, 'coverage.tsv')
OUT_FILE = os.path.join(CEN, 'coverage_by_file.tsv')
SRC_DIRS = ['X2Lib', 'KTDXLIB', 'X2']
MAX_GAP = 300

STUDIO_CLASS = re.compile(r'^(CX2|CKTD|KTDG|X2|KTR|KSk|KIocp|KIOThread|KSocketObject|KActorProxy|KSession|CX2ServerProtocol|KPerformer)')
LIB_PATH = re.compile(r'/libxml/|/kncsdk|/include/(dbg|kncsecurity)/', re.I)
ADDR = re.compile(r'\b(3[1-3][0-9a-f]{6})\b')


def read_text(p):
    with open(p, 'rb') as f:
        return f.read().decode('latin-1')


def load_funcs():
    rows = []
    with open(FUNCS, encoding='utf-8') as f:
        for line in f:
            if line.startswith('#') or line.startswith('june_va'):
                continue
            p = line.rstrip('\n').split('\t')
            p += [''] * (6 - len(p))
            rows.append(dict(va=p[0][2:].lower(), name=p[1], size=int(p[2] or 0), src=p[3], cls=p[4]))
    rows.sort(key=lambda r: int(r['va'], 16))
    return rows


def sym_class(sym):
    m = re.search(r'([A-Za-z_]\w*(?:::[A-Za-z_]\w*)*)::(~?[A-Za-z_]\w*)', sym)
    if not m:
        return '', ''
    chain = m.group(1)
    return chain.split('::')[0], m.group(2) if '::' not in chain else m.group(2)


def load_funcmap(funcs):
    starts = [int(r['va'], 16) for r in funcs]
    out = {}
    with open(FMAP, encoding='utf-8', errors='replace') as f:
        next(f)
        for line in f:
            p = line.rstrip('\n').split('\t')
            if len(p) < 3:
                continue
            toks = ADDR.findall(p[0].lower())
            for t in toks:
                a = int(t, 16)
                i = bisect.bisect_right(starts, a) - 1
                if i < 0:
                    continue
                va = funcs[i]['va']
                cur = out.get(va)
                if cur is None or ('::' not in cur[0] and '::' in p[1]):
                    out[va] = (p[1], p[2])
    return out


# ---------------------------------------------------------------- March tree index
def strip_line(s, st):
    """drop comments and string/char literals from one line; st = [in_block_comment]"""
    out, i, n = [], 0, len(s)
    while i < n:
        if st[0]:
            j = s.find('*/', i)
            if j < 0:
                return ''.join(out)
            st[0] = False
            i = j + 2
            continue
        c = s[i]
        if c == '/' and i + 1 < n and s[i + 1] == '/':
            break
        if c == '/' and i + 1 < n and s[i + 1] == '*':
            st[0] = True
            i += 2
            continue
        if c in '"\'':
            q = c
            i += 1
            while i < n and s[i] != q:
                i += 2 if s[i] == '\\' else 1
            i += 1
            continue
        out.append(c)
        i += 1
    return ''.join(out)


DEF_RE = re.compile(r'([A-Za-z_]\w*(?:::[A-Za-z_]\w*)*)::(~?[A-Za-z_]\w*)\s*\(')
CLASS_RE = re.compile(r'^\s*(?:class|struct)\s+(?:\w+\s+)?([A-Za-z_]\w*)\s*(?::[^;{]*)?\{?\s*$')


def index_tree():
    defs = {}        # (outer class, method) -> list of (file, line, guarded)
    hdr_ids = {}     # class -> set of identifiers of its header
    cls_file = {}    # class -> relative header path
    for d in SRC_DIRS:
        base = os.path.join(ROOT, d)
        for dp, _, fns in os.walk(base):
            for fn in fns:
                if not fn.lower().endswith(('.cpp', '.h', '.inl', '.hpp')):
                    continue
                p = os.path.join(dp, fn)
                rel = os.path.relpath(p, ROOT).replace('\\', '/')
                lines = read_text(p).split('\n')
                is_hdr = not fn.lower().endswith('.cpp')
                st = [False]
                stack = []
                depth = 0
                cur = None   # [outer, method, startline, guarded, depth_at_start]
                clean = []
                for ln, raw in enumerate(lines, 1):
                    s = raw.strip()
                    if s.startswith('#'):
                        m = re.match(r'#\s*(ifdef|ifndef|if|elif|else|endif)\b(.*)', s)
                        if m:
                            k, rest = m.group(1), m.group(2)
                            if k in ('if', 'ifdef', 'ifndef'):
                                stack.append(('!' if k == 'ifndef' else '') + rest.strip())
                            elif k in ('else', 'elif'):
                                if stack:
                                    top = stack.pop()
                                    stack.append(top[1:] if top.startswith('!') else '!' + top)
                            elif k == 'endif' and stack:
                                stack.pop()
                            if cur and 'SERV_IRUHADEV_JUNE_' in rest and k in ('if', 'ifdef', 'ifndef', 'elif'):
                                cur[3] = True
                        clean.append('')
                        continue
                    c = strip_line(raw, st)
                    clean.append(c)
                    if depth == 0 and not cur:
                        m = DEF_RE.search(c)
                        if m and not c.rstrip().endswith(';') and not re.match(r'\s*(return|if|else|for|while|switch|case|delete|new)\b', c):
                            g = any('SERV_IRUHADEV_JUNE_' in x and not x.startswith('!') for x in stack)
                            cur = [m.group(1).split('::')[0], m.group(2), ln, g, 0]
                        mc = CLASS_RE.match(c) if is_hdr else None
                        if mc:
                            cls_file.setdefault(mc.group(1), rel)
                    o, cl = c.count('{'), c.count('}')
                    depth += o - cl
                    if depth < 0:
                        depth = 0
                    if cur:
                        if depth == 0 and (o or cl):
                            defs.setdefault((cur[0], cur[1]), []).append((rel, cur[2], cur[3]))
                            cur = None
                        elif depth == 0 and c.rstrip().endswith(';'):
                            cur = None
                if is_hdr:
                    ids = set(re.findall(r'[A-Za-z_]\w+', ''.join(clean) if False else '\n'.join(clean)))
                    for c in [k for k, v in cls_file.items() if v == rel]:
                        hdr_ids[c] = ids
    return defs, hdr_ids, cls_file


# ---------------------------------------------------------------- backlog / plan owners
def load_owners():
    stage_text = {}
    bl = read_text(os.path.join(ROOT, 'JUNE_PORT_BACKLOG_ORDER.md'))
    parts = re.split(r'(?m)^###\s+Stage\s+(\d+)\b', bl)
    for i in range(1, len(parts) - 1, 2):
        stage_text['S' + parts[i]] = stage_text.get('S' + parts[i], '') + parts[i + 1]
    addr_stage = {}
    for k, t in stage_text.items():
        for a in ADDR.findall(t.lower()):
            addr_stage.setdefault(a, k)
    plan_docs = {}
    for fn in sorted(os.listdir(ROOT)):
        if fn.endswith('.md') and fn.startswith(('JUNE', 'LANOX', 'ATLAS', 'ELYSION')) and fn != 'JUNE_PORT_BACKLOG_ORDER.md' \
                and 'CENSUS' not in fn:
            plan_docs['P:' + fn[:-3]] = read_text(os.path.join(ROOT, fn))
    kd = os.path.join(ROOT, 'kickoffs')
    for dp, _, fns in os.walk(kd):
        for fn in fns:
            if fn.endswith('.md') and 'june-census' not in dp and 'june-backlog' not in dp:
                key = 'K:' + os.path.relpath(dp, kd).replace('\\', '/')
                plan_docs[key] = plan_docs.get(key, '') + read_text(os.path.join(dp, fn))
    addr_plan = {}
    for k, t in plan_docs.items():
        for a in ADDR.findall(t.lower()):
            addr_plan.setdefault(a, k)
    return stage_text, addr_stage, addr_plan


# ---------------------------------------------------------------- build
def lib_by_name(n):
    return bool(re.match(r'(xml|html|docb|lua|luaL|lj_|Scaleform|png|zlib|inflate|deflate|FT_|Cx[A-Z])', n))


def build():
    funcs = load_funcs()
    fm = load_funcmap(funcs)
    defs, hdr_ids, cls_file = index_tree()
    stage_text, addr_stage, addr_plan = load_owners()
    n = len(funcs)

    # 1. own evidence
    for r in funcs:
        r.update(bucket='', basis='', group='', area='', fm_sym='', fm_status='')
        sym = fm.get(r['va'])
        sf = r['src'].lower().replace('\\', '/')
        first_cls = r['cls'].split(';')[0] if r['cls'] else ''
        if sym:
            r['fm_sym'], r['fm_status'] = sym
            s = sym[0]
            if s.startswith(('std::', 'boost')):
                r['bucket'], r['basis'] = 'crt-stl', 'funcmap'
            else:
                r['bucket'], r['basis'] = 'studio', 'funcmap'
                r['group'] = sym_class(s)[0]
        if not r['bucket'] and sf:
            if LIB_PATH.search(sf):
                r['bucket'], r['basis'] = 'library', 'string'
            else:
                r['bucket'], r['basis'] = 'studio', 'string'
                r['area'] = next((a for a in ('x2lib', 'ktdxlib', 'x2serverprotocol', 'kncwx2server') if '/' + a + '/' in sf), 'x2')
        if not r['bucket'] and first_cls:
            if any(STUDIO_CLASS.match(c) for c in r['cls'].split(';')):
                r['bucket'], r['basis'] = 'studio', 'string'
            else:
                r['bucket'], r['basis'] = 'library', 'string'
        if r['bucket'] == 'studio' and not r['group']:
            r['group'] = next((c for c in r['cls'].split(';') if STUDIO_CLASS.match(c)), '')
            if not r['group'] and sf:
                r['group'] = os.path.splitext(os.path.basename(sf))[0]
        if not r['bucket'] and not r['name'].startswith(('FUN_', 'thunk_', '`', 'LAB_')):
            nm = r['name']
            if lib_by_name(nm):
                r['bucket'], r['basis'] = 'library', 'name'
            elif nm.startswith(('_', 'std::', 'operator', 'Ordinal', 'boost')):
                r['bucket'], r['basis'] = 'crt-stl', 'name'
    # 2. block inference between evidenced neighbours
    anchors = [i for i, r in enumerate(funcs) if r['bucket']]
    pos = 0
    for k in range(len(anchors) - 1):
        a, b = anchors[k], anchors[k + 1]
        if b - a <= 1:
            continue
        ra, rb = funcs[a], funcs[b]
        if ra['bucket'] == rb['bucket'] and ra['bucket'] != 'crt-stl' and b - a - 1 <= MAX_GAP:
            for i in range(a + 1, b):
                r = funcs[i]
                r['bucket'], r['basis'] = ra['bucket'], 'block'
                if ra['bucket'] == 'studio' and ra['group'] and ra['group'] == rb['group']:
                    r['group'] = ra['group']
    for r in funcs:
        if not r['bucket']:
            r['bucket'], r['basis'] = 'unknown', 'none'
    # 3. status for studio
    class_re = {}
    for r in funcs:
        r.update(status='', march_has='', guarded='', march_file='', owner='', owner_basis='')
        if r['bucket'] != 'studio':
            continue
        r['status'] = 'none'
        if r['fm_sym']:
            r['status'] = 'mapped'
        outer, meth = sym_class(r['fm_sym']) if r['fm_sym'] else ('', '')
        outer = outer or r['group']
        hit = defs.get((outer, meth)) if outer and meth else None
        if hit:
            r['march_has'] = 'yes'
            r['guarded'] = 'yes' if any(h[2] for h in hit) else 'no'
            r['march_file'] = hit[0][0]
            if r['status'] == 'none':
                r['status'] = 'march_has'
        elif outer and meth and meth in hdr_ids.get(outer, ()):
            r['march_has'] = 'yes'
            r['guarded'] = 'no'
            r['march_file'] = cls_file.get(outer, '')
            if r['status'] == 'none':
                r['status'] = 'march_has'
        else:
            r['march_has'] = 'no' if meth else ''
            r['march_file'] = cls_file.get(outer, '') if outer else ''
        # owner
        a = addr_stage.get(r['va']) or ''
        if a:
            r['owner'], r['owner_basis'] = a, 'addr'
        elif r['va'] in addr_plan:
            r['owner'], r['owner_basis'] = addr_plan[r['va']], 'addr'
        elif r['group']:
            g = r['group']
            if g not in class_re:
                rx = re.compile(r'\b' + re.escape(g) + r'\b')
                class_re[g] = ','.join(k for k, t in stage_text.items() if rx.search(t))[:60]
            if class_re[g]:
                r['owner'], r['owner_basis'] = class_re[g], 'class'
    return funcs


COLS = ['june_va', 'size', 'bucket', 'basis', 'group', 'status', 'fm_status', 'march_has', 'june_guarded', 'owner', 'owner_basis',
        'march_file', 'name', 'fm_symbol']


def write(funcs):
    with open(OUT, 'w', encoding='utf-8', newline='') as f:
        f.write('# June 2017 coverage census (junebinary.py, plan june-census V1); one row per function of functions.tsv\n')
        f.write('\t'.join(COLS) + '\n')
        for r in funcs:
            f.write('\t'.join([
                '0x' + r['va'], str(r['size']), r['bucket'], r['basis'], r['group'], r['status'], r['fm_status'].replace('\t', ' '),
                r['march_has'], r['guarded'], r['owner'], r['owner_basis'], r['march_file'], r['name'], r['fm_sym'][:120].replace('\t', ' ')]) + '\n')
    groups = collections.defaultdict(list)
    for r in funcs:
        if r['bucket'] == 'studio':
            groups[r['group'] or '(unattributed studio)'].append(r)
    rows = []
    for g, lst in groups.items():
        unowned = [r for r in lst if not r['owner']]
        top = sorted(unowned, key=lambda r: -r['size'])[:5]
        owners = collections.Counter(r['owner'] for r in lst if r['owner'])
        rows.append([g, len(lst), sum(r['status'] == 'mapped' for r in lst), sum(r['march_has'] == 'yes' for r in lst),
                     sum(r['guarded'] == 'yes' for r in lst), len(unowned), sum(r['size'] for r in unowned),
                     ','.join(k for k, _ in owners.most_common(3)),
                     (lst[0]['march_file'] or ''),
                     ' '.join('%s(%d)' % (r['va'], r['size']) for r in top)])
    rows.sort(key=lambda x: (-x[5], -x[6]))
    with open(OUT_FILE, 'w', encoding='utf-8', newline='') as f:
        f.write('# per class / file, studio functions only, sorted by unowned functions (junebinary.py, V1)\n')
        f.write('\t'.join(['group', 'functions', 'mapped', 'march_has', 'june_guarded', 'unowned', 'unowned_bytes', 'owner_stages',
                           'march_file', 'top5_unowned_va(size)']) + '\n')
        for x in rows:
            f.write('\t'.join(str(v) for v in x) + '\n')
    return rows


def read_cov():
    rows = []
    with open(OUT, encoding='utf-8') as f:
        for line in f:
            if line.startswith('#') or line.startswith('june_va'):
                continue
            p = line.rstrip('\n').split('\t')
            rows.append(dict(zip(COLS, p)))
    return rows


def summary(rows):
    tot = len(rows)
    print('total functions      %d' % tot)
    b = collections.Counter(r['bucket'] for r in rows)
    for k in ('studio', 'library', 'crt-stl', 'unknown'):
        print('  bucket %-8s %6d  %5.1f%%' % (k, b[k], 100.0 * b[k] / tot))
    bb = collections.Counter((r['bucket'], r['basis']) for r in rows)
    for k in sorted(bb):
        print('    %-8s %-8s %6d' % (k[0], k[1], bb[k]))
    st = [r for r in rows if r['bucket'] == 'studio']
    s = collections.Counter(r['status'] for r in st)
    print('studio statuses: ' + ', '.join('%s %d' % kv for kv in sorted(s.items())))
    print('  march_has (any status) %d, june_guarded %d' % (sum(r['march_has'] == 'yes' for r in st), sum(r['june_guarded'] == 'yes' for r in st)))
    ow = collections.Counter(r['owner_basis'] or 'none' for r in st)
    print('  owner basis: ' + ', '.join('%s %d' % kv for kv in sorted(ow.items())))
    bs = collections.Counter(r['basis'] for r in st)
    print('  studio with a class: %d (block %d)' % (sum(1 for r in st if r['group']), sum(1 for r in st if r['group'] and r['basis'] == 'block')))
    print('  studio by basis: ' + ', '.join('%s %d' % kv for kv in sorted(bs.items())))
    un = [r for r in st if not r['owner']]
    print('studio unowned: %d functions, %d bytes' % (len(un), sum(int(r['size']) for r in un)))


def check():
    funcs = load_funcs()
    cov = read_cov()
    ok = True
    vas = [r['june_va'][2:] for r in cov]
    if len(vas) != len(funcs):
        print('FAIL: coverage rows %d != functions.tsv rows %d' % (len(vas), len(funcs)))
        ok = False
    if len(set(vas)) != len(vas):
        print('FAIL: duplicate june_va in coverage.tsv')
        ok = False
    if set(vas) != {r['va'] for r in funcs}:
        print('FAIL: coverage.tsv and functions.tsv name different functions')
        ok = False
    b = collections.Counter(r['bucket'] for r in cov)
    if sum(b[k] for k in ('studio', 'library', 'crt-stl', 'unknown')) != len(funcs) or set(b) - {'studio', 'library', 'crt-stl', 'unknown'}:
        print('FAIL: buckets %s do not add up to %d' % (dict(b), len(funcs)))
        ok = False
    st = [r for r in cov if r['bucket'] == 'studio']
    if sum(1 for r in st if r['status'] not in ('mapped', 'march_has', 'none')):
        print('FAIL: a studio row has no status')
        ok = False
    with open(OUT_FILE, encoding='utf-8') as f:
        rows = [l.rstrip('\n').split('\t') for l in f if not l.startswith('#')][1:]
    if sum(int(x[1]) for x in rows) != len(st):
        print('FAIL: coverage_by_file.tsv functions %d != studio rows %d' % (sum(int(x[1]) for x in rows), len(st)))
        ok = False
    un = [int(x[5]) for x in rows]
    if un != sorted(un, reverse=True):
        print('FAIL: coverage_by_file.tsv not sorted by unowned')
        ok = False
    print('RESULT: %s (%d functions, buckets %s)' % ('PASS' if ok else 'FAIL', len(funcs), dict(b)))
    return 0 if ok else 1


def holdout():
    rows = read_cov()
    ev = [i for i, r in enumerate(rows) if r['basis'] not in ('block', 'none')]
    res = collections.Counter()
    gres = collections.Counter()
    for k in range(1, len(ev) - 1):
        a, i, b = ev[k - 1], ev[k], ev[k + 1]
        ra, r, rb = rows[a], rows[i], rows[b]
        if ra['bucket'] != rb['bucket'] or b - a - 1 > MAX_GAP:
            res[(r['bucket'], 'no prediction')] += 1
            continue
        res[(r['bucket'], 'right' if ra['bucket'] == r['bucket'] else 'wrong')] += 1
        if r['bucket'] == 'studio' and ra['bucket'] == 'studio' and ra['group'] and ra['group'] == rb['group']:
            gres['right' if r['group'] == ra['group'] else 'wrong'] += 1
    for b in ('studio', 'library', 'crt-stl'):
        right, wrong, none = res[(b, 'right')], res[(b, 'wrong')], res[(b, 'no prediction')]
        t = right + wrong
        print('%-8s evidenced %5d  predicted %5d  right %5d  wrong %4d  precision %5.1f%%  (no prediction %d)'
              % (b, t + none, t, right, wrong, 100.0 * right / t if t else 0, none))
    t = gres['right'] + gres['wrong']
    print('studio class by block: predicted %d right %d wrong %d precision %.1f%%' % (t, gres['right'], gres['wrong'], 100.0 * gres['right'] / t if t else 0))
    return 0


def main(a):
    if '--holdout' in a:
        return holdout()
    if '--check' in a:
        return check()
    if '--summary' in a:
        summary(read_cov())
        return 0
    if '--sample' in a:
        n = int(a[a.index('--sample') + 1])
        g = [r for r in read_cov() if r['june_guarded'] == 'yes']
        random.Random(7).shuffle(g)
        for r in g[:n]:
            print('\t'.join([r['june_va'], r['fm_symbol'], r['march_file']]))
        print('# %d guarded rows in all' % len(g))
        return 0
    funcs = build()
    write(funcs)
    summary(read_cov())
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
