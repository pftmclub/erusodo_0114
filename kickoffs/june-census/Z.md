# June coverage census - Phase Z - Close-out

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase june-census Z` |
| Plan doc | `JUNE_COVERAGE_CENSUS_PHASES.md` (branch `main`) |
| Depends on | V2 (and the user's answers) |
| Flag | none (no code) |
| Log tag | none (read-only tooling) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_COVERAGE_CENSUS_PHASES.md "Context" "Rules" "Starting context" "Phase Z - Close-out"
```

## Task

Do "Phase Z" in the plan doc: apply the user's answers, check every new stage has a STATUS row and a kickoff, add the "re-run `junebinary.py` at each stage end" line to the backlog's Standing rules, archive this plan's STATUS block, and commit.

Added by V2 (2026-11-09): the one question is in the V2 Done note's last bullet (boss raid / world mission / wedding / mentor). Apply the
answer in `june2017_re/census/v2_exclusions.py` (`SYS['multi']` and the `M` row for those names), re-run it (`RESULT: PASS`), and update
`kickoffs/june-backlog/stage40-plan.md` item 4 to match. The Standing-rules line names both `tools/junebinary.py --check` and
`python -I june2017_re/census/v2_exclusions.py`. Then set the `stage40-plan` STATUS row to ready.
