r"""char-luciel CH0: build ch0_state_skill.tsv (June Lu/Ciel states x skills). Adapted from char_chung/ch0_build_state_skill.py
(no March class: the March columns are '-', every callback inside the class range is `new`).

    python june2017_re/charport_states.py init 32b43d10 32b3e8b0 3202b110 > june2017_re/char_luciel/ch0_reg.tsv
    python june2017_re/char_luciel/ch0_skill_listing_emu.py        (-> ch0_skill_listing_rows.tsv, _emu_outcome.pkl)
    python june2017_re/char_luciel/ch0_build_skill.py              (-> ch0_skill.tsv)
    python june2017_re/char_luciel/ch0_build_state_skill.py

Inputs: ch0_reg.tsv (InitStateCommon 32b3e8b0, the base event states 3202b110, InitState 32b43d10), ch0_skill_listing_rows.tsv (the skill setter
32b46580 read from its listing), the two June scripts (every top-level table), NewSkillTempletVer2 (templets, 301 Lu/Ciel tree rows), Enum SKILL_ID.
Registration order = runtime order: InitState sets +0x8ac8, calls InitStateCommon 32b3e8b0, 3202b110, then registers its own states (rolls 9 / 11 / 12,
bounding states, LDB dash combos); the skill setter registers per equipped skill. A callback shared by several states is named after the first
state registered in that order.

Phase rules (CH0 lead rules): non-skill registered states by name: roll rows START / WIN / LOSE of the base set and the hyper-mode rows (31fdd490) and
the base / wait-change event states CH1m; `LSI_RIDING_*` CH1i (merged by CH0); COMBO states of Lu / Ciel CH1f; every other Lu state CH1b (CH1c merged in);
every other Ciel state CH1d (CH1e merged in); LLC / LCG non-skill CH1g; LLD / LCN CH1h; LDB / LDN / BOUNDING CH1i. Skill states by the first owner class
of the skill in the June tree: 200 CH2a, 37 CH2b, 38 CH2c, 39 CH2d, 127 CH3a, 128 CH3b, 129 CH3c; type 13 (hyper active) CH3d; passive types CH4a.
A state with both a registration and a skill takes the registration phase. Nothing is guessed: a name no rule settles gets UNASSIGNED.
"""
import re, os, sys, struct, collections, pickle
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ch0_lu_data as D

HERE, B, LO, HI = D.HERE, D.B, D.LO, D.HI
exe = open(D.EXE, 'rb').read()
L = 0x1cbbe00
ORDER = ['CH1a', 'CH1b', 'CH1d', 'CH1f', 'CH1g', 'CH1h', 'CH1i', 'CH1k', 'CH1l', 'CH1m', 'CH1n', 'CH1o', 'CH1p',
         'CH2a', 'CH2b', 'CH2c', 'CH2d', 'CH3a', 'CH3b', 'CH3c', 'CH3d', 'CH4a', 'CH4b', 'CH4c', 'n/a', 'UNASSIGNED']
CLS_PH = {200: 'CH2a', 37: 'CH2b', 38: 'CH2c', 39: 'CH2d', 127: 'CH3a', 128: 'CH3b', 129: 'CH3c'}
SLOT_FN = {'Init': 'Init', 'StartFuture': 'StateStartFuture', 'Start': 'StateStart', 'FrameMoveFuture': 'FrameMoveFuture', 'FrameMove': 'FrameMove',
           'CameraMove': 'CameraMove', 'EventProcess': 'EventProcess', 'EndFuture': 'StateEndFuture', 'End': 'StateEnd'}
PREFIX_CLASS = {'LUCIEL': 200, 'LD': 200, 'LLC': 37, 'LCG': 38, 'LDB': 39, 'LLD': 127, 'LCN': 128, 'LDN': 129}


def minph(a, b):
    if a is None:
        return b
    if b is None:
        return a
    return a if ORDER.index(a) <= ORDER.index(b) else b


tm = D.templets()
snames = D.skill_names()
tree = D.tree_rows()
tabs = D.script_tables()
tab_names = {t[0] for t in tabs}
owners = collections.defaultdict(set)
for c, s, r, col, m in tree:
    owners[s].add(c)


def base_of(sid):
    return tm.get(sid, (0, 0, ''))[1] or sid


