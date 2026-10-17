# June engine remainder - Phase R4e - UI control pass argument and the other post-effect overrides

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (map, then port), then a user play-test; type `/x2-phase june-engine R4e` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R4a (pass argument, `SetBlockPostEffect` / `SetBlockStringPostEffect` members); not alongside another session editing `KTDGUI*.cpp` |
| Flag | `SERV_IRUHADEV_JUNE_UI_CONTROL_PASS` (proposed; the phase may split it and says so in its Done note) |
| Log tag | `[JUNE_ENGINE] UIPASS` (`python tools/logs.py --tag JUNE_ENGINE`) |
| Review | `kickoffs/june-engine/R4e-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase R4a -- Done" "Phase R4e - UI control pass argument and the other post-effect overrides"
```

Then the funcmap rows tagged `(R4a)` (`grep -a "(R4a)" june2017_re/funcmap.tsv`) and `(U1)` for the control render addresses.

## Task

Do "Phase R4e". R4a gave the dialog render, the dialog manager and the drag slot June's pass argument, but every control still draws
through March's `OnFrameRender()`. In June the control render is `vslot 8 ( pass )` (`ret 4`) in every control class: button `31ca37f0`,
radio `31cab2b0`, slot `31cb1ef0`, static `31cbec10`, IME edit `31cb9ff0`, and the rest (list them from the control vtables). Each passes
the pass down to its texture / vertex draws, and draws its strings in a pass other than 0 only when `+0x242` (`SetBlockStringPostEffect`)
is set (`31cab34e`, `31cb1f9e`, `31cbeedc`, `31cd196c`).

- Map each control's render against March first (one row per class, `(R4e)` in funcmap), then port the pass argument whole (rule 1):
  the control renders, their draw helpers that take the pass, and the two call sites R4a left with a comment (dialog render controls
  `31cd7810 @31cd7b6c / @31cd7b9a`, guide control `31d053f0 @31d05695`).
- The other June overrides of `CKTDGObject` vslots `0x10` / `0x2c`: `CX2UnitViewerUI` (`31f97960` / `31fae370`, vtable `33471184`) and any
  other class whose vtable overrides them (sweep the RTTI vtables; route a class that is not in March to its own plan).
- Every March caller still passes 0, so nothing in game may change. Gate: build + deploy, and UI looks and behaves as before.
