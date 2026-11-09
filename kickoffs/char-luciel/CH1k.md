# June character Lu/Ciel (Stage 8) - Phase CH1k - Class core: Overrides, band 1, part 1

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH1k` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1i review, CH1k0 review; one at a time in `X2GULuciel.*`
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` (compiled, unreferenced) and `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | none of its own (user, 2026-11-10): the CH1k2 review covers CH1k, CH1k2; do not write `CH1k-review.md` (any debug define stays on until then) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Transcribe every function the class map gives phase `CH1k` (`awk -F'\t' '$6=="CH1k"' june2017_re/char_luciel/ch0_class_map.tsv`;
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

- **Functions: 31 to transcribe** (+ 0 thunks) (`awk -F'\t' '$6=="CH1k"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae4080` DamageReact `32ae4f30` InitEffect `32ae50f0` SetGageNowMp `32ae5480` GetReduceDebuffDurationRate `32aea530` SetDamageExceptionProcess `32aea610` Win `32aea650` Draw `32aeb090` `32aeb0c0` GetNowMp `32aeb0f0` GetMaxMp `32aeb120` SetMaxMp `32aeb160` UpNowMp `32aeb240` SetNowMp `32aeb280` FlushMp `32aeb450` GetStateIDByForm `32b0e3f0` `32b0e620` SetGageNowMpOfForm `32b0efc0` StampEffectSetIndex `32b0efe0` GetDamageEffectIndex `32b0eff0` Lose `32b0f080` InitComponent `32b14cd0` ChangeMaxCharacterAddResource `32b151b0` `32b16260` `32b162e0` LoadTransformedSkinAnim `32b166e0` `32b16810` EraseDamageEffectIndexEntry `32b17660` `32b17680` `32b1e720` `32b1e8e0` Init.
  Still `?` (name it by the class pattern from the hint in col 7): `32aeb090` `32b0e3f0` `32b151b0` `32b16260` `32b166e0` `32b17660` `32b17680` `32b1e720`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §5 L184-188 keys DEFAULT_UNSELECTED_UNIT_MP_CHARGE_VALUE_PER_SECOND and _AT_PVP (INIT_COMPON; §7 L238 32b1e8e0 (vslot 16, user slot 0x40) writes 0 at init; §7 L255 ChangeMaxCharacterAddResource override 32b14cd0 (unit vslot 201, BBT 97); §7a L308 vslot 0x248 override 32b0efc0 (instance +0x1b8 = unit +0x988c), vslot 0x2e0 over.
- Overrides below vslot 0x49c (except the state-id getters, CH1l), names in col 4 with the base member in col 5. **Depends on CH1k0** (buff Time-finalizer passes).
- 14 overrides stay `?` with a hint in the note (`0x2b0`, `0x2e4` = `0x2fc` one body, `0x300`, `0x318`, `0x580`, `0x62c`, `0x640`, `0x648`, `0x64c`, `0x658`, `0x6b0`, `0x848`, `0x87c`): name each from its base and body by March's pattern before transcribing.
- **Amended by Raven CH1o (2026-11-06):** vslot `0x300` is March's `AdjustDamageDataBeforeDamageReact( DamageData* )` (`X2GUUser.h`, under
  `ADD_RENA_SYSTEM`, off): name Luciel's `32b1e720` so. Raven CH1o declared the base and added the user-attacker calls in `X2DamageManager.cpp`
  (UnitToUnit / EffectToUnit / MeshToUnit, June @3262546d / @3262f1e6 / @32628652) under `#ifdef SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` + `#ifndef ADD_RENA_SYSTEM`:
  widen those four guards with Luciel's `_LIVE`. June calls the slot on every attacker: `CX2GUNPC`'s override `32778fe0` hands it to the owner of a
  `CX2AllyNPCAI` NPC (funcmap row). If a Luciel summon deals damage, port that override and drop the user-only test; Raven needed neither.
