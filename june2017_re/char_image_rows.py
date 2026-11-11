"""char_image_rows.py - June CX2Data::GetCharacterImageName ( 31ee0de0 ) rows for given classes, read off the nine static initializers.

    python june2017_re/char_image_rows.py 34:0x39 124:0x3a        # class:row pairs; row = the iVar1 31ee0de0's class switch gives
    python june2017_re/char_image_rows.py --check                  # cross-check: Little Devil / Yama Raja rows 0x30 / 0x31 (must equal March's 48 / 49)

Needs Ghidra's HTTP server (127.0.0.1:8080, the ghidra skill) for xrefs / disassembly and unpacked_x2.exe for the wide strings.
Prints per March image type ( CIT_* 1..9 ): [file, piece, transcended file, transcended piece]. Written by char-ara CHNa (2026-11-11).
"""
import sys,struct,re,subprocess,json,urllib.request,urllib.parse
import os
sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
import xordecrypt as X
d=open(X.EXE,'rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0]
ns=struct.unpack_from('<H',d,pe+6)[0]; opt=struct.unpack_from('<H',d,pe+20)[0]
ib=struct.unpack_from('<I',d,pe+24+28)[0]
secs=[];o=pe+24+opt
for i in range(ns):
    vs,va,rs,rp=struct.unpack_from('<IIII',d,o+8); secs.append((va+ib,vs,rp,rs)); o+=40
def off(va):
    for sva,vs,rp,rs in secs:
        if sva<=va<sva+max(vs,rs): return rp+va-sva
def wstr(va):
    p=off(va); s=b''
    while True:
        c=d[p:p+2]
        if c==b'\0\0': break
        s+=c; p+=2
    return s.decode('utf-16le')
G='http://127.0.0.1:8080'
op=urllib.request.build_opener(urllib.request.ProxyHandler({}))
def get(ep,**k): return op.open(G+'/'+ep+'?'+urllib.parse.urlencode(k),timeout=120).read().decode('utf-8','replace')
bases={1:0x3378f200,2:0x33785b80,3:0x33788120,4:0x3378a6c0,5:0x3378cc60,6:0x337917a0,7:0x337835e0,8:0x33793d40,9:0x337962e0}
names={1:'DungeonClear',2:'160',3:'160Gray',4:'50',5:'20',6:'Initial',7:'MyGage',8:'Party',9:'UnitSlot'}
cache={}
args=sys.argv[1:]
PAIRS=[(0x30,31),(0x31,119)] if (not args or args==['--check']) else [(int(r,0),int(c,0)) for c,r in (a.split(':') for a in args)]
for t in range(1,10):
  for idx,cl in PAIRS:
    rec=bases[t]+idx*0x70
    xr=get('xrefs_to',address='%x'%rec)
    m=re.search(r'in (FUN_[0-9a-f]+)',xr)
    if not m: print(t,cl,'no xref',xr[:80]); continue
    f=m.group(1)[4:]
    if f not in cache: cache[f]=get('disassemble_function',address=f).splitlines()
    L=cache[f]
    i=[k for k,l in enumerate(L) if ('MOV ECX,0x%x'%rec) in l]
    if not i: print(t,cl,'no mov'); continue
    i=i[0]
    # walk back: collect 4 pushes of reg with LEA
    pushes=[];j=i-1;regoff={}
    while j>0 and len(pushes)<4:
        l=L[j]
        mm=re.search(r'LEA (E[A-Z]X),\[ESP \+ 0x([0-9a-f]+)\]',l)
        if mm: regoff.setdefault(mm.group(1),[]).append((j,int(mm.group(2),16)))
        mp=re.search(r': PUSH (E[A-Z]X)\s*$',l)
        if mp: pushes.append((j,mp.group(1)))
        j-=1
    # resolve each push's offset: the LEA for reg right before the push
    args=[]
    for (pj,reg) in pushes:  # pushes in reverse order: first found = last pushed = arg1
        k=pj-1
        while not re.search(r'LEA %s,\[ESP \+ 0x([0-9a-f]+)\]'%reg,L[k]): k-=1
        o_=int(re.search(r'0x([0-9a-f]+)\]',L[k]).group(1),16)
        npre=sum(1 for (pp,_) in pushes if pp<k)  # pushes executed before this LEA
        args.append(o_-4*npre)
    # map offsets to strings: scan back from first push for PUSH imm; LEA ECX,[ESP+o]; CALL 31aa1670
    first=min(p for p,_ in pushes)
    strs={}
    k=first-1
    while k>first-120 and len(strs)<4:
        l=L[k]
        if 'CALL 0x31aa1670' in l:
            # find LEA ECX before and PUSH imm before that
            a=k-1
            while 'LEA ECX' not in L[a]: a-=1
            o_=int(re.search(r'0x([0-9a-f]+)\]',L[a]).group(1),16)-4
            b=a
            while not re.search(r'PUSH 0x[0-9a-f]+\s*$',L[b]): b-=1
            sa=int(re.search(r'PUSH 0x([0-9a-f]+)',L[b]).group(1),16)
            strs.setdefault(o_,sa)
        k-=1
    vals=[]
    for a_ in args:
        sa=strs.get(a_)
        vals.append(wstr(sa) if sa else '?%x'%a_)
    print(names[t],cl,'%x'%rec,vals)
