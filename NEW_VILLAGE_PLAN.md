# Adding village Ranox (Lanox) — its dungeons, fields, and new monster types (offline mode) — phase plan

## Context

This is the phased integration plan for adding **village Ranox** — the game's internal codename for what's marketed in-game as "Lanox" (the same codename/display-name split as `Peita`/Feita elsewhere in this tree) — to the offline client (`SERV_IRUHADEV_OFFLINE`, branch `mods/offline-mod-2`): a village with its own dungeons and battlefields, same as the existing 8 (Ruben, Elder, Besma, Altera, Peita, Velder, Hamel, Sander). It follows the same shape as `MARCH_2014_MIGRATION.md`/`OFFLINE_MODE_PLAN.md`: read it before starting work, and treat each phase as something you can hand to a **fresh Claude Code conversation** on its own, using the "Start this conversation with" prompt at the end of that phase's section. A fresh session should not need to re-derive anything already settled here — it just needs to read this file, **`NEW_VILLAGE_RANOX_IDENTIFIERS.md`** (the full symbol table — every `WI_`/`VMI_`/`LMI_`/`BFI_`/`DI_`/`NUI_` name Ranox needs, with status and newer-build reference numbers, produced in Phase 0), plus whatever files earlier phases produced or the user is supplying.

**Scope:** offline-mode only. No real GameServer/CenterServer changes, no VS2003 rebuild — the servers are unbuildable in this environment anyway, and (per *Why this is safe* below) the offline "server" for every subsystem here — village entry, dungeon entry, battlefield entry — is compiled into the same binary as the client.

**Content shape:** Ranox has 7 dungeon lines — **Gateway, AshesVillage, BurnedForest, FireTemple, FinalHall, Geyser, Secret** — each with Normal/Hard/Expert plus a Hero variant of each (Secret instead has a single Hell tier plus an `UNUSED_DUNGEON_ID`-namespaced Common entry); **4 fields** (`Ranox_Field_01`–`04`, plus a 5th rest-area map); a **dungeon lounge**; a **spa** sub-location; and **~129 new monster/object types**. Every phase below describes **one repeatable pattern per dungeon line / per field / per monster** — run it once per item, not once total. `NEW_VILLAGE_RANOX_IDENTIFIERS.md` has the exhaustive per-item lists; this file doesn't repeat them.

**Naming:** every symbol uses `RANOX` (`WI_VILLAGE_RANOX`, `DI_RANOX_GATEWAY_NORMAL`, `VMI_BATTLE_FIELD_RANOX_FIELD_01`, `NUI_MOUNT_GATE`, etc.) — see `NEW_VILLAGE_RANOX_IDENTIFIERS.md` for the full list. Names there are transcribed verbatim from the source data, typos included (`BURNDEDFOREST`, `ASHESVILLAG_GATE`, `ASHESVILLAG_SWITCH_GATE`, `GAYSER_BOSS`, `REGIEON_POINT`, `INCHANT`) — this repo's "transcribe, don't tidy" rule, since Lua and the wire both reference these exact strings.

## Why this is safe (read once, applies to every phase)

