# June character Aisha (Stage 5 pilot) - Phase CH2b - Skills: Dark Magician and Battle Magician skills

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH2b` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH2a review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (one flag for every skill sub-phase) |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`); `NUM` lines are gone (`SERV_IRUHADEV_JUNE_SKILL_NUMERICAL_DEBUG` retired in CH2a) |
| Review | `kickoffs/pilot-char-revamp/CH2b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Phase CH2 - Skills: Violet Mage and the first jobs (classes 2, 14, 15, 23)"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have" "7ae. June skill-tree ids with no March class state (from June skill system S3)"
awk -F'\t' 'NF==7 && $6=="CH2b"' june2017_re/char_arme/ch0_state_skill.tsv    # part 2: skill ids, their states, notes
awk -F'\t' 'NF==8 && $8=="CH2b"' june2017_re/char_arme/ch0_state_skill.tsv    # the states: June and March callbacks
awk -F'\t' '$6=="CH2b"' june2017_re/char_arme/ch0_class_map.tsv              # the state functions to transcribe
awk -F'\t' '$5 ~ /CH2b/' june2017_re/char_arme/ch0_checklist_rows.tsv       # checklist rows that land here
awk -F'\t' '$6=="CH2b" && $4!="june-bytes"' june2017_re/char_arme/ch0_effect_closure.tsv   # closure names not June's yet (CH0b staged the rest)
```
Then the Done notes of CH0, CH1 and the previous skill phase.

## Task

Phase CH2's work for classes 15 and 23: 24 skill ids, 34 state functions:
- for each skill id, add its case to `SetEquippedSkillFuncBySkillSlotIndex` as June's `32cede00` has it. The state table gives the normal /
  hyper table names and the callback slots. Upgrade ids reach the base case through `32207210` (templet `+0x178`), so the labels are June's:
  raw ids with an `/// RNSI_` comment, the house convention of `X2GUUser.cpp:26228` (129 of the 130 names are not in the C++ enum);
- transcribe each listed state function whole from Ghidra under the flag (March's function stays in the `#else`);
- carry the checklist rows attached to these skills in the same change; write the March vs June damage of each skill to `MODS.md`;
- skills with no state (types 1 / 5 / 8, and the ids June tests in class code: Meditation and the Body skills) are CH4's / CH1i's;
- an unmapped engine callee, or a mechanism the map does not explain, is **"this needs Opus"**: stop and say so.

Done gate: the S3 census shows 0 `no_state` for these skill ids; each skill equipped in an A or B slot casts June's state, with June's motion
and effect, and deals damage; casting adds no missing particle / mesh / texture line to `ErrorLog.txt`.

## Amended by CH0

Written by CH0 (2026-10-20): the split keeps each skill phase near the plan's ~40-state-function limit.

## From CH1h (2026-10-20)

The class map now lists `32cdcac0` (Energy Drain FrameMove, state 207, thunk `32cf2be0`, registered in `32cede00` @`32ceff5d`), which CH1h found under the default bucket.

## From the CH1i review (2026-10-21)

- `June_UpdateEquipViewListInfo` (`32cc9950`, vslot `0x644`) drops the view-list change while transformed. June queues it (`3207b5d0`, with 8-byte objects and a third int); port the queue with TransformStart / End.

## From the CH2a review (2026-10-21)

- The skill cases go into the same switch CH2a used, `June_SetEquippedSkillFuncHead` (before `default:`), not March's `SetEquippedSkillFuncBySkillSlotIndex` body.
- Reuse `June_AVSI_SKILL_EventProcess` for every June state whose EventProcess is a clone of `32cbe6a0` (compare the bytes); `June_CopyStateCallbacks` is `329cc2c0`; `June32481fa0` and `JUNE_ARME_CH2A_ANY_KEY_ZXC` are file-local in the CH2a block.
- June's skill-ability lookups pass a PvP flag (`322cc1a0`) in some places and 0 in others: copy whichever June has (the 3-argument `GetSkillAbilityValue`).
- Damage numbers for `MODS.md` come from the tsv (x0.01); with `NUM` retired, prove damage in game (hits / kills in `offline_server.log`).
