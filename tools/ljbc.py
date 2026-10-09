#!/usr/bin/env python3
"""LuaJIT 2.0 bytecode dump reader and the T1 comparison (LUA_SOURCE_PHASES.md Rule 1, Phase L2).

    python tools/ljbc.py list FILE.lua                 # disassemble a 0x81 dump, constants resolved
    python tools/ljbc.py t1 A.lua B.lua                # compare two 0x81 dumps at T1 (exit 0 PASS, 1 FAIL)

Used by tools/luasrc.py (`check`, `selftest`, `calibrate`, `convert`'s T1 tier). The reader follows lj_bcwrite.c /
lj_bcread.c of the vendored LuaJIT 2.0.2 (luajitLib/LuaJIT-2.0.2, stock opcode order; the KOG build only changes the
header to 1B 4B 4C 81). Stock 1B 4C 4A 01 dumps are read too.

T1 (Rule 1), per prototype in tree order (the main chunk first, children paired through the FNEW that creates them):
  - prototype flags, numparams, framesize, the upvalue descriptors, the instruction count and the child count are equal;
  - every instruction has the same opcode and the same A/B/C/D operands, except that an operand which indexes the
    constant tables (str / num / tab / cdata / func modes of lj_bc_mode) is compared by the RESOLVED value, byte for
    byte: strings and cdata by their bytes, numbers by their dump encoding (int32, or the IEEE-754 bit pattern - so
    -0.0, NaN payloads and 1 vs 1.0 all differ), TDUP templates by their contents - the array part position by
    position, the hash part as a set of byte-exact (key, value) pairs (byte for byte when a key repeats: the last pair wins, so
    the order is content; L2 review) - and FNEW children by a recursive T1 comparison;
  - the constant tables are also equal as multisets (unused or duplicated constants are a difference);
  - ignored: constant-slot ORDER, and the storage order of a template's hash part (user, 2026-10-09: "as long as the
    key-value pairs are not in the wrong parent". A template is compared with the one its own TDUP instruction loads, so a
    pair that moved to another table is a difference). The storage order is what `pairs`/`next` walk; a template whose
    pairs are stored in another order is counted in T1Result.layout, as information only.
"""
import collections
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))

MAR_MAGIC = b"\x1bKL\x81"
STD_MAGIC = b"\x1bLJ\x01"
F_BE, F_STRIP, F_FFI = 0x01, 0x02, 0x04

# Stock LuaJIT 2.0 BCDEF (lj_bc.h), in stock opcode order: name, ra, rb, rc. A '___' rb means the instruction
# carries a 16-bit D operand in rc's mode. Same table as june2017_re/lj_bcmode.py (asserted in selfcheck()).
BCDEF = """ISLT var ___ var;ISGE var ___ var;ISLE var ___ var;ISGT var ___ var;ISEQV var ___ var;
ISNEV var ___ var;ISEQS var ___ str;ISNES var ___ str;ISEQN var ___ num;ISNEN var ___ num;
ISEQP var ___ pri;ISNEP var ___ pri;ISTC dst ___ var;ISFC dst ___ var;IST ___ ___ var;
ISF ___ ___ var;MOV dst ___ var;NOT dst ___ var;UNM dst ___ var;LEN dst ___ var;
ADDVN dst var num;SUBVN dst var num;MULVN dst var num;DIVVN dst var num;MODVN dst var num;
ADDNV dst var num;SUBNV dst var num;MULNV dst var num;DIVNV dst var num;MODNV dst var num;
ADDVV dst var var;SUBVV dst var var;MULVV dst var var;DIVVV dst var var;MODVV dst var var;
POW dst var var;CAT dst rbase rbase;KSTR dst ___ str;KCDATA dst ___ cdata;KSHORT dst ___ lits;
KNUM dst ___ num;KPRI dst ___ pri;KNIL base ___ base;UGET dst ___ uv;USETV uv ___ var;
USETS uv ___ str;USETN uv ___ num;USETP uv ___ pri;UCLO rbase ___ jump;FNEW dst ___ func;
TNEW dst ___ lit;TDUP dst ___ tab;GGET dst ___ str;GSET var ___ str;TGETV dst var var;
TGETS dst var str;TGETB dst var lit;TSETV var var var;TSETS var var str;TSETB var var lit;
TSETM base ___ num;CALLM base lit lit;CALL base lit lit;CALLMT base ___ lit;CALLT base ___ lit;
ITERC base lit lit;ITERN base lit lit;VARG base lit lit;ISNEXT base ___ jump;RETM base ___ lit;
RET rbase ___ lit;RET0 rbase ___ lit;RET1 rbase ___ lit;FORI base ___ jump;JFORI base ___ jump;
FORL base ___ jump;IFORL base ___ jump;JFORL base ___ lit;ITERL base ___ jump;
IITERL base ___ jump;JITERL base ___ lit;LOOP rbase ___ jump;ILOOP rbase ___ jump;
JLOOP rbase ___ lit;JMP rbase ___ jump;FUNCF rbase ___ ___;IFUNCF rbase ___ ___;
JFUNCF rbase ___ lit;FUNCV rbase ___ ___;IFUNCV rbase ___ ___;JFUNCV rbase ___ lit;
FUNCC rbase ___ ___;FUNCCW rbase ___ ___"""
OPS = [x.split() for x in BCDEF.replace("\n", "").split(";")]
NAMES = [o[0] for o in OPS]
KGC_MODES = {"str", "tab", "cdata", "func"}     # index the GC constants, k[~D]
KN_MODES = {"num"}                               # index the number constants, k[D]


