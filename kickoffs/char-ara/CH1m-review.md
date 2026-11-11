# June character Ara (Stage 7) - Phase CH1m review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1m review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1m done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (`//#define`), `_CORE` (defined), `_FP` / `_SPECTER` (`//#define`) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CH1m - Done"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH1m - Review"
awk -F'\t' '$7=="CH1m"' june2017_re/char_ara/ch0_class_map.tsv
```

## Task

Review CH1m's registration against fresh Ghidra decompiles and listings of `32bcf4a0`, `32bca580`, `32bcced0`, `31fdd800`, and the exe bytes. Spot-check at least:
- **Generator:** rerun `python june2017_re/char_ara/ch1m_gen.py <dir>` and diff its `.inc` output against the `_LIVE` branches of `InitStateCommon` /
  `InitStateByUnitClass`; `ch1m_idmap.tsv` 138 ids, one member each both ways; the three shared-target callbacks (`32bbac50`, `32b9cf00`, `32bc6720`).
- **Wrapper:** the cut-in byte table `32bcfa1c` (0 / 7 / 28 / 31 -> 0, 118 -> 1, 119 -> 2, 124 -> 3, 34 no write) and its jump table targets; START / WIN / LOSE
  callbacks (thunks `329cb6e0` / `329cb6f0` / `329cb1b0`, `329cb700` / `320718d0`, `329cb710`); `PUSH 0x82` @32bcf902; the tail 0x690 / 0x684( 0 ) / `31fd8f00`.
  The order change (June: body, `3202b110`, per-class, `31fe22d0`; C++: body, per-class, `InitStateCommonRandom`) is harmless only if no id meets.
- **Body:** ids 23 / 155 and their callbacks (vslots 0x600 / 0x604 on Ara vtable `33505314`); the bounding place; the four chain lookups are each for the id
  registered right after; the left-out ids.
- **Per class:** the fall-through order of `32bcced0` (118 -> 28, 119 -> 31, 124 -> 34) and the four Specter chain lookups.
- **`31fdd800`:** `CMP dword ptr [EAX],0x7` = `UT_ARA` on templet `+0xaf0`; `+0x3c` = `IsUnitTranscended`; `RET 0x8`.
- **`ParseCommonRandomState`:** June `31feaac0` is called by the base Init for every class; `GetUnitClassName( 34 / 124 )` = the script's table names.
- **JUNE_BUGS 598-600:** the exe strings and script lines they cite.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --range <CH1m commit>^..<CH1m commit> --files X2Lib/X2GUAra.cpp` (FAIL only for the
commented-out `#define`); `_CORE --files X2Lib/X2GUUser.h X2Lib/X2GUUser.cpp`; `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check
X2Lib/X2GUAra.cpp X2Lib/X2GUUser.cpp`; one-file `/Zs` probes of `X2GUAra.cpp` off, `_LIVE`, `_LIVE` + `_FP` + `_SPECTER`. CH1m left no `_DEBUG` define. Fix clear
transcription errors in place (bytepatch only). Write "CH1m - Review" into the plan doc, update the STATUS rows (CH1m done, CH1n ready), check
`kickoffs/char-ara/CH1n.md`, commit with explicit pathspecs.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1n` (Opus 5.5 high).
