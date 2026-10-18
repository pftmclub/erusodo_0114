import re, sys, struct
sys.path.insert(0, r"F:\elsword stuff\elsword_2014\els_2014\ElswordFiles\ready_and_built\ProjectX2_SVN\ProjectX2_SVN\source\EU_CN_US\Trunk\tools")
import ljbc

class T:
    __slots__ = ("arr", "h")
    def __init__(s): s.arr = []; s.h = []
    def get(s, k):
        for kk, v in s.h:
            if kk == k: return v
        return None
    def keys(s): return [k for k, _ in s.h]

def R(n): return ("R", n)

# ---------------- June source parser
TOK = re.compile(r'''
 (?P<ws>\s+)|(?P<cm>--\[\[.*?\]\]|--[^\n]*)|(?P<ls>\[(?P<eq>=*)\[)|(?P<str>"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*')
 |(?P<num>0[xX][0-9a-fA-F]+|\d+\.?\d*(?:[eE][+-]?\d+)?|\.\d+)|(?P<name>[A-Za-z_]\w*)|(?P<p>==|~=|<=|>=|[{}()\[\],;:.=\-+*/#])
''', re.X | re.S)

def _esc(c):
    d = {"n": "\n", "t": "\t", "r": "\r", "\\": "\\", "\"": "\"", "'": "'", "a": "\a", "b": "\b", "f": "\f", "v": "\v"}
    if c in d: return d[c]
    if c[0] == "x": return chr(int(c[1:], 16))
    if c.isdigit(): return chr(int(c))
    return c

def tokenize(src):
    pos = 0; out = []; n = len(src)
    while pos < n:
        m = TOK.match(src, pos)
        if not m: raise ValueError("bad token at %d: %r" % (pos, src[pos:pos + 40]))
        if m.group("ls"):
            close = "]" + m.group("eq") + "]"
            e = src.index(close, m.end())
            txt = src[m.end():e]
            if txt.startswith("\r\n"): txt = txt[2:]
            elif txt.startswith("\n"): txt = txt[1:]
            out.append(("S", txt)); pos = e + len(close); continue
        pos = m.end()
        if m.group("ws") or m.group("cm"): continue
        if m.group("str"):
            s = m.group("str")[1:-1]
            s = re.sub(r'\\(x[0-9a-fA-F]{2}|\d{1,3}|.)', lambda mm: _esc(mm.group(1)), s, flags=re.S)
            out.append(("S", s))
        elif m.group("num"):
            t = m.group("num")
            out.append(("N", int(t, 16) if t[:2].lower() == "0x" else (float(t) if re.search(r'[.eE]', t) else int(t))))
        elif m.group("name"): out.append(("I", m.group("name")))
        else: out.append(("P", m.group("p")))
    return out

class P:
    def __init__(s, toks): s.t = toks; s.i = 0
    def peek(s, o=0): return s.t[s.i + o] if s.i + o < len(s.t) else ("E", "")
    def nxt(s): x = s.t[s.i]; s.i += 1; return x
    def expect(s, p):
        x = s.nxt()
        if x != ("P", p): raise ValueError("expected %s got %r at tok %d" % (p, x, s.i))
    def expr(s):
        a = s.primary()
        while s.peek() in (("P", "+"), ("P", "-"), ("P", "*"), ("P", "/")):
            op = s.nxt()[1]; b = s.primary()
            if isinstance(a, (int, float)) and isinstance(b, (int, float)) and not isinstance(a, bool):
                a = a + b if op == "+" else a - b if op == "-" else a * b if op == "*" else a / b
            else: a = R("(%s%s%s)" % (a[1] if isinstance(a, tuple) else a, op, b[1] if isinstance(b, tuple) else b))
        return a
    def primary(s):
        k, v = s.nxt()
        if (k, v) == ("P", "{"): return s.table()
        if k == "S" or k == "N": return v
        if (k, v) == ("P", "-"):
            k2, v2 = s.nxt(); return -v2
        if k == "I":
            if v == "nil": return None
            if v == "true": return True
            if v == "false": return False
            name = v
            while s.peek() in (("P", "."), ("P", "[")):
                if s.nxt()[1] == ".": name += "." + s.nxt()[1]
                else:
                    ix = s.expr(); s.expect("]"); name += "[%s]" % (ix,)
            if s.peek() == ("P", "("):
                s.nxt(); args = []
                while s.peek() != ("P", ")"):
                    args.append(s.expr())
                    if s.peek() == ("P", ","): s.nxt()
                s.nxt()
                return R("%s(%s)" % (name, ",".join(show(x) for x in args)))
            return R(name)
        if (k, v) == ("P", "("):
            e = s.expr(); s.expect(")"); return e
        raise ValueError("bad expr %r at %d" % ((k, v), s.i))
    def table(s):
        t = T()
        while True:
            if s.peek() == ("P", "}"): s.nxt(); return t
            if s.peek()[0] == "I" and s.peek(1) == ("P", "="):
                k = s.nxt()[1]; s.nxt(); t.h.append((k, s.expr()))
            elif s.peek() == ("P", "["):
                s.nxt(); k = s.expr(); s.expect("]"); s.expect("="); t.h.append((k, s.expr()))
            else: t.arr.append(s.expr())
            if s.peek() in (("P", ","), ("P", ";")): s.nxt()
    def stmts(s):
        out = []
        while s.i < len(s.t):
            k, v = s.nxt()
            if k != "I": raise ValueError("stmt start %r at %d" % ((k, v), s.i))
            name = v
            while s.peek() == ("P", "."): s.nxt(); name += "." + s.nxt()[1]
            if s.peek() == ("P", ":"): s.nxt(); name += ":" + s.nxt()[1]
            if s.peek() == ("P", "="):
                s.nxt(); out.append(("=%s" % name, [s.expr()])); continue
            s.expect("(")
            args = []
            while s.peek() != ("P", ")"):
                args.append(s.expr())
                if s.peek() == ("P", ","): s.nxt()
            s.nxt()
            out.append((name, args))
        return out

