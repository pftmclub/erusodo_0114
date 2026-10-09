"""C0 plumbing for JUNE2017_EFFECT_COMPLETION_PHASES.md: what census.py adds on top of the Phase 1 census.

census.py imports this module; nothing here runs a preprocessor or reads the June binary. It works on
the census rows (dicts with root, rel, key, status, phase, system, march, ...), the June effect data
(assets_cache/june_fx.json) and the March source tree. Three jobs:

1. Dispositions (`apply`). Two new row statuses, set after the census has decided the old five:

     handed-off   June reads it, March does not, and the owner is another port. `receiver` names the
                  document + phase (or section). Not open work for this plan.
     declined     the user declined the system. `receiver` names the decision and its date.

   `SYSTEM_DISPOSITION` is the machine-readable copy of C0's decision table: every absent system
   the census finds (status `unsupported`) must be in it, or the census fails. `build` / `pending`
   systems stay `unsupported` -- code under a `SERV_IRUHADEV_JUNE_*` flag is deliberately not evidence
   of a system (census.JUNE_FLAG_PREFIX) -- so their completion is the `flag` landing instead.

2. Owners (`owner_of`). Every open row (to-implement / unsupported / binary-only) is mapped to the
   C-phase that closes it, so a new open row cannot appear without an owner.

3. Checks (`checks`). Definition-of-complete items 1 and 3 (item 2 and 4 under `--final`):
     1. no June DAMAGE_DATA* block gets damage only from a system March lacks;
     3. every binary-only row has a March parse site -- or is owed to a C-phase that will give it one.
        `--final` (C11/C12) fails every owed row.
"""
import collections
import json
import os
import re

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..'))
JUNE_FX = os.path.join(HERE, 'assets_cache', 'june_fx.json')
ALWAYS_H = os.path.join(TRUNK, 'KTDXLIB', 'Always.h')
PLAN = 'JUNE2017_EFFECT_COMPLETION_PHASES.md'
CHECKLIST = 'JUNE2017_CHARACTER_PORT_CHECKLIST.md'
BUFF_PLAN = 'JUNE_BUFF_PORT_PHASES.md'

OPEN_STATUSES = ('to-implement', 'unsupported', 'binary-only')

# ------------------------------------------------------------------ dispositions
# name = the census SYSTEMS name. (disposition, owner or receiver, flag that lands it or None, decision)
# disposition: build | handed-off | declined | pending
SYSTEM_DISPOSITION = {
    'Numerical skill data': ('handed-off', '%s section 1 (character ports: June skill reform)' % CHECKLIST, None,
                             'D7 2026-10-06: C1a found every block uses June reform skill IDs no March character learns'),
    'Applied skill ID': ('handed-off', '%s section 1 (character ports: June skill reform)' % CHECKLIST, None,
                         'D7 2026-10-06: its readers (numerical override, memo/ultimate gate) need the June skill system'),
    'Effect-alpha option': ('build', 'C10', 'SERV_IRUHADEV_JUNE_EFFECT_ALPHA_OPTION', 'D2 2026-10-05: build (C10b-2 2026-10-09, engine; the option UI goes to the June UI port, D9)'),
    'HP-rate damage': ('build', 'C10', 'SERV_IRUHADEV_JUNE_HP_RATE_DAMAGE', 'D4 2026-10-05: build (C10b-1 2026-10-07)'),
    'Combination point': ('handed-off', '%s section 3 (Lu/Ciel port)' % CHECKLIST, None,
                          'D5 2026-10-06: C2 found the only reader in CX2GULuciel (32b14bc0); port it with Lu/Ciel'),
    'Natural force': ('handed-off', '%s section 3 (Rena port)' % CHECKLIST, None,
                      'D3 2026-10-05: hand off; turning on ADD_RENA_SYSTEM is a character-port decision'),
    'Rose weapon': ('handed-off', '%s section 3 (Rose port)' % CHECKLIST, None,
                    'C0 2026-10-05: waits for a Rose character port; parse + log is parity until then'),
}
BUFF_RECEIVER = ('%s Phase 8 (BFT 14-17 finalizers; nested BFT_AURA = 14); the BBT behaviors landed in 4b/7/7b, data in 13' % BUFF_PLAN)


