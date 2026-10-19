# June engine remainder - Phase R4f - Software cursor (CKTDXCursor)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (map, then port), then a user play-test; type `/x2-phase june-engine R4f` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R4b (the frame's last call is this cursor); not alongside another session editing `KTDXApp.*` |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_CURSOR` (proposed; + `_DEBUG` if it logs per frame) |
| Log tag | `[JUNE_ENGINE] CURSOR` (`python tools/logs.py --tag JUNE_ENGINE`) |
| Review | `kickoffs/june-engine/R4f-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase R4b -- Done" "Phase R4f - Software cursor (CKTDXCursor)"
```

Then the funcmap row `31ce50e0 / 31ce5090 / 31ce5400 (R4b)`.

## Task

Do "Phase R4f". R4b named KTDXApp `+0x290`: an embedded `CKTDXCursor` (RTTI `.?AVCKTDXCursor@@`, vtable `3344c980`, ctor `31ce50e0`, dtor
`31ce5090`), drawn last in every frame by `31ce5400( 0 )` (`31be3800 @31be3c7c`). March has no such class.

- **Map** from Ghidra: every vtable slot of `3344c980`; who writes the cursor's fields (texture `+0x18`, position `+0x1c / +0x20`, offset
  `+0x24 / +0x28`, size `+0x2c / +0x30`, shown `+0x38`, `+0x3c`, hidden `+0x40`) and from where (app MsgProc / OnFrameMove, CX2Main, the
  states, option code); the draw `31ce5260`; reset / lost handling; whether June hides the Windows cursor while it draws its own.
- **Decide reach.** If June's own flow never shows it (hidden `+0x40` stays 1), port it whole and inactive, as R4a did the shimmer, with a
  `JUNE_BUGS.md` row only if it is a June mistake. If June shows it, port it live; how it looks in game is the gate.
- **Port** the class (new `KTDXCursor.h/.cpp`), the `+0x290` member, the frame-end call in `CKTDXApp::OnFrameRender` (after
  `JuneStageChainRenderAfter()`, R4b left the comment there) and every writer the map finds.
- **Gate:** build + deploy; the cursor looks and behaves as June's flow says (or as before when the cursor is inactive), plus a
  `[JUNE_ENGINE] CURSOR` line naming the state it is in.

**Inherited from june-gfx G6 (2026-10-19)** (read `JUNE_GFX_RUNTIME_PHASES.md` "Phase G6 - Done" and the `(GFx G6)` funcmap rows): June
does show this cursor. Its only writer is the GFx scene's Play (`3226b130 @3226b56b` -> `3226c250( 1, -40, -40 )` -> `31ce5520( 1 )`,
DLG_Eventscene_Cursor.dds), End hides it (`31ce50d0 @3226b748`), frame move `31ce51b0` (5 s idle hide). G6 left the class to you and
stood March's `CX2Cursor` in for it behind `SERV_IRUHADEV_JUNE_GFX_CURSOR_ORDER` (drawn from `CX2State::OnFrameRenderAfter` while a GFx
scene plays). Keep: the cursor after GFx and after the 0x18 pass; Play also switches the CX2Cursor off (`32868250`: +0xb0 = 0,
+0xb8 = 1, frame move and hardware cursor stop) and `32868790` turns +0xb8 off. When your port draws the real one, retire G6's flag
(comment its define out, remove the `X2GFXEventScene.cpp` calls' effect) in the same phase, so the scene cursor is never drawn twice.
