# June GFx runtime - Phase Z - Close-out

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase june-gfx Z` |
| Plan doc | `JUNE_GFX_RUNTIME_PHASES.md` (branch `main`) |
| Depends on | G1-G5 done (reviews included) |
| Flag | all `SERV_IRUHADEV_JUNE_GFX_*` |
| Log tag | `[JUNE_GFX]` |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_GFX_RUNTIME_PHASES.md "Context" "Rules" "Phase Z - Close-out"
```

## Task

Do "Phase Z". `MODS.md` (with the `.vcxproj` hunks), backlog dated notes (Stages 4, 6, 8, 9, 12), memory, `_DEBUG` off unless
a phase kept it on for a reason. Build and deploy only if `_DEBUG` changed.
