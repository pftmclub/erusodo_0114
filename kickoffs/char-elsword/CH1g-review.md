# June character Elsword (Stage 7) - Phase CH1g review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1g review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1g done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (commented out until CH1 final) and `_CORE` (defined by CH1d) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1g - Done (2026-11-04)"
awk -F'\t' '$7=="CH1g"' june2017_re/char_elsword/ch0_class_map.tsv
```

## Task

Review the 31 CH1g functions against June (`python june2017_re/charport_adis.py elsword --phase CH1g --out <dir>`; a listing can run past its function, stop at the `int3`). The branchy ones carry the risk:
- The eight EventProcesses: Z Z Z `32cfc900` (no direction cancel, `SKILL_CANCEL_AFTER( 0.01f )` before the flag branch), Z Z Z Z `32cfcb70` (flag branch before the skill cancel), Z Z Z Z Z `32cfce30`, X X `32cfcfe0` (two flags on `m_fEventTime[0]` / `[1]`, `oneDoubleX` / `oneX`), X X X loop `32cfd210` (loop count `< 3`, flag 1 to X X X Z), X X X Z `32cfd540` (ground walk cancel BEFORE the skill cancel), X X X end `32cfd700` and dash combo Z Z `32cfd7a0` (same body, ground walk cancel only). Check each against March's `WALK_CANCEL_AFTER` (March had no foot test) and the `ELSWORD_ELSE_IF_FLAG_*` macros.
- The new states `32d054d0` (dash jump Z Z Z EP: `DASH_JUMP_LANDING` on a line) and `32d19b80` (FM at 0.296: `IsEnoughMPForAttack( 8, Dummy2_Lhand, 0, ( 0, 0, -45 ) )`, then `EffectSet_ETK_DASH_JUMP_ZZZ`): re-derive the cost 8.0 (`3344d3e4`), the rotation z -45.0 (`33491dc8`) and the default `PlayEffectSet` arguments from the listing.
- The FrameMoves: Z Z Z `32d1e200` / X X `32d1e5a0` (sheath mesh at 0.01 + alpha tag 7c, dash-cancel handle reset, `DAMAGE_DATA_NEXT`), dash combo Z Z `32d1ea10` (+ `DAMAGE_DATA_LAST` at `m_fDamageDataChangeTime2`, string at `335197f0`), the five damage-only ones.
- StateStart / StateEnd (14 tiny ones): loop count `+0x962c`, `bSuperArmorNotRed` `+0x8040`, sheath handle `+0x9670`.
- Offsets used but not in `ch1_offsets.tsv`: `+0x8d38` m_fDashCancelAfter, `+0x9624` / `+0x9628` damage change times, `+0x962c` loop count, `+0x9670` sheath handle. Confirm and add them.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_ELSWORD_CORE --with JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h KTDXLIB/Always.h`; `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (a Git Bash `/D` argument is path-converted: use `-D`). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1g - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH1g2.md`, commit with explicit pathspecs.
