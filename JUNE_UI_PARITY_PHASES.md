# June UI Parity Phases: Local Map and Party dialogs

Not Lanox-specific. This covers adopting June 2017's Local Map (dungeon list)
and Party/Dungeon button UI, both the dialog `.lua` files and the C++ that
drives them, in line with the standing goal of March engine → June parity.
Requested 2026-09-30. `LANOX_REMAINING_PHASES.md` depends on it: its Phase 3
(Lanox tab) is delivered by Phase 2 here, and the progress-bar half of its
Phase 2 is delivered by Phase 3 here.

**Rules:** the same "Starting context" and fix philosophy as
`LANOX_REMAINING_PHASES.md`, which you should read first. No partial June
updates: port whole June functions. June-only enum values for content March
lacks (`LMI_ELYSION`, `LMI_EVENT`, `LMI_SPECIAL`, `LMI_EN`, …) are omitted
from the staged lua with a comment, never invented. **Every March village's
local map (Ruben → Sander) must keep working after each phase.**

**Model, effort and review per phase:** the columns below, repeated as a
**Run with:** line above each kickoff prompt. What the model, effort and
review levels mean is defined once, in `LANOX_REMAINING_PHASES.md` ("Model,
effort and review per phase"). Every phase here ports C++, so apart from the
read-only survey, every phase needs review.

| Phase | Scope | Kind | Model | Effort | Review |
|---|---|---|---|---|---|
| 0 | Ghidra survey + gap table | read-only | Opus 5.5 | high | none (read-only) |
| 1 | Enums / custom messages the June dialogs need | C++ + Lua enum | Sonnet 5 | medium | required |
| 2 | Adopt June `DLG_UI_Local_Map_new.lua` (+ Button21 atlas, arrow paging) | C++ + data | Opus 5.5 | high | required |
| 3 | June's local-map dungeon progress display | C++ + data | Opus 5.5 | high | required |
| 4 | Local-map Mode Button (`DLG_UI_Local_Map_Mode_Button.lua`) | C++ + data | Opus 5.5 | medium | required |
| 5 | Party button `DLG_UI_PARTY_BOTTON_NEW_20161103.lua` + Party length | C++ + data | Opus 5.5 | high | required |

## Investigation ledger (proven 2026-09-30 in Ghidra, do not redo)

| June string (plain wide) | Address | Loaded by | March equivalent |
|---|---|---|---|
| `DLG_UI_Local_Map_new.lua` | `334c8350` | `FUN_3293ab50` (also uses the XOR'd key `LOCAL_MAP_DLG`) | `X2LocalMapUI.cpp:547`, same name |
| `DLG_UI_Local_Map_Mode_Button.lua` | `334c80a0` | `FUN_32938380` ← `FUN_32938450` | **none (new feature)** |
| `DLG_UI_PARTY_BOTTON_NEW_20161103.lua` | `33489a10` | `FUN_322c3d80` | `X2PartyUI.cpp:3433` loads `DLG_UI_PARTY_BOTTON_NEW.lua` (**renamed**) |
| `DLG_UI_PARTY_LENGTH_NEW.lua` | `3348986c` | `FUN_322c2000` | `X2PartyUI.cpp:1639`, same name |

These points partly correct the original hunch:
- June loads the *same* Local Map file name.
- There is no plain-string reference to `DLG_UI_Local_Map_NEW_2.lua`,
  `DLG_UI_PARTY_LENGTH_NEW_2.LUA` or `DLG_UI_PARTY_BOTTON_NEW.lua`. They look
  unused, so confirm that in Phase 0.
- June ships `DLG_UI_Local_Map_NEW.lua` in both `data034_kom` and
  `data217_kom` (with `_NEW_2` alongside in 217). Which copy wins depends on
  archive precedence, which Phase 0 must determine.

Other facts:
- June's Local Map lua uses `DLG_UI_Button21.tga` (30 refs), including
  `BTN_RANOX_N/O/D`. It references `LMUCM_LOCAL_MAP_ARROW_LEFT/RIGHT` and
  `LMI_ELYSION`, `LMI_EVENT`, `LMI_SPECIAL` and `LMI_EN`, none of which exist
  in March (`X2LocalMapUI.h:9-16`).
- March's lua (`data034`) uses `DLG_UI_Button20_NEW.tga` with 9
  `LMUCM_DUNGEON_GATE_SELECTED` tabs. The 9th is `LMI_RANOX`, already live;
  its atlas keys are missing.
- June has no `DLG_Map_Local_*_Icon_Info_NEW.lua`. March's per-dungeon
  names/progress come from those files plus `_LimitInfo` + star controls
  (`X2LocalMapUI.cpp` ~680-682, 820, 935, 955).

---

## Phase 0 - Ghidra survey (read-only)

Steps:
1. Settle which June copy of `DLG_UI_Local_Map_new.lua` is live: June's
   mass-file precedence between `data034` and `data217`. Find the
   `KGCMassFileManager` equivalent, or compare against the data217 index
   order.
2. Map June's `CX2LocalMapUI` (from `FUN_3293ab50`), `CX2PartyUI` (from
   `FUN_322c3d80` / `FUN_322c2000`) and the Mode Button owner
   (`FUN_32938450`). Record every method, especially:
   - the UICustomEventProc switch: arrow left/right, gate selected, mode
     button
   - the tab/page layout logic
   - how dungeon progress/clear state is drawn without Icon_Info
3. Enumerate every enum, custom message and lua global the June dialogs
   reference, and mark which ones March lacks. One nil argument aborts the
   whole dialog script.
4. Diff March's corresponding functions case by case.
5. Write "Phase 0 findings" into this doc: the function map (June VA ↔ March
   function), the gap table, the atlas list, and the live-copy verdict.

**Run with:** Opus 5.5, high effort. Ghidra survey that every later phase builds on; no code review, but the findings table deserves a careful re-read.

**Kickoff prompt**:
```
Read JUNE_UI_PARITY_PHASES.md in the repo root in full, and the "Starting
context" block of LANOX_REMAINING_PHASES.md. Branch: mods/lanox-port.

Your task is READ-ONLY. Survey June 2017's Local Map and Party UI in Ghidra
(ghidra skill, unpacked_x2.exe; loaders are FUN_3293ab50, FUN_32938450 ->
FUN_32938380, FUN_322c3d80 and FUN_322c2000). Settle which June copy of
DLG_UI_Local_Map_new.lua is live (data034 vs data217). Map each June
function to its March counterpart (X2LocalMapUI.cpp, X2PartyUI.cpp).
Enumerate every enum/custom-msg/lua global the June dialogs need that March
lacks. Find how June draws dungeon progress without *_Icon_Info_NEW files.
Append "Phase 0 findings" to JUNE_UI_PARITY_PHASES.md. Make no code or data
changes.
```

## Phase 1 - Enums and custom messages

Add the June `LOCAL_MAP_UI_CUSTOM_MSG` / party custom messages March lacks
(e.g. `LMUCM_LOCAL_MAP_ARROW_LEFT/RIGHT`) to both the C++ enum
(`X2LocalMapUI.h`) and the Lua enum table (the three-layer rule in
`LANOX_TROUBLESHOOTING.md`), **at June's numeric values**, taken from Phase 0.
June-only content IDs (`LMI_ELYSION` etc.) are not added; the staged luas
drop those controls instead. New flag: `SERV_IRUHADEV_JUNE_LOCAL_MAP_UI`.
Rebuild, and confirm that the existing March dialogs still load.

**Run with:** Sonnet 5, medium effort. Small, mechanical, but the values are a contract; review required (C++ and Lua enum values match June and each other).

**Kickoff prompt**:
```
Read JUNE_UI_PARITY_PHASES.md (all, especially "Phase 0 findings" and
"Phase 1") and LANOX_REMAINING_PHASES.md "Starting context". Branch:
mods/lanox-port. Never launch X2_offline.exe without asking first.
Your task: add the custom messages / enums the June Local Map and Party
dialogs need that March lacks, at June's numeric values from Phase 0, in the
C++ enum AND the Lua enum (three-layer rule, LANOX_TROUBLESHOOTING.md). Do
NOT add June-only content IDs (LMI_ELYSION/EVENT/SPECIAL/EN). Use a new flag,
SERV_IRUHADEV_JUNE_LOCAL_MAP_UI. Build, deploy, and give me a play-test
checklist confirming every March village's local map still opens.
```

## Phase 2 - Adopt June's `DLG_UI_Local_Map_new.lua` (delivers the Lanox tab)

1. Stage June's live copy (per Phase 0) as a patch in `march2014/data034/`.
   Drop the `LMI_ELYSION/EVENT/SPECIAL/EN` controls with comments.
2. Stage `DLG_UI_Button21.tga/.TET` (and any other atlas it needs; check the
   Phase 0 atlas list against the live koms) under June's archive number.
3. Port June's tab/arrow-paging handling in `CX2LocalMapUI`. Port whole
   functions, flag-gated.
4. Mark LANOX_DEFECT_PHASES Phase 4's Button20 merge as superseded. Clean
   `march2014/update/data056/_manifest.json` if LANOX_REMAINING Phase 3 hasn't
   already.

Verify:
- The dungeon list shows June's tab strip with Lanox. The arrows page
  correctly.
- Each tab selects the right local map. Hit-boxes work.
- Every village's tab still works.

**Run with:** Opus 5.5, high effort. Whole-function C++ port plus lua/atlas adoption; review required.

**Kickoff prompt**:
```
Read JUNE_UI_PARITY_PHASES.md (all, especially "Phase 0 findings" and
"Phase 2") and LANOX_REMAINING_PHASES.md "Starting context". Branch:
mods/lanox-port. Never launch X2_offline.exe without asking first.
Your task: adopt June's DLG_UI_Local_Map_new.lua (the live copy per Phase 0).
Drop the June-only LMI_ELYSION/EVENT/SPECIAL/EN controls with comments.
Stage the DLG_UI_Button21 atlas, and port June's tab/arrow-paging C++ in
CX2LocalMapUI as whole functions behind SERV_IRUHADEV_JUNE_LOCAL_MAP_UI.
This delivers the Lanox tab (LANOX_REMAINING_PHASES Phase 3), so mark
LANOX_DEFECT_PHASES Phase 4's Button20 merge superseded. Compile the lua with
elstools -v mar-2014, hand me the pack list, build and deploy.
```

## Phase 3 - June's local-map progress display (delivers Lanox progress bars)

> **Rescoped 2026-10-01 (Phase 0 findings, "User decisions" items 1-2).**
> June has no difficulty bars, and we follow June. Phase 3 is now: adopt
> June's per-village local-map files for every March village (Ruben, Elder,
> Besma, Altera, Peita, Velder, Hamel, Sander; Lanox already matches), which
> removes the bars. Hazards to settle while doing it:
> - June's `Icon_NEW` buttons carry a third dummy, `DI_*_HERO_NORMAL`
>   (100000-100410), which March lacks. Missing enum members read as 0.
>   Follow the effect-port precedent (add every June enum member the adopted
>   files reference) or accept 0. Either way, the Phase 4 hero branch must
>   not use it as a dungeon ID while no Hero data exists.
> - June's event-title statics use `DLT_EVENT_DUNGEON_TITLE` (100). Without
>   Phase 2's port of June's 1/2/100 test in `UpdateDungeonButtons`, March
>   never shows or hides them, so they are always visible. Port the test or
>   check how they render.
> - Peita: March's entry points `LOCAL_MAP_DLG` at
>   `DLG_UI_Local_Peita_Back_NEW.lua`. June's files are
>   `DLG_Map_Local_Peita_Back_NEW.lua` / `_Icon_NEW.lua`, so the entry script
>   needs repointing.
> - June's per-village *entry* scripts (`DLG_Map_Local_<V>.lua`) reference
>   `DI_HERO_DUMMY_COMMON`, `DI_BESTERA_START`, `DI_2016_XMAS`,
>   `DI_SANDER_SECRET_HELL` and `VMI_SPA_RUBEN`, which March lacks (gap
>   table). Diff them against March's before deciding whether to adopt them.
> - Diff each adopted file against March's staged Ranox patches so no Ranox
>   fix is lost; the Lanox files are already June's.
>
> The text below is the original plan and its kickoff prompt; where it says
> "port June's progress mechanism", read "adopt June's files".

