#!/usr/bin/env python3
"""K1 task 1 (id half): collect every item id March can reach, tagged by source.

Writes june_item_kim/k1_reachable_ids.tsv  (id <TAB> sources <TAB> in_march <TAB> in_june)
Read-only: the save is copied to a scratch dir first; nothing under the game dir is written.

Sources and how each id is taken (by call shape, never by a blanket integer scan):
  save_equipped / save_bag   item.item_id of els_db.sql
  save_cash_order, wish_list the same db
  gbase                      GBASE_ITEM in X2OfflineInventory.cpp
  cash_product               cash_product.item_id of the save
  classchange                every item id in X2OfflineClassChangeSeed.h that is a kim id (seed table rows)
  drop / static_drop / battlefield_drop / henir_drop / extra_drop   DropTable.lua / StaticDropTable.lua
  shop                       ShopItem.lua  (ITEM_ID, ITEM_LIST, SALE_ITEM lists)
  exchange                   ItemExchangeTable.lua args 3 and 4
  manufacture_material / manufacture_result / manufacture_village  ManufactureItem.lua
  convert                    ItemConvertTable.lua
  random_item                RandomItem.lua args 1 and 2
  random_data                RandomItemData.lua
  package                    PackageItemData.lua
  cashshop_list              CashShopItemList.lua
  cube_server                ServerResource/US/RandomItemTable.lua: m_ItemID, m_KeyItemID, AddRandomItemGroup arg 2
                             (the staged copy would not decompile; the server copy is a stand-in and is newer than the tree)
  miracle_cube_server        ServerResource/US/MiracleCubeTable.lua  (PvPDropTable.lua holds no item ids)
"""
import os
import re
import shutil
import sqlite3
import sys
import collections

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)
sys.path.insert(0, os.path.join(TRUNK, "tools"))
import kim as K  # noqa: E402

GAME = r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799"
MARCH_KIM = GAME + r"\kom_contents\data036_kom\item.kim"
JUNE_KIM = r"F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\data036_kom\item.kim"
SCRATCH = os.environ.get("K1_SCRATCH") or os.path.join(os.environ["TEMP"], "k1_scratch")
DEC = os.path.join(SCRATCH, "dec")      # decompiled staged March .src.lua (elstools decompile-batch)
SRV = os.path.join(TRUNK, "KncWX2Server", "ServerResource", "US")


def read(path):
    with open(path, encoding="utf-8-sig", errors="replace") as fh:
        return fh.read()


def strip_comments(s):
    s = re.sub(r"--\[\[.*?\]\]", "", s, flags=re.S)
    return re.sub(r"--[^\n]*", "", s)


def ints(s):
    return [int(x) for x in re.findall(r"(?<![\w.])(\d+)(?![\w.])", s)]


def block_after(text, key):
    """Yield the text of every `key = { ... }` (brace-balanced)."""
    for m in re.finditer(r"\b%s\s*=\s*\{" % re.escape(key), text):
        i, depth = m.end(), 1
        while i < len(text) and depth:
            c = text[i]
            depth += (c == "{") - (c == "}")
            i += 1
        yield text[m.end(): i - 1]


def call_args(text, name):
    """Yield the integer positional args (up to the first '{') of every `name(...)` call on one line."""
    for m in re.finditer(r"%s\(([^\n]*)" % re.escape(name), text):
        yield m.group(1)


