# June character Eve (Stage 7) - Phase CH1a2 - Shared CX2GUUser home for the two June skill-level helpers

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH1a2` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1a review (done). Runs before CH1b: both edit `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_USER_SKILL_LEVEL_HELPERS` (new, defined; needs `SERV_IRUHADEV_JUNE_TRANSCEND_UNIT`) |
| Log tag | none (no behaviour change, no play-test) |
| Review | none of its own: the CH1b review spot-checks it (CH1b's kickoff says so) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1a - Review (2026-10-26)"
grep -a -n "June_GetSkillLevel\|June_GetLearnedSkillAbilityValue" X2Lib/X2GUArme_VioletMage.h X2Lib/X2GUArme_VioletMage.cpp X2Lib/X2GuEve.h X2Lib/X2GuEve.cpp
grep -a "^31fceab0\|^32007b10" june2017_re/funcmap.tsv
```

## Task

User decision (2026-10-26, CH1a review): June has one `CX2GUUser` body each of `31fceab0` (skill level for a skill id) and `32007b10`
(the learned level's ability value). Aisha (`X2GUArme_VioletMage.*`, under `SERV_IRUHADEV_JUNE_CHAR_ARME_*`) and Eve (`X2GuEve.*`, under
`SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`) each carry a private copy, and every later character port would add one more. Give them one home:

- Add `int June_GetSkillLevel( const int iSkillID_ ) const` and `float June_GetLearnedSkillAbilityValue( const CX2SkillTree::SKILL_ABILITY_TYPE eType_, const int iSkillID_ ) const`
  to `CX2GUUser` (`X2GUUser.h` / `.cpp`, CP949: `bytepatch.py` only) under the new flag, non-virtual, callable from derived classes.
  Bodies are the live branches the two class copies already share (the hyper-class test reads `m_iUnitClassTran == 1`, `31fceab0 @31fceaef`).
  The new flag carries a `#error` guard that `SERV_IRUHADEV_JUNE_TRANSCEND_UNIT` is defined (settle it with `cl /EP`, CLAUDE.md rule 3), so the shared body needs no `#else`.
- Retire the two class copies (declarations and bodies) by wrapping each in `#ifndef SERV_IRUHADEV_JUNE_USER_SKILL_LEVEL_HELPERS` ... `#endif SERV_IRUHADEV_JUNE_USER_SKILL_LEVEL_HELPERS`,
  so flag-off is today's tree byte for byte. Every caller keeps its call: a derived class with no own member now resolves to `CX2GUUser`'s.
- Leave the inline readers in `X2DamageManager.cpp:5120` and `X2GameUnit.cpp:17442` alone (live code from earlier phases; not this phase's subject).
- Note for later character plans: their CH1 kickoffs call the `CX2GUUser` pair and never copy it (add one line to the Done note; the
  `stage7-plan` brief in `kickoffs/june-backlog/` gets the same line).
- Done gate: no class keeps its own copy with the flag on (`cl /P` of both class `.cpp`s shows no `::June_GetSkillLevel` definition); the build passes;
  `flagcheck.py JUNE_USER_SKILL_LEVEL_HELPERS --worktree` PASS. Nothing to play-test: the hand-off says so and names CH1b next.
