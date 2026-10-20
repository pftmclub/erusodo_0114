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

**Scope: what June 2017 ships, up to transcendence (user, 2026-10-20).** That means the character revamps, the class lines June has for March
characters, the new characters and transcendence. **Third jobs are out.** June's `UC_150`-`UC_186` block is never ported by any phase of this
plan or of Stage 7. It includes Aisha's `UC_ARME_AETHER_SAGE` 153, `_OZ_SORCERER` 154 and `_METAMORPHY` 155, which are enum names only, and the
one placeholder `UnitTemplet` row, `UC_150`. A June table that carries such a row is still adopted whole (the same-name rule). The row stays
inert, and the reader only has to survive it.

**What Aisha does not exercise: a June-only class line.** All seven of Aisha's June classes already exist in March. So this plan's class-line
phase (CH-N, below) is written as a template block and marked **not run for Aisha**. Its first real run is the first Stage 7 character that has
one: Ara (34 Little Specter, 124 Asura), Elesis (35 Dark Knight, 125 Crimson Avenger) or Add (33, 36, 123, 126).

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
- **Shared Lua is ported per character (user, 2026-10-20; the pilot and Stage 7).** A shared `.lua` table that holds rows for several characters or classes (`UnitTemplet.lua` and the like) is not adopted whole by a character port. Only the rows of the character being ported become June's; every other character's rows stay March's until that character's own port. The file becomes core-list BOM source (`luasrc.py convert` + a core row, `--why "edit: <phase> per-character rows"`) so each later port swaps in its own rows. A Lua file that belongs to one character (its class script, its skill NPC / summon scripts) is still adopted whole.
- **Lua.** The class script arrives through `luasrc.py adopt-june` (June's `ljconv` bytecode). It becomes core-list BOM source only if a phase
  must edit it (`luasrc.py core --add ... --why "edit: pilot-char-revamp CHn <what>"`, no question; then `adopt-june --source`).
  - Never copy from `june_src_fixed/`; that is reading material.
  - `Enum.lua` is core source: edit it as text and run `luasrc.py refresh`.
- **Assets.** June's same-name files replace March's, byte-exact, through `kom.py stage-june` (motions, meshes, textures, `.gfx` cut-ins).
  Pack them yourself; ask before every `/x2-kom-packed copy`.
- **The character's June assets come first, whole: no half-March, half-June character (user, 2026-10-20).** Every non-Lua asset that is Aisha's
  becomes June's, byte-exact, in one deploy (CH0b), before any code phase. That covers:
  - motions;
  - body, face and hair models and their textures;
  - equipment meshes and textures;
  - skill effects (effect meshes, particle and mesh-player `.txt` definitions, their textures);
  - sounds;
  - cut-in art and UI portraits.

  CH0 lists them, and CH0b stages, packs and copies them. Nothing is left on March's copy "until its skill is ported". The asset-port plan
  (Stage 15, not started) is not waited for; its waves later carry the same bytes, so nothing collides. Names built at run time are part of the
  set ([[lanox-fix-effect-data-closure]]). If a March reader breaks on a June asset, the fix is code (the same-name rule), never keeping
  March's file.
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
| CH0 | Aisha map: June class vs March class, state and skill table, checklist rows per phase, every Aisha asset (read-only) | Opus high | - |
| CH0b | **Every June Aisha asset, whole, first** (motions, models, textures, equipment, effects, particles, sounds, cut-in art) | Sonnet high, user play-test | CH0 |
| CH0c | Registration map correction: `charport_states.py` carry-over (found by the CH1b review), state / class maps rebuilt, CH1a / CH1b registration claims corrected | Opus high | CH1b review |
| CH1 | Deploy unit: June script + class core (states, common moves, hooks) | Opus high, Opus review, user play-test | CH0b |
| CH2 | Skills: Violet Mage + first jobs (classes 2, 14, 15, 23) | Sonnet high, Opus review, user play-test | CH1 review |
| CH3 | Skills: second jobs (104, 105, 113) + hyper actives | Sonnet high, Opus review, user play-test | CH2 review |
| CH4 | Passives and class mechanics (memorize, passive state data, cool-time rules, buff rows) | Opus high, Opus review, user play-test | CH2 review |
| CH5 | UI and data: `UnitTemplet.lua`, cut-ins, 10-slot dialog and tree-UI class pieces | Sonnet high, Opus review, user play-test | CH1 review |
| CH-N | *Template only, not run for Aisha:* a June-only later class path | - | - |
| CH6 | Whole-character walk + gap reruns | Sonnet medium, user play-test | CH3, CH4, CH5 reviews |
| Z | Close-out: MODS, checklist marks, the Stage 7 template, Stage 6 / 7 briefs | Sonnet medium | CH6 |

CH0 runs now, alongside anything (read-only). CH0b follows straight after it: the assets are the top priority. CH3 and CH4 both edit `X2GUArme_VioletMage.*`: one at a time. CH5 runs beside CH2-CH4.

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
5. **Every Aisha asset, and its owner.** This is the list CH0b stages. It must be complete (user: no half-March, half-June character). Find
   every June file that is Aisha's, by three routes, and union them:
   - (a) by name: `Arme` / `Aisha` (and her job abbreviations) over all 260 June koms with `junekom.py list`;
   - (b) by reference from her data: her script's `INIT_DEVICE` / `INIT_MOTION` / `INIT_COMPONENT` tables, her June `UnitTemplet` rows (body
     motion files, part meshes, face / hair), and the 5b closure;
   - (c) by reference from her equipment: the mesh and texture names of every June `item.kim` item whose class is Aisha's (`tools/kim.py`;
     costumes, weapons, armour and accessories).

   For each file, give:
   - its March state: missing, March's copy, or already June's bytes;
   - its June kom;
   - its March placement (`stage-june` rules).

   Classify each file:
   - **Aisha's non-Lua assets** (CH0b takes all of them, whole): motions (69), body / face / hair models and their `.xet` / textures,
     equipment meshes and textures, effect meshes, particle and mesh-player `.txt` definitions and their textures, sounds, cut-in `.gfx` / art,
     UI portraits.
   - **Aisha's Lua** (CH1 takes it): the class script and her skill NPC / summon scripts (e.g. `Aisha_PigBat`, `Aisha_StoneWall`, `Ufo_Aisha`,
     `Ufo_Aisha_Droper_Memo`).
   - 64 June Lua files match her name (`plan_june_side.md` §4).
   - **Owned elsewhere** (owner + one-line brief, scope rule): class-change guides `DLG_1ST/2ND_Class_Change_Guide_Aisha` and jumping-system
     dialogs (Stage 9); `DLG_Transcendental_Change_Guide_Aisha` (Stage 6); `Tutorial_Aisha_Stage0` (june-dungeons); `PET_Mystic_ARME_3` (Stage 17);
     `Ain_Arme_Thaumaturgy` (Stage 8); boss / shadow / event NPC scripts (whichever plan already adopted June's NPC Lua: check the ledger
     and `adopt-june` records).
   - Output: `ch0_assets.tsv` (file, kind, route a/b/c, June kom, June size, March kom, March size, March state, owner, phase), with totals
     per kind and per March state. Also list any March reader that a June asset could break: a renamed animation (none for Aisha per
     `r12b_anim_loss.tsv`), a frame-count or bone change, a texture name the C++ hard-codes. Each one becomes a CH0b or CH1 item.

   5b. **Effect and particle closure, per state.** Walk out from each of Aisha's 313 June states and her C++ functions:
   - **Templets.** The `DamageEffect` / `EffectSet` templet names they use, including names built at run time.
   - **What each templet plays.** Particle sequences, mesh-player entries (`.txt` definitions) and damage effects.
   - **The files behind them.** Every texture (`.dds` / `.tga`), mesh (`.x` / `.X`), animation and sound.

   For each name, record two things:
   - whether March has it today, and as June's bytes or March's;
   - which June kom it comes from.

   Output: `ch0_effect_closure.tsv` (state, kind, name, March status, June kom, phase), with a count of missing / March-copy / already-June.

   Also find out where June's particle and mesh-player `.txt` definitions stand in March today: June's, merged, or March's (the effect plan's
   Phase 10 ported the keys; the asset-port plan leaves `.txt` with the effect plan). Aisha's definitions go in with CH0b. If a definitions
   file is shared by every character, say how CH0b takes June's file whole without breaking the others.
6. **Shared data decisions** (write them down; ask the user only for real choices):
   - **June's `UnitTemplet.lua`.** It is March bytecode now; June's adds `m_HyperCutIn` / `m_BerserkCutIn` / `_Trans` keys and rows for classes
     March's C++ lacks (Lu/Ciel, Rose, Ain, and the third-job placeholder `UC_150`, out of scope). Say what March's reader does with each (a nil
     class from `UNIT_CLASS.UC_150`, unknown keys) and whether CH5 adopts it whole (the same-name rule) with a reader fix that keeps those rows inert.
   - **The class enum.** Which `RNSI_*` Aisha names the C++ `SKILL_ID` enum needs (values from June's `Enum.lua`), and whether any are absent
     from the core `Enum.lua`.
   - **The slot-5 gap.** `X2GUUser.cpp:25500-25510` returns slot data for `oneSkill5` / `oneSkillEx5` without setting `iSkillSlotIndexPressed_`.
     Confirm it is Stage 6's (TC slot, refused today), or a CH1 item if June sets it there.
7. **Amend the kickoffs.** Rewrite CH0b-CH5's "Amended by CH0" sections with the tsv filters, addresses and splits.

**Done gate:** the six tsvs exist (with `ch0_effect_closure.tsv`); every Aisha checklist row has a phase or an n/a reason; `ch0_state_skill.tsv` covers 313 / 313 states and
130 / 130 skill ids; `ch0_assets.tsv` unions routes a, b and c; the CH0b-CH5 kickoffs are amended; funcmap rows written; open questions
listed for the user.

### CH0 - Done (2026-10-20)

Read-only, no flag, no build. Outputs in `june2017_re/char_arme/` (the `ch0_build_*.py` scripts there rebuild the two maps from Ghidra):
- **`ch0_class_map.tsv`**: 560 June functions in Aisha's range **`0x32cb6e00`-`0x32cf2da0`** (Rena ends `32cb6c6e`, Elsword starts
  `32cf2da0`; 429 Ghidra carved, the rest are state callbacks it had not): 63 vtable overrides (not 47: 16 sit below `32cc0000`), 4
  registrars, 364 state callbacks, 94 helpers (phased by their earliest caller), **37 dead** (no pointer, call, jmp or thunk anywhere in the
  exe). Each row: March pair, verdict, phase. 523 to transcribe. 75 funcmap rows `Char CH0`.
- **`ch0_state_skill.tsv`**: 313 / 313 script states and 130 / 130 skill ids. States: 165 + 37 + 32 pair, 36 + 4 + 16 June-only by phase
  CH1 / CH2 / CH3 families, **23 dead data** (Ice Storm, Icicle Wave, Aqua Shower, Vital Drain ...: no June C++ names them), 6 March-only (June
  dropped the `_HYPER` tables of Binding Circle, Chain Burst, Gust Storm, Death Field, plus `DARK_MAGICIAN_DASH_COMBO_Z3up` and
  `SI_A_AV_CIRCLE_FLAME_MEMO`), 1 June quirk (`AVSI_RIDING_DAMAGE_BACK`
  registered with no table). Skills: CH2a 26, CH2b 24, CH3a 32, CH3b 22, CH4 25 (types 1 / 5 / 8), CH1 1 (900).
- **`ch0_checklist_rows.tsv`**: every §1-§7ak row, with its phase or an n/a reason. 13 sections name Aisha, 15 apply to every class, 17 are
  other characters'.
- **`ch0_assets.tsv`**: 13,833 rows (routes a 12,097 names, b script / UnitTemplet / in-mesh textures, c item.kim UT_ARME). CH0b stages
  **5,688** (`ch0b_stage_list.tsv`); 7,975 are June's bytes already.
- **`ch0_effect_closure.tsv`** (`charport_closure.py`): 6,394 rows, 2,229 names. 2,127 June's, 33 March copies, 69 missing (49 sounds; 3 June
  data gaps; the rest are names June itself never defines). **The particle and mesh-player `.txt` files are June's, byte for byte, and every
  character shares them**: CH0b stages no definitions.
