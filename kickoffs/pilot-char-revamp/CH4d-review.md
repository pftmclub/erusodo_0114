# June character Aisha (Stage 5 pilot) - Phase CH4d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH4d review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | Phase CH4d done (its Done note and commit) and its play-test |
| Flags | `SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE`, `SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE_DEBUG` (temporary) |
| Log tag | `[JUNE_CHAR]` (`ARME MEMO`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH4 split (2026-10-22, Opus)" "CH4d - Done (2026-10-22, Opus 5.5)"
grep -a "Char CH4d" june2017_re/funcmap.tsv
grep -a "^| 400 \|^| 401 \|^| 402 " JUNE_BUGS.md
```

The code: the `SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE` blocks in `X2Lib/X2Game.{h,cpp}` (the function block after `UpdateElswordSkillSlotEffect`, the
constructor / destructor / `OnFrameMove` / `UpdateSkillCoolTime` hooks), `X2Lib/X2Main.{h,cpp}` (`JuneCanShowClassGuide`, end of the `.cpp`),
`X2Lib/X2UISkillTreeNew2015.{h,cpp}`, `X2Lib/X2GUArme_VioletMage.{h,cpp}`, `X2Lib/X2SkillTree.cpp`, `X2Lib/X2KeyPad.cpp`, `KTDXLIB/KTDGUISlot.h`.

## Task

Review CH4d's commit against June, in the listing (`python june2017_re/disasm_range.py`; most helpers are carved in Ghidra):
1. **`322deb80`:** the unit-type and empty tests, `322cf350( true / false )`, the TypeA / TypeB names (option `+0x278`), `3288fff0`'s A-then-B order,
   the `break` on a miss (JUNE_BUGS 402), the type-5 slot data and its leak (401), the entry-0 hot-key string (game action `0x27` = `GAMEACTION_SIT`).
2. **`322f4740`:** the loop (`EBP` moves to the bar index, `[esp+0x18]` keeps i), the B-slot `break` (`32890ee0( 0 )`), vslots `0x718` / `0x880`, the
   bar-visible rule (`+0x116` / `+0x115` / bSlotB), the position (half size, dialog pos, `+/- 47`), the 0.1 move threshold, the 2.8 s reset (`>=`).
3. **`322eca50` / `322ecb60` / `322f4480` / `322cf790` / `322cf810`:** the slot-data lookup with the bar passed explicitly (March's `GetEquippedSlotData`
   does not map 5-9), the five sequences and `SetOverUI`, the insert after the old entry's clear; and in `322cf810` the `31d71810( 3 )` call, the
   vslot `0x714` stub (`331a4950`), the dialog manager's `m_bShow` (`+0x34`).
4. **`31d71810`:** every test against the raw disassembly (Ghidra's decompile of it is wrong); the UI manager's `+0x89` = `m_bShowSkillSlot`.
5. **Tree UI:** `328d9ac0` / `328e0f10` (`+0x340` = `m_bDragable`), `328e0460` case 2 in both branches, 29059 (`328ec719`; every path returns true via
   `328eb041`; the dropped write, JUNE_BUGS 400), `328e9160 @328e9710`, the `SwapSkillSlot` refresh (`328e0ff0 @328e1097`).
6. **Callers:** `32cd1830 @32cd204d` (timer 2.8), `3220f4dc`, `31f5ea00` case `0xcf0c`; the constructor (`@32307bb4`, `@32307e99`) and destructor
   (`@32308941`, `@32308a9b`) sites.
7. Confirm `328df9f0` has no caller, so `32ce1650` stays unported.
8. **Play-test fix 1** (`SERV_IRUHADEV_JUNE_STATE_NO_SIT_KEY`): confirm `324bc8e0` and `324d2e20` read no action `0x27`, and that nothing else in June
   sits the unit on V in a dungeon or battlefield (unit emotion code, `CX2Game` key process `322f7170`).

Then retire `SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE_DEBUG` (comment out its define with `bytepatch.py`; the code stays), run flagcheck over the CH4d
commit, build and deploy, and write the CH4d review note. Next: `CH6` (Sonnet 5.5 medium) once every CH3b / CH4a-CH4d / CH5 review is done.
