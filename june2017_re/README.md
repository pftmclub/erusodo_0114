# June2017 reverse-engineering scratch

Working files for rebuilding the March-2014 client's `EffectSet`/`DamageEffect`/`DamageData`
engine from the June2017 reference client's binary. See `JUNE2017_EFFECT_REBUILD_PLAN.md`
in the repo root for the phase plan and per-phase kickoff prompts. This directory is the
shared scratch those phases read from and write to — nothing here ships in a build.

## The binary

- Unpacked dump: `F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe`
  — 43,801,600 bytes, SHA-256 `f2376a7210a6245b68ec772afbf0dd3d89a9f52a5b66f47bc739a7ad1c462af1`.
  The original packed file next to it, `x2.exe` (17,876,512 bytes), is **not** this file —
  do not RE that one; it's the one earlier notes correctly called packed.
- Ghidra project: `F:\elsword stuff\els_jun_2017_RE\els_jun_2017.gpr`. Open it and start the
  GhidraMCP plugin's HTTP server before any phase that touches Ghidra (`.claude/skills/ghidra/SKILL.md`).
- **Imagebase `0x31aa0000`.** Raw file offset → VA:

  | Section | Raw offset | VA |
  |---|---|---|
  | `.text` | 0x400 | 0x1000 |
  | `.rsrc` | 0x1cbbe00 | 0x1d1a000 |
  | `.idata` | 0x1d4a000 | 0x1da9000 |
  | (unnamed) | 0x1d4a200 | 0x1daa000 |
  | `rhwpapaz` | 0x23ba200 | 0x241b000 |

  For `.text` specifically: `VA = raw_offset + 0xc00 + 0x31aa0000`.
- 55,555 functions total, ~1,618 named. It carries RTTI and `__FILEW__` path strings, which
  is how several functions below were identified without needing symbol names.

Before trusting anything in this directory in a new session, re-check the dump's size and
hash against the values above — if they don't match, something about "the June binary" has
changed and every address in this directory is suspect.

## The obstacle: compile-time XOR string encryption

Lua keys and lua_tinker bind names (`AddEffectSetTemplet_LUA`, `g_pEffectSet`, most
`PLUA_GET_VALUE` key literals, …) are not plaintext in the binary. The idiom, seen
repeatedly:

1. A guard test (`TEST byte ptr [flag],mask` / `JNZ` past init) so decryption runs once.
2. Init: `OR dword ptr [flag],mask`, then a run of `MOV byte/word/dword ptr [buf+i],imm`
   into static buffers in `0x3379xxxx`–`0x337axxxx`.
3. Decrypt at use: `MOV r8,byte ptr [buf+i]` (or `MOVZX`/16-bit for wide strings), then
   `XOR r8,imm8` or `NOT r8` (key `0xff`), then store into a stack buffer. The terminator
   comes from a zeroed register, not a literal.

Wide strings use 16-bit loads/XORs. Loads across a single string's bytes can interleave
with other work, and the init path sometimes preloads registers ahead of the decrypt loop.

**Hand-verified controls.** A decryptor must reproduce both of these byte for byte before
anything else it emits is trusted:
- Buffer `0x3379c56c` → `"MAIN_EFFECT_NAME%d"`.
- Buffer `0x3379c2b8` → `"RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT"` (39 chars). This one is
  in the CEffect ctor, `FUN_31f7aab0`, and was hand-verified on 2026-09-24 from
  `ghidra/dis_0x31f7aab0.txt`.

  How `0x3379c2b8` is built:

  | Step | Address | What happens |
  |---|---|---|
  | Guard | `TEST byte ptr [0x3379c328],0x10` @ `31f7f62d` | |
  | Init: bytes 0–35 | `31f7f647`–`31f7f697` | 9 dword stores |
  | Init: bytes 36–37 | `31f7f6a3` | 1 word store (`0x0750`) |
  | Init: byte 38 | `31f7f6ac` | **Stored from a register** (`MOV [0x3379c2de],AL`, with `AL=0x0c` preloaded at `31f7f643`) |
  | Decrypt | `31f7f6be`–`31f7f92d` | Per-byte keys `a7 9e 34 33 60 0b 67 6a 8e 4d 7e d2 90 d6 6f 99 ae 65 50 8a 57 34 c0 df ac 73 83 57 43 f6 6c 89 24 2b 80 78 18 4e 58` |
  | Terminator | `31f7f934` | `BL`, the function's zero register |
  | Use | getter `0x32fea370`, destination `[ESI+0xb48]` | |

  The init path also enters the decrypt loop partway through. `DL=0xf5` is preloaded, and
  the code jumps straight to the first `XOR`.

  **Byte 38 is why the prototype drops last characters.** It is initialised by a register
  store, not an immediate store. It is decrypted out of order (`XOR AL,0x58` @ `31f7f8f7`)
  and stored to its stack slot after three `PUSH`es, which shift `ESP`. The prototype saw
  only immediate stores and produced `..._BY_H`. A correct decoder has to track register
  values into init stores, and adjust stack offsets for `PUSH`es when it orders the output
  bytes.

- Buffer `0x3379b6e8` → `L"PVPEmblem_Season2.lua"` (21 wide chars), in `FUN_31f57f10`.
  This is the third control, a **wide** string, hand-verified on 2026-09-24. It uses both
  XOR forms:
  - Even characters: a load, then `XOR reg,imm`, e.g. `MOV EAX,[b6e8]` /
    `XOR EAX,0xe9f` / `MOV word [ESP+0x20],AX`.
  - Odd characters: key first, then the buffer as the memory operand, e.g.
    `MOV ECX,0xe8a` / `XOR CX,word ptr [b6ea]`.
  - Init: 10 dword stores, plus `MOV [b710],DX` with `DX=0xed17` (a register store).
  - Characters by hand: `0x0ecf^0x0e9f`=`P`, `0x0edc^0x0e8a`=`V`, `0x2efb^0x2eab`=`P`,
    `0xe324^0xe361`=`E`, … `0xed17^0xed76`=`a`. Slots step by 2 from `ESP+0x20`.

## xordecrypt.py (the decoder later phases cite)

```sh
python xordecrypt.py --all                        # whole binary -> xorstrings.tsv (~75 s)
python xordecrypt.py 0x31f7aab0 -o out.tsv        # specific functions
python xordecrypt.py --range 0x31f50000 0x31f90000 -o out.tsv
```

It runs the three controls first, and **refuses to write anything if any control fails.**
It uses capstone (`pip install capstone`, 5.0.7 here). The model is in its docstring. In
short:

- Each register byte is tracked as "buffer byte `a` XOR `k`", so plaintext is
  `init[a] ^ key[a]` in buffer order.
- Load order, interleaving and register reuse therefore cannot scramble it.
- Stack slots (normalised for `PUSH`/`POP`/`ADD`/`SUB ESP`) are used only to check
  invariants.

Idioms it handles, each found on a real site:

| Idiom | Example |
|---|---|
| Guard mask from a register (`TEST [flag],AL` / `OR [flag],EAX`) | `31f57f90` |
| Many guards per flag dword, so keyed on `(flag, mask)` | `3379c5b0`, 21 masks |
| Init byte from a register, preloaded out of line | control 2, byte 38 |
| `0xff` as `OR CL,0xff`, or `OR BL,0xff` / `OR CL,BL` | `31f7e580`, `32731f59` |
| Init block interleaved with loads of other statics (a load does not end it) | `m_iCount`, `32731f4f` |
| Reverse XOR form, `XOR CX,word ptr [buf]` | control 3 |
| `LEA EBX,[EBX]` alignment NOP (capstone reports it as a write) | `31f7c8aa` |
| Tail merge: XOR into registers, then `JMP` to a shared store sequence (followed as a side trace, pending bytes only) | `m_iTitleID`, `326fb987` |
| Guarded statics, either never decrypted or with no static store before the first call (a static object, e.g. `3379b854`): not strings, counted, not emitted | 198 |
| Unguarded copies of static variables, key 0 throughout: not strings, dropped | `3379b808` |

**Output.** The first six columns are `use_va, func_va, buf_va, kind, len, plaintext`,
as the plan specifies. Then `status, term, guard, nuses`. Plaintext escapes tabs,
newlines and non-printables, so rows always split on tabs.

**Only rows with `status` = `ok` are citable.** An `ok` row satisfies all of:

- every init byte (immediate *and* register stores) has a key;
- there are no key conflicts;
- all slots are in one space, with a constant slot-minus-offset;
- a zero is stored at `base+len` (`term`=`reg`) or encrypted in the buffer (`enc`).

That is the plan's length criterion: decoded length = init bytes − terminator.

Result on 2026-09-24: **6,240 buffers, 6,180 `ok`.** The 60 that are not:

- 54 `orphan*`: the decrypt is in a function whose init is elsewhere (the KOM/patch loader
  around `31d77310`/`31d7a490`). With no init in view there are no boundaries, so
  adjacent strings merge. None is in the effect regions.
- 6 on `337b8128`/`337b8140`: a zero-filled wide static that gets copied. Not a string.

## census.py (Phase 1: the key matrix, and every later phase's exit test)

```sh
python census.py              # ~3 min: cl /P the March parser TUs, then write the matrix + census.tsv
python census.py --summary    # same run, prints counts per status and phase, writes nothing
python census.py --reuse-pp   # development only: reuse pp/*.i (gitignored) instead of re-preprocessing
```

It writes `../JUNE2017_EFFECT_KEY_MATRIX.md` and `census.tsv`, and exits 1 when a check fails.

