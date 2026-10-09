# June effect completion - C3h review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3h review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C3h done (its Done note and commit). The play-test is a regression only (nothing sets the key today), so the review need not wait for it. **Not with** C3e-1 (PhysicProcess) |
| Flag | `SERV_IRUHADEV_JUNE_APPLY_MOVEMENT_SPEED` |
| Log tag | `JUNE_FX C3h` (`python tools/logs.py --grep "JUNE_FX C3h"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3h - Done"
grep -n "(C3h)" june2017_re/funcmap.tsv
```

## Task

Review C3h's commit against the June binary (Ghidra read-only; `june2017_re/disasm_range.py` / `dispsweep.py` for ranges).

1. **Parse.** `320506f0 @32054181-3205418e`: a miss stores false, and no branch between the SPEED_TIME call (`@32053fd4`) and `@32054181` skips it,
   so it is read in every state start. March's parse sits after PASSIVE_SPEED_X/Y and before DISABLE_GRAVITY_TIME_START.
2. **Rescale** `@3205419d-320542b9`: the rate is `31e4f990() / +0x324`, rounded to float. `nowSpeed.x` is scaled unconditionally, `passiveSpeed.x`
   unless `IsSamef( x, -1 )`, and each SPEED_TIME `vSpeed.x` whatever its flag. Re-check that `31e4f990` is March's `CX2GameUnit::GetWalkSpeed()`
   (`+0x423c` = m_AdditionalWalkSpeed's result, protected `+0x4910` = m_fRunJumpSpeedRate).
3. **Decay** `32015b30 @320168ab-32016990`: re-run the sign test and both clamps with a **popping** harness. `x87test.make_func` leaks one x87 slot
   per call and reads "unordered" after 8 calls (Done note). Check that only the decay term changed.
4. **Storage.** The bool is a PhysicParam field (not the plain member the kickoff asked for). Check the reason: June's three Org -> PhysicParam
   rep movsd copies (`322a5f90`, `322886a0`, `31fcde30`) are its only clearers, and March has the same three copies. Re-sweep for any other
   writer, including offset-0 access through a `PhysicParam*`.
5. **Not ported, check the routes:** the minor unit (checklist 7o); `31fe3a60` / `31fe2740` (SPEED_TIME 5th column, `_ULTIMATE`) -> C3l;
   `32296290` (the `+0x334` world-physics backup) -> C3m.
6. **Hygiene.** `python tools/flagcheck.py JUNE_APPLY_MOVEMENT_SPEED --range <C3h commit>^..HEAD`.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`). Write "C3h - Review" under the phase in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
