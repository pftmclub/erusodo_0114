# Offline monster grade - Phase 3b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase offline-grade 3b review` |
| Plan doc | `OFFLINE_MONSTER_GRADE_PHASES.md` (branch `main`) |
| Depends on | Phase 3b done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_OFFLINE_MONSTER_GRADE` |
| Log tag | `[OFFLINE_GRADE]` |

## Read first

```sh
python tools/docsection.py OFFLINE_MONSTER_GRADE_PHASES.md "Rules" "Phase 3b - Field middle-boss bonus items" "Phase 3b - Done" "Phase 3 - Review"
```

## Task

Review Phase 3b's commit against the server:
- `BattleFieldMiddleBossDropProcess` (`KncWX2Server/CenterServer/BattleFieldRoom.cpp:4378-4790`):
  - the contribution formula and its 1.5 / 0.08 limits;
  - the drop-row early return (`:4486`);
  - the bonus gate (`:4703`);
  - the item order and counts.
- `CXSLBattleFieldManager::SetBattleFieldMiddleBossDropInfo_LUA` / `GetBattleFieldMiddleBossDropInfo` (`XSLBattleFieldManager.cpp:425-475`): required keys, first row wins, no wildcard.
- `CalculatePartyRank` (`BattleFieldUserManager.cpp:1657`): the solo-Low claim.
- `data036_kom`: `FieldBonusDrop.src.lua` is byte-identical to the server file, and the manifest entry is CRLF, 2-space indent and algo `"2"`.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_OFFLINE_MONSTER_GRADE --range <3b commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Fix clear transcription errors under the same flag, then run `tools/build.py --deploy`. Hand
design decisions back to the user.

Finish:
- write "Phase 3b - Review" into the plan doc;
- update the STATUS.md row;
- commit.
