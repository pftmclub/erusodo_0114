# June character Chung (Stage 7) - Phase CH1k0 - Helpers: engine callees the Sonnet phase CH1k needs (Opus)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1k0` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1j review. One at a time: `X2GUChung.*` / `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` (or the owning shared flag when the helper is `CX2GUUser` / engine code) |
| Log tag | none (inert until `_LIVE`; the phase that uses it play-tests it) |
| Review | `kickoffs/char-chung/CH1k0-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH0 - Chung map (read-only)" "CH0 - Done (2026-10-31)"
awk -F'\t' '$1 ~ /^(31d28b30|3226e020|3226f870|31ff1950|32497d80|32a37130)/' june2017_re/char_chung/ch0_unmapped_identified.tsv june2017_re/char_chung/ch0_unmapped_callees.tsv
```

## Task

CH0 found callees outside Chung's class range that no doc maps and that the Sonnet phase `CH1k` would stop on ("this needs Opus"). Map each in Ghidra, name it, give it a March home (an existing March function when June's namesake does the same; else a new one under the flag, by the owning class's pattern, no `June` in the name), and port its whole body (June's numbers and quirks verbatim, a `JUNE_BUGS.md` row each). A funcmap row per function (`Char Chung CH1k0`) so `CH1k` calls it by its March name. Check first whether Aisha's, Eve's or a Stage 2-3 port already holds it (`funcmap.tsv`, `junediverge.py --list`).

- **`31d28b30`** CKTDGObject: true when this or an ancestor (+0xc chain) has +0x178 != manager +0x130; CH0 grade: candidate; callers 32bee680; evidence: used by 31fd56e0 (NotifyDGManagerChanged base, funcmap L4084); to confirm: March CKTDGObject member at +0x178
- **`3226e020`** CX2GameUnit flag setter: byte +0x416c = arg, zeroes +0x4170..+0x4188 when 0 (weapon render-pulse state); CH0 grade: candidate; callers 32bda7d0 32c0ca60; evidence: pairs with 3226f870 / 3226f900 which write the same +0x416c..+0x41a8 block (PulseWeaponRenderEffect); to confirm: March X2GameUnit.cpp:14268 area
- **`3226f870`** CX2GameUnit weapon render-pulse setter (stores into the +0x4170 slot of the PulseWeaponRenderEffect block); CH0 grade: candidate; callers 32c007e0; evidence: funcmap row 32275b00 (PulseWeaponRenderEffect via 3226f900); this sibling writes +0x4170 / +0x4178 instead of +0x4184; to confirm: March X2GameUnit.cpp:14268 ar
- **`31ff1950`** CX2GUUser show/hide object notify (GetShowObject chain via 31fae370, then 322845c0); CH0 grade: unknown; callers 32bdc700; evidence: decompiled only; near DetonationStateChange 31ff0eb0; to confirm: callers
- **`32497d80`** object ctor (vtable 3349b064 + protected-registry entry DAT_3376cdd0), copy-ctor sibling 32a37130; CH0 grade: unknown; callers 32c0feb0; evidence: decompiled only; two vtables at +0 and +0x110; to confirm: RTTI of 3349b064
- **`32a37130`** object copy ctor, sibling of 32497d80 (vtable 3349b064, protected entry copied from +0x114/+0x118); CH0 grade: unknown; callers 32c0feb0; evidence: decompiled only

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only. Done gate: every address above has a funcmap row and a March body (or a recorded reason it needs none); the build passes; `CH1k`'s kickoff names the March symbols; `junediverge.py --check` and `enumscan.py --check` PASS.
