import sys,capstone
sys.path.insert(0,r'F:/elsword stuff/elsword_2014/els_2014/ElswordFiles/ready_and_built/ProjectX2_SVN/ProjectX2_SVN/source/EU_CN_US/Trunk/june2017_re')
import disasm_range as dr
_t=None
def rawdis(va,maxn=200,stop_at_ret=True):
    global _t
    if _t is None: _t=dr.load_text()
    TB=dr.BASE+dr.TV; o=va-TB
    md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
    out=[]; maxjmp=va
    for i in md.disasm(_t[o:o+0x1000],va):
        out.append(f'{i.address:x}: {i.mnemonic} {i.op_str}')
        if i.mnemonic.startswith('j') and i.op_str.startswith('0x'):
            maxjmp=max(maxjmp,int(i.op_str,16))
        if stop_at_ret and i.mnemonic in('ret','jmp') and i.address>=maxjmp: break
        if len(out)>=maxn: break
    return out
if __name__=='__main__':
    for a in sys.argv[1:]:
        print('#####',a); print('\n'.join(rawdis(int(a,16))))
