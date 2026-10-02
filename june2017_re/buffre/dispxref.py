"""dispxref.py DISP [DISP...] : find instructions in .text whose memory displacement equals DISP (hex)."""
import sys,re,struct,capstone,json,os
sys.path.insert(0,r'F:/elsword stuff/elsword_2014/els_2014/ElswordFiles/ready_and_built/ProjectX2_SVN/ProjectX2_SVN/source/EU_CN_US/Trunk/june2017_re')
import disasm_range as dr
text=dr.load_text(); TB=dr.BASE+dr.TV
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32); md.detail=True
CODE_END=0x33200000-TB
IMM=True
def find(disp):
    pat=struct.pack('<I',disp); res=[]
    for m in re.finditer(re.escape(pat),text[:CODE_END]):
        o=m.start(); hit=None
        for back in range(1,8):
            st=o-back
            try: ins=next(md.disasm(text[st:st+15],TB+st,1))
            except StopIteration: continue
            if st+ins.size < o+4: continue
            for op in ins.operands:
                if op.type==capstone.x86.X86_OP_MEM and (op.mem.disp & 0xffffffff)==disp and op.mem.base!=0:
                    hit=ins;break
                if IMM and op.type==capstone.x86.X86_OP_IMM and (op.imm & 0xffffffff)==disp and ins.mnemonic in ('add','sub','mov','lea','push','imul','cmp'):
                    hit=ins;break
            if hit: break
        if hit: res.append((hit.address,f'{hit.mnemonic} {hit.op_str}'))
    return res
if __name__=='__main__':
    for a in sys.argv[1:]:
        d=int(a,16)
        for va,t in find(d): print(hex(d),hex(va),t)
