# June character Chung (Stage 7) - Phase CH3a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH3a review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH3a done (its Done note, commit and the user's play-test + "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`, retired by this review), `SERV_IRUHADEV_JUNE_MESH_DIR_SPEED_MODIFIER` |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH3a - Done (2026-11-02)"
grep -a "Char Chung CH3a" june2017_re/funcmap.tsv
python june2017_re/charport_adis.py chung --phase CH3a --out <short scratch dir>
```

## Task

Check CH3a against June (read-only; read the listings, not the decompiles). In order of risk:
- **Artillery Nova fire `32c076e0`.** The `fcomp` / `test ah, 5` / `jp` pairs: power rate floor 1.0 (`jp` = `>=`), life floor 0.0, the 0.99 test (`jp` skips when charge `>= 0.99`, so the 99999 store at
  `+0x950` = `m_LockOnData.m_fTimeUnlockFromStart` runs for a part charge); templet = `GetEquippedActiveSkillTemplet( 0x665e, true )`, level `GetSkillLevel( 0x665e, 0, 0, 1 )`, abilities `0xf` / `0x1f`
  (`GetSkillAbilityValue`, 4th argument 0), `0x22` = `SA_MOVE_SPEED` through `32206fb0` = `TryGetSkillAbilityValue`; `31c8aac0( 1, 0, ( L, L, L ) )` and `( 3, 0, ( L, L, L ) )` (push order: vec, add, tag);
  `+0x44c *= L`; the null-effect and null-main-effect exits to `CommonFrameMove`; the early `return` (no `CommonFrameMove`) on a null skill tree / unit; bone `TRACE_START0`.
- **Mesh DirSpeed modifier (KTDXLIB).** `31c8a720` (event vtable slot `334498f4`): non-fading, `Min <= 0` returns with no apply; `Min > 0` applies tag 3 to `+0x5cc`; fading, `fOldTime <= Min` applies to
  `+0x5d0`. `31c89d60`: tag 3 uses entry `.x` only. `31c8a9b0`: merge by (tag, add). Confirm the live `GameMajorXMeshPlayer.txt` Nova mesh = June (`Velocity` 0.0 s, then `Velocity 0` + `DirSpeed 1500` at 0.1 s).
- **Charge state `32be79d0` / `32be7a30`.** `+0x8eb8` Now / `+0x8ef4` Future = `USEFUL_VALUE0` row 0 (Chung_Iron_Cannon.lua L10111: 0.66); `jne` after `test ah, 0x41` = `anim <= value` gives 0; the clamp at 1.0.
  The key calls were CH1p's.
- **Earthquake landing `32c07990`.** `32c076e0`-style `EventTimer( 1.15 )`, `UseCannon( 3 )`, then vslot `0x228( GetEquippedActiveSkillTemplet( 0x6597, true ), 0 )` = `SetBuffFactorToGameUnit`
  (CH2d's Big Bang Stream confirmed the slot). Check the skill's BFI row (what buff June gives Chung) and that it shows.
- **Registration.** Labels 26007 / 26045 and 26206 / 26230 sit between the `case` line and `{`; the four new blocks (26226 `CSI_A_CIP_LAND_SMASH`, 26037, 26038, 26062 with `StateInit` `32bdbae0`): normal EP
  `31fcc740` = `GenericSpecialActiveSkillEventProcess`, hyper = `CopyStateCallbacks`, `m_bHyperState` (read the 32c1f530 listing at the 26037 / 26038 / 26062 / 26226 cases for the hyper store).
- **The "= March" rows** (funcmap `Char Chung CH3a`): confirm 4 against the listing, e.g. `32be7aa0`, `32be7f40`, `32bdaff0`, `32bf0c90`.
- **Play-test results:** the `artillery nova fire, charge ...` and `land demolisher landing buff factor` lines, `ES_CREATE` lines for Crack Crasher / Rage / Rapid Guardian / Land Smash, and whether the
  engine logs the 12 sounds / 1 particle of JUNE_BUGS 505. Dev free-skill gate: all skills cast with no cool time / MP under `/invin 0` + the dev gate.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_CHUNG_SKILL --range <CH3a commit>^..<CH3a commit> --with JUNE_CHAR_CHUNG_SKILL_DEBUG --with JUNE_MESH_DIR_SPEED_MODIFIER`, `git diff --patience --numstat`,
`junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`, `ch1m_regcheck.py`. Retire `_SKILL_DEBUG` (comment out the define, note it in the review). Write "CH3a - Review"
into the plan doc, update the STATUS rows (CH3a2 and CH4a unblock) and commit.
