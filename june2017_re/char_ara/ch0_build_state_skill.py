r"""char-ara CH0: build ch0_state_skill.tsv (June Ara states x skills x March). Copy of char_rena/ch0_build_state_skill.py
with Ara's registrars, script, classes and the sub-phase rules of the CH0 brief (Phase CH1 group table of JUNE_CHARACTER_ARA_PHASES.md).

    python june2017_re/charport_states.py init 32bca580 32bcf4a0 3202b110 32bcced0 > june2017_re/char_ara/ch0_reg.tsv   (already built; do not regenerate)
        (32bcf4a0 InitState wrapper = START / WIN / LOSE roll rows 9 / 11 / 12 + id 130, 32bca580 InitState body = 112 registrations + the hyper rows 23 / 155,
        3202b110 base event states, 32bcced0 = 41 per-class registrations; InitPassiveSkillState 32bcd9c0 registers nothing)
    python june2017_re/charport_states.py skill 32bd09d0                  > june2017_re/char_ara/ch0_skill.tsv
    python june2017_re/char_ara/ch0_skill_listing_emu.py <scratch>/sw.txt        (funcdis.py --func 32bd09d0; follows the class state helpers)
    python june2017_re/char_ara/ch0_march_live_states.py <ara.i>        (s3_no_state.py --keep-i, cl /P of X2GUAra.cpp)
    python june2017_re/char_ara/ch0_build_state_skill.py

Phase rules. States registered by InitState / the roll / 3202b110 / hyper mode 31fdd490 / berserk 31fdd800 are CH1, split by name (ch1_sub): CH1a movement
(ASI_WAIT*, HYPER_WAIT, WALK, JUMP*, DOUBLE*, THROW*, COMMON_BUFF, HYPER* / BERSERK* mode tables, AIRIAL*), CH1b reactions (DAMAGE, STANDUP, DIE, START, WIN /
LOSE, WALL, AMUSEMENT, FROZEN, TRANSFORMED), CH1c RIDING / PEPPER / DANCE / Phoenix run, CH1d ASI_COMBO*, CH1d2 ASI_DASH*, CH1m the USI_ rows. A line-prefixed
table (a line token and no skill marker SA / A / HA / SI* / P in second position) goes to its line's group whoever registers it: CH1e ALH / ASD, CH1f ALD / AYR,
CH1g ALS / AA / ASR. A registered state with a skill marker and no skill id is placed by its stem (the skill state it extends) or is UNASSIGNED; a P-marked
registered state with a line token follows its line. A skill state goes to the earliest phase of the skills that enter it: passive types CH4a, hyper active
(13) CH3d, type 12 (ASI_THROW_ITEM, id 900) CH1a, other actives by the earliest class that has the skill (7 CH2a, 28 CH2b, 31 CH2c, 34 CH2d, 118 CH3a,
119 CH3b, 124 CH3c). Groups over ~45 functions are split by split_sub().
"""
import re, os, collections, struct

HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..'))
LO, HI = 0x32b90b70, 0x32bd86f0
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
B = 0x31aa0c00
CORE_FN = {'32bca580': 'InitState body 32bca580', '32bcf4a0': 'InitState wrapper 32bcf4a0', '32bcced0': 'InitState per-class 32bcced0', '3202b110': 'CX2GUUser event states 3202b110'}
ORDER = ['CH1a', 'CH1b', 'CH1c', 'CH1d', 'CH1d2', 'CH1e', 'CH1f', 'CH1f2', 'CH1g', 'CH1g2', 'CH1g3', 'CH1h', 'CH1i', 'CH1j', 'CH1k', 'CH1m', 'CH1n', 'CH1o', 'CH1p', 'CH1q', 'CH1q2', 'CH1',
         'CH2a', 'CH2b0', 'CH2b', 'CH2b2', 'CH2c0', 'CH2c', 'CH2c2', 'CH2d0', 'CH2d', 'CH2d2', 'CH2d3', 'CH3a', 'CH3a2', 'CH3b', 'CH3c0', 'CH3c', 'CH3c2', 'CH3d', 'CH4a', 'CH4b', 'UNASSIGNED', 'n/a']
CLS_PH = {7: 'CH2a', 28: 'CH2b', 31: 'CH2c', 34: 'CH2d', 118: 'CH3a', 119: 'CH3b', 124: 'CH3c'}

src = open(os.path.join(T, 'june_src_fixed/data036_kom/Ara_Martial_Artist.src.lua'), encoding='utf-8', errors='replace').read()
tabs = [(m.group(1), src[:m.start()].count('\n') + 1) for m in re.finditer(r'^([A-Za-z_]\w*)\s*=\s*\{', src, re.M)]
i = src.index('INIT_COMMON_RANDOM_STATE')
rnd = set(re.findall(r'"(ASI_\w+)"', src[i:src.index('\n}\n', i)]))

