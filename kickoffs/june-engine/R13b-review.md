# June engine remainder - Phase R13b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine R13b review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R13b done (its Done note, commit, and the user's play-test log) |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_WORLD_SKIN_MESH`, `SERV_IRUHADEV_JUNE_RENDER_WORLD_SKIN_MESH_RAYHIT` (+ `_WORLD_SKIN_MESH_DEBUG`) |
| Log tag | `[JUNE_ENGINE] WSKIN` (`python tools/logs.py --grep "JUNE_ENGINE] WSKIN"`) |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Phase R13 -- Done" "Phase R13b - World skin-mesh render (Prepare)" "Phase R13b -- Done"
```

Then the funcmap rows tagged `(R13b)` (6 rows, the end of `june2017_re/funcmap.tsv`).

## Task

Review R13b's commit against June's code (`june2017_re/funcdis.py --func <va>`; Ghidra has carved none of Prepare `327f6e40`, RayHit `327f6a70`,
OnFrameMove `327f83a0`). Things this phase could get wrong:

- **Prepare `327f6e40`:** the hide gate is `+0x2f0 && +0x2f1` only, and `+0x2f1` is `m_bAnyCamera` (thunk `31e4c6a0`, bound by the skin-mesh block of binder
  `31d95080`, name `@31dbea96`); the camera write sits only in the `canHide && hide` branch and checks the DG manager for NULL; the x87 senses of the
  `0.3f < alpha`, cap and `alpha >= cap` branches; the claim that June's double `4.0` fade gives the same float as March's `4.0f`.
- **RayHit `327f6a70`:** the clamp senses (`proj <= 0` -> start, `|d|^2 <= proj` -> end), the strict `dist < r + 1`, the impact point
  (`centre + normalize(closest - centre) * dist * 0.5`) and the `-1` miss. Re-derive the x87 stack once yourself: `fsubp` / `fsubrp` operand order decides
  `end - start` vs `start - end`. Check that no other June caller of slot 0x40 on a skin mesh reads the hit distance, or say which does.
- **OnFrameMove `327f83a0` "= March":** spot-check the jump table `327f8cf8` (4 entries) and the `Play` values against March's `XAP_*`; the IsHost
  culling test.
- **Not ported:** the `AddModelXSkinMesh` `__FILEW__` / `151` arguments (R12 added the parameters; no March caller passes them). Agree or route.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <R13b commit>^..<R13b commit>`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain
every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra / capstone (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flags (then
`tools/build.py --deploy`); hand design decisions back to the user. If the play-test passed, retire `_WORLD_SKIN_MESH_DEBUG` (or say why it stays). Write
"Phase R13b -- Review" into the plan doc, update the phase's STATUS.md row, and commit.
