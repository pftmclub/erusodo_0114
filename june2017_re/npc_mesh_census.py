"""Census: assets named by the June .lua that REPLACED a March script (stage-june backups 20261009_135231 and _141115) that June
has and March lacks, plus the textures / models those meshes name (closure). A replaced script is one March's registries can reach,
so a missing mesh there crashes CX2GUNPC::Init (m_pMotion NULL) when the monster spawns (2026-10-09, Ruben field 40000, Toto).
Writes june2017_re/npc_mesh_need_replaced.tsv (name, June kom, size, needed_by); feed it to `tools/kom.py stage-june`.
Bytecode string constants carry a uleb length byte that can be printable, so each token is resolved by trying short suffixes.
    python june2017_re/npc_mesh_census.py
# --seed LIST (2026-10-12, NPC anim collision): LIST (one mesh name per line) names June meshes that COLLIDE with a March mesh of the same name
# whose animation set lacks the ANIM_NAMEs a June script uses (e.g. Motion_THIEF_THIN.X: March has `AttackA`, June `AttackA_Ready` / `_Fire`, so
# the Elder bridge guard logged "NPC Motion Error"). Seeds those names (June's copy wins, june-asset rule) plus the closure of textures /
# models they name that March lacks. Writes npc_anim_collision_need.tsv.
"""
import os, re, sys, json, collections
TRUNK = r"F:\elsword stuff\elsword_2014\els_2014\ElswordFiles\ready_and_built\ProjectX2_SVN\ProjectX2_SVN\source\EU_CN_US\Trunk"
sys.path.insert(0, os.path.join(TRUNK, "tools"))
sys.path.insert(0, os.path.join(TRUNK, "june2017_re"))
import kom, junekom

OUT = os.path.join(TRUNK, "june2017_re")
KC = kom.KOM_ROOT

march = set(kom.live_names().keys()) | set(kom.staged_names().keys())
print("march names:", len(march))

# June index: lower name -> [(path, stem, v04, entry)]
june = collections.defaultdict(list)
for path in junekom.all_koms()[0]:
    k = junekom.Kom(path)
    stem = os.path.basename(path)[:-4].lower()
    for e in k.entries:
        june[e["name"].lower()].append((path, stem, k.v04, e))
print("june names:", len(june))

MODEL = (".x", ".y")
TEX = (".dds", ".tga", ".png", ".bmp", ".jpg")
EXTS = MODEL + TEX
tok_re = re.compile(rb"[A-Za-z0-9_\-\. ]{3,}\.(?:x|X|y|Y|dds|DDS|Dds|tga|TGA|png|PNG|bmp|BMP|jpg|JPG)(?![A-Za-z0-9_])")
# --worlds LIST (June effect C3x, 2026-10-10): the world scripts / line maps named in LIST (one name per line) instead of unit
# scripts; also takes sounds (world scripts name ambient .ogg / .wav). Writes world_asset_need.tsv.
WORLDS = sys.argv[sys.argv.index("--worlds") + 1] if "--worlds" in sys.argv else None
if WORLDS:
    tok_re = re.compile(rb"[A-Za-z0-9_\-\. ]{3,}\.(?:x|X|y|Y|dds|DDS|Dds|tga|TGA|png|PNG|bmp|BMP|jpg|JPG|ogg|OGG|Ogg|wav|WAV|Wav)(?![A-Za-z0-9_])")

def resolve(tok):
    """A bytecode string constant is preceded by its uleb length byte (len + 5), which may be printable; a binary .X string by a DWORD
    length. C3z-1 (2026-10-10): a trim is taken only where that byte matches (it used to try 3 blind suffixes)."""
    t = tok.decode("latin-1")
    if t.strip().lower() in june or t.strip().lower() in march:
        return t.strip()
    for i in range(1, len(t) - 4):
        c = t[i:].strip()
        if (c.lower() in june or c.lower() in march) and ord(t[i - 1]) in (len(c) + 5, len(c)):
            return c
    return None

_end_re = re.compile(tok_re.pattern.split(rb"{3,}", 1)[1])
_run_re = re.compile(rb"[A-Za-z0-9_\-\. ]+$")
def tokens(b):
    """C3z-1: a token ends at an extension and never reaches back past the previous one. Bytecode constants sit back to back, so the
    greedy tok_re swallowed `A.Y-B.Y-C.Y` as one token and lost A and B."""
    prev = 0
    for m in _end_re.finditer(b):
        e = m.end()
        r = _run_re.search(b, max(prev, e - 200), e)
        prev = e
        if r and r.end() == e and e - r.start() > 4:
            yield b[r.start():e]

