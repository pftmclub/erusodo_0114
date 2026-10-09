# Dev overlay: loose Lua for development, pack at plan close-out and release

Plan key `dev-overlay` · flag `SERV_IRUHADEV_DEV_OVERLAY` · log tag `[DEV_OVERLAY]` (file `dev_overlay.log`) · kickoffs
`kickoffs/dev-overlay/` · tool `tools/overlay.py` + skill `x2-overlay` (both built here) · created 2026-10-09 (user request) ·
**scope narrowed to `.lua` only the same day (user): non-Lua assets keep the usual kom flow.**

## Context

**The problem.** Every `.lua` change used to go through this loop:
1. convert or decompile the file;
2. edit `kom_contents\dataNNN_kom\`;
3. `kom.py pack`;
4. ask the user;
5. `kom.py copy`;
6. play-test.

Ports kept stalling on it.

**What 2026-10-09 proved:** the engine reads loose files ahead of the koms.
- **Loose first.** `LoadDataFile` calls `LoadRealFile` before the kom map (`KTDXLIB/KGCMassFileManager.cpp:950-962`;
  `MASS_FILE_FIRST` is off).
- **Same Lua call.** A loose file is flagged `MFI_LOCALFILE`, so `LoadAndDoMemory_ErrorCode` skips XOR and zlib
  (`KTDXLIB/KTDXApp.h:585-589`). It hands the raw bytes to the same `DoMemoryNotEncrypt` that the kom path ends in (`:621-658`).
- **Play-test.** A loose BOM+UTF-8 copy of `DLG_Map_Local_Peita_Icon_NEW.lua` in `data\lua_dev\` moved the Bonghun Temple button
  100 px, with 0 KEM_ERROR. The test file was deleted afterwards.

**Goal.**
- During development, `.lua` files live in one **dev overlay** folder that the game reads first.
- A per-kom ledger records what changed and where each file ships.
- Packing ("promote") happens only at a plan's close-out and at release.
- Existing tools are reused: `kom.py`, `luasrc.py`, the Lua pack gate, `elstools`.

**User decisions (2026-10-09):**
- **Lua only.** Textures, meshes, sounds, `item.kim`, `.txt`, June assets and so on stay on `kom_contents` -> `kom.py pack` ->
  ask -> `kom.py copy`, unchanged. The overlay holds `.lua` and nothing else; the engine and the tool both enforce that.
- **Promote cadence.** Each plan's close-out phase promotes its own Lua. A final `promote --all` at release sweeps the rest.
- **Server-only Lua** may sit in the overlay as plaintext, because the offline loaders read it. At promote the tool stops and
  lists those files, and the user encrypts and packs them, as now.
- **Copy asks.** A multi-kom promotion asks **once for the listed set** (kom, entries, md5). Every copy still needs that OK.
- **Layout.** One subfolder per kom (`dev_overlay\data036\...`), not one flat folder.

## Rules

1. **Lua edits go through the overlay.** After O5, a `.lua` change is made in `dev_overlay\<kom>\` through `overlay.py`, never by
   editing `kom_contents` or writing into `data\`. Promote is the only writer of `.lua` into `kom_contents`.
   - A non-Lua change to the same kom still packs as usual. The packed kom then carries the old Lua and the overlay still wins at
     runtime until promote; that is expected.
2. **The ledger is the record.** A file without an entry, or an entry without a file, fails `check`. Promotion reads only the
   ledger and the files.
3. **Standing rules still apply:**
   - no new `.lua` (CLAUDE.md), except server Lua the client needs (`--server-lua`);
   - June `.lua` only through `adopt-june` (lua-source rule 2), which writes into the overlay through O4's API;
   - ask before every copy, with the set decision above;
   - CP949 sources are edited with `bytepatch.py` only.
4. **Every play-test hand-off runs `overlay.py check`** and quotes its OK line. This applies in this plan, and in every plan from
   O5 on.
5. **Nothing leaves the overlay before `COPY OK`.** `retire` moves files out only after the live kom's md5 equals the promoted
   pack. The next play-test then runs on the koms, so nothing is masked.
6. **Byte equality is the promotion proof.** Promote decodes the packed kom back and requires every promoted entry to be
   byte-equal to its overlay file. A play-test with the overlay `off` then confirms it in game.

## Starting context

Every repeated step is a tool (`tools/README.md`). Do not hand-type msbuild, elstools, kom packing or log greps.

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py DEV_OVERLAY_PHASES.md "<heading>"` |
| Kom pack / copy / status | `python tools/kom.py pack / copy / status dataNNN` (`/elstools pack`, `/x2-kom-packed copy`) |
| Lua sources, gate, ledger | `python tools/luasrc.py find / convert / ledger` (`/x2-lua`); `luasrc.gate()` is the pack gate |
| Inspect / extract a March kom | `elstools/elstools.exe inspect -q -i <kom>` / `extract` (absolute paths, `-q`, stdin closed, `-v mar-2014`) |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply <spec>` (x2-byte-edit) |
| Flag hygiene | `python tools/flagcheck.py SERV_IRUHADEV_DEV_OVERLAY --worktree` |
| Build + deploy | `python tools/build.py --deploy` (background; x2-build-deploy) |
| Play-test logs | `python tools/logs.py --tag DEV_OVERLAY` (after O2 it reads `dev_overlay.log`) |
| Overlay (after O2) | `python tools/overlay.py add / check / list / diff / served / drop / move / off / on` (`/x2-overlay`) |

**Paths:**
- **Game dir:** `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data`.
- **Overlay:** `...\24965799\dev_overlay\<kom>\`, a sibling of `data\`. The engine resolves it as `..\dev_overlay\` from the cwd.
- **Archive:** `...\24965799\dev_overlay_archive\<stamp>\`.
- **Pack source:** `...\24965799\kom_contents\dataNNN_kom\`.
- **Replaced-Lua snapshots:** `kom_contents_bytecode\`, through `luasrc`.
- **Intermediates** go to `.build\` or F:, never the session scratchpad; C: is short of space.

## Established facts (2026-10-09, planning session; two Explore passes + direct checks)

Line numbers are in `KTDXLIB/KGCMassFileManager.cpp` unless noted.

**Build.**
- On: `WCHAR_DIR` (`Always_Global.h:75`), `MASSFILE_MAPPING_FUNCTION` (`Always_Global.h:329`), `X2OPTIMIZE_INDICATE_LOCAL_MASSFILE`
  (`KTDX.h:449`), `X2OPTIMIZE_ENCRYPT_AFTER_COMPRESS` (`KTDX.h:407`).
- Off: `MASS_FILE_FIRST`, `_SERVICE_`.

**Every Lua load reaches `LoadRealFile` (1249-1405).**
- `LoadAndDoMemory` / `LoadLuaTinker` go through `LoadDataFile` (884-980), which tries loose first (951) and the kom second.
- `IsValidFile` (666-723) tries loose first (opened, not read), then the kom.
- `LoadDataFile_LocalFile` and `IsValidFile_LocalFile` are loose only.
- The offline server-Lua loaders (StatTable, DropTable, BattleField, Attrib, Enchant, MapData, PetData, RandomItem, Resolve,
  Socket) probe `LoadDataFile`, then call `LoadAndDoMemory`. A loose file skips decryption, so plaintext works on the first try.
- No other file decryption is reachable:
  - every `.DoMemory(` caller is compiled out;
  - the other `XORDecrypt` calls decode hard-coded strings;
  - `LuaCommon` has only `DoMemoryNotEncrypt`.
- **The overlay only needs to hook `LoadRealFile`.** `LoadTwoRealFiles` (textures) and `GetPullPath` (music, fonts, `DeleteFile`)
  never load Lua, so they are left alone.

**Search roots.**
- `SetSubDirectory` runs once, at `KTDXDeviceManager.cpp:67`, before `new CX2Main()` (`X2.cpp:1316-1317`).
- It adds the cwd and every subfolder recursively.
- The data-directory list propagates to every thread's copy through a stamp (`GetUpdatedTLSData`, 46-57).
- `LoadRealFile` probes each root with `CreateFileW(dir + name)`.
- Nothing caches file contents: every load reopens the file.

**Long names.**
- `MASSFILE_NAMESIZE` is 60 (`KGCMassFileManager.h:85`). `StrCpyW( pOut->strFileName, ... )` at 1393 is unbounded.
- The struct is `pBuffer` 0-3, `strFileName` 4-123, `size` 124, `pRealData` 128.
- A loose name of 60-61 characters corrupts `size`; 62 or more corrupts `pRealData`.
- The kom path truncates safely (2019).

**Names the overlay must refuse.**
- The game writes these settings Lua and reads them through `LoadRealFile`: `GameOptions`, `GamePadConfig`, `LoginKey`,
  `SelectedServerGroup`, `TutorSystem` (`X2GameOption.cpp:775/795`, `X2InstanceData.cpp:1065/1075`, `X2TutorSystem.cpp:643/651`).
- A loose `Item.lua` or `SetItem.lua` makes `OpenItemScriptFile` build items from Lua instead of `item.kim`
  (`X2ItemManager.cpp:523-551`).
- The only `.lua` loose in `data\` today are the five settings files.

**Kom names are one flat, unique namespace.** `NOT_ALLOW_DUPLICATE_FILES` (`Always.h:3245`; 522-577): a name in two koms drops
the later kom.

**Reload behaviour.**
- UI dialogs re-run their Lua on each open (`KTDGUIDialog.cpp:114, 2328`).
- Templets parse once at startup.
- F2 (`REALTIME_SCRIPT_PATCH`, `X2Game.cpp:5491-5589`; auth >= special user; dungeon, training or battlefield) reloads
  `UIEffectSet`, `EffectSet`, `DamageEffect*`, `NewSkillTempletVer2` + `SkillData`, riding-pet scripts and `DevScriptTable`.
- F4 in the village (`VILLAGE_MAP_FAST_RELOADING`) rebuilds the village map dialogs.

**Tooling gaps.**
- `kom.py` has no lock, and `pack` does not check for a running game.
- Only `build.py` has a lock (`:127-171`) and a running check (`:83-98`).
- `luasrc convert` writes into `kom_contents` in place (`write_source`, `luasrc.py:318-319`).

**Engine findings deliberately out of scope** (the non-Lua route stays the kom flow, so these never trigger). They are recorded
here for whoever touches loose non-Lua files later:
- `LoadTwoRealFiles` opens the `.TET` into `hFile` instead of `hFile2` (1464-1466), so loose textures are broken. The same
  unbounded copy appears at 1582-1584.
- `X2Main.cpp:1031` deletes the first `item.kim` that `GetPullPath` finds, at every launch.
- 9 loose `data\` files shadow kom entries: 4 `.tga` in data068, 2 `.ttf` in datai022, and 3 music `.ogg` in datai017.

## Design

### Overlay location, layout and precedence
- **Location:** `...\24965799\dev_overlay\`, outside `data\`, so `SetSubDirectory` never adds it and `DeleteOtherFile` never
  scans it.
- **Layout:** one subfolder per kom, named exactly like it (`dataNNN` or `dataiNNN`). Each subfolder also holds its ledger
  `_overlay.json`.
- **Startup index (under the flag).** Right after `SetSubDirectory()` (`KTDXDeviceManager.cpp:67`):
  - resolve `..\dev_overlay\` with `GetFullPathNameW`;
  - list its subfolders and their `*.lua` files once (`FindFirstFileExW`, `FindExInfoBasic`);
  - build a read-only map: upper-case name -> `unsigned short` subfolder index;
  - log any other file in a subfolder as `ignored`, and never serve it;
  - build the map before any loader thread starts, and never write to it again.
- **Lookup.** `LoadRealFile` consults the map first. A hit opens `root\<sub>\<name>` directly; a miss falls through to the
  unchanged directory loop. With `MASSFILE_MAPPING_FUNCTION`, also look up the `ExternFileMapping` name, the same way the loop does
  (1277-1286).
- **Precedence:** overlay > loose `data\` tree > kom.
- **Duplicates.** A name in two subfolders: the lowest folder name wins, and the engine logs a `DUP` line. `check` refuses it.
- **Snapshot.** The index is built once. An edited, already-indexed file is seen on its next load; a newly added file needs a
  restart.
- **Missing root** means the feature is off, and the log says so.

### Engine flag `SERV_IRUHADEV_DEV_OVERLAY`
`KTDXLIB/Always.h`; CP949 files via `bytepatch.py`; original code kept in `#else`.
1. **Index + `LoadRealFile` lookup**, as above. Touches `KTDXLIB/KGCMassFileManager.cpp/.h` and `KTDXLIB/KTDXDeviceManager.cpp`.
2. **`dev_overlay.log`** in the cwd: a KTDXLIB-local writer under a critical section, flushed per line and rewritten per launch.
   ErrorLog loses lines written this early. Lines:
   - `[DEV_OVERLAY] root=<abs> folders=F lua=N ignored=I dup=D index_ms=T`, or `[DEV_OVERLAY] off: <abs> not found`;
   - `[DEV_OVERLAY] DUP <NAME> in dataA, dataB (using dataA)`;
   - `[DEV_OVERLAY] ignored <sub>\<name> (not .lua)`;
   - `[DEV_OVERLAY] served <NAME> <- dataNNN`, once per name, at use time;
   - `[DEV_OVERLAY] summary served=N` at shutdown, if a clean-exit hook is reachable from KTDXLIB. Otherwise drop this line and say
     so in the Done note.
3. **Bounded copy** at 1393: `wcsncpy_s`, truncating to 59 characters as the kom path does at 2019. Also review
   `IsValidFile`'s `strFileName[255]` (677). This protects any loose name, overlay or not.

With the flag off, the engine is the base. With the flag on and no overlay folder, only item 3 differs.

### Tool `tools/overlay.py` + skill `x2-overlay`
**Conventions:**
- **Path overrides:** `X2_GAME_DIR` / `X2_KOM_CONTENTS` (as in `kom.py:63-64`), plus `X2_OVERLAY` for the root.
- **Exit codes:** 0 ok, 1 failed, 2 usage, 3 game running, 4 refused (as in `kom.py:48`).
- **Lock:** mutating commands take `overlay.lock`, using the O_EXCL + stale-pid pattern in `build.py:127-171`. Share the code;
  don't copy it a third time. Promote holds the lock across pack and copy.

| Command | Does |
|---|---|
| `add <name.lua> [--kom K] [--as source\|bytecode] [--from file PATH] [--server-lua] [--plan P --phase X --note ..]` | Copies the base into `dev_overlay\<kom>\<name>` and writes the entry to that folder's `_overlay.json`. The kom comes from the live index; names found in several koms are refused. **`--as source`** (default): BOM+UTF-8 text from a `luasrc` source (`find`: sibling `.src.lua`, ClientScript; T0 until L2 lands, then T1, as luasrc offers). **`--as bytecode`**: copies March bytecode as is; it loads loose but can't be edited. **`--server-lua --from file`**: plaintext server Lua (e.g. `KncWX2Server/ServerResource/US/<name>.lua`), placed in the kom the user packs it into (data036 by default). |
| `move <name> --kom K2` | Moves a file and its entry to another folder. Refuses if the name lives in a live kom other than K2. |
| `check` | Verifies the list below. Prints `OVERLAY CHECK OK: N lua, K koms (S src / B bc / V server-lua)`, or every problem with exit 4. |
| `list [--kom K] [--plan P]` / `diff <name>` | Lists entries. `diff` shows a text diff against the base (bytecode bases are decompiled for display only, or shown as size + sha1). |
| `served` | Parses the last `dev_overlay.log`. Lists served entries and active entries **never served**, and checks that the run started after the newest overlay change. |
| `drop <name>` | Moves the file to `dev_overlay_archive\<stamp>\<kom>\` and marks the entry `dropped`. |
| `off` / `on` | Renames `dev_overlay` <-> `dev_overlay.off`. Refuses while the game runs. |
| `promote <K...> \| --plan P \| --all [--dry-run]` | See *Promote* below. |
| `retire <K...>` | Requires the live `dataK.kom` md5 to equal the promoted pack. Moves the promoted files to the archive, marks them `promoted`, and appends a row to `tools/data/overlay_promotions.tsv` (date, kom, entries, md5, plan, phase). |
| `selftest` | Runs every command against a temp game dir, a temp `kom_contents` and a tiny packed kom. |

**What `check` verifies:**
- every folder name is a kom name;
- every file is `.lua`;
- every file has an entry and every entry has a file;
- no name is in two folders;
- no name is in a live kom other than its folder's;
- the refused names;
- `luasrc.gate()` accepts every non-server file (BOM text that parses, March bytecode, or a ledger EXC);
- server Lua parses as plain text, with a warning only, as the pack gate does;
- no base drift: the `kom_contents` sha1 still equals `base_sha1`;
- the deployed `X2_offline.exe` carries the flag's log string.

**Refused names:**
- anything that isn't `.lua`;
- the five settings Lua, `Item.lua` and `SetItem.lua`;
- path separators;
- names over 59 characters;
- June `.lua` outside the `adopt-june` API;
- any `.lua` that exists in no kom, unless `--server-lua`.

**Ledger** `dev_overlay\<kom>\_overlay.json`, written atomically (temp + `os.replace`) under the lock. Per entry:
- `name`, `origin` (`march` / `june` / `server`);
- `base_sha1` (null when the file is new to the kom), `base_from`;
- `format` (`bom` / `bytecode` / `server-plain`), `lua_tier`, `algo` (`"2"`; `"0"` for server Lua);
- `plan`, `phase`, `added`, `note`, `state` (`active` / `promoted` / `dropped`).

The current sha1 is computed on demand, never stored.

### Promote (per kom; `--plan P` = every kom with an active entry of plan P; `--all` = every kom)
1. **Preconditions:** the lock is held, the game is not running, `check` is clean for these entries, and there is no base drift.
2. **Server Lua:** list the `server-plain` entries with their exact paths for the user to encrypt and pack. They stay in the
   overlay until the user confirms; the rest continues.
3. **Commit through `luasrc`:** this snapshots the replaced bytecode into `kom_contents_bytecode\`, writes the overlay file into
   `kom_contents\<K>_kom\`, and appends a `luasrc` ledger row. It needs `luasrc convert` split into produce-source and
   commit-to-dest. New names (a June Lua through `adopt-june`) are added to `_manifest.json` as `"2"` with the CRLF writer
   (`kom.py:516-518`).
4. **Pack:** `kom.py pack K` must print `PACK OK`.
5. **Decode back:** `elstools extract` the packed kom. Every promoted entry must be byte-equal to its overlay file, and the entry
   set must equal the directory. The tool prints `PROMOTE VERIFY OK <K>: N entries`.
6. **Copy:** ask once for the listed set, run `kom.py copy` per kom (`COPY OK`), then `retire`.

### After the switch (O5 writes this into CLAUDE.md and the skills)
- A phase that changed Lua finishes with `overlay.py check` OK. Lua is packed only at a plan's close-out or at release.
- Non-Lua changes keep `pack` + ask + `copy` as today.
- The hand-off names the overlay entries the phase added and the expected `served` lines, and says how each change becomes
  visible:

  | Change | Visible after |
  |---|---|
  | UI dialog Lua | reopening the dialog |
  | Effect, skill or damage-effect script | F2 in a dungeon |
  | Templet or table Lua | a restart |
  | Newly added overlay file | a restart |

## Phases

| Phase | What | Depends |
|---|---|---|
| O0 | Baseline | - |
| O1 | Engine flag | O0 |
| O1-review | Review of O1 | O1 |
| O2 | `overlay.py` core + skill | O0 |
| O3 | Lua gate matrix play-test | O1-review, O2 |
| O4 | Promote + retire + adopt-june API | O3, lua-source L2 committed |
| O5 | Workflow switch (Lua rules and skills) | O4 |
| O5-review | Doc sweep review + close-out | O5 |

## Phase O0 - Baseline

**Scope.** No code.
1. **Koms in sync.** Run `kom.py status --days 9999` for every kom that holds `.lua` (15 dirs; list them from `kom_contents`), and
   for the rest too. Pack anything `PACK NEEDED`; for `COPY NEEDED`, ask once for the set, then copy. The aim is `kom_contents` ==
   live koms.
2. **Baseline file.** Write `tools/data/overlay_baseline.tsv`: kom, md5, Lua entries, date.
3. **Checksums.** Read `X2CheckSumManager.cpp` around `:78`. Say which files it checksums, and whether an overlay `.lua` could trip
   it.
4. **KEM_ERROR baseline.** Record the KEM_ERROR count of a normal run from the current `ErrorLog.txt` (the user may need to launch
   once), for O1 to compare against.

**Run with:** Sonnet 5.5 high. **Read list:** Context, Rules, Starting context, Established facts, Phase O0.

**Done gate:**
- every kom reports in sync;
- the baseline tsv is committed;
- the checksum answer is in the Done note.

**Done note:**

## Phase O1 - Engine flag

**Scope.** Engine items 1-3 of *Design*. Touches `KTDXLIB/KGCMassFileManager.cpp/.h`, `KTDXLIB/KTDXDeviceManager.cpp` and
`KTDXLIB/Always.h`, all CP949 via `bytepatch.py`.
- **Lookup helper:** `bool DevOverlayFind( const std::wstring& upperName, std::wstring& outPath )`, with `MakeUpperCase`
  semantics identical to the kom map.
- **Served-set:** guarded by the log's critical section, because loader threads call `LoadRealFile` concurrently.

**Run with:** Opus 5.5 high. **Read list:** Rules, Starting context, Established facts, Design ("Overlay location", "Engine flag"),
Phase O1.

**Done gate:**
- `flagcheck SERV_IRUHADEV_DEV_OVERLAY --worktree` passes; `DEPLOY OK`.
- **Play-test (a), no `dev_overlay` folder:** the log says `off`, the KEM_ERROR count matches the O0 baseline, and nothing visible
  changes.
- **Play-test (b), the Peita fixture.** Create `dev_overlay\data217\` with a copy of
  `kom_contents\data217_kom\DLG_Map_Local_Peita_Icon_NEW.lua`, Bonghun Temple `LEFT_TOP` y +100 (200/192/194 -> 300/292/294), plus
  one non-Lua file (e.g. a copy of a small `.tga`). Expected:
  - opening the Peita local map shows the button 100 px lower;
  - the log has one `served DLG_MAP_LOCAL_PEITA_ICON_NEW.LUA <- data217` line, one `ignored` line for the non-Lua file, and
    `index_ms`.
- **Cleanup:** delete `dev_overlay\` afterwards; it is a test fixture (O2's tool does not exist yet).
- Write `kickoffs/dev-overlay/O1-review.md` if it is missing.

**Done note:**

## Phase O1-review

**Scope.** Adversarial review of O1. Check:
- flag off = base, every `#else` intact;
- the served-set is thread-safe;
- the index is built before any loader thread starts;
- the `ExternFileMapping` name is looked up;
- the non-`.lua` filter holds;
- truncation matches the kom path;
- no lookup path other than `LoadRealFile` changed behaviour.

Fix what is wrong; design questions go to the user.

**Run with:** Opus 5.5 high. **Read list:** Established facts, Design, Phase O1 (+ its Done note).

**Done gate:** the findings are fixed or answered. If code changed, rebuild and repeat play-test (b).

**Done note:**

## Phase O2 - `overlay.py` core + skill `x2-overlay`

**Scope.**
- **`tools/overlay.py`:**
  - commands: `add` (`--as source` / `--as bytecode`, `--server-lua --from file`), `move`, `check`, `list`, `diff`, `served`,
    `drop`, `off` / `on`, `selftest`;
  - the per-kom `_overlay.json` ledger, the shared lock, and the refused names;
  - stubs for `promote` and `retire` that exit 2 with "not yet (O4)".
- **`tools/logs.py`:** read `dev_overlay.log`.
- **Skill:** `.claude/skills/x2-overlay/SKILL.md`, in the shape of `x2-kom-packed`.
- **Docs:** a `tools/README.md` row.
- **Reuse, don't re-implement:** import from `kom.py` (live index, manifest writer, `running()`) and `luasrc.py` (`find`,
  sources, `gate()`). Do not edit `luasrc.py` in this phase, because lua-source L2 is changing it.

