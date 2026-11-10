r"""char-ara CH0: read SetEquippedSkillFuncBySkillSlotIndex 32bd09d0 from its LISTING (not the decompile) by running each June skill id through
the dispatch with a small register/stack interpreter, and write ch0_skill_listing_rows.tsv. Copy of char_rena/ch0_skill_listing_emu.py with
Ara's frame: this = edi, normal state ptr = esi (32bd0a60), hyper state ptr = ebp (32bd0a95), skill id = [esp+0xac], start after the second 31fd88f0 call
(32bd0aa0), exit at 32bd27e9 (dtor call 31fdf970) / 32bd2800 (early out), epilogue `ret 0xc` 32bd2819. The listing has THREE switches (plus a pre-switch):
pre-switch at 32bd0af5 (V = 31fdbcc0's result with 32206450 true: V 0x6a56 / 0x6a6c -> 32babb50 on both states, no registration; emulated as skipped),
skill-type word (type - 5, 12 entries, byte tbl 32bd282c, jump tbl 32bd281c, 4 targets), id switch (id - 0x6978, 244 entries, byte tbl 32bd2930, jump tbl 32bd2838,
62 targets; id 0x6bd0 by compare), id switch above 0x6bd0 (id - 0x6bd1, 5 entries, jump tbl 32bd2a24, no byte table).

    python june2017_re/funcdis.py --func 32bd09d0 > <scratch>/sw.txt
    python june2017_re/char_ara/ch0_skill_listing_emu.py <scratch>/sw.txt

31fca8e0 returns the skill templet (+4 = skill type from s3_reform_ids_no_state.tsv); 31fdbcc0 returns V (tried as 0, the id and the base id of
m_DeleteSkillIDByUpgrade); 32206450 returns 0; 32207210 returns the base id. Events: mtr = call 32fe9a40 (first = normal, second = hyper); reg = call 31fd89b0.
"""
import re, sys, os, struct, collections

HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..'))
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
B = 0x31aa0c00
SL = {0x18: 'Init', 0x20: 'StartFuture', 0x28: 'Start', 0x30: 'FrameMoveFuture', 0x38: 'FrameMove', 0x40: 'CameraMove',
      0x48: 'EventProcess', 0x50: 'EndFuture', 0x58: 'End'}
M32 = 0xffffffff
exe = open(EXE, 'rb').read()
BASE_OF = {}
FN = 0x32bd09d0


def cstr(va):
    o = va - B
    e = exe.find(b'\0', o)
    return exe[o:e].decode('latin-1') if 0 <= o < len(exe) and 0 < e - o < 160 else None


def thunk_res(va):
    o = va - B
    if 0 <= o < len(exe) - 5 and exe[o] == 0xe9:
        return '%08x>%08x' % (va, (va + 5 + struct.unpack_from('<i', exe, o + 1)[0]) & M32)
    return '%08x' % va


def load(path):
    ins = {}
    order = []
    for l in open(path).read().splitlines():
        m = re.match(r'([0-9a-f]{8}): (\S+) ?(.*)', l)
        if m:
            a = int(m.group(1), 16)
            ins[a] = (m.group(2), m.group(3))
            order.append(a)
    return ins, order


R206 = [0]
HELPERS = {}   # class helper listings (state-setup helpers the skill switch calls: ( state*, id ) -> Init / mtr / callbacks), loaded through funcdis.py
CLO, CHI = 0x32b90000, 0x32bd9000
RE_DIR = os.path.normpath(os.path.join(HERE, '..'))


