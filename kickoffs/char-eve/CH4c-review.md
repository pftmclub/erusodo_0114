# June character Eve (Stage 7) - Phase CH4c review - El Crystal and Code Recovery

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH4c review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH4c done and play-tested |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_UI` (+ `SERV_IRUHADEV_JUNE_CHAR_EVE_UI_DEBUG`, retired here), `SERV_IRUHADEV_JUNE_GROUND_COMBO_END_EVENT` |
| Log tag | `[JUNE_CHAR]` (`EVE UI`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH4c - Done (2026-10-28, Opus 5.5)"
```
Then the `CH4c` rows at the end of `june2017_re/funcmap.tsv` and JUNE_BUGS rows 464-466.

## Task

Review the CH4c port against the listings (Ghidra for `329b57a0`, `329b4860`, `31fcc350`, `322e65c0`, `322dee90`, `322d9250`, `322d99d0`, `328e0aa0`; `python june2017_re/disasm_range.py` for the uncarved `3299b180` and the jump-table cases of `329b57a0` at `329b58ad` / `329b58e2` / `329b59e0`). Where it could be wrong:
- **`329b57a0`.** The MP test `fcom` direction (continue when cost <= MP, return with no cool time otherwise); `32007b10( 0x1b, 25418 )` = `SA_MP_CONSUME_REL`; `[33436c90]` = -1.0; the HP formula `+0x9c70 x max HP` with the 0.0 second argument of vslot `0xec`; the class table (`329b5a84`: 18 / 108 -> 0, 19 / 109 -> 1, 26 / 116 -> 2); the vslot `0x21c` argument order `( BFI, 0, 1, 0, 0 )` -> March `( BFI, 1, NULL, 0, false )`; the 3299b180 order 519, 384, 1621, 1622, 1454 and 329b4860's 384, 519, 1621, 1622, 1454.
- **`3299b180`.** HaveThisBuff 0x3d8 = `BTI_DEBUFF_FESSEL_HEILIG` (June's Enum value 984); AI `+0x38` = NAT_ALLY, `+0x78` owner UID; `32767930` = `SetPosition`; the target release (`+0x3c` / `+0x40`) = `CX2NPCAI::ResetTarget` and that it runs for any owner (JUNE_BUGS 465).
- **`329b4860`.** No owner test (row 465); `+0x8b40` = `GetSelfDestruction`; vslot `0x238( 25418, 0, 1 )` and vslot `0x224` on the raw pointer (the port's NULL guard).
- **`31fcc350`.** That it is June's `GenericGroundComboEndEventProcess`: March's body plus `31fcc9d0`'s chain without the back-walk (`+0x8c94`) branch; `[33472998]` = 45.0.
- **`322e65c0` / callers.** `+0x714` = `m_pDLGSkillSlot`; vslot `0x78` = `SetShowEnable( a, b )`; `31f5ee50( 0x27 )` = `GetKeyString( GAMEACTION_SIT )`; cool text = `ftol( cool + 1.0 )`, `L""` at <= 0; the four callers' arguments (`@32307c00`, `@3230898d`, `@329af1cc`, `@32981120`) and that `@32981120` has no my-unit gate (row 464).
- **`322d9250` / `322d99d0` / `328e0aa0`.** `+0x97e4` / `+0x97f0` = `m_EveElectraSystem.m_bSystemEnable` / `m_ePlaneFormationType`; `+0x9a20` / `+0x9a18` = `m_iThousandStarCount` / `m_bEntireFireMode`; the slot-9 skip rule (`32890ee0( 0 )`, `( 1 )`, `3214f3d0 == 3`); that `322d99d0` is March's `UpdateCodeEmpressSkillSlotIcon`; `328e0aa0`'s class 0x1a only (row 466).
- **Hand-off check:** the log lines the CH4c hand-off lists.

Retire `SERV_IRUHADEV_JUNE_CHAR_EVE_UI_DEBUG` (define and lines). Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_UI --with JUNE_CHAR_EVE_UI_DEBUG JUNE_GROUND_COMBO_END_EVENT --range <first CH4c commit>^..HEAD`; `enumscan.py --check X2Lib/X2GuEve.cpp X2Lib/X2Game.cpp X2Lib/X2UISkillTreeNew2015.cpp X2Lib/X2GUUser.cpp`; `python june2017_re/char_eve/ch1m_regcheck.py` (0 problems). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions to the user. Next: `/x2-phase char-eve CH5` (its kickoff exists) or CH6 once CH5 is reviewed.
