"""Transcode June2017 KOG LuaJIT bytecode (1b 4b 4c 82) to mar-2014 (1b 4b 4c 81).

Phase 10b, route (b). The March client's LuaJIT (luajitLib/LuaJIT-2.0.2) is
stock 2.0.2 except for the dump header: lj_bcdump.h swaps HEAD2/HEAD3 to
'K','L' and BCDUMP_VERSION to 0x81 under LUAJIT_MODIFIED_BYTECODE_HEADER, and
lj_bc.h's BCDEF is stock order (checked against upstream's v2.0 branch: the
only other differences are later upstream bug fixes). June is the same format
with a permuted opcode numbering. So transcoding is: rewrite byte 3 of the
header, and renumber the opcode byte (byte 0) of every instruction. Nothing
else in the dump changes.

The KOG->stock table:
  - every opcode outside FIX: elstools' own translation, read back from the
    (june .lua, .std.luac) pairs that `elstools decompile-batch` leaves behind;
  - FIX (ljd_jun2017.FIX): the 9 slots elstools gets wrong (0x2f-0x37).
The script asserts the combined table is a function and injective over every
opcode observed, and that each slot agrees with the June binary's own
lj_bc_mode[] (lj_bcmode.py) - operand types must fit the stock opcode the slot
is mapped to. An unmapped KOG opcode is a hard error, never a pass-through.

Instruction positions come from this script's own dump walker, not from ljd.

    python ljconv.py OUTDIR [june_lua/*.lua]      # default: the 14 files in june_lua/
    python ljconv.py --check-only                 # build + check the table, write nothing

Writes OUTDIR/<name>.lua (mar-2014 bytecode) and prints, per file, the
instruction count, how many instructions hit FIX slots, and whether the output
is byte-identical to the header-swapped .std.luac (expected exactly when the
file has no FIX-slot instructions).
"""
import glob
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from ljd_jun2017 import FIX  # noqa: E402

JUNE_MAGIC = b'\x1bKL\x82'
MAR_MAGIC = b'\x1bKL\x81'
STD_MAGIC = b'\x1bLJ\x01'
BCDUMP_F_BE = 0x01
BCDUMP_F_STRIP = 0x02


def uleb(d, p):
    v = 0
    sh = 0
    while True:
        b = d[p]
        p += 1
        v |= (b & 0x7f) << sh
        if b < 0x80:
            return v, p
        sh += 7


def instruction_offsets(d, magic):
    """File offsets of every instruction's opcode byte, walking the dump format
    (lj_bcdump.h): header, then length-prefixed prototypes up to a 0 length."""
    if d[:4] != magic:
        raise ValueError('header %s, expected %s' % (d[:4].hex(), magic.hex()))
    flags, p = uleb(d, 4)
    if flags & BCDUMP_F_BE:
        raise ValueError('big-endian dump')
    if not flags & BCDUMP_F_STRIP:
        n, p = uleb(d, p)
        p += n  # chunk name
    offs = []
    while True:
        plen, p = uleb(d, p)
        if plen == 0:
            break
        end = p + plen
        q = p + 4  # flags, numparams, framesize, sizeuv
        _, q = uleb(d, q)  # sizekgc
        _, q = uleb(d, q)  # sizekn
        sizebc, q = uleb(d, q)
        if not flags & BCDUMP_F_STRIP:
            sizedbg, q = uleb(d, q)
            if sizedbg:
                _, q = uleb(d, q)
                _, q = uleb(d, q)
        offs.extend(range(q, q + 4 * sizebc, 4))
        if q + 4 * sizebc > end:
            raise ValueError('prototype at 0x%x: bytecode overruns its length' % p)
        p = end
    if p != len(d):
        raise ValueError('%d trailing byte(s) after the last prototype' % (len(d) - p))
    return offs


def june_files(args):
    return args or sorted(f for f in glob.glob(os.path.join(HERE, 'june_lua', '*.lua'))
                          if not f.endswith('.src.lua'))


def build_table(files):
    """KOG opcode -> stock opcode. Returns (table, per-op evidence counts)."""
    table = {}
    seen = {}
    for f in files:
        kog = open(f, 'rb').read()
        std = open(f[:-4] + '.std.luac', 'rb').read()
        if len(kog) != len(std):
            raise SystemExit('%s: length differs from its .std.luac' % f)
        offs = instruction_offsets(kog, JUNE_MAGIC)
        if instruction_offsets(std, STD_MAGIC) != offs:
            raise SystemExit('%s: .std.luac has a different instruction layout' % f)
        opset = set(offs)
        bad = [i for i in range(4, len(kog)) if kog[i] != std[i] and i not in opset]
        if bad:
            raise SystemExit('%s: %d non-opcode byte(s) differ from .std.luac, first at 0x%x'
                             % (f, len(bad), bad[0]))
        for o in offs:
            k, s = kog[o], std[o]
            seen[k] = seen.get(k, 0) + 1
            if k in FIX:
                continue
            if table.setdefault(k, s) != s:
                raise SystemExit('elstools maps KOG 0x%02x to both 0x%02x and 0x%02x' % (k, table[k], s))
    for k, s in FIX.items():
        table[k] = s
    inv = {}
    for k, s in table.items():
        if s in inv and inv[s] != k:
            raise SystemExit('not injective: KOG 0x%02x and 0x%02x both map to stock 0x%02x' % (inv[s], k, s))
        inv[s] = k
    return table, seen


