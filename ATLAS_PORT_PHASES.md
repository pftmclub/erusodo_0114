# Atlas Port (Atlas Station, Fields 01–02, AtlasDevelopment / AtlasCity / Tower) — phased plan

> Written 2026-10-03, read-only research session, and committed on
> `mods/june-buff-port` (only these two docs; the buff session's working-tree
> edits were left alone). Phase A0 cuts `mods/elysion-port` from that tip, so
> the worktree already contains both docs. Start with Phase A0's kickoff
> prompt in a new conversation.
>
> Companion doc: `ELYSION_PORT_PHASES.md` (village, Field 03, SkyCrossroads /
> Adrian / Soless / Memory). **This doc owns all shared groundwork** (census,
> RE maps, level cap, enums for both halves); the Elysion doc assumes it.

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
The two docs follow that split.

Intended outcome of this doc:
- A character standing in Lanox Field 03 walks through the portal into
  **Atlas Station** (`VMI_BATTLE_FIELD_ELYSION_REST_00` = 1008), reaches
  **Fields 01 and 02**, sees the **Elysion tab** on the local map, and can
  enter and clear **AtlasDevelopment, AtlasCity and Tower** at
  Normal/Hard/Expert.
- Characters can level **past 80, up to 99**.
- Everything is reachable through the same routes as every other region
  (world map, Cobo Express, warp NPC, field portals), with no "found in
  play-test" switch misses.

Naming: in this doc **"Atlas"** means the region half (Atlas Station / the
Atlas page). Texture atlases are always written **"TET atlas"**, because
`*atlas*` greps also hit texture atlases.

---

## User decisions (2026-10-03)

1. **Split by June's own halves.** This doc: shared groundwork + Atlas Station
   + Fields 01–02 + AtlasDevelopment, AtlasCity, Tower. Elysion doc: village
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
3. **Branch** `mods/elysion-port` off the current `mods/june-buff-port` tip,
   in its own worktree (A0). Phases needing June buff types are marked
   blocked on the named buff phase.
4. **Level cap 99 in this port** (A2). The user **explicitly authorises
   inventing StatTable rows 81–99** — this overrides CLAUDE.md's no-invented-
   table rule for that table — and leaves the method to us. **NpcExpTable
   81–99 is invented too.**
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
| A0 | Worktree, docs, `region_census.py` v1 | – | tooling | Sonnet 5.5 | high | light (Opus 5.5) |
| A1a | RE + C++ reachability map (both halves) | A0 | Ghidra + C++ read-only | Opus 5.5 | **xhigh** | none (self-check vs census) |
| A1b | Script-engine map (both halves) | A0 | Ghidra read-only | Opus 5.5 | **xhigh** | none |
| A2 | Level cap 99 + EXP migration | A0 | C++ + data + offline DB | Opus 5.5 | high | required |
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

A1a and A1b are independent (two sessions in parallel is fine). A5 and A7 are
independent of each other and of A4.

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
7. Record each session's cost in the phase's "Done" note (`/cost` on API
   billing, usage/limit consumption on a subscription).

---

## Starting context (paste with any phase)

- **Worktree** (all edits, builds and commits):
  `F:\elsword stuff\elsword_2014\els_2014\ElswordFiles\ready_and_built\ProjectX2_SVN\ProjectX2_SVN\source\EU_CN_US\Trunk_elysion`
  on branch **`mods/elysion-port`** (created in A0). The main Trunk next to it
  belongs to the buff session — never checkout, stash, add or commit there.
- **Read-only shared inputs stay in the main Trunk, by absolute path:**
  `…\EU_CN_US\Trunk\june_src_fixed\`, `…\Trunk\elstools\elstools.exe`.
  (`june2017_re/` is tracked, so the worktree has its own copy.)
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
- **Lua compile/stage:**
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
- **Build** (exactly as `CLAUDE.md` *Toolchains*, from the worktree):
  ```sh
  MSBUILD="/c/Windows/Microsoft.NET/Framework/v4.0.30319/MSBuild.exe"
  WT="F:/elsword stuff/elsword_2014/els_2014/ElswordFiles/ready_and_built/ProjectX2_SVN/ProjectX2_SVN/source/EU_CN_US/Trunk_elysion"
  MP="-m -p:MultiProcessorCompilation=true"
  "$MSBUILD" X2ServerProtocol/X2ServerProtocol_2010.vcxproj -p:Configuration=X2TOOL     -p:Platform=Win32 "-p:SolutionDir=$WT/" $MP
  "$MSBUILD" KTDXLIB/KTDXLIB_2010.vcxproj                   -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$WT/" $MP
  "$MSBUILD" X2Lib/X2Lib_2010.vcxproj                       -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$WT/" $MP
  "$MSBUILD" X2/X2_2010.vcxproj                             -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$WT/" $MP
  ```
  After any header edit, `touch KTDXLIB/stdafx.cpp X2Lib/stdafx.cpp`. Prove
  new code compiled in by finding a UTF-16LE literal that exists only inside
  the new `#ifdef` in `X2/US_SERVICE/x2.exe` — exit code 0 is not proof.
