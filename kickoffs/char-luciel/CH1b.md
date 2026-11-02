# June character Lu/Ciel (Stage 8) - Phase CH1b - Class core: Lu's common states (CH1c merged in)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH1b` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1a review; one at a time in `X2GULuciel.*`
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` (compiled, unreferenced) and `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | kickoffs/char-luciel/CH1b-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Transcribe every function the class map gives phase `CH1b` (`awk -F'\t' '$6=="CH1b"' june2017_re/char_luciel/ch0_class_map.tsv`;
CH0's amendment names the column), whole, under the plan's "Transcription rules (CH1b-CH1l)". Leave `InitState*` alone (CH1m registers).
Gate: every row written (the Done note lists them with June addresses); build passes; `junediverge.py --check` and `enumscan.py --check` on the
class files PASS. Nothing to play-test: the hand-off says so and names the next phase.

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

- **Functions: 41 to transcribe** (+ 43 thunks) (`awk -F'\t' '$6=="CH1b"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae5540` LSI_LU_JUMP_UP_EventProcess `32ae55e0` LSI_LU_DASH_JUMP_POWER_LANDING_StateStartFuture `32ae5610` LSI_LU_DASH_JUMP_POWER_LANDING_StateStart `32ae5630` LSI_LU_STANDUP_ROLLING_FRONT_FRONT_StateEnd `32ae5670` LSI_LU_STANDUP_ROLLING_FRONT_BACK_StateEnd `32ae56b0` LSI_LU_STANDUP_ROLLING_BACK_FRONT_StateEnd `32ae56f0` LSI_LU_STANDUP_ROLLING_BACK_BACK_StateEnd `32ae5730` LSI_LU_STANDUP_ATTACK_FRONT_StateEnd `32ae5770` LSI_LU_STANDUP_ATTACK_BACK_StateEnd `32ae58a0` LSI_LU_STANDUP_ATTACK_FRONT_NO_DOWN_StateEnd `32ae58e0` LSI_LU_STANDUP_ATTACK_BACK_NO_DOWN_StateEnd `32ae5920` LSI_LU_SWITCHING_StateStartFuture `32aebc10` LSI_LU_WAIT_StateStartFuture `32aebc60` LSI_LU_JUMP_DOWN_EventProcess `32aebd10` LSI_LU_DAMAGE_DOWN_FRONT_EventProcess `32aebeb0` LSI_LU_DAMAGE_DOWN_BACK_EventProcess `32aec060` LSI_LU_STANDUP_ROLLING_FRONT_FRONT_EventProcess `32aec0f0` LSI_LU_STANDUP_ROLLING_FRONT_BACK_EventProcess `32aec180` LSI_LU_STANDUP_ROLLING_BACK_FRONT_EventProcess `32aec210` LSI_LU_STANDUP_ROLLING_BACK_BACK_EventProcess `32aec2a0` LSI_LU_STANDUP_ATTACK_FRONT_EventProcess `32aec330` LSI_LU_STANDUP_ATTACK_BACK_EventProcess `32af0150` LSI_LU_STANDUP_ATTACK_FRONT_NO_DOWN_EventProcess `32af01e0` LSI_LU_STANDUP_ATTACK_BACK_NO_DOWN_EventProcess `32af1150` LSI_LU_JUMP_DOWN_NO_ATTACK_EventProcess `32b0fd90` LSI_LU_WAIT_StateStart `32b0fde0` LSI_LU_JUMP_READY_EventProcess `32b0fef0` LSI_LU_DASH_END_FrameMove `32b0ff90` LSI_LU_DASH_JUMP_EventProcess `32b10260` LSI_LU_DAMAGE_DOWN_FRONT_FrameMove `32b10310` LSI_LU_DAMAGE_DOWN_BACK_FrameMove `32b11670` LSI_LU_JUMP_DOWN_NO_ATTACK_FrameMoveFuture `32b20b30` LSI_LU_WAIT_EventProcess `32b20d70` LSI_LU_WALK_EventProcess `32b20f70` LSI_LU_JUMP_LANDING_EventProcess `32b21250` LSI_LU_DASH_EventProcess `32b214c0` LSI_LU_DASH_END_EventProcess `32b21830` LSI_LU_DASH_JUMP_LANDING_EventProcess `32b21a70` LSI_LU_DASH_JUMP_POWER_LANDING_EventProcess `32b3a250` LSI_LU_DIE_FRONT_StateStart `32b3a270` LSI_LU_DIE_BACK_StateStart.
- **States: 55** (`awk -F'\t' '$8=="CH1b"' june2017_re/char_luciel/ch0_state_skill.tsv`): `LSI_LU_DIE_FRONT`, `LSI_LU_DIE_BACK`, `LSI_LU_WAIT`, `LSI_LU_WALK`, `LSI_LU_JUMP_READY`, `LSI_LU_JUMP_UP`, `LSI_LU_JUMP_DOWN`, `LSI_LU_JUMP_LANDING`, `LSI_LU_DASH`, `LSI_LU_DASH_END`, `LSI_LU_DASH_JUMP`, `LSI_LU_DASH_JUMP_LANDING`, `LSI_LU_DASH_JUMP_POWER_LANDING`, `LSI_LU_DAMAGE_GROGGY`, `LSI_LU_DAMAGE_SMALL_FRONT`, `LSI_LU_DAMAGE_SMALL_BACK`, `LSI_LU_DAMAGE_BIG_FRONT`, `LSI_LU_DAMAGE_BIG_BACK`, `LSI_LU_DAMAGE_DOWN_FRONT`, `LSI_LU_DAMAGE_DOWN_BACK`, `LSI_LU_DAMAGE_STANDUP_FRONT`, `LSI_LU_DAMAGE_STANDUP_BACK`, `LSI_LU_DAMAGE_BOUNDING_DOWN`, `LSI_LU_DAMAGE_BOUNDING_LANDCRASH`, `LSI_LU_DAMAGE_BOUNDING_UP`, `LSI_LU_DAMAGE_AIR_SMALL`, `LSI_LU_DAMAGE_AIR_DOWN`, `LSI_LU_DAMAGE_AIR_DOWN_INVINCIBLE`, `LSI_LU_DAMAGE_AIR_DOWN_LANDING`, `LSI_LU_DAMAGE_AIR_FALL`, `LSI_LU_DAMAGE_AIR_UP`, `LSI_LU_DAMAGE_AIR_FLY_FRONT`, `LSI_LU_DAMAGE_AIR_FLY_BACK`, `LSI_LU_DAMAGE_REVENGE`, `LSI_LU_STANDUP_ROLLING_FRONT_FRONT`, `LSI_LU_STANDUP_ROLLING_FRONT_BACK`, `LSI_LU_STANDUP_ROLLING_BACK_FRONT`, `LSI_LU_STANDUP_ROLLING_BACK_BACK`, `LSI_LU_STANDUP_ATTACK_FRONT`, `LSI_LU_STANDUP_ATTACK_BACK`, `LSI_LU_STANDUP_ATTACK_FRONT_NO_DOWN`, `LSI_LU_STANDUP_ATTACK_BACK_NO_DOWN`, `LSI_LU_PEPPER_RUN_READY`, `LSI_LU_PEPPER_RUN`, `LSI_LU_PEPPER_RUN_END`, `LSI_LU_PEPPER_RUN_JUMP_UP`, `LSI_LU_PEPPER_RUN_JUMP_DOWN`, `LSI_LU_AMUSEMENT_ITEM_PHOENIX_RUN`, `LSI_LU_AMUSEMENT_ITEM_PHOENIX_RUN_END`, `LSI_LU_DANCE_Z`, `LSI_LU_DANCE_X`, `LSI_LU_DANCE_WAIT`, `LSI_LU_THROW_ITEM`, `LSI_LU_SWITCHING`, `LSI_LU_JUMP_DOWN_NO_ATTACK`.
- **Skills: 1** (part 2 of `ch0_state_skill.tsv`, col 6): 900 SI_ETC_WS_COMMON_LOVE.
- Tree rows: 7 (part 3, col 10).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L254 class code scales the cached state power rate (+0x7cc8), e.g. 32b1cd20.
- **CH1c is merged in** (CH0): every Lu common state, movement and the rest (41 functions). The state table's col 8 says CH1b for all of them.
