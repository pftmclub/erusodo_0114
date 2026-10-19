# June character Aisha (Stage 5 pilot) - Phase CH2 - Skills: Violet Mage and the first jobs (classes 2, 14, 15, 23)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH2` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | split by CH0: run CH2a, then CH2b (CH1 review first) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (or CH0's split) |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`) and `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/pilot-char-revamp/CH2-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Violet Mage and the first jobs (classes 2, 14, 15, 23)"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have" "7ae. June skill-tree ids with no March class state (from June skill system S3)"
```
Then CH0's and CH1's Done notes, and the `ch2` rows of `june2017_re/char_arme/ch0_state_skill.tsv` and `ch0_checklist_rows.tsv`.

## Task

Do "Phase CH2": transcribe each listed June state function from Ghidra (the map gives the address), whole, under the flag.
- Carry each skill's attached checklist rows in the same change.
- Edit CP949 files with `bytepatch.py` only.
- Retire `SERV_IRUHADEV_JUNE_SKILL_NUMERICAL_DEBUG` once the `NUM use` line is seen.
- Write the March vs June damage of each skill to `MODS.md`.
- If a function needs a judgment the map does not settle (a new mechanism, an unmapped callee), stop and hand it to the user as "this needs
  Opus" rather than guess.

## Amended by CH0

Written by CH0 (2026-10-20). **Split: run `CH2a` (Violet Mage + High Magician: 26 skill ids, 49 state functions) and then `CH2b` (Dark
Magician + Battle Magician: 24 ids, 34 functions)**; their kickoff files carry the filters. This file stays as the shared brief.
- The skill's line decides the phase: a skill class 2 has is CH2a; otherwise the first of 14 / 15 / 23 that has it. Types 1 / 5 / 8 are CH4.
- Moved out of CH2: checklist 7b (`32cccc70` + the D8 Fitness -> Magical Staff swap) is passive, so it is CH4 (whole function in one phase).
  7p's forced `SetStopTime` is dead in June (`32cd5d60` unreferenced): nothing to port.
- Kept here: 7u (Blaze Step MEMO names in CH2a, Death Field names in CH2b), 7ai BFI arguments (`32cd57c0` Binding Circle, CH2a; `32cd7bf0`
  Death Field, CH2b), section 1's first live `NUM use` line (CH2a).
