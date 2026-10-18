# June skill system core - Phase S7b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7b review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S7b done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_TREE_UI` |
| Log tag | `[JUNE_SKILL] UI` |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S0b - Slots, pages and UI RE" "Phase S7 - Skill tree UI and learn logic"
```
Then the phase's Done note (`python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md --list` names it).

## Task

Review Phase S7b's commit(s) against `june2017_re/s0b_ui_funcmap.tsv`: each tree-UI function against June, the memo packets on client
and emulator, page packets handled or ignored with a reason, `SKILLTREE_NO_LOCK` retired.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first phase commit>^..HEAD --with SERV_IRUHADEV_JUNE_SKILL_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S7b -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file, and commit.

**`SERV_IRUHADEV_JUNE_SKILL_DEBUG` (S3d review, 2026-10-18; moved here from S7-review when S7 split):** once S7b's play-test has
shown the `[JUNE_SKILL] MEMO` lines (S3d) and S3e is done, retire the flag: remove its define and every
`#if defined( SERV_IRUHADEV_JUNE_SKILL_DEBUG ) ...` block (`git grep`), or say why it stays. Its Always.h comment ("remove after the S2
review") is out of date. In the same Always.h edit, add the missing guard
`#error SERV_IRUHADEV_JUNE_SKILL_MEMO_DAMAGE needs SERV_IRUHADEV_JUNE_EFFECTSET_BUFF_LIST` (its helpers sit in `X2BuffFactor.h:959`).
