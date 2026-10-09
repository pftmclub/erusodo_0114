# Lua source - Phase L2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase lua-source L2 review` |
| Plan doc | `LUA_SOURCE_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | Phase L2 done (its Done note and commits) |
| Flag | none |
| Log tag | `[LUA_SRC]` |

## Read first

```sh
python tools/docsection.py LUA_SOURCE_PHASES.md "Rules" "Phase L2" "Phase L2-review"
```

## Task

Try to make the checker pass a wrong file. Check:
- **Normalization.** Could ignoring constant order hide a swapped pair? Are child-proto indices resolved through the tree?
- **Numbers.** Are `KNUM` doubles compared bitwise (`-0.0`, NaN)?
- **Upvalues and flags.** Are the descriptors compared?
- **June mode.** Does it really compare against `ljconv` output?
- **`convert`.** Does it refuse on every non-T0/T1 path?
- **Mock environment.** Where are the T2 report's blind spots?

Add at least three new mutation seeds of your own; each must fail. Re-run the calibration on 20 fresh T0 files the phase did not use.

Fix clear tool defects and re-run the self-test. Hand design questions back to the user.

Write "Phase L2 - Review" into the plan doc, update the STATUS row, check `kickoffs/lua-source/L3.md`, and commit. After this review,
june-effect C3d-4 may start; say so in the hand-off.
