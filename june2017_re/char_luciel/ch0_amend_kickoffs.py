"""char-luciel CH0 item 10: fill each kickoff's "Amended by CH0" section from the CH0 tsvs, write the kickoffs CH0 adds, retire the merged ones
and rewire the Depends lines.
    python june2017_re/char_luciel/ch0_amend_kickoffs.py [--dry]
Per phase: functions (ch0_class_map.tsv col 6), states (ch0_state_skill.tsv part 1 col 8), skills (part 2 col 6), tree rows (part 3 col 10),
checklist rows (ch0_checklist_rows.tsv col 5), callees routed to an Opus helper phase (ch0_unmapped_identified.tsv col 6), plus NOTES.
Replaces the placeholder only; a kickoff whose section is already filled is left alone. Retired kickoffs are deleted (git rm them).
"""
import os, re, sys, collections

HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..'))
K = os.path.join(T, 'kickoffs', 'char-luciel')
PLACE = '## Amended by CH0\n\n(empty until CH0 runs)'
DATE = '2026-11-02'
CM = 'june2017_re/char_luciel/ch0_class_map.tsv'


def tsv(name):
    return [l.rstrip('\r\n').split('\t') for l in open(os.path.join(HERE, name), encoding='utf-8', errors='replace')
            if l.strip() and not l.startswith('#')]


cm = [r for r in tsv('ch0_class_map.tsv') if r[0] != 'june_fn']
ss = open(os.path.join(HERE, 'ch0_state_skill.tsv'), encoding='utf-8', errors='replace').read().replace('\r\n', '\n').split('\n')
c2 = next(i for i, l in enumerate(ss) if l.startswith('# part 2'))
c3 = next(i for i, l in enumerate(ss) if l.startswith('# part 3'))
states = [l.split('\t') for l in ss[:c2] if l and not l.startswith('#') and not l.startswith('state\t')]
skills = [l.split('\t') for l in ss[c2:c3] if l and not l.startswith('#') and not l.startswith('skill_id\t')]
tree = [l.split('\t') for l in ss[c3:] if l and not l.startswith('#') and not l.startswith('class\t')]
ck = [r for r in tsv('ch0_checklist_rows.tsv') if r[0] != 'section']
ui = [r for r in tsv('ch0_unmapped_identified.tsv') if r[0] != 'callee']

RETIRED = {'CH1c': 'CH1b', 'CH1e': 'CH1d', 'CH1j': 'CH1i', 'CH2a0': 'CH3a0'}
# split phase -> (base kickoff, title, depends)
SPLITS = collections.OrderedDict([
    ('CH1f2', ('CH1f', 'Class core: Duo combos, part 2', 'CH1f review')),
    ('CH1g2', ('CH1g', 'Class core: Chiliarch and Royal Guard job states, part 2', 'CH1g review')),
    ('CH1k2', ('CH1k', 'Class core: Overrides, band 1, part 2', 'CH1k review')),
    ('CH1l2', ('CH1l', 'Class core: Overrides, band 2, part 2', 'CH1l review')),
    ('CH1l3', ('CH1l', 'Class core: Overrides, band 2, part 3', 'CH1l2 review, CH1l30 review')),
    ('CH2c2', ('CH2c', 'Royal Guard (38) skills, part 2', 'CH2c review')),
    ('CH2d2', ('CH2d', 'Diablura (39) skills, part 2', 'CH2d review, CH2d20 review')),
    ('CH3c2', ('CH3c', 'Demonio (129) skills, part 2', 'CH3c review, CH3c20 review')),
])
# Opus helper phase -> (title, depends, touches)
HELPERS = collections.OrderedDict([
    ('CH1a0', ('Engine pieces the class core calls first: the Lu/Ciel gauge class, the transform link, the hold-key selector, the switching guide',
               'CH0, NU1 review, DU1 review; one at a time in `X2GUUser.*`')),
    ('CH1k0', ('Engine helpers: the buff Time-finalizer passes', 'CH1a0 review; one at a time in `X2GUUser.*` / `X2GameUnit.*`')),
    ('CH1l30', ('Engine helpers: unit scans and the effect aim', 'CH1a0 review; one at a time in `X2GUUser.*` / `X2GameUnit.*`')),
    ('CH2d20', ('Engine helpers: the transform HUD', 'CH1 review; one at a time in `X2GUUser.*`')),
    ('CH3c20', ('Engine helpers: ProcessLockOn out of CX2GUAra, and the Garden of Haures NPC binders', 'CH1 review')),
])
DEPENDS = {'CH1a': 'CH1a0 review', 'CH1b': 'CH1a review; one at a time in `X2GULuciel.*`', 'CH1d': 'CH1b review; one at a time in `X2GULuciel.*`',
           'CH1f': 'CH1d review; one at a time in `X2GULuciel.*`', 'CH1g': 'CH1f2 review; one at a time in `X2GULuciel.*`',
           'CH1h': 'CH1g2 review; one at a time in `X2GULuciel.*`', 'CH1k': 'CH1i review, CH1k0 review; one at a time in `X2GULuciel.*`',
           'CH1l': 'CH1k2 review; one at a time in `X2GULuciel.*`', 'CH1m': 'CH1l3 review', 'CH2d': 'CH2c2 review',
           'CH3a': 'CH2d2 review, CH3a0 review', 'CH3d': 'CH3c2 review', 'CH4a': 'CH2d2 review'}
