# June character Aisha (Stage 5 pilot) - Phase CH3b - Skills: Dimension Witch skills and the hyper actives

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH3b` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH3a review; not beside CH4 |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (one flag for every skill sub-phase) |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`) and `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/pilot-char-revamp/CH3b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Phase CH3 - Skills: second jobs (104, 105, 113) and hyper actives"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have" "7ae. June skill-tree ids with no March class state (from June skill system S3)"
awk -F'\t' 'NF==7 && $6=="CH3b"' june2017_re/char_arme/ch0_state_skill.tsv    # part 2: skill ids, their states, notes
awk -F'\t' 'NF==8 && $8=="CH3b"' june2017_re/char_arme/ch0_state_skill.tsv    # the states: June and March callbacks
awk -F'\t' '$6=="CH3b"' june2017_re/char_arme/ch0_class_map.tsv              # the state functions to transcribe
awk -F'\t' '$5 ~ /CH3b/' june2017_re/char_arme/ch0_checklist_rows.tsv       # checklist rows that land here
awk -F'\t' '$6=="CH3b" && $4!="june-bytes"' june2017_re/char_arme/ch0_effect_closure.tsv   # closure names not June's yet (CH0b staged the rest)
```
Then the Done notes of CH0, CH1 and the previous skill phase.

## Task

Phase CH3's work for class 113 and the hyper actives 22600-22605: 22 skill ids, 15 state functions:
- for each skill id, add its case to `SetEquippedSkillFuncBySkillSlotIndex` as June's `32cede00` has it. The state table gives the normal /
  hyper table names and the callback slots. Upgrade ids reach the base case through `32207210` (templet `+0x178`), so the labels are June's:
  raw ids with an `/// RNSI_` comment, the house convention of `X2GUUser.cpp:26228` (129 of the 130 names are not in the C++ enum);
- transcribe each listed state function whole from Ghidra under the flag (March's function stays in the `#else`);
- carry the checklist rows attached to these skills in the same change; write the March vs June damage of each skill to `MODS.md`;
- **First live hyper active** (checklist 7t): play-test the cut-in with the option on Hide. CH5 review: the hand-off also checks
  `logs.py --grep "ARME UI hyper cut-in"` names `HQ_Cutin_AEM/AVP/ADW_Transcendence` for classes 104 / 105 / 113 (June's `UnitTemplet` rows, CH5).
- skills with no state (types 1 / 5 / 8, and the ids June tests in class code: Meditation and the Body skills) are CH4's / CH1i's;
- an unmapped engine callee, or a mechanism the map does not explain, is **"this needs Opus"**: stop and say so.

Done gate: the S3 census shows 0 `no_state` for these skill ids; each skill equipped in an A or B slot casts June's state, with June's motion
and effect, and deals damage; casting adds no missing particle / mesh / texture line to `ErrorLog.txt`.

## Amended by CH0

Written by CH0 (2026-10-20): the split keeps each skill phase near the plan's ~40-state-function limit.

## From CH1i (2026-10-21)

- The enumerator `AVSI_SA_ADW_AIR_STUNNER_END` ( June script id 248 ) exists since CH1i ( `June_SetEquippedSkillLevel` names it ); register the state, do not add the enumerator again.

## From CH3a (2026-10-21)

- Energy Ring `32ce3e80` is Fire Ring's body with `EffectSet_A_ADW_Energy_Ring` (CH3a2): `31ff5440` is `CX2GUUser::DoAimingTarget`, not a camera sequencer. Register its FrameMove as a one-line member calling `June_PlayRingOnAimedTarget( L"EffectSet_A_ADW_Energy_Ring" )` and the EventProcess `JuneGenericSpecialActiveSkillEventProcess`, as the Fire Ring case does.
- Helpers now in `june2017_re/`: `funcann.py` (annotated listing) and `funccmp.py` (prove two bodies equal). The CH3a block at the end of `X2GUArme_VioletMage.cpp` is the pattern for the registration cases and the `June_*` bodies.
