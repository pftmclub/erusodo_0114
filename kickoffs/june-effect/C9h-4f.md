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

**Added by C3b (2026-10-08): the second (Now) set.** June has both sites.
- **Now set.** `CommonStateStart` (`32057b80`) `@32060cd9-32060dd5` zeroes a second set, `+0x8eb8..+0x8ef0` (5 rows x 3 floats). It then
  reads `USEFUL_VALUE0..4` (the same stem `33478e24` + index; 3 floats from index 1; a miss gives 0.0).
- **Its user engine readers:** `31fd0260` (the bounding LANDCRASH EventProcess, which reads `USEFUL_VALUE0[1]`; that state is C3d-3's, so
  port the member and parse here and leave the handler to C3d-3) and `3200d1d0 @3200d2cc` (unidentified: identify it with the other three).
- **Its other readers** (`dispsweep.py 8eb8 .. 8ef0`) are class code (checklist 7h) or other classes' layouts (`327xxxxx` NPC hits).
- **Port both sets with the same Now / Future split as March's frame data.** C3d-3 depends on this phase.
- funcmap row: `CX2GUUser +0x8eb8..+0x8ef0 (C3b)`.
