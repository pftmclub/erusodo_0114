# June character Aisha (Stage 5 pilot) - Phase Z - Close-out

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase pilot-char-revamp Z` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH6 |
| Flag | retires `SERV_IRUHADEV_JUNE_CHAR_DEBUG` if any phase used it |
| Log tag | - |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Phase Z - Close-out" "Template for Stage 7" "Phase CH-N - A June-only later class path (template; not run for Aisha)"
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 6: transcendence system, on the pilot" "Stage 7: remaining March characters" "Stages 3–9 (short)"
```
Then every CHn Done note and review in this plan (the template is built from them).

## Task

Do "Phase Z".
- Rewrite "Template for Stage 7" from what the pilot took: real phase sizes, the splits CH0 made, model choices that worked, and recipe
  defects with their fixes.
- Write `kickoffs/june-backlog/stage6-plan.md` (transcendence, on Aisha) and `stage7-plan.md` (one plan per remaining character, using the
  template; order the characters by the template table and say why). Add both rows to STATUS.md's `june-backlog` block.
- Add the `MODS.md` section, mark the checklist rows, and add the backlog Stage 5 closing note.
