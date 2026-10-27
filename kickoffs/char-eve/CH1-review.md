# June character Eve (Stage 7) - Phase CH1 review - Final: LIVE, the script, the play-test

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH1 done (its Done note, commit and the user's play-test), CH1r and CH1s done (the play-test's two findings) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (defined here; it turns on `_NCORE`) |
| Log tag | `[JUNE_CHAR]` (`EVE DATA`, `EVE NCORE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1 - Done (2026-10-27)"
```
Then the Aisha precedent: `python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH1 review (2026-10-21, Opus): PASS, no code change"`.

## Task

Review CH1 final against its Done gate and the Aisha CH1 review's checks:
- **Deploy unit:** data036 `Eve_Nasod.lua` equals the `june_adopt_20261027_100517.tsv` ljconv row (size, sha1, header `1B 4B 4C 81`); `kom.py status data036` in sync with live; no `Eve_Nasod.src.lua` sidecar. The deployed exe holds `EVE DATA` once.
- **`EVE DATA` lines:** state count per class against `ch1m_reg_by_class.tsv` (class gap equal, the constant offset explained as in Aisha's review), `wait_state_change 1`, `MOTION_EVE*.X`.
- **The START fix** (`ParseCommonRandomState` -> `JuneParseCommonRandomState`): the 7 June table names = `JuneGetUnitClassName`'s Eve names; the rolls pick real tables (no `list size 0` / `re-bind skipped` for `ENSI_`).
- **ErrorLog** against `data\ErrorLog_pre_CH1_eve.txt`: no new `KEM_ERROR` / `Motion Error` / missing Eve animation; anything new is routed.
- **Nasod Core** (`EVE NCORE`) lines from the play-test agree with CH1q's play-test items. (CH1s's `EVE NCORE ball` line was retired in the CH1s review; its PASS is the user's play-test 2.)
- **CH1s's data fix** is part of the deploy unit: data034 `DLG_PVP_Game_My_State_NEW.lua` = `june_adopt_20261027_123538.tsv` (sha1 `28b6cbc7`, `1B 4B 4C 81`), data057 Texture80 staged; `kom.py status data034 data057` in sync.
- The six `(Eve: CH1 ...)` checklist marks and the §4 verdict.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_LIVE --range <CH1 commit>^..<CH1 commit>`; `enumscan --check X2Lib/X2GuEve.cpp`.

Spot-check 2-3 items directly in Ghidra (read-only). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1 - Review" into the plan doc, update the CH1 STATUS.md row, check `kickoffs/char-eve/CH2a.md` still matches, and commit.
