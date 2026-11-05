# June character Elsword (Stage 7) - Phase CH2d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH2d review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH2d done and play-tested (its Done note, commit and the user's "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL`, `_SKILL_DEBUG` (defined again by CH2d; this review retires it with `//#define`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH2d - Done (2026-11-05)"
```
Then the CH2d rows of `june2017_re/funcmap.tsv` (`grep "Char Elsword CH2d"`), `june2017_re/char_elsword/ch0_skill_listing_rows.tsv` (the 21019-21021 / 21033 / 21036 / 21053 /
21212-21214 / 21221 / 21233 / 21234 rows) and `JUNE_BUGS.md` rows 526 and 532.

## Task

Check CH2d against June (read-only in Ghidra and `charport_adis.py elsword --phase CH2d`). In order of risk:
- **Sword Fall FrameMove `32d14100`** (the long one, 2285 instructions): M01 at 0.01 with the effect set and the alpha tag, then M02-M05 ( 0.03333 / 0.06666 / 0.1 / 0.15 ) into
  `+0x9650-+0x965c`, the four `_Memo` meshes ( 0.05 / 0.08 / 0.125 / 0.17 ) into `+0x9660-+0x966c`, **no memo test anywhere** ( confirm in the listing ), nine `32296ee0` sites;
  blade hits 1.0 / 1.16666 / 1.33333 / 1.5 ( handles +0x9650.. ) and 0.917 / 1.083 / 1.25 / 1.416 ( +0x9660.. ), all named `ELSWORD_SWORDFALL_BLADE1-4`, position = `Dummy01` + y 42
  +- ( -59 ) dir; the last hit's `GetEquippedActiveSkillTemplet( vslot 0x524, 1 )` +0x178 test and its two extra `BLADE4` at y + 80 and y + 160 ( the second `fadd 80` hits the same
  slot again ). **End `32d15f80`**: eight handles in order, then `CommonStateEnd`. Judge JUNE_BUGS 532.
- **`CreateCrescentCutEffect` `32d16c00`** + **`32d1b740`**: phase 2 / 3 / else names, the `32007c40` calls ( `(this, 1, id, 0, &v)` = `SA_EFFECTIVE_TIME`, `0x22` = `SA_MOVE_SPEED` ), life =
  main effect `+0x44c` ( `GetMaxLifeTime` ) x value, `ChangeMainEffectVelocity( 1, v )` only when the second ability exists ( the first call's value is reused as the buffer, never
  reset ), and the else-if chain in `32d1b740`.
- **Sword Shield `32d189a0` / `32d18a60`**: templet 21212 with swap 1, `GetSkillLevel( id, 0, 0, 1 )` twice ( identical arguments ), `GetSkillAbilityValue( 1, level, 0, 0.0f )`
  at `+0x96ec`, no unit -> return before `CommonStateStart`; the two effect-set names ( `33523da8` / `33523d6c` ).
- **The six EventProcesses** ( `32cfde50` `32cfdee0` `32cfdf70` = walk only; `32cfe840` walk, dash; `32d005e0` `32d007e0` dash, walk; all end with the skill cancel where listed ).
- **The "= March" claims** ( five Inits, `32d162d0`, `32d1f3a0` ): bodies and constants against March.
- **Registration**: the eight cases against the listing rows ( 21036 uses `32d1b740`, 21020 `32d162d0`; Final Strike and its upgrade share Init / EP and have no FrameMove / End ),
  table names, hyper = what June fills.
- **`#else` paths** ( flag-off must reduce to the base ).

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum
(June's `Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.

Hygiene: `flagcheck JUNE_CHAR_ELSWORD_SKILL --with JUNE_CHAR_ELSWORD_SKILL_DEBUG --range <CH2d commit>^..<CH2d commit> --files` the four code files,
`git diff --patience --numstat`, `junediverge.py --check-all`, `enumscan.py --check` the `.cpp`, a one-file probe of `X2GUElsword_SwordMan.cpp`. Retire `_SKILL_DEBUG`
(`//#define`, build started and not awaited per the standing exception).

Spot-check 2-3 items directly in Ghidra; if one fails, re-check the whole phase. Fix clear transcription errors under the same flag; hand design decisions back to the user.
Write "CH2d - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH3a.md`, and commit.
