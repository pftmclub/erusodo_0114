# June2017 EffectSet / DamageEffect / DamageData Rebuild

This plan updates the March-2014 client's effect engine to the June2017 one. It covers
`CX2EffectSet`, `CX2DamageEffect::CEffect` and all of its sub-blocks, and
`CX2DamageManager::SetDamageData` plus the hit path. The source is the June2017 client
binary, **read instruction by instruction in Ghidra**, not inferred from the data files.

This document is self-contained. To run a phase, paste its section together with the
"Starting context" block below into a fresh conversation, then paste its **Kickoff
prompt**. Phases are sized for one conversation each and are meant to run in order,
because each one consumes the previous one's outputs.

**Why this exists.** The Ranox/Lanox port ships June2017 content on a March-2014 engine.
Every engine gap so far was closed by inference, because `RANOX_EFFECT_ENGINE_GAPS.md` §6
said the June `x2.exe` was packed and had "zero hits". Two examples:

- Phase C's `ATTACH_SKIN_MESH` reconstruction, including a `VALUE=0.01` nobody can explain.
- The open "projectiles render but hit nothing" questions in `RANOX_EFFECT_RUNTIME_UNKNOWNS.md` §4.

On 2026-09-23 it turned out that an **unpacked** dump of that binary exists and is loaded
in Ghidra. The missing strings were never missing: they are XOR-encrypted at compile time,
and they decrypt mechanically. So the June engine can now be read rather than guessed.

**Fix philosophy (the user's, and this plan's whole point):** *no more guessing code.*

- Every behaviour added here is transcribed from a specific June function and cites it.
- Where the binary cannot settle a question, the answer is to read more of the binary. It
  is never a plausible default.
- Where June depends on a system this build does not have at all (Rose weapon, memo,
  natural force, numerical skill data), the key is **parsed, stored and logged as
  unsupported**. No behaviour is faked for it.
- The standing CLAUDE.md rules still hold: no hardcoded tables, no invented curves, and
  server-side Lua is the user's to pack.

---

## Starting context (paste this with any phase)

- Repo root: `F:\elsword stuff\elsword_2014\els_2014\ElswordFiles\ready_and_built\ProjectX2_SVN\ProjectX2_SVN\source\EU_CN_US\Trunk`
- Branch: `mods/lanox-port`
- Game directory (deploy target, logs, save):
  `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data`
- **June2017 reference client:** `F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\`
  - Binary: `unpacked_x2.exe`, 43,801,600 B, SHA-256
    `f2376a7210a6245b68ec772afbf0dd3d89a9f52a5b66f47bc739a7ad1c462af1`. Imagebase
    `0x31aa0000`; for `.text`, VA = raw + 0xc00 + imagebase. **Not** `x2.exe` next to it,
    which is the packed original.
  - Scripts: `data036_kom/` (XOR'd and compiled; decompile with elstools, see below).
    `GameMajorXMeshPlayer*.txt` / `UIMajorXMeshPlayer*.txt` in the same directory are
    plaintext.
- **RE scratch directory: `june2017_re/`.** Read its `README.md` first. It holds:
  - The confirmed/candidate June function map.
  - The XOR-string idiom and its hand-verified controls.
  - The prototype decryptors, with their known flaws.
  - The method notes (dead ends).
  - `regen.sh`, which rebuilds the gitignored Ghidra dumps and the decompiled June lua.
    Run it with `sh regen.sh`, because the exec bit isn't kept.
- **Ghidra** (`.claude/skills/ghidra/SKILL.md`, driven with curl, no MCP):
  - Project `F:\elsword stuff\els_jun_2017_RE\els_jun_2017.gpr`. The user must have it
    open with the GhidraMCP server running.
  - Health check:
    ```sh
    curl -s --noproxy '*' -m 10 "http://127.0.0.1:8080/methods?limit=1"
    ```
  - **Quote the `'*'`.** Unquoted, the shell globs it and you get a Cloudflare 302 instead
    of Ghidra. The skill file itself shows the broken unquoted form until Phase 0 fixes it.
  - If the check fails, stop and ask the user to start Ghidra and the server. Don't loop.
  - `decompile_function` fails on large functions (the CEffect ctor and up). Ask the user
    to raise Edit → Tool Options → Decompiler → timeout and max payload. Until then,
    `disassemble_function` always works.
  - `get_function_by_address` needs an entry point. To find the function that contains an
    arbitrary address, use `xrefs_to` on it.
  - **Mutations** (rename, comment, retype) edit the user's Ghidra database. Say what will
    change and get an OK first.
- **Lua tooling:** `elstools/elstools.exe`.
  - `decompile-batch -i <file.lua>` writes `<file>.src.lua` next to the input. Copy June
    files into `june2017_re/june_lua/` first, so the June directory is not littered.
  - `compile-batch -v mar-2014 -i <file.lua>` compiles **in place**.
- **Client build** (default `US_SERVICE`, see CLAUDE.md *Toolchains*):
  ```sh
  MSBUILD="/c/Windows/Microsoft.NET/Framework/v4.0.30319/MSBuild.exe"
  TRUNK="F:/elsword stuff/elsword_2014/els_2014/ElswordFiles/ready_and_built/ProjectX2_SVN/ProjectX2_SVN/source/EU_CN_US/Trunk"
  DATA="F:/elsword stuff/elsword_2014/els_2014/237311/24965799/data"
  MP="-m -p:MultiProcessorCompilation=true"
  "$MSBUILD" X2ServerProtocol/X2ServerProtocol_2010.vcxproj -p:Configuration=X2TOOL      -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  "$MSBUILD" KTDXLIB/KTDXLIB_2010.vcxproj                   -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  "$MSBUILD" X2Lib/X2Lib_2010.vcxproj                       -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  "$MSBUILD" X2/X2_2010.vcxproj                             -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  ```
  - `touch X2Lib/stdafx.cpp` after any header edit, or the PCH silently discards it.
  - Deploy `X2/US_SERVICE/x2.exe` (lowercase) to `$DATA/X2_offline.exe`, and confirm the
    size and mtime programmatically.
  - **Ask the user before launching it, every time.**
- **Rules specific to this plan**, on top of CLAUDE.md, which auto-loads:
  1. **Cite the source.** Every new code path carries a comment naming the June function
     and address range it was transcribed from, e.g.
     `// June2017 FUN_31f6ac60 @ 31f6b2e0-31f6b3a4: LOOP_ADD_POS_* read as float, default 0`.
  2. **Re-express, don't transplant.**
     - Implement June semantics on March's classes, members and helpers.
     - June member offsets (`[ECX+0x2a4]`) describe June's struct layout. They say nothing
       about March's. Never port an offset.
     - Name new members after the Lua key, in the March class's own naming style.
  3. **A key name needs two independent sources.** Use a key only when the decryptor output
     *and* the plaintext key in a decompiled June `.lua` agree. A misspelled
     `PLUA_GET_VALUE` key compiles and parses nothing, silently.
     - Studio typos are real and must be kept verbatim if both sources agree, e.g.
       `USE_ADVANCE_COLLISIOIN_CHECK`, `ONYL_ATTACK_USER`,
       `CREATE_EFFECT_SET_MAIN_EFFECT_LIFTTIME`.
     - A key the binary reads but no June data file uses is still implemented, marked
       binary-only in the matrix.
  4. **Flags.**
     - One `SERV_IRUHADEV_JUNE_<SUBSYSTEM>` per logical change, appended to
       `KTDXLIB/Always.h` after the last entry. At plan time that was
       `SERV_IRUHADEV_RANOX_EFFECT_DEBUG2` at line 4267; check for newer ones first.
     - Diagnostics go behind one shared, short-lived `SERV_IRUHADEV_JUNE_FX_DEBUG`. Emit
       them with `CX2OfflineLog::Server( L"JUNE_FX ..." )`: grep-able prefix, throttled
       to about once a second for anything per-frame.
     - `CX2OfflineLog::Server` is **not** open during every loading path; see
       `LANOX_DEFECT_PHASES.md`'s note. If a load-time line doesn't land, use
       `ErrorLogMsg` and say so.
  5. **Unsupported systems.** Parse and store the key, then log once per key per templet:
     `JUNE_FX unsupported <KEY> in <templet>: <system> absent`. No behaviour. The phase
     that introduces it records it in the key matrix as *unsupported*, with the missing
     system named.
  6. **Stop at the wire.** If a June feature needs a change under `KncWX2Server/Common/`
     (packet struct, event ID, shared enum), stop and record it. The servers cannot be
     built here (VS2003 is not installed).
  7. **Encoding.** Every engine file this plan touches is ISO-8859/CP949:
     - `X2Lib/X2EffectSet.{h,cpp}`, `X2DamageEffect.{h,cpp}`, `X2DamageManager.{h,cpp}`,
       `X2Main.cpp`, `KTDXLIB/KTDGXMeshPlayer.{h,cpp}`.
     - Edit at byte level only: Python `rb`/`wb`, anchored on exact bytes read with
       `python`/`cat -A`, not `cat`.
     - After every edit, check `file <path>` (encoding unchanged) and `git diff --stat`.
       A few lines of change should not produce hundreds, and there must be no deletions
       on a pure insertion.
  8. **Keep the key matrix current.** Every phase from 2 on updates
     `JUNE2017_EFFECT_KEY_MATRIX.md` (created in Phase 1): status changes from
     `to-implement` to `implemented` or `unsupported`, with the flag and the June citation.
     Update `MODS.md` for each new flag.
