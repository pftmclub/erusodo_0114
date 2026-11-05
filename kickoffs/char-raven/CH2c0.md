# June character Raven (Stage 7) - Phase CH2c0 - Helpers: engine callees the Sonnet phase CH2c needs (Opus)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH2c0` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | the review of the phase before CH2c; one at a time with the other `X2GURaven.*` / `X2GUUser.*` phases |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` (or the owning shared flag when the helper is `CX2GUUser` / engine code) |
| Log tag | none (inert until `_LIVE`; the phase that uses it play-tests it) |
| Review | `kickoffs/char-raven/CH2c0-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "Phase CH0 - Raven map (read-only)" "CH0 - Done (2026-11-05)"
awk -F'\t' '$1 ~ /^(31fd9f40)/' june2017_re/char_raven/ch0_unmapped_identified.tsv june2017_re/char_raven/ch0_unmapped_callees.tsv
```

## Task

CH0 found callees outside Raven's class range that no doc maps and that the Sonnet phase `CH2c` would stop on ("this needs Opus"). Map each in Ghidra, name it, give it a March home (an existing March function when June's namesake does the same; else a new one under the flag, by the owning class's pattern, no `June` in the name), and port its whole body (June's numbers and quirks verbatim, a `JUNE_BUGS.md` row each). A funcmap row per function (`Char Raven CH2c0`) so `CH2c` calls it by its March name. Check first whether Aisha's, Eve's or a Stage 2-3 port already holds it (`funcmap.tsv`, `junediverge.py --list`).

- **`31fd9f40`** CX2GUUser setter: stores three floats at +0x8e14 / +0x8e18 / +0x8e1c, multiplies by +0x8e08, builds a vector (79 lines, SEH); CH0 grade: candidate; callers 32c38710 32c5ce40; evidence: decompile head only; callers 32c38710 and 32c5ce40 (CH2c); member names not in funcmap

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only. Done gate: every address above has a funcmap row and a March body (or a recorded reason it needs none); the build passes; `CH2c`'s kickoff names the March symbols; `junediverge.py --check` and `enumscan.py --check` PASS.