ES_DESCRIPTION_RECEIVER = ('JUNE2017_EFFECT_COMPLETION_PHASES.md C9 (declined; June 32378e20 = AddNPCTemplet_LUA and '
                           '32680da0 = AddNextStepPackage_LUA, funcmap rows 1287-1288; the file is not named in the binary)')


def _nested_buff_factor(r):
    return r['root'] == 'DE.DAMAGE_DATA' and tuple(r['rel'][:1]) == ('BUFF_FACTOR',)


def _system_name(r):
    s = r.get('system') or ''
    return s.split(' (')[0] if s else None


def disposition_of(r):
    """-> (disposition, owner-or-receiver, note) for an open row, else None."""
    if r['status'] not in OPEN_STATUSES and r['status'] not in ('handed-off', 'declined'):
        return None
    sysn = _system_name(r) or KEY_SYSTEM.get(r['key'])
    if sysn in SYSTEM_DISPOSITION:      # first: the plan gives C1 all four APPLIED_SKILL_ID rows, nested one included
        d, who, _flag, note = SYSTEM_DISPOSITION[sysn]
        return (d, who, note)
    if r['root'] == 'ES.Description':
        return ('declined', ES_DESCRIPTION_RECEIVER, 'C9 2026-10-07: June never names EffectSet_Description.lua (C2)')
    if _nested_buff_factor(r):
        return ('handed-off', BUFF_RECEIVER, 'plan "Not in this definition": nested BUFF_FACTOR/* belongs to the buff port')
    return None


def apply(rows):
    """Set status `handed-off` / `declined` and r['receiver'] on open rows; set r['owner'] on the rest."""
    for r in rows:
        r['receiver'] = ''
        r['owner'] = ''
        if r['status'] not in OPEN_STATUSES:
            continue
        d = disposition_of(r)
        if d and d[0] in ('handed-off', 'declined'):
            r['status'] = d[0]
            r['receiver'] = d[1]
            continue
        r['owner'] = owner_of(r, d) or ''


# ------------------------------------------------------------------ owners
# Keys with no census system whose only June reader belongs to a system above (C2: DAMAGE_SKILL_ID is read only by
# the NUMERICAL_DATA override, 3263fefc), so they follow that system's disposition.
KEY_SYSTEM = {'DAMAGE_SKILL_ID': 'Numerical skill data'}


# C-phase that closes each open row (JUNE2017_EFFECT_COMPLETION_PHASES.md "Where things stand" + C1..C11).
_C1 = {'NUMERICAL_DATA_INDEX', 'NUMERICAL_DATA_NAME', 'APPLIED_SKILL_ID', 'DAMAGE_SKILL_ID'}
_C3 = {'INIT_EXTRA_DAMAGE_BEFORE_LOAD', 'ADD_MEMBER_COUNT', 'ADD_MEMBER_COUNT_DISTANCE', 'BOUNDING_BACK_SPEED_X',
       'BOUNDING_BACK_SPEED_Y', 'FORCE_BACK_SPEED', 'ADD_CHECK_DAMAGE_UNIT_NUM', 'EXCEPT_ME_HP_UP', 'ATTACK_ONLY_ME',
       'LIMIT_APPLY_BUFF_COUNT_EACH_UNIT', 'DAMAGE_LIGHT_TIME', 'IS_AURA_BUFF', 'IGNORE_ATTACK_SUCCESS',
       'ONYL_ATTACK_USER', 'PAUSE_NPC_AI_TIME', 'DAMAGE_DATA_KEY_NAME',
       'HIT_ADD_MP_PERCENT', 'DAMAGE_RATE_TO_SUPERAMOR', 'CREATE_DAMAGE_EFFECT_NAME_SCALE_X',
       'CREATE_DAMAGE_EFFECT_NAME_SCALE_Y', 'CREATE_DAMAGE_EFFECT_NAME_SCALE_Z', 'APPLIED_MEMO_ID'}
_C4 = {'IGNORE_DEFENCE_ON_PVP', 'IGNORE_DEFENCE_RATE', 'IGNORE_DEFENCE_RATE_ON_PVP', 'IGNORE_DEFENCE_DECLINE',
       'DIVISION_NUM', 'DIVISION_OFF_TIME', 'DIVIDE_DAMAGE_FROM_RANGE', 'RANGE%d', 'CAN_ACTIVATE_NASOD_ARMOR_COMBO'}
