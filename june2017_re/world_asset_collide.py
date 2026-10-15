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
# C3z-1 (2026-10-10): tokens end at an extension and never reach back past the previous one (bytecode constants sit back to back, so
# a greedy match swallowed `A.Y-B.Y-C.Y` and kept only C); a trim is valid only where the byte before is the LuaJIT length (len + 5).
# Names are looked up in every kom dir, not only data040. Direct names only; dungeon_map_parity.py closes over the meshes.
# C3z-1 review (2026-10-15): no lookahead after the extension. The next constant's length byte (len + 5) follows a name, and where it is a
# letter or digit (`..._Roof_B.Y1Elder_...`) `(?![A-Za-z0-9_])` dropped the name; such a glued end needs its own length byte before it.
end=re.compile(rb"\.(?:x|y|dds|tga|ogg|wav)",re.I)
glue=re.compile(rb"[A-Za-z0-9_]")
run=re.compile(rb"[A-Za-z0-9_\-\. ]+$")
names=[l.strip() for l in open(sys.argv[1]) if l.strip()]
need=collections.defaultdict(set)
for n in names:
    b=mb(n.lower())
    if b is None: continue
    prev=0
    for m in end.finditer(b):
        e=m.end(); g=glue.match(b,e) is not None; r=run.search(b,max(prev,e-200),e)
        if not r or r.end()!=e:
            if not g: prev=e
            continue
        t=b[r.start():e].decode("latin-1")
        hit=False
        for i in range(len(t)-4):
            c=t[i:] if g else t[i:].strip()
            if i and ord(t[i-1])!=len(c)+5: continue
            if g and not i and (not r.start() or b[r.start()-1]!=len(c)+5): continue
            c=c.lower()
            if g and not (c in june or c in staged): continue
            hit=True
            if c in june and c in staged: need[c].add(n); break
            if c in june or c in staged: break
        if hit or not g: prev=e
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
