# June character Add (Stage 7) - Phase CH6 - Whole-character walk and gap reruns

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase char-add CH6` |
| Plan doc | `JUNE_CHARACTER_ADD_PHASES.md` (branch `main`) |
| Depends on | CH4b review, CH5 review, CHNb review, CH0c review |
| Flag | none (a defect goes back to its phase's flag) |
| Log tag | `[JUNE_CHAR]` (`ADD *`) |
| Review | none (the user's walk is the check) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ADD_PHASES.md --no-sub "Phase CH6 - Whole-character walk and gap reruns"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"   # the transcendence rows and the transition rule
```

## Task

Do "Phase CH6": back up the save; units of 9 / 32 / 33 / 36 / 122 / 123 / 126 and one transcended per second job. Re-run `census.py`,
`charport_scan.py`, `charport_keys.py Add_NasodRuler` and `s3_no_state.py --family X2GUAdd.cpp --preprocess`; before / after numbers in the Done note
(0 unexplained `no_state`). Hand the user the walk: a new Add made on the creation screen; every skill in an A and a B slot (each cast twice: cool
time, MP or DP), a hyper, DP (fill, spend, formation mode in and out, gauge), the nasod armor combo, Phase Shift, Crest of Time, each summon, the
passives, one dungeon per job, all three first-job changes on fresh units (CHNb's quests), then leave the game and return to the village; **the
becoming-transcended transition** on un-transcended 122 / 123 / 126 by `/tc 1`, `/qdone 42030`, `/additem 288493 1` (guide popup with the class
picture, new skill-tree rows, HUD portrait, relog).

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-add <phase>` comment (at the start of an existing one-line comment, never a second line).


## Amended by CH0

(CH0 fills this: the filter with real column numbers, the function / state / skill counts and June addresses, checklist rows, any split.)
