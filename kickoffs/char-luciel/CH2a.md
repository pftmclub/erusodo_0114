# June character Lu/Ciel (Stage 8) - Phase CH2a - Duo (200) skills

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH2a` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1 review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_SKILL` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL SKILL` / `NUM`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | none of its own (user, 2026-11-10): the CH2b review covers CH2a, CH2b; do not write `CH2a-review.md` (any debug define stays on until then) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: base and first jobs (200, 37, 38, 39)" "Phase CH1 - Deploy unit: the class core"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the `CH2a` row of "Phase CH2 - Skills: base and first jobs (200, 37, 38, 39)", from `ch0_state_skill.tsv` / `ch0_class_map.tsv` (CH0's amendment names the filters). Each skill is tested
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

- **Functions: 32 to transcribe** (+ 32 thunks) (`awk -F'\t' '$6=="CH2a"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae61b0` LSI_SA_LD_TRIPLE_SHOT_Init `32ae6240` LSI_SA_LD_TRIPLE_SHOT_FrameMove `32ae6250` LSI_SA_LD_STOMP_START_Init `32ae62e0` LSI_SA_LD_STOMP_START_FrameMove `32ae62f0` LSI_SA_LD_STOMP_LANDING_FrameMove `32ae6300` LSI_SA_LD_FLICK_SHOT_Init `32ae63c0` LSI_SA_LD_EVIL_CLAW_Init `32ae6480` LSI_SA_LD_SOUL_INFEST_Init `32ae65a0` LSI_SA_LD_NETHER_SHREDDER_Init `32ae66a0` LSI_SA_LD_NETHER_SHREDDER_FrameMove `32ae66b0` LSI_A_LD_M_HEAVY_HIT_FrameMove `32ae66c0` LSI_A_LD_F_HEAVY_HIT_Init `32ae6780` LSI_A_LD_F_HEAVY_HIT_FrameMove `32ae6790` LSI_A_LD_M_HAUNTING_BEAT_FrameMove `32ae67a0` LSI_A_LD_F_HAUNTING_BEAT_Init `32ae6870` LSI_A_LD_F_HAUNTING_BEAT_FrameMove `32afdff0` LSI_SA_LD_TRIPLE_SHOT_EventProcess `32afe1f0` LSI_SA_LD_STOMP_START_EventProcess `32afe250` LSI_SA_LD_STOMP_LANDING_EventProcess `32afe450` LSI_SA_LD_FLICK_SHOT_EventProcess `32afe650` LSI_SA_LD_EVIL_CLAW_EventProcess `32afe850` LSI_SA_LD_SOUL_INFEST_EventProcess `32afea50` LSI_SA_LD_NETHER_SHREDDER_EventProcess `32afec50` LSI_A_LD_M_HEAVY_HIT_EventProcess `32afee50` LSI_A_LD_F_HEAVY_HIT_EventProcess `32aff050` LSI_A_LD_M_HAUNTING_BEAT_EventProcess `32aff250` LSI_A_LD_F_HAUNTING_BEAT_EventProcess `32b1cd20` LSI_A_LD_M_HEAVY_HIT_StateStart `32b1cf90` LSI_A_LD_F_HEAVY_HIT_StateStart `32b29780` LSI_SA_LD_FLICK_SHOT_FrameMove `32b29af0` LSI_SA_LD_EVIL_CLAW_FrameMove `32b29fb0` LSI_SA_LD_SOUL_INFEST_FrameMove.
- **States: 15** (`awk -F'\t' '$8=="CH2a"' june2017_re/char_luciel/ch0_state_skill.tsv`): `LSI_SA_F_COMMON_AURA_DEPRAVITY_ACCEL`, `LSI_SA_F_COMMON_AURA_PUNISHING_ACCEL`, `LSI_SA_M_COMMON_AURA_DEPRAVITY_ACCEL`, `LSI_SA_M_COMMON_AURA_PUNISHING_ACCEL`, `LSI_SA_LD_TRIPLE_SHOT`, `LSI_SA_LD_STOMP_START`, `LSI_SA_LD_STOMP_LANDING`, `LSI_SA_LD_FLICK_SHOT`, `LSI_SA_LD_EVIL_CLAW`, `LSI_SA_LD_SOUL_INFEST`, `LSI_SA_LD_NETHER_SHREDDER`, `LSI_A_LD_F_HEAVY_HIT`, `LSI_A_LD_M_HEAVY_HIT`, `LSI_A_LD_F_HAUNTING_BEAT`, `LSI_A_LD_M_HAUNTING_BEAT`.
- **Skills: 19** (part 2 of `ch0_state_skill.tsv`, col 6): 208 SI_SA_COMMON_AURA_DARKNESS_ACCEL, 30001 RNSI_SA_LUCIEL_TRIPLE_SHOT, 30002 RNSI_SA_LUCIEL_STOMP, 30003 RNSI_SA_LUCIEL_FLICK_SHOT, 30004 RNSI_SA_LUCIEL_NETHER_SHREDDER, 30005 RNSI_SA_LUCIEL_EVIL_CLAW, 30006 RNSI_SA_LUCIEL_SOUL_INFEST, 30025 RNSI_SA_LUCIEL_TRIPLE_SHOT_UPGRADE, 30026 RNSI_SA_LUCIEL_STOMP_UPGRADE, 30027 RNSI_SA_LUCIEL_FLICK_SHOT_UPGRADE, 30028 RNSI_SA_LUCIEL_NETHER_SHREDDER_UPGRADE, 30029 RNSI_SA_LUCIEL_EVIL_CLAW_UPGRADE, 30030 RNSI_SA_LUCIEL_SOUL_INFEST_UPGRADE, 30201 RNSI_A_LUCIEL_HEAVY_HIT, 30202 RNSI_A_LUCIEL_HAUNTING_BEAT, 30211 RNSI_A_LUCIEL_HAUNTING_BEAT_UPGRADE, 30212 RNSI_A_LUCIEL_HEAVY_HIT_UPGRADE, 30301 RNSI_A_LUCIEL_AURA_PUNISHMENT_ACCEL, 30302 RNSI_A_LUCIEL_AURA_PUNISHMENT_ACCEL_UPGRADE.
- Tree rows: 126 (part 3, col 10).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L54 numerical-only DAMAGE_DATA table LSI_A_LD_F_HEAVY_HIT (Luciel_Duo.lua L15942); §1 L54 numerical-only DAMAGE_DATA table LSI_A_LD_F_HAUNTING_BEAT (Luciel_Duo.lua L16082; §1 L37-105 blocks that index past their skill data need a JUNE_BUGS row; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178-181 June-only SKILL_ID members fail silently (52 SI_ 10000-10699 + 143 RNSI_ 30001-3; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry; §7ae L713-720 June skill-tree ids with no March class state (reform ids 21000+, RNSI_*); §7ae L729-733 June skill types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTi.
- First owner class 200 (prefixes `LD` / `LUCIEL`): states in `ch0_state_skill.tsv` col 8, skills col 6. Skill 208 (no tree row) and its `LSI_SA_[F|M]_COMMON_AURA_DEPRAVITY_ACCEL` are here.
