"""Verify ljconv's KOG June -> stock opcode table against the two VMs' own handler code (LUA_SOURCE_PHASES.md Phase L2, step 7).

ljconv_table.tsv is elstools' translation plus ljd_jun2017.FIX, and June-mode T1 uses the same table, so T1 cannot catch a
wrong entry. lj_bc_mode[] (ljconv's own assertion) cannot either: it only pins operand types, and a swap inside a family with
the same modes (ISLT/ISGE/ISLE/ISGT, the arithmetic VN/NV/VV groups, ...) passes it. This script is independent of both:

  March  x2.exe (US_SERVICE build, X2.map): handler(s) = _lj_vm_asm_begin + _lj_bc_ofs[s]   (stock order, the March VM)
  June   unpacked_x2.exe: lj_dispatch_init is FUN_32ffb270 (same shape as March's _lj_dispatch_init: the 0x145b hotcount, the
         0x5d..+0x3e loop, the 85 + 70 entry copies into GG->dispatch at +0xb38). June keeps absolute handler pointers instead
         of lj_bc_ofs: handler(k) = dword[0x337411d8 + 4k] (PTR_LAB_337411d8), KOG order. lj_vm_asm_begin = 0x32fee670.

Each handler runs from its start to the next handler start (both VMs emit build_ins in their own enum order). Instructions
are compared after normalising what legitimately differs between two links of the same VM source:
  - a branch/call target inside the handler -> its offset from the handler start;
  - inside the handler block -> (that handler's opcode in STOCK numbering, offset), June's via the table under test;
  - in the subroutine block that follows the handlers -> its offset from lj_vm_asm_begin (identical layout; checked);
  - anywhere else (C helpers) -> 'ext';
  - an 8-bit immediate is compared raw (a VM opcode immediate would show up as a mismatch, explained in the TSV).
A KOG opcode passes when its June handler equals exactly one March handler, and that handler is ljconv's mapping.

    python opcode_check.py            # writes lua_source_opcode_check.tsv next to this file
"""
import csv
import os
import re
import struct
import sys

import capstone

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)
sys.path.insert(0, HERE)
import ljconv  # noqa: E402
import lj_bcmode  # noqa: E402

JUNE_EXE = lj_bcmode.EXE
MARCH_EXE = os.path.join(TRUNK, "X2", "US_SERVICE", "x2.exe")
MARCH_MAP = os.path.join(TRUNK, "X2", "US_SERVICE", "X2.map")
JUNE_DISPATCH_INIT = 0x32FFB270
JUNE_PTRS = 0x337411D8
JUNE_ASM_BEGIN = 0x32FEE670
NOPS = 93                       # BC__MAX (stock LuaJIT 2.0; + GG_NUM_ASMFF 62 = the 155 dispatch entries)
NAMES = [s[0] for s in lj_bcmode.std]
OUT = os.path.join(HERE, "lua_source_opcode_check.tsv")


def sections(path):
    d = open(path, "rb").read()
    pe = struct.unpack_from("<I", d, 0x3C)[0]
    nsec = struct.unpack_from("<H", d, pe + 6)[0]
    optsz = struct.unpack_from("<H", d, pe + 20)[0]
    base = struct.unpack_from("<I", d, pe + 24 + 28)[0]
    secs = []
    for i in range(nsec):
        o = pe + 24 + optsz + 40 * i
        vsz, va, rsz, roff = struct.unpack_from("<IIII", d, o + 8)
        secs.append((base + va, rsz, roff))
    return d, secs


def va2off(secs, v):
    for va, rsz, roff in secs:
        if va <= v < va + rsz:
            return roff + v - va
    raise ValueError("VA 0x%x not in the file" % v)


def map_symbol(name):
    for ln in open(MARCH_MAP, encoding="latin-1"):
        w = ln.split()
        if len(w) >= 3 and w[1] == name:
            return int(w[2], 16)
    raise SystemExit("%s not in %s" % (name, MARCH_MAP))


class VM:
    def __init__(self, label, data, secs, starts, asm_begin, opname):
        self.label, self.data, self.secs, self.starts, self.asm_begin = label, data, secs, starts, asm_begin
        self.opname = opname                     # handler index -> stock opcode (identity for March, the table for June)
        order = sorted(set(starts))
        self.block_end = None
        self.next_start = {}
        for a, b in zip(order, order[1:]):
            self.next_start[a] = b
        self.by_addr = {}
        for k, a in enumerate(starts):
            self.by_addr.setdefault(a, []).append(k)
        self.md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        self.md.detail = True

    def extent(self, k):
        a = self.starts[k]
        return a, self.next_start.get(a)

    def where(self, target, a, b):
        if a <= target < b:
            return ("self", target - a)
        if min(self.by_addr) <= target < self.sub_lo:
            s = max(x for x in self.by_addr if x <= target)
            return ("handler", tuple(sorted(self.opname(k) for k in self.by_addr[s])), target - s)
        if self.sub_lo <= target < self.sub_hi:
            return ("sub", target - self.asm_begin)
        return ("ext",)

    def norm(self, k, depth=0):
        """Normalised instruction list of handler k. A handler that falls through (CALLMT/RETM, FORL/ITERL/LOOP: a one-line
        stub or the hot-counter macro, then the next handler's code) carries its successor's normalised code, so it is
        identified by what it falls into. A byte store into the bytecode stream ([esi..], the PC) writes an opcode
        number (ISNEXT despecialises to JMP/ITERC): its immediate is compared as the STOCK opcode it means."""
        a, b = self.extent(k)
        if b is None:
            return None
        code = self.data[va2off(self.secs, a):va2off(self.secs, a) + (b - a)]
        out, last = [], None
        for ins in self.md.disasm(code, a):
            g = set(ins.groups)
            ops = ins.operands
            if g & {capstone.CS_GRP_JUMP, capstone.CS_GRP_CALL} and ops and ops[0].type == capstone.x86.X86_OP_IMM:
                out.append((ins.mnemonic, self.where(ops[0].imm, a, b)))
            elif (ins.mnemonic == "mov" and len(ops) == 2 and ops[0].type == capstone.x86.X86_OP_MEM and ops[0].size == 1
                  and ops[0].mem.base == capstone.x86.X86_REG_ESI and ops[1].type == capstone.x86.X86_OP_IMM):
                out.append(("mov-opcode", ops[0].mem.disp, ("stock", self.opname(ops[1].imm & 0xFF))))
            else:
                out.append((ins.mnemonic, ins.op_str))
            last = ins
        if last is not None and last.mnemonic not in ("jmp", "ret") and depth < 3:
            nxt = self.by_addr.get(b)
            out.append(("fallthrough", tuple(self.norm(nxt[0], depth + 1) or ()) if nxt else ("sub", b - self.asm_begin)))
        return out


