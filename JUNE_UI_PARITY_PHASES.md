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

---

## Phase 2 result (built and deployed 2026-10-01; repack and play-test owed)

### What changed

- **Dialog lua.** `march2014/data034/DLG_UI_Local_Map_NEW.src.lua` is now
  June's live copy, decompiled.
  - It has a header comment block.
  - These June-only controls are dropped, each with a comment in place:
    `LocalMapRadioBtn9`/`Event`/`Special` (`LMI_ELYSION`/`EVENT`/`SPECIAL`),
    Elysion's `QuestGuide` picture, the `WeeklyMark` static, and the event
    statics.
  - There are 9 region radios in order: Ruben, Elder, Besma, Altera, Peita,
    Velder (page 1), then Hamel, Sander, Ranox (page 2). `LocalMap_arrowL/R`
    send 18009/18010. `StaticArrow` holds `BG_ARROW_L/R`.
  - Compiled with `elstools compile-batch -v mar-2014` to
    `kom_contents/data034_kom/DLG_UI_Local_Map_NEW.lua` (12,985 B; the live
    copy is 10,753 B).
  - Round-trip: decompiling the compiled output gives back both arrow
    messages and all nine `AddDummyInt(LOCAL_MAP_ID.*)` lines.
- **Atlas.** `DLG_UI_Image_07.tga` (1,048,620 B) and `.TET` (5,656 B) are
  copied from June `data215` into `kom_contents/data215_kom/` and
  `march2014/update/data215/`. Both `_manifest.json` files list them as `"0"`.
  - `DLG_UI_Button21` needed nothing: live March data056 is byte-identical
    to June's.
- **Enums.** No change needed. The enums are already live: `data036.kom`
  was repacked 2026-10-01 09:11, and its `Enum.lua` is 265,246 B, the
  Phase 1 build.
- **C++.** These are in `X2Lib/X2LocalMapUI.{h,cpp}`, behind
  `SERV_IRUHADEV_JUNE_LOCAL_MAP_UI`. Both functions are whole and keep the
  March original in `#else`.
  - The header gains `m_bHeroMode` (June `+0x44`), `m_bFirstPage` (`+0x60`,
    initialised to `true`) and `m_bLoggedNoArrows`.
  - `UICustomEventProc` (June `FUN_3293d280`): March's body, plus:
    - 18005 `LMUCM_DUNGEON_MODE_SELECTED` sets `m_bHeroMode` from the
      sender's dummy 0, then calls `UpdateLocalMapDLG`. No dialog sends it
      until Phase 4.
    - 18009/18010 flip `m_bFirstPage` only when it changes, then call
      `UpdateLocalMapDLG`.
    - Comments cover the June-only Bestera and Space-of-Thought tooltips.
  - `UpdateLocalMapDLG` (June `FUN_3293c6f0`):
    - It hides both arrows and both `StaticArrow` pictures.
    - It walks the region radios. Picked tab, `SetShowEnable`, the
      `QuestGuide` mark, and the GM bypass (which skips paging, as in June)
      all behave as before.
    - The level gate is unchanged. The quest-mark rule is unchanged.
    - Tabs 0-5 are page 1 and the rest page 2. The other page's tabs get
      `SetShowEnable(false,false)` and their quest mark is hidden.
    - After the loop it shows `arrowL` + `BG_ARROW_L` when on page 2. It shows
      `arrowR` + `BG_ARROW_R` when on page 1 and all six page-1 tabs are
      visible.
    - Every June-only branch is a comment in place.
    - `Always.h`'s flag description now mentions Phase 2.
- **Built and deployed.**
  - `X2ServerProtocol`, `KTDXLIB`, `X2Lib` and `X2` were built as
    `US_SERVICE`, with the PCH invalidated first. All four had rc=0.
  - `x2.exe` (18,981,376 B, 12:27) was copied to `$DATA/X2_offline.exe`. The
    copy is byte-identical, and the `[JUNE_LMUI]` string is present in the
    binary.
  - **Not launched.**

### Corrections to Phase 0

