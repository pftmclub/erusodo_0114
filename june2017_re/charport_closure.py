"""Per-state effect / particle closure of one June class script (pilot-char-revamp CH0 item 5b; reusable for Stage 7).

    python june2017_re/charport_closure.py                                   # Aisha defaults
    python june2017_re/charport_closure.py Arme_VioletMage --range 0x32cb6e00 0x32cf2da0 [-o out.tsv] [--refresh-fx]

Arguments
  <ClassScript>   June class script name (resolved to june_src_fixed/data036_kom/<name>.src.lua) or a path.
  --range LO HI   the June class code range in unpacked_x2.exe. Derive it from the vtables: the class's own vtable
                  (RTTI; vtslot.py) has its lowest override at LO, and the next class's vtable has its lowest override at HI.
                  Aisha (CX2GUArme_VioletMage, vtable 3351d0b4): 32cb6e00 .. 32cf2da0. Rena's last vtable override is 32cb6c50
                  (ends 32cb6c6e), Elsword's first is 32cf2da0; 32cb6c70-32cb6dff are 16-byte stubs with no string reference.
  -o              output tsv (default june2017_re/char_arme/ch0_effect_closure.tsv)
  --refresh-fx    re-run fxdump over June's effect Lua (default: reuse assets_cache/june_fx.json)

What it does (read-only on every game / June file; writes only the tsv)
  1. Roots, per state.  Every top-level table of the class script is a state (INIT_* tables are reported as states too).  Each quoted
     string in a state is looked up as an EffectSet / DamageEffect / particle / mesh-player templet name, or as a file name
     (extension).  Strings that are neither (animation, bone, state names) are ignored.
  2. C++ roots.  Every function in --range is disassembled (capstone); each imm32 / absolute operand that points at a
     NUL-terminated ASCII or UTF-16 string is a literal, plus xordecrypt.py --range output (compile-time XOR strings).  The state column
     is `fn:<function VA>`.  A literal counts if it is a templet name, a file name, a printf pattern (%d, %s, %02d ...) expanded against
     the June templet and file sets, or a run-time concatenation: two literals of the same function whose join is a templet name.
  3. Walk.  From each root, out to everything it plays, using assets.py's edge rules (lua_edges: EFFECT%d NAME by EFFECT_TYPE,
     MAIN_EFFECT_NAME, PARTICLE_NAME, DAMAGE_EFFECT_NAME, CREATE_*_NAME, FILE_NAME, HTD_*), the templet .txt references (Texture, Mesh,
     *Xet, MeshName, AniPackage, nested particle lists), and the textures a mesh / .xet embeds (fxdata.embedded_textures).
  4. March status, against kom_contents (what pack/copy ships; `staged-not-live` in the note marks a file not in a live kom yet).
       effect-set / damage-effect: June's table vs March's table (both executed with fxdump.lua): same = june-bytes, differs = march-copy.
       particle / mesh-player:     June's last definition vs March's last definition (token streams): same = june-bytes, differs = march-copy.
       texture / mesh / sound / other: bytes (size, then sha1) of June's copy (strict junekom decode) vs the kom_contents copy.
       absent from March = missing.  A name June itself does not define is march_status by March, june_kom `-`, note `june-undefined`.

Columns: state kind name march_status june_kom phase via note.  `phase` stays `-` (a later step assigns ch1/ch2/ch3 per state).
"""
import argparse
import collections
import csv
import hashlib
import json
import os
import re
import struct
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..'))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(TRUNK, 'tools'))
import fxdata as F      # noqa: E402
import assets as A      # noqa: E402
import junekom as JK    # noqa: E402
import kom as KOM       # noqa: E402

try:
    import capstone
except ImportError:     # pragma: no cover
    capstone = None

EXE = os.path.join(F.JUNE_DATA, 'unpacked_x2.exe')
SRC_DIR = os.path.join(TRUNK, 'june_src_fixed', 'data036_kom')
OUT_DEFAULT = os.path.join(HERE, 'char_arme', 'ch0_effect_closure.tsv')
AISHA = dict(script='Arme_VioletMage', lo=0x32cb6e00, hi=0x32cf2da0)