def skill_classes(sid):
    return owners.get(sid) or owners.get(base_of(sid)) or set()


HAND_SKILL = {208: ('CH2a', 'hand: no tree row ( legacy SI_SA_COMMON_AURA_DARKNESS_ACCEL, type 5 ) but a live case; same family as 30301 / 30302 ( Aura of Punishment, owner classes all 7, first 200 )')}


def skill_phase(sid):
    if sid in HAND_SKILL:
        return HAND_SKILL[sid][0]
    """phase of a skill id: type 13 CH3d, passive CH4a, else the first owner class in the June tree (the base id's tree rows for an upgrade)"""
    typ = tm.get(sid, (None,))[0]
    if typ == 13 or (snames.get(sid, '').find('_HA_') >= 0):
        return 'CH3d'
    if typ in (1, 8):
        return 'CH4a'
    cls = skill_classes(sid)
    if cls:
        return min((CLS_PH[c] for c in cls if c in CLS_PH), key=ORDER.index)
    m = re.match(r'^(?:RNSI|SI)_(?:SA|A|P|HA)_(\w+?)_', snames.get(sid, ''))
    if m and m.group(1) in PREFIX_CLASS and typ not in (None,):
        return CLS_PH[PREFIX_CLASS[m.group(1)]]
    return None


# ---- registrations ----
june = collections.OrderedDict()


def S(name):
    return june.setdefault(name, {'how': [], 'cb': [], 'skills': set(), 'reg': False, 'roll': False, 'hyper': False, 'ids': set()})


reg = [l.split('\t') + [''] * 5 for l in open(os.path.join(HERE, 'ch0_reg.tsv')).read().splitlines()[1:]]
RUNORDER = {'32b3e8b0': 0, '3202b110': 1, '32b43d10': 2}
reg.sort(key=lambda r: (RUNORDER[r[0]], int(r[1].lstrip('~').rstrip('?!') or 0)))
CORE_FN = {'32b3e8b0': 'InitStateCommon 32b3e8b0', '3202b110': 'CX2GUUser event states 3202b110', '32b43d10': 'InitState 32b43d10'}
roll_cb, hyp_cb = {}, {}
for fn, line, sid, name, cb in [r[:5] for r in reg]:
    if name == 'ppppuVar5':
        roll_cb[sid] = cb
        continue
    if name == 'puVar4':
        hyp_cb[sid] = cb
        continue
    st = S(name)
    st['how'].append('%s@%s id %s' % (CORE_FN[fn], line, sid))
    st['ids'].add(int(sid))
    st['reg'] = True
    if cb:
        st['cb'].append(cb)
# roll names: INIT_COMMON_RANDOM_STATE (all LSI_ names in the block)
src = D.rd(os.path.join(D.J, 'Luciel_Duo.src.lua'))
i = src.index('INIT_COMMON_RANDOM_STATE')
rnd = []
for n in re.findall(r'"(LSI_\w+)"', src[i:src.index('\n}', i)]):
    if n not in rnd:
        rnd.append(n)
for n in rnd:
    kind = '9' if 'START' in n else ('11' if 'WIN' in n else '12')
    st = S(n)
    st['how'].append('START/WIN/LOSE roll 31fe22d0 (INIT_COMMON_RANDOM_STATE) InitState 32b43d10 id %s' % kind)
    st['ids'].add(int(kind))
    if roll_cb.get(kind):
        st['cb'].append(roll_cb[kind])
    st['roll'] = True
# hyper names: 31fdd490 by unit class: Lu id 23 / Ciel id 204; classes 127 / 128 / 129 take their own
HYPER = [('LSI_LU_HYPER_MODE', '23', 'classes 37 38 39 200'), ('LSI_CIEL_HYPER_MODE', '204', 'classes 37 38 39 200'),
         ('LSI_LLD_LU_HYPER_MODE', '23', 'class 127'), ('LSI_LLD_CIEL_HYPER_MODE', '204', 'class 127'),
         ('LSI_LCN_LU_HYPER_MODE', '23', 'class 128'), ('LSI_LCN_CIEL_HYPER_MODE', '204', 'class 128'),
         ('LSI_LDN_LU_HYPER_MODE', '23', 'class 129'), ('LSI_LDN_CIEL_HYPER_MODE', '204', 'class 129')]
