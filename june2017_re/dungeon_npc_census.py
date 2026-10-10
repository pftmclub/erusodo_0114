"""June effect C3z-2 (2026-10-10): every NPC the dungeon scripts in kom_contents name, against March's Enum.lua NPC_UNIT_ID,
NPCTemplet.lua, the NPC stat tables and the unit script / mesh files; plus the per dungeon x stage x sub-stage parity gate.

How it reads the scripts:
  - Each dungeon script (any non-sidecar .lua in kom_contents with STAGE_LIST and SUB_STAGE) is RUN by the KOG luajit
    (elstools/vendor/luajit reads 1B 4B 4C 81 bytecode as-is) through june2017_re/dungeon_npc_dump.lua. That walks the tables the
    way the client does: X2Dungeon.cpp STAGE_LIST/STAGE<n>, X2DungeonStage.cpp SUB_STAGE<n>, X2DungeonSubStage.cpp
    New_LoadNPCData (NPC_GROUP_RATE + NPC_GROUP<N>, else NPC_GROUP; entries [1..], SUB_NPC<i>), and the CLEAR_COND<c>.NPC_ID lists.
  - The registries (Enum, NPCTemplet, NPCStat, NewNPCStat, DungeonData) run through june2017_re/lua_registry_dump.lua.
  - A NUI_ token scan of each script's bytes adds the names that only function bodies read.
  - The summon closure scans each covered NPC's unit script the same way, transitively.

Covered dungeon scripts:
  - Every script March's DungeonData registers.
  - Plus every script June registers under a dungeon id March also has, which C3z-3 makes live.
  - Anything else is C3y's: registry-less, or a June-only dungeon.
The NPCTemplet "first row for an id wins" rule follows CX2UnitManager::AddNPCTemplet_LUA. A row whose CLASS_TYPE does not resolve
in March's NPC_CLASS_TYPE, or that has no IS_NPC, fails there (LUA_GET_VALUE_RETURN_ENUM / _RETURN), so both are checked. The mesh
is the unit script's INIT_MOTION (the templet MOTION is read only under X2TOOL), so every .x the unit script names must be staged.

Writes
  june2017_re/dungeon_npc_census.tsv   one row per NUI_ name a dungeon script or covered unit script names
  june2017_re/dungeon_npc_parity.tsv   one row per dungeon script x stage x sub-stage, verdict PASS / EXC (named) / FAIL
  june2017_re/dungeon_npc_add.tsv      June-only ids the covered scripts need that March's Enum lacks (empty once C3z-2 is in)
    python june2017_re/dungeon_npc_census.py            (from the Trunk)
"""
import os, re, sys, subprocess, collections
from concurrent.futures import ThreadPoolExecutor
TRUNK = os.getcwd()
sys.path.insert(0, os.path.join(TRUNK, "tools"))
sys.path.insert(0, os.path.join(TRUNK, "june2017_re"))
import kom, junekom

KC = kom.KOM_ROOT
LJ = os.path.join(TRUNK, "elstools", "vendor", "luajit", "luajit.exe")
DUMP = os.path.join(TRUNK, "june2017_re", "dungeon_npc_dump.lua")
REG = os.path.join(TRUNK, "june2017_re", "lua_registry_dump.lua")
JS = os.path.join(TRUNK, "june_src_fixed")
OUT = os.path.join(TRUNK, "june2017_re")


def run(script, *args):
    r = subprocess.run([LJ, script] + list(args), capture_output=True, stdin=subprocess.DEVNULL, timeout=600)
    return r.stdout.decode("utf-8", "replace").splitlines()


def enum(path, table="NPC_UNIT_ID"):
    out = {}
    for ln in run(REG, "enum", path, table):
        p = ln.split("\t")
        if p[0] == "K":
            out[p[1]] = p[2]
        elif p[0] == "E":
            print("ERROR", path, ln)
    return out


