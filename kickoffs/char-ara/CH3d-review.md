# June character Ara (Stage 7) - Phase CH3d review ( the CH3 skill review: covers CH3a + CH3a2 + CH3b + CH3c + CH3c2 + CH3d )

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH3d review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH3d done (its Done note, commit and the user's play-test + "check logs"); the CH3a-CH3c2 play-tests are already in their Done notes |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` and `..._SKILL_DEBUG` (the debug define retires here: `//#define` in `Always.h`), `SERV_IRUHADEV_JUNE_CHAR_ARA_TRANSCEND` (CH3d) |
| Log tag | `[JUNE_CHAR]` (`ARA SKILL equip skill` / `state <n> skill <id>`), `[JUNE_TRANSCEND]` (`IMAGE`, `UNIT`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH3a - Done (2026-11-12)" "CH3a2 - Done (2026-11-12)" "CH3b - Done (2026-11-12)" "CH3c - Done (2026-11-12)" "CH3c2 - Done (2026-11-12)" "CH3d - Done (2026-11-12)"
grep -n "Ara: CH3" JUNE2017_CHARACTER_PORT_CHECKLIST.md
grep -n "Char Ara CH3" june2017_re/funcmap.tsv
```
Plus `MODS.md` "Stage 7 Ara, CH3a" .. "CH3d" (the damage rows), and `june2017_re/char_ara/ch0_skill_listing_rows.tsv` for the second switch ( `32bd34c0` ). If a Done heading above differs ( `--list` the outline ), pull the right one.

## Task

Check every CH3 sub-phase's code against June from the listings ( `python june2017_re/charport_adis.py ara --phase CH3a > <scratch>`, the same for `CH3a2`, `CH3b`, `CH3c`, `CH3c2`, `CH3d`; a listing runs on into the next function: stop at the `int3`; Ghidra read-only for a spot-check). For each sub-phase spot-check at least three written bodies, its cases in the first id switch ( `32bd09d0` ) and the second ( `32bd34c0` ), and two of its "= March" funcmap claims. Pick the ones its Done note calls out ( a stand-in, a quirk kept, a branch not ported, a JUNE_BUGS row ).
- **CH3d ( hyper actives, transcendence rows ): 8 class-map functions, 4 = March ( Thousands Blossoms Init / EventProcess, Energy Cannon Init / EventProcess, and the Energy Cannon helper `32baab30` = March's CreateState ), 3 new ( Nine-Tailed Fox CreateState `32baace0`, empty Init `32b95290`, EventProcess `32ba3b80` ).** Check: the hyper split ( `@32bd0bb6` `cmp 0x6bd0`, jump table `32bd2a24` = 26d8 / 26fb / 2685 / 26d8 / 26fb ) = the three cases added to March's switch; Thousands Blossoms inline `@32bd2685` ( table `@33507168`, Init / EventProcess, hyper copies ) = March's case body; `32ba3b80` = Ara's cancel macros ( skill + awakening, walk, back walk, dash, ZX, all on a line ) and the 45 fall; 27600+ has no case in `32bd34c0` ( range 27001-27242 ).
- **CH3d shared files ( verdict line each ):** `X2SkillTree.h` ( 6 `RNSI_HA_*` names 27600-27605, `JUNE_ENUM_NAMES` ); `X2Data.cpp` ( prefixes `S_DEVANAM` / `Y_RAJA` / `ASURA`, the Texture101 branch for 160by160 / gray; re-read one push per image type from the static initializer `33374620`, e.g. `@333777b2` `TR_S_DEVANAM_LARGE` -> `Texture101_NEW` ); `Handlers_Quest.cpp` ( the brace-less guard after Rena's ); `Always.h` ( `_ARA_TRANSCEND` block, its guard on `_ARA_LIVE` and `_RENA_TRANSCEND` ); `LevelUpEvent.lua` TYPE6 against `june_src_fixed/data036_kom/LevelUpEvent.src.lua:566-584`.
- **Templet swap.** Every term in the new code that reads an Ara skill id reads it through the swap ( `GetEquippedActiveSkillTemplet( id, true )`, `GetNowStateSkillID()`, `GetFutureStateSkillID()` ), never a base id alone.
- **Play-tests.** CH3a-CH3c2 from their Done notes; CH3d's from "CH3d - Done" ( `equip skill 27600 / 27601 / 27602 slot 4 bank A`, `27064 / 27065 / 27066 slot 4 bank B`, the hyper states entered, `[JUNE_TRANSCEND] IMAGE` rows for 118 / 119 / 124 with `Texture101_NEW` / `_GRAY`, 0 Motion Error / KEM_ERROR / EXCEPTION ).

Hygiene: `flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL --with SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL_DEBUG --range 9744b93a^..<CH3d code commit>` ( 9744b93a = CH3a's code commit; explain every OTHER row, e.g. `X2SkillTree.h` = `JUNE_ENUM_NAMES`, and every deleted line ), plus `SERV_IRUHADEV_JUNE_CHAR_ARA_TRANSCEND --range <CH3d code commit>^..<CH3d code commit> --files KTDXLIB/Always.h X2Lib/X2Data.cpp X2Lib/Offline/Handlers_Quest.cpp`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUAra.cpp`, `skillicons.py check 27000-27999`, a one-file `/Zs` probe of `X2GUAra.cpp`.
Retire `_SKILL_DEBUG` ( `//#define` in `Always.h`, bytepatch; a build whose only change is a commented `_DEBUG` define starts but is not awaited ).
Fix clear errors in place (bytepatch only). Write "CH3d - Review" in the plan doc ( the CH3a-CH3c2 findings in it ), update the STATUS rows ( CH3a-CH3d done ), and check `CH4a.md` and `CH6.md` ( CH6 walks the becoming-transcended transition on un-transcended 118 / 119 / 124 units: units 98 / 48 / 104 are transcended by CH3d's set-up, so CH6 uses or creates others ).
