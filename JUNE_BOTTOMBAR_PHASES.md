# June bottom bar: the menu button row, the EXP bar and the stamina bar

Plan key `june-bottombar` · branch `main` · created 2026-10-24, re-planned the same day on Opus (user: "port the bottom area buttons and the
exp + stamina bar from June now; buttons whose feature is not in the project yet go to Stage 9") · log tag `[JUNE_BOTTOMBAR]` · flags
`SERV_IRUHADEV_JUNE_BOTTOMBAR_*` · kickoffs `kickoffs/june-bottombar/` · backlog Stage 32, carved out of Stage 9.

## Context

Planning evidence (2026-10-24, Ghidra + the June / live kom files, read-only). [D] = seen in the listing or file, [I] = inferred, B0 confirms.
Per-button rows: `june2017_re/bottombar_buttons.tsv`.

**One dialog, both sides.** June's `CX2MemoryHolder::GetMenuDLG` (`32853880`) builds `DLG_Menu_Button_NEW.lua` unconditionally [D]; March's
(`X2MemoryHolder.cpp:388-415`) reaches the same name because `SERV_RECOMMEND_LIST_EVENT` is off (`JUNE_SKILL_SYSTEM_PHASES.md:1460`). Only the file's
content differs: live data034 has March's bytecode (14,551 B, `1B 4B 4C 81`, no ledger row, not on the core list).

**June's file** (`june_src_fixed/data034_kom/DLG_Menu_Button_NEW.src.lua`, June's own bytes in data034) [D]:
- backplates from `DLG_UI_Image_01`: `Gauge_EXP` (0, 743), `Gauge_Stamina` (316, 743), `Bra_Notice` (552, 743, +459 wide);
- EXP: `StaticMenu_Button_EXP_Gauge` (`EXP_Y`), `StaticMenu_Button_RESET_EXP_Gauge` (`EXP_G`), `StaticMenu_Button_Used_RESET_EXP_Gauge` (`EXP_R`), all at
  (86, 759), from `DLG_UI_Common_Texture64_NEW`; stamina: `StaticMenu_Button_Tired_Gage` (`EXP_G`, (401, 759), +127 wide, "100%" string at x 385);
- 15 row buttons on `DLG_UI_Button27` placed by the file's own `MBW_GET_SLOT_LEFT_TOP( slot, dx, dy )` (x = 987 - 30 * ( slot - 1 ), y = 718), slots
  14..1: Warp, Guide, Charac, Inven, Profession, Pet, Skill, StoryBook, Quest, Guild, Comm, System, Event (`DLG_UI_Button19_NEW`), Cash; plus
  `2016Valentine` and `FriendRecommed` (both `SetShow( false )`); badges (`*_New`, `Warp_VIP`, `Pat_Inventory_*`, tags), the F5 / F6 / F1 signs, and three
  invisible tooltip buttons over the gauges (`Button_Invisible_ExpGuage` -> `SMUCM_CHANGE_TO_REST_GAGE`, `_RestExpGuage` -> `_CHANGE_TO_EXP_GAGE`,
  `_SpiritGuage`);
- last line `IncludeLua( "DLG_UI_Event_Button_INT.lua" )`: a June-only file (datai014) of event icons and an event-EXP gauge, every control
  `SetShowEnable( false, false )`. It names a second control `"Event"` (a June quirk: `GetControl` finds the first, the advertisement button).
- Every method it calls is bound in March, every `STR_ID_` it uses is in the live `StringID_def.lua`, and every control March's `X2StateMenu.cpp` reads that
  June's file lacks (`StaticMenu_Button_EXP`, `StaticMenu_Button_LV_Num`, `ButtonBack`, `Button_TooniLand`, `ElesisEventButton`) is NULL-checked there.

