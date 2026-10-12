"""Effect-data closure of June monster scripts against March's live data036 (2026-10-12; Elder bridge guard hit-box).
A June NPC script names EffectSet blocks -> DamageEffect blocks -> XMeshPlayer templets / particle sequences. March's data036 lacks most of the
June ones, so an attack's animation plays and no hit-box (DamageEffect + mesh) is ever spawned. This tool finds every such block that June
defines and March lacks, and (--apply) appends it verbatim (June order) to March's data036 files:
  EffectSet.lua, DamageEffect.lua   (BOM source; LF appended)         <- june_src_fixed/data036_kom/*.src.lua blocks
  GameMajorXMeshPlayer.txt / GameMinorXMeshPlayer.txt (CRLF; templets inserted before the file's last `}`)  <- June's own txt
Buff chain (2026-10-12): BUFF_FACTOR_ID.BFI_x / BUFF_TEMPLET_ID.BTI_x in a script or block is followed through June's BuffFactor -> BuffTemplet tables to the effect sets they name
(Mag Stone's burning-circle buff, Ignia Attack E, ...).
Particle sequences (PS) are only reported (a particle file holds the same sequences March already has under the same names).
Reached = a quoted string in a script / reached block that names a June block. Names built at run time ( "X" .. "_2" ) are not resolved.
Excluded: blocks naming SKILL_ID.* (class skills: character ports), MeshPlayer templets with a key March's engine does not parse
(AlphaTest, ALPHABLEND ...: an unknown key aborts the rest of the file), scripts of PVP heroes / pets / riding / events / bots.
    python june2017_re/npc_effect_closure.py            # report -> june2017_re/npc_effect_closure_missing.tsv + _assets.tsv
    python june2017_re/npc_effect_closure.py --apply    # append (the originals are copied to <kom_contents>/../kom_backup_effect_closure/<stamp>/)
then: kom.py stage-june / npc_mesh_census.py --seed june2017_re/npc_effect_closure_assets.tsv ; luasrc.py refresh ; kom.py pack data036 ...
"""
import os, re, sys, glob, collections, subprocess, shutil, time
T = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(T, "tools"))
import kom
KC = os.path.join(kom.KOM_ROOT, "data036_kom")
SRC = os.path.join(T, "june_src_fixed")
SCR = os.environ.get("CLOSURE_J36") or os.path.join(os.environ.get("TEMP", "."), "june_data036_extract")
KOMS = {"data037_kom"}
EXCL = re.compile(r"PVP_HERO|^Pet|^PET_|Riding|^CSM_PVP|_Hyper|^BOT_|^Event|HERO_", re.I)
APPLY = "--apply" in sys.argv

def rd(p):
    return open(p, "rb").read().decode("utf-8", "replace").replace("\r\n", "\n").lstrip("﻿")

def blocks_es(t):
    out = {}
    for m in re.finditer(r'^g_pEffectSet:AddEffectSetTemplet_LUA\(\{\n(.*?)^\}\)\n', t, re.S | re.M):
        nm = re.search(r'^\tNAME = "([^"]+)"', m.group(1), re.M)
        if nm:
            out.setdefault(nm.group(1), m.group(0))
    return out

def blocks_de(t):
    out = {}
    for m in re.finditer(r'^([A-Za-z_][A-Za-z0-9_]*) = \{\n.*?^\}\n', t, re.S | re.M):
        out.setdefault(m.group(1), m.group(0))
    return out

def blocks_txt(t, kw):
    out = {}
    for m in re.finditer(r'^\t' + kw + r' "([^"]+)"\n\t\{\n.*?^\t\}\n', t, re.S | re.M):
        out.setdefault(m.group(1), m.group(0))
    return out

def load(base, es, de):
    d = {"ES": blocks_es(rd(es)), "DE": blocks_de(rd(de)), "MP": {}, "MPF": {}, "PS": {}, "PSF": {}}
    for f in ("GameMajorXMeshPlayer.txt", "GameMinorXMeshPlayer.txt"):
        for k, v in blocks_txt(rd(os.path.join(base, f)), "TEMPLET").items():
            if k not in d["MP"]:
                d["MP"][k] = v; d["MPF"][k] = f
    for f in sorted(glob.glob(base + "/GameMajorParticle*.txt") + glob.glob(base + "/GameMinorParticle*.txt")):
        if not f.endswith("_INT.txt"):
            for k, v in blocks_txt(rd(f), "eventsequence").items():
                if k not in d["PS"]:
                    d["PS"][k] = v; d["PSF"][k] = os.path.basename(f)
    return d

if not os.path.isdir(os.path.join(SCR, "data036_kom")):
    print("extracting June data036 ->", SCR)
    subprocess.run([sys.executable, os.path.join(T, "tools", "junekom.py"), "extract", "data036", "--out", SCR], check=True)
