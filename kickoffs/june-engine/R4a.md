# June engine remainder - Phase R4a - Pass-1 layer and heat-shimmer object

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (transcribe from Ghidra), then a user play-test; type `/x2-phase june-engine R4a` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R4 (RE done); not alongside another session editing `KTDGManager.*` or the dialog manager |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_HEAT_SHIMMER` (object) + `SERV_IRUHADEV_JUNE_RENDER_PASS1` (pass argument, post-effect lists) |
| Log tag | `[JUNE_ENGINE] SHIMMER` (`python tools/logs.py --tag JUNE_ENGINE`) |
| Review | `kickoffs/june-engine/R4a-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase R4 -- Done (RE only, 2026-10-18, Opus 5.5)" "Phase R4a - Pass-1 layer and heat-shimmer object"
```

Then the sweep rows named below (`june2017_re/r4_frame_stage_sweep.tsv`) and the funcmap rows tagged `(R4)`.

## Task

Do "Phase R4a". The rows to port are `june2017_re/r4_frame_stage_sweep.tsv` areas `shimmer` and `pass1`. Also port the funcmap rows tagged
`Engine R0b` / `(R8)` that they cite: `31ce80c0`, `31ce9470`, `31ce94a0`, `31cda1a0`, `31d053f0`, `31d05700`, `31cd7810`, `31ccfee0`,
`31c82d30`, `31ceefa0`. Transcribe each from Ghidra and port the whole function (rule 1).

- **Heat shimmer** at DG `+0x384`: ctor `31ce5630`, OnReset `31ce5e90`, OnLost `31ce5680`, begin `31ce5710`, end `31ce5d00`, commit
  `31ce56a0`. The DG hooks are ctor `31cead50`, dtor `@31ceb37c`, reset `31ce98c0` and lost `31ce97e0`. If `XHeatShimmerRenderer.fx` is
  missing (it ships with asset-port P4a), log one `[JUNE_ENGINE] SHIMMER` line and stay inactive, as R3 does.
- **Pass 1** has no caller that passes 1 until R4b. Every March caller passes 0, so nothing in game may change.
- **Gate:**
  - build + deploy;
  - one `[JUNE_ENGINE] SHIMMER` line at the first device reset, saying whether the effect loaded or is missing;
  - the dialog band and over-UI behaviour from R8's play-test unchanged.
