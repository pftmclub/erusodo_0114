# June character Lu/Ciel (Stage 8) - Phase CH1g2 - Class core: Chiliarch and Royal Guard job states, part 2

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH1g2` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1g review; one at a time in `X2GULuciel.*`
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` (compiled, unreferenced) and `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | kickoffs/char-luciel/CH1g2-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Transcribe every function the class map gives phase `CH1g2` (`awk -F'\t' '$6=="CH1g2"' june2017_re/char_luciel/ch0_class_map.tsv`;
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

- **Functions: 38 to transcribe** (+ 62 thunks) (`awk -F'\t' '$6=="CH1g2"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32af91e0` LSI_LCG_F_COMBO_ZZZX_EventProcess `32af9400` LSI_LCG_F_COMBO_ZZZXX_END_EventProcess `32af9690` LSI_LCG_F_COMBO_ZZZXZ_LOOP_EventProcess `32af98c0` LSI_LCG_F_COMBO_ZZZXZ_END_EventProcess `32af9b90` LSI_LCG_F_DASH_COMBO_Z_EventProcess `32af9e80` LSI_LCG_F_DASH_COMBO_ZX_EventProcess `32afa2d0` LSI_LCG_F_DASH_COMBO_XX_EventProcess `32afa720` LSI_LCG_F_DASH_JUMP_COMBO_ZXX_EventProcess `32afa750` LSI_LCG_M_DASH_JUMP_COMBO_ZXX_EventProcess `32afa780` LSI_LCG_F_DASH_JUMP_COMBO_XZZ_READY_EventProcess `32afa7f0` LSI_LCG_F_DASH_JUMP_COMBO_XZZ_LOOP_EventProcess `32afaac0` LSI_LCG_F_DASH_JUMP_COMBO_XZZ_END_EventProcess `32b105d0` LSI_LLC_F_DASH_JUMP_COMBO_Z_DASH_EventProcess `32b10980` LSI_LLC_F_DASH_JUMP_COMBO_Z_DASH_UP_EventProcess `32b10bd0` LSI_LLC_F_DASH_JUMP_COMBO_Z_DASH_DOWN_EventProcess `32b10e20` LSI_LLC_F_DASH_JUMP_COMBO_X_DASH_EventProcess `32b111d0` LSI_LLC_F_DASH_JUMP_COMBO_X_DASH_UP_EventProcess `32b11420` LSI_LLC_F_DASH_JUMP_COMBO_X_DASH_DOWN_EventProcess `32b11d80` LSI_LLC_M_DASH_JUMP_COMBO_Z_DASH_EventProcess `32b12130` LSI_LLC_M_DASH_JUMP_COMBO_Z_DASH_UP_EventProcess `32b12380` LSI_LLC_M_DASH_JUMP_COMBO_Z_DASH_DOWN_EventProcess `32b125d0` LSI_LLC_M_DASH_JUMP_COMBO_X_DASH_EventProcess `32b12980` LSI_LLC_M_DASH_JUMP_COMBO_X_DASH_UP_EventProcess `32b12bd0` LSI_LLC_M_DASH_JUMP_COMBO_X_DASH_DOWN_EventProcess `32b1cba0` LSI_LCG_COMBO_SNIPING_X_FIRE_StateStart `32b1cc60` LSI_LCG_F_COMBO_ZZZXX_END_StateStart `32b22240` LSI_LLC_COMBO_ZZZX_FrameMove `32b26030` LSI_LCG_COMBO_SNIPING_Z_FrameMove `32b266a0` LSI_LCG_COMBO_SNIPING_X_FIRE_FrameMove `32b26a40` LSI_LCG_DASH_JUMP_COMBO_SNIPING_LOOP_FrameMove `32b27b60` LSI_LLC_M_COMBO_ZZZX_FrameMove `32b28020` LSI_LCG_F_COMBO_ZZZXX_END_FrameMove `32b282c0` LSI_LCG_F_COMBO_ZZZXZ_LOOP_FrameMove `32b287b0` LSI_LCG_F_DASH_JUMP_COMBO_XZZ_LOOP_FrameMove `32b3a290` LSI_WIN_StateStart `32b3a2b0` LSI_LOSE_StateStart `32b3b530` LSI_LCG_COMBO_SNIPING_READY_EventProcess `32b3bc70` LSI_LCG_F_COMBO_ZZZX_WAIT_EventProcess.
- **States:** the ones whose callbacks are listed above (the state table's col 8 keeps the unsplit `CH1g`).
