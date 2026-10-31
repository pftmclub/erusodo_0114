# June character Chung (Stage 7) - Phase CH4a - Passives

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH4a` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH2d review, and no CH3 phase running (same files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG PASSIVE`) |
| Review | `kickoffs/char-chung/CH4a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH4 split (2026-10-22, Opus)"
```
Then the Chung lines of the checklist rows named below (`june2017_re/char_chung/plan_checklist_rows.md` gives the line numbers), the `CH4a`
rows of `ch0_*.tsv`, and Eve's matching CH4 Done note as the worked example.

## Task

Scope: `InitPassiveSkillState` `32c16be0` and Chung's passive ids (types 1 / 8: 3 + 27 rows), their objects and readers; checklist 7am
delegate-list registrations (`32c16be0`, A `32c12910`, A `32bdba10`); the engine hooks they need (Aisha's and Eve's CH4 already ported the unit
delegate lists and the user passive hooks: reuse them). June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through `bytepatch.py`.
Mark rows `(Chung: CH4a)`. Done gate: each item's effect visible in game or in a named log line.

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

## Amended by CH0

(CH0 fills this: the filters with real column numbers, function counts, addresses, checklist rows, callees, any split.)
