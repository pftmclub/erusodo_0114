# June character port: Lu/Ciel (Stage 8, the first new character)

Plan key `char-luciel` · branch `main` · kickoffs `kickoffs/char-luciel/` · log tag `[JUNE_CHAR]` (lines start `[JUNE_CHAR] LUCIEL <sub>`, sub-tags
`DATA`, `NEWUNIT`, `CREATE`, `DUO`, `CORE`, `SKILL`, `PASSIVE`, `UI`, `NUM`, `JOB`, `TUTORIAL`) · flags `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_*` for Lu/Ciel's
own code, `SERV_IRUHADEV_JUNE_CHAR_*` for the class-agnostic new-character and two-body pieces Rose and Ain reuse (+ a temporary `..._DEBUG` per
sub-phase) · backlog Stage 8 (`JUNE_PORT_BACKLOG_ORDER.md`, "Stage 8: new characters (Lu/Ciel, Rose, Ain)") · written 2026-11-02 by the
`new-plan char-luciel` session from the Chung plan (`JUNE_CHARACTER_CHUNG_PHASES.md`), the Aisha "Template for Stage 7" and its "Phase CH-N".

## Context

**What this plan does.** It brings June 2017's Lu/Ciel into the March client from nothing: the unit type (`UT_LUCIEL` 10), seven classes
(`UC_LUCIEL_DUO` 200; first jobs Chiliarch 37, Royal Guard 38, Diablura 39; second jobs Dread Lord 127, Noblesse 128, Demonio 129), creation,
starter items, the tutorial, job change, June's class `CX2GULuciel` (a new `X2Lib/X2GULuciel.cpp` / `.h`), his two-body ("duo") system, his
skills, passives, UI and transcendence rows. Third jobs (`UC_LUCIEL_CATASTROPHE` 177, `_INNOCENT` 178, `_DIANGELION` 179) are out (scope, user
2026-10-20).

**Why it differs from a Stage 7 port.** March has no Lu/Ciel class, script, motion or templet row: every class function is `new` (no
`inside` column), and the engine has never held a tenth unit type or a class id above 127. So before the class can run, the plan builds the
new-character foundation (phases `NU*`) and the two-body engine (phases `DU*`). Both are class-agnostic where June's code is (flags
`SERV_IRUHADEV_JUNE_CHAR_*`), so Rose (`UT_ROSE` 11, base `UC_ROSE_GUNNER` 201) and Ain (`UT_AIN` 12, base 202) reuse them.

**Order (user, 2026-11-02).** CH0 and NU0 (the two maps, read-only) and CH0b (assets) run now. **Every code phase starts after char-chung Z**,
ahead of `char-elsword`: Chung's CH2-CH4 edit `X2GUUser.*` and `X2Game.cpp`, which NU1-DU5 also edit. **Changed by the user 2026-11-03
(`new-plan char-elsword`): Lu/Ciel is lower priority; every phase after CH0b waits until the March characters are ported (Stage 7
closed).**

**The state he starts from** (planning evidence below; CH0 / NU0 re-verify anything they build on).
- **Enums are registered** (`SERV_IRUHADEV_JUNE_ENUM_NAMES`): `X2Unit.h:36` `UT_LUCIEL = 10` (**equal to March's `UT_END`**; June's `UT_END` is
  13), `X2Unit.h:167-191` the seven classes. `UC_END` is 124, and the socket / item loaders drop every class at or above it.
- **The class id does not fit the wire.** Every shared `char m_cUnitClass` (`KUnitInfo` `CommonPacket.h:1457`, 23 `CommonPacket.h` and 10
  `ClientPacket.h` members) is signed: 128, 129 and 200 wrap negative. June reads a word (`movzx word ptr` in `3235eb20` / `32360090` /
  `323614c0` / `32361e00`, june-transcend T0). The save column is an INTEGER and is fine.
- **Dormant March pieces:** `CI_CIEL = 11` (`X2Data.h:44`), `GetHyperModeTableName( bCiel_ )` (`X2GUUser.cpp:33444-33539`), "stand-in: Lu/Ciel
  not ported" branches in `X2StateClassChange.cpp`, `X2UICharInfo.cpp`, `X2UISkillTreeNew2015.cpp`, `X2UnitViewerUI.cpp:7534`
  (`plan_march_side.md` §1). `X2OfflineSkillAutoLearn.h` already covers 200-202. No `Always.h` flag names Lu/Ciel.
- **Creation:** the class buttons come from `DLG_Unit_Create_Unit_Button_New.lua` (March bytecode, no Lu slot; June's source has
  `LucielCreateUnitSlot`, `Pos_Luciel = 8`), plus nine `Select<Char>()` functions in `X2StateCreateUnit.cpp:1266-2346` and the entry switch
  `:195-215`. The emulator's `Handlers_Unit.cpp:222-308` does not check the class. `GBASE_ITEM[]` (`X2OfflineInventory.cpp:1226`) covers
  classes 1-9.
- **Stats:** `StatTable.lua` has no row for 37-39 / 127-129 / 200. `X2OfflineStatTable::UnitTypeOf` (`:271`) returns the templet's unit type
  (10) and the loader then retries **class** 10 (Sword Knight): a silent wrong curve once Lu/Ciel's templet row exists. NU4 fixes it.
- **June's class:** `CX2GULuciel`, range `32ae4080`-`32b4e76f` (Rose before, Elesis after), ~583 functions (Ghidra defines 499; **85 of the
  119 override targets are not carved**, among them 0x7b4 `32b46580`), vtable `334f06fc` with **547 slots** (0x880 / 0x884 / 0x888 are
  Lu/Ciel's own virtuals), object 0x98a0 bytes, ctor `32b1bd90` (base ctor `3203e850`), dtor `32b39380`.
- **His script is two files:** `Luciel_Duo.lua` (22,847 lines, 406 tables: 6 `INIT_*`, 400 `LSI_*`) includes `Luciel_Demonio.lua` (7,524 lines,
  158 tables). Neither is in March's koms. Both scripts are written in the `LSI_` state namespace, not `CSI_`.
- **Two bodies.** Lu is the major unit and Ciel the minor one. The keys are `DUO_TYPE` (390 + 158 uses; June's `Enum.src.lua:17515` has
  `DT_MAJOR 0 / DT_MINOR 1 / DT_BOTH 2 / DT_KEEP 3`, and the live `Enum.lua` lacks `DT_BOTH` / `DT_KEEP`), `MINOR_UNIT_ANIM_NAME`,
  `MINOR_UNIT_SPEED_TIME0-4`, `NOT_SHOW_UNIT_BY_DUOTYPE`, `HIT_ADD_COMBINATION_POINT` (damage data, 6) and `CP_AMOUNT`. The second-body
  members `+0x8e88` / `+0x8e90` / `+0x8e91` are **`CX2GUUser`'s**, written in the base ctor (`32040e4e`, `32042433`, `32042471` from the unit's form
  byte `+0xe40`, `32042479`), not in the class. The Stage 8 bullets (backlog) list the engine readers.

**Out of scope (owned elsewhere).** Rose and Ain (their own `char-rose` / `char-ain` plans, brief `kickoffs/june-backlog/stage8-plan.md`);
third jobs; generic UI (Stage 9); emotion behaviour (Stage 10; CH0b stages the 98 `*_Emotion_*` motions); `PET_*` Lu/Ciel pets (Stage 17); NPC
copies (`TIME_LU*`, `RAID_SHADOW_CIEL/RUCIEL`: already June bytecode); guild-agit form packets `0x4fd` / `0x4fe` unless NU0 finds them
reachable offline (then DU5).

## Rules

The Chung plan's Rules hold (`python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules"`) with Lu/Ciel's names and these changes:
- **Everything in the class is `new`.** `X2GULuciel.cpp` / `.h` are new files (ASCII, CRLF; the Edit tool is fine on them), added to
  `X2Lib_2010.vcxproj` (and its `.filters`) by CH1a. The class compiles under `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` from CH1a and stays
  unreferenced until CH1 final. `_LIVE` then adds the factory cases (`CX2Game::CreateGUUser` `X2Game.cpp:21276`, the intruder copy
  `:2141-2201`), the script and motions. Names follow March's sibling classes (`X2GUChung.cpp`): `LSI_<STATE>_StateStart` / `_FrameMove` /
  `_EventProcess`, Hungarian members, `_CONST_LUCIEL_<phase>_` namespaces. No `June` and no `Luciel_` copies of `CX2GUUser` members.
- **Class-agnostic pieces stay class-agnostic.** Where June's engine code tests a unit type or reads a two-body member for every class
  (the equip view, the user sync, the damage-state getters, creation), the port is in `CX2GUUser` / the engine under a `SERV_IRUHADEV_JUNE_CHAR_*`
  flag and is not copied into the class. Rose's and Ain's ports widen the `#if`, they do not repeat it.
