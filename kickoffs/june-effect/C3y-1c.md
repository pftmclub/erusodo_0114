# June effect completion - C3y-1c - the Sander secret dungeon (port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3y-1c` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3y-1b, C3y-1a; one at a time with the port halves of C3y-2 / 3 / 4 (same core registry files) |
| Flag | a `SERV_IRUHADEV_JUNE_*` flag only if C++ must change |
| Log tag | `JUNE_FX C3y-1c` (`python tools/logs.py --grep "JUNE_FX C3y-1c"`) |
| Review | `kickoffs/june-effect/C3y-1c-review.md` if the phase ships code |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3y-1c - port: the Sander secret dungeon"
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3y - Done"
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z-2 - Done (2026-10-10, Opus 5.5 high; no C++, no flag, no build; data036 `Enum.lua` + data037 `NPCTemplet.lua` packed and copied)"
grep Sander_Secret june2017_re/c3y_unreached_dungeon_census.tsv
```

## Task

1. **DungeonEnum** (data036, core): `DI_SANDER_SECRET_HELL` = 41141 (`UNUSED:DI_SANDER_SECRET_COMMON` 41140 is already there).
2. **DungeonData** (data035, core): June's 2 rows verbatim; registry census + strict load test 0 fail(s).
3. **NPCs** (C3z-2's method): the 32 ids in the census row into `Enum.lua` `NPC_UNIT_ID` (June values, collision-checked) and June's
   templet rows into data037 `NPCTemplet.lua`. Unit scripts are already staged; meshes by `npc_mesh_census.py`; stat tables
   (NPCStat / ExtraNPCStat) as C3z-2 found them. `dungeon_npc_parity.tsv` for this script: PASS.
4. **Map button:** June's `DLG_Map_Local_Sander_Icon_NEW.lua` (June data217) has `SanderSecret`, `_Desc` and `TitleBG`, which March's
   data034 copy (with a `.src.lua` sidecar) omits. Default: June's own file by `luasrc.py adopt-june`, as C3y-1a did for Hamel; editing
   March's copy needs the user's OK first. Art: `SANDER_SECRET_DUNGEON_NORMAL/_OVER`
   (`DLG_Common_Button04_NEW.tga`), `TITLE_SANDER_SECRET` (`DLG_Localmap_Title_01`, June data151), `HQ_LoadingDungeon_Sander_SECRET1/2.tga`,
   `DLG_UI_Dungeon_Image10.dds`. Entry items 700015 / 113277 in item.kim.
   C3y-1a (2026-10-11) already staged June's `DLG_Localmap_Title_01.tga` / `.TET` (data063, 77 keys, `TITLE_SANDER_SECRET` included),
   and the strict test takes `allow=TABLE.MEMBER` for a June name kept verbatim (see "C3y-1a - Done").
5. **Emulator:** C3z-6's secret path (relative level, `DT_SECRET`, poison) must not key on a fixed id list; check, and fix behind a
   flag if it does.
6. **Gate:** the Sander map shows the button; a Sander Secret Hell room starts, and all stages load with monsters; 0 EXCEPTION.
