# June skill system core - Phase S3c - June skill types 14-18 in March's engine

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S3c` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S3 (+ its review) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_TYPE` (+ `SERV_IRUHADEV_JUNE_SKILL_DEBUG`) |
| Log tag | `[JUNE_SKILL] TYPE` (`python tools/logs.py --grep "JUNE_SKILL] TYPE"`) |
| Review | `kickoffs/skill-system-core/S3c-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S3 - The flip: June skill data and the save migration"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7ae. June skill-tree ids with no March class state (from June skill system S3)"
```

## Task

Found in S3 (unowned, so planned without asking: memory `plan-unowned-june-branches-without-asking`). June's `SKILL_TYPE` has
14-16 `ST_SPECIAL_ACTIVE_GRADE_1..3`, 17 `ST_CORE` and 18 `ST_CORE_PASSIVE` (June `Enum.lua`). March's C++ `SKILL_TYPE`
(`X2SkillTree.h`) stops at 13 `ST_HYPER_ACTIVE_SKILL`, and since S3 June's data gives most special actives type 14-16.

1. Add the five enumerators with June's values.
2. List every **engine** reader of `m_eType` in X2Lib/KTDXLIB (not class GU files: those are checklist 7ae): the tree UI and tooltip, the slot
   and quick-slot rules, cut-in / time-stop, MP and cool-time helpers, socket and buff code (`X2SocketItem.cpp:197` and `X2GUUser.cpp:15655`
   already name 14-16). For each, find June's counterpart in Ghidra (`funcmap.tsv` first) and port how June treats 14-18 (rule 1: whole
   functions). UI-only readers that S7 rewrites go to S7's kickoff instead.
3. Gate: a `[JUNE_SKILL] TYPE` census line at load (templets per type 0-18), and in game a grade-1 special active in a slot shows its type
   correctly in the tree UI and tooltip. The class state itself stays with the character ports.
