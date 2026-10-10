"""Same-name collisions: meshes / textures / sounds named by the given world scripts and line maps that BOTH March (kom_contents)
and June have, with different bytes. June wins at the same filename (standing rule), so these are what a map port must replace.
C3x play-test (2026-10-10): 221 of 1062 shared names differ; March's old geometry drew over June's line maps ("layout weird").
    python june2017_re/world_asset_collide.py <names.txt>     (one world script / line map name per line; from the Trunk)
Writes june2017_re/world_asset_collide.tsv (name, march size, june size, first user). Not closed over textures yet (C3z-1 adds that).
"""
import sys,os,re,collections
sys.path.insert(0,"tools"); sys.path.insert(0,"june2017_re")
import kom,junekom
KC=kom.KOM_ROOT
june={}
for path in junekom.all_koms()[0]:
    k=junekom.Kom(path)
    for e in k.entries: june.setdefault(e["name"].lower(),(path,k.v04,e))
staged=kom.staged_names()
def jb(n):
    path,v04,e=june[n]
    with open(path,"rb") as fh: fh.seek(e["offset"]); raw=fh.read(e["csize"])
    return junekom.decode(e,raw,v04)[0]
def mb(n):
    d=staged.get(n)
    if not d: return None
    stem,sp=sorted(d.items())[0]
    return open(os.path.join(KC,stem+"_kom",sp),"rb").read()
tok=re.compile(rb"[A-Za-z0-9_\-\. ]{3,}\.(?:x|y|dds|tga|ogg|wav)(?![A-Za-z0-9_])",re.I)
names=[l.strip() for l in open(sys.argv[1]) if l.strip()]
need=collections.defaultdict(set)
for n in names:
    f=[x for x in os.listdir(KC+"/data040_kom") if x.lower()==n.lower()][0]
    for m in tok.finditer(open(KC+"/data040_kom/"+f,"rb").read()):
        t=m.group(0).decode("latin-1")
        for i in range(len(t)-4):
            c=t[i:].strip().lower()
            if c in june and c in staged: need[c].add(n); break
            if c in june or c in staged: break
c=collections.Counter(); diffs=[]
for n in sorted(need):
    j=jb(n); m=mb(n)
    if j is None: c["june undecodable"]+=1; continue
    if m==j: c["same"]+=1
    else: c["DIFF"]+=1; diffs.append((n,len(m or b""),len(j),sorted(need[n])[0]))
print(dict(c))
import csv
with open("june2017_re/world_asset_collide.tsv","w",newline="\n") as fh:
    fh.write("name\tmarch\tjune\tused_by\n")
    for d in diffs: fh.write("%s\t%d\t%d\t%s\n"%d)
for d in diffs[:25]: print(d)
