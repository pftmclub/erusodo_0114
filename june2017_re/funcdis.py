"""Disassemble a June function by int3-padding bounds (works where Ghidra has not carved one).

    python funcdis.py <va>                 # window around va: 0x60 bytes before, 0x40 after
    python funcdis.py <va> <before> <after>
    python funcdis.py --func <va>          # the whole function containing va

Bounds come from the nearest 2-byte int3 run on each side. A function that follows a `ret`
with no padding is merged with its predecessor; cross-check with Ghidra's
get_function_by_address when that matters. (Buff port Phase 7b.)
"""
import os, sys, re, capstone
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from disasm_range import load_text, BASE, TV
T = load_text()
def off(va): return va - BASE - TV
def fstart(va):
    o = off(va)
    while o > 0:
        if T[o-1] == 0xcc and T[o-2] == 0xcc: return o + BASE + TV
        o -= 1
def fend(va):
    o = off(va)
    while not (T[o] == 0xcc and T[o+1] == 0xcc): o += 1
    return o + BASE + TV
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
def dis(a, b):
    for i in md.disasm(T[off(a):off(b)+16], a):
        if i.address >= b: break
        print(f"{i.address:08x}: {i.mnemonic} {i.op_str}")
a = sys.argv
if a[1] == '--func':
    s = fstart(int(a[2],16)); e = fend(int(a[2],16)); print(f"# func {s:08x}..{e:08x}"); dis(s, e)
else:
    va = int(a[1],16); bef = int(a[2]) if len(a)>2 else 0x60; aft = int(a[3]) if len(a)>3 else 0x40
    s = fstart(va); print(f"# func start {s:08x} end {fend(va):08x}")
    # align: disasm from max(s, va-bef) may misalign; start at function start if close, else walk from s
    st = s
    for i in md.disasm(T[off(s):off(va)+aft+16], s):
        if i.address >= va + aft: break
        if i.address >= va - bef: print(f"{i.address:08x}: {i.mnemonic} {i.op_str}")
