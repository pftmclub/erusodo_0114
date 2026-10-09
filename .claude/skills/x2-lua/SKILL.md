---
name: x2-lua
description: Convert, edit and prove March client .lua files as BOM + UTF-8 source instead of bytecode, with tools/luasrc.py and kom.py (LUA_SOURCE_PHASES.md). Use when the user types /x2-lua, or says "edit / convert a March .lua", "is this lua proven", "find the source of X.lua", "decompile X" (after L2), "restore the bytecode", "ledger check", or whenever a change needs an existing March .lua edited and packed.
---

# /x2-lua - March .lua as source, with proof

Every March client `.lua` ships today as stripped LuaJIT bytecode (`1B 4B 4C 81`). The engine also loads **BOM + UTF-8 text**
(L0 proved it in game), so an edited `.lua` is stored as text and every later change is a plain text edit. Conversion
changes the **format only** (Rule 4); a content change is the port's own diff, made after conversion.

**Never decompile or compile by hand** (no `luajit -b`, no `ljd`, no `elstools compile-dir`): the tool compares against the shipped
bytes and writes the ledger. Staging is `kom_contents/dataNNN_kom/` (`data036`, `036`, `36`, `datai014` all work).

```sh
python tools/luasrc.py find data036 CashShopCategory.lua          # exact sources, each compile-compared (T0 / differs / no compile)
python tools/luasrc.py convert data036 CashShopCategory.lua       # bytecode -> BOM + source; snapshot + ledger row. Exit 4 = no proof
python tools/luasrc.py convert data036 'DLG_*.lua' --dry-run      # globs allowed; --dry-run writes nothing
python tools/luasrc.py convert --all --dry-run                    # census over every kom dir (what is provable now)
python tools/luasrc.py restore data036 CashShopCategory.lua       # put the snapshot bytecode back (ledger row -> restored)
python tools/luasrc.py refresh data036 CashShopCategory.lua       # re-record the sha after a deliberate text edit of X.lua
python tools/luasrc.py ledger --check                             # consistency; exit 1 on any FAIL
python tools/kom.py compile data036                               # fold X.src.lua into BOM-source X.lua (T0 or newer)
python tools/kom.py pack data036                                  # runs the Lua gate first
```

## Tiers and the usual flow (two lines)

**T0** = the source compiles byte-identical to the shipped bytecode (what `convert` accepts; sibling `X.src.lua`, then a studio
copy in `ClientScript\` / `_ClientScript\`). T1 (per-prototype equal decompile) and T2/T3 come with L2 and are **not** offered yet.
**To edit a bytecode-only `.lua`:** `convert` it (`find` first if you want to see the candidates), edit the text `X.lua`
(CRLF/BOM kept; ASCII comments), `luasrc.py refresh`, `kom.py pack`, then ask before `/x2-kom-packed copy`.

A new edit may also start from a sidecar: put `X.src.lua` beside `X.lua`, and `kom.py compile` folds it in when it is T0 or
**newer** than `X.lua` (an intended edit, ledger tier `EDIT`: no equivalence claimed), snapshotting the old bytecode first.

## What each refusal means

| Message | Meaning / next step |
|---|---|
| `REFUSED X: no exact source found` / `compiles, but differs ...` (convert, exit 4) | No T0 proof. The file needs L2's T1 decompile tier; until then it stays bytecode. Do **not** hand-decompile. For `C3d-4`-style needs, report the blocker (CLAUDE.md "never invent a new .lua"). |
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

## After L2 (not available yet)

`/x2-lua check` (T1 verdict for a file) and `/x2-lua decompile` (the fixed decompile route) arrive with L2. Until then say
"after L2" and stop.
