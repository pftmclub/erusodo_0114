# June HUD - Phase Z - Close-out

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase june-hud Z` |
| Plan doc | `JUNE_HUD_PHASES.md` (branch `main`) |
| Depends on | H4 review |
| Flag | `_DEBUG` off |
| Log tag | `[JUNE_HUD]` (`python tools/logs.py --tag JUNE_HUD`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_HUD_PHASES.md "Phase Z - Close-out (Sonnet 5.5, medium)" "Scope fence"
```

## Task

Do "Phase Z": the MODS.md "June HUD: Stage 37" section (flags, files, revert notes, the H1 deletion line), the Stage 37 CLOSED line in `JUNE_PORT_BACKLOG_ORDER.md` with every stub routed, memory, `_DEBUG` off, and the STATUS block moved to `STATUS_ARCHIVE.md`.

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
