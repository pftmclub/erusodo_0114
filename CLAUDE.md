# CLAUDE.md

Rules for working in this repo. Background, the toolchain and architecture reference, and the history behind the rules are in
**`CLAUDE_REFERENCE.md`**: read one section with `python tools/docsection.py CLAUDE_REFERENCE.md "<heading>"`, never the whole file.

## What this is

Full source for **Elsword ("ProjectX2")** by KOG: the DirectX 9 client (`KTDXLIB/` engine, `X2Lib/` gameplay, `X2/` exe shell,
`X2ServerProtocol/` networking), five servers (`KncWX2Server/`) and content tools. `EU_CN_US` trunk, 2014-era, imported into git.
The client is a **March 2014 snapshot** (`dd63297`, 2026-09-09) being ported to **June 2017** parity, and it runs **offline only**
(`SERV_IRUHADEV_OFFLINE`: an in-process server emulator plus a SQLite save). `MODS.md` lists every local change and how to revert it.
History docs, not instructions: `MARCH_2014_MIGRATION.md` (closed 2026-10-16; technique reference), `VS2003_to_VS2010_Port_Guide.md`.

**US live build only.** The ~50 other region/environment configurations change publisher, billing, auth and anti-cheat; ignore them
unless asked.

## Rule: one branch, `main` (user, 2026-10-16)

All work happens on `main` in this Trunk, for every plan, phase and review. Do not create a branch or a worktree, do not `git switch` or
`git checkout` another branch, and merge nothing. Other sessions share this tree and this branch, so commit with explicit pathspecs. The old
`mods/*` and `feat/*` branches are history. Never build or deploy from them: they lack june-item-kim K2 and silently misread the live
`item.kim`. Older plan text that names a `mods/*` branch means `main`.

## Rule: shared builds - one request each, and a tree that compiles (user, 2026-10-18)

Every session builds the same tree through one lock; a full build takes about 11 minutes.

1. **Full build only when the phase hands off a play-test (user, 2026-11-09).** A phase or review with no play-test (inert transcription,
   no-code review) skips `build.py`: it proves compilation with a one-file probe build (0 errors), and the Done note says "full build
   skipped: no play-test". The next phase that hands off a play-test builds the whole tree.
   **Build once, after your last edit.** `python tools/build.py --deploy` in the background. No second run while one of yours waits, no
   retry loop, no `--no-coalesce` without a reason. `COALESCED` then `DEPLOY OK` is a pass. A run covers only what was on disk when it
   started. **Exception (user, 2026-11-03):** when a phase's or review's only code change is commenting out a `_DEBUG` `#define`, start the
   build but don't wait for it: commit and hand off at once. The Done note says "not awaited" in place of the `DEPLOY OK` line.
2. **Never stop another session's build or delete `build.lock`.** Only the user orders builds stopped; then kill by PID (never by image
   name), waiting runs first, and the next build uses `--pch force`. Stale locks clear themselves.
3. **Keep the tree compiling.** Before saving a new `Always.h` define or a dependency guard (`#error X needs Y`), confirm `Y` is defined at
   that line with `cl /EP`, not by reading `#ifdef`s. If your edit breaks the build, fixing or backing it out comes first.
