# June character Raven (Stage 7) - Phase CH3d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH3d review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH3d done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` + temporary `_SKILL_DEBUG` (the review retires it); `SERV_IRUHADEV_JUNE_CHAR_RAVEN_TRANSCEND` |
| Log tag | `[JUNE_CHAR] RAVEN SKILL`, `[JUNE_TRANSCEND] IMAGE` |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH3d - Done (2026-11-07)"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH3d - Review"    # the worked example (transcendence rows)
```
Then the Ghidra listings of the 13 CH3d addresses (`cd june2017_re && python charport_adis.py raven --phase CH3d --out <dir>`) and of the base
setter `3200a590` (`python june2017_re/funcann.py 3200a590`).

## Task

Check CH3d against June (read-only in Ghidra). In order of risk:
- **`CX2GUUser::SetInvisibility`** (June `3200a590`, new in the base; Raven's March member is in `#else`): the two flags `+0x3e58 / +0x3e59`; the alpha
  `+0xc44` put back from `+0x8e98` only on `false`, then saved; `+0xcb4` (= `m_RenderParam.bAlphaTestEnable`, RenderParam at `+0xc10`, June's layout with the two
  ints at `+0x9c / +0xa0`) and `+0xc74` (`bAlphaBlend`); `+0x8e9c = 1`; skin anim `+0x164 = 1` (layer); **no `SetAlphaObject`** (confirm nothing in the function
  writes `CKTDGObject`'s alpha-object byte); the equip loop (`+0x8834`, vslot `0x9c` `IsFullHyperMode`, unit type 7, equip `+0x1b0 == 0x15` = `EP_AC_LEG`,
  `3295cd30( !b, -1 )` read as `CX2Eqip::SetShowAttachedParticle`); vslot `0x394( !b )` = `SetShowEnchantWeaponParticle`; gage alpha 0 / 255; the four
  boost handles `+0xcbc..+0xcc8` under `m_pGageData` remain time > 0. The ctor init (`32040e5c / 32040e62`: both 1.0). The new divergence row.
- **Extreme Blade** `32c4f0a0` (FrameMove) and `32c42eb0` (StateEnd): = March but the base setter; the one-shot test, scale 0.01, the effect set and its
  power-rate argument, `+350` y.
- **Transcendence rows:** `TranscendCheck` chain (Raven's guard after Elsword's, brace-less); `GetCharacterImageName` prefixes `B_MASTER` / `R_FIST` /
  `V_COMMANDER`, Texture99 for the large / gray pieces, `_STATE` gauge, against June's row pushes (`@333762c0` and the other 23; the subagent list is in the
  Done note); `LevelUpEvent.lua` TYPE4 against `june_src_fixed/data036_kom/LevelUpEvent.src.lua:509-527` line for line.
- The "= March" rows (11 functions) by a Sonnet subagent's comparison; spot-check two.
- Play-test result and log lines (`equip skill 2460x ... slot 4`, `invisibility 1` / `0`, `[JUNE_TRANSCEND] IMAGE class 106 / 107 / 115`), then retire
  `_SKILL_DEBUG` (`//#define`).

Hygiene: `flagcheck.py JUNE_CHAR_RAVEN_SKILL --with JUNE_CHAR_RAVEN_SKILL_DEBUG --range <CH3d commit>^..<CH3d commit> --files` the six code files,
`flagcheck.py JUNE_CHAR_RAVEN_TRANSCEND --range ... --files X2Lib/X2Data.cpp X2Lib/Offline/Handlers_Quest.cpp KTDXLIB/Always.h`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GUUser.cpp X2Lib/X2Data.cpp X2Lib/Offline/Handlers_Quest.cpp` (4 rows pre-existing: 104 / 105 / 113 and the `< 100` bound).
No build if the review changes no code (a `//#define` retire is a not-awaited build). Then check the CH4a kickoff (`kickoffs/char-raven/CH4a.md`).
