# June character Chung (Stage 7) - Phase CH1f review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1f review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1f done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (commented out until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1f - Done (2026-10-31)"
awk -F'	' '$7=="CH1f"' june2017_re/char_chung/ch0_class_map.tsv
```

## Task

CH1f is 27 functions (25 `inside`, 2 `new`), 21 of them claimed `= March`. Read them against June (`python june2017_re/charport_adis.py chung --phase CH1f --out <scratch dir>`; most of these are thunks or tiny stubs Ghidra's decompiler has no function for). Where June is not March:
- `CIPComboPowerDownLandingZEventProcess` `32be42c0`: the any-direction walk cancel (to `USI_WAIT`) and the dash cancel after 0.6 also need ground; skill / hyper cancel, the 0.2-0.6 `oneZ` window (flag [0]) and the flag -> `CSI_CIP_COMBO_POWER_DOWN_LANDING_ZZ` (0x100) as March; anim end sits inside the else.
- `CIPComboDashXXXXEventProcess` `32be48e0` (1.1333) and `CIPComboDashXXXZEventProcess` `32be4b40` (0.633): skill / hyper, then the walk (`USI_WAIT`) and dash cancels need ground, then the arrow / Z / X key test -> `USI_WALK` (no ground test).
- `CIPComboDashJumpXXEventProcess` `32be9930` (new): `bFootOnLine` alone -> state 0x146 `CSI_CIP_COMBO_DASHJUMP_XX_LANDING`; `CIPComboDashJumpXXLandingEventProcess` `32be9960` (new): the five cancel members `+0x8c90` / `94` / `98` / `9c` / `8c` as CH1e's `CFGComboXXFrontZZEventProcess`, each ground-gated; new enumerator `CSI_CIP_COMBO_DASHJUMP_XX_LANDING` (script id 326) at the end of the `_CORE` block (`X2GUChung.h`).
- Claimed `= March`: spot-check three line by line, including every float: `32be4500` (ZZ: dash, walk, any key; only the foot-on-line branch), `32be4810` (DashXXX 0.6 / 0.87 windows, flags [0] / [1], states 0x103 / 0x104), `32c02530` (0.35 horizontal, 0.51 vertical -220 / 220) or `32c02920` (0.335, `CanUseCannon`, no Open Mind clause).
- The Init / End / FrameMove stubs (`32bda3a0`-`32bda4c0`, 15 functions) are one-instruction thunks: confirm in `charport_adis.py` that none does more than `ret` / `CommonStateEnd` (vslot `0x7dc`) / `CommonFrameMove` (vslot `0x7cc`) / the five preloads of `32bda3c0`.
- Checklist §7 L253 (`+0x7cc8`): the Done note says none of the 27 touches it (`grep 7cc8` over the listings is empty).

Hygiene: `GIT_CONFIG_COUNT=1 GIT_CONFIG_KEY_0=diff.algorithm GIT_CONFIG_VALUE_0=patience python tools/flagcheck.py JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE --range <CH1f commit>^..<CH1f commit>` (FAIL only because the `#define` is commented out), the `_CORE` run, `junediverge.py --check-all`, `enumscan.py --check`, both one-file probes (`-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` on and off).
Write "CH1f - Review" into the plan doc, update the STATUS row, check `kickoffs/char-chung/CH1g.md`, commit with explicit pathspecs.
