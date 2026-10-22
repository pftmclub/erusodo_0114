"""Transcend S6 planning: June readers of my unit's transcendence flag (g_pData +0x68 -> +0x90 -> +0x3c).

    python june2017_re/transcend/plan_scan_readers_my.py > june2017_re/transcend/plan_readers_my.txt
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
hist=[]; hits=collections.defaultdict(list)
user={}
for ins in md.disasm(T, BASE+TV):
    s = f"{ins.mnemonic} {ins.op_str}"
    m = re.match(r"mov (\w+), dword ptr \[(\w+) \+ 0x68\]$", s)
    if m: user[m.group(1)] = 8
    m2 = re.match(r"mov (\w+), dword ptr \[(\w+) \+ 0x90\]$", s)
    unit = None
    if m2 and m2.group(2) in user:
        unit = m2.group(1)
        hist.append([unit, 8])
    for h in hist:
        if h is None: continue
        if re.search(rf"\[{h[0]} \+ 0x3c\]", s):
            hits[fstart(ins.address)].append(f"{ins.address:08x} {s}")
    nh=[]
    for h in hist:
        h[1]-=1
        if h[1]>0 and not (re.match(rf"(mov|lea|pop|xor|movzx|movsx) {h[0]}\b", s) and not (m2 and h[0]==unit)): nh.append(h)
    hist=nh
    for k in list(user):
        user[k]-=1
        if user[k]<=0: user.pop(k)
print(len(hits))
for f in sorted(hits): print(f"{f:08x}: " + " | ".join(hits[f][:3]))
