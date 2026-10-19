# June quest system - Phase Q4d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q4d review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase Q4d done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_UPPER_DIFF` |
| Log tag | `QUEST    VISIT` / `FINDNPC` / `CENSUS` lines (`python tools/logs.py --grep "upperDiff"`) |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Rules" "Phase Q4d - Sub-quest upper-difficulty flag" "Q4d - Done"
```

## Task

Short review. The reference is the March server, not June (June's client loader skips the key for these types; that is a client-only fact):
- Every `m_bUpperDifficulty` read in `KncWX2Server/Common/X2Data/XSLQuestManager.cpp` (883, 943, 1013, 1039, 1065, 1096, 1142, 1200, 1318, 1353, 1437)
  has a client read in `X2Lib/X2QuestManager.cpp` `LoadClearCondition` for the same type. Re-derive the list yourself; the Done note claims only
  `VISIT_DUNGEON`, `FIND_NPC` and `ITEM_USE` were missing (`DUNGEON_CLEAR_COUNT` / `SUITABLE_LEVEL_DUNGEON_CLEAR` already read it at the shared body).
- The three inserts sit inside their `case` braces, are optional (default false) and use the client's macro shape (`LUA_GET_VALUE`, as `X2TitleManager.cpp`).
- Nothing else that reads the client struct changes a label or a gate for these three types (`GetPartyUIQuestIcon` already forced upper for VISIT / FIND;
  the auto-description cases are in `#ifndef REFORM_QUEST`; `X2DungeonGame` reads it for `DUNGEON_TIME` only).
- The emulator's `IsExistDungeonInSub` call sites for `VISIT_DUNGEON`, `FIND_NPC`, `ITEM_USE` pass `iDungeonID + cDifficulty` (the half-compare needs it).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_QUEST_UPPER_DIFF --range <Q4d commit>^..<Q4d commit>`; paste its `files:` / `RESULT:` lines.
Fix clear errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "Q4d - Review" into the plan doc,
update the Q4d STATUS.md row, and commit.
