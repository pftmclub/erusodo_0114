# June dungeon result - Phase Z - Close-out

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase june-result Z` |
| Plan doc | `JUNE_DUNGEON_RESULT_PHASES.md` (branch `main`) |
| Depends on | R3 review (and R4 review if R4 ran) |
| Flag | all `SERV_IRUHADEV_JUNE_RESULT_*` (bookkeeping only) |
| Log tag | `[JUNE_RESULT]` |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_DUNGEON_RESULT_PHASES.md "Rules" "Phase Z - Close-out"
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 29: June dungeon result screen"
```

## Task

Do "Phase Z":
- `MODS.md`: June flag-index rows and revert lines for each flag that landed.
- Backlog: a dated close-out note in Stage 29, and on Stage 9's "GFx owed gates" and "root-UI screens" bullets (what moved, what
  stayed).
- Memory `june-result-plan`, plus its `MEMORY.md` line.
- Turn every `_DEBUG` flag off. Build and deploy only if one was still on.
- Mark the plan closed in STATUS.
