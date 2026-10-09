#!/usr/bin/env python3
"""Lua source helpers for tools/luasrc.py (LUA_SOURCE_PHASES.md Phase L2): a tokenizer, a table-constructor parser, and a
best-effort template-layout tidy-up for ljd decompiles.

What the tidy-up does. A TDUP template's hash part is dumped top node first, so its bytes fix the order the parsed table
iterates in (`next`). ljd prints the constant fields in that dump order, and the parser, inserting them in that order while
the table grows, often lays them out differently: same keys and values, another iteration order. Since the user's
2026-10-09 decision T1 ignores that storage order (the pairs must match, in the right table), so this never decides a
verdict: it runs only on a text that already passes T1, within a small budget, and keeps the last text that still passes.
It brings the decompile's `pairs` order back to the order the original source would give.
Moving a field whose key and value are both constants emits no instruction (it only goes into the template), so such
fields may be reordered freely; non-constant fields keep their relative order (and a trailing positional item stays last,
it may be a multi-value call). The order is searched with the real LuaJIT parser as the oracle - a placeholder snippet
`return {fields}` is compiled in-process and its template walked with `next` - and the repaired text is proven by the same
strict T1 as everything else. Nothing here can pass a file; it can only make a decompile that T1 then accepts.

The search runs in the vendored luajit, fed on stdin (no .lua file is written); the job is a data file in .build/.
"""
import os
import re
import subprocess
import uuid

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)
LUAJIT_DIR = os.path.join(TRUNK, "elstools", "vendor", "luajit")
LUAJIT = os.path.join(LUAJIT_DIR, "luajit.exe")
TMP = os.path.join(TRUNK, ".build", "luasrc_tmp")

KEYWORDS = set("and break do else elseif end false for function goto if in local nil not or repeat return then true until "
               "while".split())
_LONG = re.compile(r"\[(=*)\[")
_NUM = re.compile(r"0[xX][0-9a-fA-F]*(?:\.[0-9a-fA-F]*)?(?:[pP][+-]?\d+)?|(?:\d+\.?\d*|\.\d+)(?:[eE][+-]?\d+)?")
_NAME = re.compile(r"[A-Za-z_\x80-\xff][A-Za-z0-9_\x80-\xff]*")
_OPS = ("...", "..", "==", "~=", "<=", ">=", "::")


class LexError(Exception):
    pass


class Tok:
    __slots__ = ("kind", "text", "start", "end", "line")

    def __init__(self, kind, text, start, end, line):
        self.kind, self.text, self.start, self.end, self.line = kind, text, start, end, line

    def __repr__(self):
        return "%s(%r)@%d" % (self.kind, self.text[:20], self.line)


def tokenize(s):
    """s: the source as a latin-1 str (one char per byte). -> [Tok]; kinds name / kw / num / str / op."""
    toks, i, n, line = [], 0, len(s), 1
    while i < n:
        c = s[i]
        if c == "\n":
            line += 1
            i += 1
            continue
        if c in " \t\r\f\v":
            i += 1
            continue
        if s.startswith("--", i):
            m = _LONG.match(s, i + 2)
            if m:
                close = "]" + m.group(1) + "]"
                j = s.find(close, m.end())
                if j < 0:
                    raise LexError("unfinished long comment at line %d" % line)
                line += s.count("\n", i, j)
                i = j + len(close)
            else:
                j = s.find("\n", i)
                i = n if j < 0 else j
            continue
        start, l0 = i, line
        if c == "[":
            m = _LONG.match(s, i)
            if m:
                close = "]" + m.group(1) + "]"
                j = s.find(close, m.end())
                if j < 0:
                    raise LexError("unfinished long string at line %d" % line)
                line += s.count("\n", i, j)
                i = j + len(close)
                toks.append(Tok("str", s[start:i], start, i, l0))
                continue
        if c in "\"'":
            j = i + 1
            while True:
                if j >= n:
                    raise LexError("unfinished string at line %d" % line)
                ch = s[j]
                if ch == "\\":
                    if j + 1 < n and s[j + 1] == "\n":
                        line += 1
                    j += 2
                    continue
                if ch == c:
                    break
                if ch == "\n":
                    raise LexError("newline in string at line %d" % line)
                j += 1
            i = j + 1
            toks.append(Tok("str", s[start:i], start, i, l0))
            continue
        if c.isdigit() or (c == "." and i + 1 < n and s[i + 1].isdigit()):
            m = _NUM.match(s, i)
            i = m.end()
            toks.append(Tok("num", s[start:i], start, i, l0))
            continue
        m = _NAME.match(s, i)
        if m:
            i = m.end()
            w = s[start:i]
            toks.append(Tok("kw" if w in KEYWORDS else "name", w, start, i, l0))
            continue
        for op in _OPS:
            if s.startswith(op, i):
                i += len(op)
                break
        else:
            i += 1
        toks.append(Tok("op", s[start:i], start, i, l0))
    return toks


