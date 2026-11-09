# June character Lu/Ciel (Stage 8) - Phase CH1l2 - Class core: Overrides, band 2, part 2

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH1l2` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1l; one at a time in `X2GULuciel.*`
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` (compiled, unreferenced) and `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | none of its own (user, 2026-11-10): the CH1l3 review covers CH1l, CH1l2, CH1l3; do not write `CH1l2-review.md` (any debug define stays on until then) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Transcribe every function the class map gives phase `CH1l2` (`awk -F'\t' '$6=="CH1l2"' june2017_re/char_luciel/ch0_class_map.tsv`;
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

- **Functions: 32 to transcribe** (+ 0 thunks) (`awk -F'\t' '$6=="CH1l2"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae40c0` SetEquippedSkillLevel `32ae4f60` CommonStateStartFuture `32ae4f70` CommonStateEndFuture `32ae4f80` CommonStateEnd `32ae4f90` CommonRender_Prepare `32ae4fb0` SpecialAttackEventProcess `32ae5400` StateChangeDashIfPossible `32ae9bd0` `32ae9d00` GetEffectOffsetByIndex `32aea8c0` ShownSkillCoolTime `32aeb380` IsInJobLine `32aeb410` StoreGageSnapshot `32aeb430` RestoreGageSnapshot `32b0e440` `32b0e670` SumSocketOptionAfterSlots `32b0f940` SpecialAttackUse `32b0fa90` ResetMaxMP `32b14640` GetActualMPConsume `32b15050` GetActualCoolTime `32b15220` IsNotCommonMoveState `32b15430` AfterImageFrameMove `32b16c80` ShowActiveSkillCutInAndLightByScript `32b171c0` ScaleHead `32b172b0` `32b18170` `32b1c880` SpecialAttackNoStageChange `32b1edd0` CommonFrameMoveFuture `32b1eef0` SpecialAttackCooldown `32b37910` CommonStateStart `32b37c40` NoStateChangeActionFrameMove `32b397a0` CommonFrameMove `32b3a310` CanUseSpecialAttack.
  Still `?` (name it by the class pattern from the hint in col 7): `32ae9bd0` `32b0e440` `32b172b0` `32b18170`.
