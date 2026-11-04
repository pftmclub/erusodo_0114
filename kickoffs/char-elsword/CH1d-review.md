# June character Elsword (Stage 7) - Phase CH1d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1d review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1d done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (commented out until CH1 final) and `_CORE` (defined by CH1d) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1d - Done (2026-11-04)"
awk -F'\t' '$7=="CH1d"' june2017_re/char_elsword/ch0_class_map.tsv
```

## Task

Review the 28 CH1d functions against June (`python june2017_re/charport_adis.py elsword --phase CH1d --out <dir>`; a listing can run past its function: stop at the `int3`). The five large ones carry the risk:
- `32cf6440` Z1 EP, `32cf67c0` Z2 EP, `32cf6d80` Z3 EP, `32cf73e0` X1 EP, `32cf7810` X2 EP: branch order, every time constant (`0.27` / `0.4`, `0.366`, `0.333`, `0.43`, `0.6`), which `m_fEventTime[n]` window and which flag index each branch uses, `|| IsAnimationEnd()` on the flagged branches, and the class tests: unit data `+0x38` (`IsDataClass`) versus unit templet `+0xaf0 -> +4` (`IsTempletClass`), June's per-site choice.
- The cancel macros `ELSWORD_GROUND_BWALK_CANCEL` / `_WALK_` / `_DASH_` (all test `IsOnSomethingFuture()`; the March macros do not) and the shared `SKILL_CANCEL_AFTER( m_fSkillCancelAfter )` (June form, `SERV_IRUHADEV_JUNE_BUFF_AWAKE_LOCK`).
- The other 23 (10 FrameMove thunks, 7 `FrameMoveFuture`, Z4 / Z4a / X3 / X3a / jump attack EPs) are written = March after the macros expand; confirm the timers (`0.17` x1.3, `0.2` dash, `0.31` x1.5, `0.3`) and the key lists.
- Declared, body elsewhere: `IsInJobLine( int )` (`32d05960`, CH1i) and the two June-only state ids `ESSI_ERS_COMBO_ZZZ` / `ESSI_EMK_COMBO_XZ` (CH1f), added under `_CORE`.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp` (FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (a Git Bash `/D` argument is path-converted: use `-D`). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1d - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH1d2.md`, commit with explicit pathspecs.