**Run with:** Sonnet 5.5 high. **Read list:** Rules, Starting context, Design ("Tool", "Promote" for the stubs), Phase O2.

**Done gate:**
- `overlay.py selftest` passes.
- In scratch dirs, `check` refuses each of these: a non-Lua file; BOM-less Lua; each refused-name class; base drift; an untracked
  file; an entry without a file; a name in two folders; a name owned by another live kom.
- `add --as source` on a data217 dialog produces a file `luasrc.gate()` accepts.
- No live kom and no `kom_contents` file is modified.

**Done note:**

## Phase O3 - Lua gate matrix play-test

**Scope.** Add one item per kind with `overlay.py` and give it a **visible** change. For each, the Done note records the
character and job, place, steps, expected result, failure look, and `served` line.
1. UI dialog Lua, BOM source, seen on reopen.
2. A startup templet or table Lua (restart).
3. A script F2 reloads (e.g. an `EffectSet` / `DamageEffect` value), seen after F2 in a dungeon.
4. March bytecode copied with `--as bytecode` (loads unchanged; `served` line only).
5. An offline server Lua as plaintext (`--server-lua`); its loader logs "loaded N row(s)" from the overlay copy.
6. A file edited while the game runs: seen on its next load.
7. A file added while the game runs: seen only after a restart.
8. The same name planted by hand in two folders: a `DUP` line, the lowest folder wins, and `check` refuses.
9. A non-Lua file planted by hand: an `ignored` line, never served, and `check` refuses.

