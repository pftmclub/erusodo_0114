# June character port: Eve (Stage 7, first character after the pilot)

Plan key `char-eve` · branch `main` · kickoffs `kickoffs/char-eve/` · log tag `[JUNE_CHAR]` (lines start `[JUNE_CHAR] EVE <sub>`, sub-tags
`DATA`, `CORE`, `NCORE`, `SKILL`, `PASSIVE`, `UI`, `NUM`) · flags `SERV_IRUHADEV_JUNE_CHAR_EVE_*` for Eve's class code,
`SERV_IRUHADEV_JUNE_CHAR_*` for class-agnostic pieces (+ a temporary `..._DEBUG` per sub-phase) · backlog Stage 7
(`JUNE_PORT_BACKLOG_ORDER.md`, brief `kickoffs/june-backlog/stage7-plan.md`) · written 2026-10-25 by the `new-plan char-eve` session from
the Aisha plan's "Template for Stage 7" (`JUNE_CHARACTER_AISHA_PHASES.md`).

## Context

**What this plan does.** It moves Eve (unit type `UT_EVE` 5, script `Eve_Nasod.lua`, class `CX2GUEve` in `X2Lib/X2GuEve.cpp` / `.h`, seven
classes: Nasod 5, Exotic Gear 18, Architecture 19, Electra 26, Code: Nemesis 108, Code: Empress 109, Code: Battle Seraph 116) from her March
kit to her June 2017 kit, end to end: June script, skill states for June's tree, numerical damage, motions, class-only engine hooks
(`JUNE2017_CHARACTER_PORT_CHECKLIST.md` §1-§7ao), the Nasod Core, UI pieces, data and her transcendence rows.

**Why Eve is first (stage7-plan, 2026-10-22).** Smallest gap after Aisha (3 numerical-only attacks), no templet-backed June-only class, no
open decision in the backlog. **This session found one the backlog did not list:** the Nasod Core (below), the Eve counterpart of Rena's
`ADD_RENA_SYSTEM`. It is decision 1 at the end of this doc.

**She is bigger than Aisha.** Eve's June class spans `0x32965dd0`-`0x329cf86f` (Add starts at `329cf870`): **643** functions Ghidra has
carved, against Aisha's 426 by the same count (Aisha's real total was 560 once the uncarved state callbacks were added). Expect about 1.5x
Aisha's work: about 800 June functions, about 550 in the class core. Aisha took about 96 commits. The phase table below is sized for that up
front (template rule: split at planning time, not when a run overflows).

**Scope: June 2017 up to transcendence (user, 2026-10-20).** Third jobs (`UC_150`-`UC_186`) are out; the s3 tsv has no Eve row above 116.
Transcendence is live since Stage 6 closed (2026-10-24), so Eve's transcendence rows are in this plan's phases (CH3d, CH6), not waiting.

**The state she starts from.**
- **Skill system (Stage 3).** Saves are on June's trees. `s3_reform_ids_no_state.tsv` has 261 Eve rows (5: 15; 18 / 19 / 26: 29 each;
  108 / 109 / 116: 53 each), 127 distinct ids (106, 900, 25000-25605); **254 rows have no class state** (only 900 has one). An unported id
  falls to the class switch's `default:`.
- **Effect engine (June's).** June's `DamageEffect.lua` / `EffectSet.lua` ship merged; 7 Eve rows in `c11a_march_only_templets.tsv`.
- **Motions.** Eve's `Motion_EVE*` are March's (16 in data033 + `Motion_Eve_Buff_NPC` in data012), except `Motion_EVE_Common.x`, already
  June's size in the live kom. June has 70 (69 in data203).
- **Hooks waiting on the class port.** Checklist §7: 20 rows name Eve, 11 more apply to every class (`june2017_re/char_eve/plan_checklist_rows.md`).
- **The Nasod Core.** March carries Eve's 2014 Nasod Core (maneuver core that follows her in awakening, maneuver gauge, parrying states
  `ENSI_PARRYING_SMALL / _PARRYING / _PARRYING_BLINK`, `ENSI_BERSERK_MODE`, the gauge-UI orb `PGUWO_ORB_EVE` / `CX2EveMyGageUI`) behind
  `ADD_EVE_SYSTEM_2014` + `FIX_ADD_EVE_SYSTEM_2014`, both commented out in `Always.h` (`:58`, `:11`): about 175 sites in `X2GuEve.*` (116),
  `X2GUUser.*` (27), `X2GageUI.*` (26), `X2GageManager.cpp`, `X2DamageManager.h`, `X2GUNPC.cpp`. June's script uses it: the three parrying
  tables (`Eve_Nasod.src.lua:12872-12914`) and `ATTACK_TRIGGER_TYPE = DTT_EVE_MANEUVER_ATTACK` (core `Enum.lua` already has it, `= 5`).
  It is Eve's subject, so it is a phase here (CH1q).

**Out of scope (owned elsewhere).** New characters (Stage 8); UI that is not Eve's own (Stage 9); `DLG_1ST/2ND_Class_Change_Guide_Eve`,
`DLG_UI_JUMPING_SYSTEM_EVE_*` (Stage 9); `DLG_Transcendental_Change_Guide_Eve` (Stage 6's pattern: CH3d adopts it, see there);
`Tutorial_Eve_Stage0` + line map (june-dungeons); `PET_Mystic_EVE_3` (Stage 17); NPC / shadow / bot copies of Eve (`Npc_Eve_*`,
`*_SHADOW_EVE`, `PVP_BOT_EVE`, `TIME_EVE*`: whichever plan adopted June's NPC Lua, check the ledger); emotion motions' behaviour (Stage 10;
CH0b stages the files).

## Rules

The Aisha plan's Rules hold unchanged (`python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules"`), with Eve's names:
- **Standing rules** (`JUNE_PORT_BACKLOG_ORDER.md`, "Standing rules"): scope rule, no partial June updates, June wins, June bugs verbatim
  with a `JUNE_BUGS.md` row, cite June addresses, dev free-skill gate (`JuneIsDevUnlimitedSkillUse()`) in Eve's skill-use check.
- **The checklist is the work list.** Every Eve row in §1-§7ao is ported by a phase here or marked n/a by CH0 with the reason. The phase that
  ports a row appends `(Eve: CHn)` to it in place (checklist l.211).
- **One deploy unit.** June's `Eve_Nasod.lua`, her June motions (if CH0 finds a dropped animation March plays, see CH0 item 5) and the C++ that
  reads them go live together, in CH1 final, behind `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (undefined until then; the wiring compiles but never
  runs). Between phases Eve stays playable on her March kit.
- **Two flags for the core:** `_CORE` (the inert `June_*` transcriptions and the wiring) and `_LIVE` (the switch). Then `_NCORE` (Nasod Core),
  `_SKILL`, `_PASSIVE`, `_UI`, `_ASSET` (only if a June asset breaks a March reader), and a temporary `_DEBUG` per sub-phase, retired in its
  review.
- **Shared Lua per character.** `UnitTemplet.lua` is core-list BOM source since Aisha CH5: CH5 replaces only Eve's 7 rows. Her class script
  and her skill NPC / summon scripts are adopted whole (`adopt-june`, bytecode unless a phase must edit one).
- **Assets first, whole** (CH0b), byte-exact through `kom.py stage-june`; pack yourself, ask before every copy.
- **CP949.** `X2GuEve.*` (mixed line endings), `X2GUUser.*`, `X2GageUI.*` are CP949: `bytepatch.py` only.
- **Damage numbers.** Record March vs June damage per changed skill in `MODS.md`.
- **Saves.** Eve's skills are on June's tree; no schema change expected. A phase that needs one snapshots `db_backup/` first.
- **One code phase at a time in `X2GuEve.*`** (CH1-CH4). CH5 may run beside CH2-CH4.
- **No hardcoded enum literals** (CLAUDE.md, user 2026-10-26; `june-enum-names` closed at NZ): every June id with an Enum name is
  written by name from the first line, registered under a `SERV_IRUHADEV_` flag with June's value if C++ lacks it; `enumscan.py --check
  X2Lib/X2GuEve.cpp` stays 0. A phase that touches a use of an N3 duplicate local (`JUNE_CH1K_*` / `JUNE_CH1L_*` / `JUNE_RNSI_*`, an
  alias of an enum member) writes the member and drops the local after its last use.

## Starting context

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "<heading>"` |
| Checklist rows | `python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "<heading>"`; Eve's rows with line numbers: `june2017_re/char_eve/plan_checklist_rows.md` |
| Planning evidence | `june2017_re/char_eve/plan_*` (below); CH0 writes `ch0_*` beside them |
| The pilot's tools and maps | `june2017_re/charport_states.py`, `charport_keys.py`, `charport_closure.py`, `char_arme/ch0_build_*.py`, `char_arme/ch0c_listing_check.py`, `char_arme/ch0c_rebuild_reg_columns.py` |
| June Lua (reading) | `june_src_fixed/data036_kom/Eve_Nasod.src.lua`, `UnitTemplet.src.lua`, `Enum.src.lua` |
| Ghidra | `ghidra` skill; Eve's range `0x32965dd0-0x329cf86f`, vtable `334cf45c` (549 slots), ctor `3299bd70`; read-only in CH0 and reviews |
| Gap scan | `cd june2017_re && python census.py && python charport_scan.py <scratch dir>` (slow), `python charport_keys.py Eve_Nasod` |
| Bring June Lua in | `/x2-adopt-june` (`luasrc.py adopt-june data036 Eve_Nasod.lua`, `--dry-run` first) |
| Stage June assets | `python tools/kom.py stage-june <list>`, `/elstools pack dataNNN`, `/x2-kom-packed copy dataNNN` (ask first) |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply` (x2-byte-edit) |
| Hygiene / build / logs | `flagcheck.py <FLAG> --worktree --files ...`, `build.py --deploy` (background), `logs.py --grep "JUNE_CHAR EVE"` |
| Save / test character | `/x2-db` (`tools/elsdb.py setchar`, `settranscend <unit> 1`), `/x2-create-char <name> <class> <level>` |
| Dev console | auth level 3, END key; items `/additem <id> [qty]`, quests `/qdone <id>` |

## Planning evidence (2026-10-25)

Kept in `june2017_re/char_eve/` (digests written by Sonnet subagents, spot-checked; CH0 re-verifies anything it builds on):
`plan_charport_scan_20261025.txt` (full scan, every character), `plan_charport_keys.txt` / `.tsv`, `plan_march_side.md`,
`plan_june_side.md`, `plan_numerical_rows.tsv` (209 rows), `plan_checklist_rows.md` (every §1-§7ao row, Eve-named lines, Part B: the range).

Scans re-run 2026-10-25: `census.py` all PASS, `to-implement 0` (implemented 652, handed-off 9; regenerated files reverted);
`charport_scan.py` NUMERIC and ENUM identical to the 2026-10-20 run: `Eve_Nasod {'no NUMERICAL_DATA_INDEX': 25, 'no DAMAGE table': 3}`,
no Eve ENUM or KEYS line. (Run `charport_scan.py` with a short scratch dir: a long one passes Windows MAX_PATH and dies on a `.std.luac`.)

| Measure | Eve | (Aisha) |
|---|---|---|
| June class range | `32965dd0`-`329cf86f`, 643 carved functions, 69 vtable overrides in range | 426 carved, 560 real |
| June skill-tree rows | 261 over 7 classes; 127 distinct ids (106, 900, 25000-25605) | 267 / 130 |
| Rows with no class state | **254** | 260 |
| Rows by June skill type | 6: 69, 15: 53, 14: 47, 16: 35, 8: 27, 5: 14, 12: 7, 13 (hyper): 6, 1: 3 | |
| June script | `Eve_Nasod.src.lua` 14,137 lines, data only: 295 `ENSI_*` + 6 `INIT_*` tables (largest groups SA 37, A 28, RIDING 27, DAMAGE 21, DASH 17) | 313 |
| Script keys | `DAMAGE_DATA` 28 (3 numerical-only: `ENSI_A_EBS_ELECTRONIC_MOVE_FRONT / UP / DOWN`), `AT_COMMAND` 23, `TIME_STOP_IMMUNE_STATE` 3, `SHOW_HYPER_CUT_IN_TIME` 3, `APPLIED_SKILL_ID` 3, `DAMAGE_BOUNDING_*` 3, `SET_LIMIT_MAX_G_SPEED` 1, `WAIT_STATE_CHANGE` 1 | |
| Key census (`charport_keys.py Eve_Nasod`) | 239 keys: june-only **16** / neither 1 (`HEAD_IK`) / march-only-site 8 / both 214. June-only: 10 `CODE_RECOVERY_*` + `ADD_AIR_DASH_COUNT` in `INIT_COMPONENT` (reader `329c47e0`); `FORCE_DYNAMIC_CAMERA_INDEX` x15 / `RETAIN_CAMERA_STATE` (owned: backlog Stage 9 "CommonStateStart's camera keys"); `EFFECT_SET_LIST_FINAL_ULTIMATE` x5, `IMMUNITY_EDT_LIST_AT_THIS_STATE` x7 (no reader found); `BBT_CHANGE_NOW_HP_PER_SECOND`, `BFT_TIME` (damage data, census implemented) | 224: 7 / 2 / 2 / 213 |
| Numerical blocks | 209 `c1a` rows keyed to 50 Eve ids, all `ok` | 110 |
| Motions | June 70 `Motion_Eve*` / March 17; 10 base files differ; `s2_model_core.tsv`: `_NS`, `_BS`, `_Common` each **drop 1 animation name** (unnamed; `r12b_anim_loss.tsv` has 0 Eve rows: CH0 settles it) | none dropped |
| Alpha-tag sites (7c) | **167** (`c10a_alpha_sites.tsv` l.27-193; `32982623`..`329c84f8`) | 22 |
| March class | `X2GuEve.cpp` 34,813 lines, `.h` 2,629; `SetEquippedSkillFuncBySkillSlotIndex` `:2153-3212` (63 `case SI_*`); pressed-index decode `:6151-6177`; 7ag encoders `:3391-3473`, `:5529-5537`, `:5590-5598` + decoders near `:5481`, `:22110`, `:29793`, `:32358` | 21,220 |
| June class | `SetEquippedSkillFuncBySkillSlotIndex` `329c8a00` (vslot 0x7b4), `SetDamageExceptionProcess` `329bc560` (**June-only**), `SpecialAttackEventProcess` `32969300`, hyper effect start `32994710` (7ao), 7an reader `329b407b` | |
| Stat rows | `StatTable.lua` 1-99 for all 7 classes | same |
| Emulator job change | class-generic; Eve ids only in the seed tables | same |
| Save | units 19 / 27 (Empress lv80), 22 (Nasod lv99), 41 (Electra lv90); none of 18 / 19 / 108 / 116 | |
| Cut-ins | `X2GuEve.cpp:48-85`; Electra (26) has no `InitState` case, keeps set 0; `g4` 7 names, none with a June `.gfx` of that name (June's are in data152) | |

## Phases

| Phase | What | Model | Depends |
|---|---|---|---|
| CH0 | Eve map: class map, state / skill table, checklist rows, assets + effect closure, keys, registration listing check, S3 generator, unmapped helpers, the Nasod Core map, the motion question | Opus high | - |
| CH0b | Every June Eve asset, whole, first | Sonnet high, user play-test | CH0 |
| CH1a-CH1l4 | Class core transcription, inert `June_*` members (16 groups after CH0's splits, below) | Sonnet high, Opus review each | CH0; one at a time |
| CH1m-CH1p | Class core wiring behind `_LIVE` (registration; base callbacks + slot head; virtuals; 7ag + hooks + June-only keys) | Opus high, Opus review each | CH1a-CH1l reviews |
| CH1q | Nasod Core (decision 1) | Opus high, Opus review | CH1p review |
| CH1 | Final: define `_LIVE`, adopt the script, play-test | Opus high, Opus review, user play-test | CH1q review (and CH0b) |
| CH2a-CH2c2 | Skills: Nasod + first jobs (5, 18, 19, 26; CH0 split CH2a / CH2c) | Sonnet high, Opus review, user play-test | CH1 review |
| CH2b2 | How June spawns a summon: one NPC per owner or many (raised by the CH2b play-test: repeated Surface Cuting leaves several Oberons) | Opus high, Opus review, user play-test | CH2b play-test 2 |
| CH2b3 | Three unbound NPC Lua methods Oberon calls (`ApplyPassiveSkillOwnerUser_LUA`, `Reset/SetMultiplyPowerRate_LUA`) | Opus high, Opus review | CH2b play-test 2 |
| CH3a-CH3d | Skills: second jobs (108, 109, 116), hyper actives, transcendence rows | Sonnet high, Opus review, user play-test | CH2 reviews |
| CH`n`0 | One Opus sub-phase per unmapped helper CH0 finds, before the Sonnet phase that needs it (e.g. `CH2b0`) | Opus high, Opus review | as its phase |
| CH4a-CH4c | Passives; damage exception + overrides; El Crystal / recovery HUD and tree-UI pieces | Opus high, Opus review, user play-test | CH2 reviews |
| CH5 | UI and data: Eve's `UnitTemplet` rows, cut-in table, remaining dialog / tree pieces | Sonnet high, Opus review, user play-test | CH1 review; beside CH2-CH4 |
| CH-N | Not run: Eve has no templet-backed June-only class | - | - |
| CH6 | Whole-character walk + gap reruns | Sonnet medium, user walk | CH3, CH4, CH5 reviews |
| Z | Close-out | Sonnet medium | CH6 |

CH0 runs now (read-only, alongside anything). CH0b follows it. CH1-CH4 all edit `X2GuEve.*`: one at a time. CH5 runs beside CH2-CH4.
If CH0's counts put a CH1 group over ~45 functions, CH0 splits it with a digit suffix (`CH1c2`) and writes the kickoff; it does not merge groups.

## Phase CH0 - Eve map (read-only)

**Run with:** Opus 5.5 high. No code, no flag, no build. Ghidra read-only. Delegate reads, greps, tsv joins and known-address dumps to
subagents. Outputs in `june2017_re/char_eve/`.

**Step 0 (the pilot's recipe fixes).** Build the registration map with `charport_states.py init` on Eve's `InitState` / `InitStateCommon`
(found from vtable `334cf45c`, vslot 0x7a4) and `skill 329c8a00`, then run `ch0c_listing_check.py` on it: every row must match the raw
listing, all `~` rows and std::string-named rows (`USI_HYPER_MODE`, START / WIN / LOSE roll) by hand. Generalise the pilot's
`char_arme/ch0_build_*.py` / `ch0c_*` scripts to take a character argument (or copy them into `char_eve/` with the range changed) and say which.

**Scope** (the Aisha CH0 items, with Eve's numbers):
1. **Class map** `ch0_class_map.tsv`: every function in `0x32965dd0-0x329cf86f` (vtable overrides, registrars, state callbacks incl. uncarved
   thunks, helpers, dead), March pair, verdict, **target sub-phase from the table below**. Funcmap rows `Char Eve CH0`. Settle the three
   unowned functions between Eve's last override and Add (`329cf6d0` PushBuffFactor variants, `329cf850`, `329cf860`).
2. **State and skill table** `ch0_state_skill.tsv`: 301 / 301 script tables, 127 / 127 skill ids; dead-data and March-only states listed.
3. **Keys** `ch0_keys.tsv` (`charport_keys.py Eve_Nasod`): each of the 16 June-only keys becomes a CH1p / CH4c item or gets an owner
   (the two camera keys are Stage 9's already; find readers for `EFFECT_SET_LIST_FINAL_ULTIMATE` and `IMMUNITY_EDT_LIST_AT_THIS_STATE`).
4. **Checklist rows** `ch0_checklist_rows.tsv`: every §1-§7ao row -> phase or n/a with reason (start from `plan_checklist_rows.md`).
5. **Assets** `ch0_assets.tsv` + `ch0b_stage_list.tsv` (routes a name / b data / c `item.kim` Eve items, unioned) and **effect closure**
   `ch0_effect_closure.tsv` (`charport_closure.py`, run-time-built names included). **The motion question:** name the one animation each of
   `Motion_EVE_NS.x`, `_BS.x`, `_Common.x` drops in June (`s2_model_core.tsv` says -1 each; `r12b` says none) and whether March's
   `Eve_Nasod.lua` or `X2GuEve.cpp` plays it. If yes, CH0b and CH1 deploy together (checklist 7ad) and CH0b's kickoff says so.
6. **Unmapped helpers.** For every skill, list the callees outside the class range that funcmap does not map. Each gets an Opus `CH<n>0`
   sub-phase before the Sonnet phase that needs it (kickoff + STATUS row written by CH0).
7. **Nasod Core map.** Every `ADD_EVE_SYSTEM_2014` / `FIX_ADD_EVE_SYSTEM_2014` site (`cl /P` decides which `#else` is live today) paired with
   its June function or "absent in June"; June-only Nasod Core pieces; the gauge UI (`CX2EveMyGageUI`, `X2GageManager.cpp`), the NPC side
   (`X2GUNPC.cpp`), `X2DamageManager.h`. Output `ch0_nasod_core.tsv`, sized for CH1q (split it if over ~45 functions).
8. **S3 generator** (recipe defect 2): write `june2017_re/s3_no_state.py` (tree ids per class vs `case` labels / `const int` ids in the class
   file) so CH6 regenerates the census instead of rebuilding it by hand. Baseline: 254 Eve rows with no state.
9. **Shared data:** which `RNSI_*` / `ENSI_*` names the C++ enums need (house convention: raw id + `/// RNSI_` comment); Eve's 7
   `UnitTemplet` rows June vs March (lines 372-449, 664-715, 920-945, 1053-1078 of the BOM source).
10. **Amend the kickoffs** CH0b-CH5 ("Amended by CH0" sections: filters, addresses, counts, any split).

**Done gate:** the tsvs exist; `ch0c_listing_check.py` passes on every registration row; every Eve checklist row has a phase or n/a;
301 / 301 states and 127 / 127 ids mapped; assets union routes a / b / c; the motion question answered; the Nasod Core tsv exists; kickoffs
amended; open questions listed.

### CH0 - Done (2026-10-26)

Read-only, no flag, no build. Outputs in `june2017_re/char_eve/`. The builders are the pilot's scripts copied with Eve's constants
(`ch0_build_state_skill.py`, `ch0_build_class_map.py` + `ch0_hand_phases.tsv`), plus new ones: `ch0_build_assets.py eve`,
`ch0_unmapped_callees.py`, `ch0_live_lines.py` (live lines from a `cl /E` `.i`), and `june2017_re/s3_no_state.py` (recipe defect 2).

**Step 0.** `charport_states.py init 329b8030 329ba9c0 3202b110` -> `ch0_reg.tsv`; `ch0c_listing_check.py` reproduces **158 / 158** rows from the
raw listing. I also checked the 6 `~` rows and the 4 std::string rows by hand. Three tool fixes, after which Aisha's `ch0c_reg.tsv` and `ch0c_skill.tsv` still reproduce unchanged:
- the skill switch's epilogue label is found generically (was Aisha's `LAB_32cf06f8`);
- `s_X_<va> + n` labels resolve (Eve's default case);
- a case's own extra-state registrations now carry their callbacks (new column 10, incl. the `329cc2c0` copies).

**Maps**
- `ch0_class_map.tsv`: 854 functions in `32965dd0-329cf86f`: **802 to transcribe**, 37 dead, 15 not Eve's (CX2GUUser or shared code folded into the range, unwind funclets).
  The three unowned functions are settled: `329cf6d0` PushBuffFactor is CH4b (shared with Rose and Ara); `329cf850` / `329cf860` are empty CX2GUUser stubs.
- `ch0_state_skill.tsv`: 295 script tables, 293 names. 22 are dead data, and 3 March `_HYPER` states are dropped. June registers `ENSI_RIDING_DAMAGE_BACK` with no table: its script defines RIDING_DAMAGE_FRONT and HA_EBS_PSYCHIC_ARTILLERY twice each (JUNE_BUGS rows in CH1c / CH3d).
  **127 / 127 ids**: CH2a 12, CH2a2 12, CH2b 12, CH2c 8, CH2c2 4, CH3a / CH3b / CH3c 16 each, CH3d 6, CH4a 24 (types 1 / 5 / 8), CH1 1 (900).
  Fayner (25224 / 25225) has no case and plays the `default:` (`ENSI_A_EAT_FAENA`). 12 case ids outside June's tree are transcendence-memo skills (`31fdbcc0`): CH3d.
- `ch0_vtable_slots.tsv` (69 overrides; 545 slots in `ch0_vtable_all.tsv`), `ch0_checklist_rows.tsv` (every row: a phase or n/a), `ch0_keys.tsv` (239 keys; the 16 June-only keys have owners).
- `ch0_assets.tsv` 14,663 rows, routes a / b / c unioned; **`ch0b_stage_list.tsv` 5,197** (4,648 missing, 549 March copies; dry run: 0 refused; one new kom, `data203`).
  `ch0_effect_closure.tsv`: 7,231 rows, 2,443 names, 86 missing (54 sounds, 34 names June never defines).
- **The motion question:** no animation is lost. June moved Atomic Blaster NS -> EG and Taser Pilum BS -> PR; March's 108 / 116 rows load both files of each pair, so CH0b need not wait for CH1, but the 11 base motions are copied together (`ch0_motion_question.tsv`).
- **Nasod Core** (`ch0_nasod_core.tsv`, 126 rows; March sites `ch0_nasod_core_sites.tsv`, 77 sites in 9 files, all dead today by `cl /E`): same 100, differs 13, june-only 9, absent 4.
  CH1q transcribes 19 June functions; the rest keep March's ADD bodies. June uses the FIX form; `ENSI_BERSERK_MODE` is not in June; the gauge rules changed in 4 places.
- Item 9: 126 / 127 RNSI names are not in the C++ `SKILL_ID` enum (keep raw ids + `/// RNSI_`); 71 are not in the core Enum.lua (add one only for a Lua reader). UnitTemplet (`ch0_unittemplet_eve.tsv`): every March key equals June's; June adds `m_fLobbyRotationY` (ignored) and `m_HyperCutIn` (108 / 109 / 116, read).
- Item 6: 367 out-of-class callees, 94 cited nowhere; `ch0_unmapped_identified.tsv`: 45 confirmed, 45 candidate, 4 unknown. 34 confirmed engine / user ones are now funcmap rows.

**Split** (rule: ~45): CH1j -> CH1j + CH1j2 (Battle Seraph); CH1l -> CH1l / CH1l2 (Common*) / CH1l3 (skill use) / CH1l4 (CommonFrameMove); CH2a -> CH2a + CH2a2 (18); CH2c -> CH2c + CH2c2 (Sweep Rolling, Giga Stream).
Opus helper phases: **CH1j20** (June's slot key-held / slot-page helpers), **CH1l40** (7a modifier setters, customize-item struct), **CH2b0** (unit-cone collector), **CH3a0** (UI skill-tree notify, mesh float).
Functions per phase:
- CH1a 44, CH1b 39, CH1c 24, CH1d 45, CH1e 35, CH1f 25, CH1g 23, CH1h 11, CH1i 22, CH1j 30, CH1j2 46;
- CH1k 39, CH1l 10, CH1l2 9, CH1l3 26, CH1l4 37, CH1m 3, CH1n 2, CH1p 2, CH1q 49 (19 to write);
- CH2a 29, CH2a2 38, CH2b 21, CH2c 27, CH2c2 26, CH3a 39, CH3b 41, CH3c 35, CH3d 23, CH4a 7, CH4b 4, CH4c 2.

Every kickoff CH0b-CH5 has its "Amended by CH0" section: filters, counts, addresses, checklist rows, callees.

**Corrections to the plan:**
- Electronic Move (the 3 numerical-only tables) is Battle Seraph-only: it moves from CH2c to CH3c.
- 7n (writer `3297d730`, reader `329a67b0`) is Electra code: CH2c, not CH4c.
- The 7an reader `329b407b` sits in `329b3cc0`: CH1l3.
- June's InitState has no Electra cut-in case either (CH5 keeps set 0).
- funcmap l.1059's "slot 509" for `329b5af0` is slot 510.

**Drift** (scope rule): the 4 March `_INT.txt` definition files are now a bullet in backlog Stage 15. The `Tutorial_Eve_*` assets stay with june-dungeons; the guide-manager step stays with backlog l.480. Nothing needs a new plan.
`funcmap.tsv`: 111 rows `Char Eve CH0`.

**Open:** the Nasod Core map's 4 technical questions (in the CH1q kickoff). Next: CH0b (Sonnet) can start now; CH1a may run beside it.

## Phase CH0b - Every June Eve asset, whole, first

**Run with:** Sonnet 5.5 high, then a user play-test. No C++ unless CH0 found a March reader a June asset breaks (then
`SERV_IRUHADEV_JUNE_CHAR_EVE_ASSET`). Sub-tag `DATA`.

**Scope:** every non-Lua row of `ch0b_stage_list.tsv`: motions, body / face / hair, equipment, textures, effect meshes, sounds, cut-in art,
portraits. Stage byte-exact (`kom.py stage-june`), pack each kom, ask once with the whole kom list, copy what the user approves. Save a
pre-CH0b `ErrorLog.txt` first. **If CH0 found a dropped animation March plays**, stage and pack here but copy together with CH1 final.

**Done gate:** the stage list re-runs 0 `missing` / `march-copy`; in game one Eve per job tier, village and dungeon, no new `KEM_ERROR` /
`Motion Error` / missing texture or mesh against the pre-CH0b log; a costume, a weapon and three skills look right.

### CH0b - Done (2026-10-26)

Sonnet 5.5. No C++, no flag (CH0 found no March reader a June asset breaks). Assets only.
- **Reference:** `ErrorLog.txt` (272,178 B, last run 2026-10-25 19:06) saved as `data\ErrorLog_pre_CH0b_eve.txt` before the copy.
- **Staged:** `kom.py stage-june --jobs 8 june2017_re/char_eve/ch0b_stage_list.tsv`:
  `STAGE-JUNE OK: 40 kom(s), 5197 staged (4648 new, 549 replaced, 0 identical), 0 refused, 33s`. Replaced March files are backed up under
  `24965799\kom_backup_stage_june\20261026_030148`.
- **Packed:** `kom.py pack` x40, 40 `PACK OK`, 0 refusals (the list stages no Lua).
- **Copied (user approved all 40, 2026-10-26):** `kom.py copy` x40, 40 `COPY OK` (data002 52,505,756 B / 2,632 entries; data018 88,473,587 B / 5,795 entries;
  data033 144,652,971 B / 165 entries, which carries the 11 base `Motion_EVE*.x` together). `data203.kom` is a new archive (32,935,532 B / 54 entries, the June
  emotion motions); `X2Main.cpp` mounts `data001-226`, so it is mounted. Backups in `kom_backup_tools`.
- **Done-gate re-run:** `kom.py stage-june --dry-run` over the same list after staging: `5197 staged (0 new, 0 replaced, 5197 identical), 0 refused`. Every row is
  June's bytes in `kom_contents`; 0 stay `missing` or `march-copy`.
- **Save:** backup `db_backup\els_db_20261026_030525_pre_manual.sql`; added with `createchar.py` (lv 99, class costume + top gear): unit 57 EveExotic (Exotic Gear 18),
  58 EveArchi (Architecture 19), 59 EveNemesis (Code: Nemesis 108), 60 EveSeraph (Battle Seraph 116). Existing: 22 weevwe (Nasod 5), 41 ElectraT (Electra 26),
  19 Iivue / 27 jtCodeEmpress (Empress 109).
- **Play-test (user) is the gate.** Log tag `[JUNE_CHAR]` has no lines (no code); the check is `ErrorLog.txt` against `ErrorLog_pre_CH0b_eve.txt`
  (`KEM_ERROR`, `Motion Error`, missing texture / mesh lines). Names that no June kom holds (the 113 `dangling` rows) may show as KEM 428 / 429: June's own.
- **No new drift.**

## Phase CH1 - Deploy unit: June script, motions and the class core

**Groups** (CH0's map fills the counts and the exact function lists; the kickoffs carry the filters):

| Sub-phase | Run with | Scope |
|---|---|---|
| CH1a | Sonnet high, Opus review | Movement and slot-head states: wait, walk, jump, dash, charge MP, throw item, power attack, common buff |
| CH1a2 | Sonnet high (CH1b review spot-checks) | One `CX2GUUser` home for `31fceab0` / `32007b10`; Aisha's and Eve's copies retired (user, CH1a review) |
| CH1b | Sonnet high, Opus review | Reactions: damage (21 `ENSI_DAMAGE*` tables), down / stand-up, die, win / lose blocks |
| CH1c | Sonnet high, Opus review | Riding (27 `ENSI_RIDING*` tables) |
| CH1d | Sonnet high, Opus review | Nasod ground combos (Z / X chains) |
| CH1e | Sonnet high, Opus review | Dash (17) and jump combos |
| CH1f | Sonnet high, Opus review | Exotic Gear line (18) and its classes' combos |
| CH1g | Sonnet high, Opus review | Code: Nemesis line (108) |
| CH1h | Sonnet high, Opus review | Architecture line (19) |
| CH1i | Sonnet high, Opus review | Code: Empress line (109) |
| CH1j | Sonnet high, Opus review | Electra (26) line |
| CH1j2 | Sonnet high, Opus review | Code: Battle Seraph (116) line (split by CH0) |
| CH1k | Sonnet high, Opus review | Overrides below vslot 0x600 (lifecycle, render, reaction virtuals; ctor / dtor offset map) |
| CH1l | Sonnet high, Opus review | Overrides 0x600-0x7bc |
| CH1l2 | Sonnet high, Opus review | Common* overrides 0x7c0-0x7ec except CommonFrameMove (split by CH0) |
| CH1l3 | Sonnet high, Opus review | Skill-use overrides from 0x7f0 and their helpers (split by CH0) |
| CH1l4 | Sonnet high, Opus review | CommonFrameMove 0x7cc and the helpers only it reaches (split by CH0) |
| CH1l5 | Sonnet high, Opus review | One `CX2GUUser` home for the seven June skill-use helpers and their six fields; Aisha's and Eve's copies retired (user, CH1l3 review) |
| CH1m | Opus high, Opus review | Registration: June's `InitState` / `InitStateCommon`, the hyper-mode table name, START / WIN / LOSE roll, base event states, the `_LIVE` gate |
| CH1n | Opus high, Opus review | The CX2GUUser base callbacks Eve registers; `SetEquippedSkillFuncBySkillSlotIndex` `329c8a00` head (bounds, slot tables, the 7i rule) + the dev free-skill gate |
| CH1o | Opus high, Opus review | The virtuals under `_LIVE`; gaps the CH1a-CH1l reviews routed |
| CH1o2 | Opus high, Opus review | June-only and changed user virtuals that need a base + March call site first: 0x3ac, 0x7b0, 0x56c, 0x6a8, 0x65c, 0x584, 0x6ac (split by CH1o) |
| CH1p | Opus high, Opus review | 7ag pressed-index space (Mega Electronball, Taser Pilum, Kugelblitz encoders + every reader + the decode `:6151-6177`, together); hooks 7h / 7l / 7o / 7p / 7x; June-only script keys |
| CH1q | Opus high, Opus review | Nasod Core (decision 1 = (a): March's `ADD_EVE_SYSTEM_2014` code under our flag, June's bodies where they differ; `ch0_nasod_core.tsv`), flag `SERV_IRUHADEV_JUNE_CHAR_EVE_NCORE`, sub-tag `NCORE` |
| CH1 | Opus high, Opus review, user play-test | Define `_LIVE`; adopt `Eve_Nasod.lua` and the summon scripts CH0 marks CH1; pack, ask, copy; the play-test |
| CH1r | Opus high, Opus review, user play-test | CH1 play-test: June's `LoadEffectSetFromScript` `32024ff0` whole (`EFFECT_SET_LIST_FINAL` and the other June list variants; the dash-jump Z-X spear) |
| CH1s | Opus high, Opus review, user play-test | CH1 play-test: the third-awakening portrait ball (June gage data `+0xf4`, `32e6a210`) |

**Transcription rules (CH1a-CH1l):** each function whole, as an inert member `June_<March name>` (or `June_<STATE>_<Slot>`) under
`SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`; March's functions untouched; a class-range callee owned by a later group is declared with
`// June <va>: body in <phase>`; an unmapped engine callee or unexplained mechanism is "this needs Opus" (stop). June's numbers and quirks
verbatim; class code multiplies `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai). CH1c-CH1l alpha-tag calls (7c) go with the
function that makes them.

**Done gate (CH1 final):** Eve (any class) loads June's script and motions; `[JUNE_CHAR] EVE DATA` logs the load; no new `KEM_ERROR` /
`Motion Error` / missing-animation line against a pre-CH1 run; combos, dash, jump, damage, bounding and riding play June's animations; the Nasod
Core shows in awakening (per decision 1); `charport_scan.py` / `charport_keys.py Eve_Nasod` show nothing new.

### CH1a - Done (2026-10-26)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (`KTDXLIB/Always.h`, with a `#error` that `JUNE_INPUT_SIT_KEY` and `JUNE_EFFECT_ALPHA_OPTION` are defined), the commit "char-eve Phase CH1a". The 44 class-map rows of phase CH1a are 44 new `CX2GUEve::June_*` members:
declarations at the end of `X2GuEve.h`, bodies at the end of `X2GuEve.cpp`, each with its June address. Nothing registers, calls or overrides them; March's members are untouched. 44 funcmap rows `Char Eve CH1a`.

| June | Member (`June_` prefix) | June | Member |
|---|---|---|---|
| `32965f00` / `329695f0` | ENSI_WAIT_StartFuture / _Start | `32981680` | ENSI_WAIT_EventProcess |
| `32981910` | ENSI_WALK_EventProcess | `3297aad0` | ENSI_JUMP_READY_EventProcess |
| `3297acf0` / `3297ae70` | ENSI_JUMP_UP / _JUMP_DOWN_EventProcess | `32965f50` / `32981ba0` | ENSI_JUMP_LANDING_StartFuture / _EventProcess |
| `329cc1f0` / `32981eb0` | ENSI_DASH_StartFuture (thunk) / _EventProcess | `32977670` `32965fa0` `32969620` `329821a0` | ENSI_DASH_END_StartFuture / _Start / _FrameMove / _EventProcess |
| `32965fb0` `329824c0` `329776b0` `32977810` `32977a70` | ENSI_DASH_JUMP_StartFuture / _Start / _FrameMoveFuture / _EventProcess / _EndFuture | `32982790` | ENSI_DASH_JUMP_LANDING_EventProcess |
| `32965ff0` `32966020` `32982a00` | ENSI_DASH_JUMP_POWER_LANDING_StartFuture / _Start / _EventProcess | `32966040` `32982c00` `32977af0` `3297b070` `32966070` | ENSI_JUMP_FLY_UP_StartFuture / _Start / _FrameMoveFuture / _EventProcess / _EndFuture |
| `32966080` `32977b80` `3297b1f0` `329660b0` | ENSI_JUMP_FLY_DOWN_StartFuture / _FrameMoveFuture / _EventProcess / _EndFuture | `329660d0` `32982d30` `32977c10` `32977d30` `32977f80` | ENSI_JUMP_DASH_StartFuture / _Start / _FrameMoveFuture / _EventProcess / _EndFuture |
| `329cc0d0` / `329cc170` | ENSI_THROW_ITEM_FrameMove / _EventProcess (thunks to the relationship-skill virtuals) | `32970880` `32976e40` `32976e90` `32976f00` | June_StateChangeJumpDashIfPossible, June_CanEveRecovery, June_GetMaxDashJumpCount, June_GetMaxDashJumpUpCount |

**What differs from March** (the rest is March's body): the Code Recovery branch (`oneSit`, then cool time and MP) in WAIT, WALK, JUMP_LANDING, DASH, DASH_END, DASH_JUMP_LANDING and POWER_LANDING; the dash-jump limit (`32976e90`: Photon Booster float + 2, plus `ADD_AIR_DASH_COUNT` under BTI 671, plus skill 25419's air-move ability) and the dash-jump-up limit (`32976f00`) in JUMP_UP, JUMP_DOWN, DASH and JUMP_DASH; the Battle Seraph boost dash waits 0.5 s; DASH_JUMP's both-keys-held turn; five alpha tags (`329824c0` x2, `32982c00`, `32982d30` x2, checklist 7c); and the `m_bDownForce` resets June dropped (JUNE_BUGS 421). The others are March's code.

**New, June-only, under the flag:** the enumerator `ENSI_EVE_RECOVERY` (June script id 238, appended after `ENSI_TRANSFORMED`; CH4c writes the state); three fields `m_iJuneAddAirDashCount` (+0x9c60), `m_fJuneCodeRecoveryCoolLeft` (+0x9c64), `m_fJuneCodeRecoveryConsumeMp` (+0x9c6c), set to 0 in the constructor (CH1k's `InitComponent` `329c47e0` fills +0x9c60 / +0x9c6c from the script, CH4c owns +0x9c64); Eve-local `June_GetSkillLevel` (`31fceab0`) and `June_GetLearnedSkillAbilityValue` (`32007b10`), copies of the pilot's.

**Method, for CH1b-CH1l.**
- New tool: `python june2017_re/charport_adis.py eve <va>... | --phase CH1b --out DIR` prints the raw listing with the state-id names (`ch0_state_skill.tsv`), member names (`char_eve/ch1_offsets.tsv`, 72 fields and vslots read in this phase), vslot names, constants at their real width and wide strings. Ghidra's decompile hides every `StateChange( id, 0 )` argument in functions with an SEH frame and has no function for six of these (`32965fa0` `32966070` `32981910` `32982a00` `329cc0d0` `329cc170`; `32981680` runs on into `32981910`). Read the listing, not only the decompile.
- State ids: June's `USI_` ids are March's names (`0xd` WAIT .. `0x16` DASH_JUMP_LANDING); the Eve ids are in `ch0_state_skill.tsv` ("id N"); `0xee` is the one June-only name used here. `GetHyperModeStateID` (`32968470`) returns `0x17` = `USI_HYPER_MODE`, so the March macro with `USI_HYPER_MODE` is the port.
- `ShowComboCommand` takes `( id, command )`: June constructs the second string first, so `"J"` is the first argument. `m_FrameDataFuture.syncData.bIsRight` is +0x7e56, the Now copy +0x80f6.
- Constants: 0.65 is a float (`334c9ca8`), 1.85 / 1.0 / 0.5 / 45.0 / 30.0 are doubles; the inline `rate * (a ^ b ^ &c)` triples are `GetWalkSpeed` / `GetDashSpeed` (also called as `32071a80`) / `GetJumpSpeed` / `GetDashJumpSpeed`.

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h KTDXLIB/Always.h`: `files: 0 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `cl /Zs` of `X2GuEve.cpp` under the US_SERVICE defines: 0 errors, and the `cl /P` output holds the block (flag live). Build: `DEPLOY OK: X2_offline.exe is 25,857,024 B, dated 2026-10-26 03:38:08, md5 911e793a83973d644378908a70bf0e11, equal to X2/US_SERVICE/x2.exe; built at main 11a8652d + 3 uncommitted code file(s).`

**Open for the review** (named, not guessed): `+0x9c64` is named from its use as a cool time left (CH4c's writer confirms); `+0x96b4` is read as `m_sEGQueensThrone.m_bEnable`; ability type `0x80` is read as `SA_AIR_MOVE_COUNT` (March index 127 + 1); the shared `CX2GUUser` home for `June_GetSkillLevel` / `June_GetLearnedSkillAbilityValue` (the pilot and Eve keep copies); JUNE_BUGS 421 needs a play-test of Down in the air once CH1 is live. Nothing to play-test now: the code is unreferenced. Next: `/x2-phase char-eve CH1a review` (Opus 5.5 high), then CH1b.

### CH1a - Review (2026-10-26)

**PASS, no code fix.** All 44 members were read against June's raw listings (`charport_adis.py eve --phase CH1a`), the six with no Ghidra function and
`32981910` / `32982a00` in full. Ghidra decompiles of `32976e90`, `32977810` and `32982790` were spot-checked and agree.
- **Branch structure**: `32977810`'s both-keys turn (oneRight and not right turns right, else oneLeft and right turns left, else no change), with
  the speed set before `StateChange`. `32977d30`'s down-force goes straight to the power landing; its tail tests `pureDown`, not `oneDown`, and there is no riding test.
  `3297ae70`'s Queen's Throne test repeats the double-tap test. `32982790` runs its key chain after the airborne and Recovery branches (also after a Recovery
  `StateChange`). `32981eb0` uses `fStateTime >= 0.5`. All as ported.
- **Compare directions** (`test ah,1/5/0x41`) and **constants**: all match. The 1.85 / 1.0 / 0.65 doubles are March's `s_f*` statics widened (`X2GuEve.cpp:8-10`);
  `DASH_END`'s `/ 8.0f` is June's `* 0.125`, and its `+0x220 - +0x2b0` is `GetMaxAnimationTime()`. `EndFuture` calling `CommonStateEnd` (vslot `0x7dc`) in FLY_UP / FLY_DOWN / JUMP_DASH
  is March's own quirk (`X2GuEve.cpp:7908/7997/8211`). The skip-key list in `32981ba0` is `ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS` (`X2GUUser.h:245`) key for key.
- **Enums**: `ASUT_GROUND` 1 / `ASUT_AIR` 2, `EDS_END` 2, `BDS_RIGHT` 1 / `BDS_LEFT` 2, `UC_EVE_BATTLE_SERAPH` 116 = `0x74`, `0x8727` = `oneSit`.
- **Members named by use, confirmed from a June writer**: `+0x9c60` / `+0x9c6c` from `329c47e0` (`ADD_AIR_DASH_COUNT`, `CODE_RECOVERY_CONSUME_MP*`). `+0x9c64` is a
  cool-time countdown in `CommonFrameMoveFuture` `32980f90 @3298106d-3298109f` (if `0 < t`, `t -= elapsed`, clamped at 0). `+0x96b4` is written to 1 in
  `ENSI_SI3_EG_QUEENS_THRONE.FrameMove` (`3299d3f0 @3299dc10`; March `X2GuEve.cpp:19003`). `+0x99b4` is written in `InitPassiveSkillState` (`3297f290 @3297f437`), March's
  `m_iPhotonBoosterAdditionalDashJumpCount` (a float).
- **Ability `0x80` = `SA_AIR_MOVE_COUNT`**: June's `InitPassiveSkillState` reads ability `0x80` into the Photon Booster field (`3297f42d`, `3297f9b6`), the field March fills from
  `SA_AIR_MOVE_COUNT` (`X2GuEve.cpp:3634`).
- **JUNE_BUGS 421 withdrawn: not a June bug, a mapping error.** June moved March's per-state clears into Eve's `CommonStateEndFuture` override `3297a400 @3297a408-3297a437`:
  it clears `m_bDownForce` at the end of every state whose now-state index (state-data pointer `+0x7cbc` minus list base `+0x7cec`, / 100) is not 21 (DASH_JUMP).
  JUMP_DOWN / JUMP_UP / WAIT have no EndFuture, so they reach it by default, and DASH_JUMP's EndFuture `32977a70` reaches it and is excepted. A landing decides the power landing before the clear runs.
  **Owner: CH1l2** (its kickoff now says so). Port the clear with that function, or the leak 421 described becomes real in our tree.
- **Decision (user, 2026-10-26):** the two June `CX2GUUser` helpers get one shared home, new phase **CH1a2** (before CH1b). Both class copies' live branches already agree.
- Hygiene: `flagcheck.py JUNE_CHAR_EVE_CORE --range 40a43bd5^..40a43bd5`: `files: 0 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
  `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
  No rebuild: nothing changed in code. Nothing to play-test. Next: `/x2-phase char-eve CH1a2` (Sonnet 5.5 high), then CH1b.

### CH1a2 - Done (2026-10-26)

`CX2GUUser::June_GetSkillLevel` (June `31fceab0`) and `June_GetLearnedSkillAbilityValue` (`32007b10`) now live in `X2GUUser.h` / `.cpp` (public, non-virtual), flag `SERV_IRUHADEV_JUNE_USER_SKILL_LEVEL_HELPERS` (`Always.h:10129`, `#error` unless `SERV_IRUHADEV_JUNE_TRANSCEND_UNIT`, defined at `Always.h:9976`).
- Bodies are the live branches both class copies shared; the hyper-class test is `1 != m_iUnitClassTran` (`@31fceaef`). No `#else`.
- Aisha's (`X2GUArme_VioletMage.h/.cpp`) and Eve's (`X2GuEve.h/.cpp`) declarations and bodies are wrapped in `#ifndef` / `#endif SERV_IRUHADEV_JUNE_USER_SKILL_LEVEL_HELPERS`; flag-off is the old tree. Callers unchanged; a class with no member now resolves to `CX2GUUser`'s.
- Aisha behaviour note: with the flag on, her hyper-class test reads `m_iUnitClassTran == 1` where the `ARME_TRANSCEND`-off branch read `June_UnitFlag3c`; `ARME_TRANSCEND` is defined, so the live test is unchanged.
- Left alone: the inline readers in `X2DamageManager.cpp:5120` and `X2GameUnit.cpp:17442`.
- **For later character plans:** their CH1 kickoffs call the `CX2GUUser` pair and never copy it (also in `kickoffs/june-backlog/stage7-plan.md`). `funcmap.tsv` has the two rows.
- Hygiene: `flagcheck.py JUNE_USER_SKILL_LEVEL_HELPERS --worktree --files <7 files>`: `files: 6 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
- Deploy: `DEPLOY OK: X2_offline.exe is 25,857,024 B, dated 2026-10-26 04:19:19, md5 852dae1226f7571baef1067907cb406f, equal to X2/US_SERVICE/x2.exe (2026-10-26 04:19:12); built at main 7224239c + 7 uncommitted code file(s).`
- Not run: the `cl /P` "no `::June_GetSkillLevel` definition" probe of the two class `.cpp`s; the `#ifndef` pairs are proven by `flagcheck`, and the CH1b review runs the probe. Nothing to play-test. Next: `/x2-phase char-eve CH1b`.

### CH1b - Done (2026-10-26)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (a second block at the end of `X2GuEve.cpp`, declarations at the end of the class). **38 of the 39 class-map rows** are new `CX2GUEve::June_*` members; the 39th, `3299b810`, was a CH0 attribution error (the `3299d140` listing runs on past an `int3` into `3299d390`) and is now CH2b's (class map row, CH2b kickoff amended). 38 funcmap rows `Char Eve CH1b`. Nothing registers or calls them.

| June | Member (`June_` prefix) |
|---|---|
| `329af800` / `329af950` | ENSI_DIE_FRONT / BACK_Start |
| `329af920` / `329afa70` | ENSI_DIE_FRONT / BACK_FrameMove |
| `329694b0` `32969510` / `32969550` `329695b0` | ENSI_DIE_FRONT / BACK_StartFuture, _EventProcess |
| `32969660` `329696b0` / `32969820` `32969860` | ENSI_DAMAGE_DOWN_FRONT / BACK_FrameMove, _EventProcess |
| `329699d0` | ENSI_DAMAGE_AIR_DOWN_LANDING_EventProcess |
| `32969af0` `32969b70` `32969bf0` `32969c70` | ENSI_STANDUP_ROLLING_{FRONT_FRONT, FRONT_BACK, BACK_FRONT, BACK_BACK}_EventProcess |
| `32966110` `32966150` `32966190` `329661d0` | the four ROLLING _End |
| `32969cf0` `32969d70` / `32966210` `32966260` / `32966220` `32966270` | ENSI_STANDUP_ATTACK_FRONT / BACK_EventProcess, _FrameMove, _End |
| `329cc050` `329cc0b0` `329cc0c0` `329cc150` `329cc1e0` / `329cc160` | thunks: ENSI_DAMAGE_BOUNDING_UP_Start / DOWN_EventProcess / UP_EventProcess / LANDCRASH_EventProcess / UP_StartFuture; ENSI_DAMAGE_GROGGY_EventProcess |
| `3298cfe0` `32997c10` `3299d140` `329ae670` `3299bcd0` | helpers: EGQueensThroneReset, ResetLinkOverChargeIllusion, SetSelfDestructSummonedNPC( int ), ClearGuardAndSummon, `JuneEveBlock9c50::Reset( bool )` |

**What differs from March** (every damage / stand-up / rolling body is March's, line for line, checked against the listings): the Die Start/FrameMove pair. `Start` calls `SetSelfDestructSummonedNPC` for six ids (`0x655` `0x656` `0x180` `0x207` `0x2aa` `0x5ae`), resets the Queen's Throne (also `m_dMaxLifeTime = 30.0`), destroys the Sweep Parade handle (`+0x99c0`), and resets the `+0x9c50` block; `FrameMove` is `DieFrameMove` then `329ae670` (BTI `0x233` erased, id `0x82b` NPC, Oberon Guard effect set stopped, summon uid -1). In `June_SetSelfDestructSummonedNPC` the Atomic Shield and ids `0x655` `0x656` `0x82b` are killed with `SetNowHp( 0 )` (NPC vslot `0xf4` = slot 61, funcmap `3202ff60`); Ferdinand (`0x5ae`) and the watches (`0x180` `0x207`) skip an NPC that already self-destructs. The bounding / groggy rows are thunks to `CX2GUUser`'s June virtuals (`JuneBounding*`, `DAMAGE_GROGGY_EventProcess`); `Always.h`'s `#error` for the flag now also needs `SERV_IRUHADEV_JUNE_BOUNDING_STATE`.

**Mapped to March members (by use + ctor `3299bd70`):** `+0x96a8` `m_sEGQueensThrone`, `+0x976c` `m_hOberonGuard`, `+0x99c0` `m_hEffectSweepParadeTriangle`. **New June-only, under the flag, set in the ctor:** `m_pJuneBlock9c50` (+0x9c50, pointer to a 0x14-byte block made in the otherwise dead `3297a7b0`; nested struct `JuneEveBlock9c50`, fields unnamed by meaning) and `m_iJuneSummonNpcUid` (+0x9c08, 64 bit). CH1k's ctor / dtor map owns their final names.

**Left for others:** `ClearManeuverCore( true )` (`329ac350`) sits under `#ifdef ADD_EVE_SYSTEM_2014` (undefined today) in both Die Starts; CH1q makes it live (kickoff amended). No JUNE_BUGS row: nothing here looks unintended in June (the DOWN roll whose two outcomes are one state is March's own).

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h KTDXLIB/Always.h`: `files: 2 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `2 deleted line(s)` (the flag's own `#error` pair in `Always.h`, widened), `0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Build: `DEPLOY OK: X2_offline.exe is 25,861,632 B, dated 2026-10-26 04:43:37, md5 9ec66c5f923f87058917836be5ff2fb4, equal to X2/US_SERVICE/x2.exe (2026-10-26 04:43:30); built at main 9caac394 + 3 uncommitted code file(s).`

Nothing to play-test: the code is unreferenced. Review: `kickoffs/char-eve/CH1b-review.md` (also spot-checks CH1a2). Next: `/x2-phase char-eve CH1b review` (Opus 5.5 high), then CH1c.

### CH1b - Review (2026-10-26)

**PASS, no code fix.** I read all 38 members against June's raw listings (`charport_adis.py eve --phase CH1b`). The five helpers and the Die pair I read in full, and Ghidra spot-checks of `3299d140`, `32997c10` and the ctor `3299bd70` agree.
- **`3299d140`**: the g_pX2Game handle test, then per unit: `GUT_NPC` (`+0x264 == 1`), id (templet `+0x85f8`, skipped for id 0), `GetNowHp() <= 0` (gage `+0xc04`; a NULL gage counts as alive, as March's inline does), AI `+0x7c98` with `NAT_ALLY` (1), and ally uid `+0x78/+0x7c` against vslot `0x54`.
  The switch is exact. `0x5ae`: skip if `+0x8b40` (self-destructing), then `StartSelfDestruction( 0 )` (`31e4d560`) and `32997c10`. `0x180` / `0x207`: the same skip, then 3.0 s, plus `AttachParticle_LUA( true, "CountDown3", "Bip01_Head", (0, 80, 0) )` (Ghidra: `334cfd24` / `334cfd30`). `0x2aa` `0x655` `0x656` `0x82b`: vslot `0xf4` `SetNowHp( 0 )` with no self-destruct test. June dropped March's general "already self-destructing" skip, and the port follows June.
- **`3298cfe0`**: `+0xc` `m_bEnable`, `+0xb8` `m_iSelectBoneNameID` (after `wstring[5]` at `+0x2c`), 4 mesh handles at `+0x14` and 2 particle handles at `+0x24`, each destroyed when valid and then set to -1, then `+0x0` = 30.0. This matches `EG_QueensThroneData`'s layout and the port.
- **`3299bcd0`**: both effect sets are stopped through local copies (`321dafb0( h&, false )`, which is March's one-argument `StopEffectSet`). Only for `true`: `+0xc` -1.0, `+0` false, `+0x10` 1.0, both handles -1. Exact.
- **`329ae670`**: BTI `0x233` erase (`3229e160` = `EraseBuffTempletFromGameUnit`, as in the buff port), `SetSelfDestructSummonedNPC( 0x82b )`, `StopEffectSet( m_hOberonGuard )` by reference (when valid), and `+0x9c08/+0x9c0c` = -1. **`32997c10`**: BTI `0x199` erase, then `IsMyUnit` (vslot `0x198`) -> `UpdateSkillSlotUI( false )`. Exact.
- **Die pair**: `CommonStateStart`, `DyingStart` (vslot `0x338`), play speed 0.5, the six ids in June's order, the Throne reset, `DestroyInstanceHandle( +0x99c0, false )`, `329ac350( 1 )`, then the block (NULL-tested). FRONT and BACK differ only in the dust time (0.19 / 0.32; 1.12 both). June calls `329ac350` unconditionally. The `ADD_EVE_SYSTEM_2014` guard is CH1q's to lift, as already routed.
- **Fields (ctor `3299bd70`, EBP = -1, EBX = 0)**: `+0x976c` = -1 (`@3299be62`), `+0x99c0` = -1 (`@3299c0d4`), `+0x9c08/+0x9c0c` = -1 (`@3299c89d/8a9`, so 64 bit, which matches `UidType`), `+0x9c50` = NULL (`@3299cd60`). The port's ctor lines agree.
- **Thunks** against vtable `334cf45c`: `329cc0b0` -> `0x5d4` `31fd0230` (BOUNDING_DOWN EP), `329cc150` -> `0x5d8` `31fd0260` (LANDCRASH EP), `329cc1e0` -> `0x5dc` `31fc9d20` (UP StartFuture), `329cc050` -> `0x5e0` `31fd02d0` (UP Start), `329cc0c0` -> `0x5e4` `31fd0340` (UP EP), `329cc160` -> `0x5e8` `31fcbbe0` (GROGGY EP). Each port thunk makes the matching virtual call (`X2GUUser.h:2184`, `:2297-2301`).
- **Body spot-check**: in `329696b0` (DAMAGE_DOWN_FRONT EP) all six flag-write branches, the 0.66 test, both `[4]` outcomes `0xa2` and the `CommonEventProcess` tail match the port.
- **`3299b810`**: confirmed. The `3299d140` listing runs past the `int3` at `3299d38f` into `3299d390` (SURFACE_CUTING FrameMove), whose call at `3299d3cc` is the real caller. The CH2b kickoff amendment is right. I corrected the class-map row's caller column (`3299d140` -> `3299d390`).
- **CH1a2 spot-check: PASS.** A `cl /P` probe of `X2GUUser.cpp`, `X2GuEve.cpp` and `X2GUArme_VioletMage.cpp` with the flag on shows exactly one definition of each helper, `CX2GUUser::June_GetSkillLevel` / `June_GetLearnedSkillAbilityValue`, and none and no declarations in the two class files. All calls (Eve 2 + 2, Aisha 15 + 20) resolve to the base. Flag off, the `#ifndef` copies return.
- **Hygiene** (`--range 8e3a2e2c^..8e3a2e2c`): `files: 2 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 2 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The 2 deleted lines are the widened `#error` pair.

No build (no code change). CH1c kickoff amended: `ENSI_RIDING_DIE` reuses CH1b's Die-front members. Next: `/x2-phase char-eve CH1c` (Sonnet 5.5 high).

### CH1c - Done (2026-10-26)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (a third block at the end of `X2GuEve.cpp`, declarations at the end of the class). **17 of the 24 class-map rows** are new `CX2GUEve::June_ENSI_*` members; each June function is a bare `mov eax,[ecx]; jmp [eax+slot]`, so each member makes the matching `CX2GUUser` virtual call (Eve overrides none of these slots, `ch0_vtable_all.tsv`). Nothing registers or calls them. 17 funcmap rows `Char Eve CH1c` PORTED, 3 more "not ported here".

| June | Member (`June_ENSI_` prefix) | vslot -> virtual |
|---|---|---|
| `329cc180` `329cbff0` `329cc070` `329cc100` `329cc1a0` | PEPPER_RUN_READY_EventProcess / PEPPER_RUN_FrameMoveFuture / PEPPER_RUN_EventProcess / PEPPER_RUN_END_FrameMove / _EventProcess | `0x58c`-`0x59c` |
| `329cc010` `329cc090` `329cc120` | PEPPER_RUN_JUMP_UP_StartFuture / _FrameMoveFuture / _EventProcess | `0x5a0`-`0x5a8` |
| `329cc1b0` `329cc020` `329cc0a0` | PEPPER_RUN_JUMP_DOWN_StartFuture / _FrameMoveFuture / _EventProcess | `0x5ac`-`0x5b4` |
| `329cc060` `329cc0f0` `329cc190` `329cc000` `329cc080` `329cc110` | RIDING_OFF_End, RIDING_WAIT / WALK / JUMP_LANDING / DASH / DASH_END_EventProcess | `0x610`-`0x624` |

The PHOENIX_RUN states reuse the PEPPER_RUN thunks (same addresses), so one member serves both.

**Seven class-map rows are not CH1c's** (rows re-phased in `ch0_class_map.tsv`, scope rule):
- **`329ac330`** (`ENSI_RIDING_HYPER_MODE` FrameMove) is `32997c60( 0.0f, 0.1f, true )`, June's `CommonHyperModeFrameMove`: 432 decompiled lines with the awakening effect-set stand-ins and unmapped callees `31febf50` / `32290350` / `31fae370`. A bare declaration would not link (US_SERVICE has no function-level linking), so the row moves to **CH1k** with its callee; the CH1k kickoff is amended. CH0's "called by" column for `32997c60` missed this caller.
- **`329cc030` `329cc130` `329cc1c0`** (AMUSEMENT_ITEM_PHOENIX_RUN EventProcess; targets `31fcbb10` `31ff9160` `31fc7de0`): PvP amusement mode, no PvP offline. **`329cc040` `329cc140` `329cc1d0`** (DANCE_WAIT / Z / X EventProcess; `31ff9280` `31fcbb60` `31fcbba0`): June `CX2GUUser` callbacks March lacks, the dance states are `JUNE_PORT_BACKLOG_ORDER.md` Stage 10 (which already names these three thunks). Both groups' targets have no March counterpart, and the pilot left them out the same way. The registration (CH1m) leaves out the seven states `ENSI_AMUSEMENT_ITEM_PHOENIX_RUN*` / `ENSI_DANCE_*`. Drift: nothing new, both have owners.
- `ENSI_RIDING_DIE` stays CH1b's (kickoff amendment): nothing here.

**JUNE_BUGS 422:** the riding back-damage state has no script table and `RIDING_DAMAGE_FRONT` is defined twice (CH0), Eve's twin of row 379.

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Build: `DEPLOY OK: X2_offline.exe is 25,861,632 B, dated 2026-10-26 06:28:09, md5 5ebc1fe0e26f980a6e526ca4c10c4a9c, equal to X2/US_SERVICE/x2.exe (2026-10-26 06:28:02); built at main 4d5a0a14 + 2 uncommitted code file(s).`

**Open for the review:** the three decisions above; whether the "declare with `body in CHn`" instruction in the CH1j-CH1q kickoffs links (see `CH1c-review.md`). Nothing to play-test: the code is unreferenced. Next: `/x2-phase char-eve CH1c review` (Opus 5.5 high), then CH1d.

### CH1c - Review (2026-10-26)

**PASS, no code fix.** I checked all 17 members against June's raw listings (`charport_adis.py eve --phase CH1c`): each is `mov eax,[ecx]; jmp [eax+slot]`.
- **Slots**: `0x58c`-`0x5b4` (11 pepper-run) and `0x610`-`0x624` (6 riding) match the funcmap rows, and the Eve vtable `334cf45c` (`ch0_vtable_all.tsv`) holds the base `CX2GUUser` target at every one of them (`in_range` 0). Each member calls the matching `CX2GUUser` function by its March name (`X2GUUser.h:2169-2181`, `:2359-2382`, all `virtual`).
- **`RidingOffEnd`** is non-virtual in March (`X2GUUser.h:2351`, June slot `0x610`). `X2GuEve.h` declares no member of that name, so the call binds to the base, which is the function Eve's June slot holds (`31fcde30`). Aisha overrides `0x610` (`32cc8ae0`, `June_RidingOffEnd`); Eve does not. Exact.
- **Decision 1 (PHOENIX / DANCE left out): confirmed.** `31fcbb60` / `31fcbba0` (DANCE_Z / _X): when the animation ends (`+0x2ac`), `StateChange( 0x81 )` (GUSI_END + 124, DANCE_WAIT, `X2GUUser.h:472`), then `CommonEventProcess` (`0x7d4`). `31ff9280` (DANCE_WAIT): `StateChange( 0xd )` when a global manager flag (`+0xbb4`) is clear, then the same tail. March has neither the functions nor state ids for them (`SERV_IRUHADEV_JUNE_RESERVED_STATE_IDS` leaves +118..+124 unassigned). Both groups already have owners (Stage 10, PvP).
- **Decision 2 (`329ac330` -> CH1k): confirmed.** The listing is `fld 0.1; push 1; fldz` then `call 32997c60`, i.e. `32997c60( 0.0f, 0.1f, true )` with `this` passed through. The CH1k amendment is right.
- **Decision 3 (JUNE_BUGS 422): accepted** as Eve's twin of row 379.
- **Link question: a bare declaration does not link.** A two-file VS2010 test (`cl /O2`, `link /OPT:REF`) whose compiled member `Inert()` calls a declared-only `Later()` and is never called fails with `LNK2019` / `LNK1120` without `/Gy`, with `/Gy`, and through a static `.lib`. So every member of `X2GuEve.obj` needs its callees' bodies. Kickoffs amended: line 26 of CH1d-CH1l4 (a later-phase class callee is transcribed here, or the caller moves to its phase; never a bare declaration); CH1k / CH1l2 leave out the calls to the June-only 7ah statics `322e65c0` / `322dee90` with a `// June <va>: call added in CH4c` marker; CH4c (new amendment) ports both statics and adds the three calls. CH1m-CH1q and CH2+ kickoffs carry no "body in" instruction.
- **Hygiene** (`--range 798d87b4^..798d87b4`): `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.

No build (no code change). CH1d's kickoff still matches (45 class-map rows; depends on this review). Next: `/x2-phase char-eve CH1d` (Sonnet 5.5 high).

### CH1d - Done (2026-10-26)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (a fourth block at the end of `X2GuEve.cpp`, 1,284 lines; declarations at the end of the class and inside `EveElectraSystem`). **All 45 class-map rows of phase CH1d**: 38 state functions are new `CX2GUEve::June_ENSI_COMBO_*` members, 3 helpers are new `EveElectraSystem::June_*` methods, 4 are compiler-made pieces of `EveElectraEffectData` (not ported, below). Nothing registers or calls them. 41 funcmap rows `Char Eve CH1d` PORTED, 4 "not ported here"; the 7 helper rows of the class map now carry their March pair.

| State (`June_ENSI_COMBO_` prefix) | June: Start / End / FrameMove / EventProcess (and others) |
|---|---|
| `Z` `ZZ` `ZZZ` `ZZZZ` | `32969e20` `32983440` `329834a0` `32969e40` / `3296a1d0` `32983690` `329836f0` `3296a1f0` / `3296a700` `329838e0` `32983940` `3296a720` / `3296adc0` `32983b30` `32983b90` `3296ade0` |
| `ZZZX` | FrameMove `32983f60`, EventProcess `3296b080` |
| `ZZZdownZ` | StartFuture `3296b320`, Start `3296b380`, End `3296b3e0`, FrameMove `32984060`, EventProcess `3296b400` |
| `X` `XX` `XXX` | Start `329673f0` `32967410` `32967430`, FrameMove `329afc70` `32984d60` `329b0260`, EventProcess `3296b9f0` `3296bd90` `3297c830` |
| `XXfrontX` `XXfrontXX` `XXdownX` `XXdownXX` | EventProcess `3296c850` / FrameMove `32985710` + EventProcess `3296cab0` / EventProcess `3296cbd0` / FrameMove `32985930` + EventProcess `3296ce30` |
| `EveElectraSystem::June_AddEffectToSystem` `June_DestroyPlaneAndCrystalEffect` `June_CreatePlane` | `32980580` `32991a10` `329a1150` (March's `AddEffectToSystem` four-name form, `DestroyPlaneAndCrystalEffect`, `CreatePlane` Battle Seraph form) |

**What differs from March** (the rest is March's EVE_ELECTRA body):
- **Event flags** (Z ZZ ZZZ X XX ZZZdownZ): the change arm fires on `t > stateChange` **or** when the animation has ended (`32969e40 @32969ea4`); the BWALK / WALK / DASH cancels also need `IsOnSomethingFuture`. Both are local macros at the head of the block (`JUNE_EVE_*`); `SKILL_CANCEL_AFTER` is the buff port's June expansion.
- **ZZ** gains two arms (flags 1 and 2: Code Nemesis / Empress `X` -> `ENSI_ECN_COMBO_ZZX_READY` 231 / `ENSI_ECE_COMBO_ZZX` 234), **ZZZ** one (flag 4, Code Nemesis). Both ids are new enumerators under the flag (like `ENSI_EVE_RECOVERY`); CH1g / CH1i own the states.
- **ZZZdownZ**: the follow-up flag needs EG or Code Nemesis (March: also Architecture, Empress) and uses `m_fEventTime[ 0 ]`; the non-EG effect `EVE_ZZZdownZ` is `GetPowerRate() * m_fAttackPowerRateElectronBall` with `m_fForceDownValueRateElectronBall`.
- **X / XX FrameMove**: `FlushMp( 4 * m_fMPConsumeRateElectronBall )` (March 8); the fire ball is the `_BIG` set (own filtered names) when `GetRandomFloat( 0 ) < m_fJuneElectronBallBigRate` (new field, +0x97bc, default 0); effect power `GetPowerRate() * m_fAttackPowerRateElectronBall`; `AddEffectToSystem` before `SetInheritForceDownRate`. **XXX**: `FlushMp( 4 )` (March 7), non-EG triangle scaled like the above.
- **XXfrontXX / XXdownXX FrameMove**: at 0.01 s with Queen's Throne on, `Effectset_EEG_QueensThrone_ComboX4Front` is played (March: none). The two functions are identical: **JUNE_BUGS 423**.
- **XXX EventProcess** writes `nowAction` 14 / 15 below 0.4 s (`UAI_ELECTRA_PLANE_ANGLE_UP / DOWN`, same values as June's constants); XX flag 5 sets `m_iSuccessiveHit_OneZ` (`0x9698`) before the Nemesis state change.
- **Alpha tags (7c)**: 19 sites, 20 calls (`329b07d7` serves both the third `Light_Eve_AttackX3` and the 0.23 s triangle): ZZZX 1, ZZZdownZ 3, X 1, XX 1, XXX 4, `CreatePlane` 10.
- **Members by use**: `0x962c`.. = `m_ahMajorMeshInstance[ MajorMID_EG_SP3_Combo_Z1 ..]` (base `0x95f0`); `+0x97ac` range rate, `+0x97b0` MP rate (a `KProtectedType<float>` in June, March's plain float is used), `+0x97c0` / `+0x97c4` power and force-down rates (writers in `3297f290`, CH4a); `+0x99a8` `m_fPlaneZAngle`; `+0x9868` `m_EveElectraSystem.m_fPlaneZAngle`.
- **Helpers**: `32980580` / `32991a10` / `329a1150` are March's methods (June's `329a1150` also tags its four meshes and six particles). `3297b6b0` `329cdc80` `329cdf80` `329ce570` are the data's constructor, destructor, copy constructor and list node: June's struct holds two members March's lacks (`+0x8c` int -1, `+0x90` `std::vector<int>`), nothing in the class range reads them (CH1l4 kickoff amended); not mirrored.
- Later kickoffs amended: CH1g / CH1i (enumerators), CH1j / CH2c / CH2c2 / CH3c / CH1l4 (call the `June_*` Electra methods), CH4a (writer of `+0x97bc`).

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `cl /Zs` of `X2GuEve.cpp`: 0 errors; the `cl /P` output holds the block. A script cross-check of every listing against its member (wide strings, `StateChange` ids, float constants): 0 differences.
Build: `DEPLOY OK: X2_offline.exe is 25,890,816 B, dated 2026-10-26 07:22:46, md5 391777dd7cfe8d9104cd8379931b63c6, equal to X2/US_SERVICE/x2.exe (2026-10-26 07:22:39); built at main add2af39 + 2 uncommitted code file(s).`

**Open for the review**: the two macros' shape (animation-end clause, on-line test); the class-read form per site (templet `+0xaf0` vs unit data `+0x38`); `m_fJuneElectronBallBigRate` as the name of `+0x97bc` (a chance against `GetRandomFloat`); the plane math of `329a1150` (matched call by call to March's, with the constants; the x87 arithmetic itself was not re-derived); the enumerator values vs June's ids (CH1m). Nothing to play-test: the code is unreferenced. Review: `kickoffs/char-eve/CH1d-review.md`. Next: `/x2-phase char-eve CH1d review` (Opus 5.5 high), then CH1e.

### CH1d - Review (2026-10-26)

**PASS, no code fix.** Checked against June's raw listings (`charport_adis.py eve --phase CH1d`) and Ghidra decompiles; a Sonnet subagent compared the 27 March-body functions (Start / End, Z-ZZZZ / ZZZX FrameMoves, ZZZZ / ZZZX / X / XXfront* / XXdown* EventProcess, `32980580`, `32991a10`) with full strings read from the exe: all OK.
- **Macros**: `JUNE_EVE_ELSE_IF_STATE_CHANGE_ON_EX` matches `32969ea4-32969f08` (change on `stateChange < now` or `+0x2ac`, key window `keyInputStart <= now < keyInputEnd`); BWALK / WALK / WALK_CANNOT_ATTACKTIME / DASH match `3296a4cb-3296a5ac` (`time >= 0`, `now > time`, `bFootOnLine`, the keys). Sites not of this shape, all hand-written and right: XX flag 5 (`now > 0.55` / `now < 0.55`, `3296bdf3`), XXfrontX / XXdownX flag 0 (`< 0.57`, `> 0.26666` / `> 0.3`), ZZZdownZ's `> 0.55` walk-off.
- **Event flags**: Z (0/0), ZZ (0/0, 1/1 Nemesis, 2/2 Empress), ZZZ (2/2, 3/3, 0/0, 4/4, 1/1), XX (5, 4/4, 2/2, 1/1, 0/0, 3/3, 6/**5**), ZZZdownZ (0/0) match flag order, `m_fEventTime` index (`+0x8c10 + 0xc*i`), keys and states. `CancelComboWhenChangedDirection` counts match June's clears (ZZZ dword = 4, XX dword + word = 6, X 2). Class forms match every site: templet `+0xaf0 -> +4` in ZZZ flag 3, all of XX, ZZZdownZ StartFuture / Start / FrameMove; unit data `+0x38` in ZZ flags 1-2, ZZZ flag 4, ZZZdownZ EventProcess.
- **FrameMoves**: X / XX `FlushMp( 4 * MP rate )` (`+0x97b0` protected float x 4.0); `rand < +0x97bc` takes the BIG branch (`329afea0` falls into `334cd35c` `_BIG`); `GetPowerRate() * +0x97c0`; args `( landY, false, -1, +0x97c4, +0x97ac )` = March's `fReserveTime, fForceDownValueRate, fLifeTimeRate`; `AddEffectToSystem` before `SetInheritForceDownRate`; filtered names in Amplify / Spectrum / Induction / Fusion order. **XX's BIG set keeps the small Spectrum name** (`32984f6c` -> `334cd258`; X uses `334d1d70`): kept, **JUNE_BUGS 424** (new). XXX: `FlushMp( 4 )`, EG triangle at `GetPowerRate()`, other one scaled, `CreatePlane( +0x99a8, 2, 240, 130 )`, 0.4 / 0.43 as `if / else if`. ZZZdownZ: non-EG effect `( 0, false, -1, +0x97c4 )` x `+0x97c0`, EG pair `+0x95fc` / `+0x9600`. XXfrontXX / XXdownXX: identical bodies (JUNE_BUGS 423 holds); the `Dummy2_Lhand` read is unused in June too.
- **Members**: `+0x97ac` range, `+0x97b0` MP rate, `+0x97bc` big chance (written only by `3297fd5a`, skill ability 0x49), `+0x97c0` power, `+0x97c4` force-down; June ctor `3299cb15` stores the `fldz` left after `fld1` / `fstp +0x97c4`, so `= 0` is right. `0x9698` = `m_iSuccessiveHit_OneZ` (March's XX arm, `X2GuEve.cpp:10626`, sets it in the same place).
- **`329a1150` plane math**: centre `RotationAxis( axis, angle )` raw (`329a134f`), upper and lower `ToRadian( angle )` (`329a13b5`, the same value reused for lower); normals to `+0xbc` / `+0xc8` / `+0xd4`; planes `+0x8c` / `+0x9c` / `+0xac` from point `+0x78`; primary by sign of `+0x84` (> 0 upper, < 0 lower, else centre); rotate degrees centre / upper +24 / lower -24. Same as March's `SERV_EVE_BATTLE_SERAPH` form, so the transcription is exact.
- **Alpha tags**: 19 rows in `c10a_alpha_sites.tsv`, 20 `JuneTagEffectAlpha*` calls in the block (XXX's third Light and the triangle share one call site).
- **Enumerators**: `ENSI_ECN_COMBO_ZZX_READY` (June 231) / `ENSI_ECE_COMBO_ZZX` (234) accepted as new names; March's numbers differ from June's ids (ZZ 0xa6 in June), left to CH1m as for `ENSI_EVE_RECOVERY`.
- Side note (no action): June's `EveElectraSystem` keeps the effect list at `+0x17c` (March's layout has it at `+0x178`), so June has one extra field there; the inert code uses March's names.
- **Hygiene** (`--range ebdf701c^..ebdf701c`): `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.

No build (no code change). CH1e kickoff amended (reuse the `JUNE_EVE_*` macros; the dash X fire ball has the same BIG pick). Next: `/x2-phase char-eve CH1e` (Sonnet 5.5 high).

### CH1e - Done (2026-10-26)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (a fifth block at the end of `X2GuEve.cpp`, 1,182 lines; declarations at the end of the class). **All 34 class-map rows of phase CH1e** (35 minus `329cc500`, below) are new `CX2GUEve::June_*` members: 33 state functions (`June_ENSI_<STATE>_<Slot>`) and `June_StateChangeBoostAttackIfPossible` (`32978c60`); plus one helper the phase needed, `June_TryGetEquippedSkillAbilityValue` (`32007ce0`). Nothing registers or calls them. 37 funcmap rows `Char Eve CH1e` (34 + `32007ce0` PORTED, 2 "not ported here").

| State (`June_ENSI_` prefix) | June: Start / End / FrameMove / EventProcess |
|---|---|
| `DASH_COMBO_Z` | `3296cf50` / `32985b50` / `32985bb0` / `3296cf70` |
| `DASH_COMBO_X` | - / - / `329b0c10` / `3296e230` |
| `JUMP_COMBO_Z` `JUMP_COMBO_X` | `3296e6b0` `3296e700` / `329872a0` `329876e0` / `32987340` `32987780` / `3296e6d0` `3296e720` |
| `JUMP_COMBO_X_LANDING` | StartFuture `329662e0`, Start `32966300`, EventProcess `3296e760` |
| `DASH_JUMP_COMBO_Z` `_ZZ` `_ZZZ` | `3296e7c0` `3296e7e0` `3296e9d0` / `32987b20` `32987f60` `329883a0` / `32987bc0` `32988000` `32988440` / `32979b30` `3296e800` `3296e9f0` |
| `DASH_JUMP_COMBO_X` | `32978000` / `32978030` / `329b13f0` / `3296ea70` |

**What differs from March** (the rest is March's body; 25 of the 34 are March verbatim):
- **New flag arms** (the CH1d macros, event time 0 unless noted): DASH_COMBO_Z flag 0 (X by Architecture / Empress -> state 180); DASH_COMBO_X flag 1 (Empress X -> 236) and flag 2, time 1 (Battle Seraph X -> 237); DASH_JUMP_COMBO_Z flag 2 (EG / Nemesis X -> 228), DASH_JUMP_COMBO_ZZ flag 2 (-> 229), DASH_JUMP_COMBO_X flag 2 (Nemesis Z -> 230). Unit-data class form in those arms (and DASH_COMBO_X's two), templet form (null -> 0) everywhere else. Class switches: DASH_JUMP_COMBO_X sets flag 0 for 18 19 108 109 116, flag 1 for 19 109 116; DASH_COMBO_Z flag 1 for 18 108, flag 3 for 109, flag 4 for 26 116.
- **DASH_COMBO_Z FrameMove**: Empress takes `EVE_AT_DASH_COMBO_Z` (March `EVE_EP_...`) with power x `+0x97c0`, force down `+0x97c4`; the default case also covers Nasod; two alpha tags (7c).
- **DASH_COMBO_X FrameMove**: `FlushMp( 4 * m_fMPConsumeRateElectronBall )` (March 8); every class but EG / Nemesis picks the BIG ball (`m_fJuneElectronBallBigRate <= GetRandomFloat( 0 )` = small) and stores the effect pointer in new field `m_pJuneDashXFireBall` (+0x9c20); `m_pJuneDashXTargetUnit` (+0x9c24) = NULL at 0.1 s; one alpha tag. **DASH_JUMP_COMBO_X FrameMove**: same cost and BIG pick (range rate `+0x97ac` as lifetime), two alpha tags (the handle is stored before the tag). The BIG set here keeps the small Spectrum name: **JUNE_BUGS 425**.
- **`June_StateChangeBoostAttackIfPossible`**: June's rewrite. Cost 5, or `GetActualMPConsume( 25223 )` when the future state skill is Electronic Move; otherwise `m_iAddBoostAttack` (+0x9b5c), raised by Open Mind (25419, ability 0x80, `(int)( v + n )`), and `32007ce0( 0x1b, 25029, true )` replaces the 5 once `m_iBoostAttackCount >= max + 3`. States 239 / 240 / 241 (Electronic Move front / up / down) or 219 / 220 / 221; the turn, `pureDown` landing test and failed-MP landing are March's. `32979da0` (CH1j2) calls it.
- Alpha tags: 5 (`32985d93` `32985ed8` `329b0dbc` `329b15a4` `329b1765`), checklist 7c updated.

**New under the flag:** enumerators `ENSI_EEG_DASH_JUMP_ZX` 228, `_ZZX` 229, `ENSI_ECN_DASH_JUMP_XZ` 230, `ENSI_ECE_DASH_XX` 236, `ENSI_ECB_DASH_XX` 237, `ENSI_A_EBS_ELECTRONIC_MOVE_FRONT / _UP / _DOWN` 239 / 240 / 241 and `ENSI_JUNE_DASH_COMBO_ZX` 180 (March's `ENSI_DASH_COMBO_ZX` exists only under `EVE_COMBO_TEST_TEST`); fields `m_pJuneDashXFireBall` / `m_pJuneDashXTargetUnit`, both NULL in the constructor. `+0x9c24` is the Empress dash X's follow target (written by `3297b370`, CH1l4; read by `32999ce0`, CH1i).

**Class-map corrections.** `329cc500` has no caller in `32978c60` (the CH0 column was wrong): its only caller is `InitInpuDataProcess` `32978e80`; the row moved to CH1l (kickoff amended). `329cd320` (called for `EVE_PR_DASH_COMBO_Z`) is the compiler-made `CreateInstanceHandle` wrapper (CH1j's row): called through March's method, not transcribed. Kickoffs amended: CH1f CH1g CH1h CH1i CH1j2 CH1k CH1l CH1l4 CH3c.

**Checks.** A script compared the wide strings of every June listing with the string literals of its member (multiset, 34 of 34 equal); state ids, constants and the three jump tables (classes 18 19 108 109 116 / 18 108 / 26 116 / 109) were read from the raw listings; `32987340` and its four siblings (and the four End twins) are instruction-for-instruction the same shape after masking addresses and offsets.

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `cl /Zs` of `X2GuEve.cpp`: 0 errors. Range `94f2e1ea^..94f2e1ea`: same counts, `RESULT: PASS`.
Build: `DEPLOY OK: X2_offline.exe is 25,910,784 B, dated 2026-10-26 08:11:11, md5 26a071398ef653f7a9f7a72052e8793e, equal to X2/US_SERVICE/x2.exe (2026-10-26 08:11:04); built at main 6782ee63 + 2 uncommitted code file(s).`

**Open for the review**: the class form per new arm (templet vs unit data); `+0x9c20` as a raw `CEffect*` (June stores the pointer, not a handle); the nine enumerator values vs June's ids (CH1m); `GetActualMPConsume` called without June's third (bool) argument (March's signature, the base passes true). Nothing to play-test: the code is unreferenced. Review: `kickoffs/char-eve/CH1e-review.md`. Next: `/x2-phase char-eve CH1e review` (Opus 5.5 high), then CH1f.

### CH1e - Review (2026-10-26)

**PASS, no code fix.** Checked against June's raw listings (`charport_adis.py eve --phase CH1e`, 33 files) and Ghidra decompiles / disassembly of `3296cf70` `3296e230` `32979b30` `3296e800` `3296ea70` `32985bb0` `329b0c10` `329b13f0` `32978c60` `32007ce0`.
- **Event flags**: DASH_COMBO_Z flag 0 / time 0 (oneX and templet 19 / 109 -> 0xb4), then the cancel, flags 1 (-> 0xbf, cleared first), Z switch (18 108 -> 1, 109 -> 3, 26 116 -> 4), 2 (-> 0xc4), X Nemesis (templet 108), 3 (-> 0xc9), 4 (-> 0xd1), C skip > 0.7. DASH_COMBO_X flag 1 / time 0 (unit data 109, oneX -> 0xec), flag 2 / time 1 (`+0x8c1c..`, unit data 116 -> 0xed), flag 0 (-> 0xc1, StateChange then clear), Z templet 18 / 108, ZXC skip > 0.43. DASH_JUMP_COMBO_Z / ZZ flag 2 (unit data 18 / 108, -> 0xe4 / 0xe5); flag 0 not cleared on the ZZ / ZZZ change; flag 1 (Empress 0xcb; Battle Seraph zeroes `+0x9b56` (short `m_iBoostAttackCount`) and `+0x9b58`, calls `32978c60`), cleared after. DASH_JUMP_COMBO_X: the two pre-switches (18 19 108 109 116 / 19 109 116), flag 2 (unit data 108, oneZ -> 0xe6), flag 1 (19 / 109 -> 0xc3, 116 -> 0xda, not cleared). All class forms as the Done note states.
- **DASH_COMBO_Z FrameMove**: waves +-15 deg yaw (the stored y is the adjusted one, `32985d64` EDI), +-90 / -+60 offsets, tag inside the null check after AddRotate / AxisAngle; Empress / Architecture `EVE_AT_DASH_COMBO_Z` x `+0x97c0`, force-down `+0x97c4`; Electra / Battle Seraph `329cd320` -> `+0x99cc` and skips the main-handle store; default covers Nasod.
- **Dash X / dash-jump X**: `FlushMp( 4.0 x protected +0x97b0, 0 )`; `(double)+0x97bc <= GetRandomFloat( 0 )` = small; args `( landY, false, -1, +0x97c4, 1.0 )` in the dash X, `+0x97ac` in the dash-jump X; `+0x9c20` = the `CEffect*` itself, stored before `32980580`, dash X only; `+0x9c24 = 0` at 0.1 s, after an unguarded tag; EG / Nemesis (templet) ball at `x +0x97c0`, no system add. Dash X BIG set uses its own `_BIG` Spectrum (`334d20a0`), dash-jump X BIG keeps `334cd258`: **JUNE_BUGS 425 holds**. Dash-jump X handles stored before the tag; timers 0.01 / 0.266 / 0.1 as `if / else if`; bone blend R x 0.8 + L x 0.2; not-enough-MP pos and `vDegree.z`.
- **`32978c60`**: 5.0, or vslot 424 (`0x6a0`) `( 25223, level, 1 )`; March's 2-argument `GetActualMPConsume` is that slot called with true (`X2GUUser.cpp:25805` forwards to `GetActualMPConsumeJune( .., true )`), so the open question is settled. Open Mind: `fiadd` of `+0x9b5c` then `_ftol2`; `movsx` count `>= max + 3` -> `32007ce0( 0x1b, 25029, 1, &cost )`, whose fail leaves 5.0 (`JuneTryGetSkillAbilityValue` leaves `fOut_` untouched). FlushMp fail -> landing if on a line; turn; up (not `+0x9b58`) 240 / 220, down (`land.y + LINE_RADIUS < pos.y`) 241 / 221, else 239 / 219, the skill id re-read each time. `June_TryGetEquippedSkillAbilityValue` = `32007ce0` (IsPvpOrPve passed false, the pilot's 32007c40 precedent).
- **Side notes**: `32007d80` is `CX2GUUser::CreateNotEnoughMPEffect`; the members call Eve's March copy, same body. Ghidra has no function object at `32978e80`.
- **Class map**: `329cc500`'s 29 call sites all lie in `32978f41-329792b0`, inside InitInpuDataProcess `32978e80` (next function `32979350`); `32978c60` ends at `32978e74` with no call. The move to CH1l is right.
- **Enumerators**: the nine stay as new names; CH1m maps March values to June ids. `CH1f.md` has the "Amended by CH1e" section; nothing more to change.
- **Hygiene** (`--range 94f2e1ea^..94f2e1ea`): `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.

No build (no code change). Next: `/x2-phase char-eve CH1f` (Sonnet 5.5 high).

### CH1f - Done (2026-10-26)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (a sixth block at the end of `X2GuEve.cpp`, 603 lines; 26 declaration lines at the end of the class). **All 25 class-map rows of phase CH1f** are new `CX2GUEve::June_*` members (`June_ENSI_<STATE>_<Slot>`). Nothing registers or calls them. No new enumerator, field or helper: `ENSI_EEG_DASH_JUMP_ZX` / `_ZZX` came with CH1e, the two callees in the class range (`32976e90`, `32970880`) are CH1a's. 25 funcmap rows `Char Eve CH1f`.

| State (`June_ENSI_` prefix) | June: Init / StartFuture / Start / End / FrameMove / EventProcess |
|---|---|
| `COMBO_ZZZdownZZ` | `3296b690` / `3296b6c0` / `3296b7a0` / `3296b880` / `329847f0` / `3296b8a0` |
| `DASH_COMBO_ZZ` | `3296d440` / `3296d470` / `3296d550` / `329662b0` / `32986440` / `3296d630` |
| `DASH_COMBO_ZZZ` | - / `3296d8b0` / `3296d990` / `3296da80` / `32986710` / `3296daa0` |
| `DASH_COMBO_XZ` | FrameMove `32986f00`, EventProcess `3296e580` |
| `EEG_DASH_JUMP_ZX` / `_ZZX` (June-only, ids 228 / 229) | Init `32968510` / `32968570`; FrameMove `32968560` / `329685c0`; EventProcess `329795a0` / `329797d0` |

**What differs from March** (the rest is March's body, with the class read through the unit templet, `+0xaf0 -> +4`, where March read the unit class):
- **Architecture / Empress power factor**: `EVE_AT_COMBO_ZZZdownZZ` (`329847f0`), `EVE_AT_DASH_COMBO_ZZ` (`32986440`) and `EVE_AT_DASH_COMBO_ZZZ` (`32986710`) are created with `GetPowerRate() * GetStatAtkRateOfSummoned() * m_fAttackPowerRateElectronBall` (`+0x4064`, `+0x97c0`), force-down `m_fForceDownValueRateElectronBall` (`+0x97c4`), range 1.0, land position 0 (March: plain `GetPowerRate()`). The Exotic Gear / Nemesis effects keep `GetPowerRate()`.
- **`329847f0` Exotic Gear half**: March's body plus two alpha tags (7c: `32984a5b`, `32984c26`), each inside the null check after `SetLandPosition`. March's `--vAngle.z += -30.f` (a 31 degree drop) is in June too.
- **EventProcess `3296d630`** (dash Z-Z): March's `SKILL_CANCEL_BY_HYPER_MODE` branch, i.e. `SKILL_CANCEL_AFTER( 0.01 )` with the hyper-mode arm. **`3296daa0`** (dash Z-Z-Z): March's `BALANCE_CODE_NEMESIS_20121213` branch (EG / Nemesis: animation end, `ELSE_IF_ZX_PRESSED_AFTER`, skill cancel, then BWALK / WALK / DASH) with June's on-line tests (the CH1d `JUNE_EVE_*_CANCEL_AFTER` macros); every other class: animation end only.
- **June-only `329795a0` / `329797d0`** (identical bodies, instruction for instruction): animation end -> `June_StateChangeJumpDashIfPossible` while `m_iDashJumpCount < June_GetMaxDashJumpCount()`, else `USI_JUMP_DOWN`; on the line -> `USI_JUMP_LANDING`; then the skill / BWALK / WALK / DASH cancels. Their Init preloads `Arme_Critical.dds`, `Arme_Damage_Center.dds`, `Arme_Damage_Ring.dds`, `ENS_QueensSpearMesh01.x`, `star2.dds`; FrameMove is `CommonFrameMove`.

**Method notes.** Ghidra has no function object for `32986710` `3296daa0` `3296e580` `329795a0` `329797d0` (and `charport_adis.py` runs past their end): read the listing up to the next class-map address. A `[esp+N]` slot read after the `vslot 0x204` call (`GetPowerRate`) is a value stored before the argument pushes: here the `+0x4064` copy. Jump tables read with a script (`classes 18 108` / `19 109`; `18 19 108 109`; `19` alone for the ZZ Init).

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Build: `DEPLOY OK: X2_offline.exe is 25,922,048 B, dated 2026-10-26 08:46:31, md5 897f267ad2e92002b5571c22597b19bb, equal to X2/US_SERVICE/x2.exe (2026-10-26 08:46:23); built at main 752019f2 + 2 uncommitted code file(s).`

**JUNE_BUGS:** no row (nothing in the 25 looks unintended; `--vAngle.z` is March's). **Drift:** none. **Open for the review**: `+0x4064` read as `GetStatAtkRateOfSummoned()` (the Aisha CH3c row calls it user `+0x4064`); the templet-vs-unit class read per site (all 25 read the templet). Nothing to play-test: the code is unreferenced. Review: `kickoffs/char-eve/CH1f-review.md`. Next: `/x2-phase char-eve CH1f review` (Opus 5.5 high), then CH1g.

### CH1f - Review (2026-10-26)

**PASS, no code fix.** Checked against June's raw listings (`charport_adis.py eve --phase CH1f`, 25 files, each read to the next class-map address) and Ghidra decompiles of `329847f0` `32986440` `32986f00` `3297f290`.
- **`+0x4064` is `m_SkillRelatedData.m_fStatAtkRateOfSummoned`**: June's `InitPassiveSkillState` (`3297f290`) sets it to 1.0 right after the base call and multiplies it by SA `0x12` (= `SA_SUMMON_ATK_REL`, counted in `X2SkillTree.h`) of skill `0x633e`, as March's `X2GuEve.cpp:3530` / `:3618` do. So `GetStatAtkRateOfSummoned()` is right.
- **Power factor**: in `329847f0` and `32986710` the `fld [esi+0x4064]` is stored at E0+0x10 (E0 = esp after the four register pushes) and the `fmul [esp+0x60]` after the 0x50 bytes of argument pushes reads E0+0x10; `32986440` decompiles to `param_1[0x1019]` directly. Then `x +0x97c0`; tail `( 0, false, -1, +0x97c4, 1.0, -1, -1, 0, -1, false, false )` = the port's args plus March's defaults.
- **`329847f0` Exotic Gear half**: z `- 1.0 - 30.0` (`--vAngle.z += -30`), then y `+ 180`, z `- 30`; AddRotate / AxisAngle (`+0x3e0` / `+0x404`), land y (`+0x320`), tag `32296d60` inside the null check (`32984a5b`, `32984c26`); `UpDownCrashCameraNoReset( 5.0, 0.15 )` between; both `EVE_EG_ZZZdownZZ` with `GetPowerRate()` and the default tail.
- **`32986710`**: class switch first; EG / Nemesis at 0.01 `EVE_EG_DASH_COMBO_ZZZ01`-`04` in order (`334cd640` `334cd610` `334cd5e0` `334cd5b0`); Architecture / Empress at 0.23, `Dummy1_Rhand`, handle `+0x1e4` -> `+0x95f8`; then future time > 0.2 and handle != -1 -> SetPos( `Dummy1_Rhand` ) or handle = -1. `32986f00`: XZ01-04 in order.
- **Class form**: every test reads the templet (`+0x867c -> +0xaf0 -> +4`, NULL -> 0). Jump tables decoded from the binary: `18 108` / `19 109` in the Start / StartFuture / FrameMoves; `18 19 108 109` for the 0.45 skip; ZZ Init `19` alone, ZZZdownZZ Init `19 109` (compares). `3296daa0`'s skip table is `19 108` -> 0.75 and `18 109` -> 1.0, which is March's own pairing (`X2GuEve.cpp`, `ENSI_DASH_COMBO_ZZZ_EventProcess`), ported as is.
- **EventProcess chains**: `3296d630` = SKILL_CANCEL_AFTER( 0.01 ) (both arms, `+0x8d4c` set), flag 0 and time > 0.53 -> 0xc0 for 18 / 108 then cleared, oneZ and time < 0.53 sets it, skip C (no Z / X / LCtr), 0.8 / 0.6, `m_fCanNotAttackTime` 0.1. `3296daa0`: skip ZXCAS (LCtr last), 1.5 / 0.75 / 1.0; 18 / 108 then animation end, ZX `+0x8c8c`, skill `+0x8c90`, BWALK `+0x8c94`, WALK `+0x8c98`, DASH `+0x8c9c` (on-line tests as the CH1d macros); others animation end. `3296e580`: ZXC, 1.2 / 0.935. `3296b8a0`: arrows only, 0.7 / 0.45, no can-not-attack time.
- **`329795a0` = `329797d0`**: 167 instructions equal over `0x22e` bytes. The skill arm's `SpecialAttackEventProcess` half is folded away by the compiler (it sits in the not-on-line else, so `IsOnSomethingFuture()` is false): `SKILL_CANCEL_AFTER` is equivalent. Init strings match by address (`334ca1e4` `334ca1b4` `334ca188` `334cc730` `334ca060`).
- **JUNE_BUGS**: none needed; the mixed 0.75 / 1.0 pairing is March's.
- **Hygiene** (`--range 96f05392^..96f05392`): `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.

No build (no code change). `CH1g.md` still matches (CH1f added no enumerator). Next: `/x2-phase char-eve CH1g` (Sonnet 5.5 high).

### CH1g - Done (2026-10-26)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (a seventh block at the end of `X2GuEve.cpp`, 450 lines; 24 declaration lines at the end of the class). **All 23 class-map rows of phase CH1g** are new `CX2GUEve::June_*` members (`June_ENSI_<STATE>_<Slot>`). Nothing registers or calls them. No new field or helper: every callee is an engine function called through its March counterpart (`31d16ef0` / `31d16f90` / `31d170a0` = Texture / XSkinMesh / XMesh ReadyInBackground, `31d28e10` EventTimer, `31f8ff00` CreateInstance, `322731a0` / `32070cf0` / `32070d60` inside the CH1d macros); no class-range callee, no alpha tag (7c), no BFI call (7ai), no `m_fNowStatePowerRate` (7i). 23 funcmap rows `Char Eve CH1g`.

| State (`June_ENSI_` prefix) | June: Init / Start / FrameMove / EventProcess |
|---|---|
| `NS_DASH_COMBO_ZX` | `32966ea0` / `32966ec0` / `3298db90` / `329709a0` |
| `NS_DASH_COMBO_ZXX` | `32966ee0` / - / `3298dd30` / `32970b50` |
| `NS_COMBO_XXZ` | - / - / `3298de80` / `32970c30` |
| `NS_COMBO_XXZZ8` `NS_COMBO_XXZZ8Z` | - / - / `3298e090` `3298e710` / `32970f10` `32971240` |
| `ECN_DASH_JUMP_XZ` (June-only, id 230) | `329685d0` / - / `32968640` / `32975bc0` |
| `ECN_COMBO_ZZX_READY` / `_LOOP` / `_END` (June-only, ids 231 / 232 / 233) | `32968650` / - / `329686f0` `32968700` `32968710` / `32975e20` `32975e60` `32975eb0` |

**What differs from March** (10 of the 23 are March's body verbatim, 3 more are March's plus the chain below; the Inits' strings and every timer, offset and effect name were read from the listings):
- **FrameMoves and the dash Z-X / Z-X-X EventProcesses = March.** Spear effects at 0.27 / 0.1 / 0.001 (two spears) / 0.26 0.40 0.53 0.66 0.8 0.93 / 0.36, y -50 and x -/+ dir x 70 (dash Z-X), x -/+ 40 (X-X-Z-Z), Queen's Throne power rate, default CreateInstance tail (a masked block diff of the six XXZZ8 calls: identical but for register allocation).
- **June's expanded cancel chain** after March's body in `32970c30` (XXZ: after the animation-end WAIT), `32970f10` (XXZZ8: after the 1.05 / animation-end change to XXZZ8Z), `32971240` (XXZZ8Z: `SKILL_CANCEL_AFTER( 0.46 )` first, then the four, then the animation-end WAIT), `32975bc0` and `32975eb0`: skill (`+0x8c90`), BWALK (`+0x8c94`), WALK (`+0x8c98`), DASH (`+0x8c9c`) with the on-line test, as the CH1d macros. March had these four only under `BALANCE_CODE_NEMESIS_20121213` (XXZZ8, XXZZ8Z) and had none in XXZ.
- **June-only states.** Dash-jump X-Z (`32975bc0`): animation end -> `USI_JUMP_LANDING` when on a line (nested; off a line the chain is skipped), then the four cancels; Init preloads `Arme_Critical` / `_Damage_Center` / `_Damage_Ring`, `EG_DashComboZ3_Mesh01-04.X`, `star2.dds`. Z-Z-X ready (animation end -> loop 232, else off line -> JUMP_DOWN), loop (animation end, or X released -> end 233, else off line -> JUMP_DOWN), end (animation end -> WAIT, then the four); the three FrameMoves are `CommonFrameMove`; ready's Init preloads 12 effect assets (`ECN_ZZX_Finish_Drill.X`, `SI_SA_EEG_JUNK_BREAK_Effect_Drill_Main.X`, `EVE_CircleP01` / `02.Y`, ...).
- The `+0x8e90` second-body anim clock inside June's cancel arms and `ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER` is checklist 7o, not ported (as CH1d-CH1f); one comment line says so.

**New under the flag:** enumerators `ENSI_ECN_COMBO_ZZX_LOOP` 232, `_END` 233 (after `_READY`, which CH1d added; `ENSI_ECN_DASH_JUMP_XZ` came with CH1e). CH1m maps March values to June ids.

**Checks.** The 33 wide strings of the 23 listings equal the 33 literals of the block (multiset). Branch directions of every x87 compare were read with the `fcom` / `fcomp` / `fcompp` idiom (`test ah, 5` / `0x41`, `jp` / `jnp`) and the OneZ ladder in `32970f10` re-derived arm by arm. No JUNE_BUGS row (nothing looks unintended).

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Build: `DEPLOY OK: X2_offline.exe is 25,930,752 B, dated 2026-10-26 09:17:18, md5 729c12d112b9b6489be07e3a87c80533, equal to X2/US_SERVICE/x2.exe (2026-10-26 09:17:11); built at main 5a0048c0 + 2 uncommitted code file(s).`

**Drift:** none. **Open for the review**: the animation-end position relative to the chain per function (XXZZ8Z has it after, XXZ / ZZX end before); the nested animation-end / on-line `if` in `32975bc0`; whether the Nemesis dash-jump X-Z and Z-Z-X ready preload lists need anything beyond a string compare. Nothing to play-test: the code is unreferenced. Review: `kickoffs/char-eve/CH1g-review.md`. Next: `/x2-phase char-eve CH1g review` (Opus 5.5 high), then CH1h.

### CH1g - Review (2026-10-26)

**PASS, no code fix.** Checked against June's raw listings (`charport_adis.py eve --phase CH1g`, 23 files) and March's `X2GuEve.cpp:20796-21140`.
- **"= March" holds** for the ten: Inits `32966ea0` / `32966ee0` (XSkinMesh `31d16f90`), Start `32966ec0` (CommonStateStart, `++m_iDashJumpCount` `+0x95cc`), the five FrameMoves (Queen's Throne `+0x96b4` / `+0x96b8`; 0.27 with y -50 and x `-`/`+` dir.x x 70; 0.1; 0.001 two spears; 0.26 0.40 0.53 0.66 0.8 0.93 with x `-`/`+` 40; 0.36 with the power rate read inside the timer), `329709a0` (ASUT push 2 = `ASUT_AIR`, flag `+0x7d85`, 0.46, speed 900 / 0, the 14-key ZXASDC list) and `32970b50` (bIsRight toggle -> landing, ZXC list, 0.56, `m_fCanNotAttackTime` `+0xde0` 0.1). All eleven CreateInstance calls carry the same default tail (masked push / fld sequence equal).
- **Strings**: the 33 push addresses read in full equal the block's 33 literals, in order; Texture / XSkinMesh / XMesh (`31d16ef0` / `31d16f90` / `31d170a0`) match each preload call.
- **Cancel chains**: the skill / BWALK / WALK / DASH arms of `32970c30` `32970f10` `32971240` `32975bc0` `32975eb0` have the same test signature (`+0x8c90..+0x8c9c`, on-line, key pairs, hyper arm with `+0x8d4c`); the only differences are where the compiler places the shared StateChange( WAIT ) body. Animation end before the chain in XXZ and ZZX end, after it as a separate `if` reached on every path in XXZZ8Z (`3297156d`), nested in dash-jump X-Z (off a line the chain is skipped). XXZZ8: the 1.05 / animation-end block is the chain's head (`jnp` / `je` -> `3297120d`), and the OneZ ladder (< 1 and > 0.33; >= 1 and < 0.33 -> 1; < 2 and > 0.6) re-derived from the `fcom` flags. XXZZ8Z's `SKILL_CANCEL_AFTER( 0.46 )` comes first.
- **June-only states**: `32975e20` end -> `0xe8`, else off line -> JUMP_DOWN; `32975e60` end or `!pureX` (`+0x86ee`) -> `0xe9`, else off line -> JUMP_DOWN; all four FrameMoves are a tail jump to CommonFrameMove (vslot 0x7cc).
- **`+0x8e90`** second-body clock (7o) appears in every chain arm and in ZXASDC, as the block's comment says.
- **Enumerators** `ENSI_ECN_COMBO_ZZX_LOOP` / `_END` stay new names; CH1m maps them to 232 / 233. **JUNE_BUGS**: none needed.
- **Hygiene** (`--range 96ba8482^..96ba8482`): `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.

No build (no code change). `CH1h.md` still matches (CH1g added nothing it reads). Next: `/x2-phase char-eve CH1h` (Sonnet 5.5 high).

### CH1h - Done (2026-10-26)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (an eighth block at the end of `X2GuEve.cpp`, 219 lines; 12 declaration lines at the end of the class). **All 11 class-map rows of phase CH1h** are new `CX2GUEve::June_*` members. Nothing registers or calls them. No new enumerator, field or helper (`ENSI_JUNE_DASH_COMBO_ZX` came with CH1e); every callee is an engine function called through its March counterpart. 11 funcmap rows `Char Eve CH1h`.

| State (`June_` prefix) | June: Init / Start / End / FrameMove / EventProcess |
|---|---|
| `ENSI_COMBO_XXZ4` | `3296c580` / - / - / `32985300` / `3296c5d0` |
| `ENSI_DASH_COMBO_ZX` (June-only, id 180) | - / `329662c0` / `329662d0` / `32986d30` / `3296df00` |
| `ENSI_DASH_JUMP_COMBO_XZ` | `32966320` / - / `32966350` / `329889d0` / `3296ecc0` |

**What differs from March** (Inits, both Ends, the dash Z-X Start and dash-jump X-Z EventProcess are March's body; the class is read through the unit templet, `+0xaf0 -> +4`):
- **Power factor** (as CH1f): `EVE_AT_COMBO_XXZ4_START` (`32985300`) and `EVE_AT_DASH_JUMP_COMBO_XZ` (`329889d0`) take `GetPowerRate() * GetStatAtkRateOfSummoned() * m_fAttackPowerRateElectronBall`, force-down `+0x97c4`, land 0, range 1.0, the rest of the tail is `CreateInstance`'s default.
- **7c alpha tags, 5 sites**: XXZ4 mesh (`329854ac`) and 1.07 particle (`329856eb`); dash-jump X-Z guardian mesh (`32988afd`), particle (`32988bdd`, after `SetLandPosition`), blade-wave mesh (`32988e18`). The XXZ4 1.07 particle has no `SetLandPosition`, as March.
- **XXZ4 EventProcess**: off line -> JUMP_DOWN and `y -= 45`; a separate animation-end `if`; then skill / BWALK / WALK / DASH as `else if` arms (CH1d macros).
- **Dash Z-X (June-only)**: FrameMove plays `EffectSet_EVE_EAT_DashComboZX` (0.01) at the factored power rate. EventProcess: off line -> JUMP_DOWN (chain skipped), the four cancels, then two arms on `m_fAttackCancelAfter` (`+0x8c8c`): pureZ -> `GetComboZStateID()` (vslot `0x4b0`), pureX -> `GetComboXStateID()` (`0x4b4`), both with the on-line test March's `ZX_CANCEL_AFTER` lacks; the animation-end `if` comes after the chain on every path.
- `+0x8e90` second-body clock in the cancel arms is checklist 7o, not ported (as CH1d-CH1g).

**Method note.** `charport_adis.py` cut wide strings at 12 characters (`Eve_AT_DashJ` is `Eve_AT_DashJumpComboXa_Mesh_G.X`); CH1h fixed it to read 200 bytes. The 13 literals of the block equal the 13 push addresses read in full.

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Build: `DEPLOY OK: X2_offline.exe is 25,935,360 B, dated 2026-10-26 09:43:07, md5 4602ab1452b3f1216ebd53cf33126643, equal to X2/US_SERVICE/x2.exe (2026-10-26 09:43:00); built at main d1c54066 + 2 uncommitted code file(s).`

**JUNE_BUGS:** no row. **Drift:** June's `PlayEffectSet` (`321e1e60`) takes one trailing float (default 1.0, stored in the instance at `+0x14c`, it reaches the set's damage effects' force-down slot); March's `PlayEffectSet` has none, and nothing in `X2EffectSet.*` ports it. Dash Z-X passes `m_fForceDownValueRateElectronBall` there; the member calls March's signature without it (one comment line). Owner: CH1o ("gaps the CH1a-CH1l reviews routed"); it only changes force-down on that set's hits when the Electron Ball rate is not 1. **Open for the review**: the gap above; the ZX arm split (two `else if` with the on-line test). Nothing to play-test: the code is unreferenced. Review: `kickoffs/char-eve/CH1h-review.md`. Next: `/x2-phase char-eve CH1h review` (Opus 5.5 high), then CH1i.

### CH1h - Review (2026-10-26)

**PASS, no code fix** (Opus 5.5 high; commit `a55b61e4`, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`). All 11 members read against `charport_adis.py eve --phase CH1h`.
- **"= March" bodies:** `32966320` (two `XSkinMeshReadyInBackground`: `Eve_AT_DashJumpComboXa_Mesh_G.X`, `Eve_Guardian_BladeWave01.X`), `3296c580` (templet class 0x13 / 0x6d, null templet -> return; `Eve_AT_ComboX5a_mesh.X`, `Eve_Guardian_BladeWave01.X`, `DummyAttackBox_50x50x50.X`), `3296ecc0` (on line -> JUMP_LANDING 0x12, CommonEventProcess), and the tail jumps `329662c0` (`0x7c4` CommonStateStart), `329662d0` / `32966350` (`0x7dc` CommonStateEnd) match.
- **XXZ4 FrameMove `32985300`:** damage pos `y +125` then `+/- GetDirVector() x 60` (whole vector); mesh at `GetPos()`; `CreateInstance` arguments decoded slot by slot: power `GetPowerRate() x +0x4064 x +0x97c0`, land 0, reserve false, reserve time -1, force-down `+0x97c4`, life 1.0, then -1 / -1.0 / 0 / -1.0 / 0 / 0 = March's defaults. 1.07 particle at `x +/- 60`, `y +125`; AddRotate (`+0x3e0`), AxisAngle (`+0x404`), tag `329856eb`, no SetLandPosition. Mesh tag `329854ac`. 0.01 / 1.07 are an `else if` pair.
- **XXZ4 EventProcess `3296c5d0`:** off line -> JUMP_DOWN, `y -= 45` with no skip; a separate animation-end `if` (-> WAIT, jumps to CommonEventProcess); then skill (both arms, `+0x8d4c`) / BWALK / WALK / DASH as its `else if` arms, as the CH1d macros.
- **Dash Z-X FrameMove `32986d30`:** `PlayEffectSet( name, this, NULL, false, power, -1, (1,1,1), false, 0, 0, 0, +0x97c4 )`; every argument but the last is March's default. **EventProcess `3296df00`:** off line -> JUMP_DOWN and straight to the animation-end test; the four cancels; then `+0x8c8c >= 0 && t > +0x8c8c && on line && pureZ` -> vslot `0x4b0`, the same with pureX -> `0x4b4`; animation end after the chain on every path. Matches.
- **Dash-jump X-Z FrameMove `329889d0`:** the class switch wraps only the 0.01 block (a non-Architecture class falls to CommonFrameMove); guardian mesh at `GetPos()`, particle at `y +290` with SetLandPosition (vslot `0x554` `.y` -> `+0x320`); 0.1: `+0x969c` = March's `m_vSummonedUnitBonePos`, `y +150`, same damage tail as XXZ4; 0.15 blade wave at that position. Tags `32988afd`, `32988bdd`, `32988e18`. The 0.1 timer compares as a double of `0.1f` (no semantic change).
- **Drift decision (PlayEffectSet trailing float):** port it, before `_LIVE`, in CH1o (already routed). It is June's `321e1e60` signature and reaches force-down on a path the player hits once Eve is live (scope rule: combat numbers), so it stays in this plan. Own flag, since it is an engine change in `X2EffectSet.*`, not Eve core; the CH1o kickoff now says so.
- **JUNE_BUGS:** none found.
- Hygiene: `flagcheck.py JUNE_CHAR_EVE_CORE --range a55b61e4^..a55b61e4`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. No rebuild (no code change). Nothing to play-test. Next: `/x2-phase char-eve CH1i` (Sonnet 5.5 high).

### CH1i - Done (2026-10-26)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (a ninth block at the end of `X2GuEve.cpp`, 414 lines; 22 declarations at the end of the class). **All 22 class-map rows of phase CH1i** are new `CX2GUEve::June_*` members; nothing registers or calls them. New: the enumerator `ENSI_ECE_COMBO_ZZXX` (script id 235, after `ENSI_ECE_COMBO_ZZX`); no field or helper. 22 funcmap rows `Char Eve CH1i`.

| State (`June_` prefix) | June: Init / End / FrameMove / EventProcess |
|---|---|
| `ENSI_EP_DASH_COMBO_ZZ` | `32966f00` / - / `3298e860` / `329715a0` |
| `ENSI_EP_DASH_COMBO_ZZZ` | `32966f30` / - / `3298ea30` / `32971700` |
| `ENSI_EP_JUMP_COMBO_ZX` | `32966f60` / - / `3298ec00` / `329717f0` |
| `ENSI_EP_COMBO_XXdownX` | `32966fa0` / - / `3298eff0` / `32971820` |
| `ENSI_ECE_COMBO_ZZX` (June-only, 234) | `32968720` / - / `329994e0` / `32976110` |
| `ENSI_ECE_COMBO_ZZXX` (June-only, 235) | `32968750` / - / `329998f0` / `32976440` |
| `ENSI_ECE_DASH_XX` (June-only, 236) | `32968790` / `329687c0` / `32999ce0` / `32976480` |

**What differs from March** (the four pair Inits and EventProcess bodies are March's; checked op for op, key lists ZZ = ZXCAS, ZZZ / XXdownX = ZXC):
- **Pair FrameMoves** (March: `CommonFrameMove` only): dash Z-Z, Z-Z-Z and X-X-down-X each play one `EffectSet_Eve_Code_Empress_*` at 0.01 with `GetPowerRate() x GetStatAtkRateOfSummoned() x m_fAttackPowerRateElectronBall` (listings identical to CH1h's dash Z-X apart from the name length).
- **Jump Z-X `3298ec00` / Z-Z-X-X `329998f0`** (same body, 0.06 s / 3 MP and 0.4 s / 4 MP): `FlushMp( n )` (June's second argument is the constant 0); success -> `EffectSet_Eve_Code_Empress_JumpZX` then `_JumpZX_Grenade`; failure -> `CreateNotEnoughMPEffect` (`32007d80`) at the mid-point of `Dummy1_Rhand` / `Dummy2_Lhand`, angles 0, 0, rotate z (stack offsets re-derived).
- **Z-Z-X `329994e0`** is op-for-op `32985300` (XXZ4). **Its EventProcess**: animation end -> WAIT on line / JUMP_DOWN off line, flag 0 on `m_fEventTime[0]` with class 109 (unit data `+0x38`) and `oneX` -> `ENSI_ECE_COMBO_ZZXX`, then the SKILL / BWALK / WALK / DASH cancels. Z-Z-X-X EventProcess is the animation-end step only; dash X-X EventProcess is that step plus the four cancels.
- **Dash X-X `32999ce0`** = dash-jump X-Z effects placed on `m_pJuneDashXTargetUnit` (`+0x9c24`), or on Eve without one: 0.01 mesh at y +200 and particle at y +200 +290 (June adds 290 to the slot it already raised), `SetLandPosition` from Eve; 0.1 `m_vSummonedUnitBonePos` = target pos, y +350; 0.15 blade wave mesh. End clears `+0x9c24`.
- **7c alpha tags, 5 sites**: `3299968c` / `329998cb` (Z-Z-X), `32999dda` / `32999ed4` / `3299a119` (dash X-X).
- `+0x8e90` second-body clock in the cancel arms is checklist 7o, not ported (as CH1d-CH1h).

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Build: `DEPLOY OK: X2_offline.exe is 25,945,600 B, dated 2026-10-26 10:09:25, md5 0634660931152d1e46dbe31978d59513, equal to X2/US_SERVICE/x2.exe (2026-10-26 10:09:18); built at main 39853ac4 + 2 uncommitted code file(s).`

**JUNE_BUGS:** no row. **Drift:** none new; the `PlayEffectSet` trailing force-down float (now also on four more sets) stays with CH1o. **Open for the review**: the particle y +490 on dash X-X (kept as the listing reads; decide whether it is a row); the ZZX / ZZXX flag arm is one `JUNE_EVE_ELSE_IF_STATE_CHANGE_ON_EX` with the class test first. Nothing to play-test: the code is unreferenced. Review: `kickoffs/char-eve/CH1i-review.md`. Next: `/x2-phase char-eve CH1i review` (Opus 5.5 high), then CH1j.

### CH1i - Review (2026-10-26)

**PASS, no code fix** (Opus 5.5 high; commit `cb1f7bca`, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`). All 22 members read against `charport_adis.py eve --phase CH1i`.
- **"= March" pairs:** the four Inits match string for string (`32966f00` BladeWave01 / DashComboZ3_Mesh01 / ColorBallGray; `32966f30` two meshes; `32966f60` two meshes + `Wind_Liner02.Y` + ColorBallGray; `32966fa0` three meshes + four textures). `329715a0`: off line -> JUMP_DOWN, `y -= 45` and straight to the animation-end test; else flag 0 and `t > 0.46` -> ZZZ (flag cleared); else `oneZ` and `t < 0.466667` -> flag set; animation end -> WAIT, else the ZXCAS list (15 keys, LCtr last) and `t > 0.6` -> `+0xde0 = 0.1`, WAIT. `32971700` / `32971820`: the same without the flag arm, ZXC list (14 keys), 0.666667. `329717f0`: on line -> `+0xde0 = 0.1`, JUMP_LANDING. All match.
- **Effect-set FrameMoves** `3298e860` / `3298ea30` / `3298eff0`: normalised diff against CH1h's `32986d30` differs only in the name and its length push, so the CH1h argument decode holds (trailing `+0x97c4` -> CH1o).
- **Jump Z-X `3298ec00` / Z-Z-X-X `329998f0`:** the two listings differ only in 0.06 / 0.4 and 3.0 / 4.0 (no MP-rate factor). `FlushMp` is vslot `0xe4` `( n, 0 )`, arg 2 unused (funcmap row `3208dc70`). Success: JumpZX, then JumpZX_Grenade, same argument shape as the effect-set FrameMoves. Failure stack re-derived: Rhand out at `S+0x70`, Lhand at `S+0x64`, summed per axis, x 0.5, then `( mid, 0, 0, rotate.z )` into `32007d80` = `CreateNotEnoughMPEffect( pos, x, y, z )`.
- **Z-Z-X `329994e0`:** normalised diff against `32985300` is empty (op for op XXZ4), and the port body is CH1h's XXZ4 body verbatim. **EventProcess `32976110`:** animation end -> WAIT on line / JUMP_DOWN off line; flag 0 with `t > +0x8c18` or animation end -> `0xeb` ZZXX, flag cleared; else `t < +0x8c14`, `t >= +0x8c10`, GetUnit() non-null, unit `+0x38 == 0x6d` (109), `oneX` -> flag set; then SKILL (both arms) / BWALK / WALK / DASH. June tests time before class; the macro tests the condition first, and both are side-effect free, so the result is the same. `32976440` (ZZXX) and `32976480` (dash X-X: animation end + the four cancels) match.
- **Dash X-X `32999ce0`:** target `+0x9c24`, else Eve, for both the 0.01 and 0.1 blocks (vslot `0x138` GetPos(0)). 0.01: mesh at `y +200`; the particle vector is rebuilt from `x`, the stored `y +200` plus 290, `z`, i.e. `y +490`; AddRotate / AxisAngle, SetLandPosition from Eve (vslot `0x554 .y` -> `+0x320`). 0.1: `+0x969c` = target pos, `y +350`, CH1h's damage tail. 0.15: blade wave at that point. No class switch. End `329687c0` clears `+0x9c24` and tail-calls CommonStateEnd. Matches.
- **y +490 decision: no JUNE_BUGS row.** It keeps the particle 290 above the guardian mesh, the same mesh-to-particle offset dash-jump X-Z has (mesh at pos, particle at +290). That reads as intended placement, not a slip.
- **Alpha tags:** `3299968c` / `329998cb` (Z-Z-X), `32999dda` / `32999ed4` / `3299a119` (dash X-X) are all in the port.
- **JUNE_BUGS:** none found. funcmap has the 22 `Char Eve CH1i` rows.
- Hygiene: `flagcheck.py JUNE_CHAR_EVE_CORE --range cb1f7bca^..cb1f7bca`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. No rebuild (no code change). Nothing to play-test. `kickoffs/char-eve/CH1j.md` still matches. Next: `/x2-phase char-eve CH1j` (Sonnet 5.5 high).

### CH1j - Done (2026-10-26)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (a tenth block at the end of `X2GuEve.cpp`, 675 lines; declarations at the end of the class and inside `EveElectraSystem`). **All 30 class-map rows of phase CH1j**: 21 state members `June_ENSI_EPR_*` and 3 `EveElectraSystem` members; the other 6 helper rows are compiler-made or equal March's and are called, not copied. Nothing registers or calls the new members. No new enumerator. 29 funcmap rows `Char Eve CH1j` (`329cd320` keeps its CH1e row).

| State (`June_` prefix) | June: Init / StartFuture / Start / FrameMove / EventProcess / EndFuture / End |
|---|---|
| `ENSI_EPR_COMBO_ZZZfrontZ` | - / - / `32972af0` / `329a4c30` / `3297ccf0` / - / `32991c40` |
| `ENSI_EPR_COMBO_XXZ` | - / - / `32972b30` / `329a5430` / `3297d050` / - / `32991c80` |
| `ENSI_EPR_DASH_COMBO_ZZ` | `32967450` / `32967470` / `32991e10` / `32991f50` / `32972b80` / - / `32992190` |
| `ENSI_EPR_DASH_COMBO_ZZZ` | `32967480` / `32967490` / `32992210` / `32992440` / `32972ed0` / `329674b0` / `32992f60` |

Helpers: `EveElectraSystem::June_AddLaserToSystem` `32980000`, `June_LaserAttackLineSet` `3297a8e0` (= March), `June_DeleteLaserFromSystem` `32991940` (= March). Called, not copied: `32990c50` (= `EveElectraLaserData::Delete`), `3297b510` / `329cdb50` / `329cdd90` / `329ce4a0` (the data's constructor, destructor, copy constructor, list node), `329cd320` (`CreateInstanceHandle`).

**What differs from March** (the rest is March's body):
- **`32980000` AddLaserToSystem** takes two more trailing wstrings (after `_fRadiusScale`), stored at data `+0xc8` / `+0xe4`: two new fields under the flag at the end of `EveElectraLaserData`, `m_wstrJuneName4` / `m_wstrJuneName5` (names by position; the reader is in a later group). June's data also has a 4-byte pad at `+0xc`; not reproduced. The three names default to `EVE_PROTO_PURITY_LASER`, the new two do not.
- **Hyper arm** (`329a4c30` at 0.4, `329a5430` at 0.23333): while the gage (`this+0xc04`) hyper time (`+0xe8`) is above 0 (`GetRemainHyperModeTime() > 0.0f`) June creates `EVE_COMBO_ZZZFZ_LASER_HYPER` / `EVE_COMBO_XXZ_LASER_HYPER` and passes `EVE_PROTO_PURITY_LASER_HYPER` x3 / `EVE_COMBO_XXZ_LASER_NEXT_HYPER` x3; otherwise the March names. Both end `true, 2.0, L"", L""`. The effect handle is stored **after** AddLaser and only when the effect exists.
- **Dash Z-Z `32991e10`**: one 7c alpha tag on `EvePurityDashComboZBladeLoopEnd`. Dash Z-Z-Z `32992440`: `FlushMp( 10, 0 )` (second argument constant 0).
- **EventProcess x4**: March's shape with June's cancel macros (on-line test) and, for dash Z-Z, the June state-change macro (also fires on animation end). `+0x8e90` second-body clock in the cancel arms is checklist 7o, not ported.
- Everything else (Starts, Ends, StartFutures, Inits, the rotate / offset / MP-drain blocks, constants) is March's, checked op for op.

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Build: `DEPLOY OK: X2_offline.exe is 25,960,960 B, dated 2026-10-26 10:55:02, md5 a7ea820e9fcb98875112141fe4aaef3b, equal to X2/US_SERVICE/x2.exe (2026-10-26 10:54:54); built at main 25ef9949 + 2 uncommitted code file(s).`

**JUNE_BUGS:** none. **Drift:** none new. **Open for the review**: the names of the two extra laser strings; the hyper-time test as `GetRemainHyperModeTime()`; the `PlayEffectSet( "EffectSet_EVE_COMBO_XXZ_END" )` call uses defaults (its trailing force-down float is the default 1.0, as the decode reads it). Nothing to play-test: the code is unreferenced. Review: `kickoffs/char-eve/CH1j-review.md`. Next: `/x2-phase char-eve CH1j review` (Opus 5.5 high), then CH1j20.

### CH1j - Review (2026-10-26, Opus)

**PASS, one rename, no logic change.** Checked against `charport_adis.py eve --phase CH1j` (30 listings) and Ghidra (`32980000`, `329a4c30`, `329a5430`, `32992440`, plus the reader `329bc6e0`).
- **`32980000` AddLaserToSystem**: 14 arguments as transcribed (stack: effect, TraceBone, pos, dir, velocity `+0x2c`, propagate `+0x30`, width `+0x34`, three names, bool `+0x8c`, radius `+0x90`, two wstrings `+0x94` / `+0xb0`). Field order, init rotate from main effect `+0x5a8`, fire count 0, the three `EVE_PROTO_PURITY_LASER` defaults (tested on the arguments), list push, `SetLaserGroupID` with the 1..2 wrap (`+0x1b4`) and `3297a8e0` last all match. The `+0xc` pad is compiler layout; not reproduced, as the Done note says.
- **Rename (the open question).** The reader is the laser data's OnFrameMove `329bc6e0` (CH1l4). Its induction fire takes `+0xc8` (empty = `EVE_EBS_INDUCTION_LASER`) or, when attack type `+0x208 == 1` (`AT_SPECIAL`), `+0xe4` (empty = `EVE_EBS_INDUCTION_LASER_SPECIAL`). These replace March's two literals (`X2GuEve.cpp` ~23337). So `m_wstrJuneName4` / `5` became `m_wstrJuneInductionLaserName` / `m_wstrJuneInductionLaserSpecialName`, and the parameters were renamed to match. The CH1l4 kickoff and the funcmap row say so.
- **Hyper arm**: `+0xc04` gage null or protected `+0xe8` <= 0 -> normal arm, = `GetRemainHyperModeTime() > 0.0f`. The names and lengths match (ZZZfZ hyper `EVE_PROTO_PURITY_LASER_HYPER` x3, normal L"" x3; XXZ `_NEXT_HYPER` (30) / `_NEXT` (24) x3). Both calls pass `true, 2.0, L"", L""`. The handle (`+0x99d4` / `+0x99d8`) is stored after AddLaser and only when the effect exists.
- **Constants**: ZZZfZ `CreatePlane( 0, 2, 240, 130 )` @0.02, angle copy @0.3666, laser @0.4, drain `13.3333 * dt` in (0.4, 1.23333), rotate `78.3 * dt` in [0.4, 1.0). The offsets Y 15 / X -8 (`+0x983c` / `+0x9838`) are written even when the main mesh is null, as the C++ has it. XXZ `CreatePlane( +0x99a8, 2.4, 220, 130 )`, 0.3, 0.23333, rotate `41.904 * dt` in [0.83333, 1.93333). There Y 2 is set only with the mesh, 0 outside the window. END set @1.76666, drain in (0.23333, 1.8666), bone `Bip01_L_Finger2Nub`.
- **END `PlayEffectSet`**: June's arguments are March's defaults exactly (scale (1,1,1), zero vectors, trailing force-down 1.0), so this site adds nothing to CH1o.
- **EventProcess x4**: branch order matches the listings. ZZZfZ off line -> JUMP_DOWN jumps to the animation-end test. In XXZ, the cancels chain off the `pureZ == false` test. In dash Z-Z, after the off-line / animation-end arms comes `+0x7d85` flag, `+0x8c18` / `+0x8c14` / `+0x8c10`, `0xd2`, which is the macro. Dash Z-Z-Z: off line and t > 0.6, no y shift. All four cancels carry the on-line test; the `nowAction` 0xe / 0xf store is June's protected write.
- **Dash bodies**: the `32991e10` tag (`32296ee0`) follows the handle store `+0x99f4`, and `+0x99d0` = -1. `32992440`: eight timers, bones, y offsets, global times and blink fade all match. `FlushMp`'s second argument is unused (`3208dc70`, C3q), so `FlushMp( 10.f )` is equal. `32992210` / `32992f60`: alpha object `+0x144` / `+0xc74`, colours `+0xc34` / `+0xc44`.
- **Helpers**: `3297a8e0` = March (CT_LINE: radius `+0x8c`, scale-Y `+0x94`); `32991940` = Delete + erase.

**Hygiene.** CH1j commit `flagcheck.py JUNE_CHAR_EVE_CORE --range cd23cae5^..cd23cae5`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. Worktree after the rename, `--base 25ef9949`: the same lines, `RESULT: PASS` (vs HEAD the 6 "deleted" lines are CH1j's own renamed lines).
Build: `DEPLOY OK: X2_offline.exe is 25,960,960 B, dated 2026-10-26 11:14:08, md5 b92f6c1b23b59abac43b9bed89dcd688, equal to X2/US_SERVICE/x2.exe (2026-10-26 11:14:01); built at main cd23cae5 + 2 uncommitted code file(s).`

**JUNE_BUGS:** none found. **Drift:** none. Nothing to play-test (unreferenced). `CH1j20.md` still matches. Next: `/x2-phase char-eve CH1j20` (Opus 5.5 high).

### CH1j20 - Done (2026-10-26)

This phase covers the five June-only `CX2GUUser` helpers CH0 found (`ch0_unmapped_identified.tsv`). All five are ported whole under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`, in one block after `JuneCommonSpecialAttackEventProcess` in `X2GUUser.cpp` and one block after `m_vecJuneSpecialOptionValue` in `X2GUUser.h`. They are inert: nothing calls them until CH1j2 / CH1l2 / CH1l3 / CH2c / CH2c2. Funcmap: 6 rows `Char Eve CH1j20`.

| June | March | What |
|---|---|---|
| `31fce710` | `CX2GUUser::JuneIsEquippedSkillKeyPressed( const InputData&, SKILL_ID ) const` | the one-byte of the key on the skill's equipped slot (A, then B); B-lock gate on my unit's tree |
| `31fce8b0` | `CX2GUUser::JuneIsEquippedSkillKeyReleased( ... ) const` | the same lookup without the gate; returns `pure == 0` |
| `31fc89a0` | `CX2GUUser::JuneSkillSlotKey::IsKeyReleased( const InputData& ) const` | slot 0-9 -> `pure == 0`; any other index -> true |
| `31fce670` | `JuneSkillSlotKey::FollowSlotSwap()` | the UI slot-swap byte changed -> index +5 (stored byte 1) / -5 (stored byte 0), byte flipped |
| `31fce6d0` | `JuneSkillSlotKey::Set( int )` | index = arg, byte = `m_bSkillSlotSwap` |
| `+0x8b58` | `CX2GUUser::m_kJuneSkillSlotKey` | a `CX2GUUser` member (it sits below `+0x93a8`); June's ctor `@320403b5` sets -1 / 0, so the struct's default ctor does the same |

- **Key bytes are mapped by name.** March's `InputData` puts `Skill5` at `0x4c`, June at `0x50`. The mapping is A = A / S / D / C / Skill5 and B = Q / W / E / R / SkillEx5, checked against `31fcadd0`'s `+0x872b` = oneSkill5.
- **UI `+0x115` is `m_bSkillSlotSwap`** (`GetSkillSlotSwap()`): June's `SwapSkillSlotTypeA` / `TypeB` (`328db100` / `328daed0`) write it.
- **Correction to the CH0 tsv:** the re-base is +5 / -5 (`@31fce6a7`: neg / sbb / and -10 / +5), not "-11".
- **The gate in `31fce710` is written out.** March's static `JuneIsMyUnitSlotUsable` was not reused: it returns false when a pointer is NULL, but June then skips the gate.
- **No `g_pData` test** in `31fce670` / `31fce6d0`, as in June.
- **Equivalents checked:** `JuneCommonSpecialAttackEventProcess` (`31fcadd0`) is the press-to-slot direction; nothing in March maps a skill to its key. The S0b / S5 slot-state API (`IsJuneSlotUsable`, `GetSkillEquippedSlotIndex`) is reused.

Hygiene: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (worktree).
Build: `DEPLOY OK: X2_offline.exe is 25,961,984 B, dated 2026-10-26 11:39:33, md5 b02ac58fa6482ab3af167ac552d8af39, equal to X2/US_SERVICE/x2.exe (2026-10-26 11:39:26); built at main 4972faf8 + 2 uncommitted code file(s).`

**JUNE_BUGS:**
- 426: the slot-swap re-base follows the stored byte, not the slot's row, so the index can leave 0-9 and read as "released".
- 427: the B locks in `31fce710` are read from my unit's tree, like 415.

**Kickoffs amended** ("Amended by CH1j20"): CH1j2, CH1l2 (`31fce710`), CH1l3, CH2c2 (`31fce8b0`), CH2c (the struct).
**Open:** `+0x88bc`, the value Particle Ray StartFuture passes to `Set` minus 1, is not named. The candidates are `m_iNowSpecialAttack` and `m_eSpecialAttackKeyPressed`; CH2c settles it.
**Drift:** none. Nothing to play-test (unreferenced). Review: `kickoffs/char-eve/CH1j20-review.md`.

### CH1j20 - Review (2026-10-26)

**PASS, no code change.** All five bodies, the ctor store and the `+0x115` writers were re-read in Ghidra against `X2GUUser.cpp`.

- **Key bytes:** June `+0x18..+0x55` from `+0x86da` = March C / A / S / D / Q / W / E / R at the same offsets; June `+0x50` / `+0x54` = March `Skill5` / `SkillEx5` (March `+0x4c` / `+0x50`; June's `+0x4c` is Sit, `+0x8726`). Every `one*` in `31fce710` and `pure* == 0` in `31fce8b0` / `31fc89a0` matches. The members are named by bar, not by key: `X2Game.cpp` fills `pureA` from `GA_SKILL_EX1` under the swap.
- **`31fce710`:** two `31fca8e0( id, 1 )` calls with `this`, then `+8` = `SkillTemplet::m_eID`. A is searched before B on `unit +0x494`. The gate is skipped when `g_pData`, `+0x68` or `+0x90` is NULL. Key 0 is tested first (B, index >= 0 and not 4), then key 1 (B, index 4), the same order as the port.
- **`31fce8b0`:** the same prelude with no gate; the port matches.
- **`31fc89a0`:** 0-9 map to A / S / D / C / Skill5 / Q / W / E / R / SkillEx5; any other index returns true.
- **`31fce670`:** `@31fce6a7` (dec / neg / sbb / and -10 / +5) gives +5 for stored byte 1 and -5 otherwise. `setz` flips the byte; with both bytes 0/1 and unequal, that equals copying the UI byte. Neither function tests `g_pData`. March's `GetUISkillTree()` is a plain accessor, like June's `3214f050` (`+0x3c`).
- **JUNE_BUGS 426:** the wording is right. Slots 5-9 under byte 0 and 0-4 under byte 1 follow the held key (into 0-4 / 5-9); the other half leaves 0-9.
- **`+0x115`** is written by `328db100` / `328daed0` (`SwapSkillSlotTypeA` / `TypeB`) = `m_bSkillSlotSwap`. **`+0x8b58`** is set in the `CX2GUUser` ctor `3203e850`: `@320403b5` stores EDI = -1 (`or edi,-1` `@3204029d`) and `@320403bb` stores BL = 0.
- **`+0x88bc` = `m_iNowSpecialAttack`.** `32ccf4e0 @32ccf98d / @32ccf993` stores slot+1 to `+0x8ac0` and then `+0x88bc`. `32cb97a0 @32cb9872` stores only `+0x88bc`. The reviewed Aisha port maps these to `m_eSpecialAttackKeyPressed` / `m_iNowSpecialAttack` (`X2GUArme_VioletMage.cpp` ~30148 / 30239), so Particle Ray calls `Set( m_iNowSpecialAttack - 1 )`. `CH2c.md` and the funcmap row are amended.

Hygiene (`--range 2c87c040^..2c87c040`): `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Build: none. There was no code change; the deployed exe stays md5 `b02ac58f`. `CH1j2.md`'s "Amended by CH1j20" still matches. No new JUNE_BUGS row.

### CH1j2 - Done (2026-10-26)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (an eleventh block at the end of `X2GuEve.cpp`, 1,297 lines; 48 declaration lines at the end of the class). **All 46 class-map rows of phase CH1j2** (45 state functions + the helper `32979a00`) are new `CX2GUEve::June_*` members; nothing registers or calls them. No new enumerator, field or helper beyond `32979a00`. 46 funcmap rows `Char Eve CH1j2`.

| State (`June_` prefix) | June: StartFuture / Start / FrameMove(Future) / EventProcess / EndFuture / End |
|---|---|
| `ENSI_EBS_BOOST_DASH_START` | `32995020` / - / FMF `329677d0` (June-only) / `32995120` / `329784c0` / - |
| `ENSI_EBS_BOOST_DASH` | `32967830` / `32995470` / `32995550`, FMF `32978500` / `32995680` / - / `32995ba0` |
| `ENSI_EBS_BOOST_DASH_AIR_START` | `329786e0` / `32967850` / FMF `32978770` / `32978890` / `32995c10` / - |
| `ENSI_EBS_BOOST_DASH_AIR` | `32967860` / `32995d60` / `32995e40`, FMF `32978ad0` / `32979da0` / `32978bf0` / `32995f70` |
| `ENSI_EBS_DASH_JUMP_COMBO_XZ` | - / `32974000` / `329a8840` / `3297e2a0` / `32967890` / `32995fc0` |
| `..._ZFrontX` `_ZUpX` `_ZDownX` | - / `32996020` `329962a0` `329965a0` / `329678b0` `329678e0` `32967910` / `32979fe0` `3297a110` `3297a240` / `329678c0` `329678f0` `32967920` / `329740b0` `32974100` `32974150` |
| `ENSI_ECB_DASH_XX` (June-only, 237) | Init `329687e0` / - / `329ad6c0` / `329766f0` / - / - |
| helper | `June_CanContinueBoostAttack( int )` = `32979a00` |

**What differs from March** (the rest is March's body, checked op for op; most of the 46 are March verbatim):
- **Raw physic reads.** Every `nowSpeed.y = ...` here reads `m_PhysicParam` `+0x330` (dash jump) and `+0x32c` (jump), i.e. March's `m_PhysicParam.GetDashJumpSpeed()` / `GetJumpSpeed()`, not the unit-level triples CH1e used for the plain dash. `GetDashSpeed()` is the triple (`32071a80`).
- **Boost dash start / boost dash EventProcess** (`32995120`, `32995680`): March with `June_GetMaxDashJumpCount` / `June_GetMaxDashJumpUpCount` and `SKILL_CANCEL_AFTER( 0.01f )`; the 1 / 2 / 3 s speed steps now tag each boost mesh (one shared tag site, `32995b6e`). The `+0x8e90` second-body clock in the cancel arm is 7o, not ported.
- **Facing block** (`329677d0`, and at the tail of `32978500`): both keys held -> `oneRight` / `oneLeft` decides, else the single key; then `CommonFrameMoveFuture`.
- **Air boost dash start / air** (`32978890`, `32979da0`): the wall landing is live, the up limit is `32976f00`, and the `m_bDownForce = false` resets are gone (`3297a400` clears it; JUNE_BUGS 421 stays withdrawn). Air X: `+0x9b56` = 0, `+0x9b58` = false, `June_StateChangeBoostAttackIfPossible`.
- **Boost attack front / up / down EventProcess** (`32979fe0` + two copies): the cap is `32979a00( add )`; add = `+0x9b5c`, raised by Open Mind (25419, ability `0x80`); with Electronic Move (25223) as the future skill, X is `JuneIsEquippedSkillKeyPressed( m_InputData, skill )` and add = 0. The helper is true when the future skill is not Electronic Move (`JuneIsSameOrUpgradeSkill`), `+0x9b5c > 0` and the 25029 memo is learned (`IsUpgradedSkillLearned`), else when the count is below add + 3. CH0 labelled the helper "called by 32979da0"; its callers are the three EventProcess.
- **Dash-jump X-Z FrameMove** (`329a8840`): `June_CreatePlane`, the 0.533 hyper arm (`_HYPER` effect and laser names while `GetRemainHyperModeTime() > 0`), AddLaser before the main-effect rotate, the handle stored last and only when the effect exists.
- **Code Battle Seraph dash X-X** (`329ad6c0`, June-only): one else-if chain of oneshots 0.066 (plane 70 / 2 / 140 / 230), 0.2 (three `Light_Eve_AttackX3` at `Bip01_Spine` +- direction x 10 / -55 / 85, y +50 / -80 / -80), 0.315 (MP 4 x `+0x97b0`; `DamageEffect_ECB_DASH_XX_Triangle` at the 0.8 / 0.2 hand mix +- direction x 50, power `+0x97c0`, force down `+0x97c4`, `June_AddEffectToSystem` with four empty names, `Eve_FireBigShot.ogg`; else `CreateNotEnoughMPEffect`), 0.23 (`Eve_AttackX3_Triangle01` at `GetPos` +- direction x 10, y +85). EventProcess `329766f0` is the Empress dash X-X's `32976480` op for op.
- **Alpha tags (7c):** 12 calls on the 9 sites of `c10a_alpha_sites.tsv` (`329950fc` `32995b6e` `32995d38` `32996163` `329963da` `329966da` `329ada7f` `329adaf9` `329ae08f`; the last and `32995b6e` are shared tails). Checklist 7c amended.

**Checks.** `cl /Zs` of `X2GuEve.cpp` (US_SERVICE defines): 0 errors, and the `cl /P` output holds the block (flag live). Scripts compared every listing with its member: wide strings (46 of 46; the two extra listing strings are the next function's), float constants, and the state names / ids (the `ENSI_` vs `USI_` spellings differ only by the listing annotator).

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Build: `DEPLOY OK: X2_offline.exe is 25,980,416 B, dated 2026-10-26 12:16:32, md5 3e84adb84e95b760280748e5e1c43e1e, equal to X2/US_SERVICE/x2.exe (2026-10-26 12:16:25); built at main d8f6c993 + 2 uncommitted code file(s).`

**JUNE_BUGS:** none. **Drift:** none new. **Open for the review**: the raw-read sites above; the helper's name and the `IsUpgradedSkillLearned` reading of `31fcf3e0`; the 12-vs-9 tag count. Nothing to play-test: the code is unreferenced. Review: `kickoffs/char-eve/CH1j2-review.md`. Next: `/x2-phase char-eve CH1j2 review` (Opus 5.5 high), then CH1k.

### CH1j2 - Review (2026-10-26)

**PASS, no code change.** All 46 members were read against `charport_adis.py eve --phase CH1j2` and, where a function object exists, the Ghidra decompile.

- **Raw reads:** every `nowSpeed.y` store reads `+0x330` after `USI_DASH_JUMP` (`32995120`, `32995680`, `32978890`, `32979da0`) and `+0x32c` in the two jump-up caps of `32978890` / `32979da0`. These are March's `fDashJumpSpeed` / `fJumpSpeed` (`X2GameUnit.h:1093-1094`). Every `nowSpeed.x` store goes through the triple (`32071a80` = `+0x423c` x protected `+0x49c4`).
- **Boost dash:** `32978500` is `( H + n ) * D + D` with n = 0.1 / 0.2 / 0.3 and `H * D + D` by default, i.e. the port's `D + D * ( H + n )`. The gate reads `pure*` and `m_BeforeInputData.pure*` (`+0x8682` / `+0x8686`). The 1 / 2 / 3 s steps run create, then `PlaySound`, then the tag. The start's 0.3 s and the 0.01 cancel match. June's `TryHyperModeStateChange( GetHyperModeStateID() )` equals the port's `USI_HYPER_MODE`: Eve's vslot 0x6b8 (`32968470`) returns 0x17.
- **Facing (`329677d0`, tail of `32978500`):** both held: `oneRight` (`+0x86df`) -> right, else `oneLeft` (`+0x86db`) -> left. Otherwise a single held key sets the facing. Then vslot 0x7c8.
- **Air start / air (`32978890`, `32979da0`):** the wall landing sets gravity off and then 0xbd. The up limit is `32976f00` inlined (BFI 0x29f -> `+0x9c60` + 1). No `m_bDownForce` reset. Air X does `+0x9b56` (short) = 0, `+0x9b58` = 0, then `32978c60`. The FrameMoveFutures (0.4 / 0.65, 1.85, 1.0, `pow( t, 2 )`) and the StartFuture / EndFuture stores match.
- **Boost attack front / up / down:** the three EventProcess listings are identical apart from addresses. Add = `(int)( 32007b10( 0x80, 25419 ) + +0x9b5c )` when `31fceab0( 25419 ) > 0`. For 25223 it compares the skill id with `==`, then `JuneIsEquippedSkillKeyPressed` and add = 0. The windows are 0.13333 / 0.59 / 0.833. The helper's callers are `3297a07d` `3297a1ad` `3297a2dd`. Its three tests are right: `31edf580` = `JuneIsSameOrUpgradeSkill` with the skill-tree test, and `31fcf3e0` = `IsUpgradedSkillLearned`. The name stays. The Starts match: Front has no jump-fly count; Up / Down rotate by +-45 and +-90.
- **XZ:** the plane is built as `( 0x9a3c + 0x99a8, 1.8, 150, -50 )`. The 0.533 arm takes the gage at `+0xc04` (NULL or time <= 0 -> plain names), which is March's `GetRemainHyperModeTime()`. AddLaser comes before the rotate, and the handle is stored last, inside `if( effect )`. The bands are 0.733-1.233 (z += 70.904 dt, offset Y 2) and 1.233-1.733 (0.904 dt), else Y 0, all on `m_pXSkinAnim`. The 1.833 block and the MP drain (0.6-1.866, 13.3333 dt, `Bip01_L_Finger2Nub`) match. EventProcess sets nowAction only when the value differs (the inlined Set).
- **ECB dash X-X `329ad6c0`:** the chain order, the constants and the strings match. The offsets are 10 / -55 / 85 with y +50 / -80 / -80. The Triangle effect uses a 0.8 Rhand / 0.2 Lhand mix and rot.z + 70. MP is 4 x `+0x97b0`. CreateInstance's trailing arguments `landY, 0, -1, +0x97c4, 1, -1, -1, 0, -1, 0, 0` are March's defaults. Not enough MP puts `( Rhand + Lhand ) * 0.5` at z = rot.z + 70. EventProcess `329766f0` matches `32976480` instruction for instruction, and the two ports are line-identical.
- **Alpha tags:** 9 unique `32296ee0` / `32296d60` call sites in the 46 listings, 12 port calls (`32995b6e` serves three steps; `329ae08f` serves the third Light and Triangle01). Confirmed.
- **JUNE_BUGS 428 (new):** in `329ad6c0` the 0.23 s Triangle01 test sits after 0.315 in the else-if chain, so a frame that crosses both 0.2 and 0.23 shows only the 0.2 lights.
- `ch0_class_map.tsv`'s helper row is relabelled from "called by 32979da0" to its three callers.

Hygiene (`--range 722432b3^..722432b3`): `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Build: none (no code change; deployed md5 `3e84adb8`). `CH1k.md` still matches. Next: `/x2-phase char-eve CH1k` (Sonnet 5.5 high).

### CH1k - Done (2026-10-26)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (a twelfth block at the end of `X2GuEve.cpp`, 1,223 lines; 57 declaration lines at the end of the class). **All 40 class-map rows of phase CH1k** (the 39 of the kickoff + `329ac330`, moved from CH1c): **30 are new `CX2GUEve::June_*` members** (29 functions + `JuneEveBlock9c50::StopEffectSets`), **10 resolve to March or engine code** and get no member. Nothing registers or calls them. 46 funcmap rows `Char Eve CH1k` (the 40 + 6 callee rows).

| Group | June -> member (`June_` prefix) |
|---|---|
| lifecycle | `3299bd70` ConstructorInit, `329aece0` Destructor ( `329cf670` is its wrapper ), `329af460` ReInit, `329c47e0` InitComponent, `329c3da0` UpdateEveRecoveryTooltip |
| effects / particles | `32965dd0` InitEffect, `32965e10` CreateMinorParticleArray, `3297a940` CreateMajorParticleArray, `32994710` DeleteMinorParticle, `32969190` InitEnchantWeaponEffectUnitSpecific, `3297bb70` AddUnitSlashData, `329a2910` ClearThousandStar |
| reactions / render | `32977130` DamageReact, `32968ea0` / `32977300` / `32968ee0` Win / Lose / Draw, `32973d30` ModifyFinalDamage..., `32973e70` ApplyRenderParam, `32978420` ApplyRenderParamOnEmpty..., `3299cdc0` AttackResultPlayEffect, `329a4680` AttackResultByType |
| states / misc | `3297fdd0` COMMON_BUFF_FrameMove, `32994750` HyperModeBuffEffectStart, `32997c60` CommonHyperModeFrameMove, `329ac330` ENSI_RIDING_HYPER_MODE_FrameMove, `32965f80` DashStartFuture, `329cf210` GetComboZStateID, `329770d0` Vslot584, `3297b4b0` Vslot56c, `329cf170` Block9c50::StopEffectSets |

**Resolved without a member:** `329ce310` / `329ce210` / `329ce0a0` / `329cd860` / `329cd440` are the engine's `CX2UnitSlashTraceManager` and its vector (March's own; `June_AddUnitSlashData` calls it); `329cda50` (EG_QueensThroneData) and `329cc3b0` (ENERGY_NEEDLES_Data) are March's struct constructors, value for value; `32990da0` is March's `EveElectraSystem` constructor plus the June-only last member `+0x1c0 = 1` (new `EveElectraSystem::m_iJune1c0`); `32991c00` is `FlyingImpactData::GetLiveDamageEffect`.

**What differs from March** (the rest is March's body; the Win / Draw / Lose, InitEffect, particle builders, slash trace, AttackResultPlayEffect, `32978420`, `32969190`, `32994710` are verbatim):
- **`32973d30` (vslot 0x3ac).** June's hook takes `( DamageData*, float&, float& )` and runs for every attack that is not AT_SPECIAL; the Nasod Weapon is now an AT_COMMAND proc (class 108, flag `+0x8b62` clear): roll SA_EFFECTIVE_RATE or the memo buff 25416; only magic damage (+SA_DAMAGE_REL) and force down (-SA_FORCE_DOWN_ABS) change, no DT_MIX, no x2.
- **`32977130`.** No Ferdinand block. `+0x9bf8` > 0 (Queen of Vengeance) and still alive: `UpNowMp( GetMaxMp x +0x9bf8 x HP fraction lost )` for a loss of 1% or more; then `m_fInvisibilityTime = 0` and the CX2GUUser byte `+0x8fc2` = 1.
- **`3297fdd0`.** No world-colour / cut-in head; the new 0.01 oneshot arms the super armor (`bSuperArmor`, `bSuperArmorNotRed = false`, byte `+0x89fc`) for skill 25214.
- **`329a4680`.** Returns early on DD `+0x628`; the thousand star fire needs the defender not to be me; the gauge gain takes ( 1 + `+0x6c50` ) and trigger 5 is March's (both under `ADD_EVE_SYSTEM_2014`, dead today); trigger 44 moves the live ally summons 1621 / 1622 (`320017d0`, `CX2GUNPC::SetPosition`) to the defender.
- **`32997c60` / `32994750`.** The awakening effect-set stand-ins, the item effect `31febf50` (stand-in), the trace block only without the awakening set, and the Open Mind (25415) reset of the Code Recovery cool time; the four hyper boost handles are skipped for a transcended unit.
- **`329c47e0` / `329c3da0`.** Ten Lua keys by class group (18 / 108: `..._ECN`, `ADD_AIR_DASH_COUNT`; 19 / 109: `..._ECE`; 26 / 116: `..._ECB`) with their June defaults; the tooltip text (string ids 0x8fe1-0x8fe4) is built, its `322dee90` call is left for CH4c. `329af460` / `329aece0` carry the six summon ids and `329ae670`; the `322e65c0` call is left for CH4c.
- **Constructor.** `ch1k_ctor_offsets.tsv` classifies all 185 June stores: 79 March members and 29 June-only members (set by `June_ConstructorInit`), 45 inside nested structs, 15 Nasod Core (left to CH1q), 7 unmapped slots, 10 in the base classes. June's `CX2GUEve` has no `m_timerSummoned` / `m_fSummonedLifeTime` / `m_ChargingBoosterSkillDataBasePtr` and no Atomic Shield pair (CH1l4's kickoff amended); the base-class `MemoCondition*` at `+0x7d10` is not ported.
- **Alpha tag (7c):** `329a2a0b` (the die mesh in `June_ClearThousandStar`). Checklist 7c and spec A (7) amended.

**New June-only fields** (CX2GUEve unless noted): `m_fJuneVengeanceMpRate` `+0x9bf8`, `m_fJune9bfc`, `m_fJune9c00`, `m_aiJune9c30[5]`, `m_iJune9c48`, `m_fJune9c4c`, `m_bJune9c54`, `m_fJune9c58`, `m_fJune9c5c`, the Code Recovery `m_fJuneCodeRecoveryCoolTime` `+0x9c68` / `UpHpRate` `+0x9c70` / `NoneSummonMpCostRate` `+0x9c74`, and the CX2GUUser slots `m_bJune8fc2` (`+0x8fc2`), `m_bJune89fc` (`+0x89fc`, write-only), `m_fJuneSpecializationA` (`+0x6c50`). New callees: `June_HaveSkillBuff` / `June_SetSkillBuffFactors` (Eve-local copies), `June_FindAllyNPC` (`320017d0`), `June_GetFirstTimeFinalizerValue` (`32d417e0`).

**Checks.** `cl /Zs` of `X2GuEve.cpp` (US_SERVICE defines): 0 errors, 0 warnings in the block, and the `cl /P` output holds it (flag live; the `ADD_EVE_SYSTEM_2014` bodies are dead). March's Nasod Core members do not exist in this build, so every function that touches them keeps that part under `ADD_EVE_SYSTEM_2014` with the June site in the comment.

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Build: `DEPLOY OK: X2_offline.exe is 25,994,240 B, dated 2026-10-26 13:37:05, md5 1fa8b70c4c5e707c46f02c5dfce54361, equal to X2/US_SERVICE/x2.exe (2026-10-26 13:36:57); built at main 2516ab20 + 2 uncommitted code file(s).`

**JUNE_BUGS:** 429 (the unset tooltip buff time), 430 (the unguarded defender test), 431 (slot 0x584 reads the NPC templet of any unit). **Drift:** `31febf50` (the Dark Dragon hyper-mode item effect, called by every class's hyper-mode FrameMove) joins the awakening effect-set family in backlog Stage 12 (bullet amended). **Open for the review:** the names of vslots `0x56c` / `0x584`; the unmapped constructor slots (`+0x97a4 .. +0x97c7`, `+0x9b6c .. +0x9b97`); the 32-bit UID compare in `329a4680`. Nothing to play-test: the code is unreferenced. Review: `kickoffs/char-eve/CH1k-review.md`. Next: `/x2-phase char-eve CH1k review` (Opus 5.5 high), then CH1l.

### CH1k - Review (2026-10-26, Opus)

**PASS with two fixes** under `_CORE` (inert code, no behaviour change in game). I read all 40 rows against `charport_adis.py eve --phase CH1k`. A subagent cross-checked the constructor (all 185 tsv rows against the 209 stores; the extra 24 are -1 re-stores) and the bodies that should equal March; I spot-checked its two open leads.

- **Fix 1, `329a4680`:** the test at `@329a477d-329a47a9` compares the defender's protected `+0x278` with mine. `+0x278` is the **team** (`KProtectedType<int> m_TeamNum`, as in the funcmap rows for `31f70330` and the effect-set team owner), not a UID. So the Thousand Star fires only on an enemy defender. The port compared `GetUnitUID()`; it now compares `GetTeam()`. This settles the Done note's "32-bit UID" question.
- **Fix 2, `32977130`:** with no gage, June's first read is now 1 / max 1 (`@32977164`), not 0 / 1. Both reads are now June's. Behaviour was the same either way, because the second read has the same defaults.
- **`32973d30`:** the only gate is AT_COMMAND (2): June's `GetFinalDamage` `@322857fb` sends AT_SPECIAL to `0x3b0` and every other type to `0x3ac`. The arguments are `( DD, &phys, &magic )`, with the same push order as `32622780`. The checks run in this order: class 108, then `+0x8b62`, then the roll `GetRandomFloat( 0 ) < SA 5` or `June_HaveSkillBuff( 25416, 1, 1 )`, then magic += SA 15, then force down (DD `+0x3f0`) -= SA 42. There is no DT_MIX and no x2.
- **`329a4680`, the rest:** it returns early on `+0x628 == 1`. The NPC-class test is unguarded (JUNE_BUGS 430). The fire sets 9a10 / 9a14 / 9a30, does the lock-on and calls `SetDirSpeed( 0 )`, and **does not return**. March returns there when `ADD_EVE_SYSTEM_2014` is off; the port follows June. Then come the second lock-on, the gauge `max( phys, magic ) x rate x 4 x ( 1 + +0x6c50 )` capped at 100 (which also needs the gage `+0xc04`), trigger 5 (`0x70c`, then the bone name into `+0x9bdc`), and trigger 44 (`+0x9c54`, the defender's `GetPos`, then `320017d0` for 0x655 / 0x656 and `SetPosition( pos, true )`).
- **ReInit / destructor:** the six ids are called in June order, then `329ae670`, then `329a2910`. The rest matches: the Electra teardown, `+0x99a4 = 1` (`m_iJune1c0`), the Nasod Core block, and `Block9c50::Reset( true )` / `StopEffectSets` + delete. The destructor's `@329af13e-329af1a8` is `329ae670` inlined, and `322e65c0` is left for CH4c.
- **Constructor:** the June-only members equal June. March's 1.0 rates are at `+0x97b0` (MP, protected), `+0x97c0` (attack) and `+0x97c4` (force down), all March values. `+0x97a8` (-1.0) and `+0x97ac` (0) are June-only with no known role, and `+0x97a4` has no store. `+0x9b6c` / `+0x9b70` (0) stay unmapped. No June store falls on the four dropped March members. The `0x96a4` cell is fixed (it was `0x29`; the real value is 0.0), and the tsv rows are reclassed.
- **`329c47e0` / `329c3da0`:** the ten keys and their defaults (30, 50, 0.05; ECN 30 and 3; ECE 30 / 50 / 0.05 / 1; ECB 30) are read in June order, with the class groups 18 / 108, 19 / 109 and 26 / 116, then the tooltip. The string ids, the argument orders and JUNE_BUGS 429 are as noted. `322dee90` is left for CH4c.
- **Hyper mode (`32997c60` / `32994750` / `329ac330`):** the world colour is 0x22 grey. The first oneshot is skipped when absolutely invisible. The shock waves (+5, 5/10, 10/10, 200/200) run only without the awakening set. `GetShowSmallGageAndName` still runs while invisible. `31febf50` comes next. The trace block also needs no awakening set; its show/hide follows invisibility. Then come `ApplyHyperModeBuff` (vslot 0xc0) and Open Mind: `vslot 0x23c( 25415, 0, 1 )`, then `+0x9c64 = 0`. The hyper boost handles are skipped for `+0x3c` or the awakening set. The gauge is min( +40, 100 ).
- **Small ones:** `3297fdd0` 0.01 sets protected `+0x7ff4` = 1, `+0x8040` = 0 and `329ce7b0( 1 )` (`+0x89fc`). `32973e70`'s tail calls `31fd4460` (CX2GUUser's CUSTOM_RENDER_STATE setup, owned by Stage 2 Engine R0); until then the port uses March's `CX2GameUnit` base. `329770d0` `31fc8070` = `!HaveThisBuff( 984 )`. `3297b4b0` and `3299cdc0` match. The vslot names `0x56c` / `0x584` stay unsettled until a caller names them.
- **Matching March:** Win / Lose / Draw, InitEffect, both particle builders, `32969190`, `32978420`, `329cf210` and `329ac330` match. The engine rows are as resolved. `32994710`'s trailing `322ceab0` is March's unused `GetMinorParticle()` fetch. In `3297bb70` the stride is 0.0 (`fst [esp] @3297bd2f`). `32965f80` calls the base `31fcbe80`, which March's Eve never did; its funcmap note is corrected.
- **Alpha tag:** `329a2a0b` is the only `32296ee0` / `32296d60` site in the 40 listings.

Hygiene (`--range 714ceb95^..714ceb95`): `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The fix (`--worktree --files X2Lib/X2GuEve.cpp`) also passes: 1 PASS, 4 replaced lines inside the flag.
Build: `DEPLOY OK: X2_offline.exe is 25,994,240 B, dated 2026-10-26 13:57:16, md5 9fe9576f19692ae39bb1943ea1c1f67f, equal to X2/US_SERVICE/x2.exe (2026-10-26 13:57:09); built at main c7b9241c + 1 uncommitted code file(s).` Nothing in game. `CH1l.md` still matches. Next: `/x2-phase char-eve CH1l` (Sonnet 5.5 high).

### CH1l - Done (2026-10-26)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (a thirteenth block at the end of `X2GuEve.cpp`, namespace `_CONST_EVE_JUNE_CH1l_`, +507 lines; 14 declaration lines in `X2GuEve.h`). **All 11 class-map rows of phase CH1l** (the 10 + `329cc500`, moved from CH1e): 10 are new `CX2GUEve::June_*` members, `329cc500` is a namespace helper. Nothing registers or calls them. 18 funcmap rows `Char Eve CH1l`.

| June | member (`June_` prefix) | notes |
|---|---|---|
| `32968f20` ( 0x7b8 ) | SetEquippedSkillLevel | 25021 -> states 211 / 212 / 213 ( each twice ), 5018 -> 222 / 223, 25223 -> 239-241, 25206 -> 205 / 206, 25236 -> 242 ( no null test ); every state through `June_SetSkillLevelStateDataByID` ( `32028190` ) / `June_SetSkillLevelStateData` ( `3201f850`, Eve copies of the Arme ones ) |
| `32977390` ( 0x684 ) | InitEquippedActiveSkillState | Mega Electronball 25207 / Taser Pilum 25212 / Kugelblitz 25024 slot ids in June's 0-9 slot space; `m_iJune1c0` = 3 / 1; Queen's Throne rate x ( `3201ad10` + 1.0 ) |
| `32978e80` ( 0x6fc ) | InitInpuDataProcess | ZXArrow, [tutorial mask left out], then Mega / Taser / Kugelblitz -> `Init` keeping the pure key of slot 0-9; `329cc500` = March's lambda |
| `3299cf70` ( 0x600 ) | HyperModeFrameMove | `June_CommonHyperModeFrameMove( 0.05, 0.83, false )` |
| `329b7390` + `329b4ac0` ( 0x7b0 ) | ApplyAuraAcceleratorBuffToGameUnit + GetEnergyOfConcentrationBuffFactorClonePtr | four arguments ( index ); skill 25402's clone; `AdjustBuffFactorByIndex( 25201 )`; `JuneSetTimeFinalizerValue`; no party test; alpha tag `329b77ad` |
| `32977090` / `32979a70` / `3298c580` / `3299aee0` | Vslot6a8 / Vslot65c / Vslot6ac / Vslot77c | the four unnamed slots ( name unsettled until CH1o ) |

**Resolved without a member:** `31fc66d0` = `InputData::Init`, `3226e2e0` = `IsCanNotIntput`, `32767930` = `CX2GUNPC::SetPosition`, `31f8b860` = `vector<int>::push_back`, `32089f60` = `CX2BuffFactor` levelled clone, `31edf580` = `CX2UISkillTreeNew::JuneIsSameOrUpgradeSkill`. Eve-local copies: `31edf560` ( `JuneEveIsSkillOrUpgrade` ), `32007c40`, `32205440` ( `JuneEveAccelCoreParticleName`: March's `GetAccelCoreParticleName` lacks June's ids 0x56ba-0x56cf, 0x6271, 0x627e, 0x765d-0x765f ).

**What differs from March:**
- **`32977390`.** Slot space 0-9 ( A 0-4 / B 5-9 ) with no `GetEnabledSkillSlotB`; March keeps 4 + 4 under `SKILL_SLOT_10` ( CH1p converts every reader ). Only the Kugelblitz loop tests the B lock, on MY unit's tree ( JUNE_BUGS 432 ).
- **`32978e80`.** March's body plus the Taser and Kugelblitz branches and the 10-key table ( June `+0x50` / `+0x54` = March `pureSkill5` / `pureSkillEx5`; June's `InputData` has one more key group before them ).
- **`329b7390`.** Hands the base a literal 0 as the index ( JUNE_BUGS 433 ); applies to every same-team user in 700 units ( no PvP / party test ); the Energy of Concentration clone comes from `329b4ac0` ( level read with the hyper flag ).
- **`32979a70`.** The NPC byte `+0x8751` has no March member ( only `322ec9b1` sets it, for NPC 3112 ): a labelled stand-in returns false, so the getter answers 0.0 until a phase finds the writer.

**Offsets settled by `offsetprobe`** ( March layout, same base ): `+0x97e4` = `m_EveElectraSystem.m_bSystemEnable`, `+0x9978` = `m_bHyperOpticalResearch`, `+0x99a4` = `m_iJune1c0`, `+0x96b8` = `m_sEGQueensThrone.m_fPowerRate`, `+0x9a54` / `+0x9aac` = the Taser / Kugelblitz `m_bEnable`.

**Checks.** `cl /Zs` of `X2GuEve.cpp`: 0 errors, 0 warnings in the block; the `cl /P` output holds the block ( flag live ).
**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
**Build.** `DEPLOY OK: X2_offline.exe is 25,999,360 B, dated 2026-10-26 14:38:40, md5 110e8ca140fec75366f2ea457d9a45d9, equal to X2/US_SERVICE/x2.exe (2026-10-26 14:38:32); built at main 20d3622d + 2 uncommitted code file(s).` Nothing in game.

**JUNE_BUGS:** 432, 433. **Checklist:** `3297762a` ( 7, L277 ), `32978ec2` ( 7, L285 ), `329b77ad` ( 7c ) marked `(Eve: CH1l)`.
**Drift** ( scope rule ): the tutorial key mask `320708f0` ( InputData, per guide step; also called by `CX2GUUser::InitInpuDataProcess` `31fcea10` ) joins backlog Stage 9's guide-manager bullet ( l.480 ). **Open for the review:** the writer of the NPC byte `+0x8751`; the names of the four slots ( 0x65c / 0x6a8 / 0x6ac / 0x77c ). Nothing to play-test: the code is unreferenced. Review: `kickoffs/char-eve/CH1l-review.md`. Next: `/x2-phase char-eve CH1l review` ( Opus 5.5 high ), then CH1l2.

### CH1l - Review (2026-10-26, Opus)

**Verdict: PASS, one inert fix** under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`. All 11 listings (`charport_adis.py eve --phase CH1l`) were read against the block.

**Fix: `32979a70` (vslot 0x65c).** `+0x8751` is not a lone NPC byte. It is `CX2GUNPC::m_FlagList[0]`: June's `SetFlag_LUA` `31e4d280` and `GetFlag_LUA` `31e4d2a0` index `[this + i + 0x8751]`, `SetAllFlag_LUA` `327572a0` fills `+0x8751..+0x875a` (10 bytes), and the NPC ctor, `Init` and `CommonEventProcess` loop over it. March has the same member, so the stand-in `JuneEveNpcByte8751` (always false) is removed and the getter reads `GetFlag_LUA( 0 )`. The Nasod summons' Lua sets the flag, which is why no C++ writer exists. The Eve range has no writer (byte scan of the exe: writers `322ec9b1`, `32e957d6`, `32e95856` and the indexed setters, none in `0x32965dd0`-`0x329cf86f`). Three funcmap rows were added and the `32979a70` row was corrected.

**Confirmed against June:**
- **`32968f20`.** The `bChangeAll_` / id gate on all five blocks. The state ids match by name through June's registration strings in `329c8a00`: 0xd3-0xd5 SWEEP_ROLLING CHARGE / FIRE / END, 0xcd / 0xce BURST_SPEAR_Landing (+HYPER), 0xef-0xf1 ELECTRONIC_MOVE FRONT / UP / DOWN, 0xf2 REASON_PRIDE_ACTION. 0xde / 0xdf (Tri-Vulcan) have no registration in June (`june-dead-data`), and March's names keep the same relative slot. Passing March enumerators is the plan's rule ("CH1m maps March values to June ids"). The batch `3207c1a0` / `31fdf970` wraps `&m_StateList[ id ]`, and `JuneGetStateDataIterator` inserts the same way, so ByID is equivalent. Its comment "none when that state is not registered" is loose: the insert never misses. Eve does not override 0x7a8.
- **`32977390`.** Base `31fd8f60`, then three searches over 0-9 (A 0-4 / B 5-9, no `GetEnabledSkillSlotB`). Both Taser and Kugelblitz types take the first match, as March's own loops do. `31edf560` -> `32206000` (tree null false; equal, or the first equals the `+0x20` upgrade of the second). Only the Kugelblitz loop has the B lock, `32890ee0( 0 )` on MY unit, and it breaks (JUNE_BUGS 432). `m_iJune1c0` = 3 / 1 sits between Taser and Kugelblitz. Queen's Throne is the inlined `GetSkillPowerRateValue` (`32071720`: 0xad numerical row first) x ( `3201ad10` + 1.0 ). June drops March's Spectro El Crystal `ASUT_GROUND` write here (June behaviour wins; nothing to port).
- **`32978e80`.** The jump tables `329792cc` / `329792f4` / `3297931c` map case i -> body i, and the bodies' keys are C A S D Skill5 Q W E R SkillEx5 = slots 3 0 1 2 4 5 6 7 8 9. That is the same map as Arme's `31fc7ac0`. Taser and Kugelblitz pick their slot by `m_bSelectedSlot` (`+0x9a78` / `+0x9ad0`), which is `GetSkillSlotID()`. Out of range or not CanNotInput returns at `329791c2`. The tutorial block `32978ec2` stays out (backlog l.480). `329cc500` matches the helper.
- **`329b7390` / `329b4ac0`.** Four arguments; the base gets 0 (JUNE_BUGS 433). The factor index is `size > idx ? idx : 0`. Vslot 0x770 (slot 476) = `AdjustBuffFactorByIndex( clone, 25201 )`. Both `32d41910` calls are `( 1, v, true )` (`32206e50` takes 4 args, so the early `push 1` is bAdd) = `JuneSetTimeFinalizerValue( v, true )`. Loop: team, `GUT_USER`, < 490000, slot 0x210 with BTI 0, both `SetBuffFactorToGameUnit`, no party / PvP test. The particle is from `32205440`, whose whole table was decoded (200-206, 0x56ba / 0x56bf / 0x56c4 / 0x56cd-0x56cf, 0x6271 / 0x627e, 0x765d-0x765f), and the alpha tag is `32296d60( this, seq )`.
- **Small ones.** `32977090`: 0x6293 with 0 < block `+0xc` returns 0.0. `3298c580`: the arg is an NPC id; the gage test is equal to `GetNowHp() <= 0`; NAT_ALLY plus the ally UID. `3299aee0`: class 109, `+0x8b40` = `m_bSelfDestruction` (the same byte as CH1b's `3299d140`), owner == this. `3299cf70`: 0.05 / 0.83 / false. `32979a70`: NPC ids, threshold = SA_EFFECTIVE_RATE (5) of 25408, `threshold < hp rate` returns 0, else SA_CRITICAL_REL (0xc2). Enum values were checked with `enumprobe.py`.

**Hygiene.** CH1l commit `--range c6eceb93^..c6eceb93`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The fix (`--worktree --files X2Lib/X2GuEve.cpp`): `files: 1 PASS`, `7 deleted line(s)` (CH1l's own stand-in helper and its call, inside the flag), `0 encoding change(s)`, `RESULT: PASS`.
**Build.** `DEPLOY OK: X2_offline.exe is 25,999,360 B, dated 2026-10-26 14:53:27, md5 7a087ca51a04a0e1b6bc80f5ae3158c4, equal to X2/US_SERVICE/x2.exe (2026-10-26 14:53:19); built at main c6eceb93 + 1 uncommitted code file(s).` Nothing in game.
**Names** of 0x65c / 0x6a8 / 0x6ac / 0x77c stay unsettled (CH1o). User owes nothing (inert). Next: `/x2-phase char-eve CH1l2` (Sonnet 5.5 high); its kickoff still matches.

### CH1l2 - Done (2026-10-26)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (a fourteenth block at the end of `X2GuEve.cpp`, +123 lines; 11 declaration lines in `X2GuEve.h`). **All 9 class-map rows of phase CH1l2**: 8 overrides and the helper `32976fb0` are new `CX2GUEve::June_*` members. Nothing registers or calls them. 11 funcmap rows `Char Eve CH1l2`.

| June | member (`June_` prefix) | notes |
|---|---|---|
| `32965e20` ( 0x7c0 ) | CommonStateStartFuture | a bare jump to `CX2GUUser::CommonStateStartFuture` ( `320506f0` ) |
| `32965e30` ( 0x7c4 ) | CommonStateStart | base, then `m_fEffectStartTime` ( `+0x9ae4` ) = 0 |
| `32980f90` ( 0x7c8 ) | CommonFrameMoveFuture | ReAttackZ1 / ReDashJumpX run down and clamp; `iJuneClassSyncInt` ( `+0x7ec0` ) = `m_iManeuverCoreLv` ( `+0x9b9c` ) before the base ( March wrote `m_CannonBallCount` ); then the Code Recovery cool time ( `+0x9c64` ) runs down. The `322e65c0` call is left out for CH4c |
| `3297c390` ( 0x7d4 ) | CommonEventProcess | Mega Electronball ( slot id 0-9 ) and Taser Pilum start by `JuneIsEquippedSkillKeyPressed( 0x6277 / 0x627c )` and set `nowAction` ( `+0x7e84` ) to `UAI_MEGA_ELECTRONBALL` ( 7 ) / `UAI_TASER_PILUM` ( 17 ) |
| `3297a400` ( 0x7d8 ) | CommonStateEndFuture | base, then `m_bDownForce` = false unless the now state is `USI_DASH_JUMP` ( 21 ) |
| `32965e50` ( 0x7dc ) | CommonStateEnd | base, `m_fEffectStartTime` = 0, `+0x9b70` = 1 ( = March `m_bIsCheckEndElCrystal` ) |
| `32965e70` ( 0x7e8 ) | CommonRender_Prepare | the base, handed back ( the hidden RENDER_HINT pointer, `ret 4` ) |
| `329694a0` ( 0x7ec ) | CreateStepDust | `m_bAbsoluteInvisibility` returns, else the base |
| `32976fb0` | IsBlock9c50ValueCPositive | `m_pJuneBlock9c50` set and `0.0 < m_fValueC` ( name neutral ) |

**What differs from March:**
- **`3297c390`.** June has no slot/state lookup and no Kugelblitz branch here; the skill is found by `JuneIsEquippedSkillKeyPressed` ( `31fce710` ) on the skill id. The Mega block range-tests the slot id ( 0-9 ); the Taser block does not, and June calls `3288fe30` ( slot id -> index, slot B ) there with the result never read ( left out ). The `3376cdd0` batch around the `nowAction` write is June's anti-tamper re-protect ( the write is kept, with June's `!= value` test ).
- **`32980f90`.** `+0x9b9c` is the Nasod Core level ( ctor map: `m_pManeuverCore` `+0x9b98`, level `+0x9b9c`, protected gauge `+0x9ba0` ); the line sits under `ADD_EVE_SYSTEM_2014` ( off today, live with CH1q ), as March's does. The `322e65c0` call ( game singleton, args `( &true, !( cool <= 0 && GetNowMp() >= ConsumeMp ), cool )`, funcmap row ) is `// June 322e65c0: call added in CH4c`.
- **`32965e50`.** `+0x9b6c` / `+0x9b70` are March's `m_eReserveEndElCrystalBuffID` / `m_bIsCheckEndElCrystal` ( same order; `3297a440` writes the id ): two ctor-map rows re-labelled.
- **`3297a400`.** June's state index `( +0x7cbc - +0x7cec ) / 100` is the state id, so `GetNowStateID() != USI_DASH_JUMP` ( value 21 by `enumprobe` ).

**Offsets** ( `offsetprobe`, March layout, structure-relative ): Mega `m_bEnable` / `m_bSkillSlotSwap` / `m_iSkillSlotID` = +0 / +8 / +0x1c ( June `+0x9784` / `+0x978c` / `+0x97a0` ); Taser `m_bEnable` / swap / `m_bSelectedSlot` = +0 / +8 / +0x24 ( June `+0x9a54` / `+0x9a5c` / `+0x9a78` ). Ids come from the CH1l namespace constants; `UAI_*` and `USI_DASH_JUMP` are C++ enum members.

**Checks.** `cl /Zs` of `X2GuEve.cpp`: rc 0, no new warning.
**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
**Build.** `DEPLOY OK: X2_offline.exe is 25,999,872 B, dated 2026-10-26 17:16:31, md5 33c0bab564325be9c620e0734eba7ce8, equal to X2/US_SERVICE/x2.exe (2026-10-26 17:16:24); built at main cb76692d + 2 uncommitted code file(s).` Nothing in game.

**JUNE_BUGS:** none. **Checklist:** no CH1l2 rows. **Drift:** none. Nothing to play-test: the code is unreferenced. Review: `kickoffs/char-eve/CH1l2-review.md`. Next: `/x2-phase char-eve CH1l2 review` ( Opus 5.5 high ), then CH1l3.

### CH1l2 - Review (2026-10-26, Opus)

**Verdict: PASS, no code change.** All 9 listings (`charport_adis.py eve --phase CH1l2`) were read against the fourteenth `_CORE` block.

**Confirmed against June:**
- **`3297c390`.** Base `31fe7610` first. Mega: `+0x9784` gate, swap from `[g_pData +0x2cc]` -> `3214f050` -> `+0x115` written to `+0x978c` before the range test (March `X2GuEve.cpp:5488` makes the same call), `+0x97a0` in 0..9 (`js` / `jge 0xa`), key `0x6277`, then nowAction 7. Taser: `+0x9a54` gate, swap to `+0x9a5c`, `3288fe30( +0x9a78 ? +0x9a70 : +0x9a74, &idx, &bB )` with both outs unread (cdecl, out-params only: left out is right), no range test, key `0x627c`, nowAction 0x11. No Kugelblitz branch. The `3376cdd0` lookup feeds `32072ae0( &nowAction, &value, ... )`, the protected writer that stores the value, after June's decoded `!= value` test (16-bit compare): the transcription's guarded write is that. The literal `10` is `EQUIPPED_SKILL_SLOT_COUNT * 2` (a `static const int`, not an enum; March writes the product): style only.
- **`32980f90`.** Both clamps (`< 0` -> 0, the first by `test ah,0x41`, the second by parity); the sync write is `323101d0` after `decoded( +0x7ec0 ) != [+0x9b9c]`; base `32061bf0`; the cool run-down only when `0 < cool`, clamped at 0. `+0x9b9c` = `m_iManeuverCoreLv`: March declares `m_pManeuverCore`, `m_iManeuverCoreLv`, `KProtectedType<float> m_fManeuverGauge` in that order, matching the ctor map's `+0x9b98` / `+0x9b9c` / protected `+0x9ba0`. `ADD_EVE_SYSTEM_2014` is off (`Always.h:58` commented, no other define), so the line is dead until CH1q: CH1q's kickoff now names this site. The `322e65c0` args read `( &true, !( cool <= 0 && GetNowMp() >= ConsumeMp ), cool )`, as the funcmap row says; CH4c line 35 already lists the CH1l2 site.
- **`3297a400`.** `( decoded( +0x7cbc ) - +0x7cec ) / 100` (magic `0x51eb851f`, `sar 5`) against 0x15. Same reading as the shipped `JuneOverrideEffectAlphaCondition` (`X2GUUser.cpp:15410`), so `GetNowStateID()` is right. March's own `CommonStateEndFuture` is the bare base call.
- **Small ones.** `32965e30` / `32965e50` match, and March's `CommonStateEnd` sets `m_bIsCheckEndElCrystal = true` at the same place (`FIX_EVE_ELCRYSTAL_BUG` is defined, `Always.h:903`), which confirms the `+0x9b70` re-label; `3297a440` writes `+0x9b6c` = buff id and `+0x9b70` = 1 / 0, matching the other one. `32965e70` passes the hidden pointer through (`ret 4`). `329694a0` compares `+0x3e59` with 1. `32976fb0` returns `0 < m_fValueC` (C0 parity test); the name stays neutral. The class map's "called by 3297c390" is not borne out: that listing has no call to `32976fb0`.

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --range 9f70f36e^..9f70f36e`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `enumscan --check X2Lib/X2GuEve.cpp`: 0 A/B rows. No rebuild (no code change; deployed md5 `33c0bab5` stands).
**User owes nothing** (inert). CH1l3's kickoff still matches. Next: `/x2-phase char-eve CH1l3` (Sonnet 5.5 high).

### CH1l3 - Done (2026-10-26)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` ( a fifteenth block at the end of `X2GuEve.cpp`, +1113 lines; declarations at the end of the class and inside `MEGA_ELECTRONBALL_Data`, `TASER_PILUM_Data`, `EveElectraSystem` ), the commit "char-eve Phase CH1l3". Nothing registers or calls the members.
**Of the 26 class-map rows: 17 new `June_*` members, 7 resolved to March / existing code, 2 routed** ( a callee owned by a later group is never a bare declaration ). 34 funcmap rows `Char Eve CH1l3`.

| June | Member ( `CX2GUEve::` ) |
|---|---|
| `32969300` `329b5af0` `329693e0` `32981130` | `June_SpecialAttackEventProcess` / `_Lookup` / `_Use` / `_Cooldown` ( vslots 0x7f4 / 0x7f8 / 0x7fc / 0x800, steps 509-512 ) |
| `3297c570` `329b6810` | `June_SpecialAttackNoStageChange` ( 0x810 ) / `June_NoStateChangeActionFrameMove` ( 0x80c ) |
| `3297a6d0` `32976f30` `32968db0` | `June_Vslot828` ( name unsettled ) / `June_Vslot860` ( `JuneSummonedNPCInfo` ) / `June_IsPhotonBlinkSkill` |
| `32972480` `3297a800` | `June_CanUseChargingBooster` / `June_UseChargingBooster` |
| `3299f390` `329b3cc0` `3299ebd0` | `June_GetSkillSlotDataMegaElectronBall` / `June_GetSkillSlotDataTaserPilum` / `MEGA_ELECTRONBALL_Data::June_Reset` |
| `329b4b40` `329a0ae0` `329cc390` | `June_StartElCrystalSystem` / `EveElectraSystem::June_StartFormationEffect` / `EveElectraSystem::June_GetFormationFactor` |

**Resolved, no `June_` member:** `32972a70` = March `ChangePlaneFormation`, `329919a0` = `DestroyFormationMarkAndMsgParticle`, `3298d8d0` / `32994fd0` / `329978d0` = the three `ResetParticle`s, `329a7e70` = `TASER_PILUM_Data::Reset` ( same body ), `329cc370` = the existing `CX2GUUser::IsRidingKeySkillWaiting`.
**Routed ( class map + kickoffs amended ):** `3297baa0` ( cut-in by script, calls `3297b840` ) -> CH2a2; `3299ba70` ( Electronic Field summon, callers `329ae4f0` / `3299ecb0`; CH0's "called by `3299ebd0`" was a function-boundary slip ) -> CH3b; `329aea60` + `329cc200` ( Psyker Kinesis release ) -> CH3a, together with `32976fe0`: case 0x40 here and CH1l4's `329b4a60` leave those calls out.
**New, June-only:** fields `m_iJune8ea0` / `8ea4` / `8ea8` / `8eb0`, `m_abJune90d3[10]` / `90d0[3]` ( CX2GUUser state, Eve-local as the pilot's, zero in the constructor ); `MEGA_ELECTRONBALL_Data::m_bJune15` ( struct +0x15, in the padding ), `TASER_PILUM_Data::m_fJune34` ( +0x34, 1.0 ); Eve-local copies of seven CX2GUUser functions ( `32008e40` `31fc86e0` `31fc93e0` `31fd32e0` `31fdbe70` `3202d140` `3202d3c0` ); registered under `SERV_IRUHADEV_JUNE_ENUM_NAMES`: 10 `RNSI_*` ( 25200 / 25203 / 25213 / 25216 / 25219 / 25233 / 25068 / 25409 / 25420 / 25604 ), `BFI_BUFF_REASON_PRIDE_OBERON` / `_OPHELIA` / `_EVE` ( 1010 / 1012 / 1017 ), `BTI_BUFF_REASON_PRIDE_EVE` ( 961 ). Every id is written by name; `enumscan --check` is clean ( the ability ids 66 / 169 use the C++ `SA_MP_RETAKE_ABS` / `SA_TIME_ABS` ).

**What differs from March:**
- **Lookup ( `329b5af0` ).** Slots 0-9; Mega Electronball and Taser Pilum fire on the key release ( `31fce8b0` ), Mega also after 5 s; Spectro El Crystal's use condition is `m_iJune1c0` ( `+0x99a4` ), Photon Blink is an air skill with 25420; June's two ban checks and the riding-key wait; MP 0 for Spectro while the system runs and for Thousand Stars with a count; Conversion tests `+0x9c58` ( where March read an ability ); Link Overcharge Illusion's stone test uses BTI 409. The cool-time / MP / hyper block sits under the dev free-skill gate ( the pilot's form ).
- **Cooldown ( `32981130` ).** No MP for Mega / Taser; Open Mind ( 25419 ) refunds ability 66 x MP x `329cc390`; the Spectro cool-time table ( E/S/O ) equals March's; Thousand Stars ( 25028 / 25068 ) cool time 0; the Nasod gauge divides the reserved MP ( JUNE_BUGS 435, under `ADD_EVE_SYSTEM_2014` ); Charging Booster through `32972480` / `3297a800`; the rate hook `3235a710` and `31fcd3d0` last.
- **`329b6810`.** Thousand Stars needs an enemy lock-on and halves the count as `1 - (int)( n * -0.5 )`; no memo scaling of the Mega charge time; Taser stores ability 169 ( `+0x9a7c` ); the Spectro change calls `329b4b40` + `329a0ae0` ( alpha tag ); Kugelblitz and Ferdinand as March; case 0x40 ( Psyker Kinesis ) calls nothing until CH3a.
- **`329b4b40`.** BTI lists 50 / 51 / 105, no assert; the new start adds `m_fAddSpectrumTime`, runs `AdjustBuffFactorByIndex( clone, 25202 )` and sets `m_iJune1c0` = 3 with the hyper optical research ( March wrote the templet's use condition ).
- **`32976f30`.** NPC 1621 / 1622 with key code 2 take BFI 1010 / 1012 ( creator me, level 1, skip 0 ); I take BFI 1017 unless BTI 961 ( checklist 7ai ).

**Decisions and open items for the review:**
- `328941a0` ( `ResetLeftSkillCoolTimeAll( 2, { 13, 12 } )` ) is read as March's `ResetLeftSkillCoolTimeAll`; June's memo-swap type lookup, missing training branch and UI index + 4 differ ( JUNE_BUGS 436 ). Not verbatim; ask the user.
- Seven CX2GUUser functions now exist as Eve-local copies ( the pilot has the same seven ): a CH1a2-style shared home is the user's call.
- The dev free-skill gate is wired here ( the pilot's form ), not in CH1n.
- `SetSkillCoolTimeLeft` is the March 2-argument form ( June's third argument 1 ); `GetEqippedSkillMemo( 605013 )` is June's stub ( false ), the memo branch of `329b4b40` stays dead.
- CH0 corrections: the checklist 7an reader `329b407b` sits in `329b4060` ( CH1q ); `32207210` = `CX2SkillTree::GetUpgradeBaseSkillID`.
- Checks: `cl /Zs` of `X2GuEve.cpp`: rc 0, also with `ADD_EVE_SYSTEM_2014` defined; `enumprobe` values of every registered name.

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `flagcheck.py JUNE_ENUM_NAMES --worktree --files X2Lib/X2SkillTree.h X2Lib/X2BuffFactor.h`: `0 PASS, 2 PASS~, 0 FAIL, 0 ERROR`, same zero counts, `RESULT: PASS`.
**Build.** `DEPLOY OK: X2_offline.exe is 26,012,160 B, dated 2026-10-26 18:19:39, md5 4b5625f8b4a6c9103f5b817dc2d86b82, equal to X2/US_SERVICE/x2.exe (2026-10-26 18:19:32); built at main 43066238 + 4 uncommitted code file(s).` Nothing in game.
**JUNE_BUGS:** 435 ( Nasod gauge line ), 436 ( `328941a0` UI index ). **Checklist:** 7 ( spec A ), 7c, 7ai, 7an rows annotated. **Drift:** none beyond the routed rows. Nothing to play-test: the code is unreferenced. Review: `kickoffs/char-eve/CH1l3-review.md`. Next: `/x2-phase char-eve CH1l3 review` ( Opus 5.5 high ), then CH1l40.

### CH1l3 - Review (2026-10-26, Opus)

**Verdict: PASS**, with one user-decided port and two precision fixes, all under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`. All 24 `charport_adis` listings were read against the block.
- **`329b5af0`.** The QuickSlot wait ( `+0x8d78` ), the Mega release ( `+0x9784`, `31fce8b0` ) or 5 s ( `clock()` / 1000 = `CKTDXTimer::elapsed` ), `32207210` = 25207 needs the release, Taser release ( `+0x9a54` ) with `31edf580( slot, Taser )`, text 0x8bae on a swap mismatch; use condition `+0x99a4` for 25202 / 25215, 2 ( `ASUT_AIR` ) for Photon Blink with 25420; the five ban / riding checks in order; MP 0 for Spectro with `+0x97e4`, Thousand Stars with a count; `min( left, actual )`. The dev gate wraps exactly June's `0x82c`-skipped block ( cool 226, MP 2549, stone 26119 ). Queen's Throne, Conversion ( 2627 / 2662 ), `+0x9b98` state 5 ( dead `ADD_EVE_SYSTEM_2014`, CH1q ).
- **`32981130`.** Jump table: no MP for 25207 / 25212 / 25219 / 25233; Open Mind refund `ability 66 x MP x 329cc390`; stone; the cool time when `>= 0`; Spectro 0, then the E/S/O table ( E0: S0O0 actual, else 0; E1: O 0.5, S 2.0, else none ) gated on `+0x9980` / `+0x804b` ( March's own Spectro sites read `bNoChangSpectro` ); Thousand Stars 0 with an ungated UI update; JUNE_BUGS 435 line; `+0x8ac0`; Charging Booster + timer copy; `3235a710` then `31fcd3d0`.
- **`329b6810`.** Jump table `329b72f4` / `329b732c` gives actions 7 / 8 / 12 / 13 / 14 / 15 / 17 / 18 / 19 / 23 / 24 / 27 / 0x40, all equal to March's `UAI_*` values; Thousand Stars enemy-team test and `1 - (int)( n * -0.5 )`; Taser `+0x9a7c` = ability 0xa9 default 1.0; Mega fail clears `+0x978d`; case 0x40 calls nothing ( CH3a ).
- **Small ones.** `3297c570` ( returns false for Charging Booster ), `3297a6d0` ( June 154-157 / 214-217 = the eight March names by `ch0_state_skill.tsv`; numeric ids are CH1m's ), `32976f30` ( June pushes creator before level: March `( id, level, creator, adjust, skip )` = `( 1010 / 1012, 1, this, 0, false )` and `( 1017, 1, NULL, 0, false )` ), `329b4b40` ( `+0x9b68` = `m_bStartSpectroElCrystal`, `FIX_SPECTRO_EL_CRYSTAL_ACTIVE_BUG` is defined ), `329a0ae0`, `3299f390` ( no reset on a null slot ) vs `329b3cc0` ( reset on every miss ), `3299ebd0`, `329cc390`, `32969300` ( conditions 3 / 5 restore ), `329693e0` ( word `+0x9b56` = the short `m_iBoostAttackCount` ).
- **Values.** The subagent `cl` probe matched every enum the block uses ( SKILL_ID, ASUT, ST, SA, BTI, BFT, UAI, PFT, NUI ). `ApplyExternalFactorToFinalizer`'s June third argument 1 has no March parameter ( March takes two ).

**Fixes.** (1) User decision: `328941a0` ported whole as `CX2UserSkillTree::June_ResetLeftSkillCoolTimeAll( mode, vector )` ( `X2UserSkillTree.h/.cpp` ). Mode 1 keeps ids, mode 2 keeps types through `GetUpgradeSwappedSkillID` + `GetUserSkillTemplet`, mode 3 keeps all while `g_pMain` state is listed, an empty vector resets all, and B's UI index is `i + 4` ( JUNE_BUGS 436 now verbatim ). `June_UseChargingBooster` calls it with `( 2, { ST_HYPER_ACTIVE_SKILL, ST_RELATIONSHIP_SKILL } )`. (2) The x87 order: Conversion `(double)GetMaxHp() * +0x9c58`, Open Mind `(double)ability * MP * factor` ( June multiplies before rounding ).
**User decision 2:** the seven Eve-local `CX2GUUser` copies get one shared home: new phase **CH1l5** ( after CH1l4, before CH1m; `kickoffs/char-eve/CH1l5.md`; CH1m now depends on its review ).
**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --range 20e9e61e^..20e9e61e`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `JUNE_ENUM_NAMES` same range: `0 PASS, 2 PASS~, 0 DEFINE, 2 OTHER`, same zero counts, `RESULT: PASS`. The review's worktree: `2 PASS, 1 PASS~`, `4 deleted line(s)` = CH1l3's own inert lines ( comment, call, two formulas ), `RESULT: PASS`. `enumscan --check`: 0 A/B.
**Build.** `DEPLOY OK: X2_offline.exe is 26,013,184 B, dated 2026-10-26 19:41:35, md5 cf5797c17ac14f356a22525c3a0bef78, equal to X2/US_SERVICE/x2.exe (2026-10-26 19:41:28); built at main 20e9e61e + 3 uncommitted code file(s).` Nothing in game: the code is unreferenced. `kickoffs/char-eve/CH1l40.md` still matches. Next: `/x2-phase char-eve CH1l40` ( Opus 5.5 high ).

### CH1l40 - Done (2026-10-26)

Opus helper phase, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`, the commit "char-eve Phase CH1l40". Nine funcmap rows `Char Eve CH1l40`; every kickoff address has a March name, a body, or a recorded reason it needs none.

| June | March home | What |
|---|---|---|
| `321db1b0` tags 20 / 5 / 12 / 16 | `CX2EffectSet::ApplyDamageEffectModifiers` (new cases) | 20: `m_bInheritCriticalRate = true` (`+0xad4`), falls into 5: CRITICAL_RATE added (`b == 1`) or multiplied, NEXT tables take the result (JUNE_BUGS 437). 12: `m_bInheritIgnoreStateDefence = b`, IGNORE_STATE_DEFENCE (own + NEXT) = `v > 0`. 16: `m_bInheritIgnoreDefence = b`, IGNORE_DEFENCE and `_ON_PVP` (own + NEXT) = `v > 0` |
| `31f649d0` / `31f65e40` / `31f65f90` | C6b's `CEffect::JuneApplyInheritDamageRate` / `JuneSetIgnoreDefenceDown` / `JuneSetIgnoreStateDefenceDown` | already ported; `CEffect` now declares `friend class CX2EffectSet` and `friend class CX2GuEve` (end of its `JUNE_SPAWN_INHERIT` block) so the effect set and Eve's callers reach them and the gates |
| `31f8ae10` | `CXMeshInstance::SetManualDirDegree` (existing) | ECX is the child's main effect, not the CEffect (CH0 slip); the `+0x494 = 1` after it is `UseManualDirDegree( true )` |
| `32371c00` | `CEffect::SetAttackTime` (existing) | `+0x840` = `m_AttackTime` (ctor ATTACK_TIME_START `@31f7d5ed`, END `@31f7d720`; DamageData ends at `+0x840`) |
| `31f8adb0` (bonus, CH1l4 unknown) | `CXMeshInstance::GetScale` (existing) | `+0x510` = `m_vScale`, the field `SetScale` `31c1a440` writes |
| `321cdbb0` / `321cd590` / `3264e800` | none: Stage 12 | customize item struct ctor / dtor and `CX2CustomizeManager::GetItemData`; the only user is `3297b840 @3297b908` (the skill cut-in item override, CH2a2), not `3297b720` (CH0 slip). Same call as Aisha CH5 for `32cc9680`; the Stage 12 bullet names Eve's site |

**Tags are numbers on purpose.** June's `Enum.lua` has no modifier-tag table, no Lua calls `AddDamageEffectModifier`, and March has no enum, so they stay numbers with a one-line comment each (rule: no Enum entry). `enumscan --check` on both files: 0 rows.
**Drift / routed.** Eve pushes tag 15 (base scale) from Spit Fire (`3299ed30` / `3299f060`, CH3b) and Spit Fire Grenade (`329a9df0` x5, CH3d); it needs the base member and June's multiplying `SetScale` (checklist 7a, C3 review note). Routed to the Opus helper **CH3a0**, kickoff amended. Kickoffs CH1l4 (March names for every callee) and CH2a2 (leave the override out, Stage 12 marker) amended. Checklist 7a rows annotated.
**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2Lib/X2DamageEffect.h X2Lib/X2EffectSet.cpp`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
**Build.** `DEPLOY OK: X2_offline.exe is 26,013,696 B, dated 2026-10-26 20:00:03, md5 bca72610d5b224378f8964447b6a65d5, equal to X2/US_SERVICE/x2.exe (2026-10-26 19:59:56); built at main 93045203 + 2 uncommitted code file(s).` (compiled by a peer's coalesced build: `X2EffectSet.obj` 19:53, `X2DamageEffect.obj` 19:52, both newer than the edits at 19:50). Nothing in game: no ported class pushes tags 5 / 12 / 16 / 20 yet.
**JUNE_BUGS:** 437. Review: `kickoffs/char-eve/CH1l40-review.md`. Next: `/x2-phase char-eve CH1l40 review` (Opus 5.5 high).

### CH1l40 - Review (2026-10-26)

**Verdict: PASS, no code change.** Read against June's disassembly of `321db1b0` (jump table `321dbc60`: `tag - 1` in 0..0x16; 20 -> `321db4ba`, 5 -> `321db4c1`, 12 -> `321db998`, 16 -> `321dbb01`).
- **Tags 20 / 5.** `@321db4ba` stores `+0xad4 = 1` and runs straight into tag 5. Tag 5: `CMP byte [entry], 1`, `JNZ` -> `FMUL`, else `FADD`; v is `+0x8`. The result is rounded through a float temp, `FST`'d to `+0x710` and left on the stack, then stored into `+0x510` of every non-NULL `+0xd8` entry. March matches: `m_bAdd` is that byte, the result is a `const float`, `JuneGetNextTables` drops NULLs, and JUNE_BUGS 437 is stated correctly.
- **Tags 12 / 16.** `FLDZ` (loop top) then `FCOMP v`; `TEST AH,5` / `JP` gives DL = 1 only for C0 set and not unordered, i.e. `0 < v` (NaN -> false, as March's `0.f < v`). `+0xb44` / `+0xb47` = the entry byte; `+0x741` (DD `+0x541`), `+0x728` / `+0x729` (DD `+0x528` / `+0x529`) and the same offsets of each NEXT entry = DL. March matches member for member.
- **Friends.** Inside `SERV_IRUHADEV_JUNE_SPAWN_INHERIT`, which `_CORE` already needs (`JuneGetNextTables`, the gates); access stays `protected` after them. `m_bInheritCriticalRate` (`_PARSE` block) is reached through the same friend.
- **Names.** `31f8ae10` stores `+0x498..+0x4a0`, `RET 0xc`; at `@329c393b` ECX = EBP (`MOV EBP,[ESP+0x20]` @329c3888, the `31f6eb30` result saved @329c3702), at `@329c3b43` ECX = EBX (`31f6eb30` @329c3a7f), both followed by `[reg+0x494] = 1`. Main effect confirmed. `32371c00` stores `+0x840` / `+0x844`, `RET 8`; ECX = EBP = `31f8ff00`'s child @329c0e60; the ctor's `LEA EDI,[ESI+0x840]` @31f7d5ed follows the decrypt of `3379c480` = `ATTACK_TIME_START` and `+0x844` @31f7d720 follows `3379c470` = `ATTACK_TIME_END` (xorstrings.tsv). `31f8adb0` copies `+0x510..+0x518` out (`RET 4`), ECX = EBX = `31f6eb30` @329c0e69; `31c1a440` writes the same three dwords. All confirmed.
- **Customize struct -> Stage 12: agreed.** `321cdbb0` has 24 callers, `3264e800` has 16: the same cut-in override block in every class's cut-in helper (Aisha `32cc9680`, Eve `3297b840` only, at `@3297b917` / `@3297b937` / `@3297b978`). The name is read at struct `+0x2c` (`ESP+0xa0` = struct `ESP+0x74` + `0x2c`). CH2a2 needs only the marker its amendment names.
- **Tag 15 -> CH3a0: agreed, amendment completed.** Spit Fire / Grenade call `321dad70` directly with a vector (`3299ef92: PUSH 0xf`), and March has only the float wrapper. CH3a0's kickoff now asks for the vector overload too, and its STATUS Touches column now lists `X2EffectSet.*` / `X2DamageEffect.*`.
- **CH1l4 amendment refined.** `3297b720` is an equipped-skill ability test (`32007ce0`), not a memo test. Per table it is guarded by `JuneIsAttackTrigger( 0x27 )` and pushes `0x27` onto `m_vecAdditionalAttackTriggerType`. Kickoff updated; the three ids need Enum names.

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --range b9c7d4e8^..b9c7d4e8`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. No build: no code changed (deployed md5 stays `bca72610`). Next: `/x2-phase char-eve CH1l4` (Sonnet 5.5 high).

### CH1l4 - Done (2026-10-26)

Sonnet phase, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`, the commit "char-eve Phase CH1l4". 35 class-map rows: 29 inert members (28 helpers + the `CommonFrameMove` override `June_CommonFrameMove`, vslot `0x7cc`), 6 resolved to March. 36 funcmap rows `Char Eve CH1l4` (the extra row is the NPC `327623f0`). The members are the last `_CORE` block of `X2Lib/X2GuEve.cpp` (namespace `_CONST_EVE_JUNE_CH1l4_`: the filtered-name tests, `JuneEveCopyLockOnData`, `JuneEveSetLockOnNearstTarget`); `X2GuEve.h` declares them at the end of the class and inside `ENERGY_NEEDLES_Data`, `EveElectraSystem`, `EveElectraLaserData`, `EveElectraEffectData`, `EveElectraLockonData`.

| June | Home | What |
|---|---|---|
| `329c5bc0` | `June_CommonFrameMove` | base `CommonFrameMove`; Open Mind floor ( skill 25415, `SA_TIME_ABS`, clock `+0x9c44` -> `m_fManeuverGauge` at least 30, under `ADD_EVE_SYSTEM_2014` ); the Nasod Watch / Atomic Shield / Ferdinand / Oberon / Ophelia cleanup is now gated by `HaveThisBuff` ( 409-413 ) instead of the lifetime timers; cloak; Queen's Throne meshes; Mega Electron Ball and Taser Pilum ( elapsed x `m_fJune34` ); tails `32996c50`, `329986e0`, `329ae9a0`, `3299b000`, `3297b370`, `3299b790`, `329b4a60` |
| `329bc6e0` | `EveElectraLaserData::June_OnFrameMove` | March's laser with `June_LaserAttackLineSet`, `June_ApplyProjectileModifiers`, tag-alpha, `m_bDie` cleared on every live path; induction / fusion fire the June names ( `+0xc8` / `+0xe4` ) with ranges 1600 / 800 ( 1500 / 400 in the Giga Stream ), x0.5 in PvP |
| `329c12a0`, `329c3420`, `329c3bb0` + plane / giga / formation | `EveElectraEffectData` / `LockonData` / `EveElectraSystem::June_*` | effect-set path ( two new fields `m_hJuneEffectSet`, `m_vecJuneEffectSetHandle` ), lock-on copies of the Code Recovery child, the June plane, Giga Stream and formation steps |
| `3297b370` `3297b720` `3297bad0` `32968c50` `32975ac0` `32979350` `3297a500` | `June_UpdateDashXTargetUnit` ... `June_PlaySoundInMoveStates` | `CEffect +0x650 / +0x648` = `DamageData::hitUnitList / bReAttack`; the inherit-critical helper reads ability 0x33 of 25411 |
| `329a3040` `329a3760` `329b2f60` `329a7d00` `329b4a60` | Thousand Star pair, `June_DoFrameMoveThousandStar`, `June_ChangePlaneForamtionSystem`, `June_UpdateBlock9c50Timer` | the two CH3a calls of `329b4a60` are left out with a comment |
| `329673a0` `32991ac0` `32994cd0` `329cc4e0` `329cd740` `329cb2f0` | none ( March ) | same bodies as March's `GetBuffTempletIdByPlaneFormation`, `DestroyGigaPlaneAndCrystalEffect`, `ChangeThousandsOfStarsFormation`, `ResetTaserPilumTinyData`, the compiler-made `~EveElectraLockonData`, and `CXMeshInstance::SetRotateLocalDegree` |
| `327623f0` | `CX2GUNPC::June_SetBasicScale` ( `X2GUNPC.*` ) | NPC vslot `0x270`; JUNE_BUGS 438 |

**Registered** ( `SERV_IRUHADEV_JUNE_ENUM_NAMES`, explicit June values ): `RNSI_P_ECB_HYPER_OPTICAL_RESEARCH` 25411 and `JUNE_MEMO_ID` ( `X2SkillTree.h` ); `BTI_BUFF_CODE_RECOVERY_ECB` 672, `BTI_BUFF_QUEENS_CONTROL` 786, `BTI_BUFF_SENTRY` 410, `_ATOMIC_SHIELD` 411, `_OBERON` 412, `_OPHELIA` 413 and `BFI_BUFF_QUEENS_CONTROL` 812 ( `X2BuffFactor.h` ); `DTT_EVE_HYPER_OPTICAL_RESEARCH_CHECK` 39 ( `X2DamageManager.h` ). `friend class CX2GUEve;` added to `CEffect` ( the CH1l40 line `friend class CX2GuEve` names no class, so it never took effect ). `enumscan --check` on `X2GuEve.cpp/.h`, `X2GUNPC.cpp`: 0 A/B rows.
**Kept as June has it:** the 7c alpha tags ( 30 sites, all present ), JUNE_BUGS 438-441. **Left out on purpose:** `329b5230` ( Nasod Core frame move, CH1q ), the memo test `31fca9e0` ( false in June, the cloak's memo branch is dropped ), March's Kugelblitz block ( not in June's `CommonFrameMove` ), `DoParrying` writing `+0x9c44` ( CH1q ). The charging-booster pointer June reads at `+0x95ec` is March's `m_ChargingBoosterSkillDataBasePtr` ( null until a later phase sets it ).
**Open for the Opus review:** the engine mappings made by shape ( `June_SetBasicScale`, the hit-unit list, the `EffectSetInstance` rotation field name, the Thousand Stars templet receiver and the factor behind its cool-time floor ); `+0x9c44` starts at 0 here ( June's constructor value unchecked ); `SFI_SPEED_13` is June's 11 by `ev2.py`.
**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files` over the 8 touched headers and sources: `files: 3 PASS, 2 PASS~ (blank/marker only), 0 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR` ( the 3 OTHER are the enum headers, checked under their own flag ); `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `JUNE_ENUM_NAMES` over the three enum headers: `RESULT: PASS`. `cl /Zs` of the whole `X2GuEve.cpp` with and without `ADD_EVE_SYSTEM_2014`: no error in the new block ( the 6 errors with it defined are in earlier phases' code, `X2GuEve.cpp` lines 43843-44345, not in this block ).
**Build.** `DEPLOY OK: X2_offline.exe is 26,072,576 B, dated 2026-10-26 21:45:05, md5 3af7f3a87548e4b2c45ae30d2b41dc86, equal to X2/US_SERVICE/x2.exe (2026-10-26 21:44:59); built at main 7a7a3daa + 8 uncommitted code file(s).`
Nothing in game: nothing calls the members until CH1. **JUNE_BUGS:** 438-441. Review: `kickoffs/char-eve/CH1l4-review.md`. Next: `/x2-phase char-eve CH1l4 review` ( Opus 5.5 high ).

### CH1l4 - Review (2026-10-26, Opus)

**PASS, two transcription fixes** (flag `_CORE`, `X2GuEve.cpp` only, 3 lines): **(1)** June's constructor `3299bd70` sets `+0x9c44` to `clock()` ( `@3299c93b`, import `344c0270` = `clock`, the same one `329c5bc0` subtracts from ); the port started it at 0, which arms the Open Mind floor at once instead of after `SA_TIME_ABS` seconds -> `m_iJuneClock9c44 = ::clock()`. **(2)** Taser charge test ( `329c7b6d`-`329c7ba2` ): June multiplies the double elapsed by `m_fJune34`, rounds once to float and compares with the double `m_dMaxTime`; the port rounded twice and cast the max to float -> one cast around the product, double compare.
**Checked against the listings** ( `charport_adis.py eve --phase CH1l4`, three Opus forks plus own reads ):
- `329bc6e0` laser: `m_bDie` set only at `329c124f` ( empty vector / no instance / no main effect, the 5 handles freed ), 0 on every other exit; jump tables `329c1270` / `1280` / `1290` decode to the port's cases ( June compiles Giga induction / fusion as two identical blocks, the merged case is equal ); ranges 1600 / 800 ( `334c9cd4` / `334c9cd0` ) x0.5 when `g_pMain +0x8c == XS_PVP_GAME`, after the 0.75 PvP special rate; Giga 1500 / 400, no PvP scale; `+0xb10` written after the lock-on uid and seed, before `DestroyInstanceSilently`; call order LineSet -> ... -> `June_ApplyProjectileModifiers` last in all 5 penetrate blocks. The Taser rate is not in this function ( `push 0xa9` is in `329c5bc0` only ).
- `329c5bc0`: base call first; Open Mind ( 25415 level > 0, hyper time > 0, ability 169 <= elapsed, gauge floor 30 ); cleanup "no buff -> destroy" 410 -> 384 + 519, 411 -> 682, 409 -> 1454, 412 -> 1621, 413 -> 1622 ( all values compile-probed equal to June's ); cloak, Queen's Throne, Mega Electron Ball, Charging Booster and Taser in June's order; tails `329c3bb0`, `329b2f60`, Taser, `32996c50`, `329986e0`, [`329b5230` CH1q], `329ae9a0`, `3299b000`, `3297b370`, `3299b790`, `329b4a60`; no Kugelblitz in June.
- Plane / Giga / formation / lock-on / Thousand Star ( `329c3bb0` `3299f470` `3299fe60` `32991090` `329c3420` `329b2f60` `329a3040` `329a3760` `329a7d00` ): PASS, constants and compare directions as ported.
**Engine mappings by shape, all confirmed:** `327623f0` = `June_SetBasicScale` ( rate < 0 exits, per-component compare before the protected write, second scale under the bool, vslot 0x26c = `TransformScale`, `ret 8`; JUNE_BUGS 438 holds ). CEffect `+0x648` / `+0x64c` / `+0x650` = `bReAttack` / `fHitGap` / `hitUnitList` ( March's order; `31e56d20` is `HitUnit`'s copy constructor, 12 bytes ). EffectSetInstance `+0x30` = `m_vRotationDegree` ( funcmap row 1324 puts `m_vPosition` at `+0x24` ). `32070510` is not called in `329a3760`: it sits in `329a3040 @329a3696` and `32979350 @329794b2`, receiver = the equipped Thousand Stars templet ( `GetEquippedActiveSkillTemplet( 25028, true )` ); its floor is 5.0, or ability `SA_COOLTIME_1` of 25420 x5.0 when 25420 is learnt, category != 5 and `m_EnergyNiddleData.m_fMaxTime > 0` ( `+0x9b00` ) - as ported.
**Fields:** June's constructor sets `+0x9770` 1.0, `+0x9c28` 0, `+0x9c2c` 0 ( as ported ), `+0x9c44` `clock()` ( fixed ); `+0x9c10` is a container the constructor does not write. `EveElectraEffectData`: `3297b6b0` sets `+0x8c` -1 and an empty vector; `329cdf80` copies both, which the implicit copy constructor does. `SFI_SPEED_13` compiles to 11 in March ( `SFI_SPEED_6` is preprocessed out ), June's 11. `31fca9e0` is `xor al,al; ret 4`.
**Notes, no change:** the Open Mind block stays under `ADD_EVE_SYSTEM_2014` ( undefined, so it does not compile; June runs it unconditionally on a protected gauge ) -> CH1q's Nasod Core decision. Mesh scale products ( `329a3040` / `329a3760` ) are rounded to float before the factor in June, the port multiplies in one expression: last-bit scale difference, no compare reads it. June calls `322d7670` with 6 arguments ( LockOnData NULL ): same filter as March's 4-argument form. `32975ac0` tests its arguments for NULL, so `329b2f60` passing a NULL main effect is safe. The CH1l40 line `friend class CX2GuEve` declares an unrelated class; harmless, CH1l4's `CX2GUEve` line is the live one.
**Hygiene.** `flagcheck JUNE_CHAR_EVE_CORE --range 7ca59966^..7ca59966`: `files: 3 PASS, 2 PASS~ (blank/marker only), 0 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `JUNE_ENUM_NAMES` same range: `files: 0 PASS, 3 PASS~ (blank/marker only), 0 DEFINE, 5 OTHER (other flags), 0 FAIL, 0 ERROR`; `RESULT: PASS`. Review fix `--worktree --files X2Lib/X2GuEve.cpp`: `files: 1 PASS`, `3 deleted line(s)` ( the three replaced lines, inside the flag's own June-only block ), `RESULT: PASS`. `enumscan --check X2GuEve.cpp/.h X2GUNPC.cpp`: `0 A/B row(s)`.
**Build.** `DEPLOY OK: X2_offline.exe is 26,072,576 B, dated 2026-10-26 21:59:58, md5 274b0abd1c15d8fe9240eac59c0024b0, equal to X2/US_SERVICE/x2.exe (2026-10-26 21:59:51); built at main 7ca59966 + 1 uncommitted code file(s).`
Nothing in game. User owes nothing. Next: `/x2-phase char-eve CH1l5` ( Sonnet 5.5 high ).

### CH1l5 - Done (2026-10-26)

The seven June `CX2GUUser` skill-use helpers and their six fields now live once, in `X2GUUser.h` / `.cpp` (protected, non-virtual), flag `SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS` (`Always.h:10150`, `#error` unless `SERV_IRUHADEV_JUNE_ENUM_NAMES`, defined at `Always.h:10140`).
- Members: `June_UserNoStateChangeActionFrameMove` (32008e40), `June_MarkSkillSlotUsed` (31fc86e0), `June_ReserveSkillUse` (31fc93e0), `June_StateChangeBySkillSlot` (31fd32e0), `June_SkillMemoMpRefund` (31fdbe70), `June_CheckSkillUseNotPve` (3202d140), `June_CheckSkillUseNotDungeon` (3202d3c0). Fields `m_iJune8ea0` / `m_iJune8ea4` / `m_fJune8ea8` / `m_fJune8eb0` (init list) and `m_abJune90d3[10]` / `m_abJune90d0[3]` (constructor body, before the first `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` block).
- Diff of the two copies first: Aisha's and Eve's bodies agree line for line; the only differences are names (`June_PushInfoText` / `JuneEvePushInfoText`, `JUNE_CH1I_UAI_*` / `JUNE_CH1L3_UAI_*`) and Aisha's `#else` literals under `JUNE_ENUM_NAMES`. Nothing to stop on.
- Constants in `_CONST_GUUSER_JUNE_CH1l5_` (user action ids 0x25 / 0x29 / 0x36-0x39, the PVE game type 4 which March lacks, the info-text helper). The memo test now reads `CX2SkillTree::MI_REGENERATE_1` (June's 10) instead of the literal.
- Retired with `#ifndef` / `#endif SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS`: Aisha's declarations, constructor zeroing and three body regions; Eve's declarations, fields, constructor zeroing and the one body region. Callers unchanged. Flag-off is the old tree.
- **For later character plans:** CH1 kickoffs call the `CX2GUUser` members and never copy them (also in `kickoffs/june-backlog/stage7-plan.md`). `funcmap.tsv` has 7 new rows.
- Probe (`cl /P`, US_SERVICE defines, subagent): in `X2GUArme_VioletMage.cpp` and `X2GuEve.cpp` the seven names have 0 class-qualified definitions and 0 class-local declarations, the six fields 0 class-local declarations; each has one declaration, in `CX2GUUser`.
- Hygiene: `flagcheck.py JUNE_USER_SKILL_USE_HELPERS --worktree --files <7 files>`: `files: 6 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `enumscan.py --check` on the three `.cpp`s: 0 A/B rows.
- Deploy: `DEPLOY OK: X2_offline.exe is 26,071,040 B, dated 2026-10-26 22:18:39, md5 f9fa8d34fe4df7c3e432b26b250d3f5f, equal to X2/US_SERVICE/x2.exe (2026-10-26 22:18:32); built at main 5509a8c1 + 7 uncommitted code file(s).`
- **Play-test (Aisha, her skill use is live):** see the hand-off; log tag `JUNE_CHAR`. Eve's copies were inert, nothing of hers to test.
- Next: `/x2-phase char-eve CH1l5 review` (Opus 5.5 high), then CH1m.

### CH1l5 - Review (2026-10-26)

**Verdict: PASS after two fixes** (same flag; `X2GUUser.cpp`, plus `X2Game.h` under `SERV_IRUHADEV_JUNE_ENUM_NAMES`).
- **Copies:** a name-normalised diff of the seven bodies (Aisha's, Eve's, the new `CX2GUUser` ones) differs only in `MI_REGENERATE_1` (`X2SkillTree.h:2052`, June 10) and the PVE constant. Six fields: the scalars are in the `CX2GUUser` init list, the arrays are zeroed in the constructor body, and Aisha's and Eve's zeroing is compiled out. Header placement is protected (`X2GUUser.h:4125`). Flag-off is the old tree.
- **Against June** (charport_adis listings + Ghidra; the files are in the session scratchpad):
  - `31fc86e0`: June writes `+0x90d8 + (i-5)` for slots 5-9, which is `+0x90d3 + i`, so it matches.
  - `31fc93e0`: `+0x8eac` = skill and `+0x8eb4` = 1.0 is `ReserveSkillUseCount` (`X2GUUser.h:3360`), then `ea8 = eb0` and `eb0 = 0`.
  - `31fd32e0`: the four 5-entry tables are `33472040` / `054` / `068` / `07c` (normal A, hyper A, normal B, hyper B).
  - `31fdbe70`: type 10, enable flag, `GetRandomFloat(0) <= chance`, `UpNowMp(rate * mp)`.
  - `32008e40`:
    - 0x25 erases BTI 420; 0x29 erases 436 / 926, then `SetForceInvincible(1)`.
    - 0x37-0x39 call vslot 0x21c with `(824/825/827, creator 0, level 1, 0, skip 1)`, which is March's `(BFI, 1, NULL, 0, true)`.
    - Case 0x29's vslot `0x854` is the empty `31f3f070` for Eve too (`ch0_vtable_all.tsv`), as for Aisha. **Later class ports:** check that slot in your own vtable.
  - `3202d140`: correct for a game type other than 4 and for no game.
- **Fix 1 (transcription, in all three copies since CH1i):**
  - `3202d3c0` has its bit-`0xd` test inverted. At `@3202d490`, `je 0x3202d46a` returns "usable" when `31edf8f0(dungeon, 0xd)` is false, and June refuses skill 900 only when the bit is set or g_pData `+0x70` is null.
  - The port refused whenever the bit read false. With the agreed stand-in (false), it now never refuses (`if( false == bJuneDungeonBitD ) return true`). Stage 22 owns the bitset.
  - The retired copies keep the old line. They are compiled out.
- **Fix 2 (enum rule):** June's `Enum.lua` names game type 4 `GAME_TYPE.GT_PVE`. It is now registered as `, GT_PVE = 4` after `GT_NUMS` in `CX2Game::GAME_TYPE` (`X2Game.h:71`, so the count stays 3), and the body compares `CX2Game::GT_PVE`. The `JUNE_GAME_TYPE_PVE` constant is gone.
- **Routed to CH1o:** June's base slot 514 `31fc9410` clears `+0x8eac` and `+0x8ea8` together. With `ea8` now in `CX2GUUser`, hoist that clear into the base `ClearSkillUseReserve`; otherwise Eve's live skill use keeps its reserved MP (Aisha's play-test fix 1, 2026-10-21; her override is at `X2GUArme_VioletMage.cpp:35321`).
- **`cl /P` probe:** the Done note holds it (0 class-local definitions).
- **Hygiene:**
  - Range `flagcheck.py JUNE_USER_SKILL_USE_HELPERS --range 70db3b35^..70db3b35`: `files: 6 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
  - Worktree fixes: `JUNE_ENUM_NAMES` on `X2Game.h` PASS (0 deleted). `JUNE_USER_SKILL_USE_HELPERS` on `X2GUUser.cpp` PASS; its 5 deleted lines are this flag's own CH1l5 lines. `enumscan --check X2GUUser.cpp` finds 0 A/B rows.
- Deploy: `DEPLOY OK: X2_offline.exe is 26,071,040 B, dated 2026-10-26 22:36:45, md5 15d5acdf1e4f9b6174d2697b0498b1de, equal to X2/US_SERVICE/x2.exe (2026-10-26 22:36:39); built at main 70db3b35 + 2 uncommitted code file(s).`
- **In game:** Aisha's skill use goes through these bodies. Skill 900 (the couple skill) is no longer refused in dungeons. Nothing else changes.
- Next: `/x2-phase char-eve CH1m` (Opus 5.5 high).

### CH1m - Done (2026-10-26)

**Landed** (flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`; the call sits behind the new `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE`, added to `Always.h` commented out):
- `June_InitStateCommon` (`329b8030`, 91 registrations) and `June_InitState` (`329ba9c0`, 43), whole, at the end of `X2GuEve.cpp` (block `//{{ ... CH1m`).
  `CX2GUEve::InitState` runs `June_InitState()` under `_LIVE`, March's body in the `#else`.
- **How it was derived.** The C++ is generated from `ch0_reg.tsv` (callbacks through the header `/// June` comments and funcmap), then checked two ways:
  `june2017_re/char_eve/ch1m_regsim.py` walks both decompiles once per class (5 18 19 26 108 109 116; `ch1m_reg_by_class.tsv`, 904 rows), and
  `ch1m_regcheck.py` parses the C++ per class: **771 registrations match slot by slot, 0 problems** (hooks and routed ids excluded).
- **Gates, June's:** unit-data class @312 / @368 / @418 (18 / 108 `COMBO_ZZZdownZZ`, 19 / 109 `COMBO_XXZ4`, 26 / 116 the two EPR combos); `XXfrontXX`,
  `XXdownXX` and the eight EBS states for every class; templet class (`+0xaf0`, null-tested as June) @995 (18 / 108 dash combos + `EEG_DASH_JUMP_ZX / ZZX`,
  26 / 116 EPR dash combos, 109 EP dash combos), @1105 (108 NS + ECN, 109 EP + ECE, 116 `ECB_DASH_XX`), @1233 (19 / 109 `DASH_JUMP_COMBO_XZ`, `DASH_COMBO_ZX`).
  June wins: `COMBO_ZZZdownZZ` is now 18 / 108 only (March also gave it to 19 / 109).
- **Ids by name** through March's own registration of each table: 134 June ids map one-to-one (`ch1m_idmap.tsv`). 72 = `USI_RIDING_START` (March writes
  `GetRidingStartStateID()`), 180 = `ENSI_JUNE_DASH_COMBO_ZX`. The numbers stay March's, as the pilot decided in CH1j: offline nothing reads them, and every
  `June_*` body names its states. This settles the enumerator question CH1d / CH1e / CH1g routed here.
- **Callbacks:** every target resolves to a `June_*` member (CH1a-CH1l4) or a `CX2GUUser` member (March's, or Aisha's CH1j / CH1k ones: `JuneStart*`,
  `JuneWin/LoseStateStart`, `JuneRandomLoseEventProcess`, `JuneDamageSmall/Big*`). No stand-ins. Hyper id 23 takes `JuneGetHyperModeTableName( false )`
  (no Init / CameraMove slot in June, as March); its FrameMove `32adf320` = vslot 0x600 = `June_HyperModeFrameMove`.
- **New `CX2GUUser::JuneHyperModeEnd`** (`3201b8e0` = `32006c00`, flag `_CORE`, needs `JUNE_TRANSCEND_ENGINE`): the base hyper and riding-hyper StateEnd Eve
  inherits (vslot 0x604 via `329cc0e0`; vslot 0x608 via the Aisha-folded `32cf0fa0`). March's `HyperModeEnd` lacks its middle block: when the `+0x952c`
  pending byte is set, `ShowMinorParticleHyperModeTrace` + `ApplyHyperModeBuff` (the transcendence T5 note left these base readers to the class ports).
- **Hooks:** `InitStateCommonRandom` (event states `3202b110` + roll `31fe22d0`), `JuneInitWaitStateChange( "ENSI_WAIT_STATE_CHANGE" )` (7h),
  `JuneInitBoundingStates( "ENSI_" )` (7l). WIN2 / LOSE2 flags as June (`@329baead` / `@329baf14`). Tail: `InitPassiveSkillState` (March's until CH4a
  ports `3297f290`), `June_InitEquippedActiveSkillState( false )`, `CX2GUUser::InitState`.
- **Left out, comment lines with owners:** phoenix run 123-126 (PvP), dance 127-129 (Stage 10), riding 93-96, the `RIDING_ATTACK_X` frame move `32002f70`
  and the riding-die end (Stage 17), die end `3202a7d0` (already run in `StateChange`), parrying 224-226 -> CH1q, recovery 238 -> CH4c (both kickoffs amended).
  The March-only `_HYPER` states (`OBERON_GUARD`, `ATOMIC_SHIELD`, `ILLUSION_STRIKE`) are registered in March's `SetEquippedSkillFuncBySkillSlotIndex`, so
  CH1n's head drops them. **JUNE_BUGS:** none new (`RIDING_DAMAGE_BACK` is row 422; the event states' tables live in `CommonSkillState.lua`).

**Checks.** `cl /Zs` (US_SERVICE defines): `X2GuEve.cpp` with `_LIVE` off and on, and `X2GUUser.cpp`: 0 errors each. `cl /P` with `_LIVE`: both registrars,
the call and `JuneInitBoundingStates( "ENSI_" )` are present; `JuneHyperModeEnd` is present in the `X2GUUser` `.i`. `enumscan --check X2GuEve.cpp X2GUUser.cpp`: 0 A/B.

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2GuEve.cpp X2GuEve.h X2GUUser.cpp X2GUUser.h`: `files: 3 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
OTHER is the `_LIVE` pair in `InitState`. `_LIVE` reports `FAIL: the flag is not #defined anywhere tracked`, by design until CH1 final (as Aisha's CH1j).
Commits `541b4795` (the phase) and `a8776d67` (the `Always.h` LIVE-needs-CORE guard rewritten as nested `#ifdef` / `#ifndef`: flagcheck's unifdef reports ERROR on the compound test, so each per-commit range pair errs on it; no compiled change, no rebuild). `flagcheck.py JUNE_CHAR_EVE_CORE --base 541b4795^ --files <the 5 files>`: `files: 3 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (the second OTHER is `Always.h`).

**Deploy:** `DEPLOY OK: X2_offline.exe is 26,100,224 B, dated 2026-10-26 23:08:17, md5 2438acc35dfbdb8315332d2c08b8990c, equal to X2/US_SERVICE/x2.exe (2026-10-26 23:08:10); built at main c5ac3186 + 7 uncommitted code file(s).`

**In game:** nothing changes (`_LIVE` is undefined). Next: `/x2-phase char-eve CH1m review` (Opus 5.5 high).

### CH1m - Review (2026-10-27)

**Verdict: PASS, no code change** (no rebuild; the deployed `2438acc3` stands).

- **Path simulation.** Fresh Ghidra decompiles of `329ba9c0` / `329b8030` through `ch1m_regsim.py` reproduce `ch1m_reg_by_class.tsv` row for row (904);
  `ch1m_regcheck.py`: `matched 771 registrations, 0 problems; ids mapped 134`.
- **Gates, against the listing:** the unit-data tests `@329baf1f` (0x12 / 0x6c), `@329bb02f` (0x13 / 0x6d), `@329bb118` (0x1a / 0x74), each `JNZ` past
  its own registrations. Templet switch `@329bbbb8`: the byte table `329bc4f4` / targets `329bc4e4` (read from the exe) give 0x12 / 0x6c -> block 0,
  0x1a / 0x74 -> 1, 0x6d -> 2, everything else (19 and 5 too) -> `329bbebb`, past the trailing call; null templet = class 0 = default, as the C++.
  Chain `@329bbed2`: `SUB 0x6c` / `DEC` / `SUB 7` = 108 / 109 / 116, else `329bc2c2`. Joined test `@329bc2c2`: templet non-null and 0x13 or 0x6d only.
- **Head:** cut-in table `329bc474` / `329bc460`: 5 / 18 / 19 -> 0, 108 -> 1, 109 -> 2, 116 -> 3, 26 falls to the default (no store); sub-index
  `+0x8acc` 18 -> 1, 19 -> 2, else 0; `+0x4088` / `+0x7d04` = 0xd = `InitStateID`. Order `329b8030`, `3202b110`, `31fe22d0` (`32078bc0` is the
  stateData constructor). START (Start / EventProcess / End), WIN (Start, `320718d0`), LOSE (Start only), 0x82, then WIN2 / LOSE2 compares on the rolled
  names, bytes `+0x97e0` / `+0x97e1` set only. Tail vslots `0x690`, `0x684( 0 )`, `31fd8f00`.
- **`JuneHyperModeEnd`:** `3201b8e0` and `32006c00` are the same 21 instructions (only the jump target differs): vslot 0x7dc, protected byte `+0x952c`
  (`[+0x9530] ^ &+0x952c ^ [+0x9534]`) == 1 -> `31ffda00` + vslot 0xc0, `+0x8d44 = 0`. Eve's vtable holds them at 0x604 / 0x608 (`ch0_vtable_all.tsv`);
  `329cc0e0` = `8b 01 ff a0 04 06 00 00`, `32cf0fa0` = `... 08 06 00 00` (exe bytes; Ghidra has no function there).
- **Ids:** `ch1m_idmap.tsv` 134 rows, no duplicate on either side; 72 = `USI_RIDING_START`, 180 = `ENSI_JUNE_DASH_COMBO_ZX`.
- **Left out:** the comment lines and owners match the kickoff; the die StateEnd is `SERV_IRUHADEV_JUNE_BUFF_DEATH_CLEAR` in `StateChange` (DIE_FRONT / BACK only).

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --range 541b4795^..541b4795`: `files: 3 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 1 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL`
(the ERROR is the compound `#if` in that commit's `Always.h`, rewritten in `a8776d67`). Base-to-now `--base 541b4795^ --files <5 files>`:
`files: 3 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`; same zero line; `RESULT: PASS`.
`JUNE_CHAR_EVE_LIVE --range 541b4795^..541b4795`: `files: 0 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 4 OTHER (other flags), 0 FAIL, 0 ERROR`; zero line; `RESULT: FAIL`
(`the flag is not #defined anywhere tracked`, by design until CH1 final). `enumscan --check X2GuEve.cpp X2GUUser.cpp`: 0 A/B.

User owes nothing. Next: `/x2-phase char-eve CH1n` (Opus 5.5 high).

### CH1n - Done (2026-10-27)

**Landed** (flag `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`; the call sits behind `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE`, still commented out). Funcmap rows `(CH1n)`.
- **`CX2GUEve::June_SetEquippedSkillFuncHead`** = `329c8a00` up to its id switch, Aisha's CH1k shape (block `//{{ ... CH1n` at the end of `X2GuEve.cpp`).
  Under `_LIVE`, `SetEquippedSkillFuncBySkillSlotIndex` calls it in place of March's head and type switch (`#else`); `true` = the id switch is still March's.
  - Bounds 0..4; normal tables `334c9e00` / `334c9e28` (A / B), hyper `334c9e14` / `334c9e3c`, read from the exe: 50.. / 60.. and 51.. / 61.., March's four
    arrays (CH0's note had normal / hyper swapped). `31fd2af0` dropped (7i, the engine's), `+0x11` = `IsJuneSkillSlotState`, batch `3207c1a0` / `31fdf970` none.
  - Transcendence memo (`31fdbcc0`, switch on the raw memo id): 25033 / 25070 -> `HORNET_STING_SHAPED_CHARGE`, 25034 / 25071 `ENERGY_NEEDLES`,
    25035-36 / 25072-73 `THOUSANDS_OF_STARS`, 25217 / 25234 `SPIT_FIRE_GRENADE`; any other memo id registers nothing.
  - Types: 5 `ENSI_SI1_COMMON_BUFF` (+`_HYPER`), 0xc + 900 `ENSI_THROW_ITEM` (CH1a's June members), 6 / 7 / 0xd-0x10 on the raw id: 25600 / 25603 Lunatic Scud,
    25601 / 25604 Link Overcharge, 25602 / 25605 Psychic Artillery, `> 25600` Faena, else the id switch. Hyper slots as the listing: Link Overcharge's hyper
    takes Start + FrameMove / EventProcess / End (`@329ca906`, then `329c9be9`; `ch0_skill.tsv` lists Start only), Lunatic Scud's EventProcess only.
  - Callbacks: the memo / hyper-active bodies are CH3c / CH3d's; until then their March pairs (class map column 4) stand in, each with a `/// June <va>, CH3x` line.
- **`June_SetFaenaSkillFunc`** (`@329ca9d1`, the default of both June switches): table `ENSI_A_EAT_FAENA`, `JuneGenericActiveSkillEventProcess` (`31fcc9d0`),
  hyper copies. Its Init `32968da0` / FrameMove `329c4380` are June-only and CH2b's (kickoff amended). Under `_LIVE` March's id-switch `default:` (which falls
  into Illusion Strike) calls it instead; March's skill cases, with the March-only `_HYPER` states, stay until their skill phases.
- **`CX2GUUser::June_CopyStateCallbacks`** (`329cc2c0`), new flag `SERV_IRUHADEV_JUNE_USER_COPY_STATE_CALLBACKS`: one home for Aisha and Eve, as CH1a2 / CH1l5
  did; Aisha's private copy is under `#ifndef`. The head does not call it; Eve's skill cases do (`ch0_skill.tsv` column `calls`; CH2a-CH3d kickoffs amended).
- **Enum names** (`JUNE_ENUM_NAMES`, `X2SkillTree.h`): the 10 memo ids and Lunatic Scud / Psychic Artillery (+`_UPGRADE`), June's names and values; March's
  `SI_HA_ENS_LUNATIC_SCUD` is 5600, no value shared.
- **Dev free-skill gate:** `329c8a00` has no cool-time / MP / hyper-stone check; the gate is CH1l3's in `June_SpecialAttackLookup` (`329b5af0`). Checklist §7
  L257, §7b L321 and 7i marked `(Eve: ...)`. **JUNE_BUGS 442:** the memo Energy Needles' hyper gets no StateInit (June rewrites the normal's, `@329ca048`).

**Checks.** `cl /Zs` (US_SERVICE defines): `X2GuEve.cpp` with `_LIVE` off and on, `X2GUUser.cpp`, `X2GUArme_VioletMage.cpp` (ARME_LIVE off and on): 0 errors each.
`cl /P` with `_LIVE`: the head call, both `June_SetFaenaSkillFunc` calls and the head body are present. `enumscan --check X2GuEve.cpp X2GUUser.cpp`: 0 A/B.
`ch1m_regcheck.py`: `matched 771 registrations, 0 problems; ids mapped 134`.

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_CORE --worktree --files X2GuEve.cpp X2GuEve.h`: `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (OTHER = the `_LIVE` pairs).
`JUNE_USER_COPY_STATE_CALLBACKS --worktree --files <Always.h, X2GUUser.*, X2GUArme_VioletMage.*>`: `files: 3 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`; same zero line; `RESULT: PASS`
(OTHER = a peer's uncommitted `MP_JUMP_DEBUG` lines in `Always.h` / `X2GUUser.cpp`, not committed here). `JUNE_ENUM_NAMES --files X2SkillTree.h`: `files: 0 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; zero line; `RESULT: PASS`.
`JUNE_CHAR_EVE_LIVE`: zero line; `RESULT: FAIL` (`the flag is not #defined anywhere tracked`, by design until CH1 final).
After commit `e702d70e`, `--range e702d70e^..e702d70e`: `JUNE_CHAR_EVE_CORE` `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 7 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`;
`JUNE_USER_COPY_STATE_CALLBACKS` `files: 4 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`; `JUNE_ENUM_NAMES --files X2SkillTree.h`
`files: 0 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS` (unscoped it errs only on CH1l5's compound `#if` in `Always.h`); each with the zero line.

**Deploy:** `DEPLOY OK: X2_offline.exe is 26,103,808 B, dated 2026-10-27 05:37:19, md5 aeeacdf37aedbbd4b1df3f2d19c330a7, equal to X2/US_SERVICE/x2.exe (2026-10-27 05:37:13); built at main 3d566ea1 + 11 uncommitted code file(s).`

**In game:** nothing changes (`_LIVE` is undefined; Aisha's copy helper is now the base's, same body). Next: `/x2-phase char-eve CH1n review` (Opus 5.5 high).

### CH1n - Review (2026-10-27)

**Verdict: PASS, no code change** (no rebuild; the deployed `aeeacdf3` stands).

- **Head, against the listing:** bounds `0 > id || 4 < id` before `3207c1a0`; normal `334c9e00` (A) / `334c9e28` (B) = 50.. / 60.., hyper `334c9e14` / `334c9e3c`
  = 51.. / 61.. (exe bytes; equal to Aisha's `3351b05c` / `3351b184` / `3351b170` / `3351b198`, = March's interleaved `USI_SPECIAL_ATTACK_*`); `31fd2af0` on the
  normal only (dropped, 7i); hyper `+0x10` / `+0x11` = `0x101`; `31fca8e0( id, 1 )` and `32206450( memo, 1 )` as Aisha's reviewed head.
- **Memo switch** `@329c8b4a`: `memo - 25033`, `JA` past 0xc9; byte table `329caac8` (0xca entries) / targets `329caab4`: 25033 / 25070 -> 0, 25034 / 25071 -> 1,
  25035-36 / 25072-73 -> 2, 25217 / 25234 -> 3, the rest -> `329caa72` (nothing). Bodies slot by slot: Hornet / Spit Fire hyper `+0x10 = 1` then FM + EP
  (`329c8df4` / `329caa5a`); Energy Needles rewrites the normal Init (`puVar2[5..6]`, JUNE_BUGS 442) then hyper Start / FM / EP / End; Thousands of Stars FM + EP.
- **Type switch** `@329c8ca0` (byte table `329caba4` / targets `329cab94`, `type - 5`): 5 -> buff, 6 / 7 / 0xd-0x10 -> actives, 0xc -> throw item, 8-0xb and
  the rest -> nothing; March's enum gives 5 / 6 / 7 / 12 / 13 / 14-16. Actives: `> 25600` switch (25601 / 25604, 25602 / 25605, 25603, default `@329ca9d1`
  Faena), `== 25600` Lunatic Scud, else the id switch (its default is also `329ca9d1`); the C++ order (cases first, then `> 25600`) is equivalent.
  Link Overcharge's hyper: Start, then `329c9be9` / `329c9bf5` = FM, EP, End. Lunatic Scud EP only. Psychic Artillery `+0x10 = 1`, FM, EP. 900 -> `ENSI_THROW_ITEM`.
- **Callbacks:** all 31 label thunks resolved from the exe bytes (`E9` jumps) to the addresses cited in the code; the 24 memo / hyper-active targets match
  `char_eve/ch0_class_map.tsv` column 4 one for one; buff FM `32a77a20` (vslot 0x5f0, CH1o), buff EP `31fcbd60`, throw item `329cc0d0` / `329cc170`, Faena EP `31fcc9d0`.
- **Call site:** March's head and type switch are the `#else`; nothing follows March's id switch, so the head's early return skips no code.
- **Hoist:** `329cc2c0` copies dwords `+0x14..+0x58` = the nine `GUStateCBFunc` members after `m_bHyperState` (`+0x10`); Aisha's ten call sites resolve to the base.
- **Enum names:** the 14 values equal `Enum.src.lua`; no other member of `SKILL_ID` sits in 20000-26999 except the RNSI block, so no value is shared.
- **Routed:** between CH1 final and CH2b an equipped skill with no March case lands on June's default, Faena with EventProcess only (Init / FM are CH2b's);
  `kickoffs/char-eve/CH1.md` amended so its play-test expects it.

**Hygiene** (`--range e702d70e^..e702d70e`). `JUNE_CHAR_EVE_CORE`: `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 7 OTHER (other flags), 0 FAIL, 0 ERROR`; `RESULT: PASS`.
`JUNE_USER_COPY_STATE_CALLBACKS`: `files: 4 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR`; `RESULT: PASS`.
`JUNE_ENUM_NAMES --files X2Lib/X2SkillTree.h`: `files: 0 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `RESULT: PASS`.
`JUNE_CHAR_EVE_LIVE`: `files: 0 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 8 OTHER (other flags), 0 FAIL, 0 ERROR`; `RESULT: FAIL` (`the flag is not #defined anywhere tracked`, by design).
Each: `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`.
`enumscan --check X2GuEve.cpp X2GUUser.cpp`: `0 A/B row(s) remain in 2 file(s)`. `ch1m_regcheck.py`: `matched 771 registrations, 0 problems; ids mapped 134`.

User owes nothing. Next: `/x2-phase char-eve CH1o` (Opus 5.5 high).

### CH1o - Done (2026-10-27)

Wiring under `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (still undefined), one new engine flag, two small CORE / HELPERS additions. The commit is "char-eve Phase CH1o".

**1. The virtuals.** 38 March definitions in `X2GuEve.cpp` now run their `June_*` member under LIVE, with March's body whole in the `#else` (generated from the file's bytes, 76 pure insertions):
- lifecycle: ctor (`June_ConstructorInit`), dtor (`June_Destructor`), `ReInit`, `InitComponent`, `InitEffect`, `InitEnchantWeaponEffectUnitSpecific`, both particle arrays, `DeleteMinorParticle`;
- reactions / render: `DamageReact`, `Win` / `Lose` / `Draw`, `AttackResultPlayEffect`, `AttackResultByType`, `AddUnitSlashData`, `ApplyRenderParam`, `ApplyRenderParamOnEmptyRenderParamByBuff`, `HyperModeBuffEffectStart`, `HyperModeFrameMove` (0x600);
- `COMMON_BUFF_FrameMove` (0x5f0, the CH1n head's registration), `DashStartFuture`, `InitEquippedActiveSkillState`, `InitInpuDataProcess`, `SetEquippedSkillLevel`;
- `Common*` x7 + `CommonRender_Prepare`, `CreateStepDust`;
- skill use: `SpecialAttackEventProcess` (509, which reaches 510 / 511 / 512 directly), `SpecialAttackNoStageChange`, `NoStateChangeActionFrameMove`;
- `ParseCommonRandomState` (pure virtual in `CX2GUUser`): empty under LIVE, June rolls the variants in `June_InitState` (Aisha precedent).
- `GetComboZStateID` (header inline) returns `June_GetComboZStateID()`.
- **0x4b4 `GetComboXStateID` is an Eve override too:** `32cb6a20` (`mov eax, 0xac` = 172 = `ENSI_COMBO_X`) is a folded stub outside the class range, which CH0's in-range scan missed. March's inline stays. A whole-vtable scan for entries that differ from User's and point outside the range finds only this one (`ch0_vtable_slots.tsv` footer).
- No `June_*` body calls its own wrapped virtual. `June_COMMON_BUFF_FrameMove` -> `CommonFrameMove()` (vslot 0x7cc) and `June_InitEffect` / `June_InitComponent` -> the particle / slash virtuals are intended.

**2. June-only user vslots with a March hook already (Aisha's CH1l):** under `EVE_LIVE`, Eve overrides `JuneSummonedNPCInfo` (0x860 -> `June_Vslot860`) and `JuneReservedSkillStart` (32057b80 @32057c4c-cb2: `June_SpecialAttackCooldown( id, +0x8ea8 )`, then `June_SkillMemoMpRefund( id, slot-state byte +0x11, +0x8ea8 )`; the byte is `JuneEveIsSkillSlotState`, `_CONST_EVE_JUNE_CH1o_`, the same tables as `X2GUUser.cpp`'s static). The base virtuals (`X2GUUser.h`) and both call sites (`CountReservedSkillUse`, `CX2Game::SetUserSummonedNPCInfo`) existed only under `ARME_LIVE`. They are now also compiled for `EVE_LIVE` without `ARME_LIVE` (nested `#ifdef` / `#ifndef`, pure insertion).

**3. Routed gaps.**
- **PlayEffectSet force-down float (CH1h decision).** New flag `SERV_IRUHADEV_JUNE_EFFECTSET_FORCE_DOWN_RATE` (`X2EffectSet.*`):
  - the game-unit `PlayEffectSet` takes the trailing `float fForceDownValueRate = 1.f` (`321e1e60`);
  - `EffectSetInstance::m_fJuneForceDownValueRate` (`+0x14c`, ctor 1.0 @321df48f, set @321e2017);
  - every damage effect the set creates gets it as `CreateInstance`'s `fForceDownValueRate`. June does this in all three branches (pet @321ef3bb, normal / hyper @321ef462), with `fLandPos` 0, `bReserve` 0 and `fReserveTime` -1.0 (`[0x334380f0]`); March's five calls are switched.
  - The eight Eve calls pass `m_fForceDownValueRateElectronBall` (`+0x97c4`, read in every listing). These are dash Z-X `32986d30`, Empress dash ZZ / ZZZ / XXdownX `3298e860` / `3298ea30` / `3298eff0`, and both jump Z-X pairs (`3298ec00`, `329998f0`, set + grenade). The old comment line stays in the `#else`.
- **Base slot 514 (CH1l5 review).** Under `SKILL_USE_HELPERS`, `CX2GUUser::ClearSkillUseReserve` is virtual and clears `+0x8eac` and `+0x8ea8` together (`31fc9410`). Aisha's override is kept: it is redundant and harmless.
- **Checklist §7 L256 / L259:** the quick-slot prefix and the riding-key wait are in `June_SpecialAttackLookup`, which now runs under LIVE; both rows are marked `(Eve: CH1o)`.

**4. A gap found here: `June_ConstructorInit` lacked June's stores** `+0x97b0` (protected 1.0, @3299ca5c), `+0x97bc` 0 (@3299cb15), `+0x97c0` 1.0 (@3299caf9) and `+0x97c4` 1.0 (@3299cb05). Without them, the LIVE ctor would leave four Electron Ball rates unset. They are added (CORE), plus two labelled stand-ins that March's ctor sets: `m_fElectronBallRangeRate` 1.0 (June's slot is unmapped) and `m_fSummonedLifeTime` 0 (March-only).

**5. Split: CH1o2 (new sub-phase).** These seven overrides need a base virtual and a June call site in March code before Eve can override them:

| Slot | Why it needs its own sub-phase |
|---|---|
| 0x3ac | June's `( DD*, float&, float& )`. June's `GetFinalDamage` @322857fb sends every type but AT_SPECIAL there; March has a 4-arg form, AT_NORMAL only, with no `DamageData`. |
| 0x7b0 | June passes a 4th argument, the index. |
| 0x56c | Skill-slot icon override, `328e0aa0` @328e0ee4. |
| 0x6a8 | Shown cool time, the skill-slot UI `322e07e0`. |
| 0x65c | The NPC crit getter `3275a820` adds the owner's slot. |
| 0x584 | Target filter, `32f0e260`; the base is `!HaveThisBuff( 984 )`. |
| 0x6ac | `CX2DungeonGame::SubStageStart` @327cfb79. |

- Bases and callers are in `ch1_offsets.tsv` (vslot rows) and `kickoffs/char-eve/CH1o2.md`.
- 0x77c (callers only in Eve's CH2a / CH3b states) and 0x828 (the idle-timer reset `+0x91ac` @31fc87d0 / @3206f99f, Stage 9 as for Aisha) need no hook.
- The CH4a / CH4b / CH1q / CH2a2 overrides stay with those phases.
- CH1p now depends on the CH1o2 review.

**Checks.** `/Zs` (`census.cl_cmd`) of `X2GuEve` / `X2GUUser` / `X2Game` / `X2EffectSet`: 0 errors with LIVE undefined and with `/DSERV_IRUHADEV_JUNE_CHAR_EVE_LIVE`. `enumscan --check` (4 files): `0 A/B row(s) remain`.
**Hygiene** (`--worktree --files`, the 8 code files):
- `JUNE_EFFECTSET_FORCE_DOWN_RATE`: `files: 2 PASS, 0 PASS~, 1 DEFINE, 5 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s) ... 0 encoding change(s)`, `RESULT: PASS`;
- `JUNE_CHAR_EVE_LIVE`: `3 PASS, 5 OTHER, 0 FAIL`, `0 deleted line(s)`; its only FAIL is the by-design "not #defined";
- `JUNE_CHAR_EVE_CORE`, `JUNE_USER_SKILL_USE_HELPERS`: `RESULT: PASS`, 0 deleted.
**Build.** `DEPLOY OK: X2_offline.exe is 26,105,344 B, dated 2026-10-27 06:54:23, md5 4f95ffbfc6bf9826dc1e0bc121226d15, equal to X2/US_SERVICE/x2.exe (2026-10-27 06:54:16); built at main a798331e + 8 uncommitted code file(s).`

**JUNE_BUGS:** no row. **Drift:** June's pet `PlayEffectSet` (`321e2340`, called by `321e2510`) writes `+0x14c` from its own trailing float; March's `PlayEffectSetByPet` lacks it. It only matters if a pet set passes a non-1 rate, and goes to the pet owner (backlog Stage 17 bullet). **Open for the review:** whether any other March member the still-March paths read is left unset by `June_ConstructorInit`; the dtor's March-only frees.
**In game:** nothing (LIVE undefined). Review: `kickoffs/char-eve/CH1o-review.md` (Opus 5.5 high).

### CH1o - Review (2026-10-27, Opus): PASS, no logic change

Two citation fixes and no code change. The commit is "char-eve Phase CH1o review".

- **38 wraps:** each calls its own `June_*` with every argument in order (`ReInit` / `SetEquippedSkillLevel` checked against the declarations) and returns its result (`CommonRender_Prepare`, both `SpecialAttack*`). Every March body is whole in the `#else` (`X2GuEve.cpp`'s side of the commit is 76 pure insertions plus the force-down pairs). `ParseCommonRandomState` is empty; `GetComboZStateID` is inline.
- **Header vs `ch0_vtable_slots.tsv`:** every `virtual` in `X2GuEve.h` is a June override, the dtor, 0x4b4, `ParseCommonRandomState` or the new hook. The one exception, `CreateNotEnoughMPEffect`, is dead (`#ifndef SERV_9TH_NEW_CHARACTER`, defined). 0x4b4: `32cb6a20` is `b8 ac 00 00 00 c3`, 172 = `ENSI_COMBO_X` in `ch1m_idmap.tsv` (0x4b0 165 = `ENSI_COMBO_Z`). March's inline stays.
- **Constructor:** the March body sets nothing that `June_ConstructorInit` leaves unset. The March-only names are the initializer list (still runs under LIVE), the `#ifndef USER_SKILL_USE_HELPERS` fields (CX2GUUser's ctor sets them) and the `m_Effect*` pointers (dead `#else` of `X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE`, KTDX.h:328). The six stores match `@3299ca40-cb15` (FPU stack `[0, 1.0]` from `@3299ca24`): `+0x97c0` / `+0x97c4` 1.0, `+0x97bc` 0, and the protected `+0x97b0` = 1.0 through the setter `@3299ca7a-caf0`. **Fixes:** the code comment cited `@3299ca5c` (that is the `+0x97a8` store), and `ch1k_ctor_offsets.tsv` gave `+0x97ac` 0, but it is 1.0 (FST of ST0 after the FXCH). `+0x97ac` stays unmapped. A 1.0 next to the Electron Ball rates fits March's `m_fElectronBallRangeRate`, which is not proven, and the stand-in is 1.0 either way.
- **Destructor:** `June_Destructor` (`329aece0`, deleting dtor `329cf670`) frees everything March's does: ThousandStar, `ClearManeuverCore( true )`, both mesh arrays, particles, the four summons, sounds, the Electra handles, and the maneuver-core effects. The only heap members, `m_pThousandStar` and `m_pManeuverCore`, go through those clears. Nothing is March-only (no Aisha-style `m_pStrongMindData`).
- **Force-down float:** `321e1e60` stores its last stack argument at `+0x14c` (`@321e2017`; `RET 0x3c` = return slot + March's 13 + 1). The ctor sets 1.0 (`@321df48f`, in `321df230`). Both `CreateInstance` sites of `321ee600` push the same tail: pet `@321ef3bb` and normal / hyper `@321ef462`. That tail is `fLandPos` 0, `bReserve` 0, `fReserveTime` -1.0 (`[0x334380f0]` = `00 00 80 bf`), `+0x14c`, `fLifeTimeRate` 1.0, `iHitCount` -1, `fHitGap` -1.0, mesh 0, `fHitAddMp` -1.0, and two false bools. Angle = move-axis = the rotation. This is March's five calls exactly. Spot-check `3298e860` (Empress dash ZZ, `@3298e947-e9c4`): vScale (1,1,1), three zero vectors, lifetime -1.0, `bCustomPosition` 0 and `+0x97c4` last, the same as CH1o's call. The pet overload `321e2340` writes `+0x14c` too (`@321e2430`, one caller `321e2586`): the drift note stands.
- **Hooks:** `31fc9410` = `fldz; mov [ecx+0x8eac], 0; fstp [ecx+0x8ea8]; ret` = the HELPERS base. `@32057c4c-cb2` = `0x800( 0x524(), +0x8ea8 )`, then `0x764( 0x524(), 0x528( 0 ), +0x8ea8 )`, then `0x808`. That equals Eve's override, which is Aisha's shape. `JuneEveIsSkillSlotState` = `X2GUUser.cpp:16223` table for table. The 0x810 skip is not ported, as for Aisha. The base virtuals and both call sites compile for `EVE_LIVE` alone and once with both LIVE flags (nested `#ifndef`).
- **Routing:** `CH1o2.md`'s table matches the Done note: 0x3ac, 0x7b0, 0x56c, 0x6a8, 0x65c, 0x584, 0x6ac; 0x77c and 0x828 need no hook. It needs no change.

**Hygiene** (`--range 3c7c9b7e^..3c7c9b7e`). Every flag has `0 deleted line(s), 0 non-ASCII ..., 0 encoding change(s)`:
- `JUNE_CHAR_EVE_LIVE`: `3 PASS, 0 PASS~, 0 DEFINE, 5 OTHER, 0 FAIL, 0 ERROR`. Its RESULT is FAIL only by design ("not #defined").
- `JUNE_EFFECTSET_FORCE_DOWN_RATE`: `2 PASS, 0 PASS~, 1 DEFINE, 5 OTHER, 0 FAIL, 0 ERROR`, `RESULT: PASS`.
- `JUNE_CHAR_EVE_CORE`, `JUNE_USER_SKILL_USE_HELPERS`: `0 PASS, 0 PASS~, 0 DEFINE, 8 OTHER, 0 FAIL`, `RESULT: PASS`.
- This review's comment fix: `CORE --worktree --files X2Lib/X2GuEve.cpp` gives `1 PASS, 0 FAIL`, `1 deleted line(s)` (the replaced comment), `0 encoding change(s)`, `RESULT: PASS`.

**Other checks.**
- `enumscan --check` (4 files): `0 A/B row(s) remain`.
- `/Zs` of `X2GuEve`, `X2GUUser`, `X2Game` and `X2EffectSet`: 0 errors each, with LIVE undefined and with `/DSERV_IRUHADEV_JUNE_CHAR_EVE_LIVE`. Warning counts are the same in both modes.

**JUNE_BUGS:** no row. **In game:** nothing (LIVE undefined). No rebuild: the code fix is a comment, so the binary is the same as CH1o's (`4f95ffbf`).

### CH1o2 - Done (2026-10-27)

CH1o's seven split slots each have a June base and a March call site; Eve overrides them under `_LIVE` (still undefined). The commit is "char-eve Phase CH1o2". Every June caller was found by a whole-`.text` scan for `[reg + slot]` (the hits that are not vslot calls are data fields); the per-class vtable values come from the 13 RTTI vtables (`CX2GUUser` `334781f4`, Eve `334cf45c`, ...).

| Slot | June base / caller | March site (new engine flag, defined) | Eve under LIVE |
|---|---|---|---|
| 0x3ac / 0x3b0 | empty `33108560`; `GetFinalDamage` @322857fb: AT_SPECIAL -> 0x3b0, every other type -> 0x3ac, after `32622780` | `CX2GameUnit::JuneModifyFinalDamageByGameUnitType` / `JuneModifySpecialFinalDamageByGameUnitType`, called after March's AT_NORMAL 4-arg call, which stays for Ara (`JUNE_FINAL_DAMAGE_UNIT_HOOK`) | 0x3ac = `June_ModifyFinalDamageByGameUnitType`; March's 4-arg override is empty |
| 0x56c | empty; `328e0aa0` @328e0ee4, from `UpdateUI` `328e18d0` @328e1c62 | `CX2GUUser::JuneSkillIconOverride`; S7b's `CX2EquippedSkillSlotData::UpdateUI` copies the templet's icon strings, runs `JuneApplyEquippedSkillIconOverride` (the frame and tail of `328e0aa0`), then `SetItemTex` (`JUNE_USER_SKILL_ICON_OVERRIDE`) | `June_SkillIconOverride` (was `June_Vslot56c`) |
| 0x584 | `31fc8070` = `!npc->HaveThisBuff( BTI_DEBUFF_FESSEL_HEILIG )`; `32f0e260` from `CX2AllyNPCAI::RallyToAlly` `32f10e60` | `CX2GUUser::JuneCanAllyRallyToMe`. `RallyToAlly` asks a user ally after the LIRE_MEMO test, before the distances; any other ally rallies (`JUNE_ALLY_RALLY_USER_GATE`) | `June_CanAllyRallyToMe` (was `June_Vslot584`) |
| 0x65c | `322b5520` = 0; `CX2GUNPC::GetCriticalPercent` `3275a820` (NPC vtable `+0x1d4`) = rate + extra + owner's 0x65c | `CX2GUUser::JuneSummonCriticalBonus`, added in `CX2GUNPC::GetCriticalPercent` when the owner is a user. March's owner is always one (`SetOwnerGameUnit( CX2GUUser* )`); June calls it unchecked (`JUNE_SUMMON_CRITICAL_OWNER_BONUS`) | `June_SummonCriticalBonus` (was `June_Vslot65c`) |
| 0x6a8 | `3208e190` = the cool time; `322e07e0` (March `UpdateSkillCoolTime`) @322e132c test > 0 / @322e1442 text | `CX2GUUser::JuneShownSkillCoolTime( cool, id )` in the cool-time `else if` and its `ceil` text (`JUNE_USER_SHOWN_SKILL_COOL_TIME`) | `June_ShownSkillCoolTime` (was `June_Vslot6a8`) |
| 0x6ac | `32001660` = March's `CX2GUUser::CallSummonedNPC` (gage-less passes; March's `GetNowHp` returns 1 too) | none: `SubStageStart` already calls the virtual (`FIX_SUMMONED_MONSTER_POSITION`) | `CallSummonedNPC` -> `June_CallSummonedNPC` (was `June_Vslot6ac`; same body as the base) |
| 0x7b0 | `32028790`; only caller `ProcessAcceleraterBuff` `3202c360`, which passes 0 or `IsPvpOrPve()` (`322cc1a0`) | none: the base and caller pair is backlog Stage 2's (accelerator auras bullet) | March's 3-arg `ApplyAuraAcceleratorBuffToGameUnit` -> `June_ApplyAura...( ..., 0 )`: the index is false offline |

- **Other classes' June overrides of these slots** (vtables): 0x3ac Ara `32ba45d0`; 0x3b0 Ain `32a6a640`; 0x56c Ain / Rose / Luciel; 0x6a8 Ara `32b958e0`, Chung `32bdba60`, Lire `32c7bed0`, Ain / Rose / Luciel. Their skill-use code also calls 0x6a8 (`32bcff21`, `32c17b82`, `32cac5a0`, ...). These are checklist §7ap rows. 0x584 / 0x65c / 0x6ac / 0x7b0 are Eve-only.
- **Live now for every class:** a summon under Fessel Heilig no longer teleports back to its user (0x584 base). The other four bases change nothing.
- **`328e0aa0`'s class blocks are not ported.** They sit behind the tree's `+0x130` byte, which only the class icon functions write (Eve's `322d9250` = June `UpdateEveElectraSkillSlotIcon`, @322d94ee / @322d950f from Eve `+0x97e4` / `+0x97f0`). Eve's two blocks go to CH4c (kickoff amended): class 0x1a El Crystal by `+0x134`, and class 0x6d skills 25601 / 25604 -> `Hyper_Active_02`. The other blocks stay in checklist 7aj.
- **No hook needed:** 0x77c has callers only in Eve (`3299a240`, `3299a47d`, `329b2769`: CH2a / CH3b); 0x828 at `31fc87d5` / `3206f99f` is the Stage 9 idle-timer reset. Both confirmed by the scan.
- **Drift:** June's `SubStageStart` user loop also clears a June-only byte `+0x944c` next to `SetShieldDamageReduce( 1 )` (`+0x412c`). It is unidentified and every class's -> backlog Stage 2 bullet.

**Checks.** `/Zs` (`census.cl_cmd`) of `X2GuEve`, `X2GUUser`, `X2Game`, `X2GameUnit`, `X2GUNPC`, `X2AllyNPCAI`, `X2UISkillTreeNew2015`: 0 errors with LIVE undefined and with `/DSERV_IRUHADEV_JUNE_CHAR_EVE_LIVE`. A probe TU confirms the five new defines resolve and LIVE does not. `enumscan --check` (8 files): 2 A/B rows, both pre-existing `X2GUUser.h:2537-2538` (`f07940fe`, not touched here).
**Hygiene** (`--worktree --files`, the 10 changed code files):
- the five new flags: `RESULT: PASS`, `0 deleted line(s) ... 0 encoding change(s)` each (`ICON_OVERRIDE` 1 PASS~ = a blank line next to its block);
- `JUNE_CHAR_EVE_LIVE`: `0 deleted line(s)`; its only FAIL is the by-design "not #defined";
- `JUNE_CHAR_EVE_CORE`: `1 PASS, 9 OTHER, 0 FAIL`, `5 deleted line(s)`, `RESULT: PASS`. The deletions are the renames: git shows 15 deleted lines, the five members' comment and signature lines (header 5, .cpp 10), all in CORE code.
**Build.** `DEPLOY OK: X2_offline.exe is 26,105,856 B, dated 2026-10-27 08:15:48, md5 75fe95cc419e2bf7c51c72b542ac5279, equal to X2/US_SERVICE/x2.exe (2026-10-27 08:15:41); built at main 8e39c047 + 10 uncommitted code file(s).`

**JUNE_BUGS:** no row. **In game:** nothing to test (LIVE undefined; the Fessel Heilig rule needs a June debuff source). Review: `kickoffs/char-eve/CH1o2-review.md` (Opus 5.5 high).

### CH1o2 - Review (2026-10-27)

**Verdict: PASS, no change.** Fresh capstone listings of every cited range, plus the ten slots read from all 13 RTTI user vtables (`.rdata` refs to the shared `3203cf60` at `+0x69c`).
- **Bases.** `33108560` = `ret 0xc` (3 args: 0x3ac / 0x3b0 DD, &phys, &magic; 0x56c id, name&, key&). `31fc8070` = `!arg->HaveThisBuff( 0x3d8 )`: the argument (`[esp+4]`), not `this`; 0x3d8 = 984 = `BTI_DEBUFF_FESSEL_HEILIG`. `322b5520` = `fldz; ret 4`. `3208e190` = `fld [esp+4]; ret 8`. `32001660` = Eve's `3298c580` apart from the cookie and when `GetPos` is copied; both are March's `CallSummonedNPC` (gage-less passes, `NUI_NONE` = 0 = any).
- **GetFinalDamage** @322857fb: `[DD+8] - 1 == 0` (AT_SPECIAL = 1) -> 0x3b0, else 0x3ac, args (DD, &[0x4c] phys, &[0x50] magic), after the fury scale and `32622780`; March has the same order (fury, `JuneApplyDivideDamageFromRange`, hook). Keeping March's AT_NORMAL 4-arg call is right: only Ara (`X2GUAra.h:663`) and Eve override it in March, Eve's is empty under LIVE, and Ara's moves at her port (§7ap).
- **Icon.** `328e18d0` passes the protected id from `3288fe00( slot, B )`, the value it also stores to `this+0xc` (= `m_eSkillID`, the slot's own id, not `323511f0`'s). Name / key are copies of templet `+0x50` / `+0x6c`, the same strings `SetItemTex` (`31cd0850` @328e1c93) takes. The gates are June's: g_pData, `+0x2cc`, `3214f050`, the g_pX2Game handle, game vslot 0x98; a failed gate skips to `@328e0eed`. The `+0x130` / `+0x138` / `+0x139` blocks sit before the tail (CH4c / §7aj).
- **Rally.** `32f10e60`'s order is ally null / die (vslot 0x174 == 4) / HP > 0 / master / class type / `+0x8e84` == 2 / id 0x401, then `32f0e260`, then `+0x88` / `+0x84` = March's insert point. `32f0e260`: ally `+0x264 == 0` (GUT_USER) -> ally vslot 0x584( master `+0x30` ), else `mov al, 1`.
- **NPC crit** `3275a820` (`ret 8`): protected `+0x4d48` (= `GetCriticalRate()`, March's own first term) + `fExtraCritical` + owner `+0x8d5c` vslot 0x65c( this ), with no type test. March's owner is a `CX2GUUser` (`SetOwnerGameUnit` callers `X2Game.cpp:7719/7727`), so the GUT_USER test is a guard only.
- **Cool time** `322e07e0`: vslot 0x6a8( protected cool `+0x24`, id `+0`) @322e132c, compared `> [0x3342e84c]` (= 0.0), then `bShowCoolTime`; the text reads it again @322e1442, then `ceil` / ftol. March already uses `GetMyUnit()` unchecked in that `else if` chain.
- **Eve wiring.** The five overrides forward every argument; the 4-arg override is empty under LIVE (`BALANCE_CODE_NEMESIS_20121213` defined, `Always.h:2138`); `CallSummonedNPC` -> `June_CallSummonedNPC`. 0x7b0: `3202c360`'s four calls pass `push 0` @3202c530 / @3202c648 / decompile 397, 402, or `IsPvpOrPve()` (413). `322cc1a0` is type 1 or 4. March's `GAME_TYPE` stops at GT_BATTLE_FIELD, and `GT_PVE` = 4 is only a registered name that nothing sets, so index 0 is exact offline.
- **Routing.** The vtables match §7ap exactly: 0x3ac Eve / Ara; 0x3b0 Ain; 0x56c Eve / Ain / Rose / Luciel; 0x6a8 those plus Ara / Chung / Lire; 0x584 / 0x65c / 0x6ac / 0x7b0 / 0x77c Eve only. Every class has its own 0x828 (Stage 9 idle timer, no hook here). The CH4c amendment, the backlog `+0x944c` bullet and the accelerator-aura note are present. `kickoffs/char-eve/CH1p.md` still matches.

**Hygiene** (`--range 759fb289^..759fb289`):
- five engine flags: `RESULT: PASS`, 0 deleted, 0 encoding changes (`ICON_OVERRIDE` 1 PASS~);
- `JUNE_CHAR_EVE_LIVE`: 10 OTHER, 0 deleted, only the by-design "not #defined" FAIL;
- `JUNE_CHAR_EVE_CORE`: `1 PASS, 9 OTHER`, 5 deleted = the five `June_Vslot*` renames in `X2GuEve.h`, `RESULT: PASS`.

`enumscan --check` (9 files): 2 A/B rows, `X2GUUser.h:2537-2538` (older). `/Zs` of the 7 `.cpp`: 0 errors with LIVE undefined and with `/DSERV_IRUHADEV_JUNE_CHAR_EVE_LIVE`. No rebuild (no change).

### CH1p - Done (2026-10-27)

Flags: `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (Eve, insert-only) and `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (an Aisha transcription fix). No new flag.

**Method.** `cl /P` of `X2GuEve.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_EVE_LIVE`, then every surviving line that reads a slot id, the pressed index or the key-pressed value, each mapped to its function and checked for a caller that LIVE still reaches.

**7ag, the pressed-index space.**
- **Already June's** (CH1l / CH1l3 / CH1n): the encoders (`June_InitEquippedActiveSkillState` 0-9), the key table (`June_InitInpuDataProcess`, `JuneEveSlotPureKey`), the decoders (`June_GetSkillSlotData*`), the reader (`June_SpecialAttackLookup` -> `JuneCommonSpecialAttackEventProcess`, A 0-4 / B 5-9) and the state lookup (`June_StateChangeBySkillSlot`, `>= 5`).
- **Dead under LIVE** (the March side, kept in `#else`): the encoders in `InitEquippedActiveSkillState`, the `:6151-6177` decode, `CommonEventProcess`'s three key switches and `InitInpuDataProcess`. The March `GetSkillSlotData*` and `IsUpSkillSlot*` still compile but have no caller.
- **Converted.** June's `32981130 @32981602` stores slot id + 1 in the key-pressed member (`+0x8ac0`), but two March state functions that LIVE still registers (through the March switch of `SetEquippedSkillFuncBySkillSlotIndex`) read it as March's A S D C Q W E R: `ENSI_SA_EPR_SWEEP_ROLLING_CHARGE_EventProcess` and `ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Fire_EventProcess`. A skill on Q would have ended at once, because the code tests W. Both now ask the new `JuneEveIsSkillKeyUp( input, value )` (namespace `_CONST_EVE_JUNE_CH1p_`), which takes slot = value - 1 and uses `SKILL_SLOT_*` members, F / T included. No June Eve function reads `+0x8ac0` (byte scan: one write only), so these two go when CH2c2 / CH3 replace the states.
- **Aisha fix (live).** `June_AVSI_HELL_DROP_CONTROL_EventProcess` (`32cd9d00`) switched on March's `SAKP_*`, but June's table at `32cd9d18` (jump table `32cda1b0`) is slot + 1: 1-4 = A S D C, 5 (F) = up, 6-9 = Q W E R, anything else = up. The port read F as Q and Q as W, so Hell Drop on a B slot steered with the wrong key. The cases are now `SKILL_SLOT_A1..A4 / B1..B4`, keyed on value - 1. Checklist 7ag updated.

**Engine hooks (CH0's two functions).** The stage-load handler `324d9550 @324d97a0-324d97be` (my unit, `GetUnit()`, unit type 5 = `UT_EVE`) is March's `DungeonStageLoadNot` block (`X2StateDungeonGame.cpp:2943`), in the same place (after the effect-set delete loop).
- `329a7f50` = March's `ResetTaserPilumCharge`: no change.
- `329b1e00` = `ResetMegaEletronCharge` calling `June_Reset` (`3299ebd0`, which also clears the June-only byte `+0x978d`). Under LIVE, `ResetMegaEletronCharge` now does that.

**Hooks 7h / 7l / 7o / 7p / 7x.** 7h: `JuneInitWaitStateChange( "ENSI_WAIT_STATE_CHANGE" )` and WIN -> `JuneRandomLoseEventProcess` are registered (CH1m, `June_InitState`). 7l: `JuneInitBoundingStates( "ENSI_" )` (CH1m). 7o / 7p: the user readers (`SET_LIMIT_MAX_G_SPEED`, `TIME_STOP_IMMUNE_STATE`) run at every state start for every class, and the keys arrive with June's `Eve_Nasod.lua` (final CH1). Eve has no hit-stop class site in 7p. 7x: 23 `AT_COMMAND` tags in `Eve_Nasod.src.lua` arrive with the script; the Elysion stand-in stays (7x).

**June-only keys (16).** `ADD_AIR_DASH_COUNT` (`+0x9c60`). The consumer is located and already ported: the readers in the Eve range are `32976e90` / `32976f00` (`June_GetMaxDashJumpCount` / `UpCount`, CH1a) and the inlined copies in `32977d30` / `32978890` / `32979da0` (the June_ JUMP_DASH / BOOST_DASH_AIR(_START) states call the CH1a helpers). `31d71408` is an immediate (an id), not the member. The 10 `CODE_RECOVERY_*` keys go to CH4c. The other five keep the owners in `ch0_keys.tsv` (row 397, Stage 2, Stage 9, buff tables).

**Checks.** `/Zs`: `X2GuEve.cpp` 0 errors with LIVE on and off; `X2GUArme_VioletMage.cpp` and `X2StateDungeonGame.cpp` (LIVE on) 0 errors.
**Hygiene.** `flagcheck JUNE_CHAR_EVE_LIVE --worktree --files X2Lib/X2GuEve.cpp`: `files: 1 PASS`, `0 deleted line(s)`, `0 encoding change(s)`; only the by-design "not #defined" FAIL. `flagcheck JUNE_CHAR_ARME_SKILL --worktree --files X2Lib/X2GUArme_VioletMage.cpp`: `files: 1 PASS`, `9 deleted line(s)` (the switch line and the eight case labels, replaced inside the flag), `0 encoding change(s)`, `RESULT: PASS`. `enumscan --check`: 0 A/B rows.
**Build.** `DEPLOY OK: X2_offline.exe is 26,105,856 B, dated 2026-10-27 08:57:19, md5 d007ebca7c6c6e5dc2c1eef93e0bddec, equal to X2/US_SERVICE/x2.exe (2026-10-27 08:57:12); built at main 2023f592 + 2 uncommitted code file(s).`
**In game.** Eve: nothing (LIVE is off). Aisha: Hell Drop's control on a Q / W / E / R slot now follows that key: **play-test PASSED 2026-10-27** (user, Hell Drop on Q). Funcmap: `329a7f50` / `329b1e00` confirmed, `32cd9d00` corrected. Review: `kickoffs/char-eve/CH1p-review.md`.

### CH1p - Review (2026-10-27)

**Verdict: PASS, no change.** Fresh capstone listings of every cited range, a `cl /P` of `X2GuEve.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` and of `X2GUArme_VioletMage.cpp` as built.
- **The sweep.** In the LIVE `.i` the only readers of the key-pressed member are the two converted states (`:28692`, `:32406`); the only writer is `June_SpecialAttackCooldown` (`:46195`, `m_iJune8ea4 + 1`, = June `@329815fb-32981602`: `+0x8ea4` + 1 -> `+0x8ac0`). `InitEquippedActiveSkillState` / `CommonEventProcess` / `InitInpuDataProcess` are one-line `June_*` delegations; the March `GetSkillSlotData*` / `IsUpSkillSlot*` (Mega / Taser / Kugelblitz) have no caller; only the two `June_GetSkillSlotData*` are called (`:46231`, `:46247`). Byte scan of `32965dd0-329cf86f` for `0x8ac0`: one hit, the store `@32981602`.
- **`JuneEveIsSkillKeyUp`.** Slot = value - 1 against `SKILL_SLOT_A1..B5_TC` (0-9 with `JUNE_SKILL_SLOT_10`, defined), F / T = `pureSkill5` / `pureSkillEx5`; an unmatched value returns false, which is March's (no OR term true). Tri-Vulcan keeps `&& m_fEffectStartTime >= 3.0f`; Sweep Rolling Charge had no extra term.
- **Aisha.** `32cd9d18`: `ecx = +0x8ac0 - 1`, `ja` past 8 -> `xor al, al`; table `32cda1b0` = A S D C, `@32cd9d7c` (up), Q W E R; key offsets `+0x86f6 / 86fa / 86fe / 86f2 / 8706 / 870a / 870e / 8712`. The port's `default: false` covers index 4 and out of range. `ARME_LIVE` is defined (`Always.h:9648`), so the `.i` is the LIVE one: March's `AVSI_HELL_DROP_CONTROL_EventProcess` (`:16829`, `SAKP_*`) still compiles and is registered at `:3565-3645` under `case SI_A_ADM_HELL_DROP` (2214), but June's `NewSkillTempletVer2` tree has no 2214 row (Void Princess gets 22020), so it is never equipped; it goes when CH2a-CH3b remove March's cases. Raven / Chung read the member with March's own encoding; `X2GUUser` only initializes it.
- **Hooks.** `324d979a-324d97be`: GetMyUnit (game vslot 0x98), null, `+0x867c` -> `+0xaf0` -> type 5, then `329b1e00`, `329a7f50` = March `X2StateDungeonGame.cpp:2943-2954` (`SERV_EVE_BATTLE_SERAPH` defined, `ServerDefine.h:3544`). `329b1e00`: `+0x9784` (= `+0x9778 + 0xc` bEnable) == 1 -> `3299ebd0( 0, +0x7d14 )` = `June_Reset( false, &future )`. `329a7f50`: same shape on `+0x9a48` -> `329a7e70` = March.
- **`+0x9c60`.** Eight sites in the Eve range: readers `32976ebe` / `32976f17` (CH1a helpers) and the three inlined state copies, plus three the Done note did not list, all ported: `3299c95b` (ctor = 0), `329c3f01` (Recovery ECN tooltip arg, CH1k `:44464`) and `329c5084` (`InitComponent` load, CH1k `:44415`).
- **7h / 7l / 7o / 7p / 7x.** As the Done note says; nothing new.

**Hygiene** (`--range 8766131d^..8766131d`):
- `JUNE_CHAR_EVE_LIVE`: `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), ... 0 encoding change(s)`; `RESULT: FAIL` = only "not #defined" (by design).
- `JUNE_CHAR_ARME_SKILL`: `files: 1 PASS, ... 1 OTHER (other flags), 0 FAIL`; `in this flag's files: 9 deleted line(s), ... 0 encoding change(s)`; `RESULT: PASS`.

`enumscan --check` (2 files): `0 A/B row(s)`. `/Zs`: `X2GuEve.cpp` 0 errors with LIVE off and on, `X2GUArme_VioletMage.cpp` 0 errors. No rebuild (no change). `kickoffs/char-eve/CH1q.md` still matches.

### CH1q - Done (2026-10-27)

Flag: `SERV_IRUHADEV_JUNE_CHAR_EVE_NCORE` (`Always.h`), defined **only when `_LIVE` is**, and it defines `ADD_EVE_SYSTEM_2014` / `FIX_ADD_EVE_SYSTEM_2014` (decision 1 = (a)). Eve stays on her March kit until CH1 final defines `_LIVE`; nothing changes in game today.

**Turning the two studio macros on.** `/Zs` of `X2GuEve.cpp`, `X2GUUser.cpp`, `X2GageUI.cpp`, `X2GageManager.cpp`, `X2GUNPC.cpp`, `X2DamageManager.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_EVE_LIVE`: 0 errors before and after this phase's edits, so March's dormant 2014 code compiles as is; the 77 sites of `ch0_nasod_core_sites.tsv` are all it switches on (grep: the 9 files + `Always.h`). `DTT_EVE_MANEUVER_ATTACK` comes out 5 = June's (`EVE_ELECTRA`, `SERV_ADD_CHUNG_SHELLING_GUARDIAN`, `BALANCE_PATCH_20131107` on); `DTT_END` becomes 6.

**Every map row:** `june2017_re/char_eve/ch1q_nasod_core_outcome.tsv` (126 rows: June body 19, March = June 101, dropped 3, other 3). The June bodies:
- `AddManeuverGauge` `3297ee20`: gauge + ( spec A + 1.0 ) x arg.
- `DoParrying` `329b4060`: when `!( spec B > GetRandomFloat(0) )` the 30 cost, the level drop and the `+0x9c44` `clock()` stamp; `ChangeManeuverCoreState( MCS_PARRYING )` always.
- `ENSI_PARRYING_BLINK_Start` `32975740`: `GetSkillTemplet( RNSI_A_EVE_PHOTON_BLINK )`, else `_UPGRADE`, ability `SA_TELEPORT_RANGE_ABS` at level 1.
- `OnFrameMove_ManeuverCore` `329b5230`: no gauge `Verify` / hack report, no hyper ball (moved to the UI), AFTER_ATTACK cost behind the same spec B roll (`32070350`, before `IsMyUnit`); called by `June_CommonFrameMove` (`@329c89a3`, after the El Crystal end).
- `AttackManeuverCore` `329ac4a0`: the normal branch tests the laser before `SetIndex` (the hyper one does not: JUNE_BUGS 443); `+0x9bfc` x level > 0 -> `UpNowMp` + blue `MP+ %d` at pos + 50.
- `SetAttackManeuverCore` `329acea0`: `clock()` into `+0x9c44` after the gate. `GetHyperModeStateID`: `USI_HYPER_MODE` (`32968470`); `ENSI_BERSERK_MODE` enum and registration dropped.
- `IsSkillSummonedMonster` `329cf280`: + `NUI_NASOD_WATCH_EVE_MEMO` (519, CH0 missed it), `NUI_SI_SA_AT_SURFACE_CUTING`, `NUI_SI_SA_EEP_ELECTRONIC_FIELD`, `NUI_ECA_OBERON_GUARD`.
- `CX2EveMyGageUI`: `m_hSeqHyperBall` (`+0x5c`), dtor `32e725e0`, `OnFrameMove` `32e71990`, `SetShow` `32e5eca0`, `June_UpdateHyperBall` `32e6a210` (count `+0xf4` == 3 -> `Eve_PowerUp_UI_01` over the UI at ( 72, 80 ) x resolution, no owner; shown only while the gauge UI is, hidden with the dialogs; else destroyed).
- Registrations: `ENSI_PARRYING_SMALL` / `ENSI_PARRYING` / `ENSI_PARRYING_BLINK` in `June_InitStateCommon` (callbacks = March's, `ch0_reg.tsv` 1385-1401). `ch1m_regcheck.py` with 224-226 out of `ROUTED`: `matched 792 registrations, 0 problems; ids mapped 137` (11 callback funcmap rows added).

**Open questions resolved.** (1) `+0x6c50` / `+0x6d04` are the socket specialization results, `GetCharacterSpecializationA()` / `B()` (`+0x6bb0` / `+0x6c64` + `0xa0`; Aisha CH4c reads `+0x6d04` the same way): `m_fJuneSpecializationA` (CH1k) is replaced in `AddManeuverGauge`, `June_AttackResultByType` and `June_SpecialAttackCooldown` (it stays only in the `#else`). (2) Which of `32e5b650` / `32e5b8a0` is Add's `InitWakeOrb` does not touch Eve (both remove orb 6); left for Add's port. (3) `32894e40` = a June-only `CX2UserSkillTree` cool-time cut ( seconds or rate, both bars, then clears the reset lists ); **routed to CH4a** with `+0x9c00`, which stays 0 until CH4a, so the branch is a one-line comment now (CH4a kickoff amended). (4) The PARRYING 0.3 s tail: March = June (`SKILL_CANCEL_AFTER( 0.3f )`), not re-checked byte for byte.

**Parked blocks now live with `_LIVE`:** CH1b `ClearManeuverCore( true )` (die front / back), CH1k `June_AttackResultByType` / `HyperModeBuffEffectStart` / `ReInit` / dtor, CH1l2 sync int, CH1l3 gauge gain and parry refusal, CH1l4 Open Mind floor. Re-checked against June: as ported, except spec A (above) and the DTT member (`CX2DamageManager::DTT_EVE_MANEUVER_ATTACK` under NCORE; the CH1k local stays for the `#else`).

**Kept as March:** the dev overlay's Eve gauge text in `CX2GUUser::RenderName` (`#ifndef _SERVICE_`, dev auth + NPC names): June's service exe drops the whole overlay (no `Combo: %d` either), like Raven's rage text. The `X2GUUser` / `X2GUNPC` / `X2GageUI` ADD sites are March = June (map).

**Checks.** `/Zs` LIVE on: 6 TUs 0 errors; LIVE off: `X2GuEve.cpp` / `X2GageUI.cpp` / `X2GageManager.cpp` 0 errors. `cl /P` with LIVE: the new code is in, `ENSI_BERSERK_MODE` and the old hyper ball are gone.
**Hygiene.** `flagcheck JUNE_CHAR_EVE_NCORE --worktree --files` (5 files): `files: 4 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 13 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. OTHER = `Always.h` (the two studio defines). `enumscan --check` (4 files): 0 A/B rows.
**Build.** `DEPLOY OK: X2_offline.exe is 26,105,856 B, dated 2026-10-27 09:49:07, md5 8289303b365464879add009ea90ce64f, equal to X2/US_SERVICE/x2.exe (2026-10-27 09:49:01); built at main 5fa7f8df + 5 uncommitted code file(s).`
**In game.** Nothing until CH1 final (`_LIVE`); the CH1 kickoff now lists the Nasod Core play-test items. JUNE_BUGS 443 added, 435 now live. Checklist 7an / spec A / spec B rows marked `(Eve: CH1q)`. Review: `kickoffs/char-eve/CH1q-review.md`.

### CH1q - Review (2026-10-27)

**Verdict: PASS, no change.** Fresh capstone listings (`charport_adis.py`) and Ghidra decompiles of every function the kickoff names, read against `git show d075e236`.
- **Spec B roll.** `329b4060`: spec B (`+0x6d04`) is loaded, then `GetRandomFloat(0)` (vslot 0x1cc); `fcompp` + `test ah, 0x41` + `je` skips when B > roll. Otherwise `clock()` -> `+0x9c44`, gauge - 30 (clamped at 0), `--+0x9b9c` (`jns`, clamped at 0); `ChangeManeuverCoreState(5)` always runs, and a null core (`+0x9b98`) returns. `329b41f0` = `CommonStateStart` + `+0x9bc0 = 1` + `jmp 329b4060` (Ghidra's merge). `329b5230 @329b5547`: the roll goes through `32070350( B, roll )`, `jne` skips the cost, and only then comes `IsMyUnit` (vslot 0x198). The port's `!( B > roll ) && IsMyUnit()` matches, roll first.
- **Spec offsets.** `+0x6c50` / `+0x6d04` = `+0x6bb0` / `+0x6c64` (the `X2GameUnit.h:5251-5252` annotations) + 0xa0, which is the result Add's `329e5b00` reads; `3297ee20` = gauge + ( `+0x6c50` + 1.0 ) x arg.
- **`32975740`.** `32206450( 0x6270, 1 )`, else `( 0x627d, 1 )`; the 1 is the IsEnableCode level gate, dropped offline as at `X2GUUser.cpp:40518`. No `+0x867c` -> return before the blink flags; `32206e50( 0x17, 1, 0, 0 )` = `SA_TELEPORT_RANGE_ABS` at level 1.
- **`AttackManeuverCore`.** Effect-set / damage-effect failures jump to `329aca5c`, which skips the MP block; a null mesh jumps to `329ac908`, the MP block. So the block is inside the effect test and outside the mesh test, as ported. `+0x9bfc` x `fimul +0x9b9c` > 0 -> `UpNowMp` (0xf0), then PicCharBlue (`+0x158`), `_ftol2`, `L"MP+ %d"` (0x40), and the position comes from `GetPos` (0x138) + 50.0. The null test sits only after the normal `SetIndex` (`@329ac84a`; JUNE_BUGS 443).
- **`329acea0`.** The `329cd650` gate, then `clock()` -> `+0x9c44`, then the inlined `SetManeuverCoreTargetUnit` (USE_MANEUVER_GAGE case), then `ChangeManeuverCoreState(2)`. **`32968470`** = `mov eax, 0x17`. **`329cf280`**: the jump table `329cf2e0` / `329cf2e8` decodes to 384-386 and 410-411, plus 519 / 520 / 674 / 682 / 1454 / 1621 / 1622 / 2091, 13 ids = March's 9 + the four.
- **Hyper ball.** `32e6a210`: gage data `+0x10`, byte `+0xf4` == 3, else `32e5cfe0` destroys. The particle system is `322ceaa0` = `g_pData+0x84` (= March `CX2Game::GetMajorParticle`). Create takes owner 0, `Eve_PowerUp_UI_01`, ( 72 x `+0x1b8`, 80 x `+0x1bc`, 0 ) and default args, then `+0x170 = 1` (OverUI); otherwise `SetPosition( pos, 0, 0 )`. Then `GetShow` (0x1c) -> show = `!GetHideDialog`. `32e5eca0` = base + `SetShowObject( arg )`; `32e71990` = base + ball; `32e725e0` = destroy + base dtor (base `~CX2GageUI` is virtual). `OnFrameMove( float )` is the live signature: `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` is unconditional at `KTDX.h:342`.
- **Every-class sites.** These are the ADD sites outside Eve's own files: `X2GUUser.cpp` 872 / 1239 / 8919 / 9978 / 10038 / 10431 / 11655 / 13158 / 27916 / 28074 / 35247, `X2GUNPC.cpp:32127`, `X2GageManager.cpp:237` and `X2GUUser.h`. Each is gated on `UT_EVE`, an Eve-owned NPC, an Eve class case, or `m_bManeuverParryingState` (set only under the `UT_EVE` test at 9978); for everyone else the rest are false initialisers and the base virtuals' no-ops. `DTT_END` (now 6) has no code user, and June's Lua already has 6.
- **Parked blocks / registrations.** The CH1b / CH1k / CH1l2-4 blocks were reviewed when they were parked, and their spec A switch matches. The three parry registrations use March's callbacks, and `ch1m_regcheck.py` gives `matched 792 registrations, 0 problems; ids mapped 137`.

**Hygiene** (`--range d075e236^..d075e236`): `files: 4 PASS, 0 PASS~, 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 13 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (OTHER = `Always.h`). `enumscan --check` (4 files): `0 A/B row(s)`. `/Zs` of `X2GuEve.cpp`, `X2GUUser.cpp`, `X2GageUI.cpp`, `X2GageManager.cpp`, `X2GUNPC.cpp`: 0 errors each, with LIVE on and with it off. No rebuild (no change). `kickoffs/char-eve/CH1.md` still matches.

### CH1 - Done (2026-10-27)

Opus 5.5. **The deploy unit is live.** `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` is defined (`Always.h:10130`); it turns on `_NCORE` (Nasod Core), so Eve runs June's registration, virtuals, skill head, pressed-slot space and core (CH1a-CH1q). Shipped together with the exe:
- **Script:** `luasrc.py adopt-june data036 Eve_Nasod.lua`: June's file replaces March's whole, as `ljconv` bytecode (172,650 B; no core-list row, no edit). Backup `24965799\kom_backup_stage_june\20261027_100517`, record `june2017_re/june_adopt_20261027_100517.tsv`. `PACK OK data036` (vs live ~1 changed, `Eve_Nasod.lua`); `COPY OK data036 ... 14006609 bytes, 267 entries, md5 38b60f616a552c355dd2a35927e40a29` (user approved).
- **Summon / skill NPC scripts:** all 14 `char-eve/done-elsewhere` rows of `ch0_assets.tsv` (13 in data037, `Eve_AT_Sp2a_SonicWave` in data119) are June's bytes already: header `1B 4B 4C 81`, `adopt-june --dry-run` = identical. Nothing re-adopted. CH0b held no koms.
- **START fix, found before the play-test (Aisha's CH1 bug again):** Eve's LIVE `ParseCommonRandomState` was empty ("June rolls in June_InitState"), so START / WIN / LOSE would roll from empty lists. June parses in the base (`31feaac0` from `CX2GUUser::Init`); the LIVE override now calls `CX2GUUser::JuneParseCommonRandomState()`. June's 7 `INIT_COMMON_RANDOM_STATE` tables (`EVE_NASOD` .. `EVE_BATTLE_SERAPH`) equal `JuneGetUnitClassName`'s Eve names; March's parse used `UC_EVE_CODE_*` names June's script lacks. The helper's declaration and body get an Eve-only copy (`#ifdef EVE_LIVE / #ifndef ARME_LIVE`, the CH1o hook precedent).
- **`[JUNE_CHAR] EVE DATA`** (LIVE, end of `June_InitState`, offline_server.log): class, script, motion, state count, `wait_state_change` (1 only with June's script: March's has no `ENSI_WAIT_STATE_CHANGE`). `X2GuEve.cpp` includes `Offline/X2OfflineLog.h` under LIVE (first build failed C2653 without it).
- **7h:** June's `ENSI_WAIT_STATE_CHANGE` has `ANIM_NAME = ""`; March's `CommonStateStart` takes it as for Aisha (CH1 Done, Aisha plan): `ChangeAnim( L"" )` returns false, the pose holds until `USEFUL_VALUE0[0]`.
- **Scans:** `charport_keys.py Eve_Nasod`: `june-only=6 neither=1 march-only-site=8 both=224` (plan: 16 / 1 / 8 / 214; CH1k ported the 10 `INIT_COMPONENT` keys). The 6 left all have CH0 owners (`ch0_keys.tsv`: buff sub-tables read already; `EFFECT_SET_LIST_FINAL_ULTIMATE` backlog 3c; camera keys Stage 9; immunity list Stage 2). `charport_scan.py` (from `%TEMP%\cps`): Eve = `no NUMERICAL_DATA_INDEX` 25 / `no DAMAGE table` 3, the same as the plan scan (`plan_charport_scan_20261025.txt`; Electronic Move tables, CH3c); no `ENUM` line for Eve, so §4 has no table to add (marked).
- **Checklist:** `(Eve: CH1, script adopted)` on 7h list variants (l.411), 7o bounding (l.496), 7p time-stop (l.535), 7t cut-in script (l.579), 7t sound sets (l.605), 7x AT_COMMAND (l.660).
- Reference log for the play-test: `data\ErrorLog_pre_CH1_eve.txt` (182,416 B, run 2026-10-27 09:07).

Hygiene: `flagcheck.py JUNE_CHAR_EVE_LIVE --worktree --files` (4 files): `files: 3 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `2 deleted line(s)` (the `//#define` and the LIVE comment the parse call replaces), `0 encoding change(s)`, `RESULT: PASS`. `enumscan --check X2GuEve.cpp`: 0 A/B.
Build: `DEPLOY OK: X2_offline.exe is 26,080,256 B, dated 2026-10-27 10:17:39, md5 61de6ca0a8bb0309a47235dba918e638, equal to X2/US_SERVICE/x2.exe (2026-10-27 10:17:32); built at main b561b723 + 4 uncommitted code file(s).` `EVE DATA` is in the exe once.

**Play-test (user) is the gate:** one Eve per job, START / combos / dash / jump / riding (Iivue owns two riding pets) / awakening with the Nasod Core; `EVE DATA` per class; ErrorLog against the pre-CH1 copy. No new JUNE_BUGS row, no drift.

**Play-test 1 (user, 2026-10-27, run 10:51-11:04 on md5 `61de6ca0`):** `EVE DATA` for all 7 classes (5: 142 states, 18: 148, 19: 145, 26: 149, 108: 159, 109: 152, 116: 153), each `EVE_NASOD.lua` / `MOTION_EVE.X` / `wait_state_change 1`; the START fix works (`C3e-2` rolls `ENSI_START` / `ENSI_AT_START`, `ENSI_WIN2`, `ENSI_LOSE2`, no `list size 0`); 0 UNHANDLED besides the known HB check, 0 EXCEPTION; ErrorLog's new lines are map textures and event scenes, none Eve's. Three findings:
- **Exotic Gear / Code: Nemesis dash-jump Z-X has no spear on the ground.** Its effect set is in June's `EFFECT_SET_LIST_FINAL`, which March's `LoadEffectSetFromScript` never reads (checklist l.411, `32024ff0`). CH0's key scan missed it (it matched the `_FINAL_ULTIMATE` text of a March comment). Eve has 5 such states, Aisha 4 unread `ULTIMATE_EX` lists. Playable combat on her combo path, so a phase of this plan: **CH1r** (June's `32024ff0` whole).
- **The third-awakening ball on the portrait never shows** (CH1q's `June_UpdateHyperBall`). The override runs (`OnFrameMove( float )` is live), the gauge UI class is built for all 7 Eve classes, the particle is in data036. What June's gage data `+0xf4` holds (orb count or the awakening level used) and why the handle is never made need RE: **CH1s**.
- **Eve's V key and its icon are missing:** June's Code Recovery (`ENSI_EVE_RECOVERY`, the `CODE_RECOVERY_*` keys, the 7ah UI statics `322e65c0` / `322dee90`) is CH4c's by plan; nothing new.

### CH1 - Review (2026-10-27, Opus): PASS, no code change

Run after CH1r and CH1s closed, so the deploy unit reviewed is the final one (exe md5 `2ffc4610`, the CH1s review deploy).
- **Deploy unit:** `Always.h:10130` defines LIVE. data036 `Eve_Nasod.lua` is 172,650 B, sha1 `78f1d839...`, header `1B 4B 4C 81`: equal to `june_adopt_20261027_100517.tsv`'s ljconv row; no `Eve_Nasod.src.lua` sidecar. data034 `DLG_PVP_Game_My_State_NEW.lua` is 8,208 B, sha1 `28b6cbc7...`, `1B 4B 4C 81` = `june_adopt_20261027_123538.tsv`; data057 holds `DLG_UI_Common_Texture80_NEW.TET` / `.tga` (CH1s). `kom.py status data036 / data034 / data057`: `in sync`, vs live `+0 / -0 / ~0` each (data036's "stale .src.lua" note is another file's). `X2_offline.exe` holds `EVE DATA` once.
- **EVE DATA vs `ch1m_reg_by_class.tsv`:** the tsv has 123 / 129 / 126 / 127 / 138 / 133 / 128 rows for classes 5 / 18 / 19 / 26 / 108 / 109 / 116; the C++ in `June_InitStateCommon` + `June_InitState` registers exactly tsv - 16 for every class (the hooks and routed ids `ch1m_regcheck.py` skips; it still prints 792 / 0 problems). Logged minus C++ is 35 + 0 / 0 / 0 / 3 / 2 / 0 / 6. The constant 35 is the base and hook states (`CX2GUUser::InitState`, bounding, wait-state, Reason Pride's unconditional `ENSI_SA_ECE_REASON_PRIDE_ACTION`). The varying part is June's skill-gated registrations in `June_SetEquippedSkillLevel` (`X2GuEve.cpp:45089`): Electra +3 (25021 Sweep Rolling: charge / fire / end), Code Nemesis +2 (25206 Spear Burst: landing / landing hyper), Battle Seraph +6 (25021 + 25223 Electronic Move: front / up / down), each skill's `unit_skill` row checked in `els_db.sql`. They follow the unit's skills, not the class, so the class rows match. All 7 lines read `MOTION_EVE.X`, `wait_state_change 1`; June's `ENSI_WAIT_STATE_CHANGE` has `ANIM_NAME = ""`.
- **START fix vs `31feaac0`:** BeginTable( `INIT_COMMON_RANDOM_STATE`, 1 ), name `31fdcc20`, `31fe6780( name )`, `EndTable( 1 )`. Its one caller is `3204ea20 @3204eae1`, unconditional for every class, after vslot 0x3c and before 0x48. Eve's LIVE override calls the base helper, so the effect is the same; Aisha's LIVE build compiles the helper, and Eve's copy stays `#ifndef ARME_LIVE`. June's 7 table names (`EVE_NASOD` .. `EVE_BATTLE_SERAPH`) equal `JuneGetUnitClassName`'s 7 Eve cases. Latest run: `C3e-2` rolls `ENSI_BS_START` (list 3), `ENSI_BS_WIN2` / `ENSI_BS_LOSE2` (6); 0 `list size 0`, 0 `re-bind skipped`.
- **ErrorLog** (12:39-12:40 run, class 116, against `ErrorLog_pre_CH1_eve.txt`): 46 new distinct lines, 0 `KEM_ERROR`, 0 `Motion Error`. The two Eve lines, `MESH_EVE_CASH_FACE_2.TGA` and `MESH_EVE_CASH_FACE03` (429, char select), are CH0 `dangling` rows: names inside June's `Mesh_Eve_Cash_Face_Sorted.XET` that no June kom holds, so they are June's own (`ch0_assets.tsv:7720`). The pre run had no Eve on screen. The rest are Ruben map textures and `[JUNE_ENGINE]` diagnostics. Play-test 1's own ErrorLog was overwritten; its Done-note reading stands.
- **Nasod Core:** the 12:40 run's `EVE NCORE ball` lines show count 0, then count 3 / handle 1 / drawn 1 for about 2 s, then 0 when the awakening starts: this matches CH1s's reading (`+0xf4` = orb count). The line was retired in the CH1s review.
- **Checklist:** `(Eve: CH1, ...)` on l.411 / 497 / 536 / 580 / 606 / 661 (7h, 7o, 7p, 7t x2, 7x; the +1 shift is CH1r's l.411 edit); §4 at l.159 ("charport_scan.py ENUM has no Eve line: no table to add").
- **Hygiene:** `flagcheck.py JUNE_CHAR_EVE_LIVE --range 3d28894b^..3d28894b`: `3 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `2 deleted line(s)` (the `//#define` and the replaced comment), `0 encoding change(s)`, `RESULT: PASS`. `enumscan --check X2GuEve.cpp`: `0 A/B row(s) remain`. No rebuild.
- `CH2a.md` still matches (depends on this review; case bodies go in `June_SetEquippedSkillFuncHead`, CH1n's amendment). User owes nothing.

### CH1r - Done (2026-10-27)

Opus 5.5. Kickoff `kickoffs/char-eve/CH1r.md` (from the CH1 play-test). New flag `SERV_IRUHADEV_JUNE_EFFECT_SET_LIST_VARIANTS` (`Always.h`, defined; engine, every class). Funcmap rows `32024ff0 (CH1r)`, `32020ec0`, `3200b030`, `32007570 @320075e5 (CH1r)`, `32007e90`.
- **`CX2GUUser::LoadEffectSetFromScript`, the list part = June `32024ff0`** (the delete-key tail stays shared, C9h-4d's; March's list code in `#else`):
  - `LIST_MEMO` (memo byte) else `LIST`.
  - The TRANS list for every state: memo row of `GetNowStateSkillID()` (`31fdbbb0`) type `MI_TRANSCENDENCE_1` -> `EX_TRANS_1`, `_2` -> `EX_TRANS_2`, else `EX_NO_TRANS`.
  - `EX_MEMO` (memo byte).
  - Upgraded state skill (`bJuneUltimate_`, S3f): `ULTIMATE_EX_APPLIED_SKILL` when its `APPLIED_SKILL_ID` is learned (`31fceab0` > 0), else `ULTIMATE_EX`. Then `EX_APPLIED_SKILL` unless the first one loaded.
  - `EX` iff (memo byte ? no `EX_MEMO` : no ultimate / applied-skill list) (`@32026685-320266ca`). Its rows carry `POSSIBLE_NASOD_BARRIER` (read from `m_LuaManager`, as June's this `+0x2b4`).
  - `FINAL_ULTIMATE` (upgraded skill, when present) else `FINAL`.
- **`32020ec0` / `3200b030`** = `JuneLoadEffectSetListFinal` / `JuneLoadEffectSetFinalEntries`:
  - `EFFECT_SET0..` rows, plus one `EFFECT_SET_RANDOMn` picked by the `RATE` weights against `GetRandomFloat( 0 ) x 100`.
  - Per row: `IS_LEARN_SKILL_CHECK` (no unit or level < 1 drops the row), `START_TIME`, `CONSUME_MP`, `RELATE_SKILL_ID`, `DELETE_CONDITION`.
- **Play gate `32007570 @320075e5`:** a row with `CONSUME_MP > 0` plays only if `IsEnoughMPForAttack( mp, GetBonePos( L"Bip01" ), 0, 0 )` (slot 0x6f0 = March's `IsEnoughMPForAttack`, June `32007e90`). Otherwise it is erased unplayed, with the magic-fail puff. `IsEnoughMPForAttack` takes June's -100 offset when facing left (March: +100; both callers pass offset 0).
- **`EffectSetToPlay`** gains June's entry `+0x34` / `+0x38` (`m_bJunePossibleNasodBarrier`, `m_iJuneRelateSkillID`), set in every ctor. No reader exists in the loader or the play site.
- **Callers:** `CommonStateStart` `@3205ad90` ( `m_LuaManager`, `m_bApplyMemo` = false, upgraded byte ) and `CX2RidingPet::StateChange` `@32d343b5` ( pet lua, 0, 0 ) = March's two calls, unchanged. No class calls it (June xrefs).
- **Data reached now:**
  - Eve: `EFFECT_SET_LIST_FINAL` on `ENSI_EEG_DASH_JUMP_ZX` / `_ZZX` (`EffectSet_SA_ECE_Dashjump_ZX`, CONSUME_MP 4), Iron Scraps (+`_HYPER`), Assault Spear. `_FINAL_ULTIMATE` on Iron Scraps and Atomic Blaster (upgraded skills).
  - Aisha: `ULTIMATE_EX` on Chain Burst, Dark Cloud (+`_HYPER`), Dark Burst (upgraded skills show the `_Upgrade` / `_Ultimate` sets).
- **Drift, routed (checklist new l.412):** readers of `+0x34` / `+0x38`, and the per-class slots 0x4c4 / 0x4c8 around each play (base empty; Add / Ain / Rose / Ara override). Character ports. No JUNE_BUGS row (no June quirk kept beyond March's own empty-name stop).
- **Checklist:** l.411 `(Eve: CH1r ...)`. MODS.md block "June effect-set list variants".

Hygiene: `flagcheck.py JUNE_EFFECT_SET_LIST_VARIANTS --worktree --files` (4 files): `files: 3 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `/Zs` `X2GUUser.cpp`, `X2GUNPC.cpp`: 0 errors. Flag probe: the flag and its 9 prerequisites ON. `enumscan --check X2GUUser.cpp X2GUUser.h X2GameUnit.h`: 2 B rows, both older (`X2GUUser.h:2544-2545`), none in CH1r lines.
Build: `DEPLOY OK: X2_offline.exe is 26,081,280 B, dated 2026-10-27 11:44:33, md5 52ee62aea91586ee5c44b8b119dfdf1a, equal to X2/US_SERVICE/x2.exe (2026-10-27 11:44:26); built at main 83cde348 + 4 uncommitted code file(s).` No kom change.

**Play-test (user) is the gate:** EveNemesis dash-jump Z-X spear plus 4 MP; low-MP puff; ArmeVP Dark Burst `_Upgrade`.

**Play-test PASSED (user, 2026-10-27, run 11:46-11:49 on md5 `52ee62ae`):** `EVE DATA` class 108 (EveNemesis); ArmeVP (unit 52) with Dark Burst 22060 in slot 0 B; 3 UNHANDLED (the known `E_DISABLE_HB_CHECK_REQ`), 0 EXCEPTION, clean exit; ErrorLog only the load-time `JUNE_FX ES_*` key notes, no effect-set miss.

### CH1r - Review (2026-10-27)

Opus 5.5. Commit `31c8cc86` read line by line against June `32024ff0` (list part, to `@320268e0`), `32020ec0`, `3200b030`, `32007570 @320075e5-32007662` and `32007e90` (Ghidra decompile + disassembly; constants read from `unpacked_x2.exe`). **Verdict: PASS, no transcription error, no code change, no rebuild.**
- **Branch flags:** `ba` / `b9` (`[esp+0x32]` / `[esp+0x33]`) are both set only inside the learned branch of `ULTIMATE_EX_APPLIED_SKILL` (`APPLIED_SKILL_ID` non-zero and `31fceab0` > 0). Found but not learned leaves `ba` 0 and falls to `ULTIMATE_EX` (`goto @32025eac`), which sets `ba` only. `EX_APPLIED_SKILL` is skipped only by `b9`. The EX gate `@32026685-320266ca` is: memo byte 0 -> `!ba && !b9`; memo byte 1 -> `!a9` (`[esp+0x43]`). This matches the port's `bJuneReadEx`.
- **Memo:** `EFFECT_SET_LIST_MEMO` under memo byte 1, and a miss jumps to `EFFECT_SET_LIST` (`LAB_32025123`), as the port does. The pairs loop stops before 100, i.e. at most 50 pairs.
- **TRANS:** `31fdbbb0( slot 0x524 )`, row word 0 `== 0x13` / `0x14` = `MI_TRANSCENDENCE_1` / `_2` (19 / 20 in `X2SkillTree.h:2080` and June's `Enum.src.lua`). `JuneSkillMemoEntry::m_iType` is the first member. It is read for every state.
- **`POSSIBLE_NASOD_BARRIER`:** `ESI = [esp+0x34]` (this) `+0x2b4` at `@32026656`, so it is read before the gate, a miss stores 0, and only the EX ctor (`32079890`) gets it as its 4th argument; every other list passes 0. This matches the port.
- **Row bodies:** an empty name `break`s with the row left open (`@3202541e`-style), the time miss is 0, and delete conditions run from index 3 into an unseeded vector for the EX ctor. All match the port.
- **`32020ec0`:** `XOR BL,BL @32020f05` (the list is absent -> false), `MOV BL,1 @3202115e`, `RET 8`. `RATE` floats come from index 1. The roll is slot `0x1cc` GetRandomFloat( 0 ) x double `3342f360` = 100.0, stored as float. The index is the first one where `roll <= float sum`, else -1. `EFFECT_SET` -1, then `EFFECT_SET_RANDOM` only when >= 0. All match the port.
- **`3200b030`:** the key is `"%s%d"` of `max( 0, index )`. `IS_LEARN_SKILL_CHECK` (XOR key decodes `IS_L...`) uses raw `32890620( id, 0, 0, 1 )` and drops the row when there is no unit `+0x867c` or the level is < 1. The defaults are name "" with a `DCES_NONE` seed, START_TIME 0, CONSUME_MP `334380f0` = -1.0, and RELATE_SKILL_ID 0 (`-(found) & value`, `+0x38`). `DELETE_CONDITION` replaces the seed. A non-negative index reads one row (`param_3 < 0x80000000 -> LAB_3200bb5e`). All match the port.
- **Play gate:** `FCOM [ebp+0x30]` against 0 with `TEST AH,5 / JP` means only `0 < CONSUME_MP` enters the gate; equal, negative or NaN plays. The call is slot `0x6f0`( mp, slot `0x84` GetBonePos( L"Bip01" `3344a594` ), 0, 0-vector ). A failure (`JNZ 3200786d`) erases the row through `32388150` / `3207e050`, does `DEC i` and continues. This matches the port's `erase; --i; continue`.
- **`32007e90`:** facing right takes `3342f360` = +100.0, else `33476460` = -100.0 (`@32007f40` / `@32007f4c`), so the port's -100 left offset is right.
- **`#else`:** this is March's list code byte for byte (0 deleted lines). Its `APPLY_MEMO` read is under `#ifndef ADD_MEMO_1ST_CLASS`, which is dead here (`IsEqippedMemo_` exists), so the June branch loses nothing. The delete-key tail stays shared after the `#endif`.

Hygiene: `flagcheck.py JUNE_EFFECT_SET_LIST_VARIANTS --range 31c8cc86^..31c8cc86`: `files: 3 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `enumscan --check X2GUUser.cpp X2GUUser.h X2GameUnit.h`: 2 B rows (`X2GUUser.h:2544-2545`, `f07940fe`, `JUNE_EVENT_STATES`), none in CH1r lines; fresh `cl /P` runs failed transiently (exit 2, empty output), so this result is from `--reuse`. `/Zs` `X2GUUser.cpp`, `X2GUNPC.cpp`: 0 errors. Deployed exe stays md5 `52ee62ae` (play-test PASSED on it). `CH1s.md` re-checked: still matches (CH1r touched no gage code).

### CH1s - Done (2026-10-27, diagnostic; play-test pending)

Opus 5.5. Reading found no defect in CH1q's port; a temporary `SERV_IRUHADEV_JUNE_CHAR_EVE_DEBUG` line settles it in one play-test.
- **Gage data `+0xf4` is the orb count.** User vslot 0xa8 `3208eef0` = `SetHyperModeCount( n )`: `+0xc04` (gage data) `+0xf4 = n`, then the encrypted mirror at `+0x7eb4`.
  That is March's `CX2GUUser::SetHyperModeCount` (`GetGageData()->SetHyperModeCount`, `X2GUUser.h:1637`); the gauge set copies it into the UI's data every frame
  (`UpdateDataFromGameUnit`, `X2GageData.cpp:237`). June's Raven ball `32e6d900` tests the same byte where March's Raven tests `GetHyperModeCount()`.
  `HyperModeStart` zeroes the count (`X2GUUser.cpp:32578`, June `31ffd490`), so **the ball shows while 3 orbs are held and goes when the awakening starts**: an
  "awakening ready" marker, never visible during the awakening. CH1's play-test looked during the awakening, so it may have seen June's behaviour.
- **First gate:** `DAT_3372c0fc` + `type_info` = the `g_pX2Game` observer (the same gate opens Raven's `32e6d900`); the port's `NULL == g_pX2Game` test.
- **Particle system:** `322ceaa0` = `g_pData+0x84` = March `GetGameMajorParticle()`, confirmed independently: June's Elesis Extinction `32b706a0` creates
  `SA_SSK_EXTINCTION_CHARGE_P15` on `[g_pData+0x84]`, March on `GetGameMajorParticle()` (`X2GUEL.cpp:6718`). `Eve_PowerUp_UI_01` is a `PT_2D_PLANE` (RHW, screen
  coordinates) in `GameMajorParticle25.txt`, opened at start-up under `JUNE_LOADER_GAPS` (`X2StateStartUp.cpp:166`).
- **Create arguments:** owner 0, ( 72 x `+0x1b8`, 80 x `+0x1bc`, 0 ) = ( 72, 80 ) x resolution (`33534178` = 72.0, `33438090` = 80.0), defaults, OverUI (`+0x170`). As ported.
  The owner only matters to `GIANT_UNIT_GIANT_EFFECT_TEST` scaling; a 0-radius sequence passes the DG frustum test (`KTDGObject.h:512`).
- **Diagnostic:** `[JUNE_CHAR] EVE NCORE ball ...` once a second from `June_UpdateHyperBall` (before its gate): game, count, handle, templet, seq, live particles, GetShow,
  hide-dialog, seq ShowObject, drawn last frame (`IsInstantShow`), OverUI, layer, pos. Flag defined in `Always.h`, include + block in `X2GageUI.cpp`.
- **Drift (not Eve's):** March's Raven ball makes `raven_powrUp_UI_01` on `GetUIMajorParticle()`, but the template is only in `GameMajorParticle7.txt`, so March's
  Raven ball is never created; June uses the game system at ( 74, 60 ). Routed to the checklist, new **7aq** (Raven's port).

Hygiene: `flagcheck.py JUNE_CHAR_EVE_DEBUG --worktree --files X2Lib/X2GageUI.cpp KTDXLIB/Always.h`: `files: 1 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Build: `DEPLOY OK: X2_offline.exe is 26,082,304 B, dated 2026-10-27 12:16:44, md5 fcb84b19f5b00c09a35a0557250fee11, equal to X2/US_SERVICE/x2.exe (2026-10-27 12:16:38); built at main 2b623204 + 2 uncommitted code file(s).` `EVE NCORE ball` is in the exe once.
**Play-test (user) is the gate:** one run, the log line before and during the awakening decides between "June's rule, nothing to fix" and a render / data fix. Review: `kickoffs/char-eve/CH1s-review.md` (retires `_DEBUG`).

**Play-test 1 (user, 2026-10-27, run 12:28-12:30 on md5 `fcb84b19`):** the glow works as June has it (log: count 3 -> handle, seq, 4-5 live particles, drawn, OverUI, layer 14, pos 90 75; count 0 -> destroyed).
The user's finding was a different piece: **Eve's own third orb (the core slot) is missing, only 2 orbs show.** Cause: data, not code. March's `DLG_PVP_Game_My_State_NEW.lua` (data034) has no
`Bg_Eve_Core_Active` / `_InActive` pictures, so `StaticPVPGameStateWakeOrb` / `WakeSlot` end at index 5; `CX2EveMyGageUI::InitWakeOrb` removes 5, 4, 3, 2 expecting index 6 (`PGUWO_ORB_EVE`)
to slide down to the third orb, and only 0 and 1 are left. Every class's `InitWakeOrb` already removes index 6 first (ADD_EVE_SYSTEM_2014 is on with NCORE), so the code wants June's 7-picture dialog.
- **Fix (June same-name rule, no flag):** `luasrc.py adopt-june DLG_PVP_Game_My_State_NEW.lua` (June bytecode, record `june2017_re/june_adopt_20261027_123538.tsv`) and `kom.py stage-june`
  `DLG_UI_Common_Texture80_NEW.TET` / `.tga` (data057; June's TET = March's keys incl. `BG_EVE_CORE_*` + `BT_TALK_*`). Every control the C++ reads off `m_pDLGMyUnit` is in June's dialog or in the
  class dialogs opened on top (`DLG_Ara_ForcePower.lua` ...); June's `GAGE_UI_CUSTOM_MSG` / `UI_TOOLTIP_TYPE` tables are in Enum.lua. `PACK OK data034` (~1 changed), `PACK OK data057` (~2 changed). MODS.md entry added. `COPY OK data034 ... 981 entries, md5 10ae2dadc3908a147a272a8f61fa9bd9`, `COPY OK data057 ... 927 entries, md5 f039da1df3b1a58564afda846ed5f682` (user approved).
- **Drift:** June's two invisible tooltip buttons (`opacity1` over the orbs, `opacity2` over the bars; msgs 54002 / 54003, `UTT_WAKE_ORB`) have no March handler, so no wake-orb tooltip: UI sweep (Stage 9).
- **Play-test 2 (user, 2026-10-27, run 12:39-12:40 on exe `fcb84b19` + the copied koms): PASSED.** Three orbs, Eve's core third, plus the glow; logs clean (0 EXCEPTION, the known HB check only, no ErrorLog line for the dialog or Texture80).
- **Standing rule added (user, 2026-10-27):** assume March does not have an asset ported code uses; check it by name before debugging code, stage June's if missing, replace with June's if present (CLAUDE.md).

### CH1s - Review (2026-10-27)

Opus 5.5. CH1q's `June_UpdateHyperBall` and CH1s's findings were read against June `32e6a210`, `3208eef0`, `31ffd490`, `32e5cfe0` and `31d03920` (Ghidra decompile and disassembly; bytes and constants read from `unpacked_x2.exe`). **Verdict: PASS, no transcription error; the diagnostic is retired.**
- **`+0xf4` is the orb count.** The raw bytes of `3208eef0` (no Ghidra function) are `mov eax,[ecx+0xc04]` / `mov edx,[esp+4]` / `test eax,eax` / `jz` / `mov [eax+0xf4],dl`, then the mirror `lea esi,[ecx+0x7eb4]`. This is `SetHyperModeCount`. `HyperModeStart` `31ffd490` calls vslot 0xa8( 0 ) (funcmap l.4580 / 4643). So the ball shows while 3 orbs are held and goes away when the awakening starts. This is June's rule.
- **Gate and destroy:** the `DAT_3372c0fc` observer + `type_info`, then `322ceaa0` != 0, then gage data `param_1[4]` != 0. That is the port's `g_pX2Game` / `GetGameMajorParticle()` / `m_pGageData` test. When the count is not 3, `32e5cfe0` destroys handle `+0x5c` on `[g_pData+0x84]`. This matches the port's `DestroyInstanceHandle( m_hSeqHyperBall )`.
- **Create:** the args are ( 0, `Eve_PowerUp_UI_01`, ( 72 x `+0x1b8`, 80 x `+0x1bc`, 0 ), -1.0f, -1.0f, -1, -1, -1.0f, true, 0, false ). `334380f0` = -1.0f, and `33534178` / `33438090` are the doubles 72.0 / 80.0. These are March's `CreateSequenceHandle` defaults (`KTDGParticleSystem.h:1959`). After it, the byte at `+0x170` is set to 1 (OverUI). The else branch does `SetPosition` with the same vector. All match the port.
- **Show block:** vslot 0x1c (`GetShow`) == 1, then `31d03920` on `g_pKTDXApp+0x1fc -> +0x38c`, which returns `+0xbc == 1 && +0x34 == 0`. That is March's `GetHideDialog` (`m_bCanHide && !m_bShow`, `KTDGUIDialogManager.cpp:1602`). `+0x172` is then set to !hidden, with vslot 0x28 on a change; this is `SetShowObject` with `NotifyShowObjectChanged` (`KTDGObject.h:324`). This matches the port.
- **Play-test fix:** this is June's behaviour, not an engine gap. It is a same-name asset replacement with no code change. data034 `DLG_PVP_Game_My_State_NEW.lua` sha1 `28b6cbc7`, header `1B 4B 4C 81` = `june_adopt_20261027_123538.tsv`. `kom.py status data034` / `data057`: in sync, +0 / -0 / ~0 vs live.
- **Retired `SERV_IRUHADEV_JUNE_CHAR_EVE_DEBUG`** by reversing `53743937`'s two code hunks exactly (`git apply -R`; neither file had changed since). `Always.h` and `X2GageUI.cpp` are now byte-identical to `53743937^` and still CP949. `enumscan --check X2Lib/X2GageUI.cpp`: 0 rows. No `_NCORE` change.

Build: `DEPLOY OK: X2_offline.exe is 26,081,280 B, dated 2026-10-27 12:55:07, md5 2ffc4610245c1f48c3202d08f5c12e80, equal to X2/US_SERVICE/x2.exe (2026-10-27 12:55:01); built at main 54b84dac + 2 uncommitted code file(s).` `EVE NCORE ball` is not in the exe.
Hygiene: `flagcheck.py JUNE_CHAR_EVE_DEBUG --range 53743937^..83df2d01`: `files: 2 PASS, 0 PASS~, 2 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `28 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 encoding change(s)`. Its `RESULT: FAIL` is only `define: NOT FOUND`, which is expected for a retired flag. Net zero: `git diff 53743937^ 83df2d01 -- KTDXLIB/Always.h X2Lib/X2GageUI.cpp` is empty, and the flag is in no tracked file.
`kickoffs/char-eve/CH1-review.md` checked: it still matches. It now also checks CH1s's data034 / data057 fix and notes that the ball log line is gone.

### CH2a - Done (2026-10-27, Sonnet 5.5)

**Landed** (flag `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL`, needs `_CORE`, `SERV_IRUHADEV_JUNE_SKILL_NUMERICAL`, `SERV_IRUHADEV_JUNE_BUFF_BINDING`). Funcmap rows `Char Eve CH2a` (9), JUNE_BUGS 444-446, `MODS.md` section "June Eve: Nasod skills". Checklist rows marked `(Eve: CH2a)`: §7 L274 / L281 / L282, §7c (3 of 9 sites), §7ai.
- **Scope split.** 10 of the 12 ids are done (Dimension Link 25000 / 25030, Illusion Stinger 25001 / 25031, Guardian 25002 / 25032, Photon Blink 25200 / 25213, Conversion 25203 / 25216). **Mega Electronball 25207 / 25219 is not registered: its FrameMove `329b1e20` (1178 instructions, a rewritten mechanic with several unmapped June helpers) needs Opus -> new phase `CH2a3`** (`kickoffs/char-eve/CH2a3.md`). Its Init and Start are written, inert; StartFuture / FMF / EP / EndFuture are March's.
- **Registration.** `June_SetEquippedSkillFuncHead` cases per `329c8a00` (table names, callbacks, hyper copies as June writes them), each logging `[JUNE_CHAR] EVE SKILL registered <state> ( skill N, slot S A|B )`. Conversion also sets `+0x9c58` / `+0x9c5c` from the numerical rows `HP_Consume` x0.01 / `MP_Gain` through `June_GetSkillNumericalByName` (`32007910`).
- **June bodies written (7 + 2 inert):** Stinger Init, Guardian Init + FrameMove (effect set, power x `+0x4064`), Dimension Link FrameMove (March's particles + alpha tags, ground-snapped `CreateNPCReq` NPC 384, Rule Break 25418 proc -> NPC 519, BFI_BUFF_SENTRY at the learned level, BTI 410 time = `+0x9c4c` x 20 / 60 + `+0x4070`), Conversion Start, Photon Blink Start + FrameMove (dummy effects, `MI_CHECK_AND_KILL_1` time add, upgrade's two `DUMMY_REAL`), Mega Init + Start.
- **= March, read in the listing (17):** the Stinger Start / End / EventProcess, Dimension Link Init / EventProcess, Conversion Init / EventProcess, Photon Blink Init / StartFuture / FMF / EP / EndFuture / End, Mega StartFuture / FMF / EP / EndFuture. `32966440` / `32966900` forwarders and `ret`s are left empty (engine runs CommonFrameMove). Guardian's EventProcess `3296f0e0` = `JuneGenericSpecialActiveSkillEventProcess` (Eve's `m_Wait` is `USI_WAIT`).
- **Kept as June has it:** the hyper states of Stinger and Guardian read the normal script table; Photon Blink's memo buff is dead; Dimension Link never retires the old sentry (JUNE_BUGS 444-446).
- **Names/members added:** `RNSI_SA_EVE_ILLUSION_STINGER(_UPGRADE)`, `RNSI_SA_EVE_DIMENSION_LINK_GUARDIAN(_UPGRADE)`, `RNSI_P_EEP_CHEAT_CODE_RULE_BREAK`, `BFI_BUFF_SENTRY`; `m_fJune4070` (CH4a writes it; kickoff amended).
- **Assets:** every name the new code and the 5 tables use is in `ch0_assets.tsv` / `ch0_effect_closure.tsv` (112 rows, all `june-bytes`); `EVE_PHOTON_BLINK_DUMMY_REAL`, `EffectSet_SA_EVE_DemensionLinkGuardian`, `EffectSet_SA_EVE_IllusionStinger` are in the live data036 files. No kom change in this phase.
- **Not ported / routed:** `32988e40` (a second Dimension Link FrameMove, unreferenced by any table: dead, 2 of the 167 alpha sites); `3297b840` stays CH2a2's (the FrameMoves call March's `ShowActiveSkillCutInAndLight`).

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_SKILL --worktree --files <5 code files>`: `files: 2 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (OTHER = the two enum-name hunks under `SERV_IRUHADEV_JUNE_ENUM_NAMES`). `enumscan --check X2Lib/X2GuEve.cpp`: 0 A/B rows. `cl /Zs X2GuEve.cpp`: 0 errors.
**Deploy:** DEPLOY OK: X2_offline.exe is 26,090,496 B, dated 2026-10-27 13:53:54, md5 401a046129132098a4f61d5d6c958d41, equal to X2/US_SERVICE/x2.exe (2026-10-27 13:53:47); built at main 2540125f + 5 uncommitted code file(s). The new strings (`EVE SKILL registered`, `EffectSet_SA_EVE_DemensionLinkGuardian`, `EVE_PHOTON_BLINK_DUMMY_REAL`, `HP_Consume`, `MP_Gain`) are in the deployed exe. **In game:** nothing changes for kom data.

**Play-test PASSED (user, 2026-10-27, run 14:06-14:07 on md5 `401a0461`):** `[JUNE_CHAR] EVE SKILL registered` for Photon Blink 25213 (slot 0 A), Dimension Link 25030 (1 A), Illusion Stinger 25031 (2 A), Guardian 25032 (0 B), Conversion 25203 (1 B); `ErrorLog.txt` has no line matching `EVE SKILL`-related errors, 0 EXCEPTION, only the known `E_DISABLE_HB_CHECK_REQ` UNHANDLED. Base ids (25000 / 25001 / 25002 / 25200) and the B-slot upgrades were not equipped in the run.

**Play-test hand-off (as given).** Save: unit 22 `weevwe` (Eve Nasod, level 99) has Photon Blink Enhanced (slot 0), Dimension Link Enhanced (1), Illusion Stinger Enhanced (2), Dimension Link Guardian Enhanced (5) and Conversion (6) equipped (backup `db_backup\els_db_20261027_134652_pre_sql.sql`). In a dungeon (`/stage 1`, `/invin 0`), with infinite MP/cool on (dev gate): see the checks in the reply; log tag `[JUNE_CHAR] EVE SKILL`.

### CH2a - Review (2026-10-27, Opus)

**PASS, one transcription fix** (flag `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL`). Read in the listing (capstone `disasm_range.py` where Ghidra has no function):
- **Registration (`329c8a00`).** The five cases match slot for slot through the thunks. Normal and hyper table names are right (Stinger and Guardian use the normal name, JUNE_BUGS 444). The hyper copies are Stinger Start / End / FM / EP, Guardian FM / EP, Dimension Link FM / EP, Conversion Init / Start / EP and Photon Blink everything but Init. Conversion's `HP_Consume` x 0.01 -> `+0x9c58` and `MP_Gain` -> `+0x9c5c` are read at `@329c9188`, not `@329c9a64` (comment and funcmap corrected).
- **Fix: Guardian EventProcess `3296f0e0` is not `31fcc740`.** Besides the anim accessor (inert for Eve) and the literal `USI_WAIT` (= Eve's `m_Wait`), Eve's copy tests the cancel windows in a different order: `+0x8c94` back-walk, then `+0x8c98` walk, then `+0x8c9c` dash. The generic tests back-walk last. The order matters when back-walk and dash windows overlap and the player presses back and double-tap together: June waits, the generic dashed. New `CX2GUEve::June_EveSpecialActiveSkillEventProcess` (June `3296f0e0`); Guardian is its only registrar (`@329c8e90`).
- **`32989b60` Dimension Link.** 0.2 / 0.4 s `else if`; offsets -110 / -100 / 90, y +110. The snap is `GetLandPosition( pos, true, 30, &line )` from `lastTouchLineIndex`, keeping y = max. `CreateNPCReq( IsMyUnit gate, 384, hyper, true, pos, right, 0, true, 1, team, NAT_ALLY, uid, false, TN_NONE, 3 )`. Rule Break 25418 procs when rand < `SA_EFFECTIVE_RATE` (rate read first) -> NPC 519. Templet 25000 swap; BFI 439 `( creator 0, level, 0, skip 0 )` = March `( 439, level, NULL, 0, false )`. BTI 410 time = `+0x9c4c` x ( 60 if state skill 25030 else 20 ) + `+0x4070`. `ChangeBuffFinalizerTempletPtrList` also runs on an empty list. No old-sentry retire (446).
- **`329780b0` Conversion.** No templet -> `StateChange( 0xd )`. The 25420 roll fires when rand < rate; HP x `SA_DAMAGED_REL`. The Vengeance MP comes before `UpNowHp( -hp, 0 )`. MP x `SA_MP_RETAKE_ABS`, then vslot 0x23c `( 25420, 0, 1 )`, then `UpNowMp`.
- **`32993020` / `32973150` Photon Blink.** The upgrade (25213) has two `DUMMY_REAL` at -/+75 per facing; all three go in the vector. `31fdbf20( state skill, 0xf )` adds the dummy life time, else replaces it. The alpha ladder is 0.03 / 0.3 / 0.8, x10 / x5. The 0.59 s BFI 418 is behind the stub `31fca9e0` (always false, 445). Start: level of 25200 at least 1, `SA_TELEPORT_RANGE_ABS` (0x17) / `SA_EFFECTIVE_TIME` (1).
- **`3298b9b0` Guardian FM.** 0.01 s; power = `GetPowerRate` x `+0x4064`; handle -> `+0x9c30`. **`32007910`.** It matches June, but it is a June `CX2GUUser` function with 9 more call sites in other classes' code (not "one caller"). It stays an Eve member until a second class port needs it; the funcmap row says so.
- **"= March" (17).** Every pair was read against March's member, the Mega four included (EndFuture order, FMF `>= 0.66`, EP 0.4 / 0.36). All match. Init lists `329663c0` / `329665b0` / `32966910` match the strings and order; Mega Start rank ladder ok.

**Hygiene.** CH2a commit: `flagcheck --range 62ed7ddd^..62ed7ddd`: `files: 2 PASS, 0 PASS~, 1 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s) ... 0 encoding change(s)`; `RESULT: PASS`. Fix, `--worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h`: `files: 2 PASS ... 0 FAIL`; `2 deleted line(s)` (the registration line and the `@329c9a64` comment, both inside this flag's own block), `0 non-ASCII`, `0 EOL-mismatched`, `0 encoding change(s)`; `RESULT: PASS`. `enumscan --check X2Lib/X2GuEve.cpp`: 0 A/B rows.
**Deploy:** DEPLOY OK: X2_offline.exe is 26,091,008 B, dated 2026-10-27 14:30:51, md5 017179ff86069117c095deccc846ff1c, equal to X2/US_SERVICE/x2.exe (2026-10-27 14:30:44); built at main c08d07e0 + 2 uncommitted code file(s).
**In game:** Guardian (`Dimension Link Guardian`) plays as before. Only a simultaneous back + double-tap inside overlapping cancel windows behaves differently, so no play-test is owed. Next: CH2a2 and CH2a3 (kickoffs present, nothing to amend).


### CH2a2 - Done (2026-10-27, Sonnet 5.5)

**Landed** (flag `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL`, no new flag; needs what CH2a needed). Funcmap rows `Char Eve CH2a2` (6), JUNE_BUGS 447 / 448, `MODS.md` section "June Eve: Exotic Gear skills". Checklist §7c marked `(Eve: CH2a2)` (all 15 sites of the phase's functions).
- **Registration.** `June_SetEquippedSkillFuncHead` cases per `329c8a00` for Queen's Throne 25003 / 25037, Explosion Impact 25004 / 25040, Atomic Blaster 25005 / 25049, Hornet Sting 25006 / 25050, Black Hole 25007 / 25051, Metal Dust Aura 25204 / 25218 ( 12 ids, 11 new `RNSI_*` names in `X2SkillTree.h` under `SERV_IRUHADEV_JUNE_ENUM_NAMES`, values checked against `Enum.src.lua` ). Table names are read from the exe's strings, the state record slots from the listing ( `+0x18` Init, `+0x28` Start, `+0x38` FrameMove, `+0x48` EventProcess, `+0x58` End ). Each case logs `[JUNE_CHAR] EVE SKILL registered <state> ( skill N, slot S A|B )`.
- **June bodies written (9):** the cut-in helper `June_ShowActiveSkillCutInAndLight` ( `3297b840`, with the by-script `3297baa0` under `#ifdef`, Eve's own table, the item override left for Stage 12 and the code event 0x8b3 pair for CH5 -> CH5 kickoff amended ), the Init lists of Black Hole / Hornet Sting / Explosion Impact / Queen's Throne, Queen's Throne FrameMove ( `3299d3f0`, 13 alpha tags ), Atomic Blaster FrameMove ( `3298f220` / `3298f8f0` as one body with `bHyper`; dummy +-70 along the facing, force down at creation ) and Metal Dust Aura FrameMove ( `3296fda0` / `3296ff50` ).
- **= March, read in the listing (24 callbacks):** every EventProcess stub, Explosion Impact / Queen's Throne Start and End ( `+0x9620` / `+0x9628` are `MajorMID_ExplosionImpact01` / `EG_SP3_Wing` ), Metal Dust Aura Init / Start / End / EventProcess ( normal and hyper ), Atomic Blaster Init / Start / End / EventProcess ( the BALANCE_CODE_NEMESIS_20121213 chain ); normal and hyper copies of each pair are identical in March too. `32966870` / `32966880` forward to `CommonFrameMove` and stay unset.
- **Kept as June has it:** Black Hole and Hornet Sting register no FrameMove ( their March timelines are script data in June ); Explosion Impact's hyper state has no Start / End; Queen's Throne's hyper state reads the normal table; its 0.5 s branch is empty; the Metal Dust memo test is June's stub ( JUNE_BUGS 447 ); the hyper Atomic Blaster dummy skips the NEXT tables ( JUNE_BUGS 448 ). Queen's Throne's memo scaling of the life time runs through `GetJuneSkillMemoEntry`, our stub that returns null, so it is inert ( not a June bug ).
- **Upgrade ids** reach their base through `GetEquippedActiveSkillTemplet( id, true )` ( Atomic Blaster 25005, Metal Dust 25204 ) and `GetSkillLevel( templet id, false, 0, true )`; the Queen's Throne memo reads `GetNowStateSkillID()`, which is the state's own id.
- **Assets:** the 613 closure rows tagged CH2a2 are `june-bytes` in the live koms. 7 rows are `missing` and June has none either ( `Eve_EG_SP3_QueensThrone4.ogg`, and the Metal Dust `CutTendon_Slash01.tga` / `Star.tga` / `SlashLight.tga` preloads, a miss in June's own data ): unchanged by this phase.
- **Not ported / routed:** the cut-in item override `@3297b908-3297b978` ( Stage 12, as CH1l40 routed ); the 0x8b3 cut-in pair ( CH5 ); `329cd890` / `329cd9c0` are the copies of that descriptor ( no March counterpart needed ).

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_SKILL --worktree --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h X2Lib/X2SkillTree.h`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` ( OTHER = the 11 names under `SERV_IRUHADEV_JUNE_ENUM_NAMES` ). `enumscan --check X2Lib/X2GuEve.cpp`: 0 A/B rows. `cl /Zs X2GuEve.cpp`: 0 errors.
**Deploy:** DEPLOY OK: X2_offline.exe is 26,100,736 B, dated 2026-10-27 15:04:29, md5 f42027171c3598be5d072254e87bd6ca, equal to X2/US_SERVICE/x2.exe (2026-10-27 15:04:21); built at main e9ac7abb + 3 uncommitted code file(s). The new strings (`EVE SKILL registered`, `Mesh_Eve_EG_Sp3a_QT_UpLight01`, `EVE_NS_ATOMIC_BLASTER_DUMMY_HYPER`) are in the deployed exe. **In game:** nothing changes for kom data.

**Play-test hand-off (as given).** Save: unit 57 `EveExotic` ( Exotic Gear, level 99 ) has Queen's Throne Enhanced ( slot 0 ), Explosion Impact Enhanced ( 1 ), Hornet Sting Enhanced ( 2 ), Black Hole Enhanced ( 3 ) and Metal Dust Aura Enhanced ( B slot 5 ) equipped; unit 59 `EveNemesis` has Atomic Blaster ( B slot 5 ) ( backup `db_backup\els_db_20261027_145807_pre_sql.sql` ). In a dungeon ( `/stage 1`, `/invin 0` ), infinite MP / cool on ( dev gate ): log tag `[JUNE_CHAR] EVE SKILL`.

### CH2a2 - Review (2026-10-27, Opus 5.5): PASS, no code change

Read against the listing / raw disassembly, not the Done note's claims.
- **Registration** (`329c8a00` cases `@329c8f76` / `8efd` / `95d6` / `8eb0` / `8e11` / `9407`): all 11 table names read from the exe; all 35 stored thunks resolved to their targets, each = the port's cite and slot (`+0x18` / `+0x28` / `+0x38` / `+0x48` / `+0x58`). Queen's Throne hyper copies Start / End / FrameMove / EventProcess and no Init, under the normal name; Explosion Impact normal and hyper FrameMove are `32966870` / `32966880` = `jmp [vtbl+0x7cc]`, and a null OnFrameMove falls to `CommonFrameMove()` (`X2GUUser.cpp:7441`), so leaving them unset is equal. Correction to the kickoff: the Atomic Blaster hyper record reuses the normal Init / Start / End thunks (`329cbc70` / `329cefb0` / `329cbc80`); only Metal Dust has its own, and the port does both right.
- **`3299d3f0`:** constants by operand width (`charport_consts.py`): 0.02 / 0.06 / 0.5 / 1.3 / 1.4 / 1.9, x - 20, x - 15 / y + 155, camera ( 20, 0.3 ); if / else-if chain with the empty 0.5 branch; weapon strings `_01`..`_04` in order, booster on `_03` at its `Dummy01`, handles `+0x96bc..+0x96cc`; 1.9 s order (destroy, bone, `3298cfe0` on `+0x96a8`, `+0x96b4` = 1, memo type 0xf = `MI_CHECK_AND_KILL_1`, `fld dword` x `fmul qword`, weapons, `clock()`, vslot 0x23c ( 0x6348 = 25416, 0 / 1, 1 )); 13 tags (7 x `32296ee0`, 5 x `32296d60`, 1 x `32296da0`) at the port's places.
- **`3298f220` / `3298f8f0`:** 0.01 / 0.07 / 70 / 0.89 / 1.2 / 1.62; vtbl 0x170 false -> bone - dir x 70, else +; dummy gate = June's `!timer || handle set` form; 0x61ad = 25005 swap, level `< 1 -> 1`, `32206e50( 0x2a, lv, 0 )` with SA_FORCE_DOWN_ABS = 42; normal writes `+0x5f0` and loops `+0xd8..+0xdc` (`+0x3f0` = fForceDownValue), hyper writes `+0x5f0` once (JUNE_BUGS 448 real). The 0.15 constant `charport_consts` lists after `3298f8f0` belongs to `3298ff60`.
- **`3297b840` / `3297baa0`:** gate = inlined `EventTimerOneshot` (`31d28e10`) && `32001380` && `31fd3180`; entry `0x337abc00 + ( idx + [+0x8ac8] x 3 ) x 0x30`; the item override (`31fd8a50` > 0) and the 0x8b3 pair are the only blocks left out; `3297baa0` = ( `Dummy2_Lhand`, t, `[+0x8acc]`, light ).
- **Metal Dust `3296fda0` / `3296ff50`:** byte-identical, 0.333, 0x6274 = 25204, 0x93b53 = 605011, `31fca9e0` = `xor al, al; ret 4` (JUNE_BUGS 447 real), vslot 0x228. **Init lists:** the four lists = June's strings, order and loader per call.
- **"= March" spot-checks:** Metal Dust EventProcess `3296fe20` (14 keys, 0.66 s), Atomic Blaster EventProcess `32971910` (anim end, attack `+0x8c8c` with 14 keys, skill `+0x8c90`, back-walk `+0x8c94`, walk `+0x8c98`, dash `+0x8c9c`), Atomic Blaster Start `3298f1c0` (3 handles), Explosion Impact / Queen's Throne Start / End (`+0x9620` / `+0x9628`, Common Start / End order kept).

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_SKILL --range dc16df68^..dc16df68`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`; `RESULT: PASS` (OTHER = `X2SkillTree.h` names under `SERV_IRUHADEV_JUNE_ENUM_NAMES`); no later commit touches the flag. `enumscan --check X2Lib/X2GuEve.cpp`: 0 A/B rows. No rebuild: the deployed md5 `f4202717` is the reviewed code.
**Play-test PASSED 2026-10-27** (user + logs, run 15:34): the six `EVE SKILL registered` lines (25037 / 25040 / 25050 / 25051 / 25218 on unit 57, 25005 on unit 59); ErrorLog 0 Motion Error / KEM; 0 EXCEPTION, only the known `E_DISABLE_HB_CHECK_REQ` UNHANDLED. Next: `/x2-phase char-eve CH2a3` (Opus 5.5 high), then `CH2b0`; both kickoffs are in place.

### CH2a3 - Done (2026-10-27, Opus 5.5)

**Landed** (flag `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL`; temporary `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL_DEBUG`, retired in the review). Funcmap row `329b1e20` (PORTED, the whole decode), JUNE_BUGS 449, `MODS.md` "June Eve: Mega Electronball". Checklist: §7c (the last 6 Eve sites of CH2a's 9) and a §7ai note.
- **Registration.** The case 25207 / 25219 in `June_SetEquippedSkillFuncHead` per `329c8a00 @329c9225`: both tables (`ENSI_SI_A_EN_MEGA_ELECTRONBALL` / `_HYPER`) take all seven callbacks, Init included: Init `32966910` and Start `3296f9a0` (CH2a's), FrameMove `329b1e20` (new), StartFuture / FMF / EP / EndFuture = March. Log `[JUNE_CHAR] EVE SKILL registered ENSI_SI_A_EN_MEGA_ELECTRONBALL ( skill N, slot S A|B )`.
- **`329b1e20` -> `June_ENSI_A_EN_MEGA_ELECTRONBALL_FrameMove`.** The function ends at `329b2e0f`. The kickoff's "BFI 0x28c" and the `322e67c0` packet call are in `329b2e10`, Oberon Guard's FrameMove (CH2b's): there is no int3 padding between the two, so CH0's tables merged them. The kickoff's "`JuneUpNowMpWithText` on failure" is wrong: the failure is `CreateNotEnoughMPEffect` (`32007d80`), and `32278070` is Open Mind's refund.
  - 0.22 s: the plane. A shot fires on oneshot 0.23 s, or on oneshot 0.48 s when the upgrade's proc armed `+0x978d`.
  - MP is the templet's cost at every rank (0 after 0.3 s: the second ball is free). It is then x class 19 / 109 rate, x memo rate, x BBT 63 rate, x `( 1 + 32358970( 0xb ) x 0.01 )`.
  - The rank scales only size and range. Open Mind gives its refund. The upgrade's proc is `SA_EFFECTIVE_RATE` >= `GetRandomFloat( 0 )`.
  - Particles: three at the hand. The ball is x1.5 power and size on the BIG roll (`+0x97bc`). Power is x0.33 in PvP.
  - Each of Eve's Nasod watches (vslot 0x77c, `June_Vslot77c( UC_EVE_CODE_EMPRESS )`, a literal in June, so every Eve class qualifies) fires its own ball from its position + 150, with its own rotation and land y.
  - Every ball gets `SetInheritForceDownRate( true )` and `_Filtered` x4; then the sound.
- **Names reused:** `June_GetFormationFactor`, `JuneUpNowMpWithText`, `GetJuneSkillMemoMPRate`, `GetSkillRateByBuffBehavior`, `June_GetSkillLevel` / `June_GetLearnedSkillAbilityValue`, `JuneTagEffectAlphaParticle`, `m_fJuneElectronBallBigRate`, `m_iDeleteSkillIDByUpgrade`. New, file-local in `_CONST_EVE_JUNE_CH2a3_`: `JuneEveTryGetAcquiredSkillAbilityValueSwap` (`32007c40` with its swap argument, which the CH1l copy drops) and the particle helper.
- **Kept / not kept:** JUNE_BUGS 449 is kept: a watch's two lights are drawn at Eve's hand, shifted +10 x per watch. June reads the templet, and scales the pushed ball, without a NULL check. The port checks both: on a null templet the shot is skipped and `CommonFrameMove` still runs. `32358970` stays 0, as in `GetActualMPConsume`.
- **Assets:** all 6 names (`DamageEffect_A_EEG_MegaElectronBall`, `_Filtered`, `Ring_Eve_SI_A_EN_MEB01`, `Light_Eve_SI_A_EN_MEB02` / `03`, `Eve_FireShot1.ogg`) are `june-bytes` rows of `ch0_effect_closure.tsv` / `ch0_assets.tsv`. No kom change.

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_SKILL --worktree --files <3 code files>`: `files: 2 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `enumscan --check X2Lib/X2GuEve.cpp`: 0 A/B rows.
**Deploy:** DEPLOY OK: X2_offline.exe is 26,105,856 B, dated 2026-10-27 16:02:06, md5 379019c0ff45d1d59224861c4aee2624, equal to X2/US_SERVICE/x2.exe (2026-10-27 16:01:59); built at main fbb83e95 + 3 uncommitted code file(s). The strings `EVE SKILL MEGA shot`, `DamageEffect_A_EEG_MegaElectronBall_Filtered` and `ENSI_SI_A_EN_MEGA_ELECTRONBALL_HYPER` are in the deployed exe.

**Play-test hand-off (as given).** Save: unit 22 `weevwe` (Eve Nasod, level 99) now also has Mega Electronball Enhanced 25219 in slot 7 (B, third key); Dimension Link Enhanced is in slot 1. The backup is `db_backup\els_db_20261027_155426_pre_sql.sql`. Log tag `[JUNE_CHAR] EVE SKILL`.

**Play-test PASSED 2026-10-27** (user + logs, run 16:14-16:17 on md5 `379019c0`):
- Registration: `EVE SKILL registered ENSI_SI_A_EN_MEGA_ELECTRONBALL ( skill 25219, slot 2 B )`.
- 13 `MEGA shot` lines, ranks 1 / 2 / 3. Scale 1.500 / 1.875 / 2.250 and range 1.250 / 1.406 / 1.563 are the x1.25 / x1.125 and x1.5 / x1.25 steps. MP is 50 at every rank.
- Second ball: armed twice. One fired at 0.49 s with `mp 0.00`; the other cast left the state before 0.48 s.
- Watch copies: `watches 1` with the sentry up.
- Out of MP: 6 `not enough MP` lines.
- `big` stayed 0: `m_fJuneElectronBallBigRate` is set by passive 25407 (CH4).
- ErrorLog 0 matching lines, 0 EXCEPTION, only the known `E_DISABLE_HB_CHECK_REQ` UNHANDLED.

### CH2a3 - Review (2026-10-27, Opus 5.5): PASS, no transcription fix; `_SKILL_DEBUG` retired

Checked against the raw listing `329b1e20-329b2e0f` (`charport_adis.py`), with stack offsets traced through each call, not against the Done note.
- **Registration** (`329c8a00 @329c9225-329c92e9`): table names `334d4310` / `334d42e8` = `ENSI_SI_A_EN_MEGA_ELECTRONBALL` / `_HYPER` (`ch0_skill.tsv:59`). Both records store the same seven thunks at `+0x18` / `+0x20` / `+0x28` / `+0x30` / `+0x38` / `+0x48` / `+0x50`, which resolve to `32966910` / `32966960` / `3296f9a0` / `3296fa90` / `329b1e20` / `3297cba0` / `32966990`, the port's slots and cites. Cases 25207 / 25219.
- **Gate:** the inlined `EventTimerOneshot` prelude (`+0x24c` < t) is March's `m_fAnimOneshotTimer < t && EventTimer( t )`. The 0.48 s shot needs `+0x978d == 1`. When `GetUnit()` is null the code jumps to `@329b2df1`, past `CommonFrameMove`.
- **MP:** the templet vector `+0x130` is indexed at level - 1 (0 when out of range) = `GetSkillMPConsumptionValue`. GetSkillLevel is called as `( 25207, 0, 0, 1 )` and floored at 1.
  - `0.3 < +0x214` (`m_fAnimCurrTime`) zeroes the cost. Then class 19 / 109 (the templet null-check is June's; March `GetClass` has none, and the templet always exists), memo, BBT 63, the `32358970` factor, in June's order.
  - `FlushMp( mp, 0 )`: March's one-argument form. On failure, `32007d80` at `Dummy2_Lhand` with 0 / 0 / 0, checked in Ghidra.
- **Rank** `+0x9788 - 2`: x1.25 / x1.125 and x1.5 / x1.25 on size (`0x20`) and range (`0x1f`). MP and power do not scale.
- **Open Mind:** `31fceab0( 25419 ) > 0`, then the factor 0 / 1 / 3 (`June_GetFormationFactor`). `32007b10( 66, 25419 )` x mp x factor goes to `32278070`. `32007b10` was checked in Ghidra (default 1.0).
- **Upgrade proc:** `32007c40( 5, now skill, 1, &f )`, checked in Ghidra: GetUserSkillTemplet( id, swap ) and IsPvpOrPve. The `fcompp` with `test ah, 1` gives `+0x978d = ( f >= rand )`, which is the port's `GetRandomFloat( 0 ) <= fRate`. A false return leaves the byte as it was.
- **Balls:** power = `+0x97c0` x `GetPowerRate`.
  - The BIG roll is `+0x97bc > GetRandomFloat( 29 )` for the main ball and `( size + 29 )` for each watch ball.
  - CreateInstance's tail is ( landY, 0, -1.0, `+0x97c4`, `+0x97ac` x range, -1, -1.0, 0, -1.0, 0, 0 ), which equals March's defaults.
  - The watch ball sits at the NPC's vslot 0x138 position + 150 y, with the NPC's rotation and the land y from vslot 0x134. Its power arg is the PvP-scaled one.
  - SetScale goes on `vector.back()`, unchecked.
  - Every ball gets `+0xacd = 1` and `_Filtered` x4. The sound is `( name, true, false )`.
  - JUNE_BUGS 449 is real: the watch lights are at `esp+0x50`, the hand point.
- **Enums / offsets** (by count, with tsv / header cites): 25207 / 25219 / 25419; SA 5 / 31 / 32 / 66; BBT 63; CR_RANK 2 / 3; UC 19 / 109; the `+0x97ac..+0x97c4`, `+0x99a8`, `+0x97e4` members; SkillTemplet `+0x178`; particle `+0x3e0` / `+0x404`.
- **Note, no change:** the constructor note above ("`+0x97ac` ... June-only with no known role") is stale. CH1d / CH4a map `+0x97ac` to `m_fElectronBallRangeRate`, and its June ctor value 0 vs March's 1.0 stand-in belongs to CH4a's `3297f290` writer.

**Retired** `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL_DEBUG`: the `Always.h` block and both log blocks are gone (17 lines; nothing else names the flag).
**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_SKILL --worktree --files X2Lib/X2GuEve.cpp KTDXLIB/Always.h`: `files: 1 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; 17 deleted lines = the retired debug lines; `RESULT: PASS`. `--range a5fa41d4^..HEAD`: `files: 2 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `RESULT: PASS`. `enumscan --check X2Lib/X2GuEve.cpp`: 0 A/B rows.
**Deploy:** DEPLOY OK: X2_offline.exe is 26,104,832 B, dated 2026-10-27 16:36:53, md5 21c41e26d434f159ef145f1efdffa7c4, equal to X2/US_SERVICE/x2.exe (2026-10-27 16:36:46); built at main 22734bd4 + 2 uncommitted code file(s). The string `EVE SKILL MEGA` is not in the exe any more.
User owes nothing: the `EVE SKILL MEGA` lines no longer print, and the play-test already passed. Next: `/x2-phase char-eve CH2b0` (Opus 5.5 high); the kickoff is in place.


### CH2b0 - Done (2026-10-27, Opus 5.5)

The one unmapped callee CH0 left for CH2b, `32009180` (`ch0_unmapped_identified.tsv`), is ported whole as `CX2GUUser::JuneCollectEnemyUnitsInCone( OUT vecUnits_, iCount_, fMaxDistance_, fMinAngle_, fMaxAngle_, bFacingOnly_, fConeDegree_, bSkipInvincible_ )` (`X2GUUser.cpp` / `.h`, flag `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL`; inert until CH2b's Faena FrameMove calls it).
- **What it is:** the twin of C3c's `CX2GameUnit::JuneCollectUnitsAround` (`32294e30`), in June's user-class range with 15 callers across several classes (funcmap row `32009180 (CH2b0)` lists them). The skip chain, aim point, facing test, NPC size and pitch band match the twin. It differs in three ways:
  - **enemies only:** an equal team skips, and there is no team argument;
  - **invincible skip:** `GetInvincible` (vslot 0x180) skips the unit when the last byte argument is set;
  - **yaw cone:** a test against my `GetRotateDegree().y` (vslot 0x150), both yaws wrapped to 0-360, the gap wrapped at 180, kept within +-cone x 0.5. The constants are qwords (0.0 / 360.0 / 180.0 / 0.5) and a NaN passes, as June has it.
- **Reused:** `JuneIsPosAlongLastTouchLine` (`32293b80`) and `CEffect::JuneGetLockOnAimPos` (vslot 0x3e4). `CEffect` gets a `friend class CX2GUUser` under the flag (`X2DamageEffect.h`), beside C3c's `friend class CX2GameUnit`.
  - Aisha's `CX2Game::JuneIsUnitInViewCone` (`322f0e60`) is a different test (a tan pitch band and the line direction), so it is not used here.
- **Precision and order kept from the listing:**
  - the HP test is `!( 0 < HP )`, so a NaN refuses;
  - the length is `( x*x + y*y ) + z*z`, stored as a float, then square-rooted;
  - distance `<=` keeps and a NaN refuses;
  - the sort is `std::sort`, which is what June's `322acc40` `std::_Sort` is (the twin uses `stable_sort`).
  - June's "gage present, HP gage `+0x20` NULL" skip has no March shape: March's gage data always carries HP, and with no gage `GetNowHp` returns 1, which passes as June's does.
- **JUNE_BUGS 450:** the count argument (stack `+0xa4`) is never read, like row 133.
- **CH2b:** its kickoff now names the symbol and Faena's call `@329c447d`: `( vec, 1, SA 0x1f, -SA 0x9f, SA 0x9e, true, 180.f, false )`, defaults 700 / 35 / 35.

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_SKILL --worktree --files X2Lib/X2GUUser.cpp X2Lib/X2GUUser.h X2Lib/X2DamageEffect.h`: `files: 3 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `enumscan --check X2Lib/X2GUUser.cpp`: 0 A/B rows. The `#error` guard's three flags (`_DAMAGEDATA`, `_DAMAGEEFFECT_RUNTIME`, `_CHAR_ARME_CORE`) are top-level `Always.h` defines.
**Deploy:** DEPLOY OK: X2_offline.exe is 26,107,904 B, dated 2026-10-27 16:54:40, md5 07902c6423f5aaa310a5081e84472256, equal to X2/US_SERVICE/x2.exe (2026-10-27 16:54:33); built at main 8603f26f + 3 uncommitted code file(s).
**Branches not ported:** the 14 non-Eve callers belong to their own characters' ports (Stage 7 / 8 checklists); the funcmap row names them. Nothing drifted.
User owes nothing (no play-test: nothing calls it yet). Next: `/x2-phase char-eve CH2b0 review` (Opus 5.5 high).

### CH2b0 - Review (2026-10-27, Opus 5.5): PASS after one transcription fix (HP NaN sense)

The whole listing `@32009180-3200977d` (RET 0x20) was read in Ghidra and compared with `CX2GUUser::JuneCollectEnemyUnitsInCone` line by line.
- **Arguments:** out `+0xa0`, max distance `+0xa8`, min / max angle `+0xac` / `+0xb0`, facing byte `+0xb4` (`== 1`), cone `+0xb8`, invincible byte `+0xbc`. `+0xa4` is never read (JUNE_BUGS 450 real).
- **Skip order:** NULL, `322ccf50` IsValidUnit, equal team (`+0x278` decrypt), vslot 0x174 `== 4`, gage `+0xc04` / HP gage `+0x20`, vslot 0x180 GetInvincible called before the `+0xbc` test. All match.
- **Fix: the HP test.** `fldz / fcomp hp / test ah,1 / jz skip` (`@320092d7-320092f0`) keeps the unit when C0 is set, i.e. `0 < HP` **or unordered**. A NaN HP is kept. The port's `false == ( 0.f < HP )` dropped it. It is now `GetNowHp() <= 0.f`, the same as the twin C3c. The Done note's "a NaN refuses" was wrong; the funcmap row is corrected.
- **Aim / facing / NPC size:** target vslot 0x3e4 ( out, my vslot 0x138( 0 ), L"" ); `32293b80` vs vslot 0x170; NPC `+0x264 == 1`, templet `+0x85f8 +4 != 0` skips, else ( `+0x3e34` + `+0x3e30` ) x 0.25. All match.
- **Pitch band:** `@3200945b` skips `z < min`, `@32009471` skips `max < z`, and a NaN passes both. Match.
- **Yaw cone:** the target yaw `< 0` (`test ah,5 / jp`: NaN and `>= 0` kept as is) becomes 360 - |y|, and vslot 0x150 `.y` gets the same; gap = |target - mine|. The gap wraps when `> 180` (`test ah,0x41 / jnz`). It skips on `gap < -h` (`@3200953f`) or `h < gap` (`@32009548`) with h = cone x 0.5. `charport_consts.py --fn 32009180` gives qwords 0.25 / 0.0 / 360.0 / 180.0 / 0.5. Match.
- **Distance:** the x87 order is ( x*x + y*y ) + z*z, stored to a float, then `32fe2370` sqrt minus the NPC size. `test ah,0x41 / jp` keeps `<` and `==`, and refuses a NaN. Match. The sort `322acc40` = `std::_Sort` (`std::sort`), then the units are copied out. Match.
- **Home:** the address range is the user class; `friend class CX2GUUser` in `CEffect` is under the flag. OK.
- **Caller spot-check:** Faena `@329c43f1-329c447d` pushes ( &vec, 1, SA 0x1f, -SA 0x9f, SA 0x9e, 1, 180.0 `[3344d398]`, 0 ), as the CH2b kickoff says. No kickoff change.

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_SKILL --worktree --files X2Lib/X2GUUser.cpp`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`. It shows 1 deleted line: CH2b0's own HP line, which was replaced inside the flag's block. `RESULT: PASS`. `enumscan --check X2Lib/X2GUUser.cpp`: 0 A/B rows.
**Deploy:** DEPLOY OK: X2_offline.exe is 26,107,904 B, dated 2026-10-27 17:02:57, md5 f7ef19b0d5186a975b3dcd44d159d5fe, equal to X2/US_SERVICE/x2.exe (2026-10-27 17:02:49); built at main 1dd00598 + 1 uncommitted code file(s).
User owes nothing (the collector stays inert until CH2b). Next: `/x2-phase char-eve CH2b` (Sonnet 5.5 high).

### CH2b - Done (2026-10-27, Sonnet 5.5)

Architecture's twelve skill ids take June's `329c8a00` cases and state bodies (flag `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL`, in `June_SetEquippedSkillFuncHead` and its Faena default). 22 June functions ( the 21 of the class map plus the CH0 `3299b810` helper ), read from the raw listings ( `charport_adis.py eve`; Ghidra has no function for 8 of them ).
- **Registered ids:** Surface Cuting 25015 / 25057, Sonic Wave 25013 / 25041, Oberon Guard 25014 / 25056, Heaven's Fist - Pressure 25012 / 25038, Sweeper 25016 / 25058, and Faena 25224 / 25225 ( `June_SetFaenaSkillFunc` now adds its StateInit `32968da0` and FrameMove `329c4380` ). Six `RNSI_*` names are registered under `SERV_IRUHADEV_JUNE_ENUM_NAMES` ( `X2SkillTree.h` ), three BFI names ( `BFI_BUFF_OBERON` 441, `BFI_BUFF_OBERON_GUARD` 652, `BFI_DEBUFF_TARGET` 842, `X2BuffFactor.h` ). Tables and callbacks are `ch0_skill.tsv`'s; Sonic Wave and Oberon Guard hyper tables take `June_CopyStateCallbacks`, Heaven's Fist's hyper table keeps Init and has the Hyper EventProcess.
- **March bodies that are June's as is ( 8 ):** Surface Cuting Init / EventProcess, Sonic Wave EventProcess, Oberon Guard Init / EventProcess / End, both Heaven's Fist EventProcess ( keys and times checked: `ZXC` 0.66 s, `ZXCAS` 3.33 s, hyper = animation end only ). New members ( 12 + the helper ) are in the funcmap rows `Char Eve CH2b`.
- **What differs from March:**
  - Surface Cuting: `June_SummonSurfaceCuting( key, pos )` ( `3299b810` ) sets BFI 441, rewrites the first Time finalizer of BTI 412 ( `+0x9c4c` x 60 + `+0x4070`, x `SA_DECREASE_REL` in PvP ) and asks NPC 1621 ( key code from the caller, `NAT_ALLY`, `NCT_CHANGESTAGE_REMAIN` ) behind `IsMyUnit`; FrameMove calls it at 0.01 s with key 1 ( CH3b's `329ae4f0` passes its own ).
  - Sonic Wave: one `EffectSet_SA_EAT_SonicWave` at 0.01 s, handle in `+0x9c34`; no Guardian NPC. Oberon Guard: Start also sets `+0x9770` ( power x summon rate ); FrameMove at 0.001 s clears the old guard, BFI 652 level 1, and `CreateNPCReq( 2091, 0, ..., delay 0.3, key -1 )` ( the `322e67c0` core; its 19 words checked ).
  - Heaven's Fist: `Effectset_SA_EAT_HeavensFist_Pressure` with a custom position ( `GetPos`, rotation, direction; scale `GetVec3ScaleByUnit` ) at 0.01 s, normal and hyper alike. Sweeper: `..._Swerper_Right` / `_left`, 300 to the side, rotation + ( 0, 180, 0 ) on the left, camera shake at 1.241 s kept; its EventProcess has the on-line walk / dash cancels.
  - Faena: at 0.271 s the cone collector ( CH2b0 ) with SA 31 / 158 / 159 ( defaults 700 / 35 / 35 ), each enemy gets a BFI 842 clone ( slot 476 adjust, my UID ) and `EffectSet_SI_A_EAT_Faena_Damage` ( custom position at the aim point for a boss ).
  - None of the five state bodies has a host test, a cut-in or a memo branch any more ( June's `GetEqippedSkillMemo` is a stub ).
- **JUNE_BUGS 451:** Faena's loop leaves the previous clone's address in the level register and the game pointer in the skill-id register, so the second and later targets get a far-out level ( values read 0 ) and no memo rescale. Ported verbatim ( level = the address as `UINT`, skill id -1 after a boss ). Row 450 ( the collector ignores its count ) is why that matters.
- **Assets:** every `CH2b` row of `ch0_effect_closure.tsv` is `june-bytes`; the three `missing` sounds ( `Eve_Faena.ogg`, `Eve_EM_Faena01` / `02` ) are `identical` to March's koms in `stage-june --dry-run` ( CH0b staged them ). No kom change, no pack.
- **Unmapped callees settled:** `322ceae0` = `CX2Game::GetEffectSet()` ( the pushes before it are `PlayEffectSet`'s; the first stack word is the hidden Handle return and the second the name ); vslot 0x24 = `GetVec3ScaleByUnit` ( out pointer ); vslot 0x3c8 = `GetIsBosRaidNPC`; vslot 0x54 = `GetUnitUID`; vslot 0x524 = `GetNowStateSkillID`; vslot 0x198 = `IsMyUnit`. New: `CX2GUUser::JuneGetUnitLockOnAimPos` ( `X2GUUser.*`, a wrapper so Eve can reach `CEffect::JuneGetLockOnAimPos` through the friend class ).
- **Branches not ported:** none found outside the plan: `3299b810`'s second caller `329ae4f0` is CH3b's.

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_SKILL --with JUNE_CHAR_EVE_SKILL_DEBUG --worktree --files <the 7 files>`: `files: 4 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR` ( the 2 OTHER are the `JUNE_ENUM_NAMES` registrations in `X2BuffFactor.h` / `X2SkillTree.h` ); `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `enumscan --check X2Lib/X2GuEve.cpp X2Lib/X2GUUser.cpp`: 0 A/B rows.
**Deploy:** DEPLOY OK: X2_offline.exe is 26,118,656 B, dated 2026-10-27 17:35:35, md5 4ad6a8e576820c6213d4f1c2202eeb87, equal to X2/US_SERVICE/x2.exe (2026-10-27 17:35:28); built at main bd5b08ef + 7 uncommitted code file(s). The strings `EVE SKILL FAENA target`, `Effectset_SA_EAT_HeavensFist_Pressure` and `EffectSet_SA_EAT_SonicWave` are in the deployed exe.

**Play-test hand-off.** Save: unit 19 `Iivue` ( Eve Code Empress, level 80 ) now has 25015 in slot 0, 25013 in 1, 25014 ( added, level 1 ) in 2, 25012 in 3, 25016 in 4 and 25224 in 5; the old slots ( 25213 / 25030 / 25031 / 25214 in 0-3 ) are cleared. The backup is `db_backup\els_db_20261027_172852_pre_sql.sql`. Log tag `[JUNE_CHAR] EVE SKILL`.

**Play-test 1 (2026-10-27 17:43): two defects, fixed the same day.**
- **Oberon and Oberon Guard never showed** ( the Eve animation played; the log had `GAME spawning 1 NPC` and no error ). The live `data037 NPCTemplet.lua` ( core-list, March-merged ) had no rows for NPC 1621 `NUI_SI_SA_AT_SURFACE_CUTING` and 2091 `NUI_ECA_OBERON_GUARD`, and `data036 Enum.lua` had no `NUI_SI_SA_AT_SURFACE_CUTING = 1621`: the client could not build the NPC. Fix: June's two rows appended verbatim to `NPCTemplet.lua`, the Enum line added, both refreshed in the ledger ( `ledger --check` PASS ), `data036` and `data037` packed ( `PACK OK`, `~ Enum.lua` / `~ NPCTemplet.lua` only ). **Needs `/x2-kom-packed copy data036` and `data037`, with the user's yes.** `NUI_SI_SA_EEP_ELECTRONIC_FIELD` ( 1622, Ophelia ) lacks both too: CH2c's.
- **Faena crashed** ( access violation in `vector<float>::push_back` under `BUFF_FACTOR( rhs, level )` from `GetClonePtr`, resolved from the crash bundle ): the second target's level was the previous clone's address ( JUNE_BUGS 451 ). The level is no longer clobbered; the skill-id register is, as before. JUNE_BUGS 451 now reads "partly kept".
- The `_DEBUG` Surface Cuting / Oberon Guard lines also print whether the Oberon / guard buff is on.
**Deploy ( fix ):** DEPLOY OK: X2_offline.exe is 26,119,168 B, dated 2026-10-27 17:52:09, md5 dac69b50ac758e77b87969f98f654ade, equal to X2/US_SERVICE/x2.exe ( 17:52:02 ); built at main e0f9580d + 1 uncommitted code file.
**Play-test 2 (user, 2026-10-27, run 19:25-19:26, md5 `7197b609`): Oberon's own attacks showed no slash; fixed, PASSED.** June's `OBERON_ATTACK_A/B/C_FRAME_MOVE` call `game:GetIsNowPVPCorrectionGame()` before `PlayEffectSet_LUA`; March never bound it (ErrorLog: "can't find 'GetIsNowPVPCorrectionGame' class variable"), so the function died before the effect set. Added `CX2Game::GetIsNowPVPCorrectionGame` ( June `322cc1a0`: `GetGameType()` is `GT_PVP` or `GT_PVE` ) and its Lua binding in `X2Main.cpp`, flag `_SKILL` ( also fixes Nasod Watch Eve and the Electronic Field NPC ). Log: `ES_CREATE` for `..._OBERON_Attack_A` x10, `_B` (1st + 3rd) x7, `_C` x7; 0 `PVPCorrection` errors. Hitstun: no change needed, every June Oberon state is `SUPER_ARMOR` / `INVINCIBLE` and `OBERON_DAMAGE` plays `Wait`. Several Oberons at once: handed to Phase CH2b2.
**Deploy ( slash fix ):** DEPLOY OK: X2_offline.exe is 26,120,192 B, dated 2026-10-27 19:24:34, md5 7197b609955583a7e56fdef86baa2cbd, equal to X2/US_SERVICE/x2.exe ( 19:24:27 ); built at main ca0efe1a + 4 uncommitted code file(s).

### CH2b - Review (2026-10-27, Opus 5.5): PASS after two transcription fixes (Faena takes one target; PvP or PvE)

All 22 listings were read (`charport_adis.py eve`), plus registration rows `ch0_skill.tsv` 446 / 463 / 472 / 485 / 497 against the five cases and Faena's two callbacks.
- **Fix 1, Faena `329c4380`: one target, not every target.** The only back edge (`@329c44c1`) skips NULL entries. Both paths after the first real unit (boss `@329c464c`, the other by falling through) reach `@329c4751`, release the factor and free the vector (`@329c4773`). Ghidra decompiles it as `if( unit ) { ...; break; }`. June marks the nearest enemy only (the collector sorts by distance; row 450's ignored count never shows). The `ebp` / `edi` reuse is ordinary register reuse after their one use. **JUNE_BUGS 451 is withdrawn.** The play-test-1 crash came from the port's own second pass, so the level and skill-id workarounds are gone (learned level, the skill id to slot 476).
- **Fix 2, `322cc1a0` is PvP or PvE.** Surface Cuting's time scale tested `GT_PVP` only ("March has no GT_PVE"), but `GT_PVE` has been registered since the CH1l5 review. Now `GetIsNowPVPCorrectionGame()`. The same stale test was fixed in four more Eve sites: CH2a3's Mega Electronball rate, the two skill-value helpers `June_GetSkillNumericalByName` and `JuneEveTryGetAcquiredSkillAbilityValue`, and CH1k's recovery tooltip (`_CORE`, inline `GT_PVP || GT_PVE`). Three non-Eve sites (Aisha x2, `ParsingBuffFactorID`) are routed to Stage 33 ("known mixed sites").
- **Confirmed as ported:**
  - **`3299b810`:** the BFI 441 call is `( 0x1b9, creator 0, level, 0, 0 )`; `max( 1, level )`; BTI 412's first finalizer time is `+0x9c4c` x 60.0 + `+0x4070`, and `SA_DECREASE_REL` takes the 4-word push. The 18 forwarder words are gate, 1621, hyper byte (`+0x7cbc` state data `+0x10` = `m_bHyperState`), 1, pos, right, 0.0, 1, key = arg 1, team `+0x278`, `NAT_ALLY`, UID, 0, `TN_NONE`, `NCT_CHANGESTAGE_REMAIN` (3). vslot 0x54 takes no argument.
  - **`329b2e10`:** BFI 652 is `( 0x28c, 0, 1, 0, 0 )`. The 19 words of `322e67c0` are 2091, 0, 1, pos, right, 0.3, 1, -1, team, 1, UID, 0, -1, `NCT_NONE`, gate, trailing 0.
  - **`329700a0`:** `+0x304..+0x310` = nowSpeed ( 0, 0 ) / passiveSpeed ( -1, -1 ); `+0x9770` = `GetPowerRate()` x `+0x4064`.
  - **Effect sets:** Heaven's Fist `3298d550` / `3298d710` (identical but for the EH handler) and Sweeper `329a99a0` call `PlayEffectSet( name, this, NULL, false, power x +0x4064, -1, GetVec3ScaleByUnit, true, pos, rot, dir, 1.0 )`. Sweeper puts `pos +- dir x 300` and rotation `+ ( 0, 180, 0 )` on the left. Its oneshot is `timer < 0.1`, else 1.241 s `UpDownCrashCamera( 80, 1 )`. Sonic Wave `3298c730` is the defaults + power, with the handle at `+0x9c34`.
  - **`32974bb0`:** animation end -> JUMP_DOWN / WAIT on `bFootOnLine`; walk / dash cancels carry the on-line test; the skill cancel then the awake-lock chain (`322731a0`, oneLCtr, `+0x8d4c`, `GetHyperModeStateID`).
  - **Faena's other arguments:** SA `0x1f` / `0x9e` / `0x9f` (700 / 35 / 35), cone `( 1, range, -0x9f, 0x9e, true, 180, false )`; boss vslot 0x3e4 with `L""` and the narrow name `334d3828`; the other path `( name, this, target, false, -1, -1, ( 1, 1, 1 ), false, 0, 0, 0, 1.0 )`.
- **Ghidra spot-checks (read-only):** `329c4380` decompile (the `break`, SA ids, collector arguments); strings `334d37e8` / `334d3828` = `EffectSet_SI_A_EAT_Faena_Damage`. Both pass.
- **Retired:** `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL_DEBUG` (define, four log blocks, the Oberon logger and its call / declaration).

**Hygiene.** `flagcheck.py JUNE_CHAR_EVE_SKILL --with JUNE_CHAR_EVE_SKILL_DEBUG --worktree --files X2GuEve.cpp X2GuEve.h Always.h`: `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`. The OTHER rows are the CH1k tooltip line under `_CORE` and a peer's uncommitted CH2b2 block in `Always.h`. `in this flag's files: 3 deleted line(s)` = the retired header declaration; 0 non-ASCII, 0 EOL-mismatched, 0 encoding changes; `RESULT: PASS`. `enumscan --check X2Lib/X2GuEve.cpp`: 0 A/B rows.
**Deploy:** DEPLOY OK: X2_offline.exe is 26,119,680 B, dated 2026-10-27 20:01:15, md5 1593f54c7697bc2b3818c6c525e041c6, equal to X2/US_SERVICE/x2.exe (2026-10-27 20:01:08); built at main 57a982fd + 8 uncommitted code file(s) (3 mine, 5 a peer's CH2b2 work in progress).
**Play-test (user, 2026-10-27, run 20:07-20:08, md5 `1593f54c`): PASSED.** Faena cast into a group marked one enemy. The logs show 0 EXCEPTION, a clean exit (census written) and no new crash bundle. The debug lines are retired, so the one-target result is the user's visual check. (This section was first committed as a draft by the peer commit `57a982fd`.)

## Phase CH2b2 - How June spawns a summon (added 2026-10-27, Opus)

Raised by the CH2b play-test 2: repeated Surface Cuting casts leave up to 4 Oberons alive; the user expects one summon per owner and NPC type and asks whether
that is a rule of June's NPC spawn path rather than of the skill. **Run with:** `/x2-phase char-eve CH2b2` (Opus 5.5 high). **Read:** `kickoffs/char-eve/CH2b2.md`.
Scope: June's create-NPC chain (`3299b810` -> `31f8b010` -> `322e67c0`), the receive side and March's offline stand-in, NPC-side caps / replace keys, and a census of
every June user-side `CreateNPCReq` caller. **Done gate:** the rule (or its absence) proven with June addresses; a flagged port or stand-in with a play-test hand-off, or a
recorded "June allows many". Related: JUNE_BUGS 449 / 451, `June_ClearGuardAndSummon`, `June_SetSelfDestructSummonedNPC`.

### CH2b2 - Done (2026-10-27, Opus 5.5)

**Verdict: June's client has the rule, on the receive side of every summon, not in the skill.** A new summon of one of 28 NPC ids sets HP 0 on the owner's older live summons of the same id. Ported for all 28 under `SERV_IRUHADEV_JUNE_SUMMON_UNIQUE`.
- **Q1, create path: no rule.** `3299b810` -> `31f8b010` (the `IsMyUnit` gate, then the 19 words) -> `322e67c0` only fills a `KNPCUnitReq` and queues it in the game's `+0x448` delay map ( host / game-end checks only ). No NPC loop, cap or kill anywhere in the chain.
- **Q2, receive side: the rule.** June's `CX2Game::CreateNPC` `3230d2b0` calls `SetUserSummonedNPCInfo` `3230c1a0` @`3230d9b2` (create type 0 / 3), and only then `AddUnit` (game vslot 0xd4 @`3230dc69`). The common tail of `3230c1a0` (every case, incl. Oberon 0x655 / 0x656 with flags 0x240) calls `3200bec0( unitID )` @`3230cb03` on the owner: when `31fc96b0( id )` is non-zero it walks the unit list and calls `SetNowHp( 0 )` (vslot 0xf4) on each NPC of that templet id with HP > 0, `m_bSelfDestruction` (`+0x8b40`) false, `NAT_ALLY` and the same owner UID. The new NPC is not in the list yet, so only older ones die. March, the offline emulator (`Handlers_Room.cpp:2899`) and the server source (`DungeonRoom.cpp:5803`, `KRoomUser::AddAllyNPC`) have no such rule: it was always client-side.
- **Q3, NPC side: the map.** `31fc96b0` is "summon NPC -> the buff factor it gives its owner" (Oberon 1621 -> BFI 441). 28 ids, 26 BFIs ( `june2017_re/char_eve/ch2b2_summon_buff_map.tsv`, unicorn over ids 0-70000 ). Oberon's own Lua has no replace logic ( `SUMMON_END_STATE` / `SELF_DESTRUCTION_STATE` only ). `June_ClearGuardAndSummon` (Oberon Guard) is the only caller-side cleanup and stays.
- **Q4, census.** 72 create-call sites (18 on `31f8b010`, 53 on `322e67c0` + the forwarder's own call, 2 Lua-only siblings `322e6aa0` / `322e6d40`): `june2017_re/char_eve/ch2b2_createnpc_census.tsv`. 27 sites create a mapped id; every mapped id but 1937 (`SUMMON_DARKNESS_PVP`, created through Lua) has a constant create site. So the rule is bound to how the NPC spawns, for a fixed set of single-instance summons:

| Class | Mapped summons (id) |
|---|---|
| Eve | Nasod Watch 384 / 519, Atomic Shield 682, Ferdinand 1454, Apocalypse 1604, Oberon 1621, Ophelia 1622, Oberon Guard 2091, Void Impact memo 5188, Stardust Shower memo 5187 |
| Chung | targeting 863 / 1419 |
| Rena | Gungnir targeting 2083 |
| `LLD` / `LCN` ids (callers in `32b2xxxx`) | Summon Darkness 1906 / 1937, Shadow Bolt 1908 |
| Rose | Land Runner 2175 / 2194, Proton Bomb 2192, Gale Force 2206, All Bomb 2207, EZ-8 2209, Sparrow 2211, Magnetic Force 2213, Mecavolt 2214, Photon Shower 2397, Claymore 2398, Armored Tempest 2526 |

- **Port.** `CX2GUUser::JuneGetSummonBuffFactorID` (`31fc96b0`, whole) and `CX2GUUser::JuneKillOlderSummonsOfSameID` (`3200bec0`) in `X2GUUser.*`; one call in March's `SetUserSummonedNPCInfo` tail after `CopyJuneMaximizeAndRangeFrom` (June's order, before `JuneSummonedNPCInfo`). 19 `NUI_*` and 23 `BFI_*` June names registered under `SERV_IRUHADEV_JUNE_ENUM_NAMES` (`X2UnitManager.h`, `X2BuffFactor.h`). Log line `[JUNE_SUMMON] npc <id> uid <uid> of owner <uid> replaced by a new summon ( HP 0 )`. No JUNE_BUGS row: nothing June does here looks wrong.
- **Branches found, not ported (scope rule):** (1) `3230c1a0` @`3230cafb` `3204e160` = Eve's Open Mind (25417) stacking BFI 877 per summon -> **CH4b** (its "7am NPC-side push, skill 25417"). (2) `327b53b0` @`327bcdfd` = a dying summon drops the owner's summon buff (BFI's templet) unless `32778b10` finds another live one; Ferdinand / Apocalypse take `31fc8a90` -> **new Phase CH2b4** (Eve's summons are its main users; the Oberon buff then ends with the last Oberon, as in June).

**Hygiene.** `flagcheck.py JUNE_SUMMON_UNIQUE --worktree --files <the 6 files>`: `files: 3 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR` (the 2 OTHER are the `JUNE_ENUM_NAMES` registrations); `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `enumscan --check X2Lib/X2GUUser.cpp X2Lib/X2Game.cpp`: 0 A/B rows.
**Deploy:** DEPLOY OK: X2_offline.exe is 26,119,680 B, dated 2026-10-27 20:01:15, md5 1593f54c7697bc2b3818c6c525e041c6, equal to X2/US_SERVICE/x2.exe (2026-10-27 20:01:08); built at main 57a982fd + 8 uncommitted code file(s). The first build failed in this phase's own edit (the two members sat in a `protected:` block, `C2248` at the `X2Game.cpp` call); they are now `public:`. The wide string `[JUNE_SUMMON]` is in the deployed exe.

**Play-test hand-off.** Unit 19 `Iivue`, Surface Cuting in slot 0: cast it 3 times in a row near enemies. Expect one Oberon at a time: each new one makes the old one die ( `[JUNE_SUMMON] npc 1621 ... replaced` per recast, and the `[JUNE_CHAR] EVE SKILL OBERON` debug lines show one uid at a time ).

### CH2b2 - Review (2026-10-27, Opus 5.5)

**Verdict: PASS, no code change, no build** (deployed exe stays md5 `1593f54c`; play-test PASSED 20:07, four replacements of npc 1621).
- **`3200bec0` gate by gate:** `31fc96b0 != 0`, then the `g_pX2Game` handle checks (`3372c0fc`) = `NULL == g_pX2Game`. Game vslots read from the CX2Game vtable `3348bf94`: 0xf8 = `3231f540` (size of `+0x304`), 0xb0 = `322ccb40` (bounds-checked `+0x304[i]`) = March `GetUnitNum` / `GetUnit` over `m_UnitList` (not the NPC list 0xf4 / 0xb8 at `+0x324`). `+0x264 == 1` = `GUT_NPC` (probe: 1). Templet `+0x85f8[0] == id` only when `id != 0` (unreachable at 0: `31fc96b0(0)` = 0). HP: gage `+0xc04` null -> no test (March `GetNowHp` returns 1); HP gage `+0x20` null -> skip (March returns 0); `fcomp` + `test ah,1` keeps NaN = `GetNowHp() <= 0.f`. `+0x8b40` = `m_bSelfDestruction` (June `3299d140` tests it @`3299d2c0` / `@3299d328` before `StartSelfDestruction` `31e4d560`). AI `+0x7c98`, type `+0x38 == 1` = `NAT_ALLY` (probe: 1), owner `+0x78/+0x7c` vs user vslot 0x54 = `GetAllyUnitUID()` / `GetUnitUID()` (funcmap 16-step3b4). Vslot 0xf4 = `SetNowHp( 0 )`.
- **`31fc96b0`:** all 28 cases read: 18 from the compare tree, 10 from the jump table (`31fc986c` byte index, `31fc9840` targets; ids 2091 / 2175 / 2192 / 2194 / 2206 / 2207 / 2209 / 2211 / 2213 / 2214). Every id -> BFI equals the port and `ch2b2_summon_buff_map.tsv`. All 28 `NUI_*` and 26 `BFI_*` values equal `Enum.src.lua`; the four implicit March members (`NUI_NASOD_WATCH_EVE` 384, `_MEMO` 519, `NUI_ATOMIC_SHIELD_EVE` 682, `NUI_TARGETING_CHUNG` 863) by `enumprobe.py`.
- **Call site:** March calls `SetUserSummonedNPCInfo` for `NCT_NONE` (0) or `NCT_CHANGESTAGE_REMAIN` (3) = June's gate @`3230d983..3230d98a`, and `AddUnit` follows at `X2Game.cpp:8035`, so the new NPC is not yet in the list. June also skips the call for `0x10c1` = `NUI_EVENT_RAMIEL` (4289, not in the map, no C++ member): it never reaches `3200bec0`, so it does not matter for this rule; the stat skip is an event-NPC detail, noted and left (no Ramiel spawn offline known).
- **Interactions:** `June_ClearGuardAndSummon` / `June_SetSelfDestructSummonedNPC` mark old summons self-destructing or HP 0, which `3200bec0` skips, as in June. The Proton Bomb event case is `0x10f7`, a different id from `NUI_PROTON_BOMB` 2192: no overlap. Chung's `SetSelfDestructSummonedNPC` (targeting 863 / 1419) is the same skip. Log `%d` for `CX2GUNPC::GetUID()` is right (it returns `int`).
- **Hygiene:** `flagcheck.py JUNE_SUMMON_UNIQUE --range ac295991^..ac295991`: `files: 3 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The 2 OTHER are `X2BuffFactor.h` / `X2UnitManager.h`, the `JUNE_ENUM_NAMES` registrations. `git log -SJUNE_SUMMON_UNIQUE ac295991..HEAD`: none.
- `kickoffs/char-eve/CH2b4.md` checked: stands as written (its `+0x8b40` premise is confirmed above). User owes nothing.

## Phase CH2b3 - Three unbound NPC Lua methods (added 2026-10-27, Opus)

Found while asking why effects keep going missing (play-test 2): `tools/luabind.py check` shows Oberon's `OBERON_START_STATE_END` calls `ApplyPassiveSkillOwnerUser_LUA`
(June `32763c10`), `ResetMultiplyPowerRate_LUA` (`31e8d6a0`) and `SetMultiplyPowerRate_LUA` (`31e8d610`), none bound in March, so the function stops at the first call and
`SetFlag_LUA( 0, true )` never runs. The two power-rate writers are small (protected float at NPC `+0x8ce4`, read by `GetPowerRate` `32759630`); the Apply call is the
owner-passive / stat refresh of `SetUserSummonedNPCInfo` and belongs with the Stage 33 stat-calc rules. **Run with:** `/x2-phase char-eve CH2b3` (Opus 5.5 high).
**Read:** `kickoffs/char-eve/CH2b3.md`. **Done gate:** the three rows added to `tools/data/lua_unbound_known.tsv` on 2026-10-27 are deleted, `luabind.py check` PASS on the three Eve summon scripts, funcmap rows, no "can't find" in ErrorLog.

### CH2b3 - Done (2026-10-27, Opus 5.5)

Flag `SERV_IRUHADEV_JUNE_NPC_LUA_SUMMON_RATE` (new; the factor is shared by every June summon script, Rose's too). Log `[JUNE_CHAR] NPC POWER`. funcmap rows tagged `(CH2b3)`.
- **The factor.** `CX2GUNPC::m_fJuneMultiplyPowerRate` = June's protected `+0x8ce4` (ported plain, 1.0 at the ctor `327b0263`). `GetPowerRate` is June's `32759630`: `m_fDefaultPowerRate` x the factor (March: the default alone).
- **Bindings** (`X2Main.cpp`, beside `GetNumber_LUA`):
  - `SetMultiplyPowerRate_LUA` `31e8d610`: writes the argument when it differs;
  - `ResetMultiplyPowerRate_LUA` `31e8d6a0`: writes 1.0 when it differs;
  - `ApplyPassiveSkillOwnerUser_LUA` `32763c10`, read in full:
    - no owner (`GetOwnerGameUnit`, June's observer `+0x8d5c`): return;
    - `GetNPCStat` (`32758d50`, = March);
    - attack x owner `+0x4068`, then / owner `+0x4064` (`m_fStatAtkRateOfSummoned`) when it is > 0;
    - critical += owner `+0x406c`, capped at 1.0;
    - `SetNPCStat` (`32763950`, = March);
    - the three owner copies @`32763d4f` = `CopyJuneMaximizeAndRangeFrom` (the `3230ca6d` copy).
  - The whole call tree is ported or is already March's. Stat-calc check: no March / June mix inside it. Only its two new inputs have no writer yet (below).
- **Owner fields.** `SkillRelatedData::m_fJune4068` (1.0) and `m_fJune406c` (0.0), June's `CX2GameUnit` ctor values (`322a434b` / `322a4357`).
  - Their only writer is Eve's `InitPassiveSkillState` `3297f290`: Grace of Queen 25408, `SA_SUMMON_ATK_REL` and `SA_ATK_CRITICAL_RATE_ABS`.
  - That function is CH4a's whole-body port, so the writes go there (CH4a kickoff amended). Until then Apply gives an Eve without Grace of Queen exactly June's numbers.
  - Same amendment: March's `m_fStatAtkRateOfSummoned` writer still reads `SI_P_EN_QUEENS_CONTROL` (5401); June reads 25406.
- **Writers of the factor (stores moved).** June's summon helper `322ece70` puts the power product in the factor when flag 0x200 is set (@`322ed38c`), and leaves `+0x8ce0` at 1.0. March's port had folded it into `m_fDefaultPowerRate`, which was the same product only while nothing reset the factor. Now:
  - Nasod Watch (0x240): `JuneApplySummonPowerRateToFactor`;
  - Ferdinand (0x260): `SetMultiplyPowerRate_LUA( fPowerRate )`, March's product terms unchanged;
  - both keep the old store in `#else`.
  - Oberon 1621, Ophelia 1622 and Apocalypse 1604 still use March's default case. Their June cases (0x240 / 0x220) are CH3d's (checklist L274 / L281; CH3d kickoff amended).
- **What changes in game.**
  - `OBERON_START_STATE_END` (and Ophelia's, Nasod Watch's start-attack end) runs to `SetFlag_LUA( 0, true )`.
  - Nasod Watch's and Ferdinand's start power drops to 1.0 after the start state.
  - Their dying-land / start blast uses `GetNumber_LUA` (skill power + 1) as the factor, as June does.
- **luabind.** The three known-list rows are deleted. `luabind.py check SI_SA_AT_SURFACE_CUTING Nasod_Watch_Eve SI_SA_EEP_ELECTRONIC_FIELD SI_HA_FERDINAND SI_HA_APOCALYPSE` PASS.
  - Ferdinand / Apocalypse also call four other unbound names (`GetSummonAllyNpcForceTargetUnit`, `ResponseEmergency`, `GetEffectSetInstance`, `SetLocalRotateDegree`). They are now known-list rows owned by CH3d.
- **Hygiene.** flagcheck worktree: 4 PASS, 1 PASS~ (the blank line before the new `X2GUNPC.cpp` block), 1 DEFINE, 0 OTHER, 0 deleted, 0 encoding change, RESULT: PASS. enumscan 0 A/B.
- **Deploy.** `DEPLOY OK: X2_offline.exe is 26,129,920 B, dated 2026-10-27 20:45:51, md5 1ec950e484b2f1b2497c0aaf1f1979bb` (built with CH2c's uncommitted edits on disk; the three binding names and `[JUNE_CHAR] NPC POWER` x3 are in the exe). Play-test pending.
- **Not ported, routed.** `+0x4074` (ctor 1.0; read by `322ece70` flag 0x10, NPC 2206 Gale Force, and by `32abe5c0` / `32abefb0`, written by `32ad1080`) is Rose's (Stage 8).

## Phase CH2b4 - A dying summon drops its owner's summon buff (added 2026-10-27 by CH2b2, Sonnet)

Found by CH2b2: June's `CX2GUNPC::CommonFrameMove` `327b53b0` @`327bcdfd..327bcfe0` (the DYING_END block, before the animation ends): an owned summon not yet
self-destructing either runs `31fc8a90( owner unit class )` (Ferdinand 0x5ae / Apocalypse 0x644) or, for the other ids of `31fc96b0`
(`CX2GUUser::JuneGetSummonBuffFactorID`, ported by CH2b2), erases the owner's buff templet of that BFI ( `3229e160` ) unless `32778b10` finds another live summon of the
same id and owner; both set `m_bSelfDestruction`. March keeps the Oberon buff after the last Oberon dies. **Run with:** `/x2-phase char-eve CH2b4` (Sonnet 5.5 high, Opus review).
**Read:** `kickoffs/char-eve/CH2b4.md`. **Done gate:** the block ported whole beside March's DYING_END code (March's Ferdinand reset compared), funcmap rows, a play-test where the buff icon goes with the last Oberon.

## Phase CH2 - Skills: Nasod and the first jobs (classes 5, 18, 19, 26)

**Run with:** Sonnet 5.5 high (transcription from CH0's map), Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL`.
Sub-tags `SKILL`, `NUM`. A skill's line decides its phase: class 5 has it -> CH2a; else the first of 18 / 19 / 26.

| Sub-phase | Scope |
|---|---|
| CH2a | Nasod (5) skills |
| CH2a2 | Exotic Gear (18) skills (split by CH0) |
| CH2b | Architecture (19) skills |
| CH2c | Electra (26) skills except Sweep Rolling and Giga Stream |
| CH2c2 | Sweep Rolling and Giga Stream (split by CH0). Electronic Move (the 3 numerical-only tables) is Battle Seraph only: CH3c (CH0 correction) |

For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole; its attached rows (7c alpha tags, 7u
names, 7ai BFI arguments, 7b blocks if CH0 puts them here); its effect closure (a name found only now is staged and added to `ch0_assets.tsv`).
`NUM use` lines for Eve skills; March vs June damage in `MODS.md`.

**Done gate:** every class-5 / 18 / 19 / 26 id in the S3 census has a state (except CH0's passive / CH4 ids); each skill in an A and a B slot
casts June's state, motion and effect and deals damage; no missing particle / mesh / texture line.

## Phase CH3 - Skills: second jobs (108, 109, 116), hyper actives, transcendence

**Run with:** as CH2. Sub-tag `SKILL`.

| Sub-phase | Scope |
|---|---|
| CH3a | Code: Nemesis (108) skills |
| CH3b | Code: Empress (109) skills |
| CH3c | Code: Battle Seraph (116) skills |
| CH3d | Hyper actives (type 13, 6 rows), the 3 `SHOW_HYPER_CUT_IN_TIME` states (7t), and the **transcendence rows** (template, Stage 6 close-out): Eve's classes on T2's allow-list (`JuneTranscendCheck`, `Handlers_Quest.cpp`), her `LevelUpEvent.lua` TYPE row + `DLG_Transcendental_Change_Guide_Eve.lua`; checklist 7ao whole (hyper effect start `32994710` vslot 19, the four base helpers `31fcac90` / `31fcad70` / `31fceab0` `== 1` / `31ff1e80`); her hyper-class skills (`SPRT_HYPER_CLASS`) and `TRANSCENDENTAL_SKILL_DATA` rows; `JuneGetHyperModeTableName` (`31fdd490`) and `HQ_Cutin_*_Transcendence` names; `list_TR_*` rows in `GetCharacterImageName` (T6c) and her transcended pictures |

**Done gate:** as CH2 for 108 / 109 / 116 and the hyper actives; a transcended Eve (`settranscend`) casts her hyper-class skills, slots A-5 /
B-5 work, the transcended picture and cut-in show.

## Phase CH4 - Passives and class mechanics

**Run with:** Opus 5.5 high, Opus review, user play-test (each a different engine piece: planned as three from the start).

| Sub-phase | Scope |
|---|---|
| CH4a | `InitPassiveSkillState` and Eve's passive ids (types 1 / 5 / 8: 3 + 14 + 27 rows), their objects and readers; flag `_PASSIVE` |
| CH4b | `SetDamageExceptionProcess` `329bc560` whole (June-only; 7b, e.g. 25419 Open Mind), the other overrides CH0 finds (`GetActualMPConsume` / `GetActualCoolTime` style), 7an reader `329b407b`, 7am's NPC-side push (owner `UT_EVE`, skill 25417), 7ae's rule if Eve reaches it |
| CH4c | El Crystal and Code Recovery: the recovery mechanic behind the 10 June-only `CODE_RECOVERY_*` keys (read by `329c47e0` into members CH1k / CH1l transcribe), 7aj equipped-slot icon override (class type `0x1a`, tree `+0x134`, March `UpdateEveElectraSkillSlotIcon` -> June's tail), 7ah statics `Static_EveRecoveryGuide` / `_Cooltime` / `Button_EveRecoveryToolTip`, 7n sync float, 7a modifier |

**Done gate:** each passive's effect shows (CH0 names the stat or log line per passive); El Crystal icons and the recovery guide show as June;
0 Eve `no_state` rows left except passives with no state in June.

## Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces

**Run with:** Sonnet 5.5 high, Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_EVE_UI`. Sub-tag `UI`.

**Scope:** Eve's 7 `UnitTemplet.lua` rows become June's (BOM source, `luasrc.py refresh` after; every other row stays); the cut-in table
`X2GuEve.cpp:48-85` replaced by June's names and layout (Electra's missing `InitState` case is June's or March's: CH0 says); the dialog /
tree-UI pieces CH4 does not take. **Done gate:** a select-screen pass of all nine March characters with no new error; Eve's skill and hyper
cut-ins play June's `.gfx`.

## Phase CH-N - not run for Eve

All seven June Eve classes exist in March. The template block is `JUNE_CHARACTER_AISHA_PHASES.md` "Phase CH-N"; Ara runs it first.

## Phase CH6 - Whole-character walk and gap reruns

**Run with:** Sonnet 5.5 medium, then the user's walk. Back up the save; one Eve per class (classes 18 / 19 / 108 / 116 need new units:
`/x2-create-char` or `elsdb.py setchar`), one transcended. Walk: every skill in an A and a B slot, a hyper, the Nasod Core in awakening,
El Crystal, one dungeon per job. Re-run `census.py`, `charport_scan.py`, `charport_keys.py Eve_Nasod` and CH0's `s3_no_state.py`; before /
after numbers in the Done note. A defect goes back to its phase's flag.

**Done gate:** the walk passes; 0 unexplained `no_state` rows; no new `KEM_ERROR`.

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium. `MODS.md` "June character: Eve" section (flags, files, data, revert order: exe + data036 + motion koms
together); no `_DEBUG` flag left defined; checklist rows marked `(Eve: CHn)`; the Aisha plan's "Template for Stage 7" gets an Eve row (what
she took, what changed in the recipe); backlog Stage 7 note; the `stage7-plan` row points at the next character (`char-chung`).

## Play-test hand-off (every phase)

Dev console first; copy-ready lines with real values (unit, class, skill id and slot, `/additem`, `/qdone`); per check: where / paste /
what it does / look for / if not. Log tag `[JUNE_CHAR]`, `python tools/logs.py --grep "JUNE_CHAR EVE <sub>"`.

## Open decisions (for the user; asked at the end of planning; answered 2026-10-25)

1. **The Nasod Core (`ADD_EVE_SYSTEM_2014`).** June 2017 ships it; March has the 2014 code dormant (about 175 sites, Eve's class, `X2GUUser`,
   the gauge UI, the NPC side). Options: **(a, recommended)** use March's dormant code as the base: CH1q defines the two March macros under
   `SERV_IRUHADEV_JUNE_CHAR_EVE_NCORE`, then replaces every function that differs from June with June's whole body (no partial updates) and
   drops what June removed; **(b)** transcribe June's Nasod Core fresh beside March's dead code, leaving both March macros off. (a) reuses
   members, enums and UI March already wired; (b) avoids reading March's 2014 code at all but rewrites the same members.
   **Answered (user, 2026-10-25): (a), March's dormant code as the base**, June's whole body wherever it differs.
