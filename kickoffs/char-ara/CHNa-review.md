# June character Ara (Stage 7) - Phase CHNa review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CHNa review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CHNa done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CLASS_PATH` (class-generic), `SERV_IRUHADEV_JUNE_CHAR_ARA_CLASS` (Ara rows), `SERV_IRUHADEV_JUNE_CHAR_ARA_CLASS_DEBUG` (retire it here); all in `KTDXLIB/Always.h`, `#error` without `SERV_IRUHADEV_JUNE_ENUM_NAMES` |
| Log tag | `[JUNE_CHAR]` (`ARA CLASS gauge class <c> -> CX2AraGageData`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CHNa - Done (2026-11-11)"
awk -F'\t' '$7 ~ /CHNa/' june2017_re/char_ara/ch0_classn.tsv
```

## Task

Check CHNa against June from the exe bytes (Ghidra read-only; the image tables are written by static initializers, read them from
`unpacked_x2.exe` or the initializer disassembly, not `.data`). Spot-check at least:
- **Gauge** `X2GageManager.cpp`: 34 / 124 labels in Ara's arm = June `3248e290` case list (7, 0x1c, 0x1f, 0x22, 0x76, 0x77, 0x7c).
- **Compatibility** `X2Unit.h` `GetX2UnitClassCompatibility`: 34 -> {7, 34}, 124 -> {7, 34, 124} = June `31ed6500` (decompile lines ~174 / ~463).
- **Creation screen** `GetThirdLineAraClassTypeByUnitClassType`: 7 / 34 / 124 = June `3234b390` table `337a1660`, Ara block `+0x230`, filled by `3234afb0`
  (`337a1894` = 7, `337a18b0` = 0x22, `337a18c4` = 0x7c). `GetUnitStatInfomationChart` 34 / 124 = (HIGH, LOW, LOW) = June `3254ee40` 3,1,1.
- **Portraits** `X2Data.cpp`: index 55 / 56 in all nine arrays and the switch = June `31ee0de0` rows 0x39 / 0x3a (type bases `3378f200`, `33785b80`, `33788120`,
  `3378a6c0`, `3378cc60`, `337917a0`, `337835e0`, `33793d40`, `337962e0`; record 0x70 B, first pair file / piece). The decoder cross-check: rows 0x30 / 0x31
  (Little Devil / Yama Raja) equal March's 48 / 49 in every table. Every index 0..54 entry above must still be compiled in (the arrays are positional).
- **Class path** `X2SocketItem.cpp` bound `UC_AIN_ANGEL` (202) = June `3271a810` `( class - 1 ) < 0xca`; `CX2Unit::GetClassLevel` = June `3234c170`.
  Divergence rows `GetClassLevel`, `GetThirdLineAraClassTypeByUnitClassType`, `AddSkillLevelUpSocketGroup_LUA` in `tools/data/june_march_divergent.tsv`.
- **Data** `CorrelateUnitClassTemplet.lua` = June's bytecode (data036, adopt record in `june2017_re/`); data215 has June's `DLG_UI_Common_Texture101_NEW` / `_GRAY`
  (`.tga` + `.TET`). `tools/createchar.py` has 34 / 124 and their parents.
- **Not here.** `RotateYSelectCharacterType`'s templet reader (Lu/Ciel NU4; 34 / 124 show at the default angle); the transcended 124 portrait pair in the
  five-argument `GetCharacterImageName` (CH3d); job change (CHNb); Little Specter / Asura skills (CH2d / CH3c).

Hygiene: `flagcheck.py <each flag> --range <CHNa commit>^..<CHNa commit>` (the ERROR row on a file without that flag is the pre-existing compound
`#if defined( SERV_BALANCE_FINALITY_SKILL_EVENT ) || ...` the tool cannot evaluate; each file must PASS under its own flag), `junediverge.py --check-all`,
`enumscan.py --check` on the changed lines. Read the user's play-test logs with `python tools/logs.py --grep "ARA CLASS"`. Retire `_CLASS_DEBUG` (comment out the
`#define`; the build is not awaited, per the 2026-11-03 rule). Fix clear errors in place (bytepatch only). Write "CHNa - Review" into the plan doc, update the
STATUS row, check `kickoffs/char-ara/CHNb.md`, commit with explicit pathspecs.
