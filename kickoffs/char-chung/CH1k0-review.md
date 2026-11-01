# June character Chung (Stage 7) - Phase CH1k0 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1k0 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1k0 done (its Done note and commit) |
| Flag | none (CH1k0 changed no source) |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1k0 - Done (2026-11-01)"
grep -a "Char Chung CH1k0" june2017_re/funcmap.tsv
```

## Task

CH1k0 found that the six June callees need no port: each is a March function that already does what June's does. Check that verdict in Ghidra (read-only):
- **`31ff1950` -> `CX2GUUser::NotifyShowObjectChanged`:** is every June step March's, in March's order (shadow `+0xcb8`, vslot 0x394 = `32000490`,
  head marker `+0x87f8`, title emblem `+0x87fc`, hold centre / circle `+0x8818` / `+0x881c` on the game major particle)? Are `+0x8808` / `+0x8810`
  really created only by June code March lacks (`31fd54b0`, `31fcfde0`; `june2017_re/dispsweep.py 8808 8810`)? Is `322720f0( 1, 1 )` the second
  after-image? If any June step changes March's members, that is a `june_march_divergent.tsv` row plus a replacement under
  `SERV_IRUHADEV_JUNE_DIVERGE_*` (D1 shape).
- **`3226e020` / `3226f870`:** the store-to-member map against March's order (`X2GameUnit.h` `m_bEnableWeaponRenderEffect` .. colours) and the
  `<=` gate against March's `NEW_HENIR_TEST` early return, including NaN.
- **`31d28b30`:** `+0x178` = `m_hKTDGManagerHandle`, manager `+0x130` = `GetRenderObjectChainEnd()`, `+0xc` = `m_pManagerObject`.
- **`32497d80` / `32a37130`:** the four protected ints and three bools against `X2GageData.h:617-707`.

Hygiene: `junediverge.py --check-all`; there is no flag to flagcheck and no build unless the review changes source.
Write "CH1k0 - Review" into the plan doc, update the STATUS row, check `kickoffs/char-chung/CH1k.md` "Amended by CH1k0", and commit
with explicit pathspecs.
