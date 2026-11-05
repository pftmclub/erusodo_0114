r"""char-raven CH0: build ch0_state_skill.tsv (June Raven states x skills x March). Copy of char_elsword/ch0_build_state_skill.py
with Raven's registrars, script, classes and the sub-phase rules of JUNE_CHARACTER_RAVEN_PHASES.md "Phase CH1" (group table).

    python june2017_re/charport_states.py init 32c5d660 32c66fe0 3202b110 32c511c0 32c51b70 32c63670 32c57660 > june2017_re/char_raven/ch0_reg.tsv
        (32c5d660 InitStateCommon, 32c66fe0 InitState = START / WIN / LOSE roll + id 130 + the switch that calls the per-class registrars 32c511c0 (16 / 106),
        32c51b70 (17 / 107), 32c63670 (25 / 115); 32c57660 InitPassiveSkillState registers the Shadow Step states; no hand rows)
    python june2017_re/charport_states.py skill 32c641a0                  > june2017_re/char_raven/ch0_skill.tsv
    python june2017_re/char_raven/ch0_skill_listing_emu.py <scratch>/sw.txt        (funcdis.py --func 32c641a0)
    python june2017_re/char_raven/ch0_march_live_states.py <raven.i>      (s3_no_state.py --keep-i, cl /P of X2GURaven.cpp)
    python june2017_re/char_raven/ch0_build_state_skill.py

Phase rules. States registered by InitState / InitStateCommon / 3202b110 / the START-WIN-LOSE roll / hyper mode 31fdd490 are CH1, split by name
(ch1_sub): CH1a movement, CH1b reactions, CH1c riding / pepper / dance, CH1d RSI_COMBO* + base dash / jump combos, CH1e Soul Taker + Blade
Master (RBM_ tables), CH1f Over Taker + Reckless Fist (RRF_), CH1g Weapon Taker (RWT_), CH1g2 Veteran Commander (RVC_ tables, all of them),
CH1m the USI_ rows. A table with an RBM_ / RRF_ / RWT_ / RVC_ prefix goes to its line's CH1 group whether InitState or the skill switch registers it.
A skill state goes to the earliest phase of the skills that enter it: types 1 / 8 passive CH4a, 12 CH1a, 13 CH3d, other actives by the earliest class
that has the skill (4 CH2a, 16 CH2b, 17 CH2c, 25 CH2d, 106 CH3a, 107 CH3b, 115 CH3c). Unassigned or ambiguous names get phase UNASSIGNED.
"""
import re, os, collections, struct

HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..'))
LO, HI = 0x32c25a20, 0x32c6a640
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
B = 0x31aa0c00
CORE_FN = {'32c5d660': 'InitStateCommon 32c5d660', '32c66fe0': 'InitState 32c66fe0 (class-gated)', '3202b110': 'CX2GUUser event states 3202b110',
           '32c511c0': 'class registrar 32c511c0 (classes 16 / 106)', '32c51b70': 'class registrar 32c51b70 (classes 17 / 107)',
           '32c63670': 'class registrar 32c63670 (classes 25 / 115)', '32c57660': 'InitPassiveSkillState 32c57660'}
ORDER = ['CH1a', 'CH1b', 'CH1c', 'CH1d', 'CH1d2', 'CH1e', 'CH1f', 'CH1g', 'CH1g2', 'CH1h', 'CH1i', 'CH1j', 'CH1k', 'CH1m', 'CH1n', 'CH1o', 'CH1p', 'CH1q', 'CH1',
         'CH2a', 'CH2b', 'CH2c0', 'CH2c', 'CH2d', 'CH3a', 'CH3b', 'CH3c', 'CH3c2', 'CH3d', 'CH4a', 'CH4b', 'CH4c', 'n/a', 'UNASSIGNED']
CLS_PH = {4: 'CH2a', 16: 'CH2b', 17: 'CH2c', 25: 'CH2d', 106: 'CH3a', 107: 'CH3b', 115: 'CH3c'}

