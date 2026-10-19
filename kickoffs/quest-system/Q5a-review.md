# June quest system - Phase Q5a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q5a review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q5a done (its Done note and commits), user play-test of the quest window |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_UI` |
| Log tag | `[JUNE_QUEST] UI` (`python tools/logs.py --tag JUNE_QUEST`) |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Starting context" "Phase Q5a - Quest UI: window, tabs, list / detail / quick, accept / complete / give-up" "Q5a - Done"
```
Then `june2017_re/quest_plan/q0b_ui_map.tsv` (rows targeted Q5a) and the new file `X2Lib/X2UIQuestNewJune.cpp`.

## Task

Q5a ported June's quest window as June* members in `X2UIQuestNewJune.cpp`; `X2UIQuestNew.cpp` only gained early-return dispatches
(one per replaced function) and a few in-place cases. The port was read from Ghidra decompiles written by a Sonnet session, so the
review is a second reading, not a skim. What it could get wrong:
- **The list builders** (`JuneUpdateOngoingQuestInfo` = June `323d5650`, `JuneGetUnitQuest` `323cf570`, `JuneUpdateAvailQuestInfo` `323d0770`,
  `JuneAddFirstStoryQuests` `323d4910`, `JuneAddNextStoryQuests` `323d4ae0`): state numbers (0..7), which vector each type goes to
  (disasm `323d5a60`-`323d5d70` settles it), the tab-priority flags, the story progress map (insert-if-absent, the erase / set-to-1 rule for
  completed story quests), the quick-quest push condition. Re-read 3 of these in Ghidra.
- **`JuneResetQuestList`** (`323d9400`): per-mode lists (request tab drops guild and event rows; guild tab = guild-type available + guild ongoing;
  story tab groups like the epic tab), the shared first-row clamp, `323dfa80`'s mode 0 -> 1 rule.
- **Slot rows** (`JuneUpdateUIQuestSlotInfo` `323d8be0`, `JuneSetUIQuestSlotInfoToCategory` `323d80d0`): the picture numbers against the adopted
  `DLG_UI_NEW_Quest_List` (python: print the `Static_QuestList0` picture list of `june_src_fixed/data034_kom/DLG_UI_NEW_Quest_List.src.lua`),
  the tri-state done / going mark, the open rule per mode, GET_STRING 40047 / 43475 (are those two ids in the string table? the log says).
- **`JuneUpdateQuestButton`** (`323ccbc0`): the register-lost buttons were assigned by meaning (Admin = the DEV block), not read; settle them with
  `june2017_re/disasm_range.py 323ccbc0 323cd490`. Also `JuneCanSkipEpicQuest` (`323ca880`) and the skip flow (`323e1bf0` cases 0xa055 / 0xa056).
- **Deliberately not ported** (each has an owner in the Done note): scenes, movie tab, friendship manager, story helper object, tutorial guide,
  menu marks, the four June-only server pushes and their payload fields, `UPDATE_QUEST_NOT` (June moved it into this window; March's
  `X2State.cpp:4776` is behaviour-equal and stays), `PlayQuestCompleteSound`'s class-built file name. Judge each; hand design calls back.
- **The in-place edits** to `X2UIQuestNew.cpp` (flagcheck says OTHER because two sit inside the Elysion `#ifdef`): TALK ACK dispatch, GIVE_UP `154`,
  NEW_QUEST_REQ billboard default, COMPLETE_ACK `11000`, ALL_COMPLETED relation refresh removed, CATEGORY_SELECT guild / story and the kept
  first row, scroll-down guard, `CreateStringQuestClearCondition` play-together filter.

Hygiene: `python tools/flagcheck.py JUNE_QUEST_UI --range <first Q5a commit>^..HEAD`. Expected: `X2UIQuestNewJune.cpp` FAILs only because a new file
must start with `#include "StdAfx.h"` outside the flag (as `X2GFXEventScene.cpp`); `X2UIQuestNew.cpp` is OTHER (Elysion `#ifdef` hosts two inserts);
`Always.h` is OTHER-define. Explain every other row.

Play-test log check: `python tools/logs.py --tag JUNE_QUEST` - `[JUNE_QUEST] UI` lines only (tab switches, skip-epic, rows without a templet).
