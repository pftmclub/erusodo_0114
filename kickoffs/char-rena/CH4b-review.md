# June character Rena (Stage 7) - Phase CH4b review ( the CH4 review: covers CH4a + CH4b )

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH4b review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH4b done (its Done note, commit and the user's play-test + "check logs"); CH4a's play-test and log check are in "CH4a - Done" (no review of its own, user 2026-11-10) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_PASSIVE` and `..._PASSIVE_DEBUG` (the debug define retires here: `//#define` in `Always.h`) |
| Log tag | `[JUNE_CHAR]` (`RENA PASSIVE init`, `open mind effect`, `predator lucky effect`, `rage fungus lock`, `eradication debuff explosion`, `friendship of nature buff`, `vital point piercing`; CH4b: `mp:`, `cool time:`, `powerful bowstring`, `sharp kick`, `gungnir upgrade spear`, `trapping arrow fungus upgrade`, `shooting magnum upgrade`, `crazy shot upgrade`, `furious engage upgrade`, `random shot pleasure`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH4a - Done (2026-11-10)" "CH4b - Done (2026-11-10)"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH4b - Review"
grep -n "Rena: CH4a\|Rena: CH4b" JUNE2017_CHARACTER_PORT_CHECKLIST.md
grep -n "Char Rena CH4a\|Char Rena CH4b" june2017_re/funcmap.tsv
```

## Task

Check CH4a's and CH4b's code against June from the listings (`python june2017_re/charport_adis.py rena <va ...>`; a listing runs on into the next function: stop at the `int3`; Ghidra read-only for a spot-check).
- **CH4a.** `InitPassiveSkillState` `32ca78b0` (uncarved) block by block against the port ( every id is June's `RNSI_P_*`, levels through `GetSkillLevel( id, true, 0, true )`, D8 Powerful Bowstring 23405, no Sharp Arrow read, Soft Body 23402 keys the quick get-up for classes 12 / 102, Open Mind's first-learned of 23412 / 23414 / 23416 ); the four attack-result callbacks `32ca2180` / `32ca2390` / `32c86790` / `32ca28b0` and their thunks; `ProcessFriendshipNature` `32c6dac0`, `ProcessVPP` `32c7af80` ( the roll offset 0x10 + the per-frame count ); the NPC push `3275a490` in `CX2GUNPC::InitDelegateProcess` `3275f0a0` and its GUT_USER stand-in ( shared-file verdict line for `X2GUNPC.*` ).
- **CH4b, `SetDamageExceptionProcess` `32cad510`.** The eight blocks in June's order: the `+0x5c4` tests ( 23025 / 23209 / 23014 ) against `m_iAppliedSkillID`, the `31fca8e0( id, 1 ) +0x178` tests against `IsEquippedSkillUpgraded`, the reActType `+0x24` swaps ( RT_UP 4 / RT_FLY 5 -> RT_BIG_DAMAGE 2 ), the two mana-burn blocks ( `+0x50` / `+0xa4` / `+0xc8` / `+0x55` / `+0x8c`, and `+0x151` only for Shooting Magnum ), Gungnir's `+0x3c` fMagic only ( June's spear DE is `DT_MAGIC` ), Powerful Bowstring ( hitType `+0x20` 16 and attackType `+8` 2, `32890620( 23405, 1, 0, 1 )`, min 30, roll `<=` SA 5, word `+0x528`, the effect set on `g_pData +0xe0` ), Sharp Kick ( `32610fb0( 4 )`, `32890620( 23400, 0, 0, 1 )`, `+0x3f8` x SA 0x1c ), Random Shot Pleasure ( `+0x53c == 23` read directly, not through `32610fb0` ). The null `pDamageData_` return is a stand-in.
- **CH4b, `GetActualMPConsume` `32c85690`.** The switch on `32207210( id )`; the Eradication test is `limit <= count` ( `test ah,0x41; jp` ), not `!( limit > count )`; the memo cases dropped; the clamp before the natural force term; `min( NF, templet +0x10, 3 ) / 3.0 x spec B` ( `+0x6d04` ) and its operand widths ( `charport_consts.py --fn 32c85690` ); the rate templet is the original id's, the natural force templet the base id's.
- **CH4b, `GetActualCoolTime` `32c7be00`.** The -1.0 Eradication value ( `334380f0` ), the third argument treated as false ( June's only `true` caller `31fd8ff0` @31fd9077 is not in March: confirm no March caller of the virtual stands for it ), `HaveSkillBuff( 23415, 1, true )` ( vslot 0x240 index 1 ), no clamp. **Eradication's count path was never logged in the play-test** ( see the Done note ): trace where `GetActualMPConsume` / `GetActualCoolTime` are called for 23060 relative to `TryStateChangeEradicationAttack` ( June `32ca2af0` resets the count ) and `SpecialAttackCooldown` `32c89a20`, in June's order; if the free path is only reachable through the use-condition MP check, say so; if the port's order differs from June's, fix it.
- **MemoConditionRena** `32c6d4d0`: n/a confirmed ( `31fd4450` always false ).
- **Soft Body user action 0x28 writer** ( CH1k / CH4a amendment ): not in `32cad510`, `32c85690`, `32c7be00`. Say in the review whether June writes it anywhere in Rena's range ( a byte scan for the `0x28` action store ), or that June has none.
- **Shared-file verdict lines:** `X2DamageManager.h` ( triggers 23 / 34 / 35 against `Enum.src.lua`, plus CH4a's 20 / 21 / 22 / 24 ), `X2SkillTree.h` ( CH4a's 10 names ), `X2GUNPC.*`, `Always.h` ( the CH4b guard line ), `tools/data/june_march_divergent.tsv` rows `SetDamageExceptionProcess` / `GetActualMPConsume` / CH4a's three.
- **Play-test.** Read `logs.py --grep "RENA PASSIVE"` for CH4b's lines ( the Done note lists which unit gives which ), `ErrorLog.txt` ( 0 errors ), and record what was not seen.

Hygiene: `flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_PASSIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_PASSIVE_DEBUG --range dd5435d3^..<CH4b commit>` ( dd5435d3 is CH4a's code commit; the files of both phases ), `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GUNPC.cpp`, a one-file `/Zs` probe of the cpp. Retire `_PASSIVE_DEBUG` (`//#define`; a build with the only change a commented `_DEBUG` define starts but is not awaited).
Fix clear errors in place (bytepatch only). Write "CH4b - Review" in the plan doc ( CH4a findings in it ), update the STATUS rows ( CH4a, CH4b done; CH6 ready ), and check `CH6.md`.
