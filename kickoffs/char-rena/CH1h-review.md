# June character Rena (Stage 7) - Phase CH1h review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1h review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1h done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`; 25 `inside` bodies), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (defined; 2 new functions, 5 members, `TryGetAcquiredSkillAbilityValue`), `..._NF` (undefined; 3 blocks), `SERV_IRUHADEV_JUNE_ENUM_NAMES` (9 enum members) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1h - Done"
awk -F'\t' '$7=="CH1h"' june2017_re/char_rena/ch0_class_map.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1h - Review"
```

## Task

Review the 33 CH1h rows against June (`python june2017_re/charport_adis.py rena --phase CH1h --out <dir>`; every listing runs past its function: `32ca7860` runs into `32ca78b0` = InitPassiveSkillState, `32c835c0` into `32c83760`). Spot-check at least:
- **The 10 `differs` rows ( the `SetRena*ParticleByEnum` pair included ):** `DamageReact` (`[edi] == 6` = `DT_BUFF` skips the delegate; the two percent-up tests are `<= 0`), `ApplyRenderParam` (`+0x6840` pass dash, `+0x3e58` invisible, `+0x144` alpha object), `InitEnchantWeaponEffectUnitSpecific` (all 15 ids against `Enum.src.lua`: 3003 / 4004 / 4546 / 4657 / 4768 -> 1, 3033 / 4044 / 4576 / 4687 / 4798 -> 2, 3303 / 4404 / 4846 / 4957 / 5068 -> 3), `InitComponent` (no `SetNowMp( 50 )`; six tags `32296da0( handle, 1 )`; `SIEGE_ANGLE` -> +0x97c4..+0x97d0), `UpNowMpPerHitOthers` (jump table at `32c9b3bc`: types 1 / 4 plain, 2 / 3 x 0.8), `HyperModeBuffEffectStart` (transcended `+0x3c` and `31fd66b0` return), the destructor (order: +0x9818, +0x9678, seven minor handles, three anims, major / minor deletes, three spinning-kick handles, `+0x962c` delete, trap blade, evoke vector, prior planned blow, delayed firing, +0x9600, +0x977c, NF block, `KillMySummonedNPC( 0x823, 3.0 )`) and ReInit (`GetNowMp() < 50` and game type != 1; the softbody tail: `+0x405c > 0`, BFI 0x1a8 with the 5-argument call `( 424, creator 0, level 1, adjust 0, skip 0 )`, BTI 0x18b first finalizer, `KillMySummonedNPC`).
- **The two new rows:** `32ca9ca0` (`+0x5c4` = `m_iAppliedSkillID` through `GetUpgradeBaseSkillID`; ids 0x5aa7 / 0x5aa5; the `0x280` call's seven arguments = `ResetLeftSkillCoolTimeByBuff( ST_NONE, -1, false, BCT_FIX_VALUE, 1, true, 0 )`; `0x238` = `GetSkillBuffFactorPtr( 0x5b75, 0, true )`; `0x224` = `SetBuffFactorToGameUnit`) and `32cb3200` (triggers 0x25 / 0x26 = `DTT_ERADICATION` / `_EXPLOSION`; `32007c40` with swap 1; the `fcompp` tests read as `count >= limit`; the effect set is played with March's defaults).
- **The "= March" rows** the Done note lists, each read against its listing.
- **CH0 map corrections:** `32c6d870` is a thunk; `32c89270` / `32c95670` / `32c9e130` / `32c9b320` / `32c85d50` are March's functions (CH0 listed them as `new`); `32cb3500` / `32cb4200` / `32cb4270` are March's `FriendshipNatureData` / `TimeLimited` inlines.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range <CH1h commit>^..<CH1h commit>` (FAIL only for the commented-out `#define`); `python tools/flagcheck.py SERV_IRUHADEV_JUNE_ENUM_NAMES --range <CH1h commit>^..<CH1h commit>`; `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`; one-file `/Zs` probe (`census.cl_cmd`, `/c /Zs /EHsc`) of `X2GULire_ElvenRanger.cpp` off, with `-DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. Fix clear transcription errors in place (bytepatch only). Write "CH1h - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1i.md`, commit with explicit pathspecs.
