# June character Chung (Stage 7) - Phase CH1l0 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1l0 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1l0 done (its Done note and commit) |
| Flag | none (CH1l0 changed no source) |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1l0 - Done (2026-11-01)"
grep -a "Char Chung CH1l0" june2017_re/funcmap.tsv
grep -a "Char Chung CH1l0" tools/data/june_march_divergent.tsv
```

## Task

CH1l0 changed no source. It recorded four verdicts. Check each one in Ghidra (read-only):
- **`31fcd740` is dead.** Is vslot 0x688 really never called? Re-run the byte scan: `call` / `jmp [reg+0x688]` (modrm 0x90-0x97, 0xa0-0xa7,
  SIB forms) and `mov reg, [reg+0x688]`, on `14JUN17\data\unpacked_x2.exe` below raw `0x1cbbe00`. Look for any other route as well: a
  pointer-to-member table, a Lua binding, a this-adjusting thunk. If a caller exists, `32bf44d0` and the base need a body: a CX2GUUser virtual
  under `_CORE` / `_LIVE` named after what it does, with Chung's override for CH1l, and the CH1l kickoff amended back.
- **`31fe21a0` = March `RemoveEquipFromViewList`:** erase order, weapon-index mapping 2 -> 0 / 0x25 -> 1, the `--i` re-test, and the smart-pointer release.
- **`31fd2f00` / `31ffe990` / `3201b930` route to Stage 8.** Is every June-only step Lu/Ciel's (unit type 10, form `+0xe40`, equip `+0x15d4`,
  `m_ItemIDForMinorUnit`, Ciel slot ids), and does a form-0 unit get March's result? The Done note lists the residue that does apply to Chung
  (unit type passed to CreateEquip, the 3-arg `GetEquippingItemByEquipPos`, `328825c0` / `32882540`, no `INIT_COMPONENT`). Is any of it
  visible to Chung in play, for example a different model for a class-locked item? If it changes what Chung shows, it is a playability case,
  and the replacement comes here (D1 shape, `SERV_IRUHADEV_JUNE_DIVERGE_*`) instead of Stage 8.
- **`32bdc750`:** is it March's body plus `slotId` only (the disassembly at `32bdc77e`-`32bdc819`, `RET 0xc`)?

Hygiene: `junediverge.py --check-all`. There is no flag to flagcheck, and no build unless the review changes source.
Write "CH1l0 - Review" into the plan doc, update the STATUS row, check `kickoffs/char-chung/CH1l.md` "Amended by CH1l0", and commit
with explicit pathspecs.