TEX_EXT = ('.dds', '.tga', '.bmp', '.png', '.jpg', '.jpeg')
MESH_EXT = ('.x', '.y', '.ksm')
SOUND_EXT = ('.ogg', '.wav', '.mp3')
FILE_EXT = TEX_EXT + MESH_EXT + SOUND_EXT + ('.xet', '.tet', '.gfx', '.txt', '.tab', '.fx')
KIND_OF_TYPE = {'es': 'effect-set', 'de': 'damage-effect'}
MARCH_FX_ORDER = ['Enum', 'DamageSound', 'UIEffectSet', 'EffectSet', 'UIEffectSet_INT', 'EffectSet_INT',
                  'EffectSet_Description', 'DamageEffect', 'DamageEffect_Character', 'DamageEffect_Dungeon',
                  'DamageEffect_ETC', 'DamageEffect_INT']


def log(*a):
    sys.stderr.write(' '.join(str(x) for x in a) + '\n')
    sys.stderr.flush()


# ------------------------------------------------------------------------------------------------ class script
STR_OR_COMMENT = re.compile(r'--\[(=*)\[.*?\]\1\]|--[^\n]*|"((?:[^"\\\n]|\\.)*)"|\'((?:[^\'\\\n]|\\.)*)\'', re.S)
STATE_HEAD = re.compile(r'^([A-Za-z_][A-Za-z0-9_]*)\s*=\s*\{', re.M)


def script_states(path):
    """OrderedDict state -> [string literals] for every top-level table of the class script, in file order."""
    text = open(path, 'rb').read().decode('utf-8', 'replace').lstrip('﻿').replace('\r\n', '\n')
    heads = [(m.start(), m.group(1)) for m in STATE_HEAD.finditer(text)]
    out = _States()
    out.n_tables = len(heads)       # a table name defined twice counts once in `out`, twice here
    out.dups = sorted(n for n, c in collections.Counter(h[1] for h in heads).items() if c > 1)
    for i, (pos, name) in enumerate(heads):
        end = heads[i + 1][0] if i + 1 < len(heads) else len(text)
        strs = []
        for m in STR_OR_COMMENT.finditer(text, pos, end):
            s = m.group(2) if m.group(2) is not None else m.group(3)
            if s is not None:
                strs.append(s)
        out.setdefault(name, []).extend(strs)
    return out


# ------------------------------------------------------------------------------------------------ June exe literals
class _States(collections.OrderedDict):
    n_tables = 0