**C0 additions (2026-10-05, `census_completion.py`, for `JUNE2017_EFFECT_COMPLETION_PHASES.md`).** Two more row
statuses, `handed-off` and `declined` (each row's `receiver` names the document + phase or the decision and date);
an `owner` column (the C-phase that closes each open row); and the definition-of-complete checks: item 1, one line per
damage source (blocks that get damage from June's numerical skill table / HP-rate only, read from
`assets_cache/june_fx.json`), item 3 (binary-only rows need a March parse site), and the decision table
(`SYSTEM_DISPOSITION`). `python census.py --final` is the C11/C12 gate: items 2-4 strict. A system counts as landed when
its flag is defined in `KTDXLIB/Always.h` and tested by an `#if` in `X2Lib`/`KTDXLIB` code.

**If it fails with `C1083: Cannot open compiler generated file: '...pp\X.i': Permission denied`**,
another process has that cached `.i` open. Since Phase 7b (2026-09-27) this has been
`pp/X2BuffBehaviorTemplet.i`, most likely a long-running MCP server's indexer. The file can be
neither written nor renamed. Don't kill processes to free it. Point the cache somewhere else for
that run instead; the outputs are identical:

```sh
python -c "import sys, os; sys.argv=['census.py']; sys.path.insert(0, os.getcwd()); import census; census.PP_DIR=r'<scratch dir>'; census.main()"
```

The model is in its docstring. In short:

- **June side:** `reconcile.Binary` and `reconcile.Explainer`, the same machinery as
  `reconcile.md`, per scope root (`ROOTS` in the script).
  - **Levels by VA.** A key counts as read only at the sub-table level where June reads it.
    The EffectSet parser `321e48e0` is one function covering every level, so its levels come
    from a hand table, `ES_LEVELS`. The census verifies each range against the binary's
    `BeginTable` calls.
  - `SetDamageDataFromLUA` `32632390` has its level map derived mechanically.
  - A key decrypted only by *another* sub-table's loader is not read at this level. For
    example, `SetDamageData` decrypts `HIT_GAP` inside `DAMAGE_DATA`, so a top-level `HIT_GAP`
    in a DamageEffect templet is June-ignored.
- **March side:** the parser TUs are preprocessed with `cl /P` under X2Lib's `US_SERVICE`
  defines plus `UNICODE`. A read counts only if it survives the preprocessor and sits at the
  same `BeginTable` path.
  - That settles both traps: `#ifdef`'d-out reads, and stringized keys, which arrive as
    literals.
  - `cl.exe` is driven from Python. Git Bash mangles `/Fi` paths.
  - Flags are probed with the name in a string literal: a defined flag is an empty macro.
- **Semantics:** June's getter at the decrypt site (`GETTERS`: float/bool/int/string, default
  from the store after `test al,al; jne`) is compared with March's expanded `GetValue`.
  - This runs only for rows June reads in a scope reader itself. Callee reads and the buff
    rows are marked `unchecked`, with the reason.
  - Hand-verified: `REFRESH_LAND_POS` (`mov byte [edi],1`), `ATTACK_TIME_END` (`fld [334355b8]` =
    100.0) and `LINK_EFFECT_RATE` (-1.0) match March.

**Phase 2 change to census.py: the absent-system search ignores the rebuild's own code.**
Plan rule 5 makes a phase parse, store and log an unsupported key. That code names the
key, so the old search turned "Applied skill ID" and "Effect-alpha option" from `absent`
to `present` as soon as Phase 2 parsed them. It also silently flipped their rows in
Phases 3, 5, 8c and unassigned to `to-implement`. The search now works like this:

- Hits in the `#if` branch of any `SERV_IRUHADEV_JUNE_*` block are not evidence. That
  covers raw sources through `RawSource.in_june_rebuild`, and the preprocessed TUs
  through `#line` back to the raw file. They are shown in the matrix's *rule-5 hits*
  column. The `#else` branch holds the studio's original code and still counts.
- Comments are blanked before the raw search, because a comment naming a key is not its
  system. Skill memo's raw hits drop from 36 to 31, and its verdict (`present`) is
  unchanged.
- A row whose system is `absent`/`gated-off` stays `unsupported` once March parses it.
  Its March column then shows the parse site.

Checked by diffing the regenerated matrix against HEAD with line numbers normalised.
The only status changes are Phase 2's six rows.

**Hand-verified (Phase 2).** `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` @`321e5a5d` reads
`unchecked` because June's shape is `NEG AL; SBB AL,AL; AND AL,[temp]`. It is the int
getter `32fe9fd0`, with a temp zeroed @`321e59a3`, so the default is 0 and only the low
byte is stored @`321e5cec`.

**Hand-verified (Phase 3).** These are the rows the census cannot settle mechanically.
The full per-key table is in the plan's Phase 3 section.
- `SOUND%d` `RATE` reads `unchecked`: its failure store is `MOV [..],ESI`, with
  `ESI=0x64` set @`321edd92`, so the default is 100.
- `ES.EFFECT` `LIMIT_DISTANCE` (`post-processing fmul`) was checked by hand: default
  `[334380f0]` = -1.0, and it is squared only when `> [334350e8]` = 0.0. That matches March.
- Two bool defaults come from registers, which is easy to misread:
  - `NOT_ALLOW_MEMO` stores `BL` after `XOR EBX,EBX` @`321ea308`, so its default is false.
  - `BLOCK_TRIGGER` stores `BL` after `XOR BL,BL` @`321eaed6`, so its default is false.
- `USE_MESH_TRANSITION` defaults to **true** in list entries (`MOV byte [..],1`
  @`321eb78a`), although the entry ctor `321f3be0` sets it false.

**Hand-verified (Phase 5).** The CEffect ctor's top-level scope (`31f7aab0`).

- **`RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT`'s default, `[334380f0]`, is `-1.0`**, read as raw
  bytes at file offset `0x334380f0 - 0x31aa0000 - 0xc00 = 0x19974f0` (`00 00 80 bf` little-endian),
  matching Phase 3's note that the same address is `ES.EFFECT LIMIT_DISTANCE`'s default.
- **`CHANGE_DAMAGE_TIME`'s "default store not recognised" is a false alarm.** `xor cl,cl` sets
  `CL=0` (false) on a failed get; on success, `mov cl,[esp+0x17]` reads it. `CL` then gates a
  `pGameUnit->m_iDamageTimes >= 0` check before `m_DamageTime = iDamageTimes`, byte for byte what
  March's `CHANGE_DAMAGE_TIME`/`GetDamageTimes()` block already does (`X2DamageEffect.cpp:1679`
  pre-Phase-5). Same.
- **`MAIN_EFFECT_TIME`'s "non-standard shape" is also a false alarm.** `fld [334380f0]` (the same
  `-1.0` constant); on failure it's stored as the default, on success the read value is compared
  against it with `fcom`/`fnstsw`, and `SetMaxLifeTime` is called only when greater -- exactly
  March's `if ( fLiveTime > -1.f ) pMeshInst->SetMaxLifeTime( fLiveTime );`
  (`X2DamageEffect.cpp:1554` pre-Phase-5, under `RIDING_SYSTEM`). Same.
- **`NOT_CULL`'s "non-standard shape" is also a false alarm for March's purposes.** June's
  `test/je/mov/cmp` computes the bool then *also* persists it to a member (`[ESI+0xb30]`) that
  March has no equivalent field for; the immediate consumption (`SetBoundingRadius(0.f)` when
  true) is identical to March's `bNotCull`/`SetBoundingRadius` block
  (`X2DamageEffect.cpp:1566` pre-Phase-5). Not implementing the persisted copy is a
  no-op today: nothing else in scope reads it.
- **`APPLIED_SKILL_ID`/`APPLIED_SKILL_ID_NOT_AFFECT_MEMO` are read inside the ctor's own
  `BeginTable("DAMAGE_DATA")`/`EndTable`** (`32fe9940`/`32fe9a10`, the same shape as March's own
  `KLuaManagerProxy::BeginTable`/`EndTable`), decrypted at `31f7b483`/`31f7b5ec`, both int getter
  `32fe9fd0`. `census.py` places `APPLIED_SKILL_ID` at `DE.top` rather than a `DE.DAMAGE_DATA`
  path, and calls `APPLIED_SKILL_ID_NOT_AFFECT_MEMO` `june-ignores(dead-here)` even though the
  ctor decrypts and reads it (via the same int getter, immediately after) -- reported, not fixed;
  neither affects Phase 5's exit criterion.
- **`DAMAGE_DATA_CHANGE_TIME`/`DAMAGE_DATA_CHANGE_TIME_GLOBAL_TIME` read as an indexed float array
  at this scope, via a shared helper `FUN_31f64b20`** (`BeginTable(key)` then
  `GetValue(1..)`, stopping at the first miss; a bool argument selects the destination between the
  two keys), not the scalar March already reads at the same key name. `census.py` already marked
  both rows `n/a (sub-table name)` before this phase, when March had only the scalar read -- its
  table-vs-scalar check doesn't fire for this shape, so the mismatch went unflagged. Confirmed
  against data: `march2014/data036/` and the live `data036_kom/` both write this key as a plain
  number everywhere it's used; `june2017_re/june_lua/` always writes it as `{ ... }`.
- **`DAMAGE_DATA_NEXT%d`/`DAMAGE_DATA_NEXT_ULTIMATE%d` numbering starts at 2.** `FUN_31f79c20`
  (not a thin wrapper -- it allocates, copies from `m_DamageData`, calls `SetDamageDataFromLUA`,
  and on success loops) builds `"DAMAGE_DATA_NEXT%d"` with a counter that starts at 2 (stored via
  `FUN_31f8b860`, a `push_back`-style append, not a formatter) and increments after each successful
  read (`31f79ec6`/`31f79f3b`). Confirmed against `june2017_re/june_lua/DamageEffect*.src.lua`:
  every file with a numbered continuation has `..._NEXT2` and up, never `..._NEXT1`.

**Hand-verified (Phase 6a).** `CREATE_DAMAGE_EFFECT%d` (`31f6ac60`), `DIE_DAMAGE_EFFECT[_ULTIMATE]%d`
(`31f68f30` via wrapper `31f6e7e0`), `HIT_DAMAGE_EFFECT[_ULTIMATE]%d` (`31f6ceb0` via wrapper
`31f6e850`), and the die-condition parser (`31f66700`, called from the ctor's own
`DAMAGE_EFFECT_DIE_CHECK_CONDITION` block).

