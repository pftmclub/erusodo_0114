"""Annotated listing of a June function: wide / ascii strings, float and double constants, call targets (thunk + funcmap name), vslots.

    python june2017_re/funcann.py <va>        # the function starting at <va> (to the next int3 pair)

Use it where Ghidra has not carved a function (the same bounds as funcdis.py): `push 0x3351b390 ; L"Catastrophe_Laser.X"`,
`fld qword ptr [..] ; dbl 150.0`, `call 0x31d16f90 ; <funcmap name>`, `call dword ptr [eax + 0x138] ; vslot 0x138`.
Start at a real function entry: a function that follows a ret with no padding is not split. (Character port CH3a.)
"""
import os, re, struct, sys
HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, ".."))
sys.path.insert(0, HERE)
import capstone
from disasm_range import load_text, BASE, TV, EXE
T = load_text()
D = open(EXE, 'rb').read()
RAWLIM = 0x1cbbe00
def raw(v): return v - BASE - 0xc00
def inimg(v): return BASE + 0xc00 <= v < BASE + 0xc00 + RAWLIM
FM = {}
for ln in open(os.path.join(HERE, "funcmap.tsv"), encoding="utf-8", errors="replace"):
    p = ln.rstrip("\n").split("\t")
    if len(p) >= 2:
        m = re.match(r'([0-9a-f]{8})', p[0])
        if m:
            FM.setdefault(int(m.group(1), 16), p[1][:70])
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = False
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
def rd_str(v):
    if not inimg(v): return None
    r = raw(v)
    b = D[r:r+200]
    w = []
    i = 0
    while i + 1 < len(b):
        c = b[i] | (b[i+1] << 8)
        if c == 0: break
        if 32 <= c < 127: w.append(chr(c))
        else: w = None; break
        i += 2
    if w and len(w) >= 2: return 'L"' + "".join(w) + '"'
    a = []
    for c in b:
        if c == 0: break
        if 32 <= c < 127: a.append(chr(c))
        else: a = None; break
    if a and len(a) >= 2: return '"' + "".join(a) + '"'
    return None
def thunk(v):
    if not inimg(v): return None
    t = T[off(v):off(v)+5] if BASE+TV <= v < BASE+TV+len(T) else None
    if t and t[0] == 0xe9:
        d = struct.unpack('<i', t[1:5])[0]
        return (v + 5 + d) & 0xffffffff
    return None
def note(addr, mn, op):
    n = []
    for m in re.finditer(r'0x([0-9a-f]{7,8})\b', op):
        v = int(m.group(1), 16)
        if v < 0x31aa0000 or v > 0x3a000000: continue
        if mn == 'call' or mn == 'jmp':
            t = thunk(v)
            s = FM.get(v) or ""
            if t: s += f" -> {t:08x} {FM.get(t,'')}"
            n.append(s or "fn")
            continue
        s = rd_str(v)
        if s: n.append(s); continue
        if inimg(v):
            r = raw(v)
            if mn.startswith('f') and 'qword' in op: n.append("dbl %r" % struct.unpack('<d', D[r:r+8])[0])
            elif mn.startswith('f') and 'dword' in op: n.append("flt %r" % struct.unpack('<f', D[r:r+4])[0])
            elif 'xmm' in op: n.append("flt %r" % struct.unpack('<f', D[r:r+4])[0])
            else:
                dv = struct.unpack('<I', D[r:r+4])[0]
                n.append("[%08x]=%08x (f %g)" % (v, dv, struct.unpack('<f', D[r:r+4])[0]))
    m = re.search(r'call dword ptr \[(\w+) \+ (0x[0-9a-f]+)\]', f"{mn} {op}")
    if m: n.append(f"vslot {m.group(2)}")
    return "; ".join(n)
def run(va):
    s = va; e = fend(va)
    print(f"# func {s:08x}..{e:08x}")
    for i in md.disasm(T[off(s):off(e)+16], s):
        if i.address >= e: break
        nn = note(i.address, i.mnemonic, i.op_str)
        print(f"{i.address:08x}: {i.mnemonic} {i.op_str}" + (f"    ; {nn}" if nn else ""))
if __name__ == "__main__":
    run(int(sys.argv[1], 16))
