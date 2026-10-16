# Atlas/Elysion Port (Atlas Station, Fields 01–03, Elysion village, AtlasDevelopment / AtlasCity / Tower / SkyCrossroads / Adrian / Soless / Memory) — phased plan

> Written 2026-10-03 (read-only research session) as two docs, this one and
> `ELYSION_PORT_PHASES.md`, committed on `mods/june-buff-port`. **Merged into
> this one plan on 2026-10-16 (user).** Phase IDs are unchanged: **A0–A13**
> are the shared groundwork and the Atlas half, **E0–E15** the Elysion half.
> Older notes, `funcmap.tsv` rows and code comments that cite
> `ELYSION_PORT_PHASES.md` mean this doc's E sections and Elysion-half ledger;
> that file is now a pointer here.
>
> **No worktree (user, 2026-10-16).** Phase A0 creates branch
> `mods/elysion-port` in the main Trunk, and every phase works there. Start with
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
3. **Branch** `mods/elysion-port` off the committed `mods/june-buff-port`
   tip, created **in the main Trunk** by A0. **No worktree** (user,
   2026-10-16): the plan works on the new branch in the same tree. Phases
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
| A0 | Branch, docs, `region_census.py` v1 | – | tooling | Sonnet 5.5 | high | light (Opus 5.5) |
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

- **Branch** (all edits, builds and commits): **`mods/elysion-port`**, checked
  out in the main Trunk
  `F:\elsword stuff\elsword_2014\els_2014\ElswordFiles\ready_and_built\ProjectX2_SVN\ProjectX2_SVN\source\EU_CN_US\Trunk`
  (created in A0).
  - Every session in this Trunk works on whatever branch is checked out. A
    session of another plan started while this branch is checked out commits
    onto it.
  - Switch branches only on a clean tree: never `git switch`, checkout or
    stash while another session has uncommitted changes. Ask first.
  - At each phase start, if `mods/june-buff-port` has moved, merge it into this
    branch (`git merge mods/june-buff-port`). Do not merge the other way before
    the plan closes.
  - This plan's `STATUS.md` rows are committed on this branch, in the phase's
    own commit (x2-phase *Finish*).
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
- **Deploy:** `build.py` deploys as `$DATA/X2_offline.exe`. **Record in the
  phase's Done note that the deployed exe is the `mods/elysion-port` build** —
  a build from any other branch deploys to the same name, last deploy wins.
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
| `WORLD_ID` | 41 | shared: village 636, rest 637, F01–F04 638–641, PvP 752 (`WINNER_ADRIAN` 757 deferred); AD 642–645; AC 646–649; TW 650–654 (S3 → missing file); SC 672–675; AR 706–710; SL 711–716; ME 727–731 |
| `VILLAGE_MAP_ID` | 7 | rest 1008, village 20009, lounge 30009, F01–F04 40035–40038 |
| `LOCAL_MAP_ID` | 1 | `LMI_ELYSION` 10009 (free in `X2LocationManager.h:141-145`) |
| `HOUSE_ID` | 12 | `HI_YUNO` 40092, `BERNARD`/`DURANDA`/`THEODOR`/`HUGO` 40131–40134, `BIRD_QUEST_00..04` 40135–40139, `ELYSION_RED/BLUE_TREE` 40145–40146 |
| `DUNGEON_ID` | 21 | AD 33330–2, AC 33340–2, TW 33350–2, SC 33870–2, AR 33880–2, SL 33890–2, ME 33900–2 (Hero rows out of scope) |
| `NPC_UNIT_ID` | 203 | 201 region NPCs + `NUI_ELSION_EVENT_00/01` (2139/2140). Full per-line lists: Appendix A |

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
settle it); `CX2BossRaidManager::GetFieldOpenTextureName` / `GetCameraIndex`
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
- **Unbound Lua names: 65** (53 from decompiled region scripts, 9 inferred
  from `ELYSION_SOLESS` bytecode, 3 registry/UI). 8 already have a C++ method
  ("(b)": binding only), 57 need new C++. Method: every string on
  `lua_tinker`/`class_def` lines in X2Lib/KTDXLIB/X2 (1716 names), calibrated
  (`SetFlag_LUA`, `SetForceInvincible` bound; `GetStoredGUUser` unbound).
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

### Assets