def calls(path, method):
    rows = []
    for ln in run(REG, "calls", path):
        p = ln.split("\t")
        if p[0] == "E":
            print("ERROR", path, ln)
        if p[0] != "C" or p[1] != method:
            continue
        d = {}
        for kv in p[2:]:
            k, _, v = kv.partition("=")
            d[k] = v
        rows.append(d)
    return rows


def kc(kom_dir, name):
    return os.path.join(KC, kom_dir + "_kom", name)


def nui(v):
    return v[4:] if v and v.startswith("NUI:") else None


# --- registries --------------------------------------------------------------------------------------------------------------
M_ENUM_FILE = kc("data036", "Enum.lua")
J_ENUM_FILE = os.path.join(JS, "data036_kom", "Enum.src.lua")
m_enum = enum(M_ENUM_FILE)
j_enum = enum(J_ENUM_FILE)
m_nct = set(enum(M_ENUM_FILE, "NPC_CLASS_TYPE"))
m_val2names = collections.defaultdict(list)
for k, v in m_enum.items():
    m_val2names[v].append(k)


def templets(path):
    out = {}
    for d in calls(path, "AddNPCTemplet"):
        n = nui(d.get("NPC_ID"))
        if n and n not in out:            # CX2UnitManager::AddNPCTemplet_LUA: the first row for an id wins
            out[n] = d
    return out


m_tpl = templets(kc("data037", "NPCTemplet.lua"))
j_tpl = templets(os.path.join(JS, "data037_kom", "NPCTemplet.src.lua"))


def stat_names(path, method):
    return {nui(d.get("NPC_ID")) for d in calls(path, method)} - {None}


m_stat = stat_names(kc("data037", "NPCStat.lua"), "AddNPCStat") | stat_names(kc("datai016", "NPCStat_INT.lua"), "AddNPCStat")
m_extra = stat_names(kc("data036", "NewNPCStat.lua"), "AddExtraNPCStat")


def did(d):
    return d.get("dungeonID", "?").replace("E:DUNGEON_ID.", "").replace("E:UNUSED_DUNGEON_ID.", "UNUSED:")


m_dd = calls(kc("data035", "DungeonData.lua"), "AddDungeonData")
j_dd = calls(os.path.join(JS, "data035_kom", "DungeonData.src.lua"), "AddDungeonData")
m_ids = {did(d) for d in m_dd}
m_reg, j_reg, j_shared = collections.defaultdict(list), collections.defaultdict(list), set()
for d in m_dd:
    m_reg[d.get("dataFileName", "").lower()].append(did(d))
for d in j_dd:
    f = d.get("dataFileName", "").lower()
    j_reg[f].append(did(d))
    if did(d) in m_ids:                   # C3z-3 takes June's row for a dungeon id March also has
        j_shared.add(f)
print("March enum %d, June enum %d; NCT %d; templets March %d June %d; stat %d extra %d; DungeonData March %d June %d" % (
    len(m_enum), len(j_enum), len(m_nct), len(m_tpl), len(j_tpl), len(m_stat), len(m_extra), len(m_dd), len(j_dd)))

staged = kom.staged_names()     # lower -> {stem: spelling}


def have(name):
    return bool(name) and name.lower() in staged


def staged_bytes(name):
    stem, sp = sorted(staged[name.lower()].items())[0]
    with open(os.path.join(KC, stem + "_kom", sp), "rb") as fh:
        return fh.read()


june_names = set()
for _p in junekom.all_koms()[0]:
    for _e in junekom.Kom(_p).entries:
        june_names.add(_e["name"].lower())
X_RE = re.compile(rb"[A-Za-z0-9_\-\. ]{3,}\.(?:x|X)(?![A-Za-z0-9_])")
NUI_RE = re.compile(rb"NUI_[A-Za-z0-9_]+")
_mesh_cache, _nui_cache = {}, {}