class Exe(object):
    def __init__(self, path):
        self.d = open(path, 'rb').read()
        d = self.d
        pe = struct.unpack_from('<I', d, 0x3c)[0]
        nsec = struct.unpack_from('<H', d, pe + 6)[0]
        opt = struct.unpack_from('<H', d, pe + 20)[0]
        self.base = struct.unpack_from('<I', d, pe + 24 + 28)[0]
        self.secs = []
        for i in range(nsec):
            _, vs, va, rs, rp = struct.unpack_from('<8sIIII', d, pe + 24 + opt + 40 * i)
            self.secs.append((self.base + va, self.base + va + max(vs, rs), rp, rs))
        self.top = max(s[1] for s in self.secs)

    def v2r(self, v):
        for a, b, rp, rs in self.secs:
            if a <= v < b and v - a < rs:
                return rp + v - a
        return None

    def cstr(self, v):
        """NUL-terminated ASCII (>= 3 chars) or UTF-16LE (>= 3 chars) string at VA v, else None."""
        d = self.d
        o = self.v2r(v)
        if o is None or o + 8 >= len(d):
            return None
        if d[o + 1] == 0 and 0x20 <= d[o] < 0x7f and d[o + 3] == 0:
            j = o
            while j + 1 < len(d) and d[j + 1] == 0 and 0x20 <= d[j] < 0x7f and j - o < 600:
                j += 2
            if j + 1 < len(d) and d[j] == 0 and d[j + 1] == 0 and (j - o) // 2 >= 3:
                return d[o:j:2].decode('ascii')
        j = o
        while j < len(d) and d[j] != 0 and 0x20 <= d[j] < 0x7f and j - o < 300:
            j += 1
        if j < len(d) and d[j] == 0 and j - o >= 3:
            return d[o:j].decode('ascii')
        return None

    def literals(self, lo, hi):
        """OrderedDict function VA -> [literal strings] for every function in [lo, hi) (int3-padded bounds)."""
        if capstone is None:
            raise SystemExit('capstone is required (pip install capstone)')
        code = self.d[self.v2r(lo):self.v2r(hi - 1) + 1]
        md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        md.detail = True
        IMM, MEM = capstone.x86.X86_OP_IMM, capstone.x86.X86_OP_MEM
        funcs = collections.OrderedDict()
        cur = None
        prev_cc = True
        off = 0
        while off < len(code):
            n = 0
            for i in md.disasm(code[off:], lo + off):
                n += 1
                off = i.address + i.size - lo
                if i.bytes[0] == 0xcc:
                    prev_cc = True
                    continue
                if prev_cc:
                    cur = i.address
                    funcs[cur] = []
                prev_cc = False
                for op in i.operands:
                    v = None
                    if op.type == IMM:
                        v = op.imm & 0xffffffff
                    elif op.type == MEM and op.mem.base == 0 and op.mem.index == 0:
                        v = op.mem.disp & 0xffffffff
                    if v and self.base < v < self.top and not (lo <= v < hi):
                        s = self.cstr(v)
                        if s and s not in funcs[cur]:
                            funcs[cur].append(s)
            if n == 0:
                off += 1        # undecodable byte (data in .text): skip it
        return funcs


def xor_strings(lo, hi):
    """Compile-time XOR strings (xordecrypt.py --range) of the code range: [(func_va, plaintext)]."""
    out = []
    tmp = tempfile.NamedTemporaryFile(suffix='.tsv', delete=False)
    tmp.close()
    try:
        p = subprocess.run([sys.executable, os.path.join(HERE, 'xordecrypt.py'), '--range', '0x%x' % lo, '0x%x' % hi,
                            '-o', tmp.name], capture_output=True)
        if p.returncode != 0:
            log('xordecrypt failed:', p.stderr.decode('latin1')[-300:])
            return out
        with open(tmp.name, encoding='utf-8', newline='') as f:
            for r in csv.DictReader(f, delimiter='\t'):
                if r['status'] == 'ok' and r['plaintext']:
                    out.append((int(r['func_va'], 16), r['plaintext']))
    finally:
        os.unlink(tmp.name)
    return out


# ------------------------------------------------------------------------------------------------ data
def ext_of(name):
    return os.path.splitext(name)[1].lower()


def file_kind(name):
    e = ext_of(name)
    if e in TEX_EXT:
        return 'texture'
    if e in MESH_EXT:
        return 'mesh'
    if e in SOUND_EXT:
        return 'sound'
    return 'other'


def tokens_of(t):
    return t.body