JB = os.path.join(SCR, "data036_kom")
M = load(KC, os.path.join(KC, "EffectSet.lua"), os.path.join(KC, "DamageEffect.lua"))
J = load(JB, os.path.join(SRC, "data036_kom", "EffectSet.src.lua"), os.path.join(SRC, "data036_kom", "DamageEffect.src.lua"))
for k in ("ES", "DE", "MP", "PS"):
    print(k, "march", len(M[k]), "june", len(J[k]))

keyre = re.compile(r'^\t\t([A-Za-z_][A-Za-z0-9_]*)[ \t=]', re.M)
march_keys = set()
for b in M["MP"].values():
    march_keys.update(keyre.findall(b))
bad_mp = {n for n, b in J["MP"].items() if n not in M["MP"] and set(keyre.findall(b)) - march_keys}
print("MeshPlayer templets with a key March does not parse (excluded):", len(bad_mp))

vocab = collections.defaultdict(list)
for k in ("ES", "DE", "MP", "PS"):
    for n in J[k]:
        if not (k == "MP" and n in bad_mp):
            vocab[n].append(k)
strre = re.compile(r'"([^"\n]{2,80})"')
# Buff chain (2026-10-12; Mag Stone's burning circle): a script / damage effect names a buff by BUFF_FACTOR_ID.BFI_x or BUFF_TEMPLET_ID.BTI_x,
# not by a quoted string, and the factor -> templet -> BBT_EFFECT_SET* chain holds the effect-set names. Follow it in June's own tables.
bfre = re.compile(r'BUFF_FACTOR_ID\.(BFI_[A-Za-z0-9_]+)')
btre = re.compile(r'BUFF_TEMPLET_ID\.(BTI_[A-Za-z0-9_]+)')
def buff_blocks(fname, call, idre):
    out = {}
    for m in re.finditer(r'^g_pBuffTempletManager:' + call + r'\(\{\n(.*?)^\}\)\n', rd(os.path.join(SRC, "data036_kom", fname)), re.S | re.M):
        i = idre.search(m.group(1))
        if i:
            out.setdefault(i.group(1), m.group(0))
    return out
BF = buff_blocks("BattleFieldBuffFactor.src.lua", "AddBuffFactor_LUA", re.compile(r'^\tBUFF_FACTOR_ID = BUFF_FACTOR_ID\.(BFI_[A-Za-z0-9_]+)', re.M))
BT = buff_blocks("BattleFieldBuffTemplet.src.lua", "AddBuffTemplet_LUA", re.compile(r'^\tBUFF_TEMPLET_ID = BUFF_TEMPLET_ID\.(BTI_[A-Za-z0-9_]+)', re.M))
print("June buff factors", len(BF), "templets", len(BT))
def refs(src):
    """(kind, name) nodes a block of text points at: quoted block names, plus the buff factor / templet chain."""
    for s in set(strre.findall(src)):
        for k in vocab.get(s, ()):
            yield (k, s)
    for s in set(bfre.findall(src)):
        if s in BF:
            yield ("BF", s)
    for s in set(btre.findall(src)):
        if s in BT:
            yield ("BT", s)
def node_src(k, n):
    return {"BF": BF, "BT": BT}.get(k, J.get(k, {})).get(n, "")
scripts = []
for kd in sorted(os.listdir(SRC)):
    if kd in KOMS:
        for f in sorted(os.listdir(os.path.join(SRC, kd))):
            if f.endswith(".src.lua") and not EXCL.search(f):
                t = rd(os.path.join(SRC, kd, f))
                if "INIT_MOTION" in t:
                    scripts.append((f[:-8], t))
print("scripts", len(scripts))
reached = {}
queue = []
for nm, t in scripts:
    for key in refs(t):
        if key not in reached:
            reached[key] = nm; queue.append(key)
while queue:
    k, n = queue.pop()
    src = node_src(k, n)
    if "SKILL_ID." in src:
        continue
    for key in refs(src):
        if key not in reached:
            reached[key] = reached[(k, n)]; queue.append(key)
missing = {key: v for key, v in reached.items() if key[0] in M and key[1] not in M[key[0]] and not (key[0] in ("ES", "DE") and "SKILL_ID." in J[key[0]][key[1]])}
print("buff nodes reached: factors", sum(1 for k, _ in reached if k == "BF"), "templets", sum(1 for k, _ in reached if k == "BT"))
print("missing in March:", dict(collections.Counter(k for k, _ in missing)))

order = {}
for k in ("ES", "DE", "MP", "PS"):
    order[k] = {n: i for i, n in enumerate(J[k])}
