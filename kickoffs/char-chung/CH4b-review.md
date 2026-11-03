# June character Chung (Stage 7) - Phase CH4b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH4b review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH4b done (its Done note, commit and the user's play-test + "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG PASSIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH4b - Done (2026-11-03)"
grep -n "(CH4b)" june2017_re/funcmap.tsv
grep -n "^| 517 " JUNE_BUGS.md
```

## Task

Check CH4b against June (read-only; read the listings, not only the decompiles). In order of risk:
- **`GetActualMPConsume` `32bed9d0`** against the June branch in `X2GUChung.cpp`: base call `( id, level, 1 )`, the 26215 / 26238 switch and the `+0x97bc` /
  `+0x97c0` empty test (= `m_vecRemoteMortarNPCUID`), `32007b10( 0x1b, 0x6734 )` (SA 27 = `SA_MP_CONSUME_REL`, `enumprobe.py`), the clamp, the berserk block
  (`+0x9704` protected > 0, templet `31fca8e0( id, 1 )` type `+4` not 0xc / 0xd), the specialization-A block (gage `+0xc04` -> `+0x110` = now ball count;
  `DE E1` = `1 - spec A`), and `3234d840` on `g_pData +0x68 -> +0x90`.
- **`GetActualCoolTime` `32be8e50`**: NULL test, base `31fd1920` (confirm its third argument is never read), `+0x9710` > 0 in berserk, types 12 / 13, the `< 0` clamp.
- **`SetDamageExceptionProcess` `32bf28f0`**: base first; DD `+0x590` = `m_eUsedSkillID`, `+0x38` / `+0x3c` = `damage.fPhysic` / `fMagic`, `+0` = `damageType`;
  `32610fb0( 0x1f )` = `IsAttackTrigger( DTT_SIEGE_SHELLING )` (June `Enum.lua` 31); the `+0x3f8` protected add = `fHitAddMP +=`; `+0x5c4` = `m_iAppliedSkillID`;
  `32007c40( 1, 0x6661, 1, &f )` argument order, `33437778` = 2.0; `32610e90( 0, f )` = `SetBuffFactorTimeValue` (op 0 `+=`, `32d41960( 1, v, op )`). Confirm
  JUNE_BUGS 517 (both branches latent) still holds.
- **Soul gain**: `3208e1b0` = `fld1; ret`; the `DamageReact` call `@32049d8a` sits after the 0x73c hook and multiplies `[esp+0x30]` = `fIncreaseSoulRate`.
- **Mark of Commander readers**: the four sites now read 26214 (`0x6666`) as June pushes.
- **Routing**: Crest of Time / death prevention went to `char-add` (stage7-plan row 8): confirm no Chung state casts BTI 436 / 926.
- **Play-test results**: the `CHUNG PASSIVE` lines named in the Done note's "In game" row, `ErrorLog.txt`, 0 EXCEPTION.

Retire `SERV_IRUHADEV_JUNE_CHAR_CHUNG_PASSIVE_DEBUG` (comment out its `#define`) if the logs were read.
Hygiene: `python tools/flagcheck.py JUNE_CHAR_CHUNG_PASSIVE --with JUNE_CHAR_CHUNG_PASSIVE_DEBUG --range <CH4b commit>^..<CH4b commit> --files KTDXLIB/Always.h X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h X2Lib/X2GUUser.h X2Lib/X2DamageManager.h X2Lib/X2BuffTemplet.cpp X2Lib/X2GUNPC.cpp`
(and `--files X2Lib/X2GUUser.cpp` alone), `junediverge.py --check-all`, `enumscan.py --check` on the five `.cpp` / `.h` files. Write "CH4b - Review" into the plan doc,
update the STATUS row (CH4c becomes ready) and commit.
