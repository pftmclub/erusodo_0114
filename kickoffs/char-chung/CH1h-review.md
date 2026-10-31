# June character Chung (Stage 7) - Phase CH1h review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1h review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1h done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (commented out until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1h - Done (2026-11-01)"
awk -F'	' '$7=="CH1h"' june2017_re/char_chung/ch0_class_map.tsv
```

## Task

CH1h is 22 functions, all `inside` (the 13 `CSI_CDC*` states of the Deadly Chaser combo line). Read them against June (`python june2017_re/charport_adis.py chung --phase CH1h --out <scratch dir>`; the listings run on past a function's end, cut at the next class-map address that is not `dead`). Check these:
- **The three FrameMoves that differ**: `32c19f30` (X: one shot at 0.599, `Shell_Big`), `32c1a480` (X Z: 0.632 left, 0.965 right, 1.332 left, `Shell`) and `32c1ae60` (Z Z X X: loop 0.6 step 0.25 left, loop 0.67 step 0.25 right, 1.67 `Last_Shell`). Each shot: `31fceab0( 0x6731 )` > 0 then `32007b10( 0x6731, 0x1b )` for the Open Mind rate, `FlushMp` of 4 x (Z Z X X: 2 x) rate, power `GetPowerRate() * +0x98ac`, 11th argument `+0x96b8`, head shot `32c15990( effect, +0x97d0 )`, Fire1, and the short-of-MP effect at the same gun's position (Z Z X X: angle z 23). Confirm which gun (left / right) each shot's position and misfire use from the stack offsets, the loop bounds (`<= 1.62` and `<= 1.43`, 5 and 4 iterations), and the `+0x9718` write (X at 0.599, X Z and Z Z X X at 0.001).
- **The `= March` EventProcesses**: spot-check line by line including every float: `32be3320` (X Z: 1.3 / 1.458, then the cancels at 1.074 and 1.495 and the two member cancels), `32be3a30` (Z Z X X: 1.5 / 1.9, member walk / dash / skill cancels) and `32be31f0` (X X: the walk cancel clearing both flags, the 1.282 / 1.584 window).
- **Mini gun slots**: June frees 9 (X), 10 / 11 (X Z), 13 / 14 (Z Z X X) and reads the same slots in the FrameMoves (`+0x95ec`, `+0x95f0` / `+0x95f4`, `+0x95fc` / `+0x9600`); slot 12 is never touched. Confirm, and read the `CH1k.md` amendment.
- **X X FrameMove `32c1ba00`**: the `DAMAGE_DATA_NEXT` table call and the `32bf3d30( 1.232f, 1 )` cannon ball call (declared as March's `UpCannonBallCountAtThisAnimationTime`).

Hygiene: `git -c diff.algorithm=patience` runs of `python tools/flagcheck.py JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE --range <CH1h commit>^..<CH1h commit>` (FAIL only because the `#define` is commented out), `junediverge.py --check-all`, `enumscan.py --check`, both one-file probes (`-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` on and off).
Write "CH1h - Review" into the plan doc, update the STATUS row, check `kickoffs/char-chung/CH1i.md`, commit with explicit pathspecs.
