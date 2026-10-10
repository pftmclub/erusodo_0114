# June effect completion - C3x - June's reworked dungeon worlds (Druid Forest, Benders Cave boss) (port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3x` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | none (found by the C3s play-test, 2026-10-10) |
| Flag | a `SERV_IRUHADEV_JUNE_*` flag only if C++ has to change (it may be Lua + assets only) |
| Log tag | `JUNE_FX C3x` |
| Review | `kickoffs/june-effect/C3x-review.md` if code or Lua lands |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3x - port: June's reworked dungeon worlds"
python tools/docsection.py LUA_SOURCE_PHASES.md --list     # then the core-list / adopt-june rule sections only
```

## Task

**The crash (2026-10-10).** Entering Elder Druid Forest crashes in `CX2DungeonGame::StageLoading` (`X2DungeonGame.cpp:895`,
`m_pWorld->GetLineMap()` with `m_pWorld` NULL). The adopted June dungeon scripts (`data035` `Dungeon_Elder_Druid_Forest_{Normal,Hard,Expert}.lua`)
name June's reworked worlds `WI_ELDER_DARK_FOREST_STAGE_0` / `_STAGE_1` / `_BOSS_STAGE` (June Enum 575 / 576 / 577). March's
`data036 Enum.lua` and `data040 WorldData.lua` have none of them, so `WORLD_ID.X` is nil and `CX2WorldManager::CreateWorld` returns NULL
with no log line. The same gap hits `Dungeon_Elder_Benders_Cave_*` (boss stage `WI_ELDER_BENDERS_STAGE_BOSS`, June value **33**).

Port June's worlds so these dungeons load June's maps (user, 2026-10-10: June's maps, not a March-script revert):

1. **IDs.** Add the four `WORLD_ID` names to `data036 Enum.lua` (core list, BOM source: `/x2-lua`). Check every June value against
   March's `WORLD_ID` table first: 33 and 575-577 may already be March worlds. Keep June's value when free; on a collision, hand the
   choice back to the user (renumbering changes what `WorldData` maps). Check whether C++ needs the names (`X2World.h` `WORLD_ID`,
   any `WI_END` bound, world-ID switches); a C++ change gets its own flag.
2. **WorldData.** Add June's `AddWorldData` rows (`june_src_fixed/data040_kom/WorldData.src.lua`: `Elder_Darkforest_Stage0.lua`,
   `Elder_Darkforest_Stage1.lua`, the boss stage, the Benders boss stage) to `data040 WorldData.lua` (BOM source, core list).
3. **World scripts and assets.** Adopt the world `.lua` files (`luasrc.py adopt-june`, June's ljconv bytecode), then census what
   they name (line maps, world meshes / textures, sky, particles) and stage it with `kom.py stage-june`. `npc_mesh_census.py` covers
   NPC scripts only: extend it or write a world-asset pass in the same shape.
4. **Make the failure loud.** `CreateWorld` returning NULL into `StageLoading` is a crash with no log. Log the failed world ID and name
   (`JUNE_FX C3x`) and decide with the user whether `StageLoading` should bail out instead of crashing (own flag).
5. **Not here:** Henir Space's 5 missing boss-stage worlds (`WI_ADRIAN_BOSSSTAGE`, `WI_SKYCROSSROADS_BOSSSTAGE` -> elysion;
   `WI_ATLASDEVELOPMENT_BOSSATGE`, `WI_TOWER_BOSSATGE` -> atlas; `WI_SANDER_SECRET_BOSS_STAGE`: find its owner) and the 66
   unregistered June-only dungeons. List them in the Done note with their owners.

Play-test: positive. Elder Druid Forest Normal (stages 0, 1, boss) and Benders Cave boss stage load June's maps.
Pack, then **ask before every `copy`**.
