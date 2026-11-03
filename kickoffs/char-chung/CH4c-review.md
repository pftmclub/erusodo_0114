# June character Chung (Stage 7) - Phase CH4c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH4c review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH4c done (its Done note, commit and the user's play-test + "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG PASSIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH4c - Done (2026-11-03)"
grep -n "(CH4c)\|^32a34020\|^31fcd470" june2017_re/funcmap.tsv
grep -n "^| 518 " JUNE_BUGS.md
```

## Task

Check CH4c against June (read-only; read the listings with `june2017_re/funcdis.py`, not only the decompiles). In order of risk:
- **`DoExtraAttack` `3236dbf0`** against the `_PASSIVE` branch in `X2BuffTemplet.cpp`: `+0x264` == 0 is `m_GameUnitType` (`GUT_USER`); the test order
  ( `31fcf3e0( 0x65a3 )`, then `32007c40( 5, 0x65a3, 1, &f )`, then vslot 0x1cc `( 0 )` ) so the random draw happens only when both pass; `fld f; fcompp;
  test ah,1` = `roll <= f`; strings `33490230` / `33490210`. Confirm `32007c40` = the file-local `TryGetAcquiredSkillAbilityValue` copy (Chung's CH1g2 body)
  and that 26033's `NUMERICAL_DATA4` `SA_EFFECTIVE_RATE` is what `TryGetSkillAbilityValue` returns at level 1.
- **Metabolism Boost**, five sites: `322fbc60 @322fbe08` / `@322fd495`, `32a34020 @32a34142` / `@32a3422c` (Fury Guardian only, JUNE_BUGS 518), `31fcd470`
  (raw ability, no `- 1`), and `32bdc890`'s `- 1 + 5e-8` in `X2GUChung.cpp`. `32890620( 0x6720, 1, 0, 1 )` = `GetSkillLevel( id, true, 0, true )`;
  `32206450( 0x6720, 1 )` = the one-argument `GetSkillTemplet`; `32206e50( 0x37, level, 0 )` = SA 55 `SA_ITEM_SPECIAL_ABILITY_REL` (`enumprobe.py`).
- **The rest of `32a34020` and `31fcd470`** equal March apart from these blocks (the divergence rows claim it): MP up, soul, MP %, the egg 0x11170 term.
- **`MemoConditionChung`**: n/a holds (slot 1 = base `31fd4450`, slot 2 = base parse); nothing reads `+0x7d10` beyond the S3f site.
- **Play-test results**: the `CHUNG PASSIVE metabolism boost` and `big bang stream grenade` lines named in the Done note's "In game" row, `ErrorLog.txt`, 0 EXCEPTION.

Retire `SERV_IRUHADEV_JUNE_CHAR_CHUNG_PASSIVE_DEBUG` (comment out its `#define`) if the logs were read.
Hygiene: `python tools/flagcheck.py JUNE_CHAR_CHUNG_PASSIVE --with JUNE_CHAR_CHUNG_PASSIVE_DEBUG --range <CH4c commit>^..<CH4c commit> --files KTDXLIB/Always.h X2Lib/X2Game.cpp X2Lib/X2GageData.cpp X2Lib/X2GUChung.cpp X2Lib/X2BuffTemplet.cpp`
(and `--files X2Lib/X2GUUser.cpp` alone), `junediverge.py --check-all`, `enumscan.py --check` on the five `.cpp` files. Write "CH4c - Review" into the plan doc,
update the STATUS row (CH6 waits on CH3d / CH4c / CH5 reviews) and commit.
