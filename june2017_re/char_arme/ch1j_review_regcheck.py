"""Check Aisha's June_InitState / June_InitStateCommon against the per-class path simulation (pilot-char-revamp CH1j review).

    python june2017_re/char_arme/ch1j_review_regcheck.py

Parses the two C++ registrars (switch( eClass ) blocks), and for each unit class matches every June registration of
ch1j_reg_by_class.tsv to a C++ registration with the same table (order-free; ids are distinct within a class). Each June
callback must resolve, through funcmap.tsv or a header `/// June <va>` comment, to the member the C++ registers in that
slot. Prints MISSING / DIFF / EXTRA / NOT IN C++ lines and the June id -> enumerator map; known routed gaps print too.
"""
import collections, os, re, sys
sys.stdout.reconfigure(encoding='utf-8')
T = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
CLS = {'UC_ARME_VIOLET_MAGE': '2', 'UC_ARME_HIGH_MAGICIAN': '14', 'UC_ARME_DARK_MAGICIAN': '15', 'UC_ARME_BATTLE_MAGICIAN': '23',
       'UC_ARME_ELEMENTAL_MASTER': '104', 'UC_ARME_VOID_PRINCESS': '105', 'UC_ARME_DIMENSION_WITCH': '113'}
ALL = set(CLS.values())
SLOT = {'StateStart': 'Start', 'OnEventProcess': 'EventProcess', 'StateEnd': 'End', 'OnFrameMove': 'FrameMove', 'StateStartFuture': 'StartFuture',
        'StateEndFuture': 'EndFuture', 'OnFrameMoveFuture': 'FrameMoveFuture', 'StateInit': 'Init', 'OnCameraMove': 'CameraMove'}
HOOKED = ('AVSI_WAIT_STATE_CHANGE', 'AVSI_DAMAGE_BOUNDING_UP', 'AVSI_DAMAGE_BOUNDING_DOWN', 'AVSI_DAMAGE_BOUNDING_LANDCRASH')

src = open(os.path.join(T, 'X2Lib/X2GUArme_VioletMage.cpp'), 'rb').read().decode('cp949', 'replace').split('\n')

j2m = collections.defaultdict(set)
for f in ('X2Lib/X2GUArme_VioletMage.h', 'X2Lib/X2GUUser.h'):
    for l in open(os.path.join(T, f), 'rb').read().decode('cp949', 'replace').split('\n'):
        m = re.search(r'\b(\w+)\s*\([^;]*;\s*///\s*June ([0-9a-f]{8})', l)
        if m: j2m[m.group(2)].add(m.group(1))
for l in open(os.path.join(T, 'june2017_re/funcmap.tsv'), 'rb').read().decode('utf-8', 'replace').split('\n')[1:]:
    p = l.split('\t')
    m = len(p) > 1 and re.match(r'(?:\w+::)*(\w+)', p[1].strip())
    if m and re.match(r'^[0-9a-f]{8}$', p[0]): j2m[p[0]].add(m.group(1))


def parse(name):
    s = next(i for i, l in enumerate(src) if l.startswith('void CX2GUArme_VioletMage::' + name + '()'))
    e = next(i for i in range(s + 1, len(src)) if src[i].rstrip() == '}')
    regs = collections.defaultdict(list)
    active, cases, cur = set(ALL), None, None
    for i in range(s, e):
        l = src[i].strip()
        if l.startswith('switch( eClass )'):
            cases, active = set(), set()
        elif cases is not None and re.match(r'case CX2Unit::\w+:', l):
            c = re.match(r'case CX2Unit::(\w+):', l).group(1); cases.add(CLS[c]); active = active | {CLS[c]}
        elif cases is not None and l in ('} break;', 'break;'):
            active = set()
        elif cases is not None and l == 'default:':
            active = ALL - cases
        elif cases is not None and l == '}' and src[i].startswith('\t}'):
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
    for l in open(os.path.join(T, 'june2017_re/char_arme/ch1j_reg_by_class.tsv'), encoding='utf-8').read().split('\n')[1:]:
        p = l.split('\t')
        if len(p) < 6 or p[0] != fn: continue
        out[p[1]].append({'id': p[3], 'table': p[4], 'cb': dict((x.split('=')[0], x.split('>')[-1]) for x in p[5].split(' ') if '=' in x)})
    return out


idmap = collections.defaultdict(set)
for fn, name in (('32ce77b0', 'June_InitStateCommon'), ('32cea0c0', 'June_InitState')):
    C, J = parse(name), sim(fn)
    for c in sorted(ALL, key=int):
        cr, used = C[c], set()
        for r in J[c]:
            if r['table'] in HOOKED: continue
            k = next((k for k, x in enumerate(cr) if k not in used and (x['table'] == r['table'] or
                      (r['table'] in ('ppppuVar5', 'puVar4') and not x['table'].startswith('"') and not x['table'].startswith('AVSI_')))), None)
            if k is None:
                print(name, c, 'NOT IN C++', r['id'], r['table']); continue
            used.add(k); x = cr[k]; idmap[r['id']].add(x['id'])
            for slot, ja in r['cb'].items():
                if slot not in x['cb']: print(name, c, 'MISSING', r['table'], slot, ja)
                elif x['cb'][slot] not in j2m.get(ja, ()): print(name, c, 'DIFF', r['table'], slot, ja, x['cb'][slot], sorted(j2m.get(ja, ())))
            for slot in x['cb']:
                if slot not in r['cb']: print(name, c, 'EXTRA', r['table'], slot, x['cb'][slot])
        for k, x in enumerate(cr):
            if k not in used: print(name, c, 'C++ EXTRA', x['id'], x['table'])
for i, s in sorted(idmap.items(), key=lambda t: int(t[0])):
    print('ID', i, '/'.join(sorted(s)), '' if len(s) == 1 else '<- MORE THAN ONE ENUMERATOR')
