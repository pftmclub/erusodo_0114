"""Replace March's GameMajorParticle*.txt / GameMinorParticle.txt with June's, byte-exact (2026-10-12, user: "replace the particle files").
45 of the 68 files are already identical (the Lanox run shipped June 24-67). The other 23 (1-23 and Minor) differ: 140 same-name sequences take June's
body, 1,278 June-only sequences arrive, and March has 122 sequences that no June file defines (43 of them still named by EffectSet / DamageEffect /
NPC scripts). Those 122 are NOT dropped: each is re-appended, byte for byte, to the namesake file. The 231 sequences closure --particles appended
are already in June's files. A June file with a line key or enum March's files never use is refused (none today).
    python june2017_re/particle_files_replace.py            # report + missing assets -> june2017_re/particle_files_assets.tsv
    python june2017_re/particle_files_replace.py --apply    # copy originals to kom_backup_particle_files/<stamp>/, write the files
"""
import os, re, sys, glob, shutil, time, collections
T = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(T, "tools"))
import kom
KC = os.path.join(kom.KOM_ROOT, "data036_kom")
JB = os.path.join(os.environ.get("CLOSURE_J36") or os.path.join(os.environ.get("TEMP", "."), "june_data036_extract"), "data036_kom")
APPLY = "--apply" in sys.argv
BLK = re.compile(r'^\teventsequence "([^"]+)"\r?\n\t\{\r?\n.*?^\t\}\r?\n', re.S | re.M)

def rd(p):
    return open(p, "rb").read().decode("latin-1")

def blocks(t):
    out = collections.OrderedDict()
    for m in BLK.finditer(t):
        out.setdefault(m.group(1), m.group(0))
    return out

def psig(b):
    s = set()
    for line in b.replace("\r", "").split("\n")[2:-2]:
        w = line.strip().split()
        if not w:
            continue
        if w[0] in ("Initial", "Fade", "Global"):
            s.add(w[0] + "|" + (line.split("=")[0].split()[-1] if "=" in line else ""))
        else:
            s.add(w[0])
        s.update(re.findall(r"\b(?:PT|BT|D3DBLEND|D3DCMP|CP|OT)_[A-Z0-9_]+", line))
    return s

files = sorted(os.path.basename(f) for f in glob.glob(JB + "/Game*Particle*.txt") if not f.endswith("_INT.txt"))
M = {f: rd(os.path.join(KC, f)) for f in files}
J = {f: rd(os.path.join(JB, f)) for f in files}
mb = {f: blocks(M[f]) for f in files}
jb = {f: blocks(J[f]) for f in files}
jnames = set().union(*[set(v) for v in jb.values()])
msig = set()
for f in files:
    for b in mb[f].values():
        msig |= psig(b)
todo = [f for f in files if M[f] != J[f]]
print("files:", len(files), "| to replace:", len(todo))
unk = {f: [n for n, b in jb[f].items() if psig(b) - msig] for f in todo}
if any(unk.values()):
    sys.exit("REFUSED: unknown line key / enum in " + str({f: v[:3] for f, v in unk.items() if v}))
keep = {f: [(n, b) for n, b in mb[f].items() if n not in jnames] for f in todo}
print("March-only sequences re-appended:", sum(len(v) for v in keep.values()), {f: len(v) for f, v in keep.items() if v})
ext = re.compile(r'"([^"\n]{3,80}\.(?:x|y|dds|tga|png|bmp|jpg))"', re.I)
march_files = {n.lower() for n in kom.live_names()} | {n.lower() for n in kom.staged_names()}
need = collections.OrderedDict()
for f in todo:
    for n, b in jb[f].items():
        for a in ext.findall(b):
            if a.lower() not in march_files:
                need.setdefault(a, n)
with open(os.path.join(T, "june2017_re", "particle_files_assets.tsv"), "w", encoding="utf-8", newline="\n") as fo:
    fo.write("name\tneeded_by\n")
    for a, n in need.items():
        fo.write("%s\t%s\n" % (a, n))
print("assets named by June's particle files and absent from March:", len(need))

if APPLY:
    bk = os.path.join(os.path.dirname(kom.KOM_ROOT), "kom_backup_particle_files", time.strftime("%Y%m%d_%H%M%S"))
    os.makedirs(bk)
    for f in todo:
        shutil.copy2(os.path.join(KC, f), os.path.join(bk, f))
        b = open(os.path.join(JB, f), "rb").read()
        if keep[f]:
            add = "".join(x for _, x in keep[f])
            if b"\r\n" in b:
                add = add.replace("\r\n", "\n").replace("\n", "\r\n")
            i = b.rstrip(b"\r\n").rfind(b"}")
            b = b[:i] + add.encode("latin-1") + b[i:]
        open(os.path.join(KC, f), "wb").write(b)
    print("APPLIED to", len(todo), "files; originals in", bk)
