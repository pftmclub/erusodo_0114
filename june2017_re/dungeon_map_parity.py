r"""Per-dungeon map parity with June (June effect C3z-1, 2026-10-10; user: "make sure to check all dungeon").
For every dungeon script in kom_contents: the worlds it names (WorldData.lua rows), each world's script and line map, and every asset
those name, closed over the meshes (MSZIP .Y / .X inflated with fxdata.xfile_text; June's bytes walked where June has the file, since
June wins at the same filename). Each name is compared byte-for-byte: March = kom_contents dir (or the game dir's loose music\),
June = junekom's strict decode (or June's loose music\). World scripts / line maps compare after ljconv, as world_census.py does.
    python june2017_re/dungeon_map_parity.py            (from the Trunk; read-only)
Writes (june2017_re/):
  dungeon_map_parity.tsv   one row per dungeon script: worlds, world-file / asset counts, how many differ or are missing, causes
  dungeon_map_asset.tsv    one row per asset name: state, March kom + size, June kom + size, first user
  dungeon_map_replace.tsv  `name` list for `kom.py stage-june`: kom assets whose June bytes differ or that March lacks
States: same / DIFF / june-only (March lacks it) / march-only (June lacks it) / june-undecodable (strict decode refuses) /
june-disagree (June's copies differ) / march-live-only (in a live kom, not in its kom_contents dir) / march-multi (several March koms).
"""
import os, re, sys, collections, hashlib
TRUNK = os.getcwd()
sys.path.insert(0, os.path.join(TRUNK, "tools"))
sys.path.insert(0, os.path.join(TRUNK, "june2017_re"))
import kom, junekom, ljconv, fxdata

KC = kom.KOM_ROOT
OUT = os.path.join(TRUNK, "june2017_re")
MMUS = os.path.join(kom.GAME_DIR, "music")
JMUS = os.path.join(junekom.JUNE_DATA, "music")
table = ljconv.load_table(ljconv.TABLE_TSV)

def rows(t):
    return {m.group(1): m.group(2) for m in re.finditer(r'worldName\s*=\s*"([^"]+)"\s*,\s*dataFileName\s*=\s*"([^"]+)"', t)}
_wd = os.path.join(KC, "data040_kom", "WorldData.lua")
mwd = rows(open(_wd, encoding="utf-8-sig", errors="replace").read())
jwd = rows(open(os.path.join(TRUNK, "june_src_fixed", "data040_kom", "WorldData.src.lua"), encoding="utf-8", errors="replace").read())

staged = kom.staged_names()
live = kom.live_names()
june = collections.defaultdict(list)
for path in junekom.all_koms()[0]:
    k = junekom.Kom(path)
    for e in k.entries:
        june[e["name"].lower()].append((path, k.stem.lower(), k.v04, e))
mmus = {f.lower(): f for f in os.listdir(MMUS)} if os.path.isdir(MMUS) else {}
jmus = {f.lower(): f for f in os.listdir(JMUS)} if os.path.isdir(JMUS) else {}
known = set(june) | set(staged) | set(live) | set(mmus) | set(jmus)
print("names: june %d, march staged %d, march live %d, music march %d / june %d" % (len(june), len(staged), len(live), len(mmus), len(jmus)))

_jc = {}
def june_info(ln):
    """-> (bytes or None, state '' / 'june-undecodable' / 'june-disagree' / 'absent', kom stem)"""
    if ln in _jc:
        return _jc[ln]
    if ln not in june:
        r = (open(os.path.join(JMUS, jmus[ln]), "rb").read(), "", "music") if ln in jmus else (None, "absent", "")
        _jc[ln] = r; return r
    outs = []
    for path, stem, v04, e in june[ln]:
        with open(path, "rb") as fh:
            fh.seek(e["offset"]); raw = fh.read(e["csize"])
        out, err = junekom.decode(e, raw, v04)
        if out is None:
            _jc[ln] = (None, "june-undecodable", stem); return _jc[ln]
        if out[:4] == b"\x1bKL\x82":
            out = ljconv.transcode(out, table)[0]
        outs.append(out)
    r = (outs[0], "" if len({hashlib.sha1(o).digest() for o in outs}) == 1 else "june-disagree", june[ln][0][1])
    _jc[ln] = r; return r

def march_info(ln):
    """-> (bytes or None, state '' / 'march-live-only' / 'march-multi' / 'absent', kom stem)"""
    where = dict(live.get(ln, {}))
    for s, sp in staged.get(ln, {}).items():
        where.setdefault(s, sp)
    if not where:
        if ln in mmus:
            return open(os.path.join(MMUS, mmus[ln]), "rb").read(), "", "music"
        return None, "absent", ""
    st = "march-multi" if len(where) > 1 else ""
    for s in sorted(where):
        if s in staged.get(ln, {}):
            return open(os.path.join(KC, s + "_kom", staged[ln][s]), "rb").read(), st, s
    return None, st or "march-live-only", sorted(where)[0]