4. **A build that fails in someone else's edit is not yours to fix.** Don't edit that file or keep re-running. Report the error line and its
   owner (the flag's `Always.h` comment names the phase), wait, and re-run once it's fixed.

## Rule: exe string checks use `tools/exestr.py`, never a throwaway script (user, 2026-10-28)

To show a string is (or, with `--absent`, is not) in the built or deployed exe, run `python tools/exestr.py "<string>" ...` (skill
`x2-exe-strings`; `--form wide` for `L"..."`). Paste its `EXESTR OK` line beside `DEPLOY OK` in the Done note. Missing a feature: extend
`exestr.py`.

## Rule: play-test items come from `/additem`, not the shop (user, 2026-10-23)

A play-test hand-off gives the dev-console line `/additem <itemID> [qty]` (auth level 3, flag `SERV_IRUHADEV_ADMIN_CHEATS`); `/qdone
<questID>` finishes a quest. Never insert into a shop list or point at `/b` (offline `/b` charges ED and refuses unpriced items). Answered
by `Handlers_Quest.cpp`, log line `[ADMIN_CHEAT]`.

## Rule: no anonymous namespace; use the studio's named `_CONST_<NAME>_` namespaces (user, 2026-10-26)

File-local constants and helpers in client source go in a named namespace `_CONST_<CLASS>_<phase>_` (like `_CONST_EVE_`,
`_CONST_GUUSER_`) followed by `using namespace <name>;`. Never `namespace { ... }`. One block per phase. No `JUNE` in the name (amended
2026-10-28; e.g. `_CONST_EVE_CH4C_`). Older anonymous namespaces elsewhere are not rewritten unless you touch them.

## Rule: no `June` prefix; June's body goes into March's own function (user, 2026-10-28)

The codebase is updated to June in place, the studio's own pattern (March's `GetEqippedSkillMemo` sits whole inside `#ifdef SERV_SKILL_NOTE`).
- **March has the function:** June's whole body goes inside it: `#ifdef SERV_IRUHADEV_JUNE_<X>` June's body `#else` March's body
  `#endif SERV_IRUHADEV_JUNE_<X>`. Never a `June_X` / `JuneX` copy beside `X`.
- **March lacks it:** create it named by the class's pattern, never with `June` in it: `<PREFIX>_<STATE>_StateStart` / `_FrameMove` /
  `_EventProcess`, `Get` / `Set` / `Is` / `Can`, `CX2*` classes, Hungarian members (`m_i`, `m_f`, `m_b`, `m_vec`, `m_map`). Same for
  members, locals, constants, classes, files, namespaces and Lua-bound names. Flags keep `SERV_IRUHADEV_JUNE_*`.
- **A name says what the thing is** (`bOberonGuardMemoEquipped`, `fElectronBallRangeRate`): never a Ghidra address, a phase tag or `June`.
  The June address goes in the one-line comment.
- **June code calls a March function by name only if June's namesake does the same thing.** Otherwise replace March's definition as above;
  its unported callers follow June. If that would crash an unported class, guard it inside the June body with a one-line
  `stand-in: <class> not ported` comment.
- **Inert transcription (`_CORE` before `_LIVE`):** a June-only function compiles under `_CORE` under its plain name; a body inside an
  existing March function goes under `_LIVE`, and the phase proves it compiles with a one-file probe build that defines `_LIVE`.
- **The gate and the table (user, 2026-10-30; plan `june-march-divergence`, closed 2026-10-30).** `python tools/junediverge.py --list`
  before transcribing; `--check <changed files>` must print `RESULT: PASS` (a review runs `--check-all`). A phase or review that finds a
  March function whose June namesake differs adds its `tools/data/june_march_divergent.tsv` row **and** the replacement in the same commit.
  If the replacement needs its own phase, the row's `source` names where it went and `tools/data/june_diverge_allow.tsv` carries the same
  reason, until the port lands. Never allow-list a hit just to pass.
- The ~1,500 June-prefixed names from before 2026-10-28 were merged or renamed by that plan. New code adds none.

## Rule: no hardcoded enum literals (user, 2026-10-26)

An id that has a name in an Enum (June's `Enum.lua` or a C++ enum) is never written as a number in new or touched code. If C++ lacks the
member, register June's name and value under a `SERV_IRUHADEV_` flag. A value with no Enum entry stays a number with a one-line comment.
Exception: a data table whose rows are ids (a `.tsv`).
- Authority: `june_src_fixed/data036_kom/Enum.src.lua`, then the live `kom_contents/data036_kom/Enum.lua`; never a spreadsheet or an invented
  name. Check the value, not only the name.
- Shape: explicit `= value` with June's name appended inside the existing enum under the flag (`SERV_IRUHADEV_JUNE_ENUM_NAMES` in
  `X2SkillTree.h`, `X2Unit.h`, `X2BuffFactor.h`, `X2UnitManager.h`, `X2DamageManager.h`). Never renumber or rename a member; when March's
  and June's names share a value, use March's.