class Data(object):
    def __init__(self, refresh_fx=False):
        log('june inventory ...')
        self.jun = F.Build('june', F.JUNE_DATA, F.JUNE_DATA)
        log('june templets ...')
        self.jt = F.templets('june', self.jun)
        fxp = os.path.join(F.CACHE, 'june_fx.json')
        if refresh_fx or not os.path.exists(fxp):
            log('june fxdump ...')
            self.jfx = F.june_fx()
        else:
            self.jfx = json.load(open(fxp, encoding='latin1'))
        self.jes, self.jde = F.fx_templets(self.jfx)
        et = self.jfx['globals']['EFFECT_TYPE']['value']['$t']
        self.et_names = dict((v, k) for k, v in et.items())
        log('march staged index ...')
        self.staged = KOM.staged_names()
        self.live = KOM.live_names()
        self.mpath = {}                       # lower name -> path of the winning kom_contents copy
        self.mkoms = {}                       # lower name -> [kom stems]
        for low, d in self.staged.items():
            stems = sorted(d, key=lambda s: (1 if s.startswith('datai') else 0, int(re.sub(r'\D', '', s) or 0)))
            self.mkoms[low] = stems
            win = stems[-1]
            self.mpath[low] = os.path.join(KOM.KOM_ROOT, win + '_kom', d[win])
        log('march fxdump ...')
        self.mfx = self.march_fx()
        self.mes, self.mde = F.fx_templets(self.mfx)
        log('march templets ...')
        self.mt = self.march_templets()
        self._jkom = {}
        self._jbytes = {}
        self._embed = {}
        # name sets for matching literals
        self.templet_ns = collections.defaultdict(list)    # name -> [node types]
        for n in self.jes:
            self.templet_ns[n].append('es')
        for n in self.jde:
            self.templet_ns[n].append('de')
        for ns, d in self.jt.items():
            for n in d:
                self.templet_ns[n].append(ns)

    # ---- March
    def march_fx(self):
        files = []
        for n in MARCH_FX_ORDER:
            p = self.mpath.get((n + '.lua').lower())
            if p:
                files.append(p)
        return F.run_fxdump(files, os.path.join(F.CACHE, 'march_fx_current.json'))

    def march_templets(self):
        res = {}
        for ns in F.NS_FILES:
            d = collections.defaultdict(list)
            for f in F.NS_FILES[ns]:
                names = [f]
                if f == '<GameMajorParticle%d.txt>':
                    names = ['GameMajorParticle%d.txt' % i for i in range(2, F.MAR_PARTICLE_MAX + 1)]
                for n in names:
                    p = self.mpath.get(n.lower())
                    if p is None:
                        continue
                    for t in F.parse_txt(p, ns):
                        d[t.name].append(t)
            res[ns] = d
        return res

    # ---- June files
    def june_copies(self, low):
        return self.jun.inv.get(low, [])

    def june_kom_of(self, low):
        cs = self.june_copies(low)
        return ','.join(sorted(set(c.kom for c in cs))) if cs else ''

    def june_bytes(self, low):
        """Strict June bytes of a file (first copy that decodes), or None."""
        if low in self._jbytes:
            return self._jbytes[low]
        res = None
        for c in self.june_copies(low):
            if c.path:
                res = open(c.path, 'rb').read()
                break
            kp = os.path.join(F.JUNE_DATA, c.kom + '.kom')
            k = self._jkom.get(kp)
            if k is None:
                k = self._jkom[kp] = JK.Kom(kp)
            for e in k.entries:
                if e['name'].lower() == low:
                    raw = open(kp, 'rb').read() if False else None
                    with open(kp, 'rb') as fh:
                        fh.seek(e['offset'])
                        raw = fh.read(e['csize'])
                    out, err = JK.decode(e, raw, k.v04)
                    if out is not None:
                        res = out
                    break
            if res is not None:
                break
        self._jbytes[low] = res
        return res

    def file_status(self, low):
        """(march_status, note) for a file by lowercase name."""
        mp = self.mpath.get(low)
        note = []
        if mp is None:
            if low in self.live:
                note.append('live-not-staged:' + ','.join(sorted(self.live[low])))
                return 'march-copy', ';'.join(note)
            if not self.june_copies(low):
                note.append('june-undefined')
            return 'missing', ';'.join(note)
        if len(self.mkoms[low]) > 1:
            note.append('march-in:' + ','.join(self.mkoms[low]))
        if low not in self.live:
            note.append('staged-not-live')
        copies = self.june_copies(low)
        if not copies:
            note.append('june-undefined')
            return 'march-copy', ';'.join(note)
        msize = os.path.getsize(mp)
        jsizes = sorted(set(c.size for c in copies))
        if msize not in jsizes:
            note.append('size june %s march %d' % ('/'.join(str(s) for s in jsizes), msize))
            return 'march-copy', ';'.join(note)
        jb = self.june_bytes(low)
        if jb is None:
            note.append('june-bytes-unreadable;sizes-equal')
            return 'june-bytes', ';'.join(note)
        same = hashlib.sha1(jb).digest() == hashlib.sha1(open(mp, 'rb').read()).digest()
        if not same:
            note.append('same size, bytes differ')
        return ('june-bytes' if same else 'march-copy'), ';'.join(note)

    def embedded(self, low):
        """Texture names a June mesh / .xet embeds."""
        if low in self._embed:
            return self._embed[low]
        names = []
        best = None
        for c in self.june_copies(low):
            if c.path:
                best = (c.path, True)
                break
        if best is None:
            for c in self.june_copies(low):
                if c.xpath:
                    best = (c.xpath, False)
                    break
        if best:
            try:
                names, _ = F.embedded_textures(best[0], lenient=True)
            except Exception as e:      # noqa: BLE001
                log('embedded_textures', low, e)
        self._embed[low] = names
        return names