- **`ch0_keys.tsv`** (`charport_keys.py`, checklist §5): 224 keys, 7 June-only (CH1).

How June's code works (the facts the phases lean on):
- **Registration:** `InitState` `32cea0c0` (vslot 0x7a4) sets the cut-in index and calls `InitStateCommon` `32ce77b0`, `3202b110` and the
  roll `31fe22d0`, then registers per-job blocks. States go in through `UpdateUserUnitStateData` `31fd89b0( id, name, data )`.
- **Callbacks:** each callback is a `jmp` thunk (`32cf0xxx`-`32cf2xxx`) to the real function.
- **`UserUnitStateData` slots:** +0x18 Init, +0x20 StartFuture, +0x28 Start, +0x30 FrameMoveFuture, +0x38 FrameMove, +0x40 CameraMove,
  +0x48 EventProcess, +0x50 EndFuture, +0x58 End.
- **Skill switch:** `32cede00` switches on `32207210( id )` = templet `+0x178` (`m_DeleteSkillIDByUpgrade`), so upgrade ids reach the base case.
  Meditation and the three Body skills have no state: their ids are tested in class code (`32ccbca0`, `32ccf9f0`, `32cc5c30`, `32ce1cc0`,
  `32ccb830`).
- `charport_states.py` (new) extracts both from Ghidra's decompiles.

**Split** (the ~40-function rule; CH1's core is about 370 functions):
- **CH1a-CH1i** (Sonnet): transcribe inert `June_*` members. CH1a movement 40, CH1b reactions / riding 38, CH1c ground combos 30, CH1d dash /
  jump 32, CH1e HM line 46, CH1f DM line 23, CH1g BM line 21, CH1h overrides below vslot 0x600 69, CH1i overrides from 0x600 41.
- **CH1** (Opus): wires them up, adopts the script, play-test.
- **CH2a** VM + HM 49, **CH2b** DM + BM 34, **CH3a** AEM + AVP 36, **CH3b** ADW + hyper actives 15, **CH4** 20 functions + memorize UI.
- The kickoffs carry the filters.

Corrections to earlier docs:
- **7p:** June never runs the forced `SetStopTime`. `32cd5d60` is dead, and June uses the normal Binding Circle table for both slots.
- **7ad:** `Motion_Arme_EM.x` hands three Meteor Shower animations to `Motion_Arme_HM.x`. The EM class loads both, so CH0b stages them together.
- **7c:** 4 of the 22 alpha sites are in dead functions. **7 l.264:** `32cc7300` is dead.
- **7b** moves from CH2 to CH4: one passive function, ported whole.
- **Slot-5 gap:** `31fcadd0` returns 4 / 9, so it is CH1's 7ag conversion, inert until Stage 6.

Item 6 decisions:
- **Enum:** 129 / 130 Aisha names are not in the C++ `SKILL_ID` enum; keep the house convention (raw id + `/// RNSI_` comment).
  69 / 130 are not in the core Enum.lua: add a member only if a Lua reader needs it.
- **UnitTemplet:** March's reader skips `UC_150` safely (nil enum -> row dropped, `X2UnitManager.cpp:844`) and ignores unknown keys, so no
  reader fix is needed. Adopting the whole file would also rename 15 other second-job classes, so **the user decided (2026-10-20): per
  character rows only**. CH5 replaces Aisha's 7 rows; this became the shared-Lua rule in Rules and the backlog's standing rules.

Drift: none needs a new plan. Aisha's `Motion_Arme_Emotion_Cash25.x` belongs to Stage 10 (backlog l.447), which owns the motion's behaviour;
CH0b stages the file. For Stage 7 / Z, the class-map builders, `charport_states.py`, `charport_keys.py` and `charport_closure.py` are the
template's step 0.

Open questions for the user:
1. ~~CH5: UnitTemplet whole or Aisha's rows?~~ Answered (user, 2026-10-20): Aisha's rows only (shared-Lua rule).
2. CH0b: approve the copy of all 54 koms at once.

## Phase CH0b - Every June Aisha asset, whole, first

**Run with:** Sonnet 5.5 high (mechanical staging from CH0's list), then a user play-test. No C++ unless CH0 found a March reader that a June
asset breaks: then that fix comes here, under `SERV_IRUHADEV_JUNE_CHAR_ARME_ASSET`. Sub-tag `DATA`.

**Scope:** every row of `ch0_assets.tsv` whose owner is Aisha and whose kind is not Lua:
- motions, models, faces, hair, equipment, textures, effect meshes, particle / mesh-player definitions, sounds, cut-in art, portraits.

How:
- Stage them byte-exact with `kom.py stage-june` (June wins at the same name; June-only files are added).
- Pack each touched kom, then ask before each copy.
- One deploy, so Aisha is never half March, half June.

**Why before the code:** for Aisha it is safe. M3 and `r12b_anim_loss.tsv` show June's motion files keep every animation March's script plays,
with the same bones, so her March script keeps working on June's assets until CH1 swaps the script. For a Stage 7 character with renamed
animations (Rena, Elesis, Raven, Add, Ara: checklist 7ad), CH0b and CH1 deploy together instead. The assets still all go in at once.

**Done gate:**
- `ch0_assets.tsv` re-run shows 0 Aisha non-Lua rows that are missing or still March's copy.
- In game, Aisha (one save per job tier) in village and dungeon shows no new `KEM_ERROR` / `Motion Error` / missing-texture / missing-mesh line
  against a pre-CH0b `ErrorLog.txt`.
- Her body, equipment and effects render (the hand-off names a costume, a weapon and three skills to look at).

### CH0b - Done (2026-10-20)

Sonnet 5.5. No C++, no flag (CH0 found no March reader a June asset breaks). Assets only; nothing under the Trunk changed but this doc,
`STATUS.md` and the CH1 pointers.
- **Reference:** `ErrorLog.txt` (1,394,741 B, last run 2026-10-19 23:11) saved as `data\ErrorLog_pre_CH0b.txt` before the copy.
- **Staged:** `kom.py stage-june --jobs 8 june2017_re/char_arme/ch0b_stage_list.tsv`:
  `STAGE-JUNE OK: 53 kom(s), 5688 staged (5302 new, 386 replaced, 0 identical), 0 refused, 34s`. Replaced March files are backed up under
  `24965799\kom_backup_stage_june61020_055218`. 53 koms, not 54: the list's 54th was CH0's count of koms before cross-kom moves merged.
- **Packed:** `kom.py pack` x53, 53 `PACK OK`, 0 refusals (every `.lua` gate clear: the list stages no Lua).
- **Copied (user approved all 53, 2026-10-20):** `kom.py copy` x53, 53 `COPY OK` (data002 52,471,266 B / 2,623 entries md5 09a8309c...; data015 89,258,122 B /
  5,801 entries; data016 25,522,439 B / 2,078 entries). Backups in `kom_backup_tools`. Four koms are new archives with no live file: data198 (289),
  data226 (39), datai025 (1), datai031 (92). `X2Main.cpp:809-835` mounts `data001-226` and `datai001-032` (`SERV_IRUHADEV_KOM_FILE_COUNT_226`,
  `Always.h:3913`), so all four are mounted.
- **Done-gate re-run:** `kom.py stage-june --dry-run` over the same list after staging: `5688 staged (0 new, 0 replaced, 5688 identical), 0 refused`.
  Every row of `ch0b_stage_list.tsv` is June's bytes in `kom_contents`; 0 stay `missing` or `march-copy`. The 7,975 rows that were June's already, the 99
  `dangling` rows and the 3 name false positives need nothing. `elsword_SA_SwordBlastiong_slash.dds` stays March's (absent from June; CH1-CH3 remove its C++).
- **Stage together:** `Motion_Arme_EM.x` + `Motion_Arme_HM.x` (Meteor Shower Start / Loop / End moved to HM) both packed in the same run (data033 via the
  data200 -> data033 move); `Motion_Arme_Common.x` has 34 changed bind-pose frames (Bip01 y +8.6), so look at her standing.
- **Not done here:** no save changes. `createchar.py` / `elsdb.py addchar` refuse because the save has no level-1 unit to copy inventory sizes from;
  the hand-off asks the user for one throwaway level-1 character, after which the other job tiers are added in one command.
- **Play-test (user) is the gate.** Log tag `[JUNE_CHAR]` has no lines yet (no code); the check is `ErrorLog.txt` against `ErrorLog_pre_CH0b.txt`
  (`KEM_ERROR`, `Motion Error`, missing texture / mesh lines).
- **No new drift.** `Motion_Arme_Emotion_Cash25.x` is staged (file only); its behaviour is Stage 10's.
- **Play-test PASSED (user, 2026-10-20: "nothing wrong").** ErrorLog run 05:57-06:04, after the 05:52 copy, against `ErrorLog_pre_CH0b.txt`: no `Motion Error`, no missing Aisha file that June has. The new Aisha lines are all June's own: KEM 433 / 209 (anim 76 vs mesh 75 frames on `Mesh_Arme_Event_LowBody11` / `Content_Onepiece10`, both June-byte); KEM 429 / 428 on `ARME_BASIC_FACE_02.TGA`, `..._VP.TGA.TGA`, `..._EYECLOSE3 .TGA`, `Weapon08 / 09Effect` (named inside June files, in no June kom: the `dangling` rows); KEM 381 result 47 on `flame.ogg` (same bytes as March's) and `Aisha_VP_DashJumpAttackX.ogg` (engine 3D-position call, not the file). `HQ_CutIn_Aisha_VP_1.dds.gfx` (KEM 135) is the GFx lookup of the script's `.dds` name, which June has no `.gfx` for either: the cut-in table is CH5's.

## Phase CH0c - Registration map correction

**Run with:** Opus 5.5 high (RE data that CH1's wiring is built on). No play-test. Found by the CH1b review (its note says how). Runs before CH1c.

**Why.** June's registrar calls `UserUnitStateData::Init` (`320775a0`) between registrations, and Ghidra prints it as `FUN_320775a0();`. `charport_states.py`
cleared its slot table only on inlined zero stores, so each state inherited the previous state's callbacks. The CH1b review fixed that (the tool now clears on
the call). Old and fixed outputs: `june2017_re/char_arme/ch1b_review_reg_old.tsv` / `ch1b_review_reg_fixed.tsv`. 99 of 139 plain rows change. A second defect
remains: a `~` row (a name set inside a per-class branch) takes its callbacks and id when the name is assigned, and Ghidra prints that before the branch's
stores. All 33 such rows are unreliable (e.g. `AVSI_ABM_COMBO_Z4` reads id 164, but the branch sets `0xcb`).

**Scope:**
1. Fix the `~` rows in `charport_states.py`. A branch's row takes what that branch stores up to its closing brace, plus the stores after the merge and
   before the `31fd89b0` call. Check every `~` row against the listing (`funcdis.py`: the pushed id, the table, the slot stores before the call).
2. Re-run `init 32ce77b0 32cea0c0 3202b110`; re-run `skill 32cede00` and confirm it is unchanged. Rewrite `ch0_state_skill.tsv` part 1 (june_callbacks) and
   the `states` column of `ch0_class_map.tsv` from it. Re-run a builder whole only if its scratch inputs are rebuilt and the diff stays in those columns
   (`ch0_build_class_map.py` also rewrites checklist rows).
3. Phases. A row CH1a or CH1b already ported keeps its phase (e.g. `32cbb8d0` / `32cbba10` are `AVSI_COMBO_X4_CHARGE` only, ported in CH1b). List every
   function whose phase would move among CH1c-CH1i, and move it only if no phase has ported it yet. Check `CH1c.md`-`CH1i.md` against the result.
4. Correct what was written from the old map. Registration claims in the CH1a / CH1b code comments (under the flag, `bytepatch.py`); the CH1b list is in
   the CH1b review. The `Char CH1a` / `Char CH1b` funcmap rows. The CH1 kickoff (registration notes). The CH1a / CH1b Done notes stay as history; the
   correction goes in this phase's Done note.
5. Finish: `flagcheck.py JUNE_CHAR_ARME_CORE --worktree`, `build.py --deploy` (comments only: a compile check), Done note, STATUS row, commit.

**Done gate:** every registration row matches the listing (all `~` rows checked by hand, a sample of 10 plain rows); the class map's phase column changes
only for unported rows, each listed; the code comments and funcmap rows name June's real state sets.

### CH0c - Done (2026-10-20)

**Tool.** `charport_states.py init` now reads a `~` row (a name held in a variable) along its own path: a case or else starts from the state at
its switch / if, not from the previous sibling's. From the name assignment it takes the rest of the branch (to `break`, `goto` or the closing
brace), then the stores after the merge, up to the `31fd89b0` call that uses the name. The id is the variable's value on that path. `fnval`
also accepts `DAT_` labels: `AVSI_RIDING_SPECIAL_MOVE` had lost EndFuture `329cb580`, a thunk Ghidra had not carved. `skill 32cede00` is unchanged.
**Proof:** `ch0c_rebuild_reg_columns.py` fed the CH0 `reg.tsv` (`ch1b_review_reg_old.tsv`) and today's `skill.tsv` reproduces HEAD's
two maps with 0 changes.
**Listing check (every row, not a sample of 10).** `ch0c_listing_check.py` (new; Stage 7's step 0 can reuse it) walks the raw listing (esp depth, `mov reg, imm`, slot stores, the three
pushes): 170 / 170 distinct rows of the new `reg.tsv`, while the CH1b review's output fails 30. It goes back from each name push to the branch's `Init` and then forward through `jmp` to the call. Result: 163 / 163 literal-name
registrations equal the new `reg.tsv`, all 33 `~` rows among them (e.g. `AVSI_ABM_COMBO_Z4` id 203 = `0xcb`, EventProcess `32cc0390`; `AVSI_COMBO_X3front`
loses the Init `32cb7d00` it took from its AEM sibling). A walk back from the call matches the other 9 (4 std::string names and the 5 of `3202b110`). Every block
that starts right after the previous registration has June's inlined Init (fld1 + zero stores), so nothing carries over.
**Found, Aisha's, fixed here:** four registrations take their table name from a std::string, so CH0 attached no callbacks to them.
- `USI_HYPER_MODE` (`32ce77b0` id 0x17, name `31fdd490( job )`): its callbacks now sit on the four hyper-mode rows. Init `32cb7110` was a helper
  and is now a CH1 state function; CameraMove `32cd1470` is ported in CH1b.
