# June character Ara (Stage 7) - Phase CH1q2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1q2 review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1q2 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_SPECTER`, `SERV_IRUHADEV_JUNE_CHAR_SPECTER_STATE`, `SERV_IRUHADEV_JUNE_CHAR_CHAIN_ANIM_SPEED` (all `//#define` until CH1 final; no debug define) |
| Log tag | `[JUNE_CHAR]` (`ARA SPECTER`), nothing runs until CH1 final |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CH1q2 - Done (2026-11-11)"
grep "Ara CH1q2" june2017_re/funcmap.tsv
cat june2017_re/char_ara/ch0_specter.tsv
```

## Task

Review CH1q2 against the exe bytes (`june2017_re/charport_adis.py ara <va>`, `disasm_range.py`, `dispsweep.py`; Ghidra read-only). Spot-check at least:
- **Loaders:** `320506f0` @`320524a6`-`320524e4` (key `IS_SPECTER_STATE`, out `+0x7da9`, `test al,al; jne; mov [ebx],al` = false when missing) and `32057b80`
  @`3205874f`-`3205875b` (`+0x8049`); the C++ sits in the same order (future after `APPLY_ANIM_MOVE`, now after `IS_ATTACK_STATE`); `StateParam` +0x95 against
  `operator=` / `Init` (`320820e0` not diffed: check it clears +0x95).
- **Chain anim speed:** `32051bb0`-`32051c21` (`+0x8900` = `m_fAnimSpeedFuture`, the `+0x9134` add under the item-ability bool, the chain-info test, `x +0x8908`
  then `fld1` store, `ResetAnimSpeed`); the ctor store @`320425c2` is 1.0; no other writer of +0x8908 than the ctor, the reader and `32bc8380`.
- **`32baf270` UpdateEquipCollisionData:** the `fldz; fcomp [+0x9744]; test ah,5; jp` direction, `+0x6844 < 3`, base `3201c3a0` vs vslot 0x630, the vector copy
  (`32bd7780`) plus `+0x95b4` push, the else branch's three base calls, the attack boxes after both branches.
- **`IsSpecterState`:** both class readers (`32bcee1c`, `32bd2c74`) call vslot 0x128 with 0 (now copy).

Hygiene: `python tools/flagcheck.py JUNE_CHAR_SPECTER_STATE --range <CH1q2 code commit>^..<CH1q2 code commit> --with JUNE_CHAR_CHAIN_ANIM_SPEED JUNE_CHAR_ARA_SPECTER
JUNE_CHAR_ARA_LIVE --files KTDXLIB/Always.h X2Lib/X2GameUnit.h X2Lib/X2GUUser.h X2Lib/X2GUUser.cpp X2Lib/X2GUAra.h X2Lib/X2GUAra.cpp` (FAIL only for the commented-out
defines; the `Always.h` ERROR is the older `_FP && !_LIVE` guard, explained in the Done note); `python tools/junediverge.py --check-all`; `python tools/enumscan.py
--check X2Lib/X2GUAra.cpp X2Lib/X2GUUser.cpp`; one-file `/Zs` probes of `X2GUAra.cpp` and `X2GUUser.cpp` off and `_LIVE` + `_FP` + `_SPECTER` + both user flags.
Fix clear transcription errors in place (bytepatch only). Write "CH1q2 - Review" into the plan doc, update the STATUS rows (CH1q2 done, CH1 ready once CH0b is
done), check `kickoffs/char-ara/CH1.md` (its "Amended by CH1q2" section), commit with explicit pathspecs. Full build skipped: no play-test.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1` (Opus 5.5 high).
