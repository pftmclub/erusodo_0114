# Costume shop: every June costume in the offline cash shop

Created 2026-10-08 (user request, after june-item-kim K4). Branch `main`. Flag `SERV_IRUHADEV_COSTUME_SHOP`. Log tag `[COSTUME_SHOP]`.

## Context

June's `item.kim` (live since june-item-kim K2) has 39,582 fashion-flagged items (costumes, every part, every character incl. Lu/Ciel/Rose
unit types 10-12). The offline shop sells 2,378 products (727 of the costumes). The user wants **every** costume in the shop, because
the other characters will be ported later. The shop only lists items usable by the selected character, so rows for characters that
are not ported yet cost memory but show nothing.

## Rules (the user's decisions, 2026-10-08)

- **All costume items go into `cash_product`**: every fashion-flagged item in June's kim (about 38,850 new rows), all unit types.
- **Also every character's GM weapon** (user, 2026-10-08: "like Elsword is 128015, Rena is 128017"). These are real gear (not
  fashion-flagged, so the costume rule does not catch them), one weapon per character, found by name in June's kim:
  Elsword 128015, Aisha 128019, Rena 128017, Raven 128078, Eve 28005, Chung 128080, Ara 181869, Elesis 183248 (Korean name,
  untranslated), Add 183648, Luciel 184921, Rose 186038 (`Rose's GM Guns`), Ain 188856. Each has a matching GM top piece (+1 id:
  128016, 128020, 128018, 128079, 28006, 128081, 181870, 183249, 183649, 184922, 186039, 188857); the user asked for weapons only,
  so S0 adds the weapons and the top pieces are a one-line extension if the user wants them. Same price and row rules; the unported characters' weapons (Luciel, Rose, Ain) are offered without the file check, like their costumes.
- **Price is 1 KC per product** ("just like right now": every existing row is `price = 1`).
- **A load-time file check decides what the client is offered.** A costume's meshes, textures and shop image must exist (through the
  mass file manager). Rows that fail stay in the DB and are not put in the catalog; they appear on their own once their files arrive
  (the ~14,800 for existing characters with the june-asset-port). No code or data change then.
  **Exception (user, 2026-10-08): items of characters this client does not have yet are always offered, with no file check.**
  The test is the item's unit type >= `CX2Unit::UT_END` (10 today: Lu, Ciel, Rose, Ain, ...). The shop's own filter
  (`CX2CashShop::IsPossibleUsedByMyCharacter`, `m_bAllCharItem` false) already hides them, and nobody can reach them until
  that character exists. The test follows `UT_END`, so when a character is ported its items fall under the file check by themselves.
  Known side effect: the shop's "all characters" checkbox (`X2CashShop.cpp:1346`) lists every character's items, so with it
  ticked these show with blank icons. S0 does not suppress that; the Done note tells the user.
  Log the offered / withheld counts once (`[COSTUME_SHOP]`).
- Existing rows, the player's wallet and orders are not touched. `product_no` of existing rows does not change.
- The save is real data: back up `els_db.sql` to `db_backup/` before the first run that writes the new rows (allowed without asking).
- Offline only; no server sync questions. Ask before any `/x2-kom-packed copy`.

## Facts the phase starts from (measured 2026-10-08 from the live kim and save)

