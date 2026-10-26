"""Check Eve's June_InitStateCommon / June_InitState against the per-class path simulation (char-eve CH1m).

    python june2017_re/char_eve/ch1m_regcheck.py      # run from Trunk

Per class, every June registration of ch1m_reg_by_class.tsv must match a C++ registration of the same table, and every June
callback must resolve (header `/// June <va>` or funcmap.tsv) to the member the C++ puts in that slot. Hooks (bounding, wait
state change), the routed ids and the two left-empty slots are skipped. Writes ch1m_idmap.tsv (June id -> enumerator).
"""
import collections, os, re, sys
sys.stdout.reconfigure(encoding='utf-8')
S = os.path.dirname(os.path.abspath(__file__))
SRC = open('X2Lib/X2GuEve.cpp', 'rb').read().decode('cp949', 'replace').split('\n')
CLS = {'UC_EVE_NASOD': '5', 'UC_EVE_EXOTIC_GEAR': '18', 'UC_EVE_ARCHITECTURE': '19', 'UC_EVE_ELECTRA': '26',
       'UC_EVE_CODE_NEMESIS': '108', 'UC_EVE_CODE_EMPRESS': '109', 'UC_EVE_BATTLE_SERAPH': '116'}
ALL = set(CLS.values())
SLOT = {'StateStart': 'Start', 'OnEventProcess': 'EventProcess', 'StateEnd': 'End', 'OnFrameMove': 'FrameMove', 'StateStartFuture': 'StartFuture',
        'StateEndFuture': 'EndFuture', 'OnFrameMoveFuture': 'FrameMoveFuture', 'StateInit': 'Init', 'OnCameraMove': 'CameraMove'}
j2m = collections.defaultdict(set)
for f in ('X2Lib/X2GuEve.h', 'X2Lib/X2GUUser.h'):
    for l in open(f, 'rb').read().decode('cp949', 'replace').split('\n'):
        m = re.search(r'\b(\w+)\s*\([^;]*;\s*///\s*June ([0-9a-f]{8})', l)
        if m: j2m[m.group(2)].add(m.group(1))
for l in open('june2017_re/funcmap.tsv', 'rb').read().decode('utf-8', 'replace').split('\n')[1:]:
    p = l.split('\t')
    m = len(p) > 1 and re.match(r'(?:\w+::)*(\w+)', p[1].strip())
    if m and re.match(r'^[0-9a-f]{8}$', p[0]): j2m[p[0]].add(m.group(1))
j2m['32adf320'].add('June_HyperModeFrameMove'); j2m['329cc0e0'].add('JuneHyperModeEnd'); j2m['32cf0fa0'].add('JuneHyperModeEnd')
def parse(name):
    s0 = next(i for i, l in enumerate(SRC) if l.startswith('void CX2GUEve::' + name + '()'))
    e0 = next(i for i in range(s0 + 1, len(SRC)) if SRC[i].rstrip() == '}')
    src = [l.rstrip('\r') for l in SRC[s0:e0]]
    regs = collections.defaultdict(list)
    active, cases, cur = set(ALL), None, None
    for i, raw in enumerate(src):
        l = raw.strip()
        if l.startswith('switch( eClass )') or l.startswith('switch( iTempletClass )'):
            cases, active = set(), set()
        elif cases is not None and re.match(r'case CX2Unit::\w+:', l):
            c = re.match(r'case CX2Unit::(\w+):', l).group(1); cases.add(CLS[c]); active = active | {CLS[c]}
        elif cases is not None and l in ('} break;', 'break;'):
            active = set()
        elif cases is not None and l == 'default:':
            active = ALL - cases
        elif cases is not None and l == '}' and raw.startswith('\t}'):
            cases, active = None, set(ALL)
        elif l.startswith('stateData.Init()'):
            cur = {'id': None, 'table': None, 'cb': {}}
        elif re.match(r'stateData\.stateID\s*=', l):
            cur['id'] = re.match(r'stateData\.stateID\s*=\s*(\w+);', l).group(1)
        elif l.startswith('m_LuaManager.MakeTableReference('):
            cur['table'] = re.match(r'm_LuaManager\.MakeTableReference\(\s*(.+?),\s*stateData\.stateID', l).group(1).strip('" ')
        elif re.match(r'stateData\.\w+\s*=\s*SET_CB_FUNC', l):
            m = re.match(r'stateData\.(\w+)\s*=\s*SET_CB_FUNC\(\s*(\w+),\s*(\w+)\s*\);', l); cur['cb'][SLOT[m.group(1)]] = m.group(3)
        elif l.startswith('m_StateList[stateData.stateID] = stateData;'):
            for c in active: regs[c].append(cur)
    return regs
