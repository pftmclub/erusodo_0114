# June character Add (Stage 7) - Phase Z - Close-out (closes Stage 7)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase char-add Z` |
| Plan doc | `JUNE_CHARACTER_ADD_PHASES.md` (branch `main`) |
| Depends on | CH6 |
| Flag | none |
| Log tag | none |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ADD_PHASES.md --no-sub "Phase Z - Close-out"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"
```

## Task

Do "Phase Z": the `MODS.md` section `June character: Add` (flags, files, data, revert order: exe + data036 + motion koms together; the
user-packed `StatTable.lua` rows; the creation-screen limit lifted for `UT_ADD`); no `_DEBUG` flag left defined; checklist rows marked `(Add: CHn)`;
the Aisha plan's "Template for Stage 7" gets an Add paragraph (what he took against this plan); backlog Stage 7 closed (all eight March characters
done); the `stage7-plan` STATUS row closed; the user's 2026-11-03 rule unparks `char-luciel` once Stage 7 closes: set its next rows ready and say so
in the hand-off; the `char-add` block moved to `STATUS_ARCHIVE.md` with a *Closed plans* row.

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
