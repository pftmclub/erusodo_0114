# One function per name - Phase D0a - Census tool

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-march-divergence D0a` |
| Plan doc | `JUNE_MARCH_DIVERGENCE_PHASES.md` (branch `main`) |
| Depends on | **char-eve complete (its Z row)**: the user runs this plan after the Eve port (2026-10-28) |
| Flag | none (no game code) |
| Log tag | none (add one only if a check needs a log line) |
| Review | none (D0b uses it) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Context" "Rules" "Starting context" "Phase D0a - Census tool (Sonnet 5.5, high)"
```

and `tools/enumscan.py:437-666` (the region walker to reuse; do not edit enumscan), `june2017_re/funcmap.tsv` (header row).

## Task

Do "Phase D0a". Write `tools/junecensus.py` and its output in `june2017_re/diverge/` (`names.tsv`, `callees.tsv`), `--summary`,
`--check-twins`, `--file <path>`. Compare its counts with the Context census and explain every gap over a few percent. Add a
`tools/README.md` row. No game code changes, no build.
