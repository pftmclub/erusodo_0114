# Atlas/Elysion port - Phase A11 review (asset closure + TET census)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas A11 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | A11 done (its Done note, the commit, and the data057 / 151 / 215 copy, already made) |
| Flag | none (data-only phase) |
| Log tag | `[ELYDBG]`; load misses in `ErrorLog.txt` (`python tools/logs.py --grep "OpenTexture\|TET\|XEM_ERROR"`) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context (paste with any phase)" "Phase A11 — Asset closure + consolidated pack list (Atlas half)" "Phase A11 — Done"
python -I june2017_re/region_census.py --region atlas --limit 0 --no-probe --live
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review A11 against June. Things it could get wrong:

- **The 28 TETs:** each is June's exact bytes (`kom.py stage-june` dry-run lines in the Done note), new names, manifest `"0"`; the live data057 / 151 / 215 contain them
  (`elstools inspect`), `COPY OK` md5s match. Spot-check 3 by hand: keys in the TET equal the keys the staged Lua / rows use (`june2017_re/atlas_a11_tet.tsv`).
- **The census method:** `atlas_a11_closure.py` reads pairs from `SetTex` family, `IconAni`, and four Name/Key field pairs. Look for another way the region names an atlas key
  that it misses (e.g. `ChangeTexForce_LUA`, `NPCAniTex`, `MOTION_*_TEX_XET`, quest-icon keys) and, if one exists, run it.
- **Closure completeness:** roots are `ASSET_RE` literals only. Check one boss line (e.g. a Maya / Highttem texture reached through a table of names) and one dungeon world
  (`WI_TOWER_STAGE0`) end to end: mesh -> XET -> texture all in the live koms.
- **June defects #214 / #215:** June's `DLG_Localmap_Title_01_A` and Sphere keys really are missing in June's own kom (not a decode problem).
- **The 27 size differences:** confirm they are March originals, not stage-june residue (`kom_backup_stage_june` holds nothing for them).
- **Server Lua:** `StatTable` 4,950 rows and `NpcExpTable` are live in data036; the server `MapData.lua` mirror is not a client item.

Hygiene: census `--region atlas --live` shows only the routed classes and `live {"same": 287}`. Write "Phase A11 -- Review" into the plan doc, update the STATUS row, check
`kickoffs/atlas/A12.md`, commit.
