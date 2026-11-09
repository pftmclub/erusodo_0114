"""Generate Rena's June registrars as C++ (char-rena CH1m): InitStateCommon (32ca9dc0) and the class part of InitState (32cadc90).

    python june2017_re/char_rena/ch1m_gen.py <sim dir> <out dir>   # run from Trunk; <sim dir> holds sim_<fn>.tsv from ch1m_regsim.py

Raven's ch1m_gen.py with Rena's names, and one change: Rena has no line registrars, so the class tests are found, not listed. Each
registrar's per-class sequences are aligned on the registrations every class makes alike (same line, id, table and callbacks); a run
between two such anchors where the classes differ becomes `switch( eClass )`, one case per group of classes with the same run.
Ids: the `June script id N` member CH1d-CH1i wrote (X2GULire_ElvenRanger.h), else March's own registration of the table, else the
enum member of that name. Callbacks: funcmap.tsv names checked against the class headers, else CH0's `inside` member, else CH0's
`pair` for a shared target; a callback March declares only under ADD_RENA_SYSTEM is written under the `_NF` flag (CH1q), with the
CH0 pair in the `#else`. Hooks (bounding, hyper table), left-out states and left-empty slots become one-line comments.
Writes InitStateCommon.inc / InitStateClass.inc (CRLF, ASCII) and prints every name it could not resolve.
"""
import collections, os, re, sys
sys.stdout.reconfigure(encoding='utf-8')
CN = 'CX2GULire_ElvenRanger'
NF = 'SERV_IRUHADEV_JUNE_CHAR_RENA_NF'
CPP = open('X2Lib/X2GULire_ElvenRanger.cpp', 'rb').read().decode('cp949', 'replace').split('\n')
HTXT = open('X2Lib/X2GULire_ElvenRanger.h', 'rb').read().decode('cp949', 'replace')
HDR = {CN: HTXT, 'CX2GUUser': open('X2Lib/X2GUUser.h', 'rb').read().decode('cp949', 'replace')}
CLSNAME = {3: 'UC_LIRE_ELVEN_RANGER', 12: 'UC_LIRE_COMBAT_RANGER', 13: 'UC_LIRE_SNIPING_RANGER', 24: 'UC_LIRE_TRAPPING_RANGER',
           102: 'UC_LIRE_WIND_SNEAKER', 103: 'UC_LIRE_GRAND_ARCHER', 114: 'UC_LIRE_NIGHT_WATCHER'}
ORDER = [3, 12, 13, 24, 102, 103, 114]
FIELD = [('Init', 'StateInit'), ('StartFuture', 'StateStartFuture'), ('Start', 'StateStart'), ('FrameMoveFuture', 'OnFrameMoveFuture'),
         ('FrameMove', 'OnFrameMove'), ('CameraMove', 'OnCameraMove'), ('EventProcess', 'OnEventProcess'), ('EndFuture', 'StateEndFuture'),
         ('End', 'StateEnd')]
