# June engine remainder - Phase U1 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine U1 review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase U1 done (its Done note and commits) |
| Flags | `SERV_IRUHADEV_JUNE_UI_FRAMEWORK`, `_JUNE_UI_NATION_KEYS`, `_JUNE_UI_MOVIE_SUBTITLE`, `_JUNE_UI_LINE_BREAK` (U0 split; spec per item in `U1.md`) |
| Log tag | `[JUNE_ENGINE] UI` |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Investigation ledger (measured 2026-10-16)" "Phase U0 - UI framework RE" "Phase U1 - UI framework port" "Phase U1 -- Done"
```

## Task

Review Phase U1's commit(s) against the RE map it follows (R0, U0 or W0) and the June addresses it cites: enum values and table
order, every setter's source value, key names and defaults, the store members, every read site, and that the `#else` path is
March's code.

Hygiene is one command: `python tools/flagcheck.py <the phase's flag> --range <first phase commit>^..HEAD`. Paste its `files:` /
`RESULT:` lines into the review; explain every OTHER row and deleted line; a FAIL is a finding.

Spot-check 2-3 items in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same
flag (then `tools/build.py --deploy`); hand design decisions to the user. Write "Phase U1 -- Review" into the plan doc, update the
STATUS row, check the next phase's kickoff, and commit.
