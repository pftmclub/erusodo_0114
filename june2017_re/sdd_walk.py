"""Phase 8a: walk June's SetDamageDataFromLUA (32632390) key by key.

Usage: python sdd_walk.py [--all] [KEY ...]  -> per-key blocks on stdout
       python sdd_walk.py --order            -> the ordered key list (TSV) with VA ranges

A "block" is one key's read: from the XOR guard that precedes its decrypt, through the
getter call and default store, up to the next key's guard. The decrypt noise (buffer byte
loads, XOR/NOT, stack-slot stores, guard test/or, init stores into the 0x337a.... statics)
is filtered out, so what is printed is: the destination argument, the getter, the default
store, and anything else -- post-processing, conditions, calls -- that sits between this
key and the next. Keys come from xorstrings.tsv (ok rows only, plan rule 3); plaintext
sub-table names are shown where the function pushes them.
"""
import capstone, sys
from capstone.x86 import X86_OP_MEM, X86_OP_IMM, X86_OP_REG

EXE = r"F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe"
BASE, TR, TV = 0x31aa0000, 0x400, 0x1000
FUNC, FEND = 0x32632390, 0x3264063d
GETTERS = {0x32fea370: 'float', 0x32fe9e40: 'bool', 0x32fe9fd0: 'int', 0x32fea210: 'int(round)',
           0x32feae90: 'string'}
KNOWN_CALLS = {0x32fe9940: 'BeginTable', 0x32fe9a10: 'EndTable', 0x31c7b980: 'BeginTable(name,idx)',
               0x32630910: 'ParsingBuffFactor', 0x32630d20: 'ParsingBuffFactorID'}
STATIC_LO, STATIC_HI = 0x33790000, 0x337c0000


def load():
    data = open(EXE, 'rb').read()
    lo = TR + (FUNC - BASE - TV)
    hi = TR + (FEND - BASE - TV)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    return data, list(md.disasm(data[lo:hi], FUNC))


def cstring(data, va):
    """Plaintext ASCII at a VA in the image (rdata lives past .text; map by VA-BASE-0xc00)."""
    off = va - BASE - 0xc00
    if off < 0 or off >= len(data):
        return None
    s = data[off:off + 80].split(b'\0')[0]
    if len(s) >= 2 and all(0x20 <= c < 0x7f for c in s):
        return s.decode()
    return None


def keys():
    rows = []
    for line in open('xorstrings.tsv', encoding='latin1'):
        p = line.rstrip('\n').split('\t')
        if len(p) >= 7 and p[1] == '32632390' and p[6] == 'ok':
            rows.append((int(p[0], 16), p[5], p[8] if len(p) > 8 else '-'))
    return sorted(rows)


PARENT = {}
for _r in ('eax', 'ebx', 'ecx', 'edx'):
    _b = _r[1]
    for _n in (_r, _b + 'x', _b + 'l', _b + 'h'):
        PARENT[_n] = _r


def is_static(o):
    return o.type == X86_OP_MEM and o.mem.base == 0 and o.mem.index == 0         and STATIC_LO <= (o.mem.disp & 0xffffffff) < STATIC_HI


def is_noise(x, taint):
    """Decrypt-idiom noise. `taint` = registers currently holding a byte loaded from an
    encrypted static (only XOR/NOT keep it). A byte store to the stack is noise only when
    its source is tainted, so real temp/default stores (`mov byte [esp+0x1d], bl`) show."""
    m = x.mnemonic
    ops = x.operands
    reg = lambda o: x.reg_name(o.reg)
    if m in ('mov', 'movzx') and len(ops) == 2 and ops[0].type == X86_OP_REG and is_static(ops[1]):
        taint.add(PARENT.get(reg(ops[0]), reg(ops[0])))
        return True
    if m in ('xor', 'not') and ops[0].type == X86_OP_REG and PARENT.get(reg(ops[0])) in taint             and (len(ops) == 1 or ops[1].type in (X86_OP_IMM, X86_OP_MEM)):
        return True
    if m == 'mov' and len(ops) == 2 and ops[0].type == X86_OP_MEM and ops[0].size <= 2             and ops[0].mem.base == capstone.x86.X86_REG_ESP and ops[1].type == X86_OP_REG             and PARENT.get(reg(ops[1])) in taint:
        return True
    # any other write to a register clears its taint
    if ops and ops[0].type == X86_OP_REG and m not in ('push', 'cmp', 'test'):
        taint.discard(PARENT.get(reg(ops[0]), reg(ops[0])))
    if any(is_static(o) for o in ops) and m in ('mov', 'test', 'or'):
        return True      # guard test/or, init stores into the encrypted statics
    return False


