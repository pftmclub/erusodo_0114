"""pilot-char-revamp CH0: build ch0_state_skill.tsv (June Aisha states x skills x March) from the extractor outputs.

    python june2017_re/charport_states.py init 32ce77b0 32cea0c0 3202b110 > <S>/reg.tsv
    python june2017_re/charport_states.py skill 32cede00 > <S>/skill.tsv
    (march_live_states.tsv: MakeTableReference names from `cl /P` of X2GUArme_VioletMage.cpp, source line + name)
    python june2017_re/char_arme/ch0_build_state_skill.py <S>

Phase rules (CH0 Done note): states registered by InitState (32cea0c0 -> 32ce77b0, 3202b110, the START / WIN / LOSE roll,
31fdd490 hyper mode) and the SetEquippedSkillFunc head cases (default Power Attack, COMMON_BUFF, THROW_ITEM) are CH1.
A skill state goes to the earliest phase of the skills that enter it: types 1 / 8 (passive) CH4, type 5 (buff data) CH4,
type 12 CH1, type 13 (hyper active) CH3, other actives CH2 if any class 2 / 14 / 15 / 23 has the skill, else CH3.
States June's C++ never names are n/a (dead data in June's script).
"""
import re, sys, os, collections

S = sys.argv[1]
T = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)))
TIER1 = {2, 14, 15, 23}
CORE_FN = {'32ce77b0': 'InitStateCommon 32ce77b0', '32cea0c0': 'InitState 32cea0c0', '3202b110': 'CX2GUUser event states 3202b110'}
ORDER = {'CH1': 1, 'CH2': 2, 'CH3': 3, 'CH4': 4}

src = open(os.path.join(T, 'june_src_fixed/data036_kom/Arme_VioletMage.src.lua'), encoding='utf-8', errors='replace').read()
tabs = [(m.group(1), src[:m.start()].count('\n') + 1) for m in re.finditer(r'^([A-Za-z_]\w*)\s*=\s*\{', src, re.M)]
states = [t for t in tabs if not t[0].startswith('INIT_')]
i = src.index('INIT_COMMON_RANDOM_STATE')
rnd = set(re.findall(r'"(AVSI_\w+)"', src[i:src.index('\n}', i)]))

# skills: id -> (name, type, classes)
sk = {}
for l in open(os.path.join(T, 'june2017_re/s3_reform_ids_no_state.tsv'), encoding='utf-8').read().splitlines()[1:]:
    f = l.split('\t')
    if 'Arme' not in f[2]:
        continue
    e = sk.setdefault(int(f[3]), [f[4], int(f[6]), set()])
    e[2].add(int(f[1]))


# June's switch value is 32207210( id ) = templet +0x178 (m_DeleteSkillIDByUpgrade) when non-zero, else the id
base_of = {}
for blk in open(os.path.join(T, 'june_src_fixed/data036_kom/NewSkillTempletVer2.src.lua'), encoding='utf-8', errors='replace').read().split('AddSkillTemplet_LUA('):
    me = re.search(r'^	m_eID = (\d+),', blk, re.M)
    md = re.search(r'^	m_DeleteSkillIDByUpgrade = (\d+),', blk, re.M)
    if me and md and int(md.group(1)):
        base_of[int(me.group(1))] = int(md.group(1))


# actives with no state case: the id is tested inside class overrides instead (imm32 scan of 32cb6000-32cf3000)
NOSTATE = {22203: '32cc5d6f 32ccb8c6 32ccbcd9 32ccfb9f 32ce1d85', 22205: '32cb8acc 32cc26f5 32ccbcc5 32ccfc5b',
           22210: '32cc2745 32ccbd52', 22215: '32cc2795 32ccfc8b'}


def skill_phase(sid):
    name, typ, cls = sk[sid]
    if typ in (1, 8, 5):
        return 'CH4'
    if typ == 12:
        return 'CH1'
    if typ == 13:
        return 'CH3'
    return 'CH2' if cls & TIER1 else 'CH3'


june = collections.defaultdict(lambda: {'how': [], 'cb': [], 'skills': set(), 'phase': None})


