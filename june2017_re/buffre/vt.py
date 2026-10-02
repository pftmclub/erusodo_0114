import struct,re,sys,json
EXE=r"F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe"
d=open(EXE,'rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0]
nsec=struct.unpack_from('<H',d,pe+6)[0]; opt=struct.unpack_from('<H',d,pe+20)[0]
base=struct.unpack_from('<I',d,pe+24+28)[0]
secs=[]
for i in range(nsec):
    o=pe+24+opt+40*i
    name=d[o:o+8].rstrip(b'\0').decode(); vs,va,rs,ra=struct.unpack_from('<IIII',d,o+8)
    secs.append((name,base+va,vs,ra,rs))
def v2o(v):
    for n,va,vs,ra,rs in secs:
        if va<=v<va+max(vs,rs): return ra+(v-va)
def o2v(o):
    for n,va,vs,ra,rs in secs:
        if ra<=o<ra+rs: return va+(o-ra)
text=[s for s in secs if s[0]=='.text'][0]
def istext(v): return text[1]<=v<text[1]+text[2]
def u32(v): return struct.unpack_from('<I',d,v2o(v))[0]
def iscol(p):
    try:
        if u32(p)!=0: return False
        td=u32(p+12); o=v2o(td+8)
        return o is not None and d[o:o+4]==b'.?AV'
    except Exception: return False
out={}
for m in re.finditer(rb'\.\?AV([A-Za-z0-9_]*'+(sys.argv[1].encode() if len(sys.argv)>1 else b'CX2Buff')+rb'[A-Za-z0-9_@]*?)@@\0',d):
    cls=m.group(1).decode(); td=o2v(m.start())-8
    tdb=struct.pack('<I',td); vts=[]
    for h in re.finditer(re.escape(tdb),d):
        col=o2v(h.start())-12
        if col is None: continue
        sig,off,cd=struct.unpack_from('<III',d,h.start()-12)
        if sig!=0: continue
        colb=struct.pack('<I',col)
        for h2 in re.finditer(re.escape(colb),d):
            vt=o2v(h2.start())+4
            ents=[]
            v=vt
            while True:
                p=u32(v)
                if not istext(p) or iscol(p): break
                ents.append(p); v+=4
            vts.append({'off':off,'vt':hex(vt),'ents':[hex(e)[2:] for e in ents]})
    out[cls]=vts
json.dump(out,open(sys.argv[2] if len(sys.argv)>2 else 'vt.json','w'),indent=1)
for c,vts in sorted(out.items()):
    for v in vts: print(c, 'off',v['off'], v['vt'], len(v['ents']), ' '.join(v['ents']))
