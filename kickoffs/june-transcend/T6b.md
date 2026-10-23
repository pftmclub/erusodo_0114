# June transcendence - Phase T6b - The transcendence slot buffs (highlight, damage, cooldown)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (Ghidra RE of a UI subsystem first), then type `/x2-phase june-transcend T6b` |
| Plan doc | `JUNE_TRANSCENDENCE_PHASES.md` (branch `main`) |
| Depends on | T6 done (it is) |
| Flag | `SERV_IRUHADEV_JUNE_TRANSCEND_SLOTBUFF` |
| Log tag | `[JUNE_TRANSCEND] SLOTBUFF` (`python tools/logs.py --grep "JUNE_TRANSCEND] SLOTBUFF"`) |
| Review | `kickoffs/june-transcend/T6b-review.md` (the phase writes it when done) |

## Why this phase exists (user, 2026-10-24)

After a transcended awakening the transcendence slots (A-5 / B-5) should glow, the slot skill should hit harder and its cool-time should drop, and the effect
should end when that skill is used or the time runs out. In game none of that is visible.

## What T6's investigation found (read these first)

```sh
python tools/docsection.py JUNE_TRANSCENDENCE_PHASES.md "T5 - Done" "T6 - Done"
grep -n "Transcend T4 review\|Transcend T5" june2017_re/funcmap.tsv
```

- **The buffs.** `31fc8c20` (T5, `JuneApplyTranscendSlotBuffs`) rolls 0.5 on awakening and every 20 s while BTI 822 / 823 holds, and applies BFI 843 and 844. They are
  data in `june_src_fixed/data036_kom/BattleFieldBuffFactor.src.lua` (lines ~115218 and ~115343) and `BattleFieldBuffTemplet.src.lua` (~26186, ~26219):
  - BFI 843 -> BTI 820 `TRANSCENDENTAL_SLOT_ATTACK_EFFECT`: `BBT_CHANGE_SKILL_DAMAGE_BY_SKILL_SLOTTYPE`, `MULTIPLY_DAMAGE_RATE 1.2`, `SKILL_SLOT_TC`; ends after
    10 s (`BFT_TIME`) or after one skill use in the TC slot (`BFT_USE_SKILL_BY_SKILL_SLOTTYPE`, count 1).
  - BFI 844 -> BTI 821 `TRANSCENDENTAL_SLOT_COOLTIME_EFFECT`: `BBT_RESET_SKILL_COOL_TIME`, `BCT_PERCENT`, `PERCENT` 0 for every level, `RELATIVE_ALL_SKILL`, `SKILL_SLOT_TC`;
    `BFT_IMMEDIATE`. So the TC-slot skill's cool-time is set to cool time x 0.
- **Already in March** (June buff port): the two behaviours, the finalizer and the TC slot type 3 (`CX2UserSkillTree::GetEquippedSkillSlotType`,
  `ApplyResetCoolTime`, `AddSkillUseCount( 3 )`). Not proven for the TC slot: nobody has seen BFI 843 / 844 act in game.
- **Not in March: the highlight.** June's `CX2Game` cool-time loop `322d8100` (from `OnFrameMove 32308c20`) acts only on slots 4 and 9. While `HaveUnfinishedBuff( 0x334 )` (BTI 820)
  holds and `JuneCanShowClassGuide( 4 )` is true, it calls the UI manager's (`g_pData +0x2cc`) `321536e0( 4, slotIndex, x, y )` to put guide-type-4 particles on the slot
  (`32151190` finds the entries, `32151fa0( 4, i )` stops them, positions from the slot data `328da440`). That UI-manager guide-particle table has no March counterpart;
  T4's review routed it to Stage 9, and the user now wants it.

## Task

1. **Prove the buffs act.** Add `[JUNE_TRANSCEND] SLOTBUFF` lines: BFI 843 / 844 applied (and whether `HaveUnfinishedBuff( 820 / 821 )` is true next frame), the TC slot's cool-time before / after
   BTI 821, the 1.2 damage factor on a TC-slot cast, and the end reason (time or use). Remember the dev free-skill gate (`JuneIsDevUnlimitedSkillUse`) hides cool-time; the play-test must say so.
   Fix whatever keeps a behaviour from acting on slot type 3 (a transcription error, not a redesign).
2. **Port the highlight.** Read `321536e0`, `32151190`, `32151fa0`, `3215a530`, `3215b0d0`, `3215cb80` and the table at UI manager `+0xd4` / `+0xe4` / `+0xf4` in Ghidra: where the guide-type-4
   entry (particle name, offset) comes from, what the particle is, and what other guide types use the same table (list them; do not port them). Then port `322d8100`'s slot 4 / 9 branch whole
   plus the table entry for type 4, and nothing for the other types. If the table needs data March does not have (a particle in `GameMajorParticle*.txt`, a texture), stage June's file with `kom.py stage-june`.
3. If the guide table is too large for this phase, say "this needs a split", write the T6c brief, and finish step 1 alone.

Done gate: after a transcended awakening that rolls the buffs, the A-5 / B-5 slots glow while BTI 820 holds and stop when the TC skill is cast or 10 s pass; the log shows the 1.2 factor and the
cool-time reset on that cast.