def missing_meshes(lua):
    """.x names the staged unit script uses that kom_contents lacks. A bytecode string constant is preceded by its uleb length
    byte, so each token is resolved by trying suffixes (npc_mesh_census.py); only names March or June has count."""
    if not have(lua):
        return None
    ln = lua.lower()
    if ln not in _mesh_cache:
        miss = set()
        for m in X_RE.finditer(staged_bytes(lua)):
            t = m.group(0).decode("latin-1")
            for i in range(0, min(4, len(t) - 2)):
                c = t[i:].strip().lower()
                if c in staged:
                    break
                if c in june_names:
                    miss.add(t[i:].strip()); break
        _mesh_cache[ln] = sorted(miss)
    return _mesh_cache[ln]


def nui_tokens(b):
    """NUI_ names in bytecode / source; a constant can glue onto the next one, so the longest known prefix wins."""
    out = set()
    for m in set(NUI_RE.findall(b)):
        s = m.decode("latin-1")
        for i in range(len(s), 4, -1):
            if s[:i] in m_enum or s[:i] in j_enum:
                out.add(s[:i]); break
    return out


def lua_nuis(lua):
    if not have(lua):
        return set()
    ln = lua.lower()
    if ln not in _nui_cache:
        _nui_cache[ln] = nui_tokens(staged_bytes(lua))
    return _nui_cache[ln]


def templet_of(n):
    t = m_tpl.get(n)
    if t is None and n in m_enum:
        for alt in m_val2names.get(m_enum[n], []):     # the map is keyed by value
            if alt in m_tpl:
                return m_tpl[alt]
    return t


def resolve(n):
    """What the client gets for NPC_UNIT_ID.<n> and what that id needs -> (ok, why)."""
    if n not in m_enum:
        return False, "enum"
    t = templet_of(n)
    if t is None:
        return False, "templet"
    ct = t.get("CLASS_TYPE", "")
    if not ct.startswith("E:NPC_CLASS_TYPE.") or ct.split(".", 1)[1] not in m_nct or "IS_NPC" not in t:
        return False, "templet-class"
    if not have(t.get("LUA_FILE_NAME")):
        return False, "unit-lua"
    if missing_meshes(t.get("LUA_FILE_NAME")):
        return False, "mesh"
    return True, ""


def resolve_june(n):
    return n in j_enum and n in j_tpl


# --- dungeon scripts ---------------------------------------------------------------------------------------------------------
scripts = []
for kd in sorted(os.listdir(KC)):
    if not kd.endswith("_kom") or not os.path.isdir(os.path.join(KC, kd)):
        continue
    for f in os.listdir(os.path.join(KC, kd)):
        if not f.lower().endswith(".lua") or f.lower().endswith(".src.lua"):
            continue
        p = os.path.join(KC, kd, f)
        with open(p, "rb") as fh:
            b = fh.read()
        if b"STAGE_LIST" in b and b"SUB_STAGE" in b:
            scripts.append((kd[:-4], f, p, b))
print("dungeon scripts:", len(scripts))


def dump(t):
    return t, run(DUMP, t[2])


entries = collections.defaultdict(list)    # (kom, file, stage, sub) -> [(form, group, idx, subnpc, id)]
clears = collections.defaultdict(list)     # (kom, file, stage, sub) -> [(cond, idx, id)]
subs = collections.OrderedDict()
reads = collections.defaultdict(set)       # NUI name -> {file}
errors = {}
with ThreadPoolExecutor(12) as ex:
    for (kd, f, p, b), lines in ex.map(dump, scripts):
        for n in nui_tokens(b):
            reads[n].add(f)
        for ln in lines:
            q = ln.split("\t")
            if q[0] == "N":
                entries[(kd, f, int(q[1]), int(q[2]))].append(tuple(q[3:]))
            elif q[0] == "K":
                clears[(kd, f, int(q[1]), int(q[2]))].append(tuple(q[3:]))
            elif q[0] == "B":
                subs[(kd, f, int(q[1]), int(q[2]))] = True
            elif q[0] == "R":
                reads[q[1]].add(f)
            elif q[0] == "E":
                errors[(kd, f)] = q[1]
