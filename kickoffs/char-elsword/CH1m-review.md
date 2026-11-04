# June character Elsword (Stage 7) - Phase CH1m review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1m review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1m done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (`//#define` until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE`, `SERV_IRUHADEV_JUNE_ENUM_NAMES` (one name) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1m - Done (2026-11-04)"
grep -a "Char Elsword CH1m:" june2017_re/funcmap.tsv
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1m - Review (2026-11-02)"   # Chung's review, the worked example
```

## Task

Check CH1m's registration against June (read-only in Ghidra). In order of risk:
- **The simulation.** Fresh decompiles of `32d1f8f0` / `32d23ff0` through `june2017_re/char_elsword/ch1m_regsim.py` (once plain, once with
  `CH1M_21216=1`) must reproduce `ch1m_reg_by_class.tsv` row for row; its condition overrides (`iVar4 < 1` = the 21216 test, the shared_ptr
  releases, `iVar6` as the class register in the @941 if / else chain) are where it could be wrong. `ch1m_regcheck.py` must print `0 problems`.
- **The class tests of `32d23ff0`, against the listing:** 112 only (@310), 22 or 112 (@460), the @660 switch (1 / 10+100 / 11+101 / 112 falling into 22),
  the @824 switch (1+10+22 / 11+101 / 100 / 112) and the 100 / 101 / 112 chain; the tail call after each switch (`pcVar8` / `uVar7`) is the last row of its case.
- **The head of `32d23ff0`:** cut-in jump (1 / 10 / 11 / 22 -> 0, 100 -> 1, 101 -> 2, 112 -> 3, nothing for other classes), the inlined InitStateID
  @32d24080-32d2409e (`+0x7d00` 0x118, `+0x7d04` / `+0x4088` 0xd, `+0x408c` 0x28: is that all, and is March's climb block really absent in June?),
  the call order (`32d1f8f0`, `3202b110`, `31fe22d0`), START / WIN / LOSE slots, `InitWaitStateChange` @32d2446a, the `ESSI_EMK_WIN2` test (`+0x960c`),
  the tail vslots 0x690 / 0x684( 0 ) / `31fd8f00`, and `31fc93d0` (empty stub) left uncalled.
- **Id 162 @32d20f9c:** `GetSkillLevel( 0x52e0, 1, 0, 1 )` on `*( +0x867c ) + 0x494` with no null test, `JLE` to the revenge attack; the base-level flag
  is March's `bBasic = true`; `RNSI_A_EIS_COMPACT_COUNTER = 21216` in `X2SkillTree.h` against June's `Enum.src.lua` (March's `SI_P_EIS_COMPACT_COUNTER`
  is 1413, another skill).
- **Ids by name:** `ch1m_idmap.tsv` one-to-one both ways (154 ids); June 72 = `GetRidingStartStateID()`.
- **Shared callbacks:** the machine-gun and magic-spear states take March's `CX2GUUser::MACHINE_GUN_*` / `SUMMON_MAGIC_SPEAR_*` (CH0 `pair`, no funcmap
  row for June's `31f*` targets): spot-check two June bodies against March's.
- **Left out:** phoenix run 123-126, dance 127-129, riding 93-96 (June names 93 `ESSI_RIDING_HYPER_MODE`, 124 / 125 `..._PHOENIX_RUN`) and the empty slots
  `3202a7d0`, `32002f70`; JUNE_BUGS 525 (riding back damage without a table).
- **Always.h:** the eight new `#error` guards each name a flag the registration needs.

Hygiene: `flagcheck JUNE_CHAR_ELSWORD_LIVE --range <CH1m commit>^..<CH1m commit> --files X2Lib/X2GUElsword_SwordMan.cpp KTDXLIB/Always.h` (the default
diff misaligns the long insertion: confirm with `git diff --patience --numstat`, 1421 / 0), `junediverge.py --check-all`, `enumscan.py --check
X2Lib/X2GUElsword_SwordMan.cpp`, a one-file probe of `X2GUElsword_SwordMan.cpp` with and without `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`. Write
"CH1m - Review" into the plan doc, update the STATUS rows (CH1n becomes ready), check `kickoffs/char-elsword/CH1n.md` and commit with explicit pathspecs.
