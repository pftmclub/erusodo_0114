# June character Elsword (Stage 7) - Phase CH1 review - Final: LIVE, the script, the play-test

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1 review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | Phase CH1 done (its Done note, commit and the user's play-test); any `CH1r`-style fix the play-test opened done |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, `_WOS` (both defined by CH1), `_WOS_DEBUG` (retired here) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD DATA`, `ELSWORD WOS`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1 - Done (2026-11-04)" "CH1q2 - Review (2026-11-04, Opus 5.5 high)"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1 - Review (2026-11-02)"
```

## Task

Review CH1 final against its Done gate (plan section "Phase CH1 - Deploy unit", *Done gate (CH1 final)*):
- **Deploy unit:** data036 `Elsword_SwordMan.lua` and `WayOfSword.lua` equal the `june2017_re/june_adopt_20261104_194902.tsv` ljconv rows (size, sha1,
  header `1B 4B 4C 81`); no `.src.lua` sidecar for either; `kom.py status data036` in sync with live. `DLG_Elsword_WayOfSword.lua` (data034) is
  June's bytes. The deployed exe holds `ELSWORD DATA` once (`tools/exestr.py`).
- **`ELSWORD DATA` lines:** state count per class against `june2017_re/char_elsword/ch1m_reg_by_class.tsv` (constant offset and the learned-skill
  sub-states explained as in Chung's review), `wait_state_change 1`, a June `MOTION_ELSWORD*` device id, script `ELSWORD_SWORDMAN.lua`.
- **START / WIN / LOSE:** the rolls pick real tables (no `list size 0` / `re-bind skipped` for `ESSI_` in the log).
- **Way of the Sword:** `ELSWORD WOS vigor start` / `destruction start` / `destruction rage` / `attack gain` / `UI state` lines match the steps; the UI
  state reaches 2 and -2 (CH1's run did). CH1's run had no `destruction rage` line: before retiring, ask the user once for it (enter destruction
  mode in a dungeon, then take a hit from a monster) and read it. Then **retire `_WOS_DEBUG`** (comment out its `#define`; the lines go dark; the `_LIVE` include keeps `X2OfflineLog.h`),
  build, `exestr.py --absent` on the five `ELSWORD WOS` strings.
- **Revenge counter** (CH1b): `ESSI_REVENGE_ATTACK` hits for a non-zero amount (no `DAMAGE_PHYSIC`, `NUMERICAL_DATA_INDEX` 1).
- **ErrorLog** against `data\ErrorLog_pre_CH1_elsword.txt` (212,757 B, CH0b's run 2026-11-04 04:4x): no new `KEM_ERROR` / `Motion Error` / missing
  Elsword animation and no new `[LUA_SRC]` error; anything new is routed under the scope rule.
- The eight `(Elsword: CH1 ...)` checklist marks (l.15, 421, 425, 507, 546, 590, 672, 757) and the `charport_scan.py` / `charport_keys.py` verdict.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_ELSWORD_LIVE --range <CH1 commit>^..<CH1 commit>` (and `_WOS`), `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`.

Spot-check 2-3 items directly in Ghidra (read-only). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "CH1 - Review" into the plan doc, update the CH1 STATUS.md row (CH2a and CH5 become ready), check
`kickoffs/char-elsword/CH2a.md` and `CH5.md` still match, and commit.
