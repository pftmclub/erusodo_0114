# June character Elsword (Stage 7) - Phase CH4c - Rune Slayer magic reflect and memo readers

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH4c` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH4b and CH3d reviews. One at a time: `X2GUElsword_SwordMan.*`, `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE` (+ `_DEBUG`); an engine branch that is every class's takes its own `SERV_IRUHADEV_JUNE_DIVERGE_*` flag |
| Log tag | `[JUNE_CHAR]` (`ELSWORD PASSIVE`) |
| Review | `kickoffs/char-elsword/CH4c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH4c - Done (2026-11-03)"
```
Then the Elsword lines of the checklist rows named below (`june2017_re/char_elsword/plan_checklist_rows.md` gives the line numbers), the
`CH4c` rows of `ch0_*.tsv`, and Chung's matching CH4 Done note as the worked example.

## Task

Checklist 7j: the reflect branch in `320462a0` @`32046a55`-`32046aa0` (March's `X2GUUser.cpp` reflect roll; divergent row
`june_march_divergent.tsv:220`), ported whole at its definition, with the class's `32d0c820` (Magic Coating 21418 as a transcended hyper-class
skill: magic damage 0, no roll, MP gain and effect); March's `GetMagicReflectRate` `:5365` / `SI_P_ERS_MAGIC_RESISTANCE` `:2607-2636` /
`RT_REFLECT` `:646` against June. `MemoConditionElsword` (RTTI `3373d9b0`) and its memo readers; the S3d memo consumer lines that waited for his
skill states (`STATUS_ARCHIVE.md` l.715): confirm them in the play-test.

June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows. You are on Opus: delegate the reads, greps and known-address Ghidra dumps
(x2-phase, *When the phase is Opus*). CP949 files through `bytepatch.py`. Mark rows `(Elsword: CH4c)`. Done gate: each item's effect visible in
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
