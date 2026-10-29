#!/usr/bin/env python
"""junecollapse.py - apply one batch of june2017_re/diverge/collapse_map.tsv (JUNE_MARCH_DIVERGENCE_PHASES.md, Phase C0).

    python tools/junecollapse.py apply C1 --dry-run                 # plan the whole batch in memory, write nothing, print the counts
    python tools/junecollapse.py apply C1 --dry-run --files X2Lib/X2GuEve.cpp X2Lib/X2GuEve.h
    python tools/junecollapse.py apply C1                           # write the files (all or nothing: one failed check writes none)
    python tools/junecollapse.py apply C1 --report out.tsv          # also write one row per map row: status, reason, sites
    python tools/junecollapse.py --selftest

Byte level and CP949-safe: a file is never decoded. Comments and string literals are blanked in a same-length mask, so a match is
always an identifier in code; every edit is a splice of the original bytes. Identifiers are ASCII; the Korean bytes stay put.

Actions (collapse_map.tsv `action`):
  merge      move June's body into March's twin under the June body's flag (the studio pattern):
                 X(...) { #ifdef F <June body> #else <March body> #endif F }
             delete the June definition and its declaration, collapse the plain call-site wrappers
                 #ifdef F  June_X(...)  #else  X(...)  #endif      ->   X(...)
             and rename the remaining June_X references. Anything else (mixed flags, differing arguments, an inline definition in a
             header, a virtual, a note naming a hand shape) is listed for the phase to do with bytepatch.py.
  rename     whole-identifier, code only (never comments), refused when the new name already exists in the class, in any base class
             (read from the headers), or in the files it would touch.
  enum       a JUNE_* alias -> the qualified member in `new`; the alias definition line goes.
  namespace  / class: whole-identifier rename. lua: code identifier and the exact string literal. file: git mv + vcxproj / filters
             + #include. drop: delete a member and its init. keep: nothing.
A name used by rows of several classes is renamed only inside the row's own file (and its header); elsewhere it is reported.

After each file: flag-off equivalence (flagcheck.strip with every SERV_IRUHADEV_JUNE_* flag undefined, new vs the base revision,
byte for byte) and an encoding-class check. A failing file means nothing is written.
"""
import argparse
import bisect
import collections
import csv
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..'))
sys.path.insert(0, HERE)
import flagcheck  # noqa: E402

MAP_PATH = os.path.join(TRUNK, 'june2017_re', 'diverge', 'collapse_map.tsv')
SRC_DIRS = ('X2Lib', 'KTDXLIB', 'X2', 'KncWX2Server/Common')     # Common: the EGS_ / KEGS_ packet names are renamed client-side (C4)
SRC_EXT = ('.cpp', '.h', '.inl', '.hpp', '.c')
SKIP_DIRS = {'debug', 'release', 'ipch', 'us_service', 'us_internal', 'us_test', 'x2tool', '.svn', 'cximage', 'fmod', 'tinyxml'}
PROJ_FILES = ('X2Lib/X2Lib_2010.vcxproj', 'X2Lib/X2Lib_2010.vcxproj.filters', 'KTDXLIB/KTDXLIB_2010.vcxproj',
              'KTDXLIB/KTDXLIB_2010.vcxproj.filters', 'X2/X2_2010.vcxproj', 'X2/X2_2010.vcxproj.filters')
HAND_NOTE_WORDS = ('hand shape', 'signature', 'virtual', 'vslot', 'forwarder', 'copy')
JUNE_FLAG_RX = re.compile(rb'SERV_IRUHADEV_JUNE_\w+')
KEYWORDS = set(b'''return else case new delete throw sizeof goto do if for while switch typedef using namespace public private protected
virtual static extern inline const volatile operator'''.split())
PRIM = set(b'int float double char short long bool void unsigned signed wchar_t const'.split())


# ------------------------------------------------------------------------------------------------ file systems

