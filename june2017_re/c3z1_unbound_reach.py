r"""Lua methods that adopted June scripts call and March never binds (June effect C3z-1 play-test 2, 2026-10-10).
Reach = scripts a registered dungeon uses: its monsters' unit scripts (dungeon_npc_census.tsv), its world files (world_census.tsv),
the dungeon script. A method is "unbound" when no string literal in X2Lib / KTDXLIB / X2 spells it (no macro binders exist).
    python june2017_re/c3z1_unbound_reach.py      (from the Trunk; writes june2017_re/c3z1_unbound_reach.tsv)
"""
import os,re,csv,collections,sys
sys.path.insert(0,"tools"); import kom
KC=kom.KOM_ROOT
rd=lambda f: list(csv.DictReader(open("june2017_re/"+f,encoding="utf-8"),delimiter="\t"))
par=rd("dungeon_npc_parity.tsv")
covered={r["script"] for r in par if r["verdict"]!="EXC"}
npc=rd("dungeon_npc_census.tsv")
units=collections.defaultdict(set)   # unit lua (lower) -> dungeon scripts
for r in npc:
    if r["lua_file"] and r["id_state"]!="neither":
        for d in filter(None,re.split(r"[;,]",r["read_by"])):
            if d.strip() in covered: units[r["lua_file"].lower()].add(d.strip())
# dungeon_npc_census.tsv cuts read_by at ~400 characters, so also scan each covered dungeon script's June source for its NUI_ names
# (C3z-1: Wally Roof Normal was past the cut for NUI_SMALLGATE_WALLY_CASTLE_A)
lua_of={r["name"]:r["lua_file"].lower() for r in npc if r["lua_file"] and r["id_state"]!="neither"}
for kd in os.listdir("june_src_fixed"):
    p2=os.path.join("june_src_fixed",kd)
    if not os.path.isdir(p2): continue
    for f in os.listdir(p2):
        d=f[:-8]+".lua" if f.endswith(".src.lua") else None
        if d in covered:
            for n in set(re.findall(r"NUI_[A-Z0-9_]+",open(os.path.join(p2,f),encoding="utf-8",errors="replace").read())):
                if n in lua_of: units[lua_of[n]].add(d)
worlds=collections.defaultdict(set)
for r in rd("world_census.tsv"):
    for d in r["used_by"].split(";"):
        if d in covered: worlds[r["file"].split("->")[-1].lower()].add(d)
lit=set()
for d in ("X2Lib","KTDXLIB","X2"):
    for root,_,fs in os.walk(d):
        for f in fs:
            if f.endswith((".cpp",".h",".inl")):
                lit|={m.decode("latin-1") for m in re.findall(rb'"([A-Za-z_][A-Za-z0-9_]{2,80})"',open(os.path.join(root,f),"rb").read())}
src={}
for kd in os.listdir("june_src_fixed"):
    p=os.path.join("june_src_fixed",kd)
    if os.path.isdir(p):
        for f in os.listdir(p):
            if f.endswith(".src.lua"): src[f[:-8].lower()+".lua"]=os.path.join(p,f)
reach={}
for n,ds in units.items(): reach.setdefault(n,set()).update(ds)
for n,ds in worlds.items(): reach.setdefault(n,set()).update(ds)
for d in covered: reach.setdefault(d.lower(),set()).add(d)
miss=collections.defaultdict(lambda:[set(),set()])
nos=[]
for n,ds in reach.items():
    if n not in src: nos.append(n); continue
    t=open(src[n],encoding="utf-8",errors="replace").read()
    for m in re.finditer(r"[A-Za-z_][A-Za-z0-9_]*:([A-Za-z_][A-Za-z0-9_]*)\s*\(",t):
        if m.group(1) not in lit:
            miss[m.group(1)][0].add(n); miss[m.group(1)][1].update(ds)
print("covered dungeons",len(covered),"reached scripts",len(reach),"no june src",len(nos))
print("unbound methods reached:",len(miss))
with open("june2017_re/c3z1_unbound_reach.tsv","w",encoding="utf-8",newline="\n") as fh:
    fh.write("method\tn_scripts\tn_dungeons\tscripts\tdungeons\n")
    for k,(s,d) in sorted(miss.items(),key=lambda kv:-len(kv[1][1])):
        fh.write("%s\t%d\t%d\t%s\t%s\n"%(k,len(s),len(d),";".join(sorted(s)),";".join(sorted(d))))
        print("%-38s scripts %3d dungeons %3d  %s"%(k,len(s),len(d),"; ".join(sorted(s)[:3])))
