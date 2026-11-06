# June character Raven (Stage 7) - Phase CH3c2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH3c2 review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH3c2 done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` + temporary `_SKILL_DEBUG` (the review retires it) |
| Log tag | `[JUNE_CHAR] RAVEN SKILL` |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH3c2 - Done (2026-11-07)"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH3c - Review (2026-11-06)"    # the worked example
```
Then `june2017_re/char_raven/ch0_skill_listing_rows.tsv` (the six CH3c2 ids) and the Ghidra listings of the 45 CH3c2 addresses
(`cd june2017_re && python charport_adis.py raven --phase CH3c2 --out <dir>`; the registration is `charport_adis.py raven 32c641a0`).
The functions are un-carved: the raw listing is the only source, and it runs on past a function's last `ret` / `jmp` into the next one.

## Task

Check CH3c2 against June (read-only in Ghidra; floats with `june2017_re/charport_consts.py --fn <va>`). In order of risk:
- **Ignition Crow / Incineration EventProcess** `32c5b0a0` / `32c5b560`: the key count `cmp edi, 3` ( March 5 ), `_CanOverheat( 1 )`, `_ShowOverheatIcon( 0 )`; at the animation end `GetEquippedActiveSkillTemplet( vslot 0x524 = GetNowStateSkillID, 1 )` and the `+0x178` ( `m_iDeleteSkillIDByUpgrade` ) test choose 312 / 313 ( 315 / 316 ) else 267 / 268 ( 285 / 286 ); the shared overheat end states 269 / 287 ( `32c376f0` / `32c3a490` ) and the fire / overheat EventProcesses `32c374c0` / `32c376c0` / `32c3a260` / `32c3a460` ( byte-equal to the Deadly Raid finish EventProcess `32c36e80` apart from the state ids ).
- **Deadly Raid** ( 24024 / 24055, one set of states 260-266 ): EventProcesses `32c5bb40` / `32c5be30` / `32c5bf80` / `32c36e00` / `32c36e50` / `32c36e80` / `32c37080` / `32c37270` ( LOOP stage 0x104 / 0x105 / 0x106 / 0x107, FINISH 0x108 / 0x109, END 0x10a ), helpers `32c5af90` ( icon at 2 / 4 / 6 ) and `32c37470` ( inlined in the DR / LOOP_OVERHEAT EventProcesses ), the seven FrameMoves ( DEADLY_RAID / LOOP / LOOP_END / FINISH `32c4cba0` / `32c4cf30` / `32c4d350` / `32c4d770`; the three overheat ones `32c61fa0` / `32c623d0` / `32c62800` call the one-argument `_BurnOverheatHP` ) and the Starts ( `32c42690`, `32c427b0`, `32c42850`, `32c428f0`, overheat `32c538c0` / `32c53b10` / `32c53d60` with the inlined one-argument `_SetOverheat( true )` ).
- **Registration** ( `32c641a0` ): Deadly Raid one case for 24024 / 24055; Ignition Crow / Incineration base labels added, the two upgrade cases new ( `_UPGRADE` tables, March's callbacks, shared overheat end state, Incineration's hyper `m_bHyperState` ) slot for slot against `ch0_skill_listing_rows.tsv` ( rows 68-86, 101-108 ); the three new `RNSI_*` values against `Enum.src.lua` ( 24053 / 24054 / 24055 ).
- The "= March" rows ( 34 functions: the Inits, Ends, FrameMoves, Starts and EventProcesses `funcmap.tsv` marks ): spot-check a Sonnet subagent's comparison of five.
- Play-test result and the `RAVEN SKILL` log lines ( `ignition crow end skill S upgrade U overheat O`, `incineration end ...`, `deadly raid finish loop count N input count M overheat O` and the overheat variant; units 84 / 85 ), then retire `_SKILL_DEBUG` ( `//#define` ).

Hygiene: `flagcheck.py JUNE_CHAR_RAVEN_SKILL --with JUNE_CHAR_RAVEN_SKILL_DEBUG --range <CH3c2 commit>^..<CH3c2 commit>`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`, one-file probe of `X2GURaven.cpp` ( 0 errors ). No build if the review changes no code ( a `//#define` retire is a not-awaited build ).
Then check the CH3d kickoff ( `kickoffs/char-raven/CH3d.md` ).