class DumpError(Exception):
    pass


class Reader:
    def __init__(self, d, p=0):
        self.d, self.p = d, p

    def byte(self):
        if self.p >= len(self.d):
            raise DumpError("truncated at 0x%x" % self.p)
        b = self.d[self.p]
        self.p += 1
        return b

    def uleb(self):
        v, sh = 0, 0
        while True:
            b = self.byte()
            v |= (b & 0x7F) << sh
            if b < 0x80:
                return v
            sh += 7

    def uleb33(self):
        """lj_bcread.c bcread_uleb128_33: the first byte's low bit is a flag. -> (value, flag)."""
        b = self.byte()
        flag = b & 1
        v = b >> 1
        if v >= 0x40:
            v &= 0x3F
            sh = 6
            while True:
                b = self.byte()
                v |= (b & 0x7F) << sh
                if b < 0x80:
                    break
                sh += 7
        return v, flag

    def take(self, n):
        if self.p + n > len(self.d):
            raise DumpError("truncated: %d byte(s) wanted at 0x%x" % (n, self.p))
        b = self.d[self.p:self.p + n]
        self.p += n
        return b


class Proto:
    __slots__ = ("index", "flags", "numparams", "framesize", "uv", "kgc", "kn", "bc", "children", "child_at", "dbg", "firstline",
                 "numline", "lines", "kgc_raw", "kn_raw", "dbghdr")

    def __repr__(self):
        return "<proto #%d: %d ins, %d kgc, %d kn, %d children>" % (self.index, len(self.bc), len(self.kgc), len(self.kn),
                                                                     len(self.children))


def _ktabk(r):
    """One table-constant key or value (bcread_ktabk) -> canonical tuple."""
    tp = r.uleb()
    if tp >= 5:
        return ("s", r.take(tp - 5))
    if tp == 3:
        return ("i", r.uleb())
    if tp == 4:
        lo = r.uleb()
        hi = r.uleb()
        return ("n", lo, hi)
    return ("p", tp)          # 0 nil, 1 false, 2 true


def _kgc_entry(r, stack):
    tp = r.uleb()
    if tp >= 5:
        return ("str", r.take(tp - 5))
    if tp == 0:
        if not stack:
            raise DumpError("child prototype constant with an empty prototype stack")
        return ("child", stack.pop())
    if tp == 1:
        start = r.p
        narray = r.uleb()
        nhash = r.uleb()
        arr = [_ktabk(r) for _ in range(narray)]
        hsh = [(_ktabk(r), _ktabk(r)) for _ in range(nhash)]
        return ("tab", r.d[start:r.p], arr, hsh)
    if tp in (2, 3):
        lo = r.uleb()
        hi = r.uleb()
        return ("i64" if tp == 2 else "u64", lo, hi)
    if tp == 4:
        return ("complex", r.uleb(), r.uleb(), r.uleb(), r.uleb())
    raise DumpError("unknown GC constant type %d at 0x%x" % (tp, r.p))


def _kn_entry(r):
    lo, isnum = r.uleb33()
    if isnum:
        hi = r.uleb()
        return ("num", lo & 0xFFFFFFFF, hi & 0xFFFFFFFF)
    return ("int", lo & 0xFFFFFFFF)


def _lineinfo(pt):
    """Source line of each dumped instruction (fs_fixup_line: one 1/2/4-byte delta from firstline per instruction)."""
    w = 1 if pt.numline < 256 else 2 if pt.numline < 65536 else 4
    n = len(pt.bc)
    if len(pt.dbg) < w * n:
        return None
    fmt = {1: "B", 2: "H", 4: "I"}[w]
    return [pt.firstline + v for v in struct.unpack("<%d%s" % (n, fmt), pt.dbg[:w * n])]


