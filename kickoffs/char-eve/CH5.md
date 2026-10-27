# June character Eve (Stage 7) - Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH5` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1 review; may run beside CH2-CH4 (different files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_UI` |
| Log tag | `[JUNE_CHAR]` (`EVE UI`) |
| Review | `kickoffs/char-eve/CH5-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH5 - Done (2026-10-21, Sonnet 5.5)"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7t. The hyper cut-in: data, and the class hooks that pick the berserk / Lu/Ciel names (from June effect completion C3t)"
```
Then CH0's UnitTemplet note and the `CH5` rows of its tsvs.

## Task

- Replace Eve's 7 `UnitTemplet.lua` rows (data036, BOM source; lines 372-449, 664-715, 920-945, 1053-1078 at planning time) with June's text, every other row untouched; `luasrc.py refresh`; pack, ask before the copy.
- Replace the cut-in table `X2GuEve.cpp:48-85` with June's names and layout (June's `.gfx` are in data152); Electra (26) as June has it.
- The dialog / tree-UI pieces CH4 does not take.
- Hand-off: a select-screen pass of all nine March characters, then an Eve skill cut-in and a hyper cut-in.

## Amended by CH0 (2026-10-26)

- **Checklist rows** (`ch0_checklist_rows.tsv`): §7t L577-584 (hyper cut-in UnitTemplet + show option); §7t L607 (skill cut-in names -> June .gfx).
- **UnitTemplet** (`ch0_unittemplet_eve.tsv`): in all 7 rows every March key equals June's. June adds only `m_fLobbyRotationY` (all 7; March's reader ignores it) and `m_HyperCutIn` (108 / 109 / 116: `HQ_Cutin_ECN/ECE/ECB_Transcendence`; read under `SERV_IRUHADEV_JUNE_HYPER_CUT_IN_TIME`).
- **Cut-ins:** June's InitState has no Electra case either (`329ba9c0`: 5 / 18 / 19 -> set 0, 108 -> 1, 109 -> 2, 116 -> 3), so Electra keeps set 0 as in March. June names: `g4_cutin_names.tsv` l.33-39 (no June `.gfx` under March's names; June's are `HQ_CutIn_Eve_01..03 / _NM_01 / _EP_01 / _BS_01` in data152).

## Amended by CH2a2 (2026-10-27)

- `CX2GUEve::June_ShowActiveSkillCutInAndLight` (June `3297b840`, written in CH2a2) reads this phase's table and leaves two things out: the code event 0x8b3 pair (`IF_EVENT_ENABLED( 0x8b3 )` -> June `337abba0` / `337abbd0`, an even `rand()` picks the first; Aisha's `s_SkillCutInSetEvent8b3` in `X2GUArme_VioletMage.cpp` is the model) and the item override `@3297b908` (Stage 12). Add the 0x8b3 pair there together with the table; `ShowActiveSkillCutInAndLightByScript` already calls it.
