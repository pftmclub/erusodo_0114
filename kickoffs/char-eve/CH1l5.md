# June character Eve (Stage 7) - Phase CH1l5 - Shared CX2GUUser home for the seven June skill-use helpers

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH1l5` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1l4 review. Runs before CH1m. One at a time: `X2GUUser.*`, `X2GuEve.*`, `X2GUArme_VioletMage.*` |
| Flag | `SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS` (new, defined in `Always.h`) |
| Log tag | `JUNE_CHAR` (Aisha's skill use is live: one Aisha play-test) |
| Review | `kickoffs/char-eve/CH1l5-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1a2 - Done (2026-10-26)" "CH1l3 - Review (2026-10-26, Opus)"
grep -a -n "June_UserNoStateChangeActionFrameMove\|June_MarkSkillSlotUsed\|June_ReserveSkillUse\|June_StateChangeBySkillSlot\|June_SkillMemoMpRefund\|June_CheckSkillUseNotPve\|June_CheckSkillUseNotDungeon\|m_iJune8ea0\|m_iJune8ea4\|m_fJune8ea8\|m_fJune8eb0\|m_abJune90d3\|m_abJune90d0" X2Lib/X2GUArme_VioletMage.h X2Lib/X2GUArme_VioletMage.cpp X2Lib/X2GuEve.h X2Lib/X2GuEve.cpp
grep -a "^32008e40\|^31fc86e0\|^31fc93e0\|^31fd32e0\|^31fdbe70\|^3202d140\|^3202d3c0" june2017_re/funcmap.tsv
```

## Task

User decision (2026-10-26, CH1l3 review): June has one `CX2GUUser` body each of `32008e40` (user actions 0x25 / 0x29 / 0x36-0x39), `31fc86e0` (slot used marks),
`31fc93e0` (vslot 0x804, reserve the skill use and its MP), `31fd32e0` (slot state change), `31fdbe70` (vslot 0x764, type 10 memo MP refund), `3202d140` / `3202d3c0`
(skill 900 bans), and the state they keep (`+0x8ea0` / `+0x8ea4` / `+0x8ea8` / `+0x8eb0`, `+0x90d0[3]`, `+0x90d3[10]`). Aisha (live, `SERV_IRUHADEV_JUNE_CHAR_ARME_*`) and
Eve (inert, `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`) each carry a private copy, and June's base code that reads the reserved MP would not see a class-local field. Give them one home,
in CH1a2's shape:

- Add the seven members and six fields to `CX2GUUser` (`X2GUUser.h` / `.cpp`, CP949: `bytepatch.py` only) under the new flag, non-virtual, protected, callable from derived classes;
  zero the fields in the `CX2GUUser` constructor. First diff the Aisha and Eve copies: the bodies must agree (Eve's file-local helpers `JuneEvePushInfoText` /
  `_CONST_EVE_JUNE_CH1l3_` user action ids, Aisha's equivalents). If they differ in anything but names, stop and report the line, do not pick one.
- Retire both class copies (declarations, bodies, fields) with `#ifndef SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS` ... `#endif SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS`,
  so flag-off is today's tree byte for byte. Callers keep their calls. Constants the bodies need go in a `_CONST_GUUSER_JUNE_CH1l5_` named namespace (no anonymous namespace).
- Done gate: `cl /P` of `X2GUArme_VioletMage.cpp` and `X2GuEve.cpp` shows no class definition of the seven; the build passes; `flagcheck.py JUNE_USER_SKILL_USE_HELPERS --worktree` PASS;
  `enumscan.py --check` clean on the touched files. Aisha's skill use is live, so the hand-off is one Aisha play-test (skill slots A and B, MP spent, cool time shown).
- Note for later character plans: their CH1 kickoffs call the `CX2GUUser` members and never copy them (one line in the Done note and in `kickoffs/june-backlog/stage7-plan.md`).
