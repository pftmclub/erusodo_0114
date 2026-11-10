# June character port: Ara (Stage 7, sixth character after the pilot; the first CH-N run)

Plan key `char-ara` · branch `main` · kickoffs `kickoffs/char-ara/` · log tag `[JUNE_CHAR]` (lines start `[JUNE_CHAR] ARA <sub>`, sub-tags
`DATA`, `CORE`, `FP`, `SPECTER`, `CLASS`, `SKILL`, `PASSIVE`, `UI`, `NUM`) · flags `SERV_IRUHADEV_JUNE_CHAR_ARA_*` for Ara's class code,
`SERV_IRUHADEV_JUNE_CHAR_*` for class-agnostic pieces (+ a temporary `..._DEBUG` per sub-phase) · backlog Stage 7 (`JUNE_PORT_BACKLOG_ORDER.md`,
brief `kickoffs/june-backlog/stage7-plan.md`) · written 2026-11-10 by the `new-plan char-ara` session from the Aisha plan's "Template for Stage 7"
(and its "Phase CH-N" block) and the Rena plan (`JUNE_CHARACTER_RENA_PHASES.md`), the most recent worked example.

## Context

**What this plan does.** It moves Ara (unit type `UT_ARA` 7, script `Ara_Martial_Artist.lua`, class `CX2GUAra` in `X2Lib/X2GUAra.cpp` / `.h`,
seven classes up to transcendence: Martial Artist 7, Little Hsien 28, Little Devil 31, **Little Specter 34**, Sakra Devanam 118, Yama Raja 119,
**Asura 124**) from her March kit to her June 2017 kit, end to end: June script, skill states for June's tree, numerical damage, motions,
class-only engine hooks (`JUNE2017_CHARACTER_PORT_CHECKLIST.md` §1-§7au), force power and berserk (live March code, reworked by June), June's
Specter states and tails, UI pieces, data, her transcendence rows, and **her June-only class path Little Specter -> Asura (CH-N, the template's
first run)**.

**Correction to the brief.** The stage7 brief and the Aisha template list Ara's June-only classes as 34 / 124 and say nothing of 31 / 119: March
already ships both of her 2014 paths (Little Hsien 28 -> Sakra Devanam 118, Little Devil 31 -> Yama Raja 119; `X2Unit.h`, `StatTable.lua` rows 1-99
for 7 / 28 / 31 / 118 / 119, `X2OfflineClassChangeSeed.h`). June's third path, **Little Specter 34 -> Asura 124**, is the CH-N subject.

**Why Ara is sixth (stage7-plan, 2026-10-22).** After the plain characters, so CH-N is written once from the most mature template. The brief's
premises hold: `CUSTOM_RENDER_STATE` (7w; June's script has it twice, one row `ASI_SA_ASR_FOX_PHANTOM`), 2 new motion files (`Motion_Ara_LS.x`,
`Motion_Ara_AA.x`), the stat-row decision (Open decision 1), the orb handle retirement (7e, Open decision 2).

**Her size.** June's class spans `0x32b90b70`-`0x32bd86e0` (Elesis ends with a thunk table and the stub `32b90b60`; Chung starts `32bd86f0`):
**540** carved functions plus **32 uncarved vtable overrides** (list in `plan_june_side.md` §1), against Rena's 467 + 24; **77** vtable overrides
(Rena 62). June's script has **331** state tables, all `ASI_` (Rena 346), but **92 `ASI_SA_*`** (Rena 43): her weight is in skills. Expect more
than Rena's size (Eve-sized at 643 took 167 commits; Rena about 100). The phase table names **18 CH1 sub-phases** up front.

**Written June-body-inside-March from the start (CLAUDE.md, 2026-10-28), as Chung, Elsword, Raven and Rena.** Most June bodies have a March
function of the same name: **the body goes inside that function under `_LIVE`, March's in `#else`**, and each CH1 sub-phase proves it with a
one-file probe compile that defines `_LIVE`. A June function March's class lacks is written under `_CORE` with a plain name in the class's pattern.

**Scope: June 2017 up to transcendence (user, 2026-10-20).** June's Enum has only unnamed `UC_150`-`UC_186` placeholders for third jobs and no Ara
row for them: none is in scope. Transcendence is live (Stage 6), so her transcendence rows (118 / 119 / 124) are in CH3d and CH6.

**Order.** char-rena is at CH4b (running 2026-11-10). **CH0 runs now** (read-only, alongside anything); **CH0b after char-rena CH6's walk** (it
packs and copies koms and adds units to the save the walk uses); **every code phase after char-rena Z** (one character's code at a time:
`Always.h`, `X2GUUser.*` are shared). Lu/Ciel stays parked until Stage 7 closes (user, 2026-11-03); Elesis follows Ara and reuses CH-N.

**The state she starts from.**
- **Class code (March, live).** `X2GUAra.cpp` 17,140 lines (17,009 CRLF, 131 LF), `.h` 1,428 (1,424 CRLF), CP949. The class sits inside
  `#ifdef ARA_CHARACTER_BASE` (`.cpp:10-16260`, `.h:44-1428`); `AttackResultByType` `:17104` and `GetHypetMpdeStateID` `:17134` sit after it.
  **Every Ara macro is ON** under `US_SERVICE` (`ARA_CHARACTER_BASE`, `ARA_CHANGE_CLASS_FIRST`, `SERV_ARA_CHANGE_CLASS_SECOND`, `ADD_ARA_MEMO`,
  `FIX_SKILL_CANCEL_ARA`, `FIX_ARA_ONE_PIECE_FOR_BERSERK`, `FIX_ARA_SKILL_BUG`, ...; `cl /P` probe, `plan_march_side.md` §2): **no dormant Ara
  system**, unlike Rena's natural force. Off: `SERV_APRIL_FOOLS_DAY`, `TOGGLE_UNLIMITED_SKILL_USE`, and the small `SERV_EVENT_ARA_NEW_CHAR_*` /
  `CREATE_CHARACTER_ARA_FIRST` / `SERV_BUY_*_ARA*` / `SERV_NO_ARA` blocks (not hers to switch on).
- **Force power and berserk (live March code, June reworked).** `CForcePowerSystem` (`.h:280-306`), `SetForcePower` `:6139`, `UpForcePower`
  `:6008`, `m_bFPCancel` (84 sites); gauge `CX2AraGageData` (`X2GageData.*`), `CX2AraMyGageUI` (`X2GageUI.*`, `DLG_Ara_ForcePower.lua`), created by
  `X2GageManager.cpp:180-190`; berserk `SetFullHyperMode` `:5679`, hyper buff effects `:5703-5783`, `GetHypetMpdeStateID` `:17134`. June's pieces
  (checklist rows): `SetForcePower` `32ba6ef0` with a `+0x6d04` spend conversion (l.257, l.943), the buff hyper count (`32bb22d0` / `32bb2740`,
  vslots 49 / 50, and the count readers, l.236-237), `IsHyperModeState` `32bd85f0` (vslot 0x51c, l.603 / l.613), `IsFullHyperMode` /
  `SetFullHyperMode` (vslots 0x9c / 0xa0; `june-march-divergence` left them to this port), `GetHypetMpdeStateID` `32ba46c0` (count >= 3 ->
  `ASI_BERSERK_MODE`), the berserk table `31fdd800` (only caller `32bca580`, l.978), `ChangeMaxCharacterAddResource` `32baafc0` (slot 201, l.257),
  the gauge snapshot 0x740 `32ba4840` / 0x744 `32ba7b50` (l.258), the tutorial force power 3 (l.293), the FP-cancel byte `+0x9621` (funcmap
  l.1083), and seven June-only `INIT_COMPONENT` keys read by InitComponent `32bb0da0` (`FORCE_POWER_COUNT_AT_DUNGEON_START` / `_PVP_START`,
  `FULL_HYPER_MODE_ADVANTAGE_COUNT` / `_INTERVAL_TIME` / `_SOCKET_RATE`, `GAIN_FORCEPOWER_USEDHYPERCOUNT`, `GAIN_MP_BY_FPCONVERSION_HYPERMODE`).
  Phase **CH1q**, as Raven's rage.
- **Specter states and tails (June-only).** `IS_SPECTER_STATE` x40 (readers in `CX2GUUser`: `CommonStateStartFuture` `320506f0` and `32057b80`),
  `ANIM_NAME_TAIL` / `TRANSITION_TAIL` x7, `TAIL_TYPE` (in `PROMOTION_ITEM_ID_LIST`), the tail meshes `Motion_Ara_Tail_03.x` / `_06.x` (June data131).
  Phase **CH1q2** (Opus): a user-level state flag and a mesh attach, not class transcription.