- Item, string and net-error ids stay numbers. Dungeon ids are named whenever June's `DungeonEnum.lua` or `SEnum::DUNGEON_ID` has them
  (register in `DungeonEnum.h` under `JUNE_ENUM_NAMES`); `enumscan.py` skips them, so check by hand. `python tools/enumscan.py --check
  <files>` finds the rest.

## Rule: a code comment is one short line (user, 2026-10-20)

A source comment says one thing in one line, and only what the code cannot show: the June address, a stand-in, a quirk kept on purpose, or
why June differs from March. No comment blocks or tables, no retelling of the code. Detail goes in the Done note, `funcmap.tsv` or
`JUNE_BUGS.md`. Old comments are not rewritten.
**Phase tag (user, 2026-11-09):** in a char port, every edited or new code block (one `#ifdef` block, one new function, one hunk) carries
its phase in a one-line comment so a later error traces to its phase: `// char-rena CH1n`. If the block already has a one-line comment,
put the tag at its start (`// char-rena CH1n: June 32ca9dc0`); never add a second line. The tag is a comment only, never part of a name.
Blocks written before this date are not retagged.

## Workflow automation: use `tools/` and the `x2-*` skills, not hand-typed commands

Every repeated step of the phase loop is a validated script (`tools/README.md`). Never hand-type msbuild, write a throwaway byte-patch or
unifdef script, or read a plan doc "in full".

| Step | Command | Skill |
|---|---|---|
| What's running / next / parallel | `STATUS.md` (open work only; closed plans in `STATUS_ARCHIVE.md`, grep it) | `/x2-phase status` |
| Start a phase or review | `kickoffs/<plan>/<phase>.md` | `/x2-phase <plan> <phase> [review]` |
| Read part of a doc | `python tools/docsection.py <doc> "<heading>" ...` | x2-phase |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply <spec>` | x2-byte-edit |
| Flag hygiene (insert-only, `#else` intact, flag-off = base) | `python tools/flagcheck.py <FLAG> [--range A..B / --worktree / --base REV]` | x2-byte-edit |
| Build and deploy | `python tools/build.py --deploy` (background) | x2-build-deploy |
| Strings in the exe (or absent) | `python tools/exestr.py "<string>" ... [--form wide] [--absent]` | x2-exe-strings |
| Play-test logs | `python tools/logs.py --tag <TAG>` | x2-check-logs |
| Save DB / items / characters | `tools/elsdb.py`, `tools/kim.py`, `tools/createchar.py` | x2-db, x2-kim, x2-create-char |
| Pack a March kom | `python tools/kom.py pack dataNNN` | `/elstools pack dataNNN` |
| Deploy a packed kom | `python tools/kom.py copy dataNNN` | `/x2-kom-packed copy dataNNN` |
| Convert / edit / prove a March `.lua` as BOM source | `python tools/luasrc.py convert dataNNN X.lua` (and `restore`, `refresh`, `ledger`, `check`, `t2`) | `/x2-lua` |
| Adopt June `.lua` / the core list | `python tools/luasrc.py adopt-june ...` ; `luasrc.py core --check` | `/x2-adopt-june` |

- **You pack koms** (`/elstools pack`) **and copy them** (`/x2-kom-packed copy`) **without asking (user, 2026-11-10;** it was ask-first
  before). The tool backs the live kom up and refuses while the game runs: then ask the user to close it, never kill it. Plan docs and
  kickoffs that say "ask first" before a copy mean "copy now". Report `PACK OK` / `COPY OK` in the hand-off. Exception: server-only Lua stays
  the user's to pack (rule below).