def parse_june(path):
    src = open(path, "rb").read().decode("utf-8", "replace")
    if src.startswith("\ufeff"): src = src[1:]
    return P(tokenize(src)).stmts()

# ---------------- March bytecode symbolic walk
def knum(e):
    if e[0] == "int":
        v = e[1]; return v - (1 << 32) if v & 0x80000000 else v
    f = struct.unpack("<d", struct.pack("<II", e[1], e[2]))[0]
    return int(f) if f == int(f) and abs(f) < 1e15 else f

def kt(e):
    t = e[0]
    if t == "s": return e[1].decode("utf-8", "replace")
    if t == "i": return e[1] if e[1] < 0x80000000 else e[1] - (1 << 32)
    if t == "n": return knum(("num", e[1], e[2]))
    return {0: None, 1: False, 2: True}[e[1]]

def tdup(e):
    t = T()
    for i, x in enumerate(e[2]):
        if i == 0 and x == ("p", 0): continue
        t.arr.append(kt(x))
    while t.arr and t.arr[-1] is None: t.arr.pop()
    for k, v in e[3]: t.h.append((kt(k), kt(v)))
    return t

def walk(pt):
    reg = {}; out = []
    nm = ljbc.NAMES
    for ins in pt.bc:
        op, a, b, c, d = ljbc.fields(ins)
        n = nm[op] if op < len(nm) else "?"
        if n == "KSTR": reg[a] = ljbc.kgc_at(pt, d)[1].decode("utf-8", "replace")
        elif n == "KSHORT": reg[a] = d - 0x10000 if d & 0x8000 else d
        elif n == "KNUM": reg[a] = knum(ljbc.kn_at(pt, d))
        elif n == "KPRI": reg[a] = {0: None, 1: False, 2: True}[d]
        elif n == "GGET": reg[a] = R(ljbc.kgc_at(pt, d)[1].decode())
        elif n == "MOV": reg[a] = reg.get(d)
        elif n == "TNEW": reg[a] = T()
        elif n == "TDUP": reg[a] = tdup(ljbc.kgc_at(pt, d))
        elif n == "TGETS":
            base = reg.get(b); key = ljbc.kgc_at(pt, c)[1].decode()
            reg[a] = R(base[1] + "." + key) if isinstance(base, tuple) else R("?." + key)
        elif n == "TGETV":
            base = reg.get(b); key = reg.get(c)
            reg[a] = R("%s.%s" % (base[1], key)) if isinstance(base, tuple) and isinstance(key, str) else R("?[]")
        elif n == "TGETB": reg[a] = R("?[]")
        elif n in ("TSETS", "TSETB", "TSETV"):
            t = reg.get(b)
            if not isinstance(t, T): continue
            v = reg.get(a)
            if n == "TSETS": t.h.append((ljbc.kgc_at(pt, c)[1].decode("utf-8", "replace"), v))
            elif n == "TSETB":
                while len(t.arr) < c: t.arr.append(None)
                t.arr[c - 1] = v
            else:
                k = reg.get(c)
                if isinstance(k, int) and not isinstance(k, bool):
                    while len(t.arr) < k: t.arr.append(None)
                    t.arr[k - 1] = v
                else: t.h.append((k, v))
        elif n == "TSETM":
            out.append(("!TSETM", []))
        elif n == "CALL":
            f = reg.get(a)
            args = [reg.get(a + 1 + i) for i in range(c - 1)]
            out.append((f[1] if isinstance(f, tuple) else "?", args))
        elif n == "UNM":
            x = reg.get(d); reg[a] = -x if isinstance(x, (int, float)) else R("?")
        elif n in ("ADDVN", "SUBVN", "MULVN", "ADDVV", "CAT", "FNEW", "LEN", "NOT", "UGET", "KNIL"):
            reg[a] = R("?" + n)
        elif n == "GSET":
            out.append(("=" + ljbc.kgc_at(pt, d)[1].decode(), [reg.get(a)]))
    return out

def parse_march(path):
    d = open(path, "rb").read()
    dump = ljbc.parse(d)
    out = []
    for pt in dump.protos:
        out.extend(walk(pt))
    return out, dump

def show(v):
    if isinstance(v, T):
        return "{" + ",".join([show(x) for x in v.arr] + ["%s=%s" % (k, show(x)) for k, x in v.h]) + "}"
    return repr(v)
