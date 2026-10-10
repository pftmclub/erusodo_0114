# June effect completion - C3x - June's maps for the June dungeon scripts (missing worlds + 137 reworked maps) (port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3x` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | none (found by the C3s play-tests, 2026-10-10) |
| Flag | a `SERV_IRUHADEV_JUNE_*` flag only if C++ has to change (expected: step 5 only) |
| Log tag | `JUNE_FX C3x` |
| Review | `kickoffs/june-effect/C3x-review.md` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3x - port: June's maps for the June dungeon scripts"
python tools/docsection.py LUA_SOURCE_PHASES.md --list     # then only the core-list and adopt-june rule sections
head -40 june2017_re/world_census.tsv                      # the census this phase works from
```

## The problem (user, 2026-10-10: port June's maps, do not revert the dungeon scripts)

The adopted June dungeon scripts (ljconv'd June bytecode, 3,518 files) were written for June's maps, but the maps under them are
still March's. `python june2017_re/world_census.py` (writes `june2017_re/world_census.tsv`) compares every world the dungeon scripts
use, as the world script plus each `.lua` it names (its line map). June's bytecode is ljconv'd first, so `same` means byte-identical.

- **374 worlds**: 235 identical.
- **137 reworked by June**: 126 line maps and 61 world scripts differ, so **162 unique files** to bring in; 6 line maps exist only in
  June. **142 dungeon scripts** sit on them (Altera, Besma, Elder, ...). Their monsters, start points and triggers use June's line /
  position indices on March's layout, so the layout looks wrong.
  - Example: Wally's Castle Roof. Stage 1 line map is March 6,188 B vs June 4,885 B; stage 2 is 20,882 vs 23,755 B; stage 4 is the same.
- **11 world IDs point to a renamed file in June**, for example `WI_ALTERA_CORE_STAGE1`: March `Altera_core_S1.lua`, June
  `Altera_Core_Stage1.lua`. The full list is the `->` rows of the TSV. Their `WorldData` rows must switch to June's file names.
- **4 worlds are missing from March entirely**, and entering them crashes. CreateWorld returns NULL with no log line, then
  `CX2DungeonGame::StageLoading` reads `m_pWorld->GetLineMap()` at `X2DungeonGame.cpp:895`.
  - `WI_ELDER_DARK_FOREST_STAGE_0 / _1 / _BOSS_STAGE` (June Enum 575 / 576 / 577): Elder Druid Forest, all three difficulties.
  - `WI_ELDER_BENDERS_STAGE_BOSS` (June 33): the Benders Cave boss stage.

## Task

1. **Missing IDs.** Add the four `WORLD_ID` names to `data036 Enum.lua` (core list, BOM source: `/x2-lua`).
   - Check every June value against March's `WORLD_ID` table first: 33 and 575-577 may already be March worlds.
   - Keep June's value when it is free; on a collision, hand the choice back to the user.
   - Check whether C++ needs the names: `X2World.h` `WORLD_ID`, any `WI_END` bound, world-ID switches.
2. **WorldData** (`data040 WorldData.lua`, BOM source, core list).
   - Add June's rows for the 4 missing worlds, from `june_src_fixed/data040_kom/WorldData.src.lua`.
   - Switch the 11 renamed rows to June's file names. Keep the March files in their koms; nothing else names them.
   - Diff the rest of June's `WorldData` against March's for any other row change the census does not cover.
3. **Map scripts and line maps.** Run `luasrc.py adopt-june` (June's ljconv bytecode, `/x2-adopt-june`) for every DIFF / june-only
   file in the TSV, the 162, plus the 4 missing worlds' scripts and line maps.
   - Before shipping, check each adopted world script for engine calls March lacks; an unknown binding aborts the rest of the chunk.
   - Every such call is a finding: port it, or route it to a phase. Never edit June's file to dodge it.
4. **Assets.** Census what the adopted world scripts name (world meshes, textures, sky, particles, sounds) that March lacks.
   `npc_mesh_census.py` has the shape: add a world mode or write the same pass for world scripts. Stage the result with
   `kom.py stage-june`.
   - Check every target kom's `kom_contents` dir against the live archive before packing; a missing live file is dropped by a repack.
   - Pack, then **ask before every `copy`**.
5. **Make the failure loud.** CreateWorld returning NULL into StageLoading is a crash with no log.
   - Log the failed world ID and file name (`JUNE_FX C3x`).
   - Ask the user whether StageLoading should bail out cleanly (own flag).
6. **Not here.** List these in the Done note with their owners:
   - Henir Space's 5 missing boss-stage worlds: `WI_ADRIAN_BOSSSTAGE` and `WI_SKYCROSSROADS_BOSSSTAGE` -> elysion;
     `WI_ATLASDEVELOPMENT_BOSSATGE` and `WI_TOWER_BOSSATGE` -> atlas; `WI_SANDER_SECRET_BOSS_STAGE` -> find its owner.
   - The 66 June-only dungeons no registry reaches.

## Play-test (positive)

Name the character and job from `els_db.sql` for each check, with the expected result and the failure look.
- Elder Druid Forest Normal: stages 0, 1 and boss load June's maps; no crash.
- Wally's Castle Roof Normal: stages 1-2 on June's layout, monsters on their platforms.
- One Altera dungeon whose world was renamed (Altera Core or Battle Air Ship).
- The Benders Cave boss stage.
- 0 EXCEPTION, and no new `OpenXSkinMesh` / `OpenTexture` failed lines for the new maps.
