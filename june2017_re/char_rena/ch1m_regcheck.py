"""Check Rena's InitState / InitStateCommon ( _LIVE branches ) against the per-class path simulation (char-rena CH1m).

    python june2017_re/char_rena/ch1m_regcheck.py      # run from Trunk

Raven's ch1m_regcheck.py with Rena's classes and no line registrars. Per class, every June registration of ch1m_reg_by_class.tsv
(ch1m_regsim.py over 32ca9dc0 / 32cadc90) must match a C++ registration of the same table on that class's path, in the same order, and
every June callback must resolve (funcmap.tsv, the vslot thunks, CH0's ADD_RENA_SYSTEM member, March's member for an `inside` row,
March's base member for a shared `pair` row) to the member the C++ puts in that slot. The C++ is read as shipped: `_NF` on.
Hooks (bounding, wait state change), left-out states and the left-empty slots are skipped and listed. Each June id must map to one
enumerator and each enumerator to one June id. Writes ch1m_idmap.tsv (June id -> enumerator).
"""
import collections, os, re, sys
sys.stdout.reconfigure(encoding='utf-8')
S = os.path.dirname(os.path.abspath(__file__))
SRC = open('X2Lib/X2GULire_ElvenRanger.cpp', 'rb').read().decode('cp949', 'replace').split('\n')
CLS = {'UC_LIRE_ELVEN_RANGER': '3', 'UC_LIRE_COMBAT_RANGER': '12', 'UC_LIRE_SNIPING_RANGER': '13', 'UC_LIRE_TRAPPING_RANGER': '24',
       'UC_LIRE_WIND_SNEAKER': '102', 'UC_LIRE_GRAND_ARCHER': '103', 'UC_LIRE_NIGHT_WATCHER': '114'}
ALL = set(CLS.values())
NF = 'SERV_IRUHADEV_JUNE_CHAR_RENA_NF'
SLOT = {'StateStart': 'Start', 'OnEventProcess': 'EventProcess', 'StateEnd': 'End', 'OnFrameMove': 'FrameMove', 'StateStartFuture': 'StartFuture',
        'StateEndFuture': 'EndFuture', 'OnFrameMoveFuture': 'FrameMoveFuture', 'StateInit': 'Init', 'OnCameraMove': 'CameraMove'}
j2m = collections.defaultdict(set)
for l in open('june2017_re/funcmap.tsv', 'rb').read().decode('utf-8', 'replace').split('\n')[1:]:
    p = l.split('\t')
    m = len(p) > 1 and re.match(r'(?:\w+::)*(\w+)', p[1].strip())
    if m and re.match(r'^[0-9a-f]{8}$', p[0]):
        j2m[p[0]].add(m.group(1))
        if m.group(1).startswith('June'):
            j2m[p[0]].add(m.group(1)[4:])
j2m['32adf320'].add('HyperModeFrameMove'); j2m['329cc0e0'].add('HyperModeEnd'); j2m['32cf0fa0'].add('RidingHyperModeEnd')
for l in open(os.path.join(S, 'ch0_class_map.tsv'), encoding='utf-8').read().split('\n')[1:]:
    p = l.split('\t')
    if len(p) > 3:
        m = re.match(r'inside \(ADD_RENA_SYSTEM\) X2GULire_ElvenRanger\.cpp:\d+ CX2GULire_ElvenRanger::(\w+)$', p[3].strip())
        if m:
            j2m[p[0]].add(m.group(1))
for l in open(os.path.join(S, 'ch0_state_skill.tsv'), encoding='utf-8').read().split('\n')[2:]:
    p = l.split('\t')
    if len(p) > 8:
        cbs = re.findall(r'(\w+)=\S+>([0-9a-f]{8})', p[4])
        for x in p[8].split(' | '):
            m = re.match(r'(\w+)=inside X2GULire_ElvenRanger\.cpp:\d+ CX2GU\w+::(\w+)$', x)
            if m:
                for slot, tg in cbs:
                    if slot == m.group(1):
                        j2m[tg].add(m.group(2))
        for slot, fn in re.findall(r'(\w+)=CX2GUUser::(\w+)@\d+', p[5]):
            for s2, tg in cbs:
                if s2 == slot and not (0x32c6a8d0 <= int(tg, 16) < 0x32cb6c6d):
                    j2m[tg].add(fn)


def body(name):
    s0 = next(i for i, l in enumerate(SRC) if re.match(r'^(/\*virtual\*/ )?void CX2GULire_ElvenRanger::' + name + r'\(\)', l))
    e0 = next(i for i in range(s0 + 1, len(SRC)) if SRC[i].rstrip('\r') == '}')
    src = [l.rstrip('\r') for l in SRC[s0:e0]]
    a = next(i for i, l in enumerate(src) if l.startswith('#ifdef SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE'))
    b = next(i for i in range(a, len(src)) if src[i].startswith('#else SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE'))
    return src[a + 1:b]