- **The census is the exit test.** There is no test suite. Phase 1 produces
  `june2017_re/census.py`, which re-derives "June keys minus implemented parse sites" from
  the source tree. A parser phase is done when its keys leave the `to-implement` bucket.
  A runtime phase is done when its cited branches show up in `JUNE_FX` log lines during a
  play-test.

---

## Investigation ledger — what is already proven (do not redo)

### The binary is readable

- `unpacked_x2.exe` is an unlicense-unpacked dump with intact RTTI and `__FILEW__` paths,
  e.g. `D:\SOURCE\INT\source\X2Lib\X2EffectSet.cpp` at `0x3347e460`.
- `RANOX_EFFECT_ENGINE_GAPS.md` §6 ("packed, zero hits") describes the *packed* `x2.exe`.
  It is outdated for this purpose; Phase 12 rewrites it.
- The Lua key literals and lua_tinker bind names are XOR-encrypted per string. The idiom,
  byte encodings and two hand-verified controls are in `june2017_re/README.md`:
  - buffer `0x3379c56c` → `MAIN_EFFECT_NAME%d`
  - buffer `0x3379c2b8` → `RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT`. Its last byte is
    initialised from a register and decrypted out of order, which is exactly the case the
    prototype gets wrong.
- About 7,086 strings were recovered by the prototype whole-binary scan
  (`june2017_re/xorstrings.tsv`), but the prototype has known flaws:
  - It drops the last character.
  - It merges adjacent buffers.
  - It garbles a few characters.
- Phase 0 replaces it. Until then, treat any decrypted name as a lead, not a fact.

### June function map

The full table, with evidence, is in `june2017_re/README.md`. Summary:

| June VA | March equivalent | Status |
|---|---|---|
| `321f28f0` | `CX2EffectSet::OnFrameMove` | confirmed (calls match `X2EffectSet.cpp:70-73`) |
| `321ee600` | `OnFrameMove_CreateEffect` | confirmed (`__FUNCTION__` string) |
| `321e4020` / `321e0080` / `321dbcc0` | `OnFrameMove_DeleteEffect` / `UpdateEffect` / `StartCameraShake` | confirmed (call order) |
| `321e48e0` | `AddEffectSetTemplet_LUA` (39 KB) | confirmed (bind table in `FUN_31d95080`) |
| `31f7aab0` | `CEffect::CEffect` (20 KB) | confirmed |
| `31f7fc60`, `31f7ff60` | `CreateInstance*` | confirmed (only callers of the ctor) |
| `31f71930` | lock-on loader | confirmed (called by ctor) |
| `31f637e0` | drag loader | confirmed (called by ctor) |
| `31f64200` | sine-move loader | confirmed (called by ctor) |
| `31f73f30` | sound loader | confirmed (called by ctor) |
| `31f6ac60` | die damage-effect loader (`LOOP_*`, `SET_DIE_WHEN_MASTER_EFFECT_DIE`) | confirmed (called by ctor) |
| `31f76a90` | passive-particle loader | confirmed (called by ctor) |
| `31f74a80` | after-image loader | confirmed (called by ctor) |
| `31f66700` | die-condition parser | confirmed (called by ctor) |
| `31f68f30` | hit damage-effect loader | via wrapper `31f6e7e0`; key set only |
| `31f66e60` | particle loader | via four wrappers; key set only |
| `32632390` | `CX2DamageManager::SetDamageData` (11.9k disasm lines) | confirmed |
| `31f88790` | CEffect deleting dtor; CEffect's **only** virtual | confirmed (vtable `3346f008`, COL `33615530`) |
| `31f85910` | `CEffect::OnFrameMove`? | **candidate** (size and call count only) |
| `31f83aa0` | other CEffect runtime? | **candidate** |
| `31c895d0`, `31c8d140` | KTDGXMeshPlayer templet parse? | **candidate** (`__FILEW__` xref only) |
| `31fbe4a0` | — | **not ours**: its strings are UI-guide keys. Excluded |

Because CEffect's only virtual is the destructor, its runtime methods cannot be found
through a vtable. Find them from the call graph of
`CX2DamageEffect::OnFrameMove` (March `X2DamageEffect.cpp:84`), which loops over
instances calling `CEffect::OnFrameMove`.

### Keys June reads that March does not (partial, from the prototype decryptor)

These lists are **leads** until Phase 0 and Phase 1 confirm them against June lua. They
are here so the size of each phase is understood.

- **CEffect ctor:**
  - `RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT`, `ONLY_RELATIONSHIP`
  - `INHERIT_CRITICAL_DAMAGE_RATE`, `INHERIT_CRITICAL_RATE`
  - `TRACE_OWNER_BONE`, `TRACE_OWNER_BONE_ONLY_ROTATE`
  - `FOLLOW_LINE_GRAVITY`, `FOLLOW_LINE_WITH_CUSTOM_ROTATE`
  - `REACT_ANIMSPEED`, `REFRESH_LAND_POS`, `USE_GLOBAL_TIME`, `USE_OWNER_ATTACK_SPEED`
  - `IGNORE_MI_HUGE`, `USE_ADVANCE_COLLISIOIN_CHECK`
  - `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`
  - `APPLIED_SKILL_ID`, `APPLIED_SKILL_ID_NOT_AFFECT_MEMO`
- **Die damage-effect (`31f6ac60`):**
  - `LOOP_INTERVAL`, `LOOP_END_COUNT`, `LOOP_END_TIME`, `LOOP_ADD_POS_X/Y/Z`
  - `SET_DIE_WHEN_MASTER_EFFECT_DIE`
  - `INHERIT_LIFE_TIME`, `INHERIT_DAMAGE`, `INHERIT_BUFF_FACTOR`
  - `SET_GROUP_ID`, `AUTO_DIE`, `APPLY_ROTATED_OFFSET`, `APPLY_ROTATE_ORIGIN`
- **EffectSet parser (`321e48e0`):**
  - Templet level: `SYNC_SHOW_BY_CREATE_UNIT`, `DELETE_EFFECTSET_AT_TRACE_UNIT_DIE`,
    `STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE`, `GROUP_NAME`, `APPLIED_SKILL_ID`,
    `OVERLAP…`, `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`, `NOT_ALLOW_MEMO`
  - Per effect: `POSITION_OFFSET_USE_UNIT_IS_RIGHT`, `TRACE_MATRIX_EXCLUDE_SCALE`,
    `TRACE_MATRIX_IN_FIELD`, `FOLLOW_UNIT_ROTATION`, `FOLLOW_UNIT_ROTATION_C…PARTICLE`,
    `USE_IS_RIGHT_WHEN_CREATE`, `FOLLOW_UNIT_VIEWER_SCALE`,
    `USE_EFFECT_SCALE_IN_UNIT_VIEWER`, `BONE_TRACE_TARGET_MESH_INDEX`
  - Trigger sub-block: `TRIGGER_TIME`, `MOTION_NAME`, `PLAY_TYPE`, `TRIGGER_ONCE`,
    `STOP_TRACING_ON_TRIGGER`, `BLOCK_TRIGGER(_TIME)`,
    `EFFECT_SET_TOGGLE_VISIBILITY_TIMER`, `USE_MESH_TRANSITION`, …
  - Sound and camera-shake sub-blocks.
