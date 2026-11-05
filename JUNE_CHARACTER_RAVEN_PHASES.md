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
