# June character Raven (Stage 7) - Phase CH1n review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1n review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1n done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (`//#define` until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE`, `SERV_IRUHADEV_JUNE_ENUM_NAMES` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH1n - Done (2026-11-06)"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1n - Review (2026-11-04)"   # Elsword's review, the worked example
```

## Task

Check CH1n against June (read-only in Ghidra; bytes with capstone over `unpacked_x2.exe`). In order of risk:
- **The head of `32c641a0`**, from a fresh decompile and the listing: bounds `( int )slot < 0 || 4 < slot`; tables `3350f9e8` / `3350fa10` /
  `3350f9fc` / `3350fa24` by `bSlotB_` (= `_CONST_GUUSER_`'s 50.. / 60.. / 51.. / 61..); `31fd2af0` dropped, `+0x11` stores, hyper word `0x101`;
  `31fca8e0( id, 1 )`; the memo exit @32c642aa (`31fdbcc0`, `32206450( memo, 1 )`, `jne` to the epilogue `32c66d8d`) and whether any June
  `SkillNoteTemplet` row names a Raven skill (Elsword's review found none for him).
- **Types** (jump table `32c66dc0`): 5 buff (`RSI_SI1_COMMON_BUFF` / `_HYPER`, vslot 0x5f0 = Raven's `32c3da10`, EP `31fcbd60`), 6 / 7 / 0xd-0x10, 0xc + 900
  `RSI_THROW_ITEM`, default returns. Grades 14-16 = `ST_SPECIAL_ACTIVE_GRADE_1..3`.
- **Hyper switch** (`0x6018 < id`, `== 0x6018`): every thunk target and slot offset (+0x14 Init, +0x1c StartFuture, +0x24 Start, +0x34 FrameMove,
  +0x44 EventProcess, +0x54 StateEnd) against the C++; the two `31fd89b0` states of Extreme Blade (0x123 / 0x124 = the names by table); the hyper slot
  gets the EventProcess only; Burning Buster's `+0x10 = 1` write. The five new `RNSI_HA_*` values against `Enum.src.lua`.
- **`SetMaximumCannonSkillFunc`:** both switches' default jump to `@32c66cd6` (the hyper default and the id switch's `ja` / default); no explicit
  Maximum Cannon case; the helper holds March's case line for line; the `_LIVE` id-switch `default:` calls it and breaks.
- **`32c69e30`** = `WildChargeData::CreateWildChargeData()` (field layout against `X2GURaven.h`).
- **Callbacks:** `june2017_re/char_raven/ch1n_user_callback_triage.tsv`: 132 rows, every member covered by Chung's or Elsword's triage at the same
  June VA (`JuneRandomLoseEventProcess` = `RandomLoseEventProcess` `31fcd0b0`); spot-check `RidingOnEnd` `31fc8810` (SAME) and
  `RidingDamageFrontStartFuture` `31fce160` (DIFF, Stage 17) from the bytes.
- **Always.h:** the five new `#error` guards name flags the head needs.

Hygiene: `flagcheck JUNE_CHAR_RAVEN_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE --range <CH1n commit>^..<CH1n commit> --files X2Lib/X2GURaven.cpp
X2Lib/X2GURaven.h KTDXLIB/Always.h`, `JUNE_ENUM_NAMES --files X2Lib/X2SkillTree.h` (confirm with `git diff --patience --numstat`, 152 / 1 / 15 / 9 added,
0 deleted), `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GURaven.cpp`, a one-file probe of `X2GURaven.cpp` with and without
`-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. No build is needed if the review changes no code. Write "CH1n - Review" into the plan doc, update the STATUS
rows (CH1o becomes ready), check `kickoffs/char-raven/CH1o.md` and commit with explicit pathspecs.
