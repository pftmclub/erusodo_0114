# June character Elsword (Stage 7) - Phase CH1h review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1h review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1h done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (commented out until CH1 final) and `_CORE` (defined by CH1d) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1h - Done (2026-11-04)"
awk -F'\t' '$7=="CH1h"' june2017_re/char_elsword/ch0_class_map.tsv
```

## Task

Review the CH1h functions against June (`python june2017_re/charport_adis.py elsword --phase CH1h --out <dir>`; a listing can run past its function: stop at the `int3` or the next class-map address, and `32d17e90` runs on into `32d17ea0`). The risk sits in these:
- **DamageReact `32d1ac60`** (the longest). Check the order and sense of every early-out in the auto guard block: the extra-damage switch ( table `32d1b100`, types 0x10 / 0x1a / 0x22 / 0x2f ), `attackType == 1` skips, `m_bNoDamage`, the two `fcom` tests ( `DD +0x504` / `+0x588` = March `m_fHpPercentUp` / `m_fIncreaseMP` ), `reActResult` 7 / 0xa, `hitType` 0x1a, hold `+0x8a48`, grab `+0x9108`, riding, the cool-time pair `fcompp` + `test ah,0x41` ( skip when limit `+0x9770` is above elapsed `+0x9764` ), the counter state ( `GetNowStateSkillID` vs 21206 ), super armor, invincible, `GetFrameData( false ) + 0x108` = foot on a line, the `+0x7e2d` attacker-front roll, the elapsed reset, then the destruction-rage and the tutorial byte ( `g_pMain +0x16a` ), and the WSP gain.
- **ApplyBuffToHarshSlayer `32d1b9d0`**: the order ( defender first, then the 21420 buff to the attacker for `+0x3c == 1`, then the defender buff, then self buffs 1 / 2, then the elapsed reset ), `GetSkillLevel( id, 1, 0, 1 )`, the `SetGameUnitBuffFactor( unit, creator UID )` pairs, and the cool-time helper `32d059b0`. Check that `SetGameUnitBuffFactor( pUnit, GetUnitUID() )` is June's `32d4d670` call ( vslot 0x54 returns `+0x270 / +0x274` ).
- **ReInit `32d1a920`**: the +-1 point nudge before the PvP test, the RED / BLUE kill-count conditions, the non-PvP branch, and where `m_fDestructionRageTime` is zeroed ( last, for every unit ).
- **Destructor `32d1a5d0`**: the three `StopEffectSet` offsets ( `+0x95a4` / `+0x95a8` / `+0x95b8` ) against the member map in `ch1_offsets.tsv` ( the CH1h claim: June has no `m_hSwordFire` before `+0x95b0`, so `+0x95b8` is `m_hSwordEnchant` ); the sword blasting loop; no black-hole destroy.
- **InitEffect `32d0d540`** ( the my-gage-UI refresh and what vslot 0x5c `32e681d0` does: `+0x110 = 1` on the gauge data, then `UpdateUIElswordWSP` `32e67df0` ), **InitComponent `32d0d3f0`**, **HyperModeBuffEffectStart `32d17ea0`** ( `+0x3c != 0` skip, awakening present test ).
- **Hooks `32d05b60` / `32d05b70` / `32d0c780`** and the class-map corrections ( `32d0d600`, `32d0d7a0`, `32d1b9d0` are inside rows ).
- Re-read the identical-to-March ones against the listing: `32cf2da0`, Win / Draw / Lose, the two particle arrays, `COMMON_BUFF_FrameMove`, Set / UnSet specific values ( jump tables `32d1f83c` / `32d0b950` ).

Hygiene: `python tools/flagcheck.py JUNE_CHAR_ELSWORD_LIVE --with JUNE_CHAR_ELSWORD_CORE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h` ( then `--range <commit>^..<commit>` ); `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` ( a Git Bash `/D` argument is path-converted: use `-D` ). Fix clear transcription errors under the same flag ( then `tools/build.py --deploy` ).
Write "CH1h - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH1i.md`, commit with explicit pathspecs.
