# June character Lu/Ciel (Stage 8) - Phase CH1 - Class core: Final: Lu/Ciel goes live

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel CH1` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1p review, NU4 review, CH0b |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE`, `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_DUO` defined |
| Log tag | `[JUNE_CHAR]` (`LUCIEL CORE` / `DATA`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/CH1-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the CH1 row: define `_LIVE` and `_DUO`, add the `UT_LUCIEL` cases to `CX2Game::CreateGUUser` (`X2Game.cpp:21276`) and the intruder
copy (`:2141-2201`), adopt `Luciel_Duo.lua` and `Luciel_Demonio.lua` (`adopt-june`, bytecode; `luabind.py check` both = PASS), copy the motion
koms CH0b held, and add the `[JUNE_CHAR] LUCIEL DATA` line at the end of InitState. Play-test (the plan's CH1 row): create, enter the village,
walk, jump, dash, combo, swap bodies, enter a dungeon, relog. Write `CH1-review.md` and amend CH2a / DU3 / CHN / TU from what the play-test shows.

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

## Amended by CH0

(empty until CH0 runs)
