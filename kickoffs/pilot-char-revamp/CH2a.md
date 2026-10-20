# June character Aisha (Stage 5 pilot) - Phase CH2a - Skills: Violet Mage and High Magician skills

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH2a` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1 review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (one flag for every skill sub-phase) |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`) and `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/pilot-char-revamp/CH2a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Phase CH2 - Skills: Violet Mage and the first jobs (classes 2, 14, 15, 23)"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have" "7ae. June skill-tree ids with no March class state (from June skill system S3)"
awk -F'\t' 'NF==7 && $6=="CH2a"' june2017_re/char_arme/ch0_state_skill.tsv    # part 2: skill ids, their states, notes
awk -F'\t' 'NF==8 && $8=="CH2a"' june2017_re/char_arme/ch0_state_skill.tsv    # the states: June and March callbacks
awk -F'\t' '$6=="CH2a"' june2017_re/char_arme/ch0_class_map.tsv              # the state functions to transcribe
awk -F'\t' '$5 ~ /CH2a/' june2017_re/char_arme/ch0_checklist_rows.tsv       # checklist rows that land here
awk -F'\t' '$6=="CH2a" && $4!="june-bytes"' june2017_re/char_arme/ch0_effect_closure.tsv   # closure names not June's yet (CH0b staged the rest)
```
Then the Done notes of CH0, CH1 and the previous skill phase.

## Task

Phase CH2's work for classes 2 and 14 (with every skill class 2 also has): 26 skill ids, 49 state functions:
- for each skill id, add its case to `SetEquippedSkillFuncBySkillSlotIndex` as June's `32cede00` has it. The state table gives the normal /
  hyper table names and the callback slots. Upgrade ids reach the base case through `32207210` (templet `+0x178`), so the labels are June's:
  raw ids with an `/// RNSI_` comment, the house convention of `X2GUUser.cpp:26228` (129 of the 130 names are not in the C++ enum);
- transcribe each listed state function whole from Ghidra under the flag (March's function stays in the `#else`);
- carry the checklist rows attached to these skills in the same change; write the March vs June damage of each skill to `MODS.md`;
- **First live numerical damage** (checklist section 1): show `[JUNE_SKILL] NUM use ...` for an Aisha skill, then retire `SERV_IRUHADEV_JUNE_SKILL_NUMERICAL_DEBUG` in this phase.
- skills with no state (types 1 / 5 / 8, and the ids June tests in class code: Meditation and the Body skills) are CH4's / CH1i's;
- an unmapped engine callee, or a mechanism the map does not explain, is **"this needs Opus"**: stop and say so.

Done gate: the S3 census shows 0 `no_state` for these skill ids; each skill equipped in an A or B slot casts June's state, with June's motion
and effect, and deals damage; casting adds no missing particle / mesh / texture line to `ErrorLog.txt`.

## Amended by CH0

Written by CH0 (2026-10-20): the split keeps each skill phase near the plan's ~40-state-function limit.

## From CH1h (2026-10-20)

The class map now lists four skill-state callbacks CH1h found under the default bucket: `32cccb40` (Meteo Shower Start, thunk `32cf2600`), `32cdb5e0` (its FrameMove, `32cf29f0`), `32cdbb70` (its End, `32cf2a00`; registered in `32cede00` @`32cee3c0`, states 217 / 218) and `32cdc4a0` (Fire Road FrameMove, `32cf2b30`, @`32cefbe1`). The fire-count members they read are the plain C++ ones: `+0x96f8` `m_iMaxFireCount`, `+0x9704` `m_iMaxFireCountByLoopState`, `+0x9710` `m_iElapsedFireCount`, `+0x971c` `m_fUseMPConsume`, `+0x9728` `m_fFireGap` (protected in June).