- **June's tab test is not "the same rule as March's
  `FIX_LOCALMAP_OPEN_CONDITION`/`#else`".** June applies:
  1. the level gate (the same as March's compiled branch), then
  2. templet `+0x70`, which is **`RequireCompletedQuestID`**, not a dungeon.
     - The key name is XOR-obfuscated in June's parser `FUN_32501bb0` at
       `0x32502cf4` and decrypts to `RequireCompletedQuestID`.
     - It is tested through the quest manager (`FUN_32322260`,
       `this = *(g_pData+0x4c)`, a map `find != end`).
     - Of the 13 June village luas, only Elysion sets it (quest 22218).
       March's parser and templet have no such field, so it is omitted with
       a comment.
- **June does not test `RequireClearDungeonID` in the tab loop.** Every
  June village sets one; it gates village entry, not the tab. An earlier
  draft of this port added a clear-dungeon gate from March's
  `m_vecRequireClearDungeonID`, which was wrong. With page 1's
  arrow rule it would have hidden the right arrow, and so Hamel, Sander and
  Lanox, from every character missing a page-1 clear (7 of the 8 save
  characters have no Sander clears at all). It was removed before deploy.

### Deliberate deviations, for the user

- **`LMUCM_DUNGEON_CLICKED` is kept as March's body.** June's version has
  the same shape but differs in two ways:
  - the no-party mode is `DT_SECRET -> DM_SECRET_HELL` (March:
    `DM_SECRET_NORMAL`), plus a type-5 mode;
  - the difficulty comes from `FUN_327db250`, which reads hero mode and
    June-only `DungeonData` fields.

  Both belong with the hero Mode Button (Phase 4, approved but not
  started). Porting them now would change which secret-dungeon mode a solo
  party starts in.
- **June's quest-mark call** passes `!m_bHeroMode` to
  `GetNeedToClearDungeonList`. March's takes no such argument. While hero
  mode is off this makes no difference.

### Degrade path

If the loaded `DLG_UI_Local_Map_NEW.lua` has no `LocalMap_arrowL/R` (live
`data034.kom` is still the pre-June copy), paging switches off. The single
row of tabs behaves exactly as March's did. One line is logged per session:
`[JUNE_LMUI] DLG_UI_Local_Map_NEW.lua has no LocalMap_arrowL/R ...`. It goes
to `ErrorLog.txt` and `offline_server.log`. So the exe is safe to run before
the repack; it just looks unchanged.

### Pack list

| Archive | Source dir | File | Note |
|---|---|---|---|
| `data034.kom` | `kom_contents/data034_kom/` | `DLG_UI_Local_Map_NEW.lua` (12,985 B, algo 2) | replaces 10,753 B |
| `data215.kom` | `kom_contents/data215_kom/` | `DLG_UI_Image_07.tga` (1,048,620 B), `DLG_UI_Image_07.TET` (5,656 B), algo 0 | new; not in live data215 |
| `data056.kom` | (none for this phase) | (none) | Manifest cleaned of `DLG_UI_Button20_NEW` (LANOX Phase 3). Only the pending Button17 re-pack is listed there. **Do not ship Button20_NEW.** |

**`data034` and `data215` ship together or not at all.** Without
`DLG_UI_Image_07`, the arrow buttons' textures don't resolve. Nothing has
shown that a button with no texture keeps a clickable hit area. If it
doesn't, page 2 (Hamel, Sander, Lanox) is unreachable.

The March village level requirements were checked by decompiling
`kom_contents` `DLG_Map_Local_*.lua`: Ruben 1, Elder 6, Besma 15, Altera 27,
Peita 34, Velder 41, Hamel 48, Sander 58, Ranox 68. These are identical to
June's.

### Play-test checklist

Use **`reyaa` (uid 12, level 80)**. Every gate is level-only, so any
character at level 68 or above should see all nine tabs:
- `reyaa` 80, `Iivue` 80, `eresisu` 77 and `Freils` 72 see all nine.
- `ravbe` 67 sees no Lanox (Ranox requires 68).
- `arasss` 47 sees the right arrow, but page 2 is empty (Hamel requires 48).
  That is June behaviour.

1. **Before repacking** (only the new exe is live): the local map should look
   exactly as it did yesterday. `ErrorLog.txt` should hold one
   `[JUNE_LMUI] ... no LocalMap_arrowL/R` line.
2. **After repacking data034 and data215:**
   - Open the local map from a page-1 village, e.g. Ruben. Six tabs show
     (Ruben..Velder) with a right arrow and its backdrop. Hamel, Sander and
     Lanox do not show yet.
   - Click the right arrow. Hamel, Sander and Lanox show with a left arrow;
     page-1 tabs are hidden.
   - Click each of Hamel, Sander and Lanox. Each must open its own local
     map. Lanox must show Ranox's dungeon gates, and the hit-boxes must
     respond. The arrow state persists across tab clicks.
   - Click the left arrow. Back to page 1. Click each of the six. Each opens
     the right map.
   - Close and reopen the local map, and change village. It reopens on
     **whichever page you left it on**, as June does:
     - only the ctor and the two arrows write `m_bFirstPage`, and
       `OpenLocalMapDLG` does not reset it;
     - the object is owned by `CX2PartyUI` (`X2PartyUI.cpp:21`), which lives
       for the session.

     So from Hamel, Sander or Lanox, your own tab may be on the other page.
     That is expected; page across to see it checked.
   - Use a below-68 character (`ravbe`). There is no Lanox tab, and that is
     correct. Arrows still page.
   - Look for no `[JUNE_LMUI]` line, and no Lua error naming
     `DLG_UI_Local_Map_NEW` or `DLG_UI_Image_07`.

---

## Phase 3 pre-implementation findings (2026-10-01; decision put to the user, nothing built)

Every in-scope June and March local-map lua was decompiled into a scratch
directory. Neither game tree was written to. All 24 June files to be adopted
(8 villages × entry / `Back_NEW` / `Icon_NEW`) match June's kom index size
exactly, so none is truncated.

**Decision: March villages move to June's files.** There is no Icon_Info
path to keep: those files are orphans in March (Phase 0 correction 4), and
June has no separate progress mechanism. June's mechanism is its luas plus
its `UpdateDungeonButtons`. Names render on every village. The title statics
are unchanged in June's `Icon_NEW`, and Lanox's names are already live
(`data063.kom` `DLG_Localmap_Title_01.TET` = 4692 B, the LANOX Phase 2 merge).
"Progress" means no bars anywhere, per the 2026-10-01 decision.

What June's files change, per village (diffed against March's live copies):
- **`Back_NEW`:** identical apart from one added control,
  `Static_HeroMode_Filter` (`DLG_Map_Local_HeroMode_Filter.tga`, Phase 4's
  atlas).