TITLES = {'CH1b': "Class core: Lu's common states (CH1c merged in)", 'CH1d': "Class core: Ciel's common states (CH1e merged in)",
          'CH1f': 'Class core: Duo combos, part 1', 'CH1g': 'Class core: Chiliarch and Royal Guard job states, part 1',
          'CH1i': 'Class core: Demonio script states and riding (CH1j merged in)', 'CH1k': 'Class core: Overrides, band 1, part 1',
          'CH1l': 'Class core: Overrides, band 2, part 1', 'CH2c': 'Royal Guard (38) skills, part 1', 'CH2d': 'Diablura (39) skills, part 1',
          'CH3c': 'Demonio (129) skills, part 1', 'CH3a0': 'Summoned NPCs: data rows for every Lu/Ciel summon'}

NOTES = {
 'CH0b': [
  '**Stage list `june2017_re/char_luciel/ch0b_stage_list.tsv`: 14,609 files** (14,607 missing, 2 replaced: `DLG_SKILL_Buff_01.tga` / `.tet`, NO_PIECE). '
  '`kom.py stage-june` dry run: 37 koms, 0 refused; **4 new kom dirs** with no live archive (`data162` 331, `data163` 294, `datai028` 250, `datai029` 245): '
  'pack them and ask before each copy. Routes a / b / c unioned in `ch0_assets.tsv` (17,227 rows; builder `python june2017_re/char_eve/ch0_build_assets.py luciel --out june2017_re/char_luciel`, always with `--out`).',
  '**Motions: 127, not 125** (data208 Lu 63, data218 Ciel 64; 101 `*Emotion*`). The 4 same-name files (`Motion_CSM_Lu_LDB.x`, `Motion_Lu_LDB_Evil.x`, '
  '`Motion_CSM_Ciel_LDN.X`, `Motion_Ciel_LDN_Evil.X`) are already June\'s bytes in the live data208 / data218: nothing to replace. March readers: '
  '`CSM_Lu_Diabla.lua` / `CSM_Ciel_Demonio.lua` (data037 monster cards) and `GameMajorXMeshPlayer.txt` (data036, the `_Evil` meshes).',
  '**Skill icons:** `skillicons.py check 10000-10601` and `check 30001-30606` FAIL until the 12 sheets are copied (11 NO_TEXTURE: `DLG_Luciel_SKILL_ScreenShot_01-03`, '
  '`DLG_SKILL_LUCIEL_Active_01-05`, shared `DLG_SKILL_Passive_09 / _11 / _15`; `DLG_SKILL_Buff_01` NO_PIECE): all in the list (data063). Gate: both checks PASS after the copy.',
  '`POPUP_TR_NOBLESSE`, `POPUP_TR_D_LORD`, `POPUP_TR_DEMONIO` (+ `.tet`, data215) are in the list. The `HQ_Cutin_{LLD,LCN,LDN}_{LU,CIEL}_Transcendence` cut-ins are already June bytes.',
  '**Lua is not in the stage list** (June `0x82` bytecode goes through `adopt-june` by its owner): `Luciel_Duo` / `Luciel_Demonio` and `DLG_LUCIEL_BOUNDING_KEY.lua` CH1; '
  '`DLG_LUCIEL_TRANSFORM_SKILL_SLOT.lua`, `DLG_Luciel_CombinationPoint.lua` CH5; `DLG_Class_Change_Luciel_*` (6) and the 4 guide dialogs CHN; '
  '`DLG_Unit_Create_ClassChange_Button_Luciel` NU4; `Tutorial_Luciel_Stage0` (+ `_LineMap`) TU.',
  'Effect closure `ch0_effect_closure.tsv`: 13,049 rows, 3,815 names, 572 missing (557 in the list; `Ciel_Claw04.ogg`, `Luciel_Shadow_Fury04.ogg`, `WhiteImage.tga`, '
  '`__Motion_HAURES2_Ciel_LDN.X` are in no June kom; 15 templets June never defines, e.g. `LDN_Bounding_Lu_StoneFloor`): nothing to stage for those.',
  'About 14k of the rows are Lu/Ciel shop / costume meshes and `VoicePack_Lu_*`: the character\'s June assets go in whole (user rule), so they stay.'],
 'CH1a': [
  '**Depends on CH1a0** (gauge class, transform link `IsTransformedForm` / `SetTransformedForm` 31fc9d90 / 31fc9d50, switching guide): CH1a calls them by their March names.',
  'Class-map names for CH1a: ctor `32b1bd90`, dtor `32b39380`, scalar dtor `32b4e660`, own virtuals 0x880 `32b14db0` FindDamageEffectIndexEntry, '
  '0x884 `32b3acf0` `?` (a skill-use check with a `[S%1%]%2%` message, shaped like `CanUseSpecialAttack` 0x7f8: name it), 0x888 `32ae6130` PreloadAuraAccelResources; '
  'MemoConditionLuciel ctor `32aeb3f0` (vtable `334f036c`; its IsCondition `32ae5150` is CH4c\'s).',
  '**States enum** for `X2GULuciel.h`: 515 registered ids (7-592; 7-130 shared USI, 154-592 the class\'s); 585 LSI names across both scripts (28 alias lines in '
  'Duo); neither June\'s `Enum.lua` nor the live one has an `LSI_` entry. State ids are 16-bit already (`SERV_IRUHADEV_JUNE_USER_STATE_ID16`, `Always.h:6188`).',
  '**June ids are not March\'s USI from 24 up:** June inserts a state at 24 (`USI_DAMAGE_GROGGY` is 25 in June, 24 in March\'s enum). Name ids from `ch0_reg.tsv`, never by counting March\'s enum.',
  '**Register `NUI_SA_LDN_GARDEN_OF_HAURES` = 2269** in `X2UnitManager.h` under `SERV_IRUHADEV_JUNE_ENUM_NAMES`: the dtor (`@32b394c7`) and ReInit (`@32b37713`, CH1k2) look the NPC up '
  '(`ch0_summons.tsv` last row). 1906 / 1908 are already named.'],
 'CH1b': ['**CH1c is merged in** (CH0): every Lu common state, movement and the rest (41 functions). The state table\'s col 8 says CH1b for all of them.'],
 'CH1d': ['**CH1e is merged in** (CH0): every Ciel common state (30 functions).'],
 'CH1f': ['**Correction to the plan:** `LSI_A_*` states are actives (skills, CH2 / CH3 by class), not combos. CH1f / CH1f2 are the `LSI_LU_COMBO_*` / `LSI_CIEL_COMBO_*` states '
          '(77 functions, split in table order).'],
 'CH1i': ['**CH1j is merged in** (CH0): the 3 riding functions (`LSI_RIDING_*`). The rest of riding is `CX2GUUser` code.',
          'Checklist 7ac L777: `32ae9b40` (LDB dash-jump combo XZ landing) calls the user landing prediction `32021390`, which Aisha ported class-local: hoist it or take a second local copy.'],
 'CH1k': ['Overrides below vslot 0x49c (except the state-id getters, CH1l), names in col 4 with the base member in col 5. **Depends on CH1k0** (buff Time-finalizer passes).',
          '14 overrides stay `?` with a hint in the note (`0x2b0`, `0x2e4` = `0x2fc` one body, `0x300`, `0x318`, `0x580`, `0x62c`, `0x640`, `0x648`, `0x64c`, `0x658`, `0x6b0`, '
          '`0x848`, `0x87c`): name each from its base and body by March\'s pattern before transcribing.'],
 'CH1k2': ['The state-id getters from 0x438 up are here and in CH1l: base returns a June state id, Lu/Ciel return the Lu id or Ciel\'s (+0x2d) while `IsTransformedForm( 2 )`.'],
 'CH1l': ['The DU2 getters (0x114, 0x118, 0x458, 0x48c, 0x490, 0x494) and the rest of the band from 0x49c: names from June\'s registration ids (`ch0_reg.tsv`), not March\'s enum.'],
 'CH1l3': ['**Depends on CH1l30** (unit scans, effect aim).'],
 'CH1m': ['Registration map `ch0_reg.tsv`: 364 rows (InitStateCommon 294, InitState 65 = 3 rolls + `LSI_WAIT_STATE_CHANGE` + 42 BOUNDING_TARGET + 19 LDB dash combos, user event states 5); '
          '`ch0c_listing_check.py` 364 / 364 (5 std::string-named rows: rolls 9 / 11 / 12 from `31fe22d0`, hyper 23 / 204 via `31fdd490`, 8 names by class).',
          'InitState `32b43d10` sets `+0x8ac8` by class (37 / 38 / 39 / 200 -> 0, 127 -> 1, 128 -> 2, 129 -> 3; no registration is gated on it) and pushes **ten delegate callbacks** '
          '(checklist 7am): declare them, bodies in CH4a (`32b14bc0` AddCombinationPointOnHit is CH1p\'s).'],
 'CH1n': ['`ch0_skill.tsv` (listing emulator `ch0_skill_listing_emu.py`; the setter is not carved): **64 cases, 112 skill ids**; slot state ids Lu 50-59, Ciel 60-69 (SLOTB). '
          '12 states are not set here: 4 are registered in InitStateCommon (`LSI_SA_LLC_DARK_CRESCENT`, `_RUINOUS_IMPACT`, `LSI_SA_LCG_ESCHATON`, `_FANTASIE_IMPROMPTU_2ND_MOVEMENT`), '
          'the transform states 30044-30046 / 30050-30052 via vslot 0x57c `GetTransformedSkillStateID`. Check skill 30023 (Noblesse, state named LCG, linked by tail name).'],
 'CH1o': ['June-only keys and owners in `ch0_keys.tsv` (357 rows, counts = the plan\'s census). The keys `CX2GUUser::CommonStateStart(Future)` parses (`DUO_TYPE`, `MINOR_UNIT_*`, `CP_AMOUNT`, '
          '`USE_NOT_SELECT_*`, `USE_PHYSICPROCESS_NOT_SELECT_UNIT`, `FORCE_DYNAMIC_CAMERA_INDEX` @3205c0e8, `RETAIN_CAMERA_STATE` @3205b934, `SUPER_ARMOR_CUSTOM_COLOR` @3205941f) are DU1\'s parse; '
          'the class only reads the members. `DEFAULT_UNSELECTED_UNIT_MP_CHARGE_VALUE_PER_SECOND(_AT_PVP)` is read by InitComponent 0x3c `32b0f080` (CH1k).',
          'No June reader (dead data, nothing to port): `HEAD_IK`, `HEAD_IK_MAX_ANGLE`, `MINOR_SPEED_TIME0` (Duo L7969 typo), `STOP_TIME_ATK` (Demonio typo of `STOP_TIME_ATT`), '
          '`DAMAGE_DATA_MEMO`, `STATE_STRING`. Immunity keys (`31fdaaa0`) stay on the Stage 2 bullet, as for Chung.'],
 'CH1p': ['`AddCombinationPointOnHit` `32b14bc0` (the `HIT_ADD_COMBINATION_POINT` consumer, a delegate InitState pushes) is this phase\'s; the engine reader is `32632390` @3263dfd2.',
          '**`Enum.lua` also needs the `CUSTOM_RENDER_STATE` table** (checklist agent; `Luciel_Demonio` uses it in 3 states and a missing table stops the file there): add it with `DT_BOTH` / `DT_KEEP`, before CH1 adopts the scripts.'],
 'CH1': ['Adopt `DLG_LUCIEL_BOUNDING_KEY.lua` (data034) with the scripts: Init 0x40 `32b1e8e0` loads it at `32b1ea23`. `luabind.py check` all three.'],
 'CH2a': ['First owner class 200 (prefixes `LD` / `LUCIEL`): states in `ch0_state_skill.tsv` col 8, skills col 6. Skill 208 (no tree row) and its `LSI_SA_[F|M]_COMMON_AURA_DEPRAVITY_ACCEL` are here.'],
 'CH2b': ['Prefix `LLC` = 37 (also owned by 127).'],
 'CH2c': ['Prefix `LCG` = 38 (also owned by 128). Split in table order with CH2c2.'],
 'CH2d': ['Prefix `LDB` = 39 (also owned by 129). The transform states (`IsTransformedForm`, linked skill slots) are CH1a0\'s engine pieces; the transform HUD is CH2d20\'s (CH2d2 depends on it).'],
 'CH2d2': ['**Depends on CH2d20** (transform HUD).'],
 'CH3a0': ['**Moved from CH2a0** (CH0): the first spawn is Dread Lord\'s (`ch0_summons.tsv`). One spawn rule for the class (CreateNPCReq from a state FrameMove); the Garden NPC\'s own Lua spawns are CH3c20\'s.',
           'Scope: June\'s `NPCTemplet.lua` rows (core list) for 1906 `NUI_SI_SA_LLD_SUMMON_DARKNESS`, 1937 `_PVP`, 1908 `NUI_SI_SA_LCN_SHADOW_BOLT`, 2269 `NUI_SA_LDN_GARDEN_OF_HAURES`, '
           '2270 `NUI_SA_LDN_GARDEN_OF_HAURES_ADD`; their `NPC_UNIT_ID` names in the live `Enum.lua` (core list) and 2270 in `X2UnitManager.h` (2269 is CH1a\'s); their motions / meshes are in CH0b\'s list. '
           '1907 `NUI_SI_A_LCN_SHADOW` gets nothing (June ships no script and nothing spawns it). `luabind.py check` PASS for 1906 / 1937 / 1908 / 2270.'],
 'CH3a': ['**Depends on CH3a0** (summon NPC rows). Prefix `LLD` = 127. Summon Darkness spawns 1906 from helper `32b20260` (`@32b20486`).'],
 'CH3b': ['Prefix `LCN` = 128. Shadow Bolt spawns 1908 from `LSI_SA_LCN_SHADOW_BOLT_START_FrameMove` `32b2fba0` (`@32b2fe73`).'],
 'CH3c': ['Prefix `LDN` = 129. `CUSTOM_RENDER_STATE` (3 Demonio states) is this phase\'s (`ch0_keys.tsv`; parser `31fce500`, called from `32057b80` @32060740). Split with CH3c2.'],
 'CH3c2': ['**Depends on CH3c20** (ProcessLockOn, Garden binders). Garden of Haures spawns 2269 from `32b33dc0` (`@32b340ce`).'],
 'CH3d': ['7 class functions (type 13 / transcendence rows). Cut-ins `HQ_Cutin_{LLD,LCN,LDN}_{LU,CIEL}_Transcendence` are June bytes already; `POPUP_TR_*` are CH0b\'s.'],
 'CH4a': ['15 functions: InitPassiveSkillState 0x690 `32b45820`, the buff-value setters 0x24c / 0x250, and **nine of InitState\'s ten delegate callbacks** (checklist 7am; '
          '`32b3d380`, `32b0fc50`, `32b43590`, `32b435f0`, `32b0fce0`, `32b177b0`, `32b13c80`, `32b17f90`, `32b3e470`) with what only they reach. 35 skill ids are passives here. '
          'Checklist 7: the slot-251 Soul Collector hook (`3204dcd0`, buff post-apply) has no March counterpart: add it with its passive.'],
 'CH4b': ['No class function carries CH4b: SetDamageExceptionProcess 0x2b8 `32aea530` is transcribed in CH1k. This phase wires the damage-exception checklist rows below.'],
 'CH4c': ['`MemoConditionLuciel::IsCondition` `32ae5150` (`31fd4450( arg ) != 0`) and its slot-2 thunk `32ae5170`.'],
 'CH5': ['Adopt `DLG_LUCIEL_TRANSFORM_SKILL_SLOT.lua` and `DLG_Luciel_CombinationPoint.lua` (data034). The switching guide (`322e6300`) is CH1a0\'s, the transform HUD CH2d20\'s.'],
 'CH6': ['S3 baseline `june2017_re/char_luciel/ch0_s3_baseline.txt`: 301 tree rows, 301 no_state (March has no class), 137 distinct ids; rerun with `s3_no_state.py` (Lu/Ciel families added by CH0).'],
 'CHN': ['Adopt the 6 `DLG_Class_Change_Luciel_*` (data217) and the 4 guide dialogs (`DLG_1ST` / `DLG_2ND_Class_Change_Guide_Luciel`, `Class_Change_Guide_Luciel` (+ `_1`), data034).'],
 'TU': ['221 asset rows carry owner TU in `ch0_assets.tsv` (col 9).'],
 'DU1': ['**CH1a0 waits for this review** (the transform link sits beside the duo members, and CH1a calls `31fd6260`).',
         'Keys `CX2GUUser::CommonStateStart(Future)` parses for Lu/Ciel (`ch0_keys.tsv`), beyond the list NU0 gave: `FORCE_DYNAMIC_CAMERA_INDEX` @3205c0e8, `RETAIN_CAMERA_STATE` @3205b934, '
         '`SUPER_ARMOR_CUSTOM_COLOR` @3205941f (CH3c reads it), `MINOR_UNIT_AFTER_IMAGE` @3205b69b, `MINOR_UNIT_SPEED_X` @32052a1b / @320535e4; `MINOR_UNIT_MOTION_FILE_NAME` in InitMotion @31fe5fc6. '
         'InitSystem `3202b9e0` reads `LU` / `CIEL` / `DUNGEON_SPEECH_LUCIEL` (@3202bcf4 / @3202bd56 / @3202bc76).'],
 'DU2': ['CH0 names the getters from the June registration ids (`ch0_hand_names.tsv`): 0x114 GetDamageDownFrontStateID (30), 0x118 GetDamageDownBackStateID (31), 0x48c '
         'GetDamageAirDownInvincibleStateID (36), 0x490 ...AirDownLanding (37), 0x494 ...AirFall (38), 0x458 GetJumpDownStateID. the Lu/Ciel bodies are transcribed in CH1k2 / CH1l.'],
}


