# June quest system - Phase Q5a - Quest UI: window, tabs, list / detail / quick, accept / complete / give-up

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase quest-system Q5a` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q0b, Q2 review |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_UI` |
| Log tag | `[JUNE_QUEST] UI` (`python tools/logs.py --tag JUNE_QUEST`) |
| Review | `kickoffs/quest-system/Q5a-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Context" "Rules" "Starting context" "Phase Q5a - Quest UI: window, tabs, list / detail / quick, accept / complete / give-up" "Phase Q0b - Quest UI, packet and movie map"
```

## Task

Do "Phase Q5a" from Q0b's rows targeted Q5a (`june2017_re/quest_plan/q0b_ui_map.tsv`).
- **`X2UIQuestNew.cpp` is CP949:** `bytepatch.py`.
- **DLGs.** If Q0b says so, adopt June's quest DLGs together with the code. Pack, then ask before the copy.
- **Scene calls stay March's**; they are Q5b's.

Done gate as in the section.

## Amended by Q0b

Rows: `june2017_re/quest_plan/q0b_ui_map.tsv`, target `Q5a`. Funcmap rows `(Quest Q0b)`.
- **Dispatcher.** June's `323e5fb0` has 12 cases; March's `UIServerEventProc` (`:1313`) has 7. June ids in this block are March ids - 9.
  Ported handlers attach to the **March** event ids: the emulator and client share one exe, so no June numbering is needed.
  - Port whole: `0xec` NEW_QUEST_NOT `323e4cb0`, `0xf1` QUEST_COMPLETE_ACK `323e57e0`, `0xf5` ALL_COMPLETED ACK `323e5c10`,
    `0xf8` GIVE_UP ACK `323e4aa0`, `0xfa` TALK_WITH_NPC ACK `323e4930`. `0xeb` NEW_QUEST_ACK is already March's.
  - `0x27c` `323e08c0` is June's UPDATE_QUEST_NOT moved into this window. Port its body onto `EGS_UPDATE_QUEST_NOT`, and retire
    `CX2State::Handler_EGS_UPDATE_QUEST_NOT` (`X2State.cpp:4776`) behind the flag, so it is not handled twice.
  - June-only server pushes `0x27b` `323e05f0`, `0x27d` `323e15e0`, `0x593` `323e1710`, `0x605` `323cf130`: port the bodies as handlers of new
    event ids appended at the end of the client event list, behind the flag. Q4 decides which ones the emulator sends.
    `0x604` is dead (its request `0x603` is unreachable): skip it.
- **Fix a wrong citation.** `X2UIQuestNew.cpp:2032` (`SERV_IRUHADEV_ELYSION_VILLAGE`) adds a relation refresh to ALL_COMPLETED and
  cites `323e1710`. That is the `0x593` handler. June's ALL_COMPLETED (`323e5c10`) does not refresh. The `323e5c10` port drops that call
  under the new flag, and the `323e1710` port carries it.
- **UI functions.** Port whole: the ctor `323d0ca0`, `323ce6f0` (+ `NPC_FACE_EVENT`), `323cd570`, `323cdfe0`, `323ccbc0` (+ `Button_Skip_Epic_Quest`),
  `323c9e80` / `323ca520` (Guild / Story radios; `RaidoButton_Story` is June's own spelling, keep it), `323e5340`, `323dfa80`, `323d9400`,
  `323d4390`, and `UICustomEventProc` `323e1bf0`. Ghidra's body of `323e1bf0` stops early: its case bodies continue in `323e3e8d`-`323e4921`.
  Read them with `june2017_re/disasm_range.py`. Also port the request senders `323d2b40` / `323d24f0` / `323d1fc0`.
- **Skip epic quest.** `UQNCM_SKIP_EPIC_QUEST` (41045) opens a box. OK (41046) calls `323d24f0`, a normal QUEST_COMPLETE_REQ with an empty
  map. `322f4c60` is not involved: it is the dungeon-start ESC skip. `323ca880` decides when the button shows: read it.
- **New June custom messages** 41038-41047 (scenario movie x5, guild tab, story tab, skip epic, skip epic OK, skip epic scene).
  Add them to the C++ enum and to `UI_QUEST_NEW_CUSTOM_MSG` in core `Enum.lua` (text edit, then `luasrc.py refresh`; Q1 owns Enum.lua
  and is done before Q5a). The scenario-movie cases are Q8's: leave a log line in Q5a.
- **Payloads** (`q0b_pkt` layouts, quoted in the tsv): June's NEW_QUEST_NOT carries 2 extra int32 (`+0x34`, `+0x38`), and the
  complete REQ carries one more bool. Add a field only if a ported handler reads it. Name each one in the Done note, as a shared-struct
  change (`CommonPacket.h` -> `ServerDefine.h` second home).
- **DLGs: adopt June's (`ljconv`) in one data034 batch**, none of which has ever been staged:
  - with this phase's code: `DLG_UI_NEW_Quest_List` / `_Detail` / `_Quick`;
  - same-filename rule, no code: `DLG_UI_Quest`, `_Quest_Restor`, `_Request`, `_Quest_Clear` (their readers are `#ifndef SERV_EPIC_QUEST`,
    dead), `DLG_UI_Quest_Guide` (X2PlayGuide; check the control names), and `DLG_UI_Guide_Quest01` / `02`.
  - `DLG_UI_NEW_Quest_Clear` goes with Q5b.
