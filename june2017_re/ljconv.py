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
import csv
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


def instruction_offsets_tolerant(d, magic):
    """Like instruction_offsets, but never raises: a file cut short anywhere in
    the dump (header, a length prefix, a prototype's own header fields, its
    bytecode, or its tail of upvalues/constants/debug info) is reported instead
    of crashing on an IndexError from uleb().

    Returns a dict:
      status: 'complete' | 'no-terminator' | 'trailing-bytes' |
              'short-prototype-tail' | 'short-prototype-bytecode' |
              'cut-in-prototype-header' | 'cut-mid-length' | 'bad-prototype' |
              'bad-header' | 'big-endian'
      offs: instruction opcode-byte offsets for every FULLY present prototype,
            plus (for 'short-prototype-tail' only) the partial trailing
            prototype's own instructions, since its bytecode region sits right
            after its header fields and is confirmed to fit inside the file
            even though something after it (constants/debug info) is missing.
      protos_complete: count of prototypes walked with their full length prefix
            intact (does not count a 'short-prototype-tail' partial one).
      last_complete_end: file offset just past the last such prototype (or
            past the header if none), i.e. where the walk gave up.
      partial: None, or {'index', 'offs', 'declared_end', 'bc_end'} for the
            'short-prototype-tail' case.

    'no-terminator' means the walk reached exactly the position where the
    terminator's zero-length prefix would start, and the file ends there: a
    clean prototype boundary at EOF. On its own this does not prove only the
    1-byte terminator is missing -- a cut that dropped several whole trailing
    prototypes (main chunk included) plus the terminator looks identical.
    Callers must additionally check the archive manifest size (short by
    exactly 1 byte) before calling a 'no-terminator' file terminator-only."""
    if d[:4] != magic:
        return {'status': 'bad-header', 'offs': [], 'protos_complete': 0,
                'last_complete_end': 0, 'partial': None}
    try:
        flags, p = uleb(d, 4)
    except IndexError:
        return {'status': 'bad-header', 'offs': [], 'protos_complete': 0,
                'last_complete_end': 4, 'partial': None}
    if flags & BCDUMP_F_BE:
        return {'status': 'big-endian', 'offs': [], 'protos_complete': 0,
                'last_complete_end': p, 'partial': None}
    if not flags & BCDUMP_F_STRIP:
        try:
            n, p = uleb(d, p)
            p += n
            if p > len(d):
                raise IndexError
        except IndexError:
            return {'status': 'cut-in-prototype-header', 'offs': [], 'protos_complete': 0,
                    'last_complete_end': p, 'partial': None}
    offs = []
    idx = 0
    while True:
        start = p
        try:
            plen, p = uleb(d, p)
        except IndexError:
            status = 'no-terminator' if start == len(d) else 'cut-mid-length'
            return {'status': status, 'offs': offs, 'protos_complete': idx,
                    'last_complete_end': start, 'partial': None}
        if plen == 0:
            status = 'complete' if p == len(d) else 'trailing-bytes'
            return {'status': status, 'offs': offs, 'protos_complete': idx,
                    'last_complete_end': p, 'partial': None}
        end = p + plen
        try:
            q = p + 4
            if q > len(d):
                raise IndexError
            sizekgc, q = uleb(d, q)
            sizekn, q = uleb(d, q)
            sizebc, q = uleb(d, q)
            if not flags & BCDUMP_F_STRIP:
                sizedbg, q = uleb(d, q)
                if sizedbg:
                    _, q = uleb(d, q)
                    _, q = uleb(d, q)
        except IndexError:
            return {'status': 'cut-in-prototype-header', 'offs': offs, 'protos_complete': idx,
                    'last_complete_end': start, 'partial': None}
        bc_end = q + 4 * sizebc
        if end > len(d):
            if bc_end <= len(d):
                partial = {'index': idx, 'offs': list(range(q, bc_end, 4)),
                           'declared_end': end, 'bc_end': bc_end}
                return {'status': 'short-prototype-tail', 'offs': offs, 'protos_complete': idx,
                        'last_complete_end': start, 'partial': partial}
            return {'status': 'short-prototype-bytecode', 'offs': offs, 'protos_complete': idx,
                    'last_complete_end': start, 'partial': None}
        if bc_end > end:
            return {'status': 'bad-prototype', 'offs': offs, 'protos_complete': idx,
                    'last_complete_end': start, 'partial': None}
        offs.extend(range(q, bc_end, 4))
        p = end
        idx += 1


