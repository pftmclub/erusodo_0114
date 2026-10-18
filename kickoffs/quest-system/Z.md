# June quest system - Phase Z - Close-out

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase quest-system Z` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | all phases |
| Flag | all `SERV_IRUHADEV_JUNE_QUEST_*` + `SERV_IRUHADEV_JUNE_RANDOM_MISSION` (`_DEBUG` off) |
| Log tag | - (`python tools/logs.py --tag JUNE_QUEST`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Context" "Phases" "Phase Z - Close-out"
```

## Task

Do "Phase Z":
- `MODS.md` rows (flag, files, revert);
- the backlog's Stage 4 dated note;
- memory;
- `_DEBUG` flags off (one build);
- the owed items handed to Stage 6 and Stage 9.
