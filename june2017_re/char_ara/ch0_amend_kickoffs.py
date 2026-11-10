"""char-ara CH0: fill each kickoff's "Amended by CH0" section from the CH0 tsvs, and write the six kickoffs CH0 splits off.
    python june2017_re/char_ara/ch0_amend_kickoffs.py [--dry]
Copy of char_rena/ch0_amend_kickoffs.py. Per phase: functions (ch0_class_map.tsv col 7) with inside / new, states (ch0_state_skill.tsv part 1 col 8),
skills (part 2: col 6, the split halves follow the skill's states col 5), checklist rows (ch0_checklist_rows.tsv col 5), force-power rows
(ch0_fp.tsv col 7), Specter rows (ch0_specter.tsv col 7), CH-N rows (ch0_classn.tsv col 7), identified callees (ch0_unmapped_identified.tsv col 8),
plus the hand notes in NOTES. Replaces the placeholder paragraph only; a filled section is left alone. No needs-opus callee rows: no helper phase.
"""
import os, re, sys, collections

HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..'))
K = os.path.join(T, 'kickoffs', 'char-ara')
PLACE = '(CH0 fills this: the filter with real column numbers, the function / state / skill counts and June addresses, checklist rows, any split.)'
DATE = '2026-11-10'
D = 'june2017_re/char_ara/'
CM = D + 'ch0_class_map.tsv'
PHASES = ('CH0b CH1a CH1b CH1c CH1d CH1d2 CH1e CH1f CH1g CH1h CH1i CH1j CH1k CH1m CH1n CH1o CH1p CH1q CH1q2 CH1 CH5 CHNa CHNb '
          'CH2a CH2b0 CH2b CH2b2 CH2c0 CH2c CH2c2 CH2d CH2d2 CH2d3 CH3a CH3a2 CH3b CH3c CH3c2 CH3d CH4a CH4b CH6').split()
ALIAS = {'CH1q3': 'CH1q'}   # a stale tag in ch0_fp.tsv: no CH1q split, the rows are CH1q's
TOK = re.compile(r'CH(?:\d[0-9a-z]*|N[ab])')


def tsv(name):
    p = os.path.join(HERE, name)
    if not os.path.exists(p):
        return []
    return [l.rstrip('\r\n').split('\t') for l in open(p, encoding='utf-8', errors='replace') if l.strip() and not l.startswith('#')]


def owners(cell):
    """phases named in a cell, parenthesised notes ignored (a note may mention another phase)"""
    c = re.sub(r'\([^)]*\)', ' ', cell)
    out = []
    for t in TOK.findall(c):
        t = ALIAS.get(t, t)
        if t in PHASES and t not in out:
            out.append(t)
    return out


cm = [r for r in tsv('ch0_class_map.tsv') if r[0] != 'june_fn']
ss_all = open(os.path.join(HERE, 'ch0_state_skill.tsv'), encoding='utf-8', errors='replace').read().split('\n')
cut = next(i for i, l in enumerate(ss_all) if l.startswith('# part 2'))
states = [l.split('\t') for l in ss_all[:cut] if l and not l.startswith('#') and not l.startswith('state\t')]
skills = [l.split('\t') for l in ss_all[cut:] if l and not l.startswith('#') and not l.startswith('skill_id')]
state_phase = {r[0]: r[7] for r in states if len(r) > 7}
ck = [r for r in tsv('ch0_checklist_rows.tsv') if r and r[0] != 'section']
fp = [r for r in tsv('ch0_fp.tsv') if r and r[0] != 'march_fn']
sp = [r for r in tsv('ch0_specter.tsv') if r and r[0] != 'piece']
cn = [r for r in tsv('ch0_classn.tsv') if r and r[0] != 'site']
ui = [r for r in tsv('ch0_unmapped_identified.tsv') if r and r[0] != 'callee']


def skill_phases(r):
    """a skill's phase(s): col 6; when col 6 is a split parent, the phases of its states (col 5)"""
    base = r[5]
    ph = []
    for s in re.split(r'[,;]\s*', r[4]):
        p = state_phase.get(s.strip())
        if p and p.startswith(base) and p not in ph:
            ph.append(p)
    return ph or [base]