src = open(os.path.join(T, 'june_src_fixed/data036_kom/RAVEN_FIGHTER.src.lua'), encoding='utf-8', errors='replace').read()
tabs = [(m.group(1), src[:m.start()].count('\n') + 1) for m in re.finditer(r'^([A-Za-z_]\w*)\s*=\s*\{', src, re.M)]
i = src.index('INIT_COMMON_RANDOM_STATE')
rnd = set(re.findall(r'"(RSI_\w+)"', src[i:src.index('\n}', i)]))

sk = {}
for l in open(os.path.join(T, 'june2017_re/s3_reform_ids_no_state.tsv'), encoding='utf-8').read().splitlines()[1:]:
    f = l.split('\t')
    if f[2] != 'X2GURaven.cpp':
        continue
    e = sk.setdefault(int(f[3]), [f[4], int(f[6]), set()])
    e[2].add(int(f[1]))

base_of = {}
for blk in open(os.path.join(T, 'june_src_fixed/data036_kom/NewSkillTempletVer2.src.lua'), encoding='utf-8', errors='replace').read().split('AddSkillTemplet_LUA('):
    me = re.search(r'^	m_eID = (\d+),', blk, re.M)
    md = re.search(r'^	m_DeleteSkillIDByUpgrade = (\d+),', blk, re.M)
    if me and md and int(md.group(1)):
        base_of[int(me.group(1))] = int(md.group(1))

def gate(fn, line):
    return None


VC_SPLIT = {24022, 24023, 24024, 24053, 24054, 24055}   # Ignition Crow Napalm / Incineration, Deadly Raid (+ upgrades): CH3c split (CH0 lead 2026-11-05)


def skill_phase(sid):
    name, typ, cls = sk[sid]
    if typ in (1, 2, 3, 4, 8):
        return 'CH4a'
    if typ == 12:
        return 'CH1a'
    if typ == 13:
        return 'CH3d'
    if sid in VC_SPLIT:
        return 'CH3c2'
    return min((CLS_PH[c] for c in cls if c in CLS_PH), key=ORDER.index)


june = collections.defaultdict(lambda: {'how': [], 'cb': [], 'skills': set(), 'gates': [], 'ungated': False, 'roll': None, 'notes': []})
NOTES = []


def minph(a, b):
    if a is None:
        return b
    if b is None:
        return a
    return a if ORDER.index(a) <= ORDER.index(b) else b


regrows = [l.split('\t') + [''] * 5 for l in open(os.path.join(HERE, 'ch0_reg.tsv')).read().splitlines()[1:]]
roll_cb = {}
hyp_cb = None
HYPER_NAMES = ('RSI_HYPER_MODE',)
for fn, line, sid, name, cb in [r[:5] for r in regrows]:
    if name == 'puVar4':
        hyp_cb = cb
        continue
    if name == 'ppppuVar6':
        roll_cb[sid] = cb
        continue
    st = june[name]
    st['how'].append('%s@%s id %s' % (CORE_FN.get(fn, fn), line, sid))
    if cb:
        st['cb'].append(cb)
    g = gate(fn, line)
    if g:
        st['gates'].append(g)
    else:
        st['ungated'] = True
for n in sorted(rnd):
    kind = '9' if re.search(r'START$', n) else ('11' if 'WIN' in n else '12')
    june[n]['how'].append('START/WIN/LOSE roll 31fe22d0 (INIT_COMMON_RANDOM_STATE) id %s' % kind)
    if roll_cb.get(kind):
        june[n]['cb'].append(roll_cb[kind])
    june[n]['roll'] = True
for n in HYPER_NAMES:
    june[n]['how'].append('CX2GUUser hyper mode 31fdd490 (InitStateCommon 32c5d660@735 id 23)')
    june[n]['cb'].append(hyp_cb)
    june[n]['roll'] = True

