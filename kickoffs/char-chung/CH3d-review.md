# June character Chung (Stage 7) - Phase CH3d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH3d review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH3d done (its Done note, commit and the user's play-test + "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_TRANSCEND` (+ `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL hyper active registered`), `[JUNE_TRANSCEND]` (`IMAGE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH3d - Done (2026-11-03)"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH3d - Review (2026-10-28, Opus 5.5): PASS, 1 fix (transcended 160by160 pictures)"
```

## Task

Check CH3d against June (read-only; read the listings, not the decompiles). In order of risk:
- **The five hyper-state functions** `32bdb550` / `32bdb610` / `32bdb6a0` (Init preload lists, compared string by string with March: 15 / 11 / 14 entries), `32be8bb0` (EventProcess: future-anim end -> `USI_JUMP_DOWN` with `y -= LINE_RADIUS * 1.5` when off the ground, else `USI_WAIT`; then walk / dash / skill cancels and `CommonEventProcess`) and `32c08fa0` (FrameMove: 0.01 s reset + power rate + stop the big-missile set; 4.094 s summoner damage effect, `EffectSet_Satelite_Rain_Call_BigThing` with `IsHyperState()` and the power rate). The Done note says all five equal March: confirm 2 against the listing.
- **Picture rows.** `FUN_33374620` row pushes for `TR_I_PALADIN` / `TR_D_CHASER` / `TR_T_TROOPER`: LARGE and GRAY in `DLG_UI_Common_Texture99_NEW` / `_GRAY`, MEDIUM / ICON / PARTY in Texture100, STATE in Texture97, unit slot `list_TR_*` in `GUI_character_01` (+ `_gray`). Piece names are in the staged TETs (UTF-16, upper case). Check the code's `X2Data.cpp` switch against them.
- **Allow-list and level-up row.** `Handlers_Quest.cpp` `TranscendCheck` (insert-only guard before Eve's `if`), `LevelUpEvent.lua` `TYPE2` against June's `TYPE5` of event 6, `luasrc.py ledger --check`.
- **7ao leftovers** (Done note says already in place): hyper effect start `32c063d0` (`HyperModeBuffEffectStart`), `GetHyperModeTableName` Chung cases, `TRANSCENDENTAL_SKILL_DATA` ids 26407 / 26412 / 26416 / 26418 / 26420, the four base helpers.
- **JUNE_BUGS 514** (Chung's `ScreenShot_04` sheet lacks two pieces in June too).
- **Play-test results** (units 43 Iron Paladin / 50 Deadly Chaser / 18 Tactical Trooper, transcended, hyper active in slot A-5): the three `hyper active registered` lines, the cut-in, `[JUNE_TRANSCEND] IMAGE class 110 / 111 / 117 type ...` lines, the Endless Chase check on unit 50 (`shooting star ... endless chase range` needs `_SKILL_DEBUG` defined again), `ErrorLog.txt`.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_CHUNG_TRANSCEND --range <CH3d commit>^..<CH3d commit>`, `junediverge.py --check-all`, `enumscan.py --check` on the changed files, `skillicons.py check 26000-26999` (2 June-side misses, JUNE_BUGS 514). Write "CH3d - Review" into the plan doc, update the STATUS rows (CH4a unblocks) and commit.
