# June character Ara (Stage 7) - Phase CHNb review (covers CHNb + CHNb2)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CHNb review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CHNb and CHNb2 done (their Done notes, commits and the user's play-test of CHNb2) |
| Flag | `SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE` (class-generic; `KTDXLIB/Always.h` end, `#error` without `_TRANSCEND_CHANGE`, `_SKILL_CASH_OPEN_ITEM`, `_QUEST_LOAD`, `_ENUM_NAMES`, `PRIVATE_BANK`). No debug define. |
| Log tag | `[JUNE_CHAR]`, `[JUNE_SKILL] HUD shop open-at-item`, `[ADMIN_CHEAT]` |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CHNb - Done (2026-11-11)" "CHNb2 - Done"
awk -F'\t' '$7 ~ /CHNb/' june2017_re/char_ara/ch0_classn.tsv
```

## Task

Check CHNb against June from the exe bytes (Ghidra read-only). Spot-check at least:
- **Guide handler** `X2LVUpEventMgr.cpp` `UICustomEventProc` = June `327127a0` (the switch is a byte map `32712e0c` + table `32712dc0`, `wParam - 1500`): every
  case list, which `LEVEL_UP_EVENT_ID` each close uses (`32710a70( n )` + `32710ad0`), the return values (1540-1550 return 1, the rest 0), the cash-link block
  (`323a9780( 0.0 )` on `g_pTFieldGame`, menu type `+0x31c` = wParam, now-state vslot `0xac`), `32344ab0( 2, 3..6 )` / `( 9, 0 )`, `3222a4f0( 1..4 )` / `( 0 )`.
  Confirm 1551 / 1552 (the class skill description, `g_pStatic_ClassSkillDescription`) have no sender in March or the staged guide dialogs (the Done note routes them).
- **Enums** `X2LVUpEventMgr.h`: the 24 `LUEMCUM_*` and `LUEI_SECRET_DUNGEON_OPEN_GUIDE` values = June's `Enum.src.lua`; data036 `Enum.lua` gained the same 24.
- **Cash shop** `CX2CashShop::OpenClassChangeGuideItem` = June `3222a4f0`: menu type 0x1d, tier from the unit templet (`+0xaf0`), `3234b390( type, tier + 1, line )`,
  the map at `3379fcf8` (the 74 class -> item pairs written by `32230020`; re-read them from the disassembly), 288319 for line 0, the non-menu state list.
- **Class table** `GetUnitClassByTierAndLine` (`X2Unit.h`) = the CHNa-reviewed `ClassAt` rows (June `3234afb0`), now with `UC_*` names; `X2SkillNoteManager.cpp`'s
  `ClassAt` forwards to it.
- **Data** `LevelUpEvent.lua` (data036, core list): level 15 -> `DLG_1ST_Class_Change_Guide_Ara.lua`, level 35 { 28, 31, 34 } -> `DLG_2ND_Class_Change_Guide_Ara.lua`
  = June `LevelUpEvent.src.lua`; the level-70 transcendence row stays CH3d's. `luabind.py check` on the four dialogs: PASS.
- **CHNb2:** `32713c10` events 2 / 4 / 6 against the port (wrap, line picture, the `+0x27c` button rebinding to 1541 / 1545 + line - 1, name, skill / command lists).

Hygiene: `flagcheck.py SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE --range <CHNb commit>^..<CHNb commit>`, `junediverge.py --check-all`, `enumscan.py --check` on the changed
lines, `luasrc.py ledger --check`. Read the user's play-test logs (`python tools/logs.py --tag ADMIN_CHEAT`, `--grep "HUD shop open-at-item"`, the class-change lines).
Fix clear errors in place (bytepatch only). Write "CHNb - Review" into the plan doc, update the STATUS row, check `kickoffs/char-ara/CH2d.md` and `CH6.md`,
commit with explicit pathspecs.
