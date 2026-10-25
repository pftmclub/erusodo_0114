# June character Eve (Stage 7) - Phase CH2b0 - Helper: the unit-cone collector (before CH2b)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH2b0` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH2a2 review. One at a time: `X2GuEve.*` / `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` |
| Log tag | none (inert until `_LIVE`; the phase that uses it play-tests it) |
| Review | `kickoffs/char-eve/CH2b0-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Phase CH0 - Eve map (read-only)" "CH0 - Done (2026-10-26)"
awk -F'	' '$1=="32009180"' june2017_re/char_eve/ch0_unmapped_identified.tsv
```

## Task

CH0 found callees outside Eve's class range that no doc had named (`june2017_re/char_eve/ch0_unmapped_identified.tsv`, kind / candidate name / evidence). The Sonnet phase `CH2b` would stop on them ("this needs Opus"). Map each one in Ghidra, name it, give it a March home under `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL`, and port its whole body (June's numbers and quirks verbatim, a `JUNE_BUGS.md` row each). Write a funcmap row per function (`Char Eve CH2b0`) so `CH2b` calls it by its March name.
- **`32009180`** (caller `329c4380`, an Architecture skill): collects units within a distance / height / facing-cone band, nearest first, into a vector sorted by `322acc40`. It is related to `CX2GameUnit::JuneCollectUnitsAround` `32294e30` (already ported?) and to Aisha's `322f0e60` view cone (pilot CH2c): reuse what fits.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only. Done gate: every address above has a funcmap row and a March body (or a recorded reason it needs none); the build passes; `CH2b`'s kickoff names the March symbols.
