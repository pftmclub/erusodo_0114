# June character Ara (Stage 7) - Phase CH1q review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1q review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1q done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_FP` (`//#define`, needs `_LIVE`) + `SERV_IRUHADEV_JUNE_CHAR_ARA_FP_DEBUG` (defined; this review retires it) |
| Log tag | `[JUNE_CHAR]` (`ARA FP`), nothing runs until CH1 final |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CH1q - Done (2026-11-11)"
grep "Ara CH1q" june2017_re/funcmap.tsv
cat june2017_re/char_ara/ch1q_fp_outcome.tsv
grep -E "^(IsFullHyperMode|SetFullHyperMode|GetAvailableFPCancel|CX2AraMyGageUI::GetBerserkModeMyStateImage)" tools/data/june_march_divergent.tsv
```

## Task

Review CH1q against the exe bytes (`june2017_re/charport_adis.py ara <va>`, `funcdis.py`; Ghidra read-only). Spot-check at least:
- **`32bb2b40` UseForcePower:** the jump table at `32bb2d14` / `32bb2d1c` (which of `0x6a41`-`0x6a55` take `n - 1`), the `+0x973c` test direction
  (`fldz; fcomp; test ah,5; jp`), the 27417 accumulation (`322a0090` byte into `+0x986c`) and erase (`3229ea50`), the 27419 block (gage `+0xe8` remaining
  hyper time > 0, SA 59 then SA 113 in PvP / PvE, `UpNowHpWithText`, the SA 5 roll direction `fcompp; test ah,0x41; jne`).
- **`32bc9a60` hit roll:** `dec eax; je` on DD `+8` (`AT_SPECIAL`), `+0x24 == 7`, the timer test (`test ah,0x41; jp` = return when > 0), the Filled Power
  switch (`0xb1` / `0xb2` / `0xb3` = `SA_HIT_COUNT_1-3`), the unsigned index clamp (`jb` @`32bc9beb`), and `ebp > hits` -> roll (`32070350( a, b )` = `a > b`).
- **`32babba0`:** the `+0x971c == chosen` arm, the `jns` after `inc [+0x9838]`, the store order (`vslot 0x524` before the increment).
- **Hyper effect start / end `32bb22d0` / `32bb2740`:** the branch order, the tail list `+0x95e8`, the `+0x9744 > 0 && count < 3` view-list loop, the
  transcended test (unit `+0x3c == 0` to create boosts), the base call last in the end.
- **Gauge:** `32a34980` copies `+0x110` / `+0x112` but not `+0x111`; `32e69060`'s `+0x111 != +0x110` gate and the `+0x111 := +0x110` store; `32e61a60`'s
  table against the C++ (pieces and textures); the three textures exist in the deployed koms (`data057`, `data215`).
- **Equal rows:** `UpForcePower` (the inlined copies in `32baad30` / `32baae50` / `32bab5c0` / `32bab780` / `32bc5bc0` match it, Open Mind after it), `32ba46c0`,
  `32bc19a0`, `32e5b550`.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARA_FP --range <CH1q code commit>^..<CH1q code commit> --with SERV_IRUHADEV_JUNE_CHAR_ARA_FP_DEBUG --files
X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2GageData.cpp X2Lib/X2GageUI.cpp X2Lib/X2GageUI.h` (FAIL only for the commented-out `#define`);
`python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2GageUI.cpp X2Lib/X2GageData.cpp`; one-file `/Zs` probes
of `X2GUAra.cpp`, `X2GageData.cpp`, `X2GageUI.cpp` off and `_LIVE` + `_FP` (+ `_SPECTER`). Retire the debug lines: comment out
`#define SERV_IRUHADEV_JUNE_CHAR_ARA_FP_DEBUG` in `KTDXLIB/Always.h` (one-file probe, "full build skipped: no play-test"). Fix clear transcription errors in place
(bytepatch only). Write "CH1q - Review" into the plan doc, update the STATUS rows (CH1q done, CH1q2 ready), check `kickoffs/char-ara/CH1q2.md`, commit with
explicit pathspecs.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1q2` (Opus 5.5 high).
