# June character Chung (Stage 7) - Phase CH1j0 - Helpers: engine callees the Sonnet phase CH1j needs (Opus)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1j0` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1i review. One at a time: `X2GUChung.*` / `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` (or the owning shared flag when the helper is `CX2GUUser` / engine code) |
| Log tag | none (inert until `_LIVE`; the phase that uses it play-tests it) |
| Review | `kickoffs/char-chung/CH1j0-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH0 - Chung map (read-only)" "CH0 - Done (2026-10-31)"
awk -F'\t' '$1 ~ /^(32008660|31fe4870|31fe9840)/' june2017_re/char_chung/ch0_unmapped_identified.tsv june2017_re/char_chung/ch0_unmapped_callees.tsv
```

## Task

CH0 found callees outside Chung's class range that no doc maps and that the Sonnet phase `CH1j` would stop on ("this needs Opus"). Map each in Ghidra, name it, give it a March home (an existing March function when June's namesake does the same; else a new one under the flag, by the owning class's pattern, no `June` in the name), and port its whole body (June's numbers and quirks verbatim, a `JUNE_BUGS.md` row each). A funcmap row per function (`Char Chung CH1j0`) so `CH1j` calls it by its March name. Check first whether Aisha's, Eve's or a Stage 2-3 port already holds it (`funcmap.tsv`, `junediverge.py --list`).

- **`32008660`** June CX2GUUser target-hit-position helper (JuneGetTargetHitPos sibling); CH0 grade: candidate; callers 32befec0 32bf00c0; evidence: funcmap row 31fd5ef0: Other callers 32008660, 32008a20 (not ported); x87 float maths with a vec3 out; to confirm: decompile body fully
- **`31fe4870`** wstring temp built from one arg then passed to 3207e390 and freed; CH0 grade: unknown; callers 32befec0 32bf00c0; evidence: decompiled only (31fe4870 -> 320770e0 + 3207e390); to confirm: callees 320770e0 / 3207e390
- **`31fe9840`** CX2GUUser debug/format helper (lookup 328adea0, 31c1c1a0 wstring ops, boost-style wstring buffer); CH0 grade: unknown; callers 32bf00c0; evidence: decompiled only; builds a wide-string message from a map lookup at +0x8a70; to confirm: callers (CH1j 32bf00c0)

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only. Done gate: every address above has a funcmap row and a March body (or a recorded reason it needs none); the build passes; `CH1j`'s kickoff names the March symbols; `junediverge.py --check` and `enumscan.py --check` PASS.
