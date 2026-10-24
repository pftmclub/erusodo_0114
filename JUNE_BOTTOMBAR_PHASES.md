# June bottom bar: the menu button row, the EXP bar and the stamina bar

Plan key `june-bottombar` · branch `main` · created 2026-10-24 (user: "port the bottom area buttons and the exp + stamina bar from June now;
buttons whose feature is not in the project yet go to Stage 9") · log tag `[JUNE_BOTTOMBAR]` · flags `SERV_IRUHADEV_JUNE_BOTTOMBAR_*` ·
kickoffs `kickoffs/june-bottombar/` · backlog Stage 32, carved out of Stage 9.

## Context

**What the village HUD is today.** `CX2StateMenu` owns one KTDGUI dialog, `m_pDLGMenu`, made by `CX2MemoryHolder::GetMenuDLG`
(`X2MemoryHolder.cpp:388-415`), which picks one of `DLG_Menu_Button_NEW2 / NEW / Pet / .lua / New1` by compile flags. In the live
data034 `DLG_Menu_Button_NEW.lua` is **March bytecode** (14,551 B, `1B 4B 4C 81`); it is not on the core list and has no ledger row.
`RefreshMenuInfo` (`X2StateMenu.cpp:3718`) fills the stamina gauge (`StaticMenu_Button_Tired_Gage`, picture 0 width from `m_iSpirit` /
`m_iSpiritMax`; tooltip on `Button_Invisible_SpiritGuage`) and calls `UpdateExpGageBar` (`:3770-3930`, controls
`StaticMenu_Button_EXP_Gauge` / `StaticMenu_Button_EXP`). So the data paths for EXP and stamina already exist; the pictures and layout are March's.

**What June draws** (`june_src_fixed/data034_kom/DLG_Menu_Button_NEW.src.lua`, 31 KB; read-only evidence, [D] = seen in the file):

| Piece | June | Notes |
|---|---|---|
| Backplates | `DLG_UI_Image_01.TGA` `Gauge_EXP` (0, 743), `Gauge_Stamina` (316, 743), `Bra_Notice` (552, 743, +459 wide) | [D] March's live `DLG_UI_Image_01` is data215 (`x1_short51.tsv:5`); whether it has these keys is B0's check |
| EXP gauges | three statics: `StaticMenu_Button_EXP_Gauge` (`EXP_Y`), `StaticMenu_Button_RESET_EXP_Gauge` (`EXP_G`), `StaticMenu_Button_Used_RESET_EXP_Gauge` (`EXP_R`), from `DLG_UI_Common_Texture64_NEW.TGA` | [D] normal / rest / used-rest EXP. `Texture64_NEW` is data057; June's `.TET` is 6,278 B, March's 6,100 B (`x2_march_native_diff.tsv:2162`) |
| Stamina gauge | `StaticMenu_Button_Tired_Gage`, picture `EXP_G` at (401, 759), +127 wide, "100%" string right-aligned at x 385 | [D] same control name March already updates |
| Gauge toggle | two opacity buttons: `SMUCM_CHANGE_TO_REST_GAGE` / `SMUCM_CHANGE_TO_EXP_GAGE` | [D] switch which EXP gauge shows |
| Button row | 18 `STATE_MENU_UI_CUSTOM_MSG` ids (below) | [D] |
| Layout code | bar layout `3245bb70`, placement `3245cba0`, `Story_New` mark `324708c0`, state-menu dispatcher `32476050`, StoryBook handler `32478e90` | from the backlog Stage 9 notes; B0 identifies the rest |

**The 18 ids June's dialog sends.** March's `X2StateMenu.h` already has 13: `MY_INFO`, `INVENTORY`, `MY_INFO_SKILL_TREE`, `MY_INFO_QUEST`, `GUILD`,
`COMMUNITY`, `SYSTEM`, `CASH_SHOP`, `PET_LIST`, `WARP_BUTTON`, `OPEN_ELLIOS_GUIDE`, `SHOW_ADVERTISEMENT`, `FRIEND_RECOMMEND_OPEN`.
**Missing in March: 5** - `PROFESSION_LIST`, `STORY_BOOK`, `EVENT_2016_VALENTINE_UI`, `CHANGE_TO_REST_GAGE`, `CHANGE_TO_EXP_GAGE`. Whether the 13 behave
as June's handlers do (a button can exist in both and still differ) is B0's table.

## Scope fence

**In this plan:** the village menu dialog as June draws it, and what makes it work:
- adopting June's `DLG_Menu_Button_NEW.lua` and the textures/keys it needs, byte-exact (the same-name rule), or a core-list row if March's C++ needs both;
- the 5 missing ids, the EXP bar (normal, rest, used-rest, toggle, text, tooltips) and the stamina bar (layout, text, tooltip);
- every one of the 13 existing buttons: June's handler body, whole ("no partial updates").

**Not in this plan (Stage 9, never dropped):**
- a button whose **feature does not exist in the project** is drawn like June's, enabled-looking, and its click only logs
  `[JUNE_BOTTOMBAR] STUB <id> -> Stage 9`; it opens nothing. The final list is B0's, the starting list is
  `PROFESSION_LIST` (profession UI), `EVENT_2016_VALENTINE_UI` (2016 event UI), and `STORY_BOOK` (Stage 9 / quest-system Q10c owns `CX2UIStoryBook`);
- rest-EXP **earning** (if the offline save has none, the rest/used-rest bars stay at June's absent-value default, empty, and the toggle still works);
- the other `DLG_Menu_Button_*` variants (pet, warp, NEW2/NEW3), the minimap, the quick slot, the event-icon bar, the notice strip text;
- the HUD portrait / `CX2MyGageUI` (in-dungeon gauges).

A feature that is *missing* is a stub; a feature that is *present but behaves differently from June's handler* is in this plan.

## Rules

- Branch `main`; one flag family `SERV_IRUHADEV_JUNE_BOTTOMBAR_*`, defined in `KTDXLIB/Always.h`; original code in `#else`.
- June's files go in byte-exact through `luasrc.py adopt-june` / `kom.py stage-june` (rule: same name = June's). A March reader that then breaks is fixed in
  code, never by editing June's file. Core-list `.lua` only when B0 proves March's C++ and June's file disagree, with the row added by the phase.
- Comments are one short line. Keep June quirks verbatim (`JUNE_BUGS.md` row). Dev-console and `/additem` rules in CLAUDE.md apply to hand-offs.
- Pack and copy of koms: Claude packs; **ask before each `/x2-kom-packed copy`**.

## Starting context

Tools only: `python tools/build.py --deploy` (background), `flagcheck.py`, `bytepatch.py` (X2Lib sources are CP949), `docsection.py`, `logs.py --tag JUNE_BOTTOMBAR`,
`luasrc.py adopt-june`, `kom.py`. June reading: `june_src_fixed/data034_kom/DLG_Menu_Button_NEW.src.lua`; Ghidra for the code addresses above.
Funcmap rows are tagged `(Bottombar B<n>)`; the button table is `june2017_re/bottombar_buttons.tsv`.

## Phase B0 - Identify, read-only (Opus 5.5, high)

**Run with:** `/x2-phase june-bottombar B0`. No code, no build.
**Task:**
1. Decompile June's `32476050` cases for the 18 ids, `3245bb70`, `3245cba0`, `324708c0`, and June's `UpdateExpGageBar` and stamina update counterparts
   (find them from the control names `StaticMenu_Button_EXP_Gauge`, `_RESET_`, `_Used_RESET_`, `_Tired_Gage`).
2. For each of the 18 buttons write one row in `june2017_re/bottombar_buttons.tsv`: id, June address, what June does, March counterpart,
   verdict `same` / `differs` (name the difference) / `missing-feature` (-> Stage 9) / `missing-id-only` (feature exists, id does not).
3. For the EXP bar: what each of the three gauges is computed from, whether rest EXP exists in the offline save or emulator, what the toggle changes.
4. Asset census: do `Gauge_EXP`, `Gauge_Stamina`, `Bra_Notice`, `EXP_Y`, `EXP_G`, `EXP_R` exist in the **live** `DLG_UI_Image_01` / `Texture64_NEW` `.TET`s;
   which June files the dialog loads that March lacks; which of March's C++ control lookups June's file no longer has (and the reverse).
5. Decide `adopt-june` default (ljconv bytecode) vs core-list source, with the evidence; fix the B1-B3 file lists.
6. Update the backlog Stage 9 bullet with the final stub list. Write the B0 Done note (<= 60 lines) and funcmap rows.
**Gate:** `bottombar_buttons.tsv` has 18 rows with a verdict each; nothing marked "unknown" without a named reason.

## Phase B1 - June's dialog, the 5 ids, stubs (Sonnet 5.5, high)

**Depends on:** B0. **Flag:** `SERV_IRUHADEV_JUNE_BOTTOMBAR_DIALOG`.
**Task:** per B0: stage the textures/`.TET`s and `DLG_Menu_Button_NEW.lua` (replace, byte-exact; backup is the tool's), add the 5 ids to `STATE_MENU_UI_CUSTOM_MSG`
(appended, behind the flag), make `GetMenuDLG` load it for the village, add the `STUB` log cases for the `missing-feature` rows, and pack the koms (`/elstools pack`; ask before copy).
**Gate:** village shows June's bar with all 18 buttons; every stub click logs its `STUB` line; no `UNHANDLED`/`EXCEPTION`.

## Phase B2 - EXP bar (Sonnet 5.5, high)

**Depends on:** B1. **Flag:** `SERV_IRUHADEV_JUNE_BOTTOMBAR_EXP`.
**Task:** June's EXP update body, whole: the three gauges, their strings and tooltips, the rest/exp toggle; max level shows a full bar (keep
`SERV_IRUHADEV_EXP_GAUGE_MAX_LEVEL_FIX`'s behaviour or June's, B0 decides which June does).
**Gate:** level-up and EXP gain update the bar; toggle swaps gauges; log `[JUNE_BOTTOMBAR] EXP`.

## Phase B3 - Stamina bar and the 13 working buttons (Sonnet 5.5, high)

**Depends on:** B2. **Flag:** `SERV_IRUHADEV_JUNE_BOTTOMBAR_BUTTONS` (+ stamina under `_STAMINA`).
**Task:** June's stamina update (width, percentage string, tooltip); then every `differs` row of B0's table ported whole.
**Gate:** each of the 13 buttons opens what June's opens; the stamina bar matches `m_iSpirit` through a level/dungeon change.

## Review phase (Opus 5.5, high)

`/x2-phase june-bottombar B3 review` covers B1-B3 together: June vs ours line by line, flagcheck, deployed exe, play-test log lines.

## Phase Z - Close-out (Sonnet 5.5, medium)

`MODS.md` revert lines, backlog Stage 32 closing note, the Stage 9 stub bullet confirmed against the final B0 table, STATUS, memory. No code.
