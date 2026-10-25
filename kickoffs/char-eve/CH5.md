# June character Eve (Stage 7) - Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH5` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1 review; may run beside CH2-CH4 (different files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_UI` |
| Log tag | `[JUNE_CHAR]` (`EVE UI`) |
| Review | `kickoffs/char-eve/CH5-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH5 - Done (2026-10-21, Sonnet 5.5)"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7t. The hyper cut-in: data, and the class hooks that pick the berserk / Lu/Ciel names (from June effect completion C3t)"
```
Then CH0's UnitTemplet note and the `CH5` rows of its tsvs.

## Task

- Replace Eve's 7 `UnitTemplet.lua` rows (data036, BOM source; lines 372-449, 664-715, 920-945, 1053-1078 at planning time) with June's text, every other row untouched; `luasrc.py refresh`; pack, ask before the copy.
- Replace the cut-in table `X2GuEve.cpp:48-85` with June's names and layout (June's `.gfx` are in data152); Electra (26) as June has it.
- The dialog / tree-UI pieces CH4 does not take.
- Hand-off: a select-screen pass of all nine March characters, then an Eve skill cut-in and a hyper cut-in.

## Amended by CH0

(CH0 fills this: the tsv filter, the function count, June addresses, any split.)
