# <Plan name> - Phase <N> review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase <plan-key> <N> review` |
| Plan doc | `<PLAN>_PHASES.md` (branch `<branch>`) |
| Depends on | Phase <N> done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_<NAME>` |
| Log tag | `[<TAG>]` |

## Read first

```sh
python tools/docsection.py <PLAN>_PHASES.md "Starting context" "<the phase's spec section>" "<RE/map section it used>" "Phase <N> -- Done"
```

## Task

Review Phase <N>'s commit(s) against <the map / RE section>: <the specific things this phase
could get wrong: enum values, float/key order, store members, read sites, the #else path>.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first phase commit>^..HEAD --with <FLAG>_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase <N> -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file, and commit.
