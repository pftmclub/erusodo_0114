# June character Chung (Stage 7) - Phase CH4b - Damage exception and overrides

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH4b` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH4a review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG PASSIVE`) |
| Review | `kickoffs/char-chung/CH4b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH4 split (2026-10-22, Opus)"
```
Then the Chung lines of the checklist rows named below (`june2017_re/char_chung/plan_checklist_rows.md` gives the line numbers), the `CH4b`
rows of `ch0_*.tsv`, and Eve's matching CH4 Done note as the worked example.

## Task

Scope: `SetDamageExceptionProcess` `32bf28f0` whole (June-only, 7b; passives on June reform ids, e.g. `RNSI_P_CIP_IRON_WILL` 26402); the other
overrides CH0 finds (`GetActualMPConsume` / `GetActualCoolTime` style); 7an soul gain when hit (user vslot 0x748, Chung `32beeca0`: add the base
if no class has it yet) and the specialization-B reader `32bf528d` (the reading function, whole); the death-prevention pre-apply override `32bed760`
(§7 l.255, user slot 250 `3201ffb0`); the per-skill stores of §7 l.280 / l.287 (`3201ad10` socket rate `( rate + 1.0 )`, `31fd7cc0` buff rate).

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through `bytepatch.py`.
Mark rows `(Chung: CH4b)`. Done gate: each item's effect visible in game or in a named log line.

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

## Amended by CH0 (2026-10-31)

- **Functions: 3** (`awk -F'\t' '$7=="CH4b"' june2017_re/char_chung/ch0_class_map.tsv`; inside 2, new 1).
  June addresses: `32be8e50` `32bed9d0` `32bf28f0`(new SetDamageExceptionProcess).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L255 death-prevention pre-apply: gauge snapshot override (vslot 0x740) Chun; §7 L280 3201ad10 skill-group power socket rate called from Chung code: 32bec28; §7 L287 engine readers keyed by a March skill id where June pushes the reform ; §7b L327 SetDamageExceptionProcess per-class block 32bf28f0; §7an L920 vslot 0x748 soul gain when hit: Chung 32beeca0; §7an L921 GetCharacterSpecializationB() readers: Chung 32bf528d (in 32bf5280).
