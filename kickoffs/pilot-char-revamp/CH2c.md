# June character Aisha (Stage 5 pilot) - Phase CH2c - Skills: Energy Drain (Battle Magician)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH2c` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH2b done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (one flag for every skill sub-phase) |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`) |
| Review | `kickoffs/pilot-char-revamp/CH2c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH2b - Done"
grep -a "Char CH2b" june2017_re/funcmap.tsv | grep -a "Energy Drain\|322f0e60"
awk -F'\t' '$1=="22213" || $1=="22234"' june2017_re/char_arme/ch0_state_skill.tsv
awk -F'\t' '$1 ~ /ENERGY_DRAIN/' june2017_re/char_arme/ch0_state_skill.tsv
```

## Task

CH2b left Energy Drain (skill 22213, upgrade 22234; Battle Magician A slot) on the default path because its target search calls an unmapped engine function.
Map and port it:
- **`322f0e60`** (June `CX2Game` method on the `DAT_3372c0fc` holder; args: my unit, the target unit, three floats 80 / 40 / 45): a view-cone / line check.
  - It takes `tan( 80 deg )`, `tan( 40 deg )` and 45 deg, tests the horizontal offset and the height difference of the two units, then reads the line of the unit's last touch (`vslot 0x12c`) from the world's line map (`vslot 0x90` of the game, `+0x34`) and compares an angle.
  - Read it with `python june2017_re/funcdis.py --func 322f0e60` (Ghidra's decompile mis-types it as `void`). It returns a bool in `al`.
  - Name it, give it a March home (a `CX2Game` method under the SKILL flag) and port the whole body.
- **READY FrameMove `32cdc6f0`** (registered `32cf2bd0`): `EventTimerOneshot( 0.47 )`; the target filter order is valid unit, team, `GUSI_DIE`, HP <= 0, `322f0e60( this, unit, 80, 40, 45 )`, NPC basic class with the `( width + height ) * 0.25` offset, then the range check against ability `0x1f` (500 in PVP) with the `0x9e` / `0x9f` (`SA_SCOPE_ABS_Y` / `_Z`) bounds; read the vector compare in the listing (the decompile drops it). The chosen index is `+0x96c0`; none -> state `0xd0` (FAIL), else `0xcf` (SUCCESS). The decompile of this function also reuses stack slots: confirm every value from `python june2017_re/funcdis.py --func 32cdc6f0`.
- **SUCCESS FrameMove `32cdcac0`** (registered `32cf2be0`, state `0xcf`): the drain hit. It sets DamageData fields directly (drain rates, impact point, attacker / defender, `reActResult`), the memo type 0x12 force-down add, and plays `EffectSet_Aisha_ABM_EnegryDrain01` on me and `..02` on the target. Map each DamageData offset with the existing C-phase rows before writing it.
- **FAIL state `0xd0`** and the registration of the `0xcf` / `0xd0` states in `June_SetEquippedSkillFuncHead` (case `22213`, upgrade `22234` through `32207210`): EventProcess `32cf18f0` -> `32cc0d10`, READY EventProcess `32cf0f00` -> `32cb7fa0` (a `CommonEventProcess` forwarder, left empty).
- Write the March vs June damage of Energy Drain to `MODS.md` (June `DamageEffect_ABM_Enegry_Drain_Hand_Attack` = 6.46 at level 1).

Done gate: the S3 census shows 0 `no_state` for 22213 / 22234; on the Battle Magician save unit `ArmeBM` (or a new one) Energy Drain equipped in a slot casts June's READY state, drains an enemy in front (SUCCESS) and fails cleanly with nobody in range (FAIL).