_C5 = {'APPLY_LEARNED_SKILL_BUFF_FACTOR_ID', 'APPLY_LEARNED_SKILL_BUFF_FACTOR_ID_RELATIVE_SKILL_ID',
       'APPLY_LEARNED_SKILL_BUFF_FACTOR_ID_RELATIVE_BUFF_TEMPLATE_ID', 'BUFF_FACTOR_ID_ULTIMATE',
       'BUFF_FACTOR_ID_ULTIMATE_PVP', 'BUFF_FACTOR_ID_PVP', 'DAMAGE_TRIGGER_DATA', 'DAMAGE_TRIGGER_CONDITION',
       'ADDITIONAL_ATTACK_TRIGGER_TYPE', 'TRACE_NEAREST_BONE'}     # + every DAMAGE_TRIGGER_DATA/[] child, by sub-path
_C11 = {'DAMAGE_DATA_NEXT_ULTIMATE%d', 'AT_RAID_FIELD', 'HIT_EFFECT_SET%d'}   # already parsed; C11 observes them
_C10 = {'APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION', 'HP_RATE_DAMAGE_TYPE', 'HP_RATE_DAMAGE_VALUE'}


def owner_of(r, disp=None):
    """The C-phase that closes this open row, or None (the census then FAILs: an open row without an owner)."""
    k, root = r['key'], r['root']
    if disp and disp[0] in ('build', 'pending'):
        return disp[1]
    if root in ('InsertDamageData', 'ES.Description'):
        return 'C9'
    if k in _C11:
        return 'C11'
    if root == 'DE.DAMAGE_DATA' and tuple(r['rel'][:1]) == ('DAMAGE_TRIGGER_DATA',):
        return 'C5'
    if k in _C10:
        return 'C10'
    if k in _C1:
        return 'C1'
    if k in _C4:
        return 'C4'
    if k in _C5:
        return 'C5'
    if k in _C3:
        return 'C3'
    return None


# ------------------------------------------------------------------ binary-only rows with a hand-cited March site
# The census finds a March parse site by the key's own literal. A row it cannot see (the key is built at run
# time with %d) is cited by hand: (key) -> (March TU, literal that must survive the preprocessor, citation).
HAND_PARSE_SITES = {
    'DAMAGE_DATA_NEXT_ULTIMATE%d': ('X2DamageEffect.cpp', 'DAMAGE_DATA_NEXT_ULTIMATE%d',
                                    'X2DamageEffect.cpp ctor chain loop (StringCchPrintfA bDamageDataNextIsUltimate)'),
    'AT_RAID_FIELD': ('X2DamageEffect.cpp', '"AT_RAID_FIELD"', 'X2DamageEffect.cpp LoadLockOnData (m_fLockOnRangeFromOwnerAtRaidField)'),
    'HIT_EFFECT_SET%d': ('X2DamageEffect.cpp', '"HIT_EFFECT_SET"', 'X2DamageEffect.cpp LoadHitEffectSet (BeginTable "HIT_EFFECT_SET", index)'),
}


def _parse_site(r, tu_text):
    """-> citation string if March parses this binary-only row, else None."""
    if r['march']:
        return 'census: ' + ', '.join('%s:%d' % (tu.replace('.cpp', ''), ln) for tu, ln, _, _ in r['march'][:2])
    hs = HAND_PARSE_SITES.get(r['key'])
    if hs:
        text = tu_text(hs[0])
        if text is not None and hs[1] in text:
            return 'hand-cited: ' + hs[2]
    return None


# ------------------------------------------------------------------ item 1: damage June gets from a system March lacks
def _num(x):
    return isinstance(x, (int, float)) and not isinstance(x, bool)


def _truthy(x):
    return (_num(x) and x != 0) or (isinstance(x, str) and x != '')


def _src_numerical(t):
    idx = t.get('NUMERICAL_DATA_INDEX')
    named = _truthy(t.get('NUMERICAL_DATA_NAME'))
    return ((_num(idx) and idx > 0) or named) and (_truthy(t.get('APPLIED_SKILL_ID')) or _truthy(t.get('DAMAGE_SKILL_ID')))


