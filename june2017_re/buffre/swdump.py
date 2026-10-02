"""swdump.py JMPVA [--base N] [--max K] : dump a jump-table switch. Finds movzx idx table + addr table
from the instructions just before JMPVA, maps case value -> target, and disassembles each target block
(until ret/jmp/next target) printing lea/mov ecx, calls, pushes, and member stores."""
import sys,re,struct,capstone
sys.path.insert(0,r'F:/elsword stuff/elsword_2014/els_2014/ElswordFiles/ready_and_built/ProjectX2_SVN/ProjectX2_SVN/source/EU_CN_US/Trunk/june2017_re')
import disasm_range as dr
text=dr.load_text(); TB=dr.BASE+dr.TV
def rd(va,n): o=va-TB; return text[o:o+n]
def u32(va): return struct.unpack('<I',rd(va,4))[0]
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
FULL=True
jva=int(sys.argv[1],16); base=int(sys.argv[2]) if len(sys.argv)>2 else 0
pre=None
for st in range(12,64):
    ins=list(md.disasm(rd(jva-st,st+7),jva-st))
    if ins and ins[-1].address==jva:
        s=' ; '.join(f'{i.mnemonic} {i.op_str}' for i in ins[-10:])
        if re.search(r'cmp e\w\w, (0x[0-9a-f]+|\d+)\s*;\s*ja',s): pre=ins; break
m=re.search(r'jmp dword ptr \[e\w\w\*4 \+ (0x[0-9a-f]+)\]',s); at=int(m.group(1),16)
mi=re.search(r'movzx e\w\w, byte ptr \[e\w\w \+ (0x[0-9a-f]+)\]',s)
mc=re.search(r'cmp e\w\w, (0x[0-9a-f]+|\d+)\s*;\s*ja',s); cnt=int(mc.group(1),0)+1
cases={}
for i in range(cnt):
    idx=rd(int(mi.group(1),16)+i,1)[0] if mi else i
    cases.setdefault(u32(at+4*idx),[]).append(i+base)
targets=sorted(cases)
default=None
print('# jmp',hex(jva),'pre:',s); print('# cases',cnt,'targets',len(targets))
for k,t in enumerate(targets):
    nxt=targets[k+1] if k+1<len(targets) else t+0x200
    lines=[]
    for i in md.disasm(rd(t,min(nxt-t,0x200)),t):
        txt=f'{i.mnemonic} {i.op_str}'
        if re.search(r'lea ecx|mov ecx|call|push 0x|push \d|mov (byte|dword|word) ptr \[e[sd]i \+|fstp dword ptr \[e[sd]i|movss|jmp|ret|\[e[sd]i \+ 0x[0-9a-f]{3,}\]',txt): lines.append(f'{i.address:x}:{txt}')
        if i.mnemonic in ('ret','jmp'): break
    cv=cases[t]
    if len(cv)>20 and not FULL: label=f'MANY({len(cv)}) e.g. {cv[:6]}'
    else: label=','.join(map(str,cv))
    print(f'== {label} @ {t:x}'); print('   '+'\n   '.join(lines[:40]))