**The ids.** The dialog sends 18 `STATE_MENU_UI_CUSTOM_MSG` names, resolved through `Enum.lua` (data036, **core list**, BOM source). The live table has 13
(same values as June except `WARP_BUTTON` 178 / June 1001 and `SHOW_ADVERTISEMENT` 177 / June 1000; March's C++ uses March's). Missing: `PROFESSION_LIST`
(June 178, taken by March's warp), `STORY_BOOK` 181, `EVENT_2016_VALENTINE_UI` 1035, `CHANGE_TO_REST_GAGE` 195, `CHANGE_TO_EXP_GAGE` 196. Precedent
`SERV_IRUHADEV_JUNE_SKILL_SLOT_UI`: new members sit after March's last (`X2StateMenu.h:182-186`, 208 / 209), so these become **210-214** in both files.
A missing name reaches `SetCustomMsgMouseUp` as nil (sent as 0) without an error: today's symptom would be a dead button, not a broken dialog.

**June's code** [D] (all `CX2StateMenu`, vtable `334978f4`):

| June | Role | March counterpart |
|---|---|---|
| `32476050` | `UICustomEventProc`; the 18 cases in the tsv | `X2StateMenu.cpp:1002` |
| `32475480` (vslot `+0x288`, Ghidra has no function there; `@32475480-324756d0`) | `RefreshMenuInfo`: `32473810`, `3245d870`, `3245da70`; stamina = `+0x950 / +0x954` (`m_iSpirit` / `m_iSpiritMax`) * width **and** "%.1f%%" into the gauge's string 0; `_SpiritGuage` guide text written **empty** (`@324755ca-3247562f`, [I] June quirk); inventory ED / AP | `:3718` (width only; tooltip "%.1f%%") |
| `32473810` | EXP bar: rest mode (`this +0x468`) -> `32470210`; else resonance (`3247b460`, unit `+0xa99`) -> resonance EXP ("#C00FFFF%.1f%%"); else `( EXP - base ) / 32741030( level + 1 )`, clamp, width, "%.1f%%"; then (not rest mode) a tooltip on a control whose name the decompile lost | `UpdateExpGageBar` `:3762-3933` (+ level number, `SERV_IRUHADEV_EXP_GAUGE_MAX_LEVEL_FIX`) |
| `324700a0` / `32470210` | rest mode on / off (shows `_RESET_`, `_Used_RESET_`, `_RestExpGuage`); rest gauge from unit `+0xac0` / `+0xac8` (int64, June-only) over a per-level table, level < 99 | none |
| `3245bb70` | row layout: vector `this +0x3d8` of the 15 buttons, visible ones packed, badges follow; `AccompanyPetTag`, `Warp_New` (decompile breaks after `@3245bf00`) | `UpdateMenuPosistion` `:5206-5453` (13 buttons, March order) |
| `3245cba0` | badge placement (`Normal`, `*_New`, `Warp_VIP`, `Story_New`) | inside `UpdateMenuPosistion` [I] |
| `32472a40` (vslot `+0x27c`) | calls `3245cba0`, `3245bb70`, `32468c70` | caller `:3164` of `UpdateMenuPosistion` [I] |
| `32468c70` | button show / hide set + event-EXP static; also called by `31f5ff50` and `324700a0` | B0 |
| `3245d870` / `3245da70` | event-EXP gauge (`... (DEV) %f` strings) / a DEV tooltip | none [I] |
| `324708c0` (vslot 1) | frame move: calls `3245bb70` `@324714ed`, badge / sign / event-icon visibility, `Story_New` from `+0xabd` | `OnFrameMove` `:221` (calls `UpdateMenuPosistion` at `:617`) |

**Assets** (`kom.py stage-june --dry-run`, both files' textures). Same name -> June's, byte-exact (standing rule):
- replaced: `DLG_UI_Button19_NEW` (data056), `DLG_UI_Common_Texture64_NEW` (data057), `DLG_UI_Button_INT01`, `DLG_INT_Button01` (datai020);
- new: `DLG_UI_Button27` (data056), `DLG_UI_EVENT_2016VALENTINE_00` (datai020), `DLG_UI_EVENT_Friends_Invitation01`, `DLG_UI_EVENT_2016PETFULFILL_01`,
  `DLG_UI_EVENT_ElMission_01`, `DLG_UI_Event_INT01` (datai022), `DLG_INT_EVENT_PHOTOBOOK_03`, `DLG_UI_EVENT_CnSpring2016_01` (datai021), `DLG_UI_Text_03`
  (data151); each `.tga` + `.TET`; already June's: `DLG_UI_Image_01` (data215), `DLG_Common_New_Texture03` (data053);
- `.TET` keys: the four replacements only add keys, except `Texture64_NEW` drops `EXP` (adds `EXP_Y` / `_G` / `_R` / `_B`). A bytecode-constant scan of
  every live `.lua` naming `Texture64_NEW` finds `EXP` only in `DLG_Menu_Button_NEW.lua` (replaced here) and `DLG_Menu_Button_NEW2.lua` (not loaded); no C++
  reader. June's own include still asks for `EXP` (June quirk: that hidden picture draws nothing in June too).

## Scope fence

**In this plan** (the village / field bottom bar as June draws it):
- June's `DLG_Menu_Button_NEW.lua` and `DLG_UI_Event_Button_INT.lua` and the textures above, byte-exact;
- the 5 ids (210-214) in `Enum.lua` and `X2StateMenu.h`;
- June's row layout and badges (`3245bb70`, `3245cba0`, `32472a40`, `32468c70`) and the bar lines of the frame move that drive them;
- the EXP bar and the stamina bar (`32473810`, `32475480`);
- June's handler body for each existing button whose June case differs from March's (whole body, "no partial updates").

**Stays Stage 9, drawn but inert** (user, 2026-10-24: "draw, click logs"): the button is placed exactly as June places it; its click logs
`[JUNE_BOTTOMBAR] STUB <name> -> Stage 9` and opens nothing:
- `PROFESSION_LIST`: June's profession (life-job) system: `CX2ProfessionManager`, GFx `UIProfession` (`CX2UIProfessionAdapter`), menu 0x22,
  `DLG_UI_Job_professional.lua`, `DLG_UI_Change_Profession.lua`, the blacksmith / alchemist / treasure-hunt jobs. A whole system, not a dialog;
- `STORY_BOOK`: `CX2UIStoryBook` (quest-system Q10c, already Stage 9). The stub does **not** clear `+0xabd`; that is part of June's handler;
- `CHANGE_TO_REST_GAGE` / `CHANGE_TO_EXP_GAGE`: rest EXP (unit `+0xac0` / `+0xac8`, June-only `KUnitInfo` data the emulator never sends) and its gauge `32470210`;
- `EVENT_2016_VALENTINE_UI`, and whichever of `SHOW_ADVERTISEMENT` / `FRIEND_RECOMMEND_OPEN` B0 finds needs a missing system.

**Not in this plan at all:** the event icons and event-EXP gauge of the include (Stage 9, `EventButtonAddSystem`); the resonance EXP branch (Stage 30; March
answers "no resonance" as `X2UIDungeonResultAdapter.cpp:467` does); the notice strip's text (Stage 9, R4h); the rest of the frame move; the other
`DLG_Menu_Button_*` variants; quick slot, minimap, the in-dungeon HUD (`CX2MyGageUI`).

## Rules

- Branch `main`. One flag per logical change in `KTDXLIB/Always.h`, original code in `#else`; `X2StateMenu.cpp` / `.h` and `X2MemoryHolder.cpp` are CP949:
  `bytepatch.py` only. One-line comments.
- June files go in through `luasrc.py adopt-june` (Lua, default ljconv bytecode) and `kom.py stage-june` (textures); never edited. `Enum.lua` is core-list BOM
  source: edit the text, then `luasrc.py refresh`; `ledger --check` stays PASS.
- Port June's UI-manager menu ids **by name** (June `0x13` is `UI_MENU_CHARINFO`, `0x1f` inventory ...), never by number; the numbering differs.
- June quirks are kept verbatim with a `JUNE_BUGS.md` row (the empty spirit tooltip, the duplicate `"Event"`, the missing `EXP` key).
- Pack with `/elstools pack`; **ask before every `/x2-kom-packed copy`**. Hand-offs: dev console, `/additem`, copy-ready (CLAUDE.md).

## Starting context

Tools only: `python tools/build.py --deploy` (background), `flagcheck.py`, `bytepatch.py`, `docsection.py`, `logs.py --tag JUNE_BOTTOMBAR`, `luasrc.py`,
`kom.py`, `elsdb.py` (save edits for play-tests). June: Ghidra (`.claude/skills/ghidra`; `32475480` has no Ghidra function: read its disassembly or
create the function), `june_src_fixed/`. Funcmap rows are tagged `(Bottombar B<n>)`.

## Phase B0 - Identify the layout and the button bodies, read-only (Opus 5.5, high)

**Run with:** `/x2-phase june-bottombar B0`. No code, no build. Delegate pulls to Sonnet subagents; keep the readings.
1. `3245bb70` from the disassembly (the decompile breaks after `@3245bf00`): slot order, what hides a button (the gates on profession, pet, guild, warp,
   friend, event), how visible buttons pack, badge offsets, `AccompanyPetTag` / `Warp_New`. Diff against `UpdateMenuPosistion` `:5206-5453`.
2. `3245cba0`, `32472a40`, `32468c70` (and its callers `31f5ff50`, `324700a0`): March counterparts (`:3164`'s function, `ShowMainMenuButton` `:5142`,
   `UpdateMenuString` `:5151`, `SetShowMenu` `:3546`); which March function each one replaces.
3. The frame move `324708c0`: only the lines that decide `3245bb70`'s call (`@324714ed`), the badges, the F5 / F6 / F1 signs and `Story_New`; diff against
   `OnFrameMove` around `:617`. Everything else in it is out of scope: list it once in the Done note, owners named.
4. The 18 cases (`bottombar_buttons.tsv`): map each June menu id to March's `UI_MENU_*` by what `32158070` creates for it; compare body by body; set `plan_verdict`
   to `same` / `differs (<what>)` / `STUB` and `owner`. Settle `SHOW_ADVERTISEMENT` and `FRIEND_RECOMMEND_OPEN` (is the system there offline?).
5. `32473810`: the control the tooltip step writes (the decompile lost the name; disassemble the call before `31f54a10`); the level-cap answer
   (`32741030` returns 0 for a missing row, `JUNE_DUNGEON_RESULT_PHASES.md` R1 / R3b) against `SERV_IRUHADEV_EXP_GAUGE_MAX_LEVEL_FIX`; `3245d870` / `3245da70` (in or out).
   Confirm the empty `_SpiritGuage` text in `32475480`.
6. Write `june2017_re/bottombar_functions.tsv` (June function -> March function -> phase -> verdict), funcmap rows, the B0 Done note (<= 60 lines), and the
   final stub list into the backlog Stage 9 bullet. Fix B1-B4's file and function lists if the readings move them.
**Gate:** every tsv row has a verdict and an owner; every June function in the table above has a March counterpart or "none (why)".

### B0 - Done (2026-10-24, Opus 5.5)

Read-only; no code, no flag, no build. Evidence: `june2017_re/bottombar_functions.tsv` (one row per June function), `bottombar_buttons.tsv` (verdict + owner
on all 18 rows), funcmap rows `(Bottombar B0)`. March was read from a `cl /P` live-lines copy of `X2StateMenu.cpp` (US_SERVICE), not from the `#ifdef`s.
**Gate: PASS** (18/18 rows have verdict + owner; every function in the Context table has a counterpart or "none (why)").

**Counterparts.** `3245bb70` = `UpdateMenuPosistion`; `32472a40` = `InitUI`; `32468c70` = `UpdateMenuString` (its caller `31f5ff50` = the
`X2KeyPad.cpp:2141` key refresh, untouched); `32473810` = `UpdateExpGageBar`; `32475480` = `RefreshMenuInfo`; `3245a850` = `OnClickedMenuSkillButton` (same).
New in June: `3245cba0` (badge placement). None in March, Stage 9: `324700a0` / `32470210` (rest EXP), `3245d870` / `3245da70` (include event gauge / icon).
`ShowMainMenuButton` and `SetShowMenu` have no June counterpart in this set and stay March's.

**Layout (`3245bb70`).** The same rules as March in vector form, plus: the row holds 15 buttons in June's order; a hidden button shifts everything left of
it by **32** while the dialog spaces slots **30** apart (June quirk, 2 px overlap: JUNE_BUGS row in B2); Profession hides below level 40; StoryBook hides (and
the row closes up) only under code event 2316; Pet's no-pet hide is skipped under code event 2220; Guild also hides on `this +0x43d` (a June-only Lua
binder sets it; reads 0 here); Skill also hides in June's PVE states 0x17 / 0x18 (no March state). Badges move with their button by a slot jump table.
`3245cba0` places the badges once at init from each button's `Normal` picture: `*_New` ( x + 5, y - 17 ), `Warp_VIP` ( x, y - 12 ), informer anchors ( x + 7, y - 10 ).

**Absent-value defaults (gates on missing systems).** June's code events 2155 / 2206 / 2218 / 2220 / 2316 (and the tags' 2040 / 2140 / 2199) are absent
from March's `CODE_EVENT_ID` (ends at 50): `IsEnableCode` reads false, so Pet hides with no pets, StoryBook stays, FriendRecommed / Valentine stay hidden.
States 0x17 / 0x18 never occur. `+0x43d` is never written.

