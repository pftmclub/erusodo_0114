# June character Chung (Stage 7) - Phase CH1e review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1e review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1e done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (commented out until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1e - Done (2026-10-31)"
awk -F'	' '$7=="CH1e"' june2017_re/char_chung/ch0_class_map.tsv
```

## Task

CH1e is 12 functions (10 `inside`, 2 `new`). Read them against June (`python june2017_re/charport_adis.py chung --phase CH1e --out <scratch dir>`). Where June is not March:
- `CFGComboDashJumpZXEventProcess` `32bdfe00`: flags [0] / [1] with `m_fEventTime[0]` (`+0x8c10/14/18`) and `[1]` (`+0x8c1c/20/24`), the order landing / Z stateChange / Z window / X stateChange / X window, `m_bDownForce` cleared in each taken branch, `oneDown` clearing both flags, the 0.6 down force.
- `CFGComboDashJumpZXXEventProcess` `32be0030`: ground contact alone goes to the Landing state.
- `CFGComboXXFrontZEventProcess` `32be9210` / `CFGComboXXFrontZZEventProcess` `32be9600` (new): the five cancel times (`+0x8c90` skill, `+0x8c94` back-walk, `+0x8c98` walk, `+0x8c9c` dash, `+0x8c8c` Z / X via vslots `0x4b0` / `0x4b4`), each ground-gated; the dead `|| IsAnimationEnd` in the flag branch; state id `0x144` = `CSI_CFG_COMBO_XXFRONTZZ` (script id 324).
- `CFGComboZZXEventProcess` `32bdf650`, `CFGComboDashZEventProcess` `32bdf820`, `CFGComboDashZZZEventProcess` `32bdfba0`, `CFGComboDashJumpZXXLandingEventProcess` `32be0060`: the walk and dash cancels need `IsOnSomethingFuture`; the arrow test does not; ZZX and ZXXLanding have no arrow test.
- Claimed `= March`: `32bd8ed0`, `32bdfaf0`, `32c01be0`, `32c1be30`; spot-check two line by line, including every float and the two narrow strings of `32c1be30`.
- New enumerator `CSI_CFG_COMBO_XXFRONTZZ` at the end of the `_CORE` block (`X2GUChung.h`).

Hygiene: `GIT_CONFIG_COUNT=1 GIT_CONFIG_KEY_0=diff.algorithm GIT_CONFIG_VALUE_0=patience python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE --range <CH1e commit>^..<CH1e commit>` (FAIL only because the `#define` is commented out), the `_CORE` run, `junediverge.py --check-all`, `enumscan.py --check`, both one-file probes.
Write "CH1e - Review" into the plan doc, update the STATUS row, check `kickoffs/char-chung/CH1f.md`, commit with explicit pathspecs.
