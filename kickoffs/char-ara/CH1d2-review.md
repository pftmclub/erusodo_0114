# June character Ara (Stage 7) - Phase CH1d2 review (covers CH1c-CH1d2)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1d2 review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1c, CH1d and CH1d2 done (their Done notes and commits) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (`//#define`), `_CORE` (defined), `_FP` (`//#define`) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CH1c - Done" "CH1d - Done" "CH1d2 - Done"
awk -F'\t' '$7=="CH1c" || $7=="CH1d" || $7=="CH1d2"' june2017_re/char_ara/ch0_class_map.tsv
```

## Task

Review the functions of CH1c (2), CH1d (29) and CH1d2 (26) against June (`python june2017_re/charport_adis.py ara <va>...`; the listing carries the `StateChange` arguments and the `fcomp` / `test ah` direction; `je` after `cmp byte, 1` jumps when the byte IS 1). Spot-check at least:
- **`32bb37e0` / `32bb3ab0`** (Dash / DashEnd EP): line-3 X branch (`IsInJobLine( 3 )` -> `ASI_DASH_COMBO_ALS_X`), the hyper entry, the four DASH_END direction tests, `GetMaxAnimationTime() * 0.3f`.
- **`32ba74b0`** (DashJump EP): the both-keys-held branch and the unchanged `pureRight || pureLeft` landing test; `32b9a280` / `32b9a780` (class numbers 0x1c / 0x76 / 0x1f / 0x77 / 0x7c per event, event 3 on line 3).
- **`32b99960`** (ComboDashZ EP): event order 3, 0, 1, 2; the Z-push-X condition `pureX && !IsInJobLine( 3 )`.
- **`32b99fe0`** (ComboDashX FP cancel) beside CH1d's `32b99890`; **`32b91120`** vs **`32b91000`** (the two charge-scaling StateStarts); JUNE_BUGS 586 / 587.
- **CH1c / CH1d:** the combo flag macros (`ELSE_IF_STATE_CHANGE_ON_EX_OR_ANIMATION_END_`, the `_ON_LINE_` cancel pair), the job-line branches CH1d added, `RidingHyperModeFrameMove`.
- `june2017_re/char_ara/ch1_offsets.tsv` rows against the listings (the dash charge shares June's `+0x9604` with ZZZX; March keeps two members).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_ARA_CORE SERV_IRUHADEV_JUNE_CHAR_ARA_FP --range <CH1c commit>^..<CH1d2 commit>` (FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; one-file `/Zs` probe of `X2GUAra.cpp` off and with `/D SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`. Fix clear transcription errors in place (bytepatch only). Write "CH1d2 - Review" into the plan doc, update the STATUS rows (CH1d2 done, CH1e ready), check `kickoffs/char-ara/CH1e.md`, commit with explicit pathspecs.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1e` (Sonnet 5.5 high).
