# Vendored Lua toolchain

`elstools decompile` and `compile` drive an external Lua toolchain. It is
bundled here so those commands work out of the box — the tool auto-discovers
this `vendor/` directory (next to the executable, the current directory, or the
crate root). You can still override any piece with `--ljd`, `--lua-src-dir`,
`--luajit`, and `--python`.

## Contents

| Path | What | License | Provenance |
|------|------|---------|------------|
| `ljd/` | **ljd** — LuaJIT bytecode decompiler (Python) | MIT | Andrian Nord, https://github.com/NightNord/ljd (and forks) — see `ljd/LICENSE` |
| `luajit/jit/` | LuaJIT runtime Lua modules (`bcsave.lua`, `dis_*.lua`, …), used by `luajit -b` and by ljd | MIT | LuaJIT, https://luajit.org — see `luajit/COPYRIGHT` |
| `luajit/luajit.exe` | LuaJIT 2.0.2 compiler binary, 32-bit Windows | MIT (LuaJIT) + KOG modification | see note below |

## Requirements

- **Python 3** on `PATH` (ljd is a Python program). Override with `--python`.
- The `luajit.exe` here is a **Windows** binary. On Linux/macOS, build or install
  a stock LuaJIT 2.0.x and pass it with `--luajit` (the tool's built-in
  standard→KOG fallback converts stock output to the KOG format, so a stock
  LuaJIT works — the bundled binary is only a convenience).

## Note on `luajit.exe`

This binary is a LuaJIT 2.0.2 build that carries KOG Games' proprietary
`KOG_BC_FORMAT` bytecode-format modification (from the Elsword client's build of
LuaJIT). It produces the KOG `0x81`/`0x82` headers natively. It is included here
purely so `compile` works turnkey on Windows. If you prefer to avoid the KOG
build entirely, delete it and point `--luajit` at any stock LuaJIT 2.0.x — the
result is identical because of the fallback conversion.

The LuaJIT project and KOG Games are the respective owners of that binary's code;
this repository redistributes it for interoperability convenience only.