def load_helper(va, ins, order):
    """char-ara CH0: 32bd09d0 hands the state setup of most skills to class helpers (call helper( state*, flag ), ret 8); follow them like inline code."""
    import capstone
    if va in HELPERS:
        return
    HELPERS[va] = None
    md_ = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    o = va - B
    n = 0
    mine = []
    far = va
    for i_ in md_.disasm(exe[o:o + 0x1800], va):
        ins[i_.address] = (i_.mnemonic, i_.op_str)
        order.append(i_.address)
        mine.append(i_.address)
        n += 1
        if i_.mnemonic.startswith('j') and i_.op_str.startswith('0x'):
            far = max(far, int(i_.op_str, 16))
        if i_.mnemonic == 'ret' and i_.address >= far:
            break
    HELPERS[va] = n
    for a in mine:
        mn, op = ins[a]
        if mn == 'call':
            t = imm(op)
            if t is not None and CLO <= t < CHI and t != 0x32bd09d0:
                load_helper(t, ins, order)


def imm(s):
    s = s.strip()
    if re.fullmatch(r'-?0x[0-9a-f]+', s):
        return int(s, 16) & M32
    if re.fullmatch(r'-?\d+', s):
        return int(s) & M32
    return None


def sg(v):
    return v - (1 << 32) if v & 0x80000000 else v


class Stop(Exception):
    pass


