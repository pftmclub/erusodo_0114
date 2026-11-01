"""Check Chung's InitState / InitStateCommon / InitStateByUnitClass ( _LIVE branches ) against the per-class path simulation (char-chung CH1m).

    python june2017_re/char_chung/ch1m_regcheck.py      # run from Trunk

Eve's ch1m_regcheck.py with Chung's classes. Per class, every June registration of ch1m_reg_by_class.tsv (32c12a30 / 32c1d1d0 from
ch1m_regsim.py, 32c22540 from ch0_reg.tsv's listing rows) must match a C++ registration of the same table in the _LIVE branch, and every
June callback must resolve (funcmap.tsv, the vslot thunks, March's member for a later phase's `inside` row) to the member the C++ puts in
that slot. Hooks (bounding, wait state change), left-out states and the left-empty / later-phase slots are skipped and listed.
Each June id must map to one enumerator and each enumerator to one June id. Writes ch1m_idmap.tsv (June id -> enumerator).
"""
import collections, os, re, sys
sys.stdout.reconfigure(encoding='utf-8')
S = os.path.dirname(os.path.abspath(__file__))
SRC = open('X2Lib/X2GUChung.cpp', 'rb').read().decode('cp949', 'replace').split('\n')
CLS = {'UC_CHUNG_IRON_CANNON': '6', 'UC_CHUNG_FURY_GUARDIAN': '20', 'UC_CHUNG_SHOOTING_GUARDIAN': '21', 'UC_CHUNG_SHELLING_GUARDIAN': '27',
       'UC_CHUNG_IRON_PALADIN': '110', 'UC_CHUNG_DEADLY_CHASER': '111', 'UC_CHUNG_TACTICAL_TROOPER': '117'}
ALL = set(CLS.values())
SLOT = {'StateStart': 'Start', 'OnEventProcess': 'EventProcess', 'StateEnd': 'End', 'OnFrameMove': 'FrameMove', 'StateStartFuture': 'StartFuture',
        'StateEndFuture': 'EndFuture', 'OnFrameMoveFuture': 'FrameMoveFuture', 'StateInit': 'Init', 'OnCameraMove': 'CameraMove'}
j2m = collections.defaultdict(set)
for l in open('june2017_re/funcmap.tsv', 'rb').read().decode('utf-8', 'replace').split('\n')[1:]:
    p = l.split('\t')
    m = len(p) > 1 and re.match(r'(?:\w+::)*(\w+)', p[1].strip())
    if m and re.match(r'^[0-9a-f]{8}$', p[0]):
        j2m[p[0]].add(m.group(1))
        if m.group(1).startswith('June'):
            j2m[p[0]].add(m.group(1)[4:])     # merged by the june-march-divergence plan
j2m['32adf320'].add('HyperModeFrameMove'); j2m['329cc0e0'].add('HyperModeEnd'); j2m['32cf0fa0'].add('RidingHyperModeEnd')
for l in open(os.path.join(S, 'ch0_state_skill.tsv'), encoding='utf-8').read().split('\n')[2:]:
    p = l.split('\t')
    if len(p) > 9:
        cb = dict((x.split('=')[0], x.split('>')[-1]) for x in p[4].split(' ') if '=' in x)
        for x in p[8].split(' | '):
            m = re.match(r'(\w+)=inside X2GUChung\.cpp:\d+ CX2GU\w+::(\w+)$', x)
            if m and m.group(1) in cb:
                j2m[cb[m.group(1)]].add(m.group(2))


def body(name):
    s0 = next(i for i, l in enumerate(SRC) if re.match(r'^(/\*virtual\*/ )?void CX2GUChung::' + name + r'\(\)', l))
    e0 = next(i for i in range(s0 + 1, len(SRC)) if SRC[i].rstrip() == '}')
    src = [l.rstrip('\r') for l in SRC[s0:e0]]
    a = next(i for i, l in enumerate(src) if l.startswith('#ifdef SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE'))
    b = next(i for i in range(a, len(src)) if src[i].startswith('#else'))
    return src[a + 1:b]


def parse(name):
    regs = collections.defaultdict(list)
    hooks = []
    active, cases, cur = set(ALL), None, None
    for raw in body(name):
        l = raw.strip()
        if l.startswith('switch( eClass )'):
            cases, active = set(), set()
        elif cases is not None and re.match(r'case CX2Unit::\w+:', l):
            c = re.match(r'case CX2Unit::(\w+):', l).group(1)
            if c in CLS:
                cases.add(CLS[c]); active = active | {CLS[c]}
        elif cases is not None and l in ('} break;', 'break;'):
            active = set()
        elif cases is not None and l == 'default:':
            active = active | (ALL - cases - set(CLS[x] for x in CLS if 'case CX2Unit::%s:' % x in '\n'.join(body(name))))
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


HOOK = {'CSI_WAIT_STATE_CHANGE', 'CSI_DAMAGE_BOUNDING_DOWN', 'CSI_DAMAGE_BOUNDING_LANDCRASH', 'CSI_DAMAGE_BOUNDING_UP'}
LEFT = {'123', '124', '125', '126', '127', '128', '129', '93', '94', '95', '96', '335'}   # phoenix run, dance, riding Stage 17, Back Blast ( CH2a )
EMPTYOK = {'3202a7d0', '32002f70', '32bda6c0'}     # die end ( StateChange ), riding attack X frame move ( Stage 17 ), siege shelling Start ( CH2a )
ROLL = {('32c22540', '9'): 'strStartTableNameUTF8.c_str()', ('32c22540', '11'): 'strWinTableNameUTF8.c_str()',
        ('32c22540', '12'): 'strLoseTableNameUTF8.c_str()', ('32c12a30', '23'): 'strHyperModeTable.c_str()'}
idmap = collections.defaultdict(set); bad = 0; n = 0; skipped = collections.Counter(); allhooks = []
for cpp, sm in (('InitStateCommon', '32c12a30'), ('InitStateByUnitClass', '32c1d1d0'), ('InitState', '32c22540')):
    C, H = parse(cpp)
    J = sim(sm)
    allhooks += H
    for c in sorted(ALL, key=int):
        cr, used = C[c], set()
        for r in J[c]:
            if r['table'] in HOOK or r['id'] in LEFT:
                skipped[r['table'] if r['table'] in HOOK else r['id']] += 1
                continue
            want = ROLL.get((sm, r['id']), r['table'])
            k = next((k for k, x in enumerate(cr) if k not in used and x['table'] == want), None)
            if k is None:
                print('MISSING', c, r['id'], r['table']); bad += 1; continue
            used.add(k); x = cr[k]; n += 1
            idmap[r['id']].add(x['id'])
            for slot, t in r['cb'].items():
                if t in EMPTYOK:
                    if slot in x['cb']:
                        print('NOT EMPTY', c, r['table'], slot); bad += 1
                    continue
                if x['cb'].get(slot) not in j2m.get(t, set()):
                    print('DIFF', c, r['table'], slot, t, sorted(j2m.get(t, [])), x['cb'].get(slot)); bad += 1
            for slot in x['cb']:
                if slot not in r['cb']:
                    print('EXTRA SLOT', c, r['table'], slot); bad += 1
        for k, x in enumerate(cr):
            if k not in used:
                print('NOT IN JUNE', c, x['table']); bad += 1
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
    '\n'.join('%s\t%s' % (i, ' '.join(sorted(s))) for i, s in sorted(idmap.items(), key=lambda a: int(a[0]))) + '\n')
