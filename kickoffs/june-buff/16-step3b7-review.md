# June buff port - Phase 16 step 3b, part 7 - review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3b7 review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `main`) |
| Depends on | 16-step3b7 done (see its STATUS row for the commit) |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (`_DEBUG` for the first-apply lines); item 2 also uses `SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL` and `SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION` |
| Log tag | `[JUNE_BUFF]` |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16-step3b7 -- Done"
```

The `funcmap.tsv` rows tagged "Buff Phase 16-step3b7" carry the decodes (`3236df00`/`32370430`/`32369bd0`, `32773ee0`,
`3201f850`, `32028190`, `322ece70 @322ed38c`, `31e8d610`, `3201ad10 @3201ae3f`, `326c8c70`, `328d68c0`, `328d6f60`,
`3271f310`, `3204f4df`).

## Task

Review 16-step3b7 against June:
- **Item 1.** The three skill IDs and which register is the user at each call (`@3236e002` esi, `@323705a2` edi after the
  `+0x264` test, `@32369c98` esi); that 26214 maps to March's `SI_A_CHG_MARK_OF_COMMANDER` while 29024 / 30048 stay June IDs.
- **Item 2, the whole NPC block** (`32773ee0 @327740ff..32774527`) against `X2GUNPC.cpp` AttackResultByType: the owner
  (`32759f00`, `[esp+0x14]`), the power-rate order (base, then `31fd7cc0`, then `3201ad10 + 1`), the single shell name, the
  velocity, and that nothing else in the block differs from March (offsets, heights, random terms, the EVE tail).
- **Item 4.** Re-run or re-read the 0x200 case list (`3230c1a0`): 384/519/1454 are March's, and the case skills match the ones
  16-step3b3 passes to `GetSummonPowerRateBySocket`. The value is a double `( rate + 1.0 )`.
- **Item 5.** That the gate's flag really has no March source (the premium manager's 0x53b handler `326c8c70`, March's
  `CX2PremiumBuffManager::UIServerEventProc`), so leaving the term out is right.
- **Item 6.** `CalculateRealStatData` (`328d68c0`: the five calls, field + `+0x14`), the validity sum and cache key
  (`328d6f60`, `328d8880`: six floats in order), `AddStat`/`Init`, the loader key, the description branch (`3271f310 @32721b07`
  prints `+0x64`). Confirm the June item templet's sixth byte is outside March's item templet and was rightly left out.
- **Item 7.** The hook's arguments `( aggregate, templet )` and its place outside the `GetUnit()` test.

Hygiene is one command per flag:
`python tools/flagcheck.py JUNE_SOCKET_PARITY --range <first 3b7 commit>^..HEAD --with JUNE_BUFF_DEBUG`, plus the same for
`JUNE_BUFF_NEW_SKILL` and `JUNE_BUFF_CUSTOM_FUNCTION` limited to `X2Lib/X2GUNPC.cpp`. Paste the `files:` /
`in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors
under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write
"Phase 16-step3b7 -- Review" into the plan doc, update the phase's STATUS.md row, write or check the next phase's kickoff
file, and commit.
