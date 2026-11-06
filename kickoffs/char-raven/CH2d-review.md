# June character Raven (Stage 7) - Phase CH2d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH2d review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH2d done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` + temporary `_SKILL_DEBUG` (the review retires it) |
| Log tag | `[JUNE_CHAR] RAVEN SKILL` |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH2d - Done (2026-11-06)"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH2c - Review (2026-11-06)"    # the worked example
```
Then `june2017_re/char_raven/ch0_skill_listing_rows.tsv` (the CH2d ids) and the Ghidra listings of the 41 CH2d addresses
(`cd june2017_re && python charport_adis.py raven --phase CH2d --out <dir>`; the registration is `charport_adis.py raven 32c641a0`).

## Task

Check CH2d against June (read-only in Ghidra; floats with `june2017_re/charport_consts.py --fn <va>`). In order of risk:
- **Hellfire Gatling LOOP FrameMove `32c60b30`** (the riskiest, a whole new body): the 0.001 s one-shot, the shout at loop 18 ( = MIN 17 + 1 ), the two `PlayEffectSet` calls: read the
  pushes against the declaration (`X2EffectSet.h:1083`: name, unit = this, trace target NULL, bHyper = the now-state byte +0x10 = `IsHyperState()`, power rate = `GetPowerRate()`, life -1,
  scale ( 1,1,1 ), custom position false, zero vectors, force-down rate 1.0), the overheat branch's temporary `RAVEN_HELLFIRE_GATLING_IMPACT_OVERHEAT` at ( 0, -1000, 0 ) with
  `_BurnOverheatHP` and `DestroyInstance`, and that no bullet effect and no 0.09 / 0.06 / 0.12 s timer is left.
- **Barrage Attack FrameMoves** `32c4b7d0` ( STAND, 0.4 s, offset 25, y +25 ) and `32c4b9e0` ( AIR, 0.1 s, offset 75, y -75, `m_fNowStatePowerRate` ( +0x7cc8 ) x 1.2 before the effect ): both push
  the string at `33512660` = `RAVEN_BARRAGE_ATTACK_STAND` ( JUNE_BUGS 549 ); no memo effect, no buff factor.
- **Registration.** The rows of `32c641a0` for the 12 ids, slot for slot: Barrage Init + StartFuture ( hyper StartFuture ), STAND FM + EP, AIR FM + EP; Revolver Cannon Init / Start / FM / EP / End
  on both, LOOP ( Start FM EP End ), FINISH ( LOOP's Start and FM, own EP ), END and OVER_END ( one EP ); Harpoon Spear five callbacks on both, FIRE ( EP ), FIRE_OVERHEAT ( Start FM EP );
  Hellfire Gatling five callbacks on both, LOOP ( Start FM EP End ), END / OVER_END ( one EP ); Napalm Grenade Init + FM + EP; Mental Crash Init + EP. The eight new `RNSI_*` values against `Enum.src.lua`.
- **Overheat key count 3** ( `32c5a020` @32c5a0e7, `32c5a1e0` @32c5a2ae, `32c5a400` @32c5a4c7; max loop 6 / 27 at @32c5a121 / @32c5a501 ) and `_SetOverheat( true )` inlined in `32c52cc0` / `32c52f30` /
  `32c532f0` ( `m_fOverheatBuffTime`, `CreateOverHeatObject`, the Awakened Will factor ); the cancels of `32c357e0` / `32c359d0` / `32c39c70` ( foot test on walk and dash, `SKILL_CANCEL_AFTER` ).
- **Revolver LOOP FM `32c60510`:** no memo x0.8, `GetPowerRate()` only; **= March** rows ( 26 ) and the preload lists equal.
- Play-test result and the `RAVEN SKILL` log lines ( `equip skill` for the six ids slotted, `barrage stand effect`, `barrage air effect power rate`, `hellfire loop effect set played loop N overheated 0/1`,
  `mental break end` ), then retire `_SKILL_DEBUG` ( `//#define` ).

Hygiene: `flagcheck.py JUNE_CHAR_RAVEN_SKILL --with JUNE_CHAR_RAVEN_SKILL_DEBUG --range <CH2d commit>^..<CH2d commit>`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`, one-file probe of `X2GURaven.cpp` ( 0 errors ). No build if the review changes no code ( a `//#define` retire is a not-awaited build ).
Then check the CH3a kickoff ( `kickoffs/char-raven/CH3a.md` ).
