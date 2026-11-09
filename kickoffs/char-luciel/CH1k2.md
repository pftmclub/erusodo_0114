# June character Lu/Ciel (Stage 8) - Phase CH1k2 - Class core: Overrides, band 1, part 2

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH1k2` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1k; one at a time in `X2GULuciel.*`
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` (compiled, unreferenced) and `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | kickoffs/char-luciel/CH1k2-review.md (Opus 5.5 high; the phase writes it when done). **It covers CH1k, CH1k2** (user, 2026-11-10): every covered Done note, `flagcheck --range` over all of them, `--check-all`, retires any debug define |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Transcribe every function the class map gives phase `CH1k2` (`awk -F'\t' '$6=="CH1k2"' june2017_re/char_luciel/ch0_class_map.tsv`;
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

- **Functions: 30 to transcribe** (+ 0 thunks) (`awk -F'\t' '$6=="CH1k2"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32b1f0a0` DeleteMinorParticle `32b1f0b0` HyperModeBuffEffectStart `32b373e0` ReInit `32b37c70` CreateGageData `32b4b580` `32b4cfb0` `32b4d3c0` GetWaitStateID `32b4d3f0` GetWalkStateID `32b4d430` GetDashStateID `32b4d470` GetDashEndStateID `32b4d4b0` GetDashJumpStateID `32b4d4f0` GetDashJumpLandingStateID `32b4d530` GetJumpReadyStateID `32b4d570` GetJumpUpStateID `32b4d610` GetJumpLandingStateID `32b4d650` GetDieFrontStateID `32b4d690` GetDieBackStateID `32b4d6d0` GetDamageGroggyStateID `32b4d710` GetDamageSmallFrontStateID `32b4d750` GetDamageSmallBackStateID `32b4d790` GetDamageBigFrontStateID `32b4d7d0` GetDamageBigBackStateID `32b4d890` GetDamageStandUpFrontStateID `32b4d8d0` GetDamageStandUpBackStateID `32b4d910` GetDamageAirSmallStateID `32b4d950` GetDamageAirDownStateID `32b4da50` GetDamageAirUpStateID `32b4db90` StateChange `32b4dbb0` StateChange `32b4dd50`.
  Still `?` (name it by the class pattern from the hint in col 7): `32b4b580` `32b4cfb0` `32b4dd50`.
- The state-id getters from 0x438 up are here and in CH1l: base returns a June state id, Lu/Ciel return the Lu id or Ciel's (+0x2d) while `IsTransformedForm( 2 )`.
