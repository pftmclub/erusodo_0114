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
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S0b - Slots, pages and UI RE" "Phase S7 - Skill tree UI and learn logic" "Phase S7b - Done (2026-10-18, Opus 5.5 high)"
```
Then the phase's Done note (`python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md --list` names it).

## Task

Review Phase S7b's commit(s) against `june2017_re/s0b_ui_funcmap.tsv`: each ported tree-UI function against June ( the Done note lists
them; memo, pages and the panels are S7c / S7d / S7e ), the `/// S7c:` / `/// S7d:` / `/// S7e:` / checklist markers each owned, the carried S6 equip-bar code
unchanged, `SKILLTREE_NO_LOCK` retired, the data in data034 / data036 / data057.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first phase commit>^..HEAD --with SERV_IRUHADEV_JUNE_SKILL_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S7b -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file, and commit.

(The `SERV_IRUHADEV_JUNE_SKILL_DEBUG` retirement moved to `S7c-review.md`: it waits for the `[JUNE_SKILL] MEMO` lines, which S7c's play-test shows.)