# skill switch 32c641a0: head cases 5 (COMMON_BUFF) / 12 (THROW_ITEM) / default, the type switch row '6,7,13,14,15,16' is only a lead-in
# to the hyper sub-switch (skipped), then the id cases. The 'default' row that carries a table is MEGASLASH_MEMO_READY (ids come from the listing reader).
SKROWS_CASES = []
for l in open(os.path.join(HERE, 'ch0_skill.tsv')).read().splitlines()[1:]:
    f = (l.split('\t') + [''] * 10)[:10]
    fn, line, cases, nt, ht, ncb, hcb, extra, calls, xcb = f
    if cases == '6,7,13,14,15,16':
        continue
    ids = [int(c) for c in cases.split(',') if c.isdigit() and int(c) > 1000 or c == '900']
    if not ids and cases not in ('5', '12', 'default'):
        continue
    note = ''
    if cases == 'default':
        if nt:
            note = 'default branch (no case): the ids that reach it are read by the listing reader'
        else:
            note = 'skill default case (no table)'
    elif ids and not any(x in sk for x in ids):
        note = 'case ids not in June tree: %s' % ','.join(map(str, ids))
    elif ids and any(x not in sk for x in ids):
        note = 'case ids not in June tree: %s' % ','.join(str(x) for x in ids if x not in sk)
    names = [x for x in (nt, ht) if x] + [e.split(':', 1)[1] for e in extra.split() if ':' in e and e.split(':', 1)[1].startswith(('RSI_', 'RVC_', 'RBM_', 'RRF_', 'RWT_', 'SI_'))]
    xname = {e.split(':', 1)[0]: e.split(':', 1)[1] for e in extra.split()}
    for xc in (xcb.split(' | ') if xcb else []):
        sid_, cb = xc.split(':', 1)
        if sid_ in xname and cb.strip() not in ('-', '') and not cb.startswith('copy='):
            june[xname[sid_]]['cb'].append(cb.strip())
    SKROWS_CASES.append((line, cases, names, ids, note))
    for n in names:
        st = june[n]
        st['how'].append('SetEquippedSkillFunc 32c641a0@%s case %s' % (line, cases))
        for cb in (ncb, hcb):
            if cb and cb not in st['cb']:
                st['cb'].append(cb)
        st['skills'].update(ids)
        if note and note not in st['notes']:
            st['notes'].append(note + ' (line %s)' % line)
        if cases in ('5', '12'):
            st['head'] = True

# March: live table names + callbacks (ch0_march_live_states.tsv: line, table, Slot=Class::Fn@line ...)
march = collections.defaultdict(list)
for l in open(os.path.join(HERE, 'ch0_march_live_states.tsv'), encoding='utf-8').read().splitlines():
    p = (l.split('\t') + [''])[:3]
    march[p[1]].append((int(p[0]), p[2].split()))
MKOM = 'F:/elsword stuff/elsword_2014/els_2014/237311/24965799/kom_contents'
mscript = open(os.path.join(MKOM, 'data036_kom', 'RAVEN_FIGHTER.lua'), 'rb').read()
msrc = open(os.path.join(T, 'X2Lib/X2GURaven.cpp'), 'rb').read() + open(os.path.join(T, 'X2Lib/X2GUUser.cpp'), 'rb').read()


def in_march_script(n):
    return mscript.find(n.encode()) >= 0


def march_for(name, st):
    """March live registrations for a June table; roll rows use the variable-name registrations of InitStateCommonRandom."""
    if name in march:
        return march[name]
    if st['roll'] and name not in HYPER_NAMES:
        key = '<tableNameStartUTF8>' if re.search(r'_START$', name) else ('<tableNameWinUTF8>' if 'WIN' in name else '<tableNameLoseUTF8>')
        return march.get(key, [])
    return []


exe = open(EXE, 'rb').read()


HAND_PHASE = {}
LINE_PFX = (('CH1g2', r'^RVC_|^RSI_RVC_'), ('CH1e', r'^RBM_|^RSI_RBM_|^RSI_RST_|^RSI_SOUL_TAKER'), ('CH1f', r'^RRF_|^RSI_RRF_|^RSI_ROT_|^RSI_OVER_TAKER'),
            ('CH1g', r'^RWT_|^RSI_RWT_|^RSI_WEAPON_TAKER'))


