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
  `24965799\kom_backup_stage_june61026_030148`.
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
| CH1m | Opus high, Opus review | Registration: June's `InitState` / `InitStateCommon`, the hyper-mode table name, START / WIN / LOSE roll, base event states, the `_LIVE` gate |
| CH1n | Opus high, Opus review | The CX2GUUser base callbacks Eve registers; `SetEquippedSkillFuncBySkillSlotIndex` `329c8a00` head (bounds, slot tables, the 7i rule) + the dev free-skill gate |
| CH1o | Opus high, Opus review | The virtuals under `_LIVE`; gaps the CH1a-CH1l reviews routed |
| CH1p | Opus high, Opus review | 7ag pressed-index space (Mega Electronball, Taser Pilum, Kugelblitz encoders + every reader + the decode `:6151-6177`, together); hooks 7h / 7l / 7o / 7p / 7x; June-only script keys |
| CH1q | Opus high, Opus review | Nasod Core (decision 1 = (a): March's `ADD_EVE_SYSTEM_2014` code under our flag, June's bodies where they differ; `ch0_nasod_core.tsv`), flag `SERV_IRUHADEV_JUNE_CHAR_EVE_NCORE`, sub-tag `NCORE` |
| CH1 | Opus high, Opus review, user play-test | Define `_LIVE`; adopt `Eve_Nasod.lua` and the summon scripts CH0 marks CH1; pack, ask, copy; the play-test |

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
