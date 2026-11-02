# June character Lu/Ciel (Stage 8) - Phase CH3d - Hyper actives and transcendence rows

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel CH3d` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH3c2 review
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_SKILL` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL SKILL` / `NUM`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/CH3d-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH3 - Skills: second jobs (127, 128, 129), hyper actives, transcendence" "Phase CH1 - Deploy unit: the class core"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the `CH3d` row of "Phase CH3 - Skills: second jobs (127, 128, 129), hyper actives, transcendence", from `ch0_state_skill.tsv` / `ch0_class_map.tsv` (CH0's amendment names the filters). Each skill is tested
from an A and a B slot on the body that casts it; the hand-off gives the unit, class, skill id and slot, never slot 4 / 9 before
transcendence.

Transcendence rows: the Aisha plan's "Template for Stage 7" list, and `skillicons.py check` = PASS after them.

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

## Amended by CH0 (2026-11-02)

- **Functions: 7 to transcribe** (+ 14 thunks) (`awk -F'\t' '$6=="CH3d"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae8540` LSI_HA_LLD_DESCENT_DARKNESS_Init `32ae8630` LSI_HA_LCN_CASTLE_OF_ABADDON_Init `32ae99f0` LSI_HA_LDN_DIABOLIC_DISASTER_Init `32ae9b20` LSI_HA_LDN_DIABOLIC_DISASTER_StateEnd `32b02420` LSI_HA_LLD_DESCENT_DARKNESS_EventProcess `32b02630` LSI_HA_LCN_CASTLE_OF_ABADDON_EventProcess `32b07f30` LSI_HA_LDN_DIABOLIC_DISASTER_FrameMove.
- **States: 3** (`awk -F'\t' '$8=="CH3d"' june2017_re/char_luciel/ch0_state_skill.tsv`): `LSI_HA_LLD_DESCENT_DARKNESS`, `LSI_HA_LCN_CASTLE_OF_ABADDON`, `LSI_HA_LDN_DIABOLIC_DISASTER`.
- **Skills: 6** (part 2 of `ch0_state_skill.tsv`, col 6): 30601 RNSI_HA_LLD_DESCENT_DARKNESS, 30602 RNSI_HA_LCN_CASTLE_OF_ABADDON, 30603 RNSI_HA_LLD_DESCENT_DARKNESS_UPGRADE, 30604 RNSI_HA_LCN_CASTLE_OF_ABADDON_UPGRADE, 30605 RNSI_HA_LDN_DIABOLIC_DISASTER, 30606 RNSI_HA_LDN_DIABOLIC_DISASTER_UPGRADE.
- Tree rows: 6 (part 3, col 10).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178-181 June-only SKILL_ID members fail silently (52 SI_ 10000-10699 + 143 RNSI_ 30001-3; §7t L591-593 UnitTemplet m_HyperCutIn / m_BerserkCutIn / m_MinorUnitHyperCutIn and the two _T; §7t L603 32014fc0 ShowHyperSkillCutInByScript: second body + non-empty m_MinorUnitHyperCu; §7t L604-608 hyper active skills (type 13): cut-in shown whatever the option says; §7ae L713-720 June skill-tree ids with no March class state (reform ids 21000+, RNSI_*); §7ag L807-809 TC slot A-5 / B-5 states and the Stage 6 skips; §7ao L932-939 transcend allow-list id, LevelUpEvent TYPE rows, DLG_Transcendental_Change_Guide; §7ao L940 JuneGetHyperModeTableName 31fdd490 cases (March bCiel_ branch) and transcended c; §7ao L941-943 hyper-class skills (SPRT_HYPER_CLASS), the four base skill helpers with the flag; §7ao L964 Lu/Ciel hyper effect start 32b1f0a0 (vslot 19) and CommonFrameMove override 32b3.
- 7 class functions (type 13 / transcendence rows). Cut-ins `HQ_Cutin_{LLD,LCN,LDN}_{LU,CIEL}_Transcendence` are June bytes already; `POPUP_TR_*` are CH0b's.
