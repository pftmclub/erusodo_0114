# June class-change guides for the March characters

## Context

June shows its own job-advancement guides at level 15 (`DLG_1ST_Class_Change_Guide_<X>.lua`, the three first jobs) and level 35
(`DLG_2ND_Class_Change_Guide_<X>.lua`, my line's second job: name, portrait, skill icons, combo rows, Accept / Buy) for every character
(June `LevelUpEvent.src.lua`). char-ara CHNb / CHNb2 ported the code, and it is class-generic: `CX2LVUpEventMgr::UICustomEventProc` (June
`327127a0`, cases 1540-1552), `CX2LVUpEventMgr::LVUpEvent::InitClassChangeGuide` (`32713c10`), `CX2CashShop::OpenClassChangeGuideItem`
(`3222a4f0`), flag `SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE`. Only Ara's rows were switched over. The other six March characters still open
March's `DLG_Class_Change_Guide_<X>.lua` at level 15 and have no level-35 guide (user, 2026-11-11: "port it for the other characters so
there is no difference").

Out of scope: Elesis and Add (their own `char-elesis` / `char-add` CHNb phases), and Lu/Ciel, Rose and Ain (their character ports).

## Rules

CLAUDE.md and `JUNE_PORT_BACKLOG_ORDER.md` "Standing rules". June's dialogs are staged byte-exact (`kom.py stage-june`, closure included).
`LevelUpEvent.lua` is core-list BOM source: only the six characters' rows change, transcribed from June's `LevelUpEvent.src.lua`. Pack and
copy yourself. No C++ change is expected. If a guide needs code, it goes behind `SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE`.

## Starting context

Tools: `kom.py stage-june / pack / copy`, `luasrc.py refresh / ledger --check / core --check`, `luabind.py check`, `createchar.py`, `elsdb.py`
(see `tools/README.md`). Log tag `[JUNE_CHAR] CLASS GUIDE` (lines `1st`, `2nd`, `2nd skills`) and `[JUNE_SKILL] HUD shop open-at-item`.
The Ara Done notes, `JUNE_CHARACTER_ARA_PHASES.md` "CHNb - Done (2026-11-11)" and "CHNb2 - Done (2026-11-11)", describe the code and its logs.

## Phase G1 - June guides for Elsword, Aisha, Rena, Raven, Eve and Chung

**Run with:** Sonnet 5.5 high, user play-test. **Depends on:** - (may run beside the char-ara CHNb review).

**Scope:** stage June's 1ST / 2ND guide dialogs for Elsword, Aisha (`Aisha`), Rena (`Lena`), Raven, Eve and Chung, plus their textures
(`stage-june --dry-run` first; Rena's and Raven's dialogs are already in data034). Transcribe June's level-15 and level-35 `LevelUpEvent` rows
for the six characters into data036 `LevelUpEvent.lua`, then `luasrc.py refresh`. The level-70 transcendence rows stay as they are. Run
`luabind.py check` on the 12 dialogs: every `ENUM MISSING` gets its June member in `Enum.lua`, and an unbound binder is bound or explained.
Pack and copy data034 / data036 (and any other kom the closure touches).

**Done gate:** for each character, a level-14 base unit opens the June 1ST guide and logs `1st: 3 class description(s) wrapped`. A level-34
unit of each first job logs `2nd: class <c> line <l> -> next class <n>`, with the right next class and buttons, and `2nd skills` with icons
and rows above 0. Buy opens the next class's change item. 0 EXCEPTION.

### G1 - Done (2026-11-11)

Sonnet 5.5. Data only, no code, no flag; the deployed exe is CHNb2's (`DEPLOY OK` md5 `3b0f2108`). Full build skipped: no code change. `flagcheck` / `junediverge` n/a (no source file changed).
- **Dialogs:** `kom.py stage-june --dry-run` (12 names): 8 new, 4 replaced (Lena, Raven; the live ones were not June's bytes), 0 identical; staged, then `luasrc.py adopt-june --list` re-made all 12 as June's `ljconv` bytecode
  (ledger rows). The 38 textures the dialogs name (`DLG_CU_*_1_1..2_3`, `DLG_UI_Image_09 / 10`, ...): `stage-june --dry-run` 0 new / 0 replaced / 38 identical, nothing to stage. Skill icons: not part of the dialogs (code reads them).
- **`LevelUpEvent.lua`** (data036, core-list source, BOM + CRLF kept): level-15 `TYPE0-5` now name `DLG_1ST_Class_Change_Guide_<X>`; level-35 `TYPE0-5` gain `DIALOG_LIST { DLG_2ND_Class_Change_Guide_<X> }` before their `SOUND_LIST`.
  A diff of the file against June's `LevelUpEvent.src.lua` shows only Elesis / Add level-15 (their own CHNb) and Luciel / Rose / Ain level-15 rows (their ports) as differences. Conditions of the 35 rows were already June's. Level-70 event untouched.
- **Gates:** `luasrc.py refresh data036 LevelUpEvent.lua` ok; `ledger --check` PASS (0 FAIL, 0 WARN, after adopt-june); `core --check` PASS; `luabind.py check` on the 12 dialogs: 21 / 22 methods each, 0 unbound, ENUM 0 / 0 / 0, RESULT: PASS (no `Enum.lua` change needed).
- **Pack / copy:** `PACK OK data034` (+8 dialogs, ~4 replaced), `PACK OK data036` (`~ LevelUpEvent.lua`); `COPY OK data034` md5 `91ad5554`, `COPY OK data036` md5 `b2f7eaec`.
- **Units (save, backup `db_backup/els_db_20261111_211941_pre_class_guides_g1.sql`):** account `unit_slots` raised 100 -> 130 (the 100 cap blocked `createchar`). Level-14 base units at exp 81600 (100 below level 15) and level-34 first-job units at exp 1212400 (100 below 35):
  Elsword 111 `G1ElsBase`; 112 `G1ElsKn` (10), 113 `G1ElsMK` (11), 114 `G1ElsSK` (22) / Aisha 115 `G1AisBase`; 116 `G1AisHM` (14), 117 `G1AisDM` (15), 118 `G1AisBM` (23) / Rena 119 `G1RenBase`; 120 `G1RenCR` (12), 121 `G1RenSR` (13), 122 `G1RenTR` (24) /
  Raven 123 `G1RavBase`; 124 `G1RavST` (16), 125 `G1RavOT` (17), 126 `G1RavWT` (25) / Eve 127 `G1EveBase`; 128 `G1EveEG` (18), 129 `G1EveAr` (19), 130 `G1EveEl` (26) / Chung 131 `G1ChuBase`; 132 `G1ChuFG` (20), 133 `G1ChuSG` (21), 134 `G1ChuShG` (27).
- **Not in scope / nothing routed:** no June guide needed code beyond `InitClassChangeGuide`. March's own `DLG_Class_Change_Guide_<X>.lua` remain in data034, unused.
- **Play-test (2026-11-11): PASS.** `1st: 3 class description(s) wrapped to 180` x 6 (all six base units); `2nd: class <c> line <l> -> next class <n>` x 18, every pair as expected
  (10/11/22 -> 100/101/112, 14/15/23 -> 104/105/113, 12/13/24 -> 102/103/114, 16/17/25 -> 106/107/115, 18/19/26 -> 108/109/116, 20/21/27 -> 110/111/117; buttons 1541/1545, 1542/1546, 1543/1547), each with `2nd skills` 4 icon(s), 3 command row(s);
  `HUD shop open-at-item` on 8 Buy presses (first items 242310, 214450, 216420, 217930, 217850, 252670, 214500, 217980, 235120); 0 EXCEPTION in `ErrorLog.txt`, `offline_server.log`, `offline_packets.log`. Only the known `AddGroupChainQuest` unbound name (ChainQuestInfo.lua, backlog line 314).
- **User remark (Aisha / Void Princess screenshot):** the 2ND guide's description body is black on the dark panel. It is June's data (script colour black, `#CX` resets to it): kept verbatim, `JUNE_BUGS.md` row 607. A fix would be the user's call (own `SERV_IRUHADEV_JUNE_FIX_*` flag).