- **Deploy:** copy `X2/US_SERVICE/x2.exe` to `$DATA/X2_offline.exe`, confirm
  size + mtime programmatically, and **record in the phase's Done note that
  the deployed exe is the `mods/elysion-port` build** — the buff session
  deploys to the same name, last deploy wins. **Ask before every launch.**
- **Logs:** load time → `ErrorLog.txt`, `OpenScriptFile.log`; runtime →
  `offline_server.log` (`CX2OfflineLog::Server`) and `offline_packets.log`.
  `CX2OfflineLog::Server` is **not open yet** during `CX2LocationManager`
  parsing and `CX2UnitManager` loading — use `ErrorLogMsg` there. `KTDXLIB`
  cannot reach `offline_server.log` at all. New lines are prefixed `[ELYDBG]`
  (debug-only, behind `SERV_IRUHADEV_ELYSION_DEBUG`) or `[CAP99]` /
  `[ITEM_MISSING]` (permanent). **A clean log is not evidence of success**
  (`LANOX_TROUBLESHOOTING.md:186-189`).
- **Debug warp:** `SERV_IRUHADEV_CHAT_WARP_DEBUG` (`X2Lib/X2ChatBox.cpp:1064`)
  gains `atlas` (→ 1008) in A5 and `elysion` (→ 20009) in the Elysion doc.
- **Do not consult** `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md`
  or the `mods/lanox-offline` branch (a failed prior attempt). **Repeat this
  to every subagent** — one Lanox subagent read it anyway.
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
| Field 02 (40036) | L0 → F01 pos 365; L142 → `VMI_ELYSION` pos 500 (Elysion doc) | `:2104-2157` |

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

- **Local map** (owned by the Elysion doc, E4): scroll `FUN_329378a0`,
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
  - **Atlas half (this doc, A4):** `GetCenterPositionLineIndex`,
    `GetDyingStart`(b), `GetWorldObjectMeshByName`, `SetMovieDescriptionData`
    (in-dungeon boss-event subtitles, e.g. `ELSION_EVENT_1ST_BOSS.src.lua:159`),
    `IsActiveMonster`(b, `X2GUNPC.h:2155`), `FadeOriginWorldColor_LUA`,
    `GetAliveActiveNPCCount`(b, `X2Game.h:521`), `LiveKeyCodeNPCNumExceptDying`,
    `SetImmuneToExtraDamage`; `SetMonsterBuffGroupID` (**owned by JUNE_BUFF
    Phase 10**); linemap key `CHANGE_LINE_TYPE_AFTER_ALL_ACTIVE_NPC_KILLED`
    (`Elysion_Tower_stage1_LineMap:909`). Local map: `GetNationFlag`
    (`Icon_NEW:320`; March binds only `SetNationFlag`, `X2Main.cpp:3343` —
    **hard-aborts the chunk**), `SetChangeUVPoint` (`Back_NEW:15`).
  - **Elysion half** (Elysion doc E5–E7): moving platforms, SimpleDLG gauges,
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
`4942-4944` (`HI_GUILD_MARK` village row, Elysion doc E2);
`data034/DLG_Map_Local.src.lua:12`, `DLG_Map_World_Elios.src.lua:14`,
`DLG_UI_Local_Map_NEW.src.lua:532-534`, `713` (`LocalMapRadioBtn9`, A7);
`DLG_UI_PARTY_LENGTH_NEW.src.lua:272-277` (picture 9, A7);
`DLG_UI_World_Map_NEW.src.lua:2293-2316` (cloud `AddDummyInt(99999)` at
`2315`, A5); `data036/Enum.src.lua:4978`, `6618` (A3); `MapData.src.lua:453`
(A5); `data040/WorldData.src.lua:3034` (A5); `PVP_Map_List.src.lua:116-117`
(Elysion doc E12).

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
| `SERV_IRUHADEV_ELYSION_SCRIPT_B` | Elysion E7 | Elysion-half bindings + additional world |
| `SERV_IRUHADEV_ELYSION_VILLAGE` | Elysion E1 | NPC relation, house keys, in-village warps |
| `SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION` | A7, E4 | local-map Elysion tab, bindings, scroll/guide |
| `SERV_IRUHADEV_JUNE_MOVABLE_LINE` | Elysion E5 | moving platforms |
| `SERV_IRUHADEV_JUNE_SIMPLE_DLG`, `SERV_IRUHADEV_JUNE_DRAMATIC_MESSAGE` | Elysion E6 | gauges, dramatic messages |
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

