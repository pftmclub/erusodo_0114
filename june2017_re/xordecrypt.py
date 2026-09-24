"""Raw-byte decoder for the June2017 client's compile-time XOR strings.

Replaces the prototypes (xordec*.py / xorscan*.py). Works on unpacked_x2.exe's
bytes with capstone, not on Ghidra's text. See README.md "The obstacle" for the
idiom this emulates, and "xordecrypt.py" for how to read its output.

    python xordecrypt.py 0x31f7aab0 0x321e48e0 ...    # functions (entry VAs)
    python xordecrypt.py --range 0x31f50000 0x31f90000
    python xordecrypt.py --all                         # every function in funcs.txt
    options: -o out.tsv (default xorstrings.tsv), --no-controls

Model, per function, in address order:
  * A guard is `TEST [flag],mask` + `OR [flag],mask`, the mask an immediate or a
    register value. Guards are keyed on (flag dword, bit mask): MSVC packs many
    static-init bits into one dword.
  * The init block is the run of stores into the static area after the OR, up to
    the first branch/call/ret (it can interleave with loads of other statics,
    so a load does not end it). Stores may be immediates or
    registers; register bytes are tracked from `MOV r,imm` (and XOR/NOT of
    constants). Each contiguous run of init addresses is one BUFFER, so two
    strings never merge.
  * Decryption loads buffer bytes into registers (any width), XORs/NOTs them with
    constants, and stores them to a stack slot. Each register byte is tracked as
    "buffer byte a XOR k", so load order, interleaving and register reuse do not
    matter: plaintext[i] = init[buf+i] ^ key[buf+i].
  * Stack slots are normalised for PUSH/POP/ADD/SUB ESP (a CALL starts a new
    epoch). They are used only for the invariant checks, not for ordering.

Per-buffer checks (the `status` column; 'ok' only if all pass):
  noinit     a buffer byte is decrypted but no guarded init store wrote it
  initunk    an init store's value is unknown (register not tracked)
  nokey:N    N init bytes are never decrypted
  keyconf    one byte decrypted with two different keys at different sites
  slot       slot - offset is not constant, or not in one normalisable slot space
  noterm     no zero store at slot base+len (and no encrypted zero terminator)
Decoded length is the number of init bytes, minus an encrypted terminator if the
last byte decrypts to 0 (column `term`: reg / imm / enc / none).

Output TSV columns: use_va func_va buf_va kind len plaintext status term guard
nuses. plaintext escapes \\t \\n \\r \\\\ and non-printables as \\xNN (ascii) or
\\uNNNN (wide), so rows always split cleanly on tabs.
"""
import argparse
import multiprocessing
import os
import re
import sys

from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_GRP_JUMP, CS_GRP_CALL, CS_GRP_RET
from capstone.x86 import X86_OP_MEM, X86_OP_IMM, X86_OP_REG

HERE = os.path.dirname(os.path.abspath(__file__))
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
FUNCS = os.path.join(HERE, 'funcs.txt')
EXE_SIZE = 43801600
TEXT_VA, TEXT_RAW, TEXT_RAWSIZE, TEXT_END = 0x31aa1000, 0x400, 0x1cbba00, 0x337ba000
# Zero-filled tail of .text's virtual range (the merged .data/.bss): the buffers live here.
STATIC_LO, STATIC_HI = TEXT_VA + TEXT_RAWSIZE, TEXT_END

CONTROLS = [  # (buffer VA, exact plaintext, kind) -- README.md "Hand-verified controls"
    (0x3379c56c, 'MAIN_EFFECT_NAME%d', 'ascii'),
    (0x3379c2b8, 'RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT', 'ascii'),
    (0x3379b6e8, 'PVPEmblem_Season2.lua', 'wide'),  # FUN_31f57f10; both XOR forms, hand-verified
]

R32 = ['eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp']
SUB = {}
for r in 'abcd':
    SUB['e%sx' % r] = ('e%sx' % r, 0, 4)
    SUB['%sx' % r] = ('e%sx' % r, 0, 2)
    SUB['%sl' % r] = ('e%sx' % r, 0, 1)
    SUB['%sh' % r] = ('e%sx' % r, 1, 1)
for r in ('si', 'di', 'bp', 'sp'):
    SUB['e' + r] = ('e' + r, 0, 4)
    SUB[r] = ('e' + r, 0, 2)

