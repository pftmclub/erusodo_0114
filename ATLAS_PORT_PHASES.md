# Atlas/Elysion Port (Atlas Station, Fields 01–03, Elysion village, AtlasDevelopment / AtlasCity / Tower / SkyCrossroads / Adrian / Soless / Memory) — phased plan

> Written 2026-10-03 (read-only research session) as two docs, this one and
> `ELYSION_PORT_PHASES.md`, committed on `mods/june-buff-port`. **Merged into
> this one plan on 2026-10-16 (user).** Phase IDs are unchanged: **A0–A13**
> are the shared groundwork and the Atlas half, **E0–E15** the Elysion half.
> Older notes, `funcmap.tsv` rows and code comments that cite
> `ELYSION_PORT_PHASES.md` mean this doc's E sections and Elysion-half ledger;
> that file is now a pointer here.
>
> **One branch: `main` (user, 2026-10-16).** Every phase works and commits on
> `main` in the main Trunk. No branch or worktree is created and nothing is
> merged (this replaces the earlier `mods/elysion-port` plan). Start with
> Phase A0's kickoff (`kickoffs/atlas/A0.md`).

## Context

The user wants June 2017's Atlas and Elysion content (rest area, village,
fields, dungeons) running in the March 2014 client, ported the way Lanox was,
**without repeating Lanox's mistakes**. Lanox took ~30 phases of "data done",
then a long tail of defects found only by play-test
(`RANOX_PORT_PLAN.md` Phases 23–31, `LANOX_DEFECT_PHASES.md`,
`LANOX_REMAINING_PHASES.md`, `LANOX_TROUBLESHOOTING.md`). Almost all of them
were predictable from the C++ and the June binary. This plan front-loads that
reading (A0 census, A1a/A1b maps) and turns every Lanox defect class into a
gate (see *Lanox lessons → gates*).

In June both halves are one region: one local map, `LMI_ELYSION` (10009),
whose dialog draws an **"Atlas" page** (`AddDummyString("0")`) and an
**"Elysion" page** (`AddDummyString("1")`)
(`june_src_fixed/data217_kom/DLG_Map_Local_Elysion_Icon_NEW.src.lua:5,19,38,68,98,135,169,203,234`).
The plan follows that split: the **A phases** own the shared groundwork and
the Atlas page, the **E phases** the Elysion page and everything beyond
Field 02.

Intended outcome, Atlas half (A phases):
- A character standing in Lanox Field 03 walks through the portal into
  **Atlas Station** (`VMI_BATTLE_FIELD_ELYSION_REST_00` = 1008), reaches
  **Fields 01 and 02**, sees the **Elysion tab** on the local map, and can
  enter and clear **AtlasDevelopment, AtlasCity and Tower** at
  Normal/Hard/Expert.
- Characters can level **past 80, up to 99**.
- Everything is reachable through the same routes as every other region
  (world map, Cobo Express, warp NPC, field portals), with no "found in
  play-test" switch misses.

Elysion half (E phases):

- the **Elysion village** (`VMI_ELYSION` = 20009) and its new village systems
  (NPC relation data, new house keys, in-village warps);
- **Field 03** (40037) — a dead end back to the village;
- the local map's **scroll** between the Atlas and Elysion pages and the
  **guide window**;
- the **SkyCrossroads, Adrian, Soless and Memory** lines, which need three
  engine subsystems March lacks: **moving platforms**, the **SimpleDLG gauge
  subsystem**, and **dramatic messages**, plus ~35 more Lua bindings.

Intended outcome, Elysion half: from Field 02 the player walks into Elysion
village (houses, NPCs, shop, in-village warps), reaches Field 03, scrolls the
local map to the Elysion page, and can enter and clear all four lines at N/H/E
in unlock order (SkyCrossroads 83 → Adrian 84 → Soless 87 → Memory 87), with
Soless/Memory gauges, dramatic messages and moving platforms working.

Naming: in this doc **"Atlas"** means the region half (Atlas Station / the
Atlas page). Texture atlases are always written **"TET atlas"**, because
`*atlas*` greps also hit texture atlases.

---

## User decisions (2026-10-03)

1. **Split by June's own halves, in one plan** (one doc and one STATUS block
   since 2026-10-16). A phases: shared groundwork + Atlas Station + Fields
   01–02 + AtlasDevelopment, AtlasCity, Tower. E phases: village
   (`VMI_ELYSION` = 20009), Field 03, SkyCrossroads, Adrian, Soless, Memory,
   plus the local-map scroll and the guide window.
2. **Quests are out of this plan.** Everything quest-related goes to a later
   plan together with the Lanox quests (and possibly a Scaleform runtime):
   quest rows, quest-gate C++, EventScene/GFx scenes, the quest-triggered clear
   movies, quest-driven NPC swaps, bulletin-board / NPC-face cases, and the
   old-save quest migration. In this plan:
   - **June's quest keys are transcribed verbatim and stay inert** (March reads
     none of them): `REQUIRE_COMPLETE_QUEST_ID` / `REQUIRE_COMPLETE_SUB_QUEST_ID`
     (`MapData`), `m_iRequireCompleteQuestID` (`DungeonData`),
     `RequireCompletedQuestID` (local-map `EnterCondition`),
     `BATTLE_FIELD_DIRECTION_INFO` / `NEED_ONGOING_QUEST_ID` (Lanox portal row).
     Each gets a `-- quest plan:` comment. Do not "tidy" them out.
   - **The region stays gated by what March already reads:** level,
     `RequireClearDungeonID = { DI_RANOX_FINALHALL_NORMAL }`, and the
     `EXTRA_REQUIRE_DUNGEON_ID` chain.
   - Enums for the cutscene NPCs (`NUI_ELSION_EVENT_00/01`) are added (A3);
     the cutscene is not.
   - **Quest-driven NPC swaps** (Yuno → Hyper Yuno → Nono) are ported as
     *mechanism* (E1) but stay dormant until quest data exists. The Elysion
     clear movie (quest 22336) is deferred.
3. **Branch: `main`** (user, 2026-10-16). The plan works on `main` in the
   main Trunk, like every other plan: no new branch, no worktree. Phases
   needing June buff types are marked blocked on the named buff phase.
4. **Level cap 99 in this port** (A2). The user **explicitly authorises
   inventing StatTable rows 81–99** — this overrides CLAUDE.md's no-invented-
   table rule for that table — and leaves the method to us. **NpcExpTable
   81–99 is invented too** (Elysion monsters are level 83–99 and need it).
5. **Adopt June's `ExpTable.lua` whole** (levels 1–100). Existing saves get a
   one-way EXP migration (backup first, ask before the launch that runs it).
6. *(Recorded for the quest plan only.)* When quests are ported: GFx scenes
   are skipped and logged, not ported; saves that already cleared Lanox get
   the Lanox epic chain 22160–22217 marked complete (backup + ask).

---

## Fix philosophy (standing rules, applied here)

- **No partial June updates** ([[no-partial-june-updates]]). If June changed
  a function, parser or `case` March already has, port June's whole version
  and diff every case. June-only cases for content March lacks are omitted
  **with a comment in place**, never silently.
- **June wins** ([[june-behaviour-wins-even-when-it-removes-features]],
  [[june-asset-wins-over-march-at-same-filename]]). On an asset conflict,
  adopt June's file; merge only when June's file lacks a key March reads
  (prove by rendering, not by UV diff).
- **No guessing.** Every enum value comes from June's `Enum.lua` *and* is
  checked against June's parser in Ghidra when C++ reads it. Every new C++
  path cites its June function address in a comment. The only invented data
  is A2's StatTable/NpcExpTable 81–99 (user decision 4) — labelled in the
  file, logged at load.
- **Transcribe, don't tidy.** June bugs found in the ledger
  (`WI_TOWER_STAGE3` → missing file, `NUI_ELYSION_S_CIRCLE_CTR` typo,
  `LINE_TYPE.LT_INVALID`) go in verbatim with a log line.
- **Missing systems: parse, store, log.** When June depends on something
  March lacks entirely, parse and store the data and log it once as
  `[ELYDBG] unsupported …`. Never drop an entry silently.
- **C++ and Lua halves ship in the same phase** (Lanox Phase 14 shipped only
  the C++ and the Fire Temple broke). An enum value added in Lua must equal
  the C++ value; the review checks it.
- **House rules from `CLAUDE.md`:** every client edit behind a
  `SERV_IRUHADEV_*` flag in `KTDXLIB/Always.h`, all of this port's flags in
  **one delimited block** (`// ===== Atlas/Elysion port =====`) so the merge
  with the buff branch's `Always.h` tail is mechanical; `#else` keeps the
  original; `#endif FLAG` style; CP949 files edited at the byte level, then
  `file <path>` + `git diff --stat`. `KncWX2Server/Common` is touched only by
  the enum-mirror pattern Lanox already uses (`Enum.h`, `DungeonEnum.h`,
  `XSLUnitManager.h`, under `SERV_IRUHADEV_ELYSION_PORT`).
- **Server-side Lua the client loads is the user's to pack**
  (`StatTable.lua`, `NpcExpTable.lua`, `MapData.lua` mirror): implement the
  loader/degrade path now, name the file at hand-off.
- **Never launch `X2_offline.exe` without asking. Never delete
  `els_db.sql`. Kill processes by PID only.**

---

## Model, effort, review

| Phase | Item | Depends on | Kind | Model | Effort | Review |
|---|---|---|---|---|---|---|
| A0 | Docs, `region_census.py` v1 | – | tooling | Sonnet 5.5 | high | light (Opus 5.5) |
| A1a | RE + C++ reachability map (both halves) | A0 | Ghidra + C++ read-only | Opus 5.5 | **xhigh** | none (self-check vs census) |
| A1b | Script-engine map (both halves) | A0 | Ghidra read-only | Opus 5.5 | **xhigh** | none |
| A2 | Level cap 99 + EXP migration | A0, **MARCH_2014_MIGRATION Phase 7** | C++ + data + offline DB | Opus 5.5 | high | required |
| A3 | Region enums, Lua + C++ (both halves) | A1a, A1b | data + C++ | Sonnet 5.5 | high | required (Opus 5.5) |
| A4 | Atlas-half script engine | A1b, A3 | C++ | Opus 5.5 (Sonnet 5.5 for names A1b marks transcription-ready) | high | required |
| A5 | Atlas Station (rest 1008) | A3 | data + C++ switches | Opus 5.5 | high | required |
| A6 | Lanox→Atlas crossing + Fields 01/02 | A4, A5 | data + C++ | Opus 5.5 | high | required |
| A7 | Local map, Atlas half | A3 | data + C++ | Opus 5.5 | high | required |
| A8 | AtlasDevelopment line (pattern-setter) | A4, A6, A7 | data (+ C++ if gaps) | Opus 5.5 | high | required |
| A9 | AtlasCity line | A8 | data | Sonnet 5.5 if census shows 0 open gaps for the line, else Opus 5.5 | high | required (Opus 5.5) |
| A10 | Tower line | A8, **JUNE_BUFF Phase 10** | data | same rule as A9 | high | required (Opus 5.5) |
| A11 | Asset closure + pack list | A5–A10 | data | Sonnet 5.5 | high | required (Opus 5.5) |
| A12 | Build, deploy, hand-off | A11 | build | Sonnet 5.5 | medium | none |
| A13 | Play-test sweep + close-out | A12 | verify + root-cause | Opus 5.5 | high | none |
| E0 | Preconditions, census, `ELYSION_SOLESS` decompile | Atlas A0–A4 (incl. A2 cap 99), A7 | tooling | Sonnet 5.5 | medium | none |
| E1 | Village engine | E0 (+ A1a map) | C++ (new mechanisms) | Opus 5.5 | high | required |
| E2 | Elysion village 20009 | E1, Atlas A5, A6 | data + C++ switches | Opus 5.5 | high | required |
| E3 | Field 03 (+ Field 04 enum-only check) | E2 | data | Sonnet 5.5 | high | required (Opus 5.5; escalate on gaps) |
| E4 | Local map, Elysion page (scroll, guide, filter, flags) | E0, Atlas A7 | C++ + data | Opus 5.5 | high | required |
| E5 | Moving platforms | Atlas A1b, A3 | C++ (new mechanism) | Opus 5.5 | high | required |
| E6 | SimpleDLG gauges + dramatic messages | Atlas A1b, A3 | C++ (new mechanisms) | Opus 5.5 | high | required |
| E7 | Elysion-half bindings + additional world | Atlas A1b, A3 | C++ | Opus 5.5 (Sonnet 5.5 for names A1b marks transcription-ready) | high | required |
| E8 | SkyCrossroads line (first line of the half) | E2, E4, E7, Atlas A2, A8 (per-line checklist) | data (+ C++ if gaps) | Opus 5.5 | high | required |
| E9 | Adrian line | E5, E7, E8 | data | Sonnet 5.5 if census shows 0 open gaps, else Opus 5.5 | high | required (Opus 5.5) |
| E10 | Soless line | E5–E7, E9 | data + C++ glue | Opus 5.5 | high | required |
| E11 | Memory line (finale) | E6, E7, E10, **JUNE_BUFF Phase 10** | data + C++ glue | Opus 5.5 | high | required |
| E12 | Elysion field PvP map (optional) | E3 | data | Sonnet 5.5 | high | required (Opus 5.5) |
| E13 | Asset closure + pack list (Elysion half) | E2–E12 | data | Sonnet 5.5 | high | required (Opus 5.5) |
| E14 | Build, deploy, hand-off | E13 | build | Sonnet 5.5 | medium | none |
| E15 | Play-test sweep + close-out (whole region) | E14 | verify + root-cause | Opus 5.5 | high | none |

A1a and A1b are independent (two sessions in parallel is fine: both are
read-only). A5 and A7 are independent of each other and of A4.

**E1, E5, E6 and E7 are independent in content**, but all four append to
`X2Main.cpp`'s binding block and to the `Always.h` Atlas/Elysion block. On the
one branch they run **one at a time**, in the order E7 → E5 → E6 → E1.

### Model selection rules

Prices and reasoning are the same as `JUNE_BUFF_PORT_PHASES.md` *Model
selection*; condensed:

1. **Price the failure, not the request.** A wrong enum value, a misread June
   branch or a missed switch compiles cleanly and surfaces only after a pack
   plus a play-test. Any phase that **derives** behaviour from the June binary
   or **builds a mechanism March lacks** runs on Opus 5.5.
2. **Identify with Opus, transcribe with Sonnet.** A1a/A1b write down, for
   every gap, what a transcriber needs. Phases that only apply a written
   method (A3, A9 when gap-free, A11, A12) run on Sonnet 5.5 with a required
   Opus 5.5 review.
3. **Escalation rule.** A Sonnet session that hits an item the A1 maps do not
   settle **stops on that item, logs it in this doc for an Opus pass, and
   does not guess.**
4. **A1a/A1b run at `xhigh`** — every later phase builds on them. Everything
   else Opus runs at `high` (Opus defaults to `medium`, so every Opus kickoff
   says `high`). Sonnet defaults to `high`; A12 sets `medium`.
5. **No Fable 5.1, ever** (user, 2026-10-02: cost). **No Haiku 4.5**: its 200K
   context cannot hold June's `Enum.src.lua` (336 KB) next to a region file.
6. **Required review** = a fresh Opus 5.5 `high` session that checks the diff
   against the A1 maps (enum values both sides, key names, switch coverage,
   flag/`#else`/CP949 hygiene, staging + manifests). It spot-checks 2–3 items
   in Ghidra and escalates to a full re-check only if one fails.

---

## Starting context (paste with any phase)

- **Branch** (all edits, builds and commits): **`main`**, in the main Trunk
  `F:\elsword stuff\elsword_2014\els_2014\ElswordFiles\ready_and_built\ProjectX2_SVN\ProjectX2_SVN\source\EU_CN_US\Trunk`
  (user, 2026-10-16: one branch for every plan).
  - Create no branch and no worktree, and never `git switch`, checkout or
    stash. Other plans' sessions share this tree and commit onto `main` too,
    so commit with explicit pathspecs.
  - This plan's `STATUS.md` rows are committed on `main`, in the phase's own
    commit (x2-phase *Finish*).
- **Read-only shared inputs** (in the Trunk): `june_src_fixed\`,
  `elstools\elstools.exe`, `june2017_re/`.
- **Game directory** (deploy target, logs, save):
  `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data`.
  **Packing workspace** (shared with the buff port, not in git):
  `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents\dataNNN_kom\`.
- **June client:** `F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\`
  (`dataNNN_kom\` folders, loose `music\`, `movie\`, `eventscene\`).
  - **June Lua comes from `june_src_fixed/data###_kom/<Name>.src.lua`,
    always.** Do not re-decompile and do not read the `.src.lua` siblings in
    the June tree (they predate the opcode fix). Use `june2017_re/ljd_jun2017.py`
    only for a file **missing** from `june_src_fixed/`, and record which.
  - Non-Lua assets come from the June tree. Compare each copied file's size
    with June's kom index: `elstools` writes ~¼ of June algo-2 entries 1–255
    bytes short (`JUNE2017_ASSET_CLOSURE.md:3949`).
- **June binary:** `14JUN17\data\unpacked_x2.exe` in Ghidra
  (`F:\elsword stuff\els_jun_2017_RE\els_jun_2017.gpr`), imagebase
  `0x31aa0000`, GhidraMCP at `http://127.0.0.1:8080` — use the `ghidra` skill
  (`curl -s --noproxy 127.0.0.1`). Lua key strings are XOR'd per string;
  decrypted strings are in `june2017_re/xorstrings.tsv` (cite only rows with
  status `ok`). Record confirmed functions in `june2017_re/funcmap.tsv`. Do not
  mutate the Ghidra project without asking.
- **Lua compile/stage:** *(superseded 2026-10-09 by `LUA_SOURCE_PHASES.md`; the
  STATUS `atlas` block says what replaces it. The wording below is kept as
  written.)*
  - `elstools compile-batch -v mar-2014 -i <file>` compiles **in place** —
    copy to scratch first. Never `-v jun-*`.
  - Patches to an existing March file: `march2014/dataNNN/<Name>.src.lua`
    (March's archive number). New files: `march2014/update/dataNNN/` (June's
    archive number, even if March has no such archive — data180–183, data197,
    datai027 are new archives here; all inside `KOM_FILE_COUNT_226`'s 226/32
    mount range).
  - The compiled `.lua` (header `1B 4B 4C 81`) also goes into
    `kom_contents/dataNNN_kom/` and that folder's `_manifest.json`
    (`.lua` = `"2"`, everything else `"0"`; CRLF, 2-space indent, no trailing
    newline). Never ship `.src.lua` / `.std.luac` inside a kom.
  - **Shared-staging merge rule.** `kom_contents/data036_kom/Enum.lua`,
    `MapData.lua`, `StringID_def.lua`, `BattleFieldData.lua`, `EffectSet.lua`
    (and any other file both ports touch) must be merged against **both**
    branches' `march2014/` copies before staging. Diff the staged file against
    the buff branch's copy first; never overwrite the other port's rows.
  - **The user packs.** Verify against the **live** kom (`elstools inspect`
    sizes, or extract and decompile), never against `kom_contents/`. Check
    mount order: `datai*` mounts after `data*`, and the last mount wins
    (`KGCMassFileManager.cpp:490`, `X2Main.cpp:1406`).
- **Build and deploy:** `python tools/build.py --deploy` (skill
  `x2-build-deploy`; run it in the background) from the Trunk with this branch
  checked out. It builds the four projects in order with `SolutionDir`,
  handles the PCH, and checks the deploy. Prove new code compiled in by
  finding a UTF-16LE literal that exists only inside the new `#ifdef` in
  `X2/US_SERVICE/x2.exe` — exit code 0 is not proof.
- **Deploy:** `build.py` deploys as `$DATA/X2_offline.exe`. **Record the
  `DEPLOY OK` line (it names the commit built) in the phase's Done note** —
  every session deploys to the same name, last deploy wins.
  **Ask before every launch.**
- **Logs:** load time → `ErrorLog.txt`, `OpenScriptFile.log`; runtime →
  `offline_server.log` (`CX2OfflineLog::Server`) and `offline_packets.log`.
  `CX2OfflineLog::Server` is **not open yet** during `CX2LocationManager`
  parsing and `CX2UnitManager` loading — use `ErrorLogMsg` there. `KTDXLIB`
  cannot reach `offline_server.log` at all. New lines are prefixed `[ELYDBG]`
  (debug-only, behind `SERV_IRUHADEV_ELYSION_DEBUG`) or `[CAP99]` /
  `[ITEM_MISSING]` (permanent). **A clean log is not evidence of success**
  (`LANOX_TROUBLESHOOTING.md:186-189`).
- **Debug warp:** `SERV_IRUHADEV_CHAT_WARP_DEBUG` (`X2Lib/X2ChatBox.cpp:1064`)
  gains `atlas` (→ 1008) in A5 and `elysion` (→ `VMI_ELYSION`) in E2.
- **Do not consult** `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md`
  or the `mods/lanox-offline` branch (a failed prior attempt). **Repeat this
  to every subagent** — one Lanox subagent read it anyway.
- **The one June Lua file not in `june_src_fixed/`:** `ELYSION_SOLESS.lua`
  (Soless boss, 105,101 B, `14JUN17/data/data037_kom/`). E0 decompiles it to
  `june2017_re/june_lua/ELYSION_SOLESS.src.lua` with
  `elstools decompile-batch` (for the `.std.luac`) then
  `python june2017_re/ljd_jun2017.py --check …`; every later phase reads that
  copy.
- **Buff branch status** is in `JUNE_BUFF_PORT_PHASES.md`; check it before
  E10/E11.
- Line numbers below were true on 2026-10-03. Re-read the cited line before
  building on it.

---

## Investigation ledger (measured 2026-10-03 — do not redo; the census re-verifies numbers)

Path roots: **JF** = `Trunk/june_src_fixed/`, **JR** = `14JUN17/data/`,
**M14** = `march2014/`, **MK** = `kom_contents/`.

### Topology (June)

| From | Exit → target | Source |
|---|---|---|
| Lanox field row `BATTLE_FIELD_ID = VMI_BATTLE_FIELD_RANOX_FIELD_04` (NPC table `Ranox_Field_03.lua`, linemap `Field_Ranox_Stage3_LineMap.lua` — Lanox names are offset by one) | `LINE_NUMBER 103`, `POSITION_INDEX_TO_MOVE 400`, `PMT_MOVE_TO_VILLAGE` → 1008; plus `BATTLE_FIELD_DIRECTION_INFO { FORCE_MOVE_BATTLE_FIELD_START_POS 32, NEED_ONGOING_QUEST_ID 22218, DIRECTION_NPC_ID { NUI_ELSION_EVENT_00, _01 } }` | JF `data036_kom/BattleFieldData.src.lua:1599-1652` |
| Atlas Station 1008 | pos 400 arrival from Lanox; 401 → Lanox F04 line 109; 402 arrival from F01; 403 → F01 line 333; 404 IsWarp | JF `data034_kom/DLG_Map_Village.src.lua:4367-4423` |
| Field 01 (40035) | L0 → 1008 pos 402; L53 → F02 pos 1 | BattleFieldData `:2061-2103` |
| Field 02 (40036) | L0 → F01 pos 365; L142 → `VMI_ELYSION` pos 500 (E2) | `:2104-2157` |

- March already has the Lanox side: line 103 is `LT_POTAL` in
  `M14/update/data040/Field_Ranox_Stage3_LineMap.src.lua:1358-1364`. The
  portal row and direction info were omitted on purpose
  (`M14/data036/BattleFieldData.src.lua:1985-1995`, `2021-2031`).
- F01/F02 enter and return to 1008. F01 `REQUIRE_LEVEL` 80 / monster 80, F02
  80 / 83. Field 04 (VMI 40038, WI 641) is **enum-only** in June (no row,
  scene, NPC table or dialog).
- **Key rename:** June `DLG_Map_Village` uses
  `OtherPlaceID`/`OtherPlaceStartLineIndex`; March's `VillageParsing` reads
  `BattleFieldId`/`BattleFieldStartLineIndex`
  (`X2Lib/X2LocationManager.cpp:398-399`). Lanox Phase 27 lost 4 of 5 exits to
  this. Atlas Station's IsWarp/summon semantics must be checked the same way.

### Gates March reads (these are what gate the region in this plan)

- `MapData` (JF `data036_kom/MapData.src.lua`): rest 1008 at `:343-356`
  (level 75–85), `VMI_ELYSION` at `:357-370` (85–99, `BEFORE = VMI_RANOX`).
  **Cobo Express zones** (`:98-107`): 1008, F01, F02 = **17**;
  `VMI_ELYSION`, F03 = **18**. March tops out at 16
  (`M14/data036/MapData.src.lua:445-460`).
- Local map `EnterCondition` (JF `data217_kom/DLG_Map_Local_Elysion.src.lua:4-10`):
  `RequireUnitLevel = 78`, `RequireClearDungeonID = { DI_RANOX_FINALHALL_NORMAL }`
  (+ inert `RequireCompletedQuestID = 22218`). `DungeonIDList` (`:27-36`) = the
  7 lines + `DI_DEFENCE_DUNGEON_ELDER_NORMAL` (exists in March, 38300).
  `BattleFieldIDList` (`:37-41`) = F01–F03.
- **Dungeon unlock chain** (JF `data035_kom/DungeonData.src.lua`), via
  `EXTRA_REQUIRE_DUNGEON_ID`, which March parses (`X2DungeonManager.cpp:114`):
  AtlasDevelopment N (line `29932`; level 80–82; inert quest key 22218) →
  AtlasCity (`30516`, 81–83) → Tower (`31120`, 82–84) → SkyCrossroads (83–85)
  → Adrian (84–87) → Soless (87–99) → Memory (87–99).

### IDs (June values; 0 collisions in every table against March and its Lanox additions)

| Table | New | Values |
|---|---|---|
| `WORLD_ID` | 40 *(A0 correction: was 41; the rows below sum to 40)* | shared: village 636, rest 637, F01–F04 638–641, PvP 752 (`WINNER_ADRIAN` 757 deferred); AD 642–645; AC 646–649; TW 650–654 (S3 → missing file); SC 672–675; AR 706–710; SL 711–716; ME 727–731 |
| `VILLAGE_MAP_ID` | 7 | rest 1008, village 20009, lounge 30009, F01–F04 40035–40038 |
| `LOCAL_MAP_ID` | 1 | `LMI_ELYSION` 10009 (free in `X2LocationManager.h:141-145`) |
| `HOUSE_ID` | 12 | `HI_YUNO` 40092, `BERNARD`/`DURANDA`/`THEODOR`/`HUGO` 40131–40134, `BIRD_QUEST_00..04` 40135–40139, `ELYSION_RED/BLUE_TREE` 40145–40146 |
| `DUNGEON_ID` | 21 | AD 33330–2, AC 33340–2, TW 33350–2, SC 33870–2, AR 33880–2, SL 33890–2, ME 33900–2 (Hero rows out of scope) |
| `NPC_UNIT_ID` | 203 *(A0: the total reproduces; the split was wrong)* | 201 region NPCs incl. `NUI_ELSION_EVENT_00/01` (2139/2140), which are inside Appendix A/B's 201 values, **plus** `NUI_STORY_QUEST` 10142 and `NUI_TRANSCEND_QUEST` 10153 (village quest-house NPCs of `HI_STORY_QUEST` / `HI_TRANSCEND_QUEST`; in neither appendix; **deferred to the quest plan**, user 2026-10-16, see *Deferred to the quest plan*). Full per-line lists: Appendix A |

- 16 NUIs the region uses already exist in March (Lanox/common:
  `NUI_ADAMS`, `ARIEL`, `BILLBOARD`, `CAMILLA`, `GLAVE`, `LURIEL`, `MU`,
  `FIRETEMPLE_GATE`, `FIRE_DEVIL_BERSERKER`, `FIRE_DEVIL_FIGHTER_FIRETEMPLE`,
  `MOUNT_FIREBIRD_FIRETEMPLE`, `SECRET_MAG_NOM`, `SECRET_MAG_CHARGER`,
  `SECRET_SUMMON_MAG_NOM`, `SECRET_MAGI_OBJECT`, `SECRET_LAVA_DUMMY`).
- 35 NUIs deferred (Henir ×11, CSM cards ×10, `SET_*` ×2, title/event ×4,
  `EMPEROR_FEAR`, `SUMMUN_{TEMPLAR,LIBYA,PUTER}`, 4 templet-only entries).
- **Relocation trap:** June `NUI_STEEL_AND_STEING` = 10072 = March's
  relocated `NUI_EDEL` (Lanox moved it off 10067; `RANOX_PORT_PLAN.md:227`
  still says 10068 — wrong, 10072 shipped). Used only by June `NPCTemplet`;
  out of scope, but the census checks every incoming ID against shipped
  relocations.
- `M14/update/data036/NewNPCStat.src.lua` already stages 223 `NUI_EL[YS]*`
  rows; they go live once the NUIs exist in both `Enum` and `NPCTemplet`.

### March C++ end markers that Elysion IDs collide with (A3 must bump them first)

| Marker | Value | Where | Why it matters |
|---|---|---|---|
| `VMI_BATTLE_FIELD_REST_END` | 1008 | `KncWX2Server/Common/Enum/Enum.h:217` | = Atlas Station |
| `VMI_VILLAGE_MAP_END` | 20009 | `Enum.h:251` | = `VMI_ELYSION` |
| `VMI_BATTLE_FIELD_END` | 40033 | `Enum.h:322` | fields 40035–40038 must sit before it (`IsBattleFieldID`, `Enum.h:546`) |
| `WI_END` | 752 (implicit) | `X2Lib/X2World.h:824` | world creation refuses ≥ it: `X2State.cpp:6789`, `X2StateField.cpp:1293`, `X2StateServerSelect.cpp:7371`, `X2StateBeginning.cpp:3991`, asserts `X2BattleFieldGame.cpp:295/457` |
| `NUI_MAX_LIMIT` | 10191 (implicit) | `X2Lib/X2UnitManager.h:2704` | NUI 10194/10195 exceed it (`X2ItemManager_Preprocessing.cpp:1501`) |
| `DI_END` | 41311 | `KncWX2Server/Common/Enum/DungeonEnum.h:512` | in-range for N/H/E; bounds `X2LocalMapUI.cpp:1163-1176` |

- Range helpers: `IsVillageField` stops at `VMI_RANOX`
  (`X2LocationManager.h:495`; June allows up to 20010); `IsDungeonGate` /
  `IsDungeonLounge` (`:508-522`) stop at Hamel.
- Lua-side sentinels are stale (`march2014/data036/Enum.src.lua`: `WI_END`
  573, `VMI_BATTLE_FIELD_END` 40028, `DI_END` 41120).
- **Offline enum seed clobbers the real table:** `X2OfflineLuaEnum.cpp:21-63`
  `lua_setglobal("VILLAGE_MAP_ID")` from `X2Lib/Offline/X2OfflineLuaEnumSeed.h:459-542`,
  run lazily by 9 loaders. After it runs, `VILLAGE_MAP_ID.VMI_ELYSION` is nil
  for every later Lua (incl. the local map re-run at `X2LocalMapUI.cpp:1146`).
  Its own end markers are stale (`REST_END` 1007, `VILLAGE_MAP_END` 20008,
  `BATTLE_FIELD_END` 40026). Fix in the same change as `Enum.lua`.

### C++ sites that enumerate regions (the Ranox touch-list to mirror)

| File:line | Function | Purpose | Ranox flag |
|---|---|---|---|
| `X2LocationManager.cpp:1582-1585` | `IsValidWarpField` | warp validity (Cobo ACK `X2StateMenu.cpp:6623`, warp item `X2UIInventory.cpp:9470`, loading `X2TFieldGame.cpp:4634`) — false = silent no-op | `RANOX_PORT` |
| `X2TFieldGame.cpp:4609-4619` | `LoadFieldImage` | loading art; June `FUN_323ad5f0`: 0x3f0 → `HQ_LoadingDungeon_Atlas_Pit_Rest_01/02.tga`, 0x4e29 → `HQ_LoadingVillage_Elision_01/02.tga` (both only in JR `data183_kom`) | `RANOX_PORT` |
| `X2WorldMapUI.cpp:856-882` | `GetFieldMapDlg` | M-key field map dialog; default asserts → NULL | `RANOX_PORT` |
| `X2WorldMapUI.cpp:2353-2379` | `UpdateNaviMap` | navi thumbnail | `JUNE_FIELD_MAP` |
| `X2FieldMapBase.cpp:442-503`, `590-603` | `ResetCameraMatrix`, `ResetMapPosAndSize` | field-map camera, window | `JUNE_FIELD_MAP` |
| `X2StateMenu.cpp:6174`, `6514-6572` | `InitWarpList`, `CreateWarpDest` | Cobo Express (B key) rows | `RANOX_PORT` (+`WARP_DEST_LOG`) |
| `X2UIInventory.cpp:13966`, `14154-14178` | `InitWarpList`, `CreateWarpDest` | warp-NPC / warp-item rows (the second implementation Lanox missed until Phase 29) | `RANOX_PORT` |
| `X2ChatBox.cpp:1064-1101` | chat commands | `lanox` debug warp | `CHAT_WARP_DEBUG` |
| `X2LocalMapUI.cpp:737-976` | `UpdateLocalMapDLG` (June) | tab paging; an Elysion tab is the 4th on page 2 | `JUNE_LOCAL_MAP_UI` |

**Sites with a Sander case but no Ranox case (Lanox misses).** A1a decides
each; porting June's whole function fixes Lanox at the same time:
`OpenFieldName` (`X2TFieldGame.cpp:4725-4797`, live — field-title banner);
`OnRClickedItem` return scroll (`X2UIInventory.cpp:6596-6608`);
`ResetWholeMapTexture` (`X2FieldMiniMap.cpp:318-443` — one report says live,
`LANOX_REMAINING_PHASES.md` P5 says `CX2FieldMiniMap` is dead in both builds:
settle it — *A1a: dead in March, class gone from June*); `CX2BossRaidManager::GetFieldOpenTextureName` / `GetCameraIndex`
(`X2BossRaidManager.cpp:417-430`, `617-630`; only matters if a field hosts a
raid portal). `GetBoardStartQuestList` / `GetNPCFaceTextureKey` are quest
UI → quest plan.

### June-only engine the region needs

- **Local map** (owned by E4): scroll `FUN_329378a0`,
  `329379d0`, `3293c1e0`, `329366a0` ("Elysion_Local"), LMUCM 18006/18007,
  guide window `DLG_Elysion_LocalMap_Guide.lua` (`FUN_329386b0`, opened from
  `OpenLocalMapDLG` `FUN_3293c440` for 0x2719, closed by LMUCM 18008), page
  filter member +0x54, event flags 0x8d4/0x8d5 (`FUN_322aa950`). March has
  comments at `X2LocalMapUI.cpp:379, 800, 818, 885, 1304, 1338, 1399`.
  *(A1a, 2026-10-16: `322aa950` is `std::map::find`. The flags are code events
  2260/2261 (+2262), read through `CX2InstanceData::IsEnableCode` `31e5a860`. They
  also gate portals and the Cobo list: see the A1a summary.)*
- **Unbound Lua names: 65** (53 from decompiled region scripts, 9 inferred
  from `ELYSION_SOLESS` bytecode, 3 registry/UI). 8 already have a C++ method
  ("(b)": binding only), 57 need new C++. Method: every string on
  `lua_tinker`/`class_def` lines in X2Lib/KTDXLIB/X2 (1716 names), calibrated
  (`SetFlag_LUA`, `SetForceInvincible` bound; `GetStoredGUUser` unbound).
  *(A0, 2026-10-16: 32 are still unbound by the census below (A0 review: 31 before SOLESS constants were read exactly); june-effect C3z-4 bound ~30 of the listed names, `GetStoredGUUser` among them (`X2Main.cpp:4719`). The calibration control is now `AddMovableLineData_LUA`.)*
  **Name match only; the receiver class was not verified** — A1b does that.
  - **Atlas half (A4):** `GetCenterPositionLineIndex`,
    `GetDyingStart`(b), `GetWorldObjectMeshByName`, `SetMovieDescriptionData`
    (in-dungeon boss-event subtitles, e.g. `ELSION_EVENT_1ST_BOSS.src.lua:159`),
    `IsActiveMonster`(b, `X2GUNPC.h:2155`), `FadeOriginWorldColor_LUA`,
    `GetAliveActiveNPCCount`(b, `X2Game.h:521`), `LiveKeyCodeNPCNumExceptDying`,
    `SetImmuneToExtraDamage`; `SetMonsterBuffGroupID` (**owned by JUNE_BUFF
    Phase 10**); linemap key `CHANGE_LINE_TYPE_AFTER_ALL_ACTIVE_NPC_KILLED`
    (`Elysion_Tower_stage1_LineMap:909`). Local map: `GetNationFlag`
    (`Icon_NEW:320`; March binds only `SetNationFlag`, `X2Main.cpp:3343` —
    **hard-aborts the chunk**), `SetChangeUVPoint` (`Back_NEW:15`).
  - **Elysion half** (E5–E7; see *Elysion half: engine gaps* below): moving platforms, SimpleDLG gauges,
    dramatic messages, ~35 more bindings, the additional world.
  - **Unit-script keys March never reads** (shared; A1b assigns owners):
    `USE_DEFAULT_DIE_PARTICLE` (85 scripts), `DRAMATIC_MESSAGE0-4` (13),
    `IMMUNITY_DAMAGE_DATA_STOP_TIME` (13), `SOUND_PLAY_ALL_OPTION0/1` (11),
    `NOT_CHECK_INDICATOR` (9), `BUFF_GROUP_ID` (7, buff Phase 10),
    `SOUND_PLAY_EX` (7), `CAN_NOT_PASS_SPECIFIED_BONE` (5), `TARGET_FRONT_ME`,
    `SHOW_GAGE`, `INVINCIBLE_ATTACK_TYPE` (4 each), `SUPER_ARMOR_FRONT`,
    `HEIGHT_CHECK_RANGE`, `CAN_PASS_UNIT_AT_FIRST_FRAME` (2 each).
  - DungeonData keys March never quotes: `m_iRequireCompleteQuestID` (inert by
    decision 2), `ENDING_SPEECH0`, `FixMaximumDifficulty`,
    `Min/MaxAdequateMemberCount`, `additionalTicket`, `m_bForbidEnterDungeon`
    — Lanox shipped with the last six too; the census classifies each.
- **Side finding (live Lanox bug):** `M14/update/data037/SECRET_SCENE_DUMMY.src.lua:382`
  calls `SetCameraDistance`, which exists in C++ (`X2World.h:1181`) but is
  not bound. A4 binds it.

### Level cap

- Cap is 80: `SERV_IRUHADEV_LEVEL_CAP_80` (`KTDXLIB/Always.h:3621`) redefines
  `USE_MAXLEVEL_LIMIT_VAL` → `g_iMaxLevel = 80`; compile-time check at
  `X2Lib/Offline/Handlers_Room.cpp:3052`; `CX2OfflineStatTable::MAX_LEVEL = 80`
  (`X2OfflineStatTable.h:62`); `X2Define.h:18` `LIMIT_MAX_LEVEL` is dead.
- `KncWX2Server/ServerResource/US/StatTable.lua`: `ReserveMemory(class, 80)`
  per class, then `SetUnitStat(class, level, { AtkPhysic, AtkMagic,
  DefPhysic, DefMagic, HP })`, 4038 rows. Near-linear: class 1 AtkPhysic +10 /
  level, HP +1687.5 / level (`1, 80 → 1159 / 953 / 257 / 214 / 144562.5`).
  June's `CharacterStatTable.lua` is a different, reformed stat system
  (equipment-relative formulas) — not a source for these rows.
- `ServerResource/US/NpcExpTable.lua`: `DropTable:AddNpcExpTable(level, exp)`,
  levels 1–80 (80 → 4203). Atlas monsters are level 80–84: without rows 81+
  they award 0 EXP.
- June `data036_kom/ExpTable.lua` (`AddExpTable_LUA { LEVEL, NEED_EXP,
  TOTAL_EXP }`) runs to 100: TOTAL(99) = 870,692,600, TOTAL(100) =
  1,011,151,800. March TOTAL(80) = 986,793,900 — **above June's level 99**, so
  existing saves must be migrated. Packet `m_iEXP` is `int`
  (`CommonPacket.h:1381`); June's values fit.
- **Corrected by A2 (2026-10-16)**, the bullets above kept as written:
  - March TOTAL(80) is **866,438,000** (shipped bytecode, T0 decompile =
    `tools/data/march_exptable.tsv`). 986,793,900 is the Jan-2014 studio
    source's figure. Still below June's TOTAL(99), so the migration stays.
  - StatTable: 4000 live rows (50 classes × 80) plus 38 commented-out
    (classes 110/111, levels 81-99). Class 8 has no `ReserveMemory` line.
  - The typedef is `Handlers_Room.cpp:4211`, not 3052. `m_iEXP` is
    `CommonPacket.h:1405`.
  - Without rows 81+, `GetNpcExp` clamps a level 81+ monster to the level-80
    row (4203), so it does not award 0.

### Assets

- Reference count from region Lua + registry rows: **≥ 2,071 files to
  adopt** (2,383 refs; 249 already in March by name; 63 not found in June's
  extract). By archive: data180 912, data182 422, data197 179, data181 144,
  data096 90, data002 78, data104 77, data057 29, music 23, data013 21, …
  This is a **lower bound** — mesh-internal textures and particle textures
  were not followed (Lanox's Burned Forest smog miss).
  *(A0, 2026-10-16, numbers as of the A0 review: census refs 2,679; 797 already in March by name (June asset adoption since), 1,808 June-only still to adopt (June kom or loose `music` / `movie` / `eventscene`), 74 names June itself ships no file for (INFO rows `assets.absent`).)*
- **393 effect-set templets** are referenced; the staged
  `march2014/data036/EffectSet.src.lua` and `MK/data036_kom/EffectSet.lua` both
  contain **0** Elysion entries. Damage-effect children not measured.
  *(A0, 2026-10-16: no longer true. june-effect C11a (`c13f8115`) staged June's full `EffectSet` (8,141 templets incl. 451 Elysion-named), so all 416 sets the region names are present (A0 review: 410 before SOLESS constants were read exactly). The census counts 416 against the 393 above; the 393 method was not recorded.)*
- Loading screens: JR `data183_kom` (Atlas Pit Rest, Elision village, and
  per-dungeon `HQ_LoadingDungeon_*_Pit_*`).
- `DLG_UI_Dungeon_Image11.dds` (key `ELISION_ATLAS_PIT`) is already live in
  `datai022`, which mounts after `data065`. Soless/Memory need the new
  `Image12`.
- TET atlases already carrying Elysion keys live in March (adopt-June
  wholesale, nothing to merge): `Texture_Rest` (`ELISION_R`),
  `Texture_Field_03` (`ELISION_01/02`), `Texture_Village_01`, `Button21`
  (`BTN_ELISION_N/O/D`). Deliberately unmerged by Lanox, now owed:
  `DLG_Localmap_Title_01` (`Map_Title_Atlas`, `Title_Elision_01..03`, …) and
  `Fieldname_6` (`LANOX_REMAINING_PHASES.md:741,753`).

### Lanox-side omissions to revert (all in `march2014/`, verify each line first)

`data036/BattleFieldData.src.lua:1988-1991` and `2022-2031` (A6);
`update/data034/DLG_UI_FIELD_MAP_Ranox_04_NEW.src.lua:111-112` (deleted
right-button block → 1008, A6); `data034/DLG_Map_House.src.lua:4196-4197`,
`4942-4944` (`HI_GUILD_MARK` village row, E2);
`data034/DLG_Map_Local.src.lua:12`, `DLG_Map_World_Elios.src.lua:14`,
`DLG_UI_Local_Map_NEW.src.lua:532-534`, `713` (`LocalMapRadioBtn9`, A7);
`DLG_UI_PARTY_LENGTH_NEW.src.lua:272-277` (picture 9, A7);
`DLG_UI_World_Map_NEW.src.lua:2293-2316` (cloud `AddDummyInt(99999)` at
`2315`, A5); `data036/Enum.src.lua:4978`, `6618` (A3); `MapData.src.lua:453`
(A5); `data040/WorldData.src.lua:3034` (A5); `PVP_Map_List.src.lua:116-117`
(E12).

### Elysion half: topology and gates (June)

| From | Exit → target | Source |
|---|---|---|
| Field 02 (40036) | L142 → `VMI_ELYSION` pos 500 (A6 omitted it with a comment) | JF `data036_kom/BattleFieldData.src.lua:2104-2157` |
| `VMI_ELYSION` (20009) | 500 arrival from F02; 501 → F02 line 248; 516 arrival from F03; 517 → F03 line 1; 514 IsWarp; **in-village warps** 502→503, 504→505, 506→507, 508→509, 510→511, 512→513 (`InternalPlaceStartPosID` + `WaitingTime`) | JF `data034_kom/DLG_Map_Village.src.lua:991-1152` |
| Field 03 (40037) | L0 → `VMI_ELYSION` pos 516; dead end | BattleFieldData `:2158-2206` |

- F03 `REQUIRE_LEVEL` 85 / monster 88; enters and returns to `VMI_ELYSION`.
- `MapData` `VMI_ELYSION` row at `:357-370` (level 85–99, `BEFORE =
  VMI_RANOX`, inert quest keys). Cobo zone **18** for `VMI_ELYSION` and F03.
- Unlock chain continues from Tower: SkyCrossroads (`DungeonData:32268`,
  83–85) → Adrian (`32888`, 84–87) → Soless (`33120`, 87–99) → Memory
  (`33740`, 87–99), via `EXTRA_REQUIRE_DUNGEON_ID`.

### Elysion half: local map (Elysion page)

- Page buttons: `DLG_Map_Local_Elysion_Back_NEW.src.lua:39-88` (`MOVE_UP`
  dummy "0", `MOVE_DOWN` "1"); hero filter `:23-37` (Hero → out of scope).
- June C++: scroll `FUN_329378a0` (+`329379d0`, `3293c1e0`, `329366a0`,
  string "Elysion_Local"), LMUCM **18006/18007**
  (`LMUCM_LOCAL_MAP_MOVE_UP/DOWN`), `OnFrameMove` `FUN_3293a960`
  (`LocalMapMoveUp/Down`); guide window `DLG_Elysion_LocalMap_Guide.lua` opened
  by `FUN_329386b0` from `OpenLocalMapDLG` (`FUN_3293c440`) for 0x2719,
  closed by LMUCM **18008** (`FUN_31be4210(0x472, +0x58, +0x5c)`); page filter
  member +0x54 (`UpdateDungeonButtons` hides controls whose dummy string ≠
  current page); event flags 0x8d4/0x8d5 via `FUN_322aa950`
  (`JUNE_UI_PARITY_PHASES.md:370-385, 401-403`). March leaves comments at
  `X2LocalMapUI.cpp:379, 800, 818, 885, 1304, 1310, 1338-1339, 1399`.
- Elysion-page titles: `MAP_TITLE_ELYSION`, `MAP_TITLE_Elysion_SkyCrossroads`,
  `MAP_TITLE_Elysion_Adrian`, `SOLESS`, `TITLE_MEMORY` in
  `DLG_Localmap_Title_01.tga` (the `_01_A` variant is only for
  `NF_DE/ES/IT`). Stars `STR_ID_40226/40227/42846/43399`.

### Elysion half: village

- **Houses (12 new `HI_*`):** `HI_YUNO` 40092, `BERNARD`/`DURANDA`/`THEODOR`/
  `HUGO` 40131–40134, `BIRD_QUEST_00..04` 40135–40139,
  `ELYSION_RED/BLUE_TREE` 40145–40146; plus 9 shared houses with Elysion rows
  (`DLG_Map_House.src.lua` 8110-9365), `HI_GUILD_MARK`'s village row (Lanox
  omitted it, `M14/data034/DLG_Map_House.src.lua:4196-4197`, `4942-4944`).
- **Village NPCs (no unit scripts):** `NUI_YUNO` 10078, `BERNARD` 10143,
  `DURANDA` 10144, `THEODOR` 10145, `HUGO` 10146, `ELYSION_QUEST_BIRD0..4`
  10148–10152, `ELYSION_RED_TREE` 10194, `ELYSION_BLUE_TREE` 10195 (> March's
  `NUI_MAX_LIMIT` 10191 — A3 bumps it).
- **New `DLG_Map_House` keys:** `NPCMeshName2nd`, `AlphaBlend2ndMesh` (Yuno),
  `AlwaysShow`, `bOnlyClickable` (birds), `WAIT01_MOTION_NAME`,
  `bApplyAlphaTest`, `FRAGMENT_EXCHANGE` (trees), `TalkLoopAniNpc`.
- **NPC relation data:** JF `data034_kom/DLG_Map_House_NPC_RelationData.src.lua`
  (lines 28, 55, 84, 91, 667-824): `g_pQuestManager:AddVillageNpcRelationDataBy{Village,Quest}_LUA`
  + `VILLAGENPC_RELATIONDATA_TYPE.VRT_CHANGE`. It is the **only loader of the
  6 per-village `UI_*_ELYSION` dialogs** (data039: `UI_{ADAMS,ARIEL,BILLBOARD,
  CAMILLA,MU,RURIEL}_ELYSION`, plus `UI_NPC_BILLBOARD_ELYSION`,
  `UI_NPC_ELYSION_{RED,BLUE}_TREE`) and drives Yuno → Hyper Yuno (quest
  22227) → Nono (quest 22231).
- Loading art 0x4e29 → `HQ_LoadingVillage_Elision_01/02.tga` (JR data183).
  Music: `li_village_elysion001.ogg`, `field_elysion00*.ogg`,
  `li_field_elysion001.ogg` (JR `music/`).
- Seasonal village variants (`Elysion_Village_Festival`, `_XMAS`, `_XMAS_2`)
  are out of scope.

### Elysion half: engine gaps (A1b assigns owners and readiness)

- **Moving platforms (E5)** — Adrian, Soless: `LINE_MOVE_TYPE.LMT_SPIN` /
  `LMT_PATROL` (`Elysion_Adrian_Stage2_linemap:1273,1299`),
  `AddMovableLineData_LUA`, `TRIGGER_LINE_ACTION_TYPE.TAT_LINE_CHANGE_MOVABLE_DATA`
  (`Elysion_Soless_Stage0_LineMap:1309`), linemap keys `DIRECT_MOVE_DATA`,
  `TARGET_MESH_NAME`, `FOLLOWER_LINE_ID`, `STOP`, `USE_ABSOLUTE_COORDINATE`,
  `CAMERA_LAND_HEIGHT`, `CANDOWN`, `GetLineTypeByLineID_LUA`.
- **SimpleDLG gauges (E6):** `CreateSimpleDLG`, `DeleteSimpleDLG`,
  `GetSimpleDLG`, `SetShow_LUA`, `SetNumberInNumberStatic_LUA`
  (`_MoreDigit`, `_MoreParam`), `SetNumberInGageStatic_LUA`,
  `SetPercentValueInGageStatic_LUA`. Dialogs: `DLG_UI_ELYSION_SOLESS_AWAKEN_GAGE`
  (Soless boss), `…_HERBAON_GAGE` (`ELYSION_S_2STAGE_HERBAON:597`, Soless
  stage 2), **`…_OF_SUN_GAGE` → `ELYSION_M_SOLESS:3658` = the Memory boss**.
  Lanox deferred the same subsystem (`RANOX_PORT_PLAN.md:2989-3004`).
- **Dramatic messages (E6):** unit keys `DRAMATIC_MESSAGE0-4` +
  `IMAGE_FILE_NAME`, `BACK_COLOR`, `MESSAGE_COLOR`, `SOUND_FILE_NAME`,
  `MAX_ROW_CHAR`, `IMAGE_OFFSET_POSITION` (13 scripts);
  `DRAMATIC_MESSAGE_TYPE.DMT_LEFT/RIGHT` (`ELYSION_SOLESS_DRAMATIC:157`).
- **Bindings batch (E7):** `GetDyingStart`(b), `GetWorldObjectMeshByName`,
  `SetMovieDescriptionData`, `IsActiveMonster`(b), `FadeOriginWorldColor_LUA`,
  `LiveKeyCodeNPCNumExceptDying`, `GetCenterPositionLineIndex` (these first
  arrive with Atlas A4 — reuse), plus: `GetLiveUserUnitRandom`,
  `GetNowStateName`, `SetCameraDistance`(b, A4), `SetParticleSize`,
  `AddEffectSetDeleteCondition_LUA`, `AnimResume`, `AnimStop`(b),
  `CreateApplyAlphaMeshInstance_LUA`, `CreateApplyAlphaParticleSequence_LUA`,
  `GetNearestUnitInSpecificRange_LUA`, `GetSyncDummyValue`,
  `SetSyncDummyValue_Host`, `GetUnitID`, `SetPosion` (June typo — keep the
  name), `StartAlphaFade`, `StartFadeIn/OutCustomColor`,
  `AddStateCoolTime_LUA`, `SetStateCoolTime_LUA`,
  `FindRandomSelectUserUnitsFromRange_LUA`,
  `GetFindedRandomSelectUserUnits_LUA`, `GetRandomSelectUserUnitFromRange`,
  `Get/SetStoredGUUser`, `GetUnitIndex`(b), `IsSceneEnded`,
  `SetShowAllNPCExceptMe`, `SetShowHpSmallGage`, `SetUseExternalCondition_LUA`;
  from `ELYSION_SOLESS` bytecode (verified by byte context):
  `ChangeMotion_LUA`, `ClearStoredNPC`, `Play2DSound_LUA`,
  `PlayWorldAllSound`, `SetShowInvincible`(b), `SetShowWorld`; string-only
  (E0 confirms): `PlayAdditionalWorldAllSound`, `SetShowAdditionalWorld`.
  **Additional world:** WorldData `dataFileName_Additional` (`:3434`) —
  March's `AddWorldData_LUA` reads only 3 keys (`X2WorldManager.cpp:170-184`).
  Adrian keys: `DIRECT_MOVE_DATA`, `TARGET_MESH_NAME`;
  `MOTIONNAME_EFFECTSETNAME` (Lanox kept it inert — keep consistent).
  *(2026-10-16, june-engine W0: no longer inert by plan. june-engine W1 ports the key and June's mesh-anchored
  play engine-side; E9 only stages the Adrian data. `TAT_LINE_CHANGE_MOVABLE_DATA` stays E5's.)*
- **Buff-owned:** `GetBuffAccumulationCountNow` (`ELYSION_M_SOLESS` = Memory
  boss; possibly `ELYSION_SOLESS`) → **JUNE_BUFF Phase 10**.
  `SOLESS_TITLE_INVINCIBLE` (BBT 101 + a custom-function class) is a **title
  item**, not dungeon content — no block.
- **Cross-script global:** `IsReadyBreakAttack` is defined in
  `ELYSION_M_SOLESS` and called from other Memory scripts — load-order
  dependency, not an engine gap.
- **June bugs to transcribe verbatim:** `NUI_ELYSION_S_CIRCLE_CTR` typo
  (`Dungeon_Elysion_Soless_{N,H,E}.src.lua:732`; nil in June too),
  `LINE_TYPE.LT_INVALID` (`ELYSION_BOSS_LINE_A:107`, undefined in June too).
  Dead scripts with no `NPCTemplet` entry: `ELYSION_M_2STAGE_GATE`,
  `ELYSION_M_HARNIE`, `ELYSION_M_SOLESS2`, `ELYSION_S_0STAGE_FLOOR` — skip.

### Elysion half: files per line (June)

| Line | Dungeon scripts (data035) | Scenes + linemaps (data040) | Notes |
|---|---|---|---|
| SkyCrossroads | `Dungeon_Elysion_SkyCrossroads_{N,H,E}` | `Elysion_SkyCrossroads_{Stage0,1,2,BossStage}` + LineMaps (8) | `ELYSION_DUMMY_TITLE` placed N/H/E (`:415/569/890`), checked by `ELYSION_FAKE_OBJECT:277` — data only |
| Adrian | `…_Adrian_{N,H,E}` | `Elysion_Adrian_{Stage0..3,BossStage}` + LineMaps (10; `Stage2_linemap` lowercase) | moving platforms |
| Soless | `…_Soless_{N,H,E}` | `Elysion_Soless_{Stage0..3,BossStage}` + LineMaps, `Stage1_Hide` + LineMap, `BossStage_2Phase` (uses BossStage's LineMap) (13) | hidden stage `WI_SOLESS_STAGE1_HIDE` via `NUI_ELYSION_HIDE_STAGE_A/B` (`Soless_Normal:403`); phase-2 additional world; true/bad ending; boss script needs E0's decompile |
| Memory | `…_Memory_{N,H,E}` | `Elysion_Memory_{Stage0..3,BossStage}` + LineMaps (10) | finale, boss `NUI_ELYSION_M_SOLESS`, `CC_TIME` |

WORLD_IDs: SC 672–675, AR 706–710, SL 711–716, ME 727–731. Dungeon images:
`DLG_UI_Dungeon_Image12.dds` (new) for Soless/Memory. Loading screens
`HQ_LoadingDungeon_{Soless,Memory,…}_Pit_*` (JR data183).

---

## Lanox lessons → gates

| Lanox defect (source) | Gate in this plan |
|---|---|
| 267 plaintext luas packed (`RANOX_PORT_PLAN.md:5177`); `datai014`, `data002`, smog never repacked; staging ≠ live | Every data phase's **Done when**: compiled `1B 4B 4C 81`; staged in `march2014/` **and** `kom_contents/…_kom` + manifest; winning-mount check; pack list handed over; after the user packs, `elstools inspect` on the **live** kom matches |
| Mount order (`datai022` beats `data065`) | Census reports the winning archive for every staged name |
| Extractor truncation (`JUNE2017_ASSET_CLOSURE.md:3949`) | Size check of every copied June file vs June's kom index; short files are listed, never staged silently |
| `STR_ID` ceiling (`RANOX_PORT_PLAN.md:5194`) | Census: every `STR_ID_n` resolves in `StringID_def.lua` and has text in `general.ess` |
| nil hole truncates `HouseIDList`/`DungeonIDList` | Census: every element of every ID list resolves, in order |
| `NUI_EDEL` collision checked vs March only | Census: collisions vs March, vs the other incoming June IDs, vs shipped relocations |
| June renamed keys (`OtherPlaceID`) | Census: registry **key-name** diff (June keys vs March parser literals), per registry file |
| `DLG_Map_Village` `WORLD_ID`; `COMMON_NPC_LOCATION` per shared house | A5/E2 checklist: new world → `Village[]` row with `WORLD_ID` → a row in **every** shared house |
| Previous region's last field never scoped; `GetMapId` → Ruben fallback | A6 explicitly owns the Lanox F04-row portal and the matching `START_POS` 400 |
| Cobo zones; second warp implementation | A5 does both `CreateWarpDest`s and `MapData` zones together; offline reads the client `MapData.lua` |
| Hardcoded switches found only in play-test | A1a `region_reach.tsv`: every Sander/Ranox consumer gets a port/omit verdict up front |
| TET keys; textures only inside `.Y` meshes | A11: `assets.py` closure incl. MSZIP mesh-internal textures + TET-key census; boss-face keys (`BOSS_GAGE_FACE_TEX*`) per line |
| Trigger numbers invented; Lua half "not owed" | Values only from June `Enum.lua` + parser; C++ and Lua in the same phase; review checks equality |
| Unbound methods abort the chunk | Census binding audit over **every** ported lua, scene and UI file, against preprocessed March registrations |
| 0-HP NPCs (`ExtraNPCStat`) | Census NPC readiness: `Enum` + `NPCTemplet` + `NPCStat`/`NewNPCStat` per NUI |
| Effects silently missing | Per line: effect sets merged through the June effect pipeline; grep `RANOX_EFFECT_MISSING`-style lines after launch |
| Offline seed clobbers enums | A3 updates the seed in the same change |
| Decompiler drops assignments | June lua only from `june_src_fixed/`; `ljd_jun2017.py --check` on anything decompiled fresh |
| Plan avoided C++; "cosmetic" verdicts; premises as facts | A1a/A1b read C++ and Ghidra first; no "cosmetic" verdict without reading the consumer; kickoffs list premises to verify |

---

## Flags (all in `KTDXLIB/Always.h`, one delimited block)

| Flag | Phase | Notes |
|---|---|---|
| `SERV_IRUHADEV_LEVEL_CAP_99` | A2 | supersedes `LEVEL_CAP_80` (keep 80 in `#else`) |
| `SERV_IRUHADEV_ELYSION_PORT` | A3 onward | enum mirrors + region `case`s, as `RANOX_PORT` did |
| `SERV_IRUHADEV_ELYSION_SCRIPT_A` | A4 | Atlas-half bindings + keys |
| `SERV_IRUHADEV_ELYSION_SCRIPT_B` | E7 | Elysion-half bindings + additional world |
| `SERV_IRUHADEV_ELYSION_VILLAGE` | E1 | NPC relation, house keys, in-village warps |
| `SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION` | A7, E4 | local-map Elysion tab, bindings, scroll/guide |
| `SERV_IRUHADEV_JUNE_MOVABLE_LINE` | E5 | moving platforms |
| `SERV_IRUHADEV_JUNE_SIMPLE_DLG`, `SERV_IRUHADEV_JUNE_DRAMATIC_MESSAGE` | E6 | gauges, dramatic messages |
| `SERV_IRUHADEV_ELYSION_DEBUG` | any, removable | `[ELYDBG]` diagnostics |
| `SERV_IRUHADEV_CHAT_WARP_DEBUG` (existing) | A5, E2 | gains `atlas`, `elysion` |

---

## The exit metric every phase uses

`june2017_re/region_census.py` (built in A0) prints, per region half and per
line, every open item: unresolved `TABLE.MEMBER`, unbound methods/globals,
unread keys, unresolved `STR_ID`, nil holes, NPC readiness gaps, missing
assets, short/truncated assets, unstaged or uncompiled luas, manifest misses,
unknown item IDs, missing effect sets, and staged-vs-live size mismatches.
`--region atlas|elysion`, `--line AD|AC|TW|SC|AR|SL|ME|REST|FLD|VIL|UI`,
`--phase N`, `--live` (read the live koms).

A phase is done when:
- its scope drops out of `region_census.py --phase N`;
- the build returns rc=0 and its new literal is in the deployed `x2.exe`;
- everything it staged is compiled, in both staging places, with manifests;
- the pack list is handed to the user, and after packing the `--live` check
  passes.

Play-test confirmation is batched into A13 (Atlas half) and E15 (whole
region).

---

## Phase A0 — Docs, census v1

**Steps**
1. **No branch** (user 2026-10-16). Work on `main` in the main Trunk; it
   carries june-item-kim K2 (`f1549a6`), so its exe reads the live `data036`
   `item.kim` correctly. Never stash, add or commit another session's files.
2. Confirm this doc is on `main`. It is the single plan;
   `ELYSION_PORT_PHASES.md` is only a pointer to it.
3. Write `june2017_re/region_census.py` (see *Exit metric*), reusing what
   exists: `june2017_re/assets.py` (asset walk), `buff_census.py` (flag
   probing via `flagprobe.py`, preprocessed binding scan, `--json`),
   `census.py`. Inputs: `june_src_fixed/` only for June Lua; March's
   `march2014/` + `kom_contents/` (bytecode constants) + preprocessed C++.
   v1 covers the cheap checks: kom headers / manifests / live sizes,
   `TABLE.MEMBER`, bindings, `STR_ID`, nil holes, registry-key diff, NPC
   readiness, ID collisions (incl. relocations), item IDs (`BOSS_DROP_ITEM`,
   field drops vs `Item.lua`/`ItemTrans`), effect-set presence. C++ switch
   reachability is **not** in v1 — it consumes A1a's `region_reach.tsv` later.
4. Record the baseline in this doc. It must reproduce the ledger's numbers
   (41 WI, 7 VMI, 12 HI, 21 DI, 203 NUI, 0 collisions, 65 unbound names,
   393 effect sets, ≥ 2,071 assets). Where it disagrees, find out which is
   wrong before going on, and correct the ledger in place.

**Run with:** Sonnet 5.5, high effort. Tooling; light Opus 5.5 review of the
census method (spot-check 3 items each way).

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md in the main
Trunk root in full -- "Starting context", "Investigation ledger", "Lanox
lessons -> gates", "The exit metric" and "Phase A0". Never launch
X2_offline.exe without asking me first. Do NOT consult NEW_VILLAGE_PLAN.md,
NEW_VILLAGE_RANOX_IDENTIFIERS.md or the mods/lanox-offline branch, and tell
any subagent the same.

Effort: high.

Your task (Phase A0): work on main in the main Trunk (no new branch,
user 2026-10-16); never stash or commit someone else's files. Write
june2017_re/region_census.py (v1 = the cheap checks listed in Phase A0,
reusing assets.py / buff_census.py / flagprobe.py) and record its baseline in
the doc.

Premises to verify first: that the ledger's ID counts and the 0-collision
claim reproduce; that the staged EffectSet.lua really has no Elysion entries.

Done when: this doc is on main; census
baseline recorded and matching the ledger (or the ledger corrected in place).
Record a "Phase A0 -- Done (date)" note, then commit on main,
with this plan's STATUS rows in the same commit.
```

### Phase A0 — Done (2026-10-16)

*(Sonnet 5.5; tooling only: no game code, so no `SERV_IRUHADEV_` flag, no flagcheck, no build or deploy, no log tag.)*

- **Branch:** `main` in the main Trunk (user, 2026-10-16); no branch or worktree made. `ATLAS_PORT_PHASES.md` is the single plan on `main`; `ELYSION_PORT_PHASES.md` is its pointer.
- **Census:** `june2017_re/region_census.py` v1 (30-60 s; `--region`, `--line`, `--phase`, `--live`, `--json`, `--tsv`, `--no-probe`).
  Output: `june2017_re/region_census.tsv` (one row per item; OPEN = a gap a phase closes, INFO = June itself lacks it / nil in June), `region_census_baseline.tsv` (ledger vs census), `region_census_flags.tsv` (probed C++ flag cache).
  The region is **derived from June's own files**: `Dungeon_Elysion_*` rows of DungeonData, the Elysion rows of DLG_Map_Village / BattleFieldData (F01-F03 + the Lanox F04 direction row) / DLG_Map_House / WorldData, and the NUI closure through unit scripts (bytecode strings for `ELYSION_SOLESS`). Every category has two numbers: the **region set** and **missing now**.
  Checks: ids + collisions (March names, incoming twins, relocations, C++ end markers), NPC readiness, registry and file key diff, bindings (lua_tinker names, flag-resolved, calibrated), STR_ID, TABLE.MEMBER, ID-list nil holes, effect sets, item ids (item.kim), assets, kom header / manifest / mount, `--live` sizes. Not in v1: C++ switch reachability (A1a), receiver classes (A1b), mesh-internal textures (A11).
- **Baseline vs the 2026-10-03 ledger** (census = missing now + adopted into March since):

| Item | Ledger | Census | Verdict |
|---|---|---|---|
| WI | 41 | 40 | ledger slip: its own listed ranges are exactly the 40 census values (checked by value). 4 boss worlds adopted since (june-dungeons C3y-1b) |
| VMI / LMI / HI / DI | 7 / 1 / 12 / 21 | 7 / 1 / 12 / 21 | reproduces |
| NUI | 203 | 203 | total reproduces, **split corrected**: Appendix A/B list 201 values (incl. EVENT_00/01), the census adds `NUI_STORY_QUEST` 10142 + `NUI_TRANSCEND_QUEST` 10153 from the village houses (deferred to the quest plan, user 2026-10-16; INFO rows). Value-diff against the appendices: nothing missing either way. 8 adopted since (C3z-2) |
| ID value collisions | 0 | 0 | reproduces. Separately 9 missing IDs sit at/above a C++ end marker (A3 bumps them) |
| Unbound Lua names | 65 | 32 (26 + 6 from SOLESS bytecode; A0 review, was 31) | ~30 bound by june-effect C3z-4 since; calibration control is now `AddMovableLineData_LUA` |
| Effect sets | 393 | 416 referenced, 0 missing (A0 review, was 410) | **premise false**: staged `EffectSet.lua` carries June's full set since C11a (`c13f8115`) |
| Assets to adopt | >= 2,071 | 1,808 of 2,679 refs (A0 review, was 1,792 of 2,728) | 797 already in March (ledger 249); 74 names June itself does not ship |

  The key diff reproduces the ledger's named keys (`OtherPlaceID`, `OtherPlaceStartLineIndex`, `NPCMeshName2nd`, `AlphaBlend2ndMesh`, `AlwaysShow`, `bOnlyClickable`, `WAIT01_MOTION_NAME`, `bApplyAlphaTest`, `FRAGMENT_EXCHANGE`, `TalkLoopAniNpc`), `USE_DEFAULT_DIE_PARTICLE` in 85 scripts and `DRAMATIC_MESSAGE0-4` in 13. `IMMUNITY_DAMAGE_DATA_STOP_TIME`, `SOUND_PLAY_*`, `BUFF_GROUP_ID`, `SUPER_ARMOR_FRONT` now have C++ literals (ported since). BattleFieldData keys are read only by server code (`XSLBattleField.cpp`, not in the client build): the census marks them.
- **Ledger corrected in place:** WI 41 -> 40, the NUI split, and dated notes on unbound names, assets, effect sets.
- **Open at A0 close:** ids missing 272 rows (incl. 2 sentinels), NPC not ready 195, ID lists with nil holes 36 (unlock chain + local map), TABLE.MEMBER missing 13 (`LINE_MOVE_TYPE`, `DRAMATIC_MESSAGE_TYPE`, Hero DIs), registry keys 46, assets 1,808 (A0 review, was 1,792), unstaged region files 103 (staged 249, all live-equal by `--live`).
- **Findings for later phases:** `Elysion_Tower_Stage3.lua` is named by WorldData but June ships no file; `ELYSION_SOLESS.lua` is the only unit script absent from `june_src_fixed` (E0); `LINE_TYPE.LT_INVALID` and `NUI_ELYSION_S_CIRCLE_CTR` are nil in June too (JUNE_BUGS rows at the port); June's decompiled `EffectSet.src.lua` does not run under luajit (line 3), tools read it as text.
- **Review:** `kickoffs/atlas/A0-review.md` (Opus 5.5, light: 3 spot-checks each way).

### Phase A0 — Review (2026-10-16)

*(Opus 5.5, high. Census method only; no game code, no flag, no build.)* **Verdict: the method holds, with one census bug fixed in place.**

- **Bug fixed: SOLESS bytecode constants glued together.** `bytecode_text` scanned printable runs. A constant of 27 or more characters has a printable length byte (0x20 is a space, 0x2E is `.`), so it glued onto the constant before it. The glued names matched nothing, or matched as one bogus name: `soless_disappear_shadow02.y` (27 chars) was read as a "space list". The run-minus-first-byte workaround then added about 80 truncated `assets.absent` rows (`andemonium01.x`, `article_blur.dds`).
  The fix is `bytecode_consts()`: June bytes go through `ljconv.transcode` and then `tools/ljbc.py parse`, and the census takes every kgc string plus the string keys and values of table templates, each one exact (2,648 constants; no constant holds a real space list). The bindings scan and the NUI closure read the same constants.
  TABLE.MEMBER is two constants in bytecode (GGET T, TGETS M), so for bytecode-only files the pair is now inferred as `members.missing.bytecode` (E0 confirms). It adds 6 pairs, all of which resolve in March, so it adds no rows.
- **Second fix: row order.** `nui_closure` filled `R.nui` in set order, which follows the per-run string hash, so every re-run reordered `ids.missing` / `npc.ready` in the TSV. It now iterates `sorted(seen)`; two runs with different `PYTHONHASHSEED` give byte-identical TSVs.
- **What moved** (TSV diff, nothing else changed):
  - `assets.adopt`: 1,792 -> 1,808. The 16 new rows are 10 `Elysion_Soless_*.ogg` (data197), `DLG_UI_Elysion_Soless_awaken_gage.lua` and 5 `Soles_Voice_Dialogue0NN.ogg`.
  - `assets.absent`: 154 -> 74 (80 artefacts gone); `in_march`: 772 -> 797.
  - Effect sets: 410 -> 416 referenced, **still 0 missing**, so the "premise false" verdict stands.
  - `bind.unbound.bytecode`: +`SetShowWorld`, so 32 unbound in all. The baseline TSV, the Done-note table and the A1b kickoff are updated.
- **False-positive spot-checks** (row says OPEN; is it real?):
  - `ids.missing WI_MEMORY_STAGE1=728`: real. March `Enum.lua` has no 728 (726 is `WI_SPA_RANOX`); June `Enum.src.lua:2701` has it.
  - `bind.unbound GetUnitID` (`elysion_s_0stage_start` :163, called on `GetNPCUnit(i)`): real. There is no lua_tinker string in X2Lib/KTDXLIB, and no `function X:GetUnitID` anywhere in `june_src_fixed`. None of the 26 source names is Lua-defined.
  - `koms.unstaged Elysion_Village.lua`: real. It is absent from every `kom_contents/*_kom` and from `dev_overlay`, and June ships `Elysion_Village.src.lua`.
- **False-negative spot-checks** (should be flagged, is it?):
  - **NUI closure.** Every June NUI whose name or unit script matches Elysion/Atlas/Soless/Adrian/Herbaon/Harnie/Herna and is outside the closure was listed. There are 35, and they are exactly the ledger's deferred 35 (11 Henir, 10 CSM, 2 SET, 4 title/event, EMPEROR_FEAR, 3 SUMMUN, 4 templet-only). No region file names them by name. The numeric hits are line-map coordinates.
  - **TABLE.MEMBER** in `Dungeon_Elysion_Soless_Normal`: `CLEAR_CONDITION.CC_KILL_NPC_TYPE` resolves in March, so having no row is correct. The only tables outside June's Enum/DungeonEnum are the unit-local `INIT_PHYSIC` and, before the fix, glue artefacts.
  - **Assets** in `elysion_m_el_def`: `Arme_Ring2.dds` (data048) and `ADD_Cube_Map01.tga` (data147) are counted `in_march`. `ELYSION_M_EL_DEF_Dying.ogg` is INFO and correct: June ships only the element variants (`_FIRE` ...). `STR_ID_43385` is defined in March's `StringID_def.lua`.
  - **Effect sets** are matched as any quoted June templet name, with no `EffectSet_` prefix assumed.
- **Ledger corrections judged:**
  - WI 41 -> 40 is right: the ledger's own ranges sum to 40 (7+4+4+5+4+5+6+5).
  - NUI 203 is right as a composition (201 appendix values + `NUI_STORY_QUEST` / `NUI_TRANSCEND_QUEST`). The evidence is the value-diff against the appendices, not the matching total.
  - The effect-set premise is false (above).
  - Counting the **region set** and **missing now** separately is right: it is the only way to compare against a ledger that later phases have since moved.
- **Known limits, confirmed as stated (not fixed):**
  - Bindings are name matches; receiver classes are A1b's job.
  - `ELYSION_SOLESS` is read from bytecode (now exact constants), so its method and TABLE.MEMBER rows are inferences E0 confirms. `Elysion_Tower_Stage3` has no June file.
  - `--phase` is a line plus check-prefix filter (`keep()`).
  - Mesh-internal and particle textures are not followed (A11).
- Nothing for the user to decide. A1a and A1b can start (in parallel).

## Phase A1a — RE + C++ reachability map (both halves, read-only)

**Steps**
1. `X2Lib`/`KTDXLIB`/`X2ServerProtocol` grep for every consumer of
   `VMI_SANDER`, `VMI_RANOX`, `VMI_BATTLE_FIELD_SANDER_*`, `…_RANOX_*`,
   `LMI_SANDER`, `LMI_RANOX`, `WI_*SANDER*`, `WI_*RANOX*`, and every
   `*_END` marker in the ledger table. Resolve `#ifdef`s with the probed flag
   cache (`flagprobe.py`), not by reading nested conditionals.
2. For each site find June's version in Ghidra (strings, xrefs, switch
   values 0x3f0/0x4e29/0x2719/40035–40038/636–757) and record:
   `june2017_re/region_reach.tsv` columns = March file:line, function, what
   it decides, live/dead (probed), June address, June has Elysion case
   (Y/N/omitted), verdict (`port-whole` / `omit-with-comment` / `n/a`),
   owning phase.
3. Also map, for the E phases: local-map scroll/guide/filter/flags
   (addresses in the ledger), `AddVillageNpcRelationDataBy{Village,Quest}_LUA`
   + `VRT_CHANGE`, `DLG_Map_House` new keys, in-village warps
   (`InternalPlaceStartPosID`, `WaitingTime`).
4. Settle the open reachability questions: is `CX2FieldMiniMap` live; which
   `IsDungeonGate/Lounge` range June uses; does June's `IsVillageField`
   include `VMI_EN` (20010); which `*_END` consumers size arrays (a bump
   then changes memory layout).
5. Record confirmed June functions in `funcmap.tsv` (`confirmed`).
6. Add the deferred-quest addresses you pass over (`FUN_327e0f80`
   DungeonData quest key, `FUN_32501bb0` local-map templet, the direction-info
   reader if found) to *Deferred to the quest plan* — no deeper RE.

**Output:** `region_reach.tsv`, `funcmap.tsv` rows, an "A1a reachability
summary" table in this doc (one row per site, verdict + owner).

**Run with:** Opus 5.5, **xhigh** effort. Read-only. May be split into two
sessions (Atlas-owned sites / Elysion-owned sites) writing the same TSV.

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch main) in full -- especially "Starting
context", the ledger's "C++ sites" and "June-only engine" sections, and
"Phase A1a". Use the ghidra skill on unpacked_x2.exe; do not mutate the Ghidra
project without asking. Never launch X2_offline.exe without asking. Do NOT
consult NEW_VILLAGE_*.md or mods/lanox-offline; tell subagents the same.

Effort: xhigh. Read-only.

Your task (Phase A1a): build june2017_re/region_reach.tsv -- every March C++
site that enumerates regions (Sander/Ranox consumers, every *_END marker
consumer), each with its June counterpart, live/dead status from the probed
flag cache, and a port-whole / omit-with-comment verdict and owning phase.
Also map the E-phase engine sites: local-map scroll/guide/page
filter/event flags, AddVillageNpcRelationDataBy* + VRT_CHANGE, the new
DLG_Map_House keys and in-village warps. Settle CX2FieldMiniMap live/dead,
June's IsVillageField/IsDungeonGate/IsDungeonLounge ranges, and which *_END
consumers size arrays.

Premises to verify first: every file:line in the ledger's touch-list table.

Done when: region_reach.tsv covers every grep hit; funcmap.tsv rows added;
an "A1a reachability summary" is in the doc. Record a Done note, then commit on main.
```

### Phase A1a — Done

*(2026-10-16, Opus 5.5 xhigh. Read-only: no game code, no flag, no flagcheck, no build or deploy, no log tag.)*

- **Landed:** `june2017_re/region_reach.tsv` (70 rows, columns per step 2 plus `june_id` status and notes),
  `region_reach_hits.tsv` (the March inventory: 340 grep hits over X2Lib / KTDXLIB / X2 / X2ServerProtocol and the
  client-compiled `KncWX2Server/Common` headers, flag-resolved live/dead/comment), the two scripts that make them
  (`region_reach_hits.py`, `region_reach.py`), 40 `funcmap.tsv` rows (`Atlas A1a:`), 154 probed flags appended to
  `region_census_flags.tsv`. Coverage check: `python -I june2017_re/region_reach.py` ->
  `rows 70; inventory hits 340; covered 340; missing 0`.
- **Premises:** every ledger file:line re-read. Stale by a few lines (the code is as described): LoadFieldImage
  (`X2TFieldGame.cpp:4557`, Ranox cases 4616-4626), OpenFieldName (`:4736-4808`, not 4725-4797), LoadUI caller
  (`:4645`, not 4634), the WI_END handlers (`X2State.cpp:6840`, not 6789; `X2StateServerSelect.cpp:7379`, not 7371).
  The rest hold.
- **Settled (step 4):** `CX2FieldMiniMap` is compiled but never constructed (`CreateFieldMiniMap` has 0 callers), and
  June has removed the class (none of its `Minimap_*_Gate.tga` literals exist): nothing to port. June ranges:
  `IsVillageField` 20000-20010 (includes `VMI_EN`), `IsDungeonGate` 10000-10005 and `IsDungeonLounge` 30000-30006
  (both = March; `LOUNGE_RANOX` / `LOUNGE_ELYSION` sit outside in June too), `IsBattleFieldRest` 1000-1008. They come
  from June's out-of-line `IsVillage` (`32268dd0`). June's C++ markers are `VILLAGE_MAP_END` 20011, `BATTLE_FIELD_END`
  40045, `WI_END` 779 and `DI_END` 41530.
  **No `*_END` sizes an array, container, loop or memset**, and none is saved. The only layout limit is
  `NUI_MAX_LIMIT <= 65536` (`X2Item_Preprocessing.inl:1033`: NPC ids are WORD-packed in the item format).
- **Found from the June side** (no March token, so the grep could not see them):
  - **Code events 2260/2261/2262** (`IsEnableCode` `31e5a860`) gate the region's portals: Lanox F04 world 608 line 103
    and Elysion F01 world 638 line 53. They act in `IsNearPortalLineMap` `32d53810`, `CreatePotalParticle` `32d51960`
    and the `PhysicProcess_Portal*` pair `3201a2b0`/`3201a670`.
  - The same events hide Atlas Station from the Cobo list (`324627f0`; the row index still increments) and hide the
    Elysion local-map tab (`3293c6f0`). They mark Atlas / Tower dungeons "new" in `3293b5e0`.
  - Offline has no code events, so every gate stays open. Owners: A5, A6 and E4 port them as June has them.
    Checked: `SetCodeEvent` is called only from `Handler_EGS_UPDATE_CODE_EVENT_NOT` (`X2State.cpp:12876`, `:12879`),
    and `grep -rl CODE_EVENT X2Lib/Offline` finds nothing that sends it.
- **Quest gates** (`32322260` = IsCompleteQuest, 17 callers):
  - Both warp lists (`324627f0`, `32173120`) list the LMI_ELYSION rows only if templet `+0x70`
    (RequireCompletedQuestID 22218) passes `32322260`.
  - The local-map tab does the same (`3293c6f0 @3293cbe0`).
  - Ported verbatim, the gates would hide Atlas Station and Elysion from Cobo and the warp NPC, and lock the Elysion
    tab, forever offline. Verdict: port-whole, quest test omit-with-comment (decision 2).
  - Every reader is listed in *Deferred to the quest plan*.
- **June quirks the porting phase keeps** (a JUNE_BUGS row at the port):
  - `ResetMapPosAndSize` `32f1fd90` has no case for 1008 or 40038 (A5).
  - `GetFieldMapDlg` gives 40038 the `Elysion_02` dialog (E3).
- **Lanox misses fixed by the June ports:**
  - OpenFieldName: no Ranox case today.
  - The return-scroll ban: `321aa5d0@321b5752` bans 20006-20009.
  `ReLoadingVillageMap`'s loops miss Ranox too. June has no such loop and makes the dialogs on first open, so there
  is no gap. The BossRaid textures have Ranox 0-3 in June but no Elysion: outside this plan.
- **In-village warps (E1):** VillageParsing `324ff4b0` reads `WaitingTime` (StartPos +0x1c) and
  `InternalPlaceStartPosID` (+0x20). The readers are `323aa170` (is-internal), `323ac300` (move + Portal.ogg) and the
  village portal tick `3250a800`. March has neither key.
- **Ledger corrections:** `FUN_322aa950` is `std::map::find`. The event-flag test is `31e5a860`
  (`CX2InstanceData::IsEnableCode`). The guide window shows once per install: `checkfile.txt` is read with `"rb"` and
  written with `"wb"` in the cwd.
- **Open for the user:** nothing. Next: A1b (if not done), then `/x2-phase atlas A3` (kickoff `kickoffs/atlas/A3.md`).

#### A1a reachability summary

One row per site; the full 70 rows (every March line they cover, June evidence, notes) are in `june2017_re/region_reach.tsv`.
`port-whole` = June's whole function under the owner's flag, June-only cases whose enum March lacks (1500 `VMI_SPA_RUBEN`,
20010 `VMI_EN`, 25000 `VMI_GUILD_AGIT`, raid stages) omitted with a comment; `n/a` = nothing to port.

| Site (March) | June | Region case in June | Verdict | Owner |
|---|---|---|---|---|
| `IsValidWarpField` `X2LocationManager.cpp:1561` | `324f1e70` | Y (ranges, not a case list) | port-whole | A5 (E2 checks 20009) |
| `LoadFieldImage` `X2TFieldGame.cpp:4557` | `323ad5f0` | Y (Atlas_Pit_Rest, Elision art) | port-whole | A5 |
| `OpenFieldName` `X2TFieldGame.cpp:4736` | `323b1320` | Y | port-whole | A5 |
| `GetFieldMapDlg` `X2WorldMapUI.cpp:684` | `3225e1f0` | Y (40038 -> Elysion_02, quirk) | port-whole | A5; dialogs A6 / E2 / E3 |
| `UpdateNaviMap` `X2WorldMapUI.cpp:2178` | `3225cad0` | Y | port-whole | A5 |
| `ResetCameraMatrix` `X2FieldMapBase.cpp:120` | `32f1fec0` | Y | port-whole | A5 |
| `ResetMapPosAndSize` `X2FieldMapBase.cpp:535` | `32f1fd90` | omitted (no 1008, no 40038: keep) | port-whole | A5 |
| Cobo `InitWarpList` / `CreateWarpDest` `X2StateMenu.cpp:6139` / `6185` | `3245a1e0` / `324627f0` | Y (1008 hidden under event 2260) | port-whole; quest test omit-with-comment | A5 (E2: 20009) |
| warp-NPC `InitWarpList` / `CreateWarpDest` `X2UIInventory.cpp:13931` / `13977` | `32166ae0` / `32173120` | Y | port-whole; quest test omit-with-comment | A5 (E2: 20009) |
| return-scroll ban `X2UIInventory.cpp:6599` (`OnRClickedItem`) | `321aa5d0@321b5752` | Y (bans 20006-20009) | port-whole (ban block) | E2 |
| `WarpByCommand` / `FieldWarpByCommand` `X2Main.cpp:13480` / `13515` | `31d71000` / `31d71130` | Y | port-whole | A5 / A6 |
| chat warp debug `X2ChatBox.cpp:1011` | - | n/a (local debug) | add `atlas` / `elysion` | A5 / E2 |
| `IsVillageField`, `IsBattleFieldRest`, `IsVillage` `X2LocationManager.h:488-544` | `32268dd0` | June 20000-20010, 1000-1008 | port-whole (June ranges) | A3 |
| `IsDungeonGate` / `IsDungeonLounge` `X2LocationManager.h:508` / `516` | inline | = March | n/a | - |
| marker consumers: `WI_END` x3 field ACKs, `VILLAGE_MAP_END` x2 couple summon, `BATTLE_FIELD_END` x5 (login, rejoin, party, char select, village portal), `NUI_MAX_LIMIT` x2, ASSERTs x4, `IsBattleField` / `SEnum::Is*ID` (no client caller) | `31efb3a0`, `325094b0`, `3256a180`, `324bdd50`, `325712b0`, `323b5970`, ... | June's own values | n/a (A3 bumps the markers; until then Elysion fields fail every test) | A3 |
| `UpdateLocalMapDLG` / `UpdateDungeonButtons` (June copies) `X2LocalMapUI.cpp:750` / `1232` | `3293c6f0` / `3293b5e0` | Y (LMI 0x2719, events 2260/2261; the tab also tests RequireCompletedQuestID @3293cbe0) | port-whole; quest test omit-with-comment (A7) | A7 (tab, Atlas page) / E4 |
| `CreateLocalMapDLG` `X2LocalMapUI.cpp:1140` (`DI_END`) | `3293ab50` | n/a (region DIs < 41311) | n/a | A3 (sentinel) |
| `OpenLocalMapDLG` + page scroll + guide | `3293c440`, `329378a0`, `3293a960`, `329386b0` | Y | port-whole | E4 |
| portal gates (no March token): `IsNearPortalLineMap` `X2BattleFieldGame.cpp:514`, `CreatePotalParticle` `:329`, `PhysicProcess_Portal*` `X2GUUser.cpp:24500` | `32d53810`, `32d51960`, `3201a2b0` / `3201a670` | Y (events 2260-2262) | port the gates (inert offline) | A6 |
| in-village warps (no March key) | `324ff4b0`, `323aa170`, `323ac300`, `3250a800` | Y | port-whole | E1 |
| `HouseParsing` + NPC relation | funcmap rows (Effect C9h-2e / C9h-5) | Y | port-whole | E1 |
| `GetBoardStartQuestList` `X2QuestManager.cpp:1144` | `3232fd70` | Y (1008 -> itself) | deferred | quest plan |
| `ReLoadingVillageMap`, `CX2FieldMiniMap`, BossRaid texture / camera, `STATE_CHANGE_FIELD_REQ` `31f12630`, `LoadUI` `323b0f20`, `OpenFieldMap` `322633c0`, `CX2PartyUI::OpenLocalMapDlg` `322b5970`, sentinels, dead and comment hits, enum defs (A3), offline seed (A3) | - | - | n/a | - (A3 for defs and the seed) |

---

## Phase A1b — Script-engine map (both halves, read-only)

**Steps**
1. Re-run the binding audit with **receiver classes** (Lanox's
   `SetShowObject` landed on the wrong class): for each of the 65 names, which
   June class registers it (the June `class_def` site), and whether March has
   that method on that class.
2. For every name and every unread key in the ledger (unit-script keys,
   linemap keys incl. `CHANGE_LINE_TYPE_AFTER_ALL_ACTIVE_NPC_KILLED`,
   `DIRECT_MOVE_DATA`, `FOLLOWER_LINE_ID`, `STOP`, `USE_ABSOLUTE_COORDINATE`,
   `CAMERA_LAND_HEIGHT`, `CANDOWN`, `TARGET_MESH_NAME`;
   `dataFileName_Additional`), and the new tables (`LINE_MOVE_TYPE`,
   `DRAMATIC_MESSAGE_TYPE`, `TRIGGER_LINE_ACTION_TYPE.TAT_LINE_CHANGE_MOVABLE_DATA`),
   record: June address, parse layout / argument types, the store it writes,
   every read site (address + pseudocode note), the closest March analog, the
   owning phase (A4, E5, E6, E7, buff P10), and **"transcription-ready"** or
   **"Opus-only"** (the JUNE_BUFF bar: parse layout + store + all read sites
   written down).
3. Check `TAT_LINE_CHANGE_MOVABLE_DATA`'s June value against March's
   `SERV_IRUHADEV_JUNE_LINEMAP_TRIGGER` numbering (March was renumbered to
   June in Lanox Remaining P10).
4. Decompile-check: `ELYSION_SOLESS.lua` has no `june_src_fixed` copy; list
   its extra names (byte-context verified vs string-only) for E0 to confirm.

**Output:** `june2017_re/region_engine.tsv` + an "A1b engine map" table in
this doc. **Every name gets an owner** — no orphans.

**Run with:** Opus 5.5, **xhigh** effort. Read-only. May be split (A1b-1
Atlas names + shared keys, A1b-2 Elysion names).

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch main) in full -- especially "Starting
context", the ledger's "June-only engine" section and "Phase A1b". Also read
JUNE_BUFF_PORT_PHASES.md "Model selection" for the transcription-ready bar.
Use the ghidra skill; do not mutate the project without asking. Never launch
X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline; tell subagents the same.

Effort: xhigh. Read-only.

Your task (Phase A1b): for each of the 65 unbound Lua names (verify the
receiver class), every unread unit-script/linemap/world key, and the new enum
tables in the ledger, record June address, parse layout, store, read sites,
March analog, owning phase and transcription-ready vs Opus-only, in
june2017_re/region_engine.tsv and an "A1b engine map" table in the doc. Check
TAT_LINE_CHANGE_MOVABLE_DATA against March's JUNE_LINEMAP_TRIGGER numbering.

Premises to verify first: the ledger's (b) "method already exists" claims
(name match only so far).

Done when: every name/key has an owner and a readiness verdict. Record a Done
note, then commit on main.
```

### Phase A1b — Done

*(2026-10-17, Opus 5.5 xhigh. Read-only: no game code, no flag, no flagcheck, no build or deploy, no log tag.)*

- **Landed:** `june2017_re/region_engine.tsv` (92 rows: every census `bind.*` / `keys.*` / `members.*` row, the
  SOLESS byte-context names, the ledger's DungeonData keys and the names bound since the ledger), 30 `funcmap.tsv` rows
  (`Atlas A1b:`), the table below. Verdicts: 29 transcription-ready, 27 Opus-only, 21 done, the rest owned elsewhere or
  nothing to port. Phase order holds: no E-owned row is called from an Atlas-line file (`StartFadeOutCustomColor`'s `UI`
  tag is the dead `ELYSION_M_2STAGE_GATE.lua`, which the census tags by name only).
- **Premises.** Every ledger "(b)" holds: `GetAliveActiveNPCCount`, `AnimStop`, `GetUnitIndex` and `SetShowInvincible`
  exist on June's class or its March base, and their bodies match June (binding only). `GetDyingStart`, `IsActiveMonster`
  and `SetCameraDistance` were bound by june-effect C3z-4. Receivers are typed per call site (c3z4 `type_calls`): every
  typed site lands on June's registering class (the untyped counts are in the TSV; `IsSceneEnded`'s 20 all go through
  `GetSlideShot()`). `SetShowHpSmallGage` needs both the NPC and the user binding.
  Spot-checked in Ghidra: `IsActiveMonster` (byte +0x872c), `GetDyingStart` (+0xdac), `GetStoredGUUser` (+0x8d64).
- **Census corrections.**
  - `bind.unbound.bytecode` lists 5 SOLESS names. Byte context (`TGETS` after `MOV A+1, B`, via ljconv + ljbc) gives
    29 unbound calls, 23 of them SOLESS-only. The census drops a name unless some June source calls it.
  - The 413 `unit:` keys are mostly `CT_*` / `CF_*` state-callback names and state names. Those are strings looked up by
    name, so they are false positives, like `BBT_*` / `BFT_*` (inline buff-factor tables), `LINE_NUM` (March linemaps
    have it too, and neither client reads it) and `ENDING_SPEECH0` (March reads `BeginTable( "ENDING_SPEECH", i )`).
  - `FindRandomSelectUserUnitsFromRange_LUA` and `GetNearestUnitInSpecificRange_IncludeSameTeam_LUA` are CX2Game methods.
    `c3z4_binder_sites.py` labels them "RenderParam"; their `class_def` pushes CX2Game's class name 33777790.
- **Found.**
  - E6's SimpleDLG half is already in (june-effect C3z-4s: `CX2SimpleDLG_WithLuaFunc`, `X2Game.cpp:6444`). E6 is left
    with dramatic messages and the gauge dialogs.
  - `SetPosion` is not bound in June. No binder string decodes to it and the binary has no such text, so the call is a
    Lua error in June too: E10 does not bind it, a JUNE_BUGS row instead.
  - Six keys are June data typos June never reads (`STOP_TIME_ATK`, `ANIM_SPPED`, `SHAW`, `LLOW_DIR_CHANGE`,
    `CAINVINCIBLE`, linemap `CANDOWN`). `TARGET_FRONT_ME`, `AFTER_IMAGE_DEST_ALPHA_BLEND` and `dataFileName_XMAS` are
    absent from the binary. Keep all of them verbatim, with a JUNE_BUGS row from the stager.
  - `DungeonData.LU` / `.CIEL` are ENDING_SPEECH tables for June's Lu / Ciel. They go to the char-port checklist.
  - The DROP keys are server-side (`XSLDungeonSubStage.cpp:80`).
  - `Add/SetStateCoolTime_LUA` take an **int** (`fild`), so a fractional value truncates in June.
  - `SetSyncDummyValue_Host` sends a new P2P packet (0x1d). Whether its receive side comes along offline is E7's call.
- **Step 3:** `TAT_LINE_CHANGE_MOVABLE_DATA` = 8 in June `Enum.src.lua:1983` and in `AddTrigger_LUA`'s line case 8.
  March's C++ enum stops at `TAT_LINE_CHANGE_TYPE` = 7 (`KTDGLineMap.h:112`). `JUNE_LINEMAP_TRIGGER` renumbered only
  the etc. enum, and `Enum.lua:1648-1658` has 0..7 plus a comment for 8. The numbering agrees and 8 is free (E5).
- **Step 4, for E0:** SOLESS's unbound calls by byte context are the 23 `SL` / SOLESS rows of the TSV, plus `SetShowWorld`
  (the census missed it). The ledger's string-only pair `PlayAdditionalWorldAllSound` / `SetShowAdditionalWorld` is
  byte-confirmed. `luasrc.py decompile` timed out at 900 s on the file: give E0 a longer `--timeout`.
- **Open for the user:** nothing. Next: `/x2-phase atlas A3` (kickoff `kickoffs/atlas/A3.md`), then A4 reads the A4 rows.

#### A1b engine map

One row per name or group; addresses, layouts, stores and every read site are in `june2017_re/region_engine.tsv`.
"ready" = transcription-ready (the JUNE_BUFF bar); "Opus" = Opus-only.

| Name / key (June class) | June | Verdict | Owner |
|---|---|---|---|
| `GetAliveActiveNPCCount` (CX2Game) | `322d0ae0` | ready: bind, body = `X2Game.cpp:6758` | A4 |
| `GetNationFlag` (CX2Main) | `31e4e690` | ready: bind `X2Main.h:1577` | A4 |
| `NOT_CHECK_INDICATOR` (NPC +0x8fa8) | `327a0fa0`, reader `32f099f0` | Opus (June's filter = `X2MonsterIndicator.cpp:190-202` plus this key and an unnamed AI byte +0x34) | A4 |
| `SetChangeUVPoint` (CPictureData) | `31c9a7d0` | Opus (U0 left it a candidate) | A4 |
| `USE_DEFAULT_DIE_PARTICLE` (NPC +0x8fd9) | `327a0fa0`, `3277ec60`, `32778d40` | Opus ("DieLight" once at death) | A4 |
| `CHANGE_LINE_TYPE_AFTER_ALL_ACTIVE_NPC_KILLED` (line +0x150) | `31c705c0` | Opus (reader not found) | A4 |
| `CAN_NOT_PASS_SPECIFIED_BONE` (state) | `32784c00` | Opus | A4 |
| `TARGET_FRONT_ME`, `AFTER_IMAGE_DEST_ALPHA_BLEND` | - | nothing (June never reads them) | A4 |
| `AddMovableLineData_LUA` + `DIRECT_MOVE_DATA`, `TARGET_MESH_NAME`, `FOLLOWER_LINE_ID`, `STOP`, `USE_ABSOLUTE_COORDINATE` | `31c75470`, `328004a0` case 8 | Opus | E5 |
| `CAMERA_LAND_HEIGHT` (line +0x158) | `31c705c0` | Opus | E5 |
| `GetLineTypeByLineID_LUA` (CX2Game) | `322d0800` | ready | E5 |
| `LINE_MOVE_TYPE`, `TAT_LINE_CHANGE_MOVABLE_DATA` = 8 | Enum `:17823`, `:1983` | ready (A3 Lua tables) | E5 |
| `DRAMATIC_MESSAGE0-4` + 17 subkeys, `DRAMATIC_MESSAGE_TYPE` | `322e9e00`, `Playing_Dramatic_Message_LUA` | Opus | E6 |
| SimpleDLG (`Create/Get/DeleteSimpleDLG`, 12 dialog methods) | `322def10`.. | done (C3z-4s) | E6 data only |
| `AnimStop`, `AnimResume`, `GetUnitIndex`, `SetShowInvincible`, `GetUnitID` | `32757b10`, `32758810`, `31aa4d30`, `3208dd40`, `31e4d890` | ready (bind or one-line getter) | E7 |
| `SetStateCoolTime_LUA`, `AddStateCoolTime_LUA` | `3275eb70`, `3275ebc0` | ready (`m_vecStateRuntime[id].m_fStateElapsedTime`) | E7 |
| `AddEffectSetDeleteCondition_LUA` | `3227a670` | ready (June's 5-case switch) | E7 |
| `FindRandomSelectUserUnitsFromRange_LUA`, `GetFindedRandomSelectUserUnits_LUA`, `GetRandomSelectUserUnitFromRange`, `GetUserIndexByUserUID` | `322dd990`, `322d0030`, `322dc3c0`, `322cfc00` | ready | E7 |
| `IsSceneEnded` (CX2SlideShot +0x88) | `325c4a50` | ready (new read-and-clear flag) | E7 |
| `SetShowHpSmallGage` (NPC +0x8fb9, user +0x9528) | `31e4d870`, `31e4d150`, reader `32e5d580` | Opus (which March draw function holds the skip) | E7 |
| `SetParticleSize`, `SetShowWorld`, `PlayWorldAllSound` | `31e4f3f0`, `322cc040`, `322cc060` | ready | E7 |
| `Clear/Get/SetStoredNPC` (observer +0x8d6c), `Get/SetBlackHoleData_Speed/_Range` | `31e57990`.., `31e4c480`.. | ready | E7 |
| `CAN_PASS_UNIT_AT_FIRST_FRAME` (NPC +0x8fd8) | `327a0fa0`, Init `3277c030` | ready | E7 |
| `StartAlphaFade`, `StartFadeIn/OutCustomColor`, `Get/SetSyncDummyValue(_Host)`, `GetNearestUnitInSpecificRange_LUA` (+`_IncludeSameTeam`) | `3275a240`, `31ce6270`, `32778ea0`, `322d55b0` | Opus | E7 |
| `SetThornsInfo`, `*SimpleTimerCount`, `VectorUidType_*`, `StopEffectSetSilent_LUA` | `3226e350`, `32299890`, `3275d090`.., `321db020` | Opus | E7 |
| Additional world (`dataFileName_Additional`, `SetShowAdditionalWorld`, `PlayAdditionalWorldAllSound`) | `32805770`, `322cf8f0`, `322cf930` | Opus | E7 |
| `SHOW_GAGE`, `INVINCIBLE_ATTACK_TYPE` (state), `HEIGHT_CHECK_RANGE` (AI) | `32784c00`, `32efdb50` | Opus | E7 |
| `SetPosion`; `LT_INVALID`, `NUI_ELYSION_S_CIRCLE_CTR` | - | keep verbatim (June errors / nil too) | E10 |
| typo keys (6, see TSV) | - | keep verbatim | stagers A8-A9, E8, E10, E11 |
| 21 names bound since the ledger (C3z-4 / C3z-4s / buff P10) | see TSV | done, class verified | - |
| `AddSubtitleData`; `MOTIONNAME_EFFECTSETNAME`; `DI_*_HERO_NORMAL` | | owned elsewhere | june-engine U1; W1; june-dungeons C3y-5 |
| `LOCAL_MAP_UI_CUSTOM_MSG` 18006-18008; `WI_GUILD_AGIT`, `WI_SPA_RUBEN` | June Enum | ready | A3 (+E4) |
| `DLG_Map_House` / `DLG_Map_Village` keys; `BattleFieldData` keys; `DungeonData.LU/CIEL`; DROP keys | | mapped by A1a; server-read; char ports; emulator | E1 / A5; A6b; checklist; offline |
| ledger DungeonData keys: March reads 4; `additionalTicket` / `m_bForbidEnterDungeon` sit only on Hero rows; `Elysion_Tower_Stage3.lua` (`bind.nosource`) | `327e0f80` | nothing here | C3y-5; A10 (none) |

---

## Phase A2 — Level cap 99

**Precondition (user decision 2026-10-03, recorded from JUNE_PORT_BACKLOG_ORDER.md):**
`MARCH_2014_MIGRATION.md` Phase 7 (bringing the pre-March save forward) must be done
before A2 runs, so the forward-only EXP migration starts from a known save state.

**Steps**
1. **Sweep every 80-bound.** Grep and probe: `USE_MAXLEVEL_LIMIT_VAL` /
   `g_iMaxLevel` consumers, `CX2OfflineStatTable::MAX_LEVEL`,
   `ReserveMemory`, the compile-time typedef in `Handlers_Room.cpp:3052`,
   level-indexed arrays, UI level clamps, the offline level-up and level-up
   scroll paths, `CX2InstanceData::m_iMaxLevel`. Add
   `SERV_IRUHADEV_LEVEL_CAP_99` (keeps `LEVEL_CAP_80` as `#else`).
2. **ExpTable:** adopt June's `data036_kom/ExpTable.lua` whole (decision 5),
   compiled and staged as a patch (`march2014/data036/ExpTable.src.lua`).
3. **StatTable 81–99 (invented, user-authorised):** per class × per stat,
   recover March's own generator from rows 1–80 (piecewise-linear; find the
   segments and the rounding: integers for Atk/Def, .5 steps for HP). **Prove
   it reproduces 1–80 exactly** (or record the max residual per class/stat
   and stop if it is not negligible). Extrapolate 81–99 with the last
   segment's per-level delta and the same rounding. Raise `ReserveMemory` to
   99 for every class. Write the generator as `june2017_re/cap99_gen.py` so
   it reruns. Every generated row carries a `-- CAP99 generated` comment;
   `X2OfflineStatTable` logs `[CAP99] StatTable rows 81-99 present for N
   classes` once at load (or which classes lack them).
4. **NpcExpTable 81–99 (invented):** anchor to the **adopted** June curve:
   compute kills-per-level k(L) = J.NEED(L) / npcExp(L) over 70–80 using
   March's rows, extrapolate k to 81–99, set npcExp(L) = J.NEED(L) / k(L),
   round like March. Report kills-per-level at L50/70/80/90 under the old
   (March) and new (June) curves in the Done note so the change in pace is
   visible. Label rows; log once at load.
5. Both server-side files live in `KncWX2Server/ServerResource/US/` and are
   loaded by the client offline (`X2OfflineStatTable.cpp:27`,
   `X2OfflineDropTable.cpp:13`): keep the loaders' existing degrade paths
   and name both files for packing into `data036.kom` at hand-off. Preserve
   UTF-8 BOM and CRLF byte-for-byte.
6. **EXP save migration** (one-way, `X2OfflineDB` ladder): for each unit,
   keep level L and the fraction of progress through it:
   `new = J.TOTAL(L) + (old − M.TOTAL(L)) / M.NEED(next) × J.NEED(next)` —
   settle the exact index convention against both tables first and write it
   down. Clamp a level-80 unit's progress. Log one
   `[CAP99] migrated unit <uid> L=<L> exp <old> -> <new>` per unit (UIDs are
   `__int64`, print `%I64d`). Before the build is deployed: copy
   `els_db.sql` (+`-wal`/`-shm` if present) to a dated backup in the game dir
   and **ask the user before the launch that runs the migration.**

**Done when:** the census shows no 80-bound left; generator proof recorded;
build rc=0 with the `[CAP99]` literal in the exe; ExpTable compiled + staged;
both server-side files named for packing; migration code reviewed; the user
has been asked before the migrating launch.

**Run with:** Opus 5.5, high effort. Required review (Opus 5.5): generator
proof, migration formula against both tables, every 80-bound site.

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch main) in full -- "User decisions" 4-5,
"Starting context", the ledger's "Level cap" section and "Phase A2". Never
launch X2_offline.exe without asking -- and this phase's build migrates the
save, so back up els_db.sql first and ask before that launch. Never delete
els_db.sql. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase A2): raise the level cap to 99 behind
SERV_IRUHADEV_LEVEL_CAP_99 (sweep every 80-bound); adopt June's ExpTable.lua
whole; generate StatTable and NpcExpTable rows 81-99 with
june2017_re/cap99_gen.py exactly as Phase A2 describes (prove the StatTable
generator reproduces rows 1-80; anchor NpcExpTable to the adopted June curve;
report kills-per-level at L50/70/80/90 old vs new); add the one-way EXP save
migration to X2OfflineDB's ladder with [CAP99] logging.

Premises to verify first: the ledger's StatTable/NpcExpTable/ExpTable shapes
and values; that X2OfflineStatTable/X2OfflineDropTable load these files the
way the ledger says.

Done when: Phase A2's "Done when" holds. Record a Done note (incl. the
kills-per-level table), then commit on main.
```

### Phase A2 — Done

**2026-10-16, Opus 5.5.** Flags `SERV_IRUHADEV_LEVEL_CAP_99` (`LEVEL_CAP_80` block kept, so flag-off = 80) and
`SERV_IRUHADEV_EXP_GAUGE_MAX_LEVEL_FIX` (the user's "-1.$%" request, see 6). Ledger corrections are under "Level cap".

1. **80-bounds** (sweep: X2Lib incl. Offline, KTDXLIB, X2, X2ServerProtocol, Common). The four real bounds are now 99:
   - the cap (`Always.h`, end);
   - the typedef check (`Handlers_Room.cpp`, cap-99 twin in front of it; the build passing proves 99 reached `X2Game.h`);
   - `CX2OfflineStatTable::MAX_LEVEL`;
   - `CX2OfflineDropTable::MAX_NPC_LEVEL`.

   Everything else reads `g_iMaxLevel` or the ExpTable: ApplyDungeonReward, the level-up scroll, `CX2InstanceData`,
   `X2Game.cpp:9388`, the personal shop. Kept on purpose, being neither 80-bounds nor in scope:
   `X2BuffBehaviorTemplet.cpp:1339` (unit level clamped at 90 in a buff formula), `X2UISkillNote.cpp:62` (20..70
   list), the item-level stone tiers. Server-only 80s (`GameSysVal.cpp:776`, `KStatisticsLocalLog.h`) are untouched.
   New guard: the level-up loop stops at the ExpTable's last row and logs `[CAP99] ERROR` once. A cap-99 exe over
   March's ExpTable would otherwise climb a unit to 99 on one kill and pin EXP 0.
2. **ExpTable**: June's whole, `luasrc.py adopt-june ExpTable.lua` (ljconv bytecode; record
   `june2017_re/june_adopt_20261017_001004.tsv`). `PACK OK data036` (~1 changed vs live: `ExpTable.lua`).
   **Not copied yet.**
3. **StatTable 81-99** (`june2017_re/cap99_gen.py stat`). The model is `floor(a + r·L)` per class × stat, greedy segments
   from 80 down, r = the shortest decimal that fits (HP fitted doubled; class 115 keeps its truncated whole-number HP).
   - **Proof 1:** max residual **0** over all 250 curves (4000 rows). Shapes: 143 one line over 1-80; 100 with level 1 on
     its own (the studio's tutorial adjustment); 12 split at L22 (a float tie of the 9.9 curve).
   - **Proof 2** (independent): extrapolation vs the studio's own commented-out rows 81-99 of classes 110/111 matches
     **188/190** values; AtkMagic L92 is 1 low (a float tie the 1-80 rows cannot pin; review: the lowest-intercept choice, see "Phase A2 — Review"; midpoint since then, 190/190).
   - Classes 110/111 therefore ship the studio rows (uncommented copies, labelled); the other 48 ship the extrapolation
     (`-- CAP99 generated`). ReserveMemory raised to 99 on 49 lines, each `-- CAP99: was 80`.
   - BOM, CRLF and no EOF newline are preserved. `--write` is idempotent (rerun: "unchanged").
4. **NpcExpTable 81-99** (`cap99_gen.py npc`). k(L) = kills of own-level mobs from L to L+1 = J.NEED(L+1) / npc(L),
   least-squares over 70-80 (k = −1328.41 + 32.123·L), npc(L) = round(J.NEED(L+1) / k(L)). Strictly increasing 1-99.
   L90 doubles (11,810 → 23,013) because June's NEED(91) doubles. Raw table, before the offline EXP boost:

   | L | old (March ExpTable + npc) | new (June ExpTable + npc) |
   |---|---|---|
   | 50 | 2940.6 | 521.7 |
   | 70 | 12047.0 | 934.2 |
   | 80 | n/a (cap) | 1270.8 |
   | 90 | n/a | 1562.7 |

5. **Server Lua for the user to pack** (XOR + data036, manual): `KncWX2Server/ServerResource/US/StatTable.lua`,
   `KncWX2Server/ServerResource/US/NpcExpTable.lua`. The degrade paths are unchanged. At load, `[CAP99] StatTable rows
   81-99 present for N of 50 classes` and `[CAP99] NpcExpTable rows 81-99: N of 19` name what is missing.
6. **EXP save migration**: schema **v16** (`CX2OfflineDB::MigrateExpToJuneTable`), constants in `X2OfflineCap99Exp.h`
   (`cap99_gen.py exp --header`). The DB opens before `LoadEXPTable`, and March's table is gone after adoption.
   - Formula: TOTAL(L) reaches L, NEED(L) = TOTAL(L) − TOTAL(L−1), f = (old − M.TOTAL(L)) / M.NEED(L+1) clamped [0, 1),
     new = J.TOTAL(L) + ⌊f·J.NEED(L+1)⌋ in `__int64`. L80: f = 0, so new = J.TOTAL(80) = 70,016,100.
   - The rung needs v14 + v15 (`#error`). One `[CAP99] migrated unit <uid> L=<L> exp <old> -> <new>` per unit, then a summary.
   - A Python replica on the live save keeps all 27 units on their level. Examples: L80 866,438,000 → 70,016,100;
     L72 unit 18 → 39,172,196; L70 → ~33.72M.
   - **Backup** `db_backup/els_db_20261017_000918_pre_pre_A2_cap99_exp_migration.sql`.
   - `tools/createchar.py` / `elsdb.py setchar` now take EXP from `tools/data/june_exptable.tsv`, levels 1-99.
   - Gauge fix: `UpdateExpGageBar` divided by the next level's NEED, and at the cap that row is missing, so 0/0 = NaN
     prints "-1.$%". At the cap, or with no next row, the bar is full and reads 100.0%.
7. **Hygiene**:
   - `flagcheck SERV_IRUHADEV_LEVEL_CAP_99 --worktree`: 4 PASS, 3 PASS~, 2 OTHER (Always.h, X2StateMenu = the gauge
     flag), 0 FAIL, 0 deleted, RESULT PASS.
   - Gauge flag: 1 PASS, 1 OTHER, RESULT PASS.
   - Commits `6690e5cf` + fixup `c42c8efc`. The fixup returns two peer `Always.h` blocks, swept in by a whole-file add, to
     uncommitted, and wraps the generated header wholly in its flag. Net `flagcheck --base 6690e5cf^ --at HEAD`: CAP_99
     5 PASS + 3 PASS~, 0 FAIL, 0 deleted; gauge 1 PASS. RESULT PASS for both. The per-commit `--range` shows the header
     FAIL and its fix.

`DEPLOY OK: X2_offline.exe is 19,589,632 B, dated 2026-10-17 00:46:44, md5 b9ab632c51355ed0013c40fd1aff55f9, equal to X2/US_SERVICE/x2.exe (2026-10-17 00:46:39); built at main 5b5d3bf2 + 10 uncommitted code file(s).`
This is the rebuild after the review-advisor fix: with no next ExpTable row, the level-up guard now keeps the stored EXP
(no award) instead of pinning to the mounted table's TOTAL. Pinning would have written March's 866,438,000 into a June save. `COPY OK data036`
md5 bdbef077… (it also carried the A3 session's `Enum.lua` / `DungeonEnum.lua`).
All four `[CAP99]` literals are in the deployed exe.

**Server Lua packed by Claude at the user's request (2026-10-17):** `elstools compile-batch -v mar-2014`, calibrated first.
Compiling the pre-A2 sources reproduces the staged bytecode byte-identical. `COPY OK data036` md5 4b9090828ff6…
(`StatTable` / `NpcExpTable` changed).
**Play-test PASSED (user, 2026-10-17).**
- The save is at schema v16, and 24 of 27 units hold exactly the EXP the replica computed from the backup. The other 3 moved after
  the migration: reyaa 80 -> 85 at exactly June TOTAL(85); unit 21 +162 from play; weevwe is the L99 `setchar`.
- Load lines: `[CAP99] StatTable rows 81-99 present for 50 of 50 classes` and `NpcExpTable rows 81-99: 19 of 19`.
- Gauge: user-confirmed at L80 and L99, both checks passed.
- The migration launch's own `[CAP99] migrated unit` lines were overwritten by the next launch (logs are per launch). Review: `/x2-phase atlas A2 review` (Opus 5.5 high).

### Phase A2 — Review (2026-10-17)

**Opus 5.5 high. PASS, with one code fix (the no-row guard also resets the level) and one decision for the user (the StatTable intercept).**

1. **Generator.** `cap99_gen.py all`: proof 1 max residual 0 over 250 curves (4000 rows), proof 2 188/190, NPC strictly increasing, RESULT PASS.
   `all --write`: 4 × `WRITE unchanged`, `git status` empty. Classes 110/111: 38 rows × 5 values equal the studio's commented rows, and those
   38 comment lines are still there byte for byte. StatTable: BOM, 5182 CRLF / 0 lone LF, no EOF newline (as before A2). NpcExpTable: BOM, CRLF,
   ends in CRLF (as before); rows 81-99 tab-separated like 1-80. 49 `ReserveMemory` at 99 (class 8 never had one).
   - **AtkMagic L92 is not a float tie; it is the intercept choice.** `pick_rate` returns `alo` (`cap99_gen.py:221`), the lowest intercept that
     fits; there one level sits exactly on an integer, so an extrapolated value just under the next integer floors 1 low. Class 110 AtkMagic:
     r = 8.549, rows 2-80 allow a ∈ [295.491, 295.510); 295.491 gives 1081.999 at L92, the studio's 1082 needs a ≥ 295.492. The interval's
     midpoint reproduces 1-80 exactly and the studio rows 190/190, and would move 85 of the 4750 shipped values (48 of 250 curves) by +1
     (HP +0.5), never more. Thin evidence (110 and 111 share one AtkMagic curve: one curve at one level separates the methods), and the rows are
     packed and play-tested, so nothing was regenerated. **User decision**, below.
2. **Migration formula.** The convention holds: `CX2EXPTable::GetLevel` (`X2EXPTable.cpp:60-80`) = first row with EXP < TOTAL, minus 1, and June's
   100 rows satisfy NEED_EXP = TOTAL(L) − TOTAL(L−1). `X2OfflineCap99Exp.h` equals `march_exptable.tsv` (80) and `june_exptable.tsv` (100) row
   for row; the June tsv equals `june_src_fixed` `ExpTable`. Progress is clamped to [0, M.NEED − 1], so new < J.TOTAL(L+1) and the level holds.
   L80 skips the L+1 term (new = J.TOTAL(80)). The product is `__int64`, and integer division of non-negatives floors. L > 80 is left alone with a WARNING.
   - The `[CAP99] migrated unit` lines are gone: the next launch rewrote the log. Substitute: the formula replayed from
     `db_backup/els_db_20261017_000918_pre_pre_A2_cap99_exp_migration.sql` (v15) against the live save (v16) gives **24/27 exact**.
     - eresisu (21), L77: replica 56,384,285, now +162 from play.
     - weevwe (22): L2 at the replica's 1430 in `els_db_20261017_005341_pre_setchar.sql`, then the L99 setchar.
     - reyaa (12): already L85 at exactly J.TOTAL(85) = 104,400,900 in that 00:53 backup. Only 5 boxes (111064-66) left its bag. Nothing but
       the level-up scroll (tops up to TOTAL(L+1)) and setchar lands on an exact TOTAL, so its migrated value cannot be checked from what is left.
   - Not fixed: the rung commits before `PRAGMA user_version = 16` is written, the same shape as every rung. A crash between the two would
     rerun it, and units would keep their level but lose their in-level progress. Low; house pattern.
3. **80-bounds.** `g_iMaxLevel` = 99 (`Always.h:7369-7374`, after LEVEL_CAP_80's block at 3621), proven by the typedef at `Handlers_Room.cpp:4207`.
   `MAX_LEVEL` / `MAX_NPC_LEVEL` are 99 under the flag. `MARCH_MAX_LEVEL` 80 is the migration table's size. Every other reader uses `g_iMaxLevel` or
   `GetMaxLevel()`. No hard 80 is left in client code. The server-only 80s (`GameSysVal.cpp:776`, `KStatisticsLocalLog.h:11`) are untouched,
   and `X2Define.h:18 LIMIT_MAX_LEVEL` is dead.
   - Out of scope, confirmed: `X2BuffBehaviorTemplet.cpp:1338` is June's own 90 clamp inside `JUNE_BUFF_BEHAVIOR_PARITY`.
     `X2UISkillNote.cpp:62` sits in `#ifndef REFORM_SKILL_NOTE_UI` (defined, `Always.h:526`), so it is not compiled. The `<= 80` tiers in
     `X2UIInventory` / `X2UIShop` are item levels, with a 90 tier above them.
   - Gauge: both branches are right. The bar is full at the cap or with no next row, else it reads NEED(L+1). At L99 June's row 100 exists,
     so the `g_iMaxLevel` test is the one that fires.
   - **Fixed: the no-row guard.** When the loop had already climbed to the table's last row, the guard reset EXP but kept the raised level, so
     SP and the level-up went out with no EXP behind them (against its own "No EXP is awarded" line). It now resets the level too
     (`Handlers_Room.cpp` +5, same flag). Unreachable with June's table (row 100 > cap 99); it matters only on a stale data036.
4. **Hygiene.**
   - Per commit: `6690e5cf` FAIL (the header's `#pragma once` outside the flag).
   - `c42c8efc` FAIL seen alone: that is the fix (flag-off goes from `#pragma once` to empty). Its 19 deleted `Always.h` lines are two peer
     blocks returned to uncommitted.
   - `78adeb86` PASS. Its 3 deleted lines are inside the flag (the old log text and the pin). A2's guard change was committed inside A3's
     commit `78adeb86`.
   - The gauge flag's range FAILs are the same header, which never names that flag. `X2StateMenu.cpp` is PASS.
   - Net `flagcheck SERV_IRUHADEV_LEVEL_CAP_99 --base 6690e5cf^ --at 78adeb86`:
     `files: 5 PASS, 3 PASS~ (blank/marker only), 0 DEFINE, 11 OTHER (other flags), 0 FAIL, 0 ERROR`;
     `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
     `RESULT: PASS`. The OTHER rows are `Always.h`, A3's `ELYSION_PORT` headers, `X2GUEL.cpp` (`EL_DASHJUMP`) and `X2StateMenu.cpp` (the gauge flag).
     `git log -S` for either flag over `78adeb86..HEAD` finds nothing.
   - This fix: `flagcheck --worktree --files X2Lib/Offline/Handlers_Room.cpp`: 1 PASS, RESULT PASS.
     `DEPLOY OK: X2_offline.exe is 19,589,632 B, dated 2026-10-17 01:05:30, md5 c90c5a0d9ad774b093dc7c83fa1f58c9, equal to X2/US_SERVICE/x2.exe (2026-10-17 01:04:15); built at main a017242c + 1 uncommitted code file(s).`
     Review commit `720b68ef`; `flagcheck SERV_IRUHADEV_LEVEL_CAP_99 --range 720b68ef^..720b68ef`: `files: 1 PASS, 0 PASS~, 0 OTHER, 0 FAIL, 0 ERROR`, 0 deleted, `RESULT: PASS`.

**Decision applied (user, 2026-10-17: "follow your recommendation"):** `pick_rate` now returns the middle of the feasible intercepts.
`cap99_gen.py all --write`: proof 1 still residual 0 over 250 curves, **proof 2 190/190, max |diff| 0**, rerun unchanged. `StatTable.lua`: 76 generated rows
(levels 81-99, 29 classes) change 83 values, every one +1 (AtkPhysic 37, DefMagic 34, AtkMagic 12, HP 0); classes 110/111 untouched; BOM, CRLF
and no EOF newline kept. Compiled + packed + copied by Claude at the user's request: `elstools compile-batch -v mar-2014`, calibrated first
(the previous source compiles byte-identical to the staged bytecode, md5 2c34f23c); new bytecode md5 c5c6f027. `PACK OK data036` (~1 vs live:
`StatTable.lua`); `COPY OK data036: ... 12981329 bytes, 239 entries, md5 2ed1a1e6a9a78873975ffc163bafdcbf` (backup `kom_backup_tools\data036.kom.20261017_011135`).

**User decision (no rush):** switch StatTable 81-99 of the 48 extrapolated classes to the midpoint intercept? Yes = regenerate, compile, pack
data036, copy (asked first). No = keep the shipped rows. Play-test owed: none (the fix sits on an unreachable path); the next normal session
is the smoke check.

---

## Phase A3 — Region enums, Lua + C++ (both halves)

**Steps**
1. **Lua** (`march2014/data036/Enum.src.lua`, `DungeonEnum.src.lua`): add
   every ID in the ledger's ID table at June's value (WI incl. 752; VMI incl.
   30009 and Field 04; `LMI_ELYSION`; the 12 `HI_*`; the 21 `DI_*`; the 203
   NUIs). New tables at June values: `LINE_MOVE_TYPE`,
   `DRAMATIC_MESSAGE_TYPE`, `VILLAGENPC_RELATIONDATA_TYPE` (`VRT_*`), member
   `TRIGGER_LINE_ACTION_TYPE.TAT_LINE_CHANGE_MOVABLE_DATA` (value per A1b),
   `LOCAL_MAP_UI_CUSTOM_MSG` 18006–18008. Remove Lanox's "Elysion omitted"
   comments (`Enum.src.lua:4978`, `6618`). Decide each stale Lua sentinel
   (`WI_END`, `VMI_BATTLE_FIELD_END`, `DI_END`) from June's value and its
   Lua consumers.
2. **C++ mirrors** under `SERV_IRUHADEV_ELYSION_PORT`, **explicit `= N` on
   every member, inserted before the terminal sentinel**: `SEnum::VILLAGE_MAP_ID`
   (bump `REST_END`, `VILLAGE_MAP_END`, `BATTLE_FIELD_END`), `SEnum::LOCAL_MAP_ID`
   and `CX2LocationManager::LOCAL_MAP_ID`, `SEnum::DUNGEON_ID`,
   `CX2World::WORLD_ID` (`WI_END` past 752), `CX2UnitManager::NPC_UNIT_ID`
   and `CXSLUnitManager::NPC_UNIT_ID` (`NUI_MAX_LIMIT` past 10195),
   `CX2LocationManager::HOUSE_ID`, `X2LocalMapUI.h` LMUCM 18006–18008
   (explicit — the implicit-increment trap, `JUNE_UI_PARITY_PHASES.md:410`).
   New C++ enum types (`LINE_MOVE_TYPE`, …) are added by their owning engine
   phase, not here.
3. **Consumer audit** of every bumped sentinel (from `region_reach.tsv`):
   arrays sized by it, loops bounded by it, range checks. Record each.
4. `IsVillageField` / `IsDungeonGate` / `IsDungeonLounge` to June's ranges
   (A1a), June's whole function.
5. **Offline seed:** add the Elysion rows to `X2OfflineLuaEnumSeed.h` and fix
   its stale end markers, same change.
6. Compile + stage `Enum.lua` / `DungeonEnum.lua` under the shared-staging
   merge rule (the buff branch also edits `Enum.lua`).

**Done when:** census reports 0 unresolved IDs for both halves; a script
checks every C++ value == Lua value; consumer audit recorded; build rc=0;
staged + manifest; pack list (`data036.kom`) handed over, **paired with this
exe** (Enum and exe ship together).

**Run with:** Sonnet 5.5, high effort. Required review (Opus 5.5): value
equality both sides, sentinel consumers, seed, merge against the buff
branch's `Enum.lua`.

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch main) in full -- "Starting context"
(especially the shared-staging merge rule), the ledger's "IDs" and "end
markers" sections, the A1a/A1b summaries, and "Phase A3". Never launch
X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high. If A1a/A1b leave an enum value or sentinel consumer unsettled,
stop on it and log it for an Opus pass -- do not guess.

Your task (Phase A3): add every Atlas+Elysion ID at June's value to
march2014/data036/Enum.src.lua and DungeonEnum.src.lua (plus the new Lua
tables), mirror them in C++ under SERV_IRUHADEV_ELYSION_PORT with explicit
values before each sentinel, bump and audit the sentinels, port June's
IsVillageField/IsDungeonGate/IsDungeonLounge, update X2OfflineLuaEnumSeed.h,
then compile and stage the Lua merged against the buff branch's copy.

Premises to verify first: 0 collisions (re-run the census); June's own
sentinel values; that the buff branch hasn't added rows in the same ranges.

Done when: Phase A3's "Done when" holds. Record a Done note, then commit on main.
```

### Phase A3 — Done

**2026-10-17, Sonnet 5.5 high.** Flag `SERV_IRUHADEV_ELYSION_PORT` (`Always.h`; MODS.md "Atlas/Elysion port A3"). Commit: `78adeb86` (a peer's A2 commit swept my staged A3 files in; the content is all A3, nothing lost).

- **Landed:** 275 ids at June's value in both halves (`june2017_re/atlas_a3_ids.tsv` = the census `ids.missing` rows, minus the quest plan's `NUI_STORY_QUEST` /
  `NUI_TRANSCEND_QUEST` (user, 2026-10-16), plus `WI_GUILD_AGIT` 623 / `WI_SPA_RUBEN` 725 (A1b row B91)): WI 38, VMI 7, LMI 1, HI 12, DI 21, NUI 193, LMUCM 18006-18008.
  Lua: staged `data036_kom/Enum.lua` + `DungeonEnum.lua` (BOM source, ledger `refresh`ed, `ledger --check` PASS) also got `TAT_LINE_CHANGE_MOVABLE_DATA` 8, the tables
  `LINE_MOVE_TYPE` / `DRAMATIC_MESSAGE_TYPE` / `VILLAGENPC_RELATIONDATA_TYPE` (June's values; their C++ is E5 / E6 / E1) and the Lanox "omitted" comments updated.
  C++ (`#ifdef` blocks, explicit `= N`, before the sentinel): `X2World.h`, `Enum.h` (VMI, LMI), `DungeonEnum.h`, `X2UnitManager.h` + `XSLUnitManager.h` (NUI),
  `X2LocationManager.h` (HOUSE, LMI, `IsVillageField`), `X2LocalMapUI.h`, `X2OfflineLuaEnumSeed.h`. Written by `june2017_re/atlas_a3_gen.py` (bytepatch spec + Lua rows).
- **Sentinel rule (one rule, both sides):** June's value, explicit, original line kept in `#else`: C++ `VMI_BATTLE_FIELD_REST_END` 1009, `VMI_VILLAGE_MAP_END` 20011,
  `VMI_BATTLE_FIELD_END` 40045, `WI_END` 779, `DI_END` 41530; Lua `WI_END` 779, `VMI_BATTLE_FIELD_END` 40045 (`DI_END` was already 41530, C3z-3). `NUI_MAX_LIMIT` stays
  implicit (last member 10195, so 10196; limit 65536 asserted at `X2Item_Preprocessing.inl:1033`). Premise re-read in Ghidra: `32268dd0` = `x-20000 < 11`, `x-30000 < 7`,
  `x-1000 < 9`; `31efb3a0` `< 0x30b`; `322b5970` `x-40000 < 0x2d`.
- **Step 4:** `IsVillageField` = 20000-20010 (the literal 20010: VMI_ELYSION 20009 and June's `VMI_EN`, which March has no enum for), nested in the existing `JUNE_FIELD_MAP` /
  `RANOX_PORT` blocks. `IsBattleFieldRest` follows `REST_END`; `IsDungeonGate` / `IsDungeonLounge` already equal June (untouched).
- **Consumer audit** (each is a range test or an ASSERT; no marker sizes an array, loop or memset, none is saved): REST_END `X2LocationManager.h:553`; VILLAGE_MAP_END
  `X2StateBattleField.cpp:1755`, `X2StateField.cpp:2354`; BATTLE_FIELD_END `X2LocationManager.h:575`, `X2BattleFieldGame.cpp:291/454` (ASSERT), `X2PartyManager.cpp:2615`,
  `X2State.cpp:11790`, `X2StateServerSelect.cpp:5550/11436`, `X2TFieldGame.cpp:1687`; WI_END `X2State.cpp:6840`, `X2StateField.cpp:1293`, `X2StateServerSelect.cpp:7379`,
  `X2StateBeginning.cpp:3991`, `X2BattleFieldGame.cpp:295/457` (ASSERT), Lua `EnumString.lua` (`ENUM_ID = WORLD_ID.WI_END`, 779 is no other WI); DI_END `X2LocalMapUI.cpp:1174`
  (range) and the "no dungeon" return value in `X2UIQuest.cpp`, `X2UIQuestNew.cpp`, `X2WorldMapUI.cpp` (41530 is above every DI in either enum); NUI_MAX_LIMIT
  `X2ItemManager_Preprocessing.cpp:1501` (range), `X2Item_Preprocessing.inl:1033`. Comments only: `Handlers_Field.cpp:331`, `X2StateDungeonRoom.cpp:2691`; stale and untouched:
  `KTDXApp.cpp:2802` (`SetEnumToString( L"WORLD_ID", 6, ... )`).
- **Seed (step 5):** `X2OfflineLuaEnumSeed.h` markers 1007 -> 1009, 20008 -> 20011, 40026 -> 40045 (originals in `#else`) plus the 7 Elysion rows.
- **Gates:**
  - `python -I june2017_re/region_census.py --phase A3` (probes on): WI / VMI / LMI / HI / DI missing 0; NUI missing 2 = the two quest-plan NUIs; "0 missing IDs sit at/above a
    C++ end marker"; markers read 1009 / 20011 / 40045 / 779 / 10196 / 41530. Open items in scope 7 (was 323): those 2 + 2 `lists.hole` (their houses) + 2 `members.june_nil` (June nil too).
  - `python -I june2017_re/atlas_a3_check.py`: `ids checked 275 (both halves resolve: YES)`, sentinels as above, client / server NUI enums 2205 shared names 0 differ, 0 duplicate
    values touching an A3 id, `A3 CHECK PASS`. It reads the C++ through the preprocessor flag state, so it also shows `ELYSION_PORT` reaches `Enum.h` / `XSLUnitManager.h`
    with `Always.h` alone (same as `RANOX_PORT`; no second define in `ServerDefine.h`).
  - A6b cheap check (June `BattleFieldData.src.lua` against the new Enum): 28 unresolved names before, 18 after. The 10 cleared are the Elysion ones; the 18 left are
    `VMI/WI_BOSS_RAID_PERKISAS*`, `*_ELTRION_MK2*`, `*_FIELD_EVENTRAID_TEDDYBEAR` (boss-raid ids outside this region: A6b / the boss-raid owner).
  - flagcheck `--worktree --files` (before commit): `files: 8 PASS, 0 PASS~, 0 DEFINE, 1 OTHER (Always.h, peers' blocks beside mine), 0 FAIL, 0 ERROR`;
    `0 deleted line(s), 0 non-ASCII, 9 EOL-mismatched inserted line(s) (Enum.h: CRLF anchors in an LF region), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
  - After commit, `--range 78adeb86^..78adeb86 --files <the 9 code files>`: `files: 8 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; same in-flag line; `RESULT: PASS`.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,589,632 B, dated 2026-10-17 00:46:44, md5 b9ab632c51355ed0013c40fd1aff55f9, equal to X2/US_SERVICE/x2.exe (2026-10-17 00:46:39); built at main 5b5d3bf2 + 10 uncommitted code file(s).`
  Every A3 header (last edit 00:27:28) predates `X2Lib.lib` / `x2.exe` (00:46), so the exe contains the A3 blocks; enums leave no string to grep.
- **Pack:** `python tools/kom.py pack data036` -> `PACK OK`; vs the live kom 3 changed: `Enum.lua`, `DungeonEnum.lua` and `ExpTable.lua` (A2's, already staged). The A2 session then copied this exact `data036_packed.kom` into the game dir (`kom.py status data036`: in sync, 0 changed vs live), so the exe and
  the kom are already a deployed pair. The 26 `X.src.lua newer than X.lua` warnings predate A3.
- **Open / for the reviewer:** (1) `EnumString.lua` (debug id -> name map) has no rows for the new ids; June's does, a later adoption phase owns it. (2) 20 Lua-only `WORLD_ID`
  names (Sander secret, boss worlds 6xx) have no C++ name (C3y-1b chose Lua-only); A3 added C++ names only for its own ids. (3) The Lua edit is in `kom_contents` (not in git); the
  `march2014/data036/Enum*.src.lua` copies are older than C3z-3 and were left alone. (4) 275 = 272 ids (38 + 7 + 1 + 12 + 21 + 193) + 3 LMUCM.
- **How to see it in game:** nothing visible (enums only). With the exe and `data036.kom` deployed as a pair, a normal start must log no new enum / Lua load errors in
  `ErrorLog.txt`, and Lanox (enter the village, enter a Lanox field) must behave as before; the Cobo list is unchanged (A5 owns the Atlas Station entry).

### Phase A3 — Review (2026-10-17)

**Opus 5.5 high. Verdict: PASS, no transcription error, no code change** (so no build and no deploy: the deployed exe `b9ab632c` and `data036.kom` remain the pair A3 shipped).

- **Values.** `atlas_a3_check.py`: `ids checked 275 (both halves resolve: YES)`, NUI client/server 2205 shared names 0 differ, 0 duplicates touching an A3 id, `A3 CHECK PASS`.
  Hand spot-checks against `june_src_fixed/data036_kom/Enum.src.lua` / `DungeonEnum.src.lua`, the staged Lua and the C++ all agree: `NUI_ELSION_PYURITA` 2094, `NUI_ELYSION_IMPRINT_DYING` 2403
  (both NUI headers), `WI_ELYSION_FIELD_PVPMAP` 752, `HI_YUNO` 40092, `DI_TOWER_NORMAL` 33350, plus `VMI_BATTLE_FIELD_ELYSION_REST_00` 1008, `VMI_BATTLE_FIELD_ELYSION_FIELD_04` 40038, `LMI_ELYSION` 10009.
  Every inserted explicit block is followed by an explicit member or the enum's close, so no implicit March member was renumbered.
- **Sentinels (Ghidra, re-read).** `32268dd0`: `LEA x-20000; CMP 0xa` / `x-30000; CMP 6` / `x-1000; CMP 8` (VILLAGE_MAP_END 20011, REST_END 1009). `31efb3a0@31efb4e8`: `CMP EAX,0x30b; JGE` (WI_END 779).
  `322b5970@322b5a01`: `x-40000; CMP 0x2c; JA` (BATTLE_FIELD_END 40045). DI_END: `3293ab50@3293b4ad` `x-30000; CMP 0x2d0a; JA`, i.e. inclusive 41530, the same as March's
  `iDungeonID > DI_END` skip at `X2LocalMapUI.cpp:1174`. The explicit-value rule (`#ifdef ... = N #else <original> #endif`) is right: a marker can no longer move when someone adds an id
  above it, and every original line is reachable with the flag off. `NUI_MAX_LIMIT` implicit 10196 is fine (only a range test and a `<= 65536` static assert read it). Lua twins:
  `WI_END` 779 (staged max WI 752) and `VMI_BATTLE_FIELD_END` 40045 (staged max field 40038).
- **Consumer audit: the Done note's list is right but not complete.** Missed readers, none of them a problem: `SEnum::IsVillageMapID` / `IsBattleFieldID` / `IsRestFieldID`
  (`Enum.h:572/580/588`, ranges, no client caller per A1a); the three seed rows; `XSLDungeon.cpp:848` `IsTCDungeon` `[39000, DI_END)` and `XSLFieldBossData.cpp:160` (ranges, widened by
  the bump, but in no client `.vcxproj`: server-only, and the offline handlers spell the TC range themselves, `Handlers_Skill.cpp:175`, `Handlers_Room.cpp:3527`); `KTDXApp.cpp:2808`
  (`DI_END` name literal with stale 30003, a name table). `DI_END` as "no dungeon": `X2UIQuest.cpp` is dead (`#ifndef SERV_EPIC_QUEST`), `X2UIQuestNew::GetSubQuestDungeonID` is dead
  (`#else` of `REFORM_QUEST`); the one live use, `X2WorldMapUI::GetNPCQuestDesc` (`:1944-2005`), tests equality before any lookup and relies on DI_END being the top multimap key.
  Max DI is 41310 in `DungeonEnum.h` and 41271 in the staged Lua, so 41530 cannot collide and "other" still sorts last. No marker sizes an array, loop or memset; none is saved.
- **`IsVillageField` / seed.** Live line `eVillageID >= VMI_RUBEN && eVillageID <= 20010` (`X2LocationManager.h:518`, all three nested flags on) = June's `x-20000 <= 10`.
  `X2OfflineLuaEnum.cpp` publishes only one region table, `VILLAGE_MAP_ID` (others: UNIT_CLASS, USE_CONDITION, PET / RIDING_PET, ITEM_TYPE / GRADE, ENCHANT, SOCKET), so A3's 3 markers
  + 7 rows are the whole fix; NUI / WI / DI / HI / LMI are never overwritten by the seed.
- **Staging.** `luasrc.py ledger --check`: `LEDGER CHECK PASS: 0 FAIL, 0 WARN`. No peer edit was lost: the BOM-stripped sha1 of `Enum.lua` / `DungeonEnum.lua` extracted from the
  pre-A3 live kom (`kom_backup_tools/data036.kom.20261017_004241`) equals the pre-A3 ledger `source_sha1` (`3280d56e` / `210eea45`), and today's files equal the post-A3 rows
  (`addf85d1` / `6e0d9ac3`); the line diff's 10 "removed" `Enum.lua` lines are the same entries re-written (comma, sentinel, updated "omitted" comment). Kom: `kom.py status data036` in sync;
  vs the pre-A2/A3 live kom the net delta is 5 changed entries, 0 added / removed: A3's `Enum.lua`, `DungeonEnum.lua`, A2's `ExpTable.lua`, then A2's `StatTable.lua` / `NpcExpTable.lua` (`74eb6894`).
- **Hygiene.** `flagcheck ELYSION_PORT --range 78adeb86^..HEAD --files <9 code files>`: `files: 8 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII,
  9 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (the 9 are `Enum.h` CRLF anchors in an LF region, cosmetic).
- **Notes, nothing to fix now.** (1) The `SERV_IRUHADEV_JUNE_LOCAL_MAP_UI` block in `X2LocalMapUI.h` still says 18006-18008 are "deliberately omitted"; A3 now defines them just above.
  The comment is stale, it is in another flag's block, and it is left alone. (2) `data036_kom/EnumString.src.lua` (2026-10-10) is the stale sidecar behind `kom.py status`'s
  `[stale .src.lua: compile]`; there is no `Enum.src.lua` / `DungeonEnum.src.lua`, so a later `compile` cannot fold over A3's text. (3) The Done note's open items (EnumString rows,
  20 Lua-only WIs, `march2014/` copies) stand as written; none blocks A4 / A5.
- **User owes:** the A3 play-test as before (normal start, no new enum / Lua errors in `ErrorLog.txt`, Lanox unchanged); it can share A2's migrating launch.

---

## Phase A4 — Atlas-half script engine

**Steps.** Under `SERV_IRUHADEV_ELYSION_SCRIPT_A`, for every name A1b assigned
to A4 (the Atlas-half list in the ledger, incl. `SetMovieDescriptionData`,
plus `SetCameraDistance`):
1. Port June's implementation onto March classes (cite the June address in a
   comment; re-express logic, never port raw offsets). Names marked (b) need
   only a `class_def` line — still verify the receiver class.
2. Parse `CHANGE_LINE_TYPE_AFTER_ALL_ACTIVE_NPC_KILLED` and the shared
   unit-script keys A1b assigned here (likely `USE_DEFAULT_DIE_PARTICLE`,
   `IMMUNITY_DAMAGE_DATA_STOP_TIME`, `SOUND_PLAY_*`, `NOT_CHECK_INDICATOR`, …)
   with June's semantics.
3. A key or name A1b marked as owned by JUNE_BUFF (`SetMonsterBuffGroupID`,
   `BUFF_GROUP_ID`) is **not** implemented here: check the buff branch; if
   absent, record the block.
4. Diagnostics: `[ELYDBG]` lines behind `SERV_IRUHADEV_ELYSION_DEBUG` for each
   new mechanism's first use.

**Done when:** census binding/key report for `--region atlas` shows only
buff-owned items open; build rc=0; new literals in the exe; `SetCameraDistance`
fixes the Lanox `SECRET_SCENE_DUMMY` call (note it for A13's play-test).

**Run with:** Opus 5.5, high effort (Sonnet 5.5 for names A1b marked
transcription-ready, stopping on anything else). Required review.

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch main) in full -- "Starting context", the
ledger's "June-only engine" section, the A1b engine map, and "Phase A4". Use
the ghidra skill to confirm each June body before porting. Never launch
X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high.

Your task (Phase A4): implement every binding and key A1b assigned to A4
behind SERV_IRUHADEV_ELYSION_SCRIPT_A, June address cited per body, original
code in #else where behaviour changes. Bind SetCameraDistance. Leave
buff-owned items (SetMonsterBuffGroupID, BUFF_GROUP_ID) to JUNE_BUFF and
record whether they are already there.

Premises to verify first: each name's receiver class and argument types (A1b).

Done when: Phase A4's "Done when" holds. Record a Done note, then commit on main.
```

### Phase A4 — Done

*(2026-10-17, Opus 5.5 high. Flags `SERV_IRUHADEV_ELYSION_SCRIPT_A` + `SERV_IRUHADEV_ELYSION_DEBUG` (`[ELYDBG]`). 19 funcmap rows `Atlas A4`.)*

- **Scope check.** Of the ledger's A4 names, C3z-4 / buff P10 had already bound `GetDyingStart`, `IsActiveMonster`,
  `GetWorldObjectMeshByName`, `GetCenterPositionLineIndex`, `FadeOriginWorldColor_LUA`, `LiveKeyCodeNPCNumExceptDying`,
  `SetImmuneToExtraDamage`, `SetMovieDescriptionData` and `SetCameraDistance` (`X2Main.cpp:3718`; A13 still play-tests Lanox
  `SECRET_SCENE_DUMMY`). Buff-owned `SetMonsterBuffGroupID` is bound (`X2Main.cpp:4701`, buff Phase 10). `BUFF_GROUP_ID` is not on
  the region census's open list. `TARGET_FRONT_ME`, `AFTER_IMAGE_DEST_ALPHA_BLEND` and `STOP_TIME_ATK` are never read by June.
- **Landed (one flag):**
  - **Binds:** `GetAliveActiveNPCCount` (CX2Game, `322d0ae0`, body unchanged) and `GetNationFlag` (CX2Main, `31e4e690`).
  - **`SetChangeUVPoint`:** June's whole UV-change mechanism on `CPictureData` (`31c9a7d0` store, `31c95150` start, `31c95a60`
    end callback (`std::tr1::function`), `31c95580` tween tail, `31c9eea0` reset from the ctor and `SetPoint_LUA`), plus the
    `CKTDGUIStatic` frame-move gate (`31cc0950`). The only caller of start / callback is the Elysion local-map scroll (E4).
  - **`USE_DEFAULT_DIE_PARTICLE`:** "DieLight" once at death (`3277ec60` head, `32778d40`), at the top of `CommonFrameMove_DieParticle`.
  - **`NOT_CHECK_INDICATOR` and `INIT_AI NO_INDICATOR`:** the indicator filter drops them (`32f09f05`, `32f09f2a`). `NO_INDICATOR` is
    June's AI `+0x34`: a new bool between the owner and the AI type (`32f2b4c0`). A1b had left it unnamed.
  - **`CAN_NOT_PASS_SPECIFIED_BONE`:** the state list (`32784c00` @32796263), `HasCanNotPassSpecifiedBone` (vslot 0x2a8 `32755e70`),
    the player's push loop (`32010060`: an NPC with a list collides, and pushes once per bone with the results ignored), and
    `ProcessCanPushUnit`'s bone form (`31fe8ad0`). In the bone form the bone is the centre, Y spans bone ± distance, the half width
    is mine ÷ 2 + distance, and the NPC's per-point direction (`327788b0`) overrides the direction in the left/right branch.
  - **`CHANGE_LINE_TYPE_AFTER_ALL_ACTIVE_NPC_KILLED`:** line `+0x150` (`31c705c0` @31c73ab9) and the retype twin
    `ChangeLineTypeAfterAllActiveNPCKilled` (`327efc90`). It runs once per clear condition (June `+0x54`) from
    `CheckIfSubStageCleared` cases `CC_KILL_ALL_ACTIVE_NPC` / `CC_PORTAL_AFTER_KILL_ALL_ACTIVE_NPC` (`327f0c10`).
- **Deviations from June** (all guards on paths June data never takes):
  - the UV tween and the seed record skip a NULL texture (June dereferences it);
  - `StartChangeUV` refuses a direction above 2 (June reads record -1);
  - the twin's `SHOW_PORTAL` is a `true` constant until june-engine W2 parses the key (June default 1; note added to `kickoffs/june-engine/W2.md`).
- **Routed / recorded:**
  - June's `PhysicProcess` push block for `DI_EVENT_TREASURE_HOUSE_2` (41380, stage 2, `32018c26-3201906c`) goes to backlog Stage 19.
  - JUNE_BUGS 205: `327788b0` mixes frames (1/2 player, 3/4 NPC), checks no team, and gets the player's own Y.
  - The dual-body (Lu/Ciel) second push call (`@32018c1c`) stays with the char ports.
- **Gate:**
  - `region_census.py --phase A4 --region atlas`: only nothing-to-port items remain: `Elysion_Tower_Stage3.lua` (no such June file),
    `STOP_TIME_ATK` (typo), and the `CT_*` / state-name false positives (A1b).
  - flagcheck `--worktree`: `files: 13 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR` (OTHER =
    the A5 peer's `ELYSION_PORT` warp cases in `X2Main.cpp`); `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched`;
    `RESULT: PASS`.
  - All 11 new literals are in the deployed exe (keys, binds, `[ELYDBG]` texts).
  - `DEPLOY OK: X2_offline.exe is 19,601,408 B, dated 2026-10-17 01:52:55, md5 c3f55fb41963b6bd6906df3d333caa66, equal to X2/US_SERVICE/x2.exe (2026-10-17 01:52:50); built at main 09c01652 + 24 uncommitted code file(s).`
    That build includes the A5 peer's uncommitted files.
  - Commit `d726302f`; `flagcheck --range d726302f^..d726302f`: `files: 14 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `RESULT: PASS`.
  - Follow-up commit: `[ELYDBG]` first-use lines for the bone list (load, push, contact) and the indicator skip
    (flagcheck `--worktree` on the 3 files: `3 PASS ... RESULT: PASS`). Redeployed:
    `DEPLOY OK: X2_offline.exe is 19,602,944 B, dated 2026-10-17 02:03:41, md5 5ea0b3a234c5896ed8d12c5989013f7b, equal to X2/US_SERVICE/x2.exe (2026-10-17 02:03:36); built at main ac236b47 + 3 uncommitted code file(s).`
- **How to see it in game:** the Atlas content itself arrives in A8-A10 (and the local map in E4), so this phase's play-test uses
  June NPC scripts that are already live in older dungeons. The hand-off in the session names the dungeon and character.
- **Open for the user:** nothing to decide. Next: `/x2-phase atlas A4 review` (Opus 5.5 high).
- **Play-test PASSED 2026-10-17** (run 03:28 on exe md5 5ea0b3a2; `logs.py --tag ELYDBG`):
  - Besma Mine 2 (`NUI_NASOD_RAIL` 127): `first load: bone Bip01_L_Finger0Nub distance 100.0`, then
    `first bone push: npc 127 ... frame found dir 3`.
  - `first bone contact` came from both branches: dir 4 at 47.2 < 135.0, and dir 1 at 116.8 < 135.0. 135 is half the player's
    width plus 100.
  - Lanox Secret Hell: `indicator first skip: npc 2246` (`NUI_SECRET_SCENE_DUMMY`, `NOT_CHECK_INDICATOR 1`).
  - The run's 3 UNHANDLED `E_DISABLE_HB_CHECK_REQ` are outside this phase.

### Phase A4 — Review (2026-10-17)

**Opus 5.5 high. Verdict: PASS. No logic error. One comment citation fixed** (`X2DungeonSubStage.cpp:857` said `@327f1180`; June's case-5 call is
`@327f0fcd`, per xrefs to `327efc90`). The fix is comment-only, so there was no rebuild: the deployed exe `5ea0b3a2` is still A4's.
All five areas were re-read in Ghidra (decompile, plus the disassembly where the decompile drops stack arguments):

- **Bone push.**
  - `31fe8ad0` prologue: a negative index or a non-NPC (`+0x264 != 1`) takes the unit's own box. No list (vslot `0x2a8`) or an index past
    `(+0x408 - +0x404) >> 5` goes `31fe8c7f -> 31fe93bf XOR AL,AL`, which returns false.
  - No clone frame: own height, with the centre still `GetPos(true)`.
  - With a frame: centre `GetBonePos` (vslot `0x84`), height `d + d`, Y min `bone.y - d`.
  - Both branches set the half width to `GetUnitWidth(true) * 0.5 + d`.
  - `327788b0` is called only in the dir 1/2 branch, with the point's Y already set to the user's Y.
  - The rest of that branch tests `dir == 2` / else, as March's does, so an override to 3 / 4 takes the dir-1 arm in both builds.
- **`327788b0`, x87 re-traced by hand.**
  - Y overlap is the same three inclusive tests as A4.
  - In the overlap case it returns `dot(+0x8430, user - point; y * 0.0) < 0 ? 2 : 1`. Otherwise it returns `myMax <= userMin ? 3 : 4`.
  - The result is -1 unless both the NPC (`+0x8344`, decrypted) and the user (`stateparam`) cannot pass.
  - The guards come first: user NULL, user `GUSI_DIE`, and a battle field where the user is not mine and has no first sync. All match A4 and JUNE_BUGS 205.
- **`32010060`.** The collide test is June's two team terms `|| vslot 0x2a8`, so A4's `GUT_NPC && Has...` is the same (`CX2GameUnit`'s slot returns false).
  Each bone gets `31fe8ad0(..., i)` with the result ignored. Without a list it is one call with -1, and a false result resets to the sync position, as in March.
- **UV change.**
  - Key names (XOR-decoded) map `LEFT_TOP` / `RIGHT_TOP` / `LEFT_BOTTOM` / `RIGHT_BOTTOM` to record slots 0-3, which are March's `VP_*` 0-3.
    `CHANGE_TIME` is at `+0x40`.
  - Seed: when the vector is empty, `pTexture->uvOrgTexture[0..3]` (`+0x3c`) is pushed with this call's time.
  - `31c95150`: checks run in A4's order. Speed is `(to - from) * (1 / to.time)`. It returns the old and new index and stores the new one.
  - `31c95580` tail: if `time - dt <= 0` (`FCOMP` against `FLDZ`), it calls the functor with `&index` (`+0xb4`), drops it, sets the time to 0 on
    both paths, clears the state, and snaps the record. Otherwise every point moves by `speed * dt`. The recompute equals `UITextureData::SetTextureUV()`.
  - `31c9eea0` matches `ResetChangeUV`.
  - The guards are unreachable from June:
    - all four callers of `329378a0` pass a direction of 1 or 2: `3293c16a` `page ? 1 : 2`, `32937a06` constant 2, `3293d83e` 1, `3293d86f` 2;
    - June dereferences the texture unguarded at both NULL-texture sites, so shipped June data never reaches them without a texture.
  - The live `LUA_GET_USER_DEFINED_TYPE_VALUE` branch (`X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING`, `KTDX.h:390`) defaults a missing point to (0,0).
- **Die particle.**
  - `3277ec60`: `(gage && (no hp || hp <= 0)) || state 4) && +0x8fd9`.
  - `32778d40`: runs only when handle `+0x8990 == -1`. It creates `DieLight` at `GetPos(false)`. The pushes are emit (-1,-1) and trigger (3,-1); the
    constants read from the exe are 3.0 and -1.0.
  - The handle goes to `+0x8990` (`m_hSeqDieEffect`, the field the rest of the function uses). The land Y comes from vslot `0x134` (`GetLandPos`, the same field as `GetLandPosition_LUA`). No sound.
- **Indicator.** `32f09f05` `cmp [esi+0x8fa8],1; je skip`, then AI NULL, then AI type (`+0x38`) != 0, then `32f09f2a` `cmp [eax+0x34],1; je skip`.
  All four skip, so A4 testing after March's AI-type check is equivalent. The `INIT_AI` key at `+0x34` decodes to `NO_INDICATOR`.
- **Line retype.**
  - `327efc90`: skips `+0x150 == -1` and a type that already matches. Filters on `+0xec` / `+0xf0` against the stage / sub-stage.
  - When the new type is 2 and `+0x14c == 1`, it places `Peita_Teleport_MagicSquare01` at `(start + end) * 0.5 + (0, 3.0, 0)`, layer `0xe` = `XL_EFFECT_0`.
  - Hooks: case 0 calls it after `LiveActiveNPCNum == 0` once (`+0x54`) at `@327f0d81`.
    Case 5 does so at `@327f0fcd`, after the portal-effect block (every path through that block lands on `327f0fc1`), before the portal-line user test. A4 places both the same way.
- **Hygiene.** `flagcheck ELYSION_SCRIPT_A --range d726302f^..HEAD --with ELYSION_DEBUG`: `files: 17 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
  `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
  The two PASS~ are one blank line each, outside the flag (`KTDGUIControl.cpp`, `X2GUNPC.cpp`). The review fix: `--worktree --files X2Lib/X2DungeonSubStage.cpp`: `1 PASS ... RESULT: PASS`.
  Its 1 deleted line is the old comment text, inside A4's own flag block.
- **Not re-read:** `31cc0950` (one-line static gate), `32784c00` loader layout, `32755e70`. Their funcmap rows are cited, and the play-test exercised them.

---

## Phase A5 — Atlas Station (rest area 1008)

**Steps**
1. **Scene:** `Field_Elysion_rest.lua` + `Field_Elysion_rest_LineMap.lua`
   (data040; *not* the `_Festival` variant). `WorldData` rows for WI 637 (and
   lift `M14/data040/WorldData.src.lua:3034`'s omission).
2. **`DLG_Map_Village.lua`** `Village[VMI_BATTLE_FIELD_ELYSION_REST_00]`:
   `WORLD_ID`, `HouseIDList`, `START_POS` 400–404 with **March key names**
   (`BattleFieldId`/`BattleFieldStartLineIndex`); check how June's IsWarp /
   summon positions map onto March's parser.
3. **Houses:** every shared house June puts in the rest area gets a
   `COMMON_NPC_LOCATION` row for `WI_BATTLE_FIELD_ELYSION_FIELD_REST_00` in
   `DLG_Map_House.lua` (Lanox Remaining P9 is the precedent).
4. **`MapData.lua`:** `AddMapData` row (inert quest keys verbatim + comment),
   Cobo zone **17** for 1008, F01, F02 in `CheckCOBOExpressTicketMapID`;
   mirror in `KncWX2Server/ServerResource/US/MapData.lua` (BOM + mixed
   CRLF/LF — byte-level). Lift `MapData.src.lua:453`'s omission.
5. **C++** (`SERV_IRUHADEV_ELYSION_PORT`, June's whole function each, per
   A1a): `IsValidWarpField`, `LoadFieldImage` (0x3f0 →
   `HQ_LoadingDungeon_Atlas_Pit_Rest_01/02.tga`), `OpenFieldName`,
   `GetFieldMapDlg` + `DLG_UI_FIELD_MAP_Elysion_Rest_NEW.lua`,
   `ResetCameraMatrix` / `ResetMapPosAndSize` / `UpdateNaviMap`, both
   `InitWarpList`/`CreateWarpDest` pairs + `Village_1008` controls in
   `DLG_Destination.lua` and `datai014/DLG_Destination_byButton.lua`, and the
   return-scroll case if A1a says June has one. Chat `atlas` → 1008.
   **Changed 2026-10-09:** `DLG_Destination.lua` (data034) is already June's whole file (ljconv bytecode, MODS
   "June partial ports replaced"), `Village_1008` included. `DLG_Destination_byButton.lua` (datai014) is **March's
   merged copy on purpose**. June's whole file drops `Village_1000` / `Village_1002` (Elder and Altera rest), and
   March's `CX2StateMenu::InitWarpList` dereferences both unchecked, so pressing B crashed. With June's
   `InitWarpList`/`CreateWarpDest` ported here, adopt June's whole `DLG_Destination_byButton.lua` (`kom.py stage-june` +
   ljconv) instead of adding `Village_1008` by hand; the Elder and Altera rest stops then leave Cobo, as in June.
6. **World map:** June's Elysion block (`DLG_UI_World_Map_NEW.src.lua:1842-2080`)
   — the rest + F01/F02 pins land here; the cloud static's `AddDummyInt(99999)`
   placeholder (`M14 …World_Map_NEW.src.lua:2315`) becomes the real VMI.
   Town + F03 pins are E2/E3's; port the whole block once,
   here, and leave those two controls as-is with a comment if their targets
   don't exist yet (a pin to a missing village must be hidden, not dead).
7. Loading art and field-map assets staged under June's archive numbers
   (data183 is new).

**Done when:** census `--line REST` clean; build rc=0; staged + pack list.
After pack: `atlas` chat warp renders the scene with houses/NPCs; Cobo row
and warp-NPC row present and working; loading image shows; M-key map follows
the character.

**Run with:** Opus 5.5, high effort. Required review.

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch main) in full -- "Starting context", the
ledger's "Topology", "Gates", "C++ sites" and "Lanox-side omissions" sections,
the A1a summary, and "Phase A5". Also read LANOX_TROUBLESHOOTING.md's
symptom table. Never launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase A5): port Atlas Station (VMI_BATTLE_FIELD_ELYSION_REST_00,
1008): scene + linemap, WorldData, DLG_Map_Village (March key names), every
shared-house COMMON_NPC_LOCATION row, MapData + Cobo zone 17 (client and the
ServerResource mirror), June's whole versions of IsValidWarpField /
LoadFieldImage / OpenFieldName / GetFieldMapDlg / the field-map camera and
navi / both warp-list implementations, the world-map Elysion block (replace
the 99999 placeholder), the "atlas" chat warp, and the loading/field-map
assets.

Premises to verify first: June's START_POS semantics for 400-404 against
March's VillageParsing; which shared houses June places in 1008.

Done when: Phase A5's "Done when" holds (pack list handed over; live-kom check
after I pack). Record a Done note (incl. the DEPLOY OK commit), then commit on main.
```

### Phase A5 — Done

**2026-10-17, Opus 5.5 high.** Flag `SERV_IRUHADEV_ELYSION_PORT` (A3's; MODS.md "Atlas/Elysion port A5"). Commit: `f5951bbc`. After commit, `flagcheck --range f5951bbc^..f5951bbc --files <8 code files>`: `files: 8 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, same in-flag line, `RESULT: PASS`.

- **Premises:** June `START_POS` 400-404 = March's `VillageParsing` keys except `OtherPlaceID` / `OtherPlaceStartLineIndex` -> `BattleFieldId` /
  `BattleFieldStartLineIndex` (`X2LocationManager.cpp:394-399`; `IsSummon` / `IsWarp` identical). June's 1008 houses: `HI_GLAVE_HOUSE`, `HI_YUNO`,
  `HI_BILLBOARD`, `HI_POSTBOX` (+ the two quest houses, list-only). March had no `HI_YUNO` house or `NUI_YUNO` templet at all.
- **C++ (June's whole function, March's body in `#else` where replaced):** `IsValidWarpField` 324f1e70 (+1008, 20009, 1501), `LoadFieldImage` 323ad5f0,
  `OpenFieldName` 323b1320 (indices from its jump tables @323b1600/@323b1624 = June `DLG_UI_Field_Title` `SetIndex`; adds Ranox 15/27/30, a Lanox miss;
  + `SetDisableUnderWindow(false)` = dialog +0x190, funcmap 31c1bf70; June's `this+0x3c0` store has no found reader: not ported), `GetFieldMapDlg` 3225e1f0,
  `UpdateNaviMap` 3225cad0, `ResetCameraMatrix` 32f1fec0 (floats read from the binary, layout proven on the Ranox-rest case), `ResetMapPosAndSize`
  32f1fd90 (no March caller), `WarpByCommand` 31d71000 (positions 15-18, 20), Cobo `InitWarpList`/`CreateWarpDest` 3245a1e0/324627f0 and warp-NPC
  32166ae0/32173120 (flat lists, NULL-checked rows, code event 2260 skips 1008 but the row still advances @32463056, `m_iCurrentVillageWarpIndex` =
  +0x410 branch, `SetIndex` last), `atlas` chat warp. Omitted with a comment: 1500 / 25000 (no enum), Cobo's 1500/1501 rows (June-only instance-data
  gate +0xa1c), the LMI_ELYSION quest test (decision 2). June's state numbering differs: the warp-index branch uses March's `AddWarpList` mapping.
- **Lua (BOM source, ledger refreshed):** `DLG_Map_Village` Village[1008]; `DLG_Map_House` 1008 rows (POSTBOX, BILLBOARD incl. rotation, GLAVE) + `HI_YUNO`;
  `NPCTemplet` `NUI_YUNO`; `WorldData` WI 637 (Festival/XMAS keys verbatim, inert); `MapData` AddMapData + Cobo zone 17 (1008/F01/F02) + server mirror
  (byte-level, 31 lines, each region's own EOL); `DLG_UI_World_Map_NEW` June's Elysion block (1842-2081) over the Lanox `99999` cloud, town / F03 pins
  `SetShow(false)` until E2/E3. June whole files: `DLG_Destination` (ljconv), `DLG_Destination_byButton` (`--source --lifted`, T1; core row lifted),
  `DLG_UI_Field_Title`, `DLG_UI_FIELD_MAP_Elysion_Rest_NEW`, `UI_YUNO`, the 4 scene scripts + Festival line map. 128 assets staged (61 + 67 from the
  texture closure, `a5_closure.tsv` in the session scratchpad; 2 March files replaced by June's: `DLG_UI_Common_Texture_Fieldname_Rest_1.TET/.tga`, the
  only atlas with `FIELD_REST_ELISION`). Not taken: `Fieldname_Village_2.TET/.tga` (differ, Ranox keys only, not A5's).
- **Census `--phase A5`:** 93 -> 29 open, all routed: `field_rest_elysion001.ogg` is a loose June music file (copied to `data\music`, census reads koms only);
  `LINE_NUM` = B73 false positive; `WorldData.dataFileName_XMAS` = B84 (June never reads); `DLG_Map_Village.OtherPlace*` = renamed by design, `BaseLocalStateID`
  unread in both (March's own file has it); 23 `DLG_Map_House.*` keys (2nd mesh, talk lists, gifts, `EXTRACTION` / `TRADE` / `Trade_Talk`) are read only by
  June's `HouseParsing`: atlas E1's port-whole (A1a). Yuno's monitor mesh and talk loop wait for E1.
- **JUNE_BUGS** 201 (40038 opens Field 02's dialog), 202 (`ResetMapPosAndSize` skips 1008 / 40038), 203 (warp NPC never lists Elysion: June's dialog has no
  `Village_20009`), 204 (Elysion has no title banner: no picture 16).
- **Hygiene:** flagcheck `--worktree --files <8 code files>`: `files: 7 PASS, 0 PASS~, 0 DEFINE, 1 OTHER (X2Main.cpp: a peer's ELYSION_SCRIPT_A hunks), 0 FAIL,
  0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
  `RESULT: PASS`. `ledger --check` PASS, `core --check` PASS (0 WARN).
- **Pack / copy (user OK):** `PACK OK` + `COPY OK` data013 / 023 / 034 / 036 / 037 / 039 / 040 / 057 / 096 / 151 / 180 / 181 / 182 / 183 (new) / i014
  (datai014 md5 601196d3, data034 8833134b); `field_rest_elysion001.ogg` copied, md5 = June's.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,601,408 B, dated 2026-10-17 01:52:55, md5 c3f55fb41963b6bd6906df3d333caa66, equal to X2/US_SERVICE/x2.exe (2026-10-17 01:52:50); built at main 09c01652 + 24 uncommitted code file(s).` (a peer's build linked it after A5's edits; my run registered it). Proof: `HQ_LoadingDungeon_Atlas_Pit_Rest_01.tga`, `DLG_UI_FIELD_MAP_Elysion_Rest_NEW.lua`, `FieldMap_Navigation_Elision_R`, `Village_1501` are UTF-16LE literals in the deployed exe.
- **Open / for the reviewer and later phases:** (1) the Cobo and warp-NPC 1008 rows sit behind `GetLocalMapTemplet( LMI_ELYSION )`, which is NULL until A7
  stages `DLG_Map_Local_Elysion` (level 78): before A7 neither list shows Atlas Station (June's gate, not a bug). (2) Before A7 the world map has no Elysion
  templet, so `UpdateWorldMap` leaves the Lua show state: rest/F01/F02 pins show, the ELISION cloud stays drawn; after A7 its level gate (`X2WorldMapUI.cpp:603-660`)
  also re-shows the town / F03 pins, so A7 must keep them hidden until E2/E3 land. (3) Silent Night Shelter (1007) still has no house rows: Lanox Remaining P9.
  (4) The Rest field-map dialog's 6 keys resolve (`ELISION_Rest_04.TET` holds 04 + 05, `_05.TET` holds 06, as June's dialog asks).
- **Play-test 2026-10-17 (user): `atlas` does nothing visible; root cause found, fix is A7 (user's choice).** The warp reaches the emulator
  (`WARP button warp 20001 -> 1008`, `entering mapID=1008`) and the client's current village becomes 1008 (M shows the Atlas map), but the
  field state loads no world (`loaded` 21 ms later at the old line/pos). `CX2StateField::Handler_EGS_STATE_CHANGE_FIELD_ACK` skips
  `new CX2TFieldGame` when `GetWorldID( 1008 )` is invalid (`X2StateField.cpp:1293`), and 1008 has no village templet: `VillageParsing`
  runs only from `LocalParsing` for the `VillageData` / `BattleFieldRestData` ids a registered local map names (`X2LocationManager.cpp:227-327`).
  March's `DLG_Map_Local.lua` has no `LMI_ELYSION`; June's `DLG_Map_Local_Elysion.lua` (`BattleFieldRestData = 1008`) is A7 steps 1-2.
  A5's checks (warp, NPCs, art, M map, world map, Cobo) re-run with A7's play-test.
- **How to see it in game:** the play-test hand-off in the A5 STATUS row's commit message / the session's hand-off: `atlas` in chat -> Atlas Station with
  Glave, Yuno, billboard, postbox; Atlas Pit loading art; M key field map follows the character; world map shows the Atlas pins.

### Phase A5 — Review (2026-10-17)

**Opus 5.5 high. PASS: no transcription error with an effect, no code change, no build** (deployed exe stays c3f55fb4 or a later peer
build carrying `f5951bbc`). Ghidra + capstone over `unpacked_x2.exe`, read-only; floats and jump tables read straight from the file.

- **Cobo `CreateWarpDest` 324627f0:** LMI pushes 0x2711-0x2719 against March's `LMI_*` (10001 ELDER ... 10008 RANOX, 10009 ELYSION),
  every row's control name and village id, `inc esi` only inside the control-non-NULL branch; 1008 row: code event `0x8d4` (2260)
  through `g_pInstanceData` (NULL -> add), `inc esi` @32463056 outside the add; 20009 row has no `inc` (port: none). Dialog
  `DLG_Destination_byButton.lua` @32462911, `GetWindowPos( (300,400), (75,20) )` (floats 33471308 / 3344f348 / 33457df0 / 33438a8c),
  `SetIndex` last. Omitted rows still true: 1500 / 1501 behind the `+0xa1c` instance-data map (no March table), 25000 (ungated, before
  Ruben; no enum), the quest test `+0x70` -> 32322260 (decision 2).
- **Warp-index branch @32463091:** June's state switch 0xe / 0x10 -> location `+0x64`, 0xf -> `+0xb4`, 0x15 -> 324826f0( -1 ). By funcmap
  `31d6fd60` June 0xf is the new `XS_GUILD_AGIT_GAME` (shifts the rest), so 0xe / 0x10 / 0x15 = March `XS_VILLAGE_MAP` / `XS_SQUARE_GAME`
  / `XS_BATTLE_FIELD`, and June's own `AddWarpList` 3245c930 uses the same switch: A5's mapping onto March's `AddWarpList` is right, the
  guild-agit case has no March state.
- **Warp-NPC `CreateWarpDest` 32173120:** `DLG_Destination.lua`; order 1500 (ungated, omitted) / 1501 under 0x2718 / 25000 (omitted) /
  20000 / the 8 regions / ELYSION (level + quest test only, no code event, no index branch) = the port. One cosmetic difference: June
  does not `inc` after the 20009 row (@32173950); the port's `++row` there is never read again. Left as is (no behaviour, no rebuild).
- **`InitWarpList` x2:** Cobo 3245a1e0 = 21 names, all NULL-checked, port's order; warp-NPC 32166ae0 = the 14 March names unchecked,
  then 20008, 1007, 20009, 1008, 25000, 1500, 1501 checked = the port (`i < 14`).
- **Values:** `ResetCameraMatrix` all six cases re-read through the jump tables @32f20cfc (1008 -> 32f200b5) / @32f20d44 (40035 ->
  32f20beb, 40036 = 40038 -> 32f20c65, 40037 -> 32f20c2a) and the store tail @32f20ca2: eye / look exactly as ported, fov 0x3353c16c =
  0.627935 = `D3DXToRadian( 35.978 )`, aspect 2.0. `OpenFieldName` tables @323b1600 / @323b1624: 1007 27, 1008 28, 20008 15, 20009 16,
  1501 30 (`0x1e`), 1500 29 (omitted). `WarpByCommand` table @31d710d4: 15 20008, 16 1007, 17 1008, 18 20009, 19 1500, 20 1501; tail
  (skip in SQUARE_GAME, set state arg, field-change req) = March's.
- **`this+0x3c0` omission:** byte sweep (`dispsweep.py 3c0`): in June's field game only two writers (`= 1` @323baf0e / @323bb75a in setup
  323bad00) and OpenFieldName's clear; no byte reader anywhere. Write-only in June: not porting it holds.
- **Data (subagent diff, spot-read):** Village[1008] = June 4367-4423 apart from the two key renames (rows 401 / 403); the 3 house rows +
  `HI_YUNO` + `NUI_YUNO` + WI 637 verbatim; MapData row + Cobo zone 17 = June, server mirror 31 inserted / 0 deleted lines, each block in
  its neighbours' EOL (AddMapData LF, Cobo CRLF); world map = June 1842-2081 plus one 7-line hunk (comment, 4 `SetShow(false)` on the
  ELYSION_TOWN / FIELD_BATTLE_03 button + static, blank), `99999` gone, cloud keyed to 1008 as in June.
- **Hygiene:** `flagcheck ELYSION_PORT --range f5951bbc^..f5951bbc --files <8 code files>`: `files: 8 PASS, 0 PASS~, 0 DEFINE, 0 OTHER,
  0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s),
  0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (X2Main.cpp's one hunk is this flag's, RANOX_PORT nested).
  `LEDGER CHECK PASS: 0 FAIL, 0 WARN`; `CORE CHECK PASS: 0 FAIL, 0 WARN`.
- **Kickoffs:** `A6.md` written by this review; `A7.md` exists (peer) and keeps the town / F03 pins hidden until E2 / E3. A5's play-test
  checks re-run with A7's (1008 has no village templet before it).

---

## Phase A6 — Lanox→Atlas crossing + Fields 01/02

**Steps**
1. **Crossing:** add the Lanox F04-row portal (`LINE 103` → 1008 pos 400) and
   June's `BATTLE_FIELD_DIRECTION_INFO` block **verbatim, inert, with a
   `-- quest plan:` comment** (March has no reader). Restore the
   `DLG_UI_FIELD_MAP_Ranox_04_NEW` right button. Confirm Atlas Station's
   `START_POS` 401 → Lanox line 109 brings the player back.
2. **Fields 01/02:** `BattleFieldData` rows (`Field_Elysion_Stage0/stage1`
   + LineMaps; NPC tables `Elysion_Field_01/02.lua`, data036 → `update/`),
   `WorldData` (638/639), field-map dialogs (`DLG_UI_FIELD_MAP_Elysion_01/02_NEW`),
   world-map pins (from A5's block), loading images, music.
3. **Field monsters** (`NUI_ELSION_F_*`, `NUI_ELYSION_F_HERNA_*`,
   `F_BOSS_*`, `NUI_ELSION_CHEST`, `NUI_ELYSION_HERNA_SHIELD`,
   `NUI_ELYSION_NEW_CHEST`): unit scripts (data037), `NPCTemplet` rows,
   `NPCStat`/`NewNPCStat` readiness, effect sets, sounds. Run the buff census
   over them (*buff-dependency rule*).
4. F02's exit to the village (`L142 → VMI_ELYSION` pos 500) is **omitted with
   a comment** until E2 restores it.
5. Offline: field monsters and drops come from `BattleFieldData` + the NPC
   tables + `X2OfflineDropTable` (no IDs hardcoded); check the join-ACK
   start-position quirk (`Handlers_Room.cpp:1278`, `:1365`) on these fields.

**Done when:** census `--line FLD` (F01/F02) clean except buff-owned items;
build rc=0; staged + pack list. After pack: Lanox F03 → portal → 1008; 1008 ↔
F01 ↔ F02; monsters spawn with HP and drop; F02 has no dead exit.

**Run with:** Opus 5.5, high effort. Required review.

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch main) in full -- "Starting context", the
ledger's "Topology" and "Lanox-side omissions" sections, the buff-dependency
and unknown-item rules, and "Phase A6". Never launch X2_offline.exe without
asking. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase A6): add the Lanox F04-row portal into Atlas Station (plus
June's BATTLE_FIELD_DIRECTION_INFO verbatim and inert, commented for the
quest plan) and the Ranox_04 field-map right button; port Fields 01 and 02
(BattleFieldData, scenes, NPC tables, WorldData, field-map dialogs, pins,
assets) and their monsters (unit scripts, NPCTemplet, NPCStat/NewNPCStat,
effects); leave F02's village exit omitted with a comment.

Premises to verify first: which Lanox row and linemap line really carry the
portal (the June names are offset by one); NPC readiness of every field NUI.

Done when: Phase A6's "Done when" holds. Record a Done note (DEPLOY OK commit), then commit on main.
```

### Phase A6 — Done

**2026-10-17, Opus 5.5 high.** Flags `SERV_IRUHADEV_ELYSION_PORT` (A3's) + new `SERV_IRUHADEV_OFFLINE_BF_JOIN_START_POS`. Commit: `27454383`. After commit, `flagcheck --range 27454383^..27454383 --files <5 code files>`: ELYSION_PORT `3 PASS, 2 OTHER, 0 FAIL, 0 ERROR`, OFFLINE_BF_JOIN_START_POS `1 PASS, 1 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`, both `RESULT: PASS`, same in-flag line as below. MODS.md "Atlas/Elysion port A6".

- **Premises:**
  - The portal is the F04 row (`VMI_BATTLE_FIELD_RANOX_FIELD_04` = 40032, `Field_Ranox_Stage3_LineMap` / `Ranox_Field_03`, WI 608): line 103
    is `LT_POTAL` (June linemap = March sidecar).
  - Atlas Station 401 -> 109 and 403 -> 333 name `AddStartPos` keys, not lines.
  - NPC readiness: all 30 field NUIs were already in March's Enum, unit scripts (data037, ljconv), `NewNPCStat` and meshes
    (`npc_mesh_census --all-npc` 0 missing). Only `NPCTemplet` rows were absent.
- **Crash root cause (A7 play-test):** with no 40035 row, `GetWorldIdByBattleFieldId` returned 0, the world was NULL, and
  `IsNearPortalLineMap` dereferenced `GetLineMap()` (`X2BattleFieldGame.cpp:529`). The rows fix the trigger; June's own NULL return
  (`32d53810`) is ported too.
- **Join-ACK start-position quirk** (the real site is `Handlers_Room.cpp:1384-1387`, not :1278 / :1365):
  - The emulator zeroed `m_bNowBattleFieldPositionInfoStartPosition`, so the client read start-pos key 333 as a line index on Field 01's
    163-line map, and the player was never placed.
  - The fix echoes the request, as `GSUserBattleField.cpp:466` does, under `OFFLINE_BF_JOIN_START_POS`. It changes landing on every
    field (the key's spot, not a line's).
- **C++ (`ELYSION_PORT`, June addresses in each block):**
  - The 2260-2262 code-event gates (inert offline) in `IsNearPortalLineMap` `32d53810`, `CreatePotalParticle` `32d51be0` (Ghidra's
    `32d51960` carve is wrong) and `PhysicProcess_PortalInBattleField` `3201a670`.
  - `CreatePotalParticle` also gets June's second particle, `UI_Portal_Arrow` (`g_pData+0x7c`, +50), and a `SHOW_PORTAL` constant
    (W2 parses the key).
  - `FieldWarpByCommand` `31d71130` (jump table @31d71550): 27-30 Ranox, 35 / 38 F01 / F02.
  - Omitted with a comment: 31 (40028), 36 / 37 / 39-41 (raid stages, no enum), 42 / 43 (E3: no rows yet).
  - The dungeon twin `PhysicProcess_Portal` `3201a2b0` carries W2-type portal-line changes, routed to `kickoffs/june-engine/W2.md`.
- **Lua (BOM source, ledger refreshed, `ledger --check` / `core --check` PASS):**
  - data036 `BattleFieldData` (converted T0): June's F04 portal 103 -> 1008 pos 400, `BATTLE_FIELD_DIRECTION_INFO` verbatim with a
    `-- quest plan:` comment, and F01 / F02 rows verbatim. F02's `L142 -> VMI_ELYSION` 500 is commented out until E2; its line keeps
    the particle but does nothing (`CBattleFieldPortalMovingInfoPtr is NULL!` in State.log).
  - data037 `NPCTemplet`: 26 rows (16 `F_*`, 5 `F_HERNA_*`, 2 `F_BOSS_*`, 2 chests, `HERNA_SHIELD`; June ranges in the block header).
  - data040 `WorldData` 638 / 639.
  - data034 `DLG_UI_FIELD_MAP_Ranox_04_NEW` (converted T0): June's text (right button to 1008, left `AddDummyInt`).
- **June files (ljconv):** `Field_Elysion_Stage0` / `stage1` + line maps (data040), `Elysion_Field_01/02` (data036),
  `DLG_UI_FIELD_MAP_Elysion_01/02_NEW` (data034). Record: `june2017_re/june_adopt_20261017_060423.tsv`.
- **Assets:**
  - 338 census FLD assets were staged by `stage-june`: data046 1, 078 1, 096 7, 179 2 (new kom), 180 134, 181 17, 182 176. All are new
    names; nothing was replaced.
  - A texture closure over the 151 staged meshes / xet names 258 textures. **Not yet checked against March; A11 owns the closure.**
  - 3 loose music files (`field_elysion001/002.ogg`, `li_field_elysion001.ogg`) go to `data\music` with the copy.
  - The F03 / PvP-field assets that share the FLD tag rode along; their Lua stays with E3 / E12.
- **Census `--line FLD`:** 483 -> 119 open, all routed:
  - 4 `npc.ready`: non-F `NEWDEFENDER` -> A9, `HIGHTTEM` -> A10, `EVENT_00/01` -> quest plan (decision 2).
  - 5 unstaged + 5 Lua adopts: F03 -> E3, PvP field -> E12.
  - 3 music files are loose, and the census reads koms only.
  - 13 `assets.absent`: June ships none.
  - 5 `keys.registry`: read by the emulator / server; `MIDDLE_BOSS_BONUS_ITEM_COUNT` is unread in March's own rows too.
  - 84 `keys.file`: `LINE_NUM` = B73, and `CT_*` / `CF_*` / `TARGET_FRONT_ME` are Lua-side names in June's own unit scripts.
- **Drops: routed to backlog Stage 23 (user, 2026-10-17; brief `kickoffs/june-backlog/stage23-plan.md`).** The server `DropTable` / `StaticDropTable` stop at 40025 (no Lanox, no Elysion rows), so F01 / F02 kills
  log `no reward row ... grants nothing` (no EXP, ED or items), as Lanox fields do today.
- **Hygiene:**
  - `flagcheck ELYSION_PORT --worktree --files <5 code files>`: `files: 3 PASS, 0 PASS~, 0 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`.
    The 2 OTHER are Always.h's and Handlers_Room.cpp's `OFFLINE_BF_JOIN_START_POS` hunks.
  - `OFFLINE_BF_JOIN_START_POS`: `1 PASS, 1 DEFINE, 3 OTHER, 0 FAIL`.
  - Both: `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token,
    0 encoding change(s)`, `RESULT: PASS`.
- **Pack:** `PACK OK` data034 / 036 / 037 / 040 / 046 / 078 / 096 / 179 / 180 / 181 / 182. The data036 pack warns about 24 older
  `.src.lua` sidecars, not A6's. **Copied (user OK):** `COPY OK` data034 94490be6 / 036 8a709bcb / 037 6f9549cb / 040 c92d2288 / 046 c5921e14 / 078 4b7a51a0 / 096 0120127f / 179 5b2300ce (new) / 180 bbc42839 / 181 a07dc70f / 182 881dea8a; the 3 music files copied into `data\music`, md5 = June's.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,606,528 B, dated 2026-10-17 06:31:02, md5 c1c45191e375bed74b3e286ef606081c, equal to X2/US_SERVICE/x2.exe (2026-10-17 06:30:57); built at main adad9359 + 5 uncommitted code file(s).`
  The 5 files are A6's. Proof: `UI_Portal_Arrow` (only in A6's `#ifdef`) is in the deployed exe.
- **Play-test 2026-10-17 (user; run 06:36-06:39 on exe c1c45191 + the 11 koms):**
  - The crossing works: Distant Ruins 40032 -> 1008 -> Field 01 (29 monsters placed) -> Field 02 (40). No crash.
  - Finding 1, many missing assets: ~120 `OpenTexture` lines (Elysion world props, field-monster skins) and a few `OpenXSkinMesh`.
    The census lists only files Lua names, so textures embedded in meshes were never on a list.
  - Finding 2, a floating "inner circle" over every portal: this is June's second portal particle `UI_Portal_Arrow`. It is drawn with no texture,
    because March lacks `UI_Portal_Arrow.tga` (June data002), so the circle's texture shows through. The +50 offset is June's
    (`32d51e29`, 50.0); the template is a bobbing marker, identical in both clients' `UIMajorParticle.txt`.
- **Follow-up, region-wide asset closure (user: "safe to port all atlas/elysion assets at this point"; this is A11's / E13's work pulled forward):**
  - Method: every asset the census names for any region line, plus the textures those meshes / xet embed, to a fixed point.
    Evidence: `june2017_re/atlas_a6_region_closure.tsv` (2,064 rows).
  - Result: 1,954 June-only files staged (1,325 named, 653 embedded, by-name duplicates merged), all new names, 0 replaced, into 21 koms:
    data002 / 012 / 013 / 023 / 057 / 070 / 081 / 094 / 096 / 104 / 151 / 178 (new) / 179 / 180 / 181 / 182 / 183 / 197 / 215 /
    216 (new) / i027 (new). `PACK OK` x21. **Copied (user OK):** `COPY OK` data002 ff3ea25e / 012 58429073 / 013 9c574bf2 / 023 9a49f39a / 057 778f8f78 / 070 a1156674 / 081 e4f83cfd / 094 fb376e05 / 096 03029ddf / 104 8925a22b / 151 21f0e9d6 / 178 c074a4f7 / 179 d4b0b75c / 180 127ce514 / 181 1ac96752 / 182 6c35a6c3 / 183 432c73b6 / 197 f59d097a / 215 8b6f5965 / 216 c18a277a / i027 d4289688; the 15 music files copied, md5 = June's.
  - A re-run of the closure finds 0 left to adopt. 86 names June itself does not ship (73 named, 13 embedded) stay absent.
  - 15 June music files missing from `data\music` (li_boss_elysion003-005, li_stage_elysion001-007, li_village_elysion001,
    music074_stage, music076_stage, music077_boss, music078_stage) go in with the copy.
  - Not covered: particle-templet textures (the effect closure is june-effect's) and Lua (each line's phase adopts its own).
- **Re-test 2026-10-17 (user, run 06:49):**
  - Textures fixed. The run logs 18 missing files, none of them region assets: Stage 10 emotion motions, `HQ_*` logos,
    `ELYSION_FIELD_ROAD10.DDS` (June lacks it), and a few that predate A6.
  - The floating pink circle was still there: `UI_PORTAL_ARROW.TGA` still failed (ErrorLog 06:49:06). The closure skips particle-only
    textures, so the file was never staged; the A6 note claiming it was staged was wrong.
  - Fix: staged June's `UI_Portal_Arrow.tga` (data002, 65,580 B, a yellow down arrow). `PACK OK`; `COPY OK data002` md5 ac37a114
    (under the user's copy OK for the region koms).
  - **Play-test PASSED 2026-10-17 (user):** the June arrow shows over the portals, and the textures are fixed.
- **How to see it in game:** the A6 STATUS row / hand-off. Lanox F04 (Distant Ruins) far-end portal -> Atlas Station. Atlas Station's
  far portal -> Field 01, landing on start pos 333. F01 L53 -> Field 02. Each return portal works. Monsters spawn with HP bars. F02's
  far end has an inert portal.

---

### Phase A6 — Review (2026-10-17)

**Opus 5.5 high. PASS: no transcription error, no code change, no build, no pack** (deployed exe stays c1c45191 or a later peer build
carrying `27454383`). Ghidra + `funcdis.py` over `unpacked_x2.exe`, read-only; floats, strings and the jump table read from the file.

- **`IsNearPortalLineMap` 32d53810:** June returns false on no world / no line map (`+0x90`, `+0x34`); port's guard is equivalent.
  Gate: `g_pInstanceData` (33775864) set -> 2260 (0x8d4): WI 608 (0x260) + LT_POTAL + line 0x67 returns 2262; else 2261 (0x8d5):
  WI 638 (0x27e) + LT_POTAL + line 0x35 returns false; it sits before the line-type switch. Port equal.
- **`CreatePotalParticle` 32d51be0** (capstone, Ghidra's 32d51960 carve skipped): `SHOW_PORTAL` byte `+0x14c` == 1 else skip (port: constant
  true until W2). 2260: WI 637 (0x27d) **any line**, or WI 608 + line 103, -> `+0xcd4` = (2262 on), `continue`; 2261: WI 638 + line 53
  -> `+0xcd4` = 0, `continue`; 2260 on with no match falls through (no 2261 test). First particle `Peita_Teleport_MagicSquare01`
  +3.0; the second, `UI_Portal_Arrow` (33527694) from `g_pData+0x7c`, +50.0 (33449120), is made even when the first fails; both
  layer 0xe = `XL_EFFECT_0`. Port equal.
- **`PhysicProcess_PortalInBattleField` 3201a670:** gate after the timer and the `+0x21e8` sync clear, line index only (no type test),
  `return` before the teleport. Port equal. Which-is-which holds: 3201a670 is the battlefield twin (teleport only); 3201a2b0 carries
  the same gate inside the dungeon-game path, routed with W2's changes (`kickoffs/june-engine/W2.md:51`). Agreed.
- **`FieldWarpByCommand` 31d71130, table @31d71550 (43 entries):** 1-26 = 40000-40025 as March; 27-30 = 40029-40032; 31 = 40028
  (omitted); 32-34 default; 35 = 40035; 36 / 37 / 39-41 = 40040-40044 through the raid path (`+0x1160` = 5-8; omitted); 38 = 40036;
  42 / 43 = 40037 / 40038 (E3). Port equal; `SERV_IRUHADEV_RANOX_PORT` is on (Always.h:3899).
- **Join ACK:** the live servers copy the whole `KBattleFieldJoinInfo` (`GSUserBattleField.cpp:466`, `BattleFieldRoom.cpp:1065`), so the
  echo is what they did. The 3-arg custom-position REQ sends `false` (`X2State.cpp:11296`) and gets it back; the 2-arg REQ sends
  `true` (`:11339`). March / Lanox fields now land on their start-pos keys, as live did; the A6 / A7 play-tests crossed them.
- **Data** (subagent token compare against `june_src_fixed`, comments stripped): Lanox F04 block = June :1599 (portal 103 -> 1008
  pos 400, DIRECTION_INFO verbatim, `-- quest plan:` comment); F01 = June; F02 = June minus only the L142 -> `VMI_ELYSION` 500 portal,
  kept in a comment; 26 / 26 NPCTemplet rows value-identical at their cited ranges; WorldData 638 / 639 = June; Ranox_04 dialog = June
  plus one 2-line comment; the 8 adopted files are `1B 4B 4C 81` at the recorded sizes, none on the core list.
- **Comment-only nits, left as is** (fixing them costs a data036 repack and copy): `BATTLE_FIELD_DIRECTION_INFO` cited June :1641,
  starts at :1642; the F02 portal comment cites June :2122-2127, the portal is at :2125-2130.
- **Hygiene:** `flagcheck ELYSION_PORT --range 27454383^..27454383 --files <5 code files>`: `files: 3 PASS, 0 PASS~, 0 DEFINE, 2 OTHER
  (other flags), 0 FAIL, 0 ERROR`; `OFFLINE_BF_JOIN_START_POS`: `files: 1 PASS, 0 PASS~, 1 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR`;
  both `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif
  without the flag token, 0 encoding change(s)`, `RESULT: PASS`. `LEDGER CHECK PASS: 0 FAIL, 0 WARN`; `CORE CHECK PASS: 0 FAIL, 0 WARN`.
- **Open for the user:** nothing. Next kickoffs already exist (A7 done; A8 and E1 written).

---

## Phase A7 — Local map, Atlas half

**Steps**
1. Registries: `DLG_Map_World_Elios.lua` `LocalIDList` + `DLG_Map_Local.lua`
   `Local[LMI_ELYSION]` (lift the Lanox omissions), `MapData`
   `AddLocalMapInfo(LMI_ELYSION, …)` rows (21 N/H/E; the two
   `DI_HERO_DUMMY_*` rows are Hero → omit with comment).
2. `data217/DLG_Map_Local_Elysion.lua` (inert `RequireCompletedQuestID` kept)
   + `data034/DLG_Map_Local_Elysion_Back_NEW.lua` + `data217/…_Icon_NEW.lua`.
   The scroll buttons (`MOVE_UP/DOWN`) and guide belong to E4 — until then the
   dialog shows the Atlas page (page "0" is the default-shown one in June) and
   the scroll buttons are hidden with a comment, not dead.
3. Tab strip: restore `LocalMapRadioBtn9` (+ its `QuestGuide` picture if June
   draws it unconditionally) in `DLG_UI_Local_Map_NEW.lua`, and party-length
   picture 9. The tab must pass the level/clear-dungeon test March reads.
4. Bindings: `GetNationFlag` (+ the `NF_*` globals already exist,
   `X2Main.cpp:3345`) and `SetChangeUVPoint`, under
   `SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION`.
   *(june-engine U0, 2026-10-16)* June's `SetChangeUVPoint` is `31c9a7d0`
   (bound @`31bf7c70`). It owns a `CPictureData +0xb0` block (a vector plus a
   functor) that June's `SetPoint_LUA` (`31c9a700`) resets at its end
   (`31c9eea0`). Whichever of A4 / june-engine U1 lands second adds that reset.
5. TET atlases: `DLG_Localmap_Title_01` (Atlas title + `Title_Elision_01..03`;
   merge vs adopt per the corrected rule — `LANOX_REMAINING_PHASES.md` P2
   history), `_01_A` only if a nation branch reaches it under US (it doesn't:
   `NF_DE/ES/IT` only), dungeon images (`Image11` live in `datai022`; check
   mount order), star statics' `STR_ID`s.

**Done when:** census `--line UI` clean for the Atlas page; build rc=0;
staged + pack list. After pack: page 2 shows the Elysion tab at level ≥ 78
with Lanox Final Hall cleared; the Atlas page lists AD/AC/TW with titles,
stars and images; buttons honour the unlock chain.

**Run with:** Opus 5.5, high effort. Required review.

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch main) in full -- "Starting context", the
ledger's "Gates", "June-only engine" and "Assets" sections, and "Phase A7".
Also read JUNE_UI_PARITY_PHASES.md "Phase 0 findings" (function map, nil-
member hazard) and its Phase 2/3 results. Never launch X2_offline.exe without
asking. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase A7): register LMI_ELYSION (DLG_Map_World_Elios,
DLG_Map_Local, AddLocalMapInfo), port DLG_Map_Local_Elysion + Back_NEW +
Icon_NEW with the Atlas page working and the scroll/guide hidden with a
comment until Elysion E4, restore LocalMapRadioBtn9 and party picture 9, bind
GetNationFlag and SetChangeUVPoint under SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION,
and stage the title/dungeon-image TET atlases.

Premises to verify first: that no AddDummyInt in the adopted files resolves to
a nil enum member (dead-tab hazard); the winning mount of every TET atlas.

Done when: Phase A7's "Done when" holds. Record a Done note (DEPLOY OK commit), then commit on main.
```

### Phase A7 — Done

**2026-10-17, Opus 5.5 high.** Flag `SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION` (MODS.md "Atlas/Elysion port A7"). Commit: `6b988787`. After commit, `flagcheck --range 6b988787^..6b988787 --files <7 code files>`: `files: 6 PASS, 0 PASS~, 1 DEFINE,
0 OTHER, 0 FAIL, 0 ERROR`, same in-flag line, `RESULT: PASS`.
Kickoff `kickoffs/atlas/A7.md` (written this phase).

- **Premises:** no `AddDummyInt` in the adopted files resolves to a nil *tab* id: `LMI_ELYSION` = 10009 (A3), every Atlas/Elysion `DI_*_NORMAL`,
  `DLT_DUNGEON`, LMUCM 18000-18007 exist. The 3 nil members are the Icon file's Hero dummies (`DI_ATLASDEVELOPMENT/ATLASCITY/TOWER_HERO_NORMAL`,
  third dummy): they read 0, no DungeonData, so the icon stays hidden in hero mode only (`X2LocalMapUI.cpp:1281` note). Winning mounts: every
  atlas the page uses is in one kom only; `DLG_Localmap_Title_01` (data063) is byte-identical to June's (data151) and has all 9 keys.
- **Loader facts (subagent, spot-checked):** `LocalParsing` reads `VillageData`, `BattleFieldRestData`, the two lists, `RequireUnitLevel`,
  `RequireClearDungeonID` (never tested by the tab or world map: June neither), not `RequireCompletedQuestID` (decision 2, nothing to gate).
  A missing `Village[id]` row = popup + `return` with the `"Village"` table left open, so every later village failed too: June's file names
  20009 before E2 has its row. **User (2026-10-17): guard + gate** (stand-ins below).
- **Lua (BOM source, `luasrc.py convert` T0, edited, refreshed; core rows say `edit: atlas A7`):** data034 `DLG_Map_Local` `Local[LMI_ELYSION]`,
  `DLG_Map_World_Elios` `LocalIDList` + LMI_ELYSION (LMI_EVENT still omitted), `DLG_UI_Local_Map_NEW` June's `LocalMapRadioBtn9` + QuestGuide
  picture verbatim (JF data217 :520-556, :761-772); data036 `MapData` June's 21 `AddLocalMapInfo` rows (Hero rows omitted with a comment; the
  binding is a no-op offline, `X2OfflineMapData.cpp:159`) + server mirror (22 lines, LF). June's `DLG_Map_Local_Elysion`, `_Back_NEW`,
  `_Icon_NEW` adopted as ljconv bytecode (no edits). Party picture 9 (`ELYSION` plate, LMI_RUBEN + 9) was already in March's staged
  `DLG_UI_PARTY_LENGTH_NEW` (June UI parity). Assets: `DLG_New_Region_Elision_01.dds/.tet` (data065), `DLG_Common_Button06_NEW.TET/.tga`
  (data215, Memory's `M_DUNGEON_*`), sizes = June's.
- **C++ (June's code, `#else` keeps March):** `UpdateDungeonButtons` June's page filter @3293bc66-3293bd65 (dummy string "0"/"1" vs page
  +0x54 = new `m_iLocalMapPage`, 0 until E4); `LocalMapMoveUp/Down` hidden where June runs the scroll @3293c146-3293c16a (E4); `UpdateLocalMapDLG`
  event-code 2260 branches @3293c8d9-3293c91f / @3293c97e-3293c9b7 (inert offline).
  **Stand-ins (user, 2026-10-17), idle once E2 / E3 land:** `VillageParsing` logs `[ELYDBG] VillageParsing: no Village[%d] row`, closes the
  table, no popup; Cobo + warp-NPC lists skip `Village_20009` while 20009 has no village templet; the world map keeps the Elysion town pin
  (no village templet) and Field 03 pin (no battlefield data) hidden with their locked art.
- **Not reachable until later phases:** Atlas page icons (AD / AC / TW) stay hidden until A8-A10 add their DungeonData; titles show now.
  `RequireClearDungeonID` is not a tab gate in June either: the tab opens at level 78.
- **Census `--phase A7`** (the whole UI line): local-map rows left = `dlg_localmap_title_01_a.tga` (June ships none; NF_DE/ES/IT only:
  JUNE_BUGS 206), `DLG_Elysion_LocalMap_Guide.lua` (E4), the 3 Hero members (out of scope). The rest of the UI line is other phases'.
- **Hygiene:** flagcheck `--worktree --files <7 code files>`: `files: 6 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII
  inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
  `ledger --check` PASS, `core --check` PASS (0 WARN). First build failed (`X2WorldMapUI.cpp` lacked the `X2OfflineLog.h` include; added
  under the flag + `ELYSION_DEBUG`).
- **Pack / copy (user OK):** `PACK OK` + `COPY OK` data034 (md5 76159963) / 036 (b916aa51) / 065 (f9e0f570) / 215 (241f0f01) / 217 (dcc2cc48).
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,605,504 B, dated 2026-10-17 05:33:03, md5 45e6d1ae2569514980f48d1ed4e77db6, equal to
  X2/US_SERVICE/x2.exe (2026-10-17 05:32:57); built at main 49687519 + 7 uncommitted code file(s).` (A7's 7 files). Proof: the UTF-16LE
  literals `[ELYDBG] local map Elysion`, `[ELYDBG] world map: pin`, `[ELYDBG] VillageParsing: no Village`, `LocalMapMoveUp` are in the deployed exe.
- **How to see it:** M (local map) on a level ≥ 78 character -> right arrow -> Elysion tab -> Atlas map art, Atlas titles + 3 Atlas title plates,
  no up/down buttons; `atlas` warp now loads Atlas Station (A5's checks); Cobo lists Atlas Station, not Elysion; world map shows rest / F01 / F02
  pins at ≥ 78, no town / F03 pin. Log: `[ELYDBG] local map Elysion`, `[ELYDBG] world map: pin 20009 / 40037 hidden`, `VillageParsing ... 20009`.
- **Play-test 2026-10-17 (user; run 05:35-05:41 on exe 45e6d1ae + the 5 koms):** all three `[ELYDBG]` lines fired (local map page 0,
  world-map pins 20009 / 40037 hidden, `VillageParsing: no Village[20009]` in ErrorLog); `atlas` and both Cobo warps into 1008 loaded
  (State.log `XS_VILLAGE_MAP` 05:36:14 / 05:37:25, from 40029 and 40032). Findings, routed (user):
  (1) "warp from Distant Ruins (40032) hangs" = the walk-in portal at the field's end, which March's `BattleFieldData` lacks: **A6** (the
  crossing). (2) Atlas Station's board is the regular one: June swaps mesh + UI per village through `DLG_Map_House_NPC_RelationData.lua`,
  which March has no loader for: **E1, run next** (kickoff `E1.md` carries the rows and missing files). (3) Atlas Station's far-end portal
  into Field 01 (40035, `FIELDNPC field 40035 is not in BattleFieldData.lua`) crashed the client: accepted until **A6** (user: no stand-in);
  reyaa's save had `last_pos 40035`, reset to 1008 / line 17 / pos 14897 (Freils' spot; backup `db_backup\els_db_20261017_054509_pre_manual.sql`).

### Phase A7 — Review (2026-10-17)

**Opus 5.5 high. Verdict: PASS, no code change, no build, no pack.** Reviewed commit `6b988787` against June (Ghidra, read-only).

- **Page filter (June `3293b5e0` @3293bc66-3293bd65): equal.** Two `GetDummyString(0)` reads (`31c964a0`) compared with `31cb2510`
  (returns `cmp == 0`, i.e. `==`) against "0" @33458f54 and "1" @33489510; on a match `_itow_s(this+0x54, buf, 10, 10)` and `31cda210`
  (`cmp != 0`, i.e. `!=`) jump to the loop's continue @3293bfd4. It sits right after the custom-event test (@3293bc4c-3293bc60,
  `JZ 3293bfd4`), as March's insertion does; the control was hidden earlier in the same loop (March :1298).
- **Event code 2260 in `UpdateLocalMapDLG` (`3293c6f0`): equal.** @3293c8d9-3293c91f looks up 0x8d4, then the control named by
  @334c85d0 ("Elysion"), and calls vfunc +0x78(0,0); this runs before the `-1 == dummy` test (@3293c920), as in March. @3293c97e-3293c9b7
  comes after `SetShowEnable(1,1)` and `GetLocalMapTemplet`. Under 2260 the 0x2719 tab is disabled, and every tab jumps to @3293cde3
  (the loop increment): no quest mark, no level gate, no tab count. March's `continue` matches. Inert offline.
- **Scroll stand-in: right.** @3293c146-3293c16a calls `329378a0` only for 0x2719, with 1 or 2 from `this+0x54`. That function runs the
  `Elysion_Local` move (`31c95150`, `3293dc30` with callback `3293c1e0`) and writes the new page back to +0x54. That is a real scroll, so it
  belongs to E4. Until E4, page 0 plus both move buttons hidden is the only state that is not a dead button.
- **Stand-ins (user, 2026-10-17).** (a) `VillageParsing`: the fail path runs with `BeginTable("Village")` open and `BeginTable(id)`
  failed (so not pushed). The one `EndTable()` closes exactly "Village"; nothing has been allocated or inserted yet (`m_mapVillageTemplet`
  insert at :525, inside the success branch). (b) Cobo and warp-NPC lists: `X2StateMenu` adds no `++row` in either branch (20009 is the
  last row, as in March's `#else`), and `X2UIInventory` increments only when the row is added. `row` stays consistent. (c) World-map gate:
  the same locked-art loop as March's `NO_SANDER_VILLIAGE` block (`dummy 1 == 1` shown, the rest hidden), placed after the templet
  NULL test. Each half goes idle when its target exists: `GetVillageMapTemplet(20009)` (E2), and `GetBattleFieldNameByBattleFieldId`
  (returns 0 on a miss, `X2BattleFieldManager.cpp:526`) for 40037 (E3).
- **NULL village-templet readers (subagent sweep, spot-checked).** `m_mapVillageTemplet` is reached only through `GetVillageMapTemplet`
  (`find`, else NULL). Every reader that can see 20009 NULL-checks the result: quest NPC maps `X2UIQuestNew.cpp:2862` /
  `X2UIQuestReceive.cpp:2733`, TFieldGame, community, party, minimap, world-map tooltips, and the last-position fallbacks (which go to
  Ruben). The one unchecked deref, `X2QuestManager.cpp:1233` (board quests), takes `BoardVillageID` from a switch with no Elysion case
  (checked: :1186-1226), so it never sees 20009. **E1 / E2 note:** adding an Elysion case there needs E2's templet first.
- **Data (subagent, spot-checked): all equal.** Btn9 (March :536-567 = JF data217 :525-556) and the QuestGuide picture (:751-761 = JF
  :762-772) diff empty. `Local[LMI_ELYSION]` and `LocalIDList` equal June's except June's `LMI_EVENT` (still omitted). MapData has 21 rows,
  equal to and in the same order as JF :576-598 minus `DI_HERO_DUMMY_COMMON` / `_HELL`. The server mirror has the same 21 rows. The three
  June files are byte-equal to `ljconv(June)` (header `1B 4B 4C 81`; sha1 `25c23818` / `f3ea5b1c` / `9f6b5f74` = the adopt tsv). The four
  atlas files equal June's in size and sha1 (junekom, byte-exact). JUNE_BUGS 206 is correct (US is none of `NF_DE/ES/IT`).
- **Omissions, each still true.** Hero rows and dummies are out of scope. `LMI_EVENT` is absent from both registries. The guide window
  (`DLG_Elysion_LocalMap_Guide.lua`) is E4. The quest test (June @3293cbe0, RequireCompletedQuestID 22218) is decision 2, and the loader
  never reads the key (`X2LocationManager.cpp`: no hit).
- **Hygiene.** `flagcheck JUNE_LOCAL_MAP_ELYSION --range 6b988787^..6b988787 --files <7 code files>`: `files: 6 PASS, 0 PASS~ (blank/marker only),
  1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched
  inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `LEDGER CHECK PASS: 0 FAIL, 0 WARN`;
  `CORE CHECK PASS: 0 FAIL, 0 WARN`.
- **Spot-checks in Ghidra (3):** both string compares (`31cb2510` / `31cda210`), the @3293cde3 continue target, and the `329378a0` scroll.
  All agree, so no whole-phase re-check was needed. Nothing is owed by the user. A8 kickoff written (`kickoffs/atlas/A8.md`; it needs A6).

---

## Phase A8 — AtlasDevelopment line (pattern-setter)

**Steps.** This phase writes the **per-line checklist** into this doc (the
section *Per-line checklist* below) while doing it, so A9/A10 and E8–E11
can follow it.
1. `Dungeon_Elysion_AtlasDevelopment_{Normal,Hard,Expert}.lua` (data035),
   `Elysion_AtlasDevelopment_{Stage0,1,2,BossStage}` + LineMaps (data040).
2. `DungeonData.lua` rows (verbatim, incl. the inert quest key with a comment;
   Hero rows omitted with a comment): loading screens, `m_TextureName`,
   `m_PieceName`, `EXTRA_REQUIRE_DUNGEON_ID`, `BOSS_DROP_ITEM` (unknown-item
   rule).
3. Unit scripts for the line's NUIs (Appendix A, AD), `NPCTemplet`,
   `NPCStat`/`NewNPCStat` readiness, boss-gauge face (`BOSS_GAGE_FACE_TEX*`
   → `DLG_BossState*` keys), effect sets / damage effects / mesh players /
   particles through the June effect pipeline (merge, never replace —
   shared-staging rule), sounds (data096, music).
4. Reused Lanox NUIs (`FIRE_DEVIL_*`, `FIRETEMPLE_GATE`, `MOUNT_FIREBIRD_*`)
   are already live — confirm their stats/effects, don't re-port.
5. Offline: clear payout and drops for the line (`X2OfflineDropTable`,
   NpcExpTable 80–82 from A2), dungeon-clear recorded so AtlasCity unlocks.
6. Buff census over the line's scripts; block or proceed.

**Done when:** census `--line AD` clean; build rc=0 (if any C++); staged +
pack list. After pack: AD Normal/Hard/Expert enter from the local map, every
stage loads, monsters have HP and effects, boss gauge shows a face, clear
pays out, AtlasCity's button unlocks.

**Run with:** Opus 5.5, high effort. Required review.

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch main) in full -- "Starting context", the
ledger, "Lanox lessons -> gates", the buff-dependency and unknown-item rules,
Appendix A (AD NUIs) and "Phase A8". Also read RANOX_PORT_PLAN.md Phase 3/4
findings (the Lanox line pattern) and JUNE2017_CHARACTER_PORT_CHECKLIST.md's
effect section. Never launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase A8): port the AtlasDevelopment line end to end (dungeon
scripts, stages, DungeonData rows verbatim, unit scripts, NPCTemplet/NPCStat,
boss face, effect sets via the June effect pipeline, sounds, drops, offline
clear) and write the "Per-line checklist" section into the doc as you go.

Premises to verify first: NPC readiness and buff census for every AD NUI;
that every effect name the line uses exists after the merge.

Done when: Phase A8's "Done when" holds and the checklist is in the doc.
Record a Done note (DEPLOY OK commit), then commit on
main.
```

### Per-line checklist

*(written by Phase A8, 2026-10-17; A9 / A10 / E8-E11 follow it. Opus phases hand steps 1-3 and 8 to Sonnet subagents.)*

1. **Gate first.** `python -I june2017_re/region_census.py --line <XX> --limit 0 --no-probe` (10 min). The rows that need work are
   `koms.unstaged`, `assets.adopt`, `npc.ready`, `items`, `effects`, `bind.*`. The rest is usually already routed (step 9).
2. **Inventory, in staging (`kom_contents`), not git.** Per file: the June kom + size, and the March staged dir + header (`1B 4B 4C 81` =
   ljconv'd June, `EF BB BF` = BOM source) + manifest. Since C3y / C3z most unit scripts, dungeon scripts and boss worlds are
   **already staged**. A8 found only Stage0-2 + 3 line maps missing. Check the three registries for the line's rows:
   `data035 DungeonData`, `data037 NPCTemplet` (not data036), `data040 WorldData`. All three are core-list BOM source.
3. **June-only `.lua`:** `python tools/luasrc.py adopt-june <june kom> X.lua ...` (ljconv bytecode, no edit). Dry-run first. It writes
   `june2017_re/june_adopt_<stamp>.tsv`; commit that file.
4. **Registry rows, verbatim from `june_src_fixed`.** Append each block as `--{{ Iruha : <date> // SERV_IRUHADEV_ELYSION_PORT (atlas
   <phase>): ...` with June line ranges and a `-- To restore: delete this block.` line, then `--}}`. CRLF, BOM kept. A scripted append
   that asserts the first and last lines of each June slice is the safe way (A8: `a8_edit.py` shape).
   - **`DungeonData`:** comment out `m_iRequireCompleteQuestID` with `-- quest plan:`. `IsActiveDungeon` (`X2DungeonManager.cpp:750`)
     would otherwise lock the dungeon forever offline. Keep everything else verbatim, including Ciel / Lu / Ain / Rose keys and
     `EXTRA_REQUIRE_DUNGEON_ID`. Name the omitted Hero rows (C3y-5) and their June ranges in the header.
   - **Self-check (A8 review):** diff each appended block, comment lines dropped, against the June slices; the only allowed difference is
     the commented quest key. Grep the whole file for each new `dungeonID` / `NPC_ID` / `worldID` and expect exactly one hit. (NPCTemplet already
     carries two pre-existing duplicates, `NUI_RANOX_BURNDEDFOREST_GATE` / `NUI_RANOX_ASHESVILLAGE_GATE`; they are not a line's to fix.)
   - **`NPCTemplet`:** add only the rows that are missing. Check that the mesh each row's unit script names in `MOTION_FILE_NAME`
     is staged (`ls kom_contents/*/Motion_X.[xX]`). The row's own `MOTION` key is read only for a dev sheet (`X2UnitManager.cpp:968` / `:1592`),
     and June's can name a file no kom has (A9 review: `EVENT_2ST_BOSS`); keep it verbatim.
   - **`WorldData`:** every `WI_*` the dungeon's stages name. The boss world is often already in (C3y-1b).
   - Then run `luasrc.py refresh <kom> <file>` per file, and append `; edit: atlas <phase> ...` to the file's `why` in
     `tools/data/lua_core_list.tsv`. `core --check` and `ledger --check` must PASS.
5. **NPC readiness:** `Enum` (A3 did all of them), `NPCTemplet` (step 4), and stats. `NewNPCStat` (data036) carries every
   Elysion NUI; `NPCStat` (data037) carries only some, exactly as June. The meshes: the A6 region closure staged them.
   `npc_mesh_census.py` has no read-only mode (it rewrites its tracked TSVs), so check meshes by name instead.
6. **Boss face:** `BOSS_GAGE_FACE_TEX` + `_PIECE` in the boss scripts (`X2GUNPC.cpp:4424-4436`). The pieces are UTF-16LE names
   inside the TET (`data215 DLG_BossState_02.TET` holds the Elysion faces). Search case-insensitively: the lookup upper-cases the key
   (`SetTexureData_`, `KTDGUIControl.cpp:733`), and the TET may store it upper case (Dekal's `NUI_Science_Dekal` is `NUI_SCIENCE_DEKAL` there).
7. **Effects, buffs, sounds** over the unit, dungeon and stage scripts:
   - effect-set, damage-effect and particle names, one level into the effect sets;
   - names built at runtime (`..`);
   - `BFI_` / `BTI_` / `BGI_` names against `Enum.lua` and the buff templets;
   - `.ogg` names.

   A8 found nothing missing. The June effect pipeline (C11a, full `EffectSet`) already covers the region.
8. **Offline:**
   - **Clear:** a clear is recorded with no per-id code (`unit_dungeon`, key = DungeonID + difficulty, `Handlers_Room.cpp:4447`).
     The next dungeon unlocks through its `EXTRA_REQUIRE_DUNGEON_ID`.
   - **Kill EXP / ED / drops:** these need a server `DropTable.lua` row. There are none past 33xxx Lanox or Elysion, so a kill grants
     nothing and logs `no reward row`. That gap belongs to backlog **Stage 23** (field-rewards); do not invent rows here.
   - `BOSS_DROP_ITEM` feeds only the party UI list.
9. **What stays open, routed.**
   - `keys.file` `CT_*` / `CF_*` / state names are Lua-internal condition tables (`X2GUNPC.cpp:6443`, `BeginTable(name)`).
   - `TARGET_FRONT_ME`, `AFTER_IMAGE_DEST_ALPHA_BLEND`, `ANIM_SPPED` and `STOP_TIME_ATK` are never read by June (A1b).
   - `LINE_NUM` = B73.
   - `REWARD_RATE`, `NPC_REWARD_RATE`, `DROP_TIMES` and `CREATED_NPC_DROP_TIMES` are server-only (A1b :1087).
   - `keys.registry` `ENDING_SPEECH0` / `LU` / `CIEL` (A1b :1077-1088).
   - `assets.absent` = June ships no such file either.
10. **Pack, ask, hand off.** Run `kom.py pack` for each touched kom. Ask before each `copy`. A data-only line needs no build.
11. **Elysion-half notes (E8, 2026-10-17).**
    - The staged `NewNPCStat.lua` (data036) and `NPCStat.lua` (data037) are bytecode: check a NUI by its length-prefixed constant, not a text grep.
    - June ships no Hero dungeon scripts: its Hero `DungeonData` rows reuse the Normal / Hard / Expert scripts, so omitting the rows leaves no file behind.
    - `*_HENIR` unit scripts and the `F_*` rows are Henir / field NUIs (C3y-1b, A6), not the line's.
    - A NUI June names in `NPCTemplet` but no June script spawns and March's `Enum` lacks (SC: `EMPEROR_FEAR`, `SET_ELYSION_*_BOW`,
      `HERNA_LANCE_CARD`) is left out with a comment. Rows shared with a later line may come with the first line that names the family
      (E8 added the Emperor SHIELD / ORB / BOW / LANCE rows for E9 / E10).
    - A June key typo that changes behaviour (`CAINVINCIBLE`, `CANDOWN`) gets a JUNE_BUGS row; one that changes nothing stays a step-9 row.
    - June-only assets that carry the line's name but that no script references (SC: 19, e.g. `Herna_Boss_ORB_AttackC_Fire01.ogg`) are
      not the line's to stage; the census `refs` decide.

### Phase A8 — Done

**2026-10-17, Opus 5.5 high.** Data only: no C++, no flag change, no build. Census `--line AD`: 73 open rows before, 50 after,
and those 50 are only the routed classes of checklist step 9 (5 `assets.absent`, 42 `keys.file`, 3 `keys.registry`).

- **Premises held.** AD's 13 NUIs (+ CHEST and the three reused Lanox NUIs) are all in `Enum`, all 12 scripts staged (ljconv, Oct 9),
  `NewNPCStat` has all 13 (`NPCStat` 7, = June), all 10 `Motion_*` meshes in data181. Buff census: 2 names (`BFI_BUFF_DEKAL_DEFENCE_SHIELD`
  665, `BGI_JUST_BUFF` 4), both in `Enum.lua` (:11227, :11720) with templets; no block. Effects: 35 effect sets, 11 damage effects,
  80 particles (one level into the sets) all present, 0 runtime-built names. Sounds: 3 `.ogg` June ships none of; `Music074/075` loose
  in `data\music`. Boss faces: Dekal `NUI_Science_Dekal`, Dekal Pyurita `NUI_ELSION_DEKALPYURITA` in `data215 DLG_BossState_02.TET`.
  Lanox NUIs `FIRE_DEVIL_BERSERKER` / `_FIGHTER_FIRETEMPLE` / `MOUNT_FIREBIRD_FIRETEMPLE`: NPCTemplet, stats, scripts already live (census ready).
- **Staged:** `adopt-june data040` Stage0-2 + their 3 line maps (ljconv; `june2017_re/june_adopt_20261017_071905.tsv`). BOM source:
  `data035 DungeonData` June :29932-30178 (Normal / Hard / Expert; quest key 22218 commented as `-- quest plan:`; 4 Hero rows named and
  omitted), `data037 NPCTemplet` 11 rows (PIMPIM / EVENT_DISION were already in), `data040 WorldData` WI 642-644 (BOSSATGE came with C3y-1b).
  Refreshed, core-list `why` updated; `CORE CHECK PASS: 0 FAIL, 0 WARN`, `LEDGER CHECK PASS: 0 FAIL, 0 WARN`.
- **Pack:** `PACK OK data035`, `PACK OK data037`, `PACK OK data040`. Copy (user OK): `COPY OK` data035 (md5 a613a865) / data037 (a8b97091) / data040 (0a989b6c).
- **Offline:** clears record into `unit_dungeon` as 33330 / 33331 / 33332 with no code change; AtlasCity's rows name
  `DI_ATLASDEVELOPMENT_NORMAL` in `EXTRA_REQUIRE_DUNGEON_ID`, so A9's rows unlock on an AD Normal clear. **Kills pay nothing**: the server
  `DropTable` has no 3333x / NUI 2092-2127 rows (Lanox dungeons 33300 / 33310 neither), so EXP, ED and drops are 0 with a `no reward row`
  log line, and the 30 % clear bonus is 30 % of 0. NpcExpTable 80-82 is ready (A2). The fix belongs to backlog Stage 23 (field-rewards, now
  dungeons too); nothing invented here.
- **Local map:** AD's icon on page "0" turns live once its `DungeonData` exists (A7's filter). `IsActiveDungeon`: level 80+, no
  required dungeon (June gives AD none: the region gate is `RequireClearDungeonID = DI_RANOX_FINALHALL_NORMAL`).
- **How to see it (after the copy):** a level-80+ character, M -> Elysion tab -> AD icon enabled -> enter Normal; Stage0, 1, 2 and the
  boss stage load; monsters have HP and attack effects; Dekal's gauge shows his face; on clear, AtlasCity's icon (once A9 adds its rows) unlocks.
  ErrorLog must show no `DungeonData` / `NPCTemplet` / `WorldData` load errors.
- **Play-test PASSED 2026-10-17 (user):** AD entered from the local map and played through on the copied koms.
- **Open:** kill rewards (Stage 23).

### Phase A8 — Review

**2026-10-17, Opus 5.5 high. PASS; no transcription error, no repack, no copy.** Doc-only fixes: three checklist lines and the Stage 23 scope.

- **Registry rows verbatim.** Each appended block, minus comment lines, was diffed against the `june_src_fixed` slices:
  - `DungeonData` vs June :29932-30178: the one difference is the commented `m_iRequireCompleteQuestID = 22218` (Normal only; Hard / Expert carry none).
  - `NPCTemplet`, 11 rows in 5 ranges: 0 diff. The gaps are PIMPIM, `HERO_WIMP` (Hero tier) and EVENT_DISION, all correct to skip.
  - `WorldData` :3205-3219: 0 diff.
  - No duplicate `dungeonID` / `NPC_ID` / `worldID` for any block id. BOM and CRLF are intact in all three files.
- **Quest key.** Commenting it out is the right reading of decision 2. The decision says March reads none of the quest keys, but
  `IsActiveDungeon` does read `m_iRequireCompleteQuestID` (`X2DungeonManager.cpp:750`, parsed at `:241`), so a live key would lock AD offline.
  The text is kept and restorable, not tidied away. The Hero omission still holds: C3y-5 is `ready`, and DungeonEnum has no Hero AD ids.
- **Adopted files.** The six data040 files match the sha1s in `june_adopt_20261017_071905.tsv`. Each starts with `1B 4B 4C 81` and has
  manifest `"2"`. An `adopt-june --dry-run` re-derive reports all six identical.
- **Premises (spot-checks).**
  - NPC readiness for PLINGCHU_G / BERMEVA / DEKAL: Enum :4673 / :4676 / :4679, the NPCTemplet row, the `Motion_*` mesh in data181, and the NewNPCStat row are all present.
  - Boss face: Dekal's piece `NUI_Science_Dekal` is stored as `NUI_SCIENCE_DEKAL` in the TET (offset 5994). That is fine because the lookup
    upper-cases the key (`KTDGUIControl.cpp:733`). `NUI_ELSION_DEKALPYURITA` is an exact match (offset 3920).
  - Buffs: `BFI_BUFF_DEKAL_DEFENCE_SHIELD` 665 is a factor id; its templet is `BTI_` 664, and both are in `BattleFieldBuff*`. `BGI_JUST_BUFF` 4 is present.
  - Effects: Dekal's 3 effect sets and 2 damage effects are in June's `EffectSet` / `DamageEffect`.
- **Offline claims.** The server `DropTable.lua` `AddMonsterDropInfo` stops at `DungeonID = 33252`, and has no rows for 333xx or for NUIs 2092-2127,
  so "kills pay nothing" is confirmed. The Stage 23 brief and `JUNE_PORT_BACKLOG_ORDER.md` Stage 23 named only fields; both now name the
  dungeons (`AddMonsterDropInfo`) too.
- **Hygiene.** `LEDGER CHECK PASS: 0 FAIL, 0 WARN`; `CORE CHECK PASS: 0 FAIL, 0 WARN`.
  - Census `--line AD`: header 50 open (5 `assets.absent`, 47 `keys.file`, 3 `keys.registry`); every row is in a step-9 class.
    The 47 are 42 `unit:` rows plus 5 rows that span several lines: the four dungeon reward keys and `LINE_NUM`. The class counts add up to 55
    while the header says 50. That is the census's own counting, the same 50 A8 reported, not a new gap; the review did not trace it.
  - `--live`: the same open rows, `live {"same": 280}`.
- **Checklist.** It is complete enough for A9 and E8-E11. The review added three things:
  - step 4 gets a verbatim / duplicate self-check;
  - step 6 says to search the TET case-insensitively;
  - step 9 adds `CREATED_NPC_DROP_TIMES`.

  For A9: the boss is `NUI_ELSION_SUVEILLAN` 2120 (Appendix A). The A9 kickoff was written and committed by the A9 session (`4fb90cb1`).

---

## Phase A9 — AtlasCity line

Follow the *Per-line checklist*. Line-specific: `NUI_ELSION_ATLAS_GATE`,
`IsActiveMonster`(b) (A4), reused `NUI_FIRETEMPLE_GATE`; DungeonData from
line `30516`.

**Run with:** Sonnet 5.5, high effort **if** `region_census.py --line AC`
shows zero open engine gaps; otherwise Opus 5.5, high. Required review (Opus
5.5).

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch main) -- "Starting context", the
"Per-line checklist", Appendix A (AC NUIs) and "Phase A9". Never launch
X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high. If the census shows an engine gap the A1b map doesn't settle,
stop on it and log it for an Opus pass -- do not guess.

Your task (Phase A9): port the AtlasCity line by the Per-line checklist.

Done when: census --line AC is clean, staged with manifests, pack list handed
over (live-kom check after I pack). Record a Done note (DEPLOY OK commit), then commit on main.
```

### Phase A9 — Done

**2026-10-17, Sonnet 5.5 high.** Data only: no C++, no flag change, no build. Census `--line AC`: 60 open rows before, 34 after (the census's own count; its rows below), and
those rows are only the routed classes of checklist step 9 (6 `assets.absent`, 31 `keys.file`, 3 `keys.registry`).
Zero engine gaps, as the kickoff's model rule required.

- **Premises held.** All 10 missing NUIs are in `Enum`; scripts for all of them (+ `SUMMON2`) staged (ljconv, Oct 9); `NewNPCStat` complete.
  Meshes in data181 (`Motion_ELSION_PROVER / DEFENDER / SILENCE / DECEPTOR / NEWDEFENDER / SUVEILLAN`, `Motion_EVENT_ELSION_2ST_BOSS`,
  `Motion_Elysion_ATLASDEVELOPMENT_Gate`). Effects: 416 sets referenced, 0 missing; items 45 referenced, 0 missing. Boss faces:
  `NUI_ELSION_SUVEILLAN` and `NUI_ELSION_NEWDEFENDER` are in `data215 DLG_BossState_02.TET` (UTF-16LE hit). The reused `NUI_FIRETEMPLE_GATE`
  was already live (not re-ported).
- **Staged (`adopt-june data040`, ljconv):** Stage0-2 + BossStage and their 4 line maps (`june2017_re/june_adopt_20261017_074049.tsv`,
  manifest `"2"`). BOM source, appended verbatim as `atlas A9` blocks (script: scratchpad `a9_edit.py`, asserts the first / last line of
  each June slice and no existing duplicate): `data035 DungeonData` June :30516-30788 (Normal / Hard / Expert; June has no quest key on AC;
  Hero rows omitted and named: `HERO_NORMAL :30789-30939`, `HARD :30940-30961`, `EXPERT :30962-31119`, `HELL :38136-38285`),
  `data037 NPCTemplet` 10 rows (ranges in the block header; `SUMMON2` / `PROTO` were already in), `data040 WorldData` WI AtlasCity
  Stage0 / 1 / 2 / BOSSATGE (June :3225-3244). Refreshed, core-list `why` updated; `CORE CHECK PASS: 0 FAIL, 0 WARN`,
  `LEDGER CHECK PASS: 0 FAIL, 0 WARN`.
- **Pack:** `PACK OK data035`, `PACK OK data037`, `PACK OK data040`. Copy (user OK 2026-10-17): `COPY OK` data035 (md5 8d33fef1) / data037 (9b2926fd) / data040 (80c0b288).
- **Offline:** AC rows carry `EXTRA_REQUIRE_DUNGEON_ID = DI_ATLASDEVELOPMENT_NORMAL`, so an AD Normal clear (recorded as 33330) unlocks them;
  a clear of AC records 33340 / 33341 / 33342 with no code. Kills pay nothing (no server `DropTable` rows, backlog Stage 23); nothing invented.
- **How to see it (after the copy):** a level-81+ character that has cleared AtlasDevelopment Normal: M -> Elysion tab -> AC icon enabled
  -> enter Normal; Stage0, 1, 2 and the boss stage load; Suveillan's gauge shows his face. ErrorLog must show no `DungeonData` /
  `NPCTemplet` / `WorldData` load errors.
- **Play-test logs read 2026-10-17 (user played; run after the deploy):** `dungeonID=33340 CLEARED in 227s: 87 kill(s)` (played at Expert, reward key 33342),
  boss `npc=2120` (Suveillan) with `bossGauge=1`, clear broadcast sent, 0 EXCEPTION, UNHANDLED only the two known `E_DISABLE_HB_CHECK_REQ`.
  Kills pay `+0 exp / +0 ED` with the `no reward row` NOTE, as designed (Stage 23). No `DungeonData` / `NPCTemplet` / `WorldData` load errors.
  ErrorLog noise, none from A9: `result : 47` on `Update3DPosition` for `SUVEILLAN_ATTACK_D_FIRE2 / _EXPLOSION.OGG` (files are staged in data182 and
  `Sonic_Arrow.ogg` / `Arrow.ogg` log the same, so it is the existing 3D-position update on a finished channel, not a missing file) and
  `<dungeon name> <- control not found: DLG_UI_LOCAL_MAP` (`X2LocalMapUI.cpp:1324`, the event-disabled star static; NULL-checked, same for Burnt Forest etc.).
- **Open:** kill rewards (Stage 23). Review kickoff: `kickoffs/atlas/A9-review.md`.

### Phase A9 — Review

**2026-10-17, Opus 5.5 high. PASS; no transcription error, no repack, no copy.** Doc-only fixes: checklist steps 4 and 9.

- **Registry rows verbatim** (comment lines dropped, each line rstripped):
  - `DungeonData.lua:17032-17312` vs June :30516-30788: 273 lines each, **0 diff**. The block has no quest key, matching June.
  - `NPCTemplet.lua:19991-20084` vs the 10 ranges in its header: 90 lines each, **0 diff**.
  - `WorldData.lua:3240-3262` vs June :3225-3244: **0 diff**.
  - Every new `dungeonID` / `NPC_ID` / `worldID` occurs exactly once in its file, and BOM and CRLF are intact in all three files.
- **Hero omission.** The four header ranges are June's `DI_ATLASCITY_HERO_NORMAL` / `HARD` / `EXPERT` / `HELL` rows. C3y-5 still owns them and is `ready`.
- **Unlock chain.** All three AC rows carry `EXTRA_REQUIRE_DUNGEON_ID = { DI_ATLASDEVELOPMENT_NORMAL }` (= 33330, DungeonEnum :315) and
  `m_MinLevel = 81`. `IsActiveDungeon` tests `m_vecExtraRequireDungeonID` and then `m_MinLevel - m_iSuitableLevelFixValue` (`X2DungeonManager.cpp:732-756`).
  June's event-key gate on 33330-33352 (2260 / 2261) is the one already routed in the A1b map (inert offline). The play-test cleared 33340.
- **Adopted files.** All 8 data040 files equal the sha1s in `june_adopt_20261017_074049.tsv`. Each starts with `1B 4B 4C 81` and has manifest `"2"`.
  An `adopt-june --dry-run --compare` re-derive reports `COMPARE OK: 8 of 8 ... byte-identical`.
- **Premises.**
  - All 10 NPCTemplet rows (2115-2120, 2122, 2123, 2128, 2169) have an Enum line (:4681-4717), a `1B4B4C81` script in data037 and a NewNPCStat row.
    The mesh each unit script's `MOTION_FILE_NAME` names is present in data181.
  - June's `EVENT_2ST_BOSS` row says `MOTION = Motion_ELSION_EVENT_2ST_BOSS`, which no kom has; its script loads `Motion_EVENT_ELSION_2ST_BOSS.x`,
    which is staged. This is harmless: March only parses the row's `MOTION` (`X2UnitManager.cpp:968`) for the dev sheet (`:1592`), and the loaded
    mesh is the script's `MOTION_FILE_NAME` (`X2GUNPC.cpp:3264`). The row is June-verbatim, so it is not a JUNE_BUGS row either. Checklist step 4 now
    says to check the script's mesh, not the row's.
  - Boss faces: `NUI_ELSION_SUVEILLAN` (offset 4192) and `NUI_ELSION_NEWDEFENDER` (4144) are exact matches in `data215 DLG_BossState_02.TET`.
    Their scripts name `DLG_BossState_02.tga`, which is present.
  - Buffs: none of the 9 AC unit scripts names a `BFI_` / `BTI_` / `BGI_` id, so there is nothing to bind.
  - Effects: all 17 `EffectSet_Suveillan_*` names are constants in data036 `EffectSet.lua`, and the 6 `DamageEffect_Suveillan_*` names they
    reference are in `DamageEffect.lua`. Suveillan's two `.ogg` files are in data182.
- **Hygiene.** `LEDGER CHECK PASS: 0 FAIL, 0 WARN`; `CORE CHECK PASS: 0 FAIL, 0 WARN`.
  - Census `--line AC`: every open row is in a step-9 class: 6 `assets.absent`, 31 `keys.file`, 3 `keys.registry`. The header says 34 for those 40
    rows, the same counting quirk A8's review saw (50 / 55); it was not traced.
  - The 6 absent assets are not in any June kom (`junekom.py extract`: `not found 6`).
  - `keys.file` holds the 4 dungeon reward keys, `LINE_NUM`, 25 `CT_` / `CF_` names and `STOP_TIME_ATK`, which A1b (:1085) lists as a June typo
    June never reads. Step 9 now names it.
  - `--live`: `live {"same": 280}`, so the copied koms match the staged files.


---

## Phase A10 — Tower line

Follow the *Per-line checklist*. Line-specific: boss Code-Maya
(`NUI_ELSION_MAYA`, loading `HQ_LoadingDungeon_Maya_Pit`), world camera file
`Elysion_Tower_stage_Camera.lua` (bindings already live), `WI_TOWER_STAGE3`
points at a missing file — **transcribe verbatim, log it**.
`CHANGE_LINE_TYPE_AFTER_ALL_ACTIVE_NPC_KILLED`,
`LiveKeyCodeNPCNumExceptDying`, `FadeOriginWorldColor_LUA`,
`SetImmuneToExtraDamage` come from A4.

**Blocked on JUNE_BUFF Phase 10** while the census still shows
`SetMonsterBuffGroupID` / `BUFF_GROUP_ID` open. Check the buff branch first;
if Phase 10 hasn't landed, record the block and stop.

**Run with:** Sonnet 5.5, high effort **if** `region_census.py --line TW`
shows zero open engine gaps; otherwise Opus 5.5, high. Required review (Opus
5.5).

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch main) -- "Starting context", the
"Per-line checklist", the buff-dependency rule, Appendix A (TW NUIs) and
"Phase A10". Check JUNE_BUFF_PORT_PHASES.md for Phase 10's status first.
Never launch X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high. If JUNE_BUFF Phase 10 (SetMonsterBuffGroupID / BUFF_GROUP_ID)
hasn't landed, record the block and stop.

Your task (Phase A10): port the Tower line by the Per-line checklist,
transcribing June's WI_TOWER_STAGE3 bug verbatim with a log line.

Done when: census --line TW is clean, staged with manifests, pack list handed
over. Record a Done note (DEPLOY OK commit), then commit on
main.
```

### Phase A10 — Done

**2026-10-17, Sonnet 5.5 high.** Data only: no C++, no flag change, no build. Census `--line TW`: 60 open rows before, 41 after, and those are only the
routed classes of checklist step 9 (6 `assets.absent`, 1 `bind.nosource` + 1 `koms.unstaged` = `Elysion_Tower_Stage3.lua`, which June ships nowhere,
30 `keys.file`, 3 `keys.registry`). Zero engine gaps, as the kickoff's model rule required; JUNE_BUFF Phase 10 was already done (no buff `bind.*` row).

- **Premises held.** All Tower NUIs are in `Enum`; scripts staged (ljconv); `Motion_*` meshes in data181 (SUMMON3 has no `MOTION`, as in June);
  `Dungeon_Elysion_Tower_Normal / Hard / Expert.lua` in data035, `HQ_LoadingDungeon_Maya_Pit_01 / _02.tga` in data183. Effects 416 sets, 0 missing; items 0 missing.
  Boss faces: `NUI_ELSION_MAYA` and `NUI_ELSION_HIGHTTEM` name `BOSS_GAGE_FACE_TEX_PIECE` and are in `data215 DLG_BossState_02.TET` (UTF-16LE hit);
  TEMPLAR / LIBYA / PUTER / DISION name no face piece.
- **Staged (`adopt-june data040`, ljconv, manifest `"2"`):** Stage0-2, their 3 line maps and `Elysion_Tower_stage_Camera.lua` (7 new;
  `june2017_re/june_adopt_20261017_075106.tsv`). `BossStage` + its line map were already in (C3y-1b).
- **BOM source, appended verbatim as `atlas A10` blocks** (scratchpad `a10_edit.py`, asserts the first / last line of each June slice):
  `data035 DungeonData` June :31120-31362 (Normal / Hard / Expert; June has no quest key on Tower; Hero rows omitted and named in the header:
  `HERO_NORMAL :31363-31513`, `HARD :31514-31535`, `EXPERT :31536-31724`, `HELL :38286-38435`, still C3y-5's), `data037 NPCTemplet` 9 rows (TEMPLAR, LIBYA, PUTER,
  HIGHTTEM, DISION, EVENT_3ST_BOSS, EVENT_YUNO, TOWER_GATE, SUMMON3; ranges in the block header; MAYA and the AC-shared PROVER / SILENCE / DECEPTOR /
  SUMMUN_PROVER / PROTO rows were already here), `data040 WorldData` WI_TOWER_STAGE0-3 (June :3245-3264; `WI_TOWER_BOSSATGE` was in).
  Refreshed, core-list `why` updated; `CORE CHECK PASS: 0 FAIL, 0 WARN`, `LEDGER CHECK PASS: 0 FAIL, 0 WARN`.
- **June bug kept verbatim (JUNE_BUGS #209):** `WI_TOWER_STAGE3` -> `Elysion_Tower_Stage3.lua`, a file June ships in no kom. Latent: none of the three
  `Dungeon_Elysion_Tower_*` scripts names `WI_TOWER_STAGE3`. The row is in `WorldData` with a comment; no file invented.
- **Pack:** `PACK OK data035`, `PACK OK data037`, `PACK OK data040`. Copy (user OK 2026-10-17): `COPY OK` data035 (md5 a8703d01) / data037 (e97be63a) / data040 (8376d0ca).
- **Offline:** Tower's rows carry `EXTRA_REQUIRE_DUNGEON_ID = DI_ATLASCITY_NORMAL`, so an AC Normal clear (33340) unlocks them; a Tower clear records as
  33350 / 33351 / 33352 (`DungeonEnum.h:520-522`) with no code. **Kills pay nothing** (no server `DropTable` rows; backlog Stage 23); nothing invented.
- **How to see it (after the copy):** a level-80+ character that has cleared AtlasCity Normal: M -> Elysion tab -> Tower icon enabled -> enter
  Normal; Stage0, 1, 2 and the boss stage load; Maya's gauge shows her face; the loading screen is Maya Pit. ErrorLog must show no `DungeonData` /
  `NPCTemplet` / `WorldData` load errors.
- **Play-test PASSED 2026-10-17 (user); logs read (run after the copy):** two runs, both from the local map: key 33352 (Expert) with `npc=2153` (Highttem, stage 1)
  `bossGauge=1` and `dungeon=33352 stage=3 npc=2155` (Maya) `bossGauge=1`, then `dungeonID=33350 CLEARED in 269s: 87 kill(s)` (+0 exp / +0 ED, `no reward row`
  NOTE per kill, as designed; Stage 23). 0 EXCEPTION; UNHANDLED only the two known `E_DISABLE_HB_CHECK_REQ`. No `DungeonData` / `NPCTemplet` / `WorldData` load
  errors. ErrorLog noise on the line, all routed: `OpenTexture ADE_PS02.TGA` (census `assets.absent`, June fails the same way). Not from A10: `result : 47` on
  `Sonic_Arrow` / `Arrow` / `HitFire` / `Fire2` `.ogg`, the `<control> not found: DLG_UI_LOCAL_MAP` statics (as A9), and textures of other systems.
- **Open:** kill rewards (Stage 23). Review kickoff: `kickoffs/atlas/A10-review.md`.

### Phase A10 — Review

**2026-10-17, Opus 5.5 high. PASS; no transcription error, no repack, no copy, no doc fix.**

- **Registry rows verbatim** (comment lines dropped, each line rstripped):
  - `DungeonData.lua:17313-` vs June :31120-31362: 243 lines each, **0 diff**. June's slice is bounded by `})` at :31119 and the Hero Normal row at :31363.
    No quest key on Tower, in June or here.
  - `NPCTemplet.lua:20085-` vs the 9 header ranges: 80 lines each, **0 diff**. Each range sits between a `})` and the next `AddNPCTemplet({`.
  - `WorldData.lua:3263-` vs June :3245-3264: 20 lines, **0 diff**.
  - Every new `dungeonID` / `NPC_ID` / `worldID` occurs exactly once. BOM present, CRLF only (0 bare LF) in all three files.
- **Coverage.** Every NUI the three June `Dungeon_Elysion_Tower_*` scripts name has exactly one March NPCTemplet row (15 NUIs incl. `CHEST` and the
  AC-shared rows). Every world they name has one WorldData row: STAGE0-2 + `BOSSATGE`.
- **Hero omission.** June's `DI_TOWER_*` rows are at :31132 / :31296 / :31337 (ported) and Hero :31378 / :31527 / :31548 / :38299. Their row spans are
  :31363-31513, :31514-31535, :31536-31724 and :38286-38435, the same as the header. C3y-5 still owns them.
- **Unlock chain.** All three rows carry `EXTRA_REQUIRE_DUNGEON_ID = { DI_ATLASCITY_NORMAL }` (= 33340, `DungeonEnum.h:517`) and `m_MinLevel = 82`, with no
  `m_SuitableLevelFixValue` and no quest key. `IsActiveDungeon` (`X2DungeonManager.cpp:735-756`) therefore admits level 80+ after an AC Normal clear. The play-test
  entered from the local map.
- **Adopted files.** All 7 data040 files start with `1B 4B 4C 81`, have manifest `"2"` and match the sha1s in `june_adopt_20261017_075106.tsv`.
  `adopt-june --dry-run --compare` (nothing written) re-derived them from June: `COMPARE OK: 7 of 7 ... byte-identical`.
- **June bug #209.** No Tower dungeon script names `WI_TOWER_STAGE3` or `Stage3`. The only June hits are `Enum`, `WorldData` and Stage2's world, which
  loads `ELYSION_TOWER_STAGE3_X0*.X` meshes. Those are assets, not the world file, and the census finds them. The row is correctly latent.
  `WorldData`'s `Elysion_Tower_Stage0.lua` vs the staged `Elysion_Tower_stage0.lua` is June's own case. The play-test loaded those stages, so the lookup is
  case-blind.
- **Premises (spot-check).** TEMPLAR / PUTER / SUMMON3 / MAYA / HIGHTTEM are in `Enum` and `NewNPCStat`. TEMPLAR / PUTER / LIBYA / HIGHTTEM have their scripts
  in data037, and their `MOTION_FILE_NAME` meshes are in data181. `NUI_ELSION_MAYA` / `NUI_ELSION_HIGHTTEM` are UTF-16LE hits in `data215 DLG_BossState_02.TET`.
  The census shows effects 416 / 0 missing, items 0 missing and no buff `bind.*` row.
- **Hygiene.** `LEDGER CHECK PASS: 0 FAIL, 0 WARN`; `CORE CHECK PASS: 0 FAIL, 0 WARN`.
  - Census `--line TW --live`: 41 rows, all routed by step 9: 6 `assets.absent`, 1 `bind.nosource` + 1 `koms.unstaged` (both `Elysion_Tower_Stage3.lua`),
    30 `keys.file`, 3 `keys.registry`. The header's "33" is the counting quirk the A8 / A9 reviews saw.
  - `live {"same": 287}`: the copied koms match the staged files.

---

## Phase A11 — Asset closure + consolidated pack list (Atlas half)

**Steps**
1. Run `june2017_re/assets.py` closure over everything A5–A10 staged:
   meshes → XET → textures, **MSZIP-compressed `.Y`/`.X` mesh-internal
   textures** (`xof 0303bzip`), particle textures, sounds.
2. TET-key census: every `SetTex`/`SetNormalTex`/`IconAni`/piece/texture
   key the staged Lua uses resolves in the winning-mount TET.
3. Size check vs June's kom index; list truncated files.
4. Mount order: for every staged name, which archive wins.
5. One consolidated pack list (archive → files), new archives flagged, plus
   the server-side files (`StatTable.lua`, `NpcExpTable.lua`,
   `ServerResource/US/MapData.lua`) named for `data036.kom`.

**Done when:** census `--region atlas` shows 0 missing assets / keys; pack
list in the doc and handed over.

**Run with:** Sonnet 5.5, high effort. Required review (Opus 5.5) of the
staged set, manifests and census output.

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch main) -- "Starting context", the ledger's
"Assets" section, "Lanox lessons -> gates" and "Phase A11". Also read
LANOX_REMAINING_PHASES.md Phase 0 (the TET census and mesh-internal texture
method). Never launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase A11): close the Atlas half's asset set (mesh-internal
textures included), census every TET key, size-check against June's kom index,
resolve mount order, and produce one consolidated pack list.

Done when: census --region atlas shows no missing assets or keys and the pack
list is in the doc. Record a Done note, then commit on
main.
```

*(2026-10-17, atlas A6 follow-up, user: the region-wide asset closure (census-named + mesh/xet-embedded textures, both halves) is staged and packed; see "Phase A6 — Done" and `june2017_re/atlas_a6_region_closure.tsv`. A11 re-runs that closure for files later phases add, then does the pack list / live check.)*

### Phase A11 — Done

**2026-10-17, Sonnet 5.5 high. Data only: no C++, no flag, no build, no deploy (A12 owns those).** Script: `june2017_re/atlas_a11_closure.py`
(read-only; writes `atlas_a11_closure.tsv`, `atlas_a11_tet.tsv`, `atlas_a11_sizes.tsv`, `atlas_a11_packlist.tsv`). Census run: `region_census.py
--region atlas --limit 0 --no-probe --live`.

- **Gate:** `--region atlas` shows 0 `assets.adopt` (the 49 June-only names in the region-wide stats are all Elysion Lua, E-phase), 2 `assets.absent`
  (June ships none), `live {"same": 287}`, no `live.size` row. The only open rows are the routed classes (`keys.file` 62, `keys.registry` 1,
  `bind.nosource` 1, `koms.unstaged` 1: Elysion_Tower_Stage3.lua, JUNE_BUGS #209).
- **1. Closure** (both halves, as A6): 3,880 names (2,201 an Atlas line quotes) = census-named files + the textures / sub-meshes that 1,280
  meshes and XETs embed (staged copy, else June's strict decode). Result: 3,794 in March (staged = live), 6 extension mismatches (the mesh names
  `.tga`, March has the same stem as `.dds`: `elysion_cross_ap_whalefx`, `elysion_villeage_column`, `elysion_villeage_wing`, `pet_fox_ghostattacka_spark`,
  `veritable_pandemonium_t02`; `elysion_damage_object_2` the other way; June ships the same files, so June fails the same way) and 80 names June
  ships no file for (6 of them are meshes, so their textures cannot be read: `elsion_deceptor_ma_a_{attack_dummy,wing_a,wing_b}.x`,
  `elysion_s_maya_weapon_circle.y`, `motion_m_2stage_gate.x`, `motion_npc_elsion_hugo.xet`). Nothing is missing that June has. Rows: `atlas_a11_closure.tsv`.
  Particle-templet textures stay with june-effect (as in A6).
- **2. TET-key census:** 133 distinct (texture, key) pairs from `SetTex` / `SetNormalTex` / `SetOverTex` / `SetDownTex` / `SetDisableTex` / `IconAni` calls and the
  `m_TextureName`/`m_PieceName`, `BATTLE_FIELD_TEXTURE_NAME`/`_KEY`, `BOSS_GAGE_FACE_TEX`/`_PIECE`, `NPCTextureName`/`NPCTextureKey` pairs of every region file
  and registry row, looked up (upper-cased) in the winning-mount TET. First run: 79 OK, 52 `NOTET`, 2 `KEY-MISSING`.
  - **Real gap found: 28 `.TET` atlas companions were missing in March** although the `.tga`/`.dds` they index were there (the census names the picture,
    never the TET). They gated: `FIELD_NAME_ELISION_1/2/3` + `FIELD_NAME_RANOX_4` (`DLG_UI_Common_Texture_Fieldname_7/8/9`, A6's BattleFieldData), Atlas Station
    `NUI_YUNO` + 4 more faces (`DLG_Common_New_Texture76`, DLG_Map_House), `DLG_UI_Dungeon_Image12` (Soless / Memory DungeonData), and the Elysion map /
    gauge dialogs (`DLG_UI_FIELD_MAP_ELISION_01..03_*` x 15, `DLG_UI_Town_ELISION_Map01..05`, `DLG_UI_Image_02/04/05`). A second sweep over every closure
    texture whose June stem has a `.tet` that March lacks found the same 28, nothing more.
  - **Staged and packed** (June's exact bytes, 28 new names, 0 replaced, manifest `"0"`): `kom.py stage-june` -> data057 25, data151 2 (`DLG_Common_New_Texture76`,
    `DLG_UI_Dungeon_Image12`), data215 1 (`DLG_UI_Image_05`). `PACK OK` x3. **Copied (user OK):** `COPY OK` data057 d50ad827 (913 entries), data151 0f6dcdfb (11), data215 8f87f3c1 (17).
    These fix the field-name banners of Fields 01-03, the Ranox F04 banner, the Atlas Station Yuno face and the Soless / Memory dungeon icons; the Elysion
    dialogs they also cover are adopted by E4.
  - **After the copy:** 133 pairs = 129 OK + 2 `KEY-MISSING` + 2 `NOTET`; 114 of them Atlas-quoted = 112 OK + the 2 `NOTET`. The 4 left are June's own defects (JUNE_BUGS #214, #215):
    `DLG_Localmap_Title_01_A.tga` (`SOLESS`, `TITLE_MEMORY`) exists in no June kom; `NUI_ELYSION_SPHERE` / `NUI_ELYSION_S_SPHERE` are in neither June's nor March's
    (byte-identical, md5 11c00b39) `DLG_BossState_02.TET`. Kept verbatim.
- **3. Size check:** 3,770 staged names also in June's index. All 3,743 June-origin files equal June's index size (stage-june decodes strictly, so no algo-2 truncation
  reaches the tree). 27 differ, all **March's own older copies** that were in the live koms before this port (none in A6's closure, none touched by A11):
  `data013` camilla / ariel / ruriel / mu / adamas faces and `motion_npc_all_*` meshes, `village_postbox.x`, `data023 rest_adboard.dds`, `data024
  ruben_build_typo.dds`, `data026 elder_village_sign_bank.dds`. They are June-vs-March collisions for the June asset port (P-phases), not for this port; list in `atlas_a11_sizes.tsv`.
- **4. Mount order:** no closure name lives in two archives (staged or live), so no winner is contested. The TET winners are the single copy named in `atlas_a11_tet.tsv`.
  Region files sit in 80 archives (data180 1,723, data002 188, data181 359, data182 422, data197 216, data013 110, data104 100, data096 89, ... `atlas_a11_closure.tsv` column `win`);
  24 loose music / movie files live in `data\music`.
- **5. Consolidated pack list:** nothing is owed. `atlas_a11_packlist.tsv` (staged region files absent from or different in the live kom) is empty after the three copies above.
  - Server-side Lua for `data036.kom` (user packs by hand): `StatTable.lua` and `NpcExpTable.lua` are in the live data036 (sizes 436,432 / 2,445 = the staged BOM-free bytecode; latest run logs
    `[CAP99] StatTable rows 81-99 present for 50 of 50 classes`). The client `MapData.lua` (32,324 B) is live too; `KncWX2Server/ServerResource/US/MapData.lua` (35,094 B, the server's own
    mirror) is not read by the offline client and is not a pack item.
  - New archives in the whole Atlas run: data179, data178, data216, datai027 (A6 closure, all copied); A11 adds none.
- **Hygiene:** no code changed, so `flagcheck` / `build.py` do not apply. `luasrc.py ledger --check` is untouched (no Lua staged).
- **Routed, not open:** 80 absent names and 6 extension mismatches above (June's own); the 27 collisions (June asset port).

### Phase A11 — Review

**2026-10-17, Opus 5.5 high. PASS; no repack, no copy. One census-method gap fixed in the script (no asset was missing because of it).**

- **The 28 TETs.** Each staged file is byte-equal to June's strict decode (`junekom.decode`), has manifest `"0"` and sits in exactly one archive. Each is
  listed by `elstools inspect` in the live kom at the same size. Live md5 = packed md5 = the `COPY OK` values (data057 d50ad827, data151 0f6dcdfb,
  data215 8f87f3c1). Hand spot-checks read the UTF-16LE key in each TET:
  - `BattleFieldData.lua` (data036, BOM source): `FIELD_NAME_ELISION_1` is in `Fieldname_8.TET`.
  - `DLG_Map_House.lua` (data034): `HI_YUNO` is `NPCTextureKey = "NUI_YUNO"` + `NPCTextureName = "DLG_Common_New_Texture76.tga"`. The key is in `Texture76.TET`.
  - `DungeonData` rows: `SOLESS` / `MEMORY` are in `Dungeon_Image12.TET`.
  - `elsion_dekal.lua` (June bytecode): its constant is `NUI_Science_Dekal`, and the TET stores `NUI_SCIENCE_DEKAL`. Upper-casing is faithful:
    `CKTDXDeviceTexture::GetTexUV` (`KTDXDeviceTexture.cpp:433`) does a raw `find`, but the GUI `SetTex` path upper-cases first (`KTDGUIControl.cpp:733`,
    `KTDGUIStatic.cpp:1074`), and the boss face goes `CX2BossGageUI::SetBossGageTexture` -> `SetTex_LUA`.
- **Census method.** Checked over the same region texts (`RC.all_texts`):
  - No `.tga`/`.dds` literal followed by a key on the same line escapes the call regex.
  - Every `SetTex*` / `IconAni` call carries a literal texture. The non-literal ones are `SetUseTex(2, false)` and `ChangeTexForce_LUA("PC_n.dds")`, a whole
    particle file with no key; `pc_1..5.dds` are in the closure, live in data049.
  - There is no `NPCAniTex` / `MOTION_*_TEX_XET` use and no runtime-built asset name (`..` concatenation or `string.format`) in the region.
  - **Gap:** one more Name/Key field pair exists. `DLG_Map_Local_Elysion.lua` has `TEX_NAME = "DLG_New_Dungeon_Image5.dds"` / `PIECE_NAME =
    "Local_Image_Besma_Village"`. It resolves: data065 TET has `LOCAL_IMAGE_BESMA_VILLAGE`. `('TEX_NAME', 'PIECE_NAME')` is added to `tet_pairs`.
  - Re-run: `atlas_a11_tet.tsv` has 134 pairs = 130 OK + the same 2 `KEY-MISSING` + 2 `NOTET`; `PACKLIST ... 0`; `MOUNT ... 0`.
  - `atlas_a11_closure.tsv` re-ran identical except the `frm` column, which is set-order dependent, so it was kept as committed.
- **Closure end to end.** Each walk takes the June source's `ASSET_RE` literals, expands every mesh / XET (bzip `.x`/`.y` included) and looks each name up in the live koms:
  - `elsion_maya` (texture / mesh preload tables): 80 roots, 97 names, 16 of 17 meshes bzip, all live.
  - `elsion_highttem`: 48 names, all live.
  - `Elysion_Tower_stage0` (`WI_TOWER_STAGE0`): 73 names. All are live except `music078_stage.ogg`, which is the loose `data\music` copy.
- **June defects.** Both checks are against June's own kom index, so neither is a decode problem.
  - #214: no entry in any of June's 258 koms has `localmap_title_01_a` in its name. June has only `DLG_Localmap_Title_01/02` (data151) and `_INT_01` (datai021).
  - #215: June's `DLG_BossState_02.TET` decodes to its index size (6,256 B), md5 11c00b39 = March's. Of its 75 keys, none is a `SPHERE` key.
- **27 size differences.** All 27 are dated 2026-09-22 (the March extract) in `kom_contents`. None of them is in `kom_backup_stage_june`, nor in any other
  `kom_backup_*` / `kom_contents_backup_*`. They are March originals that were never replaced.
- **Server Lua.** The live data036 holds `StatTable.lua` 436,432 B, `NpcExpTable.lua` 2,445 B and `ExpTable.lua` 5,925 B, all `1B 4B 4C 81`, plus the client
  `MapData.lua` 32,324 B (BOM). Each has manifest `"2"` and equals the staged copy; data036 live md5 = packed md5. The 08:33 run logs
  `STAT 'StatTable.lua' loaded: 50 class(es), 4950 row(s)` and `[CAP99] StatTable rows 81-99 present for 50 of 50 classes`.
- **Hygiene.** Census `--region atlas --no-probe --live` shows only the routed classes: `assets.absent` 2, `bind.nosource` 1, `keys.file` 62, `keys.registry` 1,
  `koms.unstaged` 1. It also shows `live {"same": 287}`. No code changed, so flagcheck / build do not apply. `kickoffs/atlas/A12.md` checked: it is current.
- **Noted, not owned here:**
  - March has `DLG_Localmap_Title_02.tga` but not June's `.TET`. Only `DLG_Map_Local_Event/Special_Icon_NEW` use it, outside the region.
  - The 08:33 run warns `STAT no rows for class=122 / 32` (pre-existing).

---

## Phase A12 — Build, deploy, hand-off

**Steps:** full four-project build (PCH touched), literal check for every
Atlas flag, deploy as `X2_offline.exe` (size/mtime check, record the
branch), confirm every staged file is in the pack list, hand off. Write the
**Atlas reachability checklist** (below) into the hand-off. Ask before any
launch.

**Run with:** Sonnet 5.5, medium effort. No review.

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch main) -- "Starting context", "Phase A12"
and "Verification". Never launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: medium.

Your task (Phase A12): build all four projects as US_SERVICE with tools/build.py --deploy (PCH touched), prove every Atlas-port flag's literal is in x2.exe,
deploy as X2_offline.exe (check size/mtime; record the DEPLOY OK
commit), and hand me the consolidated pack list and the
Atlas reachability checklist.

Done when: deployed and handed off. Record a Done note,
then commit on main.
```

### Phase A12 — Done

**2026-10-17, Sonnet 5.5 medium. No code change, no new flag.** Build and deploy of everything the A1-A11 commits and E1a/E1b hold.

- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,670,016 B, dated 2026-10-17 08:52:20, md5 caae215304cb6b04358752e595c6fced, equal to X2/US_SERVICE/x2.exe (2026-10-17 08:52:15); built at main 4c9cf74e + 1 uncommitted code file(s).`
  `build.py`: X2ServerProtocol / KTDXLIB ok, X2Lib ok in 525 s (211 warning lines), X2 ok in 96 s; PCH touched first (`X2Lib/stdafx.cpp`, then `X2/Common/dxstdafx.cpp`).
  - **What is in the exe beyond HEAD:** the one uncommitted file is a peer's `X2Lib/X2TFieldNpc.cpp` (E1b review: `InitQuestMark`
    NpcTalkParticle, `GetHouseType`; flag `ELYSION_VILLAGE`), edited 08:46 and compiled 08:49, so it is linked in. The build header also listed
    `KTDXLIB/Always.h` + `X2Lib/X2StateMenu.cpp` (`SERV_IRUHADEV_HIDE_ELESIS_EVENT_BUTTON`, not an Atlas flag); both files were reverted on disk at
    08:42:17, before `X2StateMenu.obj` (08:48) was built, and the exe has no `ElesisEventButton` literal, so that flag is out.
- **Flag literals in `X2/US_SERVICE/x2.exe`** (UTF-16LE, ASCII where the source is a `KLOG` narrow string; each sits only inside its flag's `#ifdef`):

| Flag | Literal | Hits |
|---|---|---|
| `ELYSION_PORT` | `Village_1501` (A5 `InitWarpList`) | 3 |
| `ELYSION_SCRIPT_A` | `CAN_NOT_PASS_SPECIFIED_BONE first load`; `SetChangeUVPoint first use` (KTDXLIB, ASCII) | 1; 1 |
| `ELYSION_DEBUG` | `[ELYDBG] indicator first skip` | 1 |
| `JUNE_LOCAL_MAP_ELYSION` | `[ELYDBG] local map Elysion`; `[ELYDBG] VillageParsing: no Village` | 1; 1 |
| `ELYSION_VILLAGE` | `HouseParsing June keys`; `VillageNpcRelation: loaded` | 1; 1 |
| `LEVEL_CAP_99` | `[CAP99] migrated unit`; `[CAP99] EXP migration done` | 1; 1 |

  - **`OFFLINE_BF_JOIN_START_POS` has no literal** (its block is two assignments, no string). Not provable from the exe; its proof is behavioural
    (Atlas Station -> Field 01 lands at the start position, not 'Field 01 key 333 on a 163-line map'). `EXP_GAUGE_MAX_LEVEL_FIX` likewise has none (arithmetic only).
- **Staged files live:** `python -I june2017_re/atlas_a11_closure.py` prints `PACKLIST archives owing a pack/copy: 0, files 0` and `region_census.py --region atlas --live`
  `live {"same": 287}`. Nothing is owed to pack.
- **Hand-off:** the Atlas reachability checklist ("Verification", 11 items) is in the reply, with `reyaa` (unit 12, Lire Grand Archer, lv 85, AD/AC/TW prerequisites cleared) as the character.
  Kickoff `kickoffs/atlas/A13.md` written (Opus 5.5 high, root-cause + close-out).

---

## Phase A13 — Play-test sweep + close-out (Atlas half)

**Steps:** after the user packs and plays the reachability checklist,
root-cause every failure (add short-lived `[ELYDBG]` diagnostics rather than
reasoning further; scope log checks to the phase's own lines). Then:
`MODS.md` entry for every new flag; an "Atlas/Elysion" section in
`LANOX_TROUBLESHOOTING.md`'s symptom table for anything new; fill in
*Deferred to the quest plan* with whatever was learned.

**Run with:** Opus 5.5, high effort. No review.

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch main) in full, plus LANOX_TROUBLESHOOTING.md.
Never launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase A13): I have packed and played the Atlas reachability
checklist; I'll paste results and logs. Root-cause each failure (diagnose
with short-lived [ELYDBG] lines, not guesses), fix, rebuild, hand off again.
Then close out: MODS.md entries for every flag, troubleshooting rows, and the
"Deferred to the quest plan" section.

Done when: every checklist item passes or is recorded with a cause and
owner. Record a Done note, then commit on
main.
```

### Phase A13 — Done

**2026-10-17, Opus 5.5 high.** No fix needed: no checklist item failed on a port defect. One short-lived diagnostic was added (`SERV_IRUHADEV_ELYSION_DEBUG`):
`CX2World::SetCameraDistance_ELYDBG` is bound as Lua `SetCameraDistance` (`X2World.h/.cpp`, `X2Main.cpp`; the plain binding stays in `#else`), because
nothing had ever logged that Lanox `SECRET_SCENE_DUMMY_SUMMON_TYPE3_STATE_START` (Secret Hell 41271, stage 2) reaches C++.

**Checklist (evidence: the A12 run 08:55-09:00 on exe caae2153, plus the earlier phases' play-tests):**

| # | Item | Verdict |
|---|---|---|
| 1 | CAP99 migration | PASS (A2: schema v16, 24 of 27 units exact, reyaa 80 -> 85). The `migrated unit` line exists only in the migrating launch's logs |
| 2 | `atlas` warp renders | PASS (A7 re-test; A12 run: Cobo `20000 -> 1008` 08:56:20) |
| 3 | Lanox <-> Atlas Station | PASS. Forward: A6. Return: A12 run, 1008 pos 401 -> 40032 `startPos=109` (08:57:33). The checklist's "Lanox Field 03" is Enum `RANOX_FIELD_04` 40032 (Lanox names are offset by one) |
| 4 | Fields 01 / 02 | Spawn, HP and death PASS (A12 run: 40035 29 placed, 40036 entered, lv 80 / 84 kills). Landing on key 333 is correct (it is an `AddStartPos` key; the map has 163 lines). **Kill EXP / ED / drops = 0: owner backlog Stage 23** (no server DropTable rows past 40025; `GetNpcReward` returns before `NpcExpTable`). **F02 dead exit: owner atlas E2** (row commented on purpose) |
| 5 | Cobo / warp NPC | Cobo PASS (A7, A12 run). Warp-NPC listing: no recorded evidence -> A13 play-test check 2 |
| 6 | World map / M-key | Pins log as designed (20009 / 40037 hidden until E2 / E3). Visual: A13 play-test check 3 |
| 7 | Local map | PASS (A7; AD / AC / TW entered from it in A8-A10). The Final Hall gate is not a June gate (A7). Page scroll: owner atlas E4 |
| 8 | AD -> AC -> TW | Stages and bosses PASS (A8; AC 33340 and TW 33350 cleared with `bossGauge=1`, A9 / A10). Unlock chain checked row-by-row (A9 / A10 reviews). **Clear EXP / drops = 0: owner Stage 23.** Hero rows: owner june-dungeons C3y-5 |
| 9 | 80s levels / stats | StatTable 81-99 present for 50 of 50 classes, NpcExpTable 81-99 present (A12 run); reyaa 85 on June's curve (A2). Levelling by region kills waits on Stage 23 |
| 10 | Lanox `SECRET_SCENE_DUMMY` | Bound since june-effect C3z-4b; not yet proven. **A13 play-test check 1** (the `[ELYDBG] SetCameraDistance call` line) |
| 11 | UNHANDLED / EXCEPTION / KEM_ERROR | PASS (A12 run: 0 EXCEPTION; UNHANDLED only `E_DISABLE_HB_CHECK_REQ` x2 + `EGS_SUPPORT_MATERIAL_EVENT_TIME_REQ`, both outside the region) |

**Close-out:** MODS.md has Applied-mods rows for the six Atlas flags that lacked one, plus sections A4, A8-A11 and E1a-E1b (in progress, E15 closes it).
`LANOX_TROUBLESHOOTING.md` has a new "Atlas/Elysion" table (7 classes). *Deferred to the quest plan* has the A13 bullets.

**Hygiene:** `flagcheck SERV_IRUHADEV_ELYSION_DEBUG --worktree --files X2Lib/X2World.h X2Lib/X2World.cpp X2Lib/X2Main.cpp`: `files: 2 PASS, 1 PASS~
(blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched
inserted line(s), 1 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The PASS~ is one blank separator line before the
new `X2World.cpp` block; the bare `#else` is the house style.

**Deploy: not yet.** `build.py --deploy` at 09:09 stopped with `BUILD FAILED (X2Lib). Nothing was deployed.`. Every error is in a peer's
uncommitted atlas E1c/E1d work (`X2TFieldGame.cpp`: `m_vecJuneRelationHouseTemplet`, `m_fCameraModifyDistanceByPortal` not declared yet),
none in A13's three files. The diagnostic ships with the next successful build of `main`, either the E1d session's own deploy or an A13
rebuild. Before the play-test, check that the deployed exe has the UTF-16 literal `[ELYDBG] SetCameraDistance call`.

**Play-test PASSED 2026-10-17** (run 10:16-10:21, reyaa, exe md5 d095fe3a = the E1c/E1d deploy of `main` f9c2b2a1 + peers, so it carries the A13 wrapper; 0 EXCEPTION, UNHANDLED only `E_DISABLE_HB_CHECK_REQ` x2 + `EGS_RENEWAL_LETTER_REQ`, outside the region):
- **Check 1 (item 10):** `[10:20:38.364] [ELYDBG] SetCameraDistance call: base 0.0 -> 1000.0, steps 0.0 0.0 0.0`, 20 ms after `indicator first skip: npc 2246` in Secret Hell 41271 (`dungeonID=41271`). The Lanox call reaches C++. Item 10 PASS.
- **Check 2 (item 5):** `WARP pass 215660 -> map 1008` at 10:18:07, from Lanox village 20008 via the inventory warp list (`X2UIInventory`). Item 5 PASS.
- **Check 3 (item 6):** visual, on the user's word. This run logged no world-map line; the A12 run logged the hidden 20009 / 40037 pins as designed.
- Also in the run: Atlas 403 -> Field 01 `startPos=333`, Field 01 -> Field 02 `startPos=1`.

**Atlas half closed:** every checklist item passes or has an owner (Stage 23 rewards, atlas E2 F02 exit, E4 page scroll, C3y-5 Hero rows). The wrapper lives only under `ELYSION_DEBUG`; undefining that flag removes it along with every other `[ELYDBG]` line.

---

## Phase E0 — Preconditions, census, `ELYSION_SOLESS` decompile

**Steps**
1. Confirm Atlas A0–A4 (incl. A2, level cap 99) and A7 are Done (their Done
   notes above); E2+ also need A5/A6, and E8–E11 need A8
   (it writes the *Per-line checklist*). Read JUNE_BUFF's Phase 10
   status.
2. Decompile `ELYSION_SOLESS.lua` (see *Starting context*); `--check` must
   pass. Confirm the string-only names (`PlayAdditionalWorldAllSound`,
   `SetShowAdditionalWorld`, `SetNumberInGageStatic_LUA`,
   `SetNumberInNumberStatic_MoreParam_LUA`, `SetUseExternalCondition_LUA`,
   `Add/SetStateCoolTime_LUA`, `GetBuffAccumulationCountNow`) are real calls;
   add any new name to A1b's `region_engine.tsv` with an owner (Opus pass if
   it needs RE).
3. Point `region_census.py` at the decompiled copy and record the
   `--region elysion` baseline per line.

**Run with:** Sonnet 5.5, medium effort. No review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch main) --
"Starting context", the ledger's "Elysion half" sections and "Phase E0". Never
launch X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline; tell subagents the same.

Effort: medium.

Your task (Phase E0): confirm the Atlas phases E0 depends on are Done; decompile
14JUN17/data/data037_kom/ELYSION_SOLESS.lua with elstools decompile-batch +
june2017_re/ljd_jun2017.py --check into june2017_re/june_lua/; confirm its
string-only engine names and register any new one in region_engine.tsv; record
the region_census.py --region elysion baseline.

Done when: decompile passes --check; baseline recorded. Record a Done note, then commit on main.
```

### Phase E0 — Done

*(2026-10-17, Sonnet 5.5 medium. Tooling and read-only: no game code, no flag, no flagcheck, no build or deploy, no log tag.)*

- **Preconditions hold.** A0-A4 (incl. A2, cap 99) and A7 are done with their reviews; A5 is done (its play-test needed A7, which landed);
  A6 is still queued, and E2 needs it. JUNE_BUFF Phase 10 is done (STATUS rows A10 / E11 say so); E10 / E11 can rely on it.
- **The decompile gate is NOT met, and cannot be with this tool.** `luasrc.py decompile` (ljd via `ljd_fixed.py`) on June's
  `ELYSION_SOLESS.lua` (105,101 B) ran 29 min with a 3300 s timeout and the `ljd` child reached **19 GB** resident (of 32 GB, 2.5 GB free);
  I killed it by PID. This is the same failure A1b and the Lua-source plan recorded as "ljd timed out after 900 s", so a longer
  timeout does not help: the time is a memory blow-up. No `june2017_re/june_lua/ELYSION_SOLESS.src.lua` exists and none is claimed.
  The census therefore keeps reading the file as exact bytecode constants (A0 review), and `region_census.py` is unchanged.
- **What E0 needed from the decompile, confirmed from the bytecode instead** (`ljconv` to 0x81, `tools/ljbc.py list`, 8,886 lines):
  each of the 8 string-only names is a real method call: a `TGETS` of the name right after `MOV A+1, B` and a `CALL` on that slot, in
  every occurrence. `PlayAdditionalWorldAllSound` 1, `SetShowAdditionalWorld` 1, `SetNumberInGageStatic_LUA` 2 (first arg
  `g_pStatic_ElysionSolessAwakenGage`), `SetNumberInNumberStatic_MoreParam_LUA` 8 (`g_pStatic_AwakenRemainTime`),
  `SetUseExternalCondition_LUA` 1, `AddStateCoolTime_LUA` 10 and `SetStateCoolTime_LUA` 5 (state `ELYSION_SOLESS_COMBO_ATTACK_Z`),
  `GetBuffAccumulationCountNow` 1 (followed by `GGET BUFF_TEMPLET_ID`, so SOLESS does call it: the buff P10 binding is needed).
- **No new engine name.** The 105 method-style calls in SOLESS, filtered to the C++ / Lua method universe and to names no
  `lua_tinker` line binds today, give 12: `AddEffectSetDeleteCondition_LUA`, `AddStateCoolTime_LUA`, `ClearStoredNPC`,
  `GetNearestUnitInSpecificRange_LUA`, `GetSyncDummyValue`, `PlayAdditionalWorldAllSound`, `PlayWorldAllSound`,
  `SetShowAdditionalWorld`, `SetShowInvincible`, `SetShowWorld`, `SetStateCoolTime_LUA`, `SetSyncDummyValue_Host`. All 12 already have
  a row in `region_engine.tsv` (owners E7; the Opus-only ones are listed there), so the TSV is unchanged. (A1b's 29 counted names that
  were bound afterwards by C3z-4 / C3z-4s / buff P10; the string-only gauge names are among them.)
- **`--region elysion` baseline** (`region_census.py --region elysion --no-probe`, tree at `bdfc0614`): 353 region files, 1,273 open of 1,317
  items. Sets all present except 2 quest NUIs (`NUI_STORY_QUEST`, `NUI_TRANSCEND_QUEST`: quest plan). Open per line:

  | Line | Open | assets.adopt | keys.file | npc.ready | koms.unstaged | bind.unbound (+.bytecode) | other |
  |---|---|---|---|---|---|---|---|
  | VIL (village) | 97 | 69 | - | 11 | 2 | - | ids.missing 2, keys.registry 11, lists.hole 2 |
  | SC (SkyCrossroads) | 130 | 89 | 22 | 13 | 6 | - | - |
  | AR (Adrian) | 237 | 124 | 72 | 32 | 8 | 1 | - |
  | SL (Soless) | 431 | 287 | 72 | 45 | 13 | 8 + 6 | - |
  | ME (Memory) | 378 | 252 | 78 | 30 | 10 | 8 | - |

  Totals match the text report (assets 2,679 refs: 872 in March, 1,733 June-only, 74 June itself lacks; effect sets 416 / 0 missing;
  strings 0 undefined). `members.june_nil` INFO rows: `LINE_TYPE.LT_INVALID`, `NPC_UNIT_ID.NUI_ELYSION_S_CIRCLE_CTR` (keep verbatim).
- **Open for the user:** nothing. E10 reads SOLESS through `python tools/ljbc.py list` on the `ljconv` output (every call and constant
  resolves); it does not need a source copy. Next: `/x2-phase atlas E1` (Opus 5.5 high; kickoff `kickoffs/atlas/E1.md`).

---

## Phase E1 — Village engine

**Steps** (`SERV_IRUHADEV_ELYSION_VILLAGE`, June addresses from A1a):
1. **NPC relation system:** bind `AddVillageNpcRelationDataBy{Village,Quest}_LUA`
   on the quest manager, store June's records, and port June's whole
   evaluation (which NPC/UI a house shows per village and per quest state).
   Load `DLG_Map_House_NPC_RelationData.lua` where June does. The ByQuest
   branch evaluates against the quest manager as June does; with no
   Elysion quests in data it yields the default NPC — **dormant, not
   stubbed** (comment names the quest plan).
2. **`DLG_Map_House` keys:** parse and apply `NPCMeshName2nd`,
   `AlphaBlend2ndMesh`, `AlwaysShow`, `bOnlyClickable`, `WAIT01_MOTION_NAME`,
   `bApplyAlphaTest`, `FRAGMENT_EXCHANGE`, `TalkLoopAniNpc` with June's
   semantics (`X2TFieldNpc*.cpp`, `HouseParsing`).
3. **In-village warps:** `InternalPlaceStartPosID` + `WaitingTime` in
   `VillageParsing` and the runtime that moves the player (June's version).
4. Add the matching C++ enum for `VILLAGENPC_RELATIONDATA_TYPE` with explicit
   values equal to A3's Lua values.
5. *(Added by june-effect C9h-2e, 2026-10-08; rows are funcmap `Effect C9h-2e`.)*
   Step 1's June addresses:
   - loaders `3233b680` (ByQuest) and `32340720` (ByVillage);
   - lookups `32340320` and `323446c0`;
   - the 0x388 row (`+0` quest, `+4` house, `+0xc` code event, `+0x80`
     village), plus ByQuest's second 0x14-byte shape at `+0xbc`.

   Port all three entry points:
   - village NPC creation `323ba5f0`;
   - the per-quest refresh `32340630` (from three quest-UI handlers);
   - the code-event refresh `32344880`, called from
     `Handler_EGS_UPDATE_CODE_EVENT_NOT`'s `SERV_IRUHADEV_JUNE_PET_PREVIEW_LOOKUP`
     block on `main`.

   The TFieldGame side is `323b7ff0` (replace), `323a9ed0` (remove) and
   `323b8970` (restore).

   Reach on March data:
   - the 27 code-event rows (event 2308, quest 0) stay dormant offline;
   - the ByVillage row for `VMI_SPA_RANOX` (1501, a **March** village) swaps its
     billboard to June's `UI_NPC_BILLBOARD_SPA.lua`, which March lacks. Settle
     this row before claiming "existing villages unchanged".
6. *(Added by june-effect C9h-5, 2026-10-08; rows are funcmap `Effect C9h-5`,
   block list `june2017_re/c9h5_fieldnpc_onframemove_diff.md`.)* Step 2's key
   list is short: June's `HouseParsing` (`324f3260`) reads **56** keys March
   does not (decoded list in the C9h-5 Done note). Port June's
   `CX2TFieldNpc` with them, not only step 2's eight:
   - **`OnFrameMove` whole** (`325cdfa0`, 44 blocks, 13 June-only), plus the
     outlined talk / bye steps `325ccf60` / `325ca2f0` (diff those first;
     C9h-5 did not), `325ca0b0` (quest-conditional `MESSAGE` rows, templet
     `+0x590`), the motion map `+0x1f4` (8 `*_MOTION_NAME` keys; June's
     defaults are March's literals plus `Talkloop` / `TalkWait`), the motion
     queue `+0x204` / `325ca920`, the second skin (`NPCMeshName2nd`), the
     `MultiTex` xet, `Upgrade/Service/Trade_Talk`, the house 40082 agit-bank
     and 40129 highlight branches, `bOnlyClickable` (`323bddf0`, not a
     riding-pet getter as C9h-4 had it).
   - **The village-NPC effect set:** key `EFFECT_SET_NAME` -> templet
     `+0x5e8`; play once after the skin is built (`@325ce1d9`), handle `+0x190`
     (ctor -1, stop only in the dtor `325cd7c0`), `SetShowObject( true )`
     and nothing after. Engine: `PlayEffectSet( name, CX2TFieldNpc* )`
     `321e26d0` (instance `+0xe0`, mode `ESM_FIELD`) and its four readers:
     `GetEffectPosition @321dd1a3`, `GetCombineMatrix @321dc8af`,
     `GetEffectPositionByBoneName @321dd9dc`, the create-time anim gate
     `@321eebf3` (no `GetUnitRotation` branch), with the NPC's bone helpers
     `321f2c10` / `321f2c70`. These sit in the same switch cases as C9h-3a's
     `+0xd8` riding-pet branches (`X2EffectSet.cpp`): wait
     for C9h-3a to land on `main` before porting them.
   - **Friendship branches** (gated `IsEnableCode( 2215 )`, never on
     offline): port the gate and the calls as June has them; the manager
     `32696fa0` and the sequencer `325c9e70` belong to backlog **Stage 18**.
     Whichever lands second wires the calls.
   - **Behaviour change in March villages** (June wins).
     - The quest button follows only new normal quests: `SETG` @325cebcd,
       confirmed in raw disassembly.
     - The talk mark no longer blinks: June has no `_Down.dds` literal
       (confirmed).
     - Static NPCs with a `TalkWait` anim play it while joined (per the diff;
       confirm in Ghidra).
     - The `+0x239` completable-quest call is live and its result is
       discarded. `+0x239` is set by `CX2TFieldGame` `323b5970 @323b72fa`,
       which belongs in E1 too.

     Add JUNE_BUGS rows for the diff file's candidates. Add one more for the
     effect instance's raw `+0xe0` pointer if the queued stop lets it read a
     freed NPC.
   - **Assets (E2 / E13):** `EffectSet_Elysion_NPC_Ver_ring`, `_Dyurenda_AC`,
     `_Teo_ring` (June `EffectSet.lua` and `UIEffectSet.lua`) and their
     `NPC_Elysion_*` particles; the three houses are `HI_BERNARD`,
     `HI_DURANDA`, `HI_THEODOR`.

**Done when:** census reports no unread key/binding owned by E1; build rc=0,
new literals in the exe; existing villages unchanged (their files carry none
of these keys — confirm with the census), **except** step 6's June behaviour
changes, which come from code, not keys: each one is listed in the Done note
and confirmed in the play-test, never reverted to March's *(amended by
june-effect C9h-5, 2026-10-08)*.

**Run with:** Opus 5.5, high effort. Required review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch main) --
"Starting context", the ledger's "Elysion half: village" section, the
A1a/A1b maps, and "Phase E1". Use the ghidra skill for June's bodies. Never
launch X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high.

Your task (Phase E1): behind SERV_IRUHADEV_ELYSION_VILLAGE port June's NPC
relation system whole (ByVillage and ByQuest; the quest branch stays dormant
until the quest plan), the new DLG_Map_House keys, and in-village warps
(InternalPlaceStartPosID / WaitingTime), with June addresses cited.

Premises to verify first: where June loads DLG_Map_House_NPC_RelationData.lua;
that no existing March village file uses any of the new keys.

Done when: Phase E1's "Done when" holds. Record a Done note (DEPLOY OK commit), then commit on main.
```

### Phase E1 — Split (user, 2026-10-17)

E1 as worded is one port of four classes' June versions, not one. E1 measured it: June's `HouseParsing` reads ~120 keys (6 sub-tables
March has never seen), June's `CX2TFieldNpc` rewrite reaches into `CX2TFieldNpcShop` (4-argument `SetShopType`, 13 June-only button
ids), `CX2UIUnit` / `CX2UIUnitManager` (the UI file override), the friendship manager (Stage 18) and the agit-bank / Alarm UIs, and
the relation system's TFieldGame side calls those June NPC functions directly. The user split it into four phases, each porting
whole June functions (AskUserQuestion, 2026-10-17, twice: first E1a/b/c, then "4 phases, data first"):

| Phase | Scope | Kickoff |
|---|---|---|
| E1a | June HouseTemplet + whole `HouseParsing`; the relation loaders, lookups and applied lists on `CX2QuestManager`; `VILLAGENPC_RELATIONDATA_TYPE`; bindings; loading `DLG_Map_House_NPC_RelationData.lua`; data staged. Proven by log lines; no in-game change | `E1.md` (this phase) |
| E1b | `CX2TFieldNpc` whole except `OnFrameMove`, plus the NpcShop / UIUnit deltas it calls | `E1b.md` |
| E1c | TFieldGame relation side (323ba5f0 / 323b0980 / 323b7ff0 / 323a9ed0 / 323b8970), the entry points (code event, quest complete), `OnFrameMove` whole + the village-NPC effect set, the assets. Atlas Station's board and postbox change here | `E1c.md` |
| E1d | In-village warps (step 3) | `E1d.md` |

RE carried forward: `june2017_re/e1_houseparsing_diff.md`, `e1_createfieldnpc_diff.md`, `e1_talkbye_diff.md`, `e1_relation_runtime.md`
(the last one also holds the two refresh functions E1a wrote and parked for E1c).

### Phase E1a — Done

2026-10-17, Opus 5.5 high. Flag `SERV_IRUHADEV_ELYSION_VILLAGE` (`KTDXLIB/Always.h`), `[ELYDBG]` lines behind `SERV_IRUHADEV_ELYSION_DEBUG`.
The data half of E1 (split above). Nothing consumes the new data yet, so March villages are unchanged by design.

**What landed**
- **HouseParsing whole** (`324f3260`, raw disassembly; Ghidra cannot decompile it): the ~50 June-only keys in June's order with June's
  miss defaults, into new `HouseTemplet` members (June offsets in the header comments). These are the second mesh / alpha / no-edge,
  anim speeds, OutlineWidthMesh 1.2, bApplyAlphaTest, OLD_ENCHANT x2, PHOTOZONE, FRAGMENT_EXCHANGE, ROCKSCISSORSPAPER, the
  Upgrade / Service / Trade talks, HENIR_EQUIP_UPGRADE, GUILD_AGIT_SERVICE, GUILD_ENTER, EXTRACTION, TRADE, TalkLoopAniNpc,
  the 8 motion names (defaults `Wait01` .. `Talkloop` .. `Normal`), AlwaysShow, bOnlyClickable, bMovingNPC and EFFECT_SET_NAME.
  There are six sub-tables: `CUSTOM_NPC_LOACION` (June's spelling), `NPC_TALK_LIST<n>` (Main_Talk + 5 gift / level talks),
  `NPC_COMMON_TALK`, `Talk_Change_Condition` (only TYPE 0 rows kept, as June), `CUSTOM_MOTION_NAME`, and `RenderOption` (defaults
  copied even without the table). No March key that the US build compiles is dropped. Evidence: `june2017_re/e1_houseparsing_diff.md`.
- **Relation system, data side** (`CX2QuestManager`):
  - the loaders `3233b680` (ByQuest: VRT_NOT_SHOW -> 0x14 rows, VRT_CHANGE -> full rows) and `32340720` (ByVillage);
  - the three lookups `32340320` / `323446c0` / `32340060`;
  - the applied lists (June qm +0xcc / +0xdc), their clear `323c21e0`, and `VILLAGENPC_RELATIONDATA_TYPE` (-1 / 0 / 1 = A3's Lua);
  - the bindings (June 31d95080 @31daf889 / @31dafc2c), and `DLG_Map_House_NPC_RelationData.lua` loaded where June does (`3258c470
    @3259174c`, right after PVPData / PVEData, `X2StateLoading.cpp`).
  The two refresh functions (`32340630`, `32344880`) call the TFieldGame side, so they are written and parked in
  `june2017_re/e1_relation_runtime.md` for E1c.
- **Data:** June's `DLG_Map_House_NPC_RelationData.lua` adopted as ljconv bytecode into data034 (`june_adopt_20261017_065231.tsv`),
  `PACK OK data034` (+1 added, nothing else changed vs live), `COPY OK data034` md5 5f0e61bba11992e4dbe95d5bc038108f (user OK).
- **JUNE_BUGS 207** (the relation row's RenderOption has no ctor in June; the port uses the templet defaults, not verbatim because
  uninitialised memory has no June value) and **208** (replace copies TalkLoop into TalkWait too; E1c ports it verbatim).

**Found on the way (for E1b / E1c, in `june2017_re/e1_*.md`):**
- June's CreateFieldNpc takes a name override (`325cba44`).
- The UI file runs templet +0x120 -> NPC +0x1d0 -> `GetUIUnit( id, file )` (`325cc7a0`, `32806b30`).
- The bye step returns at once unless +0x1a6 is set (`325ca325`).
- PremiumBuffManager +0x5d = PC bang (`326c7a95`).
- June's third quest-complete caller (`323e15e0`, event 0x27d) handles a June-only packet.

**Hygiene:** `flagcheck --worktree`: 7 files, 4 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 0 FAIL; 0 deleted lines, 0 non-ASCII
inserted, 0 encoding changes; RESULT: PASS. Census `--line VIL`: no `DLG_Map_House` key unread (the 4 `NPC_TALK_LIST<n>` hits are
the census's literal matcher; the port builds the name as June does, "%s%d"); the `DLG_Map_Village` keys left are E1d / A5's.

**Deploy:** DEPLOY OK: X2_offline.exe is 19,656,704 B, dated 2026-10-17 07:13:34, md5 53cf41f886ec75a0bbac4e7b0cdc1f95, equal to
X2/US_SERVICE/x2.exe (2026-10-17 07:13:29); built at main 9a622f01 + 7 uncommitted code file(s). The new literals are in the exe
(`CUSTOM_NPC_LOACION`, `AddVillageNpcRelationDataBy{Quest,Village}_LUA`, `Talk_Change_Condition`, `NPC_TALK_LIST`,
L"DLG_Map_House_NPC_RelationData.lua", L"Talkloop").

**How to see it in game:** after data034 is copied, start the game and reach any village. ErrorLog.txt then shows one
`[ELYDBG] VillageNpcRelation: loaded N change row(s), M not-show row(s)` line (June's file has 41 VRT_CHANGE + 6 VRT_NOT_SHOW rows;
rows naming a house or village March lacks drop out), plus `[ELYDBG] HouseParsing June keys: house ...` lines for the houses that
carry June keys. No `[ELYSION] ... did not load` line should appear.

**Play-test 2026-10-17: PASSED (user).** ErrorLog: `[ELYDBG] VillageNpcRelation: loaded 40 change row(s), 6 not-show row(s)` (June has 41 change rows; the dropped one names a village March lacks, likely the ByVillage `VMI_SPA_RUBEN` row), `[ELYDBG] HouseParsing June keys: house 40092 mesh2 'Motion_YunoMonitor.X' ... talkLoop 1`; no `[ELYSION]` load failure, 0 EXCEPTION.

**Open for the user:** JUNE_BUGS 207's choice (templet defaults for a row's missing RenderOption) is a deviation from "verbatim"; say
if you want it otherwise.

### Phase E1a — Review (2026-10-17)

**Opus 5.5 high. Verdict: PASS, no code change, no build, no pack.** Reviewed commit `9dbe7530` against June (Ghidra, read-only; dumps
of `324f3260`, `3233b680`, `32340720`, `32340320`, `323446c0`, `32340060`, `323c21e0`, `32347110` and `3258c470 @3259174c`).

- **Relation loader ByQuest (`3233b680`): equal.** Type read @3233b93c; miss or -1 returns; 0 -> VRT_CHANGE @3233bf0e, 1 -> VRT_NOT_SHOW,
  anything else returns (@3233b958-3233b961). NOT_SHOW row: quest / house / code -1, force 0, state +0xc = 1 (@3233b96c-3233b97c);
  code event miss -1 (@3233bb45); QUEST_ID miss or < 0 and house miss or < 0 return (@3233bc1e-3233bc29, @3233bee8-3233bef3); push
  into qm +0xbc @3233bf04. CHANGE row (base `[esp+0x94]`): type +8 = 0, code +0xc (-1), QUEST_ID +0 and house +4 each `>= 0`-gated,
  APPLY_FORCE +0x324 (false). The change keys (xorstrings rows 3233c4f0..3233fecd, the same set as the port's
  `LoadVillageNpcRelationChangeData`) land on the header's offsets: every motion name checked by stack slot (Wait01 +0x22c .. TalkLoop
  +0x280, Talk +0x29c, TalkWait +0x2b8, Bye +0x2d4, Normal +0x2f0), UIFileName +0xd8, JoinTalk vector +0x164, Rot +0x30c / Scale
  +0x318 (0.0 per missing index, untouched without the table), EFFECT_SET_NAME +0x36c, push into +0xac @32340006.
- **RenderOption in the loaders: equal, plus a JUNE_BUGS 207 widening.** The block is read into a local `[esp+0x50]` only when the
  table opens (@3233f753) and copied by `32345bb0` into row +0x328 @3233fe63. The local has no ctor (only the MultiTex string is built,
  @3233f75b). Every key has a miss default (bools 0 via `xor bl,bl`, Src 5 @3233fc44, Dest 6 @3233fd4b, colour 1 @3233fe01) **except
  `MultiTexSpeed`**, which stays uninitialised when its subtable is missing. The port gives 0 there. Row 207 now says so too (same
  principle: uninitialised memory has no June value). June's data sets it in its only RenderOption row.
- **Relation loader ByVillage (`32340720`): equal.** Type must be 0 (@323409d1-323409e9; 1 and -1 return); code +0xc (-1), VILLAGE_ID
  +0x80 and house +4 each miss/`< 0` -> return (@32340c92, @32340e19); same change-key block; push @3234465f. No QUEST_ID, no APPLY_FORCE.
- **Lookups: equal.** `32340320` (ByQuest): gate = code -1, or `g_pInstanceData` code map entry with byte +0x10 == 1 (= March
  `IsEnableCode`); house match; skip unless +0x324 == 1 or the quest is in qm +0x34; `best.quest < row.quest` -> gate on: r=0, else
  applied +0xcc non-empty: r=2. Smalls compare against the best **row's** quest; gate on r=1, else +0xdc non-empty r=2, else skip
  without recording; r==0 pushes the row to out and +0xcc, r==1 the small to out and +0xdc (@323405ac-323405dc); returns ESI.
  `323446c0` (village = arg1, house = arg2, +0x80 test): last match wins, no quest test, only r==0 pushes. `32340060`: completed-quest
  test first; first row with +0 == quest breaks either way; then first small breaks and overrides r. All as ported.
- **HouseParsing spot-checks (raw disassembly): pass.** OutlineWidthMesh miss -> `fld [0x334a1080]` (1.2f) @324f9ef1, copied to
  +0x18c @324fbd26 (bApplyAlphaTest +0x190 @324fbd38). Talk_Change_Condition TYPE miss -> 1 @324fd9d1, STATE `sbb` 0 @324fda92, row
  dropped unless TYPE == 0 @324fdaa9. RenderOption ctor `323bf7e0` @324fe434 **before** the table, miss jumps to the copy @324feb58
  (both paths copy). The port matches all three. `BeginTable( "NPC_TALK_LIST", n )` resolves to `LuaCommon/KLuaManager.h:596`
  ("%s%d"), as June's `31c7b980`.
- **Load point and bindings: equal.** `X2StateLoading.cpp:756` runs right after `LoadJuneRankReferData` (PVPData / PVEData), as
  June's `@3259174c`. Both `class_def`s use June's names.
- **The dropped row (play-test, 40 of 41 change rows): settled.** It is June's ByVillage `HI_BILLBOARD` / `VMI_SPA_RUBEN` row:
  March's `Enum.lua` has no `VMI_SPA_RUBEN`, so `VILLAGE_ID` is nil -> -1 -> no push. Every other row's house and village resolve.
  C9h-2e's `VMI_SPA_RANOX` ByVillage row (`HI_BILLBOARD`) loads. Whether it shows is E1c's (the lookup has no caller until then).
- **Hygiene:** `flagcheck SERV_IRUHADEV_ELYSION_VILLAGE --range 9dbe7530^..9dbe7530 --with SERV_IRUHADEV_ELYSION_DEBUG`:
  `files: 4 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
  `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
  `RESULT: PASS`. No OTHER rows, no deleted lines. The two PASS~ files (`X2LocationManager.h`, `X2QuestManager.cpp`) differ by one blank line
  outside the flag; DEFINE = `Always.h`.
- **JUNE_BUGS 207, user decision 2026-10-17 ("follow what June offers"): kept as ported.** June leaves the block uninitialised, so it offers no value. Its own defaults (`323bf7e0`) are the only June-defined ones, and June's data sets `RenderOption` explicitly where a look matters (even `UseRenderOption = FALSE`, :832). Row 207 has the reasoning.

E1b's kickoff now reads this section too.

### Phase E1b — Done

2026-10-17, Opus 5.5 high. Flag `SERV_IRUHADEV_ELYSION_VILLAGE`, `[ELYDBG]` behind `SERV_IRUHADEV_ELYSION_DEBUG`. June's `CX2TFieldNpc` whole except
`OnFrameMove`; the June bodies sit in one flagged block at the end of `X2TFieldNpc.cpp`, each March original kept in `#else`.

**What landed** (June addresses; RE in `june2017_re/e1_createfieldnpc_diff.md` / `e1_talkbye_diff.md`)
- ctor `325d13e0` members, dtor `325cd7c0` (bye step first, skin / mesh / xet 2, MultiTex xet, 9 particle handles, UI effect-set
  stop of +0x190), InitParticle `325c84b0` (+ `NpcTalkParticle`, `acce_highLight_P01`).
- `CreateFieldNpc( world, templet, name )` `325ca980` whole: CUSTOM_NPC_LOACION `325c95e0`, 6-id static list, second skin / mesh /
  xet, MultiTex (background path only), name override, June flag set, motion map, CUSTOM_MOTION_NAME queue, 4-arg SetShopType in
  June's order, the 2204 block, NPC_TALK_LIST map, June talk strings, 3-id shadow gate, UI file name copy.
- Talk step `325ccf60` / bye step `325ca2f0` (March's OnFrameMove blocks call them), 325ca920 `DisableNpc`, `SetEnable`
  `325ca950`, `SetShowField` `325c9130`, `SetShowObject` `325c9000`, `GetShowObject` `325c8210`, `ResetNpc` `325c8ce0`,
  `SetCanTalkNpc` `325c8250`, `ApplyQuestCondition{Message,Motion}` `325ca0b0` / `325ca1b0`, `NpcPlaySound` `325c9c00`.
- **OnFrameRender_Draw** `325d1040` (it is vtable `334acc6c` +0xc, so it has no direct caller): skin 2, AlphaBlend1st/2ndMesh,
  NoEdge2ndMesh, bApplyAlphaTest, RenderOption (blend, colour, MultiTex UV speed x `+0x1f0`), name drawn up to 2700 (March 2300).
- **RenderName** `325ccb40` (not in the kickoff list, found June-changed): Ranox spa sign x-150, agit-bank label STR 34403.
- `CX2UIUnitManager::GetUIUnit( id, file )` `32806b30`, `CreateUIUnit( id, file )` `32806910`, `ClearUIUnit( id )` `32806800`,
  `CX2UIUnit::IsFileNameChanged` `32ee37a0` + the +0x35c override flag; the 1-arg forms forward with L"" (June's other callers).
- `CX2TFieldNpcShop`: 4-arg `SetShopType` (`32ecd610`), `SetShowFriendshipScore` (`32ec8d40`), 10 June-only `NSBT_JUNE_*` ids.
- JUNE_BUGS 210-213 (bye guard, the unreachable 2204 call, UI-unit rebuild, CUSTOM_NPC_LOACION world).

**Decisions and dormant June branches**
- **June-only buttons (user, 2026-10-17: register, don't show).** Live houses set HENIR_EQUIP_UPGRADE (Steel, Faceoff, Syota,
  Edan event NPC) and GUILD_ENTER + GUILD_AGIT_SERVICE (Mu). March's SetShopType would hide every button for an unknown type, so
  the 4-arg form drops them with an `[ELYDBG] NpcShop: ... has no March UI` line. A later phase can port June's NPC dialog.
- **Guild base** (`g_pData +0x2ec` = CX2GuildManager, June state 0xf = `XS_GUILD_AGIT_GAME`): the HI_MU menu gating, the
  agit-bank UI 0x1a and its label are June call sites only. March has no guild base, so they never fire.
- **Friendship** (code event 2215): the override lookup and the score gauge are call-site comments for backlog Stage 18.
- **Alarm_Button** (`326662c0` on g_pMain +0x1058): no March dialog; noted at both call sites.
- Bye calls `UpdatePartyMenu()` as June does; June's body (322c5980) is a rewrite. March's body runs here.

**June behaviour changes in March villages**
- Talk / bye motions come from the motion map, so the names are now `Talk` / `Bye`. Static NPCs try `Talk` / `Bye` first, then
  fall back to `Normal`. March used `normal`, and the name match is case-sensitive.
- Speeds are JoinAnimSpeed / ByeAnimSpeed.
- Static NPCs play a JoinTalk sound on talk.
- Bye clears only this NPC's UI unit.
- The static list adds GUILD_MARK / STORY_QUEST / TRANSCEND_QUEST (+40082).
- No shadow under HI_YUNO.
- Names are drawn to 2700.
- June drops March's synthesis / web-event / wedding buttons.
- SetCanTalkNpc treats GUILD_MARK / SPA_SIGN_RANOX like the tree.
- DeActive plays only if the mesh has it.
- `SetShowField` honours its argument.

**For E1c:** `NpcTalkParticle` is absent from live `UIMajorParticle.txt` (June has it in `UIMajorParticle_INT.txt`). OnFrameMove
sets `+0x1f0`, so read it there. The June ctor members E1b did not name are +0x1a8 = 1, +0x238 (word), +0x42a (byte) and +0x508
(wstring); OnFrameMove is where they get names.

**Hygiene:** `flagcheck --worktree --files <8 code files>`: `files: 7 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (PASS~ = `X2UIUnitManager.cpp`, one blank line). No compiler warning from the changed files.

**Deploy:** DEPLOY OK: X2_offline.exe is 19,669,504 B, dated 2026-10-17 08:27:35, md5 c84251e6b4babc375851c625288038e3, equal to
X2/US_SERVICE/x2.exe (2026-10-17 08:27:30); built at main 1ddd6644 + 8 uncommitted code file(s). New literals in the exe: `NpcTalkParticle`,
`acce_highLight_P01`, the `[ELYDBG] NpcShop` format, `NPC_FRIEND_SCORE_LEVEL`.

**How to see it in game:** see the play-test hand-off (talk to and leave a village NPC, the postbox, the billboard; `[ELYDBG]
NpcShop` lines for Steel / Faceoff / Mu).

**Open for the user:** none blocking. Two items have no owner yet: the guild-base gating, and June's NPC-dialog buttons (above).

**Play-test 2026-10-17: PASSED (user, "good job").** Run after the deploy (08:33), two village loads, clean exit. ErrorLog `[ELYDBG] NpcShop`: button 29
(HENIR_EQUIP_UPGRADE) on houses 40075 / 40076 / 40077 (Steel / Faceoff / Syota) and 40090 (Edan event, npc -1: its empty NPC_ID_LIST,
`[RANOXDBG]` guard as before); 30 + 37 (GUILD_AGIT_SERVICE, GUILD_ENTER) on Mu 40046. These are the live houses the phase listed, one line per
button per load. Relation and HouseParsing lines as in E1a; 0 EXCEPTION. The 3 UNHANDLED packets (`E_DISABLE_HB_CHECK_REQ`, `EGS_RENEWAL_LETTER_REQ`)
are not this phase's.

### Phase E1b — Review (2026-10-17)

Opus 5.5 high. **Verdict: PASS on everything E1b ported; three June functions it missed are now ported under the same flag** (no design question).

**Hygiene of 741ba60f** (`flagcheck --range 741ba60f^..741ba60f --with SERV_IRUHADEV_ELYSION_DEBUG`): `files: 7 PASS, 1 PASS~ (blank/marker only), 0 DEFINE,
0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif
without the flag token, 0 encoding change(s)`; `RESULT: PASS`. No OTHER rows, no deleted lines; PASS~ is `X2UIUnitManager.cpp`'s one blank line.

**Spot-checks in raw disassembly (all match the code)**
- `325ca0b0`: row +4 == state, MESSAGE (+0x10) non-empty, row +0 (TYPE) == 0, QUEST_ID +8 -> 32322380, first 8-byte sub-quest with byte +6 == 0 -> 32322350,
  clear type +0x20 == 1, talk NPC +0x60 == +0x24c, last match wins. **Byte +6 is March's `m_bIsSuccess`:** June's `SubQuestInst::IsComplete` `323225e0` returns
  byte +6 for non-collection types (types 3/4: +6 == 1 or the inventory count), March's `IsComplete` returns `m_bIsSuccess`. June's entry has one more byte before it.
- `325d1040`: the RenderParam offsets equal March's struct (+0 renderType, +8 lightPos, +0x14 fOutLineWide, +0x18 outLineColor, +0x28 colour, +0x4c/+0x54/+0x5c
  stage 0-2, +0x64 bAlphaBlend, +0x66 bZEnable, +0x68 cullMode, +0x6c/+0x70 blends); +0xa4 is past March's FOG_WORLD block, so June's struct grew by 8 bytes there
  (named bAlphaTestEnable by the D3DCULL_NONE pairing). T+0x190 == 1 skips AlphaBlend1st and the RenderOption (`jmp 325d127f`); stage = ftol(T+0x5cc), offsets +=
  T+0x5d0/0x5d4 x this+0x1f0; name gate 2700 (334acd70) or house 0x9cd3; skin 2: T+0x11d blend, T+0x11e renderType 3 (RT_CARTOON), then pick.
- `325c8250`: houses 0x9c64 / 0x9c93 / 0x9ccd / 0x9cce = March HI_CHRISTMAS_TREE / HI_GUILD_MARK (= 40083) / 40141 / HI_SPA_SIGN_RANOX (= 40142); DeActive
  plays only when ChangeAnim returns 1 (`325c837f`); CloseAll + m_bBye only when m_bJoinNpc. HI_YUNO = 40092, HI_STORY_QUEST = 40130, HI_TRANSCEND_QUEST = 40140 (explicit
  in the March enum).
- `325cc17c-325cc219` re-read: CreateFieldNpc's 2204 inner call does re-test the code (`325cc202`), JUNE_BUGS 211 stands.

**Not diffed by the phase:** `GetQuestInfo` `325c81a0` same (GetNewQuest( npc, 3 counters, level ) + GetCompleteTalkQuest); `ChangeType` `325c8130` same (NONE then
type, 4-arg with 0/0); `UICustomEventProc` is a forwarder (no June body to diff). June changed three, and E1b had left March's bodies:
- **`ResetNpcType` `325c9800`**: CreateFieldNpc's button list again, all `( type, true, false, false )`, June's own order (June-only buttons after the 2204 block), no
  wedding / web-event / synthesis (March's still re-added them after a menu reset), and the 2204 block in its reachable form (JUNE_BUGS 211 widened).
- **`GetHouseType` `325c8030`**: March's compiled exclusion list (cl /P: 40029, 40017, 40038, 40045, 40047, 40048, 40050) is June's first seven; June adds 4309,
  4346, 4254, 4255 (no March house) and house 40147 -> NT_NO_RENDER_EVENT. Same result for every March house.
- **`InitQuestMark` `325c8d60`**: also hides +0x230 (NpcTalkParticle).
- Also seen: `325c8430` is a June-only guild-base setter for the HI_MU flags (state 0xf), out of reach, as the Done note says for the gating.

**Fix hygiene** (`flagcheck --worktree --files X2Lib/X2TFieldNpc.cpp`): `files: 1 PASS, ... 0 FAIL`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched
inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (+135 lines, insert-only).

**Deploy:** DEPLOY OK: X2_offline.exe is 19,670,016 B, dated 2026-10-17 08:52:20, md5 caae215304cb6b04358752e595c6fced, equal to X2/US_SERVICE/x2.exe (2026-10-17 08:52:15); built at main 4c9cf74e + 1 uncommitted code file (X2TFieldNpc.cpp; a peer's queued build compiled it, X2TFieldNpc.obj 08:49:37 is newer than the edit 08:46:39).

**How to see it in game:** nothing new to see. `ResetNpcType`'s only March caller is the novice guide (`CX2TFieldGame::ResetNpcType( HI_ARIEL_EVENT_HOUSE )`),
`GetHouseType` returns the same type for every March house, and the talk particle is already hidden by InitParticle. A village load and an NPC talk on the new exe are the
regression check. Nothing owed by the user beyond that.


---

### Phase E1c — Done

2026-10-17, Opus 5.5 high. Flag `SERV_IRUHADEV_ELYSION_VILLAGE`, `[ELYDBG]` behind `SERV_IRUHADEV_ELYSION_DEBUG`. Whole June functions, March bodies in `#else`.
The relation data E1a loaded now reaches the village: Atlas Station's postbox and billboard become June's Elysion meshes and board UI.

**What landed** (June addresses)
- **TFieldGame** (`X2TFieldGame.*`): village NPC creation `323ba5f0` (replaces March's `AddFieldNpc()`: ByQuest then ByVillage lookup,
  0 = changed NPC, 1 = not shown, else plain path with June's gates: code events 2013 / 2081 / 2186 for houses 40091 / 40129 / 40089,
  house 40090 only for a comeback user, spa signs 40141 / 40142 need code event 31), `AddFieldNpc( templet, name )` `323b0980`,
  replace `323b7ff0` (templet copy kept in TFG +0x444, freed in the dtor as June @323b92a1), remove `323a9ed0`, restore `323b8970`;
  June's NPC loop of OnFrameMove `323b5970` @323b70cb-323b7475 (disabled NPCs skipped, AlwaysShow, effect-set show / hide,
  quest info once a second and +0x239). The 7 house ids June compares (40082 / 40089 / 40091 / 40141 / 40143 / 40144 / 40147) in `HOUSE_ID`.
- **Entry points**: `RefreshVillageNpcRelationDataByCodeEvent` `32344880` after the code-event sets (`X2State.cpp`, 31f00970 @31f00a61);
  `RefreshVillageNpcRelationDataByQuest` `32340630` after `RemoveUnitQuest( q, true )` in QUEST_COMPLETE_ACK (both SERV_DAILY_QUEST forms)
  and ALL_COMPLETED_QUEST_COMPLETE_ACK (`X2UIQuestNew.cpp`). June's third caller handles a June-only packet.
- **CX2TFieldNpc::OnFrameMove** `325cdfa0` whole (44 blocks, `c9h5_fieldnpc_onframemove_diff.md`): both skins load together, MultiTex xet
  on skin 1, the talk particle / 40129 highlight, June's quest-mark exclusions, the motion map (ids 1-3, 10 TALKLOOP, 11 the
  CUSTOM_MOTION_NAME queue, June ctor motion id 1), static TalkWait / Normal, QuestMsgOverride on every NPC message (states 0-8),
  `m_bInConversation` gates. The friendship mark / sequencer / gift cursor (Stage 18), the agit-bank latch and June's UIs 9 / 0xa / 0xb
  are June call sites only (comments).
- **Village-NPC effect set**: `EFFECT_SET_NAME` played after skin 1 is built (@325ce1d9) on the UI effect set; `PlayEffectSet( name,
  CX2TFieldNpc* )` `321e26d0` and its readers in `X2EffectSet.cpp` (instance +0xe0 `m_pFieldNpcHolder`): GetEffectPosition @321dd1a3,
  GetEffectPositionByBoneName @321dd9dc, GetCombineMatrix @321dc8af, create-time anim gate @321eebf3; NPC bone helpers 321f2c10 / 321f2c70.
- **Assets** (`kom.py stage-june`, `adopt-june` = June bytecode): data013 `EL_postbox.X`, `NPC_EL_BILLBOARD.X`, `Motion_Npc_EL_Billboard.x`,
  `Motion_NPC_SPA_BILLBOARD.x` + 4 textures (`Elysion_Tower_postbox01.tga`, `Field_Atlas_rest_board*.tga` x3); data039
  `UI_NPC_BILLBOARD_ELYSION.lua`, `UI_BILLBOARD_ELYSION.lua`, `UI_NPC_BILLBOARD_SPA.lua` (`june_adopt_20261017_092513.tsv`).
  `PACK OK data013` (+8) / `PACK OK data039` (+3), nothing else changed vs live; `COPY OK data013` md5 036e1a19, `COPY OK data039` md5 d5fcc06e (user OK).
- **VMI_SPA_RANOX row settled**: June wins; the spa village's billboard becomes June's `Motion_NPC_SPA_BILLBOARD.x` + `UI_NPC_BILLBOARD_SPA.lua`
  (both now staged). JUNE_BUGS 218-225.

**June behaviour changes in March villages** (from code, kept)
- Quest button only for new normal quests (JUNE_BUGS 221); talk mark no longer blinks (223); quest info refreshed once a second, not every frame.
- Postbox / billboard / Elysion trees use the One_Use mark; NPCs at (0,0,0), non-talkable NPCs and 40082 / 40143-40146 get no quest mark.
- Static NPCs play TalkWait while joined when the mesh has it (else Normal); idle motion speed is WaitAnimSpeed.
- House 40090 (Edan guild event NPC, a live March house) now shows only for a comeback user; E1a's "PC bang" reading of byte +0x5d was a guess,
  the buff port's Phase 11 review cites it as `m_bComeBackUser` (review to confirm).
- March's HI_EVENT_BENDERS / HI_APINK_ARCHANGEL gates and A-Pink music in AddFieldNpc are compiled out of this build (flags off), nothing lost.

**Not applicable / noted**: the effect instance's raw +0xe0 pointer cannot outlive the NPC here, E1b's dtor stops the set at once
(March's StopEffectSet), so no JUNE_BUGS row for a queued stop. Block 44 (in-house head scale) kept: June's service exe compiles it out.

**Hygiene** (`flagcheck --worktree --files <11 code files>`, before commit): `files: 10 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
No compiler warning from the changed files. X2TFieldGame.cpp / .h and X2LocationManager.h also carry peer E1d's uncommitted hunks (same flag); this commit stages E1c's hunks only.
**Deploy:** DEPLOY OK: X2_offline.exe is 19,699,712 B, dated 2026-10-17 09:36:53, md5 d095fe3a0be8795c9ab25525a18978f8, equal to X2/US_SERVICE/x2.exe
(2026-10-17 09:36:42); built at main f9c2b2a1 + 16 uncommitted code file(s) (E1c + peer E1d WIP). Literals in the exe: `[ELYDBG] NpcEffectSet`,
`[ELYDBG] VillageNpcRelation: village %d house %d -> changed NPC`, `rebuilt (quest %d`, `completed -> result %d`.
**How to see it in game:** see the play-test hand-off.

**Play-test 2026-10-17: PASSED (user).** Run 10:16 on the deployed exe (md5 d095fe3a) with data013 / data039 copied. ErrorLog: on both Atlas Station visits (10:16:22, 10:18:09) `house 40028 rebuilt (quest -1, village 1008) mesh 'NPC_EL_BILLBOARD.x' ui 'UI_NPC_BILLBOARD_ELYSION.lua'` and `house 40021 rebuilt ... mesh 'EL_postbox.x'`, each followed by `-> changed NPC`; no load failure for the new meshes / textures / UI Lua. User: June's postbox and board shown, the board opens the Elysion board window; March village NPCs, talk marks, shop and bye normal. 0 EXCEPTION (3 UNHANDLED are the usual heartbeat / letter packets).

### Phase E1c — Review (2026-10-17)

Opus 5.5 high, against raw capstone disassembly (`funcdis.py --func`, `disasm_range.py`; Ghidra was not running). **Verdict: PASS, no code
change, no build.** Commit `8ee92c45` reviewed.

**Hygiene** (`flagcheck SERV_IRUHADEV_ELYSION_VILLAGE --range 8ee92c45^..8ee92c45 --with SERV_IRUHADEV_ELYSION_DEBUG`):
`files: 11 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`. No OTHER rows, no deleted lines.

**Re-read in the binary, all equal to the port**
- **323ba5f0**: ByQuest then ByVillage on -1 (village id = location manager +0x64 @323ba701); result 0 replaces (only when a row came
  back), 1 builds nothing, any other value is forced to -1 (@323ba71e) and takes the plain path, so result 2 shows the plain NPC. Gates:
  the three event houses are a find in the instance data's code-event map (`322aa950`, absent or byte +0x10 = 0 = off), skipped
  only with instance data present; 40090 needs mgr +0x5d; the spa signs need code 31, shown when instance data is null. No quest manager
  (+0x4c null) goes straight to the plain path.
- **323b0980**: world id from world +0x2c -> [0]; a NULL world or an empty COMMON list counts as found; CUSTOM non-empty also builds;
  layer 10, AddObjectChain, `325c9000( 0 )`; the spa-manager tail (+0x624) is the noted March gap.
- **323b7ff0**: every row -> templet offset re-matched (mesh / anitex x2, the 3 bools, UI file, texture name / key, house name, both
  sound vectors, 6 talks, 3 waits, TALK, `+0x280` into both `+0x53c` and `+0x504` (JUNE_BUGS 208), BYE, NORMAL); AlphaBlend2nd parses
  `+0x84` (ebp still `+0x84` @323b8204, JUNE_BUGS 218); rot / scale "any part non-zero", rot also into every COMMON row `+0x10`;
  RenderOption copied unconditionally (bytes, src / dest, colour, speed, MultiTex string @323b8864); effect set only when non-empty; name
  `+0xf4`; push into TFG `+0x444`.
- **323a9ed0** (every NPC of the house -> `325ca920`) and **323b8970** (type 0 rows -> replace with a fresh `32347110` row carrying
  only the house, erased; small rows with `+0xc` == 1 -> the last NPC of the house `325ca950( 1 )`, else AddFieldNpc( templet, L"" ), erased).
- **NPC loop @323b70cb-323b7475**: +0x1a1 cleared first, `!+0x5fa` skips the NPC and its OnFrameMove, AlwaysShow or distance <= 2500.0
  (constant 0x3344911c read from the exe), effect set shown / hidden on `inst +0xdc`, `1.0 < timer` -> `325c81a0` + `+0x239`, hidden dialog
  -> not in area, join area `dist <= +0x5e4`, timer zeroed after the loop. March's loop did nothing else: its only other code is the
  A-Pink sub-volume block, and `APINK_NPC` is commented out in Always.h.
- **Entry points**: 31f00970 @31f00a61 calls 32344880 right after the two sets; 323e57e0 @323e5903 and 323e1710 @323e1864 call
  32340630 right after `RemoveUnitQuest( q, 1 )`.
- **325cdfa0**: block 14 (40021 / 40082, all-zero pos, `!+0x5fc`, 40143-40146 jump to block 19; hidden or not-shown jumps to 20);
  block 18 `SETG` and `SetShopType( 1, b, 0, 0 )`, gated on `+0x5a8 == 0`; block 20, all three timer branches' jump tables
  (325d0e8c / 0f14 / 0f9c) send exactly 40021 / 40028 / 40082 / 40145 / 40146 to `One_Use_Up` and the rest to `Npc_Talk_UP`, the reset
  at `1 < t`; block 37's table 325d1024 / 325d1034 (id 1 -> talk box, 2 / 3 / 10 -> map, 11 -> queue or `325ca920`, 4-9 -> map[1]),
  event NPC Wait01 / Active_Wait01, skin 2 for ids 1-3 / 10 only; block 38 `HasAnim( map[5] ) == 1 && JoinNpc` -> Play 0, else map[0]
  Play 2, both skins; block 42 `+0x1a6 && T+0x42c` -> 10, rand%9 0 / 1 toggles, queue -> 11, missing anim with an empty queue -> 1.
- **Effect set**: 321e26d0 writes only `+0xe0` and mode 2; the readers @321dd1a3 (bone `+0x40` or NPC `+0x5b4`), @321dd9dc
  (`321f2c70`, found byte), @321dc8af (skin `+0x1b8` frame `+0x90`, else NULL), @321eebf3 (skin anim time `+0x214` vs start + delay, no
  skin falls through) each return without SetEffectSetOffset, as ported.

**Settled: premium buff manager +0x5d = `m_bComeBackUser`.** 326c7a10 is June's handler of event 0x724 (dispatch 326c9bb0): on a true
packet in state 5 it sets +0x5d = 1 and builds two dialogs, on false it clears +0x5d / +0x68 / +0x69. That is March's
`Handler_EGS_COME_BACK_USER_NOT` (server-select state, the Return_User popups), and 323bbcb9 gates the comeback chat on the same
byte. The PC-bang flag is +5, set by 326c8c70 (event 0x53b). `IsComeBackUser()` stands; E1a's "PC bang" note was wrong.

Compared with the diff file only, not re-read in the binary: blocks 2-13, 22-36 (message paths, ApplyQuestConditionMessage states
0-8, the UI id renumbering) and 43; the port follows the diff file there, and every block that was re-read matched it. User owes nothing; `kickoffs/atlas/E2.md` was already written by a peer session.

### Phase E1d — Done

2026-10-17, Opus 5.5 high. Flag `SERV_IRUHADEV_ELYSION_VILLAGE`, `[ELYDBG]` lines behind `SERV_IRUHADEV_ELYSION_DEBUG`. June's in-village warps: the
parser keys and the runtime, all as whole June functions. RE is in funcmap rows `Atlas E1d` (13 new, 4 confirmed).

**What landed**
- **`VillageParsing` `324ff4b0`, diffed whole.** It reads seven key names March does not; all are now parsed with June's defaults:
  - `CameraModifyDistanceByPortal` (templet +0x58, miss 3000 = March's literal) and `SPA_INFO` (Type, miss -1, + LineIdx list; templet +0x70);
  - in `START_POS` (June's row is 0x24 bytes): `IsGuildAgitEnter` +0x1a, `WaitingTime` +0x1c (miss 3.0),
    `InternalPlaceStartPosID` +0x20 (short, miss 0), `IsSpaEnter` +0x22, `IsSpaOut` +0x23.
  - A duplicate StartPosId now frees the new row (@32500dbb; March leaked it).
  - June's `OtherPlaceID` / `OtherPlaceStartLineIndex` stay read under March's names `BattleFieldId` / `BattleFieldStartLineIndex`
    (A5's choice for this core-list file). E2 transcribes Elysion's rows the same way.
  - June's `NAME` branch (@3250167d, a string-manager flag) is the same key, so it is not ported.
- **Runtime, June's version:**
  - TFieldGame `OnFrameMove`'s portal block (`323b5970` @323b6d1d-323b6fd5). It replaces March's `X2TFieldGame.cpp` 1622-1696:
    - the target id resets to -1 every frame, and `IsNearPortalToBattleField()` is now `id != -1`; June has no bool;
    - the camera range is the village's `CameraModifyDistanceByPortal`, with no portal camera for an internal place or inside a spa (1500 / 1501);
    - a start position within 100 leading to a battle field, 25000, a village / lounge / rest area or an internal place becomes the target;
    - the target's `WaitingTime` becomes the portal timer (`323aa1b0`).
  - `CX2StateField::MoveToOtherPlace` `3250a800` whole:
    - an internal place goes to `MoveToInternalPlace` `323ac300` (SetVillage, the unit's position, Portal.ogg);
    - a village / lounge / rest target sends `EGS_STATE_CHANGE_FIELD_REQ` (skipped while its ACK is awaited);
    - anything else joins the battle field, as before.
  - `IsInternalPlace` `323aa170`. The TFieldGame ctor sets the id to -1 and the camera range from the templet.
  - `SetNearPortalToBattleField( false )` clears the id the way June's two StateField ACK handlers do (`323aa1b0( -1, 3.0 )`).
- **`GetVecLinkedPos` `324f2ff0`:** June also drops guild-base-entrance, spa-entrance and spa-exit rows, and with code 2319 drops portals to 40037. June's three side lists
  (`324f2c20` / `324f2ca0` / `324f2d30`) feed the guild base and spa only and are not ported. Neither system exists in March.
- JUNE_BUGS 216 (the in-village move passes the target StartPosId as SetVillage's line index) and 217 (ctor leaves +0x358 unset without a templet; the port uses 0).

**Census (live `data034 DLG_Map_Village.lua` vs June's):** March carries none of `WaitingTime`, `InternalPlaceStartPosID`,
`CameraModifyDistanceByPortal`, `SPA_INFO`, `IsGuildAgitEnter`, `IsSpaOut`. It has one `IsSpaEnter` row (Ranox, StartPosId 146, `OtherPlaceID =
VMI_SPA_RANOX`), and every one of its 37 portal targets is a battle field. June uses the warp keys only in Elysion (20009): six internal pairs
500-513, `WaitingTime 1.5`. **No reachable data uses the in-village warp yet. E2's `Village[VMI_ELYSION]` rows bring it.** No
`VillageParsing` key owned by E1 is left unread.

**June behaviour changes in March villages:**
- Ranox's spa-entrance row (146, at the Ranox door) is no longer a portal. March's code listed it as one: gate effect within 2000, camera tilt within 3000,
  never a move, since it has no BattleFieldId. The door itself is world art (user, play-test: "just the door"). Entering the spa through it is June's spa system: backlog Stage 24.
- Every village portal now runs June's block. Results are the same for March's battle-field portals: same 100 / 2000 / 3000 ranges and a 3.0 s wait.

**Hygiene:** `flagcheck SERV_IRUHADEV_ELYSION_VILLAGE --worktree --with SERV_IRUHADEV_ELYSION_DEBUG --files <6 code files>` right after the edit: `files: 5 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`
(PASS~ = one blank line before the helpers in `X2TFieldGame.cpp`). E1c ran alongside, under the same flag and in the same TFieldGame / LocationManager files. The commit carries only E1d's hunks (HEAD + the E1d byte patch).

**Deploy:** DEPLOY OK: X2_offline.exe is 19,699,712 B, dated 2026-10-17 09:36:53, md5 d095fe3a0be8795c9ab25525a18978f8, equal to X2/US_SERVICE/x2.exe
(2026-10-17 09:36:42); built at main f9c2b2a1 + 16 uncommitted code file(s). A peer's queued build compiled the edits: the `.obj` files are 09:32-09:34, after the 09:12-09:15 edits, and E1c's WIP is in too. The exe holds the new
literals: `CameraModifyDistanceByPortal`, `InternalPlaceStartPosID`, `WaitingTime`, `IsSpaOut`, `IsGuildAgitEnter`, `SPA_INFO`, `LineIdx`, and the three `[ELYDBG]` formats.

**How to see it in game:** the play-test hand-off: a portal walk in a March village, Atlas Station's portals, Ranox's spa entrance. Each set target logs
one `[ELYDBG] Portal: start pos N -> place M internal 0 wait 3.00` line.

**For E2:** Elysion's `START_POS` rows carry `WaitingTime` / `InternalPlaceStartPosID` and use March's `BattleFieldId` names. The internal move then logs
`[ELYDBG] InternalPlace: start pos 502 -> 503 ...`, and the village load logs `[ELYDBG] VillageParsing June keys: village 20009 internalPlaceRows 6 ...`.

**Play-test 2026-10-17: PASSED (user).** The run (10:16-10:20) used the deployed exe (md5 d095fe3a).
- Ranox: `[ELYDBG] Portal: start pos 142 -> place 40025 internal 0 wait 3.00` at 10:17:45, then `EGS_JOIN_BATTLE_FIELD_REQ` at 10:17:48.8.
- Atlas Station: `start pos 403 -> place 40035` at 10:18:24, then the join at 10:18:27.7. Both moves waited 3.0 s, as June's default.
- No line names start pos 146: Ranox's spa entrance is no longer a portal.
- 0 EXCEPTION. The UNHANDLED `E_DISABLE_HB_CHECK_REQ` / `EGS_RENEWAL_LETTER_REQ` are not this phase's.

### Phase E1d — Review (2026-10-17)

Opus 5.5 high, against Ghidra's raw disassembly plus `disasm_range.py` and byte scans of `unpacked_x2.exe`. **Verdict: PASS, no code
change, no build.** Commit `292ff32e` reviewed.

**Hygiene** (`flagcheck SERV_IRUHADEV_ELYSION_VILLAGE --range 292ff32e^..292ff32e --with SERV_IRUHADEV_ELYSION_DEBUG`):
`files: 5 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`. No OTHER rows, no deleted lines. PASS~ is the blank line before the helpers at the end of `X2TFieldGame.cpp`.

**Re-read in the binary, all equal to the port**
- **VillageParsing `324ff4b0`:**
  - `CameraModifyDistanceByPortal` misses to `[0x3344f34c]` = 3000.0.
  - In `SPA_INFO`, `Type` misses to -1 and `LineIdx` indexes from 1 (element default -1). The row's push_back (`32506720`) sits outside the `LineIdx` test, so a row without `LineIdx` is kept.
  - The `START_POS` row is `new(0x24)`:
    - bools `+0x16 / +0x17 / +0x18 / +0x19 / +0x1a / +0x22 / +0x23` miss to 0, except +0x19 (IsRight), which misses to 1;
    - `+0x10` (int) and `+0x14` (ushort) miss to 0;
    - `WaitingTime` +0x1c misses to `[0x33436700]` = 3.0;
    - `InternalPlaceStartPosID` +0x20 (ushort) misses to 0.
  - A failed map insert (`.second` false) frees the row. Every new field is written on every row (`LUA_GET_VALUE` assigns the default on a miss, `KLuaManager.h:30`), so the two structs' missing constructors do not matter.
  - The `NAME` branch is display-only. It reads the id (miss -1); if the string manager's +0x40 debug flag is set it formats the name with its id, and otherwise it uses the plain string.
  - The bool after `NAME` (templet +0x5c) is March's `IS_DUNGEON_RESULT_FIELD`.
- **Portal block `323b5970` @323b6cd2-323b6ffd:**
  - The -1 reset is inlined. EBX is -1 on both paths into `323b6cd2`, since the path from `323b6ccb` uses EBX as the -1 refcount decrement and the EH state.
  - Every constant was read from the image: gate 2000.0 (`<=`, `TEST AH,0x41 / JP`); camera `dist <= +0x358` (`TEST AH,1`), `+0x20 == 0`, village `-1500 > 1` unsigned; `5000/d + 10`, capped at 30.0; `<= 1000` gives 1000.0.
  - `FieldCamera( unit, dist, +0x360 lookat, +0x364 eye, ang, 0.4 )` matches March's 6th default (0.4).
  - Within 100.0 (`324f20a0`): `40000 <= id < 40045` (= March `VMI_BATTLE_FIELD_END` under `SERV_IRUHADEV_ELYSION_PORT`), 25000, `+0x20 != 0`, 20000-20010, 30000-30006, 1000-1008.
  - On a match the row's +0x1c goes to the inlined setter, and the loop exits (`JMP 323b7003`).
- **`MoveToOtherPlace` `3250a800`:**
  - With id -1 it zeroes the sum (+0x48c) and returns 0.
  - The timer continues only when `sum >= target`, then resets the sum to 0, which is `CheckAndResetElapsedTime`.
  - Internal place: returns `323ac300`'s AL.
  - A NULL row logs, plays the sound and returns 1.
  - Village / lounge / rest with an ACK (0x220) awaited: skips +0x22f, plays the sound.
  - Otherwise: `SetVillage( id, GetStartPosLoc( +0x14 ), 0 )`, `GetIsRight( +0x14 )` to location manager +0x98, and `SendPacket( 0x21f, {id, false}, 1, 0 )`. That matches March's defaults (`bLogging = true, bCompress = false`; `m_bUseWarpItem` ctor false). Then `AddServerPacket( 0x220, 3.0 )`.
  - The battle-field path calls `31f06350( id, +0x14 )`. Both move paths set +0x22f = 1.
  - `Portal.ogg` is played with `( 0, 0, 1 )`, which is March's `PlaySound` 4th default.
- **`323aa170` / `323ac300` / `323aa1b0`:** all equal to the port.
  - `323ac300`: the +0x3c8 write = `CX2SquareUnit::SetPos`, which only sets `m_SyncData.m_Pos`.
  - `323aa1b0`: writes state +0x488 = `m_TimerWaitingPortal.m_fTargetTime` only.
  - Ctor: `+0x2f0 = -1`, `+0x358` from templet +0x58.
- **`GetVecLinkedPos` `324f2ff0`:** as funcmap records it. The six flags were re-read.
- **Readers of the old bool:**
  - A byte scan for calls to `323aa1b0` finds four. Two are the StateField ACKs `32509400` / `32509c8c`. The other two, `31f2a64c` and `31f29fea`, are June's twins of `CX2State::Handler_EGS_JOIN_BATTLE_FIELD_ACK` and `..._CALL_MY_LOVER_JOIN_BATTLE_FIELD_NOT`: `XGM_STATE_CHANGE( 0x15 )`, then `if( g_pTFieldGame ) 323aa1b0( -1, 3.0 )`. Ghidra lists neither because no function is defined there. So all four March `SetNearPortalToBattleField( false )` sites are June's.
  - June's skill-note reader (`3292ed2a`, `X2UISkillNote.cpp:544`'s twin) tests `+0x2f0 != -1`, the same as the port's `IsNearPortalToBattleField()`.
  - The market block (`X2TFieldGame.cpp:1604`) reads the same getter.

**One comment is wrong, and the code is unchanged.** `X2LocationManager.cpp`'s `GetVecLinkedPos` comment calls 40037 "a June boss-raid field, no March enum".
Under `SERV_IRUHADEV_ELYSION_PORT` it is `VMI_BATTLE_FIELD_ELYSION_FIELD_03` (A3). The code compares the literal, so it behaves the same. Fix the comment when E2 next edits that file.

**Nothing owed.** The play-test passed 2026-10-17. Next: `/x2-phase atlas E2` (its dependencies E1c, A5 and A6 are done).

## Phase E2 — Elysion village (20009)

**Steps** — same checklist as Atlas A5, for the village:
1. Scene `Elysion_Village.lua` + `Elysion_villeage_LineMap.lua` (June's
   spelling), `WorldData` 636.
2. `DLG_Map_Village` `Village[VMI_ELYSION]`: `WORLD_ID`, `HouseIDList` (12 new
   + shared; every element resolves — nil-hole check), `START_POS` 500–517
   with March key names + the in-village warp keys (E1).
3. `DLG_Map_House`: the 12 new house blocks, Elysion rows in the 9 shared
   houses, `HI_GUILD_MARK` village row (lift Lanox's omission); village NPC
   unit rows in `NPCTemplet`; `DLG_Map_House_NPC_RelationData.lua`; the data039
   `UI_*_ELYSION` dialogs; `ShopItem.lua` `VILLIAGE_ID` block (studio typo).
4. `MapData` row (inert quest keys) + Cobo zone **18** for `VMI_ELYSION` and
   F03 (client + `ServerResource/US/MapData.lua` mirror).
5. C++ cases (June's whole functions, per A1a): `IsValidWarpField`,
   `LoadFieldImage` (0x4e29 → `HQ_LoadingVillage_Elision_01/02.tga`),
   `OpenFieldName`, `GetFieldMapDlg` + `DLG_UI_FIELD_MAP_Elysion_Village_NEW`,
   camera/navi, both warp-list implementations + `Village_20009` controls,
   `IsVillageField` already widened in A3. Chat `elysion`.
6. Restore Field 02's exit `L142 → VMI_ELYSION` pos 500 (A6 left it
   commented). Town + F03 pins on the world map (A5 ported the block).
7. Music, loading art, house/NPC meshes and motions.

**Done when:** census `--line VIL` clean; build rc=0; staged + pack list.
After pack: `elysion` warp renders the village with every house and NPC;
in-village warps work; F02 → village → F02 round trip; Cobo + warp NPC rows;
shop sells; NPC UI dialogs open.

**Run with:** Opus 5.5, high effort. Required review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch main) --
"Starting context", the ledger (both halves), Atlas "Phase A5" (the same
checklist) and "Phase E2". Also read LANOX_TROUBLESHOOTING.md's symptom table
and LANOX_DEFECT_PHASES.md Bug A (shared-house rows). Never launch
X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high.

Your task (Phase E2): port Elysion village 20009 end to end -- scene,
WorldData, DLG_Map_Village (March key names + in-village warps), the 12 houses
and shared-house rows incl. HI_GUILD_MARK, NPC relation data and UI dialogs,
ShopItem, MapData + Cobo zone 18 (client and ServerResource mirror), June's
whole C++ region functions, the "elysion" chat warp, music/art, and restore
Field 02's village exit.

Premises to verify first: every HouseIDList element resolves; June's
START_POS 500-517 semantics against March's VillageParsing.

Done when: Phase E2's "Done when" holds. Record a Done note (DEPLOY OK commit), then commit on main.
```

### Phase E2 — Done

**2026-10-17, Opus 5.5 high.** Flag `SERV_IRUHADEV_ELYSION_PORT` (C++: the `elysion` chat warp only; data blocks carry the same label).
The village engine (E1a-E1d) and June's whole region C++ (A3 / A5 / A7: `IsValidWarpField`, `LoadFieldImage` 0x4e29, `OpenFieldName`,
`GetFieldMapDlg`, `UpdateNaviMap`, `ResetCameraMatrix`, `WarpByCommand` case 18, both warp lists) were already in: E2 is the data that
turns them on. The four A7 stand-ins that wait for a 20009 templet (`X2LocationManager.cpp:603`, `X2StateMenu.cpp:6340`, `X2UIInventory.cpp:14120`,
`X2WorldMapUI.cpp:495`) now pass through to June's behaviour unchanged; none was removed.

- **Premises.** `HouseIDList` (21) resolves whole: every `HI_*` is in March's enum; `HI_STORY_QUEST` / `HI_TRANSCEND_QUEST` load with an empty
  `NPC_ID_LIST` (their NUIs are the quest plan's, census `lists.hole` 2, routed). `START_POS` 500-517 (no 515) = the E1d keys; June's
  `OtherPlaceID` / `OtherPlaceStartLineIndex` renamed to `BattleFieldId` / `BattleFieldStartLineIndex` (as A5). 500 summon (F02 exit lands here),
  501 -> F02 line 248, 502/504/506/508/510/512 -> 503/505/507/509/511/513 in-village warps (`WaitingTime 1.5`), 514 warp, 516 summon,
  **517 -> F03: its two portal keys are commented until atlas E3** (F03 has no BattleFieldData; A6's precedent).
- **Registry Lua (BOM source, ledger refreshed):** `DLG_Map_Village` Village[VMI_ELYSION] (June :991-1152); `DLG_Map_House` 11 new houses
  (June :8591-9365) + the Elysion row in 7 shared houses (CAMILLA_PVP, ARIEL_EVENT, POSTBOX, BILLBOARD, PRIVATE_BANK, GUILD_MARK, MU; the
  plan's "9" is 8 with ADAMS) + **`HI_ADAMS_HOUSE` = June's whole block** (March's had no COMMON_NPC_LOCATION, so a lone Elysion row would drop
  Adams from Ranox, the only March village listing him; Ranox's scene is June's, so June's Ranox row fits; June drops MAKING_NPC / ATTRIBUTE /
  SELL_NPC: June behaviour; March block kept commented); `NPCTemplet` 11 rows (June :28955-29406); `WorldData` WI 636 (June :150-156,
  Festival / XMAS names inert as WI 637's); `MapData` AddMapData 20009 (June :357-370) + Cobo zone 18 for 20009 and F03, and the server
  mirror (`ServerResource/US/MapData.lua`, +29 lines, LF like its neighbours); `BattleFieldData` F02 exit `L142 -> VMI_ELYSION` 500 restored
  verbatim; `DLG_UI_World_Map_NEW` town pin shown (F03's stays hidden for E3); `ShopItem.lua` converted (decompile T0, core row) + June's
  VMI_ELYSION block (June :3389-3773). June's whole ShopItem is not taken: its Ruben / Ranox / Peita rows name NUI_SHY_WISP_EVENT /
  NUI_SAYRENA_EVENT / NUI_PANE (no March enum), and a nil NPC_ID takes `AddShopItemList_LUA`'s LFail01, which deletes a list it already stored.
- **Shared houses keep March's rows for March villages.** June moved several NPCs in Ruben / Elder / Besma, but those scenes are still March's
  (`Elder_village.lua` 46,051 B vs June 10,400 B), so June's coordinates would misplace them. They follow the village scenes (asset port), not E2.
- **June files (ljconv, `june_adopt_20261017_103941.tsv`):** `Elysion_Village.lua`, `Elysion_villeage_LineMap.lua` (data040),
  `DLG_UI_FIELD_MAP_Elysion_Village_NEW.lua` (data034), 12 data039 NPC dialogs (UI_{ADAMS,ARIEL,CAMILLA,MU,RURIEL}_ELYSION, UI_NPC_ELYSION_{RED,BLUE}_TREE,
  UI_{BERNARD,DURANDA,THEODOR,HUGO}, UI_QEUST_BIRD). Assets: `HQ_LoadingVillage_Elision_01/02.tga` (data183), `Elysion_Village_Blossom1.dds`
  (data002), `Elysion_Village_POST.X` (data013). Everything else the scene, houses and NPCs name was already staged (A6 closure); music in `data\music`.
- **Census `--phase E2` 29 -> 14, all routed:** festival / XMAS scenes (seasonal, out of scope), NUI_STORY_QUEST / NUI_TRANSCEND_QUEST (quest
  plan), `LINE_NUM` (B73), `NPC_TALK_LIST0..3` (read as `"%s%d"`, `KLuaManagerProxy.cpp:84`), `BaseLocalStateID` (unread in both),
  `OtherPlace*` (renamed), `motion_npc_elsion_hugo.xet` (June ships none: JUNE_BUGS 226). `--line VIL` = the same 14.
- **Hygiene:** `flagcheck SERV_IRUHADEV_ELYSION_PORT --worktree --with SERV_IRUHADEV_CHAT_WARP_DEBUG --files X2Lib/X2ChatBox.cpp`: `files: 1 PASS, 0 PASS~,
  0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag
  token, 0 encoding change(s)`; `RESULT: PASS`. `ledger --check` PASS, `core --check` PASS.
- **Pack:** `PACK OK` data002 / 013 / 034 / 036 / 037 / 039 / 040 / 183; each diff vs live is only E2's entries. `COPY OK` all 8 (user OK): data034 md5 29266814, data036 1d65782a, data037 40de79e7, data039 84fbf888, data040 23e9aadb, data183 eefa8a32, data002 91daecb6, data013 4eaa8140.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,699,712 B, dated 2026-10-17 10:44:16, md5 23083f5020277448521cb93066309abd, equal to X2/US_SERVICE/x2.exe
  (2026-10-17 10:44:10); built at main 72491252 + 1 uncommitted code file(s).` `L"elysion"` is in the deployed exe.
- **Open, routed (no question):** March has no Ranox shop row (June's ShopItem has one; its `NUI_SAYRENA_EVENT` blocks a straight copy): a
  pre-existing Ranox-port gap, noted, not E2's. June's other shared-house changes (talk lists, EVENT2/3, JoinArea) wait for the friendship system and village scenes.
- **How to see it in game:** the hand-off below (`elysion` warp, every house / NPC, in-village warps, F02 round trip, Cobo, shop, NPC dialogs).
- **Play-test 2026-10-17 (user + log, exe 23083f50):** village loads (`VillageParsing June keys: village 20009 internalPlaceRows 6`), every NPC built;
  in-village warps 502/506/508/510/512 moved (`InternalPlace:` lines; 504 not walked); F02 round trip (501 -> 40036 line 248, back to 20009 after 5 s);
  Cobo warp Ranox -> 20009; town pin shown (only 40037 hidden); shops list, dialogs open, Ranox Adams at June's spot by the spa door (user); 0 EXCEPTION.
  **Found:** no `[Z] Use` at Luriel in Elysion: her row has no per-row join area and stands raised (y 4102), so March's house JoinArea 500 was
  out of reach. **Fix:** June's `JoinArea = 700` on `HI_PRIVATE_BANK` (June :4001; also June's value in every village), `PACK OK` + `COPY OK data034`
  md5 076e1b78 (user OK).
  **Same class, user report:** no `[Z] Use` at Camilla in Ranox (June scene, Ranox row has no per-row join area): June's `JoinArea = 500` on
  `HI_CAMILLA_PVP_HOUSE` (June :1779, March 400), `COPY OK data034` md5 a8b04f7e (user OK). A sweep of every house finds three more
  March-vs-June JoinArea differences, all in March-scene villages with no report: HI_GRAIL 600/500, HI_HOFMANN_HOUSE 500/700,
  HI_RICHANG_HOUSE 700/600 (March/June); left as is. Re-test PASSED 2026-10-17 (user): `[Z] Use` shows on both. Nothing owed but the review.

### Phase E2 — Review (2026-10-17)

**Opus 5.5 high. Verdict: PASS, no code change, no pack, no build** (the deployed exe 23083f50 and the 8 live koms stay as they are).

- **Verbatim (parsed table trees, comments / whitespace / trailing commas dropped, both sides):** Village[VMI_ELYSION] (March :4176 vs
  June :991) differs only by the two `OtherPlace*` renames on 501 and 517's two commented keys (their values are June's: F03, line 1);
  the 11 new houses, `HI_ADAMS_HOUSE` (March :151 / June :319) and the Elysion row in the 7 shared houses are identical. June has
  19 houses with a `WI_VILLAGE_ELYSION` row, and March has all 19 (none missed, none extra). JoinArea: PRIVATE_BANK 700, CAMILLA_PVP 500 = June.
  NPCTemplet 11 rows (March :20172-20258), WorldData 636, MapData 20009, Cobo zone 18 (20009 and F03, client = server mirror
  `ServerResource/US/MapData.lua:359` / `:596`), BattleFieldData F02 (whole entry; L142 -> VMI_ELYSION 500) and ShopItem's
  VMI_ELYSION block are identical to June. World map: the Elysion block = June's except the two F03 `SetShow(false)` lines (E3's).
- **Decisions re-tested, all stand.** Adams: only Ranox (`DLG_Map_Village.lua:875`) and Elysion (`:4199`) list `HI_ADAMS_HOUSE`, and
  Ranox has no March scene, so June's whole block is the only fit. Shared houses keep March's rows for March-scene villages. ShopItem:
  `AddShopItemList_LUA` inserts `pItemIDList` into `m_ShopItemList` before the `SALE_ITEM_NPC` loop. LFail01 falls through to LFail, which
  deletes that same list, so a nil `NPC_ID` (NUI_SAYRENA_EVENT etc.) leaves a dangling pointer in the map: June's whole file is still
  refused. 517 stays commented until E3.
- **Reachability:** HouseIDList 21/21 in staged `Enum.lua` and `X2LocationManager.h`. `region_census --phase E2 --live`: staged = live
  (299 files), 0 id collisions, 0 missing effect sets / items / strings, open = the Done note's 14 routed items exactly. The 15 rows of
  `june_adopt_20261017_103941.tsv` are byte-exact (sha1 + size + `1B 4C 4A 81` / `1B 4B 4C 81` header); the 4 assets are staged; the md5 of each
  live kom = its `_packed.kom` (data034 a8b04f7e, the JoinArea copy). `ledger --check` / `core --check` PASS.
- **C++:** the chat-warp hunk is the `atlas` warp's pattern with `VMI_ELYSION`. The four A7 stand-ins with a 20009 templet:
  `X2LocationManager.cpp:603` only fires on a failed `BeginTable` (idle); `X2StateMenu.cpp:6340` / `X2UIInventory.cpp:14120` add the
  Elysion row exactly as their `#else`; `X2WorldMapUI.cpp:495` lets the town pin through and still hides F03 (until E3).
- **User report, world map opens at the top-left: June does the same, kept (JUNE_BUGS 227).** June `OpenWorldMap` 322630a0
  `@322632d4` does `m_pDLGWorldMap->SetPos( (0,0) )` (`31cd2060`) on every open, as March `X2WorldMapUI.cpp:398` does. June's M handler
  (31f118c0 @31f11dc2-31f11de8: OpenWorldMap(1), UpdateWorldMap 3225d340, OpenFieldMap(1, id), UpdateFieldMap 3225d970) and the
  field-map Esc (`WMUCM_FIELD_RIGHT_CLICK`, event proc 32263750 @322637d3) match March's line for line. UpdateWorldMap 3225d340 never
  positions the dialog; it only places pins (button SetOffsetPos / SetShow) and calls UpdateNaviMap. The map is not wider: `SetSize(1800, 1200)` and the
  drag range are the same in both files. At (0,0) a 1024x768 view shows Ruben..Velder; Hamel (y 851), Sander (741, 1004), Ranox (1197, 970),
  Atlas Station (1454, 615) and Elysion (1515, 374) start off-screen in June too. The navigation thumbnail is June's way there.
- **Stale comments (no data effect):** `DLG_Map_House.lua:4382` (HI_GUILD_MARK) and `:5135` (HI_MU) still say the Elysion row is omitted.
  E3 repacks data034 anyway, so it rewords them (kickoff).
- **Hygiene:** `flagcheck SERV_IRUHADEV_ELYSION_PORT --range da93f852^..da93f852 --with SERV_IRUHADEV_CHAT_WARP_DEBUG --files X2Lib/X2ChatBox.cpp`:
  `files: 1 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched inserted, 0 #else/#endif
  without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
- **Centring the world map on the pin: declined (user, 2026-10-17: it would differ from June).** Nothing owed. Next: `/x2-phase atlas E3`.

---

## Phase E3 — Field 03 (+ Field 04 check)

Follow Atlas A6's field steps for Field 03 (`Field_Elysion_Stage2` + LineMap,
`Elysion_Field_03.lua`, WI 640, field-map `DLG_UI_FIELD_MAP_Elysion_03_NEW`,
pin, monsters, effects, drops). Confirm Field 04 (40038/641) is enum-only in
June (no row, scene or dialog) and leave it at that.

**Run with:** Sonnet 5.5, high effort; required Opus 5.5 review; escalate to
Opus on any engine gap the A1b map doesn't settle.

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch main) --
"Starting context", Atlas "Phase A6" and "Phase E3". Never
launch X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high. Stop on any engine gap the A1b map doesn't settle and log it
for an Opus pass.

Your task (Phase E3): port Field 03 the way Atlas A6 ported Fields 01/02, and
confirm Field 04 is enum-only.

Done when: census --line FLD (F03) is clean, staged with manifests, pack list
handed over. Record a Done note, then commit on
main.
```

### Phase E3 — Done

**2026-10-17, Sonnet 5.5 high.** Flag `SERV_IRUHADEV_ELYSION_PORT` (C++: one `case`; the data blocks carry the same label). Almost all data: the
region C++ for 40037 (field map, camera, navi, `LoadFieldImage`, warp lists) was A3 / A5 / A7's, and the F03 monsters, meshes and textures were
A6's region closure (`NPCTemplet` rows, unit scripts and `NewNPCStat` for all 8 F03 NUIs).

- **June files (ljconv, `june2017_re/june_adopt_20261017_111022.tsv`):** `Field_Elysion_Stage2.lua` + `_LineMap.lua` (data040), `Elysion_Field_03.lua`
  (data036), `DLG_UI_FIELD_MAP_Elysion_03_NEW.lua` (data034). All 4 new names, 0 replaced.
- **Core-list Lua (BOM source, edited with byte-exact CRLF, `luasrc.py refresh` done, `ledger --check` PASS, `core --check` PASS):**
  `BattleFieldData` (data036): June's F03 entry verbatim (:2158-2206: `Field_Elysion_Stage2_LineMap.lua`, `Elysion_Field_03.lua`, L0 -> `VMI_ELYSION`
  pos 516, `RAID_FIELD_PORTAL_POSITION_INDEX = 1`, levels 85 / 88). `WorldData` (data040): WI 640 (June :3200-3204).
  `DLG_Map_Village` (data034): START_POS 517's two keys uncommented (E2's leftover; values = June's). `DLG_UI_World_Map_NEW` (data034): the two F03
  `SetShow(false)` lines commented out (E2's leftover), so the F03 pin shows and the A7 stand-in at `X2WorldMapUI.cpp:495` passes through on its own.
  `DLG_Map_House` (data034): comment text only, one added line after each stale note (HI_GUILD_MARK, HI_MU) saying the Elysion row is back (E2 review's item).
- **C++ (`ELYSION_PORT`):** `CX2Main::FieldWarpByCommand` (`X2Main.cpp:13596`): `case 42` -> `VMI_BATTLE_FIELD_ELYSION_FIELD_03` (June table @31d71550). 43 (F04)
  stays omitted (below).
- **Cobo zone 18** already lists F03 (client `MapData.lua` and the server mirror `ServerResource/US/MapData.lua:724`): verified, unchanged.
- **Field 04 (40038 / WI 641): enum-only in June, confirmed.** The only hits in `june_src_fixed` are `Enum.src.lua` (`WI_..._FIELD_04` 641,
  `VMI_..._FIELD_04` 40038, `BFI_..._FIELD_04`): no `BattleFieldData` row, no `WorldData` row, no scene, no dialog. Left as is.
- **Census:** `region_census --region elysion --line FLD`: 0 open, and no F03 file is left in `assets.adopt` (`in_march` 2558 -> 2561, `koms.staged` 299 -> 303).
  The census scope does not list F03 rows separately; the check was the 4 files in the staged set plus the NUI readiness (8 of 8 have an `NPCTemplet`
  row and an Enum value).
- **Drops:** unchanged from A6 (backlog Stage 23, `kickoffs/june-backlog/stage23-plan.md`): the server `DropTable` stops at 40025, so F03 kills log
  `no reward row ... grants nothing`.
- **Hygiene:** `flagcheck SERV_IRUHADEV_ELYSION_PORT --worktree --files X2Lib/X2Main.cpp`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`;
  `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
  `RESULT: PASS`.
- **Pack:** `PACK OK` data034 / data036 / data040. `kom.py status` diff vs live is only E3's: data034 +`DLG_UI_FIELD_MAP_Elysion_03_NEW.lua`, ~`DLG_Map_House.lua`, ~`DLG_Map_Village.lua`,
  ~`DLG_UI_World_Map_NEW.lua`; data036 +`Elysion_Field_03.lua`, ~`BattleFieldData.lua`; data040 +`Field_Elysion_Stage2.lua`, +`Field_Elysion_Stage2_LineMap.lua`,
  ~`WorldData.lua`. **`COPY OK` (user OK):** data034 66bd7d74, data036 64bce5e6, data040 564291c5.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,699,712 B, dated 2026-10-17 11:13:06, md5 ad0dc512668182592721c21b9efd8275, equal to X2/US_SERVICE/x2.exe
  (2026-10-17 11:13:01); built at main 300a6848 + 1 uncommitted code file(s).` (`case 42` has no string literal; the build rc=0 is the proof.)
- **Play-test 2026-10-17 (log, run 11:16-11:19 on exe ad0dc512 + the 3 koms; no user report on the pin yet):** village 20009 -> `battlefield 40037 room 1001`
  -> `Elysion_Field_03.lua: 9 group(s), level 88, max 35 monster(s)`, 35 placed (UIDs 1000000..34), Herna monsters fight with June effect sets (`JUNE_FX` lines),
  kills respawn (5 / 4 / 1 / 1 ...), kills log `no reward row ... grants nothing` (expected, Stage 23); return to 20009 at 11:19:22 (`EGS_STATE_CHANGE_FIELD`,
  loaded line 119 = pos 516); 0 EXCEPTION, 2 UNHANDLED = `E_DISABLE_HB_CHECK_REQ` (pre-existing); no `PortalMovingInfoPtr is NULL`. The world-map pin hidden
  line never fired (pin shown).
  **Found:** 4 `OpenTexture` misses during the F03 load: `ELYSION_HERNA_SHIELD_WEAPON.TGA` (June ships it; the A6 closure staged only `_A`) -> staged June's
  copy (data181, 65,580 B), `PACK OK data181`, `COPY OK data181` md5 a499680e (user OK); `FIELD_ELYSION_VILLEAGE_AP_TREE02.DDS` (embedded in `Field_Elysion_Stage2_G02.y`, June lacks
  it too: June's own miss, kept), `LIRE_01_SPINNINGKICK02_AS.TGA` and `PARTICLE_BLUR_GA.TGA` (June lacks both; Lire skill particles, not F03's).
- **How to see it in game:** after the copy, the hand-off below (`elysion` warp, walk to the village's far portal at START_POS 517 -> F03 -> back via
  F03's start portal (L0) to the village; F03 pin on the world map).

### Phase E3 — Review (2026-10-17)

**Opus 5.5 high. Verdict: PASS + one missed June asset staged (data only; no code change, no build).** The deployed exe ad0dc512 stays.

- **Verbatim (comments / whitespace / CRLF / BOM stripped, scripted, both sides):** `BattleFieldData` F03 (staged :2136-2185) = June :2158-2206,
  49 lines, 0 differences (pos 516, STR_ID 40812-40814, groups 200 / 210). `WorldData` WI 640 (staged :3232-3236) = June :3200-3204.
  `Village[VMI_ELYSION]` (staged :4176-4337) = June :991-1152 except the 4 expected `OtherPlace*` -> `BattleField*` key renames (501 and 517);
  517's keys are live. `DLG_UI_World_Map_NEW`: the Elysion blocks equal June's (16 differing hunks, all March-only regions); no active
  `SetShow(false)` left. `DLG_Map_House`: pre-E3 file recovered from `kom_backup_tools/data034.kom.20261017_111539` (sha1 = the old ledger row);
  the diff is 2 added comment lines, comment-stripped text identical.
- **June files:** the 4 ljconv files match `june_adopt_20261017_111022.tsv` (size, sha1, `1B 4B 4C 81`).
- **Field 04:** every `ELYSION_FIELD_04` hit in `june_src_fixed` is `Enum.src.lua` (:2371 WI 641, :6922 VMI 40038, :18027 BFI). Enum-only, confirmed.
- **C++ `case 42`** vs June's raw bytes (`disasm_range.py`; Ghidra has no function object at 31d71130, only the table): `dec eax; cmp eax,0x2a;
  jmp [eax*4+31d71550]`; entry 42 -> 31d7150a `mov esi,0x9c65` (40037) on the plain join path (`push 1; push esi; call 31f06350`), the shape
  of 35 (0x9c63) and 38 (0x9c64). Equal. Entry 43 -> 31d71522 `0x9c66` (40038): June does send a join for F04; March keeps A6's omission (no row,
  so no world; both end in a join that loads nothing). The A7 stand-in (`X2WorldMapUI.cpp:495`) passes through: `GetBattleFieldNameByBattleFieldId`
  returns the row's name now that 40037 has one, and the run's `[ELYDBG]` hidden-pin line never fired.
- **Missed June asset (fixed):** the run logged `OpenTexture` miss `DLG_UI_COMMON_TEXTURE_FIELD_04.TGA` at 11:17:38, right after the map opened in F03.
  Reader: A5's June `UpdateNaviMap` case (`X2WorldMapUI.cpp:2463`, F03 / F04 -> `DLG_UI_Common_Texture_Field_04.tga`, key
  `FieldMap_Navigation_Elision_03`). June ships both files in data057 (TET 214 B, key `FIELDMAP_NAVIGATION_ELISION_03`; tga 1,048,620 B); the
  region census and the A11 TET closure read Lua / asset names, not C++ literals, so neither saw it. Sweep of every texture literal added to
  X2Lib / KTDXLIB since 2026-10-16 (13 names): this was the only one missing. `kom.py stage-june` (2 new, manifest 0), `PACK OK data057`
  (+2 entries vs live). `COPY OK data057` md5 e5c6c6ad (user OK).
- **Census** (`region_census --region elysion --line FLD --live`): 303 staged, `live same 303`, 0 open.
- **Play-test (log, run 11:16-11:19 on exe ad0dc512):** 20009 -> START_POS 517 (`Portal: start pos 517 -> place 40037 internal 0 wait 3.00`) ->
  battlefield 40037, 35 monsters placed, fights + respawns, 14 `no reward row` lines (expected, Stage 23); back via `MoveToOtherPlace ... 20009
  positionIndexToMove=516`; 0 EXCEPTION, 2 UNHANDLED (`E_DISABLE_HB_CHECK_REQ`, pre-existing); no `PortalMovingInfoPtr is NULL`. Other texture
  misses are pre-existing / June's own (E3 Done note) or not F03's (shop HQ items, Besma, intro). `Elysion Tower` missing in `DLG_UI_LOCAL_MAP` ->
  E4's page work.
- **Hygiene:** `flagcheck SERV_IRUHADEV_ELYSION_PORT --range 22d6655^..22d6655 --files X2Lib/X2Main.cpp`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 0 OTHER,
  0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token,
  0 encoding change(s)`; `RESULT: PASS`.
- **Cosmetic, left as is (a repack + copy for comments only):** `DLG_Map_Village.lua:4175` still says 517 is commented until E3; the two
  `DLG_Map_House` notes are labelled "atlas E2" though E3 wrote them.
- **How to see the fix:** after `COPY OK data057`, stand in F03 and open the map: the navigation panel shows F03's picture, and ErrorLog has no
  `DLG_UI_COMMON_TEXTURE_FIELD_04` line.
- **Re-test log (run 11:31-11:32 on exe ad0dc512 + data057 e5c6c6ad):** 517 -> 40037, 35 monsters, 0 EXCEPTION; no `FIELD_04` and no
  `ELYSION_HERNA_SHIELD_WEAPON` miss (data181 fix confirmed); remaining misses = the E3 Done note's June-own / non-F03 set. The run has no
  map-open line (the earlier run's `Elysion Tower <- ... DLG_UI_LOCAL_MAP` line is absent), so the navi picture rests on the user's look: **confirmed by the user 2026-10-17** (F03 picture shown). Nothing owed.

---

## Phase E4 — Local map, Elysion page

**Steps** (`SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION`, June's whole functions):
1. Page filter: port the +0x54 member and `UpdateDungeonButtons`' dummy-string
   test so only the current page's controls show (titles, buttons, stars).
2. Scroll: LMUCM 18006/18007 handlers (`FUN_329378a0(id, 1/2)`), the
   `OnFrameMove` slide (`FUN_3293a960`), `MOVE_UP/DOWN` buttons un-hidden
   (A7 hid them with a comment).
3. Guide window: open `DLG_Elysion_LocalMap_Guide.lua` from `OpenLocalMapDLG`
   for `LMI_ELYSION` as June does (first-visit logic per June), close on 18008.
4. Event flags 0x8d4/0x8d5: port the gates as June has them; if the flags
   come from a June event system March lacks, parse/store/log and leave the
   default June shows when the flag is off.
5. Elysion-page TET keys (`DLG_Localmap_Title_01`: `MAP_TITLE_ELYSION`,
   `MAP_TITLE_Elysion_SkyCrossroads`, `_Adrian`, `SOLESS`, `TITLE_MEMORY`),
   dungeon images (`Image12`), star `STR_ID`s.

**Done when:** census `--line UI` clean; build rc=0; staged + pack list.
After pack: the local map scrolls between the Atlas and Elysion pages; each
page shows only its own buttons/titles; the guide opens and closes; Elysion
buttons honour the chain.

**Run with:** Opus 5.5, high effort. Required review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch main) --
"Starting context", the ledger's "Elysion half: local map" section,
Atlas "Phase A7" and its Done note, JUNE_UI_PARITY_PHASES.md "Function map:
CX2LocalMapUI", and "Phase E4". Use the ghidra skill. Never launch
X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high.

Your task (Phase E4): port June's Elysion local-map behaviour whole behind
SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION -- page filter (+0x54), scroll
(18006/18007 + OnFrameMove), guide window (open for LMI_ELYSION, close on
18008), event flags 0x8d4/0x8d5 -- and stage the Elysion-page TET keys and
dungeon images.

Premises to verify first: the LMUCM C++ values equal the Lua values A3 added.

Done when: Phase E4's "Done when" holds. Record a Done note (DEPLOY OK commit), then commit on main.
```

### Phase E4 — Done

**2026-10-17, Opus 5.5 high.** Flag `SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION` (A7's; log lines behind `SERV_IRUHADEV_ELYSION_DEBUG`; MODS.md
"Atlas/Elysion port E4"). June rows: end of `june2017_re/funcmap.tsv` (`Atlas E4`). Commit: `cccffdb2`.

- **Premise:** LMUCM 18006 / 18007 / 18008 in the staged data036 `Enum.lua` (:7443-7445, A3) = `X2LocalMapUI.h:18-20`.
- **Scroll (June 329378a0 -> `ScrollLocalMap`):** only 0x2719; +0x48 = id; `Elysion_Local` picture 0 -> A4's `StartChangeUV( dir, from, to )`;
  on success `HideLocalMapPageControls` (329366a0: dummy string "0".."9" -> `SetShowEnable(0,0)`, plus every hero-recruit control), the end
  callback `OnLocalMapScrollEnd` (3293c1e0: shows the controls whose dummy string = the new page, then `UpdateDungeonButtons`,
  `UpdateHeroRecruitButton`), and +0x54 = `to` at the start. Callers as June: 18006 (1) / 18007 (2) with the button's dummy int; the
  `UpdateDungeonButtons` tail @3293c146 (page ? 1 : 2, a re-sync that the list's end refuses) replaces A7's hidden buttons; `ClearLocalMapDLGs`
  (329379d0) scrolls back (2) before deleting and ends with `m_bFirstPage = true` (+0x60, June's whole function).
- **Blink (3293a960 -> `OnFrameMove`):** the move button under the mouse (+0xc1 = `m_bMouseOver`, proven by the dialog's OnMouseMove
  @31cd2457) is solid; otherwise both blink, alpha 0.3..1.0 at 1/s (3345993c = 0.3f). June's tail `3293a2e0` is the LMI_SPECIAL ERDA timer
  (June-only event content): not ported.
- **Guide (329386b0 -> `OpenElysionLocalMapGuide`, from `OpenLocalMapDLG` for 0x2719):** `fopen("checkfile.txt","rb")`; missing -> new
  `DLG_Elysion_LocalMap_Guide.lua` (stage, 0.07f, layer 0) + `AddDlg`, then create the file. 18008 deletes it (`31be4210(0x472)`: all 453
  callers pass 0x472 = June's XGM_DELETE_DIALOG; `CheckDlg` stands in for the handle test); the dtor (32937e10) deletes it too.
  JUNE_BUGS 229 (once per install).
- **Code events 0x8d4 / 0x8d5 (3293b5e0 @3293b8c0 and @3293baac -> `HideDungeonByElysionCodeEvent`):** `IsEnableCode(2260)` closes dungeons
  33330-33352, else `IsEnableCode(2261)` closes 33350-33352: button, stars and quest desc off, and the Elysion title set's pictures 0/1 (2260)
  and 2 hidden. A7's comment had the sense backwards (hidden while the code is ON). Code events never fire offline: inert.
- **Assets:** June's `DLG_Elysion_LocalMap_Guide.lua` adopted (ljconv bytecode, data034, `june_adopt_20261017_115410.tsv`). Every texture /
  TET key the four Elysion local-map luas use is staged (`DLG_Localmap_Title_01` 9 keys, `DLG_UI_Image_01` `BT_LOCALMAP_UP/DOWN` +
  `LocalMap_MovePopup`, `DLG_UI_Button03_A`, `Button00/04/05/06_NEW`, `Region_ELISION`, HeroMode filter); `Image12` is in data151 (A-phase).
  No texture literal was added to C++, so no `stage-june --dry-run` list. `PACK OK data034` (vs live: +1 `DLG_Elysion_LocalMap_Guide.lua`).
- **`Elysion Tower <- control not found` (E3 review):** June's own miss. June's Icon lua has star statics only for the Elysion-page dungeons
  (`STR_ID_40226/40227/42846/43399`); AD / AC / Tower have none, so `UpdateDungeonClearStars` misses their names: JUNE_BUGS 228.
- **Census `--phase E4`:** guide row cleared; local-map rows left = `dlg_localmap_title_01_a.tga` (JUNE_BUGS 206) and the 3 Hero members (out of
  scope). The other UI rows are E7 / E10 / E11 / backlog events (Soless gauges, XMAS / Festival, `Movie_Elysion_Clear`, absent June sounds).
- **Hygiene:** flagcheck `--worktree --with ELYSION_DEBUG --files X2LocalMapUI.cpp X2LocalMapUI.h Always.h`: `files: 2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`;
  `19 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
  `RESULT: PASS`. The 19 deleted lines are A7's own flagged lines: two stale "until atlas E4" comments and A7's 17-line hidden-buttons tail.
  After commit, `--range cccffdb2^..cccffdb2` (same files): `files: 2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, same in-flag line, `RESULT: PASS`.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,705,856 B, dated 2026-10-17 12:04:18, md5 4294a0c35e2165e363ed15b65d4c7871, equal to X2/US_SERVICE/x2.exe (2026-10-17 12:04:13); built at main 369d7a1b + 3 uncommitted code file(s).` (E4's 3 files). Proof: the UTF-16LE literals `[ELYDBG] local map Elysion: scroll direction`, `[ELYDBG] local map Elysion guide: checkfile.txt absent`, `DLG_Elysion_LocalMap_Guide.lua` are in the deployed exe; A7's `move buttons %s/%s hidden` is gone. `COPY OK data034` md5 cdb289c0 (user OK).
- **Expected until E8-E11:** page 1's dungeon buttons stay hidden (no DungeonData for SkyCrossroads / Adrian / Soless / Memory), but its
  title plates and the four star strips (STAR4-7, all pictures as the lua sets them) show. June never shows that state; it ends when E8-E11 add
  the DungeonData. User (2026-10-17): leave it, no stand-in.
- **How to see it:** M on reyaa (Grand Archer, 85) -> right arrow -> Elysion tab: guide pops up (first time), Up button blinks; Up slides the
  map to the Elysion page, Down slides back. Log: `[ELYDBG] local map Elysion guide: ...`, `scroll direction`, `scroll ended, page`.
- **Play-test 2026-10-17 (user; run 12:07-12:09 on exe 4294a0c3 + data034 cdb289c0): PASSED by log.** Guide `checkfile.txt absent, guide opened`
  12:08:15, `closed (LMUCM_CLOSE_GUIDE_WINDOW)` 12:08:18, five later opens `present, guide not shown`; six round trips (`StartChangeUV 0 -> 1` /
  `1 -> 0` in 0.7 s); page 1 shows 11 controls (title, 4 buttons, title set, STAR4-7, Down), page 0 shows 6 (title, AD / AC / Tower, title set, Up);
  0 EXCEPTION, UNHANDLED only the two known `E_DISABLE_HB_CHECK_REQ`, no miss on the guide / move buttons / `Elysion_Local`. The blink (no log line)
  confirmed by the user: works. Nothing owed but the review.

### Phase E4 — Review (2026-10-17)

**Opus 5.5 high. PASS with one transcription fix** (same flag `SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION`). Read in Ghidra (read-only)
plus the raw bytes of `unpacked_x2.exe` for every string / float constant.

- **Scroll 329378a0:** pushes @3293795d-32937965 = (`*dir`, &local `[orig+8]` = from, &arg-1 slot `[orig+0x1c]` = to), both preset -1 ->
  A4's `StartChangeUV( dir, from, to )`; +0x48 = 0x2719 right after the id test; success: 329366a0, end func = 3293c1e0 bound to ESI
  (`this`), 31c95a60, then `+0x54 = [orig+0x1c]` (to). Order and operands = the port. Tail @3293c146: `NEG / SBB / ADD 2` on +0x54 ->
  page ? 1 : 2, with the loop's map id (EBP) = port. Handlers 18006 / 18007 (3293d81d / 3293d84e): `GetDummyInt( 0 )` (31c947d0), 1 / 2 = port.
- **329366a0 / 3293c1e0:** strings at 33458f54 .. 3348ac4c read from the exe = "0".."9"; vslot 0x78 with (EBP=0, 0) on a match and on every
  control of +0x34 (hero recruit; ctor and dtor agree); end callback: `_itow_s( page, 10 )`, (1, 1), the 0..10 test formats 334c83f8 and
  falls through, then `UpdateDungeonButtons( [EDI+0x48] )`, `32939530` (UpdateHeroRecruitButton). = port.
- **OnFrameMove 3293a960:** Up +0xc1 -> Up solid only; else Down +0xc1 -> Down solid only; else blink both via vslot 0x84; `1.0 <` up-test,
  `< 3345993c` (= 0.3f from the exe) down-test. Ctor 32937fa0: +0x4c = `FLDZ`, +0x48 = 0, +0x50 = 1, +0x54 = 0, +0x60 = 1 = port's inits.
- **Guide 329386b0:** `fopen( 334c8154 "checkfile.txt", 334552c0 "rb" )`; new dialog (0x204 B) with ( +0x10 stage, 334c8114
  `DLG_Elysion_LocalMap_Guide.lua`, 3344b78c = 0.07f, 0 ) -> +0x58 -> AddDlg 31d04390; then "wb" (3346aadc), no fclose on failure = port.
  ClearLocalMapDLGs 329379d0: 0x2719 -> 329378a0( id, 2 ) before the delete, +0x28 = 0, +0x60 = 1 = port.
- **Code-event gates:** both blocks: 2260 on -> only `dungeon - 0x8232 <= 0x16` (2261 is not tested), else 2261 -> `- 0x8246 <= 2`; title
  static 334c83ac = `엘리시온_타이틀 모음` (= the port's `\x` escapes); pictures 0 / 1 only under 2260, 2 always (`+8 = 0`). Not-enabled block
  (@3293b8c0): every path ends at 3293bfd4 (the loop's `inc`), = the port's `continue`.
  **FIX:** the enabled block (@3293baac) skips the dungeon (`jmp 3293bfd4` @3293bbcf) only when the title static exists; with no static
  (@3293bb7f `je 3293bbd4` -> `jmp 3293bbee`) June goes on with the open-dungeon code. The port returned true either way. Now
  `HideDungeonByElysionCodeEvent` returns false (with an `[ELYDBG] ... no title static (June goes on)` line) when the static is missing.
  Inert offline (code events never fire, and the static is in the Icon lua), so no play-test.
- **Data:** staged `data034_kom/DLG_Elysion_LocalMap_Guide.lua` 1466 B, sha1 7be757dd.. = the adopt tsv's `ljconv(June)`, header
  `1B 4B 4C 81`. Its textures: `DLG_UI_Image_01` (`LocalMap_MovePopup`, data215) and `DLG_UI_Button03_A` (`CHECK_NORMAL / OVER`, datai020),
  keys present in the TETs; `STR_ID_40385` defined (`StringID_def` :40387). Census `--phase E4` local-map rows: `dlg_localmap_title_01_a.tga`
  (206) + `DI_ATLASCITY / ATLASDEVELOPMENT / TOWER_HERO_NORMAL` only, as the Done note says.
- **JUNE_BUGS 228 / 229:** right as written (Icon lua star statics only for the 4 Elysion-page dungeons; guide gate = one generic file).
- **Play-test (log):** the guide opened once (12:08:15) and closed (12:08:18); `present, guide not shown` x6 (the Done note says five);
  scroll 6 x dir 1 / 5 x dir 2; page-1 ends 11 controls, page-0 ends 6; 0 EXCEPTION. Blink confirmed by the user.
- **Hygiene:** E4 commit `--range cccffdb2^..cccffdb2 --with ELYSION_DEBUG` (3 files): `files: 2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`,
  `19 deleted line(s)` (A7's own flagged lines: two "until atlas E4" comments + the 17-line hidden-buttons tail), `RESULT: PASS`.
  Fix `--worktree --files X2LocalMapUI.cpp`: `files: 1 PASS ... 0 FAIL`, `1 deleted line(s)` (E4's own "Returns true" comment line), `RESULT: PASS`.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,705,856 B, dated 2026-10-17 12:19:58, md5 794e68d7ca3ab86b4d1339849b5ac71f, equal to X2/US_SERVICE/x2.exe (2026-10-17 12:19:50); built at main 8b2d57a7 + 1 uncommitted code file(s).`
  The fix's UTF-16LE literal `no title static (June goes on)` is in the deployed exe. No kom change. Nothing owed. Next: E5 (kickoff
  `kickoffs/atlas/E5.md` already exists).

---

## Phase E5 — Moving platforms

**Steps** (`SERV_IRUHADEV_JUNE_MOVABLE_LINE`, June addresses from A1b):
1. C++ `LINE_MOVE_TYPE` enum with explicit values equal to A3's Lua table.
2. Line-map parser: June's movable-line data and keys (`DIRECT_MOVE_DATA`,
   `TARGET_MESH_NAME`, `FOLLOWER_LINE_ID`, `STOP`, `USE_ABSOLUTE_COORDINATE`,
   `CAMERA_LAND_HEIGHT`, `CANDOWN`) in `KTDGLineMap` (and its byte-identical
   server copy `XSLLineMap.h` only if June's header changed there too — check).
3. Runtime: per-frame line movement (spin/patrol), units standing on a
   moving line move with it, the attached mesh follows (`TARGET_MESH_NAME`),
   followers follow.
4. `AddMovableLineData_LUA`, `TAT_LINE_CHANGE_MOVABLE_DATA` trigger case
   (value per A1b, inside the June-renumbered trigger switch),
   `GetLineTypeByLineID_LUA`.
5. P2P sync: in-match state is peer-to-peer — check how June syncs platform
   phase between host and guests (or whether it's time-deterministic) and
   port that, not a client-local guess.

**Done when:** census reports no movable-line item open; build rc=0; a
`[ELYDBG] movable line …` first-use line in the exe; the Adrian/Soless
linemaps parse without `[ELYDBG] unsupported` lines.

**Run with:** Opus 5.5, high effort. Required review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch main) --
"Starting context", the ledger's "Elysion half: engine gaps" section, the A1b engine map
rows owned by E5, and "Phase E5". Use the ghidra skill. Never launch
X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high.

Your task (Phase E5): port June's moving-platform system whole behind
SERV_IRUHADEV_JUNE_MOVABLE_LINE -- LINE_MOVE_TYPE enum (values equal to the
Lua table), linemap keys and parser, per-frame movement incl. carried units
and attached meshes, AddMovableLineData_LUA, TAT_LINE_CHANGE_MOVABLE_DATA,
GetLineTypeByLineID_LUA, and June's P2P sync for it.

Done when: Phase E5's "Done when" holds. Record a Done note, then commit on main.
```

### Phase E5 — Done

**2026-10-17, Opus 5.5 high.** Flag `SERV_IRUHADEV_JUNE_MOVABLE_LINE` (log line behind `SERV_IRUHADEV_ELYSION_DEBUG`). June rows: `june2017_re/funcmap.tsv`
`(E5)` (31 rows, every address below). Commit: ``15c08485``. No Lua, no pack: A3 already put `LINE_MOVE_TYPE` and `TAT_LINE_CHANGE_MOVABLE_DATA = 8` in data036 `Enum.lua`.

- **Line map (`KTDGLineMap.h/.cpp`):** June's 0xb0 record `MovableLineData` (ctor 31c7f2b0, Reset 31c7f170) in a `map<line, record>` (+0x2d4);
  `AddMovableLineData_LUA` 31c75470 whole (keys, defaults, silent vs logged misses, the empty-name test, follower copies with both names emptied
  by 31c7e560, map insert = no overwrite; June's errors go to ErrorLog as `[MOVABLE_LINE]` in ASCII); `OnFrameMove` 31c70580 -> movers 31c6fbb0
  (patrol rows with the overshoot dropped; spin = 360 / seconds * sign deg/s about the axis 31c751b0 builds) and the moving-line vertex refresh
  (`UpdateLineVB( true )` also redraws a `+0x155` line, 31c5bf70 both loops); `MoveLine` 31c6b1f0, `MoveLineCenterTo` 31c6b0b0, stop 31c6fb50,
  new rows 31c75410, lookups 31c7dd00 / 31c74fe0 / 31c76a00, guest sync 31c6ffc0; `CAMERA_LAND_HEIGHT` (+0x158, -1) and its getter 31c6b580;
  `CanDown` head 31c6c310: a `CAN_DOWN = FALSE` line carries the unit by its record's last move, and (June order) that test now runs before the
  radius drop, for every map. Trigger case 8 parse (@31c79c4d: STOP into `m_bEnable`, rows). The D3DX calls (RotationAxis / RotationY /
  Vec3TransformCoord) were named from June's `d3dx9_43.dll` exports. 31c6f9b0 has no caller: not ported.
- **World (`X2World`):** June ticks the line map from `CX2World::OnFrameMove` (32802140), which March never did; per world object, shown or not,
  skin meshes follow (32801ab0: LMT_TARGET_MESH lines go to a motion-mesh bone, then the mesh follows its PATROL/SPIN line) and static meshes
  (327fe640, no STOP test); `DoTriggerLine` case 8 (328004a0); `GetWorldObjectSkinMeshByName` (327fca00). Matching is on the mesh file name
  (+0x1f8 = `m_XMeshName` / `m_XSkinMeshName`).
- **P2P sync (June's own):** the host broadcasts message 0x1c (28, `XPT_MOVABLE_LINE_SYNC`, 20 bytes) once a second per record (328018c0); a
  guest snaps its phase (32801fb0 -> 31c6ffc0) and re-runs the mesh follow. `CX2Game` case 0x1c (@322edf99) with a `C_ASSERT` on 28 / 0x14.
  Offline there are no guests: the host path runs and sends to nobody.
- **Game / NPC:** `GetLineTypeByLineID_LUA` 322d0800 (bound on CX2Game), the camera land height per line 322cfe20 at the end of `OnFrameMove`
  (after E6's `ProcessDramaticMessage`, June order), the NPC hit stop on a moving line (3277dfd0: `m_fAnimWaitTime = 0.1`, full `DoFrameMove`).
  New flagged accessor `CX2WorldObjectSkinMesh::GetJuneXSkinAnim` (March's `GetXSkinAnim` is tool-only).
- **JUNE_BUGS 230-233:** stale last move after a stop / end (static mesh and CanDown keep using it; E10's play-test looks at Soless Stage0
  line 34), the guest sync faults (endless row walk at 255, extra follow, unset type-1 bytes), Soless Stage3's `CANDOWN` typo, LMT_TARGET_MESH on
  the bind-pose frame.
- **Data audit (script, june_src_fixed):** the 14 `AddMovableLineData_LUA` records and 5 trigger-case-8 entries in Adrian Stage2 and Soless
  Stage0 / 1 / 1_Hide (and 3 `CAMERA_LAND_HEIGHT` lines) use only keys the port reads; those line maps load in E9 / E10.
- **Census `--phase E5`:** no movable-line row left (`CANDOWN` = JUNE_BUGS 232; `SetPosion` is E7's). `region_census.py`'s bind calibration
  flipped: `AddMovableLineData_LUA` is now a bound control, a sentinel the unbound one.
- **Hygiene:** flagcheck `--worktree --with ELYSION_DEBUG --files` (the 12 code files): `files: 4 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 7 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (the OTHER rows are E6 / E7's uncommitted hunks in the shared files; the commit holds HEAD + E5's hunks only, built by re-applying the bytepatch specs to HEAD copies). Post-commit range: `--range 15c08485^..15c08485 --with ELYSION_DEBUG`: `files: 10 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 8 EOL-mismatched inserted line(s), 1 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The 8 EOL marks are X2World.h's declaration block: LF lines between LF neighbours in a mixed-EOL header (checked byte by byte); the token mark is X2Main.cpp, whose three inserted lines carry the flag on both `#ifdef` and `#endif` (the tool's diff context, not a bare `#endif`).
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,758,592 B, dated 2026-10-17 13:41:13, md5 e6b48a065ee977f3ed681de722b4dfdd, equal to X2/US_SERVICE/x2.exe (2026-10-17 13:41:06); built at main a67e3869 + 33 uncommitted code file(s).` (the link came from E7's 13:41 tree build, which held all 12 E5 files; mine found nothing to relink). The exe holds `[ELYDBG] movable line `, `[MOVABLE_LINE] `, `LMT_SPIN DIRECT_MOVE_DATA is 0!!!`, `CAMERA_LAND_HEIGHT`, `GetLineTypeByLineID_LUA` and `AddMovableLineData_LUA` (3x).
- **How to see it:** nothing new on a March map: platforms appear with E9 (Adrian) / E10 (Soless), where `[ELYDBG] movable line <id> type ...`
  lines go to ErrorLog.txt at stage load. Regression to watch now: on a March map, standing on a `CAN_DOWN = FALSE` line and dashing / jumping
  down; the camera floor in any dungeon.
- **Play-test 2026-10-17 (user: all normal; run 13:48-13:56 on exe e6b48a06): regression PASSED.** 5 dungeon stage loads and 3 battle-field
  joins; 0 EXCEPTION, UNHANDLED only the known `E_DISABLE_HB_CHECK_REQ` x3, no `[MOVABLE_LINE]` line (no March map calls the new key), clean exit.
  Platforms themselves come with E9 / E10. Nothing owed but the review.

### Phase E5 — Review (2026-10-17)

**Opus 5.5 high. PASS, no code change** (flag `SERV_IRUHADEV_JUNE_MOVABLE_LINE`). Read in Ghidra (read-only), raw disassembly per
funcmap `(E5)` row, `disasm_range.py` where Ghidra has no function (322ed9c0, 31c77e20), float constants read from `unpacked_x2.exe`.

- **Record / Reset / ctor (31c7f2b0, 31c7f170):** word +4 = 0x0001 (LOOP 1, STOP 0), +6 IS_RIGHT 1, +0x60 / +0x94 = 0, +0x98 / +0xa4 = 0, then
  Reset: both row vectors cleared, +0x7c = 0, +0x80 = 0, +0x64..+0x78 = 0 (moved **and** last move). = port's struct, offsets and `Reset()`.
- **AddMovableLineData_LUA 31c75470:** LINE_ID (uint `<` count, else 33449500), FOLLOWER_LINE_ID (each `<` count, else the same message,
  table left open = port), MOVE_TYPE (missing or not 1-3 = silent return), LOOP def 1, STOP def 0. Type 1: TARGET_MESH_NAME / TARGET_BONE_NAME
  missing = silent; d460 / cfb0 branches give "error iff exactly one is empty" (symmetric, either way round) -> 33449310. Type 2: mesh (def ""),
  USE_ABSOLUTE_COORDINATE (def 0), DIRECT_MOVE_DATA required (33449400); type 3 adds IS_RIGHT (def 1) before the rows (33449470); rows: the
  4-float row is zeroed once before the loop, a missing field = 0. Then 31c751b0, +0x155 = 1, centre -> +0x98, insert, 31c7e560, followers
  inserted with +0x155 = 1. = port, key order included.
- **Movers 31c6fbb0** (args `double fTime, float dt`): patrol `w > time` (`TEST AH,0x41 / JP`), partial `row / w * dt`, end = 31c7ba10 (row -
  moved, 0 past the end), LOOP false -> `DEC / NEG / SBB` = UINT_MAX, time / moved = 0. Spin: `360.0 / w * axis.w * dt` stored as float
  @31c6fd2f, `* 0.01745329238474369` stored as float @31c6fd3d; 33086a92 = RotationAxis( mat, axis, angle ), 33086a3e = TransformCoord(
  out, relative, mat ); move = turned - relative; +0x64 = the new centre. 31c751b0: yaw = 31c19930(centre - pivot).y (March GetDirVecToDegree,
  same shape), `|y - 0.0| > 1.1920929e-07`, RotationY( yaw * deg2rad ), TransformCoord( axis, (0,0,1), mat ), w = 1 / -1. Constants from the
  exe: 360.0, 0.01745329238474369, 0.0, FLT_EPSILON, -1.0, 1.0, 0.1f. All = port.
- **CanDown head 31c6c310:** the carry test (@31c6c31c-31c6c3e5) returns before the `bDownRadius` drop (@31c6c3e8). The head is only reached
  with bOnLine 1, which June's PhysicProcess never passes (user @32019f76 pushes 0; NPC @32781e95 / @32782ca9 push 0): it runs from the
  down-press sites (March's 4-arg calls, bOnLine default true), whose land position is used only on true. So no June path compensates, and the
  June order changes only the out position on a refused down press. The Done note's "carries the unit every frame" overstates it (JUNE_BUGS 230
  reworded). June's CanDown also has two extra args (+0x20, +0x30): funcmap row 157, not E5's.
- **World:** 32802140 ticks the line map (vslot 1) after the fades, then per object of +0xe8, shown or not, while +0x2dc != 0: type 2 ->
  32801ab0, type 1 -> 327fe640; 328018c0 after the layer loop. 32801ab0 / 327fe640 / 327f69f0 / 327fca00 / 328018c0 / 32801fb0 = port, incl.
  +0x218 anim / +0x220 motion mesh (March member order), `combineMatrix._41` = frame +0xc0 (D3DXFRAME 0x50 + baseMatrix 0x40 + 0x30), host
  test (+0x2c0/+0x2c4 non-zero and = +0x2b8/+0x2bc), 1.0 s timer, 0x14-byte packet {word line, type, index, moved, time}, receive filter 2-3.
  March's line map OnFrameMove is empty outside the tools, so the new call adds only June's body.
- **Game / NPC:** case 0x1c (@322edf99: size 0x14, world vslot 0x90, else 322ee05e = false); 322d0800 (+4, -1); 322cfe20 (`CMP -1 / JLE`, camera
  +0x3b0 +0x3c, line map +0x1b4; every March camera land write is the line map's height, so nothing is overwritten). NPC @3277e791: byte
  +0x8470 = `syncData.lastTouchLineIndex` (UCHAR), +0x155 == 1 -> +0x8ae4 = 0.1f; +0x8ae4 = `m_fAnimWaitTime` (CommonFrameMove 327b53b0
  @327b61f3-327b62bb is March's wait / elapsed block); `X2OPTIMIZE_NPC_NONHOST_SIMULATION` is on (KTDX.h:281). The port's world / camera NULL
  tests are additions where June would crash.
- **JUNE_BUGS 230-233:** 231-233 right as written. 230 is June's code (the only `+0x70` reset is Reset, on a trigger's new rows), so a June bug,
  not a port gap; row reworded and given its E10 check (below).
- **Data audit (re-run, subagent):** 14 records (Adrian Stage2 5, Soless Stage0 1, Stage1 4, Stage1_Hide 4), 5 case-8 triggers, 3
  `CAMERA_LAND_HEIGHT` lines; every key read; MOVE_TYPE only 2 / 3; Stage3 has no movable data, only the `CANDOWN` typo (232).
- **Hygiene:** `--range 15c08485^..15c08485 --with ELYSION_DEBUG`: `files: 10 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags),
  0 FAIL, 0 ERROR`, `RESULT: PASS` (same as the Done note). Only later commit touching the flag: E7 `0cc43f29`, context lines only. No build:
  nothing changed in code.
- **E10's play-test must look at** (Soless Stage0, trigger 3 starts line 34): (1) the X02 skin mesh after the 7 s ride ends: June's code keeps
  moving it by the last frame's remainder; (2) whether a unit standing on lines 34-39 rides along at all: June has no carry outside CanDown,
  so riding depends on PhysicProcess's ordinary line snap; if the player is left behind, trace June 32015b30's line follow before calling it a gap.

---

## Phase E6 — SimpleDLG gauges + dramatic messages

**Steps**
1. **SimpleDLG** (`SERV_IRUHADEV_JUNE_SIMPLE_DLG`): port June's subsystem —
   create/get/delete a script-owned dialog, show/hide, number statics
   (`SetNumberInNumberStatic_LUA`, `_MoreDigit`, `_MoreParam`), gauge statics
   (`SetNumberInGageStatic_LUA`, `SetPercentValueInGageStatic_LUA`), lifetime
   tied to the game/stage as June does. Stage the three
   `DLG_UI_ELYSION_SOLESS_*_GAGE.lua` dialogs + their TET atlases. Check
   whether Lanox's deferred SimpleDLG uses (`RANOX_PORT_PLAN.md:2989-3004`,
   Secret Hell gauge) now work, and note it.
2. **Dramatic messages** (`SERV_IRUHADEV_JUNE_DRAMATIC_MESSAGE`): C++
   `DRAMATIC_MESSAGE_TYPE` enum (explicit values = Lua), unit-script keys
   `DRAMATIC_MESSAGE0-4` + sub-keys, and the display (image, back colour,
   text colour, sound, row length, image offset, left/right).

**Done when:** census reports no SimpleDLG/dramatic item open; build rc=0;
first-use `[ELYDBG]` literals in the exe; gauge dialogs and atlases staged.

**Run with:** Opus 5.5, high effort. Required review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch main) --
"Starting context", the ledger's "Elysion half: engine gaps" section, the A1b rows owned
by E6, and "Phase E6". Use the ghidra skill. Never launch X2_offline.exe
without asking. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase E6): port June's SimpleDLG subsystem (behind
SERV_IRUHADEV_JUNE_SIMPLE_DLG) and dramatic-message system (behind
SERV_IRUHADEV_JUNE_DRAMATIC_MESSAGE) whole, with June addresses cited; stage
the three Soless/Memory gauge dialogs and their TET atlases; note whether
Lanox's deferred SimpleDLG uses now work.

Done when: Phase E6's "Done when" holds. Record a Done note, then commit on main.
```

### Phase E6 — Done

**2026-10-17 (Opus 5.5).** Flag `SERV_IRUHADEV_JUNE_DRAMATIC_MESSAGE` (Always.h; debug lines `SERV_IRUHADEV_ELYSION_DEBUG`, `[ELYDBG] dramatic message:`).
RE notes `june2017_re/e6_dramatic_re.md`; 17 funcmap rows `Atlas E6`; JUNE_BUGS 237-238.

**1. SimpleDLG: nothing to port.** june-effect C3z-4s already ported it whole (`SERV_IRUHADEV_JUNE_SIMPLE_DLG`, play-test PASSED 2026-10-15).
Re-checked against June: Create 322def10 / Delete 322cc520 / Get 31e4de40, the +0xba8 lifetime (ctor @32307743, dtor @32308b85, no
stage-change delete in June either), SetIsSkillUITypeA's re-place (32420d30), and the 12 class methods 326b1320..326b24a0. No June behaviour
is missing. Staged the three gauge dialogs (June bytecode via `adopt-june`): `DLG_UI_ELYSION_SOLESS_{AWAKEN,HERBAON,OF_SUN}_GAGE.lua` into
data034, `PACK OK data034` (+3, nothing else changed). Their textures and TETs (`DLG_UI_Image_02/04/05`, `DLG_UI_Common_Texture93_NEW`) were
already staged (A11, `atlas_a11_tet.tsv`). Lanox's deferred SimpleDLG item (`RANOX_PORT_PLAN.md` 3d, Secret Hell fever gauge) works since C3z-4s.
Two June quirks are unchanged and need no row: nothing loads AWAKEN_GAGE, and `ELYSION_M_SOLESS` drives AWAKEN statics on the OF_SUN dialog,
so June's NULL lookup no-ops.

**2. Dramatic messages, ported whole:**
- Game side (X2Game):
  - `LoadDramaticMessage` 322e9e00 is called from `CX2GUNPC::CommonStateStart` @327af18c, right after the alarm block. It sets
    `m_bHaveDramaticMessage` (NPC +0x9060).
  - `RemoveDramaticMessage( uid, state )` 322e61c0 runs from CommonStateEnd @3277f9f7; `RemoveDramaticMessage( uid )` 322e6270 from the NPC dtor.
  - The player `ProcessDramaticMessage` 32306110 is the last call of OnFrameMove. Lua `Playing_Dramatic_Message_LUA` 322d5bb0 is bound.
  - The C++ `DRAMATIC_MESSAGE_TYPE` is { DMT_LEFT 0, DMT_RIGHT 1, DMT_END 2 }, equal to the Lua table.
  - The row struct is June's 0xb0 layout, with every default read from the exe: SHOW_TIME 3.0, IMAGE_OFFSET (50, 0), default spots
    (430, 170) / (570, 170), MAX_ROW_CHAR 16, spread gap 0.1.
- Talk box side:
  - A second manager, `TBMT_DRAMATIC_MESSAGE` (3) → `CX2TalkBoxManagerImpDramaticMessage` (vtable 3353c620), is created and deleted next
    to the default one. It ticks after it, and draws right after FrontUIRender (@3230ad36), so it stays visible over a fade.
  - The subclass carries June's overrides: Push 32f218c0, SetBalloonTip 32f21380 and LineBreak 32f21600. Push, SetBalloonTip and the
    `Update( …, manager )` signature become virtual / extended under the flag, matching June's vslots 4 and 7.
  - Base additions June made: TalkBox fields (+0xdc..+0x124, ctor 323151a0); the portrait `DrawImage` 32e92cf0 (UI major particle
    `DramaticMessageImage_Left` / `_Right`, already in the staged UIMajorParticle.txt); its destroy at every removal site (dtor, OnFrameMove ×2,
    Push, Delete, DeleteByIndex, Clear); Update's slide-up 32e945c0 with `SetMove` 32e90a30; and the per-box spread fields in InitTalkBox
    (defaults true / 0.1 = March's constants, so ordinary speech bubbles are unchanged).
- Assets: all 41 portrait / sound names the June rows use are already staged, identical to June (stage-june dry run: 0 new, 0 replaced).
- June bugs kept: #237 (a played REPEAT = FALSE row skips its EndTable; latent, no June script sets REPEAT) and #238 (the portrait stays at
  the fixed spot when a box stacks or slides).
- Ours, not June's: NULL guards on g_pX2Game / g_pData / the particle system / GetMyUnit, and a range check on TYPE (June indexes its two
  vectors unchecked).

**Decision taken (review to confirm):**
- June's US client breaks dramatic lines with the global line breaker 31d436c0 (`CLIENT_GLOBAL_LINEBREAK`). That breaker ignores
  MAX_ROW_CHAR and wraps at m_ResSize ≈ 143 px × scale.
- March's US build has no global breaker. The dramatic Push therefore runs June's own non-global LineBreak override 32f21600: row width =
  MAX_ROW_CHAR × the width of U+AC00, max 200 chars. So boxes may be wider than June US's.
- The alternative would be porting the global breaker; that is an engine-wide talk-box change, outside E6.

**Hygiene:**
- flagcheck `--worktree --files` (10 files): `files: 5 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 4 OTHER (other flags), 0 FAIL, 0 ERROR`;
  `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s)`; `RESULT: PASS`.
- The OTHER rows are E7's uncommitted `SERV_IRUHADEV_ELYSION_SCRIPT_B` hunks in the same files.
- Census `--region elysion`: no SimpleDLG, gauge or `DRAMATIC_MESSAGE` key item open. What is left is `CT_ELYSION_SOLESS_DRAMATIC_*` state
  names and the `NUI_ELYSION_SOLESS_DRAMATIC` NPCTemplet row, both E10's.

**Build:** E6's files compile clean (two runs). Both tree builds failed in E7's uncommitted code:
- run 1, X2Lib: `X2World.cpp` (C2039 `GetXSkinAnim`, C3861 `ConvertWCHARToUtf8`);
- run 2, X2: C2248 protected `CX2GUUser::Get/SetShowHpSmallGage` (`X2GageUI.cpp:549`, `X2Main.cpp:4831`).

Nothing was deployed with this commit. A peer's deploy at 13:41 (exe md5 e6b48a06, built on 761ce721 + E5 15c08485) carries E6: the
`[ELYDBG] dramatic message: show npc` and `DramaticMessageImage_Left` / `_Right` literals are in `X2_offline.exe`. JUNE_BUGS rows renumbered
230-231 -> 237-238 (E5 committed its own 230-233 after E6).

**How to see it in game:** not yet. Every NPC with `DRAMATIC_MESSAGE` rows sits in a dungeon that is not registered: Memory and Soless
(E10 / E11), Erda, Treasure, Halloween and En_Time (later stages). The E6 play-test is therefore a regression check of ordinary NPC and
village speech bubbles, which share the changed base class. The first real dramatic boxes show up in the E10 / E11 play-tests.

### Phase E6 — Review (2026-10-17)

**Opus 5.5. Verdict: PASS + one fix** (the line-break decision is handed back as wrong and fixed under the same flag).

Checked against June in Ghidra (decompile + asm, constants read from the raw exe):
- **Player 32306110**, every field: team gate (GetMyUnit vslot 0x98, team +0x278 XOR-decoded, vs row +0xa8); `START_TIME <= elapsed`; the
  TalkBox it fills (stack box at ESP+0x1e0: uid -10 / -20 and type 2 / 3, trace 0, pos, remain, text and back colour, +0xdc, +0xe0 = 200, image
  +0xe4, +0x110 / +0x111 / +0x114, image offset +0x118 / +0x11c, base y +0x124). The stacking matches the asm @32306440-323064f4: the other
  box's `SetMove( 1, NULL )` runs with ECX = the other box, the new box's `SetMove( 1, other )` with ECX = the local box, and the gap is
  10.0 (33438088). The +44 block (3348c590) adds to pos y, base y and image offset y. The sound plays through vslot 0xa8 (low uid) and
  PlaySound( name, 0, 0 ), and the played flag is set. All match.
- **Loader 322e9e00**: TYPE read first (default 0), the duplicate erase / skip on ( index, uid, state ), the skip path without EndTable
  (JUNE_BUGS 237), the POSITION (0, 0) rule (TYPE 0 / 1 spots 33733d74..80 = (430, 170) / (570, 170), other TYPEs keep (0, 0), custom = 0),
  DISPLAY_TEAM_TYPE last (-1), return `count > 0` (SETG @322eb807). Constants: SHOW_TIME 3.0, IMAGE_OFFSET 50.0, gap 0.1. All match.
- **Talk box**: `TalkBox::Update` slide-up 32e945c0 (both owners, high dword -1, base + 170.0, step 5.0, stop at base + target).
  SetBalloonTip 32f21380 (`type == 2 || type != 3`, 0.2 / 0.4, flip 2). DrawImage 32e92cf0 (both anchor sums, fixed spots
  (430, 170) / (570, 170) with 3373ef5c = 170.0, +0x23c ReverseY, +0x170 OverUI). LineBreak override 32f21600, line for line. All match.
- **Render @3230ad36**: June picks one of two CKTDGObject render wrappers on the pass flag (31cda1a0 / 31cda140), exactly as it does for the
  default manager (@3230acca). March renders the default manager with a plain `OnFrameRender()`, so the dramatic one does the same: match.
  Each of the four game-side functions has exactly one caller (xrefs), and it is the site the port uses: loader @327af18c
  (CommonStateStart), remove @3277fa43 (CommonStateEnd), remove-all @327a062f (NPC dtor), player @3230aa21. The OnFrameMove order matches:
  the +0x48c tick comes right after +0x488's @323095da, and the player call is followed only by E5's 322cfe20 @3230aa28, as in March.

**Fix (the decision handed back): the dramatic Push now runs the global line breaker.** The Done note's premise was wrong.
- March's US build defines `CLIENT_GLOBAL_LINEBREAK` and `CLIENT_GLOBAL_EU_WORDWRAP` (`KTDXLIB/OnlyGlobal/Always_US.h:143/152`).
  `CWordLineHandler::LineBreakInTalkBoxManagerImp` is linked (`X2/US_SERVICE/X2.map`), and March's base Push already uses it.
- June's 31d436c0 is that same function with the length cap and the screen width passed in. Both have the space / split-letter cut, the
  "-\n" hyphen and the resolution tail, and neither reads MAX_ROW_CHAR. June's base Push 32e943d0 and dramatic Push 32f218c0 are its only
  callers.
- `CX2TalkBoxManagerImpDramaticMessage::Push` now does what June's does (call @32f2196d): `m_ResSize = ConvertByResolution( m_Size )`, then the
  global breaker with this manager's font, then a cut to the box's max length (+0xe0, 200), which June's breaker does at its end.
- June's LineBreak override stays in the `#else` (`!CLIENT_GLOBAL_LINEBREAK`), the same build split June had. Dramatic boxes now wrap at the
  speech-bubble width (143 px x scale), like June US's, not at 16 x the width of U+AC00.
- No separate breaker phase is needed: March's and June's breakers differ only in where the 240 / max-length cap comes from.

**Hygiene:**
- E6 commit (`--range 761ce721^..761ce721 --with SERV_IRUHADEV_ELYSION_DEBUG`):
  `files: 8 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
  `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 7 #else/#endif without the flag token, 0 encoding change(s)`;
  `RESULT: PASS`.
  - PASS~ = one blank line in `X2TalkBoxManagerImp.h`.
  - The 7 bare tokens are house-style `#else` lines and the `#endif // A && B` of the debug include.
- Fix (`--worktree --files X2Lib/X2TalkBoxManagerImp.cpp`): `files: 1 PASS`, `RESULT: PASS`.
  - The 2 deleted lines are E6's own comment lines inside the flag, rewritten.
  - The 3 bare tokens are the new `#else` and the `CLIENT_GLOBAL_*` `#endif`s.
- `DEPLOY OK: X2_offline.exe is 19,759,104 B, dated 2026-10-17 14:07:58, md5 c53bba6a4f3e08c0840a2ad83c85948b, equal to X2/US_SERVICE/x2.exe (2026-10-17 14:05:41)` (deploy-only after the user closed the game).
- Post-commit flagcheck `--range 5a7bce71^..5a7bce71`: `files: 1 PASS`, `RESULT: PASS`.

**How to see it:** unchanged. There is nothing to see until E10 / E11 register a dramatic NPC. The fix touches only the TBMT 3 manager, so
ordinary speech bubbles are byte-for-byte what the E6 regression play-test passed.

---

## Phase E7 — Elysion-half bindings + additional world

**Steps** (`SERV_IRUHADEV_ELYSION_SCRIPT_B`): every name A1b assigned to E7
(the *Bindings batch* list), receiver class verified, June address cited.
Plus the **additional world**: WorldData `dataFileName_Additional` in
`AddWorldData_LUA`, loading the second world, `SetShowAdditionalWorld`,
`PlayAdditionalWorldAllSound` (Soless phase 2). Reuse Atlas A4's bindings;
don't duplicate them.

**Done when:** census reports no E7-owned name/key open; build rc=0; new
literals in the exe.

**Run with:** Opus 5.5, high effort (Sonnet 5.5 only for names A1b marked
transcription-ready, stopping on anything else). Required review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch main) --
"Starting context", the ledger's "Elysion half: engine gaps" section, the A1b rows owned
by E7, Atlas "Phase A4" and its Done note, and "Phase E7". Use the ghidra
skill. Never launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase E7): implement every binding A1b assigned to E7 behind
SERV_IRUHADEV_ELYSION_SCRIPT_B (receiver class verified, June address cited,
reusing A4's), plus the additional-world support (dataFileName_Additional,
SetShowAdditionalWorld, PlayAdditionalWorldAllSound).

Done when: Phase E7's "Done when" holds. Record a Done note, then commit on main.
```

### Phase E7 — Done

*(2026-10-17, Opus 5.5 high. Flag `SERV_IRUHADEV_ELYSION_SCRIPT_B` (+ `ELYSION_DEBUG` for `[ELYDBG]`). 25 funcmap rows `Atlas E7`. All 39
`region_engine.tsv` rows owned by E7 ported; none was already in March except the bodies of `AnimStop` / `AnimPlay` / `GetUnitIndex` /
`SetShowInvincible`, which only needed binding.)*

- **Additional world (B85, B27-B30):** WorldData `dataFileName_Additional` (32805770 @32805f81, after CodeEvent); `CreateJuneAdditionalWorlds`
  = 32803f60's second half (one hidden, silent CX2World per name, same WorldData); called after the main world by `CX2DungeonGame` stage load
  (327cd1b0), `CX2BattleFieldGame::WorldLoading` (32d52fa0) and `SwapWorld` (32d54ae0, cleared first); freed in `CX2Game::Release` (323080c0);
  shown worlds move (@32309b77), draw (322d7260), reset / lose the device (322d7320 / 322d73b0, shown only). `CX2World` gets June's +0x1ac
  (`SetShowObject` writes it, 1 from the ctor) and June's `StopBGM` (playing name cleared first). Binds `SetShowWorld`, `PlayWorldAllSound`,
  `SetShowAdditionalWorld`, `PlayAdditionalWorldAllSound`.
- **CX2Game (B15-B20, B24, B25):** `FindRandomSelectUserUnitsFromRange_LUA` / `GetFindedRandomSelectUserUnits_LUA` / `GetRandomSelectUserUnitFromRange`
  / `GetUserIndexByUserUID`; `GetNearestUnitInSpecificRange(_IncludeSameTeam)_LUA` on June's core 322d55b0 (read from the disassembly; the decompile
  mixes the arguments); `StartFadeIn/OutCustomColor` with June's colour fades in `CKTDGManager` (31ce6270 / 31ce62c0; plain fades now pass black).
- **Units (B04, B07, B08, B35, B36):** `AddEffectSetDeleteCondition_LUA`; thorns (`SetThornsInfo`, 32278150 in both DamageReacts, 322780b0 and its
  "HP +n / HP -n" text every 0.2 s); simple timer (`Start` / `Get` / `StopAndDeleteSimpleTimerCount`, "Mine_Countdown" + PC_0..9.dds, per frame).
- **NPC (B05, B06, B09, B12-B14, B21-B23, B31-B33, B37, B65, B67, B68, B70):** `AnimStop`, `AnimResume` (= AnimPlay), `GetUnitID`, Set/AddStateCoolTime,
  stored NPC trio, `VectorUidType_*`, `SetShowHpSmallGage` (NPC and user; `DrawHpGage` gate), `Get/SetSyncDummyValue_Host`, `StartAlphaFade` + its
  frame update, `CAN_PASS_UNIT_AT_FIRST_FRAME`, `SHOW_GAGE` / `INVINCIBLE_ATTACK_TYPE` (stored, June reads neither), `HEIGHT_CHECK_RANGE` (fly AI).
- **Other (B11, B26, B34, B38):** `IsSceneEnded`, `SetParticleSize`, the four BlackHole accessors, `StopEffectSetSilent_LUA` (silent flag reaches
  `DestroyInstanceHandle( h, bSilently )`).
- **Deviations (paths June data never takes, or no March counterpart):**
  - `StopEffectSetSilent_LUA` stops at once (March's `StopEffectSet` has never used June's deferred queue).
  - `VectorUidType_*` reject slots outside 0..9 (June's test can never fire and would run past the ten vectors).
  - `SetSyncDummyValue_Host` stores only; June's P2P packet 0x1d has no receiver offline.
  - The nearest-unit core skips June's raid-object test (g_pMain +0x115c does not exist in March, as in the C3 port).
  - The HP text has no second queue gate (June's +0x68ac queue has no March writer).
  - The additional-world frame move runs right after the main world's (June runs it later in the same frame).
- **Routed:** class code that sets the third thorns byte (3226e370 from 32a67a20) and the other 322780b0 callers (32a6f196, 32aa9d51, 32ac2d38,
  32b6ff0e, 32bb2cc5) -> character-port checklist. The NPC DamageReact's own 322780b0 call (@327b3485) is not E7's.
- **JUNE_BUGS** 234-236 (additional worlds pile up across dungeon stages; hidden additional worlds skip device reset / lost; VectorUidType slot test).
- **Gate:**
  - census `--phase E7 --region elysion`: with the flag probe, only `SetPosion` (E10, B40: a June typo, nil in June too) is left in bind.unbound; the 6 bytecode-inferred names, `CAN_PASS_UNIT_AT_FIRST_FRAME`, `HEIGHT_CHECK_RANGE` and `INVINCIBLE_ATTACK_TYPE` are gone (`SHOW_GAGE` was never listed).
  - flagcheck `--worktree` (E7 files): `files: 18 PASS, 3 PASS~ (blank/marker only), 0 DEFINE, 7 OTHER (other flags: E5 / E6 peers, since committed), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched`; `RESULT: PASS` (the one `#else` without token fixed after).
  - `DEPLOY OK: X2_offline.exe is 19,758,592 B, dated 2026-10-17 13:41:13, md5 e6b48a065ee977f3ed681de722b4dfdd, equal to X2/US_SERVICE/x2.exe (2026-10-17 13:41:06); built at main a67e3869 + 33 uncommitted code file(s).` (the E5 peer's files, since committed as 15c08485, were in it); every E7 bind / key name, `Mine_Countdown` and the five `[ELYDBG]` texts are in the exe.
  - Commit `0cc43f29`; `flagcheck --range 0cc43f29^..0cc43f29`: `files: 23 PASS, 4 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched, 0 #else/#endif without the flag token`; `RESULT: PASS`.
- **How to see it in game:** the Soless boss world row (with its additional world) and most callers arrive with E9-E11, so this phase's
  play-test checks what already-live scripts reach (hand-off in the session). `[ELYDBG]` first-use lines name each mechanism.
- **Open for the user:** nothing to decide. Next: `/x2-phase atlas E7 review` (Opus 5.5 high).
- **Play-test 2026-10-17 PASSED** (run 13:48 on exe e6b48a06; console on jtLordKnight in Besma Mine 2): countdown 5 -> 0 over the head (`[ELYDBG] StartSimpleTimerCount unit 25 time 5.00 count 5`), thorns (`[ELYDBG] thorns: unit 25 reflects 1427.8 ... to 1000000`, red HP text), monsters faded out / in (`[ELYDBG] StartAlphaFade npc 1000000 ( 111 ) time 2.00 in 0 hide 1`, 10 NPCs), small HP bars hidden / shown, world hide / show and music off / on; 0 EXCEPTION. **One miss, fixed:** the white flash drew black. June's fade quad is `HQ_WhiteBack.dds` (31ce9860 @31ce9a28 -> +0x3a0), not March's black `FadeInOut.dds`; now loaded behind the same flag (already in March data048, byte-equal to June; plain fades pass black, so they look as before). `DEPLOY OK: X2_offline.exe is 19,758,592 B, dated 2026-10-17 14:00:28, md5 f9eb73294bc2507ecab2ca5cd28a35bb, equal to X2/US_SERVICE/x2.exe (2026-10-17 14:00:22); built at main 4de3172a + 1 uncommitted code file(s).` (`HQ_WhiteBack.dds` in the exe). Re-test 2026-10-17: white flash confirmed (user).

### Phase E7 — Review (2026-10-17)

**Opus 5.5. Verdict: PASS + one fix** (thorns' HP-test gate was inverted; fixed under the same flag).

Re-read in June's raw disassembly (Ghidra + `funcdis.py` / `disasm_range.py`; constants read from the exe):
- **Additional world:** `AddWorldData_LUA` @32805f81-32805fee (BeginTable after CodeEvent, values 1..n until the first miss, push to +0x58);
  32803f60's second half: `new CX2World( data, arg6 )`, `OpenScriptFile( name, arg5 )`, 327fbeb0( g_pMain +0x1a8 ), then
  SetShowObject / PlayEffectBGM / Play3DEffectBGM( 0 ), push_back. Caller args: dungeon 327cd1b0 (binder 0, bg 0), battle field
  32d52fa0 and swap 32d54ae0 (0, 1; the swap clears first @32d54b07). The +0x1ac gate on reset / lost (322d7320 / 322d73b0), `SetShowWorld`
  322cc040, `PlayWorldAllSound` 322cc060, the index forms 322cf8f0 / 322cf930 (index < size, world non-NULL), `StopBGM` 327fdb80 (name
  cleared to L"" before Stop). All match.
- **CX2Game queries:** 322dd990 (HP > 0 with no gage = 1, `<=` range, unsigned `rand() % size` erase while size > count), 322d0030,
  322dc3c0 (signed `cdq / idiv`, only when n > 1), 322cfc00 (index counts empty slots, 0xffff). Nearest core 322d55b0, argument order from the
  wrapper pushes: (unit, type, front, min, max, centre, same-team); 322d7060 passes 1 and is bound to `_IncludeSameTeam_LUA` (ref @31dd32e4),
  322d7090 passes 0 and is bound to `_LUA` (@31dd3615). Best starts at max (@322d55c2), team / ally team, class 0 / 100 (NCT_BASIC / THING_GATE),
  front test 32293b80 vs vslot 0x170, compares `d < best`, `min <= d`, `d <= max`. All match.
- **Fades:** 31ce6270 / 31ce62c0 (+0x390 word 1 / 0x101), the draw 31ce94a0 overwrites the colour alpha (@31ce9583) and draws 1024 x 768;
  322cff70 passes (0, 0, 0, 1); the white quad is `HQ_WhiteBack.dds` (string 3344ca90, loaded @31ce9a28). Match.
- **Units:** 3227a670 (jump table 3227a724 = 5 cases, 5 = user with skill id > 0); HP text @3229f11f-3229f4e6 (0.2 s at 334411a8, sum > 1 green
  "HP +", < 0 red, 0..1 nothing, +50 y); the simple timer (time > 0, "Mine_Countdown" with March's CreateSequence defaults, tick when
  `count - 1 >= time`, sound only when non-empty); state cool time (fild int, `=` / `+=`); stored NPC; VectorUidType (PopValue -1 absent / 0 on
  error); small gage 32e5d580 = DrawHpGage (GetAlpha, DrawFace 32e5bc40, HP +0x20); sync dummy (host UID == my UID, value differs);
  StartAlphaFade 3275a240 / 3275a320 (rate -1.0 / time, end branch `rate >= 0` restores the blend, else alpha 0 and the optional hide);
  CAN_PASS_UNIT_AT_FIRST_FRAME stored at +0x8fd8 (@327ac314) = the byte Init compares (@3277c26f); HEIGHT_CHECK_RANGE @32f073c1-32f07410
  (> 0, `fild` range < distance skips); IsSceneEnded 325c4a50 / 325c4a70; SetParticleSize 31e4f3f0 (+0x3f8 scale); BlackHole +0xa0c / +0xa10 =
  DRAGGING_SPEED / RANGE (@31f638fd / @31f63a15); StopEffectSetSilent 321db020 -> 321e3ee0 @321e3f61 -> 321e3a80 @321e3ad7 -> 321e2a30
  @321e2af2. All match. The deviations the Done note lists hold.
- **JUNE_BUGS 234-236** are right; census `--phase E7 --region elysion` (probe): bind.unbound = `SetPosion` (E10) only, no E7 key open.

**Fix: thorns tested the defender's HP the wrong way round.** 32278150 runs the "my HP >= damage" test only when +0x75e0 == 1
(`cmp byte [ecx+0x75e0],1 / jnz` @3227815d skips it otherwise); 3226e370 (class code) is the only writer. E7 ran the test when the byte
was 0, so every thorns unit missed reflecting a hit bigger than its HP. Now `m_bJuneThornsCheckHp` (renamed) gates it as June does
(`X2GameUnit.cpp` JuneProcessThorns, `X2GameUnit.h`). funcmap row 32278150 corrected. flagcheck `--worktree` (2 files): `2 PASS`, `RESULT: PASS`
(6 deleted lines = E7's own inserted lines replaced; still insert-only vs the pre-E7 base). `DEPLOY OK: X2_offline.exe is 19,759,104 B, dated 2026-10-17 14:32:19, md5 3abf25191e5118f20ea1df5f41242ca1, equal to X2/US_SERVICE/x2.exe (2026-10-17 14:32:14); built at main b06f994c + 2 uncommitted code file(s).`

**In game:** thorns reflect even on a hit that takes the unit below 0 HP (before: no reflect, no red text). Seen with the E7 console test
when the monster's hit is larger than the player's HP.

---

## Phases E8–E11 — the four dungeon lines

Each follows Atlas's *Per-line checklist* (written by A8): dungeon scripts,
stages + linemaps, `DungeonData` rows (verbatim, Hero rows omitted with a
comment), unit scripts, `NPCTemplet`, `NPCStat`/`NewNPCStat` readiness, boss
face keys, effect sets via the June effect pipeline (merge under the
shared-staging rule), sounds, drops (unknown-item rule), offline clear and
unlock, buff census.

| Phase | Line | Line-specific | Run with |
|---|---|---|---|
| E8 | SkyCrossroads | first line of the half; `ELYSION_DUMMY_TITLE`/`FAKE_OBJECT` interplay; Herna/Emperor families | Opus 5.5, high |
| E9 | Adrian | moving platforms (E5); `SetCameraDistance`, `SetParticleSize`; emperor/laser objects | Sonnet 5.5 high if census shows 0 open gaps, else Opus 5.5 high |
| E10 | Soless | boss script from E0's decompile; hidden stage (`WI_SOLESS_STAGE1_HIDE`, `HIDE_STAGE_A/B`); phase-2 additional world (E7); `AWAKEN`/`HERBAON` gauges (E6); dramatic messages (E6); true/bad ending branches; `S_CIRCLE_CTR` typo verbatim; check `GetBuffAccumulationCountNow` use in `ELYSION_SOLESS` | Opus 5.5, high |
| E11 | Memory | finale, boss `ELYSION_M_SOLESS` (`OF_SUN_GAGE`, `IsReadyBreakAttack` global, `CC_TIME`); **blocked on JUNE_BUFF Phase 10** (`GetBuffAccumulationCountNow`) — check first, record the block and stop if not landed | Opus 5.5, high |

All four: required review (Opus 5.5).

**Done when (each):** census `--line SC|AR|SL|ME` clean (buff-owned items
only if their buff phase has landed); build rc=0 if C++ changed; staged +
manifests; pack list handed over. After pack: every difficulty enters, every
stage (incl. Soless hidden stage and phase 2) loads, gauges/messages/platforms
work, clear pays out and unlocks the next line.

### Phase E8 — SkyCrossroads

**Run with:** Opus 5.5, high effort (first line of the half). Required review (Opus 5.5).

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch main) --
"Starting context", the ledger (both halves), Atlas's "Per-line checklist",
the buff-dependency and unknown-item rules, Appendix B (SkyCrossroads NUIs) and the
"SkyCrossroads" row of "Phases E8-E11". Never launch X2_offline.exe without
asking. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline; tell subagents
the same.

Effort: high. If the census shows an engine gap the A1b map doesn't settle,
stop on it and log it for an Opus pass.

Your task (Phase E8): port the SkyCrossroads line by the Per-line checklist. It is the
first line of the half: confirm E2, E4 and E7 are Done, then port the three
dungeon scripts, the 4 stages + linemaps, the SkyCrossroads unit scripts
(Herna/Emperor families, KEYOBJECT/FAKE_OBJECT/MOVE_OBJECT, the
WATERCANNON_OBJECT.LUA uppercase extension) and ELYSION_DUMMY_TITLE as data.
Add any Elysion-specific notes the checklist lacks to it.

Done when: census --line SC is clean, staged with manifests, pack
list handed over (live-kom check after I pack). Record a "Phase E8 -- Done
(date)" note (DEPLOY OK commit), then commit on
main.
```

#### Phase E8 — Done

**2026-10-17, Opus 5.5 high.** Data only: no C++, no flag change, no build (no `DEPLOY OK`: nothing to deploy). E2, E4, E7 confirmed done and
reviewed (STATUS). Census `--line SC`: 68 open rows before, 42 after, all routed checklist step-9 classes (7 `assets.absent`, 39 `keys.file`,
3 `keys.registry`); `assets.adopt` 6 -> 0, `koms.unstaged` 6 -> 0, `npc.ready` 14 -> 0. No engine gap, no Opus pass owed.

- **Premises held.** Every SC NUI is in `Enum` (A3) with June's value; the 3 dungeon scripts, all 36 unit scripts (incl. `ELYSION_WATERCANNON_OBJECT.LUA`,
  upper-case in June and March) and BossStage + its line map were already staged (ljconv). `NewNPCStat` has every SC NUI, `NPCStat` none (= June).
  Every `MOTION_FILE_NAME` mesh is staged (data181; `Dummy_Ani_Wait.X` data008). Boss faces: `NUI_ELYSION_BOSS_ORB` / `BOSS_SHIELD` / `KEYOBJECT`
  in `data215 DLG_BossState_02.TET` (exact). Effects 416 / 416 present, items 0 missing (boss drops 30552-30591 in the June kim). Buffs: all 8 ids in
  `Enum.lua` (`BFI_BUFF_DEFENCE` is only a `print` string). Row assets: `HQ_LoadingDungeon_SkyCrossroads_Pit_01/02.tga` (data183), `DLG_UI_Dungeon_Image11.dds` (datai022).
- **Staged:** `adopt-june data040` Stage0-2 + their 3 line maps (ljconv; `june2017_re/june_adopt_20261017_143715.tsv`). BOM source, appended
  verbatim as `atlas E8` blocks (scratchpad `e8_edit.py`, A10 shape; self-check: each block minus comments == the June slices, every new id once):
  `data035 DungeonData` June :32268-32508 (Normal / Hard / Expert; no quest key in June; Hero rows named and omitted: `HERO_NORMAL :32509-32670`,
  `HARD :32671-32714`, `EXPERT :32715-32887`, `HELL :38436-38598`), `data037 NPCTemplet` 18 rows (the 14 SC rows + Emperor SHIELD / ORB / BOW / LANCE,
  which only Adrian / Soless spawn; ranges in the block header; `EMPEROR_FEAR` / `SET_*_BOW` / `HERNA_LANCE_CARD` left out: no March NUI, no June spawn),
  `data040 WorldData` WI_SKYCROSSROADS_STAGE0-2 (June :3270-3284). Refreshed, core-list `why` updated; `CORE CHECK PASS: 0 FAIL, 0 WARN`, `LEDGER CHECK PASS: 0 FAIL, 0 WARN`.
- **`ELYSION_DUMMY_TITLE` / `FAKE_OBJECT`:** data only. Two DUMMY_TITLEs per difficulty (`ACTIVE = FALSE`, start pos 11 and 22); the first hit on
  FAKE_OBJECT heals it, sends it to `_DYING` (or `_DYING_NOT_ALARM` if KEYOBJECT's flag 5 is set) and zeroes every DUMMY_TITLE's HP
  (`ELYSION_FAKE_OBJECT.lua:255-280`). All methods it calls are bound (census `bind` clear). Whatever title June gave for the dummy is server-side.
- **June bug kept verbatim (JUNE_BUGS #239):** the water cannon's Ready state says `CAINVINCIBLE`; June and March read `INVINCIBLE`, so it is not invincible.
- **Pack:** `PACK OK data035`, `PACK OK data037`, `PACK OK data040`. Copy (user OK 2026-10-17): `COPY OK` data035 (md5 81e30818) / data037 (5e1a8b6d) /
  data040 (8f4e46e7). Census `--line SC --live`: `live {"same": 313}`, the same 42 routed rows.
- **Offline:** Tower Normal clear (33350) unlocks SC via `EXTRA_REQUIRE_DUNGEON_ID`; a clear records 33870 / 33871 / 33872 with no code; Adrian's
  rows (E9) name `DI_SKYCROSSROADS_NORMAL`. Kills pay nothing (no server `DropTable` rows; backlog Stage 23).
- **How to see it (after the copy):** a level-83+ character that has cleared Tower Normal: M -> Elysion tab -> SkyCrossroads icon enabled -> enter
  Normal; Stage0, 1, 2 and the boss stage load; Herna monsters fight; the key object / fake object gates open the line map; the boss orb / shield
  gauges show their faces. ErrorLog must show no `DungeonData` / `NPCTemplet` / `WorldData` load errors.
- **Play-test PASSED 2026-10-17 (user); logs read (run 14:47-14:52, exe md5 3abf2519, koms after the copy):** reyaa (Grand Archer 85) entered from the
  local map, `dungeonID=33870` room, `KEYOBJECT` (2266) `bossGauge=1` on stage 1, `BOSS_ORB` (2264) and `BOSS_SHIELD` (2265) `bossGauge=1` on stage 3,
  `dungeonID=33870 CLEARED in 240s: 78 kill(s)` (+0 exp / +0 ED, `no reward row` per kill, as designed; Stage 23). 0 EXCEPTION; UNHANDLED only the two
  known `E_DISABLE_HB_CHECK_REQ`. No `DungeonData` / `NPCTemplet` / `WorldData` load error and no missing mesh or texture. ErrorLog noise, none from E8:
  `result : 47` on `SONIC_ARROW` / `ARROW.OGG` (the archer's own sounds), the `DLG_UI_LOCAL_MAP` statics (JUNE_BUGS 228, as A9 / A10), one `JUNE_FX HERO_ANIM` rows=0 line.

### Phase E8 — Review (2026-10-17)

**Opus 5.5 high. PASS, no fix, no repack, no copy.** Data-only phase: no C++, no flag, no build (no `DEPLOY OK`: nothing to deploy). The registry
diffs were re-run after the peer's E9 commit `8e687931` appended to the same three files; the E8 blocks are unchanged.

- **Registry rows verbatim (comment / blank lines dropped; strict and whitespace-normalised agree, 0 diffs each):** `data035 DungeonData.lua:17564-17812`
  = June :32268-32508 (241 lines; `DI_SKYCROSSROADS_NORMAL` / `HARD` / `EXPERT` at June :32280 / :32439 / :32480; no quest key in June or March);
  `data037 NPCTemplet.lua:20268-20435` = the 18 June ranges in its header (161 lines); `data040 WorldData.lua:3305-3323` = June :3270-3284 (15 lines).
  Every new `dungeonID` / `NPC_ID` / `worldID` is defined once per file; no other SkyCrossroads row exists (`WI_SKYCROSSROADS_BOSSSTAGE` :3196 is C3y-1b's).
  The file's only duplicate `NPC_ID`s (`NUI_RANOX_BURNDEDFOREST_GATE`, `NUI_RANOX_ASHESVILLAGE_GATE`) predate E8.
- **Hero omission:** the four header ranges are exactly `DI_SKYCROSSROADS_HERO_NORMAL` (:32524) / `HERO_HARD` (:32686) / `HERO_EXPERT` (:32729) /
  `HERO_HELL` (:38449). June's other SkyCrossroads mentions (:32903, :33059, :33097) are later dungeons' `EXTRA_REQUIRE_DUNGEON_ID`. Still C3y-5's.
- **Row choice:** the Emperor SHIELD / ORB / BOW / LANCE are named by June's Adrian N/H/E (all four) and Soless N/H/E (ORB, SHIELD), by no SC script. The
  left-out four (`EMPEROR_FEAR` 2303, `SET_ELYSION_HERNA_BOW` 2321, `SET_ELYSION_EMPEROR_BOW` 2322, `HERNA_LANCE_CARD` 2326 in June's Enum) are in no
  dungeon script of SC / AR / SL / ME in June or March, and March's `Enum.lua` defines none of them (Emperor 2299-2302 at :4745-4748). These are the census's 2
  missing NUIs, as designed.
- **Unlock chain:** `DI_TOWER_NORMAL` = 33350, SC = 33870 / 33871 / 33872 (`DungeonEnum.lua:321-326`). All three rows carry `EXTRA_REQUIRE_DUNGEON_ID =
  { DI_TOWER_NORMAL }`, `m_MinLevel` 83, and no `m_RequireDungeonID` (June: Hard and Expert do not need SC Normal). `IsActiveDungeon`
  (`X2DungeonManager.cpp:718-794`): the extra-require loop is at :732-739, and under `JUNE_DUNGEON_SUITABLE_LEVEL` the level gate is MinLevel - 2 (:743-757),
  so level 81+ passes, not 83+; `m_MaxLevel` is not checked.
- **Adopted files:** the six data040 files (Stage0-2 + LineMaps) match `shipped_sha1` in `june_adopt_20261017_143715.tsv`, start `1B 4B 4C 81`, manifest `"2"`.
- **JUNE_BUGS #239 confirmed in the exe.** `32784c00` @3278f50c pushes plaintext `"INVINCIBLE"` (33478fc0) into `BeginTable 32fe9940`, then reads floats
  1-4 into StateData +0x360..+0x36c. No June string, plain (Ghidra) or XOR (`xorstrings.tsv`), contains `CAINVINCIBLE`. March reads the same key
  (`X2GUNPC.cpp:6402`, :15870).
- **DUMMY_TITLE / FAKE_OBJECT:** `ELYSION_FAKE_OBJECT.src.lua:255-280` is as the Done note says. Each of June's `Dungeon_Elysion_SkyCrossroads_{Normal,Hard,Expert}` places
  `NUI_ELYSION_DUMMY_TITLE` twice: :569 (`START_POS {11}`) and :890 (`{22}`), both `ACTIVE = FALSE`. The lines are byte-identical across the three difficulties.
- **Premises (spot-checks):** BOSS_ORB 2264 / KEYOBJECT 2266 / HERNA_ORB 2260 match June's Enum. Each has a NewNPCStat row and a staged unit script, and its
  `MOTION_FILE_NAME` mesh is in data181. The three boss faces are in `data215 DLG_BossState_02.TET` (UTF-16LE, once each). Four effect names are present as exact
  constants (`EffectSet_Elsion_HernaFear_Ready`, `_EMPEROR_Wing`, `_Key_Object_Wait`, `DamageEffect_Elsion_HernaBow_Attack_C_Projectile`). The census shows
  effects 416 / 416 and items 0 missing. `SKYCROSS_EVENT0`'s `MOTION` is the NPCTemplet's `m_wstrMotionName` (`X2UnitManager.cpp:968`), which has no loader use.
  The script's own mesh, `Motion_SKYCROSS_DUMMY.X`, is staged.
- **Two Done-note counts are off by wording, not content.** The buffs are **6 distinct ids**, not 8: `BFI_INVINCIBLE_BY_HERNA_SHIELD` 834,
  `_BY_BOSS_SHIELD` 835, `BFI_BUFF_DEFENCE_STEP1/2_BOSS_SHIELD` 838 / 839, `BTI_INVINCIBLE_BY_BOSS_SHIELD` and `BTI_SIMPLE_BUFF_FINAL_DEFENCE_MULTIPLIER`, all
  in March's `Enum.lua`. `BFI_BUFF_DEFENCE` is the print string. The census header's "42 open" excludes the 7 `assets.absent` rows: 49 routed rows are listed
  (7 + 39 + 3). The Done note is left as written.
- **Routed, not open:** 7 `assets.absent` (June ships none of these `.ogg`s); 39 `keys.file`: `NPC_REWARD_RATE` / `REWARD_RATE` (21), linemap `LINE_NUM` (35),
  `ANIM_SPPED` (the two Lanox SECRET_ scripts), `CAINVINCIBLE`, and the CT_ / state keys. 3 `keys.registry`: `CIEL`, `ENDING_SPEECH0`, `LU`. Nothing else is open.
- **Checklist step 11** is accurate and covers what E9-E11 need: bytecode stat files, no Hero scripts, Henir / F_ NUIs, the left-out-NUI rule, the key-typo
  rule, and assets the census `refs` decide.
- **Hygiene:** `LEDGER CHECK PASS: 0 FAIL, 0 WARN`; `CORE CHECK PASS: 0 FAIL, 0 WARN`; census `--line SC` shows only the routed classes; `--live`
  shows `live {"same": 313}`.
- **E9:** a peer already ran it (`8e687931`, Sonnet, as the kickoff rule says: `--line AR` showed no engine-gap class, only data rows), so its kickoff
  and review kickoff exist.

### Phase E9 — Adrian

**Run with:** Sonnet 5.5, high effort **if** `region_census.py --line AR` shows zero open engine gaps; otherwise Opus 5.5, high. Required review (Opus 5.5).

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch main) --
"Starting context", the ledger (both halves), Atlas's "Per-line checklist",
the buff-dependency and unknown-item rules, Appendix B (Adrian NUIs) and the
"Adrian" row of "Phases E8-E11". Never launch X2_offline.exe without
asking. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline; tell subagents
the same.

Effort: high. If the census shows an engine gap the A1b map doesn't settle,
stop on it and log it for an Opus pass.

Your task (Phase E9): port the Adrian line by the Per-line checklist. Its linemaps
use moving platforms (E5 must be Done -- Elysion_Adrian_Stage2_linemap is
lowercase), the laser/emperor objects, SetCameraDistance and SetParticleSize
(A4/E7). Reused Lanox NUI: SECRET_MAG_NOM.

Done when: census --line AR is clean, staged with manifests, pack
list handed over (live-kom check after I pack). Record a "Phase E9 -- Done
(date)" note (DEPLOY OK commit), then commit on
main.
```

#### Phase E9 — Done

**2026-10-17, Sonnet 5.5 high.** Data only: no C++, no flag change, no build (no `DEPLOY OK`: nothing to deploy). E5, E7, E8 confirmed done.
Census `--line AR`: 129 open rows before, 83 after, all routed checklist step-9 classes (13 `assets.absent`, 80 `keys.file`, 3 `keys.registry`);
`assets.adopt` 8 -> 0, `koms.unstaged` 8 -> 0, `npc.ready` 30 -> 0. No engine gap, no Opus pass owed.

- **Premises held.** All 30 missing NPCTemplet rows have a staged unit script and every `MOTION_FILE_NAME` mesh is staged (`Dummy_Ani_Wait.X`,
  `Motion_BOSSSATGE_LASER.x`, `Motion_ADRIAN_*` ...). Boss faces: `NUI_HERBAON` / `NUI_NEW_DEKAL` in `data215 DLG_BossState_02.TET`. Buffs
  (`BFI_HOLD_BY_HERBAON`, `BGI_JUST_BUFF`, `BTI_HIT_COUNT_ATTACK`, `BTI_HOLD_WITH_STICK_SHAKE`) all in `Enum.lua`. Effects / items: census 0 missing.
  `NUI_SECRET_MAG_NOM` (Lanox) already in NPCTemplet. The `*_HENIR` rows were already in.
- **Staged:** `adopt-june data040` Elysion_Adrian_Stage0-3 + 4 line maps (ljconv; `Stage2_linemap` lower-case as June; `june2017_re/june_adopt_20261017_145520.tsv`).
  BOM source, appended verbatim as `atlas E9` blocks (scratchpad `e9_edit.py`, E8 shape; slice-boundary asserts before the append, each new id once afterwards):
  `data035 DungeonData` June :32888-33119 (Normal / Hard / Expert; no quest key; Hero rows named and omitted: `HERO_NORMAL :33355-33518`, `HARD :33519-33564`,
  `EXPERT :33565-33739`, `HELL :38599-38761`), `data037 NPCTemplet` 30 rows (lasers, wall / damage / gauge / stairs objects, Adrian events + gate, 3STAGE_BACK,
  Pang family, NEW_DEKAL, BOSS_HERBAON + DUMMY_HERBAON, L/R H/V lasers; ranges in the block header), `data040 WorldData` WI_ADRIAN_STAGE0-3 (June :3380-3399;
  `WI_ADRIAN_BOSSSTAGE` was in since C3y-1b). Core-list `why` updated; `CORE CHECK PASS: 0 FAIL, 0 WARN`, `LEDGER CHECK PASS: 0 FAIL, 0 WARN`.
- **Routed, not E9's:** `MOTIONNAME_EFFECTSETNAME` in `Elysion_Adrian_Stage1_LineMap.lua` is june-engine W1 (STATUS `W1` ready): that one trigger does nothing
  until W1 lands (March has no reader). `AFTER_IMAGE_DEST_ALPHA_BLEND`, `CT_*`, `LINE_NUM`, `REWARD_RATE` as checklist step 9. No new JUNE_BUGS row.
- **Pack:** `PACK OK data035`, `PACK OK data037`, `PACK OK data040`. Copy (user OK 2026-10-17): `COPY OK` data035 (ae183f2d) / data037 (03ee2201) / data040 (ea44f3e3); census `--line AR --live`: `live {"same": 321}`.
- **Offline:** SkyCrossroads Normal clear (33870, E8) unlocks Adrian via `EXTRA_REQUIRE_DUNGEON_ID`. A clear records Adrian's keys with no code; kills pay
  nothing (no server `DropTable` rows; backlog Stage 23).
- **How to see it (after the copy):** a level-84+ character that has cleared SkyCrossroads Normal: M -> Elysion tab -> Adrian icon enabled -> enter Normal;
  Stage0-3 and the boss stage load; the moving platforms in Stage2 carry the player (E5); laser / gauge objects fire; the Herbaon / New Dekal boss gauge shows its face.
  ErrorLog must show no `DungeonData` / `NPCTemplet` / `WorldData` load error and no missing mesh.
- **Play-test 2026-10-17 (user + log, run 15:03-15:08, exe md5 3abf2519, koms after the copy):** reyaa (lv 85) entered `dungeonID=33880`, Stage2 moving platforms
  loaded (`[ELYDBG] movable line` x5, `Elysion_Adrian_Stage2_LineMap.lua`), boss gauges on stage 1 (npc 2275) and 4 (npc 2276), `CLEARED in 279s: 107 kill(s)`, 0 EXCEPTION,
  no `DungeonData` / `NPCTemplet` / `WorldData` load error, no missing mesh. ErrorLog: `Elysion_Adrian_AP_Shield02.TGA` (named inside `Motion_ADRIAN_EVENT3_A.x`, no copy in
  any kom; June's data) and `Elysion_Adrian_center02.TGA` (the `.dds` is in data180), `ELYSION_GAUGE_OBJECT_Bubble.ogg` (assets.absent). **User saw "NPC Motion Error"
  chat lines (BOMB_PANG `DamageAirUp`, Herbaon):** `june2017_re/npc_anim_census.py` over all 151 `ELYSION_*` unit scripts: 58 name an `ANIM_NAME` their mesh lacks, every one of
  those meshes is byte-identical to June's (e.g. `Motion_ELYSION_BOMB_PANG.x` lacks `DamageAirUp` / `JumpUp`, `Motion_BOSS_HERBAON.x` lacks `Dash` / `DashEnd`), 0 are fixed by
  a June copy; table `june2017_re/elysion_npc_anim_gaps.tsv`. June's own `CommonStateStart` (327ac9d0 @~3277b4xx: `FUN_31d33fd0` false -> "NPC Motion Error. LuaFileName ..." log)
  does the same; the state just keeps the previous pose. March only adds the yellow `_IN_HOUSE_` chat line (`X2GUNPC.cpp:16071`). User decided 2026-10-17: leave it as June has it (no flag, no change).

### Phase E9 — Review (2026-10-17)

**Opus 5.5 high. PASS, no fix, no repack, no copy.** Data-only phase: no C++, no flag, no build (no `DEPLOY OK`: nothing to deploy). Run after the
peer's E10 commits (`3d01ca68`..`a5d911d9`) appended to the same three files; the E9 blocks are unchanged.

- **Registry rows verbatim (CRLF-normalised, 0 diffs each; the BOM is the only file-level difference):** `data035 DungeonData.lua:17820-18051` = June
  :32888-33119 (232 lines, `AddDungeonData({` .. `})`); `data037 NPCTemplet.lua:20441-20710` = the 30 June ranges in its header (each 9 lines,
  HAMMER_PANG :20318 .. DAMAGE_OBJECT_STAGE2_SUB_STAGE2 :20901, 270 lines); `data040 WorldData.lua:3327-3346` = June :3380-3399 (20 lines). Every new
  `dungeonID` / `NPC_ID` / `worldID` is defined once per file (`WI_ADRIAN_BOSSSTAGE` is C3y-1b's). The only duplicate `NPC_ID`s are the two Ranox gates E8's review named.
- **Hero omission:** the four header ranges are exactly `DI_ADRIAN_HERO_NORMAL` / `HERO_HARD` / `HERO_EXPERT` / `HERO_HELL`, none in the block; March's
  `DungeonEnum.lua` has no `DI_ADRIAN_HERO_*` value.
- **Unlock chain:** `DI_SKYCROSSROADS_NORMAL` 33870, `DI_ADRIAN_NORMAL` / `HARD` / `EXPERT` 33880-33882 (`DungeonEnum.lua:324-329`). All three Adrian rows carry
  `EXTRA_REQUIRE_DUNGEON_ID = { DI_SKYCROSSROADS_NORMAL }`, `m_MinLevel` 84, `m_MaxLevel` 87, no `m_RequireDungeonID`; Soless N/H/E (`:18073` / `:18238` /
  `:18273`) each name `DI_ADRIAN_NORMAL`. As in E8, the June suitable-level gate opens at MinLevel - 2 = 82, not the Done note's "84+".
- **Adopted files:** the 8 data040 files match `shipped_sha1` / size in `june_adopt_20261017_145520.tsv`, start `1B 4B 4C 81`, manifest `"2"`;
  `Elysion_Adrian_Stage2_linemap.lua` is lower-case in the file, the directory and the manifest.
- **Premises (spot-checks):** `NUI_ELYSION_BOSS_HERBAON` / `NEW_DEKAL` / `L_H_LASER` each have a staged data037 script whose mesh (`Motion_BOSS_HERBAON.x`,
  `Motion_NEW_DEKAL.x`, `Motion_BOSSSATGE_LASER.x`) is in data181, exact case. Boss faces: both scripts set `BOSS_GAGE_FACE_TEX = "DLG_BossState_02.tga"` and
  pieces `NUI_HERBAON` / `NUI_NEW_DEKAL`, both in `data215 DLG_BossState_02.TET` (read through `X2GUNPC.cpp:4454-4455`). Buffs in `Enum.lua`: `BTI_HIT_COUNT_ATTACK` 793,
  `BTI_HOLD_WITH_STICK_SHAKE` 812, `BFI_HOLD_BY_HERBAON` 836, `BGI_JUST_BUFF` 4; the BFI's factor row (`BattleFieldBuffFactor` :114514) names that BTI, whose templet
  row is `BattleFieldBuffTemplet` :25915. Effects: 4 Herbaon names (`EffectSet_Elysion_Boss_Herbaon_Attack_A_Ready`, `_Attack_C_Tel_FX`,
  `DamageEffect_Elsion_Boss_Herbaon_Attack_B`, `_Attack_A_Hold_FX`) are exact constants in data036; census effects / items / strings 0 missing.
- **Engine hooks:** `SetCameraDistance` (`X2Main.cpp:3730/3732`), `SetParticleSize` (`:3850`) bound; the movable-line reader (`KTDGLineMap.cpp:2809`,
  `X2World.cpp:1722`, sync `X2Game.cpp:1267-1275`) is E5's, and the play-test showed 5 movable lines. The `L_H_LASER` script's 16 engine calls are all bound
  (`CX2GUNPC` / `CX2GameUnit` / `CX2Game` class_def lines); lasers need no C++. `Always.h:4597`'s "TAT_LINE_CHANGE_MOVABLE_DATA ... Not ported" is a stale comment, left.
- **New finding: the Stage1 `MOTIONNAME_EFFECTSETNAME` trigger is inert in June too, not only until W1.** Its value `"Effectset_ELYSION_ADRIAN_EVENT3"` has no
  space. June case 7 (`328012b5`; funcmap row) calls `find( L" " 3344ff0c )` @32801314 and exits to `32801531` on `-1` (disassembly read this review). Ranox Geyser's
  two rows carry `"<mesh> <effect set>"`. Kept verbatim: **JUNE_BUGS 244**; `kickoffs/june-engine/W1.md` now says not to use Adrian as a case-7 check.
- **Routed, not open:** the census's "83 open" leaves out the 13 `assets.absent` rows, so 96 rows are listed (13 + 80 + 3), the same wording as E8's. 13 `assets.absent`
  (June ships none: laser / 3stage `.ogg`s, `elysion_damage_object_2.dds`, the two `_the_odd_specialattack` names); 80 `keys.file`: CT_ / state keys (75),
  `NPC_REWARD_RATE`, `REWARD_RATE`, `LINE_NUM`, `AFTER_IMAGE_DEST_ALPHA_BLEND`, `MOTIONNAME_EFFECTSETNAME`; 3 `keys.registry`: `CIEL`, `ENDING_SPEECH0`, `LU`. No other class.
- **Hygiene:** `LEDGER CHECK PASS: 0 FAIL, 0 WARN`; `CORE CHECK PASS: 0 FAIL, 0 WARN`; census `--line AR --live`: `live {"same": 334}`.
- **E10:** a peer already ran it, and its kickoff and review kickoff exist (the Soless boss is read via `ljbc.py`; no `ELYSION_SOLESS.src.lua` exists).

### Phase E10 — Soless

**Run with:** Opus 5.5, high effort. Required review (Opus 5.5).

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch main) --
"Starting context", the ledger (both halves), Atlas's "Per-line checklist",
the buff-dependency and unknown-item rules, Appendix B (Soless NUIs) and the
"Soless" row of "Phases E8-E11". Read the Soless boss from
june2017_re/june_lua/ELYSION_SOLESS.src.lua (E0's decompile -- it is not in
june_src_fixed/). Check JUNE_BUFF_PORT_PHASES.md for Phase 10's status first.
Never launch X2_offline.exe without
asking. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline; tell subagents
the same.

Effort: high. If the census shows an engine gap the A1b map doesn't settle,
stop on it and log it for an Opus pass. If ELYSION_SOLESS really calls GetBuffAccumulationCountNow and JUNE_BUFF
Phase 10 hasn't landed, record the block and stop.

Your task (Phase E10): port the Soless line by the Per-line checklist: the boss,
the hidden stage (WI_SOLESS_STAGE1_HIDE via HIDE_STAGE_A/B), the phase-2
additional world (E7), the AWAKEN and HERBAON gauges (E6), dramatic messages
(E6), the true/bad ending branches, and June's NUI_ELYSION_S_CIRCLE_CTR typo
transcribed verbatim with a log line. Reused Lanox NUIs: SECRET_MAG_CHARGER,
SECRET_SUMMON_MAG_NOM, SECRET_MAGI_OBJECT, SECRET_LAVA_DUMMY.

Done when: census --line SL is clean (hidden stage and phase 2 included), staged with manifests, pack
list handed over (live-kom check after I pack). Record a "Phase E10 -- Done
(date)" note (DEPLOY OK commit), then commit on
main.
```

#### Phase E10 — Done

**2026-10-17, Opus 5.5 high.** Data, plus one diagnostic in C++ (`SERV_IRUHADEV_ELYSION_DEBUG`). E5, E6, E7, E9 confirmed done (E9 play-test passed, `641c8d2a`).
Census `--line SL`: 152 open rows before, 81 after, all routed: 31 `assets.absent`, 77 `keys.file`, 3 `keys.registry`, plus 3 JUNE_BUGS rows (below);
`assets.adopt` 13 -> 0, `koms.unstaged` (SL) 13 -> 0, `npc.ready` 45 -> 0. No engine gap.

- **Boss: no decompile.** `june2017_re/june_lua/ELYSION_SOLESS.src.lua` never existed (E0's ljd run ran out of memory); read with `tools/ljbc.py list` on the staged
  `data037 ELYSION_SOLESS.lua` (ljconv, 105,101 B, since 2026-10-09). All 105 methods it calls are bound in March; the ~79 other globals are its own functions.
  `GetBuffAccumulationCountNow` (`BTI_DEBUFF_DARK_SOLES_ATTACK` count > 3, per player) is bound (`X2Main.cpp:4361`, `JUNE_BUFF_BINDING`, buff Phase 10 done): no block.
  The kickoff file now names `ljbc.py` (this note's correction; the plan's inline prompt is frozen).
- **Engine hooks the line uses, all in place:** AWAKEN gauge (`CreateSimpleDLG("DLG_UI_ELYSION_SOLESS_AWAKEN_GAGE.lua")` in the boss, so E6's "nothing loads
  AWAKEN_GAGE" holds only for text sources) and HERBAON gauge (`ELYSION_S_2STAGE_HERBAON`), both dialogs staged in data034 by E6; dramatic messages
  (`ELYSION_SOLESS_DRAMATIC`, E6); phase 2 = `DarkSoles_DoPhaseChange_InMiddle`: `SetShowWorld(false)` + `SetShowAdditionalWorld(0, true)`, the world being
  WI_SOLESS_BOSSSTAGE's `dataFileName_Additional` (E7 reader `X2WorldManager.cpp:304`). Endings: `CF_ELYSION_SOLESS_DYING_TRUE/BAD_ENDING` on NPC flag 3.
  Hidden stage: when STAGE0's gate `S_0STAGE_GATE` dies it reads game int 0 (+1 each time `S_0STAGE_LASER2/3` hits the player: their `ATTACKED_FUNC`, called from
  `SetAttackedUnit`): > 0 kills `HIDE_STAGE_A` -> `CT_STAGE 1`, 0 kills `HIDE_STAGE_B` -> `CT_STAGE 5` (`WI_SOLESS_STAGE1_HIDE`). No laser hit = hidden stage; no server value.
- **Premises held.** All 45 rows: unit script staged (data037, ljconv), `MOTION_FILE_NAME` meshes staged (data181 / data008), Enum value = June's, `NewNPCStat` has
  them and `NPCStat` has none (= June). Boss faces `NUI_SOLESS`, `NUI_HERBAON`, `NUI_COPY.S` in `data215 DLG_BossState_02.TET`. Buffs (`BFI/BTI_*DARK_SOLES*`,
  `BFI_HOLD_BY_SOLESS`, `BTI_DEBUFF_SHOCK_ELYSION_6`, ...) all in `Enum.lua`. Effects 416 / 416, items 0 missing.
  `ELYSION_S_MAYA_Weapon_Circle.y` is `assets.absent` (June ships none).
- **Staged:** `adopt-june data040` the 13 Soless Stage0-3 / Stage1_Hide / BossStage / BossStage_2Phase + line maps (ljconv; `june2017_re/june_adopt_20261017_150655.tsv`).
  BOM source, appended verbatim as `atlas E10` blocks (scratchpad `e10_edit.py`: slice-boundary asserts, then each new id exactly once):
  `data035 DungeonData` June :33120-33354 (N / H / E; no quest key; June has **no** Soless Hero rows), `data037 NPCTemplet` 45 rows (June :21019-21171,
  :21190-21414, :21496-21522; per-row ranges in the header; `SOLESS_EVENT2` / `_A` left out: no March NUI, no June spawn), `data040 WorldData`
  WI_SOLESS_STAGE0-3, STAGE1_HIDE, BOSSSTAGE (June :3405-3437). Refreshed, core-list `why` updated; `CORE CHECK PASS: 0 FAIL, 0 WARN`, `LEDGER CHECK PASS: 0 FAIL, 0 WARN`.
- **June bugs kept verbatim:** JUNE_BUGS 240 (`NUI_ELYSION_S_CIRCLE_CTR` in STAGE1 `READY_NPC`: nil, June's reader `327f3c40` stops there like March's, so 5 later
  entries are not preloaded), 241 (`SetPosion` on the spheres: bound in neither; Lua error, sphere not repositioned), 242 (`LINE_TYPE.LT_INVALID` nil, test never
  true), 243 (sphere faces absent from June's TET, byte-identical to ours). `CANDOWN` is 232 (E5).
- **Code:** `X2DungeonStage.cpp` `LoadReadyNPC`: after the loop, probe the next 8 indices; on a hit log `[ELYDBG] READY_NPC: world <id> entry <n> is not an NPC id ...`.
  No behaviour change. flagcheck `--worktree --files X2Lib/X2DungeonStage.cpp`: `files: 1 PASS ... 0 FAIL, 0 ERROR`, `0 deleted line(s) ... 0 encoding change(s)`, `RESULT: PASS`.
  `DEPLOY OK: X2_offline.exe is 19,759,616 B, dated 2026-10-17 15:12:58, md5 9e7072a53ababc6a323912c21d0e5cdf` (string present in the exe).
- **Pack:** `PACK OK data035`, `PACK OK data037`, `PACK OK data040`. Copy (user OK 2026-10-17): `COPY OK` data035 (1456f08a) / data037 (e490042f) / data040 (9826cece); census `--line SL --live`: `live {"same": 334}`.
- **Offline:** Adrian Normal clear (33880) unlocks Soless (33890 / 33891 / 33892, `m_MinLevel` 87, so level 85+ passes under `JUNE_DUNGEON_SUITABLE_LEVEL`).
  A clear records with no code; kills pay nothing (Stage 23). Memory (E11) names `DI_SOLESS_NORMAL`.
- **How to see it (after the copy):** a level-85+ character that has cleared Adrian Normal: M -> Elysion tab -> Soless -> Normal. Stage0 (moving platforms, E5
  review's two checks; take no laser 2 / 3 hit for the hidden stage), Stage1 or the hidden stage, Stage2 (Herbaon gauge), Stage3, boss: AWAKEN gauge, phase-2 world swap, dramatic
  boxes, true / bad ending. `offline_server.log` shows `[ELYDBG] READY_NPC: world 712 entry 10` when the stage data loads.
- **Play-test 1 (2026-10-17, reyaa, run 15:26-15:30, exe 9e7072a5): two findings.** (1) **Platform sank through the floor after the ride** (user): JUNE_BUGS 230
  as predicted. June's code read again (`31c70580` -> `31c6fbb0`, `32801ab0`, `31c74fe0`, `32802140`): no reset of the record's last move anywhere, but June's game
  stops the platform (user), so it is a port gap, not a kept bug: new flag `SERV_IRUHADEV_ELYSION_MOVABLE_LINE_END` zeroes `m_vLastMove` of a finished / stopped
  record (`KTDGLineMap.cpp` FrameMoveMovableLine, insert-only; flagcheck `--worktree` PASS, 0 deleted, 0 encoding change). JUNE_BUGS 230 verdict -> fixed.
  `DEPLOY OK: X2_offline.exe is 19,759,616 B, dated 2026-10-17 15:48:01, md5 b45748efc13c0a7494aefe06426d1b13`.
  (2) **Stuck in Stage0's gate room:** stage 0 was loaded twice (15:26:44, 15:27:18). The second load was a `GoStage` (the only path that sends
  `EGS_DUNGEON_STAGE_LOAD_REQ` with no sub-stage clear; no Soless script calls it, so the END console's `/stage`), and it left the first load's NPCs alive:
  two gates, two `HIDE_STAGE_B`. Both gates' dying function killed the first `HIDE_STAGE_B` (1000068), the second (1000145) lived, so `CC_KILL_NPC_TYPE` never
  cleared. A console restart of the current stage duplicates its NPCs: pre-existing, not E10's; restart the dungeon instead.
  Why the user restarted (user): a laser hit in the air put reyaa on floor 2 before floor 1 was clear. June's data allows it: floor 2's walkway, line 48
  (y 10455), is never disabled (trigger 2 disables only the connectors 46/47/73/74/76 and 59/60/72/75; triggers 1 / 0 re-enable them), and line 48 has
  no `CAN_DOWN = FALSE` (March default true, `KTDGLineMap.cpp:257`), so the down key drops back to floor 1. Not a port gap. 0 EXCEPTION; `READY_NPC` log as designed.

### Phase E11 — Memory

**Run with:** Opus 5.5, high effort. Required review (Opus 5.5). **Blocked on JUNE_BUFF Phase 10.**

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch main) --
"Starting context", the ledger (both halves), Atlas's "Per-line checklist",
the buff-dependency and unknown-item rules, Appendix B (Memory NUIs) and the
"Memory" row of "Phases E8-E11". FIRST check JUNE_BUFF_PORT_PHASES.md: if Phase 10
(GetBuffAccumulationCountNow) hasn't landed, record the block in this doc and
stop. Never launch X2_offline.exe without
asking. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline; tell subagents
the same.

Effort: high. If the census shows an engine gap the A1b map doesn't settle,
stop on it and log it for an Opus pass.

Your task (Phase E11): port the Memory line by the Per-line checklist -- the
finale: boss ELYSION_M_SOLESS (the OF_SUN gauge from E6, the
IsReadyBreakAttack global other Memory scripts call -- respect the load order,
CC_TIME clear condition), the memory/knight/summon families and dramatic
messages.

Done when: census --line ME is clean, staged with manifests, pack
list handed over (live-kom check after I pack). Record a "Phase E11 -- Done
(date)" note (DEPLOY OK commit), then commit on
main.
```

#### Phase E11 — Done

*(empty)*

---

## Phase E12 — Elysion field PvP map (optional)

Mirror Lanox Phase 11 (`RANOX_PORT_PLAN.md` Phase 11 findings):
`PVP_Elysion_Field` + LineMap (WI 752), `PVP_Map_List` row
(`MAP_SELECT_ELISION`; lift `M14/data036/PVP_Map_List.src.lua:116-117`'s
omission), map-select art. `WINNER_Adrian_PVPMAP` (757, tournament) stays
out.

**Run with:** Sonnet 5.5, high effort. Required review (Opus 5.5).

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch main) --
"Starting context", RANOX_PORT_PLAN.md Phase 11 findings, and
"Phase E12". Never launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase E12): port the Elysion field PvP map (WI 752,
MAP_SELECT_ELISION) the way Lanox Phase 11 ported GateWay.

Done when: staged with manifests, pack list handed over. Record a Done note, then commit on main.
```

### Phase E12 — Done

*(empty)*

---

## Phase E13 — Asset closure + pack list (Elysion half)

Same method as Atlas A11, over everything E1–E12 staged (mesh-internal
textures, TET keys, truncation, mount order); one consolidated pack list.

**Run with:** Sonnet 5.5, high effort. Required review (Opus 5.5).

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch main) --
"Starting context", Atlas "Phase A11" and "Phase E13". Never
launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase E13): close the Elysion half's asset set exactly as Atlas A11
did for its half and produce one consolidated pack list.

Done when: census --region elysion shows no missing assets or keys and the
pack list is in the doc. Record a Done note, then commit
on main.
```

*(2026-10-17, atlas A6 follow-up, user: the region-wide asset closure (census-named + mesh/xet-embedded textures, both halves) is staged and packed; see "Phase A6 — Done" and `june2017_re/atlas_a6_region_closure.tsv`. E13 re-runs that closure for files later phases add, then does the pack list / live check.)*

### Phase E13 — Done

*(empty)*

---

## Phase E14 — Build, deploy, hand-off

Same as Atlas A12, plus the Elysion reachability checklist.

**Run with:** Sonnet 5.5, medium effort. No review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md
(branch main) -- "Starting context", "Phase E14" and
"Verification (end to end, Elysion half)". Never launch X2_offline.exe
without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: medium.

Your task (Phase E14): build all four projects as US_SERVICE with tools/build.py --deploy (PCH touched), prove every Elysion-port flag's literal is in x2.exe,
deploy as X2_offline.exe (size/mtime checked; record the DEPLOY OK
commit), and hand me the pack list and the Elysion
reachability checklist.

Done when: deployed and handed off. Record a Done note,
then commit on main.
```

### Phase E14 — Done

*(empty)*

---

## Phase E15 — Play-test sweep + close-out (whole region)

Root-cause every checklist failure (short-lived `[ELYDBG]` diagnostics, scope
log checks to the phase's own lines); `MODS.md` entries for every flag;
troubleshooting rows; complete *Deferred to the quest plan*.

**Run with:** Opus 5.5, high effort. No review.

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch main) in
full, plus LANOX_TROUBLESHOOTING.md. Never launch X2_offline.exe without
asking. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase E15): I have packed and played the Elysion reachability
checklist; I'll paste results and logs. Root-cause each failure with
short-lived [ELYDBG] diagnostics, fix, rebuild, hand off again. Then close out
the whole region: MODS.md entries for every flag, troubleshooting rows, and
the "Deferred to the quest plan" section.

Done when: every checklist item passes or is recorded with a cause and owner.
Record a Done note, then commit on main.
```

### Phase E15 — Done

*(empty)*

---

## Verification (end to end, Atlas half)

Batched into one play-test after the A11 pack (each launch is an ask
round-trip). The **Atlas reachability checklist**:

- [ ] A level-80 save: the `[CAP99] migrated unit …` line appears once; level
      and progress bar look the same as before.
- [ ] `atlas` chat warp → Atlas Station renders (scene, houses, NPCs, music,
      loading image).
- [ ] Lanox Field 03 far edge → portal → Atlas Station; back through pos 401
      to Lanox.
- [ ] Atlas Station ↔ Field 01 ↔ Field 02: monsters spawn with HP, hit,
      die, drop; no `[ITEM_MISSING]` surprises; F02 has no dead exit.
- [ ] Cobo Express (B) and the warp NPC both list Atlas Station with icon +
      label; warping works from village and field.
- [ ] World map: rest + F01/F02 pins, cloud follows level; M-key field map
      follows the character.
- [ ] Local map page 2: Elysion tab at level ≥ 78 with Lanox Final Hall
      cleared; Atlas page shows AD/AC/TW with titles, images, stars.
- [ ] AD → AC → TW enter at N/H/E in unlock order; every stage loads; boss
      faces; clear pays EXP (incl. level 81+) and drops.
- [ ] A character levels from 80 into the 80s; stats rise (StatTable rows).
- [ ] Lanox `SECRET_SCENE_DUMMY` no longer errors (`SetCameraDistance`).
- [ ] `grep -E "UNHANDLED|EXCEPTION" offline_packets.log` — none new;
      `ErrorLog.txt` has no new `KEM_ERROR*` for region files.

---

## Verification (end to end, Elysion half)

Batched into one play-test after the E13 pack. The **Elysion reachability
checklist**:

- [ ] Field 02 → village → Field 02 round trip; `elysion` chat warp renders
      the village (scene, music, loading image).
- [ ] All village houses and NPCs present (no truncated `HouseIDList`); the
      birds/trees/Yuno render with their new house keys; in-village warps
      move the player after `WaitingTime`; NPC UI dialogs open; shop sells.
- [ ] Village → Field 03 → back; F03 monsters spawn, fight, drop.
- [ ] Cobo Express + warp NPC list the village (zone 18); world-map town and
      F03 pins; M-key map follows the character.
- [ ] Local map: scroll Atlas ↔ Elysion page; only the current page's
      buttons/titles/stars show; guide window opens and closes.
- [ ] SkyCrossroads → Adrian → Soless → Memory unlock in order and enter at
      N/H/E; every stage loads (Soless hidden stage, phase-2 world).
- [ ] Adrian/Soless moving platforms carry the player and stay in sync with a
      party/bot.
- [ ] Soless `AWAKEN`/`HERBAON` and Memory `OF_SUN` gauges show and update;
      dramatic messages display.
- [ ] Clears pay out (levels 83–99) and unlock the next line.
- [ ] `grep -E "UNHANDLED|EXCEPTION" offline_packets.log` — none new;
      `ErrorLog.txt` has no new `KEM_ERROR*` for region files.

## Out of scope (with the reason)

- **Hero tier** (`DI_*_HERO_*`, `DI_HERO_DUMMY_*`, `Static_HeroMode_Filter`):
  needs a `DL_HERO` engine change; June's Mode Button is unreachable even in
  June (`JUNE_UI_PARITY_PHASES.md` Phase 4 findings).
- **Seasonal variants** (`Elysion_Atlas_rest_Festival` + LineMap,
  `Field_Elysion_rest_luminarie`, `_XMAS`, `_XMAS_2`; village variants
  `Elysion_Village_Festival`, `_XMAS`, `_XMAS_2`): Lanox deferred its seasonal
  variants too.
- **Henir NPCs** (11): used only by June's Henir dungeon. **CSM cards** (10).
- **`WINNER_Adrian` PvP map** (757, tournament).
- **Dungeon lounge 30009, spa:** no content in June. **Field 04:** enum only.
- **Everything quest-related** → *Deferred to the quest plan*, including the
  quest data behind the quest-driven NPC swaps (the mechanism is ported in
  E1), the Elysion clear movie (quest 22336, `Movie_Elysion_Clear.avi`,
  `datai015` subtitles) and EventScene/GFx scenes (`Elysion.xml`,
  `GFXEventSceneInfo.xml`, `elysion*.swf`).

## Deferred to the quest plan

(Filled in further by A1a, A13 and E15.) Already measured:

- June keys kept verbatim and inert: `REQUIRE_COMPLETE_QUEST_ID` 22218 /
  `REQUIRE_COMPLETE_SUB_QUEST_ID` 22268 (`MapData:343-370`),
  `m_iRequireCompleteQuestID` 22218 (`DungeonData:29939`, parser
  `FUN_327e0f80`), `RequireCompletedQuestID` 22218 (local-map templet
  `+0x70`, parser `FUN_32501bb0` at `0x32502cf4`, tested via `FUN_32322260`),
  `BATTLE_FIELD_DIRECTION_INFO` / `NEED_ONGOING_QUEST_ID` (Lanox F04 row;
  March has no reader).
- *(A1a, 2026-10-16)* `FUN_32322260` is June's IsCompleteQuest (a map find on
  the quest manager; 17 callers). It reads the inert keys at these sites. Any port
  that adopts one of these functions leaves the test out with a comment (decision 2):
  - Local-map templet `+0x70`, read at five sites:
    - Cobo `324627f0 @32462fec` and warp NPC / item `32173120 @321738d2`, which skip
      the LMI_ELYSION rows (Atlas Station, Elysion); owners A5 / E2;
    - the local-map tab `3293c6f0 @3293cbe0`, which disables the tab; owner A7;
    - the world map `3225d340 @3225d734` and `3225d970 @3225dc93`; not ported by any
      phase today;
    - the dungeon-enable helper `327da190 @327da2d9`.
  - The DungeonData `+0x1cc` quest key (`m_iRequireCompleteQuestID`) is read at
    `327da190 @327da28b`.

  June's bulletin
  board village mapping (`GetBoardStartQuestList`) is `3232fd70` (called from
  `323357c0` for NPC 0x2729); it maps 1006 -> 20007, 1007 -> 20008 and 1008 -> 1008.
  The direction-info reader was not found in A1a's pass; it was not searched for
  (no deeper RE, step 6).
- Quest data: March's live `FieldQuest.lua` has no Lanox or Elysion quests.
  Chain: 22140 (Sander's last epic, present in March) → after-quest 22160
  "What happened in Lanox?" (start village 20008) → … 22209 → 22216 → 22217 →
  22218 "The end of the Elrios continent" → 22219 … 22257 (Atlas/Elysion
  epics) … 22336. Region side quests and `FieldSubQuest.lua` (22268).
  March's parser already reads the episode/scene/play-level keys
  (`X2QuestManager.cpp:127-132`); the offline emulator opens epics by
  after-quest and village-entry sweep (`X2OfflineQuest.cpp:996-1030`).
- Scenes: of 189 scene refs in quests 22140–22360, 117 are in June
  `EventScene.lua` (portable), 72 resolve nowhere (classify: dead in June too
  or elsewhere). Per-class `Quest_<id>_<CLASS>_End` scenes are GFx
  (`Elysion.xml` 37 scenes, `GFXEventSceneInfo.xml`, `elysion.swf`,
  `elysion7.swf`, ~596 GFx audio files) — March has no Scaleform runtime
  (user's answer: skip and log).
- Clear movies are quest-triggered: `DungeonClearMovieTemplet.lua`
  (`AddDungeonClearMovieTemplet_LUA`, unbound in March), Atlas `m_iQuestID`
  22236 → `Movie_Atlas_Clear.avi` (31 MB, SMC 10), Elysion 22336 →
  `Movie_Elysion_Clear.avi` (44 MB, SMC 11) + `datai015` subtitles.
- Quest-UI switches: `GetBoardStartQuestList` (`X2QuestManager.cpp:1144`, no
  Ranox case — bulletin board falls back to Ruben), `GetNPCFaceTextureKey`
  (`X2UIQuestNew.cpp:4126`).
- Old saves (user's answer): mark Lanox epic chain 22160–22217 complete for
  units that cleared `DI_RANOX_FINALHALL_NORMAL`; backup + ask.
- Village quest-house NPCs (user, 2026-10-16, A0): `NUI_STORY_QUEST` 10142 and
  `NUI_TRANSCEND_QUEST` 10153, used by `HI_STORY_QUEST` / `HI_TRANSCEND_QUEST`
  in the `VMI_ELYSION` and Atlas Station house lists. They are in neither
  Appendix A nor B; A3 does **not** add them and E2 leaves those two houses'
  NPC rows out with a comment. The census still lists them (`npc.ready`, line
  VIL/REST) as quest-plan items, not Atlas/Elysion gaps.
- *(A13, 2026-10-17: what the Atlas half learned)*
  - **Commented quest key.** AD Normal's `m_iRequireCompleteQuestID = 22218` is commented
    in data035 `DungeonData` as `-- quest plan:` (Hard / Expert carry no key). AD, AC
    and TW therefore open on level + `EXTRA_REQUIRE_DUNGEON_ID` alone (AD -> AC 33340 -> TW).
    The quest plan uncomments the key once 22218 can be completed offline.
  - **No quest gate on the local-map tab.** The Elysion tab opens at level 78: A7 found
    `RequireClearDungeonID` is not a tab gate in June, and the quest test (`@3293cbe0`) is
    left out by decision 2. Adopting the quest test closes the tab until 22218.
  - **Relation data waits on quest state.** E1a's `DLG_Map_House_NPC_RelationData.lua`
    rows are keyed by quest id (ByQuest `VRT_NOT_SHOW` / `VRT_CHANGE`), so the E1c runtime
    can only change Atlas Station / Elysion NPCs once the region's quests exist and can be
    completed offline (March's `FieldQuest.lua` has none, see above).
  - **E1b static list.** It already carries `HI_STORY_QUEST` / `HI_TRANSCEND_QUEST`
    (+40082, June's list), but their NPCs are not added (A0 decision above), so the houses
    stay empty until the quest plan adds the two NUIs.
  - **Rewards are not quest-owned.** Kill and clear EXP / ED / drops in Atlas fields and
    dungeons are 0 because the server DropTable has no rows past 40025 (backlog Stage 23),
    not because of any quest gate.

## Critical files

- **Lua (patches):** `march2014/data036/{Enum,DungeonEnum,MapData,BattleFieldData,ExpTable,EffectSet,DamageEffect}.src.lua`,
  `march2014/data035/DungeonData.src.lua`, `march2014/data037/NPCTemplet.src.lua`,
  `march2014/data040/WorldData.src.lua`,
  `march2014/data034/{DLG_Map_Village,DLG_Map_House,DLG_Map_World_Elios,DLG_Map_Local,DLG_UI_World_Map_NEW,DLG_UI_Local_Map_NEW,DLG_UI_PARTY_LENGTH_NEW,DLG_Destination}.src.lua`,
  `march2014/datai014/DLG_Destination_byButton.src.lua`.
- **Lua (new):** `march2014/update/data040/` scenes, `update/data035/` dungeon
  scripts, `update/data037/` unit scripts, `update/data036/Elysion_Field_0N`,
  `update/data034/` + `update/data217/` dialogs.
- **Server-side Lua the client loads (user packs into data036):**
  `KncWX2Server/ServerResource/US/{StatTable,NpcExpTable,MapData}.lua`.
- **C++:** `X2Lib/X2LocationManager.{h,cpp}`, `X2TFieldGame.cpp`,
  `X2WorldMapUI.cpp`, `X2FieldMapBase.cpp`, `X2StateMenu.cpp`,
  `X2UIInventory.cpp`, `X2LocalMapUI.{h,cpp}`, `X2Main.cpp` (bindings),
  `X2World.h`, `X2UnitManager.h`, `X2ChatBox.cpp`,
  `KncWX2Server/Common/Enum/{Enum,DungeonEnum}.h`,
  `KncWX2Server/Common/X2Data/XSLUnitManager.h`, `KTDXLIB/Always.h`.
- **Offline:** `X2Lib/Offline/{X2OfflineLuaEnumSeed.h,X2OfflineDB.{h,cpp},X2OfflineStatTable.{h,cpp},X2OfflineDropTable.cpp,Handlers_Room.cpp}`.
- **Lua (Elysion half):** `march2014/data034/DLG_Map_House_NPC_RelationData.src.lua`,
  `march2014/data036/{ShopItem,PVP_Map_List}.src.lua`,
  `march2014/update/data034/{DLG_Elysion_LocalMap_Guide,DLG_UI_ELYSION_SOLESS_*_GAGE,DLG_UI_FIELD_MAP_Elysion_*}.lua`,
  `march2014/update/data039/UI_*_ELYSION.lua`.
- **C++ (Elysion half):** `X2Lib/X2TFieldNpc*.cpp`, `X2Lib/X2LocationManager.cpp`
  (`HouseParsing`, `VillageParsing`), `X2Lib/X2QuestManager.cpp` (relation
  binding), `X2Lib/X2LocalMapUI.{h,cpp}`, `KTDXLIB/KTDGLineMap.{h,cpp}`,
  `X2Lib/X2World.cpp` (triggers), `X2Lib/X2WorldManager.cpp`
  (`AddWorldData_LUA`), `X2Lib/X2GUNPC.cpp` (dramatic messages),
  `X2Lib/X2Main.cpp` (bindings), the SimpleDLG owner A1b identifies.
- **Tools:** `june2017_re/{region_census.py,region_reach.tsv,region_engine.tsv,cap99_gen.py,funcmap.tsv}`,
  `june2017_re/june_lua/ELYSION_SOLESS.src.lua`.

---

## Appendix A — Atlas-half and shared NPC_UNIT_IDs (June values)

- **AtlasDevelopment (11):** `ELSION_PLINGCHU_G` 2092, `_R` 2093,
  `PYURITA` 2094, `BERMEVA` 2095, `SHILEDPIM` 2096, `PIMPIM` 2097,
  `DEKALPYURITA` 2098, `DEKAL` 2099, `SUMMON1` 2124, `EVENT_1ST_BOSS` 2125,
  `EVENT_DISION` 2126.
- **AtlasCity (7):** `ELSION_DEFENDER` 2116, `NEWDEFENDER` 2119,
  `SUVEILLAN` 2120, `SUMMON2` 2121, `STAGE_SUVEILLAN` 2122,
  `EVENT_2ST_BOSS` 2123, `ATLAS_GATE` 2169.
- **Tower (10):** `ELSION_TEMPLAR` 2150, `LIBYA` 2151, `PUTER` 2152,
  `HIGHTTEM` 2153, `DISION` 2154, `MAYA` 2155, `EVENT_3ST_BOSS` 2156,
  `EVENT_YUNO` 2157, `TOWER_GATE` 2170, `SUMMON3` 2177.
- **AC + TW:** `ELSION_PROVER` 2115, `SILENCE` 2117, `DECEPTOR` 2118,
  `SUMMUN_PROVER` 2128 (script `ELSION_PROVER`).
- **AD + AC + TW:** `ELSION_PROTO` 2114, `SUMMUN_PROTO` 2127 (script
  `ELSION_PROTO`); with fields also `ELSION_CHEST` 2176.
- **Fields (23):** `ELSION_F_{PLINGCHU_G 2129, PLINGCHU_R 2130, PYURITA 2131,
  BERMEVA 2132, SHILEDPIM 2133, PIMPIM 2134, DEKALPYURITA 2135, PROTO 2158,
  PROVER 2159, DEFENDER 2160, DECEPTOR 2161, NEWDEFENDER 2162, LIBYA 2163,
  PUTER 2164, HIGHTTEM 2165, TEMPLAR 2173}`; `ELYSION_F_HERNA_{SHIELD 2342,
  ORB 2343, BOW 2344, LANCE 2345, FEAR 2346}`; `ELYSION_F_BOSS_ORB` 2347,
  `ELYSION_F_BOSS_SHIELD` 2348; plus `ELYSION_HERNA_SHIELD` 2259 and
  `ELYSION_NEW_CHEST` 2332 (shared with Elysion lines).
- **Lanox-field cutscene (enum only):** `ELSION_EVENT_00` 2139, `_01` 2140.
- Elysion-half NUIs (SC/AR/SL/ME/village): Appendix B. A3 adds all of them.

## Appendix B — Elysion-half NPC_UNIT_IDs (June values)

- **SkyCrossroads (15):** `ELYSION_HERNA_ORB` 2260, `HERNA_BOW` 2261,
  `HERNA_LANCE` 2262, `HERNA_FEAR` 2263, `BOSS_ORB` 2264, `BOSS_SHIELD` 2265,
  `KEYOBJECT` 2266, `FAKE_OBJECT` 2267, `MOVE_OBJECT` 2268,
  `KEYOBJECT_TRIGGER` 2295, `WATERCANNON_OBJECT` 2296 (file extension is
  uppercase `.LUA`), `SKYCROSS_EVENT0` 2297, `SKYCROSS_BOSS_EVENT` 2298,
  `SKY_DUMMY` 2323, `DUMMY_TITLE` 2333.
- **Shared:** `ELYSION_HERNA_SHIELD` 2259 (SC + fields); `WALL_DUMMY1` 2331
  (SC + AR); `NEW_CHEST` 2332 (SC + AR + SL + fields); `EMPEROR_SHIELD` 2299,
  `EMPEROR_ORB` 2300 (AR + SL).
- **Adrian (34):** `ELYSION_HAMMER_PANG` 2271, `SING_PANG` 2272, `BOMB_PANG`
  2273, `PUPANG` 2274, `NEW_DEKAL` 2275, `BOSS_HERBAON` 2276, `DUMMY_HERBAON`
  2277, `L_H_LASER` 2278, `L_V_LASER` 2279, `R_H_LASER` 2280, `R_V_LASER`
  2281, `EMPEROR_BOW` 2301, `EMPEROR_LANCE` 2302, `2STAGE_LASER_A/B/C`
  2304–2306, `3STAGE_LASER_A/B/C` 2307–2309, `WALL_DAMAGE` 2310,
  `DAMAGE_OBJECT` 2311, `GAUGE_OBJECT` 2312, `GAUGR_DUMMY` 2313,
  `STAIRS_DUMMY` 2314, `ADRIAN_GATE` 2315, `ADRIAN_EVENT1` 2316,
  `ADRIAN_EVENT2` 2317, `ADRIAN_BOSS_EVENT` 2318, `OBJECT_HERBAON1` 2319,
  `OBJECT_HERBAON2` 2320, `ADRIAN_EVENT3` 2324, `3STAGE_BACK` 2328,
  `WALL_DUMMY0` 2330, `DAMAGE_OBJECT_STAGE2_SUB_STAGE2` 2335.
- **Soless (45):** `ELYSION_IMPRINT_SHIELD/ORB/BOW/LANCE` 2349–2352,
  `COMMANDER` 2353, `DIA` 2354, `SPHERE` 2355, `S_IMPRINT_SHIELD/ORB/BOW/LANCE`
  2356–2359, `S_COMMANDER` 2360, `S_DIA` 2361, `S_SPHERE` 2362, **`SOLESS`
  2363**, `SOLESS_EVENT1` 2364, `SOLESS_EVENT1_A` 2365, `SOLESS_DUMMY` 2368,
  `S_MAYA` 2369, `S_0STAGE_LASER1-4` 2370–2373, `S_DAMAGE_OBJECT` 2374,
  `S_CIRCLE_OBJECT` 2375, `S_CIRCLE_CTRL` 2376, `S_WALL_OBJECT` 2377,
  `S_COMMAND_3STAGE` 2378, `S_SKILL_3STAGE` 2379, `S_COMMAND_BOSS` 2380,
  `S_SKILL_BOSS` 2381, `S_3STAGE_OBJECT` 2382, `S_2STAGE_HERBAON` 2383,
  `S_DISION_OBJECT` 2384, `S_2STAGE_SUVEILLAN` 2385, `S_3STAGE_END` 2386,
  `BOSS_LINE_A` 2387, `HIDE_STAGE_A` 2388, `HIDE_STAGE_B` 2389,
  `S_0STAGE_GATE` 2390, `S_1STAGE_END` 2391, `S_0STAGE_START` 2392,
  `S_2STAGE_END` 2402, `IMPRINT_DYING` 2403, `SOLESS_DRAMATIC` 2404.
- **Memory (30):** `ELYSION_M_S_KNIGHT/LANCE/SWORD/MAGE` 2405–2408,
  `M_EL_KNIGHT/LANCE/MAGE` 2409–2411, `M_MEMORY_1..5` 2412–2416, `M_SOLESS`
  2417, `M_BOSS_EVENT` 2418, `M_0STAGE_START` 2419, `M_0STAGE_GATE` 2420,
  `M_1STAGE_START` 2421, `M_1STAGE_GATE` 2422, `M_3STAGE_START` 2423,
  `M_EL_DEF` 2424, `M_2STAGE_SUMMON` 2427,
  `M_S_{KNIGHT,SWORD,LANCE,MAGE}_SUMMON` 2428–2431, `M_0STAGE_GATE_2` 2432,
  `M_0STAGE_GATE_3` 2433, `M_1STAGE_START_1` 2434, `M_2STAGE_END` 2435,
  `M_3STAGE_END` 2436.
- **Village (12, no unit scripts):** `YUNO` 10078, `BERNARD` 10143,
  `DURANDA` 10144, `THEODOR` 10145, `HUGO` 10146, `ELYSION_QUEST_BIRD0-4`
  10148–10152, `ELYSION_RED_TREE` 10194, `ELYSION_BLUE_TREE` 10195.
- Reused Lanox NUIs in these lines: `SECRET_MAG_NOM` 2220 (AR),
  `SECRET_MAG_CHARGER` 2221, `SECRET_SUMMON_MAG_NOM` 2227,
  `SECRET_MAGI_OBJECT` 2249, `SECRET_LAVA_DUMMY` 2248 (SL).
