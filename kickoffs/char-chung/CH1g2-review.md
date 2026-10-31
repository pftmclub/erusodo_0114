# June character Chung (Stage 7) - Phase CH1g2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1g2 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1g2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (commented out until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1g2 - Done (2026-10-31)"
awk -F'	' '$7=="CH1g2"' june2017_re/char_chung/ch0_class_map.tsv
```

## Task

CH1g2 is 30 functions (26 `inside`, 4 `new`) plus one extra `new` (`32bfc860`). Read them against June (`python june2017_re/charport_adis.py chung --phase CH1g2 --out <scratch dir>`; the listings run on past a function's end, cut at the next class-map address that is not `dead`). Check these:
- **Dash-jump mini gun slots.** June frees slots 7 (left, `CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_MINI_GUN_L`) and 6 (right, `_R`) in the one End `32bf9c30` that serves dash-jump X, X X and X X landing. Confirm no June function of the three states writes any other slot (the X, X X and landing FrameMoves `32c18f30` / `32c19710` / `32bf9c70`).
- **Dash-jump X FrameMove `32c18f30`**: meshes `Mesh_CSI_CSG_COMBO_DASH_JUMP_X_Mini_Gun` (left, slot 7) and `Mesh_CSI_CDC_COMBO_Mini_Gun_R` (right, slot 6) with per-frame simulation; attach with `SetPos( GetBonePos )` first (`+0x3f0` = `m_vPos`); shots 0.207 (left, `Dummy01`) and 0.338 (right, `Gun_R_Dummy03`), `GetPos( false )` start, rotation z -35, MP 4 x the Open Mind rate, power x `+0x98ac`, head shot `32c15990( effect, +0x97d0 )`, Fire1. **Dash-jump X X `32c19710`**: left / right `Mesh_CSI_CDC_COMBO_Mini_Gun_L/R`, shots 0.013 (left, `Gun_L_Dummy03`) and 0.127 (right, `Gun_R_Dummy03`), Fire2; only the right shot `inc [+0x96c0]`, a short shot stores 4 (JUNE_BUGS 493). **Landing `32bf9c70`**: the same landing mesh in both hands, no per-frame flag and no `SetPos`.
- **X front fire `32c181f0`**: per-frame simulation, `+0x9718`, three shots at 0.01 / 0.36 / 0.72 (Shell, Shell, Shell_Big; Fire1, Fire2, Fire1), MP 4 x rate, power x `+0x98ac`, head shot on every shell (@32c18726, @32c18911).
- **Head Shot**: EventProcess `32be2ad0` (walk and dash cancels at 0.933 gated on `bFootOnLine`, skill cancel through the macro, arrow cancel ungated); FrameMove `32bfcc80` inside (memo `31fdbcc0( vslot 0x524 )` == 26225 adds 0.15 to the effect's `m_ExtraDamage.m_fRate`; `32007c40( 5, skill, 1, &f )` then `GetRandomFloat() <= f` fires a second shell at +-30 / +10) and `32bfc860` as `CSI_SI_A_CSG_HEAD_SHOT_DefaultFrameMove` (new; skill 26208 -> 26220). Check the `fcompp` direction of the chance test, `vslot 0x524` = `GetNowStateSkillID`, and that `RNSI_A_CSG_HEAD_SHOT*` values (26208 / 26220 / 26225) match `Enum.src.lua`.
- **New dash-jump Z X**: Start `32bdb7f0`, ready EventProcess `32bea970`, EventProcess `32bee360` (`IsFullHyperMode || cannon > 0`, count < 6), FrameMove `32c0ac40`. Check the cannon ball block carefully: the `% 10` roll that is never read, `GetRandomFloat() < 0.5` negating the angle z, `z - 45`, position = `Bip01` +- `GetDirVector() * 35`, the damage effect takes angle twice, `UseCannon( 1 )`, `ShowOnomatopoeiaParticle( L"", L"STR_Kwang" )`, misfire sets the count to 6 and the tail adds 1.
- **Claimed `= March`**: spot-check three line by line including every float: `32be0270` (X front fire EventProcess: 1.02 / 1.2 / 0.43), `32bf9f50` (landing EventProcess), `32bf9310` (X front X FrameMove, 0.44 cannon shot).
- The ground tests June repeats in `= March` EventProcess (`bFootOnLine` inside the `IsOnSomethingFuture` else) are treated as redundant: confirm.

Hygiene: `git -c diff.algorithm=patience` runs of `python tools/flagcheck.py JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE --range <CH1g2 commit>^..<CH1g2 commit>` (FAIL only because the `#define` is commented out), the `_CORE` run, `junediverge.py --check-all`, `enumscan.py --check`, both one-file probes (`-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` on and off).
Write "CH1g2 - Review" into the plan doc, update the STATUS row, check `kickoffs/char-chung/CH1h.md`, commit with explicit pathspecs.