- **Skill system (Stage 3).** `s3_reform_ids_no_state.tsv` and the S3 census: `Planning evidence`. The offline auto-learn already has rows for
  34 / 124 (`X2OfflineSkillAutoLearn.h`, `tools/data/june_skill_autolearn.tsv`: 7: 6, 28 / 31 / 34: 14 each, 118 / 119 / 124: 23 each).
- **Effect engine (June's).** `DamageEffect.lua` / `EffectSet.lua` ship merged; Ara's `c11a` / `c10a` rows: `Planning evidence`.
- **Motions.** June 75 Ara names (data205 + data131), March 17 (data033): 7 same-name files larger in June (`Motion_ARA.x` 4.9 -> 6.8 MB, `_LH`,
  `_LD`, `_SD`, `_YR`, `_Field`, `_Emotion`), 10 identical, **58 June-only** (`Motion_Ara_LS.x`, `Motion_Ara_AA.x`, `Motion_Standard_Ara.X`, the two
  tails, 53 emotions). Whether June drops a name March's script plays decides if the 7 deploy in CH0b or with CH1 final (7ad): CH0 answers.
  March's game dir has no `data205.kom`: CH0 names the target kom.
- **Summons: none.** No `CreateNPCReq` / `CreateNPC` call in her June range (raw E8 / E9 scan, uncarved code included) or in March's class.
- **The lock-on (shared with Lu/Ciel).** `SLockOnSystem::ProcessLockOn` (March `X2GUAra.cpp:5366-5476`, June `3200fa20`; checklist 7ac l.775-782:
  June's `CanDown( NULL, &lineIndex )` shape and the candidate-loop `CanDown`), used by `ASI_SA_ALH_SWALLOW_ASSAULT_*`. Lu/Ciel's CH3c20 planned to
  hoist it out of `CX2GUAra`; Ara now runs first, so **CH2b0** ports June's body in March's place and amends `kickoffs/char-luciel/CH3c20.md` to
  "hoist only".
- **June-only class path (CH-N).** `X2Unit.h` already names `UC_ARA_LITTLE_SPECTER` 34 (`:164`) and `UC_ARA_ASURA` 124 (`:178`) under
  `SERV_IRUHADEV_JUNE_ENUM_NAMES`, after `UC_END`. **`UC_END` is 124 = Asura** (compiler-checked), so every `< UC_END` test refuses her:
  `X2SocketItem.cpp:2897` (socket loader drops class >= 124), `X2ItemManager_Preprocessing.cpp:688` (logs invalid), `X2GameEdit.cpp:1643`,
  `X2OfflineLuaEnumSeed.h:117-118`. Class switches that name only March's classes: `X2GageManager.cpp:180-190` (gauge create), `X2GUUser.cpp:534-545`
  (cut-in table), `X2Unit.h:1989-2054`. June's route: Little Specter quests **35560-35563** (`CJT_JOB_THIRD`, from 7 at level 15), Asura **35564-35568**
  (from 34 at level 35), promotion items 41794 / 41796 (34) and 41803 / 41805 (124); dialogs `DLG_Class_Change_Ara_LS` / `_AS`, `DLG_1ST` / `2ND_Class_
  Change_Guide_Ara.lua` (none in March); `X2OfflineClassChangeSeed.h` / `X2OfflineLuaEnumSeed.h` lack 34 / 124; **no `StatTable.lua` rows for 34 /
  124** (Open decision 1).
- **Hooks waiting on the class port.** About 45 Ara rows in the checklist (`plan_checklist_rows.md`): the passive / attack hook 0x780 `32bca010`
  (l.330; Ara does not override 0x2b8 `SetDamageExceptionProcess` or 0x6a4 `GetActualCoolTime`), the delegate pushes (l.920 / l.930), death
  prevention with 27409 (l.258), `GetReduceDebuffDurationRate` 27415 (l.267), vslot 485 socket pass `32badc80` (l.284), `SetSkillLevelStateData`
  (l.286), the effect-set hooks 0x4c8 `32bc1530` (l.424), the slot-state byte in `32bd09d0` (l.444), 0x3ac / 0x6a8 (l.991), 0x428 `32b956a0` (l.940),
  the add-N-stacks buff path 7at (`32bbb050` / `32bbb720`, l.1031), `APPLY_MOVEMENT_SPEED` / `SPEED_TIME` rows (l.516-526), CEffect `+0xb18` writer
  `32bb0bd6` (l.312). Already in March: the vslot 510 prefix and riding-key wait in `CanUseSpecialAttack` (l.262 / l.265). l.854's `32a4dcaa` is in
  Ain's range, not Ara's (CH0 confirms and corrects the row).

**Out of scope (owned elsewhere).** Third jobs; new characters (Stage 8); UI that is not Ara's own (Stage 9); creation-screen buttons
(`DLG_Unit_Create_ClassChange_Button_Ara.lua` behaviour, Stage 9; CH0b stages the file); tutorials and training dungeons (june-dungeons); NPC / boss
copies of Ara; emotion motions' behaviour (Stage 10; CH0b stages the 53 files); `FORCE_DYNAMIC_CAMERA_INDEX` x15 / `RETAIN_CAMERA_STATE` (Stage 9);
`IMMUNITY_EDT_LIST_AT_THIS_STATE` (backlog Stage 2); the `CUSTOM_RENDER_STATE` reader itself (june-engine R5, not landed: Ara's row gets a labelled
stand-in as Raven's Power Assault, 7w); PvP-only paths (no PvP offline); the equip-view override `32bc89e0` (backlog l.452-453, Stage 8 verdict).

## Rules

The Rena plan's Rules hold (`python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules"`), with Ara's names and these changes:
- **Standing rules** (`JUNE_PORT_BACKLOG_ORDER.md`, "Standing rules"): scope rule, no partial June updates, June wins, June bugs verbatim with a
  `JUNE_BUGS.md` row, cite June addresses, dev free-skill gate (`CX2GUUser::IsDevUnlimitedSkillUse()`) in her skill-use check (March's class has none).
- **`ReservedSkillStart` (user, 2026-11-05).** March's `CX2GUAra` has no override. CH1k writes `CX2GUAra::ReservedSkillStart` =
  `SpecialAttackCooldown( skillID, m_fReservedSkillMp )` then `SkillMemoMpRefund( ... )`, widening the base hook chain in `X2GUUser.h` / `.cpp` with
  Ara's `_LIVE` flag (copy Rena's); CH1 final's play-test casts each skill twice (cool time shows, MP drops).
- **June's ids in passive code.** Every `SI_*` id March's `InitPassiveSkillState` / skill code reads is checked against June's `RNSI_*` id. CH0
  lists them; CH4a fixes what CH1 did not.
- **The checklist is the work list.** Every Ara row in §1-§7au is ported by a phase here or marked n/a by CH0 with the reason. The phase that ports a
  row appends `(Ara: CHn)` to it in place.
- **One deploy unit.** June's `Ara_Martial_Artist.lua`, her June motions (held for CH1 if CH0 finds a dropped name) and the C++ that reads them go
  live together in CH1 final, behind `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (`//#define` in `Always.h` until then). Between phases Ara stays playable on
  her March kit. Classes 34 / 124 are playable only from CHNa on.
- **Flags:** `_CORE` (June functions March lacks, compiled, unreferenced), `_LIVE` (June bodies inside March's functions, the registration, the
  switch), `_FP` (force power / berserk rework, CH1q; defined only with `_LIVE`), `_SPECTER` (Specter states and tails, CH1q2), `_CLASS` (the 34 /
  124 path, CHNa / CHNb: `UC_END`, gauge, templet, stat rows, job change, emulator seeds), `_LOCKON` (CH2b0), `_STACK` (7at, CH2c0), `_SKILL`,
  `_TRANSCEND`, `_PASSIVE`, `_UI`, `_ASSET` (only if a June asset breaks a March reader), and a temporary `_DEBUG` per sub-phase, retired in its
  review. New namespaces are `_CONST_ARA_<phase>_` (check March's existing `_CONST_*` names in `X2GUAra.cpp` first).
- **Names (CLAUDE.md, 2026-10-28).** No `June` in any identifier, no Ghidra address or phase tag in a name; a one-line `// char-ara CH1n` tag on every
  edited block (CLAUDE.md, 2026-11-09). `python tools/junediverge.py --list` before transcribing; `--check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h <other
  files>` = `RESULT: PASS` at every finish. `IsFullHyperMode` / `SetFullHyperMode` are on the divergence list for this port: CH1q replaces them.
- **Shared `CX2GUUser` helpers, never copied into the class:** `GetSkillLevel` / `GetLearnedSkillAbilityValue`
  (`SERV_IRUHADEV_JUNE_USER_SKILL_LEVEL_HELPERS`); the seven skill-use helpers and their fields (`SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS`); the
  earlier characters' `CX2GUUser` bases: a phase that needs one under its own flag widens the `#if` instead of copying it.
- **No hardcoded enum literals** (CLAUDE.md, 2026-10-26): every June id by name; `enumscan.py --check X2Lib/X2GUAra.cpp` stays 0. March's C++ enum
  `ARA_STATE_ID` (`.h:57-252`, 125 `ASI_` lines) takes June's missing names with June's ids (CH0 lists them; June's `Enum.lua` names only 38 `ASI_`,
  so the rest are resolved from the exe's state-name strings, as Rena's `ch0_state_enum_names`).
- **Shared Lua per character.** `UnitTemplet.lua` is core-list source: CH5 replaces Ara's 5 rows and adds June's 34 / 124 rows. Her class script is
  adopted whole (`adopt-june`, bytecode unless a phase must edit it) and passes `python tools/luabind.py check Ara_Martial_Artist`. The March
  `Ara_Martial_Artist.lua` is not T1 (`STATUS_ARCHIVE` C3d-4 route; `LUA_SOURCE_PHASES.md`): adoption replaces it, no conversion.
- **Assets first, whole** (CH0b), byte-exact through `kom.py stage-june`; pack and copy yourself (CLAUDE.md, 2026-11-10).
- **CP949.** `X2GUAra.cpp` / `.h` (mostly CRLF), `X2GUUser.*`, `X2Gage*.*`, `X2Unit.h`, `X2SocketItem.cpp`, `X2DamageManager.*` are CP949:
  `bytepatch.py` only; `cl /P` decides every `#ifdef` branch.
- **Uncarved overrides.** Ghidra stays read-only: the 32 uncarved functions are read from the raw listing (`charport_adis.py ara <va>`; CH0 adds `ara`
  to its `CLASS_RANGE` as `(0x32b90b70, 0x32bd86e0)`), never by creating functions.
- **Damage numbers.** Record March vs June damage per changed skill in `MODS.md`.
- **Saves.** Units 24 `arasss` (119 lv47), 38 `LittleDevilT` (31 lv70), 48 `Ara99` (119 lv99), all transcendence 0; no unit of 7, 28, 34, 118, 124.
  CH0b creates 7 / 28 / 118 (and a level-99 31); CHNa creates 34 / 124 once the class loads. Play-test saves never slot a skill in slot 4 / 9 before
  transcendence, and learn only the ids of the bank being tested.
- **Offline reach (Raven lesson 4).** Each phase's hand-off names only checks the offline client can reach; a June path that only a server or PvP
  drives is a stand-in or a routed item, decided in the phase, not found in CH6.
- **One code phase at a time in `X2GUAra.*`** (CH1-CH4). CH5 and CHNa / CHNb may run beside CH2-CH4 when their Touches do not overlap.

## Starting context

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "<heading>"` (`--no-sub` to skip Done notes) |
| Checklist rows | `python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "<heading>"`; Ara's rows with line numbers: `june2017_re/char_ara/plan_checklist_rows.md` |
| Planning evidence | `june2017_re/char_ara/plan_*` (below); CH0 writes `ch0_*` beside them |
| Character tools | `june2017_re/charport_states.py`, `charport_keys.py`, `charport_closure.py`, `charport_adis.py` (CH0 adds `ara`), `charport_regsim.py`, `s3_no_state.py`; Rena's builders `june2017_re/char_rena/ch0_*.py` (copy into `char_ara/` with the range changed); `char_arme/ch0c_listing_check.py`; `char_eve/ch0_build_assets.py` (CH0 adds `ara`) |
| June Lua (reading) | `june_src_fixed/data036_kom/Ara_Martial_Artist.src.lua`, `UnitTemplet.src.lua`, `Enum.src.lua`, `CharacterStatTable.src.lua`, `FieldQuest.src.lua`; `DLG_Transcendental_Change_Guide_Ara.src.lua` |
| Ghidra | `ghidra` skill; Ara's range `0x32b90b70-0x32bd86e0`, vtable `33505314` (544 slots, 77 overrides, all in range, 32 uncarved), RTTI `3373d62c`, ctor `32bc6bf0` (vtable write `32bc6c4d`), dtor `32bc79f0`, `MemoConditionAra` RTTI `3373d604` / vtable `33502680` (xrefs `32bc7933`, `32ba46f8`); read-only in CH0 and reviews |
| Gap scan | `cd june2017_re && python census.py && python charport_scan.py <short scratch dir>` (slow; revert the regenerated `census.tsv` and `JUNE2017_EFFECT_KEY_MATRIX.md` after, and only those), `python charport_keys.py Ara_Martial_Artist` |
| Bring June Lua in | `/x2-adopt-june` (`luasrc.py adopt-june data036 Ara_Martial_Artist.lua`, `--dry-run` first) |
| Stage June assets | `python tools/kom.py stage-june <list>`, `/elstools pack dataNNN`, `/x2-kom-packed copy dataNNN` |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply` (x2-byte-edit) |
| Hygiene / build / logs | `flagcheck.py <FLAG> --worktree --files ...`, `junediverge.py --check ...`, `enumscan.py --check ...`, `build.py --deploy` (background), `logs.py --grep "JUNE_CHAR ARA"` |
| Save / test character | `/x2-db` (`tools/elsdb.py setchar`, `settranscend <unit> 1`), `/x2-create-char <name> <class> <level>` |
| Dev console | auth level 3, END key; items `/additem <id> [qty]`, quests `/qdone <id>`, `/tc 1` |

## Planning evidence (2026-11-10)

Kept in `june2017_re/char_ara/` (digests by Sonnet subagents, spot-checked: the range ends against Elesis's thunk table and Chung's `32bd86f0`,
the 34 / 124 enum lines, the `CharacterStatTable` rows, the lock-on rows; CH0 re-verifies anything it builds on): `plan_charport_scan_20261110.txt`,
`plan_charport_keys.txt` / `.tsv`, `plan_march_side.md`, `plan_june_side.md`, `plan_numerical_rows.tsv`, `plan_checklist_rows.md`.

Scans re-run 2026-11-10: `census.py` 26 PASS, 0 FAIL, 0 `to-implement` rows (implemented 653, june-ignores 305, handed-off 8; regenerated
`census.tsv` / matrix reverted); `charport_scan.py` (scratch `C:/tmp/cps_ara`, output identical to Rena's 2026-11-07 run but for her lines):
`Ara_Martial_Artist {'no NUMERICAL_DATA_INDEX': 49, 'no DAMAGE table': 14}` (the brief's gap 14 holds) and an ENUM line `{'CUSTOM_RENDER_STATE': 1}`
(7w), no KEYS line; `s3_no_state.py --family X2GUAra.cpp --preprocess`: `rows=197 no_state=192 has_state=5` (only id 900 has a state, once per
class). **The S3 tsv has no rows for 34 / 124** (template recipe defect 2): CH0 item 11 regenerates it with them.

| Measure | Ara | (Rena) |
|---|---|---|
| June class range | `32b90b70`-`32bd86e0`, 540 carved + 32 uncarved overrides; 77 vtable overrides, all in range; 0x9c / 0xa0 (full hyper mode) and 0xd0 (role: CH0) overridden; 0x2b8 and 0x6a4 not | 467 + 24, 62 |
| June skill-tree rows | 197 over the 5 March classes (7: 15; 28 / 31: 34 each; 118 / 119: 57 each), **98** distinct ids (106 .. 27604; prefixes `RNSI_SA` 108 rows, `RNSI_A` 62, `RNSI_P` 18, `RNSI_HA` 4, `SI_ETC` 5); 0 unnamed; 192 with no class state; **34 / 124 absent** (CH0 regenerates) | 267 / 130 / 0 |
| June script | `Ara_Martial_Artist.src.lua` 21,049 lines: 331 `ASI_` states + 6 `INIT_*` (`INIT_SYSTEM` combo / skill-chain tables, `INIT_COMMON_RANDOM_STATE`; `MAX_HYPER_MODE_TIME` 30, `HYPER_MODE_COUNT` 1) + `PROMOTION_ITEM_ID_LIST`; groups SA 92, A 38, COMBO 31, DASH 29, RIDING 27, DAMAGE 21, STANDUP 8, P 8, JUMP 7, AYR 7, ASD 7, AA 7, PEPPER 5, ALS 5, ALH 5, ALD 5, HA 3, DOUBLE 3, DANCE 3, then small groups | 346 |
| Script keys | `DAMAGE_DATA` 54 (+9 `_NEXT`), `AT_COMMAND` 36, `APPLIED_SKILL_ID` 16, `TIME_STOP_IMMUNE_STATE` 3, `SHOW_HYPER_CUT_IN_TIME` 6, `SHOW_SKILL_CUT_IN_TIME` 34, `CUSTOM_RENDER_STATE` 2, `EFFECT_SET_LIST` 50 / `_EX` 61 / `_FINAL` 5 / `_FINAL_ULTIMATE` 2; class keys `IS_SPECTER_STATE` x40, `FP_CANCEL` x36, `BERSERK` x40, `ADD_FORCE_POWER` x9, `ANIM_NAME_TAIL` / `TRANSITION_TAIL` x7, `APPLY_MEMO` x2, `EFFECT_SET_LIST_EX_MEMO` x2 | 68 / 52 / 17 |
| Key census (`charport_keys.py Ara_Martial_Artist`) | 250 keys: both 225; june-only **14** (`IS_SPECTER_STATE`, `TAIL_TYPE`, the 7 `INIT_COMPONENT` force-power / full-hyper keys, `BBT_CHANGE_NOW_HP_PER_SECOND` / `BFT_TIME` x7 no reader, the camera pair (Stage 9), `IMMUNITY_EDT` (Stage 2)); neither 7 (`CUSTOM_RENDER_STATE`, `HEAD_IK` x9, 5 element keys); march-only-site 4 (CH0) | 256 / 8 / 3 / 4 |
| Numerical blocks | 160 `c1a` rows keyed to 59 ids (27000-27028, 27043-27048, 27064-27066, 27200-27228 in part, 27600-27602), all `ok`, none in March's templets | 141 / 57 |
| Motions | June 75 / March 17; 7 differ in size, 10 equal, 58 June-only (`LS`, `AA`, `Standard`, 2 tails, 53 emotions); names dropped: CH0 | 71 / 18 |
| Alpha-tag sites (7c) / March-only templets (7u) | **0** / **0** (no `c10a` or `c11a` row in her range) | 8 / 5 |
| Summons | none (June 0 sites, March 0) | traps |
| Class mechanics | force power + berserk: live March code, June reworked (CH1q); Specter + tails: June-only (CH1q2) | natural force dormant |
| March class | `.cpp` 17,140 lines, `.h` 1,428 (`ARA_STATE_ID` 125 `ASI_`); cut-in table `:36-60`, ctor `:77`, dtor `:167`, Init `:182`, ReInit `:209`, InitState `:278` (cut-in switch `:280-310`), InitStateCommon `:356-1328`, InitStateByUnitClass `:1329-1512`, InitPassiveSkillState `:1514-1888`, `SetEquippedSkillFuncBySkillSlotIndex` `:1890-2698` (51 `case SI_`), SetEquippedSkillLevel `:2700`, `SetSkillLevelStateData` `:2989`, InitComponent `:3017`, CommonFrameMove `:3472`, SpecialAttackEventProcess `:3563`, CanUseSpecialAttack `:3587-3916`, ProcessLockOn `:5366-5476`, DamageReact `:5497`, SetFullHyperMode `:5679`, UpForcePower `:6008`, SetForcePower `:6139`, HyperModeFrameMove `:7395` | 24,004 |
| June class | InitState `32bcf4a0` (0x7a4, wrapper) -> body `32bca580`; `SetEquippedSkillFuncBySkillSlotIndex` `32bd09d0` (0x7b4); SetEquippedSkillLevel `32b95cf0` (0x7b8, uncarved); InitComponent `32bb0da0`; InitPassiveSkillState `32bcd9c0` (0x690); passive / attack hook `32bca010` (0x780); SpecialAttackEventProcess `32b90c20` (0x7f4) + CanUseSpecialAttack `32bcfaa0` (0x7f8), 0x7fc `32ba6610`, 0x800 `32bb1ff0`; DamageReact `32bafa30`; hyper effect start / end `32bb22d0` / `32bb2740`; ReInit `32bc8030`; Init `32bc7e90` (uncarved); InitEffect `32b90b90`; Common* 0x7c0-0x7e8 (0x7d8 / 0x7dc uncarved); HyperModeFrameMove `32bc1a90` (0x600, uncarved); AttackResultByType `32bceca0` (0x4e0) | |
| Stat rows | `StatTable.lua` 1-99 for 7 / 28 / 31 / 118 / 119; **none for 34 / 124**; June `CharacterStatTable` relative rows: `UT_ARA` (first jobs, 7 / 28 / 31 / 34), `UC_ARA_SAKRA_DEVANAM`, `_YAMA_RAJA`, `_ASURA` (490, 112, 400, 97, 11750) | 1-99 all 7 |
| Save | units 24 (119 lv47), 38 (31 lv70), 48 (119 lv99); transcendence 0; **no unit of 7, 28, 34, 118, 124** | |
| Cut-ins | 3 `.dds` sets in data044; June's `HQ_CutIn_Ara_*` `.gfx` (incl. `_AS_*`) and `HQ_Cutin_ASD / AYR / ASR_Transcendence` `.gfx` already in March data152 / data195; June's `UnitTemplet` adds `m_fLobbyRotationY` (1 / 15 / 1 / 10 / 15 / 1 / 40 for 7 / 28 / 118 / 31 / 119 / 34 / 124) and `m_HyperCutIn` for 118 / 119 / 124 | |
| Transcendence | `DLG_Transcendental_Change_Guide_Ara.lua` (same size, md5 differs) names `POPUP_TR_S_DEVANAM` / `_Y_RAJA` / `_ASURA`: **none in March** (June data215, + `.TET`); `TranscendCheck` (`Handlers_Quest.cpp:1411`) has no Ara class: Ara transcend answers NetError 1726 today | |

## Phases

| Phase | What | Model | Depends |
|---|---|---|---|
| CH0 | Ara map: class map (uncarved overrides included), state / skill table, checklist rows, assets + effect closure, keys, registration listing check, unmapped helpers, the force-power / berserk map, the Specter / tail map, the CH-N map (34 / 124), the motion question, the `data205` target | Opus high | - (runs now) |
| CH0b | Every June Ara asset, whole, first; walk units 7 / 28 / 118 + a level-99 31 | Sonnet high, user play-test | CH0, char-rena CH6 walk |
| CH1a-CH1k | Class core transcription by state group and vslot band (12 sub-phases, below) | Sonnet high; Opus review every 2-4 phases | CH0, char-rena Z; one at a time |
| CH1m-CH1p | Wiring behind `_LIVE` (registration; base callbacks + slot head; virtuals; 7ag + hooks + June-only keys) | Opus high, Opus review each | the CH1a-CH1k reviews |
| CH1q | Force power and berserk: June's rework of the live March system, flag `_FP` | Opus high, Opus review | CH1p review |
| CH1q2 | Specter states and tails (June-only), flag `_SPECTER` | Opus high, Opus review | CH1q review |
| CH1 | Final: define `_LIVE`, adopt the script, copy any held motion koms, play-test on 7 / 28 / 31 / 118 / 119 | Opus high, Opus review, user play-test | CH1q2 review, CH0b |
| CH5 | UI and data: Ara's 5 `UnitTemplet` rows June's + the 34 / 124 rows added, cut-in table | Sonnet high, Opus review, user play-test | CH1 review |
| CHNa | CH-N (a): the 34 / 124 class loads and plays: `UC_END`, gauge create, class switches, stat rows (Open decision 1), units | Opus high, Opus review, user play-test | CH5 review, decision 1 |
| CHNb | CH-N (b): the job-change route to 34 and 124: quests, promotion items, dialogs, emulator seeds | Opus high, Opus review, user play-test | CHNa review |
| CH2a-CH2d | Skills: Martial Artist + first jobs (7, 28, 31, 34) | Sonnet high; one Opus review per stage at CH2d; user play-test | CH1 review (CH2d: CHNb review) |
| CH2b0 | Lock-on: June's `ProcessLockOn` `3200fa20` in March's place, flag `_LOCKON` | Opus high, Opus review | CH1 review; before CH2b |
| CH2c0 | Add-N-stacks buff path (7at, `32bbb050` / `32bbb720`), flag `_STACK` | Opus high, Opus review | CH1 review; before CH2c |
| CH3a-CH3d | Skills: second jobs (118, 119, 124), hyper actives, transcendence rows | Sonnet high (CH3d Opus); one Opus review at CH3d; user play-test | CH2d review (CH3c: CHNb review) |
| CH`n`0 | One Opus sub-phase per unmapped helper CH0 finds, before the Sonnet phase that needs it | Opus high, Opus review | as its phase |
| CH4a-CH4b | Passives (InitPassiveSkillState, the 0x780 hook, delegates); overrides + memo | Opus high; one Opus review at CH4b; user play-test | CH3d review |
| CH6 | Whole-character walk and gap reruns | Sonnet medium, user walk | CH4b review, CH5 review, CHNb review |
| Z | Close-out | Sonnet medium | CH6 |

CH1-CH4 all edit `X2GUAra.*`: one at a time. CH5, CHNa and CHNb touch `UnitTemplet.lua`, `X2Unit.h`, `X2SocketItem.cpp`, `X2GageManager.cpp`, the
emulator and the quest Lua: they may run beside a CH2 phase when the Touches columns do not overlap. If CH0's counts put a group over ~45
functions, CH0 splits it with a digit suffix (`CH1e2`) and writes the kickoff and STATUS row; it does not merge groups. Play-test follow-ups
after CH1 final get the next letters (`CH1r`).

**Review cadence (user, 2026-11-10).** CH1a-CH1k are inert transcriptions in one class file: reviewed in runs (CH1b covers CH1a-CH1b, CH1d2 covers
CH1c-CH1d2, CH1g covers CH1e-CH1g, CH1k covers CH1h-CH1k). CH1m-CH1q2, CH1 final, CH5, CHNa, CHNb, CH2b0 and CH2c0 keep their own reviews
(shared code). CH2a-CH2d have one review at CH2d, CH3a-CH3d one at CH3d, CH4a-CH4b one at CH4b: the earlier phases say "no review of its own" and
keep their `_DEBUG` define on; the stage review retires them and gives a verdict line to each incidental shared-file hunk.

## Phase CH0 - Ara map (read-only)

**Run with:** Opus 5.5 high. No code, no flag, no build. Ghidra read-only. Delegate reads, greps, tsv joins and known-address dumps to
subagents. Outputs in `june2017_re/char_ara/`.

**Step 0 (the pilot's recipe fixes).** Build the registration map with `charport_states.py init <InitStateCommon> 32bca580 <base> <line
registrars> <InitPassiveSkillState if it registers>` and `skill 32bd09d0` (June's 0x7a4 `32bcf4a0` is a wrapper around the body `32bca580`; find
the InitStateCommon / InitStateByUnitClass parts the body calls, `3202b110`, `32bcced0`), then run `ch0c_listing_check.py` on it: every row must
match the raw listing; all `~` rows and std::string-named rows (`USI_HYPER_MODE`, the berserk table `31fdd800`, START / WIN / LOSE roll) by hand.
Map every switch in `32bd09d0` (skill type, any hyper switch, the skill switch). Rena's builders are copied into `char_ara/` with Ara's range: say
which. Add `ara` to `charport_adis.py`'s `CLASS_RANGE`; confirm it lists the 32 uncarved overrides.

**Scope** (Rena's CH0 items, with Ara's numbers):
1. **Class map** `ch0_class_map.tsv`: every function in `0x32b90b70-0x32bd86e0` **plus the 32 uncarved overrides**, March pair, verdict, target
   sub-phase, and the **`inside` / `new`** column (propose the plain name for every `new` row). Funcmap rows `Char Ara CH0`. Confirm the role of each
   override by its body, not its vslot offset (the planning took Rena's offsets; 0xd0 `32bc19a0` is unidentified; ownership of `32bd86b0` /
   `32bd86e0` was a guess by position).
2. **State and skill table** `ch0_state_skill.tsv`: 338 / 338 script tables, every skill id; dead-data and March-only states listed; confirm nothing
   in `X2GUAra.*` still stores a state id in a `UCHAR` (7l). Map June's groups to the jobs (the planning guessed `ALH` / `ASD` Little Hsien / Sakra
   Devanam, `ALD` / `AYR` Little Devil / Yama Raja, `ALS` / `AA` (+ `ASR` skill tags) Little Specter / Asura) and say what `A`, `P`, `HA`, `SI1`,
   `DOUBLE`, `AIRIAL` are and which phase each goes to; list June's ids for every `ASI_` name `ARA_STATE_ID` lacks or numbers differently.
3. **Keys** `ch0_keys.tsv`: `IS_SPECTER_STATE` x40 (readers `320506f0` / `32057b80`; the census labelled `32057b80` "Lire ReservedSkillStart":
   check), `TAIL_TYPE`, `ANIM_NAME_TAIL` / `TRANSITION_TAIL`, the 7 `INIT_COMPONENT` keys (InitComponent `32bb0da0`), `FP_CANCEL`, `ADD_FORCE_POWER`,
   `BERSERK`, `APPLY_MEMO` / `EFFECT_SET_LIST_EX_MEMO` get an owner (class or engine; an engine reader is a `CH<n>0` or a routed item); the neither
   keys (`HEAD_IK` x9, the element keys) and the 4 march-only-site keys: does June's script drop a March reader's input or does June read it
   elsewhere; `MAX_HYPER_MODE_TIME` / `HYPER_MODE_COUNT` get their readers; `CUSTOM_RENDER_STATE`'s two rows get the stand-in owner (7w).
4. **Checklist rows** `ch0_checklist_rows.tsv`: every §1-§7au row -> phase or n/a with reason (start from `plan_checklist_rows.md`; class-generic
   rows matched only through an address: confirm; l.854 `32a4dcaa` (Ain's range): correct the row; l.1007-1009 `GetX2UnitClassCompatibility` and
   34 / 124: CHNa if it applies).
5. **Assets** `ch0_assets.tsv` + `ch0b_stage_list.tsv` (routes a name / b data / c `item.kim` Ara items incl. the 34 / 124 promotion and costume ids,
   unioned; `ch0_build_assets.py` gains `ara`) and **effect closure** `ch0_effect_closure.tsv` (`charport_closure.py`, run-time-built names
   included). **The motion kom:** June's data205 / data131 motions have no March `data205.kom`: name the March kom each goes to (March's class
   motions are in data033). **The motion question:** for each of the 7 differing class motions, the animation names March's script or class code
   plays that June's file lacks; if any, those files are held for CH1 final (`ch1_held_motions.tsv`), else CH0b copies them. `Motion_Ara_LS.x` /
   `_AA.x` / the tails stage in CH0b (nothing March plays reads them). Stage `POPUP_TR_S_DEVANAM` / `_Y_RAJA` / `_ASURA` (+ `.TET`, data215),
   `DLG_1ST` / `2ND_Class_Change_Guide_Ara.lua`, `DLG_Class_Change_Ara_LS` / `_AS`, `DLG_Class_Change_Guide_Ara.lua`, the 34 / 124 intro movies
   (`Movie_Create_Ara_AA.avi`, ...) if June ships them, the cut-in `.gfx` CH5 needs.
6. **Unmapped helpers.** Per skill, the callees outside the class range funcmap does not map (`ch0_unmapped_callees.py`). Each gets an Opus `CH<n>0`
   sub-phase before the Sonnet phase that needs it (kickoff + STATUS row written by CH0). CH2b0 (lock-on) and CH2c0 (7at) are planned already.
7. **Force-power / berserk map** `ch0_fp.tsv`: every March force-power, FP-cancel, berserk and hyper-count function paired with June's (or "absent in
   June"), plus June-only pieces (`+0x6d04` spend conversion, `+0x9621`, the buff hyper count readers of l.236-237, `31fdd800`, the 7 `INIT_COMPONENT`
   keys, `GAIN_MP_BY_FPCONVERSION_HYPERMODE`); the gauge data / UI (June's `CX2AraGageData` / `CX2AraMyGageUI` vtables) against March's; the
   `KGamePlayStatus` field June sends and whether the emulator (`X2OfflineStatTable.cpp:306`, `CAC_FORCE_POWER`) answers it right. Sized for CH1q;
   split if over ~45 functions (CH1q3 for the gauge UI).
8. **Specter / tail map** `ch0_specter.tsv`: what `IS_SPECTER_STATE` changes (the two `CX2GUUser` readers and every class reader), what
   `ANIM_NAME_TAIL` / `TRANSITION_TAIL` / `TAIL_TYPE` drive (mesh attach, anim, the tail meshes), which classes reach them (34 / 124 only, or all),
   and the base functions they need. Sized for CH1q2.
9. **CH-N map** `ch0_classn.tsv`: every client and emulator site that tests a class against `UC_END` / `UC_VALIDE_END` or switches on class with no
   34 / 124 arm (start from `plan_march_side.md` §4-§5), each with June's behaviour; June's job-change route (quests 35560-35568 in March's
   `FieldQuest.lua`? the reward `m_eChangeUnitClass` path in `Handlers_Quest.cpp:586-620`; promotion items 41794 / 41796 / 41803 / 41805 in
   `item.kim`); `CorrelateUnitClassTemplet`, `AnimationNameTable` (`ALS_*` / `AA_*` lobby), `CharCustomData` rows for 34 / 124; whether March's
   `StatTable` loader falls back to the type's rows for a class with none (and logs it). Sized for CHNa / CHNb.
10. **Shared data.** Which state names the C++ enum needs (raw id + comment, as Rena); every `SI_*` id March's passive / skill code reads with its June
   `RNSI_*` id; Ara's 7 `UnitTemplet` rows June vs March (are `STR_ID_31537` / `_31538` / `_31539` / `_31540` in our string table?).
11. **S3 census baseline:** `s3_no_state.py --family X2GUAra.cpp --preprocess` with the 34 / 124 rows (regenerate the S3 tsv with them if it lacks
   them: template recipe defect 2) so CH6 compares like with like.
12. **Amend the kickoffs** CH0b-CH5, CHNa, CHNb ("Amended by CH0" sections: filters with real column numbers, addresses, counts, any split).

**Done gate:** the tsvs exist; `ch0c_listing_check.py` passes on every registration row; every Ara checklist row has a phase or n/a; 338 / 338
tables and every skill id mapped; the 32 uncarved overrides have rows; assets union routes a / b / c and the motion kom is named; the motion
question answered; the force-power, Specter and CH-N maps exist; kickoffs amended; open questions listed.

## Phase CH0b - Every June Ara asset, whole, first

**Run with:** Sonnet 5.5 high, then a user play-test. No C++ unless CH0 found a March reader a June asset breaks (then
`SERV_IRUHADEV_JUNE_CHAR_ARA_ASSET`). Sub-tag `DATA`. After char-rena CH6's walk.

**Scope:** every non-Lua row of `ch0b_stage_list.tsv`: motions (to the kom CH0 named; held ones packed and kept for CH1 final), body / face /
hair, equipment (34 / 124 promotion sets included), textures, effect meshes, sounds, cut-in art, portraits, `POPUP_TR_*`, the tail meshes, the 53
emotion motions; the class-change dialogs (`adopt-june`, `luabind.py check`). Stage byte-exact (`kom.py stage-june`), pack each kom, copy each
(CLAUDE.md, 2026-11-10). Save `data\ErrorLog_pre_CH0b_ara.txt` first. Create the walk units the save lacks (classes 7, 28, 118 and a level-99 31)
with `createchar.py`, after a `db_backup` (34 / 124 wait for CHNa). Gate: `python tools/skillicons.py check <Ara's skill-id range from CH0>` = PASS
(stage what it lists; the shared `DLG_SKILL_Passive_*` sheets included; a June-side gap is a `JUNE_BUGS.md` row).

**Done gate:** the stage list re-runs 0 `missing` / `march-copy`; in game one Ara per March job tier, village and dungeon, no new `KEM_ERROR` /
`Motion Error` / missing texture or mesh against the pre-CH0b log; a costume, a weapon and three skills look right.

## Phase CH1 - Deploy unit: June script, motions and the class core

**Groups** (CH0's map fills the counts and the exact function lists; the kickoffs carry the filters):

| Sub-phase | Run with | Scope |
|---|---|---|
| CH1a | Sonnet high | Movement and slot-head states: wait (2) + wait-state-change, walk, jump (7), double jump (3), throw item, common buff, hyper (2) + hyper wait, charge, airial dash (`APPLY_MOVEMENT_SPEED`, l.516) |
| CH1b | Sonnet high, Opus review (covers CH1a-CH1b) | Reactions: damage (21 `ASI_DAMAGE*`), stand-up (8), die (2), start (2), win / lose blocks, wall, amusement (2), frozen, transformed |
| CH1c | Sonnet high | Riding (27 `ASI_RIDING*`), pepper run (5), dance (3) |
| CH1d | Sonnet high | Ground combos (31 `ASI_COMBO*`) |
| CH1d2 | Sonnet high, Opus review (covers CH1c-CH1d2) | Dash and dash / jump combos (29 `ASI_DASH*`) |
| CH1e | Sonnet high | Little Hsien / Sakra Devanam lines (`ALH`, `ASD`, and the `A` / `P` states CH0 assigns to them) |
| CH1f | Sonnet high | Little Devil / Yama Raja lines (`ALD`, `AYR`, and their `A` / `P` states) |
| CH1g | Sonnet high, Opus review (covers CH1e-CH1g) | Little Specter / Asura lines (`ALS`, `AA`, `ASR`, and their `A` / `P` states); Specter-flag lines under `_SPECTER` (CH1q2 switches them on) |
| CH1h | Sonnet high | Overrides below vslot 0x600 (lifecycle, render, reaction virtuals; DamageReact `32bafa30`; ctor `32bc6bf0` / dtor `32bc79f0` offset map; Init `32bc7e90` and the other uncarved ones from the listing; InitComponent `32bb0da0` with its alpha sites, the 7 `INIT_COMPONENT` keys' reads under `_FP`, the tutorial force power 3 (l.293) and the CEffect `+0xb18` writer `32bb0bd6` (l.312)) |
| CH1i | Sonnet high | Overrides 0x600-0x7bc (HyperModeFrameMove `32bc1a90` uncarved; SetEquippedSkillLevel `32b95cf0` uncarved; InitPassiveSkillState `32bcd9c0` and the 0x780 hook `32bca010` only as declarations: CH4a) |
| CH1j | Sonnet high | Common* overrides 0x7c0-0x7e8 (`0x7d8` / `0x7dc` uncarved), CommonFrameMove and the helpers only it reaches |
| CH1k | Sonnet high, Opus review (covers CH1h-CH1k) | Skill-use overrides from 0x7f0 (the seven `CX2GUUser` skill-use helpers are called, never copied): SpecialAttackEventProcess `32b90c20`, CanUseSpecialAttack `32bcfaa0`, 0x7fc `32ba6610`, 0x800 `32bb1ff0`, NoStateChangeActionFrameMove `32bc84b0` (l.866), 0x810 / 0x828 / 0x834 (uncarved), and **`ReservedSkillStart`** (Rules) |
| CH1m | Opus high, Opus review | Registration: June's InitState wrapper `32bcf4a0` / body `32bca580` against March's `InitState` / `InitStateCommon` / `InitStateByUnitClass`, the hyper-mode table names (`GetHyperModeTableName` `31fdd490`) and the berserk table `31fdd800` (l.978), START / WIN / LOSE roll (`INIT_COMMON_RANDOM_STATE`), base event states, the `_LIVE` gate |
| CH1n | Opus high, Opus review | The `CX2GUUser` base callbacks Ara registers (Rena's CH1n triage first: only rows earlier characters did not cover); `SetEquippedSkillFuncBySkillSlotIndex` `32bd09d0` head (bounds, slot tables, the 7i rule, the pre-switches, the slot-state store of l.444 dropped) + the dev free-skill gate |
| CH1o | Opus high, Opus review | The virtuals under `_LIVE`; June-only and changed user virtuals Ara overrides (0xd0 `32bc19a0` once CH0 names it); `SetSkillLevelStateData` `:2989` with the `( 3201ad10( skill ) + 1.0 )` term (§7 l.286); gaps the CH1a-CH1k reviews routed |
| CH1p | Opus high, Opus review | 7ag pressed-index space (her decode, `CountUsedSkill`, every reader together); hooks 7h / 7l / 7o / 7p / 7x; the June-only script keys CH0 gives an owner here (`FP_CANCEL`, `ADD_FORCE_POWER`, `BERSERK`, `APPLY_MEMO`) |
| CH1q | Opus high, Opus review | Force power and berserk (`ch0_fp.tsv`): June's bodies inside every March function that differs (`SetForcePower` `32ba6ef0` with `+0x6d04`, `UpForcePower`, FP cancel + `+0x9621`, `IsFullHyperMode` / `SetFullHyperMode` 0x9c / 0xa0 (divergence rows), hyper effect start / end `32bb22d0` / `32bb2740` with the buff hyper count (`GetHyperModeCountByBuff`, l.236-237), `IsHyperModeState` `32bd85f0`, `GetHypetMpdeStateID` `32ba46c0`, `ChangeMaxCharacterAddResource` `32baafc0`, gauge snapshot 0x740 `32ba4840` / 0x744 `32ba7b50` as `StoreGageSnapshot` / `RestoreGageSnapshot`, `CX2AraGageData` / `CX2AraMyGageUI`), the emulator's `CAC_FORCE_POWER` case checked; sub-tag `FP` |
| CH1q2 | Opus high, Opus review | Specter states and tails (`ch0_specter.tsv`): the `IS_SPECTER_STATE` loader and its `CX2GUUser` readers (`320506f0`, `32057b80`), the class readers, `ANIM_NAME_TAIL` / `TRANSITION_TAIL` / `TAIL_TYPE` and the tail mesh attach; class-agnostic pieces under `SERV_IRUHADEV_JUNE_CHAR_*` if June keeps them in `CX2GUUser`; sub-tag `SPECTER` |
| CH1 | Opus high, Opus review, user play-test | Define `_LIVE` (+ `_FP`, `_SPECTER`); adopt `Ara_Martial_Artist.lua`; copy CH0b's held motion koms (if any); pack, copy; the play-test on 7 / 28 / 31 / 118 / 119 |

**Transcription rules (CH1a-CH1k):** as Rena's (`JUNE_CHARACTER_RENA_PHASES.md`, "Phase CH1 - Deploy unit: June script, motions and the class
core", "Transcription rules"), with Ara's flags and her class range `0x32b90b70`-`0x32bd86e0`: `inside` rows inside March's function under
`SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` with March's in `#else` and a one-file probe compile; `new` rows under `SERV_IRUHADEV_JUNE_CHAR_ARA_CORE`; a later
group's callee declared with `// June <va>: body in <phase>`; an unmapped engine callee is "this needs Opus" (stop). June's numbers and quirks
verbatim; `m_fNowStatePowerRate` (7i) with June's operator; June's BFI arguments (7ai). **Force-power lines and Specter lines inside a function a
CH1a-CH1k phase transcribes** are written under `SERV_IRUHADEV_JUNE_CHAR_ARA_FP` / `_SPECTER` (an `#ifdef` inside the `_LIVE` body) when they call a
June piece CH1q / CH1q2 has not written yet; March's live force-power calls stay as March's until CH1q.

**Done gate (CH1 final):** Ara (7 / 28 / 31 / 118 / 119) loads June's script and motions; `[JUNE_CHAR] ARA DATA` logs the load; no new
`KEM_ERROR` / `Motion Error` / missing-animation line against a pre-CH1 run; combos, dash, jump, double jump, damage and riding play June's
animations; force power fills, spends and shows on the gauge as June; berserk enters at June's count; each skill cast twice shows its cool time and
spends MP; `charport_scan.py` / `charport_keys.py Ara_Martial_Artist` show nothing new.

## Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces

**Run with:** Sonnet 5.5 high, Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_UI`. Sub-tag `UI`.

**Scope:** Ara's 5 `UnitTemplet.lua` rows become June's and June's rows for 34 and 124 are added (BOM source, `luasrc.py refresh` after; every other
row stays; if CH0 found the templet loader refuses a class >= `UC_END`, the two new rows wait for CHNa): `m_fLobbyRotationY`, `m_HyperCutIn` for
118 / 119 / 124, the names; the cut-in table `X2GUAra.cpp:36-60` and the InitState index switch `:280-310` replaced by June's names and layout (7t
cut-in `.gfx` names); 7ah; the lobby orb handles `m_hSeqEnergyBullet1-3` (`X2UnitViewerUI.*`, 7e) per Open decision 2. **Done gate:** a
select-screen pass of all nine March characters with no new error; Ara's skill and hyper cut-ins play June's art; the second-job names show June's
strings.

## Phase CH-N - Ara's June-only class path: Little Specter (34) and Asura (124)

The template block is `JUNE_CHARACTER_AISHA_PHASES.md` "Phase CH-N"; Ara runs it first, as two phases. Elesis (35 / 125) and Add reuse what lands
here: every fix is class-generic unless June's own code is Ara-only. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_CLASS` for Ara's rows,
`SERV_IRUHADEV_JUNE_CLASS_PATH` for the class-generic `UC_END` / switch fixes (one flag, reused by Elesis and Add). Sub-tag `CLASS`.

| Sub-phase | Scope |
|---|---|
| CHNa | **The class loads and plays** (`ch0_classn.tsv`): every `UC_END` / `UC_VALIDE_END` test that refuses 124 (the socket loader `X2SocketItem.cpp:2897` first; `X2ItemManager_Preprocessing.cpp:688`, `X2GameEdit.cpp:1643`, `X2OfflineLuaEnumSeed.h:117-118`) gets June's bound; the class switches with no 34 / 124 arm (`X2GageManager.cpp:180-190` creates Ara's gauge, `X2GUUser.cpp:534-545`, `X2Unit.h:1989-2054`, `GetX2UnitClassCompatibility` l.1007-1009) get June's arms; `X2OfflineLuaEnumSeed.h` names them; **stat rows** for 34 / 124 per Open decision 1 (server Lua `KncWX2Server/ServerResource/US/StatTable.lua`: the user packs it; until then the loader's type fallback, logged); `Motion_Ara_LS.x` / `_AA.x` reach the unit; the units: `createchar.py` a 34 and a 124 (and a level-99 124 for CH3). Play-test: both load in the village and a dungeon, combos and the gauge work, the second-job name shows, a socketed weapon keeps its socket. |
| CHNb | **The job-change route** (`ch0_classn.tsv`): June's quests 35560-35563 (to 34) and 35564-35568 (to 124) in March's `FieldQuest.lua` (adopt June's rows if March lacks them: core-list rule), the reward's `m_eChangeUnitClass` path through `Handlers_Quest.cpp:586-620`, `X2OfflineClassChangeSeed.h` rows for 34 / 124 (promotion items 41794 / 41796, 41803 / 41805 and parent rows 34 -> 7, 124 -> 34), `DLG_Class_Change_Ara_LS` / `_AS` and the class-change guide dialogs; `luabind.py check` on each dialog. Play-test: a level-15 Martial Artist takes 35560-35563 (`/qdone`) and becomes Little Specter; a level-35 Little Specter takes 35564-35568 and becomes Asura; both survive a relog. |

**Done gate:** a 34 and a 124 reached both by job change and by `createchar.py` load, fight and relog; the socket and stat paths log no refusal; no
other class changed (a select-screen pass of all nine March characters).

## Phase CH2 - Skills: Martial Artist and the first jobs (classes 7, 28, 31, 34)

**Run with:** Sonnet 5.5 high (transcription from CH0's map), one Opus review at CH2d (covers CH2a-CH2d), user play-test per phase. Flag
`SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL`. Sub-tags `SKILL`, `NUM`. A skill's line decides its phase: class 7 has it -> CH2a; else the first of 28 / 31 / 34.

| Sub-phase | Scope |
|---|---|
| CH2a | Martial Artist (7) skills |
| CH2b0 | **Opus.** Lock-on (checklist 7ac l.775-782): June's `SLockOnSystem::ProcessLockOn` `3200fa20` whole inside March's (`X2GUAra.cpp:5366`), June's `CanDown( NULL, &lineIndex )` shape and the candidate-loop `CanDown` (`@3200fcad`), the landing helper `32bc0420`; flag `_LOCKON`; amend `kickoffs/char-luciel/CH3c20.md` to "hoist only" (own review) |
| CH2b | Little Hsien (28) skills (Swallow Assault `@32bc204c` / `@32bb70ff` on CH2b0's lock-on) |
| CH2c0 | **Opus.** Add-N-stacks buff path (7at, l.1031-1035): `32bbb050` / `32bbb720` (both new), the count through `CX2GageManager::PushBuff` (june-hud H3 gave it one); flag `_STACK` (own review) |
| CH2c | Little Devil (31) skills (`SA_ALD_WOLF_Break` on CH2c0's path) |
| CH2d | Little Specter (34) skills (after CHNb: tested on a 34 unit); CH2 stage review |

For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole; its attached rows (7u names, 7ai BFI arguments,
7h UPDOWN cancels, the power-rate operator, the numerical-only tables of `plan_numerical_rows.tsv`, `SPEED_TIME` rows l.522-526); its effect
closure (a name found only now is staged and added to `ch0_assets.tsv`); its force-power gain / cost lines (`ADD_FORCE_POWER`, `FP_CANCEL`) on
CH1q's code. Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every
term that reads the id. `NUM use` lines for Ara skills; March vs June damage in `MODS.md`.

**Done gate:** every class-7 / 28 / 31 / 34 id in the S3 census has a state (except CH0's passive / CH4 ids); each skill in an A and a B slot casts
June's state, motion and effect, deals damage, shows its cool time and spends MP; no missing particle / mesh / texture line.

## Phase CH3 - Skills: second jobs (118, 119, 124), hyper actives, transcendence

**Run with:** as CH2 (CH3d on Opus: it carries engine rows); one Opus review at CH3d (covers CH3a-CH3d). Sub-tag `SKILL`.

| Sub-phase | Scope |
|---|---|
| CH3a | Sakra Devanam (118) skills |
| CH3b | Yama Raja (119) skills |
| CH3c | Asura (124) skills (after CHNb); `ASI_SA_ASR_FOX_PHANTOM`'s `CUSTOM_RENDER_STATE` as a labelled stand-in until june-engine R5 lands (7w, Raven's Power Assault shape) |
| CH3d | **Opus.** Hyper actives (type 13; June's hyper switch in `32bd09d0`), the 6 `SHOW_HYPER_CUT_IN_TIME` states (7t), and the **transcendence rows** (template, Stage 6 close-out): 118 / 119 / 124 on the offline transcend allow-list (`CX2OfflineServer::TranscendCheck`, `Handlers_Quest.cpp:1411`), her `LevelUpEvent.lua` TYPE rows + `DLG_Transcendental_Change_Guide_Ara.lua`; checklist 7ao whole (hyper effect start `32bb22d0` returns at once when transcended; the four base helpers `31fcac90` / `31fcad70` / `31fceab0` `== 1` / `31ff1e80`); her hyper-class skills (`SPRT_HYPER_CLASS`) and `TRANSCENDENTAL_SKILL_DATA` rows; `GetHyperModeTableName` (`31fdd490`) and `HQ_Cutin_ASD / AYR / ASR_Transcendence`; `list_TR_*` rows in `GetCharacterImageName` (T6c) and her transcended pictures, read from June's row pushes; `POPUP_TR_S_DEVANAM` / `_Y_RAJA` / `_ASURA` present; CH3 stage review |

**Done gate:** as CH2 for 118 / 119 / 124 and the hyper actives; a transcended Ara of each second job (`settranscend`) casts her hyper-class skills,
slots A-5 / B-5 work, the transcended picture and cut-in show; `python tools/skillicons.py check <Ara's skill-id range>` = PASS after the
transcendence rows.

## Phase CH4 - Passives and class mechanics

**Run with:** Opus 5.5 high, one Opus review at CH4b (covers CH4a-CH4b), user play-test (two engine pieces, planned as two from the start; CH0
adds CH4c if the memo or a passive engine piece is over ~45 functions).

| Sub-phase | Scope |
|---|---|
| CH4a | `InitPassiveSkillState` `32bcd9c0` (vslot 0x690) and Ara's passive ids, their objects and readers; the passive / attack hook 0x780 `32bca010` whole (l.330; June's `RNSI_*` ids 27415 / 27417 ...); the delegate registrations (l.920: `32bcd9c0`, `32bb0d10`) replacing March's own pushes (l.930); death prevention with 27409 (l.258, the `32bc5a50` test); `GetReduceDebuffDurationRate` `32bd32f0`'s 27415 tail (l.267); the passive-id check of the Rules; flag `_PASSIVE` |
| CH4b | Overrides and memo: `GetActualMPConsume` 0x6a0 `32b96730` (uncarved), 0x3ac `32ba45d0` (March's 4-arg `ModifyFinalDamageByGameUnitType` body moves to June's override, l.991) and 0x6a8 `32b958e0`, 0x428 `32b956a0` (l.940), vslot 485 socket pass `32badc80` with its list `32d3f040` (l.284), the effect-set hooks 0x4c8 `32bc1530` (l.424); `MemoConditionAra` (RTTI `3373d604`, vtable `33502680`, `32ba46f0`) and the memo readers (`APPLY_MEMO`, `EFFECT_SET_LIST_EX_MEMO`); `P_AYR_GOOD_ENERGY_BALL` on CH2c0's path; §7 l.281 power-rate socket range if an Ara function calls `3201ad10`; CH4 stage review |

**Done gate:** each passive's effect shows (CH0 names the stat or log line per passive); 0 Ara `no_state` rows left except passives with no state in
June.

## Phase CH6 - Whole-character walk and gap reruns

**Run with:** Sonnet 5.5 medium, then the user's walk. Back up the save; one Ara per class (7 / 28 / 31 / 34 / 118 / 119 / 124), one transcended per
second job. Walk: every skill in an A and a B slot (each cast twice: cool time and MP), a hyper, force power (fill, spend, FP cancel, gauge) and
berserk, a Specter state on 34 / 124, the passives, one dungeon per job, both job changes to 34 / 124 (CHNb's quests on fresh units), then **leave the
game and return to the village**. Re-run `census.py`, `charport_scan.py`, `charport_keys.py Ara_Martial_Artist` and `s3_no_state.py`; before / after
numbers in the Done note. **The becoming-transcended transition on un-transcended units (user rule, 2026-10-28):** one unit per second-job class
(118, 119, 124) by `/tc 1`, quest 42030 (`/qdone 42030`) and the book 288493 (`/additem 288493 1`), each showing the guide popup with that class's
picture, the new skill-tree rows, the HUD portrait, and surviving a relog. A defect goes back to its phase's flag.

**Done gate:** the walk passes; 0 unexplained `no_state` rows; no new `KEM_ERROR`.

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium. `MODS.md` "June character: Ara" section (flags, files, data, revert order: exe + data036 + motion koms together;
the class-path flag `SERV_IRUHADEV_JUNE_CLASS_PATH` named as Elesis's and Add's base; the user-packed `StatTable.lua` rows); no `_DEBUG` flag left
defined; checklist rows marked `(Ara: CHn)`; the Aisha plan's "Template for Stage 7" gets an Ara row (what she took against this plan, and what CH-N
took: the template block is rewritten from it); backlog Stage 7 note; the `stage7-plan` row points at the next character (`char-elesis`, CH-N reused).

## Play-test hand-off (every phase)

Dev console first; copy-ready lines with real values (unit, class, skill id and slot, `/additem`, `/qdone`); per check: where / paste /
what it does / look for / if not. Log tag `[JUNE_CHAR]`, `python tools/logs.py --grep "JUNE_CHAR ARA <sub>"`.

## Open decisions (for the user; asked at the end of planning)

1. **Stat rows for Little Specter (34) and Asura (124)** (backlog "Open decisions that gate stages": A2's permission does not extend to a new
   class id). No June per-level table exists (June ships only the client `CharacterStatTable` relative rows; no June server Lua). But Ara's own rows
   show the rule exactly (`StatTable.lua`, checked 2026-11-10): **28 and 31 equal class 7 at every level 1-99**; **118 and 119 equal class 7 except
   level 1** (= 0.8 x June's second-class relative row: 118 L1 `384, 320, 92, 77.6, 11750` = 0.8 x `480, 400, 115, 97` and HP 11750) **and the
   `cap99_gen.py` levels 84-99** (rounding from the different L1). Options: **(a, recommended)** follow that rule: 34 = class 7's 99 rows
   verbatim; 124 = class 7's rows with level 1 = 0.8 x June's `UC_ARA_ASURA` row (`490, 112, 400, 97, 11750` -> `392, 320, 89.6, 77.6, 11750`, the
   same column order as 118's) and 81-99 regenerated by `cap99_gen.py`; written by CHNa into `KncWX2Server/ServerResource/US/StatTable.lua` as a
   labelled stand-in under `_CLASS`; you pack it. It is Lu/Ciel's answer (2026-11-02, derive from June's relative rows) narrowed to Ara's own
   evidence. **(b)** copy Little Devil's / Yama Raja's rows (31 -> 34, 119 -> 124): the same at every level but Asura's L1 and 84-99. **(c)** no rows:
   the loader's type fallback, logged (the template's interim; Asura would never get her own L1).
2. **Retire the lobby orb handles** (checklist 7e, l.368). March hand-made Little Devil's three lobby orbs (`m_hSeqEnergyBullet1-3`,
   `X2UnitViewerUI.*`); you kept them on 2026-10-07. Since C9g-2 adopted June's `Motion_ARA_Common.x` and `AnimationNameTable.lua`, June's table row
   `ALD_LobbyWait` plays the same three particles and the hard-code is dead (keyed `ALD_lobbyWait`). **(a, recommended)** retire the three handles under
   the unit viewer's flag with March's code in `#else`, as the other eight classes' hard-codes were (CH5); **(b)** keep them as dead code.

Settled by standing rules, not decisions: the motion koms deploy with CH1 final if June drops a name March plays (7ad); `CUSTOM_RENDER_STATE` on
Fox Phantom is a labelled stand-in until june-engine R5 (7w, Raven's shape); the lock-on is ported by Ara (CH2b0) and hoisted later by Lu/Ciel;
June-side icon gaps are `JUNE_BUGS.md` rows; the 53 emotion motions are staged, their behaviour is Stage 10.
