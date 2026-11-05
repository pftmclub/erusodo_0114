# June character Raven (Stage 7) - Phase CH1f review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1f review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1f done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (commented out until CH1 final) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1f - Done"
python june2017_re/charport_adis.py raven --phase CH1f --out <scratch dir>   # 38 listings, one file per address (a file runs on into the next function when June has no int3 between them: 32c306c0 runs into 32c30710, 32c31a80 into 32c31ec0)
```

## Task

Review the 38 CH1f class-map rows (29 `inside`, 9 `new`) against the listings. The cancel chain is the same in most of them, so read the **differences** carefully, and **count every `push` between a stack store and its reload** (the CH1d lesson):
- **The chain** `SKILL_CANCEL_AFTER( m_fSkillCancelAfter )`, back-walk `+0x8c94`, walk `+0x8c98`, dash `+0x8c9c`, Z / X `+0x8c8c`, each with the foot test; where it sits relative to the fall, the end test and the flag branches in `32c2d130`, `32c2ed70`, `32c2f1b0`, `32c2f6a0`, `32c2fb90`, `32c2ffb0`, `32c302e0`, `32c30710`, `32c31a80`, `32c31ec0`, `32c32210`.
- **Over Taker:** `32c2ed70` X-X-front-X (event time 0: change `+0x8c18` or the end -> `XXfrontXX`, window `+0x8c10` / `+0x8c14` with X + forward; `t > +0x8c18` -> `XXfrontXX4`, end -> WAIT), `32c2f1b0` / `32c2f6a0` / `32c2fb90` (flag 0 = X + down -> `XXfrontXXdownX` 0xc3; flag 1 on event time 1 `+0x8c1c` / `+0x8c20` / `+0x8c24` -> `XXfrontXX2` 0xc0 / `XXfrontXX3` 0xc1; `t > +0x8c24` -> `XXfrontXX4` 0xc2, no end test), `32c2ffb0` (`XXfrontXX4` and `XXfrontXXdownX`), `32c302e0` X-X-down-X, `32c2d130` dash Z-X, `32c47160` X-X-down-X FrameMove (`FlushMp( 4.0f )`, `DamageEffect_RSI_OVER_TAKER_XX_DOWN_X`, count the pushes of the 23-argument `CreateInstance`), the dash-jump Z-X `32c30600` / `32c30690` / `32c306c0` / `32c47700` (flag window + `> 0.4` ladder) / `32c30710` / `32c47970` (`CommonFrameMove` first, 0.31 s damage effect).
- **Reckless Fist:** `32c316a0` / `32c31730` / `32c317e0` / `32c31830` / `32c31860` dash-jump X / X-X (all = March after the numbers are compared), `32c49ab0` FrameMove (`FlushMp( 4.0f )` where March spends 10), `32c31a20` / `32c31a80` (0.6 s + flag 1, dash without a foot test, flag 0 -> `ZZXFRONTXX` at 0.57 s or the end, templet class 107), `32c31ec0` (end, chain, then the fall), `32c32210` (no `+0x8ca4` read: `JUNE_BUGS` #539), and the short rows (`32c26390` ... `32c26b70`).
- Enum: five `RAVEN_STATE_ID` members appended under `_CORE` (June ids 191-195, no `= value`); confirm each against `ch0_state_enum_names.tsv` and the listings' pushes (0xbf-0xc3).
- `JUNE_BUGS.md` #539.
- Probe both ways, read `cl /P` for each branch; `junediverge.py --check-all`; `enumscan.py --check X2Lib/X2GURaven.cpp`.

No build is needed if the review changes no code. Check the CH1g kickoff in the Review note.