PAD = {f: f + '\t' * max(1, (28 - len('stateData.' + f) + 3) // 4) for _, f in FIELD}
LO, HI = 0x32c6a8d0, 0x32cb6c6d
VSLOT = {'32adf320': (CN, 'HyperModeFrameMove'), '329cc0e0': ('CX2GUUser', 'HyperModeEnd'), '32cf0fa0': ('CX2GUUser', 'RidingHyperModeEnd')}
LEFT_OUT = {'123': 'PvP amusement mode, no PvP offline', '124': 'PvP amusement mode, no PvP offline', '125': 'PvP amusement mode, no PvP offline',
            '126': 'PvP amusement mode, no PvP offline', '127': 'dance state, Stage 10', '128': 'dance state, Stage 10', '129': 'dance state, Stage 10',
            '93': 'riding, Stage 17', '94': 'riding, Stage 17', '95': 'riding, Stage 17', '96': 'riding, Stage 17'}
EMPTY = {'3202a7d0': 'runs in CX2GUUser::StateChange for the die states', '32002f70': 'none in March, riding Stage 17'}
BOUNDING = {'LESI_DAMAGE_BOUNDING_DOWN', 'LESI_DAMAGE_BOUNDING_LANDCRASH', 'LESI_DAMAGE_BOUNDING_UP'}

fm = collections.defaultdict(list)
for l in open('june2017_re/funcmap.tsv', 'rb').read().decode('utf-8', 'replace').split('\n')[1:]:
    p = l.split('\t')
    if len(p) > 1 and re.match(r'^[0-9a-f]{8}$', p[0]):
        m = re.match(r'^(CX2GU(?:Lire_ElvenRanger|User))::(\w+)', p[1].strip())
        if m:
            fm[p[0]].append((m.group(1), m.group(2)))


def declared(cls, name):
    return re.search(r'\b' + name + r'\s*\(', HDR[cls]) is not None


def decl_count(name):
    """(declarations outside ADD_RENA_SYSTEM blocks, inside) in the class header"""
    out, inn, depth = 0, 0, []
    for l in HTXT.split('\n'):
        s = l.strip()
        m = re.match(r'#\s*(ifdef|ifndef|if)\b\s*(\w*)', s)
        if m:
            depth.append(m.group(1) == 'ifdef' and m.group(2) == 'ADD_RENA_SYSTEM')
            continue
        if re.match(r'#\s*else\b', s) and depth:
            depth[-1] = False
            continue
        if re.match(r'#\s*endif\b', s) and depth:
            depth.pop()
            continue
        if not s.startswith('//') and re.search(r'\b' + name + r'\s*\(', s):
            if any(depth):
                inn += 1
            else:
                out += 1
    return out, inn


shared = {}
inside = {}
for l in open('june2017_re/char_rena/ch0_state_skill.tsv', encoding='utf-8').read().split('\n')[2:]:
    p = l.split('\t')
    if len(p) > 8:
        cbs = re.findall(r'(\w+)=\S+>([0-9a-f]{8})', p[4])
        for x in p[8].split(' | '):
            m = re.match(r'(\w+)=inside X2GULire_ElvenRanger\.cpp:\d+ (CX2GU\w+)::(\w+)$', x)
            if m:
                for slot, tg in cbs:
                    if slot == m.group(1):
                        inside.setdefault((p[0], slot, tg), (m.group(2), m.group(3)))
        for x in re.findall(r'(\w+)=(CX2GUUser)::(\w+)@\d+', p[5]):
            shared.setdefault((p[0], x[0]), (x[1], x[2]))

bad = []


ADDRENA = {}    # June target -> the March member CH0 found only under ADD_RENA_SYSTEM ( CH1q's rows )
for l in open('june2017_re/char_rena/ch0_class_map.tsv', encoding='utf-8').read().split('\n')[1:]:
    p = l.split('\t')
    if len(p) > 3:
        m = re.match(r'inside \(ADD_RENA_SYSTEM\) X2GULire_ElvenRanger\.cpp:\d+ CX2GULire_ElvenRanger::(\w+)$', p[3].strip())
        if m:
            ADDRENA[p[0]] = m.group(1)


def resolve(table, slot, target):
    """-> (cls, name, pair); pair (flag-off callback for the #else, or ('', '') for none) only when name is ADD_RENA_SYSTEM-only"""
    if target in VSLOT:
        return VSLOT[target] + (None,)
    if target in ADDRENA:
        pair = None
        for (t, s, tg), v in inside.items():
            if t == table and s == slot and tg == target and v != (CN, ADDRENA[target]) and declared(*v):
                pair = v
        if pair is None and (table, slot) in shared and declared(*shared[(table, slot)]):
            pair = shared[(table, slot)]
        return (CN, ADDRENA[target], pair or ('', ''))
    cands = []
    for c, n in fm.get(target, []):
        if declared(c, n):
            cands.append((c, n))
        elif n.startswith('June') and declared(c, n[4:]):
            cands.append((c, n[4:]))
    cands = list(dict.fromkeys(cands))
    inrange = LO <= int(target, 16) < HI
    cands = [x for x in cands if (x[0] == CN) == inrange] or cands
    if not cands:
        for (t, s, tg), v in inside.items():
            if t == table and s == slot and tg == target and declared(*v):
                cands = [v]
    if not cands and not inrange and (table, slot) in shared and declared(*shared[(table, slot)]):
        cands = [shared[(table, slot)]]
    if len(cands) != 1:
        bad.append('%s %s %s -> %s' % (table, slot, target, cands or 'NONE'))
        return None
    c, n = cands[0]
    if c == CN and decl_count(n)[0] == 0:
        bad.append('ADD_RENA_SYSTEM-only declaration without a CH0 row: %s %s %s' % (table, slot, n))
    return (c, n, None)


# June id -> member written by CH1d-CH1i with a `June script id N` comment
byjune = {}
for l in HTXT.split('\n'):
    m = re.match(r'\s*(LESI_\w+),\s*/// June script id (\d+)\b', l)
    if m:
        byjune[m.group(2)] = m.group(1)
mid = collections.defaultdict(set)
cur = None
for l in CPP:
    m = re.match(r'\s*stateData\.stateID\s*=\s*(\w+(?:\(\))?);', l)
    if m:
        cur = m.group(1)
    m = re.match(r'\s*m_LuaManager\.MakeTableReference\(\s*"(\w+)",\s*stateData\.stateID', l)
    if m and cur:
        mid[m.group(1)].add(cur)
JUNE_TABLES = collections.defaultdict(set)   # table -> June ids it is registered under
EXTRA = {}   # June id -> member added by CH1m (filled by the spec; printed as UNRESOLVED otherwise)


IDMAP = collections.defaultdict(set)   # June id -> members written


def state_id(table, jid):
    m = state_id_(table, jid)
    IDMAP[jid].add(m)
    return m


def state_id_(table, jid):
    if jid in byjune:
        return byjune[jid]
    if jid in EXTRA:
        return EXTRA[jid]
    s = mid.get(table)
    if s and len(s) == 1:
        return next(iter(s))
    if s:
        other = set()
        for t2, s2 in mid.items():
            if t2 != table and len(s2) == 1 and jid in JUNE_TABLES.get(t2, ()):
                other |= s2
        if len(s & other) == 1:
            return next(iter(s & other))     # the member another table of the same June id resolves to
        bad.append('ID MULTI %s %s (June %s)' % (table, sorted(s), jid))
        return sorted(s)[0]
    if re.search(r'^\s*' + table + r'\b', HTXT, re.M) or re.search(r'^\s*' + table + r'\b', HDR['CX2GUUser'], re.M):
        return table
    bad.append('ID NONE %s (June %s)' % (table, jid))
    return table


def block(r, ind):
    t = '\t' * ind
    table, jid = r['table'], r['id']
    if jid in LEFT_OUT:
        return [t + '/// June %s @%s: %s ( id %s ), left out: %s' % (r['fn'], r['line'], table, jid, LEFT_OUT[jid])]
    if table in BOUNDING:
        return None
    out = [t + 'stateData.Init();']
    if table == 'puVar4':
        out += [t + 'stateData.stateID\t\t\t= USI_HYPER_MODE;', t + 'm_LuaManager.MakeTableReference( strHyperModeTable.c_str(), stateData.stateID );']
    else:
        out += [t + 'stateData.stateID\t\t\t= %s;' % state_id(table, jid), t + 'm_LuaManager.MakeTableReference( "%s", stateData.stateID );' % table]
    for slot, f in FIELD:
        if slot not in r['cb']:
            continue
        tg = r['cb'][slot]
        if tg in EMPTY:
            out.append(t + '/// June %s ( %s ): %s' % (tg, f, EMPTY[tg]))
            continue
        c = resolve(table, slot, tg)
        if not c:
            continue
        if c[2]:
            out += ['#ifdef ' + NF,
                    t + 'stateData.%s= SET_CB_FUNC( %s, %s );\t/// June %s: March has it only under ADD_RENA_SYSTEM' % (PAD[f], c[0], c[1], tg)]
            if c[2][0]:
                out += ['#else ' + NF, t + 'stateData.%s= SET_CB_FUNC( %s, %s );' % (PAD[f], c[2][0], c[2][1])]
            out += ['#endif ' + NF]
        else:
            out.append(t + 'stateData.%s= SET_CB_FUNC( %s, %s );' % (PAD[f], c[0], c[1]))
    out.append(t + 'm_StateList[stateData.stateID] = stateData;')
    return out


def rows(path):
    by = collections.defaultdict(list)
    for l in open(path, encoding='utf-8'):
        p = l.rstrip('\n').split('\t')
        if len(p) < 5 or not p[1].isdigit():
            continue
        by[int(p[1])].append({'fn': p[0], 'line': p[2], 'id': p[3], 'table': p[4],
                              'cb': dict((x.split('=')[0], x.split('>')[-1]) for x in (p[5] if len(p) > 5 else '').split(' ') if '=' in x)})
    return by


def emit(regs, ind, bnd):
    out = []
    for r in regs:
        b = block(r, ind)
        if b is None:
            if not bnd[0]:
                out += ['\t' * ind + '/// June %s @%s: the three DAMAGE_BOUNDING states ( checklist 7l )' % (r['fn'], r['line']),
                        '\t' * ind + 'InitBoundingStates( "LESI_" );', '']
                bnd[0] = True
            continue
        out += b + ([''] if len(b) > 1 else [])
    while out and out[-1] == '':
        out.pop()
    return out


def k(r):
    return (r['line'], r['id'], r['table'], tuple(sorted(r['cb'].items())))


CUTS = {'32ca9dc0': [532], '32cadc90': [306, 857, 1031, 1095, 1160, 1220, 1254, 1345, 1384]}   # decompile lines of the class tests


def aligned(byc, fn):
    """Common registrations (same key for every class, same relative order) as anchors; class runs between them."""
    seqs = {c: byc[c] for c in ORDER}
    common = set(k(r) for r in seqs[ORDER[0]])
    for c in ORDER[1:]:
        common &= set(k(r) for r in seqs[c])
    anchors = [k(r) for r in seqs[ORDER[0]] if k(r) in common]
    for c in ORDER:
        if [k(r) for r in seqs[c] if k(r) in common] != anchors:
            bad.append('%s anchor order differs for class %d' % (fn, c))
    pos = {c: 0 for c in ORDER}
    parts = []
    for a in anchors + [None]:
        run = {}
        for c in ORDER:
            s = seqs[c]
            j = pos[c]
            while j < len(s) and (a is None or k(s[j]) != a):
                j += 1
            run[c] = s[pos[c]:j]
            pos[c] = j + 1
        if any(run.values()):
            for c in ORDER:
                ls = [int(r['line']) for r in run[c]]
                if ls != sorted(ls):
                    bad.append('%s class %d run not in decompile order %s' % (fn, c, ls))
            bounds = [0] + [x for x in CUTS.get(fn, [])] + [10 ** 9]
            for lo, hi in zip(bounds, bounds[1:]):
                sub = dict((c, [r for r in run[c] if lo <= int(r['line']) < hi]) for c in ORDER)
                if not any(sub.values()):
                    continue
                if len(set(tuple(k(r) for r in sub[c]) for c in ORDER)) == 1:
                    parts.append(('common', sub[ORDER[0]]))
                else:
                    parts.append(('switch', sub))
        if a is not None:
            parts.append(('common', [r for r in seqs[ORDER[0]] if k(r) == a][:1]))
    return parts


def generate(byc, fn, ind):
    lines, bnd = [], [False]
    for kind, v in aligned(byc, fn):
        if kind == 'common':
            lines += emit(v, ind, bnd) + ['']
            continue
        groups = collections.OrderedDict()
        for c in ORDER:
            groups.setdefault(tuple(k(r) for r in v[c]), []).append(c)
        lo = min(int(r['line']) for c in ORDER for r in v[c])
        hi = max(int(r['line']) for c in ORDER for r in v[c])
        lines += ['\t' * ind + '/// June %s @%d-%d: by class' % (fn, lo, hi), '\t' * ind + 'switch( eClass )', '\t' * ind + '{']
        for key, cs in groups.items():
            if not key:
                continue
            for c in cs:
                lines.append('\t' * ind + 'case CX2Unit::%s:' % CLSNAME[c])
            lines.append('\t' * (ind + 1) + '{')
            lines += emit(v[cs[0]], ind + 2, bnd)
            lines.append('\t' * (ind + 1) + '} break;')
            lines.append('')
        lines += ['\t' * ind + 'default:', '\t' * (ind + 1) + 'break;', '\t' * ind + '}', '']
    while lines and lines[-1] == '':
        lines.pop()
    return lines


def write(od, name, lines):
    open(os.path.join(od, name), 'wb').write(('\r\n'.join(lines) + '\r\n').encode('ascii'))


if __name__ == '__main__':
    sd, od = sys.argv[1], sys.argv[2]
    if len(sys.argv) > 3:
        for x in sys.argv[3:]:
            j, n = x.split('=')
            EXTRA[j] = n
    sim = lambda fn: rows(os.path.join(sd, 'sim_%s.tsv' % fn))
    for fn in ('32ca9dc0', '32cadc90'):
        for c, rs_ in sim(fn).items():
            for r in rs_:
                JUNE_TABLES[r['table']].add(r['id'])
    write(od, 'InitStateCommon.inc', generate(sim('32ca9dc0'), '32ca9dc0', 1))
    byc = sim('32cadc90')
    head = [r for r in byc[ORDER[0]] if int(r['line']) <= 304]
    for c in ORDER:
        if [k(r) for r in byc[c] if int(r['line']) <= 304] != [k(r) for r in head]:
            bad.append('32cadc90 head differs by class %d' % c)
        byc[c] = [r for r in byc[c] if int(r['line']) > 304]
    for r in head:
        print('HEAD', r['line'], r['id'], r['table'], ' '.join('%s=%s:%s' % (s, t, '::'.join(x for x in (resolve(r['table'], s, t) or ('?',)) if isinstance(x, str))) for s, t in r['cb'].items()))
    write(od, 'InitStateClass.inc', generate(byc, '32cadc90', 1))
    rev = collections.defaultdict(set)
    for j, ms in IDMAP.items():
        if len(ms) > 1:
            bad.append('June id %s -> %s' % (j, sorted(ms)))
        for m in ms:
            rev[m].add(j)
    for m, js in rev.items():
        if len(js) > 1:
            bad.append('member %s <- June ids %s' % (m, sorted(js, key=int)))
    with open(os.path.join(od, 'idmap.tsv'), 'w', encoding='utf-8', newline='\n') as f:
        f.write('june_id\tmember\n')
        for j in sorted(IDMAP, key=int):
            f.write('%s\t%s\n' % (j, ' '.join(sorted(IDMAP[j]))))
    for b in dict.fromkeys(bad):
        print('UNRESOLVED', b)