- **SetDamageData (`32632390`):** about 60 new keys. Examples:
  - `DIVISION_NUM/OFFSET`, `DAMAGE_SKILL_ID`, `REACT_TYPE`
  - `CREATE_DAMAGE_EFFECT_NAME_SCALE_X/Y/Z`, `CREATE_EFFECT_SET_MAIN_EFFECT_LIFTTIME/DELAYTIME`
  - `TRACE_NEAREST_BONE`, `TRACE_UNIT_DIE_DELETE_EFFECT_SET`, `PVP_RATE`
  - `IGNORE_DEFENCE_*`, `STOP_TIME_ATT/DEF`, `HIT_ADD_MP_PERCENT`, `BUFF_TEMPLET_ID`
  - `DAMAGE_TRIGGER_CONDITION`, `DAMAGE_DATA_KEY_NAME`, `ONYL_ATTACK_USER`
  - `LIMIT_APPLY_BUFF_COUNT_EACH_UNIT`, `PAUSE_NPC_AI_TIME`
  - For absent systems: `NATURAL_FORCE_TYPE`, `ROSE_WEAPON_ATTACK_TYPE`, `HP_RATE_DAMAGE_*`,
    `NUMERICAL_DATA_*`, `APPLIED_MEMO_ID`
- **June-only enum tables** (`RANOX_EFFECT_ENGINE_GAPS.md` §2):
  `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`, `DAMAGE_EFFECT_DIE_CONDITION_TYPE`,
  `HP_RATE_DAMAGE_TYPE`, `NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE`, `ROSE_WEAPON_ATTACK_TYPE`.
  The same document's §3 also lists 413 June-only members of existing enums.

`RANOX_EFFECT_ENGINE_GAPS.md` §1's "zero unread EffectSet keys" was measured over the 196
Ranox blocks only. The keys above prove it does not hold for June's data as a whole, so do
not let it shrink scope.

### Dead ends and traps (do not repeat)

- The Ghidra `strings` endpoint only lists strings Ghidra already defined. It will
  **never** show an encrypted key, so it cannot prove absence.
- Plain grep for a key in `X2Lib` is not proof of absence either. Stringizing macros
  generate Lua key names (`CASE_BEHAVIOR_FACTOR`, `X2BuffFactor.cpp:9-22`). Use a
  bare-identifier or stem search.
- Parse `xorstrings.tsv` with a tolerant regex over `latin1`. Some decoded rows contain
  raw tab and newline bytes.
- Printing decoded UTF-16 through the cp1252 console raises. Sanitize first.
- March code landmarks, for orientation:
  - `X2EffectSet.cpp`: parser `:2209`; `OnFrameMove_*` `:77/:964/:1261/:1586`;
    `PlayEffectSet` `:1801`; `GetEffectPosition` `:2772`.
  - `X2DamageEffect.cpp`: ctor `:1240`; `CEffect::OnFrameMove` `:1836`; lock-on `:2752`;
    `DamageReact` `:3400`; `AttackResult` `:3485`; `CreateHitDamageEffect` `:5378`;
    `CreateDieDamageEffect` `:5626`; `Load*` `:6342…`.
  - `X2DamageManager.cpp`: `CollisionDataCheck` `:1740`.
  - `X2Main.cpp`: binds `:4655-4672`.

---

## Phase 0 — RE tooling: a decryptor you can trust, and the function map as data

**Paste with:** Starting context + this section.

**Goal.** Turn the prototype into a tool whose output later phases can cite.

**Deliverables** (all in `june2017_re/`):

1. **`xordecrypt.py`, a new decoder that works on raw bytes, not Ghidra text.**
   - Use a real x86 decoder: `capstone`, installed with `pip install capstone` after the
     user OKs it; Python 3.14 is on this machine and capstone is not installed. If the
     user declines, write a hardened pattern emulator of the idiom instead.
   - Track the guard, the init stores into the static buffer, and the per-byte (or
     per-word, for wide strings) load → XOR/NOT → store sequence. Emit one row per
     **buffer**, not per contiguous address run. Buffer boundaries come from each init
     block's store set, which fixes the "adjacent buffers merge" flaw.
   - Init stores can come **from registers** (`MOV [buf+n],AL` after `MOV AL,imm`), and
     the decrypt can be out of order, with stack-slot offsets shifted by interleaved
     `PUSH`es. The README's worked example of `0x3379c2b8` shows all three. Track register
     values through the init block, and order output bytes by `ESP`-adjusted destination
     slot, not by instruction order.
   - The terminator is the zero-register store. Do not include it, and do not drop the
     last real character.
   - Input: a function address list, or `--all` for a whole-`.text` sweep.
   - Output `xorstrings.tsv` with columns `use_va`, `func_va`, `buf_va`, `kind`
     (`ascii`/`wide`), `len`, `plaintext`.
2. **Controls.** The tool's first act is to decrypt both of these byte-exact. On any
   mismatch it exits non-zero.
   - buffer `0x3379c56c` → `MAIN_EFFECT_NAME%d`
   - buffer `0x3379c2b8` → `RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT` (39 chars)

   Hand-verify one more control of your own: a **wide** string, from any function. Record
   it in the README the same way.
3. **Reconciliation.**
   - Decompile the June effect lua into `june2017_re/june_lua/` (copy first, then run
     `elstools decompile-batch`):
     - `DamageEffect*.lua`, `EffectSet*.lua`, `UIEffectSet*.lua`, `UnitViewerEffectSetTable.lua`
     - `Elsword_DamageData.lua`, `DamageSound.lua`, `Enum.lua`
   - Extract every table key those files use.
   - For each parser function in the function map (`321e48e0`, `31f7aab0`, the
     sub-loaders, `32632390`), compare its decrypted key set against the data-file keys.
   - Write `reconcile.md`, which lists the intersection, the binary-only keys and the
     data-only keys. Explain every data-only key: nested sub-table, key read by a
     different function, or decoder miss.
4. **`funcmap.tsv`** with columns `june_va`, `march_symbol`, `status`
   (`confirmed`/`candidate`), `evidence`. Seed it from `README.md`. Extend it by walking
   the CEffect ctor's sub-loader call block (`0x31f7e7b2`–`0x31f7f976`, in call order) and
   the four particle wrappers.
5. **Skill fix, with OK.** Quote `--noproxy '*'` in `.claude/skills/ghidra/SKILL.md`.
6. **Ghidra annotation, optional, with explicit OK.** Rename the confirmed functions
   (`June_CX2EffectSet_OnFrameMove`, …) and set plate comments with the decrypted keys at
   each decrypt site. Later phases' disassembly then reads by name. List the renames and
   get approval before posting any.

**Exit criteria.**

- The three controls pass.
- Every buffer's decoded length equals the number of bytes its init block stored
  (immediate *and* register stores), minus the terminator.
- `reconcile.md` shows no *unexplained* data-only keys for any parser function.
- No data-only key in `reconcile.md` is a one-character extension of a binary-only key
  (e.g. data `FOO_BAR` vs. binary `FOO_BA`). That is the signature of the
  dropped-last-character bug.
- Do **not** test for "no key is a prefix of another". Real pairs like
  `TRACE_OWNER_BONE` / `TRACE_OWNER_BONE_ONLY_ROTATE` exist.

**Kickoff prompt**:
```
Starting the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md in the
repo root in full -- "Starting context", "Investigation ledger", and "Phase 0" -- and then
june2017_re/README.md. Branch: mods/lanox-port.

Goal: no more guessing code. This phase builds the tooling every later phase cites.

First: check the June dump's size and SHA-256 against the plan, then run the Ghidra health
check with --noproxy '*' QUOTED. If Ghidra isn't reachable, stop and ask me to start it.

Your task is Phase 0. Build june2017_re/xordecrypt.py, a raw-byte XOR-string decoder with
per-buffer boundaries. Ask me before pip installing capstone; if I decline, write a
hardened emulator of the idiom instead. It must decrypt the two controls byte-exact
before anything else. Then decompile the June effect lua into june2017_re/june_lua/ (copy
the files first, never decompile in place in the 14JUN17 directory), reconcile the
decoder's per-function key sets against the keys the data actually uses, and write
reconcile.md and funcmap.tsv. Ask before editing .claude/skills/ghidra/SKILL.md, and ask
(with the full list) before writing any rename or comment into my Ghidra project.

No client code changes in this phase. Commit june2017_re/ when done, with my OK.
```

---

## Phase 1 — Key census and gap matrix

**Paste with:** Starting context + this section.

**Goal.** Make one authoritative table of every June key in scope, and a script that
re-derives it, so each later phase has a measurable finish line.