def state_of(ln):
    mb, ms, mk = march_info(ln)
    jb, js, jk = june_info(ln)
    if ms in ("march-multi", "march-live-only"): s = ms
    elif js in ("june-undecodable", "june-disagree"): s = js
    elif mb is None and jb is None: s = "none"
    elif mb is None: s = "june-only"
    elif jb is None: s = "march-only"
    elif mb == jb: s = "same"
    else: s = "DIFF"
    return s, mk, len(mb or b""), jk, len(jb or b""), mb, jb

# Tokens end at an extension; a run never reaches back past the previous extension. Bytecode constants sit back to back, so one greedy
# regex match used to swallow `A.Y-B.Y-C.Y` whole and resolve only C (C3z-1; C3x's census had the same blind spot).
lua_re = re.compile(rb"\.lua(?![A-Za-z0-9_])", re.I)
asset_re = re.compile(rb"\.(?:x|y|dds|tga|png|bmp|jpg|ogg|wav|mp3)(?![A-Za-z0-9_])", re.I)
run_re = re.compile(rb"[A-Za-z0-9_\-\. ]+$")
unresolved = collections.Counter()
def resolve(t, mesh):
    """A trim is valid only where the byte before the name can be its length: in LuaJIT bytecode a string constant is preceded by
    uleb128(len + 5) (BCDUMP_KGC_STR), e.g. `-` = 45 before a 40-character name; in a binary .X a DWORD length (low byte = len).
    Longest valid name first. An unchecked suffix fallback made `Peita_Heart_Floor.tga` (in neither build) June's `floor.tga`."""
    t = t.decode("latin-1")
    s = t.lstrip()
    if s.lower() in known:
        return s.lower()
    for i in range(1, len(t) - 4):
        c = t[i:].strip()
        if c.lower() in known and ord(t[i - 1]) in ((len(c) + 5) if not mesh else -1, len(c)):
            return c.lower()
    unresolved[s] += 1
    return None
def names_in(b, rx, mesh=False):
    out, prev = set(), 0
    for m in rx.finditer(b or b""):
        e = m.end()
        r = run_re.search(b, max(prev, e - 200), e)
        prev = e
        if r and r.end() == e and e - r.start() > 4:
            n = resolve(b[r.start():e], mesh)
            if n:
                out.add(n)
    return out

MODEL = (".x", ".y")
def mesh_text(b):
    if b and b[:4] == b"xof " and b[8:12] in (b"bzip", b"tzip", b"txt ", b"bin "):
        return fxdata.xfile_text(b, lenient=True) or b
    return b

# dungeon scripts -> worlds
use = collections.defaultdict(set)          # world -> {dungeon}
dworlds = {}
for kd in sorted(os.listdir(KC)):
    if not kd.endswith("_kom") or not os.path.isdir(os.path.join(KC, kd)):
        continue
    for f in os.listdir(os.path.join(KC, kd)):
        fl = f.lower()
        if fl.startswith("dungeon_") and fl.endswith(".lua") and not fl.endswith(".src.lua"):
            ws = {w.decode() for w in re.findall(rb"WI_[A-Z0-9_]+", open(os.path.join(KC, kd, f), "rb").read())} & set(mwd)
            dworlds[(kd[:-4], f)] = ws
            for w in ws:
                use[w].add(f)

# world -> files (script + line maps) -> direct assets
wfiles, wassets = {}, {}
asset_user = {}
for w in sorted(use):
    mf, jf = mwd[w].lower(), jwd.get(w, mwd[w]).lower()
    fs = [mf] if mf == jf else [mf, jf]
    for x in list(fs):
        _, _, _, _, _, mb, jb = state_of(x)
        for n in sorted(names_in(mb, lua_re) | names_in(jb, lua_re)):
            if n not in fs:
                fs.append(n)
    wfiles[w] = fs
    a = set()
    for x in fs:
        _, _, _, _, _, mb, jb = state_of(x)
        for n in names_in(mb, asset_re) | names_in(jb, asset_re):
            if not n.endswith(".lua"):
                a.add(n); asset_user.setdefault(n, x)
    wassets[w] = a

# closure over meshes: walk June's bytes where June has the file (June wins), else March's
closure = {}
def close(n):
    if n in closure:
        return closure[n]
    closure[n] = set()
    out = set()
    if n.endswith(MODEL):
        s, mk, ms_, jk, js_, mb, jb = state_of(n)
        b = mesh_text(jb if jb is not None else mb)
        for c in names_in(b, asset_re, mesh=True):
            if c != n and not c.endswith(".lua"):
                out.add(c); asset_user.setdefault(c, "mesh:" + n)
                out |= close(c)
    closure[n] = out
    return out