# ----------------------------------------------------------------------------------------------
# table constructors
_OPEN = {"(": ")", "{": "}", "[": "]"}


def match_close(toks, i):
    """toks[i] is ( { or [ -> index of its closing token."""
    depth = 0
    for j in range(i, len(toks)):
        t = toks[j]
        if t.kind == "op" and t.text in "({[":
            depth += 1
        elif t.kind == "op" and t.text in ")}]":
            depth -= 1
            if depth == 0:
                return j
    raise LexError("unbalanced %r at line %d" % (toks[i].text, toks[i].line))


class Field:
    __slots__ = ("a", "b", "kind", "key", "key_const", "val_kind")

    def placeholder(self):
        v = {"nil": "nil", "const": "0", "expr": "x"}[self.val_kind]
        if self.kind == "name":
            return "%s = %s" % (self.key, v)
        if self.kind == "bracket":
            return "[%s] = %s" % (self.key if self.key_const else "x", v)
        return v

    @property
    def movable(self):
        """Emits no instruction: a constant key with a constant value."""
        return self.kind in ("name", "bracket") and self.key_const and self.val_kind in ("const", "nil")

    @property
    def template_const(self):
        return self.key_const and self.val_kind == "const" and self.kind in ("name", "bracket")


def _const_kind(ts):
    if len(ts) == 1:
        t = ts[0]
        if t.kind in ("num", "str") or (t.kind == "kw" and t.text in ("true", "false")):
            return "const"
        if t.kind == "kw" and t.text == "nil":
            return "nil"
    if len(ts) == 2 and ts[0].kind == "op" and ts[0].text == "-" and ts[1].kind == "num":
        return "const"
    return "expr"


def constructor(toks, i, s):
    """toks[i] is '{' -> (index of '}', [Field]) with token index ranges [a, b) per field."""
    close = match_close(toks, i)
    fields, j = [], i + 1
    while j < close:
        a, depth, k = j, 0, j
        while k < close:
            t = toks[k]
            if t.kind == "op" and t.text in "({[":
                depth += 1
            elif t.kind == "op" and t.text in ")}]":
                depth -= 1
            elif depth == 0 and t.kind == "op" and t.text in ",;":
                break
            k += 1
        f = Field()
        f.a, f.b = a, k
        ts = toks[a:k]
        if ts and ts[0].kind == "op" and ts[0].text == "[":
            e = match_close(toks, a)
            key = toks[a + 1:e]
            f.kind, f.key = "bracket", s[key[0].start:key[-1].end] if key else ""
            f.key_const = _const_kind(key) == "const"
            f.val_kind = _const_kind(toks[e + 2:k])
        elif len(ts) >= 2 and ts[0].kind in ("name", "kw") and ts[1].kind == "op" and ts[1].text == "=":
            f.kind, f.key, f.key_const = "name", ts[0].text, True
            f.val_kind = _const_kind(ts[2:])
        else:
            f.kind, f.key, f.key_const = "pos", None, False
            f.val_kind = _const_kind(ts)
        if ts:
            fields.append(f)
        j = k + 1
    return close, fields


