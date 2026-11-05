# June character Raven (Stage 7) - Phase CH1e review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1e review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1e done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (commented out until CH1 final) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1e - Done"
python june2017_re/charport_adis.py raven --phase CH1e --out <scratch dir>   # 32 listings, one file per address (a file runs on into the next function when June has no int3 between them: 32c2cc70 runs into 32c2ccc0)
```

## Task

Review the 32 CH1e class-map rows (20 `inside`, 12 `new`) against the listings. The cancel chain is the same in most of them, so read the **differences** carefully, and **count every `push` between a stack store and its reload** (the CH1d lesson):
- **The chain** `SKILL_CANCEL_AFTER( m_fSkillCancelAfter )`, back-walk `+0x8c94`, walk `+0x8c98`, dash `+0x8c9c`, Z / X `+0x8c8c`, each with the foot test; where each sits relative to the fall, the end test and the flag branches.
- **Sword Taker line:** `32c2e2f0` X-X-Z (fall, chain, end, 8-hit follow-up `>= 2` hits and `> 0.87`, counter `< 0.87`), `32c2e700` 8-hit (play count `> 4`), `32c2ea40` 8-hit Z, `32c2dd50` dash Z-Z (data class != 115, 0.5 / 0.9), `32c2dec0` dash Z-Z-Z (no cool time, 0.84; `JUNE_BUGS` #538), the dash-jump Z-Z / Z-Z-Z / Z-Z-Z-Z (`32c2cb30` `32c2cc40` `32c2cc70` `32c2ccc0`, event time 0 `+0x8c10` / `+0x8c14` / `+0x8c18`, data class 16 / 106) and the two landings `32c45520` / `32c45710` (`max / 3` ladder).
- **Blade Master line:** `32c32560` / `32c32970` / `32c32da0` X-Z / X-Z-Z / X-Z-Z-Z (templet class 16 / 106, 0.63 / 0.66, direction-change flag clear, the non-ground dash), `32c413e0` Z-Z-Z-X (templet 106 sets flag 0 at any time, `t > 0.94`, explosion before the fall), `32c41890` / `32c41a00` ZZZXZ (1.24 s, 0.4 / 0.69), `32c32de0` explosion EP, `32c41ef0` / `32c49de0` FrameMoves, `32c2cfe0` / `32c2d0f0` dash-jump X / X-Z EP, `32c459f0` / `32c45cc0` their FrameMoves (`FlushMp( 10.0f )`, `EffectSet_RST_DashJump_X` / `_XZ`, `Bip01_L_Hand` `z - 45` `y - 40` / `Bip01_R_Hand` `z - 50`), `32c2df90` / `32c2dfe0` `RSI_RBM_COMBO_ZZZupZ`.
- Enum: four `RAVEN_STATE_ID` members appended under `_CORE` (June ids 295 / 296 / 297 / 298, no `= value`); confirm each id against `ch0_state_enum_names.tsv` and the `push` values in the listings (0x127-0x12a).
- `JUNE_BUGS.md` #538.
- Probe both ways, read `cl /P` for each branch; `junediverge.py --check-all`; `enumscan.py --check X2Lib/X2GURaven.cpp`.

No build is needed if the review changes no code. Check the CH1f kickoff in the Review note.
