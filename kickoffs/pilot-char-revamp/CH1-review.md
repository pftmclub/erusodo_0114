# June character Aisha (Stage 5 pilot) - Phase CH1 review (final sub-phase: LIVE on)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1 review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | Phase CH1 done (its Done note and commit) and, for the in-game legs, the user's CH1 play-test |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_LIVE` (now defined), on top of `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` |
| Log tag | `[JUNE_CHAR]` (`python tools/logs.py --tag JUNE_CHAR`), plus `--grep "JUNE_FX C3d-3"` and `--grep "JUNE_FX C9h-4g"` |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Phase CH1 - Deploy unit: June script, motions and the class core" "CH1 split (2026-10-21, user)" "CH1 - Done (2026-10-21, Opus)"
```

## Task

Review the final CH1 against the plan's Done gate:
- **The deploy unit:** `Always.h` defines LIVE; data036 `Arme_VioletMage.lua` is June's byte-exact (`june2017_re/june_adopt_20261021_100608.tsv`, `kom.py` vs-live line), and the deployed exe has LIVE (the `[JUNE_CHAR] ARME DATA` string is in `X2_offline.exe`).
- **The ARME DATA line:** one per Aisha unit, `wait_state_change 1` (June's table present), the motion name and the state count are plausible (compare with `ch1j_reg_by_class.tsv` for her class).
- **ErrorLog:** diff the play-test run's `ErrorLog.txt` against `data\ErrorLog_pre_CH1.txt`; no new `KEM_ERROR` / `Motion Error` / missing-animation line for Aisha. Route each new line (Aisha's: fix in this plan; not hers: its owner).
- **The START fix:** `CX2GUUser::JuneParseCommonRandomState` against `31feaac0` (and its call site in `3204ea20`, `31fdcc20` = `JuneGetUnitClassName`, `31fe6780` = March's base parse); check the other LIVE drops of CH1l ("March-only virtuals") for the same mistake, a job June moved into the base.
- **The two CH1j notes:** the empty `ANIM_NAME` of `AVSI_WAIT_STATE_CHANGE` (after WIN, back to wait at `USEFUL_VALUE0[0]`), and JUNE_BUGS 379 (no table: reads fall to defaults).
- **The scans:** `charport_keys.py Arme_VioletMage` and `charport_scan.py` results in the Done note show nothing new against CH0.
- If the play-test failed, find the cause from the logs (and the CrashRpt bundle in `%TEMP%` for a crash) before changing code.
