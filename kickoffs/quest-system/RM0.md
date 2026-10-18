# June quest system - Phase RM0 - Random mission map and source proposal

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system RM0` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | - |
| Flag | none (read-only) |
| Log tag | `[JUNE_QUEST] RM` (none written) (`python tools/logs.py --tag JUNE_QUEST`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Context" "Rules" "Starting context" "Phase RM0 - Random mission map and source proposal"
```

## Task

Do "Phase RM0". It is read-only.

1. Read `kickoffs/june-backlog/stage4-plan.md` ("Carried in: June's random mission system") and
   `june2017_re/quest_plan/q_rm_callsites.tsv` first.
2. Name the March function behind each raise site. The per-class skill-use functions already carry a `3235a710` comment
   (`JUNE_SKILL_SYSTEM_PHASES.md:759-762`).
3. Do not decide the source yourself. Present the options with what each costs:
   - keep it dormant, with the cheat setting a mission;
   - a labelled stand-in schedule under its own flag.
   Then stop for the user's answer.
4. Write `rm0_map.tsv` and funcmap rows `Quest RM0`, and amend `RM1.md` with the answer.
