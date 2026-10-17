# June engine remainder - Phase R8 review - DG manager render lists

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine R8 review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase R8 done (its Done note and commit) and the user's R8 play-test |
| Flags | `SERV_IRUHADEV_JUNE_RENDER_DG_STATES`, `_JUNE_UI_DIALOG_BANDS`, `_JUNE_RENDER_DG_LISTS`, `_JUNE_RENDER_FRUSTUM_TEST`, `_JUNE_RENDER_ALPHA_SORT_KEY`, `_JUNE_RENDER_HINT_NO_GROUP`, `_JUNE_RENDER_STATE_RESET` |
| Log tag | `[JUNE_ENGINE] DG` (`python tools/logs.py --grep "JUNE_ENGINE] DG"`) |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase R8 - DG manager render lists" "Phase R8 -- Done"
```

Then the funcmap rows tagged `Engine R8` (`grep "(R8)" june2017_re/funcmap.tsv`) and JUNE_BUGS rows 251-252.

## Task

Review R8's commit against June:

1. **State tables.** Re-decode `33728270`, `337284e0` and `33728620` from `unpacked_x2.exe` (16-byte KState records, data VA = file offset
   + 0xc00 + 0x31aa0000) and compare with `akRSICKTDXDeviceTextureAdd`, `akRSICKTDGUIDialogJune` and `akRSICKTDGUIDialogAdd`, entry by entry.
2. **Dialog manager.** Compare `_OnFrameRenderJune` with `31d053f0` / `31d05700`:
   - the hide path;
   - the flagged-only branch;
   - the band test (dialog +0x18c);
   - the guide-control push;
   - the drag slot.
   Then decide whether keeping March's four hide names for mode 1 is right, or June's mode-1 preset (filled in `32597410`) differs.
3. **Chain sort and FrontUIRender:** the 500 split (`<` vs `<=`), the not-cull order, the call order in `31ce94a0`, and the comparator tie-break direction.
4. **Frustum:** June `31c829e0` against the new base `IsInFrustum`: the radius, the additive float, the plane index, and the culled flag.
5. **Hint loop:** each batch draws exactly once, and the pre-batch pop and post-batch reset are kept.
6. **Layers above 500 (JUNE_BUGS 253):** confirm in June that nothing else draws front dialogs of layer 501+ in pass 0 (CX2State `31ef76bf` draws only in flagged mode), and whether June ever plays a Lua event scene with object layers.
7. **State reset:** the DXUT adapter and device-type arguments, and the call positions in `31be6510` / `31be66b0`.

Hygiene, for each of the seven flags:
`python tools/flagcheck.py <FLAG> --range <R8 commit>^..<R8 commit>`; then `git log -S<FLAG> <R8 commit>..HEAD` for later touches.
Paste the `RESULT:` lines. Explain every OTHER row (the sibling R8 flags share files). A FAIL or ERROR is a finding.

Read the user's play-test with `python tools/logs.py --grep "JUNE_ENGINE] DG"`. Fix clear transcription errors under the same flag, then
`tools/build.py --deploy`. Write "Phase R8 -- Review" into the plan doc, update the R8 STATUS row, check the R13 kickoff, and commit.
