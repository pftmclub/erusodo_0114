# June quest system - Phase Q5a - Quest UI: window, tabs, list / detail / quick, accept / complete / give-up

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase quest-system Q5a` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q0b, Q2 review |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_UI` |
| Log tag | `[JUNE_QUEST] UI` (`python tools/logs.py --tag JUNE_QUEST`) |
| Review | `kickoffs/quest-system/Q5a-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Context" "Rules" "Starting context" "Phase Q5a - Quest UI: window, tabs, list / detail / quick, accept / complete / give-up" "Phase Q0b - Quest UI, packet and movie map"
```

## Task

Do "Phase Q5a" from Q0b's rows targeted Q5a (`june2017_re/quest_plan/q0b_ui_map.tsv`).
- **`X2UIQuestNew.cpp` is CP949:** `bytepatch.py`.
- **DLGs.** If Q0b says so, adopt June's quest DLGs together with the code. Pack, then ask before the copy.
- **Scene calls stay March's**; they are Q5b's.

Done gate as in the section.
