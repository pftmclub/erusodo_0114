---
name: x2-lua
description: Convert, edit and prove March client .lua files as BOM + UTF-8 source instead of bytecode, with tools/luasrc.py and kom.py (LUA_SOURCE_PHASES.md). Use when the user types /x2-lua, or says "edit / convert a March .lua", "is this lua proven", "find the source of X.lua", "decompile X", "check X against the bytecode", "is this T1", "T2 report", "run the lua selftest", "restore the bytecode", "ledger check", or whenever a change needs an existing March .lua edited and packed.
---

# /x2-lua - March .lua as source, with proof

Every March client `.lua` ships today as stripped LuaJIT bytecode (`1B 4B 4C 81`). The engine also loads **BOM + UTF-8 text**
(L0 proved it in game), so an edited `.lua` is stored as text and every later change is a plain text edit. Conversion
changes the **format only** (Rule 4); a content change is the port's own diff, made after conversion.

**Never decompile or compile by hand** (no `luajit -b`, no `ljd`, no `elstools compile-dir`): the tool compares against the shipped
bytes and writes the ledger. Staging is `kom_contents/dataNNN_kom/` (`data036`, `036`, `36`, `datai014` all work).

```sh
python tools/luasrc.py find data036 CashShopCategory.lua          # exact sources, each compile-compared (T0 / differs / no compile)
python tools/luasrc.py convert data036 CashShopCategory.lua       # bytecode -> BOM + source (T0 source, else a T1 decompile); exit 4 = no proof
python tools/luasrc.py convert data036 'DLG_*.lua' --dry-run      # globs allowed; --dry-run writes nothing
python tools/luasrc.py convert --all --dry-run --no-decompile     # fast census, T0 sources only (the decompile tier is slow)
python tools/luasrc.py restore data036 CashShopCategory.lua       # put the snapshot bytecode back (ledger row -> restored)
python tools/luasrc.py refresh data036 CashShopCategory.lua       # re-record the sha after a deliberate text edit of X.lua
python tools/luasrc.py ledger --check                             # consistency; exit 1 on any FAIL
python tools/kom.py compile data036                               # fold X.src.lua into BOM-source X.lua (T0 or newer)
python tools/kom.py pack data036                                  # runs the Lua gate first
```

Proof tools (L2). `convert` runs the decompile + T1 itself; these are for looking, reporting and testing the checker. A bytecode
argument is a path or `KOM/NAME` (the staged file, or its snapshot once converted):

```sh
python tools/luasrc.py decompile data038/SQUARE_GM_Ariel.lua -o .build/ariel.lua     # March 0x81 or June 0x82 -> BOM text (NOT proof)
python tools/luasrc.py check .build/ariel.lua data038/SQUARE_GM_Ariel.lua            # CHECK T0 / CHECK T1 / CHECK FAIL (exit 1)
python tools/luasrc.py t2 .build/ariel.lua data038/SQUARE_GM_Ariel.lua --out .build/t2   # T2 report (R/B/S dumps), never a pass
python tools/luasrc.py decompile june2017_re/june_lua/DamageSound.lua -o .build/ds.lua
python tools/luasrc.py check .build/ds.lua june2017_re/june_lua/DamageSound.lua --ref june   # June: against ljconv(June bytecode)
python tools/luasrc.py selftest                         # seeded edits T1 must reject + controls it must accept (PASS = 100%)
python tools/luasrc.py calibrate --sets class           # verdicts for the nine class scripts (also: march-t0, march-nogt, june)
```

## Tiers and the usual flow (two lines)

