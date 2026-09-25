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

**End goal (the user's, stated 2026-09-25).**
- The March engine should become as close to the June engine as possible, so that
  later June features port with little friction. This plan is the first subsystem done
  that way: a trial of the approach, on effects.
- For effects, the target is to **adopt June's own files**: `EffectSet*`/`DamageEffect*`
  lua and the assets they reference. The Ranox-merged March copies are a stopgap to be
  retired, not maintained.
- Consequences for every phase:
  - Match June's behaviour, not just what the current merged data exercises.
  - A log check that cannot fire on the merged data is settled at Phase 11, where
    June's data carries the keys. Phase 2's `ES_TEMPLET` check is the first case.

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
    of Ghidra. Inside a variable run as `$C`, quotes don't help. The skill file therefore
    uses `--noproxy 127.0.0.1`, since Phase 0.
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
  - **For June (`KL\x82`) files, use `june2017_re/ljd_jun2017.py`** on the `.std.luac`
    that `decompile-batch` leaves behind. elstools 1.0.0 mis-maps KOG opcodes
    `0x2f`–`0x37` in both directions, so its decompile fails on any `for … in` loop, and
    `compile-batch -v jun-*` emits broken bytecode. `mar-2014` is unaffected. See
    `june2017_re/README.md`, "June LuaJIT opcode table".
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
| `31f71930` | `LoadLockOnData` (`LOCK_ON_DATA`) | confirmed (called by ctor) |
| `31f637e0` | `LoadBlackHoleData` (`BLACKHOLE_DATA`; its keys are `DRAGGING_*`) | confirmed (called by ctor) |
| `31f64200` | `LoadSinMoveData` (`SIN_MOVE_DATA`) | confirmed (called by ctor) |
| `31f73f30` | `LoadSoundData` (`SOUND%d`) | confirmed (called by ctor) |
| `31f6ac60` | `LoadDamageEffect`: the **`CREATE_DAMAGE_EFFECT%d`** loader (`LOOP_*`, `SET_DIE_WHEN_MASTER_EFFECT_DIE`, `INHERIT_*`) | confirmed (called by ctor) |
| `31f76a90` | **`TOUCH_LAND_DAMAGE_EFFECT`** loader (June-only) | confirmed (called by ctor) |
| `31f74a80` | `LoadAfterImage` (`AFTER_IMAGE`) | confirmed (called by ctor) |
| `31f66700` | die-condition parser (`DAMAGE_EFFECT_DIE_CHECK_CONDITION`, `CONDITION_%d`) | confirmed (called by ctor) |
| `31f68f30` | `LoadDieDamageEffect` per-entry body, via wrapper `31f6e7e0` (`DIE_DAMAGE_EFFECT`) | confirmed (ctor → `31f6e7e0` @`31f7e7f7`) |
| `31f6ceb0` | `LoadHitDamageEffect` per-entry body, via wrapper `31f6e850` (`HIT_DAMAGE_EFFECT`) | confirmed (ctor → `31f6e850` @`31f7e84d`) |
| `31f66e60` | `LoadParticleTable`, via `START_`/`PASSIVE_`/`TIME_`/`HIT_PARTICLE` wrappers | confirmed |
| `32632390` | `CX2DamageManager::SetDamageDataFromLUA` (11.9k disasm lines; this plan calls it "SetDamageData") | confirmed |

**Phase 0 corrected this table** (2026-09-24). Sub-table names turned out to be plaintext, and
each loader's role is pinned by the one it opens. `31f6ac60` is the `CREATE_DAMAGE_EFFECT`
loader, not the die loader. `31f68f30` is the **die** body, not the hit one. `31f76a90` is
`TOUCH_LAND_DAMAGE_EFFECT`, not passive particles. `june2017_re/funcmap.tsv` is
authoritative, with 46 rows of evidence; this table only summarises it.
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
- **`CREATE_DAMAGE_EFFECT%d` (`31f6ac60`):**
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

**Status: done 2026-09-24.** Outputs are in `june2017_re/`: `xordecrypt.py` →
`xorstrings.tsv` (6,180 of 6,240 buffers `ok`; three controls, one of them wide),
`ljd_jun2017.py` (all 14 June files decompiled), `reconcile.py` → `reconcile.md`, and
`funcmap.tsv`. Read `june2017_re/README.md` for what changed in the function map.

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

**Status: done 2026-09-25.** Outputs: `june2017_re/census.py` (about 3 minutes, all checks
pass, deterministic) → `JUNE2017_EFFECT_KEY_MATRIX.md` + `june2017_re/census.tsv`. Read
`june2017_re/README.md`, "census.py", before running it. Things the next phases should know:

- **Phase 0's CEffect "378 of 378" was a matcher bug.** The CEffect top level reads 58 of its
  378 data keys directly, and `reconcile.py` is fixed.
- **Keys are placed by the level June reads them at.** Many top-level CEffect keys (`HIT_GAP`,
  `ATTACK_TYPE`, …) and templet-level EffectSet keys (`START_TIME`, `ROTATE_Z`, …) are
  misplaced data that June ignores (`june-ignores`). Don't implement them at that level.
- **There is a fifth status, `june-ignores`.** It covers data June never reads: typos, dead or
  misplaced keys, positional entries, and keys under a table June never opens.
- **The census verifies absent-system verdicts:**
  - Nasod armor combo is compiled in, so its key is 8b work, not 8c.
  - Natural force is gated off (`ADD_RENA_SYSTEM`).
  - Skill memo is **on** under `US_SERVICE` (`ADD_MEMO_1ST_CLASS`).
- **Scopes no phase owns** are listed in the matrix's "Open questions": nested `BUFF_FACTOR`,
  `Elsword_DamageData.lua`, and the EFFECT_TOOL-only `EffectSet_Description.lua`.

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

**Status: done 2026-09-25**, verified in a play-test. `ErrorLog.txt` showed:
- `JUNE_FX ES_LOAD EffectSet.lua loaded=1 templets=3134 with_june_templet_key=0`. 3134
  equals the count from `march2014/data036/EffectSet.src.lua`, so the packed `data036.kom`
  matches the tree.
- `UIEffectSet.lua` 52, `UIEffectSet_INT.lua` 6 and `EffectSet_INT.lua` 46, all loaded,
  none with a June key.
- No `ES_TEMPLET` or `unsupported` lines, as expected.
- The `ES_TEMPLET` check for data that actually uses the keys moves to Phase 11 (see
  below).
`SERV_IRUHADEV_JUNE_EFFECTSET_PARSE` (+ `SERV_IRUHADEV_JUNE_FX_DEBUG`). Things the next
phases should know:

- **The templet level is ten keys, not the list in *Scope* below.** June reads
  `NAME`, `EVENT_TIMER_TYPE`, `STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE`,
  `AFFECTED_BY_UNIT_SCALE`, `DELETE_SHAKE_COUNT`, `DELETE_EFFECTSET_AT_TRACE_UNIT_DIE`,
  `OVERLAP`, `APPLIED_SKILL_ID`, `SYNC_SHOW_BY_CREATE_UNIT` and
  `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`. `GROUP_NAME` and `NOT_ALLOW_MEMO` are not
  read here: `NOT_ALLOW_MEMO` is Phase 3's.
- **No deployed set uses any of the six new keys.** The 196 Ranox blocks carry none of
  them, and `march2014/data036/EffectSet.src.lua` has no occurrence at all. The three
  June sets that do use one and also exist in March
  (`EffectSet_KILL_ENEMY_WHEN_FULL_HYPER_MODE`, `EffectSet_ANCIENT_BONE_DRAGON_SLEEP`,
  `EffectSet_Wedding_Heart_06`) are March's own versions, without the key.
  - So the "`ES_TEMPLET` line for a Ranox set" criterion cannot fire on current data.
    **The user's decision: it is settled in Phase 11**, where June's own `EffectSet.lua`
    uses all six keys.
  - A `JUNE_FX ES_LOAD` summary line per script file shows that the block ran.
  - Don't port June's `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` sets to test it:
    `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION.AEACBO_*` indexes an enum table March's
    `Enum.lua` lacks (Phase 11), and the nil index would abort the whole chunk.
- **Load-time `JUNE_FX` lines go to `ErrorLog.txt`.** `EffectSet.lua` loads in
  `CX2StateStartUp::LoadData`, before the offline server opens `offline_server.log`
  (it opens on first connect).
- **census.py changed** so rule-5 parse sites don't count as evidence that a system
  exists; see `june2017_re/README.md`.

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

**Status: done 2026-09-26**, verified in a play-test. Everything was done in one session,
so there is no resume VA.

`ErrorLog.txt` showed:

| File | `loaded` | `templets` | `effects` | `with_june_effect_key` | `sounds` |
|---|---|---|---|---|---|
| `EffectSet.lua` | 1 | 3134 | 18528 | 0 | 0 |
| `UIEffectSet.lua` | 1 | 52 | 211 | 0 | 0 |
| `UIEffectSet_INT.lua` | 1 | 6 | 29 | 0 | 0 |
| `EffectSet_INT.lua` | 1 | 46 | 196 | 0 | 0 |

- Every `templets=` count equals Phase 2's, so the new sub-table reads leave the parse of
  later templets intact.
- No `ES_EFFECT`, `ES_SOUND` or `unsupported` lines appeared.
- `18528` is exact, not one short of the source's 18,529 `EFFECT<n> = {` entries:
  - `EffectSet_PET_SNOWBUMP_Attack_C` defines `EFFECT16` twice
    (`march2014/data036/EffectSet.src.lua:126666` and `:126674`).
  - Lua keeps the second, so the set really has 25 effects.
  - This is a studio data quirk, and the stock game has it too.

**The user's decisions (2026-09-26):**
- **The `ES_EFFECT` criterion is settled in Phase 11,** as Phase 2's `ES_TEMPLET` was.
- **The three `ES.Description` rows** (`VERSION`, `DESC`, `NAME`) **are out of scope for Phase
  3 and settled later.**
  - March reads `EffectSet_Description.lua` only under `EFFECT_TOOL`, and the game never
    loads it.
  - Open lead: the census matched those keys to June `32378e20`, `32680da0` and
    `324ff4b0` by name alone. A short Ghidra look at them would show whether June's game
    client loads the file. If it does, those rows need an owning phase.
- The code went in under `SERV_IRUHADEV_JUNE_EFFECTSET_PARSE`, extending Phase 2's flag,
  with `SERV_IRUHADEV_JUNE_FX_DEBUG` for the log lines.
- `census.py` shows Phase 3 at 0 `to-implement` and 0 open `binary-only`. The 5
  binary-only rows are now implemented.
- 1 `unsupported` remains: the per-effect `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`.
- Every new read's `semantics` is `same`, with three exceptions, all `unchecked`:
  - `RATE`: its default is a register store, `ESI=0x64`. It was hand-verified as 100.
  - The two binary-only `ANIMTIME_ON_START_SQUARE/_UNITVIEWER` sub-table names.
- The only EffectSet-family rows left `to-implement` are `ES.Description`
  (`VERSION`/`DESC`/`NAME`). They are `unassigned` and EFFECT_TOOL-only (plan Phase 1 *Open
  questions*), the named exception to this phase's exit criterion.
- The clean `US_SERVICE` build is deployed to `X2_offline.exe`.

Things the next phases should know:
- **The `ES_EFFECT` criterion cannot fire on current data,** exactly as in Phase 2. No deployed
  set uses any Phase-3 key: `march2014/data036/EffectSet.src.lua` and the deployed
  `UIEffectSet.lua` were both checked.
  - The play-test evidence is the `ES_LOAD` line's new `effects=` count. It shows the
    per-effect block ran; `with_june_effect_key=0` and `sounds=0` are expected.
  - The per-effect line is settled at Phase 11, as Phase 2's `ES_TEMPLET` was.
- **One parse rule changes a consumed member:** `TRACE_MATRIX_EXCLUDE_SCALE` → `TRACE_MATRIX`.
- **Scope text below corrected:** the key is `ZOFFSET_USE_UNIT_IS_RIGHT`, not
  `POSITION_OFFSET_USE_UNIT_IS_RIGHT`. Camera shake needed nothing: all 19 of its keys were
  already implemented with matching semantics.
- **New helper VAs** are in `funcmap.tsv`: `321f4a20`, `321f3be0`, `321f2a60`, `321f3690`,
  `321f2db0`, `32fea4e0`, `32fea820`, `32fe99b0`.

### June transcription (`FUN_321e48e0` @ `321e5cf8`–`321ee2a0`)

Read instruction by instruction from `ghidra/dis_0x321e48e0.txt`. Getters: `32fe9e40` bool,
`32fe9fd0` int, `32fea370` float, `32feae90` string, `32fea4e0` int by index,
`32fea820` float by index. `[334380f0]` = -1.0, `[334350e8]` = 0.0 (double),
`[33436704]` = 0.05. "Default" is what June stores when the getter fails, so it holds
whatever the ctor set.

**Per `EFFECT%d`, new keys** (the rest match March; see the matrix):

| key | VA range | getter | default | post-processing |
|---|---|---|---|---|
| `GROUP_NAME` (binary-only) | `321e6261`–`321e632b` | string | `""` | none; read after `BONE_NAME` |
| `ZOFFSET_USE_UNIT_IS_RIGHT` | `321e6f61`–`321e7115` | bool | false | none. The plan's old "POSITION_OFFSET_USE_UNIT_IS_RIGHT" is wrong; the decryptor and June data both say `ZOFFSET_…` |
| `TRACE_MATRIX_EXCLUDE_SCALE` | `321e7445`–`321e7610` | bool | false | **if true, `TRACE_MATRIX` = true** (`321e760b`–`321e7610`) |
| `TRACE_MATRIX_IN_FIELD` | `321e766e`–`321e77e2` | bool | false | none |
| `USE_IS_RIGHT_WHEN_CREATE` | `321e8114`–`321e82bb` | bool | false | none; read after `APPLY_UNIT_ROTATION_MIRROR` |
| `FOLLOW_UNIT_ROTATION` | `321e830c`–`321e846f` | bool | false | none |
| `FOLLOW_UNIT_ROTATION_CPARTICLE` | `321e84da`–`321e86dd` | bool | false | none |
| `FOLLOW_UNIT_VIEWER_SCALE` | `321e8738`–`321e88dc` | bool | false | none |
| `USE_EFFECT_SCALE_IN_UNIT_VIEWER` | `321e894f`–`321e8b63` | bool | false | none |
| `BONE_TRACE_TARGET_MESH_INDEX` | `321e9aa9`–`321e9c8d` | int | **-1** | none; read before `BONE_TRACE_TARGET_MESH` |
| `NOT_ALLOW_MEMO` | `321ea221`–`321ea326` | bool | false (`XOR EBX,EBX` @`321ea308`, then `MOV [ESI],BL`) | none; read after `LIMIT_DISTANCE` |
| `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` | `321ea3ac`–`321ea634` | int | 0 (`NEG/SBB/AND`, low byte kept) | unsupported (same as the templet level) |

`LIMIT_DISTANCE` (`321ea0b2`–`321ea1dd`) was hand-checked: float, default -1.0, stored
squared only when `> 0.0`. That is exactly March's `X2EffectSet.cpp` read, so the matrix's
`unchecked(post-processing)` is settled as same.

**Type gate** (`321ea63a`–`321ea64b`): after the keys above, `EFFECT_TYPE` == 1
(`ET_DAMAGE_EFFECT`) reads `APPLY_DAMAGE_EFFECT_VELOCITY`. Types 2..5 (the four
`ET_MESH_PLAYER_*`) read `MESH_TRIGGER_DATA_LIST`, then `MESH_TRIGGER_DATA`. Every other type
reads neither.

**`APPLY_DAMAGE_EFFECT_VELOCITY`** (`321ebf7c`–`321ec080`): a positional table.
- `[1]`, `[2]`, `[3]` are floats, default 1.0 each. `[4]` is an int, default 0.
- One 32-byte entry is built (ctor `321f2a60`: `+0`=1, `+8..+0x10`=1.0) and pushed onto a
  vector in EffectData (`321f3690` is that vector's `push_back`).
- The entry holds `+0` = (`[4]` == 0) as a bool, `+4` = 1 (a type tag), and `+8/+0xc/+0x10` =
  `[1]`/`[2]`/`[3]`.
- Every June use is `{1.5, 1, 1, 1}`. What `[4]` and the tag mean is Phase 4a's to read at
  the consumer.

**`MESH_TRIGGER_DATA_LIST` → `TRIGGER_DATA%d`** (`321ea682`–`321ebbec`):
- Each entry is a stack struct (ctor `321f3be0`), pushed into a local vector, then assigned
  into EffectData @`321ebbec`.
- Order: `TRIGGER_TIME` float -1.0; `MOTION_NAME` string `""`; `PLAY_TYPE` int **2**
  (`XAP_ONE_WAIT`, binary-only).
- Then `BOUND_MOTION_TYPE_LIST`: int by index from 1 until the first miss, each pushed
  (`321eaa27`–`321eaa99`; the data uses `EMOTION_TYPE.*`, which March's `Enum.lua` lacks).
- Then `STOP_TRACING_ON_TRIGGER` bool false; `TRIGGER_ONCE` bool false; `BLOCK_TRIGGER` bool
  false (`XOR BL,BL` @`321eaed6`); `BLOCK_TRIGGER_TIME` float -1.0.
- Then `TRIGGER_EFFECTSET_LIST`: `BeginTable(int)` from 1, `EFFECT_SET_NAME` string `""`, and
  only non-empty names are pushed (`321eb09c`–`321eb2f3`).
- Then `EFFECT_SET_TOGGLE_VISIBILITY_TIMER` float -1.0. **If the read succeeds and the value
  is not -1.0, a flag is set** (`321eb5c7`–`321eb5de`); the ctor clears it.
- Then `USE_MESH_TRANSITION` bool **true** (`321eb78a`).
- Then `ANIMTIME_ON_START_GAME` / `_SQUARE` / `_UNITVIEWER` (the last two binary-only):
  each is `ONCE` bool false, then `TIME` float 0.
- The ctor also presets, with no Lua read, a second byte after each `ONCE` to 1, plus a
  second wstring and a few dwords: runtime state for Phase 4a.

**`MESH_TRIGGER_DATA`** (`321ebbf1`–`321ebf61`), stored inline in EffectData: `TRIGGER_TIME`
float -1.0, `MOTION_NAME` string `""`, `PLAY_TYPE` int **2** (binary-only).

**`SOUND%d`** (`321edd29`–`321ee2a7`), at templet level after `CAMERA_SHAKE%d`:
- A stack entry is filled and pushed onto a templet vector, with no rejection path:
  `EVENT_TIME` float 0, `FILE_NAME` string `""`, `RATE` int **100** (binary-only),
  `USE_GLOBAL_TIME` bool false, `LOOP` bool false.
- Byte `+0x27` of the entry is preset to 1 with no Lua read: runtime, Phase 4a.
- This is field for field March's own `CX2DamageEffect::SoundData`.

**For Phase 4, not parsed here:** after pushing each effect, June also pushes it onto a
second templet list when `BONE_TRACE_TARGET_MESH` is set (`321ec09a`–`321ec0ab`).
`ONCE_PER_CYCLE` is in the data but June never reads it (`june-ignores`).

**Paste with:** Starting context + this section.

- **June source:** `FUN_321e48e0`, from the per-`EFFECT%d` loop to its end, including the
  trigger, sound and camera-shake sub-blocks.
- **March target:** the same function; `EffectData` and its sibling structs in
  `X2EffectSet.h`.

**Scope.** Per-effect keys:
- `ZOFFSET_USE_UNIT_IS_RIGHT` (was mis-listed as `POSITION_OFFSET_USE_UNIT_IS_RIGHT`), `TRACE_MATRIX_EXCLUDE_SCALE`, `TRACE_MATRIX_IN_FIELD`
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

**Status: in progress (2026-09-26).** The spec below was written before any code, from the
June disassembly, with the decompile used only as a map. The work is split into three stages.
Resume at the first stage not marked done.

| stage | scope | state |
|---|---|---|
| 1 | Offset, position, rotation, the target-mesh vector, `ZOFFSET`, `OVERLAP`, debug lines | **done** 2026-09-26: clean `US_SERVICE` build, deployed to `X2_offline.exe` (SHA-256 `33d1eb57d282fae6…`); **play-tested 2026-09-26**, see below |
| 2 | `USE_IS_RIGHT_WHEN_CREATE`, `TRACE_MATRIX_IN_FIELD`, trigger-list copy + mesh `+0x194`, `APPLY_DAMAGE_EFFECT_VELOCITY`, `SET_OVER_UI` layer | not started |
| 3 | `GROUP_NAME` (`321dd590`, the `_GROUPCHILD_` loop), unsupported-key logging | not started |

### June layout used for reading (never ported)

These are offsets from reading June. They are not March members; see rule 2.

- **EffectData** (ctor `321f4a20`, size `0x1e8`). The layout was derived mechanically from
  every getter's destination in `321e48e0`.
  - `+0` type; `+4` `NAME`; `+0x20` `HYPER_NAME`; `+0x3c` `START_TIME`.
  - `+0x40` `BONE_NAME`; `+0x5c` `GROUP_NAME`; `+0x78` `WEAPON_BONE_NAME`; `+0x94`/`+0x98`
    weapon/model index; `+0x9c` `RIDING_PET_BONE_NAME`.
  - `+0xb8..0xc0` `POSITION_OFFSET`.
  - Bytes `+0xc4..0xcb`: `ZOFFSET_USE_UNIT_IS_RIGHT`, `TRACE`, `TRACE_MORE`, `TRACE_MATRIX`,
    `_EXCLUDE_SCALE`, `_IN_FIELD`, `REVERSE_Y`, `USE_LAND_POSITION`.
  - `+0xcc..0xd4` `ROTATE`; `+0xd8`/`+0xd9`/`+0xda` `APPLY_UNIT_ROTATION` / `_MIRROR` /
    `APPLY_INSTANCE_SCALE`; `+0xdc..0xf0` `SCALE` and `SCALE_*_ON_LEFT`.
  - Bytes `+0xf4..0xf7`: `START_WHEN_TRACE_TARGET_DEAD`, `PASSIVE_EFFECT`,
    `USE_SUB_ATTACK_LIST_SET`, `SET_OVER_UI`.
  - `+0xf8` `BONE_TRACE_TARGET_MESH_INDEX`; `+0xfc` `BONE_TRACE_TARGET_MESH`; `+0xfd`
    `TRACE_USER_TARGET_FORCE`; `+0x100` `LIMIT_DISTANCE`.
  - `+0x104` `FOLLOW_UNIT_ROTATION`; `+0x105` `NOT_ALLOW_MEMO`; `+0x114`/`+0x115` the two
    unit-viewer scale keys.
  - `+0x128` the `APPLY_DAMAGE_EFFECT_VELOCITY` vector; `+0x138` `USE_IS_RIGHT_WHEN_CREATE`;
    `+0x13c` the inline `MESH_TRIGGER_DATA`; `+0x1d4` `MESH_TRIGGER_DATA_LIST`.
  - `+0x1e4` `FOLLOW_UNIT_ROTATION_CPARTICLE`; `+0x1e5` the per-effect effect-alpha option.
  - The parser never fills `+0x108..0x110` or `+0x118`. See `NOT_ALLOW_MEMO` below.
- **EffectSetData**:
  - `+0x1c` the effects; `+0x2c` the second list (effects with `BONE_TRACE_TARGET_MESH`).
  - `+0x4c` `EVENT_TIMER_TYPE`; `+0x50` `STOP_CREATE_…`; `+0x51` `AFFECTED_BY_UNIT_SCALE`;
    `+0x54` `DELETE_SHAKE_COUNT`; `+0x58` `DELETE_EFFECTSET_AT_TRACE_UNIT_DIE`; `+0x59`
    `OVERLAP`; `+0x5c` `APPLIED_SKILL_ID`.
  - `+0x60..0x68` the memo scale; `+0x6c` `SYNC_SHOW_BY_CREATE_UNIT`; `+0x6d` the templet
    effect-alpha option; `+0x70` `SOUND%d`.
- **EffectSetInstance** (ctor `321df230`):
  - `+0` mode (1 game, 2 field, 3 pet); `+0xc` unit; `+0x14` trace target; `+0x1c` square
    unit; `+0x21` custom position.
  - `+0x5c` delay; `+0x60`/`+0x64` power rate / scale; `+0x70` scale; `+0x7c` damage-effect
    scale; `+0x80` EffectSetData; `+0x98` effect instances.
  - `+0xc0` elapsed; `+0xc4` use animation timing; `+0xd4` pet; `+0xe4` local rotation.
  - `+0xf0` the **target-mesh vector**: `{handle, EFFECT_TYPE, scale}`, `0x14` bytes each.
  - `+0x100` is-target-mesh; `+0x101` target-mesh right; `+0x104` target-mesh dir; `+0x110`
    target-mesh Z; `+0x124` bone-trace target-mesh scale.
  - `+0x150` memo scale; `+0x180` **facing when the set was created** (`321df230`
    @`321df528`-`321df576`: `unit->GetIsRight()`, square `+0x3d5`, pet `+0x439`).

### Creation-time keys: spec

Verdicts:
- `new`: June-only behaviour, transcribed.
- `semantics-differ`: March reads the key, and June's behaviour differs.
- `same`: diffed, no change.
- `4b @VA`: the only reader is in Phase 4b's functions.
- `unsupported`: the input system is absent (rule 5).

| key / behaviour | June branch | March site | verdict | stage |
|---|---|---|---|---|
| `ZOFFSET_USE_UNIT_IS_RIGHT` | `321d9820` @`321d9b36`-`321d9c2a`: in the game-unit branch, a nonzero Z offset is added along `GetZVector()` when the key is false. When the key is true it is added if `GetIsRight()`, subtracted otherwise | `SetEffectSetOffset` | new | 1 |
| `POSITION_OFFSET_X`/`_Z` facing unit | `321d9820` @`321d9847`-`321d9889`: the facing unit is the **trace target** when valid, else the owner. March always uses `m_optrGameUnit` | `SetEffectSetOffset` | semantics-differ | 1 |
| target-mesh offset, game unit | `321d9820` @`321d98a3`-`321d98c2`: `CMP idx,count; JBE` skips the target-mesh branch for every in-range index. It is reachable only past the end of the vector (an out-of-bounds read, not reproduced). June therefore always offsets by unit facing. March used the target mesh's dir / Z | `SetEffectSetOffset` | semantics-differ | 1 |
| target-mesh offset, no unit / square / pet | `321d9820` @`321d9e6a`-`321d9ea6`: `idx = max(INDEX,0)`, requires `idx < count`, handle valid and `!TRACE_USER_TARGET_FORCE`, then the plain add | `SetEffectSetOffset` | same (plus index) | 1 |
| `BONE_TRACE_TARGET_MESH_INDEX`, position | `321dcd30` @`321dcd9d`-`321dce07`: `idx = BONE_TRACE_TARGET_MESH ? INDEX : max(INDEX,0)`; then `321dcac0` (in range, handle ≠ -1, type ≠ 0, `!FORCE`; a dead mesh invalidates its entry) | `GetEffectPosition`, `GetEffectPositionByTargetMesh` | new | 1 |
| `BONE_TRACE_TARGET_MESH_INDEX`, rotation | `321df7e0` @`321df80c`-`321df860`: `idx = max(INDEX,0)`; valid only if not (`INDEX < 0` and `BONE_TRACE_TARGET_MESH`), in range, handle ≠ -1 | `GetEffectRotation` | new | 1 |
| `BONE_TRACE_TARGET_MESH`, registration | Damage `321f1f76`-`321f20f4`; mesh `321f21f7`-`321f2414`. Each such effect is **pushed** onto the target-mesh vector (`{handle, type, SCALE or 1,1,1}`).<br>Damage: is-target-mesh / right / dir / Z are set only when the count becomes 1; the instance's scale is not touched.<br>Mesh: is-target-mesh when the count is 1; right (set-only) / dir / Z on every push when the unit is valid; instance scale on every push.<br>March registered only the first and set scale in both cases | creation switch tail | semantics-differ | 1 |
| `BONE_TRACE_TARGET_MESH`, as a position source for itself | `321dcd30` as above: `INDEX` -1 with the key set means the effect does not trace any mesh | `GetEffectPosition` | semantics-differ | 1 |
| `OVERLAP` | `321d9f30`, from the factory `321e1bf0` @`321e1c68`-`321e1c77`: when set, and a live instance of the same templet has the same trace target (both null counts), no instance is created | `PlayEffectSet` → `_CreateEffectSetInstance` | new | 1 |
| `SET_OVER_UI` (particles) | `321f0f49`-`321f0f59` (the same in the other three particle cases): `+0x170`=1 (`m_bOverUI`) **and** `+0x164`=500 (500 = `XDL_DIRECTING`). That `+0x164` is `m_Layer` rests on layout alone (an int 12 bytes before `m_bOverUI`, as in March's `CKTDGObject`). A byte search found no June `SetLayer`/`SetOverUI` accessor at these offsets, so it is moved to stage 2, to be confirmed from June's particle-templet `LAYER` store first | creation, particle cases | semantics-differ | 2 |
| `USE_IS_RIGHT_WHEN_CREATE` | `321ef6bc`-`321ef6d5` (damage effect only): CEffect `+0x135` = instance `+0x180` | damage case | new | 2 |
| `TRACE_MATRIX_IN_FIELD` | mesh / particle cases, e.g. `321ef8ee`, `321efe84`, `321f03b6`: bone frame from the owner (`TRACE_MATRIX`, no target mesh) or from the target mesh. Matrix copied to the instance; position = frame + offset | creation, mesh / particle cases | new | 2 |
| `MESH_TRIGGER_DATA_LIST`, creation part | `321f2130`-`321f2166`: the EffectInstance copies the list (`+0x4c`); a non-empty list sets mesh `+0x194`=0 | mesh case | new | 2 |
| `APPLY_DAMAGE_EFFECT_VELOCITY` | `321ef692`-`321ef6a0` → `321db1b0` tag 1: main effect velocity `+0x534`; add if `[4]`==0, else multiply | damage case | new | 2 |
| `GROUP_NAME` | `321eee45`-`321eeeb7`, loop `321eeec0`-`321ef0ce`, `321f2483`-`321f24f1`, `321f2550`-`321f257c`, helper `321dd590`: particle types only. Base = `GROUP_NAME` up to `_GROUP`; one particle per bone `<base>_GROUPCHILD_<n>`, n from 1 | particle cases | new | 3 |
| `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` (effect / templet) | `321f1ec8`-`321f1ee4`, factory `321e1dd7`-`321e1e0c`: effect byte, or else the instance's option (`+0x1bc`) → damage `+0xb32`, mesh `31c83620`, particle `+0x1b0` | — | unsupported | 3 |
| `APPLIED_SKILL_ID` | `321e1e60` → `31fdbbb0` (user `+0x867c` memo) → EffectSetData `+0x60` → instance `+0x150` | — | unsupported | 3 |
| `NOT_ALLOW_MEMO` | `321dddb0` @ its tail: unless set, scale ×= memo scale. June copies it through EffectData `+0x108` @`321eede8` (a write into the shared templet, not reproduced) | `GetEffectScale` | depends on `APPLIED_SKILL_ID`; no effect in March | 3 (log) |
| `STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE` | `321da070` (marks every effect created), reached only from unit `3228a120` over the handle list `unit+0x3e90` on state change (callers `32015250`, `3277ee10`). The code that pushes into that list is outside the EffectSet region and **not yet located** | unit side | deferred: unit-side registration | — |
| `SYNC_SHOW_BY_CREATE_UNIT` | getter `321f2b00`, only caller UpdateEffect @`321e077d`. At creation only particles get `SetShowObject(inst+0xdc)` (`321f251d`) | — | 4b @`321e077d` | — |
| `SOUND%d` | `321dee80`, only caller UpdateEffect @`321e07f7` | — | 4b @`321e07f7` | — |
| `FOLLOW_UNIT_ROTATION`, `_CPARTICLE`, `REVERSE_Y`, inline `MESH_TRIGGER_DATA` | read only in `321e0080` | — | 4b | — |
| `FOLLOW_UNIT_VIEWER_SCALE`, `USE_EFFECT_SCALE_IN_UNIT_VIEWER` | no reader in `0x321d0000`-`0x32208000` except the parser | — | not creation; owner open | — |
| `LIMIT_DISTANCE` | `321eedad`-`321eeddf` | creation | same | — |
| `START_WHEN_TRACE_TARGET_DEAD`, timing / `EVENT_TIMER_TYPE` | `321ee7e4`-`321eec83` | creation | same | — |
| `USE_SUB_ATTACK_LIST_SET` | `321f219f`-`321f21f2` | mesh case | same | — |
| `TRACE_MATRIX` scale division | e.g. `321ef4f5`-`321ef58f` | every case | same | — |
| `GetUnitRotation` | `321ddea0`: the target-mesh lookup result is discarded, as in March | `GetUnitRotation` | same | — |

**Keyless June changes, not transcribed** (no key and no March caller; no dead members added):
- Position: the instance extra offset `+0x1a4` and the instance override bone `+0x130`.
- Holders: `+0xd8` (riding pet), `+0xe0`, and `+0xdd` (UI viewer).
- Damage-effect copies:
  - `+0x14c` (the `fForceDownValueRate` argument, default 1.0 = March's default);
  - `+0x11c`/`+0x120` (first attack and relate skill; `ADD_RENA_SYSTEM` is off);
  - `+0x15c`, from unit vfunc `0x10c`;
  - `+0x1b8`, and CEffect `+0xb64` = hyper.
- Modifier lists: `321ee480` over EffectData `+0x118` and instance `+0x160`, and `321db1b0`
  over instance `+0x170`.
- The particle option gate `3241d2a0` (`[3377585c]+0x180`, `+0x2c == 2` skips particles).
- Unit vfunc `0x248`, called from `PlayEffectSet`, is an empty stub in every class.

**Exit-test set.** No deployed set uses a June-only key, as in Phases 2-3. The branches that
fire on Ranox content are the semantics-differ ones:
- `BONE_TRACE_TARGET_MESH` registration: 11 of the 196 Ranox sets in
  `march2014/data036/EffectSet.src.lua`.
  - Damage-effect and mesh registrations: `EffectSet_NUI_RAGE_MOTHERFIG_Attack_A_Fire`,
    `EffectSet_LOW_RAGE_FATHERFIG_FD_BOSS_Attack_A`.
  - Damage effect only: `EffectSet_NUI_RAGE_DRYADKENTA_AttackA_Fire` / `_SpacialAttackA_Fire`.
  - Mesh only: `EffectSet_NUI_SMITH_STAIN_Attack_Ready` / `_Attack_C_Fire` /
    `_SpecialAttackA_Ready`, `EffectSet_NUI_RAGE_MOTHERFIG_Attack_A_Ready` / `_Attack_B_Ready`,
    `EffectSet_NUI_RAGE_BABYFIG_Attack_A_Ready`, `EffectSet_MOUNT_MAG_STONE_Wait`.
- The X/Z offset facing: 48 sets with a nonzero `POSITION_OFFSET_X`.

**Stage 1 play-test** (any difficulty). The NPC scripts are in `march2014/update/data037/`, and
the dungeons were found by grepping `Dungeon_Ranox_*` for the NPC names.
- **Ranox Burned Forest**: Rage Motherfig (`RAGE_MOTHERFIG`), Rage Dryadkenta
  (`RAGE_DRYADKENTA`), Rage Babyfig (`RAGE_BABYFIG`).
- **Ranox Ashes Village**: Smith Stain (`SMITH_STAIN`).
- Expected in `offline_server.log`:
  - `JUNE_FX ES_CREATE EffectSet_NUI_RAGE_MOTHERFIG_Attack_A_Fire <effect> BONE_TRACE_TARGET_MESH registered_index=0(mesh)`,
    and likewise `(damage)` for the Dryadkenta sets and `(mesh)` for the Smith Stain sets.
  - Where those sets offset a traced effect: `BONE_TRACE_TARGET_MESH offset=unit_facing(321d98c0)`.
- Not expected on current data, and not a failure:
  - `OVERLAP`, `ZOFFSET_USE_UNIT_IS_RIGHT` and `BONE_TRACE_TARGET_MESH_INDEX`: no deployed set
    uses them.
  - `facing=trace_target`: it needs a set played with a trace target that is not its owner.
- Watch for BTTM effects that move or rescale:
  - a second target-mesh effect in a set no longer sits on the first mesh;
  - a damage-effect BTTM set with `TRACE_MATRIX` siblings now divides by (1,1,1).
- Revert: undefine `SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME`.

**Stage 1 play-test result (2026-09-26).** `offline_server.log` has 47 `JUNE_FX ES_CREATE`
lines, all on Ranox sets:

| set | `registered_index=0(mesh)` | `offset=unit_facing(321d98c0)` |
|---|---|---|
| `EffectSet_NUI_RAGE_BABYFIG_Attack_A_Ready` | 15 | 15 |
| `EffectSet_NUI_SMITH_STAIN_Attack_Ready` | 5 | 5 |
| `EffectSet_MOUNT_MAG_STONE_Wait` | 4 | - |
| `EffectSet_NUI_RAGE_MOTHERFIG_Attack_B_Ready` | 1 | 2 |

- No `(damage)` registration fired in that run. Dryadkenta's teleport soft-locks the stage, so
  her sets could not be waited out.
  - A second run covered the damage path with a player skill:
    `EffectSet_Rena_Perfect_Storm PERFECT_STORM BONE_TRACE_TARGET_MESH registered_index=0(damage)`
    (x2), with no unhandled packets or exceptions.
  - A Hamelling Attack X in that run logged nothing. Only `EffectSet_Riding_Dark_/Red_HAMELLING_Attack_X`
    carry `BONE_TRACE_TARGET_MESH`; `_ICE_` does not.
- No `UNHANDLED`/`EXCEPTION` packets, and the shutdown was clean.
- The only `RANOX_EFFECT_MISSING` line is `EffectSet_PVP_BOT_Armageddon_Blade` (x5), an AI-party
  set with no templet. Its lookup fails before any Phase 4a code runs.
- So the stage-1 exit criterion (a `JUNE_FX ES_CREATE` line on a named Ranox set) is met.

**Create and update share the helpers**, so the facing change is consistent frame to frame.
June's `321dcd30` is called by CreateEffect @`321ef23a` and by UpdateEffect @`321e0964`
(plus `321df750`). March's `GetEffectPosition` is called by CreateEffect and UpdateEffect
(`X2EffectSet.cpp:1428`); its other `SetEffectSetOffset` caller is `EFFECT_TOOL`-only.

**Runtime coverage is mechanical.** `census.py` checks that every key March reads at the top of
`ES.templet`, `ES.EFFECT` and `ES.SOUND` has a `RUNTIME` citation. It passes: 69 keys are cited,
the existing ones as `same` with the June VA of their read.

**For stage 2, identifying members is the whole job.** June offsets still unmapped:
- CEffect `+0x135` (`USE_IS_RIGHT_WHEN_CREATE`). It may turn out to be a Phase 5/7a dependency.
- The main-effect velocity `+0x534`, plus the two calls after it, `31c8f310` and `31c8a9b0`.
  Read those two first: they may push the modifier onto the mesh for later use.
- Mesh `+0x194`, `+0x374`/`+0x378` (the `TRACE_MATRIX_IN_FIELD` matrix).
- Particle `+0x164`.

Corroborate each one with a March `offsetof` probe, compiled with `cl` under X2Lib's
`US_SERVICE` defines as `census.py` does (`#define private public` is fine for a probe). Pair
it with one June function whose shape matches a March one. Layout alone is not enough.

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

## Phase 6a — Sub-loaders I: CREATE_DAMAGE_EFFECT, die and hit damage effects, die conditions

**Paste with:** Starting context + this section.

- **June source** (roles corrected in Phase 0; see `funcmap.tsv`):
  - `FUN_31f6ac60`: the **`CREATE_DAMAGE_EFFECT%d`** loader. It opens plaintext
    `CREATE_DAMAGE_EFFECT` @`31f6acc5` and reads `LOOP_*`, `SET_DIE_WHEN_MASTER_EFFECT_DIE`,
    `INHERIT_*`, `SET_GROUP_ID`, `AUTO_DIE`, `APPLY_ROTATED_OFFSET`, `APPLY_ROTATE_ORIGIN`.
  - `FUN_31f68f30`, reached through wrapper `FUN_31f6e7e0`: the **die** damage-effect
    loader (`DIE_DAMAGE_EFFECT[_ULTIMATE]`). The ctor calls the wrapper @`31f7e7f7`.
  - `FUN_31f6ceb0`, reached through wrapper `FUN_31f6e850`: the **hit** damage-effect loader
    (`HIT_DAMAGE_EFFECT[_ULTIMATE]`). The ctor calls the wrapper @`31f7e84d`.
  - `FUN_31f66700`: the die-condition parser. The `DAMAGE_EFFECT_DIE_CONDITION_TYPE` enum
    values come from the June `Enum.lua`, and the comparison semantics come from this
    function.
- **March target:** `LoadDamageEffect` (`:6822`, `CREATE_DAMAGE_EFFECT`, which fills
  `m_vecCreateDamageEffect`), `LoadDieDamageEffect` (`:6624`), `LoadHitDamageEffect`
  (`:6900`), the `DamageEffectData` struct, and a new die-condition structure.

**Named test case.** `DAMAGE_EFFECT_Trap_Fire_Big` uses `LOOP_ADD_POS_X/Y` and
`SET_DIE_WHEN_MASTER_EFFECT_DIE`, all inside `CREATE_DAMAGE_EFFECT0..3`
(`june_lua/DamageEffect.src.lua:300710`), so it is `31f6ac60`'s. It is one of the two
blocks listed in `RANOX_EFFECT_ENGINE_GAPS.md` §1. After this phase it must parse
completely.

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

Your task: transcribe June's CREATE_DAMAGE_EFFECT%d loader (FUN_31f6ac60), die
damage-effect loader (FUN_31f68f30, via wrapper FUN_31f6e7e0), hit damage-effect loader
(FUN_31f6ceb0, via wrapper FUN_31f6e850) and die-condition parser (FUN_31f66700) into
X2DamageEffect.cpp's LoadDamageEffect (:6822) / LoadDieDamageEffect (:6624) /
LoadHitDamageEffect (:6900) and their structs. Take the DAMAGE_EFFECT_DIE_CONDITION_TYPE
values from June's Enum.lua. Parse only; the loop and condition runtime is Phase 7c. Cite
June address ranges.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when
DAMAGE_EFFECT_Trap_Fire_Big parses completely (JUNE_FX DDE_TEMPLET line) and the census
is clean for these sub-tables. Update the matrix and MODS.md.
```

---

## Phase 6b — Sub-loaders II: particle, lock-on, sound, after-image, black hole, sine move and the small loaders

**Paste with:** Starting context + this section.

- **June source** (all identified in Phase 0; see `funcmap.tsv`):
  - Particle: `FUN_31f66e60` and its four wrappers. `31f6a7b0` is `START_PARTICLE`,
    `31f6a940` is `PASSIVE_PARTICLE`, `31f6aad0` is `TIME_PARTICLE`, and `31f6cd00` is
    `HIT_PARTICLE`.
  - Touch-land damage effect: `31f76a90` (`TOUCH_LAND_DAMAGE_EFFECT`, June-only; no March
    loader). *Not* passive particles, as this section used to say. `31f6ceb0` moved to
    Phase 6a, because it is the hit damage-effect body.
  - Lock-on: `31f71930`, with the `NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE` enum and
    `AT_PVP/AT_FIELD/AT_RAID_FIELD/AT_DUNGEON`.
  - Sound: `31f73f30` (`SOUND%d`).
  - After-image: `31f74a80` (`AFTER_IMAGE`).
  - Black hole: `31f637e0` (`BLACKHOLE_DATA`; its keys are `DRAGGING_*`, hence the old
    "drag" label).
  - Sine move: `31f64200` (`SIN_MOVE_DATA`).
  - The small loaders:
    - `31f650f0`: `CAMERA%d`, March `LoadCameraData`.
    - `31f65730`: `SCREEN%d`, March `LoadScreenData`.
    - `31f64c80`: `HIT_EFFECT_SET%d`, June-only.
    - `31f66230`: `DIE_CREATE_NPC%d`, June-only.
- **March target:** `LoadParticleTable`, `LoadLockOnData` (`:6342`), `LoadBlackHoleData`
  (`:6410`), `LoadSinMoveData` (`:6436`), `LoadCameraData` (`:6753`), `LoadScreenData`
  (`:6777`), `LoadSoundData` (`:6801`), `LoadAfterImage` (`:7019`), plus new loaders for
  the June-only sub-tables.

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
wrappers), TOUCH_LAND_DAMAGE_EFFECT (31f76a90), lock-on (31f71930), sound (31f73f30),
after-image (31f74a80), black hole (31f637e0), sine move (31f64200), CAMERA%d (31f650f0),
SCREEN%d (31f65730), HIT_EFFECT_SET%d (31f64c80) and DIE_CREATE_NPC%d (31f66230); roles
and evidence are in funcmap.tsv -- into X2DamageEffect.cpp's Load* family. Parse only. Unsupported-system keys (numerical data,
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
  structure `31f66700` filled. **`LOOP_*` is not on the die path.** Phase 0 found it in
  `CREATE_DAMAGE_EFFECT%d` (`31f6ac60`), so its runtime is June's counterpart of March's
  `CREATE_DAMAGE_EFFECT` chain: `LoadDamageEffect` (`:6822`) → `m_vecCreateDamageEffect`
  → `SetDamageEffect` (`:5139`) → `m_DamageEffectDataList`, spawned at run time. Find the
  consumer of what `31f6ac60` fills.
- **March target:** `X2DamageEffect.cpp:5378`, `:5626`, `SetDieDamageEffect` (`:4931`), and
  `SetDamageEffect` (`:5139`) with the per-frame consumer of `m_DamageEffectDataList`.

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
- **March target:** `CX2DamageManager::SetDamageDataFromLUA` (`X2DamageManager.cpp:67`; the plan's
  "SetDamageData"), and `ParsingBuffFactorID` (`:3483`), whose June counterpart is `32630d20`.

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
conditions against CX2DamageManager::SetDamageDataFromLUA. Mark each same or semantics-differ in
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

## Phase 10b — Adoption audits: asset closure, name contract, script conversion

**Status: done 2026-09-25.** Tools and method notes are in `june2017_re/README.md`, section
"Phase 10b". Outputs: `JUNE2017_ASSET_CLOSURE.md`, `JUNE2017_NAME_CONTRACT.md`, and their TSVs in
`june2017_re/`.

**Result 3: Phase 11 converts with route (b), direct transcoding (`june2017_re/ljconv.py`).**

- **What `mar-2014` is, from source, not from elstools.** The March client's LuaJIT,
  `luajitLib/LuaJIT-2.0.2`, differs from stock only in the dump header. `lj_bcdump.h`, under
  `LUAJIT_MODIFIED_BYTECODE_HEADER`, sets `1b 4b 4c` and version `0x81`. `lj_bc.h`'s `BCDEF` is
  stock order. A diff against upstream's `v2.0` branch shows nothing else but the header, an
  `X2OPTIMIZE` guard, and later upstream fixes. `lj_bcread.c` rejects any other version, so June's
  `0x82` files must be converted.
- **The table.** 44 KOG opcodes are mapped: elstools' translation plus `FIX`. It is injective, and
  every slot's operand types match June's `lj_bc_mode[]`. The 14 files use 38 distinct opcodes; an
  unmapped one is a hard error.
- **The plan's disassembly check passes.** `luajit -bl` of each transcoded file against its
  `.std.luac`:
  - The 9 files without FIX-slot opcodes are byte-identical past the header.
  - The other 5 differ only on their FIX-slot lines, exactly 22 / 3 / 3 / 3 / 7 of them.
  - This check is close to tautological, because the table is read back from `.std.luac`. The
    evidence that discriminates is next.
- **Execution** (`june2017_re/fxdump.lua`: run against stub engine objects, dump what gets
  defined):
  - **Route (b)** loads all 14 files through the `0x81` loader and runs every one to completion.
    Its stub-call counts equal the source's call sites: `AddEffectSetTemplet_LUA` 8242 / 287 /
    138 / 12, descriptions 230, damage data 107, unit viewer 18.
  - **Route (a)** loses data three ways:
    - `DamageEffect_Dungeon` does not compile: ljd emits `function … end(…)`, `.src.lua:3910`.
    - `EffectSet.lua` aborts after 2,925 of 8,242 templets.
    - `DamageEffect.lua` aborts with 837 damage effects never defined. Both crash in the bogus
      `deepcopy` (`attempt to call global 'uv0'`) and leak `slot0`..`slot5` as globals.
  - **Where both run, they agree exactly:** every captured call, and 7,869 of 7,870 shared
    globals. The one difference is `DamageEffect_Gunner_Combo_ZZZX_Grenade_Explosion`, which
    `DamageEffect.lua` defines twice (lines 328616 and 340940); route (a) crashed before the
    second definition.
- **Not a third route: the vendored `luajit.exe`.** It loads `0x82`, but with the same
  `0x2f`–`0x37` swap as elstools: June's `deepcopy` lists as `USETN`/`USETP`/`USETV`.
- **For Phase 11: how the converted files are packed.** This is the user's workflow, with one step
  changed.
  - The user normally decompiles June lua to `name.src.lua` and puts it in
    `kom_contents/dataNNN_kom/`. `elstools compile-dir` then produces `name.lua`, and only
    `"name.lua"` is registered in `_manifest.json`.
  - For the effect files, `ljconv.py`'s output replaces the decompile and compile steps: it
    already is a compiled `name.lua`.
  - Manifest values follow the user's rule: `.lua` = `"2"`, everything else = `"0"`.
  - Not yet checked: that `elstools pack` takes a compiled `.lua` as it is.
  - **A stale source would silently undo the conversion.** `kom_contents/data036_kom` holds
    `EffectSet.src.lua` and `DamageEffect.src.lua`, the Ranox-merged sources. `elstools compile-dir`
    recompiles every `name.src.lua` into `name.lua`, so running it after dropping in the converted
    files puts the merged copies back. Nothing reports it. Move those two `.src.lua` files out
    first, or skip `compile-dir` for that directory.
- **For Phase 11: the chunks depend on run-time state.**
  - `DamageEffect.lua:351018` reads `INIT_PHYSIC.WALK_SPEED`. No effect file defines it; every
    character script assigns it globally. In a Lua state where it is absent, the rest of that
    chunk never runs. Check the load order.
  - Globals read as nil, as the client also sees them: studio typos `TURE`, `FLASE`, `FASLE`, `d`,
    and the undefined `RVK_FlashBang_Ulti_Scale`.
  - Also read as nil, and harmless: `AET_MAX`, `RWT_END` and `SGI_END` in June's `Enum.lua`. They
    are bare names written as list entries (`SERVER_GROUP_ID = { SGI_END, SGI_GAIA = 1, … }`,
    `Enum.src.lua:98`), so they only add a nil slot. March's enum has the same `SGI_*` pattern.
    The engine registers nothing for them to find.

**Result 1: asset closure (`JUNE2017_ASSET_CLOSURE.md`).**

- **Scope.** 17,816 roots reach 7,392 file nodes, through every particle and mesh-player templet
  and the textures inside every mesh and `.xet`. The completeness gate is clean: 0 unexplained
  strings, and 6 `NAME` keys that June's particle loader never reads.
- **Verdicts:**
  - 3,159 same;
  - 364 same-prefix;
  - 59 differ, 0 of them atlases (no effect texture has a `.tet` partner);
  - 15 live March copies that are truncated ports;
  - 12 pending pack;
  - 3,622 June-only: 3,081 extract byte-exact; 541 extract 1–255 bytes short (see next point);
  - 156 in neither build.
- **Found on the way: elstools 1.0.0 extracts about 18.7k June algo-2 entries 1–255 bytes short.**
  - The lost bytes are the file's tail: a DDS's smallest mips, and, in the meshes sampled,
    mostly binary `.x` closing-brace tokens. Occasionally it is list data.
  - The user reports such copies work in the March client when packed as lua = algo 2, everything
    else = algo 0. So they are usable, but not byte-identical to June.
  - 51 `march2014/update/` files are such short copies, 49 of them live. Table in the closure
    document. Nothing was changed.

**Result 2: name contract (`JUNE2017_NAME_CONTRACT.md`).**

- **Scope.**
  - All 432 X2Lib and KTDXLIB translation units, `cl /P` under `US_SERVICE`, 0 failures.
  - 48,591 string literals, wide and narrow: `CreateInstance_LUA` takes a `char*` name.
  - A literal counts when March's own data defines it as an effect set, damage effect, particle
    or mesh-player templet.
  - Seven `StringCchPrintf` format strings (e.g. `L"Effectset_Swallow_Assault_Shadow0%d"`,
    `X2GUAra.cpp:11166`) were matched against every defined name, for their run-time names.
  - 1,802 names in total.
- **Loaders.** Every `OpenScriptFile` / `Reset*Particle` / `Reset*XMeshPlayer` call opens only
  the standard particle and mesh-player files. `data121`'s `rena_A_trapingArrow_Particle.txt` /
  `_Xmesh.txt` ship in both builds, and neither build's code opens them.
- **Verdicts:**
  - 1,727 kept.
  - 58 kept only in files March does not load yet: 50 in `GameMajorParticle44.txt`, 7 in
    `GameMajorParticle36.txt` (the `BossRaid_Gate_P0%d` family), 1 in `DamageEffect_Dungeon.lua`.
    Phase 11 has to extend March's loader lists.
  - The punch list, 17 names. Each is also checked against **June's own binary** (plain strings
    plus `xorstrings.tsv`), a second, independent source:
    - **15 are gone from June's code: June changed those call sites.** On adoption, the March
      call site has to change too. They are:
      - 7 dropped: `EffectSet_Aisha_DeathFieldBomb`, `EffectSet_Aisha_DeathField_FirstImpact`,
        `EffectSet_ELSWORD_DOUBLE_SLASH_HYPER`, `EffectSet_RAVEN_VJAVELIN`,
        `CHUNG_BIGBANGSTREAM_MEMO`, `DAMAGE_EFFECT_CSI_SI_A_CDC_LEG_SHOT_Mini_Gun_Shell`,
        `DamageEffect_LGA_Arc_Shot`;
      - 7 rename candidates, same references (mostly Raven `*_HYPER` damage effects). Only
        `EVE_EP_DASH_COMBO_Z` has candidates June's binary asks for (`EVE_DASH_COMBO_Z`,
        `EVE_AT_DASH_COMBO_Z*`);
      - 1 case change, confirmed by the binary: `DamageEffect_Cryotron_Bolt_GUIDED_ARROW`
        became `…_Guided_Arrow`, and June asks for the new spelling.
    - **2 are still asked for in June's binary but defined nowhere: dead in June too**, so inert
      for parity. They are `BOMB` (`X2GUNPCFunc.cpp:2711/2759`) and
      `EffectSet_CSI_CIP_LAND_DEMOLISHER_AFTER_EFFECT`.
- **13 names already dead in March** (effect call sites, no March definition).
  - Bone names and parser keywords are excluded.
  - June defines 4 of them, so adoption brings them to life: the `X2GUAdd.cpp` Reverse Reactor /
    Psychic Storm / Dust Anatomy damage effects.
- **Not reachable by construction:** names assembled only from variables (`%s_%s`).

**Paste with:** Starting context + this section.

**No engine dependency.** These are static checks over data and source. They can run any
time before Phase 11, and running them early sizes the data side of adoption. No client
code changes in this phase.

1. **Asset closure → `june2017_re/assets.py` → `JUNE2017_ASSET_CLOSURE.md`.** Start from
   every June `EffectSet*`/`DamageEffect*` templet and follow each reference to its end:
   - the effect set / damage effect → mesh-player and particle templet names
     (`GameMajorXMeshPlayer*.txt`, `GameMajorParticle*.txt`, and their UI/Minor
     siblings);
   - those templets → meshes (`.X`), textures (`.dds`/`.tga`, and `.TET` atlases),
     sounds (`.ogg`/`.wav`), and any nested templet.

   For each leaf, record where it is found: in June's `.kom`s, in March's live `.kom`s, or
   in `march2014/update/`. Also record whether a March copy differs from June's, by size
   and hash.
   - "Differs" is the dangerous bucket. Swapping a shared atlas breaks every other swatch
     in it (see `RANOX_PORT_PLAN.md`, the `DLG_UI_Button17` / Texture71 merges). Flag
     those; don't resolve them.
   - The output is a list for the user. **The user packs.** Nothing is copied into the
     game directory.
2. **Name contract → `june2017_re/names.py`.** Collect every effect-set, damage-effect,
   mesh-player and particle name that March's C++ asks for by literal. Examples:
   `PlayEffectSet( L"..." )`, `CreateInstance( ..., L"..." )`, and `GetMajorXMeshPlayer`
   lookups, in `X2Lib`/`KTDXLIB`, preprocessed under `US_SERVICE` as `census.py` does.
   Diff that set against the names June's files define.
   - A March-only name is an effect that **silently disappears** on adoption. For each
     one, record whether June renamed it (by content), moved it, or dropped it, and
     which March call site uses it.
   - Do not fix any of them here. They are the adoption punch list.
3. **Script conversion method.** June's lua is KOG LuaJIT `KL\x82` bytecode. Two routes
   to `mar-2014`:
   - **(a) decompile + recompile:** `ljd_jun2017.py`, then
     `elstools compile-batch -v mar-2014`. **Known risk:** ljd mis-structures the
     code-built tables in `DamageEffect.lua` (README, "ljd caveat").
   - **(b) direct bytecode transcoding:** the header byte, plus KOG→stock opcode
     renumbering. The README says `mar-2014` uses stock numbers and differs only in the
     header. That is **unverified** as a claim about the whole format, so test it
     first. For each of the 14 June files, transcode, then disassemble both the
     transcoded file and the `decompile-batch` `.std.luac`, and require them to be
     identical.

   Write the result into this section. Name the route Phase 11 uses and give the evidence.

**Exit criteria.** `JUNE2017_ASSET_CLOSURE.md` exists and every leaf has a location
verdict. The name-contract list is complete, with a verdict on each name. There is a
decided, evidenced conversion route.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context", "End goal", "Phase 10b"), june2017_re/README.md (especially "June
LuaJIT opcode table" and the ljd caveat), and RANOX_PORT_PLAN.md's atlas-merge notes.
Branch: mods/lanox-port.

Your task: the three adoption audits, static only, no client code changes. (1) Asset
closure from every June EffectSet*/DamageEffect* templet down to meshes/textures/sounds,
with a location + differs verdict per leaf -> JUNE2017_ASSET_CLOSURE.md (flag shared
atlases, don't resolve them; I pack, nothing goes into the game directory). (2) Name
contract: every effect/mesh/particle name March's C++ requests by literal vs. the names
June's files define; verdict per March-only name. (3) Decide the June->mar-2014 script
conversion route: test direct bytecode transcoding against the decompile/recompile route
and record the evidence. Commit with my OK.
```

---

## Phase 11 — Enums, June-data trial load, then adoption

**Paste with:** Starting context + this section. Needs Phase 10b's outputs.

**The destination is adoption, not a trial for its own sake** (see *End goal*). The trial
in a throwaway copy is the safety step before June's files replace the merged March
copies in the live data.

1. **Enum data.** Add to `march2014/data036/Enum.src.lua` every June enum table and
   member that June's adopted files reference. The Phase 1 enum census lists them.
   - **This supersedes Phase 1's "no reader, not added" rule for adoption.** A missing
     enum name makes June's lua index nil, which aborts the rest of that chunk. A member
     with no C++ reader is still added, and marked reader-less in the matrix, so it is
     visibly inert.
   - Take the values verbatim from June's `Enum.lua`.
   - Preserve the UTF-8 BOM and CRLF byte for byte.
   - Add a comment naming this plan.
   - Then compile with `elstools compile-batch -v mar-2014`. **The user packs it.** Name
     the exact file(s) when reporting.
2. **Trial load.** Point the client at June's own `EffectSet*.lua` and
   `DamageEffect*.lua`, converted to `mar-2014` by the route Phase 10b chose, instead of
   the merged March copies. Include the assets Phase 10b's closure found missing.
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
   - **Settle the deferred log checks here.** Phase 2's `JUNE_FX ES_TEMPLET` lines must
     appear, for example `SYNC_SHOW_BY_CREATE_UNIT` on
     `EffectSet_KILL_ENEMY_WHEN_FULL_HYPER_MODE`, and `OVERLAP` on
     `EffectSet_ALH_LINK_FLOWER_DeBuff`. The same goes for any later phase's check that
     could not fire on merged data.
   - Play-test the name-contract punch list from Phase 10b: every March call site whose
     name June renamed or dropped.
3. **Report.** Write `JUNE2017_TRIAL_LOAD.md` with the results. It is a punch list of what
   blocks adoption, not a keep-or-adopt verdict.
4. **Adopt**, once the punch list is clear or each remaining item is accepted by the user:
   - **The user packs** June's converted files, the enum update and the missing assets
     into the live archives. Name every file.
   - Retire the Ranox-merged copies in `march2014/data036/`. Ask before removing them;
     they stay in git history.
   - Verify from the live game directory's logs, not the copy's.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context", "Phase 11") and JUNE2017_EFFECT_KEY_MATRIX.md. Branch:
mods/lanox-port.

The goal is adopting June's effect files, not keeping the merge (see "End goal").

Your task: (1) add every June enum table/member that June's effect files reference to
march2014/data036/Enum.src.lua (reader-less ones too, marked inert in the matrix), values
verbatim from June's Enum.lua, BOM and CRLF preserved, compiled with elstools
compile-batch -v mar-2014 -- I pack it; name the exact file for me. (2) Trial-load June's
own EffectSet*/DamageEffect* lua, converted by Phase 10b's chosen route, plus the missing
assets from JUNE2017_ASSET_CLOSURE.md, from a SEPARATE COPY of the game directory (never
the live one, never my els_db.sql). Ask me whether to use loose files or a trial .kom in
that copy, don't build US_INTERNAL unless I ask, and ask before deleting the copy
afterwards. Record every load error and every key still unread; confirm the deferred
JUNE_FX checks (Phase 2's ES_TEMPLET lines) fire; play-test Phase 10b's name-contract list.
(3) Write JUNE2017_TRIAL_LOAD.md as an adoption punch list. (4) When it's clear, name
every file I need to pack for adoption, and ask before retiring the merged March copies.
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
