# June effect completion - C3l - June's SPEED_TIME additions (identify, then port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3l` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3h (found there). **Not with** a phase that edits `CX2GUUser::CommonStateStartFuture` or `CommonFrameMoveFuture` (C3e-1, C3h review) |
| Flag | new `SERV_IRUHADEV_JUNE_SPEED_TIME` |
| Log tag | `JUNE_FX C3l` |
| Review | `kickoffs/june-effect/C3l-review.md` (write it when done) |

## Read first

```sh
grep -n "31fe3a60 (C3h)\|31fe2740 (C3h)" june2017_re/funcmap.tsv
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3h - Done"
```

## Task

C3h found that June's user SPEED_TIME loading differs from March's inline loop in `CX2GUUser::CommonStateStartFuture` (X2GUUser.cpp, `m_vecSpeedFactor`):

- **Loader** `31fe3a60` (called `@32053fd4`): `SPEED_TIME_ULTIMATE`, used when `31fca8e0( protected +0x7cb0's +0xc, 1 )` is non-null and its
  `+0x178` is set, else (or when absent) `SPEED_TIME`. It also loads the `MINOR_UNIT_*` twins into a second list (`+0x8604`) and swaps the two lists on
  `+0x8e90` (the second skin anim).
- **Parser** `31fe2740`: TIME_SPEED gains a 5th column, a bool at `+0x10` (miss false); June's entry is 0x14 bytes, March's 0x10.

1. Identify `31fca8e0` and what `+0x178` means (an ultimate / hyper skill state?), and every reader of the 5th column (the consumer is
   `CommonFrameMoveFuture` `32061bf0`, `+0x85f4` hits `@320620ff-320623a2`). Verdicts into `june2017_re/funcmap.tsv`, tagged `Effect C3l`.
2. Port the engine side under the new flag: the `_ULTIMATE` choice, the 5th column and its reader. The `MINOR_UNIT_*` lists and the `+0x8e90` swap
   are Lu/Ciel's (checklist 7o): do not port them; add them to 7o.
3. June data: count the June class scripts that use `SPEED_TIME_ULTIMATE` or a 5-column SPEED_TIME (`june_src_fixed/`). They reach March only
   with the class ports (checklist row if C3h's 7o text is not enough). Play-test is a regression only unless March data uses it.
