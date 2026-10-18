# June engine remainder - Phase R4e review - UI control pass argument and the draw primitives' pass

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine R4e review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase R4e done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_DRAW_PASS`, `SERV_IRUHADEV_JUNE_UI_CONTROL_PASS` |
| Log tag | `[JUNE_ENGINE]` (R4e adds no log line: the gate is "UI unchanged") |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Starting context" "Phase R4e - UI control pass argument and the other post-effect overrides" "Phase R4e -- Done"
```

Then the funcmap rows tagged `(R4e)` (`grep -a "(R4e)" june2017_re/funcmap.tsv`) and JUNE_BUGS 287.

## Task

Review Phase R4e's commit(s) against June. What this phase could get wrong:

- **The font class.** The port is on `CKTDGDeviceFont` (June vtable `3344de70`, RTTI `3361154c`; `Always_US.h` `#undef`s `USE_FREE_TYPE`).
  Re-read `Flush` `31d1bae0` and the second state table `337295d8` (15 entries, decoded in the Done note) against the new
  `s_akStatesJunePostEffect`.
- **Live branches.** `USE_DT_VCENTER` is on, so `OutTextXY`'s live `Buffering` calls are the `VCENTER` ones; check that each of them, and
  each `Flush`, carries the pass.
- **Each control render** against June's: button `31cb1ef0`, radio `31cab2b0`, slot `31cd16a0`, static `31cbec10`, list box `31ca37f0`,
  edit `31cb3ae0`, IME `31cb9ff0`, control list `31cca350`, scroll bar `31d47b10`. Check the `+0x242` gates, the pass-1 early returns, the
  static's pass-1 picture path (draw count 1, no blend push), its movie gate, and that the edge helpers' new copies keep March's geometry.
- **The `#else` paths** and the signature-only classes (check, combo, navigation, slider, `CKTDGUIListControl`, `CX2TalkBoxManagerUI`): is
  every override still an override (no hidden `OnFrameRender()` left anywhere in `KTDXLIB` / `X2Lib`)?

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py JUNE_UI_CONTROL_PASS --range <first R4e commit>^..<last R4e commit>` and the same for `JUNE_RENDER_DRAW_PASS`.
Paste the `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every OTHER row and every deleted line; a FAIL or
ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same
flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "Phase R4e -- Review" into the plan doc, update the
phase's STATUS.md row, check the R4i / R4j kickoffs, and commit.
