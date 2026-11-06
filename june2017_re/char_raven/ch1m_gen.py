"""Generate Raven's June registrars as C++ (char-raven CH1m): InitStateCommon (32c5d660), the class part of InitState (32c66fe0)
and the three line registrars 32c511c0 / 32c51b70 / 32c63670.

    python june2017_re/char_raven/ch1m_gen.py <sim dir> <out dir>   # run from Trunk; <sim dir> holds sim_<fn>.tsv from ch1m_regsim.py

Elsword's ch1m_gen.py with Raven's names. Ids are March's enumerators by table name (March's own registration of the table, else the
RAVEN_STATE_ID member of that name, else the id another table of the same June id took); callbacks are the funcmap.tsv member names for the
June target, checked against the class headers, else CH0's `inside` member, else CH0's `pair` for a shared target. Hooks (bounding, hyper
table), left-out states and left-empty slots become one-line comments. Writes InitStateCommon.inc / InitStateClass.inc / InitStateLine_<fn>.inc
(CRLF, ASCII) and prints every name it could not resolve.
"""
import collections, os, re, sys
sys.stdout.reconfigure(encoding='utf-8')
CN = 'CX2GURaven'
CPP = open('X2Lib/X2GURaven.cpp', 'rb').read().decode('cp949', 'replace').split('\n')
HDR = {c: open(f, 'rb').read().decode('cp949', 'replace') for c, f in ((CN, 'X2Lib/X2GURaven.h'), ('CX2GUUser', 'X2Lib/X2GUUser.h'))}
CLSNAME = {4: 'UC_RAVEN_FIGHTER', 16: 'UC_RAVEN_SOUL_TAKER', 17: 'UC_RAVEN_OVER_TAKER', 25: 'UC_RAVEN_WEAPON_TAKER',
           106: 'UC_RAVEN_BLADE_MASTER', 107: 'UC_RAVEN_RECKLESS_FIST', 115: 'UC_RAVEN_VETERAN_COMMANDER'}
ORDER = [4, 16, 17, 25, 106, 107, 115]
FIELD = [('Init', 'StateInit'), ('StartFuture', 'StateStartFuture'), ('Start', 'StateStart'), ('FrameMoveFuture', 'OnFrameMoveFuture'),
         ('FrameMove', 'OnFrameMove'), ('CameraMove', 'OnCameraMove'), ('EventProcess', 'OnEventProcess'), ('EndFuture', 'StateEndFuture'),
         ('End', 'StateEnd')]
