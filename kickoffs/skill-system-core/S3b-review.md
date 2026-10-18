# June skill system core - Phase S3b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S3b review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S3b done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_NOTE` |
| Log tag | `[JUNE_SKILL] NOTE` |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S3b - June skill notes (memos)"
```
Then the funcmap rows tagged `Skill S3b` (`grep "(S3b)" june2017_re/funcmap.tsv`).

## Task

Review Phase S3b's commit(s) against June's binary:
- **AddNewSkillNoteTemplet `32454840`**: the tier test (10-99 / 100-199), the inheritance order (first job's rows, then the base's, both
  inserted at the front), the row defaults (memo 0, skill 0, level -1, five values -1.0 from `334380f0`), the memo-0 drop, unique insert.
- **The class tables** (`31eca1d0`, `32368880`, `31eca340`, the `337a1660` table from the initializer `@3234afb5-3234b389`): re-read a few
  stores and cases.
- **The readers**: which ones map the skill through `32207210` and which do not (`32890f90`, `328923e0`, `32891050` and `32451ff0` take the
  id as given).
- **`31fdbbb0` / `31fdbcc0`** in `X2GUUser.cpp`: the filter, the active test, the type 19 / 20 value index (`31fca8e0( id, 1 )` on the id as given).
- **The REFORM_SKILL_NOTE_UI finding**: March's class, binding and loader call are compiled out in the US build (`Always_Global.h:39`);
  confirm with `cl /P` that June's class is the only `CX2SkillNoteManager` compiled and that the load line comes from it.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py JUNE_SKILL_NOTE --range <first phase commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S3b -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file (`S3d.md`), and commit.
