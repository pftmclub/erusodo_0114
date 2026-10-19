# June quest system - Phase Q4b - Accept reward items and the exception dungeon list

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase quest-system Q4b` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q3 committed (it owns `X2QuestManager.*`; Q4 split this half off because Q3 was editing those files) and Q4 review |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_OFFLINE` (accept items) and `SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN` (exception list) |
| Log tag | `[JUNE_QUEST] OFF` |
| Review | `kickoffs/quest-system/Q4b-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Rules" "Q4 - Done"
```

## Task

Two server-only keys June's client never reads, so the client's parser must read them for the emulator (user answers, 2026-10-19):
1. **`m_vecAcceptRewardItem`** (7 quests: 30130, 30150, 30160, 30170, 30180, 30340, 35203; each `{ m_iItemID, m_iQuantity }`). User: pay on accept,
   as March's server pays `m_mapAcceptRewardItem` (`UserQuestManager.cpp:694-764`):
   - refuse with `ERR_QUEST_24` when the character already holds the item (inventory or bank, `IsExistInventoryItemWithBank`);
   - refuse with `ERR_QUEST_25` when the bag has no room;
   - otherwise insert the items and carry the slots in `EGS_NEW_QUEST_NOT::m_vecUpdatedInventorySlot`. That is the REQ path (`Handlers_Quest.cpp`
     `Handler_EGS_NEW_QUEST_REQ`, whose comment says the client has no such field) and the auto-open paths (`PushNewQuest`; 30130 is an epic).
   - Parse it in `X2QuestManager.cpp`'s quest parser (CP949: `bytepatch.py`), next to Q1's June quest-level keys, into a new `QuestTemplet` member.
2. **`m_eExceptionDungeonID`** (6 sub-quests; the live one is account event quest 43710 "clear 5 dungeons, Ruben excluded"). User: "follow June's":
   a listed dungeon never counts. Parse it into `ClearCondition` and apply it in `CX2OfflineQuest::OnDungeonClear`
   (SQT 8 / 32 / 48) under the stand-in flag. Say whether the list holds base ids or id + difficulty (43640's list has 100xxx / 110xxx ids).

Both are emulator-side stand-ins over a client parse. Log each refusal / exclusion with `[JUNE_QUEST] OFF`. Do not touch Q3's
`X2QuestManager` runtime code.
