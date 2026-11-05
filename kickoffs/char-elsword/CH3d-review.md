# June character Elsword (Stage 7) - Phase CH3d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH3d review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH3d done and play-tested (its Done note, commit and the user's "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL`, new `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_TRANSCEND`, `_SKILL_DEBUG` (defined again by CH3d; this review retires it with `//#define`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD SKILL`), `[JUNE_TRANSCEND]` (`IMAGE`, `CHANGE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH3d - Done (2026-11-05)"
```
Then the CH3d rows of `june2017_re/funcmap.tsv` (`grep "Char Elsword CH3d"`) and the listings (`python june2017_re/funcann.py <va>` for the five; `charport_adis.py elsword 32d17ea0`).

## Task

Check CH3d against June (read-only in Ghidra and the listing tools). In order of risk:
- **EventProcess `32d00be0` / `32d00de0`**: the anim-end branch ( `0x11` / `0xd` ), the dash and walk cancels' `+0x7e1c` test and their order ( dash first ), June's `SKILL_CANCEL_AFTER`
  expansion, and the tail call ( vslot `0x7d4` ); the ported shape uses `ELSWORD_GROUND_DASH_CANCEL` / `_WALK_CANCEL` from CH1d.
- **The picture rows**: `GetCharacterImageName` prefixes `L_KNIGHT` / `R_SLAYER` / `I_SWORD`, large in Texture98, and the `_FACE` gauge suffix for 112 only, against June's row table
  ( string refs `@33370955`, `@33371f37`, `@33377182`; every image type, not only the three the phase cited ).
- **The allow-list** guard in `TranscendCheck` ( brace-less chain with Chung's ) and the `LevelUpEvent.lua` `TYPE3` row against June's `TYPE0` ( `LevelUpEvent.src.lua:453-471` ).
- **The "= March" claims**: the three Init preload lists, hyper effect start `32d17ea0` = CH1h's port.
- **`#else` paths** ( flag-off must reduce to the base ).

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum
(June's `Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py JUNE_CHAR_ELSWORD_TRANSCEND --range <CH3d commit>^..<CH3d commit> --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2Data.cpp X2Lib/Offline/Handlers_Quest.cpp KTDXLIB/Always.h`
and the same for `JUNE_CHAR_ELSWORD_SKILL --with JUNE_CHAR_ELSWORD_SKILL_DEBUG`; `python tools/junediverge.py --check-all` = `RESULT: PASS`.

You are on Opus: delegate the reads, greps and known-address dumps. Fix what is wrong ( CP949 through `bytepatch.py` ); retire `_SKILL_DEBUG` ( the CLAUDE.md build
exception: the only change is commenting out the `_SKILL_DEBUG` define, then start the build and do not wait); hand design decisions back to the user. Check the play-test logs
first ( `python tools/logs.py --grep "ELSWORD SKILL"`, `python tools/logs.py --grep "JUNE_TRANSCEND"` ). Write "CH3d - Review" into the plan doc, update the CH3d row in STATUS.md, check
`kickoffs/char-elsword/CH4a.md` ( CH4a waits on CH2d review, done ), and commit with explicit pathspecs.
