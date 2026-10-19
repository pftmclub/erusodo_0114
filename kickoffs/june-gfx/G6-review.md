# June GFx runtime - Phase G6 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-gfx G6 review` |
| Plan doc | `JUNE_GFX_RUNTIME_PHASES.md` (branch `main`) |
| Depends on | Phase G6 done (its Done note and commit) and the user's G6 play-test |
| Flag | `SERV_IRUHADEV_JUNE_GFX_CURSOR_ORDER` |
| Log tag | `[JUNE_GFX] SCENE` (`python tools/logs.py --grep "JUNE_GFX SCENE"`) |

## Read first

```sh
python tools/docsection.py JUNE_GFX_RUNTIME_PHASES.md "Rules" "Phase G6 - Cursor and the GFx layer" "Phase G6 - Done (2026-10-19, Opus 5.5 high; flag `SERV_IRUHADEV_JUNE_GFX_CURSOR_ORDER`)"
grep -a "(GFx G6)" june2017_re/funcmap.tsv
```

## Task

Review Phase G6's commit against the map in its Done note. The things it could get wrong:
- the June frame tail order (`31be3800` @31be3c56..31be3c7c: GFx, then the 0x18 pass, then `CKTDXCursor 31ce5400( 0 )`), and that
  `31ce5520` has no caller but `3226c250` (so June's after-GFx cursor is the GFx scene's only);
- that June's scene cursor matches March's `XCS_EVENT_SCENE` draw (DLG_Eventscene_Cursor.dds, 128 x 128, offset -40, 5 s idle hide `31ce51b0`);
- March's side: every state's cursor draw (`CX2State::OnFrameRenderFrontUI` and the unsplit states' `OnFrameRender`) is skipped while the
  scene plays, `CX2State::OnFrameRenderAfter` runs for every state (no override), and End always clears the mode (forced skip, state change).

Hygiene is one command:
`python tools/flagcheck.py JUNE_GFX_CURSOR_ORDER --range <G6 commit>^..<G6 commit>`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. The Always.h OTHER row, if any, is a peer's block; say whose.

Spot-check 2-3 items in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same
flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "Phase G6 - Review" into the plan doc, update
the G6 STATUS.md row, check R4f's kickoff names what it inherits from G6, and commit.
