"""Generate Chung's June registrars as C++ (char-chung CH1m): InitStateCommon (32c12a30) and InitStateByUnitClass (32c1d1d0).

    python june2017_re/char_chung/ch1m_gen.py <sim_32c12a30.tsv> <sim_32c1d1d0.tsv> <out dir> [<March X2GUChung.cpp>]      # run from Trunk

Input: ch1m_regsim.py's per-class output of both registrars. Ids are March's enumerators by table name (March's own registration of
the table, else the CHUNG_STATE_ID member of that name); callbacks are the member funcmap.tsv names for the June target, checked
against the class headers. Hooks (bounding, hyper table), left-out states and later-phase slots become one-line comments.
Writes InitStateCommon.inc / InitStateByUnitClass.inc (CRLF, ASCII) and prints every name it could not resolve.
"""
import collections, os, re, sys
sys.stdout.reconfigure(encoding='utf-8')
CPP = open(sys.argv[4] if len(sys.argv) > 4 else 'X2Lib/X2GUChung.cpp', 'rb').read().decode('cp949', 'replace').split('\n')
HDR = {c: open(f, 'rb').read().decode('cp949', 'replace') for c, f in (('CX2GUChung', 'X2Lib/X2GUChung.h'), ('CX2GUUser', 'X2Lib/X2GUUser.h'))}
CLSNAME = {6: 'UC_CHUNG_IRON_CANNON', 20: 'UC_CHUNG_FURY_GUARDIAN', 21: 'UC_CHUNG_SHOOTING_GUARDIAN', 27: 'UC_CHUNG_SHELLING_GUARDIAN',
           110: 'UC_CHUNG_IRON_PALADIN', 111: 'UC_CHUNG_DEADLY_CHASER', 117: 'UC_CHUNG_TACTICAL_TROOPER'}
FIELD = [('Init', 'StateInit'), ('StartFuture', 'StateStartFuture'), ('Start', 'StateStart'), ('FrameMoveFuture', 'OnFrameMoveFuture'),
         ('FrameMove', 'OnFrameMove'), ('CameraMove', 'OnCameraMove'), ('EventProcess', 'OnEventProcess'), ('EndFuture', 'StateEndFuture'),
         ('End', 'StateEnd')]