- **`31f68f30`/`31f6ceb0` are the full `"<base><index>"` loop, not a "per-entry body"** as Phase 0's
  table and the plan text both said. The wrapper (`31f6e7e0`/`31f6e850`) passes the base name
  ("DIE_DAMAGE_EFFECT" or, when its own inline gate passes, "DIE_DAMAGE_EFFECT_ULTIMATE") as the
  *only* stack argument; the callee formats `"%s%d"` itself (via `31c7b920`) and loops. Its bool
  return is "did index 0 exist" (`31f6a775`: `MOV AL,BL` with `BL` reset to 0 before the first
  `BeginTable` and set to 1 right before the loop's own re-test, so a natural loop exit returns
  true only after at least one entry was read). The wrapper skips the base-name call when the
  ultimate one returns true. `funcmap.tsv` is corrected; the two roles (die vs. hit) are unchanged.
- **The wrapper's own gate (`31f6e7e3`-`31f6e822`) is an owner-unit/memo-pointer null chain then
  `FUN_31fcf3e0([ESI+0x7c4])`** -- the same unidentified skill-memo callee Phase 5's ctor calls to
  decide `m_bUseUltimateDamageData`. It is not literally that member (this is a different call
  site), but the same absent system, so March reuses `m_bUseUltimateDamageData` and stays forced
  false for the identical reason. `31f6e850`'s gate is byte-identical to `31f6e7e0`'s.
- **`INHERIT_LIFE_TIME` is a bool, default false, in all three scopes** (`CREATE_DAMAGE_EFFECT`
  `31f6ca33`, `DIE_DAMAGE_EFFECT` `31f6a433`, `HIT_DAMAGE_EFFECT` `31f6e65f`), all through the same
  bool getter `32fe9e40`. Easy to mis-read as the float "inherit at this rate" its name suggests;
  it isn't, in any of the three.
- **`SCALE_X/Y/Z` (`DIE_DAMAGE_EFFECT`/`HIT_DAMAGE_EFFECT`) default via `FLD1`, i.e. 1.0f**, not
  0.0f -- confirmed at each of the six decrypt sites (getter `32fea370`, failure path `FLD1;
  FSTP`).
- **`LOOP_END_TIME`'s default constant, `0x33441c60`, is `99999.0`**, read as raw bytes at file
  offset `0x33441c60 - 0x31aa0000 - 0xc00 = 0x19a1060` (`80 4f c3 47` little-endian), cross-checked
  against the already-verified `0x334380f0` = `-1.0` control in the same read. Matches March's own
  `fLoopEndTime` default exactly (`X2DamageEffect.h`'s `LOOP_CREATE_DAMAGE_EFFECT`-gated field,
  dead under `US_SERVICE` but same value) -- same, not a semantics-differ. Do not confuse this
  constant with the *different*, smaller `9999.0` some `LOCK_ON_DATA` keys default to.
- **`LOOP_END_COUNT`'s getter, `32fea210`, is a distinct "int" getter from `32fe9fd0`** (used for
  `DAMAGE_EFFECT_DIE_CONDITION_TYPE` and other enum-shaped ints): it rounds a Lua number through
  `FISTP` rather than reading an integer directly, but is int-typed for March's purposes all the
  same. Default 1, matching March's own `uiLoopEndCount`.