# which lua files: everything adopted today (stage backup dir lists the replaced names) plus anything else in those koms.
# --all-npc (2026-10-10, user: copy every monster mesh): every unit script in kom_contents (a .lua naming INIT_MOTION), so June-only
# monster scripts no registry reaches yet are covered too; writes npc_mesh_need_all.tsv.
ALL_NPC = "--all-npc" in sys.argv
SEED = sys.argv[sys.argv.index("--seed") + 1] if "--seed" in sys.argv else None
lua_files = []
seenf = set()
if WORLDS:
    want = {l.strip().lower() for l in open(WORLDS, encoding="utf-8") if l.strip()}
    for kd in sorted(os.listdir(KC)):
        if not kd.endswith("_kom") or not os.path.isdir(os.path.join(KC, kd)):
            continue
        for f in os.listdir(os.path.join(KC, kd)):
            if f.lower() in want:
                seenf.add(os.path.join(KC, kd, f).lower()); lua_files.append((kd, f, os.path.join(KC, kd, f)))
elif ALL_NPC:
    for kd in sorted(os.listdir(KC)):
        if not kd.endswith("_kom") or not os.path.isdir(os.path.join(KC, kd)):
            continue
        for f in os.listdir(os.path.join(KC, kd)):
            p = os.path.join(KC, kd, f)
            if f.lower().endswith(".lua") and not f.lower().endswith(".src.lua") and b"INIT_MOTION" in open(p, "rb").read():
                seenf.add(p.lower()); lua_files.append((kd, f, p))
for stamp in (() if (ALL_NPC or WORLDS or SEED) else ("20261009_135231", "20261009_141115")):
    stage = os.path.join(os.path.dirname(KC), "kom_backup_stage_june", stamp)
    for kd in os.listdir(stage):
        if not kd.endswith("_kom"):
            continue
        for f in os.listdir(os.path.join(stage, kd)):
            p = os.path.join(KC, kd, f)
            if f.lower().endswith(".lua") and os.path.exists(p) and p.lower() not in seenf:
                seenf.add(p.lower()); lua_files.append((kd, f, p))
print("seed meshes (--seed):" if SEED else "world scripts (--worlds):" if WORLDS else "unit scripts (--all-npc):" if ALL_NPC else "adopted lua files:", len(lua_files))

need = {}            # lower name -> spelling
need_by = collections.defaultdict(set)
for kd, f, p in lua_files:
    d = open(p, "rb").read()
    for tk in tokens(d):
        n = resolve(tk)
        if not n:
            continue
        ln = n.lower()
        if ln in march or ln not in june:
            continue
        need[ln] = june[ln][0][3]["name"]
        need_by[ln].add(kd + "/" + f)

if SEED:
    for l in open(SEED, encoding="utf-8"):
        sn = l.strip().split("	")[0]
        if sn and sn.lower() in june:
            need[sn.lower()] = june[sn.lower()][0][3]["name"]
            need_by[sn.lower()].add("anim-collision")
        elif sn:
            print("seed not in June:", sn)

# closure: textures (and nested models) inside each needed model
def jbytes(ln):
    path, stem, v04, e = june[ln][0]
    with open(path, "rb") as fh:
        fh.seek(e["offset"]); raw = fh.read(e["csize"])
    out, err = junekom.decode(e, raw, v04)
    # World meshes ( .Y ) are MSZIP-compressed DirectX files ( 'xof 0303bzip0032' ): inflate them, or their texture
    # names are invisible to the closure ( C3x, 2026-10-10 ). lenient: a V.0.4-truncated copy still yields its names.
    if out and out[:4] == b"xof " and out[8:12] in (b"bzip", b"tzip"):
        import fxdata
        out = fxdata.xfile_text(out, lenient=True)
    return out

todo = [ln for ln in need if ln.endswith(MODEL)]
seen = set(todo)
undecodable = []
while todo:
    ln = todo.pop()
    b = jbytes(ln)
    if b is None:
        undecodable.append(ln); continue
    for tk in tokens(b):
        n = resolve(tk)
        if not n:
            continue
        cl = n.lower()
        if cl in march or cl not in june or cl in need:
            continue
        need[cl] = june[cl][0][3]["name"]
        need_by[cl].add("mesh:" + need[ln])
        if cl.endswith(MODEL) and cl not in seen:
            seen.add(cl); todo.append(cl)

by_kom = collections.Counter(june[ln][0][1] for ln in need)
by_ext = collections.Counter(os.path.splitext(ln)[1] for ln in need)
size = sum(june[ln][0][3]["size"] for ln in need)
long_names = [need[ln] for ln in need if len(need[ln]) > 59]
multi = [need[ln] for ln in need if len({c[1] for c in june[ln]}) > 1]
print("needed files:", len(need), "bytes:", size)
print("by ext:", dict(by_ext))
print("june koms:", dict(by_kom))
print("long names (>59):", long_names)
print("undecodable:", undecodable)
print("in several June koms:", len(multi))
TSV = os.path.join(OUT, "npc_anim_collision_need.tsv" if SEED else "world_asset_need.tsv" if WORLDS else "npc_mesh_need_all.tsv" if ALL_NPC else "npc_mesh_need_replaced.tsv")
with open(TSV, "w", encoding="utf-8", newline="\n") as fh:
    fh.write("name\tjune_kom\tsize\tneeded_by\n")
    for ln in sorted(need):
        fh.write("%s\t%s\t%d\t%s\n" % (need[ln], june[ln][0][1], june[ln][0][3]["size"], ";".join(sorted(need_by[ln]))[:400]))
print("wrote", TSV)