**Frame move (`324708c0`), bar lines.** = March: F5 / F6 signs, `Inventory_New`, `Skill_New`, `Cash_New`, `Warp_VIP` (60 s), `Pat_*`, the per-frame layout call
(`@324714ed` = `:617`) and the 0.1 s `RefreshMenuInfo` timer. New: `Story_New` (hidden in a dungeon, else `g_pInstanceData +0xabd`, never set offline);
the F1 sign's raid-field exception (unreachable). Out of scope, owners: tags `DiscountTag` / `FeverTag` / `PPoruTag` / `AccompanyPetTag`, `FriendRecommed`,
`2016Valentine`, event icons, ChallengeELMission, IceHeater, Ariel / Note: Stage 9; everything else in the frame move: unchanged, no owner needed.

**Cash button swap: stand-in (rule "design missing June pieces").** June's frame move sets the Cash button's message to 180 (popup 39402 "item mall under
maintenance") unless `g_pInstanceData +0xab4` is 1, which only a server packet sets (March's `Handler_EGS_CASH_SHOP_OPEN_NOT` is compiled out). June's
absent value would lock the offline cash shop, so B4 ports the swap with the flag reading **1** (shop open), labelled. Say so if you want June's default instead.

**Buttons.** same: Charac, Inven, Skill, Quest, Comm, Pet, Guide. differs (B4): Guild, System, Cash, Warp, Event (advertisement browser: a working system,
not a stub). STUB (Stage 9): Profession, StoryBook, Valentine, both rest-gauge buttons, FriendRecommed. June menu ids (jump table `321591a8`, index id - 1):
0x13 CHARINFO, 0x14 PET_LIST, 0x17 QUEST_NEW, 0x1f INVEN, 0x20 COMMUNITY, 0x15 / 0x16 SKILL / COMBO_TREE.

**EXP / stamina.** June's EXP body = March's formula plus a "%.1f%%" string on the gauge and `SetShow( 1 )`; the tooltip control is `Button_Invisible_ExpGuage`
(`@32473b3e`). June has no cap guard (0 / 0 at the cap), so B3 keeps `SERV_IRUHADEV_EXP_GAUGE_MAX_LEVEL_FIX` as its own `#ifdef` inside the June body (100.0%).
Stamina: "%.1f%%" string on `StaticMenu_Button_Tired_Gage`, no March rounding. Confirmed: `Button_Invisible_SpiritGuage`'s tooltip is written **empty**
(`@324755e3-3247561f`, June quirk, JUNE_BUGS row in B3).

**Strings.** `general.ess` in the game dir is already June's (md5 = June data036). `UpdateMenuString` keys are June `GAMEACTION` numbers: B2 maps them by name.

## Phase B1 - June's dialog, textures, the 5 ids, the stubs (Sonnet 5.5, high)

**Depends on:** B0. **Flag:** `SERV_IRUHADEV_JUNE_BOTTOMBAR_IDS` (+ `_DEBUG`).
- `luasrc.py adopt-june DLG_Menu_Button_NEW.lua DLG_UI_Event_Button_INT.lua`; `kom.py stage-june` the texture list (Context, *Assets*).
- `Enum.lua`: the 5 names = 210-214 in `STATE_MENU_UI_CUSTOM_MSG`, then `luasrc.py refresh`; `X2StateMenu.h`: the same members and values after 209.
- `UICustomEventProc`: a case per `STUB` row (6, B0: the 5 new ids + `SMUCM_FRIEND_RECOMMEND_OPEN` 171, whose March case is compiled out) logging
  `[JUNE_BOTTOMBAR] STUB <name> -> Stage 9` once per click.
- Pack the touched koms (data034, datai014, data036, data056, data057, data151, datai020, datai021, datai022); ask before copy.
**Gate:** the village shows June's art and buttons (positions still March's runtime layout until B2: say so in the hand-off); every stub click logs; the
load logs no `KEM_ERROR68` for the include; `ledger --check` PASS; no `UNHANDLED` / `EXCEPTION`.

