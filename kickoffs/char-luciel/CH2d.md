# June character Lu/Ciel (Stage 8) - Phase CH2d - Diablura (39) skills, part 1

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH2d` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH2c2 review
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_SKILL` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL SKILL` / `NUM`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/CH2d-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: base and first jobs (200, 37, 38, 39)" "Phase CH1 - Deploy unit: the class core"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the `CH2d` row of "Phase CH2 - Skills: base and first jobs (200, 37, 38, 39)", from `ch0_state_skill.tsv` / `ch0_class_map.tsv` (CH0's amendment names the filters). Each skill is tested
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

- **Functions: 31 to transcribe** (+ 30 thunks) (`awk -F'\t' '$6=="CH2d"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae87b0` LSI_SA_LDB_LAND_CRASHER_Init `32ae88a0` LSI_SA_LDB_BLOODLUST_Init `32ae88f0` LSI_SA_LDB_NIGHTMARE_Init `32ae89c0` LSI_SA_LDB_NIGHTMARE_StateStart `32ae89e0` LSI_SA_LDB_ALTAR_OF_EVIL_Init `32ae8b60` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_JUMP_UP_EventProcess `32ae8bb0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_DEATH_TOUCH_Init `32ae8c50` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_MORTAL_FEAR_Init `32ae8cf0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_MORTAL_FEAR_StateStart `32ae8d10` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_DARK_PLASMA_Init `32ae8e00` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_DARK_PLASMA_StateStart `32ae8f50` LSI_A_LDB_F_ABYSS_STRIKE_Init `32ae8ff0` LSI_A_LDB_F_PHANTOM_RUSH_Init `32ae90b0` LSI_A_LDB_M_PHANTOM_RUSH_Init `32ae9c90` GetTransformedSkillID `32b02840` LSI_SA_LDB_LAND_CRASHER_EventProcess `32b02890` LSI_SA_LDB_LAND_CRASHER_LANDING_EventProcess `32b02b00` LSI_SA_LDB_BLOODLUST_EventProcess `32b02d80` LSI_SA_LDB_ALTAR_OF_EVIL_EventProcess `32b02dd0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_START_EventProcess `32b02e10` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_JUMP_DOWN_EventProcess `32b02e60` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_JUMP_X_EventProcess `32b02f10` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_Z_EventProcess `32b03250` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_ZZ_EventProcess `32b03590` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_ZZZ_EventProcess `32b038d0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_ZZZZ_EventProcess `32b03bb0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_X_EventProcess `32b03ef0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_XX_EventProcess `32b04230` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_XXX_READY_EventProcess `32b042d0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_XXX_END_EventProcess `32b045b0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_XXX_READY_EventProcess.
- **States: 44** (`awk -F'\t' '$8=="CH2d"' june2017_re/char_luciel/ch0_state_skill.tsv`): `LSI_A_LDB_F_ABYSS_STRIKE`, `LSI_A_LDB_M_ABYSS_STRIKE`, `LSI_SA_LDB_LAND_CRASHER`, `LSI_SA_LDB_LAND_CRASHER_LANDING`, `LSI_SA_LDB_LAND_CRASHER_CHAIN`, `LSI_A_LDB_F_PHANTOM_RUSH`, `LSI_A_LDB_F_PHANTOM_RUSH_TARGET`, `LSI_A_LDB_M_PHANTOM_RUSH`, `LSI_A_LDB_M_PHANTOM_RUSH_TARGET`, `LSI_SA_LDB_BLOODLUST`, `LSI_SA_LDB_NIGHTMARE`, `LSI_SA_LDB_NIGHTMARE_TARGET`, `LSI_SA_LDB_ALTAR_OF_EVIL`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_START`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_WAIT`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_WALK`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_JUMP_READY`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_JUMP_UP`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_JUMP_DOWN`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_JUMP_LANDING`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_DASH`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_DASH_END`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_DASH_JUMP`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_DASH_JUMP_LANDING`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_HYPER_MODE`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_HYPER_MODE_TR`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_THROW_ITEM`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_END`, `LSI_SA_LDB_ALTAR_OF_EVIL_END`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_JUMP_Z`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_JUMP_X`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_Z`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_ZZ`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_ZZZ`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_ZZZZ`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_X`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_XX`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_XXX_READY`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_XXX_END`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_DEATH_TOUCH`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_MORTAL_FEAR`, `LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_DARK_PLASMA`, `LSI_SA_F_COMMON_BUFF_PHANTOM_CIRCLE`, `LSI_SA_M_COMMON_BUFF_PHANTOM_CIRCLE`.
- **Skills: 17** (part 2 of `ch0_state_skill.tsv`, col 6): 30040 RNSI_SA_LDB_LAND_CRASHER, 30041 RNSI_SA_LDB_BLOODLUST, 30042 RNSI_SA_LDB_NIGHTMARE, 30043 RNSI_SA_LDB_ALTAR_OF_EVIL, 30044 RNSI_SA_LDB_DEATH_TOUCH, 30045 RNSI_SA_LDB_MORTAL_FEAR, 30046 RNSI_SA_LDB_DARK_PLASMA, 30054 RNSI_SA_LDB_LAND_CRASHER_UPGRADE, 30075 RNSI_SA_LDB_BLOODLUST_UPGRADE, 30076 RNSI_SA_LDB_NIGHTMARE_UPGRADE, 30077 RNSI_SA_LDB_ALTAR_OF_EVIL_UPGRADE, 30215 RNSI_A_LDB_ABYSS_STRIKE, 30216 RNSI_A_LDB_PHANTOM_RUSH, 30219 RNSI_A_LDB_ABYSS_STRIKE_UPGRADE, 30220 RNSI_A_LDB_PHANTOM_RUSH_UPGRADE, 30303 RNSI_BUFF_PHANTOM_CIRCLE, 30313 RNSI_BUFF_PHANTOM_CIRCLE_UPGRADE.
- Tree rows: 28 (part 3, col 10).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178-181 June-only SKILL_ID members fail silently (52 SI_ 10000-10699 + 143 RNSI_ 30001-3; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry; §7ae L713-720 June skill-tree ids with no March class state (reform ids 21000+, RNSI_*); §7ae L729-733 June skill types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTi.
- Prefix `LDB` = 39 (also owned by 129). The transform states (`IsTransformedForm`, linked skill slots) are CH1a0's engine pieces; the transform HUD is CH2d20's (CH2d2 depends on it).
