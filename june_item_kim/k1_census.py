#!/usr/bin/env python3
"""K1 tasks 1-4 (data census), one pass over both kims.  Read-only.

Needs k1_ids.py to have written k1_reachable_ids.tsv first, and the save copy in K1_SCRATCH.
Outputs (june_item_kim/):
  k1_reachable_diff.tsv     per reachable id and field, March value vs June value (structural + asset fields)
  k1_reachable_summary.tsv  per field and per source: how many reachable ids differ
  k1_save_breakers.tsv      every equipped save item whose March/June records differ on anything that gates equipping
  k1_newly_visible.tsv      reachable ids that exist only in June, with their asset status
  k1_enum_census.tsv        value histograms, both kims, against March's limits
  k1_cross_ids.tsv          buff factor / socket option / set ids against the staged tables
  k1_assets_reachable.tsv   asset names of reachable ids: present / missing in March's tree
  k1_assets_all.tsv         the same for every June id (summary counts + the missing names per id class)
"""
import collections
import os
import pickle
import re
import sqlite3
import struct
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)
sys.path.insert(0, os.path.join(TRUNK, "tools"))
sys.path.insert(0, HERE)
import kim as K  # noqa: E402
import k1_ids  # noqa: E402

SCRATCH = k1_ids.SCRATCH
M_PATH, J_PATH = k1_ids.MARCH_KIM, k1_ids.JUNE_KIM
GAME = k1_ids.GAME
STAGED = GAME + r"\kom_contents\data036_kom"
JUNE_SRC = os.path.join(TRUNK, "june_src_fixed", "data036_kom")


def log(s):
    sys.stderr.write(s + "\n")
    sys.stderr.flush()


# ------------------------------------------------------------------ load both kims once (cached)
def load(path, tag):
    cache = os.path.join(SCRATCH, "k1_recs_%s.pkl" % tag)
    st = os.stat(path)
    key = (st.st_size, int(st.st_mtime))
    if os.path.exists(cache):
        with open(cache, "rb") as fh:
            k, recs = pickle.load(fh)
        if k == key:
            return recs
    t0 = time.time()
    kim = K.Kim(path)
    recs = {}
    for i in kim.ids():
        recs[i] = kim.record(i)
    log("decoded %s: %d records in %.0fs" % (tag, len(recs), time.time() - t0))
    with open(cache, "wb") as fh:
        pickle.dump((key, recs), fh, protocol=4)
    return recs


def extra_keys(rec, max_key=None):
    """{group: [(key, value)]} of the Extra pool, optionally dropping keys above max_key."""
    out = {}
    for g, lst in rec["extra"].items():
        out[g] = [(k, repr(v)) for k, _, v in lst if max_key is None or k <= max_key]
    return out


# ------------------------------------------------------------------ March's own enum values, from the staged Enum.lua
def lua_enum(path, name):
    t = open(path, encoding="utf-8-sig", errors="replace").read()
    m = re.search(r"^%s\s*=\s*\{" % re.escape(name), t, flags=re.M)
    if not m:
        raise SystemExit("%s: no table %s" % (path, name))
    i, depth = m.end(), 1
    while depth:
        c = t[i]
        depth += (c == "{") - (c == "}")
        i += 1
    return {k: int(v) for k, v in re.findall(r"(\w+)\s*=\s*(-?\d+)", t[m.end(): i])}


# ------------------------------------------------------------------ GetX2UnitClassCompatibility from X2Unit.h
def class_compat():
    t = open(os.path.join(TRUNK, "X2Lib", "X2Unit.h"), encoding="cp949", errors="replace").read().splitlines()
    start = next(i for i, l in enumerate(t) if l.startswith("inline bool GetX2UnitClassCompatibility"))
    end = next(i for i in range(start, len(t)) if t[i].startswith("}"))
    body, table, cur = t[start:end], {}, None
    for line in body:
        m = re.match(r"\tcase CX2Unit::(UC_\w+):", line)
        if m:
            cur = m.group(1)
            table[cur] = set()
            continue
        if cur:
            for u in re.findall(r"CX2Unit::(UC_\w+)", line):
                table[cur].add(u)
    return table


