# June character Ara (Stage 7) - Phase CH5 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH5 review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH5 done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_UI` (defined in `KTDXLIB/Always.h`; `#error` without `_ARA_LIVE` and `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER`) |
| Log tag | `[JUNE_CHAR]` (`ARA UI cut-in ...`, `ARA UI hyper cut-in ...`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CH5 - Done (2026-11-11)"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH5 - Review (2026-11-09, Opus 5.5): PASS, no code change"
grep -n "Ara: CH5" JUNE2017_CHARACTER_PORT_CHECKLIST.md
```

## Task

Check CH5 against June from the exe bytes (Ghidra read-only) and the kom diff. Spot-check at least:
- **Table `333a8bf0`.** 12 `SkillCutIn` at `337ad900 + i * 0x30` (the reader `32bae3d0` indexes `+0x8ac8 * 0x90`, cut-in 1 at `+0x30`); strings in push order
  `33507954` `HQ_CutIn_Ara_01`, `33507934` `_11` x2, `3350790c` `_SD_01`, `335078e4` `_SD_11` x2, `335078bc` `_YR_01`, `33507894` `_YR_11` x2, `3350786c` `_AS_01`, `33507844` `_AS_11` x2.
  Sizes / positions are March's one-argument `SkillCutIn( name )` (Rena's review). The port's `s_SkillCutInSet` is that table: set 0 = 7 / 28 / 31, 1 = 118, 2 = 119, 3 = 124 (CH1m's switch).
- **UnitTemplet.** Blocks split on `AddUnitTemplet`, keyed on `unitClass`: current vs the pre-phase copy (`kom_backup_tools/data036.kom.20261111_172713`) vs June's `UnitTemplet.src.lua`. The 7 `UC_ARA_*` blocks equal June's
  text 7 of 7; the other 50 equal the pre-phase 50 of 50 (55 -> 57 blocks); key delta = `june2017_re/char_ara/ch0_unittemplet_ara.tsv`. `luasrc.py ledger --check`, `core --check`, `luabind.py check UnitTemplet`.
- **Orb retirement.** `grep -n m_hSeqEnergyBullet X2Lib/X2UnitViewerUI.cpp`: every use sits inside `#ifdef SERV_IRUHADEV_JUNE_CHAR_ARA_UI` (retired) / `#else` (March's); 7 pairs; `flagcheck` PASS.
  The viewer's `ALD_lobbyWait` / `AYR_LobbyReadyAtcion` literals and `X2SquareUnit.cpp`'s are in the dead `#else` of `SERV_IRUHADEV_JUNE_ANIMATION_NAME_TABLE` (check with `cl /P`).
- **Creation screen.** `DLG_Unit_Create_ClassChange_Button_Ara.lua` = June's bytecode (data034, `adopt-june`). The C++ is March's for classes 7 / 28 / 31 / 118 / 119: `SelectAra` (June `325548a0`: same controls, `LobbyDownLanding`,
  `AraVoice_Introduce.ogg`), `GetViewerUnitByUnitType` (`3254e2b0`: type 7 returns the Eve dummy, as March), `GetUnitStatInfomationChart` (`3254ee40`: 7 / 28 / 118 = 3,1,2 / 3,2,2 / 3,2,2; 31 / 119 = 3,3,3),
  `GetUnitClassCreateInfoTextureNameByUnitType` (`325506e0`, `..._07.tga`), the info key builders (`32550bd0`, `32550f90`: class independent). The third-line buttons (34 / 124) show and are inert until CHNa.
- **Not here.** `DLG_Transcendental_Change_Guide_Ara.lua`, the `LevelUpEvent.lua` TYPE row and the `POPUP_TR_S_DEVANAM / _Y_RAJA / _ASURA` pictures are CH3d's; 7ah is Elsword-only (n/a); the cut-in item override
  `31fd8a50` is Stage 12; the unit-select rows for 34 / 124 (`GetCharacterImageName`) and the Little Specter / Asura stat rows are CHNa's.

Hygiene: `flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARA_UI --range <CH5 commit>^..<CH5 commit>`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUAra.cpp`.
Read the user's play-test logs with `python tools/logs.py --grep "ARA UI"` and `--grep "JUNE_GFX"`. Fix clear errors in place (bytepatch only). Write "CH5 - Review" into the plan doc, update the STATUS row,
check `kickoffs/char-ara/CH6.md` (it must walk the cut-in sets 0-3 and the hyper cut-ins on 118 / 119 / 124, and the creation screen), commit with explicit pathspecs.
