"""W3c (2026-11-12): asset closure of every battle-field world, compared with June by name and bytes (W3b's closure, fields instead of
villages). Roots per BattleFieldData row: its WORLD_ID -> WorldData.lua row -> every world .lua, plus its LINE_MAP_FILE_NAME.
Writes june2017_re/w3c_field_closure.tsv (field, asset, via, state) and june2017_re/w3c_need_stage.tsv (for kom.py stage-june).
States as in w3b_asset_closure.py. --route limits need_stage to the Ruben - Elder - Besma - Altera - Peita fields.
    python june2017_re/w3c_field_closure.py [--route]
"""
import os, sys, collections
TRUNK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
_src = open(os.path.join(TRUNK, "june2017_re", "w3b_asset_closure.py"), encoding="utf-8").read()
_src = _src[:_src.rindex("\nmain()")]
_argv0 = sys.argv
exec(compile(_src, "w3b_asset_closure.py", "exec"), globals())
sys.argv = _argv0

ROUTE = ("RUBEN_FIELD", "ELDER_FIELD", "BESMA_FIELD", "ALTERA_FIELD", "PEITA_FIELD")


def field_main():
    route_only = "--route" in sys.argv
    wd = {}
    for name, args in luatable.load(os.path.join(KC, "data040_kom", "WorldData.lua"))["__calls__"]:
        if name.endswith("AddWorldData") and args and isinstance(args[0], dict):
            wd[str(args[0].get("worldID"))] = args[0]
    bfd = []
    for name, args in luatable.load(os.path.join(KC, "data036_kom", "BattleFieldData.lua"))["__calls__"]:
        if name.endswith("AddBattleFieldData") and args and isinstance(args[0], dict):
            bfd.append(args[0])
    rows, need = [], {}
    for blk in bfd:
        place = str(blk.get("BATTLE_FIELD_ID")).split(".")[-1]
        wid = str(blk.get("WORLD_ID"))
        w = wd.get(wid, {})
        roots = [(v, "WorldData:" + wid.split(".")[-1]) for k, v in sorted(w.items())
                 if isinstance(v, str) and v.lower().endswith(".lua")]
        if isinstance(blk.get("LINE_MAP_FILE_NAME"), str):
            roots.append((blk["LINE_MAP_FILE_NAME"], "BattleFieldData:LINE_MAP_FILE_NAME"))
        for ln, (n, via) in sorted(closure(roots).items()):
            s = state(ln)
            rows.append((place, n, via, s))
            if s in ("differs", "march-missing") and (not route_only or any(r in place for r in ROUTE)):
                need.setdefault(ln, (n, place))
    with open(os.path.join(TRUNK, "june2017_re", "w3c_field_closure.tsv"), "w", encoding="utf-8", newline="\n") as f:
        f.write("field\tasset\tvia\tstate\n")
        for r in rows:
            f.write("\t".join(r) + "\n")
    with open(os.path.join(TRUNK, "june2017_re", "w3c_need_stage.tsv"), "w", encoding="utf-8", newline="\n") as f:
        f.write("name\tjune_kom\tstate\tfirst_field\n")
        for ln, (n, place) in sorted(need.items()):
            f.write("%s\t%s\t%s\t%s\n" % (n, june[ln][0][1], _state[ln], place))
    c = collections.Counter(r[3] for r in rows)
    print("rows", len(rows), dict(c), "need", len(need), "(route only)" if route_only else "")
    per = collections.defaultdict(collections.Counter)
    for r in rows:
        per[r[0]][r[3]] += 1
    for p in sorted(per):
        print("  %-45s %s" % (p, dict(per[p])))


field_main()
