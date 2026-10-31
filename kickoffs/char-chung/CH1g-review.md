# June character Chung (Stage 7) - Phase CH1g review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1g review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1g done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (commented out until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1g - Done (2026-10-31)"
awk -F'	' '$7=="CH1g"' june2017_re/char_chung/ch0_class_map.tsv
```

## Task

CH1g is 20 functions (9 `inside`, 11 `new`). Read them against June (`python june2017_re/charport_adis.py chung --phase CH1g --out <scratch dir>`). The Done note claims 8 `inside` functions `= March` and one that differs; check these:
- **Dash XX FrameMove `32c18950`** (the one `inside` function that differs): FlushMp( 4 x the Open Mind MP rate ) for the 0.533 and 0.93 shots (`32007b10( 0x1b, 0x6731 )` only when `31fceab0( 0x6731 ) > 0`, else 1; 0x1b = `SA_MP_CONSUME_REL`, 0x6731 = `RNSI_P_CDC_OPEN_MIND`); shell power = `GetPowerRate()` x the protected float at `+0x98ac`; the shell goes to `32c15990` with `+0x97d0` (`m_fMiniGunHeadShotRate`); `else if` for 0.93; not-enough-MP effect z = the rotation z.
- **`32c15990`** (new, `SetExtraDamageMiniGun( CEffect*, float )`): confirm the argument is the CEffect (`+0x84c` / `+0x850` are `m_DamageTime` / `m_DamageTimeNow`, funcmap 31f76010 / `31f85910`), the null test, the reset, `++m_DamageTime` only when the rate is above 0, then type 0x26 (`EDT_HEAD_SHOT`), time 5.0, `+0x2b0` = 0 (read as `m_iLevel`: March writes it too), rate and pure rate at `+0x28c` / `+0x298`. Confirm `mov [reg+0x250], 0x26` occurs only here (the Done note says it does).
- **Dash Z `32be9c90`, Z X `32bea080`, Z X X ready `32bea460`, Z X X `32bea4c0`, Z X X end `32be8ef0`** (new EventProcess): flag / `m_fEventTime` windows ([0] = `+0x8c10/14/18` start / end / stateChange, [1] = `+0x8c1c/20/24`), the X key opens the next state, in Z X X the counter `+0x9894 >= 5` picks `CSI_SI_A_CSG_HEAD_SHOT` (id 0xec) instead of looping and the Z window [1] goes to the head shot; Z sets `m_fCanNotAttackTime` on its animation end, the others do not; the five ground-gated cancels as CH1e / CH1f; the dead `|| IsAnimationEnd()` kept.
- **ZX / ZXX / ZXX ready FrameMoves** (`32c15c00`, `32c163f0`, `32c0a8d0`) and Start / End (`32c0abc0`, `32c0ac00`): slots 0x11 / 0x12 are `CHUNG_MAJOR_MII_CSG_COMBO_DASH_ZX_MINI_GUN_L` / `_R`; meshes `Mesh_CSI_CDC_COMBO_ZZXX_Mini_Gun_L/R` (ZX) and `Mesh_CSI_CDC_COMBO_Mini_Gun_L/R` (ZXX, ready); shots 0.378 (right, `Gun_R_Dummy03`) / 0.567 (left) in ZX, 0.036 / 0.164 in ZXX, FlushMp( 2 x rate ), effect `DAMAGE_EFFECT_CSI_CSG_COMBO_DASH_ZX_FIRE_Mini_Gun_Shell`, `GetPos( false )` while the mesh is missing, not-enough-MP effect with all three degrees 0; ZXX `++` the counter in its 0.001 block, ready resets it in its 0.123 block.
- **Claimed `= March`**: spot-check three line by line, including every float: `32bf97b0` (XXX FrameMove, 0.48 cannon shot, `MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE`), `32be08b0` (XX EventProcess: 1.1 window, walk / dash / skill order, the ground tests repeating the else) or `32be0b10`, and `32bf5060` (`SetHandleChungMajorMeshByEnum`).
- Checklist §7 L253 (`+0x7cc8`): the Done note says none of the 20 touches it.
- The `+0x98ac` identification (rifling remodel damage rate, written by `32c16be0` for `RNSI_P_CSG_REMODEL_RIFLE`) and the slot numbering are the two CH1k / CH4a hand-offs; confirm the Done note's wording.

Hygiene: `GIT_CONFIG_COUNT=1 GIT_CONFIG_KEY_0=diff.algorithm GIT_CONFIG_VALUE_0=patience python tools/flagcheck.py JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE --range <CH1g commit>^..<CH1g commit>` (FAIL only because the `#define` is commented out), the `_CORE` run, `junediverge.py --check-all`, `enumscan.py --check`, both one-file probes (`-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` on and off).
Write "CH1g - Review" into the plan doc, update the STATUS row, check `kickoffs/char-chung/CH1g2.md`, commit with explicit pathspecs.
