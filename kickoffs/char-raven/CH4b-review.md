# June character Raven (Stage 7) - Phase CH4b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH4b review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH4b done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_PASSIVE` + temporary `_PASSIVE_DEBUG` (the review retires it) |
| Log tag | `[JUNE_CHAR] RAVEN PASSIVE` (`logs.py --grep "RAVEN PASSIVE"`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH4b - Done (2026-11-07)"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH4b - Review"    # the worked example
```
Then the listings of `32c5c080`, `32c38180`, `32c382d0` (`cd june2017_re && python charport_adis.py raven <va>`). Funcmap rows `(CH4b)` carry the reading.

## Task

Check CH4b against June (read-only in Ghidra). In order of risk:
- **`SetDamageExceptionProcess` `32c5c080`:** base first; Open Mind's skill id ( DD `+0x5c4` if > 0, else vslot 0x524 ), the type set { 7, 14, 15, 16 }
  ( 12 / 13 excluded ) and the order fMagic `+0x3c` then fPhysic `+0x38`; Infinity Rage's DT_MAGIC, the move of physic into magic, the
  `( rel - 1 ) x 0.5 + 1` factor on `+0x9690`, and that DD `+0x47c` is `CREATE_DAMAGE_EFFECT_NAME` ( the port reads it from the `+0x498` scale that
  follows; decode the key at `32633650` if in doubt ); Win and Win's `GetSkillBuffFactorPtr( 24422, 0, true )` then `PushBuffFactor( ptr, true )`.
- **`GetActualMPConsume` `32c38180`:** the jump structure ( 24011 / 24030 via vslot 0x240 `HaveSkillBuff`, the four-id x0.5 set via `3229a3d0` +
  `HaveThisBuff` ), the memo 5 / 28 cases left out because `31fca9e0` returns 0, the clamp and the `3234d840` event rate. JUNE_BUGS 558.
- **`GetActualCoolTime` `32c382d0`:** null -> 0, base, x `SA_COOLTIME_1` of 24422 while its buff is up, no clamp.
- **Names:** `RNSI_P_RRF_INFINITY_RAGE` 24420, `RNSI_P_RVC_WIN_AND_WIN` 24422, `DTT_RRF_BURNING_NASOD_HAND_POSSIBLE_WITH_INFINITY_RAGE` 18,
  `DTT_RVC_IGNITION_CROW_SELF_DAMAGE` 19 against June's `Enum.src.lua`.
- Play-test result and log lines ( `mp:`, `win and win cool time`, `win and win: self damage hit`, `open mind`, `infinity rage` ), then retire
  `_PASSIVE_DEBUG` ( `//#define` ).

Hygiene: `flagcheck.py JUNE_CHAR_RAVEN_PASSIVE --with JUNE_CHAR_RAVEN_PASSIVE_DEBUG --range <CH4b commit>^..<CH4b commit> --files` the five code files,
`junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GURaven.cpp`.
No build if the review changes no code (a `//#define` retire is a not-awaited build). Then check the CH4c kickoff (`kickoffs/char-raven/CH4c.md`).
