# June character Raven (Stage 7) - Phase CH1d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1d review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1d done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (commented out until CH1 final) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE`; `Always.h` gained a dependency guard on `SERV_IRUHADEV_JUNE_UPDOWN_CANCEL` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1d - Done"
python june2017_re/charport_adis.py raven --phase CH1d --out <scratch dir>   # 38 listings, one file per address (a file runs on into the next function when June has no int3 between them)
```

## Task

Review the 38 CH1d class-map rows (34 `inside`, 4 `new`) against the listings. The cancel chain is the same in most of them, so read the **differences** carefully:
- **The chain** `SKILL_CANCEL_AFTER( m_fSkillCancelAfter )`, back-walk `+0x8c94`, walk `+0x8c98`, dash `+0x8c9c`, Z / X attack cancel `+0x8c8c`: ground-only (`IsOnSomethingFuture()`), `RAVEN_GROUND_*` macros in `X2GURaven.cpp`.
  `RSI_COMBO_Z` is the exception: Z / X first, then back-walk / walk / dash **without** the foot test and each sets `m_fCanNotAttackTime = 0.1f` before `StateChange`.
- **Per function** (check the order of the `else if` chain, the flag clears, the float constants and which listing branches go to the separate end `if` and which to the `else if`):
  `32c297a0` Z, `32c29bb0` ZZ (0.39), `32c29fd0` ZZZ (no `m_fReAttackZ1Time` store; Blade Master up-Z, class 115 / 107 / other ZZZX; window 0.166 - 0.59; classes read from unit data +0x38),
  `32c2a550` ZZZZ, `32c2a880` ZZZX (end -> new `RSI_COMBO_ZZZX_FINISH`), `32c2abc0` finish (new; fall = `LINE_RADIUS`, not 1.5 x), `32c2aef0` ZZX (templet class 107),
  `32c2b360` ZZXX (`m_fUpDownCancelAfter` first), `32c2b6f0` X (templet 16 / 106, 0.48 / 0.53), `32c2bbb0` XX (OT / RF: down -> `XXdownX`, front -> `XXfrontX`; ST / BM `XXZ`),
  `32c2c070` XXX, `32c2c370` / `32c2c810` / `32c2d450` dash combos, `32c2d750` dash-jump Z (classes 16 / 106 own ZZ), `32c2d930` dash-jump X-Z, `32c45f80` landing.
- **FrameMove with an effect set**: `32c44f90` ZZZX (0.49 s, `EffectSet_A_RF_ZZZX`) and `32c45260` XXX (0.16 s, `EffectSet_A_RF_XXX`), both `FlushMp( 3.0f )`, else the March particle; `32c46170` dash-jump X (`pos.z -= 10`, `JUNE_BUGS` #537).
- `32c5c230` dash combo X FrameMove = March's `BALANCE_BLADE_MASTER_20130117` branch (`m_itNowStateData->first`).
- Enum: eight `RAVEN_STATE_ID` members appended under `_CORE` (June ids 170 / 299 / 211 / 304 / 303 / 302 / 294 / 301, no `= value`); confirm each id against `ch0_state_enum_names.tsv`.
- `JUNE_BUGS.md` #536 (Sword Taker up-Z) and #537.
- Probe both ways, read `cl /P` for each branch; `junediverge.py --check-all`; `enumscan.py --check X2Lib/X2GURaven.cpp`.

No build is needed if the review changes no code. Check the CH1e kickoff (the amendment names the enum members CH1d already added) in the Review note.
