"""Which worlds the dungeon scripts in kom_contents use differ between March and June: the world script and every .lua it names
(its line map). June bytecode is ljconv'd first, so `same` means byte-identical March bytecode and DIFF is a real content change.
Writes june2017_re/world_census.tsv (world, file, state = same / DIFF / june-only / march-only / none, sizes, used_by).
June effect C3s play-test (2026-10-10): 374 worlds, 137 reworked by June (126 line maps, 61 world scripts), 142 dungeon scripts on them.
    python june2017_re/world_census.py        (from the Trunk)
"""
import os, re, sys, collections
TRUNK = os.getcwd()
sys.path.insert(0, os.path.join(TRUNK, "tools"))
sys.path.insert(0, os.path.join(TRUNK, "june2017_re"))
import kom, junekom, ljconv

KC = kom.KOM_ROOT
table = ljconv.load_table(ljconv.TABLE_TSV)

def rows(t):
    return {m.group(1): m.group(2) for m in re.finditer(r'worldName\s*=\s*"([^"]+)"\s*,\s*dataFileName\s*=\s*"([^"]+)"', t)}

# WorldData.lua is BOM source since C3x (core list); before that its text was the .src.lua sidecar.
_wd = os.path.join(KC, "data040_kom", "WorldData.lua")
if open(_wd, "rb").read(3) != b"\xef\xbb\xbf":
    _wd = os.path.join(KC, "data040_kom", "WorldData.src.lua")
mwd = rows(open(_wd, encoding="utf-8-sig", errors="replace").read())
jwd = rows(open(os.path.join(TRUNK, "june_src_fixed", "data040_kom", "WorldData.src.lua"), encoding="utf-8", errors="replace").read())

staged = kom.staged_names()          # lower -> {stem: spelling}
def march_bytes(name):
    d = staged.get(name.lower())
    if not d:
        return None
    stem, sp = sorted(d.items())[0]
    p = os.path.join(KC, stem + "_kom", sp)
    return open(p, "rb").read() if os.path.exists(p) else None

june = {}
for path in junekom.all_koms()[0]:
    k = junekom.Kom(path)
    for e in k.entries:
        june.setdefault(e["name"].lower(), (path, k.v04, e))
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
        # every suffix (C3x): a constant can glue onto the one before it ("SetBGM.X_LineMap.lua"), and a 3-char trim missed
        # 35 line maps that way; the first (longest) known name wins
        for i in range(0, len(t) - 4):
            c = t[i:]
            if c.lower() in june or c.lower() in staged:
                out.add(c); break
    return out

# worlds used by dungeon scripts in kom_contents
use = collections.defaultdict(set)
for kd in os.listdir(KC):
    if not kd.endswith("_kom"):
        continue
    for f in os.listdir(os.path.join(KC, kd)):
        if f.lower().startswith("dungeon_") and f.lower().endswith(".lua") and not f.lower().endswith(".src.lua"):
            for w in set(re.findall(rb"WI_[A-Z0-9_]+", open(os.path.join(KC, kd, f), "rb").read())):
                w = w.decode()
                if w in mwd:
                    use[w].add(f)

res = {}
for w in sorted(use):
    mf, jf = mwd[w], jwd.get(w, mwd[w])
    mb, jb = march_bytes(mf), june_bytes(jf)
    files = [(mf, jf, mb, jb)]
    for n in sorted(names_in(mb) | names_in(jb)):
        if n.lower() in (mf.lower(), jf.lower()):
            continue
        files.append((n, n, march_bytes(n), june_bytes(n)))
    st = []
    for a, b, x, y in files:
        if x is None and y is None: s = "none"
        elif x is None: s = "june-only"
        elif y is None: s = "march-only"
        elif x == y: s = "same"
        else: s = "DIFF"
        st.append((a if a.lower() == b.lower() else a + "->" + b, s, len(x or b""), len(y or b"")))
    res[w] = st

dungeons = collections.defaultdict(set)
for w, st in res.items():
    if any(s in ("DIFF", "june-only") for _, s, _, _ in st):
        for f in use[w]:
            dungeons[f].add(w)
print("worlds used by dungeon scripts:", len(res))
print("worlds whose map script / line map differ from June:", sum(1 for st in res.values() if any(s in ("DIFF", "june-only") for _, s, _, _ in st)))
print("worlds identical to June:", sum(1 for st in res.values() if all(s in ("same", "none") for _, s, _, _ in st)))
with open(os.path.join(TRUNK, "june2017_re", "world_census.tsv"), "w", encoding="utf-8", newline="\n") as fh:
    fh.write("world\tfile\tstate\tmarch_size\tjune_size\tused_by\n")
    for w, st in res.items():
        for n, s, a, b in st:
            fh.write("%s\t%s\t%s\t%d\t%d\t%s\n" % (w, n, s, a, b, ";".join(sorted(use[w]))[:300]))
print("dungeon scripts on a changed world:", len(dungeons))
for f in sorted(dungeons)[:200]:
    print(" ", f, len(dungeons[f]))
