# June skill system core - Phase S8 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S8 review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S8 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_STATE_PTR` (+ `_DEBUG`) |
| Log tag | `[JUNE_SKILL] PTR` |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S0b - Slots, pages and UI RE" "Phase S8 - Running state data: June's pointer model"
```
Then the phase's Done note (`python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md --list` names it).

## Task

Review Phase S8's commit(s) against the S0a / S0b map it used: every Now / Future reader converted, no pointer held across a vector reallocation, the snapshot members and four consumers against June, the gate; the flag-off path is March's by-value copy.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first phase commit>^..HEAD --with SERV_IRUHADEV_JUNE_SKILL_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S8 -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file, and commit.