for n, sid, cl in HYPER:
    st = S(n)
    st['how'].append('hyper mode 31fdd490 ( %s ) InitStateCommon 32b3e8b0 id %s' % (cl, sid))
    st['ids'].add(int(sid))
    st['cb'].append(hyp_cb[sid])
    st['hyper'] = True
# the skill setter, read from its listing
LST = collections.defaultdict(list)
for l in open(os.path.join(HERE, 'ch0_skill_listing_rows.tsv'), encoding='utf-8').read().splitlines()[1:]:
    f = (l.split('\t') + [''] * 8)[:8]
    if f[3]:
        LST[f[3]].append(f)
for tbl, rws in LST.items():
    st = S(tbl)
    ids = set()
    for f in rws:
        ids.update(int(x) for x in f[4].split(',') if x.isdigit())
    st['skills'].update(ids)
    sids = sorted({f[2] for f in rws if f[2] != '-'})
    st['how'].append('SetEquippedSkillFunc 32b46580 @%s %s' % (','.join(sorted({f[1] for f in rws})), ('state id ' + ','.join(sids)) if sids else 'slot state (USI_SPECIAL_ATTACK_n / _SLOTB_n by slot index and Lu / Ciel)'))
    st['ids'].update(int(x) for x in sids)
    have = set()
    for f in rws:
        for t3 in re.findall(r'(\w+)=([0-9a-f]{8})>([0-9a-f]{8})', f[6]):
            have.add(t3)
    if have:
        st['cb'].append(' '.join('%s=%s>%s' % t3 for t3 in sorted(have, key=lambda t: list(SLOT_FN).index(t[0]))))

# ---- string xrefs (any push / mov imm32 of the table-name string anywhere in the code) for the tables nothing registered ----
def xrefs(name):
    out = []
    b = b'\0' + name.encode() + b'\0'
    j = exe.find(b)
    while j != -1:
        q = struct.pack('<I', j + 1 + B)
        k = exe.find(q, 0, L)
        while k != -1:
            out.append(k + B)
            k = exe.find(q, k + 1, L)
        j = exe.find(b, j + 1)
    return out


# ---- phases ----
MOVE = re.compile(r'^(WAIT|WALK|JUMP_READY|JUMP_UP|JUMP_DOWN|JUMP_DOWN_NO_ATTACK|JUMP_LANDING|DASH|DASH_END|DASH_JUMP|DASH_JUMP_LANDING|DASH_JUMP_POWER_LANDING)$')


def reg_phase(name, st):
    """phase of a non-skill registered state by its name; (phase, note)"""
    if st['roll']:
        if re.match(r'^LSI_(START|WIN2?|LOSE2?)$', name):
            return 'CH1m', 'base START / WIN / LOSE roll set'
        return reg_phase_by_prefix(name, st)
    if st['hyper']:
        return 'CH1m', 'hyper-mode row 31fdd490'
    if name.startswith('USI_EVENT_') or name in ('LSI_WAIT_STATE_CHANGE', 'LSI_START_INTRUDE'):
        return 'CH1m', 'base / wait-change event state (shared callbacks, registration only)'
    return reg_phase_by_prefix(name, st)


def reg_phase_by_prefix(name, st):
    if name.startswith('LSI_RIDING_'):
        return 'CH1i', ''   # riding (3 functions) merged into CH1i by CH0
    m = re.match(r'^LSI_(LU|CIEL)_(.+)$', name)
    if m:
        if 'COMBO' in m.group(2):
            return 'CH1f', ''
        return ('CH1b' if m.group(1) == 'LU' else 'CH1d'), ''   # CH0 merged CH1c into CH1b and CH1e into CH1d
    m = re.match(r'^LSI_(LLC|LCG|LLD|LCN|LDB|LDN)_', name)
    if m:
        return {'LLC': 'CH1g', 'LCG': 'CH1g', 'LLD': 'CH1h', 'LCN': 'CH1h', 'LDB': 'CH1i', 'LDN': 'CH1i'}[m.group(1)], ''
    if name.startswith('LSI_BOUNDING_'):
        return 'CH1i', ''
    return 'UNASSIGNED', 'no name rule'


