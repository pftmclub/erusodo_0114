# June effect completion - C9h-4c review - UPDOWN_CANCEL_AFTER, USE_COMMON_CANCEL_AFTER, the clamp, June's RidingAttackZEventProcess

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-4c review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-4c done (its Done note), play-test logs read |
| Flags | `SERV_IRUHADEV_JUNE_UPDOWN_CANCEL` (+ `_DEBUG`), `SERV_IRUHADEV_JUNE_COMMON_CANCEL`, `SERV_IRUHADEV_JUNE_CANCEL_CLAMP` (+ `_DEBUG`) |
| Log tags | `JUNE_FX UPDOWN_CANCEL`, `JUNE_FX CANCEL_CLAMP` (`python tools/logs.py --grep "JUNE_FX CANCEL_CLAMP"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-4c - Done" "C9h-4 - Done"
grep -n "Effect C9h-4c" june2017_re/funcmap.tsv
```

## Task

Review the C9h-4c code against June in Ghidra (read-only) and fix transcription errors under the same flags. The things it could get wrong:
- **The clamp direction.** `320568d3..32056a1d` and `3205fa38..`: `fcom st(1)` of 0.1 (double `334395e8`, float `334395e0`) against the value, `test ah,0x41; jne keep`,
  else store 0.1. Confirm it is `max( v, 0.1 )`, NaN kept, and that all eight floats are in both functions (Future `+0x8c88..+0x8c9c`, `+0x8ca4`, `+0x8ca8`;
  Now `+0x8d24..+0x8d3c`, `+0x8d40`). The Done note lists what the `CANCEL_CLAMP` play-test lines show.
- **The Now set.** June has two member sets (Future `+0x8c88..`, Now `+0x8d24..`); March has one, written by both parses. Confirm nothing in June reads the Now
  set in a way that makes the single March member differ (the funcmap row for `32057b80` and a `dispsweep` of `8d24`-`8d40` for readers that are not the parse).
- **`RidingAttackZEventProcess` `32031e60`** against the port: first block (pet, anim ptr, `+0x2ac` anim end -> 0x4c; else `!GetRidingOn` -> 0x4a), then SKILL
  (`+0x8c90`, `3202f460( 1 )`), DASH (`+0x8c9c`, `+0x86dc/+0x86e0`, vslot `0x668`), WALK (`+0x8c98`, `+0x86da/+0x86de`), UPDOWN (`+0x8ca4`, `+0x86e2/+0x86e6`), then vslot `0x7d4`.
  The `3202f460( 1 )` argument: confirm 1 = `ASUT_GROUND` (March's default) in June's own switch.
- **`DoFrameMoveFuture` arm** (`320347d0 @32034858-32034879`): pet object exists, ONGROUND check false, `+0x620` set -> `32031e60`, skip the state's event process.
- **`CommonEventProcess` block** (`31fe7610 @31fe7618-31fe7902`): each branch against the port (the Done note's table), including that `IF_SKILL_CANCEL_AFTER` is June's own
  expansion (skill needs `IsOnSomethingFuture`; the awakening branch's conditions) and that BWALK / WALK change to state 13.
- **Hygiene:** `python tools/flagcheck.py JUNE_UPDOWN_CANCEL --with JUNE_UPDOWN_CANCEL_DEBUG JUNE_COMMON_CANCEL JUNE_CANCEL_CLAMP JUNE_CANCEL_CLAMP_DEBUG --range <first C9h-4c commit>^..HEAD`.

Fix clear transcription errors under the same flags (then `tools/build.py --deploy`); hand design decisions back to the user (the clamp is the one the user may want gone:
it changes March data). Turn the `_DEBUG` flags off in the review commit. Write "C9h-4c - Review" into the plan doc, update the phase's STATUS.md row, check the next kickoff, and commit.
