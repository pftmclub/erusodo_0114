r"""char-elesis CH0: build ch0_state_skill.tsv (June Elesis states x skills x March). Copy of char_ara/ch0_build_state_skill.py
with Elesis's registrars, script (ELESIS_KNIGHT.src.lua), classes (8 / 29 / 30 / 35 / 120 / 121 / 125) and the sub-phase rules of the CH0 brief.

    ch0_reg.tsv, ch0_skill.tsv, ch0_skill_listing_rows.tsv, ch0_march_live_states.tsv are already built (registrars: wrapper 32b87a60, InitState parts 32b81be0 /
    32b864f0, base event states 3202b110; skill switch 32b8c470). Then:
    python june2017_re/char_elesis/ch0_build_state_skill.py

Phase rules. Registered non-skill states, by name: CH1a movement and slot-head (ESI_WAIT*, HYPER_WAIT, WALK, JUMP* (the jump attacks included, they are not COMBO),
dash basics ESI_DASH / DASH_END / DASH_JUMP / DASH_JUMP_LANDING / *_POWER_LANDING without COMBO, ESI_THROW_ITEM, COMMON_BUFF, the HYPER_MODE tables incl.
ESI_EGM_ / EBH_ / ECA_HYPER_MODE, charge, the QUICK group); CH1b reactions (DAMAGE*, STANDUP*, DIE*, START*, WIN / LOSE / DRAW, WALL*, AMUSEMENT*, FROZEN,
TRANSFORMED); CH1c RIDING* / PEPPER* / DANCE*; CH1d ESI_COMBO* with no job tag; CH1d2 ESI_DASH_COMBO* / ESI_DASH_JUMP_COMBO* with no job tag; CH1m the USI_ rows.
A table with a job tag (SSK / SGM -> CH1e, SPK / SBH -> CH1f, SDK / SCA -> CH1g) and no skill marker (SA / A / HA / P / SI) goes to its line group whoever
registers it. A skill state goes to the earliest phase of the skills that enter it (earliest class: 8 CH2a, 29 CH2b, 30 CH2c, 35 CH2d, 120 CH3a, 121 CH3b,
125 CH3c; hyper active (13) CH3d; passive CH4a; id 900 (ESI_THROW_ITEM, type 12) CH1a). Anything unplaceable is UNASSIGNED.
"""
import re, os, collections, struct

HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..'))
LO, HI = 0x32b4e770, 0x32b90b70
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
B = 0x31aa0c00
CORE_FN = {'32b81be0': 'InitState part 32b81be0', '32b864f0': 'InitState part 32b864f0', '32b87a60': 'InitState wrapper 32b87a60', '3202b110': 'CX2GUUser event states 3202b110'}
ORDER = ['CH1a', 'CH1b', 'CH1c', 'CH1d', 'CH1d2', 'CH1e', 'CH1f', 'CH1f2', 'CH1g', 'CH1g2', 'CH1g3', 'CH1h', 'CH1i', 'CH1j', 'CH1k', 'CH1m', 'CH1n', 'CH1o', 'CH1p', 'CH1q', 'CH1q2', 'CH1',
         'CH2a', 'CH2b0', 'CH2b', 'CH2b2', 'CH2c0', 'CH2c', 'CH2c2', 'CH2d0', 'CH2d', 'CH2d2', 'CH2d3', 'CH3a', 'CH3a2', 'CH3b0', 'CH3b', 'CH3b2', 'CH3c0', 'CH3c', 'CH3c2', 'CH3d', 'CH4a', 'CH4b', 'CH4c', 'UNASSIGNED', 'n/a']
CLS_PH = {8: 'CH2a', 29: 'CH2b', 30: 'CH2c', 35: 'CH2d', 120: 'CH3a', 121: 'CH3b', 125: 'CH3c'}

