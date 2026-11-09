# June character Rena (Stage 7) - Phase CH1f review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1f review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1f done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (defined; 2 enum members + the `ADD_RENA_SYSTEM`-off member, 4 new functions, `m_vSpineAimDegree`, the `IF_` macro, `COMBO_HIT_GROUP_VALUE`), `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (`//#define`; natural force lines in the siege shots, dash X-Z, X4 and dash-jump X3 arrows) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1f - Done"
awk -F'\t' '$7=="CH1f"' june2017_re/char_rena/ch0_class_map.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1f - Review"
```

## Task

Review the 43 CH1f rows against June (`python june2017_re/charport_adis.py rena --phase CH1f --out <dir>`; stop each listing at its first `int3` or at the next class-map start; `32c87c20` and `32c928f0` run into the next function). Spot-check at least:
- **Class-map corrections.** `32c77f90` = the Wind Sneaker's dash X (flag 1 only, `LESI_LWS_DASH_XZ` id `0xd1`, no class test), registered at line ~1195 of `32cadc90` beside the Sniping Ranger's `32c73500`; `32c7bd20` = `SetSiegeModeCoolTime` (ids 23201 / 23221 with the upgrade swap, `GetActualCoolTime` vslot 0x6a4, `SetSkillCoolTimeLeft( 23201 )`, `JUNE_BUGS` #565).
- **Siege Mode.** MP ends the mode below 15 / 25 ( `32c83f70` / `32c84020`; the listing is `GetNowMp() < N * +0x95ac` ); Z `32ca5330`: NF count at `m_pGageData +0x114`, `ConsumeNaturalForce( 1 )`, `FlushMp( 15 * rate )`, power `GetPowerRate() * +0x95b8`, aim = `+0x97d4` ( written by `32c863d0`, CH1i ), two arrows ( the second reserved 0.12 s ) collected in a vector, weak shot `FlushMp( 3 )` with `1.54 * +0x95b8`; X `32ca5aa0`: `DoAimingTarget` result dropped, 25 MP, parabolic volley, weak shot `1.154`. Check the NF arrow names against the listing strings. Ready `32c87c20` / `32c6f560`: id lookup order ( 0x5aa1 then 0x5ab5 ), `GetSkillLevel( id, 0, 0, 1 )`, `SA_READY_TIME_REL` ( 0x26 ).
- **EventProcess chains** ( cancel order and which cancels have a foot test, per listing ): combo X2 `32c7b1b0`, X3 `32c7b570` ( flags 0 / 1 / 2 -> ids `0xb5` / `0xb6` / `0xb7`; walk, dash, skill, back-walk ), X4 `32c7ba40` ( plain walk and dash, foot-tested back-walk, Z / X -> `0xd` ), dash X1 `32c73500` ( animation end tests the foot first; `SpecialAttackEventProcess( 1 )` right after it ), dash X2 `32c73960`, dash-jump X2 `32c74840` ( flag 1 = Grand Archer `oneZ` -> `0x10e` ), `RSR_DASH_ZX` `32c810e0` ( animation end AND foot -> `0x12` ), double-jump X `32c6e450` ( `0xfc` = 252, then landing `0x9f` ).
- **Arrows.** MP values and arrow names of the nine FrameMoves ( `ch1f` Done note table ); `RSR_DASH_ZX` `32c9faa0` from `Dummy2_Lhand` with y + 180 and z, z + 15, z - 15; the natural force type store ( `[effect + 0x76c] = 1` ) in X4, dash-jump X3 and `RSR_DASH_ZX` only; dash X1's class switch ( 13 / 103 RSR, 102 RWS, no default ).
- **Enum / header.** `LESI_RGA_DASH_JUMP_XXZ_FROM_X2` 270, `LESI_SNIPING_RANGER_DOUBLE_JUMP_ATTACK_XX` 252 under `#ifndef ADD_RENA_SYSTEM` ( with `ADD_RENA_SYSTEM` defined March's own member is used ), `RNSI_A_RENA_SIEGEMODE` / `_UPGRADE` in `X2SkillTree.h`.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range <CH1f commit>^..<CH1f commit>` ( FAIL only for the commented-out `#define` ); `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`; one-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` off, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. Fix clear transcription errors in place ( bytepatch only ). Write "CH1f - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1f2.md`, commit with explicit pathspecs.
