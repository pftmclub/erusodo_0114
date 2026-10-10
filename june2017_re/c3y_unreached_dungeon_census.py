"""June effect C3y (2026-10-11): the dungeon scripts in kom_contents that no March DungeonData row reaches, per script: who
registers it (June's DungeonData or nobody), its dungeon ids in both DungeonEnums, the worlds it names (WORLD_ID in both Enums,
WorldData row in both, world script and line maps staged / in June), the NPC names March cannot resolve, and the owning plan.

The script set is dungeon_npc_census.py's C3y exceptions (june2017_re/dungeon_npc_parity.tsv, exception "C3y: ..."), re-derived
here from the registries so a stale parity file shows up as a mismatch line instead of a silent count.

Writes
  june2017_re/c3y_unreached_dungeon_census.tsv   one row per script (ids, worlds, gaps, origin, owner)
  june2017_re/c3y_unreached_world_census.tsv     one row per world those scripts name (enum, WorldData, files, line maps)
    python june2017_re/c3y_unreached_dungeon_census.py        (from the Trunk)
"""
import os, re, sys, subprocess, collections
TRUNK = os.getcwd()
sys.path.insert(0, os.path.join(TRUNK, "tools"))
sys.path.insert(0, os.path.join(TRUNK, "june2017_re"))
import kom, junekom, ljconv

KC = kom.KOM_ROOT
JS = os.path.join(TRUNK, "june_src_fixed")
OUT = os.path.join(TRUNK, "june2017_re")
LJ = os.path.join(TRUNK, "elstools", "vendor", "luajit", "luajit.exe")
REG = os.path.join(OUT, "lua_registry_dump.lua")
GAME = os.path.join(os.path.dirname(KC), "data")
# data035 as it was live before the first tools copy (2026-10-09 13:55, the June NPC/dungeon ljconv adoption): March's own list
PRE035 = os.path.join(os.path.dirname(KC), "kom_backup_tools", "data035.kom.20261009_135512")
table = ljconv.load_table(ljconv.TABLE_TSV)


def run(*args):
    r = subprocess.run([LJ, REG] + list(args), capture_output=True, stdin=subprocess.DEVNULL, timeout=600)
    return r.stdout.decode("utf-8", "replace").splitlines()


def enum(path, tname):
    out = {}
    for ln in run("enum", path, tname):
        p = ln.split("\t")
        if p[0] == "K":
            out[p[1]] = p[2]
        elif p[0] == "E":
            print("ERROR", path, ln)
    return out


def calls(path, method):
    rows = []
    for ln in run("calls", path):
        p = ln.split("\t")
        if p[0] == "E":
            print("ERROR", path, ln)
        if p[0] != "C" or p[1] != method:
            continue
        rows.append(dict(kv.partition("=")[::2] for kv in p[2:]))
    return rows


def kc(stem, name):
    return os.path.join(KC, stem + "_kom", name)


def did(d):
    return d.get("dungeonID", "?").replace("E:DUNGEON_ID.", "").replace("E:UNUSED_DUNGEON_ID.", "UNUSED:")


# --- registries ----------------------------------------------------------------------------------------------------------------
m_de = enum(kc("data036", "DungeonEnum.lua"), "DUNGEON_ID")
m_de.update({"UNUSED:" + k: v for k, v in enum(kc("data036", "DungeonEnum.lua"), "UNUSED_DUNGEON_ID").items()})
j_de = enum(os.path.join(JS, "data036_kom", "DungeonEnum.src.lua"), "DUNGEON_ID")
j_de.update({"UNUSED:" + k: v for k, v in enum(os.path.join(JS, "data036_kom", "DungeonEnum.src.lua"), "UNUSED_DUNGEON_ID").items()})
m_de_by_val = collections.defaultdict(list)
for k, v in m_de.items():
    m_de_by_val[v].append(k)
m_wi = enum(kc("data036", "Enum.lua"), "WORLD_ID")
j_wi = enum(os.path.join(JS, "data036_kom", "Enum.src.lua"), "WORLD_ID")
m_wi_by_val = collections.defaultdict(list)
for k, v in m_wi.items():
    m_wi_by_val[v].append(k)

