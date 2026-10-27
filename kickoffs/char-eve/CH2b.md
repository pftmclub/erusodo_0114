# June character Eve (Stage 7) - Phase CH2b - Skills: Architecture (19)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH2b` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH2a2 review and CH2b0 review (Opus helper phase). One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-eve/CH2b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Nasod and the first jobs (classes 5, 18, 19, 26)" "Phase CH3 - Skills: second jobs (108, 109, 116), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have" "7ae. June skill-tree ids with no March class state (from June skill system S3)"
```
Then CH0's and CH1's Done notes, and the `CH2b` rows of `june2017_re/char_eve/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Scope: the class-19 skill ids not in 5 / 18. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's addresses); its attached rows (7c alpha tags, 7u names, 7ai BFI arguments, any 7b block CH0 put here); its effect closure (a name found only now is staged byte-exact and added to `ch0_assets.tsv`). Upgrade ids reach their base case through `JuneSkillIDByUpgrade`: check the templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Eve: CH2b)`.
- Hand-off: the unit and class, each skill id and its A / B slot, what June's motion and effect look like.

## Amended by CH0 (2026-10-26)

- **Functions: 21** (`awk -F'\t' '$6=="CH2b"' june2017_re/char_eve/ch0_class_map.tsv`; state-fn 21).
  June addresses: `32966620` `32966640` `32966c90` `32966cd0` `32966db0` `32967d10` `32968da0` `3296f5c0` `3296f600` `329700a0` `32970120` `32970760` `32970840` `32974bb0` `3298c730` `3298d550` `3298d710` `3299d390` `329a99a0` `329b2e10` `329c4380`.
- **States: 7** (`awk -F'\t' '$8=="CH2b"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_SI1_AT_SURFACE_CUTING`, `ENSI_SI2_AT_SONIC_WAVE`, `ENSI_A_EAT_OBERON_GUARD`, `ENSI_A_EAT_HEAVENS_FIST`, `ENSI_A_EAT_HEAVENS_FIST_HYPER`, `ENSI_A_EAT_HEAVENS_FIST_SWEEPER`, `ENSI_A_EAT_FAENA`.
- **Skill ids: 12** (`awk -F'\t' 'NF==7 && $6=="CH2b"' june2017_re/char_eve/ch0_state_skill.tsv`): 25012 RNSI_SA_EAT_HEAVENS_FIST_PRESSURE, 25013 RNSI_SA_EAT_SONIC_WAVE, 25014 RNSI_SA_EAT_OBERON_GUARD, 25015 RNSI_SA_EAT_SURFACE_CUTING, 25016 RNSI_SA_EAT_HEAVENS_FIST_SWEEPER, 25038 RNSI_SA_EAT_HEAVENS_FIST_PRESSURE_UPGRADE, 25041 RNSI_SA_EAT_SONIC_WAVE_UPGRADE, 25056 RNSI_SA_EAT_OBERON_GUARD_UPGRADE, 25057 RNSI_SA_EAT_SURFACE_CUTING_UPGRADE, 25058 RNSI_SA_EAT_HEAVENS_FIST_SWEEPER_UPGRADE, 25224 RNSI_A_EAT_FAENA, 25225 RNSI_A_EAT_FAENA_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7ai L806-819 (BFI call arguments, 3 s skill-name string).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31d170a0` CKTDXDeviceHolder::XMeshReadyInBackground (KTDXLIB/KTDXDeviceHolder.cpp:115) (confirmed); `31d006b0` CKTDGParticleSystem::CreateSequence( obj, name, const D3DXVECTOR3& pos, ... ) inline overload (KTDGParticleSystem.h:2007) (confirmed); `31e920b0` BUFF_FACTOR::~BUFF_FACTOR (X2BuffFactor.h:652, implicit) (candidate); `32290350` CX2GameUnit::UpDownCrashCamera (X2GameUnit.cpp:13091) (confirmed); `321e3100` CX2EffectSet::PlayEffectSetWithCustomPos_LUA( const char*, CX2GameUnit*, D3DXVECTOR3 vPos, D3DXVECTOR3 vRot ) (X2EffectSet.cpp:5804) (confirmed); `32783a90` CX2GUNPC::AttachParticle_LUA( bool, char*, char*, D3DXVECTOR3 ) (X2GUNPC.cpp:29395) (confirmed); `32009180` user (candidate).

## Amended by CH1b (2026-10-26)

- **`3299b810` is CH2b's** (class map row moved from CH1b): the helper behind `ENSI_SI1_AT_SURFACE_CUTING` FrameMove (`3299d390`, called at `3299d3cc`) and `329ae4f0` (CH3b). It sets BFI `0x1b9` (441) at the equipped skill's level, rewrites the time of BTI `0x19c` (412)'s first Time finalizer, then `CreateNPCReq` for NPC `0x655` (`NUI_SI_SA_AT_SURFACE_CUTING`) at the position argument. Transcribe it whole here (BFI arguments: checklist 7ai, `s3e_bybfi_callers.tsv`); `June_SetSelfDestructSummonedNPC` and `June_ResetLinkOverChargeIllusion` already exist (CH1b).

## Amended by CH1n (2026-10-27)

- **Case bodies go in `CX2GUEve::June_SetEquippedSkillFuncHead`** (`X2GuEve.cpp`, block CH1n), keyed on June's raw id as `329c8a00` switches (both base and `_UPGRADE` ids are cases; no `JuneSkillIDByUpgrade`), each case `return false`. A case whose hyper takes every callback (`329cc2c0`) calls `CX2GUUser::June_CopyStateCallbacks( hyperStateData, normalStateData )` (one home since CH1n, flag `JUNE_USER_COPY_STATE_CALLBACKS`); `ch0_skill.tsv` column `calls` names the cases.
- **Faena (25224 / 25225) is June's `default:` of both switches** (`329c8a00 @329ca9d1`): `CX2GUEve::June_SetFaenaSkillFunc` registers the table and `JuneGenericActiveSkillEventProcess` only. Add its StateInit `32968da0` and OnFrameMove `329c4380` (hyper copies the FrameMove) there; March's id-switch `default:` already calls it under `_LIVE`.

## Amended by CH2b0 (2026-10-27)

- **`32009180` is `CX2GUUser::JuneCollectEnemyUnitsInCone( OUT vecUnits, iCount, fMaxDistance, fMinAngle, fMaxAngle, bFacingOnly, fConeDegree, bSkipInvincible )`** (`X2GUUser.cpp`, flag `_CHAR_EVE_SKILL`; funcmap row `32009180 (CH2b0)`). Faena's FrameMove `329c4380` calls it at `@329c447d` as `( vec, 1, SA 0x1f, -SA 0x9f, SA 0x9e, true, 180.f, false )`, the three SA values read through `32007c40` (`@329c43f1-329c4445`) with defaults 700 / 35 / 35. The count is never read (JUNE_BUGS 450): use the vector as June does.
