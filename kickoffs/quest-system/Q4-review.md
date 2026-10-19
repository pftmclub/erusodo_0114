# June quest system - Phase Q4 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q4 review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase Q4 done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_OFFLINE` (+ stand-in `SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN`) |
| Log tag | `[JUNE_QUEST] OFF` (`python tools/logs.py --grep "JUNE_QUEST] OFF"`) |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Rules" "Phase Q4 - Offline completion of every sub-quest type" "Q4 - Done"
```

## Task

Review Q4's commit against March's server (the reference) and the three June rules it mirrors:
- `IsSuitableLevel` vs `KncWX2Server/CenterServer/Room.cpp:1593-1627` (band, slack 2, id + difficulty row, level at loading 100) and
  `UserQuestManager.cpp:2375-2420` (the exclusions; `IsQuestCountedDungeon` vs `CXSLDungeon` and `Handlers_Skill.cpp`'s copy).
- SQT 8 rules vs `UserQuestManager.cpp:2076-2097`; the counted-type branch vs `:2515-2539`; `HasDungeonFilter` vs `CheckValidDungeonID`
  (`XSLQuestManager.cpp:2089`), at all 5 sites, and whether `OnVisitDungeon` (no filter bypass) still matches the server.
- `CheckJuneRepeat` vs Ghidra `32322050` (week), `32321d10` (month), `3232caf0` (chain; what the `*templet == -1 || == +0x120` test is).
- Account quests: `CheckAccountOpen` vs `UserAccountQuestManager.cpp:361-410`; `Load` / `SaveKey` / `Finish` / `GiveUp` never write an account quest
  under the unit, or a unit quest under the account key.
- `IsClearTypeUndriven`: each new reason true (spot-check the orphan claims with `june2017_re/quest_plan/qlib.py`), and the census line
  `SQT coverage 0-48: 21 driven, 28 undriven, unclassified [none], both [none]`.
- The transcendental refusal (42030 only) and the QRT_DAY hour (US 0, `XSLQuestManager.h:36-40`).
- The play-test's one cosmetic finding: `Handler_EGS_GAME_LOADING_REQ` also runs for a field, so `IsSuitableLevel` logs "dungeon 0 has no
  DungeonData row". Skip the call when `m_kRoom.m_kInfo.m_iDungeonID` is 0 (same flag).

Hygiene is one command: `python tools/flagcheck.py JUNE_QUEST_OFFLINE --with JUNE_QUEST_OFFLINE_STANDIN --range <Q4 commit>^..<Q4 commit>`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write
"Q4 - Review" into the plan doc, update the Q4 row in `STATUS.md`, check `kickoffs/quest-system/Q4b.md` and `RM1.md`, and commit.

## Amended by the Q3 review (2026-10-19)

- **Counts over 255.** Q4's Done note says "no live sub-quest counts past 255, so `KSubQuestInstance` stays UCHAR". June's
  `june_src_fixed/data036_kom/FieldSubQuest.src.lua` has 8 SQT_NPC_HUNT sub-quests above 255: 36020 (380), 203490 / 203530 / 203580 /
  204950 (300), 203500 / 204400 / 206510 (500); also SQT 39 honor points 4530 / 8400 / 12900 (guild, undriven) and one collection of 333
  (inventory count, not the UCHAR). Check whether the emulator's hunt counter wraps or stops at 255 for these, and whether any of their quests
  is reachable offline. If one is, widening `m_ucClearData` to June's `short` is a Q4 fix (shared struct: Rules "Shared structs").