**T0** = byte-identical recompile; **T1** = every prototype equal (opcodes, operands, flags, params, framesize, upvalues, child tree),
constants equal **by value**, slot order ignored; a table constant's key/value pairs must match in their own table, their
storage order (what `pairs` walks) is ignored (user, 2026-10-09). Only T0/T1 convert: sibling
`X.src.lua`, studio copy, then the ljd decompile (with the template-layout repair). **T2** = `t2` report only (needs an Opus audit + the
user's yes); **T3** = unproven, the bytecode ships.
**To edit a bytecode-only `.lua`:** `convert` it (`find` first if you want to see the candidates), edit the text `X.lua`
(CRLF/BOM kept; ASCII comments), `luasrc.py refresh`, `kom.py pack`, then ask before `/x2-kom-packed copy`.

A new edit may also start from a sidecar: put `X.src.lua` beside `X.lua`, and `kom.py compile` folds it in when it is T0 or
**newer** than `X.lua` (an intended edit, ledger tier `EDIT`: no equivalence claimed), snapshotting the old bytecode first.

## What each refusal means

| Message | Meaning / next step |
|---|---|
| `REFUSED X: ...; decompile: decompile is not T1: N difference(s); first ...` (convert, exit 4) | No T0 source and the decompile differs (usual causes: ljd prints locals as `slotN` globals, an upvalue as `uvN`, a hoisted IIFE). The file stays bytecode. Do **not** hand-fix the decompile into a "pass"; a T2 route needs an Opus audit and the user. For `C3d-4`-style needs, report the blocker (CLAUDE.md "never invent a new .lua"; "this needs a higher model"). |
| `REFUSED X: ...; decompile: ljd timed out / ljd failed` | ljd could not decompile it (huge file, printer crash). Stays bytecode; `X2_LJD_TIMEOUT=1800` for a retry. |
| `CHECK T1 ... N table constant(s) hold the same pairs in another storage order` | Passes (the user's rule): same pairs in the same tables, only the `pairs` walk order differs. `decompile` already tries to restore the original order (best effort, small budget). |
| `not-march-bytecode` / `manifest not "2"` (skipped) | Already text, June 0x82, the January-XOR file, or a user-packed manifest-`"0"` file. Convert never touches them. |
| pack `REFUSED ... text without the UTF-8 BOM` | Plain text without `EF BB BF`: the binder returns false with no log (only `KEM_ERROR73`). Add the BOM. |
| pack `REFUSED ... June 0x82 bytecode` / `unknown header` | March cannot load it. June files go through ljconv / adopt-june (L3), never packed as-is. |
| pack `REFUSED ... does not parse under luajit (line N: ...)` | Syntax error in the text; fix it and pack again. |
| compile `REFUSED: .src.lua is OLDER than X.lua and not T0` | A stale sidecar beside a converted / ljconv file. Delete the sidecar, or `touch` it if it really is the intended text. |
| compile `REFUSED: .src.lua does not compile` | Syntax error in the sidecar. |
| ledger `text differs from the ledger sha1` | X.lua was edited after conversion: `refresh` records it. |
| ledger `BOM text with no ledger row` | A hand-made text file: add a row via `convert`, or ask what produced it. |

Manifest-`"0"` `.lua` (server Lua the user packs by hand) only **warns** in the gate, never refuses; the CLAUDE.md server-Lua rule is
unchanged (name the file, never pack it yourself with kom.py). The two oddities (`data014 UI_NPC_KELAINO.lua`, `data036 PvPDropTable.lua`)
are ledger exceptions (tier `EXC`).

## Files

Bytecode snapshots: `...\24965799\kom_contents_bytecode\dataNNN_kom\` (X.lua and any sidecar). Ledger: `tools/data/lua_source_ledger.tsv`
(kom, name, origin, reference, sha, source, tier, source_sha1, phase, date, reason, state). Scratch trees: set `X2_KOM_CONTENTS`,
`X2_KOM_BYTECODE`, `X2_LUA_LEDGER` (all three, or a test writes the real ledger).

## What the checker does not cover

- `pairs` order. T1 ignores a table constant's storage order (user, 2026-10-09), and a converted file (T0 included) loaded as
  **text** builds constant tables with the parser, while the shipped bytecode is re-inserted by the bytecode reader, so `pairs`
  can walk a table in another order than the shipped client did. L2 audited March:
  nothing depends on it (11 files use `ipairs`, 2 use `pairs` only in a shallow `TableCopy`, the engine's two `lua_next` loops are
  order-free). `t2` shows it as `BS loader effect ... order only`.
- T2's mock environment runs only the main chunk; engine globals are stubs (calls into them are recorded with their arguments and writes into them are dumped, L2 review; every call returns a stub, so a branch on an engine value takes one side only); function bodies are compared by T1 alone.
- June mode compares against `ljconv`'s output; its opcode table was verified against June's VM handlers
  (`june2017_re/lua_source_opcode_check.tsv`, 71/71).