**Deliverables.**

1. **`june2017_re/census.py`.** For each June key it records:
   - The June data files and templets that use it, with a count.
   - The June parse function and decrypt-site VA, from `xorstrings.tsv`.
   - The March parse site, if any, in `X2Lib`/`KTDXLIB`. Find it by quoted-literal *and*
     bare-identifier/stem search, because of stringizing macros.
   - The sub-table it lives in: `EFFECT%d`, `DAMAGE_DATA`, `DIE_DAMAGE_EFFECT%d`, trigger,
     sound, and so on. The same key name in two sub-tables is two rows.
   - A status: `implemented`, `to-implement(phase N)`, `unsupported(system)`, or
     `binary-only`.

   It must be re-runnable. Later phases use it as their exit check, so it must read the
   current source tree, not a cached list.
2. **`JUNE2017_EFFECT_KEY_MATRIX.md`** in the repo root. Generate it from `census.py`,
   group it by subsystem and sub-table, and assign each `to-implement` key to its phase
   below.
   - Where one March key's June *semantics* differ (default value, type, units), add a
     `semantics-differ` row. Phase 8a and the parser phases own those.
3. **Enum census.** For every June enum table and member that the in-scope data uses,
   record whether `march2014/data036/Enum.src.lua` has it. Also record which phase adds
   the C++ reader that makes the enum member meaningful. Enum members without a reader
   are *not* added. Phase 11 owns the enum data change.
4. **Absent systems list.** Collect the keys that belong to systems this build lacks
   entirely (Rose weapon, natural force, memo, numerical data, HP-rate damage, and any
   others the census turns up). Name the system for each key. For each system, record
   the evidence that it is absent, e.g. "no `CX2RoseWeapon*` class, no enum, no packet".

**Exit criteria.**

- Every key in the in-scope June data has exactly one status.
- `census.py --summary` prints the counts per status and per phase.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context", "Investigation ledger", "Phase 1") and june2017_re/README.md,
reconcile.md, funcmap.tsv (Phase 0's outputs). Branch: mods/lanox-port.

Your task is Phase 1: write june2017_re/census.py and generate
JUNE2017_EFFECT_KEY_MATRIX.md in the repo root. The census has to find March parse sites
by bare identifier / stem as well as quoted literal (X2Lib uses stringizing macros; a
zero-hit literal grep is not proof a key is unread). Classify every in-scope June key as
implemented / to-implement(phase N) / unsupported(system) / binary-only, assign phases
per the plan's phase table, and flag semantics-differ where a key March already reads is
typed, defaulted, or scaled differently in June's parser.

No client code changes in this phase. Commit the census script and the matrix, with my OK.
```

---

## Phase 2 — EffectSet parser, templet level

**Paste with:** Starting context + this section.

- **June source:** `FUN_321e48e0` (`AddEffectSetTemplet_LUA`), from its entry to the start
  of the per-`EFFECT%d` loop.
- **March target:** `CX2EffectSet::AddEffectSetTemplet_LUA` (`X2EffectSet.cpp:2209`) and
  `EffectSetData` in `X2EffectSet.h`.

**Scope.** Every templet-level key the matrix assigns to Phase 2. Examples:
`SYNC_SHOW_BY_CREATE_UNIT`, `DELETE_EFFECTSET_AT_TRACE_UNIT_DIE`,
`STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE`, `GROUP_NAME`, `OVERLAP…`,
`APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`, `APPLIED_SKILL_ID`, `NOT_ALLOW_MEMO`.

**Method.**

1. Walk the June parser's templet section instruction by instruction. For each key, record:
   - the getter it calls (int, float, bool, string, or enum — find the getter function by
     its shape);
   - the default value (the value stored before or instead of the call);
   - the destination (June member offset, **for grouping only**);
   - any post-processing: clamp, scale, lookup.
2. Add March members named after the keys and parse them with March's own `LUA_GET_VALUE`
   family, with June's defaults.
3. Parse only in this phase. Runtime use belongs to Phase 4a/4b. A member that is parsed
   but not yet consumed gets a `// consumed in Phase 4x` note.
4. Behind `SERV_IRUHADEV_JUNE_FX_DEBUG`, log one summary line per templet load,
   `JUNE_FX ES_TEMPLET <name> <key>=<v> ...`, only for templets that set a non-default
   June key.

**Exit criteria.**

- `census.py` shows 0 Phase-2 keys still `to-implement`.
- A clean `US_SERVICE` build.
- After a deploy, and after asking before launch, `JUNE_FX ES_TEMPLET` lines appear for a
  named Ranox set that uses one of these keys. Pick one from the matrix.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" -- especially its numbered rules --, "Investigation ledger", "Phase 2"),
june2017_re/README.md, and the Phase-2 rows of JUNE2017_EFFECT_KEY_MATRIX.md.
Branch: mods/lanox-port.

Your task: transcribe the templet-level keys of June's EffectSet parser (FUN_321e48e0,
entry up to the per-EFFECT loop) into CX2EffectSet::AddEffectSetTemplet_LUA
(X2EffectSet.cpp:2209). For each key record getter type, default, and post-processing
from the disassembly, and cite the June address range in a comment next to the new code.
Parse only -- runtime use is Phase 4. Put it behind a new SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
flag in KTDXLIB/Always.h, with the original code in #else where behaviour is replaced.
X2EffectSet.{h,cpp} are CP949: byte-level edits only, then check file + git diff --stat.

Build US_SERVICE, deploy to X2_offline.exe, then ASK me before launching. Done when
census.py shows no Phase-2 keys left and a JUNE_FX ES_TEMPLET line shows up for a Ranox
set that uses one. Update the matrix and MODS.md.
```

---

## Phase 3 — EffectSet parser, per-effect data and sub-blocks

**Paste with:** Starting context + this section.

- **June source:** `FUN_321e48e0`, from the per-`EFFECT%d` loop to its end, including the
  trigger, sound and camera-shake sub-blocks.
- **March target:** the same function; `EffectData` and its sibling structs in
  `X2EffectSet.h`.

**Scope.** Per-effect keys:
- `POSITION_OFFSET_USE_UNIT_IS_RIGHT`, `TRACE_MATRIX_EXCLUDE_SCALE`, `TRACE_MATRIX_IN_FIELD`
- `FOLLOW_UNIT_ROTATION`, `FOLLOW_UNIT_ROTATION_C…PARTICLE`, `USE_IS_RIGHT_WHEN_CREATE`
- `FOLLOW_UNIT_VIEWER_SCALE`, `USE_EFFECT_SCALE_IN_UNIT_VIEWER`, `BONE_TRACE_TARGET_MESH_INDEX`

Then the trigger, sound and camera-shake sub-block keys, with the exact names from the
matrix.

**The parser is 39 KB, so split the session if needed.** Do the per-effect keys first,
then the sub-blocks. If the conversation is running long, commit after the per-effect
keys, and record in this section exactly where the next session should resume (a June
VA).

**Exit criteria.**

- `census.py` shows 0 EffectSet keys `to-implement`; the exceptions (unsupported or
  binary-only) are named in the matrix.
- A clean build.
- A `JUNE_FX ES_EFFECT` summary line appears for one Ranox set.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 3"), june2017_re/README.md, and
the Phase-3 rows of JUNE2017_EFFECT_KEY_MATRIX.md. Phase 2 is done: the templet-level keys
are parsed under SERV_IRUHADEV_JUNE_EFFECTSET_PARSE. Extend that flag, since it's the same
logical change. Branch: mods/lanox-port.

Your task: transcribe the per-EFFECT%d keys and the trigger / sound / camera-shake
sub-blocks of June's FUN_321e48e0 into CX2EffectSet's parser and EffectData. Parse only;
runtime is Phase 4. Cite June address ranges. Per-effect keys first; if the session runs
long, commit and write the resume VA into Phase 3's section of the plan.

CP949 byte-level edits only. Build US_SERVICE, deploy, ASK before launching. Done when
census.py shows 0 EffectSet keys to-implement. Update the matrix and MODS.md.
```

---

## Phase 4a — EffectSet runtime: creation

**Paste with:** Starting context + this section.

- **June source:** `FUN_321ee600` (`OnFrameMove_CreateEffect`, 17 KB, confirmed). Also
  `PlayEffectSet`'s June counterpart, which you will find by walking up from `321ee600`'s
  callers and down from the EffectSet lua binds. Add it to `funcmap.tsv`.