- **Every phase and review finishes the same way** (x2-phase "Finish"): `flagcheck --worktree` passes; `python tools/junediverge.py --check
  <changed files>` prints `RESULT: PASS`; `build.py --deploy` with `DEPLOY OK` (play-test phases only, see the shared-builds rule)
  in the Done note; a Done note of at most ~60 lines; the `STATUS.md` row updated in the same commit (one line, evidence in the note); the
  next kickoff written if missing; a commit with explicit pathspecs; a copy-ready play-test hand-off. Never launch the game yourself.
- **New plans** follow x2-phase's `new-plan` shape: kickoffs that name sections, plus `STATUS.md` rows.
- **Encoding guard:** `.claude/hooks/encoding_guard.py` blocks Edit/Write on non-UTF-8 files. CP949 files are edited with `bytepatch.py` only.

## Rule: every client change goes behind a `SERV_IRUHADEV_` flag

**Every edit to `X2/`, `X2Lib/`, `KTDXLIB/` is wrapped in a `#define` with the `SERV_IRUHADEV_` prefix**, one-line fixes included. Define
it at the end of **`KTDXLIB/Always.h`** (reaches all three client projects through `KTDX.h`), with the house block:

```c
//////////////////////////////////////////////////////////////////////////
// Author: Iruha
// Date: 2026-08-24
// Description: <what this change does>
#define SERV_IRUHADEV_<SHORT_NAME>
//////////////////////////////////////////////////////////////////////////
```

At each site keep the original reachable when behaviour is replaced:
`#ifdef SERV_IRUHADEV_<X>` new `#else` original `#endif SERV_IRUHADEV_<X>` (trailing-token `#endif` is house style). One flag per
logical change, not per file. New `Always.h` entries are ASCII (the old ones are CP949 Korean).

Not in `KncWX2Server/Common/ServerDefine.h` (server/shared file, invisible to `KTDXLIB`), and not in `KTDXLIB/InHouse*.h` (the studio's
per-developer files; they compile in under `_IN_HOUSE_` but a flag there is unfindable).

**Offline-only (user, 2026-10-05): server sync is not a concern.** Shared-header and packet changes (`KncWX2Server/Common/` structs,
Put/Get, enums) are made client-side behind the `Always.h` flag; no server rebuild, no server flag, and **never ask the user about server
desync**. The emulator shares the exe, so keep its handlers and the `X2OfflineDB` schema consistent with any Put/Get change.
**`CommonPacket.h` exception:** it is reached before `KTDX.h` includes `Always.h`, so a flag guarding code there is also `#define`d at the
end of `ServerDefine.h`, both toggled together, and added to `SECOND_HOMES` in `tools/flagcheck.py` (first one:
`SERV_IRUHADEV_JUNE_BUFF_FACTOR_WIRE`). Headers included after `KTDX.h` (`Enum.h`, `DungeonEnum.h`) are unaffected.

## Rule: server-side Lua the client needs is the user's to pack — never band-aid around it

Some data existed only on the server (stat tables, drop tables, field tuning). When client code needs such a file, it must be XOR-encrypted
and packed into `data036.kom`, and **the user does that step**. Don't wait for it.
1. **Implement the whole feature in one pass:** loader, consumer and degrade path, even if a plan frames "refuse and log" as a stopping point.
2. **Load it the shipped way:** `GetMassFileManager()->LoadDataFile( name )`, then `GetLuaBinder()->DoMemory( ... )`, with a logged
   `DoMemoryNotEncript( ... )` fallback. Copy `X2Lib/Offline/X2OfflineStatTable.cpp` / `X2OfflineDropTable.cpp` / `X2OfflineBattleField.cpp`.
3. **Degrade visibly:** file missing -> feature off, log which file and what to do.
4. **Name the file when reporting done** (`KncWX2Server/ServerResource/US/<name>.lua`, needs XOR + packing into `data036.kom`), and confirm
   from the loader's "loaded N row(s)" log line if it is already packed.

