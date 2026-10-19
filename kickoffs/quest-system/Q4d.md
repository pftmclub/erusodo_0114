# June quest system - Phase Q4d - Sub-quest "or harder difficulty" flag

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase quest-system Q4d` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q4 review (done); found by Q9 play-test 1. Run after Q9 session A so the deployed exe does not change mid-walk (the build refuses while the game runs anyway) |
| Flag | new `SERV_IRUHADEV_JUNE_QUEST_UPPER_DIFF` (Always.h; needs `SERV_IRUHADEV_JUNE_QUEST_OFFLINE`) |
| Log tag | `[JUNE_QUEST] OFF` (the existing `VISIT` / `FINDNPC` lines already print the flag) |
| Review | `kickoffs/quest-system/Q4d-review.md` (the phase writes it when done) |

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

## Task

1. **Audit** which sub-quest types the server parser reads `m_bUpperDifficulty` for and the client parser does not. A quick scan of the two files gives:
   `DUNGEON_CLEAR_COUNT`, `FIND_NPC`, `ITEM_USE`, `SUITABLE_LEVEL_DUNGEON_CLEAR`, `VISIT_DUNGEON` (server only). Treat it as a lead: the client file
   groups cases, so read each case yourself. For each type say whether the emulator's counter for it uses `IsExistDungeonInSub` (and so the flag).
2. **Fix** by reading `m_bUpperDifficulty` in the client parser for exactly those types, optional with default false (the server's read is strict, the
   client's must not start failing a load), under the new flag, original code in `#else`. The emulator's `IsExistDungeonInSub` then does the rest.
   Check the data: how many June quests carry the flag on these types (`june_src_fixed/data036_kom/FieldSubQuest.src.lua`).
3. **Check what else reads the client struct**: the quest UI's dungeon text, `IsAnyDungeonCondition`, the Q4 coverage census. A flag that was always
   false must not change a label.
4. Proof: dry check on the data (list of sub-quests whose behaviour changes), then a play-test: Burnt Forest **Expert** now counts for 22182 (VISIT) and
   for 22183 (CLEAR); a Normal-only sub-quest without the flag still refuses Hard. Hand-off per the standing format (character jtLordKnight, unit 25).

No new `.lua`, no server change, no data change. Done gate: the log line for 22182 on Expert reads `upperDiff=1` and the sub-quest completes.
