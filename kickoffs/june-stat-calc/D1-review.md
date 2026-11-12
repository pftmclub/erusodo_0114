# June damage and stat calculation - Phase D1 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-stat-calc D1 review` |
| Plan doc | `JUNE_STAT_CALC_PHASES.md` (branch `main`) |
| Depends on | Phase D1 done (its Done note and commits), play-test passed |
| Flag | `SERV_IRUHADEV_JUNE_STATCALC_DEFENCE_CURVE` (+ `_STATCALC_DEBUG`, which stays on until the D6 review) |
| Log tag | `[JUNE_STATCALC]` |

## Read first

```sh
python tools/docsection.py JUNE_STAT_CALC_PHASES.md "Rules" "Starting context" "Measurement (planning session, 2026-11-12)" "Phase D1 - Defence curve: the 0-damage fix" "Phase D1 - Done"
```

## Task

Review D1's commit(s) against the exe: the rate's operand order and constant widths (doubles, the float stores between), the level
source and the damage-free override, the two multipliers' identities, the floor at 0 and what happens below 0 and above 1, the
level-gap factor verdict (re-prove "none" or the twin's body), and that the `#else` path is March's body byte for byte.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_STATCALC_DEFENCE_CURVE --range <first D1 commit>^..<last D1 commit> --with
SERV_IRUHADEV_JUNE_STATCALC_DEBUG`, then `git log -S<FLAG> <last D1 commit>..HEAD`; `python tools/junediverge.py --check-all`.

Re-run `june2017_re/statcalc/emu_defence.py` and add 50 input sets of your own. Read the play-test's `HIT` lines for the named
monsters only. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); design questions go to the user.
The debug define is not retired here. Write "Phase D1 - Review", update STATUS, check the D2 kickoff, commit.
