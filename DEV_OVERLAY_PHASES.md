# Dev overlay: loose files for development, pack at plan close-out and release

Plan key `dev-overlay` · flag `SERV_IRUHADEV_DEV_OVERLAY` · log tag `[DEV_OVERLAY]` (file `dev_overlay.log`) · kickoffs
`kickoffs/dev-overlay/` · tool `tools/overlay.py` + skill `x2-overlay` (both built here) · created 2026-10-09 (user request).

## Context

**Before.** Every content change went through the same loop: edit `kom_contents\dataNNN_kom\`, `kom.py pack`, ask, `kom.py copy`,
play-test. Every `.lua` edit also needed a convert or decompile first. Ports kept stalling on that loop.

**What 2026-10-09 proved.** The engine reads loose files ahead of the koms:
- `LoadDataFile` calls `LoadRealFile` before the kom map (`KTDXLIB/KGCMassFileManager.cpp:950-962`; `MASS_FILE_FIRST` is off).
- A loose file is flagged `MFI_LOCALFILE`, so `LoadAndDoMemory_ErrorCode` skips XOR and zlib (`KTDXLIB/KTDXApp.h:585-589`). It
  then hands the raw bytes to the same `DoMemoryNotEncrypt` the kom path ends in (`:621-658`).
- Play-test: a loose BOM+UTF-8 copy of `DLG_Map_Local_Peita_Icon_NEW.lua` in `data\lua_dev\` moved the Bonghun Temple button
  100 px, with 0 KEM_ERROR. The test file was then deleted.

**Goal.**
- Development happens in one **dev overlay** folder that the game reads first.
- A per-kom ledger records what changed and where each file ships.
- Packing ("promote") happens only at a plan's close-out and at release.
- The existing tools are reused: `kom.py`, `luasrc.py`, `junekom.py`, the Lua pack gate, and `elstools`.

**User decisions (2026-10-09):**
- **Promote cadence.** Each plan's close-out phase promotes the files that plan owns. A final `promote --all` at release sweeps
  the rest.
- **Server-only Lua** may sit in the overlay as plaintext; the offline loaders read it. At promote the tool stops and lists those
  files, and the user encrypts and packs them, as now.
- **`june-asset-port` is rebased onto the overlay.** Its kickoffs and STATUS rows are rewritten in O6.
- **Copy asks.** A multi-kom promotion asks **once for the listed set** (kom, entries, md5). Every copy still needs that OK.
- **Layout.** One subfolder per kom (`dev_overlay\data036\...`), not one flat folder.

## Rules

1. **Overlay only.** Loose development files go into `dev_overlay\<kom>\` and nowhere else.
   - Never put them in `data\`. The `data\` tree is searched behind the overlay, and `DeleteOtherFile` scans it.
   - Only `overlay.py` writes into the overlay, so each file gets a ledger entry.
2. **The ledger is the record.** A file without an entry, or an entry without a file, fails `check`. Promotion reads only the
   ledger and the files.
3. **Standing rules still apply.**
   - No new `.lua` (CLAUDE.md), except server Lua the client needs (`--server-lua`).
   - June `.lua` only through `adopt-june` (lua-source rule 2).
   - June assets are byte-exact.
   - Ask before every copy; the set decision above applies.
   - CP949 sources are edited with `bytepatch.py` only.
   - The save file is untouched by this plan.
4. **Check before every hand-off.** Every play-test hand-off, in this plan and in every plan after O6, runs
   `overlay.py check` and quotes its OK line.
5. **Nothing leaves the overlay before `COPY OK`.** `retire` moves files out only after the live kom's md5 equals the promoted
   pack, so the next play-test runs on the koms and nothing is masked.
6. **Byte equality is the promotion proof.** Promote decodes the packed kom back and requires every promoted entry to be
   byte-equal to its overlay file. A play-test with the overlay `off` then confirms in game.

## Starting context

Every repeated step is a tool (`tools/README.md`). Do not hand-type msbuild, elstools, kom packing or log greps.

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py DEV_OVERLAY_PHASES.md "<heading>"` |
| Kom pack / copy / status | `python tools/kom.py pack / copy / status dataNNN` (`/elstools pack`, `/x2-kom-packed copy`) |
| Lua sources, gate, ledger | `python tools/luasrc.py find / convert / ledger` (`/x2-lua`); `luasrc.gate()` is the pack gate |
| June kom reader (byte-exact) | `python tools/junekom.py list / extract / verify ...` |
| June placement rule | `kom.py stage-june` (`tools/kom.py:535-674`, placement `:565-577`, algo rule `:411-415`) |
| Inspect / extract a March kom | `elstools/elstools.exe inspect -q -i <kom>` / `extract` (absolute paths, `-q`, stdin closed, `-v mar-2014`) |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply <spec>` (x2-byte-edit) |
| Flag hygiene | `python tools/flagcheck.py SERV_IRUHADEV_DEV_OVERLAY --worktree` |
| Build + deploy | `python tools/build.py --deploy` (background; x2-build-deploy) |
| Play-test logs | `python tools/logs.py --tag DEV_OVERLAY` (after O2 it reads `dev_overlay.log`) |
| Overlay (after O2) | `python tools/overlay.py add / new / check / list / diff / served / drop / off / on` (`/x2-overlay`) |

**Paths:**
- **Game dir:** `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data`.
- **Overlay:** `...\24965799\dev_overlay\<kom>\` (sibling of `data\`). The engine resolves it as `..\dev_overlay\` from the cwd.
- **Archive:** `...\24965799\dev_overlay_archive\<stamp>\`.
- **Pack source:** `...\24965799\kom_contents\dataNNN_kom\`.
- **Replaced-file snapshots:** `...\24965799\kom_contents_overlay_backup\<stamp>\`, and `kom_contents_bytecode\` for Lua.
- **Intermediates** go to `.build\` or F:, never the session scratchpad (C: is short of space).

## Established facts (2026-10-09, planning session; two Explore passes + direct checks)

Line numbers are `KTDXLIB/KGCMassFileManager.cpp` unless noted. Build facts:
- **Flags:** `WCHAR_DIR` (`Always_Global.h:75`), `MASSFILE_MAPPING_FUNCTION` (`Always_Global.h:329`),
  `X2OPTIMIZE_INDICATE_LOCAL_MASSFILE` (`KTDX.h:449`), `X2OPTIMIZE_ENCRYPT_AFTER_COMPRESS` (`KTDX.h:407`) and
  `X2OPTIMIZE_SUPPORT_LOADING_TWO_ADJACENT_MASS_FILES` (`KTDX.h:416`) are on. `MASS_FILE_FIRST` and `_SERVICE_` are off.

**Entry points and their order:**
- `LoadDataFile` (884-980): loose first (951), then kom (954-962).
- `IsValidFile` (666-723): loose first, opened but not read (`dwFlag=0`), then kom.
- `LoadDataFile_LocalFile` / `IsValidFile_LocalFile`: loose only.
- `LoadTwoDataFiles` (762-878), for every texture + `.TET` (`KTDXDeviceTexture.cpp:50-56`): loose via `LoadTwoRealFiles`
  (1410-1600), then kom.
- `GetPullPath` (3782-3867): loose only, `_wfopen` per directory. It is used by music (`KTDSMP3Play.cpp:44-71`), fonts
  (`KTDGFontManager.cpp:1856`, `X2Main.cpp:7806`), `X2CheckSumManager.cpp:78`, and `CX2Main::DeleteFile`.
- **No existence check is kom-only.** The kom-only functions (`GetTotalFileCount`, `MassFileMapping`, `CMassFile::GetMemberFile`)
  never decide whether a game file exists.

**Search roots:**
- `SetSubDirectory` runs once, at `KTDXDeviceManager.cpp:67`, before `new CX2Main()` (`X2.cpp:1316-1317`).
- It adds the cwd, then every subfolder recursively, skipping `.svn`, `Branches`, `epay` and `SupportFiles` (3711-3715).
- **`AddDataDirectory`:** the `wstring` overload honours `bInsertFront` (400-432). The `string` overload drops it (168-173).
- **Threads:** a stamp propagates the list to every thread's copy (`GetUpdatedTLSData`, 46-57).
- **Today's subfolders of `data\`:** `ScreenCapture`, `XTrap`, `db_backup`, `eventscene`, `movie`, `mssql_data`, `music`, `ui`.

**Lookup cost:** `LoadRealFile` (1249-1405) probes each root with `CreateFileW(dir + name)`. Nothing caches file contents
(`m_massFileBufMan` is a buffer pool), so every load reopens the file.

**Defects that block loose files:**
- **Textures.** `LoadTwoRealFiles` assigns the `.TET` open to `hFile`, not `hFile2` (1464-1466).
  - Loose texture, no loose TET: the texture handle leaks (`FILE_SHARE_READ` only), the function returns false, and the kom copy
    wins.
  - Loose texture and loose TET: the TET bytes are returned as the texture (KEM_ERROR89, `KTDXDeviceTexture.cpp:74-81`).
  - Even fixed, the TET is looked up only in the texture's own folder when the texture is loose, and only in the kom when the
    texture is in a kom (831-858).
- **`item.kim` deletion.** `X2Main.cpp:1031` (`X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING || _IN_HOUSE_`) calls `CX2Main::DeleteFile`
  (`X2Main.cpp:5832-5847`). That resolves the name through `GetPullPath`, then calls `::DeleteFile`.
  - Every name in that `DeleteFile` block (`X2Main.cpp:~990-1033`) is deleted wherever `GetPullPath` finds it.
  - `DeleteOtherFile` (`X2Main.cpp:1037-1039`, 8847-8939) recursively deletes unlisted `*.KOM`, `*.KOM_` and `*.DLL` under the
    cwd.
- **Long names.** `MASSFILE_NAMESIZE` is 60 (`KGCMassFileManager.h:85`), and `StrCpyW( pOut->strFileName, ... )` is unbounded
  (1393, 1582-1584).
  - The struct layout is `pBuffer` 0-3, `strFileName` 4-123, `size` 124, `pRealData` 128, `dwFlag` 132.
  - A loose name of 60-61 characters corrupts `size`; 62 or more corrupts `pRealData`.
  - The kom path truncates safely (2019). June has 5 names longer than 59.
- **9 existing `data\` files shadow kom entries** (from `.build/stage_june_live_index.json`):
  - `dlg_first_loading0.tga`, `dlg_first_loading1.tga`, `dlg_new_unit_background01.tga` and `dlg_new_unit_background02.tga`
    (data068). These are inert today because of the texture defect; fixing it makes them win.
  - `blambotprol.ttf` and `tahoma.ttf` (datai022).
  - `music\event_halloweendungeon.ogg`, `fishinghole01.ogg` and `halloween_event_monster.ogg` (datai017).

**Names the overlay must refuse:**
- **Settings Lua** written by the game: `GameOptions`, `GamePadConfig`, `LoginKey`, `SelectedServerGroup` and `TutorSystem`
  (`X2GameOption.cpp:775/795`, `X2InstanceData.cpp:1065/1075`, `X2TutorSystem.cpp:643/651`).
- **`Item.lua` and `SetItem.lua`.** A loose copy of either makes `OpenItemScriptFile` build items from Lua instead of `item.kim`
  (`X2ItemManager.cpp:523-551`).

**`ShopImageName.txt` freshness** (`X2ItemManager.cpp:~4580`, `SERV_IRUHADEV_JUNE_ITEM_KIM`): the listing is skipped while the txt
is newer than `data036.kom`. An overlay `item.kim` never changes that timestamp.

**Kom duplicates are fatal** (`NOT_ALLOW_DUPLICATE_FILES`, `Always.h:3245`; 522-577). A name in two koms drops the later kom
(MessageBox, freed members, double delete). The kom namespace is flat and unique.

**What already works with loose files:**
- **Offline server-Lua loaders** (StatTable 101-171, DropTable 94-155, BattleField 131-163 and 286-329, Attrib, Enchant, MapData,
  PetData, RandomItem, Resolve, Socket) probe `LoadDataFile`, then `LoadAndDoMemory`. A loose file skips the decrypt, so
  plaintext works on the first try.
  - A loose file that is still XOR'd or zlib'd fails both attempts, with a log line.
- **Raw-byte readers** read loose files the same as kom members: world scripts, `EnumString.lua`, XET, FX, SFX and meshes.
- **No other file decryption is reachable.** Every `.DoMemory(` caller is compiled out, the other `XORDecrypt` calls decode
  hard-coded strings, and `LuaCommon` has only `DoMemoryNotEncrypt`.

**Reload behaviour:**
- **UI dialogs** re-run their Lua on each open (`KTDGUIDialog.cpp:114, 2328`).
- **Templets** parse once at startup.
- **F2** (`REALTIME_SCRIPT_PATCH`, `X2Game.cpp:5491-5589`; auth >= special user; dungeon, training or battlefield) reloads the
  XMeshPlayer `.txt`, `UIEffectSet`, `EffectSet`, `DamageEffect*`, `NewSkillTempletVer2` + `SkillData`, riding-pet scripts and
  `DevScriptTable`.
- **F4** in the village (`VILLAGE_MAP_FAST_RELOADING`) rebuilds the village map dialogs.

**Tooling gaps:** `kom.py` has no lock, and `pack` does not check for a running game. Only `build.py` has a lock (`:127-171`) and
a running check (`:83-98`).

## Design

### Overlay location, layout and precedence
- **Location:** `...\24965799\dev_overlay\`, outside `data\`, so `SetSubDirectory` never adds it and `DeleteOtherFile` never scans
  it.
- **Layout:** one subfolder per kom, named exactly like it: `dataNNN` or `dataiNNN`.
  - Each subfolder holds its ledger `_overlay.json`, which the engine never serves.
- **Startup index.** Under the flag, right after `SetSubDirectory()` (`KTDXDeviceManager.cpp:67`), the engine:
  - resolves `..\dev_overlay\` with `GetFullPathNameW`;
  - lists its subfolders and files once (`FindFirstFileExW`, `FindExInfoBasic`);
  - builds a read-only map: upper-case name -> `unsigned short` subfolder index.
  - The map is built before any loader thread starts and is never written again.
- **Lookups.** `LoadRealFile`, `LoadTwoRealFiles` (both names) and `GetPullPath` consult the map first.
  - A hit opens `root\<sub>\<name>` directly; a miss falls through to the unchanged directory loop.
  - With `MASSFILE_MAPPING_FUNCTION`, look up the `ExternFileMapping` name too, the way `LoadRealFile` does (1277-1286).
- **Precedence:** **overlay > loose `data\` tree > kom**. Overlay folder count costs nothing per load.
- **Duplicate across subfolders:** the lowest folder name wins, and the engine logs a `DUP` line. `check` refuses duplicates.
- **Snapshot:** the index is built once. An edited indexed file is seen on its next load; an added file needs a restart.
- **Missing root:** the feature is off, and the log says so.

### Engine flag `SERV_IRUHADEV_DEV_OVERLAY`
Defined in `KTDXLIB/Always.h`. CP949 files are edited via `bytepatch.py`, with the original code kept in `#else`.
1. **Index and lookups** as above.
2. **`dev_overlay.log`** in the cwd: a KTDXLIB-local writer under a critical section, flushed per line and rewritten per launch.
   ErrorLog loses lines written this early.
   - `[DEV_OVERLAY] root=<abs> folders=F files=N dup=D index_ms=T`, or `[DEV_OVERLAY] off: <abs> not found`.
   - `[DEV_OVERLAY] DUP <NAME> in dataA, dataB (using dataA)`.
   - `[DEV_OVERLAY] served <NAME> <- dataNNN`, once per name, at use time.
   - `[DEV_OVERLAY] kept <path> (DeleteFile skipped)`.
   - `[DEV_OVERLAY] summary served=N` at shutdown, if a clean-exit hook is reachable from KTDXLIB. Otherwise drop this line and
     say so in the Done note.
3. **Textures (`LoadTwoRealFiles` / `LoadTwoDataFiles`).** Fix `hFile2`, then resolve the texture and its TET **independently**,
   each overlay -> loose dirs -> kom.
   - The TET path stays quiet: no new KEM_ERROR136 or 141. `LoadDataFile`'s `.TET` exclusion is compiled out (965-975), so do
     not route through it.
4. **Bounded copies** at 1393 and 1582-1584: `wcsncpy_s` truncation to 59 characters, as the kom path does at 2019. Also review
   `IsValidFile`'s `strFileName[255]` (677).
5. **`CX2Main::DeleteFile`** (`X2Main.cpp:5832`) never deletes a path under the overlay root.
6. **`ShopImageName.txt`** is also stale when the overlay holds `item.kim` with a newer mtime. Extend the existing block.

With the flag off, the engine is the base. With the flag on and no overlay folder, only items 3, 4 and 6 differ; they fix broken
paths and leave working ones alone.

### Tool `tools/overlay.py` + skill `x2-overlay`

**Conventions:**
- Path overrides: `X2_GAME_DIR` / `X2_KOM_CONTENTS` (as `kom.py:63-64`), and `X2_OVERLAY` for the root.
- Exit codes: 0 ok, 1 failed, 2 usage, 3 game running, 4 refused (as `kom.py:48`).
- **Lock:** mutating commands take `overlay.lock` (O_EXCL + stale pid, the pattern in `build.py:127-171`; share the code, do not
  copy it a third time). Promote holds it across pack and copy.

| Command | Does |
|---|---|
| `add <name> [--kom K] [--from march\|june\|file PATH] [--as bytecode\|source] [--server-lua] [--long-names] [--plan P --phase X --note ..]` | Copies the base into `dev_overlay\<kom>\<name>` and writes the entry to that folder's `_overlay.json`. **March:** from `kom_contents`; the kom comes from the live index; refuses names found in several koms. **June:** `junekom.decode`, byte-exact; placement follows the stage-june rule. **`.lua --as source`:** writes BOM+UTF-8 text from a `luasrc` T0 source and refuses without one until L2 lands. **`.lua --as bytecode`:** copies March bytecode, which loads loose but cannot be edited. **`file PATH`:** an explicit base (e.g. server Lua from `KncWX2Server/ServerResource/US/`). |
| `new <name> --kom K` | A file in no kom: a June-only asset, or a new texture. Refuses `.lua` unless `--server-lua`. Refuses a name that any live kom has. |
| `move <name> --kom K2` | Moves a file and its entry to another folder. Refuses if the name lives in a live kom other than K2. |
| `check` | See the list below. Prints `OVERLAY CHECK OK: N files, K koms, L lua (S src / B bc), V server-lua` or every problem, then exits 4. |
| `list [--kom K] [--plan P]` / `diff <name>` | Lists the entries. `diff` shows a text diff against the base for `.lua` / `.txt`, and size + sha1 for binaries. |
| `served` | Parses the last `dev_overlay.log`. Lists the served entries and the active entries **never served**, and says whether that run started after the newest overlay change. |
| `drop <name>` | Moves the file to `dev_overlay_archive\<stamp>\<kom>\` and marks the entry `dropped`. |
| `off` / `on` | Renames `dev_overlay` <-> `dev_overlay.off`. Refuses while the game runs. |
| `promote <K...> \| --plan P \| --all [--dry-run]` | See *Promote* below. |
| `retire <K...>` | Requires the live `dataK.kom` md5 to equal the promoted pack. Moves the promoted files to the archive, marks them `promoted`, and appends a row to `tools/data/overlay_promotions.tsv` (date, kom, entries, md5, backup, plan, phase). |
| `selftest` | Every command against a temp game dir, a temp `kom_contents` and a tiny packed kom. |

**What `check` verifies:**
- every folder name is a kom name;
- file <-> entry, both ways;
- no name in two folders;
- no name in a live kom other than its folder's;
- the refused-name list;
- `luasrc.gate()` passes for every `.lua` (BOM text that parses, March bytecode, or a ledger EXC);
- no base drift: `kom_contents` sha1 still equals `base_sha1`;
- no `data\` shadow outside the O0 allowlist;
- the deployed `X2_offline.exe` carries the flag's log string.

**Refused names:**
- the five settings Lua;
- `Item.lua` and `SetItem.lua`;
- `.kom`, `.kom_` and `.dll`;
- path separators;
- names on the `X2Main.cpp` startup `DeleteFile` list;
- names over 59 characters without `--long-names`;
- June `.lua` outside the `adopt-june` API;
- any new `.lua` without `--server-lua`.

**Ledger `dev_overlay\<kom>\_overlay.json`.** Written atomically (temp file + `os.replace`) under the lock. Per entry:
- `name`, `kind` (`modify` / `add`), `origin` (`march` / `june` / `server` / `new`);
- `base_sha1` (null for `add`), `base_from`;
- `format` (`bom` / `bytecode` / `binary` / `server-plain`), `lua_tier`;
- `algo` (stage-june rule: `.lua`, `.txt`, `.kim` = `"2"`, else `"0"`);
- `plan`, `phase`, `added`, `note`, `state` (`active` / `promoted` / `dropped`).

The current sha1 is computed on demand, never stored.

### Promote (per kom; `--plan P` = every kom with an active entry of plan P; `--all` = every kom)
1. **Preconditions:** the lock is held, the game is not running, `check` is clean for these entries, and there is no base drift.
2. **Server Lua:** `server-plain` entries are listed with their exact paths for the user to encrypt and pack. They stay in the
   overlay; the rest continues.
3. **Snapshot** the files about to be replaced into `kom_contents_overlay_backup\<stamp>\<K>_kom\`. Lua bytecode goes to
   `kom_contents_bytecode\` through `luasrc`.
4. **Copy** the overlay files (not `_overlay.json`) into `kom_contents\<K>_kom\`.
   - Add new names to `_manifest.json` (CRLF writer, `kom.py:516-518`).
   - Write `luasrc` ledger rows for converted Lua. This needs `convert` split into produce-source and commit-to-dest.
5. **Pack:** `kom.py pack K` must print `PACK OK`.
6. **Decode back:** `elstools extract` the packed kom. Every promoted entry must be byte-equal to its overlay file, and the entry
   set must equal the directory. The tool prints `PROMOTE VERIFY OK <K>: N entries`.
7. **Copy:** ask once for the listed set, run `kom.py copy` per kom (`COPY OK`), then `retire`.

### After the switch (O6 writes this into CLAUDE.md and the skills)
- **Finish:** a phase ends with `overlay.py check` OK. Pack and copy run only at a plan's close-out or at release.
- **Hand-off:** it names the overlay entries the phase added and the expected `served` lines, and says how each change becomes
  visible:

  | Change | Becomes visible after |
  |---|---|
  | UI dialog Lua | reopening the dialog |
  | Effect, skill or damage-effect script | F2 in a dungeon |
  | Templet, item or stat table | a restart |
  | Newly added file | a restart |

## Phases

| Phase | What | Depends |
|---|---|---|
| O0 | Baseline + census | - |
| O1 | Engine flag | O0 |
| O1-review | Review of O1 | O1 |
| O2 | `overlay.py` core + skill | O0 |
| O3 | Gate matrix play-test | O1-review, O2 |
| O4 | Promote + retire | O3, lua-source L2 committed |
| O5 | June path | O4 |
| O6 | Workflow switch + plan rebases | O5 |
| O6-review | Doc sweep review + close-out | O6 |

## Phase O0 - Baseline + census

**Scope.** No code.
1. **Status of every kom.** Run `kom.py status --days 9999` for every kom. Pack any `PACK NEEDED`; for `COPY NEEDED`, ask once for
   the set, then copy. The goal is `kom_contents` == live koms.
2. **Baseline.** Write `tools/data/overlay_baseline.tsv`: kom, md5, entries, date.
3. **The 9 shadowing files.** Byte-compare each with its kom member (extract with `elstools`).
   - Report identical or different, with sizes and sha1.
   - The user decides keep (allowlist) or archive. Archiving moves the file to `...\24965799\data_shadow_archive\`; never delete.
   - Write the allowlist to `tools/data/overlay_shadow_allowlist.tsv`.
4. **Checksum targets.** Read `X2CheckSumManager.cpp` around `:78` and say which files it checksums, and whether an overlay copy of
   any of them would trip a check.
5. **Baseline counts.** Record the KEM_ERROR count of a normal run from the current `ErrorLog.txt` (the user may need to launch
   once). O1 compares against it.

**Run with:** Sonnet 5.5 high. **Read list:** Context, Rules, Starting context, Established facts, Phase O0.

**Done gate:**
- every kom reports in sync;
- the baseline tsv is committed;
- the allowlist decision is recorded;
- the checksum answer is in the Done note.

**Done note:**

## Phase O1 - Engine flag

**Scope.** Engine items 1-6 of *Design*. Touches `KTDXLIB/KGCMassFileManager.cpp/.h`, `KTDXLIB/KTDXDeviceManager.cpp`,
`X2Lib/X2Main.cpp`, `X2Lib/X2ItemManager.cpp` and `KTDXLIB/Always.h`, all CP949 via `bytepatch.py`.
- **Index lookup:** one helper, `bool DevOverlayFind( const std::wstring& upperName, std::wstring& outPath )`, used by all three
  lookup sites. Keep `MakeUpperCase` semantics identical to the kom map.
- **TET lookup:** a quiet helper that probes the loose dirs, then `m_mapMassFile`, without any `ErrorLogMsg` or
  `LeaveLastErrorLog`.
- **Served-set:** guarded by the log's critical section. Loader threads call `LoadRealFile` concurrently.

**Run with:** Opus 5.5 high. **Read list:** Rules, Starting context, Established facts, Design ("Overlay location", "Engine flag"),
Phase O1.

**Done gate:**
- `flagcheck SERV_IRUHADEV_DEV_OVERLAY --worktree` passes, and `DEPLOY OK`.
- **Play-test (a), no `dev_overlay` folder:** the log says `off`, the KEM_ERROR count matches the O0 baseline, and nothing visible
  changes.
- **Play-test (b), Peita file in `dev_overlay\data217\`:** create the folder, then copy `kom_contents\data217_kom\` ->
  `DLG_Map_Local_Peita_Icon_NEW.lua` with the Bonghun Temple `LEFT_TOP` y +100 (200/192/194 -> 300/292/294). Opening the Peita
  local map shows the button 100 px lower, the log has one `served DLG_MAP_LOCAL_PEITA_ICON_NEW.LUA <- data217` line, and
  `index_ms` is logged.
  - **Before O2 exists:** this test file is the only thing in the overlay. Delete `dev_overlay\` after the play-test; it is a
    test fixture, not a change.
- Write `kickoffs/dev-overlay/O1-review.md` if it is missing.

**Done note:**

## Phase O1-review

**Scope.** An adversarial review of O1:
- flag off = base, and every `#else` intact;
- no new KEM lines on the TET path;
- the served-set is thread-safe;
- the index is built before any loader thread;
- the ExternFileMapping name is looked up too;
- the bounded copies truncate exactly as the kom path does;
- the `DeleteFile` guard compares normalised full paths;
- nothing in the overlay can be reached by `DeleteOtherFile`.

Fix what is wrong. Hand anything that is a design question to the user.

**Run with:** Opus 5.5 high. **Read list:** Established facts, Design, Phase O1 (+ its Done note).

**Done gate:** the findings are fixed or answered. If code changed, rebuild and repeat play-test (b).

**Done note:**

## Phase O2 - `overlay.py` core + skill `x2-overlay`

**Scope.**
- **`tools/overlay.py` commands:** `add` (from `march`, `file`; `--as bytecode` / `--as source` T0 only; `--server-lua`), `new`,
  `move`, `check`, `list`, `diff`, `served`, `drop`, `off` / `on`, `selftest`.
- **Supporting pieces:** the lock (shared with `build.py`), the ledger, and the refused names.
- **`tools/logs.py`:** reads `dev_overlay.log` and supports `--tag DEV_OVERLAY`.
- **Skill** `.claude/skills/x2-overlay/SKILL.md`, in the shape of `x2-kom-packed`.
- **Docs:** a `tools/README.md` row.
- **Leave out:** `promote`, `retire` and `--from june` belong to O4 and O5. Leave clear stubs that exit 2 with "not yet (O4)".

**Run with:** Sonnet 5.5 high. **Read list:** Rules, Starting context, Design ("Tool", "Promote" for the stubs' shape), Phase O2.

**Done gate:**
- `overlay.py selftest` passes.
- In scratch dirs, `check` refuses each of: BOM-less Lua, each refused-name class, base drift, an untracked file, an entry without
  a file, a name in two folders, and a name owned by another live kom.
- `add --as source` on a data217 dialog produces a file that `luasrc.gate()` accepts.
- No live kom and no `kom_contents` file is modified.

**Done note:**

## Phase O3 - Gate matrix play-test

**Scope.** One item per kind, each added with `overlay.py` and given a **visible** change. For each, the Done note records the
character/place, what to look at, the expected result, and the `served` line.
1. UI dialog Lua, BOM source, seen on reopen.
2. A startup templet Lua (restart).
3. Texture + TET, both in the overlay.
4. Texture in the overlay, TET only in the kom.
5. Texture in the kom, TET only in the overlay.
6. `.x` mesh.
7. `.y` motion.
8. `.xet`.
9. A kom sound effect `.ogg`.
10. `item.kim`: it survives the launch, loads (the `[JUNE_KIM]` line) and refreshes `ShopImageName.txt`.
11. An offline server Lua as plaintext (`--server-lua`; its loader logs "loaded N row(s)").
12. An overlay-only new file (exercises `IsValidFile`).
13. Names of 59, 60 and 64 characters: no corruption, each served.
14. The same name planted by hand in two folders: a `DUP` line, the lowest folder wins, and `check` refuses.
15. A file edited while the game runs: seen on its next load.
16. A file added while the game runs: seen only after a restart.

Then `drop` everything and confirm a run with 0 `served`.

Pick test files whose change is unmistakable, in the same spirit as the Peita button. Test-only changes never reach a kom; they
are dropped at the end.

**Run with:** Sonnet 5.5 high, then user play-tests. **Read list:** Rules, Design, Phase O3.

**Done gate:**
- every row passes, with no new KEM_ERROR against the O0 baseline;
- after the drop, 0 `served`;
- any failure becomes a fix in O1's flag (rerun that row) or a named limitation the user accepts.

**Done note:**

## Phase O4 - Promote + retire

**Scope.**
- **Commands:** `promote` and `retire` as in *Promote*, plus `tools/data/overlay_promotions.tsv`.
- **`luasrc` split:** split `luasrc convert` into produce-source and commit-to-dest. **Start from lua-source L2's committed
  `luasrc.py`.** A peer session is editing it; do not start while it has uncommitted changes from another session.
- **Pilot.** On data217 (small; T0 sources):
  1. Make a real, user-approved change to one dialog; ask what to change. If there is none, use a harmless agreed edit and revert
     it with a second promotion.
  2. Add, play-test, `promote data217` (`PACK OK`, `PROMOTE VERIFY OK`), ask, copy (`COPY OK`), `retire`.
  3. Play-test with the overlay `off`: same visible result, 0 `served`.
- **Release readiness:** `promote --all --dry-run` over whatever the overlay holds.

**Run with:** Opus 5.5 high. **Read list:** Rules, Starting context, Design ("Tool", "Promote"), Phase O4.

**Done gate:**
- the pilot's four OK lines are quoted;
- the retire md5 matches;
- the overlay-off play-test passes;
- the `promotions.tsv` row is committed;
- the dry run has no refusals.

**Done note:**

## Phase O5 - June path

**Scope.**
- **`add --from june`:** `junekom.decode` strict; stage-june placement, including new kom numbers; `--long-names`; bulk
  `--list FILE`.
- **API:** the function `adopt-june` (lua-source L3) will call to place a June `.lua` in the overlay.
- **Test:** 20 June non-lua files across 3 koms, including one new-kom-number file and one long name.
  - Each is byte-equal to `junekom verify`, and each is visibly loaded or served in play.
- **Scale check:** a bulk add of one full June kom (thousands of files). Record the startup `index_ms` and the process memory
  (Task Manager or the K2 memory line), then `drop` it.

**Run with:** Sonnet 5.5 high, then user play-test. **Read list:** Rules, Starting context, Design ("Tool"), Phase O5; `tools/kom.py`
stage-june (`:405-674`).

**Done gate:**
- the tests pass;
- scale numbers are recorded, with an acceptable bar agreed with the user if they are high;
- the overlay is empty after the drop.

**Done note:**

## Phase O6 - Workflow switch + plan rebases

**Scope.**
- **CLAUDE.md:**
  - the Workflow table: add `overlay.py` and `/x2-overlay`;
  - the packing bullet: packing happens only at close-out or release, with the ask-once-per-set rule;
  - the server-Lua rule's "Do not write a loose copy into the game directory" bullet: loose files go only into `dev_overlay`
    through the tool; the encrypt and pack happen at promote;
  - the Finish list: `overlay.py check` replaces pack;
  - a short *Dev overlay* section with the precedence and the refused names.
- **Skills:**
  - `x2-phase`: Finish and hand-off;
  - `x2-lua`: convert and edit into the overlay; pack only via promote;
  - `elstools` and `x2-kom-packed`: called by promote; legacy direct use;
  - `x2-check-logs`: the `DEV_OVERLAY` tag.
- **Lua source:** `LUA_SOURCE_PHASES.md` and the `kickoffs/lua-source/` L3 and L7 kickoffs. `adopt-june` writes into the overlay
  through O5's API.
- **Rebase `june-asset-port`.** Rewrite the kickoffs and STATUS rows:
  - P0's new `kom.py` subcommands become `overlay.py add --from june --list`;
  - P2-P4 and P6-P8 become bulk overlay adds + play-tests;
  - P1 (mount diagnostic) and new-kom-number handling move to its close-out promote;
  - P9 (music / movie / eventscene) stays `copy-loose` into `data\` (`GetPullPath` consumers).
  - Keep the plan's evidence sections as they are; change only kickoffs, STATUS and the phase scopes.
- **Memory:** new `dev-overlay-workflow`; update `kom-pack-copy-skills`, `server-lua-packing-is-users-job`, `lua-source-plan` and
  `june-asset-port-plan`.

**Run with:** Opus 5.5 high. **Read list:** Rules, Design ("After the switch"), Phase O6; the files named above, section by
section.

**Done gate:**
- a grep across CLAUDE.md, `.claude/skills/`, `kickoffs/` and STATUS.md for `x2-kom-packed`, `elstools pack`, `kom.py pack` and
  "loose copy" shows only promote-time or legacy uses;
- the Done note lists every file changed.

**Done note:**

## Phase O6-review + close-out

**Scope.**
- **Doc review.** Review O6's sweep for contradictions and stale instructions. Read each changed file's changed sections, not
  whole files.
- **Close-out promote.** This plan's own promote: everything its phases put in the overlay should already be dropped or
  promoted, so `promote --plan dev-overlay --dry-run` should list nothing.
- **Memory.** Finalise the memory entries.

**Run with:** Opus 5.5 high. **Read list:** Rules, Phase O6 (+ Done note).

**Done gate:**
- the findings are fixed;
- the dry run is empty;
- the STATUS block reads done.

**Done note:**

## Effects on other plans
- **lua-source:**
  - L2 is unchanged.
  - L3's `adopt-june` output goes to the overlay (O6 rewrites the kickoff).
  - L7's convention sweep includes the overlay.
  - O4 starts after L2's `luasrc.py` is committed.
- **june-asset-port:** rebased in O6; it does not start before O6.
- **Every plan after O6:** phases finish with `overlay.py check`. Only close-out phases promote.

## Play-test hand-off
Each phase's hand-off follows the standing rule: copy-ready, with the exact character, place, steps, expected result and failure
look. It also names the overlay entries in play, and the `served` lines `python tools/logs.py --tag DEV_OVERLAY` should show.
