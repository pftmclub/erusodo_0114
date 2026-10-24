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

**Landed** (commit `b0ab40af`, flag `SERV_IRUHADEV_JUNE_BOTTOMBAR_IDS`; no `_DEBUG` flag was needed, the stub lines are the permanent log):
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

### B2 - Done (2026-10-24, Sonnet 5.5)

**Landed** (flag `SERV_IRUHADEV_JUNE_BOTTOMBAR_LAYOUT`, `Always.h` + `X2Informer.h` / `X2StateMenu.h` / `X2StateMenu.cpp`; no new `.lua`, no kom change, no `_DEBUG` flag):
- `JuneUpdateMenuPosition` (June `3245bb70` + `3245a980` / `3245adb0` / `3245aa20`): `UpdateMenuPosistion` returns into it. `m_vecpButtonMainMenu` is June's 15-slot vector (Warp Guide Charac Inven Profession Pet Skill StoryBook Quest Guild Comm System Cash FriendRecommed Event). Hide rules: Guild = no guild, Pet = March's no-pet test, Skill = March's SP test, Profession = level < 40. Each hidden slot adds 32 to every slot at or left of it; then every button gets `SetOffsetPos` + `SetGuideDescOffsetPos`, and the badges / informers follow by slot (Warp_VIP; Inventory_New + Inven informer; Pat_* + AccompanyPetTag; Skill_New + informer; Story_New; Quest_New + informer; Guild informer).
- `JunePlaceMenuBadges` (June `3245cba0`), called from `InitUI` first: `*_New` / `Pat_*` / `Story_New` at the button's Normal point ( +5, -17 ), `Warp_VIP` ( +0, -12 ), the four informer talk-box anchors ( +7, -10 ) through the new `CX2Informer::SetTalkBoxPos`.
- `JuneUpdateMenuString` (June `32468c70`): string 0 / 1 = 19974 / 19975, string 2 = empty (June 43488 shows only in rest mode), tooltips by GAMEACTION name: Charac INFO, Inven INVENTORY, Skill SKILL_TREE, Quest QUEST, Pet PET, Guild GUILD (19981), Comm 44512 with ( FRIEND, COMMUNITY ), System OPTION, Cash 19984, Guide 19985, Warp WARP (24618), Profession 31200 with an empty key (March has no profession action).
- Frame move: `Story_New` `SetShow( false )` (June reads `g_pInstanceData +0xabd`, never set offline).
- Absent-value defaults as B0: code events 2220 / 2316 read false (their branches are not ported), PVE states 0x17 / 0x18 never, `this +0x43d` = 0. Left out on purpose: `Warp_New` shift (code event 2316), resonance label (Stage 30).
- `JUNE_BUGS.md` row 419 (32 vs 30 spacing).

**Check by eye (B0 [I]):** the Comm tooltip argument order ( FRIEND, COMMUNITY ) follows B0's key list; if the two lines read reversed, swap the two `GetKeyString` arguments in `JuneUpdateMenuString`. Tooltip key names (QUEST / INFO / INVENTORY / OPTION / PET / SKILL_TREE / FRIEND / WARP) are mapped by name, so a wrong letter is a wrong string id, not a wrong key.

