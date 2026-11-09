# June character Rena (Stage 7) - Phase CH1 review - Final: LIVE, NF, the script, the motions, the play-test

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1 review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | Phase CH1 done (its Done note, commit and the user's play-test); any `CH1r`-style fix the play-test opened done |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, `_NF` (both defined by CH1; `_NF` also in `ServerDefine.h`), `_NF_DEBUG` (retired here) |
| Log tag | `[JUNE_CHAR]` (`RENA DATA`, `RENA NF`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH1 - Done (2026-11-09)" "CH1q - Review (2026-11-09)" "CH1q2 - Review (2026-11-09)"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1 - Review (2026-11-06)"
```

## Task

Review CH1 final against its Done gate (plan section "Phase CH1 - Deploy unit", *Done gate (CH1 final)*):
- **Deploy unit:** data036 `Lire_ElvenRanger.lua` equals the `june2017_re/june_adopt_20261109_180855.tsv` ljconv row (size, sha1, header
  `1B 4B 4C 81`); no `.src.lua` sidecar; data033's five `Motion_Lire*.x` are June's bytes (`kom.py stage-june --dry-run
  june2017_re/char_rena/ch1_held_motions.tsv`: 5 identical); `kom.py status data036 data033` in sync with live. The deployed exe holds
  `RENA DATA` once (`tools/exestr.py --form wide`).
- **`RENA DATA` lines:** state count per class against `june2017_re/char_rena/ch1m_reg_by_class.tsv` (C++ registrations + `USI_EVENT_*` + 20
  slot states + key 0 + learned-skill sub-states, as Raven's review), `wait_state_change 1`, a June `MOTION_LIRE*` device id, script
  `Lire_ElvenRanger.lua`.
- **START / WIN / LOSE:** the rolls pick real tables (no `list size 0` / `re-bind skipped` for `LESI_` in the log).
- **Natural force (`RENA NF`, per decision 1 = (a)):** gauge UI line on entering a game; `count n / 5` steps one point per 3 first hits;
  the combo bomb / skill explosion spends; awakening `MP +15n` and the count after = min( n + 5, 5 ); `from packet` after a relog carries the
  saved count (`abil_count`). A step the user's run missed is asked for once. Then **retire `_NF_DEBUG`** (comment out its `#define`; the
  `_LIVE` include keeps `X2OfflineLog.h` for `RENA DATA`), start the build (not awaited, debug-retire rule).
- **Skills are not checked** unless the run shows a cast (unported skills cannot be cast, Raven's precedent); the cool time / MP check is CH2a's.
- **ErrorLog** against `data\ErrorLog_pre_CH1_rena.txt` (1,046,346 B, the log left by the last pre-CH1 run, 2026-11-08 19:09) and, for
  Rena-load lines, `ErrorLog_pre_CH0b_rena.txt`: no new `KEM_ERROR` / `Motion Error` / missing Rena animation and no new `[LUA_SRC]` error;
  anything new is routed under the scope rule.
- The `(Rena: CH1 ...)` checklist marks (l.16, 423, 427, 509, 548, 592, 674, 760) and the `charport_scan.py` / `charport_keys.py` verdict.
- The two `june_diverge_allow.tsv` rows CH1 added (`ReInit` `32ca4a40` @32ca4ac9, `InitComponent` `32c895b0` @32c895f4: type 1 only): confirm
  in Ghidra.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_RENA_LIVE --range <CH1 commit>^..<CH1 commit>` (and `_NF`), `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`.

Spot-check 2-3 items directly in Ghidra (read-only). Fix clear transcription errors under the same flag; hand design decisions back to the
user. Write "CH1 - Review" into the plan doc, update the CH1 STATUS.md row (CH2a and CH5 become ready), check `kickoffs/char-rena/CH2a.md`
and `CH5.md` still match, and commit.
