import re,sys,collections
# Decrypt KOG inline XOR strings from Ghidra disassemble_function text.
# enc bytes: from init stores MOV <sz> ptr [G+..],imm ; keys: MOV r8,byte ptr [G+i] ... XOR r8,k
SZ={'byte':1,'word':2,'dword':4}
R8={'AL','BL','CL','DL','AH','BH','CH','DH'}
def run(path):
    lines=[l.rstrip() for l in open(path,encoding='utf-8',errors='replace') if ':' in l]
    enc={}      # addr->byte
    keys={}     # addr->key
    reg={}      # reg->('mem',addr) or ('imm',v)
    firstuse={}
    for l in lines:
        m=re.match(r'([0-9a-f]+): (\S+)\s*(.*)$',l)
        if not m: continue
        ia,op,args=m.group(1),m.group(2),m.group(3).strip()
        mm=re.match(r'(byte|word|dword) ptr \[(0x3[0-9a-f]+)\],(0x[0-9a-f]+|\d+)$',args)
        if op=='MOV' and mm:
            sz=SZ[mm.group(1)];a=int(mm.group(2),16);v=int(mm.group(3),0)
            if 0x33700000<=a<0x33a00000:
                for i in range(sz): enc[a+i]=(v>>(8*i))&0xff
            continue
        mm=re.match(r'([A-Z]{2}),byte ptr \[(0x3[0-9a-f]+)\]$',args)
        if op=='MOV' and mm and mm.group(1) in R8:
            reg[mm.group(1)]=int(mm.group(2),16); firstuse.setdefault(int(mm.group(2),16),ia); continue
        mm=re.match(r'([A-Z]{2}),(0x[0-9a-f]+|\d+)$',args)
        if op=='XOR' and mm and mm.group(1) in R8 and mm.group(1) in reg:
            keys[reg.pop(mm.group(1))]=int(mm.group(2),0); continue
        mm=re.match(r'([A-Z]{2}),',args)
        if mm and mm.group(1) in reg and op not in('XOR','CMP','TEST'): reg.pop(mm.group(1),None)
    # group contiguous key addresses into strings
    out=[];cur=[]
    for a in sorted(keys):
        if cur and a!=cur[-1]+1: out.append(cur);cur=[]
        cur.append(a)
    if cur: out.append(cur)
    for g in out:
        s=bytes((enc.get(a,0)^keys[a]) for a in g)
        print(firstuse.get(g[0],'?'),hex(g[0]),len(g),s.decode('latin1'))
run(sys.argv[1])