# ----------------------------------------------------------------------------------------------
# template layout repair
def lua_lit(b):
    """bytes -> a Lua string literal (decimal escapes for anything not plain printable ASCII)."""
    out = []
    for c in b:
        if 32 <= c < 127 and c not in (34, 92):
            out.append(chr(c))
        else:
            out.append("\\%d" % c)
    return '"' + "".join(out) + '"'


def _num_key(v):
    if v == int(v) and -2147483648 <= v <= 2147483647:
        return "n:%d" % int(v)
    return "n:%.17g" % v


def template_target(entry):
    """The reference template's `next` order (array part, then hash nodes bottom up = the dump's hash part reversed),
    as the key strings the harness prints. Keys only: values are placeholders on the snippet side."""
    import struct
    _tag, _raw, arr, hsh = entry

    def key(k):
        if k[0] == "s":
            return b"s:" + k[1]
        if k[0] == "i":
            v = k[1] - (1 << 32) if k[1] & 0x80000000 else k[1]
            return ("n:%d" % v).encode()
        if k[0] == "n":
            return _num_key(struct.unpack("<d", struct.pack("<II", k[1], k[2]))[0]).encode()
        return {1: b"b:false", 2: b"b:true"}.get(k[1], b"?:nil")
    out = [("n:%d" % i).encode() for i, v in enumerate(arr) if v != ("p", 0)]
    out += [key(k) for k, _v in reversed(hsh)]
    return out


