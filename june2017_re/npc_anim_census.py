import os, re, sys, glob, struct, collections
TRUNK = r"F:\elsword stuff\elsword_2014\els_2014\ElswordFiles\ready_and_built\ProjectX2_SVN\ProjectX2_SVN\source\EU_CN_US\Trunk"
os.chdir(TRUNK)
sys.path.insert(0, os.path.join(TRUNK, "tools"))
sys.path.insert(0, os.path.join(TRUNK, "june2017_re"))
import junekom, kom
KC = r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents"
pat = sys.argv[1] if len(sys.argv) > 1 else "ELYSION_*"

march = collections.defaultdict(list)
for d in os.listdir(KC):
    if not d.endswith("_kom"): continue
    for f in os.listdir(os.path.join(KC, d)):
        march[f.lower()].append(os.path.join(KC, d, f))

june = {}
def june_index():
    for path in junekom.all_koms()[0]:
        k = junekom.Kom(path)
        for e in k.entries:
            june.setdefault(e["name"].lower(), []).append((path, k.v04, e))
june_index()

def jbytes(ln):
    best = None
    for path, v04, e in june.get(ln, []):
        with open(path, "rb") as fh:
            fh.seek(e["offset"]); raw = fh.read(e["csize"])
        out, err = junekom.decode(e, raw, v04)
        if out: return out
    return None

def has(b, name):
    return (struct.pack("<I", len(name)) + name.encode()) in b

rows = []
for p in sorted(glob.glob("june_src_fixed/*/" + pat + ".src.lua")):
    t = open(p, "rb").read().decode("utf-8", "replace")
    m = re.search(r'MOTION_FILE_NAME\s*=\s*"([^"]+)"', t)
    if not m: continue
    mesh = m.group(1)
    anims = sorted(set(re.findall(r'ANIM_NAME\s*=\s*"([^"]+)"', t)))
    mp = march.get(mesh.lower(), [])
    mb = [open(x, "rb").read() for x in mp]
    jb = jbytes(mesh.lower())
    miss_m = [a for a in anims if not any(has(b, a) for b in mb)] if mb else anims
    miss_j = [a for a in anims if not (jb and has(jb, a))] if jb else None
    rows.append((os.path.basename(p)[:-8], mesh, len(mp), jb is not None, miss_m, miss_j))
for r in rows:
    nm, mesh, nmp, hasj, mm, mj = r
    if not mm: continue
    verdict = "JUNE-FIXES" if (hasj and mj is not None and not [a for a in mm if a in mj]) else ("PARTIAL" if hasj else "NO-JUNE-MESH")
    fixed = [a for a in mm if mj is not None and a not in mj]
    print("%s\t%s\tmarchcopies=%d\tmissing_in_march=%s\tstill_missing_in_june=%s\t%s" % (nm, mesh, nmp, mm, mj if mj is None else [a for a in mm if a in mj], verdict))
print("scripts", len(rows), "with march-missing anims", sum(1 for r in rows if r[4]))
