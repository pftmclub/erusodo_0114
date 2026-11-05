# June character Elsword (Stage 7) - Phase CH4c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH4c review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH4c done and play-tested (its Done note, commit and the user's "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE`; `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE_DEBUG` (re-opened by CH4c; this review retires it again with `//#define`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD PASSIVE magic coating`, `magic reflect`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH4c - Done (2026-11-05)"
```
Then the four `(CH4c)` rows at the end of `june2017_re/funcmap.tsv`, `tools/data/june_march_divergent.tsv` row 220, and the listings
(`python june2017_re/charport_adis.py elsword 32d0c820 32d1b650 32cf4d00`; `python june2017_re/disasm_range.py 320469c0 32046b40`).

## Task

Check CH4c against June (read-only in Ghidra and the listing tools). In order of risk:
- **The reflect branch `320462a0` @`32046a46`-`32046aa0`**: `m_pUnit` and class `+0x38 == 0x65`; DD `+0x447 == 1` tested before `32d0c820`; true -> `fldz; fstp [DD+0x3c]`
  (= `damage.fMagic`; `GetAllDamage` = `+0x3c` + `+0x38` at `@32046b09`) and no roll; else roll (vslot `0x1cc( 0 )`) then `32d1b650`, `fcomp; test ah,1` = roll <= rate.
  The port sits inside March's `NEW_SKILL_2010_11` block with March's roll in `#else`; the `RT_REFLECT` / `RT_REVENGE` choice after it is March's (`@32046afc`-`32046b24`).
- **`32d0c820` = `ApplyMagicCoating`**: the gate order (`HaveThisBuff( 18 )`, `GetSkillTemplet( 21418, 1 )`, unit, unit `+0x3c`, templet `+0x174 == 5`,
  `GetSkillLevel( 21418, 0, 0, 1 ) > 0`), `32206e50( 0x2b, level, 0, 0.f )` = `SA_MP_REGENERATION_ABS` (43), vslot `0xf0` = `UpNowMp`, `322d38f0( v, GetPos, GetDirVector )`
  (argument order from the pushes `@32d0c8eb`-`32d0c91f`).
- **`32d1b650` = March's `GetMagicReflectRate`** (`+0x95c0` x 3.0 in game type 2 / 3), and **`MemoConditionElsword` n/a** (`32cf4d00` = base `31fd4450` != 0).
- **`#else` / flag off**: March's roll untouched; `ApplyMagicCoating` and its declaration vanish.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum
(June's `Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py JUNE_CHAR_ELSWORD_PASSIVE --with JUNE_CHAR_ELSWORD_PASSIVE_DEBUG --range <CH4c commit>^..<CH4c commit> --files KTDXLIB/Always.h X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h X2Lib/X2GUUser.cpp`;
`python tools/junediverge.py --check-all` = `RESULT: PASS`; `python tools/enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp` = 0 A/B rows.

You are on Opus: delegate the reads, greps and known-address dumps. Fix what is wrong ( CP949 through `bytepatch.py` ); retire `_PASSIVE_DEBUG` ( the CLAUDE.md build
exception: the only change is commenting out the `_PASSIVE_DEBUG` define, then start the build and do not wait); hand design decisions back to the user. Check the play-test logs
( `python tools/logs.py --grep "ELSWORD PASSIVE"` ) against the hand-off: `magic coating: mp +5.0` with `magic damage ... -> 0` while Iron Body is up, `magic reflect` rolls
<= 0.360 in a dungeon without it. Write "CH4c - Review" into the plan doc, update the STATUS row, check the CH6 kickoff (set it ready), and commit with explicit pathspecs.
