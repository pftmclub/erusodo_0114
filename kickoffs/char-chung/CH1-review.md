# June character Chung (Stage 7) - Phase CH1 review - Final: LIVE, the script, the play-test

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | Phase CH1 done (its Done note, commit and the user's play-test); any `CH1r`-style fix the play-test opened done |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, `_CBALL` (both defined by CH1), `_CBALL_DEBUG` (retired here) |
| Log tag | `[JUNE_CHAR]` (`CHUNG DATA`, `CHUNG CBALL`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1 - Done (2026-11-02)" "CH1q - Review (2026-11-02)"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH1 - Done (2026-10-27)"
```

## Task

Review CH1 final against its Done gate (plan section "Phase CH1 - Deploy unit", *Done gate (CH1 final)*):
- **Deploy unit:** data036 `Chung_Iron_Cannon.lua` equals the `june2017_re/june_adopt_20261102_094116.tsv` ljconv row (size, sha1, header `1B 4B 4C 81`);
  no `Chung_Iron_Cannon.src.lua` sidecar; `kom.py status data036 data033` in sync with live (data033 = CH0b's 10 `Motion_CHUNG_*`). The deployed
  exe holds `CHUNG DATA` once (`tools/exestr.py`).
- **`CHUNG DATA` lines:** state count per class against `june2017_re/char_chung/ch1m_reg_by_class.tsv` (class gaps equal, a constant offset explained
  as in Aisha's / Eve's reviews), `wait_state_change 1`, a June `MOTION_CHUNG*` device id.
- **START / WIN / LOSE:** Chung's March `ParseCommonRandomState` stays (not under `_LIVE`): its 7 table names equal June's `INIT_COMMON_RANDOM_STATE`
  keys and `GetUnitClassName`'s Chung names, so the rolls pick real tables. Confirm in Ghidra that June's Chung vtable slot for it adds nothing
  over the base `31feaac0` parse (no `list size 0` / `re-bind skipped` for `CSI_` in the log).
- **Cannon balls:** `CHUNG CBALL start ... dungeon 3 pvp 0`; gain / spend / gauge refill lines match the play-test steps. CH1's run (2026-11-02)
  had no `gauge refill` line (no spend emptied the count while awakened): before retiring the debug flag, ask the user for the awakening step once
  more (`g_pX2Game:GetMyUnit():SetHyperModeCount( 3 )` in a dungeon, press the awakening key, fire X until the gauge is empty) and read it. Then **retire
  `_CBALL_DEBUG`** (comment out its `#define`; the lines and the debug include go dark), rebuild.
- **ErrorLog** against `data\ErrorLog_pre_CH1_chung.txt` (270,399 B, run 2026-11-02 07:10): no new `KEM_ERROR` / `Motion Error` / missing Chung
  animation and no new `[LUA_SRC]` error; anything new is routed under the scope rule.
- The eight `(Chung: CH1 ...)` checklist marks and the §4 verdict from `charport_scan.py`.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_CHUNG_LIVE --range <CH1 commit>^..<CH1 commit>` (and `_CBALL`), `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GUChung.cpp`.

Spot-check 2-3 items directly in Ghidra (read-only). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "CH1 - Review" into the plan doc, update the CH1 STATUS.md row (CH2a, CH3b0 and CH5 become ready),
check `kickoffs/char-chung/CH2a.md`, `CH3b0.md` and `CH5.md` still match, and commit.