_data = None
_md = None


def data():
    global _data
    if _data is None:
        _data = open(EXE, 'rb').read()
    return _data


def md():
    global _md
    if _md is None:
        _md = Cs(CS_ARCH_X86, CS_MODE_32)
        _md.detail = True
    return _md


def raw(va):
    return va - TEXT_VA + TEXT_RAW


def load_funcs():
    starts = set()
    for line in open(FUNCS, encoding='latin1'):
        m = re.match(r'(\S+) at ([0-9a-fA-F]+)', line)
        if m:
            starts.add(int(m.group(2), 16))
    return sorted(starts)


def func_end(starts, f):
    import bisect
    i = bisect.bisect_right(starts, f)
    return starts[i] if i < len(starts) else TEXT_VA + TEXT_RAWSIZE


def disasm(f, end):
    """Linear sweep (a generator: some Ghidra "functions" are megabytes of trailing
    data); on an undecodable byte, skip it and resume."""
    code = data()[raw(f):raw(min(end, TEXT_VA + TEXT_RAWSIZE))]
    off = 0
    while off < len(code):
        n = 0
        for x in md().disasm(code[off:off + 0x10000], f + off):
            yield x
            n = x.address + x.size - (f + off)
        off += n if n else 1


class StopSide(Exception):
    pass


def is_static(op):
    return (op.type == X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0
            and STATIC_LO <= (op.mem.disp & 0xffffffff) < STATIC_HI)


