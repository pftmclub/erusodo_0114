# Atlas/Elysion port - Phase E1d - In-village warps (InternalPlaceStartPosID / WaitingTime)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E1d` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | E1a (done); independent of E1b / E1c (different functions), but not in parallel with a session editing `X2LocationManager.*` |
| Flag | `SERV_IRUHADEV_ELYSION_VILLAGE` (+ `[ELYDBG]` lines behind `SERV_IRUHADEV_ELYSION_DEBUG`) |
| Log tag | `[ELYDBG]` (`python tools/logs.py --tag ELYDBG`) |
| Review | `kickoffs/atlas/E1d-review.md` (Opus 5.5; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Phase E1 — Village engine" "Phase E1 — Split (user, 2026-10-17)" "A1a reachability summary" "Phase A5 — Done"
grep -a "324ff4b0\|323aa170\|323ac300\|3250a800" june2017_re/funcmap.tsv
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Phase E1 step 3, whole: June's `VillageParsing` (`324ff4b0`) keys `InternalPlaceStartPosID` and `WaitingTime` (and any other
key June's VillageParsing reads that March's does not: diff it whole), and the runtime that moves the player inside a village
(`323aa170`, `323ac300`, `3250a800`), June's version. Find which March village / rest-area data carries the keys (census) and
name a reachable test spot for the hand-off.

**Done when:** census reports no unread VillageParsing key owned by E1; build rc=0; the warp works where June's data uses it,
or the Done note says no reachable data uses it yet (and which phase's data will).
