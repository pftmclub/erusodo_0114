# Offline monster grade - Phase 3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase offline-grade 3 review` |
| Plan doc | `OFFLINE_MONSTER_GRADE_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | Phase 3 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_OFFLINE_MONSTER_GRADE` |
| Log tag | `[OFFLINE_GRADE]` |

## Read first

```sh
python tools/docsection.py OFFLINE_MONSTER_GRADE_PHASES.md "Rules" "Established facts (verified 2026-10-06)" "Phase 3 - Rewards and remaining consumers" "Phase 3 - Done"
```

## Task

Review Phase 3's commit(s) against the server citations in "Established facts" and the June
addresses. Check `GetMonsterTypeFactor` / `GetBattleFieldMonsterTypeFactor` (attribute before grade), where the factor lands in EXP/ED (and the item lottery), the order against the offline boost, the ED piles, the 70057 / 70058 rule, and the MODS.md entry.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_OFFLINE_MONSTER_GRADE --range <first phase commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Open 2-3 of the cited server lines and check the port against them (for Phase 1, also June in
Ghidra, read-only). If one fails, re-check the whole phase.
- Fix clear transcription errors under the same flag, then run `tools/build.py --deploy`.
- Hand design decisions back to the user.

Finish:
- write "Phase 3 - Review" into the plan doc;
- update the phase's STATUS.md row;
- check the next phase's kickoff;
- commit.