Using Phase 0's finding on how June draws dungeon names, progress and clear
state without Icon_Info files, port that mechanism. Stage whatever June data
it reads, either per village or globally. Decide explicitly whether March
villages move to June's mechanism, adopting June's per-village
`DLG_Map_Local_*` files where they differ, or keep their Icon_Info path. The
end state must render names and progress on **every** village's local map,
Lanox included. If June's per-village files are adopted, diff each one
against March's staged patches (the Ranox Back/Icon/root files) so no Ranox
fix is lost.

**Run with:** Opus 5.5, high effort. Mechanism port with a design decision for all villages; review required, including every March village's local map.

**Kickoff prompt**:
```
Read JUNE_UI_PARITY_PHASES.md (all, especially "Phase 0 findings" and
"Phase 3"), LANOX_REMAINING_PHASES.md "Starting context" and its Phase 2.
Branch: mods/lanox-port. Never launch X2_offline.exe without asking first.
Your task: port June's local-map dungeon progress display (names, progress
bars, clear stars), which June draws without *_Icon_Info_NEW files. Decide,
and tell me before implementing, whether March villages move to June's
mechanism or keep Icon_Info. The end state: every village's local map,
Lanox included, shows names and progress. Port whole functions, flag-gated.
```

## Phase 4 - Local-map Mode Button

> **Decided 2026-10-01:** the user approves porting it as a dead button; Hero
> dungeons come later. See Phase 0 findings, "User decisions" item 3.

Port June's `DLG_UI_Local_Map_Mode_Button.lua` feature (`FUN_32938450` →
`FUN_32938380`): what the button toggles, its owner class, its custom
messages, and the lua plus the atlas it needs. If the mode it switches to
depends on content March lacks, report that and ask before porting a dead
button.

**Run with:** Opus 5.5, medium effort. RE first and possibly a stop-and-ask; review required if C++ is ported.

**Kickoff prompt**:
```
Read JUNE_UI_PARITY_PHASES.md (all, especially "Phase 0 findings" and
"Phase 4") and LANOX_REMAINING_PHASES.md "Starting context". Branch:
mods/lanox-port. Never launch X2_offline.exe without asking first.
Your task: port June's Local Map Mode Button (DLG_UI_Local_Map_Mode_Button.lua,
loaded by FUN_32938380, called from FUN_32938450). First report what it
toggles and whether that mode depends on content March lacks. Ask me before
porting a button that would do nothing. Otherwise port the C++ whole behind
SERV_IRUHADEV_JUNE_LOCAL_MAP_UI, stage the lua and atlas, then build and
deploy.
```

