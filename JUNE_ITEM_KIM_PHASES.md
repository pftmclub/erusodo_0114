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

## Phase K2 review

**Run with:** Sonnet 5.5 high.

Check the following:
- **Offsets.** The struct offsets match K0's map: run an `offsetof` probe with `#pragma message` or `cl /P`, then rebuild one project.
- **Flag hygiene.** `flagcheck` passes for both flags with `--range`, and flag-off equals the base.
- **Layout check.** It refuses a March kim under a flag-on build and a June kim under a flag-off build. Prove both by reading the code
  path and the log line; a deploy is not needed for the negative case.
- **Kom diff.** The packed kom diff is `item.kim` only.

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
