# Offline monster grade - Phase 3c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase offline-grade 3c review` |
| Plan doc | `OFFLINE_MONSTER_GRADE_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | Phase 3c done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_OFFLINE_MONSTER_GRADE` |
| Log tag | `[OFFLINE_GRADE]` |

## Read first

```sh
python tools/docsection.py OFFLINE_MONSTER_GRADE_PHASES.md "Rules" "Phase 3c - Die states like the server's IsDropDieState" "Phase 3c - Done"
```

## Task

Review Phase 3c's commit against the server. Each row of the Done note's gate table is a claim to
re-open at its line:
- **Dungeon:** `KDungeonRoom` `ERM_NPC_UNIT_DIE_REQ` (`KncWX2Server/CenterServer/DungeonRoom.cpp:5963-6962`,
  `SERV_REFORM_ITEM_DROP`):
  - the drop gate `:6240` and its `else` broadcast `:6940-6960`;
  - `bAddRoomUserKillCount` `:6060` and `:6187`;
  - the NO_DROP / templet early outs.
- **Field:** `BattleFieldNormalDropProcess` (`BattleFieldRoom.cpp:5024-5507`) and
  `BattleFieldMiddleBossDropProcess` (`:4378`):
  - no `else` broadcast;
  - the damage-map membership `:5191` / `:4550`;
  - `m_bQuestComplete` `:5211` / `:4575`;
  - the drop-row return `:5127-5145`.
- **GS:** `GSUserRoomCommon.cpp:1916-1980` (quest and title before the NO_DROP return) and
  `:2229-2386` (only on `m_bQuestComplete`).
- **Eater half:** `KDropTable.cpp:1873-1901`:
  - the seven enum names map to the right items;
  - the eater item is pushed before the grade item;
  - a field middle boss is skipped.
- **Client die state:** `X2GUNPC.cpp:13944-13981`. Settle `ADD_VELDER_ACADEMY_EVENT_MONSTER`
  with a `#pragma message` or `cl /EP` probe; the phase only read it.
- **The `#else`:** the original gate, kill count and `QuestOnNpcDie` still compile flag-off.
  `bCountKill` dereferences `pDieTemplet` only behind `bKnownNpc`.

- **Designed follow-up** `SERV_IRUHADEV_OFFLINE_FIELD_QUEST_NO_ROW_DESIGN` (user OK, "designed,
  not server/June"). It waives only the field drop-row test for quests. Check that it keeps the
  `bPayReward` and 0.5-share conditions, and that it is labelled as designed in code, `Always.h`,
  `MODS.md` and the Done note.

Hygiene is one command per flag:
`python tools/flagcheck.py SERV_IRUHADEV_OFFLINE_MONSTER_GRADE --range e6cc509^..HEAD` and
`python tools/flagcheck.py SERV_IRUHADEV_OFFLINE_FIELD_QUEST_NO_ROW_DESIGN --range <its commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

If the user has play-tested, read `python tools/logs.py --tag OFFLINE_GRADE`: the `die state` lines
are the evidence for the Done gate.

Fix clear transcription errors under the same flag, then run `tools/build.py --deploy`. Hand
design decisions back to the user.

Finish:
- write "Phase 3c - Review" into the plan doc;
- update the STATUS.md row;
- commit.
