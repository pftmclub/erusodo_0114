# June character Elsword (Stage 7) - Phase CH3c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH3c review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH3c done and play-tested (its Done note, commit and the user's "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL`, `_SKILL_DEBUG` (defined again by CH3c; this review retires it with `//#define`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH3c - Done (2026-11-05)"
```
Then the CH3c rows of `june2017_re/funcmap.tsv` (`grep "Char Elsword CH3c"`), `june2017_re/char_elsword/ch0_skill_listing_rows.tsv` (the 21022-21025 / 21041 / 21042 / 21054-21057 / 21060 /
21215 / 21216 / 21224 / 21235 / 21236 rows) and the listings (`python june2017_re/charport_adis.py elsword --phase CH3c --out <dir>`).

## Task

Check CH3c against June (read-only in Ghidra and `charport_adis.py`). In order of risk:
- **Registration `32d223e0` raw** (the listing tool loses register names): the hyper copy tails for Phantom Sword / Rage Cutter / Sword Blasting ( `32d22557`, `32d22d85`: FM + EP ( + End ), no Init ),
  Maelstrom Rage ( StartFuture + FM + EP + EndFuture, no Init ), Harsh Chaser ( Start + FM + EP + End; pull state `0xdb` EP only ), Compact Counter ( all six delegates ), Infinity Strike
  ( `329cc2c0` copies every delegate, Init included ), Infinite Chaser / Blade Circle / Another Sword ( `329cbfc0` -> `31fcc740` ), and the table names ( `ESSI_SI_SA_EIS_SWORD_BLASTING_UPGRADE` for 21057 only ).
- **Maelstrom Rage FM `32d13e20`** and **`ApplyDamageEffectModifiers` case 9** against `321db1b0 @321db77e-321db7cd` ( jump table `321dbc60` slot 8: `+0x5f0` = DamageData `+0x3f0` FORCE_DOWN, add or
  multiply by the entry's b, NEXT tables take the result ); the `-2.0` / tag `9` arguments and the `IsNowStateSkillUpgraded` gate ( vslot `0x524`, `+0x178` ).
- **Rage Cutter FM `32d1f470`** against `322b5320` ( clear = 1, level, Rate of `31fdbfc0` ) and the new `CX2GUUser::PushBuffFactorWithMemoRate`.
- **Compact Counter** `32d089b0` / `32d1ecc0` / `32cfdb30` ( the 21216 swap, `GetSkillTemplet( id, 1 )` read as the one-argument form, the sheath tag `32296ee0 @32d1efb2`, ground-only walk cancel ),
  **Phantom Sword** `32d170e0` ( tag `@32d17216`, buff factor order 0 then 1 ), **Sword Blasting FM** `32d177a0` ( the `_UpGrade` effect name; the rest = March ), **Harsh Chaser** `32cfd910` / `32d13940` / `32cfd990`.
- **The "= March" claims** ( Compact Counter StartFuture / FMF / End, Maelstrom Init / StartFuture / EndFuture, Sword Blasting EP, Harsh Chaser EP / End ) and the address-stripped equality `32d06c50` = `32d05b80`.
- **`#else` paths** ( flag-off must reduce to the base ).

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum
(June's `Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py JUNE_CHAR_ELSWORD_SKILL --with JUNE_CHAR_ELSWORD_SKILL_DEBUG --range <CH3c commit>^..<CH3c commit> --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h X2Lib/X2SkillTree.h X2Lib/X2EffectSet.cpp X2Lib/X2GUUser.cpp X2Lib/X2GUUser.h KTDXLIB/Always.h`;
`python tools/junediverge.py --check-all` ( RESULT: PASS ); `python tools/enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`; a one-file probe of `X2GUElsword_SwordMan.cpp`, `X2EffectSet.cpp` and
`X2GUUser.cpp` ( `/c /Zs /EHsc` ). Paste the summary lines into the review. Explain every OTHER row and every deleted line ( the `//#define` ); a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`;
exception: the only change is commenting out the `_SKILL_DEBUG` define, then start the build and do not wait); hand design decisions back to the user. Check the play-test logs
first ( `python tools/logs.py --grep "ELSWORD SKILL"`; `NUM use` for 21216 / 21025 / 21224 ). Write "CH3c - Review" into the plan doc, update the CH3c row in STATUS.md, check
`kickoffs/char-elsword/CH3d.md`, and commit with explicit pathspecs.
