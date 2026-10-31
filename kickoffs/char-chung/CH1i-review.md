# June character Chung (Stage 7) - Phase CH1i review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1i review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1i done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (commented out until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1i - Done (2026-11-01)"
awk -F'	' '$7=="CH1i"' june2017_re/char_chung/ch0_class_map.tsv
```

## Task

CH1i is 15 functions: 14 `inside` and 1 `new` (the Shelling Guardian combo line, `CSI_CHG*` and the two `CSI_P_CHG_CANNONEER_WITHSTANDING_*` states). Read them against June (`python june2017_re/charport_adis.py chung --phase CH1i --out <scratch dir>`; the listings run on past a function's end, cut at the next class-map address that is not `dead`). Check these:
- **The four EventProcesses that differ** (`32be52a0` X, `32be54f0` dash Z, `32be5680` dash Z Z, `32be5960` dash jump Z X): the flag clause is `bEventFlagList[n]` and (`m_fEventTime[n].stateChange` < now **or** the future animation's end `+0x2ac`), then the key window `keyInputStart <= now < keyInputEnd`. Confirm the flag / `m_fEventTime` index of each (X: 0 = Z to `CSI_CHG_COMBO_XZ` 0x10b, 1 = X to `CSI_CHG_COMBO_XX` 0x10a; dash Z: 0x10c; dash Z Z: 0x10d; dash jump Z X: 0x147), the order (animation end → `USI_WAIT` first, then the foot test), the member cancels (`+0x8c98` walk, `+0x8c9c` dash, `+0x8c90` skill) and that the `32be5960` end branch tests the foot (`USI_JUMP_DOWN` with y - 45, else `USI_WAIT`).
- **The `= March` bodies**: spot-check `32c042c0` (X X FrameMove: 0.55 aim, 0.666 shot / misfire with `GetCannonBallCount() >= 1` or `IsFullHyperMode()`, 0.69 `CHG_COMO_XX_EXPLOSION`) and `32c048b0` (0.415 / 0.581 / 0.585 / 0.834), including every float and the `m_fSiegeShellingUserBallRate < GetRandomFloat()` test.
- **The new row `32c11ad0`**: byte-identical to `32c0e1f0` except the jump targets; the enumerator `CSI_CHG_DASH_JUMP_COMBO_ZXX` (script id 327, `Enum.src.lua` `USI_END + 175`) and its EventProcess `32be8ef0` (CH1g's `CSGComboDashZXXEndEventProcess`, registered by CH1m).
- `+0x7cc8` (checklist §7 L253) is read by none of the 15.

Hygiene: `git -c diff.algorithm=patience` runs of `python tools/flagcheck.py JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE --range <CH1i commit>^..<CH1i commit>` (FAIL only because the `#define` is commented out), `junediverge.py --check-all`, `enumscan.py --check`, both one-file probes (`-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` on and off).
Write "CH1i - Review" into the plan doc, update the STATUS row, check `kickoffs/char-chung/CH1j0.md`, commit with explicit pathspecs.
