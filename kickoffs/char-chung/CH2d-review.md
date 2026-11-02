# June character Chung (Stage 7) - Phase CH2d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH2d review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH2d done (its Done note, commit and the user's play-test + "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`, retired by this review) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH2d - Done (2026-11-02)"
grep -a "Char Chung CH2d" june2017_re/funcmap.tsv
python june2017_re/charport_adis.py chung --phase CH2d --out <short scratch dir>
```

## Task

Check CH2d against June (read-only; read the listings, not the decompiles). In order of risk:
- **The two new damage-effect helpers.** `32c116e0` (`CreateCannonShotDamageEffect`): the six stack arguments (name, needed, use, float rate, second-rate bool, no-shot name), `CanConsumeCannonBallForShot`'s third
  argument, `_EXTRA` only when the rate is above 1.0, the position/rotation from `TRACE_START0` after the consume, power x extra rate, the trailing `CreateInstance` arguments equal March's defaults. `32c11980`
  (`SetLockOnCannonShot`): needed 1, use 1, second rate true, empty no-shot name, `SetLockOn( effect, 0, 5, 0x1e )` = `LOT_NEARST_UID_VECTOR` with the default modulus 30. The port's empty-name guard (stand-in).
- **Chaos Cannon.** START `32c0f8c0`: the 0.315 s block (`+0x9734` roll: `jp` = random above the rate gives +1, else +2 -- read the `fcomp` / `test ah` pair), 26022 swap, `UseCannon( 1 )` before `+ 3.0`,
  the `+0x9894` reset only inside the templet branch, the unit-null `return`. `32c0fc30` (new `_01_FrameMove`): the `jnp` after `fcomp 3.0` = time <= 3, the `cmp [+0x9894], 8`, `CanConsumeCannonBallForShot( 2, 2, true )`,
  `+0x9860` = `m_fCannonBallDamageUp` x ( value + 1 ), the effect set's power-rate argument, the counter increment on every path. The EP/FM "= March" rows (`32c05c90`, `32c05e80`, `32c06050`, `32bec100`, `32be6b30`).
- **Dread Chase.** FM `32c12100` / `32c12240` / `32c12400`: the names (`_BIG_MEMO` / `_FLY_MEMO`), `count > 2`, the increment order. EP `32bedfc0` / `32bee020` use `32bed560` (= `GetTotalCannonBallCount`).
- **Impact Detonation.** LOOP FM `32c12660`: play count < 5, `>= 2` -> `PlayCannonShotEffectSet( ADDITION_LOOP, 2, 1, rate, false, "" )` with no `UseCannon`, else the plain set and `UseCannon( 1 )`; EP `32bed4c0` (the
  `+0x110` now count <= 1 and the extra count `+0x128`), READY EP `32bee0e0`, Init `32bdadc0`.
- **Elastic Bomb.** `32c0ede0` / `32c0f1f0`: the `0.3` shell roll (`jp` after `fcomp 0.3`), the four-bomb roll (`fcompp` then `test ah, 1` / `jne` = `rate >= roll`), skill `0x6664` = 26212 with memo swap, ability `0x1f` =
  `SA_PROJECTILE_RANGE_REL`, the three velocity/lifetime rows (0.6 / 1.75 / -1.0, 0.8 / 1.55 / -0.5, 0.4 / 1.95 / -1.5), the registration of `32c25870` for 26224 only. JUNE_BUGS 504.
- **Big Bang Stream `32c05060`:** `push eax; push edi` before vslot `0x228` = `SetBuffFactorToGameUnit( templet, 0 )`; both effect names at `GetPowerRate`; the 12-case switch = March.
- **Magnum Shot `32c10fb0`:** vslot `0x524` = `GetNowStateSkillID`, swap false, `UseCannon( templet + 0xc )`.
- **Registration:** the 12 labels in the six blocks (Impact/Magnum labels sit between the `case` line and `{`), the three Chaos Cannon attack tables get `_01_FrameMove`, `ch1m_regcheck.py` (0 problems).
- **The "= March" rows** (funcmap `Char Chung CH2d`): confirm 4 against the listing, e.g. `32be6b30`, `32be77c0`, `32c073e0`, `32bf35a0`.
- Dev free-skill gate: confirm all six skills cast with no cool time / MP under `/invin 0` + dev gate.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_CHUNG_SKILL --range <CH2d commit>^..<CH2d commit> --with JUNE_CHAR_CHUNG_SKILL_DEBUG`, `git diff --patience --numstat`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`. Retire `_SKILL_DEBUG` (comment out the define, note it in the review). Write "CH2d - Review" into the plan doc, update the STATUS rows
(CH3a and CH4a unblock) and commit.
