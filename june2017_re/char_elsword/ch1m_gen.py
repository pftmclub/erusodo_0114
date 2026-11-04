"""Generate Elsword's June registrars as C++ (char-elsword CH1m): InitStateCommon (32d1f8f0) and the class part of InitState (32d23ff0).

    python june2017_re/char_elsword/ch1m_gen.py <sim_32d1f8f0.tsv> <sim_32d1f8f0, 21216 learned.tsv> <sim_32d23ff0.tsv> <out dir>   # run from Trunk

Chung's ch1m_gen.py with Elsword's names. Input: ch1m_regsim.py's per-class output. Ids are March's enumerators by table name (March's own
registration of the table, else the class enum member of that name, else the id another table of the same June id took); callbacks are the
funcmap.tsv member names for the June target, checked against the class headers. Hooks (bounding, hyper table), left-out states and
left-empty slots become one-line comments. Writes InitStateCommon.inc / InitStateClass.inc (CRLF, ASCII) and prints every name it could not resolve.
"""
import collections, os, re, sys
sys.stdout.reconfigure(encoding='utf-8')
CN = 'CX2GUElsword_SwordMan'
CPP = open('X2Lib/X2GUElsword_SwordMan.cpp', 'rb').read().decode('cp949', 'replace').split('\n')
HDR = {c: open(f, 'rb').read().decode('cp949', 'replace') for c, f in ((CN, 'X2Lib/X2GUElsword_SwordMan.h'), ('CX2GUUser', 'X2Lib/X2GUUser.h'))}
CLSNAME = {1: 'UC_ELSWORD_SWORDMAN', 10: 'UC_ELSWORD_KNIGHT', 11: 'UC_ELSWORD_MAGIC_KNIGHT', 22: 'UC_ELSWORD_SHEATH_KNIGHT',
           100: 'UC_ELSWORD_LORD_KNIGHT', 101: 'UC_ELSWORD_RUNE_SLAYER', 112: 'UC_ELSWORD_INFINITY_SWORD'}
ORDER = [1, 10, 11, 22, 100, 101, 112]
FIELD = [('Init', 'StateInit'), ('StartFuture', 'StateStartFuture'), ('Start', 'StateStart'), ('FrameMoveFuture', 'OnFrameMoveFuture'),
         ('FrameMove', 'OnFrameMove'), ('CameraMove', 'OnCameraMove'), ('EventProcess', 'OnEventProcess'), ('EndFuture', 'StateEndFuture'),
         ('End', 'StateEnd')]
