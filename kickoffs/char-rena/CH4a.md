# June character Rena (Stage 7) - Phase CH4a - Passives

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH4a` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH2d review, and no CH3 phase running (same files). One at a time: `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RENA PASSIVE`) |
| Review | `kickoffs/char-rena/CH4a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH4a - Done" "CH4a - Review"
```
Then the Rena lines of the checklist rows named below (`june2017_re/char_rena/plan_checklist_rows.md` gives the line numbers), the
`CH4a` rows of `ch0_*.tsv`, and Raven's matching CH4 Done note as the worked example.

## Task

`InitPassiveSkillState` `32ca78b0` (vslot 0x690, uncarved: `charport_adis.py rena 32ca78b0`) and Rena's passive ids (types 1 / 8: 3 + 21 rows), their objects
and readers; **D8** (checklist l.330-331): March's Powerful Bowstring 3405 and Sharp Kick 3400 blocks replaced by June's 23405 / 23400 together with the
reform skills (never one without the other); the delegate registrations (l.912-925) replacing March's own pushes; every March `SI_*` passive id read as
June's `RNSI_*` (plan Rules, CH0's list). The engine hooks they need (earlier characters' CH4 ported the delegate lists, the user passive hooks and June's
base `InitPassiveSkillState` `31fe19d0`): reuse them. Check the engine readers of each passive (a user state key may hide there).

June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows. You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through
`bytepatch.py`; `cl /P` each `#ifdef` you edit. Mark rows `(Rena: CH4a)`. Done gate: each passive's effect visible in game or in a named log line
(CH0 names the stat or log line per passive).

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

(CH0 fills this: filters with real column numbers, addresses, counts, any split.)
