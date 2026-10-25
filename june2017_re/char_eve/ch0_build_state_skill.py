"""char-eve CH0: build ch0_state_skill.tsv (June Eve states x skills x March). Copy of char_arme/ch0_build_state_skill.py
with Eve's registrars, script, classes and sub-phase rules.

    python june2017_re/charport_states.py init 329b8030 329ba9c0 3202b110 > june2017_re/char_eve/ch0_reg.tsv
    python june2017_re/charport_states.py skill 329c8a00                  > june2017_re/char_eve/ch0_skill.tsv
    (ch0_march_live_states.tsv: MakeTableReference names from `cl /P` of X2GuEve.cpp, source line + name)
    python june2017_re/char_eve/ch0_build_state_skill.py

Phase rules: states registered by InitState (329ba9c0 -> 329b8030, 3202b110, the START / WIN / LOSE roll, 31fdd490 hyper mode)
and the SetEquippedSkillFunc head cases (default Faena, COMMON_BUFF, THROW_ITEM) are CH1 (then split into CH1a-CH1j by name and
class gate). A skill state goes to the earliest phase of the skills that enter it: types 1 / 8 / 5 CH4, 12 CH1, 13 CH3d,
other actives CH2 if class 5 / 18 / 19 / 26 has the skill, else CH3. States June's C++ never names are n/a (dead data).
"""
import re, os, collections, struct

HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..'))
TIER1 = {5, 18, 19, 26}
CORE_FN = {'329b8030': 'InitStateCommon 329b8030', '329ba9c0': 'InitState 329ba9c0', '3202b110': 'CX2GUUser event states 3202b110'}
ORDER = {'CH1': 1, 'CH2': 2, 'CH3': 3, 'CH4': 4}  # coarse; sub-phases (CH1j2 Battle Seraph, CH2a2 Exotic Gear) come from subphase()
LO, HI = 0x32965dd0, 0x329cf870
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
B = 0x31aa0c00

src = open(os.path.join(T, 'june_src_fixed/data036_kom/Eve_Nasod.src.lua'), encoding='utf-8', errors='replace').read()
tabs = [(m.group(1), src[:m.start()].count('\n') + 1) for m in re.finditer(r'^([A-Za-z_]\w*)\s*=\s*\{', src, re.M)]
states = [t for t in tabs if not t[0].startswith('INIT_')]
i = src.index('INIT_COMMON_RANDOM_STATE')
rnd = set(re.findall(r'"(ENSI_\w+)"', src[i:src.index('\n}', i)]))

sk = {}
for l in open(os.path.join(T, 'june2017_re/s3_reform_ids_no_state.tsv'), encoding='utf-8').read().splitlines()[1:]:
    f = l.split('\t')
    if f[2] != 'X2GuEve.cpp':
        continue
    e = sk.setdefault(int(f[3]), [f[4], int(f[6]), set()])
    e[2].add(int(f[1]))

base_of = {}
for blk in open(os.path.join(T, 'june_src_fixed/data036_kom/NewSkillTempletVer2.src.lua'), encoding='utf-8', errors='replace').read().split('AddSkillTemplet_LUA('):
    me = re.search(r'^	m_eID = (\d+),', blk, re.M)
    md = re.search(r'^	m_DeleteSkillIDByUpgrade = (\d+),', blk, re.M)
    if me and md and int(md.group(1)):
        base_of[int(me.group(1))] = int(md.group(1))

# InitState class gates (decompile line ranges of 329ba9c0; gate = *(unit +0x219f*4 -> +0xaf0) +4 or local +0x38 = unit class)
GATES = [('329ba9c0', 312, 366, {18, 108}), ('329ba9c0', 368, 416, {19, 109}), ('329ba9c0', 418, 518, {26, 116}),
         ('329ba9c0', 1002, 1052, {18, 108}), ('329ba9c0', 1053, 1083, {26, 116}), ('329ba9c0', 1084, 1098, {109}),
         ('329ba9c0', 1111, 1175, {108}), ('329ba9c0', 1176, 1218, {109}), ('329ba9c0', 1219, 1228, {116}),
         ('329ba9c0', 1233, 1256, {19, 109})]
TILDE_GATE = {'ENSI_EEG_DASH_JUMP_ZZX': {18, 108}, 'ENSI_EPR_DASH_COMBO_ZZZ': {26, 116}, 'ENSI_EP_DASH_COMBO_ZZZ': {109},
              'ENSI_ECN_COMBO_ZZX_END': {108}, 'ENSI_ECE_DASH_XX': {109}, 'ENSI_ECB_DASH_XX': {116}}