# ------------------------------------------------------------------------------------------------ closure
class Node(object):
    __slots__ = ('typ', 'name')

    def __init__(self, typ, name):
        self.typ, self.name = typ, name

    def key(self):
        return (self.typ, self.name.lower() if self.typ == 'file' else self.name)


def node_kind(typ, name):
    if typ in KIND_OF_TYPE:
        return KIND_OF_TYPE[typ]
    if typ.startswith('part.'):
        return 'particle'
    if typ.startswith('mesh.'):
        return 'mesh-player'
    return file_kind(name)


def children(D, typ, name):
    """[(child typ, child name, key)] a node plays (June's definition)."""
    out = []
    if typ in ('es', 'de'):
        defs = D.jes.get(name) if typ == 'es' else None
        if typ == 'es':
            if not defs:
                return out
            tab = defs[-1][2]
        else:
            if name not in D.jde:
                return out
            tab = D.jde[name][1]
        edges, uncl = [], []
        A.lua_edges(typ, tab, D.et_names, edges, uncl)
        for p, key, ns, v in edges:
            if not v:
                continue
            if ns == 'file':
                out.append(('file', re.split(r'[\\/]', v)[-1], key))
            elif ns.startswith('es-entry-type'):
                continue
            else:
                out.append((ns, v, key))
    elif typ.startswith(('part.', 'mesh.')):
        ts = D.jt[typ].get(name)
        if ts:
            for key, kind, v in ts[-1].refs:
                if kind == 'nested':
                    out.append((typ, v, key))
                else:
                    out.append(('file', re.split(r'[\\/]', v)[-1], key))
    elif typ == 'file':
        low = name.lower()
        if ext_of(low) in ('.x', '.y', '.xet', '.ksm'):
            for n in D.embedded(low):
                out.append(('file', re.split(r'[\\/]', n)[-1], 'embedded'))
        if ext_of(low) in TEX_EXT:
            tet = os.path.splitext(low)[0] + '.tet'
            if D.june_copies(tet):
                out.append(('file', tet, 'atlas'))
    return out


def defines(D, typ, name):
    if typ == 'es':
        return name in D.jes
    if typ == 'de':
        return name in D.jde
    if typ == 'file':
        return bool(D.june_copies(name.lower()))
    return name in D.jt[typ]


def templet_file(D, typ, name, build='june'):
    """Defining file of a templet in June / March."""
    if typ == 'es':
        d = D.jes.get(name) if build == 'june' else D.mes.get(name)
        return d[-1][1] if d else ''
    if typ == 'de':
        d = D.jde.get(name) if build == 'june' else D.mde.get(name)
        return d[0] if d else ''
    src = D.jt if build == 'june' else D.mt
    ts = src[typ].get(name)
    return ts[-1].file if ts else ''


