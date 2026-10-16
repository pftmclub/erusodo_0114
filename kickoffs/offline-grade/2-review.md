# Offline monster grade - Phase 2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase offline-grade 2 review` |
| Plan doc | `OFFLINE_MONSTER_GRADE_PHASES.md` (branch `main`) |
| Depends on | Phase 2 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_OFFLINE_MONSTER_GRADE` |
| Log tag | `[OFFLINE_GRADE]` |

## Read first

```sh
python tools/docsection.py OFFLINE_MONSTER_GRADE_PHASES.md "Rules" "Established facts (verified 2026-10-06)" "Phase 2 - Battlefield like the CenterServer" "Phase 2 - Done"
```

## Task

Review Phase 2's commit(s) against the server citations in "Established facts" and the June
addresses. Check the `SPAWN_GROUP_ID` / `SPAWN_ID > 100` loader against `XSLBattleField.cpp:187-247`, the grade per spawn path, the per-UID grade store (client-sent grades kept), the middle-boss test by grade, and the danger factor table.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_OFFLINE_MONSTER_GRADE --range <first phase commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Open 2-3 of the cited server lines and check the port against them (for Phase 1, also June in
Ghidra, read-only). If one fails, re-check the whole phase.
- Fix clear transcription errors under the same flag, then run `tools/build.py --deploy`.
- Hand design decisions back to the user.

Finish:
- write "Phase 2 - Review" into the plan doc;
- update the phase's STATUS.md row;
- check the next phase's kickoff;
- commit.
