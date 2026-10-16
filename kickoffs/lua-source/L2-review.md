# Lua source - Phase L2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase lua-source L2 review` |
| Plan doc | `LUA_SOURCE_PHASES.md` (branch `main`) |
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
- **June mode.** Does it really compare against `ljconv` output? Spot-check 3 entries of `lua_source_opcode_check.tsv` in Ghidra
  yourself, including one from a same-mode family (arith VV/VN/NV or ISLT/ISGE/ISLE/ISGT).
- **`convert`.** Does it refuse on every non-T0/T1 path?
- **Mock environment.** Where are the T2 report's blind spots?
- **Table constants (user rule, 2026-10-09).** T1 compares a template's pairs as a set and ignores their storage order. Try to
  make a wrong table pass: a pair moved to another table or another nesting level, an array item moved into the hash part, a
  `nil`-valued pair, a duplicate key.
- **Layout tidy-up (`tools/ljsrc.py`).** Best effort, runs only on a text that already passes T1, and keeps the last text that
  still passes. Could it ever move a field that emits an instruction (a call, a non-constant value, the trailing positional item)?

Add at least three new mutation seeds of your own; each must fail (`luasrc.py selftest --seed KOM:NAME[:t0]` adds a seed file; new
kinds go into `ljsrc.mutants()` or `ljbc.dump_mutants()`). Re-run the calibration on 20 fresh T0 files the phase did not use:

```sh
python tools/luasrc.py calibrate --sets march-t0 --seed R1 --per-dir 2 --exclude june2017_re/lua_source_calibration.tsv --out .build/l2review_cal.tsv
```

Ghidra spot-check rows (`june2017_re/lua_source_opcode_check.tsv`): KOG `00`/`01` (ISLT `jbe` at `0x32fee670`, ISGE `ja` at `+65`) are a
same-mode pair; June's handler table is `PTR 0x337411d8`, copied by `lj_dispatch_init` (`0x32ffb270`, renamed in Ghidra by L2).

Fix clear tool defects and re-run the self-test. Hand design questions back to the user.

Write "Phase L2 - Review" into the plan doc, update the STATUS row, check `kickoffs/lua-source/L3.md`, and commit. After this review,
june-effect C3d-4 may start; say so in the hand-off.