- **March target:** `CX2EffectSet::OnFrameMove_CreateEffect` (`X2EffectSet.cpp:77`),
  `PlayEffectSet` (`:1801`), `GetEffectPosition` (`:2772`).

**Method.**

- Diff June's control flow against March's for every Phase-2/3 key consumed at creation:
  position offset by facing, trace matrix, follow rotation, bone-trace mesh index,
  sync-show, overlap/group rules, stop-create-on-state-change.
- Also diff existing behaviour. Where June changed how a March key behaves, that is in
  scope here too: record it as `semantics-differ` and transcribe it.
- Each new branch gets a citation.
- Behind the debug flag, one throttled `JUNE_FX ES_CREATE <set> <effect> <key>` line per
  June branch taken.

**Exit criteria.** Every creation-time key has a cited branch, and a play-test log shows
at least one June branch firing on a named Ranox set.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 4a"), june2017_re/README.md,
funcmap.tsv, and JUNE2017_EFFECT_KEY_MATRIX.md. Phases 2-3 are done: all EffectSet keys are
parsed under SERV_IRUHADEV_JUNE_EFFECTSET_PARSE but not yet consumed.
Branch: mods/lanox-port.

Your task: transcribe creation-time behaviour from June's OnFrameMove_CreateEffect
(FUN_321ee600, confirmed) and PlayEffectSet (find its June VA and add it to funcmap.tsv)
into X2EffectSet.cpp:77 / :1801 / :2772, under a new SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
flag. Cite June address ranges for every branch. Re-express June's logic on March's
members, never June's struct offsets. Where June changed how an existing key behaves,
transcribe that too and mark it semantics-differ in the matrix.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when every
creation-time key has a cited branch and a JUNE_FX ES_CREATE line fires on a named Ranox
effect set. Update the matrix and MODS.md.
```

---

## Phase 4b — EffectSet runtime: delete, update, camera shake

**Paste with:** Starting context + this section.

- **June source:** `FUN_321e4020` (delete), `FUN_321e0080` (update, 7 KB), `FUN_321dbcc0`
  (camera shake), all confirmed by call order.
- **March target:** `X2EffectSet.cpp:964/:1261/:1586`.

**Scope.**
- Delete-time keys: `DELETE_EFFECTSET_AT_TRACE_UNIT_DIE`, and the trigger block's
  `STOP_TRACING_ON_TRIGGER`, `BLOCK_TRIGGER*`, visibility toggle.
- Per-frame update keys: follow rotation, viewer scale, trace-matrix-in-field.
- Camera-shake sub-block semantics.
- Plus any existing-key semantics June changed.

**Exit criteria.** As 4a, with `JUNE_FX ES_UPDATE` and `ES_DELETE` lines.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 4b"), june2017_re/README.md,
funcmap.tsv, and the matrix. Phase 4a is done (creation-time behaviour under
SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME; extend that flag). Branch: mods/lanox-port.

Your task: transcribe June's OnFrameMove_DeleteEffect (FUN_321e4020), UpdateEffect
(FUN_321e0080) and StartCameraShake (FUN_321dbcc0) differences into X2EffectSet.cpp:964 /
:1261 / :1586, covering every remaining EffectSet key and any changed semantics of
existing keys. Cite June address ranges.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when the matrix has
no EffectSet key without a runtime citation (or a documented reason it has none), and
JUNE_FX ES_UPDATE / ES_DELETE lines fire. Update the matrix and MODS.md.
```

---

## Phase 5 — DamageEffect CEffect constructor keys

**Paste with:** Starting context + this section.

- **June source:** `FUN_31f7aab0` (20 KB, 4,457 disassembly lines), **excluding** the
  sub-loader call block at `0x31f7e7b2`–`0x31f7f976`, which Phase 6 owns.
- **March target:** `CX2DamageEffect::CEffect::CEffect` (`X2DamageEffect.cpp:1240`) and
  `CEffect` in `X2DamageEffect.h`.

**Scope.** Every top-level CEffect key the matrix assigns here: the list in the ledger,
plus anything Phase 1 added. For keys March already reads, check June's default, type and
post-processing, and fix any that differ (`semantics-differ`).

**Watch for:**
- `FUN_31f8ba80`, June's `"%s"`/`"%s_ULTIMATE"` GetValue wrapper. Some keys have an
  `_ULTIMATE` variant read through it; find out whether March has the same mechanism
  before adding one.
- The two `SetDamageData` calls at `31f7c63f`/`31f7c66c`. Record which `DAMAGE_DATA` keys
  they pass, as a handoff to Phase 8.

**Exit criteria.** `census.py` shows 0 CEffect top-level keys `to-implement`, a clean
build, and a load-time `JUNE_FX CE_TEMPLET` line for a named Ranox damage effect.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 5"), june2017_re/README.md,
funcmap.tsv, and the Phase-5 rows of the matrix. Branch: mods/lanox-port.

Your task: transcribe the top-level keys of June's CEffect constructor (FUN_31f7aab0,
excluding the sub-loader call block 0x31f7e7b2-0x31f7f976 that Phase 6 owns) into
CX2DamageEffect::CEffect::CEffect (X2DamageEffect.cpp:1240), under a new
SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE flag. Cite June address ranges. Check existing keys
for changed defaults/types too. Parse only; runtime is Phase 7. Note what the two
SetDamageData calls at 31f7c63f/31f7c66c pass, for Phase 8.

X2DamageEffect.{h,cpp} are CP949: byte-level edits only, then check file + git diff --stat.
Build, deploy, ASK before launching. Done when census.py shows no Phase-5 keys left.
Update the matrix and MODS.md.
```

---

## Phase 6a — Sub-loaders I: die and hit damage effects, die conditions

**Paste with:** Starting context + this section.

- **June source:**
  - `FUN_31f6ac60`: the die damage-effect loader (`LOOP_*`, `SET_DIE_WHEN_MASTER_EFFECT_DIE`,
    `INHERIT_*`, `SET_GROUP_ID`, `AUTO_DIE`, `APPLY_ROTATED_OFFSET`, `APPLY_ROTATE_ORIGIN`).
  - `FUN_31f68f30`, reached through wrapper `FUN_31f6e7e0`: the hit damage-effect loader.
    Confirm the wrapper's caller first and add it to `funcmap.tsv`.
  - `FUN_31f66700`: the die-condition parser. The `DAMAGE_EFFECT_DIE_CONDITION_TYPE` enum
    values come from the June `Enum.lua`, and the comparison semantics come from this
    function.
- **March target:** `LoadDieDamageEffect` (`:6624`), `LoadHitDamageEffect` (`:6900`), the
  `DamageEffectData` struct, and a new die-condition structure.

**Named test case.** `DAMAGE_EFFECT_Trap_Fire_Big` uses `LOOP_ADD_POS_X/Y` and
`SET_DIE_WHEN_MASTER_EFFECT_DIE`. It is one of the two blocks listed in
`RANOX_EFFECT_ENGINE_GAPS.md` §1. After this phase it must parse completely.

**Exit criteria.**

- The census is clean for these three sub-tables.
- A load-time `JUNE_FX DDE_TEMPLET DAMAGE_EFFECT_Trap_Fire_Big LOOP_ADD_POS_X=...` line
  appears.
- The runtime (the loop spawn itself) belongs to Phase 7c. Note that in the code.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 6a"), june2017_re/README.md,
funcmap.tsv, and the matrix. Phase 5 is done (CEffect top-level keys under
SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE; extend that flag). Branch: mods/lanox-port.

Your task: transcribe June's die damage-effect loader (FUN_31f6ac60), hit damage-effect
loader (FUN_31f68f30, via wrapper FUN_31f6e7e0 -- confirm its caller first), and
die-condition parser (FUN_31f66700) into X2DamageEffect.cpp's LoadDieDamageEffect (:6624) /
LoadHitDamageEffect (:6900) and their structs. Take the DAMAGE_EFFECT_DIE_CONDITION_TYPE
values from June's Enum.lua. Parse only; the loop and condition runtime is Phase 7c. Cite
June address ranges.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when
DAMAGE_EFFECT_Trap_Fire_Big parses completely (JUNE_FX DDE_TEMPLET line) and the census
is clean for these sub-tables. Update the matrix and MODS.md.
```

---

## Phase 6b — Sub-loaders II: particle, lock-on, sound, after-image, drag, sine move and the small loaders

**Paste with:** Starting context + this section.