SPLITS = collections.OrderedDict([   # new kickoff: (copied from, title scope, depends, scope sentence, bullet dropped from the copy)
    ('CH2b2', ('CH2b', 'Skills: Little Hsien (28) A tier (ids >= 27200) (CH0 split of CH2b)', 'CH2b',
               'the A tier, skill ids >= 27200 (CH2b has the SA tier, ids < 27200)', '- Swallow Assault')),
    ('CH2c2', ('CH2c', 'Skills: Little Devil (31) A tier (ids >= 27200) (CH0 split of CH2c)', 'CH2c',
               'the A tier, skill ids >= 27200 (CH2c has the SA tier, ids < 27200)', '- `SA_ALD_WOLF_Break`')),
    ('CH2d2', ('CH2d', 'Skills: Little Specter (34) ids 27024-27199 (CH0 split of CH2d)', 'CH2d',
               'skill ids 27024-27199 (CH2d has ids < 27024 and the states with no skill, CH2d3 the A tier)', None)),
    ('CH2d3', ('CH2d', 'Skills: Little Specter (34) A tier (ids >= 27200) (CH0 split of CH2d)', 'CH2d2',
               'the A tier, skill ids >= 27200 (CH2d has ids < 27024 and the states with no skill, CH2d2 ids 27024-27199)', None)),
    ('CH3a2', ('CH3a', 'Skills: Sakra Devanam (118) skills with smallest id >= 27012 (CH0 split of CH3a)', 'CH3a',
               'the skills whose smallest id is >= 27012 (CH3a has the smallest id < 27012 and the states with no skill)', None)),
    ('CH3c2', ('CH3c', 'Skills: Asura (124) A tier (ids >= 27200) (CH0 split of CH3c)', 'CH3c',
               'the A tier, skill ids >= 27200 (CH3c has the SA tier, ids < 27200)', '- `ASI_SA_ASR_FOX_PHANTOM`')),
])
# parents: (title suffix, scope sentence)
PARENTS = {
    'CH2b': (', SA tier', 'the SA tier, skill ids < 27200 (CH2b2 has the A tier)'),
    'CH2c': (', SA tier', 'the SA tier, skill ids < 27200 (CH2c2 has the A tier)'),
    'CH2d': (', ids < 27024', 'skill ids < 27024 and the states with no skill; no review of its own: the CH2d3 review covers it (CH2d2 has ids 27024-27199, CH2d3 the A tier)'),
    'CH3a': (', smallest id < 27012', 'the skills whose smallest id is < 27012 and the states with no skill (CH3a2 has the rest)'),
    'CH3c': (', SA tier', 'the SA tier, skill ids < 27200 (CH3c2 has the A tier)'),
}
SCOPE_ANCHOR = "(a skill's line decides its phase)."