def _src_keyname(t):
    return _truthy(t.get('DAMAGE_DATA_KEY_NAME'))


def _src_hp_rate(t):
    return _num(t.get('HP_RATE_DAMAGE_TYPE')) and t['HP_RATE_DAMAGE_TYPE'] > 0


# (name, predicate over a block's key->value dict, landing flag or None, owning phase)
# A source is "in March" only when its flag is defined in Always.h AND tested by an #if in X2Lib/KTDXLIB code.
# Flag None = the owning phase has not named it yet (it sets it here, in the same commit as its code).
# Each source is its own check line, so C1 can close its line without waiting for C10's.
DAMAGE_SOURCES = [
    ('numerical skill data (June SetDamageDataFromLUA override, 32632390@3263fe41)', _src_numerical,
     'SERV_IRUHADEV_JUNE_NUMERICAL_DATA', 'C1', 'Numerical skill data'),
    ('HP-rate damage (HP_RATE_DAMAGE_TYPE/VALUE; C10a confirms the formula)', _src_hp_rate,
     'SERV_IRUHADEV_JUNE_HP_RATE_DAMAGE', 'C10', 'HP-rate damage'),
]


def _flag_landed(flag, _cache={}):
    """Defined (not commented out) in Always.h, and tested by an #if/#ifdef in X2Lib or KTDXLIB code."""
    if flag is None:
        return False
    if flag in _cache:
        return _cache[flag]
    try:
        always = open(ALWAYS_H, 'rb').read().decode('latin1')
    except IOError:
        _cache[flag] = False
        return False
    defined = re.search(r'^[ \t]*#[ \t]*define[ \t]+%s\b' % re.escape(flag), always, re.M) is not None
    used = False
    if defined:
        pat = re.compile(r'^[ \t]*#[ \t]*(?:if|ifdef|ifndef|elif)\b[^\n]*\b%s\b' % re.escape(flag), re.M)
        for sub in ('X2Lib', 'KTDXLIB'):
            for dp, dn, fn in os.walk(os.path.join(TRUNK, sub)):
                for f in fn:
                    if f.endswith('.cpp') and pat.search(open(os.path.join(dp, f), 'rb').read().decode('latin1')):
                        used = True
                        break
                if used:
                    break
            if used:
                break
    _cache[flag] = defined and used
    return _cache[flag]


def june_damage_blocks(path=JUNE_FX):
    """-> [(file, effect name, block key, block table)] for every DAMAGE_DATA* table in June's DamageEffect*.lua."""
    d = json.load(open(path, encoding='utf-8', errors='replace'))
    out = []

    def walk(v, name, fil):
        t = v.get('$t') if isinstance(v, dict) else None
        if t is None:
            return
        for k, x in t.items():
            if isinstance(k, str) and k.startswith('DAMAGE_DATA') and isinstance(x, dict) and '$t' in x:
                out.append((fil, name, k, x['$t']))
            walk(x, name, fil)
    for name, e in d['globals'].items():
        if e['file'].lower().startswith('damageeffect'):
            walk(e['value'], name, e['file'])
    return out


def _direct_damage(t):
    dm = t.get('DAMAGE')
    return isinstance(dm, dict) and any(_num(x) and x != 0 for x in dm.get('$t', {}).values())


def damage_regression(path=JUNE_FX):
    """-> (n_blocks, per_source, no_skill_id). per_source[i] = (source, [(file, effect, block key)]): the blocks
    that have no non-zero DAMAGE table of their own, use that source, and whose source has not landed."""
    blocks = june_damage_blocks(path)
    per = [(src, []) for src in DAMAGE_SOURCES]
    no_skill = 0
    for fil, name, k, t in blocks:
        if _direct_damage(t):
            continue
        idx = t.get('NUMERICAL_DATA_INDEX')
        if ((_num(idx) and idx > 0) or _truthy(t.get('NUMERICAL_DATA_NAME'))) and not _src_numerical(t):
            no_skill += 1     # numerical data set, no skill ID: June's override does not run either
        for src, hits in per:
            if src[1](t) and not _flag_landed(src[2]):
                hits.append((fil, name, k))
    return len(blocks), per, no_skill