def setp(st, p):
    if p and (st['phase'] is None or ORDER[p] < ORDER[st['phase']]):
        st['phase'] = p


for l in open(os.path.join(S, 'reg.tsv')).read().splitlines()[1:]:
    fn, line, sid, name, cb = (l.split('\t') + [''])[:5]
    st = june[name]
    st['how'].append('%s@%s id %s' % (CORE_FN.get(fn, fn), line, sid))
    if cb:
        st['cb'].append(cb)
    setp(st, 'CH1')
for n in rnd:
    june[n]['how'].append('START/WIN/LOSE roll 31fe22d0 (INIT_COMMON_RANDOM_STATE)')
    setp(june[n], 'CH1')
for n in ('AVSI_HYPER_MODE', 'AVSI_AEM_HYPER_MODE', 'AVSI_AVP_HYPER_MODE', 'AVSI_ADW_HYPER_MODE'):
    june[n]['how'].append('CX2GUUser hyper mode 31fdd490 (by job)')
    setp(june[n], 'CH1')
skillrows = []
for l in open(os.path.join(S, 'skill.tsv')).read().splitlines()[1:]:
    f = l.split('\t')
    fn, line, cases, nt, ht, ncb, hcb, extra = f[:8]
    ids = [int(c) for c in cases.split(',') if c.isdigit() and int(c) > 1000 or c in ('900', '106')]
    if not ids and 'default' not in cases and not (nt or ht):
        continue
    ph = min((skill_phase(x) for x in ids if x in sk), key=lambda p: ORDER[p], default='CH1')
    skillrows.append((cases, nt, ht, ph, ids))
    names = [x for x in nt.split(';') + ht.split(';') if x] + [e.split(':', 1)[1] for e in extra.split()]
    for n in names:
        st = june[n]
        st['how'].append('SetEquippedSkillFunc 32cede00@%s case %s' % (line, cases))
        for cb in (ncb, hcb):
            if cb and cb not in st['cb']:
                st['cb'].append(cb)
        st['skills'].update(ids)
        setp(st, ph)

# March: live table names + the SET_CB_FUNC names after each (source lines are 1:1 with the .i #line numbers)
mcpp = open(os.path.join(T, 'X2Lib/X2GUArme_VioletMage.cpp'), 'rb').read().decode('latin-1').split('\n')
march = collections.defaultdict(list)
MSLOT = {'StateInit': 'Init', 'StateStartFuture': 'StartFuture', 'StateStart': 'Start', 'OnFrameMoveFuture': 'FrameMoveFuture',
         'OnFrameMove': 'FrameMove', 'OnCameraMove': 'CameraMove', 'OnEventProcess': 'EventProcess', 'StateEndFuture': 'EndFuture',
         'StateEnd': 'End'}
for l in open(os.path.join(S, 'march_live_states.tsv')).read().splitlines():
    ln, name = l.split('\t')
    ln = int(ln)
    fns = []
    for k in range(ln, min(ln + 14, len(mcpp))):
        if k > ln and ('MakeTableReference' in mcpp[k] or 'm_StateList[' in mcpp[k] or 'break;' in mcpp[k]):
            break
        fns += re.findall(r'\.\s*(\w+)\s*=\s*SET_CB_FUNC\(\s*(\w+)\s*,\s*(\w+)\s*\)', mcpp[k])
    march[name].append('%d:%s' % (ln, ','.join('%s=%s::%s' % (MSLOT.get(f[0], f[0]), f[1], f[2]) for f in fns)))

MKOM = 'F:/elsword stuff/elsword_2014/els_2014/237311/24965799/kom_contents'
mscript = open(os.path.join(MKOM, 'data036_kom', 'Arme_VioletMage.lua'), 'rb').read()
msrc = open(os.path.join(T, 'X2Lib/X2GUArme_VioletMage.cpp'), 'rb').read() + open(os.path.join(T, 'X2Lib/X2GUUser.cpp'), 'rb').read()


def in_march_script(n):
    b = n.encode()
    return any(mscript.find(bytes([len(b) + 1]) + b) >= 0 for _ in [0]) or mscript.find(b) >= 0