PAD = {f: f + '\t' * max(1, (28 - len('stateData.' + f) + 3) // 4) for _, f in FIELD}
LO, HI = 0x32c25a20, 0x32c6a63d
# vslot thunks June registers: their target is the class's override, or the user base
VSLOT = {'32adf320': (CN, 'HyperModeFrameMove'), '329cc0e0': ('CX2GUUser', 'HyperModeEnd'), '32cf0fa0': ('CX2GUUser', 'RidingHyperModeEnd')}
# June ids left out, with their owner ( Elsword's CH1m set; June's table names for 124 / 125 are copies of a neighbour's )
LEFT_OUT = {'123': 'PvP amusement mode, no PvP offline', '124': 'PvP amusement mode, no PvP offline', '125': 'PvP amusement mode, no PvP offline',
            '126': 'PvP amusement mode, no PvP offline', '127': 'dance state, Stage 10', '128': 'dance state, Stage 10', '129': 'dance state, Stage 10',
            '93': 'riding, Stage 17', '94': 'riding, Stage 17', '95': 'riding, Stage 17', '96': 'riding, Stage 17'}
EMPTY = {'3202a7d0': 'runs in CX2GUUser::StateChange for the die states', '32002f70': 'none in March, riding Stage 17'}
BOUNDING = {'RSI_DAMAGE_BOUNDING_DOWN', 'RSI_DAMAGE_BOUNDING_LANDCRASH', 'RSI_DAMAGE_BOUNDING_UP'}

fm = collections.defaultdict(list)
for l in open('june2017_re/funcmap.tsv', 'rb').read().decode('utf-8', 'replace').split('\n')[1:]:
    p = l.split('\t')
    if len(p) > 1 and re.match(r'^[0-9a-f]{8}$', p[0]):
        m = re.match(r'^(CX2GU(?:Raven|User))::(\w+)', p[1].strip())
        if m:
            fm[p[0]].append((m.group(1), m.group(2)))


def declared(cls, name):
    return re.search(r'\b' + name + r'\s*\(', HDR[cls]) is not None


shared = {}
inside = {}     # (table, slot) -> March's member CH0 paired with the June callback
for l in open('june2017_re/char_raven/ch0_state_skill.tsv', encoding='utf-8').read().split('\n')[2:]:
    p = l.split('\t')
    if len(p) > 9:
        for x in p[8].split(' | '):
            m = re.match(r'(\w+)=inside X2GURaven\.cpp:\d+ (CX2GU\w+)::(\w+)$', x)
            if m:
                inside[(p[0], m.group(1))] = (m.group(2), m.group(3))
        for x in re.findall(r'(\w+)=(CX2GUUser)::(\w+)@\d+', p[5]):
            if (p[0], x[0]) not in inside:
                shared[(p[0], x[0])] = (x[1], x[2])     # CH0 'pair': March's base callback in the slot of June's shared one

bad = []


def resolve(table, slot, target):
    if target in VSLOT:
        return VSLOT[target]
    cands = []
    for c, n in fm.get(target, []):
        if declared(c, n):
            cands.append((c, n))
        elif n.startswith('June') and declared(c, n[4:]):
            cands.append((c, n[4:]))
    cands = list(dict.fromkeys(cands))
    inrange = LO <= int(target, 16) < HI
    cands = [x for x in cands if (x[0] == CN) == inrange] or cands
    if not cands and (table, slot) in inside and declared(*inside[(table, slot)]):
        cands = [inside[(table, slot)]]
    if not cands and not inrange and (table, slot) in shared and declared(*shared[(table, slot)]):
        cands = [shared[(table, slot)]]
    if len(cands) != 1:
        bad.append('%s %s %s -> %s' % (table, slot, target, cands or 'NONE'))
        return None
    return cands[0]


# March's id for each table: its own registration, else the class enum member of that name
mid = collections.defaultdict(set)
cur = None
for l in CPP:
    m = re.match(r'\s*stateData\.stateID\s*=\s*(\w+(?:\(\))?);', l)
    if m:
        cur = m.group(1)
    m = re.match(r'\s*m_LuaManager\.MakeTableReference\(\s*"(\w+)",\s*stateData\.stateID', l)
    if m and cur:
        mid[m.group(1)].add(cur)

byjid = {}


def state_id(table, jid):
    if re.search(r'^\s*' + table + r'\b', HDR[CN], re.M):
        byjid.setdefault(jid, table)
        return table    # a table that is a RAVEN_STATE_ID member is its own state ( June 304 RWT_COMBO_ZZZX; March shared RSI_COMBO_ZZZX )
    s = mid.get(table)
    if not s and jid in byjid:
        return byjid[jid]
    if s and len(s) == 1:
        byjid.setdefault(jid, next(iter(s)))
        return next(iter(s))
    if s:
        bad.append('ID MULTI %s %s' % (table, sorted(s)))
        return sorted(s)[0]
    if re.search(r'^\s*' + table + r'\b', HDR[CN], re.M):
        byjid.setdefault(jid, table)
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
        if c:
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


def emit(regs, ind):
    out, bnd = [], False
    for r in regs:
        b = block(r, ind)
        if b is None:
            if not bnd:
                out += ['\t' * ind + '/// June %s @%s: the three DAMAGE_BOUNDING states ( checklist 7l )' % (r['fn'], r['line']),
                        '\t' * ind + 'InitBoundingStates( "RSI_" );', '']
                bnd = True
            continue
        out += b + ([''] if len(b) > 1 else [])
    while out and out[-1] == '':
        out.pop()
    return out


def key(regs):
    return [(r['id'], r['table'], r['cb']) for r in regs]


def uniform(by, fn):
    for c in ORDER:
        if key(by[c]) != key(by[ORDER[0]]):
            bad.append('%s differs by class %d' % (fn, c))
    return by[ORDER[0]]


def write(od, name, lines):
    open(os.path.join(od, name), 'wb').write(('\r\n'.join(lines) + '\r\n').encode('ascii'))


if __name__ == '__main__':
    sd, od = sys.argv[1], sys.argv[2]
    sim = lambda fn: rows(os.path.join(sd, 'sim_%s.tsv' % fn))
    # 32c5d660: no class test, no skill test
    common = uniform(sim('32c5d660'), '32c5d660')
    write(od, 'InitStateCommon.inc', emit(common, 1))
    # line registrars: no class test inside; InitState's switch picks the classes
    for fn in ('32c511c0', '32c51b70', '32c63670'):
        write(od, 'InitStateLine_%s.inc' % fn, emit(uniform(sim(fn), fn), 1))
    # 32c66fe0 after the line-registrar switch @315: five class tests, a case per class list
    byc = sim('32c66fe0')
    SECTIONS = [(344, 563, 'June 32c66fe0 @344: Reckless Fist / Veteran Commander / the others'),
                (564, 719, 'June 32c66fe0 @564: Blade Master / Veteran Commander / the others'),
                (720, 814, 'June 32c66fe0 @720: Weapon Taker and Veteran Commander / the others'),
                (815, 1085, 'June 32c66fe0 @815 switch'),
                (1086, 1120, 'June 32c66fe0 @1088: Soul Taker / Blade Master / Veteran Commander')]
    lines = []
    for a, b, label in SECTIONS:
        groups = collections.OrderedDict()
        part = {}
        for c in ORDER:
            part[c] = [r for r in byc[c] if a <= int(r['line']) <= b]
            k = tuple((r['id'], r['table'], tuple(sorted(r['cb'].items()))) for r in part[c])
            groups.setdefault(k, []).append(c)
        lines += ['\t/// ' + label, '\tswitch( eClass )', '\t{']
        for k, cs in groups.items():
            if not k:
                continue
            for c in cs:
                lines.append('\tcase CX2Unit::%s:' % CLSNAME[c])
            lines.append('\t\t{')
            lines += emit(part[cs[0]], 3)
            lines.append('\t\t} break;')
            lines.append('')
        lines += ['\tdefault:', '\t\tbreak;', '\t}', '']
    head = [r for r in byc[ORDER[0]] if int(r['line']) < 344]
    for c in ORDER:
        if [(r['id'], r['cb']) for r in byc[c] if int(r['line']) < 344] != [(r['id'], r['cb']) for r in head]:
            bad.append('32c66fe0 head differs by class %d' % c)
        if any(int(r['line']) > 1120 for r in byc[c]):
            bad.append('32c66fe0 registration after @1120 for class %d' % c)
    for r in head:
        print('HEAD', r['id'], r['table'], ' '.join('%s=%s:%s' % (s, t, '::'.join(resolve(r['table'], s, t) or ('?',))) for s, t in r['cb'].items()))
    write(od, 'InitStateClass.inc', lines)
    for b in bad:
        print('UNRESOLVED', b)
    print('common %d registrations, by class %s' % (len(common), dict((c, len(v)) for c, v in sorted(byc.items()))))