Play-test confirmation is batched into A13.

---

## Phase A0 — Worktree, docs, census v1

**Steps**
1. In the main Trunk, `git status` (read only). Do not touch the buff
   session's uncommitted files. Create the worktree from the **committed**
   tip: `git worktree add -b mods/elysion-port ../Trunk_elysion mods/june-buff-port`.
2. Make it buildable. `.gitignore` is allowlist-style, so `Libs/` (4.4 GB),
   `KNCSDK_2010/`, the untracked parts of `KNCSDK/`, `cximage/` and any other
   input the four `.vcxproj` files reference are missing. List what each
   project's include/lib paths need (`$(SolutionDir)…`), then:
   - wholly untracked dirs → directory junction (`mklink /J`) to the main
     Trunk's copy;
   - partially tracked dirs → copy only the untracked files in;
   - prebuilt libs the projects link (`KTDXLIB/luajitLib.lib`, …) → build
     them in the worktree per `CLAUDE.md`, or copy.
   Prove it: full four-project build with `SolutionDir` = the worktree, rc=0,
   `X2/US_SERVICE/x2.exe` produced. **If junctions or the build fail in a way
   you can't settle, stop and ask** — do not fall back to working in the main
   Trunk.
3. Confirm both docs are present in the worktree (they were committed on
   `mods/june-buff-port` on 2026-10-03, before the branch point). If either is
   missing, copy it from the main Trunk root and commit it.
4. Write `june2017_re/region_census.py` (see *Exit metric*), reusing what
   exists: `june2017_re/assets.py` (asset walk), `buff_census.py` (flag
   probing via `flagprobe.py`, preprocessed binding scan, `--json`),
   `census.py`. Inputs: `june_src_fixed/` only for June Lua; March's
   `march2014/` + `kom_contents/` (bytecode constants) + preprocessed C++.
   v1 covers the cheap checks: kom headers / manifests / live sizes,
   `TABLE.MEMBER`, bindings, `STR_ID`, nil holes, registry-key diff, NPC
   readiness, ID collisions (incl. relocations), item IDs (`BOSS_DROP_ITEM`,
   field drops vs `Item.lua`/`ItemTrans`), effect-set presence. C++ switch
   reachability is **not** in v1 — it consumes A1a's `region_reach.tsv` later.
5. Record the baseline in this doc. It must reproduce the ledger's numbers
   (41 WI, 7 VMI, 12 HI, 21 DI, 203 NUI, 0 collisions, 65 unbound names,
   393 effect sets, ≥ 2,071 assets). Where it disagrees, find out which is
   wrong before going on, and correct the ledger in place.

**Run with:** Sonnet 5.5, high effort. Tooling; light Opus 5.5 review of the
census method (spot-check 3 items each way).

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md in the main Trunk
root in full -- "Starting context", "Investigation ledger", "Lanox lessons ->
gates", "The exit metric" and "Phase A0". Never launch X2_offline.exe without
asking me first. Do NOT consult NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md
or the mods/lanox-offline branch, and tell any subagent the same. Another
session is working uncommitted in the main Trunk on mods/june-buff-port: never
checkout, stash, add or commit there.

Effort: high.

Your task (Phase A0): create the worktree ../Trunk_elysion on new branch
mods/elysion-port from the committed mods/june-buff-port tip; make it build
(junctions/copies for the untracked build inputs; prove with a full
US_SERVICE build, rc=0, fresh x2.exe) -- stop and ask if that fails. Confirm
both phase docs are in the worktree (committed before the branch point; copy
and commit them only if missing). Then write june2017_re/region_census.py (v1 = the cheap checks listed in
Phase A0, reusing assets.py / buff_census.py / flagprobe.py) and record its
baseline in the doc.

Premises to verify first: that the ledger's ID counts and the 0-collision
claim reproduce; that the staged EffectSet.lua really has no Elysion entries.