- Reference count from region Lua + registry rows: **≥ 2,071 files to
  adopt** (2,383 refs; 249 already in March by name; 63 not found in June's
  extract). By archive: data180 912, data182 422, data197 179, data181 144,
  data096 90, data002 78, data104 77, data057 29, music 23, data013 21, …
  This is a **lower bound** — mesh-internal textures and particle textures
  were not followed (Lanox's Burned Forest smog miss).
- **393 effect-set templets** are referenced; the staged
  `march2014/data036/EffectSet.src.lua` and `MK/data036_kom/EffectSet.lua` both
  contain **0** Elysion entries. Damage-effect children not measured.
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

## Phase A0 — Branch, docs, census v1

**Steps**
1. **Branch, in the main Trunk** (user 2026-10-16). `git status` must be
   clean, and no other session may have uncommitted work. If it is
   not, **stop and ask**. Never stash, add or commit another session's files.
   Then cut the branch from the **committed** tip:
   `git switch -c mods/elysion-port mods/june-buff-port`.
   That tip carries june-item-kim K2 (`f1549a6`), so this branch's exe reads
   the live `data036` `item.kim` correctly (STATUS "Branches and the game dir").
2. Confirm this doc is on the branch. It is the single plan;
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

Your task (Phase A0): in the main Trunk, check that git status
is clean and that no other session has uncommitted work -- if not, stop and
ask; never stash or commit someone else's files. Create branch
mods/elysion-port from the committed mods/june-buff-port tip
(git switch -c mods/elysion-port mods/june-buff-port). Then write
june2017_re/region_census.py (v1 = the cheap checks listed in Phase A0,
reusing assets.py / buff_census.py / flagprobe.py) and record its baseline in
the doc.

Premises to verify first: that the ledger's ID counts and the 0-collision
claim reproduce; that the staged EffectSet.lua really has no Elysion entries.

Done when: mods/elysion-port is checked out with this doc on it; census
baseline recorded and matching the ledger (or the ledger corrected in place).
Record a "Phase A0 -- Done (date)" note, then commit on mods/elysion-port,
with this plan's STATUS rows in the same commit.
```

### Phase A0 — Done

*(empty)*

---

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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) in full -- especially "Starting
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
an "A1a reachability summary" is in the doc. Record a Done note, then commit on mods/elysion-port.
```

### Phase A1a — Done

*(empty)*

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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) in full -- especially "Starting
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
note, then commit on mods/elysion-port.
```

### Phase A1b — Done

*(empty)*

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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) in full -- "User decisions" 4-5,
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
kills-per-level table), then commit on mods/elysion-port.
```

### Phase A2 — Done

*(empty)*

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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) in full -- "Starting context"
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

Done when: Phase A3's "Done when" holds. Record a Done note, then commit on mods/elysion-port.
```

### Phase A3 — Done

*(empty)*

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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) in full -- "Starting context", the
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

Done when: Phase A4's "Done when" holds. Record a Done note, then commit on mods/elysion-port.
```

### Phase A4 — Done

*(empty)*

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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) in full -- "Starting context", the
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
after I pack). Record a Done note (incl. which branch's exe is deployed), then commit on mods/elysion-port.
```

### Phase A5 — Done

*(empty)*

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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) in full -- "Starting context", the
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

Done when: Phase A6's "Done when" holds. Record a Done note (deployed branch), then commit on mods/elysion-port.
```

### Phase A6 — Done

*(empty)*

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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) in full -- "Starting context", the
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

Done when: Phase A7's "Done when" holds. Record a Done note (deployed branch), then commit on mods/elysion-port.
```

### Phase A7 — Done

*(empty)*

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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) in full -- "Starting context", the
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
Record a Done note (deployed branch), then commit on
mods/elysion-port.
```

### Per-line checklist

*(written by Phase A8)*

### Phase A8 — Done

*(empty)*

---

## Phase A9 — AtlasCity line

Follow the *Per-line checklist*. Line-specific: `NUI_ELSION_ATLAS_GATE`,
`IsActiveMonster`(b) (A4), reused `NUI_FIRETEMPLE_GATE`; DungeonData from
line `30516`.

**Run with:** Sonnet 5.5, high effort **if** `region_census.py --line AC`
shows zero open engine gaps; otherwise Opus 5.5, high. Required review (Opus
5.5).

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) -- "Starting context", the
"Per-line checklist", Appendix A (AC NUIs) and "Phase A9". Never launch
X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high. If the census shows an engine gap the A1b map doesn't settle,
stop on it and log it for an Opus pass -- do not guess.

Your task (Phase A9): port the AtlasCity line by the Per-line checklist.