NOTES = {   # the CH0 hand notes (C:/tmp/ara_ch0/notes.md), verbatim, one bullet per entry
 'CH0b': [
  "Stage list `june2017_re/char_ara/ch0b_stage_list.tsv` 5,886 rows; dry run `STAGE-JUNE DRY RUN: 34 kom(s), 5886 staged (5562 new, 324 replaced, 0 identical), 0 refused`; one new kom **data205** (56 June-only motions: `Motion_Ara_LS.x`, `Motion_Ara_AA.x`, `Motion_Standard_Ara.X`, 53 emotions): pack it and copy it as a new kom (Rena's data201 precedent). Differing class motions -> data033; `Motion_Ara_Tail_03.x` / `_06.x` -> data131.",
  "Held for CH1 final, not in the list (`ch1_held_motions.tsv`): `Motion_Ara_LD.x` (drops `SI_SA_ALD_ENERGY_WAVE_Start` / `_End`: June's file has `_START` / `_END`, March's lookup is case-sensitive) and `Motion_Ara_SD.x` (drops `SI_SA_ASD_FallingBlossom_Start` / `_Finish`). `_ARA`, `_Field`, `_Emotion`, `_LH`, `_YR` go now.",
  '`adopt-june` (bytecode, then `luabind.py check`): the 7 `CH0b-adopt` rows of `ch0_assets.tsv` (col 3 owner/phase): `DLG_Class_Change_Ara_LH` / `_LD` / `_LS` / `_SD` / `_YR` / `_AS`, `DLG_Class_Change_Guide_Ara`; plus `DLG_1ST_Class_Change_Guide_Ara.lua` / `DLG_2ND_Class_Change_Guide_Ara.lua` (data034, absent in March; CHNb wires them through LevelUpEvent).',
  "`POPUP_TR_S_DEVANAM` / `_Y_RAJA` / `_ASURA` (+ `.TET`, data215) are in the list. Movies (`Movie_Create_Ara_AA/_Base/_SD/_YR.avi`) are loose and identical: nothing to stage. All 41 cut-in `.gfx` are already June's; 31 `HQ_CutIn_Ara_*` textures are in the list.",
  "Icon gate: `python tools/skillicons.py check 106 900 27000-27066 27200-27244 27400-27420 27600-27605`. Before staging: 74 misses on 5 sheets, all 10 sheet files in the list. One June gap: 27414 piece `SI_P_AA_SPIRITUAL_EYE` absent from June's own `DLG_ARA_SKILL_ScreenShot_04.TET` -> `JUNE_BUGS.md` row + `skillicons_june_gaps.tsv` row.",
  'Effect closure `ch0_effect_closure.tsv` 7,600 rows: 169 unique missing, 136 files all in the list; 33 are names June itself lacks (dangling).',
  'Walk units to create (none exist): classes 7, 28, 118, and a level-99 31 (`createchar.py`, after a `db_backup`).',
 ],
 'CH1a': [
  'Hyper and berserk tables: the four berserk tables (`ASI_BERSERK_MODE` and the ASD / AYR / AA variants) are CH1a like the hyper tables; their registration is CH1m. `ASI_JUMP_ATTACK_Z` / `_X` are CH1a by the `JUMP*` prefix.',
  'Force-power lines in these states (`FP_CANCEL`, `ADD_FORCE_POWER`, the shared anim-end helper `32babba0`) go under `_FP` inside the `_LIVE` body: CH1q writes the helpers (`ch0_fp.tsv`).',
 ],
 'CH1b': [
  "`ASI_WALL_LANDING` is registered by June's C++ with no script table (state_skill note): transcribe the callbacks, no table needed.",
 ],
 'CH1c': [
  "June's script defines `ASI_RIDING_DAMAGE_FRONT` twice (the second wins) and June registers `ASI_RIDING_DAMAGE_BACK` with no table; `ASI_RIDING_JUMP_READY` is dead data. Keep verbatim; `JUNE_BUGS.md` row for the duplicate.",
 ],
 'CH1d2': [
  'All `ASI_DASH*` without a line token are here.',
 ],
 'CH1e': [
  "`ASI_COMBO_ALH_XXXZX_LANDING` (script table) has no exe string; June registers `ASI_COMBO_ALH_XXXZX_Landing` (different case, no table): keep June's registration verbatim, `JUNE_BUGS.md` row. Registered `P_*` states (quick-stand, quicken-guard, brutal-piercing) follow their line token into CH1e.",
 ],
 'CH1g': [
  "`ASI_SA_ALS_SPECTER_OF_STORM_DEGREE` is registered by June's C++ with no script table. Specter-flag lines under `_SPECTER` (CH1q2 switches them on).",
 ],
 'CH1h': [
  '35 functions. Overrides named by CH0: 0xd0 `32bc19a0` = `CreateGageData` moved to CH1q; 0x29c `32bd32f0` = `GetReduceDebuffDurationRate` moved to CH4a; 0x300 `32ba47f0` (CEffect hook, skill 27411 power rate) CH4b; 0x328 `32ba6440` (float hook, passives 27417 / 27420) CH4a; 0x318 `32bc8be0` (berserk regen hook -> `32bc5bc0`) CH1q; 0x2fc `32bab1d0` (my-unit-only call `32bab160( arg+0x5c4, arg+0x5e8 )`) stays here, name unsettled: name it from the body.',
  "InitComponent `32bb0da0` reads the 7 June-only `INIT_COMPONENT` keys into: `+0x9610` / `+0x9611` (FP count at dungeon / PvP start, bytes), `+0x96fc` (default 9), `+0x9704`, `+0x9808` (vector<float>), `+0x9720` (vector<int>), `+0x9730` (`ch0_fp.tsv`): the reads are written here under `_FP`; their readers are CH1q's. Max FP is the constant 10 in Init `32bc7e90` / ReInit `32bc8030` (March: `min(level/10, 6) + 4`): under `_FP`.",
  '`AppendMajorParticleToDeviceList` `32b96640` (March has it) is here. `AppendDeviceToDeviceList` `32bd86b0` / `AppendComponentToDeviceList` `32bd86e0` are empty in June and March: nothing to port. `32bd86c0` is unreferenced (dead).',
  'June-only record vector at Ara `+0x9858` (`{USHORT, int, vector<12-byte T>}`; STL helpers `321f3fe0` / `32ed3950` / `32a79680`): declare the member and record type here.',
 ],
 'CH1i': [
  "14 functions. 0x670 `32ba4710` = `IsInJobLine( int )` (Chung's name and CH1o decision): 1 = classes 28 / 118, 2 = 31 / 119, 3 = 34 / 124 (the 34 / 124 arm is live code only once CHNa loads them). 0x6b4 `32ba4620`: replacement state id for skills 27014-27039 (writes state ids 262 / 271 / 301 / 311): name it from the body. 0x74c `32b956d0` returns true. 0x688 `32b90da0` is a jmp to the base `31fcd740` (same as base): n/a. 0x634 `32baf270` = `UpdateEquipCollisionData` moved to CH1q2 (tail list). 0x68c `32bd34c0` = `InitAquiredActiveSkillState` moved to CH1n.",
 ],
 'CH1k': [
  '`ReservedSkillStart` (Rules). `CanUseSpecialAttack` `32bcfaa0` takes a bool (1 = FP-cancel probe) and reads `+0x9621`; `SpecialAttackEventProcess` `32b90c20` saves and restores it: the `+0x9621` lines go under `_FP`.',
  "`GetPressedSkillState` `31fd3360` differs from March (slot B at index > 4, 5 slots per bank): `june_march_divergent.tsv` row added by CH0, replaced in CH1p (7ag); CH1k's transcription of `CanUseSpecialAttack` calls it under the allow row CH0 added (`june_diverge_allow.tsv`). `32d278f0` (RidingPet dying state) is CH1b's: March's `m_DyingState` + a stand-in comment.",
 ],
 'CH1m': [
  '`ch0_reg.tsv` 162 rows (`charport_states.py init 32bca580 32bcf4a0 3202b110 32bcced0`); `ch0c_listing_check.py` 162 / 162. Five std::string rows checked by hand: 23 `USI_HYPER_MODE` (`31fdd490`), 155 `ASI_BERSERK_MODE` (`31fdd800`), 9 / 11 / 12 START / WIN / LOSE roll (`31fe22d0`, base callbacks).',
  "June's `USI_END` is 152, March's 100: every one of the 105 shared `ASI_` names differs by 52. Register by June's id (`ch0_state_enum_names.tsv`, 350 rows; 138 June registration names are not in the C++ enum). June registers `ASI_A_ALS_SHAKE_CHAOS`'s table at both 291 (its Enum value) and 333 (`32bd1c95`, `32bd4d7c`, no Enum name): 333 stays a number with a one-line comment.",
 ],
 'CH1n': [
  "`SetEquippedSkillFuncBySkillSlotIndex` `32bd09d0` switches: pre-switch at `32bd0af5` (V = `0x6a56` / `0x6a6c` -> helper `32babb50` on both states); type switch (type - 5, 12 entries, byte table `32bd282c`, jump table `32bd281c`: 5 -> `ASI_COMMON_BUFF`, 6 / 7 / 13-16 -> id switch, 12 -> `ASI_THROW_ITEM` (id 900), 8-11 exit); id switch (id - 0x6978, 244 entries, byte table `32bd2930`, jump table `32bd2838`, 62 targets, default `32bd271e`); id 27600 by compare (`32bd2685`); hyper switch (id - 0x6bd1, 5 entries, jump table `32bd2a24`). 86 cases hand setup to class helpers `helper( state*, id )`: each helper is `inside` this function with its state's phase.",
  '**Second switch:** vslot 0x68c `32bd34c0` = `InitAquiredActiveSkillState( skill id )` (March `X2GUUser.h:3039`, `X2GUAra.cpp:16262`; called by `X2ComboAndSkillChainSystem.cpp:98`): 56 states (id - 0x6979, 242 entries, byte table `32bd5150`, jump table `32bd50f4`). Its head is CH1n; its case bodies go with their state phases. `ch0_skill_listing_rows.tsv` (246 rows: 190 from `32bd09d0`, 56 from `32bd34c0`).',
 ],
 'CH1o': [
  '2 rows left by the map (unsettled callers). `GetDefenceRateBySkill` 0x428 `32b956a0` is CH4b (l.940).',
 ],
 'CH1q': [
  "`ch0_fp.tsv` (114 rows; relation: differs 56, unchecked 27, june-only 15, same 14, absent 2) is this phase's list: about 28 class functions + the gauge side (data 7, UI 5, manager 1, emulator) = about 44: **no split** (gauge rows are mostly `same`). The class map carries 23 CH1q functions (hand rows in `ch0_hand_phases.tsv`).",
  'Facts: force power lives in the gauge (`[this+0xc04]`: now +0x114, max +0x120, berserk byte +0x110, changed +0x111, FP-changed +0x112; getters `32a31a00` / `32a31a20`); `SetForcePower` `32ba6ef0` converts a spend into MP: (now - new) x 20.0 x spec B (`+0x6d04`, March\'s `GetCharacterSpecializationB`), my unit only; `IsFullHyperMode` 0x9c `32ba6cc0` (`+0x9744` > 0 ? buff hyper count `+0x6844` > 0 : gage berserk byte); `SetFullHyperMode` 0xa0 `32b96a00` (March\'s two gage writes + `+0x9700` = 0); `+0x9621` = FP-cancel probe (36 writers, readers `32b90c20` / `32bcfaa0`); June folds March\'s 22 inline "anim end: FP cancel or StateChange" blocks into `32babba0`; `UpForcePower` is inlined at about 7 sites with the `+0x9644` overflow-to-MP; `UseForcePower` `32bb2b40` merges March\'s `UseForcePower` + `ConsumeForcePower`; `CForcePowerSystem` is write-only in March and absent in June; berserk per-hit roll `32bc9a60` (FULL_HYPER_MODE_ADVANTAGE_*), regen `32bc5bc0` via the 0x318 hook `32bc8be0`; `ApplyHyperModeBuff` 0xc0 `32bab210` tail `32baae50` (GAIN_FORCEPOWER_USEDHYPERCOUNT / GAIN_MP_BY_FPCONVERSION_HYPERMODE); `CreateGageData` 0xd0 `32bc19a0`.',
  "Gauge: June `CX2AraGageData` vtable `3349b124` = March's six overrides (packet path sets max 10, `UpdateDataFromGameUnit` skips the changed byte); `CX2AraMyGageUI` vtable `3353351c` = March's. Gauge create for 34 / 124 is CHNa's. Emulator `CAC_FORCE_POWER` (3) matches June.",
  'Check: berserk portrait refresh `32e69060` compares gage +0x111 with +0x110, which `SetFullHyperMode` writes both to 1: confirm, and a `JUNE_BUGS.md` row if June never refreshes.',
 ],
 'CH1q2': [
  '`ch0_specter.tsv` (19 rows), about 14 functions / members: `IS_SPECTER_STATE` is read by `CX2GUUser::CommonStateStartFuture` `320506f0` (into `+0x7da9`) and `CommonStateStart` `32057b80` (into `+0x8049`; the census label "Lire ReservedSkillStart" is wrong) into `StateParam::bSpecterState` (+0x95); 40 tables (basic combo / dash / jump-attack / stand-up attack). One reader: Ara\'s `AttackResultByType` `32bceca0` @`32bcee1c` (Specter of Thunder buff: cooldown `+0x979c`, chance `+0x9790` from BTI 424 `BTI_BUFF_SPECTEROFTHUNDER` / skill 27023 via `32ba6d00` / `32b90d20`). Register BTI 423 / 424 under JUNE_ENUM_NAMES (March has neither). No class test: in practice only 34 / 124 reach it.',
  "Tails: `ANIM_NAME_TAIL` / `TRANSITION_TAIL` are March's already (`X2GUAra.cpp:6219`; June `ChangeAnimTail` `32bd7280` is the same). June-only: `TAIL_TYPE` (`LoadPromotionItemID` `32bab890`, class cases 34 / 124, vector at +0x95d8), `InitPromotionEquipList` `32baec10`, tail show `32badd50` / `32bae120` (index min(buff count, 3) - 1), `UpdateEquipCollisionData` 0x634 `32baf270`; items 41786 / 41787 (3 / 6 tails, in the live `item.kim`), 41580 (9 tails). No class test: all 7 classes, gated by the buff hyper count.",
 ],
 'CH1': [
  "Held motions `Motion_Ara_LD.x` / `_SD.x` (`ch1_held_motions.tsv`). Before deciding the LD case: June's script also plays the mixed-case `SI_SA_ALD_ENERGY_WAVE_Start` (src.lua:14486, 14614) against a file with `_START`: check June's animation lookup (March `KTDGXSkinAnim.cpp:2131` is case-sensitive); if June's is case-sensitive too it is June's bug (keep, `JUNE_BUGS.md` row).",
 ],
 'CH5': [
  '`ch0_unittemplet_ara.tsv`: 7 / 28 / 31 add `m_fLobbyRotationY` only (1 / 15 / 10); 118 / 119 add `m_fLobbyRotationY` (1 / 15), `m_HyperCutIn` (`HQ_Cutin_ASD_Transcendence` / `_AYR_`) and `m_BerserkCutIn` (`HQ_Cutin_ASD_1_Transcendence` / `_AYR_1_`); 34 (24 keys, `STR_ID_31537` / `_31539`, `Motion_Ara_LS.x`, `Movie_Create_Ara_AA.avi`) and 124 (27 keys, `STR_ID_31538` / `_31540`, `m_GameMotion2 = Motion_Ara_AA.x`) are new. All string ids are in `general.ess`. The templet loader has no `UC_END` refusal: the 34 / 124 rows go in here (CHNa needs them).',
  "The live data033 already holds June's `Motion_ARA_Common.x` and `Motion_ARA_Lobby.x`; they lack `ALD_lobbyWait` (June: `ALD_LobbyWait`; played at `X2SquareUnit.cpp:1079`, `X2UnitViewerUI.cpp:2173`) and `AYR_LobbyReadyAtcion` (`X2UnitViewerUI.cpp:5260`): class 31 / 119 lobby idles. Check whether those sites are live and what June plays there; fix in this phase (Ara's lobby).",
 ],
 'CHNa': [
  "`ch0_classn.tsv` (45 rows) is the list. Live breaks for 34 / 124: `X2SocketItem.cpp:2897` (`>= UC_END` drops 996 of 5,235 rows incl. Asura's 75; June `3271a810` accepts 1..202: bound 202, never renumber `UC_END`; flag `_CLASS_PATH`); `GetX2UnitClassCompatibility` (`X2Unit.h:1456-2172`, June `31ed6500`: 34 accepts {7, 34}, 124 accepts {7, 34, 124}); **`X2GageManager.cpp:180-190` has no 34 / 124 arm while `X2GUAra.cpp:3028` static_casts the gauge to `CX2AraGageData`: a crash, fix first**; `X2Unit.cpp:3021` `GetClassLevel` (-1; June `3234c170`: 34 = 1, 124 = 2; divergence row); `tools/createchar.py` (`CLASSES` / `PARENT` lack 34 / 124).",
  "Stat rows: `X2OfflineStatTable.cpp:355-369` falls back to `UnitTypeOf(class)`'s rows silently once CH5's templet rows exist (no log on success: add one); per Open decision 1 the stat rows follow Ara's own rule (server Lua, user packs).",
  "Not needed (verified): `X2GUUser.cpp:534-545` (dead behind the GFX talk cut-in early return), `X2OfflineLuaEnumSeed.h:117-118` (not a bound test), `X2ItemManager_Preprocessing.cpp:688` / `X2GameEdit.cpp:1643` (log / dev only: optional), `AnimationNameTable.lua` (already June's, `ALS_*` / `AA_*` present). `CorrelateUnitClassTemplet.lua` is March's (no 34 / 124 chain; read only by the skill-note memo list): adopt June's.",
 ],
 'CHNb': [
  "The quest route is already in place: the shipped `FieldQuest.lua` is June's; 35560-35563 (`CJT_JOB_THIRD`, class 7, level 15; 35563 rewards `UC_ARA_LITTLE_SPECTER`), 35564-35568 (class 34, level 35; 35568 rewards `UC_ARA_ASURA`); `Handlers_Quest.cpp:586-620` accepts any class; auto-learn has 14 / 23 rows.",
  '41794 / 41796 / 41803 / 41805 are Eun-mode hair / face costume pieces (PROMOTION_ITEM_ID_LIST), not class-change items. `X2OfflineClassChangeSeed.h` is cash-item only: PARENT_ROWS 34 -> 7, 124 -> 34 optional (generated file: regenerate, never hand-edit).',
  'Left for this phase: the two `LevelUpEvent.lua` rows (level 15 -> `DLG_1ST_Class_Change_Guide_Ara`; level 35 adds 34 and the 2ND dialog), the dialogs (staged in CH0b), `luabind.py check`, the play-test. Small: may fold into one session.',
 ],
 'CH2a': [
  "48 functions (whole). The `32babba0` FP-cancel helper is CH1q's (called here).",
 ],
 'CH2b': [
  '**Swallow Assault is not here:** June has it as a Sakra Devanam skill (`RNSI_SA_ASD_SWALLOW_ASSAULT` 27011; March\'s `SI_SA_ALH_SWALLOW_ASSAULT` 7007): CH3a. June moved several skills across lines (`ch0_passive_ids.tsv` "tag-mapped" rows): the line in June\'s tree decides the phase, not March\'s name. `CalcLockOnLandingPos` `32bc0420` is not the lock-on\'s: CH2d. Split: CH2b = SA tier (ids < 27200), CH2b2 = A tier.',
 ],
 'CH3a': [
  "Swallow Assault (`ASI_SA_ALH_SWALLOW_ASSAULT_*`, 27011; `ATTACK` EventProcess `@32bc204c`, `_FINISH` StateStart `@32bb70ff`) is here and runs on CH2b0's `ProcessLockOn`, passing June's `NULL, &lineIndex` (checklist l.775-776). Split: CH3a = skills with the smallest id < 27012 (and states with no skill), CH3a2 = the rest. `BUFF_FACTOR_ID_ULTIMATE` (l.121): re-check the reader gate.",
 ],
 'CH2b0': [
  'The plan\'s "landing helper `32bc0420`" is Specter of Thunder\'s (CH2d), not the lock-on\'s: this phase is `3200fa20` and its candidate-loop `CanDown` only. `SLockOnSystem::CreateLockOnSystem` `32b4cf30`: June\'s struct has one extra float at `+0x30`: add the member here. The first caller is Swallow Assault in CH3a (a 118 skill in June), so CH2b0 must land before CH3a (it does).',
 ],
 'CH2c': [
  "Split rule (key = the smallest skill id of the state's skills): CH2c SA / CH2c2 A (>= 27200); CH2d ids < 27024 and states with no skill / CH2d2 27024-27199 / CH2d3 A; CH3c SA / CH3c2 A. CH2a (48, an SA / A split would leave 6), CH2b (45), CH3b (44), CH2d (49) stay whole. The CH2 stage review moves to CH2d3; the CH3 stage review stays at CH3d.",
  "7w `+0x8e64` writers in CH2d (`32ba1d2d` / `32ba1ddb`) and CH2d2 (`32bbe885` / `32bbec3f`): the same labelled stand-in as CH3c's Fox Phantom. Reserved-buff path (`BUFF_FACTOR_ID_AND_APPLY_TIME`, checklist l.868): first Ara use in CH2d / CH3b / CH3c.",
 ],
 'CH2c2': [
  "Split rule (key = the smallest skill id of the state's skills): CH2c SA / CH2c2 A (>= 27200); CH2d ids < 27024 and states with no skill / CH2d2 27024-27199 / CH2d3 A; CH3c SA / CH3c2 A. CH2a (48, an SA / A split would leave 6), CH2b (45), CH3b (44), CH2d (49) stay whole. The CH2 stage review moves to CH2d3; the CH3 stage review stays at CH3d.",
  "7w `+0x8e64` writers in CH2d (`32ba1d2d` / `32ba1ddb`) and CH2d2 (`32bbe885` / `32bbec3f`): the same labelled stand-in as CH3c's Fox Phantom. Reserved-buff path (`BUFF_FACTOR_ID_AND_APPLY_TIME`, checklist l.868): first Ara use in CH2d / CH3b / CH3c.",
 ],
 'CH2d2': [
  "Split rule (key = the smallest skill id of the state's skills): CH2c SA / CH2c2 A (>= 27200); CH2d ids < 27024 and states with no skill / CH2d2 27024-27199 / CH2d3 A; CH3c SA / CH3c2 A. CH2a (48, an SA / A split would leave 6), CH2b (45), CH3b (44), CH2d (49) stay whole. The CH2 stage review moves to CH2d3; the CH3 stage review stays at CH3d.",
  "7w `+0x8e64` writers in CH2d (`32ba1d2d` / `32ba1ddb`) and CH2d2 (`32bbe885` / `32bbec3f`): the same labelled stand-in as CH3c's Fox Phantom. Reserved-buff path (`BUFF_FACTOR_ID_AND_APPLY_TIME`, checklist l.868): first Ara use in CH2d / CH3b / CH3c.",
 ],
 'CH2d3': [
  "Split rule (key = the smallest skill id of the state's skills): CH2c SA / CH2c2 A (>= 27200); CH2d ids < 27024 and states with no skill / CH2d2 27024-27199 / CH2d3 A; CH3c SA / CH3c2 A. CH2a (48, an SA / A split would leave 6), CH2b (45), CH3b (44), CH2d (49) stay whole. The CH2 stage review moves to CH2d3; the CH3 stage review stays at CH3d.",
  "7w `+0x8e64` writers in CH2d (`32ba1d2d` / `32ba1ddb`) and CH2d2 (`32bbe885` / `32bbec3f`): the same labelled stand-in as CH3c's Fox Phantom. Reserved-buff path (`BUFF_FACTOR_ID_AND_APPLY_TIME`, checklist l.868): first Ara use in CH2d / CH3b / CH3c.",
 ],
 'CH3a2': [
  "Split rule (key = the smallest skill id of the state's skills): CH2c SA / CH2c2 A (>= 27200); CH2d ids < 27024 and states with no skill / CH2d2 27024-27199 / CH2d3 A; CH3c SA / CH3c2 A. CH2a (48, an SA / A split would leave 6), CH2b (45), CH3b (44), CH2d (49) stay whole. The CH2 stage review moves to CH2d3; the CH3 stage review stays at CH3d.",
  "7w `+0x8e64` writers in CH2d (`32ba1d2d` / `32ba1ddb`) and CH2d2 (`32bbe885` / `32bbec3f`): the same labelled stand-in as CH3c's Fox Phantom. Reserved-buff path (`BUFF_FACTOR_ID_AND_APPLY_TIME`, checklist l.868): first Ara use in CH2d / CH3b / CH3c.",
 ],
 'CH3c2': [
  "Split rule (key = the smallest skill id of the state's skills): CH2c SA / CH2c2 A (>= 27200); CH2d ids < 27024 and states with no skill / CH2d2 27024-27199 / CH2d3 A; CH3c SA / CH3c2 A. CH2a (48, an SA / A split would leave 6), CH2b (45), CH3b (44), CH2d (49) stay whole. The CH2 stage review moves to CH2d3; the CH3 stage review stays at CH3d.",
  "7w `+0x8e64` writers in CH2d (`32ba1d2d` / `32ba1ddb`) and CH2d2 (`32bbe885` / `32bbec3f`): the same labelled stand-in as CH3c's Fox Phantom. Reserved-buff path (`BUFF_FACTOR_ID_AND_APPLY_TIME`, checklist l.868): first Ara use in CH2d / CH3b / CH3c.",
 ],
 'CH1p': [
  'Replace `CX2GUUser::GetPressedSkillState` (June `31fd3360`: B slot at index > 4, index - 5) with the 7ag reader set, then drop the CH0 allow row for `CX2GUAra::CanUseSpecialAttack`.',
 ],
 'CH2d': [
  '`ASI_A_ALS_SHAKE_CHAOS` is registered at 291 and at 333 (no Enum name for 333: a number with a comment). `ASI_SA_ALS_SPECTER_OF_BLADE` / `_FINISH` are dead data. `CalcLockOnLandingPos` `32bc0420` and `FindSpecterOfThunderLockOnTarget` `32bc5f80` are here; `ASI_SA_ALS_SPECTER_OF_STORM_LANDING` EventProcess `32ba6220`.',
 ],
 'CH3c': [
  "`ASI_SA_ASR_FOX_PHANTOM`'s `CUSTOM_RENDER_STATE`: labelled stand-in (7w).",
 ],
 'CH4a': [
  '0x29c `32bd32f0` `GetReduceDebuffDurationRate` (27415 tail, l.267); 0x328 `32ba6440` float hook (27417: FP-gated roll adding skill value 0x1f; 27420: count on state skill 27216). `ch0_passive_ids.tsv`: 65 March `SI_*` reads, 41 differ (March 7000s vs June 27000s), 9 matched across class tags (e.g. `SI_SA_ALH_PEERLESSNESS_SLASH` 7005 = `RNSI_SA_ASD_PEERLESSNESS_SLASH` 27012), 12 dead branches. `ch0_enum_names.tsv`: 136 of 139 June ids need registering.',
 ],
 'CH4b': [
  '0x300 `32ba47f0` CEffect hook (skill 27411: effect +0x3f0 x (100 - `+0x9740`) x 0.01 when `+0x973c` != 0). MemoCondition `32b955c0` / `32ba46f0`.',
 ],
}


