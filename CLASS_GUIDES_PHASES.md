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

**Run with:** Sonnet 5.5 high, user play-test. **Depends on:** char-ara CHNb review.

**Scope:** stage June's 1ST / 2ND guide dialogs for Elsword, Aisha (`Aisha`), Rena (`Lena`), Raven, Eve and Chung, plus their textures
(`stage-june --dry-run` first; Rena's and Raven's dialogs are already in data034). Transcribe June's level-15 and level-35 `LevelUpEvent` rows
for the six characters into data036 `LevelUpEvent.lua`, then `luasrc.py refresh`. The level-70 transcendence rows stay as they are. Run
`luabind.py check` on the 12 dialogs: every `ENUM MISSING` gets its June member in `Enum.lua`, and an unbound binder is bound or explained.
Pack and copy data034 / data036 (and any other kom the closure touches).

**Done gate:** for each character, a level-14 base unit opens the June 1ST guide and logs `1st: 3 class description(s) wrapped`. A level-34
unit of each first job logs `2nd: class <c> line <l> -> next class <n>`, with the right next class and buttons, and `2nd skills` with icons
and rows above 0. Buy opens the next class's change item. 0 EXCEPTION.