class Func:
    def __init__(self, f, end):
        self.f = f
        self.regs = {r: [None] * 4 for r in R32}
        self.epoch = 0
        self.delta = 0
        self.guards = {}      # (flagdword, mask) -> dict
        self.cur = None       # guard whose init block is open
        self.uses = {}        # buf byte addr -> list of (key, load_va, slot, width)
        self.consts = {}      # slot -> const byte stored there (for terminators)
        self.loads = {}       # buf byte addr -> first load VA
        self.stray = {}       # static stores outside any init block
        self.statics = 0      # guarded runs never decrypted (not strings)
        self.end = end
        self.in_side = False
        self.run(disasm(f, end))

    # --- register cells ---
    def get(self, name):
        base, o, n = SUB[name]
        return self.regs[base][o:o + n]

    def put(self, name, cells):
        base, o, n = SUB[name]
        self.regs[base][o:o + n] = list(cells)

    def kill(self, name):
        if name in SUB:
            base, o, n = SUB[name]
            self.regs[base] = [None] * 4 if n == 4 else self.regs[base][:o] + [None] * n + self.regs[base][o + n:]

    @staticmethod
    def imm_cells(v, n):
        return [('c', (v >> (8 * i)) & 0xff) for i in range(n)]

    def const_of(self, cells):
        if all(c is not None and c[0] == 'c' for c in cells):
            return sum(c[1] << (8 * i) for i, c in enumerate(cells))
        return None

    def src_cells(self, op, n):
        if op.type == X86_OP_IMM:
            return self.imm_cells(op.imm & 0xffffffff, n)
        if op.type == X86_OP_REG:
            return self.get(md().reg_name(op.reg))
        return [None] * n

    def slot(self, op):
        m = op.mem
        if m.index != 0:
            return None
        base = md().reg_name(m.base) if m.base else None
        if base == 'esp':
            return ('esp', self.epoch, m.disp + self.delta)
        if base == 'ebp':
            return ('ebp', 0, m.disp)
        return None

    def new_epoch(self):
        self.epoch += 1
        self.delta = 0

    # --- main loop ---
    def run(self, insns):
        for x in insns:
            try:
                self.step(x)
            except (KeyError, IndexError):
                self.generic(x)

    def close_init(self):
        self.cur = None

    def side(self, target):
        """Tail merge: the compiler XORs the last bytes into registers and then
        JMPs into a store sequence shared with another string (e.g. m_iTitle
        in FUN_326f8cd0 -> 326fb466). Follow the jump with a copy of the state,
        up to the next branch/call/ret, then restore the linear sweep's state."""
        if not (self.f <= target < self.end):
            return
        saved = ({r: list(v) for r, v in self.regs.items()}, self.epoch, self.delta, self.cur)
        self.side_pending = set(c[1] for r in R32 for c in self.regs[r]
                                if c is not None and c[0] == 'b' and c[1] not in self.uses)
        self.in_side = True
        try:
            for i, y in enumerate(disasm(target, min(self.end, target + 0x400))):
                if i >= 64:
                    break
                try:
                    self.step(y)
                except (KeyError, IndexError):
                    self.generic(y)
        except StopSide:
            pass
        finally:
            self.in_side = False
            self.regs, self.epoch, self.delta, self.cur = saved

    def step(self, x):
        mn = x.mnemonic
        ops = x.operands
        rn = md().reg_name

        if x.group(CS_GRP_JUMP) or x.group(CS_GRP_RET):
            if self.in_side:
                raise StopSide()
            self.close_init()
            # Only for bytes still pending: a register keeps its cell after the
            # store, and re-storing stale cells in the tail would corrupt slots.
            if mn == 'jmp' and ops and ops[0].type == X86_OP_IMM and any(
                    c is not None and c[0] == 'b' and c[1] not in self.uses
                    for r in R32 for c in self.regs[r]):
                self.side(ops[0].imm & 0xffffffff)
            if mn == 'jmp' or x.group(CS_GRP_RET):
                self.new_epoch()
            return
        if x.group(CS_GRP_CALL):
            if self.in_side:
                raise StopSide()
            self.close_init()
            for r in ('eax', 'ecx', 'edx'):
                self.regs[r] = [None] * 4
            self.new_epoch()
            return

        if mn == 'test' and len(ops) == 2 and is_static(ops[0]):
            return  # guards are recorded at the OR, which carries the same (flag, mask)

        if mn == 'or' and len(ops) == 2 and is_static(ops[0]):
            a = ops[0].mem.disp & 0xffffffff
            mask = self.const_of(self.src_cells(ops[1], ops[0].size))
            if mask is not None:
                key = (a & ~3, (mask << (8 * (a & 3))) & 0xffffffff)
                g = self.guards.setdefault(key, {'or_va': x.address, 'stores': {}})
                self.cur = g
                return
            self.close_init()
            return

        if mn == 'mov' and len(ops) == 2 and ops[0].type == X86_OP_MEM:
            n = ops[0].size
            cells = self.src_cells(ops[1], n)
            if is_static(ops[0]):
                a = ops[0].mem.disp & 0xffffffff
                target = self.cur['stores'] if self.cur is not None else self.stray
                for i in range(n):
                    target[a + i] = cells[i]
                return
            s = self.slot(ops[0])
            for i, c in enumerate(cells):
                sl = (s[0], s[1], s[2] + i) if s else None
                if c is not None and c[0] == 'b':
                    # In a side trace, record only the bytes that were pending at
                    # the JMP: the target may be the middle of another string's
                    # decrypt (the init path jumps in there), which the linear
                    # sweep will record itself.
                    if self.in_side and (c[1] not in self.side_pending or c[1] in self.uses):
                        continue
                    self.uses.setdefault(c[1], []).append((c[2], x.address, sl, n))
                elif c is not None and c[0] == 'c' and sl is not None:
                    self.consts[sl] = c[1]
            return

        if mn in ('mov', 'movzx', 'movsx') and len(ops) == 2 and ops[0].type == X86_OP_REG:
            dst = rn(ops[0].reg)
            dn = SUB[dst][2]
            if ops[1].type == X86_OP_MEM and is_static(ops[1]):
                a = ops[1].mem.disp & 0xffffffff
                sn = ops[1].size
                cells = [('b', a + i, 0) for i in range(sn)]
                for i in range(sn):
                    self.loads.setdefault(a + i, x.address)
                if dn > sn:
                    cells += [('c', 0)] * (dn - sn) if mn == 'movzx' else [None] * (dn - sn)
                self.put(dst, cells)
                return
            if mn == 'mov' and ops[1].type in (X86_OP_IMM, X86_OP_REG):
                self.put(dst, self.src_cells(ops[1], dn))
                return
            if mn == 'movzx' and ops[1].type == X86_OP_REG:
                src = self.get(rn(ops[1].reg))
                self.put(dst, src + [('c', 0)] * (dn - len(src)))
                return
            self.generic(x)
            return

        if mn == 'xor' and len(ops) == 2 and ops[0].type == X86_OP_REG:
            dst = rn(ops[0].reg)
            n = SUB[dst][2]
            if ops[1].type == X86_OP_REG and ops[1].reg == ops[0].reg:
                self.put(dst, [('c', 0)] * n)
                return
            if ops[1].type == X86_OP_MEM and is_static(ops[1]):
                # Reverse form: key already in the register, buffer as the memory
                # operand (`MOV ECX,0xe8a / XOR CX,word ptr [buf]`, wide strings).
                a = ops[1].mem.disp & 0xffffffff
                cells = []
                for i, c in enumerate(self.get(dst)):
                    self.loads.setdefault(a + i, x.address)
                    cells.append(('b', a + i, c[1]) if c is not None and c[0] == 'c' else None)
                self.put(dst, cells)
                return
            k = self.const_of(self.src_cells(ops[1], n))
            if k is None:
                self.put(dst, [None] * n)
                return
            self.put(dst, [self.xcell(c, (k >> (8 * i)) & 0xff) for i, c in enumerate(self.get(dst))])
            return

        if mn in ('or', 'and', 'add', 'sub') and len(ops) == 2 and ops[0].type == X86_OP_REG \
                and ops[1].type in (X86_OP_IMM, X86_OP_REG) and rn(ops[0].reg) != 'esp':
            # Constant folding. The compiler materialises an init byte 0xff as
            # `OR CL,0xff` (result independent of CL) and 0 as `AND r,0`.
            dst = rn(ops[0].reg)
            n = SUB[dst][2]
            mask = (1 << (8 * n)) - 1
            k = self.const_of(self.src_cells(ops[1], n))  # `OR CL,BL` with BL=0xff too
            if k is None:
                self.put(dst, [None] * n)
                return
            v = self.const_of(self.get(dst))
            if mn == 'or' and k == mask:
                r = mask
            elif mn == 'and' and k == 0:
                r = 0
            elif v is None:
                self.put(dst, [None] * n)
                return
            else:
                r = {'or': v | k, 'and': v & k, 'add': (v + k) & mask, 'sub': (v - k) & mask}[mn]
            self.put(dst, self.imm_cells(r, n))
            return

        if mn == 'not' and len(ops) == 1 and ops[0].type == X86_OP_REG:
            dst = rn(ops[0].reg)
            self.put(dst, [self.xcell(c, 0xff) for c in self.get(dst)])
            return

        if mn == 'lea' and len(ops) == 2 and ops[1].mem.index == 0 and ops[1].mem.disp == 0 \
                and ops[1].mem.base == ops[0].reg:
            return  # `LEA EBX,[EBX]`: multi-byte alignment NOP, writes nothing
        if mn == 'push':
            self.delta -= 4
            return
        if mn == 'pop':
            if ops and ops[0].type == X86_OP_REG:
                self.kill(rn(ops[0].reg))
            self.delta += 4
            return
        if mn in ('sub', 'add') and ops and ops[0].type == X86_OP_REG and rn(ops[0].reg) == 'esp':
            if ops[1].type == X86_OP_IMM:
                self.delta += -ops[1].imm if mn == 'sub' else ops[1].imm
            else:
                self.new_epoch()
            return
        self.generic(x)

    @staticmethod
    def xcell(c, k):
        if c is None:
            return None
        if c[0] == 'c':
            return ('c', c[1] ^ k)
        return ('b', c[1], c[2] ^ k)

    def generic(self, x):
        try:
            _, written = x.regs_access()
        except Exception:
            written = []
        for r in written:
            name = md().reg_name(r)
            if name == 'esp':
                self.new_epoch()
            elif name in SUB:
                self.kill(name)

    # --- buffers ---
    def buffers(self):
        rows = []
        owned = set()
        for (flag, mask), g in sorted(self.guards.items()):
            addrs = sorted(a for a in g['stores'] if a not in (flag, flag + 1, flag + 2, flag + 3))
            runs = []
            for a in addrs:
                if runs and a == runs[-1][-1] + 1:
                    runs[-1].append(a)
                else:
                    runs.append([a])
            if not runs:
                # Guard whose init stores nothing static before its first call: a
                # static object (e.g. 3379b854 in 31f6ac60: `PUSH buf; CALL ...` then
                # field stores 0/0xb8/0x20). Not a string; counted so that every
                # guard is accounted for as a buffer or a static.
                self.statics += 1
            for run in runs:
                owned.update(run)
                if not any(a in self.uses for a in run):
                    # Guarded but never decrypted: an ordinary static local (MSVC
                    # uses the same guard idiom), not a string. Counted, not emitted.
                    self.statics += 1
                    continue
                rows.append(self.decode(run, g['stores'], '%08x:%x' % (flag, mask)))
        # Decrypted bytes whose buffer no guard in this function initialised.
        orphan = sorted(a for a in self.uses if a not in owned)
        runs = []
        for a in orphan:
            if runs and a == runs[-1][-1] + 1:
                runs[-1].append(a)
            else:
                runs.append([a])
        for run in runs:
            # An unguarded run copied with key 0 throughout is an ordinary static
            # variable (`MOV EAX,[g]; MOV [ESP+x],EAX`), not a string. Anything
            # actually XORed without a guard is kept and labelled.
            if all(u[0] == 0 for a in run for u in self.uses[a]):
                continue
            r = self.decode(run, self.stray, '-')
            r['status'] = 'orphan,' + r['status'] if r['status'] != 'ok' else 'orphan'
            rows.append(r)
        return rows

    def decode(self, run, stores, guard):
        buf = run[0]
        fails = []
        out = []
        slots = []
        width = 1
        nokey = 0
        for a in run:
            c = stores.get(a)
            uses = self.uses.get(a, [])
            keys = set(u[0] for u in uses)
            if c is None:
                if 'noinit' not in fails:
                    fails.append('noinit')
                out.append(None)
                continue
            if c[0] != 'c':
                if 'initunk' not in fails:
                    fails.append('initunk')
                out.append(None)
                continue
            if not keys:
                nokey += 1
                out.append(None)
                continue
            if len(keys) > 1 and 'keyconf' not in fails:
                fails.append('keyconf')
            u = uses[0]
            width = max(width, u[3])
            slots.append((u[2], a - buf))
            out.append(c[1] ^ u[0])
        if nokey:
            fails.append('nokey:%d' % nokey)
        n = len(run)
        kind = 'ascii'
        dec = [b for b in out if b is not None]
        if n >= 2 and n % 2 == 0 and len(dec) == n and all(out[i] == 0 for i in range(1, n, 2)) and width >= 2:
            kind = 'wide'
        term = 'none'
        length = n
        zero_w = 2 if kind == 'wide' else 1
        if len(dec) == n and all(b == 0 for b in out[n - zero_w:]):
            term, length = 'enc', n - zero_w
        # Slot invariant: every byte in one normalisable slot space, slot-offset constant.
        spaces = set((s[0], s[1]) for s, _ in slots if s is not None)
        bases = set(s[2] - off for s, off in slots if s is not None)
        if slots and (len(spaces) != 1 or len(bases) != 1 or any(s is None for s, _ in slots)):
            fails.append('slot')
        elif slots and term == 'none':
            sp = spaces.pop()
            base = bases.pop()
            zs = [self.consts.get((sp[0], sp[1], base + n + i)) for i in range(zero_w)]
            if all(z == 0 for z in zs):
                term = 'reg'
        if term == 'none':
            fails.append('noterm')
        body = out[:length]
        if any(b is None for b in body):
            text = ''.join('?' if b is None else chr(b) for b in body)
        elif kind == 'wide':
            text = bytes(body).decode('utf-16le', 'replace')
        else:
            text = bytes(body).decode('latin1')
        use = min((self.loads[a] for a in run if a in self.loads), default=0)
        nuses = max((len(self.uses.get(a, [])) for a in run), default=0)
        return {'use_va': use, 'func_va': self.f, 'buf_va': buf, 'kind': kind, 'len': length // (2 if kind == 'wide' else 1),
                'plaintext': text, 'status': ','.join(fails) or 'ok', 'term': term, 'guard': guard, 'nuses': nuses}


