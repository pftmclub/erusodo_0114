# Atlas/Elysion port - Phase A10 review (Tower line)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas A10 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | A10 done (its Done note and commit; the data035 / 037 / 040 copy can come before or after) |
| Flag | none new (data-only phase; `SERV_IRUHADEV_ELYSION_PORT` names the Lua blocks) |
| Log tag | `[ELYDBG]`; load errors in `ErrorLog.txt` (`python tools/logs.py --grep "DungeonData\|NPCTemplet\|WorldData"`) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context (paste with any phase)" "Phase A10 — Tower line" "Per-line checklist" "Phase A10 — Done"
python -I june2017_re/region_census.py --line TW --limit 0 --no-probe
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review A10 (data only) against June. Things A10 could get wrong:

- **Registry rows, verbatim:** diff the three appended blocks against `june_src_fixed`:
  - `data035_kom/DungeonData.lua` `atlas A10` block against June data035 :31120-31362 (no allowed difference; June has no quest key on Tower);
  - `data037_kom/NPCTemplet.lua` 9 rows against the ranges in the block header (June data037);
  - `data040_kom/WorldData.lua` against June :3245-3264.
  Check no Tower row existed elsewhere in a file (duplicate `dungeonID` / `NPC_ID` / `worldID`).
- **Hero omission:** the four Hero ranges named in the block header (`HERO_NORMAL :31363-31513`, `HARD :31514-31535`,
  `EXPERT :31536-31724`, `HELL :38286-38435`) are right and still C3y-5's.
- **Unlock chain:** Tower rows' `EXTRA_REQUIRE_DUNGEON_ID = DI_ATLASCITY_NORMAL` against the AC clear key (`unit_dungeon` 33340);
  `IsActiveDungeon` (`X2DungeonManager.cpp:750`) lets level 80+ in. Tower's icon on the local map turns live once the row exists.
- **Adopted files:** the seven data040 files (Stage0-2, 3 line maps, camera) equal `ljconv(June)` (sha1 in `june2017_re/june_adopt_20261017_075106.tsv`), manifest `"2"`.
- **June bug:** `WI_TOWER_STAGE3` -> missing `Elysion_Tower_Stage3.lua` transcribed as June has it (JUNE_BUGS #209); confirm no Tower dungeon script names it.
- **Premises:** NPC readiness of the 9 rows (Enum / NPCTemplet / NewNPCStat / Motion meshes), boss-face pieces `NUI_ELSION_MAYA` and
  `NUI_ELSION_HIGHTTEM` in `DLG_BossState_02.TET`, the buff census and the effect closure. Spot-check 2-3 of each.
- **Routed, not open:** 6 `assets.absent` + `Elysion_Tower_Stage3.lua` (June ships none), `keys.file` CT_/CF_ names, `keys.registry` (checklist step 9).

Hygiene: `luasrc.py ledger --check` and `core --check`; census `--line TW` shows only the routed classes. If the user has copied, check
the live koms (`region_census.py --line TW --live`).

Fix clear transcription errors (repack, ask before copy); hand design decisions back to the user. Write "Phase A10 — Review" into the plan
doc, update the A10 STATUS row, check `kickoffs/atlas/A11.md` (write it from the template if missing), and commit.