def check_modes(table):
    """Each mapped slot's operand types in June's lj_bc_mode[] must be those of
    the stock opcode it maps to. Returns the number of slots checked."""
    import lj_bcmode
    d = open(lj_bcmode.EXE, 'rb').read()
    hits = lj_bcmode.find(d)
    if len(hits) != 1:
        raise SystemExit('lj_bc_mode[]: %d candidate tables in the June binary' % len(hits))
    _, t = hits[0]
    names = [s[0] for s in lj_bcmode.std]
    for k, s in sorted(table.items()):
        if (t[k] & 0x7ff) != lj_bcmode.STDLOW[s]:
            raise SystemExit('KOG 0x%02x -> %s: operand types disagree with June lj_bc_mode[]' % (k, names[s]))
    return len(table), names


def transcode(d, table):
    offs = instruction_offsets(d, JUNE_MAGIC)
    out = bytearray(d)
    out[:4] = MAR_MAGIC
    fixhits = 0
    for o in offs:
        k = d[o]
        if k not in table:
            raise ValueError('KOG opcode 0x%02x at 0x%x has no mapping' % (k, o))
        fixhits += k in FIX
        out[o] = table[k]
    instruction_offsets(bytes(out), MAR_MAGIC)  # still walks cleanly
    return bytes(out), len(offs), fixhits


LUAJIT = os.path.join(HERE, '..', 'elstools', 'vendor', 'luajit', 'luajit.exe')


def disasm_check(out, std, fixhits, tmpdir):
    """The plan's check: disassemble the transcoded file and the elstools
    .std.luac (header set to 0x81 so the KOG luajit's stock-order loader reads
    it) with `luajit -bl`, and compare. Returns (lines, differing lines, and
    whether every differing line is a FIX slot decoded two ways). Expected:
    differing == fixhits, since the two files differ only in FIX-slot opcodes."""
    import re
    import subprocess
    fixnames = {'ITERC', 'ITERN', 'VARG', 'ISNEXT', 'UGET', 'USETV', 'USETS', 'USETN', 'USETP'}
    # Same file name in two directories: `-bl` prints the chunk name in every
    # prototype header and FNEW annotation.
    paths = []
    for sub, data in (('_a', out), ('_b', MAR_MAGIC + std[4:])):
        os.makedirs(os.path.join(tmpdir, sub), exist_ok=True)
        paths.append(os.path.join(tmpdir, sub, 'chunk.luac'))
        open(paths[-1], 'wb').write(data)
    run = lambda p: subprocess.run([LUAJIT, '-bl', p], capture_output=True, cwd=os.path.dirname(LUAJIT),
                                   check=True).stdout.decode('latin1').splitlines()
    la, lb = run(paths[0]), run(paths[1])
    for p in paths:
        os.remove(p)
        os.rmdir(os.path.dirname(p))
    if len(la) != len(lb):
        return len(la), -1, False
    # A decoded jump (ISNEXT) marks its target line with "=>"; strip jump-target
    # markers before comparing. The jump itself still differs on the FIX line.
    norm = lambda s: re.sub(r'^(\d+) => ', lambda m: m.group(1) + '    ', s)
    diff = [(x, y) for x, y in zip(map(norm, la), map(norm, lb)) if x != y]
    op = lambda s: (s.split()[1:2] or [''])[0]
    ok = all(op(x) in fixnames and op(y) in fixnames for x, y in diff)
    return len(la), len(diff), ok


def main(argv):
    check_only = '--check-only' in argv
    argv = [a for a in argv if a != '--check-only']
    outdir = None if check_only else argv.pop(0)
    files = june_files(argv)
    table, seen = build_table(files)
    n, names = check_modes(table)
    print('table: %d KOG opcodes mapped (%d observed in %d files), injective, '
          'all %d consistent with June lj_bc_mode[]' % (len(table), len(seen), len(files), n))
    print('  ' + ' '.join('%02x>%s' % (k, names[s]) for k, s in sorted(table.items())))
    if check_only:
        return 0
    os.makedirs(outdir, exist_ok=True)
    for f in files:
        d = open(f, 'rb').read()
        out, nins, fixhits = transcode(d, table)
        std = open(f[:-4] + '.std.luac', 'rb').read()
        same = out[4:] == std[4:]
        open(os.path.join(outdir, os.path.basename(f)), 'wb').write(out)
        print('%-32s %8d instructions, %5d in FIX slots, %s' % (
            os.path.basename(f), nins, fixhits,
            'byte-identical to .std.luac past the header' if same else
            'differs from .std.luac at %d opcode byte(s)' % sum(1 for a, b in zip(out[4:], std[4:]) if a != b)))
        if same != (fixhits == 0):
            raise SystemExit('%s: identity with .std.luac does not match FIX-slot count' % f)
        nl, nd, ok = disasm_check(out, std, fixhits, outdir)
        print('%-32s luajit -bl: %d lines, %d differ from .std.luac%s' % (
            '', nl, nd, ' (all FIX slots, as expected)' if ok and nd == fixhits else ''))
        if not ok or nd != fixhits:
            raise SystemExit('%s: disassembly differs from .std.luac outside FIX slots' % f)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