- `WORLD_ID` (`X2Lib/X2World.h`) is a client-only enum. No `.cpp` in this tree references any `WI_*` value by name — it exists purely as a Lua-facing symbol, populated at runtime via `CX2WorldManager::AddWorldData_LUA` (`X2WorldManager.cpp:170`). This applies equally to the village hub, the dungeon lounge, the spa, and every dungeon-stage and field map — each is its own `WORLD_ID` with its own world Lua script and line-map.
- `SEnum::VILLAGE_MAP_ID`, `LOCAL_MAP_ID`, `BATTLE_FIELD_ID` (`KncWX2Server/Common/Enum/Enum.h:196-335`) and `SEnum::DUNGEON_ID` (`KncWX2Server/Common/Enum/DungeonEnum.h:5-488`) are declared in headers shared with the real servers. Normally a shared-header change means both client and server must be rebuilt together (this repo's own contract rule) — but every subsystem this plan touches (village entry, dungeon entry, battlefield entry) is emulated entirely inside `X2Lib/Offline/`, built as part of the client. There is nothing on the other side of this wire that isn't in the same binary, so skipping a server rebuild here doesn't create the drift the rule warns about.
- **`DUNGEON_ID` and `BATTLE_FIELD_ID` entries are explicit numeric literals, not auto-incremented** (confirmed: `DungeonEnum.h:487`, `DI_END = 41311`) — unlike `VILLAGE_MAP_ID`/`LOCAL_MAP_ID`, which auto-increment and must be inserted right before their `_END` sentinel. A new dungeon or field can take any unused number without any renumbering risk at all. (`BATTLE_FIELD_ID`'s `BFI_*` values are the one exception worth remembering — they aren't independent numbers at all, they're aliases of `VILLAGE_MAP_ID` entries; see Phase 1 Step 4.)
- `NPC_UNIT_ID` (`X2Lib/X2UnitManager.h:60-2557`) is a ~1900-entry enum with an explicit ceiling comment (keep `NUI_MAX_LIMIT` under 65536) — appending Ranox's ~129 new values before that sentinel is safe and has plenty of headroom.
- **Every subsystem's client-facing data is loaded purely from Lua, with no additional C++ needed**, once the enum values exist:
  - Village: `CX2LocationManager::OpenScript` (`X2Lib/X2LocationManager.cpp:129-143`) loads `DLG_Map_World.lua`/`DLG_Map_Local.lua`/`DLG_Map_Village.lua`/`DLG_Map_House.lua`; `VillageTemplet`/`HouseTemplet`/`LocalMapTemplet` (`X2Lib/X2LocationManager.h`) are populated purely from that Lua.
  - Dungeon: `CX2DungeonManager::AddDungeonData_LUA` (`X2WorldManager.cpp` sibling, `X2DungeonManager.cpp:77-101`) reads `DungeonData.lua`; each dungeon's own stage script is parsed by `CX2DungeonSubStage::SubStageData::LoadData` (`X2DungeonSubStage.cpp:1298`).
  - Battlefield: `CX2BattleFieldManager::AddBattleFieldData_LUA` (`X2BattleFieldManager.cpp:325,339`) reads `BattleFieldData.lua`; each field's own script calls `AddSpawnMonsterGroup`.
  - Monster: `CX2Data::ResetUnitManager` (`X2Data.cpp:1790`) reads `UnitTemplet.lua`/`NPCTemplet.lua`/`NPCStat.lua`; the monster's own gameunit script (named by `NPCTemplet.lua`'s `LUA_FILE_NAME` field) is parsed by `CX2GUNPC::InitState` (`X2GUNPC.cpp:4498`) for its FSM and mesh attachment.
- **The offline entry handshakes for all three are already fully generic** — no per-ID whitelist anywhere, so **no offline-mode C++ changes are needed** for any of this:
  - Village: `Handler_EGS_STATE_CHANGE_FIELD_REQ`/`Handler_EGS_FIELD_LOADING_COMPLETE_REQ` (`X2Lib/Offline/Handlers_Field.cpp:62-164`).
  - Dungeon: `OpenRoom`/`BuildStageNpcData`/the full stage-load chain (`X2Lib/Offline/Handlers_Room.cpp:482,1669-1879,1902-2149`) — a missing dungeon script fails cleanly with `"no dungeon script for ID=%d"` (`:1688`) rather than hardcoding anything.
  - Battlefield: `Handler_EGS_JOIN_BATTLE_FIELD_REQ` and the respawn/danger-value tick (`Handlers_Room.cpp:1241-1328,891-998`).
- **One confirmed gap, not a blocker:** there is no offline-mode-specific loader for monster stat/mesh data (unlike `X2OfflineStatTable`), because `NPCTemplet.lua`/`NPCStat.lua`/`UnitTemplet.lua` load once at client startup via the same client-native path real gameplay always used (`X2StateStartUp.cpp:186`), not per-dungeon like the `X2Offline*` loaders. This should work without any offline-specific code — but treat it as something to verify in Phase 7, not assume.
- Drop/reward data for the new monsters goes into files that **already exist as source** in this tree — `KncWX2Server/ServerResource/US/DropTable.lua`, `StaticDropTable.lua`, `NpcExpTable.lua` — so that piece is pure addition, no unseen-file merge risk. `BattleFieldServerData.lua` (also already in-tree) needs **no edit at all** — it's global tuning shared across every field, not per-field data.
- `Enum.h`, `DungeonEnum.h`, `X2World.h`, and `X2UnitManager.h` all carry CP949 (Korean) bytes in their comments. Every edit to any of them must be followed by the encoding gut-check this repo's `CLAUDE.md` mandates: `file <path>` should report the same encoding as before, and `git diff --stat`'s changed-line count should roughly match what you actually inserted — this is **not** "a handful of lines" as a blanket rule. Phase 1 alone adds on the order of 230 new lines across these four files combined (129 `NUI_`, 44 `DI_`, 43 `WI_`, plus the `VMI_`/`LMI_`/`BFI_` set) — that is the expected size, not a failure sign. What the check actually catches is a corrupted *existing* file: if the diff touches lines you didn't add, if the changed-line count is a large multiple of what you inserted, or if `file` reports a different encoding than before, stop — `git checkout -- <path>` (confirm with the user first) and redo the edit at the byte level.

## Phase 0 — Handoff: gather every source file — **STATUS: COMPLETE**

Findings and the full per-item symbol table are in **`NEW_VILLAGE_RANOX_IDENTIFIERS.md`**. Summary: all master/registry files (`DungeonData.lua`, `BattleFieldData.lua`, `NPCTemplet.lua`) plus every world/mesh script for the village and all 7 dungeon lines (33 stage maps total) and all 4 fields' monster-spawn scripts are in hand, at `lanox_new_village_files/`.

**Confirmed not needed:** a dedicated dungeon-gate map (Ranox has none, like Sander — confirmed against the newer build's full enum dump, not just absence-of-evidence), and any edit to `UnitTemplet.lua` (it's the player-class base-item file, unrelated to monster registration — resolves what was an open question in earlier drafts of this plan).

**Still outstanding — request before or during the phase noted:**

- `NPCStat.lua` — no monster HP/ATK/DEF data at all yet (needed before Phase 5).
- `DLG_Map_Local.lua`, `DLG_Map_Village.lua`, `DLG_Map_House.lua` — none provided (needed for Phase 2).
- Dungeon-lounge world script + line-map — the ID exists (`VMI_DUNGEON_LOUNGE_RANOX`), no map (needed for Phase 2/3).
- Spa world script + line-map + house entry — the IDs exist (`VMI_SPA_RANOX`, `WI_SPA_RANOX`, `HI_SPA_SIGN_RANOX`, `HI_SPA_INNER_NPC_RANOX`), no map (needed for Phase 2).
- 5 field world scripts + line-maps — `WI_BATTLE_FIELD_RANOX_FIELD_00..03` plus `..._REST_00`; only the monster-spawn scripts are in hand, not the maps (needed for Phase 4).
- ~129 per-monster gameunit scripts (FSM/mesh) — `NPCTemplet.src.lua`'s `LUA_FILE_NAME` field names each one exactly; extract the filename list from there rather than guessing (needed for Phase 5).
- ~157 binary assets (mesh `.Y`, texture `.tga`, audio `.ogg`) referenced by the scripts already in hand, plus a handful of `.dds` textures for the world-map UI dialogs — none present in the folder or the live game directory (needed for Phase 7; nothing renders or plays without them).

**Out of this plan's scope unless separately requested:** the Gateway PvP map (`WI_RANOX_GATEWAY_PVPMAP`), the exact wiring for the world-map UI dialogs (`DLG_UI_FIELD_MAP_Ranox_*`, `DLG_UI_RANOX_SECRET_FEVER_GAGE` — in hand but not part of the village/dungeon/field/monster critical path), and the buff/title/currency system the newer build's enum dump reveals (`BTI_BUFF_RANOX_SECRET_FEVER`, `BTI_BUFF_RANOX_6TH_DUNGEON_TITLE`, `BTI_BUFF_ELIXIR_RANOX_DUNGEON_56_EVENT`, etc.) — this plan doesn't cover buffs/titles at all.

## Phase 1 — Enum and flag wiring (C++)

**Goal:** give every Ranox identity (village, dungeon lounge, spa, each of the 7 dungeon lines × difficulty, each of the 4 fields, each of the ~129 monsters) a symbol the rest of the build can reference.

**Steps:**
1. Before touching any enum file, confirm how `WORLD_ID` names reach Lua **in this tree specifically** — grep for a `WORLD_ID` equivalent of `CX2OfflineLuaEnum` (which is confirmed to publish `VILLAGE_MAP_ID` to Lua by reading `Enum.h` directly — see `X2Lib/Offline/X2OfflineMapData.h`'s header comment). This is genuinely unresolved, not a formality: Phase 0 confirmed the *newer client* publishes `WORLD_ID` names to Lua (that's what `NEW_VILLAGE_RANOX_IDENTIFIERS.md`'s `Enum.src.lua`-derived table is) and that this tree's `CX2LocationManager::OpenScript` reads a `World = {...}` dispatch table keyed by ID — neither of those confirms this tree exposes `WORLD_ID.WI_*` symbols to Lua the same way. This is load-bearing: every dungeon and field script already in hand (`Dungeon_Ranox_Gateway_Normal.src.lua:46` and ~40 others) references `WORLD_ID.WI_RANOX_...` by symbol. If this tree doesn't publish `WORLD_ID` to Lua, every one of those resolves to `nil` and every stage script fails to load silently. Confirm before relying on it; if it's not published, every `WORLD_ID.WI_RANOX_*` reference across every script in hand needs to become a raw integer instead.
2. Add one flag to `KTDXLIB/Always.h`, using the house comment block:
   ```c
   //////////////////////////////////////////////////////////////////////////
   // Author: Iruha
   // Date: <today>
   // Description: New village Ranox (Lanox) - dungeons, fields, monsters - enum/identity wiring
   #define SERV_IRUHADEV_VILLAGE_RANOX
   //////////////////////////////////////////////////////////////////////////
   ```
   (One flag for the whole village's worth of new content is enough — this is one logical change, per this repo's "one flag per logical change" rule.)
3. `X2Lib/X2World.h`, `WORLD_ID` enum — append the 43 entries in `NEW_VILLAGE_RANOX_IDENTIFIERS.md`'s `WORLD_ID` table (the village, the spa, 33 dungeon-stage maps across the 7 lines, 5 field/rest maps, and the Gateway PvP map only if that's in scope). Resolved: **yes**, every dungeon stage is its own `WORLD_ID` — the original plan's open question about whether dungeons use distinct per-stage world maps is settled.
4. `KncWX2Server/Common/Enum/Enum.h`:
   - `LOCAL_MAP_ID` — `LMI_RANOX` after `LMI_SANDER` (currently last).
   - `VILLAGE_MAP_ID` — `VMI_RANOX` (the village itself), `VMI_DUNGEON_LOUNGE_RANOX` in the `DUNGEON LOUNGE` block, `VMI_SPA_RANOX` (a new kind of sub-location — no other village has one; place it once `DLG_Map_House.lua`/`DLG_Map_Local.lua` are in hand and show where it's referenced from), and **`VMI_BATTLE_FIELD_RANOX_FIELD_01`..`04` plus `VMI_BATTLE_FIELD_RANOX_REST_00`** — insert immediately before `VMI_BATTLE_FIELD_END` / `VMI_BATTLE_FIELD_REST_END` respectively (these auto-increment, so order matters). **No dungeon-gate entry** — confirmed absent from the newer build's own enum dump, and Sander (the prior newest village) has none either.
   - `BATTLE_FIELD_ID` — **alias, don't allocate a new number**: `BFI_RANOX_FIELD_01`..`04` `= VMI_BATTLE_FIELD_RANOX_FIELD_01`..`04`, matching every existing village's pattern (e.g. `BFI_RUBEN_FIELD_01 = VMI_BATTLE_FIELD_RUBEN_FIELD_01` at `Enum.h:292`). Do **not** follow a "pick any unused `BFI_` number" instruction here — a runtime range check (`Enum.h:501`: `VMI_BATTLE_FIELD_RUBEN_FIELD_01 <= id < VMI_BATTLE_FIELD_END`) identifies a valid battlefield ID by whether it falls inside `VILLAGE_MAP_ID`'s own numeric block; a `BFI_` value with its own standalone number outside that range would silently fail the check.
   All guarded by the flag.
5. `KncWX2Server/Common/Enum/DungeonEnum.h`, `DUNGEON_ID` — append the 44 entries in `NEW_VILLAGE_RANOX_IDENTIFIERS.md` (7 dungeon lines × Normal/Hard/Expert + Hero-Normal/Hard/Expert/Hell, plus Secret's Hell + Common), any unused explicit numbers before `DI_END = 41311`. One of the 44, `DI_RANOX_SECRET_COMMON`, is declared under `UNUSED_DUNGEON_ID` in the source data rather than `DUNGEON_ID` — check whether this tree even has an `UNUSED_DUNGEON_ID` enum before assuming it needs a home; if it doesn't, ask whether the entry is actually needed at all (the namespace name suggests the studio itself treats it as dead). Guarded by the flag.
6. `X2Lib/X2UnitManager.h`, `NPC_UNIT_ID` — append the 129 new entries in `NEW_VILLAGE_RANOX_IDENTIFIERS.md`, before `NUI_MAX_LIMIT`, staying under the documented 65536 ceiling. Two referenced names, `NUI_CHEST` and `NUI_GATE_OF_NIGHTMARE`, already exist in this tree — do **not** redefine them. Guarded by the flag.
7. Run the encoding gut-check from *Why this is safe* on every edited header (`X2World.h`, `Enum.h`, `DungeonEnum.h`, `X2UnitManager.h`) — expect roughly 230 new lines across the four files combined; that is the correct size for this insertion, not a red flag.

**Files touched:** `KTDXLIB/Always.h`, `X2Lib/X2World.h`, `KncWX2Server/Common/Enum/Enum.h`, `KncWX2Server/Common/Enum/DungeonEnum.h`, `X2Lib/X2UnitManager.h`.

**Exit criteria:** every enum value in `NEW_VILLAGE_RANOX_IDENTIFIERS.md` exists and is guarded; the `BFI_` entries alias `VMI_BATTLE_FIELD_*` rather than allocating standalone numbers; encoding checks pass on all four edited headers.

**Start this conversation with:**
> Read `NEW_VILLAGE_PLAN.md` and `NEW_VILLAGE_RANOX_IDENTIFIERS.md` in full, then do Phase 1 for village Ranox. Add the `SERV_IRUHADEV_VILLAGE_RANOX` flag to `Always.h`, then every enum value the identifiers file lists across `X2World.h`, `Enum.h` (village, dungeon lounge, spa, and the field `VMI_`/`BFI_` alias pair — not a standalone `BFI_` number), `DungeonEnum.h`, and `X2UnitManager.h` (129 new `NUI_` — `NUI_CHEST` and `NUI_GATE_OF_NIGHTMARE` already exist, don't redefine them), all guarded by that flag. First confirm whether this tree's build actually publishes `WORLD_ID` names to Lua before assuming the dungeon/field scripts' `WORLD_ID.WI_RANOX_...` references will resolve. Run the CP949 encoding check from this repo's `CLAUDE.md` on every header you edit — expect on the order of 230 new lines combined, not "a handful."

## Phase 2 — Village Lua integration

**Goal:** merge Ranox's own data into the four location-manager Lua files.

**Steps:**
1. `DLG_Map_World.lua`/`DLG_Map_World_Elios.lua`: Ranox does **not** need a new `WORLD_MAP_ID` entry. This tree's `CX2LocationManager::OpenScript` (`X2LocationManager.cpp:150`) reads `DLG_Map_World.lua` as a `World = { [id] = "filename" }` dispatch table, and the only `WORLD_MAP_ID` this tree has is `WMI_ELLIOS`. Ranox's world-layer presence is a single `LOCAL_MAP_ID.LMI_RANOX` line added to `DLG_Map_World_Elios.lua`'s `LocalIDList`. The copy of that file already in hand also lists `LMI_ELYSION` — a second village outside this plan's scope — **trim that line** when merging, don't copy the file in verbatim.
2. Merge Ranox's `LocalMapTemplet` grouping into `DLG_Map_Local.lua` (not yet in hand — request it), including `m_DungeonList` (all 44 `DI_RANOX_*` IDs from Phase 1) and `BattleFieldIDList`/`BattleFieldRestData` (the 4 `BFI_RANOX_FIELD_*` + rest IDs from Phase 1). This file is also where a dungeon-lounge hookup would be declared, if Ranox routes to its dungeons through one — confirm the exact flow once this file is in hand.
3. Merge Ranox's `LocationManager:Village{ VillageMapID = VILLAGE_MAP_ID["VMI_RANOX"], BaseLocalStateID = LOCAL_MAP_ID["LMI_RANOX"], START_POS = {...} }` block into `DLG_Map_Village.lua` (not yet in hand), using `KncWX2Server/GameClient/DLG_Map_Village.lua` as a formatting reference.
4. Merge Ranox's houses/NPCs into `DLG_Map_House.lua` (not yet in hand) — including the spa (`VMI_SPA_RANOX`, `HI_SPA_SIGN_RANOX`, `HI_SPA_INNER_NPC_RANOX`) — making sure every house has a `m_vecCommonPos` entry for `WI_VILLAGE_RANOX` specifically — required, or the assert in `X2TFieldGame.cpp:3186` fires.

**Files touched:** the four `DLG_Map_*.lua` files.

**Exit criteria:** all four files reference the same Ranox IDs consistently, with the village linking to all 44 dungeon IDs and all 4+1 field/rest IDs from Phase 1.

**Start this conversation with:**
> Read `NEW_VILLAGE_PLAN.md` and `NEW_VILLAGE_RANOX_IDENTIFIERS.md` in full, then do Phase 2 for village Ranox (enum values already added in Phase 1). Here are `DLG_Map_Local.lua`, `DLG_Map_Village.lua`, and `DLG_Map_House.lua`: [attach]. Merge Ranox's entries into all four `DLG_Map_*` files — including trimming the unrelated `LMI_ELYSION` line out of the `DLG_Map_World_Elios.lua` already in hand — cross-checking every ID against Phase 1 and `NEW_VILLAGE_RANOX_IDENTIFIERS.md`.

## Phase 2b — Cobo Express wiring (optional)

**Goal:** make Ranox reachable via paid fast-travel.

**Steps:** add Ranox's `AddMapData`/`AddLocalMapInfo` lines to `KncWX2Server/ServerResource/US/MapData.lua`.

**Exit criteria:** entry present; `CX2OfflineMapData::CheckCOBOExpressTicketMapID` returns a non-zero zone for `VMI_RANOX` once built (verified in Phase 8).

**Start this conversation with:**
> Read `NEW_VILLAGE_PLAN.md` in full, then do Phase 2b for village Ranox: add its Cobo Express entries to `KncWX2Server/ServerResource/US/MapData.lua`, matching the shape already used for the other 8 villages.

## Phase 3 — Dungeon integration

**Goal:** register each of Ranox's 7 dungeon lines — **Gateway, AshesVillage, BurnedForest, FireTemple, FinalHall, Geyser, Secret** — and give each its real stage content.

**Steps (repeat per dungeon line):**
1. Merge the `DungeonData.src.lua` entries already in hand (44 total: Normal/Hard/Expert + Hero-Normal/Hard/Expert/Hell per line, Secret with just Hell/Common) into the tree's `DungeonData.lua` — `DUNGEON_ID` values from Phase 1, `dataFileName`, level/item requirements, boss drop list, ending speech. Already confirmed: every `HERO_*` variant's `dataFileName` reuses an already-in-hand stage script (e.g. `HERO_NORMAL` reuses the `Expert` file's data) — **no additional per-instance stage scripts are needed beyond the 19 already in hand.**
2. Add each of the 19 `Dungeon_Ranox_*_{Normal,Hard,Expert}.src.lua` / `Dungeon_Ranox_Secret_Hell.src.lua` per-instance stage scripts (already in hand) to the game data — each already has `STAGE_LIST`→`STAGE<i>`→`DUNGEON_MAP`→`SUB_STAGE<j>`, `CLEAR_COND`/`NEXT_STAGE`, and `NPC_GROUP`/`READY_NPC` referencing monster IDs from Phase 1. Cross-check every `NPC_UNIT_ID` reference against `NEW_VILLAGE_RANOX_IDENTIFIERS.md`'s 129-entry list. One known anomaly: `NUI_FIRE_DEVIL_IGNIA_BOSS` (bare, no `_FINALHALL` suffix) appears in three `FinalHall` `READY_NPC` lists but has no templet row and no entry in the newer build's own enum dump either — flag it to the user rather than inventing a fix or silently dropping it.
3. `DI_RANOX_SECRET_COMMON` is declared under `UNUSED_DUNGEON_ID` in the source data, not `DUNGEON_ID` — confirm that namespace exists in this tree before merging it as a normal entry (see Phase 1 Step 5).
4. Confirm each dungeon line already appears in `DLG_Map_Local.lua`'s `m_DungeonList` for Ranox (added in Phase 2) — if using an unlock chain between Ranox's own dungeons, populate `m_vecRequireClearDungeonID` too.

**Files touched:** `DungeonData.lua`, each dungeon line's 19 stage scripts (already in hand), (cross-check only) `DLG_Map_Local.lua`.

**Exit criteria:** all 44 `DUNGEON_ID` entries exist in `DungeonData.lua` pointing at real, in-hand files, and all 7 lines are reachable from Ranox's dungeon list.

**Start this conversation with:**
> Read `NEW_VILLAGE_PLAN.md` and `NEW_VILLAGE_RANOX_IDENTIFIERS.md` in full, then do Phase 3 for village Ranox's 7 dungeon lines (Gateway, AshesVillage, BurnedForest, FireTemple, FinalHall, Geyser, Secret — IDs already added in Phase 1, linked from the village in Phase 2). `DungeonData.src.lua` and all 19 per-instance stage scripts are already in the repo at `lanox_new_village_files/`. Merge each dungeon's registration and stage content, double-checking every `NPC_GROUP`/`READY_NPC` monster reference against `NEW_VILLAGE_RANOX_IDENTIFIERS.md`, and confirm whether this tree has an `UNUSED_DUNGEON_ID` enum before placing `DI_RANOX_SECRET_COMMON`.

## Phase 4 — Battlefield integration

**Goal:** register Ranox's 4 fields (`Ranox_Field_01`–`04`) and give each its real monster-spawn content.

**Steps (repeat per field):**
1. Merge the `BattleFieldData.src.lua` entries already in hand into the tree's `BattleFieldData.lua` — `BATTLE_FIELD_ID`/`VMI_BATTLE_FIELD_RANOX_FIELD_0N` (from Phase 1), `WORLD_ID` (from Phase 1), `NPC_TABLE_FILE_NAME`. **Carry the off-by-one explicitly**: `WORLD_ID.WI_BATTLE_FIELD_RANOX_FIELD_00..03` is 0-based, `BATTLE_FIELD_ID`/`VMI_BATTLE_FIELD_RANOX_FIELD_01..04` is 1-based — confirmed by the source data binding `WI_..._FIELD_00` to `VMI_..._FIELD_01` in the same entry. Don't assume a clean 1:1 mapping between the two numbers.
2. Add each field's monster-spawn script (`Ranox_Field_01..04.src.lua`, already in hand) — `AddSpawnMonsterGroup` calls referencing monster IDs from Phase 1.
3. Request and add the 5 field **world/mesh scripts + line-maps** that are still missing: `WI_BATTLE_FIELD_RANOX_FIELD_00..03` plus `WI_BATTLE_FIELD_RANOX_FIELD_REST_00` (a 5th, rest-area map). Only the spawn scripts are in hand — none of the visual maps are.
4. Confirm each field already appears in `DLG_Map_Local.lua`'s `BattleFieldIDList`/`BattleFieldRestData` for Ranox (added in Phase 2).
5. Do **not** edit `BattleFieldServerData.lua` — it's shared global tuning, not per-field data.

**Files touched:** `BattleFieldData.lua`, each field's monster-spawn script (in hand) + world script + line-map (not in hand), (cross-check only) `DLG_Map_Local.lua`.

**Exit criteria:** all 4 fields plus the rest map have `BattleFieldData.lua` entries, spawn scripts, and world maps, and are reachable from Ranox's field list.

**Start this conversation with:**
> Read `NEW_VILLAGE_PLAN.md` and `NEW_VILLAGE_RANOX_IDENTIFIERS.md` in full, then do Phase 4 for village Ranox's 4 fields (IDs already added in Phase 1, linked from the village in Phase 2). `BattleFieldData.src.lua` and all 4 monster-spawn scripts are already in the repo at `lanox_new_village_files/`; the 5 field world/line-map files are not — request them by name (`WI_BATTLE_FIELD_RANOX_FIELD_00..03` + `..._REST_00`). Merge each field's registration and spawn content, watching for the `WI_..._FIELD_00..03` (0-based) vs `VMI_..._FIELD_01..04` (1-based) off-by-one. Confirm `BattleFieldServerData.lua` is left untouched.

## Phase 5 — New monster type integration

**Goal:** give each of Ranox's ~129 new `NPC_UNIT_ID`s real stats and a real mesh/FSM.

**Steps (repeat per monster):**
1. Merge each monster's already-in-hand `AddNPCTemplet` entry from `NPCTemplet.src.lua` into the tree's `NPCTemplet.lua` — `NPC_ID` (from Phase 1), `CLASS_TYPE`, name/desc, and `LUA_FILE_NAME` naming the monster's own gameunit script. (`NPCTemplet_LA.src.lua`, also in hand, is a near-duplicate of the same data; `NPCTemplet_1.src.lua` is the pre-Ranox baseline and isn't needed going forward.)
2. Request and merge `NPCStat.lua` entries — `MAX_HP`, `ATK_PHYSIC`/`ATK_MAGIC`, `DEF_PHYSIC`/`DEF_MAGIC`, elemental defenses, `SCAN_RATE`, `ACCURACY`/`AVOIDANCE`, keyed by the same `NPC_ID`. Not provided at all yet — request the whole file.
3. Request and add each monster's own gameunit script (FSM + mesh/animation attachment) — its `INIT_STATE` FSM and mesh/animation attachment tables (`MESH_PLAYER`, `ATTACH_MESH_NAME`), parsed by `CX2GUNPC::InitState`. The exact filename for each is already known: read it straight off `NPCTemplet.src.lua`'s `LUA_FILE_NAME` field per entry rather than guessing, and request that exact filename list from the user instead of "one gameunit script per monster."
4. Skip `NUI_CHEST` and `NUI_GATE_OF_NIGHTMARE` — already-existing IDs reused as-is, no new templet/stat/gameunit needed for them. Hold `NUI_FIRE_DEVIL_IGNIA_BOSS` (bare) pending the anomaly noted in Phase 3 — it has no templet row and no source data at all; don't invent one.
5. Add the monster's rows to the already-in-tree `KncWX2Server/ServerResource/US/DropTable.lua` (keyed by `(DungeonID/BattleFieldID, MonsterID)`), `StaticDropTable.lua` (keyed by dungeon/field ID alone, if the monster contributes a static/guaranteed drop), and `NpcExpTable.lua` (keyed by NPC level) — these three do not need to be sourced from the user, only edited here.

**Files touched:** `NPCTemplet.lua`, `NPCStat.lua`, each monster's gameunit script, `DropTable.lua`, `StaticDropTable.lua`, `NpcExpTable.lua`.

**Exit criteria:** every monster (aside from the 2 reused IDs and the 1 unresolved anomaly) has a templet entry, a stat entry, a gameunit script, and drop/exp rows.

**Start this conversation with:**
> Read `NEW_VILLAGE_PLAN.md` and `NEW_VILLAGE_RANOX_IDENTIFIERS.md` in full, then do Phase 5 for Ranox's ~129 new monsters (IDs already added in Phase 1, referenced from Phase 3/4 spawn tables; `NPCTemplet.src.lua` already in the repo at `lanox_new_village_files/`). Extract the gameunit script filenames from `NPCTemplet.src.lua`'s `LUA_FILE_NAME` fields and request exactly those, plus the whole `NPCStat.lua` file. Merge each monster's registration, stats, and mesh/FSM script, then add rows to the in-tree `DropTable.lua`/`StaticDropTable.lua`/`NpcExpTable.lua`. Skip `NUI_CHEST`/`NUI_GATE_OF_NIGHTMARE` (already exist) and hold `NUI_FIRE_DEVIL_IGNIA_BOSS` rather than guessing at it.

## Phase 6 — Build

**Goal:** produce a client binary with village Ranox — its dungeons, its fields, and its monsters — all compiled in.

**Steps:**
1. `touch X2Lib/stdafx.cpp` — `Always.h`, `Enum.h`, `DungeonEnum.h`, and `X2UnitManager.h` all sit inside the precompiled header, and msbuild does not reliably notice header-only edits otherwise.
2. Run the standard `US_SERVICE` build order from `CLAUDE.md`'s *Toolchains* section (`luajitLib` → `luaLib` → `X2ServerProtocol` → `KTDXLIB` → `X2Lib` → `X2`, each with `-p:SolutionDir=.../Trunk/` and `-m -p:MultiProcessorCompilation=true`).

**Exit criteria:** `X2/US_SERVICE/x2.exe` is freshly dated and the build produced zero errors.

**Start this conversation with:**
> Read `NEW_VILLAGE_PLAN.md` in full, then do Phase 6: build the client with village Ranox's changes from Phases 1-5(+2b) included, following the `US_SERVICE` build recipe in this repo's `CLAUDE.md`. Confirm `X2/US_SERVICE/x2.exe`'s timestamp updated.

## Phase 7 — Iterate with loose files, then verify

**Goal:** confirm the village, all 7 dungeon lines, all 4 fields, the dungeon lounge, the spa, and all new monsters actually work in-game before committing to a repack.

**Steps:**
1. Deploy `X2/US_SERVICE/x2.exe` to the game directory as `X2_offline.exe` (verify the copy landed by name/size/mtime, don't just assume it).
2. Drop every edited/new file from Phases 2-5 as **loose files** in the game directory — this build loads a loose file before the `.kom` archive, so no packing/XOR step is needed yet.
3. Run `start_offline.bat`, then play through each surface in turn:
   - **Village entry** — via `DLG_Map_Village.lua`'s `START_POS`/`DLG_Map_Local.lua`'s grouping. Check `offline_packets.log` for `HANDLED` `EGS_STATE_CHANGE_FIELD_REQ`/`EGS_FIELD_LOADING_COMPLETE_REQ`.
   - **The dungeon lounge and the spa** — both are new sub-locations this village adds beyond the base 8-village pattern; confirm both actually load.
   - **Each of the 7 dungeon lines** (Gateway, AshesVillage, BurnedForest, FireTemple, FinalHall, Geyser, Secret) — enter via the village's dungeon list, run at least one difficulty of each to a clear. Check for `HANDLED` on the room-open and `EGS_DUNGEON_STAGE_LOAD_*` chain, and that `NPC_GROUP`/`READY_NPC` monsters actually spawn.
   - **Each of the 4 fields** — enter via the village's field access. Check for `HANDLED` `EGS_JOIN_BATTLE_FIELD_REQ` and that `AddSpawnMonsterGroup` monsters spawn and respawn.
   - **The new monsters** — confirm they render with the correct mesh, animate through their FSM, and have sane combat stats (HP drains at a sane rate, deals damage) — this is the item flagged as unverified in *Why this is safe* (no offline-specific loader for `NPCTemplet.lua`/`NPCStat.lua`), so don't skip it.
4. `grep -E "UNHANDLED|EXCEPTION" offline_packets.log` should have no new hits tied to any Ranox ID; `offline_server.log` should show no assert (e.g. the `X2TFieldGame.cpp:3186` NPC-position assert) or missing-file line.

**Exit criteria:** village (including lounge and spa), all 7 dungeon lines, all 4 fields, and all new monsters work, with clean logs.

**Start this conversation with:**
> Read `NEW_VILLAGE_PLAN.md` and `NEW_VILLAGE_RANOX_IDENTIFIERS.md` in full, then do Phase 7 for village Ranox: deploy the Phase 6 build, drop all of Phases 2-5's files as loose files, and play through the village (including the dungeon lounge and spa), all 7 dungeon lines (Gateway, AshesVillage, BurnedForest, FireTemple, FinalHall, Geyser, Secret — each difficulty), all 4 fields, and the new monsters in turn. Read `offline_packets.log`/`offline_server.log` after each and report exactly what they show — including whether monster stats/mesh actually took effect, since that path has no offline-specific loader to fall back on.

## Phase 8 — Pack and ship

**Goal:** move from loose-file testing to the real, packed artifacts.

**Steps:**
1. Confirm which `.kom` archive number houses each file category for the existing 8 villages/dungeons/fields/monsters (not visible from source in this tree — check against how those files are currently packed, or ask the user). There may be more than one archive involved now (location Lua, dungeon/field scripts, and unit/monster Lua may not all live in the same `.kom`).
2. XOR-encrypt and pack every finalized Ranox file from Phases 2-5 into its correct archive (per this repo's standing rule: this is the user's step to run, not something to band-aid around with a permanent loose copy).
3. Delete every loose copy dropped in Phase 7.
4. Re-run Phase 7's full verification checklist against the packed build only — a loose-file success does not prove every pack succeeded, and a miss on any one file surfaces as `KEM_ERROR136` in the logs rather than a build error.

**Exit criteria:** everything from village entry through every dungeon, field, and monster works identically with loose copies removed; no `KEM_ERROR136` lines tied to Ranox's files.

**Start this conversation with:**
> Read `NEW_VILLAGE_PLAN.md` for village Ranox: confirm which `.kom` archive(s) should hold each category of new file, pack them (or confirm the user has packed them — name the exact files and ask them to confirm packing per this repo's `CLAUDE.md` rule if not done yet), delete the Phase 7 loose copies, and re-verify against `offline_packets.log`/`offline_server.log` with the packed build only.

## Risks / open items carried across phases

- **Struct/table drift.** The source client's Lua (location files, dungeon/field data, `NPCTemplet.lua`/`NPCStat.lua`) is from a newer build than this tree's C++ parsers. If any file uses fields this build's parser doesn't recognize, the parse may silently ignore them or error outright — read the relevant parser (`VillageParsing`/`HouseParsing` in `X2LocationManager.cpp:566-568`, `CX2DungeonManager::AddDungeonData_LUA`, `CX2BattleFieldManager::AddBattleFieldData_LUA`, `AddNPCTemplet_LUA`/`AddNPCStat_LUA` in `X2UnitManager.cpp`) once each real file is in hand, rather than assuming compatibility.
- **Monster stat/mesh load path is unverified in offline mode specifically** (no dedicated `X2Offline*` loader exists for it) — Phase 7 makes this an explicit checklist item rather than an assumption.
- **Whether this tree publishes `WORLD_ID` to Lua is still unconfirmed** — Phase 1 Step 1 is the first place this gets checked, and it's load-bearing, not a nice-to-have: every dungeon/field script already in hand references `WORLD_ID.WI_RANOX_...` by symbol.
- **Archive naming/packing convention** for every new file category isn't visible from source in this tree — resolve in Phase 8 only; no earlier phase should block on it, since Phases 2-7 all work against loose files first.
- **Dependency order matters across phases**, since Ranox's ~129 monsters (Phase 5) are referenced by both the 19 dungeon stage scripts (Phase 3) and the 4 field spawn scripts (Phase 4) — do Phase 5 before or alongside Phases 3-4, not strictly after, if a fresh conversation is tackling them out of order; cross-check every `NPC_UNIT_ID` symbol against `NEW_VILLAGE_RANOX_IDENTIFIERS.md` either way.
- **New content units this plan didn't originally anticipate** — the **spa** (`VMI_SPA_RANOX`/`WI_SPA_RANOX`, in scope, needs a world script + house entry) and the **dungeon lounge** (`VMI_DUNGEON_LOUNGE_RANOX`, in scope, needs a world script) are real and required; the **Gateway PvP map** (`WI_RANOX_GATEWAY_PVPMAP`) and the **buff/title/currency system** (`BTI_BUFF_RANOX_SECRET_FEVER`, `BTI_BUFF_RANOX_6TH_DUNGEON_TITLE`, `BTI_BUFF_ELIXIR_RANOX_DUNGEON_56_EVENT`, etc.) are visible in the newer build's enum dump but **out of scope** unless separately requested. The world-map UI dialogs (`DLG_UI_FIELD_MAP_Ranox_*`, `DLG_UI_RANOX_SECRET_FEVER_GAGE`) are in hand but reference `.dds` textures that aren't.
- **`Enum.src.lua`/`EnumString.src.lua` (in `lanox_new_village_files/`) are reference dumps only.** They're a runtime `NAME = number` dump from the newer client (hash-ordered, not authored source) and a debug name-registration file — neither is merged anywhere in this tree (`Enum.h` is C++, not Lua). Their **names** are authoritative for Phase 1's spellings; their **numbers** are advisory only — `VILLAGE_MAP_ID`/`LOCAL_MAP_ID` auto-increment in this tree and won't land on the same values, and `WORLD_ID`/`DUNGEON_ID` numbers still need a collision check against what this tree already uses before reuse.

- Resolved (kept here only as a pointer, not re-litigated per phase): `UnitTemplet.lua`'s role for monster-type NPCs (it has none — player-class file, see Phase 0) and whether dungeons use distinct per-stage world maps (they do — see Phase 1 Step 3).
