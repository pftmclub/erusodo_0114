# June character Lu/Ciel (Stage 8) - Phase CH1f - Class core: Duo combos, part 1

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH1f` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1d; one at a time in `X2GULuciel.*`
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` (compiled, unreferenced) and `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | none of its own (user, 2026-11-10): the CH1f2 review covers CH1b, CH1d, CH1f, CH1f2; do not write `CH1f-review.md` (any debug define stays on until then) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Transcribe every function the class map gives phase `CH1f` (`awk -F'\t' '$6=="CH1f"' june2017_re/char_luciel/ch0_class_map.tsv`;
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

- **Functions: 39 to transcribe** (+ 39 thunks) (`awk -F'\t' '$6=="CH1f"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae57b0` LSI_LU_COMBO_XXXZ_READY_StateStart `32ae57d0` LSI_LU_COMBO_XXXZ_LOOP_EventProcess `32ae5810` LSI_LU_JUMP_COMBO_Z_FrameMoveFuture `32ae5850` LSI_LU_JUMP_COMBO_X_FrameMoveFuture `32ae5b80` LSI_CIEL_COMBO_XXXZ_START_StateStart `32ae5ba0` LSI_CIEL_COMBO_XXXZ_LOOP_StateEnd `32ae5be0` LSI_CIEL_JUMP_COMBO_Z_FrameMoveFuture `32ae5c20` LSI_CIEL_JUMP_COMBO_X_FrameMoveFuture `32ae6050` LSI_LU_JUMP_COMBO_Z_FrameMoveFuture `32aec3c0` LSI_LU_COMBO_Z_EventProcess `32aec6b0` LSI_LU_COMBO_ZZ_EventProcess `32aec9a0` LSI_LU_COMBO_ZZZ_EventProcess `32aece00` LSI_LU_COMBO_ZZZZ_EventProcess `32aed0f0` LSI_LU_COMBO_ZZZZX_EventProcess `32aed320` LSI_LU_COMBO_X_EventProcess `32aed640` LSI_LU_COMBO_XX_EventProcess `32aed9f0` LSI_LU_COMBO_XXX_EventProcess `32aede70` LSI_LU_COMBO_XXXX_EventProcess `32aee0a0` LSI_LU_COMBO_XXXZ_READY_EventProcess `32aee130` LSI_LU_COMBO_XXXZ_CHARGE_EventProcess `32aee1b0` LSI_LU_COMBO_XXXZ_END_EventProcess `32aee4a0` LSI_LU_COMBO_XXXZX_EventProcess `32aee6d0` LSI_LU_DASH_COMBO_Z_EventProcess `32aee9e0` LSI_LU_DASH_COMBO_X_START_EventProcess `32aeea40` LSI_LU_DASH_COMBO_X_LANDING_EventProcess `32aeee30` LSI_LU_JUMP_COMBO_X_EventProcess `32aeee70` LSI_LU_DASH_JUMP_COMBO_Z_EventProcess `32aef050` LSI_LU_DASH_JUMP_COMBO_ZX_EventProcess `32aef160` LSI_LU_DASH_JUMP_COMBO_X_EventProcess `32aef530` LSI_LU_DASH_JUMP_COMBO_XX_START_FrameMove `32aef660` LSI_LU_DASH_JUMP_COMBO_XX_START_EventProcess `32aef690` LSI_LU_DASH_JUMP_COMBO_XX_END_EventProcess `32aef960` LSI_LU_DASH_JUMP_COMBO_XXX_EventProcess `32aefc40` LSI_LU_DASH_JUMP_COMBO_XXXX_EventProcess `32aeff50` LSI_LU_DASH_JUMP_COMBO_XXXXZ_EventProcess `32af2280` LSI_CIEL_COMBO_Z_EventProcess `32af2570` LSI_CIEL_COMBO_ZZ_EventProcess `32af2860` LSI_CIEL_COMBO_ZZZ_EventProcess `32af2cc0` LSI_CIEL_COMBO_ZZZZ_EventProcess.
- **States: 52** (`awk -F'\t' '$8=="CH1f"' june2017_re/char_luciel/ch0_state_skill.tsv`): `LSI_LU_COMBO_Z`, `LSI_LU_COMBO_ZZ`, `LSI_LU_COMBO_ZZZ`, `LSI_LU_COMBO_ZZZZ`, `LSI_LU_COMBO_ZZZZX`, `LSI_LU_COMBO_X`, `LSI_LU_COMBO_XX`, `LSI_LU_COMBO_XXX`, `LSI_LU_COMBO_XXXX`, `LSI_LU_COMBO_XXXZ_READY`, `LSI_LU_COMBO_XXXZ_CHARGE`, `LSI_LU_COMBO_XXXZ_LOOP`, `LSI_LU_COMBO_XXXZ_END`, `LSI_LU_COMBO_XXXZX`, `LSI_LU_DASH_COMBO_Z`, `LSI_LU_DASH_COMBO_X_START`, `LSI_LU_DASH_COMBO_X_LANDING`, `LSI_LU_JUMP_COMBO_Z`, `LSI_LU_JUMP_COMBO_X`, `LSI_LU_DASH_JUMP_COMBO_Z`, `LSI_LU_DASH_JUMP_COMBO_ZX`, `LSI_LU_DASH_JUMP_COMBO_X`, `LSI_LU_DASH_JUMP_COMBO_XX_START`, `LSI_LU_DASH_JUMP_COMBO_XX_END`, `LSI_LU_DASH_JUMP_COMBO_XXX`, `LSI_LU_DASH_JUMP_COMBO_XXXX`, `LSI_LU_DASH_JUMP_COMBO_XXXXZ`, `LSI_CIEL_COMBO_Z`, `LSI_CIEL_COMBO_ZZ`, `LSI_CIEL_COMBO_ZZZ`, `LSI_CIEL_COMBO_ZZZZ`, `LSI_CIEL_COMBO_ZZZZX`, `LSI_CIEL_COMBO_X`, `LSI_CIEL_COMBO_XX`, `LSI_CIEL_COMBO_XXX`, `LSI_CIEL_COMBO_XXXX`, `LSI_CIEL_COMBO_XXXZ_START`, `LSI_CIEL_COMBO_XXXZ_LOOP`, `LSI_CIEL_COMBO_XXXZ_END`, `LSI_CIEL_COMBO_XXXZX`, `LSI_CIEL_DASH_COMBO_Z`, `LSI_CIEL_DASH_COMBO_X`, `LSI_CIEL_JUMP_COMBO_Z`, `LSI_CIEL_JUMP_COMBO_X`, `LSI_CIEL_DASH_JUMP_COMBO_Z`, `LSI_CIEL_DASH_JUMP_COMBO_ZX`, `LSI_CIEL_DASH_JUMP_COMBO_X`, `LSI_CIEL_DASH_JUMP_COMBO_XX`, `LSI_CIEL_DASH_JUMP_COMBO_XXX`, `LSI_CIEL_DASH_JUMP_COMBO_XXXX`, `LSI_CIEL_DASH_JUMP_COMBO_XXXXZ`, `LSI_CIEL_DASH_JUMP_COMBO_XXXXZ_LANDING`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L254 class code scales the cached state power rate (+0x7cc8), e.g. 32b1cd20.
- **Correction to the plan:** `LSI_A_*` states are actives (skills, CH2 / CH3 by class), not combos. CH1f / CH1f2 are the `LSI_LU_COMBO_*` / `LSI_CIEL_COMBO_*` states (77 functions, split in table order).
