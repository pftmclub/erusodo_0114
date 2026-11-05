# June character Raven (Stage 7) - Phase CH3d - Hyper actives and transcendence rows

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH3d` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH3c2 review. One at a time: `X2GURaven.*`, `X2GUUser.*`, `X2Offline*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RAVEN SKILL`) |
| Review | `kickoffs/char-raven/CH3d-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH3 - Skills: second jobs (106, 107, 115), hyper actives, transcendence"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH3d - Done" "CH3d - Review"
```
Then CH0's Done note and the `CH3d` rows of `ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Do the CH3d row of "Phase CH3": hyper actives (type 13, 6 rows; June's hyper switch in `32c641a0`), the 3 `SHOW_HYPER_CUT_IN_TIME` states
(7t), and the transcendence rows the template lists: classes 106 / 107 / 115 on the offline transcend allow-list, `LevelUpEvent.lua` TYPE row +
`DLG_Transcendental_Change_Guide_Raven.lua`, checklist 7ao whole (hyper effect start `32c4e7c0`; the re-awakening branch of `HyperModeStart`
`31ffd490`, skill 24419 learned -> vslot `0x23c( 24419, 1, 1 )`; the four base helpers), `SPRT_HYPER_CLASS` skills and `TRANSCENDENTAL_SKILL_DATA`
rows, `GetHyperModeTableName` and `HQ_Cutin_RBM/RRF/RVC_Transcendence`, `list_TR_*` rows in `GetCharacterImageName` read from June's row pushes,
`POPUP_TR_B_MASTER` / `_R_FIST` / `_V_COMMANDER` present. Then `python tools/skillicons.py check <Raven's skill-id range>` = PASS.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps. CP949 files through `bytepatch.py`. Mark rows `(Raven: CH3d)`.
Hand-off: a transcended unit (`settranscend`) per second job, the hyper-class skills in A-5 / B-5, the transcended picture and cut-in.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.

## Amended by CH0 (2026-11-05)

- **Functions: 13** (`awk -F'\t' '$7=="CH3d"' june2017_re/char_raven/ch0_class_map.tsv`; inside 13).
  June addresses: `32c27c90` `32c27d90` `32c27e50` `32c27f10` `32c27fe0` `32c28040` `32c3c670` `32c3c6b0` `32c3c6e0` `32c3c730` `32c42e10` `32c42eb0` `32c4f0a0`.
- **States: 5** (`awk -F'\t' '$8=="CH3d"' june2017_re/char_raven/ch0_state_skill.tsv`): `RSI_HA_RRF_INFERNAL_ARM`, `RSI_HA_RBM_EXTREM_BLADE_READY`, `RSI_HA_RBM_EXTREM_BLADE_ATTACK`, `RSI_HA_RBM_EXTREM_BLADE_ATTACK_FINISH`, `RSI_HA_RVC_BURNING_BUSTER`.
- **Skills: 6** (part 2 of `ch0_state_skill.tsv`, col 6): 24600 RNSI_HA_RRF_INFERNAL_ARMS, 24601 RNSI_HA_RBM_EXTREM_BLADE, 24602 RNSI_HA_RVC_BURNING_BUSTER, 24603 RNSI_HA_RRF_INFERNAL_ARMS_UPGRADE, 24604 RNSI_HA_RBM_EXTREM_BLADE_UPGRADE, 24605 RNSI_HA_RVC_BURNING_BUSTER_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, RAVEN_STATE_ID); §7t L594 June UnitTemplet.lua m_HyperCutIn / m_BerserkCutIn keys and the HQ_Cut; §7t L604-608 hyper active skills: cut-in shown whatever the option says (type 13); §7ag L807-809 TC slot A-5 / B-5 states and the Stage 6 skips; §7ao L954 T5 branch: HyperModeStart 31ffd490, skill 24419 (0x5f63) learned while; §7ao L966 transcend: Raven hyper effect start 32c4e7c0 (vslot 49) returns at onc; §7ao L940-945 every class also brings: emulator allow-list id, LevelUpEvent TYPE row.
- 7ao: 24419 in `HyperModeStart` `31ffd490` and the transcended return of `32c4e7c0` (CH1h writes the body).
