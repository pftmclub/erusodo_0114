# June character Aisha (Stage 5 pilot) - Phase CH3 - Skills: second jobs (104, 105, 113) and hyper actives

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH3` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH2 review; not alongside CH4 (same class files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (or CH0's split) |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`) |
| Review | `kickoffs/pilot-char-revamp/CH3-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Violet Mage and the first jobs (classes 2, 14, 15, 23)" "Phase CH3 - Skills: second jobs (104, 105, 113) and hyper actives"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7t. The hyper cut-in: data, and the class hooks that pick the berserk / Lu/Ciel names (from June effect completion C3t)"
```
Then CH2's Done note and review (the recipe and what the review corrected), and the `ch3` rows of CH0's tsvs.

## Task

Do "Phase CH3" with CH2's recipe, plus the hyper actives and the three `SHOW_HYPER_CUT_IN_TIME` states. Same rules as CH2: CP949 files go
through `bytepatch.py`, damage numbers go to `MODS.md`, and anything the map does not settle is "this needs Opus".

## Amended by CH0

(CH0 fills this in.)