def june_files(args):
    return args or sorted(f for f in glob.glob(os.path.join(HERE, 'june_lua', '*.lua'))
                          if not f.endswith('.src.lua'))


def build_table(files):
    """KOG opcode -> stock opcode. Returns (table, per-op evidence counts,
    evidence) where evidence[k] is 'FIX' for the 9 hard-coded slots, or the
    basename of the first (.lua, .std.luac) pair that established it."""
    table = {}
    seen = {}
    evidence = {}
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
            evidence.setdefault(k, os.path.basename(f))
    for k, s in FIX.items():
        table[k] = s
        evidence[k] = 'FIX'
    inv = {}
    for k, s in table.items():
        if s in inv and inv[s] != k:
            raise SystemExit('not injective: KOG 0x%02x and 0x%02x both map to stock 0x%02x' % (inv[s], k, s))
        inv[s] = k
    return table, seen, evidence


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


def transcode_stock(d, table):
    """Like transcode(), but to STOCK_MAGIC (1b 4c 4a 01) instead of MAR_MAGIC:
    a June file straight to the header+opcodes ljd's own parser expects, with
    no elstools .std.luac needed at all. Same renumbering; only the header's
    3rd/4th bytes differ (mar-2014 keeps KOG's 'K','L' and 0x81; this is
    stock's 'L','J' and version 0x01 -- see lj_bcdump.h)."""
    offs = instruction_offsets(d, JUNE_MAGIC)
    out = bytearray(d)
    out[:4] = STD_MAGIC
    fixhits = 0
    for o in offs:
        k = d[o]
        if k not in table:
            raise ValueError('KOG opcode 0x%02x at 0x%x has no mapping' % (k, o))
        fixhits += k in FIX
        out[o] = table[k]
    instruction_offsets(bytes(out), STD_MAGIC)  # still walks cleanly
    return bytes(out), len(offs), fixhits


def verify_stock_mode(files, table):
    """The plan's Phase 0 exit check for the no-sibling mode: for every (june
    .lua, .std.luac) pair, transcode_stock's output must be byte-identical to
    the existing .std.luac EVERYWHERE except at FIX-slot instruction bytes --
    and every FIX-slot instruction must actually differ (elstools' own table is
    wrong there by construction, so a FIX slot that doesn't differ is itself a
    bug: either FIX is stale or the pair's evidence disagrees with it). Set
    equality, not subset: too few or too many differing bytes both fail.
    Returns the number of pairs checked."""
    n = 0
    for f in files:
        kog = open(f, 'rb').read()
        std = open(f[:-4] + '.std.luac', 'rb').read()
        if len(kog) != len(std):
            raise SystemExit('%s: length differs from its .std.luac' % f)
        offs = instruction_offsets(kog, JUNE_MAGIC)
        if instruction_offsets(std, STD_MAGIC) != offs:
            raise SystemExit('%s: .std.luac has a different instruction layout' % f)
        out, _, _ = transcode_stock(kog, table)
        fixoffs = set(o for o in offs if kog[o] in FIX)
        diffoffs = set(i for i in range(4, len(out)) if out[i] != std[i])
        if diffoffs != fixoffs:
            raise SystemExit('%s: differing bytes (%d) do not equal FIX-slot offsets (%d); '
                             'extra=%d missing=%d' % (f, len(diffoffs), len(fixoffs),
                                                       len(diffoffs - fixoffs), len(fixoffs - diffoffs)))
        n += 1
    return n


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