def ch1_sub(name, st):
    """CH1 split by name (the plan's CH1 group table); returns (phase, why-ambiguous or '')."""
    if name in HAND_PHASE:
        return HAND_PHASE[name]
    if re.match(r'^USI_EVENT_(SNIPER|PROTON_BOMB)_', name):
        return 'CH1m', "base call 3202b110, CX2GUUser code, not Raven's: registration only"
    if name in HYPER_NAMES:
        return 'CH1a', 'hyper mode table named by 31fdd490'
    if name == 'RSI_RIDING_HYPER_MODE':
        return 'CH1c', 'RIDING_HYPER_MODE is both RIDING* (CH1c) and HYPER* (CH1a): CH1c chosen (Chung / Elsword precedent)'
    if any('InitPassiveSkillState' in h for h in st['how']):
        return 'CH2a', 'registered in InitPassiveSkillState 32c57660 (its registration lines are CH4a); the callbacks serve the Shadow Step skill 24200 family'
    if st['roll']:
        return 'CH1b', 'START / WIN / LOSE roll table (31fe22d0, INIT_COMMON_RANDOM_STATE)'
    for ph, rx in LINE_PFX:
        if re.match(rx, name):
            return ph, ''
    if re.match(r'^RSI_(RIDING|PEPPER|DANCE)', name) or name.startswith('RSI_AMUSEMENT_ITEM_PHOENIX_RUN'):
        return 'CH1c', ('AMUSEMENT_ITEM_PHOENIX_RUN is the riding family: CH1c (precedent), other AMUSEMENT* are CH1b' if 'AMUSEMENT' in name else '')
    if re.match(r'^RSI_(DAMAGE|STANDUP|DIE|START|WIN|LOSE|WALL|AMUSEMENT|FROZEN|TRANSFORMED|PARRYING)', name):
        return 'CH1b', ''
    if name in ('RSI_START_INTRUDE', 'RSI_WAIT_STATE_CHANGE') or re.search(r'COMMON_BUFF|THROW_ITEM', name):
        return 'CH1a', ''
    if re.match(r'^RSI_(WAIT|WALK|JUMP_(READY|UP|DOWN|LANDING)$|DASH$|DASH_END$|DASH_JUMP$|DASH_JUMP_LANDING$|DASH_JUMP_POWER_LANDING$|SUPER_DASH)', name):
        return 'CH1a', ''
    if re.match(r'^RSI_(COMBO|DASH_COMBO|DASH_JUMP_COMBO|JUMP_COMBO)', name):
        return 'CH1d', ''
    return 'UNASSIGNED', 'no name rule (registered at %s)' % '; '.join(st['how'][:2])


def state_phase(name, st):
    sk_ph = None
    for x in st['skills']:
        if x in sk:
            sk_ph = minph(sk_ph, skill_phase(x))
    reg = bool(st['roll'] or any('SetEquippedSkillFunc' not in h for h in st['how']) or st.get('head'))
    if re.match(r'^(RBM|RRF|RWT|RVC)_', name) and not (sk_ph and not reg):   # a prefixed combo table belongs to its line; a skill state follows its skill (CH0 lead 2026-11-05)
        for ph, rx in LINE_PFX:
            if re.match(rx, name):
                return ph, ('skill state: first job would be %s' % sk_ph if sk_ph and not reg else '')
    if reg:
        ph, why = ch1_sub(name, st)
        if ph == 'UNASSIGNED' and sk_ph:
            why += '; as a skill state it would be %s' % sk_ph
        return ph, why
    return sk_ph, ''


# the listing reader (ch0_skill_listing_emu.py -> ch0_skill_listing_rows.tsv) is the ground truth for 32d223e0: every state a skill id registers, with
# its state id, skill ids and callbacks. It fills the states the decompile extractor lost (a name held in a register: pcVar9, edi) and adds
# the callbacks / ids the decompile rows lack.
LST = collections.defaultdict(list)
_lp = os.path.join(HERE, 'ch0_skill_listing_rows.tsv')
if os.path.exists(_lp):
    for l in open(_lp, encoding='utf-8').read().splitlines()[1:]:
        f = (l.split('	') + [''] * 8)[:8]
        if f[3]:
            LST[f[3]].append(f)
