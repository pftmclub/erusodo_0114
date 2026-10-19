# June quest system - Phase Q4d - Sub-quest "or harder difficulty" flag

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase quest-system Q4d` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q4 review (done); found by Q9 play-test 1. **Blocks Q9** (the walk is paused at A3) |
| Flag | new `SERV_IRUHADEV_JUNE_QUEST_UPPER_DIFF` (Always.h; needs `SERV_IRUHADEV_JUNE_QUEST_OFFLINE`) |
| Log tag | `[JUNE_QUEST] OFF` (the existing `VISIT` / `FINDNPC` lines already print the flag) |
| Review | `kickoffs/quest-system/Q4d-review.md` (the phase writes it when done) |
| Model note | the investigation is done (below); the fix is a parser change, Sonnet is enough |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Phase Q4d - Sub-quest upper-difficulty flag" "Rules"
```

## Why (Q9 play-test 1, 2026-10-19)

Quest 22162 sub 22182 "Enter Burnt Forest" (`SQT_VISIT_DUNGEON`, dungeon 33270, `m_bUpperDifficulty = True`): entering Expert (33272) did not count,
log `QUEST    VISIT quest 22162 sub 22182 wants dungeon 33270 (upperDiff=0, exact match only), got 33272 - no match`. The server's parser reads the
flag for that type (`KncWX2Server/Common/X2Data/XSLQuestManager.cpp:1293-1318`) and `IsExistDungeonInSubQuest` (:2158) then accepts the same dungeon at the same
or a harder difficulty. The client parser (`X2Lib/X2QuestManager.cpp`, `SQT_VISIT_DUNGEON` ~3223) never reads the key, the emulator copied the client's
struct, so the flag is always false there. June's own client loader skips it for these types too (`q0a_june_clearcond.md`); that is a client-only fact.

## What Q9 found (Opus, 2026-10-19, user: "June removed the difficulty UI of every dungeon before Elysion")

- **June fixes old dungeons at their top difficulty.** June's `DungeonData.lua` sets `FixMaximumDifficulty = TRUE` on 54 rows (Elder ... Ranox, Atlas;
  none in Elysion). March reads it since effect C3z-5 (`X2DungeonManager.cpp:236`, `IsFixMaximumDifficulty` :669; the party UI hides the difficulty
  boxes, `X2PartyUI.cpp:1867-1888` / `:5152`). So Burnt Forest can only be entered at Expert: the room log reads
  `ROOM dungeon room 1001 dungeonID=33270 dif=2` and the visit check gets 33272. **Normal is not selectable: the visit sub-quest cannot be done at all.**
- **June's data expects that**: sub-quests naming a low difficulty of these dungeons mostly carry `m_bUpperDifficulty = True` (rough census of June
  FieldSubQuest: VISIT_DUNGEON 18 true, FIND_NPC 5 true, DUNGEON_CLEAR_COUNT 31 true / 4 false, QUEST_ITEM_COLLECTION 29 true / 3 false, NPC_HUNT 28 true;
  the dungeon-name map used was incomplete, so recount).
- **The emulator drops the flag for some types.** Its load census (`offline_server.log`, `QUEST    CENSUS clearType=N : x sub-quest(s), y of them upperDiff=1`):
  type 26 VISIT_DUNGEON 155 / 0, type 27 FIND_NPC 43 / 0, type 32 SUITABLE_LEVEL_DUNGEON_CLEAR 89 / 0, type 22 ITEM_USE 1 / 0; type 2 NPC_HUNT 314 / 412,
  type 4 QUEST_ITEM_COLLECTION 364 / 745, type 8 DUNGEON_CLEAR_COUNT 102 / 139 do read it.
- **The server reads it** for VISIT_DUNGEON (`KncWX2Server/Common/X2Data/XSLQuestManager.cpp:1293-1318`), FIND_NPC (~:1353), ITEM_USE (:1437, :1898) and
  SUITABLE_LEVEL (:1096); `IsExistDungeonInSubQuest` (:2158) matches "same dungeon, same or harder difficulty". The client parser
  (`X2Lib/X2QuestManager.cpp` ~3223 VISIT, ~3243 FIND_NPC) does not. For type 32 check whether `SERV_SUITABLE_LEVEL_DUNGEON_CLEAR_SUB_QUEST` is defined
  (it groups 32 into case 8's body at ~2925) and why the census still shows 0.
- **A blocked visit also blocks the clear**: 22162's `m_vecSubQuest_Group` is `{ {22182}, {22183} }`, so `CheckBeforeGroup` (`X2OfflineQuest.cpp`
  OnDungeonClear) refuses 22183 until 22182 is done, although 22183's own filter matches 33272 (`upperDiff=1`).
- **Effect**: every Lanox / Atlas (and older) quest that starts with "Enter X" cannot progress offline. Q9 is paused at A3 until this lands.

## Task

1. **Audit** types 22, 26, 27, 32 (and any other type the census shows at 0 while June's data carries the flag): read each client parse case and the
   server's. Recount with a dungeon-id map built from `DungeonEnum.h` expressions, not only literal `= N` lines.
2. **Fix** by reading `m_bUpperDifficulty` in the client parser for exactly those types, optional with default false (the server's read is strict; the
   client's must not start failing a load), under the new flag, original code in `#else`. `IsExistDungeonInSub` then does the rest.
3. **Check what else reads the client struct**: the quest UI's dungeon text, `IsAnyDungeonCondition`, the Q4 coverage census. A flag that was always
   false must not change a label wrongly.
4. **List June's own unreachables**: sub-quests that name a below-top difficulty of a fixed dungeon WITHOUT the flag (rough census: clear-count 74200 /
   74210 / 204360 / 204370, item 9641 / 42050 / 42110). Port as June has them, JUNE_BUGS rows, no stand-in unless the user asks.
5. Proof: the census lines for 26 / 27 show non-zero upperDiff; play-test unit 25 `jtLordKnight` (state: 22162 ongoing, nothing counted): enter Burnt
   Forest (Expert, forced) -> "Enter Burnt Forest" ticks (`VISIT ... upperDiff=1`), clear -> "Clear Burnt Forest 1/1", 22162 completes. Then Q9 resumes.

No new `.lua`, no server change, no data change. Done gate: the log line for 22182 on Expert reads `upperDiff=1` and the sub-quest completes.
