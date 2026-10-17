# June engine remainder - Phase R4c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine R4c review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase R4c done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_STAGE_SPLIT` |
| Log tag | `[JUNE_ENGINE] SPLIT` |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase R4 -- Done (RE only, 2026-10-18, Opus 5.5)" "Phase R4c - State render split" "Phase R4c -- Done"
```

## Task

Review Phase R4c's commit(s) against the sweep rows it ported (`june2017_re/r4_frame_stage_sweep.tsv`) and June's code. Check the order of
every call inside a slot, each gate and pass argument, that the `#else` path is March's code, and that nothing a March caller reaches
changed behaviour unless June's does.

Hygiene is one command: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_RENDER_STAGE_SPLIT --range <first phase commit>^..HEAD`. Paste its `files:` / `RESULT:` lines into
the review; explain every OTHER row and deleted line; a FAIL is a finding.

Spot-check 2-3 functions in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same
flag (then `tools/build.py --deploy`); hand design decisions to the user. Write "Phase R4c -- Review" into the plan doc, update the
STATUS row, check the next phase's kickoff, and commit.
