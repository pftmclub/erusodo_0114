# June character Eve (Stage 7) - Phase CH2c - Skills: Electra (26) (CH2c2 takes Sweep Rolling and Giga Stream)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH2c` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH2b review (CH0: no helper phase needed). One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-eve/CH2c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Nasod and the first jobs (classes 5, 18, 19, 26)" "Phase CH3 - Skills: second jobs (108, 109, 116), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have" "7ae. June skill-tree ids with no March class state (from June skill system S3)"
```
Then CH0's and CH1's Done notes, and the `CH2c` rows of `june2017_re/char_eve/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Scope: the class-26 skill ids not in 5 / 18 / 19, except Sweep Rolling (25021 / 25039) and Giga Stream (25025 / 25065), which are CH2c2. Electronic Move is Battle Seraph-only (116): CH3c has it (CH0 correction). For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's addresses); its attached rows (7c alpha tags, 7u names, 7ai BFI arguments, any 7b block CH0 put here); its effect closure (a name found only now is staged byte-exact and added to `ch0_assets.tsv`). Upgrade ids reach their base case through `JuneSkillIDByUpgrade`: check the templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Eve: CH2c)`.
- Hand-off: the unit and class, each skill id and its A / B slot, what June's motion and effect look like.

## Amended by CH0 (2026-10-26)

- **Functions: 27** (`awk -F'\t' '$6=="CH2c"' june2017_re/char_eve/ch0_class_map.tsv`; state-fn 27).
  June addresses: `32967500` `32967510` `32967520` `32967540` `32967560` `32967590` `32967600` `32967f50` `32967f80` `329733b0` `32973620` `32973670` `329736a0` `32973700` `329751b0` `329752a0` `3297d3f0` `3297d730` `3297e990` `329936e0` `32993700` `32993730` `329a5d80` `329a5ee0` `329a67b0` `329aa730` `329b7370`.
- **States: 4** (`awk -F'\t' '$8=="CH2c"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_A_EPR_SPECTRO_EL_CRYSTAL`, `ENSI_SA_EPR_PARTICLE_RAY`, `ENSI_SA_EPR_PHOTON_FLARE`, `ENSI_A_EBS_KUGELBLITZ`.
- **Skill ids: 8** (`awk -F'\t' 'NF==7 && $6=="CH2c"' june2017_re/char_eve/ch0_state_skill.tsv`): 25022 RNSI_SA_EEL_PHOTON_FLARE, 25023 RNSI_SA_EEL_PARTICLE_RAY, 25024 RNSI_SA_EEL_KUGELBLITZ, 25042 RNSI_SA_EEL_PHOTON_FLARE_UPGRADE, 25063 RNSI_SA_EEL_PARTICLE_RAY_UPGRADE, 25064 RNSI_SA_EEL_KUGELBLITZ_UPGRADE, 25202 RNSI_A_EVE_SPECTRO_EL_CRYSTAL, 25215 RNSI_A_EVE_SPECTRO_EL_CRYSTAL_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7c L323-343 (167 alpha-tag sites 32982623..329c84f8); §7n L477 (user-sync class-skill float).
- **7c alpha-tag sites inside these functions: 3** (`c10a_alpha_sites.tsv`, by containing function).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31d170a0` CKTDXDeviceHolder::XMeshReadyInBackground (KTDXLIB/KTDXDeviceHolder.cpp:115) (confirmed); `31fc89a0` user (candidate); `31fce670` user (candidate); `31fce6d0` user (candidate).

## Amended by CH1d (2026-10-26)

- `329a5ee0` / `329a67b0` call `m_EveElectraSystem.June_CreatePlane` (`329a1150`, CH1d); never March's `CreatePlane`.
