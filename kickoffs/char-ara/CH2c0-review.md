# June character Ara (Stage 7) - Phase CH2c0 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH2c0 review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH2c0 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_STACK` (defined; no debug define) |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CH2c0 - Done (2026-11-11)"
grep "CH2c0" june2017_re/funcmap.tsv
```

## Task

Review CH2c0 against the exe bytes (Ghidra read-only; `disassemble_function` and `june2017_re/charport_adis.py ara <va>` beat the decompile).
Spot-check at least:
- **`3229da40`:** the find is by templet id only (`322ae0a0`, the same predicate as `EraseBuffTempletFromGameUnit`); the signed test
  `add ecx, eax; test ecx, ecx; jle` against `static_cast<int>( GetAccumulationCountNow() ) + iDelta_`; the erase path when the sum is <= 0.
- **`3236a2f0`:** the size check plus find on the UCM map at `+0x50` (key 0); `fild` / `fadd float` / `fstp float` = `+= static_cast<float>( iDelta_ )`; the
  displayer loop (vslot 1 with `( unit, this )`, then `+0xc = 1`); the `+0xa8` icon push arguments (`+0x34`, `+0x68`, `+0x84`, `+0xa0`, `+0xa4`, `+0x64`,
  the count); confirm there is no behavior re-apply and no vslot `0x788` call.
- **`32bbb050` / `32bbb720`:** the eight times of each (`charport_consts.py --fn 32bbb050 32bbb720`; `1.0` is an immediate push in `32bbb720`); vslot
  `0x240` = `HaveSkillBuff( 27418, 0, true )` before every time; `3229a3d0( 27418, 0, 1 )` then `3229da40( BTI, -1 )`; the `PlayEffectSet` argument list
  (only name, unit, NULL, the hyper byte and `GetPowerRate` differ from the defaults: -1.0, (1,1,1), false, three zero vectors, the trailing 1.0); that
  the March half of both functions matches June unchanged (Wolf Break 1.1 / 1.35 / 1.63 / 2.0, `Break2` / `Break3` / `_Special`, +-100; Open the Hell
  0.01 with the `GetEffectSet` null test).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARA_STACK --range <CH2c0 commit>^..<CH2c0 commit> --files KTDXLIB/Always.h X2Lib/X2GUAra.cpp
X2Lib/X2GameUnit.cpp X2Lib/X2GameUnit.h X2Lib/X2BuffTemplet.cpp X2Lib/X2BuffTemplet.h X2Lib/X2SkillTree.h` (`X2SkillTree.h` is OTHER = `JUNE_ENUM_NAMES`);
`python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2GameUnit.cpp X2Lib/X2BuffTemplet.cpp`; one-file `/Zs`
probes of the three `.cpp`. Fix clear transcription errors in place (bytepatch only). Write "CH2c0 - Review" into the plan doc, update the STATUS
rows (CH2c0 done; CH2c ready), commit with explicit pathspecs. Full build skipped: no play-test.

**Nothing to play-test.** Next: `/x2-phase char-ara CH2c` (Sonnet 5.5 high).
