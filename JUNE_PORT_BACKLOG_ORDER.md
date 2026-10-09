# June 2017 port backlog: execution order

> Written 2026-10-03 on `mods/june-buff-port`. This file sets the **order** of the remaining June-parity backlog and says why. It does not plan any one stage in detail.
>
> Each stage gets its own phase doc, written by a planning session started from that stage's kickoff prompt (bottom of this file). Stage 1 already has its docs: `ATLAS_PORT_PHASES.md` and `ELYSION_PORT_PHASES.md`, commit `24de3d3`. The next stage to plan is Stage 2, and its kickoff is written out in full. The rest are short, and each one asks the planning session to re-check this file's assumptions first.

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
- **Atlas/Elysion is already planned** (2026-10-03, a parallel session: `ATLAS_PORT_PHASES.md` A0–A13 and `ELYSION_PORT_PHASES.md` E0–E15). That session recorded these decisions:
  - Work happens in its own worktree, `../Trunk_elysion` on `mods/elysion-port`.
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
| 1 | Atlas + Elysion *(already planned)* | 1, plus the level cap | Planned, and the user's current priority. It is content on the engine as it stands, with quests deliberately split out. It runs in its own worktree alongside Stage 0's buff work, and it raises the level cap that transcendence assumes. |
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

**Concurrency.** At most two code sessions should run at once:
- Stage 0's buff port, in the main Trunk;
- Stage 1, in `Trunk_elysion`.

Planning sessions are read-only, and can run any time. Stage 2 *planning* can start now. Stage 2 *code* waits for the buff port to finish in the main Trunk, because both touch `KTDXLIB/Always.h` and shared engine files, and a third concurrent branch multiplies merge work.

**Merging.** Stage 1's branch is merged back before Stage 4 starts, because Stage 4 activates the quest keys that Stage 1 left inert.

Stages 5–8 are the long pole. Stages 0–4 are prerequisites or self-contained content, so finishing them first means the character work is pure repetition and not discovery.

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

   **The "new base model" claim is not verified.** Stage 2 measures it.
7. **Shaders.**
   - June's `.fx` files (`XRenderer.fx`, `GCSkin.fx`, `RenderState.fx`, `XPostEffect.fx`, …) are in `data041_kom`, which is not in `june_src_fixed/` (`JUNE_KOM_CONTENT_GUIDE.md:69`).
   - Seven June `RENDER_TYPE`s are missing from `KTDXLIB/KTDGXRenderer.h`: `RT_CARTOON_ALPHA`, `RT_CARTOON_BLACK_EDGE_ALPHA`, `RT_CUSTOM`, `RT_DESTALPHA_SRCALPHA_CCW`, `RT_INVSRC_ALPHA_NOT_CULL`, `RT_UV_ANIM`, `RT_NONE`. About 25 June files use them.
   - The June-only `CUSTOM_RENDER_STATE` enum table truncates June's Add and Ara scripts on March's `Enum.lua` (checklist).
   - Swapping in June's `.fx` is a pure data change **only if** March's C++ sets the same effect parameters and techniques. That has to be checked, not assumed.
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

Start from `ATLAS_PORT_PHASES.md`'s Phase A0 kickoff. That doc owns the shared groundwork (worktree, census, RE maps, level cap, enums); `ELYSION_PORT_PHASES.md` assumes it.

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

The last is the tail of `Handler_EGS_UPDATE_CODE_EVENT_NOT` (`31f00970`). On `mods/june-buff-port` that handler's flag-on block (`SERV_IRUHADEV_JUNE_PET_PREVIEW_LOOKUP`, C9h-2d) already copies June's map and two sets but does not make the call. Add the call there, behind E1's own flag; whichever branch merges second wires it. Rows: funcmap `Effect C9h-2e`.

