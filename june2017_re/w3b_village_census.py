"""W3b (2026-11-12): june2017_re/w3b_village_census.tsv, one row per place and check, every row explained.
Inputs: w3b_asset_closure.tsv (run w3b_asset_closure.py first; after staging it shows what is left), the staged data034
DLG_Map_Village.lua / DLG_Map_House.lua against june_src_fixed (luatable), and the routing notes below.
    python june2017_re/w3b_village_census.py
"""
import os, sys, collections
TRUNK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(TRUNK, "june2017_re"))
sys.path.insert(0, os.path.join(TRUNK, "tools"))
import luatable, kom

KC = kom.KOM_ROOT
JS = os.path.join(TRUNK, "june_src_fixed", "data034_kom")
STAGED_BY_W3B = {l.split("\t")[0].lower() for l in open(os.path.join(TRUNK, "june2017_re", "w3b_stage_assets.tsv"), encoding="utf-8").read().splitlines()[1:]}
STAGED_BY_W3B |= {"elysion_village_festival.lua", "elysion_village_xmas.lua"}     # luasrc adopt-june, june_adopt_20261112_091808.tsv

# Explanations for rows that stay different on purpose (key = (check, detail prefix)).
GRAPH = ("kept W3f: user hybrid (2026-11-12) - June's villages, March's route; this portal keeps March's target so no field is cut off "
         "(W3c had ported June's route; reader: VillageParsing BattleFieldId, X2StateField ranges 20000-20010 / 1000-1008)")
ROSTER = "routed W3d: June's village NPC roster (houses June lists that March does not; their services / guild / event gates need checking first)"
MISSING_BOTH = {
    "ruben_bank_close.dds": "June's Elder_Village_Bank.Y / Elder_Village_G02.y name it and June ships no such texture: June logs the same miss (parity)",
    "ruben_techhouse_ganpan.dds": "named by June's Ruben / Elder tech-house meshes; absent in June too (parity)",
    "ruben_village_herb01.dds": "named by June's Ruben_Villeage_alpaGR1-22.Y; absent in June too (parity)",
    "ruben_village_herb02.dds": "named by June's Ruben_Villeage_alpaGR1-22.Y; absent in June too (parity)",
    "field_elder_roadtile2.dds": "named by June's Field_Elder_Stage0_Ground2/4/5 and StageRest_ground6/8; absent in June too (parity)",
    "ruben_dungeon_gate.lua": "WorldData WI_DUNGEON_GATE_RUBEN names a file neither client has; the Ruben gate is not a reachable world (parity)",
    "motion_npc_elsion_hugo.xet": "HI_HUGO NPCAniTex names an XET June does not ship either (parity, harmless: OpenXET noise)",
}
# W3f (user 2026-11-12, March's route on June's villages): March's copy put back on purpose (backup kom_backup_stage_june/20261112_w3f_june_boards).
KEPT_MARCH_W3F = {
    "besma_village_alpha_text.y": "Besma end boards: March's names Bethagara Falls / Twin Watchtower (March's route), June's Dicey Sky Road / Wally's Memorial Bridge",
    "field_peita_villeage_sign.y": "Peita board: March's names Pilgrim's Gateway (Peita field 1), June's Dicey Sky Road (June's route)",
    "field_elder_stagerest_alpaguide.y": "Elder rest boards: March's names Wally's Memorial Bridge / Twin Watchtower, June's Bethma Village (June's route)",
    "field_besma_rest_g01_navi.y": "Besma rest sign: March's names Besma fields 1-3 + Pilgrim's Gateway, June's only the three villages (June's route)",
    "altera_village_typo.y": "Altera boards: March's names Toretugera Canyon / Graveyard of Purification, June's Dicey Sky Road (June's route)",
    "altera_village_g02.y": "Altera ground: March's carries the walkway signpost (Field_Altera_waysign) and step; June's drops both and adds a crate closing the ledge",
    "linemap_altera_village.lua": "March's line map keeps the bottom walkway to Altera field 3 (StartPos 67 / 68); user chose March's whole over June's",
}



def sp_map(lst):
    return {r.get("StartPosId"): r for r in (lst or {}).values()} if isinstance(lst, dict) else {}


