# June quest system - Phase Q4b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q4b review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase Q4b done (its Done note and commit) and its play-test |
| Flags | `SERV_IRUHADEV_JUNE_QUEST_OFFLINE` (accept items), `SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN` (exception list) |
| Log tag | `[JUNE_QUEST] OFF` (`python tools/logs.py --grep "JUNE_QUEST] OFF"`) |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Rules" "Q4b - Done"
```

## Task

Review Q4b's commit against March's server, the reference (June's client never reads either key; no June code is involved):
- The parse of `m_vecAcceptRewardItem` (`XSLQuestManager.cpp:121-141`, same guard `0 != id && 0 != quantity`, map insert so the first entry of an id wins)
  and `m_eExceptionDungeonID` (ids > 0, one table in the SQT 8 / 32 / 48 case body). Check all 7 quests and all 6 sub-quests of
  `june_src_fixed/data036_kom/FieldQuest.src.lua` / `FieldSubQuest.src.lua` reach the templet (the log's `paid accept item` / `m_eExceptionDungeonID` lines).
- `PayAcceptItems` against `UserQuestManager.cpp:694-765`: ERR_QUEST_24 (inventory or bank), ERR_QUEST_25 (space), items without a templet skipped,
  and that nothing is inserted unless the quest is then taken. `HasRoomFor` is per item (optimistic for two items sharing the last slot): no live
  quest has two accept items.
- The slots reach the client: `EGS_NEW_QUEST_NOT::m_vecUpdatedInventorySlot` on the REQ path and `PushNewQuest` (auto-open: 30130 is an epic), read by
  `X2UIQuestNew.cpp:2133`. The give-up ACK carries the deleted rows as `GSUserGameCommon.cpp:1835-1855` does, but `Handler_EGS_GIVE_UP_QUEST_ACK`
  (`X2UIQuestNew.cpp:1909`) never reads them (March's own client): say whether that leaves a stale bag until re-select and whether it matters.
- The exception check in `OnDungeonClear` (placed after the dungeon filter, before the type switch) and in `IsTogetherRunMet`; entries are dungeon id +
  difficulty (`iDungeonID + cDifficulty`, the packed number `IsExistDungeonInSub` uses). Confirm against `DungeonEnum.lua` that the listed ids are
  `base + difficulty` values (43640's 100xxx / 110xxx are the hero / TC ranges).

Hygiene is one command per flag:
`python tools/flagcheck.py JUNE_QUEST_OFFLINE --range <Q4b commit>^..<Q4b commit>` and the same for `JUNE_QUEST_OFFLINE_STANDIN`.
Paste the `files:` / `in this flag's files:` / `RESULT:` lines; explain every OTHER row and every deleted line.

Fix clear transcription errors under the same flags (then `tools/build.py --deploy`); hand design decisions back to the user. Write "Q4b - Review" into
the plan doc, update the Q4b STATUS.md row, and commit.