def awk(col, p, f, loose=False):
    if loose:
        return "awk -F'\\t' '$%d ~ /(^| )%s( |$)/' %s%s" % (col, p, D, f)
    return "awk -F'\\t' '$%d==\"%s\"' %s%s" % (col, p, D, f)


def phase_block(p):
    out = []
    fn = [r for r in cm if len(r) > 6 and r[6] == p]
    if fn:
        c = collections.Counter(r[4] for r in fn if r[4])
        out.append('- **Functions: %d** (`%s`, col 7 = phase; col 5 inside / new; %s).' % (
            len(fn), "awk -F'\\t' '$7==\"%s\"' %s" % (p, CM), ', '.join('%s %d' % kv for kv in sorted(c.items()))))
        out.append('  June addresses: ' + ' '.join('`%s`%s' % (r[0], '' if r[4] == 'inside' else '(%s %s)' % (r[4] or '-', r[5])) for r in fn) + '.')
    st = [r for r in states if len(r) > 7 and r[7] == p]
    if st:
        out.append('- **States: %d** (`%s`, part 1, col 8 = phase): %s.' % (len(st), awk(8, p, 'ch0_state_skill.tsv'), ', '.join('`%s`' % r[0] for r in st)))
    sk = [r for r in skills if len(r) > 5 and r[0].isdigit() and p in skill_phases(r)]
    if sk:
        out.append('- **Skills: %d** (`ch0_state_skill.tsv` part 2, col 6 = the class-level phase; a split half takes the skills whose states (col 5, then part 1 col 8) are its own): %s.' % (
            len(sk), ', '.join('%s %s%s' % (r[0], r[1], '' if len(skill_phases(r)) == 1 else ' (states also in %s)' % '/'.join(x for x in skill_phases(r) if x != p)) for r in sk)))
    rows = [r for r in ck if len(r) > 4 and p in owners(r[4])]
    if rows:
        out.append('- **Checklist rows: %d** (`%s`; col 5 may name several phases): %s.' % (
            len(rows), awk(5, p, 'ch0_checklist_rows.tsv', True), '; '.join('%s %s' % (r[1], r[2][:60]) for r in rows)))
    for name, data, f in (('Force-power rows', fp, 'ch0_fp.tsv'), ('Specter / tail rows', sp, 'ch0_specter.tsv'), ('CH-N (class path) rows', cn, 'ch0_classn.tsv')):
        n = [r for r in data if len(r) > 6 and p in owners(r[6])]
        if n:
            rel = ''
            if f == 'ch0_fp.tsv':
                rel = '; col 5 relation: ' + ', '.join('%s %d' % kv for kv in sorted(collections.Counter(r[4] for r in n).items()))
            out.append('- **%s: %d** (`%s`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count%s).' % (name, len(n), awk(7, p, f, True), rel))
    cal = [r for r in ui if len(r) > 7 and p in owners(r[7])]
    if cal:
        out.append('- **Identified callees first needed here: %d** (`%s`, col 8 = earliest caller phase; none needs Opus): %s.' % (
            len(cal), awk(8, p, 'ch0_unmapped_identified.tsv', True), ' '.join('`%s`' % r[0] for r in cal)))
    for t in NOTES.get(p, []):
        out.append('- ' + t)
    return out


