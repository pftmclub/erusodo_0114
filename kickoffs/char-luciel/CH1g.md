# June character Lu/Ciel (Stage 8) - Phase CH1g - Class core: Chiliarch and Royal Guard job states, part 1

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH1g` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1f2 review; one at a time in `X2GULuciel.*`
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` (compiled, unreferenced) and `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | none of its own (user, 2026-11-10): the CH1i review covers CH1g, CH1g2, CH1h, CH1i; do not write `CH1g-review.md` (any debug define stays on until then) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Transcribe every function the class map gives phase `CH1g` (`awk -F'\t' '$6=="CH1g"' june2017_re/char_luciel/ch0_class_map.tsv`;
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

- **Functions: 38 to transcribe** (+ 38 thunks) (`awk -F'\t' '$6=="CH1g"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae5c70` LSI_LCG_COMBO_ZZZX_StateStartFuture `32ae5c90` LSI_LCG_COMBO_SNIPING_Z_StateStartFuture `32ae5cb0` LSI_LCG_COMBO_SNIPING_X_CHARGE_StateStart `32ae5ce0` LSI_LCG_COMBO_SNIPING_X_CHARGE_EventProcess `32ae5d20` LSI_LCG_COMBO_SNIPING_X_CHARGE_FrameMove `32ae5db0` LSI_LCG_DASH_JUMP_COMBO_SNIPING_START_StateStartFuture `32ae5dd0` LSI_LCG_DASH_JUMP_COMBO_SNIPING_LOOP_StateStartFuture `32ae5e10` LSI_LCG_F_COMBO_ZZZX_StateStartFuture `32ae5e30` LSI_LCG_F_COMBO_ZZZXX_LOOP_StateStartFuture `32ae5e60` LSI_LCG_F_COMBO_ZZZXX_LOOP_FrameMove `32ae5ef0` LSI_LCG_F_COMBO_ZZZXX_LOOP_EventProcess `32ae5f40` LSI_LCG_F_COMBO_ZZZXZ_LOOP_StateStart `32ae5f60` LSI_LCG_F_COMBO_XXXXX_LOOP_EventProcess `32ae5fb0` LSI_LCG_F_DASH_COMBO_ZXXX_LOOP_EventProcess `32ae6000` LSI_LCG_F_DASH_COMBO_XXXX_LOOP_EventProcess `32ae6090` LSI_LCG_F_DASH_JUMP_COMBO_XZZ_READY_StateStartFuture `32ae60b0` LSI_LCG_F_DASH_JUMP_COMBO_XZZ_LOOP_StateStartFuture `32af0270` LSI_LLC_COMBO_ZZZX_EventProcess `32af0570` LSI_LLC_DASH_COMBO_Z_EventProcess `32af0860` LSI_LLC_DASH_COMBO_ZX_EventProcess `32af0b50` LSI_LLC_DASH_COMBO_ZXX_EventProcess `32af0e50` LSI_LLC_DASH_COMBO_XX_EventProcess `32af5d20` LSI_LCG_DASH_COMBO_ZX_EventProcess `32af6110` LSI_LCG_COMBO_ZZZX_EventProcess `32af6330` LSI_LCG_COMBO_SNIPING_Z_EventProcess `32af6560` LSI_LCG_COMBO_SNIPING_Z_FINISH_EventProcess `32af6780` LSI_LCG_COMBO_SNIPING_X_FIRE_EventProcess `32af69b0` LSI_LCG_DASH_JUMP_COMBO_SNIPING_START_EventProcess `32af6a20` LSI_LCG_DASH_JUMP_COMBO_SNIPING_LOOP_EventProcess `32af6c20` LSI_LCG_DASH_JUMP_COMBO_SNIPING_FINISH_EventProcess `32af7960` LSI_LLC_F_DASH_COMBO_ZXXX_EventProcess `32af7c10` LSI_LLC_F_DASH_COMBO_XXX_EventProcess `32af7ec0` LSI_LLC_M_COMBO_ZZZX_EventProcess `32af8230` LSI_LLC_M_DASH_COMBO_ZX_EventProcess `32af85a0` LSI_LLC_M_DASH_COMBO_ZXX_EventProcess `32af8910` LSI_LLC_M_DASH_COMBO_ZXXX_EventProcess `32af8bc0` LSI_LLC_M_DASH_COMBO_XX_EventProcess `32af8f30` LSI_LLC_M_DASH_COMBO_XXX_EventProcess.
- **States: 76** (`awk -F'\t' '$8=="CH1g"' june2017_re/char_luciel/ch0_state_skill.tsv`): `LSI_LLC_START`, `LSI_LLC_WIN`, `LSI_LLC_WIN2`, `LSI_LLC_LOSE`, `LSI_LLC_LOSE2`, `LSI_LCG_START`, `LSI_LCG_WIN`, `LSI_LCG_WIN2`, `LSI_LCG_LOSE`, `LSI_LCG_LOSE2`, `LSI_LLC_COMBO_ZZZX`, `LSI_LLC_DASH_COMBO_Z`, `LSI_LLC_DASH_COMBO_ZX`, `LSI_LLC_DASH_COMBO_ZXX`, `LSI_LLC_DASH_COMBO_XX`, `LSI_LLC_F_DASH_JUMP_COMBO_Z_DASH`, `LSI_LLC_F_DASH_JUMP_COMBO_Z_DASH_UP`, `LSI_LLC_F_DASH_JUMP_COMBO_Z_DASH_DOWN`, `LSI_LLC_F_DASH_JUMP_COMBO_X_DASH`, `LSI_LLC_F_DASH_JUMP_COMBO_X_DASH_UP`, `LSI_LLC_F_DASH_JUMP_COMBO_X_DASH_DOWN`, `LSI_LLC_M_DASH_JUMP_COMBO_Z_DASH`, `LSI_LLC_M_DASH_JUMP_COMBO_Z_DASH_UP`, `LSI_LLC_M_DASH_JUMP_COMBO_Z_DASH_DOWN`, `LSI_LLC_M_DASH_JUMP_COMBO_X_DASH`, `LSI_LLC_M_DASH_JUMP_COMBO_X_DASH_UP`, `LSI_LLC_M_DASH_JUMP_COMBO_X_DASH_DOWN`, `LSI_LCG_COMBO_ZZZX`, `LSI_LCG_COMBO_XXXX`, `LSI_LCG_DASH_COMBO_ZX`, `LSI_LCG_DASH_COMBO_ZXX`, `LSI_LCG_COMBO_SNIPING_READY`, `LSI_LCG_COMBO_SNIPING_Z`, `LSI_LCG_COMBO_SNIPING_Z_FINISH`, `LSI_LCG_COMBO_SNIPING_X_CHARGE`, `LSI_LCG_COMBO_SNIPING_X_FIRE`, `LSI_LCG_DASH_JUMP_COMBO_SNIPING_START`, `LSI_LCG_DASH_JUMP_COMBO_SNIPING_LOOP`, `LSI_LCG_DASH_JUMP_COMBO_SNIPING_FINISH`, `LSI_LLC_F_DASH_COMBO_ZXXX`, `LSI_LLC_M_COMBO_ZZZX`, `LSI_LLC_M_DASH_COMBO_ZX`, `LSI_LLC_M_DASH_COMBO_ZXX`, `LSI_LLC_M_DASH_COMBO_ZXXX`, `LSI_LLC_M_DASH_COMBO_XX`, `LSI_LCG_F_COMBO_ZZZX`, `LSI_LCG_F_COMBO_ZZZX_WAIT`, `LSI_LCG_F_COMBO_ZZZXZ_LOOP`, `LSI_LCG_F_COMBO_ZZZXZ_END`, `LSI_LCG_F_COMBO_ZZZXX_LOOP`, `LSI_LCG_F_COMBO_ZZZXX_END`, `LSI_LCG_F_DASH_COMBO_ZX`, `LSI_LCG_F_DASH_JUMP_COMBO_XZZ_READY`, `LSI_LCG_F_DASH_JUMP_COMBO_XZZ_LOOP`, `LSI_LCG_F_DASH_JUMP_COMBO_XZZ_END`, `LSI_LCG_F_DASH_JUMP_COMBO_ZXX`, `LSI_LCG_M_DASH_JUMP_COMBO_ZXX`, `LSI_LLC_F_DASH_COMBO_XXX`, `LSI_LLC_M_DASH_COMBO_XXX`, `LSI_LCG_F_COMBO_XXXX`, `LSI_LCG_F_COMBO_XXXXX_LOOP`, `LSI_LCG_F_COMBO_XXXXX_END`, `LSI_LCG_F_COMBO_XXXXZ_LOOP`, `LSI_LCG_F_COMBO_XXXXZ_END`, `LSI_LCG_F_DASH_COMBO_Z`, `LSI_LCG_F_DASH_COMBO_ZXX`, `LSI_LCG_F_DASH_COMBO_ZXXZ_LOOP`, `LSI_LCG_F_DASH_COMBO_ZXXZ_END`, `LSI_LCG_F_DASH_COMBO_ZXXX_LOOP`, `LSI_LCG_F_DASH_COMBO_ZXXX_END`, `LSI_LCG_F_DASH_COMBO_XX`, `LSI_LCG_F_DASH_COMBO_XXX`, `LSI_LCG_F_DASH_COMBO_XXXZ_END`, `LSI_LCG_F_DASH_COMBO_XXXX_LOOP`, `LSI_LCG_F_DASH_COMBO_XXXX_END`, `LSI_LCG_F_DASH_COMBO_XXXZ_LOOP`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L254 class code scales the cached state power rate (+0x7cc8), e.g. 32b1cd20.
