# June character Aisha (Stage 5 pilot) - Phase CH1l review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1l review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | Phase CH1l done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_LIVE` (undefined; the wiring), `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (the two CX2GUUser hoists) |
| Log tag | none (inert until the final CH1) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1 split (2026-10-21, user)" "CH1l - Done (2026-10-21, Opus)"
```

## Task

Review CH1l's commit against June's listings and the March code:
- **The 38 LIVE wraps** in `X2GUArme_VioletMage.cpp`: each override calls its own `June_*` member, its arguments are passed through, and March's body is whole in the `#else`. Check the dtor's `m_pStrongMindData` line, the empty `ParseCommonRandomState`, and `ApplyAuraAcceleratorBuffToGameUnit` -> base.
- **`CX2GUUser::JuneIsNotCommonMoveState`** against `31fd7640`: vslots 0x438-0x45c, `+0x7d04`, the ten riding ids (June = March + 1), 0x520 / 0x578.
- **The hooks** against their June call sites: 0x654 @321a2c5d (X2UIInventory.cpp), 0x850 @3206c186 (X2GUUser.cpp CommonFrameMove), 0x860 @3230cb1b (X2Game.cpp), 512 @32057c4c-cb2 (`CountReservedSkillUse`), 0x610 via the `329cc060` thunk (`June_RidingOffEnd` registered in `June_InitState`).
- **`SyncData::iJuneClassSyncInt`** for June's `+0x7ec0 / +0x8160`. Settle whether June puts it on the wire.
- **NPC `+0x874c` = `m_KeyCode`**, and the anti-tamper reading of `3207c1a0` / `31fdf970`.
- **The two open items:**
  - the equip anim array choice (`+0x544 / +0x584`) in `32cc8d30`;
  - 0x850's placement (June calls it inside the effect-on branch).
- Run a `cl /Zs` probe with LIVE defined. Write the next kickoff note in `CH1m.md` if one is needed.
