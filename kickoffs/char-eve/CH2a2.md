# June character Eve (Stage 7) - Phase CH2a2 - Skills: Exotic Gear (18) (split from CH2a by CH0)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH2a2` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH2a review (its helpers are mapped by CH1j20 / CH1l40). One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-eve/CH2a2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Nasod and the first jobs (classes 5, 18, 19, 26)" "Phase CH3 - Skills: second jobs (108, 109, 116), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have" "7ae. June skill-tree ids with no March class state (from June skill system S3)"
```
Then CH0's and CH1's Done notes, and the `CH2a` rows of `june2017_re/char_eve/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Scope: the skill ids class 18 has that class 5 does not. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's addresses); its attached rows (7c alpha tags, 7u names, 7ai BFI arguments, any 7b block CH0 put here); its effect closure (a name found only now is staged byte-exact and added to `ch0_assets.tsv`). Upgrade ids reach their base case through `JuneSkillIDByUpgrade`: check the templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Eve: CH2a2)`.
- Hand-off: the unit and class, each skill id and its A / B slot, what June's motion and effect look like.

## Amended by CH0 (2026-10-26)

- **Functions: 38** (`awk -F'\t' '$6=="CH2a2"' june2017_re/char_eve/ch0_class_map.tsv`; helper 3, state-fn 35).
  June addresses: `32966450` `329667e0` `32966820` `32966870` `32966880` `32966890` `32966bd0` `32966c10` `32966c30` `32966c70` `32967000` `32967030` `3296efa0` `3296efe0` `3296f6c0` `3296f700` `3296f740` `3296f760` `3296f780` `3296f7c0` `3296f800` `3296f820` `3296f840` `3296fd40` `3296fda0` `3296fe20` `3296fef0` `3296ff50` `3296ffd0` `32971910` `32971c40` `3297b840` `3298f1c0` `3298f220` `3298f8f0` `3299d3f0` `329cd890` `329cd9c0`.
- **States: 11** (`awk -F'\t' '$8=="CH2a2"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_SI3_GENERATE_BLACKHOLE`, `ENSI_SI3_GENERATE_BLACKHOLE_HYPER`, `ENSI_SI1_EG_HORNET_STING`, `ENSI_SI1_EG_HORNET_STING_HYPER`, `ENSI_SI2_EG_EXPLOSION_IMPACT`, `ENSI_SI2_EG_EXPLOSION_IMPACT_HYPER`, `ENSI_SI3_EG_QUEENS_THRONE`, `ENSI_A_EEG_METAL_DUST_AURA`, `ENSI_A_EEG_METAL_DUST_AURA_HYPER`, `ENSI_SA_ENS_ATOMIC_BLASTER`, `ENSI_SA_ENS_ATOMIC_BLASTER_HYPER`.
- **Skill ids: 12** (`awk -F'\t' 'NF==7 && $6=="CH2a2"' june2017_re/char_eve/ch0_state_skill.tsv`): 25003 RNSI_SA_EEG_QUEENS_THRONE, 25004 RNSI_SA_EEG_EXPLOSION_IMPACT, 25005 RNSI_SA_EEG_ATOMIC_BLASTER_PLASMAGUN, 25006 RNSI_SA_EEG_HORNET_STING_EXPLOSION, 25007 RNSI_SA_EEG_GENERATE_BLACKHOLE, 25037 RNSI_SA_EEG_QUEENS_THRONE_UPGRADE, 25040 RNSI_SA_EEG_EXPLOSION_IMPACT_UPGRADE, 25049 RNSI_SA_EEG_ATOMIC_BLASTER_PLASMAGUN_UPGRADE, 25050 RNSI_SA_EEG_HORNET_STING_EXPLOSION_UPGRADE, 25051 RNSI_SA_EEG_GENERATE_BLACKHOLE_UPGRADE, 25204 RNSI_A_EEG_METAL_DUST_AURA, 25218 RNSI_A_EEG_METAL_DUST_AURA_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7c L323-343 (167 alpha-tag sites 32982623..329c84f8).
- **7c alpha-tag sites inside these functions: 15** (`c10a_alpha_sites.tsv`, by containing function).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31d170a0` CKTDXDeviceHolder::XMeshReadyInBackground (KTDXLIB/KTDXDeviceHolder.cpp:115) (confirmed); `31d006b0` CKTDGParticleSystem::CreateSequence( obj, name, const D3DXVECTOR3& pos, ... ) inline overload (KTDGParticleSystem.h:2007) (confirmed); `31c19f50` CKTDGParticleSystem::CParticleEventSequence::SetAxisAngle (KTDGParticleSystem.h:1385) (confirmed); `32290490` CX2GameUnit::UpDownCrashCameraNoReset (X2GameUnit.cpp:13104) (confirmed); `321cd590` ~(June CX2CustomizeManager item data struct) (candidate); `321cdbb0` June CX2CustomizeManager item data struct ctor (candidate).

## Amended by CH1l3 (2026-10-26)

- **Functions: 39.** Adds `3297baa0` (vslot 0x834 `ShowActiveSkillCutInAndLightByScript`: `3297b840( L"Dummy2_Lhand", time, m_iSkillCutInSetSubIndex ( +0x8acc ), bOnlyLight )`). CH1l3 moved it here because
  its only callee `3297b840` (Eve's own copy of the base cut-in function, `ActiveSkillShow` `31fde0f0` inside) is yours, together with `329cd890` / `329cd9c0`. March's body is `ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", time, m_iSkillCutInSetSubIndex, bOnlyLight_ )`.
