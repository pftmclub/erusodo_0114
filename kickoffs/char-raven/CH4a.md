# June character Raven (Stage 7) - Phase CH4a - Passives

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH4a` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH2d review, and no CH3 phase running (same files). One at a time: `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RAVEN PASSIVE`) |
| Review | `kickoffs/char-raven/CH4a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH4a - Done" "CH4a - Review"
```
Then the Raven lines of the checklist rows named below (`june2017_re/char_raven/plan_checklist_rows.md` gives the line numbers), the
`CH4a` rows of `ch0_*.tsv`, and Elsword's matching CH4 Done note as the worked example.

## Task

`InitPassiveSkillState` `32c57660` (vslot 0x690) and Raven's passive ids (types 1 / 8: 10 + 27 rows), their objects and readers; checklist
7b Burning Nasod Hand whole (the 24401 setter `+0x9688` here, the `+0x4108 = 2` write `32c43120 @32c43ac4` if CH1 did not take it: never one
without the other and June's `AT_COMMAND` tags; MODS.md l.2557-2559); 7am's 3 delegate callbacks (`32c3df50`, `32c4e5b0`, `32c28160`) replacing
March's own pushes; 7an vslot 0x73c `32c3c7f0` and 0x748 `32c3c820` (soul gain when hit; the base and its `DamageReact` call are Chung CH4b's),
both uncarved (`charport_adis.py raven`), unless CH1i took them; every March `SI_*` passive id read as June's `RNSI_*` (plan Rules). The engine
hooks they need (Aisha's / Eve's / Chung's / Elsword's CH4 ported the delegate lists, the user passive hooks and June's base
`InitPassiveSkillState` `31fe19d0`): reuse them. Check the engine readers of each passive (Elsword's CH4a2 found a user state key).

June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows. You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through
`bytepatch.py`; `cl /P` each `#ifdef` you edit. Mark rows `(Raven: CH4a)`. Done gate: each item's effect visible in
game or in a named log line.

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

(CH0 fills this: filters with real column numbers, function counts, addresses, any split.)