def state_phase(name, st):
    sk_ph = None
    for x in st['skills']:
        p = skill_phase(x)
        sk_ph = minph(sk_ph, p)
    nonskill = bool(st['reg'] or st['roll'] or st['hyper'])
    if nonskill and not re.match(r'^LSI_(SA|A|HA|SI)_', name):
        ph, why = reg_phase(name, st)
        if ph == 'UNASSIGNED' and sk_ph:
            why += '; as a skill state it would be %s' % sk_ph
        return ph, why
    if nonskill:
        # a skill-named table registered by InitStateCommon (no skill id from the setter): the skill is linked by name below
        return (sk_ph, 'skill-named table registered by the common init; phase of its skill') if sk_ph else (None, '')
    return sk_ph, ''


# skill name <-> table name convention for a skill-named state registered in the common init or reached through a transform
def name_match(sid):
    n = snames.get(sid, '')
    m = re.match(r'^(?:RNSI|SI)_((?:SA|A|HA)_.+?)(?:_UPGRADE)?$', n)
    if not m:
        return []
    core = m.group(1).replace('LUCIEL_', 'LD_', 1)
    out = []
    for t in june:
        if t == 'LSI_' + core or t.startswith('LSI_' + core + '_'):
            out.append(t)
        elif '_TRANSFORM_' in t and t.endswith('_' + core.split('_', 2)[2] if core.count('_') >= 2 else '#'):
            out.append(t)
    if not out and core.count('_') >= 2:
        # the skill's class token differs from the table's ( RNSI_SA_LCN_FANTASIE_IMPROMPTU_2ND_MOVEMENT -> LSI_SA_LCG_... ): one registered table with the same tail
        kind, _, tail = core.split('_', 2)
        cand = [t for t in june if re.fullmatch(r'LSI_%s_[A-Z]+_%s' % (kind, re.escape(tail)), t) and (june[t]['reg'] or june[t]['skills'])]
        if len(cand) == 1:
            out = cand
    return out


# skill -> states
sk_states = collections.defaultdict(list)
for n, st in june.items():
    for x in st['skills']:
        sk_states[x].append(n)
byname = {}
def legacy(sid):
    return 10000 <= sid < 10700 and sid not in owners and sid not in sk_states


for sid in sorted(set(D.lu_skill_ids(tm)) | set(owners)):
    if sid in sk_states or tm.get(sid, (None,))[0] in (1, 8) or legacy(sid):
        continue
    for t in name_match(sid):
        if june[t]['reg'] or june[t]['skills']:
            byname.setdefault(sid, []).append(t)
for sid, ts in byname.items():
    for t in ts:
        june[t]['skills'].add(sid)
        sk_states[sid].append(t)
        june[t].setdefault('byname', set()).add(sid)

# ---- rows ----
rows = []
UNASSIGNED = []
fn_first = collections.OrderedDict()   # callback target -> first (state, slot) in registration order


def callbacks(cbstr):
    return re.findall(r'(\w+)=([0-9a-f]{8})>([0-9a-f]{8})', cbstr)


for st_name, st in june.items():
    for cbs in st['cb']:
        for slot, th, tgt in callbacks(cbs):
            if LO <= int(tgt, 16) < HI:
                fn_first.setdefault(tgt, (st_name, slot))


def build_row(name, line, dup):
    st = june.get(name)
    if st and st['how']:
        ph, why = state_phase(name, st)
        if ph is None:
            ph, why = 'UNASSIGNED', 'skill-named state with no skill id'
        verdict = 'june-only'
        how = ' | '.join(dict.fromkeys(st['how']))
        cb = ' | '.join(dict.fromkeys(st['cb']))
        skills = ','.join('%d %s (t%d)' % (x, snames.get(x, '?'), tm.get(x, (0,))[0] or 0) for x in sorted(st['skills']))
        if st.get('byname'):
            verdict += '; skill link by name convention for %s' % ','.join(str(x) for x in sorted(st['byname']))
        if why:
            verdict += '; PHASE NOTE: ' + why
    else:
        ph, verdict, st = 'n/a', 'june-dead-data', None
        xr = xrefs(name)
        how = ('string in the exe, referenced from %s but no registration found: read it' % ' '.join('%08x' % x for x in xr[:4])) if xr else \
            'no reference in June C++ (string absent from the exe or never pushed)'
        cb, skills = '', ''
    inside, prop = [], []
    for slot, th, tgt in callbacks(cb):
        t = int(tgt, 16)
        if not (LO <= t < HI):
            inside.append('%s=shared (target %s outside the Lu/Ciel range)' % (slot, tgt))
            prop.append('%s=-' % slot)
        else:
            first = fn_first.get(tgt)
            nm_ = '%s_%s' % (first[0], SLOT_FN[first[1]]) if first else '%s_%s' % (name, SLOT_FN[slot])
            inside.append('%s=new' % slot)
            prop.append('%s=%s' % (slot, nm_))
    if dup:
        verdict += '; duplicate table (the script defines it twice, the second wins: JUNE_BUGS candidate)'
    if ph == 'UNASSIGNED':
        UNASSIGNED.append((name, verdict))
    return (name, str(line), how, skills, cb, '-', verdict, ph, ' | '.join(inside) or '-', ' | '.join(prop) or '-'), st


