# June character Elsword (Stage 7) - Phase CH4a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH4a review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH4a done and play-tested (its Done note, commit and the user's "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE`, `_PASSIVE_DEBUG` (this review retires it with `//#define`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD PASSIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH4a - Done (2026-11-05)"
```
Then the three CH4a rows at the end of `tools/data/june_march_divergent.tsv`, JUNE_BUGS row 533, and the listings
(`python june2017_re/funcdis.py <va>` or the Ghidra skill) of `32d1c560`, `32d0c4e0`, `32d27190`, `32d0baf0`, `32d05a00`, `32d0bc60`, `32d05a50`,
`32d0c180`, `32d05af0`, `32d1a370`, and `charport_adis.py elsword 32d08ba0` (no Ghidra function there).

## Task

Check CH4a against June (read-only in Ghidra and the listing tools). In order of risk:
- **`InitPassiveSkillState` `32d1c560` block by block**: each `32890620` push ( `( id, 1, 0, 1 )`, but `0` for 21200 / 21407 / 21411 / 21404 ), each SA id
  ( 5 / 0xf / 0x10 / 0x1f / 0x20 / 0x22 / 0x53 / 0x67 / 0x70 / 0x71 / 0x94 ) -> member, every reset and its value ( -1 / 0 / 1 ), the class gates ( 10 / 100, 101, 22 / 112, 112 ),
  the order ( Harsh Slayer registers before its templet test ), Auto Guard's `+ 0.005` double and the x2 when the game is not PvP / PvE ( `@32d1cbd4` ), Rune Mastery's
  fireball at Spell Chain's level ( `[esp+0x20]` from `@32d1c832`, JUNE_BUGS 533 ), Cutting Sword's two clones ( `[0]` and `[1]`, `32205510` and type 13 ).
- **The eight callbacks**: list ( A / C / D ) and order of the pushes; Brutal Slayer's `cool <= elapsed`, combo > 0, count > 4, the Mind of Fighter cap; Sword Enchant's
  `GetRandomInt() % 3`; Lord Knight Open Mind's reset branch ( `322a7c30( 824 )`, `0 <= remain` without `HaveThisBuff( 824 )` ), the truncated threshold, the cool time
  store through `[esp+0x18]` = `+0x9788`; Extreme Rune Buster's UID dedupe, `count % size`, the 0.67 / 0.33 pick and the vslot `0x21c` argument order
  ( BFI, creator this, level `+0x97c0`, 0, 0 ); Lightning Step's 0.5 s.
- **The stand-ins**: Auto Guard with no game ( not doubled ), Extreme Rune Buster's empty random list ( return, June divides by zero ), the dropped write-only byte `+0x9794`.
- **`#else` paths** ( flag-off must reduce to the base ), and `m_fCreateEffectSetRate` in `CX2GUUser` ( June `+0x8e50`, its reader is CH4a2's ).

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum
(June's `Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py JUNE_CHAR_ELSWORD_PASSIVE --with JUNE_CHAR_ELSWORD_PASSIVE_DEBUG --range <CH4a commit>^..<CH4a commit> --files KTDXLIB/Always.h X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h X2Lib/X2GUUser.h X2Lib/X2GUUser.cpp X2Lib/X2SkillTree.h X2Lib/X2BuffFactor.h X2Lib/X2DamageManager.h`;
`python tools/junediverge.py --check-all` = `RESULT: PASS`; `python tools/enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp` = 0 A/B rows.

You are on Opus: delegate the reads, greps and known-address dumps. Fix what is wrong ( CP949 through `bytepatch.py` ); retire `_PASSIVE_DEBUG` ( the CLAUDE.md build
exception: the only change is commenting out the `_PASSIVE_DEBUG` define, then start the build and do not wait); hand design decisions back to the user. Check the play-test logs
( `python tools/logs.py --grep "JUNE_CHAR ELSWORD PASSIVE"` ) against the hand-off. Write "CH4a - Review" into the plan doc, update the STATUS row, check the CH4a2 and CH4b
kickoffs, and commit with explicit pathspecs.
