# June engine remainder - Phase R4g - Render-target call sites take June's arguments

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (map, then port), then a user play-test; type `/x2-phase june-engine R4g` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R4b; not alongside R4d (`X2Game.*`) or R4e (`KTDGUIControl.cpp`) or another session editing the files it touches |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_TARGET_CALLERS` (proposed) |
| Log tag | `[JUNE_ENGINE] RTSITE` (`python tools/logs.py --tag JUNE_ENGINE`) |
| Review | `kickoffs/june-engine/R4g-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase R4b -- Done" "Phase R4g - Render-target call sites take June's arguments"
```

Then the funcmap rows tagged `(R4b)`, chiefly `31cdf310 (R4b)` and `31cdb3c0 / 31cdb550 / 31cdb5c0 (R4b)`.

## Task

Do "Phase R4g". R4b ported June's render target (`SERV_IRUHADEV_JUNE_RENDER_TARGET`) and kept every March call site on March's
defaults: own depth surface, no multisample, `BeginRender( bClear )` = `( bClear, bClear, keep depth 0, colour 0 )`. Each June site passes
its own arguments.

- **Opens** (`31cdf310` callers): env box LUTs `31d0b200` (256 x 16, fmt 0x20, use depth 0), LastKillShot `32306600` (use depth 0), minimap
  `32e583d0` (fmt 0x15, use depth 1), MoveData `31c96daa`, `31c40220`. Map each to its March site and port the arguments.
- **Begins / ends** (`31cdb3c0` / `31cdb550` callers `31c3f790`, `31c94d90`, `31d0a410`, `3289fdd0`, `32e573b0`, `32e56f40`, `32e592d0`):
  each March `BeginRender( bool )` + `Clear()` pair becomes June's four-argument call where June's matching function does it.
  Watch the depth argument: with use-depth 0 and keep-depth 0 a depth clear hits the bound (scene) depth surface.
- **LastKillShot under anti-aliasing:** March's `CopyBackBufferToSurface` copies the back buffer, but with `ANTIALIASING_LEVEL > 0` the
  frame is still in the scene target. Read how June takes the shot and port it (with R4d if it sits in the game render).
- March-only sites June dropped (glow, motion blur, post effect, avatar shot) keep March's defaults; list them.
- **Gate:** build + deploy; the minimap, the dungeon-clear last-kill shot (also at `ANTIALIASING_LEVEL = 2`), the colour grading and the UI
  move data look as before.
