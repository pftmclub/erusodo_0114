# June engine remainder - Phase R0b - Render and skin-mesh sweep

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine R0b` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | - (read-only; runs now; better after R0 so it can exclude R0's functions) |
| Flag | none (no code) |
| Log tag | `[JUNE_ENGINE]` (`python tools/logs.py --tag JUNE_ENGINE`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Investigation ledger (measured 2026-10-16)" "Phase R0b - Render and skin-mesh sweep"
```

## Task

Do "Phase R0b": a function-by-function June vs March verdict for the listed render / skin-mesh classes, excluding functions R0 and earlier `Effect *` funcmap rows already map. Each difference: a funcmap row `Engine R0b` and a new phase (R7+) in the plan doc with its kickoff and STATUS row, or an owner. Done gate: every function on the list has a verdict.
