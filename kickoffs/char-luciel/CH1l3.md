# June character Lu/Ciel (Stage 8) - Phase CH1l3 - Class core: Overrides, band 2, part 3

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH1l3` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1l2, CH1l30 review; one at a time in `X2GULuciel.*`
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` (compiled, unreferenced) and `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | kickoffs/char-luciel/CH1l3-review.md (Opus 5.5 high; the phase writes it when done). **It covers CH1l, CH1l2, CH1l3** (user, 2026-11-10): every covered Done note, `flagcheck --range` over all of them, `--check-all`, retires any debug define |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Transcribe every function the class map gives phase `CH1l3` (`awk -F'\t' '$6=="CH1l3"' june2017_re/char_luciel/ch0_class_map.tsv`;
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

- **Functions: 32 to transcribe** (+ 0 thunks) (`awk -F'\t' '$6=="CH1l3"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32aeb1a0` UpGageNowMp `32aebb60` `32b0e4e0` AccumulateShadowFuryDamage `32b0e550` `32b0eed0` UpdateStableDarkEnergyBuff `32b0ef80` GetOpenMindSkillID `32b14d10` UpdateOpenMindBuff `32b153d0` `32b15780` `32b16760` `32b16880` `32b16940` `32b176a0` `32b18060` `32b1eca0` `32b1f650` `32b1f800` CreateChaserOfDarknessDamageEffect `32b204d0` `32b20780` `32b347f0` `32b349f0` `32b34a90` `32b34c70` PlayPunisherEffectSet `32b35600` PlayAltarOfEvilEffectSet `32b35c60` `32b37090` PlayBluffEffectSet `32b4c7f0` `32b4cb40` `32b4cef0` `32b4cff0` `32b4d140` `32b4d260`.
  Still `?` (name it by the class pattern from the hint in col 7): `32aebb60` `32b0e550` `32b153d0` `32b15780` `32b16760` `32b16880` `32b16940` `32b176a0` `32b18060` `32b1eca0` `32b1f650` `32b204d0` `32b20780` `32b347f0` `32b349f0` `32b34a90` `32b35c60` `32b4c7f0` `32b4cb40` `32b4cef0` `32b4cff0` `32b4d140` `32b4d260`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L240 SetBuffAccumulationCount 322a7660 / AddBuffAccumulationCount 322a8a70 callers in; §7ai L835-841 pass June skip / adjust / level / creator at every ported BFI call.
- **Depends on CH1l30** (unit scans, effect aim).
