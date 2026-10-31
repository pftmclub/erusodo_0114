# June character Chung (Stage 7) - Phase CH2b0 - Helpers: engine callees the Sonnet phase CH2b needs (Opus)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH2b0` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH2a review. One at a time: `X2GUChung.*` / `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` (or the owning shared flag when the helper is `CX2GUUser` / engine code) |
| Log tag | none (inert until `_LIVE`; the phase that uses it play-tests it) |
| Review | `kickoffs/char-chung/CH2b0-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH0 - Chung map (read-only)" "CH0 - Done (2026-10-31)"
awk -F'\t' '$1 ~ /^(31fcf430|329cc340|31fc8900)/' june2017_re/char_chung/ch0_unmapped_identified.tsv june2017_re/char_chung/ch0_unmapped_callees.tsv
```

## Task

CH0 found callees outside Chung's class range that no doc maps and that the Sonnet phase `CH2b` would stop on ("this needs Opus"). Map each in Ghidra, name it, give it a March home (an existing March function when June's namesake does the same; else a new one under the flag, by the owning class's pattern, no `June` in the name), and port its whole body (June's numbers and quirks verbatim, a `JUNE_BUGS.md` row each). A funcmap row per function (`Char Chung CH2b0`) so `CH2b` calls it by its March name. Check first whether Aisha's, Eve's or a Stage 2-3 port already holds it (`funcmap.tsv`, `junediverge.py --list`).

- **`31fcf430`** skill-memo lookup for the master ultimate flag (none in March); CH0 grade: confirmed; callers 32be1760; evidence: funcmap rows 3236a040 / 31fcf520 / 3230c1a0 (31fcf430( 26019 ) etc.); body: unit [this+0x867c] null -> returns arg else 328905b0
- **`329cc340`** CX2GUUser predicate: vslot 0x524 id -> 31fca8e0 lookup, true when result +0x178 != 0; CH0 grade: candidate; callers 32be1760; evidence: funcmap row 3229b0a0 (only caller Rose 32ababc0); next row 329cc370 IsRidingKeySkillWaiting; to confirm: vslot 0x524
- **`31fc8900`** switch on an index (0..9) returning a byte at fixed offsets of a struct (+0x1d,+0x21,+0x25,+0x19,+0x51...); CH0 grade: unknown; callers 32be1760 32be2460; evidence: decompiled only; per-type flag selector on a June struct; callers CH2b 32be1760 / 32be2460; to confirm: struct type via those callers

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only. Done gate: every address above has a funcmap row and a March body (or a recorded reason it needs none); the build passes; `CH2b`'s kickoff names the March symbols; `junediverge.py --check` and `enumscan.py --check` PASS.
