#!/usr/bin/env python3
"""Costume shop S0: add every June fashion item (and every character's GM weapon) to cash_product.

Writes straight into the game directory's els_db.sql (user decision 2026-10-08: no seeder in the client).
Idempotent: an item_id that already has a row is skipped, so a second run adds nothing.

  python costume_shop/insert_products.py            # dry run: counts per category / eqip, writes nothing
  python costume_shop/insert_products.py --apply    # back up els_db.sql to db_backup/, then insert

Refuses to run while X2_offline.exe is running. Price 1, quantity 1, is_event 0, product_no = max + 1 upward
in item-id order. Category = the billing number in CashShopCategory.lua (data036): the table was decompiled
2026-10-08 and the eqip -> billing map below is checked against the 2,378 existing rows (see COSTUME_SHOP_PHASES.md).
"""
import argparse
import collections
import os
import shutil
import sqlite3
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)
sys.path.insert(0, os.path.join(TRUNK, "tools"))
import kim as K  # noqa: E402

GAME = os.environ.get("X2_GAME_DIR", r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data")
KIM = os.environ.get("X2_KIM", r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents\data036_kom\item.kim")
DB = os.path.join(GAME, "els_db.sql")

# eqip position -> billing sub-category. Sources: CashShopCategory.lua (the numbers), and the live rows' own
# eqip->category crosstab (accessories: 17/18/19 -> 22 face, 20 -> 23, 21 -> 24 (majority 33 vs 16), 22 -> 25,
# 23 -> 26, 24 -> 27, 25 -> 21; 26/27 have only non-fashion precedent -> 22). 38 one-piece -> 17 (table only).
# NOT mapped, skipped by the user's decision: 0 (packages), 7 (Ain face), 36 (Raven left arm), 16, 28, 29.
EQIP_TO_CAT = {2: 11, 8: 12, 9: 13, 6: 14, 10: 15, 11: 16, 38: 17,
               25: 21, 17: 22, 18: 22, 19: 22, 26: 22, 27: 22, 20: 23, 21: 24, 22: 25, 23: 26, 24: 27}
GM_WEAPONS = (128015, 128019, 128017, 128078, 28005, 128080, 181869, 183248, 183648, 184921, 186038, 188856)
GM_CAT = 11  # the live shop already sells 277 non-fashion eqip-2 weapons under billing 11


def running():
    out = subprocess.run(["tasklist", "/FI", "IMAGENAME eq X2_offline.exe"], capture_output=True, text=True).stdout
    return "X2_offline.exe" in out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--apply", action="store_true")
    a = ap.parse_args()
    if running():
        sys.exit("X2_offline.exe is running; close the game first")
    kim = K.Kim(KIM)
    print("kim: %d items, stride %d" % (kim.n, kim.stride))

    con = sqlite3.connect(DB)
    have = {r[0] for r in con.execute("SELECT item_id FROM cash_product")}
    max_no = con.execute("SELECT COALESCE(MAX(product_no),0) FROM cash_product").fetchone()[0]
    print("cash_product: %d item ids already, max product_no %d" % (len(have), max_no))

    new = []  # (item_id, cat, eqip, unit_type)
    skipped = collections.Counter()
    gm = set(GM_WEAPONS)
    for iid in sorted(kim.ids()):
        r = kim.raw(iid)
        ep, fashion = r["eqip_position"], r["flags"] & 1
        if iid in gm:
            cat = GM_CAT
        elif fashion:
            cat = EQIP_TO_CAT.get(ep)
            if cat is None:
                skipped[ep] += 1
                continue
        else:
            continue
        if iid in have:
            skipped["already"] += 1
            continue
        new.append((iid, cat, ep, r["unit_type"]))

    for iid in GM_WEAPONS:
        if kim.rec_base(iid) is None:
            print("WARNING GM weapon %d is not in the kim" % iid)
    print("to insert: %d rows; skipped: %s" % (len(new), dict(skipped)))
    for cat, n in sorted(collections.Counter(c for _, c, _, _ in new).items()):
        print("  category %2d: %6d" % (cat, n))
    print("  unit_type >= 10 (offered without file check): %d" % sum(1 for x in new if x[3] >= 10))
    if not a.apply:
        print("dry run - nothing written")
        return

    bdir = os.path.join(GAME, "db_backup")
    os.makedirs(bdir, exist_ok=True)
    dst = os.path.join(bdir, "els_db_%s_pre_costume_shop.sql" % time.strftime("%Y%m%d_%H%M%S"))
    bk = sqlite3.connect(dst)
    con.backup(bk)  # consistent copy, WAL included
    bk.close()
    print("backup:", dst, os.path.getsize(dst), "bytes")

    t0 = time.time()
    con.execute("BEGIN")
    n = max_no
    for iid, cat, _, _ in new:
        n += 1
        con.execute("INSERT INTO cash_product(product_no,item_id,category,quantity,price,is_event) VALUES(?,?,?,?,?,?)",
                    (n, iid, cat, 1, 1, 0))
    con.execute("COMMIT")
    tot = con.execute("SELECT COUNT(*), MAX(product_no) FROM cash_product").fetchone()
    print("inserted %d rows in %.1fs; cash_product now %d rows, max product_no %d" % (len(new), time.time() - t0, tot[0], tot[1]))
    con.close()


if __name__ == "__main__":
    main()
