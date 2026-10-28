# One function per name - Phase D0b - Rulings: twins, funcmap hints, names

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-march-divergence D0b` |
| Plan doc | `JUNE_MARCH_DIVERGENCE_PHASES.md` (branch `main`) |
| Depends on | D0a done |
| Flag | none (no code) |
| Log tag | none (add one only if a check needs a log line) |
| Review | none (C2 / C3 / C4 / C5 reviews check the map as applied) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Context" "Rules" "Starting context" "Phase D0a - Census tool (Sonnet 5.5, high)" "Phase D0b - Rulings: twins, funcmap hints, names (Opus 5.5, high)"
```

and D0a's Done note, `june2017_re/diverge/names.tsv` and `callees.tsv` (filter, never read whole).

## Task

Do "Phase D0b", steps 1-4. Ghidra must be running. Delegate the pulls (funcmap rows per pair, March definitions, name proposals by
class pattern) to Sonnet subagents; approve every merge / rename / verdict yourself. Write `tools/data/june_march_divergent.tsv` and
`june2017_re/diverge/collapse_map.tsv`; put each batch's file list into `C1.md`-`C5.md`; write `D0c-1.md`.. from `D0c-1.md`'s shape
for the areas you set (default: unit classes / engine, effect, damage, world / data, UI, game managers). A removed March feature is not a
question (Rules). Ask the user only for a name you cannot settle from the class's pattern; list those in the Done note.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