## Phase 5 - Party button `_20161103` + Party length

> **Decided 2026-10-01:** scope includes June's `CreatePartyDlg` whole, with
> `DLG_UI_PVP_UNION_MULTIPLE.lua`. See Phase 0 findings, "User decisions"
> item 4.

1. Diff June's `DLG_UI_PARTY_BOTTON_NEW_20161103.LUA` against March's
   `DLG_UI_PARTY_BOTTON_NEW.LUA`, and June's `DLG_UI_PARTY_LENGTH_NEW.lua`
   against March's.
2. Port June's `FUN_322c3d80` / `FUN_322c2000` and their event handlers
   whole.
3. Switch the load at `X2PartyUI.cpp:3433` to the `_20161103` name under the
   flag, with the original in `#else`.
4. Stage the luas and atlases. Handle missing enums per Phase 1.

Verify: the party/dungeon buttons and the party panel match June's layout.
Every button works (create party, dungeon select, quick join, etc.). The
dungeon picture from LANOX_REMAINING Phase 1 still renders.

**Run with:** Opus 5.5, high effort. Loader and handler port touching every party button; review required.

**Kickoff prompt**:
```
Read JUNE_UI_PARITY_PHASES.md (all, especially "Phase 0 findings" and
"Phase 5") and LANOX_REMAINING_PHASES.md "Starting context". Branch:
mods/lanox-port. Never launch X2_offline.exe without asking first.
Your task: adopt June's party UI. June loads
DLG_UI_PARTY_BOTTON_NEW_20161103.lua (FUN_322c3d80) where March loads
DLG_UI_PARTY_BOTTON_NEW.lua (X2PartyUI.cpp:3433), and
DLG_UI_PARTY_LENGTH_NEW.lua (FUN_322c2000, same name as March's). Diff both
luas, port June's loaders and handlers whole behind a new SERV_IRUHADEV_
flag, stage the luas and atlases, then build and deploy. Every party button
must still work.
```

---

## Phase 0 findings (investigated 2026-10-01, read-only)

Sources: Ghidra on `unpacked_x2.exe`; `elstools inspect -q -i` on every June
kom (`14JUN17/data/*.kom`) and every live March kom (`24965799/data/*.kom`);
June and March dialog luas decompiled into a scratch directory, never into
either tree. The June ones went through `elstools decompile-batch` and then
`june2017_re/ljd_jun2017.py`, which reported all 85 OK; the March ones went
through `decompile-batch`. TET keys were read with a reader for the binary
`KTETFormatHeader` layout (`KTDXLIB/KTDXDeviceTET_Preprocessing.h`). An
extracted file was trusted only when its size matched the live kom index.
Nothing in either game tree, the repo, or the Ghidra database was modified.

Where a June function is named below, the name rests on its strings, its
callers and its structure, all matched against the March function. Ghidra was
not annotated. "Proven" means read directly from decompiled code or data.
"Inferred" is marked as such.

### Corrections to the ledger

1. **June ships exactly one `DLG_UI_Local_Map_NEW.lua`: data217 (18273 B).**
   June's `data034.kom` index has no such entry. The `14JUN17/data/data034_kom`
   extract has none either. So no precedence question exists on the June
   side.
   - Live March has its copy in `data034.kom` (10753 B). The repo's patch of
     it is `march2014/data034/DLG_UI_Local_Map_NEW.src.lua`.
   - **For Phase 2:** June's version should replace March's data034 copy, as
     a patch in `march2014/data034/`. Do not add it under
     `march2014/update/data217/`, because that would put two copies with the
     same name into March's mount order. LANOX Phase 0 §C shows how that bites.
2. **`LMI_EN` is not referenced by `DLG_UI_Local_Map_NEW.lua`.** Only
   `DLG_Map_Local_EN*.lua` uses it. The dialog references `LMI_ELYSION`,
   `LMI_EVENT` and `LMI_SPECIAL`.
3. **`DLG_UI_Button21.TET` and `DLG_UI_Button21.tga` are byte-identical in
   live March (`data056`) and June (`data056`).** Both files hash the same,
   and the TET has 36 keys, including `BTN_RANOX_N/O/D` and
   `BTN_ELISION_N/O/D`. **The tab strip needs no atlas staging.** The arrow
   buttons do (see the atlas section).
4. **March's `DLG_Map_Local_*_Icon_Info_NEW.lua` files are orphans.**
   - No live March lua `IncludeLua`s them. Every `*_Back_NEW.lua` includes
     only its `*_Icon_NEW.lua`.
   - The only `Icon_Info` *file* references are in the pre-reform
     `*_Back.lua` / `DLG_UI_Local_Peita_Back.lua`, which include the non-NEW
     `*_Icon_Info.lua`. Those are not what `LOCAL_MAP_DLG` points at.
   - The `<DungeonName>_Icon_Info` that `X2LocalMapUI.cpp:680/742/763` looks
     up is a **control** inside `Icon_NEW` (for example `Abyss_Icon_Info`,
     `CA_Icon_Info`): the hell-mode info icon. It is not a file.
   - Names and stars both come from `*_Icon_NEW.lua`:
     - names: a `"<village>_타이틀 모음"` static of `DLG_Localmap_Title_01.tga`
       pictures;
     - stars: one static per dungeon, named via `SetNameStrID(<dungeon name
       STR_ID>)`, with pictures `Star_Bg`, `Star_1..3`.
5. **The `_NEW_2` files and `DLG_UI_PARTY_BOTTON_NEW.lua` are confirmed dead
   in June.**
   - No plain or wide string for `DLG_UI_Local_Map_NEW_2`,
     `DLG_UI_PARTY_LENGTH_NEW_2` or `DLG_UI_PARTY_BOTTON_NEW.lua` exists in
     the binary.
   - No June lua in data0xx/1xx/2xx references them.
   - The binary has exactly one `PARTY_BOTTON` string (`_20161103`) and two
     `Local_Map` dialog strings (`_new.lua` and `_Mode_Button.lua`).
6. **A nil enum *member* does not abort a dialog script.**
   `KNCSDK/Src/lua_tinker.cpp:355` reads ints as `(int)lua_tonumber(...)`, so
   `AddDummyInt(LOCAL_MAP_ID.LMI_ELYSION)` silently becomes `AddDummyInt(0)`.
   Two cases do hard-abort:
   - indexing a nil *table*;
   - calling a method or global function that March never registered.
   Both were checked (see the gap section), and **no hard-abort exists** in
   any in-scope June lua. The real hazard is the silent 0:
   - A tab with `AddDummyInt(0)` passes June's `!= -1` check.
     `GetLocalMapTemplet(0)` is NULL, so it skips the level test and **stays
     visible as a dead tab.**
   - That is why Phase 2 must drop the Elysion/Event/Special controls, not
     merely leave the enums undefined.

### Function map: `CX2LocalMapUI`

The vtable is at `0x334c802c`; slot 0 is `FUN_3293dd00` (scalar deleting
dtor). Members proven from the ctor and handlers:

| Offset | Member |
|---|---|
| `+0x10` | stage |
| `+0x14` | picked local-map ID |
| `+0x1c` | dialog map |
| `+0x44` | **hero-mode flag** (bool, init 0) |
| `+0x58/+0x5c` | Elysion-guide args |
| `+0x60` | **first-page flag** (bool, **init 1**) |