- **`Icon_NEW`:**
  - the star statics are removed;
  - a third `AddDummyInt` (Hero ID) is added to each dungeon button. Ruben's
    is June's own `DI_NONE`, so 0 is a state June's C++ already handles;
  - **the Henir button (`emir_space`, `DI_ELDER_HENIR_SPACE`) is removed from
    all 7 villages that had it.** June moved Henir to the Special map
    (`DLG_Map_Local_Special_Icon_NEW`, `LMI_SPECIAL`), which Phase 2 dropped;
  - the secret-dungeon button's ID changes from `DI_*_SECRET_COMMON` to
    `DI_*_SECRET_HELL`, and a `SecretDungeonDesc` lock button is added on top
    of it;
  - the `DI_EVENT_MISSION_IMPOSSIBLE` dummy buttons are removed;
  - June-only controls are added: Besma's Bestera (`DI_BESTERA_START`,
    missing in March), `HamelSecret` (`DI_HAMEL_SECRET_HELL` has no March
    DungeonData) and `SanderSecret` (`DI_SANDER_SECRET_HELL`, missing in
    March).
- **Entry scripts:** the `DungeonIDList` changes mirror the icon changes
  (Henir out, `_COMMON` → `_HELL`, June-only IDs in). `DungeonIDList` feeds
  the tab quest marks and `CreateDungeonClearStars`, which builds Henir's
  `_LimitInfo` plate. So it must match whatever the icons end up carrying.
- **Peita:** June renamed the chain to `DLG_Map_Local_Peita_Back_NEW.lua`
  (data034) and `DLG_Map_Local_Peita_Icon_NEW.lua` (data217). Neither name
  exists in live March.

