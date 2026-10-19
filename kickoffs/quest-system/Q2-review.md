# June quest system - Phase Q2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q2 review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q2 done (its Done note and commits), user play-test of the migration |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_DATA` |
| Log tag | `[JUNE_QUEST] MIG` / `LOAD` (`python tools/logs.py --tag JUNE_QUEST`) |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Starting context" "Phase Q2 - Data flip and old-save migration" "Q2 - Done"
```
Then `june2017_re/quest_plan/q2_census.tsv` (the census the user answered), `q2_expected_mig.txt` (what the rung should log) and the
generator `q2_gen.py` (it builds `X2Lib/Offline/X2OfflineJuneQuestMig.h`).

## Task

Review Q2's commits against the census and the user's four answers (Done note). What this phase could get wrong:
- **The tables.** `python june2017_re/quest_plan/q2_gen.py --check` must PASS (the header is what the generator writes). Re-derive by hand
  two `EPIC_NEXT` rows (one in the save: 12005 -> 12020, one not: 15020 -> 15030) from March's and June's after-lists, and judge the
  RESET definition (sub-quest list or any sub-quest's clear type / condition differs; 833 quests) against what the user chose ("reset all 42").
- **The rung** `CX2OfflineDB::MigrateQuestsToJune`: the order (drop in-progress, then accept the next epic only when the unit neither holds
  nor finished it, reset only quests that have sub rows, drop completed), one transaction, logs written only after COMMIT, the v21
  `SCHEMA_VERSION` branch and the `#error` on v20.
- **The MIG lines in the play-test log against `q2_expected_mig.txt`**, line for line (25 units; 16 / 5 / 42 / 19), and the save afterwards
  (`python tools/elsdb.py q "select * from unit_quest where quest_id in (18030,21920,14010,12020)"`).
- **The guards** in `X2UIQuestNew.cpp`: the two complete-ack handlers (1468 / 2017) and the four stored-id readers
  (`GetPartyUIQuestIcon`, `GetLocalMapQuestDesc`, `UpdateUIQuestSlotInfo`, `SetUIQuickQuestSlotInfo`). The survey that chose them is
  in the Done note; check that no other live unchecked reader can see a quest id with no templet.
- **Sub-quest 205680** (JUNE_BUGS 331): read the `SQT_NPC_TALK` case of `32323ad0` in Ghidra and settle whether June's own reader refuses a nil
  `m_eTalkNPCID`, as the port does. If it does not, the port differs from June: fix it under `JUNE_QUEST_LOAD`. Then update row 331 either way.
- **The flip.** `data036` ships June's three files byte-exact (`luasrc.py ledger --check`, the adopt record `june2017_re/june_adopt_20261019_074358.tsv`);
  the load census moved off March's 1,460 / 1,733.

Hygiene is one command: `python tools/flagcheck.py JUNE_QUEST_DATA --range <first Q2 commit>^..HEAD`. Paste its `files:` /
`in this flag's files:` / `RESULT:` lines into the review. Explain every OTHER row (Always.h only) and every deleted line.

Q2 has no Ghidra content (the migration is the user's design). Fix clear transcription errors under the same flag (then
`tools/build.py --deploy`); hand design decisions back to the user. Write "Q2 - Review" into the plan doc, update the Q2 row of
`STATUS.md`, check `kickoffs/quest-system/Q3.md` / `Q4.md` / `Q5a.md` / `Q7.md` (all wait on this review), and commit.
