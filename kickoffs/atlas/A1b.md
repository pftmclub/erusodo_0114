# Atlas/Elysion port - Phase A1b - Script-engine map (read-only)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, xhigh** effort, then type `/x2-phase atlas A1b` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | A0 (done). Can run at the same time as A1a |
| Flag | none (read-only: Ghidra, greps, TSV) |
| Log tag | none |
| Review | none (A4, E5, E6, E7 read the map) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "June-only engine the region needs" "Elysion half: engine gaps" "Phase A1b"
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Model selection"
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Phase A1b's steps 1-4: for each unbound Lua name, every unread unit-script / linemap / world key and the new enum tables, record June address, parse layout, the store it writes, every read site, the closest March analog, the owning phase and **transcription-ready** or **Opus-only**, in `june2017_re/region_engine.tsv` plus an "A1b engine map" table in the plan doc. Verify the **receiver class** for each name (Lanox's `SetShowObject` landed on the wrong class). Check `TAT_LINE_CHANGE_MOVABLE_DATA` against March's `SERV_IRUHADEV_JUNE_LINEMAP_TRIGGER` numbering. List `ELYSION_SOLESS`'s extra names for E0.

**Premises to verify first:** the ledger's "(b)" claims (name match only so far).

**The list to work from is the census, not the ledger's 65 (A0, 2026-10-16):** `python june2017_re/region_census.py --phase E7 --limit 0` and `--line AD`, rows `bind.*` and `keys.*`. 31 names are still unbound (about 30 were bound by june-effect C3z-4); the unit keys March reads now (`BUFF_GROUP_ID`, `SOUND_PLAY_*`, `IMMUNITY_DAMAGE_DATA_STOP_TIME`, `SUPER_ARMOR_FRONT`) drop out. `members.missing` lists `LINE_MOVE_TYPE.*` / `DRAMATIC_MESSAGE_TYPE.*`.

**Done when:** every name and key has an owner and a readiness verdict, the map is in the plan doc, a Done note is written (under ~60 lines). Commit on `main` with explicit pathspecs; update the A1b row in `STATUS.md`.
