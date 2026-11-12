"""W3b (2026-11-12): full asset closure of every village / rest area / dungeon gate / lounge world, compared with June by name and bytes.
For each Village[] block of the staged DLG_Map_Village.lua: its WORLD_ID -> WorldData.lua row -> every world .lua (plain + variants) ->
every .lua (line map), mesh, texture and sound it names -> every texture / mesh inside each mesh (June's copy when June has it, inflated)
-> recursively. Plus each house of its HouseIDList: NPCMeshName / NPCAniTex / NPCTextureName and their closure.
Writes june2017_re/w3b_asset_closure.tsv (place, asset, via, state) and june2017_re/w3b_need_stage.tsv (name column, for kom.py stage-june).
States: same | differs (June replaces, stage) | march-missing (stage) | june-absent (March only, kept) | missing-both | undecodable.
    python june2017_re/w3b_asset_closure.py
"""
import os, sys, collections, hashlib
TRUNK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(TRUNK, "tools"))
sys.path.insert(0, os.path.join(TRUNK, "june2017_re"))
import kom, junekom, luatable, fxdata, types
# npc_mesh_census's index + tokens() / resolve(), without running its census (that part starts at "# which lua files").
_src = open(os.path.join(TRUNK, "june2017_re", "npc_mesh_census.py"), encoding="utf-8").read().split("# which lua files")[0]
nmc = types.ModuleType("nmc")
_argv, sys.argv = sys.argv, [sys.argv[0], "--worlds", os.devnull]
exec(compile(_src, "npc_mesh_census.py", "exec"), nmc.__dict__)
sys.argv = _argv

KC = kom.KOM_ROOT
GAME = kom.GAME_DIR
staged = kom.staged_names()
live = kom.live_names()
june = nmc.june
MODEL, TEX = (".x", ".y"), (".dds", ".tga", ".png", ".bmp", ".jpg")
SND = (".ogg", ".wav")
KEEP = MODEL + TEX + SND + (".lua", ".xet", ".tet")

_live_kom = {}
def live_bytes(ln):
    for stem in sorted(live.get(ln, {})):
        path = os.path.join(GAME, stem + ".kom")
        if path not in _live_kom:
            k = junekom.Kom(path)
            _live_kom[path] = (k, {e["name"].lower(): e for e in k.entries})
        k, idx = _live_kom[path]
        e = idx.get(ln)
        if e:
            with open(path, "rb") as f:
                f.seek(e["offset"]); raw = f.read(e["csize"])
            out, err = junekom.decode(e, raw, k.v04)
            if out is not None:
                return out
    return None

def march_bytes(ln):
    for stem, nm in sorted(staged.get(ln, {}).items()):
        with open(os.path.join(KC, stem + "_kom", nm), "rb") as f:
            return f.read()
    return live_bytes(ln)

def june_bytes(ln):
    for path, stem, v04, e in june.get(ln, []):
        with open(path, "rb") as f:
            f.seek(e["offset"]); raw = f.read(e["csize"])
        out, err = junekom.decode(e, raw, v04)
        if out is not None:
            return out
    return None

def inflate(b):
    if b and b[:4] == b"xof " and b[8:12] in (b"bzip", b"tzip"):
        return fxdata.xfile_text(b, lenient=True)
    return b

_lj = []
def lua_ref(jb):
    if not _lj:
        import luasrc
        _lj.append(luasrc.ljconv_mod())
    m, table = _lj[0]
    try:
        return m.transcode(jb, table)[0]
    except Exception:
        return None

_state = {}
def state(ln):
    if ln in _state:
        return _state[ln]
    m_has, j_has = ln in staged or ln in live, ln in june
    if not m_has and not j_has:
        s = "missing-both"
    elif not j_has:
        s = "june-absent"
    elif not m_has:
        s = "march-missing"
    else:
        jb, mb = june_bytes(ln), march_bytes(ln)
        if jb is not None and ln.endswith(".lua") and jb[:4] == b"\x1bKL\x82":
            jb = lua_ref(jb)              # March ships ljconv(June bytecode); a core-list file is BOM source (reported as differs)
        s = "undecodable" if jb is None or mb is None else ("same" if jb == mb else "differs")
        if s == "differs" and ln.endswith(".lua") and mb[:3] == b"\xef\xbb\xbf":
            s = "source"                  # core-list BOM source: compared by the Lua diff, not by bytes
    _state[ln] = s
    return s

