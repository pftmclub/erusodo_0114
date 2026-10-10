r"""Who else uses the names C3z-1 changes (June effect C3z-1, 2026-10-10; kickoff step 2).
For every name in c3z1_stage_plan.tsv (replaced by June's bytes, or new) and every name in c3z1_june_dropped.tsv
(a mesh June's own world script names but June no longer ships), list the files in kom_contents that name it: every .lua (not .src.lua)
and every .x / .y (MSZIP inflated). Each user is classed: june (its bytes equal June's, ljconv'd for .lua), replaced (C3z-1 replaces
it with June's), march (differs from June or June lacks it: a March-side user whose look changes).
    python june2017_re/c3z1_users.py          (from the Trunk; read-only; writes june2017_re/c3z1_users.tsv)
"""
import os, re, sys, csv, collections
TRUNK = os.getcwd()
sys.path.insert(0, os.path.join(TRUNK, "tools"))
sys.path.insert(0, os.path.join(TRUNK, "june2017_re"))
import kom, junekom, ljconv, fxdata

KC = kom.KOM_ROOT
table = ljconv.load_table(ljconv.TABLE_TSV)
rd = lambda f: list(csv.DictReader(open(os.path.join(TRUNK, "june2017_re", f), encoding="utf-8"), delimiter="\t"))
rep = {r["name"].lower(): r["state"] for r in rd("c3z1_stage_plan.tsv") if r["state"] in ("new", "replace")}
dropped = {r["name"] for r in rd("c3z1_june_dropped.tsv")}
target = set(rep) | dropped
print("targets: %d replaced/new, %d June-dropped" % (len(rep), len(dropped)))

june = {}
for path in junekom.all_koms()[0]:
    k = junekom.Kom(path)
    for e in k.entries:
        june.setdefault(e["name"].lower(), (path, k.v04, e))
def june_bytes(ln):
    t = june.get(ln)
    if not t:
        return None
    path, v04, e = t
    with open(path, "rb") as fh:
        fh.seek(e["offset"]); raw = fh.read(e["csize"])
    out, err = junekom.decode(e, raw, v04)
    if out is not None and out[:4] == b"\x1bKL\x82":
        out = ljconv.transcode(out, table)[0]
    return out

asset_re = re.compile(rb"\.(?:x|y|dds|tga|png|bmp|jpg|ogg|wav|mp3)(?![A-Za-z0-9_])", re.I)
run_re = re.compile(rb"[A-Za-z0-9_\-\. ]+$")
def hits(b, mesh):
    """Same tokenizer as dungeon_map_parity.py: a run ends at an extension and never reaches back past the previous one; a trim is
    valid only where the byte before the name is its length (bytecode len + 5, binary .X len)."""
    out, prev = set(), 0
    for m in asset_re.finditer(b):
        e = m.end()
        r = run_re.search(b, max(prev, e - 200), e)
        prev = e
        if not r or r.end() != e:
            continue
        t = b[r.start():e].decode("latin-1")
        if t.strip().lower() in target:
            out.add(t.strip().lower()); continue
        for i in range(1, len(t) - 4):
            c = t[i:].strip()
            if c.lower() in target and ord(t[i - 1]) in ((len(c) + 5) if not mesh else -1, len(c)):
                out.add(c.lower()); break
    return out

users = collections.defaultdict(list)     # name -> [(kom, file, class)]
n = 0
for kd in sorted(os.listdir(KC)):
    d = os.path.join(KC, kd)
    if not kd.endswith("_kom") or not os.path.isdir(d):
        continue
    for f in os.listdir(d):
        fl = f.lower()
        if not (fl.endswith((".lua", ".x", ".y")) and not fl.endswith(".src.lua")):
            continue
        b = open(os.path.join(d, f), "rb").read()
        n += 1
        if fl.endswith((".x", ".y")) and b[:4] == b"xof " and b[8:12] in (b"bzip", b"tzip"):
            b = fxdata.xfile_text(b, lenient=True) or b
        h = hits(b, not fl.endswith(".lua")) - {fl}
        if not h:
            continue
        if fl in rep:
            cls = "replaced"
        else:
            jb = june_bytes(fl)
            raw = open(os.path.join(d, f), "rb").read()
            cls = "june" if jb is not None and jb == raw else "march"
        for x in h:
            users[x].append((kd[:-4], f, cls))
print("files scanned:", n)
with open(os.path.join(TRUNK, "june2017_re", "c3z1_users.tsv"), "w", encoding="utf-8", newline="\n") as fh:
    fh.write("name\tchange\tmarch_users\tjune_users\treplaced_users\n")
    for x in sorted(target):
        u = users.get(x, [])
        g = lambda c: ";".join(sorted("%s/%s" % (k, f) for k, f, cc in u if cc == c))
        fh.write("%s\t%s\t%s\t%s\t%s\n" % (x, rep.get(x, "june-dropped"), g("march"), g("june"), g("replaced")))
mu = {x for x in target if any(c == "march" for _, _, c in users.get(x, []))}
print("names with a March-side user: %d (replaced/new %d, june-dropped %d)" % (len(mu), len(mu & set(rep)), len(mu & dropped)))
print("March-side user files:", collections.Counter(f for x in mu for k, f, c in users[x] if c == "march").most_common(40))
print("june-dropped with no user but June world scripts:", sum(1 for x in dropped if x not in mu))
