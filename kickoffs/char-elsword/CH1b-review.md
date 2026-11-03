# June character Elsword (Stage 7) - Phase CH1b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1b review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1b done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (commented out until CH1 final; `_CORE` is defined by the first phase with a `new` row) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1b - Done (2026-11-04)"
awk -F'\t' '$7=="CH1b"' june2017_re/char_elsword/ch0_class_map.tsv
```

## Task

Review the 35 CH1b functions against June (`python june2017_re/charport_adis.py elsword --phase CH1b --out <dir>`; the listing, not the decompile, carries the `StateChange` arguments):
- The real differences from March: the no-down stand-up attack is `0 < +0x9684` with no roll (DamageDownFront `32cf5ad0`, DamageDownBack `32cf5c90`, AirDownLanding `32cf5e10`); Revenge Attack Start `32d076f0` (Lord Knight counter ids 21206 / 21228, the 15 MP refund, `31fce5c0` inlined, the multiplied `fPhysic`, no memo 5 branch, the float / double widths of `GetSkillPowerRateValue * ( GetSkillAdditionalPowerRate + 1.0 )` and `SA_EFFECTIVE_RATE * 100.0`); Revenge FrameMoveFuture.
- Confirm `+0x9684` is a float member only the passive state writes (`32d1c560` @`32d1c685` / `@32d1c785`) and the ctor zeroes (`32d09490` @`32d0b2c7`); confirm `0x52d6` / `0x52ec` = `RNSI_A_ELK_COUNTER` / `_UPGRADE` and `SA` type 5 = `SA_EFFECTIVE_RATE`.
- Spot-check 3 of the "= March" functions end to end (suggest `32d07090` WinFrameMove, `32d0edb0` CameraMove, `32cf5c90` DamageDownBack with its `0.66` double).
- Die Start: `DyingStart` = vslot 0x338; `GetWayOfSwordState` = vslot 0x674; `32d0cd60` = Vigor, `32d0d240` = Destruction.
- `june2017_re/char_elsword/ch1_offsets.tsv`: confirm or correct the 6 rows added by CH1b.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE --range <CH1b commit>^..<CH1b commit>` (FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with `/D SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1b - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH1c.md`, commit with explicit pathspecs.
