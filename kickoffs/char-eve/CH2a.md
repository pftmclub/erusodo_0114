# June character Eve (Stage 7) - Phase CH2a - Skills: Nasod (5) (CH2a2 takes Exotic Gear)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH2a` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1 review (CH0: no helper phase needed). One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-eve/CH2a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Nasod and the first jobs (classes 5, 18, 19, 26)" "Phase CH3 - Skills: second jobs (108, 109, 116), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have" "7ae. June skill-tree ids with no March class state (from June skill system S3)"
```
Then CH0's and CH1's Done notes, and the `CH2a` rows of `june2017_re/char_eve/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Scope: the skill ids class 5 has (class 18 is CH2a2). For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's addresses); its attached rows (7c alpha tags, 7u names, 7ai BFI arguments, any 7b block CH0 put here); its effect closure (a name found only now is staged byte-exact and added to `ch0_assets.tsv`). Upgrade ids reach their base case through `JuneSkillIDByUpgrade`: check the templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Eve: CH2a)`.
- Hand-off: the unit and class, each skill id and its A / B slot, what June's motion and effect look like.

## Amended by CH0 (2026-10-26)

- **Functions: 29** (`awk -F'\t' '$6=="CH2a"' june2017_re/char_eve/ch0_class_map.tsv`; state-fn 29).
  June addresses: `329663c0` `32966440` `329664f0` `329665b0` `32966900` `32966910` `32966960` `32966990` `329674d0` `329674e0` `3296ef60` `3296f020` `3296f0e0` `3296f8c0` `3296f9a0` `3296fa90` `32973150` `32973220` `329732b0` `32973310` `32973370` `32978050` `32978080` `329780b0` `3297cba0` `32989b60` `3298b9b0` `32993020` `329b1e20`.
- **States: 9** (`awk -F'\t' '$8=="CH2a"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_SI2_ILLUSION_STINGER`, `ENSI_SI1_DIMENSION_LINK`, `ENSI_SI1_DIMENSION_LINK_HYPER`, `ENSI_SI3_DIMENSION_LINK_GUARDIAN`, `ENSI_SI_A_EN_CONVERSION`, `ENSI_SI_A_EN_CONVERSION_HYPER`, `ENSI_SI_A_EN_MEGA_ELECTRONBALL`, `ENSI_SI_A_EN_MEGA_ELECTRONBALL_HYPER`, `ENSI_A_EVE_PHOTON_BLINK`.
- **Skill ids: 12** (`awk -F'\t' 'NF==7 && $6=="CH2a"' june2017_re/char_eve/ch0_state_skill.tsv`): 25000 RNSI_SA_EVE_DIMENSION_LINK_SENTRY, 25001 RNSI_SA_EVE_ILLUSION_STINGER, 25002 RNSI_SA_EVE_DIMENSION_LINK_GUARDIAN, 25030 RNSI_SA_EVE_DIMENSION_LINK_SENTRY_UPGRADE, 25031 RNSI_SA_EVE_ILLUSION_STINGER_UPGRADE, 25032 RNSI_SA_EVE_DIMENSION_LINK_GUARDIAN_UPGRADE, 25200 RNSI_A_EVE_PHOTON_BLINK, 25203 RNSI_A_EVE_CONVERSION, 25207 RNSI_A_EAT_MEGA_ELECTRONBALL, 25213 RNSI_A_EVE_PHOTON_BLINK_UPGRADE, 25216 RNSI_A_EVE_CONVERSION_UPGRADE, 25219 RNSI_A_EAT_MEGA_ELECTRONBALL_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7 L274 (SetUserSummonedNPCInfo per-case flags); §7 L281 (SetMultiplyPowerRate_LUA in Nasod_Watch_Eve / SI_HA_FERDINAN); §7 L282 (engine readers keyed by March skill ids (25000, 25601)); §7c L323-343 (167 alpha-tag sites 32982623..329c84f8); §7ai L806-819 (BFI call arguments, 3 s skill-name string).
- **7c alpha-tag sites inside these functions: 9** (`c10a_alpha_sites.tsv`, by containing function).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31d170a0` CKTDXDeviceHolder::XMeshReadyInBackground (KTDXLIB/KTDXDeviceHolder.cpp:115) (confirmed); `31d006b0` CKTDGParticleSystem::CreateSequence( obj, name, const D3DXVECTOR3& pos, ... ) inline overload (KTDGParticleSystem.h:2007) (confirmed); `31cfe7b0` CKTDGParticleSystem::CParticleEvent_Rotate::CParticleEvent_Rotate (KTDGParticleSystem.h) (confirmed); `31fdbf20` CX2GUUser skill-memo check (nearest March fn GetEqippedSkillMemo, X2GUUser.cpp:6737) (candidate); `32274a90` CX2GameUnit::SetEnableAttackBox( const WCHAR*, bool ) (X2GameUnit.cpp:1551) (confirmed).

## Amended by CH1n (2026-10-27)

- **Case bodies go in `CX2GUEve::June_SetEquippedSkillFuncHead`** (`X2GuEve.cpp`, block CH1n), keyed on June's raw id as `329c8a00` switches (both base and `_UPGRADE` ids are cases; no `JuneSkillIDByUpgrade`), each case `return false`. A case whose hyper takes every callback (`329cc2c0`) calls `CX2GUUser::June_CopyStateCallbacks( hyperStateData, normalStateData )` (one home since CH1n, flag `JUNE_USER_COPY_STATE_CALLBACKS`); `ch0_skill.tsv` column `calls` names the cases.
