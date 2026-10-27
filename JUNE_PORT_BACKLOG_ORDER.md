# June 2017 port backlog: execution order

> Written 2026-10-03 on `mods/june-buff-port`. This file sets the **order** of the remaining June-parity backlog and says why. It does not plan any one stage in detail.
>
> Each stage gets its own phase doc, written by a planning session started from that stage's kickoff prompt (bottom of this file). Stage 1 already has its doc: `ATLAS_PORT_PHASES.md` (written as two docs in commit `24de3d3`; `ELYSION_PORT_PHASES.md` was merged into it on 2026-10-16 and is now a pointer). The next stage to plan is Stage 2, and its kickoff is written out in full. The rest are short, and each one asks the planning session to re-check this file's assumptions first.

## Context

The standing goal is to bring the March 2014 engine as close to June 2017 as possible, so that each June feature becomes a data port instead of a fresh round of inference ([[goal-march-client-runs-june-effects]]). The effect rebuild was the first trial, and the buff port, Lanox and June UI parity followed.

The user's remaining backlog, in the order they listed it:

1. Port Atlas and Elysion.
2. June revamped every March character (possibly a new base model) and added new characters.
3. Character transcendence.
4. The engine upgrade: the effect engine is done but needs checking; shaders and more remain.
5. The skill tree update (driven by transcendence and the character revamp).
6. The UI update.
7. The skill slot update.
8. The quest update.

**User decisions this order is built on:**

- **The offline client is the only target** (2026-10-03).
  - Several stages must change headers under `KncWX2Server/Common/`, which the client shares with the servers. These are the skill-slot struct, `VILLAGE_MAP_ID`, and `UNIT_TYPE`/`UNIT_CLASS`.
  - Those changes go behind `SERV_IRUHADEV_` flags, and only the offline emulator (`X2Lib/Offline/`) is kept in step.
  - The real servers will desync with the ported client. That is accepted, so no stage writes matching server code. VS2003 is not installed here anyway.
  - The shared headers stay VS2003/C++98-clean, per `CLAUDE.md`.
- **Atlas/Elysion is already planned** (2026-10-03, a parallel session; one plan since 2026-10-16: `ATLAS_PORT_PHASES.md` A0–A13 and E0–E15). That session recorded these decisions:
  - Work happens on `main` in the main Trunk, with no new branch and no worktree (user, 2026-10-16: one branch for every stage).
  - **Quests are deferred to a later quest plan**, together with the Lanox quests and possibly a Scaleform runtime. June's quest keys go in verbatim and inert.
  - **The level cap goes to 99** (A2). The user authorised invented `StatTable`/`NpcExpTable` rows 81–99, for those two tables only.
  - **June's `ExpTable.lua` is adopted whole**, so existing saves get a one-way EXP migration.
  - **Tower and Memory are blocked on JUNE_BUFF Phase 10.**

  This order slots around those decisions. It does not re-open them.

---

## The order at a glance

