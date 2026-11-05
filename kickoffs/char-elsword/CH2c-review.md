# June character Elsword (Stage 7) - Phase CH2c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH2c review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH2c done and play-tested (its Done note, commit and the user's "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL`, `_SKILL_DEBUG` (defined again by CH2c; this review retires it with `//#define`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH2c - Done (2026-11-05)"
```
Then the CH2c rows of `june2017_re/funcmap.tsv` (`grep "Char Elsword CH2c"`), `june2017_re/char_elsword/ch0_skill_listing_rows.tsv` (the 21012-21014 / 21032 / 21035 /
21048 / 21207-21209 / 21220 / 21229 / 21230 rows) and `JUNE_BUGS.md` row 531.

## Task

Check CH2c against June (read-only in Ghidra and `charport_adis.py elsword --phase CH2c`). In order of risk:
- **`ESSI_SI1_ES_WIND_BLADE_UPGRADE_EventProcess` `32cfa080`** (new, the only long function): `FollowSlotSwap` first; end -> JUMP_DOWN / WAIT by foot; flag 0 and
  `m_fEventTime[0].stateChange < t` -> `GetNowStateSkillID` (vslot 0x524), `GetSkillLevel( id, 0, 0, 1 )` > 0, `GetActualMPConsume( id, level, 1 )` (`31fd1840`), ability type
  `0x1b` = `SA_MP_CONSUME_REL` via `32007c40` (swap 0), MP x ( 1 - rel ), `fMp <= GetNowMp()` -> `StateChange( 0x106 )` then vslot 0xf0 ( `UpNowMp( -mp )` ); else the window
  `keyInputStart <= t < keyInputEnd` with `31fc8900` ( `IsKeyPressedOnce` ) sets flag 0. A missing ability must skip the transition ( a log line says so under `_DEBUG` ).
  `StateStartFuture` `32cf3bd0` = `CommonStateStartFuture` + `Set( m_iNowSpecialAttack - 1 )`. The class map said "inside" March's EventProcess; CH2c made it a separate
  function because June registers two different functions for 21013 ( `32cfa040` = March ) and 21035 ( `32cfa080` ): confirm that reading.
- **Storm Blade FrameMove `32cfa270`**: one-shot 0.3, `IsNowStateSkillUpgraded()` twice around the 21014 templet lookup ( swap 1 ), vslot 0x228 = `SetBuffFactorToGameUnit`.
- **Big Burst FrameMove `32d123f0`**: 0.65 one-shot, `Dummy2_Lhand`, only the `DAMEGA_EFFECT_ELSWORD_MNK_BIG_BURST` damage effect with the default arguments; no memo branch.
- **Sword Wave / Critical Sword EventProcess** (`32cfef60` / `32cffd60`, instruction-identical): ground dash `+0x8c9c`, walk `+0x8c98`, `SKILL_CANCEL_AFTER` `+0x8c90`.
  **Critical Sword FrameMove `32cffcf0`**: 21209 with swap 1 at the 0.333 one-shot, buff-factor list non-empty. **Sword Wave Init `32cf45c0`**: ten preloads in order.
- **The "= March" claims** (`32cf3b90`, `32cf3c00`, `32cf3c70`, `32cf3f30`, `32cf4710`, `32cfa040`, `32cfa230`, `32cfa320`, `32cfae00`, `32d086f0`): bodies and constants against March
  ( Rising Slash Init's memo preloads are behind a stub that is false in both ).
- **Registration**: the nine cases against the listing rows ( including the 21035 case's state 262 with no callbacks, hyper = what June fills ), table names, the upgrade labels,
  no March callback in a June case ( March's FrameMoves of Wind Blade and Rising Slash, which June does not register, are dropped by design ).
- **`JUNE_BUGS.md` 531** ( 21013 zero row ) and **the `#else` paths** ( flag-off must reduce to the base ).

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum
(June's `Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.

Hygiene: `flagcheck JUNE_CHAR_ELSWORD_SKILL --with JUNE_CHAR_ELSWORD_SKILL_DEBUG --range <CH2c commit>^..<CH2c commit> --files` the four code files,
`git diff --patience --numstat`, `junediverge.py --check-all`, `enumscan.py --check` the `.cpp`, a one-file probe of `X2GUElsword_SwordMan.cpp`. Retire `_SKILL_DEBUG`
(`//#define`, build started and not awaited per the standing exception).

Spot-check 2-3 items directly in Ghidra; if one fails, re-check the whole phase. Fix clear transcription errors under the same flag; hand design decisions back to the user.
Write "CH2c - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH2d.md`, and commit.
