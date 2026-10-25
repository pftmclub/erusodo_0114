# June character Eve (Stage 7) - Phase CH4a - Passives

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH4a` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH2c review, and no CH3 phase running (same files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`EVE PASSIVE` / `EVE UI`) |
| Review | `kickoffs/char-eve/CH4a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH4 split (2026-10-22, Opus)"
```
Then the Eve lines of the checklist rows named below (`june2017_re/char_eve/plan_checklist_rows.md` gives the line numbers), the `CH4a` rows of `ch0_*.tsv`, and Aisha's matching CH4 Done note as the worked example.

## Task

Scope: `InitPassiveSkillState` and Eve's passive ids (types 1 / 5 / 8), their objects and readers; the engine hooks they need (Aisha's CH4a-CH4c already ported the unit delegate lists and `JUNE_USER_PASSIVE_HOOKS`: reuse them). June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows. You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through `bytepatch.py`. Mark rows `(Eve: CH4a)`. Done gate: each item's effect visible in game or in a named log line.

## Amended by CH0

(CH0 fills this: the tsv filter, the function count, June addresses, any split.)
