# June character Lu/Ciel (Stage 8) - Phase CH2b - Chiliarch (37) skills

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH2b` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH2a, CHN review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_SKILL` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL SKILL` / `NUM`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/CH2b-review.md (Opus 5.5 high; the phase writes it when done). **It covers CH2a, CH2b** (user, 2026-11-10): every covered Done note, `flagcheck --range` over all of them, `--check-all`, retires any debug define |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: base and first jobs (200, 37, 38, 39)" "Phase CH1 - Deploy unit: the class core"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the `CH2b` row of "Phase CH2 - Skills: base and first jobs (200, 37, 38, 39)", from `ch0_state_skill.tsv` / `ch0_class_map.tsv` (CH0's amendment names the filters). Each skill is tested
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

- **Functions: 27 to transcribe** (+ 27 thunks) (`awk -F'\t' '$6=="CH2b"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae6900` LSI_SA_LLC_DEAD_HANDS_Init `32ae6a30` LSI_SA_LLC_DEAD_HANDS_StateStartFuture `32ae6a60` LSI_SA_LLC_DEAD_HANDS_FrameMoveFuture `32ae6a70` LSI_SA_LLC_DARK_CRESCENT_Init `32ae6ad0` LSI_SA_LLC_RUINOUS_IMPACT_Init `32ae6b30` LSI_SA_LLC_RUINOUS_IMPACT_FrameMove `32ae6b40` LSI_SA_LLC_RUINOUS_IMPACT_FrameMoveFuture `32ae6b80` LSI_SA_LLC_POWER_OF_ORIAX_Init `32ae6be0` LSI_SA_LLC_POWER_OF_ORIAX_FrameMove `32ae6bf0` LSI_SA_LLC_APOLLYON_Init `32ae6ce0` LSI_SA_LLC_APOLLYON_StateStart `32ae7920` LSI_A_LLC_F_BLASTING_FEAR_Init `32ae7980` LSI_A_LLC_F_BACKSTAB_Init `32aff850` LSI_SA_LLC_DEAD_HANDS_EventProcess `32affa70` LSI_SA_LLC_DARK_CRESCENT_StateStart `32affa90` LSI_SA_LLC_DARK_CRESCENT_EventProcess `32affca0` LSI_SA_LLC_RUINOUS_IMPACT_StateStart `32affcf0` LSI_SA_LLC_POWER_OF_ORIAX_EventProcess `32affef0` LSI_SA_LLC_APOLLYON_EventProcess `32b01050` LSI_A_LLC_F_BLASTING_FEAR_EventProcess `32b01250` LSI_A_LLC_F_BACKSTAB_EventProcess `32b13180` LSI_SA_LLC_DEAD_HANDS_FrameMove `32b16ca0` LSI_SA_LLC_RUINOUS_IMPACT_EventProcess `32b1d200` LSI_SA_LLC_DARK_CRESCENT_StateEndFuture `32b2ac20` LSI_SA_LLC_DARK_CRESCENT_StateStartFuture `32b2b050` LSI_SA_LLC_APOLLYON_FrameMove `32b3d010` LSI_SA_LLC_DARK_CRESCENT_FrameMove.
- **States: 9** (`awk -F'\t' '$8=="CH2b"' june2017_re/char_luciel/ch0_state_skill.tsv`): `LSI_SA_LLC_DEAD_HANDS`, `LSI_SA_LLC_DARK_CRESCENT`, `LSI_SA_LLC_RUINOUS_IMPACT`, `LSI_SA_LLC_POWER_OF_ORIAX`, `LSI_SA_LLC_APOLLYON`, `LSI_A_LLC_F_BLASTING_FEAR`, `LSI_A_LLC_M_BLASTING_FEAR`, `LSI_A_LLC_F_BACKSTAB`, `LSI_A_LLC_M_BACKSTAB`.
- **Skills: 14** (part 2 of `ch0_state_skill.tsv`, col 6): 30007 RNSI_SA_LLC_RUINOUS_IMPACT, 30008 RNSI_SA_LLC_DEAD_HANDS, 30009 RNSI_SA_LLC_DARK_CRESCENT, 30010 RNSI_SA_LLC_POWER_OF_ORIAX, 30011 RNSI_SA_LLC_APOLLYON, 30036 RNSI_SA_LLC_RUINOUS_IMPACT_UPGRADE, 30038 RNSI_SA_LLC_DEAD_HANDS_UPGRADE, 30061 RNSI_SA_LLC_DARK_CRESCENT_UPGRADE, 30062 RNSI_SA_LLC_POWER_OF_ORIAX_UPGRADE, 30063 RNSI_SA_LLC_APOLLYON_UPGRADE, 30203 RNSI_A_LLC_BLASTING_FEAR, 30205 RNSI_A_LLC_BACKSTAB, 30213 RNSI_A_LLC_BLASTING_FEAR_UPGRADE, 30307 RNSI_A_LLC_BACKSTAB_UPGRADE.
- Tree rows: 28 (part 3, col 10).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L54 numerical-only DAMAGE_DATA table LSI_A_LLC_F_BLASTING_FEAR (Luciel_Duo.lua L1715; §1 L54 numerical-only DAMAGE_DATA table LSI_A_LLC_M_BLASTING_FEAR (Luciel_Duo.lua L1723; §1 L54 numerical-only DAMAGE_DATA table LSI_A_LLC_F_BACKSTAB (Luciel_Duo.lua L17325); §1 L54 numerical-only DAMAGE_DATA table LSI_A_LLC_M_BACKSTAB (Luciel_Duo.lua L17412); §1 L37-105 blocks that index past their skill data need a JUNE_BUGS row; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178-181 June-only SKILL_ID members fail silently (52 SI_ 10000-10699 + 143 RNSI_ 30001-3; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry; §7ae L713-720 June skill-tree ids with no March class state (reform ids 21000+, RNSI_*); §7ae L729-733 June skill types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTi; §7ai L835-841 pass June skip / adjust / level / creator at every ported BFI call.
- Prefix `LLC` = 37 (also owned by 127).
