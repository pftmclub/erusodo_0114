#!/usr/bin/env python3
"""Reproduce the client's load-time costume file check (X2OfflineCashShop.cpp, SERV_IRUHADEV_COSTUME_SHOP) in Python.

Reads cash_product rows with product_no > 2378 from the game dir's els_db.sql, June's item.kim, and the entry names of
every data/*.kom plus loose files. Writes costume_shop/withheld_files.tsv (item id, billing category, unit type, missing
names) for the june-asset-port, and prints the offered / withheld counts that [COSTUME_SHOP] in offline_server.log must equal.
Name set = the client's: models 0..7, texture_change_xet, ani_xet, shop_image; "" and "0" are no asset; dir prefix stripped.
"""
import collections, glob, json, os, sqlite3, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)
sys.path.insert(0, os.path.join(TRUNK, "tools"))
import kim as K
GAME = os.environ.get("X2_GAME_DIR", r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data")
KIM = os.environ.get("X2_KIM", r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents\data036_kom\item.kim")
UT_END = 10

names = set()
for k in sorted(glob.glob(os.path.join(GAME, "*.kom"))):
    p = subprocess.run([os.path.join(TRUNK, "elstools", "elstools.exe"), "inspect", "-q", "-i", k],
                       stdin=subprocess.DEVNULL, capture_output=True, timeout=120)
    if p.returncode:
        sys.exit("inspect failed: " + k)
    for f in json.loads(p.stdout.decode("utf-8", "replace"))["files"]:
        names.add(f["name"].upper())
for dp, dn, fn in os.walk(GAME):
    for f in fn:
        names.add(f.upper())

kim = K.Kim(KIM)
con = sqlite3.connect(os.path.join(GAME, "els_db.sql"))
rows = con.execute("SELECT product_no,item_id,category FROM cash_product WHERE product_no > 2378 ORDER BY product_no").fetchall()
off, wh, nochk = collections.Counter(), collections.Counter(), 0
out = []
for pno, iid, cat in rows:
    r = kim.record(iid)
    if r is None:
        continue
    if r["unit_type"] >= UT_END:
        nochk += 1
        continue
    want = [r["model_name_%d" % i] for i in range(kim.L["models"])] + [r["texture_change_xet"], r["ani_xet"], r["shop_image"]]
    miss = []
    for w in want:
        if not w or w == "0":
            continue
        b = w.replace("\\", "/").split("/")[-1]
        if b and b.upper() not in names:
            miss.append(b)
    if miss:
        wh[cat] += 1
        out.append((iid, cat, r["unit_type"], sorted(set(miss))))
    else:
        off[cat] += 1
with open(os.path.join(HERE, "withheld_files.tsv"), "w", encoding="utf-8", newline="\n") as fh:
    fh.write("item_id\tbilling_category\tunit_type\tmissing_names\n")
    for iid, cat, ut, miss in out:
        fh.write("%d\t%d\t%d\t%s\n" % (iid, cat, ut, "|".join(miss)))
print("rows checked: %d; offered after check %d; withheld %d; offered unchecked (ut >= %d) %d; name index %d" % (
    len(rows), sum(off.values()), sum(wh.values()), UT_END, nochk, len(names)))
for c in sorted(set(off) | set(wh)):
    print("  billing category %2d: %6d offered, %6d withheld" % (c, off[c], wh[c]))
