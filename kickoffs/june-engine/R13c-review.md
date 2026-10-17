# June engine remainder - Phase R13c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine R13c review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R13c done (its Done note, commit, and the user's play-test log) |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK` (+ `_DEBUG`) |
| Log tag | `[JUNE_ENGINE] CAMTRACK` |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Phase R13c - Camera tracking and roll (UpdateCamera whole)" "Phase R13c -- Done"
```

Then `june2017_re/r13c_settracking_callers.tsv` and the funcmap rows tagged `(R13c)`.

## Task

Review R13c's commit(s) against June's camera code. Things this phase could get wrong:

- **`SetTracking` `31d01b70`:** argument order of the 10 (`ret 0x28`), the `0.001` threshold sense, `m_fTrackTime = 0` on the failing branch, and which
  member `g_pKTDXApp +0x2d4` is.
- **`UpdateCamera` `31d02e50`:** the x87 senses of the `+0x90` / `+0x94` path, the roll step `+0x84 * dt`, and the rotation's operand order after
  `LookAtLH` (which D3DX function thunk `33086abc` binds, and whether the roll matrix is multiplied left or right).
- **Callers:** each of June's 11 sites maps to the March site the table says, and the March-only sites pass neutral values.
- **Layout:** new `CKTDGCamera` members are reached only after `Always.h` (R13 review: `KTDGCamera.h` is included only via `KTDGManager.h`).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK --range <first R13c commit>^..<last R13c commit> --with SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and deleted line.

Spot-check 2-3 items in capstone / Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then
`tools/build.py --deploy`); hand design decisions back to the user. If the play-test passed, retire `_DEBUG`. Write "Phase R13c -- Review" into the plan doc,
update the STATUS row, and commit.
