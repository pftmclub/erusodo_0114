# Atlas/Elysion port - Phase E9 review (Adrian line)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E9 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | E9 done (its Done note and commit; the data035 / 037 / 040 copy can come before or after) |
| Flag | none new (data-only phase) |
| Log tag | `[ELYDBG]`; load errors in `ErrorLog.txt` |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context (paste with any phase)" "Phase E9 — Adrian" "Per-line checklist" "Phase E9 — Done"
python -I june2017_re/region_census.py --line AR --limit 0 --no-probe
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review E9 (data only) against June:
- **Registry rows, verbatim:** diff the three `atlas E9` blocks against `june_src_fixed`: `DungeonData` June :32888-33119 (no allowed difference),
  `NPCTemplet` 30 rows (ranges in the block header), `WorldData` :3380-3399. Grep each new `dungeonID` / `NPC_ID` / `worldID` once.
- **Hero omission:** header ranges `HERO_NORMAL :33355-33518`, `HARD :33519-33564`, `EXPERT :33565-33739`, `HELL :38599-38761`.
- **Unlock chain:** `EXTRA_REQUIRE_DUNGEON_ID = DI_SKYCROSSROADS_NORMAL` (E8 clear key 33870); Soless names `DI_ADRIAN_NORMAL`.
- **Adopted files:** the 8 data040 files equal `ljconv(June)` (`june2017_re/june_adopt_20261017_145520.tsv`), manifest `"2"`.
- **Premises:** NPC readiness of the 30 rows, boss faces (`NUI_HERBAON`, `NUI_NEW_DEKAL`), buffs, effect closure; spot-check 2-3 of each.
- **Engine hooks the line leans on:** moving platforms (E5), `SetCameraDistance` / `SetParticleSize` (A4 / E7), laser objects. The Stage1 line map's
  `MOTIONNAME_EFFECTSETNAME` trigger is inert until june-engine W1.
- **Routed, not open:** 13 `assets.absent`, `keys.file` CT_ / state names, `AFTER_IMAGE_DEST_ALPHA_BLEND`, `MOTIONNAME_EFFECTSETNAME`, `keys.registry`.

Hygiene: `luasrc.py ledger --check` and `core --check`; if the user has copied, `region_census.py --line AR --live`.
Fix clear transcription errors (repack, ask before copy); hand design decisions back. Write "Phase E9 — Review" into the plan doc, update the E9
STATUS row, write `kickoffs/atlas/E10.md` from the template (Opus 5.5 high; read the Soless boss from `june2017_re/june_lua/ELYSION_SOLESS.src.lua`), commit.
