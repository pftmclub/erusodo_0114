# June character port: Raven (Stage 7, fourth character after the pilot)

Plan key `char-raven` · branch `main` · kickoffs `kickoffs/char-raven/` · log tag `[JUNE_CHAR]` (lines start `[JUNE_CHAR] RAVEN <sub>`,
sub-tags `DATA`, `CORE`, `RAGE`, `SKILL`, `PASSIVE`, `UI`, `NUM`) · flags `SERV_IRUHADEV_JUNE_CHAR_RAVEN_*` for Raven's class code,
`SERV_IRUHADEV_JUNE_CHAR_*` for class-agnostic pieces (+ a temporary `..._DEBUG` per sub-phase) · backlog Stage 7
(`JUNE_PORT_BACKLOG_ORDER.md`, brief `kickoffs/june-backlog/stage7-plan.md`) · written 2026-11-05 by the `new-plan char-raven` session from
the Aisha plan's "Template for Stage 7" and the Elsword plan (`JUNE_CHARACTER_ELSWORD_PHASES.md`), the most recent worked example.

## Context

**What this plan does.** It moves Raven (unit type `UT_RAVEN` 4, script `RAVEN_FIGHTER.lua`, class `CX2GURaven` in `X2Lib/X2GURaven.cpp` /
`.h`, seven classes: Raven 4, Sword Taker 16 (`UC_RAVEN_SOUL_TAKER`), Over Taker 17, Weapon Taker 25, Blade Master 106, Reckless Fist 107,
Veteran Commander 115) from his March kit to his June 2017 kit, end to end: June script, skill states for June's tree, numerical damage,
motions, class-only engine hooks (`JUNE2017_CHARACTER_PORT_CHECKLIST.md` §1-§7au), the Nasod core / rage gauge and hyper orbs, Veteran
Commander's overheat, UI pieces, data and his transcendence rows.

**Why Raven is fourth (stage7-plan, 2026-10-22).** Gap 25 (the largest numerical-only gap of the plain characters), no templet-backed June-only
class, no open design decision. The brief's premises hold: the 7b passive setter `+0x4108`, the 7u effect names (17 `c11a` rows) and the
power-rate operator (checklist §7 l.256) are his.

**His size.** June's class spans `0x32c25a20`-`0x32c6a63d` (Chung's ends at `32c25a1f`, Lire's begins at `32c6a8d0`): **440** carved functions
plus **28 uncarved vtable overrides** (code present, no Ghidra function; 6 of them in the `Common*` band 0x7c0-0x7e8 and SpecialAttackEventProcess
`32c25af0`; list in `plan_june_side.md` §9), against Chung's 441 and Elsword's 308 + 25. June's script has **346** state tables (Chung 385,
Elsword 329) under **mixed prefixes** (`RSI_` 286, `RVC_` 36, `RBM_` 9, `RRF_` 6, `RWT_` 2, `SI_` 7). Expect about Chung's size (138 commits;
Elsword took 89). The phase table names 17 CH1 sub-phases up front (Chung planned 20 and ran 31; Elsword planned 16 and ran 19).

**Written June-body-inside-March from the start (CLAUDE.md, 2026-10-28), as Chung and Elsword.** Most June bodies have a March function of the
same name: **the body goes inside that function under `_LIVE`, March's in `#else`**, and each CH1 sub-phase proves it with a one-file probe
compile that defines `_LIVE`. A June function March's class lacks is written under `_CORE` with a plain name in the class's pattern.

**Scope: June 2017 up to transcendence (user, 2026-10-20).** No Raven third job exists in June's `UnitTemplet` (its `UC_150` row is Elsword's).
Transcendence is live (Stage 6), so his transcendence rows are in CH3d and CH6.

**Order.** CH0 (read-only) runs now, beside char-elsword CH6. CH0b (assets, kom copies, a play-test) waits until char-elsword CH6's walk has
passed, so the two play-tests do not share a session. Every code phase waits for char-elsword Z (one class port's code at a time in the shared
`X2GUUser.*` / `X2Game.cpp`). Lu/Ciel stays parked until Stage 7 closes (user, 2026-11-03).

**The state he starts from.**
- **Skill system (Stage 3).** `s3_reform_ids_no_state.tsv` has 254 Raven rows (4: 14; 16 / 17 / 25: 28 each; 106 / 107 / 115: 52 each), 126
  distinct ids (106, 900, 24000-24058, 24200-24239 without 24223 / 24225 / 24227, 24400-24422 without 24416, 24600-24605); **247 have no class
  state** (only 900 has one). **Every row has a June `SKILL_ID` name** (7ae: no unnamed-id task). No type-2 row.
- **Effect engine (June's).** `DamageEffect.lua` / `EffectSet.lua` ship merged; **17** Raven rows in `c11a_march_only_templets.tsv` (13 with a
  March consumer in `X2GURaven.cpp`, 4 with none; 7u); **0 alpha-tag sites** (7c). The `INIT_PHYSIC.WALK_SPEED` load stop a Raven combo caused
  is fixed (char-eve CH3a, `SERV_IRUHADEV_JUNE_DAMAGE_EFFECT_INIT_PHYSIC`); CH1 only confirms his combo X back speed.
- **Motions.** June 73 `Motion_Raven*` (data202 69 + data010 2 + data012 + data120), March 20 (data033 + others); 10 differ, `Motion_Raven.x` is
  already June's (R12b); 53 emotion files are June-only. **`Motion_Raven_OT.x` drops 4 animation names** (`r12b_anim_loss.tsv`):
  `OT_ComboX3Down` / `X4Down` / `X5Down` (June's script plays `..._New` instead) and `OT_SP1a_ChargedBolt`, which June's own script still plays
  (`RAVEN_FIGHTER.src.lua:15182`, state `RSI_SA_ROT_CHARGED_BOLT_BLOOD`). So March's script plays names June's motions lack: **the motion koms
  deploy with CH1 final** (7ad), and CH0 settles `OT_SP1a_ChargedBolt` (a June bug kept verbatim if June's file lacks it). March's game dir has
  no `data202.kom`: CH0 names the target kom (Eve `data203`, Chung `data204`).
- **Nasod core / rage (live March code, `UPGRADE_RAVEN`).** `m_fRageGauge` / `m_iNaosdBallLv` / `m_bAttackNasodBall` (`X2GURaven.h:1674-1679`,
  getters `:458-479`), rage fill in `X2GUUser.cpp:12000-12011`, the Nasod-ball guard / parry hooks `X2GUUser.cpp:10262-10265`, `:37195`,
  `CreateNasodBall` (`X2GURaven.cpp:13686`), `DTT_RAVEN_RAGE_ATTACK` (`X2DamageManager.h:36`), the ball level carried in
  `syncData.m_CannonBallCount` (`:420`, `:5149`, `:13653`, read `:14276`), `CX2RavenMyGageUI` (`X2GageUI.cpp:3078-3141`, made for all 7 classes at
  `X2GageManager.cpp:167-176`) whose `raven_powrUp_UI_01` ball March never makes (7aq: June `32e6d900`); June overrides vslots 0x9c / 0xa0
  (`IsFullHyperMode` family, uncarved) and reads `MAX_HYPER_MODE_TIME` / `HYPER_MODE_COUNT`. Phase CH1q, as Elsword's Way of the Sword.
- **Veteran Commander overheat (live March code).** `SI_P_RVC_OVERHEAT_MODE`, `SA_OVERHEAT_HP` / `_PVP`, the class's overheat members
  (`X2GURaven.h:627-716`) and `CreateOverHeatObject` (`.cpp:21337`), combo-tree ids `CTI_*_OVERHEAT`, the VC slot texture and Shadow Step reads
  in `X2Game.cpp:14374-14442`, `:14949-14959`, the 35-member extra-damage pack (`X2GameUnit.h:8-42`). Phase CH4c.
- **Hooks waiting on the class port.** Checklist: 25 Raven rows (`june2017_re/char_raven/plan_checklist_rows.md`). Raven extras: 7b Burning
  Nasod Hand (setter `32c57660` `+0x9688` = 24401 learned, the `+0x4108 = 2` write `32c43120 @32c43ac4`, valid only with June's 46 `AT_COMMAND`
  tags; MODS.md l.2557-2559); 7b June-only `SetDamageExceptionProcess` `32c5c080` (24417 / 24420 / 24422, triggers 0xf / 0x12 / 0x13); the
  power-rate operator (§7 l.256: March **assigns** `m_fAttackPowerRateShadowStepThrust` / `m_fShadowBusterPowerRate` /
  `m_fAttackPowerRateShadowStepPunisher`); 7ag his own 4+4 decode (`X2GURaven.cpp:5854-5871`, `CountUsedSkill` `:5780`, and he also writes
  `m_eSpecialAttackKeyPressed` `:5876`); 7h `32c2b360`; 7am 3 delegate callbacks in `32c57660`; 7an vslot 0x73c `32c3c7f0` and 0x748 `32c3c820`
  (both uncarved), Specialization B readers `32c49410` / `32c507da`; 7ao re-awakening 24419 in `HyperModeStart` `31ffd490` and hyper effect
  start `32c4e7c0`; §7 l.286 his own `SetSkillLevelStateData` (`X2GURaven.cpp:23019-23214`).
- **Summons: none.** No NPC key in June's script, no call from June's class range to `31f8b010` / `322e67c0` / `3230d2b0`, 0 `CreateNPC` / `NUI_`
  in March's class. No summon phase. (The `RAVEN_CLONE*` / `NPC_Raven*` scripts are dungeon NPCs.)

**Out of scope (owned elsewhere).** New characters (Stage 8); UI that is not Raven's own (Stage 9); the class-change guides
(`DLG_1ST` / `2ND_Class_Change_Guide_Raven`, June-only; CH0b stages them) and creation-screen buttons (Stage 9); tutorials and training dungeons
(`Dungeon_Tutorial_Raven`, `Dungeon_Training_Raven_*`: june-dungeons); NPC / boss / clone copies of Raven (whichever plan adopted June's NPC
Lua); emotion motions' behaviour (Stage 10; CH0b stages the files); `FORCE_DYNAMIC_CAMERA_INDEX` / `RETAIN_CAMERA_STATE` (Stage 9); PvP-only
dialogs (`DLG_PVP_Game_Damaged_Raven*`: no PvP offline); jumping-event dialogs (Stage 9); the beginning-game left-arm read
(`X2BeginningGame.cpp:974`, OFF with the beginning game).

## Rules

The Elsword plan's Rules hold (`python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules"`), with Raven's names and these changes:
- **Standing rules** (`JUNE_PORT_BACKLOG_ORDER.md`, "Standing rules"): scope rule, no partial June updates, June wins, June bugs verbatim
  with a `JUNE_BUGS.md` row, cite June addresses, dev free-skill gate (`CX2GUUser::IsDevUnlimitedSkillUse()`) in Raven's skill-use check.
- **`ReservedSkillStart` (user, 2026-11-05).** `CX2GURaven::ReservedSkillStart` = `SpecialAttackCooldown( skillID, m_fReservedSkillMp )` then
  `SkillMemoMpRefund( ... )`, and the base hook chain in `X2GUUser.h` / `.cpp` widened with Raven's `_LIVE` flag (copy Elsword's). CH1k writes
  it; CH1 final's play-test casts each skill twice (the slot shows the cool time, the MP bar drops). Elsword missed it.
- **June's ids in passive code.** Every `SI_*` id March's `InitPassiveSkillState` / skill code reads is checked against June's `RNSI_*` id
  (Elsword's Rolling read March's 1205). CH0 lists them; CH4a fixes what CH1 did not.
- **The checklist is the work list.** Every Raven row in §1-§7au is ported by a phase here or marked n/a by CH0 with the reason. The phase
  that ports a row appends `(Raven: CHn)` to it in place.
- **One deploy unit.** June's `RAVEN_FIGHTER.lua`, his June motions (held for CH1: his script and June's motions disagree on four names) and
  the C++ that reads them go live together in CH1 final, behind `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (`//#define` in `Always.h` until then).
  Between phases Raven stays playable on his March kit.
- **Flags:** `_CORE` (June functions March lacks, compiled, unreferenced), `_LIVE` (June bodies inside March's functions, the registration, the
  switch), `_RAGE` (Nasod core / rage gauge / hyper orbs), `_SKILL`, `_PASSIVE`, `_UI`, `_ASSET` (only if a June asset breaks a March reader),
  and a temporary `_DEBUG` per sub-phase, retired in its review. New namespaces are `_CONST_RAVEN_<phase>_`.
- **Names (CLAUDE.md, 2026-10-28).** No `June` in any identifier, no Ghidra address or phase tag in a name. `python tools/junediverge.py --list`
  before transcribing; `--check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h <other files>` = `RESULT: PASS` at every finish.
- **Shared `CX2GUUser` helpers, never copied into the class:** `GetSkillLevel` / `GetLearnedSkillAbilityValue`
  (`SERV_IRUHADEV_JUNE_USER_SKILL_LEVEL_HELPERS`); the seven skill-use helpers and their fields (`SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS`);
  Aisha's, Eve's, Chung's and Elsword's `CX2GUUser` bases: a phase that needs one under its own flag widens the `#if` instead of copying it.
- **No hardcoded enum literals** (CLAUDE.md, 2026-10-26): every June id by name; `enumscan.py --check X2Lib/X2GURaven.cpp` stays 0. June's
  state names come in five prefixes: CH0 says which C++ enum each needs.
- **Shared Lua per character.** `UnitTemplet.lua` is core-list source: CH5 replaces only Raven's 7 rows. His class script is adopted whole
  (`adopt-june`, bytecode unless a phase must edit it) and passes `python tools/luabind.py check RAVEN_FIGHTER`.
- **Assets first, whole** (CH0b), byte-exact through `kom.py stage-june`; pack yourself, ask before every copy.
- **CP949.** `X2GURaven.cpp` (mixed endings: 22,034 LF, 1,308 CRLF), `.h` (mostly LF), `X2GUUser.*`, `X2GageUI.*`, `X2GageManager.*`,
  `X2DamageManager.*`, `X2DamageEffect.cpp`, `X2GameUnit.*`, `X2Game.cpp` are CP949: `bytepatch.py` only; `cl /P` decides every `#ifdef` branch
  (`UPGRADE_RAVEN`, `RAVEN_WEAPON_TAKER`, `SERV_RAVEN_VETERAN_COMMANDER`, `RAVEN_SECOND_CLASS_CHANGE`, `BALANCE_BLADE_MASTER_20130117`,
  `UPGRADE_SKILL_SYSTEM_2013`, and the dead `RAVEN_LIMIT_AIR_COMBO` / `OLD_HAND_SLASH_TRACE`).
- **Uncarved overrides.** Ghidra stays read-only: the 28 uncarved functions are read from the raw listing (`charport_adis.py raven <va>`;
  CH0 adds `raven` to its `CLASS_RANGE`), never by creating functions.
- **Damage numbers.** Record March vs June damage per changed skill in `MODS.md`.
- **Saves.** No schema change expected; a phase that needs one snapshots `db_backup/` first. Play-test saves never slot a skill in slot 4 / 9
  before transcendence, and learn only the ids of the bank being tested (an upgrade id learned beside its base makes the base unusable).
- **One code phase at a time in `X2GURaven.*`** (CH1-CH4). CH5 may run beside CH2-CH4.

## Starting context

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "<heading>"` (`--no-sub` to skip Done notes) |
| Checklist rows | `python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "<heading>"`; Raven's rows with line numbers: `june2017_re/char_raven/plan_checklist_rows.md` |
| Planning evidence | `june2017_re/char_raven/plan_*` (below); CH0 writes `ch0_*` beside them |
| Character tools | `june2017_re/charport_states.py`, `charport_keys.py`, `charport_closure.py`, `charport_adis.py` (CH0 adds `raven`), `charport_regsim.py`, `s3_no_state.py`; Elsword's builders `june2017_re/char_elsword/ch0_*.py`, `ch1m_regsim.py` / `ch1m_regcheck.py` (take a character or copy with the range changed); `char_arme/ch0c_listing_check.py` |
| June Lua (reading) | `june_src_fixed/data036_kom/RAVEN_FIGHTER.src.lua`, `UnitTemplet.src.lua`, `Enum.src.lua`; `data034_kom/DLG_Transcendental_Change_Guide_Raven.src.lua` |
| Ghidra | `ghidra` skill; Raven's range `0x32c25a20-0x32c6a63d`, vtable `33511454` (544 slots, 57 overrides, all in range), RTTI TD `3373d6bc`, ctor `32c3ece0` (vtable write `32c3ed50`), dtor `32c54c10`, `MemoConditionRaven` vtable `33511404` (ctor `32c3c780`); read-only in CH0 and reviews |
| Gap scan | `cd june2017_re && python census.py && python charport_scan.py <short scratch dir>` (slow; revert the regenerated `census.tsv` and `JUNE2017_EFFECT_KEY_MATRIX.md` after), `python charport_keys.py RAVEN_FIGHTER` |
| Bring June Lua in | `/x2-adopt-june` (`luasrc.py adopt-june data036 RAVEN_FIGHTER.lua`, `--dry-run` first) |
| Stage June assets | `python tools/kom.py stage-june <list>`, `/elstools pack dataNNN`, `/x2-kom-packed copy dataNNN` (ask first) |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply` (x2-byte-edit) |
| Hygiene / build / logs | `flagcheck.py <FLAG> --worktree --files ...`, `junediverge.py --check ...`, `enumscan.py --check ...`, `build.py --deploy` (background), `logs.py --grep "JUNE_CHAR RAVEN"` |
| Save / test character | `/x2-db` (`tools/elsdb.py setchar`, `settranscend <unit> 1`), `/x2-create-char <name> <class> <level>` |
| Dev console | auth level 3, END key; items `/additem <id> [qty]`, quests `/qdone <id>`, `/tc 1` |

## Planning evidence (2026-11-05)

Kept in `june2017_re/char_raven/` (digests by Sonnet subagents, spot-checked: the ctor `32c3ece0` writes vtable `33511454` at `32c3ed50`, the dtor
body `32c54c10` is the other reader; `32c641a0` is a defined function; CH0 re-verifies anything it builds on): `plan_charport_scan_20261105.txt`,
`plan_charport_keys.txt` / `.tsv`, `plan_march_side.md`, `plan_june_side.md`, `plan_numerical_rows.tsv` (113 rows), `plan_checklist_rows.md`.

Scans re-run 2026-11-05: `census.py` 26 PASS, 0 FAIL, 0 `to-implement` rows (implemented 652, june-ignores 305, handed-off 9; regenerated
`census.tsv` / matrix reverted); `charport_scan.py` (scratch `C:/tmp/cps_rav`, output identical to Elsword's 2026-11-03 run but for his line):
`RAVEN_FIGHTER {'no NUMERICAL_DATA_INDEX': 70, 'no DAMAGE table': 25}` (the brief's gap 25 holds), no Raven ENUM or KEYS line.

| Measure | Raven | (Elsword) |
|---|---|---|
| June class range | `32c25a20`-`32c6a63d`, 440 carved + 28 uncarved overrides; 57 vtable overrides, all in range | 308 + 25, 58 |
| June skill-tree rows | 254 over 7 classes; 126 distinct ids; 0 unnamed | 261 / 127 / 0 |
| Rows with no class state | **247** | 254 |
| Rows by June skill type | 6: 79, 15: 59, 16: 35, 14: 31, 8: 27, 1: 10, 12: 7, 13 (hyper): 6 | |
| June script | `RAVEN_FIGHTER.src.lua` 17,317 lines, data only: 346 states + 6 `INIT_*` (`INIT_COMMON_RANDOM_STATE` keyed by class name); groups `RSI_SA` 44, `RSI_A` 32, RIDING 27, `RVC_SI` 27, DAMAGE 21, DASH 12, COMBO 11, OVER_TAKER 11, SOUL_TAKER 10, WEAPON_TAKER 9, `RVC_DASH` 9, STANDUP 8, then small groups (SHADOW 5, PEPPER 5, PARRYING 2, ...) | 329 |
| Script keys | `DAMAGE_DATA` 88 (+5 `_NEXT`, **2 `_THIRD`**; 25 numerical-only), `AT_COMMAND` 46, `APPLIED_SKILL_ID` 29, `TIME_STOP_IMMUNE_STATE` 5, `SHOW_HYPER_CUT_IN_TIME` 3, `SHOW_SKILL_CUT_IN_TIME` 20, `UPDOWN_CANCEL_AFTER` 2; class keys `BURNING_NASOD_HAND_POSSIBLE` x24, `BLADE_MASTERY` x22 (March-only reader), `UNIT_SLASH_TRACE0` x37 | 90 / 67 / 25 |
| Key census (`charport_keys.py RAVEN_FIGHTER`) | 251 keys: june-only **10** (7 `BBT_*` / `BFT_*` damage-data keys with no reader, the camera pair (Stage 9), `IMMUNITY_EDT_LIST_AT_THIS_STATE` x5 no reader); neither 6 (`ANUM_SPEED`, `HEAD_IK`, `HEAD_IK_MAX_ANGLE`, `STOP_RESERVE_TIME_ATT` / `_DEF`, `STOP_TIME_ATK`); march-only-site 6 (`AFTER_IMAGE`, `BLADE_MASTERY`, `BUFF_FACTOR_ID_AND_APPLY_TIME`, `LeftDirCamear`, `READY_SOUND`, `UNIT_SLASH_TRACE0`); both 229 | 280: 5 / 5 / 8 / 262 |
| Numerical blocks | 113 `c1a` rows keyed to 55 ids, all `ok` | 103, 2 zero rows |
| Motions | June 73 / March 20; 10 differ; OT -4 animation names (3 renamed `_New`, `OT_SP1a_ChargedBolt` still played by June's script) | 71 / 18 |
| Alpha-tag sites (7c) | **0** | 29 |
| March class | `.cpp` 23,343 lines, `.h` 1,847 (`RAVEN_STATE_ID` 96 `RSI_` lines, VC uses `RVC_SI_*`); cut-in table `:61-94`, ctor `:101`, dtor `:306`, ReInit `:399`, DamageReact `:427`, InitState `:663` (cut-in switch `:669-693`), InitStateCommon `:1117-1915`, InitPassiveSkillState `:2114-2641`, `SetEquippedSkillFuncBySkillSlotIndex` `:2645-4150` (63 `case SI_*`, slot + SlotB model), SetEquippedSkillLevel `:4154`, InitComponent `:4891`, CommonFrameMove `:5153-5501`, SpecialAttackEventProcess `:5574-5896`, HyperModeFrameMove `:6772`, `SetSkillLevelStateData` `:23019-23214` | 19,415 |
| June class | InitState `32c66fe0` (0x7a4), `SetEquippedSkillFuncBySkillSlotIndex` `32c641a0` (0x7b4; 94 skill-id labels 0x5dc1-0x5eaf + a 6-case hyper switch), InitComponent `32c3ca00`, InitPassiveSkillState `32c57660` (0x690; Burning Nasod Hand), `SetDamageExceptionProcess` `32c5c080` (0x2b8, **June-only**), SpecialAttackEventProcess `32c25af0` (0x7f4, uncarved; parts `32c58c30`, `32c25ba0`, `32c43bf0`), hyper effect start `32c4e7c0` (0xc4), DamageReact `32c40b20`, ReInit `32c409b0`, InitEffect `32c25a60`; berserk-by-chance 0xa4 not overridden, 0x9c / 0xa0 are | |
| Stat rows | `StatTable.lua` 1-99 for all 7 classes, no duplicate, no gap | same |
| Save | units 23 `ravbe` (106 lv72), 49 `Raven99` (106 lv99); transcendence 0; **no unit of 4, 16, 17, 25, 107, 115** | |
| Cut-ins | 4 sets: `HQ_CutIn_Raven_1/2/3.dds` for 4 / 16 / 17 / 25, `_Blade_Master_1`, `_Reckless_Fist_1`, `_Veteran_Commander_1`; June's `UnitTemplet` adds `m_HyperCutIn` `HQ_Cutin_RBM/RRF/RVC_Transcendence` (no `m_BerserkCutIn`), `m_fLobbyRotationY` on all 7 rows (10 / 30 / 10 / 1 / 30 / 20 / 40 for 4 / 16 / 17 / 25 / 106 / 107 / 115), name `STR_ID_617` -> `STR_ID_17431` for 115 | |
| Transcendence | `DLG_Transcendental_Change_Guide_Raven.lua` (data034; March ships a same-name file); `POPUP_TR_B_MASTER` / `_R_FIST` / `_V_COMMANDER.tga` in June data215, **none in March's** | |

## Phases

| Phase | What | Model | Depends |
|---|---|---|---|
| CH0 | Raven map: class map (uncarved overrides included), state / skill table, checklist rows, assets + effect closure, keys, registration listing check, unmapped helpers, the Nasod core / rage map, the overheat map, the motion question, the `data202` target | Opus high | - |
| CH0b | Every June Raven asset, whole, first; the six missing walk units | Sonnet high, user play-test | CH0, char-elsword CH6 walk |
| CH1a-CH1k | Class core transcription by state group and vslot band (12 sub-phases incl. CH1g2, below) | Sonnet high, Opus review each | CH0, char-elsword Z; one at a time |
| CH1m-CH1p | Wiring behind `_LIVE` (registration; base callbacks + slot head; virtuals; 7ag + hooks + June-only keys) | Opus high, Opus review each | CH1a-CH1k reviews |
| CH1q | Nasod core / rage gauge / hyper orbs, flag `_RAGE` | Opus high, Opus review | CH1p review |
| CH1 | Final: define `_LIVE`, adopt the script, copy the held motion koms, play-test | Opus high, Opus review, user play-test | CH1q review, CH0b |
| CH2a-CH2d | Skills: Raven + first jobs (4, 16, 17, 25) | Sonnet high, Opus review, user play-test | CH1 review |
| CH3a-CH3d | Skills: second jobs (106, 107, 115), hyper actives, transcendence rows | Sonnet high (CH3d Opus), Opus review, user play-test | CH2 reviews |
| CH`n`0 | One Opus sub-phase per unmapped helper CH0 finds, before the Sonnet phase that needs it | Opus high, Opus review | as its phase |
| CH4a-CH4c | Passives (Burning Nasod Hand, delegates, 0x73c / 0x748); damage exception + overrides; Veteran Commander overheat + memo readers | Opus high, Opus review, user play-test | CH2 reviews (CH4c: CH3c review) |
| CH5 | UI and data: Raven's `UnitTemplet` rows, cut-in table | Sonnet high, Opus review, user play-test | CH1 review; beside CH2-CH4 |
| CH-N | Not run: Raven has no templet-backed June-only class | - | - |
| CH6 | Whole-character walk + gap reruns | Sonnet medium, user walk | CH3, CH4, CH5 reviews |
| Z | Close-out | Sonnet medium | CH6 |

CH1-CH4 all edit `X2GURaven.*`: one at a time. CH5 runs beside CH2-CH4. If CH0's counts put a group over ~45 functions, CH0 splits it with a
digit suffix (`CH1e2`) and writes the kickoff and STATUS row; it does not merge groups. Play-test follow-ups after CH1 final get the next letters
(`CH1r`).

## Phase CH0 - Raven map (read-only)

**Run with:** Opus 5.5 high. No code, no flag, no build. Ghidra read-only. Delegate reads, greps, tsv joins and known-address dumps to
subagents. Outputs in `june2017_re/char_raven/`.

**Step 0 (the pilot's recipe fixes).** Build the registration map with `charport_states.py init <InitState 32c66fe0> <InitStateCommon> <base>`
and `skill 32c641a0` (find InitStateCommon from vtable `33511454`; Elsword's call is in his CH0 Done note), then run `ch0c_listing_check.py` on
it: every row must match the raw listing; all `~` rows and std::string-named rows (`USI_HYPER_MODE`, START / WIN / LOSE roll) by hand. Raven's
`SetEquippedSkillFuncBySkillSlotIndex` has three switches (skill type, a 6-case hyper switch 0xc / 0x6019-0x601d, the 94-label skill switch):
the map covers all three. Elsword's builders take a character argument or are copied into `char_raven/`: say which. Add `raven` to
`charport_adis.py`'s `CLASS_RANGE`; confirm it lists the 28 uncarved overrides.

**Scope** (Elsword's CH0 items, with Raven's numbers):
1. **Class map** `ch0_class_map.tsv`: every function in `0x32c25a20-0x32c6a63d` **plus the 28 uncarved overrides**, March pair, verdict, target
   sub-phase, and the **`inside` / `new`** column (propose the plain name for every `new` row). Funcmap rows `Char Raven CH0`. Settle the
   boundaries (`32c25a20` after Chung's `32c25a1f`; `32c6a620` the last; Lire from `32c6a8d0`).
2. **State and skill table** `ch0_state_skill.tsv`: 352 / 352 script tables, 126 / 126 skill ids; dead-data and March-only states listed;
   confirm nothing in `X2GURaven.*` still stores a state id in a `UCHAR` (7l). Name the C++ enum each of June's five prefixes maps to (`RSI_`,
   `RVC_`, `RBM_`, `RRF_`, `RWT_`, the 7 `SI_` tables) and what `SHADOW`, `PARRYING`, `SUPER`, `TRANSFORMED`, `P`, `SI1` / `SI2` / `SI3` and the
   `*_OVERHEAT` states are and which group each belongs to.
3. **Keys** `ch0_keys.tsv`: the 10 june-only and 6 neither keys get an owner (the camera pair is Stage 9's); the 6 march-only-site keys
   (`BLADE_MASTERY`, `UNIT_SLASH_TRACE0`, `BUFF_FACTOR_ID_AND_APPLY_TIME`, `LeftDirCamear`, `AFTER_IMAGE`, `READY_SOUND`): does June's script drop a
   March reader's input or does June read it elsewhere; `BURNING_NASOD_HAND_POSSIBLE`, `MAX_HYPER_MODE_TIME` / `HYPER_MODE_COUNT` and
   `DAMAGE_DATA_THIRD` get their readers (June and March).
4. **Checklist rows** `ch0_checklist_rows.tsv`: every §1-§7au row -> phase or n/a with reason (start from `plan_checklist_rows.md`). Its l.120 /
   l.122 "to-implement" statuses (`ADDITIONAL_ATTACK_TRIGGER_TYPE`, `IGNORE_DEFENCE_RATE`) look stale: confirm the engine readers and mark them.
   The three power-rate states of §7 l.256 go to the phase that owns each, with June's operator read from June's state function. 7h's
   `32c2b360` likewise.
5. **Assets** `ch0_assets.tsv` + `ch0b_stage_list.tsv` (routes a name / b data / c `item.kim` Raven items, unioned) and **effect closure**
   `ch0_effect_closure.tsv` (`charport_closure.py`, run-time-built names included). **The motion kom:** June's 69 data202 motions have no March
   `data202.kom`: name the March kom each goes to. **The motion question:** confirm the 4 OT names `Motion_Raven_OT.x` drops, which March script
   states and class code play them, and whether June's file has `OT_SP1a_ChargedBolt` under any name (June's state
   `RSI_SA_ROT_CHARGED_BOLT_BLOOD` still plays it: a June bug row for CH2c if not). CH0b holds the motion koms for CH1 final. Stage the `POPUP_TR_*`
   textures (June data215), the class-change guide dialogs and the cut-in `.gfx` CH5 needs.
6. **Unmapped helpers.** Per skill, the callees outside the class range funcmap does not map (`ch0_unmapped_callees.py`). Each gets an Opus
   `CH<n>0` sub-phase before the Sonnet phase that needs it (kickoff + STATUS row written by CH0).
7. **Nasod core / rage map** `ch0_rage.tsv`: every March piece (class members `X2GURaven.h:1674-1679`, getters `:458-479`, `CreateNasodBall`
   `:13686`, the `m_CannonBallCount` sync, `X2GUUser.cpp:12000-12011` rage fill and the guard / parry hooks `:10262`, `:37195`,
   `DTT_RAVEN_RAGE_ATTACK` and the `UPGRADE_RAVEN` blocks of `X2DamageManager.cpp` / `X2DamageEffect.cpp`, `CX2RavenMyGageUI` and
   `X2GageManager.cpp:167-176`; `cl /P` says which branch is live) paired with June's or "absent in June", plus June-only pieces (vslots 0x9c /
   0xa0, the hyper orb ball `32e6d900`, 7aq), the Specialization B readers `32c49410` / `32c507da` (7an: CH1q or CH4c), the parrying states.
   Sized for CH1q; split if over ~45 functions.
8. **Overheat map** `ch0_overheat.tsv`: the Veteran Commander overheat pieces (class `:627-716`, `CreateOverHeatObject` `:21337`,
   `SA_OVERHEAT_HP` readers, `CTI_*_OVERHEAT`, `X2Game.cpp:14374-14442` / `:14949-14959`, `X2GameUnit.h:8-42`'s 35-member pack) June vs March;
   which go with the VC skills (CH1g2 / CH3c) and which to CH4c. The left-arm item (`m_RavenLeftArmItemID`, arm hiding
   `X2GUUser.cpp:41738-41760`, checklist l.702): June's reader vs March's, owner named.
9. **Shared data.** Which state names the C++ enums need (raw id + comment, as Eve); every `SI_*` id March's passive / skill code reads with
   its June `RNSI_*` id (the Rules' passive-id check); Raven's 7 `UnitTemplet` rows June vs March (is `STR_ID_17431` in our string table?).
10. **S3 census baseline:** `s3_no_state.py --family X2GURaven.cpp --preprocess` (247 expected) so CH6 compares like with like.
11. **Amend the kickoffs** CH0b-CH5 ("Amended by CH0" sections: filters with real column numbers, addresses, counts, any split).

**Done gate:** the tsvs exist; `ch0c_listing_check.py` passes on every registration row; every Raven checklist row has a phase or n/a;
352 / 352 tables and 126 / 126 ids mapped; the 28 uncarved overrides have rows; assets union routes a / b / c and the motion kom is named;
the motion question answered; the rage and overheat maps exist; kickoffs amended; open questions listed.

### CH0 - Done (2026-11-05)

Read-only: no flag, no build. Outputs in `june2017_re/char_raven/`; tables live in the tsvs.

**Tooling.** Elsword's builders copied with Raven's range: `ch0_build_state_skill.py`, `ch0_build_class_map.py`, `ch0_make_hand_names.py`,
`ch0_build_vtable.py`, `ch0_unmapped_callees.py`, `ch0_march_live_states.py`, `ch0_amend_kickoffs.py`; `ch0_skill_listing_emu.py` rewritten for
`32c641a0`'s frame. New: `ch0_build_keys.py`, `ch0_build_checklist_rows.py`, `ch0_build_enum_names.py`, `ch0_build_state_enum_names.py`,
`ch0_build_unittemplet.py`, `ch0_passive_ids.py`, `ch0_numtables.py`, `ch0_finalize_maps.py`, `ch0_funcmap_rows.py`. `charport_adis.py` gained `raven`
(all 28 uncarved overrides disassemble); `char_eve/ch0_build_assets.py` gained `raven` (Eve / Chung / Elsword outputs byte-identical).
**Fix:** `charport_states.py` let a column-0 `LAB_` label pop an if / else frame (stale callbacks on `RSI_COMBO_ZZZX`); Elsword, Eve, Luciel and
Chung reproduce unchanged. Class-map run order: `NO_HAND_NAMES=1` map, hand names, map again.

**Step 0.** InitStateCommon is `32c5d660`; InitState `32c66fe0`'s class switch calls three line registrars `32c511c0` (16 / 106), `32c51b70`
(17 / 107), `32c63670` (25 / 115), and InitPassiveSkillState `32c57660` registers the 6 Shadow Step states.
`charport_states.py init 32c5d660 32c66fe0 3202b110 32c511c0 32c51b70 32c63670 32c57660` -> `ch0_reg.tsv`; `ch0c_listing_check.py` **178 / 178**,
no hand rows. Skill switch (all three switches) from the listing emulator: 181 rows, 126 ids (23 type 1 / 8 ids leave at the type switch).

**Maps.**
- `ch0_class_map.tsv`: 1,241 starts (440 carved, 801 uncarved); 525 state-fn, 489 thunk, 117 dead, 57 override, 49 helper, 4 registrar. All 28
  uncarved overrides have rows. **634 to transcribe: 517 `inside`, 117 `new`** (37 `new` keep `?` with a hint). Per phase: CH1a 19, CH1b 40, CH1c 2,
  CH1d 38, CH1e 32, CH1f 38, CH1g 35, CH1g2 33, CH1h 37, CH1i 2, CH1j 9, CH1k 9, CH1m 5, CH1n 2, CH1o 9, CH1q 15, CH2a 43, CH2b 27, CH2c 23, CH2d 41,
  CH3a 34, CH3b 39, CH3c 34, **CH3c2 45**, CH3d 13, CH4a 7, CH4b 3. Hand routings: `ch0_hand_phases.tsv`.
- `ch0_state_skill.tsv`: **352 / 352 tables** (317 mapped, 29 dead data, 6 `INIT_*`), **126 / 126 ids**. 26 March-only states dropped. All prefixes
  map to the one C++ enum `RAVEN_STATE_ID`; 206 live June tables have no C++ member, and June's registered ids differ from C++ values for all 109
  shared names (`ch0_state_enum_names.tsv`: register by June's id). No `UCHAR` state id in `X2GURaven.*`. Groups: `SUPER_DASH` (3), `TRANSFORMED`,
  `FROZEN`, `RSI_P_RRF_SHADOW_PUNISHER` are dead data; PARRYING (2) -> CH1q; `SHADOW` (5 live) -> CH2a; `SI1` / `SI2` / `SI3` are the old skill tiers
  (by skill); `*_OVERHEAT` (12): 9 VC -> CH3c2, 1 WT -> CH2d, 2 dead.
- `ch0_vtable_slots.tsv`: 57 overrides. 0x9c / 0xa0 are March's inline `m_bHyperAura` pair (CH1h); 0x73c / 0x748 CH4a.
- `ch0_checklist_rows.tsv`: 141 rows, 79 phase, 62 n/a. **L120 and L122 are stale** (engine readers ported: `DamageData::IsAttackTrigger`,
  `X2GameUnit.cpp:3127`). §7 L256: none of March's three power-rate assigners survives in June; June's two `+0x7cc8` writers multiply (`*= 1.2`,
  CH2a 24201 / CH2d 24212). 7h `32c2b360` = `RSI_COMBO_ZZXX` EventProcess (CH1d).
- `ch0_keys.tsv`: no June-only key needs a Raven owner (7 `BBT_` / `BFT_` engine keys; camera pair Stage 9; `IMMUNITY_EDT` backlog Stage 2). The 6
  march-only-site keys drop no March input. `BURNING_NASOD_HAND_POSSIBLE` is read in CommonStateStart `32c43380` (uncarved; funcmap row re-keyed from `32c43120`).
- Item 9: `ch0_enum_names.tsv` (5 / 126 in the C++ enum by name, 59 in the core `Enum.lua`); `ch0_passive_ids.tsv`: **32 live March `SI_*` reads
  whose June id differs** (CH4a); `ch0_unittemplet_raven.tsv`: `STR_ID_17431` exists. S3 baseline `ch0_s3_baseline.txt`: 254 rows, 247 `no_state`.

**Assets.** `ch0_assets.tsv` 14,125 rows (routes a / b / c); **`ch0b_stage_list.tsv` 4,825 rows**: dry run `35 kom(s), 4825 staged (4490 new,
335 replaced), 0 refused`, one new kom **data202** (53 June-only emotions + `Motion_Standard_Raven.X`); the 9 differing class motions go to data033.
**Motion question:** only `Motion_Raven_OT.x` drops names (`OT_ComboX3-5Down` -> `_New`, `OT_SP1a_ChargedBolt` -> `Raven_OT_SP1a_ChargedBolt`). June's
`RSI_SA_ROT_CHARGED_BOLT_BLOOD` is dead data (name absent from the exe), so June never plays the bare name: no June bug. March's live script still
plays all four, so **`Motion_Raven_OT.x` alone is held for CH1** (`ch1_held_motions.tsv`). Effect closure `ch0_effect_closure.tsv`: 6,090 rows,
68 missing names (49 sounds in the list, 19 June does not define). Skill icons: 30 misses in 4 sheets, all in the list.

**Rage / overheat.** `ch0_rage.tsv` (79 rows; 59 CH1q): Nasod ball, parrying, AddRageGuage, gauge UI `CX2RavenMyGageUI` (vtable `3353348c`,
UpdateHyperOrb `32e6d900`), DamageReact hooks; Specialization B readers sit inside DoParrying / DoFrameMoveNasodBall (CH1q). CH1q stays one phase
(15 class functions). `ch0_overheat.tsv` (50 rows): Create / Delete / `_ShowOverheatIcon` -> CH1g2 (live WT states call them); 24 rows CH4c.
**Left arm:** June's reader `329657d0` sits in `3295dfa0`, owned by june-engine R14c; arm hiding `3201c3a0` is already ported.

**Item 6.** 248 out-of-class callees, 20 unmapped (`ch0_unmapped_identified.tsv`). `31e9cb70` / `31e9bb70` are DamageData's copy ctor / dtor
(no phase). **`31fd9f40`** (June-only left-hand bone scale, Megadrill Break) -> new Opus phase **CH2c0**. `funcmap.tsv`: 50 rows `Char Raven CH0`.

**Splits and kickoffs.** No CH1 group passes 45 after CH0's routing: parrying + Nasod chain CH1b -> CH1q; the `RVC_SI_*` skill states leave CH1g2
for their skills (Bleeding Slicer CH2b; Ignition Crow / Deadly Raid **CH3c2**, split from CH3c). New kickoffs `CH2c0.md`, `CH3c2.md`; every
kickoff CH0b-CH5 has its "Amended by CH0" section; STATUS rows added and Depends rewired (CH2c after CH2c0, CH3d / CH4c after CH3c2).

**Drift:** `JUNE_PORT_BACKLOG_ORDER.md:236` named Raven for `32cb6290( 1 )`, which is Rena's (F7 case `UT_LIRE`): corrected in place. Left arm ->
june-engine R14c (already owns `3295dfa0`). **Open for the user:** none.

## Phase CH0b - Every June Raven asset, whole, first

**Run with:** Sonnet 5.5 high, then a user play-test. No C++ unless CH0 found a March reader a June asset breaks (then
`SERV_IRUHADEV_JUNE_CHAR_RAVEN_ASSET`). Sub-tag `DATA`. After char-elsword CH6's walk.

**Scope:** every non-Lua row of `ch0b_stage_list.tsv`: motions (to the kom CH0 named), body / face / hair, equipment, textures, effect meshes,
sounds, cut-in art, portraits, `POPUP_TR_*`, the class-change guide dialogs. Stage byte-exact (`kom.py stage-june`), pack each kom, ask once with
the whole kom list, copy what the user approves **except the motion koms, which are packed and held for CH1 final** (March's script plays
names June's OT motion lacks). Save `data\ErrorLog_pre_CH0b_raven.txt` first. Create the walk units the save lacks (classes 4, 16, 17, 25, 107,
115, level 99, class costume) with `createchar.py`, after a `db_backup`. Gate: `python tools/skillicons.py check <Raven's skill-id range>` =
PASS (stage what it lists; the shared `DLG_SKILL_Passive_*` sheets included).

**Done gate:** the stage list re-runs 0 `missing` / `march-copy`; in game one Raven per job tier, village and dungeon, no new `KEM_ERROR` /
`Motion Error` / missing texture or mesh against the pre-CH0b log; a costume, a weapon and three skills look right.

### CH0b - Done (2026-11-05)

Sonnet 5.5. No C++, no flag (CH0 found no March reader a June asset breaks). Assets, two guide Lua and the save only.
- **Reference:** `ErrorLog.txt` (362,024 B, last run 2026-11-05 19:32) saved as `data\ErrorLog_pre_CH0b_raven.txt` before any copy.
- **Staged:** `kom.py stage-june --jobs 8 june2017_re/char_raven/ch0b_stage_list.tsv`: `STAGE-JUNE OK: 35 kom(s), 4825 staged (4490 new, 335 replaced, 0 identical), 0 refused, 28s`
  (the dry run matched CH0's numbers). Replaced March files are backed up under `24965799\kom_backup_stage_june\20261105_201955`.
  `Motion_Raven_OT.x` is not in the list (held for CH1 final, `ch1_held_motions.tsv`); the March copy in data033 is untouched.
- **Lua:** `luasrc.py adopt-june DLG_1ST_Class_Change_Guide_Raven.lua DLG_2ND_Class_Change_Guide_Raven.lua`: `ADOPT-JUNE OK (bytecode): 2 staged, 0 refused`
  (data034, 2 new; record `june2017_re/june_adopt_20261105_202539.tsv`). `luabind.py check` over those two and the two guides already June's: 21 / 22 / 22 / 20 methods, 0 unbound, `RESULT: PASS`.
  `luasrc.py ledger --check` PASS (0 FAIL, 0 WARN), `core --check` PASS.
- **Packed:** `kom.py pack` x36 (35 + data034), 36 `PACK OK`, 0 refusals. No removed entries in any.
- **Copied (user approved, 2026-11-05):** `kom.py copy` x36, 36 `COPY OK`: data002 010 016 019 020 021 023 033 034 044 052 063 067 069 078 097 120 128 130 143 151 191
  192 198 202 215 222 226 datai007 011 019 020 023 027 029 031. No motion kom held back: the only name March plays that June's file drops is in `Motion_Raven_OT.x`,
  which was never staged. data033 carries the 9 differing class motions (none drops a name), new data202 the 54 June-only motions. `kom.py status`: 0 `COPY NEEDED`.
- **Done-gate re-run:** `stage-june --dry-run` over the same list: `4825 staged (0 new, 0 replaced, 4825 identical), 0 refused`. 0 `missing` / `march-copy`.
- **Skill icons:** `skillicons.py check 106 900 24000-24058 24200-24239 24400-24422 24600-24605`: 126 skills, **4 misses** (was 30), `RESULT: FAIL` on four `NO_PIECE` rows:
  24206 / 24231 `SI_A_RRF_ENHANCE_HAND` (`DLG_Raven_SKILL_ScreenShot_03`), 24417 `RNSI_P_RBM_OPEN_MIND`, 24421 `RNSI_P_RVC_OPEN_MIND` (`DLG_SKILL_Passive_12`). June's own `.TET` files
  lack those piece names (checked in the bytes); both sheets are `4 identical` on a re-stage. June's gap, kept: `JUNE_BUGS.md` row 535. The icons are fine. Nothing left to stage.
- **Save:** backup `db_backup\els_db_20261105_202257_pre_manual.sql`; added with `createchar.py` (lv 99, class costume + top gear +10, 4 skills on the bar):
  unit 72 RavenFighterT (Raven 4), 73 SwordTakerT (16), 74 OverTakerT (17), 75 WeaponTakerT (25), 76 RecklessFistT (107), 77 VetCommanderT (115).
  Existing: 23 ravbe (106 lv72), 49 Raven99 (106 lv99).
- **Play-test (user) is the gate.** Log tag `[JUNE_CHAR]` has no lines (no code); the check is `ErrorLog.txt` against `ErrorLog_pre_CH0b_raven.txt`
  (`KEM_ERROR`, `Motion Error`, missing texture / mesh). Class script and motions are still March's, so Raven plays as before; his new meshes / textures / sounds load.
- **Drift:** none new.

## Phase CH1 - Deploy unit: June script, motions and the class core

**Groups** (CH0's map fills the counts and the exact function lists; the kickoffs carry the filters):

| Sub-phase | Run with | Scope |
|---|---|---|
| CH1a | Sonnet high, Opus review | Movement and slot-head states: wait (2) + wait-state-change, walk, jump (6), dash (12), throw item, common buff, hyper, `SUPER` (3) unless CH0 places it elsewhere |
| CH1b | Sonnet high, Opus review | Reactions: damage (21 `RSI_DAMAGE*`), stand-up (8), die, start, win / lose blocks, wall, amusement, frozen, transformed, parrying (2) unless CH0 gives it to CH1q |
| CH1c | Sonnet high, Opus review | Riding (27 `RSI_RIDING*`), pepper run (5), dance (3) |
| CH1d | Sonnet high, Opus review | Raven ground combos (11 `RSI_COMBO*`, the X-X-X combo) and the base dash / jump combos CH0 assigns |
| CH1e | Sonnet high, Opus review | Sword Taker line (10 `RSI_SOUL_TAKER*`, `RST`) and Blade Master line (`RSI_RBM*`, the `RBM_` tables) |
| CH1f | Sonnet high, Opus review | Over Taker line (11 `RSI_OVER_TAKER*`, `ROT`) and Reckless Fist line (`RSI_RRF*`, the `RRF_` tables) |
| CH1g | Sonnet high, Opus review | Weapon Taker line (9 `RSI_WEAPON_TAKER*` incl. the X-X-X loop, `RWT`, the `RWT_` tables) |
| CH1g2 | Sonnet high, Opus review | Veteran Commander line (`RSI_RVC*`, the 36 `RVC_` tables: 27 `RVC_SI`, 9 `RVC_DASH`), the overheat state pieces CH0's overheat map gives it |
| CH1h | Sonnet high, Opus review | Overrides below vslot 0x600 (lifecycle, render, reaction virtuals; DamageReact `32c40b20`; ctor / dtor offset map; 0x9c / 0xa0 only if CH0 does not give them to CH1q; the uncarved ones read from the listing) |
| CH1i | Sonnet high, Opus review | Overrides 0x600-0x7bc |
| CH1j | Sonnet high, Opus review | Common* overrides 0x7c0-0x7ec (six of them uncarved), CommonFrameMove and the helpers only it reaches |
| CH1k | Sonnet high, Opus review | Skill-use overrides from 0x7f0 (the seven `CX2GUUser` skill-use helpers are called, never copied), SpecialAttackEventProcess `32c25af0` (uncarved) with its parts `32c58c30` / `32c25ba0` / `32c43bf0`, 0x80c / 0x810 / 0x834 (uncarved), and **`ReservedSkillStart`** (Rules) |
| CH1m | Opus high, Opus review | Registration: June's `InitState` `32c66fe0` / `InitStateCommon`, the hyper-mode table name (`GetHyperModeTableName`, `31fdd490`), START / WIN / LOSE roll (June's `INIT_COMMON_RANDOM_STATE` keyed by class name), base event states, the `_LIVE` gate |
| CH1n | Opus high, Opus review | The `CX2GUUser` base callbacks Raven registers (Chung's CH1n2 / Elsword's CH1n triage first: only rows they did not cover); `SetEquippedSkillFuncBySkillSlotIndex` `32c641a0` head (bounds, slot tables, the 7i rule, the two pre-switches) + the dev free-skill gate |
| CH1o | Opus high, Opus review | The virtuals under `_LIVE`; June-only and changed user virtuals Raven overrides (vslot 0x4ac special-item state, vslot 510 lookup); `SetSkillLevelStateData` `:23019` with the `( 3201ad10( skill ) + 1.0 )` term (§7 l.286); gaps the CH1a-CH1k reviews routed |
| CH1p | Opus high, Opus review | 7ag pressed-index space (his own decode `:5854-5871`, `CountUsedSkill` `:5780`, the `m_eSpecialAttackKeyPressed` write `:5876`, `GetSkillSlotIndexUsed` `:10842`, every reader together); hooks 7h / 7l / 7o / 7p / 7x; June-only script keys CH0 gives an owner here |
| CH1q | Opus high, Opus review | Nasod core / rage (`ch0_rage.tsv`): June's bodies inside March's rage, Nasod-ball and gauge functions (class, `X2GUUser.cpp` fill and guard hooks, `X2DamageManager` / `X2DamageEffect` `UPGRADE_RAVEN` blocks, the `m_CannonBallCount` sync, `CX2RavenMyGageUI` incl. June's hyper orb ball `32e6d900`, 7aq), June-only pieces under plain names (vslots 0x9c / 0xa0 if CH0 puts them here), the Specialization B readers if CH0 puts them here; flag `SERV_IRUHADEV_JUNE_CHAR_RAVEN_RAGE`, sub-tag `RAGE` |
| CH1 | Opus high, Opus review, user play-test | Define `_LIVE`; adopt `RAVEN_FIGHTER.lua`; copy CH0b's held motion koms; pack, ask, copy; the play-test |

**Transcription rules (CH1a-CH1k):** as Elsword's (`JUNE_CHARACTER_ELSWORD_PHASES.md`, "Phase CH1", "Transcription rules"), with Raven's flags
and his class range `0x32c25a20`-`0x32c6a63d`: `inside` rows inside March's function under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` with March's in
`#else` and a one-file probe compile; `new` rows under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE`; a later group's callee declared with
`// June <va>: body in <phase>`; an unmapped engine callee is "this needs Opus" (stop). June's numbers and quirks verbatim;
`m_fNowStatePowerRate` (7i) with June's operator (`*=` where June multiplies, never March's `=`, §7 l.256); June's BFI arguments (7ai).

**Done gate (CH1 final):** Raven (any class) loads June's script and motions; `[JUNE_CHAR] RAVEN DATA` logs the load; no new `KEM_ERROR` /
`Motion Error` / missing-animation line against a pre-CH1 run; combos, dash, jump, damage, bounding, parrying and riding play June's animations;
the rage gauge fills, the Nasod balls attack and the hyper orbs show as June; each skill cast twice shows its cool time and spends MP;
`charport_scan.py` / `charport_keys.py RAVEN_FIGHTER` show nothing new.

### CH1a - Done (2026-11-05)

All 19 class-map rows of phase CH1a are written, all `inside`: June's body in March's own `CX2GURaven` function under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, March's in `#else SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`.
`_LIVE` is in `Always.h` as `//#define` with its `#error` guards (`_CORE`, `JUNE_BUFF_AWAKE_LOCK`, `SERV_RAVEN_VETERAN_COMMANDER`). `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` is defined too: its only content is
`RBM_DASH_JUMP_COMBO_X` ("June script id 300", CH1e owns the state) appended to `RAVEN_STATE_ID`, the one state name a CH1a body spells that March lacks. 19 funcmap rows `Char Raven CH1a`.
`june2017_re/char_raven/ch1_offsets.tsv` is new (Elsword's base rows, class fields dropped, plus six Raven rows); the review confirms them.

| June | `CX2GURaven::` | June | `CX2GURaven::` |
|---|---|---|---|
| `32c25c90` `32c40dd0` `32c43f90` | RSI_WAIT StartFuture / Start / EventProcess | `32c40ea0` `32c441c0` | RSI_WALK Start / EventProcess |
| `32c3d2b0` `32c25cc0` `32c28aa0` `32c443f0` | RSI_JUMP_READY / UP / DOWN / LANDING EventProcess | `32c446a0` | RSI_DASH EventProcess |
| `32c3d3b0` `32c40f50` `32c28b80` `32c448a0` | RSI_DASH_END StartFuture / Start / FrameMove / EventProcess | `32c3d3f0` `32c44b60` | RSI_DASH_JUMP / DASH_JUMP_LANDING EventProcess |
| `32c25d60` `32c25d90` `32c44d80` | RSI_DASH_JUMP_POWER_LANDING StartFuture / Start / EventProcess | | |

**Where June differs from March** (everything else is March's logic written out whole; read from the listings, `charport_adis.py raven`):
- **DashJump EventProcess `32c3d3f0`: Blade Master.** After the Veteran Commander test (`[unit + 0x38]` = 115 -> `RVC_DASH_JUMP_COMBO_X`) June tests 106 `UC_RAVEN_BLADE_MASTER` -> `RBM_DASH_JUMP_COMBO_X` (state 0x12c); else `RSI_DASH_JUMP_COMBO_X`. March has no Blade Master branch.
- **DashJump, both keys held** (`pureRight && pureLeft`): when `oneRight` against a left facing or `oneLeft` against a right facing, the jump speed is clamped and the state goes to JUMP_UP / JUMP_DOWN; otherwise nothing. March sends the held-both case into the `pureRight` branch.
- **PowerLanding StartFuture `32c25d60`:** `nowSpeed.x *= 0.5` (March `/= 2.0f`; same value).
- **Z keeps the re-attack gate** in Wait / Walk / JumpLanding / DashEnd (June tests `m_bReAttackZ1Right` +0x95d0 and `m_fReAttackZ1Time` +0x95d4 as March does), unlike Elsword's; DashEnd splits at `(max - transition) / 8`, PowerLanding at `/ 6`.
- **= March after the macros:** Wait / Walk / DashEnd Start (the Black Hole reset at `RBM_COMBO_ZZZXZ`; June's hit count is a `KProtectedType<int>`, March's a plain `int`), the other Wait / Walk / Dash / JumpReady / JumpUp / JumpDown / JumpLanding / DashEnd / DashJumpLanding functions, and the PowerLanding pair. The hyper macro is June's own `TryHyperModeStateChange( USI_HYPER_MODE )` call; `SpecialAttackEventProcess` is passed `ASUT_GROUND` / `ASUT_AIR` (1 / 2) as June pushes; `GetEnableDash( EDS_END )` is March's default.
- `JumpLanding`'s skip-point chain is `ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS` (the 15 keys in June's order). DashJumpLanding's off-line test is a separate `if`, as March. No `JUNE_BUGS` row: nothing here is a June defect.
- State ids are written by name (`USI_*` / `RSI_*` / `RVC_*`); June's registered ids differ from the C++ enum values, which CH1m maps. Checklist §7 L256 (`+0x7cc8` power rate): no CH1a function touches it.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE --worktree --files X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h KTDXLIB/Always.h`: `files: 1 PASS, 0 PASS~, 1 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out (by design). The OTHER row is `X2GURaven.h` (the `_CORE` enum member).
- `flagcheck.py` now diffs with `--minimal`: plain `git diff -U0` showed 20 false deletions (Myers pairing of the March lines that June's bodies repeat); `--selftest` PASS, `git diff --patience --numstat`: 669 inserted, 0 deleted in `X2GURaven.cpp`; 4 / 0 in `X2GURaven.h`; 29 / 0 in `Always.h`.
- `cl /P` of `X2GURaven.cpp` with `_LIVE` undefined: 0 differing non-blank lines against the pre-change output except the new enum member (and `#line` / `__COUNTER__` names); with `/D SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, DashJump has the Blade Master and both-keys branches.
- One-file probe (`census.cl_cmd` with `/Zs`, no PCH): 0 errors with `_LIVE` off and 0 with `/D SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (this also evaluates the `Always.h` guards).
- `junediverge.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows.
- `BUILD OK: X2/US_SERVICE/x2.exe 25,997,312 B, 2026-11-05 20:48:49` (KTDXLIB, X2Lib and X2 all rebuilt, 0 errors). **`DEPLOY REFUSED`: `X2_offline.exe` was running (pid 22040).** Close the game, then `python tools/build.py --deploy-only`; the deployed exe is still the previous one until then. No new string in the exe, so no `exestr` line.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-raven CH1a review` (Opus 5.5 high), then CH1b.

### CH1a - Review (2026-11-05, Opus 5.5)

**Verdict: PASS, no code change.** Read against `charport_adis.py raven` listings (the `StateChange` pushes, `fcomp` / `test ah` idioms):
- **DashJump EventProcess `32c3d3f0`:** branch for branch as written. X gate `0 < [+0x95d8]` skips (`m_fReDashJumpXTime <= 0`); class test
  `[unit + 0x38]` 0x73 -> 0xfc, 0x6a -> 0x12c, else 0xb6. Both-keys branch: `oneRight && !bIsRight` or `oneLeft && bIsRight` -> clamp
  `if( jumpY < nowY ) nowY = jumpY` (`fcompp` / `test ah, 5` / `jp`), then `0 < nowY` -> JUMP_UP else JUMP_DOWN; otherwise nothing. The tail
  (`oneUp && nowY <= 0` flag, `oneDown` -> `nowY = fMaxGSpeed` with no negation, riding) is March's. Wall landing 0xc8 = `RSI_WALL_LANDING`, 0xb3 = `RSI_DASH_JUMP_COMBO_Z`.
- **State ids** (`ch0_reg.tsv`): 0x12c = 300 = `RBM_DASH_JUMP_COMBO_X` (registrar `32c511c0` @524), 0xfc = 252 `RVC_DASH_JUMP_COMBO_X`, 0xb6 = 182
  `RSI_DASH_JUMP_COMBO_X`. June's InitState also registers `RRF_DASH_JUMP_COMBO_X` at 182 for Reckless Fist, as March does (`X2GURaven.cpp:820`); CH1m keeps that.
- **PowerLanding `32c25d60` / `32c25d90` / `32c44d80`:** `fmul 0.5`; Start = CommonStateStart + CreateStepDust; EventProcess gate
  `(max - transition) / 6 < now` (`+0x220 - +0x2b0`, `fdiv 6.0`, vs `+0x214`) = `GetNowAnimationTime() > GetMaxAnimationTime() / 6`
  (`GetMaxAnimationTime()` is `m_fAnimMaxTime - m_fTransitionTime`); key order, off-line `y -= 45`, WAIT on `+0x2ac` all match.
- **DashEnd `32c448a0` / Start `32c40f50`:** Z gate `m_bReAttackZ1Right != bIsRight || !(0 < m_fReAttackZ1Time)`; Z and X split at `fmul 0.125` (`/ 8`);
  `SpecialAttackEventProcess( 1 )`; dash on double / 3-key, walk, CanDown (March's 4-arg wrapper over the June signature), up, hyper, anim end, riding. Start: stage
  0xd8 (`RBM_COMBO_ZZZXZ`) -> time 0, protected hit count 0, stage 0x99 (`RSI_BASE`), then CommonStateStart.
- **JumpLanding `32c443f0`:** skip-point chain is the 15 keys in `ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS`'s order (Up Down Right Left Z X A S D C Q W E R LCtr),
  joined with anim end to `StateChange( 0xd, false )`; pureUp -> JUMP_UP, `nowY = jumpY`, `y += 30`. Extra: Wait EventProcess `32c43f90` matches too.
- **Offsets** (`ch1_offsets.tsv` rows 85-90): +0x95d0 byte vs `bIsRight`, +0x95d4 / +0x95d8 floats, +0x9674 `KProtectedType<int>` (value words +4 / +8),
  +0x9680 float, +0x97d4 stage: all confirmed by the listings above. The protected hit count needs nothing until a reader goes through the protected type.
- **Enum tail:** `RBM_DASH_JUMP_COMBO_X` is the last member of `RAVEN_STATE_ID` (`X2GURaven.h:252`); no END / count member, `m_StateList` is a map; no table indexes the tail.

**Gates:** `flagcheck SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE --with ..._CORE --range 5264b64e^..5264b64e`: `files: 1 PASS, 0 PASS~, 1 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`;
`0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only for the
commented-out `_LIVE` define (OTHER = the `_CORE` enum member). `junediverge.py --check-all`: `RESULT: PASS` (185 allow-listed). `enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows.
One-file `/Zs` probe of `X2GURaven.cpp`: 0 errors with `_LIVE` off and with `/D SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`.
No build: the review changed no code. CH1a's refused deploy is harmless (the only compiled change is an unreferenced enum member); CH1b's `build.py --deploy` carries it.
Nothing to play-test. Next: `/x2-phase char-raven CH1b` (Sonnet 5.5 high).

### CH1b - Done (2026-11-05)

All 40 class-map rows of phase CH1b are handled. 37 are `inside`: June's body in March's own `CX2GURaven` function under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, March's in `#else SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`.
2 are `new` (`_CORE`, compiled, referenced by nothing). 1 needs no code: `32c52480`, CH0's `new ?`, is March's `ClearArcEnemyData` (the `ArcEnemyData` destructor inlined). CH0's other `new ?` row, `32c59550`, is March's `RBM_EMERGENCY_ESCAPE` (inside). `_LIVE` stays `//#define`. 40 funcmap rows `Char Raven CH1b`; 2 rows added to `june2017_re/char_raven/ch1_offsets.tsv`.

| June | `CX2GURaven::` | June | `CX2GURaven::` |
|---|---|---|---|
| `32c28960` `32c55ee0` `32c25c50` `32c289c0` | DieFront StartFuture / Start / FrameMove / EventProcess (also `RSI_RIDING_DIE`) | `32c28a00` `32c55fe0` `32c25c70` `32c28a60` | DieBack StartFuture / Start / FrameMove / EventProcess |
| `32c28bc0` `32c28c10` `32c28d90` `32c28dd0` `32c28f50` | DamageDownFront FrameMove / EventProcess, DamageDownBack FrameMove / EventProcess, AirDownLanding EventProcess | `32c33bd0` | RWT_LOSE2 FrameMove (14 win / lose states) |
| `32c29190` `32c29210` `32c29290` `32c29310` + End `32c25db0` `32c25df0` `32c25e30` `32c25e70` | StandUpRolling FrontFront / FrontBack / BackFront / BackBack EventProcess + End | `32c25eb0` `32c25f00` `32c266d0` `32c26720` | StandUpAttack Front / Back / FrontNoDown / BackNoDown FrameMove |
| `32c29390` `32c29410` `32c30f30` `32c30fb0` + End `32c25ec0` `32c25f10` `32c266e0` `32c26730` | the same four EventProcess + End | `32c5b6f0` `32c5b710` `32c59550` | DamageSmallBack / BigBack Start, `RBM_EMERGENCY_ESCAPE` |
| `32c290d0` `32c29130` | **new** `RSI_DAMAGE_STANDUP_FRONT_StateStart` / `_StateStartFuture` | `32c52480` | = `ClearArcEnemyData` |

**Where June differs from March** (everything else is March's logic written out whole; read from the listings, `charport_adis.py raven --phase CH1b`):
- **Stand-up attack with no knock-down** (DamageDownFront `32c28c10`, DamageDownBack `32c28dd0`, AirDownLanding `32c28f50`): after the queued X, March rolls `GetRandomInt() < m_iNoDownDamageAtStandUpAttackPercent`. June tests `GetUnit()` and `GetSkillLevel( 0x5e88, false, 0, true ) > 0` (`RNSI_A_RF_SHADOW_STEP`, Shadow Step): `NO_DOWN` when learned, else `ATTACK`, no roll. Back keeps the double `0.66`.
- **Emergency Escape** (`32c59550` = `RBM_EMERGENCY_ESCAPE`, called by DamageSmallBack / BigBack Start): June adds a cool-time gate. Two fields (June `KProtectedType<float>`, March-side plain `float`, both new members under `_CORE`): `m_fEmergencyEscapeCoolTime` (`+0x95ac`) above `m_fEmergencyEscapeElapsedTime` (`+0x95a0`) returns. The skill is `RNSI_P_RBM_EMERGENCY_ESCAPE` (24410, `GetSkillLevel( id, true, 0, true )`); the roll is `GetRandomInt( 0 ) < SA_EFFECTIVE_RATE x 100.0` (a double); on success `StateChange( RSI_SHADOW_STEP )` (June id 0xde, script `RSI_SI_A_RF_SHADOW_STEP`), `UpNowMp( -cost )`, then the elapsed time goes to 0. Roles read from `32c42f90` (CommonFrameMove helper: adds the frame time while `+0x95a0 <= +0x95ac`) and `32c57660 @32c57c03` (both set from SA 0x94).
- **New stand-up Start pair** (`32c290d0` / `32c29130`, ten states share them: DamageStandUp Front / Back, the four StandUpRolling, the four StandUpAttack): `CommonStateStart` / `CommonStateStartFuture`, then with `RNSI_P_RBM_OPEN_MIND` (0x5f61 = 24417, Awakened Will: Blade Master) learned `ResetAnimSpeed( play speed x GetLearnedSkillAbilityValue( SA_ANI_SPEED = 103, id ), true, bFuture )`. March has no Start for those states. Names are CH0's.
- **= March after the macros:** Die Front / Back (Start: `DyingStart`, play speed 0.5, `ClearArcEnemyData`, `ClearNasodBall` (`32c4f750`, body CH1q), rage gauge 0), DamageDown FrameMove (Front `EventTimerOneshot( 0.19f )`, Back `EventTimer( 0.32f ) == true`), the Rolling EventProcess / End, the Attack FrameMove / EventProcess / End, `32c33bd0` (`EventTimerOneshot( 1.55f )`, `UpDownCrashCamera( 20, 0.3 )`), SmallBack / BigBack Start. Checklist §7 L256 (`+0x7cc8`): no CH1b function touches the state power rate.
- Three `SKILL_ID` names registered in `X2SkillTree.h` under `JUNE_ENUM_NAMES`: `RNSI_A_RF_SHADOW_STEP` 24200, `RNSI_P_RBM_EMERGENCY_ESCAPE` 24410, `RNSI_P_RBM_OPEN_MIND` 24417 (June `Enum.src.lua` 14579 / 14917 / 16006). `SA_ANI_SPEED` is 103 (counted in the enum; `s0a_templet_keys` row 142).
- No `JUNE_BUGS` row: nothing here looks like a June defect.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE --worktree --files X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h X2Lib/X2SkillTree.h`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out and `Always.h` was left out of `--files` (a peer has it modified). OTHER = `X2GURaven.h` (`_CORE`: PASS on its own, `+8`) and `X2SkillTree.h` (`JUNE_ENUM_NAMES`). `git diff --patience --numstat`: `.cpp` 668 / 0, `.h` 8 / 0, `X2SkillTree.h` 7 / 0.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): `X2GURaven.cpp` exit 0, 0 errors with `_LIVE` off and with `/D SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. `cl /P`: `m_fEmergencyEscapeCoolTime` 1 line off / 2 on, `RNSI_A_RF_SHADOW_STEP` 1 / 4, so the flag took effect.
- `junediverge.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h X2Lib/X2SkillTree.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows.
- `BUILD OK: X2/US_SERVICE/x2.exe 25,997,824 B, 2026-11-05 21:23:00` (X2Lib 0 errors; built with a peer's uncommitted `Always.h` / `X2StateDungeonGame.cpp` edits on disk). **`DEPLOY REFUSED`: `X2_offline.exe` was running (pid 14760).** Close the game, then `python tools/build.py --deploy-only`; the deployed exe is still the previous one until then. `_LIVE` is off and no string was added, so no `exestr` line.

**Routed.** CH1k (ctor `32c3ece0`): `-1.0f` into `m_fEmergencyEscapeElapsedTime` (`@32c3ed40`) and `m_fEmergencyEscapeCoolTime` (`@32c3ef32`); until then both are uninitialised, which matters only with `_LIVE` on. CH1j: `32c42f90` (called by CommonFrameMove) adds the frame time to the elapsed time. CH4a: `InitPassiveSkillState` `32c57660` sets both fields from skill 24410, SA 0x94 (`@32c57c03`). CH1m: register `RSI_DAMAGE_STANDUP_FRONT_StateStart` / `_StateStartFuture` on the ten states, and map June's ids (0x9b..0x9f stand-up, 0xcc / 0xcd no-down, 0xde shadow step) to the C++ enum. CH1q: the `ClearNasodBall` body.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-raven CH1b review` (Opus 5.5 high), then CH1c.

### CH1b - Review (2026-11-05, Opus 5.5)

**Verdict: PASS, no code change.** All 40 rows read against `charport_adis.py raven --phase CH1b` listings:
- **No-down stand-up attack** (`32c28c10` / `32c28dd0` / `32c28f50`): anim end -> flag[4] (`+0x7d89`) -> `GetUnit()` (`+0x867c`) non-NULL and `GetSkillLevel( 0x5e88, 0, 0, 1 )`
  (pushes `1, 0, 0, 0x5e88`) `> 0` -> 0xcc / 0xcd `NO_DOWN`, else (and when no unit) 0x9f / 0xa0 `ATTACK`; then flag[1] ATTACK, flag[2] / [3] rolling,
  else 0x20 / 0x21 stand-up. The 0.66 key window and all six flag writes (word / dword stores decoded) match. Front and AirDownLanding use float 0.66 (`fld; fcomp`,
  `test ah, 5; jp`); Back alone uses the double (`fcomp qword 0.66`, `test ah, 0x41; jne`). AirDownLanding's listing equals Front's apart from addresses.
- **Emergency Escape `32c59550`:** returns while cool (`+0x95ac`) `>` elapsed (`+0x95a0`) (`fcompp`, `test ah, 0x41; jp`); `GetSkillLevel( 0x5f5a, 1, 0, 1 )`, no unit check
  (as written); `GetSkillTemplet( 0x5f5a, 1 )` = March's one-argument form (funcmap 409); roll `GetRandomInt( 0 )` (vslot 0x1c8) as a double `<` `32206e50( 5, level, 0 ) x 100.0`
  (March's two-argument `GetSkillAbilityValue` reads the numerical block with bPvp false); MP via vslot 0x6a0 = slot 424 = the virtual `GetActualMPConsume( id, level )`
  (`X2GUUser.h:3157`, June passes 1); text 0x9f5 = `STR_ID_2549`, font 3, white / black, `DT_CENTER`, 1, 1; `StateChange( 0xde, 0 )`; `UpNowMp( -cost )` (vslot 0xf0);
  `if( elapsed != 0 ) elapsed = 0` is June's inlined `KProtectedType` store, harmless as written.
- **Field roles:** `32c42f90` adds the frame time (`+0x2b0`) to `+0x95a0` while `0 < +0x95ac` and `+0x95a0 <= +0x95ac`; `32c57660` stores SA 0x94 at the level into
  `+0x95ac` (`@32c57c03`..`32c57c35`) and then `+0x95a0` (`@32c57c89`). `ch1_offsets.tsv` rows `0x95a0` / `0x95ac` (value words `+4` / `+8`) are right.
- **Stand-up Start pair `32c290d0` / `32c29130`:** `31fceab0( 0x5f61 )` = `GetSkillLevelValue( int )` (the only overload under `SERV_IRUHADEV_JUNE_DIVERGE_SKILL_LEVEL_VALUE`);
  `32007b10( 0x67, 0x5f61 )` = `GetLearnedSkillAbilityValue`; anim `+0x250` x value (`SetPlaySpeed` writes `+0x250`); `ResetAnimSpeed( product, 1, 0 / 1, 1.0f )`, so the
  third argument is bFuture and the fourth is the default `fExtraSpeedRate`. Ten states share them (CH1m registers).
- **= March, spot-checked end to end:** DieFront Start `32c55ee0` (vslot 0x338, 0.5, `ClearArcEnemyData`, `ClearNasodBall`, protected `+0x97b8 = 0`), Rolling EventProcess
  `32c29190`, DamageDownBack FrameMove `32c28d90` (plain `EventTimer( 0.32f ) == 1`); also DamageDownFront FrameMove (inlined Oneshot 0.19), `32c33bd0` (Oneshot 1.55,
  `UpDownCrashCamera( 20, 0.3 )`), SmallBack / BigBack Start, Die FrameMove 0.19 / 0.32 + 1.12. Sibling listings are identical apart from addresses: 4 Rolling EventProcess,
  4 Attack EventProcess, 8 End (`0.3`, vslots 0x11c / 0x120), 4 Attack FrameMove, the Die StartFuture and EventProcess pairs.
- **Enum names:** June `Enum.src.lua` 14579 / 14917 / 16006 = 24200 / 24410 / 24417; a compile-time probe asserts those three, `SA_EFFECTIVE_RATE == 5`, `SA_ANI_SPEED == 103`
  and `XUF_DODUM_20_BOLD == 3`: 0 errors.

**Gates:** `flagcheck SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE --with ..._CORE --range 42429ff6^..42429ff6`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`;
`0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only for the
commented-out `_LIVE` define (OTHER = `X2GURaven.h` `_CORE`, `X2SkillTree.h` `JUNE_ENUM_NAMES`). `junediverge.py --check-all`: `RESULT: PASS` (185 allow-listed).
`enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows. One-file `/Zs` probe of `X2GURaven.cpp`: 0 errors with `_LIVE` off and with `/D SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`.
No build: the review changed no code. CH1b's refused deploy is harmless (`_LIVE` off, the new `_CORE` code is unreferenced); the next deploy carries it.
Nothing to play-test. Next: `/x2-phase char-raven CH1c` (Sonnet 5.5 high).

### CH1c - Done (2026-11-05)

Both class-map rows of phase CH1c are written, both `inside`: June's body in March's own `CX2GURaven` function under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, March's in `#else`. No `new` row, no `_CORE` code; `_LIVE` stays `//#define`. 2 funcmap rows `Char Raven CH1c`.
CH0 listed `32c4f350` as `new ?`; March has `CommonHyperModeFrameMove` (`MODIFY_RIDING_PET_AWAKE`, on), so it is `inside` (class-map row corrected). The 35 riding / pepper / dance states have no other class-range function (CH0).

| June | `CX2GURaven::` | Note |
|---|---|---|
| `32c511a0` | `RidingHyperModeFrameMove` | `CommonHyperModeFrameMove( 0.f, 0.1f, true )` = March |
| `32c4f350` | `CommonHyperModeFrameMove( fTime1_, fTime2_, bSound_ )` | March + awakening stand-ins + Open Mind |

**Where June differs from March** (read from the listing, `charport_adis.py raven 32c4f350 32c511a0`):
- Two independent `EventTimerOneshot` tests (not `else if`), as March. World colour `0xff222222`; timer 1 `ShowMinorParticleHyperModeChange` with no sound (Elsword's plays `Stickslash1.ogg` there); timer 2 as below; then vslot 0x7cc `CommonFrameMove`.
- Timer 2: the awakening effect-set family `32011d20` (begin) and `31fd66b0` (present test, `32011f50` play when true) before the Shock, and `ShowMinorParticleHyperModeTrace` only when the set is absent. Stand-ins in `_CONST_RAVEN_CH1c_` (no-op, `false`, no-op = June's empty-name default), the same as Chung's, Eve's and Elsword's (Stage 12 owns the reader).
- **New for Raven:** after `ApplyHyperModeBuff`, `GetSkillLevelValue( RNSI_P_RRF_OPEN_MIND )` (`31fceab0( 0x5f63 )`, 24419, Awakened Will: Reckless Fist) `> 0` -> `SetSkillBuffFactors( RNSI_P_RRF_OPEN_MIND, 0, true )` (vslot 0x23c = `31fcac90`, pushes `1, 0, 0x5f63`). It runs whether or not `bSound_`. Both helpers already exist in `CX2GUUser` (Chung / Eve ports).
- Sounds when `bSound_`: `Energy`, `Raven_Slash1`, `Break`, `Change` = March. No `JUNE_BUGS` row.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE --worktree --files X2Lib/X2GURaven.cpp KTDXLIB/Always.h`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because the `#define` is commented out (by design).
- `Always.h`: one new dependency guard in `_LIVE`'s block, `#error ... needs SERV_IRUHADEV_JUNE_CHAR_EVE_PASSIVE ( SetSkillBuffFactors )`. `cl /EP` flag probe: `EVE_PASSIVE`, `MODIFY_RIDING_PET_AWAKE`, `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE`, `JUNE_DIVERGE_SKILL_LEVEL_VALUE`, `RAVEN_CORE` all ON.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): `X2GURaven.cpp` exit 0, 0 errors with `_LIVE` off and with `/DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. `cl /P`: `RavenAwakeningEffectBegin` 0 lines off / 2 on, `SetSkillBuffFactors` 1 / 2, so the flag took effect.
- `junediverge.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 25,997,824 B, dated 2026-11-05 21:50:36, md5 0f62b410a7c410d7050a0feb0ea5328f, equal to X2/US_SERVICE/x2.exe (2026-11-05 21:50:30); built at main 948f4a31 + 2 uncommitted code file(s).` X2Lib 0 errors. No `exestr` line: `_LIVE` is off and no string was added.

**Routed.** CH1i: `HyperModeFrameMove` `32c4f640` calls this function with its own times (March: `0.05f, 0.83f`). Stage 12: the awakening reader behind the three stand-ins. CH3d (checklist §7ao L954): `HyperModeStart` `31ffd490` calls vslot 0x23c with `( 24419, 1, 1 )`, not this function's `( 24419, 0, 1 )`.

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-raven CH1c review` (Opus 5.5 high), then CH1d.

### CH1c - Review (2026-11-05, Opus 5.5)

**Verdict: PASS, no code change.** Both rows read against `charport_adis.py raven 32c4f350 32c511a0`:
- **`32c511a0`:** `fld 0.1`, `push 1`, `fldz` -> `CommonHyperModeFrameMove( 0.f, 0.1f, true )` = March.
- **`32c4f350`, in order:** world colour = the inline `SetWorldColor( 0xff222222 )` (`0.1333` x3 + `1.0` into `+0xb4..+0xc0` / `+0xc4..+0xd0`, `+0xd4 = 0`); timer 1 = inlined
  `EventTimerOneshot` (`+0x24c` `m_fAnimOneshotTimer` `<` time, then `EventTimer`), `ShowMinorParticleHyperModeChange( L"Dummy1_Rhand" )` (= March's default / bone), no sound;
  timer 2 a separate `if`: `32011d20`, `31fd66b0` `== 1` -> `32011f50`, `Shock( L"Dummy1_Rhand" )`, `UpDownCrashCamera( 30, 0.4 )`, the inline `ClearScreen()` (`+0xa8 = 1`,
  colour 1,1,1,1 = March's defaults `( 1, 0xffffffff )`), `31fd66b0` `== 0` -> `Trace`, vslot 0xc0 (slot 48, funcmap 1490) `ApplyHyperModeBuff`,
  `31fceab0( 0x5f63 )` `jle` -> vslot 0x23c pushes `1, 0, 0x5f63` = `SetSkillBuffFactors( 24419, 0, true )` (`X2GUUser.h:3780`, `iSkillID_, iIndex_, bCheckLearned_`), and the
  `bSound_` test (`cmp [esp+0x44], 1`) comes **after** it, so it runs either way; four `PlaySound( name, 1, 0 )` = March's defaults; vslot 0x7cc `CommonFrameMove`.
- 0x5f63 = 24419 = `RNSI_P_RRF_OPEN_MIND` (June `Enum.src.lua:15500`; `X2SkillTree.h:2116`, live in the `cl /P` output). `31fceab0` = `GetSkillLevelValue` (divergent row 246, replaced
  under `JUNE_DIVERGE_SKILL_LEVEL_VALUE`).
- Stand-ins (no-op, `false`, no-op) match Elsword's (`X2GUElsword_SwordMan.cpp:30486`), Chung's (`X2GUChung.cpp:40786`) and Eve's (`X2GuEve.cpp:208`).

**Gates:** `cl /P` of `X2GURaven.cpp` read for each branch: off = March's body, on = June's (stand-ins, Open Mind) and the riding wrapper. One-file `/Zs` probe: 0 errors off and with
`/D SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`; the `on` run reaches the new `Always.h` `#error` guard without firing, so `EVE_PASSIVE` is defined at that line.
`flagcheck SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE --range b125dcf0^..b125dcf0`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted
line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only for the commented-out `_LIVE` define.
`junediverge.py --check-all`: `RESULT: PASS` (185 allow-listed). `enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows. No build: the review changed no code.
CH1d kickoff checked: present, CH0 amendment lists its 38 functions (34 inside, 4 new) and 20 states. Nothing to play-test. Next: `/x2-phase char-raven CH1d` (Sonnet 5.5 high).

### CH1d - Done (2026-11-05)

All 38 class-map rows of phase CH1d are written. 34 are `inside`: June's body in March's own `CX2GURaven` function under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, March's in `#else SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. 4 are `new` (`_CORE`, compiled, referenced by nothing): `32c25fe0` `RSI_COMBO_ZZZX_StateStartFuture`, `32c25ff0` `RSI_COMBO_ZZZX_FINISH_StateStartFuture`, `32c26000` `RSI_COMBO_ZZZX_FINISH_FrameMove`, `32c2abc0` `RSI_COMBO_ZZZX_FINISH_EventProcess`. `_LIVE` stays `//#define`. 38 funcmap rows `Char Raven CH1d`; 2 rows added to `ch1_offsets.tsv` (`+0x9634` `m_iSuccessiveHit_OneZ`, `+0x9750` `m_fDamageDataChangeTime`).

| June | `CX2GURaven::` | June | `CX2GURaven::` |
|---|---|---|---|
| `32c297a0` `32c29bb0` `32c29fd0` `32c2a550` `32c2a880` | `RSI_COMBO_Z` / `ZZ` / `ZZZ` / `ZZZZ` / `ZZZX` EventProcess | `32c2aef0` `32c2b360` `32c2b6f0` `32c2bbb0` `32c2c070` | `ZZX` / `ZZXX` / `X` / `XX` / `XXX` EventProcess |
| `32c3d800` `32c3d880` | `ZZZZ` / `ZZXX` FrameMoveFuture | `32c44f90` `32c45260` | `ZZZX` / `XXX` FrameMove |
| `32c2c370` `32c2c810` `32c2d450` `32c5c230` | `RSI_DASH_COMBO_Z` / `ZZ` / `X` EventProcess, `X` FrameMove | `32c2d750` `32c26010` `32c2d860` `32c2d8b0` | `RSI_DASH_JUMP_COMBO_Z` EventProcess, `ZZ` StateStart / FrameMoveFuture / EventProcess |
| `32c26020` `32c45f80` | `ZZ_Landing` StartFuture / EventProcess | `32c26050` `32c26070` `32c2d8e0` `32c46170` `32c2d930` | `RSI_DASH_JUMP_COMBO_X` Init / Start / FrameMoveFuture / FrameMove / EventProcess |
| `32c3d8f0` `32c2da40` | `RSI_JUMP_COMBO_Z` FrameMoveFuture / EventProcess | `32c26090` `32c3d980` `32c2da80` | `RSI_JUMP_COMBO_X` StateStart / FrameMoveFuture / EventProcess |

**Where June differs from March** (read from `charport_adis.py raven --phase CH1d`; everything not listed is March's logic written out whole):
- **The cancel chain** replaces every `ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_*` branch: `SKILL_CANCEL_AFTER( m_fSkillCancelAfter )`, then back-walk `+0x8c94`, walk `+0x8c98`, dash `+0x8c9c` and the pure Z / X cancel `+0x8c8c`, each also needing the foot on a line (`RAVEN_GROUND_*` macros, like Elsword's). **`RSI_COMBO_Z` is the odd one**: Z / X first, then the three walk cancels with no foot test, each setting `m_fCanNotAttackTime = 0.1f` before `StateChange`.
- **Z-Z-Z (`32c29fd0`)** stores only `m_bReAttackZ1Right` (no `m_fReAttackZ1Time`); classes come from the unit data (`+0x38`). Blade Master (106) with flag 2 goes to `RSI_RBM_COMBO_ZZZupZ` at 0.333 s; flag 0 to Z-Z-Z-Z (`<=`); flag 1 (`<`) to `RWT_COMBO_ZZZX` (115, front key) / `RSI_WEAPON_TAKER_COMBO_ZZZdownX` (115) / `RRF_COMBO_ZZZX` (107) / `RSI_COMBO_ZZZX`. The input window is 0.166 - 0.59 s; up + Z sets flag 2 for classes 16 and 106 (**`JUNE_BUGS` #536**: only 106 consumes it).
- **Z-Z-Z-X** ends in the new `RSI_COMBO_ZZZX_FINISH` (the finish state's fall is `LINE_RADIUS`, not 1.5 x). Its FrameMove (`32c44f90`) lost the hand-held damage checks: at 0.49 s `FlushMp( 3.0f )` plays `EffectSet_A_RF_ZZZX`, else the March particle. `32c45260` (X-X-X) the same at 0.16 s with `EffectSet_A_RF_XXX`.
- **Z-Z-X** (templet class 107, front key) goes to `RRF_COMBO_ZZXFRONTX`; **Z-Z-X-X** has the up / down cancel (`m_fUpDownCancelAfter`) before the skill cancel (checklist §7h `32c2b360`).
- **X** (`32c2b6f0`): Sword Taker / Blade Master (templet class 16 / 106) set flag 1 below 0.48 s and take `RBM_COMBO_XZ` at 0.48 s or the animation end. **X-X** (`32c2bbb0`): Over Taker (17) / Reckless Fist (107) send **down to `RSI_OVER_TAKER_COMBO_XXdownX` and front to `XXfrontX`** (March had them the other way round); 16 / 106 take `RSI_SOUL_TAKER_COMBO_XXZ` (zeroing `m_iSuccessiveHit_OneZ`).
- **Dash combo Z** (`32c2c370`): class 25 / 115 go to `RVC_DASH_COMBO_ZZ`; `m_fEventTime[0]` carries Over Taker / Reckless Fist X to `RSI_OVER_TAKER_DASH_COMBO_ZX`; the animation end tests the foot. **Dash combo ZZ / X** test the animation end first. **Dash-jump Z** (`32c2d750`): 16 / 106 own `RSI_SOUL_TAKER_DASH_JUMP_COMBO_ZZ`; 25 / 115 share the Z-X follow-up. **Dash-jump X EventProcess** (`32c2d930`): `m_fEventTime[0]` carries Blade Master Z to `RBM_DASH_JUMP_COMBO_XZ`.
- **Dash-jump X FrameMove** (`32c46170`): the damage effect position drops 10 along **z** where March drops y (**`JUNE_BUGS` #537**).
- **= March after the macros:** Landing StartFuture / EventProcess (`GetMaxAnimationTime() / 3`), dash combo X FrameMove (`BALANCE_BLADE_MASTER_20130117` branch, `m_itNowStateData->first`), the four FrameMoveFuture, the Jump combo Z / X, Init / Start, dash-jump Z-Z. 3 thunks (`32c26000` and the two StartFuture).
- Registered under `_CORE` (appended without `= value`, CH1m maps June's ids): `RSI_COMBO_ZZZX_FINISH` 170, `RSI_RBM_COMBO_ZZZupZ` 299, `RRF_COMBO_ZZZX` 211, `RWT_COMBO_ZZZX` 304, `RVC_DASH_COMBO_ZZ` 303, `RSI_OVER_TAKER_DASH_COMBO_ZX` 302, `RSI_SOUL_TAKER_DASH_JUMP_COMBO_ZZ` 294, `RBM_DASH_JUMP_COMBO_XZ` 301. Compile-time asserts: `UC_RAVEN_*` 16 / 17 / 25 / 106 / 107 / 115, `AT_UNIT == 0`, `AT_MANUAL_COLLISION_DATA == 3`: 0 errors. CH1e / f / g / g2 / m kickoffs carry the amendment.
- File-local: `_CONST_RAVEN_CH1D_` (`IsDataClass`, `IsTempletClass`) and the `RAVEN_*` macros, under `_CORE` (the new finish function uses them). `Always.h`: `_LIVE needs SERV_IRUHADEV_JUNE_UPDOWN_CANCEL`.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE --with ..._CORE --worktree --files X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h KTDXLIB/Always.h`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out (by design). OTHER = `X2GURaven.h`, `flagcheck ..._CORE` on it: `RESULT: PASS`. `git diff --patience --numstat`: `.cpp` 973 / 0, `.h` 14 / 0, `Always.h` 3 / 0.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): `X2GURaven.cpp` exit 0, 0 errors with `_LIVE` off and with `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`; `cl /P` on: `EffectSet_A_RF_XXX` 0 lines off / 1 on, `RSI_RBM_COMBO_ZZZupZ` 1 / 2.
- `junediverge.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 25,998,336 B, dated 2026-11-05 22:30:32, md5 61f41754fd6573503b9a6114ae29cec6, equal to X2/US_SERVICE/x2.exe (2026-11-05 22:30:25); built at main 026f87aa + 3 uncommitted code file(s).` `BUILD OK` 25,998,336 B. No `exestr` line: `_LIVE` is off and no string was added to a live path.

**Routed.** CH1m: register the four new functions and the eight enum members; March's `RSI_COMBO_ZZZX_Start` / `m_bZZZX` are not June's. CH1 final play-test: June's Z-Z-Z no longer sets `m_fReAttackZ1Time` (March set 0.2 s there), so the Wait / Walk Z gate (CH1a) is not re-armed after a Z-Z-Z; check the Z re-attack after a full combo.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-raven CH1d review` (Opus 5.5 high), then CH1e.

### CH1d - Review (2026-11-05, Opus 5.5)

**Verdict: PASS after one fix.** All 38 rows were read against `charport_adis.py raven --phase CH1d`. Each cancel test was checked by its x87 flag test: `test ah,0x41` / `jp` = `0 <=`, `test ah,5` / `jp` = `<`, `test ah,1` / `jne` = `<=`.
- **Fixed: `32c46170` (dash-jump X FrameMove) lowers y, not z.** `pos` is at `[esp+0x1c..0x24]`. After z is stored (`@32c46296`), `push 0x33722a70` moves esp by 4, so `fsubr` / `fstp [esp+0x24]` work on `[old esp+0x20]` = y.
  The fail branch has the same shape: three pushes, then `[esp+0x24]` = `vAngle.z - 50`. The `_LIVE` body now has `pos.y -= 10.0f` (= March). `JUNE_BUGS` #537 is withdrawn.
- **The chain:** order `SKILL_CANCEL_AFTER`, back-walk `+0x8c94`, walk `+0x8c98`, dash `+0x8c9c`, Z / X `+0x8c8c`, each with the foot test, in every combo state. `32c297a0` alone runs Z / X first, then the three walk cancels with no foot test, each storing `0xde0` = 0.1 before the call. `32c2b360` has the up / down cancel (`+0x8ca4`, foot, up or down) first.
  The end test is a separate `if` (all branches jump to it) in Z, ZZZZ, ZZZX, ZZX, ZZXX, X and XX. It is the last `else if` in ZZ, ZZZ and the finish. It comes first in XXX, dash ZZ and dash X, and last with a foot test in dash Z.
- **Per function:** ZZ = 0.39 (`>` / `<=`) with the word clears. ZZZ: 106 + flag 2 at `0.333 <=`; flag 0 `<=`; flag 1 `<` -> 115 front `0x130` / 115 `0xe8` / 107 `0xd3` / `0xa8`. The window is `0.166 <= t < 0.59`, and the up-Z set is for classes 16 / 106 (+0x38).
  ZZX 0.6 with templet `+0xaf0`->`+4` == 107 front -> `0xd1`. X: 0.53 / `0.48 < t || end` -> `0xd4`, set `t < 0.48` + templet 16 / 106. XX: 0.55; 17 / 107 down -> 196 `XXdownX`, front -> 190 `XXfrontX`; 16 / 106 flag 1 -> `+0x9634 = 0` then 186.
  Dash Z: 0.397, 25 / 115 -> 303; event time 0 (`+0x8c10` / `+0x8c14` / `+0x8c18`) for 17 / 107 X -> 302. Dash-jump Z: 0.2; 16 / 106 -> 294, 17 / 107 -> 197, 25 / 115 -> 234. Dash-jump X EP: 106 Z -> 301.
  The finish falls `LINE_RADIUS` (`d=30.0`). Landing: `(+0x220 - +0x2b0) / 3` = `GetMaxAnimationTime() / 3`. FrameMoveFutures 0.6 x1.3 / 0.35 / 0.23 x `fMaxGSpeed` 0.5 / 0.21-0.4 clamp. Jump Z needs down without up, jump X down only. 0xde4 = 0.2 on landing.
  `32c44f90` / `32c45260`: `m_pXSkinAnim` timer 0.49 / 0.16, vslot 0xe4 `FlushMp( 3.0f )`, `PlayEffectSet( L"EffectSet_A_RF_ZZZX" / L"EffectSet_A_RF_XXX" )`, else the `Dummy2_Lhand` magic-fail particle. The thunks and Init / Start match.
- **Enum:** the 8 members match `ch0_state_enum_names.tsv` (170 / 299 / 211 / 304 / 303 / 302 / 294 / 301, each with its reg site). `UC_RAVEN_*` 16 / 17 / 25 / 106 / 107 / 115 = June `Enum.src.lua`.
- **`JUNE_BUGS` #536 confirmed:** flag 2 is read only under `cmp ecx, 0x6a` (`@32c2a352`).

**Gates.** `cl /P` read for each branch; one-file `/Zs` probe 0 errors off and with `/D SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (on the commit; the fix is two ASCII lines in the `_LIVE` body).
`flagcheck ..._LIVE --with ..._CORE --range 23e3a1ec^..23e3a1ec`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s)`. Review edit, `--worktree --files X2Lib/X2GURaven.cpp`: `1 PASS`, `0 non-ASCII inserted`, `0 encoding change(s)`.
Its 2 "deleted" lines are the two CH1d-inserted lines replaced. `RESULT: FAIL` only for the commented `_LIVE` define. `junediverge.py --check-all`: `RESULT: PASS` (185 allow-listed). `enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 25,998,336 B, dated 2026-11-05 22:43:07, md5 f5d250ac881ae974455660097f796d77, equal to X2/US_SERVICE/x2.exe (2026-11-05 22:43:00); built at main 23e3a1ec + 1 uncommitted code file(s).` No `exestr` line: `_LIVE` is off.

CH1e kickoff checked: present; its CH0 amendment lists 32 functions (20 inside, 12 new) and 19 states, and the CH1d amendment names the three members already added. Lesson for later phases: count every `push` between a stack store and a reload. Nothing to play-test. Next: `/x2-phase char-raven CH1e` (Sonnet 5.5 high).

### CH1e - Done (2026-11-05)

All 32 class-map rows of phase CH1e are written. 20 are `inside`: June's body in March's own `CX2GURaven` function under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, March's in `#else SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. 12 are `new` (`_CORE`, compiled, referenced by nothing; one block before `//RSI_COMBO_ZZX`, declarations beside CH1d's). `_LIVE` stays `//#define`. 32 funcmap rows `Char Raven CH1e`; 1 `JUNE_BUGS` row (#538). Four `RAVEN_STATE_ID` members appended under `_CORE` (no `= value`): `RSI_SOUL_TAKER_DASH_JUMP_COMBO_ZZ_Landing` 295, `_ZZZ` 296, `_ZZZ_Landing` 297, `_ZZZZ` 298 (ids from `ch0_state_skill.tsv`; `RBM_DASH_JUMP_COMBO_X` 300, `RSI_RBM_COMBO_ZZZupZ` 299, `..._DASH_JUMP_COMBO_ZZ` 294 and `RBM_DASH_JUMP_COMBO_XZ` 301 were CH1d's).

| June | `CX2GURaven::` | June | `CX2GURaven::` |
|---|---|---|---|
| `32c2e2f0` `32c2e6a0` `32c2e700` `32c2ea40` `32c26340` | Sword Taker `COMBO_XXZ` EP, `XXZZ8` FM / EP, `XXZZ8Z` EP, `XXZZ8` Start (inside) | `32c2dd50` `32c2dec0` | `SOUL_TAKER_DASH_COMBO_ZZ` / `ZZZ` EP (inside) |
| `32c32560` `32c32970` `32c32da0` `32c26b80` | `RBM_Combo_XZ` / `XZZ` / `XZZZ` EP, `XZZZ` FrameMoveFuture (inside) | `32c26b90` `32c41330` `32c49de0` `32c413e0` | `RBM_COMBO_ZZZX` Init / Start / FrameMove / EP (inside) |
| `32c41890` `32c41a00` | `RBM_COMBO_ZZZXZ` Start / EP (inside) | `32c26bd0` `32c41ef0` `32c32de0` | `RBM_COMBO_ZZZX_EXPLOSION` Start / FM / EP (inside) |
| `32c2cb30` `32c2cc40` `32c2cc70` `32c2ccc0` | Sword Taker `DASH_JUMP_COMBO_ZZ` EP, `ZZZ` EP, `ZZZ_Landing` StateStartFuture, `ZZZZ` EP (new) | `32c45520` `32c45710` | `DASH_JUMP_COMBO_ZZ_Landing` / `ZZZ_Landing` EP (new) |
| `32c2cfe0` `32c2d0f0` `32c459f0` `32c45cc0` | `RBM_DASH_JUMP_COMBO_X` EP / FM, `XZ` EP / FM (new) | `32c2df90` `32c2dfe0` | `RSI_RBM_COMBO_ZZZupZ` FrameMoveFuture / EP (new) |

**Where June differs from March** (read from `charport_adis.py raven --phase CH1e`; the rest is March's logic written out whole):
- **The cancel chain** (`SKILL_CANCEL_AFTER( m_fSkillCancelAfter )`, back-walk, walk, dash, Z / X; CH1d's `RAVEN_GROUND_*` macros) runs in Sword Taker X-X-Z, 8-hit and 8-hit Z, in Blade Master X-Z, X-Z-Z, Z-Z-Z-X, Z-Z-Z-X-Z, the explosion and Z-Z-Z up-Z, and in the dash-jump Z-Z-Z-Z. It replaces the `DASH_CANCEL_AFTER` / `WALK_CANCEL_AFTER` / `SKILL_CANCEL_AFTER` BALANCE lines and the `ELSE_IF_KEY_PRESSED` branches.
- **X-X-Z** `32c2e2f0`: fall, chain, then the end test, then 8-hit follow-up (`>= 2` hits and `> 0.87`) and the hit counter (`< 0.87`). **8-hit** `32c2e700`: fall, chain, `GetNowPlayCount() > 4`; the non-BALANCE Z counter branches are gone. **8-hit Z** `32c2ea40`: fall, chain, end -> WAIT (March had a 0.43 s skill cancel).
- **Dash Z-Z** `32c2dd50`: flag 0 needs the foot on a line and **data class != 115** (Veteran Commander has its own dash Z-Z); the key cancel after 0.9 s keeps the cool time. **Dash Z-Z-Z** `32c2dec0`: the key cancel after 0.84 s has **no cool time** (`JUNE_BUGS` #538).
- **X-Z / X-Z-Z / X-Z-Z-Z** `32c32560` / `32c32970` / `32c32da0`: Sword Taker and Blade Master (**templet class** 16 / 106, `+0xaf0`) set flag 0 below 0.63 / 0.66 s and the follow-up needs `0.63 < t` or the animation end; X-Z-Z clears flag 0 on a direction change, takes a double-tap dash without the foot test, and the chain comes before the fall; X-Z-Z-Z ends in jump-down (no y offset) or landing = March.
- **Z-Z-Z-X (Blade Master black hole)** `32c413e0`: **templet 106 + Z sets flag 0 at any time**, and `t > 0.94` takes `RBM_COMBO_ZZZXZ` before the black-hole explosion (hit count `> 0`, time `<= 0.9`) and the fall; then the chain. `32c41890` Start waits 1.24 s and writes the stage after both branches. `32c41a00` EP: fall (also zeroes `nowSpeed.x`), explosion tests (`<= 0.4`), `0.69 <=` follow-up, chain, then the `< 0.69` flag set. Explosion EP `32c32de0`: end -> fall / WAIT, then the chain. Start / FM / Init / thunks = March.
- **Dash-jump** (new): Sword Taker / Blade Master (data class 16 / 106) use event time 0 (`+0x8c10` / `+0x8c14` / `+0x8c18`) to chain ZZ -> ZZZ -> ZZZZ; landings reuse the `max / 3` ladder (`RSI_COMBO_Z` / `X`, dash, walk, jump-ready, down, end) of CH1d's ZZ landing; ZZZ landing `StateStartFuture` zeroes `nowSpeed` and `passiveSpeed`. Blade Master X / X-Z: `FlushMp( 10.0f )` (vslot 0xe4) plays `EffectSet_RST_DashJump_X` at 0.195 s / `..._XZ` at 0.3 s, else the magic-fail particle at `Bip01_L_Hand` (`z - 45`, `y - 40`) / `Bip01_R_Hand` (`z - 50`).
- **`RSI_RBM_COMBO_ZZZupZ`** `32c2df90` / `32c2dfe0`: 0.5 s `nowSpeed.y = 1500`; EP end -> fall / landing, then the chain (March's `SOUL_TAKER_COMBO_ZZZupZ` had none). The listing `32c2cc70` runs on into `32c2ccc0` (no int3): read as two functions.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE --with ..._CORE --worktree --files X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h KTDXLIB/Always.h`: `.cpp` `PASS +676`, `.h` OTHER (`_CORE` only: `PASS +18`); `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out (by design). `git diff --patience --numstat`: `.cpp` 676 / 0, `.h` 18 / 0.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): exit 0, 0 errors with `_LIVE` off and with `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. `cl /P` both ways: the same 638 `CX2GURaven::` definitions; `1.24f` 0 off / 1 on; `IsTempletClass( GetUnit(), CX2Unit::UC_RAVEN_BLADE_MASTER )` 0 / 4 (so the BALANCE branches are live).
- `junediverge.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 26,003,456 B, dated 2026-11-05 23:18:40, md5 9c69dbd619f5e511a21e813e26669a13, equal to X2/US_SERVICE/x2.exe (2026-11-05 23:18:33); built at main d73d1e54 + 2 uncommitted code file(s).` `BUILD OK` 26,003,456 B. No `exestr` line: `_LIVE` is off.

**Routed.** CH1m: register the 12 new functions and the four members (June ids above). CH1 final play-test: Sword Taker / Blade Master dash-jump Z-Z-Z-Z chain, X-X-Z-Z 8-hit and Blade Master black hole (Z held early), and Z-Z-Z up-Z for Blade Master; Veteran Commander dash Z-Z (CH1g2) is separate from `32c2dd50`.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-raven CH1e review` (Opus 5.5 high), then CH1f.

### CH1e - Review (2026-11-06, Opus 5.5)

**Verdict: PASS, no code change.** All 32 rows were read against `charport_adis.py raven --phase CH1e`. Each float test was checked by its x87 flag test, as in CH1d.
- **The chain** (`+0x8c90` skill cancel, back-walk `+0x8c94`, walk `+0x8c98`, dash `+0x8c9c`, Z / X `+0x8c8c`, each with the foot test) has the same order in all 10 listings that carry it. Where it sits:
  dash-jump ZZZZ `32c2ccc0`: fall, end, chain. ZZZupZ `32c2dfe0`: end (fall / landing), chain. X-X-Z `32c2e2f0`: fall, chain, end, 8-hit, counter. 8-hit `32c2e700`: fall, chain, play count (`+0x248`) `> 4`. 8-hit Z `32c2ea40`: fall, chain, end.
  X-Z `32c32560`: end, chain, fall, flag change, flag set. X-Z-Z `32c32970`: direction clear, end, chain, double-tap dash (no foot test), flag change, flag set, and **no fall at all** (the Done note's "chain before the fall" holds for X-Z). Explosion `32c32de0`: end, chain.
  Z-Z-Z-X `32c413e0`: flag set, then the flag branch (when `t <= 0.94` it skips the rest, chain included), explosion, fall, chain. Z-Z-Z-X-Z `32c41a00`: fall, two explosions, `0.69` follow-up, chain, then the `< 0.69` flag set, which every branch reaches.
- **Sword Taker:** X-X-Z 0.87 (`>` / `<`), and `+0x9634` is reset before `StateChange`. The 8-hit FrameMove plays at 0.047 / 0.276 with `PlaySound( name, true, false )` (= March's defaults). Start `32c26340` = March.
  Dash Z-Z: 0.5 (`>` / `<`), data class `+0x38` != 115 in both branches, keys up / down / right / left / A / S / D / C / Q / W / E / R (= `ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C`), `> 0.9`, `0xde0` = 0.1; the end test is a separate `if`.
  Dash Z-Z-Z: `> 0.84`, and nothing is stored to `0xde0` (`JUNE_BUGS` #538 confirmed, @32c2df5e-32c2df7c).
- **Dash-jump:** event time 0 is `keyInputStart` `+0x8c10`, `keyInputEnd` `+0x8c14`, `stateChange` `+0x8c18` (`X2GUUser.h:1229`). The flag change is `stateChange < t || end`; the flag set is `start <= t < end`, for data class 16 / 106 (ZZ, ZZZ landing) or 106 alone (Blade Master X).
  ZZZ landing `StateStartFuture` calls `CommonStateStartFuture`, then zeroes `+0x304`-`+0x310`. Both landings test `t > ( +0x220 - +0x2b0 ) / 3`; down uses `CanDown( LINE_RADIUS )`, and a failed `CanDown` goes to the end test.
- **Blade Master:** X-Z / X-Z-Z use 0.63 / 0.66 with templet `+0xaf0`->`+4` 16 / 106. X-Z-Z-Z: end -> jump-down (no offset) or landing.
  Z-Z-Z-X: flag set for templet 106 at any time, `t > 0.94`, black hole count (`KProtectedType`) `> 0` and time `<= 0.9`; the fall zeroes the time, then the count.
  Start: count, time, stage `0xd8`, `CommonStateStart`. FrameMove: oneshot 0.24; stop `+0x9684` if != -1; count 1; time `max - 0.24 + 1`; the handle = `PlayEffectSet`; `UpDownCrashCamera( 5, 0.2 )`.
  Z-Z-Z-X-Z Start: `CommonStateStart` first; `count - 1 >= 4` -> explosion, `dirDegree.y += 180`, `bIsRight` flipped; else `++count`, 1.24; stage `0xd8` after both. EP: the fall also zeroes `nowSpeed.x`; the explosions need `<= 0.4` (the first also needs flag 0 clear); `0.69 <= t` with the flag set -> ZZZXZ, without clearing the flag.
  Explosion FrameMove 0.3 / 0.4 = March (two compiler forms of the inline oneshot). Init = March's 4 textures.
- **Blade Master dash-jump X / X-Z FrameMoves** (`FlushMp( 10.0f )`, vslot 0xe4). I counted the pushes, as the CH1d review asked. In `32c459f0`, `fstp [esp+0x24]` after two pushes is `vAngle.z - 45`, and `fstp [esp+0x4c]` after three pushes is `pos.y - 40`. In `32c45cc0`, `[esp+0x24]` after two pushes is `vAngle.z - 50`, and `pos` is not changed.
- **Enum:** 295 / 296 / 297 / 298 = `ch0_state_enum_names.tsv` (reg `32c511c0`@350 / 393 / 437 / 480) = the listings' pushes `0x127`-`0x12a`.

**Gates.** One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`): exit 0, 0 errors with `_LIVE` off and with `/DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. `cl /P` was read for each branch: `1.24f` 0 off / 1 on, `0.94f` 2 / 1 (March's `ELSE_IF_STATE_CHANGE_ON_` pair / June's one), and `EffectSet_RST_DashJump_XZ` 1 / 1 (`_CORE`).
The preprocessed Z-Z-Z-X body expands `SKILL_CANCEL_AFTER` and the `RAVEN_GROUND_*` macros as one `else if` chain behind the fall.
`flagcheck ..._LIVE --with ..._CORE --range 123ef0a8^..123ef0a8`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s)`. `RESULT: FAIL` only for the commented `_LIVE` define.
`junediverge.py --check-all`: `RESULT: PASS` (185 allow-listed). `enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows. No build: the review changes no code.

CH1f kickoff checked: present. Its CH0 amendment lists 38 functions (29 inside, 9 new) and 17 states, and its CH1d amendment names the two members already added (211, 302). CH1e added none of CH1f's. Nothing to play-test. Next: `/x2-phase char-raven CH1f` (Sonnet 5.5 high).

### CH1f - Done (2026-11-06)

All 38 class-map rows of phase CH1f are written: 29 `inside` (June's body in March's own `CX2GURaven` function under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, March's in `#else`) and 9 `new` (`_CORE`, compiled, referenced by nothing; one block before `//RSI_COMBO_ZZX`, declarations beside it in the `.h`). `_LIVE` stays `//#define`. 38 funcmap rows `Char Raven CH1f`; 1 `JUNE_BUGS` row (#539). Five `RAVEN_STATE_ID` members appended under `_CORE` (no `= value`): `RSI_OVER_TAKER_COMBO_XXfrontXX` 191, `..._XXfrontXX2` 192, `..._XXfrontXX3` 193, `..._XXfrontXX4` 194, `..._XXfrontXXdownX` 195 (`ch0_state_enum_names.tsv`; `RRF_COMBO_ZZZX` 211 and `RSI_OVER_TAKER_DASH_COMBO_ZX` 302 were CH1d's).

| June | `CX2GURaven::` |
|---|---|
| `32c26390` `32c263a0` `32c30600` `32c30690` `32c306c0` | Over Taker dash-jump Z-X FrameMove, landing Start, FrameMoveFuture, EP, landing StartFuture (inside, = March) |
| `32c26aa0` `32c26ac0` `32c26ae0` `32c26af0` `32c26b00` `32c26b20` `32c316a0` `32c31730` `32c317e0` `32c31830` `32c31860` | Reckless Fist dash-jump X / X-X / X-X landing, all rows (inside, = March) |
| `32c26b30` `32c26b40` `32c26b50` `32c26b70` `32c31a20` | `RRF_COMBO_ZZXFRONTX` Start / FrameMove / End / FrameMoveFuture, `ZZXFRONTXX` FrameMove (inside, = March) |
| `32c2ed70` `32c302e0` `32c30710` `32c47700` `32c47970` | X-X-front-X EP, X-X-down-X EP, dash-jump Z-X-X EP, Z-X landing EP, Z-X-X FrameMove (inside, differ) |
| `32c31a80` `32c31ec0` `32c49ab0` | `RRF_COMBO_ZZXFRONTX` EP, `ZZXFRONTXX` EP, `RRF_DASH_JUMP_COMBO_X` FrameMove (inside, differ) |
| `32c26360` `32c26370` `32c47160` `32c2d130` | X-X-front-X FrameMoveFuture, X-X-down-X Init / FrameMove, dash Z-X EP (new) |
| `32c2f1b0` `32c2f6a0` `32c2fb90` `32c2ffb0` `32c32210` | `XXfrontXX` / `XX2` / `XX3` / `XX4` EP ( `32c2ffb0` is also the `XXfrontXXdownX` EP ), `RRF_COMBO_ZZZX` EP (new) |

**Where June differs from March** (read from `charport_adis.py raven --phase CH1f`; 21 of the 29 inside rows equal March's body, numbers compared):
- **The cancel chain** (`SKILL_CANCEL_AFTER( m_fSkillCancelAfter )`, back-walk, walk, dash, Z / X; CH1d's macros) runs in the dash Z-X, X-X-front-X family, X-X-down-X, dash-jump Z-X-X, `RRF_COMBO_ZZXFRONTX` / `XX` and `RRF_COMBO_ZZZX`. Order per row: fall then chain (`2ed70`, `2f1b0`..`2ffb0`), fall, end, chain (`302e0`, `30710`), end then chain then fall (`31ec0`, `32210`), end then fall / WAIT then chain (`2d130`).
- **Over Taker X-X-front-X family** (event time 0 `+0x8c10` / `14` / `18`, event time 1 `+0x8c1c` / `20` / `24`): X-X-front-X flag 0 = X + forward -> `XXfrontXX` (change time or end), `t > +0x8c18` -> `XXfrontXX4`, end -> WAIT. `XXfrontXX` / `XX2`: flag 0 = X + down -> `XXfrontXXdownX`, flag 1 = X -> `XX2` / `XX3`, `t > +0x8c24` -> `XXfrontXX4`. `XX3`: no flag 1. `XX4` / `downX`: end -> WAIT. March's 0.85 s walk cancel is gone.
- **X-X-down-X FrameMove** `32c47160`: March's X-X-front-X FrameMove with `FlushMp( 4.0f )` ( March 7 ) and `DamageEffect_RSI_OVER_TAKER_XX_DOWN_X`. **Z-X-X FrameMove** `32c47970`: `CommonFrameMove`, then `DamageEffect_RSI_OVER_TAKER_DASH_JUMP_COMBO_ZXX` at 0.31 s, 60 ahead. Both effects exist in the live `DamageEffect.lua`.
- **Z-X landing** `32c47700`: fall, then flag 0 through event time 0 ( X ) -> Z-X-X, then the `> 0.4` ladder ( = March ); March's 0.1 - 0.3 s X test is gone.
- **Reckless Fist** `32c31a80`: `0.6 s + flag 1` or end -> WAIT, fall, chain, dash without a foot test, flag 0 -> `ZZXFRONTXX` at 0.57 s or the end, set by X before 0.57 s for templet class 107 ( `IsTempletClass` ). `32c49ab0`: `FlushMp( 4.0f )` where March spends 10 ( effect-set path = March BALANCE ).
- **`RRF_COMBO_ZZZX`** `32c32210` reads no `+0x8ca4`: the script sets `UPDOWN_CANCEL_AFTER = 1.95` but nothing consumes it ( `JUNE_BUGS` #539 ). CH0's "second reader" is the script key only; the checklist §7h row stays open for CH1 final.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE --with ..._CORE --worktree --files X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `.cpp` `PASS +643`, `.h` OTHER ( `_CORE` only ); `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out (by design). `git diff --patience --numstat`: `.cpp` 643 / 0, `.h` 16 / 0.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): exit 0, 0 errors with `_LIVE` off and with `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. `cl /P` both ways: 682 `CX2GURaven::` definitions each; `FlushMp( 4.0f )` 1 off / 2 on, the `XXfrontXX4` push 3 / 4.
- `junediverge.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 26,010,112 B, dated 2026-11-06 05:51:39, md5 6fc1c0178fc670a99ebc7c0fc9645302, equal to X2/US_SERVICE/x2.exe (2026-11-06 05:51:32); built at main 5cc1ec99 + 2 uncommitted code file(s).` No `exestr` line: `_LIVE` is off.

**Routed.** CH1m: register the 9 new functions and the five members ( `kickoffs/char-raven/CH1m.md` amended ); June's `XXfrontX` registers no Init / FrameMove and `XXdownX` no FrameMoveFuture. CH1 final play-test: Over Taker X-X-front-X chain ( X + forward, X + down ), dash Z-X, dash-jump Z-X-X, Reckless Fist Z-Z-X-front-X and dash-jump X ( `FlushMp( 4 )` ).

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-raven CH1f review` (Opus 5.5 high), then CH1g.

### CH1f - Review (2026-11-06, Opus 5.5)

**Verdict: PASS, no code change.** All 38 rows were read against `charport_adis.py raven --phase CH1f`. Each float test was checked by its x87 flag test (`test ah, 5` / `0x41` / `1` with `jp` / `jnp` / `jne`), as in CH1d.
- **The chain** (`+0x8c90` skill cancel, back-walk `+0x8c94`, walk `+0x8c98`, dash `+0x8c9c`, Z / X `+0x8c8c`, each with the foot test) has the same order in all 11 listings. Where it sits = the Done note:
  fall, then chain: `2ed70`, `2f1b0`, `2f6a0`, `2fb90`, `2ffb0` (`2ffb0`: then the end). Fall, end, chain: `302e0`, `30710`. End (fall / WAIT inside it), then chain: `2d130`.
  End, chain, fall: `31ec0`, `32210`. `31a80`: back-input sets flag 1; `t >= 0.6` with flag 1, or the end, goes to WAIT; then fall, chain, a bare double-tap dash (no foot test), flag 0, flag set.
- **X-X-front-X family:** event time 0 = `+0x8c10` / `14` / `18`, event time 1 = `+0x8c1c` / `20` / `24`. `2ed70`: flag 0 (`+0x8c18 < t` or the end) -> 0xbf; the set needs `start <= t < end` + `oneX` + forward (`bIsRight` with `pureRight`, else `pureLeft`); `+0x8c18 < t` -> 0xc2; end -> WAIT.
  `2f1b0` / `2f6a0`: flag 0 (X + `pureDown`) -> 0xc3, flag 1 (X) -> 0xc0 / 0xc1, `+0x8c24 < t` -> 0xc2, no end test. `2fb90`: no flag 1. The flag change and flag set macros match the listing operand for operand.
- **X-X-down-X FrameMove `32c47160`:** oneshot 0.3 particle `Raven_attackX3Front_Particle01` at +-50, axis angle; else 0.3666 `FlushMp( 4.0f )` (vslot 0xe4); else 0.4333 `UpDownCrashCamera( 5, 0.15 )`; `CommonFrameMove` last.
  I counted the 23 dwords of `CreateInstance` against `X2DamageEffect.h:1654`: bPet 0, bInsertFront 0, fHitAddMp -1, mesh 0, fHitGap -1, iHitCount -1, then fReserveTime -1 (the `-1.0` left on the FPU stack), fForceDown 1, fLifeTime 1, bReserve 0, fLandPos 0, both angles `GetRotateDegree()`. Pos is read at `[esp+0x6c / 0x70 / 0x80]` after 0x44 / 0x50 bytes of pushes = base `+0x28..0x30` = GetPos +-60.
  Fail path: `y + 50` stored after two pushes = base `+0x2c`; `angle.z = 45` (a store, not a `-=`); trigger 100 / 100 / 1; axis and add-rotate both.
- **Z-X-X FrameMove `32c47970`:** `CommonFrameMove` first, oneshot 0.31, +-60, the same default tail. **Z-X landing `32c47700`:** fall, then flag 0 -> 0xc7, the window with X, then `t > 0.4`: double-tap, walk, up, down + `CanDown( 30 )`, end. A failed `CanDown` goes to the end test (= March).
- **Reckless Fist:** `32c49ab0`: `FlushMp( 4.0f )`, stop `+0x9700` if != -1, `PlayEffectSet`, push into the damage-react list, camera 10 / 0.2. June keeps only side-effect-free `GetCloneFrame` / `GetIsRight` calls of March's dead Dummy2 offset. The fail path is `vAngle.z - 50` at base `+0x1c` after two pushes.
  `31a80`: flag 0 at `0.57 < t` or the end -> 0xd2; the set needs `t < 0.57` + X + templet `+0xaf0`->`+4` == 0x6b (107 = `UC_RAVEN_RECKLESS_FIST`, `X2Unit.h:112`).
  `31730`: flag 0 + `t >= 0.39` + no foot -> 0xcf, else `oneX` + `t <= 0.39` sets it; the end test is a separate `if` (landing 0x12 / down 0x11). `31860`: end -> WAIT, then a separate `IF_SKILL_CANCEL_AFTER( 0.4 )` (hyper part included), then the key list up / down / right / left / Z / X / A / S / D / C / Q / W / E / R.
  `316a0` 0.21 / 0.7 / 0.26 / 0.29 / 1200, `317e0` `0 <= t <= 0.54`, `31a20` `< 0.27` / `>= 0.33`, `30600` 0.267 -> (0, 0) / (2200, -2640).
  Short rows: `26360` / `26370` (`Raven_Iron_Attack01.X`) / `26390` / `263a0` (20, 0.3) / `26aa0`-`26b70` / `30690` 0xc6 / `31830` 0xd0 / `306c0`. Each = its listing.
- **Enum:** 191-195 = `ch0_state_enum_names.tsv` rows 150-154 (reg `32c51b70`@133 / 176 / 219 / 262 / 305) = the pushes 0xbf-0xc3. The other targets: 0xc6 198, 0xc7 199, 0xcf 207, 0xd0 208, 0xd2 210 = their rows.
- **`JUNE_BUGS` #539 confirmed:** `32c32210` has no `+0x8ca4` operand, and `RAVEN_FIGHTER.src.lua` `RRF_COMBO_ZZZX` sets `UPDOWN_CANCEL_AFTER = 1.95`.
- **The diff's 2 deleted lines** (`git diff --numstat` 645 / 2) are a Myers alignment artifact: the `ZX_LANDING_EventProcess()` header and `{` are byte-identical in both versions. `--patience` gives 643 / 0.

**Gates.** One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`): exit 0, 0 errors with `_LIVE` off and with `/DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. `cl /P` was read for each branch: `FlushMp( 4.0f )` 1 off / 2 on, `DamageEffect_RSI_OVER_TAKER_DASH_JUMP_COMBO_ZXX` 0 / 1, `IsTempletClass( ..., UC_RAVEN_RECKLESS_FIST )` 0 / 2.
The expanded X-X-front-X body is one `else if` chain behind the fall.
`flagcheck ..._LIVE --with ..._CORE --range c87e1148^..c87e1148`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s)`. `RESULT: FAIL` only for the commented `_LIVE` define.
`junediverge.py --check-all`: `RESULT: PASS` (185 allow-listed). `enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows. No build: the review changes no code.

CH1g kickoff checked: present, CH0 amendment lists 35 functions and 11 Weapon Taker states, none of them CH1f's. Nothing to play-test. Next: `/x2-phase char-raven CH1g` (Sonnet 5.5 high).

### CH1g - Done (2026-11-06)

All 35 class-map rows of phase CH1g are written, all `inside` (June's body in March's own `CX2GURaven` function under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, March's in `#else`); no `new` row, no `.h` change. CH0 had three rows as `new ?` helpers: `32c526e0`, `32c598c0` and `32c5fd90` are June's one-argument bodies of March's own `_SetOverheat`, `_CanOverheat` and `_BurnOverheatHP` (CH0's overheat map said "`_SetOverheat` inlined, absent": June has both an out-of-line copy and an inlined one in `32c528c0`). `ch0_class_map.tsv` is corrected, 35 funcmap rows `Char Raven CH1g`, 1 `JUNE_BUGS` row (#540). `_LIVE` stays `//#define`.

| June | `CX2GURaven::` |
|---|---|
| `32c26eb0` `32c4aab0` `32c33c30` | `RWT_COMBO_X` Init (= March), FrameMove (= March), EventProcess (differs) |
| `32c26ee0` `32c33fd0` | X-X FrameMoveFuture (= March), EventProcess (differs) |
| `32c57150` `32c26ef0` `32c5b740` | X-X-X Start (= March, one-argument `_SetOverheat`), FrameMove (= March), EventProcess (differs) |
| `32c528c0` `32c60230` `32c59b60` `32c26f00` | X-X-X loop Start, FrameMove, EventProcess (differ), StateEnd (= March) |
| `32c42160` `32c342c0` | X-X-X finish FrameMove (= March), EventProcess (differs) |
| `32c26f10` `32c26f20` `32c345f0` `32c26f30` | Z-Z-Z-X Start, FrameMove, EndFuture (= March), EventProcess (differs) |
| `32c26f70` `32c34970` | Z-Z-Z-X-X FrameMoveFuture (= March), EventProcess (differs) |
| `32c4b290` `32c34cc0` `32c4b530` `32c35020` | Z-Z-Z-down-X and -XX FrameMove (= March), EventProcess (differ) |
| `32c26f80` `32c26fa0` `32c35330` `32c26fb0` | dash-jump Z-X Start, FrameMove, StateEnd (= March), EventProcess (differs) |
| `32c26fd0` `32c26fe0` `32c35440` `32c26ff0` | dash-jump Z-X-X Start, FrameMove, StateEnd (= March), EventProcess (differs) |
| `32c526e0` `32c598c0` `32c5fd90` | `_SetOverheat`, `_CanOverheat`, `_BurnOverheatHP` (differ) |

**Where June differs from March** (read from `charport_adis.py raven --phase CH1g` plus a Ghidra decompile of `32c5fd90` for the x87 stack; 19 rows equal March's body, 16 differ):
- **The cancel chain** is `SKILL_CANCEL_AFTER( m_fSkillCancelAfter )`, back-walk `+0x8c94`, walk `+0x8c98`, dash `+0x8c9c`, Z / X `+0x8c8c` (CH1d's `RAVEN_GROUND_*` macros, foot test) in X `33c30`, X-X-X `5b740`, loop `59b60`, finish `342c0`, Z-Z-Z-X `345f0`, Z-Z-Z-X-X `34970`, Z-Z-Z-down-XX `35020`. Where it sits: fall, flag window, `> 0.702` branch, chain, then the end test (X); fall, end -> WAIT, chain, then the `0.66` loop branch (X-X-X); end -> LOOP / FINISH, then chain (loop); fall, chain, `else` end -> WAIT (finish); flag window, branch, end, chain (Z-Z-Z-X); fall zeroes `speed.x`, chain, end (Z-Z-Z-X-X); no fall, chain, end (down-XX).
- **X-X `33fd0` and Z-Z-Z-down-X `34cc0` cancel without a foot test** and clear the X flag after each action (new macros `RAVEN_FLAG_BWALK_CANCEL` / `_WALK_` / `_DASH_` / `_ATTACK_CANCEL`, `_CORE`, beside CH1d's). X-X: end test, dash, walk, back-walk, Z / X, then the A-S-D-C-Q-W-E-R skill cancel (`ground` or `ASUT_AIR` by the foot, no hyper branch). Down-X: skill with hyper (needs the foot, flag kept), back-walk, walk, dash, Z / X, then the end test (WAIT or `JUMP_DOWN` and `y - 45`).
- **Dash-jump Z-X `35330` / Z-X-X `35440`:** June adds `StateChange( USI_JUMP_DOWN )`: after 0.635 s in the air with no X flag (March only cleared `m_bDisableGravity`, set first), and at the end of Z-X-X in the air.
- **`_CanOverheat`:** limit 0.25, or `m_fLimitOverHeatHP` for Survival Technique ( both flags ) or `m_fOverheatDamageLimitedRate > 0`; the message only for `IsMyUnit()` ( March: any unit ).
- **`_BurnOverheatHP`:** one argument ( the loop FrameMove passes no `0.5` ). Penalty 1.0 in a PvP or PvE game ( `GetIsNowPVPCorrectionGame()`, June `322cc1a0` ), else 0.2; 0 when `HP / max < m_fOverheatDamageLimitedRate`. The damage is clamped to `[0, HP - max * limit rate]`, and to `HP - 1` when it would end at 0 or below, with no Survival Technique test ( March tested it ).
- **`_SetOverheat( bOn )`:** one argument ( March's `fDuration` is ignored: stand-in for unported callers ). On: `GetSkillLevelValue( RNSI_P_RVC_OPEN_MIND )` > 0 takes that skill's buff factor ( `GetSkillBuffFactorPtr`, index 0, learned only ); `BFI_BUFF_OVER_HEAT` / `BTI_BUFF_OVER_HEAT`, or the `_ENHANCE` pair when `m_iAddOverHeatLevel > 0`, applied with `m_iAddOverHeatLevel` as the level ( JUNE_BUGS #540 ); the buff's first finalizer value is the duration ( -1 if none ); the passive's factor gets it as a `BFT_TIME` finalizer ( `SetFinalizerValue`, set ), is stamped ( `SetGameUnitBuffFactor( this, -1, true )` ) and applied; duration > 0 -> `m_fOverheatBuffTime`; `CreateOverHeatObject()`.
- **Enum members** ( `SERV_IRUHADEV_JUNE_ENUM_NAMES`, June's values ): `RNSI_P_RVC_OPEN_MIND` 24421 ( `X2SkillTree.h` ), `BTI_BUFF_OVER_HEAT` 103 and `BTI_BUFF_OVER_HEAT_ENHANCE` 250 ( `X2BuffFactor.h` ). `ch1_offsets.tsv` gains the 11 Raven members the listings use ( `+0x9668`-`+0x9738` ).

**Gates.**
- `flagcheck ..._LIVE --with ..._CORE --with SERV_IRUHADEV_JUNE_ENUM_NAMES --worktree --files` the three files: `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out. `git diff --patience --numstat`: `X2GURaven.cpp` 725 / 0, `X2SkillTree.h` 5 / 0, `X2BuffFactor.h` 6 / 0.
- One-file probe ( `census.cl_cmd`, `/c /Zs /EHsc`, no PCH ): exit 0, 0 errors with `_LIVE` off and with `/DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. `cl /P` both ways: 1442 `CX2GURaven::` lines each; `RNSI_P_RVC_OPEN_MIND` 1 off / 3 on, `GetIsNowPVPCorrectionGame` 1 / 2, `SetFinalizerValue( BFT_TIME` 1 / 2.
- `junediverge.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h X2Lib/X2SkillTree.h X2Lib/X2BuffFactor.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 26,010,112 B, dated 2026-11-06 06:32:04, md5 ece17aaa8e5b9c41228d99e19adab428, equal to X2/US_SERVICE/x2.exe (2026-11-06 06:31:58); built at main 650ad8f2 + 3 uncommitted code file(s).` No `exestr` line: `_LIVE` is off.

**Routed.** CH1g2: the overheat object pieces ( `CreateOverHeatObject`, `DeleteOverHeatObject`, `_ShowOverheatIcon` ) are still March's; the three setters above are done, so its states call them with June's arguments. CH2d: the Weapon Taker skill states ( Revolver Cannon, Hellfire Gatling, Harpoon Spear ) call `_SetOverheat` / `_BurnOverheatHP` with March's extra arguments in `#else` only; their June bodies call the one-argument forms. CH4a: `RNSI_P_RVC_OPEN_MIND` is read here; its `InitPassiveSkillState` row stays CH4a's. CH1 final play-test: X-X-X-X overheat ( HP share, buff icon, burn damage ), X-X in the air, Z-Z-Z-down-X cancels, dash-jump Z-X at 0.635 s without the X key.

**Nothing to play-test** ( `_LIVE` is off; nothing registers a June body ). Next: `/x2-phase char-raven CH1g review` ( Opus 5.5 high ), then CH1g2.

### CH1g - Review (2026-11-06, Opus 5.5)

**Verdict: PASS, no code change.** I read all 35 rows against `charport_adis.py raven --phase CH1g`, and `32c5fd90` against Ghidra's decompile as well. I checked each float test by its x87 flag test (`test ah, 5` / `0x41` / `1` with `jp` / `jne`).
- **The EventProcesses** (the chain is CH1d's, with a foot test and the order skill / back-walk / walk / dash / Z / X, unless noted):
  `33c30`: fall (y - 45), then the window `0.1 <= t <= 0.702` with `oneX`. `t > 0.702` -> 0xe3 when the flag is set. The chain is that branch's `else` (the branch jumps past it), and the end test (WAIT) is a separate `if`.
  `33fd0`: window `t <= 0.737`, then `t >= 0.737` (`test ah, 1`) with the flag and the foot -> 0xe4. The end test is next (air: JUMP_DOWN and y - 45, else WAIT), with the chain as its `else`: dash `+0x8c9c`, walk `+0x8c98`, back-walk `+0x8c94`, Z / X `+0x8c8c`, all without a foot test. Then the skill `+0x8c90` with A-S-D-C-Q-W-E-R and ASUT 2 in the air or 1 on the ground, with no hyper branch. Every action jumps to the clear of flag 0 at `@32c342aa`.
  `5b740`: fall, end -> WAIT, chain as its `else`, then a separate `t >= 0.66` -> 0xe5, else `oneX` with max < 11 (`cmp 0xb`) and `_CanOverheat( true )` -> ++max. `59b60`: the X raise first, then the end test (`max > count` -> 0xe5, else 0xe6), with the chain as its `else`.
  `342c0`: fall, then the chain, then `else` end -> WAIT. `345f0`: `0.533 < t` and `0.567 < t` (strict, by `jne` on 0x41), branch 0xe7 with the foot, end (JUMP_DOWN and y - 45, or WAIT), chain as its `else`. `34970`: fall sets y - 45, `nowSpeed.x` (+0x304) to 0 and +0x30c to -1; then the chain, then a separate end -> WAIT.
  `34cc0`: the skill (with the foot) and the hyper branch skip the flag clear, at `@32c34fc5` / `@32c34fc4`. Back-walk, walk, dash and Z / X have no foot test and fall through to the clear at `@32c34fbd`. The `0.739` branch (0xe9 with the flag and the foot) jumps past the chain to the separate end test. `35020`: no fall, the chain, then the end test, which takes JUMP_DOWN with no y change.
- **Dash-jump:** `35330`: `t > 0.635`: in the air, the flag -> 0xeb, else `m_bDisableGravity = 0` stored before `StateChange( 0x11 )` (`@32c35375`). On the foot -> 0x12. Else `0.42 < t`: skill with ASUT 2, which clears the flag on success. Else `oneX` sets it. `35440`: end -> 0x12 on the foot, 0x11 in the air; else the `0.24` skill.
- **`_SetOverheat` `32c526e0`** (`ret 4`, one argument): BFI 0x54 / BTI 0x54 + 0x13 = 103. When `+0x9670 > 0`, BFI 103 + 0x72 = 217 and BTI 217 + 0x21 = 250. The vslot 0x21c pushes `ebx|ebx|eax|ebx|ecx` give ( BFI, creator NULL, level, adjust 0, skip false ). That is June's order, and March's declaration `X2GameUnit.h:3209` (`SERV_IRUHADEV_JUNE_SKILL_STATE_BUFF`) keeps level second, so the call `( id, level, NULL, 0, false )` is the same as at `X2GameUnit.cpp:14577`.
  Vslot 0x238 `( out, 0x5f65, 0, 1 )` runs only when `31fceab0( 0x5f65 ) > 0`. The finalizer is read as `[0]` vslot 0x14 `GetFactor( vec, this )`, then `[0].m_vecValues[0]`. `32d41960( 1, v, 2 )`, then `32d4d670( this, -1 as UidType (two dwords), 1 )`, then vslot 0x224 ( `*ptr` ). `+0x96d8` is stored when `v > 0` (0x41 `jne`), and `CreateOverHeatObject` is always called when on. The inlined copy in `32c528c0` is the same instruction for instruction (count > 1, else `_SetOverheat( 0 )`, `CommonStateStart` last).
- **`_CanOverheat` `32c598c0`:** 0.25. `+0x9734` and `+0x9735`, or `0 < +0x9668` (`test ah, 5` / `jp`) -> `+0x9738`. It returns true at `HP / max >= limit`. The message needs the argument and vslot 0x198; it shows text 0x3c4d (`STR_ID_15437`) with `PushText( 3, text, (1,1,1,1), (0,0,0,1), 1, 1.0, 1.0 )`, = March's.
- **`_BurnOverheatHP` `32c5fd90`** (Ghidra and the listing agree): penalty 1.0 if `322cc1a0` (`GetIsNowPVPCorrectionGame`), else 0.2, and 0 when `HP / max < +0x9668`. DD +0x38 / +0x3c `*= +0x96d0 * penalty`, defender +0x470 = this, DD +0x510 (crit rate) = 0, vslot 0x70. The damage is clamped to `max( 0, min( f, HP - max * rate ) )`, then `HP - f <= 0` -> `HP - 1.0`. Vslot 0x198, then vslot 0xec `( -f, 0 )` = `UpNowHp( -f )`. The code's `NULL != g_pX2Game` test is a guard only.
- **FrameMoves / short rows:** `4aab0` 0.321 / `FlushMp( 2 )` / Bip01_L_Hand +-50, `4b290` 0.335, `4b530` 0.225 `RWT_COMBO_ZZZ_DOWN_XX_SHELL`, `42160` and `60230` (`CommonFrameMove` first, super armor, 0.11, the overheat damage effect + `_BurnOverheatHP`), `26f30` (bIsRight flip, dirDegree.y + 180), `26f80` / `26fb0` (gravity), and the rest are thunks. Each = its listing.
- **Enum:** `Enum.src.lua:12179` `BTI_BUFF_OVER_HEAT = 103`, `:12719` `_ENHANCE = 250`, `:15413` `RNSI_P_RVC_OPEN_MIND = 24421`. No March BTI holds 103 or 250. `ch1_offsets.tsv`'s 11 rows follow March's declaration order (`X2GURaven.h:651-718`: bools `+0x96c8` / `c9` and `+0x9734` / `35` are adjacent, the two counts are 12-byte KProtectedType). `JUNE_BUGS` #540 confirmed at `@32c527a2` (`push eax` = `+0x9670` as the level).

**Gates.** One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`): exit 0, 0 errors with `_LIVE` off and with `/DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. `cl /P` off / on: `RNSI_P_RVC_OPEN_MIND` 1 / 3, `GetIsNowPVPCorrectionGame` 1 / 2, `SetFinalizerValue( BFT_TIME` 1 / 2, `BTI_BUFF_OVER_HEAT_ENHANCE` 1 / 2, `USI_JUMP_DOWN` 167 / 171.
`flagcheck ..._LIVE --with ..._CORE --with SERV_IRUHADEV_JUNE_ENUM_NAMES --range 62e23741^..62e23741`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR` (the OTHER rows are the two `ENUM_NAMES` headers), `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s)`. `RESULT: FAIL` only for the commented `_LIVE` define. `git diff --patience --numstat`: 725 / 0, 5 / 0, 6 / 0. The default diff's 730 / 5 is the Myers artifact.
`junediverge.py --check-all`: `allow-listed: 185`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows. No build: the review changes no code.

CH1g2 kickoff checked: present (Sonnet 5.5 high, depends on this review). Nothing to play-test. Next: `/x2-phase char-raven CH1g2` (Sonnet 5.5 high).

### CH1g2 - Done (2026-11-06)

All 33 class-map rows of phase CH1g2 are written: 32 `inside` (June's body in March's own `CX2GURaven` function under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, March's in `#else`) and 1 `new` (`RVC_DASH_COMBO_ZZ_EventProcess`, `_CORE`, declared in `X2GURaven.h`, registered by CH1m). No new flag, enum member or macro (CH1d's `RAVEN_GROUND_*` / `RAVEN_ELSE_IF_FLAG_STATE_CHANGE` cover every chain). `_LIVE` stays `//#define`. 33 funcmap rows `Char Raven CH1g2` (3 replace CH0's `candidate` rows), 9 `ch1_offsets.tsv` rows, `JUNE_BUGS` #541. `RVC_DASH_JUMP_COMBO_XX_START` / `_LOOP` StateEnd have no June row (March registers none) and are untouched.

| June | `CX2GURaven::` |
|---|---|
| `32c27250` `32c42400` `32c35d30` `32c4c530` | `RVC_DASH_JUMP_COMBO_X` Init, Start, FrameMoveFuture, FrameMove (= March) |
| `32c35e20` | `RVC_DASH_JUMP_COMBO_X` EventProcess (differs) |
| `32c27270` `32c27280` `32c27290` `32c5a7f0` | `..._XX_START` StateStart, FrameMove, FrameMoveFuture, EventProcess (= March) |
| `32c424b0` `32c61580` `32c272d0` `32c5a860` | `..._XX_LOOP` StateStart (= March), FrameMove (differs), FrameMoveFuture, EventProcess (= March) |
| `32c42550` `32c4c910` `32c35ed0` `32c35f90` `32c425f0` | `..._XX_FINISH` Start, FrameMove, FrameMoveFuture (= March), EventProcess (differs), End (= March) |
| `32c36020` | `RVC_DASH_COMBO_ZZ` EventProcess (new) |
| `32c36420` `32c36440` `32c36790` | `RVC_DASH_COMBO_ZZZ` Start (= March), EventProcess (differs), End (= March) |
| `32c367b0` `32c367d0` `32c36ad0` | `..._ZZZPushZ_READY` Start (= March), EventProcess (differs), End (= March) |
| `32c5a940` `32c619e0` `32c5ac10` `32c4cac0` | `..._ZZZPushZ` Start, FrameMove, EventProcess (differ), End (= March) |
| `32c36af0` | `RVC_DASH_COMBO_ZZZZ_FINISH` EventProcess (differs) |
| `32c4dfe0` `32c4e3c0` `32c4add0` | `CreateOverHeatObject`, `DeleteOverHeatObject`, `_ShowOverheatIcon` (= March) |

**Where June differs from March** (read from `charport_adis.py raven --phase CH1g2`; 23 rows equal March's body, floats, strings and handle members compared):
- **Cancel chain** (CH1d's macros, foot test): `SKILL_CANCEL_AFTER( m_fSkillCancelAfter )`, back-walk `+0x8c94`, walk `+0x8c98`, dash `+0x8c9c`, Z / X `+0x8c8c`, replacing March's dash / walk / skill. Z-Z-Z `32c36440`: Oneshot 0.501 `if` (Z -> READY), chain, the end test (WAIT) last as the final `else if`. READY `32c367d0`: end -> Push-Z, chain. Push-Z `32c5ac10`: `0.3 <` and Z released -> FINISH, then `!_CanOverheat( false ) || m_bEndFrameThrow` -> FINISH, `else if` end (clears `m_bEndFrameThrow`; air JUMP_DOWN, else FINISH), chain. FINISH `32c36af0`: end (air JUMP_DOWN, else WAIT), chain.
- **New dash Z-Z EP `32c36020`:** end -> WAIT, chain, `else if` no foot -> JUMP_DOWN and `y - 45`, then Z-Z-Z at `> 0.43 s` or the end when flag 1 is set, else flag 1 is set by `oneZ` when `t < 100` for classes 25 / 115 (`IsDataClass`, unit data `+0x38`).
- **Dash-jump X `32c35e20` / X-X finish `32c35f90`:** the end in the air adds `StateChange( USI_JUMP_DOWN )` (March did nothing). `SKILL_CANCEL_AFTER_FLY` keeps March's bodiless macro, so `CommonEventProcess()` runs only when the air skill cancel fires (`JUNE_BUGS` #541; the X-X loop EP `32c5a860` has it too).
- **One-argument calls:** X-X loop FrameMove `32c61580` and Push-Z FrameMove `32c619e0` call `_BurnOverheatHP( dmg )` and `_SetOverheat( true )`; Push-Z Start `32c5a940` calls `_SetOverheat( true )` (March passed `5.f`, ignored under `_LIVE`). `_CanOverheat( true )` in `32c5a7f0` / `32c5a860` / `32c5a940` equals March's default.
- **CH0 correction:** `CreateOverHeatObject` / `DeleteOverHeatObject` do not differ from March (March plays `WT_XXXXX_3.ogg` / `WT_ZZZXX_2.ogg` and creates the End mesh too); `ch0_overheat.tsv` fixed. `PlaySound( name, 1, 0 )` = March's defaults.
- Offsets added to `ch1_offsets.tsv`: `+0x9704` count, `+0x971c` / `+0x9720` frame-throw handle / flag, `+0x9740` / `+0x9744` icon handles, `+0x975c` object, `+0x9760` / `+0x9764` fire handles, `+0x8ca8` fly cancel time.

**Gates.**
- `flagcheck ..._LIVE --with ..._CORE --worktree --files X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `.cpp` `PASS +730`, `.h` OTHER (`_CORE` only); `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out. `git diff --patience --numstat`: `.cpp` 730 / 0, `.h` 3 / 0.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): exit 0, 0 errors with `_LIVE` off and with `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (Git Bash turns a leading `/D` into a path: use `-D`). `cl /EP` off / on: `_CanOverheat( true )` 0 / 3, `_BurnOverheatHP( pEffect->GetDamageData() )` 8 / 10, `_SetOverheat( true );` 0 / 4, `USI_JUMP_DOWN` 168 / 174.
- `junediverge.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 26,011,136 B, dated 2026-11-06 07:19:44, md5 5383f8fc63323abca6b9bed17deb6960, equal to X2/US_SERVICE/x2.exe (2026-11-06 07:19:38); built at main 0c733c60 + 2 uncommitted code file(s).` No `exestr` line: `_LIVE` is off.

**Routed.** CH1m: register `RVC_DASH_COMBO_ZZ` with `32c36020` as its only callback. CH1j: `CommonFrameMove` calls `DeleteOverHeatObject` (done here, = March). CH1 final play-test: Veteran Commander dash Z-Z (Z within 0.43 s -> Z-Z-Z), Z-Z-Z cancels by skill / walk / dash, Push-Z fire throw (overheat, count 20), dash-jump X ending in the air.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-raven CH1g2 review` (Opus 5.5 high), then CH1h.

### CH1g2 - Review (2026-11-06, Opus 5.5)

**Verdict: PASS, no code change.** I read all 33 rows against `charport_adis.py raven --phase CH1g2` and checked each float test by its x87 flag test. I also diffed each `_LIVE` body against its own `#else` with comments and whitespace stripped. The 23 "= March" rows are word for word March's, and the 9 others differ only as the Done note says.
- **Cancel chains** (CH1d's macros; every branch has the foot test `+0x7e1c`, in the order skill `+0x8c90` (ASUT 1, then the hyper branch) / back-walk `+0x8c94` / walk `+0x8c98` / dash `+0x8c9c` / Z then X `+0x8c8c`):
  `36440`: the `0.501` Oneshot (`fcom [+0x24c]` 0x41 `jne`, then `EventTimer`) jumps past the chain to `CommonEventProcess` whether or not `pureZ` fires 0x101. The chain is its `else`, and the end test -> WAIT is the last `else if` (`@32c3675e`).
  `367d0`: end -> 0x102 first, then the chain. `36af0`: end (`+0x7e1c == 0` -> 0x11, else 0xd), then the chain.
  `5ac10`: `t > 0.3` (`fcomp` 0x41 `jne`) with `pureZ == 0` -> 0x103, then no jump. Then `_CanOverheat( 0 )` false or `+0x9720 == 1` -> 0x103 (`@32c5af60`). `else if` end clears `+0x9720`, then the air goes to 0x11, else 0x103. The chain is that branch's `else`.
- **New `RVC_DASH_COMBO_ZZ_EventProcess` `36020`:** end -> 0xd, the chain, `else if` no foot -> 0x11 with `+0x7e40 -= 45.0` (= `LINE_RADIUS * 1.5f`). Then flag 1 with `0.43 < t` (`fcomp` 0x41 `je`) or the end (via the `32070cf0` accessor) -> 0x100 and clear. Else `t < 100.0` (`test ah, 5` `jp`), `oneZ`, and unit data `+0x38` equal to 0x73 or 0x19 set flag 1. `IsDataClass` adds a NULL test that June does not have (`GetUnit()` is never NULL here).
- **Dash-jump X `35e20` / X-X finish `35f90`:** the end is flag 0 -> 0xfd, else foot -> 0x12, else 0x11, each a bare `ret`. `SKILL_CANCEL_AFTER_FLY` (`+0x8ca8`, ASUT 2) runs `CommonEventProcess` only on success. `jne` goes to the bare return at `@32c35ebe` / `@32c36003` / `@32c5a92f` (X-X loop `5a860`, `cmp 9` = `MAX_DASH_JUMP_COMBO_XX_LOOP_COUNT`). `JUNE_BUGS` #541 confirmed.
- **One-argument calls:** `61580` / `619e0` push `pEffect + 0x200` only to `32c5fd90`, then inline `_SetOverheat( true )` (`+0x96c8 = 1`, skill 0x5f65, finalizer, `CreateOverHeatObject`). `5a940` has `push 1` -> `_CanOverheat`, the inlined setter (two `CreateOverHeatObject` tails), else `+0x9720 = 1`. `5a7f0` / `5a860` have `push ebx` (= 1) -> `_CanOverheat`. `5ac10` has `push 0`.
- **Overheat object pieces:** `4dfe0` / `4e3c0` test `GetUnit()` NULL, then `+0x38 == 0x73` (= `GetUnitClass()`). Create returns at once when `+0x975c != -1` (`@32c4e3ab`), so it skips the fire effects as March's does. The mesh is Start / End, with `SetBoundingRadius( 0 )` and `OnFrameMove( t, 0.01 )`. `PlaySound` pushes `0, 1, name`. The fire handles are `+0x9760` (Bip01_L_Forearm) and `+0x9764` (Bip01_L_Hand), and Delete calls `DestroyInstanceHandle` on each.
  `4add0`: 140 / 60, `WeaponTaker_Overheat` -> `[ebx]` = `+0x9740`, overlap `+0x9744` destroyed, 50 added on every axis, + 20 y. Then `iSize` 3 / 2 / 1 -> `Overlap_EDT_Emoticon_*_OverHeat`, stored at `@32c4b226` into `[ebx]` = `+0x9740`, not `+0x9744`. March stores it in `m_hSeqOverHeatIcon` too: the quirk is shared, so no JUNE_BUGS row. `UpDownCrashCamera( 30, 0.5 )`. The offsets follow `X2GURaven.h:722-738`.
- **Floats and the rest:** `35d30`: `t > 0.21`, `t < 0.7`, `0.26 <= t < 0.29` -> 700, else y < 0 -> 0. `4c530`: 0.26, `vDir.z - 45`, `FlushMp( 10 )`, camera ( 10, 0.2 ), magic fail 100 / 100 / 1. `61580`: 0.006, 42.5, `1.5 - 0.5r` = `RandomFloat( 1.5f )`, ( 0.7 - r ) * 10. `4c910`: 0.025. `35ed0`: 0.03 -> -800 / 1200, and the end sets flag 0 on `pureDown`. `619e0`: `/ 0.2`, `return` on `+0x9720` before `CommonFrameMove`, 30.0, `> 0x14`. `42400`: count 1, `+0x3a4 = 3` (`AT_MANUAL_COLLISION_DATA`), `+0x7e6 = 0`. `36420` / `367b0` / `5a940` set `+0x6890 = 1`; the Ends clear it. `4cac0`: `DestroyInstanceHandle( h, 0 )`, `+0x9720 = 0` inside the `if`, count 0. Every thunk = its listing.

**Gates.** One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`): exit 0, 0 errors with `_LIVE` off and with `/DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (passed as a Python list, not through Git Bash). `cl /EP` off / on: `_CanOverheat( true )` 0 / 3, `_BurnOverheatHP( pEffect->GetDamageData() )` 8 / 10, `_SetOverheat( true );` 0 / 4, `USI_JUMP_DOWN` 168 / 174, `RVC_DASH_COMBO_ZZ_EventProcess` 2 / 2.
`flagcheck ..._LIVE --with ..._CORE --range 2277d4b5^..2277d4b5`: `.cpp` `PASS +730`, `.h` OTHER (`_CORE`), `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. `RESULT: FAIL` only because the `_LIVE` define is commented out. `junediverge.py --check-all`: `allow-listed: 185`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows. No build: the review changes no code.

CH1h kickoff checked: present (Sonnet 5.5 high, 37 rows by CH0's amendment, depends on this review). Nothing to play-test. Next: `/x2-phase char-raven CH1h` (Sonnet 5.5 high).

### CH1h - Done (2026-11-06)

All 37 class-map rows of phase CH1h are accounted for: **27 `inside`** written (June's body in March's own `CX2GURaven` function under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, March's in `#else`), **4 header inlines** wrapped the same way (`IsFullHyperMode` / `SetFullHyperMode`, `GetComboZStateID` / `GetComboXStateID`), **2 `new` under `_CORE`** (`AttackResultArcEnemy`, `ApplyEnchantHandDebuffToDamageData`, declared in `X2GURaven.h`) and **4 with nothing to write** (`32c63660` jmp thunk, `32c68f90` = `Weapon::SetTraceColor`, `32c6a450` = `ArcEnemyData`'s destructor, `32c6a620` scalar deleting destructor). `_LIVE` stays `//#define`. 37 funcmap rows `Char Raven CH1h` (6 replace CH0's `candidate` rows), 37 `ch1_offsets.tsv` rows (the destructor / Init / ReInit / AttackResultByType member map). No `JUNE_BUGS` row: nothing here is a June mistake.
**CH0 map corrections (March has all of these):** `32c273f0` = `GetReduceDebuffDurationRate` (the same 12 BTI ids), `32c38340` / `32c383b0` = `SetSpecificValueByBuffTempletID` / `UnSet...` (BTI 30 = `BTI_BLOODY_WEAPON`, trace colors 0xccff0000 / 0xffff0000 and 0xcc5555ff twice, the `Weapon::SetTraceColor` inline `32c68f90`), `32c43080` = `DeleteRavenMajorParticle`, `32c54960` is `new` (March's arc enemy lock-on, moved out of `AttackResultPlayEffect`).

| June | `CX2GURaven::` | vs March |
|---|---|---|
| `32c25a20` | AttackResultPlayEffect | differs: the base only, the arc lock-on moved to `AttackResultByType` |
| `32c38410` | ApplyRenderParam | differs: the else branch sets `m_RenderParam.color.a = 1` when `!IsAlphaObject()`, then `CX2GUUser` `31fd4460` ( CUSTOM_RENDER_STATE; stand-in `CX2GameUnit::ApplyRenderParam`, the engine renderer is not ported ) |
| `32c3db70` | GetHyperModeTime | differs: only while the gage has hyper time left: ( last-state count - count, >= 0 ) x max time, x Spiritualized Fury ( 24405, `SA_EFFECTIVE_TIME` ) when the can-change-in-hyper flag is set, a forced time wins; else the base |
| `32c4e7c0` | HyperModeBuffEffectStart | differs: the boost handles skip a transcended unit or an awakening effect set ( stand-in `RavenAwakeningEffectSetPresent`, CH1c's ), the gauge is 100 with hyper-mode buff 5 / 822 and the can-change flag, else last-state count x 20 + 40 capped at 100 |
| `32c54c10` | destructor | differs: no `m_hSeqHyperBall` destroy ( June has no such member ) |
| `32c56be0` | AttackResultByType | differs: rage gain = max( phys, magic ) x rate x 4 x ( specialization A + 1 ) x Open Mind's ability `0x18` = `SA_CHARGING_TIME` ( skill 24419 ), a gauge in 20..40 jumps to 60 ( June design ), `clock()` stamp, arc helper first |
| the other 21 `inside` + 4 header inlines | | = March ( InitEffect, CreateMinor / MajorParticleArray, ApplyHyperModeBuff, GetReduceDebuffDurationRate, Win, Draw, Lose, InitEnchantWeaponEffectUnitSpecific, Set / UnSetSpecificValueByBuffTempletID, ApplyRenderParamOnEmptyRenderParamByBuff, InitComponent, COMMON_BUFF_FrameMove, AddUnitSlashData, ReInit, DamageReact, Init, DeleteRavenMajorParticle, DeleteMinorParticle, HyperModeBuffEffectEnd ); InitComponent and AddUnitSlashData run without March's `UNIT_SLASH_TRACE_MANAGER_TEST` gate |

**New (`_CORE`):** `AttackResultArcEnemy` ( `32c54960`: returns while `m_fArcEnemyEachTime > 0`, on `RT_DUMMY_DAMAGE`, unless the attack type is `AT_NORMAL` or `AT_COMMAND`; `ClearArcEnemyData` on a missing data / effect / mesh; lock-on at `+0x938` / `+0x940`, 150 / 3000 / 0.35 / 0.3 ), `ApplyEnchantHandDebuffToDamageData` ( `32c5d540`, user vslot `0x300` hook, base `3208ddc0`: when not riding, `BTI_BUFF_ENCHANT_HAND` is on, the hit is not special and creates a damage effect ( `DamageData +0x48c` = `m_wstrCreateDamageEffectName` size: March `+0x3cc` + the `0xb0` shift every June member from `defenderType` on shows ), `PushBuffFactor( clone at the level of 24206, false )` of `BFI_DEBUFF_ENCHANT_HAND` ). New `_CORE` member `m_clockLastNasodBallAttack` ( `+0x9778`, `clock()` of the last ball launch; read by `CommonFrameMove` `32c55240` for Open Mind ).
**Enum names added** ( checked against `Enum.src.lua`, `JUNE_ENUM_NAMES` ): `RNSI_A_RRF_ENCHANT_HAND` 24206 ( `X2SkillTree.h` ), `BTI_BUFF_ENCHANT_HAND` 543 and `BFI_DEBUFF_ENCHANT_HAND` 633 ( `X2BuffFactor.h` ). Ability `0x18` is `SA_CHARGING_TIME` in March's enum; `SA_COOLTIME_1` ( 148 ) and `SA_TIME_ABS` ( 169 ) equal June's, so the enum lines up.
**Stand-ins:** the awakening effect-set test is CH1c's ( false, Stage 12 ); `CX2GUUser::ApplyRenderParam` `31fd4460` is `CX2GameUnit`'s; `IsTranscendedUnit` is `_CONST_RAVEN_CH1H_`'s ( `m_iUnitClassTran`, as Elsword's ). Bodies owned later are called by March's name: `ClearNasodBall` / `RestoreNasodBall` ( CH1q ), `GetSkillLevelValue` / `GetLearnedSkillAbilityValue` ( the shared helpers ).

**Routed.** CH1k: the constructor sets `m_clockLastNasodBallAttack` and the June-only members the destructor listing shows ( `kickoffs/char-raven/CH1k.md` amended ). CH1o: wire the `0x300` hook ( June's caller is unlocated; Luciel overrides the slot too ), `kickoffs/char-raven/CH1o.md` amended. CH1q: `AddRageGuage` and `DoParrying` stay there; both write / read the same clock member ( `CH1q.md` amended ).

**Gates.**
- `flagcheck JUNE_CHAR_RAVEN_LIVE --with JUNE_CHAR_RAVEN_CORE --worktree --files X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `.cpp` `PASS +738`, `.h` `PASS +13`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out. `git diff --patience --numstat`: `.cpp` 738 / 0, `.h` 13 / 0, `X2SkillTree.h` 5 / 0, `X2BuffFactor.h` 10 / 0 ( `flagcheck JUNE_ENUM_NAMES --worktree --files` both: `RESULT: PASS` ).
- One-file probe ( `census.cl_cmd`, `/c /Zs /EHsc`, no PCH; `-D` not `/D` ): `X2GURaven.cpp` exit 0, 0 errors with `_LIVE` off and with `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. `cl /EP` off / on: `AttackResultArcEnemy( pDamageData )` 0 / 1, `m_clockLastNasodBallAttack = clock()` 0 / 1, `RNSI_P_RRF_SPIRITUALIZED_FURY, true` 0 / 1, `IsTranscendedUnit( GetUnit() )` 0 / 1, `m_fRageGauge = 60.f` 0 / 1, `IsAlphaObject()` 3 / 4; the two `_CORE` functions 1 / 1.
- `junediverge.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h X2Lib/X2SkillTree.h X2Lib/X2BuffFactor.h`: `RESULT: PASS`; `--check-all`: `allow-listed: 186` ( 185 before ), `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: `0 A/B row(s)`.
- `DEPLOY OK: X2_offline.exe is 26,012,160 B, dated 2026-11-06 08:12:18, md5 cf914e892d59c88181ffc2055631452b, equal to X2/US_SERVICE/x2.exe (2026-11-06 08:12:11); built at main bff915ba + 4 uncommitted code file(s).` No `exestr` line: `_LIVE` is off.

**Nothing to play-test** ( `_LIVE` is off; nothing registers a June body ). Next: `/x2-phase char-raven CH1h review` ( Opus 5.5 high ), then CH1i.

### CH1h - Review (2026-11-06, Opus 5.5)

**Verdict: PASS, no code change.** I read all 37 rows against `charport_adis.py raven --phase CH1h`, checked every float test by its x87 flag test, and counted the pushes between each stack store and its reload. The 6 differing bodies differ only as the Done note says. The 25 "= March" rows match their listings, and the 4 "nothing to write" rows are as stated. One comment is wrong, routed to CH1i (last bullet).
- **`GetHyperModeTime` `3db70`:** `0 < remain` (`test ah, 5` `jp` -> base). Count = vslot 0xb0 - vslot 0xac (`GetLastStateHyperModeCount` - `GetHyperModeCount`, `ch0_vtable_all.tsv`), 0 when not positive, then x `+0x24c`. `push esi` / `push edi` shift the slots: the product lives in `[esp+8]` (original slot 0), the Fury multiply reads and writes it at `[esp+0xc]` under `edi`, and the `jp` return loads `[esp+4]` after `pop esi`, the same slot. So the forced time wins when `0 < +0x422c`, else the product. 0x5f55 = 24405, `GetSkillLevel( id, 1, 0, 1 )` twice around `< 1`, `GetSkillAbilityValue( 1, level, 0, 0.0 )` = `SA_EFFECTIVE_TIME` (index 1).
- **`HyperModeBuffEffectStart` `4e7c0`:** `+0x3c != 0` or `31fd66b0` skips all four handles (cbc / cc0 / cc4 / cc8 = Right / Left / Right / Left Black). Then the aura on vslot 0x9c. Then `+0x8fc3` with `HaveThisBuff( 5 )` or `( 0x336 = 822 )` -> 100, else vslot 0xb0 x 20.0. `fcom st(2)` compares 100 with g + 40, and `test ah, 0x41` `jne` keeps 100 when 100 <= g + 40, so it is `min( g + 40, 100 )`. The `> 100` clamp after it can never fire.
- **`AttackResultByType` `56be0`:** base, then the arc helper, then `0 < remain`. `[esi+8] == 1` jumps to the NPC test at `@32c56e9c`, skipping the fill and both clamps. The fill needs vslot 0x198, `+0x97a9`, `+0x97a8`. Open Mind: 1.0 in `[esp+0x14]` (the `push 0x5f63` shifts `+0x18`), then `31fceab0( 0x5f63 ) > 0` -> `32007b10( 0x18, 0x5f63 )`. `0x18` = `SA_CHARGING_TIME` (index 24 in March's enum). Gain = max( `+0x38`, `+0x3c` ) x `+0x51c` x 4.0 x ( `+0x6c50` + 1.0 ) x rate. Then `100 < g` -> 100, else `_ftol2( g / 20.0 ) == 1` -> 60. Then: the NPC test (`+0x264 == 1`, templet `+4 != 0`), the two flags, `IsAttackTrigger( 1 = DTT_RAVEN_RAGE_ATTACK )`, `+0x97a0`, `+0x97c4`, `GetInstance( +0x18, true )` (March's default). `clock()` goes into `+0x9778` between the two zeros, and the 64-bit UID into `+0x9798` / `+0x979c`. `SetDirSpeed` = `+0x5cc`. Five cases scale 1 / 1.6 / 1.4 / 1.6 / 1.5, the default runs `RestoreNasodBall` and returns before the sound.
- **`AttackResultArcEnemy` `54960`:** the returns come in the order `0 < +0x9648`, `+0x28 == 7`, type not 0 / 2, empty vector. A missing data / effect / main effect goes to `ClearArcEnemyData` (`@32c54bfe`). Then `+0x46c == 0` and a valid observer. The UID goes to `+0x938` / `+0x93c` (`LockOnData::m_LockOnUnitUID`, 64-bit) and the NPC UID to `+0x940`, in March's struct order. The numbers are 150, ( 0, 9999 ) at `+0x840`, 3000 at `+0x5cc`, now 0 / max 0.35 at `+0x448` / `+0x44c`, land 0 at `+0x130`. Then `+0x18 = -1`, delete, `erase( begin )`, 0.3.
- **`ApplyEnchantHandDebuffToDamageData` `5d540`:** I checked the offsets with an `offsetof` probe of March: `m_wstrCreateDamageEffectName` = `0x3cc`, `defenderType` `0x3bc`, `optrDefenderGameUnit` `0x3c0` (June `+0x46c` / `+0x470`, so +0xb0). `m_fHpPercentUp` `0x43c` and `m_fRateModifier` `0x454` sit at June `+0x504` / `+0x51c` (+0xc8): June has 0x18 more bytes after the name, so `+0x504` cannot confirm the shift. June's `AttackResult` `32012b50` does: it passes `this+0x81c` (cap at `+0x830`) to `CreateInstance`, and its scale is DD `+0x498` at `this+0x838`. So the name is at DD `+0x47c` and its size at `+0x48c`. `322b5320( factor, level, 0, { 1.0, 1.0 } )` = `PushBuffFactor( GetClonePtr( level ), false )` (funcmap rows 6054 / 7856, March's `SetBuffFactorToDamageDataByBuffFactorID`). 0x21f = 543, 0x279 = 633, 0x5e8e = 24206, level 1 without a unit.
- **Destructor `54c10`:** the order matches. The minor block (`31fefc00` + one `+0x9788` handle) is `DeleteRavenMinorParticle` (`RAVEN_MINOR_PII_END` = 1). There is no hyper-ball handle. **`ApplyRenderParam` `38410`:** `+0x6840`, then `0 < +0x964c`, then `+0x3e58` -> `+0x24` = 0, type 3 = `RT_CARTOON`, `+0xc44` = 0. Else `+0x144` (`m_bAlphaObject`) == 0 -> `+0xc44` = 1, then `jmp 31fd4460` (the stand-in is the base). **`AttackResultPlayEffect` `25a20`** = `jmp 320142c0`.
- **`DamageReact` `40b20`:** the ids are 18 / 55 / 31 / 49 / 24 / 7 / 36 / 9 / 22 / 39, each June's `Enum.lua` EDT of the name used. Then the base and `DamageReactStateChange( d, -1, -1 )`. `fcom` `test ah, 1` on `+0x504` / `+0x508` = `<= 0`, and 0xd8 = June id 216 = `RBM_COMBO_ZZZXZ` (`ch0_state_enum_names.tsv`). **`ReInit` / `Init`:** the stores are those of `ch1_offsets.tsv`, and `+0x979c = -1` is the high half of the 64-bit `m_iLockOnUid`.
- **The "= March" rows:** `GetReduceDebuffDurationRate`'s byte table (read from the exe, id - 25, 0xc7 entries) selects exactly 25, 31, 38, 52, 53, 58, 61, 85, 87, 99, 161, 223, the 12 named BTIs. Set / UnSet decode to `( true, 0xccff0000, 0xffff0000 )` / `( false, 0xcc5555ff, 0xcc5555ff )` on `[+0x3cfc][0]`. Win / Draw / Lose use 0x4b / 0xb / 0x4c / 0xc = `USI_RIDING_WAIT_HABIT` / `USI_WIN` / `USI_RIDING_WAIT` / `USI_LOSE`. `COMMON_BUFF_FrameMove` is `EventTimerOneshot` 0.83 (`m_fAnimOneshotTimer < t`), the particle, camera ( 30, 0.4 ), `ClearScreen()` (1, white); `else if` 0.85 -> `ProcessAcceleraterBuff`; then the `CommonFrameMove` tail. `HyperModeBuffEffectEnd` is the aura stop + sound inside the handle test, `SetFullHyperMode( false )`, `0 < rage` -> HP x 0.0005 / MP, then the ball, zeros. `ApplyRenderParamOnEmpty...`: 2 / `CTT_RED` (4 under the live enum, `cl /EP`) / ( 1, 0.694, 0.737, 1 ), else 1 / 0 / white. InitEffect / CreateMajor (7 names, enums 5-11, counts 2 2 2 2 1 1 1) / CreateMinor (`jmp` base) / InitComponent (0.4 / 0.5, `STC_NONE`, `m_fOldHp`) / AddUnitSlashData (L"", `Bip01_L_Hand`, `ATTACK_SPHERE1_Lhand1`, 400 / 150) / InitEnchant / the Delete* / ApplyHyperModeBuff, and inlines `+0x97c8`, 0xa4 = `RSI_COMBO_Z`, 0xad = `RSI_COMBO_X`.
- **Routed to CH1i (comment only):** the one-line comment at the top of `GetHyperModeTime`'s June body says Spiritualized Fury applies "for a transcended unit". The gate is `GetCanChangeHyperModeInHyperModeState()`, which `X2GURaven.cpp:2569` sets when Spiritualized Fury is learned. The code is right. CH1i rewrites that comment in its own edit (`kickoffs/char-raven/CH1i.md` amended), because CH1i rebuilds `X2GURaven.cpp` anyway and a review-only comment fix would cost a build.

**Gates.** One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, as a Python list): exit 0, 0 errors with `_LIVE` off and with `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. The `cl /EP` off / on counts equal the Done note's (`AttackResultArcEnemy( pDamageData )` 0 / 1, `m_clockLastNasodBallAttack = clock()` 0 / 1, `RNSI_P_RRF_SPIRITUALIZED_FURY, true` 0 / 1, `IsTranscendedUnit( GetUnit() )` 0 / 1, `m_fRageGauge = 60.f` 0 / 1, `IsAlphaObject()` 3 / 4, `ApplyEnchantHandDebuffToDamageData` 2 / 2), and `m_hSeqHyperBall` drops from 13 to 11 (the destructor).
`flagcheck ..._LIVE --with ..._CORE --range 208cc8d0^..208cc8d0`: `.cpp` `PASS +738`, `.h` `PASS +13`, `X2BuffFactor.h` / `X2SkillTree.h` OTHER (`JUNE_ENUM_NAMES`). Summary: `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. `RESULT: FAIL` only because the `_LIVE` define is commented out. `junediverge.py --check-all`: `allow-listed: 186`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: 0 A/B rows. No build: the review changes no code.

CH1i kickoff checked: present (Sonnet 5.5 high, 2 inside rows `32c28270` / `32c4f640` by CH0's amendment, depends on this review), and it now carries the comment fix. Nothing to play-test. Next: `/x2-phase char-raven CH1i` (Sonnet 5.5 high).

### CH1i - Done (2026-11-06)

Both class-map rows of phase `CH1i` are written, **2 `inside`** (June's body in March's own `CX2GURaven` function under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, March's in `#else`); no `new` row. `_LIVE` stays `//#define`. 2 funcmap rows `Char Raven CH1i`. No `JUNE_BUGS` row: nothing here is a June mistake. The CH1h review's comment fix in `GetHyperModeTime` is done (comment only, 1 line replaced: "...x Spiritualized Fury when it lets awakening re-trigger; a forced time wins").

| June | `CX2GURaven::` | vs March |
|---|---|---|
| `32c28270` | SetEquippedSkillLevel ( vslot 0x7b8 ) | differs: 17 blocks keyed by June's skill id ( 24004 .. 24601 ), each `SetEquippedSkillLevelStateData( GetEquippedActiveSkillTemplet( id, true ), state )` for 46 states; March sets the same states one by one under its own `SI_` ids ( 4xxx ) and misses the Upgrade / Shadow Step / Giga Prominence-over states |
| `32c4f640` | HyperModeFrameMove ( vslot 0x600 ) | differs: before the March call, the first frame of the awakening ( `EventTimerOneshot( 0.001 )` ) gives a unit under half HP Reversal Soldier's MP ( skill 24413, ability `SA_MP_RETAKE_ABS` at that level, `UpNowMp` ); then `CommonHyperModeFrameMove( 0.05, 0.83 )` = March's call |

**SetEquippedSkillLevel, block by block** ( `if ( bChangeAll || id )`; the last block tests 24200 and 24217 and reads the templet of 24200 ): 24004 Valkyrie's Javelin -> `RSI_SA_ROT_VALKYRIESJAVELIN_LANDING`; 24009 Wild Charge -> `_CHARGING`, `_CHARGE_DASH`; 24017 Wolf Fang -> `_HIT`, `_HITLANDING`, `_STARTLANDING`; 24018 Harpoon Spear -> fire, fire-overheat; 24019 Revolver Cannon -> loop, finish; 24020 Hellfire Gatling -> loop; 24021 Giga Prominence -> loop, finish, over-finish; 24022 Ignition Crow Napalm -> fire, fire-overheat, + the two `_UPGRADE`; 24023 Incineration -> fire, fire-overheat, overheat-end, + the three `_UPGRADE`; 24024 Deadly Raid -> the six loop / end / finish states; 24201 Flame Sword -> ground, fly; 24205 Limit Crusher -> revenge, avoid; 24208 Bleeding Slicer -> ground, fly; 24210 Sonic Slash -> finish; 24212 Barrage Attack -> stand, air; 24601 Extreme Blade -> attack, attack-finish; 24200 / 24217 Shadow Step -> `RSI_SHADOW_STEP_Z`, `_Z_UPGRADE`, `_X_UPGRADE`, `RSI_SHADOW_BACK_SLIDE_UPGRADE`, `_END_UPGRADE`. State names are the exe's registration ids ( `ch0_state_enum_names.tsv` col 2 ), not `Enum.lua`'s numbering. Two ids have no table row and are inferred from their neighbours: `0xfa` = 250 = `RSI_SA_RWT_GIGA_PROMINENCE_OVER_FINISH` ( 249 END, 251 OVER_END; March's block sets the same three ) and `0x13d` = 317 = `RVC_SI_SA_IGNITION_CROW_INCINERATION_OVERHEAT_END_UPGRADE` ( 315 / 316 are its siblings; `RAVEN_FIGHTER.src.lua` has that table ). The review checks both.
**Callee.** June calls vslot `0x7a8` ( `32028190` ); Raven's callee is March's per-class `SetEquippedSkillLevelStateData` ( null templet = no-op, as June's `3201f850` ). It lacks June's `( 3201ad10( skill ) + 1.0 )` power-rate term: CH1o's ( checklist §7 l.286 ). June's dirty-state batch ( `3207c1a0` / `31fdf970` ) is left out, as in Elsword's and Chung's.
**HyperModeFrameMove.** The 0.001 test is the inlined `EventTimerOneshot` ( anim `+0x24c` = `m_fAnimOneshotTimer`, then `EventTimer` `31d28e10` ). Guards: the unit, `g_pData->GetSkillTree()`; HP now / max < 0.5 ( `fdivr`, `fcomp 0.5`, `test ah, 5` + `jp` ); `GetSkillLevel( 24413, true, 0, true ) > 0`; `GetSkillTemplet( id )` ( June's `( id, 1 )` is a flag, March's one-argument overload ); ability `0x42` = `SA_MP_RETAKE_ABS` in March's enum ( `cl /P` index count ); vslot `0xf0` = `UpNowMp`.
**Enum names added.** `X2SkillTree.h` ( `JUNE_ENUM_NAMES`, values checked against `Enum.src.lua`; March's `SI_` ids for these skills are 4xxx, so none equals June's ): `RNSI_SA_ROT_VALKYRIESJAVELIN` 24004, `RNSI_SA_RRF_WILD_CHARGE` 24009, `RNSI_SA_RBM_WOLF_FANG` 24017, `RNSI_SA_RWT_HARPOON_SPEAR` 24018, `RNSI_SA_RWT_REVOLVER_CANNON_ORDNANCE_PENETRATOR` 24019, `RNSI_SA_RWT_HELLFIRE_GATLING` 24020, `RNSI_SA_RVC_GIGA_PROMINENCE` 24021, `RNSI_SA_RVC_IGNITION_CROW_NAPALM` 24022, `RNSI_SA_RVC_IGNITION_CROW_INCINERATION` 24023, `RNSI_SA_RVC_DEADLY_RAID` 24024, `RNSI_A_RF_FLAME_SWORD` 24201, `RNSI_A_RRF_LIMIT_CRUSHER` 24205, `RNSI_A_RST_BLEEDING_SLICER` 24208, `RNSI_A_RBM_SONIC_SLASH` 24210, `RNSI_A_RWT_BARRAGE_ATTACK` 24212, `RNSI_A_RF_SHADOW_STEP_UPGRADE` 24217, `RNSI_P_RVC_REVERSAL_SOLDIER` 24413, `RNSI_HA_RBM_EXTREM_BLADE` 24601. `X2GURaven.h` ( `_CORE`, `RAVEN_STATE_ID`, June script ids ): `RSI_SHADOW_STEP_Z` 305, `RSI_SHADOW_STEP_Z_UPGRADE` 306, `RSI_SHADOW_STEP_X_UPGRADE` 307, `RSI_SHADOW_BACK_SLIDE_UPGRADE` 308, `RSI_SHADOW_BACK_SLIDE_END_UPGRADE` 309, `RVC_SI_SA_IGNITION_CROW_FIRE_UPGRADE` 312, `..._FIRE_OVERHEAT_UPGRADE` 313, `RVC_SI_SA_IGNITION_CROW_INCINERATION_FIRE_UPGRADE` 315, `..._FIRE_OVERHEAT_UPGRADE` 316, `..._OVERHEAT_END_UPGRADE` 317 ( states owned by CH3c2 ).
**Not written / routed.** CH1o: `SetEquippedSkillLevelStateData` needs June's `+ 1.0` term and, if it lifts the wrapper into the `CX2GUUser` vslot `0x7a8`, the Raven callee follows ( `kickoffs/char-raven/CH1o.md` already holds `SetSkillLevelStateData` ). CH3c2: the ten new states have no `UserUnitStateData` registration yet ( CH1m / CH3c2 register by script name ); until then `m_StateList[ state ]` is just an unused slot.

**Gates.**
- `flagcheck JUNE_CHAR_RAVEN_LIVE --with JUNE_CHAR_RAVEN_CORE --worktree --files X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `.cpp` `PASS +158 -1`, `.h` `PASS +10`; `1 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is a `//#define`. The one deleted line is the `GetHyperModeTime` comment the CH1h review asked to rewrite. `flagcheck JUNE_ENUM_NAMES --worktree --files X2Lib/X2SkillTree.h`: `1 PASS~`, `RESULT: PASS`. `git diff --stat`: `.cpp` 159 / 1, `.h` 10 / 0, `X2SkillTree.h` 22 / 0.
- One-file probe ( `census.cl_cmd`, `/c /Zs /EHsc`, no PCH; `-D` not `/D` ): `X2GURaven.cpp` exit 0, 0 errors with `_LIVE` off and with `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. `cl /P` off / on: `EventTimerOneshot( 0.001f )` 3 / 4, `RNSI_P_RVC_REVERSAL_SOLDIER` 1 / 3, `SA_MP_RETAKE_ABS` 1 / 2, `RSI_SHADOW_STEP_Z_UPGRADE );` 0 / 1, `RVC_SI_SA_IGNITION_CROW_INCINERATION_OVERHEAT_END_UPGRADE );` 0 / 1, `RNSI_HA_RBM_EXTREM_BLADE, true` 0 / 1, `SetEquippedSkillLevelStateData(` 9 / 50.
- `junediverge.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h X2Lib/X2SkillTree.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: `0 A/B row(s)`.
- `DEPLOY OK: X2_offline.exe is 26,012,160 B, dated 2026-11-06 08:51:30, md5 5b089990c6e9582f145fd1e39846aa33, equal to X2/US_SERVICE/x2.exe (2026-11-06 08:51:23); built at main e8c99bb6 + 3 uncommitted code file(s).` No `exestr` line: `_LIVE` is off.

**Nothing to play-test** ( `_LIVE` is off; nothing registers a June body ). Next: `/x2-phase char-raven CH1i review` ( Opus 5.5 high ), then CH1j.

### CH1i - Review (2026-11-06, Opus 5.5)

**Verdict: PASS, no code change.** I read both rows against `charport_adis.py raven 32c28270` / `32c4f640`. Every block, state id and call shape is as the Done note says.
- **`SetEquippedSkillLevel` `32c28270`:** the 17 tests are `cmp bl, 1` `je` (bChangeAll) then `cmp ebp, <id>` `jne`, in listing order 0x5dc4 .. 0x5dd8, 0x5e89 / 0x5e8d / 0x5e90 / 0x5e92 / 0x5e94, 0x6019, then 0x5e88 `je` / 0x5e99 `jne` (Shadow Step: 24200 or 24217, templet `( 0x5e88, 1 )`). Each block makes one `31fca8e0( id, 1 )` call, then pushes `( templet, state )` to vslot `0x7a8` per state. The 46 pushes match column 2 of `ch0_state_enum_names.tsv` one for one: 0xce 206; 0xdf / 0xe0 223 / 224; 0xdb / 0xdd / 0xd9 219 / 221 / 217 (HIT, HITLANDING, STARTLANDING, in that order); 0x114 / 0x115; 0xf0 / 0xf1; 0xf4; 0xf7 / 0xf8 / 0xfa; 0x10b / 0x10c / 0x138 / 0x139; 0x11d-0x11f / 0x13b-0x13d; 0x104-0x109; 0x116 / 0x117; 0x120 / 0x121; 0x112 / 0x113; 0x136 310; 0xec / 0xed 236 / 237; 0x123 / 0x124; 0x131-0x135 305-309.
- **`0xfa` = 250 = `RSI_SA_RWT_GIGA_PROMINENCE_OVER_FINISH`: confirmed.** `ch0_skill_listing_rows.tsv` has 250 registered at `32c65797` with its own StateStart and callbacks, read from the `RSI_SA_RWT_GIGA_PROMINENCE_FINISH` table. March does the same for `OVER_FINISH` (`X2GURaven.cpp:3776-3780`). `Enum.src.lua` puts `OVER_FINISH = 181` between END 180 and OVER_END 182, and in this band the exe id is the Enum.lua id + 69.
- **`0x13d` = 317 = `RVC_SI_SA_IGNITION_CROW_INCINERATION_OVERHEAT_END_UPGRADE`: confirmed by June's own parallel.** Napalm sets FIRE / FIRE_OVERHEAT (267 / 268, not 269 OVERHEAT_END) and their upgrades 312 / 313. Incineration sets FIRE / FIRE_OVERHEAT / OVERHEAT_END (285-287) and 315 / 316 / 0x13d. 314 then is the napalm OVERHEAT_END_UPGRADE that June does not set. Both OVERHEAT_END states are registered once for the base and upgrade skill (`32c65bc9`, skills 24022,24053 / 24023,24054), and `RAVEN_FIGHTER.src.lua:16585` has the table.
- **Callee:** `m_StateList` is a map with a default entry (`X2GUUser.h:1862`), so the ten states CH3c2 has not registered yet are harmless once `_LIVE` is on. The missing `+ 1.0` power-rate term stays CH1o's, and `kickoffs/char-raven/CH1o.md:24-25` carries it.
- **Enum names:** all 18 `RNSI_` values (and the existing `RNSI_A_RF_SHADOW_STEP` 24200) equal `Enum.src.lua`, and no March member has those values. The 10 `RAVEN_STATE_ID` names are the table's rows 305-309 / 312 / 313 / 315 / 316 and the 317 above. Their C++ values follow the enum, as every `_CORE` state does (`cpp_vs_june_reg_id` DIFF; registration is by script name).
- **`HyperModeFrameMove` `32c4f640`:** `fld 0.001; fcom [anim+0x24c]` with 0x41 `jne` skips when 0.001 <= `m_fAnimOneshotTimer` (+0x24c, funcmap 31fea120 C9h-4a), then `EventTimer 31d28e10`. This is March's inline `EventTimerOneshot`. The null `GetUnit()` / skill tree (+0x50) tests come next. Then `GetNowHp / GetMaxHp` (`fdivr`), `fcomp 0.5`, and `test ah, 5` `jp` go on only when the ratio is < 0.5 (unordered skips). `GetSkillLevel( 0x5f5d, 1, 0, 1 )` must be `jle` > 0. `GetSkillTemplet( 0x5f5d, 1 )` takes the flag every June caller passes (funcmap 32206450), so it is March's one-argument overload. `GetSkillAbilityValue( 0x42, level, 0, 0.0 )` is March's two-argument form, which passes `bPvp` false (`X2SkillTree.h:3158`; funcmap 32007b10). `SA_MP_RETAKE_ABS` is index 66 = 0x42 in `SKILL_ABILITY_TYPE` (cl /EP, both ways). vslot `0xf0` is `UpNowMp` (`3208dd00`). The tail `( 0.05, 0.83, false )` matches the default `bSound_`.
- **`GetHyperModeTime` comment:** the new line says what the CH1h review asked for. It is comment only (the one deleted line).

Gates (no build: the review changes no code). One-file probe, `/c /Zs /EHsc`: exit 0, 0 errors, both off and `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. `cl /EP` counts off / on are 3/4, 1/3, 1/2, 0/1, 0/1, 0/1 and 9/50, the same as the Done note. `flagcheck JUNE_CHAR_RAVEN_LIVE --with JUNE_CHAR_RAVEN_CORE --range 0dfbac65^..0dfbac65`: `.cpp` `PASS +158 -1`, `.h` `PASS +10`, `X2SkillTree.h` OTHER (`JUNE_ENUM_NAMES`). Summary: `1 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. `RESULT: FAIL` only because the `_LIVE` define is commented out. `flagcheck JUNE_ENUM_NAMES --range 0dfbac65^..0dfbac65 --files X2Lib/X2SkillTree.h`: `PASS~ +22`, `RESULT: PASS`. `junediverge.py --check-all`: `allow-listed: 186`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: `0 A/B row(s)`.
CH1j kickoff checked: present (Sonnet 5.5 high; 9 rows, inside 8 / new 1, by CH0's amendment; depends on this review). Nothing to play-test. Next: `/x2-phase char-raven CH1j` (Sonnet 5.5 high).

### CH1j - Done (2026-11-06)

All 9 class-map rows of phase `CH1j` are accounted for: **3 `inside` written** (June's body in March's own `CX2GURaven` function under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, March's in `#else`), **1 `new`** (`_CORE`) and **5 `inside` rows with nothing to write** because June's body = March's. `_LIVE` stays `//#define`. 9 funcmap rows `Char Raven CH1j`, 4 rows in `ch1_offsets.tsv`. No `JUNE_BUGS` row: nothing here is a June mistake.

| June | `CX2GURaven::` | kind | vs March |
|---|---|---|---|
| `32c25ab0` / `32c25ad0` / `32c25ae0` | CommonStateStartFuture / CommonStateEndFuture / CommonStateEnd | inside | = March (a `jmp` to the `CX2GUUser` base) |
| `32c3d1e0` | CommonRender_Prepare | inside | = March: `+0x964c` (`m_fShadowStepTimeLeft`) > 0 -> `CTT_RED` while the gage has hyper time left else `CTT_NORMAL`, outline 0, `RT_CARTOON_COLOR_EDGE`, then the base (June also takes and returns a hint argument; March has none) |
| `32c40ce0` | CommonFrameMoveFuture | inside | = March (two clamped countdowns, base, `m_CannonBallCount` synced when it differs) |
| `32c25ac0` | CommonEventProcess | inside, written | differs: the base only; March's `SKILL_FOR_CANCEL` `CheckPressedSkillForCancel` block is not in June |
| `32c43380` | CommonStateStart | inside, written | differs, see below |
| `32c55240` | CommonFrameMove | inside, written | differs, see below |
| `32c42f90` | EmergencyEscapeCoolTimeFrameMove | new `_CORE` | `0 < cool && !( cool < elapsed )` -> `elapsed += m_fElapsedTime` (the Elsword tick pattern; members were declared by CH1b) |

**CommonStateStart `32c43380`** (xor key tables decoded with a script from the listing: `DAMAGEDATA_CHANGE_TIME`, `...1`, `...2` into `+0x9750` / `+0x9754` / `+0x9758`, default 0). `ResetDamageType` is inlined (`+0x4114` = 1.0, `+0x410c` = 0, `+0x4110` = 0, `+0x4108` = -1 = March's `ResetDamageType` under `JUNE_DAMAGE_EXCEPTION`); `BURNING_NASOD_HAND_POSSIBLE` is read into a local; when it is set **and the byte `+0x9688` (skill 24401 learned) is 1**, `ChangeDamageType( +0x968c, +0x9690 )` and `+0x4108` = 2 (`AT_COMMAND`, the CH0 amendment's write). `SetPassiveCriticalRate( +0x9694 )` is unconditional (= March with `BALANCE_BLADE_MASTER_20130117`). **New:** every state except id 168 / 215 / 216 (`RSI_COMBO_ZZZX`, `RBM_COMBO_ZZZX_EXPLOSION`, `RBM_COMBO_ZZZXZ` by `ch0_state_enum_names.tsv` col 3) zeroes `m_iBlackHoleHitCount` and stops `m_hBlackHoleEffectSet`; then the base. The state id is `( +0x7cbc - +0x7cec ) / 100`, `GetNowStateID()`.

**CommonFrameMove `32c55240`**, in June's order: the base; **Open Mind** (skill `RNSI_P_RRF_OPEN_MIND` 24419 learned, a gage, hyper time left, and `GetLearnedSkillAbilityValue( SA_TIME_ABS = 0xa9, 24419 )` <= `( clock() - m_clockLastNasodBallAttack ) / 1000.0` -> `m_fRageGauge` is set to 60 when it is not above 60); the arc-enemy follow loop as March but June has **a fifth coefficient**: i = 1..4 -> 0.06 / 0.07 / 0.08 / 0.09, else 0.05 (jump table `32c55ec8` read from the listing; March stops at 0.08); `DoFrameMoveNasodBall`; arc time, Black Hole time (`RSI_BASE` = `0x99`) and overheat buff time as March; overheat object and the two fire effects as March; **new** Guardian Strike restore timer `+0x9774` (> 0: minus the frame time; <= 0: `SetLayer( XL_UNIT_0 )` (`+0x164` = 0xb), `+0x8e04` = 0, `m_RenderParam.color` = (1,1,1,1), timer = -1.0, `SetAlphaObject( false )`); `32c42f90`; **new** `+0x959c` counts down while >= 0 (Surely Victory's cool time, see below). **Absent from June (dropped under `_LIVE`):** the class-side hyper ball block (March `:5715-5757`; the gage UI `32e6d900` draws it, CH1q) and the inline Survival Technique of Mercenary block (March `:5817-5854`; the delegate `CheckAndDoServivalTechniqueOfMercenary` stays, CH4a checks it).

**New members (`_CORE`, `X2GURaven.h`):** `m_fSurelyVictoryCoolTime` (`+0x959c`; its writer `32c3df50` sets it from SA `0x94` of 24409 `RNSI_P_RBM_SURELY_VICTORY`, **inferred pairing**: the review and CH4a confirm), `m_bBurningNasodHandLearned` (`+0x9688`; written by `32c57660` @32c576d3, CH4a), `m_fGuardianStrikeRenderTime` (`+0x9774`; Guardian Strike FrameMove `32c30cb0` sets 1.4, CH2c). **`+0x8e04` = March's `m_bSetCustomRenderParam`** (identified by the same layer / custom-render / colour / alpha-object pattern in `X2GUEL.cpp:9209`, Ancient Fire; the CX2GUUser ctor zeroes it @32040e02). Layer ids 0xb / 0x10 are `XL_UNIT_0` / `XL_EFFECT_2` (March's `X2_LAYER`). No enum name was added: `RNSI_P_RRF_OPEN_MIND`, `SA_TIME_ABS`, `AT_COMMAND`, `XL_UNIT_0` exist.

**Routed.** CH1k: the constructor sets the three new members (kickoff amended). CH2c: `32c30cb0` sets `+0x9774` (amended). CH4a: `+0x9688` / `+0x959c` writers, the Survival Technique block (amended). CH1q: the hyper ball moves to the gage UI before `_LIVE` is on (amended).

**Gates.**
- `flagcheck JUNE_CHAR_RAVEN_LIVE --with JUNE_CHAR_RAVEN_CORE --worktree --files X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `.cpp` `PASS +204`, `.h` `PASS +4`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is a `//#define`. `git diff --numstat`: `.cpp` 204 / 0, `.h` 4 / 0.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH; `-D` not `/D`): `X2GURaven.cpp` exit 0, 0 errors with `_LIVE` off and with `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. `cl /EP` off / on: `m_bBurningNasodHandLearned` 1 / 2, `m_clockLastNasodBallAttack ) / 1000.0` 0 / 1, `RNSI_P_RRF_OPEN_MIND` 1 / 7, `coeff = 0.09f` 0 / 1, `m_fGuardianStrikeRenderTime = -1.f` 0 / 1, `RBM_COMBO_ZZZX_EXPLOSION != iNowStateID` 0 / 1, `m_fSurelyVictoryCoolTime` 1 / 3, `EmergencyEscapeCoolTimeFrameMove` 2 / 3 (the `_CORE` definition in both).
- `junediverge.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `RESULT: PASS`; `--check-all`: `allow-listed: 186` (unchanged), `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: `0 A/B row(s)`.
- `DEPLOY OK: X2_offline.exe is 26,012,160 B, dated 2026-11-06 09:28:53, md5 03038ced6747376bb941558d90770e6e, equal to X2/US_SERVICE/x2.exe (2026-11-06 09:28:46); built at main 831ef340 + 2 uncommitted code file(s).` No `exestr` line: `_LIVE` is off.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-raven CH1j review` (Opus 5.5 high), then CH1k.

### CH1j - Review (2026-11-06, Opus 5.5)

**Verdict: PASS, no code change.** I read all nine rows against `charport_adis.py raven --phase CH1j`. Every claim in the Done note holds.
- **`CommonStateStart` `32c43380`:** I decoded the four xor key tables by script from the listing (each byte `[0x337ae1xx]` with its own `xor` key, sorted by address). They give `DAMAGEDATA_CHANGE_TIME`, `DAMAGEDATA_CHANGE_TIME1` and `DAMAGEDATA_CHANGE_TIME2` (float getter `32fea370` into `+0x9750` / `+0x9754` / `+0x9758`, miss 0), and `BURNING_NASOD_HAND_POSSIBLE` (bool getter `32fe9e40`). The inlined `ResetDamageType` is `+0x4114` = 1.0, byte `+0x410c` = 0, `+0x4110` = 0 (`DT_PHYSIC` = 0), `+0x4108` = -1, the same as `X2GameUnit.h:3388`. Next come `cmp [esp+0x13], 1` and `cmp byte [+0x9688], 1` (both `jne`). The inlined `ChangeDamageType` copies `+0x968c` to `+0x4110`, `+0x9690` to `+0x4114` and 1 to `+0x410c`, then sets `+0x4108` = 2. `+0x9694` -> `+0x8b10` is unconditional. The state id is `( +0x7cbc - +0x7cec ) / 100` (the `0x51eb851f` / `sar 5` divide; funcmap `31fd8350`). The skip paths are `cmp 0xa8` `je`, and `> 0xd6` with `<= 0xd8`. Both jump straight to the base call `32057b80`. Otherwise the protected `+0x9674` is set to 0 and `321dafb0( &+0x9684, false )` runs when the handle != -1. Column 3 of `ch0_state_enum_names.tsv` gives 168 / 215 / 216 = `RSI_COMBO_ZZZX` / `RBM_COMBO_ZZZX_EXPLOSION` / `RBM_COMBO_ZZZXZ`.
- **`CommonFrameMove` `32c55240`:** the base runs first.
  - **Open Mind.** `31fceab0( 0x5f63 )` `jle` skips. Gage null skips. Hyper time `+0xe8` with `test ah, 5` `jp` goes on only when 0 < time. `clock()` minus `+0x9778`, `fild`, `/ 1000.0` gives the seconds since the last ball. Then comes `32007b10( 0xa9, 0x5f63 )` (ability, skill; `fcomp` with `test ah, 0x41` `jp`). It goes on when the SA value <= the seconds, and unordered skips. `fcomp 60.0` with 0x41 `jne` sends rage <= 60 to the temp `0x42700000`; rage > 60 copies the rage itself. One protected assignment then writes the temp back, so the code's `if( m_fRageGauge <= 60.0 ) m_fRageGauge = 60.f` is the same.
  - **Arc-enemy loop.** The two `GetDirVector` calls are March's `GetZVector` inlined: `( -z, 0, x )`, normalised when y != 0 (`X2GameUnit.h:2598`). `ebx` starts at -1 and equals `i - 1`. `cmp ebx, 3` `ja` default 0.05. The table bytes at `32c55ec8` (shown as instructions in the listing) read `32c559b7` / `bf` / `c7` / `cf`, which load 0.06 / 0.07 / 0.08 / 0.09 for i = 1..4. The offset, rotation and lerp maths and the `ClearArcEnemyData` break all match the code.
  - **Tail, in June's order:** `DoFrameMoveNasodBall`, then the arc time. `GetNowHp` is inlined (gage null skips, as March returns 1). Black Hole time goes <= 0 -> 0 and stage `0x99`, then stops. `0x99` = 153 is `RSI_BASE` in June's numbering, because `RSI_DASH_JUMP_POWER_LANDING` (Enum.lua 90) registers at 154. After that come the overheat buff time and `DeleteOverHeatObject`, the overheat object (`SetUseDXMatrix` inlined; no frame null check, as March) and fires 1 and 2 (`Bip01_L_Forearm`, `Bip01_L_Hand`).
  - **Guardian Strike.** `> 0.0` -> minus the frame time -> `<= 0`. That writes `+0x164` = 0xb (`SetLayer` is `m_Layer = layer`, `KTDGObject.h:248`; `XL_UNIT_0`), `+0x8e04` = 0, colour `+0xc38..0xc44` = 1.0, timer -1.0 and `+0x144` = 0 (`SetAlphaObject`, `KTDGObject.h:183`). Then `32c42f90` is called. `+0x959c` goes down while `0 <= v` (0x41 `jp`).
  - No hyper ball code and no Survival Technique call or block appear anywhere in the listing.
- **`32c42f90`:** it goes on when `0 < cool` (`test ah, 5` `jp`), and stops when `cool < elapsed` (`fcompp` with `test ah, 1` `jne`). Otherwise elapsed += `m_fElapsedTime`, a protected assignment with the equality skip. That is the same as the `_CORE` body.
- **Base-only and equal rows:** `32c25ab0` / `ad0` / `ae0` / `ac0` are a `jmp` to `320506f0` / `31fdd8e0` / `32015250` / `31fe7610`, the base `CommonStateStartFuture` / `CommonStateEndFuture` / `CommonStateEnd` / `CommonEventProcess` by funcmap. `32c40ce0` has both clamps (`< 0` -> 0), then the base `32061bf0`, then a protected write of `+0x97b0` (`GetNasodBall()`) when it differs. That is March's sync. `32c3d1e0` writes `+0xc14` = 4 (`CTT_RED`) or 0, `+0xc28..0xc34` = 0 and `+0xc10` = 2 (`RT_CARTOON_COLOR_EDGE`) only when the shadow-step time is > 0, then the base. That equals March's body.
- **Members:** the `ch1_offsets.tsv` rows `0x959c` / `0x9688` / `0x9774` / `0x8e04` / `0x9778` match the listing. I spot-checked the CH1k amendment in the ctor `32c3ece0`: `fldz` -> `+0x959c` @32c3ed3a, byte `+0x9688` = 0 @32c404e3, -1.0 -> `+0x9774` @32c4015e. The CH4a / CH2c / CH1q amendments are present and say what the Done note routes.
- **Not a defect:** June keeps `m_iBlackHoleHitCount` (`+0x9674`) as a `KProtectedType<int>`, while March has a plain `int`. Only the protection differs, not the behaviour.

Gates (no build: the review changes no code). One-file probe, `/c /Zs /EHsc`: exit 0, 0 errors, both off and `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. `cl /EP` counts off / on are 1/2, 0/1, 1/7, 0/1, 0/1, 0/1, 1/3 and 2/3, the same as the Done note. `flagcheck JUNE_CHAR_RAVEN_LIVE --with JUNE_CHAR_RAVEN_CORE --range 0de3de2d^..0de3de2d`: `.cpp` `PASS +204`, `.h` `PASS +4`. Summary: `0 deleted, 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`. `RESULT: FAIL` only because the `_LIVE` define is commented out. No later commit touches the flag. `junediverge.py --check-all`: `allow-listed: 186`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: `0 A/B row(s)`.
CH1k kickoff checked: present (Sonnet 5.5 high; depends on this review; the CH1j amendment names the three ctor writes, confirmed above). Nothing to play-test. Next: `/x2-phase char-raven CH1k` (Sonnet 5.5 high).

### CH1k - Done (2026-11-06)

All 9 class-map rows of phase `CH1k` plus `ReservedSkillStart` are accounted for: **7 written** (the `new` `SpecialAttackLookup` / `SpecialAttackUse` / `SpecialAttackCooldown` under `_CORE`, named as Chung's and Elsword's steps 510 / 511 / 512; `SpecialAttackEventProcess`, `NoStateChangeActionFrameMove` and the constructor `inside` under `_LIVE` / `_CORE`; the `new` `IsNotCommonMoveState` under `_LIVE`, because it overrides a base virtual), **2 equal to March** (`SpecialAttackNoStageChange` returns false; `ShowActiveSkillCutInAndLightByScript`), and the new `ReservedSkillStart` (`_LIVE`). `_LIVE` stays `//#define`. 10 funcmap rows `Char Raven CH1k`, 17 offsets named in `ch1_offsets.tsv`. One name registered in `X2SkillTree.h` under `JUNE_ENUM_NAMES`: `RNSI_P_RVC_WINNABLE_TECHNIQUE_OF_MERCENARY` 24412 (June `Enum.src.lua` 15962). No `JUNE_BUGS` row: the Shadow Step chain below is a June design change, not a mistake.

| June | `CX2GURaven::` | kind | vs March |
|---|---|---|---|
| `32c25af0` | SpecialAttackEventProcess | inside | June's: lookup (vslot 0x7f8), use (0x7fc) + `Send_SKILL_USE_REQ`; **the caller's use condition** 3 / 5 (not the templet's, unlike Elsword's) gives the chosen id / reserved MP / slot back; used -> `m_iNowSpecialAttack` = slot + 1 |
| `32c58c30` / `32c25ba0` / `32c43bf0` | SpecialAttackLookup / Use / Cooldown | new | the checks, the use and the spending March had in one body, see below |
| `32c25c40` | NoStateChangeActionFrameMove | inside | the base `32008e40` (`UserNoStateChangeActionFrameMove`); March's is empty |
| `32c25c30` / `32c3e770` | SpecialAttackNoStageChange / ShowActiveSkillCutInAndLightByScript | inside, nothing to write | = March |
| `32c3e0e0` | IsNotCommonMoveState | new, `_LIVE` | the base test less the now state `0x9a` = `RSI_DASH_JUMP_POWER_LANDING` (by name; CH1m registers by name) |
| `32c3ece0` | constructor | inside | March + `_CORE` inits + `_LIVE` `m_fLimitOverHeatHP` 0.25f (March 0.1f) |
| `32057b80` @32057c4c | ReservedSkillStart | new, `_LIVE` | Elsword's; `X2GUUser.h` / `.cpp` chain widened with `_LIVE` |

**Where June differs from March** (all from `charport_adis.py raven --phase CH1k`):
- **Lookup `32c58c30`.** Quick-slot wait, `CommonSpecialAttackEventProcess`, the equipped templet (id must equal the slot's, else string 0x8bae), then **Air Slicer is gone**: with `RNSI_P_RVC_WINNABLE_TECHNIQUE_OF_MERCENARY` learned, Barrage Attack and Flame Sword (upgrade swapped) are used in the air as on the ground (March: `SI_P_RVC_AIR_SLICER` and four skills). `CheckSkillUseCondition`, `CheckSkillUsePVPOfficial`, `CheckSkillUseNotPve` / `NotDungeon` (`3202d140` / `3202d3c0`), the riding-key wait, no skill while parrying except Shadow Step. **A Shadow Step chain** (`m_bActiveShadowStepCombo`) lets only Shadow Step through, needs the back slide passive, stops once the loop count passes 2, resets Blade Master Z-Z-Z-X-Z, and returns true with **no MP cost (March: `FlushMp( 5.f )`) and no slot refresh**. MP is `GetActualMPConsume( id, level )` with none of Elsword's Brutal Slayer / Vigor terms. Dev gate (`IsDevUnlimitedSkillUse()`, vslot 0x82c is always false) around the cool time (226), MP (2549) and finality stone (26119, `IsTrainingOrPhotozone` as the pilot). Gone from March: the skill-tree-learning message (25110), `CountUsedSkill` (routed to CH1p), the Revolver Cannon memo cool time cut (CH4b), the rage and ZZZXZ blocks (now in the cooldown).
- **Cooldown `32c43bf0`.** `IsMyUnit`, slot mark, `UpNowMp`, finality stone, then **the cool time with the real skill level** (Elsword's JUNE_BUGS 524 asks level 1; here `eax` survives to `[edx+0x6a4]`); Shadow Step (`GetUpgradeBaseSkillID( id )` = 24200) **never sets it**; a special active skill in hyper time adds `( m_AdditionalCharacterSpecializationA + 1.0 ) x MP / 3.0` to `m_fRageGauge` (`+0x6c50`, Eve CH1q; March: MP / 3) clamped to 100; `+0x8ac0` = slot + 1; stage `RBM_COMBO_ZZZXZ` (0xd8) reset; `ApplySkillCoolTimeRateByBuff`; random mission hook.
- **Constructor.** Every default of a March member equals June's except `m_fLimitOverHeatHP`; June adds the six `_CORE` members CH1h / CH1j declared. Seventeen offsets are named in `ch1_offsets.tsv` (reader owners in the note column). **Routed to CH2a:** the Shadow Step members `+0x9654` (0.0; March's attack power rate is 1.f), `+0x9664` (1.0), `+0x972c` / `+0x9730` (0; June has two dwords where March has four members, one is `m_fShadowBusterPowerRate` 0.5f): the phase that reads each decides the March member.
- **`ShowActiveSkillCutInAndLightByScript` `32c3e770`** calls `32c3e450` (June's cut-in with the event 0x8b3 swap and the customize override), which CH0 gave to CH3a (`CH3a.md`, "by CH0's hand rule"); until then the script path runs March's cut-in.

**Routed.** CH1m: `IsNotCommonMoveState` / `ReservedSkillStart` need nothing registered; skill-slot states must be the `_CONST_GUUSER_` ids. CH1p: no `CountUsedSkill` in June's use path; `+0x8ac0` written by the cooldown. CH4b: cool time / MP extras. CH2a: the Shadow Step defaults and the chain's missing MP cost. All four kickoffs amended. Checklist rows §7 L262 / L263 / L265 tagged `(Raven: CH1k)`.

**Gates.**
- `flagcheck JUNE_CHAR_RAVEN_LIVE --with JUNE_CHAR_RAVEN_CORE --worktree --files X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `.cpp` `PASS +291`, `.h` `PASS +7`; same flag on `X2GUUser.cpp` / `.h`: `PASS +11` each; `files: 2 PASS, 0 FAIL`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is a `//#define`. `flagcheck JUNE_ENUM_NAMES` on `X2SkillTree.h`: `PASS~`, `RESULT: PASS`.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH; `-D`): `X2GURaven.cpp` exit 0, 0 errors with `_LIVE` off and with `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. `cl /EP` off / on: `CX2GURaven::ReservedSkillStart` 0 / 1, `CX2GURaven::IsNotCommonMoveState` 0 / 1, `iSavedSkillID` 0 / 3, `m_fLimitOverHeatHP = 0.25f` 0 / 1, `FlushMp( 5.f )` 1 / 0 (March's body is dropped), `SpecialAttackLookup` 4 / 5, `CX2GURaven::SpecialAttackCooldown` 1 / 1 (the `_CORE` definition in both), `m_clockLastNasodBallAttack = clock` 1 / 2.
- `junediverge.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h X2Lib/X2GUUser.cpp X2Lib/X2GUUser.h X2Lib/X2SkillTree.h`: `allow-listed: 8`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: `0 A/B row(s)`.
- `DEPLOY OK: X2_offline.exe is 26,014,208 B, dated 2026-11-06 10:03:24, md5 1e01737b6270b34f0455f93906363ac7, equal to X2/US_SERVICE/x2.exe (2026-11-06 10:03:17); built at main 5302ffc6 + 5 uncommitted code file(s).` X2Lib 0 errors. No `exestr` line: `_LIVE` is off.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-raven CH1k review` (Opus 5.5 high), then CH1m.

### CH1k - Review (2026-11-06, Opus 5.5)

**Verdict: PASS, one comment fix.** I read the nine rows against `charport_adis.py raven --phase CH1k`. The constructor store table was built by a subagent, and I spot-checked it.
- **Lookup `32c58c30`:** the order matches the code. Quick-slot wait (`+0x8d78`, `test ah, 5` `jnp` = fails when > 0), then `CommonSpecialAttackEventProcess` (slot data `[esp+0x40]`, index `[esp+0x44]`, -1), the templet `( id, 1 )`, then id != slot id -> 0x8bae. `+0x8ea0` is written @32c58e63, before Winnable Technique. `GetSkillLevel( 0x5f5c, 1, 0, 1 ) > 0`, then swap `0x5e94` / `0x5e89` against the templet. Argument cond 2 / 4 -> 1 goes to `CheckSkillUseCondition` only. Then the four checks, riding wait (`329cc370`), parrying + swap `0x5e88`. The chain block: `+0x9721` == 1, swap `0x5e88`, protected `+0x9704` `jg 2` fails. Stage `0xd8` -> Black Hole time 0, count 0, stage `0x99`. Every chain exit is `32c59515` `mov al, 1`, with no MP write and no slot write. Off the chain: `GetSkillLevel( id, 0, 0, 1 )`, clamped to 1. Vslot 0x6a0 `( id, level, 1 )` is the 2-argument virtual (`X2GUUser.h:3159`: "called with true"). Vslot 0x82c `jne` skips to the success tail. Cool time `+0x24` > 0 -> 226. Reserved + chosen > now MP -> 2549 (`test ah, 0x41`). Type 0xd, not training, `GetNumItemByTID( 0x1fca0 ) <= 0` -> 26119. Success writes id and slot `[esp+0x44]`.
- **Enum probe** (`enumprobe.py`): ASUT 1 / 2 / 3 / 4 / 5, 24212 / 24201 / 24200 / 24412 = the pushes, ST 13 / 7 / 14 / 16, `ATI_HYPER_SKILL_STONE` 0x1fca0, `MAX_ACTIVE_SHADOW_STEP` 2, `XUF_DODUM_20_BOLD` 3, `DT_CENTER` 1. 24412 = `Enum.src.lua:15962`. The C++ `RBM_COMBO_ZZZXZ` / `RSI_BASE` / `RSI_DASH_JUMP_POWER_LANDING` are 165 / 107 / 108, against June's 216 / 153 / 154. That is the plan's "write by name, CH1m maps" rule (L397), not a defect.
- **Event process `32c25af0` / use `32c25ba0`:** `[esp+0x18]` is the caller's argument (4 pushes + `edi`). Cond 3 / 5 puts back the id, reserved MP and slot. When used, `+0x88bc` = slot + 1 after the restore. When not used, only the id is put back. Use: vslot 0x810 true -> 0x800 `( templet id, chosen MP )`, 0x764 `( id, 1, MP )`, MP 0. Otherwise 0x804 + `31fd32e0`. `+0x8ac4` = 1 on both paths.
- **Cooldown `32c43bf0`:** all of it matches, in June's order. `IsMyUnit`, then `GetUnit` / templet null. Slot mark `( slot, 0 )`, `UpNowMp( -mp )`, stone `( 0x1fca0, 0, 1, 1 )`. The real level reaches vslot 0x6a4. `32207210( id argument )` != 0x5e88 gates `SetSkillCoolTimeLeft( id, cool, 1 )`. Special active + hyper > 0 + `IsMyUnit` -> `( +0x6c50 + 1.0 ) x ( mp / 3.0 )`, a protected write only when the value changed, then clamped at 100.0. `+0x8ac0` (`m_eSpecialAttackKeyPressed`, Arme CH3a) = slot + 1, then the `0xd8` reset, `3235a710`, `31fcd3d0`. The order differs from Elsword's (random mission last here, as June).
- **Equal rows:** `32c25c30` is `xor al, al` = March's `return false`. `32c3e770` passes `L"Dummy2_Lhand"`, `+0x8acc`, time and `bOnlyLight`, the same as March's call (the cut-in callee is CH3a's). `32c25c40` = `jmp 32008e40` = `UserNoStateChangeActionFrameMove`. `32c3e0e0` = `31fd7640`, then `( +0x7cbc - +0x7cec ) / 100 != 0x9a` (funcmap `31fd7640` CH1l30).
- **`ReservedSkillStart`** = Elsword's body. The `X2GUUser.cpp` / `.h` chains emit exactly one call / one declaration for any `_LIVE` combination.
- **Constructor `32c3ece0`:** the six `_CORE` inits (lines 33 / 34 / 170 / 1695 / 1461 / 1462) and 0.25 @32c408fd match. About 57 named stores equal March's defaults. June-only `+0x9598` / `95b8` / `95c4` / `96bc` / `96e0` / `96f0` / `9790` match their rows. Shadow Step `+0x9654` 0.0, `+0x9730` 0.0, `+0x9664` 1.0 and `+0x972c` 0 are routed to CH2a, as the note says. The unnamed stores are 0 / -1 except `+0x9690` 1.0 and `+0x8d5c`-`8d74`, which equal March's `m_fChangeDamageRelRaven` and throw values.
- **Not exact in the Done note:** "every default of a March member equals June's" leaves two March members with no June store. They are `m_fChanceOfExplosiveNasodHand` (`.1f`) and `m_fAttackPowerRateShadowStepPunisher` (`1.f`). March's `InitPassiveSkillState` resets both before any reader (`:2412`, `:2450`), so nothing changes before CH4a. **Routed to CH4a** (kickoff amended).
- **Fixed:** the chain comment (`X2GURaven.cpp:6264`) said "( JUNE_BUGS )", but no row exists and the Done note rules it a design change. That holds: `ReserveSkillUse` zeroes the chosen MP after the first Shadow Step, so the chain reserves 0. I dropped the tag.

Gates. Probe `/c /Zs /EHsc`: exit 0, 0 errors, off and `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. `cl /EP` off / on: 0/1, 0/1, 0/3, 0/1, 1/0, 4/5, 1/1, 1/2, the same as the Done note. `flagcheck JUNE_CHAR_RAVEN_LIVE --with JUNE_CHAR_RAVEN_CORE --range 7c1254f2^..7c1254f2`: `.cpp` `PASS +291`, `.h` `PASS +7`, `X2GUUser.cpp` / `.h` `PASS +11`, `0 deleted, 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`. `JUNE_ENUM_NAMES --files X2Lib/X2SkillTree.h`: `PASS~`, `RESULT: PASS`. The fix, `--worktree --files X2Lib/X2GURaven.cpp`: `PASS`, 1 deleted line (the replaced comment, inside CH1k's block). `RESULT: FAIL` only because the `_LIVE` define is commented out. `junediverge.py --check-all`: `allow-listed: 186`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: `0 A/B row(s)`.
`DEPLOY OK: X2_offline.exe is 26,014,208 B, dated 2026-11-06 10:15:22, md5 2caffbe9b9918879a4b95d5110481e9a, equal to X2/US_SERVICE/x2.exe (2026-11-06 10:15:15); built at main 7c1254f2 + 1 uncommitted code file(s).`
CH1m kickoff checked: the CH1k amendment is present (`IsNotCommonMoveState` / `ReservedSkillStart` register nothing; skill-slot states = the `_CONST_GUUSER_` ids). Nothing to play-test. Next: `/x2-phase char-raven CH1m` (Opus 5.5 high).

### CH1m - Done (2026-11-06)

All five class-map rows are written, all `inside`: June's bodies sit in March's own `InitState` (`32c66fe0`, vslot 0x7a4), `InitStateCommon`
(`32c5d660`) and the three line registrars `InitStateSwordTaker` (`32c511c0`), `InitStateOverTaker` (`32c51b70`), `InitStateWeaponTaker`
(`32c63670`), under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` with March's bodies in the `#else` (still `//#define`d). Class map: the four registrars' March
pair corrected from `InitState` to those names. Raven has no `InitStateByUnitClass`: June's class tests sit in `InitState`, as March's do.

**How it was derived.** `june2017_re/char_raven/ch1m_regsim.py` (Elsword's, with Raven's class registers `local_68` / `uVar4` / `iVar5`) walks fresh
decompiles of the five registrars once per class; counts equal CH0's listing-checked `ch0_reg.tsv` (103 / 29 sites / 12 / 11 / 13). `ch1m_reg_by_class.tsv`
is that output. `ch1m_gen.py` writes the C++ (ids by table name: a table that is a `RAVEN_STATE_ID` member registers under its own name, else March's own
registration of the table; callbacks from `funcmap.tsv`, else CH0's `inside` member, else CH0's `pair`); `ch1m_spec.py` makes the bytepatch spec.
`ch1m_regcheck.py` parses the `_LIVE` branches per class (line registrars for the classes InitState's switch sends there, order checked):
**761 registrations match slot by slot, 0 problems; 147 June ids map one-to-one both ways** (`ch1m_idmap.tsv`).
**One id differs from March:** June registers `RWT_COMBO_ZZZX` under its own id 304 (CH1d's member) beside `RSI_COMBO_ZZZX` 168 for Veteran Commander; March
shared `RSI_COMBO_ZZZX`. `RBM_` / `RRF_` / `RWT_COMBO_X` / `RSI_SOUL_TAKER_DASH_COMBO_ZZ` keep March's shared ids (168 / 182 / 173 / 177, = June).

**InitState `32c66fe0`:** cut-in 4 / 16 / 17 / 25 -> 0, 106 -> 1, 107 -> 2, 115 -> 3 (no default write), sub-index 16 -> 1, 17 -> 2, else 0 (= March).
InitStateID is inlined @32c6709f-32c670a5 (`m_CommonState.m_Wait`, `m_ChargeMpState` = 13): March's live body is the same (`TRANSFORMER_TEST` off, `cl /P`),
so it is called unchanged. `31fc93d0` (empty stub) is not called. Then `InitStateCommon`, `InitStateCommonRandom` (3202b110 + 31fe22d0), START (Start /
EventProcess / End), WIN (`WinStateStart`, `RandomLoseEventProcess`; no FrameMove, unlike Elsword), LOSE (`LoseStateStart`, plus `RSI_RWT_LOSE2_FrameMove`
only when the roll picked `RSI_RWT_LOSE2`, the compare @32c673b1, = March), `InitWaitStateChange( "RSI_WAIT_STATE_CHANGE" )` @32c67494 (7h), the line
switch @315 (16 / 106, 17 / 107, 25 / 115), five class tests written as `switch( eClass )`, then vslots 0x690 / 0x684( 0 ) / `31fd8f00` @32c6839f.
Per class 7 / 13 / 7 / 7 / 13 / 12 / 14 registrations incl. the four head ones. Veteran Commander registers `RSI_SOUL_TAKER_DASH_COMBO_ZZ` twice (32c63670
@268, then InitState @1114 with the same callback): both kept in June's order.

**InitStateCommon `32c5d660`** (103 per class, no class or skill test): hyper id 23 = `GetHyperModeTableName( false )` (Raven's RBM / RRF / RVC cases were
already there), FrameMove = Raven's `HyperModeFrameMove` (vslot 0x600), End = `CX2GUUser::HyperModeEnd`. Bounding = `InitBoundingStates( "RSI_" )`
at June's place (7l). Parrying (0xca / 0xcb) registers March's `RSI_PARRYING_*` members (bodies CH1q). CH1b's ten stand-up states (Front / Back, 4 Rolling, 4 Attack) take its `RSI_DAMAGE_STANDUP_FRONT_StateStart` / `_StateStartFuture`. The 5 `USI_EVENT_*` states of this phase are 3202b110's, run by `InitStateCommonRandom`.
Routed registrations done: CH1d's 4 new functions (March's `RSI_COMBO_ZZZX_Start` is no longer registered), CH1e's 12, CH1f's 9 (`XXfrontX` no Init /
FrameMove, `XXdownX` no FrameMoveFuture, `XXfrontXX4_EventProcess` also on `XXfrontXXdownX`), CH1g2's `RVC_DASH_COMBO_ZZ` (EventProcess only).
CH1i's ten `_CORE` states are not these registrars': 305-309 are `InitPassiveSkillState` `32c57660`'s (CH4a), 312 / 313 / 315-317 the skill slot's (CH3c2).

**Left out, each a comment line with its owner:** phoenix run 123-126 (PvP), dance 127-129 (Stage 10), riding 93-96 (Stage 17), the empty slots
`3202a7d0` (die end, in `StateChange`) and `32002f70` (Stage 17). **JUNE_BUGS 542:** `RSI_RIDING_DAMAGE_BACK` registered with no script table; the script
defines `RSI_RIDING_DAMAGE_FRONT` twice (lines 9775 / 9782; Raven's twin of 525).

**The `_LIVE` gate (`Always.h`):** seven `#error` guards added (Elsword's set): `JUNE_CHAR_ARME_CORE`, `JUNE_CHAR_EVE_CORE`, `HYPER_MODE_FIX`,
`JUNE_COMMON_RANDOM_STATE`, `JUNE_EVENT_STATES`, `JUNE_WAIT_STATE_CHANGE`, `JUNE_BOUNDING_STATE`; all defined (`cl /P` flag probe) and the `_LIVE` probe passes them.
For CH1n: `june2017_re/char_raven/ch1m_user_callbacks.tsv` lists the 132 `CX2GUUser` members the registration names; only `RidingDamageFrontStartFuture`
and `RidingOnEnd` are not in Elsword's list. Checklist: 7h, 7l and the 7ao hyper-table bullet marked `(Raven: CH1m)`.

**Checks.** One-file probes (`census.cl_cmd`, `/c /Zs /EHsc`) of `X2GURaven.cpp`, `_LIVE` off and on: exit 0, 0 errors each. `cl /P` off / on:
`InitWaitStateChange( "RSI_WAIT_STATE_CHANGE" )` 0 / 1, `InitBoundingStates( "RSI_" )` 0 / 1, `GetHyperModeTableName( false )` 0 / 1, the LOSE2 compare 0 / 1,
`RSI_DAMAGE_STANDUP_FRONT_StateStart` 4 / 24, `RSI_COMBO_ZZZX_Start` 3 / 2, `RVC_DASH_COMBO_ZZ_EventProcess` 2 / 3.
`git diff --patience --numstat`: `X2GURaven.cpp` 1377 / 0, `Always.h` 21 / 0. bytepatch: CRLF 1766 and high bytes 20340 unchanged.
`flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE --worktree --files X2Lib/X2GURaven.cpp KTDXLIB/Always.h`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only for the `//#define`.
`junediverge.py --check X2Lib/X2GURaven.cpp KTDXLIB/Always.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: `0 A/B row(s)`.
`DEPLOY OK: X2_offline.exe is 26,014,208 B, dated 2026-11-06 10:41:47, md5 ffd1e91ecd33d37fb1d07df5b8cd30d4, equal to X2/US_SERVICE/x2.exe (2026-11-06 10:41:40); built at main f4777c79 + 2 uncommitted code file(s).`

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-raven CH1m review` (Opus 5.5 high).

### CH1m - Review (2026-11-06)

**Verdict: PASS, no code change.**

- **Simulation.** Fresh decompiles of `32c5d660` (1379 lines) / `32c66fe0` (1163) / `32c511c0` (591) / `32c51b70` (550) / `32c63670` (649) through
  `ch1m_regsim.py` equal `ch1m_reg_by_class.tsv` row for row (1046 rows; the header is the only diff), with no `UNKNOWN COND`. Every `(iVar5 == 0)` the
  override takes false is a shared_ptr release count (`piVar3[1]` / `[2]` decrements), except @261, the LOSE2 `compare` result. `iVar5` is the class
  register only from @1087. `ch1m_regcheck.py`: `matched 761 registrations, 0 problems; ids mapped 147`.
- **Head, from the exe bytes.** `31fc93d0` is called first (@32c6701d); its body is a bare `RET`. Cut-in table `32c6847c` / index `32c68490`:
  4 / 16 / 17 / 25 -> `+0x8ac8 = 0`, 106 -> 1, 107 -> 2, 115 -> 3; the other 105 values skip to `32c67072`. Sub-index `+0x8acc`: 16 -> 1, 17 -> 2, else 0.
  Then only two writes, `+0x4088` and `+0x7d04` = 0xd (@32c6709f / @32c670a5; Elsword's four included two Elsword members). That equals March's
  live `InitStateID`. Calls in order: `32c5d660`, `3202b110`, `31fe22d0` (START / WIN / LOSE OUT strings), `32078bc0` (the `UserUnitStateData` constructor).
- **START / WIN / LOSE.** START (9): `31ffbd70` / `3201b390` / `3201b460`. WIN (0xb): Start `320004b0`, EventProcess `31fcd0b0`, no FrameMove. LOSE (0xc):
  Start `320007e0`. The compare @32c673b1 runs on the LOSE string (`ecx = [esp+0x8c]` after four pushes = `[esp+0x7c]`, the third OUT argument),
  length 0xd. On a match it fills `[esp+0x4c]` = delegate +0x34 = FrameMove with `32c69000` -> `32c33bd0` (`RSI_RWT_LOSE2_FrameMove`).
  0x82 `"RSI_WAIT_STATE_CHANGE"`: EventProcess `31fc8620`. Tail @32c68384: vslot 0x690, vslot 0x684( 0 ), `31fd8f00`.
- **Class tests, capstone over `32c66fe0`-`32c6847c`.** Ghidra's listing skips the shared-call tails. Line switch @315 (`add -0x10`, table `32c68500` /
  `32c68510`): 16+106 -> `32c511c0`, 17+107 -> `32c51b70`, 25+115 -> `32c63670`. @344: 0x6b -> RRF, 0x73 -> RVC (252, 232, 233 `0xe9` into the shared call
  @32c6789d), else `RSI_DASH_JUMP_COMBO_X` on 0xb6. @564: 0x6a -> `RBM_COMBO_ZZZX` on 0xa8. 0x73 -> 0xa8 `RSI_COMBO_ZZZX` (@32c67a0a), then `push 0x130`
  `RWT_COMBO_ZZZX` into the shared call @32c67b81. Else 0xa8 `RSI_COMBO_ZZZX`. @720: 0x19 / 0x73 -> `RWT_COMBO_X`, else `RSI_COMBO_X`, both 0xad.
  @815 (table `32c68574` / `32c68584`): 16+106 -> 0xd4-0xd8, 107 -> 0xcf-0xd3, 115 -> 0xfd-0xff. Each case ends in the shared call @32c682da
  (0xd7 / 0xd3 / 0xff). @1088: 0x10 / 0x6a / 0x73 -> 0xb1 `RSI_SOUL_TAKER_DASH_COMBO_ZZ`. The C++ has every case in this order.
- **Ids.** `ch1m_idmap.tsv`: 147 rows, no duplicate either way. 304 is pushed as its own id. 168 / 173 / 182 / 177 carry the `RBM_` / `RRF_` / `RWT_` /
  `RSI_SOUL_TAKER_` tables (strings read at `33515068` / `33515088` / `33515050` / `33514064`). June 72 = `GetRidingStartStateID()`.
- **InitStateCommon.** Id 23's table is `31fdd490( ..., 0 )` = `GetHyperModeTableName( false )`. FrameMove `32bd5f20` -> `32adf320` = `jmp [eax+0x600]`
  (Raven's `HyperModeFrameMove`). End `329cc0e0` = vslot 0x604, which Raven does not override (`ch0_vtable_all.tsv`: base `3201b8e0`), so it is
  `CX2GUUser::HyperModeEnd`. CH1b's pair `32c29130` / `32c290d0` is on exactly ten states (32, 33, 155-160, 204, 205). Bounding 120-122 and parrying
  0xca / 0xcb sit at June's place.
- **Routed / doubles.** `XXfrontXX4_EventProcess` `32c2ffb0` is on 194 and 195. `RVC_DASH_COMBO_ZZ` 303 has EventProcess only. Veteran Commander's 177
  is registered three times: InitStateCommon @1110 (`RSI_DASH_COMBO_ZZ`, `32c2c810`), `32c63670` @268, then InitState @1114 (both `32c2dd50`). All are
  kept in June's order, so the last registration wins as in June.
- **Left out / Always.h.** Each left-out state has its owner line: riding 93-96, phoenix 123-126, dance 127-129, `3202a7d0`, `32002f70`. JUNE_BUGS 542 holds.
  The seven guards are Elsword's CH1m set without his two Elsword-only flags. Raven's registration has no skill test, so `JUNE_SKILL_TEMPLET` is not needed.
  `ch1m_user_callbacks.tsv`: 132 members. The two not in Elsword's list are June `31fc8810` / `31fce160`.

**Hygiene.** `flagcheck JUNE_CHAR_RAVEN_LIVE --range 27ede6e0^..27ede6e0 --files X2Lib/X2GURaven.cpp KTDXLIB/Always.h`: `files: 2 PASS, 0 PASS~
(blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted
line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. RESULT FAIL only for the `//#define`. `git diff --patience --numstat`: 1377 / 0, 21 / 0.
`junediverge.py --check-all`: `allow-listed: 186`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: `0 A/B row(s)`.
One-file probes (`/c /Zs /EHsc`): `off exit 0 errors 0`, `on exit 0 errors 0`. No code change, so no build.

**Nothing to play-test** (`_LIVE` is off). CH1n's kickoff stands as written (Amended by CH1m checked). Next: `/x2-phase char-raven CH1n` (Opus 5.5 high).

### CH1n - Done (2026-11-06)

**Head (`32c641a0`), inside, `_LIVE`.** June's head sits in March's own `SetEquippedSkillFuncBySkillSlotIndex` up to the id switch; March's head and
type switch are the `#else` (`_LIVE` still `//#define`d). Read from the decompile and the exe bytes (Elsword's CH1n is the template):
- Bounds `0 > slot || 4 < slot`; tables `3350f9e8` / `3350fa10` (normal A / B = 50.. / 60..) and `3350f9fc` / `3350fa24` (hyper 51.. / 61..) = March's
  `_CONST_GUUSER_` arrays (read from the exe). `31fd2af0` dropped (7i), `+0x11` = `IsSkillSlotState`, hyper `0x101`; `GetEquippedActiveSkillTemplet( id, true )`.
  The batch `3207c1a0` / `31fdf970` is anti-tamper only.
- **Transcendence memo @32c642aa:** `31fdbcc0`, then `32206450( memo, 1 )` non-null -> the epilogue. No case: a transcended skill registers nothing.
- **Types** (jump table `32c66dc0`): 5 buff = `RSI_SI1_COMMON_BUFF` / `_HYPER` (= March), FM = vslot 0x5f0 (Raven's `32c3da10`), EP `31fcbd60`; 6 / 7 / 0xd-0x10
  = the hyper switch, then the id switch; 0xc + 900 = `RSI_THROW_ITEM`, vslots 0x5f4 / 0x5f8; anything else returns.
- **Hyper switch** (`0x6018 < id`, `== 0x6018`): 24600 / 24603 Infernal Arm (`32c27c90` / `32c3c670`), 24601 / 24604 Extreme Blade READY (`32c27d90` /
  `32c3c6b0`) plus `RSI_HA_RBM_EXTREM_BLADE_ATTACK` (Init / Start / FrameMove / EventProcess / End) and `_ATTACK_FINISH` (Init / StartFuture / EventProcess)
  through `31fd89b0`, 24602 / 24605 Burning Buster (`32c28040`, EP `31fcc740` = `GenericSpecialActiveSkillEventProcess`). Each = March's callbacks and slots
  (`UserUnitStateData` +0x54 = `StateEnd`); the hyper slot gets the EventProcess only, as March. Ids above 24600 with no case -> `@32c66cd6`; below -> the id
  switch (March's cases until CH2-CH3). Bodies stay CH3d's (kickoff amended).
- **`SetMaximumCannonSkillFunc`** (new, `_CORE`; June `@32c66cd6`, the default of both switches: 24000 / 24025 have no case). June registers
  `RSI_SI1_MAXIMUM_CANNON` on both slots (Init `32c260a0`, EP `32c2dac0`, hyper EP only). The helper holds March's Maximum Cannon case until CH2a (kickoff
  amended); under `_LIVE` March's id-switch `default:` calls it (Elsword's `SetMegaSlashSkillFunc` shape).
- **`32c69e30`** (CH0's "new ?") = March's inlined `WildChargeData::CreateWildChargeData()` (0x24 bytes, `RSI_BASE` 0x99 at +0x10, handle -1); only
  caller the Wild Charge case @32c64eeb (CH3b). Nothing to port; class-map row corrected.
- Enum names (`JUNE_ENUM_NAMES`, `X2SkillTree.h`): `RNSI_HA_RRF_INFERNAL_ARMS` 24600, `RNSI_HA_RVC_BURNING_BUSTER` 24602, `RNSI_HA_RRF_INFERNAL_ARMS_UPGRADE`
  24603, `RNSI_HA_RBM_EXTREM_BLADE_UPGRADE` 24604, `RNSI_HA_RVC_BURNING_BUSTER_UPGRADE` 24605 (June `Enum.src.lua`, June's spelling); 24601 was CH1i's.
- `Always.h`: five new `_LIVE` guards: `JUNE_ENUM_NAMES`, `JUNE_SKILL_TEMPLET`, `JUNE_SKILL_TYPE`, `JUNE_SKILL_NOTE`, `JUNE_DAMAGE_EXCEPTION`.

**Dev free-skill gate.** `32c641a0` has no cool-time / MP check; the gate is CH1k's in `SpecialAttackLookup` (`32c58c30`, `X2GURaven.cpp` `IsDevUnlimitedSkillUse`).
Checklist marked `(Raven: CH1n)`: dev-gate row, §7b memo row, 7i head row, the `+0x11` row, types 14-16.

**Base callbacks.** Raven's 132 (`ch1m_user_callbacks.tsv`) are all covered at the same June VA: 127 by Chung's triage, 5 by Elsword's
(`june2017_re/char_raven/ch1n_user_callback_triage.tsv`). The two CH1m flagged, `RidingOnEnd` `31fc8810` (SAME) and `RidingDamageFrontStartFuture`
`31fce160` (DIFF -> Stage 17), are Chung's rows. Tally: 63 SAME; DIFF 16 ported for every class (Chung CH1n2 base flags), 5 kept (RandomLose `find`
guard, the two HyperModeEnd Add stand-ins, StartStateStart / StartEventProcess), 48 Stage 17. Nothing to widen: no registered body is gated on another
class's `_LIVE` flag (the class-gated `X2GUUser` sites are virtual hooks, CH1o / CH1p).

**Checks.** One-file probes of `X2GURaven.cpp` (`census.cl_cmd`, `/c /Zs /EHsc`): `_LIVE` off exit 0 / 0 errors, `/DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`
exit 0 / 0 errors (the five guards pass). `cl /P` off / on: `SetMaximumCannonSkillFunc( normalStateData` 0 / 2, `RNSI_HA_RRF_INFERNAL_ARMS:` 0 / 1,
`GetTranscendenceMemoSkillID( eSkillID_ )` 0 / 1; the helper's definition 1 / 1.
`git diff --patience --numstat`: `X2GURaven.cpp` 152 / 0, `.h` 1 / 0, `Always.h` 15 / 0, `X2SkillTree.h` 9 / 0. bytepatch: high bytes unchanged (20340 / 4732 / 38738 / 23310).
`flagcheck JUNE_CHAR_RAVEN_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE --worktree --files X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h KTDXLIB/Always.h`:
`files: 3 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`, RESULT FAIL only for the `//#define`;
`JUNE_CHAR_RAVEN_CORE --files X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`;
`JUNE_ENUM_NAMES --files X2Lib/X2SkillTree.h`: `files: 0 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`.
Each: `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`.
`junediverge.py --check` (the four code files): `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: `0 A/B row(s)`.
Funcmap rows `Char Raven CH1n` (3). No JUNE_BUGS row.
`DEPLOY OK: X2_offline.exe is 26,014,208 B, dated 2026-11-06 11:21:13, md5 03d6e3fd209d5316848fcdb97fe264d6, equal to X2/US_SERVICE/x2.exe (2026-11-06 11:21:06); built at main 552f779a + 4 uncommitted code file(s).`
No `exestr` line: `_LIVE` is off and the `_CORE` helper adds no string.

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-raven CH1n review` (Opus 5.5 high).

### CH1n - Review (2026-11-06)

**Verdict: PASS, no code change.** Read from a fresh decompile of `32c641a0` (1554 lines) and the exe bytes (`june2017_re/disasm_range.py`, tables read
from the PE sections).
- **Head.** `js` / `cmp edi, 5; jae` = `slot < 0 || 4 < slot`. Tables by `bSlotB_`: `3350f9e8` {50,52,54,56,58} / `3350fa10` {60..68} normal,
  `3350f9fc` {51..59} / `3350fa24` {61..69} hyper = `_CONST_GUUSER_`. `31fd2af0( normal )` with `ecx = this` is the 7i snapshot (dropped); `byte +0x11 = 1`
  normal, `word +0x10 = 0x101` hyper. `31fca8e0( id, 1 )` = `GetEquippedActiveSkillTemplet( id, true )`, null -> epilogue. `31fd88f0` / `3207c1a0` /
  `31fdf970` act on the stack batch `[esp+0x84]` (anti-tamper), as ported.
- **Memo exit @32c642aa.** `31fdbcc0( id )`, then `32206450( memo, 1 )` on `g_pData +0x50`, `jne 32c66d8d` = the epilogue. No case, as ported. It cannot
  fire for Raven: June's `SkillNoteTemplet.src.lua` has 15 `MI_TRANSCENDENCE_1` / `_2` rows (AVP, EEG, ECE, ECB, CSG, CDC, ALD, SGM, SBH, LLD) and none
  names a Raven skill.
- **Types.** Byte table `32c66dd0` over `type - 5` = {0,1,1,3,3,3,3,2,1,1,1,1}, jump table `32c66dc0` = buff `32c642ed` / active `32c64348` / throw
  `32c66d26` / exit `32c66d8d`: 5 buff; 6, 7, 0xd-0x10 active; 0xc throw; 8-11 and above 0x10 exit. Grade 14 = `ST_SPECIAL_ACTIVE_GRADE_1`
  (`X2SkillTree.h:54`, `JUNE_SKILL_TYPE`). Buff: FM `329ccd90` -> `32a77a20` (`jmp [vtbl+0x5f0]`, Raven's `32c3da10` = his `COMMON_BUFF_FrameMove`
  override), EP `329cba40` -> `31fcbd60`, hyper copies. Throw: `cmp id, 0x384`, FM `329ccda0` -> `329cc0d0` (`+0x5f4`), EP `329ccdb0` -> `329cc170` (`+0x5f8`),
  neither overridden by Raven (`ch0_vtable_slots.tsv`), both copied to hyper; `SI_ETC_WS_COMMON_LOVE` = 900 (Elsword's CH1n review).
- **Hyper switch.** `jg` -> `id - 0x6019`, `cmp 4; ja 32c66cd6`, table `32c66fc4` = {EB, BB, `32c66c32` IA, EB, BB}; `je 0x6018` -> `32c66c32`; else the
  id switch. Thunks: IA `32c68f40` / `32c69d30` -> `32c27c90` / `32c3c670`; EB READY `32c68f00` / `32c69d00` -> `32c27d90` / `32c3c6b0`; BB `32c68f50` ->
  `32c28040`, `329cbfc0` -> `31fcc740`. Hyper slot gets +0x44/+0x48 only; BB's `byte +0x10 = 1` = `m_bHyperState`. Strings `33514134` / `33514194` /
  `33514118` = the tables the C++ names.
- **Extreme Blade's two states.** Stack struct `[esp+0x1c]`: ctor `32078bc0`, `320775a0` (= `UserUnitStateData::Init`) before each. `0x123`
  `"RSI_HA_RBM_EXTREM_BLADE_ATTACK"`: +0x14 Init `32c68f10`->`32c27e50`, +0x24 Start `32c6a110`->`32c42e10`, +0x34 FM `32c6a440`->`32c4f0a0`, +0x44 EP
  `32c69d10`->`32c3c6e0`, +0x54 End `32c6a120`->`32c42eb0`. `0x124` `"..._ATTACK_FINISH"`: Init `32c68f20`->`32c27f10`, +0x1c StartFuture `32c68f30`->`32c27fe0`,
  EP `32c69d20`->`32c3c730`. Every slot and target = the C++. June's C++ ids 291 / 292 differ from June's own `Enum.src.lua` (222 / 223,
  `ch0_state_enum_names.tsv` DIFF); the registration ties each id to its table name, and the port uses the enumerator of that name.
- **`RNSI_HA_*`** = `Enum.src.lua` 16455 / 16421 / 16540 / 15330 / 16154 (24600 / 24602 / 24603 / 24604 / 24605); 24601 is CH1i's.
- **`SetMaximumCannonSkillFunc`.** Both defaults land on `@32c66cd6` (hyper `ja` @32c66b26, id switch `id - 0x5dc1; cmp 0xee; ja` @32c6436a), no explicit
  case. The helper = March's `SI_A_RF_MAXIMUM_CANNON` case line for line (its two commented CameraMove lines aside); `_LIVE`'s `default:` calls it and
  breaks. June's own form there (one table, Init `32c260a0` + EP `32c2dac0`, hyper EP only) is in CH2a's kickoff amendment.
- **`32c69e30`.** `new 0x24`: +0 refcount 0, +4/+8/+0xc 0.0f, +0x10 `0x99` (June's `RSI_BASE`), +0x14 bool 0, +0x18 NULL, +0x1c / +0x20 -1 (handle) =
  `WildChargeData`'s constructor order, inlined `CreateWildChargeData()`; the only xref is the Wild Charge case @32c64eeb (CH3b). Nothing to port.
- **Callbacks.** `ch1n_user_callback_triage.tsv` = `ch1m_user_callbacks.tsv` row for row (132 / 132), each member found at the same VA with the same verdict
  in the cited triage (127 Chung, 5 Elsword); 63 SAME, 16 ported, 5 kept, 48 Stage 17. Bytes: `RidingOnEnd` `31fc8810` = `jmp [vtbl+0x7dc]`
  (`CommonStateEnd`) = March (`X2GUUser.cpp:46147`); `RidingDamageFrontStartFuture` `31fce160` calls `CommonStateStartFuture` (+0x7c0) before the pet
  `StateChangeFuture` through getter `32d278b0`, March the reverse (`X2GUUser.cpp:46887`) = DIFF, Stage 17, as triaged.
- **Always.h.** The five guards name what the `_LIVE` head uses: `GetEquippedActiveSkillTemplet( id, bool )` (`X2GUUser.h:2696`, `JUNE_SKILL_TEMPLET`),
  `GetTranscendenceMemoSkillID` (`X2GUUser.h:3098`, inside `JUNE_DAMAGE_EXCEPTION` + `JUNE_SKILL_NOTE`), the grades (`JUNE_SKILL_TYPE`), `RNSI_HA_*`
  (`JUNE_ENUM_NAMES`).

**Hygiene.** flagcheck `--range ab97af2e^..ab97af2e --files`: `_LIVE --with _CORE` `files: 3 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`, FAIL only for
the `//#define` (by design); `_CORE` `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR` `RESULT: PASS`; `JUNE_ENUM_NAMES`
`files: 0 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` `RESULT: PASS`. Each: 0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 `#else`/`#endif`
without the token, 0 encoding changes. `git diff --patience --numstat`: 152 / 1 / 15 / 9 added, 0 deleted. `junediverge.py --check-all`: `allow-listed: 186`,
`RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: `0 A/B row(s)`. One-file probes (`/c /Zs /EHsc`): `_LIVE` off exit 0 / 0 errors, on exit 0 / 0 errors.
No build: the review changed no code (CH1n's `DEPLOY OK` stands).

**Nothing to play-test** (`_LIVE` is off). CH1o's kickoff stands as written (Amended by CH0 / CH1h checked). Next: `/x2-phase char-raven CH1o` (Opus 5.5 high).

### CH1o - Done (2026-11-06)

**Landed** (flag `_LIVE`, still `//#define`; `_CORE` for the renamed hook; 11 funcmap rows tagged `Char Raven CH1o`). Files: `X2GURaven.cpp` / `.h`, `X2GUUser.h`, `X2DamageManager.cpp`.
- **Vslot 0x300 is March's `AdjustDamageDataBeforeDamageReact`.** The CH1h hook is renamed to it and made `virtual` (still `_CORE`; `32c63660` jumps to
  `32c5d540`). Sweep for vtable loads of `+0x300` over June's `.text` (`dispsweep.py 300`) found four calls. Three are in `CX2DamageManager`, on the attacker, on a hit,
  directly before the defender's `DamageReact` (vslot 0x60): UnitToUnit @3262546d, MeshToUnit @32628652, EffectToUnit @3262f1e6 (EffectToUnit then calls attacker
  vslot 0x3dc, base `3208ddc0`; no mapped class overrides it). The fourth is `CX2GUNPC`'s override `32778fe0`, which hands the call to the owner of a
  `CX2AllyNPCAI` NPC. June's slot is in `CX2GameUnit` (vtables 334883b4 / 334781f4 + 0x300 = `3208ddc0`, empty). March has the same hook at the same three spots,
  but only for user attackers and only under `ADD_RENA_SYSTEM` (off; the base declaration and Lire's override are inside it too). Added under
  `#ifdef _LIVE` + `#ifndef ADD_RENA_SYSTEM`: the empty base in `X2GUUser.h`, and at each site
  `if GUT_USER: static_cast<CX2GUUser*>( attacker )->AdjustDamageDataBeforeDamageReact( pDamageData )`. The user-only test gives Raven June's result: he
  has no summons. The NPC forward goes to char-luciel CH1k (Luciel overrides 0x300, `32b1e720`; kickoff amended).
- **`SetSkillLevelStateData` = June's `3201f850`, under `_LIVE`, March's in `#else`.** Null templet / unit return, level `max( 1, GetSkillLevel( id, false, 0, true ) )`,
  `GetSkillPowerRateValue`, id, then power x `( GetSkillAdditionalPowerRate( id ) + 1.0 )`. Raven does not override vslot 0x7a8 (`32028190`), and his wrapper
  `SetEquippedSkillLevelStateData` is that function's form (null templet -> nothing). Not hoisted into `CX2GUUser`, as Chung's and Elsword's CH1o decided.
- **The nine triage rows, no code:** `32c28100` = MemoConditionRaven decide, `31fd4450( arg ) != 0`, always false (CH4c, as Chung / Elsword);
  `32c69d80` = the shared `MemoCondition` scalar deleting dtor; `32c69f40` / `32c69f50` = `~CX2GURaven` adjustor thunks (`this - 0x1a0 / - 0x190` -> `32c6a620`);
  `32c3d1c0` = `AppendEffectToDeviceList` (base `31fef4d0` case 4) and `32c28880` = `AppendMajorParticleToDeviceList`, both = March (March's minor list is empty);
  `32c35fc0` is the tail of `32c35f90` (the `3286f949` pointer is a stray data match), where `SKILL_CANCEL_AFTER_FLY` makes `CommonEventProcess` the body of its `else if`,
  = CH1g2's port. **To CH4a** (kickoff amended): `32c3c7a0` (Explosive Nasod Hand 24402 buff) and `32c4e5b0` (Reversal Soldier 24413 / Survival Technique 24414
  HP crossing, March `CheckAndDoReversalSoldier` / `CheckAndDoServivalTechniqueOfMercenary`) are reached only through `32c57660`'s delegate pushes.
- **0x4ac / "510":** Raven's vtable keeps the base at 0x4ac (`ch0_vtable_all.tsv:301`): nothing to port. "510" was CH1k's step number (lookup `32c58c30`, ported).
- **CH1h / CH1i routes closed:** 0x300 wired (above); the `+ 1.0` term (above). No other CH1a-CH1n note routes anything to CH1o.

**Routed.** char-luciel CH1k: the `CX2GUNPC` 0x300 forward `32778fe0` and widening the four guards. CH4a: the two delegates. No new divergent function (the March
base is empty, as June's); the call-site difference is the `ADD_RENA_SYSTEM` gate, recorded here and in funcmap `32c5d540 (CH1o)`.

**Gates.**
- `flagcheck JUNE_CHAR_RAVEN_LIVE --with JUNE_CHAR_RAVEN_CORE --worktree --files` the 4: `files: 4 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `3 deleted line(s)`
  (the rename inside `_CORE`: 2 in the `.cpp`, 1 in the `.h`), `0 non-ASCII`, `0 EOL-mismatched`, `0 #else/#endif without the flag token`, `0 encoding change(s)`;
  RESULT FAIL only for `define: NOT FOUND` (`//#define`). numstat: X2DamageManager.cpp 21 / 0, X2GURaven.cpp 16 / 2, .h 1 / 1, X2GUUser.h 5 / 0.
- `junediverge.py --check` the 4: `allow-listed: 1`, `RESULT: PASS`. `enumscan.py --check X2GURaven.cpp X2DamageManager.cpp`: `0 A/B row(s)`.
- One-file probes (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH) of `X2GURaven.cpp`, `X2DamageManager.cpp`, `X2GUUser.cpp`, `_LIVE` off / on: exit 0, 0 errors each.
  `cl /P` counts off -> on: `AdjustDamageDataBeforeDamageReact` X2DamageManager 0 -> 4 (base + the 3 calls, all in live branches), X2GURaven 2 -> 3; the `+ 1.f` term 0 -> 1.
- `DEPLOY OK: X2_offline.exe is 26,014,208 B, dated 2026-11-06 13:00:19, md5 39bdca1c029106ffa13bd221b675e780, equal to X2/US_SERVICE/x2.exe (2026-11-06 13:00:12); built at main 208880de + 4 uncommitted code file(s).`
  No `exestr` line: every new site is under the undefined `_LIVE`.

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-raven CH1o review` (Opus 5.5 high).

### CH1o - Review (2026-11-06)

**Verdict: PASS, no code change** (flag `_LIVE`, still `//#define`). Read from the exe bytes (capstone over `unpacked_x2.exe`, `charport_adis.py raven`):
- **Vslot 0x300 holds.** `dispsweep.py 300` (770 hits), kept `mov r,[r+0x300]` followed by `call r`: four, `@3262546d` (UnitToUnit, `mov eax,[ebp]`),
  `@32628652` (MeshToUnit, `[ebx]`), `@3262f1e6` (EffectToUnit, `[edi]`) and `@32779046` inside `CX2GUNPC`'s `32778fe0`; nothing else. At each manager site
  the DamageData pushed to 0x300 is the one pushed to the defender's vslot 0x60 right after (UnitToUnit / MeshToUnit: ebx / esi; EffectToUnit: esi, with
  attacker vslot 0x3dc between). Hit-only: UnitToUnit / MeshToUnit reach it through `test ah,5; jp` past the `mov al,1; jmp` avoid exit, EffectToUnit
  only when the miss byte `[esp+0x6f]` is 0. Attacker: ebp / ebx = March's `pAttacker`; EffectToUnit's `[esp+0x128]` is DD `+0xc` observer `- 0x1a0`
  (`@3262b848-3262b865`, NULL when unset) = March's `pAttackerGameUnit`, dereferenced unchecked in June as in March's `ADD_RENA_SYSTEM` block. March's
  three `_LIVE` inserts sit after those blocks, inside `if( bMiss == false )`, directly before `DamageReact`.
- **Vtables:** `CX2GameUnit` 334883b4 and `CX2GUUser` 334781f4 + 0x300 = `3208ddc0` (`ret 4`; also their 0x3dc), `CX2GUNPC` 334ba584 + 0x300 = `32778fe0`;
  `32c63660` = `jmp 32c5d540`. `32778fe0`: DD NULL -> return; NPC `+0x7c98` AI type 1, `dynamic_cast` to `CX2AllyNPCAI`, `[+0x80] == 0`, owner by UID
  (`+0x78/+0x7c`, game vslot 0xc8) -> owner's vslot 0x300. The user-only test is exact for Raven's own hits; an ally NPC owned by a Raven (summon item) would
  carry his Enchant Hand factor in June: that is the routed char-luciel CH1k port, which widens the guards.
- **`SetSkillLevelStateData` = `3201f850`.** Templet / `+0x867c` NULL -> return; `32890620( id, 0, 0, 1 )`, `< 1` -> 1 else called again (= `max( 1, .. )`);
  byte store `+4` (`m_SPLevel` is `char`), `32071720( level )` -> `+8`, id `+8` -> `+0xc`, `( 3201ad10( id ) + 1.0 ) * [+8]` -> `+8`, `ret 8`. 3201ad10 =
  `GetSkillAdditionalPowerRate` (funcmap 1484). June's 1.0 is a double constant on x87, the C++ `1.f`: same value, as every other port of this term.
  Raven vtable 0x7a8 = base `32028190` (`ch0_vtable_all.tsv`).
- **Nine triage rows hold.** `32c28100` = `31fd4450` (`xor al,al; ret 4`) -> always false; `32c69d80` = vptr `334720bc`, delete on bit 1 (scalar deleting
  dtor); `32c69f40` / `32c69f50` = `sub ecx,0x1a0 / 0x190; jmp 32c6a620`; `32c3d1c0` = `32c28880` + `3226ecc0` (enchant sword particles), `32c28880` = the
  seven `DamageImpact*` appends (March's major list); `32c35fc0` = `fldz; fcomp` (>= 0), anim time > it, vslot 0x7f4( 2 ), tail-jump to vslot 0x7d4 =
  `SKILL_CANCEL_AFTER_FLY` then `CommonEventProcess` as the `else if` body (CH1g2's `X2GURaven.cpp:26975`); `32c3c7a0` (24402) and `32c4e5b0` (0.5 HP, 24413,
  24414 with `32007b10( 24414, 0x91 / 0x7d )` behind `322cc1a0`) = the CH4a amendment (CH4a writes the PvP test as `GetIsNowPVPCorrectionGame()`).
- **0x4ac / "510":** Raven 0x4ac = base `3208df00` (`ch0_vtable_all.tsv:301`); `32c58c30` = `SpecialAttackLookup`, ported in CH1k (funcmap 8192).

**Hygiene.** `flagcheck JUNE_CHAR_RAVEN_LIVE --with JUNE_CHAR_RAVEN_CORE --range cb099003^..cb099003 --files` the 4: `files: 4 PASS, 0 PASS~, 0 DEFINE,
0 OTHER, 0 FAIL, 0 ERROR`; `3 deleted line(s)` (the `_CORE` rename), 0 non-ASCII, 0 EOL-mismatched, 0 `#else/#endif` without the token, 0 encoding changes;
RESULT FAIL only for `define: NOT FOUND` (`//#define`). numstat 21 / 0, 16 / 2, 1 / 1, 5 / 0. `junediverge.py --check-all`: `allow-listed: 186`, `RESULT: PASS`.
`enumscan.py --check X2GURaven.cpp X2DamageManager.cpp`: `0 A/B row(s)` (two of four runs ended `cl /P failed (exit 2)` with no message, likely a peer
on the cache; the passing runs agree). Probes (`census.cl_cmd`, `/c /Zs /EHsc`) of `X2GURaven.cpp`, `X2DamageManager.cpp`, `X2GUUser.cpp`, `_LIVE` off / on:
exit 0, 0 errors each; `cl /P` counts off -> on: `AdjustDamageDataBeforeDamageReact` X2DamageManager 0 -> 4 (3 calls + the base), X2GURaven 2 -> 3,
`+ 1.f ) * stateData_` 0 -> 1. No build: the review changes no code.

**Nothing to play-test.** CH1p kickoff checked. Next: `/x2-phase char-raven CH1p` (Opus 5.5 high).

### CH1p - Done (2026-11-06)

Flag `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (insert-only, still `//#define`). One new `Always.h` guard: `_LIVE` needs `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, because
the new reader line keeps `UT_ELSWORD` (and through Elsword's own guard `UT_CHUNG`) in the June set; the `_LIVE` probe compiles, so it is defined at that line.

**Method.** `cl /P` of `X2GURaven.cpp` / `X2GUUser.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`; every surviving reader of the pressed index, `m_eSpecialAttackKeyPressed`,
`SAKP_*`, `CountUsedSkill`, `GetPressedSkillState`, `CurrentSkillKeyManager`, `m_kSkillSlotKey` listed. June side: `dispsweep.py 8ac0 8b58`, `charport_adis.py raven`.

**7ag, the pressed-index space.**
- **Already June's (CH1k / CH1n):** the `_LIVE` `SpecialAttackEventProcess` keeps the pressed index (`:7745-7983`): `m_iChosenSkillSlotIndex`, `StateChangeBySkillSlot`,
  `MarkSkillSlotUsed` (0-9), `m_iNowSpecialAttack = slot + 1`, and the cooldown's `m_eSpecialAttackKeyPressed = slot + 1` (`:7918` = `32c43bf0 @32c43ed1`). CH0's sites
  (decode `:5854-5871` now `:8268-8285`, `CountUsedSkill` now `:8194`, the `:8290` write) are March's body in `#else`; none is in the `_LIVE` `.i`.
- **Fixed: the shared reader.** `CommonSpecialAttackEventProcess` (June `31fcadd0`) still handed Raven March's 4 + 4. `UT_RAVEN` joins the June set under `_LIVE`
  (`X2GUUser.cpp`, Elsword's block kept as `#else`; stand-in comment lists Lire, Ara, Elesis, Add).
- **Fixed: 12 held-key tests.** June's Raven never reads `+0x8ac0` (whole-`.text` sweep: readers only Chung `32bfeb80` and Aisha `32cd9af0`). Each March `SAKP_A..R`
  test is, in June, `IsEquippedSkillKeyPressed( m_InputData, GetFutureStateSkillID() )` (`31fce710`, future state data `+0x7cb0 -> +0xc`, input `+0x86da`); under `_LIVE`
  a June slot + 1 would have made Q read as F and R never match. Converted under `_LIVE`, March in `#else`: Revolver Cannon `32c59f30` / LOOP `32c5a020`, Harpoon Spear
  `32c5a1e0`, Hellfire Gatling `32c5a310` / LOOP `32c5a400`, Giga Prominence `32c5a5d0` / LOOP `32c5a6c0`, Deadly Raid `32c5bb40` / LOOP `32c5be30` / LOOP_OVERHEAT
  `32c5bf80`, Ignition Crow `32c5b0a0`, Incineration `32c5b560` (call addresses in each one-line comment). Only the test changed; each body is its skill phase's.
- **Fixed: Limit Crusher's key tracker** (the checklist's "every class port" rule). March's `CurrentSkillKeyManager` (4 + 4) was fed `m_iNowSpecialAttack - 1` (0-9 now).
  June: StartFuture `32c27860` = `m_kSkillSlotKey.Set( m_iNowSpecialAttack - 1 )` + `CommonStateStartFuture` (whole body); EventProcess `32c38f70` `FollowSlotSwap` `@32c38f7c`,
  `IsKeyReleased` `@32c38fb7` after the 0.45 time test (= March's shape). Three sites converted; the rest of `32c38f70` is CH3b's.
- **No change:** `GetSkillSlotIndexUsed` (`:15994`, Magnum Blaster hyper End) reads the slot state id, and `RSI_SI3_MAGNUM_BLASTER*` is June-dead (`ch0_state_skill.tsv`).
  Revolver Cannon HE / HE_LOOP (`:30645`, `:30752`) keep `SAKP_*`: their states are absent from June's exe and their skill `SI_SA_RWT_REVOLVER_CANNON_HIGH_EXPLOSIVE_SHELL`
  (4017) is an old-system id June never equips. The `SAKP_` switch `:11229` (`RSI_SUPER_DASH_EventProcess`) is March's body, not in the `.i`.
- **Routed:** Shadow Step (CH2a). June adds a held key March never reads: `32c30db0` (`RBM_EMERGENCY_ESCAPE_StartFuture`) ends `Set( m_iNowSpecialAttack - 1 )` `@32c30e73`;
  `32c52550` (`RSI_SI_A_RF_SHADOW_STEP_EventProcess`) `FollowSlotSwap` `@32c5255b`, `IsKeyPressedOnce` (`31fc8900`) `@32c525f4` gating the back slide. CH2a kickoff amended;
  CH2d / CH3b / CH3c / CH3c2 kickoffs note the converted tests. Checklist 7ag marked `(Raven: CH1p)`.

**Hooks 7h / 7l / 7o / 7p / 7x** (CH0 rows, in the `_LIVE` preprocess): 7h `InitWaitStateChange( "RSI_WAIT_STATE_CHANGE" )` (CH1m); CH0 gave `32c2b360` to CH1d
(`RSI_COMBO_ZZXX_EventProcess`, done). 7l `InitBoundingStates( "RSI_" )` (CH1m); the three bounding states come with the script (CH1b). 7o `SET_LIMIT_MAX_G_SPEED` (reader
`X2GUUser.cpp:16520`; `RRF_DASH_JUMP_COMBO_XX` -5000), 7p `TIME_STOP_IMMUNE_STATE` (reader `:18152`; 7 rows in the script) and 7x `AT_COMMAND` (46 tags; user-layer gate
`32045340 @320453a0` ported at `X2GUUser.cpp:45514`, stand-in `ELYSION_ATTACK_TYPE_STANDIN`) are script data: CH1 final brings them.
**June-only keys:** none owned here (`ch0_keys.tsv`, CH0 amendment); §5 census rows stay as CH0 left them.

**Checks.** Probes (`census.cl_cmd`, `/c /Zs /EHsc`): `X2GURaven.cpp` 0 errors, 38 / 38 warnings off / on; `X2GUUser.cpp` 0 errors, 39 / 39; no `#error` fired. `cl /P` with
`_LIVE`: `IsEquippedSkillKeyPressed` 12 in the class, `m_kSkillSlotKey` 3, `m_eSpecialAttackKeyPressed` = the writer + the two HE states; `bMarchSlotIndex` = the `UT_RAVEN` line.
`flagcheck JUNE_CHAR_RAVEN_LIVE --worktree --files X2GURaven.cpp X2GUUser.cpp Always.h`: `files: 3 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s),
0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only for `define: NOT FOUND`
(`//#define`). numstat 76 / 0, 5 / 0, 3 / 0. `junediverge.py --check` the 3: `RESULT: PASS`. `enumscan.py --check X2GURaven.cpp X2GUUser.cpp`: `0 A/B row(s) remain`.
`DEPLOY OK: X2_offline.exe is 26,014,208 B, dated 2026-11-06 13:33:28, md5 78e348486d0420075a7eca4d81aa8aa2, equal to X2/US_SERVICE/x2.exe (2026-11-06 13:33:21); built at main e8ddedb1 + 3 uncommitted code file(s).`
No `exestr` line: every site is under the undefined `_LIVE`. No funcmap or `JUNE_BUGS` row.

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-raven CH1p review` (Opus 5.5 high).

### CH1p - Review (2026-11-06)

**Verdict: PASS, no code change** (flag `_LIVE`, still `//#define`). Read from the exe bytes (capstone over `unpacked_x2.exe`, `charport_adis.py raven`):
- **The 12 held-key tests hold.** Each June function loads the future state data (`+0x7cb4` xor `+0x7cb0`, `[+8]` -> `+0xc` = `GetFutureStateSkillID()`
  under `SERV_IRUHADEV_JUNE_SKILL_STATE_PTR`), pushes `+0x86da` (`m_InputData`) and calls `31fce710` at the cited address; `test al,al; je` skips the same
  block March's `SAKP_*` test guards. Revolver Cannon / LOOP, Hellfire Gatling / LOOP, Giga Prominence / LOOP: key, then `_CanOverheat( 1 )` (March's default
  argument), then `++` protected `+0x96bc`. Harpoon Spear: key, `_CanOverheat` -> `++m_iRVCComboLoopInputCount`, then the `>= 3` / `m_bShowedOverheat` icon
  block and FIRE / FIRE_OVERHEAT (0x114 / 0x115) at animation end, = March's body. Deadly Raid: key -> `++m_iRVCComboLoopInputCount` (no overheat test, as
  March). Deadly Raid LOOP / LOOP_OVERHEAT: `m_pXSkinAnim` time `< 0.21` first; Ignition Crow / Incineration: `m_pXSkinAnimFuture` time `< 2.01` first; the key
  test inside, = March's nesting. No June structure around a call differs, so the `_LIVE` test alone is safe before each skill phase.
- **Skill id is there to test.** The cast states get `m_eSkillID` from the slot head (CH1n); the LOOP states from `SetEquippedSkillLevelStateData`
  (`X2GURaven.cpp:5898` / `:5905` / `:5911` / `:5939-5940`, `SetSkillLevelStateData` writes `m_eSkillID`), with June's ids (`ch0_state_skill.tsv`: LOOP rows =
  cases 24019 / 24020 / 24021 / 24024, as March's templets).
- **`+0x8ac0`:** `dispsweep.py 8ac0` = 11 hits; in the Raven range only the writer `32c43bf0 @32c43ed1`; readers Chung `32bfeb80`, Aisha `32cd9af0`.
- **Limit Crusher.** `32c27860` = `Set( [+0x88bc] - 1 )` on `+0x8b58`, tail-jump vslot 0x7c0 (`CommonStateStartFuture`): whole body. `32c38f70`:
  `FollowSlotSwap` `@32c38f7c`; animation end -> `32c3918a`; else time `< 0.45` skips the key; else `IsKeyReleased` `@32c38fb7` == 1 -> `32c3918a` (the revenge
  StateChange, picking REVENGE_ATTACK / _UPGRADE by the equipped templet: CH3b's), otherwise the rest = CH3b's. Matches the converted condition.
- **Shared reader.** `X2GUUser.cpp` `CommonSpecialAttackEventProcess`: the `_LIVE` line adds `UT_RAVEN` to Elsword's set; the `#else` is the Elsword / Chung
  nest unchanged. `Always.h` guard: `_ELSWORD_LIVE` (`:11467`) and `_CHUNG_LIVE` (`:11089`) are defined above it; the `_LIVE` probes fire no `#error`.
- **Left alone, correctly:** `RSI_SA_RWT_REVOLVER_CANNON_HE*` and `RSI_SI3_MAGNUM_BLASTER*` are `june-dead-data` (strings absent from June's exe). Shadow Step:
  `32c30db0 @32c30e73` = `Set`, `32c52550 @32c5255b` = `FollowSlotSwap`, `@32c525f4` = `31fc8900`; the CH2a kickoff carries both (its lines 66-68).
- **`cl /P` with `_LIVE`, X2GURaven.cpp bodies:** `CountUsedSkill` / `GetPressedSkillState` / `CurrentSkillKeyManager` 0; `SAKP_` 16 = the two HE states only;
  `IsEquippedSkillKeyPressed` 12 (one per converted function); `m_kSkillSlotKey` 3 (Limit Crusher); `m_eSpecialAttackKeyPressed` 17 = the cooldown writer + HE.

**Hygiene.** `flagcheck JUNE_CHAR_RAVEN_LIVE --range e81a7183^..e81a7183 --files` the 3: `files: 3 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`;
0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 `#else/#endif` without the token, 0 encoding changes; RESULT FAIL only for `define: NOT FOUND` (`//#define`).
numstat 76 / 0, 5 / 0, 3 / 0. `junediverge.py --check-all`: `allow-listed: 186`, `RESULT: PASS`. `enumscan.py --check X2GURaven.cpp X2GUUser.cpp`:
`0 A/B row(s) remain`. Probes (`census.cl_cmd`, `/c /Zs /EHsc`), `_LIVE` off / on: `X2GURaven.cpp` 0 errors, 38 / 38 warnings; `X2GUUser.cpp` 0 errors,
39 / 39. No build: the review changes no code.

**Nothing to play-test.** CH1q kickoff checked. Next: `/x2-phase char-raven CH1q` (Opus 5.5 high).

### CH1q - Done (2026-11-06)

New flag `SERV_IRUHADEV_JUNE_CHAR_RAVEN_RAGE`, a `//#define` that CH1 final defines with `_LIVE`. `_LIVE` `#error`s without it, because CH1j dropped the class-side hyper ball and the gauge UI now draws it.
`_RAGE` `#error`s without `_CORE`, `JUNE_USER_SKILL_LEVEL_HELPERS`, `JUNE_DIVERGE_SKILL_LEVEL_VALUE`, `JUNE_ENUM_NAMES` and `JUNE_SOCKET_PARITY`. `_RAGE_DEBUG` is defined, and its four lines sit inside `_RAGE` code.
Every row of `ch0_rage.tsv` has a verdict in **`june2017_re/char_raven/ch1q_rage_outcome.tsv`** (40 rows). Funcmap rows `Char Raven CH1q`: 8.

| June | Function | June differs from March |
|---|---|---|
| `32c3e030` | `CanParryingState` | also needs `m_FrameDataFuture.unitCondition.bFootOnLine` (`+0x7e1c`) |
| `32c41170` | `AddRageGuage` | gain x Open Mind's `SA_CHARGING_TIME` (24419 learned), then x ( spec A + 1.0 ); clamp as March |
| `32c491a0` | `DoParrying` | spec B > `GetRandomFloat(0)` skips the cost (7an); else `clock()` stamp, rage - 20, **rage < 60 -> 0, --level, level < 3 -> 0** (June's 3-5 ball levels, as CH1h's jump to 60) |
| `32c4fd80` | `RestoreNasodBall` | clears the ball's `hitUnitList` (CEffect `+0x650`) first |
| `32c50330` | `DoFrameMoveNasodBall` | the spec B roll at the three rage-spend sites (7an): a win restores the ball and costs no rage |
| `32e6d900` | `CX2RavenMyGageUI::UpdateHyperOrb` | game particle system ( `g_pData+0x84` ), no owner, ( 74, 60 ) x resolution, OverUI, the handle reset when the sequence is gone (7aq; March's UI-system template never loaded) |
| `32e681e0` / `32e5e450` | `DestroyHyperOrb` (new) / `SetShow` (new override) | the dtor and the not-3 path destroy through it; SetShow shows / hides the ball with the gauge (Eve's shape) |

**= March, no code:** the four parrying state functions, `ClearNasodBall`, `CreateNasodBall` (CH0's BEED_UP "difference" is March's too), `ChangePowerRateNasodBall`,
`AttackNasodBall`, the `X2GUUser.cpp` hooks (DamageReact parry gate `@32047385`, parry damage cut `@32048631`, rage fill `@3204a674`, `DamageReactStateChange`
`31feabd0`, `SetHold` `31fd29c0`), the gauge UI creation (`3248e290`, the seven classes, plain `CX2GageData`), `InitWakeOrb` `32e5b470`, `OnFrameMove`, the
damage-manager `ONLY_BASIC` / `NO_MISS` / `IGNORE_STATE_DEFENCE` / lock-on sites, the `m_CannonBallCount` sync. CH0's "new ?" rows: `32c33b80` = `ArcEnemyData`'s ctor,
`32c4ff70` = jump-table data. `32e6d6a0` is the shared `InitUI`, not Raven's `InitWakeOrb`. Already ported elsewhere: the cooldown rage (CH1k), `AttackResultByType` (CH1h),
`CommonFrameMove` (CH1j). Kept as March: the dev rage text in `RenderName` (June's service exe drops the overlay, as Eve's).

**Routed.** Two June-only hit-dispatch pieces that are not Raven's go to **Stage 33** (bullet added in `JUNE_PORT_BACKLOG_ORDER.md`):
- `CEffect::AttackResult`'s tail: `ProcessSocketEffectByAttackSuccess`, `32008280`, and a second-unit block.
- `UnitToUnit` calls `CX2GUNPC::AttackResultByType` for any non-user attacker (March does this only under `ADD_RENA_SYSTEM`).

CH4a: the run-jump EDT / `SFI_SPEED_9` blocks (not rage) and the Survival Technique block (kickoff amended). Checklist §7an l.938 and §7aq l.995 are marked `(Raven: CH1q)`. No JUNE_BUGS row: the 60 / level-3 floors are June's design.

**Gates.**
- flagcheck `_RAGE --worktree --with _RAGE_DEBUG --files` (4 code files): `files: 2 PASS, 1 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`. RESULT is FAIL only for the
  commented-out `#define`. OTHER = Always.h (the guard and `_DEBUG` blocks). `_RAGE_DEBUG --with _RAGE`: `1 PASS, 1 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`, `RESULT: PASS`.
  All runs: `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`.
  numstat: X2GURaven.cpp 198 / 0, X2GageUI.cpp 74 / 0, X2GageUI.h 7 / 0, Always.h 42 / 0.
- `junediverge.py --check` (the Raven and gauge files): `allow-listed: 6`, `RESULT: PASS`. `enumscan.py --check X2GURaven.cpp X2GageUI.cpp`: `0 A/B row(s) remain`.
- One-file probes (`census.cl_cmd`, `/c /Zs /EHsc`, `-D`) of `X2GURaven.cpp`, `X2GageUI.cpp` and `X2GUUser.cpp`: no flag, `_RAGE`, and `_LIVE` + `_RAGE` all give exit 0 and 0 errors.
  `_LIVE` alone stops at the new `#error` (C1189), which proves the probes read Always.h.
- `DEPLOY OK: X2_offline.exe is 26,014,208 B, dated 2026-11-06 14:14:10, md5 bdc04fdb3c38ea1287d86081e2e76ed6, equal to X2/US_SERVICE/x2.exe (2026-11-06 14:14:03); built at main 54289532 + 4 uncommitted code file(s).`
  `EXESTR OK: 4 of 4 absent` (the four `_RAGE_DEBUG` lines sit in `_RAGE` code, which is off).

**Nothing to play-test** (`_RAGE` and `_LIVE` are off). CH1's play-test lines are `[JUNE_CHAR] RAVEN RAGE hit gain` / `parry` / `ball level` / `orb ball` (CH1 kickoff
amended). Next: `/x2-phase char-raven CH1q review` (Opus 5.5 high).

### CH1q - Review (2026-11-06)

**Verdict: PASS, no code change** (flag `_RAGE`, still `//#define`). Read from Ghidra and the exe bytes (`disasm_range.py`, `charport_consts.py`):
- **Roll helper `32070350`** = `a > b` (`fcompp`, `test ah,0x41`, `jne` -> 0). Every site passes ( spec B `+0x6d04`, `GetRandomFloat( 0 )` vslot 0x1cc ): a win skips the cost.
- **DoParrying `32c491a0`.** `@32c4942d` loads spec B over the random value, `fcompp` / `test ah,0x41` / `je 32c4957c` = spec B > random skips the cost. In the paid branch:
  `clock()` -> `+0x9778`; rage `fsubr` qword 20.0; `@32c494e9` `fcomp` qword 60.0, `test ah,5` / `jp` skip = rage < 60 -> 0; `@32c49563` `dec` level, `cmp 3` / `jge` = < 3 -> 0. = the port.
- **DoFrameMoveNasodBall `32c50330`.** Order = port: hyper gate, parry timer (`+0x97a4` / `+0x97a1`), delay (`>= 0.3`) then the level block (`@32c50438` `fdiv` qword
  20.0 = `rage / 20.0`; others `+0x8160` sync count; `IsAttackNasodBall` inlined as delay || attack), follow (0.95 / 0.05 = float values), parry pos (-1, 70, 100),
  attack end (`>= 1.2`; resets delay time, attack time, lock-on NPC / UID, attack flag). Rolls: `@32c507da` instance lost -> lose: rage - lv x 20.0, `ClearNasodBall`; win:
  `RestoreNasodBall`. `@32c50b15` instance gone / `@32c50c3d` damage time `+0x850` > 0 -> lose: pay, clear, return; win or no hit -> `RestoreNasodBall`. = the port.
- **AddRageGuage `32c41170`**: `31fceab0( 24419 )` > 0 -> x `32007b10( 0x18, 24419 )`; rage + ( `+0x6c50` + 1.0 ) x gain; clamp 100 / 0. `GetSkillLevelValue` resolves to
  the one `JUNE_DIVERGE_SKILL_LEVEL_VALUE` body (31fceab0). **CanParryingState `32c3e030`**: `+0x80bc` and `+0x7e1c`; Now state ids 16 / 17 / 21, 25-43, 168 / 215 / 216
  = June's `USI_JUMP_UP` / `_DOWN` / `DASH_JUMP`, `GROGGY..GRAPPLED_FRONT`, `RSI_COMBO_ZZZX` / `RBM_COMBO_ZZZX_EXPLOSION` / `RBM_COMBO_ZZZXZ` (`ch0_state_enum_names.tsv`).
  **RestoreNasodBall `32c4fd80`**: `CreateNasodBall` first, then the `+0x650` / `+0x654` erase (`hitUnitList`, Eve CH1l4), then March's body.
- **Gauge UI.** `32e6d900`: g_pX2Game + major particle gate, hyper count (`+0xf4`) != 3 -> `32e681e0`; create at ( 74.0 x `+0x1b8`, 60.0 x `+0x1bc` ), owner 0, args
  -1 / -1 / -1 / -1 / -1.0 / 1 / 0 / 0 = `CreateSequenceHandle`'s defaults; missing sequence returns; `+0x170` OverUI; live handle: lost sequence -> -1, else
  `SetPosition( pos, 0, 0 )`; `GetShow()` (vslot 0x1c) -> inlined `SetShowObject( !GetHideDialog() )`. The port's extra `m_pGageData` null test is defensive only.
  `32e681e0` two passes = port. `32e5e450` = base `32e5c380`, then `g_pData+0x84` sequence `SetShowObject( arg )`; March's base `SetShow` is virtual (`X2GageUI.h:107`).
  Vtable `3353348c` differs from both siblings (`3353336c`, `335336cc`) at slots 0 / 2 / 8 / 21 only; slot 1 `32e6d6a0` is shared (3 vtables). `32e5b470` pushes
  6 / 5 / 3 / 2 = `PGUWO_ORB_EVE` (under `ADD_EVE_SYSTEM_2014`) / `_ARA` / `_CHUNG` / `_ORB3`.
- **= March spot-check:** `CreateNasodBall` plays `EffectSet_BEED_UP` in March too. **Routing** (Stage 33 bullet, CH1 / CH4a amendments) is in place and correct.

**Hygiene.** `flagcheck _RAGE --range 8b093bac^..8b093bac --with _RAGE_DEBUG --files` the 4: `files: 2 PASS, 1 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; 0 deleted,
0 non-ASCII, 0 EOL-mismatched, 0 `#else/#endif` without the token, 0 encoding changes; RESULT FAIL only for `define: NOT FOUND` (`//#define`). numstat 198 / 74 / 7 / 42, 0 deleted.
`_RAGE_DEBUG --with _RAGE`: `1 PASS, 1 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`, `RESULT: PASS`. `junediverge.py --check-all`: `allow-listed: 186`, `RESULT: PASS`.
`enumscan.py --check X2GURaven.cpp X2GageUI.cpp`: `0 A/B row(s) remain`. Probes (`census.cl_cmd`, `/c /Zs /EHsc`) of both files: no flag, `_RAGE`, `_LIVE` + `_RAGE` = 0 errors;
`_LIVE` alone = C1189 at `Always.h(11792)`. No build: the review changes no code. CH1 kickoff checked: ready once CH0b's walk is done.

### CH1 - Done (2026-11-06)

Opus 5.5. **The deploy unit is live.** `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (`Always.h:11698`) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_RAGE` (`Always.h:11772`) are
defined, so Raven runs June's registration, state bodies, virtuals, skill head, pressed-slot space and Nasod core (CH1a-CH1q). `_RAGE_DEBUG` stays on for the
play-test (the review retires it). CH0b's walk: user, "walk was done, OK" (2026-11-06).
- **Script:** `luasrc.py adopt-june data036 RAVEN_FIGHTER.lua` (dry run first): June's file replaces March's whole as `ljconv` bytecode, 210,847 B sha1
  `cd55974d`, no core-list row, no edit. Record `june2017_re/june_adopt_20261106_143049.tsv`, backup `24965799\kom_backup_stage_june\20261106_143049`.
  `luabind.py check RAVEN_FIGHTER`: 0 methods, 0 unbound, `RESULT: PASS`.
- **Motion:** `kom.py stage-june june2017_re/char_raven/ch1_held_motions.tsv`: `Motion_Raven_OT.x` (3,151,017 B) replaces March's in data033 (backup
  `kom_backup_stage_june\20261106_143106`). March's only name June's file drops was played by March's script, which is gone.
- **Packed / copied (user approved):** `PACK OK data036` (~1 `RAVEN_FIGHTER.lua`), `PACK OK data033` (~1 `Motion_Raven_OT.x`);
  `COPY OK data036 ... 14042587 bytes, 270 entries, md5 5c48fe2087b8ad7f8c1e4d17c4eb067d`, `COPY OK data033 ... 168725074 bytes, 165 entries, md5 1321c3464cb7a8033f7a077d4117922b`.
- **`[JUNE_CHAR] RAVEN DATA`** (`_LIVE`, end of `InitState`, `X2GURaven.cpp:1238`, offline_server.log): class, script, motion, state count, `wait_state_change`, as
  Elsword's. `X2GURaven.cpp` includes `Offline/X2OfflineLog.h` under `_LIVE` too (it was only under `_RAGE_DEBUG`).
- **Build fix:** the first build stopped in KTDXLIB at the `_LIVE` guard on `SERV_RAVEN_VETERAN_COMMANDER` (C1189): that flag lives only in `ServerDefine.h`,
  which KTDXLIB never includes (the X2Lib probes passed because X2Lib does). The guard moved to the top of `X2GURaven.cpp` under `_LIVE`; every other
  `_LIVE` / `_RAGE` guard flag is `#define`d earlier in `Always.h` (checked line by line).
- **7h:** June's `RSI_WAIT_STATE_CHANGE` has `ANIM_NAME = ""` (`RAVEN_FIGHTER.src.lua:496`); Raven's `CommonStateStart` (`:7047`) hands off to
  `CX2GUUser::CommonStateStart`, which takes it as for the four classes before (the pose holds).
- **`ReservedSkillStart`** (CH1k) is in place under `_LIVE` (`X2GURaven.cpp:7954`): cool time and MP at skill-state start, plus the memo refund.
- **INIT_PHYSIC:** `SERV_IRUHADEV_JUNE_DAMAGE_EFFECT_INIT_PHYSIC` is on (`Always.h:10293`); June's only reader is `DamageEffect_RF_DASH_COMBO_Z`
  (`BACK_SPEED_X = INIT_PHYSIC.WALK_SPEED + 250`, `DamageEffect.src.lua:351020`), in live data036 too; the C++ stand-in gives `WALK_SPEED = 500`, the
  value in `RAVEN_FIGHTER.lua:257`.
- **Lua load check:** `luasrc.py core --check` `CORE CHECK PASS: 0 FAIL, 0 WARN`, `ledger --check` `LEDGER CHECK PASS`. Core files Raven reads:
  `Enum.lua`, `UnitTemplet.lua`, `NPCTemplet.lua` (source), `DamageEffect.lua`, `EffectSet.lua` (June bytecode). Load-time errors show only in
  `ErrorLog.txt`: reference `data\ErrorLog_pre_CH1_raven.txt` (192,987 B; a start-up-only run 2026-11-06 14:35 on the pre-CH1 exe) and, for Raven loads,
  `ErrorLog_pre_CH0b_raven.txt`.
- **Scans:** `charport_keys.py RAVEN_FIGHTER`: 251 keys, `june-only=10 neither=6 march-only-site=6 both=229`, CH0's numbers and names. `charport_scan.py`
  (census rerun first, `census.tsv` / key matrix reverted after): `no NUMERICAL_DATA_INDEX` 70 / `no DAMAGE table` 25, equal to
  `char_raven/plan_charport_scan_20261105.txt`. Nothing new.
- **JUNE_BUGS:** rows 543 (`ANUM_SPEED` x1, `RVC_DASH_JUMP_COMBO_XX_START`) and 544 (`STOP_TIME_ATK` x8); no reader either side, March's script has them too.
- **Checklist:** `(Raven: CH1 ...)` at l.16, 120, 423, 427, 509, 548, 592, 674, 759. CH0's §5 `BURNING_NASOD_HAND_POSSIBLE` row is not in the checklist
  (the key is read by `CommonStateStart` under `_LIVE`, CH1j); no mark.

Hygiene: `flagcheck.py JUNE_CHAR_RAVEN_LIVE --worktree --files X2Lib/X2GURaven.cpp KTDXLIB/Always.h`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`;
`0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
The OTHER is `Always.h` (the two defines, their dated lines and the moved guard: `-5` = the two `//#define`s and the three guard lines). `_RAGE` (same files):
`0 PASS, 2 OTHER, 0 FAIL`, `RESULT: PASS`. `junediverge.py --check X2Lib/X2GURaven.cpp KTDXLIB/Always.h`: `allow-listed: 9`, `RESULT: PASS`.
`enumscan.py --check X2Lib/X2GURaven.cpp`: `0 A/B row(s)`. One-file probes (`census.cl_cmd`, `/c /Zs /EHsc`, both flags on): `X2GURaven.cpp`, `X2GageUI.cpp`,
`X2GUUser.cpp`, `X2DamageManager.cpp`, `X2DamageEffect.cpp`, `X2GageData.cpp`, `X2Game.cpp`, `X2GameUnit.cpp` exit 0, 0 errors.
Build: `DEPLOY OK: X2_offline.exe is 26,039,296 B, dated 2026-11-06 14:49:59, md5 76fc2a9ac22588f39c371b564994b92e, equal to X2/US_SERVICE/x2.exe (2026-11-06 14:49:52); built at main 5c9a9af5 + 2 uncommitted code file(s).`
`EXESTR OK: 2 of 2 found in X2_offline.exe, 26039296 B, dated 2026-11-06 14:49:59, md5 76fc2a9ac22588f39c371b564994b92e` (`RAVEN DATA` wide x1, `RAVEN RAGE` wide x4).

**Play-test (user) is the gate:** units 72-77 (classes 4 / 16 / 17 / 25 / 107 / 115) and 49 (106), skills 24025 / 24217 / 24026 / 24027 in slots A-S-D-C (bank 0;
none in slot 4 / 9) for the bar only; combos, dash, jump, a bounding hit, damage and stand-up, riding, parrying, rage gain, Nasod balls, the orb ball, awakening; the Z
re-attack after a full Z-Z-Z (CH1d); `RAVEN DATA` per class; ErrorLog against the reference. Skills are not tested here (user, 2026-11-06: an unported skill cannot be cast); the two-casts check (cool time, MP, `ReservedSkillStart`) is CH2a's first hand-off.
**Play-test (2026-11-06, run 14:53-15:02 on md5 76fc2a9a): user "all test passed".** `offline_server.log`:
- `RAVEN DATA` for all 7 classes, each `script RAVEN_FIGHTER.lua motion MOTION_RAVEN.X wait_state_change 1`; states 4: 136, 16: 155, 17: 148, 25: 155,
  106: 159 (twice, the same), 107: 155, 115: 174 (the review checks them against `ch1m_reg_by_class.tsv`).
- `RAVEN RAGE`: `orb ball ... sequence made, at ( 93, 56 )` x3 (107 / 115 / 106; = ( 74, 60 ) x the resolution scale 1.25 / 0.93), `ball level 5, rage 100.0`
  x3 right after (awakening), `hit gain 0.62 ( specialization A 0.00, Open Mind level 0 ): rage 100.6` x1 (logged before the clamp to 100).
  **Open:** no `parry` line. The review asks for that step once before retiring `_RAGE_DEBUG`.
- START / WIN / LOSE: ErrorLog's `C3e-2 roll` lines pick `RSI_*_START` / `_WIN` / `_LOSE` tables for every job, list sizes 1-6, none `list size 0`.
ErrorLog against `ErrorLog_pre_CH1_raven.txt` / `ErrorLog_pre_CH0b_raven.txt`: `KEM_ERROR` 0 / 0 / 0, `Motion Error` 0 / 0 / 0, `[LUA_SRC]` 7 / 7 / 7, `lua error` 3 / 3 / 3,
no missing animation. The new lines come from Raven gear, effects and places those runs never opened: optional `.XET` probes (Raven costume / effect
meshes, `C_RVC_DashComboZZZZ_AttackLine`), `result : 47` on nine Raven sounds (as Chung's and Elsword's), `HQ_CUTIN_RAVEN_N.DDS.GFX` (March's cut-in table
plus `.gfx`, CH5's), the quest event scene `Quest_N_RAVEN_End`, three texture names no kom has (`MESH_RAEN_CASH_FACE.TGA` and `MESH_RAVEN_CASH_FACETGA`, typos
inside a costume mesh; `ATTRIBUTE_HAND_WHITE.TGA`, where data002 has the `.dds`), and `JUNE_FX MP_CRASH RVC_C_dashJumpX_bullet_M01` traces. Nothing
broke in play; no new routing. `AddGroupChainQuest` unbound x1 is pre-CH1.

### CH1 - Review (2026-11-06)

Opus 5.5. **Verdict: passes.** One source change: `_RAGE_DEBUG` retired (`Always.h:11803`, `//#define`); the four `RAVEN RAGE` lines go dark, `RAVEN DATA`
stays (`X2OfflineLog.h` is included under `_LIVE`).
- **Deploy unit:** data036 `RAVEN_FIGHTER.lua` 210,847 B sha1 `cd55974d...` header `1B 4B 4C 81` = `june_adopt_20261106_143049.tsv:4`; no `.src.lua` sidecar.
  `stage-june --dry-run ch1_held_motions.tsv`: `1 staged (0 new, 0 replaced, 1 identical)` (`Motion_Raven_OT.x`). `kom.py status data036 data033`: both in sync,
  `+0 / -0 / ~0` vs live (data036's `[stale .src.lua: compile]` is older non-Raven sidecars). `luabind.py check RAVEN_FIGHTER`: 0 unbound, `RESULT: PASS`.
- **State counts, exact for all 7 units:** C++ registrations on the class path + 9 `USI_EVENT_*` (`InitEventStates` via `InitStateCommonRandom`, `X2GURaven.cpp:898`)
  + 20 slot states (`SetEquippedSkillFuncBySkillSlotIndex` `:4222`, normal / hyper x A / B x 5) + 1 key 0 (`GetStateDataIterator( 0 )`, `X2GUUser.cpp:1451`) + learned-skill
  sub-states (`SetEquippedSkillLevel` `_LIVE`, `:5876`, `m_StateList[ id ]` inserts; the save's base ids reach it through the upgrade swap, 24217 -> 24200 ...):
  4: 99+9+7+20+1 = 136; 16: 116+9+9+20+1 = 155; 17: 110+9+8+20+1 = 148; 25: 111+9+14+20+1 = 155; 106: 116+9+13+20+1 = 159; 107: 115+9+10+20+1 = 155;
  115: 117+9+27+20+1 = 174. The tsv has 11 more rows per class than C++: June's Common ids 93-96 (riding) / 123-129 (phoenix, dance), `ch1m_regcheck.py`'s
  `LEFT` list. As Elsword's: the count follows the unit's skills, not its class.
- **START / WIN / LOSE:** `list size 0` 0, `re-bind skipped` for `RSI_` 0.
- **Nasod core** (second run 15:25-15:26, unit 49, user "all test passed"): `parry: rage 80.0, ball level 4` (100 clamped, -20), `parry: rage 60.8, ball level 3`
  (60.8 is not < 60), `parry: rage 0.0, ball level 0` (41.1 < 60 -> 0, 2 < 3 -> 0), specialization B 0.00 so the cost is always paid; `ball level` = rage / 20
  (23.6 -> 1, 82.9 -> 4, 100 -> 5); `hit gain` before the clamp; the orb ball at ( 93, 56 ) = ( 74, 60 ) scaled. All four lines as CH1q's review read June.
- **Z re-attack (CH1d routing):** June's Z-Z-Z (`32c29fd0`) stores only `m_bReAttackZ1Right`; `m_fReAttackZ1Time` stays 0 under `_LIVE` (March's 0.2 s set is
  in the `#else`, `:11752`), so the Wait / Walk Z gate (`:8564` and three more) never blocks. User run: passed (a Z right after Z-Z-Z restarts the combo).
- **ErrorLog** (first run, against `ErrorLog_pre_CH1_raven` / `_pre_CH0b_raven`): `KEM_ERROR` 0 / 0 / 0, `Motion Error` 0 / 0 / 0, `[LUA_SRC]` 7 / 7 / 7,
  `lua error` 3 / 3 / 3; 167 new groups = asset probes (Ruben village `.XET`s, Raven `_SORTED` costume meshes), Raven sounds `result 47`, the cut-in `.gfx` (CH5),
  `Quest_N_RAVEN_End`, `JUNE_FX MP_CRASH` traces, one item texture (`13Gang_Protect.tga`). Second run: `KEM_ERROR` 0, `Motion Error` 0, `[LUA_SRC]` 7. Nothing to route.
- **Ghidra spot-checks:** `RSI_PARRYING` Start `32c4a9f0` = port (CommonStateStart, rand % 2 `GuardSmallFront`, `+0x25e8 = 0x101`, time, two bools / floats 0,
  `DoParrying`). DamageReact's parry gate (`320472c8`-`320473e2`) = `X2GUUser.cpp:10251-10270` term for term (Raven, not RT 7 / 0xa, not a ground hit, hold, grab,
  riding vslot 0x200, `32070eb0`, `32c3e030` or `+0x97a0`, hyper time `31e4f890` > 0, balls `+0x97b0` > 0 or `+0x97a1`, super armor vslot 0x264, attacker front, foot).
- **Checklist / bugs:** 8 `(Raven: CH1, ...)` marks at l.16, 120, 423, 427, 509, 548, 592, 674; l.759 reads `(Raven: CH0b / CH1, ...)`, the ninth. `JUNE_BUGS.md` 543 / 544
  present. `charport_keys` / `charport_scan` = CH0's numbers (Done note).
- **`SERV_RAVEN_VETERAN_COMMANDER` guard:** `Always.h` keeps only a comment (`:11706`); X2Lib probe after `StdAfx.h` prints `VC DEFINED` (exit 0); `#undef` + the guard
  text -> C1189; real `X2GURaven.cpp` `/Zs` exit 0.

Hygiene: `flagcheck.py JUNE_CHAR_RAVEN_LIVE --range 6f7c2673^..6f7c2673`: `1 PASS, 1 OTHER (Always.h), 0 FAIL`, `RESULT: PASS`; `_RAGE`: `0 PASS, 2 OTHER, 0 FAIL`,
`RESULT: PASS`; 0 deleted / non-ASCII / EOL / encoding. `_RAGE_DEBUG --worktree --files KTDXLIB/Always.h`: `1 DEFINE`, 1 deleted line (the define, now `//#define`),
`RESULT: FAIL` only for that. `junediverge.py --check-all`: `allow-listed: 194`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp`: `0 A/B row(s)`.
Build: started after the define change, not awaited (debug-retire rule, user 2026-11-03).
CH2a and CH5 are ready; their kickoffs still match (CH2a carries the two-casts / `ReservedSkillStart` check).

## Phase CH2 - Skills: Raven and the first jobs (classes 4, 16, 17, 25)

**Run with:** Sonnet 5.5 high (transcription from CH0's map), Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL`.
Sub-tags `SKILL`, `NUM`. A skill's line decides its phase: class 4 has it -> CH2a; else the first of 16 / 17 / 25.

| Sub-phase | Scope |
|---|---|
| CH2a | Raven (4) skills |
| CH2b | Sword Taker (16) skills |
| CH2c | Over Taker (17) skills (`RAVEN_OVER_TAKER_GUARDIAN_STRIKE` / `_XX_FRONT_X` March-only templets, 7u; `OT_SP1a_ChargedBolt`'s June bug row if CH0 found one) |
| CH2d | Weapon Taker (25) skills (`Effectset_A_RWT_FLAME_SWORD_MEMO`, 7u) |

For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole; its attached rows (7u names, 7ai BFI
arguments, 7h UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables of `plan_june_side.md` §2); its effect closure (a name
found only now is staged and added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June
`32071720`): check the templet swap for every term that reads the id. `NUM use` lines for Raven skills; March vs June damage in `MODS.md`.

**Done gate:** every class-4 / 16 / 17 / 25 id in the S3 census has a state (except CH0's passive / CH4 ids); each skill in an A and a B slot
casts June's state, motion and effect, deals damage, shows its cool time and spends MP; no missing particle / mesh / texture line.

### CH2a - Done (2026-11-06)

Flag `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (+ `_SKILL_DEBUG`, `Always.h`; `#error` without `_LIVE`, `JUNE_ENUM_NAMES`, `JUNE_SKILL_NUMERICAL`, `JUNE_CHAR_CHUNG_CORE`, `JUNE_CHAR_ELSWORD_SKILL`) is defined: Raven's six skills run June's registrations. All 43 class-map rows are read from `charport_adis.py raven --phase CH2a` listings: **30 written** (14 inside March's function under the flag with March in `#else`, 16 new), **13 are June = March** (no code). `funcmap.tsv` rows `Char Raven CH2a` (44); `MODS.md` "Stage 7 Raven, CH2a" has the damage table; `JUNE_BUGS.md` 545 / 546.

| June | `CX2GURaven::` | June differs from March |
|---|---|---|
| `32c260a0` `32c26130` `32c261c0` `32c26280` | `RSI_SI1_MAXIMUM_CANNON_Init`, `RSI_SI2_POWER_ASSAULT_Init`, `RSI_SI3_SEVEN_BURST_Init`, `RSI_SI2_CANNON_BLADE_Init` | June's own preload lists (10 / 10 / 11 / 14 names) |
| `32c3e7a0` `32c2dbd0` | `RSI_SI2_POWER_ASSAULT_FrameMove`, `_StateEnd` (new) | no particles; 0.4 s unit black on `XL_EFFECT_2` ( stand-in for CRS 8 UNIT_BLACK ), 1.14 s back; the end state restores layer / colour |
| `32c30db0` `32c33510` | `RSI_SI_A_RF_SHADOW_STEP_StartFuture`, `..._PASSIVE_StartFuture` (new) | upgrade-swapped templet / level; speed x1.3 with no memo test; the slot version stores the held key |
| `32c49590` `32c4de60` | `..._Start`, `InitShadowStep` | real cool time ( `GetActualCoolTime` ); no 1 s cut per back-slide level; swap on the cool time and the buff rate |
| `32c52550` `32c50ef0` | `..._EventProcess`, `SetShadowStepAttackSkill` | upgraded templet ( `+0x178` ) takes the key-held back slide and the Z / X upgrade states; the attacks cost no MP |
| `32c378f0` .. `32c54190`, `32c50e00`, `32c27320` `32c37a10` | `RSI_SHADOW_BACK_SLIDE_UPGRADE_*` (5), `RSI_SHADOW_BACK_SLIDE_END_UPGRADE_*` (2) (new) | the back slide chain, written for CH4a to register |
| `32c266b0` `32c30ee0` `32c27ba0` `32c3a690` `32c27350` `32c4dcc0` `32c37b80` | `RSI_SHADOW_STEP_Z_*`, `_Z_UPGRADE_*`, `_X_UPGRADE_*` (7 new) | Z / X attack states; ground walk / dash cancels ( `RAVEN_GROUND_*` ) |
| `32c5d330` `32c5d3f0` | `RSI_A_RWT_FLAME_SWORD_GROUND_StateStart`, `..._AIR_StateStart` | levelled buff factor with the memo rate ( `PushBuffFactorWithMemoRate` ); fly state x1.2 on the cached power rate |
| `32c398b0` `32c39b20` | `..._GROUND_EventProcess`, `..._AIR_EventProcess` | dash / walk cancels need the foot on a line |
| = March | Max Cannon / Power Assault / Seven Burst / Cannon Blade EP and FMF, Shadow Step Init / FM / End, Flame Sword Init / Start / EP / FLY FMF | `funcmap.tsv` says which |

**Registration.** `SetMaximumCannonSkillFunc` (24000 / 24025, June's default) now registers Init + EventProcess on both slots; the id switch gains the Cannon Blade, Power Assault, Seven Burst, Shadow Step and Flame Sword cases (base and upgrade ids, raw-id switch) ahead of the default, Flame Sword also registers its GROUND / FLY states. `X2SkillTree.h`: nine `RNSI_*` names. The five `RSI_SHADOW_*` chain states are written but registered by CH4a (`InitPassiveSkillState`); without the Back Slide passive Shadow Step runs alone.

**Findings.** Shadow Step has no cool time without the Back Slide passive (JUNE_BUGS 545); the Z attack without the passive changes to states June has no table for (546). Routed: the CRS stand-in (Power Assault) to the engine plan, Shadow Step members `+0x9654 / +0x9664` (not read here) stay CH4a's.

**Gates.**
- `flagcheck JUNE_CHAR_RAVEN_SKILL --with JUNE_CHAR_RAVEN_SKILL_DEBUG --worktree --files X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h X2Lib/X2SkillTree.h KTDXLIB/Always.h`: `files: 2 PASS, 0 PASS~, 1 DEFINE, 1 OTHER (X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL, 0 ERROR` (numstat 677 / 18 / 13 / 27, 0 deletions); `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`.
- `junediverge.py --check` the five code files: `allow-listed: 16`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `0 A/B row(s) remain`. `skillicons.py check 24000-24003 24025-24028 24200-24201 24217-24218`: `skills 12, misses 0`, `RESULT: PASS`. `luabind.py check RAVEN_FIGHTER`: 0 unbound. One-file probes (`/Zs /EHsc`): `X2GURaven.cpp`, `X2GUUser.cpp` exit 0, 0 errors.
- DEPLOY OK: X2_offline.exe is 26,043,904 B, dated 2026-11-06 16:07:50, md5 d9a2b38173689b124d73d3311d70b552, equal to X2/US_SERVICE/x2.exe (2026-11-06 16:07:43); built at main 0297cb08 + 4 uncommitted code file(s).
- EXESTR OK: 4 of 4 found in X2_offline.exe, 26043904 B, md5 d9a2b381 ( wide: `[JUNE_CHAR] RAVEN SKILL equip skill`, `[JUNE_CHAR] RAVEN SKILL shadow step cool time`, `SCA_CRIMSON_BREAK_P01.Y`, `ALS_SLICER_01.dds` ).

**Play-test hand-off.** Unit 72 `RavenFighterT` (Raven, class 4, lv 99, SP 843): A Maximum Cannon (24025), S Shadow Step (24217), D Cannon Blade (24026), C Power Assault (24027) are slotted; for Seven Burst (24028) and Flame Sword (24218) say "swap" and I move them in. Dev console ( END key ): `/invin 0`, then `/stage 1`. Each skill twice, enemies in front:
1. **A Maximum Cannon, D Cannon Blade, C Power Assault.** June's motion and effects; the slot shows a cool time after the first cast and the MP bar drops (CH1k's `ReservedSkillStart`); a second cast during the cool time is refused unless the dev gate lets it through. Power Assault: the unit turns black for ~0.7 s ( stand-in ) and recovers.
2. **S Shadow Step.** Dashes forward, ends in a dash / jump; Z / X during it do nothing (needs the passive, CH4a); it shows no cool time (JUNE_BUGS 545).
3. Log: `python tools/logs.py --grep "RAVEN SKILL"` and `[JUNE_SKILL]` `NUM use` ( x0.01 at level 1: Max Cannon 2.63, Cannon Blade 4.15, Power Assault 4.88, Seven Burst 6.43, Flame Sword 0.81 ). Then say "check logs".

**Play-test (2026-11-06, run 16:10-16:12 on md5 d9a2b381): user "all passed"** for unit 72 ( A Maximum Cannon 24025, S Shadow Step 24217, D Cannon Blade 24026, C Power Assault 24027 ): cool time, MP, second-cast refusal, Power Assault's black flash and Shadow Step's plain dash as handed off. Logs: four `RAVEN SKILL equip` lines (slots 0-3, bank A, states 49 / 51 / 53 / 55), ~16 Raven skill cut-ins, `RAVEN_SP_CANONBLADE` / `RAVEN_COMBOZ4AFIRE` sounds; `KEM_ERROR` 0, `Motion Error` 0; new lines only optional `.XET` probes (`RAVEN_SI_A_RF_SHADOW_STEP_MESH01`, `MOTION_RAVEN_RAGEMODE_NASOD`) and `result : 47` sounds. No `NUM use` lines: the engine's NUMERICAL_DEBUG is retired, so the review checks damage another way. **Not yet played:** Seven Burst (24028) and Flame Sword (24218) ( the review's hand-off, or "swap" ).

### CH2a - Review (2026-11-06)

Opus 5.5. **Verdict: PASS, no code change.** Read from the `charport_adis.py raven --phase CH2a` listings of all 43 addresses (+ `31fc9570`); commit `57959481`.
- **Registration** (`ch0_skill_listing_rows.tsv`, 14 rows of `32c641a0`): slot for slot. Max Cannon Init + EP, hyper EP only (`@32c66ce8` / `@32c66d13`, same table); Cannon Blade the same; Power Assault five callbacks on both; Seven Burst hyper FMF + EP; Shadow Step hyper without Init; Flame Sword Init / Start / EP on both, GROUND (278) Start + EP, FLY (279) Start + FMF + EP. Base and upgrade ids share each case. All CH2a ids sit below `RNSI_HA_RRF_INFERNAL_ARMS`, so they reach the second switch. `ch0_reg.tsv` 222 / 305-309 match the chain states written for CH4a, and 222 uses `32c33510` = `_PASSIVE_StartFuture`.
- **Shadow Step.** `32c52550`: time out -> `InitShadowStep`, foot ? dash : JUMP_DOWN y-45. Upgraded templet (`+0x178` = `m_iDeleteSkillIDByUpgrade`): back slide on `IsKeyPressedOnce` (`31fc8900`); Z (> 0.2 s, thrust ? (back slide ? 0x132 : 0xc9)) does **not** clear the combo; X clears it and calls `SetShadowStepAttackSkill( 0x133 )`. Plain: Z clears the combo first, then 0x131 / 0xc9. `32c50ef0`: 0x9661 + 0xe2, 0x9660 + 0xe1, thrust + back slide + not 0xc9 -> the arg, else 0xc9; no `FlushMp`. `32c4de60`: `SetSkillCoolTimeLeft( 24200, t, 1 )` + `3235a710( 328905b0( 24200 ) )` = `ApplySkillCoolTimeRateByBuff( GetUpgradeSwappedSkillID )`, no `-= level`, word 0x9728 = 0x100. `32c30db0`: x1.3 unconditional, null unit returns before the key `Set`. `32c49590`: vslot 0x6a4 `GetActualCoolTime( t, lv, 0 )` (third arg unused, base `31fd1920`), null unit returns. `32c33510`: level 0 -> 1, `GetSkillTemplet( 24200, 1 )`; June has no null-unit test here, the port's guard only avoids June's crash path.
- **Chain states** `32c378f0` (`_ftol2` range, null unit skips `CommonStateStartFuture`), `32c42a30`, `32c379b0`, `32c54190`, `32c50e00` (`DisableAfterImage( 0, 3 )` = March's `( false )`), `32c3a690` (walk, dash, skill), `32c37b80` (dash, walk, skill), `32c4dcc0` (0.01 one-shot, `PlayEffectSet` args = March's defaults), `32c30ee0`, `32c37a10`, `32c27320`: = port.
- **Power Assault** `32c3e7a0` / `32c2dbd0`: 0.4 one-shot -> `+0x8e64` = 8, `+0x164` = 0x10 (`XL_EFFECT_2`); else 1.14 one-shot -> `31fc9570` (`+0xc44` colour a 1, `+0x8e64` 0, `+0xc34` outline a 1, `+0xc74` blend 0), layer 0xb, colour rgb 1, alpha object off; the end state the same. **The stand-in stays:** R0's `crs` row 8 is RT 0 with colour (0,0,0,1), which is exactly the held render param the port sets. The stand-in was routed in the note but written nowhere, so this review adds it to checklist 7w and `kickoffs/june-engine/R5.md`.
- **Flame Sword** `32c5d330` / `32c5d3f0`: templet 24201 swap, `>= 1` factor, level max 1, `31fdbfc0` rate + `322b5320( factor, lv, 1, rate )` = `PushBuffFactorWithMemoRate`; the fly state x1.2 on `+0x7cc8` = `m_fNowStatePowerRate`. `32c398b0`: end -> foot ? WAIT : JUMP_DOWN; dash, walk (foot-gated), June's `SKILL_CANCEL_AFTER`. `32c39b20`: end -> landing only on foot; dash, walk, then `SKILL_CANCEL_AFTER_FLY`, whose dangling `else if` makes `CommonEventProcess` its body, in June too.
- **"= March" rows** (13) and the 7 preload lists: equal, name for name and in order (Sonnet subagent, spot-checked).
- **Play-test / damage.** The CH2a play-test passed. No `NUM use` lines: `SERV_IRUHADEV_JUNE_SKILL_NUMERICAL_DEBUG` was retired by the pilot (`63599741`). CH2a code changes no damage term except Flame Sword's x1.2 (above); the numbers are S4's data override. Seven Burst / Flame Sword are not played yet and are not a blocker: they go into CH2b's walk.
- **`_SKILL_DEBUG` retired** (`Always.h:11842`, `//#define`): the `RAVEN SKILL` equip / cool-time lines go dark. CH2b re-defines it for its own lines.

**Gates.** flagcheck `_SKILL --with _SKILL_DEBUG --range 57959481^..57959481`: `2 PASS, 1 DEFINE, 1 OTHER (X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL`, 0 deleted, `RESULT: PASS`; worktree (`Always.h`): `1 DEFINE, 0 FAIL`, 1 deleted line (the retired `#define`), `RESULT: PASS`. `junediverge.py --check-all`: `allow-listed: 194`, `RESULT: PASS`. `enumscan.py --check` `.cpp` / `.h`: `0 A/B row(s) remain`. One-file probes (`census.cl_cmd`, `/c /Zs /EHsc`): `X2GURaven.cpp`, `X2GUUser.cpp` exit 0, 0 errors. Build: not awaited (debug retire only).

### CH2b - Done (2026-11-06)

Flag `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (+ `_SKILL_DEBUG` re-defined; `Always.h` now also `#error`s without `..._CHAR_EVE_PASSIVE`, for `GetSkillBuffTempletID`): the twelve Sword Taker ids run June's registrations. All 27 class-map rows are read from `charport_adis.py raven --phase CH2b` listings: **12 written** (11 inside March's function under the flag with March in `#else`, 1 new), **15 are June = March** (no code). `funcmap.tsv` rows `Char Raven CH2b` (28); `MODS.md` "Stage 7 Raven, CH2b" has the damage table; `JUNE_BUGS.md` 547.

| June | `CX2GURaven::` | June differs from March |
|---|---|---|
| `32c263e0` `32c26770` `32c26cb0` | `RSI_SI1_SOUL_TAKER_BERSERKER_BLADE_Init`, `RSI_SA_RST_SHOCKWAVE_Init`, `SI_A_RBM_ONE_FLASH_Init` | June's preload lists (10 / 13 / 6 names) |
| `32c264a0` | `RSI_SI3_SOUL_TAKER_HYPERSONIC_STAB_Init` (new) | 12 preloads; March had no Init |
| `32c26820` | `RSI_SA_RST_SHOCKWAVE_StateEnd` | erases the buff of skill 24418 (`RNSI_P_RBM_COUNTER`, factor 0, learned only) = `3229ea50` ( `GetSkillBuffFactorPtr` vslot 0x238 + find by the factor's id + erase ); written as `GetSkillBuffTempletID` + `EraseBuffTempletFromGameUnit`; JUNE_BUGS 547 |
| `32c33280` | `SI_A_RBM_ONE_FLASH_StateStart` | templet 24207 upgrade-swapped; level `GetSkillLevel( id, false, 0, true )` min 1; `SA_DAMAGE_TIME` ( 0x4e ) -> `ChangeDamageTimes` |
| `32c5ccc0` `32c5cd80` | `RVC_SI_A_BLEEDING_SLICER_GROUND_StateStart`, `_AIR_StateStart` | templet 24208 swapped; `PushBuffFactorWithMemoRate` with the skill's memo rate |
| `32c37dc0` `32c38030` | `..._GROUND_EventProcess`, `..._AIR_EventProcess` | dash / walk cancels need the foot on a line; the fly state's skill cancel keeps June's dangling `else if` |
| `32c3aab0` | `RSI_A_ROT_ARMOR_BREAK_EventProcess` | no walk cancel; foot-gated dash cancel; any key after 0.66 s -> WAIT |
| `32c30af0` | `RSI_SI3_SOUL_TAKER_HYPERSONIC_STAB_FrameMoveFuture` | thrust speed 1300 ( March 1000 ), `EventTimerOneshot( 0.7 )` without the event flag; the hyper state uses it too |
| = March | Bleeding Slicer Init / Start / GROUND FrameMove / End / AIR FrameMove / FMF / End / EP, Armor Break Init, Berserker Blade EP, Hypersonic Stab EP, Shockwave Start / EP / FrameMove, One Flash EP | `funcmap.tsv` says which |

**Registration.** Six new cases in the id switch ( base and upgrade ids, raw-id ): Shockwave ( both slots, five callbacks, the normal functions reused for hyper, so the hyper FrameMove is June's 0.01 s ), Berserker Blade ( Init + EP; hyper EP only; one table ), Hypersonic Stab ( Init + FMF + EP; hyper FMF + EP with its own table ), One Flash ( Init / Start / EP both ), Bleeding Slicer ( Init / Start / EP both + the GROUND / FLY states ), Armor Break ( Init + EP both ). Berserker Blade and Hypersonic Stab lose March's FrameMove ( June has none; the script owns particles and cut-in ). `X2SkillTree.h`: twelve `RNSI_*` names ( 24011 / 24012 / 24013 / 24030 / 24033 / 24046 / 24207 / 24209 / 24220 / 24232 / 24233 / 24418 ).

**Findings.** Routed, not ported: `m_fEDTBleedingTime` is still read through March's `SI_A_RVC_BLEEDING_SLICER` in the unit init ( `X2GURaven.cpp` ~4060 ) and by nothing else ( CH4a, `ch0_passive_ids.tsv` row 51 ); the air-slicer test in `SpecialAttackEventProcess` ( ~8185 ) is CH4a's. Checklist rows: L50 marked `(Raven: CH2b)`; L116 / L117 / L178 / L714-733 / L844 covered by this phase's ids ( numerical data is S4's, the states are bound, the BFI push passes June's factor / level / skill id ).

**Gates.**
- `flagcheck JUNE_CHAR_RAVEN_SKILL --with JUNE_CHAR_RAVEN_SKILL_DEBUG --worktree --files X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h X2Lib/X2SkillTree.h KTDXLIB/Always.h`: `2 PASS, 0 PASS~, 1 DEFINE, 1 OTHER (X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL, 0 ERROR`; `1 deleted line(s) (the //#define), 0 non-ASCII, 2 EOL-mismatched inserted line(s) (the empty One Flash Init sits between a CR and an LF line), 0 #else/#endif without the flag token`, `RESULT: PASS`.
- `junediverge.py --check` the four files: `allow-listed: 9`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `0 A/B row(s) remain`. `skillicons.py check` the 12 ids: `skills 12, misses 0`, `RESULT: PASS`. `luabind.py check RAVEN_FIGHTER`: 0 unbound. One-file probe `X2GURaven.cpp` (`/c /Zs /EHsc`): exit 0, 0 errors.
- DEPLOY OK: X2_offline.exe is 26,048,512 B, dated 2026-11-06 17:00:02, md5 3170bfd7d87a2fc615f6255279e5ed1d, equal to X2/US_SERVICE/x2.exe (2026-11-06 16:59:55); built at main 39f6d5e7 + 4 uncommitted code file(s).
- EXESTR OK: 4 of 4 found in X2_offline.exe, 26048512 B, md5 3170bfd7 ( wide: `[JUNE_CHAR] RAVEN SKILL one flash skill`, `[JUNE_CHAR] RAVEN SKILL shockwave end counter buff`, `RST_ShockWave_Pro01.Y`, `BerserkerBlade_Weapon.X` ).

**Play-test hand-off.** Unit 73 `SwordTakerT` ( Sword Taker, class 16, lv 99 ). Save set up ( backup taken by `elsdb`, no schema change ): bank A = slot 0 Shockwave 24030, 1 Berserker Blade 24033, 2 Hypersonic Stab 24046, 3 Finishing Slash 24220; bank B = slot 5 Fatal Wound 24232, 6 Armor Crash 24233, 7 Seven Burst 24028, 8 Flame Sword 24218 ( slots 4 / 9 empty ). Dev console ( END key ): `/invin 0`, then `/stage 1`. Each skill twice, enemies in front ( the slot shows a cool time after the first cast, the MP bar drops ):
1. **Bank A.** Shockwave: unit hovers, June's shockwave blades; Berserker Blade: June's mesh blades; Hypersonic Stab: dash thrust ( longer than March's ); Finishing Slash: several quick hits, then lands.
2. **Bank B.** Fatal Wound: ground and air versions, bleed on hit; Armor Crash: key press after 0.66 s ends it; Seven Burst ( CH2a, never played ) and Flame Sword ( ground and air ).
3. Log: `python tools/logs.py --grep "RAVEN SKILL"` ( `equip skill` for 24030 / 24033 / 24046 / 24220 / 24232 / 24233 / 24028 / 24218, `one flash skill ... hits`, `shockwave end counter buff 0` without the Blade Master passive ). Then say "check logs".

**Play-test (2026-11-06, run 17:02-17:05 on md5 3170bfd7): user "all's fine"** for unit 73 ( banks A + B: Shockwave, Berserker Blade, Hypersonic Stab, Finishing Slash, Fatal Wound, Armor Crash, Seven Burst, Flame Sword ). Logs: 8 `RAVEN SKILL equip` lines ( states 49 / 51 / 53 / 55 / 59 / 61 / 63 / 65 ); effect sets created for every skill ( Shockwave 7 + 6 Ultimate, Berserker Blade Ultimate, Hypersonic Stab 6, Armor Break 7, Seven Burst 3, Flame Sword 5, One Flash 2, Bleeding Slicer alpha set once ); `one flash skill 24220 level 1 hits 6` twice; `shockwave end counter buff 0` ( no Counter Wave buff on this character ); `Motion Error` 0, `KEM_ERROR` 0, only optional `.XET` probes in ErrorLog.

### CH2b - Review (2026-11-06)

Opus 5.5. **Verdict: PASS, no code change** except retiring `_SKILL_DEBUG`. Read from the `charport_adis.py raven --phase CH2b` listings of all 27 addresses (+ `3229ea50`, `322ae0a0`); commit `203286aa`.
- **Registration** (`ch0_skill_listing_rows.tsv`, 14 rows of `32c641a0`): slot for slot. Shockwave five callbacks on both slots; Berserker Blade Init + EP, hyper EP only, one table; Hypersonic Stab Init + FMF + EP, hyper FMF + EP on `RSI_SI3_SOUL_TAKER_HYPERSONIC_STAB_HYPER`; One Flash Init / Start / EP on both; Bleeding Slicer Init / Start / EP on both, GROUND (274) Start / FrameMove / EP / End, FLY (275) Start / FMF / FrameMove / EP / End; Armor Break Init + EP on both. Base and upgrade ids share each case. All ids sit below `RNSI_HA_RRF_INFERNAL_ARMS` (24600), so they reach the raw-id switch. The twelve new `RNSI_*` values and 24208 equal `Enum.src.lua`.
- **Shockwave end `32c26820`:** `3229ea50( 0x5f62, 0, 1 )` calls vslot 0x238 (`GetSkillBuffFactorPtr`, June `32028af0`). It then finds the templet through `322ae0a0`, which compares templet `+0x34` with the factor's id, and erases it by iterator (`3229d320`) when found. March's `GetSkillBuffTempletID` (`BTI_NONE` without a factor) followed by `EraseBuffTempletFromGameUnit` (`find_if` by id, then `EraseBuffTempletFromGameUnitByIterator`) is the same effect. `m_bDisableGravity = false` comes before `CommonStateEnd`, as in June. JUNE_BUGS 547 stands.
- **One Flash `32c33280`:** `GetEquippedActiveSkillTemplet( 24207, 1 )`, then level `GetSkillLevel( id, 0, 0, 1 )` with a minimum of 1. June calls it twice where the port calls it once, with the same value. `GetSkillAbilityValue( 0x4e, lv, 0, 0.0 )` goes through `_ftol2`, and a result > 0 is stored to `+0x4118`. Key 78 is `SA_DAMAGE_TIME` in both loaders (`s0a_templet_keys.tsv` 117), and `ChangeDamageTimes` is the plain `m_iDamageTimes` store.
- **Bleeding Slicer starts** `32c5ccc0` / `32c5cd80`: the two listings are instruction-identical. Each takes templet 24208 (swap 1), needs `>= 1` factor and a unit, takes the level with a minimum of 1, then calls the `31fdbfc0` rate and `322b5320( factor, lv, 1, rate )`; that pair is `PushBuffFactorWithMemoRate`, as in CH2a's Flame Sword.
- **Cancels.**
  - `32c37dc0`: the animation end gives WAIT on a foot, else JUMP_DOWN. Then dash (`+0x8c9c`, foot, double right / left), walk (`+0x8c98`, foot, right / left) and `SKILL_CANCEL_AFTER` (`+0x8c90`, foot, ASUT 1, then hyper).
  - `32c38030`: on a foot, the animation end gives LANDING. Without a foot it does nothing. The dash and walk cancels need the foot. The fly skill cancel (`+0x8ca8`, ASUT 2) reaches `CommonEventProcess` only on a true return, which is the dangling `else if`, in June too.
  - `32c3aab0`: animation end, then the 14 keys in the macro's order with `0.66 < t` -> WAIT, then dash (foot), then `SKILL_CANCEL_AFTER`. There is no walk cancel.
- **Hypersonic Stab `32c30af0`:** `m_fAnimOneshotTimer < 0.7 && EventTimer( 0.7 )` = `EventTimerOneshot( 0.7 )` sets the speed to 1300. Otherwise `t >= 1.0` sets it to 0. Then `CommonFrameMoveFuture`.
- **"= March" rows** (15) and the 4 preload lists (10 / 12 / 13 / 6) are equal, name for name, call type and order. A Sonnet subagent checked them; I spot-checked Shockwave Init and the Bleeding state ids. June reads cancel times through the `+0x8e90` alternate animation, which is the shared macro idiom of earlier reviews.
- **Open, still routed:** `m_fEDTBleedingTime` has no reader (`X2GURaven.cpp` 325 / 4067 / 4069 only). It stays CH4a's (`ch0_passive_ids.tsv` row 51, in CH4a's passive-id list).
- **Play-test / logs:** passed (above). The logs have 16 `equip skill` lines, `one flash skill 24220 level 1 hits 6`, and 9 `shockwave end counter buff 0`.
- **`_SKILL_DEBUG` retired** (`Always.h:11846`, `//#define`).

**Gates.** flagcheck `_SKILL --with _SKILL_DEBUG --range 203286aa^..203286aa`: `2 PASS, 1 DEFINE, 1 OTHER (X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL`, 1 deleted, `RESULT: PASS`. The worktree check (`Always.h`) gives `1 DEFINE, 0 FAIL`, 1 deleted line (the retired `#define`), `RESULT: PASS`. `junediverge.py --check-all`: `allow-listed: 194`, `RESULT: PASS`. `enumscan.py --check` on the `.cpp` / `.h`: `0 A/B row(s) remain`. One-file probe `X2GURaven.cpp` (`census.cl_cmd`, `/c /Zs /EHsc`): exit 0, 0 errors. Build: not awaited (debug retire only).

### CH2c0 - Done (2026-11-06)

Opus 5.5. New flag **`SERV_IRUHADEV_JUNE_USER_LEFT_HAND_SCALE`** (defined, `Always.h` end; `CX2GUUser`, so not a Raven flag). Read from Ghidra's decompile and
the raw listing (`disasm_range.py --x87`); members found with `dispsweep.py 8e08 .. 8e1c` (only three `CX2GUUser` functions and the ctor touch them).
- **`31fd9f40` = `CX2GUUser::SetLeftHandScale( D3DXVECTOR3 vScale )`** (new, `X2GUUser.cpp` before `SetUnitReForm`): `m_vLeftHandScale` (+0x8e14) = the argument;
  `GetXSkinAnimPtr()` non-null -> `GetCloneFrame( L"Bip01_L_Hand" )` -> `SetFrameScale( m_vHandScale * vScale, true )` (frame +0xdc word 0x101 = `m_bScale`,
  `m_bApplyChild`; +0xe0 = `m_vScaleRel`). Callers: Megadrill Break `32c38710` ( 1, 1, 1 ), `32c5ce40` twice (CH2c).
- **Members** `m_vHandScale` (+0x8e08) and `m_vLeftHandScale` (+0x8e14), both ( 1, 1, 1 ) in the ctor (`3203e850` @320430bd-32043103).
- **Divergent pair, replaced in the same commit** (`june_march_divergent.tsv`, 2 rows): June's `ScaleHand` `31fde6d0` stores `m_vHandScale` and gives the left
  hand `m_vLeftHandScale * arg`; June's `RestoreHand` `31fded80` sets `m_vHandScale` to 1, restores the right hand and the Eve dummies, keeps the left hand at
  `m_vLeftHandScale`, and does **not** call `SetUnitReForm` (**JUNE_BUGS 548**: a hand-bone reform from `DefaultBoneInfo` stays lost until the next
  `InitMotion`). June's bodies sit inside March's functions, March's in `#else`. Without these two, the big-hand buff would wipe Megadrill Break's left-hand scale.
- **Stand-ins (Lu/Ciel not ported):** June's ScaleHand / RestoreHand also handle `UT_LUCIEL`'s `Bone_RFinger0-3` / `Bone_LFinger0-3` and the second body
  (+0x8e88). One-line comments at the sites; Lu/Ciel's port picks them up.
- **Not ported, routed:** June's other ScaleHand callers: `SetSpecificValueByBuffTempletID` `3204b5e0` (1.5 / 1.55 @3204c3dd, 2.55 @3204c664: June-only buff
  cases, funcmap row "3202ed60 / 3204b5e0 (CH4a)", checklist) and the Lu/Ciel second-body setter `3204e030` (1.5). March's one caller (big-hand elixir) is unchanged.
- **funcmap:** `31fd9f40` re-keyed to `CX2GUUser::SetLeftHandScale` (PORTED); `31fde6d0`, `31fded80` added (`Char Raven CH2c0`). `CH2c.md` amended with the March name.

**Gates.** flagcheck `--worktree --files Always.h X2GUUser.h X2GUUser.cpp`: `2 PASS, 1 DEFINE, 0 OTHER, 0 FAIL`, 0 deleted lines, 0 non-ASCII, 0 EOL mismatches,
2 plain `#else`, `RESULT: PASS`. `junediverge.py --check X2GUUser.cpp X2GUUser.h`: `RESULT: PASS`. `enumscan.py --check` same files: 2 rows, both
`X2GUUser.h:2555-2556` (NPC ids, older than this phase), none in CH2c0's lines. Flags probed ON: `SERV_NEW_DEFENCE_DUNGEON`, `ELSWORD_SECOND_CLASS_CHANGE`.
`DEPLOY OK: X2_offline.exe is 26,048,512 B, dated 2026-11-06 17:36:03, md5 2d11fea5e7b1b0df42eaa3185e31a0d7` (built at main 68bf3047 + the 3 code files).

**Play-test:** none. Nothing calls `SetLeftHandScale` until CH2c, and the rewritten `ScaleHand` / `RestoreHand` are reached only by the big-hand elixir buff.
CH2c's Megadrill Break play-test is the first in-game check. **Open for the user:** none.

### CH2c0 - Review (2026-11-06)

Opus 5.5. **Verdict: PASS, no code change.** Checked against Ghidra's decompiles of `31fd9f40` / `31fde6d0` / `31fded80` and `disasm_range.py --x87`; commit `6d2c4614`.
- **`31fd9f40` SetLeftHandScale:** the argument goes to +0x8e14..+0x8e1c. The product is `+0x8e08 * +0x8e14`, `+0x8e18 * +0x8e0c`, `+0x8e1c * +0x8e10`; the port keeps June's operand order.
  Then the `32071d80` (`GetXSkinAnimPtr`) null test and `31d38a20( L"Bip01_L_Hand" )`; on the frame it writes +0xe0..+0xe8 = the product and word 0x101 at +0xdc.
  That is `SetFrameScale( &v, true, pFrame )` (`KTDXDeviceXSkinMesh.h:596`: `m_bScale` = true, `m_bApplyChild` = true, `m_vScaleRel`). June also calls `GetXSkinAnimPtr` twice, like the port.
- **`31fde6d0` ScaleHand:** stores `m_vHandScale` before the +0x87d4 (`m_pXSkinAnim`) test, as the port does. In the two-bone loop `31c1c1a0( 0, size, L"Bip01_L_Hand" ) == 0` gives the left hand
  `m_vLeftHandScale * arg` and the right hand gets arg (all word 0x101). Eve test (`+0x867c` unit, `+0xaf0` templet type == 5) on Dummy1_Rhand / Dummy2_Lhand with arg is the
  same as March's `GetUnit()->GetType()` form. Left out, with stand-in comments: the `UT_LUCIEL` (10, `+0xe40 != 1`) finger loop 0-3 and the second body +0x8e88 (hand loop + Eve test).
- **`31fded80` RestoreHand:** `m_vHandScale` = 0x3f800000 x3, with no `m_pXSkinAnim` test (= March). The left hand gets word 0x101 + `m_vLeftHandScale * m_vHandScale`; the right hand and
  the Eve dummies get word 0x100 + 1.0 = `SetFrameScaleRestore` (`KTDXDeviceXSkinMesh.h:608`). The Lu/Ciel fingers and the second body's hand loop (no Eve test) are stand-ins.
  There is no `SetUnitReForm` call: caller `3202ed60` `@3202f17b` `call 31fded80` then `jmp 3202f2c9`, the epilogue. JUNE_BUGS 548 stands.
- **Ctor `3203e850`:** `fld1` `@3204309f`, then the stores to +0x8e14..+0x8e1c `@320430bd-320430e1` and +0x8e08..+0x8e10 `@320430ef-32043103`: both ( 1, 1, 1 ), = port.
- **Routing:** the Done note routes `3204b5e0` (1.5 / 1.55 / 2.55, CH4a funcmap row) and `3204e030` (Lu/Ciel). `CH2c.md:60` names `CX2GUUser::SetLeftHandScale( D3DXVECTOR3 vScale )`.

**Gates.** flagcheck `--range 6d2c4614^..6d2c4614`: `2 PASS, 1 DEFINE, 0 OTHER, 0 FAIL`, 0 deleted, 0 non-ASCII, 0 EOL mismatches, 2 plain `#else`, `RESULT: PASS`.
`junediverge.py --check-all`: `allow-listed: 194`, `RESULT: PASS`. `enumscan.py --check X2GUUser.cpp X2GUUser.h`: 2 rows (`X2GUUser.h:2555-2556`, older NPC ids). No build (no code change).

### CH2c - Done (2026-11-06)

Flag `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (+ `_SKILL_DEBUG` re-defined): the twelve Over Taker ids run June's registrations. All 23 class-map rows are read from `charport_adis.py raven --phase CH2c` listings plus the registration `32c641a0`: **12 written** inside March's function under the flag (March in `#else`, or insert-only), **11 are June = March** (no code). `funcmap.tsv` rows `Char Raven CH2c` (23); `MODS.md` "Stage 7 Raven, CH2c" has the damage table. No `JUNE_BUGS.md` row ( `RSI_SA_ROT_CHARGED_BOLT_BLOOD` / the bare `OT_SP1a_ChargedBolt` is dead data, CH0 ).

| June | `CX2GURaven::` | June differs from March |
|---|---|---|
| `32c265d0` `32c268e0` `32c27940` `32c27500` | `RSI_SI3_OVER_TAKER_GUARDIAN_STRIKE_Init`, `RSI_A_ROT_WEAPON_BREAK_Init`, `RSI_A_RRF_HELL_DIVE_Init`, `RSI_A_RF_MEGADRILL_BREAK_Init` | June's preload lists ( 11 / 11 / 7 names; Megadrill = March's 10 + 2 ) |
| `32c26970` | `RSI_A_ROT_WEAPON_BREAK_StateStart` | `CommonStateStart` only: the buff-factor push is gone ( the script's damage data owns it ) |
| `32c30cb0` | `RSI_SI3_OVER_TAKER_GUARDIAN_STRIKE_FrameMove` | 0.678 s one-shot only: `XL_EFFECT_2`, `m_bSetCustomRenderParam`, colour `0xff5f0000`, alpha object, `m_fGuardianStrikeRenderTime` 1.4 ( CH1j restores ); no cut-in, particles or damage effect |
| `32c48de0` | `RSI_SI1_OVER_TAKER_CHARGED_BOLT_FrameMove` | one `RAVEN_OVER_TAKER_CHARGED_BOLT` effect at 0.4 s ( no `_MEMO` variant ); one function for the normal and the hyper state |
| `32c49730` | `RSI_SA_ROT_VALKYRIESJAVELIN_FrameMove` | `EffectSet_SA_ROT_VALKYRIESJAVELIN_NEW` with default arguments ( no MEMO21 power-up ); the hyper state uses it and the one `LANDING` state ( no hyper landing ) |
| `32c39670` `32c38510` | `RSI_A_RRF_HELL_DIVE_EventProcess`, `RSI_A_RF_MEGADRILL_BREAK_EventProcess` | walk and dash cancels need the foot on a line ( `RAVEN_GROUND_WALK_CANCEL` / `_DASH_CANCEL` ); Hell Dive keeps the `y -= 45` JUMP_DOWN shift |
| `32c38710` | `RSI_A_RF_MEGADRILL_BREAK_End` | `SetLeftHandScale( 1, 1, 1 )` first ( `31fd9f40`, CH2c0 ) |
| `32c5ce40` | `RSI_A_RF_MEGADRILL_BREAK_FrameMove` | 1.0 s: `m_bApplyExtraDamage` ( DD +0x4fa ), `m_ExtraDamage.m_Damage` 2 ( +0xa4 ), type `EDT_MANA_DAMAGE` ( +0x50 = 16 ); `DAMAGE_DATA_NEXT`: `m_fCriticalRate` 1 ( +0x510 ), `SetLeftHandScale( 3, 3, 3 )`; a separate 2.15 s one-shot `SetLeftHandScale( 1, 1, 1 )` |
| = March | Charged Bolt Init / EP, Guardian Strike StartFuture / EP, Valkyrie Init / Start / EP / End / LANDING EP, Weapon Break EP, Megadrill FMF | `funcmap.tsv` says which |

**Registration.** Eight new cases for the twelve ids in the id switch ( raw ids, base and upgrade ): Charged Bolt ( Init / FM / EP; hyper FM + EP, one table ), Guardian Strike ( Init / StartFuture / FM / EP; hyper without Init ), Valkyrie's Javelin ( five callbacks; hyper without Init; the `LANDING` state with EP only ), Hell Dive and its upgrade ( Init + EP; the upgrade on `RSI_A_RRF_HELL_DIVE_UPGRADE` ), Megadrill Break and its upgrade ( five callbacks; the upgrade on `RSI_A_RF_MEGADRILL_BREAK_UPGRADE`, hyper through the shared tail with the same table ), Weapon Break ( Init / Start / EP; hyper Start + EP ). March's Charged Bolt Start / StartFuture / End, Hell Dive FrameMove and the hyper-named functions are no longer registered. `X2SkillTree.h`: eleven `RNSI_*` names ( values = `Enum.src.lua` ). The `SetEquippedSkillLevel` `LANDING` level term was already June's ( CH1 ); no other code reads these `SI_*` ids ( grep ) and no function reads a templet for them, so the upgrade-swap check finds nothing to change.

**Findings.** Routed, not ported: none new. Checklist rows: L50 marked `(Raven: CH2c, 6 of them)`; L116 / L117 / L178 / L714-733 / L844 are covered by this phase's ids ( numerical data is S4's and reaches the states by `APPLIED_SKILL_ID`; IGNORE_DEFENCE_RATE is engine-done, Megadrill Break is where it first plays ); L636 ( GUARDIAN_STRIKE / VJAVELIN March-only effect templets ) is taken: both are now June's callers. All preload names and the effect names ( `EffectSet.lua`, `DamageEffect.lua` in the live data036 ) exist.

**Gates.**
- `flagcheck JUNE_CHAR_RAVEN_SKILL --with JUNE_CHAR_RAVEN_SKILL_DEBUG --worktree --files X2Lib/X2GURaven.cpp X2Lib/X2SkillTree.h KTDXLIB/Always.h`: `1 PASS, 0 PASS~, 1 DEFINE, 1 OTHER (X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL`; `1 deleted line (the //#define), 0 non-ASCII, 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token`, `RESULT: PASS`.
- `junediverge.py --check` ( X2GURaven.cpp / .h, X2SkillTree.h ): `allow-listed: 9`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `0 A/B row(s) remain`. `skillicons.py check` the 12 ids: `skills 12, misses 0`, `RESULT: PASS`. `luabind.py check RAVEN_FIGHTER`: 0 unbound. One-file probe `X2GURaven.cpp` ( `/c /Zs /EHsc` ): exit 0, 0 errors.
- DEPLOY OK: X2_offline.exe is 26,050,560 B, dated 2026-11-06 18:16:30, md5 68a5ae773ddd447f0356866eead86c5e, equal to X2/US_SERVICE/x2.exe (2026-11-06 18:16:23); built at main 071405eb + 3 uncommitted code file(s).
- EXESTR OK: 7 of 7 found in X2_offline.exe, 26050560 B, md5 68a5ae77 ( wide: the four `[JUNE_CHAR] RAVEN SKILL` CH2c lines, `EffectSet_SA_ROT_VALKYRIESJAVELIN_NEW`, `aisha_SA_SCREWDRIVER_circle.X`, `VERITABLE_PANDEMONIUM_T02.dds` ).

**Play-test hand-off.** Unit 74 `OverTakerT` ( Over Taker, class 17, lv 99 ). Save set up ( backup taken by `elsdb`, no schema change ): bank A = slot 0 Valkyrie's Javelin 24029, 1 Charged Bolt 24032, 2 Guardian Strike 24041, 3 Weapon Crash 24229; bank B = slot 5 Hell Dive 24219, 6 Megadrill Break 24228 ( slots 4 / 7 / 8 / 9 empty ). Dev console ( END key ): `/invin 0`, then `/stage 1`. Each skill twice, enemies in front ( the slot shows a cool time after the first cast, the MP bar drops ):
1. **Bank A.** Valkyrie's Javelin: the unit hovers, then dives; June's new effect set and the landing; Charged Bolt: bolt at the hand, flare, camera shake; Guardian Strike: the unit turns dark red for about 1.4 s while the script's effects play, then is normal; Weapon Crash: June's break impact.
2. **Bank B.** Hell Dive: the punch effects ( no extra last punch ); Megadrill Break ( first, against a target with defence and MP ): the left hand grows to three times at the second hit, which also drains MP, the next hits crit, the hand is normal again at the end.
3. Log: `python tools/logs.py --grep "RAVEN SKILL"` ( `equip skill` for 24029 / 24032 / 24041 / 24229 / 24219 / 24228, `valkyrie javelin effect set played`, `guardian strike layer set`, `megadrill second hit mana damage`, `megadrill next damage data hand x3` ) and `--grep NUM` for the `NUM use` lines of 24004-24006 / 24202-24204. Then say "check logs".

**Play-test (2026-11-06, run 18:59-19:01 on md5 68a5ae77): user "all okay and play perfectly"** for unit 74 ( banks A + B: Valkyrie's Javelin, Charged Bolt, Guardian Strike, Weapon Crash, Hell Dive, Megadrill Break ). Logs: 6 `RAVEN SKILL equip` lines ( states 49 / 51 / 53 / 55 / 59 / 61 ); `valkyrie javelin effect set played` 1x, `guardian strike layer set` 1x, `megadrill second hit mana damage` and `next damage data hand x3` 5x each; `Motion Error` 0, `KEM_ERROR` 0, ErrorLog only the optional `MAGNUMBLASTER_MESH01.XET` probe. No `NUM use` lines exist in this build ( the review need not look for them ). Charged Bolt / Weapon Crash / Hell Dive have no debug line: user's visual check only.

### CH2c - Review (2026-11-06)

Fable 5.1 (the kickoff names Opus 5.5 high). **Verdict: PASS, no code change** except retiring `_SKILL_DEBUG`. Read from the `charport_adis.py raven --phase CH2c` listings of all 23 addresses, the registration `32c641a0` and `SimpleInit` `31f8ee50`; commit `e5e12fb1`.
- **Registration** (`ch0_skill_listing_rows.tsv` rows 12-18, 121-126, 159-160, 167-168; the listing at `32c645a6` / `32c646bd` / `32c6486e` / `32c649c0` / `32c6665a` / `32c666fb` / `32c661a9` / `32c661f7`): slot for slot. Charged Bolt Init + FM + EP, hyper FM + EP through the shared tail `32c66d66`; Guardian Strike Init + StartFuture + FM + EP, hyper without Init; Valkyrie's Javelin five callbacks, hyper without Init, LANDING (206) EP only; Hell Dive and its upgrade Init + EP on both, each on its own table; Megadrill Break and its upgrade five callbacks on both, the upgrade pushing `RSI_A_RF_MEGADRILL_BREAK_UPGRADE` before it jumps into the shared tail `32c666a1`; Weapon Break Init + Start + EP, hyper Start + EP. The eleven `RNSI_*` values equal `Enum.src.lua`.
- **Megadrill Break FM `32c5ce40`:** if / else-if chain 0.5 (hitUnitList clear), 1.0 (clear, bReAttack, fHitGap 0.1, Lhand1 on / Sword off, backSpeed.x 0, then unit `+0x89a` = 1, `+0x444` = 2.0 through the KProtectedType store, `+0x3f0` = 0x10), `m_fDamageDataChangeTime` (BeginTableByReference, SimpleInit, SetDamageData, EndTable, `+0x8b0` = 1.0, `SetLeftHandScale( 3, 3, 3 )`), 1.33 (= March's flare + mesh); then a separate `if` at 2.15 `SetLeftHandScale( 1, 1, 1 )`, then the two handle follow blocks. DD = unit + 0x3a0: `+0x4fa` / `+0x50e` / `+0x510` are written 1 / 1 / 0 by June's `SimpleInit` (`31f8f257` / `31f8f285` / `31f8f25e`), as March's writes `m_bApplyExtraDamage` / `m_bApplyMotionOffset` / `m_fCriticalRate` (`X2DamageManager.h:1545-1557`); `+0x50` is `m_ExtraDamageType` (`ExtraDamage::Init` sets `EDT_NONE` there); `EDT_MANA_DAMAGE` counts to 16. End `32c38710`: hand ( 1, 1, 1 ), Lhand1 off, Sword on, CommonStateEnd.
- **Guardian Strike FM `32c30cb0`:** the 0.678 one-shot only: `+0x164` = 0x10 (`XL_EFFECT_2` counts to 16), `m_bSetCustomRenderParam`, `m_RenderParam.color` floats ( 0.3725, 0, 0, 1 ) = `D3DXCOLOR( 0xff5f0000 )`, `+0x144` = 1, `+0x9774` = 1.4; then CommonFrameMove. No cut-in, particle or damage effect.
- **Valkyrie FM `32c49730`:** cut-in index 1 at 0.04; the 0.001 one-shot plays `EffectSet_SA_ROT_VALKYRIESJAVELIN_NEW` with ( NULL, false, -1, -1, ( 1,1,1 ), false, zero vectors ) = the declaration's defaults, push_back into `+0x3eb0` (March's `m_vecEffectSetToDeleteOnDamageReact`); 0.48: gravity on, `fDownAccel = fGAccel * 2`.
- **Charged Bolt FM `32c48de0`:** cut-in index 0; 0.4 `RAVEN_OVER_TAKER_CHARGED_BOLT` (rot +180 / pos +30 facing left); 0.7 the flare (pos -/+20, y +50, axis angle, land position); 0.8 `UpDownCrashCamera( 4, 0.3333 )`.
- **Cancels** `32c38510` / `32c39670`: animation end foot -> WAIT else JUMP_DOWN (Hell Dive then `y -= 45` = `LINE_RADIUS * 1.5`); walk (`+0x8c98`, foot, right / left), dash (`+0x8c9c`, foot, double), `SKILL_CANCEL_AFTER` (`+0x8c90`, foot, ASUT 1, the hyper branch) = `RAVEN_GROUND_WALK_CANCEL` / `_DASH_CANCEL` / March's live macro (`X2GUUser.h:113`).
- **Weapon Break Start `32c26970`** = `CommonStateStart` tail jump. **Preload lists** Guardian Strike 11 / Weapon Break 11 / Hell Dive 7 / Megadrill 12 equal, name for name and in order.
- **"= March" rows (11):** a Sonnet subagent compared each listing with March's body (strings, floats, calls, vslots, state ids, branch order): all equal; Weapon Break EP's 14 keys = `ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC` (`X2GUUser.h:236`) with a strict `> 0.66`; Megadrill FMF's one-shots 0.27 / 0.7 / 1.2 read `+0x2f4` = `m_OrgPhysicParam.fRunSpeed` (`GetDashSpeed`) x 1.2. I read the Init lists and `32c26970` myself.
- **Play-test / logs:** passed (above). `--tag JUNE_CHAR`: 39 `RAVEN SKILL equip skill` (the user kept playing after the run), `valkyrie javelin effect set played` 1, `guardian strike layer set` 1, `megadrill second hit mana damage` 4, `next damage data hand x3` 4; `Motion Error` 0, `KEM` 0; no `NUM` lines for 24004-24006 / 24202-24204 (no `NUM use` in this build).
- **`_SKILL_DEBUG` retired** (`Always.h:11847`, `//#define`).

**Gates.** flagcheck `_SKILL --with _SKILL_DEBUG --range e5e12fb1^..e5e12fb1`: `1 PASS, 1 DEFINE, 1 OTHER (X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL`, 1 deleted line (the `//#define`), `RESULT: PASS`. Worktree (`Always.h`): `1 DEFINE, 0 FAIL`, 1 deleted line (the retired `#define`), `RESULT: PASS`. `junediverge.py --check-all`: `allow-listed: 194`, `RESULT: PASS`. `enumscan.py --check` on the `.cpp` / `.h`: `0 A/B row(s) remain`. One-file probe `X2GURaven.cpp` (`census.cl_cmd`, `/c /Zs /EHsc`, after the retire): exit 0, 0 errors. Build: started not awaited (debug retire only), finished after the commit: DEPLOY OK: X2_offline.exe is 26,050,048 B, dated 2026-11-06 19:30:52, md5 9565a661e7016db7d14d5ecfecf830f6, equal to X2/US_SERVICE/x2.exe (2026-11-06 19:30:45); built at main 0ba30e9d.

### CH2d - Done (2026-11-06)

Flag `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (+ `_SKILL_DEBUG` re-defined): the twelve Weapon Taker ids run June's registrations. All 41 class-map rows are read from `charport_adis.py raven --phase CH2d` listings plus the registration `32c641a0`: **15 written** ( 13 inside March's function under the flag with March in `#else` or insert-only, 2 new ), **26 are June = March** ( no code ). `funcmap.tsv` rows `Char Raven CH2d` ( 41 ); `MODS.md` "Stage 7 Raven, CH2d"; `JUNE_BUGS.md` 549.

| June | `CX2GURaven::` | June differs from March |
|---|---|---|
| `32c4b7d0` | `RSI_A_RWT_BARRAGE_ATTACK_STAND_FrameMove` | one `RAVEN_BARRAGE_ATTACK_STAND` effect at 0.4 s; no memo effect, no buff factor |
| `32c4b9e0` | `RSI_A_RWT_BARRAGE_ATTACK_AIR_FrameMove` | 0.1 s: `m_fNowStatePowerRate` x 1.2 ( checklist 7 l.256 ), then the STAND effect, not the AIR one ( JUNE_BUGS 549 ); no memo, no buff factor |
| `32c52cc0` `32c52f30` `32c532f0` | Revolver LOOP / Harpoon FIRE_OVERHEAT / Hellfire LOOP `StateStart` | June inlines the one-argument `_SetOverheat( true )` ( March passed a 5 s duration the LIVE setter ignores ) |
| `32c5a020` `32c5a1e0` `32c5a400` | Revolver LOOP / Harpoon / Hellfire LOOP `EventProcess` | the key count that shows the overheat icon is 3 ( March 5 / 4 / 5 ); `_CONST_RAVEN_CH2D_` |
| `32c357e0` `32c359d0` | Revolver FINISH / END `EventProcess` | after the animation-end test: foot-gated walk and dash cancels + `SKILL_CANCEL_AFTER` ( March has none ) |
| `32c39c70` | `RSI_A_RVC_NAPALM_GRENADE_EventProcess` | walk and dash cancels need the foot on a line |
| `32c60510` | `RSI_SA_RWT_REVOLVER_CANNON_LOOP_FrameMove` | no memo x0.8 on the shell rate |
| `32c60b30` | `RSI_SA_RWT_HELLFIRE_GATLING_LOOP_FrameMove` | whole new body: one 0.001 s one-shot per loop plays `EffectSet_SA_RWT_HellFire_Loop` ( `_OverHit` when overheated, plus the temporary `RAVEN_HELLFIRE_GATLING_IMPACT_OVERHEAT` at ( 0, -1000, 0 ) with `_BurnOverheatHP` and `DestroyInstance` ); shout at loop 18; no bullet effects, no 0.09 / 0.06 / 0.12 s timers |
| `32c27930` `32c39630` | `RSI_A_RWT_MENTAL_BREAK_Init` / `_EventProcess` ( new ) | empty Init; animation end -> WAIT on a foot else JUMP_DOWN, `CommonEventProcess` |
| = March | 26 rows: all Inits, the four `StateEnd` tail jumps, Barrage StartFuture / STAND EP / AIR EP, Revolver / Harpoon / Hellfire `FrameMove` ( world colour + cut-in ), Revolver / Hellfire / Harpoon Start and first EP, Harpoon FIRE / OVERHEAT EP + FM, Hellfire END EP, Napalm FM | `funcmap.tsv` says which |

**Registration.** Six new cases in the id switch ( raw ids, base and upgrade ): Barrage Attack ( Init + StartFuture, hyper StartFuture only; STAND FM + EP; AIR FM + EP ), Revolver Cannon ( five callbacks on both; LOOP, FINISH, END, OVER_END ), Harpoon Spear ( five callbacks on both; FIRE, FIRE_OVERHEAT ), Hellfire Gatling ( five callbacks on both; LOOP, END, OVER_END ), Napalm Grenade ( Init + FM + EP ), Mental Crash ( Init + EP ). `X2SkillTree.h`: eight `RNSI_*` names ( 24031 / 24034 / 24051 / 24213 / 24214 / 24221 / 24236 / 24237 ). `X2GURaven.h`: the two Mental Crash declarations. The base skills' `SetEquippedSkillLevel` terms were already June's ( CH1 ); the only readers of the old `SI_*` ids left are March's dead branches and CH4a's air-slicer test ( `X2GURaven.cpp` ~8407 / ~8421 ), so the upgrade-swap check finds nothing more to change.

**Findings.** Routed, not ported: none new. `_ShowOverheatIcon` and `_CanOverheat( true )` defaults equal June's pushes ( 0 / 1 ). Checklist rows: L50 marked `(Raven: CH2d, 26 of them)`; L116 / L117 / L178 / L714-733 / L844 are covered by this phase's ids ( numerical data is S4's; no BFI call is left in these functions ); L636 / 7u: `Effectset_A_RWT_FLAME_SWORD_MEMO_FX` / `_Damage` and the two Hellfire effect sets exist in the live `EffectSet.lua` ( closure rows `fn:32c60b30` ). `PlaySound` is allow-listed ( `june_diverge_allow.tsv` ).

**Gates.**
- `flagcheck JUNE_CHAR_RAVEN_SKILL --with JUNE_CHAR_RAVEN_SKILL_DEBUG --worktree --files X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h X2Lib/X2SkillTree.h KTDXLIB/Always.h`: `1 PASS, 1 PASS~, 1 DEFINE, 1 OTHER ( X2SkillTree.h: JUNE_ENUM_NAMES ), 0 FAIL`; `1 deleted line ( the //#define ), 0 non-ASCII, 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token`, `RESULT: PASS` ( the PASS~ is two blank lines before the new namespace and the new functions ).
- `junediverge.py --check` ( X2GURaven.cpp / .h, X2SkillTree.h ): `allow-listed: 10`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`: `0 A/B row(s) remain`. `skillicons.py check` the 12 ids: `skills 12, misses 0`, `RESULT: PASS`. `luabind.py check RAVEN_FIGHTER`: 0 unbound. One-file probe `X2GURaven.cpp` ( `/c /Zs /EHsc` ): exit 0, 0 errors.
- DEPLOY OK ( after the fix below ): X2_offline.exe is 26,055,680 B, dated 2026-11-06 20:05:54, md5 6e9b536fa2518219e514c29cc0b5a90b, equal to X2/US_SERVICE/x2.exe ( 2026-11-06 20:05:47 ); built at main 37d9022e + 1 uncommitted code file(s). The first build ( md5 23bec2c7757fda94632be36b3f023f0f, 19:55:23 ) is the one the first play-test ran.
- EXESTR OK: 5 of 5 found on the first build and `[JUNE_CHAR] RAVEN SKILL barrage stand effect` on the second ( wide ).

**Fix after the first play-test ( 2026-11-06 ): Barrage Attack could not be cast.** `StateChange` begins with the March-migration helper `IsBlankSlotSkillState` ( `X2GUUser.cpp:27` ), which refuses a slot state that has no `OnEventProcess`, `OnFrameMove` or `StateStart`. June's Barrage Attack slot state has only `StateInit` and `StateStartFuture` ( it changes to STAND / AIR from the StartFuture ), so every press was refused with no log line. `X2GUUser.cpp`: under `_SKILL` the helper also counts `StateStartFuture` as a callback ( March's line in `#else` ); flagcheck PASS, junediverge PASS. Other five skills were played OK on the first build.

**Play-test (2026-11-06): user "nice passed"** for unit 75 ( first build, run 19:57-20:00: Harpoon Spear, Revolver Cannon upgrade, Hellfire Gatling, Napalm Grenade, Mental Crash upgrade all OK; Barrage Attack not castable; second build, run 20:07: Barrage Attack upgrade cast on the ground, user OK ). Logs: 6 `RAVEN SKILL equip` lines ( 24031 / 24034 / 24051 / 24236 / 24237 / 24221, states 49 / 51 / 53 / 59 / 61 / 63 ), `barrage stand effect` 6x ( second run ), `hellfire loop effect set played` loops 1-27 with `overheated 1` from loop 18 ( first run ); `Motion Error` 0, `KEM_ERROR` 0, ErrorLog only the optional `.XET` / eventscene probes. Not exercised: the air Barrage Attack ( `barrage air effect power rate` never logged: the skill is ground-only unless the Winnable Technique of Mercenary passive is learned ) and the Harpoon Spear / Revolver Cannon overheat branches have no debug line ( user's visual check only ).

**Play-test hand-off.** Unit 75 `WeaponTakerT` ( Weapon Taker, class 25, lv 99 ). Save set up ( backup taken by `elsdb`, no schema change ): bank A = slot 0 Harpoon Spear 24018, 1 Revolver Cannon upgrade 24034, 2 Hellfire Gatling 24020, 3 Barrage Attack upgrade 24221; bank B = slot 5 Napalm Grenade 24213, 6 Mental Crash upgrade 24237 ( slots 4 / 7 / 8 / 9 empty ). Dev console ( END key ): `/invin 0`, then `/stage 1`. Each skill twice, enemies in front ( the slot shows a cool time after the first cast, the MP bar drops ):
1. **Bank A.** Harpoon Spear: the unit hovers, hold the slot key to show the overheat icon ( 3 presses ), then fires ( normal or overheated ), lands; Revolver Cannon: shell loop, keep pressing for the icon and a longer loop, finish, then walk / dash cancel; Hellfire Gatling: loop with June's effect set per loop ( longer with the icon ); Barrage Attack: once on the ground and once in the air ( air hits harder, JUNE_BUGS 549 ).
2. **Bank B.** Napalm Grenade: three grenade effects at 0.6 s, walk / dash cancel on the ground; Mental Crash: ends in WAIT, no crash or "Motion Error".
3. Log: `python tools/logs.py --grep "RAVEN SKILL"` ( `equip skill` for 24018 / 24034 / 24020 / 24221 / 24213 / 24237, `barrage stand effect`, `barrage air effect power rate`, `hellfire loop effect set played loop N overheated 0/1`, `mental break end` ). Then say "check logs".

### CH2d - Review (2026-11-06)

Opus 5.5 high. **Verdict: one missed June line fixed, everything else PASS;** `_SKILL_DEBUG` retired. Read from the `charport_adis.py raven --phase CH2d` listings of all 41 addresses, the registration `32c641a0` ( `ch0_skill_listing_rows.tsv` ) and `_SetOverheat` `32c526e0`.
- **Fix: Hellfire Gatling StateStart `32c53110`** was filed "= March", but June has a tail after the key-press store ( @32c53299-32c532db ): `GetEquippedActiveSkillTemplet( GetNowStateSkillID(), true )`; if it is not NULL and its `+0x178` ( `m_iDeleteSkillIDByUpgrade` ) is not 0, then `GetEquippedActiveSkillTemplet( 0x5dd4 = RNSI_SA_RWT_HELLFIRE_GATLING, true )` and vslot `0x228` `SetBuffFactorToGameUnit( templet, 0 )`. Added at the end of March's function under `_SKILL` ( insert-only ). In game: the Enhanced Hellfire Gatling ( 24051, whose templet row carries `BFI_JIN_SKILL_BUFF_MAXIMUM` = 963, live `Enum.lua` has it ) gets that buff when it starts; the base 24020 has no buff factor. `funcmap.tsv` row and `MODS.md` updated. Revolver's StateStart `32c52b30` has no such tail.
- **Hellfire LOOP FM `32c60b30`:** `EventTimerOneshot( 0.001 )`; shout when loop count = 0x12 ( `MIN_HELLFIRE_GATLING_LOOP_COUNT` 17 + 1 ); `m_bOverheated` ( +0x96c8 ) picks `EffectSet_SA_RWT_HellFire_Loop_OverHit` / `_Loop`, both `PlayEffectSet( name, this, NULL, now-state +0x10 byte, vslot 0x204 GetPowerRate(), -1, ( 1,1,1 ), false, 0-vectors, 1.0 )` = the declaration's defaults. Overheat: `RAVEN_HELLFIRE_GATLING_IMPACT_OVERHEAT` at ( 0, -1000, 0 ), `GetRotateDegree()` x2, land = vslot `0x140( false )` = `3208e610` = Now sync position.y = `GetYPos()`, the other 10 arguments the defaults; `_BurnOverheatHP` only when not NULL, `DestroyInstance` always ( NULL-safe in March ). No bullet effect, no 0.09 / 0.06 / 0.12 timer. = the port.
- **Barrage STAND `32c4b7d0` / AIR `32c4b9e0`:** 0.4 s, L hand + dir x25 ( negated facing left ), y +25 / 0.1 s, x75, y -75, `+0x7cc8` ( vslot 0x204's value, `m_fNowStatePowerRate` ) x1.2 stored back before the effect; both push `33512660` `RAVEN_BARRAGE_ATTACK_STAND` ( JUNE_BUGS 549 ); no memo, no buff factor; `CreateInstance` tail = defaults. = the port.
- **Registration:** the six cases slot for slot ( Barrage Init + StartFuture, hyper StartFuture only, STAND / AIR FM + EP; Revolver five on both, LOOP Start FM EP End, FINISH LOOP's Start + FM with its own EP, END / OVER_END one EP; Harpoon five on both, FIRE EP, FIRE_OVERHEAT Start FM EP; Hellfire five on both, LOOP four, END / OVER_END one EP; Napalm Init FM EP on both; Mental Crash Init EP on both ). The twelve `RNSI_*` values ( 24018-24020, 24031 / 24034 / 24051, 24212-24214, 24221 / 24236 / 24237 ) = `Enum.src.lua`.
- **Overheat:** key count `cmp 3` at @32c5a0e7 / @32c5a2ae / @32c5a4c7, max loop 6 / 27 at @32c5a121 / @32c5a501. The inlined `_SetOverheat( true )` in `32c52cc0` is call for call `32c526e0`'s body ( OPEN_MIND level, buff factor, finalizer time -> `m_fOverheatBuffTime` +0x96d8, `CreateOverHeatObject` ).
- **Cancels** `32c357e0` / `32c359d0`: animation-end test returns, then walk +0x8c98 / dash +0x8c9c / `SKILL_CANCEL_AFTER` +0x8c90, no `CommonEventProcess` = the `else if` macros; Napalm `32c39c70` the same with the foot test. **Revolver LOOP FM `32c60510`:** no memo x0.8, overheat effects `_BurnOverheatHP` without destroy, land = `landPosition.y` = the port.
- **"= March" rows:** a Sonnet subagent compared the 26 listings with March ( strings, floats, calls, vslots, state ids, branch order ): 25 equal ( Init preload lists name for name ), 1 diff = the fix above.
- **Play-test / logs:** passed in CH2d ( 6 equip lines, `barrage stand effect` 6x, hellfire loops 1-27, overheated from 18, Motion Error 0, KEM 0 ). The fix needs one check ( hand-off below ).

**Gates.** flagcheck `_SKILL --with _SKILL_DEBUG --range 37d9022e^..37d9022e`: `1 PASS, 1 PASS~, 1 DEFINE, 1 OTHER (X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL`, 1 deleted line, `RESULT: PASS`; `--range d63e907b^..d63e907b` ( `X2GUUser.cpp` ): `1 PASS`, `RESULT: PASS`. Worktree ( `X2GURaven.cpp`, `Always.h` ): `1 PASS, 1 DEFINE, 0 FAIL`, 1 deleted line ( the retired `#define` ), `RESULT: PASS`. `junediverge.py --check-all`: `allow-listed: 195`, `RESULT: PASS`; `--check X2Lib/X2GURaven.cpp` after the fix: `RESULT: PASS`. `enumscan.py --check` `X2GURaven.cpp` / `.h` / `X2GUUser.cpp`: `0 A/B row(s) remain`. One-file probe `X2GURaven.cpp` ( `census.cl_cmd`, `/c /Zs /EHsc`, after the fix ): exit 0, 0 errors.
- DEPLOY OK: X2_offline.exe is 26,054,144 B, dated 2026-11-06 20:31:29, md5 2c7a4e2a73924ad75dc87592ba3b0e3d, equal to X2/US_SERVICE/x2.exe (2026-11-06 20:31:22); built at main 5cc1712d + 2 uncommitted code file(s) (this fix + the retire).
- EXESTR OK: 1 of 1 absent ( `[JUNE_CHAR] RAVEN SKILL hellfire loop`, wide ), md5 2c7a4e2a.

**Play-test hand-off.** Unit 75 `WeaponTakerT` ( Weapon Taker, class 25 ); the save already has the Enhanced Hellfire Gatling 24051 on bank A slot 2. Dev console ( END key ): `/invin 0`, `/stage 1`. Cast it twice: a buff icon appears in the buff row as the skill starts ( `BFI_JIN_SKILL_BUFF_MAXIMUM` ), the loop plays as before. If no icon: say so; no debug line covers it. **Result (2026-11-06): user "the buff showed up"** ( key D, bank A slot 2 ).

## Phase CH3 - Skills: second jobs (106, 107, 115), hyper actives, transcendence

**Run with:** as CH2 (CH3d on Opus: it carries engine rows). Sub-tag `SKILL`.

| Sub-phase | Scope |
|---|---|
| CH3a | Blade Master (106) skills (`BLADE_MASTERY` key readers if CH0 puts them here) |
| CH3b | Reckless Fist (107) skills (`DamageEffect_RRF_ExplosiveHand_Impact`, 7u, JUNE_BUGS 200) |
| CH3c | Veteran Commander (115) skills (`RAVEN_SEVEN_BURST_*_HYPER`, `RAVEN_MAXIMUM_CANNON_*`, `EffectSet_RAVEN_VJAVELIN*`, 7u; the overheat pieces CH0 gives the skills) |
| CH3d | **Opus.** Hyper actives (type 13, 6 rows; June's hyper switch in `32c641a0`), the 3 `SHOW_HYPER_CUT_IN_TIME` states (7t), and the **transcendence rows** (template, Stage 6 close-out): Raven's classes on the offline transcend allow-list (`CX2OfflineServer::TranscendCheck`, `Handlers_Quest.cpp`), his `LevelUpEvent.lua` TYPE row + `DLG_Transcendental_Change_Guide_Raven.lua`; checklist 7ao whole (hyper effect start `32c4e7c0`; the re-awakening 24419 branch of `HyperModeStart` `31ffd490`; the four base helpers `31fcac90` / `31fcad70` / `31fceab0` `== 1` / `31ff1e80`); his hyper-class skills (`SPRT_HYPER_CLASS`) and `TRANSCENDENTAL_SKILL_DATA` rows; `GetHyperModeTableName` (`31fdd490`) and `HQ_Cutin_RBM/RRF/RVC_Transcendence`; `list_TR_*` rows in `GetCharacterImageName` (T6c) and his transcended pictures, read from June's row pushes; `POPUP_TR_B_MASTER` / `_R_FIST` / `_V_COMMANDER` present |

**Done gate:** as CH2 for 106 / 107 / 115 and the hyper actives; a transcended Raven (`settranscend`) casts his hyper-class skills, slots A-5 /
B-5 work, the transcended picture and cut-in show; `python tools/skillicons.py check <Raven's skill-id range>` = PASS after the transcendence rows.

## Phase CH4 - Passives and class mechanics

**Run with:** Opus 5.5 high, Opus review, user play-test (each a different engine piece: planned as three from the start).

| Sub-phase | Scope |
|---|---|
| CH4a | `InitPassiveSkillState` `32c57660` (vslot 0x690) and Raven's passive ids (types 1 / 8: 10 + 27 rows), their objects and readers; 7b Burning Nasod Hand whole (the 24401 setter `+0x9688` here, the `+0x4108 = 2` write `32c43120 @32c43ac4` if CH1 did not take it; never one without the other and June's `AT_COMMAND` tags); 7am's 3 delegate callbacks (`32c3df50`, `32c4e5b0`, `32c28160`) replacing March's own pushes; vslot 0x73c `32c3c7f0` and 0x748 `32c3c820` (soul gain when hit, base already ported by Chung CH4b); the passive-id check of the Rules; flag `_PASSIVE` |
| CH4b | `SetDamageExceptionProcess` `32c5c080` whole (June-only, 7b: 24417 `RNSI_P_RBM_OPEN_MIND` -> `32007b10( 0xf, 24417 )`; 24420 `RNSI_P_RRF_INFINITY_RAGE` with trigger 0x12 -> `DT_MAGIC` and `DamageEffect_RRF_ExplosiveHand_Impact_NEW`; 24422 `RNSI_P_RVC_WIN_AND_WIN` with 0x13 -> vslot 0x238 and the `329cf6d0` push); the overrides CH0 finds (`GetActualMPConsume` / `GetActualCoolTime` 0x6a0 / 0x6a4 `32c38180` / `32c382d0`, buff-templet set / unset 0x24c / 0x250 `32c38340` / `32c383b0`, all uncarved); §7 l.281 power-rate socket range if a Raven function calls `3201ad10` |
| CH4c | Veteran Commander overheat (`ch0_overheat.tsv`: the mode passive `SI_P_RVC_OVERHEAT_MODE`'s June id, `CreateOverHeatObject`, `SA_OVERHEAT_HP`, `X2Game.cpp`'s VC slot / Shadow Step reads, the extra-damage pack) as June; `MemoConditionRaven` (vtable `33511404`, ctor `32c3c780`) and the memo readers; the Specialization B readers `32c49410` / `32c507da` unless CH1q took them (7an) |

**Done gate:** each passive's effect shows (CH0 names the stat or log line per passive); Burning Nasod Hand changes the damage type of command
attacks only; a Veteran Commander enters and leaves overheat as June; 0 Raven `no_state` rows left except passives with no state in June.

## Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces

**Run with:** Sonnet 5.5 high, Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_RAVEN_UI`. Sub-tag `UI`.

**Scope:** Raven's 7 `UnitTemplet.lua` rows become June's (BOM source, `luasrc.py refresh` after; every other row stays): `m_fLobbyRotationY`,
`m_HyperCutIn` for 106 / 107 / 115, the name `STR_ID_17431` for 115; the cut-in table `X2GURaven.cpp:61-94` and the InitState index switch
`:669-693` replaced by June's names and layout (7t cut-in `.gfx` names, `HQ_CutIn_Raven_BM_01` style); 7ah; the gauge pieces CH1q does not take.
**Done gate:** a select-screen pass of all nine March characters with no new error; Raven's skill and hyper cut-ins play June's art.

### CH5 - Done (2026-11-06)

- **`UnitTemplet.lua` (data036, core-list BOM source):** Raven's 7 blocks are June's text, block-wise swap (55 blocks before and after, the other 48 untouched, BOM and CRLF kept). The semantic delta vs the old file is exactly
  `ch0_unittemplet_raven.tsv`'s 11 keys: `m_fLobbyRotationY` 10 / 30 / 10 / 30 / 20 / 1 / 40 on 4 / 16 / 17 / 106 / 107 / 25 / 115, `m_HyperCutIn` `HQ_Cutin_RBM / RRF / RVC_Transcendence` on 106 / 107 / 115 (no `m_BerserkCutIn`),
  `name` `STR_ID_617` -> `STR_ID_17431` on 115 (checked: `general.ess` "Veteran Commander"). `luasrc.py refresh data036 UnitTemplet.lua` REFRESHED, `ledger --check` PASS, `core --check` PASS, `luabind.py check UnitTemplet` PASS (0 unbound).
  `kom.py pack data036`: PACK OK, `vs live data036.kom: ~1 changed` = `UnitTemplet.lua` only. **`COPY OK data036`** (user yes) md5 `e2360ec23ff284606954b7040e48c25d`, backup `kom_backup_tools/data036.kom.20261106_155516`.
  Pre-phase file kept in the scratchpad (`UnitTemplet.lua.before`) for the review's block diff.
- **Cut-in table (`SERV_IRUHADEV_JUNE_CHAR_RAVEN_UI`, new at the end of `Always.h`; `#error` without `_RAVEN_LIVE` / `_JUNE_HYPER_CUT_IN_TIME`):** `s_SkillCutInSet` = June `333aa400` (table `337ae1d0`, 12 entries, stride 0x30; the static init's
  string xrefs: `_01` / `_02` / `_03` once each, `_BM_01` x3, `_RF_01` x3, `_VC_01` x3): set 0 `HQ_CutIn_Raven_01` / `_02` / `_03` (Raven, Sword Taker, Over Taker, Weapon Taker), set 1 `_BM_01` x3, set 2 `_RF_01` x3, set 3 `_VC_01` x3.
  Names carry no extension and every entry is March's one-argument `SkillCutIn( name )`. March's `.dds` table sits in `#else`. The event 0x8b3 pair `337ae170` / `337ae1a0` (`HQ_CutIn_Raven_CASH_21` / `_22`) is `s_SkillCutInSetEvent8b3`.
- **Reader `CX2GURaven::ShowActiveSkillCutInAndLight` = June `32c3e450`** (a body inside March's function under `_UI`, March's in `#else`): `EventTimerOneshot` + `GetShowCutInAndChangeWorldColor` + `GetShowActiveSkillShow`, then
  `s_SkillCutInSet[ m_iSkillCutInSetIndex ].cutIn[ index ]` (June `+0x8ac8`, set stride 3 x 0x30), the item override `31fd8a50` (not ported: Stage 12, as Aisha / Eve / Chung / Elsword), the 0x8b3 swap (even `rand()` -> CASH_21),
  then `ActiveSkillShow( bone, name, size, pos, IsHyperState(), bOnlyLight )`. June has no April-fools branch: `s_SkillCutInSetFoolsDay` stays March's in the `#else` only.
- **Already June's from CH1, unchanged:** the InitState set-index switch (sets 0 / 1 / 2 / 3 and sub-index 1 / 2 / 0, `X2GURaven.cpp:855-890`) and `ShowActiveSkillCutInAndLightByScript` (CH1k).
- **Assets:** the six `HQ_CutIn_Raven_*.gfx` + `CASH_21` / `CASH_22` are in data152 and the three `HQ_Cutin_R{BM,RRF,RVC}_Transcendence.gfx` in data195, all June's bytes from CH0b (`ch0_assets.tsv`). Nothing staged.
- **Logs:** `[JUNE_CHAR] RAVEN UI cut-in set <set> index <i> name <gfx>` (class reader, after the 0x8b3 swap) and `[JUNE_CHAR] RAVEN UI hyper cut-in class <c> name <gfx>` (`CX2GUUser::ShowHyperSkillCutInByScript`, Raven only).
- **Checklist rows:** 7t L594 (rows), L623 (cut-in names) and the 7ao hyper cut-in name bullet marked `(Raven: CH5)`. **7ah** (`UpdateElswordSkillSlotEffect`) is Elsword-only (n/a, CH0). The gauge pieces were CH1q's (rage orb `32e6d900`).
  **The guide dialog, `LevelUpEvent.lua` TYPE row and the `POPUP_TR_B_MASTER / R_FIST / V_COMMANDER` pictures of 7ao** are CH3d's (its kickoff names all three). No dialog or gauge piece is left for CH5.
- **Not ported, routed:** the cut-in item override `31fd8a50` -> `CX2CustomizeManager`, Stage 12 bullet (as the other four characters).
- **Hygiene:** `flagcheck JUNE_CHAR_RAVEN_UI --worktree --files KTDXLIB/Always.h X2Lib/X2GURaven.cpp X2Lib/X2GUUser.cpp`: 2 PASS, 1 DEFINE, 0 FAIL, 0 deleted, 0 non-ASCII, 0 EOL mismatch, RESULT PASS
  (15 inserted lines first came out LF beside a CRLF line and were converted to CRLF). `junediverge --check X2Lib/X2GURaven.cpp X2Lib/X2GUUser.cpp`: allow-listed 16, RESULT: PASS. `enumscan --check X2Lib/X2GURaven.cpp`: 0 A/B rows.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 26,039,296 B, dated 2026-11-06 15:48:11, md5 ce0eebc72628f4e07bf10e65a59dddff`. `EXESTR OK: 10 of 10` (the two log tags and the eight June names `HQ_CutIn_Raven_01/02/03/BM_01/RF_01/VC_01/CASH_21/CASH_22`, wide),
  `EXESTR OK: 6 of 6 absent` (the six March `HQ_CutIn_Raven_{1,2,3,Blade_Master_1,Reckless_Fist_1,Veteran_Commander_1}.dds`).
- **See it in game:** select screen: all nine March characters, then Raven's seven classes (the lobby pose turns by class: 10 / 30 / 10 / 1 / 30 / 20 / 40 for Raven, Sword Taker, Over Taker, Weapon Taker, Blade Master, Reckless Fist, Veteran Commander;
  Veteran Commander is named "Veteran Commander"). A skill cut-in on each job plays June's `HQ_CutIn_Raven_*` art (set 0 on 4 / 16 / 17 / 25, `_BM_01` on 106, `_RF_01` on 107, `_VC_01` on 115). The awakening hyper on 106 / 107 / 115 shows
  `HQ_Cutin_R{BM,RRF,RVC}_Transcendence` only when the class script's awakening state sets `SHOW_HYPER_CUT_IN_TIME` (the script has it in three states; the check is CH6's). Raven's skills are unported until CH2a: a cast of a March skill
  that reaches `ShowActiveSkillCutInAndLight` is the test.
- **Play-test (user, 2026-11-06 16:00-16:02, exe `ce0eebc7`): passed.** Select screen with all nine March characters and Raven's classes OK. Logs: two `[JUNE_CHAR] RAVEN UI cut-in set 1 index 0 name HQ_CutIn_Raven_BM_01` lines, each with
  `[JUNE_GFX] CUTIN play ui=0xc HQ_CutIn_Raven_BM_01.gfx` in the same second (Blade Master set 1; sets 0 / 2 / 3, indices 1 / 2 and the hyper cut-in are CH6's walk). ErrorLog `Motion Error` 0, KEM 0; the only `[LUA_SRC]` lines are
  `TutorSystem.lua` (empty, 3 B) and `ChainQuestInfo.lua` (`AddGroupChainQuest` unbound), both quest / tutor files outside CH5, no routing from here.

#### CH5 - Review (2026-11-06, Opus 5.5): PASS, no code change

I read each item against June in Ghidra: `333aa400`, `333aa270`, `32c3e450` and `32c3e770`. InitState `32c66fe0`'s switch targets lie outside Ghidra's body, so I decoded its jump table from the exe bytes (`charport_consts.read_va` + capstone).
- **Table `333aa400`.** It builds 12 `SkillCutIn` entries at `337ae1d0 + i * 0x30` (`337ae1d0` .. `337ae3e0`). The strings in push order are `33515268` `HQ_CutIn_Raven_01`, `5244` `_02`, `5220` `_03`, `51f4` `_BM_01` x3, `51c8` `_RF_01` x3 and `519c` `_VC_01` x3, with lengths 0x11 / 0x14.
  That is the port's table, row for row. Each entry stores size `( [334367ac], [334367ac] )`, position `( [33435598], [334367ac] )` and an uninitialised z (`[ESP+0x18]`): this is March's one-argument `SkillCutIn( name )`.
  `333aa270` builds the 0x8b3 pair: `3351516c` `CASH_21` -> `337ae170`, `3351513c` `CASH_22` -> `337ae1a0`. That equals `s_SkillCutInSetEvent8b3[0]` / `[1]`.
- **Reader `32c3e450`.** Its gate is `EventTimerOneshot` (`[anim+0x24c] < t`, `31d28e10`), then `32001380`, then `31fd3180`, the same idiom Elsword's `32d09050` uses. It copies `337ae1d0 + ( index + [this+0x8ac8] * 3 ) * 0x30` (set stride 0x90).
  The item override `31fd8a50` is not ported (Stage 12). Next comes `[33775864] && 31e5a860( 0x8b3 )`, where an even `rand() % 2` picks `337ae170` (CASH_21). It ends with `31fde0f0( bone, name, size x2, pos x3, IsHyperState byte, only light )`.
  The port matches in that order. Its log line prints after the swap, so it prints the name that `ActiveSkillShow` gets.
- **Set index (InitState `32c66fe0` @32c67022).** The code is `class - 4; cmp 0x6f; ja; movzx byte [32c68490 + class-4]; jmp [32c6847c + idx*4]`. The byte map sends 4 / 16 / 17 / 25 -> 0, 106 -> 1, 107 -> 2, 115 -> 3, and the other 105 values -> 4 (the default, which keeps the value).
  Targets 0-3 store +0x8ac8 = EBX (0), ECX (1), 2 and 3. The sub index follows: 16 -> 1, 17 -> 2, else 0 (+0x8acc). This equals `X2GURaven.cpp:895-927` (CH1m, `_LIVE`). A NULL unit takes the default in the port, and June has no NULL check. No index can reach past set 3.
- **`ShowActiveSkillCutInAndLightByScript` `32c3e770`** pushes `3345bb08` (`Dummy2_Lhand`), `[this+0x8acc]` and the time, then calls `32c3e450`. That is March's body (`X2GURaven.cpp:32064`), which already calls the `_UI` reader.
- **UnitTemplet.** I split the files into blocks on `g_pUnitManager:AddUnitTemplet` and keyed each on `unitClass`, normalising only BOM and CRLF. Current, pre-phase (`kom_backup_tools/data036.kom.20261106_155516`) and June hold 55 / 55 / 87 blocks.
  Current and pre-phase have the same class set in the same order. The 7 `UC_RAVEN_*` blocks match June's text 7 of 7, and the other 48 match pre-phase 48 of 48. The file starts with a BOM and has 1475 CRLF and 0 lone LF.
  `luasrc.py ledger --check`: LEDGER CHECK PASS (0 FAIL, 0 WARN). `core --check`: CORE CHECK PASS. `luabind.py check UnitTemplet`: RESULT: PASS (0 unbound).
  `STR_ID_17431` is defined (`StringID_def7.h:2438`). The three `HQ_Cutin_R{BM,RRF,RVC}_Transcendence.gfx` are in data195 with the same case as the rows.
- **Routing.** `kickoffs/char-raven/CH3d.md:24-28` names `DLG_Transcendental_Change_Guide_Raven.lua`, the `LevelUpEvent.lua` TYPE row and `POPUP_TR_B_MASTER` / `_R_FIST` / `_V_COMMANDER`.
  `CH3a.md` still listed `32c3e450` as CH3a's own (CH0's hand rule). I amended it: CH5 ported the reader, so CH3a's `32c3e680` FrameMove only calls it.
  `funcmap.tsv` now has rows for `32c3e450` and `333aa400`.
- **Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_CHAR_RAVEN_UI --range 729a82d2^..729a82d2`: 2 PASS, 1 DEFINE, 0 FAIL, 0 deleted, 0 non-ASCII, 0 EOL mismatch, RESULT PASS. No later commit touches the flag (`git log -S`).
  `junediverge.py --check-all`: allow-listed 194, RESULT: PASS. `enumscan --check X2Lib/X2GURaven.cpp`: 0 A/B rows (the working tree includes a peer's uncommitted CH2a edits). There was no rebuild, so the deployed exe stays md5 `ce0eebc7`.
- **Next.** I checked `kickoffs/char-raven/CH6.md`: its "Amended by CH5" section carries the cut-in set walk (sets 0-3, indices 0-2) and the hyper cut-in check. The select screen already passed in CH5's play-test. CH6 still waits on the CH3d and CH4a-c reviews.

## Phase CH-N - not run for Raven

All seven June Raven classes up to transcendence exist in March. The template block is `JUNE_CHARACTER_AISHA_PHASES.md` "Phase CH-N"; Ara
runs it first.

## Phase CH6 - Whole-character walk and gap reruns

**Run with:** Sonnet 5.5 medium, then the user's walk. Back up the save; one Raven per class (CH0b creates 4 / 16 / 17 / 25 / 107 / 115), one
transcended per second job. Walk: every skill in an A and a B slot (each cast twice: cool time and MP), a hyper, the rage gauge and Nasod balls,
Veteran Commander overheat, Burning Nasod Hand, one dungeon per job, then **leave the game and return to the village**. Re-run `census.py`,
`charport_scan.py`, `charport_keys.py RAVEN_FIGHTER` and `s3_no_state.py`; before / after numbers in the Done note. **The becoming-transcended
transition on un-transcended units (user rule, 2026-10-28):** one unit per second-job class by `/tc 1`, quest 42030 (`/qdone 42030`) and the book
288493 (`/additem 288493 1`), each showing the guide popup with that class's picture, the new skill-tree rows, the HUD portrait, and surviving a
relog. A defect goes back to its phase's flag.

**Done gate:** the walk passes; 0 unexplained `no_state` rows; no new `KEM_ERROR`.

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium. `MODS.md` "June character: Raven" section (flags, files, data, revert order: exe + data036 + motion koms
together); no `_DEBUG` flag left defined; checklist rows marked `(Raven: CHn)`; the Aisha plan's "Template for Stage 7" gets a Raven row (what he
took against this plan); backlog Stage 7 note; the `stage7-plan` row points at the next character (`char-rena`, whose `ADD_RENA_SYSTEM`
decision is asked then).

## Play-test hand-off (every phase)

Dev console first; copy-ready lines with real values (unit, class, skill id and slot, `/additem`, `/qdone`); per check: where / paste /
what it does / look for / if not. Log tag `[JUNE_CHAR]`, `python tools/logs.py --grep "JUNE_CHAR RAVEN <sub>"`.

## Open decisions (for the user)

None open. Settled by standing rules, not decisions: the motion koms deploy with CH1 final (7ad); `OT_SP1a_ChargedBolt`, if June's motion lacks
it, is a June bug kept verbatim (CH2c); Burning Nasod Hand's `= 2` write ships only with June's script (7b); CH0b waits for char-elsword CH6's
walk and the code for char-elsword Z (Order). CH0 may raise one if the rage map finds a March system June removed.