**Handed in by june-effect C9h-5 (2026-10-08):** E1 step 6. June's `CX2TFieldNpc` is a rewrite. Its `OnFrameMove` has 44 blocks, and the house loader reads 56 keys March lacks. It includes the village-NPC effect set (`EFFECT_SET_NAME`, used only by the three Elysion houses) and its `+0xe0` engine branches, which sit next to C9h-3a's. Merge `mods/june-buff-port` after C9h-3a lands, before E1 ports them. The friendship branches go to Stage 18. Rows: funcmap `Effect C9h-5`.

### Stage 2: engine remainder and measurements

The planning session writes `JUNE_ENGINE_REMAINDER_PHASES.md` (full kickoff below).

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

### Stage 6: transcendence system, on the pilot

- Transcended classes (decide how June represents them: `UC_*_2` or a flag; RE first).
- The class-change quest (`QT_CHANGE_TRANSCENDENTAL_CLASS`), `TranscendentalClassItem`, and the guide UI.
- Unlocking the TC slot, `TRANSCENDENTAL_SKILL_DATA`, and `USI_TRANSCENDENTAL_HYPER_MODE`.
- Transcendence sockets (`SDT_*_TRANSCEND*`) and emulator support.

**Live-DB gate:** if transcendence uses new class IDs, see the open decision on `GBase_Item` below.

**Why after Stage 5:** transcendence skills are part of June's revamped kit, so transcending a March-kit character would be ported twice.

### Stage 7: remaining March characters

- Repeat Stages 5 and 6 per character with the proven recipe.
- One phase doc per character, or one doc with a phase per character; the planning session chooses.

### Stage 8: new characters (Lu/Ciel, Rose, Ain)

Everything from Stages 5 and 6, plus:
- a new `UNIT_TYPE` (shared header, flag-gated);
- character creation and the emulator's create-unit path;
- `Dungeon_Tutorial_{Luciel,Rose,Ain}`;
- Lu/Ciel's duo mechanics (`DUO_TYPE`, `HIT_ADD_COMBINATION_POINT`) and Rose's `ROSE_WEAPON_TYPE`;
- activating the dormant Rose/Ain/Lu/Ciel code from the buff port.
- the user-sync members only these classes drive (june-effect C3e, `june2017_re/c3e_user_sync_fields.tsv` rows B, E, F, H, I, G):
  - Lu/Ciel's dual body: the second-body position (bits `0x20000`-`0x80000`) and the active body (`0x1000000`), on `+0x8e88` / `+0x8e90` / `+0x8e91`;
  - Lu/Ciel's float (`0x10000`); the class short of Ain (Disparate Force), Rose and Lu/Ciel (`0x100000`); Ain's 10-dword array (`0x20000000`);
  - their drivers of the class-skill float C3e-1 ports (`0x200000`), including Rose's Neil the Sniper aim (`3200d3f0` / `3200d900`).
- the item-templet fields June-item-kim K0 found for them: Lu/Ciel's minor-unit item id and flag, the Extra keys 10-13,
  and their item-type and unit-class guards from K2 (character-port checklist 7g).
- Ain's SetStopTime / SetStop2Time overrides (june-effect C3e-3, funcmap `32a38e70 / 32a38ea0 (C3e-3)`): the stop time times Ain's
  `+0x97ac`, then the user's 4-argument setters that C3e-4 ports.

**Hard gate:** the live-DB starter-item rows (below).

### Stage 9: UI sweep

Port whatever June UI is still March-shaped once Stages 1–8 have delivered their own dialogs.

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

**Depends on:** Stage 2 (UI framework measurement) and june-effect C9e (the `32355a50` helper and the three effect-set handles,
which this stage widens). Lu/Ciel, Rose and Ain rows stay inert until Stage 8, as C9c-1's Rose/Ain rows do.

- **`EmotionFrameMove`'s riding-sit sound replay** (june-effect C9h-4, 2026-10-08). June's `3204a860` adds a new head that runs while an
  emotion plays (`+0x89d8`) and its anim has not ended. For emotions 2..3 while riding, it replays the state's sound records on the riding
  pet's clock (3D forced on, loop off, records not erased) and attaches their delete conditions (`3227a740`). Depends on june-effect
  C9h-4a (the record and the attach).

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

