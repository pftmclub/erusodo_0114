# June character Chung (Stage 7) - Phase CH1d0 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1d0 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1d0 done (its Done note and commit) |
| Flag | none (CH1d0 changed no client source) |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1d0 - Done (2026-10-31)"
awk -F'	' '$1 ~ /^(31fd4290|31fd4340|32071a10|32071a30)/' june2017_re/funcmap.tsv
grep -n "^ClearAnimEventTimerOneshot" tools/data/june_march_divergent.tsv
```

## Task

CH1d0 found that June `31fd4290` / `31fd4340` are March's `CX2GUUser::ClearAnimEventTimerOneshotNow` / `Future` (`X2GUUser.cpp:48871` / `:48879`), plus
June-only resets of the Lu/Ciel second-body anims `+0x8e88` / `+0x8e84`, and ported nothing. Check it:
- Decompile both in Ghidra and confirm the offsets: `+0x87d4` = `m_pXSkinAnim` (now), `+0x87d0` = `m_pXSkinAnimFuture`; the riding-pet getters
  `32071a10` (`+0x238`) / `32071a30` (`+0x23c`) are the pet's now / future anim (`X2RidingPet.h:537-538` member order).
- Confirm `+0x8e88` / `+0x8e84` are NULL for Chung (only unit type 10 loads the second pair, funcmap row `31fe5860` R12), so the `out` verdict holds and no
  `replace` row or body is owed. Spot-check one Chung caller (e.g. `32bf75b0`) calls it where March's matching state calls `ClearAnimEventTimerOneshotNow()`.
- Confirm `kickoffs/char-chung/CH1d.md` "Amended by CH1d0" names the March symbols and the backlog Stage 8 dual-body bullet carries the second-body reset.

Hygiene: `python tools/junediverge.py --check-all` (`RESULT: PASS`). No build unless the review changes source.
Write "CH1d0 - Review" into the plan doc, update the STATUS row (CH1d becomes ready), check `kickoffs/char-chung/CH1d.md`, commit with explicit pathspecs.
