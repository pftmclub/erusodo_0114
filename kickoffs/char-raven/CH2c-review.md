# June character Raven (Stage 7) - Phase CH2c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH2c review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH2c done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` + temporary `_SKILL_DEBUG` (the review retires it) |
| Log tag | `[JUNE_CHAR] RAVEN SKILL` |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH2c - Done (2026-11-06)"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH2b - Review (2026-11-06)"    # the worked example
```
Then `june2017_re/char_raven/ch0_skill_listing_rows.tsv` (the CH2c ids) and the Ghidra listings of the 23 CH2c addresses
(`cd june2017_re && python charport_adis.py raven --phase CH2c --out <dir>`; the registration is `charport_adis.py raven 32c641a0`).

## Task

Check CH2c against June (read-only in Ghidra; floats with `june2017_re/charport_consts.py --fn <va>`). In order of risk:
- **Registration.** The rows of `32c641a0` for the 12 ids, slot for slot: Charged Bolt Init + FM + EP (hyper FM + EP, same table, hyper tail `32c66d66`); Guardian Strike Init + StartFuture + FM +
  EP (hyper StartFuture + FM + EP); Valkyrie's Javelin Init + Start + FM + EP + End (hyper without Init) and the LANDING state 206 with EP only; Hell Dive Init + EP on both, the upgrade id on
  its own table `RSI_A_RRF_HELL_DIVE_UPGRADE`; Megadrill Break Init + FMF + FM + EP + End on both, the upgrade id's hyper through the shared tail `32c666a1` with the `_UPGRADE` table; Weapon
  Break Init + Start + EP (hyper Start + EP). Upgrade ids reach the same case (raw-id switch); the eleven new `RNSI_*` values against `Enum.src.lua`.
- **Megadrill Break FrameMove `32c5ce40`** (the riskiest): the DamageData offsets read as DD = unit + 0x3a0: `+0x4fa` = `m_bApplyExtraDamage` (SimpleInit `31f8ee50` sets 1), `+0xa4` =
  `m_ExtraDamage.m_Damage` (funcmap row 1348), `+0x50` = `m_ExtraDamage.m_ExtraDamageType` = 0x10 = `EDT_MANA_DAMAGE` (count the enum), `+0x510` = `m_fCriticalRate` (SimpleInit +0x510 = 0, the
  neighbours +0x50e `m_bApplyMotionOffset` and +0x51c `m_fRateModifier`); the three `SetLeftHandScale` vectors ( 3,3,3 ) / ( 1,1,1 ) / ( 1,1,1 ), the 2.15 s one-shot being a separate `if` after
  the else-if chain; the End `32c38710` order; FMF `32c38470`, EP `32c38510`.
- **Guardian Strike FM `32c30cb0`:** the 0.678 one-shot, the colour ( 0.37254903 = 0x5f / 255 -> `0xff5f0000` ), layer 0x10 = `XL_EFFECT_2`, `+0x144` alpha object, `+0x9774` = 1.4, and that
  nothing else (no cut-in, no particles, no damage effect) is left in June's FM.
- **Valkyrie FM `32c49730`:** the `PlayEffectSet` argument pushes (name, this, NULL, false, -1, -1, ( 1,1,1 ), false, zero vectors, 1.0 = defaults), the push-back into `+0x3eb0`, the 0.48 one-shot.
- **Hell Dive / Megadrill EP:** the foot test on the walk and dash cancels, `SKILL_CANCEL_AFTER` with the foot test and the hyper branch; Hell Dive keeps the `y -= 45` JUMP_DOWN shift.
- **Charged Bolt FM `32c48de0`:** no memo variant, `ShowActiveSkillCutInAndLight( ..., 0 )`; **Weapon Break** Start = `CommonStateStart` only; **= March** rows (11) and the preload lists
  (Guardian Strike 11, Weapon Break 11, Hell Dive 7, Megadrill 12) equal, name for name and in order.
- Play-test result and the `RAVEN SKILL` log lines (`equip skill`, `valkyrie javelin effect set played`, `guardian strike layer set`, `megadrill second hit mana damage`, `megadrill next damage data
  hand x3`), the `NUM use` lines for 24004-24006 / 24202-24204 (`python tools/logs.py --grep NUM`), then retire `_SKILL_DEBUG` (`//#define`).

Hygiene: `flagcheck.py JUNE_CHAR_RAVEN_SKILL --with JUNE_CHAR_RAVEN_SKILL_DEBUG --range <CH2c commit>^..<CH2c commit>`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`, one-file probe of `X2GURaven.cpp` (0 errors). No build if the review changes no code (a `//#define` retire is a not-awaited build).
Then check the CH2d kickoff (`kickoffs/char-raven/CH2d.md`).
