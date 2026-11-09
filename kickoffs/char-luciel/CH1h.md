# June character Lu/Ciel (Stage 8) - Phase CH1h - Class core: Dread Lord and Noblesse job states

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH1h` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1g2; one at a time in `X2GULuciel.*`
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` (compiled, unreferenced) and `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | none of its own (user, 2026-11-10): the CH1i review covers CH1g, CH1g2, CH1h, CH1i; do not write `CH1h-review.md` (any debug define stays on until then) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Transcribe every function the class map gives phase `CH1h` (`awk -F'\t' '$6=="CH1h"' june2017_re/char_luciel/ch0_class_map.tsv`;
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

- **Functions: 47 to transcribe** (+ 47 thunks) (`awk -F'\t' '$6=="CH1h"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae5940` LSI_LLD_COMBO_XX_DOWN_XX_LOOP_StateStartFuture `32ae5df0` LSI_LCN_DASH_COMBO_ZX_DOWN_XX_LOOP_StateStartFuture `32ae60d0` LSI_LLD_M_COMBO_XX_DOWN_XX_LOOP_StateStartFuture `32ae60f0` LSI_LCN_F_DASH_COMBO_ZX_DOWN_XX_LOOP_StateStartFuture `32ae6110` LSI_LCN_F_DASH_COMBO_XX_DOWN_XX_LOOP_StateStartFuture `32af1180` LSI_LLD_COMBO_ZZZXZ_EventProcess `32af13b0` LSI_LLD_COMBO_XX_DOWN_X_EventProcess `32af16a0` LSI_LLD_COMBO_XX_DOWN_XX_READY_EventProcess `32af16e0` LSI_LLD_COMBO_XX_DOWN_XX_LOOP_EventProcess `32af1950` LSI_LLD_COMBO_XX_DOWN_XX_LOOP_X_EventProcess `32af6c50` LSI_LCN_DASH_COMBO_ZX_DOWN_X_StateStartFuture `32af6cc0` LSI_LCN_DASH_COMBO_ZX_DOWN_X_EventProcess `32af6fc0` LSI_LCN_DASH_COMBO_ZX_DOWN_XX_LOOP_EventProcess `32af72d0` LSI_LCN_DASH_COMBO_ZX_DOWN_XX_END_EventProcess `32af7500` LSI_LCN_SNIPING_COMBO_UP_X_EventProcess `32af7730` LSI_LCN_SNIPING_COMBO_DOWN_X_EventProcess `32afabc0` LSI_LLD_M_COMBO_ZZZXZ_EventProcess `32afae50` LSI_LLD_M_COMBO_XX_DOWN_X_EventProcess `32afb1c0` LSI_LLD_M_COMBO_XX_DOWN_XX_READY_EventProcess `32afb450` LSI_LLD_M_COMBO_XX_DOWN_XX_LOOP_EventProcess `32afb6c0` LSI_LLD_M_COMBO_XX_DOWN_XX_END_EventProcess `32afb950` LSI_LCN_F_COMBO_ZZZX_UP_X_EventProcess `32afbbe0` LSI_LCN_F_COMBO_ZZZX_DOWN_X_EventProcess `32afbe70` LSI_LCN_F_COMBO_XXXX_UP_X_EventProcess `32afc100` LSI_LCN_F_COMBO_XXXX_DOWN_X_EventProcess `32afc390` LSI_LCN_F_DASH_COMBO_ZXX_UP_X_EventProcess `32afc620` LSI_LCN_F_DASH_COMBO_ZXX_DOWN_X_EventProcess `32afc8b0` LSI_LCN_F_DASH_COMBO_XXX_UP_X_EventProcess `32afcb40` LSI_LCN_F_DASH_COMBO_XXX_DOWN_X_EventProcess `32afcdd0` LSI_LCN_F_DASH_COMBO_ZX_DOWN_X_StateStartFuture `32afce40` LSI_LCN_F_DASH_COMBO_ZX_DOWN_X_EventProcess `32afd140` LSI_LCN_F_DASH_COMBO_ZX_DOWN_XX_LOOP_EventProcess `32afd450` LSI_LCN_F_DASH_COMBO_ZX_DOWN_XX_END_EventProcess `32afd6e0` LSI_LCN_F_DASH_COMBO_XX_DOWN_X_StateStartFuture `32afd750` LSI_LCN_F_DASH_COMBO_XX_DOWN_X_EventProcess `32afda50` LSI_LCN_F_DASH_COMBO_XX_DOWN_XX_LOOP_EventProcess `32afdd60` LSI_LCN_F_DASH_COMBO_XX_DOWN_XX_END_EventProcess `32b15b20` LSI_LCN_DASH_COMBO_ZX_DOWN_XX_LOOP_FrameMove `32b15d40` LSI_LCN_F_DASH_COMBO_ZX_DOWN_XX_LOOP_FrameMove `32b15fe0` LSI_LCN_F_DASH_COMBO_XX_DOWN_XX_LOOP_FrameMove `32b270b0` LSI_LCN_DASH_COMBO_ZX_DOWN_X_FrameMove `32b272c0` LSI_LCN_SNIPING_COMBO_UP_X_FrameMove `32b27710` LSI_LCN_SNIPING_COMBO_DOWN_X_FrameMove `32b28ca0` LSI_LCN_F_COMBO_ZZZX_UP_X_FrameMove `32b29000` LSI_LCN_F_COMBO_ZZZX_DOWN_X_FrameMove `32b29360` LSI_LCN_F_DASH_COMBO_ZX_DOWN_X_FrameMove `32b29570` LSI_LCN_F_DASH_COMBO_XX_DOWN_X_FrameMove.
- **States: 39** (`awk -F'\t' '$8=="CH1h"' june2017_re/char_luciel/ch0_state_skill.tsv`): `LSI_LLD_START`, `LSI_LLD_WIN`, `LSI_LLD_WIN2`, `LSI_LLD_LOSE`, `LSI_LLD_LOSE2`, `LSI_LCN_START`, `LSI_LCN_WIN`, `LSI_LCN_WIN2`, `LSI_LCN_LOSE`, `LSI_LCN_LOSE2`, `LSI_LLD_COMBO_ZZZXZ`, `LSI_LLD_COMBO_XX_DOWN_X`, `LSI_LLD_COMBO_XX_DOWN_XX_READY`, `LSI_LLD_COMBO_XX_DOWN_XX_LOOP`, `LSI_LLD_COMBO_XX_DOWN_XX_LOOP_X`, `LSI_LCN_DASH_COMBO_ZX_DOWN_X`, `LSI_LCN_DASH_COMBO_ZX_DOWN_XX_LOOP`, `LSI_LCN_DASH_COMBO_ZX_DOWN_XX_END`, `LSI_LCN_SNIPING_COMBO_UP_X`, `LSI_LCN_SNIPING_COMBO_DOWN_X`, `LSI_LLD_M_COMBO_ZZZXZ`, `LSI_LLD_M_COMBO_XX_DOWN_X`, `LSI_LLD_M_COMBO_XX_DOWN_XX_READY`, `LSI_LLD_M_COMBO_XX_DOWN_XX_LOOP`, `LSI_LLD_M_COMBO_XX_DOWN_XX_END`, `LSI_LCN_F_COMBO_ZZZX_UP_X`, `LSI_LCN_F_COMBO_ZZZX_DOWN_X`, `LSI_LCN_F_DASH_COMBO_ZX_DOWN_X`, `LSI_LCN_F_DASH_COMBO_ZX_DOWN_XX_LOOP`, `LSI_LCN_F_DASH_COMBO_ZX_DOWN_XX_END`, `LSI_LCN_F_COMBO_XXXX_UP_X`, `LSI_LCN_F_COMBO_XXXX_DOWN_X`, `LSI_LCN_F_DASH_COMBO_ZXX_UP_X`, `LSI_LCN_F_DASH_COMBO_ZXX_DOWN_X`, `LSI_LCN_F_DASH_COMBO_XXX_UP_X`, `LSI_LCN_F_DASH_COMBO_XXX_DOWN_X`, `LSI_LCN_F_DASH_COMBO_XX_DOWN_X`, `LSI_LCN_F_DASH_COMBO_XX_DOWN_XX_LOOP`, `LSI_LCN_F_DASH_COMBO_XX_DOWN_XX_END`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L254 class code scales the cached state power rate (+0x7cc8), e.g. 32b1cd20; §7ac L778 Luciel's CanDown call shape in states @32b2fd79, @32b33fb7, @32b33d09.
