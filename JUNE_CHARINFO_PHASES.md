# June character info window: the U-key window as June draws and runs it

Backlog Stage 34 (user, 2026-10-28; split out of Stage 9). Plan key `june-charinfo`, branch `main`, log tag `[JUNE_CHARINFO]`,
flags `SERV_IRUHADEV_JUNE_CHARINFO_*`, kickoffs `kickoffs/june-charinfo/`, evidence `june2017_re/charinfo_functions.tsv` (C0 writes it).

## Context

Planning evidence (2026-10-28, Ghidra + `june_src_fixed/` + the March tree, read-only). [D] = seen in the listing or file, [I] = inferred, C0 confirms.
Funcmap rows `(CharInfo S34)`.

**One class, one dialog, both sides KTDGUI.** June's window is still `CX2UICharInfo` over a Lua dialog; no class function calls `openUI`,
`loadUserWidget` or a `.swf` [D]. Only the dialog file and the class bodies differ.
- June: `CX2UIManager::CreateUICharInfo` `3214fd30` allocates 0x270 bytes and calls the ctor `32916920( stage, L"DLG_UI_CHARACTER_NEW_4.lua" )`
  (string `334795e8`) [D]. RTTI `.?AVCX2UICharInfo@@` `3373cbb0`, vtable `334c5f1c` (59 slots), listener sub-object vtable `334c5f04` at `this +0xd8`.
- March: `X2UIManager.cpp:3582-3597` builds `DLG_UI_CHARACTER_NEW_2.lua` (`SERV_PVP_NEW_SYSTEM` + `PVP_SEASON2` are on). `X2UICharInfo.cpp` is 3244 lines,
  one page, no tabs; the only toggle is the Dungeon / PvP stat radio (`DungeonStat` / `PvpStat`).
