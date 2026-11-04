# June character Elsword (Stage 7) - Phase CH1d2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1d2 review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1d2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (commented out until CH1 final) and `_CORE` (defined by CH1d) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1d2 - Done (2026-11-04)"
awk -F'\t' '$7=="CH1d2"' june2017_re/char_elsword/ch0_class_map.tsv
```

## Task

Review the 28 CH1d2 functions against June (`python june2017_re/charport_adis.py elsword --phase CH1d2 --out <dir>`; a listing can run past its function: `32cf7cb0` runs on into `32cf8110`, stop at the `int3`). The nine with branches carry the risk:
- `32cf7cb0` dash Z1 EP (four flagged follow-ups on `m_fEventTime[0..3]`, each with its class test: unit data `+0x38` versus templet `+0xaf0 -> +4`; `IsInJobLine( 1 )` for flag 2), `32cf8110` dash Z2 EP, `32cf8230` Sword-Knight dash Z2 EP (the `+0x8ca4` UPDOWN cancel, then the skill / back-walk / walk / dash / Z / X cancels, then a separate end-of-animation `if`), `32cf8b30` dash attack X EP (templet 112 first; unit data 100 / 101 / default with three different skip-key lists and `0.66666` / `0.6`), `32cf90d0` / `32cf9220` dash jump Z1 / Z2 EP (job lines 2, 3, 1), `32cf9440` dash jump X1 EP (flag writes, templet classes 11 / 101), `32d0fa10` Z3 landing EP (the dead second dash test is left out).
- Which window and flag each branch uses, every time constant (`0.233` / `0.63`, `0.9`, `1.0`, `0.26` / `0.37`, `0.15`, `0.23`), `|| IsAnimationEnd()` on the flagged branches, the key lists (`ELSWORD_ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_WITH`).
- The other 19 are written = March (thunks, start futures, landing starts, `X2End`); confirm they are.
- Declared, body elsewhere: seven June-only state ids appended to the `ESSI` enum under `_CORE` (their states are CH1e / CH1f / CH1g / CH1g2) and `IsInJobLine( int )` (CH1i). `Always.h` gained `_LIVE needs SERV_IRUHADEV_JUNE_UPDOWN_CANCEL`.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp` (FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (a Git Bash `/D` argument is path-converted: use `-D`). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1d2 - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH1e.md`, commit with explicit pathspecs.