# ------------------------------------------------------------------ checks
def checks(rows, tu_text, final=False, fx_path=JUNE_FX):
    """-> [(ok, text)]. `rows` after apply(); tu_text(tu name) -> preprocessed text or None."""
    out = []
    # --- every open row has an owner; every unsupported row has a recorded disposition
    unowned = [r for r in rows if r['status'] in OPEN_STATUSES and not r['owner']]
    out.append((not unowned, 'every open row has an owning C-phase (or is handed-off / declined)%s' % (
        '' if not unowned else ': ' + ', '.join('`%s` %s' % (r['key'], '/'.join(r['rel']) or '(top)') for r in unowned[:12]))))
    nodisp = [r for r in rows if r['status'] == 'unsupported' and not (disposition_of(r) or ('',))[0]]
    out.append((not nodisp, 'every `unsupported` row names a recorded decision (build / handed-off / declined / pending), '
                'definition item 4%s' % ('' if not nodisp else ': ' + ', '.join(
                    '`%s` (%s)' % (r['key'], r.get('system') or 'no system') for r in nodisp[:12]))))
    bad_ho = [r for r in rows if r['status'] in ('handed-off', 'declined') and not r['receiver']]
    out.append((not bad_ho, 'every `handed-off` / `declined` row names its receiver (document + phase or section)'))

    # --- item 3
    bo = [r for r in rows if r['status'] == 'binary-only']
    unsited, owed, sited = [], [], 0
    for r in bo:
        site = _parse_site(r, tu_text)
        if site:
            sited += 1
        elif r['status'] == 'binary-only' and r['owner'] in ('C3', 'C4', 'C5', 'C10'):
            owed.append(r)
        else:
            unsited.append(r)
    if final:
        unsited += owed
        owed = []
    out.append((not unsited, 'definition item 3: every binary-only row has a March parse site (%d sited%s)%s' % (
        sited, '' if final else '; %d owed to the phase that closes them: %s' % (
            len(owed), ', '.join('%s %d' % kv for kv in sorted(collections.Counter(r['owner'] for r in owed).items()))),
        '' if not unsited else ': ' + ', '.join('`%s` (%s)' % (r['key'], r['owner'] or 'no owner') for r in unsited[:12]))))

    # --- item 1, one line per damage source
    try:
        n, per, no_skill = damage_regression(fx_path)
        for src, hits in per:
            by_file = collections.Counter(f for f, _, _ in hits)
            disp = SYSTEM_DISPOSITION.get(src[4], ('build',))
            if hits and disp[0] in ('handed-off', 'declined'):
                out.append((True, 'definition item 1, %s: %s to %s (%s); %d of %d June DAMAGE_DATA* blocks depend on it' % (
                    src[0], disp[0], disp[1], disp[3], len(hits), n)))
                continue
            if hits:
                msg = ('definition item 1, %s: %d of %d June DAMAGE_DATA* blocks get damage only from it, and March lacks it '
                       '(owner %s, flag %s). By file: %s. First: %s' % (
                           src[0], len(hits), n, src[3], src[2] or 'not named yet',
                           ', '.join('%s %d' % kv for kv in by_file.most_common()),
                           ', '.join('%s/%s' % (nm, k) for _, nm, k in hits[:3])))
            else:
                msg = ('definition item 1, %s: 0 of %d June DAMAGE_DATA* blocks get damage only from it' % (src[0], n))
            if src[0].startswith('numerical'):
                msg += " (%d more set numerical data without a skill ID, where June's override does not run either)" % no_skill
            out.append((not hits, msg))
    except (IOError, ValueError, KeyError) as e:
        out.append((False, 'definition item 1: cannot read %s (%s); rebuild it with fxdata.june_fx()' % (fx_path, e)))

    if final:
        todo = [r for r in rows if r['status'] == 'to-implement']
        out.append((not todo, 'definition item 2 (--final): 0 `to-implement` rows%s' % (
            '' if not todo else ': %d, e.g. %s' % (len(todo), ', '.join('`%s`' % r['key'] for r in todo[:8])))))
        unbuilt = []
        for r in rows:
            if r['status'] != 'unsupported':
                continue
            d = disposition_of(r)
            flag = SYSTEM_DISPOSITION[_system_name(r)][2] if _system_name(r) in SYSTEM_DISPOSITION else None
            if not (d and d[0] == 'build' and _flag_landed(flag)):
                unbuilt.append(r)
        out.append((not unbuilt, 'definition item 4 (--final): every `unsupported` row is built (its flag defined and tested), '
                    'handed-off or declined%s' % ('' if not unbuilt else ': ' + ', '.join(
                        '`%s` (%s)' % (r['key'], r.get('system')) for r in unbuilt[:8]))))
    return out


