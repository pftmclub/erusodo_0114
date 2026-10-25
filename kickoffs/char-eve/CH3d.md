# June character Eve (Stage 7) - Phase CH3d - Hyper actives, hyper cut-ins, transcendence rows

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH3d` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH3c review. One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL`; transcendence pieces under the T-plan flags they extend (`JuneTranscendCheck` and the rest) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`), `[JUNE_TRANSCEND]` |
| Review | `kickoffs/char-eve/CH3d-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH3 - Skills: second jobs (108, 109, 116), hyper actives, transcendence"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"
python tools/docsection.py JUNE_TRANSCENDENCE_PHASES.md "Phase T2 - Becoming transcended: quest, notify, guide" "Phase T6 - Aisha transcended" "Phase T6c - Transcended class pictures"
```
Then checklist 7ao (Eve's row and "Every class also brings"; `plan_checklist_rows.md` has the lines) and the `CH3d` rows of `ch0_*.tsv`.

## Task

Do the CH3d row of the plan doc: the 6 hyper actives (type 13), the 3 `SHOW_HYPER_CUT_IN_TIME` states (7t), and the transcendence rows the template lists: allow-list in `Handlers_Quest.cpp`, the `LevelUpEvent.lua` TYPE row, adopt `DLG_Transcendental_Change_Guide_Eve.lua`, 7ao whole with `32994710` and the four helpers, the hyper-class skills and `TRANSCENDENTAL_SKILL_DATA` rows, the `31fdd490` table names, `HQ_Cutin_*_Transcendence`, the `list_TR_*` rows in `GetCharacterImageName`, her transcended pictures. Transcend a test unit with `python tools/elsdb.py settranscend <unit> 1`.
- Mark rows `(Eve: CH3d)`.
- Hand-off: a transcended 108 / 109 / 116, each hyper-class skill, slots A-5 / B-5.

## Amended by CH0 (2026-10-26)

- **Functions: 23** (`awk -F'\t' '$6=="CH3d"' june2017_re/char_eve/ch0_class_map.tsv`; state-fn 23).
  June addresses: `329679d0` `32967a50` `32967a70` `32967ae0` `32967f00` `32967ff0` `32968020` `32968030` `32968270` `32974220` `32974470` `32974fb0` `32975320` `32975360` `329753a0` `3297e430` `32997100` `32997350` `32997940` `32997bc0` `329a9290` `329a9df0` `329abb90`.
- **States: 7** (`awk -F'\t' '$8=="CH3d"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_SA_EN_ILLUSION_RAY`, `ENSI_SA_EEG_HORNET_STING_SHAPED_CHARGE`, `ENSI_A_EEP_SPIT_FIRE_GRENADE`, `ENSI_HA_EBS_PSYCHIC_ARTILLERY`, `ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION`, `ENSI_HA_ENS_LUNATIC_SCUD`, `ENSI_HA_EBS_PSYCHIC_ARTILLERY`.
- **Skill ids: 6** (`awk -F'\t' 'NF==7 && $6=="CH3d"' june2017_re/char_eve/ch0_state_skill.tsv`): 25600 RNSI_HA_ECN_LUNATIC_SCUD, 25601 RNSI_HA_ECE_LINK_OVERCHARGE_ILLUSION, 25602 RNSI_HA_ECB_PSYCHIC_ARTILLERRY, 25603 RNSI_HA_ECN_LUNATIC_SCUD_UPGRADE, 25604 RNSI_HA_ECE_LINK_OVERCHARGE_ILLUSION_UPGRADE, 25605 RNSI_HA_ECB_PSYCHIC_ARTILLERRY_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7 L274 (SetUserSummonedNPCInfo per-case flags); §7 L281 (SetMultiplyPowerRate_LUA in Nasod_Watch_Eve / SI_HA_FERDINAN); §7 L282 (engine readers keyed by March skill ids (25000, 25601)); §7b L318-319 (31fdbbb0 / 31fdbcc0 memo readers); §7c L323-343 (167 alpha-tag sites 32982623..329c84f8); §7t L577-584 (hyper cut-in UnitTemplet + show option); §7ai L806-819 (BFI call arguments, 3 s skill-name string).
- **7c alpha-tag sites inside these functions: 4** (`c10a_alpha_sites.tsv`, by containing function).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31d170a0` CKTDXDeviceHolder::XMeshReadyInBackground (KTDXLIB/KTDXDeviceHolder.cpp:115) (confirmed); `322de5d0` CX2Game::UpdateSkillSlotUI( bool ) (X2Game.cpp:13345) (confirmed).
- **Hyper actives:** 25600-25605. 25600 reaches Lunatic Scud through `if ( id == 0x6400 )` beside the hyper sub-switch.
- **Transcendence-memo skills** (ids not in June's tree, reached through `31fdbcc0`): 25033 / 25070 `ENSI_SA_EEG_HORNET_STING_SHAPED_CHARGE`, 25034 / 25071 (Energy Needles case), 25035-36 / 25072-73 (Thousand Star case), 25217 / 25234 `ENSI_A_EEP_SPIT_FIRE_GRENADE`, 25210 / 25220 `ENSI_SA_EN_ILLUSION_RAY` (id switch only).
- 7ao: hyper effect start `32994710` = vslot 0x4c (CH1k transcribes it whole; check its transcendence branch here). The UnitTemplet `m_HyperCutIn` rows of 108 / 109 / 116 come with CH5.