Couplings found:
- **The luas need June's C++.** March's `UpdateDungeonButtons` never hides
  `Static_HeroMode_Filter` or the `SecretDungeonDesc` overlays, so both would
  stay permanently visible. June's `UpdateDungeonButtons` (`FUN_3293b5e0`)
  hides the filter unless hero mode is on, accepts `GetDummyInt(1)` ∈
  {1, 2, 100}, and calls `FUN_32939aa0` for `SecretDungeonDesc`: a type-3
  control that builds a required-clears tooltip (string `0x9c0c`) and shows
  or hides itself. Porting both whole is part of this phase.
- **Dead controls must be dropped from the lua.** `SCRIPT_SELECT` is not
  defined for US, so March's no-DungeonData branch hides nothing. ~~June's loop
  does nothing when `GetDungeonData` is NULL~~ — **corrected during
  implementation:** June's loop hides every dungeon control first
  (`SetShowEnable(false,false)` before the data lookup), so a control with no
  DungeonData does stay hidden under the ported loop. They are dropped anyway,
  because they name June-only content (nil enums, or title keys that are
  missing or carry different art in March). The `DLT_EVENT_DUNGEON_TITLE`
  statics are **kept**, not dropped: see "Phase 3 result".
- **Secret IDs vs March's click handler.** Both `_COMMON` and `_HELL` are
  `DT_SECRET` in `march2014/data035/DungeonData.src.lua`, for Elder, Besma,
  Altera and Velder alike. March's `LMUCM_DUNGEON_CLICKED` (kept as March in
  Phase 2) maps `DT_SECRET` → `DM_SECRET_NORMAL`. Fed June's `_HELL` IDs, it
  would start a Hell dungeon ID in secret-normal mode. June maps
  `DT_SECRET` → `DM_SECRET_HELL`.
- **Henir has no other entry point in March.** Nothing outside the local-map
  icon sets `DI_ELDER_HENIR_SPACE` as the party dungeon.

Staging plan:
- June's `Icon_NEW` files for Altera, Besma, Hamel, Sander and Velder live in
  June's data217. March has them in data034. Patch them in
  `march2014/data034/` (the same duplicate-name hazard as Phase 0
  correction 1) and mirror them into `kom_contents/data034_kom/`.
- Only the two new Peita names go under `update/` with a manifest entry. Also
  patch March's `DLG_Map_Local_Peita.lua` `LOCAL_MAP_DLG`/`DLGBack`.

Open questions for the user: Henir's home, and the secret-dungeon mode (see
the chat report of 2026-10-01).

**User decisions (2026-10-01):** both recommendations accepted. Henir goes to
June's Special tab (option b), and secret dungeons follow June's click handler
(`DM_SECRET_HELL`, option a).

---

## Phase 3 result (built, deployed, packed and play-tested 2026-10-01)

**Confirmed 2026-10-01 (user):** all listed archives repacked and the play-test passed.

New flag `SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS` (`KTDXLIB/Always.h`). It needs
`SERV_IRUHADEV_JUNE_LOCAL_MAP_UI`.

### What changed

**C++**, each whole June function with March's in `#else`:
- `UpdateDungeonButtons` (June `FUN_3293b5e0`, ported from the disassembly):
  - every dungeon control is hidden first, then re-shown if playable;
  - `Static_HeroMode_Filter` is shown only while `m_bHeroMode` is on;
  - `GetDummyInt(1)` ∈ {1, 2, 100}; a 100 (`DLT_EVENT_DUNGEON_TITLE`) control
    is shown but not enabled;
  - hero mode reads dummy 2;
  - stars are not refreshed in hero mode;
  - an inactive Henir's `_LimitInfo` plate is hidden;
  - the drop-down for newly opened dungeons is unchanged, but June dropped
    March's Halloween case and the empty `CreateNewDungeonParticle` call.

  March calls June dropped: the hell-mode `<name>_Icon_Info` toggles and
  Tear-of-El-Woman (BR-only flag, inert in US). June-only systems are comments
  in place: the Elysion Atlas/Tower event flags, `CX2CustomEventManager`
  (absent from March), the Elysion page dummy strings, and the Event/Special
  NEW badges.