m_dd = calls(kc("data035", "DungeonData.lua"), "AddDungeonData")
j_dd = calls(os.path.join(JS, "data035_kom", "DungeonData.src.lua"), "AddDungeonData")
m_reg, j_reg = collections.defaultdict(list), collections.defaultdict(list)
for d in m_dd:
    m_reg[d.get("dataFileName", "").lower()].append(did(d))
for d in j_dd:
    j_reg[d.get("dataFileName", "").lower()].append(did(d))
m_ids = {did(d) for d in m_dd}


def wd_rows(path):
    # run in the KOG luajit and key by worldID (the engine's key), not worldName: June names WI_EVENT_SCHOOL_EVENT_STAGE0's row
    # "WI_TUTORIAL_EL_STAGE0" and the Eve tutorial's "WI_TUTORIAL_EVE_STAGE0 " (trailing space)
    out = {}
    for d in calls(path, "AddWorldData"):
        w = d.get("worldID", "").replace("E:WORLD_ID.", "")
        if w and w not in out:
            out[w] = d.get("dataFileName", "")
    return out


m_wd = wd_rows(kc("data040", "WorldData.lua"))
j_wd = wd_rows(os.path.join(JS, "data040_kom", "WorldData.src.lua"))
print("DungeonEnum March %d June %d; WORLD_ID March %d June %d; DungeonData March %d June %d; WorldData March %d June %d" % (
    len(m_de), len(j_de), len(m_wi), len(j_wi), len(m_dd), len(j_dd), len(m_wd), len(j_wd)))

# --- file stores -----------------------------------------------------------------------------------------------------------
staged = kom.staged_names()
live = kom.live_names()
pre035 = {e["name"].lower() for e in kom.inspect(PRE035)["files"]} if os.path.exists(PRE035) else set()
june = {}
for path in junekom.all_koms()[0]:
    k = junekom.Kom(path)
    for e in k.entries:
        june.setdefault(e["name"].lower(), (path, k.v04, e))


def march_bytes(name):
    d = staged.get(name.lower())
    if not d:
        return None
    stem, sp = sorted(d.items())[0]
    p = kc(stem, sp)
    return open(p, "rb").read() if os.path.exists(p) else None


def june_bytes(name):
    t = june.get(name.lower())
    if not t:
        return None
    path, v04, e = t
    with open(path, "rb") as fh:
        fh.seek(e["offset"]); raw = fh.read(e["csize"])
    out, err = junekom.decode(e, raw, v04)
    if out is None:
        return None
    if out[:4] == b"\x1bKL\x82":
        out = ljconv.transcode(out, table)[0]
    return out


lua_re = re.compile(rb"[A-Za-z0-9_\-\.]{3,}\.lua", re.I)


def names_in(b):
    out = set()
    for m in lua_re.finditer(b or b""):
        t = m.group(0).decode("latin-1")
        for i in range(0, len(t) - 4):        # world_census.py's every-suffix rule (C3x)
            c = t[i:]
            if c.lower() in june or c.lower() in staged:
                out.add(c); break
    return out


def where(name):
    s = "kc" if name.lower() in staged else "-"
    j = "june" if name.lower() in june else "-"
    return s + "/" + j


# --- the script set ------------------------------------------------------------------------------------------------------------
scripts = []
for kd in sorted(os.listdir(KC)):
    if not kd.endswith("_kom") or not os.path.isdir(os.path.join(KC, kd)):
        continue
    for f in sorted(os.listdir(os.path.join(KC, kd))):
        fl = f.lower()
        if not (fl.endswith(".lua") and not fl.endswith(".src.lua")):
            continue
        p = os.path.join(KC, kd, f)
        b = open(p, "rb").read()
        if b"STAGE_LIST" not in b or b"SUB_STAGE" not in b:
            continue
        if fl in m_reg or any(i in m_ids for i in j_reg.get(fl, [])):
            continue                            # covered: March registers it, or June does under an id March has (C3z-3)
        scripts.append((kd[:-4], f, b))
print("unreached dungeon scripts:", len(scripts))