- **Shared structs (2-byte class).** `CommonPacket.h` is reached before `Always.h`: NU2's flag is also `#define`d at the end of `ServerDefine.h`,
  added to `SECOND_HOMES` in `tools/flagcheck.py`, and added to `X2ServerProtocol_2010.vcxproj`'s `X2TOOL|Win32` defines (layout change). Prove it
  with `cl /P` on one file per project. Keep the emulator's handlers and `X2OfflineDB` consistent in the same commit.
- **Saves.** NU4 adds no schema; DU4 is the one migration (user, 2026-11-02: June's NESI numbering, forward-only, snapshot `db_backup/` first).
  Play-test saves never slot a skill in slot 4 / 9 before transcendence.
- **Shared Lua per character.** `UnitTemplet.lua` (core list) gets only Lu/Ciel's 7 rows (NU3). `DLG_Unit_Create_Unit_Button_New.lua` is a
  multi-character file: NU4 converts it to core-list source and adds only Lu/Ciel's slot. `Enum.lua` (core list) gains `DT_BOTH` / `DT_KEEP` (CH1p).
  `DungeonEnum.lua` / `DungeonData.lua` (core list) gain only the tutorial rows (TU). Single-owner files are adopted whole (`adopt-june`, bytecode),
  each with `luabind.py check`.
- **Starter items (user, 2026-11-02): 15 rows, a labelled stand-in** under `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_STARTER_ITEMS`. These are the gear
  113300-113304 (Cobo ... for Beginners, `unit_class` 200; Ciel's copy through `item_id_for_minor_unit` 200113300-200113304), **both** promotion
  sets (Lu's 41890-41894, set 5933; Ciel's 41900-41904, set 5934, `is_for_minor_unit` 1), and `plan_starter_items.tsv` lists the ids. The live
  shape is ten rows, so the comment says so.
- **Stat rows: open until CH0 (user, 2026-11-02).** CH0 reports whether June's `CharacterStatTable` (or any June data) is a full per-level
  table. The user decides then. Until then the loader uses the synthetic curve and logs it (NU4), and never another class's rows.
- **Ghidra.** Read-only, except that CH0 may *create* the uncarved functions (create-function only, no rename or comment) so that later phases
  can decompile them. It says which it did. Where a function stays uncarved, the capstone dump in `june2017_re/char_luciel/ghidra/` is the source.
- **One code phase at a time in `X2GULuciel.*`**, and one at a time in `X2GUUser.*` across this plan and any other.

## Starting context

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "<heading>"` |
| Planning evidence | `june2017_re/char_luciel/plan_*` and `ghidra/` (below); CH0 / NU0 write `ch0_*` / `nu0_*` beside them |
| Checklist rows | `python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "<heading>"`; Lu/Ciel's 78 lines: `plan_march_side.md` §6a |
| Stage 8 engine bullets | `python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 8: new characters (Lu/Ciel, Rose, Ain)"` |
| Chung's tools (take a character or copy with the range changed) | `june2017_re/charport_states.py`, `charport_keys.py`, `charport_closure.py`, `charport_adis.py` (`CLASS_RANGE`: CH0 adds `luciel`), `charport_regsim.py`, `s3_no_state.py`; `char_chung/ch0_*`; `char_arme/ch0c_listing_check.py` |
| June Lua (reading) | `june_src_fixed/data036_kom/Luciel_Duo.src.lua`, `Luciel_Demonio.src.lua`, `UnitTemplet.src.lua` (Lu/Ciel rows `:1723-1979`), `Enum.src.lua`, `NewSkillTempletVer2.src.lua` (tree rows from `:282612`), `DungeonEnum.src.lua:56,101`, `DungeonData.src.lua:29878`, `LevelUpEvent.src.lua:158,381-383,627-629` |
| Ghidra | `ghidra` skill; range `0x32ae4080-0x32b4e76f`, vtable `334f06fc` (547 slots), ctor `32b1bd90`, dtor `32b39380`, InitState `32b43d10` (0x7a4), InitStateCommon `32b3e8b0`, skill setter `32b46580` (0x7b4, uncarved), factory `3230f470` (case 10 in `322ced00` / `322d5bf0`) |
| Bring June Lua in | `/x2-adopt-june` (`luasrc.py adopt-june data036 Luciel_Duo.lua Luciel_Demonio.lua`, `--dry-run` first) |
| Stage June assets | `python tools/kom.py stage-june <list>`, `/elstools pack dataNNN`, `/x2-kom-packed copy dataNNN` (ask first) |
| Hygiene / build / logs | `flagcheck.py <FLAG> --worktree --files ...`, `junediverge.py --check ...`, `enumscan.py --check ...`, `build.py --deploy` (background), `logs.py --grep "JUNE_CHAR LUCIEL"` |
| Save / test character | `/x2-db`, `/x2-create-char <name> <class> <level>` (NU4 teaches `createchar.py` the seven classes) |
| Dev console | auth level 3, END key; `/additem <id> [qty]`, `/qdone <id>`, `/tc 1` |

## Planning evidence (2026-11-02)

Digests are kept in `june2017_re/char_luciel/`. They were written by Sonnet subagents and spot-checked: the ctor writes vtable `334f06fc` at
`32b1bde2`, `X2Unit.h:36` / `:167-191` hold the enums, and `UnitTypeOf` is at `X2OfflineStatTable.cpp:271-287`. The files are
`plan_june_side.md`, `plan_march_side.md`, `plan_ghidra.md` (+ `ghidra/` dumps: `vtable_334f06fc_all.tsv`, `range_function_candidates.tsv`,
`range_string_refs.tsv`, the decompiles), `plan_charport_keys.txt` and `plan_starter_items.tsv`.

| Measure | Lu/Ciel | (Chung) |
|---|---|---|
| June class range | `32ae4080`-`32b4e76f`; ~583 functions (499 carved); 122 vtable slots into the range (119 overrides + 3 own) | 441 carved, 72 overrides |
| Skill tree | 301 rows over 7 classes (200: 19; 37 / 38 / 39: 35 each; 127 / 128 / 129: 59 each); 195 templets (52 `SI_` 10000-10601, 143 `RNSI_` 30001-30606); **not in `s3_reform_ids_no_state.tsv`** | 261 |
| Skill setter | `32b46580`: jump tables 0x7532-0x760c (65 ids) and 0x778a-0x778e (5) plus compares, ~75-85 cases | 118 |
| Registrations | InitStateCommon `32b3e8b0`: 294 through `31fd89b0`; InitState's class switch sets `+0x8ac8` (37 / 38 / 39 / 200 -> 0, 127 -> 1, 128 -> 2, 129 -> 3) | |
| Script tables | Duo: LU 83, CIEL 79, SA 63, A 36, LCG 29, LLC 28, RIDING 27, LLD 17, LCN 17, LDN 7, LDB 5, rest; Demonio: SA 94, BOUNDING 30, LDB 19, A 14, HA 1 | 385 |
| Key census | Duo 230 keys: june-only 23 / neither 3 / march-only-site 4 / both 200; Demonio 127: 12 / 2 / 3 / 110 | 249: 11 / 8 / 7 / 223 |
| Gap scan | `Luciel_Duo {'no NUMERICAL_DATA_INDEX': 67, 'no DAMAGE table': 15}`, `Luciel_Demonio {11, 2}`, `Luciel_Demonio {'CUSTOM_RENDER_STATE': 3}` | 74 / 15 |
| Motions | 125 June files (data208 Lu 62, data218 Ciel 63; 98 `*_Emotion_*`); March has 4 of the names (`Motion_CSM_Lu_LDB.x`, `Motion_Lu_LDB_Evil.x`, `Motion_CSM_Ciel_LDN.X`, `Motion_Ciel_LDN_Evil.X`) | 70 / 17 |
| UnitTemplet | 7 June rows, 0 in March; 18 keys no other class has (`m_MinorUnit*`, `m_fMinorUnitLobbyRotationY`, `majorUnitName`, `minorUnitName`, `m_HyperCutIn_Trans`, `m_MinorUnitHyperCutIn(_Trans)`) | |
| Tutorial | `DI_DUNGEON_TUTORIAL_LUCIEL` 41240 (`Dungeon_Tutorial_Luciel.lua`, already June bytecode), `DI_BATTLE_FIELD_TUTORIAL_LUCIEL` 39609, `WI_TUTORIAL_LUCIEL_STAGE0` 633; live `DungeonEnum` / `DungeonData` lack them; `Tutorial_Luciel_Stage0*` (data040) missing | |
| Missing June Lua | `Luciel_Duo.lua`, `Luciel_Demonio.lua`, most Lu/Ciel UI dialogs in data034 (only `DLG_Transcendental_Change_Guide_Luciel` present), `DLG_Class_Change_Luciel_*` (data217), `DLG_LUCIEL_BOUNDING_KEY.lua` (CH0 checks) | |
| Summons | NPC scripts `CSM_*`, `SA_LDN_*`, `SI_SA_*` present as June bytecode; `NUI_SI_A_LCN_SHADOW` (1907) has no script | |
| Transcendence | guide present; `HQ_Cutin_{LLD,LCN,LDN}_{LU,CIEL}_Transcendence` (+ `_Trans` for LDN) in data195 | |
| Starter items | gear 113300-113304 exact; promotion sets Lu 41890-41894 / Ciel 41900-41904 (both, user) | |

## Phases

| Phase | What | Model | Depends |
|---|---|---|---|
| CH0 | Class map: functions (carve), states, skills, keys, checklist rows, assets + effect closure, summons, unmapped helpers, stat-row data question | Opus high | - |
| NU0 | Engine map: every `UT_END` / `UC_END` / unit-class-width site, the 60 `case UT_ADD` switches against June's, creation, templet keys, two-body readers, equip view, NESI, char info | Opus high | - (beside CH0) |
| CH0b | Every June Lu/Ciel asset, whole, first | Sonnet high, user play-test | CH0 |
| NU1 | Unit-type range: June's `UT_END` 13, arrays / loops / script table, `UC_END` guards | Opus high, Opus review | NU0, char-chung Z |
| NU2 | June's 2-byte unit class on the wire and in the emulator | Opus xhigh, Opus review | NU1 |
| NU3 | `UnitTemplet`: Lu/Ciel's 7 rows and the 18 June-only keys' parser | Opus high, Opus review | NU1 |
| NU4 | Creation: button dialog, `SelectLuciel`, emulator create-unit, starter items, stat fallback fix, tools; select screen | Opus high, Opus review, user play-test | NU2, NU3 |
| DU1 | Second body in `CX2GUUser`: members, minor-unit model, user sync, not-selected-body physics, one-shot reset | Opus high, Opus review | NU3 |
| DU2 | Damage-state getter virtuals and their two engine readers | Opus high, Opus review | NU1 |
| CH1a | Class skeleton: new files, ctor / dtor / members, own virtuals, `MemoConditionLuciel` | Opus high, Opus review | CH0, NU1 |
| CH1b-CH1l | State bodies by group and override bands (11 sub-phases, below) | Sonnet high, Opus review each | CH1a; one at a time |
| CH1m-CH1p | Wiring: registration; skill head + base callbacks; virtuals + keys + hooks; the in-class duo switch | Opus high, Opus review each | CH1b-CH1l reviews, DU1, DU2 |
| CH1 | Final: define `_LIVE`, factory cases, adopt both scripts, play-test | Opus high, Opus review, user play-test | CH1p review, NU4, CH0b |
| DU3 | June's equip-view rewrite (every class), item-templet minor-unit fields | Opus high, Opus review, user play-test | CH1 review |
| DU4 | NESI renumber and the save migration | Opus xhigh, Opus review, user play-test | DU3 review |
| DU5 | Two bodies in the character window and the unit viewer; form packets | Opus high, Opus review, user play-test | DU4 review |
| CHN | Job change for 37 / 38 / 39 / 127 / 128 / 129 (Aisha's "Phase CH-N") | Opus high, Opus review, user play-test | CH1 review |
| TU | Tutorial dungeon, field and world; first-entry route | Sonnet high, Opus review, user play-test | CH1 review |
| CH2a0 | Lu/Ciel's summoned NPCs (CH0 places it before the first skill phase that spawns one) | Opus high, Opus review, user play-test | CH1 review |
| CH2a-CH2d | Skills: base 200 and first jobs 37 / 38 / 39 | Sonnet high, Opus review, user play-test | CH1 review (CH2d: CHN) |
| CH3a-CH3d | Skills: second jobs 127 / 128 / 129, hyper actives, transcendence rows | Sonnet high (CH3d Opus), Opus review, user play-test | CH2 reviews, CHN |
| CH`n`0 | One Opus sub-phase per unmapped helper CH0 finds | Opus high, Opus review | as its phase |
| CH4a-CH4c | Passives; damage exception + overrides; memo and item readers | Opus high, Opus review, user play-test | CH2 reviews |
| CH5 | UI and data: two-body cut-ins, HUD portraits, bounding-key dialog, combination-point UI | Sonnet high, Opus review, user play-test | CH1 review, DU5 |
| CH6 | Whole-character walk + gap reruns | Sonnet medium, user walk | CH3, CH4, CH5, TU reviews |
| Z | Close-out | Sonnet medium | CH6 |

CH0 and NU0 run now and side by side. Their outputs don't overlap, and both are read-only except CH0's optional Ghidra carving. CH0b follows
CH0 and may also run before char-chung Z, because it is assets only. All other phases wait for char-chung Z. A group over ~45 functions is split
by CH0 with a digit suffix (`CH1g2`), along with its kickoff and STATUS row. Play-test follow-ups after CH1 final get the next letters.

## Phase CH0 - Lu/Ciel class map (read-only)

**Run with:** Opus 5.5 high. No code, no flag, no build. Delegate reads, greps, tsv joins, scripted runs and known-address dumps. Outputs in
`june2017_re/char_luciel/`.

**Step 0.** Decide the carving: either create the ~85 uncarved functions in Ghidra (`range_function_candidates.tsv`, create-function only) or
use capstone dumps. Then build the registration map with `charport_states.py init 32b43d10 32b3e8b0 <base>` and `skill 32b46580`, and run
`ch0c_listing_check.py` on it. Every row must match the raw listing; check the `~` rows and the std::string-named rows by hand. Add `luciel`
to `charport_adis.py`'s `CLASS_RANGE`.

**Scope:**
1. **Class map** `ch0_class_map.tsv`: every function in `0x32ae4080-0x32b4e76f`, plus the three jump-thunk tails. Columns: kind (override /
   own virtual / registrar / state callback / helper / dead), June vslot, **proposed plain name**, the March `CX2GUUser` member it overrides (by
   name, from June's base vslot), target sub-phase from the table below, and funcmap rows `Char Luciel CH0`. Settle the boundary functions
   (`32b4e770` / `32b4e780` are shared `ret` stubs).
2. **State and skill table** `ch0_state_skill.tsv`: 406 + 158 tables, 195 templets / 301 tree rows. **Regenerate the S3 census for Lu/Ciel**
   (`s3_no_state.py` with the seven classes added; the tsv has no Lu/Ciel rows) so CH6 compares like with like. Confirm no state id is stored in a
   `UCHAR`, and say whether the `LSI_` state ids overlap `CSI_` ranges anywhere a shared table is indexed by state id.
3. **Keys** `ch0_keys.tsv` (`charport_keys.py Luciel_Duo`, `Luciel_Demonio`): each of the 23 + 12 June-only keys gets an owner phase (duo keys
   -> CH1p, `CUSTOM_RENDER_STATE` -> CH3c (7w), `CUSTOM_OFFSET_NOT_SELECT_UNIT` / `USE_NOT_SELECT_*` -> DU1), and the `neither` keys are checked.
4. **Checklist rows** `ch0_checklist_rows.tsv`: every §1-§7au row that names Lu/Ciel (`plan_march_side.md` §6a, 78 lines), plus the 11
   every-class sections -> phase or n/a with the reason. NU0 owns the rows about engine sites; agree the split with its tsv.
5. **Assets** `ch0_assets.tsv` + `ch0b_stage_list.tsv` (routes a name / b data / c `item.kim` Lu/Ciel items, unioned) and **effect closure**
   `ch0_effect_closure.tsv` (`charport_closure.py`, run-time-built names included). It includes the 125 motions, the minor-unit meshes
   (`Mesh_Lu_Base_SD.X` and kin), faces and hair for both bodies, the cut-in art, `POPUP_TR_*` from `DLG_Transcendental_Change_Guide_Luciel`, the
   icon sheets `skillicons.py check` lists, and the June UI Lua the class reads (`DLG_LUCIEL_BOUNDING_KEY.lua`). The 4 March same-name motions are
   replaced by June's (asset rule). Say who reads them in March (`CSM_*` NPCs).
6. **Unmapped helpers.** For every skill, the callees outside the class range that funcmap does not map (`ch0_unmapped_callees.py`). Each one
   gets an Opus `CH<n>0` sub-phase, kickoff and STATUS row.
7. **Summon sites.** Every `CreateNPC`-style call in the class and its NPC id (`NUI_SI_A_LCN_SHADOW` 1907 has no script: what does June
   spawn?), and `luabind.py check` on every summoned NPC's script. Place CH2a0 before the first skill phase that spawns one, and split it if there
   is more than one spawn rule.
8. **Stat-row data (open decision).** Is June's `CharacterStatTable.src.lua` (`UT_LUCIEL` `:167`, `DREAD_LORD` / `NOBLESSE` / `DEMONIO`
   `:417-433`) a full per-level table in `StatTable`'s units, or a display / base table? Is there any other June stat source for 37-39, 127-129 and
   200? Report what exists, and put the question to the user with a recommendation. Don't decide it.
9. **Shared data.** Which `LSI_*` / `SI_*` / `RNSI_*` names the C++ enums need (raw id + comment where June's Enum has no name); the 7
   `UnitTemplet` rows' strings (`majorUnitName` / `minorUnitName` ids in our string table?).
10. **Amend the kickoffs** CH0b-CH5, CHN and TU ("Amended by CH0" sections: filters with real column numbers, addresses, counts, any split).

**Done gate:** the tsvs exist; `ch0c_listing_check.py` passes; every function has a sub-phase and a name; 406 + 158 tables and 195 ids mapped;
the S3 baseline exists; assets union routes a / b / c; the summon map exists; the stat question is with the user; kickoffs amended.

### CH0 - Done (2026-11-02)

Read-only, no flag, no build (Opus 5.5 high; Sonnet subagents ran the builders, each claim below spot-checked). Outputs and their builders are in
`june2017_re/char_luciel/` (`ch0_*.py`, rerun order in each docstring); `charport_adis.py` has `luciel`; `s3_no_state.py` has the 7 Lu/Ciel families;
`char_eve/ch0_build_assets.py` has a `luciel` entry (run it with `--out`). 139 funcmap rows `Char Luciel CH0` (`ch0_funcmap_new_rows.tsv`).

**Step 0.** GhidraMCP has no create-function endpoint, so nothing was carved: uncarved functions (1,144 of 1,643 starts) are read with capstone
(`charport_adis.py luciel`). `charport_states.py init 32b43d10 32b3e8b0 3202b110` -> `ch0_reg.tsv`, 364 rows; `ch0c_listing_check.py` **364 / 364**
(re-run by me), no `~` rows; the 5 std::string rows are the START / WIN / LOSE rolls (9 / 11 / 12) and hyper 23 / 204 (8 names by class).

**Maps**
- `ch0_class_map.tsv` (1,643 starts): **867 to transcribe** (651 state-fn, 119 overrides, 93 helpers, 3 own virtuals, 1 registrar), 731 thunks, 27 dead,
  18 not Lu/Ciel. Names: every state-fn, 105 of 122 vtable rows, 33 of 93 helpers; 75 stay `?` with a hint (names in `ch0_hand_names.tsv`).
  The state-id getter band (0x114 / 0x118 / 0x438-0x4a4 / 0x5c4) returns June ids: **June inserts a state at 24**, so 25 = DAMAGE_GROGGY ... 42 =
  DAMAGE_REVENGE (NU0's DU2 note agrees); never count March's enum. `31fc9d90` / `31fc9d50` = CX2GUUser transformed-form flags (+0x8f6e Lu,
  +0x8f6f Ciel; 2 = either, 3 = both), checked in the bytes.
- `ch0_state_skill.tsv`: **564 tables** (553 phased, 5 dead, 6 INIT_*, 0 unassigned) + 46 registered names with no table; **198 ids** (195 templets +
  106 / 900 / 208), 301 tree rows (271 with a state, 30 passive). Prefixes: `LD` / `LUCIEL` 200, `LLC` 37, `LCG` 38, `LDB` 39, `LLD` 127, `LCN` 128, `LDN` 129.
  State ids 7-592 (16-bit already, `SERV_IRUHADEV_JUNE_USER_STATE_ID16`); no `LSI_` in either `Enum.lua`.
- `ch0_skill.tsv`: the uncarved setter `32b46580` via a listing emulator: **64 cases, 112 ids**; 12 states are set elsewhere (InitStateCommon, vslot 0x57c).
- `ch0_vtable_all.tsv` (547) / `ch0_vtable_slots.tsv` (122). `ch0_keys.tsv` (357; census counts match the plan): owners per key. `ch0_checklist_rows.tsv`
  171 rows (43 n/a, 36 NU / DU with "NU0 confirms"). `ch0_s3_baseline.txt`: 301 rows, all no_state.
- Assets: `ch0_assets.tsv` 17,227; **`ch0b_stage_list.tsv` 14,609** (dry run 37 koms, 0 refused, 4 new kom dirs); `ch0_effect_closure.tsv` 13,049 rows,
  572 missing. **127 motions, not 125**; the 4 same-name ones are already June's bytes. `skillicons` both ranges FAIL until CH0b copies 12 sheets.
- Summons (`ch0_summons.tsv`): 3 class spawn sites, 1906 (LLD, `@32b20486`), 1908 (LCN, `@32b2fe73`), 2269 (LDN, `@32b340ce`); 2270 is spawned by the
  Garden NPC's Lua; **1907 `NUI_SI_A_LCN_SHADOW` is spawned by nothing** (June ships no script; the shadow is DamageEffect `Mesh_LCN_SHADOW`).
  `luabind` PASS for 4 scripts, Garden FAIL (4 unbound). The dtor / ReInit look up 2269, so CH1a registers its name.
- Item 9: `ch0_enum_names.tsv`: 7 of 195 ids in the C++ enum by name, 68 in the core `Enum.lua`, 126 in neither; **none unnamed in June**.
  `ch0_unittemplet_luciel.tsv`: all 28 name / description STR ids exist in March's `general.ess`; the 18 June-only keys = NU0's.
- Item 6: 379 out-of-class callees, 131 unmapped (72 confirmed, 57 candidate, 2 unknown); 21 + the transform-link group need Opus.

**Phase changes** (kickoffs amended by `ch0_amend_kickoffs.py`; STATUS rewired):
- Merged: **CH1c -> CH1b** (41), **CH1e -> CH1d** (30), **CH1j -> CH1i** (45, riding is 3 functions). Split: CH1f / **CH1f2**, CH1g / **CH1g2**, CH1k /
  **CH1k2**, CH1l / **CH1l2** / **CH1l3**, CH2c / **CH2c2**, CH2d / **CH2d2**, CH3c / **CH3c2** (CH1h 47 stays whole).
- New Opus helper phases: **CH1a0** (gauge class, transform link, hold-key selector, switching guide; before CH1a, after DU1), **CH1k0** (buff
  Time-finalizer passes), **CH1l30** (unit scans, effect aim), **CH2d20** (transform HUD), **CH3c20** (`SLockOnSystem::ProcessLockOn` out of
  `CX2GUAra`, Garden binders). **CH2a0 -> CH3a0** (first spawn is 127's). `31fd6260` / `31fdd8e0` go to DU1.
- InitState pushes the ten passive delegates (7am): nine are CH4a's, `AddCombinationPointOnHit` `32b14bc0` CH1p's; CH1m keeps 4 functions.
- **Corrections to the plan:** `LSI_A_*` are actives, not combos (CH1f is `*_COMBO_*`); `Enum.lua` also needs the `CUSTOM_RENDER_STATE` table
  (CH1p); `DLG_LUCIEL_BOUNDING_KEY.lua` is adopted at CH1 (Init reads it), the two other Lu/Ciel dialogs at CH5; the `CommonStateStart` key
  parse is DU1's (NU0 agrees); of the checklist's type-10 branches, L274 (`BOUNDING_UP` StateStart `31fd02d0`) is DU1's, L379 / L454 CH4b's, L471 CH1i's
  (none of the four is in NU0's tsvs, so their kickoffs carry them).

Functions per phase: CH1a 21, CH1b 41, CH1d 30, CH1f 39, CH1f2 38, CH1g 38, CH1g2 38, CH1h 47, CH1i 45, CH1k 31, CH1k2 30, CH1l 32, CH1l2 32,
CH1l3 32, CH1m 4, CH1n 1, CH1p 1, CH2a 32, CH2b 27, CH2c 35, CH2c2 35, CH2d 31, CH2d2 31, CH3a 26, CH3b 45, CH3c 41, CH3c2 40, CH3d 7, CH4a 15, CH4c 2.

**Stat rows (item 8, with the user).** June has **no per-level table** for Lu/Ciel: `CharacterStatTable` is gear-stat scaling (one relative row per
unit type, `UT_LUCIEL` :166-173 `430, 98, 410, 93, 10000`; second-class rows 127 / 128 / 129 :417-433; 37 / 38 / 39 / 200 use the type row), and no
June server `StatTable` exists here. But **every existing `StatTable.lua` row (50 classes, levels 1-80, 4,000 rows) is that relative row x (0.8 +
0.0225 (L-1)), HP x (1 + 0.15 (L-1)), within 1** (`ch0_stat_fit.tsv`; spot-checked Elsword L1 / L40, Lord Knight L1 / L40); second jobs start at 0.8 x
their own row and then follow the type curve; 81-99 is `cap99_gen.py`'s extension. Trap for NU4: once the templet exists, `UnitTypeOf` maps 200 to
type 10 and the fallback silently reads Knight's class-10 rows.

## Phase NU0 - New-unit and two-body engine map (read-only)

**Run with:** Opus 5.5 high, beside CH0. No code. Outputs `june2017_re/char_luciel/nu0_*.tsv`, funcmap rows `Char Luciel NU0`.

**Scope:**
1. **Unit-type range** `nu0_unit_type_sites.tsv`: every `UT_END` / `UT_END - 1` / `UT_ADD`-terminated site (the `X2UnitManager.h:3442,3444`
   arrays, `apszScript[]` + its `ASSERT` at `X2UnitManager.cpp:405-427`, `X2Item.h:208-209`, `X2Item_Preprocessing.inl:1035`, the ~25 loops in
   `X2BeginningGame` / `X2StateCreateUnit` / `X2StateServerSelect`) and every `UC_END` guard (`X2SocketItem.cpp:2897`, `X2ItemManager*.cpp`). Each
   site gets June's function, June's bound or table size (June's `UT_END` 13; June's 12-name script table at `333c1c28`), and a verdict for NU1:
   widen / June's body / leave. Use `cl /P` to say which branch is live.
2. **Per-type switches** `nu0_type_switches.tsv`: the 60 `case CX2Unit::UT_ADD:` sites (`plan_march_side.md` §4). For each one: the June
   function, what June does for `UT_LUCIEL` (and for `UT_ROSE` / `UT_AIN` in one column, for the later plans), and the owner phase. Lu/Ciel cases are
   owned here; Rose / Ain cases are recorded only.
3. **Unit-class width** `nu0_unit_class_width.tsv`: every `char` unit-class member in `CommonPacket.h` / `ClientPacket.h` (33), every
   client reader / emulator writer cast (`X2Unit.cpp:4342,4586,4753,4845`, `Handlers_Unit.cpp:251,263,411,499` and the rest), and June's type and
   offset for each (`3235eb20` / `32360090` / `323614c0` / `32361e00`). Note any `Put` / `Get` serializer and `sizeof`-dependent code.
4. **Creation and selection** `nu0_create.tsv`. This covers **Lu/Ciel's whole entry on the character-creation screen** (user, 2026-11-02):
   June's create-unit state (June's `SelectLuciel` counterpart and the entry switch), the button slot in `DLG_Unit_Create_Unit_Button_New.lua`
   June vs March (`LucielCreateUnitSlot`, `Pos_Luciel = 8`, the Lu slot only), the portrait / name / description textures and strings it shows,
   the preview model on the creation stage (both bodies? which idle / select motions, the camera and lobby rotation from `m_fLobbyRotationY` /
   `m_fMinorUnitLobbyRotationY`), the job-preview buttons `DLG_Unit_Create_ClassChange_Button_Luciel` (who reads it, what each button plays),
   the select sound / voice, and any intro movie or effect June plays on picking him. Also the server-select / character-list model for a
   two-body unit, and what June does on a new character's first entry (tutorial route?). Every asset these name goes to CH0b's list.
5. **Templet keys:** June's `UnitTemplet` parser for the 18 Lu/Ciel-only keys (function addresses, target members), and March's parser.
6. **Two-body engine** `nu0_duo_engine.tsv`: every reader / writer of the `+0x8e88` / `+0x8e90` / `+0x8e91` members and the unit's form byte
   `+0xe40` outside the class (base ctor `3203e850`; the whole-image writers `31fe633a`, `3204e07a`, `32050e1a`, `32092a04`). Plus the Stage 8
   bullets: the user-sync bits (c3e rows B, E, F, H, I, G), PhysicProcess's not-selected-body branch (`32015b30 @32019f9b-3201a1c7`), the one-shot
   reset (`31fd4290` / `31fd4340`), the damage-state getter vslots (0x114 / 0x118 / 0x494 / 0x490, 0x458) and their readers, and the minor-unit
   model and skin creation. Assign each to DU1 / DU2.
7. **Equip view, NESI, char info:** the Stage 8 bullets' functions (`3201b930`, `31fd2f00`, `31ffe990`, `31fd48a0`, `31fd63c0`, `328825c0`,
   `32882540`; NESI 20 -> 35, 21 -> 43; `charinfo_functions.tsv` rows `3234b8c0`, `323acfb0`, `3291d390`, `31f93a10`, `31faa150`, `31f94190`,
   `3291d360`, `3234f4d0`, `321693b0`, `32e22ce0`; form packets `0x4d9` / `0x4da`, plus `0x4fd` / `0x4fe` if guild agit is reachable offline)
   -> DU3 / DU4 / DU5, with the save-migration shape for DU4 (which tables and columns hold an NESI id).
8. **Job change and tutorial:** the class-change route for the six job classes (quests `CJT_*`, cash items, `X2OfflineClassChangeSeed.h`
   `PARENT_ROWS`, `X2OfflineLuaEnumSeed.h`) -> CHN; the tutorial's DungeonEnum / DungeonData rows, `Tutorial_Luciel_Stage0*`, and how June
   enters it -> TU.
9. **Amend the kickoffs** NU1-NU4, DU1-DU5, CHN and TU.

**Done gate:** the tsvs exist; every site has an owner phase and a verdict; the DU4 migration shape is written; kickoffs amended.

### Done (2026-11-02, Opus 5.5 high; read-only, no flag, no build)

Eight tsvs in `june2017_re/char_luciel/` (Sonnet subagents; each spot-checked against the June bytes or the March line it cites). The
kickoffs NU1-NU4, DU1-DU5, CHN, TU and CH0b carry the per-phase detail ("Amended by NU0"); 25 funcmap rows `Char Luciel NU0`.
- **Item 1** `nu0_unit_type_sites.tsv` (55 rows; `cl /P` liveness): widen 15, june_body 2, leave 38 (15 dead). June's script table holds 13
  (`337341b8`, `cmp esi, 0x337341ec`), getter `3237a680` `cmp eax, 0xc`, classes accepted 1..202 (`3271a858 cmp eax, 0xc9` after `dec`).
  **Trap:** March's `ASSERT` at `X2UnitManager.cpp:425` compiles out, so the loop must not widen before the table. `SERV_LIMIT_UNIT` is live (the
  planning digest said dead). `X2OfflineStatTable.cpp:283` stays (class -> type for base classes only).
- **Item 2** `nu0_type_switches.tsv` (60 sites): CH1 7, CH5 10, DU1 3, DU5 6, NU1 1, NU4 9, TU 1, none 23 (dead, already June's, or June
  data-driven). Rose / Ain cases recorded in their column. June reads the form byte (`+0x8e90` / unit `+0xe40`) in the revive voice, weapon-effect
  bones, emotion sound, Square SD mesh and reform.
- **Item 3** `nu0_unit_class_width.tsv` (202 rows): June's class is a **word** at the same offsets as March's char (`3235eb7b movzx ecx, word ptr
  [ebp + 0x15]` and three siblings; transcendence int at +2), packs 1, PUT / GET member by member. **The layout change does not reach
  X2ServerProtocol's code:** the serializers compile only in `X2ServerPacket.cpp`, offline transport is bytes with both ends in X2Lib
  (`Session.cpp:343-349`), and X2ServerProtocol sees `CommonPacket.h` as an unused header. NU2's size: 50 members, 34 serializers, 68 readers,
  39 writers, 5 `char` signatures; no `sizeof` / `memcpy`.
- **Item 4** `nu0_create.tsv` (47 rows): SelectLuciel `325559f0`, event proc `32558860` (messages 36 / 50), no June entry switch. June shows
  **two bodies** on the create and select screens (`@32555fe8` viewer `+0xd7c`); NU4 shows Lu, DU5 adds Ciel. Create request trap: March's live
  branch sends the unit type as the class (`X2StateCreateUnit.cpp:2505`). Name / description are pictures in `GUI_character_info_10.TET`. Assets
  listed in CH0b's kickoff (new: background, info_10, bt_03, 6 voices, 4 Common / Lobby motions, 29 basic / cash meshes, `SelectVoiceTable.lua`;
  June replaces `bt_01` / `bt_02`, `Texture67`); the 4 job movies are already in March.
- **Item 5** `nu0_templet_keys.tsv`: June parser `3237aa40` (XOR keys, `xorstrings.tsv`), 18 keys all scalar; 15 new members plus
  `m_fLobbyRotationY` (+0x2d4, every June row, March has none); `minorUnitName`'s length (+0x50) is June's "has a second body" test.
- **Item 6** `nu0_duo_engine.tsv` (2,964 rows: DU1 440, DU1-accessor 2,248, DU2 48, DU3 6, DU5 123, out 90, Ain / Rose 9). The second body
  mirrors the first at +0x6b4 (`+0x8e7c..+0x8e8c`), `+0x8e90` active body, `+0x8e91` dual enabled. **June's skin-anim accessor is duo-aware and
  inlined into every class** (`32070cf0`; Eve's range alone reads it 420 times): DU1 changes `X2GUUser.h:2092-2094`, not 2,248 sites (rows
  retagged from the subagent's `CH1`). The second body is built only in `InitMotion` `31fe5860`. DU2 has seven reader functions, not two
  (`31fd6ce0`, `31fd7350`, `320348c0`, `32063650`, `31fe7610`, `3202a9c0`, the 0x530-0x538 predicates).
- **Items 7-8** `nu0_equip_nesi.tsv` (33: DU3 7, DU4 10, DU5 12, none 4), `nu0_jobchange_tutorial.tsv` (19: CHN 11, TU 8). **DU4 shape:**
  `item.slot` 20 -> 35 for category 9 and `inventory_size.size` 21 -> 43 for category 9, schema v22 -> v23; no other table holds an NESI id.
  Guild agit is not reachable offline: `0x4fd` / `0x4fe` dropped. Job change: quests already in `FieldQuest.lua` (200 -> 37 35644-35647, -> 38
  35648-35651, -> 39 35660-35690; 127 35750-35790, 128 35700-35740, 129 35800-35840), tickets 276955-276958.
- **Divergence rows** (verdict `stage8`, `junediverge.py --check-all` RESULT: PASS): `Handler_EGS_CREATE_TUTORIAL_ROOM_REQ` (`325622e0`: June
  sends every class to `DI_DUNGEON_TUTORIAL_*`, March to `DI_BATTLE_FIELD_TUTORIAL_*`; TU) and `RotateYSelectCharacterType` (`31edf260`, the
  templet rotation keys; NU4).
- **Open, in the owning kickoffs:** `USE_PHYSICPROCESS_NOT_SELECT_UNIT` / `..._ANIM_TIME_IN_SPEED` members (DU1); readers of the Common /
  Field / Lobby minor motions (NU3 / DU5); where the form persists across a relog (DU4, so there is one migration); June's widths of the 46
  members outside the four disassembled structs (NU2 widens all); the 50 unit `+0xe40` hits marked "other struct?" (DU5); the 39 / 129 ticket
  family (CHN). `dispsweep.py` misreads `cmp [reg+disp32], imm` and drops 66 / F2 / F3 prefixes (noted in DU1).

## Phase CH0b - Every June Lu/Ciel asset, whole, first

**Run with:** Sonnet 5.5 high, user play-test (ErrorLog diff on a select-screen pass of all nine March characters). `kom.py stage-june
ch0b_stage_list.tsv`, then pack, then ask before each copy. Nothing reads the files until CH1, except the 4 March same-name motions (CH0
names the readers). **Gate:** `python tools/skillicons.py check <Lu/Ciel's skill-id ranges 10000-10699, 30001-30999>` = PASS, with the shared
`DLG_SKILL_Passive_*` sheets included.

### CH0b - Done (2026-11-03)

Sonnet 5.5. No C++, no flag, no build (assets and data only; CH0 found no March reader a June asset breaks).
- **Reference:** `ErrorLog.txt` (287,399 B, 2026-11-03 19:26) saved as `data\ErrorLog_pre_CH0b_luciel.txt` before any copy.
- **Staged (stage list):** `kom.py stage-june --jobs 8 ch0b_stage_list.tsv`: `STAGE-JUNE OK: 37 kom(s), 14609 staged (14607 new, 2 replaced, 0 identical), 0 refused, 89s`
  (dry run identical). Replaced: `DLG_SKILL_Buff_01.tga` / `.tet` (data063), backup `kom_backup_stage_june\20261103_201733`.
- **Packed / copied (user approved, 2026-11-03):** 37 `PACK OK`, 37 `COPY OK` (all add-only: 0 removed; 33 existing archives + 4 new: `data162` 331, `data163` 294,
  `datai028` 250, `datai029` 245; `X2Main.cpp` mounts `datai001-032`). The 4 same-name motions were already June's bytes (nothing replaced).
- **NU0 amendment (not in the stage list, which holds CH0b-owned files only):**
  - `kom.py stage-june` 12 files: `DLG_Unit_Background_Luciel.tga` (data051), `DLG_UI_Common_Texture_GUI_character_info_10.tga/.TET` and `DLG_UI_Common_Texture67.tga` (data057; Texture67 replaced),
    `DLG_UI_Button_NEW_GUI_character_bt_01/_02 .TGA/.TET` (March's data056, 4 replaced, backup `20261103_202419`), `bt_03 .TGA/.TET` (data215),
    `HQ_Loading_TUTORIAL_LUCIEL_01/02.tga` (data126, found there). `STAGE-JUNE OK: 5 kom(s), 12 staged (7 new, 5 replaced)`.
  - `luasrc.py adopt-june` (June bytecode) 14 Lua: data034 `DLG_Unit_Create_ClassChange_Button_Luciel`, `Class_Change_Guide_Luciel` / `_1`, `DLG_1ST_` / `DLG_2ND_Class_Change_Guide_Luciel`;
    data036 `SelectVoiceTable`; data040 `Tutorial_Luciel_Stage0` / `_LineMap`; data217 `DLG_Class_Change_Luciel_CG / CN / DB / DN / LC / LD`.
  - `luabind.py check` over the 14: PASS after one row in `tools/data/lua_unbound_known.tsv` (`AddVoice`, SelectVoiceTable, NU4 ports the loader; March does not open the file yet, `X2Data.cpp:1920`).
  - 9 `PACK OK`, 9 `COPY OK` (data034 036 040 051 056 057 126 215 217). `data051` also deployed 48 older entries (`DLG_Background_*`, dated 2026-09-22, not CH0b's) already in `kom_contents`; the user approved.
- **Skill icons:** `skillicons.py check 10000-10601` PASS (52 skills), `check 30001-30606` PASS (143), `check 10000-10699 30001-30999` PASS (195, 0 misses, 0 sheets).
- **Not staged here (routed):** the 43 `Tutorial_Luciel_Stage0_*` meshes / textures (data164) and the other TU / CHN / CH5 Lua stay with their owners (TU, CHN, CH5, CH1); `ch0_effect_closure` names in no June kom (`Ciel_Claw04.ogg`, `Luciel_Shadow_Fury04.ogg`, `WhiteImage.tga`, `__Motion_HAURES2_Ciel_LDN.X`) have nothing to stage.
- **Play-test (user) is the gate:** log tag `[JUNE_CHAR]` has no lines; check `ErrorLog.txt` against `ErrorLog_pre_CH0b_luciel.txt` (`KEM_ERROR`, `Motion Error`, missing texture / mesh). Lu/Ciel is not selectable until NU4.
- **Drift:** none new.

## Phase NU - New-character foundation (NU1-NU4)

| Sub-phase | Scope | Flag |
|---|---|---|
| NU1 | `UT_END` becomes June's 13 (the enum reorders so `UT_LUCIEL` / `UT_ROSE` / `UT_AIN` keep June's values and `UT_END` follows them; no member renumbered). Every NU0 item-1 site gets its verdict: arrays sized by the new end, loops bounded as June bounds them (a loop that would offer Rose / Ain before their templets exist skips a type with no templet, logged), and `apszScript[]` with June's names (Rose's and Ain's scripts are absent, so their slots load nothing and log). The `UC_END` guards accept June's class ids as June does. | `SERV_IRUHADEV_JUNE_CHAR_UNIT_TYPE_RANGE` |
| NU2 | The unit class as June's word in every NU0 item-3 member. Put / Get, the emulator writers and the client readers move together. CommonPacket.h exception (ServerDefine.h, `SECOND_HOMES`, vcxproj define). Proven by a probe of each project, and by an existing character's full login / village / dungeon round-trip with no new log line. | `SERV_IRUHADEV_JUNE_CHAR_UNIT_CLASS_WORD` |
| NU3 | Lu/Ciel's 7 `UnitTemplet.lua` rows (core-list source, `luasrc.py refresh`), and March's parser reads the 18 June-only keys into new templet members (June's names and types, NU0 item 5). A `[JUNE_CHAR] LUCIEL DATA` line at load gives the row count and both body names. | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_DATA` (rows), `SERV_IRUHADEV_JUNE_CHAR_MINOR_UNIT_TEMPLET` (parser) |
| NU4 | Creation, starting with **Lu/Ciel's entry on the character-creation screen** (NU0 item 4, user 2026-11-02): `DLG_Unit_Create_Unit_Button_New.lua` converted to core-list source with Lu/Ciel's slot from June; his portrait, name and description; the preview model with June's motions, camera and lobby rotation (one body until DU5 if June shows two); the select voice / effect; `DLG_Unit_Create_ClassChange_Button_Luciel` adopted with its job previews; `CX2StateCreateUnit::SelectLuciel` and the entry switch (June's bodies); the emulator create-unit path for class 200 (the 15 starter rows, Rules); `X2OfflineStatTable` never borrows another class's rows (`UnitTypeOf`'s result is a unit type, not a class: the fallback becomes the base class or the synthetic curve, logged once per class); `tools/createchar.py` `CLASSES` / `PARENT` / `PROMO` gain the seven classes; the select screen shows a created Lu/Ciel (one body until DU5) and **does not enter the game until CH1**: the hand-off says so. | `SERV_IRUHADEV_JUNE_CHAR_CREATE`, `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_STARTER_ITEMS` |

**Done gate:** every NU0 site has its verdict applied; an existing character's play is unchanged (ErrorLog diff); the creation screen shows
Lu/Ciel's slot, portrait, description and preview as June's, and his job-preview buttons play; a Lu/Ciel is created and
listed with its 15 items in the save (`/x2-db` inventory); NU4's `CREATE` line shows class 200, the starter-row count and the stat source.

## Phase DU - The two-body engine (DU1-DU5)

| Sub-phase | Scope | Flag |
|---|---|---|
| DU1 | The second body in `CX2GUUser`: the `+0x8e88` / `+0x8e90` / `+0x8e91` members and their base-ctor writes (from the form byte `+0xe40`), the minor-unit model / skin built from NU3's templet members, the user-sync bits (c3e rows B, E, F, H, I, G: positions `0x20000`-`0x80000`, active body `0x1000000`, float `0x10000`, class short `0x100000`, the class-skill float drivers `0x200000`), PhysicProcess's not-selected-body branch (`32015b30 @32019f9b-3201a1c7`, readers of `CUSTOM_OFFSET_NOT_SELECT_UNIT` / `USE_NOT_SELECT_MOTION_DIFFERENCE`), the one-shot reset (`31fd4290` / `31fd4340`). Every other class sees no change. | `SERV_IRUHADEV_JUNE_CHAR_DUO_BODY` |
| DU2 | `CX2GUUser` gains June's damage-state getter virtuals (vslots 0x114 / 0x118 / 0x494 / 0x490; base bodies = the `USI_DAMAGE_*` constants) and `GetJumpDownStateID` (0x458, virtual). `PhysicProcess_Jumping` (`SERV_IRUHADEV_JUNE_USER_JUMP_UP_LINE`) and StateChange's force-down flush (`320348c0 @32034931-32034994`) call them. Lu/Ciel's overrides (`32b4d810` / `32b4d850` / `32b4da10` / `32b4d9d0` / `32b4d5b0`) come with CH1. | `SERV_IRUHADEV_JUNE_USER_DAMAGE_STATE_GETTERS` |
| DU3 | June's equip-view rewrite whole (the Stage 8 bullet "June's equip-view rewrite"): `UpdateEquipViewListInfo` `3201b930` with the `slotId` argument on vslot 0x644 for every override (Chung `32bdc750`, Ara `32bc89e0`, Arme `32cc9950`, and Lu/Ciel's), `GetAddItemAndRemoveItemToViewList` `31fd2f00`, `AddEquipToViewList` `31ffe990`, the skin anim `31fd48a0`, `31fd63c0`, `CreateEquip`'s two arguments, the two cosmetic every-class cases (`328825c0`, `32882540`), and the item-templet minor-unit fields (minor-unit item id and flag, Extra keys 10-13, item-type / unit-class guards from K2; checklist 7g). | `SERV_IRUHADEV_JUNE_CHAR_EQUIP_VIEW` |
| DU4 | NESI renumber to June's ids (`NESI_ONEPIECE_FASHION` 20 -> 35, `NESI_END` 21 -> 43, `NESI_MINORUNIT_*`) and the forward-only `X2OfflineDB` migration (user, 2026-11-02; snapshot `db_backup/` first, schema step +1). It retires `SERV_IRUHADEV_JUNE_CHARINFO_EQUIP_GROUP` (CharInfo C3) and the `IS_FOR_MINOR_UNIT` stand-in (slot hide by June's ids, `32e22ce0`). Every character's equipment must show identically before and after (relog test on three characters). | `SERV_IRUHADEV_JUNE_CHAR_NESI_RENUMBER` |
| DU5 | Two bodies in the character window and the unit viewer (the Stage 8 bullet "The character window's Lu/Ciel and Rose branches", Lu/Ciel half): unit `+0xe40` (setter `3234b8c0`, NOT-apply `323acfb0`) behind `CIM_SWITCHING` / `_ONLY_THIS_DIALOG`, the viewer form byte `+0xd80` / `+0xd81` with `31f93a10` / `31faa150` / `31f94190` / `3291d360`, the MsgProc click switch `3291d390` (replaces the logged stub), OnDropAnyItem `3234f4d0` / `321693b0`, the form packets `0x4d9` / `0x4da` with emulator handlers, both bodies on the creation-screen preview and the select screen (where June shows two and NU4 showed one), and the CharInfo C3 form stand-ins (ResetStat's form, `@3291bf2d`). Rose's weapon UI stays for `char-rose`. | `SERV_IRUHADEV_JUNE_CHAR_DUO_VIEW` |

**Done gate:** DU1 / DU2: every other class's play is unchanged (the reviews diff `cl /P`), and a Lu/Ciel's second body follows and syncs once CH1
lands (CH1's play-test). DU3-DU5: Lu and Ciel each wear their own equipment in the village, in the character window and on the select screen;
switching the form in the window swaps the shown body; a relog keeps it.

## Phase CH1 - Deploy unit: the class core

**Transcription rules (CH1b-CH1l).** Each function from the class map is written whole, from Ghidra (or the capstone dump), under
`SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE`, with the name CH0 proposed. A class-range callee owned by a later group gets a declaration and
`// June <va>: body in <phase>`. An engine callee is called through its March counterpart (`funcmap.tsv`, `junediverge.py --list`); an
unmapped one means "this needs Opus": stop and say so. June's numbers, order and quirks are kept verbatim (a `JUNE_BUGS.md` row each). Skill-level
and skill-use reads go through the shared `CX2GUUser` helpers (Chung plan, Rules). The dev free-skill gate is
`CX2GUUser::IsDevUnlimitedSkillUse()`. Nothing runs until CH1 final.

| Sub-phase | Scope | Model |
|---|---|---|
| CH1a | Class skeleton: `X2GULuciel.h` / `.cpp` (new, vcxproj + filters), the 0x98a0 object's members (ctor `32b1bd90` after the base ctor `3203e850`, dtor `32b39380`), the three own virtuals (0x880 `32b14db0`, 0x884 `32b3acf0`, 0x888 `32ae6130`), `MemoConditionLuciel`, the `+0x8ac8` class index. Flags `_CORE`, `_LIVE` (`//#define`) | Opus high |
| CH1b | Lu's common states, part 1: wait, walk, jump, dash (`LSI_LU_*`) | Sonnet high |
| CH1c | Lu's common states, part 2: damage, stand-up, guard-like, throw, buff and the rest of `LSI_LU_*` | Sonnet high |
| CH1d | Ciel's common states, part 1 (`LSI_CIEL_*` movement) | Sonnet high |
| CH1e | Ciel's common states, part 2 | Sonnet high |
| CH1f | Combos `LSI_A_*` / `LSI_COMBO_*` (Duo) | Sonnet high |
| CH1g | Chiliarch `LSI_LLC_*` and Royal Guard `LSI_LCG_*` job states | Sonnet high |
| CH1h | Dread Lord `LSI_LLD_*` and Noblesse `LSI_LCN_*` job states | Sonnet high |
| CH1i | Demonio script: `LSI_LDB_*`, `LSI_LDN_*`, its `A` / `BOUNDING` / `HA` states (`Luciel_Demonio`) | Sonnet high |
| CH1j | Riding (`LSI_RIDING_*`, 27) | Sonnet high |
| CH1k | Overrides, band 1 (CH0 splits the 119 by vslot) | Sonnet high |
| CH1l | Overrides, band 2, including the five damage-state overrides DU2 calls | Sonnet high |
| CH1m | **Opus.** Registration: InitState `32b43d10`, InitStateCommon `32b3e8b0` (294 registrations), the start / win / lose rolls; regsim against the listing | Opus high |
| CH1n | **Opus.** Skill head `32b46580` (0x7b4, ~80 cases) and the base callbacks it hands to | Opus high |
| CH1o | **Opus.** Virtual wiring, the June-only keys CH0 gives CH1, the checklist hooks | Opus high |
| CH1p | **Opus.** The in-class duo switch: `DUO_TYPE` (`DT_BOTH` / `DT_KEEP` added to the live `Enum.lua`, core list), `MINOR_UNIT_ANIM_NAME`, `MINOR_UNIT_SPEED_TIME0-4`, `NOT_SHOW_UNIT_BY_DUOTYPE`, the body switch and its key (`DLG_LUCIEL_BOUNDING_KEY.lua`), combination points (`HIT_ADD_COMBINATION_POINT`, `CP_AMOUNT`). Flag `_DUO`, sub-tag `DUO` | Opus high |
| CH1 | **Opus.** Final: `_LIVE` (+ `_DUO`) defined; `CreateGUUser` / intruder case `UT_LUCIEL`; `Luciel_Duo.lua` + `Luciel_Demonio.lua` adopted (bytecode, `luabind.py check` both); motions copied with them (CH0b's holds); `[JUNE_CHAR] LUCIEL DATA` line at the end of InitState (class, script, motions, states, duo); play-test: create, enter the village, walk, jump, dash, combo, swap bodies, enter a dungeon, relog | Opus high, user play-test |

## Phase CHN - Job change (Aisha's CH-N template, every Lu/Ciel job class)

**Run with:** Opus 5.5 high, Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CLASS_CHANGE`, sub-tag `JOB`. June's class-change
route for 200 -> 37 / 38 / 39 and 37 -> 127, 38 -> 128, 39 -> 129: the quests (`CJT_*`, quest-system), the promotion items (`INIT_*` /
`PROMOTION_ITEM_ID_LIST` if June's script has one), the emulator's class-generic handlers (`Handlers_Quest.cpp`, `Handlers_Shop.cpp`),
`X2OfflineClassChangeSeed.h` `PARENT_ROWS` and `X2OfflineLuaEnumSeed.h`, `DLG_Class_Change_Luciel_*` (data217) and the class-change guide pieces
the change itself shows. Tree rows for the new class go to `SeedJuneAutoLearn` where free. **Done gate:** a level-15 Lu/Ciel reaches each first
job, and a level-35 one reaches each second job, by quest or item. The tree, the portrait and the motions follow, and a relog keeps it.

## Phase TU - Tutorial

**Run with:** Sonnet 5.5 high, Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_TUTORIAL`, sub-tag `TUTORIAL`. Add the
`DI_DUNGEON_TUTORIAL_LUCIEL` 41240 and `DI_BATTLE_FIELD_TUTORIAL_LUCIEL` 39609 rows to the core-list `DungeonEnum.lua` / `DungeonData.lua` (and
`DungeonEnum.h` under `JUNE_ENUM_NAMES`), the world `WI_TUTORIAL_LUCIEL_STAGE0` 633, adopt `Tutorial_Luciel_Stage0*` (data040), check
`Dungeon_Tutorial_Luciel.lua` with `luabind.py`, and port June's first-entry route for a new Lu/Ciel (NU0 item 8). **Done gate:** a new Lu/Ciel
plays the tutorial to its end and lands where June lands him.

## Phase CH2 - Skills: base and first jobs (200, 37, 38, 39)

**Run with:** Sonnet 5.5 high, Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_SKILL`, sub-tags `SKILL`, `NUM`. A skill's
tree row decides its phase: class 200 has it -> CH2a; otherwise the first of 37 / 38 / 39 that does.

| Sub-phase | Scope |
|---|---|
| CH2a0 | **Opus.** Summoned NPCs (CH0 item 7): spawn rules, scripts' binder check, the `NUI_SI_A_LCN_SHADOW` case; CH0 places it |
| CH2a | Duo (200) skills (19 tree rows) |
| CH2b | Chiliarch (37) skills |
| CH2c | Royal Guard (38) skills |
| CH2d | Diablura (39) skills (`Luciel_Demonio`'s `LSI_SA_*` for 39) |

For each id: its case in the skill head (`32b46580`) with June's state functions whole, its attached rows (7u names, 7ai BFI arguments, the
numerical-only tables), and its effect closure. Upgrade ids reach their base through `m_iDeleteSkillIDByUpgrade` (June `32071720`). `NUM use`
lines are logged, and March has no Lu/Ciel to compare against: `MODS.md` lists June's damage per skill. **Done gate:** every class 200 / 37 / 38 /
39 id in CH0's S3 baseline has a state (except passives); each skill cast from an A and a B slot plays June's state, motion and effect and deals
damage; both bodies' skills are tested.

## Phase CH3 - Skills: second jobs (127, 128, 129), hyper actives, transcendence

| Sub-phase | Scope |
|---|---|
| CH3a | Dread Lord (127) skills |
| CH3b | Noblesse (128) skills |
| CH3c | Demonio (129) skills; `CUSTOM_RENDER_STATE` x3 (7w), `SUPER_ARMOR_CUSTOM_COLOR`, the not-selected-unit offsets (with DU1) |
| CH3d | **Opus.** Hyper actives, the hyper cut-ins of both bodies (`m_HyperCutIn_Trans`, `m_MinorUnitHyperCutIn(_Trans)`), and the **transcendence rows** (template): the offline transcend allow-list, `LevelUpEvent.lua` rows (`:158`, `:381-383`, `:627-629`), `DLG_Transcendental_Change_Guide_Luciel`, checklist 7ao whole, `SPRT_HYPER_CLASS` skills and `TRANSCENDENTAL_SKILL_DATA` rows, `GetHyperModeTableName` (`31fdd490`; March's `bCiel_` branch), `HQ_Cutin_{LLD,LCN,LDN}_{LU,CIEL}_Transcendence`, the `list_TR_*` rows in `GetCharacterImageName` and the transcended pictures read from June's row pushes, and `POPUP_TR_*` |

**Done gate:** as CH2 for 127 / 128 / 129 and the hyper actives. A transcended Lu/Ciel (`settranscend`) casts the hyper-class skills, slots
A-5 / B-5 work, and both bodies' transcended pictures and cut-ins show. `skillicons.py check` = PASS after the transcendence rows.

## Phase CH4 - Passives and class mechanics

**Run with:** Opus 5.5 high, Opus review, user play-test.

| Sub-phase | Scope |
|---|---|
| CH4a | Lu/Ciel's passive state (June's `InitPassiveSkillState` counterpart, CH0 names it), the passive ids, their objects and readers, and the 7am delegate lists. Flag `_PASSIVE` |
| CH4b | The damage-exception override and the other overrides CH0 finds (`GetActualMPConsume` / `GetActualCoolTime` style), death prevention, the per-skill stores |
| CH4c | Memo and item readers: `MemoConditionLuciel`, the memo ids, and Lu/Ciel's branches in `CX2Game::UseItemSpecialAbility` / `X2BuffTemplet.cpp` |

**Done gate:** each passive's effect shows (CH0 names the stat or log line); 0 `no_state` rows left except passives with no state in June.

## Phase CH5 - UI and data

**Run with:** Sonnet 5.5 high, Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_UI`, sub-tag `UI`. The skill and hyper cut-in
table for both bodies (June's names from data152 / data195), the HUD portrait for both bodies, the combination-point gauge / UI if June has one
(CH0), the bounding-key dialog, the Lu/Ciel event-scene and cut-in names the june-gfx bullet lists, and the dialog pieces the earlier phases
did not take. **Done gate:** a select-screen pass of all ten characters with no new error; both bodies' cut-ins and portraits show June's art.

## Phase CH6 - Whole-character walk and gap reruns

**Run with:** Sonnet 5.5 medium, then the user's walk. Back up the save; one Lu/Ciel per class, made by creation (not only by `createchar.py`),
the tutorial and job change; one transcended unit per second job. The walk covers every skill in an A and a B slot on both bodies, the body switch,
combination points, a hyper, the summons, one dungeon per job, the character window's form switch, then **leave the game and return to the
village**. Re-run `census.py`, `charport_scan.py`, `charport_keys.py Luciel_Duo` / `Luciel_Demonio` and CH0's S3 baseline, and record before /
after in the Done note. Walk **the becoming-transcended transition** on un-transcended units (user rule, 2026-10-28): `/tc 1`, quest 42030
(`/qdone 42030`), book 288493 (`/additem 288493 1`), one per second job, with the guide popup and that class's picture, the new tree rows, the HUD
portrait, and a relog. **Done gate:** the walk passes; 0 unexplained `no_state` rows; no new `KEM_ERROR`.

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium. Write the `MODS.md` "June character: Lu/Ciel" section (flags, files, data, starter stand-in, the DU4
migration, revert order: exe + data036 + motion koms together; the migration does not revert). Check that no `_DEBUG` flag is left defined and that
the checklist rows are marked `(Lu/Ciel: <phase>)`. Add a "Template for Stage 8" block here, covering what the NU / DU foundation took and what
Rose and Ain reuse, and update `kickoffs/june-backlog/stage8-plan.md` and the backlog Stage 8 note (`char-rose` next).

## Play-test hand-off (every phase)

Dev console first; copy-ready lines with real values (unit, class, skill id and slot, `/additem`, `/qdone`). For each check give where / paste /
what it does / look for / if not. Log tag `[JUNE_CHAR]`, `python tools/logs.py --grep "JUNE_CHAR LUCIEL <sub>"`.

## Open decisions (for the user)

Settled at planning (user, 2026-11-02):
1. Code phases start after char-chung Z, ahead of `char-elsword`.
2. Starter costume: both promotion sets (15 rows, a labelled stand-in).
3. NESI renumber: June's, with the forward-only migration (DU4).

Open:
4. **Stat rows** for 37-39 / 127-129 / 200: asked again after CH0 item 8 reports what June data exists. Until then the synthetic curve is used,
   logged. CH0 (2026-11-02) reported: no June per-level table exists; the existing rows all follow June's relative table (CH0 Done note).
   **Settled (user, 2026-11-02): derive from June.** NU4 writes Lu/Ciel's `StatTable.lua` rows from June's `CharacterStatTable` relative rows with
   the fitted formula (levels 1-80; 81-99 by `cap99_gen.py` as for the other classes), a labelled stand-in under its own flag; the user packs it.