seen = set()
nonstate = []
for name, script, line in tabs:
    if name.startswith('INIT_'):
        nonstate.append((name, script, line))
        continue
    dup = name in seen
    seen.add(name)
    r, st = build_row(name, '%s:%d' % (script, line), dup)
    rows.append(r)
tabset = {t[0] for t in tabs}
for name in june:
    if name not in tabset:
        r, st = build_row(name, '-', False)
        r = list(r)
        r[6] = 'june-cpp-no-table (June C++ registers it, the June scripts have no table: JUNE_BUGS candidate); ' + r[6]
        rows.append(tuple(r))

# ---- part 2: skills ----
def skill_row(sid):
    name = snames.get(sid, '?')
    typ = tm.get(sid, (None,))[0]
    cls = sorted(skill_classes(sid), key=D.CLS_ORDER.index)
    sts = sorted(set(sk_states.get(sid, [])), key=lambda n: list(june).index(n))
    note = ''
    if sid in byname:
        note = 'state by name convention (registered in the common init or a transform extra state), not by the skill setter'
    ph = skill_phase(sid)
    if sid in HAND_SKILL:
        ph, note = HAND_SKILL[sid]
    if typ in (1, 8):
        note = 'passive (type %s): no state; buff / passive data (InitPassiveSkillState), CH4a' % typ
    elif not sts:
        if legacy(sid):
            tw = [t for t in name_match(sid) if june[t]['reg'] or june[t]['skills']]
            ph, note = 'n/a', 'legacy SI_ id: no tree row, no case in the skill setter, no state of its own' + ((' ( same-name twin state: %s )' % ';'.join(tw[:3])) if tw else '')
        else:
            note = 'NO STATE FOUND'
    if typ == 12 and sts:
        ph = min((june_state_ph[n] for n in sts if june_state_ph.get(n)), key=ORDER.index)
        note = 'type 12 ( THROW_ITEM ): the phase of its registered states'
    if sts and ph is None:
        phs = [june_state_ph[n] for n in sts if june_state_ph.get(n)]
        ph = min(phs, key=ORDER.index) if phs else 'UNASSIGNED'
        note = (note + '; ' if note else '') + 'phase of its state ( %s )' % ph
    if ph is None:
        ph = 'UNASSIGNED'
    return sid, name, typ, cls, sts, ph, note


june_state_ph = {r[0]: r[7] for r in rows if r[1] != '-' or True}
all_sids = sorted(set(D.lu_skill_ids(tm)) | set(owners) | {i for i in sk_states})
SKROWS = [skill_row(s) for s in all_sids]

