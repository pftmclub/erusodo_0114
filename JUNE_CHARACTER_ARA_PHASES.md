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

**Out of scope (owned elsewhere).** Third jobs; new characters (Stage 8); UI that is not Ara's own (Stage 9); tutorials and training dungeons (june-dungeons); NPC / boss
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

### CH0 - Done (2026-11-10)

Read-only: no flag, no build. Outputs in `june2017_re/char_ara/`; tables live in the tsvs, per-phase detail in each kickoff's "Amended by CH0".

**Tooling.** Rena's builders copied with Ara's range / vtable / registrars: `ch0_build_vtable.py`, `ch0_skill_listing_emu.py` (now follows the 86
`helper( state*, id )` cases and runs the second switch), `ch0_march_live_states.py`, `ch0_build_state_skill.py`, `ch0_build_state_enum_names.py`,
`ch0_build_keys.py`, `ch0_build_class_map.py`, `ch0_make_hand_names.py` (+ merge of `ch0_new_names.tsv`), `ch0_unmapped_callees.py`,
`ch0_passive_ids.py`, `ch0_build_enum_names.py`, `ch0_build_unittemplet.py`, `ch0_numtables.py`, `ch0_build_checklist_rows.py`, `ch0_amend_kickoffs.py`,
`ch0_funcmap_rows.py`. `charport_adis.py` and `char_eve/ch0_build_assets.py` gained `ara` (Rena's asset outputs byte-identical through the new script).
**S3 fix (template defect 2):** `s3_no_state.py --append-missing` appended June's 34 / 124 tree rows (91) to `s3_reform_ids_no_state.tsv`; the old file is
a byte prefix of the new one. Rebuild order: the class-map builder docstring (`NO_HAND_NAMES=1` build, `make_hand_names`, build).

**Step 0.** `charport_states.py init 32bca580 32bcf4a0 3202b110 32bcced0` -> `ch0_reg.tsv`; `ch0c_listing_check.py` **162 / 162**. The five std::string
rows read by hand: 23 `USI_HYPER_MODE` (`31fdd490`), 155 `ASI_BERSERK_MODE` (`31fdd800`), 9 / 11 / 12 START / WIN / LOSE (`31fe22d0`, base callbacks).
`32bd09d0`: memo pre-switch, type switch (12 entries), id switch (244 entries, 62 targets), id 27600 by compare, hyper switch (5 entries). **Second
switch:** vslot 0x68c `32bd34c0` = `InitAquiredActiveSkillState( skill id )` (March `X2GUAra.cpp:16262`), 56 more skill states.
`ch0_skill_listing_rows.tsv` 246 rows.

**Maps.**
- `ch0_class_map.tsv`: 1,408 starts (540 carved): state-fn 542, thunk 547, helper 198, override 77 (all 32 uncarved have rows), dead 35, not-Ara 7.
  Per phase: CH1a 30, CH1b 17, CH1c 3, CH1d 29, CH1d2 26, CH1e 13, CH1f 20, CH1g 28, CH1h 36, CH1i 14, CH1j 19, CH1k 16, CH1m 3, CH1n 2, CH1o 4,
  CH1q 25, CH1q2 1 (+ `ch0_specter.tsv`), CH2a 48, CH2b 45, CH2b2 28, CH2c0 2, CH2c 36, CH2c2 25, CH2d 49, CH2d2 31, CH2d3 29, CH3a 34, CH3a2 35,
  CH3b 44, CH3c 43, CH3c2 19, CH3d 8, CH4a 17, CH4b 9, n/a 31. Hand routings: `ch0_hand_phases.tsv` (col 4 = the March function). 15 `new` rows
  keep `?` (overrides whose role the note gives, passives for CH4a / CH4b to name).
- Overrides named by body: 0xd0 `CreateGageData` (CH1q), 0x29c `GetReduceDebuffDurationRate` (CH4a), 0x634 `UpdateEquipCollisionData` (CH1q2),
  0x670 `IsInJobLine` (CH1i), 0x68c `InitAquiredActiveSkillState` (CH1n), 0x688 a jmp to the base (n/a); `32bd86b0` / `32bd86e0` = her empty
  `AppendDeviceToDeviceList` / `AppendComponentToDeviceList` (empty in March too), `32bd86c0` unreferenced.
- `ch0_state_skill.tsv`: **338 / 338** (319 mapped, 12 dead data, 6 `INIT_*`, `PROMOTION_ITEM_ID_LIST`), **139 / 139 ids** (22 passive, 2 stateless);
  0 March-only states; no state id in a `UCHAR` (7l). Groups: `ALH` / `ASD` Little Hsien / Sakra Devanam (CH1e), `ALD` / `AYR` (CH1f), `ALS` / `AA` /
  `ASR` (CH1g); `A` / `SA` / `HA` / `P` / `SI1` by skill. June's `USI_END` is 152 (March 100): all 105 shared `ASI_` names differ by 52; register by
  June's id (`ch0_state_enum_names.tsv`).
- `ch0_keys.tsv` (250): 14 june-only, 7 neither, 4 march-only-site, each with an owner; `32057b80` is `CX2GUUser::CommonStateStart`.
- `ch0_checklist_rows.tsv`: 133 rows, 87 phase, 46 n/a. Eight rows corrected in place (l.121, 237, 286, 666, 775-776, 781, 854 (`32a4dcaa` is Ain's), 868).
- `ch0_passive_ids.tsv`: 65 March `SI_*` reads, 41 differ (March 7000s vs June 27000s), 9 cross-line. `ch0_enum_names.tsv`: 136 of 139 June ids need
  registering. `ch0_unittemplet_ara.tsv`: all string ids present. S3 baseline `ch0_s3_baseline.txt`: 288 rows, 281 `no_state` (34: 34, 124: 57).

**Force power** (`ch0_fp.tsv`, 114 rows): FP lives in the gauge (`+0xc04`); `SetForcePower` converts a spend into MP x spec B (`+0x6d04`); max FP 10;
`+0x9621` FP-cancel probe; 7 `INIT_COMPONENT` keys -> members -> readers mapped; gauge vtables match March's. CH1q about 44 functions: **not split**.
**Specter / tails** (`ch0_specter.tsv`, 19 rows): one reader (`AttackResultByType` Specter of Thunder); `ANIM_NAME_TAIL` / `TRANSITION_TAIL` are March's
already, June-only is `TAIL_TYPE`; CH1q2 about 14. **CH-N** (`ch0_classn.tsv`, 45 rows): live breaks are the socket bound, class compatibility, the
gauge create (**a crash today**: `X2GUAra.cpp:3028` casts to `CX2AraGageData`), `GetClassLevel`, `createchar.py`; the quest route is already June's.

**Assets.** `ch0_assets.tsv` 12,474 rows; **`ch0b_stage_list.tsv` 5,886**: `STAGE-JUNE DRY RUN: 34 kom(s), 5886 staged (5562 new, 324 replaced, 0
identical), 0 refused`; new kom **data205** (56 June-only motions); class motions -> data033; tails -> data131. **Motion question:** `Motion_Ara_LD.x`
(case: `_START` vs `_Start`) and `_SD.x` (`FallingBlossom`) drop names March plays: **held for CH1** (`ch1_held_motions.tsv`); `_ARA`, `_Field`,
`_Emotion`, `_LH`, `_YR` go now. Closure 7,600 rows (136 missing files, all in the list). Icons: 74 misses on 5 sheets, all in the list but 27414
(June's own `.TET` gap: CH0b row).

**Item 6.** `ch0_unmapped_identified.tsv` 64 rows: 49 same, 4 differs, 9 june-only, 2 unknown (inside Opus phases); **needs-opus 0**: no `CH<n>0`
phase. `CX2GUUser::GetPressedSkillState` `31fd3360` differs (5 slots per bank): `june_march_divergent.tsv` row + allow row, replaced in CH1p (7ag).
`funcmap.tsv`: rows `Char Ara CH0`.

**Splits and kickoffs.** CH2b -> CH2b / CH2b2, CH2c -> CH2c / CH2c2, CH2d -> CH2d / CH2d2 / CH2d3 (the CH2 stage review moves to CH2d3),
CH3a -> CH3a / CH3a2, CH3c -> CH3c / CH3c2. CH2a (48), CH2b (45), CH2d (49), CH3b (44) stay whole. Every kickoff CH0b-CH6 has "Amended by CH0".

**Plan-doc inconsistencies.** Swallow Assault is a Sakra Devanam (118) skill in June (27011): CH3a, not CH2b; the "landing helper `32bc0420`" is
Specter of Thunder's (CH2d), not CH2b0's; l.312's CEffect writer is in a CH2d helper, not CH1h; `X2GUUser.cpp:534-545` and `X2OfflineLuaEnumSeed.h`
need no CHNa edit; the StatTable type fallback does not log; 41794 / 41796 / 41803 / 41805 are costume pieces, not promotion items;
`plan_checklist_rows.md` calls 34 / 124 Little Devil / Yama Raja.
**Drift:** none (the class 31 / 119 lobby idle names dropped by the live June `Motion_ARA_Common.x` / `_Lobby.x` go to CH5). **Open for the user:** none.

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

### CH0b - Done (2026-11-10)

Sonnet 5.5. No C++, no flag. Assets, 9 adopted guide / class-change Lua and the save only. Full build skipped: no code change (the play-test is on assets). **Copied (CLAUDE.md 2026-11-10, no ask):** 35 `COPY OK`; `kom.py status`: only `data101` left (see below).
- **Reference:** `ErrorLog.txt` (451,995 B) saved as `data\ErrorLog_pre_CH0b_ara.txt` before staging.
- **Staged:** `kom.py stage-june --jobs 8 june2017_re/char_ara/ch0b_stage_list.tsv`: `STAGE-JUNE OK: 34 kom(s), 5886 staged (5562 new, 324 replaced, 0 identical), 0 refused, 36s` (dry run matched CH0).
  Replaced March files backed up under `kom_backup_stage_june61110_164654`. Held, not staged: `Motion_Ara_LD.x`, `Motion_Ara_SD.x` (`ch1_held_motions.tsv`; March's copies untouched, CH1 final stages them). New kom **data205** (56 June-only motions); differing class motions in data033, tails in data131.
- **Lua:** `adopt-june data034` (3 guide dialogs: `DLG_1ST_` / `DLG_2ND_Class_Change_Guide_Ara` new, `DLG_Class_Change_Guide_Ara` replaced) and `adopt-june data217` (`DLG_Class_Change_Ara_LD` / `_LH` / `_SD` / `_YR` replace March's in data034; `_AS` / `_LS` new in data217; June's kom for the six is data217, not data034): both `ADOPT-JUNE OK (bytecode)`, 9 staged, 0 refused; backups `20261110_164706` / `_164724`.
  `luabind.py check` over the 9: 0 unbound each; `luasrc.py ledger --check` PASS (0 FAIL, 0 WARN); `core --check` PASS.
- **Packed:** `kom.py pack` x36 (34 + data034 + data217), 36 `PACK OK`, 0 removed entries except data101.
- **data101 not copied:** its one Ara row (`Velder_Gate_Stage2_Sky.dds`, `march-copy`) is staged in `kom_contents\data101_kom`, but the directory is out of step with the live kom: `copy` refuses (`drops 1070 entries`). Same as Chung's CH0b (MODS.md "Stage 7 Chung, CH0b"); `--allow-removed` was not used. The file is a Velder Gate sky texture, not seen on Ara's character; routed to the data101 directory fix (existing backlog), nothing lost.
- **Done-gate re-run:** `stage-june --dry-run` over the same list: `5886 staged (0 new, 0 replaced, 5886 identical), 0 refused`. 0 `missing` / `march-copy`.
- **Skill icons:** `skillicons.py check 106 900 27000-27066 27200-27244 27400-27420 27600-27605`: 141 skills, 0 misses, 1 known June gap, `RESULT: PASS`. Gap: 27414 `SI_P_AA_SPIRITUAL_EYE` is absent from June's own `DLG_ARA_SKILL_ScreenShot_04.TET` (sheet files identical to June's): `JUNE_BUGS.md` row 582 + `skillicons_june_gaps.tsv` row.
- **Save:** backup `db_backup\els_db_20261110_164859_pre_manual.sql`; `createchar.py` (lv 99, class costume + top gear +10, skills in slots 0-3 only, none in slot 4 / 9): unit 96 MartialArtistT (7), 97 LittleHsienT (28), 98 SakraDevanamT (118), 99 LittleDevil99 (31). Existing: 24 arasss (119 lv47), 38 LittleDevilT (31 lv70), 48 Ara99 (119 lv99). 34 / 124 wait for CHNa.
- **MODS.md:** "Stage 7 Ara, CH0b" section with the backups and the revert.
- **Play-test (user) is the gate** (below). Log tag `[JUNE_CHAR]` has no lines (no code); the check is `ErrorLog.txt` against `ErrorLog_pre_CH0b_ara.txt` (`KEM_ERROR`, `Motion Error`, missing texture / mesh). Class script and motions are still March's, so Ara plays as before; her June meshes / textures / sounds / icons load.
- **Logs checked (user play-test passed, run 16:55-16:59, after the deploy):** `ErrorLog.txt` (1,464 lines) against `ErrorLog_pre_CH0b_ara.txt`: **0** `KEM_ERROR`, **0** `Motion Error`, 0 `EXCEPTION`; the 5 `UNHANDLED` are `E_DISABLE_HB_CHECK_REQ` (known). New lines, none a missing asset the list should have carried:
  - `OpenXET` sidecars (every class logs them); Ruben-village textures, `PerturbationMap.png`, `HQ_WhiteBack`, `Raven_Out.lua`, `AddGroupChainQuest`, the `ClassChange*Class` control misses (Elesis logs them too): not Ara's, existing backlog.
  - Ara's dangling face-texture names and `Movie_Create_Ara_*.lua`: June's own, `JUNE_BUGS.md` row 583.
  - **Open, routed to CH1 final:** ~21 `ErrorEnum(433)` `Anim Frame : 80, Mesh Frame : 81` on `Mesh_Ara_Basic_Face_Sorted.X` / `Mesh_Ara_Cash_Face_Berserk_Sorted.X` (the pre-log never loaded Ara's body, so it is not a regression against it). June's face mesh (15,918 B) replaced March's (12,718 B) while the face animation is still a March / lobby 80-frame one; CH1 final (June's motions and script) re-runs the check, and a still-present line there becomes a `JUNE_BUGS.md` row. `ARA_COMBOSHOT.OGG` / `EXPLOSION3.OGG` `Update3DPosition result 47` (June bytes / existing sound).
- **Drift:** none new. (**Amended, user 2026-11-10:** Ara's entries on the character-creation screen belong to this port, not Stage 9: CH5 for classes 7 / 28 / 31 / 118 / 119, CHNa for 34 / 124; `ch0_assets.tsv` owner is now `char-ara` / `CH5`.)

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
| CH1h2 | Sonnet high | `AttackResultByType` `32bceca0` whole body (routed by CH1h): FP conversion + berserk hit roll under `_FP`, Specter of Thunder under `_SPECTER`, Get Blood / Raid / Mana Drain under `_SKILL` |
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

### CH1a - Done (2026-11-10)

All 30 class-map rows of phase CH1a are written, all `inside`: June's body in March's own `CX2GUAra` function under `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`, March's in `#else`. No `new` row, so no `_CORE`; no `_FP` / `_SPECTER` line was needed (no FP helper `32babba0`, `+0x9621`, `+0x6d04` or Specter key in these 30 functions).
`Always.h` gets `_LIVE` as `//#define` with one guard (needs `SERV_IRUHADEV_JUNE_BUFF_AWAKE_LOCK`: `IsAwakeImpossible`, `IsQuickSlotSpecialItemWaiting`). `X2GUAra.cpp` gets the macro `ELSE_IF_STATE_CHANGE_ON_EX_OR_ANIMATION_END_` (June's expansion, copied from Rena's: flag fires on `stateChange < now || IsAnimationEnd`) under `_LIVE`. `X2GUAra.h` gets two members under `_LIVE`, with their constructor init (CH1h need not repeat them).
`june2017_re/char_ara/ch1_offsets.tsv` is new (Rena's class-agnostic rows + the Ara rows); 30 funcmap rows `Char Ara CH1a`; checklist rows L178 / L428 / L516-L521 carry `(Ara: CH1a)`.

| June | `CX2GUAra::` | June | `CX2GUAra::` |
|---|---|---|---|
| `32b90e10` `32b96d80` `32bb2d40` | Wait StartFuture / Start / EventProcess | `32baa1c0` `32baa1f0` `32bb2ff0` `32b90e40` | HyperWait StartFuture / Start / EventProcess / StateEnd |
| `32ba6fd0` | PlayAnimationHyperWaitStart | `32bb3290` | Walk EventProcess |
| `32ba73c0` `32b96db0` `32babee0` | JumpReady / JumpUp / JumpDown EventProcess | `32b90e60` `32bb34d0` | JumpLanding Start / EventProcess |
| `32b90f80` `32b97880` `32b97900` `32bb4200` `32bb43a0` `32b90fb0` | AirialDash StartFuture / Start / FrameMoveFuture / FrameMove / EventProcess / EndFuture | `32b91160` | StateChangeAirialDashIfPossible |
| `32b90fd0` `32ba7820` `32b979f0` | DoubleJump StartFuture / Front EventProcess / Up EventProcess | `32b9a0b0` `32ba79b0` | JumpAttackZ EventProcess / FrameMoveFuture |
| `32b9a1e0` `32ba7a40` `32b9a240` | JumpAttackX FrameMove / FrameMoveFuture / EventProcess | `32b90f20` | HyperModeStart: a thunk to `CX2GUUser::HyperModeStart` `31ffd490`, March registers it already: nothing to write |

**Where June differs from March** (everything else is March's logic written out whole, read from `charport_adis.py ara` listings and the Ghidra decompile):
- **AirialDash FrameMoveFuture `32b97900`:** the key logic runs only after `m_fEventTime[0].stateChange < m_pXSkinAnimFuture->GetNowAnimationTime()`; before that the dash keeps moving. The dash ends in `USI_DASH_JUMP` on pure-down, or when no direction key agrees with the facing (`m_FrameDataNow.syncData.bIsRight`); a new byte `+0x9628` (`m_bAirialDashFromCombo`) lets either direction key hold the dash, and it is cleared when the dash becomes a dash jump.
- **AirialDash Start `32b97880`:** with `+0x9628` and `+0x9629` (`m_bAirialDashTurnAround`) set it adds 180 to the Future `dirDegree.y`, clears `+0x9629` and toggles the Future `bIsRight` (the unit turns round for a backward dash). Both bytes are written by the combo states ComboZZX `32b987f0`, ComboZZZXAttack `32b986a0` and ALH_XXXZ `32ba4870` (CH1d / CH1e); until then they stay false. Constructor zeroes them (June `32bc6bf0` clears the word).
- **Hyper entry in Wait / HyperWait / Walk / JumpLanding:** June calls the virtual `GetHypetMpdeStateID()` (vslot 0x6b8; the `push 0` before the call is `StateChange`'s bool) where March writes `3 <= count ? ASI_BERSERK_MODE : USI_HYPER_MODE`. June's Ara override `32ba46c0` is `count < 3 ? 0x17 : 0x9b` = March's override line for line, so the call is exact today; CH1q still owns that function and the count readers.
- **JumpAttackZ `32b9a0b0`:** the `ARA_CHANGE_CLASS_FIRST` arm runs unconditionally with June's macro form (animation-end clause, classes 28 / 118 -> `ASI_JUMP_ATTACK_ALH_ZZ`). **DoubleJumpFront `32ba7820`:** the Sakra Devanam back-Z arm is likewise unconditional. **Riding arms** (`GetRidingOn()`) run with no `RIDING_SYSTEM` test.
- **= March after the macros:** the rest. Skip-point set of JumpLanding = `ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS` (15 keys checked); `+= 30` / `-= 45` = `LINE_RADIUS` x 1 / 1.5; `GetEnableDash( 2 )` = `EDS_END`; `GetWalkSpeed()` inline = triple x `m_fRunJumpSpeedRate`; `CanDown`'s five extra June arguments are March's defaults (funcmap `31c6c310`). The June `PlayAnimationHyperWaitStart` reads `PLAY_TYPE` and `TRANSITION` through two XOR-built key names (decoded by hand).
- **Not Ara-range code:** `ASI_WAIT_STATE_CHANGE`, `ASI_COMMON_BUFF`, `ASI_THROW_ITEM` and the hyper / berserk / jump-landing registrations use base callbacks (nothing to write; registration is CH1m). Skill 900 `SI_ETC_WS_COMMON_LOVE` (the class-level skill of this phase): March's `ST_RELATIONSHIP_SKILL` case in the skill switch already binds `ASI_THROW_ITEM` with `COMMON_RELATIONSHIP_SKILL_FrameMove` / `_EventProcess`, as June; June's switch body arrives with CH1n. March's `ARA_STATE_ID` numbers differ from June's (ch0 `DIFF` rows): every state is written by name. No `JUNE_BUGS` row: nothing here is a June defect.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h KTDXLIB/Always.h`: `files: 2 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out (by design). `git diff --patience --numstat`: 739 / 0 in the `.cpp`, 4 / 0 in the `.h`, 14 / 0 in `Always.h`.
- One-file `/Zs` probe of `X2GUAra.cpp` (`census.cl_cmd`, no PCH): 0 errors with `_LIVE` off and with `/D SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (the `/P` output carries the June branch: `m_bAirialDashFromCombo` 6 hits, 0 with the flag off).
- `junediverge.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: 0 A/B rows.
- Full build skipped: no play-test (`_LIVE` is off; nothing registers a June body), so no `DEPLOY OK` / `exestr` line.

**Nothing to play-test.** Phase tag in every block: `// char-ara CH1a`. Review: none of its own, the CH1b review covers CH1a-CH1b (the `_DEBUG` define stays on: there is none yet). Next: `/x2-phase char-ara CH1b` (Sonnet 5.5 high).

### CH1b - Done (2026-11-10)

All 17 class-map rows of phase CH1b are written, all `inside`: June's body in March's own `CX2GUAra` function under `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`, March's in `#else`. No `new` row, so no `_CORE`; no `_FP` / `_SPECTER` line was needed. `_LIVE` stays `//#define` (CH1a defined it with its guard). 17 funcmap rows `Char Ara CH1b`; no `ch1_offsets.tsv` row was missing (the CH1b rows were there since CH1a); checklist rows L178 and 7ai carry `(Ara: CH1b)`.

| June | `CX2GUAra::` | June | `CX2GUAra::` |
|---|---|---|---|
| `32b96b30` `32b96bc0` `32b90dd0` `32b96c30` | DieFront StartFuture / Start / FrameMove / EventProcess (also `ASI_RIDING_DIE`) | `32b96c70` `32b96cd0` `32b90df0` `32b96d40` | DieBack StartFuture / Start / FrameMove / EventProcess |
| `32b96f20` `32b96f70` | DamageDownFront FrameMove / EventProcess | `32b97350` `32b97390` | DamageDownBack FrameMove / EventProcess |
| `32b97770` | DamageAirDownLanding EventProcess (a jump to `32b96f70`) | `32b97780` `32bab2d0` | StandUpRolling EventProcess / End (9 states) |
| `32b97800` `32b90f40` | StandUpAttack EventProcess / End (4 states) | | |

**Where June differs from March** (everything else is March's logic written out whole, read from `charport_adis.py ara --phase CH1b`):
- **Stand-up flags also fire on animation end** (DamageDownFront / Back flags 5 / 6 / 7: quick stand front-front / front-back / attack, and back-front / back-back / attack): June tests `stateChange < now || IsAnimationEnd()` and runs the time tests before the key test; this is the file-local `ELSE_IF_STATE_CHANGE_ON_EX_OR_ANIMATION_END_` CH1a defined. The animation-end branch above it catches the same condition first, so the extra clause never decides (kept as June has it). The key arms, the no-down roll (`GetRandomInt() < +0x4060`, the same attack state either way) and Back's id swap are March's; the future animation accessor `32070cf0` is read as `m_pXSkinAnimFuture` (precedent: Rena / Aisha, funcmap `32070cf0`).
- **StandUpRolling End `32bab2d0`:** the Breaking Avoidance memo block (`ASI_A_ALH_BREAKING_AVOIDANCE` and memo `0x94323` = `SMI_ARA_MEMO7` 607011) is unconditional (March: `ADD_ARA_MEMO`) and calls the buff factor with June's five arguments `( BFI_BUFF_BREAKING_MEMO 0xb5, 1, NULL, 0, false )` (7ai; `s3e_bybfi_callers.tsv` row `32bab361`: skip 0, adjust 0, level 1, creator 0). June's memo query `31fca9e0` returns false (March replaces it under `SERV_IRUHADEV_JUNE_DIVERGE_SKILL_MEMO`), so the block is dead in June; no `JUNE_BUGS` row (memos are gone for every class).
- **Die StartFuture `32b96b30`:** the riding pet's dying state comes from `32d278f0` (`transformed ? +0x578 : m_DyingState`); the transform flag `+0x5a0` has no March member (Stage 17, offline it never starts), so it reads `m_ptrRidingPet->m_DyingState` with a one-line stand-in comment. June has no `RIDING_SYSTEM` test.
- **DamageDownFront FrameMove `32b96f20`:** `EventTimerOneshot( 0.19 )` (the inlined `m_fAnimOneshotTimer < t` guard) = March's `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` branch; Back is plain `EventTimer( 0.32 )`, as March.
- **= March:** Die Start (`DyingStart` vslot 0x338, play speed 0.5), Die FrameMove (`DieFrameMove( 0.19, 1.12 )`), Die EventProcess (`GetShowObject` inlined; `StateChange( GUSI_DIE )`), AirDownLanding, the Rolling / Attack EventProcess (`USI_JUMP_DOWN` and `y -= LINE_RADIUS x 1.5`, then dash on a pure key else `USI_WAIT`), the Attack / Rolling End invincibility (0.3 / 0.3, vslots 0x11c / 0x120).
- **Ids:** by March's names (`USI_*`, `ASI_*`); the June numbers differ (ch0 `DIFF` rows) and registration is CH1m. **Not in this phase though the state list names them:** the Start / FrameMove of the damage, win / lose, start and wall states are base callbacks (nothing in Ara's range; CH1m registers them). `ASI_WALL_LANDING` has no script table (state_skill note) and no Ara function.
- **Identified callee:** `32d278f0` (riding pet dying id) as above; `32d2f480` = March `CX2RidingPet::StateChangeFuture( int )`.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --worktree --files X2Lib/X2GUAra.cpp`: `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out (by design). `git diff --patience --numstat`: 352 / 0.
- One-file `/Zs` probe of `X2GUAra.cpp` (`census.cl_cmd`, no PCH): 0 errors with `_LIVE` off and with `/D SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`; the `/P` output carries June's `( BFI_BUFF_BREAKING_MEMO ), 1, 0, 0, false )` call with the flag on only.
- `junediverge.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: 0 A/B rows.
- Full build skipped: no play-test (`_LIVE` is off; nothing registers a June body), so no `DEPLOY OK` / `exestr` line.

**Nothing to play-test.** Phase tag in every block: `// char-ara CH1b`. Review: `kickoffs/char-ara/CH1b-review.md` (Opus 5.5 high, covers CH1a-CH1b; the `_DEBUG` define stays on: there is none yet). Next: `/x2-phase char-ara CH1b review`, then CH1c.

### CH1b - Review (2026-11-10, covers CH1a-CH1b)

**Verdict: pass, no code change.** The 47 functions of CH1a (`812d7465`) and CH1b (`b0b65a46`) were read against `charport_adis.py ara` listings (StateChange pushes, `fcomp` / `test ah` direction); no transcription error was found.
- **DamageDown Front / Back EventProcess `32b96f70` / `32b97390`:** exact. Flags 5 / 6 / 7 fire on `stateChange < now || IsAnimationEnd()` before the key test (`test ah,5` / `jnp`, then the `+0x2ac` byte); the key window is `keyInputStart <= now < keyInputEnd` (`test ah,0x41` `jne` / `jp`). States: Front `ASI_P_ALH_QUICK_STAND_FRONT_FRONT` 0xd8 / `ASI_STANDUP_ROLLING_FRONT_BACK` 0x9e (June, like March, uses the plain rolling state for Front's flag 6) / `ASI_STANDUP_ATTACK_FRONT` 0xa1; Back 0xda / 0xdb / 0xa2. No-down roll `GetRandomInt() < +0x4060` -> `_NO_DOWN` (0xd6 / 0xd7), the key arms and the facing swap are exact.
- **StandUpRolling End `32bab2d0`:** the state test is June's 0xde = `ASI_A_ALH_BREAKING_AVOIDANCE` 222 (`ch0_state_enum_names`), memo `0x94323` = `SMI_ARA_MEMO7`, and June's pushes `( 0xb5, NULL, 1, 0, 0 )` map onto March's `( BFI, level 1, creator NULL, adjust 0, skip false )` order (`X2GameUnit.h:3214`). The memo query is the June-false one.
- **Die StartFuture `32b96b30`:** the `32d278f0` stand-in reads `m_DyingState`; the inlined `SetPlaySpeed( 0.5 )` matches. StandUpRolling EventProcess `32b97780` is exact.
- **CH1a:** AirialDash Start `32b97880` (180 deg turn + `sete` toggle of the Future `bIsRight` only when both bytes are set) and FrameMoveFuture `32b97900` are exact. The dash keeps going while `(pureRight && (isRight || +0x9628)) || (pureLeft && (!isRight || +0x9628))`, and it becomes `USI_DASH_JUMP` on pureDown, clearing `+0x9628`. `GetHypetMpdeStateID()` at the four hyper sites is the virtual vslot 0x6b8; June's override `32ba46c0` is `count < 3 ? 0x17 : 0x9b` = March's. Wait EventProcess order and `SpecialAttackEventProcess( ASUT_GROUND = 1 )` (air sites push 2 = `ASUT_AIR`) are exact. JumpAttackZ `32b9a0b0` (classes 0x1c / 0x76 = 28 / 118) and DoubleJumpFront `32ba7820` (the Sakra back-Z arm via two `GetIsRight` calls, the oneUp test `nowSpeed.y <= 0`) are exact.
- **`ch1_offsets.tsv`:** the CH1a / CH1b rows agree with the listings. The review added 7 offsets that the listings used without a name: `+0x9640` `m_bAbleQuickStand`, `+0x7e40` position.y, `+0x30c` `passiveSpeed.x`, and the vslots 0x1c8 `GetRandomInt`, 0x11c / 0x120 invincibility, 0x21c buff factor.
- **Found, routed to CH4a:** June writes `+0x9640` only in the constructor `32bc6bf0` (zero), and no Ara function queries skill 7400 `SI_P_ALH_QUICK_STAND`, so June's quick-stand flag is always false. June's `InitPassiveSkillState` `32bcd9c0` (CH4a, whole body) therefore has no quick-stand setter. Until then, March's setter (`X2GUAra.cpp:1874-1886`) stays live under `_LIVE`. One line was added to `kickoffs/char-ara/CH4a.md`.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --range 812d7465^..b0b65a46`: `files: 3 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is `//#define` (by design).
- `junediverge.py --check-all`: `allow-listed: 202`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: 0 A/B rows.
- One-file `/Zs` probe of `X2GUAra.cpp` (`census.cl_cmd`): 0 errors with `_LIVE` off and with `/D SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (the `/P` output carries `m_bAirialDashFromCombo` 6 times).
- Full build skipped: no play-test. There is no `_DEBUG` define to retire.

Next: `/x2-phase char-ara CH1c` (Sonnet 5.5 high); its kickoff was checked and is current.

### CH1c - Done (2026-11-10)

All 3 class-map rows of phase CH1c are accounted for: 2 written (both `inside`), 1 thunk with nothing to write. No `new` row, so no `_CORE`; no `_FP` / `_SPECTER` line was needed. `_LIVE` stays `//#define`. 3 funcmap rows `Char Ara CH1c`; checklist L178 carries `(Ara: CH1c)`; `JUNE_BUGS.md` rows 584 and 585.

| June | `CX2GUAra::` | what |
|---|---|---|
| `32b90f30` | RidingHyperModeStart | a thunk (`jmp 31fe96c0`) to `CX2GUUser::RidingHyperModeStart`; March registers it for `ASI_RIDING_HYPER_MODE` already: nothing to write |
| `32bc5ba0` | RidingHyperModeFrameMove | `CommonHyperModeFrameMove( 0, 0.1, 0.1, true )`, = March (inside, `_LIVE`) |
| `32bc0140` | CommonHyperModeFrameMove | inside, `_LIVE`: see below |

**The other 34 states of the kickoff list are base callbacks.** Riding (27 `ASI_RIDING*`), pepper run (5), dance (3) and the amusement phoenix-run states have no function in Ara's range: every callback is a `CX2GUUser` function (June's thunks `329cb180`-`329cc960` jump to them; the targets are all outside `0x32b90b70`-`0x32bd86e0`) that March's `InitState` registers already, so nothing is transcribed (registration is CH1m). `ASI_RIDING_DIE` uses CH1b's DieFront functions.

**Where June differs from March** (`CommonHyperModeFrameMove`, read from `charport_adis.py ara 32bc0140`):
- **Third timer has no body** (`32bc03cf`: `EventTimerOneshot( t3 )` result dropped). March plays `AraVoice_BoostPower.ogg` there (full hyper only) and from the second timer when `t2 == t3`; June has neither (JUNE_BUGS 584; twin of Chung 490). The kept empty `else if` stays.
- **Second timer:** June adds the awakening effect-set family `32011d20` (begin), `31fd66b0` (present test), `32011f50` (play, only when present) before the shock particle and the trace `31ffda00` only when the set is absent. These are stand-ins in `_CONST_ARA_CH1C_` (`AraAwakeningEffectBegin` / `SetPresent` = false / `Play`, "not ported (Stage 12), June's empty-name default"), the same as Chung / Rena / Raven / Elsword / Eve.
- **= March:** the world color `0xff222222`, the two `EventTimerOneshot` timers in an else-if chain, `ShowMinorParticleHyperModeChange` / `Shock` with the default bone `Dummy1_Rhand`, `UpDownCrashCamera( 30, 0.4 )` (`32290350`), `ClearScreen` (inlined: the DG manager's color block set to 1.0), `ApplyHyperModeBuff` (vslot 0xc0), the three sounds when `bSound`, and the tail vslot 0x7cc `CommonFrameMove`. The `MaxHyperModeTime` label in `charport_adis` for `+0x24c` is the anim's one-shot timer (March `m_fAnimOneshotTimer`).
- **Caller:** the ordinary `HyperModeFrameMove` (`32bc1a90`, CH1i) calls the same helper with `( 0.693, 0.759, 1.287 )`; so row 584 hits every Ara awakening.
- **Not found to differ:** nothing in June's body reads `IsFullHyperMode` / force power, so no `_FP` line. The `ASI_RIDING_DAMAGE_FRONT` double table and the missing `ASI_RIDING_DAMAGE_BACK` table (kickoff note) are June script facts: JUNE_BUGS 585, kept verbatim (CH1 adopts the script as is).

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --worktree --files X2Lib/X2GUAra.cpp`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is `//#define` (by design). `git diff --patience --numstat`: 53 / 0.
- One-file `/Zs` probe of `X2GUAra.cpp` (`census.cl_cmd`, no PCH): 0 errors with `_LIVE` off and with `/D SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`; the `/P` output has `AraAwakeningEffectBegin` 2 times with the flag on, 0 off.
- `junediverge.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: 0 A/B rows.
- Full build skipped: no play-test (`_LIVE` is off), so no `DEPLOY OK` / `exestr` line.

**Nothing to play-test.** Phase tag in every block: `// char-ara CH1c`. Review: none of its own, the CH1d2 review covers CH1c-CH1d2 (the `_DEBUG` define stays on: there is none yet). Next: `/x2-phase char-ara CH1d` (Sonnet 5.5 high).

### CH1d - Done (2026-11-10)

All 29 class-map rows of phase CH1d are written: 28 `inside` (June's body in March's own `CX2GUAra` function under `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`, March's in `#else`) and 1 `new` (`ASI_COMBO_ZZZXAttack_StateStart` `32b91000`, under `_CORE`, referenced by nothing until CH1m). `Always.h` gets `_CORE` (defined) and `_FP` (`//#define`; needs `_LIVE`); `_LIVE` stays `//#define` and now also needs `_CORE`, `SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS` and `SERV_IRUHADEV_JUNE_ENUM_NAMES` (checked defined before it). 29 funcmap rows `Char Ara CH1d`; checklist L178 carries `(Ara: CH1d ...)`; `JUNE_BUGS.md` row 586; `MODS.md` section "Stage 7 Ara, CH1d".

| June | `CX2GUAra::` | June | `CX2GUAra::` |
|---|---|---|---|
| `32b97ae0` `32b97d10` `32b90ff0` `32b97f00` `32b98460` | ComboZ / ZZ / ZZZ(FrameMove) / ZZZ(EventProcess) / ZZZZ | `32b985b0` `32b98600` `32bb4630` `32b98620` `32b98680` | ZZZXStart EP / ZZZXCharge Start / FrameMove / EP / End |
| `32b91000` (new) `32bb4820` `32b986a0` `32bb0080` | ZZZXAttack StateStart / FrameMove / EP / End | `32b987f0` `32b98a90` `32b98bd0` | ZZX / X / XX EP |
| `32bb0120` `32b98d00` `32bb01c0` `32b91040` `32b98f00` | XXX Start / EP, XXXloop Start / FrameMove / EP | `32b91050` `32b91070` `32b99370` `32b91080` | XXXloopX Start / FrameMove / EP / End |
| `32b910a0` `32b995b0` `32b99890` | XXdownX FrameMove / EP, XXdownXX EP | | |

**Where June differs from March** (read from `charport_adis.py ara --phase CH1d`; the rest is March's logic written out whole):
- **Flag macro:** every `ELSE_IF_STATE_CHANGE_ON_EX_` of June's combos is CH1a's `_OR_ANIMATION_END_` form (ComboX / XX use March's plain tests, written out). `SKILL_CANCEL_AFTER` is the shared June macro (on-line test, awakening cancel). New file-local `BWALK_CANCEL_AFTER_ON_LINE_` / `DASH_CANCEL_AFTER_ON_LINE_` (June adds the on-line test; Eve has the same pair).
- **Job-line branches June adds** (March had Little Hsien / Devil only for ZZXX / ZZZ-front-Z): ZZZ flag 3 -> `ASI_COMBO_ASR_ZZZX` (Asura, X); X flag 1 -> `XXdownX` (`IsInJobLine( 2 )`); XXX flag 1 -> `ALH_XXXZ` (line 1); XXX-loop flags 1 / 2 / 3 -> `ALS_XXXLoopZ` (line 3), `ALH_XXXZ` (line 1), `XXdownX` (line 2, down + X); XXX-loopX flag 0 -> `ALS_XXXLoopZ`; XXdownX flag 1 -> `AA_XXDownXZStart` (Asura, Z), flag 2 -> `ALD_XXdownXX` (line 2, X). `IsInJobLine` (vslot 0x670, `32ba4710`: 1 = classes 28 / 118, 2 = 31 / 119, 3 = 34 / 124) is declared under `_CORE`, body in CH1i (Chung / Elsword precedent). Five state names `ASI_COMBO_ALH_XXXZ` (317) / `ALD_XXdownXX` (322) / `ALS_XXXLoopZ` (282) / `AA_XXDownXZStart` (284) / `ASR_ZZZX` (326) join `ARA_STATE_ID` under `_CORE` (CH1m maps, CH1e / CH1f / CH1g own the states).
- **ZZX:** a double key (`pureDoubleLeft` / `Right`) inside `m_fEventTime[2]` while airborne starts the airial dash and sets `m_bAirialDashFromCombo` / `m_bAirialDashTurnAround` (the two CH1a members that nothing set until now).
- **FrameMoves:** ZZZ / XXX-loop / XXX-loopX / XX-down-X are `CommonFrameMove` only (June has no `DAMAGE_DATA_NEXT` swap); ZZZXAttack no longer scales damage at 0.331 s: `ASI_COMBO_ZZZXAttack_StateStart` does it, `fPhysic *= 1 + 0.25 x charge` (March x1..x3, June x1..x1.5; in `MODS.md`).
- **XX-down-XX (FP cancel):** June's `m_iChosenSkillID` + the Reflection test (`31edf580` = `CX2UISkillTreeNew::IsSameOrUpgradeSkill( id, RNSI_A_AYR_REFLECTION 27213 )`, registered in `X2SkillTree.h`; March tests `SI_A_ALD_REFLECTION`). `StateChangeSpecialAttack` / `CanUseSpecialAttack()` stand in for June's vslots 0x7fc / 0x7f8 (CH1k). The +0x9621 probe byte is `m_bFPCancelProbe` under `_FP` (set before the test, clear after; CH1q).
- **JUNE_BUGS 586:** XX with down + X sets flag 0 as well as flag 1, flag 0 fires first: XX-down-X is unreachable from XX.
- **Not found to differ:** ZZZZ, ZZZXStart / Charge / Attack EP and End, XXX / XXX-loop Start, XXX-loopX Start / End. No branch drifted out of scope.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --with ..._CORE ..._FP --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h KTDXLIB/Always.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is `//#define` (by design). Same with `_CORE` as the flag: `define: KTDXLIB/Always.h:12207`, `RESULT: PASS`. `X2SkillTree.h` under `SERV_IRUHADEV_JUNE_ENUM_NAMES`: `RESULT: PASS`. `git diff --numstat`: cpp 466 / 0, `.h` 14 / 0, `Always.h` 24 / 0, `X2SkillTree.h` 5 / 0.
- One-file `/Zs` probe of `X2GUAra.cpp` (`census.cl_cmd`, no PCH): 0 errors with `_LIVE` off, with `/D ..._LIVE`, and with `_LIVE` + `_FP`; the `/P` output carries `ASI_COMBO_ASR_ZZZX` 1 time with `_LIVE` off (the enum) and 2 with it on.
- `junediverge.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: 0 A/B rows.
- Full build skipped: no play-test (`_LIVE` is off), so no `DEPLOY OK` / `exestr` line.

**Nothing to play-test.** Phase tag in every block: `// char-ara CH1d`. Review: none of its own, the CH1d2 review covers CH1c-CH1d2 (no `_DEBUG` define exists yet). Next: `/x2-phase char-ara CH1d2` (Sonnet 5.5 high).

### CH1d2 - Done (2026-11-10)

All 26 class-map rows of phase CH1d2 are written: 25 `inside` (June's body in March's own `CX2GUAra` function under `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`, March's in `#else`) and 1 new (`ASI_DASH_COMBO_ZpushX_ATTACK_StateStart` `32b91120`, under `_CORE`, referenced by nothing until CH1m). Five state names join `ARA_STATE_ID` under `_CORE`: `ASI_DASH_COMBO_ALS_X` (280), `ASI_DASH_JUMP_COMBO_ALS_ZX` (283), `ASI_DASH_JUMP_COMBO_ASD_XZ` (320), `ASI_DASH_COMBO_ALS_ZX` (325), `ASI_DASH_JUMP_COMBO_ASR_XZ` (327). No new `Always.h` define. 26 funcmap rows `Char Ara CH1d2`; checklist L178 carries `(Ara: CH1d2 ...)`; `JUNE_BUGS.md` row 587; `MODS.md` section "Stage 7 Ara, CH1d2".

| June | `CX2GUAra::` | June | `CX2GUAra::` |
|---|---|---|---|
| `32b90e90` `32bb37e0` `32ba7470` `32b96ee0` `32bb3ab0` | Dash Start / EP, DashEnd StartFuture / FrameMove / EP | `32ba74b0` `32bb3df0` `32b90ec0` `32b90ef0` `32bb3ff0` | DashJump EP, DashJumpLanding EP, PowerLanding StartFuture / Start / EP |
| `32b910b0` `32b99960` | ComboDashZ FrameMove / EP | `32b99df0` `32b910c0` `32b99e10` `32b99e70` | ZpushX Charge Start / FrameMove / EP / End |
| `32b91120` (new) `32bb0260` `32b99e90` `32bb0330` | ZpushX Attack StateStart / FrameMove / EP / End | `32b99fe0` | ComboDashX EP |
| `32b9a280` `32b9a5f0` `32b9a6f0` `32b9a750` `32b9a780` | DashJumpCombo Z / ZZ / ZZZ(FrameMove) / ZZZ(EP) / X EP | | |

**Where June differs from March** (read from `charport_adis.py ara --phase CH1d2`; the rest is March's logic written out whole):
- **Line 3 (classes 34 / 124, `IsInJobLine( 3 )`, CH1d's `_CORE` declaration):** Dash and DashEnd EP: X -> `ASI_DASH_COMBO_ALS_X` (DashEnd only after the 0.3 x max-time test). ComboDashZ gains event 3 (X -> `ASI_DASH_COMBO_ALS_ZX`, tested first) and the Z-push-X charge (`pureX`, event 0) is refused on line 3. DashJumpComboZ gains event 3 (X -> `ASI_DASH_JUMP_COMBO_ALS_ZX`).
- **Class tests** (unit class at `+0x38`: 28 / 118 Hsien / Sakra, 31 / 119 Devil / Yama, 124 Asura): DashJumpComboX is now event 0 Yama Z -> `AYR_XZ`, event 1 Sakra Z -> `ASD_XZ`, event 2 Asura Z -> `ASR_XZ` (March had Yama only); the other event conditions equal March's.
- **DashJump EP:** a new branch for both direction keys held: only a fresh `oneRight` or `oneLeft` against the facing clamps the speed and picks JUMP_UP / JUMP_DOWN (March ran the `pureRight` arm). Everything else equals March.
- **Dash EP / DashEnd EP hyper entry:** `StateChange( GetHypetMpdeStateID() )` with the `IsAwakeImpossible()` test, as CH1a (June's override equals March's berserk / hyper choice).
- **Flag macro:** the dash combos use CH1a's `ELSE_IF_STATE_CHANGE_ON_EX_OR_ANIMATION_END_`; `SKILL_CANCEL_AFTER` is the shared macro.
- **FrameMoves:** ComboDashZ is `CommonFrameMove` only (no `DAMAGE_DATA_NEXT` swap). Z-push-X charge has no 0.495 s effect set; Z-push-X attack keeps an empty 0.001 s timer (no scaling, no effect set): `ASI_DASH_COMBO_ZpushX_ATTACK_StateStart` scales `fPhysic *= 1 + 0.25 x charge` (March x1 .. x3, June x1 .. x1.5; `MODS.md`). June keeps two charge fields as March does (`+0x9634` ZZZX = `m_fZZZXChargeTime`, `+0x9604` dash = `m_fZpushXChargeTime`; corrected by the CH1d2 review).
- **ComboDashX (FP cancel):** the same shape as CH1d's XX-down-XX: Reflection id test (`RNSI_A_AYR_REFLECTION`), `m_bFPCancelProbe` under `_FP`, `StateChangeSpecialAttack` / `CanUseSpecialAttack()` as stand-ins for vslots 0x7fc / 0x7f8 (CH1k).
- **Equal to March:** Dash Start, DashEnd StartFuture (`GetDashSpeed()` already carries `m_fRunJumpSpeedRate`) / FrameMove, DashJumpLanding EP, PowerLanding (`(max - transition) / 6` = `GetMaxAnimationTime() / 6`), ZpushX Charge Start / EP / End, Attack EP / End, DashJumpComboZZ / ZZZ. DashEnd's `(max - transition) x 0.3` = `GetMaxAnimationTime() x 0.3f`.
- **JUNE_BUGS 587:** the dash Z-push-X states play no effect set from code. No branch drifted out of scope.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --with ..._CORE ..._FP --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is `//#define` (by design). cpp +637, `.h` +6.
- One-file `/c /Zs /EHsc` probe of `X2GUAra.cpp` (`census.cl_cmd`, no PCH): exit 0, 0 errors with `_LIVE` off, with `/D ..._LIVE`, and with `_LIVE` + `_FP`; `cl /P`: `ASI_DASH_COMBO_ALS_ZX` 1 line off / 2 on, `ASI_DASH_JUMP_COMBO_ASR_XZ` 1 / 2.
- `junediverge.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: 0 A/B rows.
- Full build skipped: no play-test (`_LIVE` is off), so no `DEPLOY OK` / `exestr` line.

**Nothing to play-test.** Phase tag in every block: `// char-ara CH1d2`. Review: `kickoffs/char-ara/CH1d2-review.md` (Opus 5.5 high, covers CH1c-CH1d2; no `_DEBUG` define exists yet). Next: `/x2-phase char-ara CH1d2 review` on Opus 5.5 high.

### CH1d2 - Review (2026-11-10, covers CH1c-CH1d2)

**Verdict: pass, no code change.** The 57 functions of CH1c (`3c9b38ef`), CH1d (`ddeaf95c`) and CH1d2 (`c6b306df`) were checked against `charport_adis.py ara` listings (StateChange pushes, `fcomp` / `test ah` direction, fall-through vs `jmp`). No transcription error was found. Two doc slips in the CH1d2 note were fixed in place.
- **Dash EP `32bb37e0` / DashEnd EP `32bb3ab0`:** exact. On line 3 X goes to `ASI_DASH_COMBO_ALS_X` 0x118 (`vslot 0x670( 3 )`); DashEnd tests `now < (max - transition) x 0.3` first (= `GetMaxAnimationTime()`, `KTDGXSkinAnim.h:174`). The hyper entry chains black hole, `oneLCtr`, count > 0, `remain <= 0 || +0x8fc3`, quick-slot wait `32070d60` and vslot 0x564, then `StateChange( vslot 0x6b8 )`. The four DASH_END tests, the double-tap / facing dash arms, WALK, the JUMP_DOWN -45 and RIDING_ON order all match June.
- **DashJump EP `32ba74b0`:** exact. The both-keys branch acts on `(oneRight && !isRight) || (oneLeft && isRight)`, clamps `nowSpeed.y` to the jump speed, then `0 < y` picks JUMP_UP; otherwise it does nothing (no fall into the `pureRight` arm). The single-key arms, DOUBLE_JUMP_FRONT, `oneUp && y <= 0` and the down force `2 x fMaxGSpeed` match June.
- **DashJumpCombo Z `32b9a280` / X `32b9a780`:** exact. Z: event 0 ZZ (no class), 1 class 0x76 -> `ASD_ZX`, 2 class 0x1f / 0x77 -> `ALD_ZX`, 3 line 3 -> `ALS_ZX`. X: 0 class 0x77 -> `AYR_XZ`, 1 0x76 -> `ASD_XZ`, 2 0x7c -> `ASR_XZ`. Each fires on `stateChange < now || IsAnimationEnd()`, key window `start <= now < end`.
- **ComboDashZ EP `32b99960`:** exact. Order WAIT, `SKILL_CANCEL_AFTER( +0x8c90 m_fSkillCancelAfter )`, events 3, 0, 1, 2; event 0 is `pureX && !IsInJobLine( 3 )`; event 1 classes 0x1c / 0x76.
- **ComboDashX `32b99fe0` / XX-down-XX `32b99890`:** the same code. WAIT `jmp`s past the skill branch but still runs the FP-cancel test; the skill branch is `+0x9620 && skill && !IsSameOrUpgrade( skill, 0x6a4d = 27213 )`. The probe `+0x9621` is set before the test and cleared on both exits. The window is `+0x9618 <= now < +0x961c` with vslot 0x7f8( true ). The port's stand-ins (CH1k) and `_FP` probe sit where June's are.
- **StateStarts `32b91120` / `32b91000`:** both are `fPhysic *= 1 + charge x 0.5 x 0.5`. **Correction:** they read different fields: ZZZX `+0x9634` (written by `32b98600` / `32bb4630`) and the dash `+0x9604` (`32b99df0` / `32b910c0` / `32bb0260`). June has two members, as March does, so the code is exact and only the CH1d2 note's "single field" sentence was wrong.
- **Z-push-X FrameMoves:** charge `32b910c0` = CommonFrameMove + `charge += dt`, capped at 2.0, no effect set. Attack `32bb0260`: the timer result is dropped, and `charge >= 2` gives super armor. JUNE_BUGS 587 holds; 586 holds (`32b98cd5`: down + X sets flag 1, then flag 0 is set anyway).
- **CH1c / CH1d:** `CommonHyperModeFrameMove` `32bc0140` follows June's order: change at t1; at t2 the awakening set, shock, crash camera `( 30, 0.4 )`, ClearScreen, trace only without the set, buff, three sounds; t3 empty (JUNE_BUGS 584). `RidingHyperModeFrameMove` = `( 0, 0.1, 0.1, true )`. ComboZ `32b97ae0` shows the macro shapes: WAIT is a separate `if`, JUMP_DOWN heads the chain, `SKILL_CANCEL_AFTER` needs the foot on the line and `CanHyperMode` (vslot 0x560), and `_OR_ANIMATION_END_` matches June.
- **`ch1_offsets.tsv`:** the rows these phases rely on agree with the listings. The review adds 7 that CH1d / CH1d2 used without a row: `+0x9604`, `+0x9634`, `+0x9618`, `+0x9620`, `+0x9621`, `+0x6890` and `+0x7eb4`.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --with ..._CORE ..._FP --range 3c9b38ef^..c6b306df`: `files: 4 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is `//#define` (by design). The OTHER row is `X2SkillTree.h` (+0 here) under `SERV_IRUHADEV_JUNE_ENUM_NAMES`. With `_CORE` as the flag: `define: KTDXLIB/Always.h:12207`, `RESULT: PASS`.
- `junediverge.py --check-all`: `allow-listed: 202`, `RESULT: PASS`, no hit in `X2GUAra.*`. `enumscan.py --check X2Lib/X2GUAra.cpp`: 0 A/B rows.
- One-file `/c /Zs /EHsc` probe of `X2GUAra.cpp` (`june2017_re/census.py` `cl_cmd`, no PCH): exit 0, 0 errors with `_LIVE` off, with `/D ..._LIVE`, and with `_LIVE` + `_FP`.
- Full build skipped: no play-test. There is no `_DEBUG` define to retire.

Next: `/x2-phase char-ara CH1e` (Sonnet 5.5 high). Its kickoff was checked and is current.

### CH1e - Done (2026-11-10)

All 13 class-map rows of phase CH1e are written: 6 `inside` (June's body in March's own `CX2GUAra` function under `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`, March's in `#else`) and 7 new under `_CORE` (referenced by nothing until CH1m). Five state names join `ARA_STATE_ID` under `_CORE`: `ASI_COMBO_ALH_XXXZX` (June id 318), `ASI_COMBO_ALH_XXXZX_Landing` (319), `ASI_DASH_JUMP_COMBO_ASD_XZ_LANDING` (321), `ASI_P_ASD_BRUTAL_PIERCING` (331), `_SMALL` (332). No new `Always.h` define. 13 funcmap rows `Char Ara CH1e`; checklist L178 carries `(Ara: CH1e ...)`; `JUNE_BUGS.md` row 588; `MODS.md` section "Stage 7 Ara, CH1e". The other 14 of the 27 states have no function of their own in the class map: START / WIN / LOSE (x2 each, shared engine callbacks) and the four quick-stand states (CH1b's `StandUpRolling*`); CH1m registers them.

| June | `CX2GUAra::` | June | `CX2GUAra::` |
|---|---|---|---|
| `32b9b920` `32b9b950` `32b9bae0` `32b9bcc0` | ZZXX EP, ZZXX landing EP, dash ZZ EP, jump-attack ZZ EP (inside) | `32b9bfe0` `32b9c050` | dash-jump ASD_ZX EP, double-jump ASD Z-back EP (inside) |
| `32ba4870` `32ba4d00` `32ba4d30` | `ASI_COMBO_ALH_XXXZ` / `_XXXZX` / `_XXXZX_Landing` `_EventProcess` (new) | `32ba5050` `32ba5080` | `ASI_DASH_JUMP_COMBO_ASD_XZ` / `_XZ_LANDING` `_EventProcess` (new) |
| `32b9cf00` `32bbac50` | `ASI_P_ASD_BRUTAL_PIERCING_EventProcess` / `_StateStart` (new; `_SMALL` shares both, CH1m registers it to the same) | | |

**Where June differs from March** (read from `charport_adis.py ara --phase CH1e`; the rest is March's logic written out whole):
- **Cancel macros.** June's walk / dash / skill cancels all carry the `IsOnSomethingFuture()` test; March's `WALK_CANCEL_AFTER` and `DASH_CANCEL_AFTER` have none. The phase uses CH1d's `DASH_CANCEL_AFTER_ON_LINE_` and adds `WALK_CANCEL_AFTER_ON_LINE_` (dash ZZ has no foot guard, so it matters there). The skill macro is the shared June-expanded `SKILL_CANCEL_AFTER` / `IF_SKILL_CANCEL_AFTER`.
- **Dash-jump ASD_ZX and double-jump Z-back EP:** the `m_fAttackCancelAfter` test reads the future animation (`m_pXSkinAnimFuture`); March read the now animation. Landing targets equal March's (`ASI_COMBO_ALH_ZZXX_LANDING` / `USI_JUMP_LANDING`).
- **`ASI_COMBO_ALH_XXXZ` EP (329 instructions):** at 0.9 s on the line it enters `ASI_COMBO_ALH_XXXZX_Landing`, then falls through. In the air: the double-key airial dash of the combo states (event time 1, `m_bAirialDashFromCombo`, turn-around) or event 0 (`oneX` and `IsInJobLine( 1 )` -> `ASI_COMBO_ALH_XXXZX`, `OR_ANIMATION_END_` macro). On the line: skill / hyper cancel, walk, back-walk, dash, then `m_fAttackCancelAfterEx` with Z / X -> `GetComboZStateID()` / `GetComboXStateID()` (`ASI_COMBO_Z` / `_X`).
- **Clones of one another** (listings equal modulo branch addresses): `ASI_COMBO_ALH_XXXZX_Landing` EP and `_ASD_XZ_LANDING` EP = ZZXX landing EP; `XXXZX` EP = ZZXX EP; `ASD_XZ` EP = ZZXX EP with `ASI_DASH_JUMP_COMBO_ASD_XZ_LANDING`.
- **Brutal piercing:** EP at animation end -> `USI_JUMP_DOWN` and `y -= 45` (`LINE_RADIUS` x 1.5, as the quicken-guard EP) off the line, else `USI_WAIT`. StateStart plays `EffectSet_CSI_CIP_IRON_WILL` then `CommonStateStart()`, as `ASI_P_ASD_QUICKEN_GUARD_StateStart`.
- **JUNE_BUGS 588:** June registers `ASI_COMBO_ALH_XXXZX_Landing`; the script's table is `ASI_COMBO_ALH_XXXZX_LANDING`, so the state has no script table. Registration keeps June's spelling (CH1m).
- **Shape of the `_CORE` functions.** CH1a / CH1d's three macros exist only under `_LIVE`; a `_CORE` block (`_CORE` and not `_LIVE`) repeats them so the new functions compile while `_LIVE` is off. `XXXZ` EP writes `m_bAirialDashFromCombo` / `m_bAirialDashTurnAround` under an inner `#ifdef _LIVE` (the members exist under `_LIVE` only). No branch drifted out of scope.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --with ..._CORE ..._FP --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is `//#define` (by design). With `_CORE` as the flag: `define: KTDXLIB/Always.h:12207`, `RESULT: PASS`. cpp +245, `.h` +14, 0 deletions.
- One-file `/c /Zs /EHsc` probe of `X2GUAra.cpp` (`census.cl_cmd`, no PCH): exit 0, 0 errors with `_LIVE` off, with `/D ..._LIVE`, and with `_LIVE` + `_FP`; `cl /P`: `ASI_COMBO_ALH_XXXZ_EventProcess` and `ASI_P_ASD_BRUTAL_PIERCING_StateStart` 1 each off and on, `m_fAttackCancelAfter <= m_pXSkinAnimFuture` 0 off / 2 on. (First probe found the two `_LIVE`-only members; fixed as above.)
- `junediverge.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: 0 A/B rows.
- Full build skipped: no play-test (`_LIVE` is off), so no `DEPLOY OK` / `exestr` line.

**Nothing to play-test.** Phase tag in every block: `// char-ara CH1e`. No `_DEBUG` define exists; no review of its own (the CH1g review covers CH1e-CH1g). Next: `/x2-phase char-ara CH1f` on Sonnet 5.5 high.

### CH1f - Done (2026-11-10)

All 20 class-map rows of phase CH1f are written: 14 `inside` (June's body in March's own `CX2GUAra` function under `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`, March's in `#else`) and 6 new under `_CORE` (referenced by nothing until CH1m). Two state names join `ARA_STATE_ID` under `_CORE`: `ASI_COMBO_ALD_XXdownXXX` (June id 323) and `ASI_COMBO_ALD_XXdownXXXX` (324; `ASI_COMBO_ALD_XXdownXX` 322 came with CH1d). No new `Always.h` define. 20 funcmap rows `Char Ara CH1f`; checklist L178 carries `(Ara: CH1f ...)`; `JUNE_BUGS.md` row 589; `MODS.md` section "Stage 7 Ara, CH1f". The other 10 of the 19 states (ALD / AYR START, WIN, WIN2, LOSE, LOSE2) have no function of their own: shared engine callbacks that CH1m registers.

| June | `CX2GUAra::` | June | `CX2GUAra::` |
|---|---|---|---|
| `32bc25a0` `32b9cfa0` | `ASI_COMBO_ALD_ZZXX` FrameMove / EP (inside) | `32bc2650` `32b9d0e0` | `ASI_COMBO_ALD_ZZZFrontZ` FrameMove / EP (inside) |
| `32bc26a0` `32b9d300` | `ASI_DASH_JUMP_COMBO_ALD_ZX` FrameMove / EP (inside) | `32bc26f0` `32b9d430` | `ASI_DASH_JUMP_COMBO_ALD_ZXX` FrameMove / EP (inside) |
| `32b93070` `32bc27a0` `32b9d470` | `ASI_DASH_JUMP_COMBO_AYR_XZ` StateStartFuture / FrameMove / EP (inside) | `32bc2850` `32b9d4b0` | `ASI_DASH_COMBO_AYR_ZZ` FrameMove / EP (inside) |
| `32bbfde0` | `CreateEnergeBullet` (inside) | `32ba5640` `32ba5740` `32ba5840` | `ASI_COMBO_ALD_XXdownXX` / `XXX` / `XXXX` `_EventProcess` (new) |
| `32bc0ac0` `32bc0b10` `32bc0b60` | `ASI_COMBO_ALD_XXdownXX` / `XXX` / `XXXX` `_FrameMove` (new) | | |

**Where June differs from March** (read from `charport_adis.py ara --phase CH1f`; the rest is March's logic written out whole):
- **Cancel macros.** The walk / dash cancels carry the `IsOnSomethingFuture()` test (CH1d / CH1e's `WALK_CANCEL_AFTER_ON_LINE_` / `DASH_CANCEL_AFTER_ON_LINE_`; the skill macro is the shared June one). In the airborne states (ZZXX, dash-jump ZX / ZXX, Yama XZ) this follows `if ( IsOnSomethingFuture() ) ... landing`, so the cancels can never run: the compiler folded them away and the listings keep only the loads of the two cancel times (**JUNE_BUGS 589**). In the ground states (ZZZFront-Z, Yama dash ZZ) the test is live.
- **Dash-jump ZX EP:** the event macro is the animation-end form (`ELSE_IF_STATE_CHANGE_ON_EX_OR_ANIMATION_END_`, CH1a): the flag also fires when the future animation ends; class test unchanged (Little Devil / Yama Raja -> ZXX).
- **FrameMoves** are March's `EventTimerOneshot` branch (`X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` is defined, probe-checked) with `CreateEnergeBullet( rot, right )` written out: ZZXX -37 (0.24 / 0.39 / 0.67), ZZZFront-Z 0 (0.34), ZX -45 (0.27), ZXX and Yama XZ -45 (0.24 / 0.39 / 0.67), Yama dash ZZ right hand at 0.186 / 0.326 and left hand at 0.263. March's non-oneshot `EventCheck( 0.349f )` quirk is not in June.
- **Clones** (listings equal modulo branch addresses): Yama XZ EP = dash-jump ZXX EP; Yama dash ZZ EP = ZZZFront-Z EP; Yama XZ FrameMove = ZXX FrameMove.
- **New XXdown states** (Little Devil X-X-down-X-X chain, `IsInJobLine( 2 )`): XX EP -> XXX at X, XXX EP -> XXXX at X, XXXX EP ends; each also goes to `USI_WAIT` at animation end. FrameMoves fire one energy bullet at 0.36 / 0.13 / 0.11 s.
- **`CreateEnergeBullet`:** equals March (MP 4, `EffectSet_SA_AYR_ORB_KI_FX_DAMAGE`, size / damage / range / force-down passives, not-enough-MP effect) except the force-down passive is assigned to the rate (1 x value in March); `CreateInstance`'s 23 June arguments are March's defaults. The `+0x96a0..0x96ac` fields are March's `m_fIncreaseEnergeBullet*`; CH1h's offset map owns their placement.
- **Not reached here:** no force-power or Specter line in these 20 functions, so no `_FP` / `_SPECTER` block.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --with ..._CORE ..._FP --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is `//#define` (by design). cpp +289, `.h` +8, 0 deletions.
- One-file `/c /Zs /EHsc` probe of `X2GUAra.cpp` (`census.cl_cmd`, no PCH): exit 0, 0 errors with `_LIVE` off, with `/D ..._LIVE`, and with `_LIVE` + `_FP`; `cl /P`: `fEnergyBulletForcePower = m_fIncrease` 0 off / 1 on, `ASI_COMBO_ALD_XXdownXXX_EventProcess` and `ASI_COMBO_ALD_XXdownXXXX_FrameMove` 1 each in both.
- `junediverge.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: 0 A/B rows.
- Full build skipped: no play-test (`_LIVE` is off), so no `DEPLOY OK` / `exestr` line.

**Nothing to play-test.** Phase tag in every block: `// char-ara CH1f`. No `_DEBUG` define exists; no review of its own (the CH1g review covers CH1e-CH1g). Next: `/x2-phase char-ara CH1g` on Sonnet 5.5 high.

### CH1g - Done (2026-11-10)

All 28 class-map rows of phase CH1g are written: every one is `new` (March has no Little Specter / Asura line), so they sit under `SERV_IRUHADEV_JUNE_CHAR_ARA_CORE`, compiled in every build and referenced by nothing until CH1m. Six state names join `ARA_STATE_ID` under `_CORE` (June script ids 281 / 285 / 286 / 287 / 328 / 329, listed in `X2GUAra.h`). No new `_LIVE` / `_FP` / `_SPECTER` define was needed. 28 funcmap rows `Char Ara CH1g`; checklist L178 and L781 carry `(Ara: CH1g ...)`; `JUNE_BUGS.md` rows 590 / 591; `MODS.md` section "Stage 7 Ara, CH1g"; `ch1_offsets.tsv` rows `0x9734` / `0x9738` / `0x9744`. The other 10 of the 23 states (ALS / AA START, WIN, WIN2, LOSE, LOSE2) have no function of their own: shared engine callbacks that CH1m registers.

| June | `CX2GUAra::` (all `new`) | June | `CX2GUAra::` (all `new`) |
|---|---|---|---|
| `32b93fa0` `32b94090` | `ASI_DASH_COMBO_ALS_XX` / `ASI_DASH_JUMP_COMBO_AA_ZXX` `_StateEndFuture` | `32b93fe0` | `ASI_DASH_JUMP_COMBO_ALS_ZX_StateStartFuture` |
| `32b94070` | `ASI_COMBO_AA_XXDownXZStart_StateStart` | `32b9edd0` `32b9ef70` `32b9f290` | `ASI_DASH_COMBO_ALS_X` / `_ALS_XX` / `ASI_COMBO_ALS_XXXLoopZ` `_EventProcess` |
| `32b9f5b0` `32b9f9b0` | `ASI_DASH_JUMP_COMBO_ALS_ZX` / `ASI_COMBO_AA_XXDownXZStart` `_EventProcess` | `32b9fce0` `32ba0040` `32ba0360` | `ASI_COMBO_AA_XXDownXZLoop` / `LoopX` / `ASI_DASH_JUMP_COMBO_AA_ZXX` `_EventProcess` |
| `32ba4ec0` `32ba5210` | `ASI_DASH_COMBO_ALS_ZX` / `ASI_COMBO_ASR_ZZZX` `_EventProcess` | `32ba53a0` `32ba54d0` `32ba5600` | `ASI_DASH_JUMP_COMBO_ASR_XZ` / `XZZ` / `XZZZ` `_EventProcess` |
| `32bbbc70` `32bbbe60` `32bbc050` | `ASI_DASH_COMBO_ALS_XX` / `ASI_COMBO_ALS_XXXLoopZ` / `ASI_DASH_JUMP_COMBO_ALS_ZX` `_FrameMove` | `32bbc240` `32bbc600` `32bbc980` | `ASI_COMBO_AA_XXDownXZStart` / `Loop` / `LoopX` `_FrameMove` |
| `32bbcbd0` `32bc08d0` | `ASI_DASH_JUMP_COMBO_AA_ZXX` / `ASI_COMBO_ASR_ZZZX` `_FrameMove` | `32bc66d0` `32bc6720` | `ASI_DASH_JUMP_COMBO_ASR_XZ` / `_XZZ` `_FrameMove` (`_XZZ` serves XZZZ too) |
| `32bc0bb0` | `CreateDashJumpXZBullet` | | |

**Where June differs from March / what the shapes are** (read from `charport_adis.py ara --phase CH1g`):
- **Clones** (listings equal modulo branch addresses): ALS_XXXLoopZ EP = ALS_XX EP; AA_ZXX EP = AA Loop-X EP; ASR_ZZZX EP = ALS_ZX EP; ASR_XZZ EP = ASR_XZ EP with XZZZ as target; AA_ZXX `EndFuture` = ALS_XX `EndFuture` (turn round: `bIsRight` flipped, `dirDegree.y += 180`). The five 0.01 / 0.26 / 0.285 effect FrameMoves share one body.
- **Ground EPs** (ALS_XX, XXXLoopZ, ALS_ZX dash-jump, the three AA ones) use CH1e's cancel set: `SKILL_CANCEL_AFTER`, walk / back-walk / dash with the on-line test, then the `m_fAttackCancelAfterEx` Z / X cancels (`GetComboZStateID()` / `GetComboXStateID()`). ALS_ZX dash and ASR_ZZZX have only the dash cancel then the skill cancel. ALS_X carries the FP-cancel lines as CH1d (stand-ins for vslot `0x7f8` / `0x7fc`, probe byte under `_FP`).
- **Asura line** (the key tests read the class, `UC_ARA_ASURA` = 0x7c): ALS_ZX + X -> AA_ZXX; AA start -> Loop at animation end; the Loop repeats while Z was pressed and `+0x9738` < 5, else ends in LoopX (X as Asura at flag 1 does the same); ASR_XZ / XZZ + Z -> the next XZ state. **Airborne ASR states:** the walk / dash cancels are dead behind the on-line test (**JUNE_BUGS 590**, as row 589).
- **Effect FrameMoves:** `EventTimerOneshot( t )` -> `PlayEffectSet( name, this, NULL, IsHyperState(), rate )` where the rate is `GetPowerRate()` times `+0x9744` while that float is above 0 (the same test `IsFullHyperMode` `32ba6cc0` reads); the handle goes to `m_vecEffectSetToDeleteOnDamageReact` (June `+0x3eb0`; corrected by the CH1g review). Names and times: ALS_XX 0.26 `EffectSet_ALS_Dash_Combo_XX_Damage`; XXXLoopZ and dash-jump ALS_ZX 0.01 `EffectSet_ALS_ComboXXXloopZ`; AA start 0.2 `EffectSet_AA_ComboXXdownXZ` and 0.4 `..._BlackHole` (handle kept in `+0x9734`); Loop 0.1 / 0.3 `..._loop_Attack1` / `_Attack2`; LoopX 0.33 `EffectSet_AA_ComboXXdownXZX_Attack`, then at 0.15 stops the black hole handle; AA_ZXX 0.01 `EffectSet_AA_Dash_Jump_Combo_ZXX_Damage`; ASR_ZZZX 0.285 `EffectSet_ALS_Dash_Combo_XX_Damage`. Two states play another state's set (**JUNE_BUGS 591**, kept).
- **`CreateDashJumpXZBullet`** is `CreateEnergeBullet`'s shape (MP 4, finger bone, same not-enough-MP effect) with `DamageEffect_ASR_Dash_Jump_XZ_projectile`, power rate `GetPowerRate()` only and no passive scaling; its callers fire it at -135 from the right hand (0.317 s in XZ, 0.1 s in XZZ / XZZZ).
- **Landing prediction hoisted.** The Little Specter dash-jump ZX start calls `32021390`, which Aisha's port had put in `CX2GUArme_VioletMage` with a "moves to `CX2GUUser`" note. It now lives in `CX2GUUser::LandingPrediction` under the new `SERV_IRUHADEV_JUNE_USER_LANDING_PREDICTION` (same body); Arme's declaration and definition are wrapped in `#ifndef` of it, so Arme calls the base (flag off = Arme's own copy). The flag needs `SERV_IRUHADEV_JUNE_LINEMAP_CANDOWN_SIGNATURE` (guard in `Always.h`).
- **Three `_CORE` members** (`X2GUAra.h`, ctor in the initializer list): `m_hComboXXdownXZBlackHole` (`+0x9734`, ctor `INVALID_EFFECTSET_HANDLE`), `m_iComboXXdownXZLoopCount` (`+0x9738`, 0), `m_fBuffHyperModeDamageRate` (`+0x9744`, 0). June's ctor values and the offset placement are CH1h's (`ctor 32bc6bf0`); `+0x9744` is written by CH1q (buff hyper count), so every effect keeps March's plain rate until then.
- A constant `_CONST_ARA_CH1G_::DASH_JUMP_XZ_BULLET_MP_CONSUMPTION` (4) holds the bullet MP.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_CORE --with ..._LIVE ..._FP --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. cpp +581, `.h` +39, 0 deletions. `flagcheck SERV_IRUHADEV_JUNE_USER_LANDING_PREDICTION --worktree` over `Always.h`, `X2GUUser.*`, `X2GUArme_VioletMage.*`: `4 PASS, 1 DEFINE, 0 FAIL, 0 ERROR`, `RESULT: PASS` (+12 / +50 / +3 / +2 / +2, 0 deletions).
- One-file `/c /Zs /EHsc` probes (`census.cl_cmd`, no PCH, `X2GUAra.cpp`): exit 0, 0 errors with `_LIVE` off, with `/D ..._LIVE`, and with `_LIVE` + `_FP`; `X2GUUser.cpp` and `X2GUArme_VioletMage.cpp`: exit 0, 0 errors. `cl /P` of `X2GUAra.cpp` off and with `_LIVE`: every new function and `m_fBuffHyperModeDamageRate` appear the same number of times in both.
- `junediverge.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `allow-listed: 0`, `RESULT: PASS` (with `X2GUUser.*` added: 7 allow-listed, all older rows, `RESULT: PASS`). `enumscan.py --check X2Lib/X2GUAra.cpp`: 0 A/B rows.
- Full build skipped: no play-test (`_LIVE` is off), so no `DEPLOY OK` / `exestr` line.

**Nothing to play-test.** Phase tag in every block: `// char-ara CH1g`. No `_DEBUG` define exists. Review: `kickoffs/char-ara/CH1g-review.md` (Opus 5.5 high, covers CH1e-CH1g). Next: `/x2-phase char-ara CH1g review`.

### CH1g - Review (2026-11-10, covers CH1e-CH1g)

**Verdict: pass after one fix.** The 61 functions of CH1e (`91815e49`), CH1f (`7a489014`) and CH1g (`269fa62a`) were checked against `charport_adis.py ara` listings (StateChange pushes, `fcomp` / `test ah` direction, `if` vs `else if` from the `jmp` after each change). One transcription error was found and fixed in place (`bytepatch`, `X2GUAra.cpp` 9 lines, inside the CH1g `_CORE` block).
- **Fix: the effect-set handle list.** All nine `PlayEffectSet` pushes of the CH1g FrameMoves (`32bbbc70`, `32bbbe60`, `32bbc050`, `32bbc240` 0.2, `32bbc600` x2, `32bbc980` 0.33, `32bbcbd0`, `32bc08d0`) go to `lea ecx, [esi + 0x3eb0]` = `m_vecEffectSetToDeleteOnDamageReact` (funcmap `32007570` C9h-4 / `3228a270`); the port had `m_vecEffectSetToDeleteOnStateEnd` (`+0x3ec0`, which CH1d's `32bb4630` @32bb47f3 rightly uses). In June these effects keep playing past the state end and stop only when Ara is hit. `ch1_offsets.tsv` now names `0x3eb0` / `0x3ec0`, so the annotator labels them; the 8 funcmap rows and the CH1g note were corrected; the CH1h kickoff carries the warning. No other CH1a-CH1g listing pushes to either list.
- **EventProcess shapes (exact):** ALS_X `32b9edd0`: WAIT is `else if` with event 0 (`oneX && IsInJobLine( 3 )` -> `ALS_XX`), skill branch `m_bFPCancel && skill && !IsSameOrUpgrade( skill, 27213 RNSI_A_AYR_REFLECTION )`, then the FP-cancel test `x <= now < y` with vslot 0x7f8( true ); the final `mov word [+0x9620], bx` sets `m_bFPCancel` and clears the probe `+0x9621` together, as the port does. ALS_ZX `32b9f5b0`: X as class 0x7c -> `AA_ZXX`. Loop `32b9fce0`: at animation end flag 0 and `+0x9738` < 5 -> `++count`, Loop, else LoopX; event 1 = X as Asura -> LoopX; Z sets flag 0; then skill / hyper / walk / back-walk / dash only, no Z / X attack cancel. ASR XZ / XZZ / XZZZ `32ba53a0` / `32ba54d0` / `32ba5600`: the walk / dash cancel times are only loaded (JUNE_BUGS 590 holds). ALS_XX and XXXLoopZ: WAIT then a separate `if` (no `jmp` after the change), as the port.
- **Clones confirmed** by normalised listing diff: ALS_XX = XXXLoopZ EP, LoopX = AA_ZXX EP, ALS_ZX dash = ASR_ZZZX EP (0 lines differ); ASR_XZ vs XZZ differ only in the target (0x148 / 0x149).
- **FrameMoves:** the rate is `GetPowerRate()` x `+0x9744` only when `0 < +0x9744` (`fldz; fcomp; test ah,5; jp`); the 4th `PlayEffectSet` argument is the byte at `[state data + 0x10]` = `IsHyperState()`; names and times match the string table (0.26 / 0.01 / 0.01 / 0.2+0.4 / 0.1+0.3 / 0.33+0.15 / 0.01 / 0.285). The black hole set (0.4, plain `GetPowerRate()`, `GetEffectSet()` NULL test) is stored at `32bbc590` into `+0x9734` and stopped at `32bbcb91` by `321dafb0( Handle&, false )`; June has no INVALID test before the stop, the port's guard is harmless (March's `StopEffectSet` ignores an invalid handle). `32bc6720` serves XZZ and XZZZ; the 3-bullet FrameMove right after it (`32bc6770`, 0.25 s at -100 / -135 / -170) is dead in June (class map: no reference) and stays unported.
- **`CreateDashJumpXZBullet` `32bc0bb0`:** MP 4 through vslot 0xe4, R / L `Finger2`, power rate `GetPowerRate()` only, `vRot.z = offset` for both angle vectors, `landPosition.y` of the now frame; the 23 `CreateInstance` pushes are March's defaults (`false, -1, 1, 1, -1, -1.0, 0, -1.0, false, false`); the not-enough-MP tail equals `32bbfde0`'s (register allocation only).
- **`LandingPrediction` hoist:** `CX2GUUser::LandingPrediction` body equals Arme's old body (diff: only the JUNE_BUGS row number in one comment); Arme's declaration and definition sit under `#ifndef SERV_IRUHADEV_JUNE_USER_LANDING_PREDICTION`; Arme's `AVSI_ABM_DASH_JUMP_ZX_StartFuture` calls it unchanged (resolves to the base). Ara's `32b93fe0` matches (future position, 1000, -600, 5 steps, write back, `CommonStateStartFuture`).
- **CH1e / CH1f:** the `_CORE`-without-`_LIVE` macro block repeats the four `_LIVE` macros verbatim; XXXZ EP `32ba4870` matches (0.9 s landing as a separate `if`, the airial-dash turn-round from the now frame's `bIsRight`, event 0 `oneX && IsInJobLine( 1 )`); `CreateEnergeBullet` `32bbfde0` assigns the force-down passive (`fld +0x96ac; fstp`), range then `SetInheritForceDownRate` (`+0xacd = 1`) then size, as ported. JUNE_BUGS 588-591 hold.
- **`ch1_offsets.tsv`:** `0x9734` / `0x9738` / `0x9744` agree with the listings; the review adds `0x3eb0` / `0x3ec0`.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_CORE --with ..._LIVE ..._FP --range 91815e49^..269fa62a --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `files: 4 PASS, 2 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. (Without `--files` the tool stops on an unrelated `X2GUUser.cpp:27130` CHUNG/EVE `#if` it cannot evaluate.) The fix, `--worktree --files X2Lib/X2GUAra.cpp`: `1 PASS`, `RESULT: PASS`; its 9 deleted lines are the 9 replaced pushes, all inside CH1g's own `_CORE` code.
- `flagcheck SERV_IRUHADEV_JUNE_USER_LANDING_PREDICTION --range 269fa62a^..269fa62a`: `files: 4 PASS, 0 PASS~, 1 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`, `0 deleted line(s)`, `RESULT: PASS` (the OTHER rows are `X2GUAra.*`, the `_CORE` caller).
- `junediverge.py --check-all`: `allow-listed: 202`, `RESULT: PASS`, no hit in `X2GUAra.*`, `X2GUUser.*`, `X2GUArme_VioletMage.*`; after the fix `--check X2Lib/X2GUAra.cpp`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: 0 A/B rows.
- One-file `/c /Zs /EHsc` probes (`census.cl_cmd`, no PCH): `X2GUAra.cpp` exit 0, 0 errors with `_LIVE` off, with `/D ..._LIVE`, and with `_LIVE` + `_FP` (re-run after the fix); `X2GUUser.cpp` and `X2GUArme_VioletMage.cpp` exit 0, 0 errors.
- Full build skipped: no play-test. There is no `_DEBUG` define to retire.

Next: `/x2-phase char-ara CH1h` (Sonnet 5.5 high). Its kickoff was checked and amended (the three CH1g members, the handle-list offsets).

### CH1h - Done (2026-11-11)

**36 class-map rows accounted for: 16 written, 19 = March (no code), 1 routed.** Written: `new` under `SERV_IRUHADEV_JUNE_CHAR_ARA_CORE` (compiled, referenced by nothing): `IsBuffHyperModeActive` `32bd5e60`, `MarkSkillFPGainHit` `32bab160`, `MarkSkillFPGainHitByDamage` `32bab1d0` (user vslot `0x2fc`, CH1o wires it); `inside` under `_LIVE` (still `//#define`): `Init` `32bc7e90`, `ReInit` `32bc8030`, `InitComponent` `32bb0da0`, `DamageReact` `32bafa30`, `SetSpecificValueByBuffTempletID` `32ba6d00`, `UnSetSpecificValueByBuffTempletID` `32b90d20`, `ApplyRenderParam` `32b96a30`, `LoadPromotionItemID` `32bab890`, `InitPromotionEquipList` `32baec10`, `SetShowPromotionEquips` `32badd50`, `SetShowOriginalViewList` `32bae120`, `NotifyDGManagerChanged` `32ba8180`, and the constructor `32bc6bf0`. **= March:** `InitEffect`, `CreateMinorParticleArray`, `CreateMajorParticleArray`, `AppendMajorParticleToDeviceList` + `32b96b10` (March's `AppendToDeviceList`), `NotifyShowObjectChanged`, `OnFrameMove`, `Win`, `Draw`, `Lose`, `COMMON_BUFF_FrameMove`, `ApplyRenderParamOnEmptyRenderParamByBuff` (same colours, `RT_CARTOON_COLOR_EDGE` 2 / `_BLACK_EDGE` 1), `InitEnchantWeaponEffectUnitSpecific`, `ReplaceWeapon`, `DeleteMinorParticle`, `~CX2GUAra` `32bc79f0` (+ scalar deleting `32bd8680`), `GetComboZ` / `XStateID`. 36 funcmap rows `Char Ara CH1h`; `ch1_offsets.tsv` +140 rows (28 named, the rest `unnamed_*` ctor values for the phase that reads them); checklist L184 / L237 / L293 carry `(Ara: CH1h ...)`.

**Not written: `AttackResultByType` `32bceca0` (routed to new phase CH1h2, kickoff written).** The function is June's skill-passive hit handler (FP conversion `0x6b18` / `0x6b19`, Gain Force intake `0x6a40` / `0x6a54`, the berserk hit roll `32bc9a60`, Specter of Thunder, Get Blood `0x6992`, Raid `0x6a52`, Mana Drain `0x6a50` with a draw-MP-loss text call that is a divergence row). It needs ~15 members whose names and ctor values come from its own use, three `SKILL_ABILITY_TYPE` names, DamageData fields `+0x450 / +0x470` and CH1q / skill-phase callees, so it was not transcribed half-way: this is a deviation from "no partial June updates" the user may want to overrule. March's function (base call + Gain Force intake) is untouched and `_LIVE` stays off, so nothing is lost until CH1.

**Where June differs from March** (`charport_adis.py ara --phase CH1h`):
- **`DamageReact`:** the enchant-duration switch takes 69 EDTs (March 38; +31 Quadra / Tripple / double-double, names all in C++); the damage state change is skipped when `m_bIgnoreAttackSuccess` (+0x628).
- **`Init` / `ReInit`:** `SetHyperModeCountByBuff( 0 )`; `_FP`: no `SetBasicForcePower( 2 )`, max force power the constant 10 (`_CONST_ARA_CH1H_::MAX_FORCE_POWER`), ReInit's PvP test is game type 1 only (PvE falls into the dungeon branch), the dungeon floor / PvP default are the INIT_COMPONENT bytes; ReInit also sets the Protection Fortitude buff's time finalizer to the cool time left (7ai args `( BFI, 1, NULL, 0, false )`) and clears the hit count, `m_fNecromancyRate` and the FP-gain records; Init clears the FP-gain records.
- **`InitComponent`:** PvP-or-PvE (`GetIsNowPVPCorrectionGame`, June inlines type 1 / 4) starts at the PvP byte, training 2, tutorial 3 (`_FP`); `_FP` reads the 7 June-only keys in June's order (count default 9, interval 0.2, socket-rate list, MP conversion, gain list x3, dungeon / PvP start). Ara's InitComponent has no alpha-tag (7c) sites.
- **Buff cases:** `BTI_BUFF_NECROMANCY` 423 / `BTI_BUFF_SPECTEROFTHUNDER` 424 hold the skills' effective rates (-1 when off); the hyper mode buff (BTI 5 and the transcendental 822) applies / finishes `BFI` / `BTI_ARA_FIRST` 660, `_SECOND` 661, `_FULL` 156 / 157 by the buff hyper count (`GetHyperModeCountByBuff`, June +0x6844). Enum names registered under `JUNE_ENUM_NAMES`: `BTI_BUFF_NECROMANCY`, `BTI_BUFF_SPECTEROFTHUNDER`, `BFI_ARA_FIRST_HYPER_MODE`, `BFI_ARA_SECOND_HYPER_MODE`, `RNSI_SA_ASR_NECROMANCY`, `RNSI_SA_ALS_SPECTER_OF_THUNDER`.
- **Promotion / tail equips:** `LoadPromotionItemID` knows 7 classes and reads `TAIL_TYPE` into a new vector; `InitPromotionEquipList` drops March's `EP_AC_LEG` tail pick, builds the tail list and keeps the last tail active (empty guard = stand-in, **JUNE_BUGS 592**); `SetShowPromotionEquips` picks the tail by the buff hyper count and shows only the weapon for counts 1 / 2 (**JUNE_BUGS 593**, unsigned index); `SetShowOriginalViewList` hides / shows the original view list by the same count (the riding byte +0x93da is a stand-in 0, Stage 17); `NotifyDGManagerChanged` also adds / removes the tail list.
- **`ApplyRenderParam`:** custom render states 1 / 2 (`CRS_SPECTOR_OF_THUNDER`, `CRS_VERITABLE_PANDEMONIUM`) are read from Ara-held `m_iCustomRenderState` (stand-in for CX2GameUnit +0x8e64; the writers are CH2d / CH2d2) with their own alphas; the default branch sets colour alpha 1 for a non-alpha object, then `CX2GameUnit::ApplyRenderParam` (stand-in for `31fd4460`, as Rena's CH1h).
- **Correction to CH1g:** the constructor stores -1.0 in `m_fBuffHyperModeDamageRate` (+0x9744), not 0 (found by simulating the constructor's x87 stack; both mean "off").
- **Constructor:** the 20 CH1h / CH1g members get June's values in the initializer list; every other June store is an `unnamed_*` row (offset, size, value) in `ch1_offsets.tsv` for the phase that declares that member (readers: CH1q FP, CH1q2, CH2-CH4).

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_CORE --with ..._LIVE ..._FP JUNE_ENUM_NAMES --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `.cpp` PASS +613 -1, `.h` PASS +41; `files: 2 PASS, 0 FAIL, 0 ERROR`; `1 deleted line(s)` (the CH1g constructor line corrected to -1.0), `0 non-ASCII inserted`, `0 EOL-mismatched`, `0 #else/#endif without the flag token`, `0 encoding change(s)`; `RESULT: PASS`. `flagcheck SERV_IRUHADEV_JUNE_ENUM_NAMES --worktree --files X2Lib/X2BuffFactor.h X2Lib/X2SkillTree.h`: `2 PASS~`, `RESULT: PASS`. `Always.h`: +9 (a guard only: `_LIVE` needs `BUFF_BEHAVIOR_PARITY` and `CHUNG_CORE`; no new flag).
- One-file `/c /Zs /EHsc` probes (`census.cl_cmd`, no PCH, `X2GUAra.cpp`): exit 0, 0 errors with `_LIVE` off, with `-DSERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`, and with `_LIVE` + `_FP` (first `_LIVE` run caught `GetSkillTemplet`'s one-argument March form: fixed). `cl /P` off / `_LIVE` / `_LIVE` + `_FP`: `m_vecPromotionEquipTailList` 1 / 39 / 39, `CRS_SPECTOR_OF_THUNDER == m_iCustomRenderState` 0 / 1 / 1, `m_ucForcePowerCountAtPvpStart` 2 / 2 / 5, `MAX_FORCE_POWER )` 0 / 0 / 2, `EDT_ENCHANT_QUADRA_BLAZE_MASTER` 1 / 2 / 2.
- `junediverge.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2BuffFactor.h X2Lib/X2SkillTree.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: 0 A/B rows.
- Full build skipped: no play-test (`_LIVE` is off), so no `DEPLOY OK` / `exestr` line. No `_DEBUG` define exists; every new block carries `// char-ara CH1h`.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1h2` (Sonnet 5.5 high), then CH1i. The CH1k review covers CH1h, CH1h2, CH1i, CH1j and CH1k.

### CH1h2 - Done (2026-11-11)

**`AttackResultByType` `32bceca0` written whole** (X2GUAra.cpp, inside March's function: June's body under `_LIVE`, March's in `#else`). Blocks in June's order: base; `DTT_DECENT_DRAGON` (68) + `HaveSkillBuff( RNSI_P_ASD_VALOR_DEVOTION )` -> `UpForcePower( SA_FP_UP_ABS )` and Open Mind `SetSkillBuffFactors`; Gain Force 27200 / 27220 hit count; `ProcessFullHyperHitReward` (`_FP`); Specter of Thunder (`_SPECTER`); Get Blood / Raid / Mana Drain (`_SKILL`); `UpForcePowerBySkillHit` (`_FP`). funcmap row `32bceca0 PORTED`.

**New flags** (`Always.h`, both `//#define`, each `#error`s without `_LIVE`): `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (also needs `JUNE_ENCHANT_PARITY` for `DrawMpLossText`) and `SERV_IRUHADEV_JUNE_CHAR_ARA_SPECTER`. Guard only: `_LIVE` needs `JUNE_CHAR_EVE_PASSIVE` (`HaveSkillBuff`, `SetSkillBuffFactors`); both flags are defined at that line (probes below).

**Enum names registered** under `JUNE_ENUM_NAMES`: `DTT_DECENT_DRAGON` 68 (`X2DamageManager.h`); `RNSI_SA_ASR_GET_BLOOD` 27026, `RNSI_A_ALS_MANA_DRAIN` 27216, `RNSI_A_ASR_RAID` 27218, `RNSI_A_ARA_GAIN_FORCE` 27200, `_UPGRADE` 27220, `RNSI_P_ASD_VALOR_DEVOTION` 27416, `RNSI_P_AYR_OPEN_MIND` 27417 (`X2SkillTree.h`). Values checked against `Enum.src.lua`. `SA_FP_UP_ABS` 191 (the kickoff's 0xbf), `SA_MP_INTAKE_ABS` 44, `SA_MP_RETAKE_ABS` 66 and `MI_CHECK_AND_KILL_2` 16 were already in C++.

**Members** (`_CORE`, ctor values from `32bc6bf0`; `ch1_offsets.tsv` rows renamed): `m_fSpecterOfThunderCoolTime` +0x979c 0.0, `m_fSpecterOfThunderPowerRate` +0x97a0 1.0, `m_bRaidHit` +0x97bc, `m_hManaDrainEffect` +0x97c8 / `m_fManaDrainRetakeMp` +0x97cc -1.0 / `m_usManaDrainHitCount` +0x97d0, `m_iGetBloodHitUnitCount` +0x97d4, `m_hGetBloodEffect` +0x97d8. **+0x96e8 / +0x96ec are March's own `m_hGainForceIntakeEffect` / `m_iGainForceDamageTimeNow`** (same ctor values), so no new member. New helper `IsNowStateSameOrUpgradeSkill` (June `31edf580` on the running state's skill, null skill tree = false; three call sites).

**Decisions to know**
- **The FP gain is not gated on `_FP`** (kickoff said "under `_FP`"): June's inline is exactly March's `UpForcePower` (sum, overflow x `m_iGainMpByFpConversion` into `UpNowMp`, `SetForcePower`), already defined above, so it is called as is; CH1q's `SetForcePower` rework then flows through it. `_FP` guards only `ProcessFullHyperHitReward` / `UpForcePowerBySkillHit`, declared in `X2GUAra.h` with the body in CH1q.
- **`IsSpecterState()` is declared only** (`_SPECTER`, body in CH1q2): June reads `StateParam` byte +0x95; the field, its `operator=` / `Init` and the loader are CH1q2's (`ch0_specter.tsv`), so no shared-header edit here.
- **No divergence row for `322d39e0`:** `CX2GameUnit::DrawMpLossText` (ENCHANT_PARITY, same strings) is that function; the defender's draw call is `DrawMpLossText( intake, defender Bip01, defender dir, false )`.
- Specter effect position = impact point + facing vector x ( right ? -80 : 80 ), y = `GetLandPos( false ).y`, cool 0.5 (June constants). Mana Drain particle `SI_A_ALS_MANA_DRAIN_10` gets `SetBlackHolePosition( Bip01_Ponytail1 )` (June writes seq +0x310; Arme's `SetBlackHolePosition` store is the same offset, `32cd1a1d`). Get Blood counts `hitUnitList.size()` (June divides the vector's bytes by 12).
- **JUNE_BUGS 594** (Mana Drain text and retake use the full amounts whatever MP the target had).

**Gates**
- `flagcheck CHAR_ARA_CORE --with _LIVE _FP _SKILL _SPECTER JUNE_ENUM_NAMES --worktree --files X2GUAra.cpp X2GUAra.h`: `.cpp` PASS +95, `.h` PASS +17; `files: 2 PASS, 0 FAIL, 0 ERROR`, `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`. `Always.h` +33: `DEFINE`, `RESULT: PASS`. `flagcheck JUNE_ENUM_NAMES --files X2SkillTree.h X2DamageManager.h`: `2 PASS~`, `RESULT: PASS`.
- One-file `/Zs /EHsc` probes (`census.cl_cmd`, no PCH, `X2GUAra.cpp`): exit 0, 0 errors with `_LIVE` off, `_LIVE`, and `_LIVE` + `_FP` + `_SKILL` + `_SPECTER` (first `_LIVE` run caught the 7 missing `RNSI_` names: registered). `cl /P`: `EffectSet_SpecterOfThunder_Buff_Damage` 0 / 0 / 1 (off / `_LIVE` / all four), `EffectSet_SI_A_ALS_MANA_DRAIN_HIT` 0 / 0 / 1 (1 with `_LIVE` + `_SKILL`), `ProcessFullHyperHitReward( pDamageData )` and `UpForcePowerBySkillHit( pDamageData )` 0 / 0 / 1, `m_bRaidHit = true` 0 / 0 / 1.
- After the commit (`0e81a873`), `flagcheck --range HEAD^..HEAD`: `CHAR_ARA_CORE --with _LIVE _FP _SKILL _SPECTER`: `files: 2 PASS, 1 DEFINE, 2 OTHER (the two enum files), 0 FAIL, 0 ERROR`, `RESULT: PASS`; `JUNE_ENUM_NAMES --files X2SkillTree.h X2DamageManager.h`: `2 PASS~`, `RESULT: PASS`.
- `junediverge.py --check X2GUAra.cpp X2GUAra.h X2SkillTree.h X2DamageManager.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2GUAra.cpp X2GUAra.h`: 0 A/B rows.
- Full build skipped: no play-test (`_LIVE` is off), so no `DEPLOY OK` / `exestr` line. No `_DEBUG` define exists; every new block carries `// char-ara CH1h2`.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1i` (Sonnet 5.5 high). The CH1k review covers CH1h-CH1k.

### CH1i - Done (2026-11-11)

**14 class-map rows accounted for: 3 written `inside` ( `_LIVE` ), 4 written `new` ( `_CORE` ), 7 = March ( no code ).** funcmap rows `Char Ara CH1i` ( 14 ); `JUNE_BUGS` 595.

| June | `CX2GUAra::` | Result |
|---|---|---|
| `32b95cf0` | SetEquippedSkillLevel ( 0x7b8 ) | **inside.** 41 blocks in June's order, keyed by June's `RNSI_` ids; `GetEquippedActiveSkillTemplet( id, true )` then `SetEquippedSkillLevelStateData` ( 64 sites ). 7 blocks ( 27043-27046, 27226, 27047, 27048 ) and Specter of Storm's two degree states ( templet `+0x178` = `m_iDeleteSkillIDByUpgrade` != 0 ) test the templet first. The callee `SetSkillLevelStateData` is CH1o's ( its `+ 1.0` term ) |
| `32ba83d0` | UpdateNotRenderingOtherEquipList ( 0x630 ) | **inside.** June's own function ( not the base's ): hyper mode only; 16 one-piece groups ( `DITT_` names, `IsDivisionItem` on the one-piece equip ) hide promotion equips by position; the arm, face1 and face2 accessories pick the hand / hair / face hides by group and by whether a one-piece group hid anything ( `bEquipHidden` ). The 17 hide loops share two named helpers in `_CONST_ARA_CH1I_`; `SetHideNoRenderable` as June ( +0x9170 ) |
| `32ba4710` | IsInJobLine ( 0x670 ) | **new.** Lines 1 / 2 / 3 = classes 28 / 118, 31 / 119, 34 / 124 ( declared by CH1d ) |
| `32ba4620` | ReplaceStateIDBySkill ( 0x6b4 ) | **new.** Skill behind the state id ( `GetSkillIDByEquipSlotID` = June `32890140` ) through `GetUpgradeBaseSkillID` ( `32207210` ): Wolf Spear 27014 / 27039, Steal Soul 27020, Sever 27024, Get Blood 27026 -> `ASI_SA_ALD_WOLF_SPEAR`, `ASI_A_AYR_STEAL_SOUL`, `ASI_SA_ALS_SEVER`, `ASI_SA_AA_GET_BLOOD` ( June 262 / 271 / 301 / 311 ); caller `32d3f140` compares it with the running state |
| `32b956d0` | IsDamageTriggerConditionMet ( 0x74c ) | **new.** Returns true. Found its caller by byte scan: `EffectToUnit` `3262efca` asks a user defender with the CEffect's `+0x7ac` ( `DAMAGE_TRIGGER_CONDITION`, -1 = none ) and skips the hit on false; March has no caller |
| `32bd6a70` | FindSkillFPGainEntry | **new.** `find_if` over the 0x18-byte FP-gain records by the skill-tree same-skill test; CH4a's `32bca010` calls it |
| `32bc1a90` `32bab410` `32bc89e0` | HyperModeFrameMove, CanRidingState, UpdateEquipViewListInfo | **= March.** `CommonHyperModeFrameMove( 0.693, 0.759, 1.287, false )`; the 13 / 14 / 19 / 20 / 75 / 76 / 77 / 81 / 82 / 154 state list ( `USI_*` and `ASI_HYPER_WAIT` ); the Berserk helper trio called in June's order. `32bc89e0` takes a third `slotId` in June: Stage 8's, as Chung CH1l0 |
| `32ba6990` `32baea90` `32bc15d0` | Get / Remove / Add BerserkEquip... | **= March**, read in full. June's differences are Stage 8 / 16 / 33 pieces ( Lu/Ciel form table, `IsDisabled` and `0x4d` usable test, 3-argument `GetEquippingItemByEquipPos`, enchant getter `32882540` ); CreateEquip's unit type is zeroed by its ctor unless UC_ANYONE, which is March's gate |
| `32bae630` | SetShowViewList ( 0x63c ) | **= March.** June first clears `bShow` when the riding byte `+0x93da` is 1 ( not ported, Stage 17, reads 0 as in CH1h ); no code |
| `32bd7780` | vector<int> growth helper | compiler template, no code |

**Names registered.** `X2SkillTree.h` ( `JUNE_ENUM_NAMES`, values checked against `Enum.src.lua` by the generator ): 39 `RNSI_` names ( `RNSI_SA_ARA_WIND_WEDGE` 27003 ... `RNSI_SA_ASR_SPECTER_OF_BLADE` 27048, plus `_WOLF_SPEAR_UPGRADE` 27039 and `RNSI_SA_ALS_SEVER` 27024 ). `X2GUAra.h` ( `_CORE`, `ARA_STATE_ID` tail ): 27 June-only state members with their June script ids and owning phases ( `ASI_SA_ASR_SHADOW_RISING_LANDING` 195 ... `ASI_A_ASD_SWALLOW_ASSAULT_TEETH_END` 342, `ASI_SA_ALS_SEVER` 301, `ASI_SA_AA_GET_BLOOD` 311 ); C++ values follow the enum, registration is by script name ( CH1m ). **Three June states keep March's member:** 223 `ASI_A_ALH_ROCK_CRASH_FALLING_START` = `ASI_A_ALH_ROCK_CRASH_START`, 251 `ASI_A_ASD_FALLING_BLOSSOM_Finish` = `ASI_A_ASD_FALLING_BLOSSOM_FINISH`, 250 = `ASI_SA_ASD_SUPPRESSION` ( CH0's pair rows ). State 305 has two June names ( `..._SPECTER_OF_STORM_FINISH` / `_ULTIMATE_FINISH` ); the first is used.
**Always.h:** +12, guards only ( `_LIVE` needs `SERV_IRUHADEV_JUNE_DIVERGE_NOT_RENDERING_EQUIP`, `_CORE` needs `SERV_IRUHADEV_JUNE_SKILL_TEMPLET`; both defined above the guard, lines 10814 / 7968 ). No new flag.
**Routed.** CH1m: register the 27 new state names by script name. CH2 / CH3 skill phases own those states ( comment on each member ). CH1o: wire `ReplaceStateIDBySkill` ( 0x6b4 ), `IsDamageTriggerConditionMet` ( 0x74c, with an `EffectToUnit` caller ) and `IsInJobLine` ( 0x670 ); `SetSkillLevelStateData`'s `+ 1.0` term. Stage 8 / 16: the equip-view set listed above.

**Gates.**
- `flagcheck JUNE_CHAR_ARA_CORE --with JUNE_CHAR_ARA_LIVE JUNE_CHAR_ARA_FP JUNE_ENUM_NAMES --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `.cpp` PASS +759, `.h` PASS +31; `files: 2 PASS, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `flagcheck JUNE_ENUM_NAMES --worktree --files X2Lib/X2SkillTree.h`: `PASS~`, `RESULT: PASS`. `Always.h` +12 ( pure insertion ): flagcheck reports `ERROR` at the earlier phase's line 12220 ( a conditional it cannot evaluate ), as for CH1h.
- After the commit ( `9b0e5c05` ), `flagcheck JUNE_CHAR_ARA_CORE --with _LIVE _FP JUNE_ENUM_NAMES --range 9b0e5c05^..9b0e5c05`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 1 OTHER ( X2SkillTree.h, JUNE_ENUM_NAMES ), 0 FAIL, 1 ERROR` ( the ERROR is `Always.h`'s earlier line 12220, which the tool cannot evaluate; my 12 lines are a pure insertion ), `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`.
- One-file `/c /Zs /EHsc` probes ( `census.cl_cmd`, no PCH, `X2GUAra.cpp` ): exit 0, 0 errors with `_LIVE` off, with `-DSERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`, and with `_LIVE` + `_FP` + `_SKILL` + `_SPECTER`. `cl /P` off / `_LIVE`: `HideEquipsAtPositions(` 0 / 21, `RNSI_SA_ARA_WIND_WEDGE, true` 0 / 1, `DITT_ITEM_ID_ONE_PIECE_AVATAR_RENDERING_HAIR,` 0 / 1, `SI_A_AM_WIND_WEDGE` 7 / 5.
- `junediverge.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2SkillTree.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `0 A/B row(s)`.
- Full build skipped: no play-test ( `_LIVE` is off ), so no `DEPLOY OK` / `exestr` line. No `_DEBUG` define exists; every new block carries `// char-ara CH1i`.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1j` ( Sonnet 5.5 high ). The CH1k review covers CH1h-CH1k.

### CH1j - Done (2026-11-11)

**19 class-map rows accounted for: 4 written `inside` ( `_LIVE` ), 10 written `new` ( `_CORE` ), 5 = March ( no code ).** funcmap rows `Char Ara CH1j` ( 20 ); `JUNE_BUGS` 596. The kickoff's 4 identified callees need nothing: `32feae90` is `ChangeAnimTail`'s string getter ( = March ), `32e87810` is the vector copy inside the record erase, `3207ff00` / `33048830` have no live caller.

| June | `CX2GUAra::` | Result |
|---|---|---|
| `32bc8380` | CommonStateStartFuture ( 0x7c0 ) | **inside.** `m_fNowSkillPassiveRate = GetNowSkillPassiveRate()`; Valor Devotion's buff and a Full Moon Slash group skill multiply it by `SA_CHAIN_SKILL_ANI_SPEED_REL` ( 0xb0 ); future state skill > 0: `UpForcePowerByReadySkillGain` ( `32bab5c0`, CH1q ) under `_FP`, then the FP-gain reset ( `32bd7230`, four lines as Init ) unless the new skill is the tracked one; else `CleanupSkillFPGainTable`, `m_bSkillFPGainLocked = false`, +0x9834 = 0. **The base runs last** ( June jumps to it ) |
| `32bd2a40` | CommonStateStart ( 0x7c4 ) | **inside.** `USE_FORCE_POWER` / `ADD_FORCE_POWER` ( March's members ), `FP_CANCEL` ( new -1 / -1 else ), `ChangeAnimTail`, `StopKillEnemyEffectSet`, the base, `MarkSkillFPGainHit( previous skill, serial )` when the damage data's skill or serial changed, brutal piercing ( March ), `SetUsedForcePowerAtThisState( 0 )`; `_SPECTER`: Necromancy's fox effect set ( `EffectSet_SI_SA_AA_NECROMANCY_FOX_ATTACK%d` ); then the Asura X-X-down-X-Z black hole is stopped outside its three states ( members tested one by one: the C++ values are not consecutive ). **June has no `DAMAGEDATA_CHANGE_TIME` reads** |
| `32bce2e0` | CommonFrameMove ( 0x7cc ) | **inside.** Base, the three new helpers, Link Flower Slash time `-=` dt, remote FP sync, promotion equips, the tail equips ( new loop ), Necromancy / Specter of Thunder cool times, Protection Fortitude ( March ), `USE_FORCE_POWER` ( the count falls back to the equipped skill's `m_usFPConsumtion` when 0, oneshot timer ), `ADD_FORCE_POWER` ( March ), the berserk advantage timer |
| `32baa0d0` | UpForcePowerAtThisAnimationTime | **inside.** June's pre-test + `EventTimer` = March's `EventTimerOneshot`; MP conversion = `UpForcePower`; new: `SetSkillBuffFactors( RNSI_P_AYR_OPEN_MIND, 0, true )` after the gain ( 3 lines under `_LIVE` ) |
| `32ba7ec0` `32ba7b80` | GetNowSkillPassiveRate, GetPassiveSkillRate | **new.** Future state skill's upgrade base picks Full Moon Slash ( 27402 ) / Power of Tiger ( 27403 ) / Howling ( 27407 ) groups ( switch tables decoded from the exe, groups in `_CONST_ARA_CH1J_` ), then the Hell Gate Open ( 27408 ) / Load of Asura ( 27412 ) / Fox Smile ( 27413 ) helpers, else 1.0. The rate is the passive's ability 0xb0 **at level 1** ( JUNE_BUGS 596 ) |
| `32b95a00` `32b95b00` `32b95b30` | IsSkillAffectedByHellGateOpen / LoadOfAsura / FoxSmile | **new.** Upgrade base in 27016 / 27018 / 27020 / 27209, 27004 / 27022 / 27024, 27026 / 27214 / 27215 / 27218 |
| `32ba7cf0` `32ba7da0` | UpdateOpenMindBuffByForcePower, UpdateValorDevotionTimer | **new.** Open Mind ( 27415 ): full gauge sets buff 0 and drops buff 1, empty the reverse, between drops both ( `3229ea50` = `EraseBuffTempletFromGameUnit( GetSkillBuffTempletID )`, Raven's idiom ); Valor Devotion ( 27416 ): timer `-=` dt, full gauge reloads it with `SA_TIME_ABS` ( 0xa9 ), empty gauge with timer > 0 resets it and sets the buff again |
| `32bc13c0` | CreateTranscendenceDamageEffectByTimer | **new.** Open Mind of Yama Raja ( 27417 ) learned, counter > 0, more than **0.1 s** since the last one ( the CH0 note said 1/3 s: the constant at `334395e8` is 0.1 ): counter `--`, `DamageEffect_P_AYR_TRANCENDENCE_Hell_Ghost` at Bip01 with June's 23 arguments ( reserve time 1.0 ) |
| `32bc5ea0` `32bc6aa0` | StopKillEnemyEffectSet, CleanupSkillFPGainTable | **new.** The kill-reward effect set ends with the next state unless it is Swallow Assault ( 244 ); the FP-gain table drops entries a hit used or that went negative, then empty records |
| `32b90be0` `32b90bf0` `32b90c00` `32bb0bf0` `32bd7280` | CommonStateEndFuture, CommonStateEnd, CommonRender_Prepare, CommonFrameMoveFuture, ChangeAnimTail | **= March.** The first three are the base ( `jmp` / hint ); `32bb0bf0` is the base, the gauge FP into the Future sync value ( protected `+0x7ec0` = `syncData.m_CannonBallCount` ) and the combo chain ( `32bd6de0` / `32bd7b10` are the shared COMDAT copies ); `32bd7280` reads the same two keys and defaults |

**Names / members.** `X2SkillTree.h` ( `JUNE_ENUM_NAMES` ): 10 `RNSI_` names ( 27030, 27037, 27223, 27402, 27403, 27407, 27408, 27412, 27413, 27415 ), values from `Enum.src.lua`. `X2GUAra.h` ( `_CORE` ): 9 members ( June offsets in `ch1_offsets.tsv`, now named ): `m_fNowSkillPassiveRate` ( June `CX2GUUser` +0x8908, **Ara-held stand-in** ), `m_hKillEnemyEffectSet`, `m_fNecromancyFoxCoolTime`, `m_fNecromancyFoxPowerRate`, `m_fValorDevotionTimer`, `m_usHellGhostCount`, `m_dwHellGhostTime`, `m_fLinkFlowerSlashTime`, `m_iFPGainStateSkillID`; their constructor values are in the initializer list. **Renamed ( 5 deleted lines ):** CH1h's `m_iSkillFPGainCount` ( +0x9844 ) is a skill id ( the same-skill test reads it; `32ba62c0` matches entries by it ), now `m_iSkillFPGainSkillID`; the `m_iUnread` comment says the cleanup reads it.
**Always.h:** +9, guards only ( `_CORE` needs `EVE_PASSIVE`, `USER_SKILL_LEVEL_HELPERS`, `DIVERGE_SKILL_LEVEL_VALUE`, `ENUM_NAMES`, all defined above it at lines 10164 / 10153 / 10329 / 10425 ). No new flag; `_LIVE` stays `//#define`.
**Routed.** CH1q2: June's base `CommonStateStartFuture` ( `320506f0` @`32051bef` ) multiplies the state power rate member ( +0x8900 ) by `m_fNowSkillPassiveRate` and resets it to 1.0; until it is ported the rate has no effect ( `ch1_offsets.tsv` row 0x8908 ). CH1q: `UpForcePowerByReadySkillGain` ( `32bab5c0` ) body, the writer of `m_hKillEnemyEffectSet`, and the meaning of +0x9834 / +0x9838 ( `m_iFPGainStateSkillID` is named from its writers' use of vslot 0x524 ). CH3c: `m_fNecromancyFoxPowerRate` is set by the Necromancy start state ( `32b94690` ). CH3a2: `m_fLinkFlowerSlashTime` is set by the Link Flower Slash states. The time tick ( timeGetTime vs GetTickCount, two different imports in June ) is written as `timeGetTime()` in both sites.

**Gates.**
- `flagcheck JUNE_CHAR_ARA_CORE --with JUNE_CHAR_ARA_LIVE JUNE_CHAR_ARA_FP JUNE_CHAR_ARA_SPECTER JUNE_ENUM_NAMES --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `.cpp` PASS +426 -3, `.h` PASS +23 -2; `files: 2 PASS, 0 FAIL, 0 ERROR`; `5 deleted line(s)` ( the renames above ), `0 non-ASCII inserted`, `0 EOL-mismatched`, `0 #else/#endif without the flag token`, `0 encoding change(s)`; `RESULT: PASS`. `flagcheck JUNE_ENUM_NAMES --worktree --files X2Lib/X2SkillTree.h`: `PASS~`, `RESULT: PASS`.
- After the commit ( `5ca87951` ), `flagcheck JUNE_CHAR_ARA_CORE --with _LIVE _FP _SPECTER JUNE_ENUM_NAMES --range 5ca87951^..5ca87951`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 1 OTHER ( X2SkillTree.h, JUNE_ENUM_NAMES ), 0 FAIL, 1 ERROR` ( the ERROR is `Always.h`'s earlier line 10175, which the tool cannot evaluate; my 9 lines are a pure insertion ), `5 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`.
- One-file `/c /Zs /EHsc` probes ( `census.cl_cmd`, no PCH, `X2GUAra.cpp` ): exit 0, 0 errors with `_LIVE` off, with `-DSERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`, and with `_LIVE` + `_FP` + `_SPECTER`. `cl /P` off / `_LIVE` / `_LIVE` + `_FP` + `_SPECTER`: `m_fNowSkillPassiveRate = GetNowSkillPassiveRate` 0 / 1 / 1, `"DAMAGEDATA_CHANGE_TIME"` 1 / 0 / 0, `MarkSkillFPGainHit( iPrevAppliedSkillID` 0 / 1 / 1, `"EffectSet_SI_SA_AA_NECROMANCY_FOX_ATTACK%d"` 0 / 0 / 1, `UpForcePowerByReadySkillGain();` 0 / 0 / 2 ( call + declaration ), `"DamageEffect_P_AYR_TRANCENDENCE_Hell_Ghost"` 1 / 1 / 1 ( the `_CORE` body ).
- `junediverge.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2SkillTree.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `0 A/B row(s)`.
- Full build skipped: no play-test ( `_LIVE` is off ), so no `DEPLOY OK` / `exestr` line. No `_DEBUG` define exists; every new block carries `// char-ara CH1j`.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1k` ( Sonnet 5.5 high ); its review covers CH1h-CH1k.

### CH1k - Done (2026-11-11)

**16 class-map rows plus `ReservedSkillStart` accounted for: 10 written `inside` ( `_LIVE` ), 6 written `new` ( 5 `_CORE`, `IsNotCommonMoveState` `_LIVE` ), 1 = March ( `32baea70` ).** The map lists the lookup / use / cooldown ( `32bcfaa0` / `32ba6610` / `32bb1ff0` ) as `inside` with no March pair; the lookup and the use sit inside March's `CanUseSpecialAttack` and `StateChangeSpecialAttack( SKILL_ID )` ( the CH0 allow row names `CanUseSpecialAttack` ), the cooldown is `new` as Rena's. funcmap rows `Char Ara CH1k` ( 17 ); `JUNE_BUGS` 597; checklist rows L214 / L253 / L262 / L263 / L265 / L867 appended `(Ara: CH1k)`.

| June | `CX2GUAra::` | Result |
|---|---|---|
| `32b90c20` | SpecialAttackEventProcess ( 0x7f4 ) | **inside.** Lookup, use, `Send_SKILL_USE_REQ`; a skill of use condition 3 / 5 ( anytime ) gives `m_fReservedSkillMp`, the chosen skill / slot, the `+0x9621` probe ( `_FP` ) and `m_usForceGainCount` ( `+0x9622` ) back; a failed lookup restores only the skill id |
| `32bcfaa0` | CanUseSpecialAttack ( 0x7f8 ) | **inside.** Quick-slot wait, slot, templet-mismatch text `0x8bae`, use condition, summon / PvP / two ban / riding-key checks, Conversion Health ( memo 0, same-or-upgrade 27210, `CanUseConversionHealth` ) text `0x68f4`, MP, `ShownSkillCoolTime`, **five slots a bank** ( `>= 5`, `- 5` ), FP need ( `GetActualFPConsume` + the probe ) and gain ( 0 for Reflection ), chain block ( no ASSERT, no learned-slot message 25110; a missing slot reads cool time 0 ), `+0x9626` = FP need, dev gate around cool time / MP / FP ( ready entries added under the probe ) / hyper stone, Moonlight Slash data ( June also reads ability 5 and drops it ) |
| `32ba6610` | StateChangeSpecialAttack ( 0x7fc ) | **inside.** `+0x971c` = skill, `+0x970c` = skill while `m_bFPCancel`; chain info: reserve + chain state; no-stage-change: cooldown, memo refund, `m_fChosenSkillMp` = 0; else reserve, `StateChangeLinkFlowerSlash`, slot state; processed = true, `m_bFPCancel` = false. The three CH1d / CH1g FP-cancel sites that said "stand-in ... CH1k's" are now this call ( comments only ) |
| `32bb1ff0` | SpecialAttackCooldown ( 0x800 ) | **new `_CORE`.** Slot used, `UpNowMp( -mp )`, Open Mind 27415 with an empty gauge and `m_fUseForcePowerTime` unset ( `+0x9690` ) gives `SA_FP_UP_ABS` force power through `UpForcePower` ( June inlines the `+0x9644` conversion ) and sets 27417's buff factors; finality stone; `SetSkillCoolTimeLeft( GetActualCoolTime )`; `OnSkillUse` **then** `ApplySkillCoolTimeRateByBuff` ( March did it the other way round ) |
| `32057b80` | ReservedSkillStart | **new `_LIVE`.** `SpecialAttackCooldown( skill, reserved MP )` + `SkillMemoMpRefund`; `X2GUUser.h` / `.cpp` hook chain widened with Ara's `_LIVE` |
| `32baf980` `32b95590` | SpecialAttackNoStageChange ( 0x810 ), IsReflectionSkill | **inside** / **new.** Reflection ( 27213 ) and its upgrade ( 27238 ) set the future action `UAI_REFLECTION`, answer true |
| `32bc84b0` | NoStateChangeActionFrameMove ( 0x80c ) | **inside.** Base `32008e40` first; Reflection: buff factor 259 with adjust skill 27213 ( 7ai ), effect set, sound, `UseForcePower` of the skill's FP ( 2 without it ), skill-name string; **action 0x27** ( no March enumerator, `UAI_RESURRECTION_HEAL` ): heal effect set, "HP +n" text, protection debuff 264 at 27409's level, its duration = the resurrection cool time |
| `32bae3d0` `32baea70` | ShowActiveSkillCutInAndLightByBerserkMode, ...ByScript | **inside** / **= March** ( June jumps to the first with the same arguments ). `EventTimerOneshot` gate, cut-in 0 ( 1 with `IsFullHyperMode()` and buff hyper count >= 3 ), event 0x8b3 swap ( Ara's own `s_SkillCutInSetEvent8b3`, CASH_21 / 22 ), `Dummy2_Lhand`; the cut-in item override ( `31fd8a50` ) is Stage 12 |
| `32ba44e0` `32b95560` | GetActualFPConsume, GetActualFPGainCount | **inside.** Jump tables read from the exe bytes: Wind Wedge ( 27003 / 27032 ) memo 2 -> 1, Shadow Knot ( 27004 / 27033 ) memo 5 -> 2 ( March: 3 ); the probe argument ( default false: `X2Game.cpp:15111` keeps two ) takes one off for the upgrade base 27201 / 27205 / 27211 / 27217 / 27221; the gain count only Wind Wedge itself. The memo tests read false ( `DIVERGE_SKILL_MEMO` ) |
| `32badb50` | CanUseConversionHealth | **inside.** `GetSkillNumericalByName( "HP_CONSUME_REL", 27210 ) x GetMaxHp()`; the level argument is not read |
| `32b95870` | StateChangeLinkFlowerSlash | **new.** 27043: while `m_fLinkFlowerSlashTime > 0` the 1st state goes to the 2nd, the 2nd to the LAST ( `+0x9878` ), else the 1st |
| `32ba62c0` `32bab090` | MarkNowSkillFPGainReady, GetReadyFPGainOfSkillHit | **new.** Entry layout fixed from the listings: bytes +0 / +1 / +2, ints +4 / +8; `+1` is set when the attack serial is in the serial vector and +8 equals `+0x9840`; the getter needs +0 and +1 and a hit id >= 0 ( JUNE_BUGS 597 ) |
| `32bab220` | IsNotCommonMoveState ( 0x828 ) | **new `_LIVE`.** Base test less `ASI_HYPER_WAIT` ( future state ), `ASI_DASH_JUMP_POWER_LANDING`, `ASI_AIRIAL_DASH`, `ASI_DOUBLE_JUMP_FRONT` / `_UP` |

**Names / members.** `X2SkillTree.h` ( `JUNE_ENUM_NAMES` ): 10 `RNSI_` names ( 27032, 27033, 27201, 27205, 27210, 27211, 27217, 27221, 27238, 27409 ). `X2GUAra.h`: 4 `_CORE` members ( `m_iLinkFlowerSlashStateID` +0x9878, `m_usSkillFPConsumption` +0x9626, `m_iFPCancelSkillID` +0x970c, `m_iUsedSkillID` +0x971c ), constructor values in the initializer list; the FP-gain entry / record fields CH1h and CH1j had guessed are now the listing's ( `m_bEnabled`, `m_bReady`, `m_iGainHitID`, record `m_usGain`; CH1j's `CleanupSkillFPGainTable` follows ). **Always.h:** +12, guards only ( `_CORE` needs `USER_SKILL_USE_HELPERS`, `BUFF_NEW_SKILL`, `RANDOM_MISSION`; `_LIVE` needs `USER_SHOWN_SKILL_COOL_TIME`, `USER_NUMERICAL_BY_NAME`; all defined above it, and the three probes pass them ). No new flag; `_LIVE`, `_FP` stay `//#define`.
**Routed.** CH1p: June's pressed-index decode ( `>= 5` here ); until then the base `CommonSpecialAttackEventProcess` hands Ara March's index ( the 7ag phase ). CH4a ( 27409 ): who writes action 0x27 ( March's `SetResurrection` heals locally; June's remote-visible writer is not in the class map ) and the resurrection test `32bc5a50`. CH1q: `UseForcePower` ( `+0x970c` discount ), the `+0x9614` used-force-power counter ( CH0 wrote "June keeps none": wrong, CH1j clears it and the Reflection action adds to it ). Review: forcing `_LIVE` / `_FP` / `_SPECTER` / `_SKILL` on in `junediverge.run_check` gives one hit outside this phase, CH1h's `ReInit` `GT_PVP` ( `:379`, June tests type 1 only: needs its allow row ).

**Gates.**
- `flagcheck JUNE_CHAR_ARA_CORE --with JUNE_CHAR_ARA_LIVE JUNE_CHAR_ARA_FP JUNE_CHAR_ARA_SPECTER JUNE_ENUM_NAMES --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `.cpp` PASS +600 -7, `.h` PASS +22 -3; `files: 2 PASS, 0 FAIL, 0 ERROR`; `10 deleted line(s)` ( 6 stand-in comment lines, 1 renamed member use, the entry / record field lines ), `0 non-ASCII inserted`, `0 EOL-mismatched`, `0 #else/#endif without the flag token`, `0 encoding change(s)`; `RESULT: PASS`. `X2GUUser.cpp` / `.h` ( `--worktree`, `JUNE_CHAR_ARA_LIVE` ): PASS +15 / +15, `RESULT: FAIL` only because `_LIVE` is a `//#define`. `X2SkillTree.h` ( `JUNE_ENUM_NAMES` ): PASS +10. `Always.h`: the tool cannot evaluate it ( an earlier `#if` ), my 12 lines are a pure insertion.
- After the commit ( `464e738c` ), `flagcheck JUNE_CHAR_ARA_CORE --with _LIVE _FP _SPECTER JUNE_ENUM_NAMES --range 464e738c^..464e738c --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `10 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`; `X2GUUser.cpp` / `.h` ( `JUNE_CHAR_ARA_LIVE`, `--files` ): 2 PASS +15 / +15, `RESULT: FAIL` only for the commented-out `#define`; `X2SkillTree.h` ( `JUNE_ENUM_NAMES` ): PASS +10, `RESULT: PASS`. The unrestricted range run stops on an `Always.h` / `X2GUUser.cpp` `#if` the tool cannot evaluate ( line 27130 ), so the commit's files were checked in three `--files` groups.
- One-file `/c /Zs /EHsc` probes ( `census.cl_cmd`, no PCH ): `X2GUAra.cpp` exit 0, 0 errors with `_LIVE` off, with `-DSERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`, and with `_LIVE` + `_FP` + `_SPECTER`; `X2GUUser.cpp` and `X2Game.cpp` off and with `_LIVE`: 0 errors. `cl /EP` off / `_LIVE` / `_LIVE` + `_FP` + `_SPECTER`: `m_iUsedSkillID = eSkillId_` 0 / 1 / 1, `EffectSet_AYR_HEAL_FX` 1 / 2 / 2, `CX2GUAra::IsNotCommonMoveState` 0 / 1 / 1, `bFPCancelProbe = m_bFPCancelProbe` 0 / 0 / 1, `bSavedFPCancelProbe = m_bFPCancelProbe` 0 / 0 / 1, `GetReadyFPGainOfSkillHit( iReadyHitID )` 0 / 0 / 1, `HP_CONSUME_REL` 1 / 2 / 2.
- `junediverge.py --check` ( the five code files ): `allow-listed: 7`, `RESULT: PASS` ( `X2GUAra.cpp` / `.h` alone: `allow-listed: 0` ). `enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `0 A/B row(s)`.
- Full build skipped: no play-test ( `_LIVE` is off ), so no `DEPLOY OK` / `exestr` line. No `_DEBUG` define exists; every new block carries `// char-ara CH1k`.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1k review` ( Opus 5.5 high ), then CH1m.

### CH1k - Review (2026-11-11; covers CH1h, CH1h2, CH1i, CH1j, CH1k)

**Verdict: pass with one fix and one allow row.** Checked against `charport_adis.py ara` listings, not the Done notes.

| June | Checked | Result |
|---|---|---|
| `32bcfaa0` lookup | check order ( `+0x8d78` wait, `CommonSpecialAttackEventProcess`, slot, templet, mismatch text `0x8bae`, `m_iChosenSkillID` before `CheckSkillUseCondition`, summon / PvP / `3202d140` / `3202d3c0`, riding key `cmp al, 1`, `GetUnit()` null ); Conversion Health ( memo == 0, same-skill 0x6a4a, `CanUseConversionHealth( templet, level )`, text `0x68f4` ); MP `GetActualMPConsume( id, level, 1 )` = March's virtual; `>= 5` / `- 5` decode; `GetFPConsumption( slot, bSlotB )` -> `GetActualFPConsume( id, n, +0x9621 )`; Reflection 0 gain into `+0x9622`; chain block ( type `- 2` = `CASCT_SKILL`, else `ResetChaining`; slot found: the first level and raw cool time; slot missing: own level, cool 0; templet `+0xe` gain / `+0x10` need = March's field order ); probe-else ( `+0x9621` and the future state skill ); `+0x9626` @32bd01c2; dev gate 0x82c; cool `> 0` text 226, MP `reserved + chosen > now` text 2549, FP `<` need text 21303 ( the ready-gain out param reuses the need's stack slot, the compare uses `bx`: same as C++ ), hyper type 0xd + training + stone `0x1fca0` + infinity, text 26119 | **one gap fixed:** the Moonlight Slash tail stores the skill's `SA_EFFECTIVE_RATE` ( `32206fb0( 5, level, &out, 0 )`, out preset 1.0 ) into the Moonlight data **+0x20** @32bd08c5; CH1k's note said June "drops it". June's Moonlight data is 0x28 bytes ( ctor `32bd6e80`: +0x20 / +0x24 new floats, 1.0 ). Added `sMoonLightSlashData::m_fEffectiveRate` ( `_CORE`, ctor 1.0, `Get` / `SetEffectiveRate` ) and the store ( `TryGetSkillAbilityValue( SA_EFFECTIVE_RATE, iSkillTempletLevel, false, ... )` under `_LIVE` ). +0x24 routed to CH2b ( kickoff amended ) |
| `32ba6610` use | `+0x971c` / `+0x970c` ( `setne / dec / and` ); chain: reserve 0x804, hyper by gauge `+0xe8` > 0, chain info +4 / +8 else +0 = `GetStateIdMatchedToChain` / `GetHyperStateIdMatchedToChain` / `GetStateIdChained` ( layout read in the header ); no-stage-change: 0x800 ( skill, chosen MP ), 0x764 ( arg skill, 1, chosen MP ), MP = 0; else reserve, `32b95870`, `31fd32e0`; `+0x8ac4` = 1, `m_bFPCancel` = 0 | = C++ |
| `32bb1ff0` cooldown | `IsMyUnit`, `MarkSkillSlotUsed( chosen slot, 0 )`, `UpNowMp( -mp )`, Open Mind 27415 / FP <= 0 / `|+0x9690 + 1| <= FLT_EPSILON`, `SA_FP_UP_ABS` ( 0xbf ) inline = `UpForcePower` ( overflow x `+0x9644` into MP, then `SetForcePower` ), 27417 nested; templet, type 0xd and state != 0x12, `GetItemByTID( 0x1fca0, 0, 1 )`, finality REQ; level, `0x6a4` cool, `SetSkillCoolTimeLeft( .., 1 )`; the chain-info fetch is a discarded temporary; random mission then `3235a710` | = C++ |
| `32bc84b0` | base first; 0x1a: GetUnit / effect set / riding, level max 1, `0x21c` pushes ( skip 0, adjust 27213, level, creator 0, BFI 0x103 ) = 7ai's `( BFI, level, NULL, 27213, false )`, effect set, sound, FP `+0x10` or 2, `UseForcePower` ( `32bb2b40` ) **then** `+0x9614 += n`; 0x27: heal FX, PicCharGreen "HP +%d", BFI 0x108 at 27409's level, BTI 0x102 time finalizer = `m_fResurrectionCoolTime` | = C++. June's `UseForcePower` has no counter, March's has it: one count either way until CH1q replaces it ( routed by CH1k, `CH1q.md` names `32bb2b40` ) |
| `32ba44e0` / `32b95560` | jump tables decoded from the listing's data bytes: table 1 ( 27003 + i, 31 entries ) i 0 / 29 -> memo 1, i 1 / 30 -> memo 2; table 2 ( base - 27201, 21 entries ) i 0 / 4 / 10 / 16 / 20 -> `--` | = C++ |
| `32ba62c0` / `32bab090` / `32bab160` / `32bc6aa0` | entry +0 / +1 / +2 bytes, +4 serial, +8 hit id; record +0 gain ( ushort ), +4 skill, +8 vector; ready = +1 when `+4 == serial[i]` and `+8 == +0x9840`; the getter needs +8 >= 0, +1, +0, stops on hit id > 0; hit mark +2; cleanup drops +2 set or +8 < 0 | = C++; the CH1k renames ( `m_bEnabled`, `m_bReady`, `m_iGainHitID`, `m_usGain` ) agree with CH1h's and CH1j's functions |
| `32bab220` / `32b90c20` / `32057b80` | state ids 0x9a ( future ) / 0x9c / 0xa4 / 0xa5 / 0xa6 = enum order; save / restore of the five values for use condition 3 / 5, `m_iNowSpecialAttack = slot + 1`; hook @32057c4c-cb2 = 0x800 + 0x764 with the slot-state byte | = C++ |
| CH1h `32bc8030` | game type `vslot 0x10c`, `dec eax / je` ( type 1 only ) | the forced-`_LIVE` `junediverge` hit at `X2GUAra.cpp:379` is right: **allow row added** |
| CH1j `32bc8380` | `GetNowSkillPassiveRate`, 0xb0 at 0x6b18, `32bab5c0`, reset `32bd7230` / cleanup branch, base by `jmp 320506f0` last | = C++ |

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --with _CORE _FP _SKILL _SPECTER --range ae64b2a8^..464e738c`: `files: 12 PASS, 0 PASS~, 5 DEFINE, 7 OTHER, 0 FAIL, 0 ERROR`, `16 deleted line(s)` ( the five phases' renames and corrections ), `0 non-ASCII inserted`, `0 EOL-mismatched`, `0 #else/#endif without the flag token`, `0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is `//#define` ( Always.h:12193 ). The 7 OTHER are the enum headers: `flagcheck JUNE_ENUM_NAMES --range ae64b2a8^..464e738c --files X2SkillTree.h X2BuffFactor.h X2DamageManager.h`: `1 PASS, 6 PASS~`, `RESULT: PASS`.
- This fix: `flagcheck JUNE_CHAR_ARA_CORE --with _LIVE _FP _SKILL _SPECTER JUNE_ENUM_NAMES --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `.cpp` PASS +4, `.h` PASS +11, `0 deleted`, `0 non-ASCII inserted`, `0 EOL-mismatched`, `0 encoding change(s)`, `RESULT: PASS`. After the commit ( `ef7ed004` ), `--range ef7ed004^..ef7ed004` on the same files: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check-all`: `allow-listed: 202`, `RESULT: PASS`; `run_check` on `X2GUAra.cpp` / `.h` with `_LIVE` `_FP` `_SPECTER` `_SKILL` forced on: before the row `RESULT: FAIL, 1 hit(s)` ( `:379` ), after `allow-listed: 4`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `0 A/B row(s)`.
- One-file `/c /Zs /EHsc` probes of `X2GUAra.cpp` ( `june2017_re/census.py cl_cmd`, no PCH ), after the fix: exit 0, 0 errors with `_LIVE` off, with `_LIVE`, and with `_LIVE` + `_FP` + `_SPECTER` + `_SKILL`.
- Full build skipped: no play-test. No `_DEBUG` define in CH1h-CH1k, nothing to retire. New blocks carry `// char-ara CH1k review`.

**Kickoffs.** `CH1m.md` amended ( CH1i's 27 state names register by script name there; the allow row ); `CH2b.md` amended ( Moonlight data +0x20 / +0x24 ).

**Nothing to play-test.** Next: `/x2-phase char-ara CH1m` ( Opus 5.5 high ).

### CH1m - Done (2026-11-11)

All three class-map rows are `inside`, under `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` ( still `//#define` ) with March's bodies in the `#else`: wrapper `32bcf4a0`
-> `InitState`, body `32bca580` -> `InitStateCommon`, per-class part `32bcced0` -> `InitStateByUnitClass`. Class map corrected ( CH0 had both registrars as
"InitState" ). New in `CX2GUUser` ( `_CORE` ): `GetBerserkModeTableName` = June `31fdd800` whole ( checklist l.979 ).

**How it was derived.** `june2017_re/char_ara/ch1m_gen.py` ( Rena's generator, Ara's names ) writes the C++ from CH0's listing-checked `ch0_reg.tsv`;
`ch1m_spec.py` makes the bytepatch spec. Fresh decompiles of `32bcf4a0` / `32bca580` / `32bcced0` / `31fdd800` / `31fdd490` read against it.
Ids: the `June script id N` member, else March's registration of the table, else the member of that name: **138 June ids, one member each, both ways**
( `ch1m_idmap.tsv` ). Callbacks: 333 resolved, all equal to CH0's inside / proposed / shared pair except three shared targets ( `32bbac50`, `32b9cf00`,
`32bc6720`: two June states share one function, written once by CH1e / CH1g ).

**InitState `32bcf4a0`:** cut-in from the byte table `32bcfa1c`: 0 / 7 / 28 / 31 -> 0, 118 -> 1, 119 -> 2, 124 -> 3; **34 and every other class keep the
index** ( no write; cut-ins are CH5's ). `+0x4088` = 0xd = March `InitStateID`; `31fc93d0` ( empty stub ) not called. Then `InitStateCommon`,
`InitStateByUnitClass`, `InitStateCommonRandom` ( June runs `InitEventStates` 3202b110 between the two registrars; the ids do not meet ), START
( Start / EventProcess / End ), WIN ( `WinStateStart`, `RandomLoseEventProcess` ), LOSE, `InitWaitStateChange( "ASI_WAIT_STATE_CHANGE" )` @32bcf90d ( 7h ),
vslot 0x690 `InitPassiveSkillState`, 0x684( 0 ), `31fd8f00`. No WIN2-style compare in Ara's.

**InitStateCommon `32bca580`** ( no class test, 98 registrations ): hyper id 23 = `GetHyperModeTableName( false )`, berserk id 155 = `ASI_BERSERK_MODE` with
`GetBerserkModeTableName()`, both with June's hyper callbacks ( FrameMove = Ara vslot 0x600 `32bc1a90`, End = base 0x604 `3201b8e0`, Ara vtable `33505314` ).
Bounding = `InitBoundingStates( "ASI_" )` at June's place ( 7l ). Skill-chain lookups `32d3ee60` -> `m_ptrComboAndSkillChainInfo` on ids 169 / 181 / 182 / 185
( = March's four ). Left out, a comment line each: phoenix run 123-126 ( PvP ), dance 127-129 ( Stage 10 ), riding 93-96 ( Stage 17 ); empty slots `3202a7d0`
( die end, `StateChange` ) and `32002f70` ( Stage 17 ).

**InitStateByUnitClass `32bcced0`:** one fall-through switch, as June: 118 -> 28 ( 4 + 15 ), 119 -> 31 ( 2 + 7 ), 124 -> 34 ( 8 + 5 ); 7 registers nothing.
March's `ASI_P_ASD_QUICKEN_GUARD` / `_SMALL` are not registered ( June registers `ASI_P_ASD_BRUTAL_PIERCING` / `_SMALL` there ). Chain lookups on 280 / 282 / 283 / 325.

**`ParseCommonRandomState`** ( CH-N row ): under `_LIVE` it calls `ParseCommonRandomStateByUnitClass()` ( June `31feaac0` by class name, as Arme / Eve ), so
34 / 124 get their `ARA_LITTLE_SPECTER` / `ARA_ASURA` START / WIN / LOSE tables. `LoadPromotionItemID`'s CH-N row was CH1h's ( done ).

**CH1i's routed item.** The 27 June-only skill states are not registered by InitState in June: their cases in `32bd09d0` / `32bd34c0` do it ( CH2 / CH3,
owner on each member ). Their names are June script tables except `ASI_SA_ALS_SPECTER_OF_STORM_DEGREE` ( exe string, no script table: CH2d2 checks it ).

**JUNE_BUGS 598-600:** 598 `ASI_COMBO_ALH_XXXZX_Landing` ( exe ) vs the script's `_LANDING`: case-sensitive lookup, the state keeps no table; 599
`ASI_RIDING_DAMAGE_BACK` has no table, `_FRONT` is defined twice ( Rena's 571 ); 600 `ASI_WALL_LANDING` has no table in June's or March's script. All verbatim.

**Always.h:** +12, guards only: `_LIVE` needs `ARME_CORE`, `ARME_LIVE` ( `ParseCommonRandomStateByUnitClass` ), `EVE_CORE`, `HYPER_MODE_FIX`,
`JUNE_COMMON_RANDOM_STATE`, `JUNE_EVENT_STATES`, `JUNE_WAIT_STATE_CHANGE`, `JUNE_BOUNDING_STATE`; `_CORE` needs `ARME_CORE` ( `IsUnitTranscended`; the
helper sits in its block ). The `_LIVE` probe passes them. Checklist l.178 / 428 / 472 / 957 / 979 marked `(Ara: CH1m)`; funcmap rows updated.

**Checks.**
- One-file `/c /Zs /EHsc` probes ( `census.cl_cmd`, no PCH ): `X2GUAra.cpp` off, `-D_LIVE`, `_LIVE` + `_FP` + `_SPECTER`: exit 0, 0 errors each;
  `X2GUUser.cpp` off and `_LIVE`: 0 errors.
- `cl /P` off / `_LIVE`: `InitWaitStateChange( "ASI_WAIT_STATE_CHANGE" )` 0 / 1, `InitBoundingStates( "ASI_" )` 0 / 1, `"ASI_COMBO_ALH_XXXZX_Landing"` 0 / 1,
  `GetSkillChainInfoPtrByStateID( stateData.stateID )` 0 / 8, `m_iSkillCutInSetIndex = 3` 0 / 1, `"ASI_P_ASD_QUICKEN_GUARD"` 1 / 0.
- `git diff --patience --numstat`: `X2GUAra.cpp` 1163 / 0, `X2GUUser.cpp` 23 / 0, `X2GUUser.h` 3 / 0, `Always.h` 12 / 0; encodings and high-byte counts unchanged.
- `flagcheck JUNE_CHAR_ARA_LIVE --worktree --files X2Lib/X2GUAra.cpp`: `files: 1 PASS`, `0 deleted, 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif
  without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only for the `//#define`. `JUNE_CHAR_ARA_CORE --files X2Lib/X2GUUser.h X2Lib/X2GUUser.cpp`:
  `files: 2 PASS`, `RESULT: PASS`. `Always.h`: the tool cannot evaluate it ( an earlier `#if` ); a pure insertion.
- `junediverge.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2GUUser.cpp X2Lib/X2GUUser.h KTDXLIB/Always.h`: `RESULT: PASS`. `enumscan --check`: `0 A/B row(s)`.
- Full build skipped: no play-test.
- After the commit ( `2a45acf4` ): `flagcheck JUNE_CHAR_ARA_LIVE --range 2a45acf4^..2a45acf4 --files X2Lib/X2GUAra.cpp`: `files: 1 PASS`, `0 deleted, 0 non-ASCII
  inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: FAIL` only for the `//#define`; `JUNE_CHAR_ARA_CORE
  --range 2a45acf4^..2a45acf4 --files X2Lib/X2GUUser.h X2Lib/X2GUUser.cpp`: `files: 2 PASS`, same zeros, `RESULT: PASS`.

**For CH1 final:** June's chain table keys come from the script's `ASI_` values; the C++ passes the member, so `Enum.lua` must give the C++ values ( check
in CH1's play-test with a Little Hsien chain ).

**Nothing to play-test** ( `_LIVE` is off ). Next: `/x2-phase char-ara CH1m review` ( Opus 5.5 high ).

### CH1m - Review (2026-11-11, Opus 5.5)

**Pass, no code change.** Commit `2a45acf4` read against fresh Ghidra decompiles of `32bcf4a0` / `32bca580` / `32bcced0` / `31fdd800` and raw listings.
- **Generator.** `ch1m_gen.py` rerun: both `.inc` files = the `_LIVE` branches line for line ( only the spec's 4-5 header lines differ ); `idmap.tsv` =
  `ch1m_idmap.tsv` ( 138 ids, no duplicate either way; id 23 is the generator's hand row `USI_HYPER_MODE`, as in the five earlier ports ).
- **Independent simulation.** `charport_regsim` over the fresh decompiles ( scratch wrapper: Ara's `switch(uVar3)` on unit `+0x38` ): body 112 registrations,
  per class 28 = 15, 118 = 19, 31 = 7, 119 = 9, 34 = 5, 124 = 13, 7 = 0. Every ( id, table, thunk>target ) equals `ch0_reg.tsv` ( 112 / 112, 41 / 41, no
  extra either side ), which the generator transcribes. Shared targets: `32bbac50` / `32b9cf00` on 331 / 332 ( BRUTAL_PIERCING / _SMALL ), `32bc6720` on
  328 / 329 ( ASR_XZZ / _XZZZ ); the C++ names each once per state.
- **Wrapper.** Byte table `32bcfa1c` ( 125 bytes ): 0 / 7 / 28 / 31 -> 0, 118 -> 1, 119 -> 2, 124 -> 3, the other 118 -> 4; jump table `32bcfa08`: 0 ->
  `mov [esi+0x8ac8], ebx` ( `xor ebx, ebx` @32bcf4e8 ), 1 / 2 / 3 -> literals, 4 -> `32bcf532` ( no write ); class > 0x7c skips; null unit -> index 0. Delegates
  ( UserUnitStateData slots 2 / 6 / 8 = Start / EventProcess / End ): START `329cb6e0` -> 31ffbd70 `StartStateStart`, `329cb6f0` -> 3201b390
  `StartEventProcess`, `329cb1b0` -> 3201b460 `StartEnd`; WIN `329cb700` -> 320004b0 `WinStateStart`, `320718d0` -> 31fcd0b0 `RandomLoseEventProcess`; LOSE
  `329cb710` -> 320007e0 `LoseStateStart`. `PUSH 0x82` @32bcf902 with `33507130` "ASI_WAIT_STATE_CHANGE"; tail 0x690 / 0x684( ebx = 0 ) / `31fd8f00`.
  Order: no body or per-class id lies in the event range 0x6e-0x76, so running `InitEventStates` after the per-class part changes nothing. `31fc93d0` is called
  by June ( a bare `RET` ); no WIN2 compare.
- **Body.** Id 23 = `31fdd490( ..., 0 )`, 155 = `31fdd800( ..., 0 )`; FrameMove `32bd5f20` -> `32adf320` ( `jmp [eax+0x600]` ) -> Ara vtable `33505314` +0x600 =
  `32bc1a90` `HyperModeFrameMove`; End `329cc5a0` -> `329cc0e0` ( `jmp [eax+0x604]` ) -> +0x604 = `3201b8e0` = user vtable `334781f4` +0x604 ( `HyperModeEnd` );
  Start `32bd5300` -> 32b90f20 ( Ara-range thunk of base `HyperModeStart` ). Bounding after dance 127-129, before `ASI_THROW_ITEM` ( id 70 ) = the C++. Chain
  lookups `32d3ee60` 0xa9 / 0xb5 / 0xb6 / 0xb9 each directly before that id's registration = the C++'s four ( ZZZ, XXdownXX, DASH_COMBO_Z, DASH_COMBO_X ).
  Left out 123-126, 127-129, 93-96 match the decompile.
- **Per class.** One switch, 0x76 falls into 0x1c, 0x77 into 0x1f, 0x7c into 0x22, default skips; Specter lookups 0x118 / 0x11a / 0x11b / 0x145 each before
  their own registration ( ALS_X, XXXLoopZ, DASH_JUMP_COMBO_ALS_ZX, DASH_COMBO_ALS_ZX ) = the C++.
- **`31fdd800`.** Unit `+0xaf0` templet `[0] == 7` = `UT_ARA` ( `X2Unit.h:29` ); unit `+0x3c` = transcended; `+0x38` 0x76 / 0x77 / 0x7c -> ASD / AYR / AA. RET 0x8
  ( hidden return + an unread argument ). `GetBerserkModeTableName` matches.
- **`ParseCommonRandomState`.** `GetUnitClassName` ( `X2UISkillTreeNew2015.cpp:5640` / `5677` ) gives `ARA_LITTLE_SPECTER` / `ARA_ASURA`, both tables of June's
  `INIT_COMMON_RANDOM_STATE` ( `Ara_Martial_Artist.src.lua:719` ).
- **JUNE_BUGS 598-600.** `PUSH 0x13f` @32bcd266 ( string `33506c5c` ..._Landing; the script has `_LANDING` at 19229 ); `PUSH 0x56` @32bcc3b8 ( no `_BACK` table,
  `_FRONT` at 7238 and 7244 ); `PUSH 0xa3` @32bcbbc7 ( `WALL_LANDING` in neither June's source nor March's `Ara_Martial_Artist.lua` ). Confirmed.

Gates. `flagcheck JUNE_CHAR_ARA_LIVE --range 2a45acf4^..2a45acf4 --files X2Lib/X2GUAra.cpp`: `files: 1 PASS` ( +1163 ), `0 deleted, 0 non-ASCII inserted, 0
EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only for the `//#define`. `JUNE_CHAR_ARA_CORE --files
X2Lib/X2GUUser.h X2Lib/X2GUUser.cpp`: `files: 2 PASS`, same zeros, `RESULT: PASS`. `junediverge.py --check-all`: `allow-listed: 202`, `RESULT: PASS`.
`enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUUser.cpp`: `0 A/B row(s)`. Probes `/c /Zs /EHsc` of `X2GUAra.cpp` ( `census.cl_cmd`, no PCH ): off, `_LIVE`,
`_LIVE` + `_FP` + `_SPECTER`: exit 0, 0 errors each. No `_DEBUG` define to retire. Full build skipped: no play-test ( no code change ). `CH1n.md` checked.
Nothing to play-test. Next: `/x2-phase char-ara CH1n` ( Opus 5.5 high ).

### CH1n - Done (2026-11-11)

**Slot head (`32bd09d0`), inside, `_LIVE`.** June's head sits in March's own `SetEquippedSkillFuncBySkillSlotIndex` up to the id switch; March's head and
type switch are the `#else` (`_LIVE` still `//#define`d). Rena's CH1n is the template; read from the decompile and the exe bytes:
- Bounds `0 > slot || 4 < slot`; tables `334fea50` / `334fea78` (normal A / B) and `334fea64` / `334fea8c` (hyper) read from the PE = {50..58} / {60..68} /
  {51..59} / {61..69} = March's `_CONST_GUUSER_` arrays. `31fd2af0` dropped (7i); the `+0x11 = 1` stores (head and epilogue `@32bd2778`) dropped: March answers
  through `IsSkillSlotState`; hyper word `+0x10 = 0x101` = `m_bHyperState`. `31fca8e0( id, 1 )` = `GetEquippedActiveSkillTemplet( id, true )`.
- **Memo exit `@32bd0af5` (Ara differs from Rena).** `31fdbcc0( id )` (`GetTranscendenceMemoSkillID`), then `32206450( memo, 1 )` non-null: memo `0x6a56` / `0x6a6c`
  (`RNSI_A_ALD_CONVERSION_HEALTH_ADD_TRANS` / `_ADD_TRANS_UPGRADE`, 27222 / 27244, registered under `JUNE_ENUM_NAMES`) -> `32babb50` on both states = March's
  `ASI_A_ALD_CONVERSION_MANA_CreateState` (same table, Init / FrameMove / EventProcess); any other transcended skill returns. June's helper runs `Init()` on
  the hyper state too (clears its hyper flag), as March's own Conversion Mana case does.
- **Types** (raw `[templet +4] - 5`, 12 entries): 5 buff = `ASI_COMMON_BUFF` on both (FM `329ccd90` -> vslot 0x5f0 = Ara's `32ba6830` `COMMON_BUFF_FrameMove`, EP
  `329cba40` -> `31fcbd60`); 6 / 7 / 0xd-0x10 active; 0xc + id 900 = `ASI_THROW_ITEM` (vslots 0x5f4 / 0x5f8, not overridden); 8-11 and 17 / 18 return.
- **Raw id (Ara differs from Rena).** Both June switches take `param_2` itself: no `GetUpgradeBaseSkillID`. The active arm `break`s to March's id switch.
- **Hyper split = CH3d** (its kickoff says so; amended with the shape): `0x6bd0 < id`: 27601 / 27604 -> `32baab30` (Energy Cannon), 27602 / 27605 -> `32baace0`
  (Nine-Tailed Fox, June-only), 27603 -> Thousands Blossoms; `== 0x6bd0` Thousands Blossoms inline. Its default `@32bd271e` = the id switch default = Wind
  Wedge START (`32baf390`) + END 193 (`32baa230`) = March's `default:` placement on Wind Wedge: nothing new for CH1n.
- No cool-time / MP / hyper-stone block in `32bd09d0` or `32bd34c0` (checklist L263): the dev free-skill gate stays CH1k's (`CanUseSpecialAttack`).

**Second switch head (`32bd34c0` = `InitAquiredActiveSkillState`, vslot 0x68c), inside, `_LIVE`.** Unit null -> return; templet =
`GetUserSkillTemplet( GetUpgradeSwappedSkillID( id ), true )` (`328905b0` then `32890720`, which swaps once more: kept verbatim, harmless); null -> return;
switch on the raw id = March's frame. The swapped id is a local (`eUpgradeSwappedSkillID`) for the 56 case bodies' chain lookups (June @32bd35a1 ...):
routed to the skill phases (CH2a-CH3d kickoffs amended).

**`GetSkillChainInfoPtrBySkillID` (`32d3ed30`), replaced under `_LIVE`.** June also matches the chain skill's enhanced id (`GetUpgradedSkillID` `32205c00`,
skill tree present). June callers `32024b00` (`SetEquippedSkillLevelCommon`, every class) and `32bd34c0`. `june_march_divergent.tsv` row added.

**Base callbacks.** `june2017_re/char_ara/ch1n_triage.py` joins every callback target outside Ara's range in `ch0_reg.tsv` (174 VAs) with Raven's, Chung's,
Elsword's and Rena's CH1n triages -> `ch1n_user_callback_triage.tsv`: 174 / 174 found (61 SAME; DIFF: 15 ported for every class, 5 kept, 47 Stage 17), 46 `n/a`
(19 the `USI_EVENT_SNIPER_*` / `PROTON_BOMB_*` callbacks of `3202b110` = shared `InitEventStates`, Effect C9h-4h; 14 riding / empty, 3 phoenix, 3 dance, 5
bounding, 1 wait-state-change, 1 vslot 0x600 thunk). Nothing new: no `X2GUUser` site needs Ara's `_LIVE`.

**Not ported here, routed:** hyper split (CH3d), the 86 + 56 case bodies (their state phases), `CreateLockOnSystem`'s June member (CH2b0), l.286 (CH1o).

**Checks.**
- One-file probes (`census.cl_cmd`, `/c /Zs /EHsc`): `X2GUAra.cpp` off / `_LIVE` / `_LIVE` + `_FP` + `_SPECTER` and `X2ComboAndSkillChainSystem.cpp` off / `_LIVE`:
  `exit 0 errors 0` each (the new `Always.h` guard passes with `_LIVE`).
- `cl /P` off / `_LIVE`: `GetUpgradeSwappedSkillID( static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ) )` 0 / 1, `RNSI_A_ALD_CONVERSION_HEALTH_ADD_TRANS ==` 0 / 1,
  `ST_SPECIAL_ACTIVE_GRADE_3:` 0 / 1, `GetTranscendenceMemoSkillID( eSkillID_ )` 0 / 1, `ASI_A_ALD_CONVERSION_MANA_CreateState( normalStateData` 1 / 2;
  `GetUpgradedSkillID( ptrComboAndSkillChainInfo` 0 / 1.
- `git diff --patience --numstat`: `X2GUAra.cpp` 77 / 0, `X2ComboAndSkillChainSystem.cpp` 18 / 0, `X2SkillTree.h` 6 / 0, `Always.h` 9 / 0; bytepatch: high bytes
  unchanged (25962 / 204 / 23310 / 38738), EOLs kept.
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_ARA_CORE --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2ComboAndSkillChainSystem.cpp KTDXLIB/Always.h`:
  `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 1 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
  RESULT FAIL only for the `//#define`; the ERROR is `Always.h` (an earlier `#if` the tool cannot evaluate, as CH1m), a pure 9-line insertion.
- `flagcheck SERV_IRUHADEV_JUNE_ENUM_NAMES --worktree --files X2Lib/X2SkillTree.h`: `files: 0 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`.
- `junediverge.py --check X2Lib/X2GUAra.cpp X2Lib/X2ComboAndSkillChainSystem.cpp X2Lib/X2SkillTree.h KTDXLIB/Always.h`: `RESULT: PASS`. `enumscan.py --check`
  (`X2GUAra.cpp`, `X2ComboAndSkillChainSystem.cpp`): `0 A/B row(s)`.
- Checklist marked `(Ara: CH1n)`: L263 (no check here), L329 (memo exit), L440 (head), L444 (both stores dropped), L738 (types 14-16). Funcmap rows `Ara CH1n` (3).
  No JUNE_BUGS row. Full build skipped: no play-test.

- After the commit ( `044fcdd5` ): `flagcheck JUNE_CHAR_ARA_LIVE --with JUNE_CHAR_ARA_CORE --range 044fcdd5^..044fcdd5 --files` ( the three files ): `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 1 ERROR`,
  same zeros, `RESULT: FAIL` only for the `//#define` ( ERROR = `Always.h`, as above ); `JUNE_ENUM_NAMES --range 044fcdd5^..044fcdd5 --files X2Lib/X2SkillTree.h`: `files: 0 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`.

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-ara CH1n review` (Opus 5.5 high).

### CH1n - Review (2026-11-11): pass, no code change

Fresh Ghidra decompiles of `32bd09d0`, `32bd34c0`, `32d3ed30` and `32babb50`, read against `044fcdd5` and the exe bytes (`read_va`, `disasm_range.py`):
- **Slot head.** `(int)slot < 0 || 4 < slot` -> exit; PE tables `334fea50` = 50 / 52 / 54 / 56 / 58, `334fea64` = 51 ... 59, `334fea78` = 60 ... 68,
  `334fea8c` = 61 ... 69 = `_CONST_GUUSER_` (normal A, hyper A, normal B, hyper B, as the port indexes them). `31fd2af0` before `Init()` dropped (7i);
  hyper word `+0x10 = 0x101` = `m_bHyperState` + the slot byte; every `+0x11 = 1` store (head, memo arm, case arms, `@32bd2778`) = `IsSkillSlotState`.
  `31fca8e0( id, 1 )` = `GetEquippedActiveSkillTemplet( id, true )` (`X2GUUser.h:2704`, the `bUpgradeSwap_` overload).
- **Memo exit `@32bd0af5`.** `31fdbcc0( id )` -> memo; `memo != 0 && 32206450( memo, 1 )` (`GetSkillTemplet`, callers always pass 1) ; the compare is on
  the memo (`0x6a56` = 27222, `0x6a6c` = 27244 = June `Enum.src.lua` l.16211 / l.14942); other memo -> `@32bd2778` (two `+0x11` stores) -> return. `32babb50`
  = `Init()`, table `ASI_A_ALD_CONVERSION_MANA`, +0x14 / +0x34 / +0x44 = thunks to `32b932b0` Init / `32baa680` FrameMove / `32b9da00` EventProcess =
  March's `ASI_A_ALD_CONVERSION_MANA_CreateState` (`X2GUAra.cpp:21067`; its `stateID` store repeats the caller's).
- **Types.** `SKILL_TYPE` 5 `ST_BUFF`, 6 / 7 / 13-16 active (13 = `ST_HYPER_ACTIVE_SKILL`, `FINALITY_SKILL_SYSTEM` defined), 12 `ST_RELATIONSHIP_SKILL` +
  raw id 900 (`SI_ETC_WS_COMMON_LOVE`, live value 900 under `UPGRADE_SKILL_SYSTEM_2013`; June `Enum.src.lua` l.16050), the rest return. Thunks:
  `329ccd90` -> `32a77a20` -> vslot 0x5f0; `329cba40` -> `31fcbd60`; `329ccda0` -> `329cc0d0` -> vslot 0x5f4; `329ccdb0` -> `329cc170` -> vslot 0x5f8.
  Strings `ASI_COMMON_BUFF` (both slots) and `ASI_THROW_ITEM` (both). Both switches take `param_2` raw.
- **Hyper split (CH3d).** 0x6bd1 / 0x6bd4 = 27601 / 27604 `RNSI_HA_AYR_ENERGY_CANNON(_UPGRADE)` -> `32baab30`; 0x6bd2 / 0x6bd5 = 27602 / 27605
  `RNSI_HA_ASR_NINE_TAILED_FOX(_UPGRADE)` -> `32baace0`; 0x6bd3 and `== 0x6bd0` -> Thousands Blossoms inline: `CH3d.md` amendment matches. Default
  `@32bd271e` = `32baf390` + END 0xc1 = March's `default:` on `SI_A_AM_WIND_WEDGE` (`X2GUAra.cpp:3436`).
- **`32bd34c0` head.** `+0x867c` null -> exit; user tree = unit +0x494; `328905b0( id )` then `32890720( swapped, 1 )`; null -> exit; switch on `param_2`.
- **`32d3ed30`.** Per entry: null skip; `match = id == +0xc`, else (g_pData and its +0x50 tree present) `id == 32205c00( +0xc )`; then `+0x10 == 2`
  (`CASCT_SKILL`) && match -> return: the port's order. Callers: `32024d6d` in `32024b00` and 32 sites inside `32bd34c0` (one per chain-carrying case;
  the Done note's "56 case bodies" are the states, not the lookups). March's callers: `X2GUUser.cpp` 1, `X2GUAra.cpp` 20: no other class reaches it.
- **Callbacks.** `ch1n_triage.py` rerun: `targets 174 rows 174 missing 0`, output `cmp`-identical to the committed `ch1n_user_callback_triage.tsv`.
- Nits kept: the throw comment cites the thunk's target (`329cc0d0`) where the buff one cites the thunk (`329ccd90`); both are correct.

**Checks.**
- One-file probes (`census.cl_cmd`, `/c /Zs /EHsc`): `X2GUAra.cpp` off / `_LIVE` / `_LIVE` + `_FP` + `_SPECTER`, `X2ComboAndSkillChainSystem.cpp` off / `_LIVE`:
  `exit 0 errors 0` each.
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_ARA_CORE --range 044fcdd5^..044fcdd5 --files` (the three files): `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 1 ERROR`,
  `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only for the
  `//#define` (ERROR = `Always.h` l.12217, an `#if` the tool cannot evaluate; a pure 9-line insertion).
- `flagcheck SERV_IRUHADEV_JUNE_ENUM_NAMES --range 044fcdd5^..044fcdd5 --files X2Lib/X2SkillTree.h`: `files: 0 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`.
- `junediverge.py --check-all`: `allow-listed: 202`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2ComboAndSkillChainSystem.cpp`: `0 A/B row(s)`.
- No `_DEBUG` define to retire. Full build skipped: no play-test.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1o` (Opus 5.5 high).

### CH1o - Done (2026-11-11)

**Landed** (flag `_LIVE`, still `//#define`; `_CORE` for the two new helpers; 12 funcmap rows `Char Ara CH1o`; `JUNE_BUGS` 601; checklist L286
`(Ara: CH1o)`; divergent row `GetStateIDBySecretSkillTriggerStateID`). Files: `X2GUAra.cpp` / `.h`, `X2GUUser.h`, `X2GameUnit.h`, `X2GUNPC.h` / `.cpp`,
`X2DamageManager.h` / `.cpp`. Every block carries `char-ara CH1o`. Read from the exe bytes (`charport_adis.py ara`, `disasm_range.py`, `vtslot.py`, a
`[r+slot]`-then-`call r` sweep over `dispsweep 2fc 6b4 74c`, an E8 / E9 xref scan).
- **`SetSkillLevelStateData` = June `3201f850`** inside March's class copy under `_LIVE` (Rena's CH1o form): null templet / unit return,
  `max( 1, GetSkillLevel( id, false, 0, true ) )`, power rate, id, x `( GetSkillAdditionalPowerRate( id ) + 1.f )`. Ara calls `3201f850` directly (no
  0x7a8 override); CH1i's 41 `SetEquippedSkillLevel` blocks and CH1n's head reach it through `SetEquippedSkillLevelStateData`.
- **Vslot 0x6b4 is March's `GetStateIDBySecretSkillTriggerStateID` (corrects CH1i).** Its only June caller `32d3f140 @32d3f1e3` = March's
  `CX2ComboAndSkillChainSystem::GetTriggerSecretSkillState` (future state = ( `+0x7cb0` - `+0x7cec` ) / 100, trigger id `+0x18`, `CanTriggerToSecretSkill`
  `32d3ff20`, the 100.0 / anim-time tests, returns `+0x14`; caller `CommonEventProcess @31fe7a46`): no code there. Ara already overrides the March virtual
  (`X2GUAra.cpp:23641`), so CH1i's body moved inside it under `_LIVE` (March's two `SI_` cases in `#else`) and CH1i's `_CORE` duplicate
  `ReplaceStateIDBySkill` is gone. Divergent row added in this commit.
- **Vslot 0x2fc: June calls it from `DamageData`'s destructor.** `~DamageData` `31e9bb70` (38 callers) opens with `32610d50 @31e9bba5`: attacker handle
  `+0xc` -> unit vslot 0x2fc( this ). Base `3208ddc0` empty (`CX2GameUnit`); the NPC vtable `334ba584` (+ `334b8380` / `8800` / `8c80`) + 0x2fc =
  `3275a410`, which hands it to the owner (`+0x8d5c`, any unit type). Ported: `CX2GameUnit::DoProcessWhenDamageDataDestroyed` (empty, `_LIVE`),
  `CX2GUNPC`'s owner forwarder, `DamageData::NotifyAttackerWhenDestroyed()` (declared in the struct, defined in `X2DamageManager.cpp`, called first in
  `~DamageData`), and CH1h's `MarkSkillFPGainHitByDamage` renamed to the virtual's name (still `_CORE`, now `virtual`). Teardown: a unit's own
  `DamageData` members die while its observable lives; base-level members dispatch to the empty base, Ara has none, and the NPC's `m_ReflectDamageData`
  (`X2GUNPC.h:3652`) dies after `m_optrOwnerGameUnit` (`:4551`) has nulled its handle, so the forwarder finds no owner.
- **Vslot 0x74c is asked of the effect's owner, not the defender (corrects CH1i).** `EffectToUnit @3262efa4-3262efd7`, right after the hit stores
  impact point / defender / `reActResult`: owner `31f8b560` a user and CEffect `+0x7ac` (= its `DamageData` `+0x5ac` `m_iDamageTriggerCondition`) > -1 ->
  owner vslot 0x74c( condition ); false -> next unit. `vtslot.py 0x74c CX2GU`: every user class keeps base `32ec5990` (`mov al,1`) except Ara, whose
  `32b956d0` also returns true, so the gate never fires in June. Wired anyway under `_LIVE`: base `CX2GUUser::IsDamageTriggerConditionMet` (true), Ara's
  made `virtual`, the test after `X2DamageManager.cpp:2998` with a NULL-owner test June lacks. The C5 loader's "no reader" log (`:864`) is now stale; left.
- **`32bab110` `TakeFPGainByHitID` / `32bab6b0` `TakeSkillFPGainByHit`**: new `_CORE` on the FP-gain records (CH1h / CH1k types). Their only callers are
  CH1q's `32bab5c0` / `32bab780`. `32bab6b0` also sets entry `+0` on every matching attack serial before testing ready; its walk keeps going on a hit
  id of 0 (**JUNE_BUGS 601**, as 597).
- **`32b92fa0` / `32b92fb0` = March's `GetBlockingRate` / `GetTurnigRate`** (`+0x9650` / `+0x9654`), called by `CX2GUUser::DamageReact @320474ec` /
  `@3204756c` in the Quicken Guard block (UT_ARA, class 118), = March `X2GUUser.cpp:10310` / `:10335`: no code.
- **No code:** 0x28 / 0x30 (written in CH1h); 0xd0 `CreateGageData` `32bc19a0` is CH1q's (class map; `ch0_fp.tsv`'s "CH1q3" means CH1q); 0x670
  `IsInJobLine` stays a plain member (Chung / Elsword CH1o: only class code calls it); `31d29310` stale (no live caller).

**Routed (scope rule).** CH4a (amendment line written): June's Quicken Guard gate `@32047441` is `attackType != AT_SPECIAL` (AT_COMMAND passes),
March's `X2GUUser.cpp:10297` `== AT_NORMAL`.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_ARA_CORE --worktree --files` the 8: `files: 8 PASS, 0 PASS~, 0 DEFINE, 0 OTHER,
  0 FAIL, 0 ERROR`; `38 deleted line(s)` (all CH1h / CH1i `_CORE` lines: the 33-line `ReplaceStateIDBySkill`, its declaration, the renamed declaration
  and definition head, one corrected comment), 0 non-ASCII, 0 EOL-mismatched, 0 `#else/#endif` without the token, 0 encoding changes; RESULT FAIL only
  for `define: NOT FOUND` (`//#define`). numstat: X2GUAra.cpp 115 / 35, .h 4 / 3, X2DamageManager.cpp 15 / 0, .h 6 / 0, X2GUNPC.cpp 9 / 0, .h 3 / 0,
  X2GUUser.h 3 / 0, X2GameUnit.h 3 / 0.
- `junediverge.py --check` the 8: `allow-listed: 1`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2DamageManager.cpp X2Lib/X2GUNPC.cpp`:
  `0 A/B row(s)`.
- One-file probes (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH) of X2GUAra / X2DamageManager / X2GUNPC / X2GUUser `.cpp`, `_LIVE` off and on: exit 0,
  0 errors x8; X2GUAra.cpp with `_LIVE` + `_FP` + `_SPECTER`: exit 0, 0 errors.
- After the commit (`238ddfcc`), the same flagcheck `--range 238ddfcc^..238ddfcc --files` the 8: `files: 8 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `38 deleted line(s)`, 0 non-ASCII, 0 EOL-mismatched, 0 `#else/#endif` without the token, 0 encoding changes; RESULT FAIL only for the `//#define`.
- No `_DEBUG` define. Full build skipped: no play-test.

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-ara CH1o review` (Opus 5.5 high).

### CH1o - Review (2026-11-11): pass, no code change

Read from the exe bytes (`disasm_range.py`, a linear sweep of `EffectToUnit` from `3262b6e0`, `vtslot.py`, `dispsweep.py`):
- **`SetSkillLevelStateData`** = `3201f850`: templet / `+0x867c` null return, `32890620( id, 0, 0, 1 )` floored at 1, level byte `+4`, power rate
  `32071720` -> `+8`, id `+0xc`, `( 3201ad10( id ) + 1.0 ) * +8`. The C++ matches; Ara's 0x7a8 is not overridden.
- **0x6b4.** Ara 33505314 + 0x6b4 = `32ba4620`. `32d3f140 @32d3f1c4-32d3f1e3`: ( state ) / 100 (magic `imul`, `sar 5`) -> `&state` -> vslot 0x6b4 = March
  `X2ComboAndSkillChainSystem.cpp:159`. `32ba4620`: `+0x867c` null return, `32890140` slot skill, `32207210` base, 0 return, `- 27014`, `> 0x19` out; byte
  table `[0,4,4,4,4,4,1,4,4,4,2,4,3,4,...,4,0]` -> cases 0 / 25 = 0x106 (262), 6 = 0x10f (271), 10 = 0x12d (301), 12 = 0x137 (311) = the four `case`
  groups (27014 / 27039, 27020, 27024, 27026). The moved body is the deleted `ReplaceStateIDBySkill` with the dereference dropped; no name is left in code;
  divergent row 451 holds.
- **0x2fc.** `31e9bb70` is `~DamageData` (EH frame, state 0x13, `call 32610d50 @31e9bba5` first). `32610d50`: observer `+0xc` (= `optrAttackerGameUnit`,
  4th member) -> `-0x1a0` -> vslot 0x2fc( this ), `ret` (no args). `vtslot 0x2fc`: `CX2GameUnit` / `CX2GUUser` = `3208ddc0`; `CX2GUNPC` 334ba584 and every
  NPC subclass = `3275a410` (owner observer `+0x8d5c`, tail `jmp`); Ara = `32bab1d0`. `[r+0x2fc]` then `call` / `jmp r` over all 271 displacement hits:
  exactly `32610d79`, `32610d89`, `3275a43a`; no `call [r+0x2fc]` form. `GetOwnerGameUnit()` (either `X2OPTIMIZE_UNITTYPE_BUG_FIX` form) returns the
  observable, so the upcast keeps the pointer. **Teardown: agreed.** Ara has no `DamageData` member; `CX2GUUser` / `CX2GameUnit` members die under their
  own vptr (empty base); the NPC owner observer dies first. `MarkSkillFPGainHit` only sets `m_bHit`, so a March-only `DamageData` copy can mark a hit
  earlier than June but never twice: a CH1q watch item once CH4a fills the table.
- **Other 0x2fc overrides:** Rose `32aabaa0` and Lu/Ciel `32b17680` also override it. Their ports override the new `CX2GameUnit` virtual (note for
  char-luciel CH1o and the Rose port, no change here).
- **0x74c.** `3262ef49-3262ef9e` stores impact point, `+0x470` defender (`31e56b80`), `+0x28 = +0x24`, then `31f8b560` (CEffect owner `+0x1dc`)
  `[+0x264] == 0`, `[edi+0x7ac] > -1`, owner vslot 0x74c( `+0x7ac` ), `je 3262f54e` = `inc esi; jmp 3262b78a` (the loop's next unit = `continue`), then
  `cmp [edi+0xb11]` = March's `GetIsGroupDamage` block next. `GetDamageData()` returns a reference (`X2DamageEffect.h:1022`), so no extra `~DamageData`. The
  NULL-owner test is the one addition (June dereferences). `vtslot 0x74c CX2GU`: only Ara (`32b956d0`) leaves `32ec5990`.
- **`32bab110`:** `<= -1` -> 0; entry `+8 == id` -> gain = record word `+0`, `+8 = -1`, walk on (last match wins) = the C++. **`32bab6b0`:** `31edf580(
  record +4, arg1 )`; entry `+8 >= 0` and `+4 == arg2` -> `+0 = 1` (the `cmp [eax+1]` sits before the store, flags kept) then `+1 == 1` -> gain, id, inner
  break; outer `test ebp,ebp; jg` stop; `> -1` second loop clears the id; gain in the `[esp+0xc]` slot. The C++ matches; JUNE_BUGS 601 holds.
- **`32b92fa0` / `32b92fb0`:** `fld [ecx+0x9650]` / `+0x9654`; direct calls `@320474ec` / `@3204756c` = March `X2GUUser.cpp:10310` / `:10335`; the CH4a
  amendment (gate `@32047441`) is written.

**Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_ARA_CORE --range 238ddfcc^..238ddfcc --files` the 8: `files: 8 PASS,
0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `38 deleted line(s)` (X2GUAra.cpp / .h only, explained in the Done note), 0 non-ASCII, 0 EOL-mismatched, 0
`#else/#endif` without the token, 0 encoding changes; RESULT FAIL only for `define: NOT FOUND` (`//#define`). `junediverge.py --check-all`: `allow-listed:
202`, `RESULT: PASS`. `enumscan.py --check X2GUAra.cpp X2DamageManager.cpp X2GUNPC.cpp`: `0 A/B row(s)`. Probes (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH):
X2GUAra.cpp off / `_LIVE` / `_LIVE` + `_FP` + `_SPECTER`, X2DamageManager / X2GUNPC / X2GUUser `.cpp` off / `_LIVE`: exit 0, 0 errors x9. No `_DEBUG`
define. Full build skipped: no play-test.

**Nothing to play-test.** CH1p kickoff checked (its review kickoff is written by CH1p). Next: `/x2-phase char-ara CH1p` (Opus 5.5 high).

### CH1p - Done (2026-11-11)

**Three readers moved to June's pressed index, one `CX2GUUser` function replaced, no new flag.** All under `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (still `//#define`),
March's code in `#else`; every block tagged `// char-ara CH1p`. Read from the exe bytes (`funcdis.py`, `dispsweep.py 8ac0 8b58 88bc 8ea0 8ea4 8ea8` over
`32b90b70-32bd86e0`) and `cl /P` of both files, `_LIVE` off / on.

**7ag, the pressed-index space.**
- **Class side: June's already (CH1k).** `CanUseSpecialAttack`'s lookup decodes `>= 5` / `- 5` and stores `m_iChosenSkillSlotIndex` (`+0x8ea4`, `@32bd0812`);
  `SpecialAttackCooldown` marks the slot 0-9 (`@32bb201a`); `m_iNowSpecialAttack = slot + 1` (`+0x88bc`, `@32b90cf2`). March's 4 + 4 decode and `CountUsedSkill`
  sit only in `#else` (`_LIVE` `.i`: `iPressedSkillSlotIndex > 3` 0 / off 4, `>= 5` 1 / 0). June never touches `+0x8ac0` in her range. The stale "until CH1p"
  comment in the lookup now says A 0-4 / B 5-9 (the one deleted line).
- **Shared reader.** `CommonSpecialAttackEventProcess` (June `31fcadd0`): `UT_ARA` joins the June set (`X2GUUser.cpp`, a new outer `_LIVE` level over Rena's chain;
  stand-in comment now names Elesis, Add). `.i`: `UT_ARA != ...` 1 with `_LIVE`, 0 off.
- **`GetPressedSkillState` replaced** (divergent row, June `31fd3360`): June's body inside March's: hyper test, then `> 4` / `- 5` over the four tables
  (`33472040` normal A 50-58, `33472054` hyper A, `33472068` normal B 60-68, `3347207c` hyper B; read from `.rdata`). Its only callers are Ara's lookup (June body)
  and her March body in `#else`. The CH0 allow row for `CX2GUAra::CanUseSpecialAttack` is dropped; the divergent row says "ported under ...". Guard added in
  `Always.h`: `_LIVE` needs `SERV_IRUHADEV_JUNE_SKILL_SLOT_10` (5-entry tables; the `_LIVE` probe proves it is defined there).
- **`CurrentSkillKeyManager` (March 4 + 4) fed June's 0-9: fixed for Energy Void.** June's `+0x8b58` is `CX2GUUser::m_kSkillSlotKey` (Eve CH1j20). Energy Void
  START StartFuture is June's whole body (`32b93bc0`: `Set( m_iNowSpecialAttack - 1 )` `31fce6d0`, then `CommonStateStartFuture`); the LOOP EventProcess takes
  June's two key calls (`FollowSlotSwap` `@32b9e82a`, `IsKeyReleased` `@32b9e85e`). Its charge test (June `>= 5` / `>= 10`, March `3 <`) stays for **CH3b**
  (kickoff amended). June's other `+0x8b58` users are new states: Energy Wave `32b93540` / `32b9dd20` (**CH2c**), Specter of Storm `32ba7c20` / `32bc9c70`
  (**CH2d**) and its DEGREE pair `32ba5b50` / `32ba5e90` (**CH2d2**); kickoffs amended. `32b93590` is dead (CH0).
- Checklist 7ag gets an Ara paragraph.

**Hooks 7h / 7l / 7o / 7p / 7x.** 7h `InitWaitStateChange( "ASI_WAIT_STATE_CHANGE" )` (`X2GUAra.cpp:576`) and 7l `InitBoundingStates( "ASI_" )` (`:1021`) are
CH1m's (rows marked). 7o `SET_LIMIT_MAX_G_SPEED` (June script x1; reader `X2GUUser.cpp:16566`), 7p `TIME_STOP_IMMUNE_STATE` (x3; `:18203`), 7x `AT_COMMAND` x36 /
`AT_SPECIAL` x8 are script data that `Ara_Martial_Artist.lua` brings in CH1 final; their readers are done.

**The script keys CH0 gave this phase** (`ch0_keys.tsv`: all five are "both", March reads them too). `USE_FORCE_POWER` / `ADD_FORCE_POWER` / `FP_CANCEL`: loaded by
CH1j inside `CommonStateStart` (`32bd2a40`); consumers are CH1q's. `BERSERK` x40 / `USE_BERSERK_SOUND`: the engine's `SOUND_SET` sub-table (C3t-2, done); Ara's
inputs `IsFullHyperMode` (0x9c) and `IsHyperModeState` (`32bd85f0`) are CH1q's. `APPLY_MEMO` / `EFFECT_SET_LIST_EX_MEMO` x2: only in the unregistered
`ASI_SA_ALS_SPECTER_OF_BLADE` / `_FINISH` tables (CH0, june-dead-data), n/a. §5 rows marked.

**Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUUser.cpp`: `X2GUAra.cpp` PASS +13 -1, `X2GUUser.cpp` PASS +23;
`files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `1 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token,
0 encoding change(s)`; RESULT FAIL only for `define: NOT FOUND` (`//#define`). After the commit (`e291386c`), `--range e291386c^..e291386c` with the
same `--files`: the same tallies (`2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `1 deleted line(s)`, 0 / 0 / 0 / 0), FAIL only for the `//#define`.
`Always.h`: +9, a pure insertion (guard only).
`junediverge.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2GUUser.cpp`: `allow-listed: 7`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp
X2Lib/X2GUUser.cpp`: `0 A/B row(s)`. One-file `/c /Zs /EHsc` probes (`census.cl_cmd`): `X2GUAra.cpp` and `X2GUUser.cpp` off, `_LIVE`, `_LIVE` + `_FP` + `_SPECTER`
+ `_SKILL`: exit 0, 0 errors, 38 / 39 warnings each (unchanged). Full build skipped: no play-test. No `_DEBUG` define. Funcmap: `31fd3360` PORTED, `32b93bc0`
PORTED, `32b9e820` routed (`Char Ara CH1p`). No `JUNE_BUGS` row.

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-ara CH1p review` (Opus 5.5 high).

### CH1p - Review (2026-11-11): pass, no code change

Read from the exe bytes (`funcdis.py --func`, `dispsweep.py`, the `.rdata` words through the PE section table) and the `.i` of both files, `_LIVE` off / on:
- **`31fd3360`:** gauge `+0xc04` null -> normal; else `fldz; fcomp [+0xe8 protected]; test ah,1; jne` = 0 < remaining time -> hyper (NaN also hyper, as
  `<= 0.f` false in C++). `cmp 4; jle`: normal A `[i*4 + 33472040]`, normal B `[i*4 + 33472054]` (- 5 folded = `33472068`), hyper A `33472054`, hyper B
  `[i*4 + 33472068]` (= `3347207c`); `movzx word`, `* 0x64 + [+0x7cec]` = `m_StateList[]`. `.rdata`: 50-58 / 51-59 / 60-68 / 61-69 step 2 = the
  `_CONST_GUUSER_` tables under `JUNE_SKILL_SLOT_10` (`X2GUUser.h:5400-5420`, `1, H1, ..., 5, H5, SLOTB_1` layout). `GetRemainHyperModeTime()` returns 0
  with no gauge (`X2GameUnit.h:2333`). The C++ matches. Sibling `31fd32e0` (StateChange by `+0x8ea4`, same tables) is the earlier port at `X2GUUser.cpp:44741`.
- **Shared reader:** `guuser_live.i` `bMarchSlotIndex` ends `&& CX2Unit::UT_ARA != ...`; `guuser_off.i` has no `UT_ARA`. Every earlier class `_LIVE`
  (Arme, Eve, Chung, Elsword, Raven, Rena) is `#define`d, so the new outer level drops no class.
- **Sweep** (`dispsweep 8ac0 8b58 88bc 8ea0 8ea4 8ea8`, `32b90b70-32bd86e0`): `+0x8ac0` none; `+0x8b58` = `32b93540` / `32b9dd20` (CH2c), `32b93590`
  (dead, `ch0_class_map.tsv`), `32b93bc0` / `32b9e820` (here), `32ba5b50` / `32ba5e90` (CH2d2), `32ba7c20` / `32bc9c70` (CH2d); `+0x88bc` = `32b90c20`
  (CH1k write) + the three StartFutures; `+0x8ea4` = `32b90c20`, `32bb1ff0`, `32bcfaa0` (CH1k). `+0x8ea0` hits are the chosen skill, not the index. `ara_live.i`:
  `iPressedSkillSlotIndex > 3` 0 (off 4), `- 4 ]` 0 (off 2); `CurrentSkillKeyManager` / `SAKP_` left only as the header enum, struct and member: no 4 + 4 user.
- **Energy Void:** `32b93bc0` = `Set( [+0x88bc] - 1 )` on `+0x8b58` (`31fce6d0`), tail `jmp` vslot 0x7c0 = the C++. `32b9e820`: `FollowSlotSwap @32b9e82a`,
  `IsKeyReleased( +0x86da = m_InputData ) @32b9e85e` after the charge test (same short-circuit order); `fcomp [33487220]` = 0.4 (double of 0.4f),
  `+0x96c8 >= 5` / `>= 10` (`ENERGY_VOID_MAX_CHARGE_VALUE` 10), else 0x113 LOOP, `+0x96d0` -> 0x114 END: the CH3b amendment's numbers hold.
- **Keys:** `USE_FORCE_POWER` / `ADD_FORCE_POWER` / `FP_CANCEL` in CH1j's `CommonStateStart` (`X2GUAra.cpp:5377-5396`), `USE_BERSERK_SOUND` / `"BERSERK"`
  in the `SOUND_SET` reader (`X2GUUser.cpp:17793`), Specter of Blade tables dead (`ch0_keys.tsv`). Agreed.

**Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --range e291386c^..e291386c --files X2Lib/X2GUAra.cpp X2Lib/X2GUUser.cpp`: `files: 2 PASS, 0 PASS~,
0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `1 deleted line(s)` (the stale comment), 0 non-ASCII, 0 EOL-mismatched, 0 `#else/#endif` without the token, 0 encoding
changes; RESULT FAIL only for `define: NOT FOUND` (`//#define`). `junediverge.py --check-all`: `allow-listed: 202`, `RESULT: PASS`. `enumscan.py --check
X2GUAra.cpp X2GUUser.cpp`: `0 A/B row(s)`. Probes (`census.cl_cmd`, `/c /Zs /EHsc`): both files off / `_LIVE` / `_LIVE` + `_FP` + `_SPECTER` + `_SKILL`: exit 0,
0 errors x6 (38 / 39 warnings, unchanged). No `_DEBUG` define. Full build skipped: no play-test.

**Nothing to play-test.** CH1q kickoff checked. Next: `/x2-phase char-ara CH1q` (Opus 5.5 high).

### CH1q - Done (2026-11-11)

**Landed** (flag `SERV_IRUHADEV_JUNE_CHAR_ARA_FP`, still `//#define`: it needs `_LIVE`; `SERV_IRUHADEV_JUNE_CHAR_ARA_FP_DEBUG` defined for the `[JUNE_CHAR] ARA FP`
lines; every block tagged `char-ara CH1q`; 31 funcmap rows `Char Ara CH1q`; per-row verdicts `june2017_re/char_ara/ch1q_fp_outcome.tsv`). Files: `X2GUAra.cpp` / `.h`,
`X2GageData.cpp`, `X2GageUI.cpp` / `.h`, `X2SkillTree.h`, `KTDXLIB/Always.h` (CH1q block: `_FP_DEBUG` and the `_FP` guard). No split: CH1q3 not needed.
- **June bodies inside March's functions (March in `#else`):** `IsFullHyperMode` `32ba6cc0` (buff hyper mode active -> buff hyper count > 0, else the gage
  byte), `SetFullHyperMode` `32b96a00` (+ hit count 0), `ApplyHyperModeBuff` `32bab210`, `HyperModeBuffEffectStart` / `End` `32bb22d0` / `32bb2740` (tail
  equips `+0x95e8` with the promotion list; view list again while the buff rate is on and count < 3; a transcended unit or an awakening set skips only the
  boosts; the end clears the buff hyper count and calls the base last), `UseForcePower` `32bb2b40` (n - 1 for an FP cancel into 27201 / 27205 / 27211 / 27217 /
  27221; berserk MP per force power; 27417 Hell Ghost count + buff erase; 27419 heal in hyper time and its keep-the-force-power roll; `ConsumeForcePower`
  merged in), `SetForcePower` `32ba6ef0` (my unit: spent x 20.0 x `GetCharacterSpecializationB()` -> `UpNowMpWithText`), `DoProcessWhenKillEnemy` `32bc8be0`,
  `ResetForcePowerWhenRebirthInPVPGame` `32ba6f80` (`+0x9611`), `UpNowForcePowerHitOthers` `32baad30`, `GetAvailableFPCancel` `32b95c30` (June ids, .cpp body).
- **New under `_FP`:** `UpForcePowerByHyperModeBuffCount` `32baae50` (list `[ last - now hyper count - 1 ]`, over-max x max MP x `+0x9730`),
  `ChangeMaxCharacterAddResource` `32baafc0`, `UpForcePowerByReadySkillGain` `32bab5c0`, `UpForcePowerBySkillHit` `32bab780`, `StateChangeOrFPCancel` `32babba0`
  (no caller yet: the CH2-CH3 skill states), `GainFullHyperKillReward` `32bc5bc0` (max HP x 0.03 + now, force power + 1, the kill effect set),
  `ProcessFullHyperHitReward` `32bc9a60`, `StoreGageSnapshot` / `RestoreGageSnapshot` `32ba4840` / `32ba7b50` (stand-ins: bases not ported, unreferenced, as
  Rena / Chung), `IsHyperModeState` `32bd85f0` (also picks the berserk sound set, checklist l.613). Members `m_fMpGainPerForcePowerInFullHyperMode` (`+0x973c`),
  `m_iFPCancelChainCount` (`+0x9838`), `m_iSnapshotForcePower` (`+0x92bc`), ctor 0. Namespace `_CONST_ARA_CH1Q_` (20.0, 0.03).
- **Dropped under `_FP` (June has none):** `CForcePowerSystem m_ForcePowerSystem` and its reset in `CommonStateStart` (CH1j had kept it), `m_ucBasicForcePower` with
  `Get` / `SetBasicForcePower`, `ConsumeForcePower`.
- **= March:** `UpForcePower` (June inlines it 7 times with the same `+0x9644` conversion; the Open Mind 27417 refresh after it is each caller's, as CH1h2 / CH1j
  wrote), `GetHypetMpdeStateID` `32ba46c0` (0x17 / 0x9b = 155), `CreateGageData` `32bc19a0`, `SetFPCancel` `32b95c70`, `CX2AraMyGageUI::InitWakeOrb`
  `32e5b550` (6 / 4 / 3 / 2 = `PGUWO_ORB_EVE` / `_RAVEN` / `_CHUNG` / `ORB3`), the "same" gauge rows; emulator `CAC_FORCE_POWER` (`X2OfflineStatTable.cpp:306`) = June.
  CH1h's Init / ReInit / InitComponent `_FP` lines and `SetSpecificValueByBuffTempletID` hyper cases checked: they stand.
- **Gauge:** `CX2AraGageData::UpdateDataFromGameUnit` `32a34980` no longer copies the changed byte `+0x111`; `SetMyPlayStatusFromPacket` `32a34a10` sets max 10;
  `CX2AraMyGageUI::UpdateCharacterImage` `32e69060` refreshes only when `+0x111 != +0x110`, then stores `+0x110` into `+0x111`; `GetBerserkModeMyStateImage`
  `32e61a60` takes my unit's transcend value (== 1: `TR_*_1_STATE` on `DLG_UI_Common_Texture102_NEW` for 118 / 119 / 124) and adds 34 / 124.
  **The kickoff's portrait check: no June bug.** The UI reads the manager's gage copy, whose `+0x111` only the UI writes (the copy never takes it), so it holds
  the last shown berserk byte; `SetFullHyperMode` writing both bytes on the unit's own gage does not matter.
- **Enum names** (`X2SkillTree.h`, `JUNE_ENUM_NAMES`): `RNSI_P_ALS_FILLED_POWER` 27410, `RNSI_P_ASR_OPEN_MIND` 27419. SA 59 / 113 / 177-179 are March's
  `SA_HP_GAIN_REL_MAX_HP`, `HP_CONSUME_REL`, `SA_HIT_COUNT_1-3` (`s0a_templet_keys.tsv`).
- **Data (asset rule):** `kom.py stage-june`: June's `DLG_UI_Common_Texture77_NEW.tga` / `.TET` replace March's (data057), `Texture82_NEW` (data057) and
  `Texture102_NEW` (data215) new; backup `kom_backup_stage_june\20261111_104850`. `PACK OK data057`, `PACK OK data215`; `COPY OK data057` (70,521,896 B, 936
  entries, md5 237bf2b54fafd6536f2715c56637190c), `COPY OK data215` (21,506,102 B, 174 entries, md5 06d0000fa45d6b50b11d11b0b39dd474). `MODS.md` entry.
- **Found:** DamageData `+0x5aa` (the hit roll skips a hit with it set) is zeroed by June's DamageData init (@`31f8f340`) and copied, never set: not ported,
  one-line comment. `+0x9718` is Swallow Assault's handle, not the kill reward's (CH3a amended). `+0x973c` / `+0x9744` are written only by
  `InitPassiveSkillState` (CH4a amended). Checklist l.961: Ara's hyper-effect start does not return for a transcended unit (only the boosts are skipped).
- **Divergence rows:** `IsFullHyperMode` / `SetFullHyperMode` (Ara arm noted ported), new `GetAvailableFPCancel` and `CX2AraMyGageUI::GetBerserkModeMyStateImage`
  (both replaced here). `JUNE_BUGS.md` row 602 (the unsigned socket-rate index). Checklist rows l.184, 236, 237, 257, 258, 278, 293, 603, 613, 945, 961, 980
  marked `(Ara: CH1q)`.

**Hygiene.** `flagcheck _FP --worktree --files <6 code files>`: 4 PASS, 2 OTHER (`Always.h`: this phase's block plus a peer's uncommitted
`OFFLINE_JOB_CHANGE_SKILL_PUSH` block; `X2GUAra.cpp`: the `_FP_DEBUG` lines nested in `_FP`), 0 FAIL; 0 deleted, 0 non-ASCII inserted, 0 EOL mismatches; RESULT
FAIL only for the commented-out `//#define`. `flagcheck _FP_DEBUG` (same files): `RESULT: PASS`. `flagcheck JUNE_ENUM_NAMES --files X2SkillTree.h`: `RESULT: PASS`.
`junediverge.py --check` (6 files): allow-listed 5, `RESULT: PASS`. `enumscan.py --check X2GUAra.cpp X2GageUI.cpp X2GageData.cpp`: 0 A/B rows.
Probes (`/c /Zs /EHsc`): off: `X2GUAra.cpp`, `X2GageData.cpp`, `X2GageUI.cpp` 0 errors; `_LIVE`: `X2GUAra.cpp` 0; `_LIVE` + `_FP`: `X2GUAra.cpp`, `X2GageData.cpp`,
`X2GageUI.cpp`, `X2GUUser.cpp`, `X2BuffBehaviorTemplet.cpp` 0 errors (the new `Always.h` guard did not fire: its four flags are defined); `_FP` alone: C1189 at
`Always.h(12218)`. `cl /P` with `_LIVE` + `_FP`: the new bodies present, `m_ForcePowerSystem` / `GetBasicForcePower` / `ConsumeForcePower` 0 in `X2GUAra.i`,
`TR_ASURA_1_STATE` 1 in `X2GageUI.i`. Full build skipped: no play-test.
Range (`flagcheck _FP --range 249e437a^..249e437a --with _FP_DEBUG --files <5 code files>`): 5 PASS, 0 OTHER, 0 FAIL; 0 deleted, 0 non-ASCII, 0 EOL; RESULT FAIL
only for the commented-out `//#define`.

**Routed.** CH2a (callers of `StateChangeOrFPCancel`), CH3a (`+0x9718` rename and writer), CH4a (the `+0x973c` / `+0x9744` writers, the `+0x9838` reader),
CH1 (amended: what `_FP` shows in the play-test), CHNa (gauge creation for 34 / 124, unchanged). Debug define stays until the CH1q review.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1q review` (Opus 5.5 high).

### CH1q - Review (2026-11-11): pass, debug define retired

Read from the exe bytes (`charport_adis.py ara`, the jump tables through `disasm_range.load_text`) against the C++ under `_FP`:
- **`32bb2b40` UseForcePower:** index `32bb2d1c` (base `0x6a41`) picks `32bb2b6c` (`n - 1` when `n > 0`) at 0 / 4 / 10 / 16 / 20 = 27201 / 27205 / 27211 /
  27217 / 27221, every other index `32bb2b7b`: the five `RNSI_*` cases. `fldz; fcomp [+0x973c]; test ah,5; jp` = continue when the rate > 0, then
  `IsFullHyperMode` (0x9c) and `UpNowMp` (0xf0) of `n x rate`. 27417: `GetSkillBuffTempletID( 27417, 0, 1 )` -> `322a0090` byte added to `+0x986c`, erase
  `3229ea50( 27417, 0, 1 )` (vslot 0x238 factor, then erase by its BTI = `EraseBuffTempletFromGameUnit( GetSkillBuffTempletID( ... ) )`). 27419: gauge non-null,
  decoded `+0xe8` > 0 (`GetRemainHyperModeTime` reads the same gauge), `n > 0`; SA 59, x SA 113 when `322cc1a0`; `UpNowHpWithText( max HP x n x rate )`; SA 5
  vs `GetRandomFloat( 0 )`: `fcompp; test ah,0x41; jne` = keep the force power (`n = 0`) when SA 5 > roll. Then `SetForcePower( now - n )`. Matches.
- **`32bc9a60` hit roll:** `dec eax; je` = `AT_SPECIAL`, `+0x24 == 7` = `RT_DUMMY_DAMAGE`, `+0x5aa` (left out, as noted), timer `test ah,0x41; jp` = return when
  > 0, riding; `++` hit count, timer = interval. Filled Power: `GetSkillLevel( 27410, 0, 0, 1 )`, `32206450( id, 1 )` = March's one-argument `GetSkillTemplet`;
  buff count 1 / 2 / else `IsFullHyperMode` -> `0xb1` / `0xb2` / `0xb3`; value > 0 -> count. Index `count - hits - 1` clamped at 0, then `jb` @`32bc9beb` = size - 1
  only when size - 1 < index (unsigned, so an empty list keeps the index: `JUNE_BUGS.md` row 602 stands). Roll `32070350( list[i] x spec A +0x6c50, rand )`. Matches.
- **`32babba0`:** used == chosen: `jns` -> `StateChange( next, 0 )`, else clear. FP cancel: `m_bFPCancel`, chosen != 0, not `31edf580( chosen, 27213 )`;
  `0x7fc( chosen, 1 )`: `32ba6610` never reads its second argument (`[esp+0x34]` @`32ba67bf` is the first after a push), so March's one-argument call is right.
  `vslot 0x524` before `inc [+0x9838]`, `jns` from the `inc`. Matches.
- **`32bb22d0` / `32bb2740`:** start: 0x9c, 0x63c( 1 ), 0x634, `+0x41f8`, promotion, tail `+0x95e8`, then `+0x9744 > 0` and decoded `+0x6844 < 3` -> view list
  `+0x8834`; else unit `+0x3c != 0` or `31fd66b0` skips only the four boosts, view list always. End: promotion + tail normal, `EffectSet_CHUNG_BERSERK_MODE_START`,
  0xa0( 0 ), `+0x6844 := 0` only when non-zero (the `KProtectedType` changed test), show / collision / weapon, promotion; else view list; `31fd5d70` last. Matches.
- **Gauge:** `32a34980` copies now / max, `+0x110`, `+0x112`, not `+0x111`. `32e69060`: dialog `+0x10`, `+0x111 != +0x110`, picture 0, berserk and game ->
  `32e61a60( class, g_pData +0x68 +0x90 +0x3c )` else `31ee0de0( CIT 7, same )` (= `IsMyGageTranscended`), SetTex, size, `+0x111 := +0x110`. `32e61a60`
  tables `32e61bd8` / `32e61c70`: transcend 1: 7 / 28 / 31 / 34 as untranscended, 118 / 119 / 124 the `TR_*_1_STATE` pieces; else 118 Texture70, 119
  Texture77, 124 Texture82. Matches. `kom.py status`: data057 / data215 in sync with live; all 7 pieces are in the staged `.TET` files (UTF-16).
- **Equal rows:** `32ba46c0` = `< 3 ? 0x17 : 0x9b`; `32bc5bc0`: game + effect set, `max x 0.03 + now` (`3344f248` = 0.03f widened), `+0x9644` conversion = `UpForcePower`.

**Hygiene.** Range (`flagcheck _FP --range 249e437a^..249e437a --with _FP_DEBUG --files <5 code files>`): 5 PASS, 0 OTHER, 0 FAIL; 0 deleted, 0 non-ASCII,
0 EOL; RESULT FAIL only for the commented-out `//#define`. `junediverge.py --check-all`: allow-listed 202, `RESULT: PASS`. `enumscan.py --check X2GUAra.cpp
X2GageUI.cpp X2GageData.cpp`: 0 A/B rows (D 1, X 1 ignored). Probes (`/c /Zs /EHsc`): off, `_LIVE` + `_FP`, `_LIVE` + `_FP` + `_SPECTER`: `X2GUAra.cpp`,
`X2GageData.cpp`, `X2GageUI.cpp` 0 errors each (9 runs). Debug retired: `//#define SERV_IRUHADEV_JUNE_CHAR_ARA_FP_DEBUG` (`bytepatch.py`); after it `X2GUAra.cpp`
off and `_LIVE` + `_FP` 0 errors; `flagcheck _FP_DEBUG --worktree --files Always.h`: 1 OTHER (a peer's uncommitted block, not committed here), 0 deleted
beyond the retired line, RESULT FAIL only for the now-commented define. Full build skipped: no play-test.

**Kickoff:** `CH1q2.md`'s read list named Rena's CH1q sections (template leftover); it now names this phase's Done and Review.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1q2` (Opus 5.5 high).

### CH1q2 - Done (2026-11-11)

**Landed** (every block tagged `char-ara CH1q2`; all three flags `//#define` until CH1 final; no `_SPECTER_DEBUG`: nothing runs before CH1, and a review
would retire it unrun). Files: `KTDXLIB/Always.h`, `X2GameUnit.h`, `X2GUUser.h` / `.cpp`, `X2GUAra.h` / `.cpp`. 4 funcmap rows `Char Ara CH1q2`.
- **`SERV_IRUHADEV_JUNE_CHAR_SPECTER_STATE` (new, class-agnostic):** `StateParam::bSpecterState` (June StateParam +0x95, right after `bAttackState` +0x94;
  June's +0x96 is `USE_CHANGED_DISPARATE_FORCE`, +0x97 `bNoChangSpectro`), copied in `operator=`, false in `Init`; loaded false-default from `IS_SPECTER_STATE`
  by `CX2GUUser::CommonStateStartFuture` (`320506f0` @`320523c2`, future copy, after `APPLY_ANIM_MOVE` as June) and `CommonStateStart` (`32057b80` @`3205866c`,
  now copy, after `IS_ATTACK_STATE`).
- **`SERV_IRUHADEV_JUNE_CHAR_CHAIN_ANIM_SPEED` (new, class-agnostic; the CH1j amendment):** `CX2GUUser::m_fNowSkillPassiveRate` (+0x8908, ctor 1.0 @`320425c2`).
  **Correction to CH1j:** +0x8900 is `m_fAnimSpeedFuture`, not a power rate. June 320506f0 @`32051bef`: after `APPLY_ANIM_SPEED_BY_ITEM_ABILITY`'s add, while
  `m_ComboAndSkillChainSystem.GetPtrNowUseChainInfo()` is set, the future anim speed x= the rate and the rate goes back to 1.0, then `ResetAnimSpeed`. Only
  writers (`dispsweep 8908`): the ctor, this reader and Ara's `CommonStateStartFuture` `32bc8380` (CH1j, base last). Ara's stand-in member and its ctor init
  sit in `#else`. CH1j's comment on `GetNowSkillPassiveRate` ("power rate") is left as written (old comments are not rewritten).
- **`_SPECTER` (Ara):** `IsSpecterState()` body = `GetStateparam( false ).bSpecterState` (June inlines vslot 0x128( 0 ) byte +0x95 at both class readers,
  `32bcee1c` Specter of Thunder and `32bd2c74` Necromancy fox: both read the now copy, as CH1h2 / CH1j wrote them). `UpdateEquipCollisionData` `32baf270`
  inside March's: berserk -> base `UpdateNotRenderingOtherEquipList` when `m_fBuffHyperModeDamageRate > 0` and the buff hyper count < 3, else Ara's (vslot
  0x630); collision from a copy of `m_vecPromotionEquipList` plus `m_ptrPromotionEquipTail` (the base skips a null entry); else March's three base calls; the
  three attack boxes off after both.
- **Already ported, checked, no change:** `TAIL_TYPE` load (`LoadPromotionItemID` `32bab890`), tail equips (`InitPromotionEquipList` `32baec10`), tail show
  by count (`32badd50` / `32bae120` = `SetShowPromotionEquips` / `SetShowOriginalViewList`), tail anim (`ChangeAnimTail` = March), BTI 423 / 424 cases and
  enum names (`X2BuffFactor.h:545-546`), +0x979c / +0x9768 countdowns: CH1h / CH1h2 / CH1j. `EffectSet_SpecterOfThunder_Buff_Damage` is in the live
  `data036_kom/EffectSet.lua` (2 hits); `Motion_Ara_Tail_03.x` / `_06.x` are in `data131_kom` (CH0b).
- **Found, not ported:** June also loads `IS_ATTACK_STATE` into the future copy (@`32052269`, +0x7da8); `dispsweep 7da8` finds no reader but the loader, so
  nothing to port (a `GetStateparam( true )` read would not show in the sweep: CH6's scans recheck).
- Checklist l.184 marked `(Ara: CH1q2)`.

**Hygiene.** `flagcheck JUNE_CHAR_SPECTER_STATE --worktree --with JUNE_CHAR_CHAIN_ANIM_SPEED JUNE_CHAR_ARA_SPECTER JUNE_CHAR_ARA_LIVE --files <6 files>`: 2 PASS
(`X2GUUser.cpp`, `X2GameUnit.h`), 3 OTHER (`X2GUAra.*`, `X2GUUser.h`: the sibling flags), 0 FAIL, 1 ERROR; the same with `CHAIN_ANIM_SPEED` primary: 4 PASS, 1
OTHER, 0 FAIL, 1 ERROR; `_SPECTER --with CHAIN_ANIM_SPEED --files X2GUAra.*`: 2 PASS. The ERROR is `Always.h`: the tool cannot evaluate an older compound guard
(`_FP && !_LIVE`, line 12217) once `_LIVE` is in the set; the hunk is 24 inserted lines, 0 deleted (`git diff`). 0 deleted, 0 non-ASCII, 0 EOL in every run;
RESULT FAIL only for the commented-out `//#define`s. `junediverge.py --check <6 files>`: allow-listed 7, `RESULT: PASS`. `enumscan.py --check X2GUAra.cpp
X2GUUser.cpp`: 0 A/B rows. Probes (`/c /Zs /EHsc`, `X2GUAra.cpp` and `X2GUUser.cpp`): off, the two user flags, `_LIVE` + `CHAIN_ANIM_SPEED`, `_LIVE` + `_FP` +
`_SPECTER` + both user flags: 0 errors (8 runs); `_LIVE` alone and `_SPECTER` without `SPECTER_STATE`: the new guards fire (C1189), as meant. Full build
skipped: no play-test.
Range (`41127c94^..41127c94`, same `--with` set and 6 files): `SPECTER_STATE` primary 2 PASS, 3 OTHER, 0 FAIL, 1 ERROR (`Always.h`, as above); `CHAIN_ANIM_SPEED`
primary 4 PASS, 1 OTHER, 0 FAIL, 1 ERROR; 0 deleted, 0 non-ASCII, 0 EOL; RESULT FAIL only for the commented-out `//#define`s.

**Routed.** CH1 (amended): define both user flags with `_LIVE` / `_SPECTER`; tail check in the play-test. CHNa (amended): the Specter checks on 34 / 124.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1q2 review` (Opus 5.5 high).

### CH1q2 - Review (2026-11-11): pass, no code change

Checked against the exe bytes (`disasm_range.py`, `charport_adis.py`, `dispsweep.py`, raw vtable reads; Ghidra not needed). No debug define to retire.
- **Loaders.** `xorstrings.tsv`: `320506f0` loads `APPLY_ANIM_MOVE` @`3205210b`, `IS_ATTACK_STATE` @`32052269`, `IS_SPECTER_STATE` @`320523c2`, then
  `USE_CHANGED_DISPARATE_FORCE`; the C++ load sits after `APPLY_ANIM_MOVE` (March has no future `IS_ATTACK_STATE`), same order. @`320524a6`-`320524e4`: `push 1`,
  `lea ebx,[esi+0x7da9]`, `call 32fe9e40`, `test al,al; jne; mov [ebx],al` = false when missing. `32057b80` @`3205874f`-`3205878e`: same shape into
  `[edi+0x8049]`, right after `IS_ATTACK_STATE` (`[edi+0x8048]` @`320585f6`) and before `TECH_POINT`: matches. `StateParam` +0x95: June `operator=` `32081a30`
  copies byte +0x95 @`32081eb8` between +0x94 and +0x96; June `Init` `320820e0` clears it with the dword store `[esi+0x94] = ebx` (0, `xor ebx,ebx` @`3208210a`)
  @`320824ad`. The C++ copies it in `operator=` and sets false in `Init`: matches.
- **Chain anim speed.** `32051b78`-`32051bc1`: `APPLY_ANIM_SPEED_BY_ITEM_ABILITY` (default true) then `+0x9134` added to `+0x8900` under it (March's
  `GetAddAnimSpeedFactor` block); `+0x8900` is the `ResetAnimSpeed` first argument @`32051c14`, so `m_fAnimSpeedFuture` is right. Chain test `32071930`
  `GetPtrNowUseChainInfo` non-null -> `fld [+0x8908]; fmul [+0x8900]; fstp [+0x8900]; fld1; fstp [+0x8908]`, then `ResetAnimSpeed`: matches. Ctor
  @`320425c2` stores the `fld1` @`32042580` (1.0). `dispsweep 8908`: ctor, the two reader lines, and Ara `32bc8380` only. `ADD_ANIM_SPEED_FUTURE` is defined
  (probe), so the block is in the live branch.
- **`32baf270`.** vslot 0x9c = Ara `IsFullHyperMode` `32ba6cc0` (vtable `33505314`). `fldz; fcomp [+0x9744]; test ah,5; jp` falls through when 0 < rate;
  the protected int `+0x6844` = `GetHyperModeCountByBuff()` (`X2GameUnit.h:3269`) `cmp 3; jge` -> base `3201c3a0` when < 3, else vslot 0x630 = Ara
  `UpdateNotRenderingOtherEquipList` `32ba83d0`. `32bd7780` copy-constructs the local from `+0x95a4`, `+0x95b4` pushed, base `31fd8c00`. Else branch: direct
  `31fd5b80( 1 )` (= base `SetShowViewList`, vslot 0x63c), `3201c3a0`, `31fd8c00( +0x8834 )`. The three `SetEnableAttackBox` calls follow both: matches.
- **`IsSpecterState`.** `32bcee1c` and `32bd2c74` both call vslot 0x128 with `push 0` (now copy), `cmp byte [eax+0x95],1`. Body = `GetStateparam( false )
  .bSpecterState`: matches. Guard chain closed: `_SPECTER` -> `_LIVE` -> `_CORE` (header decl), `_SPECTER` -> `SPECTER_STATE`; `m_fBuffHyperModeDamageRate`
  is `_CORE`.

**Hygiene.** `flagcheck JUNE_CHAR_SPECTER_STATE --range 41127c94^..41127c94 --with JUNE_CHAR_CHAIN_ANIM_SPEED JUNE_CHAR_ARA_SPECTER JUNE_CHAR_ARA_LIVE --files <6>`:
2 PASS, 3 OTHER, 0 FAIL, 1 ERROR; `CHAIN_ANIM_SPEED` primary: 4 PASS, 1 OTHER, 0 FAIL, 1 ERROR; 0 deleted, 0 non-ASCII, 0 EOL, 0 bare `#else`/`#endif`, 0
encoding changes; RESULT FAIL only for the commented-out `//#define`s; the ERROR is the older `_FP && !_LIVE` guard (`Always.h:12217`), as in the Done note.
`junediverge.py --check-all`: allow-listed 202, `RESULT: PASS`. `enumscan.py --check X2GUAra.cpp X2GUUser.cpp`: 0 A/B rows. Probes (`/c /Zs /EHsc`):
`X2GUAra.cpp`, `X2GUUser.cpp` off and with `_LIVE` + `_FP` + `_SPECTER` + `SPECTER_STATE` + `CHAIN_ANIM_SPEED`: 0 errors (4 runs). Full build skipped: no
play-test. `kickoffs/char-ara/CH1.md` "Amended by CH1q2" checked: complete (both flags, `--pch force`, tail and chain checks).

**Nothing to play-test.** Next: `/x2-phase char-ara CH1` (Opus 5.5 high).

### CH1 - Done (2026-11-11)

Opus 5.5. **The deploy unit is live.** Defined in `Always.h`: `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (:12193), `_FP` (:12216), `_SPECTER` (:12264),
`SERV_IRUHADEV_JUNE_CHAR_SPECTER_STATE` (:12384), `SERV_IRUHADEV_JUNE_CHAR_CHAIN_ANIM_SPEED` (:12393). Ara now runs June's registration, state bodies,
virtuals, skill head, pressed-slot space, force power / berserk and the tails (CH1a-CH1q2). No debug define: the `ARA FP` lines were retired by the CH1q review.
Every `#error` guard is satisfied: the probes below compile with all five on.
- **`[JUNE_CHAR] ARA DATA`** (`_LIVE`, end of June's `InitState`, `X2GUAra.cpp:594-597`; include `:6-8`), Rena's line: class, script, motion device,
  state count, `wait_state_change`. Not June; stays after the review, as Rena's did.
- **Script:** `luasrc.py adopt-june data036 Ara_Martial_Artist.lua` (dry run first): June's file replaces March's whole as `ljconv` bytecode, 250,093 B
  sha1 `8b6d21d6`, no core-list row, no sidecar. Record `june2017_re/june_adopt_20261111_121334.tsv`, backup `kom_backup_stage_june\20261111_121334`.
  `luabind.py check Ara_Martial_Artist`: 0 methods, 0 unbound, `RESULT: PASS`. `luasrc.py core --check` / `ledger --check`: PASS, 0 FAIL, 0 WARN.
- **Held motions** (`ch1_held_motions.tsv`): `kom.py stage-june`: June's `Motion_Ara_LD.x` (3,162,196 B) / `_SD.x` (3,557,126 B) replace March's in data033
  (backup `kom_backup_stage_june\20261111_121404`). **The case question is closed:** June's script plays `SI_SA_ALD_ENERGY_WAVE_START` / `_LOOP` / `_END`
  (src.lua:14486, 14588, 14614), the names June's LD file has. The mixed-case `_Start` / `_End` were March's script's names (CH0's tsv row pointed at the
  June line of the renamed state). No June bug, no `JUNE_BUGS.md` row. SD's dropped `ASD_FallingBlossom_*` were played only by March's script; June plays
  `ALH_FallingBlossom_*`. `SI_SA_ASD_SUPPRESSION` sits in June's LH file, which 118 loads as `m_GameMotion1` (March's `UnitTemplet.lua` has the same slots).
  No March C++ plays a dropped name.
- **Packed / copied (no ask, CLAUDE.md 2026-11-10):** `PACK OK data036` (~1), `PACK OK data033` (~2);
  `COPY OK data036 ... 14069098 bytes, 270 entries, md5 9aedbce1533e800e8767f3f2cf911030`, `COPY OK data033 ... 183744353 bytes, 165 entries, md5 263ace5f15c8da8105588f650f61eb14`.
- **Reference log:** `data\ErrorLog_pre_CH1_ara.txt` (425,351 B, the log of the last pre-CH1 run, 2026-11-11 11:58); Ara-load lines also against
  `ErrorLog_pre_CH0b_ara.txt`. CH0b's routed `ErrorEnum(433)` face-mesh lines are re-checked on this play-test.
- **Scans:** `charport_keys.py Ara_Martial_Artist`: `june-only=5 neither=7 march-only-site=4 both=234` (CH0: 14 / 7 / 4 / 225). The 9 that moved to both are the
  CH1q / CH1q2 readers: `FORCE_POWER_COUNT_AT_DUNGEON_START` / `_PVP_START`, `FULL_HYPER_MODE_ADVANTAGE_COUNT` / `_INTERVAL_TIME` / `_SOCKET_RATE`,
  `GAIN_FORCEPOWER_USEDHYPERCOUNT`, `GAIN_MP_BY_FPCONVERSION_HYPERMODE`, `IS_SPECTER_STATE`, `TAIL_TYPE`. `charport_scan.py` (census rerun first: 26 PASS, 0 FAIL;
  `census.tsv` / key matrix reverted after): byte-identical to `plan_charport_scan_20261110.txt` (`no NUMERICAL_DATA_INDEX` 49, `no DAMAGE table` 14). Nothing new.
- **Skills** still go through March's id switch below June's head (CH1n) until CH2a. `ReservedSkillStart` (CH1k) is live, so the two-cast check is in
  the hand-off as a light check; a skill that misbehaves goes to CH2a (Rena's precedent).
- **Checklist:** `(Ara: CH1 ...)` at l.16, 212, 423, 427, 509, 548, 592, 674, and `(Ara: CH0b / CH1 ...)` at l.760.
- `MODS.md`: "Stage 7 Ara, CH1" (flags, the two replaced assets and their backups, the revert order).

Hygiene: `flagcheck.py JUNE_CHAR_ARA_LIVE --with JUNE_CHAR_ARA_FP JUNE_CHAR_ARA_SPECTER JUNE_CHAR_SPECTER_STATE JUNE_CHAR_CHAIN_ANIM_SPEED JUNE_CHAR_ARA_SKILL
JUNE_CHAR_ARA_FP_DEBUG --worktree --files X2Lib/X2GUAra.cpp KTDXLIB/Always.h`: `1 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `RESULT: PASS`. The 5 deleted lines
are the five `//#define` lines. Without `--with`, `Always.h` is an ERROR: the `#if defined( _FP ) && !defined( _LIVE )` guards cannot be evaluated with `_LIVE`
stripped alone. `_FP` / `_SPECTER` alone: `2 OTHER, 0 FAIL`, `RESULT: PASS`. `junediverge.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2GUUser.cpp
X2Lib/X2GUUser.h KTDXLIB/Always.h`: `allow-listed: 10`, `RESULT: PASS` (no new `GT_PVP` hit now that `_LIVE` is on). `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s)`.
Probes (`/c /Zs /EHsc`, all five on): `X2GUAra.cpp`, `X2GUUser.cpp` exit 0, 0 errors.
Build (`--pch force`): `DEPLOY OK: X2_offline.exe is 26,225,152 B, dated 2026-11-11 12:23:18, md5 328d70d53527873f2764a9296ae0493a, equal to X2/US_SERVICE/x2.exe (2026-11-11 12:23:11); built at main f3450220 + 2 uncommitted code file(s).`
`EXESTR OK: 1 of 1 found in X2_offline.exe, 26225152 B, dated 2026-11-11 12:23:18, md5 328d70d53527873f2764a9296ae0493a` (`[JUNE_CHAR] ARA DATA` wide x1).

**Play-test (user) is the gate:** units 96 (7), 97 (28), 99 / 38 (31), 98 (118), 48 / 24 (119), none transcended, skills in slots 0-3 only.
Check: `ARA DATA` per class, combos, dash, jump, double jump, damage / stand-up, riding, force power and berserk (visual), tails, the chain speed on 98, and
ErrorLog against the references. The review is `kickoffs/char-ara/CH1-review.md`.

**Play-test fix (2026-11-11, first run 12:28-12:30).** Entering a field showed two popups, `StateID` then `ComboAndSkillChainSystem Parsing
Error`, and ErrorLog had `lua error while loading ARA_MARTIAL_ARTIST.LUA: can't find 'CRS_INVISIBLE' class variable`. Both come from the
merged data036 `Enum.lua` (core-list source) missing June members that June's script reads:
- **`CHAIN_SYSTEM`** (script top, read by `CX2ComboAndSkillChainSystem::CreateComboAndSkillChain`, `X2ComboAndSkillChainSystem.cpp:40`): 13 of its
  37 `ARA_STATE_ID` names were absent, so slot [1] read nil. **The 24 present names were also wrong:** `Enum.lua`'s `USER_STATE_ID` stopped
  following the C++ when `SKILL_SLOT_10` added `USI_SPECIAL_ATTACK_5` / `_HYPER_5` / `_SLOTB_5` / `_SLOTB_HYPER_5`, so from `SLOTB_1` on every
  `USI_` value and every class state table built on `USI_END` read 2 or 4 low (`enumprobe.py`: C++ `USI_END` = `GUSI_END + 101`, Lua `+ 97`). Ara's
  chain table is the only Lua that hands these ids back to C++, so she shows it first. The training-dungeon scripts read them too, and are now right.
- **Fix (data):** `USER_STATE_ID` = the C++ values for all 97 names plus the 4 slot-5 names (46 values changed); `ARA_STATE_ID` gains the 13 names at
  their C++ values; `SKILL_ID.RNSI_A_ASR_RAID_UPGRADE = 27242` (June's; the script's `RELATE_SKILL_ID`); June's `CUSTOM_RENDER_STATE` table
  verbatim (`Enum.src.lua:17473`; Asura's Fox Phantom state at src.lua:21006, also June's `Add_NasodRuler.lua`; no C++ reader yet). Each block
  carries a one-line `-- char-ara CH1` comment. `luasrc.py refresh data036 Enum.lua`; `core --check` / `ledger --check` PASS. Every enum
  reference in the script now resolves except `CAMERA_TYPE.CT_PARTS_LOOK_TRACKINGT` (absent from June's own `Enum.lua` too; it reads nil in June).
  `PACK OK data036`, `COPY OK data036 ... 14069630 bytes, 270 entries, md5 45b061dc523c59bd03f55d2f5e62a253`.
- **Fix (C++):** 5 of the 13 had no C++ member (34 / 124 skill states): `ASI_A_ALS_VIEW_BETWEEN`, `ASI_A_ALS_SHAKE_CHAOS`,
  `ASI_SA_ALS_RESTRAINTS_SPECTER`, `ASI_A_AA_RAID`, `ASI_SA_AA_SMASH` are appended to `ARA_STATE_ID` under `_CORE` (`X2GUAra.h`, `USI_END + 176
  .. 180`, after CH1i's last name). Their owners are unchanged (CH2d3, CH2d3, CH2d, CH3c2, CH3c). June's own ids (288 / 291 / 302 / 294 / 315) would
  land on C++ ids that other Ara states already hold. `flagcheck JUNE_CHAR_ARA_CORE --worktree --files X2Lib/X2GUAra.h`: `1 PASS, 0 FAIL`, `RESULT:
  PASS`; `junediverge --check` PASS; `enumscan --check X2Lib/X2GUAra.h` 0 A/B. Probe `X2GUAra.cpp`: 0 errors.
- **Guard for later ports (user, 2026-11-11: "make sure this dont happen on future char ports"):** `tools/luabind.py check` now runs an
  ENUM gate (every `TABLE.NAME` from June's `Enum.lua` resolves in the staged `Enum.lua`) and a STATE gate (every `*_STATE_ID` value equals
  the C++ enum, `/Zs` probe, ~15 s; `--no-cpp` skips it). Against the pre-fix `Enum.lua` it reports Ara's whole failure (1 table, 14 names,
  24 wrong state ids); on the fixed one: `STATE: 37 ... 0 wrong`, `RESULT: PASS`. Wired into the x2-phase Lua binder gate, the checklist (§6),
  `tools/README.md`, and amendments to `kickoffs/char-elesis/CH0.md` / `CH1.md` and `kickoffs/char-luciel/CH1.md`.
- **Found in the ported classes, not fixed here (scope rule):** `luabind.py check` over the June class scripts lists silent nils:
  Elsword `SKILL_ID.RNSI_A_ERS_ENCHANT_BREAK` / `RNSI_P_ELK_FIGHT_GOD`, Raven `SKILL_ID.RNSI_A_RBM_POINTED_STILETTO` (all three have C++ members),
  Chung `CHUNG_STATE_ID.CSI_CFG_COMBO_XXFRONTZ` (C++ `USI_END + 168`), Aisha `DAMAGE_TRIGGER_TYPE.DTT_AISHA_STICK_ATTACK` (June 17, no C++ member).
  Asked of the user in the CH1 hand-off: fix now, or route.

**Play-test (2026-11-11, second run 12:52-12:58 on md5 0909cda3): user "all test passed".** Load, combos / movement, force power fill,
berserk with portrait and tail on units 96 / 97 / 99 / 98 / 48; skills not cast (CH2a's).
- `offline_server.log`: `ARA DATA` for all five classes, each `script ARA_MARTIAL_ARTIST.lua motion MOTION_ARA.X wait_state_change 1`;
  states 7: 139, 28: 164, 31: 154, 118: 182, 119: 162 (the review checks them against the registrations).
- `ErrorLog.txt` against `ErrorLog_pre_CH1_ara.txt` / `_pre_CH0b_ara.txt`: `KEM_ERROR` 0 / 0 / 0, `Motion Error` 0 / 0 / 0, `[LUA_SRC]` 7 / 7 / 7,
  `lua error` 3 / 3 / 3 (TutorSystem, `ChainQuestInfo` `AddGroupChainQuest`, both pre-existing); **no `ARA_MARTIAL_ARTIST` error, no popup.**
  No missing animation. `offline_packets.log`: `EXCEPTION` 0, `UNHANDLED` only `E_DISABLE_HB_CHECK_REQ` (known).
- CH0b's routed face lines: `ErrorEnum(433) Anim Frame : 80, Mesh Frame : 81` x21 (`MESH_ARA_BASIC_FACE_SORTED.X` 19, `..._CASH_FACE_BERSERK_SORTED.X`
  2) remain with June's script and motions: June's own data, `JUNE_BUGS.md` row 603. `ARA_COMBOSHOT.OGG` `result : 47` (CH0b's, existing sound
  path); `QUEST_11000_ARA_END.GFX` probe (event scene, plays from the `.swf`). Nothing new to route.
  `DEPLOY OK: X2_offline.exe is 26,225,152 B, dated 2026-11-11 12:51:03, md5 0909cda31c6bbad805e34fb4193375fa, equal to X2/US_SERVICE/x2.exe (2026-11-11 12:50:55); built at main d98916f0 + 1 uncommitted code file(s).`

### CH1 - Review (2026-11-11): pass, no code change

Opus 5.5. **Verdict: passes.** No source change; no debug define to retire (the `ARA FP` lines went with the CH1q review). `ARA DATA` stays, as Rena's did.
- **Deploy unit:** data036 `Ara_Martial_Artist.lua` 250,093 B sha1 `8b6d21d6...` header `1B 4B 4C 81` = the `june_adopt_20261111_121334.tsv` row (ljconv); no
  `.src.lua` sidecar. `stage-june --dry-run ch1_held_motions.tsv`: `2 staged (0 new, 0 replaced, 2 identical)`. `kom.py status data036 data033`: both in sync,
  `+0 / -0 / ~0` vs live (data036's `[stale .src.lua: compile]` is older non-Ara sidecars, as in Rena's review). `exestr.py "ARA DATA" --form wide`: `EXESTR OK: 1 of 1`
  (wide x1), md5 `0909cda3`. `luabind.py check Ara_Martial_Artist`: `ENUM 0 table(s) missing, 0 member(s) missing, 1 nil in June too` (`CT_PARTS_LOOK_TRACKINGT`),
  `STATE: 37 ... 0 wrong`, `RESULT: PASS`. `luasrc.py core --check` / `ledger --check`: PASS, 0 FAIL, 0 WARN.
- **State counts, exact for all 5 units** (logged at the end of June's `InitState`, `X2GUAra.cpp:595`): body 98 (`InitStateCommon`; June's 112 = 98 + 3 bounding +
  11 left out: riding 93-96, PvP amusement 123-126, dance 127-129) + 3 START / WIN / LOSE + 4 hooks (3 `DAMAGE_BOUNDING` from `InitBoundingStates( "ASI_" )`, 1
  `USI_WAIT_STATE_CHANGE`) + 9 `USI_EVENT_*` + 20 slot + 1 key 0 = 135, + class path (`InitStateByUnitClass`, fall-through as the CH1m review: 7 = 0, 28 = 15, 31 = 7,
  118 = 4 + 15, 119 = 2 + 7) + learned-skill sub-states (`SetEquippedSkillLevelStateData` `m_StateList[ id ]`, ids the class path did not register):
  7 (unit 96): 135+0+4 = 139; 28 (97): 135+15+14 = 164; 31 (99): 135+7+12 = 154; 118 (98): 135+19+28 = 182; 119 (48): 135+9+18 = 162. All = the log. The 99 / 48
  lines follow `ENTER unitUID=99` / `SLOT sent unit 48` (38 and 24 not loaded). Rena's formula counted START / WIN / LOSE inside her "C++" term; same totals.
- **START / WIN / LOSE:** `list size 0` 0, `re-bind skipped` 0 (both logs); 18 `JUNE_FX C3e-2 roll` lines pick real tables: `ASI_START`, `ASI_WIN2`, `ASI_LOSE`,
  `ASI_LOSE2`, `ASI_ALH_START`, `ASI_ALH_WIN2` (index 3 of 4), and the ALD / ASD / AYR rows.
- **Force power / berserk / tails / chain speed:** the user's run passed the visual checks (fill, berserk with portrait and tail on 96 / 97 / 99 / 98 / 48); skills
  were not cast, so the two-cast check moves to CH2a (its hand-off already carries it). Chain speed was not chained in the run (optional per the gate); it moves to
  CH3a's hand-off (`CH3a.md` amended).
- **ErrorLog** (current / `_pre_CH1_ara` / `_pre_CH0b_ara`): `KEM_ERROR` 0 / 0 / 0, `Motion Error` 0 / 0 / 0, `[LUA_SRC]` 7 / 7 / 7, `lua error` 3 / 3 / 3, `StateID` /
  `Parsing Error` / `ARA_MARTIAL_ARTIST` 0; `ErrorEnum(433)` 21 = the face-mesh 80 / 81 lines (`JUNE_BUGS.md` 603). New Ara lines are known or unopened: `ARA_COMBOSHOT.OGG`
  `result : 47` (CH0b's), `QUEST_11000_ARA_END.GFX` and 6 `HQ_CUTIN_ARA_RAGE.DDS.GFX` `.gfx` probes (both play; the cut-in one is routed to CH5's 7t `.gfx` names,
  `CH5.md` amended), one-time `OpenXET` probes for `MOTION_ARA_LOBBY` / `MESH_ARA_CASH_*`. `offline_packets.log`: `EXCEPTION` 0, `UNHANDLED` only `E_DISABLE_HB_CHECK_REQ`.
- **Ghidra spot-checks:** (1) Max force power 10 at every level: June's four `SetMaxForcePower` `32a32720` calls are `Init` @32bc7efc `push 0xa`, `ReInit`
  @32bc81a5 `push 0xa` (own unit only, after `test al,al`), `SetMyPlayStatusFromPacket` @32a34a2b `push 0xa`, and the gauge copy @32a349c3 (copies the other's max) =
  `X2GUAra.cpp:344` / `:430` (`MAX_FORCE_POWER` 10, `:123`) and `X2GageData.cpp:1024` / `:970`; the level formula is only in the `#else` branches. (2) June's chain parser
  `32d3f2a0` (the caller of `SECRET_SKILL_STATE_ID` `32d3f710`, then `32d3f8c0` / `32d3fb30`) reads the same three fields per row (`32fea4e0` 1 / 2 / 3), looks the state
  up with `32d3ee60` and creates / connects / pushes as March's `CreateComboAndSkillChain` (`X2ComboAndSkillChainSystem.cpp:25-95`), so June's `CHAIN_SYSTEM` rows parse
  as March reads them. June returns false silently on a bad row where March pops `DISPLAY_ERROR`; reachable only with a broken table, which the enum gate now refuses
  before a pack: noted, not ported (no Ara behaviour).
- **Routed gaps:** the five silent nils CH1's enum gate found in Elsword / Raven / Chung / Aisha have their plan (`enum-gate-fix`, `a3b0ca83`).
- **Checklist / scans:** `(Ara: CH1 ...)` marks present (now l.16, 212, 426, 430, 512, 551, 595, 677, and `(Ara: CH0b / CH1 ...)` l.763: the checklist grew 3 lines).
  `charport_keys.py Ara_Martial_Artist`: `june-only=5 neither=7 march-only-site=4 both=234` = CH1's.
- **Kickoffs:** `CH5.md` amended (cut-in table now `X2GUAra.cpp:89-118`, live switch `:538-556`, March's `#else` `:599-629`, every `m_hSeqEnergyBullet` use, the cut-in
  `.gfx` probe). `CH2b0.md` amended (`ProcessLockOn` now `:8677-8780`). `CH2a.md` (no line refs; carries the two-cast check) and `CH2c0.md` (checklist 7at, addresses
  only) match.

Hygiene: `flagcheck JUNE_CHAR_ARA_CORE --range 74eca033^..74eca033 --files X2Lib/X2GUAra.h`: `1 PASS, 0 FAIL, 0 ERROR`, all zeros, `RESULT: PASS`. `JUNE_CHAR_ARA_LIVE --range
d98916f0^..d98916f0`: `X2GUAra.cpp` PASS +7; `5 deleted` (the five `//#define` lines), `0 non-ASCII, 0 EOL, 0 #else/#endif without the flag token, 0 encoding`; `Always.h`
ERROR only: each `--with` set strips a flag another guard names (`:12217` `_FP && !_LIVE`, then `:12253` `_SKILL`, then `:6033` `EFFECTSET_BUFF_LIST`), so RESULT FAIL is
the guard evaluation, as in the CH1 Done note and the CH1q2 review. `_FP` / `_SPECTER`: `OTHER` only, `RESULT: PASS`; `SPECTER_STATE` / `CHAIN_ANIM_SPEED`: the same
`Always.h` guard ERROR, no code row. `junediverge.py --check-all`: `allow-listed: 205`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `0 A/B row(s)`.
Full build skipped: no play-test (no code change). CH5, CH2a, CH2b0 and CH2c0 are ready.

## Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces

**Run with:** Sonnet 5.5 high, Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_UI`. Sub-tag `UI`.

**Scope:** Ara's 5 `UnitTemplet.lua` rows become June's and June's rows for 34 and 124 are added (BOM source, `luasrc.py refresh` after; every other
row stays; if CH0 found the templet loader refuses a class >= `UC_END`, the two new rows wait for CHNa): `m_fLobbyRotationY`, `m_HyperCutIn` for
118 / 119 / 124, the names; the cut-in table `X2GUAra.cpp:36-60` and the InitState index switch `:280-310` replaced by June's names and layout (7t
cut-in `.gfx` names); 7ah; the lobby orb handles `m_hSeqEnergyBullet1-3` (`X2UnitViewerUI.*`, 7e) per Open decision 2. **Creation screen (user, 2026-11-10: a char-port item, not Stage 9).** Ara's entries on the character-creation screen are part of this phase: `DLG_Unit_Create_ClassChange_Button_Ara.lua` (`adopt-june`, `luabind.py check`) and every June function the creation state / unit viewer runs for Ara (class-preview switch, class buttons, `Movie_Create_Ara_*` previews), listed from June first, then ported; classes 34 / 124 are CHNa's. **Done gate:** a
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

**Creation screen (user, 2026-11-10: a char-port item, not Stage 9).** CHNa adds Little Specter (34) and Asura (124) to the creation screen's Ara class tree and previews (the button-dialog rows CH5 adopted, preview mesh / motion, `Movie_Create_Ara_AA`), once the class loads.

**Done gate:** a 34 and a 124 reached both by job change and by `createchar.py` load, fight and relog; the socket and stat paths log no refusal; no
other class changed (a select-screen pass of all nine March characters).

### CHNa - Done (2026-11-11)

Opus 5.5. Flags `SERV_IRUHADEV_JUNE_CLASS_PATH` (class-generic, reused by Elesis / Add) and `SERV_IRUHADEV_JUNE_CHAR_ARA_CLASS` (Ara rows), + `_CHAR_ARA_CLASS_DEBUG` (the review retires it);
all at `Always.h` end, `#error` without `SERV_IRUHADEV_JUNE_ENUM_NAMES`. Review kickoff `kickoffs/char-ara/CHNa-review.md` written.
- **Gauge (crash fix):** `X2GageManager.cpp` Ara arm gains 34 / 124 (June `3248e290`), so `CX2GUAra`'s static_cast to `CX2AraGageData` is no longer a smaller object. Log `[JUNE_CHAR] ARA CLASS gauge class <c> -> CX2AraGageData` (debug).
- **Items:** `GetX2UnitClassCompatibility` 34 -> {7, 34}, 124 -> {7, 34, 124} (June `31ed6500`; spot-checked lines 174 / 463).
- **Creation screen:** `GetThirdLineAraClassTypeByUnitClassType` = June's table (`3234b390` reads `337a1660`, filled by `3234afb0`: Ara block 7 / 0x22 / 0x7c), March's all-`UC_NONE` body in `#else`;
  the June third-line buttons CH5 adopted now select Little Specter / Asura. Everything after is templet-driven (`SelectUpperClass`): promotion costume, `Movie_Create_Ara_AA.avi` (CH5's rows),
  `text_class_2_3` / `name_class_*_3` keys present in `..._character_info_07.TET`. Stat chart (`GetUnitStatInfomationChart`, CH5-routed): 34 / 124 = (HIGH, LOW, LOW) = June `3254ee40` 3,1,1.
- **Portraits (CH5-routed, `GetCharacterImageName`):** index 55 / 56 appended to all nine March arrays + the switch, from June `31ee0de0` rows 0x39 / 0x3a (decoded from the nine static initializers;
  the decoder reproduces March's 48 / 49 Little Devil / Yama Raja rows exactly in every table): `L_SPECTER_*` in Texture77, `ASURA_*` in Texture82, `HQ_Dungeon_Clear_Ara_Little_Specter / _Asura_Normal.tga`,
  `CLASS_L` / `CLASS_A`, `list_ARA_L_SPECTER` / `list_ARA_ASURA` (the gray slot adds `_GRAY`, both present). Every piece checked in the March TETs.
- **Portrait assets (user, mid-phase: "replace the portrait asset with june's"):** `kom.py stage-june` on the 22 portrait files: 18 already June's byte-exact, 4 new (`DLG_UI_Common_Texture101_NEW` / `_GRAY` `.tga` + `.TET`, the
  transcended pairs, data215). `PACK OK` + `COPY OK data215` md5 `4c7d003e67256dc9ce5a7b51e81a97ab` (backup `kom_backup_tools/data215.kom.20261111_175929`).
- **Class path:** `X2SocketItem.cpp` SkillLevelUpSocketGroup bound `> UC_AIN_ANGEL` (June `3271a810`: class 1..202) instead of `>= UC_END`: the 996 dropped rows (Asura 75, every class >= 124) now load.
  `CX2Unit::GetClassLevel` = June `3234c170` (34 = 1, 124 = 2, outside 10-199 = 0; no caller tests March's -1). Divergence rows added: `GetClassLevel`, `GetThirdLineAraClassTypeByUnitClassType`, `AddSkillLevelUpSocketGroup_LUA`.
- **Data:** `CorrelateUnitClassTemplet.lua` = June's bytecode (`adopt-june data036`, record `june2017_re/june_adopt_20261111_180551.tsv`; loader reads up to 3 ids per row, no bound test; `luabind` PASS).
  `PACK OK` + `COPY OK data036` md5 `cb3fa6093b0e16adc7a6f6ab4041bec0` (1 changed; backup `kom_backup_tools/data036.kom.20261111_180610`).
- **Stat rows: none written.** Superseded by the user's 2026-11-11 flat StatTable (one curve for all 123 ids): `statflat.py check` = `STATFLAT OK: 12177 rows, 123 classes`, 34 / 124 included, already in the live data036.
  So Open decision 1's per-class rows and the type-fallback log are moot (the fallback path is not reached).
- **Tool:** `createchar.py` CLASSES + PARENT gain 34 (-> 7) / 124 (-> 34). Units (backup `db_backup/els_db_20261111_180530_pre_manual.sql`): 102 `LSpecter20` (34, L20), 103 `Asura40` (124, L40), 104 `Asura99` (124, L99);
  slots 0-3 = Gain Force 27200 / Tempest Dance 27000 / Pulling Pierce 27001 / Falling Dragon 27002; slot 5 = Specter of Thunder 27023 (102, 104), slot 6 = Necromancy 27028 (104) (backup `..._181001_pre_sql.sql`).
- **Checked, not needed:** `RotateYSelectCharacterType` (June `31edf260` reads the templet's `m_fLobbyRotationY`; March's templet has no such field, so 34 / 124 rotate at the default 10: shows fine; the reader stays Lu/Ciel NU4).
  `X2GUUser.cpp:534-545`, `X2OfflineLuaEnumSeed.h`, the log-only `X2ItemManager_Preprocessing.cpp:688` / dev `X2GameEdit.cpp:1643` (no June test to copy) per CH0.
- **Routed:** the transcended 124 portrait pair (`TR_ASURA_*`, five-argument overload's prefix switch) -> CH3d; job change -> CHNb; 34 / 124 skills -> CH2d / CH3c. A skill-level socket on an Asura weapon
  is not observable without such an item: the bound fix is proven by code (June's test), not by the play-test.
- **Hygiene:** each changed file PASSes under its own flag: `_CLASS_PATH` (`X2SocketItem.cpp`, `X2Unit.cpp`), `_CHAR_ARA_CLASS` (`X2Data.cpp`, `X2StateCreateUnit.cpp`, `X2Unit.h`), `_CHAR_ARA_CLASS --with _CLASS_DEBUG`
  (`X2GageManager.cpp`); 0 deleted, 0 non-ASCII, 0 EOL mismatch, 0 untokened. The single ERROR per run is the pre-existing `#if defined( SERV_BALANCE_FINALITY_SKILL_EVENT ) || defined( SERV_IRUHADEV_JUNE_DIVERGE_SKILL_COOL_TIME_RATE )`
  in `X2Unit.cpp:5842` / `X2Unit.h:1252`, reported on the file that does not carry that run's flag (tool limit, not this phase's code). `junediverge --check` (6 files): allow-listed 9, RESULT: PASS. `enumscan --check`: 0 rows on the changed lines.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 26,225,664 B, dated 2026-11-11 18:19:44, md5 82393388935ba52ba03c227714bb9f84`.
  `EXESTR OK: 6 of 6` (wide: the `ARA CLASS gauge class` tag, `L_SPECTER_STATE`, `ASURA_PARTY`, `list_ARA_L_SPECTER`, `HQ_Dungeon_Clear_Ara_Asura_Normal.tga`, `CLASS_A`).
- **Play-test hand-off:** see the session reply. Log tag `[JUNE_CHAR]` (`ARA CLASS`).
- **Play-test (user, 2026-11-11 19:23-19:28, exe `82393388`).** Creation screen (user screenshot): Little Specter / Asura select, model, costume and the speed / range / difficulty chart work; the class name showed March's Korean
  placeholder ("1-3...") and the story text, radar chart and job text were blank. Cause: all four are pieces of `DLG_UI_Common_Texture_GUI_character_info_07` (`name_class_*_3`, `text_class_2_3`, `star_graph_class_2_3`,
  `text_class_under_2_3`), and March's copy differs from June's. `stage-june` replaced its `.tga` + `.TET` (backup `kom_backup_stage_june/20261111_192614`), `PACK OK` + `COPY OK data057` md5 `f0d9482f6467e1dba00db7bf3126841f`
  (backup `kom_backup_tools/data057.kom.20261111_192841`); re-check on the creation screen owed (no rebuild). Logs: `ARA CLASS gauge class 34` x1, `124` x2 (LSpecter20, Asura40 / Asura99 loaded), EXCEPTION 0, KEM_ERROR 0.
  `ErrorEnum(433) Anim Frame 80, Mesh Frame 81, MESH_ARA_AA_BASIC_FACE_BERSERK_SORTED.X` x16 and the misspelt `MESH_ARA_AA_CASH_BERSERK_FACE*.TGA` texture names: the mesh and `Motion_Ara_AA.x` are June's byte-exact
  (`stage-june --dry-run`: identical), so this is June's own data (Asura berserk face, cosmetic), not a port gap. `OpenXET` misses are the usual optional-file probe (non-Ara names too). Not mine: 4 `E_DISABLE_HB_CHECK_REQ`, `AddGroupChainQuest`, `Add_Skill_Page_ED`.

### CHNa - Review (2026-11-11, Opus 5.5): PASS, one change (debug define retired)

- **Gauge** `3248e290` (Ghidra): Ara arm cases 7, 0x1c, 0x1f, 0x22, 0x76, 0x77, 0x7c (`new` 300 B gauge data, `32e64700` UI) = March's five + the port's 34 / 124 (`X2GageManager.cpp:192-196`).
- **Compatibility** `31ed6500`: case 0x22 -> 7 or 0x22; case 0x7c -> 7, 0x22 or 0x7c = the two arms (`X2Unit.h:2055-2082`).
- **Creation screen:** `3234b390` = `337a1660[ unitType ][ classType ][ line ]` (`(line + type * 4) ... ` strides 0x14 / 4); `3234afb0` writes Ara (`+0x230`) basic `1894/1898/189c` = 7 (lines 1-3), first `18a8/18ac/18b0` = 0x1c / 0x1f / 0x22,
  second `18bc/18c0/18c4` = 0x76 / 0x77 / 0x7c. Line 3 (`UCL_THIRD` = 3) = 7 / 34 / 124 = the port's function; `UCT_*` 0 / 1 / 2 match. `3254ee40` case 0x22 / 0x7c writes 3, 1, 1 in the struct's order = (`USICL_HIGH`, `USICL_LOW`, `USICL_LOW`).
- **Portraits** `31ee0de0`: class 0x22 -> row 0x39, 0x7c -> 0x3a; type 1..9 bases = March's `CIT_DungeonClear` .. `CIT_UnitSlot` order (type 10 = `CIT_UnitSlotGray`, already June's). Read from the type-7 (`CIT_MyGage`) initializer `3336f3e0`:
  row 0x39 (`33784ed0`) = `DLG_UI_Common_Texture77_NEW.tga` / `L_SPECTER_STATE`, empty transcended pair; row 0x3a (`33784f40`) = `Texture82_NEW` / `ASURA_STATE` + the `TR_ASURA_STATE` pair (CH3d) = the port's 55 / 56.
  Positional check: each of the nine arrays has 57 entry lines, no `#else`, guards only `SERV_ARA_CHANGE_CLASS_SECOND`, `SERV_ELESIS_SECOND_CLASS_CHANGE`, `SERV_9TH_NEW_CHARACTER`, `SERV_ADD_LUNATIC_PSYKER`, `_CHAR_ARA_CLASS`;
  the four March groups' X2Data-only pieces are in the deployed exe (`EXESTR OK: 4 of 4`: `Y_RAJA_STATE`, `EL_SBH_FACE`, `ADD_APT_FACE`, `ADD_ALP_FACE`, md5 `82393388`), so 0..54 are compiled and 55 / 56 land on their rows.
- **Class path:** `3271a810` `( 0 < group ) && ( 0 < skill ) && ( class - 1U < 0xca )`; `UC_AIN_ANGEL` = 202 (`X2Unit.h:193`) so `> UC_AIN_ANGEL` refuses the same set. `3234c170` `( class - 10U < 0xbe ) ? 1 + ( 99 < class ) : 0` = `X2Unit.cpp:3023-3029`.
  The three `june_march_divergent.tsv` rows are present and say the replacement landed.
- **Data:** `CorrelateUnitClassTemplet.lua` adopt record (data036, 2914 B bytecode, ljconv); `luabind.py check CorrelateUnitClassTemplet` PASS (ENUM 0 / 0). `stage-june --dry-run`: `DLG_UI_Common_Texture101_NEW` and `Texture101_GRAY` (`.tga` + `.TET`, data215) and
  `..._GUI_character_info_07` (data057) are all June's, identical (the kickoff's "_NEW / _GRAY" is these two names; no `Texture101_NEW_GRAY` exists). `kom.py status data215`: in sync. `createchar.py` CLASSES 34 / 124, PARENT 34 -> 7, 124 -> 34.
- **Hygiene:** `flagcheck --range d7f0e948^..d7f0e948`: `_CLASS_PATH` 2 PASS (`X2SocketItem.cpp`, `X2Unit.cpp`), `_CHAR_ARA_CLASS` 3 PASS (`X2Data.cpp`, `X2StateCreateUnit.cpp`, `X2Unit.h`), `--with _CLASS_DEBUG` 4 PASS (+ `X2GageManager.cpp`); 0 FAIL,
  0 deleted, 0 non-ASCII, 0 EOL, 0 token, 0 encoding; the one ERROR per run is the known compound `#if` (`X2Unit.cpp:5842` / `X2Unit.h:1252`) on the file outside that run's flag. `junediverge --check-all`: allow-listed 205, RESULT: PASS.
  `enumscan --check` (6 files): 0 rows on the changed lines (210 pre-existing elsewhere).
- **Logs:** the last run (19:33, exe `82393388`) is the user's creation-screen re-check after the data057 copy: no unit loaded, 0 EXCEPTION, no `ARA CLASS` line (none expected); the gauge lines (34 x1, 124 x2) are the 19:23 run's, recorded above.
- **Change:** `SERV_IRUHADEV_JUNE_CHAR_ARA_CLASS_DEBUG` commented out (`Always.h:12501`; it also drops the `X2OfflineLog.h` include in `X2GageManager.cpp`). Build started, not awaited (2026-11-03 rule).
- **Kickoffs:** `CHNb.md` amended (debug define retired; `createchar.py` already makes 7 / 34 units; `GetClassLevel` = June).

### CHNb - Done (2026-11-11)

Opus 5.5. New class-generic flag `SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE` (`Always.h` end; `#error` without `_TRANSCEND_CHANGE`, `_SKILL_CASH_OPEN_ITEM`, `_QUEST_LOAD`,
`_ENUM_NAMES`, `PRIVATE_BANK`, all confirmed defined by a `cl /EP` probe). Review kickoff `kickoffs/char-ara/CHNb-review.md` written.
- **Quest route (no change):** the shipped `FieldQuest.lua` is June's: 35560-35563 (class 7, level 15, `CJT_JOB_THIRD`, 35563 rewards `UC_ARA_LITTLE_SPECTER`) and
  35564-35568 (class 34, level 35, 35568 rewards `UC_ARA_ASURA`); `Handlers_Quest.cpp` turn-in takes any class (CH0 row). Proven by the play-test.
- **Level-up guides (data036 `LevelUpEvent.lua`, core-list source):** level 15 now opens `DLG_1ST_Class_Change_Guide_Ara.lua` (was March's `DLG_Class_Change_Guide_Ara.lua`),
  level 35 lists 28 / 31 / 34 and opens `DLG_2ND_Class_Change_Guide_Ara.lua` (had no dialog) = June `LevelUpEvent.src.lua:119-135, :338-355`. The level-70 row stays CH3d's.
  `luasrc.py refresh` done; `ledger --check` / `core --check` PASS.
- **The guides' buttons were dead in March:** they send June-only messages 1540-1548. `luabind.py check` on the two guides found 6 + 3 `ENUM MISSING NAME`
  (`LUEMCUM_CHANGE_CLASS_GUIDE_*`). data036 `Enum.lua` gains June's 24 missing `LV_UP_EVENT_MGR_CUSTOM_UI_MSG` members (values checked against June's table);
  `luabind.py check` on the four dialogs (1ST / 2ND guide, `DLG_Class_Change_Ara_LS` / `_AS`): 0 unbound, ENUM 0 / 0, RESULT: PASS.
- **`CX2LVUpEventMgr::UICustomEventProc` = June's whole switch** (`327127a0`, the T2 note's `32712790` is padding before it), March's in `#else`: closes events 1-6, the
  cash-shop links for every June class (1503-1539, 1553: `CloseFieldName( 0 )`, menu type = wParam, now state's `ToggleCashShop`, no state test, as June), 1540 closes
  the second guide, 1541-1544 `GuideAcceptClassUpQuest( QT_CHANGE_JOB, CJT_JOB_FIRST..FOURTH )`, 1545-1548 the shop on line 1-4's next class, 1549 / 1550 the
  transcendence pair (now through the same shop function). Returns: 1540-1550 true, the rest false (June's `AL`). 24 `LUEMCUM_*` + `LUEI_SECRET_DUNGEON_OPEN_GUIDE` (5)
  registered in `X2LVUpEventMgr.h` with June's values. Divergence row `CX2LVUpEventMgr::UICustomEventProc` added (ported).
- **`CX2CashShop::OpenClassChangeGuideItem( line )`** (new, June `3222a4f0`): menu type 0x1d; line 0 = 288319 (transcendence); otherwise the unit templet's tier,
  `GetUnitClassByTierAndLine( type, tier + 1, line )` and June's class -> change-item map (`3379fcf8`, 74 pairs read from `32230020`'s disassembly; 34 -> 252801,
  124 -> 252802), pushed to the open-at list (June's `+0x334`, carried by `+0x338` as the T2 port does); then `ToggleCashShop` outside June's non-menu states.
- **`GetUnitClassByTierAndLine`** (`X2Unit.h`, June `3234b390`): the CHNa-reviewed `ClassAt` table with `UC_*` names; `X2SkillNoteManager.cpp`'s `ClassAt` forwards to it.
- **Not ported (routed):** cases 1551 / 1552 (June fills `g_pStatic_ClassSkillDescription` on them); nothing in March or the staged guides sends them. Owner: Stage 9 UI sweep
  (class-change guide skill description), together with the 34 / 124 cash-shop class-change items (`UNIT_CLASS_CHANGE_*` maps `3379fce8` / `fd08` / `fd18`, the offline
  `X2OfflineClassChangeSeed.h` ROWS / PARENT_ROWS for 252801 / 252802; CH0 row "cash route"). `PARENT_ROWS` left as is: the quest route does not read it.
- **Found at play-test (routed to CHNb2):** June's `CheckLevel` (`327141e0`) calls `32713c10` on every guide it opens: event 2 word-wraps the three class descriptions
  (the 1ST guide's overlapping text), event 4 fills the 2ND guide for my class (name, portrait of my line, buttons rebound to 1541 / 1545 + line - 1, skill / command lists
  via `32713420`), event 6 a control call. Missing here, so the 2ND guide shows its script placeholders ("Psychic Tracer", "Primary Skill"). The JUNE_BUGS row this note
  first carried (buy button on line 1) was wrong (event 4 rebinds the buttons) and is withdrawn.
- **Data:** `kom.py pack data036` + `copy`: PACK OK / COPY OK (md5 `ac431479`, `Enum.lua` + `LevelUpEvent.lua` changed); data034 / data217 in sync (CH0b staged the dialogs).
- **Units:** `AraGuideMA` (uid 105, class 7, level 14, exp 81600 = 100 below 15) and `AraGuideLS` (uid 106, class 34, level 34, exp 1212400 = 100 below 35).
- **Hygiene:** `flagcheck --worktree` (7 files): 6 PASS, 1 DEFINE, 0 FAIL, 0 deleted, 0 non-ASCII, 0 EOL, 0 token, 0 encoding. `junediverge --check` (7 files): RESULT: PASS.
  `enumscan --check`: 0 rows on the changed lines.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 26,226,688 B, dated 2026-11-11 20:06:50, md5 8b63ccc6152ffdca693c5e549025a5a1` (built at `fae707a6` + the 7 files). No new strings, so no EXESTR line.
- **Play-test (2026-11-11, user screenshots):** 1ST guide opens with June's art (all 10 textures June-identical, `stage-june --dry-run`) but the descriptions overlap; 2ND guide opens unfilled. Both = `32713c10`, CHNb2. Job changes PASS: AraGuideMA 7 -> 34 (`/qdone 35560-35563`), AraGuideLS 34 -> 124 (`/qdone 35564-35568`, NetError 0 x5), both kept their class after a relog (save: 34 / 124); 2ND guide's Buy opened the shop on Sakra Devanam's item (line 1 until CHNb2's button rebinding); 0 EXCEPTION. `AddGroupChainQuest` unbound (ChainQuestInfo.lua) is pre-existing, backlog line 314.

### CHNb2 - Done (2026-11-11)

Opus 5.5. Flag `SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE` (CHNb's, class-generic), one more `Always.h` description line and four `#error` guards (`_SKILL_TREE_PANELS`,
`_UI_LINE_BREAK`, `_TRANSCEND_UNIT`, `_SKILL_TYPE`; all ON in a `flagprobe.py` `cl` probe and `#define`d above the block). Phase tag `// char-ara CHNb2` on every block.
- **`CX2LVUpEventMgr::LVUpEvent::InitClassChangeGuide`** (new, June `32713c10`), called in `CheckLevel` on each opened dialog right after the transcendence init
  (June `327141e0`: 32711490 / 32710710 / 32713c10; June pushes the dialog into the event list after it, March before: no reader in between).
  - **Event 2 (1ST guide):** `g_pStatic_String_ClassDescription` strings 0-3 re-wrapped to 180 with their own font (`31d471f0` = `CKTDXWordWrap::GetStrByLineBreak`).
  - **Event 4 (2ND guide):** line = `31eca340( class )`, next class = `3234b390( type, 31ef05e0( class ) + 1, line )`, its templet (`32373770`), stop if none;
    `g_pStatic_CharacterImg` picture line - 1 shown, `g_pButton_Accept_Quest1` / `g_pButton_Buy_CashItem1` mouse-up (`+0x27c`) = 1541 / 1545 + line - 1
    (June's switch, lines 1-4), `g_pStatic_String_ClassName` string 0 = the next class's name; then, with the UI manager's skill tree (`3214f050`),
    `LoadSkillCommandDescription( next, minor, D )` (`3220bdb0`) and `32713420`.
  - **Event 6 (transcendence guide):** `ButtonClass_Change_X` button vslot `0x68` = `31cacc00` = `CKTDGUIButton::SetShow`, with false. March's `InitTranscendGuide`
    only touches that button for the class popup, so it is here.
- **`32713420`** (file-local `CreateClassSkillDescription`, `_CONST_LVUP_EVENT_CHNB2_`): `m_kSkill = m_kTranscendental` for a transcended unit (no instance-code test,
  unlike the skill tree's `328e42d0`); `g_pStatic_ClassSkillDescription` string 1 = the description, its id as dummy int 0; per SKILL_DATA row a `MajorSkillBack_%d`
  static (type background `32207560`) and `MajorSkillButton_%d` button (icon, dummy int = the row's string, mouse-over `+0x274` = 1551, mouse-out `+0x270` = 1552),
  86 px apart from ( 260, 230 ), button 3 px in; string 1 wrapped (`31d45690`, 320); then ComboTree rows (`3220d4d0`) and `32712e50`.
- **`32712e50`** (file-local `CreateClassCommandDescription`): per MAJOR_COMBO row a `MajorCommandBack_%d` button (BG_COMMAND, dummy int = the row's string, at
  ( 262, y - 7 ); no guide text, unlike `328e5290`) and a `MajorCommand_%d` static (title `32205610` at ( 268, y + 1 ), font 2, icons from x 352, 2 px apart), rows 40 px
  from y 360. Constants read with `charport_consts.py` (all qwords except 352 / 360 / 262 / 268 / 230 / 233 dwords).
- **Cases 1551 / 1552** (`UICustomEventProc`, June `@32712a25` / `@32712bf6`, return true): the sender's dialog's `g_pStatic_ClassSkillDescription` string 1 = the hovered
  icon's dummy int 0 / the description's own dummy int 0, then wrapped (`31d45690`, 320). No June Enum names: numbers with a comment. CHNb's "not ported" line now points here.
- **`CKTDXWordWrap::GetStrByLineBreakColorByChar`** (new, June `31d45690`, KTDXLIB): March has no twin (its `CutStringWithEllipse` is a word wrap plus a cut). The KR
  per-character colour wrap (March's non-EU `GetStrByLineBreakColorInX2Main` branch) with June's differences: the colour key goes in after the break even when the next
  character was already `\n`, the cut test is `i + 1 <= size` (its else branch never runs), and an optional line cap ending in "..." (both callers pass it off).
- **`X2Unit.h`:** `GetUnitClassLineByUnitClass` (`31eca340`, with `UC_*` names; Rose's 41 -> 1 / 40 -> 2 as June) and `GetUnitClassTierByUnitClass` (`31ef05e0`).
  `X2SkillNoteManager.cpp`'s `LineOfClass` forwards to the first (as CHNb did for `ClassAt`).
- **Stand-ins (one-line comments):** `GetControl` where June calls `GetStatic_LUA` / `GetButton_LUA`, because March's other classes' level-15 guides
  (`DLG_Class_Change_Guide_<X>.lua`, until their CHNb) lack `g_pStatic_String_ClassDescription` and the `_LUA` getters pop a message box; the miss is logged. The
  `3220bdb0` minor flag is June's `CX2Unit +0xe40 == 1` (Luciel's second form, not ported): false. June's extra `337676ec` test (an app global set at start-up,
  `31c18eb6`) is left out, as the skill tree's `328e8820` port did.
- **Not a JUNE_BUGS row:** `31d45690`'s `\n` test only while the width is not passed always holds (the width resets at each break), and the early colour key is read back by
  the `#C` branch, so nothing shows twice.
- **Logs (permanent, once per guide open):** `[JUNE_CHAR] CLASS GUIDE 1st: <n> class description(s) wrapped to 180` (or `no g_pStatic_String_ClassDescription`),
  `... 2nd: class <c> line <l> -> next class <n>, buttons <accept> / <buy>`, `... 2nd skills: next class <n>, description <id>, <i> skill icon(s), <r> command row(s)`.
- **Units:** `AraGuide2MA` (107, class 7, lv 14, exp 81600), `AraGuide2LS` (108, 34), `AraGuideLH` (109, 28), `AraGuideLD` (110, 31), all lv 34 at exp 1212400.
- **Hygiene:** `flagcheck --worktree` (7 files): 5 PASS, 1 PASS~ (blank line), 1 DEFINE, 0 OTHER, 0 FAIL; 1 deleted line = CHNb's own "1551 / 1552 not ported"
  comment inside the flag, replaced; 0 non-ASCII, 0 EOL, 0 token, 0 encoding. `junediverge --check` (7 files): RESULT: PASS. `enumscan --check`: 0 rows on the changed
  lines (`X2Unit.h`'s 17 rows are its own enum definition; KTDXLIB files do not preprocess in the tool). No June `.lua` staged (luabind / skillicons n/a).
- **Deploy:** `DEPLOY OK: X2_offline.exe is 26,238,464 B, dated 2026-11-11 20:54:27, md5 3b0f210899f32b842e33343cfe197c3d` (built at `c8200145` + the 7 files). `EXESTR OK: 3 of 3` (the `2nd skills` log line, `g_pStatic_ClassSkillDescription`, `MajorCommandBack_%d`, wide).
- **Play-test (2026-11-11): PASS.** `1st: 3 class description(s) wrapped to 180`; `2nd: class 34 line 3 -> next class 124, buttons 1543 / 1547` (skills: description 36200, 4 icons, 3 rows), 28 -> 118 ( 1541 / 1545, 36164 ), 31 -> 119 ( 1542 / 1546, 36182 ); Accept on the Asura guide sent `EGS_NEW_QUEST_REQ`; Buy opened 264390 (Hsien) / 264394 (Devil); 0 EXCEPTION. The other six March characters' guides: new plan `class-guides` G1 (user request).

### CHNb - Review (2026-11-11): pass, no code change (covers CHNb `db3bf791` + CHNb2 `c54b64bb`)

Opus 5.5, Ghidra read-only. Every check below was re-read from the exe bytes; nothing was taken from the Done notes.
- **`UICustomEventProc` = `327127a0`:** the range test `wParam - 1500 <= 0x35` and the byte map. 1500 / 1501 / 1502 close events 1 / 2 / 3, 1532 closes 5, 1537 closes 6, 1540 closes 4.
  The 36 cash links (1503-1531, 1533-1536, 1538, 1539, 1553) close event 2, then run `CloseFieldName( 0 )` if `g_pTFieldGame`, set `+0x31c` = wParam and call the now state's vslot `0xac`.
  1541-1544 run `32710b60` + `32344ab0( 2, 3..6 )`, 1549 runs `( 9, 0 )`, 1545-1548 run `3222a4f0( 1..4 )` and 1550 runs `( 0 )`. All of these equal the port.
  Returns: `MOV AL,1` on 1540-1552 and `XOR AL,AL` on the rest, the same as the port.
  1551 reads the hovered control's dummy int 0 (ESI = lParam), and 1552 reads the description static's own (ESI overwritten @`32712c19`). Both wrap with `31d45690( 320 )`, and both return true even when the static is missing.
- **Enums:** a script diff of the 52 `LUEMCUM_*` in C++ against June's `Enum.src.lua` and the live data036 `Enum.lua` finds 0 mismatches.
  `QT_CHANGE_JOB` 2, `CJT_JOB_FIRST..FOURTH` 3-6 and `QT_CHANGE_TRANSCENDENTAL_CLASS` 9 equal June. June's `Enum.lua` has no `UCL_` table, so the line-4 literal correctly stays a number.
- **`OpenClassChangeGuideItem` = `3222a4f0`:** menu 0x1d, line 0 -> 288319, and the templet tier test (class 10-199, then `> 99`) equal the port.
  The map `3379fcf8` was re-parsed from `32230020`'s disassembly (key stores at both `[ESP+4]` and `[ESP+8]`): 74 pairs, 0 differences against the port's 74 (scored with June's `UC_*` values).
  June stores 0 when the lookup misses. The port skips the push, which gives the same result: the open-at vector is cleared after every open.
  The state list is June's 1-6, 9, 12, 13. Its 24-26 (PVE / photozone) do not exist in March, and the port excludes TRAINING_RESULT / ARCADE_RESULT instead.
  That stand-in is correct: both derive from `CX2StateDungeonResult`, not `CX2StateMenu`, so calling `ToggleCashShop` there would hit a foreign vtable slot.
- **Class tables:** `3234b390` indexes `[ type ][ tier ][ line ]` in steps of 0x14 / 4, which the port's `[13][4][5]` matches (bounds check added).
  `GetUnitClassLineByUnitClass` against `31eca340`'s switch: 74 / 74 classes, 0 differences (Rose 41 / 133 -> 4). `GetUnitClassTierByUnitClass` = `31ef05e0`.
- **`InitClassChangeGuide` = `32713c10`:** the dispatch (2 / 4 / 6) and event 2's strings 0-3 wrapped by `31d471f0( 0xb4, font )` match.
  Event 4 matches: the user / unit / `+0x38` gate, line, tier + 1, templet, picture line - 1 shown, `+0x27c` = 0x605 + n / 0x609 + n, name from templet `+8`, skill tree `+0x2cc` -> `3214f050`, then `3220bdb0` on `g_pData +0x50`.
  Event 6 is vslot `0x68` with 0. The call site comes right after `InitTranscendGuide`.
- **`32713420` / `32712e50`:** the transcended swap (unit `+0x3c`, transcendental list not empty), the description string and dummy int, and the per-row static / picture / button creation order.
  The icon / piece textures, the dummy int (row second) and `+0x274` / `+0x270` = 0x60f / 0x610 also match. `charport_consts.py` gives 352 / 360 / 262 / 7 / 268 / 1 / 2 / 40 and 86 / 260 / 230 / 3 / 233, all as ported.
  Neither function reads its incoming ECX except as a dead tag argument, so making them file-local is sound.
- **`31d45690`:** a branch-by-branch read matches the port: the `\n` width test, the `#C` / `#Cx` skip of 7 / 2, `>= width` breaks, the limit path that inserts "..." at i + 1 and erases the rest, the `\n` insert with the next-space erase, the colour key at i + 2 and the dead `size < i + 1` branch.
- **Stand-ins:** (1) `GetControl` in place of the `_LUA` getters is kept: March's other 1ST guides lack the control, and the miss is logged.
  (2) The `+0xe40` minor flag stays false (no second form until char-luciel). Handed to `kickoffs/char-luciel/DU5.md` as a reader of the form byte.
  (3) The left-out `337676ec` test is June's behaviour: an object `31c17ed0` allocates at start-up (`@31c18eb6`), set for the whole run.
  (4) June's string-id debug format (`31c31060 +0x40`, `"%S_1_%2"`) is not ported: a dev display, as the skill-tree port left it.
- **Data:** `LevelUpEvent.lua` rows as in the CHNb note. `luabind.py check` on the four dialogs: 0 unbound, ENUM 0 / 0, RESULT: PASS.
- **Hygiene:** `flagcheck --range db3bf791^..db3bf791`: 6 PASS, 1 DEFINE, 0 OTHER, 0 FAIL, 0 deleted, RESULT: PASS.
  `--range c54b64bb^..c54b64bb`: 5 PASS, 1 PASS~, 1 DEFINE, 0 FAIL, 1 deleted (CHNb's own comment inside the flag), RESULT: PASS. No later code commit touches the flag.
  `junediverge --check-all`: RESULT: PASS (206 allow-listed). `enumscan --check` (5 files): 0 rows inside the flag. `luasrc.py ledger --check` / `core --check`: PASS.
- **Logs (CHNb2 play-test):** all 7 `CLASS GUIDE` lines are present; `HUD shop open-at-item` first 264390 / 264394; 0 EXCEPTION. `AddGroupChainQuest` unbound x1 is pre-existing (backlog line 314).
- **No debug define; full build skipped: no play-test.** CH2d is unblocked. CH6's walk now also opens both guides on its fresh units (kickoff amended).

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

### CH2a - Done (2026-11-11)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (`Always.h:12252`, defined now; its `#error` gains `..._ARA_FP`, `StateChangeOrFPCancel` needs it) and the temporary `..._SKILL_DEBUG` (`[JUNE_CHAR] ARA SKILL equip` lines; the CH2d3 review retires it). All 48 class-map rows read from `charport_adis.py ara --phase CH2a` listings and Ghidra decompiles: **19 functions written** (inside March's function under the flag, March's in `#else`), **4 new** (`ASI_SA_ARA_SHADOW_KNOT_START / WAIT / CATCH / END_CreateState`, June's registration helpers `32bce710 / 32bc8d90 / 32bae750 / 32baa280`), **25 are June = March** (12 `Init` preload lists compared by script, the six anim-end / WAIT-or-JUMP_DOWN EventProcess, Shadow Knot's START / WAIT EventProcess and WAIT `StateEnd`, Pulling Pierce's END FrameMove, the four Wind Wedge / Pulling Pierce registration helpers = March's `_CreateState`). `funcmap.tsv` rows `Char Ara CH2a` (48); `MODS.md` "Stage 7 Ara, CH2a" has the damage table; no `JUNE_BUGS` row.

| June | `CX2GUAra::` | June differs from March |
|---|---|---|
| `32bae680` `32bae6f0` | `ASI_SA_ARA_WIND_WEDGE_START_FrameMove`, `..._TEMPEST_DANCE_START_FrameMove` | cut-in, one `m_usBeforeForceGainCount` store, `CommonFrameMove`; the effect sets, attack boxes and `UseForcePower` are the state script's ( `EFFECT_SET_LIST_EX`, `USE_FORCE_POWER` ) |
| `32bac120` `32bac1b0` `32bac2d0` `32bac3a0` | Wind Wedge / Pulling Pierce / Tempest Dance START and Shadow Knot CATCH `_EventProcess` | `StateChangeOrFPCancel( next )` at anim end, the FP-cancel window with the `m_bFPCancelProbe` bracket, `CommonEventProcess` ( March's Pulling Pierce START had none ); Tempest Dance's memo tail is the stub |
| `32b91250` `32b917e0` `32b91d80` `32b92d60` | Wind Wedge END, Tempest Dance END, Shadow Knot END, Shoulder Blow `_FrameMove` | `CommonFrameMove` only |
| `32bb4a40` | `..._PULLING_PIERCE_START_FrameMove` | no `UpForcePower( 1 )`; the 0.462 s attack box passes false ( June's memo test is the stub `31fca9e0` ) |
| `32bc1ac0` | `..._FALLING_DRAGON_FrameMove` | at 0.001 s all force power is spent: `m_fPullingDragonPowerRate = GetPowerRate() x ( ( FP - 5 ) x 0.2 + 1 )` ( double ), `UseForcePower`; the used-force-power counter add is not written ( CH1q removed the member ); the memo effect set at 1.33 s never plays |
| `32bc1ef0` `32bb57a0` `32b91a90` | Shadow Knot START StateStart / FrameMove, WAIT FrameMove | no memo `UseForcePower`; the weapon mesh at 0.385 s ( March 0.667 s ); WAIT keeps the countdown, loses its effect set |
| `32bb7e10` | `ASI_A_ARA_GAIN_FORCE_FrameMove` | 0.066 s: skill templet `GetEquippedActiveSkillTemplet( 27200, true )`, level `max( 1, GetSkillLevel( 27200, false, 0, true ) )`, intake ( MP drain ) and normal damage effects, piercing as the intake's damage time; 0.297 s: `UpForcePower( FP gain x hits )` ( June inlines it, `32baad30` ) then the Open Mind buff factors, hits and handle reset |
| `32b9b820` `32b9c910` | Gain Force / Shoulder Blow `_EventProcess` | walk and dash cancels need the foot on a line ( `WALK_` / `DASH_CANCEL_AFTER_ON_LINE_`, CH1d / CH1e's macros ); Shoulder Blow keeps `SKILL_CANCEL_AFTER` |
| `32b92c90` | `ASI_A_ARA_Shoulder_Blow_Init` | 16 preloads ( March 5 ) |

**Registration.** `SetEquippedSkillFuncBySkillSlotIndex`: the RNSI base and upgrade ids are added as labels to March's cases ( Tempest Dance, Pulling Pierce, Falling Dragon, Shadow Knot, Gain Force, Shoulder Blow; Wind Wedge is the switch `default:` in June and March ), the Shadow Knot hyper state also gets `StateInit` ( June's helper fills both ). `InitAquiredActiveSkillState` gains June's three cases ( `32bd34c0`: Pulling Pierce, Wind Wedge, Shadow Knot; `SetSkillLevelStateData`, the chain pointer from `eUpgradeSwappedSkillID` on every state but the last ). `X2SkillTree.h`: `RNSI_SA_ARA_FALLING_DRAGON` 27002, `_TEMPEST_DANCE_UPGRADE` 27029, `_FALLING_DRAGON_UPGRADE` 27031.

**Checklist** `(Ara: CH2a ...)`: §4 l.178, the class-switch default row, the types 14-16 row ( 14 class-7 ids bound ). §7ai: none of the 19 functions calls `SetBuffFactor...`; 7h / `NF`: the walk / dash lines above; `NUM use`: the damage effects' numerical blocks ( `plan_numerical_rows.tsv`: 13 rows ) are June data, read through the shipped engine, nothing to write.

**Findings, none blocking.** (1) Defining `_SKILL` also activates CH1h2's three skill-passive blocks ( Get Blood, Raid, Mana Drain ) in `AttackResultByType`; they only fire for skills later phases port. (2) Falling Dragon, Tempest Dance and Shadow Knot's memo branches are June's always-false stub, not written ( Rena's precedent ). (3) Shoulder Blow's memo effect set is the state script's, not C++'s: it ran in the play-test ( `Effectset_A_ARA_SHOULDER_BLOW_MEMO` and its impact ), so the script gates it, not June's stub.

**Gates.**
- `flagcheck JUNE_CHAR_ARA_SKILL --with JUNE_CHAR_ARA_SKILL_DEBUG --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2SkillTree.h KTDXLIB/Always.h`: `2 PASS ( +448 / +6 ), 1 DEFINE, 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL`, `3 deleted line(s)` ( the `//#define` and the `#error` pair in `Always.h` ), `0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` the three code files: `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s) remain`. `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing`, STATE `37 compared, 0 wrong`, `RESULT: PASS`. `skillicons.py check` ids 27000-27004 / 27029-27033 / 27200-27201 / 27220-27221: `skills 14, misses 0`, `RESULT: PASS`.
- `DEPLOY OK: X2_offline.exe is 26,219,520 B, dated 2026-11-11 13:43:01, md5 7a99c94e75a5c128be9b69cf5726ae74, equal to X2/US_SERVICE/x2.exe (2026-11-11 13:42:54); built at main e43b8e2c + 4 uncommitted code file(s).` The first build failed on `m_ForcePowerSystem` ( CH1q removed the member under `_FP` ); Falling Dragon's counter add is dropped with a comment.
- `EXESTR OK: 1 of 1 found in X2_offline.exe, 26219520 B, md5 7a99c94e` ( wide `ARA SKILL equip skill` ).

**Play-test hand-off.** Unit 96 `MartialArtistT` ( Ara, class 7, lv 99, not transcended ). Set A is already slotted: A Gain Force ( Enhanced, 27220 ), S Tempest Dance ( Enhanced, 27029 ), D Pulling Pierce ( Enhanced, 27030 ), C Falling Dragon ( Enhanced, 27031 ). For Wind Wedge ( Enhanced 27032 ), Shadow Knot ( Enhanced 27033 ) and Shoulder Blow ( 27201 ) say "swap" and I move them into set A ( no slot 4 / 9, no bank B ). Dev console ( END key ): `/invin 0`, then `/stage 1`. Each skill twice, enemies in front: June's motion and effect play, the slot shows a cool time after the first cast, the MP bar drops, a second cast during the cool time is refused ( unless the dev gate lets it through ). Falling Dragon spends all your force power and hits harder the more you had; the other costs and gains come from each state's script, watch the force-power orb. Then `python tools/logs.py --grep "ARA SKILL"` ( `equip` lines ), `--grep "NUM use"` ( the damage effects' numbers ) and say "check logs".

**Play-test (2026-11-11, run 13:44:49-13:51 on md5 7a99c94e; user asked for the logs, on-screen cool time / MP not reported).** Unit 96: `ARA SKILL equip` 27220 / 27029 / 27030 / 27031 ( bank A slots 0-3, states 49 / 51 / 53 / 55 ), 27032 / 27033 / 27201 ( bank B slots 0 / 1 / 2, states 59 / 61 / 63 ). Casts seen: Pulling Pierce x4, Tempest Dance x2, Falling Dragon x5, Wind Wedge x2, Shadow Knot x1 ( weapon mesh hit, Stiff and Final effect sets ), Gain Force ( intake and normal damage effects hit ), Shoulder Blow ( memo effect set and impact ). ErrorLog: `KEM_ERROR` 0, `Motion Error` 0; only the unrelated `AddGroupChainQuest` and TutorSystem lines. `NUM use` lines: none ( only `JUNE_SKILL LOAD` carries the tag ). Not played: the hyper casts and the base ids 27000-27004 / 27200 / 27201 as A-bank skills ( the CH2d3 review or CH6 ).

### CH2b0 - Done (2026-11-11)

**What landed** (Opus 5.5; Ghidra read-only; flags `SERV_IRUHADEV_JUNE_CHAR_ARA_LOCKON` and `SERV_IRUHADEV_JUNE_NPC_ESCAPE_FROM_LOCK_ON`, both defined):
- **`CX2GUAra::SLockOnSystem::ProcessLockOn`** (`X2GUAra.cpp`, ~:8710): June `3200fa20` whole under `_LOCKON`, March in `#else`. June's loop order:
  team; `GetIsBosRaidNPC` unless the NPC is `NUI_RAID_RED_DRAGON_PHASE3_2` (`32757e60( 0x835 )` = `IsUnitID`); slot 94 `GetInvincible( AT_SPECIAL )`
  (NPC `GetInvincibleByAttackTypeId`, Aisha's); `GetNowHp` (June inlines March's); NPC class GATE / BOX / TRAP / CHECKER; NPC `GetEscapeFromLockOn`;
  the candidate `CanDown( pos, LINE_RADIUS, NULL, NULL, NULL, false, false, true, false )` (`@3200fcad`) that clears `bEveryCandidateOverLine`;
  `GetIsNowPVPCorrectionGame()` (`322cc1a0`) -> skip when `m_fFacingLineRange < GetDistanceFromDirLineXZ( owner pos, owner GetDirVector, pos )`;
  the PvP range in `XS_PVP_GAME` (June also tests its state 23 `XS_PVE_GAME`, which March lacks; March's 23 is `XS_ARCADE_ROOM`, so it is not
  tested); the March erase / pick; the final `CanDown( dest, LINE_RADIUS, NULL, NULL, &lineIndex, false, false, true, false )` (`@3200ff98`), with
  the safe position written only when the line is `LT_NORMAL`, enabled and the loop flag held.
- **`SLockOnSystem::m_fFacingLineRange`** (`X2GUAra.h`): June's `+0x30` (`32b4cf30`), 99999.0 (`33441c60`), `Get/SetFacingLineRange`. June's
  Swallow Assault Ready (`32b9b3c0`) rewrites it with the same value before `ProcessLockOn`; CH3a calls the setter.
- **`GetDistanceFromDirLineXZ`** (`KTDXLIB/KTDXCommonFunc.h`, after `GetDirVecToDegree`): June `320701e0`, traced from the disassembly (Ghidra's
  decompile is wrong): `| sin( ( GetDirVecToDegree( P1 - P3 ).y - ( GetDirVecToDegree( -dir ).y + 180.0 ) ) * 0.017453292f ) * xz|P3 - P1| |`, the
  xz distance from the owner's facing line. Its other callers (`32295cc0`, Elesis `32b7ad80`) widen its `#if` when ported.
- **NPC key `ESCAPE_FROM_LOCK_ON`** (`X2GUNPC.h` / `.cpp`): June `+0x8fb6`, read in `327a0fa0 @327abb70`, cleared by the ctor (`@327633f2`);
  `m_bEscapeFromLockOn` / `GetEscapeFromLockOn`. 17 June NPC scripts set it (ERDA_MID_*, Raid_Buff1, ...).
- **Enum**: `NUI_RAID_RED_DRAGON_PHASE3_2 = 2101` in `X2UnitManager.h` under `SERV_IRUHADEV_JUNE_ENUM_NAMES` (June `Enum.src.lua:4079`).
- **Plan correction**: the landing helper `32bc0420` is Specter of Thunder's (CH2d), as CH0 found; not ported here.
- **char-luciel CH3c20 amended to "hoist only"**: June's `ProcessLockOn` is one function shared by Ara and Lu/Ciel (callers `32b07a40`, `32b15290`), so
  its body now lives once, here; CH3c20 moves the struct out of `CX2GUAra` without transcribing it again.
- funcmap rows `Char Ara CH2b0` (5); `JUNE_BUGS.md` 604 (one off-line enemy anywhere blocks the safe-pos update, kept verbatim); checklist 7ac marked.

**Routed (scope rule).** The other June readers of the NPC byte (`CX2Game` picks `322d0f70` ... `322dadd0`, the stand-in at `X2Game.cpp:16010`,
NPC `32771b80` / `32773cd0` / `32774750`; Elesis `32b7ad80` comes with char-elesis): backlog Stage 2 bullet "NPC `ESCAPE_FROM_LOCK_ON` in the other
lock-on picks". `MODS.md` (its `+0x8fb6` "not identified" line) is updated at Z.

**Gates.**
- `flagcheck JUNE_CHAR_ARA_LOCKON --worktree --with JUNE_NPC_ESCAPE_FROM_LOCK_ON` (7 code files): 2 PASS (`X2GUAra.cpp` +115, `.h` +10), 1 PASS~
  (`KTDXCommonFunc.h`, one blank line before the block), 1 DEFINE, 3 OTHER (`X2GUNPC.*` = the NPC flag, `X2UnitManager.h` = `JUNE_ENUM_NAMES`), 0 deleted,
  `RESULT: PASS`. `flagcheck JUNE_NPC_ESCAPE_FROM_LOCK_ON`: 2 PASS, 1 ERROR = the `Always.h` guard evaluation (turning the NPC flag off alone trips
  `_LOCKON`'s `#error`, as intended; same class as CH1's). `flagcheck JUNE_ENUM_NAMES --files X2UnitManager.h`: PASS.
- `junediverge.py --check` the six code files: `allow-listed: 4`, `RESULT: PASS`. `enumscan.py --check X2GUAra.cpp X2GUNPC.cpp`: 0 A/B rows.
- Probes (`/c /Zs /EHsc`): `X2GUAra.cpp` 0 errors, `X2GUNPC.cpp` 0 errors, and a probe that `#error`s unless both flags are defined and uses
  `GetDistanceFromDirLineXZ` and the new enum: 0 errors.
- Full build skipped: no play-test (the callers are Swallow Assault's Ready `:17529` and Attack `:17741` states; CH2b / CH3a's play-test exercises them).

**How to see it in game** (CH3a): Swallow Assault picks a random enemy within 1200 (dungeon), re-picks while force power lasts; with an
`ESCAPE_FROM_LOCK_ON` NPC (e.g. a raid buff totem) in range, it is never picked. No log line of its own.

### CH2b0 - Review (2026-11-11): pass, no code change

Checked against the exe bytes (Ghidra `disassemble_function`, read-only; `charport_consts.py` for operand widths; `xorstrings.tsv`). No debug define to retire.
- **`ProcessLockOn` `3200fa20`.** Filter order @`3200faf1`-`3200fbe0`: team (`+0x278` protected); slot 0x3c8 `== 1` -> skip unless `+0x264 == 1` and
  `32757e60( 0x835 )` (C++ `GUT_NPC` + `IsUnitID( NUI_RAID_RED_DRAGON_PHASE3_2 )`, 2101 = June `Enum.src.lua:4079`, no March member at 2101); slot 0x178 with
  `[esp+0x24] = 1` by pointer (user = slot 0x180, NPC = the attack-type list, funcmap 3953) = `AT_SPECIAL` (1); `+0xc04` null passes, `+0x20` null skips,
  `fldz; fcomp; test ah,1` keeps NaN = March `GetNowHp() <= 0.f`; NPC class 100-102 / 106 = `NCT_THING_GATE` / `BOX` / `TRAP` / `CHECKER`; `+0x8fb6 == 1`.
  Flag `[esp+0x17]`: set @`3200fa8b`, cleared from AL @`3200fcb6` after the candidate `CanDown` (world / line map null -> untouched), tested @`3200fffd`.
  `322cc1a0` branch: pushes = candidate pos, owner slot 0x16c (`GetDirVector`, funcmap 4797), owner pos; `fld [this+0x30]`, `fld result; fcompp; test
  ah,0x41; jz 3200fdc5` (a continue) = skip when distance > `+0x30`, NaN kept: matches `m_fFacingLineRange < dist`. States `9` (`XS_PVP_GAME`, March 9)
  and `0x17` (June's own `XS_PVE_GAME`, funcmap 1529; March's 23 is `XS_ARCADE_ROOM`): testing `XS_PVP_GAME` only is right. Range test `fld dist; fld
  range; fcompp; test ah,0x41; jnz skip` = `dist < range` on the start-of-call position. Both `CanDown` lists against `(pos, radius, pOutPos, pInLineIndex,
  pOutLineIndex, bDownRadius, bIsMonster, bOnLine, bDroppable)`: @`3200fcad` `0,0,0,0,0,1,0`; @`3200ff98` `0,0,&idx,0,0,1,0` with `idx` = 0 in the reused
  arg slot; radius `30.0` dword = `LINE_RADIUS`. Line test `+4 == 0` (`LT_NORMAL`), `+8 == 1`, flag `== 1`; `pUnit` null sets neither lock state: matches.
- **`320701e0`.** Slots traced: A1 = line pos, A2 = dir, A3 = pos. Vector 1 = `( 0 - dir.x, 0 - 0, 0 - dir.z )` (y = 0 stored into A1's arg slot
  `[esp+0x4c]`, reused for vector 2's y and the distance's x*0 term); vector 2 = `A1 - A3` in xz; distance = `sqrt( (0 + dx*dx) + dz*dz )` of `A3 - A1`,
  float stores between steps; `fadd qword 180.0; fchs; fadd vToLine.y; fmul qword 0.01745329238474369` (= `0.017453292f` widened, so the float literal is
  exact); `_CIsin` then `* distance`, `fabs`, each stored as float. The C++ matches step for step (`0.f - x` keeps June's +0).
- **`32b4cf30`.** `fld dword [33441c60]` (99999.0) `fstp [eax+0x30]` @`32b4cf64`. NPC key @`327abb70`: `xorstrings` row `327aba53` = `ESCAPE_FROM_LOCK_ON`,
  `lea ebx,[edi+0x8fb6]`, `call 32fe9e40`, `test al,al; jnz; mov [ebx],al` = false when missing (same shape as `NOT_AFFECT_BLACKHOLE` @`327a5f0b`); ctor
  `mov word [esi+0x8fb6],bx` (0) @`327633f2`.

**Hygiene.** `flagcheck JUNE_CHAR_ARA_LOCKON --range a3b59faa^..a3b59faa --with JUNE_NPC_ESCAPE_FROM_LOCK_ON --files <7>`: 2 PASS, 1 PASS~, 1 DEFINE, 3 OTHER,
0 FAIL, 0 ERROR, 0 deleted, `RESULT: PASS`. `flagcheck JUNE_NPC_ESCAPE_FROM_LOCK_ON` (3 files): 2 PASS, 1 ERROR = the `Always.h:12439` dependency guard,
as in the Done note. `junediverge.py --check-all`: allow-listed 205, `RESULT: PASS`. `enumscan.py --check X2GUAra.cpp X2GUNPC.cpp`: 0 A/B rows. Probes
(`/c /Zs /EHsc`): `X2GUAra.cpp`, `X2GUNPC.cpp` 0 errors. Full build skipped: no play-test.

### CH2b - Done (2026-11-11)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (CH2a's, no new define; its `_DEBUG` equip log stays on until the CH2d3 review; no review of its own). All 45 class-map rows read from `charport_adis.py ara --phase CH2b` listings (+ the June jump tables of `32bd09d0` / `32bd34c0` read from the exe); 8 Init preload lists compared by script. **20 functions written** inside March's (`#ifdef _SKILL` June `#else` March), **3 new** (`ASI_SA_ALH_DESCENT_DRAGON_StateStart`, `ASI_SA_ALH_LINK_FLOWER_CreateState` / `_EventProcess`), **2 registration helpers edited** ( `ASI_SA_ALH_DOUBLE_COLLISION_START / _END_CreateState`: June registers no FrameMove; `ASI_SA_ALH_DESCENT_DRAGON_CreateState` gains StateStart ), the rest = March ( 8 Init lists, Suppression Start / FM / EP, White Tiger EP, Crescent / Full Moon End, 4 registration helpers ). `funcmap.tsv` rows `Char Ara CH2b` (44); `MODS.md` "Stage 7 Ara, CH2b" has the damage table; `JUNE_BUGS.md` 605.

| June | `CX2GUAra::` | June differs from March |
|---|---|---|
| `32baa2d0` `32b9ac40` | Moonlight Slash READY `_StateStart` / `_StateStartFuture` | Open Mind buff after the force power gain; charge key for slot 5 / B5 ( `pureSkill5` / `pureSkillEx5`; June's switch has 10 cases ) |
| `32bb5bb0` `32b9ad90` `32b9ae80` | READY `_FrameMove` / `_EventProcess`, CHARGE `_EventProcess` | thresholds 1.5 / 1.5 / 5 s x the skill's `SA_EFFECTIVE_RATE` ( data `+0x20`, CH1k review ) |
| `32b9ae20` | CHARGE `_FrameMove` | the animation-end test reads the now animation ( March: future ) |
| `32b9aee0` `32b9b160` | Crescent / Full Moon `_StateStart` | power rate `floor( charge x ( 1 / SA_EFFECTIVE_RATE ) [ - 2 ] ) x 0.1` |
| `32bb5f40` `32bb6410` | Crescent / Full Moon `_FrameMove` | power rate `GetPowerRate() x ( 1 + POWER_RATE_MUL x charge rate )` ( new member `sMoonLightSlashData::m_fPowerRateMul` = June `+0x24`, 1.0; written by `InitAquiredActiveSkillState` from the Full Moon Slash passive 27402 ); memo test = June's stub |
| `32b9af60` `32b9b1c0` `32b9bee0` | Crescent / Full Moon / Double Collision END `_EventProcess` | `WALK_` / `DASH_CANCEL_AFTER_ON_LINE_` ( foot on a line ) |
| `32baa460` `32bac4c0` | Double Collision START `_Start` / `_EventProcess` | the one force power moves from FrameMove ( now none ) to Start + Open Mind buff; `StateChangeOrFPCancel` + FP-cancel probe window |
| `32bb96d0` `32b928d0` | White Tiger `_FrameMove` / `_End` | only the 1.0 s face effect set stays; start / black hole sets and the force power are the script's; End = `CommonStateEnd` |
| `32bb99d0` `32b9c1d0` | White Tiger landing `_FrameMove` / `_EventProcess` | no skill buff-factor push; the landing gets walk, skill and dash cancels |
| `32bb9ec0` | Descent Dragon `_FrameMove` | power rate x `GetLearnedSkillAbilityValue( SA_DAMAGE_REL, RNSI_P_ASD_FULLMOON_SLASH )` when `m_bMoonSlash`; passed to the effect set and the damage effect |
| `32ba5870` | Link Flower ( Fire Lotus ) `_EventProcess` | new: anim end -> WAIT, walk / back walk / dash on line, `SKILL_CANCEL_AFTER` |

**Registration.** `SetEquippedSkillFuncBySkillSlotIndex`: RNSI base + enhanced ids added as labels to March's White Tiger ( 27005 / 27034 ), Double Collision ( 27006 / 27037 ), Moonlight Slash ( 27008 / 27049 ) and Suppression ( 27009 / 27050 ) cases, one new Link Flower case ( 27007 / 27038, both states by the one helper; June's `+0x11` store is dropped as the checklist says ). `InitAquiredActiveSkillState`: June's Double Collision, Moonlight Slash ( four chain states, Descent Dragon without chain, the data object with `SetEffectiveRate` and `SetPowerRateMul` ) and Link Flower cases; **the enhanced Fire Lotus 27038 has no case in June's jump table** ( JUNE_BUGS 605, kept ). `X2SkillTree.h`: `RNSI_SA_ALH_LINK_FLOWER` 27007, `_WHITE_TIGER_UPGRADE` 27034, `_LINK_FLOWER_UPGRADE` 27038, `_MOONLIGHT_SLASH_UPGRADE` 27049, `_SUPPRESSION_UPGRADE` 27050; `X2GUAra.h`: `ASI_SA_ALH_LINK_FLOWER` in `ARA_STATE_ID`. Templet-swap check: the March readers of these ids ( `SetEquippedSkillLevel`, `CanUseSpecialAttack`, the passive-rate groups ) were already June's ( CH1i / CH1k ); White Tiger landing's `GetEquippedActiveSkillTemplet( SI_SA_ALH_WHITE_TIGER )` read is gone with the push.

**Checklist** `(Ara: CH2b ...)`: §4 l.178, §7ae l.728 / 732, §7h l.426 ( `_FINAL` / `_FINAL_ULTIMATE`, Link Flower ), §2 l.117. No function calls `SetBuffFactor...` ( 7ai ): the White Tiger push is gone. Found, not ported: Swallow Assault ( 27011 ) is CH3a's.

**Gates.**
- `flagcheck JUNE_CHAR_ARA_SKILL --with ..._SKILL_DEBUG --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2SkillTree.h`: `2 PASS ( +539 / +16 ), 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL`, `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` the three files: `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s) remain`. `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing`, STATE `37 compared, 0 wrong`, `RESULT: PASS`. `skillicons.py check` 27005-27009 / 27034-27038 / 27049-27050: `skills 12, misses 0`, `RESULT: PASS`. Probe `X2GUAra.cpp` ( `/c /Zs /EHsc` ): 0 errors.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,221,056 B, 2026-11-11 15:08:06` ( md5 a87524a7 ); the first deploy was refused ( the game was running ); after the user closed it, `DEPLOY OK: X2_offline.exe is 26,221,056 B, dated 2026-11-11 15:09:09, md5 a87524a70d6f4616bca4179bed09867a, equal to X2/US_SERVICE/x2.exe ( 2026-11-11 15:08:06 ); built at main d30d489f` ( `--deploy-only` ). `EXESTR OK: 1 of 1 found in X2_offline.exe, 26221056 B, md5 a87524a7` twice ( wide `POWER_RATE_MUL`, narrow `ASI_SA_ALH_LINK_FLOWER` ).

**Play-test hand-off.** Unit 97 `LittleHsienT` ( Ara, class 28, lv 99, not transcended; backup `db_backup/els_db_20261111_145422_pre_sql.sql` ). Slotted for you: A bank ( slots 0-3 ) Moonlight Slash ( Enhanced 27049 ), Double Collision ( Enhanced 27037 ), White Tiger ( Enhanced 27034 ), Suppression ( Enhanced 27050 ); B bank ( slot 5 ) Fire Lotus ( Enhanced 27038 ). Dev console ( END key ): `/invin 0`, then `/stage 1`. Each skill twice, enemies in front: June's motion and effect, a cool time on the slot after the first cast, the MP bar drops, a second cast during the cool time is refused. **Moonlight Slash:** tap = Crescent attack, hold past 1.5 s = Full Moon attack ( release ), hold past 5 s fires by itself; the charge effect set plays at 0.001 s and the "2 second" set at 1.5 s. **Double Collision:** one force power at the start; cast, then press another skill key in the window after 0.73 s ( FP cancel ). **White Tiger:** lands, the landing buff effect and shock wave hit. **Suppression:** spends all force power, the ground effect then the down force. **Fire Lotus:** two hits with a fly-up, walk / dash cancel after 1.2 s. The Descent Dragon secret skill and the base ids ( not learned ) are not played. Then `python tools/logs.py --grep "ARA SKILL"` ( five `equip` lines ), `--grep "NUM use"` ( Fire Lotus tables 1 / 2 and the damage effects' numbers ) and say "check logs".

**Play-test (2026-11-11, run 15:10-15:12 on md5 a87524a7; user asked for the logs, on-screen cool time / MP not reported).** Unit 97: five `ARA SKILL equip` lines ( 27049 / 27037 / 27034 / 27050 bank A slots 0-3, states 49 / 51 / 53 / 55; 27038 bank B slot 0, state 59 ), twice ( two stage loads ). Casts seen: Moonlight Slash held ( `Effectset_MoonLight_Slash_Charge`, `_2Second`, `_Full`, `_Fullmoon_Attack` at 15:11:22-24 ) and tapped twice ( `_New`, `_Crescent_Attack` ), White Tiger x4 ( `Power_of_WhiteTiger_BlackHole` from the script, then `_Landing` 0.5 s later ), Double Collision x2 ( `EffectSet_Ara_Double_Collision_02` / `_04`, flight damage ), Suppression ( `EffectSet_ASD_SUPPRESSION`, `MP_CRASH mesh_SI_SA_ASD_SUPPRESSION_Wave` hit ), Fire Lotus x6+ ( `EffectSet_SA_ALH_LINK_FLOWER_Ultimate` = the `_FINAL_ULTIMATE` list, `_DeBuff` and `_DeBuff_Explosion` hits ). ErrorLog: `KEM_ERROR` 0, `Motion Error` 0; the `ErrorEnum(433)` face-frame lines are JUNE_BUGS 603; `OpenXET` misses ( 454 in the run, also `ALH_LINKFLOWER_*`, `SI_SA_ASD_SUPPRESSION_*`, village maps ) are the optional trace file the mesh loader tries beside every `.X`, the meshes themselves are staged byte-exact ( `ch0_assets.tsv` ). `NUM use` lines: none ( only `JUNE_SKILL LOAD` carries the tag, as CH2a ). `AddGroupChainQuest` and the two `E_DISABLE_HB_CHECK_REQ` lines are not CH2b's. Not played: FP cancel out of Double Collision, Descent Dragon, the base ids, the Full Moon Slash passive ( 27402, `POWER_RATE_MUL` stays 1.0 ).

### CH2b2 - Done (2026-11-11)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (CH2a's, no new define; its `_DEBUG` equip log stays on until the CH2d3 review; no review of its own). All 28 class-map rows read from `charport_adis.py ara --phase CH2b2` listings; 7 Init preload lists compared by script (all March's). **10 functions written** inside March's (`#ifdef _SKILL` June `#else` March), 0 new; the other 18 rows = March (7 Init lists, 7 state functions with the same calls and constants, 4 registration helpers). `funcmap.tsv` rows `Char Ara CH2b2` (28); `MODS.md` "Stage 7 Ara, CH2b2" has the damage table. No `JUNE_BUGS` row: nothing in this phase is a June bug (the memo tests are June's always-false stub, the same as CH2a / CH2b).

| June | `CX2GUAra::` | June differs from March |
|---|---|---|
| `32b92300` | Tiger Claw START `_StateStart` | no memo count on the FP-cancel gain ( June's memo test is the always-false stub `31fca9e0` ) |
| `32bac430` `32bac6d0` `32bac7f0` | Tiger Claw START / Rock Crash START / Fall START `_EventProcess` | animation end -> `StateChangeOrFPCancel` ( END / END / FINISH ), then the FP-cancel probe window ( as CH2b Double Collision ) |
| `32b9b740` | Tiger Claw END `_EventProcess` | dash cancel needs the foot on a line ( `DASH_CANCEL_AFTER_ON_LINE_` ) |
| `32b9c410` | Rock Crash END `_EventProcess` | walk and dash cancels need the foot on a line |
| `32baa560` | Fall START `_StateStart` | the Open Mind buff ( 27417 ) follows the one force power |
| `32b9cb10` | Fall FINISH `_EventProcess` | walk and dash cancels need the foot on a line ( skill cancel as March ) |
| `32b92f90` | Quick Thrust `_FrameMove` | `CommonFrameMove` only: `USE_FORCE_POWER { 0.001, 1 }` and the effect set are the script's |
| `32b9cd20` | Quick Thrust `_EventProcess` | animation end -> WAIT, on-line walk cancel, skill cancel, on-line dash cancel; March's jump-down test is gone |

**Registration.** `SetEquippedSkillFuncBySkillSlotIndex`: the RNSI base + enhanced ids added as labels to March's cases ( Rock Crash 27202 / 27223, Tiger Claw 27203 / 27229, Fall 27204 / 27230, Quick Thrust 27205 / 27231 ); the four June registrations equal March's ( same callbacks per state, hyper state a copy ). `InitAquiredActiveSkillState`: labels on March's Rock Crash and Tiger Claw cases, and their chain lookups take `eUpgradeSwappedSkillID` ( June `@32bd35a1` / `@32bd3686`; the March line stays in `#else` ); June has no case for Fall or Quick Thrust there. `X2SkillTree.h`: `RNSI_A_ALH_TIGER_CLAW_UPGRADE` 27229, `_DRAGON_FALLING_UPGRADE` 27230, `_QUICK_STAB_UPGRADE` 27231 ( June `Enum.lua` values ). Templet-swap check: every reader of these ids ( `SetEquippedSkillLevel`, the Power-of-Tiger group, the FP-cancel cost switch, `UseForcePower` ) already switches on the upgrade-base id ( CH1i / CH1j / CH1k / CH1q ), so the three new names need no entry there. Effect closure: every effect set the six C++ literals and the four scripts name is `june-bytes` in `ch0_effect_closure.tsv`; nothing new to stage.

**Checklist** `(Ara: CH2b2 ...)`: §2 l.117, §4 l.178, §7o l.522-526 ( Quick Thrust's two `SPEED_TIME_ULTIMATE` rows: no C++, the script carries them ). Found, not ported: nothing.

**Gates.**
- `flagcheck JUNE_CHAR_ARA_SKILL --with ..._SKILL_DEBUG --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2SkillTree.h`: `1 PASS ( +165 ), 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL`, `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` the two files: `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s) remain`. `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing`, STATE `37 compared, 0 wrong`, `RESULT: PASS`. `skillicons.py check` 27202-27205 / 27223 / 27229-27231: `skills 8, misses 0`, `RESULT: PASS`. Probe `X2GUAra.cpp` ( `/c /Zs /EHsc` ): 0 errors.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,220,544 B, 2026-11-11 15:35:18`; `DEPLOY OK: X2_offline.exe is 26,220,544 B, dated 2026-11-11 15:35:25, md5 14de03bf5e5290c4e6921fbf180c4fd9, equal to X2/US_SERVICE/x2.exe (2026-11-11 15:35:18); built at main e18487a0 + 2 uncommitted code file(s).` `EXESTR OK: 2 of 2 found in X2_offline.exe, 26220544 B, md5 14de03bf` ( narrow `ASI_A_ALH_ROCK_CRASH_FALLING_START`, `ASI_A_ARA_QUICK_STAB`; the phase adds no new string, so this proves the deployed exe is the built one only ).

**Play-test hand-off.** Unit 97 `LittleHsienT` ( Ara, class 28, lv 99, not transcended; backup `db_backup/els_db_20261111_152550_pre_sql.sql` ). Slotted for you: A bank slot 0 Rock Crash ( Enhanced 27223 ), slot 1 Tiger Claw ( Enhanced 27229 ); B bank slot 5 Fall ( Enhanced 27230 ), slot 6 Quick Thrust ( Enhanced 27231 ). The CH2b special actives are unlearned and unslotted ( level 0 ), the base ids are not learned. Dev console ( END key ): `/invin 0`, then `/stage 1`. Each skill twice, enemies in front: a cool time on the slot after the first cast, the MP bar drops, a second cast during the cool time is refused. **Rock Crash:** falling slam, the start effect set at once and the attack box in the middle, the end effect set on landing; walk / dash cancel late in the end state only on the ground. **Tiger Claw:** two claw hits ( the first effect at 0.4 s, the second state's at its start ); press another skill key during the cancel window of the first state ( FP cancel ). **Fall:** the force power bar goes up by one at the start, then the finish state on landing. **Quick Thrust:** a dash thrust, one force power spent at the start ( have at least one ), the effect set `EffectSet_Ara_A_Sinsok_Attack`; it must not jump down on its own at the end. Then `python tools/logs.py --grep "ARA SKILL"` ( four `equip` lines ) and say "check logs"; I read ErrorLog for `KEM_ERROR` / `Motion Error` and the four skills' effect sets by name.

**Play-test (2026-11-11, run 15:40-15:42 on md5 14de03bf; user asked for the logs, on-screen cool time / MP not reported).** Unit 97: four `ARA SKILL equip` lines ( 27223 slot 0 bank A state 49, 27229 slot 1 bank A state 51, 27230 slot 0 bank B state 59, 27231 slot 1 bank B state 61 ). Casts seen: Tiger Claw x2 ( `EffectSet_Ara_ATTACK_BOX_Tiger_Claw_1st` / `_2nd`, hits on `DAMAGE_EFFECT_SI_A_ARA_Tiger_Claw_1st` / `_2nd`, second state 161 ), Rock Crash x2 ( `EffectSet_ALH_DOUBLE_COLLISION_ATTACKBOX`, hits `Rock_Crash_1` / `_2` / `Flight_Damage_02` / `Flight_Damage` ), Fall x4 ( `EffectSet_ASD_DRAGON_FALLING` then `_FINISH` state 205, hits `Dragon_Falling_UP` / `_UP_STOP` / `_DOWN` / `_DOWN_DAMAGE` / `_Explosion` ), Quick Thrust x1 seen ( `EffectSet_Ara_A_Sinsok_Attack`, 5 unit-to-unit hits ). ErrorLog: `KEM_ERROR` 0, `Motion Error` 0; the 16 `ErrorEnum(433)` face-frame lines are JUNE_BUGS 603. `NUM use` lines: none ( as CH2a / CH2b ). The `JUNE_FX unsupported APPLIED_SKILL_ID ... system absent` lines ( `X2DamageEffect.cpp:1708`, `X2EffectSet.cpp:4068` ) are the engine's old informational log for every June damage effect with that key, not this phase's. `AddGroupChainQuest` and the two `E_DISABLE_HB_CHECK_REQ` lines are not CH2b2's. Not played / not proven by the log: the FP cancel out of Tiger Claw / Rock Crash / Fall, a second Quick Thrust cast, the base ids ( not learned ).

### CH2c0 - Done (2026-11-11)

**What landed** (Opus 5.5; Ghidra read-only; flag `SERV_IRUHADEV_JUNE_CHAR_ARA_STACK`, defined; its `#error` guard names the five flags it needs):
- **Plan correction.** The kickoff called `32bbb050` / `32bbb720` "both new in March". They are not: CH0's map is right, they are March's own
  `ASI_SA_ALD_WOLF_BREAK_FrameMove` / `ASI_SA_AYR_OPEN_THE_HELL_FrameMove`. The two new functions are their callees `3229da40` and `3236a2f0`.
- **`CX2GameUnit::AddBuffAccumulationCountByTempletID( BTI, iDelta )`** (`X2GameUnit.cpp`, after `AddBuffAccumulationCount`): June `3229da40`. It finds
  the buff by templet id. If count + delta > 0 (signed), the templet adds the delta. Otherwise the buff is erased (`EraseBuffTempletFromGameUnitByIterator`).
- **`CX2BuffTemplet::AddAccumulationCountNow( iDelta, pGameUnit )`** (`X2BuffTemplet.cpp`, after `SetAccumulationCountNow`): June `3236a2f0`. The stack
  count goes up by the delta (nothing happens if the templet has no count). Every displayer starts again, and the icon is pushed through
  `CX2GageManager::PushBuff` with the new count (june-hud H3's 4-argument overload, so `X2GageManager.*` needed no change). Unlike the set path, it does not
  re-apply behaviors and has no icon-text hook. This is June's own shape.
- **Demigod block in both FrameMoves** (`X2GUAra.cpp`, right after `CommonFrameMove`): June's bodies are March's plus one new block, inserted
  under the flag (flag off = March). If `RNSI_P_AYR_DEMIGOD` (27418) is learned, then at each of eight times where `HaveSkillBuff( 27418, 0, true )` holds,
  one stack is spent (`AddBuffAccumulationCountByTempletID( GetSkillBuffTempletID( 27418, 0, true ), -1 )`) and
  `EffectSet_P_AYR_GOOD_ENERGY_BALL_Wolf_Break` / `_Open_The_Hell` plays at `GetPowerRate()`. Wolf Break's times are 1.2-1.9 s and Open the Hell's are
  0.75-1.1 s (exe floats, `charport_consts.py`). The rest of both functions is byte-for-byte June's already. June has no `GetEffectSet` null test in this
  block, so none was added. Both effect sets are in the live `data036` `EffectSet.lua`.
- **Enum**: `RNSI_P_AYR_DEMIGOD = 27418` in `X2SkillTree.h` under `SERV_IRUHADEV_JUNE_ENUM_NAMES` (June `Enum.src.lua:15134`; March's own Ara passives
  sit at 74xx, so the value is free).
- funcmap rows `Char Ara CH2c0` (4). Checklist 7at marked. No `JUNE_BUGS.md` row (nothing to keep). `CH2c.md` / `CH3b.md` amended: both FrameMoves are
  June's already, so neither phase transcribes them again.

**Reach.** Nothing in the tree applies Demigod's buff yet, so `HaveSkillBuff` stays false and the new block is inert until June's passive code lands
(CH4a / CH4b). Wolf Break is class 31's (CH2c) and Open the Hell is Yama Raja's (119, CH3b). Both already run on March's states, so their play-tests
cover the unchanged part.

**Gates.**
- `flagcheck JUNE_CHAR_ARA_STACK --worktree` (7 files): 4 PASS, 1 PASS~ (`X2GUAra.cpp` +34, the blank line before each block), 1 DEFINE, 1 OTHER
  (`X2SkillTree.h` = `JUNE_ENUM_NAMES`, its own flagcheck PASS); `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s),
  0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` (7 files): `allow-listed: 5`, `RESULT: PASS`. `enumscan.py --check` the three `.cpp`: `0 A/B row(s) remain`.
- Probes (`/c /Zs /EHsc`): `X2GUAra.cpp`, `X2GameUnit.cpp`, `X2BuffTemplet.cpp`: 0 errors each. A `cl /P` check shows both effect-set names, both new
  function definitions and the enum member in the preprocessed output.
- Full build skipped: no play-test.

### CH2c0 - Review (2026-11-11): pass, no code change

Checked against the exe bytes (`charport_adis.py ara`, `charport_consts.py --fn`, read-only). No debug define to retire.
- **`3229da40`.** `322ae0a0( begin +0xe14, end +0xe18, BTI )` compares `[templet + 0x34]` (the templet id) only, so it uses the same predicate as the lambda.
  `32369da0` returns an int (`fistp`, truncating), then `add ecx, eax; test ecx, ecx; jle` = signed `count + iDelta > 0`. The pass path is
  `3236a2f0( iDelta, this )` and the fail path is `3229d320( &it )`, `RET 8`. This matches line for line, and March's `UINT` getter cast to `int` gives the same sum.
- **`3236a2f0`.** `+0x58` size test, then the `+0x50` find with key 0, then end test `+0x54`. `fild iDelta; fadd [node+0x10]; fstp` = `+= (float)iDelta`.
  The displayer loop calls slot 1 `( unit, this )` on each non-null entry, then `+0xc = 1`. `+0xa8 == 1` builds `BuffIcon( +0x34, &+0x68, &+0x84, +0xa0, +0xa4 )`
  (= `GetBuffTempletID`, `GetIconFileName`, `GetIconKeyName`, `GetBuffName`, `GetBuffDesc`), then `PushBuff( unit slot 0x54 UID, icon, +0x64, GetAccumulationCountNow )`.
  There is no behavior loop and no `0x788` call. The port matches, and it has the same shape as the reviewed `SetAccumulationCountNow` tail.
- **`32bbb050` / `32bbb720`.** The gate is `31fceab0( 0x6b1a ) > 0` (= `GetSkillLevelValue`, the only overload while `_DIVERGE_SKILL_LEVEL_VALUE` is on). Before
  each of the eight times: slot `0x240( 27418, 0, 1 )`. Times: `1.2`-`1.9` and `0.75`-`1.1` (`1.0` = `fld1` before a `31e4c230` call). The first three of each
  function are inlined `EventTimerOneshot` calls (`fcom [anim+0x24c]` / `fcomp qword`), and the rest go through `31e4c230`. Then `3229a3d0( 27418, 0, 1 )` -> `3229da40( BTI, -1 )`.
  `PlayEffectSet` is called as `( name, this, NULL, now-state +0x10 byte = IsHyperState, slot 0x204 GetPowerRate, -1.0, (1,1,1), false, 0-vec x3, 1.0 )`: the header defaults. In both functions the March
  half is June's unchanged: Wolf Break `1.1` / `1.35` / `1.63` / `2.0`, count `+0x96e0` cases 1 -> `Break2` and 3 -> `Break3`, `+0x96d8` > random -> `_Special` with
  `+0x96d4`, `Bip01` +-`100.0`, `landPosition.y`. Open the Hell: `0.01`, `GetEffectSet` null test, `SA_AYR_OPEN_THE_HELL_ALL`, `GetPowerRate * +0x96dc`.

**Hygiene.** `flagcheck JUNE_CHAR_ARA_STACK --range 104bf253^..104bf253 --files <7>`: 4 PASS, 1 PASS~ (`X2GUAra.cpp`, 2 blank lines), 1 DEFINE, 1 OTHER
(`X2SkillTree.h` = `JUNE_ENUM_NAMES`), 0 FAIL, 0 ERROR, 0 deleted, `RESULT: PASS`. `junediverge.py --check-all`: allow-listed 205, `RESULT: PASS`.
`enumscan.py --check` the three `.cpp`: `0 A/B row(s) remain` (D 4 = leads). Probes (`/c /Zs /EHsc`, census `cl_cmd` defines): `X2GUAra.cpp`,
`X2GameUnit.cpp`, `X2BuffTemplet.cpp` 0 errors. Full build skipped: no play-test.

### CH2c - Done (2026-11-11)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (CH2a's, no new define; its `_DEBUG` equip log stays on until the CH2d3 review; no review of its own). All 36 class-map rows read from `charport_adis.py ara --phase CH2c` listings; the 9 Init preload lists compared by script (all March's). **14 functions written** ( 9 inside March's under `#ifdef` June `#else` March, 5 new ), the other 22 rows = March ( 9 Init lists, 4 Start functions, Wolf Break's event process, 8 registration helpers ). `funcmap.tsv` rows `Char Ara CH2c` ( 36 ); `MODS.md` "Stage 7 Ara, CH2c" has the damage table. One `JUNE_BUGS.md` row ( 606 ).

| June | `CX2GUAra::` | June differs from March |
|---|---|---|
| `32b93540` `32b93570` | new `ASI_SA_ALD_ENERGY_WAVE_StateStartFuture` / `_StateStart` | the held key is set to the cast slot ( `m_kSkillSlotKey.Set( m_iNowSpecialAttack - 1 )`, CH1p's key ); Start zeroes new `m_fEnergyWaveChargeTime` ( `+0x9714` ) |
| `32b9dd20` | `ASI_SA_ALD_ENERGY_WAVE_EventProcess` | `FollowSlotSwap`, then the animation end changes to `ASI_SA_ALD_ENERGY_WAVE_FIRE` ( March's body ran the end of the skill here ) |
| `32bce770` `32bc2900` `32b9dd60` | new `ASI_SA_ALD_ENERGY_WAVE_FIRE_CreateState` / `_FrameMove` / `_EventProcess` | the fire state: at 0.01 s `EffectSet_SA_AHD_ENERGY_WAVE_PROJECTILE` ( `_Devided` while `RNSI_P_AYR_DEMIGOD` is learned ) at ( `SA_DAMAGE_REL` + 1 ) x power rate, scale 1 + `SA_PROJECTILE_SIZE` ( the state skill's abilities, `32007b10` ); end -> jump down with skill / on-line walk / on-line dash cancels |
| `32bacb50` | `ASI_SA_ALD_HELL_OF_TORNADO_EventProcess` | animation end -> `StateChangeOrFPCancel` ( finish ), the FP-cancel probe window ( `+0x9621` ), then skill cancel and on-line walk / dash cancels |
| `32bacdb0` | `ASI_SA_ALD_WOLF_SPEAR_EventProcess` | animation end -> `StateChangeOrFPCancel` ( finish ), the probe window, no cancel |
| `32bad100` | `ASI_SA_ALD_PRISON_SPEAR_EventProcess` | as Hell of Tornado without the skill cancel |
| `32b9df70` `32b9e190` `32b9eac0` `32b9e4c0` | Hell of Tornado / Wolf Spear / Suppression Energy / ( Energy Bead + Prison Spear ) end `_EventProcess` | the walk and dash cancels need the foot on a line ( `WALK_` / `DASH_CANCEL_AFTER_ON_LINE_` ); the rest = March |

**Registration.** `SetEquippedSkillFuncBySkillSlotIndex`: the five cases take the `RNSI` base + enhanced labels ( 27013 / 27035, 27014 / 27039, 27015 / 27040, 27016 / 27054, 27017 / 27055 ); Energy Wave's `CreateState` also sets StartFuture and Start, and its case adds the fire sub-state ( June `@32bd1903` ). The other four registrations equal March's ( same callbacks per state ). `InitAquiredActiveSkillState`: labels on Wolf Spear and Prison Spear, and their chain lookups take `eUpgradeSwappedSkillID` ( June `@32bd41fd` / `@32bd4291` / `@32bd4536`; Wolf Break has none ). `X2SkillTree.h`: five names ( 27017, 27035, 27040, 27054, 27055, June `Enum.lua` ). Templet-swap check: the readers of these ids ( `IsSkillAffectedByHowling`, the secret-skill state map, `IsSkillAffectedByHellGateOpen`, `SetEquippedSkillLevel`'s `GetEquippedActiveSkillTemplet( base, true )` ) already take the upgrade-base id, so no further term. Effect closure: every effect set the new C++ literals and the scripts name is `june-bytes` in `ch0_effect_closure.tsv`; nothing to stage.

**Checklist** `(Ara: CH2c ...)`: §2 l.117, §4 l.178 ( 5 names ), §7h l.426 ( Wolf Spear `_FINAL` sets: the shipped engine reads them ), §7ae l.728 / l.732. No `FP_CANCEL` / `ADD_FORCE_POWER` C++ beyond the probe windows above ( the script rows run through CH1q's engine ). **Found, not ported:** nothing ( June's dead `ASI_SA_ALD_ENERGY_WAVE_CHARGE` functions `32b93590` / `32b935d0` stay unported: CH0 judged them dead data, `JUNE_BUGS` 606 ). `32b9e4c0` also serves Energy Bead's finish state ( CH2c2's ): its kickoff says so.

**Gates.**
- `flagcheck JUNE_CHAR_ARA_SKILL --with ..._SKILL_DEBUG --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2SkillTree.h`: `2 PASS ( +208, +8 ), 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL`, `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`. `flagcheck JUNE_ENUM_NAMES --worktree --files X2Lib/X2SkillTree.h`: `PASS~`, `RESULT: PASS`.
- `junediverge.py --check` the three files: `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s) remain`. `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing`, STATE `37 compared, 0 wrong`, `RESULT: PASS`. `skillicons.py check` 27013-27017 / 27035 / 27039 / 27040 / 27054 / 27055: `skills 10, misses 0`, `RESULT: PASS`.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,223,616 B, 2026-11-11 16:37:46`; `DEPLOY OK: X2_offline.exe is 26,223,616 B, dated 2026-11-11 16:37:52, md5 9567d6509858b93c5399850eefbd6559, equal to X2/US_SERVICE/x2.exe (2026-11-11 16:37:46); built at main 385554ab + 3 uncommitted code file(s).`
- `EXESTR OK: 2 of 2 found in X2_offline.exe, 26223616 B, md5 9567d650` ( wide `EffectSet_SA_AHD_ENERGY_WAVE_PROJECTILE_Devided`, `EffectSet_SA_AHD_ENERGY_WAVE_PROJECTILE` ); `EXESTR OK: 1 of 1` ( narrow `ASI_SA_ALD_ENERGY_WAVE_FIRE` ).

**Play-test hand-off.** Unit 99 `LittleDevil99` ( Ara, class 31, lv 99, not transcended; backup `db_backup/els_db_20261111_162910_pre_sql.sql` ). Slotted for you: A bank slot 0 Hell of Tornado ( Enhanced 27035 ), slot 1 Wolf Spear ( Enhanced 27039 ), slot 2 Energy Wave ( Enhanced 27040 ), slot 3 Prison Spear ( Enhanced 27054 ); B bank slot 0 ( DB slot 5 ) Suppression Energy ( Enhanced 27055 ). Dev console ( END key ): `/invin 0`, then `/stage 1`. Each skill twice, enemies in front: a cool time on the slot after the first cast, the MP bar drops, a second cast during the cool time is refused. **Hell of Tornado:** a whirlwind that spends two force power at the start ( have two ); press another skill key late in the first state ( FP cancel ); the end state on landing. **Wolf Spear:** a spear thrust, then the finish state with no cancel ( Wolf Break, its secret chain Z Z Z -> Wind Wedge -> Wolf Claw -> Wolf Spear, is not part of this hand-off: Wolf Claw is CH2c2's ). **Energy Wave:** a start motion with the big camera, then a second motion that fires the wave; the projectile effect appears at the second motion's start, and you land with a jump-down. **Prison Spear:** spear throw then the finish state; walk / dash out late on the ground. **Suppression Energy:** one explosion, high damage. Then `python tools/logs.py --grep "ARA SKILL"` ( five `equip` lines ) and say "check logs"; I read ErrorLog for `KEM_ERROR` / `Motion Error` and the five skills' effect sets by name ( `EffectSet_SA_AHD_ENERGY_WAVE_START`, `EffectSet_SA_AHD_ENERGY_WAVE_END_NEW`, `EffectSet_SA_AHD_ENERGY_WAVE_PROJECTILE` ).

**Play-test (2026-11-11, run 16:39-16:42 on md5 9567d650; user asked for the logs, on-screen cool time / MP not reported).** Unit 99: five `ARA SKILL equip` lines ( 27035 / 27039 / 27040 / 27054 bank A states 49 / 51 / 53 / 55, 27055 bank B state 59; two runs ). Casts seen: Hell of Tornado ( `EffectSet_SA_ALD_HELL_OF_TORNADO_START` / `_END`, `DAMAGE_EFFECT_SA_ALD_HELL_OF_TORNADO_EXPLOSION` x11 ), Wolf Spear ( `EffectSet_SA_ALD_WOLF_SPEAR_Ultimate` + the MEMO sets x5 ), Energy Wave x2 ( the fire state, owner state 257: `EffectSet_SA_AHD_ENERGY_WAVE_PROJECTILE` created, `DamageEffect_SA_AHD_ENERGY_WAVE_PROJECTILE` hit both times ), Prison Spear ( `EffectSet_SI_SA_AHD_PRISON_SPEAR_B`, `DAMAGE_AHD_PRISON_SPEAR_B_Damage` hit ), Suppression Energy x4 ( `EffectSet_SI_SA_AHD_SUPPRESSION_ENERGY` ). ErrorLog: `KEM_ERROR` 0, `Motion Error` 0; the four `OpenXET` probes ( `PRISON_SPEAR_WEAPON` / `_EFFECT`, `SA_AHD_SUPPRESSION_ENERGY_P05`, `SA_ALD_HELL_OF_TORNADO_DUMMY_FX` ) are the sidecar probe every mesh logs ( 422 lines this run ). Not CH2c's: `AddGroupChainQuest`, `E_DISABLE_HB_CHECK_REQ`, the `APPLIED_SKILL_ID` stand-in lines. Not proven by the log: the FP cancel out of Hell of Tornado / Prison Spear, the `_Devided` set ( Demigod is not applied until CH4 ), cool times and the MP bar.

### CH2c2 - Done (2026-11-11)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (CH2a's, no new define; its `_DEBUG` equip log stays on until the CH2d3 review; no review of its own). All 25 class-map rows read from `charport_adis.py ara --phase CH2c2` listings plus the two second-switch cases in `32bd34c0`; the 6 Init preload lists compared by script (all March's). **10 functions written** inside March's (`#ifdef _SKILL` June `#else` March), 0 new; the other 15 rows = March (6 Init lists, Wolf Claw / Finger Bullet Start, Force Spear's event process, 6 registration helpers). `funcmap.tsv` rows `Char Ara CH2c2` (25); `MODS.md` "Stage 7 Ara, CH2c2" has the damage table. No `JUNE_BUGS` row: nothing here is a June bug (the memo tests are June's always-false stub, Conversion Health's dead level read is harmless).

| June | `CX2GUAra::` | June differs from March |
|---|---|---|
| `32bb77b0` | Force Spear `_FrameMove` | no `UseForcePower( 1 )` ( the script's `USE_FORCE_POWER { 0.3, 1 }` ); `EffectSet_Force_Spear_With_Demigod` / `DamageEffect_Force_Spear_With_Demigod` while 27418 is learned; spawn offset 40 ( March 25 ); piercing and level from 27211; no memo branches ( stub `31fca9e0` ) |
| `32bac880` | Wolf Claw `_EventProcess` | animation end -> `StateChangeOrFPCancel( ..._FINISH )`, the FP-cancel probe window ( `+0x9621` ), nothing else |
| `32b9d6d0` | Wolf Claw finish `_EventProcess` | skill cancel as March; walk and dash cancels need the foot on a line |
| `32bac910` | Conversion Health `_FrameMove` | `FP_GAIN_COUNT` ( +0.0005, truncated ) and `HP_CONSUME_REL` x max HP are the named numerical values of 27210 ( March: templet abilities ); `UpForcePower`, then the Open Mind buff ( 27417 ) |
| `32baa680` | Conversion Mana `_FrameMove` | skill = `GetTranscendenceMemoSkillID( GetNowStateSkillID() )`, level = Conversion Health's ( 27210 ), force power = `SA_EFFECTIVE_COUNT`, then the Open Mind buff; **no MP spent** ( March: `SA_MP_CONSUME_REL` ) |
| `32b9d8f0` `32b9da00` `32b9e3b0` | Conversion Health / Mana / Finger Bullet finish `_EventProcess` | walk and dash cancels need the foot on a line |
| `32bace40` | Finger Bullet `_EventProcess` | animation end -> `StateChangeOrFPCancel( ..._FINISH )`, the probe window, then on-line walk / dash cancels |
| `32baf450` | Finger Bullet finish `_FrameMove` | March's loop plus `AttackResult()` ( vslot 0x64 ) after each pulled unit's `DamageReact` |

**Registration.** `SetEquippedSkillFuncBySkillSlotIndex`: the RNSI base + enhanced ids added as labels to March's four cases ( Wolf Claw 27208 / 27224, Finger Bullet 27209 / 27234, Conversion Health 27210 / 27235, Force Spear 27211 / 27236 ); the six registration helpers equal March's ( same callbacks per state; Wolf Claw's finish has only `OnEventProcess`, March's `CreateState` stores it twice and the first store is overwritten ). `InitAquiredActiveSkillState`: labels on Wolf Claw and Finger Bullet ( June `@32bd4163` / `@32bd43b8` ), their first state's chain lookup takes `eUpgradeSwappedSkillID` ( June `@32bd4128` / `@32bd437d`; the finish states have no lookup ); June has no case there for Conversion Health or Force Spear. `X2SkillTree.h`: `RNSI_A_ALD_WOLF_CLAW_UPGRADE` 27224, `_FINGER_BULLET_UPGRADE` 27234, `_CONVERSION_HEALTH_UPGRADE` 27235, `_FORCE_SPEAR_UPGRADE` 27236 ( June `Enum.lua` values ). Conversion Mana has no label: only the transcended memo of Conversion Health reaches it, through CH1n's memo exit in the slot head. Templet-swap check: the readers of these ids ( `SetEquippedSkillLevel`, the FP-cancel cost switch, the use check `IsSameOrUpgradeSkill` ) already switch on the upgrade-base id ( CH1k / CH1q ). Effect closure: every effect set and damage effect the C++ literals and the scripts name is `june-bytes` in `ch0_effect_closure.tsv`, and both Demigod names are in the live `EffectSet.lua` / `DamageEffect.lua`; nothing to stage.

**Checklist** `(Ara: CH2c2 ...)`: §1 / §2 l.117, §4 l.178 ( 4 names ), §7ae l.728 / l.732. **Found, not ported:** Finger Bullet's pull-down needs `m_vecHitUnitPointer` filled, which is CH4a's `SetAttackResultDelegate` ( `32bb0d10`; March's still tests the March ids 7009 / `SI_A_AYR_STEAL_SOUL` ), so the finish state finds no unit until CH4a ( no crash; CH4a's kickoff owns it ).

**Gates.**
- `flagcheck JUNE_CHAR_ARA_SKILL --with ..._SKILL_DEBUG --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2SkillTree.h`: `1 PASS ( +254 ), 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL`, `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`. `flagcheck JUNE_ENUM_NAMES --worktree --files X2Lib/X2SkillTree.h`: `PASS~`, `RESULT: PASS`.
- `junediverge.py --check` the two files: `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s) remain`. `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing`, STATE `37 compared, 0 wrong`, `RESULT: PASS`. `skillicons.py check` 27208-27211 / 27222 / 27224 / 27234-27236 / 27244: `skills 10, misses 0`, `RESULT: PASS`.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,223,104 B, 2026-11-11 17:09:26`; `DEPLOY OK: X2_offline.exe is 26,223,104 B, dated 2026-11-11 17:09:33, md5 49843cb4b59684794ca6c6cef8dcc35c, equal to X2/US_SERVICE/x2.exe (2026-11-11 17:09:26); built at main 1ff24df8 + 2 uncommitted code file(s).`
- `EXESTR OK: 2 of 2 found in X2_offline.exe, 26223104 B, md5 49843cb4` ( wide `EffectSet_Force_Spear_With_Demigod`, `DamageEffect_Force_Spear_With_Demigod` ); `EXESTR OK: 1 of 1` ( wide `FP_GAIN_COUNT` ).

**Play-test hand-off.** Unit 99 `LittleDevil99` ( Ara, class 31, lv 99, not transcended; backup `db_backup/els_db_20261111_170102_pre_sql.sql` ). Slotted for you: A bank slot 0 Wolf Claw ( Enhanced 27224 ), slot 1 Finger Bullet ( Enhanced 27234 ); B bank slot 5 ( DB slot 5 ) Conversion Health ( Enhanced 27235, learned now ), slot 6 Force Spear ( Enhanced 27236 ). CH2c's five special actives are unslotted. Dev console ( END key ): `/invin 0`, then `/stage 1`. Each skill twice, enemies in front: a cool time on the slot after the first cast, the MP bar drops, a second cast during the cool time is refused. **Wolf Claw:** a claw strike, then the finish state; a hit must show damage ( the state tables are numerical-only ); press another skill key late in the first state ( FP cancel ); walk / dash out of the finish state late on the ground only. **Finger Bullet:** a shot, then the finish state; walk / dash out late on the ground only; the pull-down of hit enemies is **not** expected yet ( CH4a ). **Conversion Health:** HP drops by a fixed share of max HP and the force power rises ( have HP above that share ); the Open Mind buff is not reachable until Open Mind is learned, so nothing to see there. **Force Spear:** one force power is spent at 0.3 s ( have at least one ); a spear appears behind the weapon and pierces; no Demigod set ( not learned ). Conversion Mana ( transcended memo ) is not reachable offline: not part of this test. Then `python tools/logs.py --grep "ARA SKILL"` ( four `equip` lines ) and say "check logs"; I read ErrorLog for `KEM_ERROR` / `Motion Error` and the four skills' effect sets by name ( `EffectSet_AYR_WOLF_CLAW_FX`, `EffectSet_A_AYR_WOLF_CLAW_FINISH`, `EffectSet_SA_ALD_FINGER_BULLET`, `EffectSet_SA_ALD_FINGER_BULLET_END`, `EffectSet_A_ALD_CONVERSION_HEALTH`, `EffectSet_Force_Spear` ).

**Play-test (2026-11-11, run 17:11-17:12 on md5 49843cb4; user asked for the logs, on-screen cool time / MP / damage numbers not reported).** Unit 99: four `ARA SKILL equip` lines ( 27224 slot 0 bank A state 49, 27234 slot 1 bank A state 51, 27235 slot 0 bank B state 59, 27236 slot 1 bank B state 61 ). Casts seen: Force Spear x2 ( `EffectSet_Force_Spear`, `DAMAGE_ARA_FORCE_SPEAR` hit both times ), Conversion Health x2 ( `EffectSet_A_ALD_CONVERSION_HEALTH`, `_Damage_MEMO`, owner state 59, hits ), Finger Bullet x3 ( start sets, owner state 51; finish state 220 reached each time: `EffectSet_SA_ALD_FINGER_BULLET_END` -> `DAMAGE_EFFECT_FINGER_BULLET_ATTACK_SUCCESS` ), Wolf Claw x2 ( `EffectSet_AYR_WOLF_CLAW_FX`, `_ATTACK`, `DAMAGE_EFFECT_A_AYR_WOLF_CLAW_IMPACT` hit ). ErrorLog: `KEM_ERROR` 0, `Motion Error` 0; the four `OpenXET` lines ( `A_ALD_WOLF_CLAW_START_TRACE` / `_PROJECTICLE`, `SA_ALD_FINGER_BULLET_ATTACK_BOX_START` / `_END` ) are the sidecar probe every mesh logs. Not CH2c2's: `AddGroupChainQuest`, `E_DISABLE_HB_CHECK_REQ`, the `APPLIED_SKILL_ID` stand-in lines. Not proven by the log: Wolf Claw's own finish state ( `EffectSet_A_AYR_WOLF_CLAW_FINISH` never appeared: both casts chained into Wolf Spear, state 216 / 217, through the secret chain Wolf Claw -> Wolf Spear ), the FP cancels, cool times, the MP bar, hit damage numbers.

### CH2d - Done (2026-11-11)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (CH2a's, no new define; its `_DEBUG` equip log stays on until the CH2d3 review; no review of its own). All 49 class-map rows read from `charport_adis.py ara --phase CH2d` listings (Ghidra's decompile beside them) plus the registration switches `32bd09d0` / `32bd34c0`. **49 rows = 31 hand-written bodies, 7 Init preload lists generated from the listings (89 preloads, counts checked), 10 registration helpers written as `_CreateState` functions, and `ScaleDamageByRate` on `CEffect`; 0 inside March's functions** (Little Specter is June-only: every state is new). Two small helpers of mine carry code June inlines: `ClearCustomRenderState` ( `31fc9570` ) and `SetSpecterOfStormEffectSetScale` ( `31e4c4c0` ). New members and the `SStoneProtractorAim` struct are in `X2GUAra.h`. `funcmap.tsv` rows `Char Ara CH2d` (51); `MODS.md` "Stage 7 Ara, CH2d"; `JUNE_BUGS.md` 608 / 609.

| June | `CX2GUAra::` | What it does |
|---|---|---|
| `32bc5f80` `32bc0420` | `FindSpecterOfThunderLockOnTarget`, `CalcLockOnLandingPos` | every living enemy unit ( NPCs: basic class, not invincible to `AT_SPECIAL` ): its collision points within `SA_SCOPE_ABS` ( PvP `_PVP` ) of 27023 -> the nearest landing spot ( `CanDown` radius 30, else the nearest point on any enabled non-wall non-monster line ); false when the spot lies behind the unit |
| `32bc94f0` `32ba19b0` `32ba1d00` `32ba1d70` `32bb0790` `32ba1db0` `32ba1e40` `32b94810` | Specter of Thunder states | StartFuture finds the spot and turns round when it is behind; EventProcess hands over to the find state at 0.69 s ( or, with no spot, is a plain skill state with its cancels ); find = fade out, landing = placed on the spot, one physic step with push off / pass on, fade in, `ClearCustomRenderState` at 0.769 s |
| `32bc4410` `32bad5e0` `32bbd840` `32ba2460` | Restraints Specter | 0.395 s: up to 3 force power -> damage rate ( `GetMpScaledPowerRate` ), `DamageEffect_RESTRAINTS_SPECTER_Damage` at the hip; finish 0.2 s: `..._Finish_Damage` locked on to every living unit the ring hit, then the ring's handle is released |
| `32bc4670` `32bad670` `32bbdae0` `32ba2760` `32ba27a0` | Specter of Storm ( 27021 ) | 0.01 s up to 4 force power size the effect set ( 1 + 0.1 each ) and the damage; 0.25 s `DamageEffect_SpecterOfStorm_Damage` 100 ahead of the foot; finish effect set + finish damage at 0.49 s; landing on a line |
| `32ba5ab0` `32ba7c20` `32bc9c70` `32ba6220` | enhanced Storm ( 27036 ) | held key + hold time + the stone protractor ring / aim arrow particles ( `32bc6820` / `32bc12e0` ); degree states are CH2d2's |
| `32bb09f0` `32ba4760` | `CX2DamageEffect::CEffect::ScaleDamageByRate`, `GetMpScaledPowerRate` | rate above 0 scales the effect's and its next tables' damage / rate modifier / tech point ( x truncated rate ), `+0xb18` keeps the rate; count x 30 / MP cost + 1 |

**Registration.** `SetEquippedSkillFuncBySkillSlotIndex`: four new cases ( Thunder 27023 / 27042 with its find and landing sub-states and `m_fSpecterOfThunderRate = -1`; Restraints 27022 / 27041 + finish; Storm 27021 + finish + landing; enhanced Storm 27036 with main + landing only ). `InitAquiredActiveSkillState`: Restraints 27022 / 27041 ( June `@32bd474e-@32bd4803`: states 302 / 303, the chain lookup takes the swapped id ). `X2SkillTree.h`: `RNSI_SA_ALS_SPECTER_OF_STORM_UPGRADE` 27036, `_RESTRAINTS_SPECTER_UPGRADE` 27041, `_SPECTER_OF_THUNDER_UPGRADE` 27042 ( June `Enum.lua` ). **CH0's names that changed:** `32bb09f0` is not a mesh-instance helper but the damage-rate scaler ( it is on `CX2DamageEffect::CEffect`, whose members are protected ); `32bc12e0` rotates the aim arrow particle ( not a fade ); `31cf1920` is `GetParticle`; `32a2dec0` in the finish FrameMove is `m_vecEffectSetToDeleteOnStateEnd.push_back`; `31fcf430` is `GetUpgradeSwappedSkillID`. Specter of Thunder's main state and Storm's base state have no `ASI_` id in June ( the slot state id is used ), so no enum member was added. Memo tests ( `GetEqippedSkillMemo` stub `31fca9e0` ) are dropped, as in CH2c. `+0x9614` ( used force power ) has no reader and no member under `_FP`: not written. Templet-swap check: the readers of these ids ( `SetEquippedSkillLevel`, `GetUpgradeSwappedSkillID` in the finder and the rate, `GetNowStateSkillID` for the ability reads ) already take the swapped id.

**Checklist** `(Ara: CH2d ...)`: §1 NUMERICAL_DATA row ( Restraints 7.83 / 4.51 ), §4 l.178 ( 3 names ), §7ae l.728 ( the cases ). **Found, not ported:** nothing new; the enhanced Storm's degree states, ultimate degree and ultimate finish stay CH2d2 ( kickoff amended: it must finish 27036's case before 27036 is learned ).

**Gates.**
- `flagcheck JUNE_CHAR_ARA_SKILL --with ..._SKILL_DEBUG --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2SkillTree.h X2Lib/X2DamageEffect.cpp X2Lib/X2DamageEffect.h`: `4 PASS ( +1024, +72, +33, +3 ), 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL`, `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`. `flagcheck JUNE_ENUM_NAMES --worktree --files X2Lib/X2SkillTree.h`: `PASS~`, `RESULT: PASS`.
- `junediverge.py --check` the five files: `allow-listed: 4`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s) remain`. `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing`, STATE `37 compared, 0 wrong`, `RESULT: PASS`. `skillicons.py check` 27021-27023 / 27036 / 27041 / 27042: `skills 6, misses 0`, `RESULT: PASS`.
- `/Zs` probes: `X2GUAra.cpp` and `X2DamageEffect.cpp` 0 errors; static asserts ( `SA_EFFECTIVE_TIME` 1, `SA_SCOPE_ABS` 0x1d, `_PVP` 0x9d, `SA_SCOPE_ABS_Y` 0x9e, the six June ids ) hold.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,259,968 B, 2026-11-11 22:08:40`; `DEPLOY OK: X2_offline.exe is 26,259,968 B, dated 2026-11-11 22:08:47, md5 df11ffb23739a1abe06a65b32201a585, equal to X2/US_SERVICE/x2.exe (2026-11-11 22:08:40); built at main 33f01c3f + 5 uncommitted code file(s).`
- `EXESTR OK: 4 of 4 found in X2_offline.exe, 26259968 B, md5 df11ffb2` ( wide `EffectSet_SI_SA_ALS_SPECTER_OF_STORM_FINISH`, `DamageEffect_RESTRAINTS_SPECTER_Finish_Damage`, `DamageEffect_SpecterOfStorm_Finish_Damage`, `Bip01_Footsteps` ); `EXESTR OK: 1 of 1` ( narrow `ASI_SA_AA_SPECTER_OF_THUNDER_FIND_LANDING` ).

**Play-test hand-off.** Unit 135 `LSpecter99` ( Ara, class 34, lv 99, not transcended; backups `db_backup/els_db_20261111_215943_pre_ch2d_pre_addchar.sql`, `_220003_pre_sql.sql` ). Slotted for you, A bank: slot 0 Specter of Storm ( 27021 ), slot 1 Restraints Specter ( 27022 ), slot 2 Specter of Thunder ( 27023 ). Do **not** learn the enhanced ids ( 27036 / 27041 / 27042 ): 27036 needs CH2d2. Dev console ( END key ): `/invin 0`, then `/stage 1`. Each skill twice, enemies in front, force power on the bar ( Storm spends up to 4, Restraints up to 3, Thunder none ): a cool time on the slot after the first cast, the MP bar drops, a second cast during the cool time is refused. **Specter of Thunder:** with an enemy in range the unit fades out, appears on the enemy's line and fades back in ( turn to face it ); with no enemy in range it is a plain cast with walk / dash cancels. **Restraints Specter:** the ring damage at the hip ( more force power, bigger damage ), then the finish state; every enemy the ring hit takes a second hit. **Specter of Storm:** an effect set that grows with the force power ( more force power, bigger effect and damage ), the damage 100 in front of the foot, the finish effect, then the landing. Then `python tools/logs.py --grep "ARA SKILL"` ( three `equip` lines ) and say "check logs"; I read ErrorLog for `KEM_ERROR` / `Motion Error` and the effect names ( `EffectSet_SI_SA_ALS_SPECTER_OF_STORM`, `..._FINISH`, `DamageEffect_RESTRAINTS_SPECTER_Damage`, `DamageEffect_SpecterOfStorm_Damage`, the Thunder states ).

### CH2dx - Done (2026-11-11)

Opus 5.5. Fix of CH2d's play-test ( unit 135 `LSpecter99`: Specter of Thunder worked, Specter of Storm and Restraints Specter left the unit stuck ). Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` ( CH2a's, no new define; `_SKILL_DEBUG` stays on until the CH2d3 review ). No review of its own ( CH2d3's covers it ).

**Cause ( both ).** A state id that nothing registered is entered as a blank state ( no callbacks, no Lua table ), which never leaves ( `X2GUUser.cpp` commit at the future-state switch ).
- **Storm 27036.** The game swaps the slotted base ids for the learned enhanced ones ( save rows 27036 / 27041 / 27042 at level 1 ). 27036's held EventProcess ( June `32bc9c70` ) changes to `ASI_SA_ALS_SPECTER_OF_STORM_FINISH`, which only the base 27021 case registered ( its `SetEquippedSkillLevel` block even created the entry blank with `m_StateList[]` ). Log: the Storm damage at 22:13:05.7, then nothing until the warp at 22:13:30.
- **Restraints 27041.** Not Restraints itself: June's `CHAIN_SYSTEM` row ( combo `ASI_COMBO_ALS_XXXLoopZ` -> Restraints -> Shadow Knot -> Sever, `ARA_MARTIAL_ARTIST.lua` l.211-230 ) let the second press of the slot run Shadow Knot ( learned 27033: `ARA_Shadow_Knot_Start` at 22:13:54.851, the weapon hit, the catch box at 22:13:56.2 ) and the third press, in the catch state's FP-cancel window, start **Sever** ( `ASI_SA_ALS_SEVER`, CH2d2's, unregistered ). Shadow Knot's end effect never appears in the log, so the catch state was left for Sever, not for its end state. A chained skill that is learned but not slotted also skips the cool time ( `fCoolTimeLeft = 0` ), which is why the "second cast during the cool time" was not refused.

**Fixes.**

| June | `CX2GUAra::` | What it does |
|---|---|---|
| `32bd09d0` @`32bd1ef3`-@`32bd1f65` | 27036 case | registers the up / down aim states ( 0x14e / 0x14f ) and the ultimate finish on the finish id 0x131, June's order |
| `32bcf400` `32bcf450` `32bc9f50` | `..._DEGREE_` / `_ULTIMATE_DEGREE_` / `_ULTIMATE_FINISH_CreateState` | tables `ASI_SA_ALS_SPECTER_OF_STORM_DEGREE` ( absent from June's script: JUNE_BUGS 610 ), `_ULTIMATE_DEGREE`, `_ULTIMATE_FINISH`; the finish's init is the base finish's `32b94bc0` |
| `32bc67f0` `32bab500` | `..._DEGREE_FrameMoveFuture`, `_FrameMove` ( both aim states ) | aim arrow follows the facing; hold time down; aim +-100 a second by the now state, kept within 0 and `SA_SCOPE_ABS_Y` |
| `32ba5b50` `32ba5e90` | `..._DEGREE_` / `_ULTIMATE_DEGREE_EventProcess` | hold over or key released -> finish; Down / Up -> the other aim state; the skill-end cancel block ( same shape as `32ba19b0` ) |
| `32ba7c90` `32ba61d0` `32bc0e30` `32ba61f0` | `..._ULTIMATE_FINISH_StateStartFuture`, `_FrameMoveFuture`, `_FrameMove`, `_EventProcess` | ring and arrow particles go; future / now `dirDegree.z` = aim ( now past 0.4 s ); base finish effect set and damage, Bip01 tilted by `SetRotateBone( (-1,0,-1), (-1,0,aim) )` first ( JUNE_BUGS 611 ); animation end -> landing |
| - ( stand-in ) | `IsStateRegistered`, guard in `CanUseSpecialAttack`'s chain block | a chain step into an unregistered or blank state is refused ( `ARA SKILL chain refused` ); covers Sever until CH2d2 and CH3's chain states until theirs |
| - ( debug ) | `CommonStateStartFuture` | `[JUNE_CHAR] ARA SKILL state <id> skill <id>` for every skill state entered, `BLANK` for any state entered with no callback |

Scope: the degree states and ultimate finish were CH2d2's; 27036 is unplayable without them ( the held key reaches both aim states ), so they moved here ( 11 class-map rows, col 7 now `CH2dx`; `CH2d2.md` amended: 20 functions left ). Sever stays CH2d2's behind the guard. `funcmap.tsv` rows `Char Ara CH2dx` ( 11 ); `MODS.md` "Stage 7 Ara, CH2dx"; `JUNE_BUGS.md` 610 / 611.

**Gates.**
- `flagcheck JUNE_CHAR_ARA_SKILL --with ..._SKILL_DEBUG --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `2 PASS ( +202 -1, +12 ), 0 FAIL`, `1 deleted line(s)` ( the 27036 case comment, which named the states as CH2d2's ), `0 non-ASCII inserted`, `0 EOL-mismatched`, `0 #else/#endif without the flag token`, `0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`: `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s) remain`. `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing`, STATE `37 compared, 0 wrong`, `RESULT: PASS`. `/Zs` probe `X2GUAra.cpp`: 0 errors.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,264,064 B, 2026-11-11 22:42:14`; `DEPLOY OK: X2_offline.exe is 26,264,064 B, dated 2026-11-11 22:42:21, md5 a2e8854a38f54e2888cf78a84c1887fb, equal to X2/US_SERVICE/x2.exe (2026-11-11 22:42:14); built at main 6236adac + 2 uncommitted code file(s).`
- `EXESTR OK: 2 of 2 found in X2_offline.exe, 26264064 B, md5 a2e8854a` ( narrow `ASI_SA_ALS_SPECTER_OF_STORM_ULTIMATE_FINISH`, `ASI_SA_ALS_SPECTER_OF_STORM_DEGREE` ); `EXESTR OK: 2 of 2` ( wide `[JUNE_CHAR] ARA SKILL chain refused: ...`, `[JUNE_CHAR] ARA SKILL state %d skill %d%s` ).

**Save.** New unit 136 `LSpecBase99` ( class 34, lv 99, `createchar.py`; backup `db_backup/els_db_20261111_223500_pre_createchar.sql` ): base ids only, A0 Specter of Storm 27021, A1 Restraints Specter 27022, A2 Specter of Thunder 27023 ( also learned: Shadow Knot 27004, Sever 27024 ). Unit 135 keeps the enhanced ids ( A0 27036, A1 27041, A2 27042 ).

**Play-test hand-off.** In the chat of 2026-11-11 ( both units, each skill twice, the held Storm with Up / Down, the Restraints key pressed three times ). Lines to read: `[JUNE_CHAR] ARA SKILL state` ( no `BLANK` ), `ARA SKILL chain refused`, the Storm finish / landing effects, 0 `EXCEPTION`.

**Play-test passed (2026-11-11 22:44-22:46, exe md5 a2e8854a).** 0 `EXCEPTION`, 0 `KEM_ERROR` / `Motion Error`, no `BLANK` state line. Storm 27036 x6: `state 49 -> 266 ( finish ) -> 267 ( landing )`, one held cast through `273` ( an aim state ) to the finish; finish and landing effect sets and `DamageEffect_SpecterOfStorm_Finish_Damage` hits. Restraints 27041: `51 -> 265` ( finish ); the chain `51 -> 155 / 156 / 157 ( Shadow Knot ) -> 158 ( end )` with `ARA SKILL chain refused: skill 27024 state 264 not registered` x2 per run ( the Sever press ), the unit back to input. Thunder 27042: `53 -> 262 -> 263` x3.
- **Base ids not reachable at level 99.** The emulator's `[JUNE_SKILL] ACQ grant` ( on character select: `15 row(s) raised, 14 base(s) replaced` ) raised unit 136 to the enhanced ids too, so it played 27036 / 27041 / 27042 like 135. Restraints and Thunder share their states with the base ids; only the base Storm case ( 27021, CH2d's, unchanged here ) is still unplayed: the CH2d3 stage review's play-test covers it on a unit below the upgrade level. Unit 136 is kept ( now the same kit as 135 ).

### CH2d2 - Done (2026-11-11)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` ( CH2a's, no new define; `_SKILL_DEBUG` stays on until the CH2d3 review ). No review of its own ( CH2d3's covers it ). Little Specter ( 34 ) ids 27024 / 27059 Sever, 27025 / 27060 Suppression Specter, and Veritable Pandemonium ( the chain's secret skill, states 309 / 310 ).
All 20 class-map rows read from `charport_adis.py ara --phase CH2d2` listings and Ghidra decompiles; `32bc3f50` ( Sever end ) is uncarved, read from the listing. Every block carries `// char-ara CH2d2`. `funcmap.tsv` rows `Char Ara CH2d2` ( 20 ); `MODS.md` "Stage 7 Ara, CH2d2"; `JUNE_BUGS.md` 612.

| June | `CX2GUAra::` | What it does |
|---|---|---|
| `32bcea30` `32baac90` `32bc9750` `32bcec30` | `ASI_SA_ALS_SEVER_` / `ASI_SA_AA_SUPPRESSION_SPECTER_` / `..._VERITABLE_PANDEMONIUM_` / `..._VERITABLE_PANDEMONIUM_FINISH_CreateState` | the four helpers: script table + callbacks. Registered in the 27024 / 27059 and 27025 / 27060 cases ( first switch ) and Sever + both Pandemonium states in `InitAquiredActiveSkillState` ( `32bd34c0` @`32bd4a54`-@`32bd4b6b`, chain lookup on the swapped id ) |
| `32b94830` `32b95030` `32b95120` `32b95150` | `..._Init` | 11 / 19 / 2 / 11 preloads |
| `32ba2140` `32ba3170` `32ba3500` | Sever / Suppression Specter / Pandemonium finish `EventProcess` | one body: animation end -> wait on a line, else jump down 45 lower; skill / walk / dash / attack cancels |
| `32bbd640` `32bc3f50` | Sever `FrameMove`, `StateEnd` | 0.379 s the damage effect set ( handle kept ); the end spot = landing spot ( `CalcLockOnLandingPos` ) of the hit units' collision boxes, the first taken, later ones if no farther than the kept one; falls back to the unit position |
| `32ba3490` `32ba34d0` `32bbe550` | Pandemonium `StateStartFuture`, `EventProcess`, `FrameMove` | learned Spiritual Eye rate cuts every left cool time ( member written by CH4a, default none ); animation end -> finish; 0.001 s start set, 0.307 s render state 2 + fade-out ( 12 / s ), 0.63 s black hole at Sever's spot ( x / z ) and the land height; Weird Fox ( 27420 ) learned + awakened -> the `_TRENSCENDENCE` sets |
| `32bc5280` `32bbec00` `32b951e0` `32b951f0` | Pandemonium finish `StateStartFuture`, `FrameMove`, `EndFuture`, `End` | the unit moves to the nearest enabled line point to Sever's spot ( JUNE_BUGS 612 ); fade-in ( 5 / s ) ends the render state; FX1 0.001 s ( land position ), FX2 0.3 s, Damage 0.635 s, power rates x Load of Asura's member |

**New names.** `X2SkillTree.h` ( `JUNE_ENUM_NAMES` ): `RNSI_SA_ALS_SUPPRESSION_SPECTER` 27025, `RNSI_SA_ALS_SEVER_UPGRADE` 27059, `RNSI_SA_ALS_SUPPRESSION_SPECTER_UPGRADE` 27060, `RNSI_P_ASR_SPIRITISM_WEIRD_FOX` 27420. Members ( `X2GUAra.h`, +0x974c / +0x9750 / +0x97a4 / +0x97a8 ): `m_fPandemoniumCoolTimeRate` ( -1 ), `m_fPandemoniumPowerRate` ( 1 ), `m_hSeverEffectSet`, `m_vSeverEndPos`. The first two are written by CH4a's passive refresher ( `32bcd9c0`: 27414 Spiritual Eye ability 148, 27412 Load of Asura ability 15 ); until then Pandemonium runs with the defaults.
**How the secret skill starts.** The engine already does it ( `GetTriggerSecretSkillState`, `X2ComboAndSkillChainSystem.cpp` ): June's chain rows ( combo -> Restraints Specter -> Shadow Knot -> Sever, `TRIGGER_TIME_TO_SECRET_SKILL = 0.615` ) must all be chaining. A plain Sever press does not start it.
**Chain stand-in.** CH2dx's guard stays ( CH3's chain states rely on it ); Sever is registered now, so Restraints -> Shadow Knot -> Sever runs.
**Found, not ported.** Nothing new. Veritable Pandemonium's damage and the Suppression Specter's are script data ( effect sets / damage tables ): no `NUM` code here.

**Gates.**
- `flagcheck JUNE_CHAR_ARA_SKILL --with ..._SKILL_DEBUG --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2SkillTree.h`: `2 PASS ( +445, +25 ), 0 DEFINE, 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` ( the 3 files ): `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s) remain`. `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing`, STATE `37 compared, 0 wrong`, `RESULT: PASS`. `skillicons.py check 27024 27025 27059 27060`: `misses 0`, PASS.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,276,352 B, 2026-11-11 23:21:59`; `DEPLOY OK: X2_offline.exe is 26,276,352 B, dated 2026-11-11 23:22:05, md5 65294f1cb3c0cee2c4fc3def3ff3c3e7, equal to X2/US_SERVICE/x2.exe (2026-11-11 23:21:59); built at main 6ff0845c + 3 uncommitted code file(s).`
- `EXESTR OK: 2 of 2 found in X2_offline.exe, 26276352 B, md5 65294f1c` ( wide `EffectSet_SI_SA_ALS_SEVER_DAMAGE`, `..._VERITABLE_PANDEMONIUM_BLACKHOLE_TRENSCENDENCE` ).

**Save.** Unit 135 `LSpecter99` ( class 34, lv 99; backup `db_backup/els_db_20261111_231517_pre_sql.sql` ): bank A slot 0 Sever 27059, slot 1 Restraints Specter 27041, slot 2 Suppression Specter 27060; Shadow Knot 27033 learned, unslotted ( chain ).

**Play-test hand-off.** In the chat of 2026-11-11 ( unit 135 ). Lines to read: `[JUNE_CHAR] ARA SKILL state` ( Sever 301, Pandemonium 309 / 310, no `BLANK` ), `EffectSet_SI_SA_ALS_SEVER_DAMAGE`, 0 `EXCEPTION`.

**Play-test passed (2026-11-11 23:24-23:26, exe md5 65294f1c, unit 135).** 0 `EXCEPTION`, 0 `KEM_ERROR` / `Motion Error`, no `BLANK` state line. Sever 27059 x5 ( state 49: `SEVER_FX1` / `FX2`, `SEVER_DAMAGE` at +0.4 s, `DamageEffect_SI_SA_ALS_SEVER_Damage` hits ). Suppression Specter 27060 x2 ( state 53: `FX1`, `FX2`, `Damage` + a hit, `_TARGET` set on the enemy ). Restraints 27041 -> Shadow Knot ( 155 / 156 / 157 ) -> Sever ( 49 ) x5; once the chain ran through, **Veritable Pandemonium started 0.615 s into Sever** ( state 269: `_START` at +0.1 s, `_BLACKHOLE` 0.64 s after the state start; then 270: `FINISH_FX1` / `FX2` / `Damage` at +0.0 / +0.29 / +0.62 s ) x2. The chain's Sever step runs in the skill's own slot state ( 49 ), not the registered 301. `ErrorLog.txt`: 6 `OpenXET` lines for the new meshes ( `ALS_VERITABLE_PANDEMONIUM_P01 / P02 / P04`, `AA_SUPPRESSION_SPECTER_FOX / P01 / P02` ): the same line every June mesh preload gets ( 305 in the run ), the files are in data132. Cool time and MP are not in the logs; not reported wrong by the user. `AddGroupChainQuest` unbound and 2 `E_DISABLE_HB_CHECK_REQ` UNHANDLED are not this phase's.

### CH2d3 - Done (2026-11-11)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` ( CH2a's, no new define; `_SKILL_DEBUG` stays on until the CH2d3 stage review ). Little Specter ( 34 ) A tier: View Between 27214 / 27225, Shake Chaos 27215 / 27239, Mana Drain 27216 / 27240, Quick Piercing 27217 / 27241. All 29 class-map rows read from `charport_adis.py ara --phase CH2d3` listings ( Ghidra decompile beside them; `32bc3e40`, `32ba1370`, `32ba0cc0`, `32ba0680` are uncarved ). Every block carries `// char-ara CH2d3`. `funcmap.tsv` rows `Char Ara CH2d3` ( 29 ); `MODS.md` "Stage 7 Ara, CH2d3"; no JUNE_BUGS row.

| June | `CX2GUAra::` | What it does |
|---|---|---|
| `32bce810` `32bc9400` `32bce8c0` `32bce920` `32bc9450` `32bce9c0` | `ASI_A_ALS_VIEW_BETWEEN_` / `_FINISH_` / `SHAKE_CHAOS_` / `_REPEAT_` / `_FINISH_` / `MANA_DRAIN_CreateState` | the six helpers: table + callbacks. First switch ( 32bd09d0 ): 27214 / 27225 + finish 289, 27215 / 27239 + repeat 333 + finish 292, 27216 / 27240. Second switch ( 32bd34c0 ): View Between 288 + 289, Shake Chaos 291 + 333 + 292, with the chain lookup on the swapped id |
| `32b92c50` `32b940d0` `32b94150` `32b94290` `32b94360` `32b94580` | `..._Init` | 3 / 9 / 9 / 9 / 9 / 9 preloads |
| `32bc3190` `32b94310` `32b94340` `32ba1340` | `..._StateStart` | View Between: up to one force power -> `GetMpScaledPowerRate`; Shake Chaos: repeat count 0 ( the repeat state counts +1 ); Mana Drain: handle / retake MP / hit count cleared |
| `32bad3c0` `32bad450` | View Between / Shake Chaos `EventProcess` | end -> `StateChangeOrFPCancel( finish )`, FP-cancel probe window |
| `32ba0680` `32ba0cc0` `32ba1370` | finish / Mana Drain `EventProcess` | the plain skill-end body, equal to Sever's `32ba2140` after address normalisation |
| `32bbcdc0` `32bbcff0` | View Between `FrameMove` ( start, finish ) | 0.338 s / 0.266 s damage effect 65 / 220 in front of the hip ( behind when facing left ), `ScaleDamageByRate( rate )` |
| `32bc3260` `32bbd240` | Shake Chaos `FrameMove` ( run, finish ) | 0.135 s first run spends 1 force power into the rate, Damage1; 0.309 s Damage2; repeat runs at half power; finish 0.01 s |
| `32bc3bc0` `32bc3e40` | Mana Drain `FrameMove`, `StateEnd` | 0.3 s the drain effect ( handle kept ); 0.65 s and at the end the MP taken is given back with the +MP popup |
| `32b92c80` `32b9c810` | Quick Piercing `FrameMove`, `EventProcess` | inside March's functions ( March in `#else` ): FrameMove = thunk to `CommonFrameMove`; EventProcess = wait on a line else jump down, on-line walk / dash cancels. Case: table `_TRENSCENDENCE` when Weird Fox ( 27420 ) is learned, Init on the normal state only |

**New names.** `X2SkillTree.h`: `RNSI_A_ALS_VIEW_BETWEEN_UPGRADE` 27225, `_SHAKE_CHAOS_UPGRADE` 27239, `_MANA_DRAIN_UPGRADE` 27240, `_QUICK_PIERCING_UPGRADE` 27241. `X2GUAra.h`: `ASI_A_ALS_SHAKE_CHAOS_REPEAT` at the end of `ARA_STATE_ID` ( June id 333, unnamed in June ), members `m_fViewBetweenRangeRate` ( +0x9758 ), `m_fShakeChaosRangeRate` ( +0x975c ), `m_usShakeChaosCount` ( +0x9760 ).
**Stub, not ported.** June's memo tests ( View Between's `_MEMO_` effect name and 0.15 rate lift, Shake Chaos' repeat at 0.5 s with memo 0x94342 ) go through `31fca9e0`, which never holds a memo ( as CH2d's 0x9433c ): the repeat state is registered and never entered. The `+0x9614` used-force-power increments have no reader: not kept. The chain View Between -> Shake Chaos runs; the step into Raid ( CH3c2 ) is refused by CH2dx's guard.
**Found, not ported.** Nothing new. Weird Fox's table choice for Quick Piercing is Asura-only in play ( transcendence passive ): not reachable on class 34.

**Gates.**
- `flagcheck JUNE_CHAR_ARA_SKILL --with ..._SKILL_DEBUG --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2SkillTree.h`: `2 PASS ( +534, +32 ), 0 DEFINE, 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` ( the 3 files ): `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s) remain`. `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing`, STATE `37 compared, 0 wrong`, `RESULT: PASS`. `skillicons.py check` ( 8 ids ): `misses 0`, PASS. `/Zs` probe `X2GUAra.cpp`: 0 errors.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,287,616 B, 2026-11-11 23:54:13`; `DEPLOY OK: X2_offline.exe is 26,287,616 B, dated 2026-11-11 23:54:20, md5 3279ce854d85edd8b6b43ffb803a96d0, equal to X2/US_SERVICE/x2.exe (2026-11-11 23:54:13); built at main c0cc6b8f + 3 uncommitted code file(s).`
- `EXESTR OK: 2 of 2 found in X2_offline.exe, 26287616 B, md5 3279ce85` ( wide `DamageEffect_VIEW_BETWEEN_FINISH_Damage`, `DamageEffect_MANA_DRAIN_MP_Damage` ); `EXESTR OK: 2 of 2` ( narrow `ASI_A_ARA_QUICK_PIERCING_TRENSCENDENCE`, `ASI_A_ALS_SHAKE_CHAOS_FINISH` ).

**Save.** Unit 137 `LSpecBase45` ( class 34, lv 45; backup `db_backup/els_db_20261111_234821_pre_createchar.sql` ): bank A 0 Storm 27021, 1 View Between 27214, 2 Shake Chaos 27215, 3 Mana Drain 27216; bank B slot 5 Quick Piercing 27217. Unit 138 `LSpecUp99` ( lv 99 ): bank A 0 View Between 27225, 1 Shake Chaos 27239, 2 Mana Drain 27240, 3 Quick Piercing 27241.

**Play-test hand-off.** In the chat of 2026-11-11 ( units 137 and 138 ). Lines to read: `[JUNE_CHAR] ARA SKILL state` ( View Between 288 / 289, Shake Chaos 291 / 292, Mana Drain, Quick Piercing, Storm 27021's states; no `BLANK` ), the `DamageEffect_VIEW_BETWEEN*` / `SHAKE_CHAOS*` / `MANA_DRAIN_MP` effects, `[JUNE_SKILL] NUM use`, `ARA SKILL chain refused` ( at Raid ), 0 `EXCEPTION`.

**Play-test (2026-11-12, run 04:53-04:56 on md5 3279ce85; unit 137 only).** Base Storm 27021: states 50 -> 266 -> 267 twice, no `BLANK` ( the CH2dx test owed to this phase passes ). View Between 27214 ( state 52 ): chained into Shake Chaos ( 54 -> finish 259 ) three times, alone once ( 52 -> finish 258, the finish damage effect created ); `DamageEffect_VIEW_BETWEEN_Damage` hit. Shake Chaos: `Damage1` / `Damage2` / `FINISH_Damage` hit. Mana Drain 27216 ( state 56 ) x2: `MANA_DRAIN_MP_Damage` then `MANA_DRAIN_Damage` hit ( MP give-back popup is on-screen only, not reported ). Quick Piercing 27217 ( bank B, state 60 ) x3. ErrorLog: `KEM_ERROR` 0, `Motion Error` 0, packets `EXCEPTION` 0; the `OpenXET` lines for `ALS_VIEW_BETWEEN_H1/H2*` and `SI_A_ALS_MANA_DRAIN_01` are the engine's companion-file lookup ( the meshes are in data132 ), as the `MOTION_ARA_*` lines. `NUM use`: none ( as CH2a ). **Not played:** unit 138 ( upgrade ids 27225 / 27239 / 27240 / 27241, second-switch registration ) and `chain refused` at Raid ( 0 lines ): both go to the review.

### CH2d3 - Review (2026-11-12, Opus 5.5; the CH2 stage review: covers CH2a, CH2b, CH2b2, CH2c, CH2c2, CH2d, CH2dx, CH2d2, CH2d3)

**Verdict: pass, no code fix.** The one code change is retiring `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL_DEBUG` ( `KTDXLIB/Always.h`, `//#define`; the guarded log lines stay in the source ). `_SKILL` stays on. CH2b0 / CH2c0 had their own reviews and are not covered again.

**Spot-checks against the June exe** ( Ghidra decompile + `charport_adis.py` + `charport_consts.py` for operand widths ), all equal to the port:
- CH2d3 View Between FrameMove `32bbcdc0` / finish `32bbcff0`: 0.338 s / 0.266 s ( floats ), offsets 65.0 / 220.0 ( qwords ) times `GetDirVector()` added to Bip01 when `GetIsRight()`, subtracted otherwise; `CreateInstance( this, name, GetPowerRate(), pos, rot, rot, landPosition.y )`; `32bb09f0( effect, +0x9758 )` = `ScaleDamageByRate( m_fViewBetweenRangeRate )`. The finish's `fadd 0.15` and the `_MEMO_` name sit behind `31fca9e0( 0x94340 )`.
- CH2d3 Shake Chaos FrameMove `32bc3260`: 0.135 / 0.309 s; first run ( `+0x9760 == 0` ) with force power -> `32ba4760( 1, 27215, 1 )` into `+0x975c`, `UseForcePower( 1 )`, `+0x9614 += 1` ( no reader, not kept ); both damages x0.5 when the count is not 0; `+0x975c` scales both.
- CH2d3 Mana Drain FrameMove `32bc3bc0` ( 0.3 s `MANA_DRAIN_MP_Damage`, handle `+0xe8` -> `+0x97c8`; 0.65 s `UpNowMp` + `DisplayAddMPInformation( mp, GetPos(), GetDirVector() )`, zero `+0x97cc` / `+0x97d0` ) and uncarved StateEnd `32bc3e40` ( same give-back, then `CommonStateEnd` ).
- CH2d3 registrations: `32bd09d0` @`32bd1bef`-@`32bd1d34` ( View Between x2 + finish 0x121; Shake Chaos x2 + repeat 0x14d + finish 0x124; Mana Drain x2 ), Quick Piercing @`32bd2017` ( `31fceab0( 0x6b1c = 27420 )` picks the `_TRENSCENDENCE` table, Init on the normal state only, hyper copies EP / FM ); `32bd34c0` @`32bd4b9a`-@`32bd4ddb`: states 0x120 / 0x121 / 0x123 / 0x14d / 0x124, chain lookup on 0x120, 0x123, 0x14d only, as ported. June's `+0x11 = 1` stores are `IsSkillSlotState` ( CH1n: March answers it ), correctly dropped.
- CH2a Falling Dragon FrameMove `32bc1ac0`: 0.001 s, `!IsEmptyForcePower`, `( FP - 5 ) * 0.2 + 1.0` in double times `GetPowerRate()`, all FP spent; 1.32 s `EffectSet_Falling_Dragon` with the rate; the 1.33 s `_Memo` set is behind `31fca9e0( 0x94337 )`.
- CH2d Storm FrameMove `32bc4670`: 0.01 s, up to 4 FP, size `n * 0.1 + 1.0` ( qwords ), `32ba4760( n, 27021, 1 )`; 0.25 s damage at `Bip01_Footsteps` +- 100.0 by `GetIsRight()`, `SetScale( size )`, `ScaleDamageByRate`; hold time runs down. June calls `32bb09f0` even on a NULL effect; it returns at once on NULL, so the port's NULL test is the same.
- CH2c2 Force Spear FrameMove `32bb77b0`: 0.198 s, `Dummy3_Weapon` minus dir x40 when facing right ( June's sign, as March's 25 ), y -25; Demigod 27418 picks both `_With_Demigod` names; piercing = `SA_PROJECTILE_PIERCING` of 27211 at `max( 1, level )`, truncated ( `fistp` with RC 0xc00 ); the x1.33 range lift is behind `31fca9e0( 0x9431f )`.
- **Stub.** `31fca9e0` re-read: `return 0;` with no body, so every memo branch claimed in CH2a-CH2d3 ( 0x94337, 0x9431f, 0x9433c, 0x94340, 0x94342 ) is dead in June. The Shake Chaos repeat state is registered and never entered.

**Hygiene** ( per commit, `flagcheck JUNE_CHAR_ARA_SKILL --with JUNE_CHAR_ARA_SKILL_DEBUG --range <c>^..<c>` ): every commit `RESULT: PASS`, 0 FAIL, 0 non-ASCII, 0 EOL mismatch, 0 bare `#else` / `#endif`, 0 encoding change. `files:` CH2a `2 PASS, 1 DEFINE, 1 OTHER` ( 3 deleted lines: the `//#define` and the old 2-line `#error` guard, rewritten ); CH2b `2 PASS, 1 OTHER`; CH2b2 `1 PASS, 1 OTHER`; CH2c `2 PASS, 1 OTHER`; CH2c2 `1 PASS, 1 OTHER`; CH2d `4 PASS, 1 OTHER`; CH2dx `2 PASS, 0 OTHER` ( 1 deleted line: the 27036 case comment, as its Done note says ); CH2d2 `2 PASS, 1 OTHER`; CH2d3 `2 PASS, 1 OTHER`. Every OTHER is `X2Lib/X2SkillTree.h` = `SERV_IRUHADEV_JUNE_ENUM_NAMES`. `junediverge.py --check-all`: `allow-listed: 206`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s) remain ( C 0, D 1, X 0 ignored )`. `skillicons.py check 27000-27299`: `skills 112, misses 0`, `RESULT: PASS`. `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing` ( `CAMERA_TYPE.CT_PARTS_LOOK_TRACKINGT` nil in June too ), `STATE: 37 compared, 0 wrong`, `RESULT: PASS`. The retirement: `flagcheck JUNE_CHAR_ARA_SKILL_DEBUG --worktree --files KTDXLIB/Always.h`: `1 DEFINE, 0 FAIL`, 1 deleted line ( the `#define`, now `//#define` ), 0 non-ASCII, 0 EOL, 0 encoding change; RESULT FAIL only for `define: NOT FOUND` ( `//#define` ). `junediverge.py --check KTDXLIB/Always.h`: `RESULT: PASS`.

**Play-test (2026-11-12, run 05:05-05:07 on md5 3279ce85; unit 138 `LSpecUp99`, debug still on).** Save: unit 138 given Raid 27218 learned, unslotted ( backup `db_backup/els_db_20261112_050342_pre_sql.sql` ), so the chain reaches the guard. Equip lines 27225 / 27239 / 27240 / 27241 on bank A slots 0-3. States: 27225 `50` x6 -> finish `258` x2; 27239 `52` x5 -> finish `259` x5; 27240 `54` x7; 27241 `56` x10. `ARA SKILL chain refused: skill 27218 state 285 not registered` x4, each followed by Shake Chaos' finish 259 ( unit back to input ). Hits: `VIEW_BETWEEN_Damage` 5, `_FINISH_Damage` 2, `SHAKE_CHAOS_Damage1/2` 10, `_FINISH_Damage` 5, `MANA_DRAIN_MP_Damage` 4, `MANA_DRAIN_Damage` 4. 0 `BLANK`, 0 `EXCEPTION`, 0 `KEM_ERROR` / `Motion Error`, clean shutdown. `OpenXET` misses = companion-file lookups ( as CH2d3 ). Base Storm 27021 passed in CH2d3's run ( unit 137 ).

**Build:** not awaited ( only change: `//#define ..._SKILL_DEBUG`, standing rule 2026-11-03 ).

**Kickoffs.** `CH3a.md` amended: CH3a turns `_SKILL_DEBUG` back on ( the CH3 stage's play-tests read its `state` / `BLANK` / `chain refused` lines ); the CH3d review retires it again. Carried forward: the `IsStateRegistered` stand-in guard stays until CH3's chain states ( Raid, Get Blood, Smash, Low Brandish ... ) are registered; the Raid step refuses until CH3c2.

### CH5 - Done (2026-11-11)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_UI` (`Always.h` end; `#error` without `_ARA_LIVE` and `JUNE_UNIT_VIEWER_EFFECT_PLAYER`). Review kickoff `kickoffs/char-ara/CH5-review.md` written.
- **`UnitTemplet.lua` (data036, core-list BOM source):** Ara's 5 blocks are June's text, 34 / 124 added after 119 (55 -> 57 blocks, the other 50 untouched, BOM + CRLF kept). Semantic delta vs the old file = exactly `ch0_unittemplet_ara.tsv`:
  `m_fLobbyRotationY` 1 / 15 / 1 / 10 / 15, `m_HyperCutIn` + `m_BerserkCutIn` on 118 / 119, 34 (24 keys) and 124 (27 keys) new. `luasrc.py refresh` REFRESHED, `ledger --check` PASS, `core --check` PASS, `luabind.py check UnitTemplet` PASS.
- **Cut-in table (`X2GUAra.cpp`):** `s_SkillCutInSet` = June `333a8bf0` (table `337ad900`, 4 sets x 3, GFx names): set 0 `HQ_CutIn_Ara_01` / `_11` / `_11`, 1 `_SD_`, 2 `_YR_`, 3 `_AS_`; March's `.dds` table in the `#else`. The InitState switch (June byte table, Asura = 3)
  and the reader with the 0x8b3 pair were already June's (CH1m / CH1k). New logs `[JUNE_CHAR] ARA UI cut-in set <set> name <gfx>` (reader) and `ARA UI hyper cut-in class <c> name <gfx>` (`ShowHyperSkillCutInByScript`). All 16 `.gfx` are in data152 / data195. Item override `31fd8a50` stays Stage 12.
  The CH1 play-test's 6 x `ErrorEnum(135) HQ_CUTIN_ARA_RAGE.DDS.GFX` came from March's `.dds` names: the names are `.gfx` now, so it should be gone (play-test confirms).
- **Orb handles (`X2UnitViewerUI.cpp`):** all 7 sites (ctor, dtor, `OnFrameMove`, `NotifyShowObjectChanged`, render, `PlayAnim`, `SetUnit`) retired under `_UI`, March's in `#else`. The lobby orb is the `ALD_LobbyWait` row of `UnitViewerEffectSetTable.lua` (live in data036).
  `ALD_lobbyWait` / `AYR_LobbyReadyAtcion` literals (viewer 4414-5260, `X2SquareUnit.cpp:1079`) are in the dead `#else` of `JUNE_ANIMATION_NAME_TABLE`; the live path reads `AnimationNameTable.lua` (June's `ALD_LobbyWait`, `AYR_LobbyReadyAction`): nothing to fix. 7ah is Elsword-only (n/a).
- **Creation screen:** `DLG_Unit_Create_ClassChange_Button_Ara.lua` = June's bytecode (`adopt-june data034`; `luabind.py check` PASS, record `june2017_re/june_adopt_20261111_172401.tsv`, backup `kom_backup_stage_june/20261111_172401`). June's creation-state functions read in Ghidra = March for 7 / 28 / 31 / 118 / 119:
  `SelectAra` `325548a0`, `GetViewerUnitByUnitType` `3254e2b0` (type 7 -> Eve dummy, as March), `GetUnitStatInfomationChart` `3254ee40` (3,1,2 / 3,2,2 / 3,2,2 / 3,3,3 / 3,3,3), texture name `325506e0` (`_07`), info key builders `32550bd0` / `32550f90` (class independent): no C++ change. `Movie_Create_Ara_{Base,SD,YR,AA}.avi` are loose in `data\movie`,
  `BT_ARA_*` pieces are in data056's TET. June's third-line buttons (34 / 124) show and are inert until CHNa (`GetUnitClassByUnitClassTypeAndLineType` returns `UC_NONE`).
- **Packed and copied (no ask):** `PACK OK` + `COPY OK data036` md5 `2f2dfe027e377041738a34f4ae5ad3fc` (backup `kom_backup_tools/data036.kom.20261111_172713`, 1 changed = `UnitTemplet.lua`); `COPY OK data034` md5 `0f2cd0c9629c3989259338a4c5f8016b` (1 changed).
- **Routed, not here:** guide dialog, `LevelUpEvent.lua` TYPE row, `POPUP_TR_*` pictures -> CH3d; 34 / 124 portrait (`GetCharacterImageName`), stat chart rows, `RotateYSelectCharacterType` reader -> CHNa; cut-in item override -> Stage 12.
- **Hygiene:** `flagcheck JUNE_CHAR_ARA_UI --worktree --files KTDXLIB/Always.h X2Lib/X2GUAra.cpp X2Lib/X2GUUser.cpp X2Lib/X2UnitViewerUI.cpp`: 3 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR; 0 deleted, 0 non-ASCII, 0 EOL mismatch, 0 token-less #else/#endif, RESULT: PASS.
  `junediverge --check` (3 files): allow-listed 10, RESULT: PASS. `enumscan --check X2Lib/X2GUAra.cpp`: 0 A/B rows.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 26,220,032 B, dated 2026-11-11 17:37:43, md5 af4b0c3815182f96f5f1212a78ebfbc3`. `EXESTR OK: 10 of 10` (the two log tags and the eight `HQ_CutIn_Ara_*` names, wide), `EXESTR OK: 3 of 3 absent` (March's `HQ_CutIn_Ara_normal.dds` / `_SD_Rage.dds` / `_YR_normal.dds`).
- **Save (backup made by `elsdb x`):** unit 99 `LittleDevil99` slots 0-3 = Hell of Tornado 27035, Wolf Spear 27039, Energy Wave 27040, Prison Spear 27054 (SA tier, cut-in carriers); units 98 / 48 already hold Tempest Dance 27029 (slot 1).
- **Play-test hand-off:** see the session reply. Log tag `[JUNE_CHAR]` (`ARA UI`).
- **Play-test (user, 2026-11-11 17:40-17:42, exe `af4b0c38`; logs read, on-screen results not reported).** `offline_server.log`: 8 `[JUNE_CHAR] ARA UI cut-in` lines, each with a `[JUNE_GFX] CUTIN play ui=0xc <gfx>` in the same second: set 0 (unit 99) `HQ_CutIn_Ara_01` / `_11`, set 1 (unit 98) `_SD_01` / `_SD_11` x2, set 2 (unit 48) `_YR_01` / `_YR_11` x2. `ErrorEnum(135) HQ_CUTIN_ARA_RAGE.DDS.GFX`: 0 (gone). `KEM_ERROR` 0, `Motion Error` 0, EXCEPTION 0. Lobby orb: one `UNIT_VIEWER_FX_PLAY EffectSet_ALD_LobbyWait particles=3` per viewer load (June's row only). Creation screen: Ara's dialog loaded with no missing control (the 4 `ErrorEnum(36) ClassChange*` lines are 2 s earlier, the default Elesis dialog, not this phase); `MOVIE_CREATE_ARA_{BASE,SD,YR}.LUA` `ErrorEnum(136)` = the movie name probe (Elesis has it too). Not mine: 4 `E_DISABLE_HB_CHECK_REQ` UNHANDLED, `AddGroupChainQuest` unbound, `Add_Skill_Page_ED` x3. Not reached: set 3 and the hyper / berserk cut-ins (CH3d / CHNa), the Little Specter / Asura buttons.

### CH5 - Review (2026-11-11, Opus 5.5): PASS, no code change

- **Cut-in table `333a8bf0` (Ghidra):** 12 `SkillCutIn` at `337ad900 + i * 0x30`, strings in push order `_01` / `_11` x2, `_SD_01` / `_SD_11` x2, `_YR_01` / `_YR_11` x2, `_AS_01` / `_AS_11` x2 (`33507954` .. `33507844`) = the port's 4 x 3 `s_SkillCutInSet`, exactly.
  Size / position per entry are `334367ac` = 512.0, `33435598` = 256.0: (512, 512) at (256, 512, 0) = March's one-argument `SkillCutIn( name )` defaults (`X2GUUser.h:829-833`). The log line sits after the 0x8b3 override, so it names the drawn cut-in.
- **UnitTemplet:** pre-phase (backup `data036.kom.20261111_172713`) 55 blocks, current 57, no duplicate keys; the 7 `UC_ARA_*` blocks byte-equal June's `UnitTemplet.src.lua` (7 of 7); the other 50 byte-equal pre-phase, order kept;
  semantic key delta 60 rows = the 60 non-`same` rows of `ch0_unittemplet_ara.tsv`, no mismatch. BOM + CRLF (1549 CRLF, 0 bare). `ledger --check` PASS 0 / 0, `core --check` PASS 0 / 0, `luabind.py check UnitTemplet` PASS (ENUM 0 / 0).
- **Orb retirement:** `m_hSeqEnergyBullet` uses in `X2UnitViewerUI.cpp` are all in the `#else` of `_ARA_UI`, 7 pairs (ctor, dtor, `OnFrameMove`, `NotifyShowObjectChanged`, render, `PlayAnim`, `SetUnit`); the members stay declared (`X2UnitViewerUI.h:337-339`, unread). June's `PlayAnim` `31fa3010` has no hand-made
  particle code (funcmap l.1587). `ALD_lobbyWait` (viewer 4442 / 4640 / 4837, `X2SquareUnit.cpp:1079`) and `AYR_LobbyReadyAtcion` (5288) are in the dead `#else` of `JUNE_ANIMATION_NAME_TABLE` (`cl /P`: ON, as `_ARA_UI`, `_ARA_LIVE`, `UNIT_VIEWER_EFFECT_PLAYER`, `SERV_ARA_CHANGE_CLASS_SECOND`).
- **Creation screen:** data034 `DLG_Unit_Create_ClassChange_Button_Ara.lua` = `1B 4B 4C 81`, 4964 B, sha1 `2b97f839..cce8` = the adopt record; `luabind.py check` 22 methods, 0 unbound, PASS. Spot-check `GetUnitStatInfomationChart` `3254ee40`: 7 = 3,1,2; 28 / 118 = 3,2,2; 31 / 119 = 3,3,3 = March's
  (`X2StateCreateUnit.cpp:3856-3868`). June also has 34 / 124 = 3,1,1: CHNa's (routed, below).
- **Hygiene:** `flagcheck JUNE_CHAR_ARA_UI --range 3e30cd75^..3e30cd75`: 3 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR; 0 deleted, 0 non-ASCII, 0 EOL mismatch, 0 token-less, 0 encoding, RESULT: PASS; no later commit touches the flag.
  `junediverge --check-all`: allow-listed 205, RESULT: PASS. `enumscan --check X2GUAra.cpp X2UnitViewerUI.cpp`: 0 A/B rows. No `_UI_DEBUG` define to retire.
- **Logs (exe `af4b0c38`):** 8 `[JUNE_CHAR] ARA UI cut-in` (set 0 `_01` / `_11`, set 1 `_SD_01` / `_SD_11` x2, set 2 `_YR_01` / `_YR_11` x2), each matched by a `[JUNE_GFX] CUTIN play ui=0xc`; no ErrorEnum names a cut-in file; KEM_ERROR / Motion Error / EXCEPTION 0.
  Not CH5's: 101 `ErrorEnum(431)` `.XET` misses, one per mesh for every class (Elsword, Aisha too: the engine's optional-XET probe); `QUEST_110x5_ARA_END` event-scene `.gfx` (135) and `EffectSet_ARA_Gain_Force` `APPLIED_SKILL_ID` (10142) predate CH5.
- **Kickoffs:** `CH6.md` already walks sets 0-3, the hyper / berserk cut-ins on 118 / 119 / 124 and the creation screen. `CHNa.md` amended: CH5's routed items (34 / 124 stat chart 3,1,1 from `3254ee40`, `GetCharacterImageName` rows from `31ee0de0`, the `RotateYSelectCharacterType` reader `31edf260`) were named only in this plan, not in its kickoff.
- Full build skipped: no play-test (no code change).

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

### CH3a - Done (2026-11-12)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` ( CH2a's, no new define ); `_SKILL_DEBUG` is defined again ( `Always.h`, the CH3d review retires it ). Sakra Devanam ( 118 ): Turbulent Wave 27010 / 27051, Swallow Assault 27011 / 27052, and Void Break ( state 249, no skill id ). All 34 class-map rows read from `charport_adis.py ara --phase CH3a` listings ( Ghidra decompile beside them; `32bd34c0` read from its raw listing ). **25 rows are June = March ( no code ), 8 got June bodies under the flag ( March in `#else` ), 1 is the registration, and the "new" `32bd6a20` is March's `MoveAraSafePosition` ( no new function ).** Every block carries `// char-ara CH3a`. `funcmap.tsv` rows `Char Ara CH3a` ( 36 ); `MODS.md` "Stage 7 Ara, CH3a"; `JUNE_BUGS.md` 613.

| June | `CX2GUAra::` | June differs from March |
|---|---|---|
| `32b9b3c0` | Swallow Assault Ready `StateStart` | lock-on facing range 99999, ranges 1800 / 900 ( March 1200 / 700 ), attack count 0 ( `+0x9710` ) |
| `32bae7a0` | Ready `FrameMove` | no `UseForcePower( 1 )` and no `Effectset_Swallow_Assault_Ready` at 0.001 s; the 0.818-1.020 s fade is March's |
| `32bb6950` | Attack `FrameMove` | = March plus the first attack of a use plays `Effectset_Swallow_Assault_Attack_First` into the handle ( `+0x9718` ); the count rises every attack |
| `32bc1f30` | Attack `EventProcess` | FP cancel ( not into Reflection ): June's `CanDown` ( `@32bc204c` ), `MoveAraSafePosition` off a normal line, then `StateChangeSpecialAttack`; else `ProcessLockOn` / Attack / Finish as March; probe window ( `+0x9621` ) |
| `32bb7010` | Finish `StateStart` | the same safe-position test with June's `CanDown` ( `@32bb70ff` ) |
| `32b9b490` `32b9b4e0` | Finish `FrameMove` / `EventProcess` | `FrameMove`: only `SetInvisibility( false )`, no Finish / Attack effect set; `EventProcess`: skill cancel, then the on-line walk and dash cancels |
| `32bb8850` | Turbulent Wave start `FrameMove` | = March ( `UpForcePower( 1 )` is June's inlined gain ) plus Open Mind's buff factors set again after it ( `@32bb8953` ) |
| `32bac550` | Turbulent Wave loop `EventProcess` | end through `StateChangeOrFPCancel` and the probe window ( March restored the force power itself ) |
| `32bd2ec0` `32b9c610` | Void Break `FrameMove` / `EventProcess` | `FrameMove`: rate x `SA_DAMAGE_REL` of Power of Tiger ( and Valor Devotion's buff ) scales `m_DamageData` physic, the buff damage effect and the effect set; buff factor 2 of Valor Devotion else factor 0 of Power of Tiger. `EventProcess`: on-line walk and dash cancels |
| = March | 25 functions | the 6 `Init` preload lists ( every call and string compared one by one: same calls, same order ), Ready / Attack `StateEnd` / `StateStart`, Ready `EventProcess`, Escape, the Turbulent Wave `Start` / `End` / loop `FrameMove` / `End` / end `FrameMove` / `EventProcess` ( start, end ) |

**Registration.** First switch ( `32bd09d0` ): RNSI labels added to March's Turbulent Wave ( 27010 / 27051 ) and Swallow Assault ( 27011 / 27052 ) cases; the callbacks are the same as March's `CreateState`. Second switch ( `32bd34c0` ): labels on March's Turbulent Wave case ( START 240, LOOP 241, END 242, Void Break 249 ); the three chain lookups take `eUpgradeSwappedSkillID` ( June: START / LOOP / END have one, Void Break none ); Swallow Assault has no case in June's second switch. `X2SkillTree.h`: `RNSI_SA_ASD_TURBULENT_WAVE_UPGRADE` 27051, `RNSI_SA_ASD_SWALLOW_ASSAULT_UPGRADE` 27052 ( probe-checked against 27051 / 27052, `SA_DAMAGE_REL` = 15 and the passive ids 27403 / 27416 / 27417 / 27213 ). The readers of the base ids ( `SetEquippedSkillLevel`, the passive-rate groups ) were June's since CH1i / CH1j and read the upgrade through its base.
**Amended by CH1q done here.** `m_hKillEnemyEffectSet` / `StopKillEnemyEffectSet` are renamed `m_hSwallowAssaultFirstEffectSet` / `StopSwallowAssaultFirstEffectSet` ( both `.h` / `.cpp` sites, 6 lines ); the new `m_iSwallowAssaultAttackCount` ( `+0x9710` ) is under `_SKILL`. June's `32bd6a20` ( `StateChangeSwallowAssaultEscape` ) is March's `MoveAraSafePosition`: it is called by name.
**Checklist** `(Ara: CH3a ...)`: §1 l.55 / §2 l.116 / l.117 ( the Turbulent Wave tables, the Swallow Assault / Void Break effects run through the engine ), l.121 ( `BUFF_FACTOR_ID_ULTIMATE`: re-checked, **live**: `SERV_IRUHADEV_JUNE_SKILL_TEMPLET` makes `IsUpgradedSkillLearned( 27010 )` swap to 27051, so the three lists are read for an Enhanced Turbulent Wave; no code ), §4 l.178 ( 2 names ), §7ae l.728 / l.732 ( 4 ids ), §7ac l.775-777 ( both `CanDown` calls ). Not in this phase: the FP rows for Suppression, Low Brandish and Falling Blossom in `ch0_fp.tsv` ( `proposed CH3a` is the generic "state phase" label; their states are CH2b / CH3a2's ).
**Used-force-power count.** June's Attack `EventProcess` does `UseForcePower( 1 )` and `++ +0x9614`; March's `UseForcePower` already adds `iCount_` to the count ( CH1q ), so the increment is not written twice.
**Found, not ported.** Void Break ( state 249 ) is the secret skill of the Turbulent Wave chain ( Rock Crash, Tiger Claw, Low Brandish, Turbulent Wave; the loop state past 0.45 s ): Low Brandish is CH3a2's, so its play-test goes to CH3a2 ( kickoff amended ). Swallow Assault's Finish plays no effect set from C++ in June ( the state script may ): watch the log line list below.

**Gates.**
- `flagcheck JUNE_CHAR_ARA_SKILL --with ..._SKILL_DEBUG --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2SkillTree.h KTDXLIB/Always.h`: `0 PASS, 1 DEFINE ( Always.h ), 3 OTHER ( X2GUAra.cpp / .h: CH1j / CH1q2's `_CORE` / `_LIVE` lines the rename touches; X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL, 0 ERROR`; `1 deleted line ( the //#define turned into #define ) in this flag's files, 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`. The other 7 deleted lines are the rename ( `git diff -U0`: ctor x2, one call, the function head / its test / its stop, the `.h` declaration and member ).
- `junediverge.py --check` ( the 4 files ): `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s) remain ( C 0, D 1, X 0 ignored )`. `skillicons.py check 27010-27011` and `27051-27052`: `misses 0`, PASS. `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing`, STATE `37 compared, 0 wrong`, `RESULT: PASS`.
- `/Zs` probe of `X2GUAra.cpp` ( `census.cl_cmd`, no PCH ): 0 errors; the enum probe file: 0 errors.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,286,080 B, 2026-11-12 05:48:17`; `DEPLOY OK: X2_offline.exe is 26,286,080 B, dated 2026-11-12 05:48:24, md5 53283239777b9fd8d56d9ae651adb588, equal to X2/US_SERVICE/x2.exe (2026-11-12 05:48:17); built at main 5e8c42e1 + 4 uncommitted code file(s).`
- `EXESTR OK: 2 of 2 found in X2_offline.exe, 26286080 B, md5 53283239` ( wide `Effectset_Swallow_Assault_Attack_First`, `ARA SKILL state %d skill` ).

**Save.** Unit 98 `SakraDevanamT` ( class 118, lv 99, not transcended; backup `db_backup/els_db_20261112_053735_pre_sql.sql` ): bank A slot 0 Turbulent Wave Enhanced 27051, slot 1 Swallow Assault Enhanced 27052, slot 2 Pulling Pierce Enhanced 27030, slot 3 Falling Dragon Enhanced 27031; the bases 27010 / 27011 are level 0 ( set 2 below ).

**Play-test hand-off.** In the chat of 2026-11-12 ( unit 98; set 2 on "swap" ). Lines to read: `[JUNE_CHAR] ARA SKILL equip` ( 27051, 27052 ), `ARA SKILL state` ( C++ ids: Turbulent Wave 195 / 196 after the skill state, Swallow Assault 198 / 199; June's exe ids are 240-246; no `BLANK` ), `Effectset_Swallow_Assault_Attack_First`, `Effectset_Turbulent_Wave_Stream`, `[JUNE_SKILL] NUM use`, 0 `EXCEPTION`.

**Play-test (2026-11-12, runs 05:51-05:53 and 05:55-05:56 on md5 53283239; unit 98).** Set 1 ( Enhanced ids ): equip lines 27051 / 27052 / 27030 / 27031 ( states 49 / 51 / 53 / 55 ); Turbulent Wave 49 -> 195 -> 196 twice, stream and final damage effects hit; Swallow Assault 51 -> 198 x5 -> 199 twice, `Effectset_Swallow_Assault_Attack_First` once per use ( the later attacks play the Shadow sets ), its damage effect hit; Pulling Pierce 53 / 152 twice, Falling Dragon 55. Set 2: Swallow Assault base 27011 ( 51 -> 198 x7 -> 199, then x5 ) with `Attack_First` once per use. **Base Turbulent Wave 27010 was not reached:** the emulator's `SeedAutoLearn` ( X2OfflineSkill.cpp:645, at character select ) learned 27051 again and the slot went back to it ( `SLOT sent` 27010 at 05:55:30, 27051 at 05:55:34 ), so June's upgrade swap ran for the base id; both ids share one case. 0 `BLANK` / `EXCEPTION` / `KEM_ERROR` / `Motion Error`; `OpenXET` misses are the companion-file lookup; `OpenXMesh ALH_SWALLOW_ASSAULT_BASE.Y` is June's dangling preload ( **JUNE_BUGS 613**, kept ) and `ARA_SWALLOWASSAULT3.OGG` gives sound error 47 on each attack ( sound only, not chased ). No `NUM use` lines ( the numerical debug is retired ). The user reports the checks looked normal ( cool time, MP bar, Pulling Pierce chain by eye ). Void Break was not reachable ( CH3a2 ).

### CH3a2 - Done (2026-11-12)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` ( no new define; `_SKILL_DEBUG` stays on, the CH3d stage review retires it ). Sakra Devanam ( 118 ): 27012 / 27053 Peerlessness Slash, 27043 Link Flower Slash, 27044 Continuity Strike Throwing, 27064 Advent Dragon, 27206 / 27232 Low Brandish, 27207 / 27233 Breaking, 27226 Swallow Assault Teeth. All 35 class-map rows read from `charport_adis.py ara --phase CH3a2` listings. **11 new functions, 9 bodies changed under the flag ( March in `#else` ), 15 rows are June = March ( no code ).** Every block carries `// char-ara CH3a2`. `funcmap.tsv` rows `Char Ara CH3a2` ( 37 ); `MODS.md` "Stage 7 Ara, CH3a2"; `JUNE_BUGS.md` 614.

| June | `CX2GUAra::` | June differs from March |
|---|---|---|
| `32b92480` `32b92530` | Breaking attack / avoidance `StateStart` ( new ) | Valor Devotion's buff set 1 when learned ( vslot 0x23c ), then the enhanced Breaking's ( 27233, exact id ) factor list ( vslot 0x228 ) |
| `32bb8680` | Breaking attack `FrameMove` | memo buff factor with June's 7ai arguments ( `1, NULL, 0, false` ) |
| `32b9bd00` `32b9bde0` `32b9c510` | Breaking / Breaking attack / Low Brandish kick `EventProcess` | walk and dash cancels need the foot on a line ( `_ON_LINE_` macros ) |
| `32bb03d0` | Peerlessness start `FrameMove` | 0.55 s hides the unit ( invisible, alpha 0, pre-scale kept ), 1.532 s shows it; no force gain, no 0.01 shrink, no effect sets ( the script's ) |
| `32bac5e0` | Peerlessness start `EventProcess` | end through `StateChangeOrFPCancel( 234 )`; FP cancel tried on the ground ( + foot ), then in the air |
| `32b929e0` | Peerlessness end `FrameMove` | `CommonFrameMove` only ( March plays Slash_End / Last_Attack ) |
| `32bba8f0` | Low Brandish slide `FrameMove` | effect set only: the templet lookup's result is dropped ( JUNE_BUGS 614 ) |
| `32bac760` | Low Brandish slide `EventProcess` | end through `StateChangeOrFPCancel( 248 )`, probe window |
| `32ba3ea0` `32ba40a0` `32ba42a0` `32bab370` `32bab3c0` `32b952a0` | Link Flower Slash 1st / 2nd / last ( new ) | `EventProcess` ×3 ( same body ), `StartFuture` ( window 3.0 / 0, running state id = `GetFutureStateID()` ), `EndFuture` ( window 3.0 ) |
| `32bad910` `32badac0` | Continuity Strike Throwing / Teeth start `EventProcess` ( new ) | end through `StateChangeOrFPCancel( 339 / 342 )`, probe window |
| `32b952c0` | Advent Dragon `Init` ( new ) | 13 preloads |
| = March | 15 functions | the `Init` lists, Peerlessness `Start` / `End` / end `EventProcess`, Low Brandish slide `Start` / `End`, Breaking `FrameMove` / `EndFuture`, kick `FrameMove`, the two `CreateState` helpers `32bc9230` / `32bc92a0` |

**Registration** ( `32bd09d0` ): RNSI labels on March's Peerlessness ( 27012 / 27053 ), Breaking ( 27207 / 27233 ) and Low Brandish ( 27206 / 27232 ) cases; Breaking's attack and avoidance states gain `StateStart`. New cases: 27043 ( the three chain states only, as June ), 27044 and 27226 ( normal `EventProcess`, hyper = `CopyStateCallbacks`, end state with `GenericActiveSkillEventProcess` = June `329cba60` ), 27064 ( `Init` + `GenericSpecialActiveSkillEventProcess` = `329cbfc0` ). Second switch ( `32bd34c0` ): labels 27206 / 27232 on Low Brandish, chain lookup takes `eUpgradeSwappedSkillID`. `X2SkillTree.h`: 27053, 27064, 27232, 27233.
**Checklist** `(Ara: CH3a2 ...)`: §1 l.55, §2 l.116 / l.117 ( 3 tables through the engine ), §4 l.178 ( 4 names ), §7ai ( Breaking attack BFI 0xb5 behind the false memo stub ). No 7u / 7h / power-rate rows in these functions.
**Found, not ported.** `SetCanAvoidance( 0 < GetSkillLevel( SI_A_ALH_BREAKING ) )` reads March's id 7209: Breaking's avoidance stays off until CH4a ( kickoffs CH4a / CH3b amended ).

**Gates.**
- `flagcheck JUNE_CHAR_ARA_SKILL --with ..._SKILL_DEBUG --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2SkillTree.h`: `2 PASS, 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` ( the 3 files ): `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s) remain`. `skillicons.py check` ( the 10 ids ): `misses 0`, PASS. `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing`, STATE `37 compared, 0 wrong`, PASS.
- `/Zs` probe of `X2GUAra.cpp`: 0 errors.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,287,616 B, 2026-11-12 06:24:54`; `DEPLOY OK: X2_offline.exe is 26,287,616 B, dated 2026-11-12 06:25:01, md5 6b58b2fc580988b1f8828d14eb0d7cc6, equal to X2/US_SERVICE/x2.exe (2026-11-12 06:24:54); built at main c585fab8 + 3 uncommitted code file(s).`
- `EXESTR OK: 2 of 2 found` ( wide `ASD_Dragon_Head.Y`, `SI_SA_SBH_WALL_OF_PROMINENCE_01.X` ) and `3 of 3` ( narrow `ASI_SA_ASD_ADVENT_DRAGON`, `ASI_A_ASD_SWALLOW_ASSAULT_TEETH_END`, `ASI_SA_ASD_CONTINUITY_STRIKE_THROWING_END` ), md5 6b58b2fc.

**Save.** Unit 98 `SakraDevanamT` ( 118, lv 99; backup `db_backup/els_db_20261112_061728_pre_sql.sql` ), set 1, all Enhanced ids ( already learned ): bank A slot 0 Rock Crash 27223, 1 Tiger Claw 27229, 2 Low Brandish 27232, 3 Turbulent Wave 27051; bank B slot 5 Breaking 27233, 6 Swallow Assault Teeth 27226, 7 Peerlessness Slash 27053, 8 Link Flower Slash 27043.

**Play-test hand-off.** In the chat of 2026-11-12. Logs: `[JUNE_CHAR] ARA SKILL state <n> skill <id>` ( one per state entered, no `BLANK` ), 0 `EXCEPTION` / `Motion Error`.

**Play-test (2026-11-12, runs 06:30-06:33 and 06:35-06:37 on md5 6b58b2fc; unit 98).** Set 1: equip lines for the 8 Enhanced ids; Void Break chain 49 -> 51 -> 53 -> 55 -> 195 -> 203 ( `DAMAGE_EFFECT_VOID_BREAK_BUFF` hit ) x3; Low Brandish slide effect set; Peerlessness Slash 63 -> 188 with Start / Attack_Effect / Attack / End / Last_Attack from the script only; Link Flower Slash 275 -> 276 -> 277 ( chain window works ); Swallow Assault Teeth 61 -> 281; Breaking 59 -> 175. Set 2: Continuity Strike Throwing 49 -> 278 x3 ( start / projectile effect sets and damage effects hit ), Advent Dragon ( ready, attack and explosion damage effects hit ). 0 `BLANK` / `EXCEPTION` / `KEM_ERROR` / `Motion Error`; the `OpenXET` misses are the companion-file lookup. The user reported set 1 all fine. **Not reached:** Low Brandish's kick state ( the slide was always chained on into Turbulent Wave; the kick body = March plus on-line cancels ), Breaking's avoidance ( CH4a ), the base ids 27206 / 27207 / 27012 ( only the Enhanced ids are learned in the save ). Unit 98 now holds set 2 ( 27044, 27064, 27232, 27051, 27233, 27226, 27053, 27043 ).

### CH3b - Done (2026-11-12)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` ( no new define; `_SKILL_DEBUG` stays on, the CH3d stage review retires it ). Yama Raja ( 119 ): 27018 / 27056 Energy Bead, 27019 / 27057 Energy Void, 27020 / 27058 Steal Soul, 27212 / 27237 High Speed, 27213 / 27238 Reflection, 27045 Restriction Energy, 27046 Reverse Gravity Field, 27065 Raksasa Fire, 27227 Gain Good Energy Ball. All 44 class-map rows read from `charport_adis.py ara --phase CH3b` listings plus the registration rows of `32bd09d0` ( `ch0_skill_listing_rows.tsv` ). **6 new functions + 1 helper, 10 June bodies changed under the flag ( March in `#else` ), 18 functions are June = March ( no code ), 10 outlined case helpers = the registration rows.** Every block carries `// char-ara CH3b`. `funcmap.tsv` rows `Char Ara CH3b` ( 47 ); `MODS.md` "Stage 7 Ara, CH3b"; `JUNE_BUGS.md` 615.

| June | `CX2GUAra::` | June differs from March |
|---|---|---|
| `32b933e0` | High Speed `StateStartFuture` | speed value through `TryGetEquippedSkillAbilityValue( SA_REMOTE_SPEED_ABS, 27212, swap )`, a failed read keeps the old value |
| `32b9db10` `32b9e6e0` `32b9e8a0` | High Speed / Steal Soul finish / Energy Void end `EventProcess` | walk and dash cancels need the foot on a line ( `_ON_LINE_` macros ) |
| `32bacfa0` | Energy Bead `EventProcess` | end through `StateChangeOrFPCancel( 268 )`, probe window, cancels on a line ( = Prison Spear's shape ) |
| `32bad260` | Steal Soul `EventProcess` | end through `StateChangeOrFPCancel( 272 )` and nothing else that frame ( the FP test is the else branch ), probe window |
| `32baf670` | Steal Soul finish `FrameMove` | `AttackResult()` ( vslot 0x64 ) after each unit pulled down |
| `32bc2c50` | Energy Void loop `StateStart` | force power is spent ( and the attack effect made ) from charge 5, March 4; June's +1 on the used-force-power count ( `+0x9614` ) is not ported: CH1q dropped that member and nothing reads it |
| `32b9e820` | Energy Void loop `EventProcess` | a released key ends the loop from charge 5, March 4 ( the key calls are CH1p's ) |
| `32bbae10` | Energy Void end `FrameMove` | templet and level of June's 27019 ( upgrade swapped level ) |
| new: `32b95370` | Raksasa Fire `Init` | 18 preloads |
| new: `32ba44a0` `32bc56d0` `32bad9a0` `32bc9810` | Restriction Energy start `StateStart` / `FrameMove` / `EventProcess`, end `FrameMove` | at 0.271 s the first enemy in a cone ( `CollectEnemyUnitsInCone`, Eve's ) gets the effect set and its UID + type are kept; the end state finds it again ( `GetUnitByUIDAndType` = `322d80a0` ) and, if the hit flag is set ( **CH4a** ), gives it the buff factor and the end effect set |
| new: `32bada30` | Reverse Gravity Field start `EventProcess` | end through the FP cancel ( 341 ) |
| = March | 18 functions | the 9 `Init` lists, High Speed Ready `EventProcess`, High Speed `EndFuture`, Energy Bead / Steal Soul `StateStart`, Energy Void start `StateStart` / `EventProcess` ( `StartFuture` is CH1p's ), end `StateStart`, Open the Hell `EventProcess` ( the funcmap rows say which ) |

**Registration** ( `32bd09d0` ): RNSI labels on March's cases for 27212 / 27237, 27018 / 27056, 27020 / 27058, 27019 / 27057 and 27213 / 27238 ( Reflection: the sound preload only, as June ); new cases 27045 ( start `StateStart` + `FrameMove` + `EventProcess`, end state 340 with `FrameMove` + `GenericActiveSkillEventProcess` ), 27046 ( start `EventProcess`, end state 341 with `GenericActiveSkillEventProcess` ), 27065 ( `Init` + `GenericSpecialActiveSkillEventProcess` ), 27227 ( `GenericActiveSkillEventProcess` ); hyper = `CopyStateCallbacks`. Second switch ( `32bd34c0` ): labels 27018 / 27056 and 27020 / 27058, the chain lookup takes `eUpgradeSwappedSkillID`. `X2SkillTree.h`: 27056, 27057, 27058, 27065, 27227, 27237.
**Checklist** `(Ara: CH3b ...)`: §1 l.55, §2 l.116 / l.117 ( the Steal Soul finish table + the others through the engine ), §4 l.178 ( 6 names ), §7ai ( High Speed Ready's `..._ULTIMATE` key is script data ). §7ae l.713-745 / l.726 carry no Ara tags ( the skill ids are bound by the registration ).
**Found, not ported ( CH4a, kickoff amended ).** `MarkRestrictionEnergyHit` ( sets the hit flag ): until then Restriction Energy's end buff and end effect set never play. `AddHitUnitInfoByDamageData` still lists March's `SI_A_AYR_STEAL_SOUL`: Steal Soul's finish has no unit to pull down until June's id is read there.

**Gates.**
- `flagcheck JUNE_CHAR_ARA_SKILL --with ..._SKILL_DEBUG --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2SkillTree.h`: `2 PASS, 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`. After the commit ( `e3137822` ): `flagcheck --range HEAD^..HEAD --files` the same 3 files: `2 PASS, 1 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s)`, `RESULT: PASS`.
- `junediverge.py --check` ( the 3 files ): `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s) remain`. `skillicons.py check` ( the 14 ids ): `misses 0`, PASS. `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing`, STATE `37 compared, 0 wrong`, PASS.
- First build failed ( `m_ForcePowerSystem` is not a member under `_FP`: June's +1 on the used-force-power count at `32bc305c` is not ported, CH1q dropped the member ); second: `BUILD OK: X2/US_SERVICE/x2.exe 26,292,224 B, 2026-11-12 07:09:10`; `DEPLOY OK: X2_offline.exe is 26,292,224 B, dated 2026-11-12 07:09:17, md5 dbbed0b37dc9d2db1cebe1609144279f, equal to X2/US_SERVICE/x2.exe (2026-11-12 07:09:10); built at main 0e8089cb + 3 uncommitted code file(s).`
- `EXESTR OK: 3 of 3 found` ( wide `EffectSet_SA_AYR_RESTRICTION_ENERGY_End`, `AYR_RaksasaFire_Dummy1.X`, `eve_SA_photonFlare_lightLine.Y` ) and `4 of 4` ( narrow `ASI_SA_AYR_REVERSE_GRAVITY_FIELD_START`, `ASI_A_AYR_GAIN_GOOD_ENERGY_BALL`, `ASI_SA_AYR_RAKSASA_FIRE`, `ASI_SA_AYR_RESTRICTION_ENERGY_END` ), md5 dbbed0b3.

**Save.** Unit 48 `Ara99` ( 119, lv 99; backup `db_backup/els_db_20261112_065946_pre_sql.sql` ), set 1: bank A slot 0 Energy Bead 27056, 1 Energy Void 27057 ( added at level 1 ), 2 Steal Soul 27058, 3 High Speed 27237; bank B slot 5 Restriction Energy 27045, 6 Reverse Gravity Field 27046, 7 Raksasa Fire 27065, 8 Gain Good Energy Ball 27227. Reflection ( 27238 ) and the base ids are the second set.

**Play-test hand-off.** In the chat of 2026-11-12. Logs: `[JUNE_CHAR] ARA SKILL state <n> skill <id>` ( one per state entered, no `BLANK` ), `[JUNE_SKILL]` `NUM use`, 0 `EXCEPTION` / `Motion Error`.

**Play-test ( 2026-11-12, run 07:13-07:15 on md5 dbbed0b3; unit 48 ).** All 8 equip lines ( 27056, 27057, 27058, 27237, 27045, 27046, 27065, 27227 ). States entered: Energy Bead 50 ( chained into Prison Spear 223, skill 27054 ); Steal Soul 54 -> 226 ( `ES_CREATE EffectSet_A_AYR_STEAL_SOUL_END` ); Energy Void 52 -> 229 x5 -> 230 twice, plus a third use ( `EffectSet_SI_SA_AHD_ENERGY_VOID_FX_Hit` made at each end, line 1310 / 1351 / 1427 ); High Speed 56 -> 212; Restriction Energy 60 -> 279 ( `EffectSet_SA_AYR_RESTRICTION_ENERGY` on the target, hyper=1 ); Reverse Gravity Field 62 -> 280; Raksasa Fire 64; Gain Good Energy Ball 66. 0 `BLANK` / `EXCEPTION` / `KEM_ERROR` / `Motion Error`; the `OpenXET` misses are the companion-file lookup; the one unbound name ( `AddGroupChainQuest` ) is a quest script's. **Not reached / no line:** `NUM use` and `SBUF` lines ( none printed by this build ), Restriction Energy's end effect ( CH4a ), Reflection ( second set ), the base ids.

### CH3c - Done (2026-11-12)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` ( no new define; `_SKILL_DEBUG` stays on, the CH3d stage review retires it ). Asura ( 124 ) SA tier: 27026 / 27061 Get Blood ( + Smash chain state ), 27027 / 27062 Fire Fox, 27028 / 27063 Necromancy, 27047 Shadow Rising, 27048 Specter of Blade, 27066 Fox Phantom. All 43 class-map rows read from `charport_adis.py ara --phase CH3c` listings. **40 new functions + 2 stand-ins ( Fox Phantom's `FrameMove` / `StateEnd` ), 3 `CreateState` helpers ( = June's helpers `32bc9680` / `32bc96f0` / `32bc97b0` ), 6 new cases in the first id switch, 1 in the second ( `32bd34c0` ); no June body went inside a March function** ( none of these skills exists in March; March's `ASI_SA_ARA_SHADOW_RISING_*` stay for the old skill ). Every block carries `// char-ara CH3c`. `funcmap.tsv` rows `Char Ara CH3c` ( 44 ); `MODS.md` "Stage 7 Ara, CH3c"; `JUNE_BUGS.md` 616-618.

| June | `CX2GUAra::` | What it does |
|---|---|---|
| `32b91390` `32b914a0` `32b915c0` + `32bb50c0` `32bb5310` `32bb5600` + `32b9ab00` `32bac240` `32b9ab40` | Shadow Rising jump / landing / end ( `Init`, `FrameMove`, `EventProcess` ) | jump: effect set, 1 force power, Open Mind's buff; landing: effect set, end through the FP cancel; end: effect set, wait / jump-down |
| `32b94cd0` `32b94d90` `32b94d70` `32bc4b90` `32bbdf00` `32bad790` `32ba2aa0` | Specter of Blade + finish | 0.4 s up to 3 force power -> damage rate ( skill id 7021, JUNE_BUGS 616 ), damage effect at the hip; the finish repeats it; end through the FP cancel ( ground, then air ) |
| `32b94e40` `32ba2af0` `32bbe060` `32bad880` `32bbe2a0` + finish `32b94ec0` `32bbe2f0` `32ba2b30` `32bbe500` | Get Blood + finish | 0.4 s the attack at the feet, 1.01 s up to 2 force power for its hits; the finish attacks 75 in front of the hip at 0.47 s; both ends let the attack go |
| `32b95210` `32ba3820` `32bbf500` `32ba3860` | Smash ( chain state of Get Blood ) | start lowers every left cool time by Spiritual Eye's rate; effect sets at 0.001 / 0.25 / 0.63 s, `_TRENSCENDENCE` twins while a transcended hyper mode runs ( 27420 learned ), power rate x `+0x9754` ( CH4a writes it ); the plain impact set does not exist in June ( JUNE_BUGS 617 ) |
| `32b94f00` `32b94ff0` `32b95010` `32bc4d50` `32ba2e50` | Fire Fox | 0.01 s fox kind ( fire / frozen / shock ), up to 4 force power ( 9 + n foxes ), a fox every 0.035 s with the angle jittered by the count's parity, main effect life 2 s ( PvP / PvE ) / 3 s; 0.4 s the scratch effect 330 in front |
| `32b94600` `32b94690` `32ba1690` | Necromancy | the state sets the fox effect set's power rate; the standard end / cancel chain ( same body as Get Blood finish, Fire Fox, Smash ends ) |
| `32b95460` | Fox Phantom `Init` | 15 preloads; June has no `FrameMove`: the script's `CUSTOM_RENDER_STATE { 1.5, 2.4, CRS_INVISIBLE }` is a **stand-in** ( 7w ): `CRS_INVISIBLE` = 5 in Ara's render state, `ApplyRenderParam` case as June `31fd4460` case 5 ( cartoon, alpha 0 ) |

**Registration** ( `32bd09d0`, `32bd34c0` ): hyper forms = `CopyStateCallbacks` ( or the same `CreateState` ); Fox Phantom: `Init` + `GenericActiveSkillEventProcess` ( June `329cba60` ); the second switch lists Get Blood / its finish / Smash with the chain lookup on the swapped id for Get Blood only. `X2SkillTree.h`: 27027, 27061, 27062, 27063, 27066, 7021 ( `SI_SA_ALS_SPECTER_OF_BLADE` ).
**Checklist** `(Ara: CH3c ...)`: §1 l.55, §2 l.116 / l.117, §4 l.178 ( 6 names ), §7w ( the stand-in ). §7 l.256: none of the 43 writes the state rate; §7h: no `USEFUL_VALUE` read; §7ai: no BFI call; §7u: all effect names are in June's `EffectSet.lua` / `DamageEffect.lua` except `EffectSet_AA_Smash_Impact` ( June-undefined ).
**Found, not ported.** June's `+0x9614` used-force-power adds ( JUNE_BUGS 618 ). The memo effect set of Shadow Rising's landing ( June's false stub ). Nothing else.

**Gates.**
- `flagcheck JUNE_CHAR_ARA_SKILL --with ..._SKILL_DEBUG --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2SkillTree.h`: `2 PASS, 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` ( the 3 files ): `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s) remain`. `skillicons.py check 27026-27066`: `skills 41, misses 0`, PASS. `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing`, STATE `37 compared, 0 wrong`, PASS.
- `/Zs` probe of `X2GUAra.cpp`: 0 errors.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,316,800 B, 2026-11-12 07:49:38`; `DEPLOY OK: X2_offline.exe is 26,316,800 B, dated 2026-11-12 07:49:45, md5 248fc58a6700df6346198dac2a321d4d, equal to X2/US_SERVICE/x2.exe (2026-11-12 07:49:38); built at main 94430255 + 3 uncommitted code file(s).`
- `EXESTR OK: 4 of 4 found` ( wide `EffectSet_SMASH_Damage_TRENSCENDENCE`, `DamageEffect_SPECTER_OF_BLADE_Damage_Finish`, `DAMAGE_EFFECT_SI_SA_AA_FIRE_FOX_FROZEN`, `ASR_FOX_PHANTOM_FOX.X` ) and `4 of 4` ( narrow `ASI_SA_ASR_FOX_PHANTOM`, `ASI_SA_AA_FIRE_FOX`, `ASI_SA_AA_GET_BLOOD_FINISH`, `ASI_SA_ASR_SHADOW_RISING_END` ), md5 248fc58a.

**Save.** Unit 104 `Asura99` ( 124, lv 99; backup in `db_backup/`, tag `pre_sql` ), set 1: bank A slot 0 Get Blood 27061, 1 Fire Fox 27062 ( added at level 1 ), 2 Necromancy 27063, 3 Shadow Rising 27047; bank B slot 5 Specter of Blade 27048, 6 Fox Phantom 27066. Slots 4 / 9 empty ( no transcendence ).

**Play-test hand-off.** In the chat of 2026-11-12. Logs: `[JUNE_CHAR] ARA SKILL state <n> skill <id>` ( one per state entered, no `BLANK` ), `ES_CREATE EffectSet_Shadow_Rising_Jump` / `_Landing` / `_End`, `EffectSet_SI_SA_AA_SMASH_TRENSCENDENCE` only when transcended, 0 `EXCEPTION` / `Motion Error`.

**Play-test ( 2026-11-12, run 08:19-08:21 on md5 248fc58a; unit 104 ).** All 6 equip lines ( 27061, 27062, 27063, 27047, 27048, 27066 ). States entered: Specter of Blade 60 -> 268 ( finish ) twice and a third use; Fox Phantom 62 x4; Get Blood 50 -> 272 ( finish ) x3 ( `ES_CREATE EffectSet_SI_A_AA_GET_BLOOD_FX1` ); Fire Fox 52 x2; Necromancy 54 x2 ( `EffectSet_SI_SA_AA_NECROMANCY_SMILE_FOX_FX2`, then the fox attack sets `..._FOX_ATTACK0 / 1` ); Shadow Rising 56 -> 255 -> 256 x3 ( `ES_CREATE EffectSet_Shadow_Rising_Landing` and `_End` ). 0 `BLANK` / `EXCEPTION` / `KEM_ERROR` / `Motion Error`; the `OpenXET` misses are the companion-file lookup; the one unbound name ( `AddGroupChainQuest` ) is a quest script's. **Not reached / no line:** Smash ( the Get Blood chain was not entered ), the particle-only Shadow Rising jump set and the Fire Fox / Specter of Blade damage effects ( the log prints no line for them: the user's eye decides ), Fox Phantom's invisible window ( visual ), the transcendence effect sets ( unit not transcended ). The user's own verdict is not in yet.

### CH3c2 - Done (2026-11-12)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` ( no new define; `_SKILL_DEBUG` stays on, the CH3d stage review retires it ). Asura ( 124 ) A tier: 27218 / 27242 Raid ( dash, attack, finish states ), 27219 / 27243 Specter Walk, 27228 Fox Ghost. All 19 class-map rows read from `charport_adis.py ara --phase CH3c2` listings. **18 new functions + 1 shared helper ( `CreateRaidAttackEffect` = the hit June writes out three times ), 3 `CreateState` helpers ( = June `32baab80` / `32bce970` / `32bc94a0` ), 3 cases in the first id switch, 1 in the second ( `32bd34c0`, Raid ); no June body went inside a March function** ( none of these skills exists in March ). Fox Ghost has no function of its own ( June: generic end only ). Every block carries `// char-ara CH3c2`. `funcmap.tsv` rows `Char Ara CH3c2` ( 20 ); `MODS.md` "Stage 7 Ara, CH3c2"; `JUNE_BUGS.md` 619.

| June | `CX2GUAra::` | What it does |
|---|---|---|
| `32b94420` `32b94440` `32b94460` `32ba0fe0` | Raid dash `StateStartFuture` / `StateStart` / `StateEnd` / `EventProcess` | clears the Raid hit flag ( +0x97bc, set by CH1h2's attack-result code ); start copies the force-gain count; on a hit or animation end -> attack state 295 |
| `32bc3580` `32bad550` | Raid attack `FrameMove` / `EventProcess` | 0.1 s one force power -> damage rate ( +0x97c0 ), hit `DamageEffect_AAS_Raid_Attack` at 0.1 / 0.4 / 0.52 s, 150 in front of the hip ( behind it facing left ); end through the FP cancel -> finish 296 |
| `32bbd3a0` `32ba1020` | Raid finish `FrameMove` / `EventProcess` | 0.222 s `DamageEffect_AAS_Raid_Attack_Finish` at the hip, again with rotation y + 180; animation end -> wait, foot off the line -> fall |
| `32ba6560` | `ApplyRaidRangeRate` | `SA_SIZE_ABS` of Raid multiplies the effect's scale base ( +0xb24 ), then `SetScale( 1, 1, 1 )` |
| `32bc31f0` `32ba09a0` | Specter Walk `FrameMove` / `EventProcess` | 0.001 s one force power; end -> wait / fall, the plain skill cancels on a line |
| 4 `Init` | Specter Walk 14, Raid 4, attack 10, finish 8 preloads | |

**Registration** ( `32bd09d0`, `32bd34c0` ): Raid = helper for normal and hyper plus states 295 / 296 in `m_StateList`; second switch: all three states with `SetSkillLevelStateData`, 294 and 295 with the chain lookup on the swapped id ( `@32bd4e2e`, `@32bd4ec5` ). `X2SkillTree.h`: 27219, 27228, 27242, 27243.
**Checklist** `(Ara: CH3c2 ...)`: §4 l.178 ( 4 names ), §7h l.426 ( Raid's `_FINAL` set ), §7ai ( Specter Walk's `BUFF_FACTOR_ID_AND_APPLY_TIME` is script data ). §7 l.256: none of the 5 writes the state rate; §7u: every effect name is in June's `EffectSet.lua` / `DamageEffect.lua` ( closure rows all `june-bytes`; the 10 sound rows `missing` in the census were staged by CH0b ).
**Found, not ported.** June's `+0x9614` used-force-power adds ( JUNE_BUGS 619, as 618 ); Specter Walk's 2-force-power branch ( memo 0x94341, June's stub never holds it ). Nothing else.

**Gates.**
- `flagcheck JUNE_CHAR_ARA_SKILL --with ..._SKILL_DEBUG --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2SkillTree.h`: `2 PASS, 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` ( the 3 files ): `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s) remain`. `skillicons.py check 27218-27243`: `skills 26, misses 0`, PASS. `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing`, STATE `37 compared, 0 wrong`, PASS.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,323,456 B, 2026-11-12 08:43:18`; `DEPLOY OK: X2_offline.exe is 26,323,456 B, dated 2026-11-12 08:43:26, md5 5e92ccd3008296d379dbd58a9cbf2316, equal to X2/US_SERVICE/x2.exe (2026-11-12 08:43:18); built at main ec412103 + 3 uncommitted code file(s).`
- `EXESTR OK: 3 of 3 found` ( wide `DamageEffect_AAS_Raid_Attack_Finish`, `AA_RAID_MESH_TRACE_F01.X`, `ALS_SPECTER_WALK_P01.X` ) and `3 of 3` ( narrow `ASI_A_AA_RAID_ATTACK_FINISH`, `ASI_A_ALS_SPECTER_WALK`, `ASI_A_ASR_FOX_GHOST` ), md5 5e92ccd3.

**Save.** Unit 104 `Asura99` ( 124, lv 99; backup `db_backup/els_db_20261112_083554_pre_sql.sql` ): bank A slot 0 Raid 27242, slot 1 Specter Walk 27243; bank B slot 5 Fox Ghost 27228. Other slots empty ( CH3c's cleared ).

**Play-test hand-off.** In the chat of 2026-11-12. Logs: `[JUNE_CHAR] ARA SKILL state <n> skill <id>` ( one per state entered, no `BLANK` ), `ES_CREATE EffectSet_SI_A_AA_RAID_DASH` / `_RAID_ATTACK_FX1` / `_RAID_FINISH`, `EffectSet_SI_A_ALS_SPECTER_WALK_Start`, `EffectSet_A_ASR_Fox_Ghost`, 0 `EXCEPTION` / `Motion Error`.

**Play-test ( 2026-11-12, run 08:46-08:49 on md5 5e92ccd3; unit 104 ).** All 3 equip lines ( 27242 / 27243 / 27228 ). States entered: Raid 49 -> 260 -> 261 x7 ( `EffectSet_RaidAttack_Find_Target` made at each start; `alpha set=` lines for `EffectSet_SI_A_AA_RAID_DASH`, `_ATTACK_FX1` / `FX2` / `FX3`, `_FINISH`; `HIT DamageEffect_AAS_Raid_Attack` and `C3 ATTACK_SUCCESS ... owner bHit set` at 08:47:00.520, so the hit flag path works; the finish effect `DamageEffect_AAS_Raid_Attack_Finish` created ), twice chained from the attack state into Get Blood ( state 271, skill 27061, the script chain ); Specter Walk 51 x2 ( `ES_CREATE EffectSet_SI_A_ALS_SPECTER_WALK_Start`, then `EffectSet_Specter_Walk_Buff` and the `..._Display_Debuff1-3` sets ); Fox Ghost 59 x3 ( `ES_CREATE EffectSet_A_ASR_Fox_Ghost` ). 0 `BLANK` / `EXCEPTION` / `KEM_ERROR` / `Motion Error`; the `OpenXET` misses ( `ALS_SPECTER_WALK_P01`, `AA_RAID_MESH_TRACE01` / `02`, `AA_RAID_HAND_F01`, `AA_RAID_MESH_TRACE_F01` ) are the companion-file lookup; the one unbound name ( `AddGroupChainQuest` ) is a quest script's. **Not reached / no line:** the `[JUNE_SKILL]` SBUF lines are not printed by this build ( as CH3b ); the size-scaled hit ( `SA_SIZE_ABS` ) leaves no log line.

### CH3d - Done (2026-11-12)

Opus 5.5. Flags `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` ( no new define; `_SKILL_DEBUG` still on, the CH3d review retires it ) and new `SERV_IRUHADEV_JUNE_CHAR_ARA_TRANSCEND` ( needs `_ARA_LIVE` and `_RENA_TRANSCEND`, the chains it extends ). `MODS.md` "Stage 7 Ara, CH3d"; `funcmap.tsv` rows `Char Ara CH3d` ( 8 ); checklist rows marked `(Ara: CH3d ...)` ( §4 l.178, 7t l.595 / 599 / 611, 7ae l.728, 7ag l.816, 7ao l.963 / 967 / 986 ). No JUNE_BUGS row, no divergent function. Review: `CH3d-review.md` ( the CH3 stage review, CH3a-CH3d ).

**Hyper actives ( the 8 class-map functions, `charport_adis.py ara --phase CH3d` ).**
- **June's split.** `32bd09d0 @32bd0bb6`: `id > 0x6bd0` -> jump table `32bd2a24` ( 27601 / 27604 -> `@32bd26d8` Energy Cannon helper `32baab30` x2; 27602 / 27605 -> `@32bd26fb` Nine-Tailed Fox helper `32baace0` x2; 27603 -> `@32bd2685` ); `== 0x6bd0` -> `@32bd2685` ( Thousands Blossoms inline ); anything else falls to the main table, whose default is Wind Wedge ( `@32bd271e` ). That is MSVC's tree for one switch, so the ids go into March's own switch as cases ( CH1n's comment at `X2GUAra.cpp:3389` kept: old comments are not rewritten ).
- **= March (5):** Thousands Blossoms Init `32b92fc0` ( 13 preloads, same names and order ) and EventProcess `32b9cf50`; Energy Cannon Init `32b93ec0` ( 14 textures + 3 meshes ) and EventProcess `32b9ed80` ( byte-identical to `32b9cf50`: animation end -> wait on a line, else jump down and 45 lower, then CommonEventProcess ); the Energy Cannon helper `32baab30` = March's `ASI_HA_AYR_ENERGY_CANNON_CreateState` ( table, Init, EventProcess; the `+0x11 = 1` stores are `IsSkillSlotState`, CH1n ). The inline Thousands Blossoms body ( table `@33507168`, Init / EventProcess, hyper copies `+0x14/+0x18`, `+0x44/+0x48` ) = March's case. Cases `RNSI_HA_ASD_THOUSANDS_BLOSSOMS( _UPGRADE )` and `RNSI_HA_AYR_ENERGY_CANNON( _UPGRADE )` are added to March's cases under `_SKILL`.
- **New (3), Asura 124:** `ASI_HA_AA_NINE_TAILED_FOX_CreateState` ( `32baace0`, table `@335028fc` ), `_Init` ( `32b95290`: a bare `ret`, kept empty so the callback matches June ) and `_EventProcess` ( `32ba3b80`: animation end -> wait / jump down 45, then `SKILL_CANCEL_AFTER` with the awakening branch, walk, back walk, dash and ZX cancels on a line = Ara's CH2d macros, the same body as Specter Walk's ). Case 27602 / 27605 in the CH3c2 block.
- **Second switch:** none. `32bd34c0` covers 27001-27242 ( `lea eax, [ebx - 0x6979]; cmp eax, 0xf1` ), so 27600+ get no `SetSkillLevelStateData` there, as in June.
- **Names:** `X2SkillTree.h` ( `JUNE_ENUM_NAMES` ) 27600-27605 from June's `Enum.src.lua` ( 14840 / 14440 / 14352 / 16277 / 14550 / 15944 ). The state tables are June's script ( `Ara_Martial_Artist.src.lua` 13705 / 15784 / 18763 ).
- **7t:** the 6 `SHOW_HYPER_CUT_IN_TIME` tables are June's script, read by the engine since CH1a; the type-13 cut-in is the engine's. No CH3d code.

**Transcendence rows (7ao).**
- Allow-list: `TranscendCheck` takes 118 / 119 / 124 ( a brace-less guard after Rena's, `Handlers_Quest.cpp:1438` ).
- Pictures ( `GetCharacterImageName`, `X2Data.cpp` ): prefixes `S_DEVANAM` / `Y_RAJA` / `ASURA` from June's row pushes ( subagent, Ghidra read-only on the static initializer `33374620`; spot-checked `@333777b2` `TR_S_DEVANAM_LARGE` `@3345e24c` -> `DLG_UI_Common_Texture101_NEW.tga` `@3345e208` ). Large + gray are **Texture101** ( `_NEW` / `_GRAY`; a new branch ahead of the Eve / Chung / Raven Texture99 test ); 50by50 / 20by20 / party Texture100, gauge `TR_*_STATE` Texture97, unit slot `list_TR_*` `GUI_character_01` ( June spells `list_TR_S_Devanam`; the TET stores `LIST_TR_S_DEVANAM`, as Rena's mixed-case rows ). No gauge override. Every piece is in the staged TETs ( data215 Texture97 / 100 / 101_NEW / 101_GRAY, data057 / datai021 `GUI_character_01` ).
- `LevelUpEvent.lua` ( data036, BOM source ) gets `TYPE6` = June's `TYPE6` ( `LevelUpEvent.src.lua:566-584`: LEVEL 70, `UC_ARA_SAKRA_DEVANAM` / `_YAMA_RAJA` / `_ASURA`, `DLG_Transcendental_Change_Guide_Ara.lua`, `AraVoice_ClassUpReady01 / 02.ogg` ( data134 ) ). `luasrc.py refresh`, `LEDGER CHECK PASS: 0 FAIL, 0 WARN`; `PACK OK data036` ( `~1 changed: LevelUpEvent.lua` ); `COPY OK data036` md5 21cb2201.
- Already June's, verified: the guide dialog ( `adopt-june --dry-run data034`: `1 identical` ); `POPUP_TR_S_DEVANAM / _Y_RAJA / _ASURA` ( data215 ); `HQ_Cutin_ASD / AYR / ASR_Transcendence.gfx` and the `_1_` variants ( data195 ); `GetHyperModeTableName` 118 / 119 / 124 ( `X2GUUser.cpp:34158-34164` ); 18 `SPRT_HYPER_CLASS` ids ( 27043-27048, 27064-27066, 27226-27228, 27415-27420 ) and 3 `TRANSCENDENTAL_SKILL_DATA` rows ( `NewSkillTempletVer2.lua` / `SkillCommandDescriptionData.lua` identical to June's ); the hyper effect start's transcended return ( CH1q, `X2GUAra.cpp:9872` = June `@32bb24c1`, `_FP` on ). Four base helpers: CX2GUUser's, not copied.
- **Skill icon gate:** `skillicons.py check 27000-27999`: `skills 139, misses 0, known June gaps 1, sheets 0`, `RESULT: PASS` ( 27414 `SI_P_AA_SPIRITUAL_EYE` ).

**Gates.**
- `flagcheck JUNE_CHAR_ARA_SKILL --with ..._SKILL_DEBUG --worktree --files X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2SkillTree.h`: `2 PASS, 0 PASS~, 0 DEFINE, 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `flagcheck JUNE_CHAR_ARA_TRANSCEND --worktree --files KTDXLIB/Always.h X2Lib/Offline/Handlers_Quest.cpp X2Lib/X2Data.cpp`: `2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s)`, the rest 0, `RESULT: PASS`.
- `junediverge.py --check` ( the 6 files ): `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check` the three `.cpp`: 4 A rows, all pre-existing ( `X2Data.cpp` 3056-3058, `Handlers_Quest.cpp:1421` `< 100` ). `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing`, STATE `37 compared, 0 wrong`, `RESULT: PASS`.
- `DEPLOY OK: X2_offline.exe is 26,324,992 B, dated 2026-11-12 09:12:18, md5 0c1fc75410a2bd98010b441bfe414f91, equal to X2/US_SERVICE/x2.exe (2026-11-12 09:12:11); built at main c3ac5df5 + 6 uncommitted code file(s).`
- `EXESTR OK: 2 of 2 found` ( narrow `ASI_HA_AA_NINE_TAILED_FOX`, `S_DEVANAM` ) and `4 of 4` ( wide `DLG_UI_Common_Texture101_NEW.tga`, `S_DEVANAM`, `Y_RAJA`, `ASURA` ), md5 0c1fc754.

**Play-test hand-off.** In the chat of 2026-11-12. Logs: `[JUNE_CHAR] ARA SKILL equip skill 27600 / 27601 / 27602 slot 4 bank A` and `27064 / 27065 / 27066 slot 4 bank B`, `ARA SKILL state <n> skill <id>` for each cast ( no `BLANK` ), `[JUNE_TRANSCEND] IMAGE class 118 / 119 / 124` rows ( type 2 / 3 -> `Texture101_NEW` / `_GRAY`, type 7 -> `Texture97_NEW TR_*_STATE`, type 9 -> `list_TR_*` ), the transcended cut-in, 0 `EXCEPTION` / `Motion Error` / `KEM_ERROR`.

- Post-commit ( `2666c763` ): `flagcheck JUNE_CHAR_ARA_SKILL --with ..._SKILL_DEBUG --range 2666c763^..2666c763`: `2 PASS, 0 DEFINE, 4 OTHER ( other flags: X2SkillTree.h JUNE_ENUM_NAMES, Always.h / X2Data.cpp / Handlers_Quest.cpp = _ARA_TRANSCEND ), 0 FAIL`, `0 deleted line(s)`, `RESULT: PASS`; `_ARA_TRANSCEND --range` same commit, `--files` the three: `2 PASS, 1 DEFINE, 0 OTHER, 0 FAIL`, `0 deleted line(s)`, `RESULT: PASS`.

**Play-test ( 2026-11-12, run 09:16-09:19 on md5 0c1fc754, after the deploy; user: "all test passed" ).** `[JUNE_TRANSCEND] UNIT 98 / 48 / 104 transcendence 1` ( select and room ). `IMAGE` rows: type 9 `list_TR_S_DEVANAM / Y_RAJA / ASURA` ( `GUI_character_01` ), type 2 `Texture101_NEW` `TR_S_DEVANAM / Y_RAJA / ASURA_LARGE`, type 7 `Texture97_NEW` `TR_*_STATE`, type 5 `Texture100_NEW` `TR_ASURA_ICON`. Equip lines for all 15 slotted ids, 27600 / 27601 / 27602 `slot 4 bank A state 57`, 27064 / 27065 / 27066 `slot 4 bank B state 67`. States entered: Thousands Blossoms `state 58 skill 27600` x2, Energy Cannon `58 / 27601` x2, Nine-Tailed Fox `58 / 27602` x1 ( 58 = the awakened slot state ); B-5 `68` 27064 x3, 27065 x2, 27066 x1; 27043 chain `275`, 27227 `50` x2, 27045 `52` -> `279`. Awakening on each class: `ARA UI hyper cut-in ... HQ_Cutin_ASD / ASR / AYR_1_Transcendence` played and `ENGINE awakening: BFI 845` ( the transcended wake-up ). 0 EXCEPTION, 0 Motion Error, 0 KEM_ERROR, no `BLANK`. Not CH3d's: 4 UNHANDLED `E_DISABLE_HB_CHECK_REQ`, unbound `AddGroupChainQuest` x1 ( both seen in earlier runs ). Not in this run's lines ( the user's visual pass covers them ): the gray type-3 and party pictures, and a cast of A1-A3 on 104 and of 27226 / 27044 / 27046; those ids were played in CH3a-CH3c2.

**Save** ( backup `db_backup/els_db_20261112_090317_pre_pre_ara_ch3d.sql` ): units 98 `SakraDevanamT` ( 118 ), 48 `Ara99` ( 119 ), 104 `Asura99` ( 124 ), all lv 99, transcendence 1. Slots ( other slots cleared, learned skills kept ): A1-A3 = 27226 / 27043 / 27044, 27227 / 27045 / 27046, 27228 / 27047 / 27048; A-5 ( slot 4 ) = 27600 / 27601 / 27602; B-5 ( slot 9 ) = 27064 / 27065 / 27066. Not exercised: the level-70 guide popup ( CH6 walks the transition on un-transcended units ).

### CH3d - Review (2026-11-12, Opus 5.5; the CH3 stage review: covers CH3a, CH3a2, CH3b, CH3c, CH3c2, CH3d)

**Verdict: pass, no code fix.** The one code change is retiring `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL_DEBUG` ( `KTDXLIB/Always.h:12265`, `//#define`; the guarded log lines stay in the source ). `_SKILL` and `_ARA_TRANSCEND` stay on.

**Spot-checks against the June exe** ( `charport_adis.py ara --phase <p>` listings; Ghidra read-only for the CH3d pictures ), all equal to the port:
- CH3a: Swallow Assault Ready StateStart `32b9b3c0` ( facing range 99999, 1800 / 900, safe pos, `ProcessLockOn`, `+0x9710 = 0` ); Attack EventProcess `32bc1f30` ( FP cancel unless the chosen skill is Reflection 27213; `CanDown( pos, 30, 0, 0, &line, 0, 0, 1, 0 )` @`32bc204c`, line test type `LT_NORMAL` + enabled, else `MoveAraSafePosition`; vslot 0x7fc; lock-on -> 0xf4 / visible + 0xf5; probe window ); Void Break FrameMove `32bd2ec0` ( rate = `GetPowerRate x SA_DAMAGE_REL( 27403 )`, x `SA_DAMAGE_REL( 27416 )` when its buff is on; physic x rate; factor 2 of 27416 else factor 0 of 27403; damage effect only with a factor, the effect set always ). "= March": Ready EventProcess `32b9b440`, Turbulent Wave loop FrameMove `32bb90a0` ( on a line, 0.051 / 0.251 / 0.451 s ).
- CH3a2: Breaking attack StateStart `32b92480` ( Valor Devotion set 1, then `GetEquippedActiveSkillTemplet( 27233, false )` = June's exact id, factor 0 ); Low Brandish slide FrameMove `32bba8f0` ( templet `( 27206, true )` looked up and dropped: JUNE_BUGS 614 as written ); Peerlessness start FrameMove `32bb03d0` ( 0.55 s hide, colour 0, pre-scale; 1.532 s show ). "= March": Peerlessness end EventProcess `32b9c3b0`, Low Brandish kick FrameMove `32bbaab0`.
- CH3b: High Speed StateStartFuture `32b933e0` ( `SA` 0x4c of 27212 swapped, value `<= 0` -> 1, `passiveSpeed.x *=` ); Energy Void loop StateStart `32bc2c50` ( charge `>= 5` @`32bc2c84` / @`32bc2f73`, effects at 0 / 4 / 7 ); Restriction Energy start FrameMove `32bc56d0` ( 0.271 s; range `SA 0x1f`, PvP-or-PvE `0x90`, up 35 `0x9e`, down -35 `0x9f`; cone args re-traced from the stack: `( vec, 1, range, down, up, 1, 180, 0 )` = the port ). "= March": High Speed Ready EventProcess `32b93320` ( `stateTime >= 0.1` first ), Energy Void start EventProcess `32b9e7f0`.
- CH3c: Specter of Blade FrameMove `32bc4b90` ( 0.4 s, up to 3 FP, `32ba4760( n, 0x1b6d = 7021, 1 )`: JUNE_BUGS 616 ); Fire Fox FrameMove `32bc4d50` ( 9 + n foxes, n <= 4, rate of 27027, cool += 0.035, sign `+1` on odd counts ( `and 0x80000001` / `sbb` decoded ), `rand( count ) % 20` -> y, `rand( count + 1 ) % 20` -> z, life 2 s PvP-or-PvE else 3 s, scratch at Bip01 +-330 ); Smash FrameMove `32bbf500` ( three independent timers, `27420` learned and remaining hyper time `> 0` pick the `_TRENSCENDENCE` names, power x `+0x9754` ).
- CH3c2: Raid dash EventProcess `32ba0fe0` ( hit flag `+0x97bc` or animation end -> 0x127 ); `ApplyRaidRangeRate` `32ba6560` ( `SA` 0x52 of 27218 swapped scales `m_vScaleBase` = June `+0xb24`, then `SetScale( 1, 1, 1 )` ); Raid attack FrameMove `32bc3580` ( 0.1 s one FP -> rate of 27218, hits 0.1 / 0.4 / 0.52 s, 150 off the hip, `ApplyRaidRangeRate` after each ). Second switch @`32bd4e2e` / @`32bd4ec5`: both chain lookups on the swapped id, finish without.
- CH3d: Nine-Tailed Fox EventProcess `32ba3b80` is instruction-for-instruction Specter Walk's `32ba09a0` ( normalised listings diff empty; `SKILL_CANCEL_AFTER` keeps the foot test and the awakening branch ); helper `32baace0` = `CreateState` ( table `@335028fc`, Init + EventProcess ). Thousands Blossoms inline `@32bd2685` + tail `@32bd0edb` ( Init copied, EventProcess copied, `+0x11 = 1` on both ) = March's case; Energy Cannon / Nine-Tailed Fox jump targets `@32bd26d8` / `@32bd26fb`. Enum values 27600-27605 re-read from `Enum.src.lua`.
- **Templet swap:** every active-skill id read in the CH3 hunks goes through the swap ( `GetMpScaledPowerRate( .., true )`, `TryGet*SkillAbilityValue( .., true, .. )`, the `eUpgradeSwappedSkillID` chain lookups ) except June's own exact reads: Breaking's `( 27233, false )` and Energy Void end's base templet `GetSkillTemplet( 27019 )` ( June `32206450( 0x698b, 1 )`, the level gate ).

**CH3d shared files.**
- `X2SkillTree.h`: 6 `RNSI_HA_*` 27600-27605 in `JUNE_ENUM_NAMES`, values = June's. Pass.
- `X2Data.cpp`: prefixes `S_DEVANAM` / `Y_RAJA` / `ASURA`; the Texture101 branch ends in `else` into the Chung / Raven / Eve chain ( well-formed under every flag combination the guard allows ). Re-read from June's static initializer: `TR_S_DEVANAM_LARGE_GRAY` @`3337c9fd` -> `Texture101_GRAY` @`3345f928`, `_MEDIUM` @`33381c4d` and `_PARTY` @`3339683d` -> `Texture100_NEW` @`33461f00`, `_STATE` @`33372567` -> `Texture97_NEW` @`3345d8f0`. Pass.
- `Handlers_Quest.cpp`: the brace-less guard after Rena's chains into the Aisha / Eve test, as Rena's. Pass.
- `Always.h`: `_ARA_TRANSCEND` block, `#error` without `_ARA_LIVE` or `_RENA_TRANSCEND`. Pass.
- `LevelUpEvent.lua` TYPE6 ( kom_contents data036, BOM + CRLF, lines 433-451 ) = `LevelUpEvent.src.lua:566-584` ( diff: only the trailing comma of the last entry ). Pass.

**Hygiene.**
- `flagcheck JUNE_CHAR_ARA_SKILL --with JUNE_CHAR_ARA_SKILL_DEBUG --range 9744b93a^..2666c763` ( 6 commits, 15 revision pairs ): `files: 10 PASS, 0 PASS~, 1 DEFINE, 11 OTHER, 0 FAIL, 0 ERROR`; `1 deleted line(s)` ( the `//#define` turned on in CH3a ), 0 non-ASCII, 0 EOL mismatch, 0 bare `#else` / `#endif`, 0 encoding change; `RESULT: PASS`. OTHER rows: `X2SkillTree.h` x6 = `JUNE_ENUM_NAMES`; CH3a's `X2GUAra.cpp` / `.h` = the `_CORE` / `_LIVE` rename lines ( 7 + 2 deleted lines, `m_hKillEnemyEffectSet` -> `m_hSwallowAssaultFirstEffectSet`, listed in "CH3a - Done" ); CH3d's `Always.h` / `X2Data.cpp` / `Handlers_Quest.cpp` = `_ARA_TRANSCEND`.
- `flagcheck JUNE_CHAR_ARA_TRANSCEND --range 2666c763^..2666c763 --files` the three: `2 PASS, 1 DEFINE, 0 OTHER, 0 FAIL`, `0 deleted line(s)`, `RESULT: PASS`.
- `junediverge.py --check-all`: `allow-listed: 206`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUAra.cpp`: `0 A/B row(s) remain ( C 0, D 1, X 0 ignored )`. `skillicons.py check 27000-27999`: `skills 139, misses 0, known June gaps 1, sheets 0`, `RESULT: PASS`.
- `/Zs` probe of `X2GUAra.cpp` ( `census.cl_cmd` recipe ): 0 errors ( 38 warnings, none in `X2GUAra.cpp` ).
- This review's change: `flagcheck JUNE_CHAR_ARA_SKILL --with JUNE_CHAR_ARA_SKILL_DEBUG --worktree --files KTDXLIB/Always.h`: `1 DEFINE, 0 FAIL`, `1 deleted line(s)` ( the `#define` commented ), `RESULT: PASS`.

**Play-tests.** CH3a-CH3c2 as their Done notes ( all slotted ids cast, 0 `BLANK` / `EXCEPTION` / `Motion Error` ); CH3d's 09:16-09:19 run ( 15 equip lines, the three hyper actives in state 58, B-5 in 68, `[JUNE_TRANSCEND] IMAGE` type 2 / 5 / 7 / 9 rows, transcended cut-ins, 0 EXCEPTION / Motion Error / KEM_ERROR ). Not reached anywhere in CH3, handed to CH6: Smash ( Get Blood's chain ), Low Brandish's kick, the base ids of the Enhanced pairs, the gray type-3 and party pictures, the level-70 guide popup.

**Build:** not awaited ( only change: `//#define ..._SKILL_DEBUG`, standing rule 2026-11-03 ).

**Notes, not fixes.** The `m_f*RangeRate` members of the force-power skills ( Specter of Blade, Fire Fox, Raid ) hold a damage scale; the name follows CH2's members of the same kind ( reviewed in CH2d3 ). CH3a's Done note says March's `UseForcePower` adds to the used-force-power count; under `_FP` that member is gone ( CH3b / JUNE_BUGS 618-619 ), so neither form reads it: no effect.

**Kickoffs.** `CH4a.md` amended ( Smash's multiplier `m_fSmashPowerRate` = June `+0x9754` is written by the passive handler: name the write and test Smash ). `CH6.md` amended ( fresh un-transcended 118 / 119 / 124 for the transition, since 98 / 48 / 104 are transcended; the CH3 items not reached; the CH2dx `IsStateRegistered` chain guard, whose condition "until CH3's chain states are registered" is now met: CH6's scans decide, Z retires it ).

## Phase CH4 - Passives and class mechanics

**Run with:** Opus 5.5 high, one Opus review at CH4b (covers CH4a-CH4b), user play-test (two engine pieces, planned as two from the start; CH0
adds CH4c if the memo or a passive engine piece is over ~45 functions).

| Sub-phase | Scope |
|---|---|
| CH4a | `InitPassiveSkillState` `32bcd9c0` (vslot 0x690) and Ara's passive ids, their objects and readers; the passive / attack hook 0x780 `32bca010` whole (l.330; June's `RNSI_*` ids 27415 / 27417 ...); the delegate registrations (l.920: `32bcd9c0`, `32bb0d10`) replacing March's own pushes (l.930); death prevention with 27409 (l.258, the `32bc5a50` test); `GetReduceDebuffDurationRate` `32bd32f0`'s 27415 tail (l.267); the passive-id check of the Rules; flag `_PASSIVE` |
| CH4b | Overrides and memo: `GetActualMPConsume` 0x6a0 `32b96730` (uncarved), 0x3ac `32ba45d0` (March's 4-arg `ModifyFinalDamageByGameUnitType` body moves to June's override, l.991) and 0x6a8 `32b958e0`, 0x428 `32b956a0` (l.940), vslot 485 socket pass `32badc80` with its list `32d3f040` (l.284), the effect-set hooks 0x4c8 `32bc1530` (l.424); `MemoConditionAra` (RTTI `3373d604`, vtable `33502680`, `32ba46f0`) and the memo readers (`APPLY_MEMO`, `EFFECT_SET_LIST_EX_MEMO`); `P_AYR_GOOD_ENERGY_BALL` on CH2c0's path; §7 l.281 power-rate socket range if an Ara function calls `3201ad10`; CH4 stage review |

**Done gate:** each passive's effect shows (CH0 names the stat or log line per passive); 0 Ara `no_state` rows left except passives with no state in
June.

### CH4a - Done (2026-11-12)

Opus 5.5. New flag `SERV_IRUHADEV_JUNE_CHAR_ARA_PASSIVE` ( needs `_ARA_SKILL`, `_ARA_FP`, `JUNE_DAMAGE_EXCEPTION`, `JUNE_SPAWN_INHERIT`, `JUNE_SOCKET_PARITY`, `JUNE_BUFF_NEW_FLAG`, `JUNE_CHAR_CHUNG_PASSIVE`, `JUNE_SKILL_NUMERICAL`, `JUNE_BUFF_NEW_STAT`, `JUNE_BUFF_GROUP`, `FIX_NOT_ACTIVE_SKILL_AT_ENCHANT_DAMAGE`, all ON by `flagprobe.py` ) + `_PASSIVE_DEBUG` ( on until the CH4b stage review ). Files: `X2GUAra.*`, `X2GUUser.*`, `X2GameUnit.*`, `X2SkillTree.h` / `X2DamageManager.h` ( `JUNE_ENUM_NAMES` ), `Always.h`. `funcmap.tsv` rows `Char Ara CH4a` ( 13 ); checklist rows marked `(Ara: CH4a ...)` ( §4 l.178, §7 l.261 / 270 / 333, 7ae l.728, 7am l.928 / 938 ); `JUNE_BUGS` 620-622; `june_march_divergent.tsv` 5 rows ( below ). No review of its own: the CH4b stage review covers CH4a ( `CH4b.md` amended ).

**Ported ( read from the raw listings, `charport_adis.py ara` ).**
- **`InitPassiveSkillState` `32bcd9c0` whole** inside March's ( March in `#else` ). March read its 74xx ids, which a June save never has, so **every Ara passive below was off** before. Per class ( June's switch, the fall-throughs as June: 118 -> 28, 119 -> 31, 124 -> 34 ): Sakra Devanam 27404 `RNSI_P_ASD_BRUTAL_PIERCING` ( `m_fBlockingRate` / `m_fTurningRate` = SA 5: **June folds March's Quicken Guard 7403 into it**, + the debuff factor clone, rate x2 in dungeon / field / PvE from **level + 1**, JUNE_BUGS 620 ), 27402 Full Moon Slash ( `m_fCurrentSpeed`, `m_bMoonSlash`, Moonlight data `POWER_RATE_MUL` = the CH3d review's `m_fSmashPowerRate` question for Moonlight, CH2b's reader ); Little Hsien 27400 ( `m_iGainMpByFpConversion` ), **`SetCanAvoidance( 27207 learned )`** ( the CH3a2 amendment: June reads the base id with the upgrade swap, so 27233 counts ), 27401 ( `m_fReduceEDTDurationRate` ); Yama Raja 27407 Howling; Little Devil 27405 / 27406 / 27408; Asura 27414 ( new `m_fSpiritualEyeDefenceRate` +0x9748, `m_fPandemoniumCoolTimeRate` ), 27412 ( `m_fPandemoniumPowerRate` ), **27413 ( `m_fSmashPowerRate`, the CH3d review amendment )**; Little Specter 27411 ( new `m_fEnergyReinforceForceDownPercent` +0x9740, `m_fMpGainPerForcePowerInFullHyperMode`: CH1q's member ), 27410 ( `m_fBuffHyperModeDamageRate` ). Quick-stand setter gone ( the CH1b review amendment ). Level reads `GetSkillLevel( id, 1|0, 0, 1 )` as June's ( Asura / Specter blocks pass 0 ).
- **Delegates ( 7am l.928 / 938 ):** `SetAttackResultDelegate` = `32bb0d10` on 27209 / 27020; `AddHitUnitInfoByDamageData` = `32baf810` ( **Steal Soul's pull-down list**, the CH3b amendment; June tests the used skill 27020 / 27209 only ); new `ProcessDemigodHitCount` `32b95910` ( `m_usDemigodHitCount` +0x9874, SA_HIT_COUNT_1 -> buff ), `MarkRestrictionEnergyHit` `32b95980` ( **the CH3b end buff's flag** ), `ProcessGainGoodEnergyBallOnHit` `32b959a0` ( DTT 27 ), `ProcessAsuraOpenMindOnHit` `32ba7e40` ( a debuffed or enchant-burning target; `3226fce0` inlined ), pushed when 27418 / 27419 / 27045 / 27227 are learned.
- **The 0x780 hook `32bca010`** = `CX2GUAra::DamageExceptionClassHook` ( the base's call site in `SetDamageExceptionProcess`, `JUNE_DAMAGE_EXCEPTION` ): 27415 on `AT_COMMAND` ( own `m_DamageData.fStopTimeDef` += SA_DAMAGE_TIME, JUNE_BUGS 621; SA 5 roll -> ignore state defence + both defence bytes ), 27417 ( FP >= SA 2 and DTT 26, roll ), 27420 ( DTT 28: ignore state defence, critical rate + SA 0x33 capped at 1 ); applied skill 27027 ( debuff time x equipped SA 5 ), 27210 ( `HP_RECOVERY` / `_PVP` numerical row -> `fHitAddHPPer` = DD +0x404, key decrypted from `SetDamageDataFromLUA` ), 27217 ( debuff time + ); then **the force-power gain table**: a skill with `SA_FP_CANCEL_SKILL` ( 0xbc ) on the own unit adds a record ( `m_usFPGainCount` = templet +0xe ) or an entry, `m_iFPCancelChainCount` confirmed as +0x9838 ( the CH1q amendment ).
- **CEffect hook 0x328 `32ba6440`** = `OnDamageEffectCreated` ( `JUNE_SPAWN_INHERIT`'s call in the CEffect ctor ): 27417 spirit ball max life += SA 0x1f; 27420 / Mana Drain 27216 `m_DamageTime` + 1 and + equipped SA 2.
- **Death prevention ( l.261 ):** new `CX2GUUser::ProcessPassiveReturnFromDeath` = `320072d0`'s Ara piece, run before the SIEE 19 gate in `UpNowHp` / `SetNowHp`; `ProcessResurrection` = `32bc5a50` ( FP >= 5, debuffs reserved to finish by new `CX2GameUnit::ReserveToFinishDebuffTempletsFromGameUnit` = `32282190`, **HP set to** max HP x ( SA 0x55 + ( FP - 5 ) x SA 0x3b ), gauge spent; no effect set / text / debuff, unlike March ). March's DamageReact copy in `ProcessSkillAtDie` is off under the flag ( June has none: `dispsweep 6b11` = 320072d0 / 32bc5a50 / 32bc84b0 only ).
- **`GetReduceDebuffDurationRate` `32bd32f0` tail ( l.270 ):** 27415 learned, full gauge, debuff templet -> x SA 0x7c.
- **Not ported, routed:** `320072d0`'s Crest of Time piece returns false ( char-add's Crest of Time engine, l.261 ); its Soft Body 23402 piece ( Rena, closed ) stays March's `ProcessSkillAtDie` ( a Rena leftover, noted on l.261 ); `+0x8e74` ( read only by June's unported CommonEventProcess rewrite ) and the PvP / PvE vslot 0x280 call ( not reached offline ).
- **Divergent ( `june_march_divergent.tsv` ):** `CX2GUAra::InitPassiveSkillState`, `::SetAttackResultDelegate`, `::AddHitUnitInfoByDamageData`, `::ProcessResurrection`, `::GetReduceDebuffDurationRate`, all replaced in this commit.
- **Names:** `X2SkillTree.h` 27400 / 27404 / 27405 / 27406 / 27411 / 27414 ( June `Enum.src.lua` ); `X2DamageManager.h` `DTT_AYR_ENERGY_BULLET` 26 / `DTT_AYR_GAIN_GOOD_ENERGY_BALL` 27 ( the live data036 `Enum.lua`: June's `.src.lua` lost them ) / `DTT_ASR_LIGHTNING` 28.

**Gates.**
- One-file probes ( `/c /Zs /EHsc`, no PCH ): `X2GUAra.cpp`, `X2GUUser.cpp`, `X2GameUnit.cpp` exit 0, 0 errors.
- `flagcheck JUNE_CHAR_ARA_PASSIVE --with ..._PASSIVE_DEBUG --worktree --files` ( 9 ): `files: 4 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` ( OTHER = `JUNE_ENUM_NAMES` in the two enum headers ).
- `junediverge.py --check` ( 8 files ): `allow-listed: 12`, `RESULT: PASS`. `enumscan.py --check` the three `.cpp`: `0 A/B row(s)`. `skillicons.py check 27400-27420`: `skills 21, misses 0, known June gaps 1`, `RESULT: PASS`. `luabind.py check Ara_Martial_Artist`: `0 unbound`, ENUM `0 missing`, STATE `37 compared, 0 wrong`, `RESULT: PASS`.
- `DEPLOY OK: X2_offline.exe is 26,331,648 B, dated 2026-11-12 10:22:58, md5 81d47bd2cb545cd5992a1d9f003259b5, equal to X2/US_SERVICE/x2.exe (2026-11-12 10:22:51); built at main 8686a645 + 9 uncommitted code file(s).`
- `EXESTR OK: 3 of 3 found in X2_offline.exe, 26331648 B, dated 2026-11-12 10:22:58, md5 81d47bd2cb545cd5992a1d9f003259b5` ( wide: the init and resurrection-gate log lines, `HP_RECOVERY_PVP` ).
- **Save:** backup `db_backup/els_db_20261112_101358_pre_sql.sql`; unit 98 + 27404 ( 4 ), slot 3 = 27034; unit 48 + 27409 ( 4 ), 27418, 27210 ( slot 5 ), slot 3 = 27058; unit 104 + 27414 ( 4 ), 27420, 27216 ( slot 5 ), 27027 ( slot 6 ), slot 3 = 27023, slot 7 = 27217, slot 8 = 27061.

**How to see it in game:** one `[JUNE_CHAR] ARA PASSIVE init` line per load with the values above, then `sakra open mind ignore defence`, `yama open mind ignore defence`, `yama open mind spirit ball range`, `weird fox thunder`, `fire fox debuff time`, `conversion health hp recovery`, `quick piercing debuff time`, `fp gain record`, `demigod energy ball buff`, `restriction energy hit`, `gain good energy ball buff`, `asura open mind buff`, `mana drain hit count`, `open mind debuff time`, `resurrection` + `resurrection gate fired` as each fires.

**Logs ( 2026-11-12, run 10:25-10:28, after deploy md5 81d47bd2; 0 EXCEPTION / Motion Error / KEM_ERROR; known `E_DISABLE_HB_CHECK_REQ` x4, `AddGroupChainQuest` ).** `init` x2 per unit, values as designed: 98 block / turn 0.300, brutal piercing 1, moon speed 1.700, mul 0.333 ( 0 on the first init, before the slot state builds the Moonlight data, as June orders it ), gain mp 10, avoidance 1, edt 0.550; 48 howling 1.800 / 0.500, bullet 1.400 / 1.500, range 1.600 / 0.700, hell 1.400; 104 spiritual eye 0.800 / 0.875, asura 1.400, smash 1.400, energy 30.000 / 8.000. Seen: `sakra open mind ignore defence` ( stop time def 0.080 ), `open mind debuff time` x280 ( BTI 860 `BTI_BUFF_ASD_OPEN_MIND_MAX_ENERGY`, which June's data marks `IS_DEBUF = TRUE` and CH1j re-applies every frame at full gauge: June's behaviour ), `fp gain record` x12 ( 27226 / 27044 / 27045 / 27046 / 27058 / 27061 / 27048 / 27047 ), `gain good energy ball buff`, `restriction energy hit`, `yama open mind spirit ball range` x5 ( life 0.5 -> 0.75 ), `yama open mind ignore defence` x2 ( force power 10 ), `demigod energy ball buff` x25, `asura open mind buff` x9, `fire fox debuff time x1.330` x4, `mana drain hit count` 3 / 10000 ( two effects per cast ). Not seen: `weird fox thunder` and `quick piercing debuff time` ( the save slotted the base rows 27023 / 27217 at level 0, which the slot packet drops; re-slotted to 27042 / 27241, backup `els_db_20261112_103148_pre_sql.sql` ), `resurrection` ( no death with 5+ force power ), `conversion health hp recovery` ( routed to CH4b, kickoff amended ). User report: Yama Raja's command-attack energy ball colour ( routed to CH4b ).

**Re-test logs ( 2026-11-12, run 10:33-10:36, md5 81d47bd2; 0 EXCEPTION / Motion Error / KEM_ERROR ).** Unit 104 only: slots sent `A 27228/27047/27048/27042/27602 B 27216/27062/27241/27061/27066`; `weird fox thunder ( critical rate 0.150 )` x3, `asura open mind buff` x13, `fire fox debuff time` x2, `mana drain hit count` x4, `fp gain record` x4. Still unseen, carried to CH4b's play-test ( kickoff amended ): `quick piercing debuff time` ( June tests applied id 27217; the slotted upgrade is 27241 ) and Resurrection on unit 48. User owes nothing more for CH4a.

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

## Open decisions (for the user; asked at the end of planning; answered 2026-11-10)

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
   **Answered (user, 2026-11-10): (a), follow Ara's rule.** CHNa writes the rows; the user packs `StatTable.lua`.
2. **Retire the lobby orb handles** (checklist 7e, l.368). March hand-made Little Devil's three lobby orbs (`m_hSeqEnergyBullet1-3`,
   `X2UnitViewerUI.*`); you kept them on 2026-10-07. Since C9g-2 adopted June's `Motion_ARA_Common.x` and `AnimationNameTable.lua`, June's table row
   `ALD_LobbyWait` plays the same three particles and the hard-code is dead (keyed `ALD_lobbyWait`). **(a, recommended)** retire the three handles under
   the unit viewer's flag with March's code in `#else`, as the other eight classes' hard-codes were (CH5); **(b)** keep them as dead code.
   **Answered (user, 2026-11-10): (a), retire them** (CH5).

Settled by standing rules, not decisions: the motion koms deploy with CH1 final if June drops a name March plays (7ad); `CUSTOM_RENDER_STATE` on
Fox Phantom is a labelled stand-in until june-engine R5 (7w, Raven's shape); the lock-on is ported by Ara (CH2b0) and hoisted later by Lu/Ciel;
June-side icon gaps are `JUNE_BUGS.md` rows; the 53 emotion motions are staged, their behaviour is Stage 10.
