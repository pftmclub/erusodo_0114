# Atlas/Elysion port - Phase A9 review (AtlasCity line)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas A9 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | A9 done (its Done note and commit; the data035 / 037 / 040 copy can come before or after) |
| Flag | none new (data-only phase; `SERV_IRUHADEV_ELYSION_PORT` names the Lua blocks) |
| Log tag | `[ELYDBG]`; load errors in `ErrorLog.txt` (`python tools/logs.py --grep "DungeonData\|NPCTemplet\|WorldData"`) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context (paste with any phase)" "Phase A9 — AtlasCity line" "Per-line checklist" "Phase A9 — Done"
python -I june2017_re/region_census.py --line AC --limit 0 --no-probe
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review A9 (data only) against June. Things A9 could get wrong:

- **Registry rows, verbatim:** diff the three appended blocks against `june_src_fixed`:
  - `data035_kom/DungeonData.lua` `atlas A9` block against June data035 :30516-30788 (no allowed difference; June has no quest key on AC);
  - `data037_kom/NPCTemplet.lua` 10 rows against the ranges in the block header (June data037);
  - `data040_kom/WorldData.lua` against June :3225-3244.
  Check no AC row existed elsewhere in a file (duplicate `dungeonID` / `NPC_ID` / `worldID`).
- **Hero omission:** the four Hero ranges named in the block header (`HERO_NORMAL :30789-30939`, `HARD :30940-30961`,
  `EXPERT :30962-31119`, `HELL :38136-38285`) are right and still C3y-5's.
- **Unlock chain:** AC rows' `EXTRA_REQUIRE_DUNGEON_ID = DI_ATLASDEVELOPMENT_NORMAL` against the AD clear key (`unit_dungeon` 33330);
  `IsActiveDungeon` (`X2DungeonManager.cpp:750`) lets level 81+ in. AC's icon on the local map page "0" turns live once the row exists.
- **Adopted files:** the eight data040 files equal `ljconv(June)` (sha1 in the A9 `june2017_re/june_adopt_*.tsv`), manifest `"2"`.
- **Premises:** NPC readiness of the 10 rows (Enum / NPCTemplet / NewNPCStat / Motion meshes incl. `Motion_EVENT_ELSION_2ST_BOSS.x`),
  boss-face pieces `NUI_ELSION_SUVEILLAN` and `NUI_ELSION_NEWDEFENDER` in `DLG_BossState_02.TET`, the buff census and the effect closure.
  Spot-check 2-3 of each.
- **Routed, not open:** 6 `assets.absent` (June ships none), `keys.file` CT_/CF_ names, `keys.registry` (checklist step 9).

Hygiene: `luasrc.py ledger --check` and `core --check`; census `--line AC` shows only the routed classes. If the user has copied, check
the live koms (`region_census.py --line AC --live`).

Fix clear transcription errors (repack, ask before copy); hand design decisions back to the user. Write "Phase A9 — Review" into the plan
doc, update the A9 STATUS row, check `kickoffs/atlas/A10.md`, and commit.