par = collections.defaultdict(lambda: {"unres": set(), "mesh_bad": 0, "rows": 0})
par_set = set()
pf = os.path.join(OUT, "dungeon_npc_parity.tsv")
with open(pf, encoding="utf-8") as fh:
    hdr = fh.readline().rstrip("\n").split("\t")
    for ln in fh:
        r = dict(zip(hdr, ln.rstrip("\n").split("\t")))
        if not r.get("exception", "").startswith("C3y"):
            continue
        par_set.add(r["script"].lower())
        a = par[r["script"].lower()]
        a["rows"] += 1
        for tok in filter(None, r.get("unresolved", "").split(";")):
            a["unres"].add(tok.split("(")[0])
        if r.get("mesh_ok") not in ("", "1", None):
            a["mesh_bad"] += 1
mine = {f.lower() for _, f, _ in scripts}
for f in sorted(mine - par_set):
    print("MISMATCH not in parity C3y set:", f)
for f in sorted(par_set - mine):
    print("MISMATCH parity C3y row now covered or gone:", f)


# --- owners ----------------------------------------------------------------------------------------------------------------
def owner(f, reg):
    n = f[:-4]
    if re.match(r"Dungeon_Elysion_(SkyCrossroads|Adrian|Soless|Memory)_", n):
        line = {"SkyCrossroads": "E8", "Adrian": "E9", "Soless": "E10", "Memory": "E11"}[n.split("_")[2]]
        return "elysion " + line
    if re.match(r"Dungeon_Elysion_(AtlasDevelopment|AtlasCity|Tower)_", n):
        return "atlas " + {"AtlasDevelopment": "A8", "AtlasCity": "A9", "Tower": "A10"}[n.split("_")[2]]
    if n in ("Dungeon_Tutorial_Luciel", "Dungeon_Tutorial_Rose", "Dungeon_Tutorial_Ain"):
        return "backlog Stage 8"
    if n.startswith("Dungeon_Tutorial_"):
        return "june-effect C3y-2"
    if n == "Dungeon_Sander_Secret_Hell":
        return "june-effect C3y-1c"            # its 9 worlds are C3y-1b (Henir Space stage 22 names the boss world)
    if n in ("Dungeon_Hamel_Secret_Hell", "Dungeon_Hamel_Secret_Common", "Dungeon_Ranox_Secret_Hell"):
        return "june-effect C3y-1a"            # Ranox: only June's UNUSED:DI_RANOX_SECRET_COMMON row reaches here
    if n.startswith("Dungeon_Bestera"):
        return "june-effect C3y-3"
    if n.startswith("Dungeon_Space_of_Thought"):
        return "june-effect C3y-4"
    if reg == "june":
        return "backlog Stage 19"          # June's event dungeons (and the remaining June-only rows)
    return "none (dead in both)"


rows, worlds_used = [], collections.defaultdict(set)
for kd, f, b in scripts:
    fl = f.lower()
    reg = "june" if fl in j_reg else "none"
    ids = j_reg.get(fl, [])
    idcol = []
    for i in ids:
        jv, mv = j_de.get(i, "?"), m_de.get(i)
        clash = [n for n in m_de_by_val.get(jv, []) if n != i]
        idcol.append("%s=%s%s%s" % (i, jv, "" if mv is None else (" (March %s)" % mv if mv == jv else " (March %s!)" % mv),
                                     " CLASH:" + ",".join(clash) if clash and mv is None else ""))
    ws = sorted({w.decode() for w in re.findall(rb"WI_[A-Z0-9_]+", b)})
    for w in ws:
        worlds_used[w].add(f)
    gaps = []
    for w in ws:
        if w not in m_wi:
            gaps.append("enum")
        elif w not in m_wd:
            gaps.append("wd")
    jb = june_bytes(f)
    origin = ("june-bytecode" if jb is not None and jb == b else "march(differs from June)" if jb is not None else "march-only")
    in_pre = fl in pre035 if kd == "data035" else (fl in live)
    p = par.get(fl, {"unres": set(), "mesh_bad": 0, "rows": 0})
    rows.append((kd, f, reg, ";".join(idcol) or "-", ";".join(ws) or "-",
                 sum(1 for w in ws if w not in m_wi), sum(1 for w in ws if w in m_wi and w not in m_wd),
                 len(p["unres"]), ",".join(sorted(p["unres"]))[:400], origin, "yes" if in_pre else "no", owner(f, reg)))