def main():
    rows = []
    add = lambda place, check, result, why: rows.append((place, check, result, why))
    # ---- assets
    per = collections.defaultdict(collections.Counter)
    odd = collections.defaultdict(list)
    for l in open(os.path.join(TRUNK, "june2017_re", "w3b_asset_closure.tsv"), encoding="utf-8").read().splitlines()[1:]:
        place, asset, via, state = l.split("\t")
        per[place][state] += 1
        if state != "same":
            odd[place].append((asset, via, state))
    for place in sorted(per):
        c = per[place]
        add(place, "assets", "%d same, %d other" % (c["same"], sum(c.values()) - c["same"]),
            "closure Lua -> mesh -> texture / XET / TET (w3b_asset_closure.tsv); %d June file(s) staged by W3b in this place's closure" %
            len({a.lower() for a, v, s in odd[place]} & STAGED_BY_W3B) if True else "")
        for asset, via, state in odd[place]:
            if state == "missing-both":
                add(place, "asset " + asset, state, MISSING_BOTH.get(asset.lower(), "UNEXPLAINED"))
            elif state in ("differs", "march-missing") and asset.lower() in KEPT_MARCH_W3F:
                add(place, "asset " + asset, state, "kept March's W3f: " + KEPT_MARCH_W3F[asset.lower()])
            elif state in ("differs", "march-missing"):
                add(place, "asset " + asset, state, "UNEXPLAINED (re-run after staging)" if asset.lower() not in STAGED_BY_W3B
                    else "staged from June by W3b; re-run the closure after the copy to see it as same")
            else:
                add(place, "asset " + asset, state, "UNEXPLAINED")
    for n, why in sorted(MISSING_BOTH.items()):
        if n.endswith(".dds"):
            add("(log 2026-11-12 8:48)", "OpenTexture " + n, "missing-both", why)
    # ---- Village blocks
    m = luatable.load(os.path.join(KC, "data034_kom", "DLG_Map_Village.lua"))["Village"]
    j = luatable.load(os.path.join(JS, "DLG_Map_Village.src.lua"))["Village"]
    for vid in sorted(set(m) | set(j), key=str):
        place = str(vid).split(".")[-1]
        if vid not in m:
            add(place, "Village block", "june-only", ROSTER.replace("houses", "places and houses") + "; June-only place (spa / guild agit), no March world or system")
            continue
        a, b = m[vid], j.get(vid, {})
        diff = 0
        for k in sorted(set(a) | set(b), key=str):
            if k == "START_POS":
                A, B = sp_map(a.get(k)), sp_map(b.get(k))
                for i in sorted(set(A) | set(B), key=lambda x: (x is None, x)):
                    ra, rb = A.get(i), B.get(i)
                    if ra == rb:
                        continue
                    diff += 1
                    if ra is None:
                        if rb.get("IsSpaEnter") == "TRUE":
                            add(place, "START_POS %s" % i, "june-only", ROSTER + "; spa entrance to June-only VMI_SPA_*")
                        else:
                            add(place, "START_POS %s" % i, "june-only", "UNEXPLAINED")
                        continue
                    if rb is None:
                        add(place, "START_POS %s" % i, "march-only", GRAPH if ra.get("BattleFieldId") else "UNEXPLAINED")
                        continue
                    ren = dict(ra)
                    if "BattleFieldId" in ren:
                        ren["OtherPlaceID"] = ren.pop("BattleFieldId")
                    if "BattleFieldStartLineIndex" in ren:
                        ren["OtherPlaceStartLineIndex"] = ren.pop("BattleFieldStartLineIndex")
                    if ren == rb:
                        add(place, "START_POS %s" % i, "key-name only",
                            "June OtherPlaceID / OtherPlaceStartLineIndex = March BattleFieldId / BattleFieldStartLineIndex, same values (atlas A5 naming, reader X2LocationManager.cpp:481-482)")
                    else:
                        add(place, "START_POS %s" % i, "differs", GRAPH)
            elif k == "HouseIDList":
                A, B = list((a.get(k) or {}).values()), list((b.get(k) or {}).values())
                if A != B:
                    diff += 1
                    add(place, "HouseIDList", "-%s +%s" % ([x.split(".")[-1] for x in A if x not in B], [x.split(".")[-1] for x in B if x not in A]), ROSTER)
            elif a.get(k) != b.get(k):
                diff += 1
                add(place, "Village key " + k, "differs", "UNEXPLAINED")
        if not diff:
            add(place, "Village block", "same as June", "every key incl. START_POS (W3b moved Elder 20 / 22 x and Hamel lounge 87 -> 88 to June)")
    # ---- House blocks
    hm = luatable.load(os.path.join(KC, "data034_kom", "DLG_Map_House.lua"))["House"]
    hj = luatable.load(os.path.join(JS, "DLG_Map_House.src.lua"))["House"]
    SERVICE = {"EXCHANGE", "MAKING_NPC", "OLD_ENCHANT", "OLD_ADVANCED_ENCHANT", "HENIR_EQUIP_UPGRADE", "EXTRACTION", "TRADE", "SYNTHESIS",
               "GUILD", "GURA_2014_EVENT", "EVENT2", "EVENT3"}
    for h in sorted(set(hm) | set(hj), key=str):
        name = str(h).split(".")[-1]
        if h not in hm:
            add(name, "House block", "june-only", ROSTER + "; June-only house")
            continue
        if h not in hj:
            add(name, "House block", "march-only", "kept: March event house June dropped; no village list names it in June (no effect)")
            continue
        a, b = hm[h], hj[h]
        ks = [k for k in sorted(set(a) | set(b), key=str) if a.get(k) != b.get(k)]
        talk = [k for k in ks if k.endswith("_Talk") or k.startswith("NPC_TALK_LIST") or k in ("NPC_COMMON_TALK", "TALK_BOX", "NPCJoinTalk", "NPCByeTalk")]
        serv = [k for k in ks if k in SERVICE]
        cnl = [k for k in ks if k in ("COMMON_NPC_LOCATION", "CUSTOM_NPC_LOACION")]
        rest = [k for k in ks if k not in talk and k not in serv and k not in cnl]
        if talk:
            add(name, "talk keys", ",".join(talk)[:200], "routed W3d: NPC talk tables (June string ids, gift / daily talk); not layout")
        if serv:
            add(name, "service keys", ",".join(serv), "routed W3d: NPC menu services June turns on / off; each needs its system checked offline")
        if cnl:
            A = [tuple(r.values()) for r in (a.get("COMMON_NPC_LOCATION") or {}).values()]
            B = [tuple(r.values()) for r in (b.get("COMMON_NPC_LOCATION") or {}).values()]
            if sorted(map(str, A)) == sorted(map(str, B)):
                add(name, "COMMON_NPC_LOCATION", "order only", "same rows, other order; the reader matches by world id (X2TFieldNpc.cpp:1847-1867)")
            else:
                ja = [r for r in B if r not in A]
                ma = [r for r in A if r not in B]
                why = []
                if ja and all(str(r[0]).endswith(("WI_SPA_RUBEN", "WI_SPA_RANOX", "WI_GUILD_AGIT")) for r in ja):
                    why.append("June-only rows are for June-only worlds (spa / guild agit): routed W3d")
                if ma:
                    why.append("March-only rows; house is in no March or June village list" if name in ("HI_LUTO",) else "UNEXPLAINED March-only rows")
                add(name, "COMMON_NPC_LOCATION", "+%d june / +%d march rows" % (len(ja), len(ma)), "; ".join(why) or "UNEXPLAINED")
        if rest:
            unlisted = name in ("HI_APINK", "HI_LUTO")
            add(name, "layout / look keys", ",".join(rest),
                "kept: house is in no March village list (June lists it: routed W3d with the roster)" if unlisted else "UNEXPLAINED")
        if not ks:
            add(name, "House block", "same as June", "")
    # ---- other map Lua
    add("(all)", "DLG_Map_Village_*_Back / _Front / _House_Button.lua", "same", "every village minimap dialog equals ljconv(June)")
    add("(all)", "DLG_Map_Local_*.lua (region map UI)", "22 differ, 10 June-only, 22 March-only",
        "routed Stage 9 bullet: region-map dialogs; the 2026-10-09 user rollback keeps the core-list ones March's, a partial wave is not redone here")
    add("(all)", "DLG_Map_House_1.lua / _2.lua", "differs / June-only", "routed W3d: house dialog data that goes with the NPC roster")
    add("(all)", "MapData.lua AddMapData (BEFORE / AFTER)", "same as June for the 20 shared rows",
        "W3c: shared rows were already June's; W3c added the Lanox rest row; VMI_GUILD_AGIT / VMI_SPA_RANOX / VMI_SPA_RUBEN rows routed W3d (June-only places)")
    add("(all)", "MapData.lua AddLocalMapInfo", "June 201, March 186", "routed Stage 9 with the region-map dialogs")
    with open(os.path.join(TRUNK, "june2017_re", "w3b_village_census.tsv"), "w", encoding="utf-8", newline="\n") as f:
        f.write("place\tcheck\tresult\twhy\n")
        for r in rows:
            f.write("\t".join(str(x) for x in r) + "\n")
    un = [r for r in rows if "UNEXPLAINED" in r[3]]
    print("rows", len(rows), "unexplained", len(un))
    for r in un:
        print("  ", r)


main()