Then `drop` everything and confirm a run with 0 `served`. Test-only changes never reach a kom.

**Run with:** Sonnet 5.5 high, then user play-tests. **Read list:** Rules, Design, Phase O3.

**Done gate:**
- every row passes, with no new KEM_ERROR against the O0 baseline;
- after the drop, 0 `served`;
- each failure becomes either a fix in O1's flag (then rerun that row) or a named limitation the user accepts.

**Done note:**

## Phase O4 - Promote + retire + adopt-june API

**Scope.**
- **`promote` / `retire`** exactly as in *Promote*, plus `tools/data/overlay_promotions.tsv`.
- **`luasrc` split:** split `luasrc convert` into produce-source and commit-to-dest. **Start from lua-source L2's committed
  `luasrc.py`**; if another session has uncommitted changes in it, stop and say so.
- **Placement API:** the function lua-source L3's `adopt-june` will call to place a June `.lua` in the overlay, with origin
  `june` and the kom chosen by the stage-june placement rule (`kom.py:565-577`).
- **Pilot on data217:**
  1. Ask the user for a real change; if there isn't one, make an agreed harmless edit and revert it with a second promotion.
  2. Add the change, then play-test.
  3. `promote` (`PACK OK`, `PROMOTE VERIFY OK`), ask, copy (`COPY OK`), `retire`.
  4. Play-test with `overlay.py off`: same result, 0 `served`.
