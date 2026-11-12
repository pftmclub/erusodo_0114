"""Minimal reader for data-only Lua table files (DLG_Map_Village / DLG_Map_House / WorldData style): `Name = { ... }` statements,
tables with [key] = / name = / positional fields, strings, numbers and dotted names (kept as strings). W3b, 2026-11-12.
    import luatable; g = luatable.load(path)   # -> {global name: value}; a table is a dict (positional keys 1..n)
Function calls at top level (`f( { ... } )`, `f{...}`) are collected under '__calls__' as (name, [args]).
"""
import re

_tok = re.compile(r'''\s+|--\[\[.*?\]\]|--[^\n]*|"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|-?\d+\.?\d*(?:[eE][-+]?\d+)?|-?\.\d+|0x[0-9A-Fa-f]+|[A-Za-z_][A-Za-z0-9_]*(?:[.:][A-Za-z_][A-Za-z0-9_]*)*|[{}\[\]=,;()]''', re.S)


class Name(str):
    """A bare identifier / dotted enum name (not a string literal)."""


def tokens(s):
    pos, out = 0, []
    while pos < len(s):
        m = _tok.match(s, pos)
        if not m:
            raise ValueError("lex error at %d: %r" % (pos, s[pos:pos + 40]))
        t = m.group(0)
        pos = m.end()
        if t.isspace() or t.startswith("--"):
            continue
        out.append(t)
    return out


def _atom(t):
    if t[0] in "\"'":
        return bytes(t[1:-1], "utf-8").decode("unicode_escape") if "\\" in t else t[1:-1]
    if re.fullmatch(r"-?\d+", t):
        return int(t)
    if re.fullmatch(r"0x[0-9A-Fa-f]+", t):
        return int(t, 16)
    if re.fullmatch(r"-?\d*\.?\d*(?:[eE][-+]?\d+)?", t):
        return float(t)
    return Name(t)


class P:
    def __init__(self, toks):
        self.t, self.i = toks, 0

    def peek(self, k=0):
        return self.t[self.i + k] if self.i + k < len(self.t) else None

    def take(self, want=None):
        t = self.t[self.i]
        if want is not None and t != want:
            raise ValueError("want %r got %r at token %d (%r)" % (want, t, self.i, self.t[max(0, self.i - 8):self.i + 3]))
        self.i += 1
        return t

    def value(self):
        t = self.peek()
        if t == "{":
            return self.table()
        self.take()
        v = _atom(t)
        if isinstance(v, Name) and self.peek() in ("(", "{"):
            return ("call", v, self.args())
        return v

    def args(self):
        if self.peek() == "{":
            return [self.table()]
        self.take("(")
        out = []
        while self.peek() != ")":
            out.append(self.value())
            if self.peek() == ",":
                self.take()
        self.take(")")
        return out

    def table(self):
        self.take("{")
        d, n = {}, 0
        while self.peek() != "}":
            if self.peek() == "[":
                self.take("[")
                k = self.value()
                self.take("]")
                self.take("=")
                d[k] = self.value()
            elif self.peek(1) == "=" and re.fullmatch(r"[A-Za-z_]\w*", self.peek() or ""):
                k = self.take()
                self.take("=")
                d[k] = self.value()
            else:
                n += 1
                d[n] = self.value()
            if self.peek() in (",", ";"):
                self.take()
        self.take("}")
        return d


def loads(s):
    s = s.lstrip("\ufeff")
    p = P(tokens(s))
    g = {"__calls__": []}
    while p.peek() is not None:
        name = p.take()
        if p.peek() == "=":
            p.take()
            g[name] = p.value()
        elif p.peek() in ("(", "{"):
            g["__calls__"].append((name, p.args()))
        elif p.peek() in (";",):
            p.take()
        else:
            raise ValueError("unexpected %r after %r" % (p.peek(), name))
        if p.peek() == ";":
            p.take()
    return g


def load(path):
    with open(path, "rb") as f:
        return loads(f.read().decode("utf-8", "replace"))
