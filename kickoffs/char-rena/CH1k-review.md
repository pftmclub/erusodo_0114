# June character Rena (Stage 7) - Phase CH1k review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1k review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1k done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`; `SpecialAttackEventProcess`, `SpecialAttackNoStageChange`, `NoStateChangeActionFrameMove`, `GetSkillUseCondition`, constructor tail, `IsNotCommonMoveState`, `ReservedSkillStart`, `AfterImageFrameMove`, `GetStateAnimSpeedRate`, `X2GUUser.*` chains), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (defined; lookup / use / cooldown, the Eradication helper, constructor stores), `SERV_IRUHADEV_JUNE_ENUM_NAMES` (8 `RNSI_` ids) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1k - Done"
awk -F'\t' '$7=="CH1k"' june2017_re/char_rena/ch0_class_map.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1k - Review"
```

## Task

Review the 21 CH1k rows plus `ReservedSkillStart` against June (`python june2017_re/charport_adis.py rena --phase CH1k --out <dir>`; listing of `32cb5b70` ends in jump-table bytes, not code). Spot-check at least:
- **Lookup `32cac200`:** the order ( quick-slot wait `+0x8d78`, `CommonSpecialAttackEventProcess`, templet, id != slot -> 0x8bae, `+0x8ea0` store, Delayed Fire `0xc9b` + active + cond != 3 -> 1, the three passive widenings to 2, `CheckSkillUseCondition`, `3202daf0`, `3202cba0`, `3202d140`, `3202d3c0`, riding wait, `GetActualMPConsume` vslot 0x6a0 `( id, level, 1 )`, vslot 0x82c, `ShownSkillCoolTime` vslot 0x6a8 > 0 -> 226, MP -> 2549, stone -> 26119 ). The helper bases: `32c6d700` { 0x59dd, 0x59f7, 0x59e0 }, `32c6d760` { 0x5aa0, 0x5ab4 }, `32c6d790` { 0x5aaf, 0x5ab3, 0x5ab1, 0x59ee }; `32c6d480` Fatality 0xbc7 -> 2.
- **Use `32ca50b0` / cooldown `32c89a20` / Eradication `32ca2af0`:** the `0xc9b` skip, `fcompp` direction ( limit <= hit count -> attack state 0x12e ), the base-id gate 0x5aa1 / 0x5ab1 and the `IsSamef( cool, -1.0 )` skip, Siege cool time ( 0x5aa1 / 0x5ab5 and `+0x96b8` ).
- **`32ca5160`:** actions 0x10 and 0x28 ( skill 0x5b6a, vslot 0x228 index 0 then 1 ); cool time id 0x5ab1. **`32c87ac0`:** base id 0x5ab1, action 0x10.
- **`32c83320` / `32c85a60` / `32c85c00`:** skill 0x5b6a with ability 0x67, classes 12 / 102; states 154 / 155 / 160 / 231-233 by name; wait ( 13 ) and 160.
- **"= March" rows:** `32c9ac00` ( table and READY types ), `32c9af70`, `32cb5cb0`, `32cb5b70` ( switch cases 2 / 4 / 6 / 10 / 12 / 14 ), `32c86af0`, `32cb5ee0`.
- **Constructor:** the `_CORE` stores against the CH1f2 / CH1h / CH1i / CH1j amendments, `+0x8fb8` 1.1 / `+0x8fbc` 1.3, and the two unmapped stores ( `+0x973c` = 7, `+0x9768` = 5.5 ).
- **X2GUUser:** the `ReservedSkillStart` chain emits one declaration / one call for any `_LIVE` combination; the `CommonSpecialAttackEventProcess` stand-in now excludes `UT_LIRE`; `ResetAnimSpeed` skips March's Agile Movement block under `_LIVE`.
- **JUNE_BUGS 570** ( legacy Delayed Fire id 3227 in lookup and use ).

Hygiene: `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with ..._CORE ..._NF --range <CH1k commit>^..<CH1k commit>` ( FAIL only for the commented-out `#define` ), `flagcheck SERV_IRUHADEV_JUNE_ENUM_NAMES --range ...`, `junediverge.py --check-all`, `enumscan.py --check` on the class files, one-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` and `X2GUUser.cpp` off and with `-DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`. Fix clear transcription errors in place ( bytepatch only ). Write "CH1k - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1m.md`, commit with explicit pathspecs.