JUNE_DATA = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data'
TABLE_TSV = os.path.join(HERE, 'ljconv_table.tsv')


def find_pairs(roots):
    """Every (kog .lua, its .std.luac sibling) found recursively under roots."""
    pairs = []
    for root in roots:
        for dirpath, _, filenames in os.walk(root):
            for f in filenames:
                if f.lower().endswith('.std.luac'):
                    stem = os.path.join(dirpath, f[:-len('.std.luac')])
                    if os.path.exists(stem + '.lua'):
                        pairs.append(stem + '.lua')
    return sorted(pairs)


def save_table(table, names, evidence, path):
    with open(path, 'w', newline='', encoding='utf-8') as fh:
        w = csv.writer(fh, delimiter='\t')
        w.writerow(['kog_opcode_hex', 'stock_opcode_hex', 'stock_name', 'is_fix', 'evidence'])
        for k in sorted(table):
            w.writerow(['%02x' % k, '%02x' % table[k], names[table[k]], 'yes' if k in FIX else 'no',
                       evidence.get(k, '')])


def load_table(path):
    table = {}
    with open(path, encoding='utf-8') as fh:
        r = csv.DictReader(fh, delimiter='\t')
        for row in r:
            table[int(row['kog_opcode_hex'], 16)] = int(row['stock_opcode_hex'], 16)
    return table


def main(argv):
    if '--build-table' in argv:
        force = '--force' in argv
        argv = [a for a in argv if a not in ('--build-table', '--force')]
        roots = argv or [JUNE_DATA]
        pairs = find_pairs(roots)
        print('found %d (.lua, .std.luac) pairs under %s' % (len(pairs), ', '.join(roots)))
        table, seen, evidence = build_table(pairs)
        n, names = check_modes(table)
        print('table: %d KOG opcodes mapped (%d observed), all %d consistent with June lj_bc_mode[]'
              % (len(table), len(seen), n))
        if os.path.exists(TABLE_TSV) and not force:
            old = load_table(TABLE_TSV)
            lost = sorted(set(old) - set(table))
            if lost:
                raise SystemExit(
                    '%s already maps %d opcode(s) this run has no evidence for (%s); '
                    'refusing to shrink the table -- pass --force if that is really '
                    'intended (e.g. the roots deliberately exclude tmp/opsamples-style '
                    'one-off evidence files)' % (TABLE_TSV, len(lost),
                                                 ' '.join('0x%02x' % k for k in lost)))
        save_table(table, names, evidence, TABLE_TSV)
        print('wrote %s' % TABLE_TSV)
        return 0
    if '--verify-stock' in argv:
        argv = [a for a in argv if a != '--verify-stock']
        roots = argv or [JUNE_DATA]
        table = load_table(TABLE_TSV)
        pairs = find_pairs(roots)
        n = verify_stock_mode(pairs, table)
        print('verify-stock: %d pair(s) OK (differ from .std.luac only at FIX-slot instructions)' % n)
        return 0
    if '--stock' in argv:
        argv = [a for a in argv if a != '--stock']
        outdir = argv.pop(0)
        files = june_files(argv)
        table = load_table(TABLE_TSV)
        os.makedirs(outdir, exist_ok=True)
        for f in files:
            d = open(f, 'rb').read()
            out, nins, fixhits = transcode_stock(d, table)
            open(os.path.join(outdir, os.path.basename(f)), 'wb').write(out)
            print('%-32s %8d instructions, %5d in FIX slots -> stock bytecode, no .std.luac needed'
                  % (os.path.basename(f), nins, fixhits))
        return 0

    check_only = '--check-only' in argv
    argv = [a for a in argv if a != '--check-only']
    outdir = None if check_only else argv.pop(0)
    files = june_files(argv)
    table, seen, _ = build_table(files)
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
