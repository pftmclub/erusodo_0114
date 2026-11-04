# June character Elsword (Stage 7) - Phase CH1q review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1q review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1q done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_WOS` (`//#define` until CH1 final; `_LIVE` `#error`s without it), `_CORE`, `_WOS_DEBUG` (defined; its lines sit in `_WOS` code) |
| Log tag | none (nothing runs until `_LIVE` / `_WOS`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1q - Done (2026-11-04)"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1q - Review (2026-11-02)"   # Chung's review, the worked example
```
Then the CH1q rows of `june2017_re/funcmap.tsv` (`grep "Char Elsword CH1q"`) and of `tools/data/june_march_divergent.tsv` (`grep "CH1q"`).

## Task

Check CH1q against June (read-only in Ghidra and the exe bytes). In order of risk:
- **The attack-result gain** (`31f85910 @31f86bab-31f86ef0`, `32012b50 @32012d3b-32013088`) against `CX2GUUser::ApplyWayOfSwordPointGainRate` and the two
  call sites: the protected reads `+0x6c50` / `+0x6d04` are `GetCharacterSpecializationA` / `B` (result value at `+0xa0` of the `+0x6bb0` / `+0x6c64`
  members), `DE E1` = `1.0 - B`, the Madness test (skill `0x6f04` = `RNSI_P_SCA_MADNESS` 28420, SA `0x6a` = `SA_DECREASE_REL` from the name table at
  `3379e6b0`, HP > 0.0 at `3342e84c`, `v > HP / max` doubles), the MP factor still `CalcVigorMpUpOnAttack( type, state )`. Is a CX2GUUser helper right
  for a block June inlines twice? Is the Elesis stand-in (vslot `0x720`, base `3208e1b0` = 1.0) the right call?
- **CH0's replace row overturned**: `CalcWayOfSwordPointOnDamageReact` `326106c0` and `CalcWayOfSwordPoint` `326105c0` = March's live
  `BALANCE_PATCH_20120329` branch (re-run `june2017_re/flagprobe.py BALANCE_PATCH_20120329`); the row now says "no code" and the allow row is gone.
- **The "= March" claims**: `WayOfSwordStateChange` `32d0eeb0`, Start / End vigor / destruction except the one move-speed line, Get / Set state and point
  (`32cf5140` clamp), the sword-wait Start / StartFuture / animation, the destruction-rage state thunks and EP, `CreateGageData`, the damage-manager
  MP functions and the `3261aee0` loader (28 keys).
- **The ported bodies**: `GetFinalDamage_WayOfSword` `32d18cd0`, `GetDestRageWSPGain` `32622600` / `_Elesis` `32622660`, `SwordWaitEventProcess`
  `32d0f0c0` (state ids 0xa3 / 0xa8, vslot `0x7f4( 1 )`, `0x55c( 0x17 )`, `0x200`, `0x49`), the two alpha tags in `32d0f300`, the snapshot pair
  `32d058a0` / `32d058d0` (store width at `+0x92b8`, own-unit test, state not kept), JUNE_BUGS 527.
- **Routing**: the not-located `ch0_wos.tsv` rows the note sends to CH1q2 / CH4b / CH5 are each named in that kickoff.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.

Hygiene: `flagcheck` on `JUNE_CHAR_ELSWORD_WOS` (and `_CORE`, `_WOS_DEBUG`) with `--range <CH1q commit>^..<CH1q commit> --files` the eight code files,
`git diff --patience --numstat`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`, one-file probes of
`X2GUElsword_SwordMan.cpp`, `X2GUUser.cpp`, `X2DamageEffect.cpp`, `X2DamageManager.cpp` with no flag, with `_WOS`, and with `_LIVE` + `_WOS` (`_LIVE` alone must
stop at the new `#error`). Write "CH1q - Review" into the plan doc, update the STATUS rows (CH1q2 becomes ready), check
`kickoffs/char-elsword/CH1q2.md` and commit with explicit pathspecs.
