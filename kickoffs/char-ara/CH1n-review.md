# June character Ara (Stage 7) - Phase CH1n review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1n review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1n done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (`//#define`), `_CORE` (defined), `SERV_IRUHADEV_JUNE_ENUM_NAMES` (defined) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CH1n - Done"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH1n - Review"
awk -F'\t' '$7=="CH1n"' june2017_re/char_ara/ch0_class_map.tsv
```

## Task

Review CH1n against fresh Ghidra decompiles of `32bd09d0`, `32bd34c0`, `32d3ed30`, `32babb50` and the exe bytes (`june2017_re/charport_consts.py` `read_va`,
`june2017_re/disasm_range.py`). Spot-check at least:
- **Slot head:** `slot < 0 || 4 < slot`; the four tables `334fea50` / `334fea64` / `334fea78` / `334fea8c` = `_CONST_GUUSER_`; `31fd2af0` dropped; every `+0x11`
  store (head and `@32bd2778`) is `IsSkillSlotState`; `31fca8e0( id, 1 )`.
- **Memo exit `@32bd0af5`:** `31fdbcc0`, `32206450( memo, 1 )`, the `0x6a56` / `0x6a6c` compare is on the memo id (not the skill id), `32babb50` = March's
  `ASI_A_ALD_CONVERSION_MANA_CreateState` (table string, three callback targets), 27222 / 27244 = June `Enum.src.lua`.
- **Types:** jump / byte tables of the type switch (5 buff, 6 / 7 / 0xd-0x10 active, 0xc throw on the raw id 900, rest exit); buff thunks `329ccd90` /
  `329cba40`, throw thunks `329ccda0` / `329ccdb0`; the strings `ASI_COMMON_BUFF` (both slots) and `ASI_THROW_ITEM`. Both switches take the raw id (no `32207210`).
- **Hyper split left to CH3d:** the CH3d amendment's ids and helpers match the decompile; the default `@32bd271e` = March's `default:` on Wind Wedge.
- **`32bd34c0` head:** `+0x867c` null test, `328905b0` then `32890720( swapped, 1 )`, raw-id switch.
- **`32d3ed30`:** the C++ match order and the skill-tree null tests; its two June callers.
- **Callbacks:** rerun `python june2017_re/char_ara/ch1n_triage.py`; it must reproduce `ch1n_user_callback_triage.tsv` byte for byte (174 rows, 0 missing).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_ARA_CORE --range <CH1n commit>^..<CH1n commit> --files
X2Lib/X2GUAra.cpp X2Lib/X2ComboAndSkillChainSystem.cpp KTDXLIB/Always.h` (FAIL only for the commented-out `#define`); `SERV_IRUHADEV_JUNE_ENUM_NAMES --files
X2Lib/X2SkillTree.h`; `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2ComboAndSkillChainSystem.cpp`;
one-file `/Zs` probes of `X2GUAra.cpp` off, `_LIVE`, `_LIVE` + `_FP` + `_SPECTER` and `X2ComboAndSkillChainSystem.cpp` off / `_LIVE`. CH1n left no `_DEBUG`
define. Fix clear transcription errors in place (bytepatch only). Write "CH1n - Review" into the plan doc, update the STATUS rows (CH1n done, CH1o ready),
check `kickoffs/char-ara/CH1o.md`, commit with explicit pathspecs.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1o` (Opus 5.5 high).
