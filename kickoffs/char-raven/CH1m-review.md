# June character Raven (Stage 7) - Phase CH1m review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1m review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1m done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (`//#define` until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH1m - Done (2026-11-06)"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1m - Review (2026-11-04)"   # Elsword's review, the worked example
```

## Task

Check CH1m's registration against June (read-only in Ghidra). In order of risk:
- **The simulation.** Fresh decompiles of `32c5d660` / `32c66fe0` / `32c511c0` / `32c51b70` / `32c63670` through `june2017_re/char_raven/ch1m_regsim.py`
  must reproduce `ch1m_reg_by_class.tsv` row for row; its condition overrides (`iVar5` as the class register @1088 but also the LOSE2 compare @261 and
  the shared_ptr release counts, all `(iVar5 == 0)` taken false; the `piVar` / `local_10` / `piStack_` releases) are where it could be wrong.
  `ch1m_regcheck.py` must print `0 problems`.
- **The class tests of `32c66fe0`, against the listing:** the @344 chain (107 / not 115 / 115), the @564 chain (106 / not 115 / 115), 25 or 115 (@720),
  the @815 switch (16+106 / 107 / 115; the shared call @1085 with `uVar7` / `pcVar8` is the last row of each case, after the `320775a0` Init), and
  16 / 106 / 115 @1088; the line switch @315 (16+106 -> `32c511c0`, 17+107 -> `32c51b70`, 25+115 -> `32c63670`).
- **The head of `32c66fe0`:** cut-in jump table `32c6847c` / `32c68490` (4 / 16 / 17 / 25 -> 0, 106 -> 1, 107 -> 2, 115 -> 3, nothing for others), the
  sub-index chain, the inlined InitStateID @32c6709f-32c670a5 (`+0x4088` / `+0x7d04` = 0xd: is that all, so March's live body really is equal?), the call
  order (`32c5d660`, `3202b110`, `31fe22d0`), START / WIN / LOSE slots (WIN: no FrameMove for Raven), the `RSI_RWT_LOSE2` compare @32c673b1 and the slot
  it fills, `InitWaitStateChange` @32c67494, the tail vslots 0x690 / 0x684( 0 ) / `31fd8f00` @32c6839f, and `31fc93d0` (empty stub) left uncalled.
- **Ids by name:** `ch1m_idmap.tsv` one-to-one both ways (147 ids). June 304 `RWT_COMBO_ZZZX` is its own state beside 168 `RSI_COMBO_ZZZX` (March
  shared 168): confirm from the listing pushes, and that the `RBM_` / `RRF_` / `RWT_` / `RSI_SOUL_TAKER_` tables June puts on 168 / 182 / 173 / 177 match
  March's shared ids. June 72 = `GetRidingStartStateID()`.
- **Routed registrations:** CH1b's stand-up pair on its ten states, CH1d / CH1e / CH1f / CH1g2's new functions, `XXfrontXX4_EventProcess` on `XXfrontXXdownX`,
  Veteran Commander's double `RSI_SOUL_TAKER_DASH_COMBO_ZZ` (32c63670 @268 then InitState @1114).
- **Left out:** phoenix run 123-126, dance 127-129, riding 93-96 and the empty slots `3202a7d0`, `32002f70`; JUNE_BUGS 542 (riding back damage without a table).
- **Always.h:** the seven new `#error` guards each name a flag the registration needs.

Hygiene: `flagcheck JUNE_CHAR_RAVEN_LIVE --range <CH1m commit>^..<CH1m commit> --files X2Lib/X2GURaven.cpp KTDXLIB/Always.h` (confirm with
`git diff --patience --numstat`, 1377 / 0 and 21 / 0), `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GURaven.cpp`, a one-file probe of
`X2GURaven.cpp` with and without `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. No build is needed if the review changes no code. Write "CH1m - Review"
into the plan doc, update the STATUS rows (CH1n becomes ready), check `kickoffs/char-raven/CH1n.md` (Amended by CH1m) and commit with explicit pathspecs.
