"""Sweep June .text for memory operands at +0x16a / +0x16b and say whether the base register was just loaded
from g_pMain (0x3377585c). Buff Phase 14c (the tutorial flag, CX2Main m_bIsPlayingTutorial).

    python june2017_re/sweep_main_tutorial.py      # site  f=<int3-bounded fn>  disp  GMAIN|other  insn  <- base writer

'other' rows need a manual look (this-relative CX2Main methods, or another struct). Results: c14c_tutorial_reads.tsv.
"""
import os, sys, re, capstone
from capstone.x86 import X86_OP_MEM, X86_OP_REG
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from disasm_range import load_text, BASE, TV
T = load_text(); md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail=True
GM = 0x3377585c
def fstart(o):
    while o > 0:
        if T[o-1] == 0xcc and T[o-2] == 0xcc: return o
        o -= 1
def fend(o):
    while not (T[o] == 0xcc and T[o+1] == 0xcc): o += 1
    return o
seen=set(); funcs={}
for disp in (0x16a,0x16b):
    pat=disp.to_bytes(4,'little'); i=T.find(pat)
    while i>=0:
        fs=fstart(i); funcs.setdefault(fs,None); i=T.find(pat,i+1)
out=[]
for fs in sorted(funcs):
    fe=fend(fs+1)
    ins=list(md.disasm(T[fs:fe+16], fs+BASE+TV))
    regsrc={}  # reg -> (addr, desc)
    for k,x in enumerate(ins):
        if x.address >= fe+BASE+TV: break
        hit=None
        for op in x.operands:
            if op.type==X86_OP_MEM and op.mem.disp in (0x16a,0x16b) and op.mem.base!=0 and x.reg_name(op.mem.base)!='esp' and x.reg_name(op.mem.base)!='ebp':
                hit=(op.mem.disp, x.reg_name(op.mem.base))
        if hit:
            # look back up to 24 bytes for g_pMain load into base reg, else note last writer
            src='?'
            for y in reversed(ins[max(0,k-12):k]):
                if x.address - y.address > 40: break
                if y.op_str.startswith(hit[1]+',') or y.op_str.startswith(hit[1]+' ,'):
                    src = f"{y.address:08x} {y.mnemonic} {y.op_str}"; break
            gm = ('0x3377585c' in src)
            out.append((x.address, fs+BASE+TV, hex(hit[0]), f"{x.mnemonic} {x.op_str}", 'GMAIN' if gm else 'other', src))
for r in out: print(f"{r[0]:08x}\tf={r[1]:08x}\t{r[2]}\t{r[4]}\t{r[3]}\t<- {r[5]}")