- **The die-condition parser's `AND_OPERATION`/`OR_OPERATION` calls share one vector, tagged by a
  third argument.** The ctor pushes `EBX` (0, the function's zero sentinel) for the `AND_OPERATION`
  call and a literal `1` for `OR_OPERATION`, both as the *third* stack argument to `31f66700`
  (`this`=CEffect, arg1=`pLuaManager`, arg2=key name, arg3=this flag). Confirmed by tracing it
  through the frame to `31f66d3d`: `MOV DL, byte ptr[ESP+0x174]` reads it back at
  `ESP_entry+0xc` (two callee-saved pushes, `EBP`+`EBX`, plus `ESI`+`EDI` saved once before the
  per-`CONDITION_` loop puts the read at `ESP_entry-0x168`; `0x174-0x168=0xc`, the third stack
  slot), then `MOV byte[ESP+0x14],DL` stores it into the second dword of the four-dword entry
  pushed into the vector -- i.e. every condition (from either sub-table) carries its own
  `bIsOrOperation`, and there is one flat list, not two. `DAMAGE_EFFECT_DIE_CONDITION_TYPE`'s
  values (`DEDCT_NONE=-1`, `DEDCT_DAMAGE_TIME=0`, `DEDCT_LOCK_ON_TARGET=1`) are from
  `june2017_re/june_lua/Enum.src.lua:11746-11749`, matching the binary's default (`-1`) and its
  two `JZ`-tested branch values (`0`, `1`) exactly.
- **census.py's `ROOTS` needed real entries, not code changes**, for the new `DE.DIE_CHECK_CONDITION`
  scope: its March column was `[]` (no prior March reader existed). `LoadDieCheckCondition` was
  added to `CE_LOADERS` (so census finds the top-level `DAMAGE_EFFECT_DIE_CHECK_CONDITION` key),
  and `ce('LoadDieCheckCondition')` / `ce('LoadDieCheckConditionBlock', ())` were added to the
  root's own March list. No change to `march_reads`/`path_match` was needed: a `BeginTable` call
  with a variable name pushes `'*'` (a wildcard on either side of `path_match`), which is why
  `LoadDieCheckConditionBlock(pTableName, bIsOrOperation)` -- one shared body for both sub-tables,
  called with a runtime name -- still resolves correctly; only a *numbered* `BeginTable(var, i)`
  loses the name entirely (`'*%d'`, not a wildcard), which is what forced the die/hit `_ULTIMATE`
  loops below to use a literal table name at their own call site rather than a shared helper.

**Hand-verified (Phase 6b).** `LoadParticleTable` (`31f66e60`), `LoadLockOnData`/
`LoadLockOnDataBlock` (`31f71930`), `LoadBlackHoleData`/`LoadBlackHoleDataBlock` (`31f637e0`),
`LoadSoundData` (`31f73f30`), and the three June-only loaders `LoadTouchLandDamageEffect`
(`31f76a90`), `LoadHitEffectSet` (`31f64c80`), `LoadDieCreateNpc` (`31f66230`).

- **`APPLY_START_PARTICLE_SCALE` (`31f66e60`) is read unconditionally**, at the end of
  `LoadParticleTable`, with no branch on which of the four wrappers (`START_`/`PASSIVE_`/
  `TIME_`/`HIT_PARTICLE`) called it -- confirmed by disassembling the read site and finding no
  parameter or caller-distinguishing state consulted before the getter call. Getter `32fe9fd0`
  (int), default 0.
- **`LOCK_ON_DATA`/`BLACKHOLE_DATA` are called with a table-name argument in June**
  (`31f71930`/`31f637e0` each take one stack argument, the table name), used from the ctor at
  `31f7e7a4-31f7e7da`/`31f7e7d3-31f7e7e9` to try `"..._ULTIMATE"` first when
  `m_bUseUltimateDamageData`-equivalent is set, falling back to the base name otherwise -- the
  same shape as Phase 6a's die/hit wrapper gate, confirmed by the identical
  `CMP byte ptr[ESP+0x2c],0x1; JNZ ...` branch structure around each call pair.
- **`LOCK_ON_RANGE_FROM_OWNER` (`31f71930` @`31f73202-31f7354b`) is a real nested sub-table, not
  four independent top-level keys.** `BeginTable("LOCK_ON_RANGE_FROM_OWNER")` wraps a
  `*(int*)(DAT_3377585c+0x8c)` switch: `iVar10==9` reads `AT_PVP`; `iVar10==0x15` (21) calls
  `FUN_32482650`/`FUN_32481f80` (an unidentified "is this a raid field?" pair) to choose between
  `AT_RAID_FIELD` (true) and `AT_FIELD` (false); `iVar10==0x17` (23) skips lua entirely and
  hardcodes `_DAT_3346ed60` (5000.0); any other value reads `AT_DUNGEON`. All four share getter
  `32fea370` (float) and, except `AT_FIELD` (default `_DAT_33437b40` = 1000.0, read inline rather
  than through the shared fail-path), the same fail-default `_DAT_33441c60` = 99999.0. When the
  table itself is absent, June computes a fallback via the *same* `iVar10` switch but with no lua
  read at all (`_DAT_33437b40`/`_DAT_33441c60`/`_DAT_3346ed60` picked directly) -- a value with no
  key, so nothing to parse; wiring the whole selection (both the in-table and the no-table path)
  is Phase 7b's. Raw float constants hand-verified from the exe's own bytes (file offset
  `VA - 0xc00 - 0x31aa0000`): `0x3346bda8` = 150.0, `0x3346ede4` = 750.0, `0x3344f348` = 300.0,
  `0x3346ede8` = 9999.0, `0x33437b40` = 1000.0, `0x3346ed60` = 5000.0, `0x334355b8` = 100.0 --
  cross-checked against the already-verified `-1.0`/`99999.0` controls in the same read.
- **`DUNGEON_TARGET_RANGE_MIN_MAX`/`PVP_TARGET_RANGE_MIN_MAX` (`31f71930` @`31f73225-31f73438`)
  are plaintext (unencrypted) sub-table names**, confirmed by reading the raw exe bytes at their
  string addresses (`0x3346ef90` = `"DUNGEON_TARGET_RANGE_MIN_MAX"`, `0x3346ef74` =
  `"PVP_TARGET_RANGE_MIN_MAX"`) directly, the same technique that decoded
  `0x3346ef56` = `"i@LOCK_ON_RANGE_FROM_OWNER"` (Ghidra's own symbol name for that string carries a
  2-byte prefix that isn't part of the literal). Each reads `GetValue(1,..)`/`GetValue(2,..)`
  positionally and resets to its own default pair unless both indices parse and
  `(*pfVar2 < *pfVar1) != (*pfVar2 == *pfVar1)` is false, i.e. max is strictly greater than min.
- **`census.py`'s brace-depth tracker requires `BeginTable` to sit textually inside the
  `if( true == ... )`/`if( ... == true )` it's gated by**, not as a preceding assignment
  statement with a bare `if` on the stored bool afterward. `march_reads`' stack only auto-pops a
  `'block'`-mode entry when the enclosing `{ }` it was pushed just before closes at the same
  depth; a bare-statement push is `'end'`-mode instead, which needs a same-depth `EndTable()` to
  pop, and an `EndTable()` nested one level deeper (inside the `if`'s own block, as `LOCK_ON_DATA`
  etc. all write it) never satisfies that. Confirmed with `march_reads()` called directly on the
  preprocessed `.i` text: the first-pass code (`bHas... = BeginTable(...); if( true == bHas... )
  { ...; EndTable(); }`) recorded `DUNGEON_TARGET_RANGE_MIN_MAX` at path
  `('LOCK_ON_RANGE_FROM_OWNER',)` and `PVP_TARGET_RANGE_MIN_MAX` at
  `('LOCK_ON_RANGE_FROM_OWNER', 'DUNGEON_TARGET_RANGE_MIN_MAX')` instead of `()`; rewriting each
  as `if( true == BeginTable(...) ) { bHas... = true; ...; EndTable(); }` fixed all three.
- **`TOUCH_LAND_DAMAGE_EFFECT%d` (`31f76a90`) is the full `DamageEffectData` field set**: its 21
  decrypted keys, in order (`POSITION_TYPE`, `DAMAGE_EFFECT_NAME`, `BONE_NAME`, `APPLY_ROTATE`,
  `APPLY_ROTATE_ORIGIN`, `ONLY_APPLY_ROTATE`, `APPLY_ROTATED_OFFSET`, `ROTATE_X/Y/Z`,
  `OFFSET_X/Y/Z`, `SCALE_X/Y/Z`, `PASSIVE_PARTICLE_X/Y/Z`, `INHERIT_BUFF_FACTOR`,
  `INHERIT_LIFE_TIME`), match `DamageEffectData`'s existing fields 1:1 with no key left over
  (confirmed by walking the whole per-entry decompile), including reusing Phase 6a's `vScale`
  (default 1.f via `FLD1`/hardcoded `0x3f800000`, not `vMainEffectScale`'s 0.f) for `SCALE_X/Y/Z`.
  No `EVENT_TIME` read at this scope, unlike `CREATE`/`DIE`/`HIT_DAMAGE_EFFECT`. The whole parsed
  record is built on the stack and passed by address to a single push helper (`FUN_31f8e750`),
  not filled field-by-field into a heap struct as March's other `Load*` loaders do -- re-expressed
  on March's own `new DamageEffectData` + `push_back` pattern rather than transplanted.
- **`HIT_EFFECT_SET%d` (`31f64c80`) and `DIE_CREATE_NPC%d` (`31f66230`) are both June-only,
  two-field structs** (`EVENT_TIME` + `EFFECT_SET_NAME`/`NPC_ID`), heap-allocated
  (`FUN_32fe243e(0x20)`, a pointer vector) and stack/value (an 8-byte pair, a value vector)
  respectively -- confirmed by reading each loop's growth arithmetic (`>>2`-shift pointer diffs vs.
  `>>3`-shift for 8-byte elements).

**Hand-verified (Phase 8a).** `SetDamageDataFromLUA` (`32632390`) and `ParsingBuffFactorID`
(`32630d20`), for the keys March already parses. These verdicts are in `census.py`'s `HAND_SEMANTICS`,
which a check keeps in step with the rows. Read with `sdd_walk.py` (below). `decompile_function` fails
on `32632390`, but works on `32630d20`, `32630910` and `326309f0`.

- **The idiom.** Every key is guard → decrypt → `push 1; lea <dest>; push <name>; mov ecx,esi; call
  <getter>; test al,al; jne; <default store>`, with `EBP` = `pDamageData` and `ESI` = the lua manager.
  Enum keys read into a zeroed temp and store `-(ok) & temp` (`movzx/neg/sbb/and`), so their default
  is 0. That is why the census calls that shape non-standard.
- **Register defaults.** Each was resolved by the last write to that register before the store,
  which always sits in the same block, just before the getter.
  - Zero (`EBX`/`BL`): `FLIP_DIR`, `NO_MISS`, `HIT_ADD_FORCE_POWER`, `IGNORE_RESIST`,
    `DEFENDER_LEVEL`, `CREATE_EFFECT_SET_TRACE_UNIT`, `EXTRA_DAMAGE_TYPE` and `ADD_LEVEL`.
  - -1 (`EDI`): `LEVEL` (`@326367a9`) and `DYNAMO_PARTICLE_RATE_TYPE` (`@3263532a`).
- **`MELEE_ATTACK` ignores the getter's result** (`@3263305e`), so the member keeps its prior value.
  That is exactly March's `LUA_GET_VALUE( .., m_bMeleeAttack, m_bMeleeAttack )`.
- **Enum defaults were checked against March's C++ values** with a `char(*)[N]` compile probe under
  `US_SERVICE` (the enums have `#ifdef`'d members). `EDT_MANA_DAMAGE` = 16, `DECT_UP_DOWN` = 3, and
  every other default is 0. March's `EXTRA_DAMAGE_TYPE` has 112 members up to 144, and **67 is a gap**.
- **Protected stores are plumbing.** `ATTACKER_GET` HP/MP, the `EXTRA_DAMAGE` numerics, `HIT_ADD_MP` and
  `HIT_ADD_HP_PERCENT` go through a compare-then-register idiom (`0x3376cdc8`, `323101d0`). That is
  `KProtectedType<>::operator=`, which March's `ExtraDamageData` already declares.
- **The one real post-processing difference.** `@32636204`: `if (EXTRA_DAMAGE_TYPE == 0x43) { type =
  EDI (0x10, @32635df5); byte [ebp+0x151] = 1 }`, i.e. `EDT_MANA_DAMAGE_WITH_NO_DISPLAY` →
  `EDT_MANA_DAMAGE` plus a flag. The flag is not reset by the ExtraDamageData init (`31f8e430` →
  `31f8d4d0`, which clears `+0x100`). `FUN_32cad510` clears it explicitly (`@32cad8dc`) when it rebuilds
  an extra damage.
  - A byte-displacement scan for `+0x151`/`+0x101` is pure noise: stack slots, other structs. To find
    the reader, go from March's `EDT_MANA_DAMAGE` consumer to its June counterpart, and look for the
    test there.
- **`DRAIN_HP_RATE` is mirrored** into `+0x4e4` (`@3263afdd`). This is a June-only member: `+0x4e0` is
  the rate and `+0x4e8` is `m_vecHitParticleName`, which `31d47300` resizes. It has no known reader.
- **`ParsingBuffFactorID` (`32630d20`)** reads `BUFF_FACTOR_ID` only when nothing earlier was read. The
  order is:
  1. `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID`, gated by skill learned (`31fceab0`) and buff template
     (`3227baf0`).
  2. `BUFF_FACTOR_ID_ULTIMATE_PVP`, then `_ULTIMATE`, both gated by `31fcf3e0(APPLIED_SKILL_ID)`, the
     same absent skill-memo callee as Phase 5/6a.
  3. `BUFF_FACTOR_ID_PVP`, in PvP (`322cc1a0`).
  4. `BUFF_FACTOR_ID`.

  On March's data none of 1-3 exists, so the result equals March's non-memo path. The level block is
  identical:
  - The user default comes from vtable `0x524` (`3208e410`), a now-state skill-ID getter, i.e.
    `GetNowStateSkillID`.
  - The NPC path uses master UID `>= 0` and `GetUserUnitByUID` (game vtable `0xa0`), and skips the
    level when it is 0.
  - A learn-check with level 0 skips the apply; otherwise the level is `lv ? lv : 1`.
- **Empty-string default** `0x3344f1f4` starts `00 00`, so it is `L""`.

**Hand-verified (Phase 8c).** The absent-system `DAMAGE_DATA` keys of `SetDamageDataFromLUA`
(`32632390`), read with `sdd_walk.py`. The per-key table is in the plan's Phase 8c section.

- **`NATURAL_FORCE_TYPE`'s default is a register.** Its store `@326354db` writes `EDI`, which
  `DYNAMO_PARTICLE_RATE_TYPE`'s block set to -1 (`or edi,0xffffffff` @`3263532a`). A capstone scan of
  `[3263532a, 326354e1)` finds no other write to `EDI`, so the default is -1, i.e. `NFT_NONE`. That is
  the same as March's own `ADD_RENA_SYSTEM`-gated read.
- **`NUMERICAL_DATA_INDEX`/`_NAME` are not DamageData members in June.** Both read into stack locals
  (`[esp+0x3c]`, `[esp+0x1d0]`), used only by the override `@3263fe41-32640054`.
- **`HP_RATE_DAMAGE_TYPE` is the enum shape** (`movzx/neg/sbb/and` @`3263f3fc`), so its default is 0
  whatever the temp held.
- **`census.py` changes.**
  - `phase_of()` tests the absent system before the March site. Before this change, a
    `DE.DAMAGE_DATA` row with any March site was labelled `8a`, so a parsed absent-system key would have
    left phase 8c.
  - New check: every phase-8c `DE.DAMAGE_DATA` row has a March parse site (Phase 8c's exit).
  - The Rose-weapon and applied-skill-ID `SYSTEMS` notes carry this phase's added evidence.
- **Enum tables are not `LUA_GET_VALUE_ENUM`'s business.** X2Lib compiles
  `LuaCommon/KLuaManager.h` (via `KTDXLIB`), not `luaLib/`. Its macro reads the built table and falls
  back to the default. A missing enum *table* errors in the chunk itself, and a missing *member* is a
  silent nil. See the plan's Phase 8c section.

**Phase 10: the MeshPlayer mode (`census.py --meshplayer`).** It runs about a minute, and every full
run appends it to the matrix as its own section. Its model is in the comment above `mp_census`.
- **The `.txt` keywords are plaintext.** Only Lua keys are XOR-encrypted.
- **June side.** Each keyword's plaintext address is found by a raw byte search, then Ghidra
  `xrefs_to` finds the function that uses it. That found:
  - the tokenizer `31c85840`, from `POSTEFFECT` et al.;
  - `ProcessTempletBlock` `31c8ae10`, from `"Unknown sequence bool property!"`;
  - `Compile` `31c8cd30`, from `"First word must be XMeshPlayer"`.
- **Each `Is*()` helper pushes exactly one keyword.** So the census reads June's dispatch set as
  "helpers `31c8ae10` calls" → "the string each pushes", with no hand list.
- **March side.** `KTDGXMeshPlayer.cpp` under **KTDXLIB's** `/I` list and defines, not X2Lib's.
- **It covers the particle parser too** (`TXT_SPECS`: tokenizer `31cf2250`, block `31cfa4a0`, events
  `31cf1e80`, TU `KTDGParticleSystem.cpp`).
  - **Look up each `Is*()` helper in its own parser's token class** (`CXMeshPlayerToken` /
    `CParticleEmitterToken`). Both classes have an `IsAniXET`: the mesh one compares `ANIPACKAGE`,
    the particle one `ANIXET`. A single map over the whole `.i` let one silently overwrite the other.
  - `TEXTURE` is dispatched by a keyword case, not a helper, in both builds, so it is declared
    per spec (`keyword_props`).
- **Negative control.** Run the March side over `git show HEAD:` of the pre-change `.cpp`, saved as
  a probe TU in `KTDXLIB/` and deleted afterwards. It must report exactly the keys the phase added.
- **Mapping templet offsets to instance offsets.** The instance ctor copies the templet to
  `m_TempletData` at instance `+0x1b4`. That was pinned by `SetLayer`/`SetForceLayer` reading
  templet `+0x1c`/`+0x20` back at `+0x1d0`/`+0x1d4`. A new templet field at `+X` is therefore read
  at instance `+0x1b4+X`, and a displacement scan for that finds every runtime reader.
- **A June-only object flag can be read by the caller, not the callee.** `CKTDGObject +0x17c`
  (not-cull) is never read in `IsInFrustum`, but the DG manager's sort loop `31ceb5f0` tests it
  before calling it. Scan for the displacement before concluding a flag is dead.

**Phase 1 corrections to Phase 0's `reconcile.py`.** `reconcile.md` changed only in the
`<CEffect top level>` scope, plus two explanations:

- **The CEffect top level really reads 58 of 378 data keys, not 378.** `matcher()` turned the
  `%s`/`%s_ULTIMATE` wrapper formats into `^.+$`, which matched every key. Templates with no
  identifier stem are now skipped.
- **Numbered-table helpers are recognised with no call-count limit.** This covers the die/hit
  wrappers into their full loaders, and `31f79c20`'s `DAMAGE_DATA_NEXT` + `"%s%d"` from 2.
- **A name used only as a `"%s%d"` base is category `numbered-base`.** `TIME_PARTICLE0..` is
  read; bare `TIME_PARTICLE` is not.
- **Deterministic tie-breaks**, and the explanation logic is now the shared `Explainer` class.

## offsetprobe.py and flagprobe.py (Phase 4a stage 2: mapping a June offset to a March member)

```sh
python offsetprobe.py ITEMS        # ITEMS: one "Type|member" (or "Type|SIZEOF") per line
python flagprobe.py FLAG [FLAG ...] # ON/OFF for each flag under X2Lib's US_SERVICE defines
```

Both reuse `census.py`'s `cl` setup and write nothing into the tree (the probe `.cpp` is deleted
again).
- `offsetprobe.py` compiles with `/Zs` and `#define private public`, and reads each offset from a
  deliberate `C2440` on `char (*)[N]`.
- A probe gives the March side of a mapping only. Pair it with one June function whose shape
  matches a March function; the plan's stage-2 member table shows the pattern.
- For a KTDXLIB-only TU (a size check across the two projects), compile with KTDXLIB's own `/I`
  list and defines from its build log.

## June LuaJIT opcode table (why elstools can't decompile the big June files)

June's scripts are KOG LuaJIT 2.0 bytecode, header `1b 4b 4c 82`. KOG permutes the
opcode numbers; `mar-2014` (`…81`) uses **stock** numbers and only the header differs.
**elstools 1.0.0's June table is wrong for KOG opcodes `0x2f`–`0x37`**: two sub-blocks
are swapped. Found 2026-09-24.

| KOG op | `2f` | `30` | `31` | `32` | `33` | `34` | `35` | `36` | `37` |
|---|---|---|---|---|---|---|---|---|---|
| June, actually | ITERC | ITERN | VARG | ISNEXT | UGET | USETV | USETS | USETN | USETP |
| elstools assumes | UGET | USETV | USETS | USETN | USETP | ITERC | ITERN | VARG | ISNEXT |

Consequences:

- **`decompile-batch`** fails on any June file containing a `for … in` loop, with
  ljd's `assert iterator.opcode in (ITERC, ITERN)`. In our set that is `DamageEffect`,
  `_Character`, `_Dungeon`, `_ETC` and `EffectSet`: each opens with a recursive
  `deepcopy` helper. The other nine files contain none of these opcodes. Their
  elstools output is byte-identical to the corrected decompile.
- **`compile-batch -v jun-2014` / `-v jun-2017`** emits the same wrong numbers in
  reverse. A June-format file with a loop, an upvalue or `...` compiled that way is
  broken. `-v mar-2014` is unaffected. `decompile-batch` has no `-v` option in 1.0.0.

Two independent sources give the table:

1. **The binary's own `lj_bc_mode[]`**, at `unpacked_x2.exe` file offset `0x1abfa08`.
   `lj_bcmode.py` finds it as the only 93-entry uint16 array in the binary whose
   operand types are an exact permutation of stock LuaJIT 2.0's. That check only works
   with `JFORL`/`JITERL`/`JLOOP` taking `lit`, not `jump`, as they do in stock.
   - Slots `33`–`37` each fit exactly one opcode: `UGET` and `USETV`/`S`/`N`/`P`.
   - `2f`, `30` and `31` all read `(base,lit,lit)`. The metamethod field splits them:
     `2f` and `30` carry `CALL`'s `__call`, so they are `ITERC`/`ITERN`. `31` has none,
     so it is `VARG`.
   - `32` reads `(base,___,jump)`, so it is `ISNEXT`.
2. **The data.** `DamageEffect_ETC.lua`'s first prototype (1 parameter, 1 upvalue, 19
   instructions at file offset 13) is `deepcopy(t)`.
   - Instruction 5 (`0x32`) jumps forward to the loop head: `ISNEXT`.
   - Instruction 17 (`0x30`, A=5 B=3 C=3) sits immediately before `ITERL`: `ITERN`.
   - Instruction 11 (`0x33`) loads upvalue 0, the function itself, for the recursive
     call: `UGET`.

**Use `ljd_jun2017.py`**, which `regen.sh` runs. It does not modify elstools. It runs the
vendored ljd on the `.std.luac` that `decompile-batch` leaves behind. For each
instruction it reads the *original KOG opcode byte* at the same file offset, and decodes
the nine above with the corrected mapping. It fails if the two files differ anywhere but
the header and opcode bytes.

**ljd caveat.** ljd mis-structures the `deepcopy` helper: it hoists `slot1[k]=uv0(v)`
into a bogus `return {…}`. Flat data tables come out clean, but code-built tables (the
`Get*` helpers in `DamageEffect.lua`) are not trustworthy as source. Phase 0's key
extraction therefore reads keys from the **bytecode**, and requires that set to equal
the one read from the `.src.lua`.

**ljd dropped-assignment fix (default since Phase 9).** ljd also drops real register writes on
stripped bytecode: it folds `slotN = <call>` into the one read it counted and leaves every
other read naming a register nothing writes (`../RANOX_LUA_DECOMPILE_BUG.md`, "Root cause").
`ljd_fixed.py` installs a fix for that into the vendored ljd at run time. It refuses to
install if `slotworks.py` isn't the exact file it was written against.
- June files: `ljd_jun2017.py FILE.lua` installs the fix by default; pass `--unfixed` to get
  the plain vendored ljd back. `regen.sh` uses the default.
- Stock bytecode: `ljd_fixed.py` with `main.py`'s own arguments.
- The harnesses: `ljd_oracle/run_oracle.py --fixed` and `ljdiff/ljdiff.py --fixed` (these two
  still default to unfixed, since they're also what proves the fix against the plain
  decompile -- pass `--fixed` explicitly).
- **Verifying a file's own June bytecode against its fixed decompile in one command:**
  `ljd_jun2017.py --check FILE.lua` (learn + trace + diff, 32 seeds; fails on any
  `dropped-assignment` or `clobber` verdict). `regen.sh` runs this automatically on every file
  it regenerates. See `../RANOX_LUA_DECOMPILE_BUG.md`'s "Detecting more instances" for what
  this does and doesn't cover (dynamic coverage only; it does apply the deepcopy-hoist
  exemption, so `EffectSet.lua`/`DamageEffect*.lua` correctly pass).