want = {k: sorted((n for (kk, n) in missing if kk == k), key=lambda n: order[k][n]) for k in ("ES", "DE", "MP", "PS")}
with open(os.path.join(T, "june2017_re", "npc_effect_closure_missing.tsv"), "w", encoding="utf-8", newline="\n") as f:
    f.write("kind\tname\tfirst_script\n")
    for k in want:
        for n in want[k]:
            f.write("%s\t%s\t%s\n" % (k, n, reached[(k, n)]))
ext = re.compile(r'"([^"\n]{3,80}\.(?:x|y|dds|tga|png|bmp|jpg|ogg|wav))"', re.I)
march_files = {n.lower() for n in kom.live_names()} | {n.lower() for n in kom.staged_names()}
need_assets = collections.OrderedDict()
for k in ("ES", "DE", "MP", "PS"):
    for n in want[k]:
        for a in ext.findall(J[k][n]):
            if a.lower() not in march_files:
                need_assets.setdefault(a, n)
with open(os.path.join(T, "june2017_re", "npc_effect_closure_assets.tsv"), "w", encoding="utf-8", newline="\n") as f:
    f.write("name\tneeded_by\n")
    for a, n in need_assets.items():
        f.write("%s\t%s\n" % (a, n))
print("assets named by the new blocks and absent from March:", len(need_assets), "| particle sequences missing (report only):", len(want["PS"]))

# particle sequences (--particles): inserted into the June file's namesake; a sequence using a line key / enum March's files never use is skipped
PARTICLES = "--particles" in sys.argv
def psig(b):
    s = set()
    for line in b.split("\n")[2:-2]:
        w = line.strip().split()
        if not w:
            continue
        if w[0] in ("Initial", "Fade", "Global"):
            s.add(w[0] + "|" + (line.split("=")[0].split()[-1] if "=" in line else ""))
        else:
            s.add(w[0])
        s.update(re.findall(r"\b(?:PT|BT|D3DBLEND|D3DCMP|CP|OT)_[A-Z0-9_]+", line))
    return s
if PARTICLES:
    mark = set()
    for b in M["PS"].values():
        mark |= psig(b)
    bad_ps = {n for n in want["PS"] if psig(J["PS"][n]) - mark}
    print("particle sequences missing:", len(want["PS"]), "| with a key / enum March never uses (skipped):", len(bad_ps))
    for n in sorted(bad_ps)[:8]:
        print("   skip", n, sorted(psig(J["PS"][n]) - mark)[:3])
    want["PS"] = [n for n in want["PS"] if n not in bad_ps]
    byfile = collections.defaultdict(list)
    for n in want["PS"]:
        byfile[J["PSF"][n]].append(n)
    print("destination files:", {f: len(v) for f, v in sorted(byfile.items())})
    for f in byfile:
        if not os.path.exists(os.path.join(KC, f)):
            sys.exit("destination missing in March: " + f)

if APPLY:
    stamp = time.strftime("%Y%m%d_%H%M%S")
    bk = os.path.join(os.path.dirname(kom.KOM_ROOT), "kom_backup_effect_closure", stamp)
    os.makedirs(bk)
    def put(name, add_bytes_fn):
        p = os.path.join(KC, name)
        shutil.copy2(p, os.path.join(bk, name))
        b = open(p, "rb").read()
        open(p, "wb").write(add_bytes_fn(b))
    def lua_add(kind):
        def fn(b):
            txt = "".join(J[kind][n] for n in want[kind]).encode("utf-8")
            if not b.endswith(b"\n"):
                b += b"\n"
            return b + txt
        return fn
    put("EffectSet.lua", lua_add("ES"))
    put("DamageEffect.lua", lua_add("DE"))
    for fname in ("GameMajorXMeshPlayer.txt", "GameMinorXMeshPlayer.txt"):
        names = [n for n in want["MP"] if J["MPF"][n] == fname]
        def fn(b, names=names):
            txt = "".join(J["MP"][n] for n in names).replace("\n", "\r\n").encode("utf-8")
            i = b.rstrip(b"\r\n").rfind(b"}")
            # insert before the file's closing brace line
            return b[:i] + txt + b[i:]
        if names:
            put(fname, fn)
        print(fname, "+", len(names), "templets")
    if PARTICLES:
        for fname, names in sorted(byfile.items()):
            def fn(b, names=names):
                txt = "".join(J["PS"][n] for n in names).replace("\n", "\r\n").encode("utf-8")
                i = b.rstrip(b"\r\n").rfind(b"}")
                return b[:i] + txt + b[i:]
            put(fname, fn)
        print("particle sequences appended:", sum(len(v) for v in byfile.values()))
    print("APPLIED: ES +%d, DE +%d; originals in %s" % (len(want["ES"]), len(want["DE"]), bk))
