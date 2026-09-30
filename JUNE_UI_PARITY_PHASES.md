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

| Phase | Scope | Kind |
|---|---|---|
| 0 | Ghidra survey + gap table | read-only |
| 1 | Enums / custom messages the June dialogs need | C++ + Lua enum |
| 2 | Adopt June `DLG_UI_Local_Map_new.lua` (+ Button21 atlas, arrow paging) | C++ + data |
| 3 | June's local-map dungeon progress display | C++ + data |
| 4 | Local-map Mode Button (`DLG_UI_Local_Map_Mode_Button.lua`) | C++ + data |
| 5 | Party button `DLG_UI_PARTY_BOTTON_NEW_20161103.lua` + Party length | C++ + data |

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

Using Phase 0's finding on how June draws dungeon names, progress and clear
state without Icon_Info files, port that mechanism. Stage whatever June data
it reads, either per village or globally. Decide explicitly whether March
villages move to June's mechanism, adopting June's per-village
`DLG_Map_Local_*` files where they differ, or keep their Icon_Info path. The
end state must render names and progress on **every** village's local map,
Lanox included. If June's per-village files are adopted, diff each one
against March's staged patches (the Ranox Back/Icon/root files) so no Ranox
fix is lost.

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

Port June's `DLG_UI_Local_Map_Mode_Button.lua` feature (`FUN_32938450` →
`FUN_32938380`): what the button toggles, its owner class, its custom
messages, and the lua plus the atlas it needs. If the mode it switches to
depends on content March lacks, report that and ask before porting a dead
button.

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
