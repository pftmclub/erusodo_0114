# June character Raven (Stage 7) - Phase CH1h review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1h review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1h done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (commented out until CH1 final) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1h - Done"
python june2017_re/charport_adis.py raven --phase CH1h --out <scratch dir>   # 37 listings, one file per address (a file runs on past its `ret` when June has no int3 between functions: cut at the first int3 / ret; 32c273f0 ends with its jump-table data)
```

## Task

Review the CH1h rows: 27 `inside` function bodies, 4 header inlines, 2 `new` (`_CORE`) and 4 with nothing to write, against the listings. The Done note says 25 equal March's body and 6 differ (`AttackResultPlayEffect`, `ApplyRenderParam`, `GetHyperModeTime`, `HyperModeBuffEffectStart`, the destructor, `AttackResultByType`): check the **equal** claims as hard as the differing ones, and the five CH0 map corrections (`32c273f0` = `GetReduceDebuffDurationRate`, `32c38340` / `32c383b0` exist in March, `32c43080` = `DeleteRavenMajorParticle`, `32c68f90` = `Weapon::SetTraceColor`). Test every float and flag with its x87 flag test (`test ah, 5` + `jp` = NOT less; `fucompp` + `test ah, 0x44` + `jnp` = equal) and **count the `push`es between a stack store and its reload**:
- **`GetHyperModeTime` `32c3db70`:** the gage test is `0 < remain` (`jp` to the base otherwise), the count is `vslot 0xb0 - vslot 0xac` (`GetLastStateHyperModeCount` minus `GetHyperModeCount`) clamped at 0 and called twice only when positive, x the protected float at `+0x24c`, `+0x8fc3` = `GetCanChangeHyperModeInHyperModeState`, `GetUserSkillTemplet( 0x5f55, 1 )`, `GetSkillLevel( id, 1, 0, 1 )` twice around the `< 1` test, `GetSkillAbilityValue( 1, level, 0 )` = `SA_EFFECTIVE_TIME`, and the final `0 < [+0x422c]` returns the forced time instead of the product (the stack slots are shifted by `push esi` / `push edi`: confirm which slot holds the result).
- **`HyperModeBuffEffectStart` `32c4e7c0`:** `GetUnit()` and `+0x3c` and the awakening test (`31fd66b0`) skip only the four boost handles; then the aura, then `+0x8fc3` with `HaveThisBuff( 5 )` or `( 0x336 )` -> gauge 100, otherwise `vslot 0xb0 x 20.0`, then `min( g + 40, 100 )` (confirm the `fcom st(2)` / `test ah, 0x41` direction) and the dead `> 100` clamp.
- **`AttackResultByType` `32c56be0`:** the arc helper runs before the hyper-time test; `[esi+8] == 1` (`AT_SPECIAL`) skips the whole fill block; the fill needs my unit and both ball flags clear (`+0x97a9` then `+0x97a8`); gain order `rate x max x 4.0 x (+0x6c50 + 1.0) x ability`, `GetSkillLevelValue( 0x5f63 )` and `32007b10( 0x18, 0x5f63 )` (is `0x18` = `SA_CHARGING_TIME` in June? `SA_COOLTIME_1` = 148 / `SA_TIME_ABS` = 169 agree with March); then `> 100` -> 100, else `( int )( g / 20.0 ) == 1` -> 60 (`@32c56e0d-32c56e93`, `jnp` = equal skips); the NPC templet class test, the flags, `32610fb0( 1 )` = `IsAttackTrigger( DTT_RAVEN_RAGE_ATTACK )`, the ball effect (`GetInstance( +0x18, 1 )`), `clock()` into `+0x9778`, the lock-on, the five `Attack1`-`Attack5` scales 1 / 1.6 / 1.4 / 1.6 / 1.5, `RestoreNasodBall` on the default.
- **`AttackResultArcEnemy` `32c54960`:** the order of the returns, the attack type gate (`0` or `2`), `Clear` on a missing data / effect / mesh, the lock-on offsets `+0x938` / `+0x93c` / `+0x940`, 150 / 3000 / 0.35 / 9999 / 0.3.
- **Destructor `32c54c10`:** the order and the missing `m_hSeqHyperBall`; **`ApplyRenderParam` `32c38410`:** `+0x144` = `IsAlphaObject()`, `RT_CARTOON` = 3, the alpha store; **`DamageReact` `32c40b20`:** the ten extra damage ids; **`ReInit` / `Init`** member offsets against `ch1_offsets.tsv`'s new rows and March's declaration order.
- **`ApplyEnchantHandDebuffToDamageData` `32c5d540`:** `DamageData +0x48c` is `m_wstrCreateDamageEffectName`'s size (March layout `+0x3cc` + `0xb0` = June `+0x47c`, size at `+0x10`: confirm the `+0xb0` shift with `m_fHpPercentUp` `+0x504`), `GetBuffFactorPtr( 0x279 )`, level `GetSkillLevel( 0x5e8e, 1, 0, 1 )` or 1 without a unit, `PushBuffFactor( clone, false )` (the `322b5320` argument list: factor, level, `0`, rate pair 1.0 / 1.0).
- Probe both ways (`census.cl_cmd`, `/c /Zs /EHsc`; pass `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, **not** `/D...`: Git Bash rewrites a leading `/D` into a path), `cl /EP` string counts for each branch; `junediverge.py --check-all`; `enumscan.py --check X2Lib/X2GURaven.cpp`.

No build is needed if the review changes no code. Check the CH1i kickoff in the Review note.