- **June source:**
  - Particle: `FUN_31f66e60` and its four wrappers (`31f6a7b0`, `31f6a940`, `31f6aad0`,
    `31f6cd00`).
  - Passive particle: `31f76a90`, and `31f6ceb0` via `31f6e850`.
  - Lock-on: `31f71930`, with the `NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE` enum and
    `AT_PVP/AT_FIELD/AT_RAID_FIELD/AT_DUNGEON`.
  - Sound: `31f73f30`.
  - After-image: `31f74a80`.
  - Drag: `31f637e0`.
  - Sine move: `31f64200`.
  - The small loaders `31f64c80` (`EFFECT_SET_NAME`/`EVENT_TIME`), `31f650f0`, `31f65730`
    (color/screen?) and `31f66230` (`NPC_`/`EVENT_TIME`). Identify each one and add it to
    `funcmap.tsv`.
- **March target:** `LoadParticleTable`, `LoadLockOnData` (`:6342`), `LoadSoundData`,
  `LoadAfterImage`, `LoadSinMoveData`, and the rest of the `Load*` family.

**Named test case.** `DamageEffect_SA_RHB_Lockon_Stratos_Attack` for lock-on. Its
`NUMERICAL_DATA_INDEX`/`APPLIED_SKILL_ID` are unsupported systems: parse, store and log.

**Exit criteria.** `census.py` shows 0 DamageEffect keys `to-implement`, with named
exceptions.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 6b"), june2017_re/README.md,
funcmap.tsv, and the matrix. Phases 5 and 6a are done (extend
SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE). Branch: mods/lanox-port.

Your task: transcribe the remaining CEffect sub-loaders -- particle (FUN_31f66e60 + its 4
wrappers), passive particle (31f76a90, 31f6ceb0), lock-on (31f71930), sound (31f73f30),
after-image (31f74a80), drag (31f637e0), sine move (31f64200), and the small loaders at
31f64c80 / 31f650f0 / 31f65730 / 31f66230 (identify each and add it to funcmap.tsv) -- into
X2DamageEffect.cpp's Load* family. Parse only. Unsupported-system keys (numerical data,
memo, skill-ID-driven systems) get parse + store + one JUNE_FX unsupported log line. Cite
June address ranges.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when census.py
shows 0 DamageEffect keys to-implement. Update the matrix and MODS.md.
```

---

## Phase 7a — DamageEffect runtime: identify, then OnFrameMove, trace and follow-line

**Paste with:** Starting context + this section.

**Identification comes first, and nothing is transcribed until it is done.**

- Find June's `CX2DamageEffect::OnFrameMove` (March `:84`). It loops over instances
  calling `CEffect::OnFrameMove`, then deletes the dead ones.
- Go down from it to `CEffect::OnFrameMove`, then to its callees:
  `OnFrameMove_SoundData`, `BlackHoleData`, `LockOnData`, `SineMoveData`, `Trace`,
  `FollowLine`.
- `31f85910` (11.7 KB, 248 calls) is the leading candidate for `CEffect::OnFrameMove` and
  `31f83aa0` a secondary one. **Confirm them or replace them.**
  - Evidence that counts: the call structure matching March, and the decrypted strings.
  - Evidence that does not count: size alone.
- Record everything in `funcmap.tsv` as `confirmed`.

**Then transcribe** the per-frame semantics for the Phase-5 keys:
- `TRACE_OWNER_BONE*`, `FOLLOW_LINE_GRAVITY`, `FOLLOW_LINE_WITH_CUSTOM_ROTATE`
- `REFRESH_LAND_POS`, `USE_GLOBAL_TIME`, `USE_OWNER_ATTACK_SPEED`, `IGNORE_MI_HUGE`
- `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`

Also transcribe any changed semantics in existing per-frame code.

**Exit criteria.**
- The runtime functions are confirmed in `funcmap.tsv`.
- Each key has a cited branch.
- A `JUNE_FX CE_FRAME` line fires in a play-test.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 7a"), june2017_re/README.md,
funcmap.tsv, and the matrix. Phases 5-6 are done: all DamageEffect keys are parsed but not
consumed. Branch: mods/lanox-port.

Your task, step 1 (no code): identify June's CX2DamageEffect::OnFrameMove and
CEffect::OnFrameMove and its per-frame callees by call graph. FUN_31f85910 is only a
size-based candidate -- confirm it or replace it with evidence, then record everything in
funcmap.tsv. Step 2: transcribe the per-frame behaviour of the Phase-5 keys (trace owner
bone, follow-line gravity/rotation, refresh land pos, global time, owner attack speed,
alpha-by-option, ...) and any changed semantics into X2DamageEffect.cpp:1836 and its
helpers, under a new SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME flag. Cite June address
ranges.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when each key has
a cited branch and JUNE_FX CE_FRAME fires. Update the matrix and MODS.md.
```

---

## Phase 7b — Runtime: lock-on

**Paste with:** Starting context + this section.

- **June source:** June's `OnFrameMove_LockOnData`, `SetLockOnNearstTarget` and
  `VectorLockOn` (identified in 7a; if not, identify them first).
- **March target:** `X2DamageEffect.cpp:2752`, `:7341`, `:6465`.

**Scope.** The Phase-6b lock-on keys:
- `NOT_LOCKON_INVINCIBLE_TARGET`, `INIT_TARGET`, `RANDOM_S…`
- the per-game-type `AT_*` flags
- `LOCK_ON_TARGET_BONE_NAME`, `LOCK_ON_OFFSET_Y_*`, `NOT_FIND_TARGET_MOVE_STOP`
- the `NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE` semantics

**Named test case.** `DamageEffect_SA_RHB_Lockon_Stratos_Attack`.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 7b"), june2017_re/README.md,
funcmap.tsv, and the matrix. Phase 7a is done (extend
SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME). Branch: mods/lanox-port.

Your task: transcribe June's lock-on runtime (OnFrameMove_LockOnData,
SetLockOnNearstTarget, VectorLockOn -- VAs in funcmap.tsv, or identify them first) into
X2DamageEffect.cpp:2752 / :7341 / :6465 for every Phase-6b lock-on key, including the
NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE semantics. Cite June address ranges.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when a play-test
shows JUNE_FX CE_LOCKON lines on DamageEffect_SA_RHB_Lockon_Stratos_Attack (or another
named lock-on effect from the matrix). Update the matrix and MODS.md.
```

---

## Phase 7c — Runtime: hit/die damage-effect creation, LOOP_*, die conditions

**Paste with:** Starting context + this section.

- **June source:** June's `CreateHitDamageEffect` / `CreateDieDamageEffect`, found from the
  sub-effect data consumers, plus the die-condition evaluation site: whatever reads the
  structure `31f66700` filled.
- **March target:** `X2DamageEffect.cpp:5378`, `:5626`, `SetDieDamageEffect` (`:4931`).

**Scope.**
- `LOOP_INTERVAL/END_COUNT/END_TIME/ADD_POS_X/Y/Z`: how June schedules repeated spawns and
  steps their position (per spawn? cumulative? in facing space?). **Read it; don't
  assume.**
- `SET_DIE_WHEN_MASTER_EFFECT_DIE`, `INHERIT_*`, `SET_GROUP_ID`, `AUTO_DIE`,
  `APPLY_ROTATED_OFFSET`, `APPLY_ROTATE_ORIGIN`.
- The die conditions.

**Named test case.** `DAMAGE_EFFECT_Trap_Fire_Big`. The log must show N spawns at the
stepped positions, and the N and the step must match the Lua values through June's
formula.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 7c"), june2017_re/README.md,
funcmap.tsv, and the matrix. Phases 6a and 7a are done (extend
SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME). Branch: mods/lanox-port.

Your task: find June's CreateHitDamageEffect / CreateDieDamageEffect and the
die-condition evaluation site (the consumer of what FUN_31f66700 parses), then transcribe
the LOOP_* spawn scheduling and position stepping, SET_DIE_WHEN_MASTER_EFFECT_DIE,
INHERIT_*, SET_GROUP_ID, AUTO_DIE, the rotated-offset/origin keys, and the die conditions
into X2DamageEffect.cpp:5378 / :5626 / :4931. Read the loop math exactly -- per-spawn vs
cumulative, local vs world space -- and cite June address ranges.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when
DAMAGE_EFFECT_Trap_Fire_Big logs JUNE_FX CE_LOOP spawns whose count and positions match
the Lua values through June's formula. Update the matrix and MODS.md.
```

