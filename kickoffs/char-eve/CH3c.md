# June character Eve (Stage 7) - Phase CH3c - Skills: Code: Battle Seraph (116)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH3c` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH3b review (CH0: no helper phase needed). One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-eve/CH3c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Nasod and the first jobs (classes 5, 18, 19, 26)" "Phase CH3 - Skills: second jobs (108, 109, 116), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have" "7ae. June skill-tree ids with no March class state (from June skill system S3)"
```
Then CH0's and CH1's Done notes, and the `CH3c` rows of `june2017_re/char_eve/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Scope: the class-116 skill ids not in an earlier phase, incl. Electronic Move 25223 (`ENSI_A_EBS_ELECTRONIC_MOVE_FRONT / UP / DOWN`: the 3 numerical-only tables, checklist section 1; their callbacks are copied from states 219-221 by `329cc2c0`). For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's addresses); its attached rows (7c alpha tags, 7u names, 7ai BFI arguments, any 7b block CH0 put here); its effect closure (a name found only now is staged byte-exact and added to `ch0_assets.tsv`). Upgrade ids reach their base case through `JuneSkillIDByUpgrade`: check the templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Eve: CH3c)`.
- Hand-off: the unit and class, each skill id and its A / B slot, what June's motion and effect look like.

## Amended by CH0 (2026-10-26)

- **Functions: 35** (`awk -F'\t' '$6=="CH3c"' june2017_re/char_eve/ch0_class_map.tsv`; helper 2, state-fn 33).
  June addresses: `329676f0` `32967770` `32967780` `329677b0` `32967940` `32967980` `32967990` `329679a0` `32967fa0` `32967fb0` `32967fd0` `329688b0` `329688e0` `32968950` `32968b20` `32973a70` `32973f10` `32973fd0` `329741a0` `329741e0` `329752d0` `3297ba70` `3297e150` `3297eae0` `3297efc0` `329809c0` `329968a0` `32997920` `3299a9d0` `329a2b70` `329a6fe0` `329a7f70` `329aafc0` `329ae0c0` `329b4f10`.
- **States: 12** (`awk -F'\t' '$8=="CH3c"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_SA_EPR_THOUSANDS_OF_STARS`, `ENSI_SI_A_EBS_TASER_PILUM`, `ENSI_EBS_ENERGY_NEEDLES`, `ENSI_EBS_ENERGETIC_HEART`, `ENSI_SA_EEL_LINEAR_DIVIDER`, `ENSI_A_ECB_TESLA_SHOCK`, `ENSI_A_EBS_ELECTRONIC_MOVE_FRONT`, `ENSI_A_EBS_ELECTRONIC_MOVE_UP`, `ENSI_A_EBS_ELECTRONIC_MOVE_DOWN`, `ENSI_SA_EBS_NEEDLES_CUTTING`, `ENSI_SA_EBS_LASER_POLE`, `ENSI_SA_ECB_EXTREME_VIRTUAL`.
- **Skill ids: 16** (`awk -F'\t' 'NF==7 && $6=="CH3c"' june2017_re/char_eve/ch0_state_skill.tsv`): 25026 RNSI_SA_ECB_LINEAR_DIVIDER, 25027 RNSI_SA_ECB_ENERGY_NEEDLES, 25028 RNSI_SA_ECB_THOUSANDS_OF_STARS, 25029 RNSI_SA_ECB_ENERGETIC_HEART, 25047 RNSI_SA_EBS_NEEDLES_CUTTING, 25048 RNSI_SA_EBS_LASER_POLE, 25066 RNSI_SA_ECB_LINEAR_DIVIDER_UPGRADE, 25067 RNSI_SA_ECB_ENERGY_NEEDLES_UPGRADE, 25068 RNSI_SA_ECB_THOUSANDS_OF_STARS_UPGRADE, 25069 RNSI_SA_ECB_ENERGETIC_HEART_UPGRADE, 25211 RNSI_A_ECB_TESLA_SHOCK, 25212 RNSI_A_ECB_TASER_PILUM, 25223 RNSI_A_EBS_ELECTRONIC_MOVE, 25232 RNSI_A_ECB_TESLA_SHOCK_UPGRADE, 25233 RNSI_A_ECB_TASER_PILUM_UPGRADE, 25237 RNSI_SA_ECB_EXTREME_VIRTUAL.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §1 L37-105 (numerical-only DAMAGE_DATA + numerical override live); §2 L113 (APPLIED_SKILL_ID (parsed since 8c)); §2 L114 (NUMERICAL_DATA_INDEX); §7 L277 (3201ad10 skill-group power socket rate from class code); §7c L323-343 (167 alpha-tag sites 32982623..329c84f8); §7h L410 (USEFUL_VALUE rows).
- **7c alpha-tag sites inside these functions: 3** (`c10a_alpha_sites.tsv`, by containing function).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31d170a0` CKTDXDeviceHolder::XMeshReadyInBackground (KTDXLIB/KTDXDeviceHolder.cpp:115) (confirmed); `322de5d0` CX2Game::UpdateSkillSlotUI( bool ) (X2Game.cpp:13345) (confirmed); `31d006b0` CKTDGParticleSystem::CreateSequence( obj, name, const D3DXVECTOR3& pos, ... ) inline overload (KTDGParticleSystem.h:2007) (confirmed); `31e920b0` BUFF_FACTOR::~BUFF_FACTOR (X2BuffFactor.h:652, implicit) (candidate); `31e4cd60` CX2GameUnit::GetZVector (X2GameUnit.h:2545) (confirmed); `31fdbf00` user (candidate).
- **Electronic Move 25223** is Battle Seraph only (CH0 correction: the plan said CH2c). Its states 239-241 copy the callbacks of states 219-221 (`ENSI_EBS_DASH_JUMP_COMBO_ZFrontX / ZUpX / ZDownX`) through `329cc2c0`; the 3 tables are numerical-only (checklist section 1).

## Amended by CH1d (2026-10-26)

- `329968a0` calls `m_EveElectraSystem.June_AddEffectToSystem` (`32980580`, CH1d); `329809c0` (AddLockonToSystem) builds the same `EveElectraEffectData`-like record: see the CH1d Done note on June's two extra members (+0x8c, +0x90).

## Amended by CH1e (2026-10-26)

- The three Electronic Move state enumerators `ENSI_A_EBS_ELECTRONIC_MOVE_FRONT` / `_UP` / `_DOWN` (script ids 239 / 240 / 241) are already enumerators (CH1e added them for `June_StateChangeBoostAttackIfPossible`); do not add them again.