# hand-checked (CH0): names the extractor cannot see
MANUAL = {'AVSI_A_AV_TELEPORT_START': ('CH2a', 'SetEquippedSkillFunc 32cede00 case 22200,22216: kept in a std::string (31aa9150) when '
                                       '32071e70 IsEquippedSkillUpgraded is false; the table reference is always _UPGRADE')}
def jobline(ids):
    cl, typ = set(), set()
    for i in ids:
        if i in sk:
            cl |= sk[i][2]
            typ.add(sk[i][1])
    if 13 in typ:
        return 'HYPER'
    for c, n in ((2, 'VM'), (14, 'HM'), (15, 'DM'), (23, 'BM'), (104, 'AEM'), (105, 'AVP'), (113, 'ADW')):
        if c in cl:
            return n
    return '?'


SUB2 = {'VM': 'CH2a', 'HM': 'CH2a', 'DM': 'CH2b', 'BM': 'CH2b', 'AEM': 'CH3a', 'AVP': 'CH3a', 'ADW': 'CH3b', 'HYPER': 'CH3b'}


def subphase(name, how, ph, ids):
    """CH0 split (Done note): CH1a movement and slot-head states (wait, walk, jump, dash, charge MP, throw item, power
    attack, common buff); CH1b reactions (damage, die, bounding, stand-up, teleport stand-up, intrude), riding, pepper run, event dance;
    CH1c ground combos; CH1d dash / jump combos; CH1e the High Magician line (HM, AEM); CH1f the Dark Magician line (DM, AVP);
    CH1g the Battle Magician line (BM, ADW); CH1 = registration-only (START/WIN/LOSE, hyper mode) and everything that is not a
    state function. Skills: CH2a VM + HM, CH2b DM + BM, CH3a AEM + AVP, CH3b ADW + hyper actives."""
    if ph in ('CH2', 'CH3'):
        return SUB2.get(jobline(ids), ph)
    if ph != 'CH1':
        return ph
    if re.search(r'_(START|WIN2?|LOSE2?)$', name) and 'roll' in how or 'HYPER_MODE' in name:
        return 'CH1'
    if re.search(r'DAMAGE_|_DIE_|DIE_FRONT|DIE_BACK|TELEPORT|START_INTRUDE|WALL_LANDING', name) and 'RIDING' not in name:
        return 'CH1b'
    if 'RIDING' in name or 'AMUSEMENT' in name or 'DANCE' in name or 'PEPPER' in name:
        return 'CH1b'
    if re.search(r'ABM|ADW|DIMENSION_WITCH', name):
        return 'CH1g'
    if re.search(r'ADM|DARK_MAGIC|AVP', name):
        return 'CH1f'
    if re.search(r'AHM|HIGH_MAGIC|AEM|_EM_', name):
        return 'CH1e'
    if re.search(r'DASH|JUMP', name) and re.search(r'COMBO|ATTACK|_[XZ]', name):
        return 'CH1d'
    if re.search(r'COMBO|ATTACK_[XZ]|_Z\d|_X\d|ZZ|XX', name):
        return 'CH1c'
    return 'CH1a'


rows = []
for name, line in states:
    st = june.get(name)
    if name in MANUAL:
        ph, how = MANUAL[name]
        rows.append((name, str(line), how, '22200 RNSI_A_AISHA_TELEPORT (t6),22216 RNSI_A_AISHA_TELEPORT_UPGRADE (t6)', '',
                     ' ; '.join(march.get(name, [])) or '-', 'pair' if name in march else 'june-only', ph))
        continue
    if st and st['how']:
        ph = st['phase']
        if name in march:
            verdict = 'pair'
        elif in_march_script(name) or msrc.find(b'"' + name.encode() + b'"') >= 0:
            verdict = 'pair (March reads it by name outside MakeTableReference / March script has the table)'
        else:
            verdict = 'june-only'
        how = ' | '.join(dict.fromkeys(st['how']))
        cb = ' | '.join(st['cb'])
        skills = ','.join('%d %s (t%d)' % (x, sk[x][0], sk[x][1]) for x in sorted(st['skills']) if x in sk)
    else:
        ph, verdict, how, cb, skills = 'n/a', 'june-dead-data', 'no reference in June C++ (string absent from the exe)', '', ''
    if ph != 'n/a':
        ph = subphase(name, how, ph, st['skills'])
    rows.append((name, str(line), how, skills, cb, ' ; '.join(march.get(name, [])) or '-', verdict, ph))