class Run:
    def __init__(self, ins, order, sid, typ, V, cls=0):
        self.ins, self.order = ins, order
        self.next = {a: order[i + 1] for i, a in enumerate(order[:-1])}
        self.regs = {'eax': None, 'ebx': 'EBX', 'ecx': None, 'edx': None, 'esi': 'NORM', 'edi': 'THIS', 'ebp': 'HYP'}
        self.mem = {0xac: sid}
        self.obj = collections.defaultdict(dict)   # token -> {off: value}
        self.depth = 0
        self.hlevel = 0   # char-ara CH0: inside a class helper a push of a callee-saved register is a prologue save, not a call argument
        self.saves = 0
        self.pushes = []
        self.flags = None
        self.events = []
        self.sid, self.typ, self.V, self.cls = sid, typ, V, cls
        self.mtr = 0
        self.fld_reset = {}
        self.exits = (0x32bd27e9, 0x32bd2800)
        self.choices = None   # char-ara CH0: 32bd34c0 branches on object state; each unknown condition takes the next choice (the driver tries every vector)
        self.nunk = 0

    # operand helpers
    def memref(self, op):
        m = re.match(r'(?:(?:byte|word|dword) ptr )?\[(.*)\]$', op.strip())
        if not m:
            return None
        terms = re.split(r' ([+-]) ', m.group(1))
        sign, parts = '+', []
        base, val, unknown = None, 0, False
        for t in terms:
            if t in '+-' and len(t) == 1:
                sign = t
                continue
            t = t.strip()
            sc = re.fullmatch(r'(e[a-z]{2})\*(\d)', t)
            if sc:
                r = self.regs.get(sc.group(1))
                if isinstance(r, int):
                    val += (1 if sign == '+' else -1) * r * int(sc.group(2))
                else:
                    unknown = True
            elif t in ('esp',):
                base = 'esp'
            elif re.fullmatch(r'e[a-z]{2}', t):
                r = self.regs.get(t)
                if isinstance(r, int):
                    val += (1 if sign == '+' else -1) * r
                elif isinstance(r, str) or isinstance(r, tuple):
                    base = r
                else:
                    unknown = True
            else:
                v = imm(t)
                if v is None:
                    unknown = True
                else:
                    val += (1 if sign == '+' else -1) * v
        if unknown:
            return None
        return base, val

    def getv(self, op):
        op = op.strip()
        if re.fullmatch(r'e[a-z]{2}', op):
            return self.regs.get(op)
        v = imm(op)
        if v is not None:
            return v
        mr = self.memref(op)
        if mr is None:
            return None
        base, off = mr
        if base is None:
            o = (off & M32) - B
            if 0 <= o < len(exe) - 4:
                if op.startswith('byte'):
                    return exe[o]
                return struct.unpack_from('<I', exe, o)[0]
            return None
        if base == 'esp':
            return self.mem.get(off - 4 * self.depth)
        if base == 'OBJ':
            if off == 4:
                return self.typ
            return None
        return self.obj[base].get(off)

    def setv(self, op, v):
        op = op.strip()
        if re.fullmatch(r'e[a-z]{2}', op):
            self.regs[op] = v
            return
        mr = self.memref(op)
        if mr is None:
            return
        base, off = mr
        if base == 'esp':
            self.mem[off - 4 * self.depth] = v
        elif base is not None and not isinstance(base, tuple):
            self.obj[base][off] = v

    def unknown(self, why):
        if self.choices is None:
            raise Stop(why)
        i = self.nunk
        self.nunk += 1
        self.unk_why = why
        return self.choices[i] if i < len(self.choices) else False

    def cond(self, mn):
        r_ = self.cond2(mn)
        return r_

    def cond2(self, mn):
        f = self.flags
        if f is None:
            return self.unknown('flags unknown')
        kind, a, b = f
        if kind == 'test':
            a = 1 if isinstance(a, (str, tuple)) else a
            b = 1 if isinstance(b, (str, tuple)) else b
        if a is None or b is None or not isinstance(a, int) or not isinstance(b, int):
            return self.unknown('flag operands unknown at %s' % mn)
        if kind == 'test':
            r = a & b
            zf, sf = r == 0, bool(r & 0x80000000)
            cf = False
            of = False
        else:
            r = (a - b) & M32
            zf, sf, cf = r == 0, bool(r & 0x80000000), a < b
            of = (sg(a) - sg(b)) != sg(r)
        t = {'je': zf, 'jz': zf, 'jne': not zf, 'jnz': not zf, 'ja': not cf and not zf, 'jae': not cf, 'jb': cf, 'jbe': cf or zf,
             'jg': not zf and sf == of, 'jge': sf == of, 'jl': sf != of, 'jle': zf or sf != of, 'js': sf, 'jns': not sf}
        return t[mn]

    def go(self, start, limit=3000):
        a = start
        steps = 0
        while steps < limit:
            steps += 1
            if a not in self.ins:
                raise Stop('off listing %08x' % a)
            mn, op = self.ins[a]
            nxt = self.next.get(a)
            if a in self.exits:
                return 'exit'
            if mn == 'jmp':
                if op.startswith('dword ptr'):
                    mr = self.memref(op)
                    base, off = mr if mr else (None, None)
                    if base is None and off is not None:
                        o = (off & M32) - B
                        return self.stop_jump(a, op)
                    raise Stop('jmp table unknown %08x' % a)
                a = imm(op)
                continue
            if mn.startswith('j') and mn != 'jmp':
                if self.cond(mn):
                    a = imm(op)
                    continue
                a = nxt
                continue
            if mn == 'push':
                if self.hlevel and op.strip() in ('esi', 'edi', 'ebx', 'ebp'):
                    self.saves += 1
                    self.depth += 1
                    a = nxt
                    continue
                v = self.getv(op)
                self.pushes.append(v)
                self.depth += 1
            elif mn == 'pop':
                self.depth -= 1
                if self.hlevel and op.strip() in ('esi', 'edi', 'ebx', 'ebp'):
                    self.saves -= 1
                    a = nxt
                    continue
                self.setv(op, None)
            elif mn == 'call':
                t = imm(op)
                if t is not None and CLO <= t < CHI and HELPERS.get(t):
                    args = [x for x in reversed(self.pushes[-3:])]   # args[0] = last pushed = [esp + 4]
                    saved = (dict(self.regs), self.mem, self.depth, self.pushes, self.saves)
                    self.saves = 0
                    self.hlevel += 1
                    self.mem = {0: nxt}
                    for k_, v_ in enumerate(args[:2]):
                        self.mem[4 + 4 * k_] = ('LOC%d' % v_[1]) if isinstance(v_, tuple) else v_
                    self.depth, self.pushes = 0, []
                    res_ = self.go(t, limit - steps)
                    if res_ != 'ret':
                        raise Stop('helper %08x ended %s' % (t, res_))
                    self.hlevel -= 1
                    regs_, self.mem, self.depth, self.pushes, self.saves = saved
                    for r_ in ('ebx', 'esi', 'edi', 'ebp'):
                        self.regs[r_] = regs_[r_]
                    self.regs['eax'] = self.regs['ecx'] = self.regs['edx'] = None
                    self.depth = self.saves
                    self.pushes = []
                    a = nxt
                    continue
                if t == 0x32fe9a40:
                    nm = self.pushes[-1] if self.pushes else None
                    idv = self.pushes[-2] if len(self.pushes) > 1 else None
                    if isinstance(idv, int):
                        self.last_extra = (cstr(nm) if isinstance(nm, int) else None, idv)
                    else:
                        self.mtr += 1
                        self.events.append(('mtr', a, cstr(nm) if isinstance(nm, int) else None, self.mtr,
                                            dict(self.obj['NORM']), dict(self.obj['HYP'])))
                elif t == 0x31fd8970 and len(self.pushes) >= 2 and getattr(self, 'last_extra', None):
                    tok = self.pushes[-2]
                    cbs = {}
                    if isinstance(tok, tuple):
                        for k_, v_ in self.obj['LOC%d' % tok[1]].items():
                            if k_ in SL and isinstance(v_, int) and v_ and CLO <= v_ < CHI:
                                cbs[k_] = v_
                    self.events.append(('reg', a, self.last_extra[0], self.pushes[-1], cbs))
                elif t == 0x31fd89b0:
                    p = self.pushes[-3:]
                    fr = {k: v for k, v in self.mem.items()}
                    base = p[0][1] if len(p) == 3 and isinstance(p[0], tuple) else None
                    cbs = {}
                    if base is not None:
                        for k, v in fr.items():
                            if (k - base) in SL and isinstance(v, int) and v and CLO <= v < CHI:
                                cbs[k - base] = v
                    if len(p) == 3 and isinstance(p[0], tuple):
                        for k_, v_ in self.obj['LOC%d' % p[0][1]].items():
                            if k_ in SL and isinstance(v_, int) and v_ and CLO <= v_ < CHI:
                                cbs.setdefault(k_, v_)
                    nm = p[1] if len(p) == 3 else None
                    self.events.append(('reg', a, cstr(nm) if isinstance(nm, int) else None, p[2] if len(p) == 3 else None, cbs))
                elif t in (0x320775a0, 0x32078bc0):
                    # clears the local struct whose address is in ecx
                    c = self.regs.get('ecx')
                    if isinstance(c, tuple):
                        self.obj['LOC%d' % c[1]].clear()
                        for k in [k for k in self.mem if c[1] <= k < c[1] + 0x70]:
                            del self.mem[k]
                self.depth = self.saves
                self.pushes = []
                ret = None
                if t == 0x31fca8e0:
                    ret = 'OBJ'
                elif t == 0x31fdbcc0:
                    ret = self.V
                elif t == 0x32207210:
                    ret = BASE_OF.get(self.sid, self.sid)
                elif t == 0x32206450:
                    ret = R206[0]   # char-ara CH0: 1 only in the memo pre-switch scenario (V = 0x6a56 / 0x6a6c)
                elif t in (0x31e4d0c0, 0x31fceab0):   # char-ara CH0: 0x31fceab0 (this, 0x6b1c) is a buff query, both results are tried (cls = 0 / 1)
                    ret = self.cls
                self.regs['eax'] = ret
                self.regs['ecx'] = None
                self.regs['edx'] = None
            elif mn in ('mov', 'movzx'):
                d, s = [x.strip() for x in op.split(',', 1)]
                v = self.getv(s)
                if re.fullmatch(r'[a-d][lh]|[a-d]x', d):
                    self.regs['e' + d[0] + 'x'] = None
                elif mn == 'mov' and 'ptr' in d and d.startswith(('byte', 'word')):
                    pass
                else:
                    self.setv(d, v)
            elif mn == 'lea':
                d, s = [x.strip() for x in op.split(',', 1)]
                mr = self.memref(s)
                if mr is None:
                    self.regs[d] = None
                elif mr[0] == 'esp':
                    self.regs[d] = ('lea', mr[1] - 4 * self.depth)
                elif mr[0] is None:
                    self.regs[d] = mr[1] & M32
                else:
                    self.regs[d] = None
            elif mn == 'xor':
                d, s = [x.strip() for x in op.split(',', 1)]
                if d == s:
                    self.setv(d, 0)
                    self.flags = ('test', 0, 0)
                else:
                    self.setv(d, None)
            elif mn in ('add', 'sub'):
                d, s = [x.strip() for x in op.split(',', 1)]
                if d == 'esp':
                    self.depth -= (imm(s) or 0) // 4 if mn == 'add' else 0
                    self.depth = max(0, self.depth)
                else:
                    x, y = self.getv(d), self.getv(s)
                    if isinstance(x, int) and isinstance(y, int):
                        r = (x + y if mn == 'add' else x - y) & M32
                        self.setv(d, r)
                        self.flags = ('cmp', r, 0)
                    else:
                        self.setv(d, None)
                        self.flags = None
            elif mn == 'cmp':
                d, s = [x.strip() for x in op.split(',', 1)]
                self.flags = ('cmp', self.getv(d), self.getv(s))
            elif mn == 'test':
                d, s = [x.strip() for x in op.split(',', 1)]
                self.flags = ('test', self.getv(d), self.getv(s))
            elif mn in ('ret', 'nop'):
                if mn == 'ret':
                    return 'ret'
            else:
                # unknown instruction: forget the destination
                if ',' in op:
                    self.setv(op.split(',', 1)[0], None)
            a = nxt

    def stop_jump(self, a, op):
        """jmp dword ptr [reg*4 + table]: read the table, pick by the register."""
        m = re.match(r'dword ptr \[(e[a-z]{2})\*4 \+ (0x[0-9a-f]+)\]', op)
        if not m:
            raise Stop('jmp form')
        r = self.regs.get(m.group(1))
        if not isinstance(r, int):
            raise Stop('jmp table index unknown')
        t = int(m.group(2), 16) - B + 4 * r
        tgt = struct.unpack_from('<I', exe, t)[0]
        return ('goto', tgt)


