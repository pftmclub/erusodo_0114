# June skill system core - Phase S4 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S4 review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S4 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_NUMERICAL` (+ `_DEBUG`) |
| Log tag | `[JUNE_SKILL] NUM` |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S0a - Templet and tree RE" "Phase S4 - Numerical skill data override"
```
Then the phase's Done note (`python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md --list` names it).

## Task

Review Phase S4's commit(s) against the S0a / S0b map it used: the condition (index > 0 or name, and APPLIED / DAMAGE skill id), the row pick by learned level and the clamp, x0.01, the PVP_RATE / TECH_POINT / HIT_ADD_MP defaults, the four JUNE_BUGS rows (282-285) and the MODS.md values; re-run the self-check lines against `c1a_numerical_blocks.tsv`.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first phase commit>^..HEAD --with SERV_IRUHADEV_JUNE_SKILL_NUMERICAL_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S4 -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file, and commit.
