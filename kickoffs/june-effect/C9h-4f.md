# June effect completion - C9h-4f - June's `USEFUL_VALUE<n>` per-state float table (found in C9h-4c)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C9h-4f` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-4c (same two parse sites in `X2GUUser.cpp`, so not at the same time as 4a / 4b / 4d) |
| Flag | `SERV_IRUHADEV_JUNE_USEFUL_VALUE` (+ `_DEBUG` only if you need temporary lines) |
| Log tag | `JUNE_FX USEFUL_VALUE` |
| Review | write `kickoffs/june-effect/C9h-4f-review.md` when done (Opus 5.5 high) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-4c - Done"
```

## Task

C9h-4c's inventory of June's `CommonStateStartFuture` (`320506f0`) found a block right after the cancel-after clamp that March has no counterpart for:
`@32056a15-32056c..` zeroes the user floats `+0x8ef4..+0x8f2c` (14 floats), then reads a table whose key stem is the string `USEFUL_VALUE` (at `33478e24`) with an index
appended (June scripts write `USEFUL_VALUE0 = { ... }`; 26 `june_src_fixed/data036_kom` scripts carry it, e.g. `Add_NasodRuler.src.lua`, `Ain_Angel.src.lua`).
Readers of `+0x8ef4` are mostly class code (`dispsweep.py 8ef4`: `329d7220`.. `32b6ced0`) plus three engine-looking functions (`31fc8620`, `31fc9a20`, `3200d3f0`).

1. Decode the block in Ghidra / `disasm_range.py` (the exact key strings, the index range, the table's row type, what `+0x8ef4` itself is, what happens on a miss).
2. Identify the three engine-side readers; port the member, the parse (both CommonStateStart sites if June has both) and those readers under the flag.
3. Class readers stay with the character ports: add a row to `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7h (what the port must bring).

**Reach:** inert on March data (no March script carries `USEFUL_VALUE`); June scripts of March classes use it, so it matters when those scripts are adopted.
