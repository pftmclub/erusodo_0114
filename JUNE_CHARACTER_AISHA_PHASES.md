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

### CH1f - Done (2026-10-20, Sonnet 5.5)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE`, the commit "Character pilot Phase CH1f". **All 31 class-map rows of phase CH1f are new `CX2GUArme_VioletMage::June_AVSI_*` members** (declared after CH1e's in the header, bodies in a sixth block at the end of the `.cpp`, each with its June address). No enum member, no stand-in. Nothing registers or calls them.
31 funcmap rows `Char CH1f` (`june2017_re/funcmap.tsv`). Eight of the functions are not carved in Ghidra and were read from `funcdis.py` listings (`32cb7de0`, `32cb8840`, `32cb8be0`, `32cbc880`, `32cbd120`, `32cd4450` / `32cd4650`, `32cd5180`, `32ce6b30`), and so was `32cdfb70` (its decompile does not settle).

| Group | June functions |
|---|---|
| DM `COMBO_X2` | `32cd4450` FrameMove, `32cbd120` EventProcess (shared with HM X2) |
| DM `COMBO_X3down` / `X4down` | `32cbc520`, `32cbc880`; `32cb7400`, `32cd3be0`, `32cbc930` |
| DM `COMBO_X3back` | `32cd4650`, `32cbd670` |
| DM `DASH_COMBO_Z2a` | `32cc6a30` Start, `32cd5050` FrameMove, `32cbde90` EventProcess |
| ADM (June-only) `DASH_COMBO_ZXX` / `ZZupZZ` / `DASH_JUMP_ZX` | `32cd5180`, `32cbdfb0`; `32cc2950`, `32ce1950`, `32cc29a0`; `32cb8be0`, `32ce2b20`, `32cc3ed0` |
| AVP `ZZfrontZ` / `ZZfrontZZ` / `_FALL` / `DASH_JUMP_X` | `32cbf810`; `32cb7d60`, `32cb7db0`, `32cbf9a0`; `32cbfa70`; `32cb7de0`, `32ce6b30`, `32cbfaa0` |
| AVP skills | Summon Bat Heavy `32cb8450`, `32cdfb70`; Abyss Angor `32cb8840` |

**Same as March:** `32cbc520`, `32cbc880`, `32cb7400`, `32cd3be0` (constants checked), `32cbfa70`, `32cbfaa0`, `32cb7db0`, the Inits, and the Summon Bat body apart from the skill lookup below.
**What differs from March:**
- **X2 `EventProcess` (`32cbd120`).** Four X windows from `m_fEventTime` (X3down with pureDown, X3front forward, X3back back key, X3 plain); `SKILL_CANCEL_AFTER( 0.01 )`; the 12-key tail at `0.966`; `m_bCanChangeState` from flag 3. March used fixed `0.9`.
- **Fireballs.** X2: `32cc2b90( 7 x rate )`, FIREBALL_SHORT, range `+0x95c8`. X3back and ADM ZXX: flat 6 MP, DARK_SHOCK_BALL, range 1. ADM ZZupZZ: flat 6 MP, DARK_SHOCK_BALL_AIR, +-87 along the facing. March paid 7 / 10 / 20 through `FlushMp`.
- **`32ce6b30` / `32ce2b20`.** Power is `+0x9640`, read as `m_fGravityBallPower` (constructor 1.0; the only other reader is the gravity-ball scale inside the `32ce4180` helper). BM / ADW: transformed ARME_ACTIVE_MAGIC_BALL2, else `June_CreateComboFireBall( 0 )` with z -30; other classes AISHA_GRAVITY_BALL.
- **Cancels.** `X3back` / `X4down` / ZXX / ZZupZZ: ground test first, then keys after 1.4 s, then the animation end as a plain `if` or the same `if`; no skill cancel. `32cc3ed0`: landing, then the hyper half only and the ground-only walk / bwalk / dash / ZX cancels (dead in the air, kept as June has them).
- **`ZZfrontZ` / `ZZfrontZZ`.** The flag also fires on the animation end; the 12-key exits are `0.94` / `1.65` (March: `ASDC_PRESSED_AFTER`).
- **Summon Bat Heavy.** The templet comes from the whole tree (`GetSkillTemplet( 22217 )`; June passes level 1, the live build has the one-argument form), not from the equipped skills. A NULL unit returns before `CommonFrameMove`, as March.
- ADM `ZZupZZ` `FrameMoveFuture` zeroes `nowSpeed` between 0.2 s and 0.8 s; DM Z2a `Start` calls `June_AdaptAdvancedTeleportationBuff`; Z2a `FrameMove` carries the alpha tag (7c).

