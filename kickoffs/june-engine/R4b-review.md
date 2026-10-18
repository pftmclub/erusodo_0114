# June engine remainder - Phase R4b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine R4b review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase R4b done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_TARGET` + `SERV_IRUHADEV_JUNE_RENDER_FRAME` (R4b split them; no `_DEBUG`) |
| Log tag | `[JUNE_ENGINE] FRAME` |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase R4 -- Done (RE only, 2026-10-18, Opus 5.5)" "Phase R4b - Frame render, render targets, anti-aliasing option" "Phase R4b -- Done"
```

## Task

Review Phase R4b's commit(s) against the sweep rows it ported (`june2017_re/r4_frame_stage_sweep.tsv`) and June's code. Check the order of
every call inside a slot, each gate and pass argument, that the `#else` path is March's code, and that nothing a March caller reaches
changed behaviour unless June's does. Spot-check the new surface manager (`31d00b30` / `31d00d20` / `31d00ff0`) and the
render-target BeginRender / EndRender (`31cdb3c0` / `31cdb550`): every March render target now runs them.

Hygiene is one command per flag: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_RENDER_TARGET --range <first phase commit>^..<it>` and the same for
`SERV_IRUHADEV_JUNE_RENDER_FRAME` (memory: `..HEAD` times out; add `git log -S<FLAG> <c>..HEAD`). The new `KTDGDeviceSurfaceManager.cpp`'s
`#include "StdAfx.h"` FAIL is the R4a PCH-line precedent. Paste its `files:` / `RESULT:` lines into
the review; explain every OTHER row and deleted line; a FAIL is a finding.

Spot-check 2-3 functions in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same
flag (then `tools/build.py --deploy`); hand design decisions to the user. Write "Phase R4b -- Review" into the plan doc, update the
STATUS row, check the next phase's kickoff, and commit.
