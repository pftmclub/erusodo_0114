# June character Lu/Ciel (Stage 8) - Phase CH1d - Class core: Ciel's common states (CH1e merged in)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH1d` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1b; one at a time in `X2GULuciel.*`
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` (compiled, unreferenced) and `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | none of its own (user, 2026-11-10): the CH1f2 review covers CH1b, CH1d, CH1f, CH1f2; do not write `CH1d-review.md` (any debug define stays on until then) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Transcribe every function the class map gives phase `CH1d` (`awk -F'\t' '$6=="CH1d"' june2017_re/char_luciel/ch0_class_map.tsv`;
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

- **Functions: 30 to transcribe** (+ 32 thunks) (`awk -F'\t' '$6=="CH1d"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae5960` LSI_CIEL_JUMP_UP_EventProcess `32ae5a00` LSI_CIEL_STANDUP_ROLLING_FRONT_FRONT_StateEnd `32ae5a40` LSI_CIEL_STANDUP_ROLLING_FRONT_BACK_StateEnd `32ae5a80` LSI_CIEL_STANDUP_ROLLING_BACK_FRONT_StateEnd `32ae5ac0` LSI_CIEL_STANDUP_ROLLING_BACK_BACK_StateEnd `32ae5b00` LSI_CIEL_STANDUP_ATTACK_FRONT_StateEnd `32ae5b40` LSI_CIEL_STANDUP_ATTACK_BACK_StateEnd `32af1b80` LSI_CIEL_JUMP_DOWN_EventProcess `32af1c30` LSI_CIEL_DAMAGE_DOWN_FRONT_EventProcess `32af1da0` LSI_CIEL_DAMAGE_DOWN_BACK_EventProcess `32af1f20` LSI_CIEL_STANDUP_ROLLING_FRONT_FRONT_EventProcess `32af1fb0` LSI_CIEL_STANDUP_ROLLING_FRONT_BACK_EventProcess `32af2040` LSI_CIEL_STANDUP_ROLLING_BACK_FRONT_EventProcess `32af20d0` LSI_CIEL_STANDUP_ROLLING_BACK_BACK_EventProcess `32af2160` LSI_CIEL_STANDUP_ATTACK_FRONT_EventProcess `32af21f0` LSI_CIEL_STANDUP_ATTACK_BACK_EventProcess `32af5cf0` LSI_CIEL_JUMP_DOWN_NO_ATTACK_EventProcess `32b11720` LSI_CIEL_JUMP_READY_EventProcess `32b11830` LSI_CIEL_DASH_END_FrameMove `32b118d0` LSI_CIEL_DASH_JUMP_EventProcess `32b11ba0` LSI_CIEL_DAMAGE_DOWN_FRONT_FrameMove `32b11c50` LSI_CIEL_DAMAGE_DOWN_BACK_FrameMove `32b12e20` LSI_CIEL_JUMP_DOWN_NO_ATTACK_FrameMoveFuture `32b22710` LSI_CIEL_WAIT_EventProcess `32b22950` LSI_CIEL_WALK_EventProcess `32b22b50` LSI_CIEL_JUMP_LANDING_EventProcess `32b22e30` LSI_CIEL_DASH_EventProcess `32b23050` LSI_CIEL_DASH_END_EventProcess `32b23380` LSI_CIEL_DASH_JUMP_LANDING_EventProcess `32b235c0` LSI_CIEL_DASH_JUMP_POWER_LANDING_EventProcess.
- **States: 53** (`awk -F'\t' '$8=="CH1d"' june2017_re/char_luciel/ch0_state_skill.tsv`): `LSI_CIEL_DIE_FRONT`, `LSI_CIEL_DIE_BACK`, `LSI_CIEL_WAIT`, `LSI_CIEL_WALK`, `LSI_CIEL_JUMP_READY`, `LSI_CIEL_JUMP_UP`, `LSI_CIEL_JUMP_DOWN`, `LSI_CIEL_JUMP_LANDING`, `LSI_CIEL_DASH`, `LSI_CIEL_DASH_END`, `LSI_CIEL_DASH_JUMP`, `LSI_CIEL_DASH_JUMP_LANDING`, `LSI_CIEL_DASH_JUMP_POWER_LANDING`, `LSI_CIEL_DAMAGE_GROGGY`, `LSI_CIEL_DAMAGE_SMALL_FRONT`, `LSI_CIEL_DAMAGE_SMALL_BACK`, `LSI_CIEL_DAMAGE_BIG_FRONT`, `LSI_CIEL_DAMAGE_BIG_BACK`, `LSI_CIEL_DAMAGE_DOWN_FRONT`, `LSI_CIEL_DAMAGE_DOWN_BACK`, `LSI_CIEL_DAMAGE_STANDUP_FRONT`, `LSI_CIEL_DAMAGE_STANDUP_BACK`, `LSI_CIEL_DAMAGE_BOUNDING_DOWN`, `LSI_CIEL_DAMAGE_BOUNDING_LANDCRASH`, `LSI_CIEL_DAMAGE_BOUNDING_UP`, `LSI_CIEL_DAMAGE_AIR_SMALL`, `LSI_CIEL_DAMAGE_AIR_DOWN`, `LSI_CIEL_DAMAGE_AIR_DOWN_INVINCIBLE`, `LSI_CIEL_DAMAGE_AIR_DOWN_LANDING`, `LSI_CIEL_DAMAGE_AIR_FALL`, `LSI_CIEL_DAMAGE_AIR_UP`, `LSI_CIEL_DAMAGE_AIR_FLY_FRONT`, `LSI_CIEL_DAMAGE_AIR_FLY_BACK`, `LSI_CIEL_DAMAGE_REVENGE`, `LSI_CIEL_STANDUP_ROLLING_FRONT_FRONT`, `LSI_CIEL_STANDUP_ROLLING_FRONT_BACK`, `LSI_CIEL_STANDUP_ROLLING_BACK_FRONT`, `LSI_CIEL_STANDUP_ROLLING_BACK_BACK`, `LSI_CIEL_STANDUP_ATTACK_FRONT`, `LSI_CIEL_STANDUP_ATTACK_BACK`, `LSI_CIEL_PEPPER_RUN_READY`, `LSI_CIEL_PEPPER_RUN`, `LSI_CIEL_PEPPER_RUN_END`, `LSI_CIEL_PEPPER_RUN_JUMP_UP`, `LSI_CIEL_PEPPER_RUN_JUMP_DOWN`, `LSI_CIEL_AMUSEMENT_ITEM_PHOENIX_RUN`, `LSI_CIEL_AMUSEMENT_ITEM_PHOENIX_RUN_END`, `LSI_CIEL_DANCE_Z`, `LSI_CIEL_DANCE_X`, `LSI_CIEL_DANCE_WAIT`, `LSI_CIEL_THROW_ITEM`, `LSI_CIEL_SWITCHING`, `LSI_CIEL_JUMP_DOWN_NO_ATTACK`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L254 class code scales the cached state power rate (+0x7cc8), e.g. 32b1cd20.
- **CH1e is merged in** (CH0): every Ciel common state (30 functions).