HARNESS = r"""
local ju = require("jit.util")
local jobs = assert(loadfile(arg[1]))()
local MAXEVAL = tonumber(arg[2]) or 400000
local JOBSECS = tonumber(arg[3]) or 20
local TOTALSECS = tonumber(arg[4]) or 180
local T_START = os.clock()
local function tk(v)
  local t = type(v)
  if t == "string" then return "s:" .. v end
  if t == "number" then
    if v == math.floor(v) and v >= -2147483648 and v <= 2147483647 then return string.format("n:%d", v) end
    return string.format("n:%.17g", v)
  end
  return "b:" .. tostring(v)
end
local function template_of(src)
  local f = loadstring(src)
  if not f then return nil end
  local i, t = -1, nil
  while true do
    local k = ju.funck(f, i)
    if k == nil then break end
    if type(k) == "table" then t = k end
    i = i - 1
  end
  return t
end
local function seq_of(fields, order)
  local parts = {}
  for i, ix in ipairs(order) do parts[i] = fields[ix] end
  local t = template_of("return {" .. table.concat(parts, ",\n") .. "}")
  if not t then return nil end
  local out, k = {}, next(t)
  while k ~= nil do out[#out + 1] = tk(k); k = next(t, k) end
  return out
end
local seed = 20261009
local function rnd(n) seed = (seed * 1103515245 + 12345) % 2147483648; return seed % n + 1 end
for ji, job in ipairs(jobs) do
  local t0 = os.clock()
  local fields, mov, target, lastfixed = job.fields, job.mov, job.target, job.lastfixed
  local n, want, evals = #fields, #target, 0
  local nmov = 0
  for i = 1, n do if mov[i] then nmov = nmov + 1 end end
  -- the key each movable field puts in the template (read back from a one-field constructor)
  local fkey, field_of = {}, {}
  for i = 1, n do
    if mov[i] then
      local t = template_of("return {" .. fields[i] .. "}")
      local k = t and next(t)
      if k ~= nil then fkey[i] = tk(k); field_of[fkey[i]] = i end
    end
  end
  local succ, pred = {}, {}
  for i = 1, want - 1 do succ[target[i]] = target[i + 1]; pred[target[i + 1]] = target[i] end
  local function score(s)
    -- agreement of adjacent pairs with the target (robust to a shift), plus the first key; want = exact
    if not s then return -1 end
    local sc = 0
    for i = 1, #s - 1 do if succ[s[i]] == s[i + 1] then sc = sc + 1 end end
    if s[1] == target[1] then sc = sc + 1 end
    if #s ~= want then sc = sc - 2 end
    return sc
  end
  local function exact(s)
    if not s or #s ~= want then return false end
    for i = 1, want do if s[i] ~= target[i] then return false end end
    return true
  end
  local function moved(order, a, b)
    local o = {}
    for i = 1, n do if i ~= a then o[#o + 1] = order[i] end end
    table.insert(o, b, order[a])
    return o
  end
  local function out_of_time() return evals >= MAXEVAL or os.clock() - t0 > JOBSECS or os.clock() - T_START > TOTALSECS end
  local cur = {}
  for i = 1, n do cur[i] = i end
  local cseq = seq_of(fields, cur); evals = evals + 1
  local cs = score(cseq)
  local found = exact(cseq) and cur or nil
  local hi = lastfixed and n - 1 or n
  while not found and not out_of_time() do
    local pos = {}
    for p, ix in ipairs(cur) do pos[ix] = p end
    -- keys whose neighbours disagree with the target go first
    local bad, rest = {}, {}
    local sidx = {}
    for i, k in ipairs(cseq or {}) do sidx[k] = i end
    for ix = 1, n do
      if mov[ix] and fkey[ix] then
        local k, i = fkey[ix], sidx[fkey[ix]]
        if not i or succ[k] ~= (cseq[i + 1]) or pred[k] ~= (cseq[i - 1]) then bad[#bad + 1] = ix else rest[#rest + 1] = ix end
      elseif mov[ix] then rest[#rest + 1] = ix end
    end
    local tries = bad
    if nmov <= 60 then for _, ix in ipairs(rest) do tries[#tries + 1] = ix end end
    local improved = false
    for _, ix in ipairs(tries) do
      local a = pos[ix]
      -- positions next to the target neighbours first, then everything
      local cand, seen = {}, {}
      local function add(b) if b and b >= 1 and b <= hi and b ~= a and not seen[b] then seen[b] = true; cand[#cand + 1] = b end end
      local k = fkey[ix]
      if k then
        local pf, sf = field_of[pred[k] or ""], field_of[succ[k] or ""]
        if pf then local b = pos[pf]; add(b < a and b + 1 or b) end
        if sf then local b = pos[sf]; add(b < a and b or b - 1) end
      end
      for b = 1, hi do add(b) end
      for _, b in ipairs(cand) do
        local o = moved(cur, a, b)
        local s = seq_of(fields, o); evals = evals + 1
        local sc = score(s)
        if exact(s) then found = o; break end
        if sc > cs then cur, cseq, cs, improved = o, s, sc, true; break end
        if out_of_time() then break end
      end
      if found or improved or out_of_time() then break end
    end
    if not found and not improved then
      if nmov > 60 then break end
      -- small constructor: random restart
      local o, ms = {}, {}
      for i = 1, n do if mov[i] then ms[#ms + 1] = i else o[#o + 1] = i end end
      for _, ix in ipairs(ms) do
        local top = #o + 1
        if lastfixed and #o > 0 then top = #o end
        table.insert(o, rnd(top), ix)
      end
      cur = o; cseq = seq_of(fields, cur); evals = evals + 1; cs = score(cseq)
      if exact(cseq) then found = cur end
    end
  end
  if found then
    io.write("J ", ji, " OK ", table.concat(found, " "), "\n")
  else
    io.write("J ", ji, " FAIL ", cs, "/", want, " ", evals, string.format(" %.1fs", os.clock() - t0), "\n")
  end
  io.flush()
end
"""