- Opened by `GA_INFO` (U; `X2Main.cpp:13602`, `X2StateBattleField.cpp:514`, `X2StateDungeonGame.cpp:4963`) -> `ToggleUI( UI_MENU_CHARINFO )` (`X2UIManager.cpp:2362`,
  June `32158070` case 0x13, already March's body, skill S7n), and by the bottom bar's `SMUCM_MY_INFO` (`X2StateMenu.cpp:1207`).

**Dialog files** (all data034; none on the core list or in the ledger) [D]:

| File | June source | March |
|---|---|---|
| `DLG_UI_CHARACTER_NEW_4.lua` | 104,551 B, 94 controls, **loaded by June** | none |
| `DLG_UI_CHARACTER_NEW_3.lua` | 90,142 B (NEW_2 + damage meter + resonance) | none |
| `DLG_UI_CHARACTER_NEW_2.lua` | 91,404 B | 36,424 B bytecode, **loaded by March** |
| `DLG_UI_CHARACTER_NEW.lua` / `DLG_UI_CHARACTER.lua` | 58,688 / 50,635 B | 34,649 / 30,349 B |

NEW_4 adds over NEW_2: `AccountAchivement`, `g_pStatic_Achievement`, `Button_DamageLog`, `CharWindowCharLightSet`, `Light_On` / `Light_Off`, `Ring_Item_Check`,
`g_pButton_Resonance` (+ `_Guide`, `_Guide_OnGame`), the PvE record (`g_pStaticInfo_PVE_Win`, `PVE_Grade`), and earring / minime / second-ring slots
(`NESI_AC_EARRING` / `_MINIME` / `_RING2`, `NESI_MINORUNIT_*`: `june2017_re/ui_dlg_census.tsv` rows 332-347, 1136-1137, 1556-1558, 3990). No IncludeLua.
`SetCustomMsgRButtonUp` is bound in March (`KTDXApp.cpp:2427`); the full binder check is C0's.

**Textures** (NEW_4's `SetTex`): every image is in a March kom, but [D]:
- `DLG_UI_Common_Texture_PVE_01` has no `.TET` in March (data057 has the `.tga` only); NEW_4 reads `Bg_Character`, `Text_Pve_Record`, `Text_Pvp_Record`;
- `DLG_UI_Title04.TET` lacks `Maximize`, `MpIncByAttack`, `MpIncByAttacked`, `SpecializationA`, `SpecializationB`, `Text_Dp_Recharge`, `Text_Dp_Spent`;
- the rest (Button21 / 22_NEW, Texture70 / 92_NEW, Image_01 / 02 / 07 / 09, Text_05, ...) go through `kom.py stage-june` by the same-name rule.

**Messages.** `CHARACTER_INFORMATION_MSG` (June `Enum.src.lua:8699-8724`, live `Enum.lua:7208-7216`; `X2UICharInfo.h:14-29`) [D]. 10000-10006 are
the same on both sides. June-only: `CIM_CUSTOMIZE` 10007, `_SWITCHING` 10008, `_SWITCHING_ONLY_THIS_DIALOG` 10009, `_ALL_EQUIPPED_ITEM_RELEASE` 10010,
`_WEAPON_CHANGE_REVOLVER / AUTOGUN / MUSKET / HANDCANNON` 10011-10014, `_LOCK_WEAPON_CHANGE_*` 10015-10018, `_DAMAGE_METER` 10019, `_RESONANCE` 10020,
`_LIGHT_ON` / `_OFF` 10021 / 10022, `_ACHIEVEMENT_OPEN` / `_CLOSE` 10023 / 10024. NEW_4 also sends `UI_ACHIEVEMENT_SYSTEM_CUSTOM_MSG.UASCM_ACHIEVEMENT_OPEN`
(83010; the whole table is June-only, `Enum.src.lua:10722`).

**June's `UICustomEventProc`** (slot 55, `3291db60`, not a Ghidra function; jump table `3291df1c`, byte map `3291df50`) [D]. First, if the title
manager (`g_pData +0x28c`) shows (`326f7d20`), the event goes to it (`32703a70`). Then:

| Id | June body | March |
|---|---|---|
| EXIT | `SetShow( false )` | same |
| TITLE | title manager `326fd970` | `OpenTitle` |
| MOTION_SELECT, PROFILE, PASSWORD_OK / _CANCLE | not handled (false) | PROFILE / PASSWORD = local ranking (`:966-1040`) |
| SPECIAL_OPTIONDATA_TOGGLE | radio -> `+0x258` = 1 or 0x100, `ResetStat( 0 )` | `:1041` |
| CUSTOMIZE | allowed states only (`3291df70`); REQ `0x62b` until `+0x25c`, then UI manager `+0x68` `3214f3a0` | none (Stage 12) |
| SWITCHING | toggles unit `+0xe40`; sends `0x4fd` in state 0xf, else `0x4d9` | none |
| SWITCHING_ONLY_THIS_DIALOG | `32912c50` (`+0x25a`, viewer `+0xd80`), `3291d2e0`, viewer `31f94190`, `ResetStat( 1 )`, ResetNowEquipUI, inventory refresh | none |
| ALL_EQUIPPED_ITEM_RELEASE | `32918c70` | none |
| WEAPON_CHANGE_* | viewer HABIT motion `31fa5050( idx, 1 )`; class 0xb (Rose) sends `0x4fd` / `0x4d9` | none (Stage 8) |
| LOCK_WEAPON_CHANGE_* | `329191c0( idx )` check (unit `+0xe50`, string 0x9ea0), `32916ed0( idx )` REQ `0x5e3` | none (Stage 8) |
| DAMAGE_METER | not handled anywhere: byte map sends 10019 to default, `Button_DamageLog` stays hidden (C0) | none |
| RESONANCE | `g_pData +0x5a4` -> `3260bd00` | none (Stage 30) |
| LIGHT_ON / _OFF | UI manager `32150e30( 0 / 1 )` -> `329142a0` | none |
| ACHIEVEMENT_OPEN / _CLOSE | outside the jump range: not handled | none (Stage 31) |

**June's class functions** [D unless marked]:

| June | Role | March (`X2UICharInfo.cpp`) |
|---|---|---|
| `32916920` / `32914520` | ctor / dtor body | `:11` / `:91` |
| `3291df90` (vslot 0x20) | SetShow: `Static_Nickname`, `Static_GuildName`, PvP + **PvE** record, `Static_CharIcon` (T6c: ported), radio from `+0x259`, `EquipRelease`, resonance buttons, `CharacterSwitching*` (states 0xe / 0xf only); then `32912c50( unit +0xe40 )`, ResetStat, ResetNowEquipUI, ED, level, stone, viewer, `31fa7ef0` motion, light / weapon / lock UIs, inventory refresh | `:1075` |
| `3291cfb0` / `3291d2e0` / `3291d360` | ResetStat( bool ): `3291cee0`, `32913df0`, `32913460`, `32915ab0`, `32919430`, `32916160`, `32915620` / public wrapper / small entry | `:1423` |
| `32913460` | basic stat sum incl. skills 0x6722, 0x96b, 0x539d (`32890620` / `32206e50`) | `UpdateBasicStatFromSpecialSkill` `:2172` [I] |
| `32913df0` | `GetEqipStat( +0x258, 2 )` (S7k) | inside `:1423` |
| `32915ab0` | basic UI: hp / attacks / defences `%5d`, guide buttons `Button_Physic* / Magic*`, Lua `CalculateFinalDamageMaximizeRate`, strings 0x9a07 / 0x9a08 | `UpdateBasicStatUI` `:1496` |
| `32919430` | option UI (1149 lines): Critical, Add, Charge, Time, Decline, Moving, Speed, Jump, **Maximize, MpIncByAttack, MpIncByAttacked, SpecializationA, SpecializationB** | `UpdateOptionStatUI` `:1543` (+ Accuracy, Evade) |
| `3291f0c0`-`3291f960`, `3291eb50`, `32920210` | Lua callers: `CalculateFinal*Coefficient`, `CalculateFinalMpIncCoefficientByAttack(ed)`, `CalculateIntermediate*`, 12 per-class `Specialization A / B`, `CalculateHpIncrementValue` | `X2UICharInfo.h:125-168` |
| `32916160` | resistances `g_pStaticInfo_fire..dark`, `opacity1..6`, `  %d` | `:2086` |
| `32915620` | `ButtonPveOpacity` / `ButtonPvpOpacity` | none |
| `3291ba40` / `3291cee0` / `3291b4b0` | socket total by equip group (S7m; was Stage 9's S7p bullet) | `GetMyEquippedSocketData` `:2478`, `SumSocketOption` `:2404` |
| `329146d0` / `32914440` | ResetNowEquipUI / UpdateItemLevel | `:2656` / `:3173` |
| `32914ca0` / `32915490` / `32915550` | level / `g_pStaticInfo_cash` / resurrection stone | `:2732` / `:2713` / `:2753` |
| `32915100` | SetTitle (`#C00FFFF`) | `:1327` |
| `3291d390` (slot 4) | MsgProc: drag-rotate, click-on-unit switching toggle | `:310` |
| `32912d90` | UnitHandling (rotate, zoom `+0x1c8 / +0x1cc`) | `:161` |
| `329148d0` / `32914d70` | viewer create (`31fab020`) / viewer render (dead in June, C0) | `:2803` / none |
| `329142a0` | light (`CharWindowCharLightSet`, `Light_On` / `_Off`) | none |
| `32912c50` | switching view: `+0x25a`, viewer `+0xd80`, per-slot refresh `32e22ce0` | none |
| `32918c70` | unequip all (`[S%1%]%2%`) | none |
| `32913f40` / `32912d10` / `329141b0` | Rose weapon-change / lock UI | none (Stage 8) |
| `32914690` / `32914fe0` / `3291ece0` | customize REQ `0x62b` / ACK parse | none (Stage 12) |
| `32916e10` / `32916e40` / `32916e70` / `32916ea0` / `32916ed0` / `329191c0` | packet senders `0x4d9` / `0x4fd` / `0x5e3`, lock check | none |
| `32915080` (slot 56) | UIServerEventProc: `0x62c` / `0x62d` | `:1070` (empty) |
| `32917080` (slot 21) / `32918410` (slot 58) / `32917fd0` (slot 57) / `329128e0` (slot 54) | MouseUp (cursor switch) / OnDropAnyItem / OnRClickedItem / MouseRButtonUp (C0) | `:485` / `:784` / `:657` / `:647` |
| `32913b90` (slot 27) / `32916810` (slot 17) / `3291d830` (slot 20) / `3291d9f0` (slot 16) / `32914380` (slot 25) | DrawSlotMouseOverImage / GetSlotItemDesc / MouseDown / SetNowDragItemReturn (+ skill tree `328eea70`) / OnFrameMove (C0) | `:2900` / `:2990` / `:430` / `:754` / `:113` |

**C0 readings (2026-10-30; verdict per function in `june2017_re/charinfo_functions.tsv`):** the vtable map above is anchored on RegisterLuaBind
`327433e0` = slot 19 (the ctor calls it). `32914d70` is dead in June (its only caller `32915760` is referenced nowhere in the image). Unit `+0xe40` is
the Lu/Ciel form byte (1 = Ciel, the minor unit; setter `3234b8c0` refreshes the inventory) and `+0xe50` Rose's weapon index: **switching is Lu/Ciel-only**
(SetShow shows the buttons only for `UT_LUCIEL`), like the weapon change is Rose-only. June has no event-name table, so the six packets stay unnamed:
`0x4d9` / `0x4da` and `0x4fd` / `0x4fe` = Lu/Ciel form (or Rose weapon) REQ / NOT in village / guild agit, `0x5e3` = Rose weapon-lock REQ, `0x62b`-`0x62d` =
customize list REQ / ACK / NOT; March has none, the emulator answers none. Unequip-all sends only the slot-change REQ March already has (`32916d90`).

June members: `+0x1b4` viewer, `+0x1e4 / +0x1e5` show, `+0x258` (word) / `+0x259` stat radio, `+0x25a` switching view, `+0x25b` unit-click latch,
`+0x25c` customize data received, `+0x260..+0x268` a vector. Unnamed: packets `0x4d9`, `0x4fd`, `0x5e3`, `0x62b`-`0x62d`, unit `+0xe40` / `+0xe50` (C0).

**Already June in this window (keep):** `SERV_IRUHADEV_JUNE_TRANSCEND_IMAGE` (`X2UICharInfo.cpp:1185-1189`, T6c). Other plans' hooks that call into it
and must keep working: `JUNE_SKILL_TITLE_NOTICE` (`X2TitleManager.cpp:2195`), `JUNE_SKILL_HUD_MAXHPMP` (`X2SkillTree.cpp:2733`, `X2TitleManager.cpp:2190`,
`X2UIShop.cpp:2231`), `JUNE_SKILL_MEMO_PACKET` (`X2SkillTree.cpp:3509`).

## Scope fence

**In this plan:** June's `DLG_UI_CHARACTER_NEW_4.lua` (and June's same-name `DLG_UI_CHARACTER*` files) with their textures, byte-exact; CIM 10007-10024
and `UI_ACHIEVEMENT_SYSTEM_CUSTOM_MSG` in `Enum.lua` and `X2UICharInfo.h`; every `CX2UICharInfo` function in the table above whose June body differs
(whole body, "no partial updates"); the socket total (`3291ba40` family, taken over from Stage 9's S7p bullet); unequip-all (it rides March's
slot-change REQ, which the emulator already answers).

**Drawn, click logs `[JUNE_CHARINFO] STUB <name> -> <owner>`, opens nothing (user, 2026-10-28):** Customize (Stage 12), Resonance (Stage 30),
Achievement (Stage 31), Rose's weapon change / lock (Stage 8; June shows them for Rose only), Lu/Ciel switching `CIM_SWITCHING` /
`_ONLY_THIS_DIALOG` (Stage 8; June shows them for Lu/Ciel only, C0). **Damage Meter** is dead in June (C0): drawn hidden as NEW_4 has it, no stub.
**Lu/Ciel in ported bodies (C0):** unit `+0xe40` reads as 0 (`stand-in: Lu/Ciel not ported`); the 17 `NESI_MINORUNIT_*` slots stay hidden. June's NESI
renumbering (`NESI_ONEPIECE_FASHION` 20 -> 35, `NESI_END` 21 -> 43, a save migration) and the minor-unit content are Stage 8's.

**Not in this plan:** `CX2UIWatch` (other players' info, `DLG_UI_Watch_Ver1.lua`: Stage 9, user 2026-10-28); the title window itself; the resonance,
damage-meter and customize windows; the item data behind earring / minime / second ring (Stage 16 unless C0 finds March already has them); the
`CX2Stat` sums under the printed numbers (`GetUnitStat`, `GetEqipStat`, `GetSkillStat`: Stage 33); the transcend-UI leftovers in Stage 9.

## Rules

- Branch `main`; commit with explicit pathspecs. One flag per logical change in `KTDXLIB/Always.h`, March's code in `#else`; one-line comments.
  `X2UICharInfo.{h,cpp}`, `X2UIManager.cpp` are CP949: `bytepatch.py` only.
- June files go in through `luasrc.py adopt-june` (Lua, ljconv bytecode) and `kom.py stage-june` (textures); never edited. `Enum.lua` is core-list BOM source:
  edit the text, then `luasrc.py refresh`; `ledger --check` stays PASS. Assume March lacks any asset the ported code uses: check by name first.
- No hardcoded enum literals: CIM / NESI / `UI_MENU_*` / state ids by name (`enumscan.py --check`). June's UI-manager ids are mapped by name, never by number.
- File-local helpers go in `_CONST_CHARINFO_<phase>_` named namespaces (no anonymous namespace, no `JUNE` in names: CLAUDE.md 2026-10-28).
  June's body goes inside March's own `CX2UICharInfo` function under the flag; a function March lacks gets a plain name in the class's pattern.
- June quirks verbatim with a `JUNE_BUGS.md` row; absent June state takes June's default or a labelled stand-in under its own flag; no popups, always log.
- For every June `.lua` staged: `luabind.py check`. Pack with `/elstools pack`; **ask before every `/x2-kom-packed copy`**.
- Hand-offs: dev console, `/additem <id> [qty]` for gear, copy-ready; never put skills in slots 4 / 9 of a non-transcended save.

## Starting context

Tools only: `python tools/build.py --deploy` (background), `flagcheck.py`, `bytepatch.py`, `docsection.py`, `logs.py --tag JUNE_CHARINFO`, `luasrc.py`,
`kom.py`, `luabind.py`, `enumscan.py`, `elsdb.py`. June: Ghidra (`.claude/skills/ghidra`; slots 16 / 20 / 54-57 have no Ghidra function: read them with
`june2017_re/funcdis.py` / `disasm_range.py`), `june_src_fixed/data034_kom/DLG_UI_CHARACTER_NEW_4.src.lua`. March: read `X2UICharInfo.cpp` from a `cl /P`
live-lines copy (US_SERVICE), not from its `#ifdef`s (`MODIFY_CHARACTER_STAT_INFO` and `MONSTER_TROPHY` are off).

## Phase C0 - Identify, read-only (Opus 5.5, high)

**Run with:** `/x2-phase june-charinfo C0`. No code, no build. Delegate pulls to Sonnet subagents; keep the readings.
1. Every function in the Context table: confirm the March counterpart and give a verdict `same` / `differs (<what>)` / `June-only` / `STUB (<owner>)`, and a phase.
   Name slots 54 / 57 and `32914d70`'s caller. Write `june2017_re/charinfo_functions.tsv` and funcmap rows `(CharInfo C0)`.
2. Name the packets `0x4d9`, `0x4fd`, `0x5e3`, `0x62b`, `0x62c`, `0x62d` (June's event-name table vs March's `EventID_*.h`), and unit `+0xe40` / `+0xe50`
   (readers and writers; `GetEqipStat`'s caller tests `+0xe40`). Say per packet whether March has the id and the emulator answers it.
3. Who handles `CIM_DAMAGE_METER` in June (xrefs to 10019 / the `Button_DamageLog` path); set the owner for its stub.
4. Dialog: `luabind.py check DLG_UI_CHARACTER_NEW_4`; every control March's C++ reads that NEW_4 lacks, and whether it is NULL-checked; the `STR_ID`s NEW_4 uses
   that are missing from the live `StringID_def.lua`; `kom.py stage-june --dry-run` for every texture of NEW_4 (and `.TET` key drops, the B0 method).
5. The Lua functions the stat UI calls (`CalculateFinal*Coefficient`, `Specialization*`, `CalculateHpIncrementValue`): present in March's data036 scripts?
   If not, which June file holds them and is it a core-list file.
6. The new slots (earring, minime, second ring, `NESI_MINORUNIT_*`): does March's item / equip-position enum have them? In this plan, or Stage 16.
7. Fix C1-C5's lists if the readings move them; add the Done note (<= 60 lines).
**Gate:** every tsv row has a verdict and an owner; every packet is named or "none (why)"; the asset list is final.

### C0 Done (2026-10-30, Opus 5.5 high; read-only, no code, no build)

**Landed:** `june2017_re/charinfo_functions.tsv` (49 rows: verdict + phase each), funcmap rows `(CharInfo C0)`, the Context / Scope fence / C1-C5 corrections
above. Method: Ghidra decompiles + a string/call signature per June body, against a `cl /P` live-lines copy of `X2UICharInfo.cpp` (US_SERVICE:
`SERV_PVP_NEW_SYSTEM`, `PVP_SEASON2`, `SERV_SKILL_NOTE` on; `MODIFY_CHARACTER_STAT_INFO`, `MONSTER_TROPHY` off).

**1. Verdicts.** Same as March (keep): UpdateLevel, ResetResurrectionStoneUI, UpdateItemLevel, GetSlotItemDesc, MouseRButtonUp; ED [I]. MouseDown
keeps March's call order. Everything else differs or is June-only (tsv). Vtable map anchored on RegisterLuaBind = slot 19: 16 SetNowDragItemReturn,
17 GetSlotItemDesc, 20 MouseDown, 21 MouseUp, 25 OnFrameMove, 27 DrawSlotMouseOverImage, 54 MouseRButtonUp, 57 OnRClickedItem (the plan had 57 as
SetNowDragItemReturn), 58 OnDropAnyItem. `32914d70` is dead (only caller `32915760`, which nothing references: whole-image scan).
**2. Packets.** June has no event-name table (as the quest plan found); none of the six can be named and March has none. `0x4d9`/`0x4da`,
`0x4fd`/`0x4fe` = Lu/Ciel form (Rose: weapon index) REQ / NOT in village / guild agit; `0x5e3` Rose weapon-lock REQ (4-bool vector); `0x62b`/`0x62c`/`0x62d`
customize list REQ / ACK / NOT. No emulator handler; none needed (all are stubs). Unit `+0xe40` = Lu/Ciel form byte (1 = Ciel; setter `3234b8c0`
refreshes the inventory; equip lookups `3234fa70`/`3234fb00` test it for item group 2/3, type 10 only); `+0xe50` = Rose's weapon index (setter
`3234dff0`, lock vector `+0xe54`). Unequip-all sends June `0x100` via `32916d90` = March's `Handler_EGS_CHANGE_INVENTORY_SLOT_REQ` (answered).
**3. Damage meter.** Nobody handles 10019: CharInfo's byte map `3291df50` sends it to default, no other June compare of 0x2723 is a CIM, no C++
names `Button_DamageLog` (hidden in NEW_4). Owner: none; drawn hidden, no stub.
**4. Dialog.** `luabind check DLG_UI_CHARACTER_NEW_4`: 45 methods, 0 unbound, PASS. March's C++ reads 13 controls NEW_4 lacks; all are NULL-checked
(Accuracy / Evade rows go through `SetStatInfoStatic`, whose miss is a no-op in US_SERVICE; the rest are other dialogs or dead flags). 43 STR_IDs, all
in the live `StringID_def` with June's values. Textures: 22 sheets, 33 identical, 1 new `.TET`, 9 replaced (list in C1); every replaced TET only adds
keys (March's `DLG_UI_Title04.TET` lacks the 7 keys the plan named). NEW_4 names 22 enum members the live `Enum.lua` lacks (C1 list).
**5. Lua.** `SocketOptionForm.lua` (data036, June bytecode, not core-list) already holds all 145 `Calculate*` functions, decompile-equal to June.
Lead (Stage 33): `X2GameUnit.cpp:15916` / `:15992` / `:2473`, `X2SocketItem.cpp:1178` still say March lacks June's curves.
**6. Slots.** `EP_AC_MINIME` / `EARRING` / `RING2` = March's `EP_AC_TEMP2-4` (26-28, same values); `item.kim` has 475 / 305 / 0 items there, remapped to
FACE1/FACE2 by `JUNE_KIM_MINIME_REMAP` (Stage 16 removes it): the slots draw empty until then. `NESI_MINORUNIT_*` are Lu/Ciel's second body.
**Decision (mine, user may overrule):** keep March's NESI numbering. June's `NESI_MINORUNIT_DEFENCE_HAIR_FASHION = 20` is March's
`NESI_ONEPIECE_FASHION`, the equipped-bag index in every save; June's renumber (ONEPIECE 35, END 43) needs a save migration and only matters to Lu/Ciel.
So C1 registers every June name but that one, and C2 hides minor-unit slots by their `IS_FOR_MINOR_UNIT` flag (stand-in), not by June's ids
(`32e22ce0` would hide the onepiece slot at 20). Stage 8 owns the renumber.
**7. Plan moves.** Switching leaves C5 for a Stage 8 stub (Lu/Ciel only, like Rose's weapon change); C5 keeps unequip-all with no new packet or DB
column; C2 takes the switching view `32912c50` (SetShow calls it) and the slot hide; C4 gains OnFrameMove and `3291d360`, loses `32914d70`.

**Drift (routed in this commit):** Stage 8 (Lu/Ciel switching UI + `+0xe40`, the four form packets, NESI renumber, minor-unit slot content, Rose's
`0x5e3`); Stage 9 (Watch's light branch `328883b0` of `32150e30`); Stage 12 (customize light branch `32d76da0`, CharInfo's `0x62b`-`0x62d`);
MagicalCloset light `32d9c150` with the closet window (Stage 9, no owner yet); Stage 33 (the stale SocketOptionForm comments above).
**Open questions:** none blocking. The NESI decision above is the one to overrule before C1.

## Phase C1 - June's dialog, textures, ids, stubs (Sonnet 5.5, high)

**Depends on:** C0. **Flag:** `SERV_IRUHADEV_JUNE_CHARINFO_DLG`.
- `luasrc.py adopt-june` (data034): `DLG_UI_CHARACTER_NEW_4.lua`, `_NEW_3.lua` (new), `_NEW_2.lua`, `_NEW.lua`, `DLG_UI_CHARACTER.lua` (replace; C0).
- `kom.py stage-june` (C0 list, every other NEW_4 sheet is already identical): new `DLG_UI_Common_Texture_PVE_01.TET` (data057); replace
  `DLG_UI_Button21_NEW.TET/.tga` (data056), `DLG_UI_Common_Texture66_NEW.tga` (data057), `DLG_UI_Title04.TET/.tga` (data063),
  `DLG_Common_Texture_INT_ALL01.TET/.tga` (datai021), `DLG_UI_Title01_A.TET/.tga` (datai022). Every replaced TET only adds keys.
- `Enum.lua`: CIM 10007-10024 and `UI_ACHIEVEMENT_SYSTEM_CUSTOM_MSG`; `EP_AC_MINIME` 26 / `EP_AC_EARRING` 27 / `EP_AC_RING2` 28 (beside March's
  `EP_AC_TEMP2-4`, same values); June's `NESI_AC_*` 37 / 39 / 41 and every `NESI_MINORUNIT_*` except `NESI_MINORUNIT_DEFENCE_HAIR_FASHION` (June 20 =
  March's `NESI_ONEPIECE_FASHION`: held back, Stage 8; its slot reads `SLOT_ID` 0). Then `luasrc.py refresh`. `X2UICharInfo.h`: the same CIM members.
- `CreateUICharInfo` builds NEW_4 (March's NEW_2 line in `#else`).
- `UICustomEventProc`: a case per STUB id (customize, resonance, achievement, Rose's 8, Lu/Ciel's 2) logging `[JUNE_CHARINFO] STUB <name> -> <owner>`
  once per click. No damage-meter case (C0). NEW_4's minor-unit slots overlap the main ones until C2 hides them: say so in the hand-off.
- Pack the touched koms; ask before copy.
**Gate:** U in the village shows June's dialog art; every stub click logs; no `KEM_ERROR` for the dialog; `ledger --check` and `luabind` PASS; no new
`UNHANDLED` / `EXCEPTION`. (Numbers and new rows are still March's code until C2 / C3: say so in the hand-off.)

### C1 Done (2026-10-30, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_CHARINFO_DLG`)

**Landed:** the window builds June's `DLG_UI_CHARACTER_NEW_4.lua`; ids and stubs in place. Numbers and the new stat rows are still March's code until C2 / C3.
- **Lua (data034, `adopt-june`, June's ljconv bytecode):** NEW_4 and NEW_3 new; NEW_2, NEW, `DLG_UI_CHARACTER.lua` replaced (backups in `kom_backup_stage_june\20261030_154313`). `luabind.py check` 5 files, 0 unbound, PASS.
- **Textures (`stage-june`, 1 new, 9 replaced):** data057 new `DLG_UI_Common_Texture_PVE_01.TET`, replaced `Texture66_NEW.tga`; data056 `Button21_NEW.TET/.tga`; data063 `Title04.TET/.tga`; datai021 `Common_Texture_INT_ALL01.TET/.tga`; datai022 `Title01_A.TET/.tga` (backups `...\20261030_154328`).
- **`Enum.lua` (data036, core-list source, `luasrc.py refresh`):** CIM 10007-10024; `UI_ACHIEVEMENT_SYSTEM_CUSTOM_MSG` (whole table); `EP_AC_MINIME` / `EARRING` / `RING2` 26-28; `NESI_AC_MINIME` 37 / `EARRING` 39 / `RING2` 41 and 18 `NESI_MINORUNIT_*` (21-34, 36, 38, 40, 42). Held back per C0: `NESI_MINORUNIT_DEFENCE_HAIR_FASHION` (20) and June's `NESI_ONEPIECE_FASHION` 35 / `NESI_END` 43; March's 20 / 21 stand. Re-scan of NEW_4 against the live `Enum.lua`: only those two remain.
- **Client:** `X2UICharInfo.h` CIM 10007-10024 + a small `UI_ACHIEVEMENT_SYSTEM_CUSTOM_MSG { UASCM_ACHIEVEMENT_OPEN = 83010 }`; `X2UIManager.cpp` `CreateUICharInfo` builds NEW_4 (NEW_2 in `#else`); `UICustomEventProc` 13 STUB cases logging `[JUNE_CHARINFO] STUB <name> -> <owner>`: CUSTOMIZE -> Stage 12, RESONANCE -> Stage 30, UASCM_ACHIEVEMENT_OPEN -> Stage 31, SWITCHING / SWITCHING_ONLY_THIS_DIALOG and the 8 Rose weapon change / lock -> Stage 8. No damage-meter case (C0).
- **Not stubbed (their phase owns them):** `CIM_LIGHT_ON` / `_OFF` (C4), `CIM_ALL_EQUIPPED_ITEM_RELEASE` (C5): clicking them does nothing until then. NEW_4's minor-unit slots overlap the main ones until C2 hides them.
- **Gates:** `flagcheck JUNE_CHARINFO_DLG --worktree` RESULT: PASS (3 PASS, 1 DEFINE, 0 FAIL); `junediverge --check` RESULT: PASS; `enumscan --check` 0 A/B rows; `ledger --check` PASS, `core --check` PASS. `PACK OK` data034 / 036 / 056 / 057 / 063 / datai021 / datai022; `COPY OK` (user said "copy") md5 data056 `3ebecf46`, data057 `be599ed5`, data063 `3339d269`, datai021 `74f98a13`, datai022 `e6df65ff`; data034 / data036 copied in the same run.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 25,850,368 B, dated 2026-10-30 16:00:14, md5 a3cbf643d3fd4cb5ff365ce845454182`. `EXESTR OK: 5 of 5 found in X2_offline.exe` (the four stub tags checked + `DLG_UI_CHARACTER_NEW_4.lua`, wide).
- **Slip, fixed:** the first build failed (`CX2OfflineLog` undeclared): a bytepatch spec with two `@@ file` sections for `X2UICharInfo.cpp` dropped the `#include`. Re-applied alone; no code change otherwise.
**Open questions:** none.

## Phase C2 - SetShow, header, equipment, events (Sonnet 5.5, high)

**Depends on:** C1. **Flag:** `SERV_IRUHADEV_JUNE_CHARINFO_SHOW`.
June's whole bodies: SetShow `3291df90` (T6c icon kept), SetTitle `32915100`, level / ED / stone (`32914ca0` / `32915490` / `32915550`), ResetNowEquipUI +
item level (`329146d0` / `32914440`), UICustomEventProc `3291db60` (title-manager pre-forward; March's PROFILE / PASSWORD bodies in `#else`), UIServerEventProc
`32915080`. Resonance buttons and the PvE record take June's absent-value default (resonance off; the PvE record reads June UnitData members March
lacks: 0 wins, grade hidden). C0: level / ED / stone / item level are the same as March (keep); UIServerEventProc only parses the customize ACKs, so March's empty
body stays. SetShow's switching view `32912c50` comes here (ctor members `+0x25a..+0x268` too), with the per-slot hide `32e22ce0` as a labelled stand-in
that hides by the slot's `IS_FOR_MINOR_UNIT` (new `CX2SlotItem` field) instead of June's NESI ids, which clash with March's numbering; adds
`X2SlotItem.{h,cpp}` to Touches.
**Gate:** the header and equipment match June's layout in game; the radio toggles; title opens; log clean.

## Phase C3 - Stat panels and the socket total (Opus 5.5, high)

**Depends on:** C2. **Flag:** `SERV_IRUHADEV_JUNE_CHARINFO_STATS`.
ResetStat `3291cfb0` / `3291d2e0`, `32913460`, `32913df0`, basic `32915ab0`, option `32919430` (13 rows), resistances `32916160`, opacity `32915620`, the Lua
callers `3291eb50` / `3291f0c0`-`3291f960` / `32920210`, and the socket total `3291ba40` / `3291cee0` / `3291b4b0` (S7p's brief `kickoffs/skill-system-core/S7p.md`
is the reading list for it). Add the rows to Stage 33's map (`june2017_re/statcalc/`, or a note in the Stage 33 section if the map does not exist yet) as
"display: June (CharInfo C3)". C0: add EquipChanged `3291d2e0` and UpdateOptionStatFromSpecialSkill `32912a90`; `SocketOptionForm.lua` (data036) is already
June's with all 145 curve functions (no Lua work); ResetStat's socket group and UpdateResistanceStatUI's `GetEnchantStat` 4th argument (June `32358df0`) read
the Lu/Ciel form (stand-in 0): C3 decides whether March's 3-argument `GetEnchantStat` is June's namesake at form 0 or a `june_march_divergent.tsv` row.
**Gate:** the 13 option rows and resistances show; PvE / PvP switch changes numbers; a `_DEBUG` line per refresh prints the inputs and each printed value
(retired by the review); log clean.

## Phase C4 - Unit viewer, light, slots (Sonnet 5.5, high)

**Depends on:** C3 (same file). **Flag:** `SERV_IRUHADEV_JUNE_CHARINFO_VIEW`.
MsgProc `3291d390` (switching click: Lu/Ciel only, stand-in), UnitHandling `32912d90`, OnFrameMove `32914380`, viewer `329148d0` + `3291d360`, light
`329142a0` + UI manager `32150e30` (CharInfo branch; its Custom / MagicalCloset / Watch branches belong to their windows), and the slot paths (C0 map):
MouseUp `32917080`, OnDropAnyItem `32918410`, OnRClickedItem `32917fd0`, DrawSlotMouseOverImage `32913b90`, MouseDown `3291d830`, SetNowDragItemReturn
`3291d9f0`. MouseRButtonUp `329128e0` and GetSlotItemDesc `32916810` are the same as March (keep). `32914d70` is dead in June (not ported).
**Gate:** rotate / zoom the model, light on / off, drag-equip, right-click unequip, tooltips; log clean.
**Add-on (moved from june-march-divergence D1-3g, user 2026-10-29, scope rule):** the viewer's attribute-enchant family, June's two handle lists
(fire `+0xd90`, hand `+0xda0`) and the ~18 functions that read them. Flag `SERV_IRUHADEV_JUNE_CHARINFO_VIEW_ATTRIB_ENCHANT`, brief `kickoffs/june-charinfo/C4-enchant.md`.
Gate adds: the model in the U window shows the weapon's attribute sparks, none while riding or in an avatar emotion.

## Phase C5 - Actions that send packets (Sonnet 5.5, high)

**Depends on:** C4. **Flag:** `SERV_IRUHADEV_JUNE_CHARINFO_ACTIONS`.
Unequip-all `32918c70` (`CIM_ALL_EQUIPPED_ITEM_RELEASE`): per equipped item, the first empty bag slot, then `32916d90` = March's
`Handler_EGS_CHANGE_INVENTORY_SLOT_REQ` (already answered offline); no new wire id, no `X2OfflineDB` column. C0 moved switching (unit `+0xe40`, `0x4d9` /
`0x4fd`) to Stage 8 (Lu/Ciel only); C1 stubs it.
**Gate:** unequip-all moves every item to the bag (and refuses with a full bag as June does); `offline_packets.log` shows the slot-change REQs `HANDLED`.

## Review (Opus 5.5, high)

`/x2-phase june-charinfo C5 review` covers C1-C5: June vs ours line by line, the asset list against June's bytes, flagcheck ranges per flag, enumscan,
luabind, the deployed exe and the play-test lines; retire C3's `_DEBUG`.

## Phase Z - Close-out (Sonnet 5.5, medium)

`MODS.md` (flags, staged files and backups, revert steps), backlog Stage 34 closing note, the Stage 9 bullets (Watch window, transcend-UI items) checked,
STATUS, memory. No code.
