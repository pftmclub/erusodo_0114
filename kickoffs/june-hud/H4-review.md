# June HUD - Review of H1-H4

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-hud H4 review` |
| Plan doc | `JUNE_HUD_PHASES.md` (branch `main`) |
| Depends on | H1, H2, H2b, H3, H4 |
| Flag | every `SERV_IRUHADEV_JUNE_HUD_*` flag + `SERV_IRUHADEV_JUNE_BUFF_FACTOR_LUA_BIND` (H2b) |
| Log tag | `[JUNE_HUD]` (`python tools/logs.py --tag JUNE_HUD`) |
| Review | this is the review |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_HUD_PHASES.md "Context" "Scope fence" "Rules" "Review (Opus 5.5, high)" "Phase H1 - Delete the buff-duration mod (Sonnet 5.5, medium)"
```

## Task

Do "Review": re-read each ported body against the assembly (delegate the pulls), run `junediverge.py --check-all` and `flagcheck` per flag, re-prove H1's deletion against its parent commit, and add JUNE_BUGS rows for kept quirks. Fix transcription errors under the phase's own flag; new June branches follow the scope rule (CLAUDE.md).

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
