# June character Chung (Stage 7) - Phase Z - Close-out

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase char-chung Z` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH6 (walk passed) |
| Flag | none (docs only; `_DEBUG` flags must be off) |
| Log tag | none |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase Z - Close-out"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Z - Done (2026-10-28, Sonnet 5.5)"
```

## Task

Do "Phase Z": the `MODS.md` section `June character: Chung` (flags, files, data, revert order: exe + data036 + motion koms together); no
`SERV_IRUHADEV_JUNE_CHAR_CHUNG_*_DEBUG` defined; checklist rows carry `(Chung: CHn)`; a Chung row and lessons in the Aisha plan's "Template for
Stage 7" (what he took against this plan; what writing June bodies inside March's functions from CH1a on changed); backlog Stage 7 note; the
`stage7-plan` STATUS row pointing at `char-elsword` (`/x2-phase new-plan char-elsword`); the `char-chung` block moved to `STATUS_ARCHIVE.md` with a
*Closed plans* row.

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