def gate(fn, line):
    for f, a, b, g in GATES:
        if f == fn and a <= line <= b:
            return g
    return None


def skill_phase(sid):
    name, typ, cls = sk[sid]
    if typ in (1, 8, 5):
        return 'CH4'
    if typ == 12:
        return 'CH1'
    if typ == 13:
        return 'CH3'
    return 'CH2' if cls & TIER1 else 'CH3'


june = collections.defaultdict(lambda: {'how': [], 'cb': [], 'skills': set(), 'phase': None, 'gate': None})


def setp(st, p):
    if p and (st['phase'] is None or ORDER[p] < ORDER[st['phase']]):
        st['phase'] = p


for l in open(os.path.join(HERE, 'ch0_reg.tsv')).read().splitlines()[1:]:
    fn, line, sid, name, cb = (l.split('\t') + [''])[:5]
    if name == 'puVar4':
        continue  # USI_HYPER_MODE (id 23, name from 31fdd490): added below
    if name == 'ppppuVar3':
        continue  # START / WIN / LOSE roll (ids 9 / 11 / 12): added below
    st = june[name]
    st['how'].append('%s@%s id %s' % (CORE_FN.get(fn, fn), line, sid))
    if cb:
        st['cb'].append(cb)
    g = TILDE_GATE.get(name) if line.startswith('~') else gate(fn, int(line))
    if g:
        st['gate'] = g
    setp(st, 'CH1')
hyp_cb = next(l.split('\t')[4] for l in open(os.path.join(HERE, 'ch0_reg.tsv')).read().splitlines() if '\tpuVar4\t' in l)
roll_cb = {l.split('\t')[2]: l.split('\t')[4] for l in open(os.path.join(HERE, 'ch0_reg.tsv')).read().splitlines() if '\tppppuVar3\t' in l}
for n in rnd:
    june[n]['how'].append('START/WIN/LOSE roll 31fe22d0 (INIT_COMMON_RANDOM_STATE)')
    kind = '9' if n.endswith('_START') or n == 'ENSI_START' else ('11' if 'WIN' in n else '12')
    if roll_cb.get(kind):
        june[n]['cb'].append(roll_cb[kind])
    setp(june[n], 'CH1')
for n in ('ENSI_HYPER_MODE', 'ENSI_ECN_HYPER_MODE', 'ENSI_ECE_HYPER_MODE', 'ENSI_ECB_HYPER_MODE'):
    june[n]['how'].append('CX2GUUser hyper mode 31fdd490 (by job; InitStateCommon 329b8030@822 id 23)')
    june[n]['cb'].append(hyp_cb)
    setp(june[n], 'CH1')

# skill switch 329c8a00: head = transcendence-memo switch (31fdbcc0), then the type switch (5 / 6,7,13-16 hyper > 25600 / 12),
# then the id switch. The type-switch row '6,7,13,14,15,16' only leads into the hyper sub-switch rows: skipped.
DEFAULT_IDS = (25224, 25225)
MEMO_ONLY = set()
EXTRA_COPY = {'ENSI_A_EBS_ELECTRONIC_MOVE_FRONT': 219, 'ENSI_A_EBS_ELECTRONIC_MOVE_UP': 220, 'ENSI_A_EBS_ELECTRONIC_MOVE_DOWN': 221}
for l in open(os.path.join(HERE, 'ch0_skill.tsv')).read().splitlines()[1:]:
    f = l.split('\t')
    fn, line, cases, nt, ht, ncb, hcb, extra = f[:8]
    if cases == '6,7,13,14,15,16':
        continue
    ids = [int(c) for c in cases.split(',') if c.isdigit() and int(c) > 1000 or c in ('900', '106')]
    if not ids and 'default' not in cases and not (nt or ht) and cases not in ('5', '12'):
        continue
    if 'default' in cases:
        ids = [x for x in DEFAULT_IDS]   # Fayner has no case: its ids reach the default (ENSI_A_EAT_FAENA)
    if int(line) == 202:
        ids.append(25600)                # `if (id == 0x6400)` beside the hyper sub-switch: Lunatic Scud's base id
    head = ' (transcendence-memo head, 31fdbcc0)' if int(line) < 148 else ''
    if not ids and cases in ('5', '12'):
        ph = 'CH1'
    elif ids and not any(x in sk for x in ids):
        ph = 'CH3'                       # ids outside June's tree: transcendence memo skills (CH3d)
        head += ' (ids not in the tree: transcendence memo skills)'
    else:
        ph = min((skill_phase(x) for x in ids if x in sk), key=lambda p: ORDER[p], default='CH1')
    names = [x for x in nt.split(';') + ht.split(';') if x] + [e.split(':', 1)[1] for e in extra.split()]
    xname = {e.split(':', 1)[0]: e.split(':', 1)[1] for e in extra.split()}
    for xc in (f[9].split(' | ') if len(f) > 9 and f[9] else []):
        sid, cb = xc.split(':', 1)
        if sid in xname and cb.strip() not in ('-', '') and not cb.startswith('copy='):
            june[xname[sid]]['cb'].append(cb.strip())
    for n in names:
        st = june[n]
        st['how'].append('SetEquippedSkillFunc 329c8a00@%s case %s%s' % (line, cases, head))
        if n in EXTRA_COPY:
            st['how'].append('callbacks copied from state %d by 329cc2c0' % EXTRA_COPY[n])
        for cb in (ncb, hcb):
            if cb and cb not in st['cb'] and n not in EXTRA_COPY:
                st['cb'].append(cb)
        st['skills'].update(ids)
        if ph == 'CH3' and not any(x in sk for x in ids):
            MEMO_ONLY.add(n)
        setp(st, ph)