**Hygiene:** `flagcheck JUNE_BOTTOMBAR_LAYOUT --worktree --files ...`: `files: 3 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR` / `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`.
**Deploy:** DEPLOY OK: X2_offline.exe is 25,841,152 B, dated 2026-10-24 10:53:16, md5 19f0ed402aa350e091a26adffe6108cb, equal to X2/US_SERVICE/x2.exe; built at main 1dcafffe + 4 uncommitted code file(s). `kom.py status`: 0 need a pack, 0 need a copy (B1's koms are already in the game dir).

**How to see it:** village bar. Char `wewe` (uid 20, level 7): no Profession button, no guild, no pet, so the row is packed to the right with 2 px overlaps. Char `reyaa` (uid 12, level 99): Profession (stub, click logs) and StoryBook (stub) drawn in their slots.

## Phase B3 - EXP bar and stamina bar (Sonnet 5.5, high)

**Depends on:** B2. **Flag:** `SERV_IRUHADEV_JUNE_BOTTOMBAR_GAUGES`.
`32473810` whole in `UpdateExpGageBar`: the rest-mode branch is ported but unreachable (only the stub toggle could set `+0x468`) and calls a one-time log
instead of `32470210`; the resonance test answers false (comment names Stage 30); the level cap: `SERV_IRUHADEV_EXP_GAUGE_MAX_LEVEL_FIX` stays as its own
`#ifdef` inside the June body. `32475480` whole in `RefreshMenuInfo`: stamina width + percentage string + the empty spirit tooltip (JUNE_BUGS row); the
event calls `3245d870` / `3245da70` stay out (Stage 9). ED / AP as before.
**Gate:** EXP gain and level-up move the yellow bar and its text; a dungeon run moves the stamina bar and its "%" text; level 99 shows 100.0%.

### B3 - Done (2026-10-24, Sonnet 5.5)

**Landed** (flag `SERV_IRUHADEV_JUNE_BOTTOMBAR_GAUGES`, `Always.h` + `X2StateMenu.cpp`; no new `.lua`, no kom change, no `_DEBUG` flag):
- `UpdateExpGageBar` (June `32473810`): in the live `REAL_TIME_ELSWORD` branch, March's ratio and clamp stay shared (so `SERV_IRUHADEV_EXP_GAUGE_MAX_LEVEL_FIX` still gives 100.0% at the cap). June's output replaces March's: `StaticMenu_Button_EXP_Gauge` `SetShow( true )`, picture 0 `SetSize( orig.x * rate, orig.y )`, string 0 "%.1f%%", then the `Button_Invisible_ExpGuage` tooltip "%.1f%%" + `SetShowEnable( true, true )`.
- Rest mode (`this +0x468`) is a local `false`; its branch logs `STUB rest gauge 32470210 -> Stage 9` once and skips the bar and tooltip as June does. Resonance (`3247b460`) is not ported (Stage 30; March answers "no resonance").
- `RefreshMenuInfo` (June `32475480`): `StaticMenu_Button_Tired_Gage` picture 0 `SetSize( orig.x * spirit / max, orig.y )` and string 0 "%.1f%%" (no March rounding); `Button_Invisible_SpiritGuage` guide text set **empty** + `SetShowEnable( true, true )` (`JUNE_BUGS.md` row 420). Event calls `3245d870` / `3245da70` stay out (Stage 9). `UpdateExpGageBar()` call and the inventory ED / AP lines are March's, unchanged.
- No guard on `m_iSpiritMax == 0` (June has none; the emulator never sends 0).

**Gate note (stamina):** the offline emulator pins spirit full (`CX2OfflineServer::FillSpirit`, `SHRT_MAX` / `SHRT_MAX`), so the stamina bar shows **100.0%** and does not move in a dungeon; the gate line "a dungeon run moves the stamina bar" cannot happen offline. Drift: a spirit model got its own bullet in the Stage 9 block of `JUNE_PORT_BACKLOG_ORDER.md` (not needed to play).

**Fix after the first play-test (2026-10-24):** the dialog draws the rest-EXP gauge (`EXP_G`), the used-rest gauge (`EXP_R`) and the invisible `Button_Invisible_RestExpGuage` at the same spot, visible by default; the full-width red picture hid the yellow bar and the button took the hover. The non-rest branch now does June's `324700a0( false )` (hide + disable the three, picture 0 `SetSizeX( 0 )`) every refresh; no June call site was found for the first hide (only the two toggle cases and the ctor call it), so this is a labelled stand-in. Stamina tooltip stays empty (JUNE_BUGS 420, June quirk).
**Hygiene:** `flagcheck JUNE_BOTTOMBAR_GAUGES --worktree --files KTDXLIB/Always.h X2Lib/X2StateMenu.cpp`: `files: 1 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR` / `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`.
**Deploy:** DEPLOY OK: X2_offline.exe is 25,842,176 B, dated 2026-10-24 11:26:04, md5 75bb6ad79ed9bb1fe61a7e33636b763b, equal to X2/US_SERVICE/x2.exe (2026-10-24 11:25:57); built at main 94550cdc + 3 uncommitted code file(s).

**How to see it:** village bar of `eresisu` (uid 21, level 77); save set to EXP 58496900 (backup `db_backup/els_db_20261024_110355_pre_setchar.sql`), about 50% into level 77. Yellow bar half full, "50.0%" text on it, same text in the hover tooltip. Stamina bar full with "100.0%"; hovering it shows no tooltip (June quirk). Level 99 char `reyaa` (uid 12): EXP bar full, "100.0%".

## Phase B4 - Button bodies that differ (Sonnet 5.5, high)

**Depends on:** B3. **Flag:** `SERV_IRUHADEV_JUNE_BOTTOMBAR_BUTTONS`.
Each `differs` row of `bottombar_buttons.tsv` (Guild, System, Cash, Warp, Event), June's case body whole at March's case; menu ids by name; June-only members
read as their absent value (duel request, tournament, `+0x191`, `+0x70`: never set). Plus the frame move's Cash message swap 102 / 180 with
`g_pInstanceData +0xab4` as a labelled stand-in reading 1 (B0 Done note). Write `kickoffs/june-bottombar/B4-review.md` from the review template when done.
**Gate:** each of the 12 working buttons opens what June's opens (hand-off lists them one per line).

### B4 - Done (2026-10-24, Sonnet 5.5)

**Landed** (flag `SERV_IRUHADEV_JUNE_BOTTOMBAR_BUTTONS`, `Always.h` + `X2StateMenu.{h,cpp}`, no `.lua`, no kom change, no new `JUNE_BUGS` row). Original code stays in the `#else`. June's case bodies read from the `32476050` decompile and disassembly:
- **Cash** (`0x66`): March's refusals are replaced by June's in June's order. The PvP-match popup (13599) is March's. June's state-8 popup (32329) is `CX2Main::XS_PVP_ROOM`; March's `X2_STATE` matches June's numbering up to 0xb. Then `ToggleCashShop()`, with March's field-name / buff-name close kept because June's `ToggleCashShop` `32465fc0` starts with that close. The `NEW_SKILL_TREE_UI` skill-tree hide is gone (June has none). June-only refusals read 0: `CX2State +0x16c` busy (16478), party `+0x70` (16478), duel request `+0xda` (39561), tournament `PartyData +0x10 == 6` (42839).
- **System** (`0x95`): `g_pInstanceData` set, its `+0x191` (June-only, reads 0), and no dungeon-match popup (`31f38e90( 10 )` = `IsThereTimedMessagePopup( MT_DUNGEON_MATCH_START_REQUEST )`; March's ESC handler uses 9 for the same function) -> `ToggleSystemMenu( true )`, always `return true`. March's private-bank test and its old fall-through into the next case are the `#else`.
- **Event / advertisement** (`0x3e8`): a toggle. The browser open -> `CloseBrowser`, `SetDialogBoxMode( FALSE )`; closed -> `SetDialogBoxMode( TRUE )`, `OpenAdvertisementDialog( "http://en.elswordonline.com/ingameinfo/" )`, then June's `31be34b0` (keyboard, mouse and joystick `ReleaseAllButtonsAndKeys`, without `DeactivateApp`'s `m_bActivated = false`). `g_pMain +0x708` (Axeso5 URL) has no March member and reads 0; March's `es` URL by server group is not June's.
- **Guild** (`0xa7`): June's body is `CX2UIGuild::SetShow( true )` (`325ab110`, builds `DLG_Guild_Info_New_3.lua`), a June-only guild UI. The gate (`GetGuildManager()->GetUIGuild()`) is ported and the click logs `STUB SMUCM_GUILD -> Stage 9`. Offline has no guilds, so the button is hidden and unreachable.
- **Warp** (`0x3e9`): no code. Under June's case only the duel-request popup (party `+0xda`) differs from March, and it reads 0; the rest (`ClickWarpButton( !m_bWarpVip )`) is March's.
- **Frame move Cash swap** (`@32470bd4-32470c45`): msg 102, or 180 with `g_pInstanceData +0xab4` = 0 (and a closing `ToggleCashShop` when the shop is open). The stand-in `bJuneCashShopOpen` reads **true** (B0 decision). June's 180 is March's `SMUCM_WARP_DEST_SELECT`, so the maintenance message is a new `SMUCM_JUNE_CASH_MAINTENANCE = 215` (C++ only); its popup case (39402) is **not** written: unreachable with the stand-in.
- Drift found, bullets in the Stage 9 block of `JUNE_PORT_BACKLOG_ORDER.md`: June guild UI, Surprise Sale (the singleton `32696fa0` + `3267ac90` that the Cash click runs first), the June-only refusal state.

**Hygiene:** `flagcheck JUNE_BOTTOMBAR_BUTTONS --worktree --files KTDXLIB/Always.h X2Lib/X2StateMenu.cpp X2Lib/X2StateMenu.h`: `files: 2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR` / `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`.
**Deploy:** DEPLOY OK: X2_offline.exe is 25,842,688 B, dated 2026-10-24 11:55:22, md5 ccad9689106cf5d0eddc3e37604df3d9, equal to X2/US_SERVICE/x2.exe (2026-10-24 11:55:15); built at main da4ffc56 + 3 uncommitted code file(s).

**How to see it** (village; the guild button stays hidden, offline has no guilds): System opens and closes the system menu; Cash opens the shop (a second click closes it); Event opens the in-game browser on `en.elswordonline.com/ingameinfo` (a page error is possible: the site is gone) and a second click closes it. PvP-room and duel paths can't be tested offline.
**Open for the user:** none. Review kickoff: `kickoffs/june-bottombar/B4-review.md`.

## Review (Opus 5.5, high)

`/x2-phase june-bottombar B4 review` covers B1-B4: June vs ours line by line, the asset list against June bytes, flagcheck ranges, the deployed exe, the
play-test lines.

### Review - Done (2026-10-24, Opus 5.5)

**Verdict: PASS, with one fix** (B3's rest-gauge hide moved to June's call site). Ghidra, line by line:
- **Cash** (`0x66`) matches. June's refusal order is: state `+0x16c` 16478, then Surprise Sale `32696fa0` / `3267ac90`, then party `+0x58` (42839 if `326662b0` == 6, else 13599), then `+0x70` 16478, then `+0xda` 39561, then state 8 32329, then vslot `0xac` `ToggleCashShop`. B4 keeps that order once June-only reads are 0.
  - `XS_PVP_ROOM` = 8 in March (`X2Main.h:776`, the `ELSWORD_NEW_BEGINNING` branch is the same length either way).
  - `32465fc0` starts with `g_pTFieldGame` (`33775870`) -> `323a9780` / `323a9920` (field-name / buff-name close). Its boss-raid refusal 28379 is already in March's `ToggleCashShop`.
  - June has no skill-tree hide.
- **System** (`0x95`) matches. With `SERV_PVP_NEW_SYSTEM` on (`ServerDefine.h:1188`), March's `MT_DUNGEON_MATCH_START_REQUEST` = 10 = June's `PUSH 0xa`. The exit is `@3247663f MOV AL,1`, so it always returns true.
- **Event** (`0x3e8`) matches. June's browser wrapper sits at `g_pMain +0x564`, and `33222f80` (CloseBrowser) sets `+0x10` = `m_bClosed`, so `+0x574 == 0` means "open" and the toggle reads as B4 has it. The calls are SetDialogBoxMode = D3D vslot `0x50`, `31f07bd0` = `OpenAdvertisementDialog`, and `31be34b0` -> `31c48d90` (DI manager `+0x40` / `+0x44` / `+0x48` -> `ReleaseAllButtonsAndKeys`).
- **Guild** (`0xa7`): the gate (`g_pData +0x2ec +0x1e8`) -> `325ab110( 1 )`, and June returns false; the STUB is right. **Warp** (`0x3e9`): only the `+0xda` duel popup differs, then `+0x9f0` -> `3246c780( !vip )`. No code needed.
- **Cash swap** `@32470bbf-32470c45` matches: `g_pInstanceData` set, `"Cash"` (`334976e8`), `+0x27c` 0x66 / 0xb4, cash shop `g_pData +0x290 +0xd8` -> vslot `0xac`. `SMUCM_JUNE_CASH_MAINTENANCE = 215` is right (180 is March's warp-dest). The stand-in reads 1.
- **B2** (`3245bb70`) matches. The push order `@3245bd48-3245bdf0` is B2's 15-slot order. `3245a980( i )` adds `33439e00` to `v[0..i]`, and the hide indices are Profession 4 (level < 0x28), Pet 5, Skill 6, Guild 9.
- **B3** (`32473810`) matches: `SetShow( 1 )` (vslot `0x68`), picture 0 width = `+0x24` * rate (June's `31c933d0` takes one float, i.e. a SetSizeX; ours `SetSize( x * rate, orig.y )` draws the same), "%.1f%%", tooltip + `SetShowEnable( 1, 1 )` only when not in rest mode. `32475480` has no function in Ghidra and was not re-read here (stamina is pinned at 100% offline).

**Fix (flag `_GAUGES`, `X2StateMenu.cpp`).** The B3 fix hid the rest gauges on every refresh, and its comment said June had no call site for the first hide. B0 had recorded one (`bottombar_functions.tsv` row 14): the CX2StateMenu ctor `32474d80` (vtable `334978f4`) ends `@32475400` with `+0x43d = 0; 324700a0( false )`. The hide now sits at the end of March's ctor, after its `InitUI()`, with `324700a0`'s trailing `32468c70` (`JuneUpdateMenuString`, under `_LAYOUT`). The per-refresh copy is gone. The picture stays the same. `funcmap.tsv` row updated.

**Hygiene** (one run per flag over its commits): IDS `b0ab40af` `files: 2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; LAYOUT `886efc91` `3 PASS ... 1 DEFINE`; GAUGES `94550cdc` `1 PASS ... 1 DEFINE`, `a1536b18` `1 PASS`; BUTTONS `3427a2f0` `2 PASS ... 1 DEFINE`. Every run has 0 OTHER / FAIL / ERROR, `in this flag's files: 0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
Review fix `--worktree --files X2Lib/X2StateMenu.cpp`: `files: 1 PASS`, `18 deleted line(s)` (the B3 fix's own flag-on block, moved to the ctor; no March line deleted), `RESULT: PASS`.
**Koms:** data034 / datai014 / data036 / data056 / data057 / data151 / datai020-022 are in sync (`kom.py status`, 0 changed). The B4 exe (md5 `ccad9689`) has the STUB / `ingameinfo` strings.
**Play-test lines** (run 12:03): one `STUB SMUCM_PROFESSION_LIST -> Stage 9`, plus 4 UNHANDLED `E_DISABLE_HB_CHECK_REQ`, which are not this plan's. 0 EXCEPTION.
**Deploy:** BUILD OK: X2/US_SERVICE/x2.exe 25,842,688 B, 2026-10-24 12:14:20 (main 91c8e36f + this fix). Then `--deploy-only`: DEPLOY OK: X2_offline.exe is 25,842,688 B, dated 2026-10-24 12:17:17, md5 a80846f7543e25dca78a47de20e92b27, equal to X2/US_SERVICE/x2.exe (2026-10-24 12:14:20); built at main ab26bca6.

**Open for the user:** none. Next: Phase Z.

## Phase Z - Close-out (Sonnet 5.5, medium)

`MODS.md` (flags, staged files and their backups, revert steps), backlog Stage 32 closing note, the Stage 9 stub bullet against the final tsv, STATUS, memory. No code.
