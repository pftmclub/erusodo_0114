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

Do "Phase Z" in the plan doc: apply the user's answers, check every new stage has a STATUS row and a kickoff, add the "re-run `junecensus.py` at each stage end" line to the backlog's Standing rules, archive this plan's STATUS block, and commit.