Done when: census --line AC is clean, staged with manifests, pack list handed
over (live-kom check after I pack). Record a Done note (deployed branch), then commit on mods/elysion-port.
```

### Phase A9 — Done

*(empty)*

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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) -- "Starting context", the
"Per-line checklist", the buff-dependency rule, Appendix A (TW NUIs) and
"Phase A10". Check JUNE_BUFF_PORT_PHASES.md for Phase 10's status first.
Never launch X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high. If JUNE_BUFF Phase 10 (SetMonsterBuffGroupID / BUFF_GROUP_ID)
hasn't landed, record the block and stop.

Your task (Phase A10): port the Tower line by the Per-line checklist,
transcribing June's WI_TOWER_STAGE3 bug verbatim with a log line.

Done when: census --line TW is clean, staged with manifests, pack list handed
over. Record a Done note (deployed branch), then commit on
mods/elysion-port.
```

### Phase A10 — Done

*(empty)*

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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) -- "Starting context", the ledger's
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
mods/elysion-port.
```

### Phase A11 — Done

*(empty)*

---

## Phase A12 — Build, deploy, hand-off

**Steps:** full four-project build (PCH touched), literal check for every
Atlas flag, deploy as `X2_offline.exe` (size/mtime check, record the
branch), confirm every staged file is in the pack list, hand off. Write the
**Atlas reachability checklist** (below) into the hand-off. Ask before any
launch.

**Run with:** Sonnet 5.5, medium effort. No review.

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) -- "Starting context", "Phase A12"
and "Verification". Never launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: medium.

Your task (Phase A12): build all four projects as US_SERVICE with tools/build.py --deploy (PCH touched), prove every Atlas-port flag's literal is in x2.exe,
deploy as X2_offline.exe (check size/mtime; record that it is the
mods/elysion-port build), and hand me the consolidated pack list and the
Atlas reachability checklist.

Done when: deployed and handed off. Record a Done note,
then commit on mods/elysion-port.
```

### Phase A12 — Done

*(empty)*

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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) in full, plus LANOX_TROUBLESHOOTING.md.
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
mods/elysion-port.
```

### Phase A13 — Done

*(empty)*

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
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) --
"Starting context", the ledger's "Elysion half" sections and "Phase E0". Never
launch X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline; tell subagents the same.

Effort: medium.

Your task (Phase E0): confirm the Atlas phases E0 depends on are Done; decompile
14JUN17/data/data037_kom/ELYSION_SOLESS.lua with elstools decompile-batch +
june2017_re/ljd_jun2017.py --check into june2017_re/june_lua/; confirm its
string-only engine names and register any new one in region_engine.tsv; record
the region_census.py --region elysion baseline.

Done when: decompile passes --check; baseline recorded. Record a Done note, then commit on mods/elysion-port.
```

### Phase E0 — Done

*(empty)*

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
     block on `mods/june-buff-port`.

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
     `+0xd8` riding-pet branches (`X2EffectSet.cpp`): merge
     `mods/june-buff-port` after C9h-3a lands before porting them.
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
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) --
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

Done when: Phase E1's "Done when" holds. Record a Done note (deployed branch), then commit on mods/elysion-port.
```

### Phase E1 — Done

*(empty)*

---

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
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) --
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

Done when: Phase E2's "Done when" holds. Record a Done note (deployed branch), then commit on mods/elysion-port.
```

### Phase E2 — Done

*(empty)*

---

## Phase E3 — Field 03 (+ Field 04 check)

Follow Atlas A6's field steps for Field 03 (`Field_Elysion_Stage2` + LineMap,
`Elysion_Field_03.lua`, WI 640, field-map `DLG_UI_FIELD_MAP_Elysion_03_NEW`,
pin, monsters, effects, drops). Confirm Field 04 (40038/641) is enum-only in
June (no row, scene or dialog) and leave it at that.

**Run with:** Sonnet 5.5, high effort; required Opus 5.5 review; escalate to
Opus on any engine gap the A1b map doesn't settle.

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) --
"Starting context", Atlas "Phase A6" and "Phase E3". Never
launch X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high. Stop on any engine gap the A1b map doesn't settle and log it
for an Opus pass.

Your task (Phase E3): port Field 03 the way Atlas A6 ported Fields 01/02, and
confirm Field 04 is enum-only.

Done when: census --line FLD (F03) is clean, staged with manifests, pack list
handed over. Record a Done note, then commit on
mods/elysion-port.
```

### Phase E3 — Done

*(empty)*

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
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) --
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

Done when: Phase E4's "Done when" holds. Record a Done note (deployed branch), then commit on mods/elysion-port.
```

### Phase E4 — Done

*(empty)*

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
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) --
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