sk = {}
for l in open(os.path.join(T, 'june2017_re/s3_reform_ids_no_state.tsv'), encoding='utf-8').read().splitlines()[1:]:
    f = l.split('\t')
    if f[2] != 'X2GUAra.cpp':
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
        return 'CH1a'   # type 12 = ASI_THROW_ITEM (id 900, case 12 of the type switch): a movement-group state, the rule applies
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
HYPER_NAMES = ('ASI_HYPER_MODE', 'ASI_ASD_HYPER_MODE', 'ASI_AYR_HYPER_MODE', 'ASI_AA_HYPER_MODE',   # the hyper tables 31fdd490 names (push @31fdd53a / 31fdd657 / 31fdd661 / 31fdd66b)
               'ASI_BERSERK_MODE', 'ASI_ASD_BERSERK_MODE', 'ASI_AYR_BERSERK_MODE', 'ASI_AA_BERSERK_MODE')   # the berserk tables 31fdd800 names (push @31fdd874 / 31fdd8b8 / 31fdd8af / 31fdd8a6)
for fn, line, sid, name, cb in [r[:5] for r in regrows]:
    if name == 'puVar4':
        hyp_cb = cb
        continue
    if name.startswith('ppp'):   # char-ara CH0: the decompile names the roll rows pppuVar5
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
    june[n]['how'].append(('CX2GUUser berserk mode 31fdd800 (InitState 32bca580@775 id 155; the callbacks are the puVar4 row)' if 'BERSERK' in n else 'CX2GUUser hyper mode 31fdd490 (InitState 32bca580@737 id 23; the callbacks are the puVar4 row, shared by the class variants)'))
    june[n]['cb'].append(hyp_cb)
    june[n]['roll'] = True

# skill switch 32bd09d0: head cases 5 (COMMON_BUFF) / 12 (THROW_ITEM) / default, the type switch row '6,7,13,14,15,16' is only a lead-in
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
    names = [x for x in (nt, ht) if x] + [e.split(':', 1)[1] for e in extra.split() if ':' in e and e.split(':', 1)[1].startswith(('ASI_', 'USI_'))]
    names = [y for x in names for y in x.split(';')]   # char-ara CH0: the extractor joins the two arms of a memo choice with ';'
    xname = {e.split(':', 1)[0]: e.split(':', 1)[1] for e in extra.split()}
    for xc in (xcb.split(' | ') if xcb else []):
        sid_, cb = xc.split(':', 1)
        if sid_ in xname and cb.strip() not in ('-', '') and not cb.startswith('copy='):
            june[xname[sid_]]['cb'].append(cb.strip())
    SKROWS_CASES.append((line, cases, names, ids, note))
    for n in names:
        st = june[n]
        st['how'].append('SetEquippedSkillFunc 32bd09d0@%s case %s' % (line, cases))
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
mscript = open(os.path.join(MKOM, 'data036_kom', 'Ara_Martial_Artist.lua'), 'rb').read()
msrc = open(os.path.join(T, 'X2Lib/X2GUAra.cpp'), 'rb').read() + open(os.path.join(T, 'X2Lib/X2GUUser.cpp'), 'rb').read()


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


# char-ara CH0 hand phases (none: Rena's rows do not apply to Ara; open rows are UNASSIGNED for the caller)
HAND_PHASE = {}


def split_sub(name, ph, st=None):
    """char-ara CH0 splits of groups over ~45 functions (rules in SPLIT, counts in the Done note)"""
    return SPLIT[ph](name, minid_of(st) if st else None) if ph in SPLIT else ph


def minid_of(st):
    ids = [x for x in st['skills'] if x in sk]
    return min(ids) if ids else st.get('_minid')