class Dump:
    __slots__ = ("flags", "protos", "main", "name", "head")


def parse(d):
    """A 0x81 (or stock 0x01) dump -> Dump. Raises DumpError on anything malformed or trailing."""
    if d[:4] not in (MAR_MAGIC, STD_MAGIC):
        raise DumpError("header %s is not a March (1b4b4c81) or stock (1b4c4a01) LuaJIT 2.0 dump" % d[:4].hex())
    r = Reader(d, 4)
    dump = Dump()
    dump.flags = r.uleb()
    if dump.flags & F_BE:
        raise DumpError("big-endian dump")
    dump.name = None
    if not dump.flags & F_STRIP:
        dump.name = r.take(r.uleb())
    hend = r.p
    stack, protos = [], []
    while True:
        if r.p >= len(d):
            raise DumpError("no terminating zero-length prototype")
        plen = r.uleb()
        if plen == 0:
            break
        end = r.p + plen
        pt = Proto()
        pt.index = len(protos)
        pt.flags = r.byte()
        pt.numparams = r.byte()
        pt.framesize = r.byte()
        sizeuv = r.byte()
        sizekgc = r.uleb()
        sizekn = r.uleb()
        sizebc = r.uleb()
        sizedbg = 0
        pt.firstline = pt.numline = 0
        h0 = r.p
        if not dump.flags & F_STRIP:
            sizedbg = r.uleb()
            if sizedbg:
                pt.firstline = r.uleb()
                pt.numline = r.uleb()
        pt.dbghdr = d[h0:r.p]
        raw = r.take(4 * sizebc)
        pt.bc = list(struct.unpack("<%dI" % sizebc, raw))
        pt.uv = list(struct.unpack("<%dH" % sizeuv, r.take(2 * sizeuv)))
        pt.kgc, pt.kgc_raw = [], []
        for _ in range(sizekgc):
            k0 = r.p
            pt.kgc.append(_kgc_entry(r, stack))
            pt.kgc_raw.append(d[k0:r.p])
        pt.kn, pt.kn_raw = [], []
        for _ in range(sizekn):
            k0 = r.p
            pt.kn.append(_kn_entry(r))
            pt.kn_raw.append(d[k0:r.p])
        pt.dbg = r.take(sizedbg) if sizedbg else b""
        pt.lines = _lineinfo(pt) if sizedbg else None
        if r.p != end:
            raise DumpError("prototype #%d: read %d byte(s), its length says %d" % (pt.index, r.p - (end - plen), plen))
        pt.children = [e[1] for e in pt.kgc if e[0] == "child"]
        pt.child_at = {len(pt.kgc) - 1 - j: e[1] for j, e in enumerate(pt.kgc) if e[0] == "child"}   # D -> child
        stack.append(pt)
        protos.append(pt)
    if r.p != len(d):
        raise DumpError("%d trailing byte(s) after the terminator" % (len(d) - r.p))
    if len(stack) != 1:
        raise DumpError("%d prototype(s) left on the stack (expected only the main chunk)" % len(stack))
    dump.protos = protos
    dump.main = stack[0]
    dump.head = d[:hend]
    return dump


def _uleb(v):
    out = bytearray()
    while True:
        b = v & 0x7F
        v >>= 7
        if v:
            out.append(b | 0x80)
        else:
            out.append(b)
            return bytes(out)


def _uleb33(v, flag):
    """lj_bcwrite.c bcwrite_uleb128_33."""
    out = bytearray()
    first = True
    while True:
        if first:
            bits, v = (v & 0x3F), v >> 6
            b = (bits << 1) | flag
            out.append(b | (0x80 if v else 0))
            first = False
        else:
            b, v = v & 0x7F, v >> 7
            out.append(b | (0x80 if v else 0))
        if not v:
            return bytes(out)


def kn_bytes(e):
    if e[0] == "int":
        return _uleb33(e[1], 0)
    return _uleb33(e[1], 1) + _uleb(e[2])


