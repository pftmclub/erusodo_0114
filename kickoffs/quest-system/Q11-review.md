# June quest system - Phase Q11 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q11 review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase Q11 done (its Done note and commit) and the user's 3-launch play-test |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_BATCH_ACK`, `SERV_IRUHADEV_JUNE_QUEST_FORBIDDEN_GUARD` |
| Log tag | `[JUNE_QUEST]` (sub-tags `BATCH`, `CRASH`) |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Starting context" "Phase Q11 - Chase the IsForbiddenQuest crash" "Q11 - Done (2026-10-28, Opus 5.5 high)"
```

## Task

Review Q11's commit against its evidence:
- Re-run `python tools/mdscan.py <bundle>\X2_offline.dmp X2/US_SERVICE/X2.map` only if the crash bundle (`%TEMP%\X2_404A.tmp`) and a map of exe md5 `21638825` are still at hand; otherwise accept the Done note's stack.
- `+0x8d8` = `CX2Unit::m_pUnitTemplet`: check `X2Unit.h` and the disasm at `IsForbiddenQuest` `@32cb5bf8`.
- The emulator change keeps every successful entry and its `AfterQuestComplete` work (`m_kQuestAfter` is cleared at the top of `CompleteOneQuest`, so `continue` skips nothing).
- The guard's return value (false = not forbidden) against all eight `IsForbiddenQuest` callers (Q11 kickoff lists them); `GiveUpForbiddenQuest` must not give up quests on a broken unit.
- Decide on the Done gate's "minidump on a forced test exception": the Done note offers the real crash's 117 KB dump in the zip instead. Accept it, or hand the question to the user.

Hygiene is one command per flag:
`python tools/flagcheck.py <FLAG> --range <Q11 commit>^..<Q11 commit>`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Read the play-test logs with `python tools/logs.py --tag JUNE_QUEST --grep "BATCH|CRASH"`: no crash bundle newer than the deploy, `BATCH` lines for the seeded quest 11000 (or a clean batch), no `CRASH guard` line expected.
Fix clear errors under the same flag (then `tools/build.py --deploy`); hand design decisions back
to the user. Write "Q11 - Review" into the plan doc, update the Q11 STATUS.md row, and commit.