def write(path, s, dry):
    if not dry:
        open(path, 'w', encoding='utf-8', newline='\n').write(s)


def main():
    dry = '--dry' in sys.argv
    # 1. new kickoffs, copied from the parent as it stands before CH0 edits it
    for p, (base, title, dep, scope, drop) in SPLITS.items():
        dst = os.path.join(K, p + '.md')
        if os.path.exists(dst):
            continue
        s = open(os.path.join(K, base + '.md'), encoding='utf-8').read()
        s = re.sub(r'^(# .*? - Phase )%s - .*$' % base, lambda m: '%s%s - %s' % (m.group(1), p, title), s, count=1, flags=re.M)
        s = s.replace('`/x2-phase char-ara %s`' % base, '`/x2-phase char-ara %s`' % p)
        s = re.sub(r'^(\| Depends on \| )[^.]*\.', lambda m: m.group(1) + dep + '.', s, count=1, flags=re.M)
        if p == 'CH2d3':
            s = re.sub(r'^\| Review \|.*$', lambda m: '| Review | `kickoffs/char-ara/CH2d3-review.md` (Opus 5.5 high; the phase writes it when done); the CH2 stage review: covers CH2a-CH2d3, retires `_SKILL_DEBUG` |', s, count=1, flags=re.M)
        elif p.startswith('CH2'):
            s = re.sub(r'^\| Review \|.*$', lambda m: '| Review | none of its own: the CH2d3 review covers it (its `_DEBUG` define stays on) |', s, count=1, flags=re.M)
        s = s.replace(SCOPE_ANCHOR, SCOPE_ANCHOR + ' **CH0 split: this phase is %s.**' % scope, 1)
        if drop:
            s = ''.join(l for l in s.splitlines(True) if not l.startswith(drop))
        write(dst, s, dry)
        print('new', p)
    # 2. parents and dependants
    for p, (suffix, scope) in PARENTS.items():
        path = os.path.join(K, p + '.md')
        s = open(path, encoding='utf-8').read()
        s2 = re.sub(r'^(# .*? - Phase %s - .*?)$' % p, lambda m: m.group(1) if m.group(1).endswith(suffix) else m.group(1) + suffix, s, count=1, flags=re.M)
        if 'CH0 split:' not in s2:
            s2 = s2.replace(SCOPE_ANCHOR, SCOPE_ANCHOR + ' **CH0 split: this phase is %s.**' % scope, 1)
        if p == 'CH2b':
            s2 = ''.join(l for l in s2.splitlines(True) if not l.startswith('- Swallow Assault'))
        if p == 'CH2d':
            s2 = re.sub(r'^\| Review \|.*$', lambda m: '| Review | none of its own: the CH2d3 review covers it (its `_DEBUG` define stays on) |', s2, count=1, flags=re.M)
        if s2 != s:
            write(path, s2, dry)
            print('parent', p)
    DEPS = {'CH2a': [('the CH2d review covers it', 'the CH2d3 review covers it')],
            'CH2b': [('the CH2d review covers it', 'the CH2d3 review covers it')],
            'CH2c': [('the CH2d review covers it', 'the CH2d3 review covers it'), ('| CH2b, CH2c0 review.', '| CH2b2, CH2c0 review.')],
            'CH2d': [('| CH2c, CHNb review', '| CH2c2, CHNb review')],
            'CH3a': [('| CH2d review.', '| CH2d3 review.')],
            'CH3b': [('| CH3a.', '| CH3a2.')],
            'CH3d': [('| CH3c.', '| CH3c2.')]}
    for p, subs in DEPS.items():
        path = os.path.join(K, p + '.md')
        s = open(path, encoding='utf-8').read()
        s2 = s
        for a, b in subs:
            s2 = s2.replace(a, b, 1)
        if s2 != s:
            write(path, s2, dry)
            print('dependant', p)
    # 3. the amended sections
    for f in sorted(os.listdir(K)):
        if not f.endswith('.md') or f.endswith('-review.md'):
            continue
        p = f[:-3]
        path = os.path.join(K, f)
        s = open(path, encoding='utf-8').read()
        if PLACE not in s:
            print('no placeholder', p)
            continue
        body = phase_block(p) or ["Nothing in the CH0 maps carries this phase's id; the plan section is the scope."]
        s2 = s.replace('## Amended by CH0\n\n' + PLACE, '## Amended by CH0 (%s)\n\n' % DATE + '\n'.join(body))
        if s2 != s:
            write(path, s2, dry)
            print('amended', p)


if __name__ == '__main__':
    main()