src = open(os.path.join(T, 'june_src_fixed/data036_kom/ELESIS_KNIGHT.src.lua'), encoding='utf-8', errors='replace').read()
tabs = [(m.group(1), src[:m.start()].count('\n') + 1) for m in re.finditer(r'^([A-Za-z_]\w*)\s*=\s*\{', src, re.M)]
i = src.index('INIT_COMMON_RANDOM_STATE')
rnd = set(re.findall(r'"(ESI_\w+)"', src[i:src.index('\n}\n', i)]))

sk = {}
for l in open(os.path.join(T, 'june2017_re/s3_reform_ids_no_state.tsv'), encoding='utf-8').read().splitlines()[1:]:
    f = l.split('\t')
    if f[2] != 'X2GUEL.cpp':
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


VC_SPLIT = set()


def skill_phase(sid):
    name, typ, cls = sk[sid]
    if typ in (1, 2, 3, 4, 8):
        return 'CH4a'
    if typ == 12:
        return 'CH1a'   # type 12 = ESI_THROW_ITEM (id 900, case 12 of the type switch): a movement-group state, the rule applies
    if typ == 13:
        return 'CH3d'
    if sid in VC_SPLIT:
        return 'CH3c2'
    return min((CLS_PH[c] for c in cls if c in CLS_PH), key=ORDER.index)


june = collections.defaultdict(lambda: {'how': [], 'cb': [], 'skills': set(), 'gates': [], 'ungated': False, 'roll': None, 'notes': []})
NOTES = []


def expand_cb(cb):
    """slot=A>B|C>D (two arms of one slot, Night Watcher takes the second) -> slot=A>B slot=C>D"""
    return re.sub(r'(\w+)=((?:[0-9a-f]{8}>[0-9a-f]{8})(?:\|[0-9a-f]{8}>[0-9a-f]{8})+)', lambda m: ' '.join('%s=%s' % (m.group(1), a) for a in m.group(2).split('|')), cb)


def minph(a, b):
    if a is None:
        return b
    if b is None:
        return a
    return a if ORDER.index(a) <= ORDER.index(b) else b


regrows = [l.split('\t') + [''] * 5 for l in open(os.path.join(HERE, 'ch0_reg.tsv')).read().splitlines()[1:]]
for r_ in regrows:
    r_[4] = expand_cb(r_[4])
roll_cb = {}
hyp_cb = None
HYPER_NAMES = ('ESI_HYPER_MODE', 'ESI_EGM_HYPER_MODE', 'ESI_EBH_HYPER_MODE', 'ESI_ECA_HYPER_MODE')   # the hyper tables 31fdd490 names (push @31fdd542 / 31fdd676 / 31fdd680 / 31fdd68a); no berserk table: ESI_BERSERK_MODE is not in the exe
for fn, line, sid, name, cb in [r[:5] for r in regrows]:
    if name == 'puVar4':
        hyp_cb = cb
        continue
    if name.startswith('ppp'):   # char-elesis CH0: the decompile names the roll rows ppppuVar6
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
    june[n]['how'].append(('CX2GUUser hyper mode 31fdd490 (InitState 32b81be0@687 id 23; the callbacks are the puVar4 row, shared by the class variants)'))
    june[n]['cb'].append(hyp_cb)
    june[n]['roll'] = True

# skill switch 32b8c470: head cases 5 (COMMON_BUFF) / 12 (THROW_ITEM) / default, the type switch row '6,7,13,14,15,16' is only a lead-in
# to the hyper sub-switch (skipped), then the id cases. The 'default' row that carries a table is MEGASLASH_MEMO_READY (ids come from the listing reader).
SKROWS_CASES = []
for l in open(os.path.join(HERE, 'ch0_skill.tsv')).read().splitlines()[1:]:
    f = (l.split('\t') + [''] * 10)[:10]
    fn, line, cases, nt, ht, ncb, hcb, extra, calls, xcb = f
    ncb, hcb, xcb = expand_cb(ncb), expand_cb(hcb), expand_cb(xcb)
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
    names = [x for x in (nt, ht) if x] + [e.split(':', 1)[1] for e in extra.split() if ':' in e and e.split(':', 1)[1].startswith(('ESI_', 'USI_', 'SI_'))]
    names = [y for x in names for y in x.split(';')]   # char-elesis CH0: the extractor joins the two arms of a memo choice with ';'
    xname = {e.split(':', 1)[0]: e.split(':', 1)[1] for e in extra.split()}
    for xc in (xcb.split(' | ') if xcb else []):
        sid_, cb = xc.split(':', 1)
        if sid_ in xname and cb.strip() not in ('-', '') and not cb.startswith('copy='):
            june[xname[sid_]]['cb'].append(cb.strip())
    SKROWS_CASES.append((line, cases, names, ids, note))
    for n in names:
        st = june[n]
        st['how'].append('SetEquippedSkillFunc 32b8c470@%s case %s' % (line, cases))
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
mscript = open(os.path.join(MKOM, 'data036_kom', 'ELESIS_KNIGHT.lua'), 'rb').read()
msrc = open(os.path.join(T, 'X2Lib/X2GUEl.cpp'), 'rb').read() + open(os.path.join(T, 'X2Lib/X2GUUser.cpp'), 'rb').read()


