# June engine remainder - Phase R4 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine R4 review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase R4 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_HEAT_SHIMMER` |
| Log tag | `[JUNE_ENGINE] SHIMMER` |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase R0 - Renderer contract RE" "Phase R4 - Heat shimmer" "Open questions (planning, 2026-10-16)" "Phase R4 -- Done"
```

## Task

Review Phase R4's commit(s) against the RE map it follows (R0, U0 or W0) and the June addresses it cites: enum values and table
order, every setter's source value, key names and defaults, the store members, every read site, and that the `#else` path is
March's code.

Hygiene is one command: `python tools/flagcheck.py <the phase's flag> --range <first phase commit>^..HEAD`. Paste its `files:` /
`RESULT:` lines into the review; explain every OTHER row and deleted line; a FAIL is a finding.

Spot-check 2-3 items in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same
flag (then `tools/build.py --deploy`); hand design decisions to the user. Write "Phase R4 -- Review" into the plan doc, update the
STATUS row, check the next phase's kickoff, and commit.
