# June character Elsword (Stage 7) - Phase CH2a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH2a review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH2a done and play-tested (its Done note, commit and the user's "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL`, `_SKILL_DEBUG` (defined; this review retires it with `//#define`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH2a - Done (2026-11-04)"
```
Then the CH2a rows of `june2017_re/funcmap.tsv` (`grep "Char Elsword CH2a"`), `june2017_re/char_elsword/ch0_skill_listing_rows.tsv` (the 21000-21004 / 21026-21030 /
21200 / 21201 / 21217 / 21218 rows) and `JUNE_BUGS.md` rows 528 and 529.

## Task

Check CH2a against June (read-only in Ghidra and `charport_adis.py elsword --phase CH2a`). In order of risk:
- **`UNLIMITED_BLADE_FrameMove` `32d101f0`** (the one big function, shared by the normal and the hyper state): the nine timers and their order (0.166, 0.433, 0.666,
  0.966 + `RT_BIG_DAMAGE`, 1.3; then not hyper 1.566, 1.95; hyper 1.5, 1.73), the hyper final hit's two `PlaySequenceByTriggerCount` calls (position y + 10 for the fire
  wave, land position = the same y; the KuAAang one at the projected position, 100 / 100 / 1, only with the small gauge shown), `IsHyperState()` against the byte at
  `+0x10` of the state data behind `+0x7cbc`, handle slots `+0x97a8` / `+0x97b4` = `ELSWORD_MINOR_PII_STR_KUAANG` / `_MAGNUM_BLADE_FIRE_WAVE`.
- **The Mega Slash memo states** (`32cf3920` .. `32cf9960`, `32d0fe90`, `32d10020`): the registration in `SetMegaSlashSkillFunc` (slot names against `ch0_skill_listing_rows.tsv`,
  the hyper state has no Init, CHARGE 0xea and FIRE 0xeb own their state ids), the charge test (`>= 1.1` or the key released, `FollowSlotSwap` first), the x1.5 power
  at `0.5 <= charge`, `PlayEffectSet`'s argument order (hyper flag fourth, power rate fifth, the rest default).
- **Rolling end** `32d08880` (the 10% HP gate and `UpNowHp` with minimum 0 = `UpHPIfAlive`) and `32cfd880` (`ELSWORD_GROUND_WALK_CANCEL`); the "= March" claims of the
  funcmap rows (`32cf3a10`, `32cf3aa0`, `32cf41d0`, `32cf4200`, `32cf9990`, `32cf9d80`, `32cf9f20`, `32cfd840`, `32d137a0`).
- **Registration**: each case's table names (hyper table = the normal name where June has no `_HYPER`), the slots June fills (Flame Geyser and Triple Geyser: Init + EventProcess
  only), the upgrade labels, and that no March callback survives in a June case.
- **The `#else` path** and the two shared edits (`ELSWORD_EVENT_TIMER_ONESHOT`, the equip debug line): flag-off must reduce to the base.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.

Hygiene: `flagcheck JUNE_CHAR_ELSWORD_SKILL --with JUNE_CHAR_ELSWORD_SKILL_DEBUG --range <CH2a commit>^..<CH2a commit> --files` the four code files,
`git diff --patience --numstat`, `junediverge.py --check-all`, `enumscan.py --check` the `.cpp`, a one-file probe of `X2GUElsword_SwordMan.cpp`. Retire `_SKILL_DEBUG`
(`//#define`, build started and not awaited per the standing exception). Note for CH2b: `RNSI_SA_ES_*` names for 21001-21004 / 21027-21030 exist now.

Spot-check 2-3 items directly in Ghidra; if one fails, re-check the whole phase. Fix clear transcription errors under the same flag; hand design decisions back to the user.
Write "CH2a - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH2b.md`, and commit.