PAD = {f: f + '\t' * max(1, (28 - len('stateData.' + f) + 3) // 4) for _, f in FIELD}
LO, HI = 0x32bd86f0, 0x32c25a20
# vslot thunks June registers: their target is the class's override, or the user base
VSLOT = {'32adf320': ('CX2GUChung', 'HyperModeFrameMove'), '329cc0e0': ('CX2GUUser', 'HyperModeEnd'),
         '32cf0fa0': ('CX2GUUser', 'RidingHyperModeEnd')}
# states left out, with their owner ( CH1m kickoff, amended by CH1c )
LEFT_OUT = {'CSI_AMUSEMENT_ITEM_PHOENIX_RUN': 'PvP amusement mode, no PvP offline', 'CSI_AMUSEMENT_ITEM_PHOENIX_RUN_END': 'PvP amusement mode, no PvP offline', 'CSI_DANCE_Z': 'dance state, Stage 10',
            'CSI_DANCE_X': 'dance state, Stage 10', 'CSI_DANCE_WAIT': 'dance state, Stage 10', 'CSI_RIDING_SPECIFIC': 'riding, Stage 17',
            'CSI_RIDING_ATTACK_X_READY': 'riding, Stage 17', 'CSI_RIDING_ATTACK_X_READY_2': 'riding, Stage 17',
            'CSI_RIDING_ATTACK_X_END': 'riding, Stage 17', 'CSI_A_CC_BACK_BLAST': 'registered when skill 26200 is equipped, CH2a'}
# a slot left empty, with the reason
EMPTY = {'3202a7d0': 'runs in CX2GUUser::StateChange for the die states', '32002f70': 'none in March, riding Stage 17'}
BOUNDING = {'CSI_DAMAGE_BOUNDING_DOWN', 'CSI_DAMAGE_BOUNDING_LANDCRASH', 'CSI_DAMAGE_BOUNDING_UP'}

fm = collections.defaultdict(list)
for l in open('june2017_re/funcmap.tsv', 'rb').read().decode('utf-8', 'replace').split('\n')[1:]:
    p = l.split('\t')
    if len(p) > 1 and re.match(r'^[0-9a-f]{8}$', p[0]):
        m = re.match(r'^(CX2GU(?:Chung|User))::(\w+)', p[1].strip())
        if m:
            fm[p[0]].append((m.group(1), m.group(2)))


def declared(cls, name):
    return re.search(r'\b' + name + r'\s*\(', HDR[cls]) is not None


later = {}      # (table, slot) -> phase, for callbacks a later phase writes
inside = {}     # (table, slot) -> March's member CH0 paired with the June callback ( a later phase writes June's body into it )
for l in open('june2017_re/char_chung/ch0_state_skill.tsv', encoding='utf-8').read().split('\n')[2:]:
    p = l.split('\t')
    if len(p) > 9:
        for x in p[8].split(' | '):
            if x.endswith('=new') and p[7] in ('CH2a',):
                later[(p[0], x.split('=')[0])] = p[7]
            m = re.match(r'(\w+)=inside X2GUChung\.cpp:\d+ (CX2GU\w+)::(\w+)$', x)
            if m:
                inside[(p[0], m.group(1))] = (m.group(2), m.group(3))

bad = []


def resolve(table, slot, target):
    if target in VSLOT:
        c, n = VSLOT[target]
        return c, n
    cands = []
    for c, n in fm.get(target, []):
        if declared(c, n):
            cands.append((c, n))
        elif n.startswith('June') and declared(c, n[4:]):
            cands.append((c, n[4:]))
    cands = list(dict.fromkeys(cands))
    inrange = LO <= int(target, 16) < HI
    cands = [x for x in cands if (x[0] == 'CX2GUChung') == inrange] or cands
    if not cands and (table, slot) in inside and declared(*inside[(table, slot)]):
        cands = [inside[(table, slot)]]
    if len(cands) != 1:
        bad.append('%s %s %s -> %s' % (table, slot, target, cands or 'NONE'))
        return None
    return cands[0]


# March's id for each table: its own registration, else the CHUNG_STATE_ID member of that name
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
    if re.search(r'^\s*' + table + r'\b', HDR['CX2GUChung'], re.M):
        return table
    bad.append('ID NONE %s (June %s)' % (table, jid))
    return table


def block(r, ind):
    t = '\t' * ind
    table, jid = r['table'], r['id']
    if table in LEFT_OUT:
        return [t + '/// June %s @%s: %s ( id %s ), left out: %s' % (r['fn'], r['line'], table, jid, LEFT_OUT[table])]
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
        if (table, slot) in later:
            out.append(t + '/// June %s ( %s ): written by %s' % (tg, f, later[(table, slot)]))
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
                        '\t' * ind + 'InitBoundingStates( "CSI_" );', '']
                bnd = True
            continue
        out += b + ([''] if len(b) > 1 else [])
    while out and out[-1] == '':
        out.pop()
    return out


common = rows(sys.argv[1])
first = [common[c] for c in sorted(common)]
for c in common:
    if [(r['id'], r['table'], r['cb']) for r in common[c]] != [(r['id'], r['table'], r['cb']) for r in first[0]]:
        bad.append('32c12a30 differs by class %d' % c)
body_common = emit(first[0], 1)

# 32c1d1d0: June's two switches; a case per class, each written out whole
byc = rows(sys.argv[2])
split = {}
for c, regs in byc.items():
    k = next((i for i, r in enumerate(regs) if int(r['line']) > 1074), len(regs))
    split[c] = (regs[:k], regs[k:])
lines = []
for part, label in ((0, 'June 32c1d1d0 first switch ( unit class *( +0x867c ) + 0x38 )'), (1, 'June 32c1d1d0 @1075 second switch')):
    groups = collections.OrderedDict()
    for c in (20, 21, 27, 110, 111, 117, 6):
        key = tuple((r['id'], r['table'], tuple(sorted(r['cb'].items()))) for r in split[c][part])
        groups.setdefault(key, []).append(c)
    lines += ['\t/// ' + label, '\tswitch( eClass )', '\t{']
    for key, cs in groups.items():
        if not key:
            continue
        if 6 in cs:
            lines.append('\tdefault:')
        for c in cs:
            lines.append('\tcase CX2Unit::%s:' % CLSNAME[c])
        lines.append('\t\t{')
        lines += emit(split[cs[0]][part], 3)
        lines.append('\t\t} break;')
        lines.append('')
    if lines[-1] == '':
        lines.pop()
    if part == 1:
        lines += ['', '\tdefault:', '\t\tbreak;']
    lines += ['\t}', '']
od = sys.argv[3]
open(os.path.join(od, 'InitStateCommon.inc'), 'wb').write(('\r\n'.join(body_common) + '\r\n').encode('ascii'))
open(os.path.join(od, 'InitStateByUnitClass.inc'), 'wb').write(('\r\n'.join(lines) + '\r\n').encode('ascii'))
for b in bad:
    print('UNRESOLVED', b)
print('common %d registrations, by class %s' % (len(first[0]), dict((c, len(v)) for c, v in sorted(byc.items()))))
