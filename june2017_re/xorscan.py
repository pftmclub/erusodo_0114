import struct,re,sys,collections
d=open("F:/elsword stuff/elsword_2014/els_2014/237311/14JUN17/data/unpacked_x2.exe",'rb').read()
BASE=0x31aa0000; TR=0x400; TS=0x1cbba00; TV=0x1000
text=d[TR:TR+TS]
def va(o): return BASE+TV+o
LO,HI=0x33700000,0x33a00000
enc={}; keys={}; use={}
# enc init stores
for m in re.finditer(rb'\xc7\x05(....)(....)',text,re.S):
    a=struct.unpack('<I',m.group(1))[0]
    if LO<=a<HI:
        v=m.group(2)
        for i in range(4): enc[a+i]=v[i]
for m in re.finditer(rb'\x66\xc7\x05(....)(..)',text,re.S):
    a=struct.unpack('<I',m.group(1))[0]
    if LO<=a<HI:
        for i in range(2): enc[a+i]=m.group(2)[i]
for m in re.finditer(rb'\xc6\x05(....)(.)',text,re.S):
    a=struct.unpack('<I',m.group(1))[0]
    if LO<=a<HI: enc[a]=m.group(2)[0]
# loads + xor pairing (reg index 0..7)
loadpat=re.compile(rb'(?:\x8a([\x05\x0d\x15\x1d\x25\x2d\x35\x3d])|(\xa0))(....)',re.S)
for m in loadpat.finditer(text):
    a=struct.unpack('<I',m.group(3))[0]
    if not(LO<=a<HI): continue
    r=0 if m.group(2) else (m.group(1)[0]>>3)&7
    # search forward up to 40 bytes for XOR r,imm8
    w=text[m.end():m.end()+48]
    k=None
    for j in range(len(w)-2):
        if w[j]==0x80 and w[j+1]==0xf0+r: k=w[j+2];break
        if r==0 and w[j]==0x34: k=w[j+1];break
    if k is not None and a in enc:
        keys.setdefault(a,k); use.setdefault(a,va(m.start()))
groups=[];cur=[]
for a in sorted(keys):
    if cur and a!=cur[-1]+1: groups.append(cur);cur=[]
    cur.append(a)
if cur: groups.append(cur)
out=open(sys.argv[1],'w',encoding='utf-8')
for g in groups:
    s=bytes(enc[a]^keys[a] for a in g).decode('latin1')
    out.write('%08x\t%08x\t%d\t%s\n'%(use[g[0]],g[0],len(g),s))
print(len(groups),'strings')
