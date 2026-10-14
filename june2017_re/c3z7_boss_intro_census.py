r"""C3z-7: spawned (NPC_GROUP) event/boss NPCs whose June unit script has DIRECTION_STATE intro states.
    python june2017_re/c3z7_boss_intro_census.py   (from the Trunk; writes june2017_re/c3z7_boss_intro_census.tsv)
"""
import os,re,csv,collections
rd=lambda f: list(csv.DictReader(open("june2017_re/"+f,encoding="utf-8"),delimiter="\t"))
covered={r["script"] for r in rd("dungeon_npc_parity.tsv") if r["verdict"]!="EXC"}
lua_of={r["name"]:r["lua_file"].lower() for r in rd("dungeon_npc_census.tsv") if r["lua_file"] and r["id_state"]!="neither"}
src={}
for kd in os.listdir("june_src_fixed"):
    p=os.path.join("june_src_fixed",kd)
    if os.path.isdir(p):
        for f in os.listdir(p):
            if f.endswith(".src.lua"): src.setdefault(f[:-8].lower()+".lua",os.path.join(p,f))
rt=lambda p: open(p,encoding="utf-8",errors="replace").read()
def block(t,i):  # text of {...} starting at first '{' at/after i
    j=t.index("{",i); d=0
    for k in range(j,len(t)):
        c=t[k]
        if c=="{": d+=1
        elif c=="}":
            d-=1
            if d==0: return j,t[j:k+1]
    return j,t[j:]
def strip(t): return re.sub(r"--[^\n]*","",t)
def spawned(t):
    t=strip(t); out=[]
    st=[(m.start(),m.group(1)) for m in re.finditer(r"^\tSTAGE(\d+)\s*=\s*\{",t,re.M)]
    ss=[(m.start(),m.group(1)) for m in re.finditer(r"^\t\tSUB_STAGE(\d+)\s*=\s*\{",t,re.M)]
    for m in re.finditer(r"NPC_GROUP\s*=\s*\{",t):
        j,b=block(t,m.start())
        s=[x for p,x in st if p<m.start()]; u=[x for p,x in ss if p<m.start()]
        # sub-stage index only valid if its position is after the stage start
        sp=[p for p,x in st if p<m.start()]
        u=[x for p,x in ss if p<m.start() and (not sp or p>sp[-1])]
        hint="S%s/SS%s"%(s[-1] if s else "?",u[-1] if u else "?")
        for n in re.findall(r"NPC_ID\s*=\s*NPC_UNIT_ID\.(NUI_[A-Z0-9_]+)",b): out.append((n,hint))
    return out
def unitinfo(t):
    t=strip(t); ds=[];foc=stop=False
    anyfoc=False
    wm=re.search(r"^\s*WAIT_STATE\s*=\s*([^\n]+)",t,re.M); wait=wm.group(1).strip().rstrip(",") if wm else ""
    for m in re.finditer(r"^([A-Za-z_][A-Za-z0-9_]*)\s*=\s*\{",t,re.M):
        j,b=block(t,m.start())
        hasf=bool(re.search(r"FOCUS_CAMERA_AT_CURRENT_STATE\s*=\s*TRUE",b))
        anyfoc|=hasf
        if re.search(r"DIRECTION_STATE\s*=\s*TRUE",b):
            ds.append((m.group(1),hasf,bool(re.search(r"STOP_AT_CURRENT_STATE\s*=\s*TRUE",b))))
    return ds,wait,anyfoc
rows=[];dun_of=collections.defaultdict(set);cache={}
for d in sorted(covered):
    p=src.get(d.lower())
    if not p: continue
    for n,h in spawned(rt(p)):
        u=lua_of.get(n)
        if not u or u not in src: continue
        if u not in cache: cache[u]=unitinfo(rt(src[u]))
        ds,wait,anyfoc=cache[u]
        if ds: v="skippable"
        elif re.match(r"event_",u,re.I) and anyfoc: v="no_direction"
        else: continue
        rows.append((d,h,n,u,v,";".join(x[0] for x in ds),wait,"Y" if any(x[1] for x in ds) else "N","Y" if any(x[2] for x in ds) else "N"))
rows=sorted(set(rows))
with open("june2017_re/c3z7_boss_intro_census.tsv","w",encoding="utf-8",newline="\n") as fh:
    fh.write("dungeon_script\tstage_hint\tnui_name\tunit_lua\tverdict\tdirection_states\twait_state\tfocus_camera\tstop_at_state\n")
    for r in rows: fh.write("\t".join(r)+"\n")
sk=[r for r in rows if r[4]=="skippable"]
print("rows",len(rows),"dungeons w/ skippable",len({r[0] for r in sk}),"distinct skippable units",len({r[3] for r in sk}))
print("no_direction:")
for r in rows:
    if r[4]=="no_direction": print(" ",r[0],r[1],r[2],r[3])
print("GATEWAY/BATTLE_AIR_SHIP rows:")
for r in rows:
    if "GATEWAY_BOSS" in r[2] or "BATTLE_AIR_SHIP" in r[2].upper(): print(" ","\t".join(r))
