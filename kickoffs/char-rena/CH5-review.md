# June character Rena (Stage 7) - Phase CH5 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH5 review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH5 done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_UI` (defined in `KTDXLIB/Always.h`; `#error` without `_RENA_LIVE` and `SERV_IRUHADEV_JUNE_HYPER_CUT_IN_TIME`) |
| Log tag | `[JUNE_CHAR]` (`RENA UI cut-in ...`, `RENA UI hyper cut-in ...`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH5 - Done (2026-11-09)"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH5 - Review"
grep -n "Rena: CH5" JUNE2017_CHARACTER_PORT_CHECKLIST.md
grep -n "Char Rena CH5" june2017_re/funcmap.tsv
```

## Task

Check CH5 against June from the exe bytes (Ghidra read-only) and the kom diff. Spot-check at least:
- **Table `333aad30`.** 12 `SkillCutIn` at `337ae6e8 + i * 0x30`; strings in push order `3351afc8` `HQ_CutIn_Rena_01`, `afa4` `_02`, `af80` `_03`, `af58` `_WS_01` x3,
  `af30` `_GA_01` x3, `af08` `_NW_01` x3. The port's `s_SkillCutInSet` is that table, set 0 = 3 / 12 / 13 / 24, 1 = 102, 2 = 103, 3 = 114.
- **Pair `333aaba0`.** `337ae688` = `HQ_CutIn_Rena_CASH_21` (`3351aedc`), `337ae6b8` = `_CASH_22` (`3351aeb0`); reader `32c86890` picks `337ae688` on an even `rand()`
  after `31e5a860( 0x8b3 )`; the item override `31fd8a50` is left out (Stage 12).
- **InitState `32cadc90` set index.** CH1m already ported the switch (`X2GULire_ElvenRanger.cpp` InitState, sets 0 / 1 / 2 / 3, sub index 1 / 2 / 0 for 12 / 13 / rest);
  confirm it against the jump table once: no index can reach past set 3.
- **UnitTemplet.** Split current, pre-phase (`kom_backup_tools/data036.kom.20261109_201125`) and June's `UnitTemplet.src.lua` into blocks on `AddUnitTemplet`, keyed on `unitClass`:
  the 7 `UC_LIRE_*` blocks have June's keys and values (13 key changes: `ch0_unittemplet_rena.tsv`), the other 48 are pre-phase's. `luasrc.py ledger --check`, `core --check`,
  `luabind.py check UnitTemplet`. The three `HQ_Cutin_R{WS,GA,NW}_Transcendence.gfx` are in data195; `STR_ID_4783` / `_4784` / `_17429` are in `StringID_def2.h` / `_def7.h`.
- **Routed, not here.** `DLG_Transcendental_Change_Guide_Lena.lua`, the `LevelUpEvent.lua` TYPE row and the `POPUP_TR_W_SNEAKER` / `_G_ARCHER` / `_N_ARCHER` pictures are CH3d's.
  7ah (`UpdateElswordSkillSlotEffect`) is Elsword-only (n/a, CH0); the gauge pieces were CH1q2's.

Hygiene: `flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_UI --range <CH5 commit>^..<CH5 commit>`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`.
Read the user's play-test logs with `python tools/logs.py --grep "JUNE_CHAR RENA UI"` and `--grep "JUNE_GFX"`. Fix clear errors in place (bytepatch only). Write "CH5 - Review" into the plan
doc, update the STATUS row, check `kickoffs/char-rena/CH6.md` (it must walk the cut-in sets 0-3, indices 0-2, and the hyper cut-ins on 102 / 103 / 114), commit with explicit pathspecs.