def parse(name):
    regs = collections.defaultdict(list)
    hooks = []
    active, cases, cur, skip = set(ALL), None, None, False
    for raw in body(name):
        l = raw.strip()
        if l == '#ifdef ' + NF:
            skip = False
            continue
        if l == '#else ' + NF:
            skip = True
            continue
        if l == '#endif ' + NF:
            skip = False
            continue
        if skip:
            continue
        if l.startswith('switch( eClass )'):
            cases, active = set(), set()
        elif cases is not None and re.match(r'case CX2Unit::\w+:', l):
            c = re.match(r'case CX2Unit::(\w+):', l).group(1)
            cases.add(CLS[c]); active = active | {CLS[c]}
        elif cases is not None and l in ('} break;', 'break;'):
            active = set()
        elif cases is not None and l == 'default:':
            active = set()
        elif cases is not None and l == '}' and raw.startswith('\t}'):
            cases, active = None, set(ALL)
        elif l.startswith('stateData.Init()'):
            cur = {'id': None, 'table': None, 'cb': {}}
        elif re.match(r'stateData\.stateID\s*=', l):
            cur['id'] = re.match(r'stateData\.stateID\s*=\s*(\w+(?:\(\))?);', l).group(1)
        elif l.startswith('m_LuaManager.MakeTableReference('):
            cur['table'] = re.match(r'm_LuaManager\.MakeTableReference\(\s*(.+?),\s*stateData\.stateID', l).group(1).strip('" ')
        elif re.match(r'stateData\.\w+\s*=\s*SET_CB_FUNC', l):
            m = re.match(r'stateData\.(\w+)\s*=\s*SET_CB_FUNC\(\s*(\w+),\s*(\w+)\s*\);', l)
            cur['cb'][SLOT[m.group(1)]] = m.group(3)
        elif l.startswith('m_StateList[stateData.stateID] = stateData;'):
            for c in active:
                regs[c].append(cur)
        elif re.match(r'Init(Bounding|WaitStateChange)\w*\(', l):
            hooks.append(l)
    return regs, hooks


def sim(fn):
    out = collections.defaultdict(list)
    for l in open(os.path.join(S, 'ch1m_reg_by_class.tsv'), encoding='utf-8').read().split('\n')[1:]:
        p = l.split('\t')
        if len(p) < 5 or p[0] != fn:
            continue
        out[p[1]].append({'id': p[3], 'table': p[4], 'cb': dict((x.split('=')[0], x.split('>')[-1]) for x in (p[5] if len(p) > 5 else '').split(' ') if '=' in x)})
    return out


HOOK = {'LESI_WAIT_STATE_CHANGE', 'LESI_DAMAGE_BOUNDING_DOWN', 'LESI_DAMAGE_BOUNDING_LANDCRASH', 'LESI_DAMAGE_BOUNDING_UP'}
LEFT = {'123', '124', '125', '126', '127', '128', '129', '93', '94', '95', '96'}   # phoenix run, dance, riding Stage 17
EMPTYOK = {'3202a7d0', '32002f70'}     # die end ( StateChange ), riding attack X frame move ( Stage 17 )
ROLL = {('32cadc90', '9'): 'strStartTableNameUTF8.c_str()', ('32cadc90', '11'): 'strWinTableNameUTF8.c_str()',
        ('32cadc90', '12'): 'strLoseTableNameUTF8.c_str()', ('32ca9dc0', '23'): 'strHyperModeTable.c_str()'}
idmap = collections.defaultdict(set); bad = 0; n = 0; skipped = collections.Counter(); allhooks = []
for cpp, sm in (('InitStateCommon', '32ca9dc0'), ('InitState', '32cadc90')):
    C, H = parse(cpp)
    J = sim(sm)
    allhooks += H
    for c in sorted(ALL, key=int):
        cr, used, last = C[c], set(), -1
        if not J[c]:
            print('NO SIM ROWS', sm, c); bad += 1
        for r in J[c]:
            if r['table'] in HOOK or r['id'] in LEFT:
                skipped[r['table'] if r['table'] in HOOK else r['id']] += 1
                continue
            want = ROLL.get((sm, r['id']), r['table'])
            k = next((k for k, x in enumerate(cr) if k not in used and x['table'] == want and k > last), None)
            if k is None:
                print('MISSING', sm, c, r['id'], r['table']); bad += 1; continue
            last = k
            used.add(k); x = cr[k]; n += 1
            idmap[r['id']].add(x['id'])
            for slot, t in r['cb'].items():
                if t in EMPTYOK:
                    if slot in x['cb']:
                        print('NOT EMPTY', c, r['table'], slot); bad += 1
                    continue
                if x['cb'].get(slot) not in j2m.get(t, set()):
                    print('DIFF', sm, c, r['table'], slot, t, sorted(j2m.get(t, [])), x['cb'].get(slot)); bad += 1
            for slot in x['cb']:
                if slot not in r['cb']:
                    print('EXTRA SLOT', sm, c, r['table'], slot); bad += 1
        for k, x in enumerate(cr):
            if k not in used:
                print('NOT IN JUNE', sm, c, x['table']); bad += 1
rev = collections.defaultdict(set)
for i, s in idmap.items():
    for e in s:
        rev[e].add(i)
    if len(s) != 1:
        print('ID MULTI', i, s); bad += 1
for e, s in rev.items():
    if len(s) != 1:
        print('ENUM MULTI', e, sorted(s)); bad += 1
print('hooks:', sorted(set(allhooks)))
print('skipped (hook / left out, per class):', dict(skipped))
print('matched %d registrations, %d problems; ids mapped %d' % (n, bad, len(idmap)))
open(os.path.join(S, 'ch1m_idmap.tsv'), 'w', encoding='utf-8', newline='\n').write(
    'june_id\tenumerator\n' + '\n'.join('%s\t%s' % (i, ' '.join(sorted(s))) for i, s in sorted(idmap.items(), key=lambda a: int(a[0]))) + '\n')
