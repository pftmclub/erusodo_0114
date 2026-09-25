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
- `func_strings.tsv`: the `ok` strings grouped by function (`func_va`, count,
  `|`-separated strings, each truncated to 60 chars), derived from `xorstrings.tsv`.
- `ljd_jun2017.py` and `lj_bcmode.py`: the correct June lua decompiler, and the evidence
  for its opcode fix. See "June LuaJIT opcode table".
- `reconcile.py` → `reconcile.md`: data keys against binary keys, per parser.
- `census.py` → `../JUNE2017_EFFECT_KEY_MATRIX.md` + `census.tsv`: the Phase 1 key census.
- `funcmap.tsv`: the June function map, `june_va, march_symbol, status, evidence`. **This
  is the authoritative map;** the section below only summarises it.
- `regen.sh`: rebuilds the gitignored files.
- `xordec*.py`, `xorscan*.py`: the superseded prototypes.

Gitignored, regenerable with `sh regen.sh` (git on Windows doesn't keep the exec bit):

- `funcs.txt`: the full `NAME at ADDR` function listing (`list_functions`), 55,556 lines.
  `xordecrypt.py` and `reconcile.py` need it.
- `methods.txt`: the `methods` endpoint output.
- `ghidra/dis_*.txt`: `disassemble_function` output for specific addresses.
- `june_lua/`: copies of the 14 June effect lua, plus their `.std.luac`/`.src.lua`.
- `pp/`: `census.py`'s `cl /P` output (~20 MB per TU) and flag probe. Rebuilt on every run.

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