def collect():
    src = collections.defaultdict(set)   # id -> tags

    def add(tag, ids):
        for i in ids:
            src[i].add(tag)

    # ---- the save (copied, then opened)
    gd = GAME + r"\data"
    os.makedirs(SCRATCH, exist_ok=True)
    for ext in ("", "-wal", "-shm"):
        p = gd + r"\els_db.sql" + ext
        if os.path.exists(p):
            shutil.copy2(p, os.path.join(SCRATCH, "els_db.sql" + ext))
    db = sqlite3.connect(os.path.join(SCRATCH, "els_db.sql"))
    db.row_factory = sqlite3.Row
    add("save_equipped", [r[0] for r in db.execute("select item_id from item where equipped=1")])
    add("save_bag", [r[0] for r in db.execute("select item_id from item where equipped=0")])
    add("save_cash_order", [r[0] for r in db.execute("select item_id from cash_order")])
    add("save_wish_list", [r[0] for r in db.execute("select item_id from wish_list")])
    add("cash_product", [r[0] for r in db.execute("select item_id from cash_product")])

    # ---- GBASE_ITEM
    t = read(os.path.join(TRUNK, "X2Lib", "Offline", "X2OfflineInventory.cpp"))
    i = t.index("const KBaseItemSet GBASE_ITEM[]")
    j = t.index("};", i)
    add("gbase", [int(x) for x in re.findall(r"\b(\d{4,})\b", re.sub(r"///.*", "", t[i:j]))])

    # ---- class-change seed: ids are the first column of each seed row; take every integer >= 1000
    t = read(os.path.join(TRUNK, "X2Lib", "Offline", "X2OfflineClassChangeSeed.h"))
    t = re.sub(r"//[^\n]*", "", t)
    add("classchange", [int(x) for x in re.findall(r"\b(\d{5,})\b", t)])

    # ---- DropTable.lua
    t = strip_comments(read(os.path.join(DEC, "DropTable.src.lua")))
    for fn, tag in (("DropTable:AddMonsterDropInfo", "drop"), ("DropTable:AddBattleFieldNpcDropInfo", "battlefield_drop"),
                    ("DropTable:AddHenirMonsterDropInfo", "henir_drop"),
                    ("DropTable:AddExtraStageMonsterDropInfo", "extra_drop")):
        for m in re.finditer(re.escape(fn) + r"\(\{", t):
            for b in block_after(t[m.end() - 1:], "DropItemList"):
                add(tag, [int(x) for x in re.findall(r"\{\s*(\d+)\s*,", b)])
                break
    for a in call_args(t, "DropTable:AddToGroup"):
        v = [x.strip() for x in a.rstrip(")").split(",")]
        if len(v) >= 2 and v[1].isdigit():
            add("drop_group", [int(v[1])])
    # ---- StaticDropTable.lua
    t = strip_comments(read(os.path.join(DEC, "StaticDropTable.src.lua")))
    for fn, tag in (("DropTable:AddStaticDropInfo", "static_drop"), ("DropTable:AddBattleFieldStaticDropInfo", "static_drop")):
        for m in re.finditer(re.escape(fn) + r"\(\{", t):
            for b in block_after(t[m.end() - 1:], "DropItemList"):
                add(tag, [int(x) for x in re.findall(r"\{\s*(\d+)\s*,", b)])
                break
    # ---- ShopItem.lua
    t = strip_comments(read(os.path.join(DEC, "ShopItem.src.lua")))
    for key in ("ITEM_ID", "ITEM_LIST", "SALE_ITEM"):
        for b in block_after(t, key):
            add("shop", ints(b))
    # ---- ItemExchangeTable.lua  AddItemExchangeData(npc, ?, src, dst, ...)
    t = strip_comments(read(os.path.join(DEC, "ItemExchangeTable.src.lua")))
    for a in call_args(t, "g_pItemManager:AddItemExchangeData"):
        v = [x.strip() for x in a.split(",")]
        if len(v) >= 4 and v[2].isdigit() and v[3].isdigit():
            add("exchange", [int(v[2]), int(v[3])])
    # ---- ManufactureItem.lua
    t = strip_comments(read(os.path.join(DEC, "ManufactureItem.src.lua")))
    add("manufacture_material", [int(x) for x in re.findall(r"m_MaterialID\s*=\s*(\d+)", t)])
    for a in call_args(t, "g_pManufactureItemManager:AddManufactureResultGroup"):
        v = [x.strip().rstrip(")") for x in a.split(",")]
        if len(v) >= 2 and v[1].isdigit():
            add("manufacture_result", [int(v[1])])
    for a in call_args(t, "g_pManufactureItemManager:AddManufactureVillageData"):
        v = [x.strip() for x in a.split(",")]
        if v and v[0].isdigit():
            add("manufacture_village", [int(v[0])])
    # ---- ItemConvertTable.lua
    t = strip_comments(read(os.path.join(DEC, "ItemConvertTable.src.lua")))
    for key in ("m_TargetItemID", "m_ResultItemID"):
        for b in block_after(t, key):
            add("convert", ints(b))
    for a in call_args(t, "g_pItemManager:AddItemConvertGroup"):
        v = [x.strip().rstrip(")") for x in a.split(",")]
        if len(v) >= 2 and v[1].isdigit():
            add("convert", [int(v[1])])
    # ---- RandomItem.lua / RandomItemData.lua / PackageItemData.lua / CashShopItemList.lua
    t = strip_comments(read(os.path.join(DEC, "RandomItem.src.lua")))
    for a in call_args(t, "g_pItemManager:AddRandomItemTemplet"):
        v = [x.strip() for x in a.split(",")]
        add("random_item", [int(x) for x in v[:2] if x.isdigit()])
    t = strip_comments(read(os.path.join(DEC, "RandomItemData.src.lua")))
    for fn in ("AddAttractionItemInfo", "AddNotifyResultItemInfo", "AddSealRandomItemInfo", "AddCharmItem"):
        for a in call_args(t, "g_pRandomItemManager:" + fn):
            add("random_data", [int(x) for x in re.findall(r"\b(\d+)\b", a.split("true")[0])])
    t = strip_comments(read(os.path.join(DEC, "PackageItemData.src.lua")))
    for a in call_args(t, "g_pCashItemManager:AddPackageItemData"):
        v = [x.strip() for x in a.split(",")]
        add("package", [int(x) for x in v[:2] if x.isdigit()])
    t = strip_comments(read(os.path.join(DEC, "CashShopItemList.src.lua")))
    add("cashshop_list", [x for x in ints(t) if x >= 1000])
    # ---- server plaintext (cube / pvp drop)
    t = strip_comments(read(os.path.join(SRV, "RandomItemTable.lua")))
    add("cube_server", [int(x) for x in re.findall(r"m_ItemID\s*=\s*(\d+)", t)])
    add("cube_server", [int(x) for x in re.findall(r"m_KeyItemID\s*=\s*(\d+)", t) if int(x)])
    add("cube_server", [int(m.group(1)) for m in re.finditer(r"AddRandomItemGroup\(\s*\d+\s*,\s*(\d+)", t)])
    t = strip_comments(read(os.path.join(SRV, "MiracleCubeTable.lua")))
    add("miracle_cube_server", [int(x) for x in re.findall(r"AddMiracleCubeInfo\(\s*(\d+)", t)])
    return src