- Fashion-flagged items: 39,582; unit types 0-9: 34,314; already in the shop: 727; new usable-type rows not in the shop: 33,587, of
  which 14,832 name a file March lacks (K1's per-id list `june_item_kim/k1_assets_all.tsv`, before D1 staging).
- By part (fashion, all characters): weapon (eqip 2) 6,235; hair (6) 6,596; body (8) 5,998; leg (9) 5,651; hand (10) 5,655;
  foot (11) 5,656; accessory slots 17-27 about 2,400; one-piece (38) 1,306.
- Existing rows carry the **billing category** (not the `CSSC_*` enum): weapon 11, body 12, leg 13, hair 14, hand 15, foot 16;
  accessory eqip 25 -> 21, 18/19 -> 22, 23 -> 26, 24 -> 27. The authoritative table is `CashShopCategory.lua` in data036
  (`CASH_SHOP_REAL_SUB_CATEGORY_ID`, compiled); `X2OfflineCashShop.h:25` explains the number space.
- Quantity is 1 for almost all rows (a few 5/10/20/100 for consumables; none for costumes).
- Per-click cost in the client: `CX2CashShop::GetItemByCategory` walks `GetAllCashItemList()` (every product's item) and filters per
  item, so a catalog 14x larger is about 14x the work per tab click. Estimated only; this plan measures it.

## S0 - Rows, file check, timing

**Run with:** Sonnet 5.5 high. **Flag:** `SERV_IRUHADEV_COSTUME_SHOP` (define in `KTDXLIB/Always.h`).

1. **Category map first.** Decompile `CashShopCategory.lua` (the project's usual route, `june2017_re/ljd_jun2017.py` is June-only; March
   luas: `/elstools`) and write down the billing category for every fashion eqip position, including the accessory slots (17-27, 36)
   and one-piece (38). Where a position has no category in the table, do not guess: list it and ask the user.
2. **Fill the table.** A top-up in `CX2OfflineCashShop::EnsureLoaded` (the item manager is ready there; the schema-migration path runs
   before the kim is loaded): for every fashion-flagged item in the item manager's templet table that has no `cash_product` row,
   `INSERT OR IGNORE` a row (new `product_no` after the current max, price 1, quantity 1, the category from step 1), in one
   `Begin()/Commit()` transaction. Idempotent: a second launch adds nothing. Log the inserted count and the time it took.
2b. **GM weapons.** The same top-up for the 12 ids above (a fixed list in the code, with the kim name as a cross-check: log a line if an
   id's name stops containing "GM"). They are real gear, so step 1 must also find which billing category the shop gives a non-fashion
   weapon (none of the 2,378 rows is one: `CategoryFor` puts real gear in the "etc" tab, but that is the CSSC enum, not the billing
   number). Do not guess; ask the user if the table has no tab for it.
3. **File check.** When building the catalog, withhold a row whose item's model, texture and shop-image names are not in the mounted koms.
   Use the same lookup the loader already uses (`GetMassFileManager()`), not a directory scan. Log offered/withheld per part.
4. **Timing.** Log (`[COSTUME_SHOP]`) the time EnsureLoaded takes and the catalog size. The play-test measures opening the shop and
   switching tabs by feel; if it stutters, the fix is a cache of the filtered list in the client (separate flag, ask first).
5. Done gate: flagcheck PASS, `build.py --deploy`, Done note under this heading, STATUS row, a play-test hand-off (open the shop on a
   character, browse every costume tab, buy and equip one new costume, then one for each other tab; first launch writes the rows).

### S0 - Done (2026-10-08)

**Landed.** Flag `SERV_IRUHADEV_COSTUME_SHOP` (`Always.h`), `X2OfflineCashShop.cpp` (load-time file check + `[COSTUME_SHOP]` log),
`costume_shop/insert_products.py`, `costume_shop/withheld_files.py` + `withheld_files.tsv`. **Deviation from steps 2/2b (user,
2026-10-08): no C++ top-up / seeder.** The rows were written straight into the game dir's `els_db.sql` by the Python script (a seeder
can come in a later phase if wanted). Backup: `db_backup/els_db_20261008_172622_pre_costume_shop.sql`.

- **Rows inserted: 38,844** (product_no 2379..41222; 38,832 fashion + 12 GM weapons), price 1, quantity 1, event 0. A second run adds
  nothing (key = item_id; 39,571 already present). `cash_product` now 41,222 rows, integrity ok.
- **Skipped on purpose (user: "skip them"):** eqip 0 = 12 packages, eqip 7 = 4 Ain faces, eqip 36 = 7 Raven left arms; none has a tab.
- **Category map** (eqip -> billing, `CashShopCategory.lua` numbers; accessories checked against the 2,378 live rows):
  2->11 weapon, 8->12 body, 9->13 leg, 6->14 hair, 10->15 hand, 11->16 foot, 38->17 one-piece (table only; `SERV_NEW_ONE_PIECE_AVATAR_SLOT`
  is on for US), 25->21, 17/18/19/26/27->22 (26/27 have only non-fashion precedent), 20->23, 21->24 (majority 33 vs 16), 22->25, 23->26, 24->27.
- **Plan fact corrected:** the live rows DO include non-fashion weapons: 277 rows (eqip 2, item type 1) sit in billing 11. So the GM
  weapons go to 11 with that precedent and no question was needed. All 12 GM names contain "GM" (Elesis's is Korean `...GM...`).
- **File check** (client, rows with product_no > 2378 only): model 0..7, texture-xet, ani-xet, shop-image, "" and "0" = no asset,
  directory prefix stripped, one `IsValidFile` per distinct name. Unit type >= `UT_END` (10) is offered unchecked. Per the Python
  reproduction against the deployed koms (name index 105,075): **19,017 offered after the check, 14,564 withheld, 5,263 offered
  unchecked** (Lu / Ciel / Rose / Ain). 592 of the withheld miss only an `HQ_Shop_*.dds` shop image. Per-category numbers are in
  `withheld_files.py`'s output; the list for the june-asset-port is `costume_shop/withheld_files.tsv` (item, category, unit type, names).
  The play-test checks that the logged counts equal these.
- **Known side effect:** the shop's "all characters" checkbox shows the 5,263 unchecked items with blank icons.
- **Hygiene:** flagcheck `--worktree --files KTDXLIB/Always.h X2Lib/Offline/X2OfflineCashShop.cpp`: 0 PASS, 1 PASS~ (3 blank lines),
  1 DEFINE, 0 FAIL; 0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 encoding change. RESULT: PASS.
- **Deploy:** DEPLOY OK: X2_offline.exe is 19,390,976 B, dated 2026-10-08 17:53:01, md5 82822aa7e9f98d12500e6f37420ee415, equal to
  X2/US_SERVICE/x2.exe (2026-10-08 17:52:56); built at mods/june-buff-port 007555f + 2 uncommitted code file(s). `[COSTUME_SHOP]` is in the exe.
- **Open risk:** per-click cost of a 41k-product catalog (estimated 14x); the log line gives the EnsureLoaded time, tab clicks are by feel.
  If it stutters the fix is a cached filtered list in the client (separate flag, ask first).

**Play-test hand-off** (log tag `[COSTUME_SHOP]`): launch with `start_offline.bat`, open the cash shop on any character (the first open
loads the catalog; the product pages arrive in about 100 packets). Look for: (1) all fashion tabs filled, including the one-piece tab;
(2) buy one new costume of each tab, claim it from the deposit, equip it, confirm it renders; (3) tab-switch and scroll feel; (4) a
withheld item (for example item 48, missing its shop image) must NOT be listed. If not: tell me which tab/item. Then say "check logs".

## Close

The user's play-test. Then Claude reads `logs.py --tag COSTUME_SHOP`, and the `june-asset-port` hand-off gets the withheld list
(`costume_shop/withheld_files.tsv`, written by the phase) so those files are pulled in early.
