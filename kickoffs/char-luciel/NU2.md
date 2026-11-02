# June character Lu/Ciel (Stage 8) - Phase NU2 - The 2-byte unit class on the wire

| | |
|---|---|
| Start | new conversation on **Opus 5.5, xhigh** effort, then type `/x2-phase char-luciel NU2` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | NU1 review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_UNIT_CLASS_WORD` (Always.h **and** `ServerDefine.h`, `SECOND_HOMES`, `X2ServerProtocol_2010.vcxproj` defines) |
| Log tag | `[JUNE_CHAR]` (`LUCIEL NEWUNIT`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/NU2-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase NU - New-character foundation (NU1-NU4)" "Phase NU0 - New-unit and two-body engine map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the NU2 row of "Phase NU" over `nu0_unit_class_width.tsv`: every shared unit-class member becomes June's width (June's type, NU0 item 3),
and its Put / Get, emulator writers (`Handlers_Unit.cpp` casts) and client readers move in the same commit. Follow the CommonPacket.h exception
in CLAUDE.md, and prove both projects agree with `cl /P` on one file each (a `sizeof` probe of `KUnitInfo` in both). Gate: an existing character's
login, village, dungeon and relog round-trip with no new log line; a unit with class 200 written by hand into a scratch copy of the save reads back
as 200 (the `NEWUNIT` line).

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

## Amended by NU0

(empty until NU0 runs)
