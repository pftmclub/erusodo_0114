# June dungeons - Phase W3f - March's route on June's villages

> User decision (2026-11-12, after the W3c play-test): keep June's villages, NPC placement and worlds, but walk March's route with March's sign
> boards, so no field is cut off. W3c ported June's route, which strands Elder field 2 + Elder rest, Besma fields 1-3, Peita field 1, Altera
> fields 1-3 and the Altera rest.

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-dungeons W3f` |
| Plan doc | `JUNE_DUNGEONS_PHASES.md` ("Phase W3f", "W3c - Done") (branch `main`) |
| Depends on | W3c (deployed). One at a time with any session packing `data027`-`data031`, `data034`, `data036` or `data040` |
| Flag | none for data (the W3c fix `SERV_IRUHADEV_JUNE_FIELD_CHANGE_ACK` stays on) |
| Log tag | `[JUNE_ENGINE] WVIL` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_DUNGEONS_PHASES.md "Phase W3f - March's route on June's villages (user 2026-11-12, after W3c)" "W3c - Done (2026-11-12, Opus 5.5)"
```

## Task

The decision is taken; do not ask it again. It overrides the same-name rule only for the route's boards and line maps listed below.
1. **Route data back to March.** data034 `DLG_Map_Village.lua`: every row with a `-- june-dungeons W3c` comment gets March's target and line back
   (the comment holds them) and the comment goes; restore Altera StartPosId 67 (36174, 409, -193 -> `VMI_BATTLE_FIELD_ALTERA_FIELD_03` line 2).
   data036 `BattleFieldData.lua`: Elder field 1 portal 2 back to `POSITION_INDEX_TO_MOVE = 300`, `VMI_BATTLE_FIELD_ELDER_REST_00`. `luasrc.py refresh`
   each; `ledger --check` and `core --check` PASS.
2. **Boards.** For each board along the route (village ends, rest-area ends, Besma rest navigation sign), find which mesh / texture carries the
   text and whether W3 / W3b replaced it with June's: start from `Besma_Village_Alpha_Text.Y`, `Field_Peita_Villeage_sign.Y`,
   `Field_Elder_StageRest_alpaguide.Y`, `Field_Besma_Rest_G01_Navi.Y`, `Altera_Village_Typo.Y` (render the textures, as W3c did with
   `Field_Elder_Type1.dds`). Put March's copy back from `kom_backup_stage_june/20261112_*` (byte-exact) where the June one names June's route; leave
   the rest June's. A board whose June mesh is fine but whose texture changed: restore the texture only.
3. **Altera bottom walkway.** Diff March's `LINEMAP_Altera_Village.lua` (backup `20261112_083532/data040_kom`) with the live (June) one. If it only
   adds the walkway lines (x 34583 -> 36264, y 480 -> 409) and the rest matches June's world, restore March's; if June moved other lines, stop and
   report the diff (the user decides).
4. Re-run `w3b_village_census.py` (update its `GRAPH` note: user hybrid, March's route) and the closure: 0 unexplained.
5. Pack + copy the touched koms. Record each restored March file in the Done note and `MODS.md`.

Keep: the W3c field-change ACK fix, the Lanox rest `AddMapData` row, the staged Altera field 1 / Peita field 2 assets, Ruben field 1 as March's.
W3e (June's field worlds) gains weight once the fields are reachable again; note that in its kickoff.

## Play-test hand-off

Walk March's route both ways: Ruben - Ruben field 1 - Elder - Elder field 1 - Elder rest - Elder field 2 - Besma - Besma field 1 - Besma rest -
Besma fields 2 / 3 - Altera / Peita field 1 - Peita, and Altera - Altera field 1 - Altera rest - Altera fields 2 / 3, incl. Altera's bottom walkway
portal. Every board names where its portal goes.
