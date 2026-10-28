# June character Eve (Stage 7) - Phase CH4a review - Passives

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH4a review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH4a done and play-tested (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_PASSIVE` (+ `_DEBUG`, retired here) |
| Log tag | `[JUNE_CHAR]` (`EVE PASSIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH4a - Done (2026-10-28, Opus 5.5)"
```
Then the `(CH4a)` rows at the end of `june2017_re/funcmap.tsv` and JUNE_BUGS row 462.

## Task

Review the CH4a port against the listings (Ghidra for `3297f290`, `32978370`, `32894e40`; `python june2017_re/funcdis.py --func <va>` for the uncarved `329b3b50` / `3297a440`). Where it could be wrong:
- **`3297f290` block by block.** Each skill id, its `GetSkillLevel( id, true, 0, true )` / `GetUserSkillTemplet( id, true )` / `GetEquippedActiveSkillTemplet( id, true )` choice, each ability index read through `s0a_templet_keys.tsv` (0x22 / 0x23 / 0x80 / 0x8b / 0x1b / 0x12 / 0x33 / 1 / 0x1f / 0x52 / 0x49 / 0x2b / 0x7f / 0x20 / 0x19 / 0xaa / 0x4f / 0x94), and the offset -> March member map (walk / dash / jump / dash-jump `+0x4870..`, critical `+0x4ca8` option value, hyper charge `+0x63ec`, Electra `+0x9834 / +0x9831 / +0x997c / +0x9978`, `+0x95ec` charging booster, `+0x97d0 / +0x97d8` Oberon Guard). Confirm each with a ported reader, not the stride alone.
- **What June drops.** March's electronball up / master blocks, the memo 5 fix; confirm no June code writes `m_fAttackPowerRateElectronBall` / `m_fForceDownValueRateElectronBall` and that their ctor 1.0 is June's.
- **Charging Booster.** The 15 s set only when the object is built (`@3297f7f5`, inside the null test).
- **`32978370`.** Argument unused, memo type 15, `+ 0.0`.
- **Set / UnSet.** The jump table (50 / 51 / 105 -> 0 / 1 / 2, 106), `3297a440`'s 961 case and that `320017d0`'s UID argument is the owner's `GetUnitUID()`; `3229e160` = `EraseBuffTempletFromGameUnit`.
- **`32894e40`.** Force / type / ignore order per bar, rate vs seconds, the clears, `GetUserSkillTemplet( id, true )` against the inline swap of bar B; JUNE_BUGS 462 is real.
- **Hand-off check:** units 59 / 60 / 61 and the log lines the hand-off lists.

Retire `SERV_IRUHADEV_JUNE_CHAR_EVE_PASSIVE_DEBUG` (define and lines). Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_PASSIVE --with JUNE_CHAR_EVE_PASSIVE_DEBUG --range <first CH4a commit>^..HEAD`; `enumscan.py --check X2Lib/X2GuEve.cpp X2Lib/X2UserSkillTree.cpp`. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions to the user. Next phase kickoff `CH4b.md` exists.