with open(os.path.join(HERE, 'ch0_state_skill.tsv'), 'w', encoding='utf-8', newline='\n') as o:
    o.write('# part 1: states (June script tables + June registrations with no table; no March class: march columns are -)\n')
    o.write('state\tjune_script_line\tjune_registration\tskills\tjune_callbacks (thunk>target)\tmarch (cpp line:callbacks Class::Fn@line)\tverdict\tphase\t'
            'inside_new (per callback; every callback inside the Lu/Ciel range is new)\tproposed_name (per new callback; shared by several states: first registered)\n')
    for r in rows:
        o.write('\t'.join(r) + '\n')
    o.write('# part 2: skill ids (%d: the %d Lu/Ciel templets 10000-10999 / 30000-30999, tree ids 106 / 900, case id 208)\n' % (len(SKROWS), len(D.lu_skill_ids(tm))))
    o.write('skill_id\tname\ttype\tclasses\tjune_state(s)\tphase\tnote\n')
    for sid, name, typ, cls, sts, ph, note in SKROWS:
        o.write('%d\t%s\t%s\t%s\t%s\t%s\t%s\n' % (sid, name, typ, ','.join(map(str, cls)), ';'.join(sts), ph, note))
    o.write('# part 3: June skill tree rows (%d: UC_LUCIEL_* rows of NewSkillTempletVer2)\n' % len(tree))
    o.write('class\tclass_name\tskill_id\tname\ttype\trow\tcolumn\tmaster_level\tjune_state(s)\tphase\n')
    skmap = {r[0]: r for r in SKROWS}
    for c, s, rw, col, ml in tree:
        r = skmap[s]
        o.write('%d\t%s\t%d\t%s\t%s\t%d\t%d\t%d\t%s\t%s\n' % (c, D.CLASSES[c], s, r[1], r[2], rw, col, ml, ';'.join(r[4]), r[5]))
    # totals
    ntab = [t for t in tabs if not t[0].startswith('INIT_')]
    c1 = collections.Counter()
    for r in rows:
        if r[1] == '-':
            continue
        c1['dead' if r[7] == 'n/a' else ('UNASSIGNED' if r[7] == 'UNASSIGNED' else 'mapped')] += 1
    o.write('# script tables: %d (%d distinct names): %d LSI_* state tables, %d INIT_* (non-state)\n' % (len(tabs), len(tabset), len(ntab), len(nonstate)))
    o.write('# tables mapped (phase assigned) %d, UNASSIGNED %d, dead n/a %d, INIT_* non-state %d = %d / %d\n' % (c1['mapped'], c1['UNASSIGNED'], c1['dead'], len(nonstate),
                                                                                                         c1['mapped'] + c1['UNASSIGNED'] + c1['dead'] + len(nonstate), len(tabs)))
    o.write('# dead tables: %s\n' % ' '.join(r[0] for r in rows if r[1] != '-' and r[7] == 'n/a'))
    o.write('# unassigned: %s\n' % ' '.join(r[0] for r in rows if r[1] != '-' and r[7] == 'UNASSIGNED'))
    o.write('# registered names with no table: %s\n' % ' '.join(r[0] for r in rows if r[1] == '-'))
    pc = collections.Counter(r[7] for r in rows if r[1] != '-')
    o.write('# totals tables by phase: %s\n' % ', '.join('%s=%d' % (k, pc[k]) for k in ORDER if pc[k]))
    fn_ph = {}
    for name, st in june.items():
        if not st['how']:
            continue
        p = june_state_ph.get(name, 'UNASSIGNED')
        for cbs in st['cb']:
            for slot, th, tgt in callbacks(cbs):
                if LO <= int(tgt, 16) < HI:
                    if tgt not in fn_ph or ORDER.index(p) < ORDER.index(fn_ph[tgt]):
                        fn_ph[tgt] = p
    fc = collections.Counter(fn_ph.values())
    o.write('# totals distinct Lu/Ciel-range state functions by phase: %s\n' % ', '.join('%s=%d' % (k, fc[k]) for k in ORDER if fc[k]))
    sc = collections.Counter(r[5] for r in SKROWS)
    o.write('# totals skill ids by phase: %s\n' % ', '.join('%s=%d' % (k, sc[k]) for k in ORDER if sc[k]))
    nost = [r[0] for r in SKROWS if not r[4] and r[6].find('NO STATE') >= 0]
    o.write('# skill ids with no state found: %s\n' % nost)
    tc = collections.Counter('state' if skmap[s][4] else ('passive' if skmap[s][2] in (1, 8) else ('legacy' if skmap[s][5] == 'n/a' else 'NONE')) for c, s, *_ in tree)
    o.write('# tree rows (%d): with a state %d, passive %d, legacy n/a %d, none %d\n' % (len(tree), tc['state'], tc['passive'], tc['legacy'], tc['NONE']))
    ids = sorted({i for st in june.values() for i in st['ids']})
    o.write('# state ids registered: %d distinct, min %d max %d\n' % (len(ids), ids[0], ids[-1]))
print(open(os.path.join(HERE, 'ch0_state_skill.tsv'), encoding='utf-8').read().split('\n# script tables', 1)[1])
print('UNASSIGNED', len(UNASSIGNED), UNASSIGNED[:10])
