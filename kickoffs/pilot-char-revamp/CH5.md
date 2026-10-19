# June character Aisha (Stage 5 pilot) - Phase CH5 - UI and data: UnitTemplet, cut-ins, dialog and tree-UI class pieces

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH5` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1 review; may run beside CH2-CH4 (different files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_UNITTEMPLET` (if CH0 says the adoption needs a reader fix), `SERV_IRUHADEV_JUNE_CHAR_ARME_UI` |
| Log tag | `[JUNE_CHAR]` (`ARME UI`) |
| Review | `kickoffs/pilot-char-revamp/CH5-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Starting context" "Phase CH5 - UI and data: UnitTemplet, cut-ins, dialog and tree-UI class pieces"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7t. The hyper cut-in: data, and the class hooks that pick the berserk / Lu/Ciel names (from June effect completion C3t)"
```
Then CH0's `UnitTemplet.lua` decision (Done note) and the `ch5` rows of its tsvs.

## Task

Do "Phase CH5".
- **`UnitTemplet.lua`.** Every class loads it, so the play-test covers a select-screen pass of all nine March characters, not only Aisha.
- **June files.** Adopt them with `adopt-june` and stage them with `kom.py stage-june`. Pack, then ask before each copy.
- **CP949 files.** Edit them with `bytepatch.py` only.

## Amended by CH0

(CH0 fills this in.)
