"""Read a CrashRpt minidump from X2_offline.exe without a debugger.

Usage:
    python tools/mdscan.py <X2_offline.dmp> <X2/US_SERVICE/X2.map> [--all] [--depth BYTES]

Where the dump comes from: a client crash leaves %TEMP%\\X2_<hex>.tmp, a zip holding
X2_offline.dmp, X2_offline.xml and that session's logs (the bare X2_offline.dmp beside it is
often 0 bytes). Unzip it first.

Prints the exception code, address and registers, resolves EIP, then scans the faulting
thread's stack for dwords inside .text and resolves each against the linker map. By default only
values preceded by a call instruction are listed (likely return addresses); --all lists every
.text value. CrashRpt's dumps usually hold no code pages, and then the call check cannot run:
those values are listed with a '?' (stale stack slots included), so read the chain top-down.

Check first that the map belongs to the crashed exe: the map's "Timestamp is" line must equal the
exe's PE TimeDateStamp. The preferred load address and .text extent are read from the map itself.
"""
import argparse
import bisect
import re
import struct


def load_map(path):
    base = None
    text_start = text_len = None
    rows = []
    for line in open(path, encoding='latin-1'):
        m = re.match(r'\s*Preferred load address is ([0-9a-f]+)', line)
        if m:
            base = int(m.group(1), 16)
            continue
        m = re.match(r'\s*0001:([0-9a-f]{8})\s+([0-9a-f]{8})H\s+\.text\b', line)
        if m and text_start is None:
            text_start, text_len = int(m.group(1), 16), int(m.group(2), 16)
            continue
        m = re.match(r'\s*0001:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s+(?:f\s+)?(?:i\s+)?(\S+)', line)
        if m:
            rows.append((int(m.group(2), 16), m.group(1), m.group(3)))
    rows.sort()
    if base is None or text_len is None:
        raise SystemExit('map: could not find the load address or the .text extent')
    lo = base + 0x1000 + text_start
    return rows, lo, lo + text_len


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('dmp')
    ap.add_argument('map')
    ap.add_argument('--all', action='store_true', help='list every .text value, not only call sites')
    ap.add_argument('--depth', type=lambda v: int(v, 0), default=0x8000, help='stack bytes to scan')
    ap.add_argument('--max', type=int, default=80, help='stop after this many frames')
    args = ap.parse_args()

    d = open(args.dmp, 'rb').read()
    if d[:4] != b'MDMP':
        raise SystemExit('not a minidump')
    nstreams, dir_rva = struct.unpack_from('<II', d, 8)
    streams = {}
    for i in range(nstreams):
        stype, size, rva = struct.unpack_from('<III', d, dir_rva + 12 * i)
        streams.setdefault(stype, []).append((size, rva))

    # MINIDUMP_EXCEPTION_STREAM (type 6)
    _, r = streams[6][0]
    tid, = struct.unpack_from('<I', d, r)
    code, _flags, _rec, addr = struct.unpack_from('<IIQQ', d, r + 8)
    nparams, = struct.unpack_from('<I', d, r + 8 + 24)
    params = struct.unpack_from('<15Q', d, r + 8 + 32)[:nparams]
    ctx_size, ctx_rva = struct.unpack_from('<II', d, r + 8 + 152)
    ctx = d[ctx_rva:ctx_rva + ctx_size]
    names = ['edi', 'esi', 'ebx', 'edx', 'ecx', 'eax', 'ebp', 'eip', 'cs', 'eflags', 'esp']
    regs = dict(zip(names, struct.unpack_from('<11I', ctx, 0x9c)))  # x86 CONTEXT
    print('thread %d  code 0x%08x  address 0x%08x  params %s' % (tid, code, addr, [hex(p) for p in params]))
    print('  '.join('%s=%08x' % (k, regs[k]) for k in names if k not in ('cs', 'eflags')))

    # memory: the memory list (type 5) plus every thread's stack (type 3)
    mem = []
    for _, r in streams.get(5, []):
        cnt, = struct.unpack_from('<I', d, r)
        for i in range(cnt):
            start, size, rva = struct.unpack_from('<QII', d, r + 4 + 16 * i)
            mem.append((start, d[rva:rva + size]))
    for _, r in streams.get(3, []):
        cnt, = struct.unpack_from('<I', d, r)
        for i in range(cnt):
            off = r + 4 + 48 * i
            start, size, rva = struct.unpack_from('<QII', d, off + 24)
            mem.append((start, d[rva:rva + size]))

    def read(a, k):
        for s, b in mem:
            if s <= a and a + k <= s + len(b):
                return b[a - s:a - s + k]
        return None

    rows, lo, hi = load_map(args.map)
    keys = [x[0] for x in rows]

    def sym(a):
        i = bisect.bisect_right(keys, a) - 1
        if i < 0:
            return '?'
        return '%s+0x%x [%s]' % (rows[i][1], a - rows[i][0], rows[i][2])

    def after_call(v):
        # A normal minidump does not capture code pages; with no bytes to check, keep the value.
        if read(v - 6, 6) is None:
            return None
        b5, b6 = read(v - 5, 5), read(v - 6, 6)
        b2, b3 = read(v - 2, 2), read(v - 3, 3)
        return bool((b5 and b5[0] == 0xE8)                                   # call rel32
                    or (b6 and b6[0] == 0xFF and (b6[1] & 0x38) == 0x10)     # call [mem]
                    or (b3 and b3[0] == 0xFF and (b3[1] & 0x38) == 0x10)     # call [reg+disp8]
                    or (b2 and b2[0] == 0xFF and (b2[1] & 0x38) == 0x10))    # call reg / [reg]

    print('EIP  %s' % sym(regs['eip']))
    shown = 0
    for off in range(0, args.depth, 4):
        b = read(regs['esp'] + off, 4)
        if b is None:
            continue
        v, = struct.unpack('<I', b)
        if not lo <= v < hi:
            continue
        verdict = True if args.all else after_call(v)
        if verdict is not False:
            print('esp+%04x  %08x %s %s' % (off, v, '?' if verdict is None else ' ', sym(v)))
            shown += 1
            if shown >= args.max:
                break


if __name__ == '__main__':
    main()
