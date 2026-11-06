# June character Raven (Stage 7) - Phase CH2c0 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH2c0 review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH2c0 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_USER_LEFT_HAND_SCALE` (no `_DEBUG`: nothing calls the setter until CH2c) |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH2c0 - Done (2026-11-06)"
grep -nE "^(31fd9f40|31fde6d0|31fded80)" june2017_re/funcmap.tsv
grep -nE "^CX2GUUser::(ScaleHand|RestoreHand)" tools/data/june_march_divergent.tsv
```
Then `X2Lib/X2GUUser.cpp` (`ScaleHand`, `RestoreHand`, `SetLeftHandScale`, the ctor init) and `X2GUUser.h` under the flag.

## Task

Check CH2c0 against June (Ghidra read-only; `june2017_re/disasm_range.py <a> <b> --x87` for the float loads). In order of risk:
- **`31fd9f40` SetLeftHandScale:** stores the argument at +0x8e14..+0x8e1c, multiplies by +0x8e08..+0x8e10 axis by axis, `GetXSkinAnimPtr` (32071d80) null test,
  `GetCloneFrame( Bip01_L_Hand )`, frame +0xdc word 0x101 = `m_bScale` / `m_bApplyChild` true (`MultiAnimFrame` layout in `KTDXDeviceXSkinMesh.h`), +0xe0..+0xe8 the product.
- **`31fde6d0` ScaleHand:** left hand = `m_vLeftHandScale * arg` (the `31c1c1a0` compare picks the left hand), right hand and the Eve dummies = arg; the
  Lu/Ciel finger-bone and second-body (+0x8e88) stand-ins are the only parts left out.
- **`31fded80` RestoreHand:** right hand and dummies `SetFrameScaleRestore` (word 0x100), left hand `SetFrameScale( m_vLeftHandScale, true )`; no
  `m_pXSkinAnim` null test in June or March; no `SetUnitReForm` (JUNE_BUGS 548: confirm caller `3202ed60` @3202f17b calls nothing after it).
- **Ctor** `3203e850` @320430bd-32043103: both vectors ( 1, 1, 1 ) (the `fld1` at @3204309f).
- **Other June callers** of ScaleHand (`3204b5e0` 1.5 / 1.55 / 2.55 cases, `3204e030` Lu/Ciel) stay with their owners: confirm the Done note routes them.
- `CH2c.md` names `CX2GUUser::SetLeftHandScale( D3DXVECTOR3 )` for `31fd9f40`.

Hygiene: `flagcheck.py SERV_IRUHADEV_JUNE_USER_LEFT_HAND_SCALE --range <CH2c0 commit>^..<CH2c0 commit>`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GUUser.cpp X2Lib/X2GUUser.h` (the 2 rows at `X2GUUser.h:2555-2556` are older than CH2c0). No build if the review changes no code.
Then check the CH2c kickoff.
