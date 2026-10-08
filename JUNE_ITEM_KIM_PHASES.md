# June item.kim adoption: March loads June 2017's item.kim

Plan key `june-item-kim` · branch `mods/june-buff-port` · flags `SERV_IRUHADEV_JUNE_ITEM_KIM` (the record layout) and
`SERV_IRUHADEV_KIM_LAYOUT_CHECK` (the structural check), both in `KTDXLIB/Always.h` · log tag `[JUNE_KIM]` (ErrorLog.txt) ·
kickoffs `kickoffs/june-item-kim/` · board: the `june-item-kim` block in `STATUS.md` · backlog Stage 13 in
`JUNE_PORT_BACKLOG_ORDER.md`. Created 2026-10-07.

## Context

The user will replace March's `item.kim` with June's (said 2026-10-07, june-effect C9g), so that June item data, and every June
system keyed on it, works without gaps. Today those systems look inert only because their ids are missing:
- none of C9f-2's 5 equip-effect items is in March's kim;
- 13 of the 1,270 `CharCustomData` ids are in March's kim, and 1,066 are in June's;
- backlog Stages 11 and 12 both wait on this plan.

**The swap is not a data drop-in.** A planning session on 2026-10-07 compared the two files byte by byte:

| | March `24965799\kom_contents\data036_kom\item.kim` | June `237311\14JUN17\data\data036_kom\item.kim` |
|---|---|---|
| Size | 21,594,896 B (the buff Phase 13 patched copy; pristine in `march_data\data036\`) | 67,557,756 B |
| Header | `KIM `, version `0x1201` | **the same** `KIM `, `0x1201` |
| Items / sets | 49,837 / 3,360 | 111,428 / 6,740 |
| Record stride | **188** | **228** (+40 B, inserted mid-record) |

- **March misreads June's file without any error.** `CX2ItemManager::DoMemoryNotEncrypt` (`X2Lib/X2ItemManager.cpp:338-358`) checks
  only three things: the magic, the version, and `nSize >= header + N*(4 + sizeof(KItemFormatTemplet))`. June's file passes all three.
  March then steps through it at 188 bytes per record, so every record after index 0, and the whole set table, is read from the wrong bytes.
- **Approach: port June's 228-byte record verbatim behind a flag, and adopt June's file whole.** This follows June-wins-at-the-same-filename
  and no-partial-June-updates.
- **Rejected:** a converter that rewrites June's records into March's 188-byte layout. It throws away the 40 June-only bytes that the
  Stage 11/12 systems read.

## Rules

- **Two flags, toggled together with the data.**
  - `SERV_IRUHADEV_JUNE_ITEM_KIM` switches the record layout.
  - `SERV_IRUHADEV_KIM_LAYOUT_CHECK` is independent and stays on. Its job is to make a file/layout mismatch, in either direction,
    fail loudly instead of misreading. Original code stays in `#else`.
- **CP949 files.** `X2Item_Preprocessing.inl`, `X2ItemManager.cpp` and `X2ItemManager_Preprocessing.cpp` are ISO-8859/CP949, so edit them
  with `tools/bytepatch.py` only.
- **June verbatim.** Field names, order and widths come from June's binary (K0), never from guesses about the data. June quirks are kept,
  each with a `JUNE_BUGS.md` row.
- **Out-of-range June values are guarded, never clamped.** These are the unit types and classes of June-only characters, item type 14, and
  similar. The guard skips the item and logs once per kind, with a count. Clamping into a March slot would show wrong data.
- **Kom packing is Claude's job; deploying it is not unasked.** Run `/elstools pack data036` yourself. **Ask before every
  `/x2-kom-packed copy data036`.**
- **The save is real data.** Back up `els_db.sql` to `db_backup/` before any change that touches it (allowed without asking).
- **Offline only.** No server sync questions.

## Starting context

Every repeated step is a tool (`tools/README.md`). Do not hand-type msbuild, unifdef or log greps.

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_ITEM_KIM_PHASES.md "<heading>"` |
| Read/diff a kim (from K0 on) | `python tools/kim.py dump / diff / census / assets ...` |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply <spec>` (x2-byte-edit) |
| Flag hygiene | `python tools/flagcheck.py SERV_IRUHADEV_JUNE_ITEM_KIM --worktree --files <files>` (and the same for `..._KIM_LAYOUT_CHECK`) |
| Build + deploy | `python tools/build.py --deploy` (background; x2-build-deploy) |
| Pack / deploy data036 | `/elstools pack data036`, then ask, then `/x2-kom-packed copy data036` |
| Play-test logs | `python tools/logs.py --tag JUNE_KIM` (x2-check-logs) |
| June client | Ghidra through the `ghidra` skill (read-only); record rows in `june2017_re/funcmap.tsv` |
| June Lua | `june_src_fixed/data036_kom/*.src.lua` only |

Paths:
- Game dir: `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data`. Never launch the game; ask the user.
- June kim: `F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\data036_kom\item.kim`.
- Staged March kim: `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents\data036_kom\item.kim`.

## Established facts (verified 2026-10-07, planning session)

**File format.**

```
+0   'KIM '  u32 version=0x1201  u32 NumItems  u32 NumSetIDs           (16 B)
+16  u32 itemID[NumItems]                 sorted; GetItemTemplet = lower_bound
     KItemFormatTemplet rec[NumItems]     record i <-> itemID[i]; dword +0 = item id
     u32 setID[NumSetIDs]                 sorted
     KItemFormatSetItemData[NumSetIDs]    20 B: setID, maxLevel, ofsName, ofsItemIDs, ofsNeedPartsNumNOptions
     pool: WORD-len UTF-16 strings, stat (5 floats), [WORD n][WORD buffId...], special abilities (24 B each),
           socket / random-socket int lists, particle/mesh pairs (bit 31 = jiggle), Extra pairs, set lists
```

- **Where the code is.**
  - Struct: `X2Lib/X2Item_Preprocessing.inl:428-917`, `#pragma pack(1)`, `MAX_MODEL_COUNT_A_ITEM = 6` at `:4`.
  - Writer and lookup: `X2Lib/X2ItemManager_Preprocessing.cpp`.
  - String helpers: `X2Lib/X2Item_Preprocessing.cpp`.
  - The buffer is copied once (`vector::assign`) and cast in place. There are no fixups: every offset is from the file start, and 0 means absent.
  - The only size check is `sizeof/4*4 == sizeof`. Nothing pins 188.
- **June is not truncated.**
  - The sizes equal the kom index (`june2017_re/assets_cache/inspect/14JUN17_data_data036.kom_*.json`).
  - The last set's option array ends exactly at EOF.
  - No offset points past the end.
  - `item_LA.kim` has the same ids and value columns; only its strings and offsets differ.
- **The June record, as far as probing the data shows.** Only the starred rows are confirmed; K0 replaces this table with the binary's.

| June | Contents | vs March |
|---|---|---|
| +0..+43 * | id, flags, flags2, level, quantity, repair, price, PvP price, set id, AddMaxMP | same |
| +44 / +48 / +52 | float / int / a second copy of the id | new, 12 B |
| +56 * .. +88 | Name, Description, DescriptionInShop, AttachFrameName[6] | March +44.. |
| +92..+104 | 4 new string slots; +100 holds dates like `"2017-01-25 9:00"` | new, 16 B |
| +108..+128 | ModelName[6] | March +80.. |
| +132 / +136 | always 0 in this file | new, 8 B |
| +140..+160 | the 6 March strings (TextureChangeXET, AniXET, AniName, ShopImage, DropViewer, DescInSkillNote) | March +104.. |
| +164..+196 | the 9 pool offsets, buff factors at +168 * | March +128.. |
| +200..+223 | the March tail (endurance, summon NPC, the BYTE block, Extra counts) | March +164.. |
| +224 | a new byte (0/1), then 3 B of padding | new, 4 B |

- **June values outside March's ranges:**
  - unit type up to 12 (`UT_END` = 10);
  - unit class up to 202 (March's max is 122);
  - item type 14 on 1,207 items (`IT_NUM` = 14);
  - `flags2` bit 2 set on 2,582 items (March has bit 0 only);
  - Extra arrays with up to 13 unit-type entries (March: 9);
  - grade 6 fits, because `IG_LEGEND` is on via `SERV_IRUHADEV_JUNE_ENCHANT_PARITY`.
- **Ids.**
  - Every March id is in June, and 61,591 ids are new.
  - The save holds 282 distinct item ids, and all are in both files.
  - Of 2,360 `cash_product` ids, 294 are missing from March and present in June. `X2OfflineCashShop.cpp:85` drops those rows today.
  - Of 91 `GBASE_ITEM` ids, 81 are in March and all are in June. The 10 missing are class row 9, marked "unreachable" at
    `X2OfflineInventory.cpp:1212`.
  - The Heretic potions (85003830/1) already read buff factors 481/482 at June +168, so buff Phase 13's 2-byte patch is moot.
- **Load path.**
  - US runs `OpenItemScriptFile( L"Item.kim", ... )` (`X2Data.cpp:1734/1744`) through `LoadAndDoMemory`, which XOR-decrypts by file name
    and then zlib-decompresses.
  - If a loose `Item.lua`/`SetItem.lua` exists, the kim is skipped, because `MASS_FILE_FIRST` is off.
  - If the kim fails, the client logs `KEM_ERROR72` and builds the file in memory from Lua (`KProxy` -> `KProxy2` -> `PostProcess`).
  - `X2Main.cpp:1032` deletes a loose `item.kim` at startup, so the file must ship in `data036.kom`. It already does: the manifest
    entry is `"item.kim": "2"`, and buff Phase 13 packed it.
  - March never references `item_LA.kim`.
  - On the kim path `SetItem.lua` is not loaded: sets come from the kim.
  - June ships no `Item.lua`. It has `SetItem_LA.lua` and `SetItemGroup.lua` (`g_pCX2SetItemManager:AddSetItemGroupData`, a separate
    June system), and `ItemManager.lua` (`SetItemLock_MaxDay/EDPerDay`).
- **Consumers.**
  - About 1,069 `GetItemTemplet(` calls exist, and about 531 dereference the result unchecked.
  - Every offline lookup null-checks and logs: `CX2OfflineInventory::Templet` writes `ITEM WARNING no templet`. The checks are in
    CashShop `:85`, RandomItem `:290`, DropTable `:284`, and Handlers_Room.
  - `CX2SocketItem::GetSocketData` returns NULL for an unknown option id.
- **Generator.** `X2ScriptPreprocessingTool/` compiles the client's own `X2ItemManager_Preprocessing.cpp` and `X2Item_Preprocessing.cpp`
  under `X2OPTIMIZE_SCRIPT_PREPROCESSING_TOOL`. It is not built here.

## Phase K0 - June's record from the binary, and tools/kim.py

**Scope.** Identify only. No game code, no flag, no build.

**Run with:** Opus 5.5 high.

**Read list:** Context, Starting context, Established facts.

1. **Find June's loader in Ghidra.** Search for the `KIM ` magic (`0x204d494b`), the `0x1201` compare, and ×`0xe4` index arithmetic.
   Record June's `DoMemoryNotEncrypt` equivalent (did June add a check?), `GetItemTemplet`, the set-table reader and `ResetItemManager`.
2. **Map all 228 bytes.**
   - For each templet accessor June has, record the offset, type and June address.
   - Name every one of the 40 new bytes from its xrefs.
   - Also answer:
     - What does `flags2` bit 2 mean?
     - What is item type 14 (June `Enum.src.lua`)?
     - What width does the Extra per-unit-type data have in June?
     - Did `MAX_MODEL_COUNT_A_ITEM` change, and is June's `KItemFormatSetItemData` still 20 B?
3. **Route every new field.**
   - (a) **Port in K2** if something March already has reads it.
   - (b) **A backlog stage** otherwise: 11 (division), 12 (customize), 8 (new characters), or a new stage. Follow the rule that an
     unowned June branch gets its own phase.
   - (c) **The character-port checklist** if it is per-class material.
4. **For the record:**
   - how June chooses `item_LA.kim` / `SetItem_LA.lua`;
   - whether `ResetItemManager` loads `SetItemGroup.lua` / `ItemManager.lua` beside the kim.

   US keeps `item.kim`. Route the `SetItemGroup` / `ItemLock` systems to a stage.
5. **Write `tools/kim.py`** (read-only, stdlib). It detects the stride from the header and the id table: 188 is March, 228 is June,
   and anything else is an error. Subcommands:
   - `dump <kim> <id>`: one record decoded, with strings and pools;
   - `diff <kimA> <kimB> --ids <file|list>`: a field-level diff by id;
   - `census <kim> --field <name>`: a histogram, plus an out-of-range report against March's limits;
   - `assets <kim> --ids ...`: every model, XET, texture, ani, particle and mesh name.

   Take the June offsets from step 2's map. Add a line to `tools/README.md`.
6. **Done gate:**
   - "Phase K0 - Done" carries the 228-byte map, with no `?` rows and every field routed. Long tables go to
     `june2017_re/funcmap.tsv` (rows `KIM K0`).
   - `kim.py dump` of 85003831 shows buff factor 482 in both the June file and the staged March file.
   - `kickoffs/june-item-kim/K1.md` is amended with anything K0 changed, and its STATUS row is set to `ready`.

### Phase K0 - Done

2026-10-07, Opus 5.5 high. No game code, no flag, no build. Landed: `tools/kim.py`, `june_item_kim/k0_record_map.tsv` (the full
228-byte map: every offset, June key, evidence address, data census, route), 29 `KIM K0` rows in `june2017_re/funcmap.tsv`.

**Loader.** `DoMemoryNotEncrypt` `328101f0` makes March's three checks with `N*0xe8 + 16`: the stride is 228, and June adds no check, so
there is nothing March cannot reproduce. `GetItemTemplet` `325bb740`, `GetSetItem` `325bb7c0` (set record still **20 B**),
`ResetItemManager` `31edda90`, `OpenItemScriptFile` `3281c590`, June writer `KProxy2::AddItemTemplet_LUA` `325bcdc0`.

**The map corrects the planning guesses.** Only the starred rows of "Established facts" stand. Changes against March (offsets June / March):
- +44 `m_fDamageRange` float, +48 `m_ItemIDForMinorUnit` int, +52 `m_ItemEquipmentGroup` int (defaults to the item's own id: the "second id").
- **`MAX_MODEL_COUNT_A_ITEM` is 8**: AttachFrameName[8] +68 (the guessed +92/+96 are slots 6, 7), ModelName[8] +108 (+132/+136 are
  slots 6, 7). The writer fills attach [0..1] and models [0..3] (`m_ModelName2/3` are new keys).
- +100 `m_wstrExpiredDate` (accessor `328671a0`, read by the item-period UI `3218aba0` and the description builder `32e102f0`),
  +104 `m_wstrPreExpiredDate` (no accessor; June's client never reads it).
- +224 BYTE `m_eDescriptionInShopType` (`DIST_DEFAULT 0 / MERGE_SOCKET_KR 1 / MERGE_SOCKET_INT 2`), then 3 B of zero padding.
- Everything else is March's field moved by +12, +28 or +36.

**The pools changed too, and these are K2 must-ports.**
- Stat is 28 B: `{HP, AtkP, AtkM, 0, 0, DefP, DefM}` (`328d68c0` reads [5]/[6]). March's 5-float read gives every item 0 defence.
- StatRelLV is 6 B; the sixth byte is `m_fAllStatRelLV`, which buff 16-step3b7's calculator already expects.
- **flags bit 7 is PvP-equip** (set when a `SAT_PVP_EQUIP` = 40 ability is listed: 240 items). HideSetDesc moved to flags2 bit 1, so
  March's `GetHideSetDesc` would hide the set text on those 240 items. This agrees with the buff Phase 15 step 2 row on `325bcdc0`.
- Extra entries stay 8 B, but the key is not `UNIT_TYPE`: the accessors map 1-9 to themselves, 10 to 10 (Lu) or 11 (Ciel) by a bool
  argument, 11 to 12 (Rose) and 12 to 13 (Ain).
- `GetNameColor_` gains grade 6 (`IG_LEGEND`), shown as `#CFF6900`.

**Answers.**
- flags2 bit 2 = `m_bIsForMinorUnit`: Ciel's dummy copy of a Lu item. It is set on 2,582 items, and 429 Lu items point at theirs
  through +48 (30344 -> 200030344).
- Item type 14 = `IT_CUSTOM` (June `Enum.src.lua`, `IT_END` = 15): 1,207 customize items such as skill cut-ins.
- The Extra per-unit-type width is unchanged (8 B), with 13 keys.
- The model count changed 6 -> 8; the set record is 20 B.

**Routing.**
| What | Route |
|---|---|
| The record, both pools, flags bit 7 / flags2 bit 1, the model count, the Extra key remap, the grade-6 colour | K2 (March already reads each) |
| `m_wstrPreExpiredDate`, the attach [2..7] / model [4..7] tails | K2 struct only: empty in the data, or never read by June |
| `m_ItemIDForMinorUnit`, flags2 bit 2, and the Ciel bool of the Extra remap | Stage 8 + the character-port checklist (Lu/Ciel) |
| Item type 14 | Stage 12 (customize); K2 guards it per K1 |
| `m_fDamageRange`, `m_ItemEquipmentGroup`, `m_eDescriptionInShopType` (inline readers, not located in K0), item expiry, PvP-equip (flags bit 7, SAT 40 and 44-50), SAT 42/43/51 | new **Stage 16**: June item-side systems |
| SAT 41 `SAT_ADD_DP_PERCENT_UP` | character-port checklist (Add) |
| `SetItemGroup.lua` (`g_pCX2SetItemManager:AddSetItemGroupData`), `ItemManager.lua` (`SetItemLock_MaxDay/EDPerDay`), `CoordiItem.lua` (`g_pCoordi_ItemManager`, XOR-built name), and the June-only tables `ResetItemManager` opens (`PackageItemData_INT`, `IceHeaterPreView_INT`, `ItemEtcAccessory`, `CanUseInventoryItem`, `GuildAgitShopItem`, `Synthesis*`, `PetUpgradeItemData`, `EventSealItemData`, `UnlockSocketItem(Data)`) | Stage 16 |
| `CoboRenderDivisionItemTemplet`, `DivisionItemTemplet`, `RenderDivisionMeshFile`; `TranscendentalClassItem` | Stage 11; Stage 6 |

- **`_LA`.** June loads `Item_LA.kim` + `SetItem_LA.lua` only when `argc == 3` and `argv[1] == "Axeso5"` (`31e480c5` sets `g_pMain+0x708`).
  US keeps `item.kim`.

**Self-test.** `kim.py diff <March> <June> --common --summary` over all 49,837 shared ids:
- Non-text fields March also has mismatch on at most 1.7 % of records (socket options, 871): data changes, not misreads.
- Def: 215 of 38,104. `item_type`, `endurance`, `buff_factors`, `stat_rel_lv`, `hide_set_desc`: 0.
- Text fields differ most (`desc_in_shop`: 4,854): localisation.
- Of the 1,293 Extra-list diffs, 1,215 are only June's added keys 10-13.
- Gate: `kim.py dump <either kim> 85003831` gives `buff_factors = [482]` for both files.

**For the user:** nothing to decide. Stage 16 follows the rule that an unowned June branch gets its own phase.

## Phase K1 - Data and code census

**Scope.** No code changes, no flag.

**Run with:** Sonnet 5.5 high.

**Read list:** Context, Established facts, Phase K0 - Done.

Write `.tsv`s under `june_item_kim/`, plus a Done note of at most about 60 lines.

1. **Reachable-item diff, March record against June record.**
   - Fields: type, equip position, unit type and class, use level, set id, seal count, endurance, period, price type, fashion,
     buff list and socket lists.
   - Id sources:
     - the save, read-only sqlite;
     - `GBASE_ITEM` (`X2OfflineInventory.cpp:1202+`);
     - `cash_product`;
     - `X2OfflineClassChangeSeed.h`;
     - the drop, static-drop and cube tables;
     - `ShopItem.lua`, `ItemExchangeTable.lua`, `ManufactureItem.lua`, `ItemConvertTable.lua`.
   - **An equipped item whose equip position or unit class changed is a save-breaker.** List those separately.
2. **Enum-range census** against March's limits: unit type and class, item type, equip position, grade, use type and condition,
   price type, special-ability types, and `flags2` bits.
3. **Cross-table ids.**
   - Buff-factor ids against the staged `Enum.lua`.
   - Socket, random-socket and set option ids against `OptionItemData.lua` (already June's) and `SocketOptionForm.lua`.
4. **Asset census.** Check the names referenced by the reachable set first, then all ids, against March `kom_contents` plus staged files.
   A March item that June gives a model March lacks is a visible regression.
5. **Code census, for K2's guard list.**
   - Every March array indexed by a templet field: `UnitType`, `UnitClass`, `ItemType`, `EqipPosition`, and the Extra
     per-unit-type arrays.
   - Every loop over all items at startup or at UI build: `GetItemNum` / `GetItemTempletFromIndex`, inventory sort and filter, the
     cash-shop and item-view cheat lists.
   - For each site, propose guard + skip + log, or extend.
6. **Memory headroom.**
   - Is `/LARGEADDRESSAWARE` set in `X2/X2_2010.vcxproj` for `US_SERVICE`?
   - Estimate the peak transient for a 67.5 MB member: the decompress buffer plus the `vector::assign` copy.
   - Propose loading without the copy only if headroom is thin.
7. **Settle which path loads today.** Is `KEM_ERROR72` in the last `ErrorLog.txt`? `CASH_SHOP_TOOL_PLAN.md:136-142` says "no .kim
   anywhere", which is believed stale.
8. **Done gate:**
   - The `.tsv`s are committed.
   - Every save-breaker and missing reachable asset is listed with a proposed handling.
   - User decisions are gathered in one `decision` row. Likely ones: whether newly visible cash products with missing assets are
     shown, and how save-breakers are handled.
   - `K2.md` is amended.

### Phase K1 - Done

2026-10-08, Sonnet 5.5 high. No game code, no flag, no build. Landed in `june_item_kim/` (scripts `k1_ids.py`, `k1_census.py`; they
need the decompiled staged March tables and a save copy in `K1_SCRATCH`, see their headers): `k1_reachable_ids.tsv`,
`k1_reachable_diff.tsv` + `_summary.tsv`, `k1_save_breakers.tsv`, `k1_newly_visible.tsv`, `k1_enum_census.tsv`, `k1_enum_names.tsv`,
`k1_cross_ids.tsv`, `k1_assets_reachable.tsv`, `k1_assets_all.tsv`, `k1_assets_to_adopt.tsv`, `k1_code_census.tsv` (66 rows).

**Which path loads today (task 7).** `item.kim`. The last `ErrorLog.txt` (2026-10-07 21:20) holds 91 `ErrorEnum(72)` lines and none names
`item.kim`, `Item.lua` or `SetItem.lua` (`KEM_ERROR72` is `LoadAndDoMemory`'s file-not-found line, `KTDXApp.h:530-570`, so the log shows
`ErrorEnum(72)`, not the macro name). No `Item.lua`/`SetItem.lua` exists in any extracted kom or loose in the game dir, so the Lua
fallback has nothing to load; `CASH_SHOP_TOOL_PLAN.md:136-142` ("no .kim anywhere") is stale.

**Reachable set (task 1).** 22 id sources by call shape (`k1_ids.py` header). 58,387 distinct ids hit a kim: 46,110 in both, 12,277
June-only, **0 March ids missing from June**. Sources that are stand-ins: cube = `ServerResource/US/RandomItemTable.lua` (the
staged copy would not decompile; it is newer than the tree, so 3,961 of its ids are June-only).
- **Save-breakers: none.** 91 equipped rows, 8 units, every one has a record in both kims and no gating field differs (`eqip_position`,
  unit type/class, use type/condition, level, `no_equip`). What does change on 27 equipped items: `set_id` 0 -> a set on 20 (Ara
  41460-41500, Elesis 41720-24, 130134-38, 133125-29: they gain set bonuses) and default socket ids on 7. June wins; no handling.
- **66 reachable items change equip slot, 64 of them 17 -> 26.** June 26/27 are `EP_AC_MINIME`/`EP_AC_EARRING`; March has no slot
  there (`EP_AC_TEMP2`, unused). They are the Mini-me items, equippable in March today as a face accessory. June has 475 items at 26
  (127 reachable) and 305 at 27 (9 reachable). **Decision D2.**
- Other reachable diffs (`_summary.tsv`): socket ids 845, set id 355, Def 214, Atk 142, quantity 146, price 124, 58 unit-class and 20
  unit-type fixes (0 -> the right class), eqip 66, Lu/Ciel/Rose/Ain Extra keys on 780. Data changes, not misreads.

**Enum census (task 2, `k1_enum_census.tsv`).** Out of March's range, all unreachable: item type 14 (1,207), unit type 10-12 (8,572),
unit class 123-136 and 200-202 (8,462), SA 40-43 and 51 (249 records), Extra key 10-13, flags2 bit 2 (2,582). Reachable: **none**.
Eqip stays <= 38, grade <= 6. `buy_pvp_rank` 9 (31 items) and 10 (52) exceed March's `PVP_RANK` (max 9; 10 = `PVPRANK_MAX`): none
reachable, but hovering one crashes (`X2ItemSlotManager.cpp:3552`, null `GetPVPEmblemData`; checked). SAT 0-39 and `ITEM_TYPE` 0-13 have
the same names at the same numbers in March and June (`k1_enum_names.tsv`, 0 different), so the SA pool layout claim holds.

**Cross-table (task 3, `k1_cross_ids.tsv`).** Buff factors: 46 ids, 0 absent from the staged `Enum.lua`. Socket ids: 0 absent from
`OptionItemData.lua`. Sets: 0 June items point to a missing set. Gaps: 2 set-option ids (31701, 35300; 19 refs) absent from
`OptionItemData`; March has 0 (K2 guard: log). Random-socket ids are not `OptionItemData` ids (March has the same 5 misses): no gap.
`SocketOptionForm.lua` holds functions only, no ids.

**Assets (task 4).** Index = every file under `kom_contents` + the game dir (103,894 names); `drop_viewer` and particle names are not files
and are not checked. (a) **39 reachable March-item asset names change to a file March lacks, on 28 ids** (`k1_assets_reachable.tsv`,
`march-visible`; Lu/Ciel/Rose/Ain keys excluded: those units do not exist): e.g. 90003060-67 Ara/Elsword.. INT hair/hand meshes + shop
images, 129716-18 `_old_Event_Foot30`. (b) **294 newly visible `cash_product` rows, 122 with a missing file (268 names)**, 36,190 of
111,428 June ids overall (`k1_assets_all.tsv`, unreachable mostly). **Every one of the 268 + 39 names exists in a June kom**
(`k1_assets_to_adopt.tsv` names the kom: data191, data143, data016, datai023, ...). 119 other missing names (110 package-item
`HQ_Shop_Item_*.dds`, 9 cube) exist in no June kom. **Decision D1.**

**Code census (task 5, `k1_code_census.tsv`; 66 rows: 22 K2-change, 9 guard+skip+log, 9 extend, 26 negative).** Checked by hand: the
struct/loader sites, `X2ItemSlotManager.cpp:3552`, `X2BeginningGame.cpp:946`, `X2Unit.cpp:589`, `CreateShopImageNameList`. Highest risk:
(1) struct + `DoMemoryNotEncrypt` (K2); (2) the PvP-emblem deref; (3) the 5- vs 7-float stat pool (Def reads 0); (4) `EP_END`-sized arrays
indexed by eqip position (safe only because `SERV_NEW_ONE_PIECE_AVATAR_SLOT` is on for US); (5) 39 unguarded `GetItemTemplet(id)->`
sites (safe: 0 reachable id is missing); (6) `item.kim` skipped if a June `Item.lua` appears; (7) memory; (8) three O(n) lists at 2.3x
(`CreateShopImageNameList` runs every launch, ~8 MB for 111k; cheat list; personal-shop autocomplete). No array is sized `IT_NUM`,
`IG_NUM` or `SAT_END`; the Extra readers are searches, so keys 10-13 just default.

**Memory (task 6).** `X2_offline.exe` header Characteristics 0x0103: **not LargeAddressAware** (2 GB). June's entry is zlib, 6.2 MB packed,
67.5 MB inflated. `LoadAndDoMemory` takes a 67.5 MB buffer (above the pooled buckets, `BUCKET_NUMS` 23 = 4 MB max, so it is freed after
the load), then `vector::assign` copies it into one retained 67.5 MB block: **peak about 141 MB, 67.5 MB retained**, both contiguous,
during `CX2Data::ResetItemManager` at startup (load order against texture init not measured). Headroom looks adequate; no copy-free
load proposed unless the K4 working-set line says otherwise. K2 logs the working set after load.

**Handed to K2/K3 / not K1:** gold coin 127041 is in June's kim (K3 task 4). **Open decisions** are one STATUS row (`decision`).

## Phase K2 - Loader port and data swap

**Flags:** `SERV_IRUHADEV_JUNE_ITEM_KIM`, `SERV_IRUHADEV_KIM_LAYOUT_CHECK`.

**Run with:** Opus 5.5 high.

**Read list:** Context, Rules, Established facts, Phase K0 - Done, Phase K1 - Done.

1. **`X2Item_Preprocessing.inl`.**
   - Under the flag, June's `KItemFormatTemplet`, field for field from K0. `#else` keeps March's.
   - Existing member names stay, so the call sites compile untouched. New members get June's names.
   - `BOOST_STATIC_ASSERT( sizeof(CX2Item::KItemFormatTemplet) == 228 )` under the flag, and `== 188` in `#else`.
   - A changed model count or Extra width goes under the flag too.
   - **Prove the flag is seen** (the `CommonPacket.h` precedent: there, a header reached before `Always.h` compiles `#else` with no
     error, and here the `== 188` assert would pass with it):
     - put a `#pragma message` inside the flag-on branch and read it in the build output;
     - grep the includers. Only `X2Lib/X2Item.h:19` includes the `.inl` today, so check that `X2Item.h` is reached after `KTDX.h`.
2. **`SERV_IRUHADEV_KIM_LAYOUT_CHECK` in `DoMemoryNotEncrypt`.**
   - Derive the file's stride: `(offset of the setID table - 16 - 4N) / N`. Find that offset by scanning for the first `setID`
     past the records, or by checking that `rec[k]+0 == itemID[k]`.
   - Require the id at record +0 to equal the id table at index 1, the last index, and every 997th index, at the compiled `sizeof`.
     Do not sample record 0: it sits at `16 + 4N` in both layouts, so it proves nothing.
   - On a mismatch, refuse and log `[JUNE_KIM] layout mismatch: file stride %d != compiled %d`. The refusal falls into the existing
     `KEM_ERROR72` / Lua path.
   - **That path has nothing to load.** No `Item.lua` is staged in `data036_kom`, so the fallback yields no items, and the client will
     likely crash at the first unchecked `GetItemTemplet`. The mismatch line must therefore be written and flushed *before*
     `DoMemoryNotEncrypt` returns, so the crash log still names the cause. That is the whole point of the check.
   - On success, log `[JUNE_KIM] loaded items=%d sets=%d stride=%d`.
3. **Lua fallback writer** (`KProxy2` / `PostProcess`).
   - Under the flag it writes June's layout, with June-only fields 0 or empty.
   - It logs `[JUNE_KIM] Lua fallback: June-only fields defaulted`.
   - It is kept, not disabled: it is the dev path when a loose `Item.lua` exists.
   - Comment that `X2ScriptPreprocessingTool` is untouched.
4. **Guards** from K1's code census. Skip the item, log once per kind, and keep a count.
5. **Accessors and consumers** for the fields K0 routed to "port in K2".
6. **Data.**
   - Back up the staged March kim as `item.kim.march_patched` in the session scratchpad and next to `march_data\data036\`.
   - `kom_contents\data036_kom\` is shared staging with the Atlas/Elysion worktree (`ATLAS_PORT_PHASES.md` "Shared-staging merge
     rule"). Neither port stages `item.kim`, so the file is this plan's alone. Pack data036 only when no other session is packing it,
     and diff the staged directory against the live kom first so you don't ship someone's half-staged files.
   - Copy June's `item.kim` into `kom_contents\data036_kom\`. Keep the manifest entry `"item.kim": "2"`. Do not add `item_LA.kim`.
   - Run `kim.py dump` on 85003830/1 and confirm 481/482. That retires buff Phase 13 to-do 2; say so in its doc.
   - Run `/elstools pack data036`. Verify by extracting the packed kom and comparing every entry's md5: only `item.kim` may differ.
     Use the same method as the buff Phase 13 review.
   - **Ask** before `/x2-kom-packed copy data036`.
7. **MODS.md entry.** It states that the June kim and `SERV_IRUHADEV_JUNE_ITEM_KIM` go together, and that the layout check catches a
   mismatch. Revert recipe: undefine the flag and restore `march_data\data036\item.kim` plus the Phase 13 patch, or the `.march_patched`
   copy.
8. **Finish** per x2-phase. Write `K2-review.md` and `K3.md`.

### Phase K2 - Done

2026-10-08, Opus 5.5 high. Commit: the one carrying this note (`June item.kim Phase K2: ...`). Flags (all `KTDXLIB/Always.h`): `SERV_IRUHADEV_JUNE_ITEM_KIM`, `SERV_IRUHADEV_KIM_LAYOUT_CHECK`,
`SERV_IRUHADEV_JUNE_KIM_MINIME_REMAP` (D2 stand-in, `#error` without the first). All edits insert-only, CP949 via `bytepatch`.

**Code.**
- `X2Item_Preprocessing.inl`: June's 228-byte record (static asserts 228 / stat 28 / RelLV 6; `== 188` in `#else`), `MAX_MODEL_COUNT_A_ITEM` 8
  (`#pragma message` seen in the X2Lib build), `IT_CUSTOM` = 14, stat pool 7 floats (`m_afJuneStatSlot3_4`), RelLV `m_byAllStatRelLV`
  (ctor/Init/AddStat/`IsValideCheck`/`operator<`), flags bit 7 `FLAG_BIT_PVP_EQUIP` with HideSetDesc on flags2 bit 1 and minor-unit on bit 2,
  getters for the June-only fields, `GetEqipPosition` = D2 remap + `>= EP_END` -> `EP_NONE` guard (every EP_END array site at once).
- `X2Item_Preprocessing.cpp`: Extra readers take June's key (`_GetJuneExtraKey`, from `32867660`: 1-9 same, 10 -> 10/11 by the Lu/Ciel bool,
  11/12 -> +1, else 0; the 10 callers compile unchanged via `bIsMinorUnit_ = false`); `GetNameColor_` grade 6 `#CFF6900`; `GetExpiredDate`;
  `JuneKimLine` / `JuneKimGuard` (first hit, then 10, 100, ...).
- `X2ItemManager.cpp`: layout check (stride read from the data: index 1, every 997th, last; size bound incl. the set table; refusal line
  flushed before `return false`), `loaded ... workingset` line, load census (2 lines), Lua-fallback line, `CreateShopImageNameList` skipped
  while `ShopImageName.txt` is newer than `data036.kom` (else timed).
- Grade 6 / type 14 from June (subagent Ghidra pass): tooltip label `[` + STR 34930 "Legend" + `]` (`32e102f0`; `X2ItemSlotManager.cpp`),
  UIWatch label (`3288a5d0`), drop colour `0xffff6900` (`32739c30`), `IT_CUSTOM` sorts as `ST_SPECIAL` (`32869aa0`, not `ST_NONE` as K1
  proposed: June wins). No change, June has no case 6 either: pickup sound (`3273b150`), seal check (`32171b50`). `GetRequireEDFactor`: June
  has no C++, it runs `AttribEnchantRequireED.lua` (LEGEND 8), which March already calls under `_JUNE_ENCHANT_PARITY`; the C++ is the
  missing-script fallback, left alone.
- Guards: PvP emblem NULL (`WriteStringForRestrictionPurchaseToDesc`; June has no check, rank 10 has an emblem there: routed to Stage 16).
  Set options 31701/35300: every March and June `GetSocketData` consumer already null-checks (silent skip in both); no edit.
- `X2ItemStatCalculator.cpp`: the templet's all-stat byte feeds `m_fAllStatRelLV` (with `_JUNE_SOCKET_PARITY`). The IN_HOUSE dev tooltip
  is not extended (0 in all June data).
- Not touched: `X2Lib/Offline/` (K3), the cheat list and personal-shop autocomplete (note only, 2.3x n; revisit if K4 shows a stall).

**Hygiene** (`flagcheck --worktree --files <the 9 code files>`; the `--range` run after commit is in the hand-off):
`JUNE_ITEM_KIM`: 6 PASS, 1 PASS~ (blank line), 2 OTHER (`Always.h`, `X2ItemManager.cpp`: LAYOUT_CHECK blocks), 0 FAIL, 0 deleted, RESULT PASS;
`KIM_LAYOUT_CHECK`: 9 OTHER, RESULT PASS; `JUNE_KIM_MINIME_REMAP --with JUNE_ITEM_KIM` (it is nested in it): 1 PASS, RESULT PASS.
17 `eol!` (LF lines inserted next to CRLF anchors in mixed-EOL `X2Item_Preprocessing.cpp`) and bare `#else` lines (`tok!`) are style only.
The `== 188` assert sits in `X2ItemManager.cpp`'s LAYOUT_CHECK block (both layouts pinned there), not in the struct's `#else`.

**Data.** `data036_kom/item.kim` = June's (md5 `a16b6f6e…`, 111,428 items, 6,740 sets, stride 228); backups `item.kim.march_patched` in the
scratchpad and `march_data/data036/`. `kim.py dump` 85003830/1 -> `[481]` / `[482]` (buff Phase 13 to-do 2 retired, noted there). D1:
501 June files staged into 25 koms (6 new: data158/159/185/191/192/211; 48 short copies, 2-35 B per kom), list
`june_item_kim/k2_assets_staged.tsv` (+ 79 names in no June kom). Pre-stage dir-vs-live diff: all clean. Packed-vs-live md5 per entry:
every entry of all 26 koms
extracted from live and packed: data036 differs by `item.kim` only, every other kom only gains the staged names, 0 removed
(`june_item_kim/k2_kom_md5_check.txt`; its `~_manifest.json` rows are the extractor's own generated manifest). The pack's
thousands of "changed" rows are compressed size only (same raw size, same algo, same md5). `/x2-kom-packed copy`: 26 `COPY OK`
(user approved 2026-10-08), e.g. `COPY OK data036: ... data036.kom 10652987 bytes, 192 entries, md5 1a078acb914b81a11052bf29e67e22cc`.

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,339,776 B, dated 2026-10-08 06:12:03, md5 c49f362852f888c3efaf987cd62a9a4b, equal to
X2/US_SERVICE/x2.exe (2026-10-08 05:45:54); built at mods/june-buff-port e9cfdca + 9 uncommitted code file(s).` (the 9 are K2's). The
`#pragma message` printed in both the X2Lib and X2 compiles; the deployed exe carries the layout-mismatch, census, ShopImageName,
emblem-guard and Lua-fallback strings and `#CFF6900`.

**How to see it.** `[JUNE_KIM] loaded items=111428 sets=6740 stride=228 ...` and two `census` lines in ErrorLog.txt whose counts
prove the offsets (computed with `tools/kim.py` from the staged kim): `unit_type>=UT_END(10)=8572 unit_class>=UC_END(124)=8445
IT_CUSTOM=1207 eqip>=EP_END(39)=0 eqip26=475 eqip27=305`, `SAT>=SAT_END(40) records=249 types=[40,41,42,43,51] buy_pvp_rank9=31
rank10=52 pvp_equip=240 minor_unit=2582` (a different number means a misplaced field). Stride probe ported to Python: June kim 228
accepted / 188 refused, March kim 188 accepted / 228 refused; armour shows its
defence; a Mini-me item equips in the face slot; a Legend item's name is orange with `[Legend]` in its tooltip.

**Play-test (2026-10-08, reyaa / Rena Grand Archer): passed.** ErrorLog: `loaded items=111428 sets=6740 stride=228 size=67557756
workingset=654 MB peak_workingset=654 MB private=738 MB` and both census lines exactly as predicted (every field at June's offset);
no layout mismatch, no new UNHANDLED/EXCEPTION. Equipped top shows defence; the Legend ring (188834, added to the save, backup
`db_backup/els_db_20261008_062020_pre_k2_legend_ring.sql`) shows the orange name and `[Legend]`; Mini-me 81081 (bill category 22)
bought from the cash shop, equipped in the face slot, renders.

**Open.** None for the user. Deploy order matters: exe and the 26 koms go together (a K2 exe refuses March's kim and then crashes;
an exe without the flags misreads June's kim silently, e.g. a `Trunk_elysion` deploy whose `Always.h` predates K2).

## Phase K2 review

**Run with:** Sonnet 5.5 high.

Check the following:
- **Offsets.** The struct offsets match K0's map: run an `offsetof` probe with `#pragma message` or `cl /P`, then rebuild one project.
- **Flag hygiene.** `flagcheck` passes for both flags with `--range`, and flag-off equals the base.
- **Layout check.** It refuses a March kim under a flag-on build and a June kim under a flag-off build. Prove both by reading the code
  path and the log line; a deploy is not needed for the negative case.
- **Kom diff.** The packed kom diff is `item.kim` only.

### Phase K2 - Review

2026-10-08, Opus 5.5 high (the kickoff names Sonnet; the user chose to continue). **Passed, one comment fixed, nothing for the user.**

1. **Offsets: PASS.** The probe was `june_item_kim/k2r_offset_probe.cpp`: `offsetof` / `sizeof` / enum values read out of `cl.exe` errors with X2Lib's
   US_SERVICE flags through the real `StdAfx.h` chain, and the flag's `#pragma message` printed. Every one of the 55 members sits at K0's
   offset (+44/48/52, name 56, attach[8] 68, dates 100/104, model[8] 108, strings 140-160, pools 164-196, shorts 200-206,
   bytes 208-223, desc-in-shop type 224, pad 225). `sizeof` is 228. Stat is 28 B (AtkM 8, slot[3..4] 12, DefP 20, DefM 24). RelLV is 6 B
   (all-stat at 5). `IT_CUSTOM` 14 (`IT_NUM` 15 <= 16), `MAX_MODEL_COUNT_A_ITEM` 8, flag bits 7 / flags2 1 / 2. Table:
   `june_item_kim/k2r_offset_probe.tsv`. Flag-off is March's 188 B: the `== 188` assert plus flagcheck's flag-off == base.
2. **Hygiene: PASS** (`--range f1549a6^..HEAD`).
   - `JUNE_ITEM_KIM`: 6 PASS, 1 PASS~, 2 OTHER, 0 FAIL, 0 deleted, RESULT PASS.
   - `KIM_LAYOUT_CHECK`: 9 OTHER, RESULT PASS.
   - `JUNE_KIM_MINIME_REMAP --with JUNE_ITEM_KIM`: 1 PASS, RESULT PASS.
   - All three stripped together (`KIM_LAYOUT_CHECK --with JUNE_ITEM_KIM JUNE_KIM_MINIME_REMAP`): 1 PASS~, 1 DEFINE, 7 OTHER, 0 FAIL,
     RESULT PASS. This is the run that clears `X2ItemManager.cpp`, which mixes two flags.
   - The OTHER rows: `Always.h` holds the sibling flags' define blocks, and `X2ItemManager.cpp` belongs to `LAYOUT_CHECK`.
   - The two PASS~ lines are each one blank line outside a flag: after the `GetExpiredDate`/`JuneKim*` `#endif` in
     `X2Item_Preprocessing.cpp`, and between the `LAYOUT_CHECK` `#endif` and the `JUNE_ITEM_KIM` `#ifdef` in `X2ItemManager.cpp`.
   - The 17 `eol!` and 16 `tok!` rows are style only. The `== 188` assert is where K2 says it is.
3. **Layout check: PASS.** Read path: `IruhaKimFileStride` checks index 1, then every 997th, then the last, and returns the smallest stride
   whose samples all match. A mismatch, or a file smaller than header + N*(4+stride) + S*(4+20), writes `layout mismatch` with
   `fileout` (flushed) before `return false`. I ran the same logic in Python:
   - March kim (N 49,837): stride 188, accepted at 188, refused at 228.
   - June kim (N 111,428): stride 228, refused at 188, accepted at 228.
4. **Kom diff: PASS** (`june_item_kim/k2r_kom_md5_check.txt`, script `.py`). Each live kom against its pre-K2-copy backup in
   `kom_backup_tools/` (`*.20261008_0611*`), md5 per entry:
   - `data036`: +0 -0 ~1, and the one changed entry is `item.kim` (live kom md5 `1a078acb` = K2's `COPY OK`).
   - The 19 asset koms that already existed: + exactly their `k2_assets_staged.tsv` names, -0, ~0, every added entry algo 0.
   - The 6 new koms hold exactly their staged names.
5. **Ghidra: PASS.**
   - Verified: the Extra remap `32867660`; the grade-6 colour `32867e80` `#CFF6900`; `IT_CUSTOM` -> 4 = `ST_SPECIAL` at `32869aa0`
     (decompiled as a switch: item types 7/11/14 -> 4); the tooltip and UIWatch labels (`32e102f0` and `3288a5d0`, whose jump tables
     send grade 6 to `@32e10d8e` / `@3288acad`, each `"[" + STR 0x8872 + "]"`).
   - June's `[EAX+0x40]` / `[S%1%]%2%` arm sits in all six grade cases. It is the string-ID view inside `GET_STRING`, which March
     also has (`KTDXStringTable.h:120`), so K2 is not missing an arm.
   - Drop colour: r 1, g 105/255 (`334b73cc` = 0.41176), b 0 (`334387b0` * `334350e8` = 0), giving `0xffff6900`.
     **Fixed:** the function is `32739cb0`, not `32739c30` (the case address `@32739fcf` was right). The comment in
     `X2DropItemManager.cpp` is corrected; K2's Done note keeps the old address.
   - No case 6 at the pickup sound `3273b150` (grades 1/2/3, else default) or the seal check `32171b50` (cases 1-4).
6. **Guards: PASS**, one verdict per K1 `guard+skip+log` row:
   - PvP emblem: `JuneKimGuard` plus `return` (a skip; hits 1, 10, 100, ...; the power-of-ten logic checked).
   - EP_END: `GetEqipPosition` returns `EP_NONE`, so the item equips nowhere (a skip, not a clamp into a slot). Its count is the load
     census `eqip>=EP_END=0`, not `JuneKimGuard`.
   - `GetItemTemplet` NULL (39 sites): moot, since K1 found 0 March ids missing from June.
   - Cheat list and personal-shop autocomplete: performance rows, deferred by K2 as note-only. The never-clamp rule does not apply.
   - `ShopImageName`: skipped while the list file is newer than `data036.kom`, timed otherwise.
   - Lua source selection: plan K2 item 3 replaced K1's "assert absent" with the logged fallback. The writer's June-only fields are
     0 through `ZeroMemory` in the ctor.
   - The two D rows: as their A rows.
   - The D2 remap (26->17, 27->18) **is** a clamp, by the user's decision, under its own flag. It is not a guard.
   - No June-sized stat struct is copied anywhere else: `X2OfflineInventory.cpp:318` is a by-value local.

**Code change:** the comment address in `X2Lib/X2DropItemManager.cpp:294`, byte-patched. Encoding and EOL classes are unchanged, and
`flagcheck JUNE_ITEM_KIM --worktree --files X2Lib/X2DropItemManager.cpp` gives 1 PASS, RESULT PASS (the 1 deleted line is that
comment, inside the flag). **No build or deploy**:
- a comment-only edit compiles to the same code. At review time (07:10) the game dir still held `c49f3628`, whose build was
  `e9cfdca` + K2's 9 files: HEAD's code exactly, apart from this comment;
- a peer's `build.py` held the lock while building its own uncommitted C9h work, so a deploy from here would have shipped their files.

No re-play-test is needed.

## Phase K3 - Emulator and save reconciliation

**Run with:** Sonnet 5.5 high.

**Flag:** `SERV_IRUHADEV_JUNE_ITEM_KIM`.

**Scope:** fixed by K1. If K1 finds nothing in scope, this phase is a no-op Done note.

1. **Save-breakers.** Apply the handling the user chose. Example: an equipped item whose slot changed moves to the inventory on load,
   with a log line. Back up `els_db.sql` first.
2. **Cash shop.** The 294 newly resolvable `cash_product` rows appear, because June wins. Products with missing assets get the user's
   chosen handling. Log the before/after counts.
3. **Cubes and drops.** Log the `X2OfflineRandomItem` / `X2OfflineDropTable` count deltas.
4. **Stale "not in item.kim" comments.**
   - The class-9 `GBASE_ITEM` row (`X2OfflineInventory.cpp:1212`) stays unreachable, because `gup_create_unit` rejects classes
     outside 1..8 (`:1193`). Only note that its ids now resolve.
   - Gold coin 127041 (`Handlers_Room.cpp:2974-2975`, "March's item.kim has no 127041 row"): check June. If June has the row, decide
     with the offline-grade owner whether the floor-drop workaround stays.

## Phase K4 - Play-test and close

**Run with:** the user's play-test. Claude reads the logs.

**Hand-off.** Copy-ready, with dev-console commands, the character and job, and a where / what / look for / if not for each check.
1. **Boot.** `[JUNE_KIM] loaded items=111428 sets=6740 stride=228` appears, and there is no `KEM_ERROR72`.
2. **Character select.** Every saved character renders with its gear.
3. **Inventory.** Check tooltips on every tab, set tooltips (June set data), names, descriptions and sockets.
4. **Heretic potion.** The buff is 481/482, with no crash at `X2Game.cpp:18160`.
5. **Dungeon.** Run one with drops and open a cube.
6. **Cash shop.** Browse it, including a newly visible product, and preview it.
7. **Memory.** The working set in the village, against the pre-swap baseline.

**Logs.** `logs.py --tag JUNE_KIM`: guard counts, `ITEM WARNING no templet` for saved items (expect 0), and no new EXCEPTION lines.

**Close.** Mark these as reachable, with no permanent "inert" verdicts:
- STATUS and MODS.md;
- C9f-2 and JUNE_BUGS #102 ("latent: none of June's 5 item ids is in March's item.kim");
- Stages 11 and 12 in `JUNE_PORT_BACKLOG_ORDER.md`.

## Out of scope (routed, not dropped)

- **`item_LA.kim`.** US loads `item.kim`.
- **June's `SetItemGroup.lua` / `g_pCX2SetItemManager`, `SetItem_LA.lua`, and `ItemManager.lua` (`SetItemLock_*`).** New June
  systems; K0 routes them to a stage.
- **June's other item tables.** These are `ShopItem`, `CashShopItemList`, `ManufactureItem`, `PackageItemData`, `RandomItem`,
  `ItemExchangeTable`, `ItemConvertTable`, `CoolTimeGroupItem` and others. March's tables keep working, because every March id is in
  June; K0 or K1 proposes a backlog stage.
- **Items of June-only characters** (unit type > 10). Unreachable until Stage 8; K2 only guards them.
- **Bulk asset adoption for the 61,591 new ids.** K1 checks reachable items only.
- **The cash shop editor** (`CASH_SHOP_TOOL_PLAN.md`). It does not parse the kim: it runs the luac `Item.lua` from an older install
  in an embedded Lua 5.1. So the stride change cannot break it. It will not list June's new items, though; that is a follow-up for
  that plan.
