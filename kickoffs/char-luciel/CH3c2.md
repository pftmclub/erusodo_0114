# June character Lu/Ciel (Stage 8) - Phase CH3c2 - Demonio (129) skills, part 2

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH3c2` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH3c review, CH3c20 review
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_SKILL` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL SKILL` / `NUM`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/CH3c2-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH3 - Skills: second jobs (127, 128, 129), hyper actives, transcendence" "Phase CH1 - Deploy unit: the class core"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the `CH3c2` row of "Phase CH3 - Skills: second jobs (127, 128, 129), hyper actives, transcendence", from `ch0_state_skill.tsv` / `ch0_class_map.tsv` (CH0's amendment names the filters). Each skill is tested
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

- **Functions: 40 to transcribe** (+ 41 thunks) (`awk -F'\t' '$6=="CH3c2"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32b06fb0` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_COMBO_X_EventProcess `32b072f0` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_COMBO_XX_EventProcess `32b07630` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_COMBO_XXX_READY_EventProcess `32b076d0` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_COMBO_XXX_LOOP_EventProcess `32b07780` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_COMBO_XXX_END_EventProcess `32b07a40` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_SHREWD_CUTTER_StateStart `32b07ac0` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_SHREWD_CUTTER_EventProcess `32b07b20` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_SHREWD_CUTTER_ATTACK_A_StateStartFuture `32b07c30` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_SHADOW_FURY_EventProcess `32b07eb0` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_END_EventProcess `32b07ef0` LSI_SA_LDN_TRANSCENDENCE_END_EventProcess `32b0ed50` LSI_SA_LDN_ABYSS_SNIPING_READY_EventProcess `32b0ee00` LSI_SA_LDN_ABYSS_SNIPING_UP_EventProcess `32b0ee40` LSI_SA_LDN_DEMONIC_RUSH_START_EventProcess `32b0ee80` LSI_SA_LDN_DEMONIC_RUSH_LAST_EventProcess `32b135e0` LSI_SA_LDN_DESPERADO_FrameMove `32b13730` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_JUMP_READY_EventProcess `32b13860` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_JUMP_Z_LOOP_EventProcess `32b14030` `32b15290` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_SHREWD_CUTTER_ATTACK_A_EventProcess `32b16fc0` LSI_SA_LDN_DESPERADO_StateStart `32b17040` LSI_SA_LDN_DESPERADO_ATTACK_END_StateStart `32b172d0` LSI_SA_LDN_ABYSS_SNIPING_READY_FrameMove `32b31e90` LSI_SA_LDN_DESPERADO_ATTACK_READY_StateStart `32b32090` LSI_SA_LDN_DESPERADO_ATTACK_FrameMove `32b32550` LSI_A_LDN_F_RAPID_IMPACT_MOVEATTACK_StateStart `32b32800` LSI_A_LDN_F_PUNISHER_FrameMove `32b32b30` LSI_A_LDN_M_PUNISHER_FrameMove `32b32fe0` LSI_SA_LDN_SINISTER_MIND_FrameMove `32b33480` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_WAIT_EventProcess `32b336a0` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_WALK_EventProcess `32b338c0` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_JUMP_LANDING_EventProcess `32b33b00` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_SHADOW_FURY_StateEnd `32b33bf0` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_SHREWD_CUTTER_END_StateStartFuture `32b33dc0` LSI_SA_LDN_GARDEN_OF_HAURES_FrameMove `32b36d90` LSI_SA_LDN_ABYSS_SNIPING_UP_FrameMove `32b3a2f0` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_END_StateEnd `32b43b10` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_START_StateStartFuture `32b43ce0` LSI_SA_LDN_TRANSCENDENCE_TRANSFORM_START_StateStart `32b4ce50`.
  Still `?` (name it by the class pattern from the hint in col 7): `32b14030` `32b4ce50`.
- **States:** the ones whose callbacks are listed above (the state table's col 8 keeps the unsplit `CH3c`).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7y L680 GetLandPosition bMonster true in Luciel class code; §7ac L774 ProcessLockOn (3200fa20) is shared with Luciel; §7ai L835-841 pass June skip / adjust / level / creator at every ported BFI call.
- **Depends on CH3c20** (ProcessLockOn, Garden binders). Garden of Haures spawns 2269 from `32b33dc0` (`@32b340ce`).
