"""Float / double constants a June class phase reads, with the operand width (character port, step 0 of every transcription phase).

    python charport_consts.py CH1a                 # every constant of the class-map rows of phase CH1a
    python charport_consts.py --fn 32cd0200 ...    # or named functions

Why this exists: Ghidra's decompile prints `(float)_DAT_xxxx` for every constant, but `unpacked_x2.exe` stores most of them as QWORDS
(`fsub qword ptr [..]`). Read as a float, a qword like 45.0 reads 0 or a junk value (0xa0000000, 0x40000000). The width is in the
mnemonic operand, so this walks the function's instructions and reads each `fld/fadd/fsub/fmul/fdiv/fcomp ... [0x3xxxxxxx]` with the
width the instruction uses. A dword in the listing is a float, a qword a double.

The function bounds are the int3 padding bounds of funcdis.py (a function that follows a `ret` with no padding is merged with its
neighbour: check the address column).
"""
import os, re, struct, sys
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import capstone
from disasm_range import load_text, BASE, TV, EXE

T = load_text()
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
_D = None


def _image():
    global _D
    if _D is None:
        _D = open(EXE, 'rb').read()
    return _D


def _sections(d):
    e = struct.unpack_from('<I', d, 0x3c)[0]
    nsec = struct.unpack_from('<H', d, e + 6)[0]
    optsz = struct.unpack_from('<H', d, e + 20)[0]
    o = e + 24 + optsz
    secs = []
    for i in range(nsec):
        vs, va, rs, rp = struct.unpack_from('<IIII', d, o + 8)
        secs.append((va, max(vs, rs), rp))
        o += 40
    return secs


def read_va(va, n):
    d = _image()
    rva = va - BASE
    for sva, size, rp in _sections(d):
        if sva <= rva < sva + size:
            return d[rp + rva - sva: rp + rva - sva + n]
    return None


def func_bounds(va):
    o = va - BASE - TV
    e = o
    while not (T[e] == 0xcc and T[e + 1] == 0xcc):
        e += 1
    return o, e


def scan(va):
    o, e = func_bounds(va)
    for i in md.disasm(T[o:e], va):
        m = re.search(r'(dword|qword) ptr \[(0x3[0-9a-f]{7})\]', i.op_str)
        if not m or not i.mnemonic.startswith('f') or i.mnemonic.startswith('fst'):
            continue
        a = int(m.group(2), 16)
        if m.group(1) == 'qword':
            v = struct.unpack('<d', read_va(a, 8))[0]
        else:
            v = struct.unpack('<f', read_va(a, 4))[0]
        yield i.address, i.mnemonic, m.group(1), m.group(2), v


def main(argv):
    if argv and argv[0] == '--fn':
        fns = argv[1:]
    elif argv:
        tsv = os.path.join(HERE, 'char_arme', 'ch0_class_map.tsv')
        fns = []
        for l in open(tsv):
            c = l.rstrip('\n').split('\t')
            if len(c) > 5 and c[5].strip() == argv[0]:
                fns.append(c[0])
    else:
        print(__doc__)
        return
    for f in fns:
        for addr, mn, w, a, v in scan(int(f, 16)):
            print('%s %08x %-6s %s %s = %r' % (f, addr, mn, w, a, v))


if __name__ == '__main__':
    main(sys.argv[1:])
