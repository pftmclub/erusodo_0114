# June character Lu/Ciel (Stage 8) - Phase CH2c - Royal Guard (38) skills, part 1

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH2c` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH2b review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_SKILL` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL SKILL` / `NUM`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/CH2c-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: base and first jobs (200, 37, 38, 39)" "Phase CH1 - Deploy unit: the class core"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the `CH2c` row of "Phase CH2 - Skills: base and first jobs (200, 37, 38, 39)", from `ch0_state_skill.tsv` / `ch0_class_map.tsv` (CH0's amendment names the filters). Each skill is tested
from an A and a B slot on the body that casts it; the hand-off gives the unit, class, skill id and slot, never slot 4 / 9 before
transcendence.

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

- **Functions: 35 to transcribe** (+ 35 thunks) (`awk -F'\t' '$6=="CH2c"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae6e10` LSI_SA_LCG_SOUL_EXTORTION_Init `32ae6eb0` LSI_SA_LCG_SOUL_EXTORTION_FrameMove `32ae6ec0` LSI_SA_LCG_SOUL_EXTORTION_StateStartFuture `32ae6f30` LSI_SA_LCG_SOUL_EXTORTION_StateStart `32ae6fa0` LSI_SA_LCG_DEMONIC_BREATH_START_Init `32ae70f0` LSI_SA_LCG_DEMONIC_BREATH_FIRE_StateStartFuture `32ae7110` LSI_SA_LCG_DEMONIC_BREATH_FIRE_FrameMoveFuture `32ae7150` LSI_SA_LCG_FANTASIE_IMPROMPTU_CANTABILE_READY_Init `32ae7280` LSI_SA_LCG_FANTASIE_IMPROMPTU_CANTABILE_READY_StateStart `32ae72c0` LSI_SA_LCG_FANTASIE_IMPROMPTU_CANTABILE_WAIT_EventProcess `32ae7350` LSI_SA_LCG_FANTASIE_IMPROMPTU_CANTABILE_FRONT_MOVE_EventProcess `32ae73e0` LSI_SA_LCG_FANTASIE_IMPROMPTU_CANTABILE_BACK_MOVE_EventProcess `32ae7470` LSI_SA_LCG_FANTASIE_IMPROMPTU_ACCELERANDO_READY_StateStart `32ae7490` LSI_SA_LCG_FANTASIE_IMPROMPTU_ACCELERANDO_WAIT_EventProcess `32ae7520` LSI_SA_LCG_FANTASIE_IMPROMPTU_ACCELERANDO_FRONT_MOVE_EventProcess `32ae75b0` LSI_SA_LCG_FANTASIE_IMPROMPTU_ACCELERANDO_BACK_MOVE_EventProcess `32ae7640` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_READY_Init `32ae7700` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_WAIT_StateEndFuture `32ae7720` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_END_StateStart `32ae7730` LSI_SA_LCG_ESCHATON_Init `32ae79f0` LSI_A_LCG_F_SOUL_SHOCK_Init `32ae7a80` LSI_A_LCG_F_SOUL_STRIKE_READY_Init `32ae84a0` LSI_A_LCN_F_SHADOW_Init `32b002f0` LSI_SA_LCG_SOUL_EXTORTION_EventProcess `32b004f0` LSI_SA_LCG_DEMONIC_BREATH_CHARGING_StateStart `32b00510` LSI_SA_LCG_DEMONIC_BREATH_FIRE_EventProcess `32b005f0` LSI_SA_LCG_FANTASIE_IMPROMPTU_CANTABILE_READY_EventProcess `32b00800` LSI_SA_LCG_FANTASIE_IMPROMPTU_ACCELERANDO_READY_EventProcess `32b00840` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_READY_StateStart `32b00920` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_READY_EventProcess `32b00960` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_SMALL_FIRE_StateStartFuture `32b009a0` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_BIG_FIRE_StateStartFuture `32b009e0` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_BIG_FIRE_EventProcess `32b00c10` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_END_EventProcess `32b00e10` LSI_SA_LCG_ESCHATON_EventProcess.
- **States: 30** (`awk -F'\t' '$8=="CH2c"' june2017_re/char_luciel/ch0_state_skill.tsv`): `LSI_SA_LCG_SOUL_EXTORTION`, `LSI_SA_LCG_DEMONIC_BREATH_START`, `LSI_SA_LCG_DEMONIC_BREATH_CHARGING`, `LSI_SA_LCG_DEMONIC_BREATH_FIRE`, `LSI_SA_LCG_FANTASIE_IMPROMPTU_CANTABILE_READY`, `LSI_SA_LCG_FANTASIE_IMPROMPTU_CANTABILE_WAIT`, `LSI_SA_LCG_FANTASIE_IMPROMPTU_CANTABILE_FRONT_MOVE`, `LSI_SA_LCG_FANTASIE_IMPROMPTU_CANTABILE_BACK_MOVE`, `LSI_SA_LCG_FANTASIE_IMPROMPTU_ACCELERANDO_READY`, `LSI_SA_LCG_FANTASIE_IMPROMPTU_ACCELERANDO_WAIT`, `LSI_SA_LCG_FANTASIE_IMPROMPTU_ACCELERANDO_FRONT_MOVE`, `LSI_SA_LCG_FANTASIE_IMPROMPTU_ACCELERANDO_BACK_MOVE`, `LSI_SA_LCG_FANTASIE_IMPROMPTU_ACCELERANDO_END`, `LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_READY`, `LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_WAIT`, `LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_SMALL_FIRE`, `LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_BIG_CHARGE`, `LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_BIG_FIRE`, `LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_END`, `LSI_SA_LCG_ESCHATON`, `LSI_A_LCG_F_SOUL_SHOCK`, `LSI_A_LCG_M_SOUL_SHOCK`, `LSI_A_LCG_F_SOUL_STRIKE_READY`, `LSI_A_LCG_F_SOUL_STRIKE_LOOP`, `LSI_A_LCG_F_SOUL_STRIKE_END`, `LSI_A_LCG_M_SOUL_STRIKE_READY`, `LSI_A_LCG_M_SOUL_STRIKE_LOOP`, `LSI_A_LCG_M_SOUL_STRIKE_END`, `LSI_A_LCN_F_SHADOW`, `LSI_A_LCN_M_SHADOW`.
- **Skills: 15** (part 2 of `ch0_state_skill.tsv`, col 6): 10205 SI_A_LCG_SOUL_SHOCK, 30017 RNSI_SA_LCG_DEMONIC_BREATH, 30018 RNSI_SA_LCG_DEATH_CRISIS_OF_MARBAS, 30019 RNSI_SA_LCG_SOUL_EXTORTION, 30020 RNSI_SA_LCG_ESCHATON, 30021 RNSI_SA_LCG_FANTASIE_IMPROMPTU_1ST_MOVEMENT, 30037 RNSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_UPGRADE, 30039 RNSI_SA_LCG_SOUL_EXTORTION_UPGRADE, 30068 RNSI_SA_LCG_DEMONIC_BREATH_UPGRADE, 30069 RNSI_SA_LCG_FANTASIE_IMPROMPTU_1ST_MOVEMENT_UPGRADE, 30070 RNSI_SA_LCG_ESCHATON_UPGRADE, 30207 RNSI_A_LCG_SOUL_STRIKE, 30209 RNSI_A_LCG_SHADOW, 30214 RNSI_A_LCG_SOUL_STRIKE_UPGRADE, 30310 RNSI_A_LCG_SHADOW_UPGRADE.
- Tree rows: 28 (part 3, col 10).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178-181 June-only SKILL_ID members fail silently (52 SI_ 10000-10699 + 143 RNSI_ 30001-3; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry; §7ae L713-720 June skill-tree ids with no March class state (reform ids 21000+, RNSI_*); §7ae L729-733 June skill types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTi.
- Prefix `LCG` = 38 (also owned by 128). Split in table order with CH2c2.
