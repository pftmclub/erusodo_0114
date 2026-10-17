# June engine remainder - Phase R4b - Frame render, render targets, anti-aliasing option

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (transcribe from Ghidra), then a user play-test; type `/x2-phase june-engine R4b` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R4a; not alongside another session editing `KTDXApp.cpp`, `KTDXDeviceRenderTargetTexture.*` or `X2GameOption.*` |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_FRAME` (+ `_DEBUG` if the phase logs per frame) |
| Log tag | `[JUNE_ENGINE] FRAME` (`python tools/logs.py --tag JUNE_ENGINE`) |
| Review | `kickoffs/june-engine/R4b-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase R4 -- Done (RE only, 2026-10-18, Opus 5.5)" "Phase R4b - Frame render, render targets, anti-aliasing option"
```

Then the sweep rows named below (`june2017_re/r4_frame_stage_sweep.tsv`) and the funcmap rows tagged `(R4)`.

## From the R4a review (2026-10-18)

- Pass 1's entry points are in: `CKTDGManager::FrontUIRender( 1 )`, `ObjectChainAlphaRenderPass1()`, `GetJuneHeatShimmer()->BeginEffect( 0, pWorld )` / `EndEffect()`.
- JUNE_BUGS 274: the pass-1 hint loop (`31ce80c0`) does not advance its index past a skipped entry, so a pass-1 batch reads the wrong range. This is
  harmless only while pass 1 stays dead (option `+0x34` 0, setter `3241cd20` caller-less). Keep it dead, as June does.

## Task

Do "Phase R4b". The rows to port are `june2017_re/r4_frame_stage_sweep.tsv` areas `frame` and `option`, and the funcmap rows tagged
`(R4)`.

- **Frame.** Port `31be3800` whole, plus the four stage loops and the `CKTDXStage` slots `0xc` / `0x10` / `0x14` / `0x18` with June's base
  bodies. A March class that still overrides only `OnFrameRender` keeps drawing everything in `0x8`, which is March's order until R4c.
- **Render targets.** Port the lifecycle (`31be4230`, `31be2ef0`, `31be4cf0`, `31be4d10`), `31be4510` and `31be34c0`.
  - The render-target class gets its multisample surface and the 4-argument `BeginRender` (`31cdb3c0` / `31cdb550` / `31cdb5c0` /
    `31cdf310`). Keep March's existing callers working: R3's env box uses `BeginRender( false )`.
  - Port the DG `BeginRender` split (`31ce63f0` / `31ce6600`).
- **Option.** `ANTIALIASING_LEVEL` load / save / apply into the R2 option block. Option `+0x34` / `+0x35` default 0, the low-spec zero, and
  the caller-less setter `3241cd20`, verbatim.
- **JUNE_BUGS row:** the RT-pair setter has no caller, so pass 1 and the shimmer never run.
- **Name** KTDXApp `+0x290` (`31ce5400( 0 )` at frame end) against March's `CKTDXApp` members, and port it or route it.
- **Gate:**
  - the game looks unchanged at the default `ANTIALIASING_LEVEL = 0`;
  - with `ANTIALIASING_LEVEL = 2` in the game dir's `GameOptions.lua`, edges are smoothed and a `[JUNE_ENGINE] FRAME` line names the level
    and the targets made;
  - `ANTIALIASING_LEVEL` is written back on exit.
