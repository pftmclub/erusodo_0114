# June skill system core - Phase S7e review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7e review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S7e done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS` |
| Log tag | `[JUNE_SKILL] UI` |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7b - Done (2026-10-18, Opus 5.5 high)"
```
Then the phase's Done note (`python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md --list` names it).

## Task

Review Phase S7e (major-skill and command panels) against `june2017_re/s0b_ui_funcmap.tsv`: the major-skill panel against `328e42d0` / `3220bdb0`, the command tab against `328e7ba0` / `328e4bd0` / `328de9a0` / `328e5290` / `328e8820` / `328e6cd0`, cases 29061 / 29062 / 29069-29074.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS --range <first phase commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S7e -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file, and commit.

**`SERV_IRUHADEV_JUNE_SKILL_DEBUG` (carried from the S7d review, 2026-10-19):** retire it only once a play-test has shown the S3d
`[JUNE_SKILL] MEMO effectset` / `damageeffect` / `attack box` lines (a class whose memo skill plays). Then remove its define and every
`#if defined( SERV_IRUHADEV_JUNE_SKILL_DEBUG ) ...` block (`git grep`). If they still have not run, say so and carry this note to the next review.
