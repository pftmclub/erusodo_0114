# June character port: Aisha (Stage 5 pilot, and the template for Stage 7)

Plan key `pilot-char-revamp` · branch `main` · kickoffs `kickoffs/pilot-char-revamp/` · log tag `[JUNE_CHAR]` (lines start
`[JUNE_CHAR] ARME <sub>`, sub-tags `DATA`, `CORE`, `SKILL`, `MEMO`, `UI`, `NUM`) · flags `SERV_IRUHADEV_JUNE_CHAR_ARME_*` for Aisha's
class code, `SERV_IRUHADEV_JUNE_CHAR_*` for class-agnostic pieces the pilot is first to need (+ `SERV_IRUHADEV_JUNE_CHAR_DEBUG`) ·
backlog Stage 5 (`JUNE_PORT_BACKLOG_ORDER.md`) · written 2026-10-20 by the `new-plan pilot-char-revamp` session.

## Context

**What this plan does.** It moves Aisha (unit type 2, script prefix `Arme`, class `CX2GUArme_VioletMage`, seven classes: Violet Mage 2,
High Magician 14, Dark Magician 15, Battle Magician 23, Elemental Master 104, Void Princess 105, Dimension Witch 113) from her March kit to her
June 2017 kit, end to end. That covers her June script, her skill states for June's skill tree, her numerical damage, her motions, her
class-only engine hooks (`JUNE2017_CHARACTER_PORT_CHECKLIST.md` §1-§7ak), her UI pieces and her data. Then it writes down the recipe so
Stage 7 repeats it for the other eight March characters.

**Why Aisha (user, 2026-10-20).** The planning session reran `june2017_re/census.py` (all PASS, 0 `to-implement`) and `charport_scan.py`.
It ranked the characters by June attacks whose damage comes only from June's numerical skill data (`no DAMAGE table`). The counts:
- Aisha 1, Eve 3, Rena 12, Ara 14, Chung 15, Elsword 20, Elesis 20, Raven 25.
- Add's March kit is a skeleton (checklist l.237), so its 0 is not a small gap.

Aisha also scores well on the other measures:
- Stage 2's model diff (`JUNE_ENGINE_REMAINDER_PHASES.md` M3) finds no new motion file for her, and `r12b_anim_loss.tsv` finds no renamed animation.
- Every Aisha class already has `StatTable.lua` rows 1-99 (Atlas A2).
- She carries no enum-table truncation: `CUSTOM_RENDER_STATE` is an Add / Ara problem.

The user chose her over Ara, the smallest character that would exercise every template step.

**What Aisha does not exercise: a June-only later class path.** June's `Enum.lua` names `UC_ARME_AETHER_SAGE` 153, `_OZ_SORCERER` 154 and
`_METAMORPHY` 155, but June's own `UnitTemplet.lua` has no row for them (it has rows for only `UC_150` in the 150-186 block). In June 2017 they
are enum placeholders, not a playable path. So this plan's class-path phase (CH-N, below) is written as a template block and marked
**not run for Aisha**. Its first real run is the first Stage 7 character that has one: Ara (34 Little Specter, 124 Asura), Elesis (35, 125)
or Add (33, 36, 123, 126).

**The state she starts from.**
- **Skill system (Stage 3, closed 2026-10-19).** Every save is on June's skill trees (schema v17-v19), and June's `NewSkillTempletVer2.lua` ships.
  But Aisha's C++ has a state for almost none of June's reform skill IDs.
  - `june2017_re/s3_reform_ids_no_state.tsv` lists her rows. Pressing an unported skill falls to the class switch's `default:` (Power Attack).
  - A June-only type (14-18) is refused with `[JUNE_SKILL] MIG no-state skill`.
  - The numerical-data engine is live (`SERV_IRUHADEV_JUNE_SKILL_NUMERICAL`); a port that adds the states reaches it.
