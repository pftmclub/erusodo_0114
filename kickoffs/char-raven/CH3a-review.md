# June character Raven (Stage 7) - Phase CH3a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH3a review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH3a done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` + temporary `_SKILL_DEBUG` (the review retires it) |
| Log tag | `[JUNE_CHAR] RAVEN SKILL` |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH3a - Done (2026-11-06)"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH2d - Review (2026-11-06)"    # the worked example
```
Then `june2017_re/char_raven/ch0_skill_listing_rows.tsv` (the CH3a ids) and the Ghidra listings of the 34 CH3a addresses
(`cd june2017_re && python charport_adis.py raven --phase CH3a --out <dir>`; the registration is `charport_adis.py raven 32c641a0`).

## Task

Check CH3a against June (read-only in Ghidra; floats with `june2017_re/charport_consts.py --fn <va>`). In order of risk:
- **Giga Drive FrameMove `32c42010`:** `EventTimerOneshot( 1.1 )`, the buff factor of `GetEquippedActiveSkillTemplet( 24015, true )` ( no NULL test ), and with Counter Wave ( `RNSI_P_RBM_COUNTER` 24418, `GetSkillLevelValue` > 0 ) `m_fEmergencyEscapeElapsedTime = m_fEmergencyEscapeCoolTime - 1.0` ( the `fucompp` / `jnp` skip when equal ).
- **Shockwave - Front Start `32c38a30`** ( @32c38a8e tail: the 24418 templet with factor 0, then `GetNowStateSkillID()` upgrade swap -> base 24016 factor 0 ) and **EventProcess `32c38b20`** ( no cancels ).
- **Cut Tendon Start `32c5c4a0`** ( memo test = the always-false stub, factor [0] with the memo rate, `size() > 1` ) and **Bloody Accel FrameMove `32c49930`** ( templet 24014 factor 0, the update block of the never-created handle, JUNE_BUGS 550 ).
- **Wolf Fang** `32c33420` ( no hyper ids, 0xdb / 0xd9 ) and `32c3dce0` ( `GetNowStateSkillID()` slot MP refund x 0.5, DASH 0x13 / JUMP_DOWN 0x11 and y - 45, hit 0xdb ).
- **The four new states:** EventProcess `32c3ad00` ( Deadly Blow ), `32c3b040` ( Shoot Up Genocide: USEFUL_VALUE0 [0][0] at +0x8ef4, no cancels ), `32c3bd40` ( Pointed Stiletto ), `32c3c8a0` ( Sharp Charge ); the cancel order ( skill +0x8c90, back-walk +0x8c94, walk +0x8c98, dash +0x8c9c, attack +0x8c8c ) and the empty Inits.
- **Registration** ( `32c641a0` rows for the 16 ids, slot for slot; the four new skills' hyper state is `CopyStateCallbacks` ( June `329cc2c0` through the shared tail @32c668b1 ); Sonic Slash base / upgrade use two table names and share the FINISH state 0x136 ) and the 14 new `RNSI_*` values against `Enum.src.lua`.
- **"= March" rows** ( 12: Wolf Fang Init / Start / FM / HIT / HITLANDING, Cut Tendon EP, Sonic Slash Start / FM / LOOP EP / FINISH EP, Shockwave Front FM / End ) and the Init preload lists ( June's lists replace March's ).
- Play-test result and the `RAVEN SKILL` log lines ( `equip skill` for each id slotted, `cut tendon buff factor`, `bloody accel buff factor`, `giga drive counter escape cool time`, `shockwave front start`, `wolf fang mp refund` ), `[JUNE_SKILL] NUM use` lines, then retire `_SKILL_DEBUG` ( `//#define` ).

Hygiene: `flagcheck.py JUNE_CHAR_RAVEN_SKILL --with JUNE_CHAR_RAVEN_SKILL_DEBUG --range <CH3a commit>^..<CH3a commit>`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`, one-file probe of `X2GURaven.cpp` ( 0 errors ). No build if the review changes no code ( a `//#define` retire is a not-awaited build ).
Then check the CH3b kickoff ( `kickoffs/char-raven/CH3b.md` ).
