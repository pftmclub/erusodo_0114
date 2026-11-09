# June coverage census - how much of the June 2017 binary is left to port

Plan key `june-census` - branch `main` - created 2026-11-09 (user question: "I don't know how long porting ALL of 2017 will take; the
backlog surely doesn't cover the whole binary"). Read-only: no client code, no flag, no build, no game launch.

## Context

`JUNE_PORT_BACKLOG_ORDER.md` lists 37 stages, but they are the systems found so far, not a measurement of the binary. The June exe
(`unpacked_x2.exe`, 55,555 functions, imagebase `0x31aa0000`, Ghidra project `els_jun_2017`) has `june2017_re/funcmap.tsv` rows for
about 8,800 of them. Nobody has counted what the other ~46,000 are, which of them are the studio's own gameplay code, and which stages
(if any) already own them.

This plan produces one table and one estimate:
1. every June function, tagged with its source file / class where the binary names it, and a status (mapped, March has it, owned by a stage);
2. every unowned group of gameplay functions, either excluded with a reason or turned into a new backlog stage brief;
3. a size range for the remaining work, calibrated on the pace of the closed plans. It is a range with its assumptions, never a date.

The June 2020 exe (`unpacked_x2020.exe`, same Ghidra project, analysed) is a reference only and is **not** part of this census.

## Rules

- Read-only on the game, the source tree and the Ghidra programs: no renames, no comments, no retypes in either program. Only the June
  program is open in the CodeBrowser; V0 checks the imagebase (`0x31aa0000`) and the exe's size (43,801,600 B) before reading anything.
- Outputs go under `june2017_re/census/` (tsv) plus the tool `tools/junebinary.py` (V1). Nothing here ships in a build.
- A count is only reported with the file it came from; a classification with no evidence is `unknown`, never guessed.
- Scope rule (CLAUDE.md, "a plan keeps to its subject"): this plan finds and sizes work, it never ports any. A found system becomes a
  backlog stage brief (`kickoffs/june-backlog/stageN-plan.md`) in V2, not a phase here.
- Delegation: V0 and V1 are Sonnet phases and do the work themselves. V2 is Opus and delegates its reads and counts to Sonnet subagents
  (x2-phase "When the phase is Opus"); it keeps the exclusion decisions and the estimate.
- Finish for every phase: no `flagcheck`, `junediverge` or build (no code changed). Update the `STATUS.md` row in the same commit as the
  Done note, commit with explicit pathspecs, write the next kickoff if missing.

## Starting context

- Ghidra: skill `ghidra` (curl to the GhidraMCP server, `--noproxy '*'` quoted). Listings are paginated (`methods`, `strings`, `searchFunctions`,
  `xrefs`); try a large `limit` first and fall back to pages. If the HTTP route is too slow for 55k functions, V0 says so and hands the user a
  Ghidra script to run once (a decision for the user, with a recommendation); never loop thousands of calls blindly.
- `june2017_re/README.md` (binary facts), `june2017_re/funcmap.tsv` (columns `june_va`, `march_symbol`, `status`, `evidence`).
- The studio's own code carries `__FILEW__` path strings (`X2EffectSet.cpp`) and RTTI class names: those are the grouping key. Library code
  (CRT, STL, Lua / LuaJIT, FreeType, Scaleform GFx, zlib, DirectX wrappers) has none, or paths outside the studio tree.
- March side: `X2Lib/`, `KTDXLIB/`, `X2/` sources (scope searches to these, never `Libs/`); `MODS.md` "June flag index" for what is already
  ported under `SERV_IRUHADEV_JUNE_*`; `tools/data/june_march_divergent.tsv`.
- Backlog side: `JUNE_PORT_BACKLOG_ORDER.md` stage list, `STATUS.md`, `STATUS_ARCHIVE.md` (grep by plan key).

## Phase V0 - Function inventory

**Run with:** Sonnet 5.5 / medium. **Depends:** - (runs alongside anything; read-only).

1. Verify the program (imagebase, exe size). Stop if it is not the June 2017 exe.
2. Export every function (`june_va`, Ghidra name) to `june2017_re/census/functions.tsv`; sizes from the gap to the next function start
   (say so in the header, it is approximate).
3. Attribute functions to source files: take the `strings` hits for `.cpp` / `.h` paths, follow their xrefs to the functions that use them
   (an `__FILEW__` use marks the function's file), and take class names from RTTI / namespaces. Columns `source_file`, `class`, `evidence`.
4. Write `june2017_re/census/V0_README.md` (under 40 lines): the counts (total, with source file, with class, with neither), how long the
   export took, and the method's blind spots.

**Done gate:** the function count equals Ghidra's own count for the June program; `functions.tsv` has no duplicate `june_va`; the share of
functions with a source file or class is stated. No row is invented for a function the binary does not name.

**Done note (V0, 2026-11-09, Sonnet 5.5):** `june2017_re/census/functions.tsv` (55,556 rows, no duplicate `june_va`; `list_functions` = `methods`
= 55,556, so the 55,555 above was one short), `V0_README.md`, `v0_export.py` (reproducible, 10 s over HTTP, no Ghidra script needed).
Program verified first (imagebase `0x31aa0000`, exe 43,801,600 B). Read-only; no code, flag, build or log tag.
- Attribution: 358 functions with a source file (54 files), 890 with a class (87 names), 972 with either = **1.75%**; 98.25% have neither.
- The low share is the method, not the binary: Ghidra holds 2 `vftable` labels and 1,113 `.?A` names (nearly all `lua_tinker`), and
  only ~100 `__FILEW__` paths exist, so classes are known only where a log string names them. Top "classes" (`ClassTraits`, `Instances`,
  `KSession`) and `Session.h` / `libxml` files are library code. Studio files hit: 56 functions under `X2Lib` / `KTDXLIB` / `X2`.
- **For V1:** do not read the 98% as library code. Attribute by `funcmap.tsv` joins, call-graph neighbours of attributed functions,
  link-order address blocks and the March function list; a function with none stays `unknown`.
- Open questions for the user: none.

## Phase V1 - Bucket and coverage

**Run with:** Sonnet 5.5 / medium. **Depends:** V0.

1. Write `tools/junebinary.py` (documented in `tools/README.md`): joins `functions.tsv` with `funcmap.tsv` (by `june_va`), with the March
   source tree (does a file or class of that name exist under `X2Lib/`, `KTDXLIB/`, `X2/`), and with the June flag index in `MODS.md`.
2. Per function: `bucket` = `studio` (source file or class under the studio's tree), `library` (known third-party paths / names),
   `crt-stl` (compiler or runtime), `unknown`. Per studio function: `mapped` (funcmap row), `march_has` (a March function of that name
   exists), `june_guarded` (that March function sits inside a `SERV_IRUHADEV_JUNE_*` block; best effort, measured on a 50-row hand-checked
   sample and the precision stated), `owner_stage` (a backlog stage or plan names the file, class or address; text match against the backlog
   and the kickoffs).
3. Output `june2017_re/census/coverage.tsv` (per function) and `coverage_by_file.tsv` (per source file / class: functions, mapped, march_has,
   june_guarded, owner stage, top 5 unowned functions by size).
4. A `RESULT:` line from `junebinary.py --check` that fails if the buckets do not add up to the total.

**Done gate:** `--check` prints `RESULT: PASS`; the by-file table is sorted by unowned studio functions; the Done note (about 40 lines)
gives the totals per bucket and per status, the sample precision for `june_guarded`, and the ten biggest unowned groups.

**Done note (V1, 2026-11-09, Sonnet 5.5):** tool is `tools/junebinary.py` (not `junecensus.py`: that file is the D0a identifier census of
`june-march-divergence`; the plan, kickoffs and README now say `junebinary.py`). Outputs `june2017_re/census/coverage.tsv` (55,556 rows) and
`coverage_by_file.tsv` (341 groups, sorted by unowned). `python tools/junebinary.py --check` -> `RESULT: PASS (55556 functions)`. 10 s, no code changed.
- **Buckets:** studio 29,511 (53.1%) / library 9,126 (16.4%) / crt-stl 143 (0.3%) / unknown 16,776 (30.2%).
- **Basis:** studio = funcmap 6,254 + string 285 + **block 22,972**; library = string 657 + name 1,363 + block 7,106; crt-stl = funcmap 70 + name 73.
  `block` = no evidence of its own, but the nearest evidenced function on each side has the same bucket within 300 functions. Hold-out test
  (`--holdout`, hide each evidenced function, predict it from its neighbours): studio 99.8% (6,370 / 6,383), library 98.7%; class-by-block 92.3%.
  crt-stl by block scored 42% so it is never inferred (those rows are `unknown`).
- **Studio status (29,511):** mapped (funcmap row) 6,254; the other 23,257 have no name, so `march_has` cannot be asked. March has the function
  for 3,903 of the mapped rows; **june_guarded 1,675**. Only 9,471 studio functions have a class (4,300 of them by block): **20,040 studio
  functions are `(unattributed studio)`**, the binary gives no class or file for them.
- **Owner:** address named in a stage / plan doc / kickoff 7,580, class named in a stage 2,354, **unowned 19,577 functions (12.2 MB)**, of which
  17,503 sit in the unattributed group.
- **june_guarded sample (50 random rows, seed 7, `--sample 50`):** each checked in the March source: 38 have a `SERV_IRUHADEV_JUNE_*` block inside the
  body, 9 sit entirely inside one, 3 hold one in a body my first check missed (`SetKUnitInfo` 4390, `EveElectraEffectData` 34715,
  `FollowLine` 8222); `CanUseQuickSlotItemBeforeStart` has 2 flag lines. Precision 50 / 50 = 100% as defined ("definition holds or sits in a
  JUNE block"). It is a lower bound for coverage: header-inline bodies count as not guarded.
- **Ten biggest unowned groups (functions, bytes):** unattributed studio 17,503 (8.9 MB); CX2GUEL 218 (194 kB); CX2UIInventory 166 (135 kB); CKTDGParticleSystem 104; CX2TFieldGame 72; CX2GUAra 62; CKTDGUIDialog 62; CX2SocketItem 60;
  CX2PartyUI 55; CKTDGUIIMEEditBox 54; CX2CustomEventManager 52 (61 kB).
- **For V2:** the real gap is the 17.5k unattributed studio functions and the 16.8k `unknown`. The class owner column is by name in a stage
  and is weak for shared classes (CX2GUUser); the address owner is strong. V2 should split the unattributed block by address range
  (`coverage.tsv` sorted by `june_va`), not by class.

## Phase V2 - Exclusions, new stages, estimate (Opus)

**Run with:** Opus 5.5 / high. **Depends:** V1.

1. **Exclusion list (Opus's judgment).** Go through every unowned studio group and decide one of: port (it is gameplay the offline client
   needs), exclude (anti-cheat, billing, server-only, publisher login, telemetry, 2020-only leftovers in a 2017 file, dead code), or park
   (needs a user decision). Write `june2017_re/census/exclusions.tsv` (`group`, `functions`, `decision`, `reason`). Anything that is not
   obvious goes to the user as one question with a recommendation (CLAUDE.md "Skip non-important open questions": only decisions that change scope).
2. **New stages.** For each `port` group not owned by a stage, add a stage to `JUNE_PORT_BACKLOG_ORDER.md` ("Stage details" and the order
   table) and a brief `kickoffs/june-backlog/stageN-plan.md` with a `june-backlog` STATUS row. Group related ones; do not make one stage per file.
3. **Estimate.** Pace from the closed plans (phases per plan and calendar days from `STATUS_ARCHIVE.md` and `git log`; functions per phase
   from `funcmap.tsv` rows tagged per plan). Remaining = functions in `port` groups plus the open stages' own unfinished phases. Report a
   low / mid / high range in phases and in weeks at the observed pace, with each assumption named (parallel sessions, review overhead, how
   many functions a phase really covers). Never a single date.
4. Write the short result into the Done note (about 50 lines) and `june2017_re/census/SUMMARY.md` (one page for the user).

**Done gate:** every unowned studio group appears in `exclusions.tsv` or a new stage brief; the estimate lists its assumptions; the user
decisions (if any) are asked in the hand-off, not buried in the table.

**Done note (V2, 2026-11-09, Opus 5.5):** read-only, no code, no flag, no build. Outputs in `june2017_re/census/`: `exclusions.tsv` (567 groups)
from `v2_exclusions.py` (`RESULT: PASS`, 19,577 of 19,577 unowned studio functions placed), `v2_sample.tsv` from `v2_sample.py`, `SUMMARY.md`
(the one page for the user). `tools/junebinary.py --check` still `RESULT: PASS (55556 functions)`. Subagents (Sonnet) read the backlog shape
and measured the pace; the grouping, decisions and estimate are mine.
- **Grouping.** V1's class column covers only 9,471 studio functions, so V2 places by address: March's `X2/US_SERVICE/X2.map` maps
  each funcmap symbol / class to its `.obj` (non-inline entries only), giving 4,783 June anchors; 4,337 of 4,782 neighbouring anchor pairs
  share an `.obj`. An unowned function between two anchors of one `.obj` is that file's (`file:` rows); between two files it is a `gap:` row.
  `Session.obj` / luajit anchors are skipped: shared serializer and binding instantiations sit all over the binary and split real files.
- **Decisions** (`SYS` / `M` / `STR` tables in the script): a class or file maps to a system by name; a gap takes a system when both
  neighbours agree (or share a stage), or when its decrypted strings (`func_strings.tsv`) name exactly one system; otherwise it is triage.
  - attach to existing stages, 9,014: S7 2,457, S9 2,009, S2 1,988, S22 725, S27 353, S31 327, S0 311, S16 271, S17 213, S25 138, S3 96,
    S33 77, S4 37, S8 8, S18 4;
  - new stages, 9,958: **S38** village / field core 169, **S39** game events 148, **S40** triage 9,641 (191 gaps);
  - exclude 596: network layer and Lua binding glue between `KLuaManagerProxy` and `X2ServerProtocol`, the serializer gap
    `X2UIInventory..X2CashShop@0x321c6800` (sample 10 / 10 serializers), the anti-tamper gap (`MEMORY BOMB TRIGGERED!!!`), `X2StatePVPRoom`
    (no PvP offline), `CX2PacketLog`;
  - park 9: `CX2BossRaidManager` (question below).
- **Work share** (`v2_sample.tsv`, 300 unowned studio functions, seed 11): logic 189, trivial 59, STL 37, serializer 15; a hand check of 12
  "logic" bodies found 7 gameplay, 5 container / ctor / dtor. Work share 35 / 51 / 67 %. Per-group samples (10 each, groups >= 60) show
  most groups mixed; only the gap above is pure glue. The biggest gap, `X2GUUser..X2Data@0x32092b20` (1,133 + 2,074 unknown), is mostly
  packet serializers (5 / 10, and 3 / 3 hand-read).
- **Pace** (git, distinct phase / review ids per ISO week): W42 167, W43 171, W44 214, W45 200. Character ports: 36-54 sessions and 3-4
  days per character; 20-45 June functions per implementation phase (char-raven / char-rena subjects).
- **Estimate:** low 818 / mid 1,462 / high 2,946 sessions = **~4 / ~8 / ~20 weeks** at 215 / 190 / 150 sessions a week. Terms: census
  work functions 6,438 / 11,115 / 18,363 at 40 / 30 / 20 per phase with review and planning overhead; S40 triage 10 / 15 / 25; open STATUS
  rows 120 / 150 / 180; unplanned characters (Ara, Elesis, Add, Rose, Ain) 230 / 275 / 342; 19 unplanned system stages plus Stage 9
  192 / 288 / 480. Assumptions are listed in `SUMMARY.md` (parallel sessions, review ratio, functions per phase, work-share error, unknown
  share, a ~60-session overlap between character ports and S7, play-test time not modelled).
- **Backlog:** Stages 38-40 added to `JUNE_PORT_BACKLOG_ORDER.md` (order table and details); briefs `kickoffs/june-backlog/stage38-plan.md`,
  `stage39-plan.md`, `stage40-plan.md`; three `june-backlog` STATUS rows (S40 waits for Z).
- **Not done here (by design):** no per-stage bullets for the 9,014 attached functions; a stage's planner filters `exclusions.tsv` by its
  `stage`, and Z adds the standing rule to re-run `junebinary.py` / `v2_exclusions.py` at each stage end.
- **Question for the user (Z applies the answer):** boss raid, world mission, wedding and mentor are server-driven multiplayer systems.
  Recommendation: exclude wedding and mentor; Stage 40 identifies boss raid and world mission and routes them to Stage 22 if solo-playable.
- **Answer (user, 2026-11-09), applied in V2:** wedding and mentor excluded; boss raid / world mission kept for Stage 40 (`SYS['raid']`,
  `SYS['social']`). `CX2InformerManager` is the notice pop-ups, not a mentor system (fix `cec84b06`). Now: park 0, S40 9,650 (192 groups).

## Phase Z - Close-out

**Run with:** Sonnet 5.5 / medium. **Depends:** V2 (and the user's answers, if V2 asked any).

Apply the user's answers to `exclusions.tsv` and the new stage briefs, check that every new stage has a STATUS row and a kickoff, record the plan in
`JUNE_PORT_BACKLOG_ORDER.md` ("Standing rules": re-run `junebinary.py` at the end of each stage to track the remaining count), move this plan's
STATUS block to `STATUS_ARCHIVE.md` with one *Closed plans* row, and commit.

**Done gate:** the closed-plans row exists; `junebinary.py --check` still passes; the backlog file lists the new stages.