LISTING_NEW = []
for tbl, rws in LST.items():
    st = june[tbl]
    ids = set()
    for f in rws:
        ids.update(int(x) for x in f[4].split(',') if x.isdigit())
    if not st['how']:
        sid_ = sorted({f[2] for f in rws if f[2] != '-'})
        st['how'].append('SetEquippedSkillFunc 32c641a0 listing @%s state id %s (name held in a register: the decompile extractor lost it)' % (
            ','.join(sorted({f[1] for f in rws})), ','.join(sid_) or '-'))
        LISTING_NEW.append(tbl)
    st['skills'].update(ids)
    have = set(re.findall(r'(\w+)=([0-9a-f]{8})>([0-9a-f]{8})', ' '.join(st['cb'])))
    add = []
    for f in rws:
        for t3 in re.findall(r'(\w+)=([0-9a-f]{8})>([0-9a-f]{8})', f[6]):
            if t3 not in have and not any(t3[0] == h[0] and t3[2] == h[2] for h in have):
                have.add(t3)
                add.append('%s=%s>%s' % t3)
    if add:
        st['cb'].append(' '.join(add))

# names present in the exe whose registration the extractor missed (variable-name registrations): referenced by push imm32 inside the skill
# switch; the nearest preceding referenced table of a registered skill case is the anchor and gives the skill ids
SKLO, SKHI = 0x32c641a0, 0x32c66dc0
script_names = sorted({n for n, _ in tabs})
xref_sites = []
for n in script_names:
    b = b'\0' + n.encode() + b'\0'
    j = exe.find(b)
    while j != -1:
        p_ = struct.pack('<I', j + 1 + B)
        k = exe.find(p_, 0, 0x1cbbe00)
        while k != -1:
            if SKLO <= k + B < SKHI:
                xref_sites.append((k + B, n))
            k = exe.find(p_, k + 1, 0x1cbbe00)
        j = exe.find(b, j + 1)
xref_sites.sort()
AUX = {}
for idx, (a, n) in enumerate(xref_sites):
    if june.get(n) and june[n]['how']:
        continue
    for a2, n2 in reversed(xref_sites[:idx]):
        if june.get(n2) and june[n2]['how'] and june[n2]['skills']:
            AUX.setdefault(n, []).append((a, n2))
            break
for n, lst in AUX.items():
    st = june[n]
    for a, anchor in lst:
        if n in LST:
            continue
        st['how'].append('SetEquippedSkillFunc 32c641a0 name xref @%08x (variable-name registration the extractor missed; ids and callbacks not extracted); anchor %s' % (a, anchor))
        st['skills'].update(june[anchor]['skills'])

rows, seen = [], set()
MEMO = set()
states = [t for t in tabs if not t[0].startswith('INIT_') and t[0] != 'PROMOTION_ITEM_ID_LIST']
SLOT_FN = {'Init': 'Init', 'StartFuture': 'StateStartFuture', 'Start': 'StateStart', 'FrameMoveFuture': 'FrameMoveFuture', 'FrameMove': 'FrameMove',
           'CameraMove': 'CameraMove', 'EventProcess': 'EventProcess', 'EndFuture': 'StateEndFuture', 'End': 'StateEnd'}
UNASSIGNED = []
HN = {}   # optional ch0_hand_names.tsv: fn, proposed_name, why ('inside: ...' = the function is already in March)
_hp = os.path.join(HERE, 'ch0_hand_names.tsv')
if os.path.exists(_hp):
    for l in open(_hp, encoding='utf-8').read().splitlines()[1:]:
        f = (l.split('	') + ['', '', ''])[:3]
        if re.fullmatch(r'[0-9a-f]{8}', f[0]):
            HN[f[0]] = (f[1], f[2])
CBINFO = {}   # target fn -> list of (state, slot, march)


def callbacks(cbstr):
    return re.findall(r'(\w+)=([0-9a-f]{8})>([0-9a-f]{8})', cbstr)