- `SecretDungeonDesc` (June `FUN_32939aa0`), new. June's "is it open"
  predicate (`FUN_327d9d50`) reads a server-fed open list and an entry-item map
  that March's `CX2DungeonManager` does not have. March has no open-day gate
  beyond `IsActiveDungeon`, which the caller has already passed, so June's open
  branch applies: the lock button is hidden. The closed branch's tooltip
  strings (`STR_ID_39948`/`41876`) are not in March's C++ string table; it is
  documented in place and not compiled.
- `UICustomEventProc` `LMUCM_DUNGEON_CLICKED` (June case 18000):
  - hero mode takes dummy 2;
  - difficulty is `heroMode ? DL_EXPERT : DL_NORMAL`. June's override through
    DungeonData `+0xb0/+0xb6/+0xb7` uses fields March lacks;
  - `DT_SECRET` → `DM_SECRET_HELL`, both in a party and solo.

  The novice-guide tail is unchanged (it is identical to June's).
- `UpdateLocalMapDLG`: the Special tab is re-shown by level alone, so it sits
  on both pages, and it is not counted toward page 1's six (June
  `FUN_3293c6f0`).
- `CX2LocationManager::LMI_SPECIAL = 10011`.
- **Secret-mode resets.** Three handlers reset a `DT_SECRET` party's mode to
  `DM_SECRET_NORMAL`:
  - `X2PartyManager.cpp` `Handler_EGS_CLOSE_PARTY_NOT`;
  - `X2StateBeginning.cpp` `Handler_EGS_SELECT_UNIT_ACK`;
  - `X2StateServerSelect.cpp` `Handler_EGS_SELECT_UNIT_ACK` (two copies).

  June's versions set `DM_SECRET_HELL`: `FUN_324331c0` (type 3 → 4) and
  `FUN_32563800` (type 3 → `EBX` = 4), read from the disassembly. Only that
  case is changed, under the same flag. Those June handlers differ elsewhere
  too, and those differences are left for a party-UI phase:
  - on party close, June leaves Henir at `DM_INVALID` (March:
    `DM_HENIR_PRACTICE`);
  - on unit select, June also sets Henir → `DM_HENIR_PRACTICE` and type 5 → 5.

**Secret-dungeon consumers checked** (the advisor review asked for every
reader of the dungeon ID, mode or difficulty on its own):
- `X2QuestManager::MakeSubQuestString`: quest-text formatting from quest data
  only.
- The offline quest code (`IsExistDungeonInSub`) and the party UI's
  ID + difficulty lookups (`X2PartyUI.cpp:1737/3069/5015/5059`) both use the
  sum, which is unchanged.
- Offline titles match the base ID plus difficulty. `SubTitleMission.lua`'s
  Hell titles name `DI_*_SECRET_HELL` with `DL_NULL`, so they **now match**;
  under March's old (`_COMMON`, `DL_HARD`) they never did. The normal-secret
  titles (`_COMMON`, `DM_SECRET_NORMAL`) become unreachable, as in June.
- No quest script names a secret dungeon.
- `X2GUNPC.cpp:1840` and `X2Game.cpp:16916` key on mode `HELL`, which is now
  set.
- `X2StateDungeonResult.cpp:2383` labels by mode, so it now reads Hell.

Henir resolving to `LMI_SPECIAL`:
- `X2PartyUI.cpp:1863`'s region-name switch already skips Henir
  (`IsHenirDungeon`).
- `X2UIQuest.cpp:795`'s quest-list region icon for Henir quests loses Elder's
  icon. This is cosmetic, and is what June does.

The kept Mission Impossible event titles can never show in March: those
dungeons (41030/41040) have no DungeonData, so the ported loop leaves them
hidden. Keeping them costs nothing; it is still a change from the "drop them"
I told the user before implementing.

Secret dungeons resolve as ID + difficulty in the client
(`X2PartyManager.cpp:3088`, `X2PartyUI.cpp:5015`) and in the offline emulator
(`Handlers_Room.cpp:1679`). So June's (`_HELL` ID, `DL_NORMAL`) plays the same
dungeon as March's (`_COMMON` ID, `DL_HARD`), for example 33031. The emulator
passes `m_cDungeonMode` straight through.

