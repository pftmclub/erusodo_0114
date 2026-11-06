# June character Raven (Stage 7) - Phase CH1 review - Final: LIVE, RAGE, the script, the motions, the play-test

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1 review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | Phase CH1 done (its Done note, commit and the user's play-test); any `CH1r`-style fix the play-test opened done |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, `_RAGE` (both defined by CH1), `_RAGE_DEBUG` (retired here) |
| Log tag | `[JUNE_CHAR]` (`RAVEN DATA`, `RAVEN RAGE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH1 - Done (2026-11-06)" "CH1q - Review (2026-11-06)"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1 - Review (2026-11-04)"
```

## Task

Review CH1 final against its Done gate (plan section "Phase CH1 - Deploy unit", *Done gate (CH1 final)*):
- **Deploy unit:** data036 `RAVEN_FIGHTER.lua` equals the `june2017_re/june_adopt_20261106_143049.tsv` ljconv row (size, sha1, header `1B 4B 4C 81`);
  no `.src.lua` sidecar; data033 `Motion_Raven_OT.x` is June's bytes (`kom.py stage-june --dry-run june2017_re/char_raven/ch1_held_motions.tsv`: 1 identical);
  `kom.py status` shows data036 / data033 in sync with live. The deployed exe holds `RAVEN DATA` once (`tools/exestr.py`).
- **`RAVEN DATA` lines:** state count per class against `june2017_re/char_raven/ch1m_reg_by_class.tsv` (constant offset and the learned-skill
  sub-states explained as in Elsword's review), `wait_state_change 1`, a June `MOTION_RAVEN*` device id, script `RAVEN_FIGHTER.lua`.
- **START / WIN / LOSE:** the rolls pick real tables (no `list size 0` / `re-bind skipped` for `RSI_` in the log).
- **Nasod core (`RAVEN RAGE`):** `hit gain`, `ball level`, `parry` and `orb ball` lines match the steps (rage / 20 ball level, parry -20 and the < 60
  floor, the orb ball at ( 74, 60 ) while three orbs are held, gone at awakening). A step the user's run missed is asked for once. Then **retire
  `_RAGE_DEBUG`** (comment out its `#define`; the `_LIVE` include keeps `X2OfflineLog.h` for `RAVEN DATA`), build, `exestr.py --absent` on the
  four `RAVEN RAGE` strings.
- **Z re-attack after a full Z-Z-Z** (CH1d routing): June no longer re-arms `m_fReAttackZ1Time`; record what the run showed.
- **ErrorLog** against `data\ErrorLog_pre_CH1_raven.txt` (192,987 B, a start-up-only run 2026-11-06 14:35 on the pre-CH1 exe) and, for Raven-load
  lines, `ErrorLog_pre_CH0b_raven.txt`: no new `KEM_ERROR` / `Motion Error` / missing Raven animation and no new `[LUA_SRC]` error; anything new is
  routed under the scope rule.
- The nine `(Raven: CH1 ...)` checklist marks (l.16, 120, 423, 427, 509, 548, 592, 674, 759), `JUNE_BUGS.md` rows 543 / 544, and the
  `charport_scan.py` / `charport_keys.py` verdict.
- The `SERV_RAVEN_VETERAN_COMMANDER` guard moved from `Always.h` to the top of `X2GURaven.cpp` (ServerDefine.h is invisible to KTDXLIB): confirm with
  `cl /EP` that it fires in X2Lib with the flag off and never runs in KTDXLIB.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_RAVEN_LIVE --range <CH1 commit>^..<CH1 commit>` (and `_RAGE`), `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GURaven.cpp`.

Spot-check 2-3 items directly in Ghidra (read-only). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "CH1 - Review" into the plan doc, update the CH1 STATUS.md row (CH2a and CH5 become ready), check
`kickoffs/char-raven/CH2a.md` and `CH5.md` still match, and commit.