def in_march_script(n):
    return mscript.find(n.encode()) >= 0


def march_for(name, st):
    """March live registrations for a June table; roll rows use the variable-name registrations of InitStateCommonRandom."""
    if name in march:
        return march[name]
    if st['roll'] and name not in HYPER_NAMES:
        key = '<strStartTableNameUTF8>' if re.search(r'_START$', name) else ('<strWinTableNameUTF8>' if 'WIN' in name else '<strLoseTableNameUTF8>')
        return march.get(key, [])
    return []


exe = open(EXE, 'rb').read()


# char-elesis CH0 hand phases (none: open rows are UNASSIGNED for the caller)
HAND_PHASE = {   # char-elesis CH0 Opus: the four names no rule placed
    'ESI_CAN_NOT_ATTACK_JUMP_DOWN': ('CH1a', 'CH0 Opus: jump-down movement state (InitState id 186)'),
    'ESI_SWORD_WAIT': ('CH1a', 'CH0 Opus: wait state (id 187; PlayAnimationSwordWaitStart 32b6e0c0)'),
    'ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE': ('CH1k', 'CH0 Opus: Endurance Power (28201 / 28218) has no switch state: entered from NoStateChangeActionFrameMove 32b818b0 -> 32b80c60 (ids 205 / 207)'),
    'ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE_MEMO': ('CH1k', 'CH0 Opus: as ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE (id 206)'),
}


def split_sub(name, ph, st=None):
    """char-elesis CH0 splits of groups over ~45 functions (rules in SPLIT, counts in the Done note)"""
    return SPLIT[ph](name, minid_of(st) if st else None) if ph in SPLIT else ph


def minid_of(st):
    ids = [x for x in st['skills'] if x in sk]
    return min(ids) if ids else st.get('_minid')


# char-elesis CH0 Opus splits (over ~25 functions with most listing-only): by job line for CH1f / CH1g, by skill (actives) for CH2b / CH3a / CH3b
SPLIT = {
    'CH1f': lambda n, i: 'CH1f2' if 'SBH' in n.split('_') else 'CH1f',
    'CH1g': lambda n, i: 'CH1g2' if 'SCA' in n.split('_') else 'CH1g',
    'CH2b': lambda n, i: 'CH2b2' if i in (28202, 28203, 28204) else 'CH2b',
    'CH3a': lambda n, i: 'CH3a2' if i in (28205, 28206, 28222) else 'CH3a',
    'CH3b': lambda n, i: 'CH3b2' if i in (28210, 28211, 28223) else 'CH3b',
}
LINE_TOK = (('CH1e', {'SSK', 'SGM'}), ('CH1f', {'SPK', 'SBH'}), ('CH1g', {'SDK', 'SCA'}))
SKILL_MARK = {'SA', 'A', 'HA', 'SI', 'P'}


def line_of(name):
    """CH1 line groups whose token the name carries (one = the line, several = ambiguous)"""
    t = name.split('_')
    return [ph for ph, S in LINE_TOK if any(x in S for x in t)]


