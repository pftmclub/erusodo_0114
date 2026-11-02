# June character Lu/Ciel (Stage 8) - Phase CH1f2 - Class core: Duo combos, part 2

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH1f2` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1f review; one at a time in `X2GULuciel.*`
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` (compiled, unreferenced) and `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | kickoffs/char-luciel/CH1f2-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Transcribe every function the class map gives phase `CH1f2` (`awk -F'\t' '$6=="CH1f2"' june2017_re/char_luciel/ch0_class_map.tsv`;
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

- **Functions: 38 to transcribe** (+ 38 thunks) (`awk -F'\t' '$6=="CH1f2"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32af2fb0` LSI_CIEL_COMBO_ZZZZX_EventProcess `32af31e0` LSI_CIEL_COMBO_X_EventProcess `32af3500` LSI_CIEL_COMBO_XX_EventProcess `32af38b0` LSI_CIEL_COMBO_XXX_EventProcess `32af3d30` LSI_CIEL_COMBO_XXXX_EventProcess `32af3f60` LSI_CIEL_COMBO_XXXZ_START_EventProcess `32af3fe0` LSI_CIEL_COMBO_XXXZ_LOOP_EventProcess `32af41e0` LSI_CIEL_COMBO_XXXZ_END_EventProcess `32af44d0` LSI_CIEL_COMBO_XXXZX_EventProcess `32af4700` LSI_CIEL_DASH_COMBO_Z_EventProcess `32af4b90` LSI_CIEL_DASH_COMBO_X_EventProcess `32af4f80` LSI_CIEL_JUMP_COMBO_X_EventProcess `32af4fc0` LSI_CIEL_DASH_JUMP_COMBO_Z_EventProcess `32af51b0` LSI_CIEL_DASH_JUMP_COMBO_ZX_EventProcess `32af52c0` LSI_CIEL_DASH_JUMP_COMBO_X_EventProcess `32af5690` LSI_CIEL_DASH_JUMP_COMBO_XX_EventProcess `32af5790` LSI_CIEL_DASH_JUMP_COMBO_XXX_EventProcess `32af5890` LSI_CIEL_DASH_JUMP_COMBO_XXXX_EventProcess `32af5990` LSI_CIEL_DASH_JUMP_COMBO_XXXXZ_EventProcess `32af59c0` LSI_CIEL_DASH_JUMP_COMBO_XXXXZ_FrameMove `32af5af0` LSI_CIEL_DASH_JUMP_COMBO_XXXXZ_LANDING_EventProcess `32b103b0` LSI_LU_COMBO_XXXZ_CHARGE_StateEnd `32b10540` LSI_LU_JUMP_COMBO_Z_EventProcess `32b11cf0` LSI_CIEL_JUMP_COMBO_Z_EventProcess `32b12ed0` LSI_LU_JUMP_COMBO_Z_EventProcess `32b21cd0` LSI_LU_COMBO_XXXZ_END_FrameMove `32b21f80` LSI_LU_DASH_JUMP_COMBO_ZX_FrameMove `32b23820` LSI_CIEL_COMBO_X_FrameMove `32b23b80` LSI_CIEL_COMBO_XX_FrameMove `32b23ee0` LSI_CIEL_COMBO_XXX_FrameMove `32b24560` LSI_CIEL_COMBO_XXXX_FrameMove `32b248c0` LSI_CIEL_COMBO_XXXZ_END_FrameMove `32b24c40` LSI_CIEL_DASH_COMBO_X_FrameMove `32b24fa0` LSI_CIEL_JUMP_COMBO_X_FrameMove `32b25300` LSI_CIEL_DASH_JUMP_COMBO_ZX_FrameMove `32b25610` LSI_CIEL_DASH_JUMP_COMBO_XX_FrameMove `32b25970` LSI_CIEL_DASH_JUMP_COMBO_XXX_FrameMove `32b25cd0` LSI_CIEL_DASH_JUMP_COMBO_XXXX_FrameMove.
- **States:** the ones whose callbacks are listed above (the state table's col 8 keeps the unsplit `CH1f`).
