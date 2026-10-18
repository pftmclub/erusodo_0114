# June engine remainder - Phase R4i - Post-effect object overrides and the talk-box pass

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (map, then port), then a user play-test; type `/x2-phase june-engine R4i` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R4e (`DRAW_PASS`: the texture / font pass; `UI_CONTROL_PASS`: `CX2TalkBoxManagerUI::OnFrameRender( pass )`) |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_POST_EFFECT_OBJECTS` (proposed; the phase may split it and says so in its Done note) |
| Log tag | `[JUNE_ENGINE] POSTOBJ` (`python tools/logs.py --tag JUNE_ENGINE`) |
| Review | `kickoffs/june-engine/R4i-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase R4a -- Done" "Phase R4e -- Done" "Phase R4i - Post-effect object overrides and the talk-box pass"
```

Then the funcmap rows tagged `(R4a)` and `(R4e)` (`grep -a "(R4[ae])" june2017_re/funcmap.tsv`).

## Task

Do "Phase R4i". R4a gave `CKTDGObject` the post-effect slots (`OnFrameRender_PostEffectDraw` 0x10, `IsPostEffect` 0x2c) and the mesh /
particle overrides. R4e's RTTI sweep found three more classes that override them:

- `CX2UnitViewerUI`: `31f97960` / `31fae370`;
- `CX2Equip`: `3295caa0` / `3295bd50`;
- `CX2TalkBoxManager`: `32e903e0` / `31fae370`.

It also found the talk-box imp's `OnFrameRender( pass )`: imp vslot `0xc`, base `32e930c0`, party leader `32f21da0`.

- Map each override against March's draw (`CX2UnitViewerUI::OnFrameRender_Draw` X2UnitViewerUI.cpp:1438, `CX2Equip`'s draw, the talk-box
  manager's draw). Then port each whole (rule 1): the post-effect draws, `IsPostEffect`, and the imp's pass down to its texture and font
  draws, which take it since R4e.
- Make `CX2TalkBoxManagerUI` (June `32eec4a0`) hand the pass to the imp.
- Every caller of pass 1 is unreached, so nothing in game may change. Gate: build + deploy; the character-info / cash-shop 3D models,
  equipped-item effects and talk boxes look as before.