def search_orders(jobs, max_eval=400000, job_secs=20, total_secs=180):
    """jobs: [(placeholders [str], movable [bool], target [bytes], last_fixed)] -> [order (0-based indices) or None].
    Each constructor gets at most max_eval parser runs and job_secs seconds (total_secs for the whole call); one not solved
    stays as ljd printed it (same pairs, another storage order)."""
    if not jobs:
        return []
    os.makedirs(TMP, exist_ok=True)
    path = os.path.join(TMP, uuid.uuid4().hex + ".job")
    parts = ["return {"]
    for ph, mv, tg, lf in jobs:
        parts.append("{fields={%s},mov={%s},target={%s},lastfixed=%s}," % (
            ",".join(lua_lit(x.encode("latin-1")) for x in ph), ",".join("true" if m else "false" for m in mv),
            ",".join(lua_lit(t) for t in tg), "true" if lf else "false"))
    parts.append("}")
    with open(path, "wb") as f:
        f.write("\n".join(parts).encode("latin-1"))
    try:
        r = subprocess.run([LUAJIT, "-", path, str(max_eval), str(job_secs), str(total_secs)], input=HARNESS.encode(),
                           capture_output=True, cwd=LUAJIT_DIR, timeout=300 + 2 * total_secs)
    finally:
        try:
            os.remove(path)
        except OSError:
            pass
    out = [None] * len(jobs)
    for ln in r.stdout.decode("latin-1").splitlines():
        w = ln.split()
        if len(w) >= 3 and w[0] == "J" and w[2] == "OK":
            out[int(w[1]) - 1] = [int(x) - 1 for x in w[3:]]
    if r.returncode != 0 and not any(out):
        raise RuntimeError("layout search harness failed: %s" % r.stderr.decode("latin-1", "replace")[-300:])
    return out


def brace_index(toks):
    """line -> [index of each `{` that a TDUP stamped with that line may belong to], built once per round. LuaJIT stamps an
    instruction with ls->lastline, the line the PREVIOUS token ended on, so a `{` opened on the next line is listed under its
    predecessor's line (and under its own line as a fallback)."""
    idx = {}
    for i, t in enumerate(toks):
        if t.kind == "op" and t.text == "{":
            prev_end = toks[i - 1].line + toks[i - 1].text.count(chr(10)) if i else 0
            idx.setdefault(prev_end, []).append((i, True))
            if t.line != prev_end:
                idx.setdefault(t.line, []).append((i, False))
    return idx


def _site_constructor(toks, s, line, entry, used, index):
    """The constructor a TDUP stamped with `line` belongs to: among the candidates on that line, the first unused one whose
    constant-field counts match the reference template, preferring a `{` found through its predecessor, in text order."""
    _tag, _raw, arr, hsh = entry
    want = (len(hsh), sum(1 for v in arr if v != ("p", 0)))
    cands = []
    for i, via_prev in index.get(line, ()):
        if i in used:
            continue
        close, fields = constructor(toks, i, s)
        got = (sum(1 for f in fields if f.template_const), sum(1 for f in fields if f.kind == "pos" and f.val_kind == "const"))
        cands.append((got != want, not via_prev, i, close, fields))
    cands.sort(key=lambda c: c[:3])
    if cands and not cands[0][0]:
        return cands[0][2:]
    return None


