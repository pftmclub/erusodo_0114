# June character Eve (Stage 7) - Phase CH3d - Hyper actives, hyper cut-ins, transcendence rows

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH3d` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH3c review. One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL`; transcendence pieces under the T-plan flags they extend (`JuneTranscendCheck` and the rest) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`), `[JUNE_TRANSCEND]` |
| Review | `kickoffs/char-eve/CH3d-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH3 - Skills: second jobs (108, 109, 116), hyper actives, transcendence"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"
python tools/docsection.py JUNE_TRANSCENDENCE_PHASES.md "Phase T2 - Becoming transcended: quest, notify, guide" "Phase T6 - Aisha transcended" "Phase T6c - Transcended class pictures"
```
Then checklist 7ao (Eve's row and "Every class also brings"; `plan_checklist_rows.md` has the lines) and the `CH3d` rows of `ch0_*.tsv`.

## Task

Do the CH3d row of the plan doc: the 6 hyper actives (type 13), the 3 `SHOW_HYPER_CUT_IN_TIME` states (7t), and the transcendence rows the template lists: allow-list in `Handlers_Quest.cpp`, the `LevelUpEvent.lua` TYPE row, adopt `DLG_Transcendental_Change_Guide_Eve.lua`, 7ao whole with `32994710` and the four helpers, the hyper-class skills and `TRANSCENDENTAL_SKILL_DATA` rows, the `31fdd490` table names, `HQ_Cutin_*_Transcendence`, the `list_TR_*` rows in `GetCharacterImageName`, her transcended pictures. Transcend a test unit with `python tools/elsdb.py settranscend <unit> 1`.
- Mark rows `(Eve: CH3d)`.
- Hand-off: a transcended 108 / 109 / 116, each hyper-class skill, slots A-5 / B-5.

## Amended by CH0

(CH0 fills this: the tsv filter, the function count, June addresses, any split.)