# char-ara CH0 splits (groups over ~45 functions in the class map; the key is the smallest skill id of the state's skills, SA tiers 27000-27099, A tier 27200+):
#   CH2b: SA < 27200 | A CH2b2     CH2c: SA | A CH2c2     CH2d: ids < 27024 and states with no skill | 27024-27199 CH2d2 | A CH2d3
#   CH3a: ids < 27012 and states with no skill | the rest CH3a2     CH3c: SA | A CH3c2.   CH2a (50: an SA / A split leaves 6, so whole), CH1h (48), CH3b (44) stay whole.
SPLIT = {
    'CH2b': lambda n, i: 'CH2b2' if i and i >= 27200 else 'CH2b',
    'CH2c': lambda n, i: 'CH2c2' if i and i >= 27200 else 'CH2c',
    'CH2d': lambda n, i: ('CH2d3' if i >= 27200 else 'CH2d2' if i >= 27024 else 'CH2d') if i else 'CH2d',
    'CH3a': lambda n, i: 'CH3a2' if i and i >= 27012 else 'CH3a',
    'CH3c': lambda n, i: 'CH3c2' if i and i >= 27200 else 'CH3c',
}
LINE_TOK = (('CH1e', {'ALH', 'ASD'}), ('CH1f', {'ALD', 'AYR'}), ('CH1g', {'ALS', 'AA', 'ASR'}))
SKILL_MARK = {'SA', 'A', 'HA', 'SI', 'SI1', 'SI2', 'SI3', 'P'}


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
        return 'CH1m', "base call 3202b110, CX2GUUser code, not Ara's: registration only"
    if name in HYPER_NAMES:
        return 'CH1a', ('berserk mode table named by 31fdd800 (registration CH1m)' if 'BERSERK' in name else 'hyper mode table named by 31fdd490')
    if name == 'ASI_RIDING_HYPER_MODE':
        return 'CH1c', 'RIDING_HYPER_MODE is both RIDING* (CH1c) and HYPER* (CH1a): CH1c chosen (Chung / Elsword / Raven / Rena precedent)'
    if st['roll']:
        return 'CH1b', 'START / WIN / LOSE roll table (31fe22d0, INIT_COMMON_RANDOM_STATE)'
    ln = line_of(name)
    if len(ln) > 1:
        return 'UNASSIGNED', 'name carries tokens of two lines (%s)' % ' '.join(ln)
    if ln:
        return ln[0], ''
    if re.match(r'^ASI_(RIDING|PEPPER|DANCE)', name) or name.startswith('ASI_AMUSEMENT_ITEM_PHOENIX_RUN'):
        return 'CH1c', ('AMUSEMENT_ITEM_PHOENIX_RUN is the riding family: CH1c (precedent), other AMUSEMENT* are CH1b' if 'AMUSEMENT' in name else '')
    if re.match(r'^ASI_(DAMAGE|STANDUP|DIE|START|WIN|LOSE|WALL|AMUSEMENT|FROZEN|TRANSFORMED)', name):
        return 'CH1b', ''
    if re.match(r'^ASI_(WAIT|HYPER|WALK|JUMP|DOUBLE|THROW|AIRIAL|BERSERK|CHARGE)', name) or re.search(r'COMMON_BUFF', name):
        return 'CH1a', ''
    if name.startswith('ASI_DASH'):
        return 'CH1d2', ''
    if name.startswith('ASI_COMBO'):
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
    if reg and len(t_) > 2 and t_[1] in ('SA', 'A', 'HA') and name not in HYPER_NAMES:   # char-ara CH0: a skill-marked state InitState registers follows its skill
        if sk_ph:
            return sk_ph, 'registered by InitState and entered by skills: phase of the skills'
        ph, ms, mi = stem_phase(name)
        st['_minid'] = mi
        if ph:
            return ph, 'registered by InitState, no skill id: phase inferred from the stem (skill state %s)' % ' '.join(ms[:2])
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
        lab = 'SetEquippedSkillFunc 32bd09d0' if fn_ == '32bd09d0' else 'vslot 0x68c skill-state registration %s' % fn_
        sid_ = sorted({f[2] for f in rf if f[2] != '-'})
        how_ = '%s listing @%s state id %s (state set up through a class helper / a name the decompile extractor lost)' % (
            lab, ','.join(sorted({f[1] for f in rf})), ','.join(sid_) or '-')
        if how_ not in st['how'] and not (fn_ == '32bd09d0' and any('SetEquippedSkillFunc' in h for h in st['how'])):
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
SKLO, SKHI = 0x32bd09d0, 0x32bd281c
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
        st['how'].append('SetEquippedSkillFunc 32bd09d0 name xref @%08x (variable-name registration the extractor missed; ids and callbacks not extracted); anchor %s' % (a, anchor))
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
            inside.append('%s=shared (target %s outside the Ara range)' % (slot, tgt))
            prop.append('%s=-' % slot)
            continue
        if slot in mcb:
            ln, rest = mcb[slot]
            fnm, cl = rest.split('@')[0], rest.split('@')[-1] if '@' in rest else '?'
            inside.append('%s=inside X2GUAra.cpp:%s %s' % (slot, cl.rstrip(')'), fnm))
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
    o.write('# part 2: skill ids (%d June Ara ids from s3_reform_ids_no_state.tsv)\n' % len(sk))
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
                note = 'passive (type %d): no state; buff / passive data (InitPassiveSkillState 32bcd9c0, registers nothing), CH4a' % typ
            elif typ == 12:
                sts = ['ASI_THROW_ITEM']
                note = 'type 12: case 12 -> ASI_THROW_ITEM (id 900)'
            elif sid in (27213, 27238):
                note = 'case 32bd1b6f of the id switch: no state registration (SoundReady( L"Elsword_Endurance.ogg" ) 31d17130 on this+0x190 and the +0x11 flags only); the skill is a buff'
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
    o.write('# totals distinct Ara-range state functions by phase: %s\n' % ', '.join('%s=%d' % kv for kv in sorted(fc.items(), key=lambda kv: ORDER.index(kv[0]))))
    sc = collections.Counter(r2[5] for r2 in SKROWS)
    o.write('# totals skills by phase: %s\n' % ', '.join('%s=%d' % kv for kv in sorted(sc.items())))
    o.write('# skill ids mapped (state, or passive / buff with no state) %d / %d; unmapped: %s\n' % (len(sk) - len(unmapped_sk), len(sk), unmapped_sk))
print(open(os.path.join(HERE, 'ch0_state_skill.tsv'), encoding='utf-8').read().split('# totals', 1)[1])
print('UNASSIGNED', len(UNASSIGNED))