def main():
    M, J = load(M_PATH, "march"), load(J_PATH, "june")
    log("March %d records, June %d records" % (len(M), len(J)))
    reach = {}
    with open(os.path.join(HERE, "k1_reachable_ids.tsv"), encoding="utf-8") as fh:
        next(fh)
        for line in fh:
            i, s, im, ij = line.rstrip("\n").split("\t")
            reach[int(i)] = (set(s.split(",")), int(im), int(ij))

    # ---------------------------------------------------------------- field groups
    STRUCT = ["item_type", "eqip_position", "unit_type", "unit_class", "use_type", "use_condition", "item_level",
              "item_grade", "set_id", "max_seal_count", "endurance", "period_type", "shop_price_type", "fashion",
              "vested", "can_enchant", "no_equip", "can_use_inventory", "can_hyper_mode", "is_pc_bang", "quantity",
              "price", "price_pvp_point", "use_level", "buy_pvp_rank", "buff_id", "cool_time", "max_attrib_enchant",
              "attribute_level", "summon_npc", "endurance_dmg_min", "endurance_dmg_max", "buff_factors",
              "socket_options", "random_socket_options", "stat.hp", "stat.atk_physic", "stat.atk_magic",
              "stat.def_physic", "stat.def_magic", "stat_rel_lv", "special_abilities"]
    ASSET = ["model_name_%d" % n for n in range(6)] + ["shop_image", "texture_change_xet", "ani_xet", "ani_name",
                                                       "drop_viewer", "attach_frame_0", "attach_frame_1"]
    JUNE_ONLY_BIT = ("flags_bit7", "is_for_minor_unit")

    def fl(rec):
        d = K.flat(rec)
        for k, v in list(d.items()):
            if v == "0" and (k in ASSET or k in ("ani_name", "desc_in_skill_note")):
                d[k] = ""          # March stores "0" for an absent string slot; June stores ""
        return d

    diff_rows = []                       # id, sources, field, march, june
    summ = collections.Counter()         # field -> count of reachable ids that differ
    summ_src = collections.defaultdict(collections.Counter)
    seen_total = 0
    for i, (tags, im, ij) in sorted(reach.items()):
        if not (im and ij):
            continue
        seen_total += 1
        a, b = fl(M[i]), fl(J[i])
        for f in STRUCT + ASSET:
            if f not in a or f not in b:
                continue
            if f == "stat_rel_lv":
                # March 5 bytes vs June 6: compare the shared five
                av = {k: v for k, v in (M[i]["stat_rel_lv"] or {}).items()}
                bv = {k: v for k, v in (J[i]["stat_rel_lv"] or {}).items() if k != "all_stat"}
                if av != bv:
                    diff_rows.append((i, tags, f, av, bv))
                    summ[f] += 1
                    for t in tags:
                        summ_src[t][f] += 1
                continue
            if a[f] != b[f]:
                diff_rows.append((i, tags, f, a[f], b[f]))
                summ[f] += 1
                for t in tags:
                    summ_src[t][f] += 1
        ea, eb = extra_keys(M[i]), extra_keys(J[i], 9)
        if ea != eb:
            diff_rows.append((i, tags, "extra(keys1-9)", ea, eb))
            summ["extra(keys1-9)"] += 1
            for t in tags:
                summ_src[t]["extra(keys1-9)"] += 1
        # fields only June has that change March behaviour
        if J[i].get("flags_bit7"):
            summ["june:pvp_equip_bit7"] += 1
            diff_rows.append((i, tags, "june:pvp_equip_bit7", M[i].get("hide_set_desc"), 1))
        jx = extra_keys(J[i])
        if any(k > 9 for lst in jx.values() for k, _ in lst):
            summ["june:extra_key>9 (Lu/Ciel/Rose/Ain)"] += 1
    with open(os.path.join(HERE, "k1_reachable_diff.tsv"), "w", encoding="utf-8", newline="\n") as fh:
        fh.write("id\tsources\tfield\tmarch\tjune\n")
        for i, tags, f, x, y in diff_rows:
            fh.write("%d\t%s\t%s\t%s\t%s\n" % (i, ",".join(sorted(tags)), f, str(x).replace("\t", " ").replace("\n", " ")[:160],
                                              str(y).replace("\t", " ").replace("\n", " ")[:160]))
    with open(os.path.join(HERE, "k1_reachable_summary.tsv"), "w", encoding="utf-8", newline="\n") as fh:
        fh.write("scope\tfield\tids_differing\tof\n")
        for f, c in sorted(summ.items(), key=lambda kv: (-kv[1], kv[0])):
            fh.write("ALL\t%s\t%d\t%d\n" % (f, c, seen_total))
        per = collections.Counter()
        for i, (tags, im, ij) in reach.items():
            if im and ij:
                for t in tags:
                    per[t] += 1
        for t in sorted(summ_src):
            for f, c in sorted(summ_src[t].items(), key=lambda kv: (-kv[1], kv[0])):
                fh.write("%s\t%s\t%d\t%d\n" % (t, f, c, per[t]))
    log("reachable ids in both kims: %d; differing rows %d" % (seen_total, len(diff_rows)))

    # ---------------------------------------------------------------- save-breakers
    scr = os.path.join(SCRATCH, "els_db.sql")
    db = sqlite3.connect(scr)
    cls_name = {v: k for k, v in lua_enum(STAGED + r"\Enum.src.lua", "UNIT_CLASS").items()}
    jun_cls = {v: k for k, v in lua_enum(os.path.join(JUNE_SRC, "Enum.src.lua"), "UNIT_CLASS").items()}
    eq_name = {v: k for k, v in lua_enum(STAGED + r"\Enum.src.lua", "EQIP_POSITION").items()}
    compat = class_compat()
    rows = db.execute("select u.unit_uid, u.nickname, u.unit_class, i.item_id, i.category, i.slot, i.equipped "
                      "from item i join unit u on u.unit_uid=i.unit_uid where i.equipped=1 order by u.unit_uid, i.slot").fetchall()

    name_to_id = lua_enum(STAGED + r"\Enum.src.lua", "UNIT_CLASS")

    def class_ids_for(wn):
        return [name_to_id[n] for n in compat.get(wn, ()) if n in name_to_id]

    sb = []
    unit_eq_counts = collections.Counter()
    for uid, nick, ucls, iid, cat, slot, eqd in rows:
        unit_eq_counts[(uid, nick, ucls)] += 1
        m, j = M.get(iid), J.get(iid)
        if m is None or j is None:
            sb.append((uid, nick, ucls, iid, "MISSING_IN_" + ("MARCH" if m is None else "JUNE"), "", "", ""))
            continue
        wn = cls_name.get(ucls, "?%d" % ucls)
        # eligibility under each record
        elig = {}
        for tag, rec in (("march", m), ("june", j)):
            cond = rec["use_condition"]
            if rec["use_type"] == 0:
                elig[tag] = False
            elif cond == 0:
                elig[tag] = False
            elif cond == 1:
                elig[tag] = True
            elif cond == 2:
                elig[tag] = rec["unit_type"]    # the wearer's type is compared below
            else:
                elig[tag] = rec["unit_class"] in class_ids_for(wn)
        fields = []
        for f in ("eqip_position", "unit_type", "unit_class", "use_type", "use_condition", "item_level", "no_equip",
                  "item_type", "fashion"):
            if m[f] != j[f]:
                fields.append("%s %s->%s" % (f, m[f], j[f]))
        if elig["march"] != elig["june"] or fields:
            verdict = "ELIGIBILITY_CHANGES" if elig["march"] != elig["june"] else "gating field differs, eligibility same"
            if m["eqip_position"] != j["eqip_position"]:
                verdict = "EQUIP_POSITION_CHANGES"
            sb.append((uid, nick, ucls, iid, verdict, "; ".join(fields), "march_elig=%s june_elig=%s" % (elig["march"], elig["june"]),
                       "slot %s; March name '%s'" % (slot, m["name"][:40])))
    with open(os.path.join(HERE, "k1_save_breakers.tsv"), "w", encoding="utf-8", newline="\n") as fh:
        fh.write("unit_uid\tnickname\tunit_class\titem_id\tverdict\tfields_march_to_june\teligibility\tnote\n")
        for r in sb:
            fh.write("\t".join(str(x) for x in r) + "\n")
        fh.write("# equipped rows checked: %d across %d unit(s); breakers/diffs listed: %d\n" % (
            len(rows), len(unit_eq_counts), len(sb)))
    log("save: %d equipped rows, %d flagged" % (len(rows), len(sb)))

    # ---------------------------------------------------------------- newly visible (reachable, June only)
    # ---------------------------------------------------------------- asset index
    t0 = time.time()
    have = set()
    for root in (GAME + r"\kom_contents", GAME + r"\data"):
        for dp, dn, fn in os.walk(root):
            for f in fn:
                have.add(f.lower())
    log("asset index: %d names in %.0fs" % (len(have), time.time() - t0))

    def asset_names(rec):
        out = []
        for n in range(8):
            v = rec.get("model_name_%d" % n)
            if v and v != "0":
                out.append(("model%d" % n, v))
        for key in ("shop_image", "texture_change_xet", "ani_xet"):
            if rec.get(key) and rec[key] != "0":
                out.append((key, rec[key]))
        for g in ("common_model", "common_xet"):
            for k, nm, v in rec["extra"].get(g, []):
                if v and v != "0":
                    out.append(("%s[%s]" % (g, nm), v))
        return out

    def present(name):
        n = name.replace("\\", "/").split("/")[-1].lower()
        return n in have

    nv = []
    for i, (tags, im, ij) in sorted(reach.items()):
        if im or not ij:
            continue
        miss = [(k, n) for k, n in asset_names(J[i]) if not present(n)]
        nv.append((i, tags, J[i]["name"], J[i]["item_type"], J[i]["eqip_position"], J[i]["unit_type"], J[i]["unit_class"],
                   len(asset_names(J[i])), miss))
    with open(os.path.join(HERE, "k1_newly_visible.tsv"), "w", encoding="utf-8", newline="\n") as fh:
        fh.write("id\tsources\tjune_name\titem_type\teqip_position\tunit_type\tunit_class\tn_assets\tmissing_assets\n")
        for i, tags, nm, it, ep, ut, uc, na, miss in nv:
            fh.write("%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\n" % (i, ",".join(sorted(tags)), nm.replace("\t", " ")[:50], it, ep, ut, uc, na,
                                                              "|".join("%s:%s" % x for x in miss)))
    log("newly visible (reachable, June only): %d" % len(nv))

    # ---------------------------------------------------------------- assets of reachable ids: June names March lacks
    reg = []                              # (id, kind, march_name, june_name, june_present, march_present)
    for i, (tags, im, ij) in sorted(reach.items()):
        if not (im and ij):
            continue
        am, aj = dict(), dict()
        for k, n in asset_names(M[i]):
            am.setdefault(k, n)
        for k, n in asset_names(J[i]):
            aj.setdefault(k, n)
        for k in aj:
            if am.get(k) != aj[k]:
                rel = "june-unit-only (Lu/Ciel/Rose/Ain)" if re.search(r"\[(Lu|Ciel|Rose|Ain)\]", k) else "march-visible"
                reg.append((i, ",".join(sorted(tags)), k, am.get(k, ""), aj[k], int(present(aj[k])), int(present(am[k])) if k in am else -1, rel))
    with open(os.path.join(HERE, "k1_assets_reachable.tsv"), "w", encoding="utf-8", newline="\n") as fh:
        fh.write("id\tsources\tkind\tmarch_name\tjune_name\tjune_file_in_march_tree\tmarch_file_in_march_tree\trelevance\n")
        for r in reg:
            fh.write("%d\t%s\t%s\t%s\t%s\t%d\t%d\t%s\n" % r)
    vis = [r for r in reg if r[5] == 0]
    visM = [r for r in vis if r[7] == "march-visible"]
    log("reachable id/kind where June's name differs from March's: %d; June file absent from March's tree: %d (march-visible kinds: %d, ids %d)" % (
        len(reg), len(vis), len(visM), len({r[0] for r in visM})))

    # all June ids
    allmiss = collections.Counter()
    all_ids_missing = {}
    kinds = collections.Counter()
    for i, rec in J.items():
        mi = [(k, n) for k, n in asset_names(rec) if not present(n)]
        kinds["items_total"] += 1
        if mi:
            kinds["items_with_missing_asset"] += 1
            all_ids_missing[i] = mi
            for k, n in mi:
                allmiss[k.split("[")[0]] += 1
    with open(os.path.join(HERE, "k1_assets_all.tsv"), "w", encoding="utf-8", newline="\n") as fh:
        fh.write("# every June id: items with >=1 referenced model/xet/shop_image absent from March's kom_contents + game dir\n")
        fh.write("scope\tkind\tcount\n")
        fh.write("june_all\titems_total\t%d\n" % kinds["items_total"])
        fh.write("june_all\titems_with_missing_asset\t%d\n" % kinds["items_with_missing_asset"])
        for k, c in allmiss.most_common():
            fh.write("june_all\tmissing_name_kind:%s\t%d\n" % (k, c))
        fh.write("# per id (june only, item types that decide visibility)\nid\tin_march\tmissing\n")
        for i in sorted(all_ids_missing):
            fh.write("%d\t%d\t%s\n" % (i, int(i in M), "|".join("%s:%s" % x for x in all_ids_missing[i][:6])))
    log("June ids with a missing asset: %d of %d" % (kinds["items_with_missing_asset"], kinds["items_total"]))

    # ---------------------------------------------------------------- enum census
    mx = lambda d: max(d.values())
    ET = {}
    for nm in ("ITEM_TYPE", "ITEM_GRADE", "SPECIAL_ABILITY_TYPE", "EQIP_POSITION", "UNIT_CLASS", "USE_TYPE",
               "USE_CONDITION", "PERIOD_TYPE", "SHOP_PRICE_TYPE"):
        try:
            ET[nm] = lua_enum(STAGED + r"\Enum.src.lua", nm)
        except SystemExit:
            ET[nm] = None
    rows = []

    def hist(recs, field):
        h = collections.Counter()
        ex = {}
        for i, r in recs.items():
            if field == "special_ability_type":
                vals = [s["type"] for s in r["special_abilities"]]
            elif field == "extra_key":
                vals = [k for lst in r["extra"].values() for k, _, _ in lst]
            elif field == "flags2_bit2":
                vals = [int(r["is_for_minor_unit"])] if "is_for_minor_unit" in r else [0]
            elif field == "flags_bit7(June=PvP equip)":
                vals = [r.get("flags_bit7", 0)]
            elif field == "hide_set_desc":
                vals = [r["hide_set_desc"]]
            else:
                vals = [r[field]]
            for v in vals:
                h[v] += 1
                ex.setdefault(v, i)
        return h, ex

    for field, lim in (("item_type", ("IT_NUM", 0, 13)), ("item_grade", ("IG_LEGEND on", 0, 6)), ("use_type", ("UT_NUM", 0, 3)),
                       ("use_condition", ("UC_NUM", 0, 3)), ("period_type", ("PT_NUM", 0, 2)),
                       ("shop_price_type", ("SPT_NUM", 0, 2)), ("unit_type", ("UT_END-1", 0, 9)),
                       ("unit_class", ("last UNIT_CLASS", 0, 122)), ("eqip_position", ("EP_END-1", 0, 38)),
                       ("special_ability_type", ("SAT_END-1", 0, 39)), ("extra_key", ("UNIT_TYPE 1..9", 1, 9)),
                       ("flags_bit7(June=PvP equip)", ("March: HideSetDesc", 0, 1)), ("flags2_bit2", ("March flags2 bit0 only", 0, 0)),
                       ("hide_set_desc", ("both", 0, 1))):
        hm, em = hist(M, field)
        hj, ej = hist(J, field)
        for v in sorted(set(hm) | set(hj)):
            out_of = "" if lim[1] <= v <= lim[2] else "OUT_OF_MARCH_RANGE(%s %d..%d)" % lim
            rows.append((field, v, hm.get(v, 0), hj.get(v, 0), out_of, ej.get(v, em.get(v, ""))))
    with open(os.path.join(HERE, "k1_enum_census.tsv"), "w", encoding="utf-8", newline="\n") as fh:
        fh.write("field\tvalue\tcount_march\tcount_june\tflag\texample_id\n")
        for r in rows:
            fh.write("\t".join(str(x) for x in r) + "\n")
    log("enum census rows: %d" % len(rows))

    # ---------------------------------------------------------------- enum name equality, March Enum.lua vs June Enum.lua
    names = []
    for nm, upto in (("SPECIAL_ABILITY_TYPE", 39), ("ITEM_TYPE", 13)):
        a = {v: k for k, v in lua_enum(STAGED + r"\Enum.src.lua", nm).items()}
        b = {v: k for k, v in lua_enum(os.path.join(JUNE_SRC, "Enum.src.lua"), nm).items()}
        for v in range(0, upto + 1):
            names.append((nm, v, a.get(v, ""), b.get(v, ""), "same" if a.get(v) == b.get(v) else "DIFFERENT"))
        for v in sorted(set(b) - set(a)):
            names.append((nm, v, "", b[v], "June only"))
    with open(os.path.join(HERE, "k1_enum_names.tsv"), "w", encoding="utf-8", newline="\n") as fh:
        fh.write("enum\tvalue\tmarch_name\tjune_name\tverdict\n")
        for r in names:
            fh.write("\t".join(str(x) for x in r) + "\n")
    log("enum names: %d different" % sum(1 for r in names if r[4] == "DIFFERENT"))

    # ---------------------------------------------------------------- cross-table ids
    bf_march = set(lua_enum(STAGED + r"\Enum.src.lua", "BUFF_FACTOR_ID").values())
    bf_all_j = collections.Counter()
    bf_missing = collections.defaultdict(list)
    for i, r in J.items():
        for f in r["buff_factors"]:
            bf_all_j[f] += 1
            if f not in bf_march:
                bf_missing[f].append(i)
    reach_bf_missing = {f: [i for i in ids if i in reach] for f, ids in bf_missing.items()}
    # socket option ids in OptionItemData (the staged copy is June's)
    opt = open(STAGED + r"\OptionItemData.src.lua", encoding="utf-8-sig", errors="replace").read()
    opt_ids = set(int(x) for x in re.findall(r"\n\tID = (\d+),", opt))
    sock_miss = collections.defaultdict(list)
    rsock_miss = collections.defaultdict(list)
    for i, r in J.items():
        for s in r["socket_options"]:
            if s not in opt_ids:
                sock_miss[s].append(i)
        for s in r["random_socket_options"]:
            if s not in opt_ids:
                rsock_miss[s].append(i)
    # set ids
    jk = K.Kim(J_PATH)
    mk = K.Kim(M_PATH)
    set_missing = [i for i, r in J.items() if r["set_id"] and jk.set_record(r["set_id"]) is None]
    set_missing_m = [i for i, r in M.items() if r["set_id"] and mk.set_record(r["set_id"]) is None]
    with open(os.path.join(HERE, "k1_cross_ids.tsv"), "w", encoding="utf-8", newline="\n") as fh:
        fh.write("check\tcount\tdetail\n")
        fh.write("june buff-factor ids used (distinct)\t%d\t\n" % len(bf_all_j))
        fh.write("june buff-factor ids absent from staged Enum.lua BUFF_FACTOR_ID (distinct)\t%d\t%s\n" % (
            len(bf_missing), sorted(bf_missing)[:30]))
        fh.write("  ... of which carried by a reachable id\t%d\t%s\n" % (
            sum(1 for ids in reach_bf_missing.values() if ids),
            {f: ids[:3] for f, ids in reach_bf_missing.items() if ids}))
        fh.write("OptionItemData.lua socket option ids defined\t%d\t\n" % len(opt_ids))
        fh.write("june socket_options ids absent from OptionItemData (distinct / items)\t%d / %d\t%s\n" % (
            len(sock_miss), len({i for v in sock_miss.values() for i in v}), sorted(sock_miss)[:20]))
        fh.write("june random_socket_options ids absent from OptionItemData (distinct / items)\t%d / %d\t%s\n" % (
            len(rsock_miss), len({i for v in rsock_miss.values() for i in v}), sorted(rsock_miss)[:20]))
        fh.write("june items whose set_id has no set record\t%d\t%s\n" % (len(set_missing), set_missing[:20]))
        fh.write("march items whose set_id has no set record (baseline)\t%d\t%s\n" % (len(set_missing_m), set_missing_m[:20]))
        fh.write("set options referenced by June set records: see k1 section in the plan (set option ids are in OptionItemData too)\t\t\n")
        so_miss = collections.Counter()
        so_tot = 0
        for sid in range(jk.nsets):
            sidv = jk.u32(jk.setid_ofs + 4 * sid)
            rec = jk.set_record(sidv)
            for need, opt_id in rec["need_parts_and_option"]:
                so_tot += 1
                if opt_id not in opt_ids:
                    so_miss[opt_id] += 1
        fh.write("june set option refs absent from OptionItemData (refs / distinct ids)\t%d / %d\t%s (of %d refs)\n" % (
            sum(so_miss.values()), len(so_miss), sorted(so_miss)[:20], so_tot))


    # March baselines: the same checks on March's own records, so a June number is read against what already is
    m_sock = {s for r in M.values() for s in r["socket_options"] if s not in opt_ids}
    m_rsock = {s for r in M.values() for s in r["random_socket_options"] if s not in opt_ids}
    m_so = collections.Counter()
    for n in range(mk.nsets):
        for need, opt_id in mk.set_record(mk.u32(mk.setid_ofs + 4 * n))["need_parts_and_option"]:
            if opt_id not in opt_ids:
                m_so[opt_id] += 1
    sit = open(os.path.join(k1_ids.SCRATCH, "dec", "SocketItemTable.src.lua"), encoding="utf-8", errors="replace").read()
    in_sit = {int(m.group(1)) for m in re.finditer(r"AddSocketRandomData\([^,]+,\s*(\d+)\s*,", sit)}
    j_rsock_neither = {s for s in rsock_miss if s not in in_sit}
    with open(os.path.join(HERE, "k1_cross_ids.tsv"), "a", encoding="utf-8", newline="\n") as fh:
        fh.write("BASELINE march socket_options absent from OptionItemData (distinct)\t%d\t\n" % len(m_sock))
        fh.write("BASELINE march random_socket_options absent from OptionItemData (distinct)\t%d\t%s\n" % (len(m_rsock), sorted(m_rsock)[:12]))
        fh.write("BASELINE march set option refs absent from OptionItemData (refs / distinct)\t%d / %d\t%s\n" % (
            sum(m_so.values()), len(m_so), sorted(m_so)[:12]))
        fh.write("june random_socket ids absent from OptionItemData AND from SocketItemTable.lua AddSocketRandomData (distinct)\t%d\t%s\n" % (
            len(j_rsock_neither), sorted(j_rsock_neither)[:20]))
    log("cross-table written")


if __name__ == "__main__":
    main()
