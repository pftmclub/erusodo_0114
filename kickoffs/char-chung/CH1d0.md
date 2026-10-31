# June character Chung (Stage 7) - Phase CH1d0 - Helpers: engine callees the Sonnet phase CH1d needs (Opus)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1d0` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1c review. One at a time: `X2GUChung.*` / `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` (or the owning shared flag when the helper is `CX2GUUser` / engine code) |
| Log tag | none (inert until `_LIVE`; the phase that uses it play-tests it) |
| Review | `kickoffs/char-chung/CH1d0-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH0 - Chung map (read-only)" "CH0 - Done (2026-10-31)"
awk -F'\t' '$1 ~ /^(31fd4290|31fd4340)/' june2017_re/char_chung/ch0_unmapped_identified.tsv june2017_re/char_chung/ch0_unmapped_callees.tsv
```

## Task

CH0 found callees outside Chung's class range that no doc maps and that the Sonnet phase `CH1d` would stop on ("this needs Opus"). Map each in Ghidra, name it, give it a March home (an existing March function when June's namesake does the same; else a new one under the flag, by the owning class's pattern, no `June` in the name), and port its whole body (June's numbers and quirks verbatim, a `JUNE_BUGS.md` row each). A funcmap row per function (`Char Chung CH1d0`) so `CH1d` calls it by its March name. Check first whether Aisha's, Eve's or a Stage 2-3 port already holds it (`funcmap.tsv`, `junediverge.py --list`).

- **`31fd4290`** CX2GUUser pair with 31fd4340: clears +0x24c on the objects at +0x87d4 / +0x8e88 and on the +0x683c riding-pet optr; CH0 grade: unknown; callers 32bf1000 32bf75b0 32bf7cd0 32bfb6e0 32bfc310 32bfd2b0; evidence: decompiled only; sits after RidingSpecialMoveEnd 31fd4000; March RidingSpecialMoveEnd (X2GUUser.cpp:46457) does not match; to confirm: callers in funcmap + memb
- **`31fd4340`** CX2GUUser pair with 31fd4290 (offsets +0x87d0 / +0x8e84); CH0 grade: unknown; callers 32beba10; evidence: see 31fd4290

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only. Done gate: every address above has a funcmap row and a March body (or a recorded reason it needs none); the build passes; `CH1d`'s kickoff names the March symbols; `junediverge.py --check` and `enumscan.py --check` PASS.
