# June effect completion - C3g review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3g review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C3g done (its Done note and commit). The play-test is a regression only (nothing sets the key today), so the review need not wait for it |
| Flag | `SERV_IRUHADEV_JUNE_LIMIT_MAX_G_SPEED` |
| Log tag | `JUNE_FX C3g` (`python tools/logs.py --grep "JUNE_FX C3g"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3g - Done"
grep -n "(C3g)" june2017_re/funcmap.tsv
grep -n "^| 142 " JUNE_BUGS.md
```

## Task

Review C3g's commit against the June binary (Ghidra read-only; `june2017_re/disasm_range.py` / `funcdis.py` for ranges Ghidra merges).

1. **Parse sites.** User `320506f0 @32054734-3205493b` (default `fld1`, assign-if-changed into `+0x40f8`, placed after `DISABLE_GRAVITY_TIME_END`
   in March) and NPC `32784c00 @32787303-3278747f` (StateData `+0x430`, after `TRANSITION`). Confirm the user parse is reached in every
   state start (no branch skips it between the previous key and `@32054734`).
2. **NPC copy** `327ac9d0 @327ad4a0-327ad527` (the Done note corrects the kickoff's `327a0fa0`): reached on every path, and March's copy sits after
   the NOT_BOUNDING copy in both `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` branches.
3. **Readers.** User `32015b30 @32016f5e-320173e6`, NPC `3277fbf0 @32780462-327806d7`: re-run the reader compare and both clamps with
   `june2017_re/x87test.py` (or the two-operand harness the Done note describes): limit only when `L < 0`; 0, positive and NaN keep
   `fMaxGSpeed * buoyancy`; the limit is not multiplied by buoyancy; both clamps use the cap. Confirm the reverse-gravity branches read no `+0x40f8`.
4. **Not ported, check the reasons:** the minor-unit clamp `@3201745a-32017861` (JUNE_BUGS 142, checklist 7o) and `APPLY_MOVEMENT_SPEED` (-> C3h).
5. **Reach.** Re-check that no March `NPCTemplet` entry names the 17 data037 scripts carrying the key and no March class script carries it, so
   the regression play-test can show no `JUNE_FX C3g` line.
6. **Hygiene.** `python tools/flagcheck.py JUNE_LIMIT_MAX_G_SPEED --range <C3g commit>^..HEAD`. Explain the nested `X2OPTIMIZE_*` rows.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`). Write "C3g - Review" under the phase in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