def phase_block(p):
    out = []
    fn = [r for r in cm if r[5] == p]
    tr = [r for r in fn if r[1] != 'thunk']
    if fn:
        out.append('- **Functions: %d to transcribe** (+ %d thunks) (`awk -F\'\\t\' \'$6=="%s"\' %s`; all new).' % (len(tr), len(fn) - len(tr), p, CM))
        out.append('  June addresses: ' + ' '.join('`%s`%s' % (r[0], '' if r[3] in ('?', '-') else ' %s' % r[3]) for r in tr) + '.')
        q = [r[0] for r in tr if r[3] == '?']
        if q:
            out.append('  Still `?` (name it by the class pattern from the hint in col 7): ' + ' '.join('`%s`' % x for x in q) + '.')
    base = p.rstrip('0123456789') if p[-1].isdigit() and p not in HELPERS else p
    st = [r for r in states if len(r) > 7 and r[7] == base]
    if st and base == p:
        out.append('- **States: %d** (`awk -F\'\\t\' \'$8=="%s"\' june2017_re/char_luciel/ch0_state_skill.tsv`): %s.' % (len(st), p, ', '.join('`%s`' % r[0] for r in st)))
    elif st:
        out.append('- **States:** the ones whose callbacks are listed above (the state table\'s col 8 keeps the unsplit `%s`).' % base)
    sk = [r for r in skills if len(r) > 5 and r[5] == p]
    if sk:
        out.append('- **Skills: %d** (part 2 of `ch0_state_skill.tsv`, col 6): %s.' % (len(sk), ', '.join('%s %s' % (r[0], r[1]) for r in sk)))
    tw = [r for r in tree if len(r) > 9 and r[9] == p]
    if tw:
        out.append('- Tree rows: %d (part 3, col 10).' % len(tw))
    rows = [r for r in ck if len(r) > 4 and re.search(r'(^|\s)%s(\s|$)' % re.escape(p), r[4])]
    if rows:
        out.append('- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): ' + '; '.join('%s %s %s' % (r[0], r[1], r[2][:80]) for r in rows) + '.')
    cs = [r for r in ui if len(r) > 5 and r[5] == p]
    if cs:
        out.append('- **Callees to map and port** (`ch0_unmapped_identified.tsv` col 6):')
        out += ['  - `%s` %s (%s; callers\' phases %s)' % (r[0], r[2][:150], r[3], r[7]) for r in cs]
    for n in NOTES.get(p, []):
        out.append('- ' + n)
    return out