for k, e in errors.items():
    print("SCRIPT ERROR", k, e)


def registry(f):
    fl = f.lower()
    m, j = fl in m_reg, fl in j_reg
    return "both" if m and j else "march" if m else ("june-shared" if fl in j_shared else "june") if j else "none"


def covered(f):
    return registry(f) in ("both", "march", "june-shared")


# --- what the covered scripts need: names they read / spawn / clear on, plus the unit-script summon closure -------------------
need_src = collections.defaultdict(set)
for n, fs in reads.items():
    for f in fs:
        if covered(f):
            need_src[n].add("dungeon")
todo = list(need_src)
while todo:
    n = todo.pop()
    t = templet_of(n) or j_tpl.get(n)
    if not t:
        continue
    for c in lua_nuis(t.get("LUA_FILE_NAME")):
        if c not in need_src:
            todo.append(c)
        if "dungeon" not in need_src[c] and c != n:
            need_src[c].add("summon:" + n)
to_add = sorted((n for n in need_src if n not in m_enum and n in j_enum), key=lambda n: int(j_enum[n]))
neither = sorted(n for n in set(reads) | set(need_src) if n not in m_enum and n not in j_enum)
with open(os.path.join(OUT, "dungeon_npc_add.tsv"), "w", encoding="utf-8", newline="\n") as fh:
    fh.write("name\tjune_id\tjune_templet\tlua_file\tlua_in_kom\tmeshes_missing\tstat\tneeded_by\n")
    for n in to_add:
        t = j_tpl.get(n, {})
        fh.write("\t".join([n, j_enum[n], "y" if t else "", t.get("LUA_FILE_NAME", ""), "y" if have(t.get("LUA_FILE_NAME")) else "",
                            ";".join(missing_meshes(t.get("LUA_FILE_NAME")) or []),
                            "npcstat" if n in m_stat else "extra" if n in m_extra else "none",
                            ";".join(sorted(need_src[n]))[:300]]) + "\n")
print("June-only ids the covered scripts need and March's Enum lacks:", len(to_add))

# --- census per NUI name -----------------------------------------------------------------------------------------------------
spawned = collections.defaultdict(set)
for (kd, f, s, b), es in entries.items():
    for e in es:
        n = nui(e[4])
        if n:
            spawned[n].add(f)
names = sorted(set(reads) | set(spawned) | set(need_src))
with open(os.path.join(OUT, "dungeon_npc_census.tsv"), "w", encoding="utf-8", newline="\n") as fh:
    fh.write("name\tmarch_id\tjune_id\tid_state\tmarch_templet\tjune_templet\ttemplet_diff\tlua_file\tlua_in_kom\tmeshes_missing\t"
             "stat\tresolves\tneeded\tspawned_in\tread_by\n")
    for n in names:
        mv, jv = m_enum.get(n), j_enum.get(n)
        st = ("same" if mv == jv else "DIFF") if mv and jv else "march-only" if mv else "june-only" if jv else "neither"
        mt, jt = m_tpl.get(n), j_tpl.get(n)
        t = templet_of(n) or jt or {}
        diff = ""
        if mt and jt:
            diff = ",".join(k for k in ("LUA_FILE_NAME", "MOTION", "CLASS_TYPE") if mt.get(k, "") != jt.get(k, ""))
        stat = "npcstat" if n in m_stat else "extra" if n in m_extra else "none"
        ok, why = resolve(n)
        fh.write("\t".join([n, mv or "", jv or "", st, "y" if mt else "", "y" if jt else "", diff, t.get("LUA_FILE_NAME", ""),
                            "y" if have(t.get("LUA_FILE_NAME")) else "", ";".join(missing_meshes(t.get("LUA_FILE_NAME")) or []),
                            stat, "ok" if ok else why, ";".join(sorted(need_src.get(n, ())))[:200], str(len(spawned.get(n, ()))),
                            ";".join(sorted(reads.get(n, ())))[:400]]) + "\n")

