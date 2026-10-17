# June engine remainder - Phase R13 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine R13 review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R13 done (its Done note, commit, and the user's play-test log) |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_WORLD_MESH`, `SERV_IRUHADEV_JUNE_RENDER_CAMERA_MESH_HIDDEN` (+ `_WORLD_MESH_DEBUG`) |
| Log tag | `[JUNE_ENGINE] WMESH` (`python tools/logs.py --grep "JUNE_ENGINE] WMESH"`) |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Phase R8 -- Done" "Phase R13 - World object mesh render" "Phase R13 -- Done"
```

Then the funcmap rows tagged `(R13)` (9 rows, the end of `june2017_re/funcmap.tsv`).

## Task

Review R13's commit against June's code (`june2017_re/funcdis.py --func <va>` for what Ghidra has not carved: Prepare `327f9020`, slot 0x44 `327f95a0`).
Things this phase could get wrong:

- **Prepare `327f9020`:** the x87 branch senses (hide / near distance, `0.3f < alpha`, the cap `limit < alpha`, `alpha >= limit` -> `bAlphaBlend`), the hide test gated
  on `+0x2f0 && +0x2f2` only (and that `+0x2f2` is `m_bAnyCamera`, `+0x2f1` `m_bNotRenderWhenHidden`: confirm with the `SetCanHide` binder, xorstrings
  `31dbcf6c`), the saved-layer save only when `+0x324 == -1`, the restore only on the not-hidden path, `fDist` starting at 0, and the camera write only in the
  `canHide && hide` branch.
- **The dead slot-0x48 claim** (JUNE_BUGS 267): re-run a caller search your own way (any `call [reg+0x48]` / `mov reg,[reg+0x48]` whose object can be a world
  mesh). If June does call it somewhere, subset culling is live and the port is wrong.
- **`UpdateCamera` = `31d02e50`** (not `31d02c70`): the reset is its first store, before the tracking.
- **Layout:** `KTDGCamera.h` is only reached after `Always.h` (through `KTDX.h` -> `KTDGManager.h`), so `CKTDGCamera`'s size is the same in every TU.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <R13 commit>^..<R13 commit>`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra / capstone (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flags (then
`tools/build.py --deploy`); hand design decisions back to the user. If the play-test passed, retire `_WORLD_MESH_DEBUG` (or say why it stays). Write
"Phase R13 -- Review" into the plan doc, update the phase's STATUS.md row, check R13b's kickoff against R13's final state, and commit.
