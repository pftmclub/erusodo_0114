import sys,re,capstone,struct
sys.path.insert(0,r'F:/elsword stuff/elsword_2014/els_2014/ElswordFiles/ready_and_built/ProjectX2_SVN/ProjectX2_SVN/source/EU_CN_US/Trunk/june2017_re')
import disasm_range as dr
text=dr.load_text(); base=dr.BASE+dr.TV
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32); md.skipdata=True
# find candidate jmp [reg*4+imm] by byte pattern FF 24 85/8D/95/9D/AD/B5/BD imm32
out=[]
for m in re.finditer(rb'\xff\x24[\x85\x8d\x95\x9d\xad\xb5\xbd]',text):
    off=m.start(); va=base+off
    # disassemble window before
    win=text[max(0,off-48):off+7]
    best=None
    for st in range(0,40):
        ins=list(md.disasm(win[st:],va-48+st if off>=48 else va-off+st))
        if ins and ins[-1].address==va and ins[-1].mnemonic=='jmp': best=ins;break
    if not best: continue
    s=' ; '.join(f'{i.mnemonic} {i.op_str}' for i in best[-7:])
    cmp=[int(x,16) if x.startswith('0x') else int(x) for x in re.findall(r'cmp e[a-z]{2}, (0x[0-9a-f]+|\d+)',s)]
    add=re.findall(r'(?:add|sub|lea) e[a-z]{2}, (?:\[e[a-z]{2} )?([-+]? ?0x[0-9a-f]+|[-+]? ?\d+)\]?',s)
    out.append((va,cmp,add,s))
for va,cmp,add,s in out:
    print(hex(va),cmp,add,s,sep='\t')
