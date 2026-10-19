# June character Aisha (Stage 5 pilot) - Phase CH2 - Skills: Violet Mage and the first jobs (classes 2, 14, 15, 23)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH2` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1 review |
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

(CH0 fills this in.)
