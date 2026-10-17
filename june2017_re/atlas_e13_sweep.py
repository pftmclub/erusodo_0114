"""Atlas Phase E13 - sweep of the files the E-phases adopted that sit OUTSIDE the region file set (ATLAS_PORT_PHASES.md, "Phase E13").

    python -I june2017_re/atlas_e13_sweep.py

Read-only.  Every .lua in kom_contents modified on/after 2026-10-17 that region_census.file_set does not own (registries, PvP lists, relation data,
NPC dialogs): asset names from its text / ljconv bytecode constants, mesh / XET embedded textures expanded, each looked up staged / live / June.
BAD rows: MISSING-june-has = stage it; STAGED-NOT-LIVE = copy owed; absent-june-too = June fails the same way (registry noise).
"""
import sys,os,re,time,tempfile,collections,datetime
sys.path.insert(0,'june2017_re'); sys.path.insert(0,'tools')
import region_census as RC, fxdata, atlas_e13_closure as A
ST=RC.ST; KC=RC.KC
cut=datetime.datetime(2026,10,17,0,0).timestamp()
ext=re.compile(r'\.(x|y|xet|ksm|tga|dds|bmp|png|jpg|ogg|wav|mp3|tet)$',re.I)
region=set()
J,M,errs=RC.load_enums(); R=RC.derive(J); RC.nui_closure(R,J)
files=RC.file_set(R,J)
region={k for k in files}
cand=[]
for d in sorted(os.listdir(KC)):
    if not d.endswith('_kom'): continue
    for fn in os.listdir(os.path.join(KC,d)):
        p=os.path.join(KC,d,fn)
        if fn.lower().endswith('.lua') and os.path.getmtime(p)>=cut:
            cand.append((d,fn,p))
print('lua files modified 2026-10-17+:',len(cand),' not in region set:',sum(1 for d,fn,p in cand if fn.lower() not in region))
extra=[(d,fn,p) for d,fn,p in cand if fn.lower() not in region]
for d,fn,p in extra: print('  extra',d,fn)
tmp=tempfile.mkdtemp(); queue=[]; origin={}
for d,fn,p in extra:
    b=open(p,'rb').read()
    if b[:4]==b'\x1bKL\x81':
        cs=RC.bytecode_consts(fn) or []
    else:
        cs=RC.ASSET_RE.findall(b.decode('utf-8','replace'))
    for c in cs:
        c=c.strip()
        if ext.search(c):
            queue.append(c.lower()); origin.setdefault(c.lower(),fn)
seen=set(); bad=[]
while queue:
    low=queue.pop()
    if low in seen: continue
    seen.add(low)
    st=sorted(ST.staged.get(low,{})); lv=sorted(ST.live.get(low,{})); loose=low in ST.loose_march; jn=bool(ST.june.get(low) or ST.loose_june.get(low))
    if not (st or lv or loose):
        bad.append((low,origin.get(low),'MISSING-june-has' if jn else 'absent-june-too')); 
    elif st and not set(st)<=set(lv): bad.append((low,origin.get(low),'STAGED-NOT-LIVE'))
    if low.endswith(('.x','.y','.xet','.ksm')):
        p=ST.staged_path(low)
        if p is None:
            data,err=A.june_bytes(low)
            if data is None: continue
            p=os.path.join(tmp,'m'+os.path.splitext(low)[1]); open(p,'wb').write(data)
        try: names,fmt=fxdata.embedded_textures(p,lenient=True)
        except Exception as ex: continue
        for n in names:
            origin.setdefault(n.lower(),low); queue.append(n.lower())
print('names checked',len(seen)); 
for b in sorted(bad): print('BAD',b)