PAD = {f: f + '\t' * max(1, (28 - len('stateData.' + f) + 3) // 4) for _, f in FIELD}
LO, HI = 0x32cf2da0, 0x32d27640
# vslot thunks June registers: their target is the class's override, or the user base
VSLOT = {'32adf320': (CN, 'HyperModeFrameMove'), '329cc0e0': ('CX2GUUser', 'HyperModeEnd'), '32cf0fa0': ('CX2GUUser', 'RidingHyperModeEnd')}
# June ids left out, with their owner ( Chung's CH1m set; June's table names for 93 / 124 / 125 are copies of a neighbour's )
LEFT_OUT = {'123': 'PvP amusement mode, no PvP offline', '124': 'PvP amusement mode, no PvP offline', '125': 'PvP amusement mode, no PvP offline',
            '126': 'PvP amusement mode, no PvP offline', '127': 'dance state, Stage 10', '128': 'dance state, Stage 10', '129': 'dance state, Stage 10',
            '93': 'riding, Stage 17', '94': 'riding, Stage 17', '95': 'riding, Stage 17', '96': 'riding, Stage 17'}
# a slot left empty, with the reason
EMPTY = {'3202a7d0': 'runs in CX2GUUser::StateChange for the die states', '32002f70': 'none in March, riding Stage 17'}
BOUNDING = {'ESSI_DAMAGE_BOUNDING_DOWN', 'ESSI_DAMAGE_BOUNDING_LANDCRASH', 'ESSI_DAMAGE_BOUNDING_UP'}

fm = collections.defaultdict(list)
for l in open('june2017_re/funcmap.tsv', 'rb').read().decode('utf-8', 'replace').split('\n')[1:]:
    p = l.split('\t')
    if len(p) > 1 and re.match(r'^[0-9a-f]{8}$', p[0]):
        m = re.match(r'^(CX2GU(?:Elsword_SwordMan|User))::(\w+)', p[1].strip())
        if m:
            fm[p[0]].append((m.group(1), m.group(2)))


def declared(cls, name):
    return re.search(r'\b' + name + r'\s*\(', HDR[cls]) is not None


shared = {}
inside = {}     # (table, slot) -> March's member CH0 paired with the June callback
for l in open('june2017_re/char_elsword/ch0_state_skill.tsv', encoding='utf-8').read().split('\n')[2:]:
    p = l.split('\t')
    if len(p) > 9:
        for x in p[8].split(' | '):
            m = re.match(r'(\w+)=inside X2GUElsword_SwordMan\.cpp:\d+ (CX2GU\w+)::(\w+)$', x)
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
    if table == 'puVar3':
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
                        '\t' * ind + 'InitBoundingStates( "ESSI_" );', '']
                bnd = True
            continue
        out += b + ([''] if len(b) > 1 else [])
    while out and out[-1] == '':
        out.pop()
    return out


def same(a, b):
    return [(r['id'], r['table'], r['cb']) for r in a] == [(r['id'], r['table'], r['cb']) for r in b]


if __name__ == '__main__':
    # 32d1f8f0: no class test; one skill test at id 162
    common, learned = rows(sys.argv[1]), rows(sys.argv[2])
    for c in ORDER:
        if not same(common[c], common[ORDER[0]]) or not same(learned[c], learned[ORDER[0]]):
            bad.append('32d1f8f0 differs by class %d' % c)
    nl, ll = common[ORDER[0]], learned[ORDER[0]]
    k = next(i for i, (a, b) in enumerate(zip(nl, ll)) if a['table'] != b['table'])
    if not (same(nl[:k], ll[:k]) and same(nl[k + 1:], ll[k + 1:])):
        bad.append('32d1f8f0 learned path differs beyond one registration')
    body_common = emit(nl[:k], 1)
    body_common += ['', '\t/// June @%s: base skill level of 21216 below 1 registers the revenge attack, else the compact counter' % nl[k]['line'],
                    '\tif( GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::RNSI_A_EIS_COMPACT_COUNTER, true ) < 1 )', '\t{']
    body_common += emit([nl[k]], 2) + ['\t}', '\telse', '\t{'] + emit([ll[k]], 2) + ['\t}', '']
    body_common += emit(nl[k + 1:], 1)

    # 32d23ff0 after the START / WIN / LOSE roll and the wait-state change: five class tests, a case per class list
    byc = rows(sys.argv[3])
    SECTIONS = [(310, 459, 'June 32d23ff0 @310: Infinity Sword'), (460, 659, 'June 32d23ff0 @460: Sheath Knight and Infinity Sword'),
                (660, 823, 'June 32d23ff0 @660 switch ( 112 falls into 22 )'), (824, 940, 'June 32d23ff0 @824 switch'),
                (941, 1113, 'June 32d23ff0 @941: Lord Knight / Rune Slayer / Infinity Sword')]
    lines = []
    for a, b, label in SECTIONS:
        groups = collections.OrderedDict()
        part = {}
        for c in ORDER:
            part[c] = [r for r in byc[c] if a <= int(r['line']) <= b]
            key = tuple((r['id'], r['table'], tuple(sorted(r['cb'].items()))) for r in part[c])
            groups.setdefault(key, []).append(c)
        lines += ['\t/// ' + label, '\tswitch( eClass )', '\t{']
        for key, cs in groups.items():
            if not key:
                continue
            for c in cs:
                lines.append('\tcase CX2Unit::%s:' % CLSNAME[c])
            lines.append('\t\t{')
            lines += emit(part[cs[0]], 3)
            lines.append('\t\t} break;')
            lines.append('')
        lines += ['\tdefault:', '\t\tbreak;', '\t}', '']
    head = [r for r in byc[ORDER[0]] if int(r['line']) < 310]
    for c in ORDER:
        if [(r['id'], r['cb']) for r in byc[c] if int(r['line']) < 310] != [(r['id'], r['cb']) for r in head]:
            bad.append('32d23ff0 head differs by class %d' % c)
        if any(int(r['line']) > 1113 for r in byc[c]):
            bad.append('32d23ff0 registration after @1113 for class %d' % c)
    for r in head:
        print('HEAD', r['id'], r['table'], ' '.join('%s=%s:%s' % (s, t, '::'.join(resolve(r['table'], s, t) or ('?',))) for s, t in r['cb'].items()))
    od = sys.argv[4]
    open(os.path.join(od, 'InitStateCommon.inc'), 'wb').write(('\r\n'.join(body_common) + '\r\n').encode('ascii'))
    open(os.path.join(od, 'InitStateClass.inc'), 'wb').write(('\r\n'.join(lines) + '\r\n').encode('ascii'))
    for b in bad:
        print('UNRESOLVED', b)
    print('common %d registrations ( +1 learned ), by class %s' % (len(nl), dict((c, len(v)) for c, v in sorted(byc.items()))))
