# June character Raven (Stage 7) - Phase CH1j review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1j review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1j done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (commented out until CH1 final) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1j - Done"
python june2017_re/charport_adis.py raven --phase CH1j --out <scratchpad dir>   # 9 listings: 32c43380 (507 lines) and 32c55240 (878) are the big ones
```

## Task

Review the nine CH1j rows against the listings. Check as hard as the Done note's claims:
- **`CommonStateStart` `32c43380`:** the three keys (`DAMAGEDATA_CHANGE_TIME`, `...1`, `...2` into `+0x9750` / `+0x9754` / `+0x9758`; the Done note decoded them from the xor tables), the inlined `ResetDamageType` (`+0x4114` = 1.0, `+0x410c` = 0, `+0x4110` = 0, `+0x4108` = -1), `BURNING_NASOD_HAND_POSSIBLE` (decoded @32c43911) and the `+0x9688` byte gate, `+0x4108` = 2 (`AT_COMMAND`), `+0x9694` -> `+0x8b10` (`SetPassiveCriticalRate`), then the state test: skip when the id (`( +0x7cbc - +0x7cec ) / 100`) is 0xa8 or 0xd7 / 0xd8 (`RSI_COMBO_ZZZX`, `RBM_COMBO_ZZZX_EXPLOSION`, `RBM_COMBO_ZZZXZ` by `ch0_state_enum_names.tsv` col 3), else the hit count `+0x9674` -> 0 and `StopEffectSet( +0x9684, false )`.
- **`CommonFrameMove` `32c55240`:** Open Mind (skill 0x5f63 level, gage `+0xc04` hyper time `+0xe8` > 0, `GetLearnedSkillAbilityValue( 0xa9 = SA_TIME_ABS, 0x5f63 )` against `( clock() - +0x9778 ) / 1000.0` and the jump direction of `test ah, 0x41` + `jp`; the rage gauge `+0x97b8` compared with 60.0 and the temp written back as 60.0f (`0x42700000`) or itself); the arc-enemy loop (index counter starts at -1, jump table `32c55ec8` = 0.06 / 0.07 / 0.08 / 0.09 for i = 1..4, default 0.05); the tail order (nasod ball, arc time, Black Hole `0x99` = `RSI_BASE`, overheat buff time, overheat object, two fire effects, `+0x9774`, `32c42f90`, `+0x959c`); the Guardian Strike restore (`+0x164` = 0xb = `XL_UNIT_0`, `+0x8e04` = `m_bSetCustomRenderParam` identified by X2GUEL.cpp:9209, colour `+0xc38`.. = 1, `+0x144`); March's hyper ball and inline Survival Technique blocks are absent from the listing.
- **`32c42f90`, `32c25ac0`** (base only), and the equal rows `32c25ab0` / `32c25ad0` / `32c25ae0` / `32c3d1e0` / `32c40ce0` (nothing written).
- Names and offsets: `m_fSurelyVictoryCoolTime` (`+0x959c`), `m_bBurningNasodHandLearned` (`+0x9688`), `m_fGuardianStrikeRenderTime` (`+0x9774`) in `ch1_offsets.tsv`; the CH1k / CH4a / CH2c / CH1q kickoff amendments.
- Probe both ways (`census.cl_cmd`, `/c /Zs /EHsc`; `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, not `/D`), `cl /EP` string counts, `flagcheck JUNE_CHAR_RAVEN_LIVE --with JUNE_CHAR_RAVEN_CORE --range <commit>^..<commit>`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GURaven.cpp`.

No build is needed if the review changes no code. Check the CH1k kickoff (Amended by CH1j) in the Review note.