wrows = []
for w in sorted(worlds_used):
    jf, mf = j_wd.get(w), m_wd.get(w)
    jv, mv = j_wi.get(w, "?"), m_wi.get(w)
    clash = [n for n in m_wi_by_val.get(jv, []) if n != w] if mv is None else []
    f = jf or mf
    lm = sorted(names_in(june_bytes(f) if f else None) | names_in(march_bytes(f) if f else None)) if f else []
    lm = [n for n in lm if not f or n.lower() != f.lower()]
    wrows.append((w, jv, mv or "-", ",".join(clash) or "-", mf or "-", jf or "-", where(f) if f else "-",
                  ";".join("%s[%s]" % (n, where(n)) for n in lm) or "-", ";".join(sorted(worlds_used[w]))[:300]))

with open(os.path.join(OUT, "c3y_unreached_dungeon_census.tsv"), "w", encoding="utf-8", newline="\n") as fh:
    fh.write("kom\tscript\tregistry\tjune_dungeon_ids\tworlds\tworlds_no_enum\tworlds_no_worlddata\tnpc_unresolved_n\t"
             "npc_unresolved\torigin\tin_march_before_june\towner\n")
    for r in rows:
        fh.write("\t".join(str(x) for x in r) + "\n")
with open(os.path.join(OUT, "c3y_unreached_world_census.tsv"), "w", encoding="utf-8", newline="\n") as fh:
    fh.write("world\tjune_value\tmarch_value\tmarch_clash_at_june_value\tmarch_worlddata\tjune_worlddata\tworld_script(kc/june)\t"
             "line_maps(kc/june)\tused_by\n")
    for r in wrows:
        fh.write("\t".join(str(x) for x in r) + "\n")

# --- every June DungeonData id March does not register (C3z-3 routed 251 to C3y) ---------------------------------------------
mine_by = {f.lower(): (kd, f) for kd, f, _ in scripts}
irows = []
for d in j_dd:
    i = did(d)
    if i in m_ids:
        continue
    fn = d.get("dataFileName", "")
    st = "unreached-script" if fn.lower() in mine_by else "staged-covered" if fn.lower() in staged else "no-script"
    own = owner(mine_by[fn.lower()][1], "june") if fn.lower() in mine_by else owner(fn, "june") if fn else "?"
    if re.search(r"_HERO_|HERO_DUMMY", i):
        own = "june-effect C3y-5"              # Hero tier: June's real entry route is unidentified (JUNE_UI_PARITY Phase 4 findings)
    jv, mv = j_de.get(i, "?"), m_de.get(i)
    clash = [n for n in m_de_by_val.get(jv, []) if n != i] if mv is None else []
    irows.append((i, jv, mv or "-", ",".join(clash) or "-", fn, st, where(fn) if fn else "-", own))
with open(os.path.join(OUT, "c3y_june_only_dungeon_ids.tsv"), "w", encoding="utf-8", newline="\n") as fh:
    fh.write("dungeon_id\tjune_value\tmarch_value\tmarch_clash_at_june_value\tdata_file\tscript_state\tscript(kc/june)\towner\n")
    for r in sorted(irows, key=lambda r: int(r[1]) if r[1].isdigit() else 0):
        fh.write("\t".join(str(x) for x in r) + "\n")
print("June-only DungeonData ids:", len(irows), collections.Counter(r[5] for r in irows))
print("  by owner:", sorted(collections.Counter(r[7] for r in irows).items()))
print("by registry:", collections.Counter(r[2] for r in rows))
print("by owner:", sorted(collections.Counter(r[11] for r in rows).items()))
print("by origin:", collections.Counter(r[9] for r in rows))
print("worlds named:", len(wrows), "no March enum:", sum(1 for r in wrows if r[2] == "-"),
      "no March WorldData:", sum(1 for r in wrows if r[4] == "-"))
