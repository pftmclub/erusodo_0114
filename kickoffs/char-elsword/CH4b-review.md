# June character Elsword (Stage 7) - Phase CH4b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH4b review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH4b done and play-tested (its Done note, commit and the user's "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE`; `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE_DEBUG` (re-opened by CH4b; this review retires it again with `//#define`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD PASSIVE infinity sword open mind`, `fight god mp`, `active cool time`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH4b - Done (2026-11-05)"
```
Then the four `(CH4b)` rows at the end of `june2017_re/funcmap.tsv`, `JUNE_BUGS.md` row 534, and the listings
(`python june2017_re/charport_adis.py elsword 32d1e0e0 32cf4b70 32cf4df0`; `python june2017_re/funcdis.py --func 322b5320`;
`python june2017_re/disasm_range.py 32028af0 32028ba0`).

## Task

Check CH4b against June (read-only in Ghidra and the listing tools). In order of risk:
- **`SetDamageExceptionProcess` `32d1e0e0`**: base first; `31fceab0( 21419 ) > 0` and `+0x53c == 0xf` -> `+0x510` = 1.0 (`+0x510` is `m_fCriticalRate`, the
  `CRITICAL_RATE` key: funcmap row 178 has it at `+0x200 + 0x510` in the effect); the 21420 branch: vslot `0x238` `( 21420, 4, 1 )`, then `322b5320( factor, level, 1, &rate )`
  with rate = `{ 1.0, 1.0, false }` (stack offsets `@32d1e172`-`32d1e1b6`), so no memo rescale; the port's `PushBuffFactor( GetClonePtr( level ), true )`, and the null
  factor still clearing and re-adding the socket factors as `322b5320` does. Confirm JUNE_BUGS 534 (trigger 16 set by no June data or code; 21420 has four factors).
- **`GetActualMPConsume` `32cf4b70`**: the switch tables (`32cf4c10` / `32cf4c1c`: 21008 / 21044 -> case 0, 21201 / 21218 -> case 1), `HaveThisBuff( 18 )` before the level
  test, `DE E1` = `1 - SA_MP_CONSUMPTION`, the double 10.0 at `33438088`, no clamp; March's memo 19 and `SERV_BALANCE_FINALITY_SKILL_EVENT` rate dropped.
- **`GetActualCoolTime` `32cf4df0`**: templet `+4 == 6` (`ST_ACTIVE`), `+0x96e0` = `m_fIncreaseCoolTimeRel` (its only June writer is the ctor `@32d09d7c`), the
  `test ah,5; jp` clamp (< 0 -> 0).
- **The four Way of the Sword sites** (funcmap row "320688a0 / 31ff4642 / 31ffe262 / 322fd29d"): spot-check one; the June user state ids are March's + 1.
- **`#else` paths**: flag off, March's `GetActualMPConsume` body is untouched and the two new overrides vanish; `DTT_EIS_INFINITY_CHASER = 16` under `JUNE_ENUM_NAMES`.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum
(June's `Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py JUNE_CHAR_ELSWORD_PASSIVE --with JUNE_CHAR_ELSWORD_PASSIVE_DEBUG --range <CH4b commit>^..<CH4b commit> --files KTDXLIB/Always.h X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h X2Lib/X2DamageManager.h`;
`python tools/junediverge.py --check-all` = `RESULT: PASS`; `python tools/enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp` = 0 A/B rows.

You are on Opus: delegate the reads, greps and known-address dumps. Fix what is wrong ( CP949 through `bytepatch.py` ); retire `_PASSIVE_DEBUG` ( the CLAUDE.md build
exception: the only change is commenting out the `_PASSIVE_DEBUG` define, then start the build and do not wait); hand design decisions back to the user. Check the play-test logs
( `python tools/logs.py --grep "JUNE_CHAR ELSWORD PASSIVE"` ) against the hand-off: `fight god mp` 250 -> 200 / 60 -> 50, `active cool time` x 1.000, `infinity sword open mind`
on Conwell combos. Write "CH4b - Review" into the plan doc, update the STATUS row, check the CH4c kickoff, and commit with explicit pathspecs.