def sim(fn):
    out = collections.defaultdict(list)
    for l in open(os.path.join(S, 'ch1m_reg_by_class.tsv'), encoding='utf-8').read().split('\n')[1:]:
        p = l.split('\t')
        if len(p) < 5 or p[0] != fn: continue
        out[p[1]].append({'id': p[3], 'table': p[4], 'cb': dict((x.split('=')[0], x.split('>')[-1]) for x in (p[5] if len(p) > 5 else '').split(' ') if '=' in x)})
    return out
HOOK = {'ENSI_WAIT_STATE_CHANGE', 'ENSI_DAMAGE_BOUNDING_DOWN', 'ENSI_DAMAGE_BOUNDING_LANDCRASH', 'ENSI_DAMAGE_BOUNDING_UP'}
ROUTED = {'93', '94', '95', '96', '123', '124', '125', '126', '127', '128', '129', '224', '225', '226', '238', '111', '112', '113', '115', '116'}
EMPTYOK = {'3202a7d0', '32002f70'}
ROLL = {'9': 'tableNameStartUTF8.c_str()', '11': 'tableNameWinUTF8.c_str()', '12': 'tableNameLoseUTF8.c_str()', '23': 'strHyperModeTable.c_str()'}
idmap = collections.defaultdict(set); bad = 0; n = 0
for cpp, sm in (('June_InitStateCommon', '329b8030'), ('June_InitState', '329ba9c0')):
    C, J = parse(cpp), sim(sm)
    for c in sorted(ALL, key=int):
        cr, used = C[c], set()
        for r in J[c]:
            if r['table'] in HOOK or r['id'] in ROUTED: continue
            want = ROLL.get(r['id'], r['table'])
            k = next((k for k, x in enumerate(cr) if k not in used and x['table'] == want), None)
            if k is None: print('MISSING', c, r['id'], r['table']); bad += 1; continue
            used.add(k); x = cr[k]; n += 1
            idmap[r['id']].add(x['id'])
            for slot, t in r['cb'].items():
                if t in EMPTYOK:
                    if slot in x['cb']: print('NOT EMPTY', c, r['table'], slot); bad += 1
                    continue
                if x['cb'].get(slot) not in j2m.get(t, set()): print('DIFF', c, r['table'], slot, t, sorted(j2m.get(t, [])), x['cb'].get(slot)); bad += 1
            for slot in x['cb']:
                if slot not in r['cb']: print('EXTRA SLOT', c, r['table'], slot); bad += 1
        for k, x in enumerate(cr):
            if k not in used: print('NOT IN JUNE', c, x['table']); bad += 1
for i, s in sorted(idmap.items(), key=lambda a: int(a[0])):
    if len(s) != 1: print('ID MULTI', i, s); bad += 1
print('matched %d registrations, %d problems; ids mapped %d' % (n, bad, len(idmap)))
open(os.path.join(S, 'ch1m_idmap.tsv'), 'w').write('\n'.join('%s\t%s' % (i, ' '.join(sorted(s))) for i, s in sorted(idmap.items(), key=lambda a: int(a[0]))) + '\n')
