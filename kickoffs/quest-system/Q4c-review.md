# June quest system - Phase Q4c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q4c review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase Q4c done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_RESULT_RANK` |
| Log tag | `[JUNE_QUEST] RANK` (`python tools/logs.py --grep "JUNE_QUEST] RANK"`) |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Rules" "Phase Q4c - Dungeon result ranks offline" "Q4c - Done"
```

## Task

Review Q4c's commit against March's server, the reference (no June code is involved):
- `KResultProcess::Result_DUNGEON` (`KncWX2Server/CenterServer/ResultProcess.cpp` 1397-1487, 1696-1703, 1944-2063): call order, arguments
  (`iDungeonID + cDifficulty`, `iEndNumMember`, `iPartyTotalGivenDamage`, `m_nSec`), the `GetPercent` / `GetRank` read-back, the total's clamps,
  the 0.15 cap and `m_nRankBonusEXP` = `(m_nEXP + m_nPartyBonusEXP) * rate`; the dead branch.
- The bindings (`ResultProcess.cpp:54-101`, `RoomUser.h:128`, `:251-260`, `:286-288`, `DungeonMonsterManager.h:65-69`): every Lua name the 8 functions of
  `ServerResource/US/ResultData_new.lua` call is bound, and each getter returns the same field the server's does (`GetBaseHP` = game stat).
- The die count (`DungeonRoom.cpp:6170-6244`, `IsDropDieState` :2576-2595, `GetMonsterTypeFactor` `XSLUnitManager.cpp:238-269`): the gates, no
  attacker test, attribute monsters absent offline.
- The play time (`ComputeTotalPlayTime` :2632-2634, the loading timer :1065-1070 / :2189-2190 / :3643) against `m_fPoisonLoadingSec`.
- The move of `QuestOnDungeonClear` (`GSUserDungeon.cpp:365-719`, the quest / title hooks before the result is sent; `m_nSec`, `m_nDamaged`,
  `m_mapHaveExpInDungeon`), and that nothing in `Handler_EGS_END_GAME_REQ` still needed it there.
- The rank-bonus payout (`ApplyDungeonReward`, the second `EGS_UPDATE_UNIT_INFO_NOT`): idempotent on the client (`X2StateDungeonGame.cpp:3217-3300`,
  `X2Game.cpp:9740`), level-up handled.
- `lua_tinker::call`'s error path and the pad / `lua_settop` guard in `CallAxis` and the total call.

Hygiene is one command:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_QUEST_RESULT_RANK --range <Q4c commit>^..<Q4c commit>`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. The two new files' FAIL rows (their `#pragma once` / `#include "stdafx.h"`) are expected;
explain every OTHER row and every deleted line.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write
"Q4c - Review" into the plan doc, update the Q4c STATUS.md row, and commit.