Never write a loose copy into the game directory (a loose file beats the kom under this build, since `MASS_FILE_FIRST` is undefined, and it
masks a failed repack). Never hardcode a table, invent a curve or approximate a function for an unpacked file. Never edit studio `.lua` to
simplify code; tuning gameplay in data is fine, keeping the original value and restore note in comments and the file's BOM/CRLF exact. The
one allowed fallback is a constant read out of the repo, cited and logged (`BattleFieldServerData.lua`'s respawn window). Full text and
history: `CLAUDE_REFERENCE.md`, "Background: the server-Lua rule".

## Rule: never invent a new `.lua` file (user, 2026-10-09)

No new `.lua` files: no side files, helper tables or stand-ins. The only exception is a server Lua the client needs packed (rule above). If
data you need sits in an existing Lua you can't reach safely, say what blocks it; if it feels beyond the current model, say **"this needs a
higher model"** and stop. Editing an existing Lua that has a `.src.lua`: ask first. Exception: a port that must edit a June `.lua` adds it
to the core list and edits it without asking.

## Rule: core client `.lua` is BOM source, the rest is June's bytecode (user, 2026-10-09; `LUA_SOURCE_PHASES.md`, skill `x2-lua`)

The engine loads XOR-encrypted **BOM + UTF-8 text** as readily as bytecode, so a March `.lua` that must change is stored as source. Use
`tools/luasrc.py` and `kom.py`; never decompile or compile by hand.
1. **Proof tiers.** Only T0 (byte-identical recompile) and T1 (identical per prototype; constants by value, table pairs as a set) convert
   automatically. T2 (behaviour-equal) needs an Opus audit and the user's yes. T3 ships the bytecode. Reference: the shipped March bytecode
   (June files: `ljconv(June bytecode)`).
2. **Source only for the core list** (`tools/data/lua_core_list.tsv`, `luasrc.py core --check`): files kept merged because March's C++
   and June's data disagree (`Enum`, `DungeonEnum`, `DamageEffect`, `EffectSet`, `DLG_Map_*`, `NPCTemplet`, `DungeonData`, ...), or that a
   port must edit. Everything else from June arrives by `luasrc.py adopt-june` as June's `ljconv` bytecode (no decompile, no edit; refuses
   core-list names). Core files take `adopt-june --source` (decompile, T1, ledger row; on a T1 fail nothing is staged, row
   `blocked: <reason>`). A port that must edit a June file runs `luasrc.py core --add <kom> X.lua --origin june --why "edit: <phase> <what>"`.