def run_id(ins, order, sid, typ, V, cls=0):
    r = Run(ins, order, sid, typ, V, cls)
    start = 0x32bd0aa0
    a = start
    r.regs['ecx'] = None
    for _ in range(50):
        try:
            res = r.go(a)
        except Stop as e:
            return r, 'stop: %s' % e
        if isinstance(res, tuple):
            a = res[1]
            continue
        return r, res
    return r, 'loop'


FN2 = 0x32bd34c0


def load_range(lo, hi, ins, order):
    import capstone
    md_ = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    for i_ in md_.disasm(exe[lo - B:hi - B], lo):
        ins[i_.address] = (i_.mnemonic, i_.op_str)
        order.append(i_.address)


def run_id2(ins, order, sid):
    """char-ara CH0: vslot 0x68c 32bd34c0 (a second skill-id switch: id - 0x6979, 242 entries, byte tbl 32bd5150, jump tbl 32bd50f4) from the dispatch at 32bd3541.
    this = esi, id = ebx, edi = the skill templet (non-null). Every unknown branch is explored (vectors of choices)."""
    results = []
    k = 0
    vecs = [[]]
    seen = set()
    while vecs and len(results) < 64:
        ch = vecs.pop()
        if tuple(ch) in seen:
            continue
        seen.add(tuple(ch))
        r = Run(ins, order, sid, 6, 0, 0)
        r.exits = (0x32bd50bc, 0x32bd50c8)
        r.choices = list(ch)
        r.regs.update({'eax': None, 'ebx': sid, 'ecx': None, 'edx': None, 'esi': 'THIS', 'edi': 'TPL', 'ebp': None})
        r.mem = {0x344: sid, 0x78: sid}
        a = 0x32bd3541
        res = 'loop'
        for _ in range(50):
            try:
                res = r.go(a)
            except Stop as e:
                res = 'stop: %s' % e
                break
            if isinstance(res, tuple):
                a = res[1]
                continue
            break
        results.append((r, res, ch))
        for j in range(len(ch), r.nunk):   # a vector for each unknown branch first met on this path: flip it
            vecs.append(list(ch) + [False] * (j - len(ch)) + [True])
    return results


