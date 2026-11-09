# June character Lu/Ciel (Stage 8) - Phase CH3b - Noblesse (128) skills

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH3b` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH3a |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_SKILL` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL SKILL` / `NUM`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/CH3b-review.md (Opus 5.5 high; the phase writes it when done). **It covers CH3a, CH3b** (user, 2026-11-10): every covered Done note, `flagcheck --range` over all of them, `--check-all`, retires any debug define |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH3 - Skills: second jobs (127, 128, 129), hyper actives, transcendence" "Phase CH1 - Deploy unit: the class core"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the `CH3b` row of "Phase CH3 - Skills: second jobs (127, 128, 129), hyper actives, transcendence", from `ch0_state_skill.tsv` / `ch0_class_map.tsv` (CH0's amendment names the filters). Each skill is tested
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

- **Functions: 45 to transcribe** (+ 51 thunks) (`awk -F'\t' '$6=="CH3b"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae77f0` LSI_SA_LCG_FANTASIE_IMPROMPTU_2ND_MOVEMENT_Init `32ae7f00` LSI_SA_LCN_F_SWIFT_SMASH_Init `32ae7f30` LSI_SA_LCN_SPECTRAL_SPEAR_Init `32ae7fd0` LSI_SA_LCN_SHADOW_BOLT_START_Init `32ae80a0` LSI_SA_LCN_SHADOW_BOLT_START_StateStartFuture `32ae80d0` LSI_SA_LCN_TWILIGHT_JUDGEMENT_Init `32ae8390` LSI_A_LCN_F_SUMMON_FAMILIAR_Init `32aea000` LSI_A_LCN_SOUL_SPREAD_LU_READY_Init `32aea270` LSI_SA_LCN_SOUL_LOCK_Init `32aea2f0` LSI_SA_LCN_MAGDONIC_CANNON_Init `32aea5c0` LSI_SA_LCN_RAIN_OF_CHAOS_Init `32aea5d0` LSI_SA_LCN_RAIN_OF_CHAOS_StateStart `32aea5e0` LSI_SA_LCN_RAIN_OF_CHAOS_StateEnd `32b01b50` LSI_SA_LCN_F_SWIFT_SMASH_EventProcess `32b01d60` LSI_SA_LCN_SHADOW_BOLT_END_EventProcess `32b01f80` LSI_SA_LCN_F_TWILIGHT_JUDGEMENT_REATTACK_EventProcess `32b0e710` LSI_A_LCN_SOUL_SPREAD_LU_READY_EventProcess `32b0e7f0` LSI_A_LCN_SOUL_SPREAD_LU_LOOP_StateEndFuture `32b0e850` LSI_A_LCN_SOUL_SPREAD_CIEL_READY_EventProcess `32b0e930` LSI_A_LCN_SOUL_SPREAD_CIEL_LOOP_StateEndFuture `32b0e990` LSI_SA_LCN_MAGDONIC_CANNON_EventProcess `32b1e200` LSI_SA_LCG_FANTASIE_IMPROMPTU_2ND_MOVEMENT_EventProcess `32b20a10` `32b2d2a0` LSI_SA_LCG_FANTASIE_IMPROMPTU_2ND_MOVEMENT_FrameMove `32b2f4b0` LSI_SA_LCN_SPECTRAL_SPEAR_FrameMove `32b2fba0` LSI_SA_LCN_SHADOW_BOLT_START_FrameMove `32b2ff50` LSI_SA_LCN_SHADOW_BOLT_START_EventProcess `32b30000` LSI_SA_LCN_SHADOW_BOLT_END_StateEndFuture `32b30090` LSI_SA_LCN_SHADOW_BOLT_CONTROL_EventProcess `32b30500` LSI_SA_LCN_SHADOW_BOLT_CONTROL_StateStart `32b30560` LSI_SA_LCN_SHADOW_BOLT_CONTROL_StateStartFuture `32b305c0` LSI_SA_LCN_SHADOW_BOLT_CONTROL_StateStart `32b30620` LSI_SA_LCN_SHADOW_BOLT_CONTROL_StateStart `32b30680` LSI_SA_LCN_SHADOW_BOLT_CONTROL_StateStart `32b306e0` LSI_SA_LCN_SHADOW_BOLT_CONTROL_StateStart `32b30740` LSI_SA_LCN_SHADOW_BOLT_CONTROL_StateStart `32b307a0` LSI_SA_LCN_SHADOW_BOLT_CONTROL_StateStart `32b30800` LSI_SA_LCN_SHADOW_BOLT_CONTROL_StateStart `32b30860` LSI_SA_LCN_SHADOW_BOLT_CONTROL_StateStart `32b308c0` LSI_SA_LCN_TWILIGHT_JUDGEMENT_StateStart `32b308e0` LSI_SA_LCN_F_TWILIGHT_JUDGEMENT_REATTACK_StateStart `32b369b0` LSI_A_LCN_SOUL_SPREAD_LU_LOOP_FrameMove `32b36ba0` LSI_A_LCN_SOUL_SPREAD_CIEL_LOOP_FrameMove `32b39e90` LSI_A_LCN_F_SUMMON_FAMILIAR_FrameMove `32b3e120` LSI_SA_LCN_F_SWIFT_SMASH_FrameMove.
  Still `?` (name it by the class pattern from the hint in col 7): `32b20a10`.