- **Release readiness:** `promote --all --dry-run`.

**Run with:** Opus 5.5 high. **Read list:** Rules, Starting context, Design ("Tool", "Promote"), Phase O4.

**Done gate:**
- the pilot's four OK lines are quoted;
- the retire md5 matches;
- the overlay-off play-test passes;
- the `promotions.tsv` row is committed;
- the placement API passes `selftest`;
- the dry run has no refusals.

**Done note:**

## Phase O5 - Workflow switch (Lua rules and skills)

**Scope.**
- **CLAUDE.md:**
  - the Workflow table: add `overlay.py` and `/x2-overlay`;
  - the packing bullet: Lua is packed only at close-out or release, with the ask-once-per-set rule; non-Lua is unchanged;
  - the server-Lua rule's "Do not write a loose copy into the game directory" bullet: plaintext server Lua may live in
    `dev_overlay` through the tool, and encrypt+pack happens at promote; never loose in `data\`;
  - the Finish list: `overlay.py check` for Lua changes;
  - a short *Dev overlay* section: precedence, Lua-only, refused names.
- **Skills:**
  - `x2-lua`: convert and edit into the overlay, pack only via promote;
  - `x2-phase`: Finish and hand-off;
  - `elstools` and `x2-kom-packed`: non-Lua as before; Lua packing only through promote;
  - `x2-check-logs`: the `DEV_OVERLAY` tag.
- **Lua source plan:** `LUA_SOURCE_PHASES.md` and the `kickoffs/lua-source/` L3 and L7 kickoffs. `adopt-june` writes into the
  overlay through O4's API.
- **Memory:** new `dev-overlay-workflow`; update `kom-pack-copy-skills`, `server-lua-packing-is-users-job` and `lua-source-plan`.

**Run with:** Opus 5.5 high. **Read list:** Rules, Design ("After the switch"), Phase O5; the files named above, section by
section.

**Done gate:**
- A grep across CLAUDE.md, `.claude/skills/`, `kickoffs/` and STATUS.md for `x2-kom-packed`, `elstools pack`, `kom.py pack`,
  `luasrc.py convert` and "loose copy" shows Lua packing only at promote. Non-Lua uses stay unchanged.
- The Done note lists every file changed.

**Done note:**

## Phase O5-review + close-out

**Scope.**
- Review O5's changed sections for contradictions and stale Lua pack/copy or loose-copy instructions, and fix them.
- This plan's own close-out promote: `promote --plan dev-overlay --dry-run` must list nothing.
- Finalise the memory entries.

**Run with:** Opus 5.5 high. **Read list:** Rules, Phase O5 (+ Done note).

**Done gate:**
- the findings are fixed;
- the dry run is empty;
- the STATUS block reads done.

**Done note:**

## Effects on other plans
- **lua-source:**
  - L2 is unchanged.
  - L3's `adopt-june` places June Lua through O4's API (O5 rewrites the kickoff).
  - L7's convention sweep includes the overlay.
  - O4 starts only after L2's `luasrc.py` is committed.
- **Every plan from O5 on:**
  - Lua changes finish with `overlay.py check`, and only close-out phases promote them.
  - Non-Lua changes, including june-asset-port, stay on the kom flow.

## Play-test hand-off
Each phase's hand-off follows the standing rule: copy-ready, with the exact character, place, steps, expected result and failure
look. It also names the overlay entries in play and the `served` lines that `python tools/logs.py --tag DEV_OVERLAY` should
show.
