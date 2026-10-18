# June engine remainder - Phase R12b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine R12b review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase R12b done (its Done note and commit) and the user's R12b play-test |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_MOTION_ORDER`, `SERV_IRUHADEV_JUNE_RENDER_MOTION_CASH_EMOTION`, `SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION` (back on); `SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG` on for the play-test |
| Log tag | `[JUNE_ENGINE] SKREF` (`python tools/logs.py --grep "JUNE_ENGINE] SKREF"`) |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Starting context" "Phase R12b - Motion collision source (trace)" "Phase R12b -- Done (trace 2 + port)"
```

Then the funcmap rows tagged `(R12b)` (the trace-1 rows and the 8 trace-2 rows at the end of the file).

## Task

Review R12b's commit against June's InitMotion `31fe5860` and the templet parser `3237aa40`:
- the main motion: both `m_pMotionFuture` and `m_pMotion` from templet `+0x178` (`@31fe5a48-31fe5b4c`), and `+0x178` = `m_GameMotion0`
  (parser store `@3237c865` with ESI = node pair, templet = ESI + 4; the empty test `@31fe5d1f`);
- the mix order: `MOTION_FILE_NAME` first (`@31fe5c78`), then GameMotion2 / GameMotion1 by grade, Emotion, then `31ee9150( type, 1, vec )`
  (`@31fe5cfa`), then the `m_GameMotion0 == L""` clear; the `#else` paths are March's;
- `31ee9150`'s 12 cases and the exact file names (strings `3345c238`..`3345bf7c`), type 10 Lu on `true`;
- no skin part adds a second copy of the unit's spheres with MOTION_COLLISION on (X2Eqip / AfterImage `false, false`).

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_RENDER_MOTION_ORDER --range <R12b commit>^..<R12b commit> --with SERV_IRUHADEV_JUNE_RENDER_MOTION_CASH_EMOTION SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Always.h's FAIL on comment lines is the multi-flag define
shape; explain it, and every deleted line.

Check the play-test log: one `SKREF motion collision MOTION_<CLASS>.X collision N attack M` per user unit, N and M > 0 for every class played
(Raven 3 / 4, others 3 / 2). Then turn `SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG` off again (rebuild, deploy).

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the
same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "Phase R12b -- Review" into the plan doc,
update the R12b STATUS.md row, and commit. Next after the review: R14 (STATUS).