def esc(s, wide):
    o = []
    for ch in s:
        c = ord(ch)
        if ch == '\\':
            o.append('\\\\')
        elif ch == '\t':
            o.append('\\t')
        elif ch == '\n':
            o.append('\\n')
        elif ch == '\r':
            o.append('\\r')
        elif 0x20 <= c < 0x7f:
            o.append(ch)
        elif wide and c >= 0xa0:
            o.append(ch)
        else:
            o.append('\\u%04x' % c if wide else '\\x%02x' % c)
    return ''.join(o)


def decode_func(args):
    f, end = args
    try:
        fn = Func(f, end)
        rows = fn.buffers()
        return rows + [{'statics': fn.statics}] if fn.statics else rows
    except Exception as e:  # never silently drop a function
        return [{'use_va': 0, 'func_va': f, 'buf_va': 0, 'kind': '-', 'len': 0, 'plaintext': repr(e),
                 'status': 'crash', 'term': '-', 'guard': '-', 'nuses': 0}]


def funcs_touching(addr, starts):
    """Functions whose bytes contain the little-endian address (init or load site)."""
    d = data()
    pat = addr.to_bytes(4, 'little')
    hits = set()
    i = d.find(pat, TEXT_RAW, TEXT_RAW + TEXT_RAWSIZE)
    import bisect
    while i != -1:
        va = i - TEXT_RAW + TEXT_VA
        j = bisect.bisect_right(starts, va) - 1
        if j >= 0:
            hits.add(starts[j])
        i = d.find(pat, i + 1, TEXT_RAW + TEXT_RAWSIZE)
    return sorted(hits)