| June VA | Role | March counterpart | Notes |
|---|---|---|---|
| `FUN_32937fa0` | ctor | `CX2LocalMapUI()` | Loads `DLG_SlotManager.lua` (quest desc) and `DLG_Map_Local_Hero_Recruit.lua`, as March does. Adds the `+0x44`/`+0x60` members. |
| `FUN_3293d280` | `UICustomEventProc` | `X2LocalMapUI.cpp:69` | 11 cases; table below. |
| `FUN_3293c440` | `OpenLocalMapDLG(bOpen, id)` | `:301` | Callers are all in `X2PartyUI` (`FUN_322b5970`, `322c2670`, `322c3fa0`, `322c5880`). For `0x2719`=`LMI_ELYSION` it also opens `DLG_Elysion_LocalMap_Guide.lua` (`FUN_329386b0`). |
| `FUN_3293c6f0` | `UpdateLocalMapDLG` | `:387` | Tab visibility plus **paging**; see below. Ends with `UpdateDungeonButtons`, then a party-manager call (`FUN_32434340`, the auto-party-bonus send), then `UpdateHeroRecruitButton`. |
| `FUN_3293ab50` | `CreateLocalMapDLG(id)` | `:541` | Loads `DLG_UI_Local_Map_new.lua`, reads the XOR'd key `LOCAL_MAP_DLG` from the village script, and runs `OpenScriptFile`. Then: the dungeon-enable loop. For every control whose `dummy0 - 30000 < 0x2d0b` (IDs 30000..41530), it hides the control when `FUN_322cac30` (the `IsDungeonEnable` equivalent) is false. This is March's `SERV_NEW_EVENT_TYPES` loop at `:570-583`, not an event-only loop. `FUN_32935f30` maps the ID first; `CreateDungeonClearStars` per dungeon (`FUN_329387a0`); `CreateLocalQuestPicture` per dungeon (`FUN_32936330`). |
| `FUN_329358b0` | `GetLocalMapDLG(id)` | `:612` | |
| `FUN_3293b5e0` | `UpdateDungeonButtons(id)` | `:625` | Accepts `GetDummyInt(1)` in {1, 2, **100**}. 100 is `DLT_EVENT_DUNGEON_TITLE`. Uses dummy **index 2** (Hero dungeon) when `+0x44` is set, otherwise index 0. Handles `Static_HeroMode_Filter` visibility, and `g_pStatic_NewEvent%d` for the Event/Special maps. Also calls `FUN_32939aa0` (`SecretDungeonDesc`; new) and `FUN_32935780` (`UpdateDefenceDungeonIcon`). Plays `NewStage_Open.ogg` via `FUN_32935910` + `FUN_3293d940`, i.e. `IsNewlyOpenedDungeon` → particle/drop-down. |
| `FUN_32936b00` | `UpdateDungeonClearStars(id, dungeon, bShow)` | `:801` | Same shape as March, plus Bestera / Valentine-2017 (`0xa1e0`) / Space-of-Thought (`0xa226`) `_LimitInfo` counters on `DLG_UI_Button21_NEW.tga` `Bg_HenirNum_%d`. **The star loop is unchanged:** `m_eDungeonType==DT_NORMAL && !normalOnly`; control named `m_DungeonName`; pictures 1..3 = `IsClearDungeon(id+0..2)`. |
| `FUN_329387a0` | `CreateDungeonClearStars(id, dungeon)` | `:966` | Only the limit-info blocks remain: Henir, Bestera, Valentine 2017, Space of Thought, each loading its `DLG_Map_Local_*_limit.lua`. |
| `FUN_32936330` | `CreateLocalQuestPicture` | `:1287` | `_LocalQuest`, `Quset_Notice_Big`. |
| `FUN_32935d20` | `UdateLocalMapQuestDesc` | `:1230` | |
| `FUN_32937b10` | `DrawLocalMapQuestDesc` | `:1174` | `ItemDesc`. |
| `FUN_32935780` | `UpdateDefenceDungeonIcon` | `:1358` | `DefenceDungeonDisable`. |
| `FUN_32938ea0` | `GetHenirDungeonIcnoDesc` | `:1387` | |
| `FUN_32939530` | `UpdateHeroRecruitButton` | `:1468` | Also called from party manager `FUN_32432dd0`. |
| `FUN_3293a960` | `OnFrameMove` (inferred from its `(double, float)` signature) | `:64` (empty in March) | `LocalMapMoveUp/Down` plus the ERDA timer (`FUN_3293a2e0`, `ERDA_ENTER_START_TIME`, `LMI_SPECIAL`). Elysion/Special only. |
| `FUN_32939e30`, `FUN_32937660` | Bestera / Space-of-Thought tooltip text | none | June-only content. |
| `FUN_329378a0`, `FUN_329379d0`, `FUN_3293c1e0`, `FUN_329366a0` | Elysion local-map scroll (`Elysion_Local`, `LMI_ELYSION`) | none | June-only content; omit. |
| `FUN_32938450` → `FUN_32938380` | open / create the Mode Button dialog | none | See Phase 4 verdict. |
| `FUN_329354e0` | sync the `NormalDungeonMode`/`HeroDungeonMode` radio state | none | Mode Button. |
| not located | `ClearLocalMapDLGs`, `SetStage`, `CloseOtherUI`, `DropDownDungeonButton`, `Create/UpdateLocalTearOfELWoman`, `GetEventValentineDungeonIconDesc` | `:374`, `:284`, `:1509`, `:1094`, `:1569/1605`, `:1705` | Likely inlined or removed. Not needed for Phases 1-3. |

**`UICustomEventProc` cases (`FUN_3293d280`), values from June
`data036/Enum.lua` `LOCAL_MAP_UI_CUSTOM_MSG`:**

| Value | Name | March C++ / Lua | June handler |
|---|---|---|---|
| 18000 | `LMUCM_DUNGEON_CLICKED` | yes / yes | Same shape as March: party-manager dungeon set; default 30000 = `DI_RUBEN_EL_TREE_NORMAL`; novice-guide step 6/8. Its difficulty→mode mapping (2→1, 3→4, 5→5, else 0) differs from March's `DT_*` switch, so diff it in Phase 2. |
| 18001 | `LMUCM_DUNGEON_GATE_SELECTED` | yes / yes | `OpenLocalMapDLG(true, dummy0)` + `UpdateLocalMapDLG`, as in March. |
| 18002 | `LMUCM_CLOSE_WINDOW` | yes / yes | `OpenLocalMapDLG(false)`. |
| 18003 | `LMUCM_DUNGEON_MOUSE_OVER` | yes / yes | Quest desc, then defence / Henir / Bestera / Space-of-Thought tooltips. March has defence / Henir / Valentine. |
| 18004 | `LMUCM_DUNGEON_MOUSE_OUT` | yes / yes | Hide quest desc. |
| **18005** | `LMUCM_DUNGEON_MODE_SELECTED` | **no / no** | `+0x44 = (dummy0 != 0)`, then `UpdateLocalMapDLG`. Mode Button only. |
| **18006** | `LMUCM_LOCAL_MAP_MOVE_UP` | **no / no** | Elysion scroll (`FUN_329378a0(id, 1)`). |
| **18007** | `LMUCM_LOCAL_MAP_MOVE_DOWN` | **no / no** | Elysion scroll (`FUN_329378a0(id, 2)`). |
| **18008** | `LMUCM_CLOSE_GUIDE_WINDOW` | **no / no** | Elysion guide close (`FUN_31be4210(0x472, +0x58, +0x5c)`). |
| **18009** | `LMUCM_LOCAL_MAP_ARROW_LEFT` | **no / no** | `if (!+0x60) { +0x60 = 1; UpdateLocalMapDLG(); }` |
| **18010** | `LMUCM_LOCAL_MAP_ARROW_RIGHT` | **no / no** | `if (+0x60) { +0x60 = 0; UpdateLocalMapDLG(); }` |