def main():
    data, ins = load()
    idx = {x.address: i for i, x in enumerate(ins)}
    ks = keys()
    # guard start of each key: the nearest preceding 'test [static],imm' within 0x80 insns
    starts = []
    for u, name, guard in ks:
        i = idx.get(u)
        if i is None:  # use_va may be mid-instruction-stream; find nearest
            i = min(idx.values(), key=lambda j: abs(ins[j].address - u))
        g = i
        for j in range(i, max(0, i - 60), -1):
            x = ins[j]
            if x.mnemonic == 'test' and x.operands[0].type == X86_OP_MEM and x.operands[0].mem.base == 0:
                g = j
                break
        starts.append(g)
    blocks = []
    for n, (u, name, guard) in enumerate(ks):
        a = starts[n]
        b = starts[n + 1] if n + 1 < len(ks) else len(ins)
        # the getter call: first call to a known getter at or after the decrypt site
        ci = None
        for j in range(idx.get(u, a), b):
            if ins[j].mnemonic == 'call' and ins[j].operands[0].type == X86_OP_IMM and ins[j].operands[0].imm in GETTERS:
                ci = j
                break
        blocks.append((name, a, b, ci, u))
    return data, ins, blocks


def boring_pre(x):
    """--compact: pre-getter instructions that are only argument setup for the getter or the
    guard's own branches. The destination LEA on EBP/EDI is kept."""
    m, ops = x.mnemonic, x.operands
    if m in ('push', 'jne', 'jmp'):
        return True
    if m == 'mov' and len(ops) == 2 and ops[0].type == X86_OP_REG and ops[0].size == 1 and ops[1].type == X86_OP_IMM:
        return True      # init preload
    if m == 'mov' and x.op_str == 'ecx, esi':
        return True
    if m == 'lea' and ops[1].mem.base == capstone.x86.X86_REG_ESP and ops[1].mem.disp >= 0x200:
        return True      # the key-name stack buffer
    if m == 'mov' and ops[0].type == X86_OP_MEM and ops[0].size == 1 and ops[0].mem.base == capstone.x86.X86_REG_ESP             and ops[0].mem.disp >= 0x200:
        return True      # terminator into the key-name buffer
    if m == 'xor' and len(ops) == 2 and ops[1].type == X86_OP_REG and ops[0].reg == ops[1].reg:
        return True      # zero register for the terminator
    return False


def fmt(data, x):
    s = '%08x: %s %s' % (x.address, x.mnemonic, x.op_str)
    if x.mnemonic in ('call', 'jmp') and x.operands and x.operands[0].type == X86_OP_IMM:
        t = x.operands[0].imm
        if t in GETTERS:
            s += '    ; GETTER ' + GETTERS[t]
        elif t in KNOWN_CALLS:
            s += '    ; ' + KNOWN_CALLS[t]
    if x.mnemonic == 'push' and x.operands[0].type == X86_OP_IMM:
        c = cstring(data, x.operands[0].imm)
        if c:
            s += '    ; "%s"' % c
    for o in x.operands:
        if o.type == X86_OP_MEM and o.mem.base == 0 and o.mem.index == 0 and o.size in (4, 8) and x.mnemonic.startswith('f'):
            off = (o.mem.disp & 0xffffffff) - BASE - 0xc00
            import struct
            if 0 <= off < len(data) - 8:
                v = struct.unpack('<f' if o.size == 4 else '<d', data[off:off + o.size])[0]
                s += '    ; =%r' % v
    return s


if __name__ == '__main__':
    data, ins, blocks = main()
    args = sys.argv[1:]
    if '--order' in args:
        print('n\tkey\tguard_va\tdecrypt_va\tgetter_call_va\tgetter\tblock_end_va')
        for n, (name, a, b, ci, u) in enumerate(blocks):
            g = GETTERS.get(ins[ci].operands[0].imm) if ci is not None else '-'
            print('%d\t%s\t%08x\t%08x\t%s\t%s\t%08x' % (n + 1, name, ins[a].address, u,
                  '%08x' % ins[ci].address if ci is not None else '-', g,
                  ins[b].address if b < len(ins) else FEND))
        sys.exit(0)
    want = set(a for a in args if not a.startswith('--'))
    for name, a, b, ci, u in blocks:
        if want and name not in want and '--all' not in args:
            continue
        print('=== %s  [%08x-%08x)' % (name, ins[a].address, ins[b].address if b < len(ins) else FEND))
        taint = set()
        compact = '--compact' in args
        for j in range(a, b):
            x = ins[j]
            if is_noise(x, taint):
                continue
            if compact and ci is not None and j < ci and boring_pre(x):
                continue
            print('   ' + fmt(data, x))
