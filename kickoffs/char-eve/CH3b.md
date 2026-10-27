# June character Eve (Stage 7) - Phase CH3b - Skills: Code: Empress (109)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH3b` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH3a review (CH0: no helper phase needed). One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-eve/CH3b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Nasod and the first jobs (classes 5, 18, 19, 26)" "Phase CH3 - Skills: second jobs (108, 109, 116), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have" "7ae. June skill-tree ids with no March class state (from June skill system S3)"
```
Then CH0's and CH1's Done notes, and the `CH3b` rows of `june2017_re/char_eve/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Scope: the class-109 skill ids not in an earlier phase. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's addresses); its attached rows (7c alpha tags, 7u names, 7ai BFI arguments, any 7b block CH0 put here); its effect closure (a name found only now is staged byte-exact and added to `ch0_assets.tsv`). Upgrade ids reach their base case through `JuneSkillIDByUpgrade`: check the templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Eve: CH3b)`.
- Hand-off: the unit and class, each skill id and its A / B slot, what June's motion and effect look like.

## Amended by CH0 (2026-10-26)

- **Functions: 41** (`awk -F'\t' '$6=="CH3b"' june2017_re/char_eve/ch0_class_map.tsv`; helper 1, state-fn 40).
  June addresses: `329666a0` `32966b00` `32966b90` `32966bb0` `32967110` `32967190` `329671e0` `32967250` `32968860` `32968890` `32968970` `3296f640` `3296f680` `3296fc00` `3296fc60` `3296fca0` `3296fd00` `329721e0` `329722c0` `329723a0` `32972540` `32972670` `32976c10` `32976e10` `32978280` `32978320` `3298c900` `3298cc60` `3298ce20` `3298d090` `3298d2f0` `32990060` `32990220` `3299a140` `3299a400` `3299ecb0` `3299ed30` `3299f060` `329ae470` `329ae4f0` `329cc290`.
- **States: 15** (`awk -F'\t' '$8=="CH3b"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_SI3_AT_GENOCIDE_RIPPER`, `ENSI_SI3_AT_GENOCIDE_RIPPER_HYPER`, `ENSI_SA_EAT_SPACE_WRENCH`, `ENSI_SA_EAT_SPACE_WRENCH_HYPER`, `ENSI_SA_EEP_ELECTRONIC_FIELD`, `ENSI_A_EEP_SPIT_FIRE`, `ENSI_A_EEP_SPIT_FIRE_HYPER`, `ENSI_SA_EPP_ASSUALT_SPEAR`, `ENSI_SA_EPP_ASSUALT_SPEAR_HYPER`, `ENSI_A_ECE_TESLA_SHOCK`, `ENSI_A_EEP_SUMMON_TO_MIND`, `ENSI_SA_EEP_TESLA_FLARE`, `ENSI_SA_EEP_ELECTRONIC_THUNDER`, `ENSI_SA_ECE_REASON_PRIDE`, `ENSI_SA_ECE_REASON_PRIDE_ACTION`.
- **Skill ids: 16** (`awk -F'\t' 'NF==7 && $6=="CH3b"' june2017_re/char_eve/ch0_state_skill.tsv`): 25017 RNSI_SA_ECE_GENOCIDE_RIPPER, 25018 RNSI_SA_ECE_ASSUALT_SPEAR_BURST, 25019 RNSI_SA_ECE_SPACE_WRENCH, 25020 RNSI_SA_ECE_ELECTRONIC_FIELD, 25045 RNSI_SA_EEP_TESLA_FLARE, 25046 RNSI_SA_EEP_ELECTRONIC_THUNDER, 25059 RNSI_SA_ECE_GENOCIDE_RIPPER_UPGRADE, 25060 RNSI_SA_ECE_ASSUALT_SPEAR_BURST_UPGRADE, 25061 RNSI_SA_ECE_SPACE_WRENCH_UPGRADE, 25062 RNSI_SA_ECE_ELECTRONIC_FIELD_UPGRADE, 25208 RNSI_A_ECE_TESLA_SHOCK, 25209 RNSI_A_ECE_SPIT_FIRE_FLUSH, 25222 RNSI_A_EEP_SUMMON_TO_MIND, 25229 RNSI_A_ECE_TESLA_SHOCK_UPGRADE, 25230 RNSI_A_ECE_SPIT_FIRE_FLUSH_UPGRADE, 25236 RNSI_SA_ECE_REASON_PRIDE.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7c L323-343 (167 alpha-tag sites 32982623..329c84f8); §7h L410 (USEFUL_VALUE rows).
- **7c alpha-tag sites inside these functions: 4** (`c10a_alpha_sites.tsv`, by containing function).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31d170a0` CKTDXDeviceHolder::XMeshReadyInBackground (KTDXLIB/KTDXDeviceHolder.cpp:115) (confirmed).

## Amended by june-enum-names N1 (2026-10-26)

- Register `ENSI_SA_ECE_REASON_PRIDE_ACTION` in `X2GuEve.h`'s June block (March numbering, `/// June script id 242`, like l.152-166). Then switch `June_SetEquippedSkillLevel`'s `JUNE_CH1L_ENSI_SA_ECE_REASON_PRIDE_ACTION` (June's raw 242, `X2GuEve.cpp:44556`) to it: the state map is keyed by March ids, so 242 misses the entry (June `329c8a00` case 0x6294, `31fd89b0( 0xf2, ... )`).

## Amended by CH1l3 (2026-10-26)

- **Functions: 42.** Adds `3299ba70` (Electronic Field summon helper, NPC 1622 `NUI_SI_SA_EEP_ELECTRONIC_FIELD`; BFI `0x1ba` = `BFI_BUFF_OPHELIA` 442 and BTI `0x19d` = `BTI_BUFF_OPHELIA` 413 are not registered in C++ yet:
  register them under `SERV_IRUHADEV_JUNE_ENUM_NAMES` as CH1l3 did). CH0 gave it to CH1l3 because of a function-boundary slip (`3299ebd0` runs on into `3299ecb0`); its callers are `329ae4f0` and `3299ecb0`, both CH3b.

## Amended by CH1n (2026-10-27)

- **Case bodies go in `CX2GUEve::June_SetEquippedSkillFuncHead`** (`X2GuEve.cpp`, block CH1n), keyed on June's raw id as `329c8a00` switches (both base and `_UPGRADE` ids are cases; no `JuneSkillIDByUpgrade`), each case `return false`. A case whose hyper takes every callback (`329cc2c0`) calls `CX2GUUser::June_CopyStateCallbacks( hyperStateData, normalStateData )` (one home since CH1n, flag `JUNE_USER_COPY_STATE_CALLBACKS`); `ch0_skill.tsv` column `calls` names the cases.

## Amended by CH3a0 (2026-10-28)

- **Modifier tag 15 is ported.** Push it with the vector overload `g_pX2Game->GetEffectSet()->AddDamageEffectModifier( hHandle, 15, D3DXVECTOR3( s, s, s ), bAdd )` (June `321dad70`; take `bAdd` from the listing's last push), not the float form. `CX2EffectSet::ApplyDamageEffectModifiers` case 15 applies it (JUNE_BUGS 453). The tag has no enum name in June's `Enum.lua`, so it stays the number `15` with a one-line comment.