def line_prefixed(name):
    t = name.split('_')
    return len(t) > 1 and t[1] not in SKILL_MARK and bool(line_of(name))


def ch1_sub(name, st):
    """CH1 split by name (the plan's CH1 group table); returns (phase, why-ambiguous or '')."""
    if name in HAND_PHASE:
        return HAND_PHASE[name]
    if name.startswith('USI_'):
        return 'CH1m', "base call 3202b110, CX2GUUser code, not Elesis's: registration only"
    if name in HYPER_NAMES:
        return 'CH1a', 'hyper mode table named by 31fdd490'
    if name == 'ESI_RIDING_HYPER_MODE':
        return 'CH1c', 'RIDING_HYPER_MODE is both RIDING* (CH1c) and HYPER* (CH1a): CH1c chosen (Chung / Elsword / Raven / Rena / Ara precedent)'
    if st['roll']:
        return 'CH1b', 'START / WIN / LOSE roll table (31fe22d0, INIT_COMMON_RANDOM_STATE)'
    ln = line_of(name)
    if len(ln) > 1:
        return 'UNASSIGNED', 'name carries tokens of two lines (%s)' % ' '.join(ln)
    if ln:
        return ln[0], ''
    if re.match(r'^ESI_(RIDING|PEPPER|DANCE)', name) or name.startswith('ESI_AMUSEMENT_ITEM_PHOENIX_RUN'):
        return 'CH1c', ('AMUSEMENT_ITEM_PHOENIX_RUN is the riding family: CH1c (precedent), other AMUSEMENT* are CH1b' if 'AMUSEMENT' in name else '')
    if re.match(r'^ESI_(DAMAGE|STANDUP|DIE|START|WIN|LOSE|WALL|AMUSEMENT|FROZEN|TRANSFORMED)', name):
        return 'CH1b', ''
    if name.startswith('ESI_DASH'):
        return ('CH1d2', '') if 'COMBO' in name else ('CH1a', 'dash basic (no COMBO)')
    if name.startswith('ESI_JUMP_ATTACK'):
        return 'CH1a', 'JUMP* prefix: the jump attacks are not COMBO-named; CH1d would also fit (caller decides)'
    if re.match(r'^ESI_(WAIT|HYPER|WALK|JUMP|THROW|CHARGE|QUICK)', name) or re.search(r'COMMON_BUFF', name):
        return 'CH1a', ''
    if name.startswith('ESI_COMBO'):
        return 'CH1d', ''
    return 'UNASSIGNED', 'no name rule (registered at %s)' % '; '.join(st['how'][:2])


def stem_phase(name):
    """phase of the skill state(s) whose name is a prefix (>= 4 tokens) of `name`: the follow-up state of that skill"""
    t = name.split('_')
    for k in range(len(t) - 1, 3, -1):
        pre = '_'.join(t[:k])
        ms = sorted(n for n, st in june.items() if n != name and st['how'] and (n == pre or n.startswith(pre + '_')) and any(x in sk for x in st['skills']))
        if ms:
            ph = None
            mi = None
            for n in ms:
                for x in june[n]['skills']:
                    if x in sk:
                        ph = minph(ph, skill_phase(x))
                        mi = x if mi is None else min(mi, x)
            return ph, ms, mi
    return None, [], None


def state_phase(name, st):
    if name in HAND_PHASE:
        return HAND_PHASE[name]
    ph, why = state_phase_raw(name, st)
    return split_sub(name, ph, st) if ph else ph, why