wall = {}
for w, a in wassets.items():
    s = set(a)
    for n in a:
        s |= close(n)
    wall[w] = s

allassets = set().union(*wall.values()) if wall else set()
ast = {n: state_of(n)[:5] for n in sorted(allassets)}
fst = {}
for w, fs in wfiles.items():
    for x in fs:
        fst[x] = state_of(x)[:5]
print("worlds %d, world files %d, assets %d (direct %d)" % (len(wfiles), len(fst), len(ast), len(set().union(*wassets.values()))))
print("world files:", dict(collections.Counter(v[0] for v in fst.values())))
print("assets:", dict(collections.Counter(v[0] for v in ast.values())))

BAD = ("DIFF", "june-only")
CAUSE = ("june-undecodable", "june-disagree", "march-live-only", "march-multi")
with open(os.path.join(OUT, "dungeon_map_parity.tsv"), "w", encoding="utf-8", newline="\n") as fh:
    fh.write("kom\tdungeon\tworlds\tworld_files\twf_differ\twf_missing\tassets\tas_differ\tas_missing\tcauses\tworld_list\n")
    for (kd, f), ws in sorted(dworlds.items(), key=lambda t: t[0][1].lower()):
        fs = {x for w in ws for x in wfiles[w]}
        a = {x for w in ws for x in wall[w]}
        causes = collections.Counter()
        for x in fs:
            if fst[x][0] in CAUSE or fst[x][0] == "march-only": causes["wf " + fst[x][0]] += 1
        for x in a:
            if ast[x][1] == "data101" or (not ast[x][1] and ast[x][3] == "data101"):
                causes["data101-dir"] += 1          # its kom_contents dir lacks 1,071 live entries: never packed (june-asset-port P0)
            elif ast[x][0] in CAUSE: causes[ast[x][0]] += 1
            elif ast[x][0] == "march-only": causes["june-lacks"] += 1    # June's own world script names it, June ships none
        fh.write("%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%s\n" % (kd, f, len(ws), len(fs),
            sum(fst[x][0] == "DIFF" for x in fs), sum(fst[x][0] == "june-only" for x in fs),
            len(a), sum(ast[x][0] == "DIFF" for x in a), sum(ast[x][0] == "june-only" for x in a),
            ";".join("%s=%d" % kv for kv in sorted(causes.items())), ";".join(sorted(ws))))
with open(os.path.join(OUT, "dungeon_map_asset.tsv"), "w", encoding="utf-8", newline="\n") as fh:
    fh.write("name\tstate\tmarch_kom\tmarch_size\tjune_kom\tjune_size\tfirst_user\n")
    for n, (s, mk, ms_, jk, js_) in sorted(ast.items()):
        fh.write("%s\t%s\t%s\t%d\t%s\t%d\t%s\n" % (n, s, mk, ms_, jk, js_, asset_user.get(n, "")))
rep = [n for n, v in ast.items() if v[0] in BAD and v[3] != "music" and "data101" not in (v[1], v[3])]
d101 = [n for n, v in ast.items() if v[0] in BAD and "data101" in (v[1], v[3])]
with open(os.path.join(OUT, "c3z1_data101_pending.tsv"), "w", encoding="utf-8", newline="\n") as fh:
    fh.write("name\tstate\tmarch_kom\tjune_kom\n")
    for n in d101:
        fh.write("%s\t%s\t%s\t%s\n" % (n, ast[n][0], ast[n][1], ast[n][3]))
print("data101 (pending its dir restore, june-asset-port P0): %d" % len(d101))
mus = [n for n, v in ast.items() if v[0] in BAD and v[3] == "music"]
with open(os.path.join(OUT, "dungeon_map_replace.tsv"), "w", encoding="utf-8", newline="\n") as fh:
    fh.write("name\tstate\tmarch_kom\tjune_kom\n")
    for n in rep:
        fh.write("%s\t%s\t%s\t%s\n" % (n, ast[n][0], ast[n][1], ast[n][3]))
print("to stage (kom): %d  (%s)" % (len(rep), dict(collections.Counter(ast[n][0] for n in rep))))
print("loose music differing / missing:", mus)
print("causes:", {s: [n for n, v in ast.items() if v[0] == s][:12] for s in CAUSE if any(v[0] == s for v in ast.values())})
print("world files not same:", {x: v[0] for x, v in fst.items() if v[0] not in ("same", "none")})
print("dungeon rows: %d, rows not 0/0: %d" % (len(dworlds), sum(1 for ws in dworlds.values()
      if any(ast[x][0] in BAD for w in ws for x in wall[w]) or any(fst[x][0] in BAD for w in ws for x in wfiles[w]))))

print("unresolved asset tokens (named, in neither build): %d distinct" % len(unresolved), unresolved.most_common(15))