def serialize(dump):
    """Dump -> bytes, in the parsed protos' dump order, from each proto's fields and raw constants (the self-test edits
    those; parse(serialize(parse(x))) == parse(x) and serialize(parse(x)) == x for every shipped file)."""
    out = bytearray(dump.head)
    for pt in dump.protos:
        body = bytearray([pt.flags, pt.numparams, pt.framesize, len(pt.uv)])
        body += _uleb(len(pt.kgc_raw)) + _uleb(len(pt.kn_raw)) + _uleb(len(pt.bc)) + pt.dbghdr
        body += struct.pack("<%dI" % len(pt.bc), *pt.bc) + struct.pack("<%dH" % len(pt.uv), *pt.uv)
        body += b"".join(pt.kgc_raw) + b"".join(pt.kn_raw) + pt.dbg
        out += _uleb(len(body)) + body
    out += b"\x00"
    return bytes(out)


# ----------------------------------------------------------------------------------------------
# operands
def fields(ins):
    op = ins & 0xFF
    return op, (ins >> 8) & 0xFF, ins >> 24, (ins >> 16) & 0xFF, ins >> 16      # op, A, B, C, D


def kgc_at(pt, idx):
    """k[~idx] -> the dump-order entry (bcread fills k[-sizekgc] .. k[-1] in dump order)."""
    j = len(pt.kgc) - 1 - idx
    if not 0 <= j < len(pt.kgc):
        return None
    return pt.kgc[j]


def kn_at(pt, idx):
    return pt.kn[idx] if 0 <= idx < len(pt.kn) else None


def value_key(e):
    """Canonical comparable form of a constant: byte-exact, slot-independent. A TDUP template keys on its whole
    serialisation, so its hash part counts in dump order (= the parsed table's node order, top down)."""
    if e is None:
        return ("missing",)
    if e[0] == "tab":
        return ("tab", e[1])
    if e[0] == "child":
        return ("child",)
    return e


def loose_key(e):
    """Like value_key, but a template's hash part as a set: equal contents, possibly a different node layout. Only when
    its keys are unique: bcread_ktab stores the pairs in dump order, so with a repeated key the last pair wins and the order
    is content (the parser never writes one; a crafted dump can. L2 review)."""
    if e is not None and e[0] == "tab" and len({k for k, _v in e[3]}) == len(e[3]):
        return ("tab", tuple(e[2]), frozenset(e[3]))
    return value_key(e)


def show_const(e, limit=60):
    if e is None:
        return "<no such constant>"
    k = e[0]
    if k == "str":
        s = e[1].decode("utf-8", "backslashreplace")
        s = s if len(s) <= limit else s[:limit] + "..."
        return '"%s"' % s.replace("\n", "\\n").replace("\r", "\\r")
    if k == "int":
        v = e[1]
        return str(v - (1 << 32) if v & 0x80000000 else v)
    if k == "num":
        f = struct.unpack("<d", struct.pack("<II", e[1], e[2]))[0]
        return "%r (0x%08x%08x)" % (f, e[2], e[1])
    if k == "tab":
        return "template{%d array, %d hash; %d bytes}" % (len(e[2]), len(e[3]), len(e[1]))
    if k == "child":
        return "function #%d" % e[1].index
    return repr(e)


def operand_text(pt, ins):
    op, a, b, c, d = fields(ins)
    if op >= len(OPS):
        return "op 0x%02x?" % op
    name, ma, mb, mc = OPS[op]
    out = [name]
    if ma != "___":
        out.append(str(a))
    if mb != "___":
        out.append(str(b))
        out.append(_res(pt, mc, c))
    elif mc != "___":
        out.append(_res(pt, mc, d))
    return " ".join(out)


def _res(pt, mode, v):
    if mode in KGC_MODES:
        return show_const(kgc_at(pt, v))
    if mode in KN_MODES:
        return show_const(kn_at(pt, v))
    if mode == "jump":
        return "=>%+d" % (v - 0x8000 + 1)
    if mode == "lits":
        return str(v - 0x10000 if v & 0x8000 else v)
    return str(v)


def listing(dump):
    out = []
    for pt in dump.protos:
        out.append("-- proto #%d flags=%02x params=%d frame=%d uv=%s kgc=%d kn=%d children=%s" % (
            pt.index, pt.flags, pt.numparams, pt.framesize, ",".join("%04x" % u for u in pt.uv), len(pt.kgc), len(pt.kn),
            ",".join("#%d" % c.index for c in pt.children) or "-"))
        for pc, ins in enumerate(pt.bc):
            out.append("%04d %s" % (pc + 1, operand_text(pt, ins)))
    return out