The fix code is `ljd_slotworks_fix.py`. `ljd_slotworks.patch` is the same change as a diff
for the two vendored ljd copies. **Applied 2026-10-01**: both `elstools/vendor/ljd` and the
second copy `elstools.exe` runs have the fix baked in statically now, so
`elstools.exe decompile-batch` gets it directly too, not just `ljd_jun2017.py`. One consequence:
`--unfixed` no longer produces genuinely unfixed output, since the fix is in the vendored file
itself regardless of the hook. See "Ranox Lua decompile fix: corpus tooling" below for the rest
of the toolset this fix was proven and swept with.

## Ranox Lua decompile fix: corpus tooling (`RANOX_LUA_DECOMPILE_FIX_PLAN.md`)

`ljd_fixed.py` and `--check` (above) are the day-to-day tools. The plan's other phases built a
bigger toolset to prove the fix and sweep the whole 7,328-file June corpus with it; it stays
committed (`june_src_fixed/` is gitignored, everything else here isn't) in case a later port
or phase needs it again.

- **`corpus_run.py`**: the generic resumable per-file runner every sweep (Phases 0, 5, 7, 8)
  reuses. One subprocess per file (ljd keeps per-process global state, so files can't share a
  process), a `faulthandler`-armed budget with kill-by-PID only (never by image name -- other
  `python.exe` processes, e.g. MCP servers, run on this machine), one TSV row appended per
  file, and `--resume` (which truncates a dangling partial last line off the TSV before
  appending, not just skips it in memory -- an earlier version that only skipped it would have
  glued a new row onto the fragment).
- **`decompile_worker.py`, `truncation_census.py`, `build_decompile_corpus.py` →
  `decompile_corpus.tsv`**: Phase 0's corpus inventory, one row per June `.lua` (7,328 total):
  size, manifest size, truncation status (none found in this corpus's `.lua` entries -- see
  the plan's "Phase 0 result"), the file's opcode set, and whether it decompiles
  (`ok`/`assert`/`unmapped-opcode`/timeout).
- **`ljconv_table.tsv`**: the full 71-opcode June→stock table `ljconv.py`'s `transcode`/
  `transcode_stock` read, each row's `evidence` column naming the elstools pair or
  `lj_bcmode.py` check that settled it.
- **`ljdiff/`**: the harness. `ljdiff.py` builds up to three sides per June file (`orig` the
  real bytecode, `rt` the round-tripped decompile, optionally `--extra NAME=PATH.src.lua` for
  e.g. a `march2014/` port file), runs `ljdiff.lua` (a mock Lua environment with a
  deterministic per-seed call driver) over each, and diffs `orig` against every other side:
  `dropped-assignment` / `clobber` / `upvalue` / `reorder` / `other-ljd` / `pass`, per
  (function, seed), plus a static must-define pass when line info is available. `types.tsv` is
  the shared, generated type oracle (learned from `orig` traces only, then frozen so both sides
  compare against the same assumptions); `calibrate.py` and `fixes/` are Phase 2b's calibration
  run and its restored-assignment fix sets for Kenta and MapData.
- **`ljd_oracle/`**: the engine-free regression suite (Phase 1). Hand-written Lua cases
  (`cases/*.lua`) cover the dropped-assignment shape, `elseif` chains, read-after-merge, the
  HP-ratio double-compare shape, genuine single-use controls (which a correct fix must *not*
  stop inlining), and the known deepcopy-hoist shape as a labelled, separate control.
  `run_oracle.py` compiles each case stripped and with `-g`, decompiles, recompiles, executes
  original vs round trip with `slotN` globals poisoned before each call, and prints a pass/fail
  table; `--fixed` uses `ljd_fixed.py`, `--expect fixed` checks against the Phase 4 target,
  `--twice` checks determinism.
- **`decompile_verdicts.tsv`**: the corpus-wide verdict table (Phases 5/7/8), one row per
  (file, function): `scope` (`ported` for `march2014/**/*.src.lua`, `corpus` for every other
  June file), the verdict, lint/coverage/clobber detail, and a first-divergence summary. The
  `scope=ported` rows are what Phase 6 repaired; see `../RANOX_LUA_DECOMPILE_BUG.md`'s "Scope"
  section for the summary and `../MODS.md`'s Dryad Kenta entry for one repair worked end to end.
- **`phase5_*.py`, `phase7_*.py`, `phase8_*.py`**: the sweep drivers behind
  `decompile_verdicts.tsv` -- `phase5_worker.py`/`phase5_aggregate.py` (the 276 mapped
  `march2014/` files, against both the fixed round trip and the port text as it stands),
  `phase7_worker.py`/`phase7_aggregate.py` (`data037_kom`'s 3,105 files), `phase8_worker.py`/
  `phase8_build_targets.py`/`phase8_aggregate.py` (the remaining ~4,200 files, with a
  proven-safe main-chunk-only fast path for files that define no functions).
  `phase5_map.py`/`phase5_files.tsv`/`phase5_targets.tsv` map `march2014/` files to their June
  originals; `phase5_clobber_census.tsv` and `phase7_cluster_residuals.py` are the clobber and
  residual-defect classifiers `RANOX_LUA_DECOMPILE_BUG.md`'s "Residual ljd defects" cites.
- **`../june_src_fixed/<kom_dir>/<name>.src.lua`** (repo root, not under `june2017_re/`;
  gitignored via the root `.gitignore`'s `/*` allowlist, same mechanism as `../tmp/`): the
  fixed decompile of every June file the Phase 7/8 sweeps produced a verdict for (7,313 of
  7,328 -- the 15 with none are listed in `../RANOX_LUA_DECOMPILE_BUG.md`), persisted so a
  later port doesn't have to re-decompile from scratch. Not touched by `regen.sh`, which only
  rebuilds the 14 `june_lua/` reference files.

## Phase 10b: converting June lua, and the two adoption audits

**Converting June bytecode to `mar-2014`: `ljconv.py` (route (b), chosen in Phase 10b).**

```sh
python ljconv.py OUTDIR              # the 14 files in june_lua/ -> OUTDIR/*.lua, mar-2014 bytecode
python ljconv.py --check-only        # build and check the opcode table only
```

- `mar-2014` is stock LuaJIT 2.0.2 plus a header. This comes from the March client's own source,
  `luajitLib/LuaJIT-2.0.2`, where `lj_bcdump.h` under `LUAJIT_MODIFIED_BYTECODE_HEADER` sets
  `1b 4b 4c` and version `0x81`. `lj_bc.h`'s `BCDEF` is stock order; a diff against upstream's
  `v2.0` branch shows nothing else but later upstream fixes. `lj_bcread.c` rejects any other
  version, so a June `0x82` file cannot load unconverted.
- Transcoding therefore rewrites header byte 3 and the opcode byte of every instruction, found
  by the script's own dump walker. The table is elstools' translation (read back from the
  `.std.luac` pairs) plus `ljd_jun2017.FIX`. It must be injective, it is checked slot by slot
  against June's `lj_bc_mode[]`, and an unmapped opcode is a hard error.
- Why not decompile + recompile (route (a)): see the plan's Phase 10b result.
- **The vendored `elstools/vendor/luajit/luajit.exe` loads both `0x81` and `0x82`, but its
  `0x82` table has the same `0x2f`-`0x37` swap as elstools** (`-bl` lists June's `deepcopy` as
  `USETN`/`USETP`/`USETV`). Use it only on `0x81` files.

**`fxdump.lua`: execute effect lua instead of reading it.** Run with the vendored `luajit.exe`
on `0x81` files (`luajit fxdump.lua OUT.json Enum.lua ...`). All files share one environment, in
the order given. Only an explicit allowlist exists, plus stubs for `g_pEffectSet`,
`g_pDamageManager`, `g_pUnitManager` (unknown methods raise) and a `D3DXVECTOR3` constructor. Any
other global read is recorded as `missing_reads`. `INIT_PHYSIC` is a probe: every character
script assigns it globally, and `DamageEffect.lua:351018` reads `INIT_PHYSIC.WALK_SPEED`, so its
reads are recorded and yield NaN.

**`fxdata.py`** is the shared layer: `.kom` inventories (`elstools inspect` manifests, cached in
`assets_cache/`), the two harness runs, and parsers for the particle / mesh-player `.txt` files
and for the texture names inside meshes (`xof` txt/bin/bzip, KOG `KSM`/`KME`) and `.xet` files.

- March's content comes from the user's staging copy, `24965799\kom_contents\*_kom`, but only
  where the file's size equals the live `.kom` manifest's.
- **June's content cannot be fully trusted: elstools 1.0.0 truncates June algo-2 entries.**
  About 18.7k June (V.0.4) algo-2 entries extract 1-255 bytes short of their manifest size. This
  happens with every `-v`, and in the old `14JUN17\data\*_kom` dirs too. It is lost data, not
  padding: DDS headers and MSZIP streams prove it. The loss is the tail, though (a DDS's smallest
  mips; mostly closing-brace tokens in binary `.x`), and the user reports such copies work in the
  March client when packed as lua = algo 2, everything else = algo 0. So they are usable, but not
  byte-exact. `fxdata.Copy.xpath` keeps such a copy for prefix comparison and for reading names;
  only `Copy.path` (exact size) is hashed as the file.

**`assets.py` -> `../JUNE2017_ASSET_CLOSURE.md` + `asset_closure.tsv`** (about 1 minute warm). It
walks from every June effect set and damage effect to files, and gives each file a location and a
same/differs verdict.

**`names.py` -> `../JUNE2017_NAME_CONTRACT.md` + `name_contract.tsv`** (about 40 minutes: 432 TUs
at ~20 MB of `cl /P` output each; `--resume` continues an interrupted run, `--reuse` redoes only
the comparison, about 1 minute). It runs
`cl /P` on every X2Lib and KTDXLIB TU under `US_SERVICE`, then checks every wide literal against
the names March's own data defines and against June's.

**Templet names are case-sensitive; file names are not.** Particle, mesh-player and effect-set
templets live in a `std::map<std::wstring>`, and a later definition replaces an earlier one
(`KTDGXMeshPlayer.cpp:769-778`, `KTDGParticleSystem.cpp:1318-1332`, `X2EffectSet.h:856`). Damage
effects are Lua globals. `KGCMassFileManager` upper-cases file names.

## Superseded: the prototype decryptors

`xordec.py`, `xordec2.py`, `xorscan.py` and `xorscan2.py` are the first-pass tools, kept
only for history. **Do not use them.**

`xorstrings.tsv` and `func_strings.tsv` are now `xordecrypt.py`'s output. The old
prototype `xorstrings.tsv` (7,086 rows) is in git history, at `9a8dfa8`.

Measured against the new decoder, over the 6,090 clean ASCII buffers:

| Prototype result | Buffers |
|---|---|
| Identical | 18 |
| Truncated by more than one char (it only followed byte loads, so `D3DXVECTOR2` came out as `D3D`) | 3,941 |
| Dropped exactly the last char | 103 |
| Merged into the next buffer (`MoveSetFr` for `Move`) | 32 |
| Garbled | 285 |
| Missed entirely | 1,711 |

The README's old "known flaws" list understated it: essentially every prototype string
is wrong.

## Files in this directory

Committed. These are small, or are the record of what was found:

- `xordecrypt.py` → `xorstrings.tsv`: the decoder and its whole-binary output.
- `charport_scan.py`: what `census.py` cannot see, June's character and other non-effect scripts (open
  DamageData keys by exact sub-path, numerical-only damage tables, June-only enum tables). The evidence
  behind `../JUNE2017_CHARACTER_PORT_CHECKLIST.md`; run it with a scratch directory argument.
- `func_strings.tsv`: the `ok` strings grouped by function (`func_va`, count,
  `|`-separated strings, each truncated to 60 chars), derived from `xorstrings.tsv`.
- `ljd_jun2017.py` and `lj_bcmode.py`: the correct June lua decompiler, and the evidence
  for its opcode fix. See "June LuaJIT opcode table".
- `ljd_fixed.py`, `ljd_slotworks_fix.py`, `ljd_slotworks.patch`: the ljd dropped-assignment
  fix, as a run-time hook and as an unapplied patch. See "ljd dropped-assignment fix".
- `corpus_run.py`, `decompile_worker.py`, `truncation_census.py`, `build_decompile_corpus.py`
  → `decompile_corpus.tsv`; `ljconv_table.tsv`; `ljdiff/`; `ljd_oracle/`;
  `decompile_verdicts.tsv`; `phase5_*.py`/`phase7_*.py`/`phase8_*.py` and their `.tsv`s: the
  rest of the Ranox Lua decompile fix's tooling (proving the fix and sweeping the whole June
  corpus with it). See "Ranox Lua decompile fix: corpus tooling".
- `reconcile.py` → `reconcile.md`: data keys against binary keys, per parser.
- `census.py` → `../JUNE2017_EFFECT_KEY_MATRIX.md` + `census.tsv`: the Phase 1 key census.
- `funcmap.tsv`: the June function map, `june_va, march_symbol, status, evidence`. **This
  is the authoritative map;** the section below only summarises it.
- `regen.sh`: rebuilds the gitignored files.
- `ljconv.py`, `fxdump.lua`, `fxdata.py`, `assets.py` -> `asset_closure.tsv`, `names.py` ->
  `name_contract.tsv`: Phase 10b (see that section).
- `offsetprobe.py`, `flagprobe.py`: Phase 4a's March-side member and flag probes.
- `vtslot.py`: Phase 9. `python vtslot.py 0x2b8 CX2GU` prints one vtable slot for every RTTI class
  with that name prefix (TD → COL → vtable, as in the RTTI note below). Per-class overrides live in
  each derived vtable, not the base's. That is how the nine `SetDamageExceptionProcess` overrides
  were found.
- `disasm_range.py`: raw capstone disassembly of an arbitrary VA range in `unpacked_x2.exe`, for
  addresses Ghidra hasn't carved into a function (e.g. `CEffect::OnFrameMove` `31f88790`, which has
  none at all). `--x87` also prints a best-effort symbolic x87 stack trace over one straight-line
  path, expanding `fcom(p)`/`fucomp(p)`/`f(u)compp` into the two operands compared -- Phase 7e.
- `x87test.py`: a ground-truth x87 compare+branch emulator. JITs the actual idiom (`fldz`/`fcom`/
  `fnstsw`/`test`/`jcc`) via ctypes-allocated RWX memory and runs it on concrete floats, rather than
  hand-deriving C0/C2/C3 condition-code and parity-flag tables from the Intel SDM -- Phase 7e found
  a real hand-trace error this way (see "Method notes").
- `sdd_walk.py`: Phase 8a's key-by-key walker for `SetDamageDataFromLUA` (`32632390`). By default it
  prints one block per decrypted key, from its guard to the next key's guard. XOR-decrypt noise is
  dropped by register taint tracking, so real stack-temp default stores still show.
  - `--compact` also hides the getter's argument setup.
  - `--order` prints the ordered key list with VA ranges: the 8b/8c handoff in the plan's Phase 8a
    section.
  - Its ranges run up to the next guard, so a range can include the next sub-table's `BeginTable`.
- `xordec*.py`, `xorscan*.py`: the superseded prototypes.

Gitignored, regenerable with `sh regen.sh` (git on Windows doesn't keep the exec bit):

- `funcs.txt`: the full `NAME at ADDR` function listing (`list_functions`), 55,556 lines.
  `xordecrypt.py` and `reconcile.py` need it.
- `methods.txt`: the `methods` endpoint output.
- `ghidra/dis_*.txt`: `disassemble_function` output for specific addresses.
- `june_lua/`: copies of the 14 June effect lua, plus their `.std.luac`/`.src.lua`.
- `pp/`: `census.py`'s `cl /P` output (~20 MB per TU) and flag probe. Rebuilt on every run.
- `assets_cache/`: Phase 10b's cache: `.kom` manifests, SHA-1s, transcoded June lua, harness
  dumps, `names.py`'s literal table. Safe to delete; the scripts rebuild it.

`regen.sh` needs Ghidra open with the project loaded, and the GhidraMCP server on
`127.0.0.1:8080`. Rebuild order after a regen: `python xordecrypt.py --all`, then
`python reconcile.py`.

## June function map

**`funcmap.tsv` is authoritative**, with 46 rows, each with its evidence. Roles are now
pinned by the plaintext sub-table name each loader opens: sub-table names are **not**
encrypted, only keys are. Phase 0 walked the CEffect ctor's sub-loader block
(`0x31f7e7b2`–`0x31f7f976`) in call order:

| Call site | Callee | Sub-table (plaintext) | March |
|---|---|---|---|
| `7e7b2`, `7e7c2` | `31f71930` | `LOCK_ON_DATA[_ULTIMATE]` | `LoadLockOnData` |
| `7e7d5`, `7e7e9` | `31f637e0` | `BLACKHOLE_DATA[_ULTIMATE]` | `LoadBlackHoleData` |
| `7e7f0` | `31f64200` | `SIN_MOVE_DATA` | `LoadSinMoveData` |
| `7e7f7` | `31f6e7e0` → `31f68f30` | `DIE_DAMAGE_EFFECT[_ULTIMATE]` + `%s%d` | `LoadDieDamageEffect` |
| `7e815`–`7e82a` | `31f6a7b0`/`a940`/`aad0`/`cd00` → `31f66e60` | `START_`/`PASSIVE_`/`TIME_`/`HIT_PARTICLE` + `%s%d` | `Load*Particle` → `LoadParticleTable` |
| `7e831` | `31f650f0` | `CAMERA%d` | `LoadCameraData` |
| `7e838` | `31f65730` | `SCREEN%d` | `LoadScreenData` |
| `7e83f` | `31f73f30` | `SOUND%d` | `LoadSoundData` |
| `7e846` | `31f6ac60` | `CREATE_DAMAGE_EFFECT%d` | `LoadDamageEffect` |
| `7e84d` | `31f6e850` → `31f6ceb0` | `HIT_DAMAGE_EFFECT[_ULTIMATE]` + `%s%d` | `LoadHitDamageEffect` |
| `7e854` | `31f64c80` | `HIT_EFFECT_SET%d` | none (June-only) |
| `7e85b` | `31f66230` | `DIE_CREATE_NPC%d` | none (June-only) |
| `7e862` | `31f76a90` | `TOUCH_LAND_DAMAGE_EFFECT` | none (June-only) |
| `7e875`, `7e885` | `31f79c20` → `32632390` | `DAMAGE_DATA_NEXT[_ULTIMATE]` | inline, `SetDamageDataFromLUA` |
| `7f0bb` | `31f74a80` | `AFTER_IMAGE` | `LoadAfterImage` |
| `7f967`, `7f976` | `31f66700` | `DAMAGE_EFFECT_DIE_CHECK_CONDITION`, `CONDITION_%d` | none (June-only) |

**Corrections to the pre-Phase-0 map**, which the plan's Phase 6a/6b text still repeats:

- `31f6ac60` is the **`CREATE_DAMAGE_EFFECT`** loader, not the die loader. `LOOP_*`,
  `SET_DIE_WHEN_MASTER_EFFECT_DIE` and `INHERIT_*` belong to `CREATE_DAMAGE_EFFECT%d`.
- `31f68f30` is the **die** damage-effect body, not the hit one. `31f6ceb0` is the **hit**
  body.
- `31f76a90` is **`TOUCH_LAND_DAMAGE_EFFECT`**, not passive particles.
- `31f637e0` is **`BLACKHOLE_DATA`**. Its keys are the `DRAGGING_*` ones, hence the old
  "drag loader" label.
- `32632390` is March's **`SetDamageDataFromLUA`**, with `ParsingBuffFactorID` = `32630d20`.
- `31c7b980` is **`KLuaManager::BeginTable(name, index, bLogOutput)`**: it formats
  `"%s%d"`. Every `EFFECT%d`/`CAMERA_SHAKE%d` in the EffectSet parser goes through it.
- **Not ours:** `31fbe4a0` (UI guide keys), `321fbb80` (riding-pet templet:
  `RIDING_PET_ID`, `STAMINA_*`).

**Ghidra names (2026-09-24, user-approved).** The 36 `confirmed` rows that Ghidra has a
function for are renamed `June_<Class>_<Method>` in the project, e.g.
`June_CEffect_LoadCreateDamageEffect` = `31f6ac60`. `31f88790` is not renamed, because Ghidra
defines no function there. No comments were written.

The candidates (`31f85910`, `31f83aa0`, `31c895d0`, `31c8d140`, `32057b80`,
`32784c00`) are unchanged; Phases 7a and 10 own them.

**Phase 7a (2026-09-27) settled the DamageEffect runtime.** Details and evidence are in
`funcmap.tsv`.
- `31f88790` is **`CEffect::OnFrameMove`**, not the deleting dtor. It is the only vtable slot,
  it ends `RET 0xc`, and it is called as `vtable[0](double, float)` by
  `CX2DamageEffect::OnFrameMove` `31f79fb0`.
- `~CEffect` is `31f78550`, non-virtual.
- `31f83aa0` is `OnFrameMove_LockOnData`.
- `31f85910` is **not** OnFrameMove. It sits on the damage manager's `DamageCheck` chain and stays
  a candidate for Phase 7d/9.
- Unit and game virtuals were matched from the RTTI vtables:
  - CX2GUUser `334781f4`: slot `0x80` = `GetBonePos` (bool form), `0x138` = `GetPos`, `0x150` =
    `GetRotateDegree`, `0x158` = `GetYRotateDegree`.
  - CX2Game `3348bf94`: slot `0x98` = `GetMyUnit`.
  - To find a vtable, search the binary for `.?AV<Class>@@`. The type descriptor is that address
    - 8; the COL is where `+0xc` holds the TD address and the signature is 0; the vtable follows
    the dword that points at the COL.
- None of these new rows is renamed in Ghidra: renaming needs the user's OK. `31f88790` still has
  no Ghidra function at all.

Two method notes from Phase 7a:
- **Emulate x87 code rather than hand-trace it.** A capstone-driven stack emulator settled two
  formulas.
- **Capstone prints the popping forms with one operand.** `DE E9` shows as `fsubp st(1)` and means
  `st1 = st1 - st0`, then pop. The emulator's first run got this wrong, and so would a hand trace.

### Region survey (functions ≥1500 bytes, for Phase 7a's identification work)

`EffectSet` object region `0x321d0000`–`0x32208000` and `DamageEffect` object region
`0x31f50000`–`0x31f90000` — full listing with sizes is in `funcs.txt` (regenerate with
`regen.sh`) filtered through the snippet in `regen.sh`'s comments. The confirmed/candidate
tables above are the ones this investigation actually looked at; there are more functions in
each region nobody has looked at yet.

## Method notes (things that went wrong, so the next phase doesn't repeat them)

- `curl --noproxy *` (unquoted) gets glob-expanded by the shell and silently returns a
  Cloudflare 302 instead of hitting Ghidra. **Always quote it**: `--noproxy '*'`.
  Inside a variable (`C="curl ... --noproxy * ..."`, run as unquoted `$C`), quoting
  doesn't help: the quotes become literal. Use `--noproxy 127.0.0.1` there.
  `.claude/skills/ghidra/SKILL.md` was fixed that way in Phase 0. It is gitignored,
  so the fix lives only in the working tree.
- `decompile_function` on large functions (`FUN_31f7aab0` and bigger) returns
  `"Decompilation failed"`, most likely a timeout. Ask the user to raise Ghidra's decompiler
  timeout and max payload (Edit → Tool Options → Decompiler) before phases that need large
  functions decompiled rather than just disassembled.
- `get_function_by_address` only resolves **entry points**; it returns `"No function found"`
  for any other address inside a function's body. To identify which function contains an
  arbitrary address, use `xrefs_to` on that address instead — the response names the
  containing function (`"From <addr> in FUN_xxx [...]"`).
- The `strings` endpoint only returns strings Ghidra has already defined as data at analysis
  time — it does **not** find the XOR-encrypted key literals (they aren't defined strings at
  all, just byte arrays), so it's unreliable for this investigation specifically. Don't rely
  on it to prove a key is absent.
- A first TSV parse of the byte-wise scan failed because some decrypted "plaintext" strings
  contain raw tab and newline bytes (mis-decoded regions, or genuine embedded formatting).
  Parse `xorstrings.tsv` with a tolerant regex over `latin1`, not a naive tab-split.
- Printing decrypted UTF-16 text through a `cp1252` Windows console raises an encoding
  error. Decode as `latin1` or otherwise sanitize before printing; don't assume the console
  is UTF-8.
- **Sub-table names are plaintext; only keys are encrypted.** A loader's role shows in
  the plaintext string it pushes before `BeginTable`. `reconcile.py` counts those
  strings as binary keys, and so must any census.
- **`%s_ULTIMATE` variants.** `31f8ba80`/`31f8b370` read `K` and `K_ULTIMATE` for a
  plaintext `K` pushed just before the call (e.g. `MAIN_EFFECT_NAME`, `FOLLOW_LINE`).
  `K_ULTIMATE` never appears as a literal.
- **`xorstrings.tsv` rows with `status` other than `ok` are not citable.** Use `ok` rows
  only.
- **Finding who reads a CEffect member (Phase 7d).** Take the member's offset from the ctor's
  getter destination (`LEA EDI,[ESI+off]` before the `CALL 0x32fe....`), then scan all of
  `.text` with capstone for that displacement. Include `EBP` as a base: these functions use it
  as a general register, and excluding it hides readers. Where Ghidra has no function, find
  the containing function's start as the 16-aligned address just after a run of `CC` padding.
  Then list its `E8`/`E9` callers with a byte scan for `rel32`. That settled every 7d key in
  minutes.
- **Templet lists and per-call scratch lists are different members.** The hit/die loaders push
  into the templet vectors through a pointer, so a displacement scan for the list the spawner
  walks (`+0xa18` hit, `+0xa28` die) finds only the wrapper that copies the templet list
  (`+0x98` / `+0x8`) into it (`31f831e0`, `31f83400`). Attribute a spawner by its caller
  (AttackResult vs. the die paths), not by the loader.
- **`decompile_function` handled 11.7 KB `31f85910` and the 3,736-line `326413c0`** this
  session. Try it before falling back to raw disassembly.
- **Ghidra's decompile can smear one call's pushes across two adjacent calls (Phase 7e).**
  `FUN_31f70130`'s decompile gave both `FUN_31aada50` (a plain `g_pX2Game` fetch with no stack
  args at all) and `FUN_321e1e60` (the real target) the *same* 15-argument list. Trust the raw
  pushes instead: `disassemble_function`/`disasm_range.py`, then track ESP through the
  interleaved `PUSH`/`SUB ESP` sequence by hand, computing each read's original-frame offset as
  `stack-offset - cumulative-bytes-pushed-so-far`. Checking that the total push count matches the
  callee's own `RET N` is a cheap sanity check on the count (not the content).
- **x87 hand traces get the polarity wrong even when the C0/C2/C3 table looks right on paper
  (Phase 7e).** A touch-land velocity gate's three comparisons were hand-traced once as "all
  three `< 0`" and flagged "not yet emulated" — correctly flagged, because the third actually
  uses a different mask/branch idiom (`test ah,5; jp`, not `test ah,0x41; jne`) and is the
  *opposite* polarity. `x87test.py` settles this kind of question by running the real idiom on
  concrete floats (`-1.0`, `0.0`, `1.0`, `nan`) rather than re-deriving flag tables. To test a new
  mask/branch pair, add another `make_func(mask, use_jp)` variant with the matching opcode
  (`0x75`=JNE, `0x7A`=JP, `0x7B`=JNP, ...).
- **`disasm_range.py`'s own x87 tracer had a matching bug**: it only special-cased the *popping*
  compare forms (`fcomp`/`fucomp`/`fcompp`/`fucompp`), so a plain non-popping `fcom`/`fucom` fell
  through to a generic stack dump with no comparison shown — silently hiding 2 of 3 compares in
  the touch-land gate. Fixed; both forms now print `compare A vs B`.
- **An E8-relative-call scan across the whole `.text` is stronger evidence than "the decompile
  shows a caller" (Phase 7e).** For `DIE_CREATE_NPC`'s die-path pairing, scanning every `0xE8`
  byte and resolving its `rel32` found exactly 6 callers of `31f70330` and 6 of `31f83400`,
  pairing 1:1 — confirming the pairing generalizes rather than trusting the handful of call sites
  the decompile happened to surface.
- **Effect C2 (2026-10-06): three ways an identification went wrong before, and how C2 avoided them.**
  - *A displacement hit that decodes as `mov esi, 0x570` is a misaligned real read.* `dispsweep.py` picks
    the first start that decodes, so re-decode 2-4 bytes earlier: `329f64b2` is `cmp byte ptr [esi+0x570], 1`
    @`329f64b1`, the CAN_ACTIVATE_NASOD_ARMOR_COMBO reader. Byte-level re-decode found it when the sweep's
    listing showed nothing.
  - *Name an offset from its getter destination before naming its reader.* The buff port read DD+0x618 as a
    DAMAGE_DATA_KEY_NAME index. `sdd_walk.py --compact <KEY>` shows it is DIVISION_NUM. The `--all` dump is
    regex-able for a key-to-offset table, but take each offset from that key's own block: a default store or
    stack temp can sit there too.
  - *An inlined or restructured March function is found by its literals, not by a member offset.*
    Process_ExtraDamage's `+0x151` scan was noise because the field is `ExtraDamageData +0x101`. Searching the
    image for March's own UTF-16 literals (`DAMAGE_EFFECT_EDT_FIRE_WRAITH`, ...) and resolving their absolute
    refs found `32034e50`/`327995c0` at once. The same `"MP -"` literal led to the wrong functions
    (DRAIN_MP_REACT), so check which March branch the literal belongs to.
  - *A census row "other-func" is a name match, not a reader.* `DESC`/`NAME`/`VERSION` in `32378e20`/`32680da0`
    belong to AddNPCTemplet/AddNextStepPackage. Resolve the bind name in `31d95080` (the decrypted name just
    before the `push <handler>`) before assigning a file to a function.
