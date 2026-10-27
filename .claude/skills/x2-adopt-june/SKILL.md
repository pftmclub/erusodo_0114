---
name: x2-adopt-june
description: Bring June's client .lua into the March kom_contents with tools/luasrc.py adopt-june (default - June's own bytecode via ljconv; --source for core-list files - decompile, T1, BOM source) and manage the core list (LUA_SOURCE_PHASES.md Rule 2). Use when the user types /x2-adopt-june, or says "port / adopt / bring in / take June's X.lua", "replace X.lua with June's", "is X on the core list", "add X to the core list", "which Lua is source", "core check", "iteration-order audit", or whenever a port needs a June .lua in the March tree.
---

# /x2-adopt-june - June's `.lua` into March, with one command

Every June `.lua` ships as June's own bytecode transcoded to March's opcode numbering (`june2017_re/ljconv.py`: header `0x82` -> `0x81`,
opcodes renumbered, nothing else; verified 71/71 against June's VM). Nothing is decompiled or edited, so no decompile defect can enter.
A file is **BOM source** only if it is on the **core list** (`tools/data/lua_core_list.tsv`): kept merged because March's C++ and
June's file disagree, or a port must edit it. Never hand-run `elstools`, `ljconv.py`, `junekom.py extract` or a `.lua` decompile for this.

```sh
# DEFAULT: June's bytecode. First positional may be the June kom (restricts where June's file is read from).
python tools/luasrc.py adopt-june Elsword_SwordMan.lua --dry-run          # plan only: placement, new/replaced/identical, sidecars
python tools/luasrc.py adopt-june data036 Elsword_SwordMan.lua Arme_VioletMage.lua
python tools/luasrc.py adopt-june --list names.txt                        # one name per line, or a TSV with a `name` column
python tools/luasrc.py adopt-june --list names.txt --compare june2017_re/june_npc_lua_port_20261009.tsv --dry-run   # byte-check vs an old run

# CORE-LIST FILES ONLY: decompile + T1 against ljconv(June) -> BOM + source, ledger row, snapshot, core row = source
python tools/luasrc.py adopt-june --source EffectSet.lua --dry-run
python tools/luasrc.py adopt-june --source EffectSet.lua

# The core list
python tools/luasrc.py core --check                                       # exit 1 on any FAIL; every BOM-text .lua must be on it
python tools/luasrc.py core --list [--state pending]
python tools/luasrc.py core --add data036 X.lua --origin june --why "edit: <plan phase> <what the port changes>"   # a port that must edit a June file
python tools/luasrc.py core --set data036 X.lua bytecode                   # source | bytecode | pending | "blocked: <reason>"
python tools/luasrc.py core --classify --out .build/classify.tsv           # every X.src.lua sidecar vs ljconv(June's file)
python tools/luasrc.py iter-audit path/to/X.lua                            # pairs/next/foreach tables in a source file
```

Both modes use `kom.py stage-june`'s placement (March's kom if March already has the name, else June's), duplicate (a name March has
in several koms is refused), long-name (>59 characters) and manifest rules. The staged file's manifest value is `"2"`.
A superseded `X.src.lua` beside a replaced file is moved to `<kom_contents>\..\kom_backup_stage_june\<stamp>\`, never deleted
(`kom.py compile` would fold it back over June's file). Each real run writes `june2017_re/june_adopt_<stamp>.tsv`
(name, kom, state, june_koms, size, shipped_sha1, mode, tier, note); `convert` later refuses those files by their sha1.

## When a file belongs on the core list

Add a row (`core --add`) only when one of these holds, and put the reason in `--why`:
- **kept-merged:** March's C++ needs something June's file lacks, or June's file needs something March's C++ lacks (an enum value, a UI
  id, a registry name). Measure it (load June's file over March's enums) and name the blocker.
- **edit:** a port must change a June file (a March-compat edit) before it can land. Add the row yourself and carry on; no question
  (user, 2026-10-09). Do not add a row because a file is big, important, or "might need an edit someday".

A sidecar (`X.src.lua`) is not a reason: `core --classify` showed 54 of the 65 sidecars equal June's bytecode at T0.
**Never delete a sidecar without the user.**

## Reading the outcome

| Output | Meaning / next step |
|---|---|
| `REFUSED X: on the core list ...` (default mode) | June's bytecode would drop the merge/edit. Use `--source` once its blocker is lifted, or leave it. |
| `REFUSED X: not on the core list` (`--source`) | `--source` is for core rows only. Add the row, or adopt without `--source`. |
| `REFUSED X: March already has it in N koms` / `name is N characters` / `not in any June kom` | stage-june's rules; fix the cause, do not rename. |
| `REFUSED X: T1 FAIL ...` + `BLOCKED X` (`--source`) | The decompile is not provably June's. Nothing was staged, the core row says `blocked: <reason>`. The file is an L5 (ljd) or T2 candidate (Opus audit + the user's yes); do not hand-fix a decompile into a pass. |
| `ITER AUDIT X: N site(s), M order-sensitive` | Text loading builds table constants with the parser, so `pairs`/`next`/`foreach`/`foreachi` can walk them in another order than the bytecode build. Read each flagged line; `ipairs` is index order. |
| `COMPARE OK: n of n ...` | The staged sha1s equal an earlier run's `shipped_sha1` and kom. |
| any adopted script (default or `--source`) | Run `python tools/luabind.py check <NAME>` on it before the play-test: a method the script calls that March never bound stops that function at the call (no popup, one ErrorLog line). Bind it from June's function or record it in `tools/data/lua_unbound_known.tsv`. |
| `ADOPT-JUNE OK / PARTIAL` | PARTIAL = something was refused (exit 4). The staged names are staged either way. |

## Steps this skill does NOT take

1. **Pack:** `/elstools pack dataNNN` (Claude does it; the pack gate refuses June `0x82` bytecode, BOM-less text and parse errors).
2. **Ask** the user before every copy, then `/x2-kom-packed copy dataNNN` (it overwrites a live archive; it refuses while the game runs).
3. The play-test. A June script stopping at a name March lacks logs `[LUA_SRC] lua error while loading X.lua` in `ErrorLog.txt`.

A calibration or experiment never touches the live tree: set `X2_KOM_CONTENTS`, `X2_LUA_LEDGER` and `X2_LUA_CORE_LIST` to scratch paths
first (snapshots and backups follow `X2_KOM_CONTENTS`' parent). A real run must also keep `luasrc.py ledger --check` and `core --check` at PASS.