**Data** (all June files, decompiled from extracts verified against June's
kom index):
- **Per-village luas.** Ruben, Elder, Besma, Altera, Velder, Hamel and Sander
  `Icon_NEW`/`Back_NEW`/entry are patches in `march2014/data034/`. Peita's
  renamed chain is new:
  - `update/data034/DLG_Map_Local_Peita_Back_NEW`;
  - `update/data217/DLG_Map_Local_Peita_Icon_NEW`;
  - March's `DLG_Map_Local_Peita.lua` is repointed.
- **Dropped** (each leaves a comment in place):
  - Bestera (Besma, Altera);
  - the Hamel and Sander secret buttons, their lock buttons and their
    `TITLE_SECRET`;
  - June-only IDs in `DungeonIDList`. A nil entry would end
    `LocalParsing`'s read of the list early;
  - Ruben's `VMI_SPA_RUBEN`.
- **Kept**: the Hero third dummies (they read 0) and the Mission Impossible
  `DLT_EVENT_DUNGEON_TITLE` statics. This deviates from the pre-implementation
  plan: the 1/2/100 test is now ported, so they behave as in June.
- **Lanox**: `Ranox_Icon_NEW` and `Ranox_Back_NEW` were re-staged as June's,
  restoring the event titles and the hero filter that the 2026-09-18 port had
  to omit. The Lanox entry script was already June's.
- **Special** (`update/data217/DLG_Map_Local_Special{,_Back_NEW,_Icon_NEW}`):
  only Henir is kept, with its button, `TITLE_HENIR` and `STAR8`
  (`STR_ID_3722`, the static `CreateDungeonClearStars` anchors the
  `_LimitInfo` plate to). The tab requires level 40, as in June.
- **Registration and tab strip**: `LMI_SPECIAL` was added to
  `DLG_Map_Local.src.lua` and `DLG_Map_World_Elios.src.lua`'s `LocalIDList`.
  June's `LocalMapRadioBtnSpecial` was restored in
  `DLG_UI_Local_Map_NEW.src.lua`.
- **`Enum.src.lua`**: `LOCAL_MAP_ID.LMI_SPECIAL = 10011` and
  `DETAIL_LOCAL_TYPE.DLT_EVENT_DUNGEON_TITLE = 100`, both June's values.
- **`DLG_Map_Local_Henir_limit.lua` is kept as March's.** June's version is
  laid out for its `NEW_HENIR_DUNGEON` challenge counters. That flag is off for
  US, so March's C++ still writes `PC_%d.dds`. It belongs with June's Henir
  challenge system, not this phase.

**Verified:**
- Every enum and `STR_ID` the staged luas use resolves in March. The only
  exceptions are the 42 accepted Hero IDs.
- Every atlas key the kept controls draw exists in March or is staged here.
- All 34 luas compiled with `-v mar-2014`. Decompiling them back matches the
  sources, apart from comments.
- Unmodified June files recompile to exactly June's index size.

**Built**: `X2ServerProtocol`, `KTDXLIB`, `X2Lib`, `X2` as `US_SERVICE`, with
the PCH invalidated first. All four returned rc=0, and `X2LocalMapUI.cpp`
produced no warnings. The new code is proven to be in the exe:
`Static_HeroMode_Filter` and `SecretDungeonDesc` (both only in the new code)
are present as wide strings. `x2.exe` (18,981,888 B, 13:29) was copied to
`X2_offline.exe` and is byte-identical. After the reset-site edits, `X2Lib`
and `X2` were rebuilt (rc=0) and redeployed: 18,981,888 B, 13:42,
byte-identical. **Not launched.**

### Pack list

Everything is already in `kom_contents/dataNNN_kom/`, with the manifests
updated. The sizes below are the compiled sizes the live kom should show
after packing.