- `USI_START / WIN / LOSE` (`32cea0c0` ids 9 / 0xb / 0xc, the roll `31fe22d0`): CX2GUUser callbacks, now on the rolled rows.
- Two new funcmap rows: `32ce77b0 @32ce8674` and `32cea0c0 @32cea2f5`. Nothing drifts: both are CH1's registration.

**Maps** (`ch0c_rebuild_reg_columns.py <S> --write` on `ch0c_reg.tsv` / `ch0c_skill.tsv`, saved beside them; it also recomputes the totals lines):
- `ch0_state_skill.tsv`: 162 state rows change (june_registration ids and paths, june_callbacks).
- `ch0_class_map.tsv`: 77 rows change.
- Helper phases re-run with the builder's fixed point; none moves because of the fix. 11 helpers disagree with the builder on the old map
  too (its order), so they are left alone.

**Phase moves**, functions to transcribe after CH0c: CH1c 30, CH1d 32 -> 26, CH1e 46 -> 50, CH1f 23 -> 31, CH1g 21 -> 23, CH1h 69 -> 62,
CH1i 41, CH1 29 -> 28. Each kickoff's "From CH0c" section lists its ins and outs.
- The seven state callbacks CH1h held as "job branch the extractor skips" go to their states' phases (c / d / e / f).
- `32cc0390` (`ABM_COMBO_Z4` EventProcess) moves CH1 -> CH1g.
- **Kept (ported):** 11 functions whose map phase would change keep their phase and member. `32cc8800` (CH1a); `32cbb8d0` / `32cbba10`;
  `32cb7530` / `32cc6ca0` / `32cd5360`; `32cb8a50` / `32ce6f20` / `32ce1590` / `32cf0fa0` / `32cd1470` (all CH1b).

**Corrections to what the old map wrote** (the CH1a / CH1b Done notes stay as history and are superseded here):
- 12 comments under the flag in `X2GUArme_VioletMage.cpp`, 1 in `.h` (`bytepatch.py`):
  - `32cc8800` is the Init of the two TELEPORT_STANDUP_ATTACK states, not of 59 states;
  - DIE (no riding-attack FrameMove), AIR_DOWN_LANDING Init, DAMAGE_DOWN_BACK FrameMove and DASH_JUMP_ATTACK_Z each serve their own state only;
  - STANDUP_ATTACK Start / End serve the 2 states, not 12 / 11; `32cbb8d0` / `32cbba10` serve COMBO_X4_CHARGE only;
  - the riding hyper set serves RIDING_HYPER_MODE only; TELEPORT_END FrameMove has no STAND_UP_START;
  - `32cd1470` is also the USI_HYPER_MODE CameraMove; `GetJumpSpeed()` is `+0x4a78`.
- funcmap: the same claims in rows `32cc8800`, `32cb9de0`, `32cb7240`, `32cb7530`, `32cbb8d0` and `32cd1470`.
- CH1 kickoff: wire from `ch0_state_skill.tsv` only; the hyper-mode / roll rows; the kept-phase list.

**Hygiene.** `flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARME_CORE --worktree --files X2Lib/X2GUArme_VioletMage.cpp X2Lib/X2GUArme_VioletMage.h`:
`files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 13 deleted line(s), 0 non-ASCII
inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The 13 deleted lines
are the 13 replaced comment lines, each inside this flag's own block. Build (comments only, a compile check): `BUILD OK: X2/US_SERVICE/x2.exe 25,526,784 B, 2026-10-20 08:57:28`; the deploy was first refused (the game was running), then `--deploy-only`: `DEPLOY OK: X2_offline.exe is 25,526,784 B, dated 2026-10-20 09:00:03, md5 3be06d4f402f411062dffa0badcedcb3, equal to X2/US_SERVICE/x2.exe (2026-10-20 08:57:28); built at main 86b78004.`
**In game:** nothing (comments and RE data). Next: CH1c.

## Phase CH1 - Deploy unit: June script, motions and the class core

**Run with:** Opus 5.5 high, then an Opus review, then a user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (one flag per logical change
if CH0 splits it). Sub-tags `CORE`, `DATA`.

**Scope** (CH0's `ch1` rows decide the detail):
- **Data in one unit.** Adopt June's `Arme_VioletMage.lua` (`adopt-june`, bytecode unless an edit is needed). Adopt her skill NPC / summon
  scripts that CH0 marked CH1. Her non-Lua assets are already June's (CH0b); stage only what CH0b's play-test or a run-time name found
  missing. Pack, then ask before the copy.
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

### CH1a - Done (2026-10-20)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (`KTDXLIB/Always.h`), the commit "Character pilot Phase CH1a". The 40 class-map rows of phase CH1a are 40 new
`CX2GUArme_VioletMage::June_*` members: declarations after `ChargeMPEnd` in `X2GUArme_VioletMage.h`, bodies at the end of `X2GUArme_VioletMage.cpp`, each with its
June address. Nothing registers, calls or overrides them; March's members are untouched. 44 funcmap rows `Char CH1a` (`june2017_re/funcmap.tsv`).

| June | Member (`June_` prefix) | June | Member |
|---|---|---|---|
| `32cb6f70` | WaitStartFuture | `32cb9a40` | WaitStart |
| `32cb6fa0` | WaitFrameMoveFuture | `32cd0200` | WaitEventProcess |
| `32cd0490` | WalkEventProcess | `32cc5e40` | JumpReadyEventProcess |
| `32cb6fd0` | JumpUpEventProcess | `32cb9a70` | JumpDownEventProcess |
| `32cb7090` | JumpLevitationStartFuture | `32cc5f40` | JumpLevitationFrameMoveFuture |
| `32cd0670` | JumpLevitationFrameMove | `32cb9ba0` | JumpLevitationEventProcess |
| `32cd0730` | JumpLevitationEnd | `32cd07e0` | JumpLandingEventProcess |
| `32cd0a40` | DashEventProcess | `32cb70b0` | DashEndStart |
| `32cc6040` | DashEndFrameMoveFuture | `32cb9c60` | DashEndFrameMove |
| `32cd0c40` | DashEndEventProcess | `32cc60a0` | DashJumpEventProcess |
| `32cb70c0` | DashJumpLevitationStartFuture | `32cc63c0` | DashJumpLevitationFrameMoveFuture |
| `32cd0ef0` | DashJumpLevitationFrameMove | `32cb9ce0` | DashJumpLevitationEventProcess |
| `32cd0fb0` | DashJumpLevitationEnd | `32cd1060` | DashJumpLandingEventProcess |
| `32cb70f0` | DashJumpPowerLandingStart | `32cd1280` | DashJumpPowerLandingEventProcess |
| `32cc8410` | ChargeMPInit | `32ccbe10` | ChargeMPStartFuture |
| `32cd1540` | ChargeMPStart | `32cd1830` | ChargeMPFrameMove |
| `32cd21d0` | ChargeMPEventProcess | `32ccc070` | ChargeMPEndFuture |
| `32cd2420` | ChargeMPEnd | `32cc8800` | EvasionStandupAttackInit (the Init June registers for 59 states) |
| `32cb7570` / `32cb7580` | AVSI_SI_AV_POWER_ATTACK(_HYPER)_FrameMove (a bare jump to `CommonFrameMove`) | `32cbe620` / `32cbe660` | AVSI_SI_AV_POWER_ATTACK(_HYPER)_EventProcess |

**What differs from March** (the rest is March's body): the wait event's MP-charge test, jump up / down / levitation events (double-tap dash, `AVSI_WALL_LANDING`),
the two levitation `FrameMoveFuture` (newest key press picks the facing when both keys are held), the dash-jump events (turn-around, the `@32cc62fd` block runs on the
ground too), the charge-MP start / frame-move / event (MemoRise effect set, `+0x9770` rule), and the two power-attack `FrameMove`s (June dropped the hyper world colour
and the cut-in light).

**Method, for CH1b-CH1i:**
- `m_InputData` is bytes from `+0x86da` in March's member order (`oneZ` `+0x86eb`, `oneX` `+0x86ef`); `SpecialAttackEventProcess( 1 / 2 )` is `ASUT_GROUND / ASUT_AIR`;
  vslots `0x5c` StateChange, `0x668` StateChangeDashIfPossible, `0x55c` TryHyperModeStateChange, `0x200` GetRidingOn, `0x7ec` CreateStepDust; `FUN_3226ddb0( 2 )` is `GetEnableDash( EDS_END )`.
- June inlines `GetWalkSpeed / GetDashSpeed / GetJumpSpeed / GetDashJumpSpeed` as `m_fRunJumpSpeedRate * (a ^ b ^ &c)` triples: March's getters are the same.
- **Constants are mostly qwords.** Read as floats they come out 0 or junk. `june2017_re/charport_consts.py <phase>` lists every constant with its width. Values used here:
  45.0 (`33472998`, `LINE_RADIUS * 1.5`), 30.0, 40.0, 3.0, 0.5, 1.0, 0.2 / 0.7 / 1.2 / 1.1 / 0.36 / 0.23 / 0.4 as March has them.
- Decompiler comparisons: `X < Y != (X == Y)` is `X <= Y`; `(0 < f) == (f == 0)` is "f < 0".
- `CanDown`: June's 11-argument call is March's 4-argument form (7ac, `SERV_IRUHADEV_JUNE_LINE_INDEX16`); the defaults match June's arguments.

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --worktree --files ...`: `files: 0 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`;
`0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
`cl /Zs` of `X2GUArme_VioletMage.cpp` under the US_SERVICE defines: 0 errors; the preprocessed file holds the block (flag live). Build: `DEPLOY OK: X2_offline.exe is 25,518,592 B, dated 2026-10-20 06:27:53, md5 914acf30d7ed78a3b4418d61a413ba51, equal to X2/US_SERVICE/x2.exe; built at main 58e5a759 + 3 uncommitted code file(s).`.

**Open for the review (unmapped, not guessed; the stand-ins keep June's constructor default).** All need Opus:
1. CX2Game `+0xc20` (a byte, cleared by the constructor `32306600 @323077a6`; the charge entry needs it clear, the exit needs it set). No March member: `June_GameByteC20()` returns false.
2. `322cc030`: CX2Game `+0xb38` = float (reader `322f4740 @322f47d2`), called with 2.8 by the MemoRise block. A file-local no-op, `June_SetGameFloatB38`.
3. Arme `+0x9770` (`m_iJune9770`, unsigned-compared with 3 in 9 functions, next to a container at `+0x9760`; only the constructor `32cc99d0` touches it) and `+0x9778`
   (`m_hJuneMemoRiseLoop`, an `EffectSet_AIsha_MemoRise_Loop` handle, constructor sets -1). CH1 must initialise both in the constructor; the owner of `+0x9770` is CH1h / CH1i.
4. vslot `0x718` (`32cc7b30`): `+0x9770 < 3`, state index 157 (charge MP), state time >= 1.5. `June_IsMemoRiseChargeActive` answers false until CH1i, so the MemoRise branch stays off.
5. With `+0x9770 < 3` June's charge state stays on a full MP bar and the wait event enters it (the MP-full test applies only above 2). Whether that is intended depends on item 3; no `JUNE_BUGS` row until then.

No `JUNE_BUGS` row (no June bug found). No drift: every branch found is Aisha's class core. Checklist rows: none of the 40 functions multiplies `m_fNowStatePowerRate` or reads
`USEFUL_VALUE` (§7 l.243, §7h stay with CH1b+). **In game:** nothing to see (the flag adds unreferenced members). Review: `kickoffs/pilot-char-revamp/CH1a-review.md`.

### CH1a -- Review (2026-10-20, Opus)

**Verdict: PASS with one fix.** All 40 functions read against June's decompile, and against the listing where the kickoff named a risk. State ids were checked through
`ch0_state_skill.tsv`, InputData bytes against March's member order (`+0x8722` = `pureLCtr`, after LShift / LAlt / Space), and every compare direction from the
`fnstsw` / `test ah` / `jp` triple. Spot-checked in depth: `32cd0200`, `32cc60a0`, `32cd1830`.
- **Branch structure holds.** `32cc60a0`: the `@32cc62fd` tail runs after every branch, on the ground too, and the turn-around matches (both keys held: a new
  press against the facing). `32cb9ce0`: the `pureUp == false -> USI_DASH_JUMP` test runs after the whole chain. `32cb9a70` / `32cb9ba0`: `0 <= nowSpeed.y`.
  `32cd0200` / `32cd21d0`: the MP-full test only at `+0x9770 >= 3`, the slide show, and the `+0xc20` polarity (entry needs 0, exit needs 1).
- **Compares.** `0.5 <= m_fMPChargeWaitTime`, `fStateTime < 0.36`, `now <= (max - transition) / 3` (= March's `>` with the branches swapped), `now <= max * 0.5`,
  `pos.y - landPos.y < 60.0`, and the `GetJumpSpeed() < nowSpeed.y` cap.
- **Constants.** Re-read from the file bytes: `33472998` = 45.0 (qword), `334411a8` = 0.2f widened (qword), `33457f50` = 60.0, `3343777c` = 0.5 (dword),
  `3351aff0` = 2.8f (the by-pointer argument of `322cc030`). The 0.1 that `charport_consts.py` lists under `32cd1280` belongs to the next function, `32cd1470`.
- **15-key condition** of `32cd07e0`: Up, Down, Right, Left, Z, X, A, S, D, C, Q, W, E, R, LCtr and the animation end. It matches.

**Fixed (under the same flag).** `32cd1830 @32cd217d-32cd2197`: after `PlayEffectSet`, June pushes the MemoRise loop handle into `+0x3ec0` (`32a2dec0` =
`vector::push_back`; `+0x3ec0` is the STATE_END list, funcmap `3227a570`). The port had dropped that push, so the loop would have outlived the charge state. It now
calls `m_vecEffectSetToDeleteOnStateEnd.push_back( m_hJuneMemoRiseLoop )`. Two stand-in comments were reworded, a review rule went into the CH1b kickoff, and 5 funcmap rows
`Char CH1a review` were added.

**Open items, decided:**
1. CX2Game `+0xc20` is **settled**. Only the constructor writes it: the byte, setcc, word and dword encodings at `+0xc20` were scanned over all of `.text`, and the other hits
   are another class or game-unit / NPC objects. `June_GameByteC20()` = false is June's value; nothing is owed.
2. `322cc030` / `+0xb38` -> **CH1i**. The reader `322f4740` is CX2Game's per-frame MemoRise slot timer for the own user: it accumulates elapsed time and makes a 10-slot
   pass at 2.8 s. The 2.8 forces the first pass at 1.5 s into the charge. If the pass draws the HUD, the drawing goes to CH5.
3. `+0x9770` is the **size of a container at `+0x9760`** (MemoRise entries), not a scalar. CH1h constructs it (with `+0x9778` / `+0x977c` = -1). CH1i fills it
   (`32ccf4e0`, `32ced640`, `32cc7f10`, `32ce15a0`; the last is missing from the class map) and replaces `m_iJune9770` with its `size()`.
4. vslot `0x718` (`32cc7b30`) is read from the listing: size < 3, state `AVSI_CHARGE_MP`, Now `fStateTime >= 1.5`. No skill or job gate. Its body is CH1i's.
5. **Not a June bug.** Below 3 entries June lets the charge start and continue on a full MP bar on purpose (`jb` past the MP test). That is the MemoRise mechanic's
   input. No `JUNE_BUGS` row. CH1 wires these events only after CH1i (kickoff note).

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --range ab13f4e0^..ab13f4e0`: `files: 0 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`. Fix, `--worktree --files X2GUArme_VioletMage.cpp .h`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `4 deleted line(s)`, all of them CH1a's own comment lines
inside the flag, reworded (2 stand-in comments, 1 MemoRise comment, the `m_iJune9770` comment); `RESULT: PASS`. Build: `DEPLOY OK: X2_offline.exe is 25,518,592 B, dated 2026-10-20 06:56:10, md5 4b6543895215ee7b47b59d2d71a09316, equal to X2/US_SERVICE/x2.exe (2026-10-20 06:56:02); built at main ab13f4e0 + 2 uncommitted code file(s).`