id2name = {}
for l in open(os.path.join(HERE, 'ch0_reg.tsv')).read().splitlines()[1:]:
    p = l.split('\t')
    id2name.setdefault(p[2], p[3])
for n, sid in EXTRA_COPY.items():
    src_name = id2name.get(str(sid), '?')
    june[n]['cb'] += [c for c in june[src_name]['cb'] if c not in june[n]['cb']]
    june[n]['how'][-1] += ' (%s)' % src_name

# March: live table names + the SET_CB_FUNC names after each
mcpp = open(os.path.join(T, 'X2Lib/X2GuEve.cpp'), 'rb').read().decode('latin-1').split('\n')
march = collections.defaultdict(list)
MSLOT = {'StateInit': 'Init', 'StateStartFuture': 'StartFuture', 'StateStart': 'Start', 'OnFrameMoveFuture': 'FrameMoveFuture',
         'OnFrameMove': 'FrameMove', 'OnCameraMove': 'CameraMove', 'OnEventProcess': 'EventProcess', 'StateEndFuture': 'EndFuture',
         'StateEnd': 'End'}
for l in open(os.path.join(HERE, 'ch0_march_live_states.tsv')).read().splitlines():
    ln, name = l.split('\t')[:2]
    if name.startswith('<'):
        continue
    ln = int(ln)
    fns = []
    for k in range(ln, min(ln + 14, len(mcpp))):
        if k > ln and ('MakeTableReference' in mcpp[k] or 'm_StateList[' in mcpp[k] or 'break;' in mcpp[k]):
            break
        fns += re.findall(r'\.\s*(\w+)\s*=\s*SET_CB_FUNC\(\s*(\w+)\s*,\s*(\w+)\s*\)', mcpp[k])
    march[name].append('%d:%s' % (ln, ','.join('%s=%s::%s' % (MSLOT.get(f[0], f[0]), f[1], f[2]) for f in fns)))

MKOM = 'F:/elsword stuff/elsword_2014/els_2014/237311/24965799/kom_contents'
mscript = open(os.path.join(MKOM, 'data036_kom', 'Eve_Nasod.lua'), 'rb').read()
msrc = open(os.path.join(T, 'X2Lib/X2GuEve.cpp'), 'rb').read() + open(os.path.join(T, 'X2Lib/X2GUUser.cpp'), 'rb').read()


def in_march_script(n):
    return mscript.find(n.encode()) >= 0


# ids with no case in 329c8a00: imm32 occurrences inside Eve's range (push / cmp / mov operands), a lead for the class map
exe = open(EXE, 'rb').read()


def imm_sites(v):
    p, out = struct.pack('<I', v), []
    j = exe.find(p, LO - B, HI - B)
    while j != -1 and len(out) < 8:
        out.append('%08x' % (j + B))
        j = exe.find(p, j + 1, HI - B)
    return out


JOB = ((5, 'EN'), (18, 'EEG'), (19, 'EAT'), (26, 'EPR'), (108, 'ECN'), (109, 'ECE'), (116, 'ECB'))


def jobline(ids):
    cl, typ = set(), set()
    for i in ids:
        if i in sk:
            cl |= sk[i][2]
            typ.add(sk[i][1])
    if 13 in typ:
        return 'HYPER'
    for c, n in JOB:
        if c in cl:
            return n
    return '?'