class DiskFS(object):
    def __init__(self, root, rev='HEAD'):
        self.root, self.rev = root, rev
        self._cache = {}

    def files(self):
        out = []
        for d in SRC_DIRS:
            for dp, dn, fn in os.walk(os.path.join(self.root, d)):
                dn[:] = [x for x in dn if x.lower() not in SKIP_DIRS]
                for f in fn:
                    if f.lower().endswith(SRC_EXT):
                        out.append(os.path.relpath(os.path.join(dp, f), self.root).replace('\\', '/'))
        return sorted(out)

    def read(self, rel):
        if rel not in self._cache:
            with open(os.path.join(self.root, rel), 'rb') as f:
                self._cache[rel] = f.read()
        return self._cache[rel]

    def exists(self, rel):
        return os.path.exists(os.path.join(self.root, rel))

    def write(self, rel, data):
        with open(os.path.join(self.root, rel), 'wb') as f:
            f.write(data)
        self._cache[rel] = data

    def base(self, rel):
        r = subprocess.run(['git', 'show', '%s:%s' % (self.rev, rel)], cwd=self.root, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        return r.stdout if r.returncode == 0 else None

    def rename(self, old, new):
        r = subprocess.run(['git', 'mv', old, new], cwd=self.root, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if r.returncode:
            raise RuntimeError('git mv %s %s: %s' % (old, new, r.stderr.decode('latin-1').strip()))
        self._cache.pop(old, None)


class MemFS(object):
    def __init__(self, files):
        self.f = dict(files)
        self.orig = dict(files)

    def files(self):
        return sorted(k for k in self.f if k.lower().endswith(SRC_EXT))

    def read(self, rel):
        return self.f[rel]

    def exists(self, rel):
        return rel in self.f

    def write(self, rel, data):
        self.f[rel] = data

    def base(self, rel):
        return self.orig.get(rel)

    def rename(self, old, new):
        self.f[new] = self.f.pop(old)


# ------------------------------------------------------------------------------------------------ source model

_TOK_RX = re.compile(rb'(//[^\n]*)|(/\*.*?\*/)|([LuU]?"(?:[^"\\\n]|\\.)*")|(\'(?:[^\'\\\n]|\\.)*\')', re.S)
_BLANK_RX = re.compile(rb'[^\r\n]')
DIR_RX = re.compile(rb'^[ \t]*#[ \t]*(ifdef|ifndef|if|elif|else|endif)\b(.*)$')
ID_RX = re.compile(rb'[A-Za-z_][A-Za-z0-9_]*')
CMP_TOK = re.compile(rb'[LuU]?"(?:[^"\\\n]|\\.)*"|\'(?:[^\'\\\n]|\\.)*\'|[A-Za-z_]\w*|\d+\.?\w*|\S')


def _masks(data):
    """(code mask: comments and literals blanked, comment-free copy: only comments blanked); both the length of data."""
    full, nocom = [], []
    pos = 0
    for m in _TOK_RX.finditer(data):
        s, e = m.span()
        full.append(data[pos:s]); nocom.append(data[pos:s])
        blank = _BLANK_RX.sub(b' ', m.group())
        full.append(blank)
        nocom.append(blank if m.group(1) is not None or m.group(2) is not None else m.group())
        pos = e
    full.append(data[pos:]); nocom.append(data[pos:])
    return b''.join(full), b''.join(nocom)


class Frame(object):
    __slots__ = ('start', 'kind', 'rest', 'else_line', 'end', 'june', 'simple')

    def __init__(self, start, kind, rest):
        self.start, self.kind, self.rest = start, kind, rest
        self.else_line = self.end = None
        self.june = set(JUNE_FLAG_RX.findall(rest))
        s = rest.strip()
        self.simple = s.split()[0] if (kind == b'ifdef' and s) else None


class Src(object):
    def __init__(self, data):
        self.data = data
        self.mask, self.nocom = _masks(data)
        self.lines = data.splitlines(True)
        self.starts = []
        o = 0
        for l in self.lines:
            self.starts.append(o)
            o += len(l)
        self.starts.append(o)
        self._frames = None
        self._idx = None

    def index(self):
        """identifier -> [start offsets] in code (built once)."""
        if self._idx is None:
            d = collections.defaultdict(list)
            for m in ID_RX.finditer(self.mask):
                d[m.group()].append(m.start())
            self._idx = d
        return self._idx

    def n(self):
        return len(self.lines)

    def line_of(self, off):
        return bisect.bisect_right(self.starts, off) - 1

    def mline(self, i):
        return self.mask[self.starts[i]:self.starts[i + 1]]

    def eol(self, i):
        l = self.lines[i]
        return b'\r\n' if l.endswith(b'\r\n') else (b'\n' if l.endswith(b'\n') else b'\r' if l.endswith(b'\r') else b'')

    def frames(self):
        if self._frames is None:
            stack, allf, per = [], [], [()] * self.n()
            for i in range(self.n()):
                m = DIR_RX.match(self.mline(i).rstrip(b'\r\n'))
                if m:
                    kw = m.group(1)
                    if kw in (b'ifdef', b'ifndef', b'if'):
                        f = Frame(i, kw, m.group(2))
                        stack.append(f)
                        allf.append(f)
                    elif kw in (b'else', b'elif'):
                        if stack and stack[-1].else_line is None:
                            stack[-1].else_line = i
                    elif stack:
                        stack.pop().end = i
                per[i] = tuple(stack)
            self._frames = (allf, per)
        return self._frames

    def stack_at(self, line):
        return self.frames()[1][line]


def match_paren(mask, lp):
    d = 0
    for i in range(lp, min(len(mask), lp + 20000)):
        c = mask[i]
        if c == 0x28:
            d += 1
        elif c == 0x29:
            d -= 1
            if d == 0:
                return i
    return -1


_BR_RX = re.compile(rb'^[ \t]*#[ \t]*(if|ifdef|ifndef|elif|else|endif)\b|[{}]', re.M)


def match_brace(mask, lb, info=None):
    """Offset of the `}` closing the `{` at lb. Both branches of an #if/#else count from the same depth (the first branch wins);
    info['split'] is set when the branches end at different depths (braces split across #if branches)."""
    depth, stack = 0, []
    for m in _BR_RX.finditer(mask, lb):
        t = m.group()
        if t == b'{':
            depth += 1
        elif t == b'}':
            depth -= 1
            if depth == 0:
                if stack and info is not None:
                    info['split'] = True      # the closing brace sits inside a conditional opened within the body
                return m.start()
        else:
            kw = m.group(1)
            if kw in (b'if', b'ifdef', b'ifndef'):
                stack.append([depth, None])
            elif kw in (b'else', b'elif') and stack:
                if stack[-1][1] is None:
                    stack[-1][1] = depth
                depth = stack[-1][0]
            elif kw == b'endif' and stack:
                top = stack.pop()
                if top[1] is not None:
                    if info is not None and depth != top[1]:
                        info['split'] = True
                    depth = top[1]
    return -1


def split_top(text, sep=b','):
    out, d, cur = [], 0, []
    prev = b''
    for i in range(len(text)):
        c = text[i:i + 1]
        if c in (b'(', b'[', b'{') or (c == b'<' and prev.isalnum()):
            d += 1
        elif c in (b')', b']', b'}') or (c == b'>' and d > 0 and text[i - 1:i] != b'-'):
            d -= 1
        if c == sep and d == 0:
            out.append(b''.join(cur)); cur = []
        else:
            cur.append(c)
        if not c.isspace():
            prev = c
    out.append(b''.join(cur))
    return out


def parse_params(masked):
    """[(type, name|None, default|None)] from the masked text between the parentheses."""
    txt = b' '.join(l for l in masked.split(b'\n') if not l.lstrip().startswith(b'#'))
    txt = re.sub(rb'\s+', b' ', txt).strip()
    if txt in (b'', b'void'):
        return []
    out = []
    for p in split_top(txt):
        parts = split_top(p, b'=')
        decl = parts[0].strip()
        dflt = re.sub(rb'\s+', b' ', b'='.join(parts[1:]).strip()) if len(parts) > 1 else None
        decl = re.sub(rb'\[\s*\w*\s*\]\s*$', b'', decl)
        toks = re.findall(rb'[A-Za-z_]\w*|::|[*&<>,]|\d+', decl)
        name = None
        if len(toks) >= 2 and re.match(rb'[A-Za-z_]', toks[-1]) and toks[-1] not in PRIM and (
                re.match(rb'[A-Za-z_]', toks[-2]) or toks[-2] in (b'*', b'&', b'>')) and toks[-2] != b'::':
            if not (toks[-2] in PRIM and all(t in PRIM for t in toks[:-1]) and toks[-1] in PRIM):
                name = toks[-1]
                toks = toks[:-1]
        out.append((b' '.join(toks), name, dflt))
    return out


class Def(object):
    pass


def _qual_rx(qual, name):
    if qual:
        q = rb'\s*::\s*'.join(re.escape(p.encode()) for p in qual.split('::'))
        return re.compile(rb'(?<![\w.>:])' + q + rb'\s*::\s*' + re.escape(name.encode()) + rb'(?![\w])\s*\(')
    return re.compile(rb'(?<![\w.>:~])' + re.escape(name.encode()) + rb'(?![\w])\s*\(')


def find_defs(src, qual, name):
    """Function definitions `qual::name( ... ) { ... }` (variants of one signature under #if / #else share one Def)."""
    mask = src.mask
    found = collections.OrderedDict()
    nb = name.encode()
    tail = None
    if qual:
        tail = re.compile(rb'(?<![\w.>:])' + rb'\s*::\s*'.join(re.escape(p.encode()) for p in qual.split('::')) + rb'\s*::\s*$')
    for pos in src.index().get(nb, ()):
        mm = re.compile(rb'\s*\(').match(mask, pos + len(nb))
        if not mm:
            continue
        if tail is not None:
            lo = max(0, pos - 160)
            t = tail.search(mask[lo:pos])
            if not t:
                continue
            start = lo + t.start()
        else:
            if mask[max(0, pos - 1):pos] in (b'.', b'>', b':', b'~'):
                continue
            start = pos
        lp = mm.end() - 1
        rp = match_paren(mask, lp)
        if rp < 0:
            continue
        if not qual:
            pre = mask[max(0, start - 80):start].rstrip()
            lastline = pre.split(b'\n')[-1].strip() if pre else b''
            tk = re.findall(rb'[A-Za-z_]\w*|[*&>]', pre[-40:])
            if not tk or (tk[-1] in KEYWORDS) or lastline.endswith((b'=', b'(', b',', b'.', b'->', b'!', b'?', b':', b'&&', b'||', b'+', b'-', b'<<')):
                continue
        i, d, lim, brace = rp + 1, 0, min(len(mask), rp + 3000), -1
        while i < lim:
            c = mask[i]
            if c == 0x7b and d == 0:
                brace = i
                break
            if c == 0x3b and d == 0:
                break
            if c == 0x28:
                d += 1
            elif c == 0x29:
                d -= 1
                if d < 0:
                    break
            elif c == 0x23 and (i == 0 or mask[i - 1] in (0x0a, 0x20, 0x09)) and mask[src.starts[src.line_of(i)]:i].strip() == b'':
                i = mask.find(b'\n', i)
                if i < 0:
                    break
            i += 1
        if brace < 0:
            continue
        if brace not in found:
            df = Def()
            df.variants, df.brace, df.name_off = [], brace, start
            found[brace] = df
        found[brace].variants.append((lp, rp, start))
    out = []
    for brace, df in found.items():
        info = {}
        bc = match_brace(mask, brace, info)
        if bc < 0:
            continue
        df.split = bool(info.get('split'))
        df.bclose = bc
        df.first_line = src.line_of(df.variants[0][2])
        df.brace_line = src.line_of(brace)
        df.close_line = src.line_of(bc)
        # a signature-variant block (#ifdef X sig #else sig #endif) right above the first signature line belongs to the definition
        ls = df.first_line
        lo = src.line_of(df.variants[0][1])
        for f in src.frames()[0]:
            if f.end is not None and ls - 3 <= f.start < ls < f.end and lo <= f.end <= df.brace_line:
                ls = min(ls, f.start)
        df.sig_line = ls
        a = src.starts[df.first_line]
        df.ret = re.sub(rb'\s+', b' ', mask[a:df.variants[0][2]]).strip()
        df.ret = re.sub(rb'\b(inline|static|virtual)\b\s*', b'', df.ret).strip()
        out.append(df)
    return out


def def_signature(src, df, variant=0):
    lp, rp, _ = df.variants[variant]
    return parse_params(src.mask[lp + 1:rp])


# ------------------------------------------------------------------------------------------------ class hierarchy

_CLASS_RX = re.compile(rb'\b(?:class|struct)\s+(?:[A-Z][A-Z0-9_]*\s+)?([A-Za-z_]\w*)\s*(?::\s*([^{;]*))?\{')


class Hierarchy(object):
    def __init__(self, fs, rels=None):
        self.fs = fs
        self.classes = collections.defaultdict(list)    # name -> [(rel, body_start, body_end, bases)]
        for rel in (rels if rels is not None else [r for r in fs.files() if r.lower().endswith(('.h', '.inl', '.hpp'))]):
            if not rel.lower().endswith(('.h', '.inl', '.hpp')):
                continue
            data = fs.read(rel)
            if b'class' not in data and b'struct' not in data:
                continue
            src = Src(data)
            for m in _CLASS_RX.finditer(src.mask):
                be = match_brace(src.mask, m.end() - 1)
                if be < 0:
                    continue
                bases = []
                for b in split_top(m.group(2) or b''):
                    b = re.sub(rb'\b(public|protected|private|virtual)\b', b'', b)
                    b = re.sub(rb'<.*', b'', b, flags=re.S)
                    nm = re.findall(rb'[A-Za-z_]\w*', b)
                    if nm:
                        bases.append(nm[-1].decode())
                self.classes[m.group(1).decode()].append((rel, m.end(), be, bases, src))

    def lookup(self, path):
        """The (rel, start, end, bases, src) for `Outer::Inner` (the inner class sitting inside Outer's body when ambiguous)."""
        parts = path.split('::')
        cands = self.classes.get(parts[-1], [])
        if len(parts) > 1 and len(cands) > 1:
            for o in self.classes.get(parts[-2], []):
                for c in cands:
                    if c[0] == o[0] and o[1] <= c[1] < o[2]:
                        return c
        return cands[0] if cands else None

    @staticmethod
    def declared(entry):
        rel, bs, be, bases, src = entry
        mask = src.mask[bs:be]
        names = set()
        depth = pdepth = 0
        for m in re.finditer(rb'[A-Za-z_]\w*|[{}()]', mask):
            t = m.group()
            if t == b'{':
                depth += 1
            elif t == b'}':
                depth -= 1
            elif t == b'(':
                pdepth += 1
            elif t == b')':
                pdepth -= 1
            elif depth == 0 and pdepth == 0 and t not in KEYWORDS:
                if re.match(rb'\s*[(;=\[,:]', mask[m.end():m.end() + 8]):
                    names.add(t)
        for m in re.finditer(rb'\b(?:class|struct|enum|union)\s+([A-Za-z_]\w*)', mask):
            names.add(m.group(1))
        for m in re.finditer(rb'\benum\b[^{;]*\{([^}]*)\}', mask):
            for ent in m.group(1).split(b','):
                n = re.match(rb'\s*([A-Za-z_]\w*)', ent)
                if n:
                    names.add(n.group(1))
        return names

    def chain(self, path, seen=None):
        e = self.lookup(path)
        seen = seen if seen is not None else set()
        if e is None or path in seen:
            return []
        seen.add(path)
        out = [(path, e)]
        for b in e[3]:
            out += self.chain(b, seen)
        return out

    def collision(self, path, newname):
        nb = newname.encode()
        for p, e in self.chain(path):
            if nb in self.declared(e):
                return p
        return None


# ------------------------------------------------------------------------------------------------ editing

class Editor(object):
    """Line deletions and byte insertions on one Src; empty `#ifdef JUNE_FLAG ... #endif` frames it emptied go too."""

    def __init__(self, src):
        self.src = src
        self.del_lines = set()
        self.ins = []
        self.splices = []

    def delete_lines(self, a, b):
        self.del_lines.update(range(a, b))

    def insert(self, off, data):
        self.ins.append((off, data))

    def splice(self, s, e, data):
        self.splices.append((s, e, data))

    def _drop_empty_frames(self):
        src = self.src
        changed = True
        while changed:
            changed = False
            for f in sorted(src.frames()[0], key=lambda x: (x.end or 0) - x.start):
                if f.end is None or f.else_line is not None or not f.june or f.kind != b'ifdef':
                    continue
                rng = set(range(f.start, f.end + 1))
                if rng <= self.del_lines:
                    continue
                inner = [i for i in range(f.start + 1, f.end) if i not in self.del_lines]
                if inner and not (set(range(f.start + 1, f.end)) & self.del_lines):
                    continue
                if all(not src.mline(i).strip() for i in inner) and (set(range(f.start + 1, f.end)) & self.del_lines):
                    self.del_lines |= rng
                    changed = True

    def result(self):
        self._drop_empty_frames()
        src = self.src
        sp = list(self.splices)
        ln = sorted(self.del_lines)
        i = 0
        while i < len(ln):
            j = i
            while j + 1 < len(ln) and ln[j + 1] == ln[j] + 1:
                j += 1
            sp.append((src.starts[ln[i]], src.starts[ln[j] + 1], b''))
            i = j + 1
        for off, d in self.ins:
            sp.append((off, off, d))
        sp.sort(key=lambda x: (x[0], x[1]))
        out, pos = [], 0
        for s, e, d in sp:
            if s < pos:
                raise ValueError('overlapping edits at byte %d' % s)
            out.append(src.data[pos:s]); out.append(d)
            pos = e
        out.append(src.data[pos:])
        return b''.join(out)


def sub_idents(data, mapping):
    """Whole-identifier replacement in code (comments and literals untouched)."""
    mask, _ = _masks(data)
    out, pos, n = [], 0, 0
    for m in ID_RX.finditer(mask):
        r = mapping.get(m.group())
        if r is not None:
            out.append(data[pos:m.start()]); out.append(r)
            pos = m.end(); n += 1
    out.append(data[pos:])
    return b''.join(out), n


def to_eol(chunk, eol):
    return b''.join(l.rstrip(b'\r\n') + eol for l in chunk.splitlines(True))


# ------------------------------------------------------------------------------------------------ the collapse

class Row(object):
    def __init__(self, d):
        self.__dict__.update(d)
        self.old, self.action, self.file, self.cls = d['old'], d['action'], d['file'], d['class']
        self.status, self.reason, self.sites = 'pending', '', []

    def hand(self, why):
        self.status, self.reason = 'hand', why

    def ok(self, note=''):
        self.status, self.reason = 'auto', note


def load_map(path=MAP_PATH):
    with open(path, 'r', encoding='utf-8', newline='') as f:
        rd = csv.reader(f, delimiter='\t')
        hdr = next(rd)
        hdr[0] = hdr[0].lstrip('# ')
        return [Row(dict(zip(hdr, r))) for r in rd if r]


def qual_of(cls):
    return '' if (not cls or cls.startswith('_CONST_')) else cls


class Collapse(object):
    def __init__(self, fs, rows, batch, files=None, only=None, log=print):
        self.fs, self.log = fs, log
        self.all_rows = rows
        self.rows = [r for r in rows if r.batch == batch and (not files or any(r.file.endswith(x) or x.endswith(r.file) for x in files))
                     and (not only or r.old in only)]
        by_old = collections.defaultdict(list)
        for r in rows:
            by_old[r.old].append(r)
        self.by_old = by_old
        self.out = {}            # rel -> new bytes
        self._srcs = {}
        self.hier = None
        self.allow_existing = False     # --allow-existing: finish a rename that earlier rows left half done (the new name is already in use)
        self.scope_locals = False       # --scope-locals: a local's new name only has to be free in its own function (column-0 braces), not in the whole tree
        self._claims = collections.defaultdict(list)

    # -- scope of a name: which files a row's old name may be renamed in
    def shared(self, r):
        return len(set(x.cls for x in self.by_old[r.old] if x.action != 'keep')) > 1

    @staticmethod
    def stem(rel):
        return os.path.splitext(os.path.basename(rel))[0].lower()

    def in_scope(self, r, rel):
        if not self.shared(r):
            return True
        owners = set(self.stem(x.file) for x in self.by_old[r.old] if x.cls == r.cls and x.file)
        return self.stem(rel) in owners

    def cur(self, rel):
        return self.out.get(rel, self.fs.read(rel))

    def src_of(self, rel):
        d = self.cur(rel)
        c = self._srcs.get(rel)
        if c is None or c[0] is not d:
            c = (d, Src(d))
            self._srcs[rel] = c
        return c[1]

    # -- run
    def run(self):
        rows = self.rows
        for r in rows:
            if r.action == 'keep':
                r.ok('already the rule form')
            elif not r.file:
                r.hand('unresolved: the census could not scope it (%s)' % r.note[:80])
            elif any(w in r.note for w in HAND_NOTE_WORDS):
                r.hand('note: ' + r.note[:110])
        olds = set(r.old.encode() for r in rows if r.status == 'pending')
        cand = []
        for rel in self.fs.files():
            d = self.fs.read(rel)
            if any(o in d for o in olds):
                cand.append(rel)
        self.cand = cand
        self.hier = Hierarchy(self.fs)
        self.check_collisions()
        self.pass_wrappers()
        self.pass_merges()
        self.report_mixed()
        self.pass_renames()
        self.pass_others()
        self.pass_files()
        return self.verify()

    # -- collisions: a rename may not meet an existing name
    def check_collisions(self):
        seen = {}
        for r in self.rows:
            if r.status != 'pending' or r.action not in ('rename', 'namespace', 'class', 'lua'):
                continue
            if '::' in r.new:
                continue
            if self.scope_locals and r.kind == 'local':
                why = self.local_conflict(r)
                if why:
                    r.hand('refused: ' + why)
                continue
            key = (r.cls, r.new, r.kind in ('local',))
            if key in seen and seen[key] is not r.old:
                r.hand('two rows of this batch rename to %s::%s (%s and %s)' % (r.cls, r.new, seen[key], r.old))
                continue
            seen[key] = r.old
            if r.kind in ('member function', 'data member') and qual_of(r.cls):
                hit = self.hier.collision(r.cls, r.new)
                if hit:
                    r.hand('refused: %s already declares %s%s' % (hit, r.new, '' if hit == r.cls else ' (base class of %s)' % r.cls))
                    continue
            elif not self.allow_existing:
                where = self.name_exists(r)
                if where:
                    r.hand('refused: %s already used in %s' % (r.new, where))

    @staticmethod
    def exclusive(src, l1, l2):
        """True when lines l1 and l2 sit on different branches of one #ifdef / #else (they never compile together)."""
        s2 = src.stack_at(l2)
        for f in src.stack_at(l1):
            if f in s2 and f.else_line is not None and (l1 < f.else_line) != (l2 < f.else_line):
                return True
        return False

    def local_conflict(self, r):
        """--scope-locals: why a local cannot take its new name (used in the same function on a branch the old name can meet), else None."""
        ob, nb = r.old.encode(), r.new.encode()
        nrx = re.compile(rb'(?<![\w.>:])' + re.escape(nb) + rb'(?![\w])')
        claims = []
        for rel in self.cand:
            if not self.in_scope(r, rel):
                continue
            src = self.src_of(rel)
            m = src.mask
            for pos in src.index().get(ob, ()):
                a = m.rfind(b'\n}', 0, pos)
                a = 0 if a < 0 else a + 2
                b = m.find(b'\n}', pos)
                b = len(m) if b < 0 else b
                pl = src.line_of(pos)
                for q in nrx.finditer(m, a, b):
                    if not self.exclusive(src, pl, src.line_of(q.start())):
                        return '%s used in %s:%d (same function)' % (r.new, rel, src.line_of(q.start()) + 1)
                for o2, l2 in self._claims.get((rel, a, r.new), ()):
                    if o2 != r.old and not self.exclusive(src, pl, l2):
                        return '%s taken by %s in the same function (%s)' % (r.new, o2, rel)
                claims.append(((rel, a, r.new), (r.old, pl)))
        for k, v in claims:
            self._claims[k].append(v)
        return None
    def name_exists(self, r):
        nb = r.new.encode()
        rx = re.compile(rb'(?<![\w.>:])' + re.escape(nb) + rb'(?![\w])')
        for rel in self.cand:
            if not self.in_scope(r, rel) and r.action == 'rename':
                continue
            if rx.search(self.src_of(rel).mask):
                return rel
        return None

    # -- wrappers: #ifdef F  June_X(..)  #else  X(..)  #endif   ->   X(..)
    def pass_wrappers(self):
        merge = {}
        for r in self.rows:
            if r.action == 'merge' and r.status == 'pending':
                merge.setdefault(r.old.encode(), []).append(r)
        if not merge:
            return
        for rel in self.cand:
            mine = dict((o, [r for r in rs if self.in_scope(r, rel)]) for o, rs in merge.items())
            mine = dict((o, rs) for o, rs in mine.items() if rs)
            if not mine:
                continue
            orx = re.compile(rb'(?<![A-Za-z0-9_])(?:' + b'|'.join(re.escape(o) for o in sorted(mine, key=len, reverse=True)) + rb')(?![A-Za-z0-9_])')
            allmap = dict((o, rs[0].new.rsplit('::', 1)[-1].encode()) for o, rs in mine.items())
            data = self.cur(rel)
            if not orx.search(data):
                continue
            for _ in range(6):
                src = Src(data)
                ed = Editor(src)
                taken = []
                for f in src.frames()[0]:
                    if f.kind != b'ifdef' or f.else_line is None or f.end is None or not f.june or f.simple is None:
                        continue
                    if DIR_RX.match(src.mline(f.else_line).rstrip(b'\r\n')).group(1) != b'else':
                        continue
                    a, b, c = src.starts[f.start + 1], src.starts[f.else_line], src.starts[f.else_line + 1]
                    z = src.starts[f.end]
                    if not orx.search(src.mask[a:b]):
                        continue
                    t1 = [allmap.get(t, t) for t in CMP_TOK.findall(src.nocom[a:b])]
                    t2 = CMP_TOK.findall(src.nocom[c:z])
                    if t1 == t2 and not any(s_ <= src.starts[f.start] < e_ for s_, e_ in taken):
                        ed.splice(src.starts[f.start], src.starts[f.end + 1], src.data[c:z])
                        taken.append((src.starts[f.start], src.starts[f.end + 1]))
                if not taken:
                    break
                data = ed.result()
            if data != self.fs.read(rel):
                self.out[rel] = data

    def report_mixed(self):
        """June branches that still call a merged name beside a different March branch (listed for the phase)."""
        merge = {}
        for r in self.rows:
            if r.action == 'merge' and r.status != 'pending':
                merge.setdefault(r.old.encode(), []).append(r)
        for rel in self.cand:
            mine = dict((o, [r for r in rs if self.in_scope(r, rel)]) for o, rs in merge.items())
            mine = dict((o, rs) for o, rs in mine.items() if rs)
            if not mine:
                continue
            orx = re.compile(rb'(?<![A-Za-z0-9_])(?:' + b'|'.join(re.escape(o) for o in sorted(mine, key=len, reverse=True)) + rb')(?![A-Za-z0-9_])')
            src = Src(self.cur(rel))
            for f in src.frames()[0]:
                if f.kind != b'ifdef' or f.else_line is None or f.end is None or not f.june:
                    continue
                if f.else_line - f.start > 13:      # a whole June alternative of a block, not a call-site wrapper
                    continue
                for m in orx.finditer(src.mask, src.starts[f.start + 1], src.starts[f.else_line]):
                    mine[m.group()][0].sites.append('%s:%d mixed call site (#ifdef %s branch differs from #else)' % (
                        rel, src.line_of(m.start()) + 1, (f.simple or b'?').decode()))

    # -- merges
    def pass_merges(self):
        byfile = collections.defaultdict(list)
        for r in self.rows:
            if r.action == 'merge' and r.status == 'pending':
                if r.file.lower().endswith(('.h', '.inl')):
                    r.hand('inline definition in a header: hand shape')
                else:
                    byfile[r.file].append(r)
        for rel, rs in sorted(byfile.items()):
            src = Src(self.cur(rel))
            ed = Editor(src)
            moved_spans = []
            for r in rs:
                self.plan_merge(r, rel, src, ed, moved_spans)
            if any(r.status == 'auto' for r in rs):
                self.out[rel] = ed.result()
        # declarations of the merged functions in the headers
        merged = [r for r in self.rows if r.action == 'merge' and r.status == 'auto']
        if merged:
            self.drop_decls(merged)

    def plan_merge(self, r, rel, src, ed, spans):
        qj = qual_of(r.cls)
        if '::' in r.new:
            q2, n2 = r.new.rsplit('::', 1)
            if q2 != r.cls:
                r.hand('merges into %s, another class' % q2)
                return
        else:
            q2, n2 = qj, r.new
        jd = find_defs(src, qj, r.old)
        if len(jd) != 1:
            r.hand('June definition not found once in %s (%d found)' % (rel, len(jd)))
            return
        jd = jd[0]
        md = find_defs(src, q2, n2)
        if not md:
            r.hand('March twin %s not defined in %s' % (n2, rel))
            return
        jsig = def_signature(src, jd)
        pick, pv = None, 0
        for d in md:
            for v in range(len(d.variants)):
                ms = def_signature(src, d, v)
                if [p[0] for p in ms] == [p[0] for p in jsig]:
                    pick, pv = d, v
                    break
            if pick:
                break
        if pick is None:
            r.hand('no March overload of %s has the parameter types of the June function' % n2)
            return
        if pick.ret != jd.ret:
            r.hand('return type differs: %r vs %r' % (jd.ret.decode('latin-1'), pick.ret.decode('latin-1')))
            return
        deleg = None
        if jd.split:
            r.hand('June definition splits its braces across #if branches: hand shape')
            return
        if pick.split:
            deleg = self.delegating(r, src, pick)
            if deleg is None:
                r.hand('March definition splits its braces across #if branches and is not a plain delegating wrapper: hand shape')
                return
        for d, nm in ((jd, 'June'), (pick, 'March')):
            if d is pick and deleg:
                continue
            if src.mline(d.brace_line).strip() != b'{' or src.mline(d.close_line).strip() != b'}':
                r.hand('%s definition: the braces are not alone on their lines' % nm)
                return
        # the flag frame the June definition sits in
        frame = None
        for f in reversed(src.stack_at(jd.first_line)):
            if f.kind == b'ifdef' and r.flag.encode() == f.simple and (f.else_line is None or jd.first_line < f.else_line) and f.end is not None:
                frame = f
                break
        if frame is None:
            r.hand('June definition is not inside #ifdef %s' % r.flag)
            return
        if not (frame.start < jd.sig_line and jd.close_line < frame.end):
            r.hand('June definition leaves its #ifdef %s frame' % r.flag)
            return
        if any(f.start < pick.sig_line and f.end and f.end > pick.close_line and r.flag.encode() in f.june for f in src.stack_at(pick.first_line)):
            r.hand('March twin is itself inside #ifdef %s' % r.flag)
            return
        for lo, hi in spans:
            if lo <= jd.close_line and jd.sig_line < hi:
                r.hand('overlaps another merge')
                return
        # parameter names: June's body is read in March's names
        msig = def_signature(src, pick, pv)
        ren = {}
        for jp, mp in zip(jsig, msig):
            if jp[1] != mp[1]:
                if jp[1] is None or mp[1] is None:
                    r.hand('a parameter is named in one signature only')
                    return
                ren[jp[1]] = mp[1]
        a, b = src.starts[jd.brace_line + 1], src.starts[jd.close_line]
        body = src.data[a:b]
        if ren:
            used = set(ID_RX.findall(src.mask[a:b]))
            if any(v in used and v not in ren for v in ren.values()) or any(v in ren for v in ren.values() if v in used):
                r.hand('parameter names differ and the new name is already used in the June body')
                return
            body, _ = sub_idents(body, ren)
        if sum(1 for l in src.mask[a:b].splitlines() if re.match(rb'[ \t]*#[ \t]*if', l)) != \
                sum(1 for l in src.mask[a:b].splitlines() if re.match(rb'[ \t]*#[ \t]*endif', l)):
            r.hand('June body has unbalanced #if / #endif')
            return
        # default arguments of the two declarations (headers) must agree
        why = self.defaults_differ(r, q2, n2, jsig, msig)
        if why:
            r.hand(why)
            return
        # attached comment lines above the June signature (`// June 3298...`)
        c0 = jd.sig_line
        while c0 - 1 > frame.start and src.lines[c0 - 1].lstrip().startswith(b'//'):
            c0 -= 1
        eol = src.eol(pick.brace_line) or b'\r\n'
        flag = r.flag.encode()
        if deleg:
            flag = deleg['flag']           # the wrapper's flag: the June body goes live under it
        head = b'#ifdef ' + flag + eol + to_eol(b''.join(src.lines[c0:jd.sig_line]), eol) + to_eol(body, eol) + b'#else' + eol
        if deleg:
            f, q = deleg['frame'], deleg['last']
            ed.delete_lines(f.start, f.else_line + 1)
            ed.insert(src.starts[f.start], head)
            ed.delete_lines(f.end, f.end + 1)
            ed.insert(src.starts[q], b'#endif ' + flag + eol)
        else:
            ed.insert(src.starts[pick.brace_line + 1], head)
            ed.insert(src.starts[pick.close_line], b'#endif ' + flag + eol)
        d1 = jd.close_line + 1
        while d1 < frame.end and not src.mline(d1).strip():
            d1 += 1
        ed.delete_lines(c0, d1)
        spans.append((c0, d1))
        r.ok('moved %d line(s)%s%s' % (jd.close_line - jd.brace_line - 1, ', parameter names renamed' if ren else '',
                                      ', delegating wrapper under %s replaced' % flag.decode() if deleg else ', reach point: March callers now run the June body under %s' % flag.decode()))

    def delegating(self, r, src, pick):
        """The port's scaffolding: { #ifdef WF  [return] June_X( args );  }  #else WF  <March body>  }  #endif WF."""
        f = next((x for x in src.frames()[0] if x.start == pick.brace_line + 1), None)
        if f is None or f.kind != b'ifdef' or f.else_line is None or f.end is None or not f.june or f.simple is None:
            return None
        if DIR_RX.match(src.mline(f.else_line).rstrip(b'\r\n')).group(1) != b'else':
            return None
        first = [src.mline(i).strip() for i in range(f.start + 1, f.else_line) if src.mline(i).strip()]
        if len(first) < 2 or first[-1] != b'}' or r.old.encode() not in b' '.join(first[:-1]) or not first[-2].endswith(b';'):
            return None
        q = f.end - 1
        while q > f.else_line and not src.mline(q).strip():
            q -= 1
        if src.mline(q).strip() != b'}':
            return None
        return {'flag': f.simple, 'frame': f, 'last': q}

    def defaults_differ(self, r, q2, n2, jsig, msig):
        if not qual_of(r.cls):
            return None
        e = self.hier.lookup(q2)
        if e is None:
            return None
        rel, bs, be, bases, hs = e
        found = {}
        for nm in (r.old, n2):
            rx = re.compile(rb'(?<![\w.>:])' + re.escape(nm.encode()) + rb'\s*\(')
            for m in rx.finditer(hs.mask, bs, be):
                rp = match_paren(hs.mask, m.end() - 1)
                if rp < 0:
                    continue
                tail = hs.mask[rp + 1:rp + 60].lstrip(b' \t\r\n')
                if tail.startswith(b';') or tail.startswith(b'const'):
                    ps = parse_params(hs.mask[m.end():rp])
                    if [p[0] for p in ps] == [p[0] for p in jsig]:
                        found[nm] = ps
        if r.old in found and n2 in found:
            dj = [p[2] for p in found[r.old]]
            dm = [p[2] for p in found[n2]]
            if dj != dm:
                return 'default arguments differ between the two declarations'
        return None

    def drop_decls(self, merged):
        for rel in self.fs.files():
            if not rel.lower().endswith(('.h', '.inl', '.hpp')):
                continue
            data = self.cur(rel)
            todo = [r for r in merged if r.old.encode() in data]
            if not todo:
                continue
            src = Src(data)
            ed = Editor(src)
            hit = False
            for r in todo:
                e = self.hier.lookup(qual_of(r.cls)) if qual_of(r.cls) else None
                if e is None or e[0] != rel:
                    continue
                bs, be = e[1], e[2]
                rx = re.compile(rb'(?<![\w.>:])' + re.escape(r.old.encode()) + rb'\s*\(')
                for m in rx.finditer(src.mask, bs, be):
                    ls = src.line_of(m.start())
                    pre = src.mask[src.starts[ls]:m.start()]
                    if re.search(rb'[;{}]', pre) or not re.search(rb'[A-Za-z_*&>]\s*$', pre.rstrip() + b' ') or \
                            re.search(rb'\b(return|else|new|delete)\b\s*$', pre):
                        continue
                    rp = match_paren(src.mask, m.end() - 1)
                    tail = src.mask[rp + 1:src.starts[src.line_of(rp) + 1]]
                    if rp < 0 or b'{' in src.mask[rp + 1:rp + 200].split(b';')[0]:
                        r.sites.append('%s:%d inline definition of the June function in the header: hand shape' % (rel, ls + 1))
                        continue
                    if re.match(rb'\s*(const\s*)?;\s*$', tail) and b'virtual' not in pre:
                        ed.delete_lines(ls, src.line_of(rp) + 1)
                        hit = True
                    else:
                        r.sites.append('%s:%d declaration shares its line or is virtual: hand shape' % (rel, ls + 1))
            if hit:
                self.out[rel] = ed.result()

    # -- renames
    def pass_renames(self):
        rr = [r for r in self.rows if r.status == 'pending' and r.action in ('rename',) or (r.action == 'merge' and r.status == 'auto')]
        mp = {}
        for r in rr:
            mp.setdefault(r.old.encode(), []).append(r)
        for rel in self.cand:
            data = self.cur(rel)
            src = Src(data)
            out, pos = [], 0
            for m in ID_RX.finditer(src.mask):
                rs = mp.get(m.group())
                if not rs:
                    continue
                r = next((x for x in rs if self.in_scope(x, rel)), None)
                if r is None:
                    if self.stem(rel) in set(self.stem(x.file) for x in self.by_old[m.group().decode()] if x.file):
                        continue
                    rs[0].sites.append('%s:%d name shared with another class: not renamed here' % (rel, src.line_of(m.start()) + 1))
                    continue
                out.append(data[pos:m.start()]); out.append(r.new.rsplit('::', 1)[-1].encode())
                pos = m.end()
                r.used = getattr(r, 'used', 0) + 1
            if out:
                out.append(data[pos:])
                self.out[rel] = b''.join(out)
        for r in rr:
            if r.status == 'pending':
                r.ok('%d use(s)' % getattr(r, 'used', 0)) if getattr(r, 'used', 0) else r.hand('no use or definition found in code')

    # -- enum / namespace / class / lua / drop
    def pass_others(self):
        for r in self.rows:
            if r.status != 'pending':
                continue
            if r.action == 'enum':
                self.do_enum(r)
            elif r.action in ('namespace', 'class', 'lua'):
                self.do_rename_all(r)
            elif r.action == 'drop':
                self.do_drop(r)

    def do_rename_all(self, r):
        if r.action == 'class' and r.kind == 'class' and not r.old[0:1] in ('C', 'K'):
            pass
        total = 0
        for rel in self.cand:
            if not self.in_scope(r, rel):
                continue
            data = self.cur(rel)
            new, n = sub_idents(data, {r.old.encode(): r.new.encode()})
            if r.action == 'lua':
                lit = b'"' + r.old.encode() + b'"'
                if lit in new:
                    n += new.count(lit)
                    new = new.replace(lit, b'"' + r.new.encode() + b'"')
            if n:
                self.out[rel] = new
                total += n
        r.ok('%d use(s)' % total) if total else r.hand('no use or definition found in code')

    def do_enum(self, r):
        if r.kind == 'class':
            r.hand('enum type rewrite: hand shape (%s)' % r.note[:80])
            return
        new = r.new.encode()
        dfn = re.compile(rb'^\s*(#\s*define\s+%s\b.*|(static\s+)?const\s+\w+\s+%s\s*=.*;|%s\s*=[^,;]*,?)\s*$' % ((re.escape(r.old.encode()),) * 3))
        total, dels = 0, 0
        for rel in self.cand:
            data = self.cur(rel)
            src = Src(data)
            ed = Editor(src)
            n = 0
            defl = set()
            for m in ID_RX.finditer(src.mask):
                if m.group() != r.old.encode():
                    continue
                ln = src.line_of(m.start())
                if dfn.match(src.mline(ln).rstrip(b'\r\n')):
                    defl.add(ln)
                else:
                    ed.splice(m.start(), m.end(), new)
                    n += 1
            for ln in defl:
                ed.delete_lines(ln, ln + 1)
            if n or defl:
                self.out[rel] = ed.result()
                total += n
                dels += len(defl)
        if not total and not dels:
            r.hand('no use or definition found in code')
        else:
            r.ok('%d use(s), %d definition line(s) removed%s' % (total, dels, '' if dels else ' (alias definition not found: left)'))

    def do_drop(self, r):
        name = r.old.encode()
        todo = []
        for rel in self.cand:
            src = Src(self.cur(rel))
            for m in ID_RX.finditer(src.mask):
                if m.group() == name:
                    todo.append((rel, src, m))
        if not todo:
            r.hand('no use or definition found in code')
            return
        by = collections.defaultdict(list)
        for rel, src, m in todo:
            ln = src.line_of(m.start())
            line = src.mline(ln).rstrip(b'\r\n')
            n = re.escape(name)
            if re.match(rb'^\s*[\w:<>*&\s,]+\s+%s\s*(\[[^\]]*\])?\s*;\s*$' % n, line) or re.match(rb'^\s*%s\s*=[^;]*;\s*$' % n, line) \
                    or re.match(rb'^\s*[,:]?\s*%s\s*\([^()]*\)\s*,?\s*$' % n, line):
                by[rel].append((src, ln, line))
            else:
                r.hand('%s:%d is a use that is not a declaration or an init: %s' % (rel, ln + 1, line.strip().decode('latin-1')[:60]))
                return
        for rel, items in by.items():
            src = items[0][0]
            ed = Editor(src)
            for _, ln, line in items:
                if re.match(rb'^\s*[:]\s*%s' % re.escape(name), line):
                    r.hand('%s:%d first initializer of the list: hand shape' % (rel, ln + 1))
                    return
                ed.delete_lines(ln, ln + 1)
            self.out[rel] = ed.result()
        r.ok('%d line(s) removed' % sum(len(v) for v in by.values()))

    def pass_files(self):
        self.renames = []
        for r in self.rows:
            if r.status != 'pending' or r.action != 'file':
                continue
            if not self.fs.exists(r.file):
                r.hand('file %s not found' % r.file)
                continue
            if r.new.startswith('('):
                r.hand('file merged by hand: %s' % r.new)
                continue
            d = os.path.dirname(r.file)
            self.renames.append((r.file, (d + '/' if d else '') + r.new))
            ob, nb = os.path.basename(r.file).encode(), r.new.encode()
            for rel in list(self.cand) + [p for p in PROJ_FILES if self.fs.exists(p)]:
                data = self.cur(rel)
                if ob in data:
                    self.out[rel] = data.replace(ob, nb)
            r.ok('git mv + project files + #include')

    def premap(self, rel, base):
        mp, lit = {}, {}
        for r in self.rows:
            if r.status != 'auto' or r.action not in ('rename', 'namespace', 'class', 'lua', 'enum', 'merge') or not self.in_scope(r, rel):
                continue
            mp[r.old.encode()] = r.new.rsplit('::', 1)[-1].encode() if r.action != 'enum' else r.new.encode()
            if r.action == 'lua':
                lit[b'"' + r.old.encode() + b'"'] = b'"' + r.new.encode() + b'"'
        for r in self.rows:
            if r.status == 'auto' and r.action == 'file' and not r.new.startswith('('):
                lit[os.path.basename(r.file).encode()] = r.new.encode()     # pass_files renames the name in every text, comments too
        if not mp and not lit:
            return base
        out = sub_idents(base, mp)[0] if mp else base
        for a, b in lit.items():
            out = out.replace(a, b)
        return out

    # -- verify: flag-off equivalence and encoding
    def verify(self):
        bad = []
        for rel, new in sorted(self.out.items()):
            old = self.fs.read(rel) if self.fs.exists(rel) else b''
            base = self.fs.base(rel)
            if base is None:
                continue
            ec = (flagcheck.enc_class(base), flagcheck.enc_class(new))
            if ec[0] != ec[1]:
                bad.append((rel, 'encoding class changed %s -> %s' % ec))
            if not rel.lower().endswith(SRC_EXT):
                continue
            base = self.premap(rel, base)       # a rename outside every flag is neutral: compare against the base with the same renames
            if base == new:
                continue                        # renames only: neutral without evaluating any #if (Always.h #error text)
            flags = set(JUNE_FLAG_RX.findall(base)) | set(JUNE_FLAG_RX.findall(new))
            targets = set(f.decode() for f in flags)
            if not targets:
                if base != new:
                    bad.append((rel, 'no June flag in the file, yet it changed beyond the renames'))
                continue
            try:
                a = flagcheck.strip(base, targets)
                b = flagcheck.strip(new, targets)
            except flagcheck.Unevaluable as ex:
                bad.append((rel, 'flag-off check not evaluable: %s' % ex))
                continue
            if a != b:
                fd = flagcheck.first_diff(a, b)
                bad.append((rel, 'flag-off bytes differ from base: %s' % (fd,)))
        return bad


# ------------------------------------------------------------------------------------------------ report

def nested_same_flag(data):
    """Lines of `#ifdef F` inside F's own `#else`: never compiled. A merge leaves one when March's X held a dispatch
    `#ifdef F return June_X(); #endif F` that the rename turned into a call of X itself (C3 review, 2026-10-29)."""
    hits, st = [], []
    for i, line in enumerate(data.split(b'\n'), 1):
        m = DIR_RX.match(line)
        if not m:
            continue
        k, arg = m.group(1), m.group(2).split()
        flag = arg[0] if arg else b''
        if k in (b'ifdef', b'ifndef', b'if'):
            if k == b'ifdef' and any(s[0] == flag and s[1] for s in st):
                hits.append((i, flag.decode('ascii', 'replace')))
            st.append([flag if k == b'ifdef' else b'', False])
        elif k == b'else' and st:
            st[-1][1] = True
        elif k == b'endif' and st:
            st.pop()
    return hits


def summarize(c, rows_all_of_batch, bad, out=print):
    cnt = collections.OrderedDict()
    for r in c.rows:
        d = cnt.setdefault(r.action, collections.Counter())
        d['map'] += 1
        d[r.status] += 1
    out('batch rows: %d (selected %d)' % (len(rows_all_of_batch), len(c.rows)))
    out('%-10s %6s %6s %6s' % ('action', 'map', 'auto', 'hand'))
    for a, d in cnt.items():
        out('%-10s %6d %6d %6d' % (a, d['map'], d['auto'], d['hand'] + d['pending']))
    hands = [r for r in c.rows if r.status != 'auto']
    why = collections.Counter(re.sub(r'[\w:]*June\w*|\b\d+\b', '#', r.reason)[:70] for r in hands)
    out('hand reasons:')
    for k, v in why.most_common(14):
        out('  %4d  %s' % (v, k))
    mixed = [(r, s) for r in c.rows for s in r.sites]
    out('reach points (merge kept under a non-wrapper flag, March callers now run the June body): %d' % sum(1 for r in c.rows if 'reach point' in r.reason))
    out('sites reported for the phase: %d' % len(mixed))
    for r, s in mixed[:12]:
        out('  %s: %s' % (r.old, s))
    dead = []
    for rel, data in sorted(c.out.items()):
        base = c.fs.base(rel) if c.fs.exists(rel) else None
        known = collections.Counter(f for _, f in nested_same_flag(base)) if base else collections.Counter()
        for ln, f in nested_same_flag(data):
            if known[f] > 0:
                known[f] -= 1
            else:
                dead.append('%s:%d %s' % (rel, ln, f))
    out('dead dispatches (#ifdef F inside its own #else; delete the block by hand): %d' % len(dead))
    for s in dead:
        out('  ' + s)
    for rel, w in bad:
        out('FAIL %s: %s' % (rel, w))
    out('files changed: %d, checks %s' % (len(c.out), 'FAILED' if bad else 'PASS'))


def write_report(c, path):
    with open(path, 'w', encoding='utf-8', newline='') as f:
        f.write('old\tnew\taction\tfile\tclass\tstatus\treason\tsites\n')
        for r in c.rows:
            f.write('\t'.join([r.old, r.new, r.action, r.file, r.cls, r.status, r.reason.replace('\t', ' '),
                               ' | '.join(r.sites[:5]).replace('\t', ' ')]) + '\n')


def cmd_apply(args):
    fs = DiskFS(TRUNK, args.base)
    rows = load_map(args.map)
    c = Collapse(fs, rows, args.batch, files=args.files, only=set(args.only) if args.only else None)
    c.allow_existing = args.allow_existing
    c.scope_locals = args.scope_locals
    if not c.rows:
        print('no rows for batch %s' % args.batch)
        return 2
    bad = c.run()
    summarize(c, [r for r in rows if r.batch == args.batch], bad)
    if args.report:
        write_report(c, args.report)
    if args.out_dir:
        for rel, data in c.out.items():
            p = os.path.join(args.out_dir, rel)
            os.makedirs(os.path.dirname(p), exist_ok=True)
            with open(p, 'wb') as f:
                f.write(data)
    if bad:
        print('NOTHING WRITTEN: fix the failing file(s) first')
        return 1
    if args.dry_run:
        print('DRY RUN: nothing written')
        return 0
    for rel, data in c.out.items():
        fs.write(rel, data)
    for a, b in getattr(c, 'renames', []):
        fs.rename(a, b)
    print('WROTE %d file(s), %d rename(s)' % (len(c.out), len(getattr(c, 'renames', []))))
    return 0


# ------------------------------------------------------------------------------------------------ selftest

def selftest():
    kor = b'\xb0\xa1\xb3\xaa'          # CP949 bytes that must survive
    files = {
        'X2Lib/Base.h': b'class CBase\r\n{\r\npublic:\r\n\tvoid Taken();\r\n};\r\n',
        'X2Lib/Fix.h': (b'class CFix : public CBase\r\n{\r\npublic:\r\n\tvoid Frame( int a = 0 );\r\n#ifdef SERV_IRUHADEV_JUNE_FIX\r\n'
                        b'\tvoid June_Frame( int a = 0 );\t/// June 3200aaaa\r\n\tvoid June_Taken();\r\n\tvoid June_Free();\r\n#endif SERV_IRUHADEV_JUNE_FIX\r\n};\r\n'),
        'X2Lib/Fix.cpp': (
            b'void CFix::Frame( int a )\r\n{\r\n\tDoMarch( a );\r\n}\r\n\r\n'
            b'void CFix::Other()\r\n{\r\n'
            b'#ifdef SERV_IRUHADEV_JUNE_FIX\r\n\tJune_Frame( 1 );\r\n#else\r\n\tFrame( 1 );\r\n#endif SERV_IRUHADEV_JUNE_FIX\r\n'
            b'#ifdef SERV_IRUHADEV_JUNE_FIX\r\n\tJune_Frame( 2, 3 );\r\n#else\r\n\tFrame( 2 );\r\n#endif SERV_IRUHADEV_JUNE_FIX\r\n'
            b'}\r\n\r\n'
            b'#ifdef SERV_IRUHADEV_JUNE_FIX\r\n// June 3200aaaa: the frame ' + kor + b'\r\nvoid CFix::June_Frame( int a_ )\r\n{\r\n'
            b'\tDoJune( a_ ); // ' + kor + b'\r\n\tPlay( L"x" );\r\n}\r\n\r\n'
            b'void CFix::June_Taken()\r\n{\r\n\tJune_Free();\r\n}\r\n\r\n'
            b'void CFix::June_Free()\r\n{\r\n\tDoFree();\r\n}\r\n#endif SERV_IRUHADEV_JUNE_FIX\r\n'),
    }
    fs = MemFS(files)
    base = {'X2Lib/Fix.cpp': files['X2Lib/Fix.cpp'], 'X2Lib/Fix.h': files['X2Lib/Fix.h']}
    mapt = [
        ('June_Frame', 'Frame', 'merge', 'X2Lib/Fix.cpp', 'CFix', 'SERV_IRUHADEV_JUNE_FIX', '3200aaaa', '', 'twin', 'T', 'member function'),
        ('June_Taken', 'Taken', 'rename', 'X2Lib/Fix.cpp', 'CFix', 'SERV_IRUHADEV_JUNE_FIX', '', '', '', 'T', 'member function'),
        ('June_Free', 'RunFree', 'rename', 'X2Lib/Fix.cpp', 'CFix', 'SERV_IRUHADEV_JUNE_FIX', '', '', '', 'T', 'member function'),
    ]
    hdr = 'old new action file class flag june_va guard note batch kind'.split()
    rows = [Row(dict(zip(hdr, t))) for t in mapt]
    c = Collapse(fs, rows, 'T', log=lambda *a: None)
    bad = c.run()
    st = dict((r.old, r) for r in rows)
    fails = []

    def check(cond, what):
        if not cond:
            fails.append(what)

    check(not bad, 'equivalence / encoding checks pass: %r' % (bad,))
    check(st['June_Frame'].status == 'auto', 'twin merge applied: %s' % st['June_Frame'].reason)
    check(st['June_Taken'].status == 'hand' and 'CBase' in st['June_Taken'].reason, 'rename blocked by a base-class name: %s' % st['June_Taken'].reason)
    check(st['June_Free'].status == 'auto', 'free rename applied: %s' % st['June_Free'].reason)
    check(any('mixed call site' in s for s in st['June_Frame'].sites), 'mixed-flag call site reported: %r' % st['June_Frame'].sites)
    cpp = c.out.get('X2Lib/Fix.cpp', b'')
    check(b'June_Frame' not in cpp.replace(b'// June', b''), 'no June_Frame left in code')
    check(cpp.count(kor) == 2, 'CP949 bytes kept (%d)' % cpp.count(kor))
    want = (b'void CFix::Frame( int a )\r\n{\r\n#ifdef SERV_IRUHADEV_JUNE_FIX\r\n// June 3200aaaa: the frame ' + kor + b'\r\n\tDoJune( a ); // ' + kor +
            b'\r\n\tPlay( L"x" );\r\n#else\r\n\tDoMarch( a );\r\n#endif SERV_IRUHADEV_JUNE_FIX\r\n}\r\n')
    check(cpp.startswith(want), 'merged body shape: %r' % cpp[:300])
    check(b'#ifdef SERV_IRUHADEV_JUNE_FIX\r\n\tJune_Frame( 1 )' not in cpp and b'\tFrame( 1 );\r\n#else' not in cpp, 'plain wrapper collapsed')
    check(b'Frame( 1 );' in cpp, 'wrapper left the March call')
    hh = c.out.get('X2Lib/Fix.h', b'')
    check(b'June_Frame' not in hh and b'void RunFree' in hh and b'June_Taken' in hh, 'header: decl removed, rename applied, blocked name kept: %r' % hh)
    # a corrupted result must be caught by the equivalence check
    bad2 = Collapse(fs, [], 'T').verify()
    check(bad2 == [], 'verify on no output')
    broken = dict(c.out)
    broken['X2Lib/Fix.cpp'] = cpp.replace(b'DoMarch', b'DoMarsh')
    c2 = Collapse(fs, [], 'T')
    c2.out = broken
    check(bool(c2.verify()), 'a March-side change is caught by the flag-off check')
    # second fixture: the port's delegating wrapper, a data member, an enum alias, a namespace, a Lua-bound name, a dropped member
    kor2 = b'\xc7\xd1'
    f2 = {
        'X2Lib/Cls.h': (b'class CCls\r\n{\r\npublic:\r\n\tvoid ReInit( int a );\r\n\tint m_iOld;\r\n#ifdef SERV_IRUHADEV_JUNE_CLS_CORE\r\n'
                        b'\tvoid June_ReInit( int a );\r\n\tint m_iJuneMemo;\r\n\tbool m_bJune978c;\r\n#endif SERV_IRUHADEV_JUNE_CLS_CORE\r\n};\r\n'),
        'X2Lib/Cls.cpp': (
            b'void CCls::ReInit( int a )\r\n{\r\n#ifdef SERV_IRUHADEV_JUNE_CLS_LIVE\r\n\tJune_ReInit( a );\r\n}\r\n#else SERV_IRUHADEV_JUNE_CLS_LIVE\r\n'
            b'\tMarchReInit( a ); // ' + kor2 + b'\r\n}\r\n#endif SERV_IRUHADEV_JUNE_CLS_LIVE\r\n\r\n'
            b'#ifdef SERV_IRUHADEV_JUNE_CLS_CORE\r\nnamespace _CONST_CLS_JUNE_CH1_\r\n{\r\n\tconst int JUNE_CH1_ALIAS = 5;\r\n}\r\n'
            b'using namespace _CONST_CLS_JUNE_CH1_;\r\n'
            b'void CCls::June_ReInit( int b )\r\n{\r\n\tm_iJuneMemo = JUNE_CH1_ALIAS + b;\r\n\tm_bJune978c = false;\r\n}\r\n'
            b'void Reg()\r\n{\r\n\tdecl( "JuneLoad", &Load );\r\n}\r\n#endif SERV_IRUHADEV_JUNE_CLS_CORE\r\n'),
    }
    fs2 = MemFS(f2)
    h2 = 'old new action file class flag june_va guard note batch kind'.split()
    t2 = [
        ('June_ReInit', 'ReInit', 'merge', 'X2Lib/Cls.cpp', 'CCls', 'SERV_IRUHADEV_JUNE_CLS_CORE', '', '', 'twin', 'T', 'member function'),
        ('m_iJuneMemo', 'm_iMemo', 'rename', 'X2Lib/Cls.h', 'CCls', 'SERV_IRUHADEV_JUNE_CLS_CORE', '', '', '', 'T', 'data member'),
        ('_CONST_CLS_JUNE_CH1_', '_CONST_CLS_CH1_', 'namespace', 'X2Lib/Cls.cpp', '', 'SERV_IRUHADEV_JUNE_CLS_CORE', '', '', '', 'T', 'namespace'),
        ('JuneLoad', 'LoadWidget', 'lua', 'X2Lib/Cls.cpp', '', '', '', '', '', 'T', 'lua name'),
        ('JUNE_CH1_ALIAS', 'CCls::E_ALIAS', 'enum', 'X2Lib/Cls.cpp', '', 'SERV_IRUHADEV_JUNE_CLS_CORE', '', '', '', 'T', 'constant'),
        ('m_bJune978c', '-', 'drop', 'X2Lib/Cls.h', 'CCls', 'SERV_IRUHADEV_JUNE_CLS_CORE', '', '', '', 'T', 'data member'),
    ]
    rows2 = [Row(dict(zip(h2, t))) for t in t2]
    c3 = Collapse(fs2, rows2, 'T', log=lambda *a: None)
    bad3 = c3.run()
    check(not bad3, 'second fixture checks pass: %r' % (bad3,))
    for r in rows2:
        check(r.status == 'auto', '%s: %s %s' % (r.old, r.status, r.reason))
    cpp2 = c3.out.get('X2Lib/Cls.cpp', b'')
    want2 = (b'void CCls::ReInit( int a )\r\n{\r\n#ifdef SERV_IRUHADEV_JUNE_CLS_LIVE\r\n\tm_iMemo = CCls::E_ALIAS + a;\r\n#else\r\n'
             b'\tMarchReInit( a ); // ' + kor2 + b'\r\n#endif SERV_IRUHADEV_JUNE_CLS_LIVE\r\n}\r\n')
    check(cpp2.startswith(want2), 'delegating wrapper merged under LIVE: %r' % cpp2[:300])
    check(b'"LoadWidget"' in cpp2 and b'_CONST_CLS_CH1_' in cpp2 and b'JUNE_CH1_ALIAS' not in cpp2, 'namespace / lua / enum rewritten: %r' % cpp2)
    check(b'm_bJune978c' not in cpp2 and b'm_bJune978c' not in c3.out.get('X2Lib/Cls.h', b''), 'dropped member is gone')
    if fails:
        for f in fails:
            print('FAIL: ' + f)
        print('SELFTEST FAIL (%d)' % len(fails))
        return 1
    print('SELFTEST PASS (merge, delegating wrapper, base-class block, mixed site, CP949, header decl, member / namespace / lua / enum / drop, equivalence catch)')
    return 0


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('cmd', nargs='?', choices=['apply'])
    ap.add_argument('batch', nargs='?')
    ap.add_argument('--dry-run', action='store_true')
    ap.add_argument('--allow-existing', action='store_true', help='do not refuse a class / lua / free rename whose new name is already in use (finishing a half-done rename)')
    ap.add_argument('--scope-locals', action='store_true', help='a local rename is refused only when its new name is used in the same function (column-0 braces) on a branch the old name can meet')
    ap.add_argument('--files', nargs='+')
    ap.add_argument('--only', nargs='+')
    ap.add_argument('--map', default=MAP_PATH)
    ap.add_argument('--base', default='HEAD')
    ap.add_argument('--report')
    ap.add_argument('--out-dir', help='dry run: also write the changed files under this directory (for diffing)')
    ap.add_argument('--selftest', action='store_true')
    a = ap.parse_args()
    if a.selftest:
        return selftest()
    if a.cmd != 'apply' or not a.batch:
        ap.print_help()
        return 2
    return cmd_apply(a)


if __name__ == '__main__':
    sys.exit(main())
