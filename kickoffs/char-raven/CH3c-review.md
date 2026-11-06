# June character Raven (Stage 7) - Phase CH3c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH3c review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH3c done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` + temporary `_SKILL_DEBUG` (the review retires it) |
| Log tag | `[JUNE_CHAR] RAVEN SKILL` |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH3c - Done (2026-11-06)"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH3b - Review (2026-11-06)"    # the worked example
```
Then `june2017_re/char_raven/ch0_skill_listing_rows.tsv` (the CH3c ids) and the Ghidra listings of the 34 CH3c addresses
(`cd june2017_re && python charport_adis.py raven --phase CH3c --out <dir>`; the registration is `charport_adis.py raven 32c641a0`).
The functions are un-carved: Ghidra has no function for them, the raw listing is the only source.

## Task

Check CH3c against June (read-only in Ghidra; floats with `june2017_re/charport_consts.py --fn <va>`). In order of risk:
- **Giga Prominence** ( five states, ids 24021 / 24052 ): LOOP EventProcess `32c5a6c0` ( loop count >= 8 then the key count test `>= 3` @32c5a7b7 -> OVER_FINISH 250 else FINISH 248; else LOOP 247 ), OVER_FINISH Start `32c536c0` ( `_ShowOverheatIcon( 0 )`, the inlined one-argument `_SetOverheat( true )`, CommonStateStart ), LOOP / FINISH / OVER_FINISH FrameMoves `32c4bcd0` / `32c4c070` / `32c60fc0` ( no memo branches; the effect-set and damage-effect argument lists ), the "= March" rows ( Init `32c271a0`, Start `32c53560`, FrameMove `32c3ea10`, EventProcess `32c5a5d0`, LOOP Start `32c42370`, the position helper `32c3e130`, FINISH / END / OVER_END EventProcess, OVER_END Start ).
- **Ground Impact** `32c4a080` ( skill 0x5e98 = 24216 with the swap flag; `GetSkillLevel( id, 0, 0, 1 )`; `SA_PROJECTILE_SIZE` ( 0x20 ); the effect set's life time -1.0 ( two floats pushed -1 ); Init `32c26bf0` two more textures ) and EventProcess `32c33100` ( = March ).
- **Breaking Fist** ( 24215 / 24238, table `RSI_A_RF_BREAKING_FIST[_UPGRADE]` ): no StateEnd and no hyper Init in the registration ( confirm @32c641a0 lines 1375 / 1525 ), EventProcess `32c35510` = March's `BALANCE_BLADE_MASTER_20130117` branch.
- **New states:** Hellfire `32c3b720`, Bursting Blade `32c3ba30`, Flame Sword: Flurry `32c3c360` ( all three = Pointed Stiletto's `32c3bd40` ), Ifrit Flame `32c3c930` ( animation end only ); the four empty Inits.
- **Registration** ( Giga Prominence / Ground Impact reuse March's cases with the RNSI labels added; Breaking Fist ( two ), Hellfire, Bursting Blade, Flame Sword: Flurry, Ifrit Flame are new cases; hyper = copy ) slot for slot against `ch0_skill_listing_rows.tsv`, and the nine new `RNSI_*` values against `Enum.src.lua`.
- JUNE_BUGS 554 ( the `__Raven_Hand_FireSM_Loop` particle ): confirm no sheet defines it.
- Play-test result and the `RAVEN SKILL` log lines ( `equip skill` for each id slotted, `giga prominence finish loops 8 key presses N`, `ground impact skill 24216 level L scale S`; units 82 / 83 ), `[JUNE_SKILL] NUM use` lines, then retire `_SKILL_DEBUG` ( `//#define` ).

Hygiene: `flagcheck.py JUNE_CHAR_RAVEN_SKILL --with JUNE_CHAR_RAVEN_SKILL_DEBUG --range <CH3c commit>^..<CH3c commit>`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`, one-file probe of `X2GURaven.cpp` ( 0 errors ). No build if the review changes no code ( a `//#define` retire is a not-awaited build ).
Then check the CH3c2 kickoff ( `kickoffs/char-raven/CH3c2.md` ).