def state_phase_raw(name, st):
    sk_ph = None
    for x in st['skills']:
        if x in sk:
            sk_ph = minph(sk_ph, skill_phase(x))
    reg = bool(st['roll'] or any('SetEquippedSkillFunc' not in h and 'vslot 0x68c' not in h for h in st['how']) or st.get('head'))
    if line_prefixed(name) and name not in HYPER_NAMES:   # a line-prefixed table belongs to its line whoever registers it; a skill-marked state follows reg / its skill
        ln = line_of(name)
        if len(ln) == 1:
            return ln[0], ('skill state: first job would be %s' % sk_ph if sk_ph and not reg else '')
    t_ = name.split('_')
    if reg and len(t_) > 2 and t_[1] in ('SA', 'A', 'HA') and name not in HYPER_NAMES:   # char-elesis CH0: a skill-marked state InitState registers follows its skill
        if sk_ph:
            return sk_ph, 'registered by InitState and entered by skills: phase of the skills'
        ph, ms, mi = stem_phase(name)
        st['_minid'] = mi
        if ph:
            return ph, 'registered by InitState, no skill id: phase inferred from the stem (skill state %s)' % ' '.join(ms[:2])
        if 'ENDURANCE_POWER_ACTIVE' in name:
            return 'UNASSIGNED', 'skill-marked state registered by InitState: no skill id (switch exit for 28201 / 28218 RNSI_A_EL_ENDURANCE_POWER, CH2a by name only) and no stem match'
        return 'UNASSIGNED', 'skill-marked state registered by InitState: no skill id and no stem match'
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
    for fn_ in sorted({f[0] for f in rws}):
        rf = [f for f in rws if f[0] == fn_]
        lab = 'SetEquippedSkillFunc 32b8c470' if fn_ == '32b8c470' else 'vslot 0x68c skill-state registration %s' % fn_
        sid_ = sorted({f[2] for f in rf if f[2] != '-'})
        how_ = '%s listing @%s state id %s (state set up through a class helper / a name the decompile extractor lost)' % (
            lab, ','.join(sorted({f[1] for f in rf})), ','.join(sid_) or '-')
        if how_ not in st['how'] and not (fn_ == '32b8c470' and any('SetEquippedSkillFunc' in h for h in st['how'])):
            st['how'].append(how_)
    if tbl not in LISTING_NEW and not any(h for h in st['how'] if 'case' in h or 'InitState' in h or 'roll' in h):
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
SKLO, SKHI = 0x32b8c470, 0x32b8eabc
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
        st['how'].append('SetEquippedSkillFunc 32b8c470 name xref @%08x (variable-name registration the extractor missed; ids and callbacks not extracted); anchor %s' % (a, anchor))
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
            inside.append('%s=shared (target %s outside the Elesis range)' % (slot, tgt))
            prop.append('%s=-' % slot)
            continue
        if slot in mcb:
            ln, rest = mcb[slot]
            fnm, cl = rest.split('@')[0], rest.split('@')[-1] if '@' in rest else '?'
            inside.append('%s=inside X2GUEl.cpp:%s %s' % (slot, cl.rstrip(')'), fnm))
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
    o.write('# part 2: skill ids (%d June Elesis ids from s3_reform_ids_no_state.tsv)\n' % len(sk))
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
                note = 'passive (type %d): no state; buff / passive data (InitPassiveSkillState, registers nothing), CH4a' % typ
            elif typ == 12:
                sts = ['ESI_THROW_ITEM']
                note = 'type 12: case 12 -> ESI_THROW_ITEM (id 900)'
            elif sid in (28201, 28218):
                note = ('exit case of the id switch (32b8ea89, ch0_skill_switches.tsv: nothing registered): the skill is a buff; the states ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE / _MEMO '
                        '(InitState ids 205-207) carry its name and are entered from elsewhere (candidate, not proven)')
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
    o.write('# totals distinct Elesis-range state functions by phase: %s\n' % ', '.join('%s=%d' % kv for kv in sorted(fc.items(), key=lambda kv: ORDER.index(kv[0]))))
    sc = collections.Counter(r2[5] for r2 in SKROWS)
    o.write('# totals skills by phase: %s\n' % ', '.join('%s=%d' % kv for kv in sorted(sc.items())))
    o.write('# skill ids mapped (state, or passive / buff with no state) %d / %d; unmapped: %s\n' % (len(sk) - len(unmapped_sk), len(sk), unmapped_sk))
print(open(os.path.join(HERE, 'ch0_state_skill.tsv'), encoding='utf-8').read().split('# totals', 1)[1])
print('UNASSIGNED', len(UNASSIGNED))