# ----------------------------------------------------------------------------------------------
# T1
class T1Result:
    def __init__(self):
        self.diffs = []          # (path, message)
        self.total = 0           # every difference (T1 holds when 0)
        self.layout = 0          # information: templates holding the same pairs in another storage order (not a difference)
        self.layout_sites = []   # (source proto index, pc 0-based, reference template entry) of each such TDUP
        self.src_at = []         # per kept diff: (source proto index, pc 0-based) for an instruction difference, else None
        self.protos = 0
        self.ins = 0

    @property
    def ok(self):
        return self.total == 0

    def add(self, path, msg, cap=40, src=None):
        self.total += 1
        if len(self.diffs) < cap:
            self.diffs.append((path, msg))
            self.src_at.append(src)


def compare(src, ref, cap=40):
    """T1 of two parsed dumps. `src` is the recompiled source, `ref` the reference. -> T1Result."""
    res = T1Result()
    if (src.flags & ~F_STRIP) != (ref.flags & ~F_STRIP):
        res.add("header", "dump flags %02x vs %02x" % (src.flags, ref.flags), cap)
    if not (src.flags & F_STRIP and ref.flags & F_STRIP):
        res.add("header", "both dumps must be stripped (flags %02x vs %02x)" % (src.flags, ref.flags), cap)
    seen = set()
    _cmp_proto(src.main, ref.main, "main", res, cap, seen)
    if len(src.protos) != len(ref.protos):
        res.add("tree", "%d vs %d prototypes in total" % (len(src.protos), len(ref.protos)), cap)
    return res


def const_field(op):
    """-> (mask of the operand that indexes a constant table, its mode), or (0, None). Every other bit of the
    instruction word is compared raw, unused fields included."""
    _name, _ma, mb, mc = OPS[op]
    if mc in KGC_MODES or mc in KN_MODES:
        return (0x00FF0000 if mb != "___" else 0xFFFF0000), mc
    return 0, None


def _cmp_proto(p, q, path, res, cap, seen):
    if (p.index, q.index) in seen:
        return
    seen.add((p.index, q.index))
    res.protos += 1
    for f in ("flags", "numparams", "framesize"):
        if getattr(p, f) != getattr(q, f):
            res.add(path, "%s %r vs %r" % (f, getattr(p, f), getattr(q, f)), cap)
    if p.uv != q.uv:
        res.add(path, "upvalue descriptors [%s] vs [%s]" % (",".join("%04x" % u for u in p.uv),
                                                            ",".join("%04x" % u for u in q.uv)), cap)
    if len(p.children) != len(q.children):
        res.add(path, "%d vs %d child prototypes" % (len(p.children), len(q.children)), cap)
    kp = [e for e in p.kgc if e[0] != "child"]
    kq = [e for e in q.kgc if e[0] != "child"]
    ca, cb = collections.Counter(map(loose_key, kp)), collections.Counter(map(loose_key, kq))
    if ca != cb:
        res.add(path, "GC constant multiset differs: only in source %s; only in reference %s" % (
            _few(kp, ca - cb, loose_key), _few(kq, cb - ca, loose_key)), cap)
    na, nb = collections.Counter(p.kn), collections.Counter(q.kn)
    if na != nb:
        res.add(path, "number constant multiset differs: only in source %s; only in reference %s" % (
            _few(p.kn, na - nb), _few(q.kn, nb - na)), cap)
    if len(p.bc) != len(q.bc):
        res.add(path, "%d vs %d instructions" % (len(p.bc), len(q.bc)), cap)
    pairs = []
    for pc, (x, y) in enumerate(zip(p.bc, q.bc)):
        res.ins += 1
        op = x & 0xFF
        where = "%s pc %d" % (path, pc + 1)
        if op != (y & 0xFF) or op >= len(OPS):
            res.add(where, "%s  vs  %s" % (operand_text(p, x), operand_text(q, y)), cap, src=(p.index, pc))
            continue
        mask, mode = const_field(op)
        if (x & ~mask & 0xFFFFFFFF) != (y & ~mask & 0xFFFFFFFF):
            res.add(where, "%s  vs  %s" % (operand_text(p, x), operand_text(q, y)), cap, src=(p.index, pc))
            continue
        if not mode:
            continue
        verdict = _same_operand(p, q, mode, (x & mask) >> 16, (y & mask) >> 16, pairs)
        if verdict == "layout":
            res.layout += 1
            res.layout_sites.append((p.index, pc, kgc_at(q, (y & mask) >> 16)))
        elif verdict != "same":
            res.add(where, "%s  vs  %s" % (operand_text(p, x), operand_text(q, y)), cap, src=(p.index, pc))
    done = set()
    for cp, cq in pairs:
        if (cp.index, cq.index) in done:
            continue
        done.add((cp.index, cq.index))
        _cmp_proto(cp, cq, "%s/fn#%d" % (path, cq.index), res, cap, seen)
    # a child no FNEW reaches (the parser never makes one) is still compared, in kgc order
    used_p, used_q = {a.index for a, _ in pairs}, {b.index for _, b in pairs}
    rest_p = [c for c in p.children if c.index not in used_p]
    rest_q = [c for c in q.children if c.index not in used_q]
    for cp, cq in zip(rest_p, rest_q):
        _cmp_proto(cp, cq, "%s/unreferenced#%d" % (path, cq.index), res, cap, seen)


