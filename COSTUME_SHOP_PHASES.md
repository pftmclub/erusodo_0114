# Costume shop: every June costume in the offline cash shop

Created 2026-10-08 (user request, after june-item-kim K4). Branch `mods/june-buff-port`. Flag `SERV_IRUHADEV_COSTUME_SHOP`. Log tag `[COSTUME_SHOP]`.

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

## Close

The user's play-test. Then Claude reads `logs.py --tag COSTUME_SHOP`, and the `june-asset-port` hand-off gets the withheld list
(`costume_shop/withheld_files.tsv`, written by the phase) so those files are pulled in early.
