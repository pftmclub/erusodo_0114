# June character Elsword (Stage 7) - Phase CH2b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH2b review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH2b done and play-tested (its Done note, commit and the user's "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL`, `_SKILL_DEBUG` (defined again by CH2b; this review retires it with `//#define`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH2b - Done (2026-11-05)"
```
Then the CH2b rows of `june2017_re/funcmap.tsv` (`grep "Char Elsword CH2b"`), `june2017_re/char_elsword/ch0_skill_listing_rows.tsv` (the 21005 / 21006 / 21007 / 21031 /
21034 / 21043 / 21202-21204 / 21219 / 21225 / 21226 rows) and `JUNE_BUGS.md` row 530.

## Task

Check CH2b against June (read-only in Ghidra and `charport_adis.py elsword --phase CH2b`). In order of risk:
- **`ESSI_SA_ESK_MORTAL_BLOW_EventProcess` `32d05530`** (new, the only long function): the order of the branches (end -> up / down `+0x8ca4` -> skill `+0x8c90` -> hyper ->
  back-walk `+0x8c94` -> walk `+0x8c98` -> dash `+0x8c9c` -> attack `+0x8c8c` with Z then X), the foot-on-line test in each, vslots `0x4b0` / `0x4b4` = `GetComboZStateID` /
  `GetComboXStateID`; FrameMove `32cf4de0` = `CommonFrameMove` only; no Init, no StateStart.
- **Kick Start `32cfa6c0`**: `GetEquippedActiveSkillTemplet( 21202, true )`, level = `max( 1, GetSkillLevel( id, 0, 0, 1 ) )`, `+0x790` = `m_DamageData.fForceDownValue` (March's
  `m_DamageData` attacker type is at +0x2c4, June's at +0x3a8; the +0x790 write is the June `FORCE_DOWN` the buff phase named), ability `0x2a` = `SA_FORCE_DOWN_ABS` ( key 42 ),
  negated; memo 7's x1.1 and no-detonation are gone ( June has no such lines ). Kick EventProcess `32cfa740` = March.
- **Impact Smash EventProcess `32cfed60`**: dash `+0x8c9c`, walk `+0x8c98`, skill `+0x8c90` with the foot on a line, then the hyper branch; macros `ELSWORD_GROUND_DASH_CANCEL` /
  `_WALK_CANCEL` and `SKILL_CANCEL_AFTER`.
- **The "= March" claims** (`32cf3b00`, `32cf3f10`, `32cf4530`, `32cf9e50`, `32cfa740`, `32cfadc0`, `32cfc430`): bodies and float constants ( 2.4, 0.5, 3.667 ) against March.
- **Registration**: the six cases against the listing rows ( slot callbacks June fills: Spiral Blast Init + EP, Mortal Blow FM + EP, Double Slash EP, Kick Start + EP,
  Impact Smash Init + EP, Armor Break Init + EP; hyper = EP / FM / Start only ), table names, the upgrade labels, no March callback in a June case.
- **`JUNE_BUGS.md` 530** (21006 zero row): the numerical engine's out-of-range row.
- **The `#else` path** (Kick Start, Impact Smash EP): flag-off must reduce to the base.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum
(June's `Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.

Hygiene: `flagcheck JUNE_CHAR_ELSWORD_SKILL --with JUNE_CHAR_ELSWORD_SKILL_DEBUG --range <CH2b commit>^..<CH2b commit> --files` the four code files,
`git diff --patience --numstat`, `junediverge.py --check-all`, `enumscan.py --check` the `.cpp`, a one-file probe of `X2GUElsword_SwordMan.cpp`. Retire `_SKILL_DEBUG`
(`//#define`, build started and not awaited per the standing exception).

Spot-check 2-3 items directly in Ghidra; if one fails, re-check the whole phase. Fix clear transcription errors under the same flag; hand design decisions back to the user.
Write "CH2b - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH2c.md`, and commit.
