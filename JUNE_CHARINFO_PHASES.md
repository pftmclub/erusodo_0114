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

### C2 Done (2026-10-30, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_CHARINFO_SHOW`)

**Landed:** June's bodies inside March's functions (`X2UICharInfo.cpp`, March in `#else`): SetShow `3291df90`, SetTitle `32915100`, ResetNowEquipUI `329146d0`, UICustomEventProc `3291db60` (PROFILE / PASSWORD_* not handled); new `SetSwitchingView` (`32912c50`) with the slot show rule in `_CONST_CHARINFO_C2_`; `CX2SlotItem` reads `IS_FOR_MINOR_UNIT` (`GetIsForMinorUnit`); ctor members `m_bSwitchingView` (+0x25a), `m_bUnitClickedForSwitching` (+0x25b). Funcmap rows `(CharInfo C2)`.
- **SetShow:** nickname, guild, PvP win (STR 669), PVP_Grade, PvE win, PVE_Grade, char icon (T6c line kept under `JUNE_TRANSCEND_IMAGE`), radio forced Dungeon in a dungeon, EquipRelease (auth level > special user; offline level 3 shows it), resonance buttons, `CharacterSwitching*` (UT_LUCIEL only), inventory refresh at the tail; no Profile / PVP_SCORE / PVP_Total rows (June has none). Close hides the viewer, the item description and the slots.
- **SetTitle:** the title picture is index 1 of `g_pStaticInfo_Title` (March read `g_pStaticInfo`); no title-score row.
- **Kept as March (same in June):** ctor/dtor (GetDummyPos already guarded), level / ED / stone / item level, UIServerEventProc (empty).
- **Stand-ins (labelled in code):** PvE record 0 wins and grade hidden (UnitData lacks June +0xa00 / +0xa0c / +0xa0d); resonance flag (unit +0xaa9) false, so the button is drawn disabled with its guide; Lu/Ciel form byte false; `3286a350` / `3234fe90` (minor-unit item test, ring-in-second-slot remap) false / no-op; state 0x17 `XS_PVE_GAME` and 0xf `XS_GUILD_AGIT_GAME` have no March state; the achievement window June hides on close (`g_pData +0x5c8`, class unlocated here, Stage 31).
- **Left to later phases:** viewer lines in SetShow stay March's (`329148d0` / `31faa150` ApplyCustomize / `31fa7ef0`: C4); light `329142a0` (C4); Rose weapon UIs `32913f40` / `329141b0` (Stage 8); the +0x25c customize flag and the +0x260 vector (Stage 12, no reader yet); viewer flag +0xd80 in `32912c50` (C4).
- **Ghidra readings worth keeping:** slot show rule `32e22ce0`: minor slots show only in the switching view; BODY / LEG / HAND / FOOT / WEAPON_HAND always; other equipped slots only outside it. `3218b410` = `SetInventorySort( sort, page )`.

**Gates:** `flagcheck JUNE_CHARINFO_SHOW --worktree --files ...` files: 4 PASS, 1 DEFINE, 0 FAIL, 0 deleted, 0 non-ASCII, 0 EOL-mismatched, `RESULT: PASS`. `junediverge --check` `RESULT: PASS`. `enumscan --check` 0 A/B rows. `DEPLOY OK: X2_offline.exe is 25,851,392 B, dated 2026-10-30 16:39:52, md5 8f83d6a231d4c18a902f7400ed26a51a`. `EXESTR OK: 4 of 4` (wide control names) + `1 of 1` (`IS_FOR_MINOR_UNIT`). No Lua or kom change this phase.

**See it in game:** U window: header text, record rows (PvE 0), equipment in the right slots with no overlapping minor-unit slots, Dungeon / PvP radio switches the numbers, Title button opens the title window and the picked title shows in the window, EquipRelease radio drawn (no action until C5), resonance button greyed with its guide. No new log line.
**Open questions:** none.

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

### C3 Done (2026-10-30, Opus 5.5 high; flag `SERV_IRUHADEV_JUNE_CHARINFO_STATS`, + `_DEBUG`, `_EQUIP_GROUP`)