**Build note.** The first build failed (`C2660`): the live build compiles the one-argument `GetSkillTemplet` (`UPGRADE_SKILL_SYSTEM_2013`). Fixed in the same flag; the second build passed.

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --worktree --files X2Lib/X2GUArme_VioletMage.cpp X2Lib/X2GUArme_VioletMage.h`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Build: `DEPLOY OK: X2_offline.exe is 25,590,784 B, dated 2026-10-20 19:57:25, md5 2c9d7125bd7bf83a5f23a210dc4d4033, equal to X2/US_SERVICE/x2.exe (2026-10-20 19:57:17); built at main 7b9d62fa + 2 uncommitted code file(s).`

No `JUNE_BUGS` row (no June bug found). No drift. Checklist rows: 7c (Z2a tag) lands here; none of the 31 functions multiplies `m_fNowStatePowerRate` or passes a BFI.
**In game:** nothing (the flag adds unreferenced members). Review: `kickoffs/pilot-char-revamp/CH1f-review.md`. Next: CH1g after the review.

### CH1f -- Review (2026-10-20, Opus)

**Verdict: PASS, no code fix.** All 31 functions were read against `funcdis.py` listings of the whole function. Spot-checked in Ghidra against the listings: `32cc3ed0`, `32cbf810` and `32ce1950`, which agree.
- **`32cbd120` (X2 EventProcess).**
  - `32cbd152-32cbd21d` is the `SERV_IRUHADEV_JUNE_BUFF_AWAKE_LOCK` `SKILL_CANCEL_AFTER( 0.01 )` expansion. The skill half is `0.01 < t`, ground, then vslot `0x7f4`. The awakening half is `322731a0`, oneLCtr, count > 0, slot `0x560`, `32070d60`, `+0x8d4c`, then slot `0x6b8`.
  - The windows are `+0x8c10 + 12 i`, the flags `+0x7d85..88`, the states `0xab` / `0xaa` / `0xad` / `0xa9`.
  - The keys are pureDown, forward and back as kicked off; the window tests are `start <= t < end` and the flag tests `t > stateChange || end`.
  - The 12-key tail is `0.966` (qword) with `!ground` -> `JUMP_DOWN` -45, else `WAIT`. The animation end comes only when no key is down.
  - `+0x96f0` is set from flag 3 on every path, including the first `JUMP_DOWN`.
- **Fireballs.**
  - Calls: X2 is `32cc2b90( 7 x +0x95bc )` with `FIREBALL_SHORT`, range `+0x95c8`. X3back and ZXX (byte-identical listings) are flat 6 with `DARK_SHOCK_BALL`, range 1. ZZupZZ is flat 6 with `DARK_SHOCK_BALL_AIR` at `+-87 x GetDirVector()` (slots `0x170` / `0x16c`), range 1.
  - Arguments: `CreateInstance( this, name, slot 0x204, pos, rot, rot, +0x80c4, 0, -1, 1, range, -1, -1, 0, -1, 0, 0 )`. Failures are `(0, 0, 0)`.
- **`32ce6b30` / `32ce2b20`.**
  - The class test reads `[[this+0x867c]+0xaf0]+4` (`0x17` / `0x71` = BM 23 / ADW 113); `+0x8c0c` is `m_bIsTransformed`.
  - Non-transformed is `32ce4180( &0, &pos, &angle )` with `z = -30`. Other classes always get `AISHA_GRAVITY_BALL` (June drops March's ice-ball / research test).
  - ADM ZX is `FlushMp( 5 )`, `GetWeaponBonePos( 0, TRACE_START0, 0 )`, `z = -30`, failure `(0, 0, -30)`.
  - **`+0x9640` = `m_fGravityBallPower`, confirmed by its writer.** `32ced341-32ced39b` (passive init) is March's Gravity Ball Research block (`X2GUArme_VioletMage.cpp:2818-2830`): skill `0x5784`, `+0x9640 = 1`, `+0x963c` (`m_bResearchGravityBall`) `= 0/1`, `*= GetSkillAbilityValue( 6 = SA_ATK_REL )`.
  - **Accepted deviation:** June null-checks the templet and not the unit; the port null-checks `GetUnit()` and reads `GetClass()`. That only differs when a `CX2GUUser` has no unit or templet.
- **`32cdfb70`.**
  - `GetSkillTemplet( 0x56c9, 1 )` on `[g_pData+0x50]`. A NULL unit jumps past `CommonFrameMove` (`32cdffe0`).
  - The level is `max( 1, 32890620( id, 0, 0, 1 ) )` and the lifetime `32206e50( 1, lv, 0, 0.0 )`.
  - Both `PlayEffectSet` calls carry March's defaults plus the trailing `1.0`, which is `321e1e60`'s own default (funcmap). The inlined Z vector matches `CX2GameUnit::GetZVector`.
- **`32cc3ed0`.** Landing `0x16` leads; the skill half is dead after it (the compiler dropped it, leaving one dangling `fcomp`). Walk `+0x8c98`, bwalk `+0x8c94`, dash `+0x8c9c` and ZX `+0x8c88` each require `+0x7e1c` (Ghidra agrees).
- **`32cbf810` / `32cbf9a0`.** The direction change clears flag 0; then end, air (`0x11`, no y shift), flag (`> 0.8` or end) -> `0xc9`, arm (`< 0.8`, oneZ), `> 0.94` 12-key. ZZfrontZZ is end or (`> 1.65` and 12 keys), then air `0xca` / `WAIT`.
- **Rest.** X3down (`< 0.85` arm, `> 0.85` -> `0xac`, then plain ifs), X4down = X3back (byte-identical; 14 keys incl. pureZ / pureX `+0x86ea` / `+0x86ee`), Z2a (end leads, `0xb1` = the `AVSI_DASH_COMBO_Z3up` slot), ZZupZZ `FrameMoveFuture` (`+0x304` / `+0x308`), the small Start / EventProcess bodies, and every Init (names and counts, 23 + 6 for Abyss Angor) match June.
- **Constants, strings, state ids.** All `charport_consts.py CH1f` rows match. The names `FIREBALL_SHORT` / `DARK_SHOCK_BALL(_AIR)` / `DamageEffect_ADM_DashJumpZX` / `AISHA_GRAVITY_BALL` / `ARME_ACTIVE_MAGIC_BALL2` / `TRACE_START0` / both effect sets are read from June. The ids `0xa9`..`0xad`, `0xb1` and `0xc9` / `0xca` fit `ch0c_reg.tsv`.

**Hygiene.** CH1f commit, `flagcheck.py JUNE_CHAR_ARME_CORE --range 74c6bee4^..74c6bee4`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
No code change, so no build. The CH1f deploy (`X2_offline.exe` md5 `2c9d7125...`, 19:57:25) stands. No drift. **In game:** nothing (inert). Next: CH1g (`kickoffs/pilot-char-revamp/CH1g.md`; still matches, plus a From-CH1f-review note).

### CH1g - Done (2026-10-20, Sonnet 5.5)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE`, the commit "Character pilot Phase CH1g". **23 new `CX2GUArme_VioletMage::June_*` members** (declared after CH1f's in the header, bodies in a seventh block at the end of the `.cpp`, each with its June address): 21 of the 23 class-map rows, plus **two callbacks the class map lacks** (`32cc4980` ABM_DASH_JUMP_ZXX and `32cc5050` ADW_DASH_JUMP_X3 EventProcess; `ch0_state_skill.tsv` gives both states to CH1g, no phase owns them otherwise).
23 funcmap rows `Char CH1g`. Eleven functions are not carved in Ghidra and were read from `funcdis.py` listings (`32cb8570`, `32cb8640`, `32cc1620`, `32cc16f0`, `32cc45c0`, `32cc4980`, `32cc4c90`, `32cc5050`, `32cc5260`, `32cc73e0`, `32ce72f0`).
Four enum members were added after `AVSI_AEM_ZZXX_END`: `AVSI_ABM_DASH_JUMP_ZXX` (238), `AVSI_ADW_DASH_JUMP_X2` (239), `AVSI_ADW_DASH_JUMP_X3` (240), `AVSI_ADW_ZZZBackZ` (244); CH1 decides the final numbers.

**NOT written - this needs Opus (2 rows):** `32cb8bf0` (`AVSI_ABM_DASH_JUMP_ZX.StartFuture`) and `32cb8c80` (`AVSI_ADW_DASH_JUMP_X2.StartFuture`). Both copy `syncData.position`, call **`32021390( 1000.0, -600.0, &pos, 5 )`** and write it back. `32021390` is the unmapped CX2GUUser "landing prediction" helper (checklist 7ac, `ch0_checklist_rows.tsv` L747; a physics-step loop plus a line-map query). The kickoff stops on an unmapped engine callee, so the CH1g review ports the helper and the two StartFutures (Part 1 of `CH1g-review.md`).

| Group | June functions |
|---|---|
| ABM `COMBO_Z1..Z4` | `32cbfe50`, `32cc0000`, `32cc00f0`, `32cc0390` |
| ABM `DASH_COMBO_Z2a` / `ZXX` / `ZXXX` | `32cdc0f0`, `32cc0500`; `32cc05a0`; `32cc06e0`, `32cc0b80` |
| DW `DASH_ATTACK_X` / `X2`, `ADW_COMBO_ADD_X` | `32cc1620`; `32cc16f0`; `32cc1900` |
| ABM / ADW dash jump | `32cc45c0`, `32cc4980`; `32cc4b90`; `32cc4c90`, `32cc5050` |
| ADW `ZZZBackZ` (June-only) | `32cc73e0`, `32ce72f0`, `32cc5260`, `32cb8d10` |
| ADW Screwdriver Driller | `32cb8570` Init, `32cb8640` FrameMove |

**Same as March:** `32cc0000`, `32cc0390`, `32cc0500`, `32cc05a0`, `32cc06e0`, `32cc0b80`, `32cc1620`, `32cdc0f0` (plus the alpha tag, 7c), `32cb8570`.
**What differs from March:**
- **Z1.** The animation end (`m_fCanNotAttackTime` 0.1, WAIT) now comes before `SKILL_CANCEL_AFTER( 0.01 )`.
- **Z3.** Dimension Witch + the back key inside 0.251-0.466 s goes to `ADW_ZZZBackZ` (`0xf4`, flag kept). Then flag -> Z4 `0xcb`; back + 0.647 s -> walk; double tap -> dash (both clear the flag); last the skill / hyper cancel and the end. The cancel chain is new; March had a plain window and the flag only.
- **Four identical bodies.** `32cc16f0`, `32cc1900`, `32cc4980`, `32cc5050` are byte-equal after normalising jump targets: end, ground walk, `SKILL_CANCEL_AFTER( m_fSkillCancelAfter )`, ground dash (March has the same four without the ground test). One file-local macro serves all four.
- **`32cc45c0` / `32cc4c90`.** The end only waits; X (or Z for X2) window 0 -> next state `0xee` / `0xf0`; then skill, walk, bwalk, dash, oneZ / oneX cancels. **`32cc4b90`:** landing `0x16`, Z window 0 -> `0xef`.
- **`ZZZBackZ`.** `StartFuture` sets `nowSpeed.x = GetDashSpeed()` when the held key faces the facing; `FrameMove` is the `32ce61f0` fireball with 5 MP; `EventProcess` ends with the pure Z / X attack cancels (`+0x8c8c`); `EndFuture` turns the facing (as `32cb7450`).
- `32cb8640` is a bare jump to `CommonFrameMove` (March: world colour and cut-in light).

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --worktree --files X2Lib/X2GUArme_VioletMage.cpp X2Lib/X2GUArme_VioletMage.h`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Build: `DEPLOY OK: X2_offline.exe is 25,601,024 B, dated 2026-10-20 20:36:14, md5 e1a55a7bf3eec14f7005282661fffa52, equal to X2/US_SERVICE/x2.exe (2026-10-20 20:36:06); built at main a964e5db + 2 uncommitted code file(s).`

No `JUNE_BUGS` row (no June bug found). No drift beyond the two unowned callbacks, which were taken in. None of the functions multiplies `m_fNowStatePowerRate` or passes a BFI. Checklist row 7ac L747 stays open until the review ports `32021390`; 7c (Z2a tag) lands here.
**In game:** nothing (the flag adds unreferenced members). Review: `kickoffs/pilot-char-revamp/CH1g-review.md` (Opus; Part 1 is the two StartFutures). Next: CH1h after the review.

### CH1g -- Review (2026-10-20, Opus)

**Verdict: PASS with one fix; Part 1 written.** All 23 functions were read against `funcdis.py` listings of the whole function. Spot-checked in Ghidra against the listings: `32cc00f0`, `32cc1900` and `32cb8d10`, which agree.

**Part 1 - the landing prediction and the two StartFutures.**
- `32021390` -> `CX2GUArme_VioletMage::JuneLandingPrediction( fSpeedXAdd, fMinYOffset, pOut, iStep )`. In June it is a CX2GUUser method (other callers `329d22b0`, `329d2340`, `329f092b`, `32ae9b40`, `32b93fe0`); it stays in the class files until one of those ports moves it (funcmap row).
- **It steps `nowSpeed.x`, not `.y`.** June's PhysicParam starts with a byte at `+0x300`, so `+0x304` / `+0x308` are `nowSpeed.x` / `.y` (funcmap's SPEED_X/Y row; `fReloadAccel +0x318` and `fWalkSpeed +0x324` fit). The kickoff and checklist 7ac said `.y`.
- Body: save `syncData.position`; `iStep` x { `nowSpeed.x += fSpeedXAdd`; `CX2GUUser::PhysicProcess()` (`32015b30`, a direct call) }.
- Then `CanDown( moved pos, LINE_RADIUS, &out, NULL, &idx, false, false, true, false )` on `GetWorld()->GetLineMap()` (vslot `0x90`, `+0x34`). The hit counts only on an `LT_NORMAL`, enabled line (`GetLineData` = June's inline bounds test).
- Otherwise `CanDown( start pos, ..., NULL, NULL, ... )`. Either hit clamps `y = max( out.y, start.y + fMinYOffset )`; a miss returns the start with `y = landPosition.y` (`+0x7e24`; June's PhysicParam writes `+0x7e20..28` on landing). Every path restores the position and sets `nowSpeed.x = 0`.
- The fallback query has no world / line-map NULL test: kept verbatim, `JUNE_BUGS` 375 (latent).
- `32cb8bf0` / `32cb8c80` (byte-equal): `JuneLandingPrediction( 1000 (33437b40), -600 (33483508), &pos, 5 )` on a copy of `syncData.position`, write back, then `CommonStateStartFuture` (vslot `0x7c0`). Both states start by teleporting to a landing point ahead.

**Part 2 - the 23 functions.**
1. Z1 / Z3 are as the Done note says. Z3's `fcom`s give `0.251 <= t <= 0.466` for the `0xf4` branch and the window, `t > 0.466` for Z4, and `0.647 <= t` for walk. Z4 / walk / dash clear the flag, `0xf4` does not. The skill / hyper cancel and the end are the last `else`.
2. The shared bodies `32cc16f0` = `32cc1900` = `32cc4980` = `32cc5050` (0 diff lines after normalising jump targets). `+0x8c98` walk (-> `WAIT 0xd`), `+0x8c90` skill, `+0x8c9c` dash; `+0x8c88..0x8c9c` follow `X2GUUser.h:4582-4587`.
3. `32cc45c0` / `32cc4c90` differ only in `0xee` / `0xf0` and oneX `+0x86ef` / oneZ `+0x86eb`. Flag test `t > stateChange (+0x8c18) || end`, window `start <= t < end`; then skill, walk, bwalk, dash, oneZ / oneX.
4. `32cc5260`: end, skill, bwalk, walk, dash, pureZ `+0x86ea` / pureX `+0x86ee` on `+0x8c8c`.
5. `32ce72f0`: `0.3` timer, `5.0 x` protected `+0x95bc`, angle + 180, bone `Dummy2_Lhand` (`3345bb08`), failure `( pos, 0, 180, 0 )`. `32cc73e0`: start future first, then `+0x304 = GetDashSpeed()` (`nowSpeed.x`, as written). `32cb8d10`: end future, `+0x7e08 += 180`, `bIsRight` flipped.
6. `32cdc0f0`: `+0x983c > 0` and `31fca9e0( 0x92f95 = 602005 = SMI_AISHA_MEMO5 )` (June's `GetEqippedSkillMemo` stub, always 0), super armor `( 0, 0.5 )`, `+0x8040 = 0`.
7. The `= March` bodies: constants match `charport_consts.py CH1g`; Driller Init's 16 names are in June's order (June's duplicate `screw.dds` kept).

**Fix (transcription).** `32cc0b80` tests `[this+0x867c]+0x38` = `GetUnitData().m_UnitClass`, as March's own ZXXX EventProcess does. CH1g had `NULL != GetUnit() && GetClass()` (the templet's class). It now reads `GetUnit()->GetUnitData().m_UnitClass`.

**Hygiene.** CH1g commit, `flagcheck.py JUNE_CHAR_ARME_CORE --range 4ec278df^..4ec278df`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Review edits, `--worktree --files X2Lib/X2GUArme_VioletMage.cpp X2Lib/X2GUArme_VioletMage.h`: `+68 -1` / `+3`, `files: 2 PASS`, `1 deleted line(s)` (CH1g's own ZXXX test, inside the flag), `RESULT: PASS`.
Build: `DEPLOY OK: X2_offline.exe is 25,601,536 B, dated 2026-10-20 20:59:58, md5 cd64689565df16404aeff65bcbb53d33, equal to X2/US_SERVICE/x2.exe (2026-10-20 20:59:51); built at main 4ec278df + 2 uncommitted code file(s).`

Records: funcmap `32cb8bf0` PORTED, new rows `32cb8c80` and `32021390`, note on `32cc0b80`; checklist 7ac L747 annotated, its `ch0_checklist_rows.tsv` row moved to "CH1g review"; `JUNE_BUGS` 375. No drift. **In game:** nothing (inert). Next: CH1h (its kickoff still matches; a From-CH1g-review note was added).

### CH1h - Done (2026-10-20, Sonnet 5.5)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE`, the commit "Character pilot Phase CH1h". **33 of the 62 class-map rows became `CX2GUArme_VioletMage::June_*` members** (declared after CH1g's in the header, bodies in an eighth block at the end of the `.cpp`, each with its June address); the other 29 were **not CH1h's** and moved (below). Three enum members `EffSetID_June2..4` were added (June's `m_ahEffectSet` has five slots, March two). 47 funcmap rows `Char CH1h`.

| Group | June functions |
|---|---|
| Lifecycle | ctor `32cc99d0` (`June_ConstructorInit`, the initial values), dtor body `32ce4600` (`June_Destructor`; `32cf2d50` is the scalar deleting wrapper), `ReInit` `32ce4db0` |
| Overrides | `DamageReact` `32cb6e00`, `InitEffect` `32cb6e50`, `OnFrameMove` `32cb7eb0`, `NotifyDGManagerChanged` `32cb7ef0`, `Win` `32cb8fd0`, `Lose` `32cc5ca0`, `Draw` `32cb9010`, `InitEnchantWeaponEffectUnitSpecific` `32cb9110`, `InitEtc` `32cb96b0`, `ApplyRenderParamOnEmptyRenderParamByBuff` `32cc1c50`, `COMMON_BUFF_FrameMove` `32cc6e50`, `InitDevice` `32cc7fa0`, `InitComponent` `32ccdb70`, `DeleteMinorParticle` `32cdd4e0`, `HyperModeBuffEffectStart` `32cddcc0`, `CreateMajor / MinorParticleArray` `32ce4e80` / `32ce55e0` |
| Helpers | `32cce090` Delete Arme minor, `32cce230` / `32cce370` set Aisha major / minor by enum, `32cb9160` init major array, `32cc98e0` AddStoneWall, `32cc2880` MemoRise empty, `32ce1900` clear MemoRise, and five static device-list builders `32cb9050` `32cb9220` `32cb95c0` `32cb96c0` `32cc5d30` |

**Same as March:** `32cb9110`, `32cc1c50`, `32cc6e50` (minus its `#ifndef UPGRADE_SKILL_SYSTEM_2013` head), `32cc7fa0` (June has no `m_pLightningBolt` block), both particle-array builders (names, ids, draw counts), `32cb9050` / `32cb9220` / `32cb95c0` / `32cc5d30`, `32cc98e0` (= `AddStoneWall`).
**What differs from March:**
- **`InitComponent`.** The PVP initial MP is gone (`GetGameType` is called and the result dropped); the eight minor handles are tagged with the effect-alpha category (`32296da0`). The two setters `32cce230` / `32cce370` tag their new handle the same way (checklist 7c).
- **`ReInit`.** `MP < 50.0` and game type != PVP -> `SetNowMp( 50 )`; in game types 1 and 4 (4 has no March value) it clears the MemoRise entries.
- **`DamageReact`.** After `DamageReactStateChange( pDD, -1, -1 )`, meditation + `DD.m_bNoDamage` false erases BTI 35 and BTI 107 (March tests memo 11 and erases 35 only).
- **`Win` / `Draw` / `Lose`.** June's riding states are ids `0x4b` / `0x4c`, the names `USI_RIDING_WAIT_HABIT` / `USI_RIDING_WAIT` (June `GUSI_END + 70 / 71`; March's enum is 74 / 75). `Lose` skips the HP test when there is no gage.
- **`HyperModeBuffEffectStart`.** Returns before the four boost handles when CX2Unit `+0x3c` is set (June reads it as "transcended") or the awakening effect set exists; both are stand-ins (false).
- **`InitEtc`** is an empty override. **The constructor** sets `m_fElementalResearchRate` to 1.0 (March 0), has no `m_pLightningBolt`, and sets the June-only fields to the values in the header comments (named by offset, `m_iJune96e4` .. `m_vecJune9848`, `m_fJune8fb8` 1.1 and `m_fJune8fbc` 1.2 are CX2GUUser members). `+0x7d10` is the Aisha `MemoCondition` (funcmap `334720bc`, not ported); the base ctor gets a sixth argument `0xf9`, not ported.
- **Destructor.** Explicit releases only; `m_SubAttackListSet` is cleared a second time without the staff test.

**Moved (scope rule; the class-map filter and funcmap rows carry them):** these were bucketed CH1h because the map found no registration. They are skill-state callbacks registered inside `32cede00` (thunks `32cf2600` .. `32cf2be0`), or helpers of a later phase's function.
- **CH2a:** `32cccb40` `32cdb5e0` `32cdbb70` (Meteo Shower Start / FrameMove / End), `32cdc4a0` (Fire Road). **CH2b:** `32cdcac0` (Energy Drain). **CH3a:** `32ccd320` `32ccd400` `32cdf240` (Lightning Shower), `32cd75f0` `32cd7800` (Circle Flame), `32cd9af0` + `32cf1a80` (Hell Drop, UFO getter).
- **CH4:** `32ce9f90` (skill 22416 passive callback). **CH1i:** `32cc9680` (helper of `32cc98b0`), `32ce6f40` (helper of `32ce7480`), `32ce16d3` (tail of `32ce15a0`), `32ce1770` (container sweep, checklist 7ak).
- **n/a:** `32cf1ab0` `32cf1bd0` `32cf1c60` `32cf1dc0` `32cf1eb0` `32cf20a0` `32cf2100` `32cf2550` (STL instantiations for `m_vecStoneWallData`), and `32cf2d90` `32cf2da0` `32cf2e70` `32cf2e90` (past Aisha's end: Elsword starts at `32cf2da0`).
The five kickoffs got a "From CH1h" note.

**Open for the review (stand-ins, not guessed; all need Opus).**
1. The MemoRise container at `+0x9760` (element type; the entries look like skill ids) is still `m_iJune9770`; `June_ClearMemoRise` and `June_IsMemoRiseEmpty` sit on it.
2. CX2Game `322deb80` (redraws the MemoRise slots) is the no-op `June_GameRefreshMemoRise`; CX2Unit `+0x3c` is `June_UnitFlag3c` (false).
3. The June-only constructor fields (roles unread) and `EffSetID_June2..4`.

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --worktree --files X2Lib/X2GUArme_VioletMage.cpp X2Lib/X2GUArme_VioletMage.h`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `cl -Zs` of the `.cpp` under the US_SERVICE defines: 0 errors, the block is in the preprocessed unit.
Build: `DEPLOY OK: X2_offline.exe is 25,616,384 B, dated 2026-10-20 21:40:54, md5 e4a46981038d8dd7cdc3b1cd6aee3830, equal to X2/US_SERVICE/x2.exe (2026-10-20 21:40:47); built at main ff568169 + 2 uncommitted code file(s).`

No `JUNE_BUGS` row (the dropped PVP initial MP and the null-gage `Lose` are June behaviour, not defects). Checklist rows touched: 7 l.266 (`32cc98e0`, ported here) and 7c (`InitComponent` / setter tags, ported here).
**In game:** nothing (the flag adds unreferenced members). Review: `kickoffs/pilot-char-revamp/CH1h-review.md` (Opus). Next: CH1i after the review.

### CH1h -- Review (2026-10-21, Opus)

**Verdict: PASS with one fix (member map).** Read in Ghidra and in `funcdis.py` listings: the constructor `32cc99d0` (whole store sequence plus the constants), the destructor `32ce4600`, `Win` / `Draw` / `Lose`, `ReInit`, `InitComponent`, `HyperModeBuffEffectStart`, `DamageReact`, `ApplyRenderParam...`, `32ce1900`, both particle arrays and the five device lists. All of them match June apart from the member map below.

**1. Constructor.** I re-derived the map from `+0x9598` (`m_bZ4up`) to `+0x9854`; June keeps March's declaration order.
- All values check out, read from the exe: `5.0` / `0.2` (`3351b044` / `3351aff8`, protected `+0x971c` / `+0x9728`), `m_iHyperChargeMP` 100, `m_fElementalResearchRate` 1.0, the throw values, `+0x8fb8` / `+0x8fbc` = 1.1 / 1.2.
- **Fix:** `+0x96ec` is `m_fLevitationElapsedTime`, because CH1a's ported JumpLevitation StartFuture / FrameMoveFuture read and write it there. `+0x96e0` is the June-only float. CH1h had the two swapped.
- **Fix:** `+0x9758`, `+0x9780` and `+0x97a4[5]` are floats (`fld` / `fadd` / `fstp` in their readers). CH1h declared them `int`.
- **Fix:** `+0x983c` is March's `m_iAdvancedTeleportationLv`, which March keeps in CX2GUUser; `32cc1cd0`, ported by CH1b, reads it. CH1h's duplicate `m_iJune983c` is gone, and the constructor sets the March member to 0.
- All three fixes change names and types only: every value is 0, so nothing behaves differently.
- **Roles found:** `+0x96e0` is a Hell Stone recast window. It is set to 5.0 at the first cast (`32cc6d70`), and `+0x96e4` counts casts. On the third cast, `32cc2ab0` resets both and starts skill 22017's cooldown. `+0x96e8` is a mode tested for 1 / 2 / 3. The other June-only fields go to their readers' phases (CH1i, CH4 notes).
- **The MemoRise container (`+0x9760`) is a VS2010 `std::deque<int>` of skill ids:** proxy / map / map size / offset / size `+0x9770`, entry `32cc7df0`. CH1i replaces `m_iJune9770` with it.

**2. Destructor.** The release order and the two `SubAttackListSet` clears (one with the staff test, one without) match June, and so does the `+0x96dc` mesh destroy. `322ceab0` / `322ceaa0` are the minor / major getters, as used.

**3. Lifecycle.** `Win` / `Draw` push `0x4b`, `Lose` pushes `0x4c`. June's `GUSI_END` is 5, so these are `USI_RIDING_WAIT_HABIT` / `USI_RIDING_WAIT` by name; `0xb` / `0xc` are `USI_WIN` / `USI_LOSE`.
- **Correction to the Done note:** `Lose`'s gage test is March's `GetNowHp()` inlined (a null gage gives 1, a null HP gage gives 0). The body is therefore the same as March's, not a June difference; the code is equivalent and stays.
- `ReInit`: `MP < 50.0` and game type != 1 (`GT_PVP`) -> `SetNowMp( 50 )`; the MemoRise clear runs in game types 1 (PVP) and 4.
- `InitComponent`: the eight `32296da0( h, 1 )` tags match. `HyperModeBuffEffectStart`: unit `+0x867c` -> `+0x3c`, then `31fd66b0`. `DamageReact`: `( pDD, -1, -1 )`, `DD+0x4dc` is `m_bNoDamage`, BTI 35 and 107.

**4. Stand-ins.**
- `June_UnitFlag3c` is confirmed as the transcendence flag. The skill plan's S7 rows use the same no-field stand-in; Stage 6 owns it.
- `322deb80` is the **CX2Game memorize-slot HUD**: `Memorize_Slot_%d_TypeA/B` and `UseMemorizedSkillHotKey_A/B` per entry, for unit type 2 with a non-empty list. It goes to **CH4** (memorize statics, 7ah); `June_GameRefreshMemoRise` stays a no-op until then.

**5. Routing.**
- Checked registrations in `32cede00`: `32cccb40` / `32cdb5e0` / `32cdbb70` at `32cee3bf` (`AVSI_SA_AHM_METEOSHOWER_LOOP`, `0xd9`); `32cdf240` at `32ceeb1b` / `32ceeb7a` (`AVSI_SA_AHM_LIGHTNING_SHOWER_LOOP`, `0xdd` / `0xde`); `32cd9af0` at `32cef318..` (`AVSI_HELL_DROP_CONTROL_*`, `0xbb..`).
- `32cf2da0` is referenced from `33522de4` in Elsword's SwordMan vtable (`33522914`), and `32cf2e90` from `335230d8`: both are past Aisha's end.

**6. `= March` bodies.** Names and order match, including the common slots 5-11 and the draw counts 2,2,2,2,1,1,1 / 2, then -1 / 1. `LightningBolt10Red` is missing from both append lists.

**Hygiene.** CH1h commit, `flagcheck.py JUNE_CHAR_ARME_CORE --range a1314641^..a1314641`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Review edits, `--worktree --files X2Lib/X2GUArme_VioletMage.cpp X2Lib/X2GUArme_VioletMage.h`: `files: 2 PASS`, `13 deleted line(s)` (CH1h's own lines, inside the flag), `RESULT: PASS`.
Build: `DEPLOY OK: X2_offline.exe is 25,616,384 B, dated 2026-10-21 00:39:09, md5 d8f5541642038731e232ea2dba5c8918, equal to X2/US_SERVICE/x2.exe (2026-10-21 00:39:03); built at main a1314641 + 2 uncommitted code file(s).`

Records: funcmap `32cc5ca0`, `322deb80` (ROUTED CH4) and `32cc99d0` amended, new row `32cc7df0`; "From the CH1h review" notes in `CH1i.md` and `CH4.md`. No JUNE_BUGS row. No drift. **In game:** nothing (inert). Next: CH1i.

### CH1i - Done (2026-10-21, Sonnet 5.5)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (no new flag; `Always.h` untouched), the commit "Character pilot Phase CH1i". **Of the 45 class-map rows, 38 became `CX2GUArme_VioletMage::June_*` members, 4 are partial or stand-ins (below), 3 are n/a** (`32ce16d3` is a decompiler split of `32ce15a0`'s tail; `32cf1c20` / `32cf1c40` are deque iterator constructors). Also ported because the rows call them: the class helpers `32ce15a0` (erase entry), `32cc7e60` (pressed-slot kind, CH1), `32cc7df0` (entry i, CH1), `32ceca70` (memorize a skill, CH4), and the CX2GUUser pieces June's class code reaches, as Arme-local members (the base has no March host): `31fcac90` `31fceab0` `31fcad70` `32007b10` `3202d140` `3202d3c0` `32008e40` `3201f850` / `32028190` `31fd32e0` `31fdbe70` `31fc86e0` `31fc93e0` `31fcec40`. 60 funcmap rows `Char CH1i`.

| Group | June functions |
|---|---|
| Skill use ( the 509-512 split ) | `32cb97a0` event process, `32cebe80` lookup, `32ced640` use, `32ccf4e0` cooldown / MP, `32ccbca0` no-stage-change, `32ccf9f0` action frame move, `32ccb830` skill levels |
| Common overrides | `32ce7480` start, `32cce4b0` frame move, `32ce5700` end, `32cb9720` start future, `32cb9750` frame move future, `32cb6ea0` end future, `32cb6eb0` render prepare, `32ce5760` hyper frame move, `32cb9650` equipped skill state |
| MemoRise | `32cc7b30` charge test, `32cc7f10` contains, `32ce1770` sweep, plus the four above; **`m_iJune9770` is gone: `std::deque<int> m_deqJuneMemoRise`** |
| Transform weapon / riding | `32cc8980` `32cc8ae0` `32cc8cf0` `32cc8d30` `32cc9950` `32cc5b00` `32cf25a0` `32cb8b60` / ( `32ccda90` = `32ccda40` ) `32cc7a40` `32cb8ab0` |
| Other | `32cc2ab0` Hell Stone window, `32cc5b20` Mana Flow Overdrive, `32cc26f0` / `2740` / `2790` bodies, `32ce6f40` Angor Hammer, `32cc7c30` after-image, `32ccdae0` UFO factor, `32cc9680` / `32cc98b0` cut-in, `32cc7b90` |

**What the transcription settled** (each with its June address in the code): the lookup keeps March's checks in March's order and adds June's kind (`32cc7e60`), two skill-900 checks, the clamped cool time (`min( slot cool time, actual cool time )`) and `m_fJune8ea8 + m_fJune8eb0 > NowMp`. Mode `+0x96e8` is the skill-use kind: 1 slot, 2 memorize (cost x skill 22412's `SA_COOLTIME_3`), 3 cast the first memorized skill (Skill5 key on an empty slot). `+0x8ea0 / 4 / 8`, `+0x8eb0` are the chosen skill, its slot, the reserved MP and the MP to spend. State ids of `SetEquippedSkillLevel` were read from `32cede00` (`june2017_re` scan: `push name; push id; call 31fd89b0`): 186 `CIRCLE_FLAME`, 187-196 `HELL_DROP_CONTROL_IDLE..FIRE`, 207 `ENERGY_DRAIN_SUCCESS`, 209 / 210 `GUILLOTINE_PRESS_LOOP / END`, 217 / 218 and 221 / 222 the Meteor / Lightning Shower loops, 246 / 247 / 248 `RUIN_LOOP / END`, `AIR_STUNNER_END`. **Three enumerators were added** for the last three, which March has not got (`AVSI_SA_AVP_RUIN_LOOP`, `_RUIN_END`, `AVSI_SA_ADW_AIR_STUNNER_END`, script ids 246-248): CH3a / CH3b register the states and must not add them again. `JuneMpRefundData` (CH1c) now is June's Strong Mind object: it derives `TimeLimited` and has the rate fields `32cce4b0` / `32ccf4e0` read.

**Stand-ins and gaps (all need Opus; nothing guessed):**
1. `June_Vslot828` ( `32cc7b90` ): its base test `31fd7640` ( the common wait / move / jump / riding state list, with an unidentified member at `+0x7d04` ) is not ported, so it answers false.
2. `June_UpdateEquipCollisionData` ( `32cc8d30` ): March's walk plus the transform weapon's boxes; June's riding pet part ( `32d2eb50`, pet `+0x1bc / +0x238 / +0x478` ) and the item-type anim array choice ( equip `+0x544 / +0x584` ) are missing.
3. `June_UpdateEquipViewListInfo` ( `32cc9950` ): while transformed June queues the change ( `3207b5d0` ); here it is dropped. June's signature takes 8-byte objects and a third int.
4. `June_ShowActiveSkillCutInAndLight` ( `32cc9680` ): the cut-in item override ( `31fd8a50`, a unit map at `+0x8e58` ) and the event `0x8b3` swap are missing.
5. `3202d3c0`: the dungeon attribute bit `0xd` ( `327d9df0` ) has no March home; read as false.
6. The June sync field ( Now `+0x8160`, Future `+0x7ec0` ) is `m_iJune8160` / `m_iJune7ec0`: nothing copies Future to Now yet ( the frame data is March's ).
7. `June_Vslot860` reads NPC `+0x874c` as 0; `June_UnitFlag3c` is still the stand-in.
8. `SetEquippedSkillLevel` writes the state data directly; June's dirty-state batch ( `3207c1a0` / `31fdf970` ) has no map form.
9. **Not ported, routed to CH4 with `322deb80`:** `322f4740`, the CX2Game per-frame memorize-slot HUD whose throttle is the `+0xb38` float that `322cc030` sets ( `June_SetGameFloatB38` stays a no-op ).

**Findings.** JUNE_BUGS 376 ( Shadow Body shows skill 2218's name ), 377 ( the MemoRise sweep skips the entry after an erase ), 378 ( negative MemoRise index ). `32ccda90` is the checklist's "spec A reader"; the real reader is `32ccdae0`.

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --worktree --files X2Lib/X2GUArme_VioletMage.cpp X2Lib/X2GUArme_VioletMage.h`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 31 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The 31 deleted lines are CH1a / CH1b / CH1h's own stand-ins, inside the flag ( `June_IsMemoRiseChargeActive`, the empty `June_Vslot60c / 608`, `m_iJune9770` and its five uses, `m_wJune978c` -> two bools, the `JuneMpRefundData` struct ). `cl -Zs` of the `.cpp` under the US_SERVICE defines: 0 errors, 0 warnings in the file.
Build: `DEPLOY OK: X2_offline.exe is 25,635,840 B, dated 2026-10-21 01:59:05, md5 15bef82779c9dc7af89a74fcf127fcd0, equal to X2/US_SERVICE/x2.exe (2026-10-21 01:58:58); built at main 567de9f7 + 2 uncommitted code file(s).`

**In game:** nothing ( inert ). Review: `kickoffs/pilot-char-revamp/CH1i-review.md` ( Opus ). Next: the CH1i review, then CH1.

### CH1i -- Review (2026-10-21, Opus)

**Verdict: PASS, no code fix.** I read each function against June's listing (`disasm_range.py` / `funcdis.py`) or a Ghidra decompile: `32cebe80`, `32ccf4e0`, `32ced640`, `32cce4b0`, `32ce7480`, `32ccf9f0`, `32008e40`, `32ccb830`, `31fd32e0`, `31fc86e0`, `32cb8af0`, `31fdaa70`, `31fdbf50`, `32ccdae0`, `3202d140`, `3202d3c0`, and the sweep loop of `32ce1770`. Every port matches its June function.

**1. The 510 lookup `32cebe80`.**
- The gates run in this order: quick-slot wait `+0x8d78`, then `31fcadd0`, then the kind `32cc7e60`.
- Kind 2: vslot `0x880` contains -> kind 1. Otherwise `32cb8af0` (= `0 < +0x9758`) refuses, `+0x9758` = 0.1 (`334395e0`), and if `31fcec40` fails, text `0x7697`.
- Then the slot / templet id mismatch (text `0x8bae`). Teleport 22200 / 22216 with 22417 learned widens the condition to 5 (`ASUT_ANYTIME_EXCEPT_AIR`) in the states of vslots `0x114` / `0x118` / `0x490` (DOWN_FRONT / DOWN_BACK / AIR_DOWN_LANDING, C3d-1).
- Then `31fca9f0` and `3202daf0` (both waived for kind 2), `3202cba0`, `3202d140`, `3202d3c0`, `329cc370` and vslot `0x818`.
- The MP-cost call is vslot `0x6a0` with `( id, level, 1 )`, which is March's `GetActualMPConsume` (it passes `true`, X2GUUser.cpp:25618).
- Sync field: cleared, then set to the skill in kind 3. Open Mind uses SA `0x96` (`SA_COOLTIME_3`). Vslot `0x82c` is `31fdaa70`, which always answers false.
- The clamp `fcomp st(1)` + `test ah, 0x41` is `min( slot cool time, actual cool time )`. The MP test is `fcompp` on `ea8 + eb0` against vslot `0xd8`.
- The hyper stone test is type 13 with `31fd0570` false and `GetNumItemByTID( 130208 ) <= 0`.
- The texts `0xe2` / `0x9f5` / `0x6607` are 226 / 2549 / 26119. When the global `+0x40` flag is set (in-house), June also shows each text's id in front of the text; `June_PushInfoText` shows the text alone, which is the service form.

**2. Use and cooldown.**
- `32ced640`: mode 2 runs `32ceca70` -> cooldown, action 0x24, `AISHA_Memorize_System.ogg`, and the `+0x9778` stop at 3 or more entries. The other modes use type 7 / 14-16 for Elemental Friendship (action 9), then vslot `0x810` -> `0x800` + `0x764( id, 1, eb0 )`, else `0x804` + `31fd32e0`.
- `31fd32e0` tables at `33472040` are 50.. / 51.. / 60.. / 61.. (A, hyper A, B, hyper B): March's `aNormal` / `aHyper` / `SlotB` arrays by name.
- `32ccf4e0`: the Strong Mind rates are type 5 / 7 / 14-16 -> `+8` and type 6 -> `+0xc`. The timer is 3.0 (`33436700`), the restore uses `+0x14` x MP, and the text sits at y + 50.0 (`33449120`, AT_CENTER = 2). Open Mind is `322cc1a0` ? `0x95` : `0x94`.
- `31fc86e0` sets `+0x90d3 + slot` for slots 0-9 (both branches write 1).

**3. `32cce4b0` frame move.**
- The `+0x95f0` fade is min( t, 0.33 ) / 0.33 (`3351b030` float / `3343bd70` double).
- The UFO test `31fdbf50( 22020 )` is `22020 != 31fdbcc0( 22020 )`, which is the same test as the port's `GetJuneTranscendenceMemoSkillID`. The state switch skips the 20 slot states (50-69) and Hell Drop 187-196.
- The MP-charge, staff and hammer blocks match, and so does the `+0x9754` <- Now `+0x8160` copy.

**4. State start and actions.**
- `32ce7480`: the nine states are ids 162-165 / 174-176 / 180 / 200. ABM_COMBO_Z1-Z3 and DARK_MAGICIAN_DASH_COMBO_Z2a register under the same ids, so the port's names cover them.
- The Hell Drop timer covers 187-195 (not FIRE), with `+0x974c` = 1.0. The empowered cast is ( spec A `+0x6c50` + 1.0 ) x 0.7.
- `32ccf9f0`: the cases are 2 / 3 / 9 / 0x14-0x16 / 0x24, the same as March's `UAI_*` values. March's highest is 0x1d, so June's 0x24+ ids collide with nothing.
- `32008e40`: in case 0x29, Arme's vslot `0x854` is the empty `ret` stub `31f3f070`, so the port is right to leave it out. Vslot `0x11c` is `SetForceInvincible`. The buff factors are 824 / 825 / 827.

**5. `32ccb830`.**
- The nine skill ids and the state ids 186, 217 / 218, 221 / 222, 207, 209 / 210, 187-196, 246-248 match. 218 / 222 are June's hyper copies registered under the LOOP names (`32cee46d`, `32ceebc0`), which are March's `_HYPER_LOOP` enumerators.
- Meditation reads SA 43 (`SA_MP_REGENERATION_ABS`).
- June reaches the first seven blocks through `31fd88f0`, which is `&m_StateList[ id ]` (insert). The port finds the state instead (`32028190`'s form). The two differ only for a state that is not registered, where June would insert an empty one. Kept.

**6. Owners of the Done note's gaps.** All are real.
- **CH1:** gap 1, `31fd7640` (the user-base test behind vslot `0x828`, with the `+0x7d04` member). Gap 6, the Future -> Now copy of `+0x7ec0` / `+0x8160`. Gap 7, NPC `+0x874c`: in `32ccdae0` it is a "skip if non-zero" gate, so CH1 identifies it while wiring vslot `0x860`. Gap 8, the dirty-state batch `3207c1a0` / `31fdf970` around `SetEquippedSkillLevel`. Also the item-type anim array choice (equip `+0x544` / `+0x584`) of gap 2.
- **Stage 17:** the riding pet half of gap 2 (`32d2eb50`).
- **CH2b:** gap 3, the transformed view-list queue `3207b5d0`.
- **CH5:** gap 4, the cut-in item override `31fd8a50` (`+0x8e58`) and the event `0x8b3` swap.
- **Stage 6:** `June_UnitFlag3c`.
- **CH4:** gap 9.
- **Drift -> Stage 22:** gap 5. It is the `CX2DungeonManager` (g_pData `+0x44`) per-dungeon 15-bit attribute set (`31edf8f0` -> `327d9df0`: map find, data `+0x10`, `327e6d20( bit )`). Bit 13 refuses the common skill 900 in that dungeon, for every class. Its partner is `3202d140`'s PVE test (game type 4, absent here).

**7. Skill 900.** Game type 4 is June's PVE type, which March lacks; game type 2 is `GT_DUNGEON`. Both checks follow X2Unit.cpp:5170's precedent.

**Hygiene.** CH1i commit, `flagcheck.py JUNE_CHAR_ARME_CORE --range 4da003e7^..4da003e7`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 31 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The 31 deleted lines are CH1a / CH1b / CH1h stand-ins, as the Done note says.
No code change, so no build. The CH1i deploy (`md5 15bef827...`) stands.

Records: "From the CH1i review" notes in `CH1.md`, `CH2b.md` and `CH5.md`; a Stage 22 bullet in `JUNE_PORT_BACKLOG_ORDER.md`. No JUNE_BUGS row. **In game:** nothing (inert). Next: CH1.

### CH1 split (2026-10-21, user)

CH1 started on Opus and found the wiring larger than its kickoff: June moved ~20 common state callbacks (damage small / big, dash jump, START / WIN / LOSE)
from each class into CX2GUUser, and five registered Arme callbacks were never transcribed. The user chose to split CH1 behind a second flag,
`SERV_IRUHADEV_JUNE_CHAR_ARME_LIVE`, which stays undefined until the last sub-phase. Until then the wiring is compiled but never runs, so the
one-deploy-unit rule holds: the C++ and June's script go live together in the final CH1.

| Sub-phase | Run with | Scope (kickoff items of `CH1.md`) |
|---|---|---|
| CH1j | Opus 5.5 high, Opus review | Item 1: June's `InitState` / `InitStateCommon` registration, the hyper-mode table name, the dash-jump base callbacks, the LIVE gate |
| CH1j2 | Sonnet 5.5 high, Opus review | The five never-transcribed callbacks CH1j left empty or on a stand-in |
| CH1k | Opus 5.5 high, Opus review | The six June CX2GUUser base callbacks CH1j left on stand-ins; item 2 (`SetEquippedSkillFuncBySkillSlotIndex` head and its CH1 helpers) |
| CH1l | Opus 5.5 high, Opus review | Item 3 (the virtuals, under LIVE); the CH1i-review gaps; the CX2GUUser hoists of the CH1i notes |
| CH1m | Opus 5.5 high, Opus review | Item 4 (7ag pressed-index space); item 5 (hooks and the 7 June-only script keys) |
| CH1 (final) | Opus 5.5 high, Opus review, then user play-test | Define LIVE; item 6 (adopt the script, scans); pack, copy, build, the play-test and the Done gate above |

### CH1j - Done (2026-10-21, Opus)

**Landed** (flag `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE`; the call sits behind the new, undefined `SERV_IRUHADEV_JUNE_CHAR_ARME_LIVE`):
- `June_InitStateCommon` (`32ce77b0`) and `June_InitState` (`32cea0c0`), whole, at the end of `X2GUArme_VioletMage.cpp` (block `//{{ ... CH1j`).
  `InitState` runs `June_InitState()` under LIVE, March's body in the `#else`.
- **How the registration was derived.** `june2017_re/charport_regsim.py` walks each decompile once per job class (2, 14, 15, 23, 104, 105, 113), taking the
  branch each class takes at every class test (switch, `if`, the copied class register, InitStateCommon's templet test). Its 830 registrations
  (`char_arme/ch1j_reg_by_class.tsv`) reach every one of CH0c's 167 `ch0c_reg.tsv` rows and add none. The code keeps June's order: a registration
  reached by every class is plain, the others sit in a `switch( eClass )` with the classes that reach them.
- **State ids by name.** June's numeric ids map to March enumerators through March's own registration of the same table. Two exceptions: 177 is
  `AVSI_DASH_COMBO_Z3up` (CH1f's choice), and 237 gets a new enumerator `AVSI_DIMENSION_WITCH_DASH_ATTACK_X`, since June registers the Dimension
  Witch dash attack X twice (178 and 237). June-only ids keep the enumerators that CH1b-CH1i appended. The numbers stay March's, because offline nothing
  reads them.
- **Callbacks.** 227 targets resolve to `June_*` members (header `/// June <addr>`). 125 resolve to March members, or to the new CX2GUUser ones below.
  Each CX2GUUser target pairs with March's registration of the same table and slot, and every pair holds over all its states (funcmap rows
  `Char CH1j`). 13 targets are left empty or on a stand-in (see "Left for later"). Hooks replace three blocks:
  `JuneInitBoundingStates( "AVSI_" )` (ids 120-122, 7l), `JuneInitWaitStateChange( "AVSI_WAIT_STATE_CHANGE" )` (0x82, 7h), and WIN's OnEventProcess
  = `JuneRandomLoseEventProcess` (7h). The die states' StateEnd `3202a7d0` is left empty, because `CX2GUUser::StateChange` already runs it for DIE_FRONT / BACK.
- **New CX2GUUser pieces:** `JuneGetHyperModeTableName` (`31fdd490`, whole; its transcended branch is off until Stage 6, so Aisha gets `AVSI_HYPER_MODE`),
  `JuneDashJumpStartFuture` / `Start` / `FrameMoveFuture` / `EndFuture` (`31fc7f50` / `31fcbdf0` / `31fd2520` / `31fc7f80`), `JuneDamageBigStart` (`31fc7fd0`).
  The members `+0x8fb8` / `+0x8fbc` move up from the Arme class as `m_fJuneDashJumpDownAccelRate` (1.2) / `m_fJuneDashJumpSpeedRate` (1.6). Aisha's
  constructor sets them to 1.1 / 1.2. June's dash-jump FrameMoveFuture has no down-force branch, and both keys held counts as forward.
- `32cb7110` = March `HyperModeInit` (same six textures). `32ce1ba0` = `June_LoadAngorHammerMesh` (major mesh player `AppendToDeviceList` of
  `Mesh_AVP_AngorHammer`). The tail is June's: `InitPassiveSkillState`, `InitEquippedActiveSkillState( false )`, `CX2GUUser::InitState`.

**Left for later sub-phases (stand-ins, inert while LIVE is off; funcmap `ROUTED`):** CH1k ports `31fcbed0` / `31fcbf80` (damage small / big
EventProcess; they call `SpecialAttackEventProcess( 7 )`, a use condition March lacks), `31ffbd70` / `3201b390` (START), `320004b0` (WIN Start) and
`320007e0` (LOSE Start). CH1j2 transcribes `32cbca20` (HM COMBO_X2 EventProcess), `32cc3040` (AHM_DASH_JUMP_ZXX_FINISH FrameMove), `32ce7170`
(AEM_DASH_JUMP_X3 FrameMove), `32cc42a0` / `32ce2cc0` (AVP_DASH_ATTACK_X2). CH0c's map rebuild attached these five to states after CH1e / CH1f had run.
**Not registered (drift, routed):** the riding June-only states 93-96 and the `AVSI_RIDING_ATTACK_X` FrameMove `32002f70` go to Stage 17. The dance
states 127-129 go to Stage 10 (a bullet is added). The phoenix-run states 123-126 are PvP's amusement mode, and there is no PvP offline.
JUNE_BUGS 379: June registers `AVSI_RIDING_DAMAGE_BACK`, but June's script has no table for it.

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --worktree --files <5 files>`: `files: 3 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 2 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`. OTHER is the LIVE gate in `InitState`. The deleted lines are CH1h's two Arme members and their constructor lines, now hoisted. For LIVE,
flagcheck reports `FAIL: the flag is not #defined anywhere tracked`: that is by design, and its check runs in the final CH1. A `cl /Zs` probe of
both changed TUs passed before the build.

**Deploy:** `DEPLOY OK: X2_offline.exe is 25,668,608 B, dated 2026-10-21 05:27:23, md5 a7ed9dd0fda350ebbce60c664b46e305, equal to X2/US_SERVICE/x2.exe (2026-10-21 05:27:16); built at main e93a6cb6 + 5 uncommitted code file(s).`

**In game:** nothing changes (LIVE is undefined). Next: `/x2-phase pilot-char-revamp CH1j review` (Opus 5.5 high). CH1j2 may follow the review.

### CH1j -- Review (2026-10-21, Opus)

**Verdict: PASS, one comment fixed.** Read against fresh Ghidra decompiles and listings of `32cea0c0`, `32ce77b0`, `31fdd490`, `31fc7f50`,
`31fcbdf0`, `31fd2520`, `31fc7f80`, `31fc7fd0`, `3202a7d0` and six CX2GUUser pairs.

**1. Path simulation.** `charport_regsim.py` on fresh decompiles of both registrars reproduces `ch1j_reg_by_class.tsv` exactly (830 = 830 rows, no diff).
The new `june2017_re/char_arme/ch1j_review_regcheck.py` parses the C++ per class and matches every June row to the C++ registration of the same table,
slot by slot, through funcmap and the header's `/// June` comments. Everything matches except the routed gaps listed in item 5. Only two orders differ,
VP `AVP_ZZfrontZZ_FALL` and DW `ADW_COMBO_ADD_X` / `_X2`; they are distinct ids, so the result is the same.
- `@32ce8eb0`: unit `+0xaf0` (templet) `+4` = `m_UnitClass` (X2Unit.h:291), compared with 0x17 / 0x71; a NULL templet takes the non-ABM path, the
  same as the port for any real unit.
- `@32ceae8b` / `@32ceae9b`: class - 15 == 0 and class - 105 == 0 both jump to `32ceb027`, and 104 goes to `32ceaf6d`, so the Void Princess shares the DM block.
- `@32ceb6bb`: classes 14 / 104, 105 and 113 have blocks (232-236; 178 + 179; 237, 183, 239, 240). The default (2, 15, 23) goes to `32ceb989`. Each class's last write is June's.
- The head matches: the cut-in index switch and the sub-index chain. `31fc93d0` is an empty stub.

**2. Ids.** The id -> enumerator map is one-to-one over all 127 ids. 72 = `USI_RIDING_START`. 177 is DM / VP `AVSI_ADM_DASH_COMBO_ZXX` in March's
`AVSI_DASH_COMBO_Z3up` slot (March X2GUArme_VioletMage.cpp:1231). 237 is DW only and is appended inside the CORE enum block. The shared ids hold:
166 = `COMBO_Z4up` / `ADM_ZZupZZ` / `AEM_ZZupZZ`, 168 and 171 = the three COMBO_X2 / X3down tables, 178 = `DASH_ATTACK_X` (DW: its own table),
183 = `DASH_JUMP_ATTACK_X` / `ABM_` / `AVP_DASH_JUMP_X`.

**3. CX2GUUser pairs (June body vs March body).** `31fcc0e0` DamageStandUpEventProcess, `31fc8050` HyperModeStartFuture (`+0x9176`),
`32030580` RidingOnEventProcess (every branch and state id), `31fcc2b0` DamageRevengeEventProcess and `31fcc260` DamageAirFlyEventProcess are
identical. `329cc1f0` DashStartFuture is the vcall thunk to vslot `0x5fc`, the same as March's virtual (Arme overrides go to CH1l). `31fd23e0`
WalkFrameMoveFuture has no Ghidra function and was not read; the other five stand in for it.

**4. New members.** `31fc7f50`: `+0x8bf8` = `m_bDownForce`, `+0x314` = `fDownAccel` = `+0x8fb8` x `+0x31c` `fGAccel`. `31fd2520`: both keys
held, or the key matching `+0x80f6` bIsRight (Now) -> `nowSpeed.x` = dash speed x `+0x8fbc`; no down-force branch. `31fc7f80` tail
vslot `0x7dc` = `CommonStateEnd` (March's Arme copy calls the same). `31fcbdf0` / `31fc7fd0` = CommonStateStart + `0x7ec` CreateStepDust.
`31fdd490` is identical: type 11 shares RSI, the 37-case transcended table, CX2Unit `+0x3c`. Arme's constructor sets 1.1 / 1.2.

**5. What was left out.** All 13 slots have owners: CH1k (`31fcbed0`, `31fcbf80`, `31ffbd70`, `3201b390`, `320004b0`, `320007e0`), CH1j2 (the five
in its kickoff, which still matches), and Stage 17 (`32002f70`, ids 93-96). Stage 10 has the dance states, and the phoenix run is PvP. **Fix:** June also registers
`3202a7d0` as `AVSI_RIDING_DIE`'s StateEnd, but `DoStateEndStart` (X2GUUser.cpp:7097-7117) emulates it only for DIE_FRONT / BACK, so
the C++ comment there was wrong. It now says so, and the riding die is added to the Stage 17 riding-states bullet (the gap is common to every class).

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --range 3d0a3c3c^..3d0a3c3c`: `files: 3 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 2 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`. OTHER is the LIVE gate; the deleted lines are CH1h's members (2 in the .h, 2 constructor lines in the .cpp). The comment fix:
`--worktree --files X2Lib/X2GUArme_VioletMage.cpp`: `1 PASS`, `1 deleted line(s)` (the replaced comment), `RESULT: PASS`.

**Deploy:** none. The fix changes one comment line and leaves the line count the same, so the code CH1j deployed (md5 `a7ed9dd0...`) is unchanged. No build was queued.

**Next:** `/x2-phase pilot-char-revamp CH1j2` (Sonnet 5.5 high). It should re-run `ch1j_review_regcheck.py` after it repoints the five slots.

### CH1j2 - Done (2026-10-21, Sonnet 5.5)

Inert transcription, flag `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE`. **Five new `June_*` members** (block `//{{ ... CH1j2` at the end of `X2GUArme_VioletMage.cpp`, declared after
the CH1j block in the header), and `June_InitState` now registers them. None of the five has a Ghidra function; all were read from `funcdis.py` listings.

| June | Member | Replaces |
|---|---|---|
| `32cbca20` | `June_AVSI_HIGH_MAGICIAN_COMBO_X2_EventProcess` | the stand-in (March's `AVSI_HIGH_MAGICIAN_COMBO_X2_EventProcess`) |
| `32cc3040` | `June_AVSI_AHM_DASH_JUMP_ZXX_FINISH_FrameMove` | empty slot |
| `32ce7170` | `June_AVSI_AEM_DASH_JUMP_X3_FrameMove` | empty slot |
| `32ce2cc0` | `June_AVSI_AVP_DASH_ATTACK_X2_FrameMove` | empty slot |
| `32cc42a0` | `June_AVSI_AVP_DASH_ATTACK_X2_EventProcess` | empty slot |

- **`32cbca20`** is not the Dark Magician's `32cbd120`: it has a fifth window (`m_fEventTime[ 4 ]`, `oneZ` -> `AVSI_AEM_XZ`), and the back-X3 (window 2) and XZ (window 4)
  chains exist only for the Elemental Master (`[[user+0x867c]+0xaf0]+4 == 0x68`; a High Magician gets X3down / X3front / X3 only). The tail is `32cbcff0`
  (12 keys after 0.966 s) and `+0x96f0` (`m_bCanChangeState`) while flag 3.
- **`32cc3040`** only evaluates `EventTimer( 0.27 )` and drops the result, then `CommonFrameMove`.
- **`32ce7170`**: 0.16 s timer, `June_FlushMpWithRefund( 4 x rate )`, combo fireball type 1 at `Dummy2_Lhand` with rotation z -30; failure ( 0, 0, -30 ).
- **`32ce2cc0`** is `32ce2b20` (ADM dash-jump ZX) with `FlushMp( 10 )`, `Dummy2_Lhand` and the rotation unturned. **JUNE_BUGS 380:** the Void Princess fires `DamageEffect_ADM_DashJumpZX`.
- **`32cc42a0`**: fall -> `USI_JUMP_DOWN` (y -45), animation end -> WAIT, `SKILL_CANCEL_AFTER( m_fSkillCancelAfter )`, then walk / bwalk / dash / Z X cancels (ground-only, as in CH1f).
- funcmap: the five `ROUTED` rows are now `PORTED` (`Char CH1j2`). `ch1j_review_regcheck.py` prints no MISSING for the five; the 35 left are `31ffbd70` (CH1k), `32002f70` and `3202a7d0` (Stage 17 / base, routed in the CH1j review).

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --worktree --files X2Lib/X2GUArme_VioletMage.cpp X2Lib/X2GUArme_VioletMage.h`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 5 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
The 5 deleted lines are CH1j's own stand-in line and four "CH1j2 transcribes" comment lines in `June_InitState`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 25,672,704 B, dated 2026-10-21 06:28:39, md5 744259defe74a2c43e46cbfcd7e33a0a, equal to X2/US_SERVICE/x2.exe (2026-10-21 06:28:33); built at main ea0b49ea + 2 uncommitted code file(s).`

No drift, no new branch found. **In game:** nothing (the members are unreferenced until `SERV_IRUHADEV_JUNE_CHAR_ARME_LIVE`). Next: `/x2-phase pilot-char-revamp CH1j2 review` (Opus 5.5 high); CH1k follows the review.

### CH1j2 -- Review (2026-10-21, Opus)

**Verdict: PASS, no code change.** All five read instruction by instruction from `funcdis.py` listings (none is carved in Ghidra), with constants
from `charport_consts.py` (operand width) and strings decoded from the image.

1. **`32cbca20`** (tail `32cbcff0`). Fall -> `USI_JUMP_DOWN`, y -45 (qword `33472998`). `SKILL_CANCEL_AFTER( 0.01 )`: both halves, including the
   `+0x8d4c` hyper latch and `32070d60`, are the AWAKE_LOCK macro. Windows `+0x8c10 + 12 * i`: start / end / stateChange are `+0 / +4 / +8`, and the flags
   are `+0x7d85..+0x7d89` = `bEventFlagList[0..4]`. The due test is `now > stateChange || animEnd`. Ids 0xab / 0xaa / 0xad / 0xa9 / 0xc6.
   Input bytes against `InputData` (X2GameUnit.h:170, base `+0x86da` = pureLeft): oneX `+0x86ef`, oneZ `+0x86eb`, pureDown `+0x86e6`, forward / back from
   `+0x7e56` bIsRight. Windows 2 and 4 test `[[+0x867c]+0xaf0]+4 == 0x68`, i.e. `m_pUnitTemplet->m_UnitClass` = `GetUnit()->GetClass()` (X2Unit.h:689),
   class 104. The 12 keys (`+0x86e2 .. +0x8712`) feed the tail: `now > 0.966` (a qword equal to `(double)0.966f`), then ground ? WAIT : JUMP_DOWN; at or
   below 0.966, nothing. Anim-end WAIT reads `[+0x87d0]` directly. `+0x96f0 = 1` while flag 3, then CommonEventProcess (vslot 0x7d4). All match.
2. **`32cc42a0`**: fall, then anim-end WAIT (ground only), `SKILL_CANCEL_AFTER( +0x8c90 )`, WALK `+0x8c98` (either arrow), BWALK `+0x8c94`, DASH `+0x8c9c`
   (pureDouble `+0x86dc` / `+0x86e0` -> vslot 0x668), Z / X on `+0x8c88` = `m_fAttackCancelAfterEx` with oneZ / oneX (vslot 0x4b0 / 0x4b4).
   Every cancel tests `+0x7e1c` (ground) and `0 <= t`. The order is the listing's.
3. **`32ce2cc0`** vs `32ce2b20`: 0.27 oneshot; vslot 0xe4 `( 10.0, 0 )`, which is plain `FlushMp`, not the refund helper; bone `3345bb08` = `Dummy2_Lhand`
   through `m_pXSkinAnim` (`+0x87d4`), not `TRACE_START0`; the vslot 0x150 rotation goes in untouched; power `+0x9640`, range `+0x95c8`, land y `+0x80c4`;
   `3351ee64` = `DamageEffect_ADM_DashJumpZX`. The failure path is `32007d80( pos, 0, 0, 0 )`. JUNE_BUGS 380 stands.
4. **`32ce7170`**: 0.16 (dword) oneshot; `32cc2b90( 4.0 * protected +0x95bc )`; type 1 is stored in the local the helper receives by pointer; z -30 (`3350ad60`);
   failure `( 0, 0, -30 )`. The function ends at `32ce72ee`: the 0.3 / 5.0 / 180 constants `charport_consts.py` prints belong to the merged neighbour.
5. **`32cc3040`** is June's inlined `EventTimerOneshot( 0.27 )` (`+0x24c` = `m_fAnimOneshotTimer` guard, KTDGXSkinAnim.h:258) with the result dropped,
   then CommonFrameMove (vslot 0x7cc). The C++ calls `EventTimer( 0.27f )` alone. Both are pure queries (Ghidra `31d28e10` reads `+0x21c` / `+0x214` and
   writes nothing), so the outcome is the same; noted in funcmap, no code change.

**Ghidra spot-check** (read-only; the five are not carved, so the callees were checked instead): `31d28e10` EventTimer
(`before < t <= curr`, no writes); `32cc2b90` is vslot 0xe4 `( mp, 0 )` plus the `+0x96ac` refund; `32007d80` takes `( pos, x, y, z )` =
CreateNotEnoughMPEffect. All agree with the transcription.

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --range e0ddb88e^..e0ddb88e`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 5 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`. The 5 deletions are the stand-in line and four "CH1j2 transcribes" comments. `ch1j_review_regcheck.py`: none of the five is MISSING (35 left, all routed).
No build: nothing changed in code. `CH1k.md` checked: it still matches (depends on this review; scope unchanged). No drift found.

### CH1k - Done (2026-10-21, Opus)

**Landed** (flag `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE`; calls into the new code sit behind the undefined `SERV_IRUHADEV_JUNE_CHAR_ARME_LIVE`). Funcmap rows `Char CH1k`.
- **Six CX2GUUser base callbacks**, registered in `June_InitState` in place of CH1j's stand-ins:
  - `JuneDamageSmallEventProcess` / `JuneDamageBigEventProcess` (`31fcbed0` / `31fcbf80`): Elsword's March body, plus a skill use with condition 7 before the animation-end test.
  - `JuneStartStateStart` / `JuneStartEventProcess` (`31ffbd70` / `3201b390`).
  - `JuneWinStateStart` / `JuneLoseStateStart` (`320004b0` / `320007e0`): March's pet reactions, then `m_bDeleteEffectSetOnStateEnd` = true and `SetForceInvincible( state +0x218 + 5 )`. In `DI_EVENT_2017_TREASURE` (41470), WIN also plays the five `Clear_*` sequences on the game major particle system.
- **Condition 7.** June's `CheckSkillUseCondition` (`31fca9f0`) has `ASUT_DAMAGE` (6) and `ASUT_DAMAGE_EXCEPT_AIR` (7, damage states, ground only).
  - Both are added to March's enum, with June's case in the live (`ADD_MEMO_1ST_CLASS`) overload.
  - Cases 1-3 are unchanged, and no caller passes 6 / 7 until LIVE.
- **`June_SetEquippedSkillFuncHead`** (`32cede00` up to its skill switch). Under LIVE, March's `SetEquippedSkillFuncBySkillSlotIndex` calls it in place of March's head and type switch (`#else` keeps March's).
  - What the head does: bounds 0..4, then March's four slot arrays (the same as June's tables `3351b05c` / `3351b170` / `3351b184` / `3351b198`).
  - The transcendence branch (`GetJuneTranscendenceMemoSkillID`, only 22217 registers; inert until Stage 6).
  - Type cases: ST_BUFF, ST_RELATIONSHIP for 900, and the active types on `32207210( id )`. In those, 22600-22605 are the three hyper actives and `> 22600` is Power Attack. Anything else returns to March's skill cases (keyed on `eSkillID_` until CH2a; June keys on the base id).
  - The skill switch's `default:` takes June's Power Attack members under LIVE.
  - Dropped: `31fd2af0` (7i, the engine's), and `+0x11 = 1`. `+0x11` is June's slot-state byte (user slot 330, `3208e430`), which `IsJuneSkillSlotState` answers. Checklist 7i is updated.
- **`JuneGenericActiveSkillEventProcess` / `JuneGenericSpecialActiveSkillEventProcess`** (`31fcc9d0` / `31fcc740`), which the head registers. Each is March's body plus June's cancel chain: SKILL_CANCEL_AFTER, then WALK / DASH / BWALK, each on ground.
- **The 19 sub-state members** of the class map's CH1 "helpers" (inert; CH2a-CH3b register them with their skill cases):
  - the Meteor / Lightning Shower loop and end pairs (`32cc6fb0` / `32cbfb70`, `32cc7200` / `32cc2110`); the ends carry June's cancel chain;
  - the Guillotine Press loop `32cc10f0`, Circle Flame `32cbeee0` / `32cbef40`, and the Ruin loop `32cc5990` (March has no body; `+0x97a4..+0x97b4`);
  - Hell Drop: the control FIRE EventProcess `32cb7ce0` and the ten control StateStarts `32cc74a0`..`32cc79b0`.
  - The Hell Drop starts drop March's memo test, because June's `GetEqippedSkillMemo` (`31fca9e0`) always returns false.
  - `32cf1b10` is std::deque library code (n/a).
- **CH1j fix:** `InitState`'s LIVE branch had no closing brace. A `cl /Zs` probe with LIVE defined found it, and one inserted `}` fixes it.

**Decisions (inert; say if you want otherwise):**
1. CX2State `+0x218` (June's result-UI time, `31ef7be0`, not in March) reads as its absent value 0, so WIN / LOSE invincibility lasts 5 s.
2. START's PvP best-of-N round particles are not ported (no PvP offline).
3. June InputData `+0x8726` (a key March lacks) is `false` until CH1m names it.

**Routed:** `3200ac50` (code event 2143 START summon) gets a Stage 19 bullet. The state batch `3207c1a0` / `31fdf970`, which also wraps this function, goes to CH1l, as does the virtual `COMMON_BUFF_FrameMove`. `+0x8726` goes to CH1m. Kickoff notes are added.

**Hygiene.** `cl /Zs` of both TUs passes with and without LIVE (flags: AWAKE_LOCK, SKILL_CANCEL_BY_HYPER_MODE, FINALITY, RELATIONSHIP, UPGRADE_2013, MEMO, CODE_EVENT, PET, SLOT_10 all defined).
`flagcheck.py JUNE_CHAR_ARME_CORE --worktree --files <5 files>`: `files: 4 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
- OTHER is the LIVE gate: the head wrap, the Power Attack default and the InitState brace.
- The 8 deleted lines are CH1j's stand-in registrations inside `June_InitState`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 25,680,896 B, dated 2026-10-21 07:41:01, md5 ea0b3d7cd8a18f1d99625476ba1ddad2, equal to X2/US_SERVICE/x2.exe (2026-10-21 07:40:54); built at main 1587f86f + 5 uncommitted code file(s).`

**In game:** nothing changes (LIVE is undefined). Next: `/x2-phase pilot-char-revamp CH1k review` (Opus 5.5 high).

### CH1k -- Review (2026-10-21, Opus)

**Verdict: PASS, no code change.** Every function was read against its Ghidra decompile. `31fcc740` does not decompile, so it was read from the
`disasm_range.py` listing. Each head registration was resolved thunk by thunk (`jmp` targets read from the image). The float constants were read from
the image: 200, 45, 0.93, 5.0, 1000, -1, 1.0, 4.5 and 0.

1. **Damage small / big** (`31fcbed0` / `31fcbf80`). The `> 200` test runs against land y; then vslot 0x7f4( 7 ) before the anim-end test; fall ->
   `0x22` (June's id of `USI_DAMAGE_AIR_SMALL`, which is 0x21 in March: the match is by name, and CH1 numbers the ids) with y -45; ground -> WAIT.
   The big one's skip uses the 14 bytes `+0x86da..+0x8712` (no `+0x8702`), the same 14 keys as `ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC`, with `> 0.93`.
2. **START** (`31ffbd70`): everything after CommonStateStart sits under `IsPvp` (`322cc180`) and state `+0xd8 == 10`, so leaving it out is exact
   offline. **`3201b390`**: CommonEventProcess first; code 0x85f = 2143 (map `+0xa18`, entry `+0x10`), host, `> 1.0` -> `3200ac50`; `>= 4.5` -> WAIT.
3. **WIN / LOSE** (`320004b0` / `320007e0`): the pet Happy1 / Happy2 (`+0x1e0` / `+0x1e4`, `rand` even) and Sad `+0x1f0`, then `+0x8625` = 1, then
   vslot 0x11c( state `+0x218` + 5.0 ). The dungeon test is vslot 0x1c4 == 0xa1fe (41470). The five `Clear_*` calls go to `31cf90f0` with
   ecx = `[g_pData +0x84]` and arguments `( 0, name, 0, 0, 0, 1000, 1000, 1, 1, -1, true, 0, false, -1 )`. March's defaults cover all of them except
   June's 14th argument, which March's signature does not have.
4. **`31fca9f0`**: case 6 / 7 rejects templet 3 / 4; templet 7 tests `+0x7e1c` (the same byte March's ANYTIME case reads as `IsOnSomethingFuture`);
   any other templet must equal the condition. The port's `break` reaches March's `return true`, so the result is the same. `enumprobe.py`:
   ASUT_DAMAGE 6, ASUT_DAMAGE_EXCEPT_AIR 7. No live caller passes 6 / 7: the only callers are the two new damage callbacks, registered under LIVE.
5. **Head `32cede00`**:
   - It checks the bounds 0..4 before `3207c1a0`. It reads the A / B normal tables (`3351b05c` / `3351b184`) and the hyper tables (`3351b170` / `3351b198`).
   - It sets `+0x11` on the normal state and `0x101` on the hyper state (`m_bHyperState` + the slot byte).
   - `31fd2af0` is the engine's: `JuneSnapshotBeforeSlotRebind` (X2GUUser.cpp:7017) calls it before the class function.
   - Transcendence: `31fdbcc0`, then `32206450( tid, 1 )`. When `32207210 == 0x56c9` it registers Init `32cb8450`, FrameMove `32cdfb70` and
     EventProcess `329cba60` -> `31fcc9d0`, and the hyper state copies them. Then it returns.
   - The type cases are 5 / 6 / 7 / 0xc / 0xd-0x10, which are `ST_BUFF` / `ACTIVE` / `SPECIAL_ACTIVE` / `RELATIONSHIP` / `HYPER_ACTIVE` / `GRADE_1-3`
     (`enumprobe.py`). The buff thunks go to `32a77a20` / `31fcbd60`; the item-900 thunks to `329cc0d0` / `329cc170`.
   - Elemental Storm, Abyss Angor and Fate Space: Init `32cb8650` / `32cb8840` / `32cb89b0`; their EventProcess `329cbfc0` -> `31fcc740`.
   - The inner `default:` and the skill switch's `default:` both register Power Attack, from `32cb7570` / `32cbe620` / `32cb7580` / `32cbe660`.
     They match the `June_*` members' funcmap rows.
   - Under LIVE, March's `#else` keeps March's head.
6. **`31fcc740` / `31fcc9d0`** (listing): anim end -> JUMP_DOWN / `m_Wait` (`+0x4088`); `31fcc9d0` adds SKIP_TIME (default -1) and the 14-key skip;
   then SKILL (`+0x8c90`, ground, 0x7f4( 1 )), awakening (blackhole, oneLCtr `+0x8723`, count, 0x560, `32070d60`, `+0x8d4c` = 1), WALK `+0x8c98`,
   DASH `+0x8c9c` (vslot 0x668), BWALK `+0x8c94`; each on `+0x7e1c`. Same order as the macro chain.
7. **The 19 members**: every one is the callback June registers for its state:
   - Meteor LOOP `32cf1a30` -> `32cc6fb0`, END `32cf16a0` -> `32cbfb70`; Lightning `32cc7200` / `32cc2110`;
   - Guillotine LOOP `32cc10f0` (0xd1 / 0xd2); Ruin LOOP `32cc5990` (END 0xf7); Circle Flame 0xba StartFuture `32cbeee0`, EventProcess `32cbef40`;
   - Hell Drop: IDLE `32cc74a0` .. FIRE `32cc79b0`, FIRE EventProcess `32cb7ce0` (-> 0xbb).
   - The loops' key list `31fc7ac0` is `+0x86f2..+0x86fe`, `+0x8706..+0x8712`, `+0x872a`, `+0x872e`: March's eight keys plus Skill5 / SkillEx5.
   - In the shower END, June's Z / X / walk / dash tests run in the airborne branch with a ground test, so they are dead code in June too. The port keeps them.
   - Ruin: the five floats `[0]..[4]` and the `[1] <= [3] + [2]` tick match. Its anim test reads `+0x87d0` directly.
   - `31fca9e0` is `return 0`, so dropping the Hell Drop memo test is exact.
8. **Decisions.** `+0x218` is written only by `31ef7be0( mode, t )` when `t > 0`; offline nothing writes it, so 0 and 5 s are June's absent value. Agreed.
   `JUNE_ARME_INPUT_8726` = false: CH1m names the key (its kickoff note is there). Agreed.

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_CORE --range 17dddf23^..17dddf23`: `files: 4 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`. The OTHER row is `X2GUArme_VioletMage.cpp` (LIVE: the head wrap, the Power Attack `default:`, the InitState brace). Its 8 deletions are CH1j's own
stand-in lines (four damage lines, the START comment and stand-in, WIN, LOSE), replaced in place. No build: no code changed. `CH1l.md` checked: its
"From CH1k" notes (the batch around the head, the virtual `COMMON_BUFF_FrameMove`) stand. No drift found.

### CH1l - Done (2026-10-21, Opus)

The virtuals are wired under `SERV_IRUHADEV_JUNE_CHAR_ARME_LIVE` (still undefined), plus two inert CX2GUUser hoists under `..._ARME_CORE`. The commit is "Character pilot Phase CH1l".

**1. The virtuals (item 3).** 38 March overrides now run their `June_*` member under LIVE, with March's body in the `#else`:
- the lifecycle: ctor, dtor, `ReInit`, `InitDevice`, `InitComponent`, `InitEffect`, `InitEtc`;
- `DamageReact`, `Win`, `Lose`, `Draw`, `OnFrameMove`, `NotifyDGManagerChanged`, the particle arrays, `HyperModeBuffEffectStart`, `HyperModeFrameMove`, `COMMON_BUFF_FrameMove` (`32cc6e50`, CH1k's note);
- `Common*` x6 and `CommonRender_Prepare`;
- the 509 step: `SpecialAttackEventProcess` -> `June_SpecialAttackEventProcess`, which calls 510 / 511 directly;
- `SpecialAttackNoStageChange`, `NoStateChangeActionFrameMove`, `InitEquippedActiveSkillState`, `SetEquippedSkillLevel`, `CanRidingState`, `IsValideSkillRidingOn`, `ShowActiveSkillCutInAndLightByScript`, `ApplyRenderParam...`.

Three cases differ from a plain call:
- **Dtor:** the LIVE branch also deletes `m_pStrongMindData`. March's `InitPassiveSkillState` still allocates it until CH4.
- **Dropped March-only virtuals:** `ParseCommonRandomState` is pure virtual in CX2GUUser, so the LIVE body is empty (June rolls the variants in `June_InitState`). `ApplyAuraAcceleratorBuffToGameUnit` calls the base.
- **New Arme overrides** (header, LIVE):
  - `SetShowViewList` (0x63c), `UpdateEquipViewListInfo` (0x644).
  - `UpdateEquipCollisionData( vector& )` (0x638). Under LIVE the CX2GUUser vector overload is `virtual`, as in June; a `using` keeps the no-arg form visible.

**2. June-only user vslots and their March call sites** (each base is empty or March's code; LIVE hooks in CX2GUUser):
- **0x654** `JuneAfterEquipSlotChange`: `X2UIInventory.cpp`, `Handler_EGS_CHANGE_INVENTORY_SLOT_ACK` (June `321a29a0` @321a2c5d, the only one of five Rfoot sites with the call).
- **0x850** `JuneAfterImageFrameMove`: replaces March's after-image `OnFrameMove` in `CX2GUUser::CommonFrameMove` (June @3206c186). The base keeps March's call.
- **0x860** `JuneSummonedNPCInfo`: end of `CX2Game::SetUserSummonedNPCInfo` (June @3230cb1b).
- **0x610** `RidingOffEnd`: June registers a thunk to vslot 0x610 (`329cc060`), so `June_InitState` now registers `June_RidingOffEnd` directly. Nothing else becomes virtual.
- **Reserved skill (512)**: `CountReservedSkillUse` calls `JuneReservedSkillStart` before `ClearSkillUseReserve` (June @32057c4c-cb2). Arme's override runs `June_SpecialAttackCooldown( id, m_fJune8ea8 )` and `June_SkillMemoMpRefund( id, state byte +0x11, m_fJune8ea8 )`. June's vslot 0x810 test is not called (March's comment there holds).
- **No March caller (correct as is):**
  - 0x604 / 0x608 / 0x60c and 0x680 are called only from Arme code; 0x718 and 0x880 are Arme's own or HUD-only. 0x880 is not a CX2GUUser slot at all: the base vtable ends at 0x87c.
  - 0x718's other callers are `322cf810` and `322e1bb3`, which belong to the MemoRise HUD (`322f4740`, CH4).

**3. The CH1i-review gaps.**
- `31fd7640` is now `CX2GUUser::JuneIsNotCommonMoveState` (CORE). It returns false in these states:
  - wait, charge-MP (`+0x7d04` = `m_ChargeMpState`);
  - walk, dash, dash end, dash jump and its landing, jump ready / up / down / landing (vslots 0x438-0x45c resolved one by one);
  - the ten riding move states (June ids are March + 1 there).
  - vslots 0x520 (now == wait) and 0x578 (false) add nothing.
- `June_Vslot828` uses it. Its only June caller is `CX2GUUser::OnFrameMove` @3206f99d, which resets the June-only idle timer `+0x91a8 / +0x91ac` (CX2Game `323022b0`, text `0x4b1f`). That timer is not Aisha's -> drift, below.
- **Future -> Now:** `+0x7ec0 / +0x8160` is one 12-byte protected slot of the frame data, used by many classes. It is now `SyncData::iJuneClassSyncInt` (CORE, `KProtectedType<int>`), so March's frame copy carries it. It is off-wire, and the wire status is not read. `m_iJune7ec0 / 8160` are gone.
- **NPC `+0x874c` is `m_KeyCode`** (CreateNPC 3230d97d writes it next to `m_bIsActiveMonster` +0x872c). `June_Vslot860` now returns when it is non-zero.
- **The dirty-state batch** `3207c1a0` / `31fdf970` collects the state ids it edits. At the end it re-protects each entry with June's anti-tamper registry (`0x3376d8d8`, `31c594b0`, funcmap `31c59710`), which March does not have. One decision covers both callers: there is nothing to port.
- **The equip anim array choice** (`+0x544 / +0x584`) is not settled. It needs the June item-type layout, so it stays open for the CH1l review (item 1 below).

**4. Hoists.** Moved to CX2GUUser: the test above and the sync slot. Everything else from the CH1i list stays Arme-local, because only Aisha's code calls it (`m_iJune8ea0..8eb0`, `m_abJune90d3 / 90d0`, `m_bJuneAfterImageSaved`, `June_SetSkillBuffFactors`, `June_GetSkillLevel` and the others).

**Hygiene.**
- `flagcheck.py JUNE_CHAR_ARME_CORE --worktree --files` (Arme .cpp/.h, X2GUUser.h/.cpp): `files: 1 PASS, 0 PASS~, 0 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`; `2 deleted line(s)` (the two Arme sync members), `0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
- LIVE: `0 deleted line(s)`. Its only FAIL is the by-design "not #defined" (CH1j precedent).
- `cl /Zs` of X2GUArme_VioletMage / X2GUUser / X2UIInventory / X2Game: 0 errors with LIVE defined and with LIVE undefined.

Build: `DEPLOY OK: X2_offline.exe is 25,681,408 B, dated 2026-10-21 09:06:55, md5 21d5d641c6f8407c1eb25d2700351f48, equal to X2/US_SERVICE/x2.exe (2026-10-21 09:06:48); built at main cf8de0cb + 6 uncommitted code file(s).`

**Drift (scope rule):** June's idle timer `+0x91a8 / +0x91ac` (setter `3203e850` @32041ae3, reader CX2Game `323022b0`, reset by user vslot 0x828) is for every class -> Stage 9 (UI sweep) bullet. June's base 0x850 also toggles the after-image for class 13 -> that class's port (checklist 7). `+0x9440` (the MP the reserved skill spent, @32057ce2) is read only by `32b60650 / 32b63be0`, another class -> checklist 7.
**Open for the review:** (1) the `+0x544 / +0x584` anim-array choice; (2) whether 0x850's March placement (after the shadow block, not inside its effect-on branch) is acceptable.
**In game:** nothing (LIVE undefined). Review: `kickoffs/pilot-char-revamp/CH1l-review.md` (Opus).

### CH1l review (2026-10-21, Opus): PASS, no code change

- **38 LIVE wraps:** each calls its own `June_*` member with every argument in order and returns its result; every March body is whole in the `#else` (the commit's 10 deleted `.cpp` lines are all inside CORE stand-in code: `m_iJune7ec0 / 8160` -> `iJuneClassSyncInt`, `June_Vslot828`'s `false` -> `JuneIsNotCommonMoveState()`, the `June_RidingOffEnd` registration). Dtor `SAFE_DELETE( m_pStrongMindData )` before `June_Destructor()`, the empty `ParseCommonRandomState` and `ApplyAuraAcceleratorBuffToGameUnit` -> base: as described.
- **`JuneIsNotCommonMoveState` = `31fd7640`:** `+0x7d04` (`m_ChargeMpState`), vslots 0x438-0x45c, then June ids 0x4b-0x54 (75-84); March `USI_RIDING_WAIT_HABIT..USI_RIDING_DASH_JUMP_LANDING` count out to 74-83 with `JUNE_SKILL_SLOT_10` on, so June = March + 1 holds.
- **Hooks:** 0x850 @3206c186 sits inside the effect-on branch after the shadow update, exactly where March's call now is (open item 2 settled: the placement matches; the Done note's "not inside" wording was wrong). 0x654 @321a2c5f runs after 0x63c / 0x630 and before 0x634, the same order as `X2UIInventory.cpp`. 0x860 @3230cb1b is the last call of `SetUserSummonedNPCInfo`, as in March; the three June calls before it (`3204e160`, `3200bec0`, `31fd05c0` -> vslot 0x85c) are checklist rows 260 / 266 already. 512: June @32057c4c-cb2 = `0x800( GetNowStateSkillID(), +0x8ea8 )` then `0x764( id, 0x528( 0 ), +0x8ea8 )`, then 0x808 (clear), skipped as a whole when 0x810 is true: matches `JuneReservedSkillStart`.
- **`iJuneClassSyncInt` wire:** settled off-wire. Neither `SyncData::SetData` (`3201f5f0`) nor `KDYNAMIC_UNIT_USER_SYNC::Deserialize` (`32d5be30`) touches the slot.
- **NPC `+0x874c` = `m_KeyCode`** and the dirty-state batch reading: agreed.
- **Open item 1, `+0x544 / +0x584`:** these are `CX2Eqip::m_pNormalXSkinAnim[8]` / `m_pHyperXSkinAnim[8]`. The choice (templet `+0xd2` in {9, 11} -> normal, else hyper) is the base `31fd8c00`'s loop, which `32cc8d30` inlines; funcmap R12b already records it, unported for every class. Arme keeps calling March's base, so it gets the choice when the base is ported -> Stage 2 bullet (engine, every class), with the riding-pet branch going to Stage 17.
- `cl /Zs` with LIVE defined: X2GUArme_VioletMage / X2GUUser / X2UIInventory / X2Game, 0 errors each. `CH1m.md` already exists and needs no change.

### CH1m - Done (2026-10-21, Opus)

Flags: `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (the pressed-index reader, the `SPECIAL_OPTION_VALUE` parse, the Arme fixes) and the new
`SERV_IRUHADEV_JUNE_INPUT_SIT_KEY` (Always.h; CORE needs it). Inert until LIVE: only `June_*` code reads either.

**Item 4, pressed-index space (7ag). Design call:** Aisha alone gets June's space by calling June's own reader. `31fcadd0` is ported whole
as `CX2GUUser::JuneCommonSpecialAttackEventProcess` (A 0-3, F `oneSkill5` 4, Q/W/E/R 5-8, T `oneSkillEx5` 9 while `IsJuneSlotUsable( 1 )`;
the guards, the `IsExpandSkillSlot` gate and the SI_NONE test are March's, in June's order). `June_SpecialAttackEventProcess` calls it; March's
`CommonSpecialAttackEventProcess` keeps 0-3 / 4-7 for every unported class, and Aisha's March body (`#else` LIVE, `:5382-5397`, `CountUsedSkill`)
still pairs with it. A later class port switches its caller the same way. No shared signature changed. Slot 4 / 9 stay locked until Stage 6.

**`+0x8726` (from CH1k) = the Sit key.** June `CX2Game::KeyProcess 322f7170 @322f7fa4` calls `3230f5f0( 0x27, ... )` on group `+0x390` (user `+0x8726`)
right after the skill keys; `0x27` = GAMEACTION_SIT (June's RIDING = 0x28; `31f5da70` maps 0x27 to KBI `0x2e` = V). `InputData` gains
`pureSit / oneSit / pureDoubleSit / oneDoubleSit`; X2Game feeds them after the skill block, passing the one byte for both double arguments
as June does (JUNE_BUGS 381). `JUNE_ARME_INPUT_8726` = `m_InputData.pureSit` (the shower loops read `+0x8726`, `32cc7071`).

**Two CH1-era transcription fixes in `June_SelectSkillSlotKind` (`32cc7e60`):** the MemoRise first-entry press is `+0x8727` = `oneSit`, not
`oneSkill5`; and the slot lookup is `3288fff0`'s: index -1, then the first match, A gives `i`, B gives `i + 5` (it gave `i` for both, last match).

**Item 5, hooks and script keys** (each against March's reader):
| Key | Verdict |
|---|---|
| `SPECIAL_OPTION_VALUE` | ported: `CommonStateStart` clears `m_vecJuneSpecialOptionValue` (CX2GUUser, June `+0x93a8`) and pushes values 1..n (`@320608a9-3206092e`), before `TIME_STOP_IMMUNE_STATE` as in June; Arme's stand-in member is gone, `June_GetSpecialOptionValue` reads the user list |
| `BBT_CHANGE_NOW_HP_PER_SECOND`, `BFT_TIME` | read already: buff sub-tables found by `STRINGIZEA2( type_ )` (`X2BuffBehaviorTemplet.cpp:22`, `X2BuffFinalizerTemplet.cpp`); the census cannot see stringized names |
| `SET_LIMIT_MAX_G_SPEED` (7o) | read already (`X2GUUser.cpp:15863`, C3g); June's `AVSI_DAMAGE_BOUNDING_DOWN` carries -4000 |
| `TIME_STOP_IMMUNE_STATE` (7p) | read already (`:17463`, C3e-4); 12 Aisha states |
| `AT_COMMAND` (7x) | read already (DAMAGE_DATA `ATTACK_TYPE`); 18 tags arrive with the script; the Elysion stand-in stays until every played class is ported |
| `EFFECT_SET_LIST_ULTIMATE_EX` | routed: checklist row 397 (`32024ff0` list variants); its gate, the upgraded-skill flag, is off in March until backlog 3c |
| `IMMUNITY_EDT_LIST_AT_THIS_STATE` | routed: Stage 2 bullet "Per-state immunity lists" (`31fdaaa0`, with `IMMUNITY_LIST_AT_THIS_STATE`, `+0x8638 / +0x8648`) |
| `FORCE_DYNAMIC_CAMERA_INDEX`, `RETAIN_CAMERA_STATE` | routed: Stage 9 bullet "CommonStateStart's camera keys" (`@3205b934`, `@3205c0e8`, `+0x801c`) |

7l: `JuneInitBoundingStates( "AVSI_" )` and `JuneInitWaitStateChange` are registered (CH1j). C3d-3's review legs (the `JUNE_FX C3d-3` line, a
bounding hit) are play-test legs; they run in the final CH1 with LIVE.

Hygiene: flagcheck `--worktree --files` PASS for both flags; CORE's 9 deleted lines are the replaced CORE-only lines above.
Build: `DEPLOY OK: X2_offline.exe is 25,681,920 B, dated 2026-10-21 09:35:06, md5 0606f95a3a128b6f1c061e91190aa579` (main 7a797c44 + these 7 files)

In game: nothing (inert). Open questions: none.

### CH1m review (2026-10-21, Opus): PASS after two fixes

A first pass (`fee14739`) read only the diff hunks; this second pass checked every offset and callee against June and found two gaps, fixed here.

- **`JuneCommonSpecialAttackEventProcess` = `31fcadd0`:** the KeyProcess pushes give the game-to-user InputData delta `0x8396` (C `+0x35c` -> `+0x86f2`, riding GA 5), so A `+0x86f7` / S / D / C `+0x86f3` -> 0-3, F `+0x872b` (Skill5) -> 4 with no lock test, T `+0x872f` (SkillEx5) -> 9, Q-R `+0x8707..+0x8713` -> 5-8 as an else-if chain. Guards: `322731a0` = `+0x3f78` (black hole), `+0x8ac4`, `3226e2e0` = `IsCanNotIntput` (time > 0 or the byte). `g_pData` null skips the expand gate (`+0x2cc`, `3214f050` = GetUISkillTree, `+0x109`). Deviation kept: June asks slot 9's state of the local player's tree (`[[g_pData +0x68] +0x90] +0x494`), the port asks the unit's own tree, as March's SKILL_SLOT_10 reader already does; equal for the only unit that reads keys offline. Design call agreed.
- **Sit key:** `@322f7fa4` pushes `0x27, +0x391, +0x391, +0x390, +0x391` (one, one, pure, one); GA_SIT 0x27 / GA_RIDING 0x28 by a compile probe (June pushes 0x28 @322f7fe3). Both swap branches tail-merge into the call (`322f7e2e` -> `322f7f9d`), the riding branch jumps past it (`322f7ca1` -> `322f7fb6`): placement matches. `32cc7071` and `32cc72c1` read `+0x8726` = `pureSit`. JUNE_BUGS 381 agreed.
- **Fix 1, `SetInputData` (`31fe83a0`):** June copies the 22-dword InputData to `+0x8732` and its skill-impossible block clears dword `0x13` (the Sit group) with Skill5 / SkillEx5; March's block lacked it, so a skill-lock debuff let Aisha's V (MemoRise first entry) through. Added under `SERV_IRUHADEV_JUNE_INPUT_SIT_KEY`. `Init()` clears Sit already; `Init_ZXArrowOnly` clears no skill key in either client; `IsKeyPressed` walks `sizeof`.
- **`June_SelectSkillSlotKind`:** `32cc7e60` tests `+0x8727` (`oneSit`) and `+0x9770` (deque size); `3288fff0` (tree = `param_2`) sets -1, first match A[i] then B[i], B gives `i + 5`. March's `GetSkillSlot( id )` interleaves A[i] / B[i] the same way. June writes -1 on a miss too; the caller drops the index on kind 0.
- **`SPECIAL_OPTION_VALUE`:** clear, `BeginTable( name, true )`, `GetValue( i, f, true )` from 1 with `f` not reset, `EndTable( true )` only when opened, before TIME_STOP: matches. **Fix 2:** the block sat inside `#ifdef SERV_IRUHADEV_JUNE_HIT_STOP_LISTS`, so undefining that flag would silently empty Aisha's list; moved above that `#ifdef` (same order).
- **Routes:** `BBT_CHANGE_NOW_HP_PER_SECOND` / `BFT_TIME` are in `X2BuffFactor.h`; `SET_LIMIT_MAX_G_SPEED` `:15863`; `TIME_STOP_IMMUNE_STATE` is now `:17473` (the Done note's `:17463` predates its own insert); checklist row 397 (`32024ff0`); `31fdaaa0` reads `+0x8638 / +0x8648`; `@3205b934` / `@3205c0e8` are key decode sites in `32057b80`. All as cited.

Hygiene: flagcheck `--worktree --files X2Lib/X2GUUser.cpp` PASS for CORE, INPUT_SIT_KEY and HIT_STOP_LISTS (its `#ifdef` moved down 10 lines, block unchanged); `--range 545ddbcd^..545ddbcd` PASS for both CH1m flags. `cl /Zs` with LIVE: X2GUArme_VioletMage / X2GUUser / X2Game 0 errors.
Build: `DEPLOY OK: X2_offline.exe is 25,681,920 B, dated 2026-10-21 09:55:32, md5 744ee46f136a4dcb62f6b482a2865551, equal to X2/US_SERVICE/x2.exe (2026-10-21 09:55:25); built at main fee14739 + 1 uncommitted code file(s).`
In game: nothing (only Aisha's LIVE code reads the Sit bytes). `CH1.md` needs no change.

### CH1 - Done (2026-10-21, Opus)

**The deploy unit is live.** `SERV_IRUHADEV_JUNE_CHAR_ARME_LIVE` is defined (`Always.h`), so Aisha runs June's registration, virtuals, skill-use
steps and pressed-slot reader (CH1j-CH1m). Shipped together with the exe:
- **Script:** `luasrc.py adopt-june data036 Arme_VioletMage.lua`: June's file replaces March's whole, as `ljconv` bytecode (169,337 B, was 136,603 B;
  no core-list row, no edit). Backup `24965799\kom_backup_stage_june\20261021_100608`, record `june2017_re/june_adopt_20261021_100608.tsv`.
  `PACK OK data036` (vs live: ~1 changed, `Arme_VioletMage.lua`); `COPY OK data036 ... 13998632 bytes, 266 entries, md5 344de0cc73f1044e64e04ae409caacd0`
  (user approved). CH0 assigned no other Lua to CH1.
- **`[JUNE_CHAR] ARME DATA`** (LIVE, end of `June_InitState`, offline_server.log): class, script, motion, state count and `wait_state_change`, which is
  1 only when June's script is loaded, because March's has no `AVSI_WAIT_STATE_CHANGE` table.
- **Fix found by the play-test: START floated with no animation.** CH1l emptied the LIVE `ParseCommonRandomState` ("June has no class parse"),
  but June parses `INIT_COMMON_RANDOM_STATE` in the base: `31feaac0`, called directly by `CX2GUUser::Init 3204ea20` between vslots 0x3c and 0x48,
  where March calls the virtual. It takes the table name from `31fdcc20` (unit class -> name; its 86 cases equal `CX2SkillTree::JuneGetUnitClassName`'s,
  diffed), then runs `31fe6780` (= March's `ParseCommonRandomState( tableName )`, same three lists and pairs). Ported as
  `CX2GUUser::JuneParseCommonRandomState()` (LIVE), called by Aisha's LIVE override. The other classes keep their March parse until their port.
  Before the fix the log showed `JUNE_FX C3e-2 roll state 9 ... ( list size 0 ) table ""` for START / WIN / LOSE.
- **CH1j's two notes.** (1) March's `CommonStateStart` accepts June's empty `ANIM_NAME` of `AVSI_WAIT_STATE_CHANGE`: `ChangeAnim( L"" )` finds
  no anim set and returns false without logging, so the WIN pose holds until `USEFUL_VALUE0[0]`, then `USI_WAIT`. (2) JUNE_BUGS 379: with no table,
  `BeginTableByReference` returns false and every key read falls to its default. No crash; kept verbatim.
- **Scans:** `charport_keys.py Arme_VioletMage`: 224 keys, `june-only=6 neither=2 march-only-site=2 both=214` (CH0: 7 / 2 / 2 / 213). The one change is
  `SPECIAL_OPTION_VALUE` -> both (CH1m). The six June-only keys are CH1m's routed ones. `charport_scan.py`: identical to
  `char_arme/plan_charport_scan_20261020.txt` for Arme (NUMERIC 36 / 1; no KEYS / ENUM line). Its SKIPPED list grew by 8 non-character files
  (decompile timeouts). Rerun the scan from a short path (`%TEMP%\cps`): a long scratch path breaks `ljd_jun2017` (MAX_PATH).
- **Bounding (7l):** `JUNE_FX C3d-3 bounding states registered for AVSI_ ( USI_BOUNDING_DOWN 102, LANDCRASH 103, UP 104 )` seen. The hit leg cannot
  be reached offline: only player skills (Lu/Ciel, Ain, Code Empress) carry `RT_BOUNDING`, and there is no PvP.

Hygiene: `flagcheck.py JUNE_CHAR_ARME_LIVE --worktree --files` (4 files): `files: 3 PASS, 1 DEFINE, 0 OTHER, 0 FAIL`; `2 deleted line(s)` (the
`//#define` and the LIVE-only comment the parse call replaces), `0 encoding change(s)`, `RESULT: PASS`. `cl /Zs` (LIVE): X2GUArme_VioletMage, X2GUUser,
X2Game, X2UIInventory 0 errors.
Build (first play-test): `DEPLOY OK: X2_offline.exe is 25,653,248 B, dated 2026-10-21 10:21:09, md5 66d4c01066976849db6351f5057d821d`.
Build (with the START fix): `DEPLOY OK: X2_offline.exe is 25,653,248 B, dated 2026-10-21 10:49:29, md5 6e2d39295ede2be8800bfda285647e74, equal to X2/US_SERVICE/x2.exe (2026-10-21 10:49:21); built at main 2b76c620 + 4 uncommitted code file(s).`

**Play-test 1 (user, 2026-10-21, Void Princess):** checks 2-6 passed. Check 1 failed: START floated, fixed above. Check 7 ran on the Void Princess
with the same empty WIN table, so it needs a rerun. ErrorLog against `data\ErrorLog_pre_CH1.txt`: no `Motion Error` and no missing Aisha animation.
The new Aisha lines are the empty-list C3e-2 lines (fixed) and CH0b's two dangling June face textures. Re-test: START and WIN, on both saves.
Checklist: 7h / 7l / 7o / 7p rows marked `(Aisha: CH1 ...)`. No new JUNE_BUGS row, no drift.
**Play-test 2 PASSED (user, 2026-10-21, both saves):** run 10:55-11:00 on md5 `6e2d3929...`. START and WIN play June's animations. `ARME DATA` for class 105 (133 states) and class 2 (126 states), `wait_state_change 1` both. The rolls pick real tables (`AVSI_AVP_START` of 3, `AVSI_AVP_WIN`, `AVSI_START` of 1, `AVSI_WIN2` / `AVSI_LOSE2` of 2), and the re-binds land on them. 0 `list size 0` / `re-bind skipped`, 0 `Motion Error`, 0 UNHANDLED. The only new Aisha ErrorLog lines are CH0b's dangling June face textures (`ARME_BASIC_FACE_02`, `_EYECLOSE3 `, `_VP.TGA.TGA`).

### CH1 review (2026-10-21, Opus): PASS, no code change

- **Deploy unit:** `Always.h:9656` defines LIVE. data036 `Arme_VioletMage.lua` is 169,337 B, sha1 `25a6b9ea...`, header `1B 4B 4C 81`: equal to `june_adopt_20261021_100608.tsv`'s ljconv row. `kom.py status data036`: `in sync`, vs live `+0 / -0 / ~0`. No `Arme_VioletMage.src.lua` sidecar exists, so a later pack keeps June's bytecode. The data036 "stale .src.lua" warning belongs to other files. `X2_offline.exe` md5 `6e2d3929...` (the 10:49:29 deploy) holds `ARME DATA` once. It was built from the four code files committed in `1429a473`; `eb117b6d` changed docs only.
- **ARME DATA:** 3 x class 105 (133 states) and 1 x class 2 (126 states), each `MOTION_ARME.X`, `wait_state_change 1`. `ch1j_reg_by_class.tsv` gives 118 / 111. The class gap is 7 in both, so the class rows match. The constant +15 is class-independent: states the tsv's two June functions do not list, such as bounding 102-104 (`JuneInitBoundingStates`), `AVSI_WAIT_STATE_CHANGE` and the common ids below the tsv's lowest, 7 (`USI_DIE_*`, ...). They were not counted one by one.
- **ErrorLog** against `ErrorLog_pre_CH1.txt` (402 new distinct lines): 0 `KEM_ERROR`, 0 `Motion Error`. The only new Aisha failures are CH0b's three face textures (ErrorEnum 429), already routed. `MOTION_<class>_EMOTION_BBABBABBA.X` (428), `HQ_WHITEBACK` and `HQ_KOGLOGOWINTER2015` belong to other classes or to the UI: they are not Aisha's and are left as they are. `C3d-3` logged once. `C9h-4g` logged nothing: that leg needs a state with `USEFUL_STRING` / `CUSTOM_OFFSET_*`, which this run did not reach.
- **START fix vs `31feaac0`:** BeginTable( `INIT_COMMON_RANDOM_STATE`, 1 ) on `+0x2b4` (March's default `bLogOutput = true`), name `31fdcc20` narrowed `31c7d950`, `31fe6780( name )`, `EndTable( 1 )`: matches. Call site `3204ea20`: vslot 0x3c, `31feaac0`, 0x48 = March's `InitComponent`, `ParseCommonRandomState`, `InitEffect`. June tests the unit (`+0x867c ? +0x38 : 0`); the port does not, as March's own parse and the log line never did. A user unit always has one. The decision is not to port the test (funcmap row).
- **Other CH1l drop, `ApplyAuraAcceleratorBuffToGameUnit`:** slot 0x7b0 is `32028790` in both the Arme (`3351d864`) and base (`334789a4`) vtables. June has no Arme override, so March's Energy of Plenty branch is gone and LIVE's base call is right. Its sibling 0x770 is the same in both vtables (`32028250`, S3d). The base `32028790` and its caller `3202c360` are unported for every class. That is drift: a Stage 2 bullet in `JUNE_PORT_BACKLOG_ORDER.md` and funcmap rows (C10b-2 had called the host absent).
- **CH1j notes and scans:** re-read, agreed with the Done note (empty `ANIM_NAME` holds the WIN pose until `USEFUL_VALUE0[0]`; JUNE_BUGS 379 falls to defaults; keys 6 / 2 / 2 / 214, scan unchanged).
- Next: `CH2a.md` exists and needs no change.

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

### CH2a - Done (2026-10-21, Sonnet 5.5)

**Landed** (flags `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL`, `SERV_IRUHADEV_JUNE_CHAR_EFFECTSET_MODIFIER`; commit `de9b4b6b`). Funcmap rows `Char CH2a` (31), JUNE_BUGS 382 / 383, `MODS.md` section "Aisha's Violet Mage / High Magician skills" (March vs June damage per skill).
- **Registration.** `June_SetEquippedSkillFuncHead` gets June's `32cede00` cases for the 26 ids: 22000-22009, 22030-22035, 22038, 22200 / 22216, 22201 / 22218 (22047-22049 reach their base through `32207210`; Meditation 22203 / 22227 has no state: CH4). Each case sets the table name, the callbacks and the hyper copy as the state table shows, then logs `[JUNE_CHAR] ARME SKILL registered <name> ( skill N, slot S A|B )`.
- **June bodies written** (`June_*`, 17 states / helpers): Binding Circle FrameMove (`32cd57c0`: effect set scaled by SCOPE_ABS / 500, BFI 73 with June's 7ai arguments, memo 18 force-down), Chain Fireball FrameMove (`32cd6600` / `32cd6800`), Magic Missile (`32cd6a00` / `32cd6b70`), Blizzard Shower (`32cd78b0` / `32cd7a50`), Teleport StartFuture + Start (`32cb71a0`, `32cd26b0`), Chain Lightning Start / FrameMove / EventProcess (`32cc23c0`, `32cdf7f0`, `32cc2440`), Meteor Call FrameMove + EventProcess (`32ce2e90`, `32cc5580`, June-only), Meteo Shower Start / FrameMove / EventProcess + loop Start / FrameMove / End (`32ccc990`, `32cdae90`, `32cbfad0`, `32cccb40`, `32cdb5e0`, `32cdbb70`), Fire Road Init / FrameMove x2 / EventProcess (`32cb7f00`, `32cdc270`, `32cdc4a0`, `32cc0c50`), the generic skill EventProcess (`32cbe6a0` and its 9 identical clones), `June_GetPassiveSkillAbilityValue` (`31ff1e80`), `June_SetMeteoShowerValues` (the `32cede00` tail) and `June_CopyStateCallbacks` (`329cc2c0`).
- **Registered as March's member** (disassembly diffed, funcmap `= March`): the Inits (`32cb7640` `32cb76c0` `32cb7750` `32cb7910` `32cb7e00` `32cb83e0`), Blizzard / Meteo Shower Start and End (`32cbefb0` `32cbf050` `32cb7950` `32cb7970` `32cb7e30` `32cdbc50`), Teleport FrameMoveFuture / EventProcess (`32ccc130`, `32cb71d0`), both CameraMoves (`32cd5c60`, `32cd6460`). Three `jmp [vslot 0x7cc]` forwarders (`32cb75b0` `32cb76f0` `32cb77a0`) and the lone `ret` (`32cb8d50`) are left empty: the engine runs CommonFrameMove / nothing.
- **Engine half (checklist 7a).** Chain Fireball is the first class to push an effect-set modifier. `SERV_IRUHADEV_JUNE_CHAR_EFFECTSET_MODIFIER` adds the instance list, `AddDamageEffectModifier` and the apply hook; **only tag 2 is applied**, the other tags log once `[JUNE_FX] effect-set modifier tag N is not ported` (their classes bring them; checklist note added). This is a scope decision: `321db1b0` is one new function with 23 tags, only 2 is Aisha's.
- **Kept as June has it:** Chain Lightning always plays the `_MEMO` effect set (JUNE_BUGS 382); each Meteo Shower tick spawns `DamageEffect_SA_AHM_MeteoShower_Attack` twice (383). Teleport's buff applies in PVP games only (`GT_PVE` does not exist here); boss-raid gate `32481fa0` is false.
- **Not found in the closure (`ch0_effect_closure.tsv`, "june-undefined"):** `Black_Light.tga` (Magic Missile Init) and the particles `Aisha_StartFireBP04`, `MeteoCall_EP`, `ADE`, `--ADE_GRAVITY_BURSTER_P07R` are named by June's own data and exist in neither kom; expect those lines in `ErrorLog.txt`, they are June's. 20 targets are not carved in Ghidra and were read with `funcdis.py` / `ann.py`: `32cb75b0 32cb7640 32cb76c0 32cb76f0 32cb7750 32cb77a0 32cb7910 32cb7950 32cb7970 32cb7e00 32cb7e30 32cb7f00 32cb83e0 32cb8d50 32cbfad0 32cc0c50 32cc23c0 32cc2440 32cc5580 32ccc990`.

**Hygiene.** `flagcheck.py JUNE_CHAR_ARME_SKILL --with JUNE_CHAR_EFFECTSET_MODIFIER --worktree --files <5 code files>`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (OTHER = `Always.h` carries the peer's CH5 hunk; the two EffectSet files are the modifier flag's: `files: 2 PASS`). `cl /Zs` of `X2GUArme_VioletMage.cpp` and `X2EffectSet.cpp`: 0 errors.
**Deploy:** `DEPLOY OK: X2_offline.exe is 25,672,192 B, dated 2026-10-21 12:09:40, md5 0684cef45e770cc931c88c734a5500cb, equal to X2/US_SERVICE/x2.exe (2026-10-21 12:09:33); built at main f2de6149 + 5 uncommitted code file(s).` The four new strings (`ARME SKILL registered`, `EffectSet_SA_Aisha_ChainFireBall`, `DamageEffect_SA_AHM_MeteoShower_Attack`, the modifier-tag line) are in the deployed exe. **In game:** nothing changes for kom data; the 26 skill ids now cast June states.
**Tooling:** `tools/createchar.py` falls back to the lowest-level unit when the save has no level-1 unit (the seed for inventory sizes).
**Save for the play-test** (backups in `data\db_backup`): new unit 45 `ArmeHM` (High Magician, level 60) with eight CH2a skills in slots 0-3 and 5-8; unit 44 `arishiaaa` got Meteor Call, Binding Circle and Lightning Bolt in B slots 5-7. Slots 4 / 9 (keys F / T) are June's transcendence slots: nothing sits there.

**Play-test fix 1 (2026-10-21, user: "You need more mana" on every skill after the first two):** CH1l's reserved MP (`m_fJune8ea8`, Arme-local) was never cleared; June's slot 514 `31fc9410` clears `+0x8eac` and `+0x8ea8` together. A cast left its cost behind and the next lookup (`ea8 + cost > MP`, text 2549) asked for both. `CX2GUUser::ClearSkillUseReserve` is virtual under the SKILL flag and Aisha's override zeroes `ea8` (called by `CountReservedSkillUse` and by the 1 s reservation timeout). `DEPLOY OK: X2_offline.exe is 25,672,192 B, dated 2026-10-21 13:20:35, md5 4c18f85bae33307074fea80f642107af, equal to X2/US_SERVICE/x2.exe (2026-10-21 13:20:28); built at main da3b12b0 + 3 uncommitted code file(s).` Same run showed the NUM lines for 22002 / 22003 / 22006 / 22007 / 22008 / 22009 / 22201 matching the tsv x0.01 values (e.g. Chain Lightning 6.5, Fire Road 9.97).

**Play-test result (user, 2026-10-21; `check logs` after the MP fix):** `[JUNE_CHAR] ARME SKILL registered` for every equipped skill on `arishiaaa` (class 2) and `ArmeHM` (class 14); `[JUNE_SKILL] NUM use` for 11 damage effects (Meteor Call 9.61 / 5.14, Lightning Bolt 2.43, Gust Screw 4.68, Gust Storm 4.15 / 3.75, Meteor Shower 0.72 / 2.44, Blizzard Shower 0.62 / 7.16, Chain Lightning 6.5), each equal to the tsv's x0.01 column at level 1 (the earlier run also logged Magic Missile 4.9 and Fire Road 9.97). 0 `Motion Error`, 0 `KEM_ERROR`, 0 EXCEPTION, no `[JUNE_FX] effect-set modifier tag` line. New `ErrorLog.txt` lines against `ErrorLog_pre_CH1.txt` are CH0b's three face textures and `EVENTSCENE\PC_AISHA_PNG.SWF` (not Aisha's skills; CH5's portrait), none of the six June-undefined names. Not seen in the last run: Chain Fireball's damage lines (it cast, `ES_CREATE` logged, but no `NUM` line was written in that run: the review checks it), Binding Circle's bind, Teleport.
**`SERV_IRUHADEV_JUNE_SKILL_NUMERICAL_DEBUG` retired** (its `Always.h` block, the `X2DamageManager.cpp` log blocks, `JuneNumericalSelfCheck` in `X2SkillTree.cpp/.h`; `cl /Zs` 0 errors on the three TUs). The flag is gone, so no more `NUM` lines. `DEPLOY OK: X2_offline.exe is 25,667,584 B, dated 2026-10-21 13:37:57, md5 4a0b4c9acd0dabebedd7063d9bb4106d, equal to X2/US_SERVICE/x2.exe (2026-10-21 13:37:49); built at main 9658d261 + 4 uncommitted code file(s).`

### CH2a -- Review (2026-10-21, Opus)

**Verdict: PASS, one transcription fix.** Each function was read against its Ghidra decompile, or for the uncarved ones against the `funcdis.py` / `disasm_range.py`
listing. Every registration thunk was resolved to its `jmp` target from the image. The float constants were read from the image too: 0.3, 0.5, 500,
0.001, 0.55, 150 / 300 / 750, 1.2, 1.1, 600, 0.1, 250, 850, 45, 900, 10 / 20, 0.24, 0.49, 0.25, 0.638 and 30.

1. **Registration vs `32cede00`** (callback slots read as `UserUnitStateData` dwords 5 / 7 / 9 / 0xb / 0xd / 0xf / 0x11 / 0x15). Every case label, table name
   and callback matches. Six hypers reuse the normal table name: Gust Storm, Meteor Call, Binding Circle, Chain Lightning, Fire Road and Teleport. The slot-by-slot hypers copy no
   `Init`; Chain Lightning, Teleport and Fire Road copy everything through `329cc2c0`. The three Meteo Shower states are 0xd9 / 0xda (hyper byte) / 0xdb. The Blizzard hyper's Start / End (`32cbf050` /
   `32cb7970`) are byte-identical to the normal ones. The generic EventProcess `32cbe6a0` and its 9 clones are byte-identical. Teleport's MP tail matches:
   `SA_MP_CONSUME_REL` (0x1b), level at least 1, `+0x95b0`.
2. **Meteo Shower** (`32cdae90` / `32cdb5e0`). The random is `( r*3 - 150 ) * 4` with `r = vslot 0x1c8( elapsed*4 + 1 )`, after the increment. Parity and facing give ±250 / ±500.
   The start point is +850 up and ∓900 along the facing, with rotation z -45 / +45. `CommonFrameMove` sits inside the `elapsed < max` block (the early return). The loop handle is `+0x95f8`. Emit rate 10 / 20
   goes to `+0x95a8` / `+0x95ac`. The tail `June_SetMeteoShowerValues` maps 0x18 / 0x1e / 2 / 0x1b to `+0x9728` / `+0x96f8` / `+0x9704` / `+0x971c`; passive 22413
   does `imul` on the count, and type 0x8f = `SA_MP_CONSUMPTION` (June 143) scales the MP.
3. **Binding Circle** (`32cd57c0`). The checks run in June's order: team, then slot 94( 1 ), then the squared scope. The bind is `( 0x49, this, 1, 0, 0 )`: the port passes skip = false, as June does (March's default is true).
   The memo type is 18, and `SA_SCOPE_ABS` = 0x1d.
4. **Chain Lightning** (`32cc23c0` / `32cdf7f0` / `32cc2440`). The set name is 37 characters long, `…_MEMO`, so JUNE_BUGS 382 is real. `IF_SKILL_CANCEL_AFTER` (the AWAKE_LOCK form) is June's first two blocks. Then come
   WAIT for Right / Left and dash for the double arrows. Last is WALK on the 14 bytes `+0x86da..+0x8712` without `+0x8702` (F). Start: `ResetAnimSpeed( +0x88fc, +0x8904, 0, value( 0x67 ) )`;
   the port's re-read of `APPLY_ANIM_SPEED_BY_ITEM_ABILITY` gives the value `CommonStateStart` stores.
5. **Meteor Call** (`32ce2e90` / `32cc5580`). The filters run valid, team, state 4 (`GUSI_DIE`), HP <= 0, facing, NPC class. Distance uses `<=`, starting from 600. The line-map call is
   `( dir, pos, 600, isRight, &out, &idx )`, with a ±dir*600 fallback (the decompile's shifted fields are a Ghidra artifact; the listing is consistent). The EventProcess order is
   landing, skill, hyper (no on-ground test), walk, dash.
6. **Fire Road / Teleport**: `0.24`, then `0.24 || 0.49`; a NULL unit returns before `CommonFrameMove`; dash at `0.25 <=`, walk at `0.638 <=` against `+0x7e56`.
7. **Modifier list**: `( handle, 2, +0x95c8, 0 )`; the hook is at `321ef6b3` (after the velocity list `+0x128`, before the follow-line block); tag 2 = mesh `+0x44c` max life, add / multiply.
8. **"= March"**: the Blizzard Start / End, Meteo Shower End and END StateEnd (emit 0 / 0) are identical.

**Fix (same flag):** `June_GetPassiveSkillAbilityValue` left out June's PvP-or-PvE argument. `31ff1e80` passes `322cc1a0( 0 )` as the third argument of
`32206e50`; the port now passes `GT_PVP == GetGameType()`, the project's stand-in. This changes nothing offline, since there is no PvP. Hygiene: `flagcheck.py JUNE_CHAR_ARME_SKILL --with
JUNE_CHAR_EFFECTSET_MODIFIER --range de9b4b6b^..de9b4b6b`: `files: 1 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted
line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 2 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (OTHER = the two `X2EffectSet`
files, which belong to the modifier flag). `9658d261`: `files: 3 PASS`, `RESULT: PASS`. Review fix `--worktree`: `files: 1 PASS`, `RESULT: PASS` (1 deleted line = the CH2a line it replaces).
`DEPLOY OK: X2_offline.exe is 25,667,072 B, dated 2026-10-21 13:53:15, md5 d8b76905f63f84a3cf55ae32d2b136b8, equal to X2/US_SERVICE/x2.exe (2026-10-21 13:53:08); built at main 63599741 + 1 uncommitted code file(s).`

**Play-test record.** Chain Fireball did deal damage in the 13:29 run. The Done note missed this: `offline_server.log` has `NUM use …ChainFireBall_Small … magic=5.7600` and
`…_Big … magic=5.2300` (the tsv's 576 / 523 x0.01), both `HIT`, and two NPC kills. Binding Circle: the user saw the enemy bound in game (2026-10-21). That was an earlier run; the 13:29 logs hold only its registration line.
Teleport was registered but never cast: it is this review's only play-test check.
**Found, owned elsewhere:** `+0x95c8` (the fire-ball range rate that tag 2 multiplies by) is set by June's `InitPassiveSkillState` `32cecd70` from passive 22400. March's setter (still live,
`SI_P_AV_FIREBALL_UP`) never finds that id on a June tree, so the rate stays 1.0 until CH4 ports `32cecd70` (already in CH4's map row). Teleport's `m_fMPConsumeRateTeleport` base is the same case.

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

### CH5 - Done (2026-10-21, Sonnet 5.5)

- **`UnitTemplet.lua` (data036):** converted to core-list BOM source (`luasrc.py convert`, decompile T1; ledger + core row `edit: pilot-char-revamp CH5 per-character rows`).
  The 7 `UC_ARME_*` blocks (2, 14, 15, 23, 104, 105, 113) are June's text; the other 48 blocks are untouched (block-wise swap, 55 blocks before and after).
  New in Aisha's rows: `m_HyperCutIn` `HQ_Cutin_AEM/AVP/ADW_Transcendence` on 104 / 105 / 113 (the `.gfx` are in data195, CH0b), names `STR_ID_4792` / `4793` / `17427`,
  `m_fLobbyRotationY` (ignored by March). No reader fix needed (all keys read since C3t, `UC_150` is nil and skipped), so no `JUNE_CHAR_UNITTEMPLET` flag.
  `kom.py pack data036`: PACK OK, the only changed entry vs the live kom is `UnitTemplet.lua`. **Copy not done: it waits for the user's yes.**
- **Cut-in table (flag `SERV_IRUHADEV_JUNE_CHAR_ARME_UI`, needs `ARME_LIVE`):** `s_SkillCutInSet` = June `333ab660` (`HQ_CutIn_Aisha_01/02/03`, `EM_01` x3, `VP_01` x3, `DW_01` x3,
  no extension; 4 sets x 3, same shape and `SkillCutIn( name )` size/position as March), March's table stays in the `#else`. `s_SkillCutInSetEvent8b3` = `333ab4d0`
  (`HQ_CutIn_Aisha_CASH_21/22`); `June_ShowActiveSkillCutInAndLight` takes it under `IF_EVENT_ENABLED( 0x8b3 )` (even `rand()` -> CASH_21, as `32cc9680`). March's
  `ShowActiveSkillCutInAndLight` reads the same table, so March-state skills show the June art too. The `.gfx` of all 12 names exist (CH0b).
- **Logs (`[JUNE_CHAR] ARME UI`):** `cut-in set/index/name` (June path and March-state path) and `hyper cut-in class/name` (`CX2GUUser::ShowHyperSkillCutInByScript`).
- **Not ported, routed:** the cut-in item override (`31fd8a50` -> customize manager) -> Stage 12 bullet in `JUNE_PORT_BACKLOG_ORDER.md`. Code event 0x8b3 is never set offline, so that branch is dormant (compiled in).
- **7ah / 7aj (10-slot dialog, tree UI):** CH0's map assigns them to CH4, not CH5; nothing left for CH5 (the third scope item had no CH5 rows).
- **Open for the review:** floats `334367ac` / `33435598` (512 / 256) not read in Ghidra; Always.h comment says "March's function would draw textures", but the real reason for the LIVE dependency is the `X2OfflineLog` include.
  `luasrc.py core --check` fails on `data036 ResultData_new.lua` (a peer's BOM file, not on the core list): not mine, not touched.
- **Hygiene:** `flagcheck JUNE_CHAR_ARME_UI --worktree --files X2GUArme_VioletMage.cpp X2GUUser.cpp Always.h`: 2 PASS, 1 DEFINE, 0 FAIL; 0 deleted lines, 0 non-ASCII, 0 EOL mismatch; RESULT PASS.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 25,654,784 B, dated 2026-10-21 11:37:03, md5 148fe470e2bf48ae6dd7a8ac4095d3ee`; the 4 `ARME UI` strings are in the exe.
- **Play-test (user, 2026-10-21, after `copy data036` COPY OK md5 `935bf012...`):** select screen and character creation load all characters (PASS). ErrorLog: 0 `KEM_ERROR` / `Motion Error`,
  nothing on UnitTemplet / `UC_150`; `ARME DATA` lines for classes 2 and 105 normal. **Skill cut-in not tested** (user: skip; June's skills are not ported, so the hero art and the `ARME UI` lines wait for
  the first ported skill, CH2a, or the review). Hyper cut-in waits for CH3b.
- **See it in game:** select screen: all nine characters' names and models; Aisha's skill cut-in shows June hero art with the frame and balloon; a transcendence hyper (104 / 105 / 113) shows `HQ_Cutin_A*_Transcendence`
  only when the class script's awakening states set `SHOW_HYPER_CUT_IN_TIME`.

### CH5 - Review (2026-10-21, Opus 5.5)

**PASS, one guard fix** (Always.h only, same flag `SERV_IRUHADEV_JUNE_CHAR_ARME_UI`).
- **`UnitTemplet.lua`:** 55 blocks before and after, same classes, same order. The 48 non-Aisha blocks equal a fresh decompile of the snapshot bytecode
  (`kom_contents_bytecode\data036_kom`), whitespace-normalised: 0 differ. The 7 `UC_ARME_*` blocks equal June's (`june_src_fixed`) per class. None of June's 32 June-only
  classes came in. BOM + LF, like the other converted sources. `ledger --check`: PASS (0 FAIL, 0 WARN). Ledger source sha1 = the file without its BOM.
  `kom.py status data036`: in sync, +0 / -0 / ~0 against the live kom (the copy landed). `core --check`'s one FAIL is the peer's `ResultData_new.lua` (Q4c), not CH5.
- **Cut-in table vs June:** `333ab660` builds 12 `SkillCutIn` at `337ae9c0` (stride 0x30): `_01/_02/_03`, `EM_01` x3, `VP_01` x3, `DW_01` x3. The strings at `335210b0` ..
  `33520fe4` are read from the exe, length 0x11 and up, no extension. `333ab4d0`: `337ae960` = `CASH_21`, `337ae990` = `CASH_22`. All match the port, in order.
  The 12 `.gfx` are in data152 (8) / data195 (3, `HQ_Cutin_` case), and both koms are in sync with the live ones.
- **0x8b3 branch (`32cc9680`):** `g_pInstanceData (33775864) != 0 && IsEnableCode(0x8b3) (31e5a860) == 1` = `IF_EVENT_ENABLED`. `rand() & 0x80000001`, zero (even) -> `337ae960` CASH_21.
  It runs after the unported item override (`31fd8a50`), so June's event art wins over a custom cut-in; the port's order is the same. The port is right.
- **Size / position:** `334367ac` = 512.0, `33435598` = 256.0 (read from the exe). `32078b70` / the inline ctor store size (512, 512), position (256, 512, z) = March's
  `SkillCutIn( name )` (X2GUUser.h:830). The GFx path ignores both.
- **Fix:** the Always.h comment gave the wrong reason for the dependency. The extensionless names work only because `ActiveSkillShow` appends `.gfx` under
  `SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN` (G4, X2GUUser.cpp:28581-28603). `ARME_LIVE` is needed for `June_ShowActiveSkillCutInAndLight` and the `X2OfflineLog` include.
  The comment now says that, and a second `#error` requires `GFX_TALK_CUTIN` (a top-level `#define` at Always.h:8820, no enclosing `#if`, no `#undef`: checked by a depth scan).
- **Routing:** the item override -> Stage 12 bullet (present). 7ah / 7aj belong to CH4 per CH0's map.
- **Play-test legs not run:** the logs of the CH5 run (11:39-11:44) have 0 `ARME UI` lines, 0 `KEM_ERROR`, 0 `Motion Error`, 3 `ARME DATA` (2, 105, 105), 4 UNHANDLED
  `E_DISABLE_HB_CHECK_REQ` (pre-existing). The skill cut-in needs a June skill state, and the hyper cut-in needs an awakening state. Both checks are now in the CH2a / CH3b kickoffs.
- **Hygiene:** `flagcheck JUNE_CHAR_ARME_UI --range 3c14ac1b^..3c14ac1b`: 2 PASS, 1 DEFINE, 0 FAIL, 0 deleted, RESULT PASS. Review fix `--worktree --files KTDXLIB/Always.h`:
  1 DEFINE (the 2 deleted lines are the replaced Description lines), RESULT PASS.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 25,654,784 B, dated 2026-10-21 11:59:12, md5 d02432b4d8c591804a7cb8d78cc85663, equal to X2/US_SERVICE/x2.exe (2026-10-21 11:59:04); built at main 40c1064f + 5 uncommitted code file(s)`
  (the uncommitted files are the running CH2a's). User owes nothing.

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
