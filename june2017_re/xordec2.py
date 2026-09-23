import re,sys
SZ={'byte':1,'word':2,'dword':4}
R8={'AL','BL','CL','DL','AH','BH','CH','DH'}; R16={'AX','BX','CX','DX','SI','DI','BP'}
def run(path,lo=0x33700000,hi=0x33a00000):
    enc={};keys={};wkeys={};reg={};first={}
    for l in open(path,encoding='utf-8',errors='replace'):
        m=re.match(r'([0-9a-f]+): (\S+)\s*(.*?)\s*$',l)
        if not m: continue
        ia,op,args=m.groups()
        mm=re.match(r'(byte|word|dword) ptr \[(0x[0-9a-f]+)\],(-?0x[0-9a-f]+|-?\d+)$',args)
        if op=='MOV' and mm:
            sz=SZ[mm.group(1)];a=int(mm.group(2),16);v=int(mm.group(3),0)&((1<<(8*sz))-1)
            if lo<=a<hi:
                for i in range(sz): enc[a+i]=(v>>(8*i))&0xff
            continue
        mm=re.match(r'([A-Z]{2}),(?:(?:byte|word) ptr )?\[(0x[0-9a-f]+)\]$',args)
        if op in('MOV','MOVZX') and mm and (mm.group(1) in R8 or mm.group(1) in R16):
            a=int(mm.group(2),16)
            if lo<=a<hi: reg[mm.group(1)]=a; first.setdefault(a,ia)
            continue
        mm=re.match(r'([A-Z]{2}),(-?0x[0-9a-f]+|-?\d+)$',args)
        if op=='XOR' and mm and mm.group(1) in reg:
            r=mm.group(1);a=reg.pop(r);k=int(mm.group(2),0)
            if r in R8: keys[a]=k&0xff
            else: keys[a]=k&0xff; keys[a+1]=(k>>8)&0xff
            continue
        if op=='NOT' and args in reg:
            a=reg.pop(args); keys[a]=0xff
            if args in R16: keys[a+1]=0xff
            continue
        mm=re.match(r'([A-Z]{2})\b',args)
        if mm and mm.group(1) in reg and not args.startswith(mm.group(1)+',byte ptr [E') and op not in('CMP','TEST'):
            if not (op=='MOV' and ',' in args and args.split(',')[0]!=mm.group(1)): reg.pop(mm.group(1),None)
    gs=[];cur=[]
    for a in sorted(keys):
        if a not in enc: continue
        if cur and a!=cur[-1]+1: gs.append(cur);cur=[]
        cur.append(a)
    if cur: gs.append(cur)
    res=[]
    for g in gs:
        b=bytes(enc[a]^keys[a] for a in g)
        if len(b)>=4 and b[1:2]==b'\0' and b[3:4]==b'\0': s='L"'+b.decode('utf-16le','replace')+'"'
        else: s=b.decode('latin1')
        res.append((first.get(g[0],'?'),g[0],s))
    return res
if __name__=='__main__':
    for fa,a,s in sorted(run(sys.argv[1])): print(fa,hex(a),s)