SUB2 = {'EN': 'CH2a', 'EEG': 'CH2a2', 'EAT': 'CH2b', 'EPR': 'CH2c', 'ECN': 'CH3a', 'ECE': 'CH3b', 'ECB': 'CH3c', 'HYPER': 'CH3d'}
GATE_PH = ((18, 'CH1f'), (19, 'CH1h'), (26, 'CH1j'), (108, 'CH1g'), (109, 'CH1i'), (116, 'CH1j2'))


def subphase(name, how, ph, ids, g):
    """CH1a movement and slot-head states (wait, walk, jump, dash, charge MP, throw item, common buff, Faena default, intrude,
    wall landing); CH1b reactions (damage, down / stand-up, die, bounding); CH1c riding + pepper run / phoenix run / dance;
    CH1d Nasod ground combos; CH1e dash and jump combos; class-gated InitState blocks by their first class (CH1f 18, CH1g 108,
    CH1h 19, CH1i 109, CH1j 26 / 116); CH1m registration only (START / WIN / LOSE roll, hyper mode); CH1q Nasod Core (parrying);
    CH4c Code Recovery (EVE_RECOVERY). Skills: CH2a 5 + 18, CH2b 19, CH2c 26, CH3a 108, CH3b 109, CH3c 116, CH3d hyper."""
    if name in MEMO_ONLY and not any(x in sk for x in ids):
        return 'CH3d'
    if ph in ('CH2', 'CH3'):
        p = SUB2.get(jobline(ids), ph)
        return 'CH2c2' if p == 'CH2c' and re.search(r'SWEEP_ROLLING|GIGA_STREAM', name) else p
    if name.startswith('USI_'):
        return 'CH1m'
    if ph != 'CH1':
        return ph
    if re.search(r'_(START|WIN2?|LOSE2?)$', name) and 'roll' in how or re.search(r'^ENSI_(EC[NEB]_)?HYPER_MODE$', name):
        return 'CH1m'
    if 'PARRYING' in name:
        return 'CH1q'
    if name == 'ENSI_EVE_RECOVERY':
        return 'CH4c'
    if 'RIDING' in name or 'AMUSEMENT' in name or 'DANCE' in name or 'PEPPER' in name:
        return 'CH1c'
    if re.search(r'DAMAGE_|_DIE_|DIE_FRONT|DIE_BACK|STANDUP_', name):
        return 'CH1b'
    if g:
        for c, p in GATE_PH:
            if c in g:
                return p
    if re.search(r'^ENSI_(EEG|EG)_', name):
        return 'CH1f'
    if re.search(r'^ENSI_(ECN|NS)_', name):
        return 'CH1g'
    if re.search(r'^ENSI_(ECE|EP)_', name):
        return 'CH1i'
    if re.search(r'^ENSI_(EPR|PR)_', name):
        return 'CH1j'
    if re.search(r'^ENSI_(EBS|ECB|BS)_', name):
        return 'CH1j2'
    if re.search(r'DASH|JUMP', name) and re.search(r'COMBO|ATTACK|_[XZ]', name):
        return 'CH1e'
    if re.search(r'COMBO|ATTACK_[XZ]|_Z\d|_X\d|ZZ|XX', name):
        return 'CH1d'
    return 'CH1a'


rows, seen = [], set()
for name, line in states:
    dup = name in seen
    seen.add(name)
    st = june.get(name)
    if st and st['how']:
        ph = st['phase']
        if name in march:
            verdict = 'pair'
        elif in_march_script(name) or msrc.find(b'"' + name.encode() + b'"') >= 0:
            verdict = 'pair (March reads it by name outside MakeTableReference / March script has the table)'
        else:
            verdict = 'june-only'
        how = ' | '.join(dict.fromkeys(st['how']))
        cb = ' | '.join(dict.fromkeys(st['cb']))
        skills = ','.join('%d %s (t%d)' % (x, sk[x][0], sk[x][1]) for x in sorted(st['skills']) if x in sk)
    else:
        ph, verdict, how, cb, skills = 'n/a', 'june-dead-data', 'no reference in June C++ (string absent from the exe)', '', ''
        if exe.find(b'\0' + name.encode() + b'\0') >= 0:
            how = 'string in the exe but no registration found: read its xrefs'
    if ph != 'n/a':
        ph = subphase(name, how, ph, st['skills'], st['gate'])
    if dup:
        verdict += '; duplicate table (June script defines it twice, the second wins: JUNE_BUGS candidate)'
    rows.append((name, str(line), how, skills, cb, ' ; '.join(march.get(name, [])) or '-', verdict, ph))