def summary_lines(rows):
    ho = [r for r in rows if r['status'] in ('handed-off', 'declined')]
    own = collections.Counter(r['owner'] for r in rows if r['status'] in OPEN_STATUSES)
    lines = ['handed-off/declined: %d' % len(ho)]
    lines.append('open rows by owner: ' + ', '.join('%s %d' % kv for kv in sorted(own.items())))
    return lines


# Hand-written consumer notes for `implemented` rows, appended to the matrix's last column by census.note_s (so a regeneration
# keeps them). Keyed by the bare key name. C3 (JUNE2017_EFFECT_COMPLETION_PHASES.md "C3 - Done"): what the key does in March.
KEY_NOTES = {
    'ADD_MEMBER_COUNT': 'C3c: read by Process_ExtraDamage EDT_TRANSIT_DEBUFF (32034e50 @3203683c): above 0 runs the member loop; the count only gates, it is never a limit (JUNE_BUGS 133)',
    'ADD_MEMBER_COUNT_DISTANCE': 'C3c: read by the same member loop (@3203686d): the collection distance, 1000.0 when not above 0',
    'BOUNDING_BACK_SPEED_X': 'C3: parsed, stored, logged; reader = DamageReactStateChange 31feabd0 @31feb6c6 behind 31fd86b0 (C3b)',
    'BOUNDING_BACK_SPEED_Y': 'C3: parsed, stored, logged; reader = DamageReactStateChange 31feabd0 @31feb6d8 behind 31fd86b0 (C3b)',
    'FORCE_BACK_SPEED': 'C3: DamageReact (user 320462a0 @32046f56, NPC 327b1f30 @327b2efa) ported; C3c: the six DamageReactStateChange tests and the FORCE block (31feabd0, 31feb28f) ported',
    'APPLIED_MEMO_ID': 'C3: parsed and stored (+0x5c8), logged; reader = skill-memo switch 3202ac40 (C8b, queued)',
    'ATTACK_ONLY_ME': 'C3: six hit paths + both DamageReact invincible gates',
    'ONYL_ATTACK_USER': 'C3: UnitToUnit / MeshToUnit / CollisionDataToUnit / EffectToUnit',
    'ADD_CHECK_DAMAGE_UNIT_NUM': 'C3: AreaHit result count',
    'DAMAGE_LIGHT_TIME': 'C3: user DamageReact + AttackResult, NPC DamageReact',
    'EXCEPT_ME_HP_UP': 'C3: NPC DamageReact heal gate',
    'IGNORE_ATTACK_SUCCESS': 'C3: CEffect::AttackResult (bHit of the owner, 31fcee10); 31fcee40 counter not ported (C3b)',
    'INIT_EXTRA_DAMAGE_BEFORE_LOAD': 'C3: parse-time ExtraDamageData reset (31f8e430)',
    'PAUSE_NPC_AI_TIME': 'C3: GetFinalDamage -> SetTimePauseNPCAI (3275b330)',
    'DAMAGE_RATE_TO_SUPERAMOR': "C3: GetFinalDamage multiply for a super-armored defender (June's Luciel_Duo uses it)",
    'HIT_ADD_MP_PERCENT': 'C3: CEffect::AttackResult + CX2GUUser::AttackResult (322736f0)',
    'CREATE_DAMAGE_EFFECT_NAME_SCALE_X': 'C3: SetScale on RT_CREATE_DAMAGE_EFFECT children (3 sites)',
    'CREATE_DAMAGE_EFFECT_NAME_SCALE_Y': 'C3: see _X',
    'CREATE_DAMAGE_EFFECT_NAME_SCALE_Z': 'C3: see _X',
}