def run_controls(starts):
    ok = True
    for buf, want, kind in CONTROLS:
        rows = []
        for f in funcs_touching(buf, starts):
            rows += [r for r in decode_func((f, func_end(starts, f))) if r.get('buf_va') == buf]
        if len(rows) != 1:
            print('CONTROL %08x: expected 1 buffer row, got %d' % (buf, len(rows)), file=sys.stderr)
            ok = False
            continue
        r = rows[0]
        good = r['status'] == 'ok' and r['kind'] == kind and (want is None or r['plaintext'] == want)
        print('CONTROL %08x %s: %s %r len=%d status=%s term=%s func=%08x' % (
            buf, 'PASS' if good else 'FAIL', r['kind'], r['plaintext'], r['len'], r['status'], r['term'], r['func_va']),
            file=sys.stderr)
        ok &= good
    return ok


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('funcs', nargs='*')
    ap.add_argument('--all', action='store_true')
    ap.add_argument('--range', nargs=2)
    ap.add_argument('-o', default=os.path.join(HERE, 'xorstrings.tsv'))
    ap.add_argument('--no-controls', action='store_true')
    ap.add_argument('-j', type=int, default=os.cpu_count())
    a = ap.parse_args()

    if os.path.getsize(EXE) != EXE_SIZE:
        sys.exit('unexpected dump size; check README.md "The binary"')
    starts = load_funcs()
    if not a.no_controls and not run_controls(starts):
        sys.exit('controls failed; refusing to emit output')

    if a.all:
        todo = starts
    elif a.range:
        lo, hi = int(a.range[0], 16), int(a.range[1], 16)
        todo = [f for f in starts if lo <= f < hi]
    else:
        todo = [int(x, 16) for x in a.funcs]
    if not todo:
        return
    work = [(f, func_end(starts, f)) for f in todo]
    rows = []
    if len(work) > 8 and a.j > 1:
        with multiprocessing.Pool(a.j) as pool:
            for r in pool.imap(decode_func, work, chunksize=64):
                rows += r
    else:
        for w in work:
            rows += decode_func(w)
    statics = sum(r['statics'] for r in rows if 'statics' in r)
    rows = [r for r in rows if 'statics' not in r]
    rows.sort(key=lambda r: (r['func_va'], r['use_va'], r['buf_va']))
    cols = ['use_va', 'func_va', 'buf_va', 'kind', 'len', 'plaintext', 'status', 'term', 'guard', 'nuses']
    with open(a.o, 'w', encoding='utf-8', newline='\n') as fo:
        fo.write('\t'.join(cols) + '\n')
        for r in rows:
            fo.write('%08x\t%08x\t%08x\t%s\t%d\t%s\t%s\t%s\t%s\t%d\n' % (
                r['use_va'], r['func_va'], r['buf_va'], r['kind'], r['len'], esc(r['plaintext'], r['kind'] == 'wide'),
                r['status'], r['term'], r['guard'], r['nuses']))
    from collections import Counter
    st = Counter(r['status'].split(':')[0] if r['status'].startswith('nokey') else r['status'] for r in rows)
    print('%d functions, %d buffers -> %s; status: %s; %d guarded non-string statics skipped' % (
        len(work), len(rows), a.o, ', '.join('%s=%d' % kv for kv in st.most_common()), statics), file=sys.stderr)


if __name__ == '__main__':
    main()