for name in sorted(june):
    if name not in dict(states) and june[name]['how']:
        st = june[name]
        rows.append((name, '-', ' | '.join(st['how']), '', ' | '.join(st['cb']), ' ; '.join(march.get(name, [])) or '-',
                     'june-cpp-no-table (June C++ registers it, June script has no table: JUNE_BUGS candidate)',
                     subphase(name, ' | '.join(st['how']), st['phase'], st['skills'], st['gate'])))
for name in sorted(march):
    if name not in dict(states) and name not in june:
        rows.append((name, '-', 'March only', '', '', ' ; '.join(march[name]), 'march-only', 'CH1m (registration drops it: June wins)'))

with open(os.path.join(HERE, 'ch0_state_skill.tsv'), 'w', encoding='utf-8', newline='\n') as o:
    o.write('# part 1: states (June script tables + June registrations with no table + March-only states)\n')
    o.write('state\tjune_script_line\tjune_registration\tskills\tjune_callbacks (thunk>target)\tmarch (cpp line:callbacks)\tverdict\tphase\n')
    for r in rows:
        o.write('\t'.join(r) + '\n')
    o.write('# part 2: skill ids (%d June Eve ids from s3_reform_ids_no_state.tsv)\n' % len(sk))
    o.write('skill_id\tname\ttype\tclasses\tjune_state(s)\tphase\tnote\n')
    SKROWS = []
    for sid in sorted(sk):
        name, typ, cls = sk[sid]
        note = ''
        sts = sorted({n for n, st in june.items() if sid in st['skills']})
        if not sts and sid in base_of:
            sts = sorted({n for n, st in june.items() if base_of[sid] in st['skills']})
            note = 'switch value = base %d (m_DeleteSkillIDByUpgrade, 32207210)' % base_of[sid]
        ph = SUB2.get(jobline([sid]), skill_phase(sid)) if skill_phase(sid) in ('CH2', 'CH3') else skill_phase(sid)
        if ph == 'CH2c' and sid in (25021, 25039, 25025, 25065):
            ph = 'CH2c2'
        if not sts:
            note = 'no case in 329c8a00: passive / buff data (no state)' if typ in (1, 5, 8) else 'no case in 329c8a00 (falls to default: ENSI_A_EAT_FAENA)'
            hits = imm_sites(base_of.get(sid, sid))
            if hits and typ not in (5,):
                note += '; imm32 in class code at %s' % ' '.join(hits)
            if typ == 5:
                note = 'type 5 = ST_BUFF: state ENSI_SI1_COMMON_BUFF (type case 5, CH1a); buff data CH4a'
            if typ == 12:
                sts = ['ENSI_THROW_ITEM']
                note = 'type 12: case 0xc -> ENSI_THROW_ITEM when id 900'
        if typ in (1, 8, 5):
            ph = 'CH4a'
        SKROWS.append((sid, name, typ, cls, sts, ph))
        o.write('%d\t%s\t%d\t%s\t%s\t%s\t%s\n' % (sid, name, typ, ','.join(map(str, sorted(cls))), ';'.join(sts), ph, note))
    c = collections.Counter((r[6].split(' (')[0].split(';')[0], r[7]) for r in rows)
    o.write('# totals states: %s\n' % ', '.join('%s/%s=%d' % (k[0], k[1], v) for k, v in sorted(c.items())))
    o.write('# script tables: %d (%d distinct names)\n' % (len(states), len(dict(states))))
    fn_ph = {}
    for name, st in june.items():
        for cb in st['cb']:
            for t in re.findall(r'>([0-9a-f]{8})', cb):
                if LO <= int(t, 16) < HI:
                    p = subphase(name, ' | '.join(st['how']), st['phase'], st['skills'], st['gate'])
                    if t not in fn_ph or (p, t) < (fn_ph[t], t):
                        fn_ph[t] = p
    fc = collections.Counter(fn_ph.values())
    o.write('# totals distinct Eve-range state functions by phase: %s\n' % ', '.join('%s=%d' % kv for kv in sorted(fc.items())))
    sc = collections.Counter(r2[5] for r2 in SKROWS)
    o.write('# totals skills by phase: %s\n' % ', '.join('%s=%d' % kv for kv in sorted(sc.items())))
print(open(os.path.join(HERE, 'ch0_state_skill.tsv'), encoding='utf-8').read().split('# totals', 1)[1])