def load():
    jd, js = sections(JUNE_EXE)
    jptr = list(struct.unpack_from("<%dI" % NOPS, jd, va2off(js, JUNE_PTRS)))
    md_, ms = sections(MARCH_EXE)
    asm = map_symbol("_lj_vm_asm_begin")
    ofs_va = map_symbol("_lj_bc_ofs")
    mofs = struct.unpack_from("<%dH" % NOPS, md_, va2off(ms, ofs_va))
    mstart = [asm + o for o in mofs]
    table = ljconv.load_table(ljconv.TABLE_TSV)
    march = VM("March", md_, ms, mstart, asm, lambda k: k)
    june = VM("June", jd, js, jptr, JUNE_ASM_BEGIN, lambda k: table.get(k, -1))
    return march, june, table, asm, ofs_va


def main():
    march, june, table, asm, ofs_va = load()
    # last handler ends where the subroutines start: use March's _lj_vm_returnp as the anchor for both
    ret = map_symbol("_lj_vm_returnp")
    sub_ofs = ret - asm
    for vm in (march, june):
        last = max(vm.starts)
        vm.next_start[last] = vm.asm_begin + sub_ofs
        vm.sub_lo = vm.asm_begin + sub_ofs
        vm.sub_hi = vm.asm_begin + sub_ofs + 0x4000
    sub_same = march.data[va2off(march.secs, march.sub_lo):va2off(march.secs, march.sub_lo) + 64] == \
        june.data[va2off(june.secs, june.sub_lo):va2off(june.secs, june.sub_lo) + 64]
    mnorm = {s: march.norm(s) for s in range(NOPS)}
    rows, fails = [], 0
    for k in range(NOPS):
        jn = june.norm(k)
        exact = [s for s in range(NOPS) if mnorm[s] == jn]
        mapped = table.get(k)
        best = None
        if not exact:
            def sim(s):
                m = mnorm[s] or []
                return sum(1 for x, y in zip(m, jn or []) if x == y) - abs(len(m) - len(jn or []))
            best = max(range(NOPS), key=sim)
        if mapped is None:
            verdict = "not-in-ljconv" + (" (June handler = stock %s)" % "/".join(NAMES[s] for s in exact) if exact else "")
        elif exact == [mapped]:
            verdict = "VERIFIED"
        elif mapped in exact:
            verdict = "AMBIGUOUS (identical March handlers: %s)" % "/".join(NAMES[s] for s in exact)
        else:
            verdict = "MISMATCH"
            fails += 1
        rows.append({
            "kog_opcode_hex": "%02x" % k, "june_handler_va": "%08x" % june.starts[k],
            "june_handler_bytes": june.extent(k)[1] - june.extent(k)[0],
            "ljconv_stock": NAMES[mapped] if mapped is not None else "",
            "march_handler_va": "%08x" % march.starts[mapped] if mapped is not None else "",
            "march_handler_bytes": (march.extent(mapped)[1] - march.extent(mapped)[0]) if mapped is not None else "",
            "instructions": len(jn or []),
            "exact_march_matches": "/".join(NAMES[s] for s in exact) or "none (closest %s)" % NAMES[best],
            "verdict": verdict})
    cols = list(rows[0])
    with open(OUT, "w", encoding="utf-8", newline="") as f:
        f.write("# KOG June opcode -> ljconv stock opcode, checked against the VM handler code (opcode_check.py, LUA_SOURCE_PHASES.md L2)\n")
        f.write("# June: lj_dispatch_init FUN_32ffb270, handler pointers PTR 0x337411d8, lj_vm_asm_begin 0x32fee670. "
                "March: X2/US_SERVICE/x2.exe _lj_vm_asm_begin 0x%08x + _lj_bc_ofs 0x%08x.\n" % (asm, ofs_va))
        w = csv.DictWriter(f, cols, delimiter="\t", lineterminator="\n")
        w.writeheader()
        w.writerows(rows)
    tally = {}
    for r in rows:
        key = r["verdict"].split(" ")[0]
        tally[key] = tally.get(key, 0) + 1
    print("subroutine block identical at lj_vm_returnp (first 64 bytes): %s" % sub_same)
    print("opcode check: %s -> %s" % (dict(sorted(tally.items())), OUT))
    for r in rows:
        if not r["verdict"].startswith(("VERIFIED", "not-in-ljconv")):
            print("  %s -> %s: %s (exact %s)" % (r["kog_opcode_hex"], r["ljconv_stock"], r["verdict"], r["exact_march_matches"]))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
