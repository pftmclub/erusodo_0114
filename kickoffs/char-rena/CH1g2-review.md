# June character Rena (Stage 7) - Phase CH1g2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1g2 review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1g2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (defined; 17 new `LESI_RNW_DASH_XZ*` functions, their 17 declarations, 4 enum members) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1g2 - Done"
awk -F'\t' '$7=="CH1g2"' june2017_re/char_rena/ch0_class_map.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1g - Review"
```

## Task

Review the 29 CH1g2 rows against June (`python june2017_re/charport_adis.py rena --phase CH1g2 --out <dir>`; every listing runs past its function: stop at the first `ret` / `jmp edx` that is followed by `int3`). Spot-check at least:
- **The three combo EventProcesses** (ZZX `32c75160`, ZZXX `32c754d0`, ZZXXX `32c75840`): flag 0 / event time 0 `oneX` to `LESI_COMBO_RNW_ZZXX` / `ZZXXX` (ZZXXX has no follow-up), the animation end next to the change-time test, the 0.2 s `m_fCanNotAttackTime` and the cancel order with the foot test on each: ZZX dash (`+0x8c9c`), walk (`+0x8c98`), skill, back-walk (`+0x8c94`); **ZZXX dash, back-walk input with the walk time `+0x8c98`, skill, walk** (March has the same pairing); ZZXXX skill, walk, back-walk, dash.
- **ZZXXX FrameMove `32ca9550`:** damage `*= 1 + increase * ( count - 4 )` (March `count`), the flag and `m_bPossibleHitCount` branches, `CheckPriorPlannedBlow` inside the `m_bUsePriorPlannedBlow` block.
- **The five dash XZ / XZZ EventProcesses** (`32c81b30`, `32c81e80`, `32c82100`, `32c82390`, `32c825f0`): XZ zeroes `m_PhysicParam.nowSpeed.x` on a lost foot, START ends into LOOP (`0x112`), LOOP past `m_fEventTime[0].keyInputStart` runs both the animation-end test (FULL_CHARGE_END `0x113`) and the Z-release test (END `0x114`), FULL_CHARGE_END / END = animation end -> wait; cancels skill, walk, back-walk, dash with the foot test.
- **StateStart / StateEnd / StateEndFuture:** LOOP sets `SetNoDetonation( true )` after `CommonStateStart` and `false` after `CommonStateEnd` (June `+0x6890`); END's StateEndFuture flips `bIsRight` and `dirDegree.y + 180`; the other StateEnds are `DeleteTrapBlade` then `CommonStateEnd` (June inlines `DeleteTrapBlade`).
- **Class-map correction:** CH0's trap row 4 (`32ca11f2`, `CreateNPCReq` of `NUI_RENA_TARGETING_GUNGNIR`) named `32ca0e60`; the site is inside `32ca0f80` (Gungnir ready FrameMove, CH1f2, written at `.cpp` ~21791). `32ca0e60` only deletes the blade. START / WIN / LOSE have no Rena-range function (shared base states), so nothing was written for them.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range <CH1g2 commit>^..<CH1g2 commit>` ( FAIL only for the commented-out `#define` ); `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`; one-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` off, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. Fix clear transcription errors in place ( bytepatch only ). Write "CH1g2 - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1g3.md`, commit with explicit pathspecs.
