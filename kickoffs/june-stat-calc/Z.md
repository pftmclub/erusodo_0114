# June damage and stat calculation - Phase Z - close

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase june-stat-calc Z` |
| Plan doc | `JUNE_STAT_CALC_PHASES.md` (branch `main`) |
| Depends on | every S / R review, D6 review |
| Flag | none new |
| Log tag | `[JUNE_STATCALC]` (`python tools/logs.py --tag JUNE_STATCALC`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_STAT_CALC_PHASES.md "Rules" "Phase Z - Close"
```

## Task

Close the plan as the phase section says: `MODS.md` complete for every `SERV_IRUHADEV_JUNE_STATCALC_*` flag; the map has no
`March` / `mixed` / `unknown` row (each remaining `class:` row names an open plan); `tools/data/june_diverge_allow.tsv` holds no
`stage33` reason; `SERV_IRUHADEV_JUNE_STATCALC_DEBUG` commented out (build started, not awaited); the backlog's Stage 33 marked
closed; the memory note `june-stat-calc-plan` updated; the STATUS block moved to `STATUS_ARCHIVE.md` with one *Closed plans* row.
`python tools/junediverge.py --check-all` must print `RESULT: PASS`.
