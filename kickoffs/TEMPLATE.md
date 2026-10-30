# <Plan name> - Phase <N> - <short title>

| | |
|---|---|
| Start | new conversation on **<Opus 5.5 / Sonnet 5.5>, <high / medium>** effort, then type `/x2-phase <plan-key> <N>` |
| Plan doc | `<PLAN>_PHASES.md` (branch `<branch>`) |
| Depends on | <phases that must be done, or "-"> |
| Flag | `SERV_IRUHADEV_<NAME>` (+ `<NAME>_DEBUG` if the phase logs) |
| Log tag | `[<TAG>]` (`python tools/logs.py --tag <TAG>`) |
| Review | `kickoffs/<plan-key>/<N>-review.md` (the phase writes it when done) / none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py <PLAN>_PHASES.md "Starting context" "<section>" "<section>"
```

## Task

<What to do, in the plan doc's words. Name June addresses and March functions. Say what
"done" means as a checkable gate (a census at 0, strings in the deployed exe, a log line).
Say where to stop and hand a decision to the user. Do NOT repeat build/deploy/hygiene/log
instructions: the x2-phase skill's Tools and Finish lists cover them for every phase.>

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
