# June character Ara (Stage 7) - Phase CH6 - Whole-character walk and gap reruns

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase char-ara CH6` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH4b review, CH5 review, CHNb review |
| Flag | none (a defect goes back to its phase's flag) |
| Log tag | `[JUNE_CHAR]` (`ARA *`) |
| Review | none (the user's walk is the check) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Phase CH6 - Whole-character walk and gap reruns"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"   # the transcendence rows and the transition rule
```

## Task

Do "Phase CH6": back up the save; units of 7 / 28 / 31 / 34 / 118 / 119 / 124 and one transcended per second job. Re-run `census.py`,
`charport_scan.py`, `charport_keys.py Ara_Martial_Artist` and `s3_no_state.py --family X2GUAra.cpp --preprocess`; before / after numbers in the Done note
(0 unexplained `no_state`). Hand the user the walk: every skill in an A and a B slot (each cast twice), a hyper, force power (fill, spend, FP cancel,
gauge) and berserk, a Specter state on 34 / 124, the passives, one dungeon per job, both job changes on fresh units (CHNb's quests), then leave the game
and return to the village; **the becoming-transcended transition** on un-transcended 118 / 119 / 124 by `/tc 1`, `/qdone 42030`, `/additem 288493 1`
(guide popup with the class picture, new skill-tree rows, HUD portrait, relog).


## Amended by CH0

(CH0 fills this: the filter with real column numbers, the function / state / skill counts and June addresses, checklist rows, any split.)