def repair_layout(text, ref, compile_fn, t1_fn, rounds=2, max_eval=200000, job_secs=5, total_secs=60):
    """text: decompiled source bytes (no BOM); ref: 0x81 reference; compile_fn(text, debug) -> (bc|None, err);
    t1_fn(bc, ref) -> ljbc.T1Result. Returns (text, notes): the last text of the rounds that still passes T1 (the input when
    it does not pass, or nothing improves). The caller proves whatever comes back; this never passes a file."""
    import ljbc
    notes = []
    good = None
    for rnd in range(rounds + 1):
        bc, err = compile_fn(text, False)
        if bc is None:
            notes.append("round %d: does not compile (%s)" % (rnd + 1, err))
            break
        res = t1_fn(bc, ref)
        if not res.ok:
            if rnd == 0:
                notes.append("not T1 (%d difference(s)); no tidy-up" % res.total)
            break
        good = text
        if not res.layout_sites or rnd == rounds:
            break
        gbc, err = compile_fn(text, True)
        if gbc is None:
            notes.append("round %d: -g compile failed (%s)" % (rnd + 1, err))
            return text, notes
        gd = ljbc.parse(gbc)
        s = text.decode("latin-1")
        toks = tokenize(s)
        index = brace_index(toks)
        used, jobs, spans = set(), [], []
        for pidx, pc, entry in res.layout_sites:
            pt = gd.protos[pidx]
            if pt.lines is None:
                continue
            hit = _site_constructor(toks, s, pt.lines[pc], entry, used, index)
            if not hit:
                notes.append("round %d: no constructor found for proto #%d pc %d (line %d)" % (rnd + 1, pidx, pc + 1, pt.lines[pc]))
                continue
            i, close, fields = hit
            used.add(i)
            last_fixed = bool(fields) and fields[-1].kind == "pos"
            jobs.append(([f.placeholder() for f in fields], [f.movable for f in fields], template_target(entry), last_fixed))
            spans.append((toks[i].start, toks[close].end, i, close, fields))
        # rewrite only non-overlapping constructors this round (innermost first); the rest get the next round
        order = sorted(range(len(spans)), key=lambda k: spans[k][1] - spans[k][0])
        chosen, taken = [], []
        for k in order:
            a, b = spans[k][0], spans[k][1]
            if all(b <= x or a >= y for x, y in taken):
                chosen.append(k)
                taken.append((a, b))
        found = search_orders([jobs[k] for k in chosen], max_eval=max_eval, job_secs=job_secs, total_secs=total_secs)
        edits = []
        for k, perm in zip(chosen, found):
            a, b, i, close, fields = spans[k]
            if perm is None:
                notes.append("round %d: no field order reproduces the template at line %d" % (rnd + 1, toks[i].line))
                continue
            slots = [(toks[f.a].start, toks[f.b - 1].end) for f in fields]
            texts = [s[x:y] for x, y in slots]
            for slot, src in zip(slots, perm):
                edits.append((slot[0], slot[1], texts[src]))
        if not edits:
            break
        edits.sort()
        out, pos = [], 0
        for x, y, t in edits:
            out.append(s[pos:x])
            out.append(t)
            pos = y
        out.append(s[pos:])
        text = "".join(out).encode("latin-1")
        notes.append("round %d: reordered %d constructor(s)" % (rnd + 1, sum(1 for p in found if p is not None)))
    return (good if good is not None else text), notes


# ----------------------------------------------------------------------------------------------
# mutation seeds for `luasrc.py selftest` (each kind is an edit T1 must reject)
_BINOPS = {"+": "-", "-": "+", "*": "/", "/": "*", "<": "<=", "<=": "<", ">": ">=", ">=": ">", "==": "~=", "~=": "=="}
_VALUE_END = ("name", "num", "str")
_STMT = re.compile(r"^[ \t]*(local[ \t]+)?[A-Za-z_][\w.:\[\]\"]*([ \t]*=[ \t]*\S.*|\(.*\))[ \t]*$")


def _spread(items, k):
    """Up to k items spread evenly over the list (deterministic)."""
    if len(items) <= k:
        return list(items)
    step = len(items) / float(k)
    return [items[int(i * step)] for i in range(k)]


def _sub(s, a, b, new):
    return s[:a] + new + s[b:]


def _str_body(t):
    """(quote, body start offset in the token, body end offset) for a quoted string token, else None."""
    if t.text[:1] in "\"'":
        return t.text[0], 1, len(t.text) - 1
    return None


