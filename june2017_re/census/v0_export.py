"""Phase V0 of june-census: export every June function from Ghidra (read-only HTTP) to functions.tsv.
Usage: python june2017_re/census/v0_export.py   (Ghidra open on the June 2017 program, GhidraMCP on :8080)"""
import re, sys, time, bisect, urllib.request, urllib.parse, os

G = "http://127.0.0.1:8080"
OUT = os.path.dirname(os.path.abspath(__file__))
opener = urllib.request.build_opener(urllib.request.ProxyHandler({}))

def get(ep, **kw):
    url = G + "/" + ep + ("?" + urllib.parse.urlencode(kw) if kw else "")
    return opener.open(url, timeout=300).read().decode("utf-8", "replace")

t0 = time.time()
segs = get("segments", limit=50)
assert "31aa0000" in segs.splitlines()[0], "not the June 2017 program: " + segs.splitlines()[0]

funcs = []  # (va, name)
for ln in get("list_functions").splitlines():
    m = re.match(r"^(.*) at ([0-9a-f]{8})$", ln)
    if m:
        funcs.append((int(m.group(2), 16), m.group(1)))
funcs.sort()
starts = [f[0] for f in funcs]
assert len(set(starts)) == len(starts), "duplicate function start"
methods_count = len(get("methods", offset=0, limit=100000).splitlines())

def owner(addr):
    i = bisect.bisect_right(starts, addr) - 1
    return starts[i] if i >= 0 else None

# candidate strings: (addr, text) from the ascii listing and the data listing (wide strings)
cands = {}
for ln in get("strings", limit=2000000).splitlines():
    m = re.match(r'^([0-9a-f]{8}): "(.*)"$', ln)
    if m:
        cands[int(m.group(1), 16)] = m.group(2)
for ln in get("data", limit=3000000).splitlines():
    m = re.match(r'^([0-9a-f]{8}): .* = u"(.*)"$', ln)
    if m:
        cands.setdefault(int(m.group(1), 16), m.group(2).replace("\\\\", "\\"))

path_re = re.compile(r'([A-Za-z]:\\[^"(,]*?[\\/]([A-Za-z0-9_]+\.(?:cpp|h|c|cc)))\b')
cls_re = re.compile(r'^\(?([A-Za-z_][A-Za-z0-9_]*)::~?([A-Za-z_][A-Za-z0-9_]*)')
file_ev, cls_ev = {}, {}  # va -> {value: evidence}
n_path = n_cls = 0
for a, s in sorted(cands.items()):
    pm = path_re.search(s)
    cm = None if pm else cls_re.match(s)
    if not pm and not cm:
        continue
    refs = get("xrefs_to", address="0x%08x" % a, limit=200)
    for r in re.finditer(r"From ([0-9a-f]{8})", refs):
        fo = owner(int(r.group(1), 16))
        if fo is None:
            continue
        if pm:
            file_ev.setdefault(fo, {}).setdefault(pm.group(1).replace("\\", "/"), "path@%08x" % a)
        else:
            cls_ev.setdefault(fo, {}).setdefault(cm.group(1), "str@%08x" % a)
    n_path += bool(pm); n_cls += bool(cm)

rows = []
for i, (va, name) in enumerate(funcs):
    nxt = funcs[i + 1][0] if i + 1 < len(funcs) else None
    size = (nxt - va) if nxt else ""
    fe = file_ev.get(va, {}); ce = cls_ev.get(va, {})
    sf = ";".join(sorted(fe)); cl = ";".join(sorted(ce))
    ev = ";".join([v for v in list(fe.values()) + list(ce.values())][:4])
    rows.append(("0x%08x" % va, name, size, sf, cl, ev))

with open(os.path.join(OUT, "functions.tsv"), "w", encoding="utf-8", newline="\n") as f:
    f.write("# June 2017 (unpacked_x2.exe, imagebase 0x31aa0000) function inventory; size_approx = gap to next function start (approximate)\n")
    f.write("june_va\tghidra_name\tsize_approx\tsource_file\tclass\tevidence\n")
    for r in rows:
        f.write("\t".join(str(x) for x in r) + "\n")

tot = len(rows)
wf = sum(1 for r in rows if r[3]); wc = sum(1 for r in rows if r[4])
wb = sum(1 for r in rows if r[3] and r[4]); wn = sum(1 for r in rows if not r[3] and not r[4])
print("TOTAL", tot, "methods_endpoint", methods_count, "unique_va", len(set(r[0] for r in rows)))
print("path strings used", n_path, "class strings used", n_cls)
print("with_file", wf, "with_class", wc, "both", wb, "neither", wn, "any", tot - wn)
print("seconds %.1f" % (time.time() - t0))
