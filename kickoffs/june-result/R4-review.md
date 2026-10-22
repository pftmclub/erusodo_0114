# June dungeon result - Phase R4 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-result R4 review` |
| Plan doc | `JUNE_DUNGEON_RESULT_PHASES.md` (branch `main`) |
| Depends on | Phase R4 done and play-tested (clear a training at Camilla's school) |
| Flag | `SERV_IRUHADEV_JUNE_RESULT_LEGACY` |
| Log tag | `[JUNE_RESULT] LEGACY` (`python tools/logs.py --tag JUNE_RESULT`) |

## Read first

```sh
python tools/docsection.py JUNE_DUNGEON_RESULT_PHASES.md "Phase R4 - June's legacy result state (conditional)" "R4 - Done (2026-10-23, Sonnet 5.5 high; flag \`SERV_IRUHADEV_JUNE_RESULT_LEGACY\`, log \`[JUNE_RESULT] LEGACY\`)"
```

Then `kickoffs/june-result/R4.md` ("Amended by R0") and `june2017_re/result_r0_map.tsv` rows tagged R4.

## Task

R4 found the training result equal to March apart from two small deltas (the Done note lists what was compared). The review checks that
"equal" claim, because a port that compared too little would pass play-test and still differ:
- **Training `OnFrameMove` `324c1020`** (999 lines): re-read the ED_BG (0x10) .. END (0x1a) cases against `X2StateTrainingResult.cpp:17-491`
  (the level-up block, the item loop `15 .. 38`, the EXP bar `%d / %d (%.1f%%)`, the BONUS_EXP timer 0.4 s, the END timer start).
  June's END is 0x1a, March's is 0x19 (June has one more enum member after `DRAO_PICKED_MVP_PLAYER`): confirm nothing outside the
  training state compares past `DRAO_EXP_BAR`.
- **Setup `32513d30`**: the part R4 compared by structure, not line by line: the `Dungeon_Info` block (picture 2/3/4, dungeon texture,
  difficulty strings), the unit viewer block and the "other users" loop. Spot-check 2 of them in Ghidra.
- **The ctor's failure path** (`32515670 @32515894-32515a7d`): `SendID( EGS_RESULT_SUCCESS_REQ )` + `AddServerPacket( ACK )`; check
  0x10c / 0x10d are those two ids in June's `EventID_Client.h` numbering (R2 matched them to `324d84c0`).
- **`3250f6c0` (base OnFrameMove) is not ported**: R0 closed state 0xD (no June code requests it). Confirm the training route never calls it
  (the training vtable `3349dc24` slot 1 is `324c1020`).
- **Data:** `DLG_Dungeon_Result_Reward_Window3.lua` is June's `ljconv` bytes (2967 B); `kom.py status data034` in sync after the copy.

Hygiene: `python tools/flagcheck.py JUNE_RESULT_LEGACY --range <R4 commit>^..<R4 commit>`; paste `files:` / `in this flag's files:` /
`RESULT:`. Read the play-test lines with `python tools/logs.py --tag JUNE_RESULT`. Fix clear transcription errors under the same flag;
hand design decisions back to the user. Write "R4 - Review" into the plan doc, update the R4 row in STATUS.md, and commit. The next
phase is Z (`kickoffs/june-result/Z.md`): make sure it lists the `_LEGACY` revert line.
