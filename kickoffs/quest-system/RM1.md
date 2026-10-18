# June quest system - Phase RM1 - Random mission port

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase quest-system RM1` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | RM0 (and the user's answer), Q4 review |
| Flag | `SERV_IRUHADEV_JUNE_RANDOM_MISSION` |
| Log tag | `[JUNE_QUEST] RM` (`python tools/logs.py --tag JUNE_QUEST`) |
| Review | `kickoffs/quest-system/RM1-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Context" "Rules" "Starting context" "Phase RM1 - Random mission port" "Phase RM0 - Random mission map and source proposal"
```

## Task

Do "Phase RM1" from `june2017_re/quest_plan/rm0_map.tsv` and the user's answer recorded in RM0's Done note.

The 13 per-class skill-use sites sit in class files that other ports also edit. Check `git status` and keep to your own hunks.

Done gate as in the section.
