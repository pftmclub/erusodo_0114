# June effect completion - C3x review - June's maps for the June dungeon scripts

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3x review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3x done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_WORLD_LOAD_LOG` |
| Log tag | `JUNE_FX C3x` (`python tools/logs.py --grep "JUNE_FX C3x"`; JUNE_FX lines are unbracketed) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3x - port: June's maps for the June dungeon scripts" "C3x - Done"
head -5 june2017_re/june_adopt_20261010_133232.tsv          # the 170 adopted map files
head -5 june2017_re/world_asset_need.tsv                    # the 363 staged world assets
```

## Task

Review C3x against June's data:

1. **Enum.lua** (data036): the four `WORLD_ID` rows equal June's values (`june_src_fixed/data036_kom/Enum.src.lua`), the old
   `WI_ELDER_BENDERS_CAVE_STAGE2_1 = 33` alias is kept, and `CTT_RIM_LIGHT_BLUE = 10` matches `KTDXLIB/KTDGXRenderer.h`
   under `SERV_IRUHADEV_RANOX_RIM_LIGHT` (June's Lua says 15; March's C++ owns the number).
2. **WorldData.lua** (data040): every row for an ID March knows equals June's row (`june_src_fixed/data040_kom/WorldData.src.lua`):
   the 11 renames, Valley of Abyss, world 33, the three Druid Forest rows. The only intended differences from June are March's
   own rows June lacks (`WI_RANOX_SECRET_EXTRA_STAGE1-3`, used by June's `Dungeon_Ranox_Secret_Hell.lua`) and the 157 June
   rows whose IDs March lacks. Re-run the row diff to prove it.
3. **The 170 adopted files** are byte-equal to `ljconv(June)`: `python tools/luasrc.py adopt-june --list <names> --dry-run
   --compare june2017_re/june_adopt_20261010_133232.tsv` should say `COMPARE OK`.
4. **Binding audit:** re-check that every `obj:Method(` in the adopted world scripts is bound under an ON flag (C3x found all
   bound; the Ranox ones under `SERV_IRUHADEV_RANOX_*`, flagprobe ON).
5. **Assets:** spot-check 3 staged meshes inflate (`fxdata.xfile_text`) and their textures resolve in March or the staged set.
6. **C++:** the three `CreateWorld` log paths and the `SetLineMap_LUA` one; the flag-off build is March's code.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_WORLD_LOAD_LOG --range <C3x commit>^..HEAD`. Paste its summary lines;
the two untokened lines are the inner `#endif SERV_IRUHADEV_OFFLINE`.
Write "C3x - Review" into the plan doc, update the STATUS row, and check C3y's kickoff.