The June lua table lists keys in hash order, but the values are contiguous
18000..18010.

**Value trap for Phase 1.** March's C++ enum (`X2LocalMapUI.h:9-16`) uses
implicit increments from `LMUCM_DUNGEON_CLICKED = 18000`. Appending only the
two arrow messages after `LMUCM_DUNGEON_MOUSE_OUT` would give them **18005
and 18006**, not 18009/18010, while the Lua side has explicit values. The two
layers would then disagree silently. Do one of these:
- add all six, in June's order above, after `LMUCM_DUNGEON_MOUSE_OUT`; or
- give every message added out of sequence an explicit initializer
  (`LMUCM_LOCAL_MAP_ARROW_LEFT = 18009`, `LMUCM_LOCAL_MAP_ARROW_RIGHT = 18010`).

After the edit, check each name's C++ value against its value in
`march2014/data036/Enum.lua`. That check is the review gate. Only 18009/18010
are needed for the tab strip. 18005 is needed only if Phase 4 goes ahead.
18006-18008 are Elysion-only.

**Tab/page layout (`FUN_3293c6f0`, proven).** June's lua lays out two pages
of radio buttons at the same x positions:
- page 1: Ruben, Elder, Besma, Altera, Peita, Velder;
- page 2: Hamel, Sander, Ranox, Elysion.

Event and Special have their own positions; both are dropped for March. The
loop is over controls of type 6 (`UCT_RADIOBUTTON`) with `dummy0 != -1`:
- Set visible first, then hide if `GetSelectUnitLevel() < m_RequireUnitLevel`
  or a `RequireClearDungeonID` is not cleared. That is the same rule as
  March's.
- `QuestGuide` and `WeeklyMark` pictures are indexed by local-map ID.
- `local_72` counts non-Event/Special tabs. The first six are page 1.
- If `+0x60` (first page) is set, page-2 tabs are hidden; otherwise page-1
  tabs are hidden.
- Arrows: `LocalMap_arrowR` plus `StaticArrow` picture 1 appear on page 1
  when six or more tabs are visible. `LocalMap_arrowL` plus picture 0 appear
  whenever page 2 is showing.

After the loop, `UpdateLocalMapDLG` toggles the event statics:
`APOCALYPSE_STATIC`/`TITLE_APOCALYPSE` (dungeon `0xa1b8`),
`g_pStatic_Halloween2016`, `g_pStatic_Event`, `BASTERA_DUNGEON`/`TITLE_BESTERA`,
`g_pStatic_Event2017`, `g_pStatic_NewEvent`, `g_pStatic_NewSpecial`. All of
them are gated on June event flags or June-only maps. **Port hazard:** keep
the code path but drop the controls, and accept that the lookups return NULL.

### How June draws dungeon progress (Phase 3's premise does not hold)

**June draws no per-dungeon clear stars on any village local map.**
- The C++ star loop still exists (see `FUN_32936b00` above).
- But June's village `*_Icon_NEW.lua` files no longer contain star statics
  for normal dungeons. Each keeps only 1-2 statics (for example `STAR2` =
  `SetNameStrID(STR_ID_4979)`). Those are the **Tree Day event** dungeons
  (`DI_EVENT_TREE_DAY_*`), which are `normalOnly = TRUE`, so the loop returns
  early. They are also created `SetShow(false)`.
- Ranox in June: 1 star static (Tree Day). Sander in June: 2 (Tree Day and
  `STR_ID_34700`). Compare March's Sander, which has 8 statics × `Star_Bg` +
  3 stars.
- The only per-dungeon counters June still draws are the `_LimitInfo` plates
  (Henir, Bestera, Valentine 2017, Space of Thought).

Consequences. These are **decisions for the user, not a plan**:
- **There is no June progress mechanism to port.** Adopting June's
  per-village `*_Icon_NEW.lua` files would **remove** the clear stars from
  Ruben through Sander. It would also bring in the Hero `AddDummyInt`s,
  which read as 0. Both violate this doc's "every March village keeps
  working" rule.
- Ranox dungeons do have Normal/Hard/Expert entries in the staged
  `march2014/data035/DungeonData.src.lua` (`DI_RANOX_*_NORMAL/HARD/EXPERT`).
  So **March's own star mechanism would work for Ranox unchanged**, if
  `DLG_Map_Local_Ranox_Icon_NEW.lua` had one star static per dungeon. Each
  would be named by that dungeon's `dungeonName` STR_ID, with `Star_Bg`,
  `Star_1..3` from `DLG_Common_Button00_NEW.tga`.
- June has no source for those statics' positions. Authoring them is
  new data, and under the fix philosophy it needs the user's explicit OK.
  Sander's March layout (`Star_Bg` offset below each icon) is the obvious
  template.
- **"Progress bars" was never matched against the user's actual report.**
  The only per-dungeon progress UI in either build is the three clear stars.
  If the report meant something else, Phase 3 needs a description or
  screenshot from the user before anything is built.
- The Ranox `Icon_NEW` already staged in `march2014/update/data217/` matches
  June's structurally. The only difference is that it omits the two
  `DLT_EVENT_DUNGEON_TITLE` Halloween statics, with a comment.

### Phase 4 verdict: Mode Button would be a dead button

`DLG_UI_Local_Map_Mode_Button.lua` (June `data034`, 2513 B) contains:
- `NormalDungeonMode` (`AddDummyInt(0)`) and `HeroDungeonMode`
  (`AddDummyInt(1)`) radio buttons, both sending
  `LMUCM_DUNGEON_MODE_SELECTED`;
- a `HeroDungeonModeTooltip` button;
- the art `DLG_UI_Image_01.tga BG_Hero`, `DLG_UI_Text_01.tga Bt_Normal_*/Bt_Hero_*`
  and `DLG_Common_New_Texture03.tga`.

Selecting Hero sets `+0x44`. `UpdateDungeonButtons` then uses each icon's
**third** dummy, the Hero dungeon IDs (`DI_*_HERO_NORMAL`,
100000-100410). It also shows `Static_HeroMode_Filter`
(`DLG_Map_Local_HeroMode_Filter.tga`, June `data065`).

March has **none** of the Hero dungeon IDs, no Hero dungeon data, and no
filter atlas. March's `Icon_NEW` controls carry only two dummies, and
`GetDummyInt(2)` returns -1 (`KTDGUIControl.cpp:1113`). By Phase 4's own
rule, **ask the user before porting it.**

### Party UI map