# --- parity gate per dungeon x stage x sub-stage ------------------------------------------------------------------------------
GATEISH = re.compile(r"(^NUI_BOX|GATE|^NUI_SMALLGATE|_DOOR|TRAP|^NUI_INVISIBLE|DUMMY|CHECKER|_TRIGGER)")
verdicts = collections.Counter()
fails = []
with open(os.path.join(OUT, "dungeon_npc_parity.tsv"), "w", encoding="utf-8", newline="\n") as fh:
    fh.write("kom\tscript\tregistry\tmarch_dungeon\tjune_dungeon\tstage\tsub\tform\tentries\tid_ok\ttemplet_ok\tlua_ok\tmesh_ok\t"
             "clear_ids\tclear_ok\tunresolved\tonly_gate_box\tjune_only_gate_box\tverdict\texception\n")
    for key in subs:
        kd, f, s, b = key
        es = entries.get(key, [])
        n_id = n_tpl = n_lua = n_mot = 0
        bad = collections.Counter()
        live, named = [], []
        for e in es:
            n = nui(e[4])
            if n is None:
                if e[4] != "nil":
                    bad["<%s>" % e[4]] += 1      # a literal number or something odd; flagged for a look
                continue
            named.append(n)
            ok, why = resolve(n)
            n_id += why != "enum"
            n_tpl += why not in ("enum", "templet", "templet-class")
            n_lua += why not in ("enum", "templet", "templet-class", "unit-lua")
            n_mot += ok
            if ok:
                live.append(n)
            else:
                bad["%s(%s)" % (n, why)] += 1
        cl = clears.get(key, [])
        cl_bad = []
        for c, i, v in cl:
            n = nui(v)
            if n is None or not resolve(n)[0]:
                cl_bad.append("CLEAR_COND%s:%s" % (c, n or v))
                bad["%s(clear)" % (n or v)] += 1
        # the Druid Forest symptom: the script names monsters, but what resolves is only boxes / gates / devices (or nothing)
        only_gate = any(not GATEISH.search(n) for n in named) and not any(not GATEISH.search(n) for n in live)
        j_live = [n for n in named if resolve_june(n)]
        j_only_gate = any(not GATEISH.search(n) for n in named) and not any(not GATEISH.search(n) for n in j_live)
        reg = registry(f)
        exc = []
        if not covered(f):
            exc.append("C3y: " + ("registry-less script" if reg == "none" else "June-only dungeon"))
        else:
            bad_names = {k.rsplit("(", 1)[0] for k in bad}
            if bad_names and all(n in neither for n in bad_names):
                exc.append("June bug: id in neither Enum (JUNE_BUGS.md)")
            if only_gate and j_only_gate and not bad:
                exc.append("by design: box/gate only in June too")
        problem = bool(bad) or only_gate
        verdict = "PASS" if not problem else "EXC" if exc else "FAIL"
        verdicts[verdict] += 1
        if verdict == "FAIL":
            fails.append((f, s, b, sorted(bad), only_gate))
        fh.write("\t".join([kd, f, reg, ";".join(m_reg.get(f.lower(), [])) or "-", ";".join(j_reg.get(f.lower(), [])) or "-",
                            str(s), str(b), es[0][0] if es else "-", str(len(es)), str(n_id), str(n_tpl), str(n_lua), str(n_mot),
                            str(len(cl)), str(len(cl) - len(cl_bad)), ";".join("%s x%d" % kv for kv in sorted(bad.items())),
                            "YES" if only_gate else "", "YES" if j_only_gate else "", verdict, "; ".join(exc)]) + "\n")
print("sub-stages:", len(subs), dict(verdicts))
print("covered scripts:", len({f for _, f, _, _ in subs if covered(f)}), "of", len({f for _, f, _, _ in subs}))
print("names in neither Enum read by covered scripts:", [n for n in neither if any(covered(f) for f in reads.get(n, ()))])
for x in fails[:60]:
    print("FAIL", x)