- **Effect engine (June's, complete 2026-10-16).** June's `DamageEffect.lua` / `EffectSet.lua` ship merged; 30 March-only class effect names
  were lost (checklist 7u lists Aisha's).
- **Motions.** Aisha's `Motion_Arme*` files are still March's (checklist 7ad); June's go in with her script.
- **Hooks waiting on the class port.** The engine side of checklist §7-§7ak is ported and waits on per-class calls (bounding states 7l,
  wait-state change 7h, pressed-index space 7ag, alpha tags 7c, BFI call arguments 7ai, tree-UI pieces 7aj, decide-page ACK 7ak).

**Out of scope (owned elsewhere).**
- Transcendence: Stage 6, built on Aisha once this plan closes. Her TC slot, `_Trans` cut-ins, `SDT_*_TRANSCEND*` and `CX2Unit +0x3c` wait for it.
- The new characters: Stage 8.
- UI that is not Aisha's own: Stage 9.
- Other characters' rows in the checklist: Stage 7.

## Rules

- **Standing rules:** `JUNE_PORT_BACKLOG_ORDER.md`, "Standing rules (every stage, every kickoff)". In particular:
  - the scope rule: a branch found here that is not Aisha's, and not needed to play her, gets an owner and a brief, never a phase here;
  - no partial June updates (a ported function's whole body);
  - June wins, even when it removes a feature;
  - June bugs are kept verbatim with a `JUNE_BUGS.md` row;
  - cite June addresses, never raw offsets.
- **The checklist is the work list.** `JUNE2017_CHARACTER_PORT_CHECKLIST.md` is read by section, never in full.
  - Every Aisha row in §1-§7ak is either ported by a phase here, or marked "n/a for Aisha" with the reason in CH0's map.
  - When a phase ports a row, it appends `(Aisha: <phase>, <commit>)` to that row in the checklist. The checklist then shows Stage 7 what one
    class port actually took.
- **One deploy unit.** June's `Arme_VioletMage.lua`, June's `Motion_Arme*` files and the C++ that reads them go live together, in one phase
  (CH1).
  - A June script on March C++ (or the reverse) loads states the other side does not have.
  - Between phases, Aisha stays playable: unported skills keep falling to `default:`, as they do today.
- **Lua.** The class script arrives through `luasrc.py adopt-june` (June's `ljconv` bytecode). It becomes core-list BOM source only if a phase
  must edit it (`luasrc.py core --add ... --why "edit: pilot-char-revamp CHn <what>"`, no question; then `adopt-june --source`).
  - Never copy from `june_src_fixed/`; that is reading material.
  - `Enum.lua` is core source: edit it as text and run `luasrc.py refresh`.
- **Assets.** June's same-name files replace March's, byte-exact, through `kom.py stage-june` (motions, meshes, textures, `.gfx` cut-ins).
  Pack them yourself; ask before every `/x2-kom-packed copy`.
- **CP949.** `X2GUArme_VioletMage.*` and most `X2Lib` sources are CP949: edit them with `bytepatch.py` only.
- **Damage numbers.** June's numerical scale differs from March's (checklist §1). Record both values in `MODS.md` for every Aisha skill whose
  damage changes.
- **Saves.** Aisha's skills are already on June's tree, so no schema change is expected. If a phase needs one, it snapshots `db_backup/` first,
  and the rung is forward-only.
- **One code phase at a time** in `X2GUArme_VioletMage.*` (CH1-CH4). CH5 (UI files) may run beside CH3 / CH4.

## Starting context

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "<heading>"` |
| Checklist rows | `python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "<heading>"` (CH0's map names the rows per phase) |
| Pilot evidence | `june2017_re/char_arme/` (CH0 writes it), `s3_reform_ids_no_state.tsv`, `c1a_numerical_blocks.tsv`, `c10a_alpha_sites.tsv`, `c11a_march_only_templets.tsv`, `g4_cutin_names.tsv`, `funcmap.tsv` |
| June Lua (reading) | `june_src_fixed/data036_kom/Arme_VioletMage.src.lua`, `UnitTemplet.src.lua`, `Enum.src.lua` |
| Ghidra | `ghidra` skill; June class range `0x32cc0000-0x32cfffff` (Aisha); read-only in CH0 and reviews |
| Gap scan | `cd june2017_re && python census.py && python charport_scan.py <scratch dir>` (slow: about 5 min + decompile; cache the scratch dir) |
| Bring June Lua in | `/x2-adopt-june` (`luasrc.py adopt-june data036 Arme_VioletMage.lua`, `--dry-run` first) |
| Stage June assets | `python tools/kom.py stage-june <list>`, then `/elstools pack dataNNN`, then `/x2-kom-packed copy dataNNN` (ask first) |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply` (x2-byte-edit) |
| Hygiene / build / logs | `flagcheck.py <FLAG> --worktree --files ...`, `build.py --deploy` (background), `logs.py --tag JUNE_CHAR` |
| Save / test character | `/x2-db` (`tools/elsdb.py setchar` for class + level), `/x2-create-char <name> <class> <level>` |
| Dev console | auth level 3 (`python tools/elsdb.py setting auth_level 3`), END key |

## Planning evidence (2026-10-20)

Kept in `june2017_re/char_arme/` (digests written by Sonnet subagents, spot-checked; CH0 re-verifies anything it builds on):
- `plan_charport_scan_20261020.txt`: the full scan, every character.
- `plan_march_side.md`: Aisha's March class file, script, motions, emulator job change, stat rows, cut-in table, `UNIT_CLASS` lines.
- `plan_june_side.md`: June's script census, the per-class tsv hits, the 64 June Aisha Lua files, June's 69 `Motion_Arme*` (data200).
- `plan_numerical_rows.tsv`: the 110 `c1a_numerical_blocks.tsv` rows keyed to Aisha skill ids.

| Measure | Aisha |
|---|---|
| June skill-tree rows (`s3_reform_ids_no_state.tsv`) | 267 over 7 classes (2: 15; 14 / 15 / 23: 30 each; 104 / 105 / 113: 54 each); 130 distinct ids (106, 900, 22000-22605) |
| Rows with no class state | **260** (only 900 `SI_ETC_WS_COMMON_LOVE` has one) |
| Rows by June skill type | 15: 65, 6: 53, 14: 51, 16: 49, 8: 21, 5: 12, 12: 7, 13 (hyper): 6, 1: 3 |
| June script | `Arme_VioletMage.src.lua` 13,787 lines, data only (no top-level call, no binder): 306 `AVSI_*` + 4 `RNSI_*` + 2 `ABM_*` + 1 `SI_*` state tables, 6 `INIT_*` |
| Script keys to carry | `DAMAGE_DATA` 37 (1 numerical-only), `AT_COMMAND` 18, `TIME_STOP_IMMUNE_STATE` 12, `SHOW_HYPER_CUT_IN_TIME` 3, `APPLIED_SKILL_ID` 3, `DAMAGE_BOUNDING_*` 3, `SET_LIMIT_MAX_G_SPEED` 1, `AVSI_WAIT_STATE_CHANGE` 1 |
| Numerical damage blocks | 110 `DamageEffect*.lua` blocks keyed to Aisha ids, all `ok` |
| Motions | June: 69 `Motion_Arme*` in data200. March: 16 in data033. Base files differ by +0 to +11 animations, none dropped, same bones |
| March class | `X2GUArme_VioletMage.cpp` 21,220 lines (CP949); `SetEquippedSkillFuncBySkillSlotIndex` `:2842-4088`, 57 textual `case SI_*`; pressed-index decode `:5232-5257` |
| June class | `SetEquippedSkillFuncBySkillSlotIndex` `32cede00`, `SetDamageExceptionProcess` `32cccc70`, decide-ACK hook `32ce1770`; only 5 funcmap rows in `0x32cc0000-0x32cfffff`: the class is unmapped |
| Stat rows | `StatTable.lua` 1-99 for all 7 classes (A2) |
| Emulator job change | class-generic (`Handlers_Quest.cpp:555-590`, `Handlers_Shop.cpp`), seeds June's free rows |

## Phases

| Phase | What | Model | Depends |
|---|---|---|---|
| CH0 | Aisha map: June class vs March class, state and skill table, checklist rows per phase, assets per owner (read-only) | Opus high | - |
| CH1 | Deploy unit: June script + motions + class core (states, common moves, hooks) | Opus high, Opus review, user play-test | CH0 |
| CH2 | Skills: Violet Mage + first jobs (classes 2, 14, 15, 23) | Sonnet high, Opus review, user play-test | CH1 review |
| CH3 | Skills: second jobs (104, 105, 113) + hyper actives | Sonnet high, Opus review, user play-test | CH2 review |
| CH4 | Passives and class mechanics (memorize, passive state data, cool-time rules, buff rows) | Opus high, Opus review, user play-test | CH2 review |
| CH5 | UI and data: `UnitTemplet.lua`, cut-ins, 10-slot dialog and tree-UI class pieces | Sonnet high, Opus review, user play-test | CH1 review |
| CH-N | *Template only, not run for Aisha:* a June-only later class path | - | - |
| CH6 | Whole-character walk + gap reruns | Sonnet medium, user play-test | CH3, CH4, CH5 reviews |
| Z | Close-out: MODS, checklist marks, the Stage 7 template, Stage 6 / 7 briefs | Sonnet medium | CH6 |

CH0 runs now, alongside anything (read-only). CH3 and CH4 both edit `X2GUArme_VioletMage.*`: one at a time. CH5 runs beside CH2-CH4.

## Phase CH0 - Aisha map (read-only)

**Run with:** Opus 5.5 high. No code, no flag, no build. Ghidra read-only. Delegate reads, greps and tsv work to subagents (x2-phase,
*When the phase is Opus*).

**Scope.**
1. **Class map.** Walk June's Aisha class: vtable, `InitState` / state registration, `SetEquippedSkillFuncBySkillSlotIndex` `32cede00`,
   every state's start / frame-move / event-process / end function, `DamageReact`, `AttackResult`, `SetDamageExceptionProcess` `32cccc70`,
   the class's `CommonSpecialAttackEventProcess` decode, and every class-only hook the checklist names.
   - Pair each with March's function (or "June-only" / "March-only").
   - Verdict: same / differs / june-only / march-only (June wins: march-only goes).
   - Assign each to its target phase (CH1-CH5).
   - Output: `june2017_re/char_arme/ch0_class_map.tsv` + funcmap rows `Char CH0`.
2. **State and skill table.** For each of the 313 June state tables and each of the 130 skill ids, list: the June state name, the skill id(s)
   that enter it, the June C++ function(s), the March state / function it replaces, and the target phase.
   - Output: `ch0_state_skill.tsv`.
   - Count per phase. If CH2 or CH3 is over ~40 state functions to transcribe, split it (CH2a / CH2b ...) and write the kickoffs for the split.
3. **Unit-script key census** (checklist §5: not built so far, and Stage 7 needs it for every character). Build a reusable
   `june2017_re/charport_keys.py <class script>`. It lists every key the class script uses, June's reader (`xordecrypt.py` / Ghidra) and
   whether March's preprocessed parser reads it. Run it on `Arme_VioletMage`.
   - A key March lacks becomes a CH1 item, or an owner elsewhere if it is not Aisha's.
4. **Checklist rows.** Every Aisha row in §1-§7ak goes to a phase or is marked "n/a for Aisha" with the reason. The known rows:
   - §1 (1 numerical-only table, 110 numerical blocks);
   - §7 l.230; 7b l.307-311 (block `32cccc70`, D8 Fitness 2413 -> Magical Staff 22409); 7c (22 alpha sites, `c10a_alpha_sites.tsv` l.214-235);
   - 7h / 7l / 7o / 7p (binding-circle hyper forced `SetStopTime( 3.0, ... )`) / 7x;
   - 7i (`32cede00` drops June's `31fd2af0` call); 7t (3 `SHOW_HYPER_CUT_IN_TIME`; June cut-in names: `g4_cutin_names.tsv` l.4-10 has no June `.gfx` for March's names);
   - 7u (6 rows, `c11a_march_only_templets.tsv` l.4-5, 40-43); 7ac (Hell Drop, `NUI_UFO_AISHA_DROPPER_MEMO`);
   - 7ae (260 rows); 7ag (decode `:5232-5257`); 7ah / 7aj (memorize slots, slot-data type 5 `328d9ac0`, tooltip `328e9160`); 7ai (`s3e_bybfi_callers.tsv` l.309-312);
   - 7ak (`32ce1770`).
   - Output: a "CH0 checklist map" table in this doc's Done note, and the rows in `ch0_checklist_rows.tsv`.
5. **Assets per owner.** Classify every June Aisha file:
   - 64 Lua (`plan_june_side.md` §4); 69 motions; effect meshes and textures named in the script's `INIT_DEVICE` / state tables.
   - **Aisha's** (CH1 / CH5 take them): the class script, her skill NPC / summon scripts (e.g. `Aisha_PigBat`, `Aisha_StoneWall`, `Ufo_Aisha`,
     `Ufo_Aisha_Droper_Memo`), motions, effect meshes, sounds.
   - **Owned elsewhere** (owner + one-line brief, scope rule): class-change guides `DLG_1ST/2ND_Class_Change_Guide_Aisha` and jumping-system
     dialogs (Stage 9); `DLG_Transcendental_Change_Guide_Aisha` (Stage 6); `Tutorial_Aisha_Stage0` (june-dungeons); `PET_Mystic_ARME_3` (Stage 17);
     `Ain_Arme_Thaumaturgy` (Stage 8); boss / shadow / event NPC scripts (whichever plan already adopted June's NPC Lua: check the ledger
     and `adopt-june` records).
   - Output: `ch0_assets.tsv` (file, kom, June size, March size, owner, phase).
6. **Shared data decisions** (write them down; ask the user only for real choices):
   - **June's `UnitTemplet.lua`.** It is March bytecode now; June's adds `m_HyperCutIn` / `m_BerserkCutIn` / `_Trans` keys and rows for classes
     March's C++ lacks (Lu/Ciel, Rose, Ain, `UC_150`). Say what March's reader does with each (a nil class from `UNIT_CLASS.UC_150`, unknown keys)
     and whether CH5 adopts it whole (the same-name rule) with a reader fix.
   - **The class enum.** Which `RNSI_*` Aisha names the C++ `SKILL_ID` enum needs (values from June's `Enum.lua`), and whether any are absent
     from the core `Enum.lua`.
   - **The slot-5 gap.** `X2GUUser.cpp:25500-25510` returns slot data for `oneSkill5` / `oneSkillEx5` without setting `iSkillSlotIndexPressed_`.
     Confirm it is Stage 6's (TC slot, refused today), or a CH1 item if June sets it there.
7. **Amend the kickoffs.** Rewrite CH1-CH5's "Amended by CH0" sections with the tsv filters, addresses and splits.

**Done gate:** the five tsvs exist; every Aisha checklist row has a phase or an n/a reason; `ch0_state_skill.tsv` covers 313 / 313 states and
130 / 130 skill ids; the CH1-CH5 kickoffs are amended; funcmap rows written; open questions listed for the user.

## Phase CH1 - Deploy unit: June script, motions and the class core

**Run with:** Opus 5.5 high, then an Opus review, then a user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (one flag per logical change
if CH0 splits it). Sub-tags `CORE`, `DATA`.

**Scope** (CH0's `ch1` rows decide the detail):
- **Data in one unit.** Adopt June's `Arme_VioletMage.lua` (`adopt-june`, bytecode unless an edit is needed). Adopt her skill NPC / summon
  scripts that CH0 marked CH1. Stage June's `Motion_Arme*` (data200 -> March's placement via `kom.py stage-june`) and the effect assets the
  script loads. Pack them, then ask before the copy.
- **Class core.** June's state registration and `InitState` order; the common states (wait, walk, dash, jump, combos, dash combos, damage,
  bounding, win / lose, the job start / win / lose blocks) as June has them; the class `CommonSpecialAttackEventProcess` decode, converted to
  June's pressed-index space with every `> 3` / `- 4` site together (7ag; Aisha `:5232-5257`, `:5072`).
- **The class-call hooks** whose engine side already waits: `JuneInitWaitStateChange( "AVSI_WAIT_STATE_CHANGE" )` + the WIN callback (7h),
  `JuneInitBoundingStates( "AVSI_" )` (7l; the first class port also runs C3d-3's review legs), `SET_LIMIT_MAX_G_SPEED` (7o), the 12
  `TIME_STOP_IMMUNE_STATE` states (7p), `AT_COMMAND` combos (7x), the 7i snapshot rule in `SetEquippedSkillFuncBySkillSlotIndex`.
- **Skills stay as they are today.** Unported skill ids keep falling to `default:`; no skill state is ported in CH1 unless CH0 says a
  common state needs it.

**Done gate:** in game, Aisha (any class) loads with June's script and motions. `[JUNE_CHAR] ARME DATA` logs the script and motion load, and
`ErrorLog.txt` has no new `KEM_ERROR` / `Motion Error` / missing-animation line against a pre-CH1 run. Combos, dash, jump, damage and bounding
play June's animations (the 7l `JUNE_FX C3d-3` legs pass). `charport_scan.py` shows nothing new for `Arme_VioletMage`.

## Phase CH2 - Skills: Violet Mage and the first jobs (classes 2, 14, 15, 23)

**Run with:** Sonnet 5.5 high (transcription from CH0's map and the Ghidra decompile of each listed function), then an Opus review, then a
user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (CH0 may split by job). Sub-tags `SKILL`, `NUM`.

**Scope:** CH0's `ch2` rows. For each skill id of these four classes:
- add the `RNSI_*` enumerator;
- add its case in `SetEquippedSkillFuncBySkillSlotIndex`, with June's state functions whole;
- carry the checklist rows attached to that skill: 7b (the `32cccc70` block + D8 swap), 7c alpha-tag calls, 7u effect-name swaps, 7ai BFI
  arguments, 7ac (Hell Drop), 7p (binding circle).
- **Numerical damage, first live class:** checklist §1's gate.
  - Show `[JUNE_SKILL] NUM use ...` for an Aisha skill (`logs.py --tag JUNE_SKILL --grep NUM`).
  - Retire `SERV_IRUHADEV_JUNE_SKILL_NUMERICAL_DEBUG` in the same phase.
  - Write each changed skill's March and June damage to `MODS.md`.

**Done gate:** every class-2 / 14 / 15 / 23 skill id in `s3_reform_ids_no_state.tsv` has a state (re-run the S3 census: 0 `no_state` for these
classes, except ids CH0 marked passive / CH4 / Stage 6). In game, each skill equipped in an A or B slot casts June's state, with June's motion
and effect, and deals damage.

## Phase CH3 - Skills: second jobs (104, 105, 113) and hyper actives

**Run with:** Sonnet 5.5 high, then an Opus review, then a user play-test. Flag as CH2 (or CH0's split). Sub-tag `SKILL`.

**Scope:** CH0's `ch3` rows. It is the same recipe as CH2, for the three second jobs' skills, plus:
- the hyper actives (skill type 13, 6 rows): `ST_HYPER_ACTIVE_SKILL` handling;
- the 3 `SHOW_HYPER_CUT_IN_TIME` states (7t, part 1).

**Done gate:** as CH2, for classes 104 / 105 / 113 and the hyper actives.

## Phase CH4 - Passives and class mechanics

**Run with:** Opus 5.5 high (new mechanisms), then an Opus review, then a user play-test. Flags `SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE`,
`SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE` (or CH0's split). Sub-tags `SKILL`, `MEMO`.

**Scope:** CH0's `ch4` rows:
- June's passive skill-level state data for Aisha's passive ids (types 5 / 8 / 1, and the rest CH0 lists);
- June's memorize system: the 10-slot dialog's memorize slots 7ah, the tree-UI slot-data type 5 `328d9ac0` and its tooltip `328e9160` (7aj),
  the decide-page ACK `32ce1770` (7ak), and the class side that consumes them;
- `ResetLeftSkillCoolTimeBySkillType( ST_SPECIAL_ACTIVE )` with June's rule `32895390` if Aisha reaches it (7ae);
- the §7 buff row (l.230);
- March's `ADD_MEMO_1ST_CLASS` memo branches and Meditation (`:371-417`, `:3930`), kept or removed as June has them.

**Done gate:** each passive's effect shows in game (CH0 names the stat or log line per passive). Memorize stores and casts a skill as June
does. 0 `no_state` rows left for Aisha in the S3 census, except the ones Stage 6 owns.

## Phase CH5 - UI and data: UnitTemplet, cut-ins, dialog and tree-UI class pieces

**Run with:** Sonnet 5.5 high, then an Opus review, then a user play-test. Flags `SERV_IRUHADEV_JUNE_CHAR_UNITTEMPLET` (class-agnostic, if
CH0 decides the adoption needs a reader fix) and `SERV_IRUHADEV_JUNE_CHAR_ARME_UI`. Sub-tag `UI`.

**Scope:** CH0's `ch5` rows:
- June's `UnitTemplet.lua` per CH0's decision (7t data: `m_HyperCutIn`, `m_BerserkCutIn`, sound sets), with whatever reader change it needs.
  Every class sees this file: prove the other eight March characters still load.
- Aisha's cut-in table (`s_SkillCutInSet` / `_FoolsDay`, `X2GUArme_VioletMage.cpp:6-10`, `:964-987`) replaced with June's GFx names (7t, june-gfx G4).
- The 10-slot dialog and tree-UI pieces CH4 did not take (7ah, 7aj).

**Done gate:** `UnitTemplet` loads with no new error for all nine characters (one select-screen pass). Aisha's hyper cut-in plays June's
`.gfx`. The tree UI shows Aisha's June-specific pieces.

## Phase CH-N - A June-only later class path (template; not run for Aisha)

Written here so Stage 7 does not rediscover it. The first character with a templet-backed June-only class runs it as its own phase. The
candidates: Ara 34 / 124, Elesis 35 / 125, Add 33 / 36 / 123 / 126, and `UC_150` (Elsword, motions shared with SK / LK; check it first, it may
be a placeholder).

- **Class enum.** The C++ `UNIT_CLASS` entry, flag-gated. `Enum.lua` already has 31 June names (S3). `UNIT_CLASS` is client-side (`X2Unit.h`),
  but check every struct that carries a class for `UC_END`-sized arrays (the socket loader drops classes >= `UC_END` 124).
- **Templet and motions.** The June `UnitTemplet` row (CH5's adoption), the new motion file (e.g. `Motion_Ara_LS.x`), and the job start / win /
  lose state blocks.
- **Job change.** June's class-change route for the new class: the quest (`CJT_*`, quest-system Q1), the cash item, and the emulator's
  class-generic handlers (`Handlers_Quest.cpp:555-590`, `Handlers_Shop.cpp`). Add the class to `X2OfflineClassChangeSeed.h` / `X2OfflineLuaEnumSeed.h`.
- **Skills.** Tree rows for the class: `s3_reform_ids_no_state.tsv` has none for June-only classes, so regenerate it with the class added.
  Free rows go to `SeedJuneAutoLearn` (`X2OfflineJuneSkillAutoLearn.h`, `tools/data/june_skill_autolearn.tsv`).
- **Stat rows (open decision, backlog "Open decisions that gate stages").** A2's permission does not extend to a new class ID. First look for
  June data (June binary, June server Lua, the live DB). If there is none, ask the user, then use A2's generator (`june2017_re/cap99_gen.py`)
  only with their yes. Until then the loader falls back to the base class's rows and logs it.
- **Starter items.** None needed: a later path is reached by job change, not creation.

## Phase CH6 - Whole-character walk and gap reruns

**Run with:** Sonnet 5.5 medium, then the user's play-test.

**Scope:**
- Prepare one Aisha per class (`/x2-create-char` or `elsdb.py setchar`, levels per class; back up first). Hand the user a walk: every skill
  in an A and a B slot, a hyper, memorize, one dungeon per job.
- Re-run `census.py`, `charport_scan.py`, the S3 no-state census and `charport_keys.py` for Aisha, and record the before / after counts.

**Done gate:** the user's walk passes; 0 Aisha `no_state` rows except Stage 6's; no new `KEM_ERROR` in the walk's `ErrorLog.txt`.

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium.

- `MODS.md`: one "June character: Aisha" section (flags, files, data, revert order: exe + data036 + motion koms together).
- Retire `SERV_IRUHADEV_JUNE_CHAR_DEBUG` if it was used.
- The checklist rows marked `(Aisha: CHn, <commit>)`.
- **The Stage 7 template.** Update "Template for Stage 7" below with what the pilot actually took (phase sizes, splits, model choices that
  worked, defects found in the recipe).
- Write `kickoffs/june-backlog/stage6-plan.md` (transcendence on Aisha) and `stage7-plan.md`, with their `june-backlog` STATUS rows.
- Add a backlog Stage 5 closing note.

## Template for Stage 7

Stage 7 repeats this doc per character: `JUNE_CHARACTER_<NAME>_PHASES.md`, plan key `char-<name>`, tag `[JUNE_CHAR] <PREFIX>`, flags
`SERV_IRUHADEV_JUNE_CHAR_<PREFIX>_*`. The phase skeleton is CH0 -> CH1 -> CH2 / CH3 (skills by job tier) -> CH4 -> CH5 -> CH-N (if the character
has a templet-backed June-only class) -> CH6 -> Z. Z of this plan rewrites this section from what the pilot took.

What differs per character, as known on 2026-10-20 (gap = June attacks whose damage is numerical-only; `l.` = checklist line):

| Character | Gap | Templet-backed June-only class | Known extras |
|---|---|---|---|
| Eve | 3 | - | 167 alpha-tag sites (7c); slot encoders in `X2GuEve.cpp` (7ag trap); El Crystal icons, recovery statics (7ah / 7aj); 7a modifier, 7n sync float |
| Rena | 12 | - | **`ADD_RENA_SYSTEM` decision** (natural force, 77 uses); 7ad renames (`Lire_*`); D8 3405 -> 23405 |
| Ara | 14 | 34, 124 | `CUSTOM_RENDER_STATE` (7w); 2 new motion files; orb handle retirement needs the user's OK (7e) |
| Chung | 15 | - | `ChangeMaxCharacterAddResource` already ported; 49 unnamed upgrade ids (7ae); cinematic UI (7r) |
| Elsword | 20 | `UC_150` (verify) | `DAMAGE_SKILL_ID` x103 in `Elsword_DamageData.lua` (§2); Rune Slayer reflect (7j); S3d memo consumers |
| Elesis | 20 | 35, 125 | already on June base / SK motions (6 anims lost, accepted); first live reserved-buff path (7ai) |
| Raven | 25 | - | 7b passive setter `+0x4108`; 7u effect names; power-rate operator (§7 l.243) |
| Add | 0 (skeleton) | 33, 36, 123, 126 | March kit incomplete ("the user ports Add from June", l.237); `CUSTOM_RENDER_STATE`; no stat rows for 9 / 32 / 122 at all; 4 new motion files; 13 §7 rows |