| June VA | Role | March counterpart | Differences |
|---|---|---|---|
| `FUN_322c3d80` | `OpenPartyMenu(bOpen)` | `X2PartyUI.cpp:3429` | Loads `DLG_UI_PARTY_BOTTON_NEW_20161103.lua` (March: `DLG_UI_PARTY_BOTTON_NEW.lua`); same `ButtonQuestGuide` flicker; same `g_pCheckBox_DUNGEON`/`g_pCheckBox_PVP`. The `BUTTON_START` reset is gated on **three** party-manager flags (`+0x70`, `+0x58`, **`+0xda`**) instead of two, and June adds calls to `FUN_31d70d90` (7-line getter on `g_pMain`) and `FUN_326662c0` (`Alarm_Button`). The `+0xda` flag and both calls are **unidentified; open items**. |
| `FUN_322c2000` | `CreatePartyDlg` | `:1634` | Same `DLG_UI_PARTY_LENGTH_NEW.lua`, but the PvP dialog is **`DLG_UI_PVP_UNION_MULTIPLE.lua`** (March: `DLG_UI_PVP_NEW.lua`). Also adds `FUN_322b7be0` (logs `CX2PartyUI_PvpDlgType`) and checks `pvp_player_check1` plus one more unresolved control (string at `0x33489638`). |
| `FUN_322c2670` | `SetDungeonButton` (by strings: `DUNGEON_STAY`, `DungeonMatchWaiting`, `PvpMatchCancel`) | `:5153` | Not diffed yet. |
| `FUN_322c23c0` | `SetPVPButton` (by strings: `PVP_STAY`, `PvpMatchWaiting`, `DungeonMatchCancel`) | `:5083` | Not diffed yet. |
| `FUN_322c0a80` | `ShowPvpModeDesc` | in `CreatePartyDlg` | |

**Phase 5 scope problem:** a whole-function port of June's `CreatePartyDlg`
swaps in June's PvP union UI (`DLG_UI_PVP_UNION_MULTIPLE.lua`). That is the
user's call. Porting `OpenPartyMenu` alone is self-contained, apart from the
three open items above.

Lua diffs:
- **`DLG_UI_PARTY_BOTTON_NEW_20161103` vs March `DLG_UI_PARTY_BOTTON_NEW`:**
  identical control names, custom messages and textures. Only the
  `DUNGEON_STAY`/`PVP_STAY` matchmaking-wait panel differs:
  - a new layout (`*_STAY_OFFSET_X = -370`, `_Y = 40`);
  - the background key `DUNGEON_STAY_1`, used for both panels;
  - one extra string, `STR_ID_43192`.

  June's own `DLG_UI_PARTY_BOTTON_NEW.LUA` (dead in June) is *not* March's
  file. It already has the offset relayout and `DUNGEON_STAY_1`, but not the
  enlarged background or the `STR_ID_43192` line. It is an intermediate
  revision, which confirms `_20161103` is the later file.
- **`DLG_UI_PARTY_LENGTH_NEW` (June vs March):**
  - June adds the controls `ButtonErdaHelp`, `Button_Pve_Rating_Notice`,
    `GuildPve`, `HELL_HERO`, `InhouseCurrentMemberHeroBuffBox`,
    `Static_HeroDungeonTitle`, `Static_Pve_RankGauge` and `VSFOUR`;
  - June adds the messages `PUCM_GUILD_PVE_EVENT_MATCH_CHECKED/UNCHECKED`.
  - Most of these are Hero, Guild-PvE or ERDA content March lacks.

### Enum / custom-message / global gap table

Every `TABLE.MEMBER` and `STR_ID_n` in the in-scope June luas was resolved
against March's effective Lua enums. "Effective" means the `march2014/data036/`
staged copy where one exists, otherwise `kom_contents/data036_kom`. A value
that differs from June's would also have been flagged; **none differs**.

| Needed by | Name | June value | March Lua | March C++ | Phase-1 action |
|---|---|---|---|---|---|
| `DLG_UI_Local_Map_NEW` | `LMUCM_LOCAL_MAP_ARROW_LEFT` / `_RIGHT` | 18009 / 18010 | missing | missing (`X2LocalMapUI.h:9-16`) | **add both** |
| Mode Button | `LMUCM_DUNGEON_MODE_SELECTED` | 18005 | missing | missing | add only if Phase 4 is approved |
| (Elysion only) | `LMUCM_LOCAL_MAP_MOVE_UP/DOWN`, `LMUCM_CLOSE_GUIDE_WINDOW` | 18006 / 18007 / 18008 | missing | missing | optional; no user in March |
| `DLG_UI_Local_Map_NEW` | `LMI_ELYSION` / `LMI_EVENT` / `LMI_SPECIAL` | 10009 / 10010 / 10011 | missing | missing | **do not add**; drop the controls (dead-tab hazard, correction 6) |
| village `*_Icon_NEW` (June) | `DETAIL_LOCAL_TYPE.DLT_EVENT_DUNGEON_TITLE` | 100 | missing | no C++ enum; June C++ tests the literal 100 | only if June's `UpdateDungeonButtons` 1/2/100 test is ported; the controls using it are event-only |
| village `*_Icon_NEW` (June) | `DI_*_HERO_NORMAL` (Elder, Besma, Altera, Peita, Velder, Hamel, Sander) | 100000-100410 | missing | missing | **do not add** (Hero content) |
| June village entry scripts | `DI_HERO_DUMMY_COMMON` 120000, `DI_BESTERA_START` 150000, `DI_2016_XMAS` 41430, `DI_SANDER_SECRET_HELL` 41141, `VMI_SPA_RUBEN` 1500 | as listed | missing | n/a | only matters if June's entry scripts are adopted; they should not be (March's are already patched) |
| `DLG_UI_PARTY_LENGTH_NEW` | `PUCM_GUILD_PVE_EVENT_MATCH_CHECKED/UNCHECKED` | 14065 / 14066 | missing | not checked | Guild-PvE content; drop the control in Phase 5 rather than add |

Hard-abort check: every `:Method(` and bare function call in the 33 in-scope
June files (`DLG_UI_Local_Map_NEW`, Mode Button, both party luas, every
village entry/Back_NEW/Icon_NEW file, Henir limit) was checked by name
against the string literals in `KTDXLIB/`+`X2Lib/`, where the `lua_tinker::
class_def`/`def` registrations live. Bare upper-case globals were checked
against March's top-level Lua names.
- **Nothing is missing.** All 47 GUI methods are registered (for example
  `SetNameStrID` at `KTDXApp.cpp:1878`, `SetShowOffBGByCheck` at `:2061`),
  and `XUF_DODUM_13_SEMIBOLD` is defined.
- Limitation: this is a name-level check. It does not prove each method is
  registered on the class it is called on.

### Atlas list

Keys the in-scope June luas draw, checked against the live kom TETs:

| Atlas | March live | June | Keys March lacks | Needed? |
|---|---|---|---|---|
| `DLG_UI_Button21.tga` | data056, 36 keys | data056, **byte-identical** | none | yes; nothing to stage |
| `DLG_UI_Common_Texture49_NEW.tga` | data057, 39 keys | data057, 40 | `DUNGEON_STAY_1` | Phase 5 only (`_20161103` wait panel) |
| `DLG_UI_Image_07.tga` | **no TET, no image** | data215, 74 keys | `ARROW_L/R_N/O`, `BG_ARROW_L/R` | **yes**: the page arrows (Phase 2) |
| `DLG_UI_Text_14.tga` | no TET, no image | data151 | `Btn_EVENT_*`, `Btn_SPECIAL_*` | no (controls dropped) |
| `DLG_UI_Text_02.tga` | no TET, no image | data151 | `S_Dungeon_Icon` (`WeeklyMark`) | open: June shows `WeeklyMark` only when a weekly/event dungeon is flagged (`FUN_327d9d50`/`FUN_327da190`, unidentified). Decide in Phase 2 whether to keep the static. |
| `DLG_UI_Image_02.tga` | no TET, no image | data057 | `ALTERA_APOCALYPSE` | no (event) |
| `DLG_UI_Common_Texture78_NEW.tga` | data057, 2 keys | data057, 4 | `halloween_event`, `2016Xmas_stage` | no (event) |
| `DLG_UI_Image_01.tga` | image in data215, **no TET** | data215, 102 | `BG_Hero` | Phase 4 only |
| `DLG_UI_Text_01.tga` | no TET, no image | data151 | `Bt_Normal_*`, `Bt_Hero_*` (+ `NAME_HERO` for party length) | Phase 4 only |
| `DLG_Map_Local_HeroMode_Filter.tga` | none | data065 | `Region_HeroMode_Filter` | Phase 4 only |
| `DLG_Localmap_Title_01.tga` | data063, **60 keys** (LANOX Phase 2 merge live) | data151, 77 | `TITLE_BESTERA`, `TITLE_SANDER_SECRET`, `ELYSION` | no (June-only content) |
| `DLG_Common_Button06_NEW.tga` | none | data215 | `BESTERA_N/O` | no |
| `DLG_UI_Common_Texture_Field_00`, `_PVE_01`, `DLG_UI_Text_05` | partial / none | data057 / data151 | `RANOX`, `PVP_RANKUP_GAUGE*`, `BT_GUIDEBOOK_*` | Phase 5, only if the matching party-length controls are kept |