| # | Stage | Backlog items covered | Why it sits here |
|---|---|---|---|
| 0 | Close the open loops | 4 (effects, buffs) | Nothing new stacks on a half-updated engine ([[no-partial-june-updates]]). Each open item is mapped below to the stage it actually gates. |
| 1 | Atlas + Elysion *(already planned)* | 1, plus the level cap | Planned, and the user's current priority. It is content on the engine as it stands, with quests deliberately split out. It runs on `main`, like every stage, and it raises the level cap that transcendence assumes. |
| 2 | Engine remainder and measurements | 4 (shaders, render types) | June character effects need the missing render types and June's `.fx` contract. The measurements (models, UI widgets, effect check) size every later stage. |
| 3 | Skill system core | 5 (engine half), 7 | Every character, transcendence and the dormant buff types BBT 98/BFT 16 sit on it. This stage removes the "June character attacks do 0 damage" blocker. |
| 4 | Quest system | 8, plus the quests deferred from Lanox and Atlas/Elysion | Transcendence's class change is a quest type (`QT_CHANGE_TRANSCENDENTAL_CLASS`), and two regions' worth of inert quest keys wait on this engine. One quest engine serves all of them. |
| 5 | Pilot character revamp | 2 (first character) | Proves the character pipeline end to end on one March character before repeating it seven times. |
| 6 | Transcendence system, on the pilot | 3 | Needs slots (3), quests (4), the level cap (1) and a character already on its June kit (5). It is built once, on one character. |
| 7 | Remaining March characters | 2, 3, 5 (per-class trees) | Repeats a proven recipe per character: revamp, the June-only later class paths, transcendence and the skill tree. |
| 8 | New characters: Lu/Ciel, Rose, Ain | 2 (new characters) | The largest surface: a new `UNIT_TYPE`, creation, tutorial dungeons and server-side data. It goes last because it reuses everything before it. |
| 9 | UI sweep | 6 | Whatever June UI no earlier stage delivered. See "Why UI is last" below. |
| 10 | Emotion system *(added 2026-10-07, user)* | - (found by june-effect C9d) | Not a backlog item but a missing system: June's emotion catalogue, data, items, sit effects and emotion UI. It depends only on Stage 2 (UI framework) and june-effect C9e, so it can run alongside Stages 3-8. Numbered 10 to keep the earlier numbers stable, not because it must go last. |
| 11 | Division item templets *(added 2026-10-07, june-effect C9f)* | - (found by june-effect C9f) | Not a backlog item but a missing system: June's per-item render and behaviour lists (`AddDivisionItemTemplet_LUA`, 112 types), of which C9f-2 ports only the per-equip effect sets. Its render rules wait on Stage 2. Numbered 11 to keep the earlier numbers stable, not because it must go last. |
| 12 | Character customize system *(added 2026-10-07, june-effect C9g)* | - (found by june-effect C9g) | Not a backlog item but a missing system: June's `CX2CustomizeManager` (`CharCustomData.lua`) applies cash custom motions, sit motions and effects, faces, textures and skill cut-ins to a character, on the unit, square and viewer. It depends on Stage 10 (sit emotions), Stage 2 (UI) and the user's June `item.kim`. Numbered 12 to keep the earlier numbers stable. |
| 13 | June `item.kim` adoption *(added 2026-10-07, user)* | - (the user's decision during june-effect C9g) | Not a backlog item but a prerequisite: March loads June's `item.kim`. That needs June's 228-byte record ported, since March's is 188 and both files carry the same header version. Planned in full as `JUNE_ITEM_KIM_PHASES.md` (K0-K4). Stages 11 and 12 depend on it for real data. It touches no other stage's files, so it can run alongside Stages 2-8. Numbered 13 to keep the earlier numbers stable. |
| 14 | Byte-exact June asset extraction *(added 2026-10-07, user)* | - (the user's question: do June assets load in March unchanged?) | Not a backlog item but a prerequisite for porting any June asset faithfully. June's formats load in March unchanged, but elstools cuts 18,571 June V.0.4 entries short. Planned in full as `JUNE_ASSET_EXTRACT_PHASES.md` (X0-X5): an extractor, a packing helper and a long-name engine fix. It touches only tooling, staging and `KGCMassFileManager.cpp`, so it can run alongside Stages 2-8. Every stage that stages June assets benefits from it. |
| 16 | June item-side systems *(added 2026-10-07, june-item-kim K0)* | - (found by june-item-kim K0) | Not a backlog item but missing systems: June's item templet carries fields and flags March has no reader for (damage range, equipment group, description-in-shop type, item expiry, PvP-equip), and June's `ResetItemManager` opens item tables March never loads (`SetItemGroup`, `ItemManager` item lock, `CoordiItem` and others). It depends on Stage 13 (K2) for the record. Numbered 16 to keep the earlier numbers stable. |
| 17 | Pet and riding-pet remainder *(added 2026-10-07, june-effect C9h)* | - (found by june-effect C9h) | Not a backlog item but missing data and one packet field: June's pet **variant** byte (a `KPetInfo` field read by the pet-info viewer, the in-game pet and `ResetPet`'s texture-variant branch) and June's 8 pets / riding pets that March has no enum, templet or script for. Its code waits on june-effect C9h-2a (the viewer half) and C9h-3 (the riding-pet engine). Numbered 17 to keep the earlier numbers stable. |
| 18 | NPC friendship system *(added 2026-10-08, june-effect C9h-5)* | - (found by june-effect C9h-5) | Not a backlog item but a missing system: June's per-NPC friendship (likeability) manager `32696fa0`, its gift / level-up / daily-reward talks, the friendship talk sequencer and the friendship marks on village NPCs. Every reach is gated on code event 2215, which the offline emulator never enables. Elysion E1 ports the gated call sites in `CX2TFieldNpc`. |
| 19 | June event dungeons *(added 2026-10-08, june-effect C10d)* | - (found by june-effect C10d) | Not a backlog item but missing content: June's event dungeons (e.g. `DI_EVENT_2017_TREASURE` 41470 and `DI_EVENT_ALCHEMY_LAB` 41350) carry engine gimmicks in `DamageReact`, a `CX2Game` manager and June-only NPC classes (`CX2GUKiddayNPC`). March has none of these dungeon IDs; its highest is 41311. Nothing reaches them until the dungeons themselves are ported. |
| 20 | Damage-number customization *(added 2026-10-09, june-effect C3e)* | - (found by june-effect C3e) | Not a backlog item but a missing system: June's damage-number skin items (Damage Custom A / B / Critical, Christmas, Nature) pick the font of the numbers a character deals, and the user sync carries the choice to other players. March has none of it. It depends on Stage 13 (the item ids) and touches no other stage's files. Numbered 20 to keep the earlier numbers stable. |
| 21 | Photozone *(added 2026-10-09, june-effect C3e-2)* | - (found by june-effect C3e-2) | Not a backlog item but a missing game state: June's `XS_PHOTOZONE_LOBBY` (0x19) / `XS_PHOTOZONE_GAME` (0x1a), where the chat box's WIN / LOSE commands re-roll the start / win / lose motion. March has no Photozone. Numbered 21 to keep the earlier numbers stable. |
| 22 | June dungeons and worlds *(added 2026-10-11, user: split out of june-effect)* | - (found by june-effect C3x / C3y / C3z) | Not a backlog item but missing content: the June dungeons, worlds and map entries that March lacks or has half of (Sander and Hamel secret dungeons, the five Henir Space boss worlds, class tutorial dungeons, Bestera, Space of Thought, the Hero tier, June's data101 map assets, the DungeonData readers june-effect C3z-5 routed, the Battle Field tutorial guide). Planned as `JUNE_DUNGEONS_PHASES.md`, STATUS block `june-dungeons`; phase IDs kept from june-effect. It shares the core registries with Stages 1 (Atlas / Elysion dungeons A8-A10, E8-E11) and 19 (event dungeons), so those port halves run one at a time. Numbered 22 to keep the earlier numbers stable. |
| 23 | Offline field rewards stand-in *(added 2026-10-17, user: atlas A6)* | - (found by atlas A6) | Not a backlog item but missing server data: the server `DropTable.lua` / `StaticDropTable.lua` stop at Sander (40025), so Lanox (40029-40032) and Elysion (40035-40038) field kills grant no EXP, ED or items offline. No June server exists, so the rewards are a labelled stand-in (own flag, proposed first, [[design-missing-june-pieces]]). It touches only the offline emulator and server Lua, so it can run alongside any stage. Numbered 23 to keep the earlier numbers stable. |
| 25 | June fonts and text rendering *(added 2026-10-17, user)* | - (the user: March's text looks un-anti-aliased; use June's fonts) | Not a backlog item but a visible gap: March installs one FreeType face (`Gulim.ttc`) and ships 3 font files in `datai022`; June ships 26 in `data196`. Which faces June installs for the US build, and how it rasterizes them, is unmeasured. It touches the font manager, `X2Main` and one font kom, so it can run alongside Stages 2-8 (not alongside a session in `KTDGFontManager.*` / `KTDGFreeTypeFont.*`). Stage 20's damage-number fonts are separate. Numbered 25 to keep the earlier numbers stable. |
| 26 | June AVI movie system *(added 2026-10-19, june-gfx G5)* | - (found by june-gfx G0 / G5) | Not a backlog item but a missing system: June plays AVI movies from `DungeonClearMovieTemplet` / `Movie.lua` (dungeon-clear and quest movies) and flags them in `g_pMain +0x80a`; March has none. It is not GFx, so the GFx plan only noted it. |
| 29 | June dungeon result screen *(added 2026-10-21, user: split out of Stage 9)* | 6 (one screen of it) | The screen every dungeon clear shows. June keeps the player in the dungeon state and opens `dungeon_result.gfx` (GFx UI 8) over the frozen game; March leaves for the old KTDGUI `XS_DUNGEON_RESULT` state. It needs only june-gfx (closed), so it runs now, outside the Stage 9 order. Planned in full as `JUNE_DUNGEON_RESULT_PHASES.md` (R0-R4, Z). |
| 30 | June resonance system *(added 2026-10-22, june-transcend planning)* | - (found by june-result R0) | Not a backlog item but a missing system: June's level-99 progression. `KUnitInfo` carries five June-only fields (CX2Unit `+0xa99`..`+0xaa9`); gate `3247b460`; the dungeon result shows its bars. No March counterpart. Brief `kickoffs/june-backlog/stage30-plan.md`. |
| 31 | June achievement system *(added 2026-10-22, june-transcend planning)* | - (found by june-transcend planning) | Not a backlog item but a missing system: `CX2AchievementManager`, `AchievementData.lua` / `AchievementConditionData.lua`, `DLG_Achievement_System_UI.lua`. March has none. Brief `kickoffs/june-backlog/stage31-plan.md`. |
| 32 | June bottom bar *(added 2026-10-24, user: split out of Stage 9)* | - (user request) | The village / field bottom bar as June draws it: June's `DLG_Menu_Button_NEW.lua` and its textures, the button row layout, the EXP bar and the stamina bar, and June's body for each working button. Buttons whose feature the project lacks are drawn and log a `STUB` click; their features stay Stage 9. Planned in full as `JUNE_BOTTOMBAR_PHASES.md` (B0-B4, Z); runs now, outside the Stage 9 order. |
| 33 | June damage and character stat calculation *(added 2026-10-27, user)* | - (the Aisha instant-MP bug) | Not a backlog item but a correctness gap: June's stat and damage formulas were ported piecemeal, by the buff, socket, effect and skill plans. Where a June seed or refresh now sits beside a March helper, the result is neither game's number: one ranged hit gave +6722 MP. This stage maps every stat and damage formula as June / March / mixed and replaces each mixed one with June's whole calculation. Its code runs after the character ports (Stages 7 and 8), so every character's damage can be tested on June's formulas (user, 2026-10-27). A mixed formula that breaks play before then is still fixed at once (*Scope*). Brief `kickoffs/june-backlog/stage33-plan.md`. |

**Concurrency.** There is one working tree and one branch, `main` (user, 2026-10-16). Every stage's code sessions commit onto it with explicit pathspecs; no stage creates a branch or a worktree, and nothing is merged.

Planning sessions are read-only, and can run any time. Stage 2 *planning* can start now. Stage 2 *code* waits for the buff port to finish in the main Trunk, because both touch `KTDXLIB/Always.h` and shared engine files.

**Merging.** Stage 1's branch is merged back before Stage 4 starts, because Stage 4 activates the quest keys that Stage 1 left inert.

Stages 5–8 are the long pole. Stages 0–4 are prerequisites or self-contained content, so finishing them first means the character work is pure repetition and not discovery.

**Priority (user, 2026-10-19).** Once Stage 3 closes and the quest system (Stage 4) finishes, the order is:
1. pilot character revamp (5);
2. transcendence (6);
3. the remaining March characters (7);
4. the new characters (8);
5. the UI sweep (9).

The scattered systems (Stages 10-26, and any stage the scope rule adds) come after. They stay in reach because each has a
brief, and one moves earlier only when it blocks playability. See *Scope* under "Standing rules".

---

## Facts behind the order

These were measured on 2026-10-03. Re-verify any of them before building on it.

1. **June's skill templets are not in `june_src_fixed/`.**
   - `data036_kom/NewSkillTempletVer2.lua` and `_LA.lua` are about 5.2 MB of bytecode each. They decompile `clean/complete` (`june2017_re/decompile_corpus.tsv:1634`) but fail the compile round-trip check (`june2017_re/decompile_verdicts.tsv:42457`), so the fixed decompiler never wrote them out.
   - March loads `NewSkillTempletVer2Trans.lua` (`X2Lib/X2SkillTree.cpp:123`).
   - The quest tables, `OptionItemData`, `PetTemplet` and `TitleTable` are in the same state (`JUNE2017_CHARACTER_PORT_CHECKLIST.md:156`).
   - So Stage 3 starts with a tooling step: get these files out, and check them against `ljd_jun2017.py --check` or an equivalent.
2. **The 0-damage blocker is in the skill system, not the character scripts.**
   - 121 June character `DAMAGE_DATA` tables carry no `DAMAGE` table and take their numbers from June's numerical-skill-data override (`NUMERICAL_DATA_INDEX` into `NUMERICAL_DATA1..12`, `JUNE2017_CHARACTER_PORT_CHECKLIST.md` §1).
   - The override's callees are not identified in Ghidra yet.
   - Until that override exists, every character stage would port attacks that do nothing.
3. **Skill slots change both the shared struct and the save.**
   - March has 4+4 slots: `EQUIPPED_SKILL_SLOT_COUNT = 4` with arrays A and B, at `KncWX2Server/Common/CommonPacket.h:1051` and again at `:6387`.
   - June has 5+5: `DLG_GAME_SKILL_SLOT_NEW_10slot` with `SlotA1..A5`/`SlotB1..B5`. June's `SlotIndexToSlotType` maps 0–3 to A, 5–8 to B, and 4 and 9 to `SKILL_SLOT_TC`, the transcendence slot (`JUNE_BUFF_PORT_PHASES.md:783`).
   - The offline save stores one slot index per skill (`X2Lib/Offline/X2OfflineDB.cpp:2063`). March's B slots 4–7 would become June's 5–8, so this is a **forward-only migration of the user's real `els_db.sql`**. Take a `db_backup/` snapshot before running it ([[game-dir-db-backup-snapshots]]). Stage 1's A2 EXP migration runs first, so the two migrations must chain cleanly.
   - Existing flags in this area: `SERV_IRUHADEV_SKILL_SLOT_B_FREE` and `SKILLTREE_NO_LOCK`. Read `MARCH_2014_MIGRATION.md`'s `SetUsingPage` trap first.
4. **Transcendence touches almost everything.** Evidence in June's `Enum.src.lua`, all absent from March:
   - `QT_CHANGE_TRANSCENDENTAL_CLASS`, `QR_TRANSCENDENTAL_CLASS`, `NUI_TRANSCEND_QUEST`;
   - `SKILL_SLOT_TC`, `SMUCM_DISABLED_TRANSCENDENTAL_(EXPAND_)SKILL_SLOT_MOUSE_UP`;
   - `USI_TRANSCENDENTAL_HYPER_MODE`, `SDT_WEAPON/DEFENCE_(SHINE_)TRANSCEND(_PVP)`;
   - `TRANSCENDENTAL_SKILL_DATA`, `TranscendentalClassItem.src.lua`, and 12 `DLG_Transcendental_Change_Guide_<character>` dialogs.

   March has essentially nothing. `X2Lib/Offline/X2OfflineLuaEnumSeed.h:39` notes the server's `UC_*_2` placeholder classes, which are compiled out of the client.
5. **Atlas and Elysion are two halves of one region**, split by June's own pages.
   - **Atlas:** Atlas Station (rest area 1008), Fields 01–02, and the AtlasDevelopment, AtlasCity and Tower dungeons.
   - **Elysion:** the village (`VMI_ELYSION` = 20009), Field 03, and the SkyCrossroads, Adrian, Soless and Memory dungeons.
   - Both are planned (see Context). This file only places them in the order.
6. **Characters.** June's `Enum.src.lua` has 85 `UC_*` names March lacks:
   - new characters: Lu/Ciel ×10, Rose ×9, Ain ×10, with new types `UT_LUCIEL`, `UT_ROSE`, `UT_AIN`;
   - later paths for existing characters: Ara, Elesis, Add, Aisha, Chung, Eve and Rena;
   - a few `UC_150..183` entries that appear only as numbers.

   *Scope (user, 2026-10-20): third jobs are out.* The port targets what June 2017 ships, up to transcendence: the revamps, the June class lines
   of March characters (Ara 34 / 124, Elesis 35 / 125, Add 33 / 36 / 123 / 126) and the new characters. The `UC_150`-`UC_186` block holds the
   third-job names, e.g. Aisha 153-155 and Eve 162-164. It is enum names plus one placeholder `UnitTemplet` row (`UC_150`), and no stage ports it.

   **The "new base model" claim is not verified.** Stage 2 measures it.
   - *Measured 2026-10-16 (Stage 2 planning, `JUNE_ENGINE_REMAINDER_PHASES.md` M3):* **no new base model** for any of the 9 March characters. Motion file names, frame counts and bone parent maps are unchanged; June adds animation sets (e.g. Elsword 235 -> 300) and new job motion files (Ara 2, Elesis 2, Add 4). Lu/Ciel, Rose and Ain are June-only in every bucket. Stages 5 and 7 need no skeleton work; Stage 8 is data plus new unit types.
7. **Shaders.**
   - June's `.fx` files (`XRenderer.fx`, `GCSkin.fx`, `RenderState.fx`, `XPostEffect.fx`, …) are in `data041_kom`, which is not in `june_src_fixed/` (`JUNE_KOM_CONTENT_GUIDE.md:69`).
   - Seven June `RENDER_TYPE`s are missing from `KTDXLIB/KTDGXRenderer.h`: `RT_CARTOON_ALPHA`, `RT_CARTOON_BLACK_EDGE_ALPHA`, `RT_CUSTOM`, `RT_DESTALPHA_SRCALPHA_CCW`, `RT_INVSRC_ALPHA_NOT_CULL`, `RT_UV_ANIM`, `RT_NONE`. About 25 June files use them.
   - The June-only `CUSTOM_RENDER_STATE` enum table truncates June's Add and Ara scripts on March's `Enum.lua` (checklist).
   - Swapping in June's `.fx` is a pure data change **only if** March's C++ sets the same effect parameters and techniques. That has to be checked, not assumed.
   - *Corrected 2026-10-16 (Stage 2 planning, `JUNE_ENGINE_REMAINDER_PHASES.md` M1/M2):* the seven names are not `RENDER_TYPE`s. They are `SKIN_MESH_RENDER_TYPE.SMRT_*`, already in March's `Enum.lua`; March implements only `SMRT_UV_ANIM` (phase R6 ports the rest). June's `RENDER_TYPE` equals March's (19). The real gaps: `CARTOON_TEX_TYPE` renumbered (19 ramps), 14 new `XRenderer.fx` parameters plus compile macros March never passes, two June-only renderers (`XEnvBoxRenderer.fx`, `XHeatShimmerRenderer.fx`), and `CUSTOM_RENDER_STATE`. So `.fx` adoption is **not** a pure data change: asset-port P10 waits for phase R2.
8. **Level cap.** March uses `LIMIT_MAX_LEVEL 70` (`X2Lib/X2Define.h:18`) and `CX2OfflineStatTable::MAX_LEVEL 80`. Stage 1's A2 takes it to 99, so no separate progression stage is needed.

---

## Stage details

### Stage 0: close the open loops

No new plan doc. Every item already has one, with kickoff prompts. What this stage adds is **which item gates which later stage**, so nothing is finished early for no reason and nothing is forgotten.

| Open item | Doc | Gates |
|---|---|---|
| Owed repacks and play-tests: Lanox P5/P10, UI parity 2 and 5, buff launch checks (3, 4a, 4b-i), effect 9 and 10, ExtraNPCStat P5 | the respective docs | **Stage 2 code.** Clear these first, so a later regression has a known-good baseline. Stage 1 follows its own docs' preconditions. |
| Effect rebuild 8b (31 keys, including `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID_RELATIVE_BUFF_TEMPLATE_ID` and `DAMAGE_RATE_TO_SUPERAMOR`), 9's `SetDamageExceptionProcess`, 10c, 11 (enum tables plus June-data trial load), 12 | `JUNE2017_EFFECT_REBUILD_PLAN.md` | Stage 5. The character checklist §6 requires 8b and the 11 enum tables before any character script. |
| Buff port 4b-i part 2 to 13, plus 14 | `JUNE_BUFF_PORT_PHASES.md` | **Phase 10 gates Stage 1's Tower (A10) and Memory lines.** All of it gates Stage 5. Port BBT 98/BFT 16 (skill-slot-type) code **dormant** now, as was done for the absent characters. Stage 3 wires them in. |
| Lanox P6–P9 | `LANOX_REMAINING_PHASES.md` | Stage 1's own docs say what they need. Close them before Stage 1's play-test sweep (A13), so Lanox defects are not mistaken for Atlas ones. |
| ExtraNPCStat P5 go/no-go, plus the "Lanox kills give 0 EXP/ED" follow-up | `LANOX_EXTRA_NPC_STAT_PLAN.md` | Stage 1. Atlas/Elysion monsters go through the same stat system. |
| UI parity follow-ups (`IsActiveDungeon` / `RequireCompletedQuestID`, secret-dungeon difficulty) | `JUNE_UI_PARITY_PHASES.md` | Stage 4 takes `RequireCompletedQuestID`. The rest can wait for Stage 9. |
| March migration Phase 7 (save migration, full verification) | `MARCH_2014_MIGRATION.md` | Before Stage 1's A2 EXP migration, so the save is migrated forward from a known state. The user confirmed this on 2026-10-03, and it is now in A2's dependency column in `ATLAS_PORT_PHASES.md`. |

### Stage 1: Atlas + Elysion (already planned)

Start from `ATLAS_PORT_PHASES.md`'s Phase A0 kickoff. It is one plan for both halves: the A phases own the shared groundwork (branch, census, RE maps, level cap, enums) and the Atlas half, the E phases the Elysion half.

**Why it is first among the new stages:**
- The user has already decided on it and planned it.
- It is content, with its own engine subsystems front-loaded (moving platforms, SimpleDLG gauges, dramatic messages, the script engine), and it needs no character or skill work.
- It delivers the level cap that transcendence assumes.
- Its quest split leaves Stage 4 a clean, well-defined job: activate the inert keys of three regions at once.

**Hand-offs to later stages:** the inert quest keys, the dormant quest-driven NPC swaps (Yuno → Hyper Yuno → Nono), and the deferred quest movies (e.g. the Elysion clear movie, quest 22336) all go to Stage 4. Record them in the Atlas/Elysion Done notes as they are found.

**Handed in by june-effect C9h-2e (2026-10-08):** Elysion E1's NPC relation system has three entry points, not one:
- village NPC creation `323ba5f0`;
- the per-quest refresh `32340630`, called from three quest-UI handlers;
- the code-event refresh `32344880`.

The last is the tail of `Handler_EGS_UPDATE_CODE_EVENT_NOT` (`31f00970`). On `main` that handler's flag-on block (`SERV_IRUHADEV_JUNE_PET_PREVIEW_LOOKUP`, C9h-2d) already copies June's map and two sets but does not make the call. Add the call there, behind E1's own flag; whichever branch merges second wires it. Rows: funcmap `Effect C9h-2e`.

**Handed in by june-effect C9h-5 (2026-10-08):** E1 step 6. June's `CX2TFieldNpc` is a rewrite. Its `OnFrameMove` has 44 blocks, and the house loader reads 56 keys March lacks. It includes the village-NPC effect set (`EFFECT_SET_NAME`, used only by the three Elysion houses) and its `+0xe0` engine branches, which sit next to C9h-3a's. Wait for C9h-3a to land on `main` before E1 ports them. The friendship branches go to Stage 18. Rows: funcmap `Effect C9h-5`.

### Stage 2: engine remainder and measurements

The planning session writes `JUNE_ENGINE_REMAINDER_PHASES.md` (full kickoff below).

*Planned 2026-10-16:* `JUNE_ENGINE_REMAINDER_PHASES.md`, STATUS block `june-engine`, phases R0-R6 (renderer), U0-U1 (UI framework),
W0-W1 (world timeline), Z. The measurements are its ledger. Corrections to the lists below: the "7 missing RENDER_TYPEs" are
`SKIN_MESH_RENDER_TYPE` cases (fact 7); the UI framework gap is small (no new control type; 15 methods and 21 keys in 30 / 21 of
1,519 June UI scripts), and the 2,107 `STR_ID_*` / enum-table misses June DLGs show are Lua data for the owning ports, not
framework; the effect-engine check is pending june-effect C11 (phase Z records it).

**Build:**
- the 7 missing `RENDER_TYPE`s in `KTDGXRenderer`, plus `CUSTOM_RENDER_STATE`;
- June's `.fx` set, adopted once the parameter and technique contract matches, with March's renderer brought to June wherever it does not;
- any other June engine-side difference that the census finds in renderer or skin-mesh code March already has. The no-partial rule applies.
- **`CX2World` world-event timeline** *(from june-effect C9h-7, 2026-10-08)*: June-only. `CX2World::OnFrameMove` `32802140` ticks `32801580` over three event tracks of the world `+0x34` object (0x58-byte -> `327fc8c0`, 0x38 -> `328004a0`, 0x3c -> `32800e10`: type 1 sound with the device auto-release, type 3 world `+0x18c/+0x190`, types 4..8 game / unit actions). Map the loader and the data first; reach in March data is unknown.

**Measure** (findings only; these size Stages 5, 8 and 9):
- **Models.** Per-character mesh, motion and texture diff between March (`24965799/kom_contents`) and June. Is there a new base model, new motions only, or nothing? See [[march-kom-contents-extract]] for the size rule.
- **UI framework.** Widget and control types or keys used by June `DLG_*` files that March's `KTDGUI*` parser doesn't know. If any exist, they become a Stage 2 build phase, because every later stage's UI depends on them. Check first whether Stage 1's census already covered this.
- **Effect-engine check** (the user's "still need checking"): one census plus trial-load run against the current tree. It confirms Stage 0's effect items really closed.

**Why here:** the render types are a hard dependency of June character effects, and the measurements turn guesses about Stages 5, 8 and 9 into numbers. Both are cheap compared with discovering them mid-character. Atlas/Elysion doesn't need them, which is why Stage 1 can go first.

**Scope audit (2026-10-20, june-engine).** The plan grew from 13 phases to 45 on 2026-10-16..18, before the scope rule. Checked against
the rule (`JUNE_ENGINE_REMAINDER_PHASES.md` "Scope audit (2026-10-20)"):
- Done before the rule and left done, but outside the subject: R12b's cash-emotion half, R13c (camera tracking / roll), R14 / R14b (equipment skins,
  attach-anim; Stage 11 / 16 territory), W1c-W1g (line-map `CanDown` / land-position callers, pet-AI re-seat, pathfinder fly).
- Moved: R4f (software cursor), R4h's notice text manager and R13d (camera callers outside the camera classes) -> Stage 9; W3 (village world Lua)
  -> june-dungeons (Stage 22).
- Left in the plan: R4c2 -> R4d, R4g, R4h (pass arguments), R4i, R4j, R5, R6, R7, R9, W2, Z.
- **Equip collision base `31fd8c00`** (pilot CH1l review, 2026-10-21): June picks each attach-anim equip's normal (`+0x544`) or hyper (`+0x584`) anim by templet `+0xd2` in {9, 11}; March always takes normal. Every class; unported (funcmap R12b). Its riding-pet branch is Stage 17's.
- **Accelerator auras** (pilot CH1 review, 2026-10-21): June's `CX2GUUser::ApplyAuraAcceleratorBuffToGameUnit` is `32028790` (user vslot `0x7b0`, same in the
  Arme and base vtables), March's base function's counterpart (C10b-2 called its host absent). It takes a 4th argument, the buff-factor index (`RET 0x10`; `3202c360` passes 0 or `322cc1a0`'s bool), and calls
  vslot `0x770` (`AdjustBuffFactorByIndex`, S3d) before the unit loop; its caller `3202c360` (`ProcessAcceleraterBuff`) has June's own skill switch (`0x7a45` / `0x7a59` ->
  `320284c0`, the `0x765d..` range re-applying skill `0x28ac`). Every class; port the pair whole with the C10b-2 site `@32028aa9`. March's Arme override (Energy of Plenty) is gone in June.
  Eve's June override `329b7390` (char-eve CH1o2) sits on March's 3-arg virtual and passes index 0 until this lands; it then becomes the 4-arg override.
- **`SubStageStart`'s user byte `+0x944c`** (char-eve CH1o2, 2026-10-27): June's `327cf620` user loop calls vslot `0x6ac` (= March's `CallSummonedNPC`),
  sets `+0x412c` = 1.0 (`SetShieldDamageReduce`), then clears a June-only byte `+0x944c` that March lacks. Identify its readers; every class.
- **Per-state immunity lists** (pilot-char-revamp CH1m, 2026-10-21). June `31fdaaa0`, called from `CX2GUUser::CommonStateStart` `@32057e68`, reads
  `IMMUNITY_LIST_AT_THIS_STATE` (ints into user `+0x8638`) and `IMMUNITY_EDT_LIST_AT_THIS_STATE` (`+0x8648`); March has neither key. Find the
  readers of `+0x8638 / +0x8648` (buff and extra-damage paths) and port the function with them. Aisha's June script has the EDT key in 12 states.
- **Attack-result / damage-react hooks beside the delegate lists** (pilot-char-revamp CH4b, 2026-10-22). Two June-only unit vslots run right after
  the lists: `0x1e8` (`32011b80`, after the `+0x66ac` run in CEffect / user / NPC AttackResult: BTI 0x382 Celestial Blitz, BFI 0x3a1, 1 s timer
  `+0x94f0`) and `0x1ec` (`32296570`, after the `+0x66dc` run in both DamageReacts: skill 0x7dcd). Also the NPC script key `ATTACK_RESULT%d`
  (`CX2GUNPC::InitComponent` `327a0fa0 @327ac7ef`): value 1 registers `3275a520` (holder of BTI 0x1d0 / 0x208 gives the hit target BFI 0x20e).
  Every unit; find the June data that reaches them first (funcmap rows "Char CH4b").

### Stage 3: skill system core (skill tree engine + skill slots)

**Sub-steps, in order:**
- **3a. Extract `NewSkillTempletVer2(_LA).lua`.** Fix or work around the round-trip failure, verify with `--check`, then write it to `june_src_fixed/data036_kom/`. Do the quest tables, `OptionItemData`, `PetTemplet` and `TitleTable` in the same pass; Stage 4 and later need them.
- **3b. RE June's numerical-skill-data override.** Identify the callees named in checklist §1, then port the override under its own flag. Every value that changes lands in `MODS.md`.
- **3c. Bring the skill templet loader and `CX2SkillTree` to June.** Port the full June parser, including keys for absent characters, stored and logged.
  - It includes `m_DeleteSkillIDByUpgrade` (templet `+0x178`; 615 uses in June's `NewSkillTempletVer2.lua`, none in March's), its `+0x20` old->new map (`32205c00`) and the user skill tree's upgrade swap (`328905b0`; june-effect C3l, funcmap `32216320 @3221851e-322186ca (C3l)`). A second June reader is `GetSkillPowerRateValue 32071720`: a non-zero field reads the power rate from the `32206fb0( 0xad, level )` lookup before the `+0x140` vector (C3l review, funcmap `32071720 (C3l review)`). The `_ULTIMATE` gates that are forced off for lack of them switch on together: `SPEED_TIME_ULTIMATE` (`SERV_IRUHADEV_JUNE_SPEED_TIME`), `BUFF_FACTOR_ID_ULTIMATE` (`SERV_IRUHADEV_JUNE_BUFF_FACTOR_ID`, `31fcf3e0`) and DamageEffect's `_ULTIMATE` keys (`SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE`).
- **3d. Slots: 4+4 → 5+5, with types A/B/TC.**
  - This is the shared struct (`CommonPacket.h`, flag-gated, offline-only), the client slot UI (`DLG_GAME_SKILL_SLOT_NEW_10slot`), the offline emulator handlers, and the save migration.
  - Take the `db_backup/` snapshot first, and chain the migration after Stage 1's A2 EXP migration.
  - Wire in the dormant buff BBT 98 and BFT 16 here.
  - The TC slot exists but stays locked until Stage 6.
- **3e. Skill tree UI and learn logic to June.** That means `DLG_SkillTree_*`, `DLG_Skill_Tree_Main`, and the emulator's learn/reset handlers.
- **3f. Running state data: June's pointer model (from june-effect C9h-6).** June keeps `m_NowStateData`/`m_FutureStateData` as pointers into the state vector (`+0x7cb0/+0x7cbc`); March copies by value. Decide whether to adopt the pointer model: only that model reproduces C9h-6's residual June behaviour after a mid-state slot re-equip, namely (a) a null snapshotted delegate falls through to the new entry's, (b) the new entry's skill id, hyper byte, `+0x11` and OnCameraMove go live, and (c) a second re-equip overwrites the snapshot. If it is adopted, port the `31fd2af0` snapshot (user `+0x9544..+0x9568`) and its four consumers with it. On March's copies the snapshot alone stays inert. The class-side call is checklist 7i.

**Why here:** this is the single dependency shared by every character stage, by transcendence, and by two buff types. It also closes the 0-damage blocker. The skill tree's *per-class content* is not here; it comes with each character in Stages 5, 7 and 8.

*Planned 2026-10-17:* `JUNE_SKILL_SYSTEM_PHASES.md`, STATUS block `skill-system-core`. It has these phases:
- S0a / S0b: RE;
- S1: 3a;
- S2: 3c loader;
- S3: the flip;
- S4: 3b;
- S5 / S6: 3d;
- S7: 3e;
- S8: 3f;
- Z: close-out.

**User decisions, recorded in that plan.** Both of them change the sub-steps above:
- **Per-class content is not deferred.** June's `NewSkillTempletVer2.lua` carries every class's tree: 3,360 rows, 3,237 of them
  reform ids. It is adopted whole in S3, and every save is reset onto June's trees. Unported characters lose most usable skills
  until their ports, so their class states are checklist rows.
- **3f's pointer model is adopted** (S8).

**Fact corrections (2026-10-17):**
- Fact 1 holds only for `NewSkillTempletVer2(_LA)`; the companion tables are already in `june_src_fixed/`.
- Fact 2: effect C1a already identified the override's callees (funcmap 1302-1306).
- Fact 3: the macro is at `CommonPacket.h:6411`, and `KUserSkillPageData` carries slot arrays too.

**Stage 3 closed (2026-10-19, skill-system-core Z).** Sub-steps 3a-3f are done: 3a = S1, 3b = S4, 3c = S2 / S3 / S3b / S3c, 3d = S5 / S6 / S6b,
3e = S7 / S7b-S7f, 3f = S8 (all play-tested; the two `Review PASS` rows are in the plan doc). Facts 1-3 above stand as corrected. What the plan added or routed:
- Playable-path phases kept in the plan: S3d-S3f (skill state), S7h (class change resets the tree), S7j / S7k / S7m (max MP / HP, socket aggregate, extra damage).
- Done before the 2026-10-19 scope rule and left done, but Stage 9 / Stage 16 territory: S7g (HUD stat singleton, cash-shop open-at-item list), S7i / S7l / S7n (level-up and new-title emblems).
- Routed, not run in the plan: S7o (boss-raid enter UI) -> june-dungeons C3z-8; S7p (character-window socket total `3291ba40`) -> Stage 9 (below); S7m's June-only item socket vectors -> Stage 16.
- Class states for the reform IDs are character-port rows (checklist 7ae, census `june2017_re/s3_reform_ids_no_state.tsv`: 1,989 of 2,044 reform rows of March's 55 classes have no state); the memo consumers (S3d) and the numerical override (S4) show their numbers only once a class port gives a skill a state.
- `MODS.md` carries the flag lines and revert steps; the exe, `data036` and the save (v17-v19) revert together. `SERV_IRUHADEV_JUNE_SKILL_DEBUG` was retired in Z (its MEMO lines never ran in a play-test; no behaviour change).

### Stage 4: quest system (including the deferred region quests)

This is the "later quest plan" that the Atlas/Elysion docs defer to.

**Engine:**
- Bring `X2QuestManager` to June:
  - the `QT_`/`QR_`/`SQT_` types March lacks (`QT_STORY`, `QT_CHANGE_TRANSCENDENTAL_CLASS`, `SQT_PVE_WIN`, `SQT_ITEM_EXCHANGE`, `SQT_IMMEDIATELY_CLEAR`, …);
  - `AddChainQuestDays`, `AddGroupChainQuest`;
  - the quest side of `AddVillageNpcRelationDataBy*` (Elysion E1 ports the mechanism);
  - the `RequireCompletedQuestID` gate left over from UI parity;
  - the quest UI.
- Bring `X2OfflineQuest` up so every new sub-quest type can actually complete offline.

**Content:**
- the Lanox, Atlas and Elysion quest rows;
- quest-gate C++ and the activation of the inert `REQUIRE_COMPLETE_QUEST_ID`/`REQUIRE_COMPLETE_SUB_QUEST_ID` keys;
- quest-driven NPC swaps;
- EventScene/GFx scenes and quest clear movies, which may need a Scaleform runtime decision;
- the old-save quest migration.

**Open decisions:**
- **Guild quests** (`QT_GUILD`, `AddGuildQuestTemplet_LUA`, `GuildCoopQuestRewardTable`) in a single-player offline build. Either parse-and-log or a real port.
- **Scaleform/GFx:** a runtime, or skipping the scenes with a log line.

Ask both before planning past the parser.

**Answered (user, 2026-10-17):** guild quests parse-and-log; Scaleform = a runtime, planned now as its own plan
(`JUNE_GFX_RUNTIME_PHASES.md`, key `june-gfx`; user-supplied GFx 4.2.22, June ran 4.0.15), scope event scenes + talk cut-ins, root-UI
screens stay Stage 9. The quest plan itself waits for Stage 1, skill S1 and june-gfx G0: brief `kickoffs/june-backlog/stage4-plan.md`.

**Scaleform runtime done (june-gfx, 2026-10-19; Z closed it):** G1-G5 and their reviews are PASS. The quest plan can call `PlayEventScene( name )` / `IsRecentlyPlayedEventScene( name )`
(`X2GFXEventScene`, flag `SERV_IRUHADEV_JUNE_GFX_EVENTSCENE`) and the scene's `GetGFXEventScene()` on `CX2Data`; 439 of the 493 scenes are Rose, 21 Ain, the Elysion quests 22329-22340 are the ones that reach today's
characters. Not ported by june-gfx and owned here: the quest UI callers `323cb2e0` / `323e4cb0` (`SetNotifyNewEpicQuest` is the hook for `+0x70`). The black dimming layer needs June's `GameMajorParticle_INT.txt` (asset port).
See `MODS.md` "June GFx runtime: close-out".

**Planned (2026-10-19):** `JUNE_QUEST_SYSTEM_PHASES.md`, key `quest-system`, phases Q0a-Q9 + RM0 / RM1 + Z (`kickoffs/quest-system/`).
The planning census (`june2017_re/quest_plan/`) found June's data replaces March's wholesale (430 June-only and 257 March-only quests,
8 new sub-quest types), so the plan flips the data in one phase (Q2) with an old-save migration, as skill S3 did. June's random
mission system (carried in from skill S3c) is RM0 / RM1.

**Why here:** transcendence is quest-gated, and three regions' worth of inert quest keys wait on the same engine. Doing quests once, after all three regions are in and before transcendence, avoids porting the quest engine piecemeal inside each feature.

### Stage 5: pilot character revamp

- **Pick one March character**, by the smallest `june2017_re/charport_scan.py` gap and Stage 2's model diff. Then work through `JUNE2017_CHARACTER_PORT_CHECKLIST.md` end to end for it:
  - its June scripts;
  - its June-only later class path;
  - its per-class skill tree data;
  - its model and motions (if Stage 2 found changes);
  - its `UNIT_CLASS` additions (shared header, flag-gated);
  - its server-side per-level stat Lua (user packs).
    - For classes March already has, rows 81–99 follow Stage 1's A2 method.
    - For a **new** class ID (a June-only later path), see the stat-row decision below. A2's permission to invent rows does not extend to it.
- **`ADD_RENA_SYSTEM`** needs the user's decision if the pilot is Rena; otherwise it waits for Stage 7.

**Why a pilot:** every character repeats the same pipeline. Discovering a pipeline defect once, on one character, is far cheaper than discovering it across seven.

**Planned 2026-10-20 (`/x2-phase new-plan pilot-char-revamp`): the pilot is Aisha (user).** The plan is `JUNE_CHARACTER_AISHA_PHASES.md`, with
kickoffs in `kickoffs/pilot-char-revamp/` and the STATUS block `pilot-char-revamp`.
- **Phases:** CH0 map, CH1 script + motions + class core, CH2 / CH3 skills by job tier, CH4 passives + memorize, CH5 `UnitTemplet` + cut-ins + UI pieces, CH6 walk, Z.
- **Gap scan** (`charport_scan.py`, attacks whose damage is numerical-only): Aisha 1, Eve 3, Rena 12, Ara 14, Chung 15, Elsword 20, Elesis 20, Raven 25. Add's March kit is a skeleton.
- **Model diff (M3):** no new Aisha motion file, no renamed animation.
- **The class-path step is not exercised.** All seven of Aisha's June classes are in March already, and third jobs are out of scope (below). That step is
  written as the template block CH-N, and Ara, Elesis or Add runs it first in Stage 7.
- **Stat rows:** every Aisha class already has rows 1-99.
- **`ADD_RENA_SYSTEM`:** waits for Rena in Stage 7.
- **The Stage 7 template** is a section of that plan, and its Z rewrites it from what the pilot took.
- **Closed 2026-10-22 (pilot-char-revamp Z).** Aisha's seven classes run on June's class core, skills, passives, cut-ins and memorize HUD (about 96 commits; CH1 alone needed 14 sub-phases). The corrected phase sizes, the recipe defects and the Stage 7 order are in `JUNE_CHARACTER_AISHA_PHASES.md` "Template for Stage 7" and `kickoffs/june-backlog/stage7-plan.md`; Stage 6's brief is `stage6-plan.md`. Waiting on Stage 6: the transcendence slot (A-5 / F), passives 22415 / 22416, the transcended hyper-mode branch, the awakening effect-set family. Routed elsewhere during the pilot: Stage 2 (base `32028790`, vslots `0x1e8` / `0x1ec`, NPC `ATTACK_RESULT%d`), Stage 8 (`322f0e60` caller Lied Schwach), Stage 9 (`m_bShowSkillSlot` writers, `ShortCutKeyProcess`), Stage 12 (cut-in item override), Stage 17 (riding pet part), Stage 22 (dungeon attribute bit).

### Stage 6: transcendence system, on the pilot

- Transcended classes (decide how June represents them: `UC_*_2` or a flag; RE first).
- The class-change quest (`QT_CHANGE_TRANSCENDENTAL_CLASS`), `TranscendentalClassItem`, and the guide UI.
- Unlocking the TC slot, `TRANSCENDENTAL_SKILL_DATA`, and `USI_TRANSCENDENTAL_HYPER_MODE`.
- Transcendence sockets (`SDT_*_TRANSCEND*`) and emulator support.
- **Awakening effect-set family** (pilot CH1b, 2026-10-20). **Moved to Stage 12 by the Stage 6 planning (2026-10-22):** `+0x9578` is a wstring (the set name), not a map. `32011d20` fills it from the applied-customize map (CX2Unit `+0xa48` key 15, `325d2370( 3 )`), so it is a customization item effect. Only `Effectset_TRANScendence_WakeUp_FX` (`32298590` -> `+0x9f8`, stop `32296cb0`) is transcendence; it is `june-transcend` T5. Eve CH1k (2026-10-26): `31febf50`, the Dark Dragon hyper-mode effect set of the items `0x39395d3` / `0x5f5e6e6` (position 20; game mode 2 / 3 -> `EffectSet_DarkDragon_HyperMode_Dungeon` / `_Pvp`), is the same family and is called by each class's CommonHyperModeFrameMove; Eve has a no-op stand-in (`JuneEveHyperModeItemEffect`), March counterpart none, offline reach only with those cash items.
- **Skill cut-in art** (june-gfx Z, 2026-10-19): the user skill cut-in is June's GFx widget set (flag `SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN`); the `HQ_CutIn_*.gfx` art ships in the staged data152 / data195. A transcended class's cut-in
  names must be June's `.gfx` names, like any class port (`JUNE2017_CHARACTER_PORT_CHECKLIST.md`, after 7t; candidates per prefix in `june2017_re/g4_cutin_names.tsv`).

**Live-DB gate:** if transcendence uses new class IDs, see the open decision on `GBase_Item` below. *Answered by the planning (2026-10-22): it uses none.*

**Why after Stage 5:** transcendence skills are part of June's revamped kit, so transcending a March-kit character would be ported twice.

- **June's resonance system** *(from june-result R0, 2026-10-22)*. **Identified by the Stage 6 planning: its own stage, Stage 30.** It is a level-99
  progression, not transcendence.

**Closed 2026-10-24 (june-transcend Z).** T0-T8 done, walk passed by the user. Aisha's EM / VP / DW transcend through quest 42030, the items or `/tc`; the unit flag (save v22), guide popup, items, TC slots A-5 / B-5 with the glow, awakening engine, class pictures and passives are live. Stand-ins: the class allow-list (each Stage 7 / 8 port adds its class), key 1 permanent. T7 sockets closed by T0. Per-class work lives in checklist 7ao and the Stage 7 template. Routed out: awakening customization effect -> Stage 12, resonance -> Stage 30, achievements -> Stage 31, June UI readers of the flag -> Stage 9, wire 2-byte class -> Stage 8, result-screen class image -> Stage 29, the 32869940 job-ticket helper -> Stage 9. MODS.md has the close-out section.

*Planned 2026-10-22 (Opus 5.5, xhigh):* `JUNE_TRANSCENDENCE_PHASES.md` (T0-T8, Z), key `june-transcend`, start `/x2-phase june-transcend T0`.
- June marks a transcended unit with an int at CX2Unit `+0x3c`, beside the class at `+0x38`. It travels in `KUnitInfo` and the room / field /
  square infos (setter `3235eb20` and three siblings), and in June event 0x596 (`31ef82f0`).
- A unit becomes transcended through quest 42030 ("Limit Break 5/5", `QR_TRANSCENDENTAL_CLASS`) or the `TranscendentalClassItem` tickets.
- No class ids are added. Evidence: `june2017_re/transcend/`, funcmap rows `Transcend S6`.
- Routed: the awakening customization effect -> Stage 12; resonance -> Stage 30; achievements (`ACT_TRANCENDENTAL_CLASS_COUNT`) -> Stage 31.

*T0 done (2026-10-23):* the readers are mapped (`june2017_re/transcend/t0_readers.tsv`; 35 class rows in checklist 7ao). Event 0x596 is the
NOT of the pair 0x594 / 0x595 that `/tc` sends. The ticket rule is decoded (type 0 before, type 1 after transcendence). T7 (sockets) is closed.
Routed by T0: the June UI readers -> Stage 9; the wire's 2-byte class -> Stage 8; the result screen's class image -> Stage 29.

### Stage 7: remaining March characters

- Repeat Stages 5 and 6 per character with the proven recipe.
- One phase doc per character, or one doc with a phase per character; the planning session chooses.
- **Eve planned 2026-10-25** (`new-plan char-eve`): `JUNE_CHARACTER_EVE_PHASES.md` + `kickoffs/char-eve/` + the `char-eve` STATUS block. One doc per character. Eve is about 1.5x Aisha (643 carved June functions), so CH1 is 17 sub-phases up front. Found: the Nasod Core (`ADD_EVE_SYSTEM_2014`, dormant in March, live in June) is Eve's phase CH1q; the user chose March's dormant code as its base (decisions table).

### Stage 8: new characters (Lu/Ciel, Rose, Ain)

Everything from Stages 5 and 6, plus:
- a new `UNIT_TYPE` (shared header, flag-gated);
- character creation and the emulator's create-unit path;
- `Dungeon_Tutorial_{Luciel,Rose,Ain}`;
- Lu/Ciel's duo mechanics (`DUO_TYPE`, `HIT_ADD_COMBINATION_POINT`) and Rose's `ROSE_WEAPON_TYPE`;
- activating the dormant Rose/Ain/Lu/Ciel code from the buff port.
- the user-sync members only these classes drive (june-effect C3e, `june2017_re/c3e_user_sync_fields.tsv` rows B, E, F, H, I, G):
  - Lu/Ciel's dual body: the second-body position (bits `0x20000`-`0x80000`) and the active body (`0x1000000`), on `+0x8e88` / `+0x8e90` / `+0x8e91`;
    with it PhysicProcess's not-selected-body branch (`32015b30 @32019f9b-3201a1c7`, funcmap `(C9h-4g)`), the only reader of
    `CUSTOM_OFFSET_NOT_SELECT_UNIT` / `USE_NOT_SELECT_MOTION_DIFFERENCE` (parsed since june-effect C9h-4g: `JuneGetNotSelectUnitOffset( i )`,
    `JuneGetUseNotSelectMotionDifference()`; June data: `Luciel_Demonio`);
  - Ain's `USEFUL_STRING` readers: 15 class-code callers of the getter `32a78fa0` (ported as `JuneGetUsefulString( index, bFuture )`, june-effect
    C9h-4g) and the static `ParseUsefulString( table, vector )` helper `31fea4d0` (6 Ain callers, not ported); June data: `Ain_Apostasia`,
    `Ain_Erbluhen_Emotion`;
  - Lu/Ciel's float (`0x10000`); the class short of Ain (Disparate Force), Rose and Lu/Ciel (`0x100000`); Ain's 10-dword array (`0x20000000`);
  - their drivers of the class-skill float C3e-1 ports (`0x200000`), including Rose's Neil the Sniper aim (`3200d3f0` / `3200d900`).
- the item-templet fields June-item-kim K0 found for them: Lu/Ciel's minor-unit item id and flag, the Extra keys 10-13,
  and their item-type and unit-class guards from K2 (character-port checklist 7g).
- Ain's SetStopTime / SetStop2Time overrides (june-effect C3e-3, funcmap `32a38e70 / 32a38ea0 (C3e-3)`): the stop time times Ain's
  `+0x97ac`, then the user's 4-argument setters that C3e-4 ports.
- Ain's Lied Schwach ( `RNSI_A_ALA_LIED_SCHWACH` 32218 ): its target search `32a71030` ( thunk `32a7a210` ) is Energy Drain's shape and calls the
  view cone `322f0e60`, which pilot CH2c ported as `CX2Game::JuneIsUnitInViewCone` ( funcmap `322f0e60 (CH2b)` row ); reuse it, do not re-port.
- **Event scenes and cut-in art** (june-gfx Z, 2026-10-19): 439 of the 493 GFx event scenes are Rose quests and 21 are Ain (`GFXEventSceneInfo.xml`, scene `.swf` loose in `data\eventscene\`, all staged); they play once the quest
  system and these classes exist. Rose / Ain / Lu-Ciel cut-in names are June `.gfx` names in data152 / data195 (staged by june-gfx GA); name them in the class tables when the classes are ported.

- **June's 2-byte unit class on the wire** *(from june-transcend T0, 2026-10-23)*. June reads the class as a word in every unit-info struct
  (`KUnitInfo +0x15`, Room `+0x21`, Field `+0x24`, Square `+0x4a`: `movzx ... word ptr` in `3235eb20` / `32360090` / `323614c0` / `32361e00`);
  March's `m_cUnitClass` is a `char`, which ends at 127. Check it when a class id above 127 first appears (the new characters). Transcendence
  only adds the int after it (T1).
**Hard gate:** the starter-item rows (below). The live DB has none for these classes (user, 2026-10-27), so each character's planning session derives them from `item.kim` by the name pattern of the existing classes' starter items, as a labelled stand-in under that character's own flag.

### Stage 9: UI sweep

Port whatever June UI is still March-shaped once Stages 1–8 have delivered their own dialogs.

- **Two result-flow leftovers (june-result R3, 2026-10-22).**
  - June's `AddTimedMessagePopup` (`31d94a20`) takes one more argument than March's, an ( x, y ) pair after the time-out message (the result
    retry box passes ( 230, 200 ), constants `33478a84` / `33457460`); R3 calls March's form. Read what June does with it before porting.
  - The exit box's Quit (`31f23e70` case 0xbff `@31f25076-31f250fe`) has PvP-state branches: confirm already sent in state 9 -> the
    `g_pMain +0x408` test (not 0xc-0xf) and game vslot 0x138; then party manager `324334d0( 0x6b3 )`. They go with the PvP result.
  - **Tab on the PvE / PvP results (june-result R3b, 2026-10-22).** The PvE result adapter (ctor `32ee8620`, made by `324ac130`) and the PvP
    one (ctor `32eee330`, made by `3251d3a0`) register the same `GameResult.onTabKeyProcess` trigger as the dungeon result (`32eea070`). Who
    notifies theirs is not mapped (`324adc20 @324adf87` calls a vslot 0x294 after a dynamic_cast: a candidate). R3b ported only the dungeon state's
    (funcmap `(Result R3b)`). The dungeon adapter's `32eea050` (notify `UIDungeonResult.hideUI`) has no caller found.
  - **GFx callbacks after a widget reload (june-result R3c, 2026-10-22).** June's result callbacks ignore their owner and act on `g_pMain +0x90`
    at call time, so a handler kept on a persistent AS object still works; ours dies with the bridge that made it (`CJuneGfxFunctionHandler`,
    `KTDGGFXManager.cpp:693-707`). R3d fixes the dungeon result only. The event-scene and talk-cut-in bridges use the same owner link
    (`KTDGGFXManager.cpp:1848`, `:2056-2066`) and June's same Call `32eee800`. Whether their June targets read the owner is not checked; a widget
    reopened in one session would fail the same silent way. Map: `june2017_re/result_r3c_map.tsv`.

- **June's menu bar and its StoryBook button** (handed here by quest-system Q10, user 2026-10-19: "no opener; Stage 9 owns it").
  - **Bar, EXP and stamina gauges and the button row moved to Stage 32 (user, 2026-10-24).** Stage 32 draws every June button; a click whose
    feature is missing logs `[JUNE_BOTTOMBAR] STUB <name> -> Stage 9`. This stage owns those features (final list: `june2017_re/bottombar_buttons.tsv`, owner `Stage 9`):
    - the StoryBook button's real handler (`ToggleUI( 0x2f )`, `g_pInstanceData +0xabd = 0`) with `CX2UIStoryBook`, and the `Story_New` badge from `+0xabd`;
    - **June's profession system** (button `SMUCM_PROFESSION_LIST`, June case `0xb2` -> menu 0x22): `CX2ProfessionManager`, GFx `UIProfession`
      (`CX2UIProfessionAdapter`, `UIProfession.showUI` / `.hideUI`, `notifySelectProfession` / `notifyChagneProfession`), `DLG_UI_Job_professional.lua`,
      `DLG_UI_Change_Profession.lua`, `DLG_UI_Profession_Change_Item.lua`, the blacksmith / alchemist / treasure-hunt level-up effects. A whole system:
      the Stage 9 planner may give it its own stage;
    - **rest EXP** (buttons `SMUCM_CHANGE_TO_REST_GAGE` / `_EXP_GAGE`, June `324700a0` / `32470210`): June-only unit int64s `+0xac0` / `+0xac8` (from June's
      `KUnitInfo`), a per-level cap table, level < 99; the emulator would need a labelled stand-in to earn it;
    - the 2016 Valentine UI (`SMUCM_EVENT_2016_VALENTINE_UI`, case `0x40b`, event 0x8aa) and the include's event icons and event-EXP gauge
      (`3245d870`, `Static_EventEXP_BG`), with the `EventButtonAddSystem` bullet below.
    - **B0 final stub list (2026-10-24):** `PROFESSION_LIST`, `STORY_BOOK`, `CHANGE_TO_REST_GAGE`, `CHANGE_TO_EXP_GAGE`, `EVENT_2016_VALENTINE_UI` and
      `FRIEND_RECOMMEND_OPEN` (Recruit-a-Friend: June `324633e0` + informer `32854a20`; the button shows only under code event 2206 = 0x89e and level >= 10).
      `SHOW_ADVERTISEMENT` is not a stub (the browser exists; B4 ports it). Also here, all June-only code events the bar reads (absent from March's
      `CODE_EVENT_ID`, so they read off): 2220 accompany-pet (Pet never hides, `AccompanyPetTag`), 2316 (StoryBook hidden, row closes up), 2155 (Guide sets
      singleton `324943c0 +0xd1`), 2040 / 2140 / 2199 (`DiscountTag` / `FeverTag` / `PPoruTag`), the include's 2209 / 2222 / 2279 / 2302 / 2258 / 2274, the
      `GrowKnowHow` icon (`3245da70`), and `CX2StateMenu +0x43d` (a Lua binder `31fc3f80` next to `CheckEventTriggerNPCPos`; set, it hides Guild).
      Evidence: `june2017_re/bottombar_functions.tsv`.
    - **stamina (spirit) model (found in B3, 2026-10-24):** the emulator pins spirit full (`CX2OfflineServer::FillSpirit`, `SHRT_MAX` / `SHRT_MAX`), so June's stamina
      bar (`32475480`, ported in B3) reads 100.0% and never moves. Spending and restoring spirit per dungeon run (and the entry check) would need its own
      model in `X2OfflineDB`; June addresses: `RefreshMenuInfo` `32475480`, unit `+0x950` / `+0x954`. Not needed to play.
    - **June's guild UI (found in B4, 2026-10-24):** the Guild button's June body is `CX2UIGuild::SetShow( true )` (`325ab110`), which builds `DLG_Guild_Info_New_3.lua`
      (March opens the messenger's guild tab). B4 ports the gate and logs `STUB SMUCM_GUILD -> Stage 9`; offline has no guilds (`Handlers_Social.cpp`), so the button is hidden and unreachable.
    - **Surprise Sale (found in B4):** June's Cash click first runs the singleton `32696fa0` + `3267ac90` (`BtnSurpriseSale_%d` buttons, `+0x4fc` list); March has no such system (grep "SurpriseSale" is empty). Not ported.
    - **Cash / Warp refusals on June-only state (found in B4):** `CX2State +0x16c` busy (16478), `CX2PartyManager +0x70` (16478), the 1v1 duel request `+0xda` (39561, also Warp's case), and
      `PartyData +0x10 == 6` tournament (42839) have no March member, so B4 reads them as 0. They arrive with a duel / tournament system if one is ever ported.
  - **The story book UI itself (quest-system Q10c) moved here too** (user, 2026-10-19): port `CX2UIStoryBook` + menu 0x2f together with the button.
    Scope: `JUNE_QUEST_SYSTEM_PHASES.md` "Phase Q10c - Story book UI"; kickoff `kickoffs/quest-system/Q10c.md` (its dev-console opener becomes optional once the button lands).
  - June opens the story book (quest-system Q10c's `CX2UIStoryBook`, UI manager menu 0x2f) only from the `StoryBook` button of June's own
    `DLG_Menu_Button_NEW.lua` (data034; `DLG_UI_Button27` sheet, slot layout `MBW_GET_SLOT_LEFT_TOP`, 18 `STATE_MENU_UI_CUSTOM_MSG` ids, the
    `EXP_Y` gauge skill S7i held back). March still runs its own bar, so the book has only a dev-console opener until this lands.
  - The button: `SMUCM_STORY_BOOK` 181 -> state-menu handler `32478e90` (in `32476050`): `ToggleUI( 0x2f, !IsShow( 0x2f ), 0, 0 )`, then
    `g_pInstanceData +0xabd = 0` (`32478ecc`). The `Story_New` mark: `324708c0` @32471129 shows it from `+0xabd` (hidden in state 0xb); placed by
    `3245cba0`, the bar layout `3245bb70`. Quest-system Q10 writes `+0xabd` (`CX2InstanceData::m_bJuneStoryBookNew`).

- **The own gauge's wake-orb tooltip** (handed here by char-eve CH1s, 2026-10-27).
  - June's `DLG_PVP_Game_My_State_NEW.lua` (live since CH1s) has two invisible buttons, `opacity1` (orbs) and `opacity2` (bars), that send
    `GAGE_UI_CUSTOM_MSG.STUCM_CHARACTER_SYSTEM_TOOLTIP_ON` / `_OFF` (54002 / 54003) with dummy int `UI_TOOLTIP_TYPE.UTT_WAKE_ORB`.
  - March's `CX2GageManager::UICustomEventProc` knows only 54000 / 54001, so nothing shows. Find June's handler (xrefs to 54002) and port it.
- **Effect-alpha option UI** (handed here by june-effect D9, user 2026-10-06).
  - June-effect C10b-2 builds the engine: the three values in `GameOptions.lua` under June's keys (`MY_EFFECT_ALPHA`,
    `MY_TEAM_EFFECT_ALPHA`, `ANOTHER_TEAM_EFFECT_ALPHA`), and `CKTDXApp::SetEffectAlpha` / `GetEffectAlpha`.
  - The UI only has to read and write those.
  - June's version is the GFx option window (funcmap `32eddbb0`, `32eddcf0`, `32edee60`, `32f246d0`).
  - Brief: `kickoffs/june-effect/C10b-3.md`.
- **The non-tutorial rest of three June UI rewrites** (handed here by june-buff 14c, user 2026-10-07).
  - 14c ported only their tutorial-flag terms, under `SERV_IRUHADEV_JUNE_TUTORIAL_MODE`: `CX2UIManager::SetShowUI` (June `32154820`), `CX2UIQuickSlot::SetEnable` (`3294c460`) and `CX2UIQuestNew::ToggleUserShowQuickQuestDLG` (`323ddfe0`).
  - What is left in `SetShowUI`:
    - the shop-open early return (`g_pData+0x290 +0xd8`);
    - the state switch around the now-state call (vslot `0x104`);
    - the quick-slot terms `3266d250` and training -> hidden;
    - the minimap call (`32481fa0` / `+4 == 6`);
    - the gage-manager cooltime UI (`324943c0`, `32489e30`, `3248ac30`);
    - `32150f50`, `32151980`, `32457e70` / `32457b40` and `32676150` / `326765f0`;
    - the guide-manager step gates on the skill and riding slots (`+0xe4 < 3`), which need the June-only `CX2TutorialGuideManager`.
    - the tutorial key mask `320708f0( InputData&, guide step +0xe4, unit +0xaf0 value )` (zeroes the held keys per step; step -1 does nothing), called by `CX2GUUser::InitInpuDataProcess` `31fcea10` and Eve's `32978e80` (handed here by char-eve CH1l, 2026-10-26; the Eve port leaves the call out).
  - What is left in `SetEnable`: the `+0x100` / `+0x101` slot-count modes and the unit `+0xdba` term.
  - What is left in the toggle body: the double refresh `31c91850`, the June-only `g_pMain+0x2aa` mirror and the `32e904d0` call.
  - Rows: `june2017_re/c14c_tutorial_reads.tsv`; Done note: `JUNE_BUFF_PORT_PHASES.md` "Phase 14c — Done".
- **June's code-event menu icons (`EventButtonAddSystem`)** (handed here by june-effect C9h-2e, 2026-10-08). March has no counterpart; its menu hard-codes `IF_EVENT_ENABLED` blocks.
  - Data: `data036` `EventButtonIconInfo.lua`, 29 rows on 6 states, loaded by `AddEventButtonIconInfo` (`31ee94a0`, into `g_pData+0x584`, 0x58-byte rows) and `AddEventButtonIconUsingState` (`31ee39f0`, set `+0x594`).
  - Runtime: three `CX2State` virtuals that June inserted. In the base they are empty.
    - `CX2StateMenu` `+0xfc` adds one icon (`324637d0`, layout `3245d2b0`).
    - `+0x100` refreshes icons by code event and level (`3245d3a0`). `Handler_EGS_UPDATE_CODE_EVENT_NOT` calls it last; C9h-2d left that call out.
    - `+0x104` handles the class-up notice buttons (`3245abd0`).
  - Check against March first: the `STATE_MENU_UI_CUSTOM_MSG` ids the rows send, and the 29 `DLG_UI_Menu_*_Event*.lua` dialogs.
  - Inert offline: nothing enables a code event, because the emulator never sends the packet.
  - Rows: funcmap `Effect C9h-2e`.
- **June's lobby rotation keys in `UnitTemplet.lua`** (handed here by the june-effect C3t review, 2026-10-14). March has neither key.
  - Parse: `AddUnitTemplet_LUA` `3237aa40` reads `m_fLobbyRotationY` (`@3237efaf`, stored at parser `+0x2d8` = templet `+0x2d4`) and
    `m_fMinorUnitLobbyRotationY` (`@3237f144`, `+0x2dc` = templet `+0x2d8`). They come right after `introMovieFIleName` (`32fea370` float getter).
  - Reader: not identified yet. It is probably the character-select / lobby model. Find the readers of templet `+0x2d4 / +0x2d8` first.
  - The minor-unit key belongs to Lu/Ciel (Stage 8).

- **June GFx owed gates that need a screen** (handed here by june-gfx G5, 2026-10-19). G5 ported the mechanism (`isOpenUI( 0xb )` in the cut-in term, `closeToTopZorder` in the ESC chain, the dialog hide modes with all five presets, `JuneSetShow( bShow, mode )`). What waits for these screens:
  - **Cash shop = GFx UI `0xb`**: opened by June's cash-shop code (`32228478` / `3223dd01`) through `openUI( 0xb )`. Until it exists the cut-in term stays false.
    **ESC does not close it in June either** (G5 review follow-up, 2026-10-19): the AS `closeToTopZorder` answers false for `0xb`, and the
    field states' ESC handler (vslot 0x1ec `32464b20` @32464c13) sees the cash shop open (state `+0x2f4` = `g_pData +0x290`, `+0xd8`), calls
    `322273f0` (closes only an inner popup: `+0x3b0` and `+0x358` -> `32e3e8d0`) and returns true, so ESC is swallowed and the shop stays.
    It closes through `CX2CashShop::SetOpen( false )` (`3223d680`, `unloadWidget( 0xb )` @3223e164). Port all three with the screen;
    until then a hand-opened `0xb` (G5 debug) has no owner: ESC opens March's menu and nothing unloads the widget.
  - **Hide modes 2-5 are entered only by June's result-UI / presentation code**: `31f14e40`, `324c95b0`, `324d8600`, `3243be8b` (mode 2), `324ac353` (mode 4), `328c3750` (mode 5, `PresentProlBlackBack`). Each calls `CKTDGUIDialogManager::JuneSetShow( false, mode )`; the lists are ready (March has no caller). `31f02f50`'s toggle and `31d03a10`'s ESC rule already handle them.
  - **The result-UI mode `+0x210`** at `31f1e214` (setter `31ef7be0`, exit box `31f1dd70`, writers listed in the C3p-2 Done note) and the **profession-manager close** at `31f1e27a` (`CX2UIProfessionManager` `32396dc0`, SetShow `3239d190`).
  - **Moved to Stage 29 (user, 2026-10-21):** the result-UI mode `+0x210` (`31ef7be0` / `31ef7390` / `31ef7c60`, ESC `@31f1e214`,
    exit box `31f1dd70`) and the dungeon flow's hide modes 2-4 (`324c95b0`, `324c966e`, `324d880f`, `324cf508`) go to
    `JUNE_DUNGEON_RESULT_PHASES.md`. What stays here: the profession-manager close `@31f1e27a`, PvP's mode 3 (`3251d63d`), PvE's mode 4
    (`324ac353`), mode 5 (`328c3750`) and the `31f14e40` / `3243be8b` callers, unless that plan's R0 finds them on the dungeon path.
  - **The tutorial guide ESC gate** stays with C3z-4t (june-dungeons). **The GFx show byte `+0x108`** is already written by the toggle; the other writers (`31f25112`, state dtors `324ad780` / `324d5670` / `32520d70`) come with their states.
- **June's GFx root-UI screens and the other wrapper callers** (handed here by june-gfx Z, 2026-10-19). june-gfx loads `main.gfx` as the host of event scenes and cut-ins and ports no screen. What is left in June's `main.gfx`:
  the cash shop (UI `0xb`), the option window (effect-alpha item above), the PvE / PvP result screens, bingo, the job-professional manager and the tutorial guide. Most of them bypass the 14 AS wrappers
  that `CKTDGGFXManager` already carries (`openUI`, `ShowUI`, `HideUI`, `loadUserWidget`, `unloadWidget`, `showOkPopup`, `changeCharacter`, `clearFocus`, ...): June uses 13 adapter / callback pairs and about 39 `DW_*` wrappers,
  none ported (`june2017_re/g0_gfx_map.tsv`, funcmap `GFx G0` / `GFx G2`). The runtime pieces a screen needs are there: loader, kom-first opener, font config "US", data bindings (G3's wrapper), listener / adapter table, `SimpleWidgetPlayer`.
  The IME text field stand-in (id 0) and the resize path (DXUT reset) are G2's placeholders for the first screen that needs them.
  **Correction and move (2026-10-21):** the result screens are their own widget files in `data194`, not part of `main.gfx`:
  `dungeon_result.gfx`, `PVE_result.gfx` and `BATTLE_PVP_result.gfx`, each with 13 `.dds`, all staged by june-gfx GA. The **dungeon**
  result (`CX2UIDungeonResultAdapter` `32eea7e0`, UI 8) moved to Stage 29. The PvE result (`UIPvEResult`, `XS_PVE_RESULT` `324ab1c0`)
  and the PvP result stay here.
  *June-result R0 (2026-10-22):* `CX2StatePVEGame` (vtable `3349d06c`) shares the dungeon state's retry vslots 0xe0 / 0xe4 / 0xe8 / 0xec
  (`324d8600`, `324c83f0`, `324d11e0`, `324c9990`), so the PvE result's retry is the same code R3 ports; its mode step is `324ac1f0`.
  *June-result R1 (2026-10-22):* the dungeon fill `324d60e0` opens with two walks over the dialog stack (`31d05fe0` / `31d05ec0`: for every dialog whose byte `+0x144` / `+0x146` is 1, `FUN_31d08590` -> Release -> delete) on the object at `g_pKTDXApp +0x1fc +0x38c`, run when the state's vslot `0xf8` is false. Not ported: no March counterpart found (probably a June-only `CKTDGUIDialogManager` close-on-result flag pair). Offline reach: every dungeon clear. Related: funcmap `(Result R1)` row `324d60e0`.
- **June's character-window socket total** (handed here by skill-system-core S7p, user 2026-10-19, scope rule). June `3291ba40( out, group )` / `3291cee0` / `3291cfb0` / `3291b4b0`
  are June's form of March's `CX2UICharInfo::GetMyEquippedSocketData()` (`X2UICharInfo.cpp:2474`, read at `:1423`): an equip-group filter (`3286a4b0`; `32350b80`'s group test, S7m review), riding pet, title,
  sets and the party pass (`31ed40c0`, already ported as `CX2Unit::JuneGetPartyShareSocketOptions`). The combat sums are June's since S7k / S7m; only this display total is March-shaped.
  Brief: `kickoffs/skill-system-core/S7p.md` (identify, then port whole; June-only item data `+0x8c` / `+0x9c` / `+0xac` stays with Stage 16).

- **June's software cursor `CKTDXCursor`** (handed here by the june-engine scope audit, 2026-10-20; was june-engine R4f). KTDXApp `+0x290`
  (vtable `3344c980`, ctor `31ce50e0`, dtor `31ce5090`, draw `31ce5260`), drawn last in the frame by `31ce5400( 0 )` (`31be3800 @31be3c7c`); March has
  no such class. Its only writer is the GFx event scene's Play (`3226b130 @3226b56b` -> `31ce5520( 1 )`), so june-gfx G6's
  `SERV_IRUHADEV_JUNE_GFX_CURSOR_ORDER` stand-in covers play until it lands; the port retires that flag. Brief: `kickoffs/june-engine/R4f.md`.
- **June's notice text manager** (handed here by the june-engine scope audit, 2026-10-20; was half of june-engine R4h). June's second
  `CX2TextManager` at `g_pMain +0x444` (made @31e4abb5, freed @31d93b3f, frame-moved @31f0d945), written only by `CX2NoticeManager` (`+0x448`, ctor
  `325ad9a0`, `325acf90` / `325ad410`) and drawn after the frame by `CX2State`'s `0x18` slot (`31ef76b0`, `32744860( 0 )`). Map the notice manager
  against March's `X2NoticeManager.cpp` and port what differs whole. The pass argument of `32744860` stays with june-engine R4h. Brief:
  `JUNE_ENGINE_REMAINDER_PHASES.md` "Phase R4h - ..." (the notice bullet) and `kickoffs/june-engine/R4h.md`.
- **June's camera callers outside the camera classes** (handed here by the june-engine scope audit, 2026-10-20; was june-engine R13d). R13c
  ported `CKTDGCamera` whole; its new members have outside callers: the shake lock set by `CX2CashShop::SetOpen` `3223d680` (`@3223e0a5`,
  `@3223e144`; goes with the cash-shop screen bullet above) and cleared at the game-camera creation `322dfed2 @322dfedb`; `PointAndUpdateViewTransform`
  (`31d01640`) from `324ba910 @324baa7c` / `328c41f0` (candidates: March's Field / BattleField `SetShowStateDLG( false )` view); the camera
  sequencers `3276c6f0` / `32d2e520` (name them; class-only ones go to checklist 7af). Brief: `kickoffs/june-engine/R13d.md`.
- **CommonStateStart's camera keys** (handed here by pilot-char-revamp CH1m, 2026-10-21). `RETAIN_CAMERA_STATE` (`32057b80 @3205b934`, a local bool)
  skips the camera reset at state start (`@3205ba89-3205bb11`: `+0x8020 = 0`, then, unless retained, `g_pX2Game` camera vslot `0x94` with `32420bc0` /
  `31fb3c50`); `FORCE_DYNAMIC_CAMERA_INDEX` (`@3205c0e8`) stores an int at user `+0x801c` (miss 0). March reads neither. Name `+0x801c`'s readers and
  port both keys with the camera they drive. June class scripts use them (Aisha: 15 + 1 states); inert until then, play is unaffected.
- **June's idle timer** (handed here by pilot-char-revamp CH1l, 2026-10-21). User floats `+0x91a8 / +0x91ac` (set @32041ae3 in `3203e850`,
  reset by `CX2GUUser::OnFrameMove` @3206f99d when user vslot 0x828 answers true), read by CX2Game `323022b0`, which shows text `0x4b1f` with the
  remaining time. All classes; the 0x828 base is `CX2GUUser::JuneIsNotCommonMoveState` (CH1l). Identify the screen it drives, then port whole.
- **June's skill-slot show writers March lacks** (handed here by pilot-char-revamp CH4d, 2026-10-22). UI manager `+0x89` is March's `m_bShowSkillSlot`
  (setter `32150b70` = `SetShowSkillSlot`, same guard); 8 of June's 16 writers are already March's. Missing: `CX2GUUser::Init` `3204ef16` (dungeon
  without a riding pet, DCC 8: `SetShowSkillSlot( true )` + `SetShowRidingPetSkillSlot( false )`, anchor `X2GUUser.cpp:1812-1819`); the PvP game ctor
  `32529e7c` (`true`, after `#endif FIX_OBSERVER_MODE`); the DELETE-key hide `31f036a0` (`31f03719` / `31f03742`, after `ProcessShowMiniMap( bHide )`
  in `CX2State::ProcessGameScoreAndPostEffect`); `CX2StateMenu` `324617c0`'s UI id `0x2c` branch (gate `31fd0380`). June-only, no March home:
  photozone states (`324a03ee`, `324a62b5`), the PVE state (`324adbd2`), the tutorial guide manager (`31fc5502`), and `SetShowUI` `32154820`'s guide-step
  gate and cash-shop early return. Reader that matters now: the class-guide gate `31d71810` (CH4d's `CX2Main::JuneCanShowClassGuide`, Aisha's memorize
  sparkle; Luciel's `322dd0f0` / `322d8100` / `322d8590` later). Port the four March-reachable writers together.
- **June's dungeon and battlefield shortcut handlers, whole** (handed here by pilot-char-revamp CH4d play-test fix 1, 2026-10-22). Only their missing
  `GA_SIT` block is ported (`SERV_IRUHADEV_JUNE_STATE_NO_SIT_KEY`: V no longer sits there, so Aisha's memorized skill gets it). The rest differs and is
  unported: `CX2StateDungeonGame::ShortCutKeyProcess` = June `324d2e20` (also the PVE / training states), `CX2StateBattleField::ShortCutKeyProcess` =
  `324bc8e0` (vslot 129 of `3349d88c`); their action reads are `0x11 0x14 0x15 0x17 0x1b 0x1a 0x2c 0xf 0x38 0x18 0x2a 0x2e 0x32 0x2b` (battlefield) and
  DIK `0x44 0xa 0x28 0xf 0x38 0x42` plus actions `0x11 0x14 0x15 ...` (dungeon). Diff each against March and port whole; check PvP / village sit too.
- **Transcendence in the June UI** *(from june-transcend T0, 2026-10-23; rows `UI Stage 9` in `june2017_re/transcend/t0_readers.tsv`)*.
  **Moved 2026-10-24 (user): the transcended class pictures (select screen, HUD portrait, character info, inventory, class name / icon) are june-transcend T6c, not this stage.** The rest of this bullet stays here.
  **T6c result (2026-10-24):** ported the select screen, character info, dungeon result, user list popup and other-player gauge. Left for this stage (rows `Transcend T6c` in `funcmap.tsv`): `321a9da0` is the June-only
  Jumping-system mail dialog, not the inventory (March has no counterpart); the `DLG_UI_Character_Icon` fills `325da870` / `325dacb0` / `325db6f0` (friend / field / guild / recommended lists: the flag comes from June's list
  entries, March's structs carry the class only); the class picker `32ed5180` (caller `32ed7210`); `X2StateArcadeResult.cpp:957` / `:968` (same shape as the dungeon result, no matching June function).
  T2 ports `CX2Data::GetCharacterImageName`'s fifth argument (June `31ee0de0`, bTranscend) for the skill tree. Its other 39 callers pass the
  unit's flag and are this stage's: the HUD portrait (`CX2MyGageUI` `32e5f3a0` / `32e62190` / `32e62d50` / `32e673e0` / `32e68d20` / `32e6cbb0`),
  character info (`3291df90`), class name / icon (`32ed5180`), the unit-info struct fill (`328b3350`), inventory (`321a9da0`), boss-raid score
  (`3215fd30`). Also: the GFx cash shop's transcend-item filter by `TranscendentalClassItem` type (`3222a840` / `3222af00` / `3222ec70` /
  `32233860`, `3290c490`; getter `32807fe0`, which T3 ports) and the world map's quest marks (`32260ba0`, the `QR_` condition at quest
  `+0xe0`).
  From T3 (2026-10-23): the job-ticket kind helper on the unit's inventory (`32869940`: 0x27d80 / 0x2783e / 0x2da9d.. -> 1, 0x2da9f /
  0x2daa0 / 0x5f5eeeb -> 2 or 0 by the flag), read by the inventory's ticket confirm (`@32190c09`, event 0x558) and `CX2CashShop::MouseLRButtonUpEquip`
  (`@3290d692`); the 1st-job ticket refusals (NetError 1737 / 1738) and the right-click cases 163200 / 163316 / 163323 / 163325 / 161854
  (class step 2 and transcended -> 1727; `321aa5d0 @321b8b1a` / `@321bc8bd`), which March has no case for.
- **The TC-slot class-guide effect** *(from june-transcend T4, 2026-10-23)*. June `322d8100` (called from the game's OnFrameMove `32308c20`) is not a
  cool-time loop: it visits only slots 4 and 9 and, while the slot's skill is not type 0xc / 0xd, the hyper-mode time is above a threshold and the
  unit has an unfinished buff BTI 820 (`3227b860( 0x334 )`), shows a guide particle on the slot (guide 4, `31d71810( 4 )` = `JuneCanShowClassGuide`,
  already ported; the particle store `32151190` / `321536e0` / `32151fa0` is not); otherwise it removes it. It breaks out when slot-state key 1 is
  unusable and (the slot is B or the unit is not transcended). March has no BTI 820 or guide-4 store, and the slot works without it; it follows T5's
  awakening buffs (BFI 843 / 844 / 845) and the HUD guide family of Stage 9.
- **Transcendence UI pieces found by june-transcend T5** *(2026-10-23)*. (1) The name mark `ICON_TR` (`31ff6920`) is ported and draws nothing
  until June's `DLG_Common_Emblem00.tet` (data063) replaces March's. June's copy has `ICON_TR` but lacks keys that March's dungeon map reads
  (`X2StateDungeonGame.cpp:5532-5623`: ARA, BACK_ROAD, BOSS_MARK and others), so that replace waits for Stage 15 P5's forecast and its owner.
  (2) `32157c10` is a UI object constructor (created by `31edeac0`) whose particle map (`32154f60`, key 4) plays
  `EffectSet_UI_Transcendence_Open` / `_Loop`. Identify it with the character-info / HUD family above.
  (3) June's `RenderRank` body (`31ff6aa0`, T5 review): a season emblem when `3241d290` == 2 (CX2Unit `+0xa0c`, `+0xa00` >= 5), otherwise the rank
  emblem only for ranks other than `PVPRANK_NONE` / `PVPRANK_RANK_ARRANGE`; March draws any rank. T5 ported only its bool return.

**Why UI is last and not first:** UI is not one system. Each stage above brings its own June dialogs: the region maps (1), slots and skill tree (3), quests (4), and the transcendence guides (6). A UI-first pass would port buttons with no system behind them, and then collide with every feature stage that touches the same dialog. The only UI work that must come early is the framework itself, and that is measured in Stage 2.

### Stage 10: emotion system (user, 2026-10-07)

Found by june-effect C9d (`JUNE2017_EFFECT_COMPLETION_PHASES.md`, "C9d - Done"; funcmap rows `Effect C9d`). March's `EMOTION_TYPE` ends at
`ET_EMOTION_AVATAR7 = 15`; June's runs to `ET_END = 65`. What June has and March lacks:
- **The catalogue:** `EMOTION_TYPE` 16-64 (AVATAR8-41, `ET_OTHER_WORK_SITREADY`, `ET_RIDING_PET_TRANSFORM`, `ET_EMOTION_CASH1-3_*`,
  `ET_TWO_SEATER_SUB_INDEX_1_SIT_*`), as both the C++ enum and the Lua `EMOTION_TYPE` table (March's Lua has a 4-entry stub).
- **The data:** `EmotionData.lua` (`AddEmotionData`, 20 entries), `CX2UnitManager::GetEmotionData` (`32375250`), `UI_EFFECT_SET` per
  unit (`31fac950`), and the data-driven `IsAvatarEmotion` (`3234cb00`). The hard-coded emotions (Keroro, Twice, Xmas box, Maid ...) sit
  outside the file, in `3234cb00`'s fallback list and the per-class sound switches.
- **Unlocks:** `CHANGE_EMOTION_CONDITION` (`CEC_SET_ITEM`, `CEC_ITEM`), checked against the inventory.
- **Cash sits:** the `EMOTION_CASH*_SIT*` states (emotion ids). The equipped-item sit effects C9d listed here (`3203e1b0`, the
  `+0x93dc`/`+0x93ec`/`+0x940c` vectors, the CX2Unit `+0xa44` map, `32651ab0`, square `+0x714`/`+0x724`, the viewer's group 1/2 block
  `31fa52b0 @31fa5645`) are June's customize system: moved to **Stage 12** by june-effect C9g (`32651ab0` is `CX2CustomizeManager`,
  not the item templet; `+0xa44` holds applied customizations). Stage 12 depends on this stage for the sit emotion ids.
- **June's rewritten `PlayEmotion`** in `CX2GUUser`, `CX2SquareUnit` and `CX2UnitViewerUI`, their EmotionData `UI_EFFECT_SET` paths
  (the second writers of `+0x9434` / `+0x7ac`), and June's two-seater (`0x2c`/`0x2d`) handling.
- **Assets and UI:** per-class emotion motions, sounds, the 5 June-only mesh players C9e lists, `DLG_ChatBox_Emotion_List.lua`.
- **The Cash25 emotion motion** (june-engine R12b): `31ee9150` appends `Motion_<Class>_Emotion_Cash25` to a motion list. The game unit's
  call is ported (`SERV_IRUHADEV_JUNE_RENDER_MOTION_CASH_EMOTION`, inert until june-asset-port adds the June-only files); the unit
  viewer's motion builder `31faa6f0` (two calls, 29 UI callers) is this stage's.

**Depends on:** Stage 2 (UI framework measurement) and june-effect C9e (the `32355a50` helper and the three effect-set handles,
which this stage widens). Lu/Ciel, Rose and Ain rows stay inert until Stage 8, as C9c-1's Rose/Ain rows do.

- **`EmotionFrameMove`'s riding-sit sound replay** (june-effect C9h-4, 2026-10-08). June's `3204a860` adds a new head that runs while an
  emotion plays (`+0x89d8`) and its anim has not ended. For emotions 2..3 while riding, it replays the state's sound records on the riding
  pet's clock (3D forced on, loop off, records not erased) and attaches their delete conditions (`3227a740`). Depends on june-effect
  C9h-4a (the record and the attach).

- **The dance states** (pilot-char-revamp CH1j, 2026-10-21). Every June class InitState registers `<prefix>DANCE_Z` / `_X` / `_WAIT` (June user
  ids 0x7f-0x81, funcmap row "June user state ids 0x7b..0x81"). Their callbacks are June CX2GUUser functions March lacks: thunks `329cc140` /
  `329cc1d0` / `329cc040` (EventProcess). March reserves no state id for them. Aisha's `June_InitStateCommon` leaves them out with a note. This
  stage gives them ids and their CX2GUUser callbacks, then adds the registration to each ported class. First find what enters them (emotion or item).

### Stage 11: division item templets (june-effect C9f, 2026-10-07)

Found by june-effect C9f (`JUNE2017_EFFECT_COMPLETION_PHASES.md`, "C9f - Done"; funcmap rows `Effect C9f`). June's
`CX2ItemManager::AddDivisionItemTemplet_LUA` (`32818480`) reads per-item rendering and behaviour lists. There are 112
`DIVISION_ITEM_TEMPLET_TYPE`s: alpha-blend, cull, cartoon and real-colour render rules, jiggle, accessory hiding, trigger and loop
animations, the new enchant stones, PC-bang and spa avatars, and more. March has none of this system. Planned under the standing rule
that an unowned June system gets its own stage. What it covers:
- **The data:** `RenderDivisionItemTemplet.lua` (113 blocks), `DivisionItemTemplet.lua` (24), `CoboRenderDivisionItemTemplet.lua`,
  and the `DIVISION_ITEM_TEMPLET_TYPE` / `READ_TYPE` Lua tables.
- **The dispatcher's other paths:**
  - types 55 and 85 (`DITT_CUSTOM_RENDER`, `DITT_TEST_RENDER_DIVISION_SETTING`) -> `328186a7`;
  - types 89-93 (`DITT_NEW_ENCHANT_ITEM_*`) -> `328186b5`;
  - the five other default-chain parsers: `32814170`, `32810700`, `328144f0`, `32817470`, `32815af0`.
- **The consumers:** every `IsDivisionItem` (`32807db0`) caller, e.g. the `DITT_LOOP_ANIM_ITEM_ID` (72) block in GUUser's equip loop
  (`@32065c9f`, `3295c960`). The renderer and equip consumers are not mapped yet; mapping them is this stage's first, read-only phase.

- **R14 (june-engine, 2026-10-18) consumers waiting on this stage:** `CX2Eqip::_UpdateSkin()` `3295ae20` attaches every model when `IsDivisionItem` group 6 / 0x31 / 0x36 / 7 holds the item (ported; the groups' parser `32814170` is this stage's) or the eqip's custom-render vector (+0x92c, item manager `32810450`, maps +0x240 / +0x250 = `DITT_CUSTOM_RENDER`) has 2+ entries (not ported); `InitSkin` `32963eb0` sets per-item frame vectors (item manager `32812f40`, map +0x370 -> frame +0x108 / +0xfc via `329656d0`) and the mesh item id (device manager map +0x260 `31cdcc70` -> mesh +0xa40) (not ported). Each is a comment at its site (`X2Eqip.cpp`, `SERV_IRUHADEV_JUNE_EQIP_SKIN`).
- **R14b (june-engine, 2026-10-18) consumers waiting on this stage:** `InitAttatchAnim` `3295dfa0` jiggles normal and hyper attach anims for `DITT_ITEM_ID_JIGGLE_TYPE1` (30) items (ported; June moved March's `ITEM_ID_JIGGLE_TYPE1` ids into `RenderDivisionItemTemplet.lua`, so those items do not jiggle until this stage fills group 30), and for `DITT_LOOP_ANIM_ITEM_ID` (72) items looks up the record (item manager `32807e00`, map +0x2c0) and changes every attach anim to its anim name (`3295a570`), setting eqip +0x15f0 = 1 (not ported, comment at its site, `SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM`).

**Already ported:** the CEPE slice (type 81), by june-effect C9f-2: the binding, the dispatcher shell, parser `32814da0`, the two maps and
the three play sites. This stage adds parsers and consumers to that shell; it does not re-bind.

**Depends on:** june-effect C9f-2, Stage 2 (render types) for the render rules, and Stage 13 (June `item.kim`, **done 2026-10-08**: the item ids its lists name now resolve) for the item ids its lists name. It can run alongside Stages 3-8.

### Stage 12: character customize system (june-effect C9g, 2026-10-07)

Found by june-effect C9g (`JUNE2017_EFFECT_COMPLETION_PHASES.md`, "C9g - Done"; funcmap rows `Effect C9g`). June's
`CX2CustomizeManager` (embedded in `CX2Main` at `+0x9e4`, Lua `g_pCustomizeManager`) applies cash "custom" items to a character:
lobby motions, sit motions, face and texture, skill cut-ins and viewer effect sets. March has none of it. Planned under the standing
rule that an unowned June system gets its own stage. What it covers:
- **The data:**
  - `CharCustomData.lua` + `CharCustomData_INT.lua`, parsed by `326525d0`: `AddCustomPeriodItem`, `AddFreeMotion`,
    `AddReplaceCustomID`, `AddFreeSkillCutIn`, and the motion / sit / face / texture / skill-cut-in blocks. There are 1,270 `ITEM_ID`s;
    1,066 are in June's `item.kim`, which the user plans to adopt.
  - Also `CharCustomPreviewData.lua` (`3264d790`), `CustomItemHide.lua`, and the `Cash1-6_*` motions in June's `Motion_*_Common.x`.
- **The unit side:** the applied-customize map on `CX2Unit` (`+0xa44`, keys 1 motion, 2 face/texture, 7/8/0xd/0xe sit) and how
  it is filled (insert site not found yet).
- **The viewer side:**
  - the members `+0xf48..+0x11d0`: six custom motion names, three sit-name arrays, the sit-key and per-motion effect-set maps;
  - `ApplyCustomize` `31fa89b0`, `ResetCustomize` `31fa4980`, and the motion-list load `31fa8230`;
  - the custom pre-pass of PlayByMotionType, `31fa5c90`, whose cases 0/3/6 play the custom motion and its group-1 set
    (`31f968a0`); its data-independent head is ported by june-effect C9g-2 (done 2026-10-08, with `AnimationNameTable.lua` and the
    renumbered enum; the viewer's HABIT idle timer `+0xf9c` / `+0xfa0`, `31fa8ce0 @31fa99f1`, is this stage's);
  - the custom sit sets, groups 1/2: `31fa4ec0`/`31fa4e10`, `31fa5140`/`31fa5180`, and PlayEmotion's block `@31fa5645`;
  - the HABIT helpers `31fa5050` / `323eeea0`.
- **The square side:** StateStart's custom WAIT set (`323fad80 @323fb795/@323fde30`, group 1) and the GUUser / square sit-effect
  vectors that C9d listed under Stage 10 (`3203e1b0`, `+0x93dc`/`+0x93ec`/`+0x940c`, square `+0x714`/`+0x724`).
- **The UI:** the customize preview dialogs. `3224b340` / `3224d0f0` play group 3; `32d77220` plays groups 3 and 4 (`3264f350`,
  `3264f080`). Groups 3 and 4 are used only here.
- **`GFX_SKILL_CUT`** (handed here by june-gfx Z, 2026-10-19). `CharCustomData`'s parser `326525d0` stores a `GFX_SKILL_CUT` key whose consumer june-gfx G0 / G4 did not find. June's user skill cut-in itself
  is a GFx widget set (`ActiveSkillShow` leaf `31fd95a0` -> `CX2Main::JunePlayCutInWidget`, ported by june-gfx G4); the customize skill cut-in (`AddFreeSkillCutIn`, item type 14) is this stage's data, and its cut-in name must be a June
  `.gfx` name (no `.dds`). Find the `GFX_SKILL_CUT` reader before porting the block.

- **The custom-sit / two-seater sync** (june-effect C3e): user-sync bits `0x2000000` / `0x4000000`. `320455e0` writes the customize
  manager's sit id (`32363280`, map key 7) and the pending emotion byte `+0x94b0`. `320458f0` plays them on other players
  (`PlayEmotion` `32043380`, the `0x2c` / `0x2d` two-seater). It is wire only, since offline has no other player.

- **The class cut-in item override** (found by pilot-char-revamp CH5, 2026-10-21). Each class's `ShowActiveSkillCutInAndLight` (Aisha: `32cc9680`) asks `31fd8a50` for the user's applied skill
  cut-in item (unit map `+0x8e58`; needs the equipped custom item and `32890620` / `32206450`), looks it up in the customize manager (`31d70b60`, `3264e800`), and replaces the cut-in with
  `SkillCutIn( name )` (`32078b70`, 512 x 512). Aisha's port keeps the class table and skips the override; port it here for every class at once (`June_ShowActiveSkillCutInAndLight` marks the site).
  Eve (char-eve CH1l40, 2026-10-26): her helper `3297b840 @3297b908` is the same block; `3264e800` = `CX2CustomizeManager::GetItemData( id, out )` (map `+0x10c`), the out struct is `321cdbb0` / `321cd590` (4 wstrings, a map, a vector) and the cut-in reads its `+0x2c` wstring. CH2a2 marks the site.

- **The awakening customization effect** (moved here from Stage 6 by its planning, 2026-10-22). Every class's awakening frame-move calls:
  - `32011d20`: it finds the item with entry `[0x10] == 1` under key 15 of the applied-customize map (CX2Unit `+0xa48`), checks it with `3264ec50`, and
    writes the set name `325d2370( 3 )` (+ "_Add" for unit type 9) into the GUUser wstring `+0x9578`;
  - `31fd66b0`: "the name is not empty";
  - `32011f50`: plays the set into `+0x9f4`, stopped by `32296cb0`.
  - Also `3204b5e0` (GUUser vslot 147, a switch on effect ids; identify it here).
  - Aisha's `June_CommonHyperModeFrameMove` (`32ce12c0`) carries file-local stand-ins (`June_AwakeningEffectBegin` / `_SetPresent` / `_Play`)
    with June's empty default; a class port keeps the same stand-ins until this stage.
  - Also here: `CharCustomData`'s `CHAR_CUSTOM_HYPER_EFFECT_DATA.BASE_HYPER_EFFECT_NOT_CUSTOM_IN_VIEWER`, whose per-unit-type `EFFECT_SET_NAME_TRANSCENDENTAL`
    (key read by `326525d0 @32658c12`) names the viewer's transcended awakening sets (Aisha: `Effectset_TRANScendence_WakeUp` / `_FX`).
  - Offline reach: none until a customize item is applied.
- **Item type 14** is June's `IT_CUSTOM` (1,207 customize items in June's `item.kim`, e.g. skill cut-ins). June-item-kim K2 guards
  it; this stage reads it (june-item-kim K0).

**Depends on:** june-effect C9g-2 (the PlayByMotionType shell and `AnimationNameTable.lua`), Stage 10 (the sit emotions 0x19-0x3b and
the EmotionData path the sit sets ride on), Stage 2 (UI framework) for the dialogs, and Stage 13 (the June `item.kim`, **done 2026-10-08**: its 1,066 customize items and 1,207 type-14 items now resolve). It can run
alongside Stages 3-8.

### Stage 13: June item.kim adoption (user, 2026-10-07)

The user decided during june-effect C9g to adopt June's `item.kim`. A planning session on 2026-10-07 found that the swap is not a
drop-in:
- June's records are 228 B and March's 188, inside the same `KIM ` / `0x1201` header.
- March's loader accepts June's file and misreads every record without any error.

The full plan is `JUNE_ITEM_KIM_PHASES.md` (STATUS block `june-item-kim`, kickoffs `kickoffs/june-item-kim/`):
- K0: June's record from the binary, plus `tools/kim.py`.
- K1: data and code census (save-breakers, out-of-range enums, assets).
- K2: the loader port behind `SERV_IRUHADEV_JUNE_ITEM_KIM`, a layout check, and the data swap.
- K3: emulator and save reconciliation.
- K4: play-test.

Every March id is in June, which adds 61,591 new ids. June's other item tables, and its `SetItemGroup` / `ItemLock` systems, are routed
by K0/K1, not by this stage.

**Depends on:** nothing. Stages 11 and 12 depend on it for real data.

### Stage 14: Byte-exact June asset extraction (user, 2026-10-07)

The user asked whether June's non-lua/kim assets load in March without changing them. An investigation the same day found that they
do:
- the formats match: DDS, TGA, `.x/.y` (KSM/KME v0x200, the same X templates), TET/XET v0x0100, Vorbis, and June's `XRenderer.fx`;
- the extraction does not: elstools cuts 18,571 V.0.4 Blowfish entries short (about 4,900 damaged), and `data101_kom` is corrupt.

The full plan is `JUNE_ASSET_EXTRACT_PHASES.md` (STATUS block `june-asset-extract`, kickoffs `kickoffs/june-asset-extract/`):
- X0: V.0.4 container RE.
- X1: `tools/junekom.py`.
- X2: re-extract.
- X3: `kom.py stage-june`, which enforces the algorithm, placement, duplicate and long-name rules.
- X4: `SERV_IRUHADEV_KOM_LONG_NAMES`.
- X5: trial pack and play-test.

X0 answers the deferred June loader plan's RE items 1-3. The loader port itself stays deferred.

**Depends on:** nothing.


### Stage 15: Every remaining June non-lua asset (user, 2026-10-08)

After Stage 14's extractor and its trial, the user asked for every June non-lua asset still missing or different in March,
byte-exact. The plan is `JUNE_ASSET_PORT_PHASES.md` (STATUS block `june-asset-port`, kickoffs `kickoffs/june-asset-port/`).
- **Scope:** 117,562 June-only files added, and 4,363 colliding files replaced by June's. There are no holds: March consumers that
  break are routed to their owners.
- **Also covered:** loose music, movie and eventscene files, and `XRenderer.fx` alone.
- **Out of scope:** `.txt` stays with june-effect, and `.lua` with the ports.
- **Added by char-eve CH0 (2026-10-26): the 4 `_INT.txt` definitions.** `UIMajorParticle_INT`, `GameMajorParticle_INT`, `UIMajorXMeshPlayer_INT`
  and `GameMajorXMeshPlayer_INT.txt` are still March's (datai015); June's are bigger, in data036. The other 75 particle / mesh-player
  definition files are June's bytes. june-effect is closed, so this stage takes them: stage the 4 byte-exact, then check the game and UI
  particles still load (every character and the UI share them; the l.304 dimming layer needs `GameMajorParticle_INT`). Evidence:
  `june2017_re/char_eve/ch0_effect_closure.tsv`.

**Depends on:** Stage 14 (done). Every later stage benefits, because its assets are already in place.

### Stage 16: June item-side systems (june-item-kim K0, 2026-10-07)

K0 mapped June's 228-byte item record (`JUNE_ITEM_KIM_PHASES.md` "Phase K0 - Done", `june_item_kim/k0_record_map.tsv`). K2 ports
the record itself and every field March already reads. The rest has no March owner, so it lands here, one phase per system:
- **Templet fields with no March reader.** `m_fDamageRange` (+44, 10,670 items), `m_ItemEquipmentGroup` (+52, defaults to the own
  id, 485 differ) and `m_eDescriptionInShopType` (+224, `DIST_*`, 195 items). June reads them inline; K0 did not locate the readers,
  so this phase's first step is finding them.
- **Item expiry.** `m_wstrExpiredDate` (+100, 277 items) is read by the item-period UI `3218aba0` and the description builder
  `32e102f0`. `m_wstrPreExpiredDate` is never read by June's client.
- **PvP equipment.** flags bit 7 = has `SAT_PVP_EQUIP` (40, 240 items), plus `SAT_PVP_EQUIP_ENCHANT_LEVEL_7..13` (44-50).
  Also June's buy-PvP rank 10 (`PVPRANK_RANK_STAR`, 52 items): March registers no emblem for it, so K2 guards the tooltip deref
  (`X2ItemSlotManager.cpp` `WriteStringForRestrictionPurchaseToDesc`; June has no NULL check there, `32e09910`). Porting June's
  rank-10 emblem (`PVPEmblem_Season2.lua`) makes the guard dead.
- **June's equip slots 26/27** (`EP_AC_MINIME`, `EP_AC_EARRING`; 475 / 305 items). K2 stands in with
  `SERV_IRUHADEV_JUNE_KIM_MINIME_REMAP` (26 -> 17, 27 -> 18, decision D2, 2026-10-08). This phase ports June's two slots (enum,
  `EP_END`, viewer/unit arrays, equip UI) and **removes that flag**.
- **New special abilities.** 42 `SAT_CURR_LEVEL_EXP_PERCENT_UP`, 43 `SAT_STEAL_AMUSEMENT_ITEM`, 51 `SAT_IMMEDIATELY_EXCHANGE_TO_ED`
  (52 `SAT_ELIANODE_PIECE_EQUIP_TYPE` has no item yet). 41 `SAT_ADD_DP_PERCENT_UP` goes to the character-port checklist (Add).
- **Set item groups.** `SetItemGroup.lua` (`g_pCX2SetItemManager:AddSetItemGroupData`: linked items and a replacement string per set).
- **Item lock.** `ItemManager.lua` (`SetItemLock_MaxDay`, `SetItemLock_EDPerDay`).
- **Coordi catalogue.** `CoordiItem.lua` (`g_pCoordi_ItemManager:AddCoordiItemTemplet`; June builds the file name XOR-encrypted).
- **Other June-only item tables** opened by `ResetItemManager` `31edda90`: `PackageItemData_INT`, `IceHeaterPreView_INT`,
  `ItemEtcAccessory` (voice packs), `CanUseInventoryItem` (right-click opens the cash shop), `GuildAgitShopItem`,
  `SynthesisMaterialTable`, `SynthesisTable_Client`, `PetUpgradeItemData`, `EventSealItemData`, `UnlockSocketItemData`,
  `UnlockSocketItem`.
- **Ice-heater preview rewrite** (june-effect C9h-2c, 2026-10-08): loader `3281a0d0` (`AddIceHeaterItemData`, rows into map
  `+0x1b0`) and lookup `328136f0` (date windows or the code-event set `g_pInstanceData+0xaa4` pick a second id, then two
  `std::set`s and a `rand()` pick). Same 0x4c row as the pet preview (C9h-2d); June's 9,545 IceHeaterPreView rows all pass 4 args.
- **June-only item socket vectors** (routed from skill-system-core S7m, 2026-10-19). `31ed7e90` (from `KInventoryItemInfo`; `32887100` from a packed record) fills the
  item's `+0x8c` / `+0x9c` / `+0xac` lists from three June-only `KItemInfo` socket-id vectors (`+0x48` / `+0x58` / `+0x68`); `+0xac` counts only when `32883300`'s apply condition
  is met by the set's equipped parts. They belong to June's socket extraction / apply system (`m_bSocketExtraction` / `m_iSocketApply` / `m_iSocketCondition`, `32883300` /
  `32883450`, item socket count `328836c0`); March's `KItemInfo` has no such fields and the emulator no source. Also here: the usable-expiry part `328823d0` and the equip-group
  set at unit `+0xe40`. Identify and port whole with the socket extraction system; the combat sums already ignore these lists (S7k / S7m).
- **ED limit by level** (routed from june-result R1 review, 2026-10-22). `LimitsTheMaxEDInfo.lua` (data036, `GameSysVal:AddEDLimitsByLevel( level, a, b )`) fills a
  per-level map on `g_pData +0x258`; `328b1880` returns the current level's int64 limit (-1 on a miss). Eight readers: item/ED code `3216d8c0`, `328461a0`, `32849da0`,
  `3286f7b0`, `3286feb0`, `32dd6020`, `32dec9f0`, and the dungeon result `324d60e0`, which caps the shown ED gain at the limit less the ED saved at dungeon start
  (state `+0x7e8`, writer `324ccc32`; R1 keeps it uncapped, June's no-table path). March has no ED limit at all; offline reach: every ED gain.

Routed elsewhere:
- the division and render-division tables go to Stage 11;
- `TranscendentalClassItem` goes to Stage 6;
- the Lu/Ciel minor-unit fields go to Stage 8.

**Depends on:** Stage 13 (K2).

---

### Stage 17: Pet and riding-pet remainder (june-effect C9h, 2026-10-07)

june-effect C9h mapped June's pet and riding-pet viewer changes (`JUNE2017_EFFECT_COMPLETION_PHASES.md` "C9h - Done"). The engine
half is planned there (C9h-2a/2b/3). Two pieces are not effect work and have no March owner, so they land here:
- **The pet variant byte.** June's `ResetPet` takes a third argument. With a non-zero variant at evolution step 3, it swaps the motion
  and texture to `...N.x` / `...N.xet`, or to the hard-coded `H.xet` (pet 39) / `2.xet` (pet 134, variants 1-2) from `326cc990`.
  - The pet-info viewer (`328f0aa0`) passes a June `KPetInfo` byte at `+0xca`, and the in-game pet (`326e5cf0`) uses the same suffix
    lookup.
  - Porting it means a shared-header field (`CommonPacket.h`, so the flag also goes in `ServerDefine.h`), Put/Get, an `X2OfflineDB`
    column, the emulator's pet handlers, and whatever June uses to set the byte (not identified yet).
  - Reach: pets 39 (`PUI_GRIM_REAPER_REBIRTH`) and 134 (`PUI_SHERIFF_NORM`) are March pets.
- **June's pet / riding-pet roster.** Eight pets carry the new viewer and reserved effect-set keys, and none is in March:
  - the pet: `PET_Santagirl`;
  - the riding pets: `RidingPet_Jet_Ski`, `RidingPet_Lord_Of_Inferno`, `RidingPet_Lord_Of_Inferno_2P`, `RidingPet_Raul_Puppet`,
    `RidingPet_Cat_Maru`, `RidingPet_Dog_Kkogi`, `RidingPet_Pporu_Rickshaw`.

  Each needs its enum, templet, script, motions and effect sets. 16 June preview items (`PetPreView.lua`) point at them, and all 16 are
  in June's `item.kim`.

- **June's transformer riding pets** (june-effect C9h-3, 2026-10-08). Bumblebee and Optimus Prime carry `TRANSFORM`,
  `ORIGINAL/TRANSFORM_SCALE_TIME`, `APPLY_RIDINGPET_ANIM_MOVE`, `CHECK_RIDINGPET_SPACIAR_BUFF` and `ONGROUND_CHANGE_STATE`.
  - The system is: a second state-id set (`+0x544..`, 22 getters `32d27750-32d279f0`), the toggle `32d281f0` (my unit `+0xe44`, pet info, manager
    `321f5f70`), the scale picker `32d284d0`, the village states `TRANSFORM(_REVERSE)_VILLAGE` (`32d3cf40`), the SPACIAR buff check `32d2f3d0`
    (BTI 532), and the user readers of `+0x5a1`.
  - C9h-3b parses the keys; the consumers land here, with the pets.
- **C9h-2b's pet-info riding block** (`328fce40 @328fd0d8`). It is gated on `IsEnableCode( 2155 )`, which the offline emulator never sends. Its byte is
  `+0xd1` of the unnamed June singleton `324943c0` (no vtable, 0xd4 bytes; not `CX2RidingPetManager`), and no writer has been found. Name both
  if the code event is ever emulated.
- **The roster is 42 riding pets, not 7.** June has 57 riding-pet scripts; March's 15 are all among them. The 7 named above carry the viewer and
  reserved keys; DarkChaser and Rakshasa carry the target-search keys (C9h-4).
- **The pet `PlayEffectSet` force-down float** (char-eve CH1o, 2026-10-27). June's pet overload `321e2340` (called by `321e2510`) takes the same
  trailing float as the game-unit one and stores it in EffectSetInstance `+0x14c`. CH1o ported the game-unit half
  (`SERV_IRUHADEV_JUNE_EFFECTSET_FORCE_DOWN_RATE`, `m_fJuneForceDownValueRate`); March's `PlayEffectSetByPet` has no parameter. It matters only
  if a pet caller passes a rate other than 1.

**Depends on:** june-effect C9h-2a and C9h-3 (the code they would exercise), Stage 13 (the items), Stage 14 (byte-exact assets).

- **June's `RidingPetSpecialAttackEventProcess`** (june-effect C9h-4, 2026-10-08).
  - `3202f460` rewrites March X2GUUser.cpp:38948: the transformed state-id sets (pet `+0x5a0`) and the target search `31fe2420`
    (`TARGET_RANGE` / `LIMIT_Z_DEPTH` / `DEGREE_RANGE`, stored by C9h-3b).
  - Every riding EventProcess calls it (10 callers). C9h-4c ports `RidingAttackZEventProcess` calling March's version meanwhile.
  - The `VALUE_POS` reader is `32005520`, via `320322e0`.
  - The data that reaches it is DarkChaser and Rakshasa.
- **The `APPLY_RIDINGPET_ANIM_MOVE` readers** (`+0x5a1`; user `32015b30`, square `323b2e80` / `323c2aa0` /
  `323eef70`). Bumblebee and Optimus_Prime set it, with the transformer pets above. *(june-effect C9h-5, 2026-10-08: `323bddf0`
  and `324f3260` were wrongly listed here. They are the field NPC's house templet `bOnlyClickable` (`+0x198` -> `+0x5a1`), which goes to
  Elysion E1. The four readers above were not re-checked against that offset coincidence.)*
- **The `CX2Pet` loader's `SOUND_PLAY_ALL_OPTION`** (`326ef840`, 8 fields, via the nation reader). Only June's `PET_Pette_HEDGEHOG_STEB_1..3`
  use it, and they are not in March. `CX2Pet` has its own `SoundPlayData` (X2Pet.h:136), so this is a separate change from C9h-4a's.
- **The riding templet's `+0x15c` byte and the user's `+0x93da` copy** (june-effect C9h-4d review, 2026-10-08). Riding-on `3204d120 @3204d232` copies
  `RidingPetTemplet +0x15c` (June-only, after `+0x158`, zeroed by the templet reset `@32204b5e`) into user `+0x93da`; riding-off `@3204d335` clears it
  right after `3228a3b0`. Readers: `31fe4030` (riding on and the byte set -> buff `0x20` handling via `3227baf0` / vslot `0x250`), `31fd5b80`
  (forces the show argument of the equipped-item loop `+0x8834` to false), `3204dfc8`, class code `329e54bf` / `329e555c` / `32a52dfa` /
  `32a52f0e` / `32bae235` / `32bae633` / `32bf0a43`. March has neither the templet field nor the member; the templet key was not decoded.

---

- **The riding pet's collision boxes** (june-effect C9h-4e, 2026-10-08). June's `INIT_SYSTEM` carries `ENABLE_USER_COLLISION_BOX` (a bool) and
  `ENABLE_RIDING_PET_COLLISION_BOX` (a bone-name list) in 5 of March's riding-pet scripts (GodBeast_Fire, the three Oberon 650s, Scorpion Type R; the other June pets were not counted).
  March has no reader (inert), and June's version of all 14 adopted pets dropped `CAN_PUSH_UNIT` / `CAN_PASS_UNIT` (Scorpion's `PASS_DASH` no longer passes through
  monsters). Identify the reader (Ghidra: the `INIT_SYSTEM` loader of `CX2RidingPet`) and port it with the pets.

- **June-only riding user states** (pilot-char-revamp CH1j, 2026-10-21). Every June class InitStateCommon registers `<prefix>RIDING_SPECIFIC`,
  `_ATTACK_X_READY`, `_ATTACK_X_READY_2` and `_ATTACK_X_END` (June ids 93-96). Their callbacks are CX2GUUser functions March lacks
  (`31fce380` / `31fce3b0` / `31fd40b0`, `31fce410` / `31fce3e0` / `31fd41a0`, `31fce4d0` / `31fce4a0` / `320343e0`, `31fce470` / `31fce440` /
  `32033fe0`). June also gives `<prefix>RIDING_ATTACK_X` a FrameMove, `32002f70`, and `<prefix>RIDING_DIE` the die states' StateEnd `3202a7d0`
  (SIEE 18 + clear-all, then `CommonStateEnd`), which March's `DoStateEndStart` copy runs only for DIE_FRONT / BACK (CH1j review). Aisha's registration leaves all of these out with a note. This stage
  ports the states with the riding code that enters them, then adds them to each ported class (`june2017_re/char_arme/ch1j_reg_by_class.tsv` has Aisha's rows).

### Stage 18: NPC friendship system (june-effect C9h-5, 2026-10-08)

june-effect C9h-5 diffed June's `CX2TFieldNpc::OnFrameMove` (`june2017_re/c9h5_fieldnpc_onframemove_diff.md`, blocks 19, 26, 30, 32 and 33). The village NPC calls into a June-only friendship system. March has none of it.
- **The manager:** singleton getter `32696fa0` (0x610 bytes), lookup `321cd500( houseID, &value, &flag )`, max test `325d1d70` (value >= `+0x490`), a gift-cursor flag at `+0x4a4`, plus `32679e10` and `326878c0`.
- **The NPC side:** the talk sequencer `325c9e70` (NPC `+0x5a4..+0x5ab`), the friendship mark on particle `+0x230` (`Npc_Talk.dds` / `Npc_FriendShip_Max.dds`), and the gift cursor (state 0x1a, likely) with talk `+0x588`.
- **The keys:** the house keys `Receive_Like/DisLike/Normal/Cash_Gift_Talk`, `Level_Up_Talk`, `Every_Day_Reward_Talk`, `After_Talk`, `Reject_Gift_Talk` and `Gift_UI_Talk` (HouseParsing `324f3260`; the C9h-5 Done note lists the addresses).
- **The gate:** everything sits behind `IsEnableCode( 2215 )` (`@325cec47`).
  - The offline emulator never sets a code event, so nothing here is reachable until code events are emulated.
  - Elysion E1 ports the gated call sites with June's gate; whichever of E1 and this stage lands second wires them.
- **Still to identify:** the manager's loader, data file and packets. Name them when this stage is planned.

**Depends on:** Elysion E1 (the call sites). Code event 2215 is switched on from the save DB (standing rule "Game events are switched on from the save DB").

### Stage 19: June event dungeons (june-effect C10d, 2026-10-08)

june-effect C10d found two June damage steps that exist only for event dungeons. March has neither dungeon. Funcmap rows `Effect C10d`.
- **`DI_EVENT_2017_TREASURE` (41470).**
  - `327c9560` creates a manager at `CX2Game +0xbbc` (ctor `32d5c830`) only in this dungeon. It holds a score and a unit list.
  - `CX2GUKiddayNPC` removes itself from the list in its dtor (`327555a0`). `327554e0` adds score.
  - User `DamageReact @3204968f-32049719`: a hit from a listed unit sets the damage to 10% of the user's max HP and gives 2 s of force/show invincibility.
  - The 41470 start hook `31d87420` (buff 7d review) is part of the same dungeon.
- **`DI_EVENT_ALCHEMY_LAB` (41350).** NPC `DamageReact @327b452f-327b47a4`.
  - The gate: dungeon `+0xc44 +8 == 4`, and the hitting effect is not `EVENT_SPECIAL_ITEM_EFFECT_WIND[_EDT]`.
  - At or below 20% HP the damage becomes 3e9 (and `327be3e0` fires).
  - Above 20% the NPC heals 25% of its max HP and gets its attack x1.5 (`SetNPCStat`).
- **Still to identify** when this stage is planned:
  - the list's writers;
  - the other `+0xbbc` readers (`322d0110`, `322f0372`, `323dc5xx`/`323ddxxx`, `327d5704`);
  - the Kidday/AprilFools/Note NPC classes;
  - the dungeons' June scripts and enums;
  - every other event dungeon June has and March does not.
- **The other event dungeons (june-effect C3y, 2026-10-11; rows in `june2017_re/c3y_unreached_dungeon_census.tsv`).** June registers
  these and March does not; every script is staged (June bytecode), and none has a March DungeonData row:
  `DI_EVENT_VALENTINE_DAY` 38400 (March's own script and enum), `DI_PVE_BATTLE_WORLD_ISLAND_OF_ERDA_NEW` 41130, `DI_2017_ERDA` 41131,
  `DI_EVENT_SCHOOL` 41260, `DI_EVENT_WALLY_LAND` 41280, `DI_EVENT_ALCHEMY_LAB` 41350, `DI_ADD_CENTER` 41400,
  `DI_EVENT_HALLOWEEN_ZOMBIE` 41410, `DI_2016_XMAS` 41430, `DI_EVENT_2017_VALENTINE` 41440, `DI_EVENT_HIGHSCHOOL` 41450,
  `DI_EVENT_2017_FOOLSDAY` 41460, `DI_EVENT_2017_TREASURE` 41470. Their worlds and unresolved NPCs are in the C3y TSVs.
  `DI_2017_ERDA` and `DI_ADD_CENTER` are on June's Special map (their lines in data217 `DLG_Map_Local_Special.lua` are commented out).
- **`DI_EVENT_TREASURE_HOUSE_2` (41380, atlas A4, 2026-10-17).** User `PhysicProcess` `32015b30` @32018c26-3201906c: when the player
  *can* pass units, June still runs the push loop (team / ally rules, plus NPCs with a `CAN_NOT_PASS_SPECIFIED_BONE` state, once per
  bone) only in this dungeon (vslot 0x1c4 == 0xa1a4), at stage index 2 (`+0xc44 +8 == 2`), behind `32530a80` / `31aadcb0`. A4 ported
  the bone push itself (`SERV_IRUHADEV_ELYSION_SCRIPT_A`, `CX2GUUser::ProcessCanPushUnit`'s bone form); this block calls it the same way.
- **The START-state code-event summon (pilot-char-revamp CH1k, 2026-10-21).** June's user START EventProcess `3201b390` calls `3200ac50`
  when code event 2143 is on (`g_pInstanceData` map `+0xa18`), the unit is host and the state is older than 1 s. `3200ac50` summons a list of
  NPCs (manager `32696fa0` `+0x260..+0x264`, `CreateNPC` `322e9af0`) in game state 0xb with a dungeon room set. `CX2GUUser::JuneStartEventProcess`
  keeps the gate and logs once that the summon is not ported. Offline never sends `EGS_UPDATE_CODE_EVENT_NOT`, so the event is switched
  on from the save DB (standing rule "Game events are switched on from the save DB"). Related: WIN StateStart `320004b0` plays the `Clear_C/L/E/A/R` particles in `DI_EVENT_2017_TREASURE` (ported, CH1k).

- **The `DI_EVENT_HIGHSCHOOL` (41450) result effect (june-result R3, 2026-10-22).** June's dungeon end handler `324db6b0 @324db8f3-324dbaf5`,
  on a win in 41450 only, walks a June-only UID -> score map in the `EGS_END_GAME_DUNGEON_RESULT_DATA_NOT` tail (R0 map row `0xbd`), compares
  my score (`327c9480` = my UID) with the other side's and plays `GameWinNew` / `GameLoseNew` / `GameDrawNew` (`322ceaa0` + `31cf90f0`, as
  the clear effect `327c96e0`). March's packet has no such map and March has no 41450 DungeonData row; R3 ported the rest of the handler.

**Depends on:** nothing in code. It is content, so plan it after Stage 1 has proven how a June dungeon is brought over.

### Stage 20: damage-number customization (june-effect C3e, 2026-10-09)

Found by june-effect C3e (funcmap row `31fe0f60 / 31fe9ff0 / 31fcfb10 (C3e)`). What June has and March lacks:
- **The pick:** `31fe0f60`, run for my unit, maps owned items to two indexes 0..3:
  - 164384 / 164385 / 165089 / 294875 (Damage Custom A / B, Christmas, Nature) -> `CX2Unit +0xe88`;
  - 164405 / 164406 / 165099 / 294875 (the Critical set) -> `+0xe8c`.
  Both are mirrored at unit `+0x94b8` / `+0x94bc`.
- **The sync:** user-sync bits `0x8000000` / `0x10000000` (`31fe9ff0` writes Future, `31fcfb10` copies Now for other players). It is
  wire only offline.
- **Still to identify:**
  - the readers: the `CX2Game` vslot `322cfd70` returns `+0x94bc` by UID, and the damage-number renderer that picks the font;
  - the font assets;
  - how the items are owned (inventory lookup `32869c80`).

**Depends on:** Stage 13 (the item ids, done). It touches no other stage's files.

### Stage 21: Photozone (june-effect C3e-2, 2026-10-09)

Found by june-effect C3e-2 (funcmap rows `31ec2700 @31ec6187-31ec66db (C3e-2)`, `31d6fd60`). What June has and March lacks:
- **The states:** `XS_PHOTOZONE_LOBBY` 0x19 and `XS_PHOTOZONE_GAME` 0x1a (March's `X2_STATE` ends at `XS_WEDDING_GAME`).
- **The chat commands** (chat box handler `31ec2700`, only while `g_pMain +0x8c == 0x1a` and my unit exists):
  - the typed text equal to string `0xabeb` (44011) picks `rand() % size` of the WIN list, calls `JuneSetCommonRandomStateIndex( USI_WIN, index, true )`
    and, unless riding, `StateChange( USI_WIN )`;
  - string `0xabec` (44012) does the same for LOSE.
- **What is already in March** (june-effect C3e-2): the roll, the re-bind on StateChange / DoStateEndStart, N in the user sync, and the setter
  `CX2GUUser::JuneSetCommonRandomStateIndex`, uncalled. The LOSE re-bind sets `OnEventProcess` = `JuneRandomLoseEventProcess` while the by-chat flag is set.
- **Still to identify:** the Photozone's lobby, scene, UI and entry path; the two command strings in June's string table.

**Depends on:** C9h-4g for June's `*_WAIT_STATE_CHANGE` state (the delegate's target, state `0x82`). It touches no other stage's files.

### Stage 22: June dungeons and worlds (june-dungeons, split out of june-effect 2026-10-11)

june-effect's C3x / C3y / C3z phases kept finding dungeon and world content that is not effect-engine work, so on 2026-10-11 (user) the open
ones became their own plan: `JUNE_DUNGEONS_PHASES.md`, kickoffs `kickoffs/june-dungeons/`, STATUS block `june-dungeons`, same phase IDs.
- **Content:** Hamel secret (C3y-1a, done) and Sander secret (C3y-1b worlds, C3y-1c dungeon), the 5 Henir Space boss worlds (C3y-1b), the 9 class
  tutorial dungeons (C3y-2, identify the entry route first), Bestera 1-4 (C3y-3), Space of Thought (C3y-4), the Hero tier (C3y-5, identify
  June's route first; 191 ids), June's data101 map assets (C3z-1b, waits for Stage 14/15's data101 restore).
- **Systems that serve them:** the DungeonData readers C3z-5 routed (C3z-5b) and the Battle Field tutorial guide manager (C3z-4t, scope first).
- **Overlap with other stages:** Atlas / Elysion own their dungeons (A8-A10, E8-E11) and reuse the 4 Henir boss worlds C3y-1b ports; Stage 19 owns the
  event dungeons. All three edit the core registries (data036 `Enum.lua` + `DungeonEnum.lua`, data035 `DungeonData.lua`, data040 `WorldData.lua`,
  data037 `NPCTemplet.lua`): one port half at a time.

- **June's village world Lua (W3)**, moved here from june-engine by its scope audit (2026-10-20): every village world script and line map in
  data040 replaced by June's (ljconv), with its asset closure. Kickoff `kickoffs/june-dungeons/W3.md`.

- **The per-dungeon attribute set** *(from pilot-char-revamp CH1i review, 2026-10-21)*: June's `CX2DungeonManager` (g_pData `+0x44`) keeps a
  15-bit attribute set per dungeon. The reader is `31edf8f0( dungeon id, bit )` -> `327d9df0` (map find, data `+0x10`, `327e6d20( bit )`), and
  its loader is not mapped yet. Bit 13 makes `3202d3c0` (every class's skill lookup) refuse the common skill 900 `SI_ETC_WS_COMMON_LOVE` in that
  dungeon. March has no counterpart; Aisha's port reads the bit as false. Related: `3202d140` refuses skill 900 in June's PVE game type 4,
  which March lacks. Scope it with the DungeonData readers (C3z-5b).
  *June-result R0 (2026-10-22):* bit 14 (`31edf8f0( dungeon, 0xe )`) also hides the dungeon result's retry button (`324d1120`).
- **June event 0x3e7, a post-clear NPC spawn** *(from june-result R0, 2026-10-22)*: the dungeon state's handler `324d1290` (dispatcher
  `324dbc20` case 0x3e7) reads a position vector, sets state `+0x7a9`, makes the host create NPC 0x55c there (`322e67c0`), and plays a
  random line from NPC 0x959c by unit type. `+0x7a9` skips the result's 0.1 s wait and hides its exit button and retry. There is no
  March counterpart, and offline it is unreached (no emulator sends it). Related: `funcmap` row `324d1290 (Result R0)`.

**Depends on:** june-effect C3x-C3z-6 (done). Stage 14/15 for C3z-1b only.


### Stage 23: offline field rewards stand-in (atlas A6, 2026-10-17)

Found by atlas A6 (Done note "Drops"): every F01 / F02 kill logs `GAME -> NOTE monster N has no reward row for key 4003x ... grants nothing`
(`Handlers_Room.cpp:3312`). The emulator pays EXP / ED / items only from `DropTable.lua` (`AddBattleFieldNpcDropInfo`, keyed `BattleFieldID`,
wildcard key 0) and `StaticDropTable.lua` (`AddBattleFieldStaticDropInfo`), read by `X2OfflineDropTable.cpp:415` / `:529`. Both server files
(`KncWX2Server/ServerResource/US/`) carry battlefield rows 40000-40025 only. Lanox (40029-40032) and the Elysion fields (40035-40038) have none.
- **Scope:** design and implement a stand-in for the missing field rows, both regions, behind its own `SERV_IRUHADEV_` flag, labelled in the log.
  Candidate sources to weigh: June client data that names drops (item templets, `DROP = TRUE` per spawn), A2's `NpcExpTable` 81-99, the
  pattern of the last real region (Sander 40022-40025 rows, by level), and the Lanox / Elysion dungeons' own rows if the server files have them.
- **Dungeons are in scope too (atlas A8 review, 2026-10-17):** they have no rows either. `DropTable.lua`'s `AddMonsterDropInfo` stops at
  `DungeonID = 33252`, so Lanox 33300 / 33310 and AtlasDevelopment 33330-33332 kills pay 0 EXP / ED / items (and the 30 % clear bonus is 0).
  A9-A10 and E8-E11 add more such dungeons. The stand-in covers `AddMonsterDropInfo` (keyed `DungeonID` + `MonsterID`) as well as the fields.
- **Rules:** never invent silently; the user picks the design before code (memory `design-missing-june-pieces`); server Lua is the user's to pack
  if the stand-in lives in `DropTable.lua` / `StaticDropTable.lua` (CLAUDE.md server-Lua rule).

**Depends on:** atlas A6 (done). Touches `X2Lib/Offline/` and server Lua only; runs alongside any stage.

### Stage 24: June spa villages (atlas E1d, 2026-10-17)

Found by atlas E1d (Done note): June has two spa villages, `VMI_SPA_RUBEN` 1500 (no March enum) and `VMI_SPA_RANOX` 1501, and March has neither
village row. The Ranox door is the entrance: Ranox's `START_POS` 146 (`IsSpaEnter`, `OtherPlaceID = VMI_SPA_RANOX`, line 1). On today's data,
walking up to the door does nothing (user, E1d play-test: "just the door").
- **Already landed (E1d):** `VillageParsing` reads `IsSpaEnter` / `IsSpaOut` / `SPA_INFO` (type + `LineIdx`, templet +0x70). June's link filter
  `324f2ff0` keeps spa rows out of the portal list. There is no portal camera inside a spa.
- **Scope:**
  - June's `Village[VMI_SPA_RUBEN]` / `[VMI_SPA_RANOX]` rows (June `DLG_Map_Village` :4424 / :4502; core-list file) and the spa worlds / assets.
  - June's entrance / exit lists `324f2ca0` (TFieldGame +0x324) and `324f2d30` (+0x334). The TFieldGame ctor builds them only when
    code event 31 is on (@323bb5c9-323bb5ee). Port their consumers too: whatever reads +0x324 / +0x334 and moves the player.
  - `SPA_INFO`'s `SPA_BUFF_TYPE` buffs by line.
  - The emulator's state change into 1500 / 1501.
  - Spa avatars are the cosmetic stage's (line ~382).
- **Rules:** June wins. Find June's consumers in Ghidra before planning phases. If code event 31 is off offline, decide with June's own behaviour
  where the event is absent (memory `design-missing-june-pieces`).

**Depends on:** atlas E1d (done). Touches `X2TFieldGame.*`, `X2LocationManager.*`, `DLG_Map_Village.lua`, `X2Lib/Offline/`; not alongside
another session in `X2TFieldGame.*`.

### Stage 25: June fonts and text rendering (user, 2026-10-17)

The user's report: March's UI text looks like it has no anti-aliasing, and they want June's fonts. The "no anti-aliasing" part is
their impression, not a measurement, so the stage proves the cause before it changes anything.

What March does today (read 2026-10-17):
- **One FreeType face.** `USE_FREE_TYPE` is on (`KTDXLIB/Always.h:504`, `FT_DEFAULT_FONTNAME "GULIM"`). `CX2Main` installs
  `Gulim.ttc` only (`X2Lib/X2Main.cpp:1538`; `2002LKOG.ttf` is commented out at `:1539`). `InstallFont` is also Lua-bound
  (`X2Main.cpp:3361`, `KTDXApp.cpp:2063`), so a config script may install more: check before assuming.
- **Rasterizing.** `KTDGFreeTypeFont.cpp:397-438` loads glyphs with `FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP` and renders
  `FT_RENDER_MODE_NORMAL` (8-bit grey, so anti-aliased in principle). Whatever looks jagged may come later: the glyph-to-texture
  copy, alpha thresholding, texture filtering or point sampling, the hinting, or a GDI path (`KTDGDeviceFont*.cpp` picks
  `ANTIALIASED_QUALITY` / `CLEARTYPE_QUALITY` per case) rather than FreeType.
- **Font files.** March `datai022` holds `gulim.ttc`, `tahoma.ttf`, `blambotprol.ttf`. June `data196` holds 26 (`junekom.py list
  data196`), among them `2002LKOG.ttf`, `NanumBarunGothicBold.ttf`, `H2HDRM.TTF`, `tahoma.ttf` + `tahomabd.ttf`, `gulim.ttc`, the
  KozGoPro / meiryo / msgothic / CJK set and `blambotprol.ttf`.

Scope:
- **Identify in Ghidra** June's `InstallFont` calls and font-manager setup for the US build: which faces, which default name,
  the sub-face / Korean-face choice, the glyph load / render flags, and the glyph-texture upload and sampler state. Compare with
  March's same functions ([[no-partial-june-updates]]: a changed function is ported whole).
- **Adopt June's font files** at the same names ([[june-asset-wins-over-march-at-same-filename]]), byte-exact through
  `tools/junekom.py` ([[june-assets-byte-exact-no-edits]]), into the kom June's code reads them from, plus the June-only faces it installs.
- **Port June's font code** behind its own `SERV_IRUHADEV_` flag, with March's in `#else`.
- **Before/after screenshots** from the user decide whether the anti-aliasing complaint is fixed; if June renders the same way, say so.

**Depends on:** nothing in code. Stage 14's extractor (`tools/junekom.py`) for the files. Planning is read-only and can start now.

### Stage 26: June AVI movie system (added 2026-10-19, june-gfx G5)

June plays AVI movies for dungeon clears and quests. March has only `PLAY_PROMOTION_MOVIE` and the server-select intro flag.

**quest-system Q8 built the core (2026-10-19, flag `SERV_IRUHADEV_JUNE_QUEST_MOVIE`):**
- the table (`DungeonClearMovieTemplet.lua`, parser `31d8cb90`) and the play state on `CX2Main` (`+0x7f8..+0x84c`);
- start / stop (`31d880b0` / `31d883a0`), the end check in `OnFrameMove`, and the skip box (CX2State 3032 / 3033);
- the quest-driven movie (22236 / 22336, `31d7a240`), the cut-in term (`31fd324c`) and the dungeon-ctor clear (`324dadc2`).

The quest window's Video tab is quest-system Q8b.

What is left here (brief: `kickoffs/june-backlog/stage26-plan.md`, funcmap rows `(Quest Q8)`):
- **Dungeon movies:**
  - first clear: `31d7a150` from END_GAME `324db6b0`, = March `X2StateDungeonGame.cpp:3179`;
  - the field and battle-field starts `32507840` / `324bb780`;
  - the intro movies (Elder Secret, Henir) on the party manager's dungeon start `32434710`, resumed by `32438fa0`.
- **The other readers of `+0x80a`:** CX2Game::KeyProcess's head (`322f7200`), the game render's state string (`3230ada5`), the field and battle-field titles
  (`323b7da1` / `32d54ff3`, opened only with no movie), and CX2StateField::OnFrameMove (`32508736`).
- **`31d87560`'s bodies:** battle-field event 0x3af (`324bafc0`) and the user's busy emotion 0x11 (`32044a80`).
- **Stops:** `31f06e60` and `32db33e0`.

Depends on: quest-system Q8 review. Planning: `/x2-phase new-plan june-movies`.

### Stage 27: June Lua event scenes (added 2026-10-19, quest-system Q8)

June's `EventScene.lua` / `EventSceneObject.lua` (data036) parse with March's two binders. June's parser also reads `SCENE_OBJECT_DUO_TYPE` (`325b1f40`,
`325b0020`; 113 uses) and `OBJECT_TEXT_TAG_NAME_DUO_TYPE` (`325b0c70`; 17 uses), which March ignores. So the files are adopted together with the DUO keys,
never alone. No quest plays a Lua scene (Q0b, re-proved by Q5b), so this is not the quest plan's. Brief: `kickoffs/june-backlog/stage27-plan.md`.
Planning: `/x2-phase new-plan june-lua-scenes`. Not started.

### Stage 28: June party-manager handlers (added 2026-10-19, quest-system Q10b)

June's `CX2PartyManager` packet handler (`3243ee30`, vtable `33496b80` slot 1) changed handlers March has: the ready-check
(`3243d1d0`, March `PARTY_GAME_START_OPINION_CHECK_NOT`) gained a June-only branch on a packet byte (`3243cec0`), and the auto-party
match success (`3243d4e0`) a June-only tail after its popup (`31d70b00`, `31fc6010`, manager `+0x8a`, `32461a70` / `324639a0`). Q10b
ported only their story-slide-show forced Ends. Offline the AI party sends neither packet, so the handlers are undriven. Not the quest
plan's subject (scope rule). Brief: `kickoffs/june-backlog/stage28-plan.md`. Planning: `/x2-phase new-plan june-party`. Not started.
*June-result R0 (2026-10-22):* the party manager also calls the dungeon state's retry refresh (vslot 0xe8, `@32434dd6`). The result's
`RESULT_SUCCESS_ACK` handler (`324d4cd0`) has a `+0x22d` branch into `324334d0( 0x72 )`, which offline is never taken (always 0).

### Stage 29: June dungeon result screen (user, 2026-10-21; split out of Stage 9)

March leaves the dungeon state on a clear (`StateChangeResultReq` -> `XS_DUNGEON_RESULT`, KTDGUI `CX2StateDungeonResult`). June keeps
the player in the dungeon state:
- the end handler `324db6b0` sets result-UI mode 1;
- the mode machine `324cf3d0` opens `dungeon_result.gfx` as GFx UI 8 through `CX2UIDungeonResultAdapter` (`32eea7e0`), which has a
  retry button (ACK `0x649`);
- ESC is eaten while the result shows, and June's exit box replaces March's.

June's KTDGUI result state still exists (`32515670`). The plan's R0 decides whether a route offline reaches it. The plan is
`JUNE_DUNGEON_RESULT_PHASES.md`, key `june-result`, STATUS block `june-result`, start `/x2-phase june-result R0`.

**Fence:** the dungeon result only, from the end packet to leaving or retrying, plus the emulator answers it needs. The PvE / PvP
results, cash shop, option, bingo, the profession close and hide mode 5 stay in Stage 9. Movies stay in Stage 26, the party handlers
in Stage 28, quest popups with quest-system.

*R0 done (2026-10-22):* the map is `june2017_re/result_r0_map.tsv`. The 0xbd handler is `END_GAME_DUNGEON_RESULT_DATA_NOT`. R4 is
opened for the training result only. The drift went to Stages 6, 9, 22 and 28.
*June-transcend T0 (2026-10-23):* the result state's class image (`324ce260`, `31ee0de0` type 1) passes the transcendence flag; it is this
stage's when the result screen draws the class icon.

### Stage 30: June resonance system (june-transcend planning, 2026-10-22)

Found by june-result R0 (`resonanceLevel` in the dungeon result) and identified by the Stage 6 planning (funcmap rows `Transcend S6`).

**Unit data.** June's `KUnitInfo` ends with five June-only fields. `SetKUnitInfo` `3235eb20` copies packet dwords `0xc9..0xcd` to:
- CX2Unit `+0xa99` (int);
- `+0xa9d` / `+0xaa1` (an int64 EXP);
- `+0xaa5` (int);
- the byte `+0xaa9`.

**Setter and gate.**
- The setter `3234b3e0` is called from three packet handlers (`3260c140`, `3260c5d0`, `3260ca30`).
- The gate `3247b460` is unit level > 98 and byte `+0xaa9`.
- `322f8920` adds EXP to the int64 (callers `3230b7b0`, `32d53530`).

**Readers.**
- the dungeon result `324d60e0`;
- `324b0f80`, `32529f90`, `32473810`, `32476050`, `3260a360`;
- quest `3232e510`;
- UI `328b3810`, `3291df90`, `31d8e280`, `32468c70`, `32356640`.

**Offline reach:** none. March's `KUnitInfo` has no fields, so every reader takes June's off branch; level-99 units exist in the save (48-54).

**Depends on:** nothing for planning; code after Stage 9 by the priority rule, unless it blocks play. Brief: `kickoffs/june-backlog/stage30-plan.md`
(`/x2-phase new-plan june-resonance`).

### Stage 31: June achievement system (june-transcend planning, 2026-10-22)

Found by the Stage 6 planning: June's `AchievementConditionData.lua` counts transcended characters (`ACT_TRANCENDENTAL_CLASS_COUNT` 14,
ids 70-72). March has no achievement system at all.
- **June's pieces:**
  - `CX2AchievementManager` (RTTI `33735a64`; binders `AddAchievementData_LUA` / `AddAchievementCondition_LUA` registered in `31d95080`);
  - parsers `325fa910` / `325fdd90` / `32600540`;
  - `AchievementData.lua` / `AchievementConditionData.lua` (data036);
  - `DLG_Achievement_System_UI.lua` / `DLG_Achievement_System_Piece_Detail_Info.lua`, `ACHIEVEMENT_SCORE_EMBLEM`;
  - `ERR_ACHIEVEMENT_00-03`, admin handlers.
- **Offline reach:** none.
- **Depends on:** nothing for planning; code after Stage 9. Brief: `kickoffs/june-backlog/stage31-plan.md` (`/x2-phase new-plan june-achievement`).

### Stage 32: June bottom bar (user, 2026-10-24; split out of Stage 9)

The user asked for June's bottom area now: the menu button row, the EXP bar and the stamina bar. Plan `JUNE_BOTTOMBAR_PHASES.md` (`june-bottombar`, B0-B4, Z;
evidence `june2017_re/bottombar_buttons.tsv`).
- **In the plan:** June's `DLG_Menu_Button_NEW.lua` + `DLG_UI_Event_Button_INT.lua` and 13 textures (byte-exact), the 5 ids March lacks (210-214),
  the row layout and badges (`3245bb70`, `3245cba0`, `32472a40`, `32468c70`), the EXP bar (`32473810`) and stamina bar (`32475480`), and June's case body
  for each working button.
- **Stays Stage 9 (drawn, click logs `STUB`; user, 2026-10-24):** profession system, story book UI, rest EXP and its gauge, the 2016 Valentine UI; B0 adds
  any of advertisement / friend-recommend that needs a missing system.
- **Depends on:** nothing. Start: `/x2-phase june-bottombar B0` (Opus 5.5 high).
- **CLOSED 2026-10-24 (plan `june-bottombar`, B0-B4 + review + Z; MODS.md "June bottom bar: Stage 32").** Flags `SERV_IRUHADEV_JUNE_BOTTOMBAR_IDS` / `_LAYOUT` / `_GAUGES` / `_BUTTONS`. Every June button is drawn;
  the Stage 9 bullets above hold the final stub list (`june2017_re/bottombar_buttons.tsv`) and the drift found on the way (spirit model, June guild UI, Surprise Sale, June-only refusal state).

### Stage 33: June damage and character stat calculation (user, 2026-10-27)

**Why.** An Aisha play-test showed MP jumping to full on every hit. Buff Phase 16 (`SERV_IRUHADEV_JUNE_SOCKET_PARITY`) changed two things:
- it seeds the raw socket sum `m_fMPIncByAttack` / `m_fMPIncByAttacked` as the option value of MP members 24/25/26;
- it moved their stat refresh `32290950` to June's option-data curve.

The three per-hit refreshes in `CX2GameUnit::SetDamageExceptionProcess` and `CX2GUUser::DamageReact` stayed March's plain update. That update adds the option value raw, so every ranged hit gave +6722 MP. June uses the curve there as well (`322b4130` / `322b4170` / `320870a0`).
- The fix is `SERV_IRUHADEV_JUNE_SOCKET_HIT_MP_CURVE`.
- The user's conclusion: stat and damage math must not be a March / June merge. Each formula is replaced with June's whole calculation.

**In scope.** Every function that computes a number the player sees or feels:
- **Character stat sheet:**
  - base stats by level, equipment (June `item.kim`, K2), set items;
  - sockets and options (`SocketData`, the `SocketOptionForm` curves, the option-data template `320839e0`);
  - titles, pets, skill passives;
  - buff stat members (seeds `32270dd0`, basics `32271540`, passive `322718e0`, refresh `32290950`);
  - `SetGameStat` `322814a0` (June's seven-field `KStat`), `ResetMaxHP` / `ResetMaxMP` (`31fd2810`);
  - the numbers the character-info UI prints.
- **Damage:**
  - `GetFinalDamage` `32284aa0` (variance `322788c0`, piercing, `32275ba0`, the critical threshold `32273480`);
  - `SetDamageExceptionProcess` `322990e0` and the per-class overrides;
  - user / NPC `DamageReact` (`320462a0` / `327b1f30`);
  - defence, resist and enchant (buff Phase 15), PvE / PvP / boss rates.
- **Resource gain:**
  - MP on hit and on hit taken, the awakening-gauge gain, HP / MP regen;
  - MP cost (`GetActualMPConsume`).

**Method.**
- A map phase first: one row per formula function and per stat member. Each row gives its June address, its March counterpart, its port state (**June / March / mixed**) and the flag that last touched it.
- Mixed rows are the work; June rows are inherited and spot-checked, not redone. Already June: the buff plan's members, socket parity, effect C3 damage data, ExtraNPCStat, Stage 3 skill values, transcendence.
- Each formula is ported as June's whole body, with March's code in `#else`, as [[no-partial-june-updates]] asks.
- Verification:
  - a stat-sheet and per-hit breakdown log under a `_DEBUG` flag (`SERV_IRUHADEV_MP_JUMP_DEBUG` is the model);
  - pure formula functions run under Unicorn on June's binary against the port's outputs for the same inputs (precedent: buff Phase 16 review, `320839e0` on 360 input sets).

**Depends on:** nothing for planning (read-only).
**Code priority (user, 2026-10-27):** after the character ports (Stages 7 and 8), so the damage of every character is tested on June's formulas at once. A mixed formula that breaks play before then (as the MP bug did) is fixed at once under *Scope*, in its own flag, and the map marks it June.
**Brief:** `kickoffs/june-backlog/stage33-plan.md` (`/x2-phase new-plan june-stat-calc`).

## Open decisions that gate stages

| Decision | Gates | How to settle |
|---|---|---|
| **Starter items for classes the live DB never had.** Live `gup_create_unit` copies exactly ten `dbo.GBase_Item` rows per class and aborts with `-28` otherwise. Rose, Ain, Lu/Ciel, and possibly transcended classes, are almost certainly absent. The no-band-aid rule forbids inventing rows unless the user authorises it, as they did for A2's stat rows. | 8 (not 6: the Stage 6 planning found that transcendence adds no class IDs, 2026-10-22) | **Answered 2026-10-27 (user): the live DB has no starter rows for Lu/Ciel, Rose or Ain.** No pull is needed. Each new character's planning session (`new-plan char-<name>`) derives the ten rows from `item.kim` (`tools/kim.py`, `/x2-kim`) by the name pattern of the existing classes' starter items (the `131641..`, `111094..`, `112700..` blocks). The result is a labelled stand-in under that character's own `SERV_IRUHADEV_` flag; the planning session records the pattern and the ids, and lists ambiguous picks for the user. Never an id that is not in `item.kim`. |
| **Stat rows for June-only class IDs.** The user authorised inventing `StatTable`/`NpcExpTable` rows 81–99 only for classes that already exist (Atlas A2). The later class paths and the new characters need a **full** per-level row set, and `KncWX2Server/ServerResource/US/` has none for them. | 5, 7, 8 | First look for June-side data: the June binary, any June server Lua, or the live DB. If there is none, ask the user whether A2's permission extends to these classes, and how the rows should be derived. |
| **Guild quests offline** | 4 | Ask: parse-and-log or a real port. **Answered 2026-10-17: parse-and-log.** |
| **Scaleform/GFx runtime** for EventScene and quest movies | 4 | Ask: a runtime, or skip with a log line. **Answered 2026-10-17: a runtime, plan `june-gfx` (`JUNE_GFX_RUNTIME_PHASES.md`); its G0 assigns the owed gates below, G5 ports the GFx ones, the rest go to Stage 9.** Whichever lands also owes June's two cut-in hide terms in `GetShowActiveSkillShow` (`31fd3180 @31fd3228`, left out by effect C3t-1): GFx UI `0xb` open (`[g_pKTDXApp+0x1c8]`, `isOpenUI` `31c44710`), and a movie playing (`g_pMain +0x80a`, June's movie table `+0x7f8`; funcmap `(C3t-1)`). Also owes June's ESC gates from effect C3p-2 (`31f1e1a0`, funcmap `(C3p-2)`): the result-UI mode `+0x210` switch (`@31f1e214`, with its setter `31ef7be0` and exit box `31f1dd70`), `closeToTopZorder` + the `CX2UIProfessionManager` close (`@31f1e262`), and the dialog manager's hide modes 2-4 (`31d03a10` / `31d05af0`). Insertion point: the C3p-2 Done note (`python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3p-2 - Done (2026-10-15, Opus 5.5 high; every gate mapped, nothing portable, no code, no build)"`); the C3p-2 review adds that June's end-of-game handlers (`324db6b0`, `3252ba10`) open the mode-1 window when result info exists, so ESC is eaten from the end packet until the result UI moves on, where March's ESC still works during the `m_bGameEnd` wait: that comes with this port. |
| **`ADD_RENA_SYSTEM`** (Rena's natural force) | 5 or 7 | Ask when Rena is next. |
| **Eve's Nasod Core** (`ADD_EVE_SYSTEM_2014` + `FIX_`, about 175 dormant March sites; June ships it) | 7 (`char-eve` CH1q) | **Answered (user, 2026-10-25): March's dormant code as the base**, enabled under `SERV_IRUHADEV_JUNE_CHAR_EVE_NCORE`, June's whole body wherever it differs. |
| **Pilot character** | 5 | Chosen by data (`charport_scan.py` gap plus Stage 2's model diff); the user confirms. **Answered 2026-10-20: Aisha** (`JUNE_CHARACTER_AISHA_PHASES.md`). |

```sql
-- which classes have starter-item rows, and how many each
SELECT UnitClass, COUNT(*) AS rows_
FROM   dbo.GBase_Item
GROUP  BY UnitClass
ORDER  BY UnitClass;
-- (column names as in the live table; adjust if they differ -- ask the user to
--  run `SELECT TOP 1 * FROM dbo.GBase_Item` first if unsure)
```

---

## Standing rules (every stage, every kickoff)

- **Sources.** Read June Lua only from `june_src_fixed/`. Use the `june2017_re/` tools only to extract files missing there ([[june-lua-from-june-src-fixed]]). June C++ behaviour comes from the binary in Ghidra and cites the function address. Never port raw offsets.
- **No partial June updates** ([[no-partial-june-updates]]). If June changed a function March already has, port June's whole version in the same pass.
  This covers the function's own body. Its callees are separate functions, and the scope rule below decides each one.
- **Scope: the plan's subject and the playable path, not the whole call graph (user, 2026-10-19).** When a phase finds a June
  branch it does not own:
  1. **Port it in this phase** if it is part of the function being ported, or if this phase's feature does not work without it.
  2. **Give it a phase in the current plan** only if it is the plan's subject, or if the game is not playable without it:
     a crash, a softlock, blocked progression, or wrong combat numbers on a path the player hits.
  3. **Otherwise it has drifted.** Do not add it to the current plan. Instead:
     - **Identify it.** In the Done note, give its June addresses, what it is, which March code it maps to, whether it is
       reachable offline, and the branches that belong with it (its related callers, data and UI). Add funcmap rows.
     - **Give it a plan of its own, in the same session.** If an existing stage or plan owns the subject (UI -> Stage 9,
       item data -> Stage 16, dungeons -> june-dungeons, character material -> the checklist), add a bullet to that stage's
       section with the addresses and a brief. If none does, add a **new backlog stage**: a row in "The order at a glance",
       a "Stage N" section, and a planning brief `kickoffs/june-backlog/stageN-plan.md` (`/x2-phase new-plan <key>`).
       Group related findings into one stage, not one stage per function.
     - **Leave the call site honest.** Keep March's behaviour there, or stub it with a log line. Never use a guessed value.
  4. Drifted work is deferred, never dropped: each item has an owner and a brief, so it gets done in its stage's turn.
     Choose the bucket yourself and say so in the hand-off. Ask the user only when you cannot tell whether an item
     affects playability.
- **A character's June assets come first and whole (user, 2026-10-20; Stages 5, 7 and 8).** Every non-Lua asset of the character becomes June's,
  byte-exact, in one deploy before that character's code phases. For March characters this is their revamp; for the new characters it is their
  whole set. It covers motions, models, faces, hair, equipment, textures, effects, particle definitions, sounds and cut-in art. No character runs
  half-March, half-June. A March reader that breaks on a June asset gets a code fix, never March's file back. See `JUNE_CHARACTER_AISHA_PHASES.md`
  CH0 / CH0b, the pattern.
- **Shared Lua is ported per character (user, 2026-10-20; Stages 5, 7, 8).** A shared `.lua` table that holds rows for several characters or classes (`UnitTemplet.lua` and the like) is not adopted whole by a character port. Only the rows of the character being ported become June's; every other character's rows stay March's until that character's own port. The file becomes core-list BOM source (`luasrc.py convert` + a core row, `--why "edit: <phase> per-character rows"`) so each later port swaps in its own rows. A Lua file that belongs to one character (its class script, its skill NPC / summon scripts) is still adopted whole.
- **June wins, even when it removes a feature** ([[june-behaviour-wins-even-when-it-removes-features]]), and June's asset wins at the same filename ([[june-asset-wins-over-march-at-same-filename]]).
- **Flags.** Every client change is behind a `SERV_IRUHADEV_` flag in `KTDXLIB/Always.h`, with the original code kept in `#else` and the `#endif FLAG` style. Shared-header changes are flag-gated and offline-only (decision above).
- **Shared-struct flags must reach every project that sees the struct.**
  - `X2ServerProtocol` never includes `KTDX.h`, so `Always.h` does not reach it. It does include `CommonPacket.h`/`ClientPacket.h` through `X2ServerProtocol/X2ServerProtocol.h`.
  - Any flag that changes a shared struct's layout (`EQUIPPED_SKILL_SLOT_COUNT`, unit-info members) must also be defined in `X2ServerProtocol_2010.vcxproj`'s `X2TOOL|Win32` `PreprocessorDefinitions`, the same way `SERV_IRUHADEV_OFFLINE` is.
  - Otherwise the two projects disagree on the struct's size and nothing reports it. See the `KSession` sizeof split in `CLAUDE.md`.
  - Prove it with `cl /P` on a file from each project.
- **CP949.** Make byte-level edits on CP949 files, then verify with `file` and `git diff --stat`.
- **No hardcoded enum literals (user, 2026-10-26; every stage).** An id with a name in an Enum (June's `Enum.lua` or a C++ enum) is written as the member,
  registered under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it; a value with no Enum entry stays a number with a one-line comment;
  a `.tsv` of ids is data, not code. Gate: `python tools/enumscan.py --check <files>`. Rule in `CLAUDE.md`; set by plan `june-enum-names`
  (`JUNE_ENUM_NAMES_PHASES.md`, N0-N5c + NZ, **CLOSED 2026-10-26**, flag `SERV_IRUHADEV_JUNE_ENUM_NAMES`).
- **Dev free-skill gate in every ported class (user, 2026-10-22; Stages 5, 7, 8).** March skips the skill cool-time / MP checks for a dev account in internal builds; June's service binary has no such check. Every class port that brings in June's skill-use check wraps that check (cool time, MP, hyper stone) with `false == JuneIsDevUnlimitedSkillUse()` under `SERV_IRUHADEV_JUNE_DEV_UNLIMITED_SKILL`. The helper uses March's own macros (`TOGGLE_UNLIMITED_SKILL_USE` / `_SERVICE_`, auth `XUAL_DEV`), so defining `_SERVICE_` turns March's and ours off together. It is a dev convenience, not June behaviour, and needs no JUNE_BUGS row. Pattern: Aisha's `June_SpecialAttackLookup` (pilot CH4a). Checklist row in section 7.
- **Packing.** The user packs `.lua` and assets. Build the degrade path, and name the files when the work is done ([[server-lua-packing-is-users-job]]).
- **Game events are switched on from the save DB (user, 2026-10-21; Stages 18, 19 and every event feature).** A game event (a seasonal or limited-time event, event dungeon, event NPC, event reward) that June only activates inside a date window or on another original condition does not rely on that condition. Today some only run with the PC clock set back (e.g. to 2016); that is what this replaces.
  - The trigger is data in `els_db.sql`: an event is on when its row says so, whatever the date. The emulator and the client's event check read it in place of the clock or the server's event list (`EGS_UPDATE_CODE_EVENT_NOT` code events included, where an event uses one).
  - The stage's planning session designs the table, its `X2OfflineDB` migration, and a `tools/elsdb.py` command to switch an event on and off.
  - Keep the rest of June's event logic as June has it; only the "is the event active" answer changes source.
- **Data of record.** The live DB beats `DataBase/` and `ScriptData/` ([[live-db-over-repo-snapshot]]).
- **Saves.** `els_db.sql` is the user's only copy. Snapshot it before any migration, and remember that migrations are forward-only.
- **Models.** Justify the model per phase on cost versus failure. Fable 5.1 is not used ([[no-fable-model]]), so the top tier is Opus 5.5 at `xhigh`. Opus does RE and new mechanisms. Sonnet 5.5 does transcription and data work, with a scoped Opus review. State the effort in every kickoff, and record each session's usage in its Done note.
- **Running the game.** Never launch `X2_offline.exe` without asking first ([[ask-before-running-deployed-exe]]).
- **Play-test checks** cover only the phase's own log lines ([[scope-playtest-log-checks-to-the-phase]]).
- **Commits.** Commit only your own hunks; parallel sessions edit the same docs and the same tree.

---

## Kickoff prompts

### Stage 1, Atlas + Elysion

Use `ATLAS_PORT_PHASES.md`'s **Phase A0 kickoff** (`kickoffs/atlas/A0.md`). It is already written. It creates no branch: the work is on `main`.

### Stage 2 planning (full), Opus 5.5 high

```
Planning session for Stage 2 of the June 2017 port backlog (engine remainder and
measurements). Read in the repo root: JUNE_PORT_BACKLOG_ORDER.md in full (order,
concurrency note, "Facts behind the order", Stage 0's gate table, Stage 2, Standing
rules), then CLAUDE.md, JUNE_KOM_CONTENT_GUIDE.md, JUNE2017_EFFECT_REBUILD_PLAN.md
(status board + Phases 10-12), JUNE2017_CHARACTER_PORT_CHECKLIST.md,
ATLAS_PORT_PHASES.md's investigation ledger (do not redo its census), and
RANOX_PORT_PLAN.md's render-pass notes (grep CTT_RIM_LIGHT_BLUE / ".fx"). Work on
main (the only branch, user 2026-10-16). Never launch X2_offline.exe without asking first. This session
plans and measures; it changes no C++.

Goal: write JUNE_ENGINE_REMAINDER_PHASES.md in the repo root, in the same shape as
JUNE_BUFF_PORT_PHASES.md (Context, fix philosophy, model/effort/review rules, phase
table, investigation ledger, one kickoff prompt + Run-with line per phase).

Before writing phases, measure and record in the ledger (file:line or tool output
for every number):
1. Shaders: extract June data041_kom's .fx files (XRenderer, GCSkin, RenderState,
   XEnvBoxRenderer, XHeatShimmerRenderer, XPostEffect) and diff each against March's;
   for every parameter/technique/pass June's .fx expects, find whether March's C++
   (KTDXLIB KTDGXRenderer / skin-mesh / post-effect code) sets it. Use Ghidra on the
   June binary for June's setter side. Result: which .fx can be adopted as data and
   which need C++ brought to June first.
2. The 7 RENDER_TYPEs missing from KTDXLIB/KTDGXRenderer.h (RT_CARTOON_ALPHA,
   RT_CARTOON_BLACK_EDGE_ALPHA, RT_CUSTOM, RT_DESTALPHA_SRCALPHA_CCW,
   RT_INVSRC_ALPHA_NOT_CULL, RT_UV_ANIM, RT_NONE) and the CUSTOM_RENDER_STATE table:
   June's values, June's implementation addresses, which June files use them.
3. Models: per character, diff mesh / motion / texture files between March
   (24965799/kom_contents, trust only at live-manifest size) and June. Answer "new
   base model, new motions only, or unchanged" per character.
4. UI framework: control types / keys used by June DLG_* files that March's KTDGUI
   parser does not handle (census, not eyeballing; reuse Atlas's region_census.py
   output if it already covers this). Any hit becomes a Stage 2 build phase.
5. Effect-engine check: rerun june2017_re/census.py and the June-data trial load
   against the current tree; confirm which Stage 0 effect items are actually closed.

Rules: JUNE_PORT_BACKLOG_ORDER.md's Standing rules apply. Per phase, justify the
model (Opus 5.5 for RE/new mechanisms, Sonnet 5.5 + scoped Opus review for
transcription; no Fable) and state effort. Code phases are scheduled after the buff
port finishes in the main Trunk (concurrency note). If a measurement contradicts an
assumption in JUNE_PORT_BACKLOG_ORDER.md (e.g. base models unchanged, UI framework
gap found), update that file's affected rows with a dated note. Commit only your own
files; record this session's usage in the doc.
```

### Stages 3–9 (short)

Each of these is a **planning** session. Paste it, then let the session read and ask.

**Stage 3, skill system core** (Opus 5.5 high):
```
Plan backlog Stage 3 (skill system core). Read JUNE_PORT_BACKLOG_ORDER.md (Stage 3,
facts 1-3, Standing rules), JUNE2017_CHARACTER_PORT_CHECKLIST.md section 1,
JUNE_BUFF_PORT_PHASES.md (BBT 98 / BFT 16 / SlotIndexToSlotType notes),
MARCH_2014_MIGRATION.md (SKILL_SLOT_B_FREE, SKILLTREE_NO_LOCK, SetUsingPage trap),
ATLAS_PORT_PHASES.md Phase A2 (the EXP save migration this one must chain after),
and JUNE_ENGINE_REMAINDER_PHASES.md's outcome. Preconditions: Stage 0 gate rows
closed, Stage 2 done. First re-check the order doc's facts still hold, and find
every project that sees CommonPacket.h's skill-slot struct (X2Lib, X2ServerProtocol
via X2ServerProtocol.h): the slot-count flag must be defined in each (Standing
rules, shared-struct flags), proven with cl /P. Output:
JUNE_SKILL_SYSTEM_PHASES.md with sub-steps 3a-3e as phases (3a extraction of
NewSkillTempletVer2 + the other tokenizer-rejected tables; 3d includes the shared
CommonPacket.h struct, flag-gated and offline-only, and an els_db.sql migration
preceded by a db_backup/ snapshot). Never launch X2_offline.exe without asking.
```

**Stage 4, quest system** (Opus 5.5 high):
```
Plan backlog Stage 4 (quest system, including the quests deferred from Lanox,
Atlas and Elysion). Read JUNE_PORT_BACKLOG_ORDER.md (Stage 4, Standing rules, open
decisions), ATLAS_PORT_PHASES.md (the Atlas/Elysion plan, A and E phases; decision 2 "quests out
of plan" and every Done note's quest hand-off), X2Lib/X2QuestManager.h/.cpp,
X2Lib/Offline's quest module, JUNE_UI_PARITY_PHASES.md's
IsActiveDungeon/RequireCompletedQuestID follow-up, and the June quest tables
extracted in Stage 3a. Precondition: Stage 1 (Atlas/Elysion) closed on main. First ask the user
the guild-quest and Scaleform/GFx decisions. Output: JUNE_QUEST_SYSTEM_PHASES.md
(engine: loader keys, new QT/QR/SQT types, chain/group quests, NPC relation quest
side, quest UI, offline completion for every new sub-quest type; content: Lanox /
Atlas / Elysion quest rows, activating the inert REQUIRE_COMPLETE_* keys, quest-
driven NPC swaps, EventScene/movies, old-save quest migration). Never launch
X2_offline.exe without asking.
```

**Stage 5, pilot character** (Opus 5.5 high):
```
Plan backlog Stage 5 (pilot character revamp). Read JUNE_PORT_BACKLOG_ORDER.md
(fact 6, Stages 5-7, open decisions, Standing rules),
JUNE2017_CHARACTER_PORT_CHECKLIST.md in full, Stage 2's model-diff results, and
ATLAS_PORT_PHASES.md Phase A2 (how stat rows 81-99 were handled). Rerun
june2017_re/census.py then charport_scan.py; propose the pilot (smallest gap among
March characters) and get the user's confirmation. Output:
JUNE_CHARACTER_<NAME>_PHASES.md covering scripts, the June-only later class path,
per-class skill tree data, model/motions, UNIT_CLASS additions (shared, flag-gated,
offline-only), server-side stat Lua the user packs. Write it as a reusable template
for Stage 7. Never launch X2_offline.exe without asking.
```

**Stage 6, transcendence** (Opus 5.5 xhigh for the RE phase, high otherwise):
```
Plan backlog Stage 6 (transcendence, on the pilot character). Read
JUNE_PORT_BACKLOG_ORDER.md (fact 4, Stage 6, open decisions, Standing rules), the
Stage 3/4/5 phase docs' Done notes, X2Lib/Offline/X2OfflineLuaEnumSeed.h:39. First
RE how June represents a transcended unit (class IDs vs flag) and whether new class
IDs need GBase_Item rows; if so, raise the live-DB decision before planning further.
Output: JUNE_TRANSCENDENCE_PHASES.md (classes, class-change quest,
TranscendentalClassItem, guide UI, TC slot unlock, TRANSCENDENTAL_SKILL_DATA,
hyper mode, SDT_*_TRANSCEND sockets, emulator). Never launch X2_offline.exe without
asking.
```

**Stage 7, remaining March characters** (Opus 5.5 high to plan; per-character phases per the Stage 5 template):
```
Plan backlog Stage 7. Read JUNE_PORT_BACKLOG_ORDER.md (Stage 7, Standing rules)
and the pilot's JUNE_CHARACTER_<NAME>_PHASES.md and JUNE_TRANSCENDENCE_PHASES.md
Done notes, especially what the pilot got wrong. Order the remaining March
characters by charport_scan.py gap; ask the ADD_RENA_SYSTEM decision when Rena is
scheduled. Output: one phase doc per character from the pilot's template.
```

**Stage 8, new characters** (Opus 5.5 high):
```
Plan backlog Stage 8 (Lu/Ciel, Rose, Ain). Read JUNE_PORT_BACKLOG_ORDER.md (fact 6,
Stage 8, open decisions, Standing rules), the Stage 5-7 docs, and
JUNE_BUFF_PORT_PHASES.md's dormant Rose/Ain/Lu/Ciel notes. Hard gate first: the live DB has
no starter rows for these classes (user, 2026-10-27), so derive each character's
ten starter items from item.kim by the name pattern of the existing classes'
starter items (open decisions table), as a labelled stand-in under its own flag. Output: one phase doc per new character (UNIT_TYPE in the shared
header, flag-gated, offline-only; creation + emulator create-unit; tutorial dungeon;
DUO_TYPE / ROSE_WEAPON_TYPE; activating dormant buff code). Never launch
X2_offline.exe without asking.
```

**Stage 9, UI sweep** (Opus 5.5 high to census; Sonnet 5.5 for adopt-only phases, with Opus review):
```
Plan backlog Stage 9 (UI sweep). Read JUNE_PORT_BACKLOG_ORDER.md ("Why UI is last",
Standing rules) and every earlier stage's Done notes for which DLG_* files they
adopted. Census June DLG_* / UI_* files still March-shaped in the live koms, group
them by the system behind them, and drop any whose backing system is absent (log
why). Output: JUNE_UI_SWEEP_PHASES.md.
```

**Stage 10, emotion system** (Opus 5.5 high):
```
Plan backlog Stage 10 (June emotion system). Read JUNE_PORT_BACKLOG_ORDER.md
(Stage 10, Standing rules), JUNE2017_EFFECT_COMPLETION_PHASES.md "C9d - Done" and
"C9e - Done" (if C9e has run), every funcmap row tagged "Effect C9d", and in
june_src_fixed/: EmotionData.src.lua, Enum.src.lua (EMOTION_TYPE,
CHANGE_EMOTION_CONDITION), DLG_ChatBox_Emotion_List.src.lua. First RE what C9d left
open: the AddEmotionData bind and the EmotionData struct, the CX2Unit +0xa44 insert
site, the item-templet key behind 32651ab0, June PlayEmotion in all three classes
(GUUser 32043380, SquareUnit 323fef90, UnitViewerUI 31fa52b0) end to end, the
two-seater ids, and how the chat box lists and unlocks emotions. Census the motions,
sounds and meshes against March kom_contents. Output: JUNE_EMOTION_SYSTEM_PHASES.md
(enum + Lua table, EmotionData loader, unlocks, cash sits and item sit effects,
the three PlayEmotion rewrites, assets, chat-box UI, emulator if any packet carries
the new ids), kickoffs, and a STATUS block. Never launch X2_offline.exe without asking.
```

**Stage 12, character customize system** (Opus 5.5 high): `kickoffs/june-backlog/stage12-plan.md` (written by the june-effect C9g-2 review, 2026-10-08).