def _same_operand(p, q, mode, x, y, pairs):
    """-> 'same' | 'layout' (a template with equal contents in another hash layout) | 'diff'."""
    if mode == "func":
        a, b = kgc_at(p, x), kgc_at(q, y)
        if not a or not b or a[0] != "child" or b[0] != "child":
            return "diff"
        pairs.append((a[1], b[1]))
        return "same"
    if mode in KGC_MODES:
        a, b = kgc_at(p, x), kgc_at(q, y)
        if a is None or b is None or a[0] != b[0] or a[0] == "child":
            return "diff"
        if value_key(a) == value_key(b):
            return "same"
        return "layout" if loose_key(a) == loose_key(b) else "diff"
    if mode in KN_MODES:
        a, b = kn_at(p, x), kn_at(q, y)
        return "same" if a is not None and a == b else "diff"
    return "diff"


def _few(pool, counter, keyf=value_key, n=3):
    keys = list(counter.elements())
    shown = []
    for k in keys[:n]:
        e = next((e for e in pool if keyf(e) == k), None) or ("?",)
        shown.append(show_const(e))
    more = ", ... %d more" % (len(keys) - n) if len(keys) > n else ""
    return "{%s%s}" % (", ".join(shown), more) if keys else "{}"


def t1_bytes(src_bc, ref_bc, cap=40):
    """T1 of two dump byte strings. -> T1Result (a parse failure is a diff, never a pass)."""
    try:
        a = parse(src_bc)
    except DumpError as e:
        r = T1Result(); r.add("source dump", str(e)); return r
    try:
        b = parse(ref_bc)
    except DumpError as e:
        r = T1Result(); r.add("reference dump", str(e)); return r
    return compare(a, b, cap)


# ----------------------------------------------------------------------------------------------
# bytecode-level seeds for `luasrc.py selftest`: edits a source change cannot isolate. expect 'fail' = T1 must reject,
# 'pass' = a pure constant-slot renumbering T1 must accept (Rule 1 ignores slot order and nothing else).
def _ktabk_bytes(k):
    if k[0] == "s":
        return _uleb(5 + len(k[1])) + k[1]
    if k[0] == "i":
        return _uleb(3) + _uleb(k[1])
    if k[0] == "n":
        return _uleb(4) + _uleb(k[1]) + _uleb(k[2])
    return _uleb(k[1])


def _tab_entry(arr, hsh):
    """-> (kgc entry, kgc raw bytes) of a template with this array part and these hash pairs, in this order."""
    body = _uleb(len(arr)) + _uleb(len(hsh)) + b"".join(_ktabk_bytes(v) for v in arr) + \
        b"".join(_ktabk_bytes(k) + _ktabk_bytes(v) for k, v in hsh)
    return ("tab", body, list(arr), list(hsh)), _uleb(1) + body


def _renumber(pt, kperm=None, nperm=None):
    """Apply a permutation of the non-child GC constants (dump index -> new dump index) and/or of the number constants,
    renumbering every operand that indexes them."""
    if kperm:
        n = len(pt.kgc_raw)
        raw, ent = list(pt.kgc_raw), list(pt.kgc)
        for j, j2 in kperm.items():
            raw[j2], ent[j2] = pt.kgc_raw[j], pt.kgc[j]
        pt.kgc_raw, pt.kgc = raw, ent
    if nperm:
        raw, ent = list(pt.kn_raw), list(pt.kn)
        for j, j2 in nperm.items():
            raw[j2], ent[j2] = pt.kn_raw[j], pt.kn[j]
        pt.kn_raw, pt.kn = raw, ent
    for pc, ins in enumerate(pt.bc):
        mask, mode = const_field(ins & 0xFF) if (ins & 0xFF) < len(OPS) else (0, None)
        if not mode or mode == "func":
            continue
        idx = (ins & mask) >> 16
        if mode in KGC_MODES and kperm:
            j = len(pt.kgc_raw) - 1 - idx
            if j in kperm:
                idx = len(pt.kgc_raw) - 1 - kperm[j]
        elif mode in KN_MODES and nperm and idx in nperm:
            idx = nperm[idx]
        else:
            continue
        pt.bc[pc] = (ins & ~mask & 0xFFFFFFFF) | (idx << 16)