Done when: worktree builds; both docs present on mods/elysion-port; census
baseline recorded and matching the ledger (or the ledger corrected in place).
Record a "Phase A0 -- Done (date)" note with the session cost, then commit on
mods/elysion-port.
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
3. Also map, for the Elysion doc: local-map scroll/guide/filter/flags
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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (worktree
Trunk_elysion, branch mods/elysion-port) in full -- especially "Starting
context", the ledger's "C++ sites" and "June-only engine" sections, and
"Phase A1a". Use the ghidra skill on unpacked_x2.exe; do not mutate the Ghidra
project without asking. Never launch X2_offline.exe without asking. Do NOT
consult NEW_VILLAGE_*.md or mods/lanox-offline; tell subagents the same.

Effort: xhigh. Read-only.

Your task (Phase A1a): build june2017_re/region_reach.tsv -- every March C++
site that enumerates regions (Sander/Ranox consumers, every *_END marker
consumer), each with its June counterpart, live/dead status from the probed
flag cache, and a port-whole / omit-with-comment verdict and owning phase.
Also map the Elysion-doc engine sites: local-map scroll/guide/page
filter/event flags, AddVillageNpcRelationDataBy* + VRT_CHANGE, the new
DLG_Map_House keys and in-village warps. Settle CX2FieldMiniMap live/dead,
June's IsVillageField/IsDungeonGate/IsDungeonLounge ranges, and which *_END
consumers size arrays.

Premises to verify first: every file:line in the ledger's touch-list table.

Done when: region_reach.tsv covers every grep hit; funcmap.tsv rows added;
an "A1a reachability summary" is in the doc. Record a Done note with the
session cost, then commit on mods/elysion-port.
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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (worktree
Trunk_elysion, branch mods/elysion-port) in full -- especially "Starting
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
note with the session cost, then commit on mods/elysion-port.
```

### Phase A1b — Done

*(empty)*

---

## Phase A2 — Level cap 99

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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (worktree
Trunk_elysion, branch mods/elysion-port) in full -- "User decisions" 4-5,
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
kills-per-level table and session cost), then commit on mods/elysion-port.
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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (worktree
Trunk_elysion, branch mods/elysion-port) in full -- "Starting context"
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

Done when: Phase A3's "Done when" holds. Record a Done note with the session
cost, then commit on mods/elysion-port.
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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (worktree
Trunk_elysion, branch mods/elysion-port) in full -- "Starting context", the
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

Done when: Phase A4's "Done when" holds. Record a Done note with the session
cost, then commit on mods/elysion-port.
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
6. **World map:** June's Elysion block (`DLG_UI_World_Map_NEW.src.lua:1842-2080`)
   — the rest + F01/F02 pins land here; the cloud static's `AddDummyInt(99999)`
   placeholder (`M14 …World_Map_NEW.src.lua:2315`) becomes the real VMI.
   Town + F03 pins are the Elysion doc's (E2/E3); port the whole block once,
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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (worktree
Trunk_elysion, branch mods/elysion-port) in full -- "Starting context", the
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
after I pack). Record a Done note (incl. which branch's exe is deployed) and
the session cost, then commit on mods/elysion-port.
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
   a comment** until the Elysion doc's E2 restores it.
5. Offline: field monsters and drops come from `BattleFieldData` + the NPC
   tables + `X2OfflineDropTable` (no IDs hardcoded); check the join-ACK
   start-position quirk (`Handlers_Room.cpp:1278`, `:1365`) on these fields.

**Done when:** census `--line FLD` (F01/F02) clean except buff-owned items;
build rc=0; staged + pack list. After pack: Lanox F03 → portal → 1008; 1008 ↔
F01 ↔ F02; monsters spawn with HP and drop; F02 has no dead exit.

**Run with:** Opus 5.5, high effort. Required review.

```
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (worktree
Trunk_elysion, branch mods/elysion-port) in full -- "Starting context", the
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

Done when: Phase A6's "Done when" holds. Record a Done note (deployed branch,
session cost), then commit on mods/elysion-port.
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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (worktree
Trunk_elysion, branch mods/elysion-port) in full -- "Starting context", the
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

