"""Resolve one vtable slot for every RTTI class matching a prefix (Phase 9).

    python vtslot.py 0x2b8 CX2GU        # slot byte offset, class-name prefix

For each `.?AV<Class>@@` type descriptor (TD = name - 8), finds the complete-object locators
(signature 0, +0xc = TD) and each vtable that follows a dword pointing at a COL (vtable[-1] = COL),
then prints the COL's offset (+4, non-zero for a secondary base's vtable) and the function at the
given slot. Uses the README's mapping: raw = VA - 0x31aa0000 - 0xc00 (holds for everything
below raw 0x1cbbe00, which covers .text and the read-only data after it).
"""
import re, struct, sys

EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
BASE = 0x31aa0000 + 0xc00
d = open(EXE, 'rb').read()
LIMIT = 0x1cbbe00

def va(raw): return raw + BASE
def raw(v): return v - BASE

slot = int(sys.argv[1], 16)
prefix = sys.argv[2].encode()

dwords = {}  # value -> list of raw offsets (aligned), built lazily per target
def find_dword(value):
    pat = struct.pack('<I', value)
    out = []
    i = d.find(pat, 0, LIMIT)
    while i != -1:
        if i % 4 == 0:
            out.append(i)
        i = d.find(pat, i + 1, LIMIT)
    return out

for m in re.finditer(rb'\.\?AV(' + re.escape(prefix) + rb'[A-Za-z0-9_]*)@@\x00', d[:LIMIT]):
    name = m.group(1).decode()
    td = va(m.start()) - 8
    rows = []
    for c in find_dword(td):
        col = c - 0xc
        sig, off = struct.unpack_from('<II', d, col)
        if sig != 0:
            continue
        for vref in find_dword(va(col)):
            vt = vref + 4
            fn = struct.unpack_from('<I', d, vt + slot)[0]
            rows.append((off, va(vt), fn))
    for off, vt, fn in sorted(rows):
        print('%-40s off=%-4x vtable=%08x slot%x=%08x' % (name, off, vt, slot, fn))
