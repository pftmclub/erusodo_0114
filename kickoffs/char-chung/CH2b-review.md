# June character Chung (Stage 7) - Phase CH2b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH2b review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH2b done (its Done note, commit and the user's play-test + "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`, retired by this review) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH2b - Done (2026-11-02)"
grep -a "Char Chung CH2b" june2017_re/funcmap.tsv
python june2017_re/charport_adis.py chung --phase CH2b --out <short scratch dir>
```

## Task

Check CH2b against June (read-only; read the listings, not the decompiles). In order of risk:
- **Leap Attack landing `32be1760`:** the chain order (animation end, Paladin Z at class 110 `0x6e`, flag `[0]` + `m_fEventTime[0].stateChange` -> Back Blast, key window `keyInputStart <= t < keyInputEnd` +
  `IsNowStateSkillUpgraded` + `IsKeyPressedOnce`, then `SKILL_CANCEL_AFTER( 0.4 )`, walk, dash); that the Back Blast MP is `GetActualMPConsume( GetUpgradeSwappedSkillID( 26200 ), level ) x ( 1 - ability )`
  with the ability read from `GetNowStateSkillID()` (`32007c40`, swap 0) and the cast only when it exists and `cost <= GetNowMp()`; `UpNowMp( -cost )` after the `StateChange`. JUNE_BUGS 503.
  Check in June's `Chung_Iron_Cannon.src.lua` that `m_fEventTime[0]` is filled for the landing state (key names), and that the walk / dash cancels really carry the foot test (`+0x7e1c`).
- **Burst Wolf:** `32c007e0` (0.1 fade, else 0.33 lunge: 200 / -200 ahead, debuff 180 / -180 and +100 up, no bHit test, no `EffectSet_CSI_CIP_BURST_WOLF_FIRST / _SECOND` — grep the exe for other
  users of those names), `32be4e70` (`GetEquippedActiveSkillTemplet( 26003, true )`, ability type 2 = `SA_EFFECTIVE_COUNT`, the 4th stack argument of `32206e50`), `32be4f50` (skill / back walk / walk / dash, no attack cancel).
- **Brutal Swing `32bfa4a0`:** the `_MEMO` name and the rate (`vslot 0x204` = `GetPowerRate`); **Lunatic Fury END `32bfb8f0`:** the hyper test (`+0x7cbc` decode, byte `+0x10`) = `IsHyperState()`, power rate.
- **Registration:** the 12 labels, Leap Attack READY `StateStartFuture` on normal and hyper, the LANDING table by class (CIP for 110, no memo 7), Pandemonium without FrameMove (June `32c23170`); run
  `python june2017_re/char_chung/ch1m_regcheck.py` (0 problems).
- **The "= March" rows** (funcmap `Char Chung CH2b`, 32 of 39): confirm 4 of them against the listing, e.g. `32bee840` (all four damage-data timers and names), `32bfb6e0`, `32beb890`, `32c003c0`.
- Dev free-skill gate: `SpecialAttackLookup` (CH1l3) covers the six slot skills; confirm Burst Wolf / Brutal Swing / Lunatic Fury / Pandemonium / Leap Attack / Sudden Burster cast with no cool time / MP under `/invin 0` + dev gate.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_CHUNG_SKILL --range <CH2b commit>^..<CH2b commit> --with JUNE_CHAR_CHUNG_SKILL_DEBUG`, `git diff --patience --numstat`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`, one-file probe of `X2GUChung.cpp`. Retire `_SKILL_DEBUG` (comment out the define, note it in the review). Write "CH2b - Review" into the
plan doc, update the STATUS rows (CH2c unblocks with CH3b0's review) and commit.