def names_in(b):
    for tk in nmc.tokens(b):
        n = nmc.resolve(tk)
        if n and n.lower().endswith(KEEP):
            yield n

def closure(roots):
    """roots: [(name, via)] -> {lower: (spelling, via)}; recurse into .lua and meshes."""
    out, todo = {}, list(roots)
    while todo:
        n, via = todo.pop()
        ln = n.lower()
        if ln in out:
            continue
        out[ln] = (n, via)
        if ln.endswith(".tga"):          # a UI sheet's keys live in its .tet (house NPCTextureName + NPCTextureKey)
            tet = n[:-4] + ".tet"
            if tet.lower() not in out and (tet.lower() in june or tet.lower() in staged or tet.lower() in live):
                todo.append((tet, n))
        if ln.endswith(MODEL) or ln.endswith(".lua") or ln.endswith(".xet"):
            b = june_bytes(ln) if ln in june else march_bytes(ln)
            b = inflate(b) if b else b
            if not b:
                continue
            for c in names_in(b):
                if c.lower() not in out:
                    todo.append((c, n))
    return out

def main():
    vil = luatable.load(os.path.join(KC, "data034_kom", "DLG_Map_Village.lua"))["Village"]
    hou = luatable.load(os.path.join(KC, "data034_kom", "DLG_Map_House.lua"))["House"]
    wd = {}
    for name, args in luatable.load(os.path.join(KC, "data040_kom", "WorldData.lua"))["__calls__"]:
        if name.endswith("AddWorldData") and args and isinstance(args[0], dict):
            wd[str(args[0].get("worldID"))] = args[0]
    rows, need = [], {}
    # worlds no Village[] block names: the Arcade rest (a lounge START_POS_WORLD_ID) and the Peita rest (no VMI in March or June)
    extra = {"WI_ARCADE_NEW_STAGE_REST": {"WORLD_ID": "WORLD_ID.WI_ARCADE_NEW_STAGE_REST"},
             "WI_BATTLE_FIELD_PEITA_FIELD_REST_00": {"WORLD_ID": "WORLD_ID.WI_BATTLE_FIELD_PEITA_FIELD_REST_00"}}
    for vid, blk in list(vil.items()) + list(extra.items()):
        place = str(vid).split(".")[-1]
        worlds = [str(blk.get("WORLD_ID"))]
        roots = []
        for wid in worlds:
            w = wd.get(wid, {})
            roots += [(v, "WorldData:" + wid.split(".")[-1]) for k, v in sorted(w.items())
                      if isinstance(v, str) and v.lower().endswith(".lua")]
        for h in (blk.get("HouseIDList") or {}).values():
            hb = hou.get(h, {})
            for key in ("NPCMeshName", "NPCAniTex", "NPCTextureName", "NPCMeshName2nd", "NPCAniTex2nd"):
                if isinstance(hb.get(key), str):
                    roots.append((hb[key], "house:" + str(h).split(".")[-1]))
        for ln, (n, via) in sorted(closure(roots).items()):
            s = state(ln)
            rows.append((place, n, via, s))
            if s in ("differs", "march-missing"):
                need.setdefault(ln, (n, place))
    with open(os.path.join(TRUNK, "june2017_re", "w3b_asset_closure.tsv"), "w", encoding="utf-8", newline="\n") as f:
        f.write("place\tasset\tvia\tstate\n")
        for r in rows:
            f.write("\t".join(r) + "\n")
    with open(os.path.join(TRUNK, "june2017_re", "w3b_need_stage.tsv"), "w", encoding="utf-8", newline="\n") as f:
        f.write("name\tjune_kom\tstate\tfirst_place\n")
        for ln, (n, place) in sorted(need.items()):
            f.write("%s\t%s\t%s\t%s\n" % (n, june[ln][0][1], _state[ln], place))
    c = collections.Counter(r[3] for r in rows)
    print("rows", len(rows), dict(c), "need", len(need))
    per = collections.defaultdict(collections.Counter)
    for r in rows:
        per[r[0]][r[3]] += 1
    for p in sorted(per):
        print("  %-45s %s" % (p, dict(per[p])))

main()
