# June dungeons - Phase W3e - June's field worlds and field rows

> Routed by W3c (2026-11-12). W3c made the field graph June's and staged the route fields' assets that leave the ground in place. What is left
> changes a field's ground or its BattleFieldData columns other than the portals.

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-dungeons W3e` |
| Plan doc | `JUNE_DUNGEONS_PHASES.md` ("Phase W3e", "W3c - Done") (branch `main`) |
| Depends on | W3c (play-test). One at a time with any session packing `data036` or `data040` |
| Flag | none for data; a C++ gap (a June reader March lacks) gets its own `SERV_IRUHADEV_JUNE_*` flag |
| Log tag | `[JUNE_ENGINE] WFLD` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_DUNGEONS_PHASES.md "Phase W3e - June's field worlds and field rows (routed by W3c, 2026-11-12)" "W3c - Done (2026-11-12, Opus 5.5)"
```

Then `june2017_re/w3c_field_closure.tsv` (rows not `same`; re-run `python june2017_re/w3c_field_closure.py` first, it reads the live tree).

## Task

Identify, then port what is safe:
- **Ruben field 1 rebuild.** June's `Field_Ruben_Stage0_Linemap.lua` has 53 lines (March 116), 10 meshes differ, BattleFieldData moves the Elder
  portal to line 51 and drops the cap to 10 monsters. March's server spawn table `Ruben_Field_00.lua` (data036, server Lua) names petrol / play lines
  up to 109. Ask the user once whether they have June's server `Ruben_Field_00.lua`; without it Ruben field 1 stays March's whole (never re-map the
  lines by hand, never invent a table).
- **Line maps June changes:** Velder field 1, Sander field 4, Lanox fields 1-4, and the Sander field 3 sign `field_sander_stage2_Guidealpa.Y`. For each,
  diff line counts and portal lines against BattleFieldData and the field's spawn table before staging (`kom.py stage-june` / `luasrc.py adopt-june`).
  A line map that only adds June's boss-raid portal lines goes with C3z-8.
- **BattleFieldData columns that are not the graph** (data036, core-list source): `REQUIRE_LEVEL` on 26 rows (find June's reader; March has none),
  Elder fields 1 / 2 `BATTLE_FIELD_MIDDLE_BOSS_INFO` + `MIDDLE_BOSS_BONUS_ITEM_COUNT` (June has neither; `X2OfflineBattleField.cpp:608` reads them),
  `MAX_JOIN_USER_COUNT` (Ruben 1, Elder 1), the Velder ship's `REQUIRE_DUNGEON` (June drops it). Diff with the scratch approach W3c used
  (luatable over `__calls__`).
- Boss-raid parts stay with C3z-8: Hamel 1-4 / Sander 1-4 third portals, `RAID_FIELD_PORTAL_POSITION_INDEX` (8 rows), the Nasod raid row
  (`BATTLE_FIELD_TYPE`, NPC table), June-only raid rows, `raid_ssky.dds`.