def helper_kickoff(hp, title, dep):
    cs = [r[0] for r in ui if len(r) > 5 and r[5] == hp]
    return '\n'.join([
        '# June character Lu/Ciel (Stage 8) - Phase %s - %s (Opus)' % (hp, title), '',
        '| | |', '|---|---|',
        '| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel %s` |' % hp,
        '| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |',
        '| Depends on | %s |' % dep,
        '| Flag | the owning shared flag (`SERV_IRUHADEV_JUNE_CHAR_*` for `CX2GUUser` / engine code, class-agnostic) or `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` |',
        '| Log tag | none (inert until `_LIVE`; the phase that uses it play-tests it) |',
        '| Review | kickoffs/char-luciel/%s-review.md (Opus 5.5 high; the phase writes it when done) |' % hp, '',
        '## Read first (and nothing else from the plan doc unless you need it)', '', '```sh',
        'python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Phase CH0 - Lu/Ciel class map (read-only)" "CH0 - Done (2026-11-02)"',
        "awk -F'\\t' '$6==\"%s\"' june2017_re/char_luciel/ch0_unmapped_identified.tsv" % hp,
        '```', '', '## Task', '',
        'CH0 found callees outside Lu/Ciel\'s class range that no doc maps and that a Sonnet phase would stop on ("this needs Opus"). '
        'Map each in Ghidra, name it, give it a March home (an existing March function when June\'s namesake does the same; else a new one under the flag, '
        'by the owning class\'s pattern, no `June` in the name; class-agnostic code stays in `CX2GUUser` / the engine), and port its whole body (June\'s numbers '
        'and quirks verbatim, a `JUNE_BUGS.md` row each). A funcmap row per function (`Char Luciel %s`) so the class phases call it by its March name. '
        'Check first whether Chung\'s, Eve\'s or Aisha\'s port already holds it (`funcmap.tsv`, `junediverge.py --list`).' % hp, '',
        'You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only. '
        'Done gate: every address below has a funcmap row and a March body (or a recorded reason it needs none); the build passes; '
        '`junediverge.py --check` and `enumscan.py --check` PASS.', '',
        'No hardcoded enum literals and no `June` prefix (CLAUDE.md rules): use the member, register June\'s name and value under a flag if C++ lacks it; '
        'names say what the thing is, never an address or phase tag.', '',
        '## Amended by CH0 (%s)' % DATE, ''] + phase_block(hp) + [''])


