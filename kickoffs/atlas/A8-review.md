# Atlas/Elysion port - Phase A8 review (AtlasDevelopment line)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas A8 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | A8 done (its Done note and commit; the data035 / 037 / 040 copy can come before or after) |
| Flag | none new (data-only phase; `SERV_IRUHADEV_ELYSION_PORT` names the Lua blocks) |
| Log tag | `[ELYDBG]`; load errors in `ErrorLog.txt` (`python tools/logs.py --grep "DungeonData\|NPCTemplet\|WorldData"`) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Phase A8 — AtlasDevelopment line (pattern-setter)" "Per-line checklist" "Phase A8 — Done"
python -I june2017_re/region_census.py --line AD --limit 0 --no-probe
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review A8 (data only) against June. The things A8 could get wrong:

- **Registry rows, verbatim:** diff the three appended blocks against `june_src_fixed`:
  - `kom_contents/data035_kom/DungeonData.lua` against June data035 :29932-30178; the only allowed difference is the commented quest key;
  - `data037_kom/NPCTemplet.lua` 11 rows against the ranges in the block header;
  - `data040_kom/WorldData.lua` against June :3205-3219.
  Check that no AD row was already present elsewhere in a file (no duplicate `dungeonID` / `NPC_ID` / `worldID`).
- **Quest key inert:** is commenting `m_iRequireCompleteQuestID` the right reading of decision 2? (`IsActiveDungeon`
  `X2DungeonManager.cpp:750` reads it.) Hero omission still true (C3y-5).
- **Adopted files:** the six data040 files equal `ljconv(June)` (sha1 in `june2017_re/june_adopt_20261017_071905.tsv`), manifest `"2"`.
- **Premises:** NPC readiness (Enum / NPCTemplet / NewNPCStat / Motion meshes), boss-face pieces in `DLG_BossState_02.TET`, the buff
  census (2 names), and the effect closure (35 sets, 11 damage effects, 80 particles, 0 runtime-built names). Spot-check 2-3 of each.
- **Offline claims:** clear recorded with no per-id code, AtlasCity unlock via `EXTRA_REQUIRE_DUNGEON_ID`, kills pay nothing (no
  `DropTable` rows): routed to backlog Stage 23. Check that the Stage 23 brief (`kickoffs/june-backlog/stage23-plan.md`) names dungeons too;
  if not, add a line.
- **Checklist:** is "Per-line checklist" complete enough for A9 (AtlasCity: `ATLAS_GATE`, `SUVEILLAN` boss) and E8-E11 to follow?

Hygiene: `python tools/luasrc.py ledger --check` and `core --check`; census `--line AD` shows only the routed classes. If the user has
copied, check the live koms (`region_census.py --line AD --live`).

Fix clear transcription errors (repack, ask before copy); hand design decisions back to the user. Write "Phase A8 — Review" into the
plan doc, update the A8 STATUS row, write `kickoffs/atlas/A9.md` from the plan's Phase A9 section if missing, and commit.