def cbstr(d):
    return ' '.join('%s=%s' % (SL[o], thunk_res(v)) for o, v in sorted(d.items()) if o in SL and isinstance(v, int) and v)


def main():
    ins, order = load(sys.argv[1])
    for a_ in list(order):
        mn_, op_ = ins[a_]
        if mn_ == 'call':
            t_ = imm(op_)
            if t_ is not None and CLO <= t_ < CHI and t_ != FN:
                load_helper(t_, ins, order)
    print('helpers loaded', sum(1 for v in HELPERS.values() if v))
    sk = {}
    for l in open(os.path.join(T, 'june2017_re/s3_reform_ids_no_state.tsv'), encoding='utf-8').read().splitlines()[1:]:
        f = l.split('\t')
        if f[2] != 'X2GUAra.cpp':
            continue
        sk[int(f[3])] = (f[4], int(f[6]))
    base_of = BASE_OF
    for blk in open(os.path.join(T, 'june_src_fixed/data036_kom/NewSkillTempletVer2.src.lua'), encoding='utf-8', errors='replace').read().split('AddSkillTemplet_LUA('):
        me = re.search(r'^	m_eID = (\d+),', blk, re.M)
        md = re.search(r'^	m_DeleteSkillIDByUpgrade = (\d+),', blk, re.M)
        if me and md and int(md.group(1)):
            base_of[int(me.group(1))] = int(md.group(1))
    oldids = set()
    for l in open(os.path.join(HERE, 'ch0_skill.tsv')).read().splitlines()[1:]:
        for c in l.split('	')[2].split(','):
            if c.isdigit() and int(c) > 1000:
                oldids.add(int(c))
    ids = sorted(set(sk) | oldids | {900} | set(range(27000, 27700)))   # char-ara CH0: every id of the id switch (27000-27243, 0x6bd0-0x6bd5), not only the June tree's
    # call address -> accumulated record
    rec = collections.OrderedDict()
    outcome = {}
    for sid in ids:
        typs = [sk[sid][1]] if sid in sk else ([12] if sid == 900 else [6])
        for ((vname, V), typ), cls in [((vv, tt), cc) for cc in (0, 1) for vv in ((('0', 0), ('id', sid), ('base', base_of.get(sid, 0))) if sid in sk or sid == 900 else (('0', 0),)) for tt in typs]:
            r, res = run_id(ins, order, sid, typ, V, cls)
            evs = [e for e in r.events]
            outcome[(sid, vname)] = (res, outcome.get((sid, vname), (0, []))[1] + [e[1] for e in evs])
            if not evs:
                continue
            mt = [e for e in evs if e[0] == 'mtr']
            for e in evs:
                if e[0] == 'mtr':
                    role = 'normal' if e[3] == 1 else 'hyper'
                    cb = dict(r.obj['NORM']) if e[3] == 1 else dict(r.obj['HYP'])
                    key = (e[1], e[2], role)
                    ent = rec.setdefault(key, {'id': None, 'ids': set(), 'cb': cb, 'how': set(), 'cls': set()})
                    ent['ids'].add(sid)
                    ent['how'].add(vname); ent['cls'].add(cls)
                else:
                    key = (e[1], e[2], 'reg')
                    ent = rec.setdefault(key, {'id': e[3], 'ids': set(), 'cb': e[4], 'how': set(), 'cls': set()})
                    ent['ids'].add(sid)
                    ent['how'].add(vname); ent['cls'].add(cls)
    load_range(FN2, 0x32bd50f0, ins, order)
    for a_ in list(order):
        mn_, op_ = ins[a_]
        if mn_ == 'call' and FN2 <= a_ < 0x32bd50f0:
            t_ = imm(op_)
            if t_ is not None and CLO <= t_ < CHI and t_ not in (FN, FN2):
                load_helper(t_, ins, order)
    # pre-switch 32bd0af5: V (31fdbcc0, the memo-replaced id) 0x6a56 / 0x6a6c (RNSI_A_ALD_CONVERSION_HEALTH_ADD_TRANS / _UPGRADE) with a templet (32206450 true) -> helper 32babb50 on both states
    R206[0] = 1
    for sid, V in ((27210, 0x6a56), (27235, 0x6a6c)):
        r, res = run_id(ins, order, sid, 6, V, 0)
        for e in r.events:
            if e[0] == 'mtr':
                role = 'normal' if e[3] == 1 else 'hyper'
                ent = rec.setdefault((e[1], e[2], role), {'id': None, 'ids': set(), 'cb': dict(r.obj['NORM'] if e[3] == 1 else r.obj['HYP']), 'how': set(), 'cls': set()})
                ent['ids'].add(sid)
                ent['how'].add('memo V=0x%x' % V)
                ent['cls'].add(0)
    R206[0] = 0
    n2 = collections.Counter()
    for sid in range(27001, 27243):
        for r, res, ch in run_id2(ins, order, sid):
            n2[res.split(':')[0]] += 1
            if res != 'exit':
                n2['stop ' + res] += 1
            for e in r.events:
                if e[0] != 'reg':
                    continue
                key = (e[1], e[2], 'reg')
                ent = rec.setdefault(key, {'id': e[3], 'ids': set(), 'cb': e[4], 'how': set(), 'cls': set(), 'fn': '32bd34c0'})
                ent['ids'].add(sid)
                ent['how'].add('v68c')
                ent['cls'].add(len(ch))
    print('32bd34c0 runs', dict(n2))
    # hyper callbacks: stores into HYP at the time of the second mtr call (copies from NORM are followed by reading obj['HYP'])
    with open(os.path.join(HERE, 'ch0_skill_listing_rows.tsv'), 'w', encoding='utf-8', newline='\n') as o:
        o.write('fn\tcall_va\tstate_id\ttable\tskill_ids\trole\tcallbacks\tlisting_note\n')
        for (a, nm, role), e in rec.items():
            sidv = e['id'] if isinstance(e['id'], int) else '-'
            o.write('%s\t%08x\t%s\t%s\t%s\t%s\t%s\t%s\n' % (e.get('fn', '32bd09d0'), a, sidv, nm, ','.join(map(str, sorted(e['ids']))), role, cbstr(e['cb']),
                                                                'V=' + '/'.join(sorted(e['how'])) + (('; 31fceab0=' + '/'.join(str(x) for x in sorted(e['cls'])) if e['cls'] != {0, 1} else '') if e.get('fn') is None else '')))
    noev = [sid for sid in ids if not any(outcome.get((sid, v), (0, []))[1] for v in ('0', 'id', 'base'))]
    print('ids', len(ids), 'rows', len(rec), 'ids with no event in any scenario', len(noev), noev)
    print('stops', collections.Counter(outcome.get((s_, v), ('-',))[0] for s_ in noev for v in ('0', 'id', 'base')))


main()