- **The custom-sit / two-seater sync** (june-effect C3e): user-sync bits `0x2000000` / `0x4000000`. `320455e0` writes the customize
  manager's sit id (`32363280`, map key 7) and the pending emotion byte `+0x94b0`. `320458f0` plays them on other players
  (`PlayEmotion` `32043380`, the `0x2c` / `0x2d` two-seater). It is wire only, since offline has no other player.

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

### Stage 18: NPC friendship system (june-effect C9h-5, 2026-10-08)

june-effect C9h-5 diffed June's `CX2TFieldNpc::OnFrameMove` (`june2017_re/c9h5_fieldnpc_onframemove_diff.md`, blocks 19, 26, 30, 32 and 33). The village NPC calls into a June-only friendship system. March has none of it.
- **The manager:** singleton getter `32696fa0` (0x610 bytes), lookup `321cd500( houseID, &value, &flag )`, max test `325d1d70` (value >= `+0x490`), a gift-cursor flag at `+0x4a4`, plus `32679e10` and `326878c0`.
- **The NPC side:** the talk sequencer `325c9e70` (NPC `+0x5a4..+0x5ab`), the friendship mark on particle `+0x230` (`Npc_Talk.dds` / `Npc_FriendShip_Max.dds`), and the gift cursor (state 0x1a, likely) with talk `+0x588`.
- **The keys:** the house keys `Receive_Like/DisLike/Normal/Cash_Gift_Talk`, `Level_Up_Talk`, `Every_Day_Reward_Talk`, `After_Talk`, `Reject_Gift_Talk` and `Gift_UI_Talk` (HouseParsing `324f3260`; the C9h-5 Done note lists the addresses).
- **The gate:** everything sits behind `IsEnableCode( 2215 )` (`@325cec47`).
  - The offline emulator never sets a code event, so nothing here is reachable until code events are emulated.
  - Elysion E1 ports the gated call sites with June's gate; whichever of E1 and this stage lands second wires them.
- **Still to identify:** the manager's loader, data file and packets. Name them when this stage is planned.

**Depends on:** Elysion E1 (the call sites), and a decision to emulate code event 2215.

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

## Open decisions that gate stages

