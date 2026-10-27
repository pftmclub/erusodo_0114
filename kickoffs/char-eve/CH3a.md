# June character Eve (Stage 7) - Phase CH3a - Skills: Code: Nemesis (108)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH3a` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH2c2 review and CH3a0 review (Opus helper phase). One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-eve/CH3a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Nasod and the first jobs (classes 5, 18, 19, 26)" "Phase CH3 - Skills: second jobs (108, 109, 116), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have" "7ae. June skill-tree ids with no March class state (from June skill system S3)"
```
Then CH0's and CH1's Done notes, and the `CH3a` rows of `june2017_re/char_eve/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Scope: the class-108 skill ids not in a first-job phase. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's addresses); its attached rows (7c alpha tags, 7u names, 7ai BFI arguments, any 7b block CH0 put here); its effect closure (a name found only now is staged byte-exact and added to `ch0_assets.tsv`). Upgrade ids reach their base case through `JuneSkillIDByUpgrade`: check the templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Eve: CH3a)`.
- Hand-off: the unit and class, each skill id and its A / B slot, what June's motion and effect look like.

## Amended by CH0 (2026-10-26)

- **Functions: 39** (`awk -F'\t' '$6=="CH3a"' june2017_re/char_eve/ch0_class_map.tsv`; helper 2, state-fn 37).
  June addresses: `329669b0` `32966a60` `32966a80` `32966ae0` `32966cf0` `329670d0` `329670f0` `32967300` `32967e30` `32968810` `32968840` `32968ab0` `3296fac0` `3296fb20` `3296fb60` `3296fbc0` `329701f0` `329704f0` `32971f70` `32971fe0` `32972020` `32972030` `32972060` `32972120` `329727a0` `32972800` `32974db0` `32976960` `32976a10` `32976fe0` `3297bb10` `3297bb40` `3298d920` `3298ff60` `3298ffc0` `32990020` `32990040` `329903e0` `3299a670`.
- **States: 15** (`awk -F'\t' '$8=="CH3a"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_SA_EEG_IRONSCRAPS`, `ENSI_SA_EEG_IRONSCRAPS_HYPER`, `ENSI_A_EEG_JUNK_BREAK`, `ENSI_A_EEG_JUNK_BREAK_HYPER`, `ENSI_A_ENS_BURST_SPEAR`, `ENSI_A_ENS_BURST_SPEAR_HYPER`, `ENSI_A_ENS_BURST_SPEAR_Landing`, `ENSI_A_ENS_BURST_SPEAR_Landing_HYPER`, `ENSI_SA_EEG_ATOMIC_SHIELD`, `ENSI_SA_ENS_ASSUALT_SPEAR`, `ENSI_A_ECN_TESLA_SHOCK`, `ENSI_A_ENS_SHOT_GUN`, `ENSI_SA_ENS_ILLUSIONIC_THORN`, `ENSI_SA_ENS_EXTREMING_CUTTING`, `ENSI_SA_ECN_PSYKER_KINESIS`.
- **Skill ids: 16** (`awk -F'\t' 'NF==7 && $6=="CH3a"' june2017_re/char_eve/ch0_state_skill.tsv`): 25008 RNSI_SA_ECN_JUNK_BREAK, 25009 RNSI_SA_ECN_ASSAULT_SPEAR_ANNIHILATOR, 25010 RNSI_SA_ECN_IRONSCRAPS, 25011 RNSI_SA_ECN_ATOMIC_SHIELD, 25043 RNSI_SA_ENS_ILLUSIONIC_THORN, 25044 RNSI_SA_ENS_EXTREMING_CUTTING, 25052 RNSI_SA_ECN_JUNK_BREAK_UPGRADE, 25053 RNSI_SA_ECN_ASSAULT_SPEAR_ANNIHILATOR_UPGRADE, 25054 RNSI_SA_ECN_IRONSCRAPS_UPGRADE, 25055 RNSI_SA_ECN_ATOMIC_SHIELD_UPGRADE, 25205 RNSI_A_ECN_TESLA_SHOCK, 25206 RNSI_A_ECN_SPEAR_BURST, 25221 RNSI_A_ENS_SHOT_GUN, 25227 RNSI_A_ECN_TESLA_SHOCK_UPGRADE, 25228 RNSI_A_ECN_SPEAR_BURST_UPGRADE, 25235 RNSI_SA_ECN_PSYKER_KINESIS.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7 L248 (32976fe0 PSYKER_KINESIS cooldown (caller 329b4ab1)); §7ai L806-819 (BFI call arguments, 3 s skill-name string).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31d170a0` CKTDXDeviceHolder::XMeshReadyInBackground (KTDXLIB/KTDXDeviceHolder.cpp:115) (confirmed); `32071d20` CX2GUUser::GetXSkinAnimPtr (X2GUUser.h:2084) (candidate); `32d5da80` CX2UnitSlashTraceManager::AddSlashTraceData (X2UnitSlashTraceManager.cpp:88) (confirmed); `31fd0620` user (candidate); `31c82df0` engine (unknown).

## Amended by CH1l3 (2026-10-26)

- **Functions: 41.** Adds `329aea60` (Psyker Kinesis release: the effect set `+0x9c50` block's handle `+8`, `GetEffectSetInstance` `321d97c0`, one `DamageEffect_SA_ECN_Psyker_Kinesis_Spear_Attack_Move` per live damage-effect element
  at its `Dummy02` bone through `329cc200`, then `JuneEveBlock9c50::Reset( true )`) and its only callee `329cc200`, from CH1l4.
- **Add the calls** that CH1l3 and CH1l4 left out: `CX2GUEve::June_NoStateChangeActionFrameMove` case `JUNE_CH1L3_UAI_PSYKER_KINESIS` ( 0x40 ) calls `329aea60` then `32976fe0`; CH1l4's `329b4a60` ( the block's `+0xc` countdown ) does the same.
  `32976fe0` itself ends in `31fd0620( 25235, true )` ( UI notify, CH3a0 ).

## Amended by CH1n (2026-10-27)

- **Case bodies go in `CX2GUEve::June_SetEquippedSkillFuncHead`** (`X2GuEve.cpp`, block CH1n), keyed on June's raw id as `329c8a00` switches (both base and `_UPGRADE` ids are cases; no `JuneSkillIDByUpgrade`), each case `return false`. A case whose hyper takes every callback (`329cc2c0`) calls `CX2GUUser::June_CopyStateCallbacks( hyperStateData, normalStateData )` (one home since CH1n, flag `JUNE_USER_COPY_STATE_CALLBACKS`); `ch0_skill.tsv` column `calls` names the cases.

## Amended by CH3a0 (2026-10-28)

- **`31fd0620` = `CX2GUUser::JuneRefreshSkillSlotUI( CX2SkillTree::SKILL_ID, bool bUpgradeSwap )`** (X2GUUser.cpp, flag `_SKILL`). Both Eve call sites pass `( CX2SkillTree::RNSI_SA_ECN_PSYKER_KINESIS, true )`: `32976fe0 @3297707f` and `3299a670 @3299a993`. It calls `CX2UISkillTreeNew::JuneMarkEquipSlotUpdated` (June `328da730`); nothing else to port.
- **`31c82df0` = March's `CKTDGXMeshPlayer::CXMeshInstance::SetNotCheckLandTime`** (`+0x36c` = `fNotCheckLandTime`). `3298d920 @3298db39` is Spear Burst's main-effect step: `SetLandPosition( land y )` (`+0x490`), then `SetNotCheckLandTime( gapY / 3500 + 0.17 )`, as March `X2GuEve.cpp:20988-20991` already does.
