# June character Add (Stage 7) - Phase CHNb - CH-N (b): the job-change routes and the class-change dialogs

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-add CHNb` |
| Plan doc | `JUNE_CHARACTER_ADD_PHASES.md` (branch `main`) |
| Depends on | CHNa review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ADD_CLASS` (+ `..._CLASS_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ADD CLASS`) |
| Review | `kickoffs/char-add/CHNb-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ADD_PHASES.md --no-sub "Rules" "Phase CH-N - Add's June-only class paths: Arc Tracer (33) -> Mastermind (123), Time Tracer (36) -> Diabolic Esper (126)"
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md "CHNb - Done" "CHNb - Review"
cat june2017_re/char_add/ch0_classn.tsv
```

## Task

Do CHNb from `ch0_classn.tsv`: June's quests 35540-35543 (to Arc Tracer 33, from 9 at level 15), 35550-35554 (to Mastermind 123, from 33 at
35), 35580-35583 (to Time Tracer 36, from 9 at 15) and 35590-35594 (to Diabolic Esper 126, from 36 at 35) with their rewards through
`Handlers_Quest.cpp`; `LevelUpEvent.lua`'s level-15 row opens `DLG_1ST_Class_Change_Guide_Add.lua` (three paths) and its level-35 row the second
guide (core-list rule if it must be edited); the `DLG_Class_Change_Add_*` dialogs (staged by CH0b); `X2OfflineClassChangeSeed.h` rows only if June's
cash route needs them; `luabind.py check` on each dialog. Full build and deploy; play-test: a level-15 Nasod Ruler sees the three-path dialog, takes
35540-35543 (`/qdone`) and becomes Arc Tracer; a level-35 Arc Tracer takes 35550-35554 and becomes Mastermind; a second level-15 unit takes
35580-35583 -> Time Tracer, then 35590-35594 -> Diabolic Esper; a third still reaches Psychic Tracer (35470-35473); all survive a relog.

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