---

## Phase 7d — Runtime: DamageReact, AttackResult

**Paste with:** Starting context + this section.

- **June source:** June's `DamageReact` / `AttackResult`, found from the collision
  consumers or from CEffect's own call graph.
- **March target:** `X2DamageEffect.cpp:3400`, `:3485`.

**Scope.**
- `RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT`, `REACT_ANIMSPEED`, `ONLY_RELATIONSHIP`
- `INHERIT_CRITICAL_*`, `USE_ADVANCE_COLLISIOIN_CHECK`
- any changed semantics of the existing hit-response code

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 7d"), june2017_re/README.md,
funcmap.tsv, and the matrix. Phases 7a-7c are done (extend
SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME). Branch: mods/lanox-port.

Your task: identify June's CEffect::DamageReact and AttackResult, then transcribe the
hit-response keys (RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT, REACT_ANIMSPEED,
ONLY_RELATIONSHIP, INHERIT_CRITICAL_*, USE_ADVANCE_COLLISIOIN_CHECK -- the typo is the
studio's) and changed semantics into X2DamageEffect.cpp:3400 / :3485. Cite June address
ranges. If USE_ADVANCE_COLLISIOIN_CHECK reaches into the DamageManager collision path,
record what it needs and hand it to Phase 9 rather than widening this phase.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when every CEffect
key in the matrix is implemented or classified. Update the matrix and MODS.md.
```

---

## Phase 8a — SetDamageData I: existing keys, June semantics

**Paste with:** Starting context + this section.

- **June source:** `FUN_32632390` (11,918 disassembly lines). **Only** the keys March
  already parses.
- **March target:** `CX2DamageManager::SetDamageData` in `X2DamageManager.cpp`.

**Method.**
- For each March-parsed `DAMAGE_DATA` key, find its June read: default, getter type,
  post-processing, and conditions around it such as a PvP branch.
- Record each key as `same` or `semantics-differ` in the matrix.
- Fix the differences, with citations.
- Hand off the list of *new* keys to 8b/8c in their exact June read order, with their
  June VA ranges.

This function has 9 callers across the binary, so it is shared (skills, NPC scripts). A
semantic change here moves every damage number in the game. Log every changed default in
`MODS.md` with the before and after values.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 8a"), june2017_re/README.md, and
the DamageData rows of the matrix. Branch: mods/lanox-port.

Your task: walk June's SetDamageData (FUN_32632390, ~12k disasm lines) and, ONLY for the
keys March already parses, compare default / type / post-processing / surrounding
conditions against CX2DamageManager::SetDamageData. Mark each same or semantics-differ in
the matrix, and fix the differences under a new SERV_IRUHADEV_JUNE_DAMAGEDATA flag with
June citations. This function is shared by every damage source in the game: list every
changed default in MODS.md with before/after values. Also produce the ordered list of NEW
keys with their June VA ranges, for Phases 8b/8c.

X2DamageManager.{h,cpp} are CP949: byte-level edits only. Build, deploy, ASK before
launching. Update the matrix and MODS.md.
```

---

## Phase 8b — SetDamageData II: new keys whose systems exist

**Paste with:** Starting context + this section.

**Scope.** The 8a handoff list, filtered to systems this build has. Examples, to be
verified against the census:
- `DIVISION_NUM/OFFSET`, `DAMAGE_SKILL_ID`, `REACT_TYPE`
- `CREATE_DAMAGE_EFFECT_NAME_SCALE_X/Y/Z`, `CREATE_EFFECT_SET_MAIN_EFFECT_LIFTTIME/DELAYTIME`
- `TRACE_NEAREST_BONE`, `TRACE_UNIT_DIE_DELETE_EFFECT_SET`, `PVP_RATE`
- `IGNORE_DEFENCE_ON_PVP/_RATE/_RATE_ON_PVP/_DECLINE`, `IGNORE_STATE_DEFENCE`
- `STOP_TIME_ATT/DEF`, `HIT_ADD_MP_PERCENT`, `BUFF_TEMPLET_ID`
- `LIMIT_APPLY_BUFF_COUNT_EACH_UNIT`, `PAUSE_NPC_AI_TIME`, `DAMAGE_TRIGGER_CONDITION`
- `DAMAGE_DATA_KEY_NAME`, `ONYL_ATTACK_USER`, `NO_BUFF`, `ONLY_BASIC`

**Parse and consume both, in this phase.** Each key's consumer is usually in
`X2DamageManager.cpp` (hit resolution) or in the unit's damage-taken path. Find June's
consumer with `xrefs` on the member's write site, and transcribe it.

**Split rule.** If one conversation can't hold it all, do the damage-math keys first
(ignore-defence, PvP rate, division), commit, and write the resume point here.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 8b"), june2017_re/README.md,
funcmap.tsv, and the matrix. Phase 8a is done and produced the ordered new-key list
(extend SERV_IRUHADEV_JUNE_DAMAGEDATA). Branch: mods/lanox-port.

Your task: for each new DamageData key whose system exists in this build, transcribe both
June's parse (FUN_32632390 range from 8a's list) and June's consumer (find it via xrefs on
the June member write), citing both. Damage-math keys first; if the session runs long,
commit and write the resume point into Phase 8b's section.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when census.py
shows no DamageData keys to-implement except Phase 8c's absent-system list. Update the
matrix and MODS.md.
```

---

## Phase 8c — SetDamageData III: absent systems, and the enum tables so June lua loads

**Paste with:** Starting context + this section.

**Scope.**
- `ROSE_WEAPON_ATTACK_TYPE`, `NATURAL_FORCE_TYPE`, `HP_RATE_DAMAGE_VALUE/TYPE`,
  `NUMERICAL_DATA_INDEX/NAME`, `APPLIED_MEMO_ID`, `APPLIED_SKILL_ID`,
  `CAN_ACTIVATE_NASOD_ARMOR_COMBO`, `HIT_ADD_COMBINATION_POINT`, and whatever else the
  census marked `unsupported`.
- Each gets parse and store, one `JUNE_FX unsupported` log line per key per templet, and
  **no behaviour**.
- Confirm the "system absent" verdict before logging it. Search for the system's classes,
  enums and packets. If the system turns out to exist, move the key to 8b's rules instead.

**Enum tables.** Whether a key is parsed as an enum or an int, the June data references
`ROSE_WEAPON_ATTACK_TYPE.*`, `HP_RATE_DAMAGE_TYPE.*` and so on. Without the table the Lua
chunk errors on nil indexing, or reads nil silently.
- Decide by reading how `LUA_GET_VALUE_ENUM` behaves on a nil table in this build.
- Hand the table additions to Phase 11 with exact values from June's `Enum.lua`. Do not
  edit `Enum.src.lua` here.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 8c"), june2017_re/README.md, and
the matrix. Phases 8a-8b are done (extend SERV_IRUHADEV_JUNE_DAMAGEDATA).
Branch: mods/lanox-port.

Your task: for every DamageData key the census marked unsupported(system), first confirm
the system really is absent from this build (classes / enums / packets). Then transcribe
June's parse and store it, with one JUNE_FX unsupported log line per key per templet and
no behaviour. Work out from this build's LUA_GET_VALUE_ENUM what a missing enum table
does, and hand exact table values from June's Enum.lua to Phase 11 -- don't edit
Enum.src.lua in this phase.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when census.py
reports 0 unclassified DamageData keys. Update the matrix and MODS.md.
```

---

## Phase 9 — Collision and hit path (answers RANOX_EFFECT_RUNTIME_UNKNOWNS §4)

**Paste with:** Starting context + this section.

- **June source:** June's `CX2DamageManager::CollisionDataCheck` and
  `CheckCollisionGuard`, plus the attack-sphere/line construction.
  - Find them via `SetDamageData`'s neighbours in the `0x3263xxxx` region, the
    `X2DamageManager.cpp` `__FILEW__` string if present, and the callers of June's
    `KTDGXSkinAnim` collision helpers (March `KTDGXSkinAnim.cpp:556-568`).
- **March target:** `X2DamageManager.cpp:1740` and the chain documented in
  `RANOX_EFFECT_RUNTIME_UNKNOWNS.md`: `KTDGXSkinAnim.cpp`, and
  `KTDXDeviceXSkinMesh.cpp:770-878`, which builds attack data from the
  `ATTACK_LINE_START/END` and `ATTACK_SPHERE` frames.