def dump_mutants(ref):
    """-> [(kind, expect, mutant bytes, reference bytes)] for one 0x81 reference."""
    out = []

    def fresh():
        return parse(ref)

    def add(kind, expect, d, against=None):
        out.append((kind, expect, serialize(d), against if against is not None else ref))

    base = fresh()
    child = next((pt for pt in base.protos if pt is not base.main), None)
    tgt = child.index if child else base.main.index
    d = fresh(); d.protos[tgt].framesize += 1; add("framesize", "fail", d)
    d = fresh(); d.protos[tgt].numparams += 1; add("numparams", "fail", d)
    d = fresh(); d.protos[tgt].flags ^= 0x02; add("proto-flags (vararg)", "fail", d)
    uvp = next((pt.index for pt in base.protos if pt.uv), None)
    if uvp is not None:
        d = fresh(); d.protos[uvp].uv[0] ^= 0x4000; add("upvalue-descriptor (immutable bit)", "fail", d)
    nump = next(((pt.index, j) for pt in base.protos for j, e in enumerate(pt.kn) if e[0] == "num"), None)
    if nump:
        i, j = nump
        d = fresh(); e = d.protos[i].kn[j]; e2 = ("num", e[1], e[2] ^ 0x80000000)
        d.protos[i].kn[j], d.protos[i].kn_raw[j] = e2, kn_bytes(e2); add("number sign bit", "fail", d)
        a = fresh(); b = fresh()
        for x, payload in ((a, 1), (b, 2)):
            e2 = ("num", payload, 0x7FF80000)
            x.protos[i].kn[j], x.protos[i].kn_raw[j] = e2, kn_bytes(e2)
        add("NaN payload (1 vs 2)", "fail", b, serialize(a))
    intp = next(((pt.index, j) for pt in base.protos for j, e in enumerate(pt.kn) if e[0] == "int"), None)
    if intp:
        i, j = intp
        d = fresh(); v = d.protos[i].kn[j][1]
        f = float(v - (1 << 32) if v & 0x80000000 else v)
        lo, hi = struct.unpack("<II", struct.pack("<d", -0.0 if f == 0 else f))
        e2 = ("num", lo, hi)
        d.protos[i].kn[j], d.protos[i].kn_raw[j] = e2, kn_bytes(e2)
        add("-0.0 for 0" if f == 0 else "int constant re-encoded as the equal double", "fail", d)
    sp = next((pt for pt in base.protos if sum(1 for e in pt.kgc if e[0] == "str") >= 2), None)
    if sp is not None:
        js = [j for j, e in enumerate(sp.kgc) if e[0] == "str"]
        d = fresh(); pt = d.protos[sp.index]
        a, b = js[0], js[-1]
        pt.kgc_raw[a], pt.kgc_raw[b] = pt.kgc_raw[b], pt.kgc_raw[a]
        pt.kgc[a], pt.kgc[b] = pt.kgc[b], pt.kgc[a]
        add("two string slots swapped, operands not renumbered", "fail", d)
        d = fresh(); pt = d.protos[sp.index]
        nonchild = [j for j, e in enumerate(pt.kgc) if e[0] != "child"]
        rot = {j: nonchild[(k + 1) % len(nonchild)] for k, j in enumerate(nonchild)}
        _renumber(pt, kperm=rot)
        add("GC constant slots rotated + operands renumbered (control)", "pass", d)
    np_ = next((pt for pt in base.protos if len(pt.kn) >= 2), None)
    if np_ is not None:
        d = fresh(); pt = d.protos[np_.index]
        if pt.kn[0] != pt.kn[-1]:
            pt.kn_raw[0], pt.kn_raw[-1] = pt.kn_raw[-1], pt.kn_raw[0]
            pt.kn[0], pt.kn[-1] = pt.kn[-1], pt.kn[0]
            add("two number slots swapped, operands not renumbered", "fail", d)
        d = fresh(); pt = d.protos[np_.index]
        k = len(pt.kn)
        _renumber(pt, nperm={j: (j + 1) % k for j in range(k)})
        add("number constant slots rotated + operands renumbered (control)", "pass", d)
    tp = next((pt for pt in base.protos if sum(1 for e in pt.kgc if e[0] == "tab" and len(e[3]) >= 2) >= 2), None)
    if tp is not None:
        tabs = [j for j, e in enumerate(tp.kgc) if e[0] == "tab" and len(e[3]) >= 2]
        d = fresh(); pt = d.protos[tp.index]
        j = tabs[0]
        _t, _raw, arr, hsh = pt.kgc[j]
        pt.kgc[j], pt.kgc_raw[j] = _tab_entry(arr, list(reversed(hsh)))
        add("template hash pairs stored in reverse order (control: same pairs)", "pass", d)
        d = fresh(); pt = d.protos[tp.index]
        a, b = tabs[0], tabs[1]
        _t, _r, arr_a, hsh_a = pt.kgc[a]
        _t, _r, arr_b, hsh_b = pt.kgc[b]
        move = next((kv for kv in hsh_a if kv[0] not in {k for k, _v in hsh_b}), None)
        if move is not None:
            pt.kgc[a], pt.kgc_raw[a] = _tab_entry(arr_a, [kv for kv in hsh_a if kv is not move])
            pt.kgc[b], pt.kgc_raw[b] = _tab_entry(arr_b, hsh_b + [move])
            add("one key/value pair moved to another table", "fail", d)
        # L2 review: a repeated key (last pair wins), the two pairs in the other order; and an array item stored in the
        # hash part at its own index (an equal table, but Rule 1 compares the array part position by position)
        k0, v0 = hsh_a[0]
        other = next((v for _k, v in hsh_a if v != v0), ("s", v0[1] + b"Z") if v0[0] == "s" else ("p", 2 if v0 != ("p", 2) else 1))
        a_ = fresh(); b_ = fresh()
        a_.protos[tp.index].kgc[a], a_.protos[tp.index].kgc_raw[a] = _tab_entry(arr_a, hsh_a + [(k0, other)])
        b_.protos[tp.index].kgc[a], b_.protos[tp.index].kgc_raw[a] = _tab_entry(arr_a, [(k0, other)] + hsh_a)
        add("repeated hash key, its two pairs in the other order (last one wins)", "fail", b_, serialize(a_))
    ap = next(((pt, j) for pt in base.protos for j, e in enumerate(pt.kgc)
               if e[0] == "tab" and len([v for v in e[2] if v != ("p", 0)]) >= 1), None)
    if ap is not None:
        pt0, j = ap
        d = fresh(); pt = d.protos[pt0.index]
        _t, _r, arr, hsh = pt.kgc[j]
        i = max(i for i, v in enumerate(arr) if v != ("p", 0))
        pt.kgc[j], pt.kgc_raw[j] = _tab_entry(arr[:i], hsh + [(("i", i), arr[i])])
        add("last array item stored in the hash part at its own index", "fail", d)
    fp = next((pt for pt in base.protos if len([e for e in pt.kgc if e[0] == "child"]) >= 2), None)
    if fp is not None:
        d = fresh(); pt = d.protos[fp.index]
        fn = [pc for pc, ins in enumerate(pt.bc) if NAMES[ins & 0xFF] == "FNEW"]
        if len(fn) >= 2:
            x, y = fn[0], fn[1]
            dx, dy = pt.bc[x] >> 16, pt.bc[y] >> 16
            if dx != dy and kgc_at(pt, dx)[1].bc != kgc_at(pt, dy)[1].bc:
                pt.bc[x] = (pt.bc[x] & 0xFFFF) | (dy << 16)
                pt.bc[y] = (pt.bc[y] & 0xFFFF) | (dx << 16)
                add("two FNEW children swapped", "fail", d)
    return out


def selfcheck():
    sys.path.insert(0, os.path.join(os.path.dirname(HERE), "june2017_re"))
    import lj_bcmode
    assert [tuple(s) for s in lj_bcmode.std] == [tuple(o) for o in OPS], "BCDEF differs from june2017_re/lj_bcmode.py"


def main(argv):
    if len(argv) >= 2 and argv[0] == "list":
        for ln in listing(parse(open(argv[1], "rb").read())):
            print(ln)
        return 0
    if len(argv) == 3 and argv[0] == "t1":
        r = t1_bytes(open(argv[1], "rb").read(), open(argv[2], "rb").read())
        for path, msg in r.diffs:
            print("  %s: %s" % (path, msg))
        print("T1 %s: %d difference(s) over %d prototype(s), %d instruction(s)%s" % (
            "PASS" if r.ok else "FAIL", r.total, r.protos, r.ins,
            "; %d table constant(s) hold the same pairs in another storage order" % r.layout if r.layout else ""))
        return 0 if r.ok else 1
    print(__doc__)
    return 2


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
