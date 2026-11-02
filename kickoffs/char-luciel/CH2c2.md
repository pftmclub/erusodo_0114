# June character Lu/Ciel (Stage 8) - Phase CH2c2 - Royal Guard (38) skills, part 2

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH2c2` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH2c review
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_SKILL` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL SKILL` / `NUM`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/CH2c2-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: base and first jobs (200, 37, 38, 39)" "Phase CH1 - Deploy unit: the class core"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the `CH2c2` row of "Phase CH2 - Skills: base and first jobs (200, 37, 38, 39)", from `ch0_state_skill.tsv` / `ch0_class_map.tsv` (CH0's amendment names the filters). Each skill is tested
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

- **Functions: 35 to transcribe** (+ 32 thunks) (`awk -F'\t' '$6=="CH2c2"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32b01450` LSI_A_LCG_F_SOUL_SHOCK_EventProcess `32b01650` LSI_A_LCG_F_SOUL_STRIKE_READY_EventProcess `32b01690` LSI_A_LCG_F_SOUL_STRIKE_END_EventProcess `32b01890` LSI_A_LCG_M_SOUL_STRIKE_READY_EventProcess `32b13260` LSI_SA_LCG_FANTASIE_IMPROMPTU_CANTABILE_WAIT_FrameMoveFuture `32b15680` SetLeftHandRotateBone `32b16200` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_WAIT_FrameMove `32b1d2a0` LSI_SA_LCG_DEMONIC_BREATH_START_StateStartFuture `32b1d350` LSI_SA_LCG_DEMONIC_BREATH_START_EventProcess `32b1d490` LSI_SA_LCG_DEMONIC_BREATH_CHARGING_EventProcess `32b1d5f0` LSI_SA_LCG_FANTASIE_IMPROMPTU_ACCELERANDO_END_EventProcess `32b1d8d0` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_READY_StateStartFuture `32b1d970` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_WAIT_EventProcess `32b1dc50` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_BIG_CHARGE_EventProcess `32b1de10` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_SMALL_FIRE_EventProcess `32b1e490` LSI_A_LCG_F_SOUL_STRIKE_READY_StateEndFuture `32b1e540` LSI_A_LCG_F_SOUL_STRIKE_LOOP_EventProcess `32b1e630` LSI_A_LCG_M_SOUL_STRIKE_LOOP_EventProcess `32b20670` `32b2ba70` LSI_SA_LCG_DEMONIC_BREATH_FIRE_FrameMove `32b2bf20` LSI_SA_LCG_FANTASIE_IMPROMPTU_CANTABILE_WAIT_FrameMove `32b2c270` LSI_SA_LCG_FANTASIE_IMPROMPTU_ACCELERANDO_WAIT_FrameMove `32b2c5e0` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_BIG_CHARGE_FrameMove `32b2c7d0` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_SMALL_FIRE_FrameMove `32b2cd70` LSI_SA_LCG_DEATH_CRISIS_OF_MARBAS_BIG_FIRE_FrameMove `32b2cfc0` LSI_SA_LCG_ESCHATON_FrameMove `32b2db60` LSI_A_LCG_F_SOUL_SHOCK_FrameMove `32b2e080` LSI_A_LCG_M_SOUL_SHOCK_FrameMove `32b2e5a0` LSI_A_LCG_F_SOUL_STRIKE_LOOP_FrameMove `32b2ec50` LSI_A_LCG_M_SOUL_STRIKE_LOOP_FrameMove `32b30bc0` LSI_A_LCN_F_SHADOW_FrameMove `32b30d40` LSI_A_LCN_M_SHADOW_FrameMove `32b3db80` `32b3dd80` LSI_SA_LCG_SOUL_EXTORTION_StateEnd `32b3dea0` LSI_A_LCG_F_SOUL_STRIKE_READY_StateEnd.
  Still `?` (name it by the class pattern from the hint in col 7): `32b20670` `32b3db80`.
- **States:** the ones whose callbacks are listed above (the state table's col 8 keeps the unsplit `CH2c`).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7ai L835-841 pass June skip / adjust / level / creator at every ported BFI call.