**Landed (`X2UICharInfo.{h,cpp}`, March in `#else`):** June's bodies in ResetStat `3291cfb0` (new `bool` argument, default false), GetDamageDecrement
`3291eb50` (def / ( def + 39.35 x level + 258.65 ), `322a8b60` inlined in `_CONST_CHARINFO_C3_`), UpdateBasicStatUI `32915ab0`, UpdateOptionStatUI
`32919430`, UpdateOptionStatFromSpecialSkill `32912a90`, UpdateBasicStatFromSpecialSkill `32913460`, SumSocketOption `3291b4b0`, GetMyEquippedSocketData
`3291ba40` (new equip-group argument, default 2). New: GetMyFormSocketData `3291cee0`, GetShownEquipStat `32913df0`, UpdateRecordGuideUI `32915620`.
Funcmap rows `(CharInfo C3)`. Nine June SKILL_ID names in `X2SkillTree.h` (`JUNE_ENUM_NAMES`). `boost::math::erfc` (boost 1.44, probe-compiled) for Maximize.
- **Readings.** June's `Stat` has 13 floats: extra [0..5], base [6..12] = HP, AtkP, AtkM, **[9] / [10] (a physical / magical attack spread March lacks)**,
  DefP, DefM. Socket data: +0xc move, +0x10 jump, +0x14 animation, +0x18 charge, +0x1c hyper time, +0xbc critical, +0xd8 / +0xdc add def / atk, +0x124 /
  +0x12c MP by attack / attacked, +0x134 / +0x138 specialization A / B, +0x13c maximize, +0x2f0 stat. Radio: +0x258 = `m_bShowPVPOption`, +0x259 =
  `m_bShowDungeonOption` (March's member order). Option rows: static `%5.0f` (critical clamped at 0, time `%3.0f%%`), guide = Final*Percent x 100 in the
  row's string + "\n(@1% Efficiency Stage)" (39999, Final*Coefficient x 100 + 0.1f); Add = two `13555` ranges ( atk x 1.5 x F x 1^0.65 / 50^0.65 );
  Charge / Time switch picture 0 / 1 and string 13556 / 29871, 13557 / 29872 for Add; Maximize adds 39433 = 1 - erfc( 2 ( r - 1 ) / sqrt 2 ) / 2;
  specializations: per-class strings 36704-36723 / 38430-1 / 43546-7 (Ara A "fffff", Lire B "fff", Ara B x20, Add B x50, Luciel A x10, Rose A x0.6).
  Basic: attacks / defences `%5d`, guides 39431 / 39432 = min, max, min + maximize x ( max - min ), max. Record guides: 42074 total PvP wins (unit +0x12c
  m_Win + +0x120, read as m_iPastSeasonWin [I]), 32067 PvE (0).
- **S7p answers.** Equipment uses the group argument (only fashion items and accessories are filtered; 2 = both forms); sets use 32350b80's own group
  (my unit's +0xe40). Sources in June order: equipment, sets, (Lu/Ciel sets), riding pet by UID (no riding-on test), title, party shared sockets
  (S7m's GetPartyShareSocketOptions), pet aura, premium buffs, (customize, Stage 12), LimitMaximum's caps. Item +0x8c / +0x9c: Stage 16.
- **GetEnchantStat 4th argument:** June `32358df0`'s 4th argument filters equipment by group ( 2 or == arg ). At form 0 every June-reachable slot is
  group 0 or 2, so March's 3-argument form is June's namesake there: no divergent row; UpdateResistanceStatUI stays March's (same strings, `  %d`, 500).
- **EquipChanged `3291d2e0`:** same as March at C3 (ResetStat( false ) is the default; it does call ResetIncreaseSkillLevelBySocket: tsv row corrected);
  its viewer calls `31fa9e60` / `31f94190` and `3291d360` are C4's.
- **`_EQUIP_GROUP` (found here, playability):** S7k's GetEquipGroup reads June slot ids, so March's NESI_ONEPIECE_FASHION (20) read as June's minor-unit
  group 1 and dropped a onepiece costume from every form-0 sum (this window, HUD max HP). It now returns group 0 for slot 20 (June's onepiece is 35).
  Stage 8 removes it with the NESI renumber.
- **Stand-ins (labelled):** Lu/Ciel form 0 (unit +0xe40, viewer +0xd80); Stat [9] / [10] spread 0 (attack range min = max); 32358970 ability terms 0;
  PvE grade 0 for PvE-rank-linked sockets; PvE record 0.
- **Not ported (dead in June):** the discarded Final / Intermediate calls in Maximize, MP and specialization rows.
- **Stage 33:** display rows added to its section ("display: June (CharInfo C3)"). JUNE_BUGS 483.

**Gates:** flagcheck `--worktree` STATS / DEBUG / EQUIP_GROUP: RESULT PASS each (0 FAIL, 0 deleted, 0 non-ASCII, 0 EOL-mismatched).
`junediverge --check` RESULT: PASS (1 allow row: SumSocketOption GT_PVP, June `@3291b71a` tests type 1 only). `enumscan --check` 0 A/B rows.
`DEPLOY OK: X2_offline.exe is 25,861,632 B, dated 2026-10-30 17:30:13, md5 adb2bd5e66c2791e6c9dc44a3c2b97e7`.
`EXESTR OK: 3 of 3` (wide: STATS ResetStat / option, ButtonPvpOpacity) + `2 of 2` (narrow: CalculateFinalCriticalCoefficient, CalculateFinalAinSpecializationBCoefficient).

**See it in game:** U window: every option row shows a number and a hover guide ending in "(N% Efficiency Stage)"; Maximize / MP / specialization rows
filled; attack hover shows the range lines; Dungeon / PvP radio changes the numbers of PvP- / dungeon-only sockets; one `[JUNE_CHARINFO] STATS` group
(ResetStat, basic, option, record) per refresh. **Open questions:** none.
**Play-test (2026-10-30, md5 adb2bd5e, run after deploy):** 75 `[JUNE_CHARINFO] STATS` lines, 0 EXCEPTION, no Lua curve error. Two level-99 Aisha saves:
every refresh logs basic / option / record / ResetStat; the radio flips pvp 0/1 dungeon 1/0 (numbers equal: no PvP- or dungeon-only sockets on that gear);
unequip / re-equip drops and restores hp / attacks / crit / decline / moving / MP (e.g. hp 292183 -> 247939 -> 292183); the second save fills charge 1430 /
time 11. Maximize and specializations read 0 (no such sockets); PvP record 0 (offline). C3 done.

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

### C4 Done (2026-10-30, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_CHARINFO_VIEW`; the viewer attribute-enchant add-on is NOT done, it is the Opus half)

**Landed (`X2UICharInfo.{h,cpp}`, `X2UIManager.{h,cpp}`, `Always.h`; March in `#else`):** OnFrameMove `32914380` (no Invalidate* when the mouse is off the
dialog), UnitHandling `32912d90` (a drag with mouse X delta != 0 sets the `+0x25b` latch = `m_bUnitClickedForSwitching`), MsgProc `3291d390` (WM_LBUTTONUP now
runs rotation bookkeeping, the Lu/Ciel click-switch stub `[JUNE_CHARINFO] STUB unit click switching -> Stage 8`, latch + click reset, and only then the
mouse-over test), light: new `CX2UICharInfo::SetLight` `329142a0` + `CX2UIManager::SetCharWindowDarkBackground` `32150e30` (CharInfo branch; the flag June keeps in
g_pMain `+0x2ac` lives in CX2UIManager) + CIM_LIGHT_ON / OFF cases (return false as June) + SetShow applies it, DrawSlotMouseOverImage `32913b90` and
OnDropAnyItem `32918410` (the slot search skips minor-unit slots: NEW_4's `IS_FOR_MINOR_UNIT` slots sit after the normal ones with the same equip position, so
the drag frame used to land on a hidden one), OnRClickedItem `32917fd0` and MouseUp `32917080` (June has no `IsTransformed` test). Funcmap rows `(CharInfo C4)`,
`charinfo_functions.tsv` verdicts, JUNE_BUGS 484.
- **Same as March, kept (no code):** MouseDown `3291d830` (viewer RemoveEqip's 3rd arg 0 = March's default), SetNowDragItemReturn `3291d9f0` (June also calls the
  skill-tree window's viewer refresh `328eea70`; March's skill tree has no viewer), MouseRButtonUp, GetSlotItemDesc, EquipChanged `3291d2e0` (June's `31fa9e60` =
  UpdateEqip( true ); `31f94190` is Lu/Ciel), ResetUnitViewerUI `329148d0` at form 0 (`SetUnit( unit, 7 )` = UVS_CHARINFO, dummy 2 / 3 positions, SetFixed, ApplyMotionOffset
  false are all March's; GetDummyPos is bounds-safe), SetShow's viewer lines (`31faa150` Lu/Ciel; `31f927b0` SetHideSelectUnitWeapon is not in the C4 table).
- **Routed (not ported, no partial copy):** MouseUp's cursor-mode switch is June's own 33-state cursor set with UIInventory handlers (March's CX2Cursor has 20 in
  another order; June's 0xc / 0xd = March's DELETE_ITEM / SEAL_ITEM messages 4426 / 4425) -> backlog Stage 9 (bullet added). Lu/Ciel form byte `+0xd80` / `+0xd81`,
  `31f93a10`, `31faa150`, `31f94190`, `3291d360` (only Lu/Ciel callers, not created), OnDropAnyItem's `3234f4d0` (STR 39493) and `321693b0` -> Stage 8 (bullet added).
- **Stand-ins (labelled):** Lu/Ciel click switching (stub log), DrawSlotMouseOverImage's `3286a350( uid )` = false, OnDropAnyItem's viewer-form accessory case = form 0.

**Gates:** `flagcheck JUNE_CHARINFO_VIEW --worktree`: RESULT PASS (3 PASS, 1 PASS~, 1 DEFINE, 0 FAIL; 3 deleted lines = three comments I rewrote inside C2's
`_SHOW` region; 3 EOL-mismatched inserts in MouseUp / OnRClickedItem where the file mixes LF and CRLF). `junediverge --check` RESULT: PASS. `enumscan --check` 0 A/B rows.
`DEPLOY OK: X2_offline.exe is 25,862,656 B, dated 2026-10-30 18:24:23, md5 e51c3dd2a32bd3e4c46d2730da0a5967`. `EXESTR OK: 2 of 2` (wide: VIEW light dark, STUB unit
click switching) + `3 of 3` (narrow: CharWindowCharLightSet, Light_On, Light_Off). No Lua staged, no kom change.

**See it in game (hand-off in the session):** U window: Light button toggles the dark / lit model background and survives close / reopen; the model rotates by
drag and eases back; dragging an inventory weapon / armor over the window frames the real slot; drag-equip, right-click unequip and the tooltips work; log lines
`[JUNE_CHARINFO] VIEW light dark N` at each open and click. **Open questions:** none. Watch for one June quirk (JUNE_BUGS 484): a tooltip can stay when the mouse leaves.

### C4 add-on Done (2026-10-30, Opus 5.5 high; flag `SERV_IRUHADEV_JUNE_CHARINFO_VIEW_ATTRIB_ENCHANT`)

**Landed (`X2UnitViewerUI.{h,cpp}`, `Always.h`; June's body inside March's function, March's in `#else`):** the seven scalar attribute-enchant handles
become June's two lists `m_vecSeqWeaponFire` (`+0xd90`) and `m_vecSeqWeaponWind` (`+0xda0`, the hand sparks); ctor `31fab020` (clear + reserve 3 / 4).
- **Creators** `31f9d1f0` / `31f9d3b0`: variadic `( int iNameCount, ... )`, one handle per name, emit 0, push_back (whole definitions swapped: the parameter list changes).
- **EDT switches** `31f9ea20` / `31f9eeb0` / `31f9d570`: case tables read by emulating each switch over every June `EDT_ENCHANT_*` value (unicorn, sinks on the
  creators). Weapon names are March's; June adds the `TRIPPLE_*` types to the element groups, `DOUBLE_<elem>_DOUBLE_*` / `TRIPPLE_<elem>_*` to the master groups and
  `QUADRA_<elem>_MASTER` -> `Property04_M_*`. The hand switch is new: `Attribute_Piercing / Light / Dark` `_Ring_1` + `_Particle` (single), `Ring_2` / `4` + `Particle`
  (master or double), `Ring_1` / `2` / `4` + `Particle` (double master, triple), `Ring_1`-`4` + `Particle` (`QUADRA_*_MASTER`). All 21 names are already in data036's
  `UIMajorParticle.txt`. **Play-test fix (2026-10-30):** the hand rings drew as flat solid rings because the ring mesh `Attribute_Light_1.x` (June's bytes) asks for
  `Attribute_Hand_White.tga` -> `.dds`, which March lacked; staged June's `Attribute_Hand_White.dds` and `black_02.dds` (a `Property04` quadra sequence) into
  data002 (`kom.py stage-june`, `PACK OK data002`: +2 added, 0 removed, 0 changed; copy owed). The census of the 21 sequences' meshes and textures is clean after it.
- **Delete** `31f9da10` (destroy both lists, clear). **Hand sparks** `31f961a0`: five rates per class from the disassembly (floats 10 / 15 / 30 / 60 / 25): Elsword,
  Aisha, Raven, Elesis `Dummy1_Rhand` 10/30/60/25/10; Rena `Dummy2_Lhand` 15/30/60/25/15; Eve `Dummy2_Lhand` 10/10/10/25/10; Chung `Bip01_R_Hand` 10/30/60/25/10
  (March: `Dummy1_Rhand`); Ara `Dummy3_Weapon`, Add `Weapon02` 15/30/60/25/15. Rate i goes to handle i.
- **Frame moves** Eve / Ara / Add / Bow `31f92de0` / `31f92ec0` / `31f92f70` / `31f932e0` (list [0], March's bones) and Sword `31f9daf0` (index 0 centre, 1 trace end,
  2 trace start, 3+ centre): the hide test is `CanShowWeaponEffect` (`31f91230`, D1-3c) instead of March's riding / `FIX_UPGRADE_WEAPON_PARTICLE` test.
- **Draw** `31fa00b0` and **`SetShowAttribEnchant`** `31f91f90`: loops over both lists, the hand list without the z test, as March.
- **Compared equal, no edit:** `OnFrameMove` `31fa8ce0` and `SetUnit` `31faa6f0` enchant parts (only June-only types 10-12 differ), dtor `31fab920`.
- **Kickoff corrections:** `31f97960` is June's vslot 0x10 post-effect draw (engine R4a / R4i), not March's `Render_DrawForAvartaScreenCapture`; that one is under
  `AVATAR_IMAGE_TEST` (off) with no June twin found, so it got March's draw over the lists only to compile if the flag returns. The `#error` names
  `..._DIVERGE_UPGRADE_WEAPON_PARTICLE` (where the viewer's `CanShowWeaponEffect` lives) and `..._JUNE_ENCHANT_PARITY` (the triple / quadra names), both proved
  DEFINED at the end of `Always.h` and after `X2Lib/StdAfx.h` by `cl /EP`.
- **Routed:** types 10-12 (viewer at-hand cases, frame moves `31f93050` / `31f93200` / `31f940b0`, Rose -> ForBow in SetUnit): checklist 7as, updated.
- **June bug kept:** JUNE_BUGS 486 (sword sparks: a missing trace frame is looked up again only while the fire list is empty).
- **Bookkeeping:** funcmap rows `(CharInfo S34)` x10; `MODS.md` entry; checklist 7as.

**Gates:** `flagcheck JUNE_CHARINFO_VIEW_ATTRIB_ENCHANT --worktree --files`: RESULT PASS (`X2UnitViewerUI.cpp` +727, `.h` +10, `Always.h` DEFINE +13; 0 deleted,
0 non-ASCII, 0 EOL mismatch). `junediverge --check`: RESULT: PASS. `enumscan --check`: 0 A/B rows.
`DEPLOY OK: X2_offline.exe is 25,863,680 B, dated 2026-10-30 19:48:24, md5 54293f3bbb9a43024ecd07e536c2f026, equal to X2/US_SERVICE/x2.exe (2026-10-30 19:48:17); built at main ef1f3805 + 3 uncommitted code file(s).` (all three are this phase's.)
`EXESTR OK: 4 of 4 found in X2_offline.exe, 25863680 B, dated 2026-10-30 19:48:24, md5 54293f3bbb9a43024ecd07e536c2f026` (wide: `Attribute_Piercing_Ring_1`, `Attribute_Dark_Ring_3`, `Weapon_Elsword_Property04_M_Fire`, `Bip01_R_Hand`).

**See it in game:** U window with an attribute weapon: the element glow on the weapon (Elsword's sword shows three sequences along the blade) and the
`Attribute_*_Ring` sparks on the hand; Chung's hand sparks now sit on the right hand bone. The U window always shows them (June's charinfo state skips the riding /
emotion test); a village viewer hides them while riding or during an avatar emotion. No log tag. **Open questions:** none.

**Play-test (2026-10-30, after deploy md5 54293f3b + data002 copy):** passed. 19 characters with three of one attribute on the weapon (fire / ice / poison
glows, wind / light / dark ring sets); the user confirmed the flat hand rings against the wiki's June look. Logs: 0 EXCEPTION; ErrorLog's
`OpenTexture ATTRIBUTE_HAND_WHITE.TGA` and `OpenXET ATTRIBUTE_LIGHT_1.XET` are June's own misses (the ring mesh names a `.tga` June ships only as `.dds`, and
June ships no `.xet`; no extension fallback in either loader, as ATLAS A11 found), so the staged `.dds` is unused but harmless. C4's light toggle logged
`VIEW light dark 0` -> `1`. Unrelated lines left alone: `E_DISABLE_HB_CHECK_REQ` UNHANDLED x3, `AddGroupChainQuest` unbound, the JUNE_FX applied-skill line.

## Phase C5 - Actions that send packets (Sonnet 5.5, high)

**Depends on:** C4. **Flag:** `SERV_IRUHADEV_JUNE_CHARINFO_ACTIONS`.
Unequip-all `32918c70` (`CIM_ALL_EQUIPPED_ITEM_RELEASE`): per equipped item, the first empty bag slot, then `32916d90` = March's
`Handler_EGS_CHANGE_INVENTORY_SLOT_REQ` (already answered offline); no new wire id, no `X2OfflineDB` column. C0 moved switching (unit `+0xe40`, `0x4d9` /
`0x4fd`) to Stage 8 (Lu/Ciel only); C1 stubs it.
**Gate:** unequip-all moves every item to the bag (and refuses with a full bag as June does); `offline_packets.log` shows the slot-change REQs `HANDLED`.

### C5 Done (2026-10-30, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_CHARINFO_ACTIONS`)

**Landed (`X2UICharInfo.{h,cpp}`, `Always.h`; no `#else` needed, the case and the member are June-only):** new `CX2UICharInfo::ReleaseAllEquippedItems` = June
`32918c70`, called from `CIM_ALL_EQUIPPED_ITEM_RELEASE` (`3291dc91`, answers false). Body: only for auth level above `XUAL_SPECIAL_USER` (June `+0x2c > 1`; same gate
as C2's EquipRelease button); walks a copy of the slot list (June vslot 15 = `m_SlotList` copy); per `ST_EQUIPPED` slot that is enabled, `IsResetItemUI` and has a templet:
`CheckIsReady` -> unready box, send-start flag, then in `XS_DUNGEON_GAME` / `XS_BATTLE_FIELD` (June's 0x17 / 0x1a states do not exist in March): no game or unit, **IsTransformed**
(June tests unit `+0x8c0c` here; its OnRClickedItem does not), hp <= 0 -> STR_ID_2691 box, busy state; then `ToggleUI( UI_MENU_INVEN, true )`, `ChangeInventoryTabByUid`,
and the first empty slot of that tab from a per-tab cursor (EQUIP / ACCESSORY / AVARTA, June `local_110 / 10c / 114`) because the REQ is not answered yet; sends March's
`Handler_EGS_CHANGE_INVENTORY_SLOT_REQ` (= June `32916d90`, already answered), `DestroyItemUI`, `InvalidSlotDesc`. A full tab sends nothing and says nothing (June). No new wire id,
no `X2OfflineDB` change, no Lua, no kom. Switching stays Stage 8's stub (C0). JUNE_BUGS 485 (the refusal box repeats per item). Funcmap + `charinfo_functions.tsv` rows updated.
Debug line under C3's `_DEBUG`: `[JUNE_CHARINFO] RELEASE item <tid> equip slot <n> -> sort <s> slot <i>`.

**Gates:** `flagcheck JUNE_CHARINFO_ACTIONS --worktree`: files 2 PASS, 1 DEFINE, 0 FAIL, 0 deleted, 0 EOL-mismatched; RESULT PASS. `junediverge --check` RESULT: PASS.
`enumscan --check` 0 A/B rows. `DEPLOY OK: X2_offline.exe 25,864,192 B, 2026-10-30 19:08:33, md5 b3e4ef33762de4cf2b31074a2b55cefa`. `EXESTR OK: 1 of 1 found` (wide `[JUNE_CHARINFO] RELEASE item`).

**Save prepared (backup `db_backup/els_db_20261030_190902_pre_manual.sql`):** unit 32 MagicKnightT got a worn Bronze Sword and 24 Intermediate Bronze Top Piece (equip bag full) for the full-bag check.
**Play-test (2026-10-30, passed):** unit 25 three runs, 7 slot-change REQs `HANDLED` + ACK, 0 EXCEPTION; weapon -> equip slot 15, accessories advance per-tab cursor, Chronicle -> avatar slot 0; unit 32 full bag: nothing moved. Check 3 (button hidden below auth 2) not tested.
**Open questions:** none.

## Review (Opus 5.5, high)

`/x2-phase june-charinfo C5 review` covers C1-C5: June vs ours line by line, the asset list against June's bytes, flagcheck ranges per flag, enumscan,
luabind, the deployed exe and the play-test lines; retire C3's `_DEBUG`.

### Review - Done (2026-10-30, Opus 5.5 high; verdict PASS, no transcription error found)

**Code change:** `_DEBUG` retired (define commented out in `Always.h`, house pattern; the five STATS / RELEASE log sites stay behind it). No other edit.
**Read against June (Ghidra + raw disassembly), line by line:**
- **UICustomEventProc `3291db60`** (vslot 55, no Ghidra function): title-manager pre-forward (`g_pData +0x28c`, `326f7d20` / `32703a70`) = ours. Jump table `3291df1c`
  with byte map `3291df50` decoded: 10000 SetShow( false ), 10001 OpenTitle, 10003 radio (index 0 -> dungeon `+0x258 = 0x100`, 1 -> PvP, then ResetStat( 0 )),
  10007 customize, 10008 / 10009 switching, 10010 `32918c70` -> false, 10011-10014 / 10015-10018 Rose, 10020 resonance, 10021 / 10022 `32150e30( 0 / 1 )` -> false;
  10002, 10004-10006, 10019 default. Our cases match; stubs return true where June opens a window (no effect). 83010 is outside June's range (the achievement
  window takes it); March sends every custom message to CharInfo while it is shown (`X2UIManager.cpp:1026`), so our stub fires.
- **SetShow `3291df90`:** call order matches (ResetStat / equip / ED / level / stone / viewer / PlayByMotionType / SetShowObject / SetHideSelectUnitWeapon / UIOpened
  / play guide, resonance, switching buttons, `+0x25b` reset, light, Rose stubs); radio forced dungeon in states 0xb / 0x17; EquipRelease at auth `>= 2`
  (`cmp [+0x2c],2 / jl`) = `> XUAL_SPECIAL_USER`; close path and the slot tail (LostItemUI when hidden, SetShow / SetEnable( m_bShow )), dialog, inventory re-sort.
- **SetTitle `32915100`**, **ResetNowEquipUI `329146d0`** (no minimap call; slot `+0x44` minor flag, `31d41d10` = IsCreatedItemUI), **SetSwitchingView**: match.
- **Light `329142a0` / `32150e30`:** match (pictures 0-2 = flag, 3-5 = not flag, 6 untouched; Light_On shown while dark). Default lit = June's ctor (`@32ee7b7b`).
- **C3:** socket group filter `3291ba40` (fashion / accessory, group 2 = both) re-read: match. The rest was C3's own Opus reading plus its numbers play-test.
- **C5 `32918c70`:** gate, slot copy (vslot 15), `+0xc` = `m_bEnable`, `+0x10` = ST_EQUIPPED (March `CX2Slot` layout), CheckIsReady, send-start flag, states
  0xb / 0x15 = March `XS_DUNGEON_GAME` / `XS_BATTLE_FIELD` (0x17 / 0x1a absent), IsTransformed `+0x8c0c`, hp > 0 else STR 2691, busy, ToggleUI, tab, per-tab cursors
  for sorts 1 / 2 / 7, send, DestroyItemUI, InvalidSlotDesc: match.
- **C4 add-on, re-run independently:** my own unicorn harness over June's three EDT switches (`31f9ea20` / `31f9eeb0` / `31f9d570`), every June `EDT_ENCHANT_*`
  value, diffed against the C++ cases: 66 / 66 / 66 types produce names, 0 mismatches; all 78 C++ `EDT_ENCHANT_*` values equal June's. Hand sparks `31f961a0`:
  floats `3344a51c` 10, `33441b9c` 30, `33458fa0` 60, `3345b370` 25, `3345b368` 15; per class and bone exactly as ported. JUNE_BUGS 486 matches `31f9daf0`.

**Hygiene (one run per flag, own commit range):** all `RESULT: PASS`, `0 FAIL, 0 ERROR`, every flag 0 deleted / 0 non-ASCII / 0 EOL-mismatched in its own files.
DLG `files: 3 PASS, 1 DEFINE`; SHOW `4 PASS, 1 DEFINE`; STATS `2 PASS, 3 OTHER` (Always.h block; `X2Inventory.cpp` = `_EQUIP_GROUP`; `X2SkillTree.h` =
`JUNE_ENUM_NAMES`); EQUIP_GROUP `1 PASS, 4 OTHER` (same commit); DEBUG `0 PASS, 13 OTHER` (its sites sit inside STATS / ACTIONS code, attributed there); VIEW
`2 PASS, 1 PASS~, 1 DEFINE, 1 OTHER` (the `-3` = C4's three rewritten comments in C2's `_SHOW` region, as its note says); ACTIONS `2 PASS, 1 DEFINE`;
VIEW_ATTRIB_ENCHANT `2 PASS, 1 DEFINE`. Review edit: `flagcheck JUNE_CHARINFO_DEBUG --worktree --files KTDXLIB/Always.h`: `1 DEFINE`, 1 deleted
line (the define, now `//#define`), `RESULT: FAIL` only for "not #defined anywhere": the intended state of a retired debug flag (as `JUNE_BUFF_DEBUG` and the rest).
`junediverge --check-all` RESULT: PASS. `enumscan --check` (8 files) 0 A/B rows. `luabind check` NEW_4 / NEW_3 / NEW_2 / NEW / DLG_UI_CHARACTER: 0 unbound, PASS.
`ledger --check` PASS, `core --check` PASS. **Assets:** 17 staged files (5 Lua in data034, 12 textures in data057 / 056 / 063 / datai021 / datai022 / data002)
identical to June (`stage-june` / `adopt-june --dry-run`: 0 new, 0 replaced); all 8 packed koms equal the deployed ones (data002 copy done).

`DEPLOY OK: X2_offline.exe is 25,861,120 B, dated 2026-10-30 21:19:29, md5 e7c166a1201d1fc9d7d3c466e4e56629, equal to X2/US_SERVICE/x2.exe (2026-10-30 21:19:22); built at main 8aff342d + 1 uncommitted code file(s).`
`EXESTR OK: 2 of 2 absent` (wide `[JUNE_CHARINFO] RELEASE item`, `[JUNE_CHARINFO] STATS`); `EXESTR OK: 2 of 2 found` (wide `VIEW light dark`, `STUB CIM_CUSTOMIZE`).

**Routed:** the light as June's persisted game option `0x15` -> Stage 9 bullet (option window). **Kept:** the `[JUNE_CHARINFO] VIEW light dark` line (one per
open / click, C4's play-test evidence). **Play-tests still owed:** C1, C2 (C3 / C4 add-on / C5 passed; C4's drag / equip checks untested). **Open questions:** none.

## Phase Z - Close-out (Sonnet 5.5, medium)

`MODS.md` (flags, staged files and backups, revert steps), backlog Stage 34 closing note, the Stage 9 bullets (Watch window, transcend-UI items) checked,
STATUS, memory. No code.