Done when: Phase A7's "Done when" holds. Record a Done note (deployed branch,
session cost), then commit on mods/elysion-port.
```

### Phase A7 — Done

*(empty)*

---

## Phase A8 — AtlasDevelopment line (pattern-setter)

**Steps.** This phase writes the **per-line checklist** into this doc (the
section *Per-line checklist* below) while doing it, so A9/A10 and the Elysion
doc's lines can follow it.
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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (worktree
Trunk_elysion, branch mods/elysion-port) in full -- "Starting context", the
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
Record a Done note (deployed branch, session cost), then commit on
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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (worktree
Trunk_elysion, branch mods/elysion-port) -- "Starting context", the
"Per-line checklist", Appendix A (AC NUIs) and "Phase A9". Never launch
X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high. If the census shows an engine gap the A1b map doesn't settle,
stop on it and log it for an Opus pass -- do not guess.

Your task (Phase A9): port the AtlasCity line by the Per-line checklist.

Done when: census --line AC is clean, staged with manifests, pack list handed
over (live-kom check after I pack). Record a Done note (deployed branch,
session cost), then commit on mods/elysion-port.
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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (worktree
Trunk_elysion, branch mods/elysion-port) -- "Starting context", the
"Per-line checklist", the buff-dependency rule, Appendix A (TW NUIs) and
"Phase A10". Check JUNE_BUFF_PORT_PHASES.md for Phase 10's status first.
Never launch X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high. If JUNE_BUFF Phase 10 (SetMonsterBuffGroupID / BUFF_GROUP_ID)
hasn't landed, record the block and stop.

Your task (Phase A10): port the Tower line by the Per-line checklist,
transcribing June's WI_TOWER_STAGE3 bug verbatim with a log line.

Done when: census --line TW is clean, staged with manifests, pack list handed
over. Record a Done note (deployed branch, session cost), then commit on
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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (worktree
Trunk_elysion, branch mods/elysion-port) -- "Starting context", the ledger's
"Assets" section, "Lanox lessons -> gates" and "Phase A11". Also read
LANOX_REMAINING_PHASES.md Phase 0 (the TET census and mesh-internal texture
method). Never launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase A11): close the Atlas half's asset set (mesh-internal
textures included), census every TET key, size-check against June's kom index,
resolve mount order, and produce one consolidated pack list.

Done when: census --region atlas shows no missing assets or keys and the pack
list is in the doc. Record a Done note with the session cost, then commit on
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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (worktree
Trunk_elysion, branch mods/elysion-port) -- "Starting context", "Phase A12"
and "Verification". Never launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: medium.

Your task (Phase A12): build all four projects as US_SERVICE from the
worktree (PCH touched), prove every Atlas-port flag's literal is in x2.exe,
deploy as X2_offline.exe (check size/mtime; record that it is the
mods/elysion-port build), and hand me the consolidated pack list and the
Atlas reachability checklist.

Done when: deployed and handed off. Record a Done note with the session cost,
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
Continuing the Atlas/Elysion port. Read ATLAS_PORT_PHASES.md (worktree
Trunk_elysion, branch mods/elysion-port) in full, plus LANOX_TROUBLESHOOTING.md.
Never launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase A13): I have packed and played the Atlas reachability
checklist; I'll paste results and logs. Root-cause each failure (diagnose
with short-lived [ELYDBG] lines, not guesses), fix, rebuild, hand off again.
Then close out: MODS.md entries for every flag, troubleshooting rows, and the
"Deferred to the quest plan" section.

Done when: every checklist item passes or is recorded with a cause and
owner. Record a Done note with the session cost, then commit on
mods/elysion-port.
```

### Phase A13 — Done

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

## Out of scope (with the reason)

- **Hero tier** (`DI_*_HERO_*`, `DI_HERO_DUMMY_*`, `Static_HeroMode_Filter`):
  needs a `DL_HERO` engine change; June's Mode Button is unreachable even in
  June (`JUNE_UI_PARITY_PHASES.md` Phase 4 findings).
- **Seasonal variants** (`Elysion_Atlas_rest_Festival` + LineMap,
  `Field_Elysion_rest_luminarie`, `_XMAS`, `_XMAS_2`; village variants in the
  Elysion doc): Lanox deferred its seasonal variants too.
- **Henir NPCs** (11): used only by June's Henir dungeon. **CSM cards** (10).
- **Dungeon lounge 30009, spa:** no content in June. **Field 04:** enum only.
- **Everything quest-related** → *Deferred to the quest plan*.

## Deferred to the quest plan

(Filled in further by A1a and A13.) Already measured:

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
- **Tools:** `june2017_re/{region_census.py,region_reach.tsv,region_engine.tsv,cap99_gen.py,funcmap.tsv}`.

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
- Elysion-half NUIs (SC/AR/SL/ME/village): `ELYSION_PORT_PHASES.md` Appendix A.
  A3 adds all of them.
