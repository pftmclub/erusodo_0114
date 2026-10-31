# June character Chung (Stage 7) - Phase CH1l0 - Helpers: engine callees the Sonnet phase CH1l needs (Opus)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1l0` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1k review. One at a time: `X2GUChung.*` / `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` (or the owning shared flag when the helper is `CX2GUUser` / engine code) |
| Log tag | none (inert until `_LIVE`; the phase that uses it play-tests it) |
| Review | `kickoffs/char-chung/CH1l0-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH0 - Chung map (read-only)" "CH0 - Done (2026-10-31)"
awk -F'\t' '$1 ~ /^(31fcd740|31fd2f00|31fe21a0|31ffe990)/' june2017_re/char_chung/ch0_unmapped_identified.tsv june2017_re/char_chung/ch0_unmapped_callees.tsv
```

## Task

CH0 found callees outside Chung's class range that no doc maps and that the Sonnet phase `CH1l` would stop on ("this needs Opus"). Map each in Ghidra, name it, give it a March home (an existing March function when June's namesake does the same; else a new one under the flag, by the owning class's pattern, no `June` in the name), and port its whole body (June's numbers and quirks verbatim, a `JUNE_BUGS.md` row each). A funcmap row per function (`Char Chung CH1l0`) so `CH1l` calls it by its March name. Check first whether Aisha's, Eve's or a Stage 2-3 port already holds it (`funcmap.tsv`, `junediverge.py --list`).

- **`31fcd740`** CX2GUUser per-frame bar fix: vslot 0xdc -> vslot 0xf8, then count test via vslots 0xac / 0xa8 on protected +0x8154; CH0 grade: unknown; callers 32bf44d0; evidence: decompiled only; to confirm: vslot rows 0xdc 0xf8 0xac 0xa8
- **`31fd2f00`** CX2GUUser equip-list scan on +0x8834 comparing +0x1b0 with a byte at ([arg]+0xc0)+0xd5; CH0 grade: unknown; callers 32bdc750; evidence: decompiled only; sits after GetNowMPChange; to confirm: callers 32bdc750
- **`31fe21a0`** CX2GUUser erase of an equip from +0x8834 and rescan of +0x3cfc list for +0x19c match when type is 2 or 0x25; CH0 grade: unknown; callers 32bdc750; evidence: decompiled only; to confirm: callers 32bdc750
- **`31ffe990`** CX2GUUser effect-attach loop over an NPC effect list (32882540); CH0 grade: unknown; callers 32bdc750; evidence: decompiled only; large body with 0x1b0 locals; to confirm: callers

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only. Done gate: every address above has a funcmap row and a March body (or a recorded reason it needs none); the build passes; `CH1l`'s kickoff names the March symbols; `junediverge.py --check` and `enumscan.py --check` PASS.
