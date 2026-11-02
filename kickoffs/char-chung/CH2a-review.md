# June character Chung (Stage 7) - Phase CH2a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH2a review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH2a done (its Done note, commit and the user's play-test + "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`, retired by this review) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH2a - Done (2026-11-02)"
grep -a "Char Chung CH2a" june2017_re/funcmap.tsv
python june2017_re/charport_adis.py chung --phase CH2a --out <short scratch dir>
```

## Task

Check CH2a against June (read-only; read the listings, not the decompiles). In order of risk:
- **Lunatic Blow EXPLOSION FrameMove `32c11e10` and `PlayCannonShotEffectSet` `32c11410`:** the argument order of the call (needed 2, use 1, power, second-rate flag true,
  fallback name `L""` = `3344f1f4`), the loop test (`>= 2` -> addition explosion, no `UseCannon` in that branch; `< 2` -> plain explosion + `UseCannon( 1 )`), and that the final
  `PlayEffectSet` carries `rate x power` as `fPowerRate` (the `fmul [esp+0xd0]`). `CanConsumeCannonBallForShot` `32c0a230`: the two rate slots (`+0x9898` / `+0x989c`),
  the extra-balls-first order, the leftover-extra branch and that `32c24d40( n )` is `SetExtraCannonBallCount( GetExtraCannonBallCount() - n )`.
- **Aimming Shot `32bfa860`:** the extra-ball branch order (`extra >= cost` first, then `count < cost && !hyper` -> misfire, else `UseCannon`), `+0x98a8` set before the aiming test,
  the EXTRA explosion name and rate slot 0 (`@32bfae33`).
- **Scare Chase `32bfa670`:** `+0x970 / +0x974 / +0x978 / +0x97c` of the CEffect are the lock-on DUNGEON / PVP min / max (LoadLockOnData `@31f72375` / `@31f72385` / `@31f72407` / `@31f72412`),
  `32007b10( 0x1d, 0x6732 )` = `SA_SCOPE_ABS` (29) of `RNSI_P_CDC_ENDLESS_CHASE` (26418), and that June really has no `SetLockOn` call.
- **Siege Shelling:** `32beba80` (flag 0 = `m_fEventTime[0]` at `+0x8c10`, `+0x8c18` state change, `GetExtraCannonBallCount() > 0`), `32bebc40` (the 26419 test, flag order 1 / 2 / key X / key down / Z flag / cancels),
  `32be5a80` (`g_pMain +0x1a0 == 0` = `OL_HIGH`), `32c0e530` and `32c0e9b0` (bullet types 0 / 1, the extra ball per shell, the dead `GetCannonBallCount() > 0` test in `32c0e9b0` kept as June has it).
  The hyper table of 26201 and 26200 is the same script table as the normal one.
- **Back Blast:** `32bea9c0` cancels (`+0x8c8c` / `8c90` / `8c94` / `8c98` / `8c9c`), InitStateCommon registration of state 335 (`@1543`-`@1549`), `CSI_A_CC_BACK_BLAST` enumerator.
- **The "= March" rows** (funcmap `Char Chung CH2a`, 22 of 41): confirm 3 of them against the listing (e.g. `32bf2e10`, `32be0f50`, `32bd9510`).
- **The sound errors:** 63 `ErrorEnum(381) result : 47` lines in `Update3DPosition` for the seven `CHUNG_*.OGG` files named in the Done note's play-test paragraph. Find out why (file format vs the 3D path; compare a class without the error) and fix or route it.
- Dev free-skill gate: CH1l3 put it in `SpecialAttackLookup`; confirm the six skills cast with no cool time / MP under `/invin 0` + dev gate.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_CHUNG_SKILL --range <CH2a commit>^..<CH2a commit> --with JUNE_CHAR_CHUNG_SKILL_DEBUG`, `git diff --patience --numstat`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`, `ch1m_regcheck.py` (0 problems), one-file probe of `X2GUChung.cpp`. Retire `_SKILL_DEBUG` (comment out the define, note it in
the review). Write "CH2a - Review" into the plan doc, update the STATUS rows (CH2b0 unblocks), check `kickoffs/char-chung/CH2b0.md` and commit.