Done when: Phase E5's "Done when" holds. Record a Done note, then commit on mods/elysion-port.
```

### Phase E5 — Done

*(empty)*

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
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) --
"Starting context", the ledger's "Elysion half: engine gaps" section, the A1b rows owned
by E6, and "Phase E6". Use the ghidra skill. Never launch X2_offline.exe
without asking. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase E6): port June's SimpleDLG subsystem (behind
SERV_IRUHADEV_JUNE_SIMPLE_DLG) and dramatic-message system (behind
SERV_IRUHADEV_JUNE_DRAMATIC_MESSAGE) whole, with June addresses cited; stage
the three Soless/Memory gauge dialogs and their TET atlases; note whether
Lanox's deferred SimpleDLG uses now work.

Done when: Phase E6's "Done when" holds. Record a Done note, then commit on mods/elysion-port.
```

### Phase E6 — Done

*(empty)*

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
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) --
"Starting context", the ledger's "Elysion half: engine gaps" section, the A1b rows owned
by E7, Atlas "Phase A4" and its Done note, and "Phase E7". Use the ghidra
skill. Never launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase E7): implement every binding A1b assigned to E7 behind
SERV_IRUHADEV_ELYSION_SCRIPT_B (receiver class verified, June address cited,
reusing A4's), plus the additional-world support (dataFileName_Additional,
SetShowAdditionalWorld, PlayAdditionalWorldAllSound).

Done when: Phase E7's "Done when" holds. Record a Done note, then commit on mods/elysion-port.
```

### Phase E7 — Done

*(empty)*

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
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) --
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
(date)" note (deployed branch), then commit on
mods/elysion-port.
```

#### Phase E8 — Done

*(empty)*

### Phase E9 — Adrian

**Run with:** Sonnet 5.5, high effort **if** `region_census.py --line AR` shows zero open engine gaps; otherwise Opus 5.5, high. Required review (Opus 5.5).

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) --
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
(date)" note (deployed branch), then commit on
mods/elysion-port.
```

#### Phase E9 — Done

*(empty)*

### Phase E10 — Soless

**Run with:** Opus 5.5, high effort. Required review (Opus 5.5).

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) --
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
(date)" note (deployed branch), then commit on
mods/elysion-port.
```

#### Phase E10 — Done

*(empty)*

### Phase E11 — Memory

**Run with:** Opus 5.5, high effort. Required review (Opus 5.5). **Blocked on JUNE_BUFF Phase 10.**

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) --
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
(date)" note (deployed branch), then commit on
mods/elysion-port.
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
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) --
"Starting context", RANOX_PORT_PLAN.md Phase 11 findings, and
"Phase E12". Never launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase E12): port the Elysion field PvP map (WI 752,
MAP_SELECT_ELISION) the way Lanox Phase 11 ported GateWay.

Done when: staged with manifests, pack list handed over. Record a Done note, then commit on mods/elysion-port.
```

### Phase E12 — Done

*(empty)*

---

## Phase E13 — Asset closure + pack list (Elysion half)

Same method as Atlas A11, over everything E1–E12 staged (mesh-internal
textures, TET keys, truncation, mount order); one consolidated pack list.

**Run with:** Sonnet 5.5, high effort. Required review (Opus 5.5).

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) --
"Starting context", Atlas "Phase A11" and "Phase E13". Never
launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase E13): close the Elysion half's asset set exactly as Atlas A11
did for its half and produce one consolidated pack list.

Done when: census --region elysion shows no missing assets or keys and the
pack list is in the doc. Record a Done note, then commit
on mods/elysion-port.
```

### Phase E13 — Done

*(empty)*

---

## Phase E14 — Build, deploy, hand-off

Same as Atlas A12, plus the Elysion reachability checklist.

**Run with:** Sonnet 5.5, medium effort. No review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ATLAS_PORT_PHASES.md
(branch mods/elysion-port) -- "Starting context", "Phase E14" and
"Verification (end to end, Elysion half)". Never launch X2_offline.exe
without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: medium.

Your task (Phase E14): build all four projects as US_SERVICE with tools/build.py --deploy (PCH touched), prove every Elysion-port flag's literal is in x2.exe,
deploy as X2_offline.exe (size/mtime checked; record that it is the
mods/elysion-port build), and hand me the pack list and the Elysion
reachability checklist.

Done when: deployed and handed off. Record a Done note,
then commit on mods/elysion-port.
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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (branch mods/elysion-port) in
full, plus LANOX_TROUBLESHOOTING.md. Never launch X2_offline.exe without
asking. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase E15): I have packed and played the Elysion reachability
checklist; I'll paste results and logs. Root-cause each failure with
short-lived [ELYDBG] diagnostics, fix, rebuild, hand off again. Then close out
the whole region: MODS.md entries for every flag, troubleshooting rows, and
the "Deferred to the quest plan" section.

Done when: every checklist item passes or is recorded with a cause and owner.
Record a Done note, then commit on mods/elysion-port.
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