No drift: the MemoRise container, timer and slot pass are Aisha's class mechanic, owned by CH1h / CH1i (and CH5 for any HUD drawing). **In game:** nothing (inert code).
Next: CH1b (`kickoffs/pilot-char-revamp/CH1b.md`, unchanged apart from the review note).

### CH1b - Done (2026-10-20)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE`, the commit "Character pilot Phase CH1b". The 38 class-map rows of phase CH1b are 38 new `CX2GUArme_VioletMage::June_*`
members (declared after CH1a's in `X2GUArme_VioletMage.h`, bodies in a second block at the end of the `.cpp`, each with its June address), plus two new `ARME_VIOLEMAGE_STATE_ID` members
(`AVSI_TELEPORT_STAND_UP_START_UPGRADE`, `AVSI_TELEPORT_STAND_UP_ATTACK_UPGRADE`, June ids 242 / 243, appended after `AVSI_TRANSFORMED`) and 3 file-local stand-ins. Nothing registers or calls them.
41 funcmap rows `Char CH1b` (38 ported, 3 stand-ins). One `JUNE_BUGS.md` row (371).

| June | Member (`June_` prefix) | June | Member |
|---|---|---|---|
| `32cb6ed0` / `32cb6ef0` | DieFrontFrameMove / EventProcess | `32cb6f20` / `32cb6f40` | DieBackFrameMove / EventProcess |
| `32cb98a0` / `32cb9900` | DieFrontStartFuture / Start | `32cb9970` / `32cb99d0` | DieBackStartFuture / Start |
| `32cb7170` | DamageAirDownLandingInit | `32cb8a50` | RidingHyperModeInit |
| `32cb9d90` / `32cb9de0` | DamageDownFront / BackFrameMove | `32cc5ae0` | IsTeleportUpgradeAcquired (helper) |
| `32cc6520` / `32cc66a0` | DamageDownFront / BackEventProcess | `32cc6820` | DamageAirDownLandingEventProcess |
| `32cc9160` | AVSI_TELEPORT_STAND_UP_START_Start | `32cc1cd0` | AdaptAdvancedTeleportationBuff (helper) |
| `32ccc5a0` | AVSI_TELEPORT_STAND_UP_START_FrameMoveFuture | `32cb7210` | AVSI_TELEPORT_STAND_UP_START_EventProcess |
| `32cd2740` | EvasionEndFrameMove | `32cb9e30` | EvasionEndEventProcess |
| `32cb7240` / `32cb7270` | EvasionStandupAttackStart / End | `32cd2870` / `32cb9e90` | EvasionStandupAttackFrameMove / EventProcess |
| `32cb7530` | DashJumpAttackZStartFuture | `32cc6ca0` / `32cd5360` | DashJumpAttackZFrameMoveFuture / FrameMove |
| `32cbb8d0` / `32cbba10` | ComboX4ChargeStateStart / End | `32cd1470` / `32ce1590` | HyperModeCameraMove (helper) / RidingHyperModeCameraMove |
| `32ce12c0` | CommonHyperModeFrameMove (helper) | `32ce6f20` | RidingHyperModeFrameMove |
| `32ccd4a0` | ApplyLimitedManaManagement (helper) | `32cf0f90` / `32cf0fa0` | RidingOffStart / RidingHyperModeEnd (bare jumps to vslots `0x60c` / `0x608`) |

**What differs from March** (the rest is March's body): the three damage-down event processes pick the new `_UPGRADE` stand-up states by `32cc5ae0` (skill 22216 acquired) and draw an ignored `GetRandomInt( 0 )` in the
`[4]` branch; the teleport stand-up start adds a 1.1 s super-armor window and `32cc1cd0`, then follows to `AVSI_TELEPORT_STAND_UP_END`; Air Blast's centre sequence comes from the Major particle system (March: the minor one);
the Air Blast / Evasion meshes are tagged for the effect-alpha option (`JuneTagEffectAlphaMesh`, checklist 7c, `c10a_alpha_sites.tsv` `32cd2853` / `32cd29bf`); `CommonHyperModeFrameMove` carries the June-only
awakening effect-set calls and a mana chance of 0.25 / 0.5 / 1.0 (March `0.33 * count`), now its own function `32ccd4a0`.

**Decoded by structure, not by a table** (worth one check in the review): `+0x2480` / `+0x1730` are `m_ExtraDamagePack.m_EnchantShock` / `m_Frozen` times (order of March's test, offsets differ by 0xd50 = 12 entries);
the `0x4f` / `0x15` ability types of `32206e50` are `SA_RECOVER_MP_ABS` / `SA_MAX_MP_ABS` (`s0a_templet_keys.tsv`); `322ceaa0` / `322ceab0` / `322cead0` are `g_pData +0x84 / +0x88 / +0x98` (major / minor particle, minor mesh player).

**Stand-ins (need Opus; June's absent-value default, announced nowhere because nothing calls them yet):** `32011d20`, `31fd66b0`, `32011f50` (the June-only awakening effect-set family: unit `+0x9578` map, handle `+0x9f4`,
a CX2Unit `+0xa48` lookup). No March member (funcmap `32296cb0`, `32298590`: "ported with its host"). They are file-local no-ops, `31fd66b0` answers false, so the March trace shows. `June_Vslot60c` / `June_Vslot608` are
empty until CH1i writes the overrides `32cb8b60` / `32ccda40`.

**Drift (scope rule):** the awakening effect-set family is not Aisha's alone (every class's `CommonHyperModeFrameMove` / `ApplyHyperModeBuff` calls it). Identified, not planned here: June `32011d20` / `32011f50` /
`31fd66b0` / `3204b5e0` / `32298590`, March counterpart none, offline reach = every awakening; owner: Stage 6 (bullet "Awakening effect-set family" added to `JUNE_PORT_BACKLOG_ORDER.md`). CH1 needs only the stand-ins.

**Registration notes for CH1:** the `_UPGRADE` states 242 / 243 and `AVSI_TELEPORT_START_UPGRADE` (CH2a) are June-only; June shares `32cb7240` / `32cb7270` / `32cbb8d0` / `32cbba10` across 12 / 11 / 59 states (ch0_state_skill.tsv), and
registers the Die `FrameMove` for `AVSI_RIDING_ATTACK_Z` / `AVSI_RIDING_JUMP_ATTACK_Z` (JUNE_BUGS 371).

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --worktree --files X2Lib/X2GUArme_VioletMage.cpp X2Lib/X2GUArme_VioletMage.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Build (the first run failed on the enum members only: my spec repeated an `@@ file` section for the header and the first section was dropped; 11 `C2065`, nothing else): `DEPLOY OK: X2_offline.exe is 25,527,296 B, dated 2026-10-20 07:37:35, md5 137df3179a550733b26a02c8b5d38a19, equal to X2/US_SERVICE/x2.exe (2026-10-20 07:37:28); built at main dd0c90dc + 2 uncommitted code file(s).`

**In game:** nothing (the flag adds unreferenced members). Review: `kickoffs/pilot-char-revamp/CH1b-review.md`. Next: CH1c (`kickoffs/pilot-char-revamp/CH1c.md`) after the review.

### CH1b -- Review (2026-10-20, Opus)

**Verdict: PASS, no code fix.** All 38 functions were read against June's decompile (dumps of the 38 + the 3 stand-ins + the registrar), and against the
listing where the kickoff named a risk. Compare directions were read from the `fnstsw` / `test ah` / `jp` triples. Constants are from `charport_consts.py CH1b`.
Spot-checked in depth: `32cc6520`, `32cd5360`, `32ccc5a0`. The front / back twins (`32cb6ed0`/`6f20`, `6ef0`/`6f40`, `98a0`/`9970`, `9900`/`99d0`) are identical
apart from their constants, and `32cc66a0` = `32cc6520` with `0x21`.
- **Branch structure.** `32cc6520` / `32cc66a0`: the order is `[4]` (`+0x7d89`: vslot `0x1c8( 0 )`, result dropped `@32cc6577`, then the attack choice), `[1]`, `[2]`, `[3]`,
  then STANDUP. The `undefined2` stores at `+0x7d87` / `+0x7d86` / `+0x1f62*4` clear the same flag pairs as the port. `32cc6820` inlines `32cc5ae0` in `[1]`, with Z or X.
- **`32ccc5a0`.** `+0x2480` / `+0x1730` are `m_EnchantShock` / `m_Frozen`: `m_Frozen` + 12 entries x `0x11c` (the ExtraDamageData stride, funcmap `32273480`)
  = `m_EnchantShock`, and 2 more = `m_StigmaDebuff` `+0x26b8`. Then the push / pass bools, MOVE_TIME (default 0), `+45`, the loop `(int)( t * 85 )` re-evaluated,
  `700`, `+100`, `[0]`. As ported.
- **`32cd5360`.** The ring is handle `+0x9828` into `322ceab0` (minor), the centre `+0x97c0` into `322ceaa0` (major). With March's arrays (28 major, then 5 minor;
  base `+0x97b8`) these are `AIR_BLAST_RING` and `AIR_BLAST_CENTER` (funcmap row). The window is `0.28 < t < 0.7` and `+0x9599`.
- **Constants and compares.** `32cc6ca0`: `0.26 <= t <= 0.4` -> stop, else the `0.41` oneshot (qword compare, dword timer). `0.66 < t` in the three event
  processes. `32ccd4a0`: 0.25 / 0.5 / 1.0 by vslot `0xb0`, `rand <= rate`, SA `0x4f` / `0x15`, text offsets +50 / +30. `32ce12c0`: world colour 0.1333 = `0xff222222`,
  `UpDownCrashCamera( 20, 0.3 )`, `ClearScreen` inlined.
- **One comment slip** (no code effect, CH0c fixes it in its comment pass): the block header gives `GetJumpSpeed()` as `+0x4978`. June reads `+0x4a78` (`param_1[0x129e]`).

**Decisions:**
1. Awakening effect-set stand-ins: **owner stays Stage 6** (the backlog bullet). `32011d20` plays a set named by the item equipped at position 15 (CX2Unit `+0xa48`; an
   `_Add` suffix for unit type 9) into the map `+0x9578`. With no such item the map stays empty, `31fd66b0` is false and the trace shows: that is June's own path,
   so the stand-ins are exact for an unequipped character. Not Aisha's, and play does not need it.
2. `June_Vslot60c` / `June_Vslot608` -> CH1i writes `32cb8b60` / `32ccda40`. CH1 points `June_RidingOffStart` / `June_RidingHyperModeEnd` at them.
3. **Confirmed.** `32cc5ae0 @32cc5ae0-32cc5afc`: `+0x867c` -> `lea ecx, [eax + 0x494]` -> `328914b0( 0x56c8, 0 )`, the acquired-map find at tree `+0x4a8`. The same
   reading as `JuneIsSkillAcquired`.
4. The `_UPGRADE` ids stay symbolic. CH1 decides the numbering when it registers them (CH2a registers the states).
5. **JUNE_BUGS 371 withdrawn: a CH0 mapping error.** The registrar calls `UserUnitStateData::Init` `320775a0` between `AVSI_RIDING_DIE` and `AVSI_RIDING_ATTACK_Z`
   (`@32ce9b61`), and `32cf0ab0` occurs twice in the whole image (`AVSI_DIE_FRONT`, `AVSI_RIDING_DIE`). The riding attacks register no FrameMove.

**Found: `charport_states.py` carried callbacks across states.** See the funcmap row `32ce77b0 @32ce9b5d-32ce9ba4`. The tool now clears its slot table on the `Init`
call (docstring updated). The `~` per-class rows keep a second defect, flagged in the tool. The fix and the map rebuild are **Phase CH0c** (the scope rule: CH0's own
deliverable, so it stays in this plan). Effect on CH1a / CH1b: the code is right, the function sets do not change, but the registration claims written from the map are
wrong. In the CH1b block (sets from the fixed plain rows; CH0c adds any `~` row): `32cb6ed0` (die states only), `32cb7170` (`AVSI_DAMAGE_AIR_DOWN_LANDING` only, not 10 states), `32cb9de0` (`DAMAGE_DOWN_BACK` only), `32cd2740`
(`A_AV_TELEPORT_END` / `TELEPORT_STAND_UP_END`), `32cc9160` / `32ccc5a0` (the two `STAND_UP_START` states), `32cb7240` / `32cb7270` (the two `STANDUP_ATTACK` states, not 12 / 11),
`32cb7530` / `32cc6ca0` / `32cd5360` (`DASH_JUMP_ATTACK_Z` only), `32cbb8d0` / `32cbba10` (`COMBO_X4_CHARGE` only, not 59), and the riding hyper set
(`AVSI_RIDING_HYPER_MODE` only). The CH1b Done note's "Registration notes for CH1" are superseded the same way.

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --range dee57cb6^..dee57cb6`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`. No code changed in the review, so no build (the deployed exe is CH1b's). **In game:** nothing (inert code). Next: CH0c (`kickoffs/pilot-char-revamp/CH0c.md`),
then CH1c (its kickoff now depends on CH0c).

### CH1c - Done (2026-10-20)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE`, the commit "Character pilot Phase CH1c". The 30 class-map rows of phase CH1c are 30 new `CX2GUArme_VioletMage::June_*` members
(declared after CH1b's in `X2GUArme_VioletMage.h`, bodies in a third block at the end of the `.cpp`, each with its June address), one new `ARME_VIOLEMAGE_STATE_ID` member
(`AVSI_AEM_ZZX`, June id 233, after the CH1b `_UPGRADE` pair) and two June-only members (`m_pJuneMpRefund`, `m_iJuneBallElement`). Nothing registers or calls them. 32 funcmap rows `Char CH1c`.

| June | Member (`June_` prefix) | June | Member |
|---|---|---|---|
| `32cb72b0` / `32cb9ee0` | ComboZ1FrameMove / EventProcess | `32cba3e0` | ComboZ2EventProcess |
| `32cba970` / `32cbaa50` | ComboZ3 / Z3up EventProcess | `32cb72c0` / `32cb7310` | ComboZ4upStart / FrameMoveFuture |
| `32cd2b10` / `32cbabf0` | ComboZ4upFrameMove / EventProcess | `32ce5780` / `32cbacd0` | ComboX1FrameMove / EventProcess |
| `32ce5970` / `32cbafb0` | ComboX2FrameMove / EventProcess (**not carved in Ghidra: read from the listing**) | `32ce5ac0` / `32cbb520` | ComboX3FrameMove / EventProcess |
| `32cb7320` | ComboX3StateEnd | `32cb7340` / `32cbb8f0` | ComboX4ChargeFrameMove / EventProcess |
| `32ce5c10` / `32cbba30` | ComboX4ChargeFireFrameMove / EventProcess | `32ce5dc0` / `32cbbc40` | ComboX4FrameMove / EventProcess |
| `32cb73a0` / `32cb73f0` | ComboX3frontStart / FrameMoveFuture | `32cd3370` / `32cbbe50` | ComboX3frontFrameMove / EventProcess |
| `32cbbf40` / `32cbc3e0` | ComboX3downFrameMove / EventProcess | `32cc2b90` | FlushMpWithRefund (helper) |
| `32ce4180` | CreateComboFireBall (helper) | | |

**What differs from March** (the rest is March's body): every combo `EventProcess` is June's data-driven cancel template (March: fixed times) - the `m_fSkillCancelAfter` / `BWalk` / `Walk` / `Dash` / `AttackCancelAfter` / `UpDown`
members, the four `m_fEventTime[ i ]` input windows and `bEventFlagList[ 0..3 ]` - with new hand-offs: Z2 -> Z3up / AVP_ZZfrontZ / Z3 / AEM_ZZX, X1 -> AEM_XZ, X2 -> X3down / X3front / X3, X3 -> X4_CHARGE (if `m_bCanChangeState`
and not transformed) / X4, X3down -> ADW_COMBO_ADD_X. The Z4up / X3front `FrameMoveFuture` lose March's speed-zero window. Fireballs: X1-X4 spend MP through `32cc2b90` (a chance refund of the MP just spent,
from the passive object at `+0x96ac`) and are created by `32ce4180` (Battle Magician / Dimension Witch magic-ball names, the ice / energy element at `+0x9630`, the gravity ball, the Magical Makeup damage
bonus); timers X3 0.382 s (March 0.5), X4 / X4-fire 0.033 s (March 0.166); the charged ball no longer scales its power by the charge (the scale only). `Z4up` / `X3front` / `X3down` `FrameMove` equal March's.

**Decoded by structure, not by a table** (one check in the review): June `+0x8c10` is `m_fEventTime[ 0 ]` (March `EventProcessTime`, 12 bytes: `keyInputStart`, `keyInputEnd`, `stateChange`; ten entries end at `m_fAttackCancelAfterEx` `+0x8c88`);
`+0x7d85..+0x7d88` are `bEventFlagList[ 0..3 ]` (CH1b's reading); `+0x8d4c` is the awakening-by-key flag of the `SKILL_CANCEL_AFTER` expansion (March macro, AWAKE_LOCK form, used as is); `+0x95bc` is `m_fMPConsumeRateFireBall`
(a protected float in June, `+0x95c8` `m_fFireBallRangeRate`); `+0x9634 / +0x963c / +0x9640` are `m_fIceBallPower / m_bResearchGravityBall / m_fGravityBallPower` (June applies `m_fIceBallPower` to every element);
`+0x8e90 ? +0x8e84 : +0x87d0` is Luciel's second body (checklist 7o), false for Aisha. Skill ids 22022 / 22040 are `RNSI_SA_ABM_MAGICAL_MAKEUP` (+ `_UPGRADE`); the ability type `0xf` is March's `SA_DAMAGE_REL`.

**Stand-ins (June layout owned by CH4 / CH1; need Opus if the review doubts them):**
1. `m_pJuneMpRefund` (`JuneMpRefundData`: `m_fChance` = object `+0x10`, `m_fRate` = the protected float at `+0x20`): the passive object `InitPassiveSkillState` (`32cecd70`, CH4) builds with `32cf2190` for skills 22405 / 22415.
   The constructor must set it to NULL (June `32cc99d0 @32cca662`), the destructor deletes it (`32ce4180 @32ce4647` is inside the destructor that follows the function). Until CH4 it is NULL, so no refund.
2. `m_iJuneBallElement` (`+0x9630`, constructor 0; March's `m_bIceBall` is its 0 / 1 case): CH4 sets it (`32ced3dc` 0, `32ced402` 2, the ice case in the same function).
3. `IsPvpOrPve` is false (the offline client has no PvP); the class tests read `GetUnit()->GetClass()` where June reads `[ templet +0xaf0 ]->[ 4 ]` (the same field, plus a NULL test).

**Registration notes for CH1:** state ids are shared by the job variants (0xaa is `AVSI_COMBO_X3front` and the Elemental Master's `AVSI_AEM_XXfrontX`; 0xab the High / Dark Magician `X3down`); the transcription names the base state. CH1 decides the numbering; `AVSI_AEM_ZZX` (CH1e writes
its functions) is already an enum member. The states these functions are registered for are in `ch0_state_skill.tsv` (`phase` CH1c).

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --worktree --files X2Lib/X2GUArme_VioletMage.cpp X2Lib/X2GUArme_VioletMage.h`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `cl /Zs` of the class file under the US_SERVICE defines: 0 errors.
Build (before the comment trim below): `DEPLOY OK: X2_offline.exe is 25,547,264 B, dated 2026-10-20 09:32:50, md5 528cf7d5eb9eb70262ce9e8573ea6fab, equal to X2/US_SERVICE/x2.exe (2026-10-20 09:32:42); built at main 03f25348 + 2 uncommitted code file(s).`
The source comments were then cut to one line each (CLAUDE.md, *Rule: a code comment is one short line*); the code lines are identical (checked line by line), `flagcheck` and `cl /Zs` re-run clean, so the deployed exe is still this phase's code.

**Decoding tables (moved out of the source):**
- Key bytes (March `m_InputData` order): pureLeft `+0x86da`, pureDoubleLeft `+0x86dc`, pureRight `+0x86de`, pureDoubleRight `+0x86e0`, pureUp `+0x86e2`, pureDown `+0x86e6`, pureZ `+0x86ea` / oneZ `+0x86eb`, pureX `+0x86ee` / oneX `+0x86ef`,
  pureC `+0x86f2`, pureA `+0x86f6`, pureS `+0x86fa`, pureD `+0x86fe`, pureQ `+0x8706`, pureW `+0x870a`, pureE `+0x870e`, pureR `+0x8712`, oneLCtr `+0x8723`.
- State ids: `0xd` WAIT, `0x11` JUMP_DOWN, `0xa2` Z1, `0xa3` Z2, `0xa4` Z3, `0xa5` Z3up, `0xa6` Z4up, `0xa7` X1, `0xa8` X2, `0xa9` X3, `0xaa` X3front, `0xab` X3down, `0xd3` X4, `0xd6` X4_CHARGE, `0xd7` X4_CHARGE_FIRE, `0xc6` AEM_XZ, `0xc8` AVP_ZZfrontZ, `0xd4` ADW_COMBO_ADD_X, `0xe9` AEM_ZZX.
- Class ids: HM `0xe`, DM `0xf`, BM `0x17`, AEM `0x68`, AVP `0x69`, ADW `0x71`. Vslots: `0x5c` StateChange, `0x668` StateChangeDashIfPossible, `0x7f4( 1 )` SpecialAttackEventProcess( ASUT_GROUND ), `0x560` CanHyperMode, `0x6b8` GetHypetMpdeStateID,
  `0x4b0` / `0x4b4` GetComboZ / XStateID, `0xe4` FlushMp, `0xf0` UpNowMp, `0x1cc` GetRandomFloat, `0x204` GetPowerRate, `0x150` GetRotateDegree, `0x16c` GetDirVector, `0x170` GetIsRight, `0x7ec` CreateStepDust, `0x7c4`..`0x7dc` CommonState* / CommonFrameMove*.

No `JUNE_BUGS` row (Z3's animation-end-first order is March's too; the other oddities are June changes, not mistakes). No drift: the refund passive, the element ball and the combo windows are Aisha's class mechanics (CH4 / CH1). Checklist row 396 (UPDOWN branches in class states): Z1 carries it;
the row is annotated when CH1 closes the combo set. **In game:** nothing (the flag adds unreferenced members). Review: `kickoffs/pilot-char-revamp/CH1c-review.md`. Next: CH1d after the review.

### CH1c -- Review (2026-10-20, Opus)

**Verdict: PASS, no code fix.** All 30 functions were read against June: 28 Ghidra decompiles, plus the listings of `32cbafb0` / `32ce5970` (`funcdis.py --func`). Compare directions were read
from the `fnstsw` / `test ah` / `jp` triples where the decompile hides them (X2 EventProcess / FrameMove, `32cc2b90`). Constants are from `charport_consts.py CH1c`. Spot-checked in depth: `32cba3e0` (Z2), `32cbafb0` (X2), `32cbacd0` / `32ce5780` (X1).
- **Cancel template.** The else-if order matches June in every EventProcess: Z1 is UPDOWN, SKILL, BWALK, WALK, DASH, Z, X, flag, window, last key. X2 / X3 are the flags, then WALK, SKILL, DASH, anim end.
  `SKILL_CANCEL_AFTER` (AWAKE_LOCK form) is June's test set exactly: `0x7f4( 1 )` with on-ground, then black hole `322731a0`, oneLCtr, count > 0, `0x560`, time, `32070d60`, then `+0x8d4c = 1`.
  The flag consume reads `!( t <= stateChange && !end )`; the window `start <= t < end`. Last-key times: Z1 `0.5` -> `0.2` / `0.1`, Z2 `0.6` / `0.2`, Z3 `0.65`, Z3up `0.7`, Z4up / X3front `1.4` (14 keys).
- **Indices.** `m_fEventTime[ i ]` = `+0x8c10 + 12 i` (X2 reads `[2]` at `+0x8c28` / `2c` / `30`); `+0x8c0c` = `m_bIsTransformed` (the member before `m_fEventTime`, `X2GUUser.h:4577`); `+0x8c98` / `+0x8c9c` = Walk / Dash.
  `+0x96f0` = `m_bCanChangeState` (X2 tail, X3 window, X3 End). `+0x96f4` = `m_fPushXChargeTime`. `+0x80c4` = `landPosition.y` (it ends at `bHit` `+0x80cc`). State and class ids are as tabled.
- **`32ce4180`.** The name table, the type 1 -> 7 fall-through, types 5 / 6, the empty-name test and every string (the three truncated in Ghidra were read in full) match. Power is `GetPowerRate() x ( +0x9634 | +0x9640 gravity )`,
  and with Magical Makeup it is x `32007c40( 0xf, 22040 )` behind `31fcf3e0( 22022 )` (= `IsUpgradedSkillLearned`). `CreateInstance`'s trailing arguments are all March's defaults; June's `fLifeTimeRate` slot carries `+0x95c8`.
- **`32cc2b90`.** The roll is `rand < +0x10` (`fcompp`, `test ah, 0x41`), the amount is protected `+0x20` x the argument (`fmul [esp+0xc]`). `32ced2a3-32ced2c3`: `32007b10( 5, 0x578f )` -> `+0x10`,
  `32007b10( 0x42, 0x578f )` -> `+0x20` (skill 22415). **One gap, routed to CH4:** June calls `32278070` (UpNowMp, then `amount + 0.005` onto the unit's MP-text queue `+0x68ac`, June's "MP +N" text),
  not bare vslot `0xf0`. The port calls `UpNowMp`, so no MP text appears when the refund fires. This is inert until CH4 gives the object its data. The queue is the unported MP half of `3229ecc0`
  (`JuneFrameMoveHpChangeText` names it). CH4 ports it (kickoff "From CH1c review"); funcmap row `32278070`.
- **Constants** all match: 0.3 / 0.53 / 0.382 / 0.033 / 0.35 / 0.47 / 0.82 / 0.83 / 0.71, MP x4 (X1, X2) and x6 (X3, X4, X4 fire), X1's `100` offset, scale `x 0.5 + 1`, and the 20 / 50 / 20 offsets in X3front.
  June's X4-fire also calls `0x204` and drops the result; it is a getter, so it is not ported.

**Decisions:**
1. **Confirmed:** `GetUnit()->GetClass()` for the class tests. June reads the class two ways: `[ unit +0xaf0 ]->[ 4 ]` (`m_pUnitTemplet->m_UnitClass`, which is exactly March's `GetClass`) and unit `+0x38` (Z2 AVP, X1 AEM, X3down ADW).
   For the player's own unit both hold the same class. June does not NULL-test the unit; the port does, which is harmless.
2. **Confirmed:** `m_pXSkinAnimFuture` for June's `+0x8e90 ? +0x8e84 : +0x87d0` (Luciel's second body, checklist 7o). June's direct `+0x87d0` reads are the same object.

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --range 0bb5cd02^..0bb5cd02`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
No code changed in the review, so there was no build; the deployed exe is CH1c's. **In game:** nothing (inert code). Next: CH1d (`kickoffs/pilot-char-revamp/CH1d.md`).

### CH1d - Done (2026-10-20, Sonnet 5.5)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE`, the commit "Character pilot Phase CH1d". **25 of the 26 class-map rows are new `CX2GUArme_VioletMage::June_*` members** (declared after CH1c's in the header,
bodies in a fourth block at the end of the `.cpp`), plus two enum members. Nothing registers or calls them. **One row is not transcribed and needs Opus (below).** 28 funcmap rows `Char CH1d`.

| June | Member (`June_` prefix) | June | Member |
|---|---|---|---|
| `32cbd760` / `32cb7490` / `32cbd7b0` | DashComboZ1 StartFuture / FrameMove / EventProcess | `32cc6980` / `32cbdc40` | DashComboZ2 StartFuture / EventProcess |
| `32cd4df0` / `32cbdd20` | DashComboZ2a FrameMove / EventProcess | `32cc6a50` / `32ce61f0` | DashAttackX StartFuture / FrameMove |
| `32cbe090` / `32cc4180` | DashAttackX EventProcess / `AVP_DashAttackXEventProcess` (the Void Princess re-registration) | `32cb74a0` | DashAttackX EndFuture |
| `32cc6b70` / `32cb7520` / `32cbe2f0` | JumpAttackZ FrameMoveFuture / FrameMove / EventProcess | `32cc6c00` / `32ce6660` / `32cbe330` | JumpAttackX FrameMoveFuture / FrameMove / EventProcess |
| `32cbe360` | DashJumpAttackZ EventProcess | `32cb7550` / `32ce6830` / `32cbe480` | DashJumpAttackX FrameMoveFuture / FrameMove / EventProcess |
| `32cb7560` / `32ce69b0` / `32cbe510` | DashJumpAttackX2 FrameMoveFuture / FrameMove / EventProcess | `32cd4830` | **DashComboZ2aStartFuture: declared only** |

**Not transcribed - needs Opus: `32cd4830` (`AVSI_DASH_COMBO_Z2a` StartFuture).** June rebuilt the teleport target pick: the enemy loop (not invincible, hp > 0, line enabled and `LT_NORMAL`) now also runs an 11-argument `CanDown` at the enemy's position, `GetDistance < 700`,
`32293b80` (pos along my last touched line, ported as `JuneIsPosAlongLastTouchLine`) against `GetIsRight`, and keeps the best by distance; the landing point then comes from **`32293f80`, a CX2GameUnit method that funcmap does not map** (copies `in` to `out`; with a line map and a
vslot `0x80` test it walks every enabled line of type other than 7 / 1 for the nearest point, else `out = GetPos( false )`; 8 other callers). Read both in Ghidra (`decompile_function` at `0x32cd4830` and `0x32293f80`). The review (Opus) maps `32293f80`, transcribes the function whole and writes the body; the
declaration `June_DashComboZ2aStartFuture` is already in the header. Rule: no partial update and no guess, so no stand-in body was written.

**What differs from March** (the rest is March's body):
- Z1 `EventProcess` is the m_fEventTime cancel chain (four windows, flags 0..3): Z -> Z2; X -> ABM / ADW Z2a (`0xcc`), EM Z2a (`0xdc`), any other class Z2a (`0xb0`, flag 3); March used fixed 0.46 s. Z2 ends on animation end or any ZXC key after 0.55 s (March: end first, 0.55 on keys).
- `DashAttackX` has two registered EventProcess: `32cbe090` (end only) and the Void Princess `32cc4180` (X in the `m_fEventTime[ 0 ]` window -> `0xb3` `AVSI_DASH_ATTACK_X2`). The jump / dash-jump attack events are March's, except the new hand-offs below.
- `DashJumpAttackZ` `EventProcess` (`32cbe360`): X inside the window, once the Z blast was spent (`m_bDashJumpZ`, June `+0x9599`) and the class is not `UC_ARME_VIOLET_MAGE`, chains to the job's dash-jump ZX (`0xe4`). `DashJumpAttackX2` `EventProcess` (`32cbe510`): the Elemental Master's X chains to `0xe8`.
- The four fireballs (`32ce61f0`, `32ce6660`, `32ce6830`, `32ce69b0`) spend `4.0 x m_fMPConsumeRateFireBall` through `June_FlushMpWithRefund` and create types 0 (+180 on y), 1, 0 (z = -30) and 7 (z = -30) through `June_CreateComboFireBall`. The failure effects are March's (`CreateNotEnoughMPEffect`; `MagicFail` for JumpAttackX).
- `DashJumpAttackX` / `X2` `FrameMoveFuture` are bare jumps to `CommonFrameMoveFuture`: June dropped March's speed-zero window (0.13-0.27 s). `DashAttackX`'s `EndFuture` is March's.
- `32cd4df0` (Z2a `FrameMove`): the ArmeEvasion mesh is tagged with the effect-alpha category (`JuneTagEffectAlphaMesh`, checklist 7c, `32296ee0 @32cd4f03`, under `SERV_IRUHADEV_JUNE_EFFECT_ALPHA_OPTION`).

**Enum:** two June-only ids added after `AVSI_AEM_ZZX` - `AVSI_AHM_DASH_JUMP_ZX` (June 228 / `0xe4`, shared by the ABM / ADM / AHM `DASH_JUMP_ZX` states) and `AVSI_AEM_DASH_JUMP_X3` (232 / `0xe8`). CH1e's `32cb8bd0` (`AEM_DASH_JUMP_X3`) must not add the second one again. Other ids use March's names:
`0xaf` `AVSI_DASH_COMBO_Z2`, `0xb0` `AVSI_DASH_COMBO_Z2a`, `0xb3` `AVSI_DASH_ATTACK_X2`, `0xb8` `AVSI_DASH_JUMP_ATTACK_X2`, `0xcc` `AVSI_ABM_DASH_COMBO_Z2a`, `0xdc` `AVSI_EM_DASH_COMBO_Z2a`, `0x12` / `0x16` `USI_JUMP_LANDING` / `USI_DASH_JUMP_LANDING`.
Decoded: `m_fEventTime[ i ]` = start / end / stateChange at `+0x8c10 + 12 i`; `32070cf0` is the future-animation accessor (`+0x8e90 ? +0x8e84 : +0x87d0`), read as `m_pXSkinAnimFuture`; `+0x8c0c` is `m_bIsTransformed`.

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --worktree --files X2Lib/X2GUArme_VioletMage.cpp X2Lib/X2GUArme_VioletMage.h`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `cl /Zs` of the class file under the US_SERVICE defines: 0 errors.
Build: `DEPLOY OK: X2_offline.exe is 25,553,408 B, dated 2026-10-20 10:12:51, md5 b52ea1fdf30d016a7ceb9d1af3041ae3, equal to X2/US_SERVICE/x2.exe (2026-10-20 10:12:44); built at main 5877e338 + 2 uncommitted code file(s).`

No `JUNE_BUGS` row (the dropped speed-zero window and the `m_bDashJumpZ` gate are June changes, not mistakes; the `MagicFail` sequence is not NULL-tested in March either). No drift: every branch found is Aisha's class core. **In game:** nothing (the flag adds unreferenced members).
Review: `kickoffs/pilot-char-revamp/CH1d-review.md` (it also owns `32cd4830`). Next: CH1e after the review.

### CH1d -- Review (2026-10-20, Opus)

**Verdict: PASS, no transcription fix; the review wrote the 26th function.** All 25 were read against June: Ghidra decompiles, plus listings (`funcdis.py`) of `32cb7550` / `32cb7560` / `32cc6a50`, the four fireballs and `32cb74a0`.
Spot-checked in depth: `32cbd7b0` (Z1 EventProcess), `32ce61f0` (DashAttackX FrameMove), `32cd4830`.
- **Z1 EventProcess.** The chain matches: flag 0 -> `0xaf`, window 0 (oneZ, on something); flag 1 -> `0xcc`, window 1 (BM `0x17` / ADW `0x71`); flag 2 -> `0xdc` (AEM `0x68`); flag 3 -> `0xb0`; then the 12-key `0.65` rule; then the animation-end tail. Key offsets: `+0x86da` left ... `+0x8712` R, so `_C` is 12 keys and `_ZXC` is 14.
- **`32cbe360` / `32cbe480` / `32cbe510` / `32cc4180`.** Order, gates (`+0x9599`, class `!= 2` with a NULL templet passing, `0x68`), ids `0xe4` / `0xb8` / `0xe8` / `0xb3`, and landing `0x16` all match. `32cbe480`'s `0.4` is a double (`(double)0.4f`).
- **Fireballs.** Type slot `0` / `1` / `0` / `7`; `32ce61f0` adds 180 to `angle.y`; `32ce6830` / `32ce69b0` *set* `angle.z = -30`; the failures are `(0, 180, 0)` and `(0, 0, -30)`; bone `Dummy2_Lhand` (`3345bb08`); cost `4.0 x +0x95bc`.
- **`32cb74a0`.** The decompile drops the `+0x7e08 += 180.0` (`fadd [33438080]`); the listing has it, so the port's March body is right.
- **Speeds, constants.** Walk / dash speeds, the `0.13` / `0.26` window and the two bare `CommonFrameMoveFuture` jumps match. All 13 constants (`charport_consts.py CH1d`) match. The alpha tag is `32296ee0` after `CreateInstance`.
- **Enum.** `AVSI_AHM_DASH_JUMP_ZX` for the shared id 228 is fine: June's script lists AHM first, ADM / ABM reuse 228, and CH1 numbers the ids.

**`32cd4830`, written** (`June_DashComboZ2aStartFuture`; funcmap rows `32cd4830`, `32293f80`, `3275bc30`):
- Unit loop. It skips my team, `GetInvincible( AT_COMMAND )` (slot 94), hp <= 0, `GetIsBosRaidNPC` (slot `0x3c8`), and a last line that is not enabled `LT_NORMAL`.
- Landing point. `land = target->JuneGetBoneLinePos( my pos )`, then the 11-argument `CanDown( land, 30, NULL x3, false, false, true, false )`, `GetDistance < 700`, and `along = ( JuneIsPosAlongLastTouchLine( land ) == GetIsRight() )`. The pick rule is `@32cd4ae2-32cd4b2d`, verbatim.
- After the loop. A non-facing best flips Future `bIsRight`. The position is the target's, with `y = line startPos.y + 20`. With no target there is no lift (March added 350), and the `FIX_AISHA_TELEPORT_POSITON` test is gone. Then 4 push / pass steps at `-700`.
- New callees under the same flag:
  - `CX2GameUnit::JuneGetBoneLinePos` (`32293f80`, `X2GameUnit.cpp`): the target's nearest collision bone (`JuneGetNearestCollisionFrameName`, `GetBonePos`), moved in x / z to the nearest point of an enabled line that is not `LT_WALL` / `LT_MONSTER_ROAD`. None of its 8 other callers is ported.
  - `CX2GameUnit::JuneIsPosAlongLastTouchLine`, a member over the file's existing `32293b80` static.
  - `CX2GUNPC::JuneGetInvincibleByAttackTypeId` (slot 94 `3275bc30`; the user's slot 94 is plain `GetInvincible`).
- **JUNE_BUGS 374:** when no line qualifies, `32293f80` reads an uninitialised point. C++ cannot copy that read, so the port starts the point at the bone position.
- `#error` guard: the body needs `JUNE_DAMAGEDATA`, `JUNE_LINEMAP_CANDOWN_SIGNATURE`, `ELYSION_INVINCIBLE_ATTACK_TYPE` and `JUNE_BUFF_ENUM`. All four resolve ON (`flagprobe.py`, plus `FIELD_BOSS_RAID`).

**Hygiene.** CH1d commit, `flagcheck.py JUNE_CHAR_ARME_CORE --range 699c3f84^..699c3f84`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Review edits, `--worktree` over the 5 code files: `files: 5 PASS, ... 0 FAIL, 0 ERROR`; `2 deleted line(s)`, which are CH1d's own two placeholder comments; `RESULT: PASS`.
Build: `DEPLOY OK: X2_offline.exe is 25,555,968 B, dated 2026-10-20 10:37:25, md5 4a9e6e97aca08838eb4f83cef7b6bbc9, equal to X2/US_SERVICE/x2.exe (2026-10-20 10:37:18); built at main 699c3f84 + 5 uncommitted code file(s).`
No drift: every branch is Aisha's class core or an engine callee it needs. **In game:** nothing (inert). Next: CH1e (`kickoffs/pilot-char-revamp/CH1e.md`, note "From CH1d review").

### CH1e - Done (2026-10-20, Sonnet 5.5)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE`, the commit "Character pilot Phase CH1e". **All 50 class-map rows of phase CH1e are new `CX2GUArme_VioletMage::June_AVSI_<state>_<Slot>` members** (declared after CH1d's in the header,
bodies in a fifth block at the end of the `.cpp`, each with its June address), six enum members after `AVSI_AEM_DASH_JUMP_X3` (June ids 229 / 230 / 231 `AVSI_AHM_DASH_JUMP_ZXX_START / _CHARGE / _FINISH`, 234 / 235 / 236 `AVSI_AEM_ZZXX_START / _LOOP / _END`)
and three stand-in members. Nothing registers or calls them. 52 funcmap rows `Char CH1e` (50 functions + `31fd6230`, `32272f50`). Addresses are in the source comments and `funcmap.tsv`; the groups:
- **High Magician combos / dash attacks:** X2, X3back (FrameMove / EventProcess / EndFuture), X3down, X4down, DASH_ATTACK_X (StartFuture / FrameMove / EventProcess / EndFuture), DASH_ATTACK_X2, EM_DASH_COMBO_Z2a.
- **Elemental Master combos:** ZZupZZ, XXfrontX, XZ, XZZ, XZZ_END.
- **June-only states:** AHM_DASH_JUMP_ZX / ZXX_START / ZXX_CHARGE / ZXX_FINISH (12 functions), AEM_DASH_JUMP_X3 (2), AEM_ZZX / ZZXX_START / ZZXX_LOOP / ZZXX_END (9).

**Same as March** (body copied, checked against the listing): the two `Init`s, the ZZupZZ / XXfrontX / XZ / X3down / X4down / Z2a `FrameMove`s, both `EndFuture`s, and `32cc6ae0` (= CH1d's `32cc6a50`).
**What differs from March:**
- **Fireballs.** The X2 / X3back / DASH_ATTACK_X / X2 `FrameMove`s spend `4 / 6 / 5 / 10 x m_fMPConsumeRateFireBall` through `June_FlushMpWithRefund` and create types 2 / 5 / 0 / 6 through `June_CreateComboFireBall` (March: flat `FlushMp` and fixed names). Failure args `( 0, 180, 0 )` for X3back and DASH_ATTACK_X, `( 0, 0, 0 )` for X2 and DASH_ATTACK_X2.
- **Cancels.** Every June `EventProcess` here is the data-driven template (`m_fEventTime[ 0 ]` window + flag 0, `m_fSkillCancelAfter` / `BWalk` / `Walk` / `Dash` / `AttackCancelAfterEx`) and the walk / bwalk / dash / ZX cancels also test `IsOnSomethingFuture()`; March used fixed times.
  X3back ends on the animation end first, then any ZXC key after 1.4 s (March's leading off-the-ground branch is gone). X4down uses 0.8333 and the ZXC key set. DASH_ATTACK_X2 leaves on 0.9 s with a key, else on the animation end.
- **XZ / XZZ.** XZ -> XZZ is the window + flag (March: `ELSE_IF_STATE_CHANGE_ON_ 0.89`); XZZ ends when Z is released past `keyInputStart` (March: a skill cancel).
- **XZZ `FrameMove` (`32cdab20`).** Six `EventTimerOneshot` times from the state's `SPECIAL_OPTION_VALUE[ 0..5 ]` (March: 0.333 / 1.133 / 1.933).
- **ZXX family.** `CHARGE` writes `+0x6890` (`SetNoDetonation`) every frame and has no SpecialAttack half in its skill cancel; it owns the effect handle `+0x9788`.
- **ZZXX family.** `LOOP`'s `EventProcess` is three plain `if`s (end of the loop, end of the animation = `StateChange` to itself, off the ground), not an else chain; `LOOP` / `END` `FrameMove` call `CommonFrameMove` first; `+0x9784` counts the loop seconds.
- ZZX `EventProcess` JUMP_DOWN has no `y` shift; Z2a's alpha tag (checklist 7c) is in `June_AVSI_EM_DASH_COMBO_Z2a_FrameMove`.

**Stand-ins (June layout owned by CH1; inert until then):**
1. `m_vecJuneSpecialOptionValue` + `June_GetSpecialOptionValue( i )` (June `31fd6230`: bounds-checked read of a float list at **user `+0x93a8`**, 0 out of range). The list is cleared (`31f8b7e0( 0 )` @320608a9) and filled by `CommonStateStart` `32057b80 @320608c8`
   from the state key `SPECIAL_OPTION_VALUE` (xorstr `0x33478eb4`; values from index 1 on, each pushed by `31fac820`). `ch0_keys.tsv` / CH1 item 5 already own the key; CH1 moves the list to `CX2GUUser` and parses it. Until then all six XZZ times read 0.
2. `m_fJuneZZXXLoopTime` (`+0x9784`) and `m_hJuneDashJumpChargeEffect` (`+0x9788`): the constructor sets them (`32ccb02e` 0, `32ccb034` -1); CH1 initialises them like CH1a's `m_hJuneMemoRiseLoop`.
- No unmapped engine callee remains (`32272f50` is `GetWeaponBonePos( 0, name, 0 )`, `321dafb0` `StopEffectSet( Handle& )`, `31d28e10` / `31e4c230` the anim `EventTimer` / `EventTimerOneshot`).
- `0.8333` and `0.9` read as qwords ( `0.8333`, `(double)0.9f` ); every constant is in `charport_consts.py CH1e`.

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --worktree --files X2Lib/X2GUArme_VioletMage.cpp X2Lib/X2GUArme_VioletMage.h`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `cl /Zs` of the class file under the US_SERVICE defines: 0 errors.
Build: `DEPLOY OK: X2_offline.exe is 25,575,936 B, dated 2026-10-20 11:11:20, md5 009aac7fe8bd35a886752d776fc67dec, equal to X2/US_SERVICE/x2.exe (2026-10-20 11:11:12); built at main 36bc1d22 + 2 uncommitted code file(s).`

No `JUNE_BUGS` row (the self-transition of `ZZXX_LOOP` and the per-frame `+0x6890` write are June structure, not mistakes). **Drift:** none. Checklist rows: 7c (Z2a tag) lands here; the 7ac row that `ch0_checklist_rows.tsv` assigns to CH1e (`32021390`, called from `32cb8bf0`) is **CH1g's** since CH0c moved `32cb8bf0`. None of the 50 functions multiplies `m_fNowStatePowerRate` or passes a BFI.
**In game:** nothing (the flag adds unreferenced members). Review: `kickoffs/pilot-char-revamp/CH1e-review.md`. Next: CH1f after the review.

### CH1e -- Review (2026-10-20, Opus)

**Verdict: PASS, no code fix.** All 50 functions were read against June: Ghidra decompiles, plus `funcdis.py` listings of every function that is not carved and of every cancel chain.
Spot-checked in depth in Ghidra: `32cdab20` (XZZ FrameMove), `32cc2d50` (ZXX_CHARGE EventProcess), `32cbf620` (XZZ EventProcess).
- **Cancel chains.**
  - The else-if order matches the listings in all eight chains: `32cc30b0`, `32cc34c0`, `32cc3800`, `32cc3b90`, `32cc2d50`, `32cc11f0`, `32cc1460`, `32cbf620`.
  - The order is skill / awakening, walk (`+0x8c98`), bwalk (`+0x8c94`), dash (`+0x8c9c`), then ZX (`+0x8c88`, oneZ -> vslot `0x4b0`, oneX -> `0x4b4`).
  - Each test is `0 <= f` and `f < now`, and each cancel requires `+0x7e1c` (on the ground); the bwalk test is `pureLeft && bIsRight || pureRight && !bIsRight`.
  - `32cc2d50` has only the awakening half; it sets `+0x8d4c`, then calls vslot `0x6b8`. `32cc11f0` is two plain `if`s (`0.6 <= now`, X4down `0xac`) ahead of the ground chain, and every chain ends with the animation-end test. `32cc1460` uses `0.8333` (qword, `now > 0.8333`) and has no skill cancel.
- **`32cc3800`.**
  - Three plain `if`s on `+0x9784`: `keyInputStart < t && !pureX` -> `0xec`; `keyInputEnd < t` -> `0xec`; animation end -> `0xeb` (the state's own id).
  - Then `!ground` -> `JUMP_DOWN`, with no y shift, which leads the cancels.
  - `32cc34c0` zeroes `+0x9784` before `0xeb`; `32cc37e0` / `32cc3b70` set and clear `+0x6890`, which is `SetNoDetonation` -> `SetNoDetonationByState`.
- **`32cdab20`.**
  - Six `31fd6230( 0..5 )` reads (bounds-checked, list at `+0x93a8`, confirmed). Times 0-2 are the inlined one-shot, 3-5 call `31e4c230`, short-circuit OR.
  - `FlushMp( 3 )`. The hit is `CreateInstance( this, XXZ_INVISIBLE_ATTACKBOX, GetPowerRate(), GetPos( 0 ) (slot 0x138), rot, rot, GetLandPos( 0 ).y (slot 0x134) )` with March's defaults verbatim (`0, -1, 1, 1, -1, -1, 0, -1, 0, 0`).
  - The failure path is `Dummy2_Lhand` +/- dir x 100.
  - **Stand-in accepted:** `m_vecJuneSpecialOptionValue` stays until CH1 parses the key in `CommonStateStart` (CH1 item 5, written into the CH1 kickoff by CH1e).
- **Fireballs.** Types `2` / `5` / `0` / `6` (`mov [esp+8], N`); cost `4` / `6` / `5` / `10` x `+0x95bc`; `+180` on `angle.y` for X3back and DASH_ATTACK_X; failures `(0, 0, 0)` / `(0, 180, 0)` / `(0, 180, 0)` / `(0, 0, 0)`.
- **State ids.** `0xac`, `0xab`, `0xb3`, `0xc7`, `0xd8`, `0xe5` / `0xe6` / `0xe7`, `0xea` / `0xeb` / `0xec`, landings `0x16` (DASH_JUMP_LANDING) and `0x12` (JUMP_LANDING). The six new enum names fit; CH1 numbers them.
- **Constants, strings.** All 82 rows of `charport_consts.py CH1e` match. The five `EffectSet_AHM_*`, both `EffectSet_Aisha_EM_*`, `ComboXXDownX` / `XX`, `Elemental_Master_XZ` and the six Init resource names match June's strings. The ZZupZZ / XXfrontX handles go to `+0x3eb0` (`32a2dec0` push_back).
- **Not ported, no effect:** `32ce26b0` calls vslot `0x150` (`GetRotateDegree`) before `FlushMp` and discards the result. It is a getter, so nothing is lost.

**Hygiene.** CH1e commit, `flagcheck.py JUNE_CHAR_ARME_CORE --range f65098ac^..f65098ac`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
No code change, so no build. The CH1e deploy (`X2_offline.exe` md5 `009aac7f...`, 11:11:20) stands. No drift. **In game:** nothing (inert). Next: CH1f (`kickoffs/pilot-char-revamp/CH1f.md`; still matches).

## Phase CH2 - Skills: Violet Mage and the first jobs (classes 2, 14, 15, 23)

**Run with:** Sonnet 5.5 high (transcription from CH0's map and the Ghidra decompile of each listed function), then an Opus review, then a
user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (CH0 may split by job). Sub-tags `SKILL`, `NUM`.

**Scope:** CH0's `ch2` rows. For each skill id of these four classes:
- add the `RNSI_*` enumerator;
- add its case in `SetEquippedSkillFuncBySkillSlotIndex`, with June's state functions whole;
- carry the checklist rows attached to that skill: 7b (the `32cccc70` block + D8 swap), 7c alpha-tag calls, 7u effect-name swaps, 7ai BFI
  arguments, 7ac (Hell Drop), 7p (binding circle).
- check the skill's effect closure (`ch0_effect_closure.tsv`): CH0b already staged it. A name found only now (a run-time-built name the map
  missed) is staged here, byte-exact, and added to `ch0_assets.tsv`.
- **Numerical damage, first live class:** checklist §1's gate.
  - Show `[JUNE_SKILL] NUM use ...` for an Aisha skill (`logs.py --tag JUNE_SKILL --grep NUM`).
  - Retire `SERV_IRUHADEV_JUNE_SKILL_NUMERICAL_DEBUG` in the same phase.
  - Write each changed skill's March and June damage to `MODS.md`.

**Done gate:** every class-2 / 14 / 15 / 23 skill id in `s3_reform_ids_no_state.tsv` has a state (re-run the S3 census: 0 `no_state` for these
classes, except ids CH0 marked passive / CH4 / Stage 6). In game, each skill equipped in an A or B slot casts June's state, with June's motion
and effect, and deals damage. `ch0_effect_closure.tsv` has 0 `missing` rows for these classes, and casting them adds no missing particle /
mesh / texture line to `ErrorLog.txt`.

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
candidates are Ara 34 / 124, Elesis 35 / 125 and Add 33 / 36 / 123 / 126. Third jobs (`UC_150`-`UC_186`) are out of scope (Context).

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
`SERV_IRUHADEV_JUNE_CHAR_<PREFIX>_*`.

The phase skeleton is:
- CH0;
- CH0b (every June asset of the character, whole, first; deployed together with CH1 when the character has renamed animations, checklist 7ad);
- CH1;
- CH2 / CH3 (skills by job tier);
- CH4;
- CH5;
- CH-N (only if the character has a templet-backed June-only class);
- CH6;
- Z.

The new characters (Stage 8) follow the same asset rule: every June asset of the character, whole, before their code. Z of this plan rewrites
this section from what the pilot took.

What differs per character, as known on 2026-10-20 (gap = June attacks whose damage is numerical-only; `l.` = checklist line):

| Character | Gap | Templet-backed June-only class | Known extras |
|---|---|---|---|
| Eve | 3 | - | 167 alpha-tag sites (7c); slot encoders in `X2GuEve.cpp` (7ag trap); El Crystal icons, recovery statics (7ah / 7aj); 7a modifier, 7n sync float |
| Rena | 12 | - | **`ADD_RENA_SYSTEM` decision** (natural force, 77 uses); 7ad renames (`Lire_*`); D8 3405 -> 23405 |
| Ara | 14 | 34, 124 | `CUSTOM_RENDER_STATE` (7w); 2 new motion files; orb handle retirement needs the user's OK (7e) |
| Chung | 15 | - | `ChangeMaxCharacterAddResource` already ported; 49 unnamed upgrade ids (7ae); cinematic UI (7r) |
| Elsword | 20 | - | `DAMAGE_SKILL_ID` x103 in `Elsword_DamageData.lua` (§2); Rune Slayer reflect (7j); S3d memo consumers |
| Elesis | 20 | 35, 125 | already on June base / SK motions (6 anims lost, accepted); first live reserved-buff path (7ai) |
| Raven | 25 | - | 7b passive setter `+0x4108`; 7u effect names; power-rate operator (§7 l.243) |
| Add | 0 (skeleton) | 33, 36, 123, 126 | March kit incomplete ("the user ports Add from June", l.237); `CUSTOM_RENDER_STATE`; no stat rows for 9 / 32 / 122 at all; 4 new motion files; 13 §7 rows |