Everything else these luas reference (`DLG_Common_Button00`,
`Button00..05_NEW`, `DLG_UI_Common_Texture13`, `Texture49_NEW` `BG_*` and
`MAIN_BAR`, the `DLG_New_Region_*` backgrounds) is present in live March
with every key. **The minimum new atlas for Phase 2 is `DLG_UI_Image_07`
(`.tga` + `.TET`, June `data215`).** Check its other 68 keys against March
lookups before adopting it wholesale; March has no file of that name, so
there is no conflict.

### Phase 2 port hazards

- **The arrows are load-bearing.** June's layout puts Hamel, Sander *and*
  Ranox on page 2. If paging fails, three March villages become unreachable
  from the dungeon list. Phase 2's verification must include paging to page
  2 and selecting each of the three, then paging back.
- New members: `bool` first-page flag, initialised **true**. A hero flag only
  if Phase 4 is approved.
- `UpdateDungeonButtons` reads dummy index 2 when the hero flag is set. With
  March data that returns -1, so don't port the hero branch without Phase 4.
- The tab loop's level/clear-dungeon test is the same rule as March's
  `FIX_LOCALMAP_OPEN_CONDITION` / `#else` branches. Re-check which one is
  compiled in before porting.
- March's GM bypass (`XUAL_OPERATOR` shows every tab) also exists in June
  (`*(DAT_3377585c+0x43c)+0xf8`, inferred). Keep it.
- The Elysion branches (`0x2719`), the Event/Special `NewEvent` loops
  (`0x271a/0x271b`) and the event statics all key on June-only IDs or flags.
  Omit them with a comment, per the rules at the top of this doc.

### Open items carried forward

- **Peita coverage gap.** March's Peita chain is named differently:
  `DLG_Map_Local_Peita.lua` → `LOCAL_MAP_DLG = "DLG_UI_Local_Peita_Back_NEW.lua"`
  → `DLG_UI_Local_Peita_*`. June renamed it to
  `DLG_Map_Local_Peita_Back_NEW.lua` (data034, a June-only file) and
  `DLG_Map_Local_Peita_Icon_NEW.lua` (data217). The decompile pattern missed
  March's `DLG_UI_Local_Peita_*` files, so they are absent from the star tally
  and the village atlas census, and were not diffed against June's. The
  orphan conclusion still holds for Peita, because the raw byte scan of
  data034 did cover them: only the pre-reform `DLG_UI_Local_Peita_Back.lua`
  includes an `Icon_Info` file.

- June `OpenPartyMenu`: the `+0xda` match flag, `FUN_31d70d90`, and
  `FUN_326662c0` (`Alarm_Button`).
- June `CreatePartyDlg`: the control at string `0x33489638`.
- `FUN_327d9d50`/`FUN_327da190` (the `WeeklyMark` condition) and
  `FUN_322aa950` (event-flag map lookup used for the Elysion/event gates).
- `SetDungeonButton`/`SetPVPButton` were identified by their strings only and
  not diffed against March.
- ~~The meaning of "progress bars" in the user's original Lanox item 4
  report.~~ Resolved below.

### User decisions and follow-up checks (2026-10-01)

**1. "Progress bars" = the clear-difficulty bar under each dungeon name.**
The user means the strip under each name plate that lights up per cleared
difficulty. That is the March star static: one per dungeon, named via
`SetNameStrID(<dungeon name>)`, with pictures `Star_Bg` and `Star_1..3`
(`DLG_Common_Button00_NEW.tga`), driven by `UpdateDungeonClearStars`
(`X2LocalMapUI.cpp:801`).

**2. Decision: follow June. No difficulty bars, on any village.** June
removed these bars from every village's `*_Icon_NEW.lua`. The C++ star loop
is still in June, but it has nothing to draw. Stated by the user 2026-10-01:
the project's goal is to be as close as possible to June's content *and*
behaviour, so June's removal of a March feature is followed, not worked
around.
- **Lanox:** its `Icon_NEW` already matches June (no bars), so Lanox needs
  nothing. LANOX_REMAINING item 4(b) is closed as "matches June".
- **Ruben through Sander:** these still show March's bars. Phase 3 now
  means adopting June's per-village `*_Icon_NEW.lua` (and `*_Back_NEW.lua`
  where June's differs) for every March village. That removes the bars and
  brings each village's icon layout in line with June. See the rewritten
  scope at the top of Phase 3.
- An earlier draft of this item derived bar positions for Lanox from
  March's (-8, +17) name-plate offset. That was the wrong direction (adding
  a feature June removed) and is withdrawn.

**3. Phase 4: port the Mode Button, accepting that it is dead for now.**
The user plans to port Hero dungeons later. Port the button and its message
(`LMUCM_DUNGEON_MODE_SELECTED = 18005`) and the hero flag at `+0x44`. Keep
`UpdateDungeonButtons`' hero branch. With March's two-dummy icons it reads
`GetDummyInt(2) = -1`, so it must not hand -1 on as a dungeon ID: guard it,
or leave the branch unreachable until Hero data exists. Say which in the
review. Stage `DLG_UI_Local_Map_Mode_Button.lua` and its atlases:
- `DLG_UI_Image_01.TET` (June data215; March has the image but no TET);
- `DLG_UI_Text_01.tga/.TET` (June data151);
- `DLG_Map_Local_HeroMode_Filter.tga/.tet` (June data065).

**4. Phase 5: port `CreatePartyDlg` whole, including June's
`DLG_UI_PVP_UNION_MULTIPLE.lua` (June data217, 23909 B).** Feasibility,
checked the same way as the other dialogs:
- No hard-abort: every method is registered in March.
- All 6 controls March's C++ reads from `m_pDLGPartyPVP` exist in June's
  dialog under the same names.
- It needs 5 `PARTY_UI_CUSTOM_MSG` values March lacks:
  - `PUCM_PVP_REWARD_ITEM_MOUSE_OVER`/`_OUT` (14062/14063);
  - `PUCM_PVP_TOURNAMENT_JOIN` (14069), `_JOIN_CANCEL` (14072) and
    `_START_MATCH` (14073).

  The tournament three, and the controls `JoinTournament`,
  `JoinCancelTournament`, `StartMatch_Tournament` and `Static_Tournament*`,
  are June-only content: drop them with a comment.
- The reward-item hover pair goes with `PVPITEM0..5`. Decide in Phase 5
  whether March's PvP reward data can feed it. If not, drop them too.
