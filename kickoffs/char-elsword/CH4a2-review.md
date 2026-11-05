# June character Elsword (Stage 7) - Phase CH4a2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH4a2 review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH4a2 done and play-tested (its Done note, commit and the user's "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_USER_CREATE_EFFECT_SET`; `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE_DEBUG` (re-opened by CH4a2; this review retires it again with `//#define`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD PASSIVE create effect set`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH4a2 - Done (2026-11-05)"
```
Then the three `(CH4a2)` rows at the end of `june2017_re/funcmap.tsv`, and the listings (`python june2017_re/disasm_range.py <a> <b> --x87`;
Ghidra cannot decompile `32063650`): `32060680`-`32060750` (the parse), `32063a40`-`32063d20` (the play), `32610cb0`-`32610d50`.

## Task

Check CH4a2 against June (read-only in Ghidra and the listing tools). In order of risk:
- **The play** `32063650` @`32063a6c`-`32063d06`: the bonus order ( `HaveThisBuff( 18 )`, `GetSkillTemplet( 21416, 1 )`, `32205590` = `+0x174 == 5`, `+0x3c == 1`,
  `GetSkillLevel( 21416, 1, 0, 1 ) > 0` ); the `fcompp` sense ( plays when roll < rate + bonus ); `31e4c230` = `EventTimerOneshot` on `32071d80`'s anim; the
  `PlayEffectSet` `321e1e60` arguments by stack offset ( bHyper = the now state's `m_bHyperState` from `[E+0x34]`, power rate vslot `0x204`, life -1, scale `+0xe0c` x3,
  custom position false, `GetPos( false )` vslot `0x138`, rotation vslot `0x150` with **z** += `+0x8e4c`, direction 0, force-down 1.0 ); the March placement ( after the
  buff resync, before the `RIDING_SYSTEM` riding-wait block that June runs at `@32063d06` ).
- **The parse** `32057b80` @`320606b5`-`32060736`: name cleared first, the three misses ( L"", 0, 0 ), time / [3] kept when the key is absent, placement after the
  skill-state-end stops and before `SPECIAL_OPTION_VALUE`.
- **`32610cb0`**: confirm no caller ( calls, jumps, absolute pointers, an inlined `0x53a9` test ) before accepting "dead, not ported".
- **`#else` paths** ( flag-off reduces to the base: `m_fCreateEffectSetRate` back under `_PASSIVE` only ), the ctor stand-in ( June leaves `+0x8e48` / `+0x8e4c` unset ),
  and the names `RNSI_P_ELK_FIGHT_GOD` 21416 / `BTI_SI_A_ES_ENDURANCE` 18 ( June's `Enum.src.lua` values ).

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum
(June's `Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py JUNE_USER_CREATE_EFFECT_SET --with JUNE_CHAR_ELSWORD_PASSIVE_DEBUG --range <CH4a2 commit>^..<CH4a2 commit> --files KTDXLIB/Always.h X2Lib/X2GUUser.h X2Lib/X2GUUser.cpp X2Lib/X2SkillTree.h X2Lib/X2BuffFactor.h`;
`python tools/junediverge.py --check-all` = `RESULT: PASS`; `python tools/enumscan.py --check X2Lib/X2GUUser.cpp` = 0 A/B rows.

You are on Opus: delegate the reads, greps and known-address dumps. Fix what is wrong ( CP949 through `bytepatch.py` ); retire `_PASSIVE_DEBUG` ( the CLAUDE.md build
exception: the only change is commenting out the `_PASSIVE_DEBUG` define, then start the build and do not wait); hand design decisions back to the user. Check the play-test logs
( `python tools/logs.py --grep "JUNE_CHAR ELSWORD PASSIVE create effect set"` ) against the hand-off: played / missed counts near rate 0.20 without Iron Body and 0.40 with it.
Write "CH4a2 - Review" into the plan doc, update the STATUS row, check the CH4b and CH6 kickoffs, and commit with explicit pathspecs.
