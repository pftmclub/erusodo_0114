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