def march_status(D, typ, name):
    """(status, june_kom, note) for one node."""
    jdef = defines(D, typ, name)
    if typ == 'file':
        low = name.lower()
        st, note = D.file_status(low)
        return st, D.june_kom_of(low) or '-', note
    jf = templet_file(D, typ, name, 'june')
    jk = D.june_kom_of(jf.lower()) if jf else ''
    notes = []
    if not jdef:
        notes.append('june-undefined')
    if typ in ('es', 'de'):
        if typ == 'es':
            md = D.mes.get(name)
            jt = D.jes[name][-1][2] if jdef else None
            mt = md[-1][2] if md else None
        else:
            md = D.mde.get(name)
            jt = D.jde[name][1] if jdef else None
            mt = md[1] if md else None
        if mt is None:
            return 'missing', jk or '-', ';'.join(notes + ['june:' + jf])
        mf = templet_file(D, typ, name, 'march')
        if jt is not None and jt == mt:
            return 'june-bytes', jk or '-', ';'.join(notes + ['same;march:' + mf])
        return 'march-copy', jk or '-', ';'.join(notes + ['differs;june:%s march:%s' % (jf, mf)])
    mts = D.mt[typ].get(name)
    if not mts:
        return 'missing', jk or '-', ';'.join(notes + ['june:' + jf])
    mf = mts[-1].file
    if jdef and tokens_of(D.jt[typ][name][-1]) == tokens_of(mts[-1]):
        return 'june-bytes', jk or '-', ';'.join(notes + ['same;march:' + mf])
    return 'march-copy', jk or '-', ';'.join(notes + ['differs;june:%s march:%s' % (jf, mf)])


# ------------------------------------------------------------------------------------------------ roots
PAT = re.compile(r'%(?:0?\d*)[ds]')


def pattern_regex(lit):
    """Compiled regex for a printf-style literal (%d, %s, %02d), or None."""
    if not PAT.search(lit):
        return None
    prefix = PAT.split(lit)[0]
    if len(re.sub(r'[^A-Za-z]', '', prefix)) < 4:
        return None
    parts = PAT.split(lit)
    rx = '^' + '(.+?)'.join(re.escape(p) for p in parts) + '$'
    rx = rx.replace('(.+?)', '([0-9]+)') if '%d' in lit and '%s' not in lit else rx
    return re.compile(rx, re.I)


def roots_from_literals(D, lits, nfile_names):
    """lits: [(literal, origin)] -> [(typ, name, note)] roots; also returns unmatched name-like literals."""
    roots = []
    seen = set()
    unmatched = []
    names_all = D.templet_ns
    file_lows = None

    def add(typ, name, note):
        k = (typ, name.lower() if typ == 'file' else name)
        if k not in seen:
            seen.add(k)
            roots.append((typ, name, note))

    litset = [l for l, _ in lits]
    for lit in litset:
        if lit in names_all:
            for t in names_all[lit]:
                add(t, lit, 'cpp-literal')
            continue
        e = ext_of(lit)
        if e in FILE_EXT and '%' not in lit and len(lit) > 4 and '/' not in lit and '\\' not in lit and ' ' not in lit:
            add('file', lit, 'cpp-literal')
            continue
        rx = pattern_regex(lit)
        if rx is not None:
            hit = 0
            for n, ts in names_all.items():
                if rx.match(n):
                    hit += 1
                    for t in ts:
                        add(t, n, 'cpp-pattern:' + lit)
            if ext_of(lit) in FILE_EXT:
                if file_lows is None:
                    file_lows = list(D.jun.inv.keys())
                for low in file_lows:
                    if rx.match(low) or rx.match(D.jun.inv[low][0].name):
                        hit += 1
                        add('file', D.jun.inv[low][0].name, 'cpp-pattern:' + lit)
            if not hit:
                unmatched.append((lit, 'pattern-no-match'))
            continue
        if re.match(r'(?i)^(effectset|damageeffect|damage_effect)[_A-Za-z0-9]+$', lit) and len(lit) > 9:
            # the code asks for a name June itself does not define: a row (june-undefined), not a guess
            add('es' if lit.lower().startswith('effectset') else 'de', lit, 'cpp-literal')
    return roots, unmatched


def roots_from_concat(D, per_func, seen_roots):
    """Run-time built names: two literals of one function whose join is a templet name."""
    roots = []
    seen = set(seen_roots)
    for fn, lits in per_func.items():
        ls = [l for l in lits if 1 <= len(l) <= 60]
        for a in ls:
            for b in ls:
                if a == b:
                    continue
                n = a + b
                if n in D.templet_ns:
                    for t in D.templet_ns[n]:
                        k = (fn, t, n)
                        if k not in seen:
                            seen.add(k)
                            roots.append((fn, t, n, 'cpp-concat:%s+%s' % (a, b)))
    return roots


