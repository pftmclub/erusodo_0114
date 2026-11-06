# June character Raven (Stage 7) - Phase CH3b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH3b review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH3b done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` + temporary `_SKILL_DEBUG` (the review retires it) |
| Log tag | `[JUNE_CHAR] RAVEN SKILL` |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH3b - Done (2026-11-06)"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH3a - Review (2026-11-06)"    # the worked example
```
Then `june2017_re/char_raven/ch0_skill_listing_rows.tsv` (the CH3b ids) and the Ghidra listings of the 39 CH3b addresses
(`cd june2017_re && python charport_adis.py raven --phase CH3b --out <dir>`; the registration is `charport_adis.py raven 32c641a0`).
The 39 functions are un-carved: Ghidra has no function for them, the raw listing is the only source.

## Task

Check CH3b against June (read-only in Ghidra; floats with `june2017_re/charport_consts.py --fn <va>`). In order of risk:
- **Wild Charge** ( the state data object and three states ): registration `32c641a0` @32c64d98-@32c64fba ( `WildChargeData` 0x24 B `32c69e30`: refcount, three floats, slot id `+0x10`, slot B `+0x14`, key pointer `+0x18`, two effect-set handles `+0x1c` / `+0x20`; `SA_EFFECTIVE_TIME` -> `+8`, `SA_ATK_REL` -> `+0xc` from `GetUserSkillTemplet( id, true )` at the learned level ); READY Start `32c33830` ( timer `+4` = 0 ), StartFuture `32c33850` ( = March ), FrameMove `32c4a2a0` ( 0.3 s ready effect set + the `+0x3f00` skill-end pair = `AddEffectSetDeleteCondition_LUA( h, DCES_SKILL_STATE_END )` ), CHARGING FrameMove `32c4a490` / EventProcess `32c33960` / End `32c4a670`, DASH FrameMove `32c4a6c0` ( 0.01 s attack effect set, `+0x20` handle, `+0x8e64` = 8 and layer 0x10; 0.35 s stop; 0.8 s the four-field reset `31fc9570` + layer 0xb + colour 1 ) / End `32c339a0`.
- **Arc Enemy FrameMove `32c560e0`** ( `GetEquippedActiveSkillTemplet( GetNowStateSkillID(), true )` + `+0x178` != 0 -> offset -0.3 and five spears; the effect name is one string; the fifth offset ( -66.6, 318, 21 ) rot z -79; no `m_TimerFlyingImpact.restart`; float steps are `float( offset + <double> )` ).
- **X Crash** `32c26c90` / `32c331a0` ( future-animation timer, flag +0x978c never set, JUNE_BUGS 551 ) and Init `32c26c50`.
- **Limit Crusher** EventProcess `32c38f70` ( anim end or 0.45 s + key release -> state `0x120` or, with an upgrade-delete id and the animation not ended, `0x13e`; else JUMP_DOWN; walk / dash cancels with the foot test, then the skill cancel ), `32c391f0` / `32c393f0` ( twins ), the new enum member ( C++ value 272, June script id 318 ) and `SetEquippedSkillLevel` ( June sets only states 0x120 / 0x121 ).
- **New states:** Enchant Hand `32c278d0` / `32c395f0`, Maximum Dive `32c3b0a0`, Power Bomb `32c3b3e0` ( both = Deadly Blow's EventProcess ), Spread `32c3c050` ( = Pointed Stiletto's ), Devastating Strike `32c3c8e0` / `32c3c8f0` ( the End writes `+0x41dc`: **named `m_fNowSlashTraceTipWide` only by its 4.0 default** ( X2GUUser.cpp:918 ); confirm in Ghidra ).
- **Registration** ( 12 case groups, 16 ids, slot for slot against `ch0_skill_listing_rows.tsv`; Nuclear / Arc Enemy hyper = the normal functions; hyper of Maximum Dive / Power Bomb / Devastating Strike / Spread = `CopyStateCallbacks` ) and the 13 new `RNSI_*` values against `Enum.src.lua`.
- **"= March" rows** ( 12: Arc Enemy Init / EventProcess, Nuclear Init / FrameMove / EventProcess, X Crash EventProcess, Wild Charge READY StartFuture / EventProcess and DASH EventProcess, Limit Crusher Start / StartFuture / End ) and the Init preload lists ( June's lists replace March's ).
- `skillicons.py check` 2 misses = Enchant Hand picture ( June's own gap, JUNE_BUGS 552 ): confirm June's `.TET` really lacks the piece ( `kom.py stage-june` says identical ).
- Play-test result and the `RAVEN SKILL` log lines ( `equip skill` for each id slotted, `wild charge skill` ( learned level, full charge time, power rate ), `wild charge dash power rate`, `arc enemy spears 4 / 5`, `x crash mp refund`, units 80 / 81 ), `[JUNE_SKILL] NUM use` lines, then retire `_SKILL_DEBUG` ( `//#define` ).

Hygiene: `flagcheck.py JUNE_CHAR_RAVEN_SKILL --with JUNE_CHAR_RAVEN_SKILL_DEBUG --range <CH3b commit>^..<CH3b commit>`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`, one-file probe of `X2GURaven.cpp` ( 0 errors ). No build if the review changes no code ( a `//#define` retire is a not-awaited build ).
Then check the CH3c kickoff ( `kickoffs/char-raven/CH3c.md` ).
