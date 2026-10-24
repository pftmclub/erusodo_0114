# June bottom bar - Phase B4 review (covers B1-B4)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-bottombar B4 review` |
| Plan doc | `JUNE_BOTTOMBAR_PHASES.md` (branch `main`) |
| Depends on | B4 done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_BOTTOMBAR_IDS` / `_LAYOUT` / `_GAUGES` / `_BUTTONS` |
| Log tag | `[JUNE_BOTTOMBAR]` (`python tools/logs.py --tag JUNE_BOTTOMBAR`) |

## Read first

```sh
python tools/docsection.py JUNE_BOTTOMBAR_PHASES.md "Context" "Scope fence" "Rules" "B1 - Done (2026-10-24, Sonnet 5.5)" "B2 - Done (2026-10-24, Sonnet 5.5)" "B3 - Done (2026-10-24, Sonnet 5.5)" "B4 - Done (2026-10-24, Sonnet 5.5)" "Review (Opus 5.5, high)"
```

and `june2017_re/bottombar_buttons.tsv`, `june2017_re/bottombar_functions.tsv`.

## Task

Review B1-B4 against the June functions in the tsv, line by line in Ghidra (`32476050` cases `0x66` / `0x95` / `0xa7` / `0x3e8` / `0x3e9`, `324708c0` `@32470bd4-32470c45`,
`3245bb70`, `3245cba0`, `32473810`, `32475480`). Things B4 could get wrong:
- **Cash:** the order of June's refusals; that `XS_PVP_ROOM` really is June's state 8 (March's enum agrees with June's up to 0xb: 9 = PVP_GAME, 0xb = DUNGEON_GAME); that dropping the
  `NEW_SKILL_TREE_UI` skill-tree hide is right (June has none); that June's `ToggleCashShop` (`32465fc0`) starts with the field-name / buff-name close that stays in our case.
- **System:** `MT_DUNGEON_MATCH_START_REQUEST` as June's `31f38e90( 10 )` (March's `HandleMsgByESC` uses 9 = `MT_PARTY_READY_TO_PLAY_DUNGEON_REQUEST` for the same function);
  the March private-bank test and the old fall-through into the next case are gone under the flag.
- **Event:** the toggle order, `31be34b0` = the three devices' `ReleaseAllButtonsAndKeys` (without `DeactivateApp`'s `m_bActivated = false`).
- **Guild / Warp:** Guild is a gate + STUB (June's `325ab110` is a June-only guild UI); Warp needs no code (only the duel gate differs, reads 0). Confirm.
- Cash message swap: the `SMUCM_JUNE_CASH_MAINTENANCE = 215` id (June's 180 is March's `SMUCM_WARP_DEST_SELECT`); the stand-in reads 1.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py JUNE_BOTTOMBAR_BUTTONS --range <B4 commit>^..<B4 commit>`, and the same for `_IDS`, `_LAYOUT`, `_GAUGES` over their phases' commits.
Paste the `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and deleted line.

Check the deployed exe and the B1 koms; read the play-test lines (`logs.py --tag JUNE_BOTTOMBAR`). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`);
hand design decisions back to the user. Write "Review" under the plan doc's Review heading, update the STATUS rows, check `Z.md`, commit.