# ------------------------------------------------------------------------------------------------ main
def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('script', nargs='?', default=AISHA['script'])
    ap.add_argument('--range', nargs=2, metavar=('LO', 'HI'), default=None)
    ap.add_argument('-o', '--out', default=OUT_DEFAULT)
    ap.add_argument('--refresh-fx', action='store_true')
    args = ap.parse_args()
    lo, hi = (AISHA['lo'], AISHA['hi']) if args.range is None else (int(args.range[0], 16), int(args.range[1], 16))
    sp = args.script
    if not os.path.isfile(sp):
        sp = os.path.join(SRC_DIR, sp + '.src.lua')
        if not os.path.isfile(sp):
            sp = os.path.join(SRC_DIR, args.script.replace('.src.lua', '').replace('.lua', '') + '.src.lua')
    if not os.path.isfile(sp):
        raise SystemExit('class script not found: %s' % args.script)

    D = Data(args.refresh_fx)

    # ---- state roots from the script
    states = script_states(sp)
    state_roots = collections.OrderedDict()      # state -> [(typ, name, note)]
    for st, strs in states.items():
        seen = set()
        for s in strs:
            if not s:
                continue
            cands = []
            if s in D.templet_ns:
                cands = [(t, s) for t in D.templet_ns[s]]
            elif ext_of(s) in FILE_EXT and '%' not in s:
                cands = [('file', s)]
            elif re.match(r'(?i)^(effectset|damageeffect|damage_effect)[_A-Za-z0-9]+$', s):
                cands = [('es' if s.lower().startswith('effectset') else 'de', s)]     # June-undefined name
            for typ, nm in cands:
                k = (typ, nm.lower() if typ == 'file' else nm)
                if k not in seen:
                    seen.add(k)
                    state_roots.setdefault(st, []).append((typ, nm, '-'))
    n_states = states.n_tables
    n_nonint = sum(1 for s in states if not s.startswith('INIT_'))

    # ---- C++ roots
    log('exe literals %08x..%08x ...' % (lo, hi))
    exe = Exe(EXE)
    per_func = exe.literals(lo, hi)
    per_lit = collections.OrderedDict((fn, list(l)) for fn, l in per_func.items())
    xs = xor_strings(lo, hi)
    for fn, s in xs:
        # attribute to the containing function in per_lit (nearest start <= fn)
        starts = sorted(per_lit)
        cand = [a for a in starts if a <= fn]
        key = cand[-1] if cand else fn
        per_lit.setdefault(key, [])
        if s not in per_lit[key]:
            per_lit[key].append(s)
    n_plain = sum(len(v) for v in per_func.values())
    cpp_roots = collections.OrderedDict()        # 'fn:%08x' -> [(typ, name, note)]
    unmatched_all = []
    for fn, lits in per_lit.items():
        rts, um = roots_from_literals(D, [(l, fn) for l in lits], None)
        unmatched_all.extend((fn, l, why) for l, why in um)
        if rts:
            cpp_roots['fn:%08x' % fn] = rts
    seen_roots = set()
    for fn, rts in cpp_roots.items():
        for typ, nm, _ in rts:
            seen_roots.add((int(fn[3:], 16), typ, nm))
    for fn, typ, nm, note in roots_from_concat(D, per_lit, seen_roots):
        cpp_roots.setdefault('fn:%08x' % fn, []).append((typ, nm, note))

    # ---- walk + rows
    cache = {}
    rows = []
    status_cache = {}

    def st_of(typ, name):
        k = (typ, name.lower() if typ == 'file' else name)
        if k not in status_cache:
            status_cache[k] = march_status(D, typ, name)
        return status_cache[k]

    def walk(state, roots):
        seen = {}
        order = []
        queue = collections.deque()
        for typ, nm, note in roots:
            k = (typ, nm.lower() if typ == 'file' else nm)
            if k not in seen:
                seen[k] = ('-', note, typ, nm)
                order.append(k)
                queue.append(k)
        while queue:
            k = queue.popleft()
            typ, nm = seen[k][2], seen[k][3]
            for ctyp, cname, key in children(D, typ, nm):
                ck = (ctyp, cname.lower() if ctyp == 'file' else cname)
                if ck not in seen:
                    seen[ck] = (nm, key, ctyp, cname)
                    order.append(ck)
                    queue.append(ck)
        for k in order:
            via, note0, typ, nm = seen[k]
            st, jk, note = st_of(typ, nm)
            ns = [x for x in (note0 if note0 not in ('-', '') else '', note) if x]
            rows.append((state, node_kind(typ, nm), nm, st, jk, '-', via, ';'.join(ns)))

    for st, rts in state_roots.items():
        walk(st, rts)
    for st, rts in cpp_roots.items():
        walk(st, rts)

    # ---- write
    os.makedirs(os.path.dirname(args.out), exist_ok=True)
    kinds = ['damage-effect', 'effect-set', 'particle', 'mesh-player', 'texture', 'mesh', 'sound', 'other']
    stats = ['june-bytes', 'march-copy', 'missing']
    uniq = {}
    for r in rows:
        uniq.setdefault((r[1], r[2].lower() if r[1] in ('texture', 'mesh', 'sound', 'other') else r[2]), r)
    with open(args.out, 'w', encoding='utf-8', newline='\n') as f:
        f.write('state\tkind\tname\tmarch_status\tjune_kom\tphase\tvia\tnote\n')
        for r in rows:
            f.write('\t'.join(x.replace('\t', ' ') for x in r) + '\n')
        f.write('# total\n')
        f.write('# class script %s: %d top-level tables, %d unique non-INIT states (names defined twice: %s), %d INIT_*; %d tables reach a name; C++ range %08x..%08x: %d functions, %d plain + %d xor literals, %d functions reach a name\n'
                % (os.path.basename(sp), n_states, n_nonint, ','.join(states.dups) or 'none', len([s for s in states if s.startswith('INIT_')]), len(state_roots), lo, hi,
                   len(per_func), n_plain, len(xs), len(cpp_roots)))
        f.write('# rows %d, unique (kind,name) %d\n' % (len(rows), len(uniq)))
        for label, src in (('unique', list(uniq.values())), ('rows', rows)):
            f.write('# by kind (%s)\t%s\n' % (label, '\t'.join('%s=%d' % (k, sum(1 for r in src if r[1] == k)) for k in kinds)))
            f.write('# by march_status (%s)\t%s\n' % (label, '\t'.join('%s=%d' % (s, sum(1 for r in src if r[3] == s)) for s in stats)))
            f.write('# kind x status (%s)\n' % label)
            for k in kinds:
                f.write('#   %s\t%s\n' % (k, '\t'.join('%s=%d' % (s, sum(1 for r in src if r[1] == k and r[3] == s)) for s in stats)))
        und = [r for r in uniq.values() if 'june-undefined' in r[7]]
        f.write('# june-undefined (name the code / script asks for that June itself does not define): %d\n' % len(und))
        f.write('# cpp literals that look like templet names but match nothing in June: %d\n' % len(set(u[1] for u in unmatched_all)))
        for fn, lit, why in sorted(set(unmatched_all)):
            f.write('#   unmatched fn:%08x\t%s\t%s\n' % (fn, lit, why))
    # stdout summary
    log('rows', len(rows), 'unique', len(uniq))
    for k in kinds:
        log('  %-14s' % k, ' '.join('%s=%d' % (s, sum(1 for r in uniq.values() if r[1] == k and r[3] == s)) for s in stats))
    log('written', args.out)
    miss = sorted(r for r in uniq.values() if r[3] == 'missing')
    log('missing (unique):', len(miss))
    for r in miss[:40]:
        log('   ', r[1], r[2], 'june=%s' % r[4], r[7][:60])


if __name__ == '__main__':
    main()
