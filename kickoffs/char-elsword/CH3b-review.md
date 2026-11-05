# June character Elsword (Stage 7) - Phase CH3b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH3b review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH3b done and play-tested (its Done note, commit and the user's "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL`, `_SKILL_DEBUG` (defined again by CH3b; this review retires it with `//#define`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH3b - Done (2026-11-05)"
```
Then the CH3b rows of `june2017_re/funcmap.tsv` (`grep "Char Elsword CH3b"`), `june2017_re/char_elsword/ch0_skill_listing_rows.tsv` (the 21015-21018 / 21039 / 21040 / 21049-21052 / 21059 /
21210 / 21211 / 21223 / 21231 / 21232 rows) and the listings (`python june2017_re/charport_adis.py elsword --phase CH3b --out <dir>`).

## Task

Check CH3b against June (read-only in Ghidra and `charport_adis.py`). In order of risk:
- **Registration `32d223e0` raw** (the listing tool loses register names): the Counter's ready state ( StartFuture `32cf4e50`, EventProcess `32d05500`; hyper copies both ), state 259 ( `0x103`,
  EventProcess `32cf4d70` ) and 261 ( `0x105`, `32d05b80` ), Sword Fire's landing state 192 ( `0xc0`: EventProcess `32cfad80`, End `32cf3ee0` ), Extreme Rune Buster ( Init `32cf4ff0`, Start
  `32d09310`, EventProcess `32d068d0`; hyper through `329cc2c0` ), Phoenix Talon ( normal and `_UPGRADE` tables; hyper Start / FrameMove / EventProcess ), Rune of Fire ( hyper FrameMove
  + EventProcess, no Init ), Splash Explosion ( Start + `31fcc9d0` ), Triple Saber / Overs Geyser / Enchant Break ( `31fcc740` ).
- **Counter Guard** `32cf4e50` ( hold time zeroed first, `Set( +0x88bc - 1 )`, `CommonStateStartFuture` ), `32cf4d70` ( `+0x96f4 += elapsed`, `FollowSlotSwap`, `IsKeyReleased` or `5.0 < +0x96f4`
  -> `0x105` ), `32d05500` ( animation end -> `0x103` ), `32d05b80` ( end -> `0x11` and `y -= 45.0` / `0xd`; up-down `+0x8ca4`; skill `+0x8c90` with the awakening branch; back-walk `+0x8c94`,
  walk `+0x8c98`, dash `+0x8c9c`, Z / X `+0x8c8c`, all foot-gated ).
- **Extreme Rune Buster** `32d09310` ( the two clears, ten `GetRandomFloat( i )` pushes, `CommonStateStart` ) and the claim that `32d068d0` equals `32d05b80`.
- **Rune of Fire** `32d18230` ( the modifier call `321db050( handle, 0x13, +0x96d4, 0 )` inside the instance test ) and **`ApplyDamageEffectModifiers` case 19** against `321db1b0 @321dbb95-321dbbec`
  ( `b5c` set before the main-effect test, `b60` and the life time only with a main effect ); `32cfff60` ( dash, walk, skill ); **Phoenix Talon** `32d09290` and **Sword Fire Landing End** `32cf3ee0`.
- **The "= March" claims** ( Sword Fire Init / EndFuture / EP, landing EP, Phoenix Talon Init / Start / EP, Luna Blade Init / EP, Rune of Fire Init, Splash Explosion Start ).
- **Members**: `m_fSkillKeyHoldTime` ( `+0x96f4` ) is the one member of the Mega Slash charge and the guard; the two vectors ( `+0x97c4` / `+0x97d4` ).
- **`#else` paths** ( flag-off must reduce to the base ).

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum
(June's `Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py JUNE_CHAR_ELSWORD_SKILL --with JUNE_CHAR_ELSWORD_SKILL_DEBUG --range <CH3b commit>^..<CH3b commit> --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h X2Lib/X2SkillTree.h X2Lib/X2EffectSet.cpp KTDXLIB/Always.h`;
`python tools/junediverge.py --check-all` ( RESULT: PASS ); `python tools/enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`; a one-file probe of `X2GUElsword_SwordMan.cpp` and `X2EffectSet.cpp`
( `/c /Zs /EHsc` ). Paste the summary lines into the review. Explain every OTHER row and every deleted line ( the member rename is seven ); a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`;
exception: the only change is commenting out the `_SKILL_DEBUG` define, then start the build and do not wait); hand design decisions back to the user. Check the play-test logs
first ( `python tools/logs.py --grep "ELSWORD SKILL"`; `NUM use` for 21223 ). Write "CH3b - Review" into the plan doc, update the CH3b row in STATUS.md, check
`kickoffs/char-elsword/CH3c.md`, and commit with explicit pathspecs.