- Missing atlas keys:
  - `DLG_UI_Common_Texture_PVE_01` (`PVP_MAP_SETUP_N/O`, `PVP_RANKUP_GAUGE*`);
  - `DLG_UI_Image_02` (`BT_LINE_L`);
  - `DLG_UI_Text_01` (`ICON_NEW`);
  - `DLG_UI_Text_03` (`BT_EVENT_TOURNAMENT`);
  - `DLG_UI_Text_11` (`BT_APPLY/CANCEL_N/O`).

  None of these atlases is in live March; all come from June.
- **As with every PUCM addition, check the March `PARTY_UI_CUSTOM_MSG` enum
  for implicit numbering before appending** (Phase 1's value trap).

---

## Phase 1 result (done 2026-10-01)

Added three `LOCAL_MAP_UI_CUSTOM_MSG` members, June's own values, behind a new
flag `SERV_IRUHADEV_JUNE_LOCAL_MAP_UI`:

| Name | Value |
|---|---|
| `LMUCM_DUNGEON_MODE_SELECTED` | 18005 |
| `LMUCM_LOCAL_MAP_ARROW_LEFT` | 18009 |
| `LMUCM_LOCAL_MAP_ARROW_RIGHT` | 18010 |

Both layers: `X2Lib/X2LocalMapUI.h` (C++ enum, explicit initializers so the
gap at 18006-18008 can't shift the later two implicitly) and
`march2014/data036/Enum.src.lua`'s `LOCAL_MAP_UI_CUSTOM_MSG` table (explicit
values already matched June's, as they did for every existing entry).

**18006-18008 (`LMUCM_LOCAL_MAP_MOVE_UP/DOWN`, `LMUCM_CLOSE_GUIDE_WINDOW`)
deliberately not added** — Elysion-only per Phase 0, no March dialog will
ever send them, and nothing in Phases 2-5 needs them. If a later phase turns
out to need one, add it then with its own explicit value; don't infer it
from this table's gap.

**`LMUCM_DUNGEON_MODE_SELECTED` added now, not deferred to Phase 4** — Phase
0's gap table made that conditional on Phase 4 being approved, and it was
(2026-10-01, "User decisions" item 3). No case uses it yet; Phase 4 wires the
switch and the Mode Button's own `+0x44` hero flag.

**No `LMI_*` content IDs added** (`LMI_ELYSION`/`EVENT`/`SPECIAL`/`EN`), per
the rule at the top of this doc. **No `PARTY_UI_CUSTOM_MSG` touched** — none
of the three in-scope messages are party messages (all three are
`LOCAL_MAP_UI_CUSTOM_MSG`), and Phase 5 owns the `PUCM_*` additions with its
own flag.

**Verified the C++ and Lua values agree through the real bytecode, not just
the two source files**: compiled the edited `Enum.src.lua` with
`elstools compile-batch -v mar-2014`, then decompiled that output back and
grepped for all three names — `LMUCM_DUNGEON_MODE_SELECTED = 18005`,
`LMUCM_LOCAL_MAP_ARROW_LEFT = 18009`, `LMUCM_LOCAL_MAP_ARROW_RIGHT = 18010`,
matching the C++ exactly. Also confirmed before editing that the *previous*
committed `march2014/data036/Enum.lua` was already byte-identical to a fresh
compile of the pre-edit `Enum.src.lua` (no drift to inherit).

**Also confirmed `SERV_IRUHADEV_JUNE_LOCAL_MAP_UI` actually reaches
`X2LocalMapUI.cpp`** (not just the PCH silently discarding the `Always.h`
edit): a temporary `#pragma message` inside the `#ifdef`, built once, seen in
the compiler output, then removed before the real build.

**Needs packing**: `march2014/data036/Enum.lua` (265,246 B, was 265,151 B) is
ready; copied into `kom_contents/data036_kom/Enum.lua` and `.src.lua` too, so
the next repack of `data036.kom` picks it up. **That repack will also ship
every other pending `march2014/data036` enum addition** already staged ahead
of `kom_contents` before this phase (Ranox Phase 1's `NPC_UNIT_ID`/`WORLD_ID`/
`VILLAGE_MAP_ID`, the ExtraNPCStat port, and the June2017 effect rebuild's
`HIT_TYPE`/`BUFF_FACTOR_ID` work) — `kom_contents/data036_kom/Enum.src.lua`
was ~42 KB behind the repo copy, now caught up in one shot. Not a problem
(those are all meant to ship), but the next repack's play-test should expect
all of them live together, not just this phase's three values.

Built and deployed: `KTDXLIB`, `X2Lib`, `X2` rebuilt `US_SERVICE` (PCH
invalidated via `touch X2Lib/stdafx.cpp` first, since `Always.h` and
`X2LocalMapUI.h` both changed), 0 errors across all three. `X2/US_SERVICE/x2.exe`
(18,979,840 B, 2026-10-01 09:03) copied to
`$DATA/X2_offline.exe`, confirmed by size and mtime. **Not launched** — see
the play-test checklist below.

### Play-test checklist (no server/client packets changed, local-map-only risk)

Nothing in the adopted dialog lua changed yet (that's Phase 2), and no C++
switch case reads the three new values yet (that's Phases 2 and 4) — this
phase only makes three more names resolve instead of erroring if something
*does* reference them later. So the only realistic regression is the Lua
load itself (a syntax slip in `Enum.src.lua`, or the compiled bytecode not
matching). Confirm, in order:

1. **Before any repack** (current live `data036.kom`, old `Enum.lua` size
   236,117 B, is unaffected by this phase — the new exe doesn't need the new
   enum values yet):
   - Launch, log in, reach the village screen for each of: Ruben, Elder,
     Besma, Altera, Peita, Velder, Hamel, Sander, Lanox.
   - For each village, open the Local Map / dungeon-list dialog and confirm
     it still opens, shows the same tabs as before, and a dungeon gate click
     still opens that dungeon's panel.
   - Open the Party UI and confirm the party/dungeon buttons and the party
     panel look and behave exactly as before (nothing here should differ —
     no party enum changed).
   - Check `ErrorLog.txt` / `offline_server.log` / `OpenScriptFile.log` for
     anything new.
   - **Confirmed 2026-10-01 (user play-test): all village local-map tabs
     behave as usual** on the new `x2.exe`, before any repack. No regression
     from adding the three enum members.
2. **After the user repacks `data036.kom`** (ships this phase's 3 values plus
   the other pending `data036` enum work listed above):
   - Repeat step 1's full village list — Local Map must still open for all
     nine, tabs and dungeon clicks still work, Party UI unchanged.
   - `elstools inspect -q -i data036.kom` shows the new `Enum.lua` entry size
     (236,117 -> should track the fresh `march2014/data036/Enum.lua` compiled
     size once repacked).
   - General smoke test: login, character select, enter a village, enter one
     dungeon — confirms the `data036.kom` repack as a whole didn't regress
     anything else riding in `Enum.lua` (NPC IDs, world IDs, HIT_TYPE,
     BUFF_FACTOR_ID).
   - Watch for any Lua error naming `Enum.lua` or `LOCAL_MAP_UI_CUSTOM_MSG`
     specifically, which would mean the compiled bytecode and the C++ enum
     disagree after all.

Phase 2 is the first phase that actually exercises `LMUCM_LOCAL_MAP_ARROW_LEFT/RIGHT`
(the tab-paging arrows) and depends on this phase's values being right.