- **States: 22** (`awk -F'\t' '$8=="CH3b"' june2017_re/char_luciel/ch0_state_skill.tsv`): `LSI_SA_LCG_FANTASIE_IMPROMPTU_2ND_MOVEMENT`, `LSI_SA_LCN_F_SWIFT_SMASH`, `LSI_SA_LCN_M_SWIFT_SMASH`, `LSI_SA_LCN_SPECTRAL_SPEAR`, `LSI_SA_LCN_SHADOW_BOLT_START`, `LSI_SA_LCN_SHADOW_BOLT_CONTROL`, `LSI_SA_LCN_SHADOW_BOLT_END`, `LSI_SA_LCN_TWILIGHT_JUDGEMENT`, `LSI_SA_LCN_F_TWILIGHT_JUDGEMENT_REATTACK`, `LSI_SA_LCN_M_TWILIGHT_JUDGEMENT_REATTACK`, `LSI_A_LCN_F_SUMMON_FAMILIAR`, `LSI_A_LCN_M_SUMMON_FAMILIAR`, `LSI_A_LCN_SOUL_SPREAD_LU_READY`, `LSI_A_LCN_SOUL_SPREAD_LU_LOOP`, `LSI_A_LCN_SOUL_SPREAD_LU_END`, `LSI_A_LCN_SOUL_SPREAD_CIEL_READY`, `LSI_A_LCN_SOUL_SPREAD_CIEL_LOOP`, `LSI_A_LCN_SOUL_SPREAD_CIEL_END`, `LSI_SA_LCN_SOUL_LOCK`, `LSI_SA_LCN_MAGDONIC_CANNON`, `LSI_SA_LCN_MAGDONIC_CANNON_FLY`, `LSI_SA_LCN_RAIN_OF_CHAOS`.
- **Skills: 16** (part 2 of `ch0_state_skill.tsv`, col 6): 30016 RNSI_SA_LCN_SHADOW_BOLT, 30022 RNSI_SA_LCN_TWILIGHT_JUDGEMENT, 30023 RNSI_SA_LCN_FANTASIE_IMPROMPTU_2ND_MOVEMENT, 30024 RNSI_SA_LCN_SPECTRAL_SPEAR, 30057 RNSI_SA_LCN_SOUL_LOCK, 30058 RNSI_SA_LCN_MAGDONIC_CANNON, 30071 RNSI_SA_LCN_SHADOW_BOLT_UPGRADE, 30072 RNSI_SA_LCN_TWILIGHT_JUDGEMENT_UPGRADE, 30073 RNSI_SA_LCN_FANTASIE_IMPROMPTU_2ND_MOVEMENT_UPGRADE, 30074 RNSI_SA_LCN_SPECTRAL_SPEAR_UPGRADE, 30084 RNSI_SA_LCN_RAIN_OF_CHAOS, 30208 RNSI_A_LCN_SUMMON_FAMILIAR, 30210 RNSI_A_LCN_SWIFT_SMASH, 30305 RNSI_A_LCN_SOUL_SPREAD, 30311 RNSI_A_LCN_SUMMON_FAMILIAR_UPGRADE, 30312 RNSI_A_LCN_SWIFT_SMASH_UPGRADE.
- Tree rows: 16 (part 3, col 10).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178-181 June-only SKILL_ID members fail silently (52 SI_ 10000-10699 + 143 RNSI_ 30001-3; §7 L240 SetBuffAccumulationCount 322a7660 / AddBuffAccumulationCount 322a8a70 callers in; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry; §7ae L713-720 June skill-tree ids with no March class state (reform ids 21000+, RNSI_*); §7ae L729-733 June skill types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTi; §7ai L835-841 pass June skip / adjust / level / creator at every ported BFI call.
- Prefix `LCN` = 128. Shadow Bolt spawns 1908 from `LSI_SA_LCN_SHADOW_BOLT_START_FrameMove` `32b2fba0` (`@32b2fe73`).