def build_row(name, line, dup):
    st = june.get(name)
    if st and st['how']:
        ph, why = state_phase(name, st)
        if ph is None:
            ph, why = 'UNASSIGNED', 'no skill id in the tree and no registration phase'
        mrows = march_for(name, st)
        if mrows:
            verdict = 'pair'
        elif in_march_script(name) or msrc.find(b'"' + name.encode() + b'"') >= 0:
            verdict = 'pair (March reads it by name outside MakeTableReference / March script has the table)'
        else:
            verdict = 'june-only'
        how = ' | '.join(dict.fromkeys(st['how']))
        cb = ' | '.join(dict.fromkeys(st['cb']))
        skills = ','.join('%d %s (t%d)' % (x, sk[x][0], sk[x][1]) for x in sorted(st['skills']) if x in sk)
        if st['notes']:
            verdict += '; ' + '; '.join(st['notes'])
        if why:
            verdict += '; PHASE NOTE: ' + why
    else:
        ph, verdict, how, cb, skills, mrows = 'n/a', 'june-dead-data', 'no reference in June C++ (string absent from the exe)', '', '', []
        st = None
        if exe.find(b'\0' + name.encode() + b'\0') >= 0:
            how = 'string in the exe but no registration found: read its xrefs'
    # per-callback inside / new
    mcb = {}
    for ln, cbs in mrows:
        for c in cbs:
            s, rest = c.split('=', 1)
            mcb.setdefault(s, (ln, rest))
    inside, prop = [], []
    for slot, th, tgt in callbacks(cb):
        t = int(tgt, 16)
        if not (LO <= t < HI):
            inside.append('%s=shared (target %s outside the Raven range)' % (slot, tgt))
            prop.append('%s=-' % slot)
            continue
        if slot in mcb:
            ln, rest = mcb[slot]
            fnm, cl = rest.split('@')[0], rest.split('@')[-1] if '@' in rest else '?'
            inside.append('%s=inside X2GURaven.cpp:%s %s' % (slot, cl.rstrip(')'), fnm))
            prop.append('%s=-' % slot)
        elif tgt in HN and HN[tgt][1].startswith('inside:'):
            inside.append('%s=inside (hand names) %s' % (slot, HN[tgt][0]))
            prop.append('%s=-' % slot)
        else:
            nm = HN[tgt][0] if tgt in HN else name + '_' + SLOT_FN[slot]
            inside.append('%s=new' % slot)
            prop.append('%s=%s' % (slot, nm))
    if dup:
        verdict += '; duplicate table (June script defines it twice, the second wins: JUNE_BUGS candidate)'
    mtxt = ' ; '.join('%d:%s' % (ln, ','.join(c.replace('::', '::', 1) for c in cbs)) for ln, cbs in mrows) or '-'
    if ph == 'UNASSIGNED':
        UNASSIGNED.append((name, verdict))
    return (name, str(line), how, skills, cb, mtxt, verdict, ph, ' | '.join(inside) or '-', ' | '.join(prop) or '-'), st


seen = set()
for name, line in states:
    dup = name in seen
    seen.add(name)
    r, st = build_row(name, line, dup)
    rows.append(r)
names_in_script = set(n for n, _ in states)
for name in sorted(june):
    if name not in names_in_script and june[name]['how']:
        r, st = build_row(name, '-', False)
        r = list(r)
        r[6] = 'june-cpp-no-table (June C++ registers it, June script has no table: JUNE_BUGS candidate); ' + r[6]
        rows.append(tuple(r))
for name in sorted(march):
    if name.startswith('<'):
        continue
    if name not in names_in_script and name not in june:
        cbs = ' ; '.join('%d:%s' % (ln, ','.join(c)) for ln, c in march[name])
        rows.append((name, '-', 'March only', '', '', cbs, 'march-only', 'n/a', '-', '-'))

