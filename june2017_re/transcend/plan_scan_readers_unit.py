"""Transcend S6 planning: June readers of the transcendence flag (CX2Unit +0x3c) through the user unit (+0x867c) or a [X+0x90] load.

    python june2017_re/transcend/plan_scan_readers_unit.py > june2017_re/transcend/plan_readers_unit.txt

Linear capstone sweep of .text. The +0x867c rows are the census; the [X+0x90] rows are noise (see plan_scan_readers_my.py).
"""
import os, sys, re, capstone, collections
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
from disasm_range import load_text, BASE, TV
T = load_text()
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.skipdata = True
def fstart(va):
    o = va - BASE - TV
    while o > 2:
        if T[o-1] == 0xcc and T[o-2] == 0xcc: return o + BASE + TV
        o -= 1
    return 0
unitregs = {}  # reg -> (addr, source)
hits = []
for ins in md.disasm(T, BASE+TV):
    s = f"{ins.mnemonic} {ins.op_str}"
    m = re.match(r"mov (\w+), dword ptr \[\w+ \+ (0x867c|0x90)\]$", s)
    if m:
        unitregs[m.group(1)] = (ins.address, m.group(2), 6)
        continue
    for r,(a,src,ttl) in list(unitregs.items()):
        if re.search(rf"\[{r} \+ 0x3c\]", s):
            hits.append((ins.address, src, s))
        if re.match(rf"(mov|lea|pop|xor|movzx|movsx|add|sub|call) {r}\b", s) or s.startswith("call") or s.startswith("ret") or s.startswith("jmp"):
            unitregs.pop(r, None)
        else:
            unitregs[r] = (a, src, ttl-1)
            if ttl-1 <= 0: unitregs.pop(r, None)
by = collections.defaultdict(list)
for a,src,s in hits: by[fstart(a)].append(f"{a:08x}[{src}] {s}")
print(len(hits), "hits in", len(by), "functions")
for f in sorted(by): print(f"{f:08x}: " + " | ".join(by[f][:3]))
