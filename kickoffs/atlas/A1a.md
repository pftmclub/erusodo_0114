# Atlas/Elysion port - Phase A1a - RE + C++ reachability map (read-only)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, xhigh** effort, then type `/x2-phase atlas A1a` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | A0 (done). Can run at the same time as A1b |
| Flag | none (read-only: Ghidra, greps, TSV) |
| Log tag | none |
| Review | none (the reachability table is read by A3 and A5-A7) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "C++ sites that enumerate regions" "March C++ end markers that Elysion IDs collide with" "June-only engine the region needs" "Phase A1a"
python tools/docsection.py ATLAS_PORT_PHASES.md "Elysion half: local map (Elysion page)" "Elysion half: village"
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Phase A1a's steps 1-6 as the plan words them: build `june2017_re/region_reach.tsv` (every March C++ site that enumerates regions or sits on a `*_END` marker, with its June counterpart in Ghidra, live/dead from the probed flag cache, a port-whole / omit-with-comment verdict and the owning phase). Also map the E-phase engine sites (local-map scroll / guide / page filter / event flags, `AddVillageNpcRelationDataBy*` + `VRT_CHANGE`, the new `DLG_Map_House` keys, in-village warps) and settle: is `CX2FieldMiniMap` live; June's `IsVillageField` / `IsDungeonGate` / `IsDungeonLounge` ranges; which `*_END` consumers size arrays.

**Premises to verify first:** every `file:line` in the ledger's touch-list table (re-read the cited line). Resolve `#ifdef`s with `june2017_re/flagprobe.py` / the cache in `june2017_re/region_census_flags.tsv`, never by reading nested conditionals.

**Start point:** `python june2017_re/region_census.py --phase A3` prints the end-marker hits (9 missing IDs sit at or above a marker) and the 36 ID lists with nil holes.

**Done when:** `region_reach.tsv` covers every grep hit, `funcmap.tsv` has the confirmed June functions, the "A1a reachability summary" is in the plan doc, and a Done note is written (under ~60 lines). Commit on `main` with explicit pathspecs; update the A1a row in `STATUS.md`.
