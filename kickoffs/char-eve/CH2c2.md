# June character Eve (Stage 7) - Phase CH2c2 - Skills: Electra (26) Sweep Rolling and Giga Stream (split from CH2c by CH0)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH2c2` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH2c review (its helpers are mapped by CH1j20). One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-eve/CH2c2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Nasod and the first jobs (classes 5, 18, 19, 26)" "Phase CH3 - Skills: second jobs (108, 109, 116), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have" "7ae. June skill-tree ids with no March class state (from June skill system S3)"
```
Then CH0's and CH1's Done notes, and the `CH2c` rows of `june2017_re/char_eve/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Scope: Sweep Rolling (25021 / 25039, states READY / CHARGE / FIRE / END) and Giga Stream (25025 / 25065). For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's addresses); its attached rows (7c alpha tags, 7u names, 7ai BFI arguments, any 7b block CH0 put here); its effect closure (a name found only now is staged byte-exact and added to `ch0_assets.tsv`). Upgrade ids reach their base case through `JuneSkillIDByUpgrade`: check the templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Eve: CH2c2)`.
- Hand-off: the unit and class, each skill id and its A / B slot, what June's motion and effect look like.

## Amended by CH0 (2026-10-26)

- **Functions: 26** (`awk -F'\t' '$6=="CH2c2"' june2017_re/char_eve/ch0_class_map.tsv`; helper 2, state-fn 24).
  June addresses: `32967620` `32967690` `329676b0` `329676c0` `329676d0` `329676e0` `32967700` `32967710` `32967730` `32973770` `329737c0` `32973a30` `32973ce0` `3297dbd0` `3297dcc0` `3297de10` `32993c40` `32994230` `329944b0` `32994540` `32994610` `329946f0` `3299b2e0` `329a1b90` `329a6f20` `329a7250`.
- **States: 5** (`awk -F'\t' '$8=="CH2c2"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_SA_EPR_SWEEP_ROLLING_READY`, `ENSI_SA_EPR_SWEEP_ROLLING_CHARGE`, `ENSI_SA_EPR_SWEEP_ROLLING_FIRE`, `ENSI_SA_EPR_SWEEP_ROLLING_END`, `ENSI_SA_GIGA_STREAM`.
- **Skill ids: 4** (`awk -F'\t' 'NF==7 && $6=="CH2c2"' june2017_re/char_eve/ch0_state_skill.tsv`): 25021 RNSI_SA_EEL_SWEEP_ROLLING_OVER_CHARGE, 25025 RNSI_SA_EEL_GIGA_STREAM, 25039 RNSI_SA_EEL_SWEEP_ROLLING_OVER_CHARGE_UPGRADE, 25065 RNSI_SA_EEL_GIGA_STREAM_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7c L323-343 (167 alpha-tag sites 32982623..329c84f8).
- **7c alpha-tag sites inside these functions: 16** (`c10a_alpha_sites.tsv`, by containing function).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31d170a0` CKTDXDeviceHolder::XMeshReadyInBackground (KTDXLIB/KTDXDeviceHolder.cpp:115) (confirmed); `31fce8b0` user (candidate); `33086b6a` D3DXPlaneFromPointNormal (d3dx9 import thunk) (confirmed).