with open(os.path.join(HERE, 'ch0_state_skill.tsv'), 'w', encoding='utf-8', newline='\n') as o:
    o.write('# part 1: states (June script tables + June registrations with no table + March-only states)\n')
    o.write('state\tjune_script_line\tjune_registration\tskills\tjune_callbacks (thunk>target)\tmarch (cpp line:callbacks Class::Fn@line)\tverdict\tphase\t'
            'inside_new (per callback; inside = March live registration of the same state gives a callback in the same slot)\t'
            'proposed_name (per new callback)\n')
    for r in rows:
        o.write('\t'.join(r) + '\n')
    o.write('# part 2: skill ids (%d June Raven ids from s3_reform_ids_no_state.tsv)\n' % len(sk))
    o.write('skill_id\tname\ttype\tclasses\tjune_state(s)\tphase\tnote\n')
    SKROWS = []
    unmapped_sk = []
    for sid in sorted(sk):
        name, typ, cls = sk[sid]
        note = ''
        sts = sorted({n for n, st in june.items() if sid in st['skills']})
        if not sts and sid in base_of:
            sts = sorted({n for n, st in june.items() if base_of[sid] in st['skills']})
            note = 'switch value = base %d (m_DeleteSkillIDByUpgrade, 32207210)' % base_of[sid]
        ph = skill_phase(sid)
        if not sts:
            if typ in (1, 2, 3, 4, 8):
                note = 'passive (type %d): no state; buff / passive data (InitPassiveSkillState 32c57660), CH4a' % typ
            elif typ == 12:
                sts = ['RSI_THROW_ITEM']
                note = 'type 12: case 12 -> ESSI_THROW_ITEM (id 900)'
            else:
                note = 'NO STATE FOUND'
                unmapped_sk.append(sid)
        SKROWS.append((sid, name, typ, cls, sts, ph))
        o.write('%d\t%s\t%d\t%s\t%s\t%s\t%s\n' % (sid, name, typ, ','.join(map(str, sorted(cls))), ';'.join(sts), ph, note))
    c = collections.Counter((r[6].split(' (')[0].split(';')[0], r[7]) for r in rows)
    o.write('# totals states: %s\n' % ', '.join('%s/%s=%d' % (k[0], k[1], v) for k, v in sorted(c.items())))
    o.write('# script tables: %d (%d distinct names): %d state tables, %d INIT_*, %d PROMOTION_ITEM_ID_LIST\n' % (
        len(tabs), len(set(n for n, _ in tabs)), len(states), sum(1 for t in tabs if t[0].startswith('INIT_')), sum(1 for t in tabs if t[0] == 'PROMOTION_ITEM_ID_LIST')))
    mapped_tabs = sum(1 for r in rows if r[1] != '-' and r[7] not in ('n/a', 'UNASSIGNED'))
    una_tabs = [r[0] for r in rows if r[1] != '-' and r[7] == 'UNASSIGNED']
    dead_tabs = [r[0] for r in rows if r[1] != '-' and r[7] == 'n/a']
    o.write('# tables mapped (phase assigned) %d, UNASSIGNED %d, dead n/a %d, plus INIT_* / PROMOTION_ITEM_ID_LIST %d (not states) = %d / %d\n' % (
        mapped_tabs, len(una_tabs), len(dead_tabs), len(tabs) - len(states), mapped_tabs + len(una_tabs) + len(dead_tabs) + len(tabs) - len(states), len(tabs)))
    o.write('# dead tables: %s\n' % ' '.join(dead_tabs))
    o.write('# unassigned: %s\n' % ' '.join(una_tabs))
    o.write('# march-only: %s\n' % ' '.join(r[0] for r in rows if r[6] == 'march-only'))
    fn_ph = {}
    for name, st in june.items():
        if not st['how']:
            continue
        p, _ = state_phase(name, st)
        for cb in st['cb']:
            for t in re.findall(r'>([0-9a-f]{8})', cb):
                if LO <= int(t, 16) < HI:
                    if t not in fn_ph or ORDER.index(p or 'UNASSIGNED') < ORDER.index(fn_ph[t]):
                        fn_ph[t] = p or 'UNASSIGNED'
    fc = collections.Counter(fn_ph.values())
    o.write('# totals distinct Raven-range state functions by phase: %s\n' % ', '.join('%s=%d' % kv for kv in sorted(fc.items(), key=lambda kv: ORDER.index(kv[0]))))
    sc = collections.Counter(r2[5] for r2 in SKROWS)
    o.write('# totals skills by phase: %s\n' % ', '.join('%s=%d' % kv for kv in sorted(sc.items())))
    o.write('# skill ids mapped (state, or passive / buff with no state) %d / %d; unmapped: %s\n' % (len(sk) - len(unmapped_sk), len(sk), unmapped_sk))
print(open(os.path.join(HERE, 'ch0_state_skill.tsv'), encoding='utf-8').read().split('# totals', 1)[1])
print('UNASSIGNED', len(UNASSIGNED))