3. **Editing a March `.lua` starts with `luasrc.py convert`** (sources tried: sibling `X.src.lua`, studio original in `ClientScript\`, a T1
   decompile). After it, `X.lua` is BOM source; `kom.py compile` folds a sidecar `X.src.lua` in.
4. **Conversion changes format only;** content changes are the port's own diff, after conversion.
5. **The pack gate enforces it:** every `.lua` is BOM text that parses, or `1B 4B 4C 81` bytecode, or a ledger `EXC`. `pack` refuses
   BOM-less text, June `0x82` bytecode and parse errors. Manifest-`"0"` `.lua` (server Lua) only warns.
6. **Bookkeeping:** originals snapshotted to `24965799\kom_contents_bytecode\`; ledger `tools/data/lua_source_ledger.tsv` must pass
   `luasrc.py ledger --check`; `luasrc.py refresh` after a deliberate text edit. `june_src_fixed/` is reading material.

## Rule: a plan keeps to its subject; drift gets its own plan (user, 2026-10-19)

A June branch a phase finds becomes a phase in the current plan only if it is the plan's subject or the game is unplayable without it
(crash, softlock, blocked progression, wrong combat numbers on a reachable path). Anything else is identified (June addresses, March
counterpart, offline reach, related branches) and gets its own plan in the same session: a bullet and brief in the owning stage, or a new
backlog stage. Deferred, never dropped. Priority: Stage 4 quests, then character revamp (5), transcendence (6), remaining characters (7),
new characters (8), UI sweep (9), then scattered systems unless one blocks play. Details: `JUNE_PORT_BACKLOG_ORDER.md`, "Standing rules",
*Scope*. "No partial updates" means a ported function's whole body, not its callees.

## Rule: a March asset with the same name as a June asset is replaced by June's (user, 2026-10-19)

Same file name in June and March (texture, `.tet`, mesh, sound, dialog or data `.lua`, in a kom or loose in `data\`): **June's replaces
March's, byte-exact, through the tools** (`kom.py stage-june`, `luasrc.py adopt-june`). No merge, no hand edit, no asking. If a March reader
then breaks, the fix is code (port June's reader) or a routed follow-up.
- **Exception: core-list `.lua`** stays merged BOM source; a phase adds June's missing members to it.
- **Shared Lua in a character port (user, 2026-10-20):** a multi-character `.lua` (`UnitTemplet.lua`, ...) gets only the ported character's
  rows from June and becomes core-list source.
- Record each replace in the Done note and `MODS.md`; the tools back up the replaced file.
- **Assume March does not have the asset (user, 2026-10-27).** Before debugging code that uses an asset, check it by name: March lacks it ->
  stage June's; March has it -> replace with June's anyway.

## Rule: `DataBase/` and `ScriptData/` are a stale snapshot — the live DB is the source of record

The user has the original SQL Server databases (`Account`, `Game01`). `DataBase/` and `ScriptData/` are an old checkout and have been proven
wrong (`gup_create_unit` here inserts no items, the live one inserts ten; `ItemTemplet.xlsx` lacks every live beginner-gear id). **When
behaviour depends on a stored procedure or server data table, ask the user to pull it from the live DB first**, naming it and giving the
query:

```sql
SELECT OBJECT_NAME(object_id) AS proc_name, definition
FROM   sys.sql_modules WHERE OBJECT_NAME(object_id) IN ( 'gup_...', … );
SELECT name FROM sys.procedures WHERE name LIKE 'gup_%' ORDER BY name;
```

A procedure missing from `DataBase/` is not evidence it does nothing. Internally consistent local copies are not corroboration; only
genuinely independent sources count. Commented-out live code is read for structure, never values. Transcribe live data verbatim, don't
tidy it. Ask for dumps as plain text in the conversation, and keep server names, linked servers and logins out of anything committed or
logged. The two failures behind this rule: `CLAUDE_REFERENCE.md`, "Background: the live-DB rule".

## Build facts that matter every session

- **Client = `US_SERVICE`** (VS2010 `v100`, Win32, Unicode; optimized clone of `US_INTERNAL`, same defines). `US_INTERNAL` only if asked.
  `python tools/build.py` encodes the order, paths, `SolutionDir` and PCH handling; the manual recipe is in `CLAUDE_REFERENCE.md`, "Toolchains".
- **The servers cannot be built here** (VS2003 not installed; the VS2010 server port was overwritten by the March sources).
- **Never build the client from `X2Project_2010.sln`** (it never runs `X2_2010.vcxproj`).
- **`_IN_HOUSE_` is defined**, so the seven `InHouse*.h` files compile in. Resolve any flag's real state with `cl /EP` on a probe that includes
  `KTDX.h`, never by reading nested `#ifdef`s. Notably: `MASS_FILE_FIRST`, `WORLD_TOOL`, `_SERVICE_` are **not** defined;
  `AUTO_LOGIN_IN_HOUSE`, `NO_GAMEGUARD`, `NOT_USE_UDP_CHECK_INHOUSE` and the in-house cheat/dev-UI surface **are**.
- **A stale PCH silently drops header edits** (`Always.h`, `X2Define.h`): if a change seems to do nothing, prove it compiled with a
  `#pragma message` inside the guard. `build.py --pch force` rebuilds it.
- No test suite. Verification is the play-test plus the logs.

## Offline mode: what not to break

`SERV_IRUHADEV_OFFLINE`: the client plays with no servers; an emulator answers their packets and state persists to `els_db.sql` in the game
directory. Code: `X2Lib/Offline/` (`X2OfflineServer` dispatch, `Handlers_*.cpp`, `X2OfflineDB` with a forward-only migration ladder, one
module per reimplemented subsystem). Design history: `OFFLINE_MODE_PLAN.md`; reference: `CLAUDE_REFERENCE.md`, "Offline mode".
- **The flag lives in two places, toggled together:** `KTDXLIB/Always.h` and `X2ServerProtocol_2010.vcxproj`'s `X2TOOL|Win32` defines.
- **Seams:** `X2ServerProtocol/Socket/Session.cpp` (outbound `KEvent` -> `g_pX2OfflineHook->OnClientSend()`, answered via
  `KSession::QueueingEvent()`), `X2Data.cpp` `ResetServerProtocol()`, and a few revived client paths.
- **One packet is one transaction:** handlers run under `m_csDispatch` inside a SQLite savepoint; code in play uses
  `Begin()`/`Commit()`/`Rollback()`, never a literal `BEGIN`.
- **`sizeof(KSession)` differs between `X2Lib` and `X2ServerProtocol`:** never read a `KActorProxy`/`KUserProxy` member from `X2Lib`.
- **Logs** (game dir, flushed per line): `offline_server.log`, `offline_packets.log` (`HANDLED` / `IGNORED` / `UNHANDLED` / `EXCEPTION`),
  `ErrorLog.txt`. Read them with `tools/logs.py`. A temporary diagnostic is `CX2OfflineLog::Server( L"..." )` behind its own
  `SERV_IRUHADEV_*_DEBUG` flag, grep-able tag, throttled to about once a second.

## Deploy and the game directory

Game dir: `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data`. `build.py --deploy` copies `X2/US_SERVICE/x2.exe` to
`X2_offline.exe` there and verifies it; it refuses while the game runs. The user launches through `start_offline.bat` (sets the working
directory). **Never launch the game yourself; ask.** **`els_db.sql` (+ `-wal`/`-shm`) is the only copy of the user's characters:** never
delete it, back up before editing (`tools/elsdb.py`), and treat schema changes as irreversible. Defender may quarantine a fresh build
(Bearfoos false positive; folder exclusions are set).

## Conventions

- **Many sources are CP949 (Korean comments), not UTF-8.** Never re-encode them. Edit with `tools/bytepatch.py`, verify with `flagcheck.py`;
  the Edit tool can silently re-encode a whole file. After touching a non-ASCII file, check `file <path>` and `git diff --stat`.
- Non-trivial edits use the change-block markers `//{{ author : date // description` ... `//}}`.
- Prefixes: `CKTDX*` / `KTDG*` engine, `CX2*` gameplay, `K*` server and KNCSDK. Wide strings throughout; `#pragma pack(push,1)` around packets.
- Server code (`KncWX2Server/`) stays C++98-clean (no `auto`, range-for, `nullptr`, rvalue refs).
- June client `.lua` in source form: read `june_src_fixed/data###_kom/<Name>.src.lua` (fixed decompiler), not a fresh `elstools`/`ljd` run.
- `march2014/update/*/_manifest.json`: CRLF, 2-space indent, no trailing newline (`CLAUDE_REFERENCE.md`, "`march2014/update/` manifests").

## Cautions

- Scope searches to `X2Lib`, `KTDXLIB`, `KncWX2Server`, `X2ServerProtocol`; `Libs/` has ~22,000 vendored files.
- `.dsn` files hold real hostnames and plaintext credentials: never copy them into new files, logs or anything published.
- `*.sdf`, `*.ncb`, `ipch/` are IntelliSense caches: never edit.
