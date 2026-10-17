# Lua source: core client Lua as proven plaintext source, the rest as June's bytecode

Plan key `lua-source` · branch `main` · log tag `[LUA_SRC]` · flag `SERV_IRUHADEV_LUA_SOURCE` (+ `_DEBUG`, L6 only) ·
kickoffs `kickoffs/lua-source/` · skills `x2-lua`, `x2-adopt-june` (built here) · created 2026-10-09.

## Context

The project ends with June's functionality and assets running on the March client. Every March `.lua` is eventually replaced
by June's file at the same name, or dropped where June removed the feature.

Lua blocks that today:
- June files are ported by decompile, edit and recompile, and decompile defects leak in. Examples are dropped assignments
  (`RANOX_LUA_DECOMPILE_BUG.md`) and the Dryad Kenta softlock. They are the reason `june_src_fixed/` exists.
- Editing a bytecode-only March file needs a recompile nobody has proven equivalent. This is C3d-3/C3d-4's blocker for the nine
  class scripts.

The user wants this settled once, so nobody has to think about it again (2026-10-09). End state:

- **Scope: source only where a file must differ from June (user, 2026-10-09, rescope after L2).** Most June `.lua` ships as June's
  own bytecode transcoded by `ljconv.py` (the "adjusted June lua": header `0x82` -> `0x81` and opcode bytes renumbered, nothing else).
  This is already how 3,638 files (3,518 + 120) shipped on 2026-10-09 (`a5f7298`, `c6c8c58`, MODS "June NPC and dungeon Lua via ljconv" and
  "June partial ports replaced"). A file becomes BOM source only if it is in the **core list** (Rule 2):
  - it is **kept merged** because March's C++ lacks something June's file needs, or June's file lacks something March's C++ needs
    (today: `Enum`, `DungeonEnum`, `DamageEffect`, `EffectSet`, the three `DLG_Map_*` dialogs, `NPCTemplet`, `DungeonData`,
    `DLG_Destination_byButton`, with their blockers in MODS);
  - or a port **must edit** it (a March-compat edit to a June file, or a March file that needs a change before June's replacement
    can land).

  In kom terms (checked 2026-10-09) the core list sits in data036 (`Enum`, `DungeonEnum`, `DamageEffect`, `EffectSet`; class scripts
  and templets when a port must edit them) and data034 (the three `DLG_Map_*` dialogs; its nine `*_Back_NEW` sidecars are classified in L3; other
  dialogs only where June's calls a UI binding March lacks), with single files in data035 (`DungeonData`), data037 (`NPCTemplet`) and
  datai014 (`DLG_Destination_byButton`). The kom is a pointer, not the rule.
- **June `.lua` arrives through one command, `adopt-june` (skill `x2-adopt-june`).** The default is the `ljconv` bytecode, with no
  decompile. `--source` (core-list files only) adds decompile + T1 against the `ljconv` output, and ships BOM source on a pass.
  L2 verified `ljconv`'s opcode table against June's own VM handlers (71/71).
- **March `.lua` is not mass-converted**, because June overwrites it. A March file is converted, with proof, only when it is on
  the core list (`convert`, skill `x2-lua`).
- **Why the core files are worth source:** every shipped bytecode file, March and June, is stripped (flags `02`, checked
  2026-10-09 over data034/035/036/037/datai016), so L6's line-numbered Lua errors only exist for source files. A failing dungeon
  script without a line is cheap to chase; a failing `DamageEffect` is not.
- **The tools enforce it.** `kom.py pack` refuses anything but parseable BOM source or a ledger-listed bytecode exception. The
  engine logs Lua errors with line numbers.
- **One-way.** Once converted, a file stays source. The bytecode snapshot exists for rollback only.
- **Every tool is also a skill**, so a future session reaches it by `/x2-lua`, `/x2-adopt-june`, or plain words.

**This plan builds tools; it does not do other plans' conversions.** june-effect C3d-4 is about the `*_DAMAGE_BOUNDING_*` tables
missing from the nine class scripts. It closed on the "wait" route (`fdcf60b`, user): each character port adopts June's class
script with `adopt-june`. The tables are inline in June's file, so they arrive with it; the per-class C++ registration call is
`JUNE2017_CHARACTER_PORT_CHECKLIST.md` §7l. (The "add now" route, `convert` on the nine March class scripts, was not taken.)

**User decisions (2026-10-09):**
- Keep the LuaJIT VM; there is no PUC 5.1 revert.
- Unprovable files stay bytecode, with a named reason.
- Sources live in kom_contents only, not mirrored into git.
- ~~This is the permanent, June-focused version, not convert-on-touch alone.~~ **Superseded the same day (user, 2026-10-09,
  after L2):** source is scoped to the core list; everything else is June's `ljconv` bytecode ("the bytecode changed header one").
  L3 is rescoped, L4 is dropped, L5 is limited to core-list files.
- The class-script conversion stays in june-effect.
- Every tool is a skill.

**Correction to the starting premise.** The older (January) engine did **not** read plaintext either. It read XOR'd PUC Lua 5.1
bytecode; see "Established facts".

## Rules

1. **Proof tiers.** Only T0 and T1 convert automatically.
   - **T0:** the source compiles byte-identical to the reference bytecode.
   - **T1:** the recompile is identical per prototype:
     - opcodes, registers, child-proto tree, upvalues, numparams, framesize and flags are all equal;
     - constants are compared by value, byte for byte, and constant-slot order is ignored;
     - a table constant counts as its key/value pairs: the array part position by position, the hash part as a set, so their storage order (what `pairs` walks) is ignored (user, 2026-10-09: "as long as the key-value pairs are not in the wrong parent"); a pair in another table is a difference.
   - **T2:** behaviour-equal only. It needs an Opus reader audit and the user's yes.
   - **T3:** unproven. The bytecode ships.

   The reference is the shipped March bytecode for March files, and `ljconv(June bytecode)` for June files.
2. **June `.lua` only through `adopt-june`; source only for the core list (rescope, user 2026-10-09).**
   - **Default:** junekom extract, then `ljconv`. June's bytecode ships as `1B 4B 4C 81`, with no decompile and no edit. The run is
     recorded in a per-run TSV under `june2017_re/` (the shape of `june_npc_lua_port_20261009.tsv`), not in the ledger.
   - **Core list** (`tools/data/lua_core_list.tsv`, built in L3): a file that is kept merged or that a port must edit. Only these
     take `adopt-june --source`: decompile, T1 against the `ljconv` output, then BOM source plus a ledger row. If T1 fails, nothing
     is staged (June's bytecode would drop the merge or the edit); the core-list row records the reason, and the file goes to L5 or
     a T2 sign-off. The default route refuses a core-list name.
   - Adding a file to the core list needs a reason in its row (the blocker, or the port and the edit). A port that finds it must edit
     a June file adds the row itself and carries on; no question (user, 2026-10-09). This is the one exception to Rule 8's byte-exact
     June rule, and the row is the record of it.
   - Never decompile or compile by hand. `june_src_fixed/` is reading material.
3. **Editing a March `.lua` starts with `convert`** (the file is on the core list by definition). Sources are tried in this order:
   - the sibling `X.src.lua` (T0);
   - a studio original from `ClientScript\` or `_ClientScript\` (T0);
   - a T1 decompile.

   Afterwards `X.lua` **is** BOM + UTF-8 source, with the BOM stored on disk, and every later change is a text edit.
4. **Conversion changes format only.** Content changes are the port's own diff, made after conversion.
5. **The pack gate enforces it.** Every `.lua` must be BOM text that parses under the vendored luajit, March `1B 4B 4C 81` bytecode
   (which is also what `ljconv` writes), or a bytecode file listed as an exception in the ledger. June `0x82` bytecode is refused.
   - **Exception: server-only Lua that the user packs by hand** (CLAUDE.md's server-Lua rule, which stays as written). These files
     are manifest `"0"` and loaded with `bEncryption=false`, and they may be PUC-5.1 text LuaJIT rejects. The gate only warns on them,
     never refuses.
6. **Bookkeeping.**
   - Original bytecode is snapshotted to `24965799\kom_contents_bytecode\dataNNN_kom\` (on F:).
   - The ledger `tools/data/lua_source_ledger.tsv` has one row per converted or adopted file, with these columns: kom, name,
     origin (march / june), reference, reference sha1, source, tier, source sha1, phase, date, reason.
7. **June's binary is open for reference (user, 2026-10-09).**
   - Any phase may look into June's `unpacked_x2.exe` with the `ghidra` skill for logic that helps. Useful targets: the
     opcode/handler mapping (L2), June's loader and Lua handling (L3, L6), and how June treats a case March gets wrong.
   - Read-only: rename or comment only when it helps later sessions, and record addresses in `june2017_re/funcmap.tsv` with the tag
     `LUA_SRC`.
   - Cite the June address for every conclusion drawn from it.
8. **Standing rules still apply.**
   - No new `.lua` files (CLAUDE.md). Conversion keeps every name.
   - June files are byte-exact (`june-assets-byte-exact-no-edits`), except a `.lua` on the core list that a port must edit (Rule 2).
   - Ask before every `/x2-kom-packed copy`.
   - Edit CP949 sources with `bytepatch.py` only.

## Starting context

Every repeated step is a tool (`tools/README.md`). Do not hand-type msbuild, elstools, kom packing or log greps.

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py LUA_SOURCE_PHASES.md "<heading>"` |
| Pack / deploy a kom | `/elstools pack dataNNN`, then ask, then `/x2-kom-packed copy dataNNN` (`tools/kom.py`) |
| Inspect / extract a March kom | `elstools/elstools.exe inspect -q -i <kom>` / `extract` (absolute paths, `-q`, stdin closed, `-v mar-2014`) |
| June kom reader (byte-exact) | `python tools/junekom.py list / extract / verify ...` |
| Stage a June file (placement, dupes) | `python tools/kom.py stage-june <name> [--dry-run]` |
| June -> March bytecode transcode | `python june2017_re/ljconv.py OUTDIR <june .lua ...>` |
| Fixed decompiler (June) | `python june2017_re/ljd_jun2017.py ... [--check]`; `june2017_re/ljd_fixed.py` |
| Vendored compiler | `elstools/vendor/luajit/luajit.exe` (LuaJIT 2.0.2, KOG build); `-b - -` through pipes works (undo CRLF on stdout) |
| June binary (Ghidra, read-only reference) | `ghidra` skill (curl to `http://127.0.0.1:8080`); June addresses in `june2017_re/funcmap.tsv` |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply <spec>` (x2-byte-edit) |
| Flag hygiene | `python tools/flagcheck.py SERV_IRUHADEV_LUA_SOURCE --worktree --files <files>` |
| Build + deploy | `python tools/build.py --deploy` (background; x2-build-deploy) |
| Play-test logs | `python tools/logs.py --tag LUA_SRC` (x2-check-logs); `KEM_ERROR72/73` lines are in `ErrorLog.txt` |
| After L1 / L3 | `/x2-lua ...`, `/x2-adopt-june ...` |

**Paths:**
- **March staging:** `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents\dataNNN_kom\`.
- **March game dir:** `...\24965799\data`.
- **June data (read-only):** `...\237311\14JUN17\data\`.
- **Studio originals (read-only):** `F:\elsword stuff\elsword_2014\els_2014\ElswordFiles\Elsword\Elsword\ClientScript\` and
  `...\_ClientScript\`.
- **Intermediates** go to `.build\` or F:, never the session scratchpad: C: is short of space.

## Established facts (2026-10-09, planning session; three Explore passes + direct checks)

**The engine already loads XOR-encrypted plaintext, so no VM migration is needed.**
- **Load path.** `CKTDXApp::LoadAndDoMemory_ErrorCode` (`KTDXLIB/KTDXApp.h:530-675`) runs in this order:
  1. `LoadDataFile(name, true, bEncryption)` keeps the raw compressed bytes (`KGCMassFileManager.cpp:907-910`).
  2. `XORCRCDecrypt` over the compressed size (`KTDXApp.h:585-613`). The key is 20 bytes,
     `{XOR_KEY30,42,14,22,40} ^ XOR_KEY49` (`KTDXCommonFunc.h:747-801`), seeded by the lowercased member name.
  3. `uncompress`.
  4. `DoMemoryNotEncrypt`.
- **Archive layer.** The mass file manager never decrypts, and the client ignores the manifest `Algorithm` attribute. "Algo 2" is the
  packer's term for zlib followed by XORCRC.
- **Local files** (`MFI_LOCALFILE`) skip decryption, and they beat the kom under US_SERVICE.
- **Binder gate.** `LuaCommon/KLuabinder.h:24-61` and `LuaCommon/KLuaManager.h:545-588` are the only text-vs-bytecode branch.
  Encoding is set to UTF-8 at `LuaCommon/KLuabinder.cpp:28`, so the gate's non-legacy path is live:
  - first byte `0x1B`: the buffer goes straight to `lua_tinker::dobuffer`;
  - starts with `EF BB BF`: the BOM is stripped and the rest is parsed;
  - anything else: **returns false with no log**. Only `KEM_ERROR73 <file>` (`KTDXApp.h:649-657`) reaches ErrorLog.
- **Into LuaJIT.** `lua_tinker::dobuffer` (`LuaCommon/lua_tinker.cpp:184-210`) calls `luaL_loadbuffer` with mode NULL. `lj_lex_setup`
  then picks the bytecode reader or the parser per chunk (`lj_load.c:28-58`, `lj_lex.c:419-437`).
- **The parser is linked.** luajitLib `Release` compiles `lj_lex.c` / `lj_parse.c`, and `X2/US_SERVICE/X2.map:49283` has `_lj_parse`.
  `X2OPTIMIZE_REMOVE_LUA_INTERPRETER_MODULE` (`KTDXLIB/KTDX.h:398-400`) is only defined under `_SERVICE_` /
  `_IN_HOUSE_SERVICE_READY_QA_`, neither of which US_SERVICE defines. **If a future "stage 2" defines `_SERVICE_`, text loading
  breaks** (`lj_load.c:37-43`); L6 guards against that.
- **The KOG encoding patch does not touch the lexer.** It only affects `luaL_loadfilex` and `print`: `lj_load.c:119-140`,
  `lj_state.c:291-309`. The bytecode header is `1B 4B 4C 81` (`lj_bcdump.h:31-48`, `luaconf.h:13`), with stock opcode order.
  `elstools`' LuaJIT 2.0.2 `-b` output is therefore the bytecode the client builds in memory from the same source.
- **No C++ reads Lua bytecode.** The other text readers are `KLuaManagerProxy::DoMemoryNotEncrypt` (`GameOption.txt`,
  `TutorSystem.lua`), the particle / mesh-player `.txt` compilers, and binary TET/XET/item.kim. None of them is affected.
- **Error visibility.**
  - Syntax and runtime errors go `print_error` -> `_ALERT` (unregistered) -> `printf` (`lua_tinker.cpp:253-273`), so they are invisible.
  - The chunk name is always `"lua_tinker::dobuffer()"`.
  - Shipped March bytecode is stripped (flags `02`), so it carries no line numbers. Source does.
- **June hides Lua errors the same way** (Ghidra, `unpacked_x2.exe`, 2026-10-09).
  - `print_error` is `FUN_32fe7730`. It looks up the global `"_ALERT"` (`3355f880`), and that string's only xref is this read, so
    nothing in June registers `_ALERT`.
  - `lua_tinker::dobuffer` is `FUN_32fe8880`, same shape as March's. It also latches the first error message into a function-local
    static (`DAT_337b2714`) that nothing reads.
  - So L6's error logging is a March-side diagnostic with no June counterpart to port; it is not a parity change.
- **Build traps for engine work.**
  - `LuaCommon/` is gitignored (`.gitignore:11`).
  - `lua_tinker.cpp`, `KLuabinder.cpp` and `KLuaManager.cpp` compile inside luajitLib (`luajitLib.vcxproj:489-491`), which never
    sees `Always.h`.
- **Large data scripts.**
  - JIT is off globally (`KLuabinder.cpp:30`); only shareable NPC scripts turn it on (`X2UnitManager.cpp:747-749`).
  - Each loader thread has its own binder (`KTDXApp.cpp:499-511`).
  - Source is about 1.5-2x the bytecode size; parse cost has not been measured yet (L6 `_DEBUG` does it).

**The older engine did not read plaintext.**
- All **4,106** `.lua` entries in every kom of `237311\22191271\data` are `zlib(XOR12(PUC Lua 5.1 bytecode))`. So are the 152 loose
  files in its `data036\`. The key is `02 AA F8 C6 DC AB 47 26 EF BB 00 98`; the header is `1b 4c 75 61 51`.
- January linked `luaLib` (PUC 5.1). Its 12-byte `XORDecrypt` ran in `DoMemory`, before zlib (`git show dd63297^:KTDXLIB/...`).
- HEAD's `XORDecrypt` uses the 5-int key, so January payloads cannot be decrypted by current code.

**March inventory** (`kom_contents`, 191 dirs, 15 with `.lua`):
- **4,851 `.lua`, 91.5 MB.**
- **4,849 are KOG `1b 4b 4c 81`**: stripped, manifest `"2"`.
- **Two oddities at `"0"`:**
  - `data014 UI_NPC_KELAINO.lua` is June `0x82` bytecode, unloadable today.
  - `data036 PvPDropTable.lua` has the January XOR baked in, and no loader reads it.
  - Both stay as ledger exceptions.
- **2,408 files have exact (T0) source:**
  - 334 `X.src.lua` (all compile byte-exact to their `X.lua`; 268 are earlier June-port additions);
  - 2,072 studio originals in `ClientScript\` (4,077 BOM files, 2014-01-25);
  - 2 in `_ClientScript\`.
- **2,443 need a decompile.**
  - By kom: data034 879, data037 661, data040 563, datai014 99, data036 91, datai016 78, data035 36, data001 13, datai015 12,
    data039 8, and data014 / data119 / datai022 1 each.
  - The data036 share includes the nine class scripts, RandomItemTable (4.66 MB), NewSkillTempletVer2, PetTemplet, TitleTable
    and StringFilter.
- **Fully T0 dirs:** data038 (2, ClientScript) and data217 (6, `.src.lua`: the Ranox / Special local-map dialogs).

**Class scripts (C3d-4).** The nine March class scripts are bytecode-only. C3d-4's evidence came from `luajit -bl` listings plus a
mock-environment dump of a decompile + recompile:
- eight classes show 0 opcode and 0 resolved-constant differences;
- Ara has one TDUP-vs-TNEW table constructor;
- whether anything iterates that table order-dependently is unchecked.

**Tooling state.**
- `june2017_re/ljconv.py` is an exact June -> March transcode. It rewrites the header byte and each instruction's opcode byte only,
  and is asserted against June's own `lj_bc_mode`.
- `tools/junekom.py` extracts June byte-exactly (june-asset-extract X0-X5). Old `elstools` extractions of June V.0.4 algo-2 entries
  are 1-255 bytes short; never use those.
- March decompile route: header swap `1b4b4c81` -> `1b4c4a01`, then `june2017_re/ljd_fixed.py`.
- `ljd_jun2017.py:125` and `ljdiff.py:212` refuse non-June input. **No instruction-level (T1) checker exists** for March or June.
- Two Explore passes disagreed on whether the slotworks fix is baked into both vendored ljd copies. L2 checks.
- **Known ljd printer defects** (`RANOX_LUA_DECOMPILE_BUG.md`, "Residual ljd defects"):
  - an IIFE printed without parens;
  - `]]` inside a long string (NewSkillTempletVer2);
  - deepcopy-hoist;
  - TIMEOUT / MemoryError on huge files.
- **`kom.py`.**
  - `compile` turns a stale `X.src.lua` into bytecode. Some `.lua` were made another way (June `deepcopy` files via `ljconv`), so
    blind recompiles are wrong.
  - `pack` packs every file except `*.src.lua` and `_manifest.json`, and has no BOM or parse check.
- **Lanox evidence.** 267 BOM-less plaintext `.lua` once shipped and half-failed (`LANOX_TROUBLESHOOTING.md:12-19, 266-270`). That
  matches the silent BOM gate above.

## Phases

| Phase | What | Run with | Depends | Touches | User owes |
|---|---|---|---|---|---|
| L0 | Pilot: encrypted source through a real kom (no code) | Sonnet 5.5 high | - | kom_contents/data217 | copy OK; play-test |
| L1 | `luasrc.py` convert / restore / ledger, pack gate, compile fold, **skill `x2-lua`** | Sonnet 5.5 high | L0 | `tools/kom.py`, `tools/luasrc.py` (new), `tools/data/lua_source_ledger.tsv` (new), `.claude/skills/x2-lua/` (new), `.claude/skills/elstools/`, `.claude/skills/x2-phase/`, CLAUDE.md, `tools/README.md` | - |
| L2 | Equivalence checker (March + June modes); `x2-lua` gains `check` / `decompile` | Opus 5.5 **xhigh** | L1 | `tools/luasrc.py`, `june2017_re/ljdiff/`, `.claude/skills/x2-lua/` | - |
| L2-review | Adversarial review of the checker | Opus 5.5 high | L2 | review only | - |
| L3 | Core list + `adopt-june` (default `ljconv`, `--source` for the core list) + **skill `x2-adopt-june`** | Sonnet 5.5 high | L2-review | `tools/luasrc.py`, `tools/data/lua_core_list.tsv` (new), `.claude/skills/x2-adopt-june/` (new), CLAUDE.md | confirm the core list |
| L4 | ~~Audit of existing June ports~~ **dropped** (rescope 2026-10-09: the 120 partial ports are already June's `ljconv` bytecode) | - | - | - | - |
| L5 | Decompiler fixes for core-list files only (conditional on L3) | Opus 5.5 **xhigh** | L3 | `june2017_re/` ljd hooks | - |
| L6 | Engine: errors visible, parser kept | Opus 5.5 high | L0 | `KTDXLIB/KTDX.h`, `KTDXLIB/KTDXApp.h/.cpp`, `KTDXLIB/Always.h` | play-test via dev console |
| L6-review | Review of L6 | Opus 5.5 high | L6 | review only | - |
| L7 | Close-out | Sonnet 5.5 high | L3, L6-review (+L5 if run) | docs, kickoffs, skills, memory | - |

**Running together:**
- L6 alongside L1/L2 (engine files vs tools).
- june-effect C3d-4 closed on route 2 (`fdcf60b`): the character ports bring June's class scripts. A class script a port must edit
  is a core-list file and takes `adopt-june --source`; one that loads unchanged takes the default.
- Builds and copies queue as usual.

## Phase L0 - Pilot: encrypted source through a real kom

**Scope.** Prove, with no code, that a BOM-source `.lua` survives `kom.py pack` and loads in game. data217's six Ranox / Special
local-map dialogs all have an exact `.src.lua`, so semantic risk is zero.

**Run with:** Sonnet 5.5 high. **Read list:** Rules, Starting context, Established facts.

1. **Confirm T0.** Pipe-compile each `.src.lua` (`luajit -b - -`, mar-2014 header) and compare it with the shipped `X.lua`. Stop on
   any mismatch.
2. **Snapshot.** Copy all six `X.lua` and `X.src.lua` to `24965799\kom_contents_bytecode\data217_kom\`.
3. **Stage.** Write `EF BB BF` + the `.src.lua` text as `X.lua`, and remove the sidecars (the snapshot holds them). The manifest
   stays `"2"`.
4. **Pack.** Run `/elstools pack data217` and quote `PACK OK`.
5. **Decode back.** `elstools extract` the packed kom into `.build\lua_source_l0\` and byte-compare every entry with the staged
   file. This also settles `LANOX_TROUBLESHOOTING.md:258`'s claim that the packer compiles on the fly.
6. **Rollback.** Restore the snapshot into a scratch copy of the dir, and confirm a pack of it equals the pre-pilot packed kom's
   entries.
7. **Copy.** Ask, then `/x2-kom-packed copy data217`, and quote `COPY OK`.

**Done gate:**
- decoded = staged for all 6;
- the play-test passes: world map, then the Ranox local map and the Special local maps, with icons and buttons as before;
- `ErrorLog.txt` has no `KEM_ERROR72/73` naming a data217 file.

Write a Done note, start the ledger with these 6 rows (tier T0, source `src.lua`, phase L0), and commit.

**Done note (2026-10-09, Sonnet 5.5): L0 closed - gate passed, play-test passed.**
- **T0:** all 6 `.src.lua` compile byte-identical to the shipped `X.lua`. The vendored luajit needs `LUA_PATH=<luajit dir>\?.lua` or it
  dies with "jit.* modules not installed"; its stdout CRLF is the usual artefact, undone before comparing. The sidecars are CRLF with no BOM.
- **Snapshot:** `24965799\kom_contents_bytecode\data217_kom\` (all 13 files, sha1-verified against the originals before staging) and
  `data217_packed_prepilot.kom` beside it (md5 equals the live kom's before the copy).
- **Stage and pack:** each `X.lua` is `EF BB BF` + the sidecar text; sidecars removed; manifest unchanged (`"2"`).
  `PACK OK data217`: 6 entries, 8047 bytes, algos `{'2': 6}`.
- **Decode back:** `elstools extract` of the packed kom equals the staged files, 6 of 6 (`.build\lua_source_l0\decoded`). The packer
  does not compile on the fly (settles `LANOX_TROUBLESHOOTING.md:258`).
- **Rollback:** a scratch copy of the snapshot packs to a kom byte-identical to `data217_packed_prepilot.kom`.
- **Copy:** `COPY OK data217 ... 8047 bytes, 6 entries, md5 9fb43f49f066485bd080bd86f722ee6b`; the old kom is in
  `kom_backup_tools\data217.kom.20261009_083925`.
- **Ledger:** `tools/data/lua_source_ledger.tsv` started with the 6 rows (`source_sha1` = sidecar text without the BOM).
- **Play-test (user, 2026-10-09):** world map, Ranox, Special and Peita local maps looked as before. Logs (run started 08:41, after the
  08:39 copy): `ErrorLog.txt` has 0 `KEM_ERROR` lines; `OpenScriptFile.log` shows `DLG_Map_Local_Special_Back_NEW.lua` (data217) opened at 08:42:8.
  The two `E_DISABLE_HB_CHECK_REQ` UNHANDLED lines are unrelated. L0 proves BOM source survives pack + load; L1 and L6 are unblocked.

## Phase L1 - Conversion tooling, pack gate, compile fold, skill `x2-lua`

**Scope.** Write the conversion tool and its skill, and make `kom.py` enforce the format.

**Run with:** Sonnet 5.5 high. **Read list:** Rules, Starting context, Established facts, Phase L0's Done note.

1. **`tools/luasrc.py`**, stdlib Python driving the vendored luajit:
   - `find dataNNN X`: lists the exact sources (`.src.lua`, ClientScript, `_ClientScript`; duplicate names are tried copy by copy)
     by pipe-compiling and comparing.
   - `convert dataNNN X ... [--dry-run]`:
     - picks a source by Rule 3, T0 only until L2;
     - snapshots the bytecode, writes BOM + text to `X.lua`, folds the sidecar, appends the ledger row;
     - refuses (exit 4) without a proof, and says why.
   - `restore dataNNN X ...`: puts back the snapshot bytes and marks the ledger row `restored`.
   - `ledger [--check]`: census and consistency. Every ledger row's `X.lua` must match the ledger sha. Every BOM-text `.lua` in
     kom_contents must have a row.
2. **`kom.py pack` gate (Rule 5).**
   - Each `.lua` must be either `1B 4B 4C 81` bytecode or BOM + text that parses (`luajit -b` to null).
   - It refuses BOM-less text, `0x82` bytecode, anything else, and any text that fails to parse. The refusal names the file and the
     parser's message.
   - The two oddities are seeded as ledger exceptions.
   - Manifest-`"0"` `.lua` files (user-packed server Lua, Rule 5's exception) get a warning, never a refusal. The CLAUDE.md
     server-Lua rule survives the docs rewrite verbatim.
3. **`kom.py compile` repointed** to fold `X.src.lua` into BOM-source `X.lua`. It no longer produces bytecode; `--bytecode` stays
   for rollback only.
   - It folds when the `.src.lua` is T0 against the current `X.lua`, or newer than it (an intended edit, reported as such).
   - It refuses when the `.src.lua` is older and not T0, e.g. a stale decompile beside an `ljconv` file.
   - Every existing kickoff that says `kom.py compile` (june-effect Enum, UIEffectSet, DamageEffect, ...) now yields source.
4. **Skill `.claude/skills/x2-lua/SKILL.md`**, in the same shape as `x2-kom-packed`:
   - frontmatter `name` and a trigger-rich `description` ("edit / convert a March .lua", "is this lua proven", "decompile X",
     "/x2-lua");
   - copy-ready commands and the tier rules in two lines;
   - what each refusal means and what to do next;
   - "never decompile/compile by hand";
   - `check` / `decompile` marked "after L2".
5. **Docs.**
   - CLAUDE.md: replace the bytecode-workflow lines with Rules 1-6, and add an `x2-lua` row to the *Workflow automation* table.
   - The `elstools` skill: the pack gate and the compile fold.
   - The `x2-phase` Tools table.
   - `tools/README.md`.

**Done gate:**
- in a scratch dir, pack refuses a BOM-less text file, a `0x82` file and a syntax error, each with a clear message;
- `compile` on a scratch copy yields BOM source;
- `convert` then `restore` round-trips byte-exact;
- `convert --dry-run` over all 15 dirs reports the T0 count (2,408 minus data217's 6) and changes nothing;
- `ledger --check` is clean;
- the skill appears in Claude Code's skill list, and its example commands run as written on a scratch copy.

No kom copy in this phase.

**Done note (2026-10-09, Sonnet 5.5): L1 done - tools, gate, fold, skill and docs landed; no live kom touched.**
- **Files:** `tools/luasrc.py` (new: `find` / `convert` / `restore` / `refresh` / `ledger [--check]`, plus the `gate()` and `fold()`
  that `kom.py` imports), `tools/kom.py` (Lua pack gate; `compile` folds, `--bytecode` keeps the old path), `.claude/skills/x2-lua/`
  (new), CLAUDE.md (*Rule: client `.lua` is BOM source*, a *Workflow automation* row, the line-27 tail; the server-Lua exception
  sentence is byte-identical), the `elstools` and `x2-phase` skills, `tools/README.md`. No engine code, so no flag, no build.
- **Compile mechanics:** every compile is file to file from `.build/luasrc_tmp/` with the BOM stripped by us (the stdout pipe adds
  CRLFs: 1151 vs 1150 bytes on a data217 file). The standalone luajit also accepts a BOM file, but we never rely on it.
- **Ledger:** gained a 12th column `state` (`live` / `restored`; the L0 rows read as `live`) and two seeded exception rows, tier `EXC`
  (`data014 UI_NPC_KELAINO.lua`, June 0x82; `data036 PvPDropTable.lua`, January XOR). `--check` verifies an EXC row against its
  `reference_sha1`, demands no BOM row for it, and a BOM-text file needs a live row whose `source_sha1` matches (else `refresh`).
  A fold of a newer sidecar records tier `EDIT` (no equivalence claimed). Real ledger: `LEDGER CHECK PASS: 0 FAIL, 0 WARN`.
- **Gate evidence** (scratch tree under `.build/l1/scratch`, all three roots overridden so the real ledger and snapshots were never
  written): pack refused, exit 4, nothing packed, naming each file: BOM-less text (`... binder returns false with no log`), a June
  0x82 file listed `"2"`, and a syntax error (`line 3: unexpected symbol near '='` on a 3-line file). A manifest-`"0"` 0x82 file only
  warned; after the three bad files were removed `PACK OK` (3 entries, algos `{'0': 1, '2': 2}`). The real gate over all 15 kom dirs:
  0 refusals, 2 exception notes.
- **Fold evidence:** a sidecar that compiles byte-identical folds as T0; a newer sidecar folds as `EDIT`; an older non-T0 sidecar is
  refused (`stale sidecar beside a converted file`); a sidecar with a syntax error is refused; each fold snapshots the old bytecode.
- **Round trip:** `convert` then `restore` over 4 files (two with sidecars, two ClientScript originals) left the scratch dirs
  byte-identical (`diff -r` clean); restore also brings the sidecars back; ledger rows go to `restored`.
- **Dry run, all 15 dirs (real tree):** `CONVERT DRY RUN: 2402 T0 convertible, 2441 need L2, 8 skipped; nothing changed.` Breakdown
  of the 2,402: 328 sibling `src.lua` (334 minus data217's 6) + 2,072 ClientScript + 2 `_ClientScript`, which is exactly the plan's
  2,408 minus the 6 already converted. The 2,441 are the plan's 2,443 minus the two oddities (skipped as not March bytecode). A
  sha1 of every `.lua` / `.src.lua` (path, bytes, mtime) in the 15 dirs plus the ledger was identical before and after.
  By kom (T0): data037 1249, data040 619, data035 241, datai016 81, data039 68, data034 68, data036 65, datai015 6, datai014 3, data038 2.
- **Skill:** `x2-lua` appeared in this session's skill list right after the file was written, so the list picks it up live. The
  example commands in `SKILL.md` were run as written against the scratch copy (`find`, `convert`, `restore`, `refresh`,
  `ledger --check`, `kom.py compile`, `kom.py pack`).
- **Known edges (not defects):** whole-kom `convert` without `--dry-run` converts every provable file in that kom (name files, or go one
  kom at a time; `--all` needs `--dry-run`). Sidecar-vs-`X.lua` age uses mtime, so a deliberate sidecar edit must be saved after the
  last `X.lua` write (the fold says so when it refuses).
- **Open question for the user:** none. Next: L2 on Opus 5.5 xhigh (`/x2-phase lua-source L2`); L6 can run alongside.

## Phase L2 - Equivalence checker (March + June)

**Scope.** Build the instruction-level proof that makes decompiles shippable. This is the core engineering of the plan.

**Run with:** Opus 5.5 **xhigh**. **Read list:** Rules, Established facts, Phase L1's Done note, plus `RANOX_LUA_DECOMPILE_BUG.md`
sections "Residual ljd defects (Phase 8, the remaining ~4,200 files)" and "Status".

1. **ljd fix state.** Compare the SHA of both vendored `ljd/ast/slotworks.py` copies with `june2017_re/ljd_slotworks.patch`'s recorded
   post-patch SHA, and record the answer.
2. **`luasrc.py decompile <bytecode>`.**
   - March: header swap to `1b4c4a01`, then `ljd_fixed.py`.
   - June: the `.std.luac` route via the fixed `ljd_jun2017.py`.
   - Output gets a BOM. Intermediates go to `.build\`.
3. **T1 check** (`luasrc.py check <source> <reference bytecode>`). Write a dump parser in Python (`lj_bcmode.py`, `funcdis.py` and
   `ljdiff/bcfn.lua` are prior art). For every prototype, in tree order, compare:
   - flags, numparams, framesize;
   - the upvalue descriptors;
   - each instruction's opcode and A/B/C/D operands. Operands that index constants (KSTR, KNUM, KCDATA, TDUP templates, FNEW child
     index, GGET/GSET/TGETS/TSETS keys, ...) are compared by the **resolved value**, byte for byte, not by slot number.

   Anything else is a diff. The modes are `--ref march` (shipped `0x81`) and `--ref june` (`ljconv` output of June's bytecode).
4. **T2 report.**
   - Run both chunks in a mock environment and dump every global table in **raw `next` order**, not sorted, so a layout change shows.
   - Add a ljdiff mode that accepts `0x81` (drop the June-only refusals at `ljdiff.py:212` / `ljd_jun2017.py:125` behind a flag).
   - T2 only reports. It never passes a file.
5. **Mutation self-test** (`luasrc.py selftest`). Seed edits into known-T1 decompiles and require each one to fail T1:
   - change a number or string constant;
   - flip one byte of a string;
   - drop a statement;
   - change an operator;
   - reorder a table constructor;
   - turn a local into a `slotN` global;
   - swap two constants of the same type.
6. **Calibration on ground truth.**
   - Decompile **at least 200 T0 files** (`.src.lua` / ClientScript-exact) spread over all 15 dirs. Every T1 miss on a file whose
     exact source is known is a ljd or checker bug, and each must be explained.
   - Then run the 14 `june2017_re/june_lua/` files (`--ref june`) and the **nine class scripts** (`--ref march`). Record the class
     verdicts for C3d-4; nothing is converted.
7. **Opcode-table independence (June mode).** `ljconv.py`'s table is elstools' translation plus `ljd_jun2017.FIX`, and the June
   decompile uses the same table. So June-mode T1 catches ljd printer and AST defects, but **not a wrong table entry**.
   - `ljconv`'s own assertions (injective, operand modes agree with June's `lj_bc_mode`) cannot catch a swap inside a same-mode
     family. Examples: the arithmetic VV/VN/NV groups, or ISLT/ISGE/ISLE/ISGT. That is the same class of mistake as the 0x2f-0x37 bug.
   - Verify every mapped opcode independently. In Ghidra (`june2017-binary-is-readable`; skill `ghidra`), locate June's VM dispatch
     table and compare each June opcode's handler with March's handler for the stock opcode it maps to.
   - Any mismatch is fixed in `ljconv` and re-checked.
   - If this proves infeasible, say so. Until it is done, June files may only be labelled "T1 vs ljconv", never "exactly June".
8. **Wire T1 into `convert`** as Rule 3's last source. Extend the `x2-lua` skill with `check` / `decompile` and what each tier means.

**Done gate:**
- every `ljconv` opcode mapping is verified against June's handlers (table in `june2017_re/lua_source_opcode_check.tsv`), or the
  gap is stated;
- the self-test catches 100%;
- every calibration miss is explained, in an archive `.tsv` (`june2017_re/lua_source_calibration.tsv`);
- the class-script verdicts are recorded in the Done note;
- the skill's `check` example runs as written.

**Done note (2026-10-09, Opus 5.5 xhigh): L2 done.** It delivers the T1 checker, the decompile route, the T2 report and the
self-test (100%), verifies `ljconv` 71/71 against June's VM, and records the class verdicts. Tools only: no flag, no build, no kom
touched.
- **Files.**
  - New: `tools/ljbc.py` (dump reader and writer, T1 compare, bytecode seeds), `tools/ljsrc.py` (tokenizer, constructors, layout
    tidy-up, source seeds), `june2017_re/opcode_check.py`, `lua_source_opcode_check.tsv`, `lua_source_calibration.tsv`.
  - Changed: `tools/luasrc.py` (`decompile` / `check` / `t2` / `selftest` / `calibrate`, T1 in `convert`, `KOM/NAME` arguments),
    `ljdiff.py` and `ljd_jun2017.py` (`--march`), `funcmap.tsv` (4 `LUA_SRC` rows), skill `x2-lua`, kickoffs `L2-review.md` and
    `L3.md`, Rule 1 here and in CLAUDE.md.
- **1. ljd.** Both vendored `slotworks.py` carry the patched SHA `2235d0a1...`, and the trees are `diff -rq` clean.
- **2. `decompile`.** March: header to `1b4c4a01`, then `ljd_fixed.py`. June: `ljconv.transcode_stock`;
  `ljconv.py --verify-stock june_lua` (14/14) proves ljd gets the same bytes as the `.std.luac` route.
- **3. T1 (`check`).**
  - The reader and writer round-trip all 4,843 March files byte-exact.
  - Instruction words are compared raw, with only the constant-index operand resolved by value: strings by bytes, numbers by
    bits, FNEW children recursively. Constant multisets, flags, params, framesize, upvalues and the child tree are compared too.
  - `--ref june` compares against `ljconv(June)`.
  - **Rule 1 amended (user, 2026-10-09):** a table constant compares as its key/value pairs (the array part by position, the
    hash part as a set). Storage order is ignored; a pair in another table still fails.
  - ljd's key order re-parses into another storage order, which had failed ~1 in 4 decompiles. `decompile` still tidies the
    order where it can (constant fields only, searched with the real parser, small budget, the result kept only if it still
    passes). Ariel: T1 raw, T0 after the tidy-up.
- **4. T2 (`t2`).**
  - R = reference bytecode, B = source compiled, S = source as text; globals dumped in raw `next` order in a mock environment.
  - Blind spots: only the main chunk runs, and engine globals are stubs.
  - `ljdiff.py --march` / `ljd_jun2017.py --march --check` accept `0x81` (Ariel 52/52 pass); without the flag they still refuse.
- **5. Self-test: PASS 293/293.**
  - Seeds: 9 source kinds (incl. local to global and swapped constants) and 11 bytecode kinds (incl. `-0.0`, NaN payload,
    upvalue bit, unrenumbered slot swaps, a pair moved to another table).
  - Controls that must pass, and do: slot renumbering, and a table's pairs stored in reverse order.
- **6. Calibration** (`june2017_re/lua_source_calibration.tsv`; every miss has a cause read from its full diff, and none is a
  checker defect):
  - **March, exact source known:** 227 files over all 11 dirs that have one; T0 138 + T1 44 = **182 (80%)**.
  - The 45 March misses are ljd defects: 39 locals printed as `slotN` globals, 4 constructor folding, 1 constructor field
    order, 1 inlined local.
  - **March, no exact source** (data001/014/119/i022): **15/15 T0**.
  - **June:** T0 4 + T1 4. The other 6 are all the catalogued deepcopy hoist (DamageEffect* / EffectSet).
- **Class scripts (C3d-4).**
  - **T1:** Elsword_SwordMan, Arme_VioletMage, Lire_ElvenRanger, RAVEN_FIGHTER, Eve_Nasod, Chung_Iron_Cannon, ELESIS_KNIGHT,
    Add_NasodRuler.
  - **Ara_Martial_Artist: not T1.** In `CHAIN_SYSTEM[1]` the original sets `SECRET_SKILL_STATE_ID` and
    `TRIGGER_STATE_ID_TO_SECRET_SKILL` before its positional sub-tables; ljd prints them after.
  - T2 for Ara: same content, order only. So Ara is a T2 candidate (Opus audit + the user's yes), or L5 work (let the tidy-up
    move keyed non-constant fields too, then prove it at T1).
- **7. Opcode table.**
  - June addresses: `lj_dispatch_init` `0x32ffb270` (renamed in Ghidra), handler pointers `PTR 0x337411d8`, `lj_vm_asm_begin`
    `0x32fee670`.
  - Each June handler was compared with every March one: **71/71 VERIFIED**, unique, same-mode families included.
  - ISNEXT writes `0x2f`, which is FIX's ITERC.
  - The 22 unmapped slots are identified, and `ljconv` refuses them, so there is no gap.
- **8. Wiring.**
  - `convert` tries the decompile last and records T0 or T1; anything else refuses with exit 4 and the first difference.
  - `--no-decompile` keeps the fast T0-only census.
  - The skill's examples ran as written.
  - Real write, proven in a scratch tree (all three roots overridden): Elsword_SwordMan converted at T1 (ledger tier T1, source
    `decompile`, sha1 matches the text). `ledger --check` and the `kom.py pack` gate PASS, and `restore` is byte-identical.
  - Real-tree dry run of the nine class scripts: 8 `[decompile, T1]`, Ara refused with exit 4.
- **Finding: loading text changes `pairs` order anyway, even at T0.**
  - Cause: the bytecode reader re-inserts keys into a pre-sized table, while the parser grows it. Ariel's T0 source iterates 9 of
    17 tables differently from the shipped file, and L0's data217 does too.
  - March audit: nothing depends on it. 11 files use `ipairs`, 2 use `pairs` only in a `TableCopy`, and the engine's two
    `lua_next` loops are order-free.
  - `L3.md` now asks for the same check on each adopted June file.
- **Hygiene.** `ledger --check` PASS (0 FAIL, 0 WARN). There is no `SERV_IRUHADEV_` code, so no flagcheck.
- **Open question:** none. Ara's route is C3d-4's call.

## Phase L2-review

Adversarial: try to make the checker pass a wrong file.
- **Normalisation.** Could ignoring constant order hide a swapped pair? Are child-proto indices resolved through the tree?
- **Numbers.** Are `KNUM` doubles compared bitwise (`-0.0`, NaN)?
- **June mode.** Does it really compare against `ljconv` output?
- **The `convert` refusals.**
- **The mock environment's blind spots.**

Fix clear defects in the tool and send design questions to the user. Write the review into this doc, update the STATUS row, and check
the L3 kickoff.

**Done note (2026-10-09, Opus 5.5 high): L2 review done.** T1 is sound for every file the vendored compiler can produce. Two tool
defects were fixed (one T1 hole reachable only by a crafted dump, one T2 blind spot). Tools only: no flag, no build, no kom touched.
- **Attacks on T1** (24 hand-made source pairs, all judged right).
  - These all fail: swapped keyed values; a pair moved to a sibling, parent, child, or same-shape sibling table; an array item moved
    to the hash part at another index; a nil value replacing a pair; a repeated key with another final value; int vs string vs
    float keys; templates swapped between two TDUPs; swapped functions, top-level and nested; upvalue order; `"1"` vs `1`; keyed
    non-constant field order.
  - These pass, correctly: equal tables (a nil-valued pair added, a repeated key with the same final value, pair order only).
  - An array item moved to the hash part **at its own index** fails. That is an equal table, so the check is stricter than needed
    (Rule 1 compares the array part by position), and safe.
  - Child prototypes are paired through each FNEW's constant, recursively, plus the total prototype count. Numbers compare by their
    dump encoding (int vs double, sign, NaN payload). Upvalue descriptors and prototype flags compare raw.
- **Fixed: repeated key in a template (`tools/ljbc.py` `loose_key`).**
  - The hash part was compared as a `frozenset`. Two dumps holding `(b,1),(b,2)` and `(b,2),(b,1)` passed T1, yet LuaJIT loads them
    as `b=2` and `b=1` (`bcread_ktab`: the last pair wins).
  - The parser never writes a repeated key, but a crafted dump can. The set comparison now applies only when the keys are unique;
    otherwise the template compares byte for byte.
- **Fixed: T2 dropped every engine call (`luasrc.py` T2 harness).**
  - Stub calls threw away their arguments, so `g_pX:AddY{...}` scripts (most March data files) reported "identical" whatever the
    tables held. Writes into engine tables were lost too, because each read of a global made a fresh stub.
  - Each stub call is now recorded in call order with its arguments dumped. Each engine global has one cached stub, and writes into
    it are dumped. Hamel_Battle_Ship_Field's T2 now compares its spawn groups (RS identical).
  - Ara has no engine calls, so its "order only" verdict stands.
  - Blind spots still printed: function bodies are not run, every call returns a stub (a branch on one takes one side only),
    functions compare by presence, metatables are not dumped, NaN payloads are not seen, an erroring chunk is compared up to the error.
- **June mode.** `reference_of(--ref june)` is `ljconv.transcode(June)`. The decompile uses `transcode_stock` with the same table, so
  only the Ghidra check covers the table.
  - Spot-check (Ghidra for the pointer table and `lj_dispatch_init`; handler bytes disassembled with capstone through
    `opcode_check.py`'s PE/map helpers, compared by hand): `lj_dispatch_init` `0x32ffb270` copies `PTR 0x337411d8` (85 + 70 entries). Its JIT-off fixups land on stock
    FORL/ITERL/LOOP/FUNCF/FUNCV, so ops `0x49+` keep stock positions.
  - The pointer table matches the TSV for `00`, `01`, `1a`, `1f` and `24`. Disassembly compared myself (normalised): June `00` = March
    ISLT (`jbe`), `01` = ISGE (`ja`), `1a` = ADDVN, `1f` = ADDNV. These are the same-mode families.
  - March VAs differ from the TSV's (the exe was rebuilt since L2); handler content is unchanged.
- **`convert`.** It refuses every non-T0/T1 path (decompile fail, no compile, T1 FAIL), with exit 4 on named files (checked on Ara).
  - The text it writes is exactly the text T1 proved. Every studio source (4,115) and every March string constant is valid UTF-8.
- **Layout tidy-up (`ljsrc.py`).** Only fields with a constant key and a constant or nil value move. Every other field keeps its
  relative order, and a trailing positional item is pinned. Each round's result is kept only if T1 passes, and `prove_decompile`
  re-checks the final text. So a move that changed anything but storage order (a crossed duplicate key, a multret tail) is discarded.
  - **Fixed: `-0` counted as movable.** `a = -0` (or `-0.0`, `-0x0`) is not folded by LuaJIT (`bcemit_unop` avoids `-0`): it emits
    `KSHORT; UNM; TSETS`, yet `_const_kind` called it a constant. It was never a soundness hole, because T1 re-proves every round.
    A negated zero now counts as an expression.
- **Self-test: PASS 517/517** (re-run after every fix).
  - 5 new source kinds (`swap-keyed-values`, `pair-to-other-table`, `array-item-to-hash`, `nil-valued-pair`, `duplicate-key`, now
    required) and 2 new bytecode kinds (a repeated hash key reordered; an array item stored in the hash part at its own index).
  - Seeds: `ANCIENT_CRAB.lua` had no T0 source any more (June's ljconv bytecode since `a5f7298`). It was replaced by
    `PET_ANCIENT_PPORU_HALLOW_DARK.lua` and `Night_Watcher_Event.lua` (March, T0, declare locals). `Wedding_OfficiantTemplet.lua` was added.
- **Calibration: 35 fresh known-T0 files** (`R1` per-dir 2 gave 14; `R2` per-dir 3 gave 21; rows appended to
  `june2017_re/lua_source_calibration.tsv`): T0 20, T1 9, FAIL 6.
  - Every FAIL is an ljd defect T1 rightly rejects: 3 slotN locals, 2 constructor field order (Hamel/Velder_Battle_Ship_Field, both
    diffs read: `SIEGE_MONSTER = {}` printed last; Ara's class), 1 dropped local (Shadow_Stinger_EVENT, registers one lower).
- **Finding for L3 (rescope).** Since `a5f7298` / `c6c8c58`, 3,639 staged `.lua` are ljconv'd June bytecode with the March `0x81`
  header. `kind()` calls them March, so `convert` would prove one against itself and write a ledger row saying origin `march`.
  `L3.md` now asks `convert` to refuse names in the two per-file TSVs and point to `adopt-june --source`.
- **Hygiene.** `ledger --check` PASS (0 FAIL, 0 WARN). There is no `SERV_IRUHADEV_` code, so no flagcheck or build.
- **Open questions:** none.

## Phase L3 - Core list + `adopt-june` + skill `x2-adopt-june`

**Scope (rescoped 2026-10-09, user).** Implement Rule 2 behind one command and one skill. The default route is June's `ljconv`
bytecode; source is only for the core list.

**Run with:** Sonnet 5.5 high. **Read list:** Context, Rules, Starting context, Phase L2's Done note and review, MODS.md sections
"June NPC and dungeon Lua via ljconv" and "June partial ports replaced".

1. **Core list `tools/data/lua_core_list.tsv`.** Columns: kom, name, origin (march / june / merged), why (`kept-merged: <blocker>` or
   `edit: <plan phase> <what>`), state (`source` / `bytecode` / `pending` / `blocked: <reason>`), date. Seed it from:
   - the ten kept-merged files and their blockers (MODS "June partial ports replaced");
   - every `X.src.lua` sidecar still in kom_contents (65 on 2026-10-09: data034 12, data035 1, data036 11, data037 1, data039 1,
     data040 38, datai014 1). Classify each one: a live intended edit (core), or a leftover that June's bytecode has superseded
     (report it; do not delete a sidecar without the user);
   - edits other plans already name: Atlas A5 (`DLG_Destination_byButton` + warp C++), the three `DLG_Map_*` dialogs (Atlas/Elysion
     ids), class scripts a character port must edit (checklist §6/§7l).

   Add `luasrc.py core --check`: every BOM-source `.lua` in kom_contents is on the list, and every `source` row is BOM source.
2. **`luasrc.py adopt-june <june kom> <file> ... | --list F [--dry-run] [--source]`.**
   - **Default:** extract with `tools/junekom.py`, `ljconv` to `0x81`, stage through `kom.py stage-june`'s placement, duplicate and
     long-name rules (reuse them; do not reimplement), write the per-run TSV. **It refuses a name on the core list** (staging June's
     bytecode would drop the merge; that is how `DLG_Destination_byButton` crashed Cobo) unless `--source` is given.
   - **`--source`** (core-list names only; refuses others): extract, `ljconv`, decompile, T1 against the `ljconv` output; on a pass,
     BOM source + a ledger row. On a fail, **nothing is staged**; the reason goes into the core-list row (`blocked: ...`) and the file
     is an L5 / T2 candidate.
   - Packing stays a separate step.
3. **Calibration in a scratch target.** No writes to live dirs:
   - default mode on 20 files from `june2017_re/june_npc_lua_port_20261009.tsv`: output byte-identical to what `a5f7298` staged;
   - `--source` on the 14 `june_lua/` files, `ELYSION_SOLESS.lua`, `NewSkillTempletVer2` (expect a fail), and one June class script
     (show the `*_DAMAGE_BOUNDING_*` tables arrive);
   - the refusal on a core-list name without `--source`.
4. **Iteration-order audit for `--source` files (added by L2).** A file loaded as text builds its constant tables with the parser, so
   `pairs`/`next` can walk them in another order than the bytecode build would. Flag every adopted source file that has an
   ITERC/ITERN/ISNEXT loop or a `next`/`pairs`/`foreach`/`foreachi` lookup (`tools/ljbc.py` reads both), naming the table each walks.
5. **Skill `.claude/skills/x2-adopt-june/SKILL.md`:** triggers ("port / adopt / bring in June's X.lua", "/x2-adopt-june"); copy-ready
   commands for both modes; when a file belongs on the core list; how to read the outcome; the steps it does not take itself
   (`/elstools pack`, ask, `/x2-kom-packed copy`).
6. **CLAUDE.md:** the Rule 2 wording plus a *Workflow automation* table row.

**Done gate:**
- the core list is written, `core --check` passes, and the user has confirmed the list (one AskUserQuestion with the rows that are
  not kept-merged);
- the default mode reproduces `a5f7298`'s bytes on the sample;
- `--source` calibration outcomes recorded, and the core-list files that fail T1 are named (they decide whether L5 runs);
- the skill triggers, and its examples run as written on a scratch target.

**Done note (2026-10-09, Sonnet 5.5 high): L3 done.** Tools only: no flag, no build, no live kom touched, no kom copy. Calibration ran
in a scratch target (`.build/l3/scratch`, with `X2_KOM_CONTENTS` / `X2_LUA_LEDGER` / `X2_LUA_CORE_LIST` overridden).
- **New / changed.**
  - New: `tools/adoptjune.py`, `tools/data/lua_core_list.tsv`, skill `.claude/skills/x2-adopt-june/SKILL.md`.
  - Changed: `tools/luasrc.py` (subcommands `core`, `adopt-june`, `iter-audit`; `convert` refuses ljconv'd June files and adds a core
    row for what it converts; `restore` sets the row to `bytecode`), `tools/kom.py` (`cmd_stage_june` got `transform`, `only_koms`,
    `move_sidecars`, `exit_on_refuse`, `transform_jobs` and now returns its result; the CLI is unchanged), `CLAUDE.md` (Rule 2 + a
    *Workflow automation* row), `tools/README.md`.
- **Core list (user confirmed 2026-10-09: "Only the 10").**
  - The ten kept-merged files, each with its blocker: `Enum`, `DungeonEnum`, `DamageEffect`, `EffectSet` (data036); `DLG_Map_Village`,
    `DLG_Map_Local`, `DLG_Map_House` (data034); `NPCTemplet` (data037); `DungeonData` (data035); `DLG_Destination_byButton` (datai014).
  - `core --classify` compared all 65 `X.src.lua` sidecars with `ljconv(June's file)`:
    - **54 recompile T0 to it**, so the shipped file already equals June's bytecode and the sidecar is a leftover June superseded: 9
      `DLG_Map_Local_*_Back_NEW`, 6 data036 tables (`AnimationNameTable`, `BattleFieldBuffFactor_INT`, `PVPData`, `PetPreView`,
      `SkillLevelUpSocketGroup`, `StringID_def`), `UI_MUL_MAI`, the 38 data040 Ranox stage scripts.
    - 10 differ from June: exactly the kept-merged ten.
    - 1 has no June file: data036 `FieldBonusDrop` (server-only Lua the user packs; stays bytecode, not core).
    - **No sidecar was deleted or moved.** No class-script row: C3d-4 chose "character port adopts June's whole script".
  - `core --check` PASS (`ledger --check` PASS). It fails on BOM text that is not on the list, and on a `source` row that is not BOM text.
- **`adopt-june`, default mode.**
  - Sample: 20 names spread over `june_npc_lua_port_20261009.tsv` (data035, data037, datai016).
  - Result: **COMPARE OK 20 of 20 byte-identical to `a5f7298`'s staged sha1s, same kom.**
  - The sidecar move was proven in a second scratch: a staged `X.src.lua` goes to the `kom_backup_stage_june` backup, not left to be
    folded back.
  - Refusals proven: a core-list name, a name outside the given June kom, and `--source` on a non-core name.
- **`adopt-june --source` calibration** (scratch core rows for the non-core names; the 3 kept-merged ones used `--lifted`, a guard I added:
  `--source` on a kept-merged row would swap the merge for June's whole, which is what crashed Cobo):

| file | result |
|---|---|
| T0: `DamageSound`, `EffectSet_Description`, `Elsword_DamageData`, `UnitViewerEffectSetTable` | staged, ledger T0 |
| T1: `EffectSet_INT`, `UIEffectSet`, `UIEffectSet_INT`, `Enum` (core) | staged, ledger T1 |
| `DamageEffect`, `_Character`, `_ETC`, `_INT`, `EffectSet` | FAIL, the catalogued deepcopy hoist (matches L2's 6 misses) |
| `DamageEffect_Dungeon`, `NewSkillTempletVer2` | decompile does not compile (`slot3` global; a `}` expected). The expected `NewSkillTempletVer2` fail |
| `ELYSION_SOLESS` | ljd timed out after 900 s (nothing staged) |
| class script `Elsword_SwordMan` | T1; June's has `ESSI_DAMAGE_BOUNDING_DOWN / _LANDCRASH / _UP`, March's has none (bounding tables arrive) |

  - Every failure stages nothing and writes `blocked: <reason>` on the scratch row. The 12 staged files passed `ledger --check`, and
    `luasrc.py check` against the snapshot (the ljconv bytes) gives T1 again for `Enum` and `DLG_Destination_byButton`.
  - The real core-list files, measured the same way (all ten):
    - **T1 ok (4):** `Enum`, `DungeonEnum`, `DungeonData`, `DLG_Destination_byButton`.
    - **T1 FAIL (6, `blocked:` in the real list):** `DamageEffect`, `EffectSet` (the hoist), and four with a new class,
      `DLG_Map_Village` (3,979 vs 4,013 instructions), `DLG_Map_Local` (51 vs 63), `DLG_Map_House` (10,075 vs 10,167), `NPCTemplet`
      (45,612 vs 45,611). **These six decide L5.**
    - These measure `--source` (adopting June's file whole). The kept-merged files stay March's merged bytecode today, and their
      sidecars are T0, so `convert` can still turn them into source for edits.
- **Iteration-order audit** (`iter-audit`, also run by `--source`): each site is reported as the call or loop (`ISNEXT` / `ITERN` /
  `ITERC`, or a `pairs` / `next` / `foreach` / `foreachi` / `ipairs` call), its table expression and its source line, and marked
  order-sensitive or index order. All 12 staged calibration files have **0 sites** (they are data tables). A real file (June
  `Crazy_Puppet`) reports its one `ipairs` site. Note: `Enum` passes T1 with 72 table constants stored in another order, which matters only
  to a `pairs` walk, and the engine's two `lua_next` loops are order-free (L2).
- **`convert` refusal.** It refuses ljconv'd June files by sha1 against the per-run TSVs (`june_npc_lua_port_*`, `june_partial_port_replace_*`,
  `june_adopt_*`), exit 4. I matched on kom + name + **sha1**, not kom + name alone: `DLG_Destination_byButton` is in the partial TSV but was
  reverted to March's, and must stay convertible. `selftest` PASS 517/517 (re-run after the last edit).
- **Open for the user:** whether L5 runs for the six failing core files (STATUS L5 row; Opus 5.5 xhigh). Packing and copying: nothing owed.

## Phase L4 - dropped (rescope, 2026-10-09)

Was: audit every shipped June-named `.lua` against `ljconv(June)`. The 120 partial ports were replaced by June's `ljconv` bytecode on
2026-10-09 (`c6c8c58`, per-file kinds in `june2017_re/june_partial_port_replace_20261009.tsv`), and the NPC / dungeon scripts shipped
the same way (`a5f7298`), so there is nothing left to audit outside the core list, and L3 covers that.

## Phase L5 - Decompiler fixes (core-list files only; only if L3 names failures)

**Scope.** Make the core-list files that fail T1 in L3 convertible, so they can be edited as source. No fix is made for a file off the
core list; those ship as June's bytecode and need none. `NewSkillTempletVer2` (data036) is the expected first case.

**Run with:** Opus 5.5 **xhigh**. **Read list:** Established facts, Phase L3's Done note and `tools/data/lua_core_list.tsv`, and
`RANOX_LUA_DECOMPILE_BUG.md` "Residual ljd defects".

Fix as tracked hooks, like `june2017_re/ljd_fixed.py`, never by silently editing the vendored tree:
- parens around an IIFE;
- a long-bracket level that does not collide with the string's content (**done 2026-10-17** in skill-system-core S1:
  `june2017_re/ljd_writer_fix.py`, installed by `ljd_fixed.install_fix()`; it also prints a value with `\r` quoted);
- deepcopy-hoist;
- TIMEOUT / MemoryError on big tables.

Fix only the defects the blocked core-list files actually hit. Re-run `adopt-june --source` on them. Where a fix is out of reach, the
alternative is a T2 report for that one file (Rule 1: Opus audit + the user's yes).

**Done gate:** every `blocked` core-list row is either `source` now or carries a specific reason and a T2 / wait decision.

## Phase L6 - Engine: errors visible, parser kept

**Scope.** Make source failures visible, and make sure text loading can never be compiled out. Flag `SERV_IRUHADEV_LUA_SOURCE`
(+ `_DEBUG`) in `KTDXLIB/Always.h`.

**Run with:** Opus 5.5 high. **Read list:** Rules, Established facts.

Changes, kept out of `LuaCommon/`:
1. **Parser guard.**
   - Under the flag, `KTDX.h:398-400` must not define `X2OPTIMIZE_REMOVE_LUA_INTERPRETER_MODULE`.
   - Add an `#error` tripwire if both ever end up defined.
   - `Always.h` is included at `KTDX.h:190`, before that block. `KTDX.h` is CP949, so edit it with `bytepatch.py`.
2. **Load diagnostics** in `CKTDXApp::LoadAndDoMemory_ErrorCode` (and its `LoadLuaTinker` sibling):
   - classify the decoded buffer: KOG bytecode / other bytecode / BOM text / BOM-less text / garbage;
   - on failure, log `[LUA_SRC] <file>: <kind>, <reason>` beside `KEM_ERROR73`.
3. **Lua error text.**
   - Register a C `_ALERT` from KTDXLIB code in every binder state: the main one plus the per-thread binders (`KTDXApp.cpp:499-511`).
     First verify that `print_error` looks `_ALERT` up as a global (`lua_tinker.cpp:262`).
   - It forwards syntax and runtime errors to ErrorLog with the `[LUA_SRC]` prefix, deduplicated and throttled (per-frame errors
     exist).
4. **`_DEBUG`:** per-load kind and milliseconds over a threshold, as a load-time baseline.

Acceptance is unchanged: bytecode still loads.

If a `LuaCommon/` edit proves unavoidable:
- first add `!/LuaCommon/` to `.gitignore` with a baseline commit of the untouched files;
- also define the flag in luajitLib's `Release` `PreprocessorDefinitions`;
- add it to `SECOND_HOMES` in `tools/flagcheck.py`.

**Done gate:**
- `flagcheck --worktree` PASS;
- `DEPLOY OK`;
- dev console play-test (auth_level 3, raw Lua): `error("LUA_SRC test")` and a deliberate parse error both reach `ErrorLog.txt`, with
  text and line;
- a normal session shows no new noise.

**Done note (2026-10-09, Opus 5.5): L6 built and deployed, play-test pending.** Flags `SERV_IRUHADEV_LUA_SOURCE` +
`SERV_IRUHADEV_LUA_SOURCE_DEBUG` (`KTDXLIB/Always.h`). `LuaCommon/` untouched, so there is no second home.
- **Parser guard.** `KTDX.h:398-405` keeps the `#define X2OPTIMIZE_REMOVE_LUA_INTERPRETER_MODULE` block in the flag's `#else`.
  `#error` tripwires sit at `KTDX.h:419-423` and at the lib pragma in `KTDXApp.cpp:28`; the latter also catches a later or
  command-line define.
- **Load diagnostics.** `KLuaSrcLoadScope` (declared in `KTDXApp.h`, defined in `KTDXApp.cpp`) wraps
  `LoadAndDoMemory_ErrorCode` and `LoadAndDoMemory_LocalFile_ErrorCode`. `LoadLuaTinker` already routes through the first.
  - It classifies the bytes handed to `DoMemoryNotEncrypt` at that moment, because the uncompress buffer is gone by the time
    `KEM_ERROR73` is written.
  - On failure it writes `[LUA_SRC] <file>: <kind>, <reason>`. Kinds: KOG bytecode, other bytecode (header printed; 82 = June),
    BOM text, text without BOM, not Lua (first bytes printed), uncompress failed.
  - Not covered: the two X2Lib loaders that call `DoMemoryNotEncrypt` directly (`X2ItemManager.cpp`, `X2World.cpp`). Their Lua
    errors still reach ErrorLog through `_ALERT`, with "at run time" in place of the file name.
- **`_ALERT`.** It is registered at the top of `CKTDXApp::RegisterLuabind`, which runs for the main binder (`KTDXApp.cpp:461`)
  and every per-thread binder (`:513`). `KLuaManager` threads find it through their env's `__index`, because `print_error`
  uses `lua_gettable` (`lua_tinker.cpp:262`).
  - Lines: `[LUA_SRC] lua error while loading <file>: <msg>`, or `... at run time: <msg>`, followed by on_error's call stack.
  - Heads are deduplicated by text and written on the 1st, 10th, 100th... sighting. A repeat has no stack. A cap of 512
    distinct heads, then one "no longer written" line. All writes take one lock.
  - The function never raises, since `print_error` calls it unprotected from inside the pcall handler.
- **`_DEBUG`.** `[LUA_SRC_DEBUG]` lines (a separate tag, so `--tag LUA_SRC` stays clean). One line per BOM-text load and per
  load over 20 ms, with kind, bytes, total ms and run (parse + execute) ms. Running totals per kind every 256 loads.
  Mount-time loads never reach ErrorLog (memory `errorlog-loses-mount-time-lines`), so the baseline covers use-time loads;
  the totals line still counts every load.
- **Expected in a normal session:** Lua errors that were printf-only before now appear once each, for example
  `lua_tinker::call() attempt to call global ...`. They are existing defects made visible, not noise; the gate fails only on
  repeated lines.
- Hygiene (`--worktree`): `files: 1 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; 0 deleted lines;
  one tokenless `#endif` (the `#if defined(...)` DEBUG-needs-main tripwire in `Always.h`, same shape as its neighbours);
  `RESULT: PASS`.
- Deploy: `DEPLOY OK: X2_offline.exe is 19,449,856 B, dated 2026-10-09 09:21:17, md5 8da01b9451b8d7d2952c58eadbac3ebf`
  (built at 3d48592 + the four L6 files). The exe carries `[LUA_SRC]`, `[LUA_SRC_DEBUG]` and `_ALERT`.
- **How to see it in game.** Dev console (END key; `settings.auth_level` is 3), any character, in a village or dungeon:
  1. `error("LUA_SRC test")` -> `[LUA_SRC] lua error at run time: [string "lua_tinker::dobuffer()"]:1: LUA_SRC test`, plus stack lines.
  2. `x = = 1` -> `[LUA_SRC] lua error at run time: [string "lua_tinker::dobuffer()"]:1: unexpected symbol near '='`.
  3. `assert(loadstring("local a = 1\nlocal b = = 2"))` -> the same kind of line with `:2:`, so line numbers come from the source.
  4. Paste 1 nine more times -> nothing new until the 10th, which writes `(seen 10 times) ...` with no stack.

**Play-test (2026-10-09 09:43-09:45, deployed exe md5 8da01b94): PASSED.** The session covered login, a village and a battle-field join
(no dungeon), plus the four console checks.
- All four console checks were written exactly as predicted (ErrorLog lines 463-474). Check 3 shows
  `[string "local a = 1..."]:2: unexpected symbol near '='`, so source line numbers reach the log.
- No noise: the session wrote no `[LUA_SRC]` head besides the tests and the one below.
- Loads are unchanged: 47 `KEM_ERROR72` and 1 `KEM_ERROR73`, the same file list as the pre-L6 run.
- `[LUA_SRC_DEBUG]` works. Particle `.txt` loads show as "text without BOM" and load fine, because their reader takes BOM-less text.
- **Finding for the review: the reason text is misleading for one kind of failure.** The one `KEM_ERROR73` is `TutorSystem.lua`,
  logged as "BOM text, parse or run error; the lua error line just above...", but no lua line precedes it.
  - The file is 3 B, a BOM and nothing else. It is the per-user tutor save written by `CX2TutorSystem::SaveScriptFile`.
  - Its reader is `KLuaManagerProxy::DoMemoryNotEncrypt` (`KLuaManagerProxy.cpp:532-542`), a hand parser, not Lua. After the BOM
    it returns false on an empty buffer, so `_ALERT` never runs.
  - The failure is pre-existing and harmless. The fix belongs in the reason text: class an empty buffer (BOM only) on its own,
    and say "no lua line above = a non-Lua reader refused it" in the BOM-text reason.

## Phase L6-review

- **Threads.** Is `_ALERT` safe when loader threads call it? Does ErrorLog writing lock?
- **Throttle.** Can a per-frame error flood the log?
- **Flag-off.** Does it equal base?
- **Loads.** The set of loaded files is unchanged.

Hygiene: `flagcheck --range <first L6 commit>^..HEAD --with SERV_IRUHADEV_LUA_SOURCE_DEBUG`.

**Done note (2026-10-09, Opus 5.5 high): two defects fixed, the play-test finding fixed, one older hazard handed to the user.
Built and deployed; play-test pending.** The fix commit is `7f6f3fb`, under the same flag. It changes only L6's own lines
in `KTDXApp.cpp`, plus one enum line in `KTDXApp.h` and the comment in `Always.h`.
- **Threads.**
  - `_ALERT` itself is safe. The file being loaded and the continuation state are thread-local, the head map sits under the
    lock, and the function never raises.
  - `KLuaManager` threads do reach it, confirmed by reading the code: `lua_newthread` + `setfenv` with `__index`
    (`KLuaManager.cpp:108-115`), and `print_error` uses `lua_gettable(L, LUA_GLOBALSINDEX)`, which resolves to the thread's env.
  - **Defect, fixed: ErrorLog writing did not lock.** `KLOG` is `g_kSuperAssert` (`KSuperAssert.h:109,231`), a single global
    whose `std::string` buffers have no lock. L6's lock covered only L6's own lines, while `ErrorLogMsg` on any other thread
    edits the same strings. That is a heap-corruption risk, and L6 added loader-thread writers (71 `[LUA_SRC_DEBUG]` lines in
    the play-test). `LuaSrcWrite` now appends to the file itself, so no L6 line touches `g_kSuperAssert`. It mirrors
    `KSuperAssert::_fileout` (`fstream` app in text mode, the `CHAT_LOG_TO_TXT` timestamp, opened per line because
    `X2Main.cpp:429` deletes the file at start).
  - **Older hazard, not fixed (your call):** the studio's own `ErrorLogMsg` from loader threads (`KEM_ERROR72/73/136`) races
    in the same way. The fix would be a lock inside `KSuperAssert`, a studio file that every `KLOG` user goes through.
- **Throttle: bounded.**
  - The same text is written the 1st, 10th, 100th... time.
  - Varying text stops at 512 heads plus their stacks, then one cap line.
  - **Defect, fixed:** load-failure heads shared that cap, so a per-frame error with varying text could silence every later
    `<file>: <kind>` line. They are now exempt (`LuaSrcHead(.., true)`); their number is bounded by the file count.
- **Flag off = base.** flagcheck below. `KTDX.h`'s `#else` is the base block verbatim.
- **Loads: unchanged.**
  - The scope only reads (a head of at most 4 B) and sets a thread-local pointer.
  - `_ALERT` changes where `print_error`'s text goes, nothing else.
  - No client script reads `_ALERT`. Greping `kom_contents` finds only the `NUI_MI_*_ALERT_*` enum names.
  - L6 play-test: 47 `KEM_ERROR72` and 1 `KEM_ERROR73` (TutorSystem.lua), the same list as the pre-L6 run according to the L6
    note. That sorted list is kept in this session's scratchpad (`kem7273_L6_baseline.txt`) to diff against the review play-test.
- **Parser guard: trips as intended.** A scratch `cl -Zs` probe of `KTDX.h` with KTDXLIB's US_SERVICE defines gave:
  - as built: `REMOVE_INTERPRETER` off;
  - with `_SERVICE_`: off;
  - with `_IN_HOUSE_SERVICE_READY_QA_`: off;
  - with a command-line define: `KTDX.h(421) : fatal error C1189`.
  - luajitLib `Release` never defines it (only `Release_NoInterpreter` and `IN_*`, `luajitLib.vcxproj:353-458`), so the
    client headers and the linked lib agree.
- **Play-test finding, fixed.**
  - New kind `LSK_EMPTY` ("empty"): 0 B (which was "not Lua" before), or a BOM with nothing after it. The Lua binder runs a
    bare BOM as an empty chunk, so only a non-Lua reader refuses one.
  - The BOM-text and KOG-bytecode reasons now say that no lua error line above means a reader that is not Lua refused the
    file. The text-without-BOM reason now covers the `.txt` compilers.
- Hygiene (`--range dc32d70^..HEAD --with SERV_IRUHADEV_LUA_SOURCE_DEBUG`):
  - `files: 3 PASS, 2 PASS~ (blank/marker only), 2 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 21 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 2 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
  - The deleted lines are all L6's own replaced lines inside the flag: comments, the `LuaSrcWrite` body, the `LuaSrcHead`
    signature and cap, the "seen" `KLOG`, two call sites, four reason strings, the kind array, the totals, and two buffer
    sizes.
  - The tokenless `#endif` (counted once per revision pair) is L6's DEBUG-needs-main tripwire in `Always.h`, the same shape as the JUNE_BOUNDING_STATE one
    above it.
  - The PASS~ lines are one blank line after each L6 block in `KTDXApp.cpp` and `KTDXApp.h`.
- Deploy: `DEPLOY OK: X2_offline.exe is 19,450,880 B, dated 2026-10-09 10:11:51, md5 567c51df156d3f74c26a72d18408a890` (built at 509e3a1 + the three fix files). The exe carries the new strings ("no script after the BOM", "empty %u", the timestamp format).
- **L7 baseline (from the L6 play-test, `_DEBUG`):** totals after 2,048 loads were KOG bytecode 2,006 loads / 2,483 ms run,
  BOM text 6 / 1 ms, text without BOM 35, not Lua 1. Five of the BOM-text loads (63-1,263 B) took 0.1-0.4 ms each. The sixth,
  the 3 B TutorSystem.lua (now counted as "empty"), took 1.1 ms. There is no large source file yet, so this does not measure parse cost.

## Phase L7 - Close-out

**Run with:** Sonnet 5.5 high. **Read list:** Rules, every phase's Done note.

1. **Convention sweep.** Edit kickoffs only, never frozen inline kickoffs in old plan docs. Targets: any kickoff that says
   "compiled `1B 4B 4C 81`" or decompiles/compiles by hand.
   - **atlas:** the Starting context's "Lua compile/stage" block and the "Lanox lessons" gate.
   - **elysion E0.**
   - **lanox:** the "compile every `.lua`" rule.
   - **june-effect** kickoffs that use `kom.py compile`.
   - **`JUNE2017_CHARACTER_PORT_CHECKLIST.md` §6:** copy the June script becomes `adopt-june` (default), or `adopt-june --source`
     after adding a core-list row when the port must edit it.
   - **STATUS:** a "superseded by `LUA_SOURCE_PHASES.md`" line for the plans whose docs keep the old wording.
2. **Memory.** Update `march-class-scripts-bytecode-only-no-recompile`, `kom-pack-copy-skills`, `march-kom-contents-extract` and
   `elstools-decompile-drops-assignments-becomes-globals`. The `lua-source-plan` note names `x2-lua` / `x2-adopt-june` as the entry
   points.
3. **Skills.** `x2-lua`, `x2-adopt-june`, `elstools` and `x2-phase` must agree with the final tool behaviour and with
   `tools/README.md`.
4. **Wrap-up.** Switch `_DEBUG` off; the error logging stays. `ledger --check` and `core --check` are clean.

**Done gate:** the sweep is listed in the Done note, the ledger is clean, and the skills are consistent.

**Done note (2026-10-09, Sonnet 5.5 high): plan closed. `_DEBUG` off, built and deployed. No play-test owed for L7 itself.**
- **Sweep (kickoff files only; frozen plan docs untouched):**
  - `kickoffs/june-asset-port/P3.md`: the RidingPet_GodBeast_Fire step now says `luasrc.py adopt-june data037 RidingPet_GodBeast_Fire.lua`
    (was: copy from `june_src_fixed/` + `kom.py compile`).
  - Every other kickoff that names `compile` / `decompile-batch` / `compile-batch` / `.src.lua` belongs to a phase STATUS shows done
    (june-buff 13 review, 15-step2b, 16-data; june-effect C3d-2, C9c-2b, C9h-2d, C10b-1 review), so it is history and stays as written.
    The remaining kickoff greps (`june-backlog`, `offline-grade`, `dev-overlay`, `june-item-kim`) only read `june_src_fixed/` or describe
    the pack gate; nothing to change. Atlas A0 is the only atlas kickoff and has no Lua step; Elysion and Lanox have no kickoff files yet.
  - `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 6 step 6: `adopt-june` (default), or a `core --add` row + `adopt-june --source` when the
    port must edit the script; never copy from `june_src_fixed/` or compile by hand.
  - STATUS: "Superseded by `LUA_SOURCE_PHASES.md` (L7)" lines in the `atlas` and `elysion` blocks and the `lanox` row of "Other plans"
    (their plan docs keep the `compile-batch` / "compiled `1B 4B 4C 81`" wording, frozen; Atlas A5 owns the `DLG_Destination_byButton` row).
- **Memory:** `march-class-scripts-bytecode-only-no-recompile`, `kom-pack-copy-skills`, `march-kom-contents-extract`,
  `elstools-decompile-drops-assignments-becomes-globals` updated; `lua-source-plan` names `/x2-adopt-june` and `/x2-lua` as the entry
  points and records L7.
- **Skills vs the tools:** compared with `luasrc.py --help`, `kom.py --help` and a real `convert --dry-run` on an ljconv'd June file.
  Fixed: `x2-lua` (new refusal row for ljconv'd June files; a converted file is on the core list), `x2-phase` (tool table gets the
  `adopt-june` row), `elstools` (the `compile-dir` warning no longer cites "deepcopy files via ljconv"). `x2-adopt-june` and
  `tools/README.md` already matched.
- **`_DEBUG` off:** `//#define SERV_IRUHADEV_LUA_SOURCE_DEBUG` in `KTDXLIB/Always.h` (bytepatch, CRLF kept). `flagcheck LUA_SOURCE --with
  LUA_SOURCE_DEBUG --worktree --files KTDXLIB/Always.h`: `files: 0 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `in this flag's files: 2
  deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 1 #else/#endif without the flag token, 0 encoding
  change(s)`; `RESULT: PASS` (the 2 deleted lines are the comment and `#define` of the DEBUG block; the tokenless `#endif` is L6's tripwire).
  The `[LUA_SRC]` error logging stays on. The deployed exe holds 0 copies of `LUA_SRC_DEBUG` and 6 of `LUA_SRC]`.
- Post-commit `flagcheck LUA_SOURCE --with LUA_SOURCE_DEBUG --range 04d4964^..04d4964`: `files: 0 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; same `in this flag's files` counts as above; `RESULT: PASS`.
- Deploy: `DEPLOY OK: X2_offline.exe is 19,454,976 B, dated 2026-10-09 17:11:47, md5 10df7f3e94b22aa5d31af22a1b191ee7, equal to X2/US_SERVICE/x2.exe (2026-10-09 17:11:42); built at mods/june-buff-port 7f00a64 + 1 uncommitted code file(s).`
- **Parse-cost baseline:** no new baseline. L3 put no BOM-source file into a kom that reached the game (`kom_contents` holds 0 BOM-text
  `.lua`; ledger rows are the 2 `EXC` bytecode exceptions), so the L6 numbers stand: 2,048 loads, KOG bytecode 2,006 loads / 2,483 ms,
  BOM text 6 loads / 1 ms (63-1,263 B, 0.1-0.4 ms each). Re-enable `_DEBUG` when the first large core-list source file is packed.
- **Checks:** `luasrc.py ledger --check` PASS (0 FAIL, 0 WARN); `luasrc.py core --check` PASS (0 FAIL, 0 WARN; a peer session
  added `data036 BattleFieldData.lua` to the list, not part of this commit).
- **Open for the user:** (1) the L6-review play-test (4 console checks, KEM_ERROR72/73 list against `kem7273_L6_baseline.txt`) was never
  reported; L7 switched the debug lines off, which does not affect those checks. (2) L5 (6 core files fail T1 under `--source`) is still
  your call.

## Effects on other plans

- **june-effect C3d-4:** closed on route 2 (`fdcf60b`, user): the character ports bring June's class scripts.
- **Character ports:** June's class scripts arrive through `adopt-june`, with the bounding tables inline; the default `ljconv` route
  when the script loads unchanged, `--source` after a core-list row when the port must edit it. The C++ registration call is
  checklist §7l.
- **atlas / elysion / lanox:** their "compiled `1B 4B 4C 81`" gates are superseded once L1/L3 land (kickoffs updated in L7). Elysion
  E0's decompile-check of `ELYSION_SOLESS` becomes `adopt-june` (`--source` only if Elysion must edit it). Atlas A5 owns the
  core-list row for `DLG_Destination_byButton`; Atlas/Elysion own the three `DLG_Map_*` rows.
- **Any plan that must edit a June `.lua`:** add the core-list row (Rule 2) and run `adopt-june --source`; no question (user, 2026-10-09).
- **june-asset-port:** unaffected (non-lua). P5's March constant scan reads text for any converted file.

## Play-test hand-off

Only L0 and L6 need the user in game. Give copy-ready steps:
- **L0:** the world map, then the Ranox local map and the Special local maps.
- **L6:** dev console commands with the exact text to paste.

End with `python tools/logs.py --tag LUA_SRC` and the `KEM_ERROR72/73` check in `ErrorLog.txt`. Never launch the game.