| Archive | Files (B) |
|---|---|
| `data034.kom` | Icon_NEW: Ruben 2741, Elder 9499, Besma 9101, Altera_Island 9296, Velder 8959, Hamel 10171, Sander 8522 · Back_NEW: Ruben 833, Elder 832, Besma 835, Altera_Island 814, Velder 836, Hamel 813, Sander 819, Ranox 815, **Peita 814 (new)** · entry: Ruben 894, Elder 1251, Besma 1313, Altera_Island 1337, Velder 1432, Hamel 1454, Sander 1226, Peita 1087 · `DLG_Map_Local.lua` 570 · `DLG_Map_World_Elios.lua` 376 · `DLG_UI_Local_Map_NEW.lua` 13689 |
| `data036.kom` | `Enum.lua` 265287 |
| `data217.kom` | `DLG_Map_Local_Ranox_Icon_NEW.lua` 9302 · **new:** `DLG_Map_Local_Peita_Icon_NEW.lua` 7471, `DLG_Map_Local_Special.lua` 388, `_Special_Back_NEW.lua` 818, `_Special_Icon_NEW.lua` 2474 |
| `data065.kom` | **new:** `DLG_Map_Local_HeroMode_Filter.tga` 1048620, `.tet` 104 |
| `data215.kom` | **new:** `DLG_New_Region_SPECIAL_01.dds` 262272, `.TET` 78 |
| **`data151.kom` (new archive)** | `DLG_UI_Text_14.tga` 1048620, `.TET` 4640 (Special tab art). This is a new archive number for March, the same precedent as `data217` (RANOX_PORT_PLAN rule 2). Source: `kom_contents/data151_kom/` (new dir and manifest). |

**Ship them together.** A new exe with old luas mostly looks as before, with
two side effects:
- a March secret icon (still its `_COMMON` ID) now starts in Hell mode, while
  the dungeon played stays the `_COMMON` one;
- March's hell-mode `_Icon_Info` overlays are no longer toggled.

New luas without `Enum.lua` read `LMI_SPECIAL` as 0, which would leave the
Special tab dead.

**Backup**: the live-kom originals of every file this phase overwrote in `kom_contents/` were
extracted to `...\24965799\kom_contents_backup_june_ui_phase3\dataNNN_kom\` (29 files, with a
README); the packer does not read that folder.

### Known follow-ups

- **Party UI difficulty checkbox on a secret dungeon (Phase 5).**
  `CX2PartyUI::SelectDungeonDifficulty` maps a Hard/Expert click on a
  `DT_SECRET` dungeon to `DM_SECRET_HELL` + `DL_HARD`, i.e. `_HELL` ID + 1,
  which does not exist. The map click itself never does this, but a manual
  difficulty click would. Phase 5 ports June's party UI.
- June's `IsActiveDungeon` (`FUN_327da190`) has a `RequireCompletedQuestID`
  gate and hero/Bestera handling. It is not ported; it belongs to
  `CX2DungeonManager`, not the local map.
- Henir's June limit-plate layout depends on June's `NEW_HENIR_DUNGEON`
  challenge system.

### Play-test checklist (after repacking every archive above)

Use `reyaa` (level 80):
1. **Every village, Ruben through Sander and Lanox:**
   - no difficulty stars;
   - name plates present;
   - dungeon buttons present per level;
   - **no Henir button on any village map**;
   - the map art is not dimmed (the hero filter must be hidden).
2. **Peita** opens its local map (renamed files).
3. **Special tab**, at level 40 or above:
   - it is visible on both pages, after Lanox on page 2;
   - clicking it shows the Special background, the Henir button, the
     `TITLE_HENIR` plate and the Henir reward-count plate;
   - clicking Henir selects Henir practice in the party panel and enters
     normally.
4. **Secret dungeons** (Elder, Besma, Altera, Velder), with expert clears:
   - the button shows and the lock overlay is hidden;
   - clicking it sets Hell in the party panel;
   - entering plays the `_HELL` dungeon.

   **Do not click the party panel's difficulty checkboxes on a secret
   dungeon** (see follow-ups).
5. **Mission Impossible event titles** stay hidden unless that event dungeon is
   active.
6. **Logs**: `ErrorLog.txt` / `OpenScriptFile.log` should show no Lua error
   naming `DLG_Map_Local_*`, `DLG_UI_Local_Map_NEW` or `Enum.lua`, and no
   missing-texture line for `DLG_UI_Text_14`, `DLG_New_Region_SPECIAL_01` or
   `DLG_Map_Local_HeroMode_Filter`.
7. **Live koms**: `elstools inspect` on each archive should show the sizes in
   the pack list.
