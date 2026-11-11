# June character Add (Stage 7) - Phase CH3d - Hyper actives and the transcendence rows; CH3 stage review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-add CH3d` |
| Plan doc | `JUNE_CHARACTER_ADD_PHASES.md` (branch `main`) |
| Depends on | CH3c. One at a time with CH1-CH4: they all edit `X2GUAdd.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ADD_SKILL` and `SERV_IRUHADEV_JUNE_CHAR_ADD_TRANSCEND` (+ `..._DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ADD SKILL`) |
| Review | `kickoffs/char-add/CH3d-review.md` (Opus 5.5 high; covers CH3a-CH3d and every incidental shared-file hunk; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ADD_PHASES.md --no-sub "Rules" "Phase CH3 - Skills: second jobs (122, 123, 126), hyper actives, transcendence"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"   # "Transcendence rows every class port carries"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH3d - Done"   # once written
```

## Task

Do "CH3d": hyper actives (`RNSI_HA`, June's hyper switch in `32a28e00`), the `SHOW_HYPER_CUT_IN_TIME` states (7t), and the transcendence rows:
122 / 123 / 126 on the offline transcend allow-list (`TranscendCheck`, `Handlers_Quest.cpp`), his `LevelUpEvent.lua` TYPE rows +
`DLG_Transcendental_Change_Guide_Add.lua`; checklist 7ao whole (l.968 `+0x9330`; l.982's readers of `CX2Unit +0x3c`: `329e23a0`, `329e6f30`,
`329e6f80`, `329e7210`, `329e7ca0`, `329e7d60`, `329e99a0`, `329f10e0`, `329f9380`, `32a02260`, `32a047a0`, `32a0bfd0`, `32a15580`, `32a19bb0`,
`32a08150`, each branch whole; the four base helpers `31fcac90` / `31fcad70` / `31fceab0` `== 1` / `31ff1e80`); hyper-class skills
(`SPRT_HYPER_CLASS`) and `TRANSCENDENTAL_SKILL_DATA` rows; the transcended hyper table (`31fdd490`) and `HQ_Cutin_*_Transcendence`; `list_TR_*` rows
in `GetCharacterImageName` (T6c) and his transcended pictures, read from June's row pushes; `POPUP_TR_*` present. `python tools/skillicons.py check
<Add's skill-id range>` = PASS. Play-test on transcended units (`settranscend`): hyper-class skills, slots A-5 / B-5, picture and cut-in. Then write the
CH3 stage review kickoff.

You are on Opus: delegate the reads, greps, known-address Ghidra dumps, probe compiles and flagcheck runs (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only.

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
