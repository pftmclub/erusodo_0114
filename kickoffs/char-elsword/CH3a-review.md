# June character Elsword (Stage 7) - Phase CH3a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH3a review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH3a done and play-tested (its Done note, commit and the user's "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL`, `_SKILL_DEBUG` (defined again by CH3a; this review retires it with `//#define`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH3a - Done (2026-11-05)"
```
Then the CH3a rows of `june2017_re/funcmap.tsv` (`grep "Char Elsword CH3a"`), `june2017_re/char_elsword/ch0_skill_listing_rows.tsv` (the 21008-21011 / 21037 / 21038 / 21044-21047 /
21058 / 21205 / 21206 / 21222 / 21227 / 21228 rows) and the listings (`python june2017_re/charport_adis.py elsword --phase CH3a --out <dir>`).

## Task

Check CH3a against June (read-only in Ghidra and `charport_adis.py`). In order of risk:
- **Windmill EventProcess `32d11fd0`**: END (191) after `fStateTime > duration + 2.8` ( float + double ), or past the duration when `m_kSkillSlotKey.IsKeyReleased` or the mana
  `elapsed x 100 / 2.8` is above `GetNowMp()`; the movement block runs only otherwise ( foot test around the up / down part only, the x speed always ). **FrameMoveFuture `32cf3e20`**
  ( `UpNowMp( -cost )` slot 0xf0 only when `cost <= mana` and past the duration ), **StateEnd `32d12210`** ( LAND_END_NEW ), **START FrameMove `32d11cb0`** ( `_NEW` names, `bHyper`
  false, `GetPowerRate` ), **FrameMove `32cfa9a0`** ( BTI 318, templet 21010 with the swap ), **END EventProcess `32cfaa30`** ( order: skill, back-walk, walk, dash, Z, X ).
- **Grand Cross**: the start / charge / end / full-charge functions and the helper **`32d19dd0`**. The helper's float slots are shared: the scale is `1 + 0.2 x steps` ( the store to
  `[esp+0x3c]` runs while the `GetPos` argument is still pushed, so it lands on the slot that held the step count ); the power rate is `GetPowerRate() x ( 1 + 0.1 x steps )`; the four
  offsets are `D`, `-D`, `( -Dz, Dy, Dx )`, `( Dz, -Dy, -Dx )` ( jump table `32d1a360` ); the main effect's `+0x5cc` is `CXMeshInstance` DirSpeed, scaled for effects 1-4 only.
  Judge whether the `CreateInstance` argument list ( 15 values ) matches `32d1a26b`.
- **Sonic Blade: Crescent EventProcess `32d05f00`**: the `[0][0]` USEFUL_VALUE gate and the `m_CommonState.m_Wait` change; order walk, dash, back-walk.
- **Armageddon Blade FrameMove `32cf9f60`**, **Rolling Smash StartFuture `32cfc150`**, **Sand Storm EventProcess `32cfc5d0`**, **Revenge Wait FrameMove `32d0eaf0`** ( no memo branch ).
- **The "= March" claims** ( Revenge Start / FMF / EP, Armageddon Init / EP, Windmill Init / Start / EP / End, Sonic Blade EP, Rolling Smash READY / EP, Sand Storm FMF ): bodies and constants against March.
- **Registration**: the cases against the listing rows ( table names, hyper = what June fills, states 190 / 191 / 202 / 204 / 263-265; Sonic Blade registers EventProcess only ).
- **`32cfc230`** is dead ( no reference ): confirm, so the 7ai row is n/a.
- **`#else` paths** ( flag-off must reduce to the base ).

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum
(June's `Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py JUNE_CHAR_ELSWORD_SKILL --with JUNE_CHAR_ELSWORD_SKILL_DEBUG --range <CH3a commit>^..<CH3a commit> --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h X2Lib/X2SkillTree.h KTDXLIB/Always.h`;
`python tools/junediverge.py --check-all` ( RESULT: PASS ); `python tools/enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`; a one-file probe of `X2GUElsword_SwordMan.cpp`
( `/c /Zs /EHsc`, with and without `_LIVE` ). Paste the summary lines into the review. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`;
exception: the only change is commenting out the `_SKILL_DEBUG` define, then start the build and do not wait); hand design decisions back to the user. Check the play-test logs
first ( `python tools/logs.py --grep "ELSWORD SKILL"`; `NUM use` for 21008 / 21205 / 21206 ). Write "CH3a - Review" into the plan doc, update the CH3a row in STATUS.md, check
`kickoffs/char-elsword/CH3b.md`, and commit with explicit pathspecs.
