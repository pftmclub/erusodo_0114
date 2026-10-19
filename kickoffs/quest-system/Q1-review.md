# June quest system - Phase Q1 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q1 review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q1 done (its Done note and commits), user play-test of the load census |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_LOAD` |
| Log tag | `[JUNE_QUEST] LOAD` (`python tools/logs.py --tag JUNE_QUEST`) |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Starting context" "Phase Q1 - Enums, loader keys, new binders, new June quest files" "Q1 - Done"
```
Then `june2017_re/quest_plan/q0a_june_clearcond.md` (the key table), `q0a_june_templets.md` and the Q1 rows of `q0a_engine_map.tsv` (target column contains `Q1`).

## Task

Review Q1's commits against the Q0a map. What this phase could get wrong:
- **Enum values** in `X2QuestManager.h` and in the merged `Enum.lua` against June's `Enum.src.lua` (QT 8-10, QRT 3-5, CJT 6 / END 7, SQT 37-48 / END 49, `QUEST_REWARD_TRAN_CLASS`,
  `PVP_TOURNAMENT_TYPE`, the NPC ids). The three `_END` readers (`CJT_JOB_END` loops in `X2QuestManager.cpp`, `X2UIQuestNew.cpp`, `X2UIQuestReceive.cpp`) now include `CJT_JOB_FOURTH`.
- **Parser deltas** against the key table: required vs optional per case (37 / 38 / 39 / 43-45), the `m_cDifficulty` post-switch list (2, 4, 5, 6, 7, 8 / 32 / 48, 12, 17, 22, 26, 27, 37),
  the 11 `m_eDungeonID` `>= 0` sites, `m_ePvpTournamentType` on cases 9 / 10, case 43's inverted loop (with the 1000-index cap), cases 46 / 47 (true, no keys), 40-42 / 19 / 20 / 29 / 30 / 35 (false).
- **Quest-level** keys: chain index and story index are written only for a stored quest (June writes while parsing, so a failed quest stays in its chain set); the QT 9 reward split; guild
  quests parsed then dropped (QT_GUILD or `m_bIsGuild`).
- **Scene-info loader** against `31eeb040`: the MENT keys are arrays of string ids, END_FRAME ints default -1, per-CHAR rows keyed by unit type. Spot-check one entry that has both a DEFAULT
  and a per-CHAR row.
- **Guild coop table**: `g_pGuildCoopQuestRewardTable` is the guild manager under a second name; `AddGuildRewardData` takes (reward id, item id, quantity, 4th).

Hygiene is one command: `python tools/flagcheck.py JUNE_QUEST_LOAD --range <first Q1 commit>^..HEAD`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review.
Spot-check 2-3 items in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "Q1 - Review" into the plan doc, update the Q1 row of `STATUS.md`, write or check `kickoffs/quest-system/Q2.md`, and commit.
