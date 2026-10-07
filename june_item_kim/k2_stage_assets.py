"""June item.kim K2, decision D1: stage the June assets K1 listed (june_item_kim/k1_assets_to_adopt.tsv) into
kom_contents/<June kom number>_kom/ (precedent: data215/data217), manifest value "0" (non-lua rule).

    python k2_stage_assets.py plan      # print what would happen, write k2_assets_plan.tsv
    python k2_stage_assets.py apply     # copy + manifest edits (only files absent from every March dir)

Source choice per name: every June kom index entry with that name; prefer an extracted copy whose size equals the
index size (byte-exact), then a non-algo-2 entry, then K1's kom. Short copies are allowed (user, memory
elstools-truncates-june-algo2) and reported with the bytes lost.
"""
import glob, json, os, shutil, sys
from collections import OrderedDict, defaultdict

TRUNK = r"F:/elsword stuff/elsword_2014/els_2014/ElswordFiles/ready_and_built/ProjectX2_SVN/ProjectX2_SVN/source/EU_CN_US/Trunk"
JUNE = r"F:/elsword stuff/elsword_2014/els_2014/237311/14JUN17/data"
KC = r"F:/elsword stuff/elsword_2014/els_2014/237311/24965799/kom_contents"
OUT = os.path.dirname(os.path.abspath(__file__))

def june_index():
    idx = defaultdict(list)
    for p in glob.glob(os.path.join(TRUNK, "june2017_re/assets_cache/inspect/14JUN17_data_*.kom_*.json")):
        kom = os.path.basename(p)[len("14JUN17_data_"):].split(".kom_")[0]
        d = json.load(open(p, encoding="utf-8"))
        for e in d["files"]:
            idx[e["name"].lower()].append((kom, e["name"], e["size"], str(e["algo"])))
    return idx

def march_names():
    names = defaultdict(list)
    for d in os.listdir(KC):
        if d.endswith("_kom") and os.path.isdir(os.path.join(KC, d)):
            for f in os.listdir(os.path.join(KC, d)):
                names[f.lower()].append(d)
    return names

def load_manifest(path):
    raw = open(path, "rb").read()
    return json.loads(raw.decode("utf-8-sig"), object_pairs_hook=OrderedDict), raw

def write_manifest(path, m):
    body = "{\r\n" + ",\r\n".join('  %s: %s' % (json.dumps(k, ensure_ascii=False), json.dumps(v)) for k, v in m.items()) + "\r\n}"
    open(path, "wb").write(body.encode("utf-8"))

def main(mode):
    rows = [l.rstrip("\n").split("\t") for l in open(os.path.join(TRUNK, "june_item_kim/k1_assets_to_adopt.tsv"), encoding="utf-8")][1:]
    want = OrderedDict()
    for r in rows:
        if r[4].startswith("NOT IN"):
            continue
        want.setdefault(r[3], r[4].split("(")[0])
    idx, march = june_index(), march_names()
    plan = []
    for name, k1kom in want.items():
        cands = []
        for kom, real, size, algo in idx.get(name.lower(), []):
            src = os.path.join(JUNE, kom + "_kom", real)
            have = os.path.getsize(src) if os.path.isfile(src) else -1
            cands.append((have == size, algo != "2", kom == k1kom, kom, real, size, algo, src, have))
        cands.sort(reverse=True)
        if not cands or cands[0][8] < 0:
            plan.append((name, k1kom, "", 0, 0, "", "NO_SOURCE", ""))
            continue
        exact, _, _, kom, real, size, algo, src, have = cands[0]
        state = "exact" if exact else "short%d" % (size - have)
        if name.lower() in march:
            state = "IN_MARCH:" + ",".join(march[name.lower()])
        plan.append((real, k1kom, kom, size, have, algo, state, src))
    with open(os.path.join(OUT, "k2_assets_plan.tsv"), "w", encoding="utf-8") as f:
        f.write("name\tk1_kom\tsource_kom\tindex_size\tfile_size\tjune_algo\tstate\n")
        for p in plan:
            f.write("\t".join(str(x) for x in p[:7]) + "\n")
    bykom = defaultdict(lambda: [0, 0, 0])
    for p in plan:
        if p[6] in ("NO_SOURCE",) or p[6].startswith("IN_MARCH"):
            continue
        b = bykom[p[2]]
        b[0] += 1
        if p[6] != "exact":
            b[1] += 1
            b[2] += p[3] - p[4]
    print("names %d; no source %d; already in March %d; long names (>59) %d" % (len(plan),
          sum(1 for p in plan if p[6] == "NO_SOURCE"), sum(1 for p in plan if p[6].startswith("IN_MARCH")),
          sum(1 for p in plan if len(p[0]) > 59)))
    for kom in sorted(bykom):
        exists = os.path.isdir(os.path.join(KC, kom + "_kom"))
        print("  %-9s files %3d  short %3d (%d B)  dir %s" % (kom, bykom[kom][0], bykom[kom][1], bykom[kom][2], "exists" if exists else "NEW"))
    if mode != "apply":
        return
    changed = defaultdict(list)
    for p in plan:
        name, _, kom, size, have, algo, state, src = p
        if state == "NO_SOURCE" or state.startswith("IN_MARCH"):
            continue
        d = os.path.join(KC, kom + "_kom")
        os.makedirs(d, exist_ok=True)
        shutil.copy2(src, os.path.join(d, name))
        changed[kom].append(name)
    for kom, names in changed.items():
        mp = os.path.join(KC, kom + "_kom", "_manifest.json")
        m = load_manifest(mp)[0] if os.path.isfile(mp) else OrderedDict()
        for n in names:
            m[n] = "0"
        write_manifest(mp, m)
        print("  %s: +%d file(s), manifest %d entries" % (kom, len(names), len(m)))

if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "plan")