def main():
    dry = '--dry' in sys.argv
    w = (lambda path, s: None) if dry else (lambda path, s: open(path, 'w', encoding='utf-8', newline='\n').write(s))
    # CH2a0 -> CH3a0
    src, dst = os.path.join(K, 'CH2a0.md'), os.path.join(K, 'CH3a0.md')
    if os.path.exists(src) and not os.path.exists(dst):
        s = open(src, encoding='utf-8').read().replace('CH2a0', 'CH3a0')
        s = re.sub(r'(\| Depends on \| )[^\n]*', r'\g<1>CH2d2 review', s, count=1)
        s = s.replace('"Phase CH2 - Skills: base and first jobs (200, 37, 38, 39)"', '"Phase CH3 - Skills: second jobs (127, 128, 129), hyper actives, transcendence"')
        w(dst, s)
        print('new CH3a0 (from CH2a0)')
    for p, (base, title, dep) in SPLITS.items():
        dst = os.path.join(K, p + '.md')
        if os.path.exists(dst):
            continue
        s = open(os.path.join(K, base + '.md'), encoding='utf-8').read()
        s = re.sub(r'^(# .*? - Phase )%s - .*$' % re.escape(base), r'\g<1>%s - %s' % (p, title), s, count=1, flags=re.M)
        s = s.replace('char-luciel %s`' % base, 'char-luciel %s`' % p).replace('%s-review.md' % base, '%s-review.md' % p)
        s = s.replace('phase `%s`' % base, 'phase `%s`' % p).replace('"%s"' % base, '"%s"' % p).replace('`%s` row' % base, '`%s` row' % p)
        s = re.sub(r'(\| Depends on \| )[^\n]*', r'\g<1>' + dep + ('; one at a time in `X2GULuciel.*`' if p.startswith('CH1') else ''), s, count=1)
        w(dst, s)
        print('new', p)
    for hp, (title, dep) in HELPERS.items():
        dst = os.path.join(K, hp + '.md')
        if not os.path.exists(dst):
            w(dst, helper_kickoff(hp, title, dep))
            print('new', hp)
    for f in sorted(os.listdir(K)):
        if not f.endswith('.md') or f.endswith('-review.md'):
            continue
        p = f[:-3]
        if p in RETIRED or p in ('CH0', 'NU0') or p.startswith('TEMPLATE'):
            continue
        path = os.path.join(K, f)
        if dry and not os.path.exists(path):
            continue
        s = open(path, encoding='utf-8').read()
        s2 = s
        if p in DEPENDS:
            s2 = re.sub(r'(\| Depends on \| )[^\n]*', r'\g<1>' + DEPENDS[p], s2, count=1)
        if p in TITLES:
            s2 = re.sub(r'^(# .*? - Phase %s - ).*$' % re.escape(p), r'\g<1>' + TITLES[p], s2, count=1, flags=re.M)
        s2 = s2.replace("'$<phase column>==", "'$6==")
        if PLACE in s2:
            body = phase_block(p) or ['- Nothing in the CH0 maps carries this phase\'s id; the plan section is the scope.']
            s2 = s2.replace(PLACE, '## Amended by CH0 (%s)\n\n' % DATE + '\n'.join(body))
        elif p in SPLITS and '## Amended by CH0 (' in s2:
            s2 = s2[:s2.index('## Amended by CH0 (')] + '## Amended by CH0 (%s)\n\n' % DATE + '\n'.join(phase_block(p)) + '\n'
        elif '## Amended by CH0' not in s2 and phase_block(p):
            s2 = s2.rstrip('\n') + '\n\n## Amended by CH0 (%s)\n\n' % DATE + '\n'.join(phase_block(p)) + '\n'
        if s2 != s:
            w(path, s2)
            print('amended', p)
    for p in RETIRED:
        path = os.path.join(K, p + '.md')
        if os.path.exists(path):
            print('retire', p, '-> git rm', os.path.relpath(path, T))


if __name__ == '__main__':
    main()