| Decision | Gates | How to settle |
|---|---|---|
| **Starter items for classes the live DB never had.** Live `gup_create_unit` copies exactly ten `dbo.GBase_Item` rows per class and aborts with `-28` otherwise. Rose, Ain, Lu/Ciel, and possibly transcended classes, are almost certainly absent. The no-band-aid rule forbids inventing rows unless the user authorises it, as they did for A2's stat rows. | 8 (and 6 if transcendence adds class IDs) | Ask the user for a live-DB pull (query below). If the rows are absent, the user decides the source: June data, if it exists anywhere, or an explicit, logged, user-chosen set. |
| **Stat rows for June-only class IDs.** The user authorised inventing `StatTable`/`NpcExpTable` rows 81–99 only for classes that already exist (Atlas A2). The later class paths and the new characters need a **full** per-level row set, and `KncWX2Server/ServerResource/US/` has none for them. | 5, 7, 8 | First look for June-side data: the June binary, any June server Lua, or the live DB. If there is none, ask the user whether A2's permission extends to these classes, and how the rows should be derived. |
| **Guild quests offline** | 4 | Ask: parse-and-log or a real port. |
| **Scaleform/GFx runtime** for EventScene and quest movies | 4 | Ask: a runtime, or skip with a log line. |
| **`ADD_RENA_SYSTEM`** (Rena's natural force) | 5 or 7 | Ask when Rena is next. |
| **Pilot character** | 5 | Chosen by data (`charport_scan.py` gap plus Stage 2's model diff); the user confirms. |

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
- **June wins, even when it removes a feature** ([[june-behaviour-wins-even-when-it-removes-features]]), and June's asset wins at the same filename ([[june-asset-wins-over-march-at-same-filename]]).
- **Flags.** Every client change is behind a `SERV_IRUHADEV_` flag in `KTDXLIB/Always.h`, with the original code kept in `#else` and the `#endif FLAG` style. Shared-header changes are flag-gated and offline-only (decision above).
- **Shared-struct flags must reach every project that sees the struct.**
  - `X2ServerProtocol` never includes `KTDX.h`, so `Always.h` does not reach it. It does include `CommonPacket.h`/`ClientPacket.h` through `X2ServerProtocol/X2ServerProtocol.h`.
  - Any flag that changes a shared struct's layout (`EQUIPPED_SKILL_SLOT_COUNT`, unit-info members) must also be defined in `X2ServerProtocol_2010.vcxproj`'s `X2TOOL|Win32` `PreprocessorDefinitions`, the same way `SERV_IRUHADEV_OFFLINE` is.
  - Otherwise the two projects disagree on the struct's size and nothing reports it. See the `KSession` sizeof split in `CLAUDE.md`.
  - Prove it with `cl /P` on a file from each project.
- **CP949.** Make byte-level edits on CP949 files, then verify with `file` and `git diff --stat`.
- **Packing.** The user packs `.lua` and assets. Build the degrade path, and name the files when the work is done ([[server-lua-packing-is-users-job]]).
- **Data of record.** The live DB beats `DataBase/` and `ScriptData/` ([[live-db-over-repo-snapshot]]).
- **Saves.** `els_db.sql` is the user's only copy. Snapshot it before any migration, and remember that migrations are forward-only.
- **Models.** Justify the model per phase on cost versus failure. Fable 5.1 is not used ([[no-fable-model]]), so the top tier is Opus 5.5 at `xhigh`. Opus does RE and new mechanisms. Sonnet 5.5 does transcription and data work, with a scoped Opus review. State the effort in every kickoff, and record each session's usage in its Done note.
- **Running the game.** Never launch `X2_offline.exe` without asking first ([[ask-before-running-deployed-exe]]).
- **Play-test checks** cover only the phase's own log lines ([[scope-playtest-log-checks-to-the-phase]]).
- **Commits.** Commit only your own hunks; parallel sessions edit the same docs and the same tree.

---

## Kickoff prompts

### Stage 1, Atlas + Elysion

Use `ATLAS_PORT_PHASES.md`'s **Phase A0 kickoff**. It is already written, and creates the worktree.

### Stage 2 planning (full), Opus 5.5 high

```
Planning session for Stage 2 of the June 2017 port backlog (engine remainder and
measurements). Read in the repo root: JUNE_PORT_BACKLOG_ORDER.md in full (order,
concurrency note, "Facts behind the order", Stage 0's gate table, Stage 2, Standing
rules), then CLAUDE.md, JUNE_KOM_CONTENT_GUIDE.md, JUNE2017_EFFECT_REBUILD_PLAN.md
(status board + Phases 10-12), JUNE2017_CHARACTER_PORT_CHECKLIST.md,
ATLAS_PORT_PHASES.md's investigation ledger (do not redo its census), and
RANOX_PORT_PLAN.md's render-pass notes (grep CTT_RIM_LIGHT_BLUE / ".fx"). Ask which
branch/worktree to plan on: the main Trunk is the buff port's, Trunk_elysion is
Atlas/Elysion's. Never launch X2_offline.exe without asking first. This session
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
decisions), ATLAS_PORT_PHASES.md and ELYSION_PORT_PHASES.md (decision 2 "quests out
of plan" and every Done note's quest hand-off), X2Lib/X2QuestManager.h/.cpp,
X2Lib/Offline's quest module, JUNE_UI_PARITY_PHASES.md's
IsActiveDungeon/RequireCompletedQuestID follow-up, and the June quest tables
extracted in Stage 3a. Precondition: mods/elysion-port merged. First ask the user
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
JUNE_BUFF_PORT_PHASES.md's dormant Rose/Ain/Lu/Ciel notes. Hard gate first: ask the
user for the live GBase_Item pull (query in the order doc) and settle the starter-
item source. Output: one phase doc per new character (UNIT_TYPE in the shared
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