### B1 - Done (2026-10-24, Sonnet 5.5)

**Landed** (commit `<B1 commit>`, flag `SERV_IRUHADEV_JUNE_BOTTOMBAR_IDS`; no `_DEBUG` flag was needed, the stub lines are the permanent log):
- `luasrc.py adopt-june`: `DLG_Menu_Button_NEW.lua` (data034, replaced, 18,530 B, June's `ljconv` bytecode) and `DLG_UI_Event_Button_INT.lua` (datai014, new, 7,534 B). Run record `june2017_re/june_adopt_20261024_102123.tsv`.
- `kom.py stage-june`, 26 files, 18 new / 8 replaced / 0 refused (matches the dry run and the Context list): data056 (`Button19_NEW` replaced, `Button27` new), data057 (`Texture64_NEW` replaced), data151 (`Text_03`), datai020 (`Button_INT01`, `INT_Button01` replaced, `2016VALENTINE_00` new), datai021 (`PHOTOBOOK_03`, `CnSpring2016_01`), datai022 (`Friends_Invitation01`, `2016PETFULFILL_01`, `ElMission_01`, `Event_INT01`). Backups: `kom_backup_stage_june61024_102123` / `_102128`.
- `Enum.lua` (data036, core list): `SMUCM_PROFESSION_LIST` 210, `STORY_BOOK` 211, `EVENT_2016_VALENTINE_UI` 212, `CHANGE_TO_REST_GAGE` 213, `CHANGE_TO_EXP_GAGE` 214 after the 209 line, then `luasrc.py refresh`. `X2StateMenu.h`: the same five members. (June's own numbers 178 / 181 / 1035 / 195 / 196 clash with March's, as B0 found.)
- `X2StateMenu.cpp` `UICustomEventProc`: six cases before `SMUCM_GUILD`, each logs `[JUNE_BOTTOMBAR] STUB <name> -> Stage 9` and returns true: the five new ids and `SMUCM_FRIEND_RECOMMEND_OPEN` (under `#ifndef SERV_RECOMMEND_LIST_EVENT`, so it cannot double March's compiled-out case).
- Packed: data034, datai014, data036, data056, data057, data151, datai020, datai021, datai022 (`PACK OK` x9). **Not copied to the game dir yet: the user is asked.**
- `JUNE_BUGS.md` rows 417 (two controls named `"Event"`) and 418 (include asks for the dropped `EXP` key).

**Hygiene:** `flagcheck JUNE_BOTTOMBAR_IDS --worktree --files ...`: `files: 2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR` / `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`. `luasrc.py ledger --check`: PASS (0 FAIL, 0 WARN). `core --check` reports one FAIL that is not B1's: `data036 ResultData_new.lua` BOM text not on the core list (a peer's file).
**Deploy:** DEPLOY OK: X2_offline.exe is 25,839,104 B, dated 2026-10-24 10:33:20, md5 aa3c79dcdd35114854207a358967d3d8, equal to X2/US_SERVICE/x2.exe; built at main 07c4ed56 + 3 uncommitted code file(s). The STUB strings are in the deployed exe.

**How to see it:** after the kom copy, the village bar is June's dialog: June's EXP / stamina plates and the 15-button row art. Button **positions are still March's runtime layout** (`UpdateMenuPosistion`) until B2; the June-only buttons (Profession, StoryBook, Valentine) may sit stacked or hidden until then. Stub clicks: Profession / StoryBook log the STUB line; the rest-gauge buttons need the EXP bar's invisible buttons, which B3 wires.
**Not in B1:** layout, badges, bars (B2 / B3); the include's event icons and event-EXP gauge are hidden by June's own file (Stage 9).

## Phase B2 - Row layout and badges (Sonnet 5.5, high)

**Depends on:** B1. **Flag:** `SERV_IRUHADEV_JUNE_BOTTOMBAR_LAYOUT`.
Port B0's layout rows whole (`june2017_re/bottombar_functions.tsv`): `3245bb70` in place of `UpdateMenuPosistion`'s body (with `3245a980` / `3245adb0` /
`3245aa20`); `3245cba0` as a new member called from `InitUI` before the layout; in `InitUI` only the three calls (`32472a40`); `32468c70` as
`UpdateMenuString`'s body; the frame move's `Story_New` lines. Absent-value defaults per the B0 Done note (code events read false, PVE states never,
`+0x43d` = 0); the 32-vs-30 shift is a JUNE_BUGS row. `UpdateMenuString` keys are June `GAMEACTION` numbers, map by name. B0 reading [I], verify by the
tooltips in game: 0x11 QUEST, 0x13 COMMUNITY, 0x14 INFO, 0x15 INVENTORY, 0x16 OPTION, 0x17 PET, 0x19 SKILL_TREE, 0x1b FRIEND (June = March's block +1),
0x2c WARP, 0x2d Guild's (June tail; use March's `GAMEACTION_GUILD`), 0x2b profession (no March action: empty key, labelled).
**Gate:** the row matches June's slot order with stubs drawn; hiding a button (no pet / no guild) packs the row as June does; badges sit on their buttons.

## Phase B3 - EXP bar and stamina bar (Sonnet 5.5, high)

**Depends on:** B2. **Flag:** `SERV_IRUHADEV_JUNE_BOTTOMBAR_GAUGES`.
`32473810` whole in `UpdateExpGageBar`: the rest-mode branch is ported but unreachable (only the stub toggle could set `+0x468`) and calls a one-time log
instead of `32470210`; the resonance test answers false (comment names Stage 30); the level cap: `SERV_IRUHADEV_EXP_GAUGE_MAX_LEVEL_FIX` stays as its own
`#ifdef` inside the June body. `32475480` whole in `RefreshMenuInfo`: stamina width + percentage string + the empty spirit tooltip (JUNE_BUGS row); the
event calls `3245d870` / `3245da70` stay out (Stage 9). ED / AP as before.
**Gate:** EXP gain and level-up move the yellow bar and its text; a dungeon run moves the stamina bar and its "%" text; level 99 shows 100.0%.

## Phase B4 - Button bodies that differ (Sonnet 5.5, high)

**Depends on:** B3. **Flag:** `SERV_IRUHADEV_JUNE_BOTTOMBAR_BUTTONS`.
Each `differs` row of `bottombar_buttons.tsv` (Guild, System, Cash, Warp, Event), June's case body whole at March's case; menu ids by name; June-only members
read as their absent value (duel request, tournament, `+0x191`, `+0x70`: never set). Plus the frame move's Cash message swap 102 / 180 with
`g_pInstanceData +0xab4` as a labelled stand-in reading 1 (B0 Done note). Write `kickoffs/june-bottombar/B4-review.md` from the review template when done.
**Gate:** each of the 12 working buttons opens what June's opens (hand-off lists them one per line).

## Review (Opus 5.5, high)

`/x2-phase june-bottombar B4 review` covers B1-B4: June vs ours line by line, the asset list against June bytes, flagcheck ranges, the deployed exe, the
play-test lines.

## Phase Z - Close-out (Sonnet 5.5, medium)

`MODS.md` (flags, staged files and their backups, revert steps), backlog Stage 32 closing note, the Stage 9 stub bullet against the final tsv, STATUS, memory. No code.
