# June character Ara (Stage 7) - Phase CH4b review ( the CH4 stage review: covers CH4a + CH4b )

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH4b review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH4b done (its Done note, commit and the user's play-test + "check logs"); CH4a's play-tests are in its Done note |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_PASSIVE` and `..._PASSIVE_DEBUG` (the debug define retires here: `//#define` in `Always.h`) |
| Log tag | `[JUNE_CHAR]` (`ARA PASSIVE <what>`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH4a - Done (2026-11-12)" "CH4b - Done (2026-11-12)"
grep -n "Ara: CH4" JUNE2017_CHARACTER_PORT_CHECKLIST.md
grep -n "Char Ara CH4" june2017_re/funcmap.tsv
grep -n "char-ara CH4" tools/data/june_march_divergent.tsv JUNE_BUGS.md
```
If a Done heading above differs ( `--list` the outline ), pull the right one.

## Task

Check every CH4 sub-phase's code against June from the listings ( `python june2017_re/charport_adis.py ara --phase CH4a > <scratch>`, the same for `CH4b`; a listing runs on into the next function: stop at the `int3` / `ret`; Ghidra read-only for a spot-check).
- **CH4a ( from its kickoff amendment ):** re-read `InitPassiveSkillState` `32bcd9c0`, the 0x780 hook `32bca010`, the CEffect hook `32ba6440` and the four delegates ( `32b95910`, `32b95980`, `32b959a0`, `32ba7e40` ) from the exe; verdict lines of their own for `X2GUUser.*` ( `ProcessPassiveReturnFromDeath`, the `ProcessSkillAtDie` Ara block ), `X2GameUnit.*` ( `ReserveToFinishDebuffTempletsFromGameUnit` ) and the enum headers ( `X2SkillTree.h`, `X2DamageManager.h` `JUNE_ENUM_NAMES` ).
- **CH4b: 9 class-map functions + 2 callees.** Re-read all of `32b96730` ( the switch table `32b96810` / bytes `32b96818`: cases 27010 / 27202 / 27203 / 27206 / 27223 ), `32ba45d0`, `32b956a0`, `32b958e0`, `32ba47f0`, `32badc80`, `32bc1530` ( stops at `ret 8` `32bc15cd` ), `31fca980`, `32d3f040`, and the two call sites in `32007570` ( `@32007664` 0x4c4, `@3200772d` 0x4c8 ). Confirm the n/a verdict on `MemoConditionAra` ( `32b955c0` = `31fd4450( arg ) != 0` ).
- **CH4b shared files ( verdict line each ):** `X2GUUser.h` / `.cpp` ( the two base virtuals 0x4c4 / 0x4c8 and their calls around `PlayEffectSet` in `CommonFrameMove_EffectSet`, `IsSkillEquipped` ), `X2ComboAndSkillChainSystem.h` ( `GetSkillIDList` ), `Always.h` ( the third `#error` guard; `SERV_BALANCE_FINALITY_SKILL_EVENT` is defined after `Always.h`, so it is not in the guard ).
- **Open from CH4a / CH4b:** the Conversion Health branch of the 0x780 hook ( `conversion health hp recovery`; CH4b moved 27210 to bank A on unit 48, see CH4b's logs ); the Yama Raja energy-ball colour report ( CH4b found every asset June's byte-exact: the cause is not Ara's data; decide the route from CH4b's Done note ).
- **Valor Devotion's MP cut ( `valor devotion mp` ) was not logged in CH4b's test:** it needs buff index 0 of 27416, which `UpdateValorDevotionTimer` ( `32ba7da0`, CH1j ) applies at a full force-power gauge. Confirm that path from the listing and, if the review hands off a play-test, have unit 98 fill the gauge and then cast Turbulent Wave / Rock Crash / Low Brandish / Tiger Claw.
- **Play-tests:** CH4a's from its Done note; CH4b's from "CH4b - Done" ( the log lines it lists, 0 EXCEPTION / Motion Error / KEM_ERROR ).

Hygiene: `flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARA_PASSIVE --with SERV_IRUHADEV_JUNE_CHAR_ARA_PASSIVE_DEBUG --range e3b2a1fe^..<CH4b code commit>` ( e3b2a1fe = CH4a's code commit; explain every OTHER row, e.g. the enum headers = `JUNE_ENUM_NAMES`, and every deleted line ), `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUUser.cpp X2Lib/X2GameUnit.cpp`, `skillicons.py check 27400-27420`, a one-file `/Zs` probe of `X2GUAra.cpp`.
Retire `_PASSIVE_DEBUG` ( `//#define` in `Always.h`, bytepatch; a build whose only change is a commented `_DEBUG` define starts but is not awaited ).
Fix clear errors in place (bytepatch only). Write "CH4b - Review" in the plan doc ( the CH4a findings in it ), update the STATUS rows ( CH4a-CH4b done ), and check `CH6.md`.