for name in sorted(march):
    if name not in dict(states):
        if name in june and june[name]['how']:
            rows.append((name, '-', ' | '.join(june[name]['how']), '', ' | '.join(june[name]['cb']), ' ; '.join(march[name]),
                         'june-cpp-no-table (June C++ names it, June script has no table: JUNE_BUGS candidate)', 'CH1'))
        else:
            rows.append((name, '-', 'March only', '', '', ' ; '.join(march[name]), 'march-only', 'CH1 (registration drops it: June wins)'))

with open(os.path.join(OUT, 'ch0_state_skill.tsv'), 'w', encoding='utf-8', newline='\n') as o:
    o.write('# part 1: states (June script tables + March-only states)\n')
    o.write('state\tjune_script_line\tjune_registration\tskills\tjune_callbacks (thunk>target)\tmarch (cpp line:callbacks)\tverdict\tphase\n')
    for r in rows:
        o.write('\t'.join(r) + '\n')
    o.write('# part 2: skill ids (130 June Aisha ids from s3_reform_ids_no_state.tsv)\n')
    o.write('skill_id\tname\ttype\tclasses\tjune_state(s)\tphase\tnote\n')
    for sid in sorted(sk):
        name, typ, cls = sk[sid]
        note = ''
        sts = sorted({n for n, st in june.items() if sid in st['skills']})
        if not sts and sid in base_of:
            sts = sorted({n for n, st in june.items() if base_of[sid] in st['skills']})
            note = 'switch value = base %d (m_DeleteSkillIDByUpgrade, 32207210)' % base_of[sid]
        ph = SUB2.get(jobline([sid]), skill_phase(sid)) if skill_phase(sid) in ('CH2', 'CH3') else skill_phase(sid)
        if not sts:
            note = 'no case in 32cede00: passive / buff data (no state)' if typ in (1, 5, 8) else 'no case in 32cede00 (falls to default)'
            b = base_of.get(sid, sid)
            if b in NOSTATE:
                note = 'no state: June tests id 0x%x in class code at %s (see ch0_class_map.tsv)' % (b, NOSTATE[b])
            if typ == 5:
                note = 'type 5 = ST_BUFF: state AVSI_SI_AV_COMMON_BUFF (case 5, CH1); buff data CH4'
            if typ == 12:
                sts = ['AVSI_THROW_ITEM']
                note = 'type 12: case 0xc -> AVSI_THROW_ITEM when id 900'
        o.write('%d\t%s\t%d\t%s\t%s\t%s\t%s\n' % (sid, name, typ, ','.join(map(str, sorted(cls))), ';'.join(sts), ph, note))
    # totals
    c = collections.Counter((r[6].split(' (')[0], r[7]) for r in rows)
    o.write('# totals states: %s\n' % ', '.join('%s/%s=%d' % (k[0], k[1], v) for k, v in sorted(c.items())))
    fn_ph = {}
    for name, st in june.items():
        for cb in st['cb']:
            for t in re.findall(r'>(32c[b-f][0-9a-f]{4})', cb):
                if 0x32cb6e00 <= int(t, 16) < 0x32cf3000:
                    p = subphase(name, ' | '.join(st['how']), st['phase'], st['skills'])
                    if t not in fn_ph or (p, t) < (fn_ph[t], t):
                        fn_ph[t] = p
    fc = collections.Counter(fn_ph.values())
    o.write('# totals distinct Aisha-range state functions by phase: %s\n' % ', '.join('%s=%d' % kv for kv in sorted(fc.items())))
    o.write('# totals skills by phase: %s\n' % ', '.join('%s=%d' % kv for kv in sorted(collections.Counter((SUB2.get(jobline([s]), skill_phase(s)) if skill_phase(s) in ('CH2', 'CH3') else skill_phase(s)) for s in sk).items())))
print(open(os.path.join(OUT, 'ch0_state_skill.tsv'), encoding='utf-8').read().split('# totals', 1)[1])