def mutants(text, per_kind=3):
    """text: source bytes (no BOM). Yields (kind, where, mutated bytes). Kinds follow LUA_SOURCE_PHASES.md L2 step 5."""
    s = text.decode("latin-1")
    toks = tokenize(s)

    def emit(kind, t, new_s):
        return kind, "line %d" % t.line, new_s.encode("latin-1")

    # 1. a number constant changes
    nums = [t for t in toks if t.kind == "num" and not t.text.lower().startswith("0x")]
    for t in _spread(nums, per_kind):
        v = float(t.text)
        new = str(int(v) + 1) if v == int(v) and "." not in t.text and "e" not in t.text.lower() else repr(v * 1.5 + 0.25)
        yield emit("number-constant", t, _sub(s, t.start, t.end, new))
    # 2. a string constant changes
    strs = [t for t in toks if t.kind == "str" and _str_body(t)]
    for t in _spread(strs, per_kind):
        q, a, b = _str_body(t)
        yield emit("string-constant", t, _sub(s, t.start, t.end, t.text[:b] + "Z" + t.text[b:]))
    # 3. one byte of a string flips (an ASCII letter changes case)
    for t in _spread([t for t in strs if re.search(r"[A-Za-z]", t.text[1:-1])], per_kind):
        q, a, b = _str_body(t)
        body = t.text[a:b]
        i = next(i for i, c in enumerate(body) if c.isalpha() and (i == 0 or body[i - 1] != "\\"))
        nb = body[:i] + chr(ord(body[i]) ^ 0x20) + body[i + 1:]
        yield emit("string-byte-flip", t, _sub(s, t.start, t.end, q + nb + q))
    # 4. a statement is dropped (a one-line assignment or call, not the last field of a constructor)
    lines = s.split("\n")
    cands = []
    for i, ln in enumerate(lines):
        body = ln.rstrip("\r")
        if not _STMT.match(body) or body.rstrip().endswith((",", "{", "(")):
            continue
        nxt = next((x.strip() for x in lines[i + 1:] if x.strip()), "")
        if nxt.startswith(("}", ")")):
            continue
        if sum(body.count(c) for c in "({[") != sum(body.count(c) for c in ")}]"):
            continue
        cands.append(i)
    for i in _spread(cands, per_kind):
        yield "drop-statement", "line %d" % (i + 1), "\n".join(lines[:i] + lines[i + 1:]).encode("latin-1")
    # 5. an operator changes (binary only)
    ops = []
    for j, t in enumerate(toks):
        if j and ((t.kind == "op" and t.text in _BINOPS) or (t.kind == "kw" and t.text in ("and", "or"))):
            pv = toks[j - 1]
            if pv.kind in _VALUE_END or (pv.kind == "op" and pv.text in ")]}") or (pv.kind == "kw" and pv.text in ("nil", "true", "false")):
                ops.append(t)
    for t in _spread(ops, per_kind):
        new = _BINOPS.get(t.text) or {"and": "or", "or": "and"}[t.text]
        yield emit("operator", t, _sub(s, t.start, t.end, new))
    # 6. a table constructor's positional items are reordered (two adjacent, different items swap)
    swaps = []
    for j, t in enumerate(toks):
        if t.kind == "op" and t.text == "{":
            try:
                _close, fields = constructor(toks, j, s)
            except LexError:
                continue
            pos = [f for f in fields if f.kind == "pos"]
            for f, g in zip(pos, pos[1:]):
                fa = s[toks[f.a].start:toks[f.b - 1].end]
                ga = s[toks[g.a].start:toks[g.b - 1].end]
                if fa != ga:
                    swaps.append((toks[f.a], toks[f.a].start, toks[f.b - 1].end, toks[g.a].start, toks[g.b - 1].end))
                    break
    for t, a0, a1, b0, b1 in _spread(swaps, per_kind):
        new = s[:a0] + s[b0:b1] + s[a1:b0] + s[a0:a1] + s[b1:]
        yield emit("constructor-reorder", t, new)
    # 7. a local turns into a global (the ljd slotN class)
    locs = [(j, t) for j, t in enumerate(toks[:-2]) if t.kind == "kw" and t.text == "local" and toks[j + 1].kind == "name"
            and toks[j + 2].kind == "op" and toks[j + 2].text == "="]
    for j, t in _spread(locs, per_kind):
        yield emit("local-to-global", t, _sub(s, t.start, toks[j + 1].start, ""))
    # 8. two different constants of the same type swap places (same multiset, different use)
    for kind, pool in (("swap-string-constants", strs), ("swap-number-constants", nums)):
        pairs = []
        for x, y in zip(pool, pool[1:]):
            if x.text != y.text and y.line - x.line <= 40:
                pairs.append((x, y))
        for x, y in _spread(pairs, per_kind):
            new = s[:x.start] + y.text + s[x.end:y.start] + x.text + s[y.end:]
            yield emit(kind, x, new)