**Scope.** Answer each open item with a June citation, and implement the differences:
- §4.2: the `CollisionDataCheck`/`CheckCollisionGuard` bodies.
- §4.3: whether the radius scales with instance scale.
- §4.4: what happens to a zero-radius attack datum.
- §4.5: `FIX_EFFECTTOUNIT01`.
- §4.6: the direction of the effect-to-NPC test.
- §4.7: the units of `ATTACK_TIME_START/END`.
- §4.8: whether `USE_CANNONBALL` is user-only.
- `USE_ADVANCE_COLLISIOIN_CHECK`, if Phase 7d handed it over.

**Named test case.** The Ranox projectile that "renders but hits nothing". Pick it from
`RANOX_EFFECT_RUNTIME_UNKNOWNS.md`.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 9"), RANOX_EFFECT_RUNTIME_UNKNOWNS.md
in full, june2017_re/README.md, and funcmap.tsv. Branch: mods/lanox-port.

Your task: identify June's CollisionDataCheck / CheckCollisionGuard and the attack
sphere/line construction, then answer each open item in RANOX_EFFECT_RUNTIME_UNKNOWNS.md
section 4 (4.2-4.8) with a June citation, and transcribe the differences into
X2DamageManager.cpp:1740 and the chain it documents, under a new
SERV_IRUHADEV_JUNE_COLLISION flag. Also finish USE_ADVANCE_COLLISIOIN_CHECK if Phase 7d
handed it over. Update RANOX_EFFECT_RUNTIME_UNKNOWNS.md with the answers.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when every
section-4 item is answered with a citation and the Ranox projectile named there hits in a
play-test (JUNE_FX HIT lines). Update the matrix and MODS.md.
```

---

## Phase 10 — KTDGXMeshPlayer templet keys

**Paste with:** Starting context + this section.

- **June source:** the KTDGXMeshPlayer templet parser. Candidates are `31c895d0` and
  `31c8d140`, which are only known to reference the `KTDGXMeshPlayer.cpp` `__FILEW__`
  string. Confirm which one parses the `.txt` templet.
  - Its key literals may be plaintext, because the `.txt` format predates the Lua
    encryption. Check `xorstrings.tsv` and plain strings both.
- **March target:** `KTDXLIB/KTDGXMeshPlayer.cpp`.
- **Data:** June's plaintext `GameMajorXMeshPlayer*.txt`, `GameMinorXMeshPlayer.txt` and
  `UIMajorXMeshPlayer*.txt`, against March's own copies.

**Scope.**
- `RANOX_EFFECT_RUNTIME_UNKNOWNS.md` §4.1: `UseLand`, `CrashLand`, `Gravity`,
  `ElasticCoeff`, `ElasticCoeff_X`, `NOT_CHECK_LAND_TIME`, `LifeType`, `LightPos`,
  `DrawCount`.
- Every other key June's `.txt` files use that March's parser does not read.
- Extend `census.py` with a MeshPlayer mode.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 10"), RANOX_EFFECT_RUNTIME_UNKNOWNS.md
section 4.1, june2017_re/README.md, and funcmap.tsv. Branch: mods/lanox-port.

Your task: confirm which of FUN_31c895d0 / FUN_31c8d140 (or another function) is June's
KTDGXMeshPlayer .txt templet parser, extend census.py with a MeshPlayer mode over June's
plaintext *XMeshPlayer*.txt files, and transcribe every key March's
KTDXLIB/KTDGXMeshPlayer.cpp doesn't read -- parse and its runtime use -- under a new
SERV_IRUHADEV_JUNE_MESHPLAYER flag, with June citations. KTDGXMeshPlayer.cpp is CP949:
byte-level edits only.

Build (KTDXLIB changes need the full chain), deploy, ASK before launching. Done when the
census shows 0 MeshPlayer keys to-implement. Update the matrix and MODS.md.
```

---

## Phase 11 — Enums, data, and the June-data trial load

**Paste with:** Starting context + this section.

1. **Enum data.** Add to `march2014/data036/Enum.src.lua` the June enum tables and members
   that now have C++ readers: the Phase 1 enum census, filtered by what Phases 2–10
   implemented.
   - Take the values verbatim from June's `Enum.lua`.
   - Preserve the UTF-8 BOM and CRLF byte for byte.
   - Add a comment naming this plan.
   - Then compile with `elstools compile-batch -v mar-2014`. **The user packs it.** Name
     the exact file(s) when reporting.
2. **Trial load.** Point the client at June's own `EffectSet*.lua` and
   `DamageEffect*.lua`, recompiled `-v mar-2014`, instead of the merged March copies.
   - Run it from a **separate copy of the game directory**, never `$DATA`. Copy
     everything except `els_db.sql*`, and give the copy its own save.
   - CLAUDE.md forbids loose copies in the game directory. That rule exists because a
     loose file masks a failed repack, and it applies to the real `$DATA`. In the
     throwaway copy, loose-file precedence is acceptable, but only because the copy is
     deleted afterwards. That precedence exists under `US_SERVICE` too for now, since it
     still defines `_IN_HOUSE_` and not `MASS_FILE_FIRST`.
   - Do not build `US_INTERNAL` unless the user asks for it.
   - Alternatively, pack a trial `data036.kom` for the copy. Ask the user which they
     prefer.
   - Record every load error in `OpenScriptFile.log`/`ErrorLog.txt` and every unread key
     the census still reports. Delete the copy when done, after asking.
3. **Report.** Write `JUNE2017_TRIAL_LOAD.md` with the results. The user decides whether to
   adopt June's files wholesale, or keep the merge.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context", "Phase 11") and JUNE2017_EFFECT_KEY_MATRIX.md. Branch:
mods/lanox-port.

Your task: (1) add the June enum tables/members that now have C++ readers to
march2014/data036/Enum.src.lua, values verbatim from June's Enum.lua, BOM and CRLF
preserved, compiled with elstools compile-batch -v mar-2014 -- I pack it; name the exact
file for me. (2) Trial-load June's own EffectSet*/DamageEffect* lua against the new engine
from a SEPARATE COPY of the game directory (never the live one, never my els_db.sql).
Ask me whether to use loose files or a trial .kom in that copy, don't build US_INTERNAL
unless I ask, and ask before deleting the copy afterwards.
Record every load error and every key still unread. (3) Write JUNE2017_TRIAL_LOAD.md.
Ask before launching the client, every time.
```

---

## Phase 12 — Docs and memory

**Paste with:** Starting context + this section.

- **`RANOX_EFFECT_ENGINE_GAPS.md`:**
  - Rewrite §6: the packed `x2.exe` vs. `unpacked_x2.exe`, the XOR-string encryption, and
    a pointer to `june2017_re/`.
  - Close the §1 items (both named blocks) and the §2 enum tables with the phase that
    closed each.
  - Revisit the Phase C `ATTACH_SKIN_MESH` inference and its `VALUE=0.01` against June's
    code, if Phase 3 or 4 reached it. Otherwise open a follow-up.
- **`RANOX_EFFECT_RUNTIME_UNKNOWNS.md`:** confirm that every §4 item carries its Phase 9
  answer.
- **`MODS.md`:** every `SERV_IRUHADEV_JUNE_*` flag, what it changes, and how to revert it.
  Remove `SERV_IRUHADEV_JUNE_FX_DEBUG` if the user agrees.
- **`CLAUDE.md`:** a short pointer to this plan and `june2017_re/`, in the architecture
  section next to *Offline mode*, with the user's OK.
- **Memory:** record the non-obvious facts:
  - the June binary is readable (dump path and hash);
  - the XOR idiom;
  - CEffect's runtime methods are not virtual;
  - `SetDamageData` is shared, so a semantic change there moves every damage number.

**Kickoff prompt**:
```
Finishing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Phase 12"), JUNE2017_EFFECT_KEY_MATRIX.md, and MODS.md. Branch: mods/lanox-port.

Your task: bring the docs in line with the code. Rewrite RANOX_EFFECT_ENGINE_GAPS.md
section 6 and close sections 1-2 with the phases that closed them; revisit the Phase C
ATTACH_SKIN_MESH inference against June's code; confirm RANOX_EFFECT_RUNTIME_UNKNOWNS.md
section 4 is fully answered; make MODS.md list every SERV_IRUHADEV_JUNE_* flag with revert
steps (ask me before removing SERV_IRUHADEV_JUNE_FX_DEBUG); ask before adding the CLAUDE.md
pointer; save the non-obvious facts to memory.
```
