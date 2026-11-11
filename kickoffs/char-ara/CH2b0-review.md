# June character Ara (Stage 7) - Phase CH2b0 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH2b0 review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH2b0 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_LOCKON`, `SERV_IRUHADEV_JUNE_NPC_ESCAPE_FROM_LOCK_ON` (both defined; no debug define) |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CH2b0 - Done (2026-11-11)"
grep "CH2b0" june2017_re/funcmap.tsv
grep "^| 604 " JUNE_BUGS.md
```

## Task

Review CH2b0 against the exe bytes (Ghidra read-only; `disassemble_function` beats the decompile here). Spot-check at least:
- **`ProcessLockOn` `3200fa20`:** the filter order `@3200faf1`-`3200fbe0` (team, slot 0x3c8 + `32757e60( 0x835 )` only for `+0x264 == 1`, slot 0x178 with
  `[esp+0x24] = 1`, the `+0xc04` / `+0x20` HP inline, class 100-102 / 106, `+0x8fb6`); the `[esp+0x17]` flag (set `@3200fa8b` before the loop, cleared
  `@3200fcb6`, tested `@3200fffd`); the `322cc1a0` branch (`fcompp` + `test ah,0x41; jz` = skip when distance > `+0x30`; the three pushes = owner pos,
  owner slot 0x16c, candidate pos); states 9 / 0x17 and the decision to test `XS_PVP_GAME` only; both `CanDown` argument lists against the 9-arg overload.
- **`320701e0`:** the C++ `GetDistanceFromDirLineXZ` against `@320701e0`-`3207034b` (which stack slot holds which vector, the y = 0 from the
  reused arg slot, `fadd 180.0; fchs; fadd B.y`, the float stores between steps).
- **`32b4cf30`:** `+0x30` = 99999.0; the NPC key `327a0fa0 @327abb70` and the ctor clear `@327633f2`.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_ARA_LOCKON --range <CH2b0 commit>^..<CH2b0 commit> --with JUNE_NPC_ESCAPE_FROM_LOCK_ON --files KTDXLIB/Always.h
KTDXLIB/KTDXCommonFunc.h X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2GUNPC.cpp X2Lib/X2GUNPC.h X2Lib/X2UnitManager.h` (the NPC flag's `Always.h` ERROR is the guard
evaluation, explained in the Done note); `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUNPC.cpp`;
one-file `/Zs` probes of `X2GUAra.cpp` and `X2GUNPC.cpp`. Fix clear transcription errors in place (bytepatch only). Write "CH2b0 - Review" into the plan
doc, update the STATUS rows (CH2b0 done; CH2b ready once CH2a is done, which it is), commit with explicit pathspecs. Full build skipped: no play-test.

**Nothing to play-test.** Next: `/x2-phase char-ara CH2b` (Sonnet 5.5 high).