def main():
    M, J = K.Kim(MARCH_KIM), K.Kim(JUNE_KIM)
    mset, jset = set(M.ids()), set(J.ids())
    src = collect()
    # keep only ids that exist in at least one kim; report the rest per source as "not an item id / unknown"
    rows, unknown = [], collections.Counter()
    per_tag = collections.defaultdict(lambda: [0, 0, 0])  # total, in March, in June
    for i, tags in sorted(src.items()):
        if i not in mset and i not in jset:
            for t in tags:
                unknown[t] += 1
            continue
        rows.append((i, ",".join(sorted(tags)), int(i in mset), int(i in jset)))
        for t in tags:
            per_tag[t][0] += 1
            per_tag[t][1] += int(i in mset)
            per_tag[t][2] += int(i in jset)
    out = os.path.join(HERE, "k1_reachable_ids.tsv")
    with open(out, "w", encoding="utf-8", newline="\n") as fh:
        fh.write("id\tsources\tin_march\tin_june\n")
        for r in rows:
            fh.write("%d\t%s\t%d\t%d\n" % r)
    sys.stdout.write("wrote %s: %d ids (in a kim)\n" % (out, len(rows)))
    sys.stdout.write("%-22s %8s %8s %8s %8s\n" % ("source", "ids", "inMarch", "inJune", "notInKim"))
    for t in sorted(per_tag):
        sys.stdout.write("%-22s %8d %8d %8d %8d\n" % (t, per_tag[t][0], per_tag[t][1], per_tag[t][2], unknown[t]))
    sys.stdout.write("not in either kim (any source): %d\n" % sum(1 for i in src if i not in mset and i not in jset))
    bad = [i for i, _, m, j in rows if m and not j]
    sys.stdout.write("in March but NOT June: %d  %s\n" % (len(bad), bad[:40]))


if __name__ == "__main__":
    main()
