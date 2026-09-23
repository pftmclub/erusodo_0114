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

## Known-bad: the prototype decryptors in this directory

`xordec.py` and `xordec2.py` are the first-pass tools from the investigation that produced
`JUNE2017_EFFECT_REBUILD_PLAN.md`. They work off Ghidra's `disassemble_function` text and a
register-tracking heuristic (MOV r8/r16 ← `[abs]`, then XOR/NOT). They are kept for
reference and as a starting point, **not as a source of truth** — known flaws:

- The last character of a decrypted string is frequently dropped.
- Adjacent encrypted buffers can merge into one decoded string.
- A handful of characters come out garbled where the register-tracking heuristic loses
  track (interleaved loads, reused registers).

Phase 0 of the rebuild plan replaces these with a decoder built on a real x86 disassembler
(e.g. `capstone`, pending the user's OK to `pip install` it) or a hardened emulator of the
idiom above, validated against the controls in the previous section before it's trusted for
anything else.

`xorscan.py` / `xorscan2.py` are whole-binary sweeps built on the same heuristics (byte-wise
and dword/word-wise respectively) — same caveats apply to their output,
`xorstrings.tsv`/(dword scan not yet exported to a file).

## Files in this directory

Committed (small, and not cheaply regenerable — they're the record of what this session's
manual investigation actually found):

- `xordec.py`, `xordec2.py`, `xorscan.py`, `xorscan2.py` — the prototype decryptors, kept as
  Phase 0's starting point (see above; do not trust their output as-is).
- `xorstrings.tsv` — whole-binary byte-wise XOR scan output: `use_va, buf_va, len, plaintext`
  (~7,086 rows, prototype-quality per the caveats above).
- `func_strings.tsv` — decrypted strings grouped by containing function, one row per
  function: `address, count, pipe-separated truncated strings`. Built from the same
  prototype decoder.

Gitignored (regenerable from a running Ghidra instance — see `regen.sh`):

- `funcs.txt` — full `NAME at ADDR` function listing (`list_functions`), 55,556 lines.
- `methods.txt` — `methods` endpoint output.
- `ghidra/dis_*.txt` — `disassemble_function` output for specific addresses of interest.

Run `sh regen.sh` (git on Windows doesn't keep the exec bit) (needs Ghidra open with the project loaded and the GhidraMCP server
running on `127.0.0.1:8080`) to rebuild the gitignored files.

## June function map (confirmed vs. candidate)

Address, role, and how it was identified. All addresses are June (`unpacked_x2.exe`) VAs.
"Confirmed" means an independent check beyond just call order or size (a `__FUNCTION__`
string, RTTI/vtable, or a decrypted key set that matches March's own code for the same
class). "Candidate" means only call-graph position or size pointed at it — a later phase
must confirm or replace it before code is transcribed from it.

### Confirmed

| Address | March equivalent | Evidence |
|---|---|---|
| `0x321f28f0` | `CX2EffectSet::OnFrameMove` | Its 4 calls, in order, match `X2EffectSet.cpp:70-73` (`OnFrameMove_CreateEffect/DeleteEffect/UpdateEffect/StartCameraShake`) |
| `0x321ee600` | `CX2EffectSet::OnFrameMove_CreateEffect` | References the plaintext wide string `L"CX2EffectSet::OnFrameMove_CreateEffect"` at VA `0x3347e4f0` (file offset `0x19dd8f0`), xref from `0x321f2684`. It also references the `__FILEW__` string `D:\SOURCE\INT\source\X2Lib\X2EffectSet.cpp` at `0x3347e460` from `0x321f2777` |
| `0x321e4020` | `OnFrameMove_DeleteEffect` | 2nd call in `0x321f28f0` |
| `0x321e0080` | `OnFrameMove_UpdateEffect` | 3rd call in `0x321f28f0` |
| `0x321dbcc0` | `OnFrameMove_StartCameraShake` | 4th call in `0x321f28f0` |
| `0x321e48e0` | `CX2EffectSet::AddEffectSetTemplet_LUA` | Referenced from the lua_tinker `class_def` bind table inside `FUN_31d95080` (the bind-registration function analogous to March's `X2Main.cpp:4655-4672`); 39,456 bytes, matches the scale of March's 2209-line parser |
| `0x31f7aab0` | `CX2DamageEffect::CEffect::CEffect` (ctor/parser) | Calls `SetDamageData` (`0x32632390`) twice, near strings resembling `"DAMAGE_DATA"` (`0x3346f12c`, `0x3346f158`). Its decrypted key set overlaps March's ctor (`X2DamageEffect.cpp:1240-1690`) heavily: `MAIN_EFFECT_NAME%d`, `DAMAGE_TIME`, `ATTACK_TIME_START/END`, `USE_CANNONBALL`, `EFFECT_TYPE`, … plus June-only keys. Called from both CreateInstance variants below |
| `0x32632390` | `CX2DamageManager::SetDamageData` | Called from `0x31f7aab0` (the CEffect ctor) at two sites and from 7 other call sites across the binary (a shared parser, as in March). Decrypted keys overlap March's `SetDamageData` and add about 60 June-only keys. 11,918 disassembly lines |
| `0x31f7fc60`, `0x31f7ff60` | `CX2DamageEffect::CreateInstance*` variants | Each has one call to the CEffect ctor `0x31f7aab0`. `0x31f7ff60` has 14 callers across the binary (the common entry point) |
| `0x31f88790` | `CX2DamageEffect::CEffect` scalar-deleting destructor | The sole vtable entry at `0x3346f008`, whose COL (`0x33615530`) type-descriptor pointer (`0x337326c0`) matches the RTTI string `.?AVCEffect@CX2DamageEffect@@` at that address |
| `0x31f6ac60` | die/hit damage-effect sub-loader | Decrypted keys include `LOOP_INTERVAL`, `LOOP_END_COUNT/TIME`, `LOOP_ADD_POS_X/Y/Z`, `SET_DIE_WHEN_MASTER_EFFECT_DIE` — matches the March `CreateDieDamageEffect`/`LoadDieDamageEffect` key set plus new keys; called from `0x31f7aab0` |
| `0x31f68f30` | hit damage-effect sub-loader (role from key set). Reached through the wrapper `FUN_31f6e7e0` (2 calls), not directly from `0x31f7aab0`. Confirm the wrapper's caller in Phase 6a | Keys: `INHERIT_LIFE_TIME`, `INHERIT?LOCKON?DATA` (separators garbled by the prototype), `INHERIT_BUFF_FACTOR`, `SET_GROUP_ID`, `LIFE_TIME_ONLY`, `LAND_CRASH_ONLY`, `OFFSET`, `ROTATE`, `ONLY_APPLY_ROTATE` |
| `0x31f66700` | die-condition sub-parser | Keys: `*_DAMAGE_EFFECT_DIE_*`, `CONDITION_TYPE`, `IS_EXIST_DAMAGE_*`, `CHECK_DAMAGE_TIME_VALUE`, `IS_OVER_*`; called from `0x31f7aab0` |
| `0x31f71930` | lock-on data sub-loader | Keys: `NOT_LOCKON_INVINCIBLE_TARGET`, `INIT_TARGET`, `AT_PVP/AT_FIELD/AT_RAID_FIELD/AT_DUNGEON`, `LOCK_ON_TARGET_BONE_NAME`, `LOCK_ON_OFFSET_Y_*`, `NOT_FIND_TARGET_MOVE_STOP`; called from `0x31f7aab0` |
| `0x31f73f30` | sound sub-loader | Keys: `THREE_DIMENSIONAL_SOUND`, `DELETE_WHEN_EFFECT_DIE`, `LOOP`, `USE_GLOBAL_TIME`, `RATE`, `FILE_NAME`, `EVENT_TIME`. Called from `0x31f7aab0` @ `0x31f7e83f` |
| `0x31f74a80` | after-image sub-loader | Keys: `AFTER_IMAGE_*`. Called from `0x31f7aab0` @ `0x31f7f0bb` |
| `0x31f66e60` | particle sub-loader | Keys: `APPLY_START_PARTICLE_SCALE`, `SHOW_LEVEL`, `REVERSE`, `SCALE_BY_MAINEFFECT_SCALE`, `TRACE_DEFENDER_UNIT`, `MOVE_ANGLE_*_PER_FRAME`. Reached through four wrappers (`FUN_31f6a7b0`, `31f6a940`, `31f6aad0`, `31f6cd00`), probably one per particle list (start/passive/time/hit) |
| `0x31f76a90` | passive-particle loader (`PASSIVE_PARTICLE…` keys) | Called from `0x31f7aab0` @ `0x31f7e862`. `0x31f6ceb0` has the same key family, reached through wrapper `FUN_31f6e850` |
| `0x31f637e0` | drag data loader (`CAN_PASS_UNIT`, `DRAG_ONLY_HITTED_UNIT`, `DRAGGING_RANGE/SPEED`) | Called from `0x31f7aab0` @ `0x31f7e7d5`, `0x31f7e7e9` |
| `0x31f64200` | sine-move loader (`SIN_MOVE_*`) — March `LoadSinMoveData` | Called from `0x31f7aab0` @ `0x31f7e7f0` |

The CEffect ctor calls its sub-loaders in a block at `0x31f7e7b2`–`0x31f7f976`:

| Call site | Callee |
|---|---|
| `7e7b2`, `7e7c2` | lock-on `31f71930` |
| `7e7d5`, `7e7e9` | drag `31f637e0` |
| `7e7f0` | sine-move `31f64200` |
| `7e83f` | sound `31f73f30` |
| `7e846` | die-DE `31f6ac60` |
| `7e862` | passive-particle `31f76a90` |
| `7f0bb` | after-image `31f74a80` |
| `7f967`, `7f976` | die-condition `31f66700` |

Walk that block in call order to map the rest of the ctor's sub-loaders.

### Candidate only — confirm before transcribing from these

| Address | Suspected role | Why it's only a candidate |
|---|---|---|
| `0x31f85910` | `CEffect::OnFrameMove` (or a large sub-piece of it) | Largest function in the DamageEffect `.obj` region by far (11,696 bytes, 248 calls, 23 calls stay local to the region) — a size/position argument only |
| `0x31f83aa0` | Some other `CEffect` runtime method | 7,792 bytes, 143 calls, but only 8 stay local — mostly calls out to engine/math code, not diagnostic on its own |
| `0x31fbe4a0` | Unclear — decrypted strings are UI-guide keys (`REPLAY_GUIDE`, `NEXT_GUIDE`, …), **not** DamageEffect/EffectSet related | Originally flagged from region scanning; the string content says this is a different subsystem entirely (likely a quest/tutorial guide parser) that happens to sit in the address range scanned. **Do not use for DamageEffect work** — re-scope or drop in Phase 0/7a |
| `0x31c895d0`, `0x31c8d140` | `KTDGXMeshPlayer` templet parse | Reached via the `__FILEW__` string `...KTDXLIB\KTDGXMeshPlayer.cpp` (`0x3344a530`) cross-references — confirms *a* connection to that file, not which function does the templet key parsing specifically |
| `0x32057b80` | Possibly out of scope — user/NPC state script parser | Has `ATTACK_TIME`-adjacent strings but may not be DamageEffect proper |
| `0x32784c00` | Unit script (`EFFECT_SET_LIST` etc.) | Likely a different binding layer (per-unit effect-set list), not the EffectSet engine itself |

### Region survey (functions ≥1500 bytes, for Phase 7a's identification work)

`EffectSet` object region `0x321d0000`–`0x32208000` and `DamageEffect` object region
`0x31f50000`–`0x31f90000` — full listing with sizes is in `funcs.txt` (regenerate with
`regen.sh`) filtered through the snippet in `regen.sh`'s comments. The confirmed/candidate
tables above are the ones this investigation actually looked at; there are more functions in
each region nobody has looked at yet.

## Method notes (things that went wrong, so the next phase doesn't repeat them)

- `curl --noproxy *` (unquoted) gets glob-expanded by the shell and silently returns a
  Cloudflare 302 instead of hitting Ghidra. **Always quote it**: `--noproxy '*'`.
  `.claude/skills/ghidra/SKILL.md` still has the broken unquoted form as of this writing —
  fixing it is a Phase 0 task, pending the user's OK to edit a skill file.
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
