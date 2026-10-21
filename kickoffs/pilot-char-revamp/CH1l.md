# June character Aisha (Stage 5 pilot) - Phase CH1l - The virtuals, the CH1i-review gaps, the CX2GUUser hoists

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1l` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1k review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE`; every virtual switch-over behind `SERV_IRUHADEV_JUNE_CHAR_ARME_LIVE` (March's body in the `#else`) |
| Log tag | none (inert until the final CH1) |
| Review | `kickoffs/pilot-char-revamp/CH1l-review.md` (the phase writes it) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1 split (2026-10-21, user)" "CH1h - Done (2026-10-20, Sonnet 5.5)" "CH1i - Done (2026-10-21, Sonnet 5.5)" "CH1i -- Review (2026-10-21, Opus)"
```
Then `kickoffs/pilot-char-revamp/CH1.md` item 3 and its "From CH1a / CH1c / CH1e / CH1i / the CH1i review" notes (constructor members, MemoRise handle, `SPECIAL_OPTION_VALUE`).

## Task

1. **Virtuals (`CH1.md` item 3).** Each override that CH1h / CH1i wrote as an inert `June_*` member becomes the virtual's body under LIVE, with
   March's body in the `#else`. That includes the lifecycle (`June_ConstructorInit`, the destructor, `ReInit`, `InitDevice`, `InitComponent`,
   `InitEffect`), the vslot `0x600`-`0x880` overrides, and the 509-512 skill-use steps (`CountReservedSkillUse` calls 512). Drop March-only virtuals
   that June lacks (`ParseCommonRandomState`, `ApplyAuraAcceleratorBuffToGameUnit`) under the flag.
2. **The CH1i-review gaps:** `31fd7640` (the user-base test behind vslot `0x828`, for `June_Vslot828`); the Future -> Now copy of the synced skill
   id (`+0x7ec0` -> `+0x8160`); NPC `+0x874c` (the gate in `32ccdae0`); the dirty-state batch `3207c1a0` / `31fdf970` around `SetEquippedSkillLevel`;
   the equip anim array choice (`+0x544` / `+0x584`) in `June_UpdateEquipCollisionData`.
3. **The CX2GUUser hoists** listed in the CH1i notes (`m_iJune8ea0 / 8ea4`, `m_fJune8ea8 / 8eb0`, `m_iJune7ec0 / 8160`, `m_abJune90d3 / 90d0`,
   `m_bJuneAfterImageSaved`, and the CX2GUUser pieces June has: `SetSkillBuffFactors`, `GetSkillLevel`, ...). Where a piece is used only by Aisha, it
   may stay Arme-local; say which in the Done note.

## From CH1k (2026-10-21)

- **The dirty-state batch is in `SetEquippedSkillFuncBySkillSlotIndex` too.** June `32cede00` builds `3207c1a0` on entry and runs `31fdf970` at
  every exit, so the batch wraps the head (`June_SetEquippedSkillFuncHead`) and March's skill cases that follow it under LIVE. Settle both
  callers (this one and `SetEquippedSkillLevel`) with one design.
- **`COMMON_BUFF_FrameMove` is virtual dispatch in June.** The head registers it as June does: `32a77a20` is `jmp [vtable+0x5f0]`, and Arme's body
  is `32cc6e50` (`June_COMMON_BUFF_FrameMove`). Making that virtual June's body (item 1) is what gives the buff states June's FrameMove.
