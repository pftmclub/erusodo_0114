# June character Aisha (Stage 5 pilot) - Phase CH2c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH2c review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH2c done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH2c - Done"
grep -a "Char CH2c" june2017_re/funcmap.tsv                                   # the 5 rows the phase wrote or corrected
```

The `June_*` members are in the CH2c block at the end of `X2Lib/X2GUArme_VioletMage.cpp` (`//{{ ... Phase CH2c`), the registration case is in `June_SetEquippedSkillFuncHead`
(after the CH2b cases), the cone is `CX2Game::JuneIsUnitInViewCone` in `X2Lib/X2Game.cpp` (after `IsFront`). Not carved in Ghidra: read with
`python june2017_re/funcdis.py --func <va>` (`32cc0d10` is merged with `32cc0b80` / `32cc0c50` there, it starts at `32cc0d10`).

## Task

What the phase could get wrong, in the order of risk. **Ghidra's decompile reuses stack slots here: confirm every value in the listing.**
1. **`322f0e60`** (`@322f0e60-322f11c5`): the x87 stack walk of the height band (`@322f0f89-322f1042`: which slot is `A` = lenSq * tan up, `Bv` = lenSq * -tan down,
   the sign, `dy^2` from the pow loop) and the three compares' directions (`test ah, 5 / 0x41` + `jp / jnp / je`); GetLandPos (vslot `0x134`) for x / z, GetPos
   (vslot `0x138`) for y and for the side vector; facing `0.1 / -0.1`; `-side > angle` and `side < angle`; `32fe2c94` = `_CIacos` (the IAT `+0x40` argument in the
   funcmap row). JUNE_BUGS 386 (tan, not tan^2): confirm it is June's, not a misreading.
2. **READY `32cdc6f0`**: filter order (valid, team, `GUSI_DIE`, HP, cone, NPC class + offset, angle band, distance), the band `SCOPE_ABS_Z <= GetDirVecToDegree( d ).z <= SCOPE_ABS_Y`
   (`@32cdc9cd-32cdc9ee`, slots `[B+8]` / `[B+0xc]`), `<=` on the distance, PVP 500, the unit-NULL return without CommonFrameMove, the `[JUNE_CHAR]` decision line (ours, not June's).
3. **SUCCESS `32cdcac0`**: the DamageData writes (`+0x3a0` base; FORCE_DOWN `+0x3f0` decoded from `32632390 @3263892e`), the memo stub `31fca9e0` (no x2 and no random draw),
   the type 0x12 memo add, the effect-set calls against March's `PlayEffectSet( name, this )` / `( name, unit, unit )`.
4. **`32cc0d10`** against the inline chain (BWALK before WALK, Z / X on `m_fAttackCancelAfter`, no CommonEventProcess) and **`32cb7fa0`** registered as a CommonEventProcess
   forwarder (CH2b's "left empty" was wrong for an EventProcess: check the dispatcher in `X2GUUser.cpp`).
5. **Registration vs `32cede00 @32cefeba-32ceffa5`**: READY on the slot states (hyper copies FrameMove and EventProcess), SUCCESS `0xcf` with both callbacks, FAIL `0xd0` with the
   EventProcess only; 22234 reaches case 22213 through `JuneSkillIDByUpgrade`.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_ARME_SKILL --range <CH2c commit>^..<CH2c commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines.

Play-test: unit `ArmeBM` (Battle Magician) with Energy Drain in slot 6; the log shows `ARME SKILL registered A_ABM_ENERGY_DRAIN` and one `ARME SKILL Energy Drain SUCCESS / FAIL`
line per cast.

Spot-check 3 functions directly in the listing; if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH2c -- Review" into the plan doc under the Done note, update the CH2c row in `STATUS.md`, and commit with explicit pathspecs.
