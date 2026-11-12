r"""char-elesis CH0: read SetEquippedSkillFuncBySkillSlotIndex 32b8c470 from its LISTING (not the decompile) by running each June skill id through
the dispatch with a small register/stack interpreter, and write ch0_skill_listing_rows.tsv + ch0_skill_switches.tsv. Copy of
char_ara/ch0_skill_listing_emu.py with Elesis's frame: this = edi, normal state ptr = esi, hyper state ptr = ebp (both from 31fd88f0 at 32b8c503 / 32b8c538),
hyper id = ebx, skill id = [esp+0xb0], slot index [esp+0xb4] (< 5), hyper flag byte [esp+0xb8]; start after the second 31fd88f0 (32b8c548), exit at 32b8ea89
(dtor call 31fdf970), epilogue `ret 0xc` 32b8eab9. The listing has FOUR jump-table switches and two compares:
  (a) 32b8c5bb  V - 28031 (V = 31fdbcc0 result, only when V != 0 and 32206450( V, 1 ) is true), 31 entries, byte tbl 32b8eac8, jump tbl 32b8eabc (3 targets)
  (b) 32b8c7e0  skill type - 5 (templet +4), 12 entries, byte tbl 32b8eaf8, jump tbl 32b8eae8 (4 targets)
  (c) 32b8c875  id - 28000, 237 entries, byte tbl 32b8ebe0, jump tbl 32b8eb04 (55 targets); id 28600 (0x6fb8) by compare -> 32b8e95c
  (d) 32b8e955  id - 28601, 5 entries, jump tbl 32b8ecd0 (no byte table; 3 distinct targets)
  compare: id 0x384 (900) at 32b8ea22 (type 12 target of (b)).
Hyper copies: call 329cc2c0( src ) on ecx copies slots 0x14..0x58 (emulated).

    python june2017_re/funcdis.py --func 32b8c470 > <scratch>/sw.txt
    python june2017_re/char_elesis/ch0_skill_listing_emu.py <scratch>/sw.txt

31fca8e0 returns the skill templet (+4 = skill type from s3_reform_ids_no_state.tsv, family X2GUEL.cpp); 31fdbcc0 returns V (tried as 0, the id and the base id of
m_DeleteSkillIDByUpgrade); 32206450 returns 0 except in the V scenario; 32207210 returns the base id. Events: mtr = call 32fe9a40 (first = normal, second = hyper);
reg = call 31fd89b0. Every unknown branch (memo / skill-level checks near 32b8e06c) is explored with a vector of choices.
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
FN = 0x32b8c470


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
CLO, CHI = 0x32b4e000, 0x32b91000
RE_DIR = os.path.normpath(os.path.join(HERE, '..'))


def load_helper(va, ins, order):
    """char-elesis CH0: 32b8c470 (no class helpers are called; kept from the Ara copy) hands the state setup of most skills to class helpers (call helper( state*, flag ), ret 8); follow them like inline code."""
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
            if t is not None and CLO <= t < CHI and t != 0x32b8c470:
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
        self.mem = {0xb0: sid}
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
        self.exits = (0x32b8ea89, 0x32b8eaa0)
        self.choices = None   # char-elesis CH0: unknown conditions (memo checks near 32b8e06c) branch on object state; each unknown condition takes the next choice (the driver tries every vector)
        self.nunk = 0
        self.hits = []   # (switch table base, index, target)

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
                elif t == 0x329cc2c0:
                    src_, dst_ = (self.pushes[-1] if self.pushes else None), self.regs.get('ecx')
                    if isinstance(src_, str) and isinstance(dst_, str):
                        for k_, v_ in list(self.obj[src_].items()):
                            if 0x14 <= k_ <= 0x58:
                                self.obj[dst_][k_] = v_
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
        self.hits.append((int(m.group(2), 16), r, tgt))
        return ('goto', tgt)


START = 0x32b8c548
JT = {0x32b8eabc: ('a', 28031, 0x32b8eac8, 31), 0x32b8eae8: ('b', 5, 0x32b8eaf8, 12), 0x32b8eb04: ('c', 28000, 0x32b8ebe0, 237), 0x32b8ecd0: ('d', 28601, None, 5)}


def explore(sid, typ, V, cls, r206, ins, order):
    """Run the dispatch for one scenario, trying every unknown branch (vector of choices); returns [(run, result)]."""
    R206[0] = r206
    out = []
    vecs, seen = [[]], set()
    while vecs and len(out) < 64:
        ch = vecs.pop()
        if tuple(ch) in seen:
            continue
        seen.add(tuple(ch))
        r = Run(ins, order, sid, typ, V, cls)
        r.choices = list(ch)
        r.regs.update({'ebx': 'EBX', 'esi': 'NORM', 'edi': 'THIS', 'ebp': 'HYP'})
        a, res = START, 'loop'
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
        out.append((r, res))
        for j in range(len(ch), r.nunk):
            vecs.append(list(ch) + [False] * (j - len(ch)) + [True])
    R206[0] = 0
    return out


def cbstr(d):
    return ' '.join('%s=%s' % (SL[o], thunk_res(v)) for o, v in sorted(d.items()) if o in SL and isinstance(v, int) and v)


def main():
    ins, order = load(sys.argv[1])
    sk = {}
    for l in open(os.path.join(T, 'june2017_re/s3_reform_ids_no_state.tsv'), encoding='utf-8').read().splitlines()[1:]:
        f = l.split('\t')
        if f[2] != 'X2GUEL.cpp':
            continue
        sk[int(f[3])] = (f[4], int(f[6]))
    for blk in open(os.path.join(T, 'june_src_fixed/data036_kom/NewSkillTempletVer2.src.lua'), encoding='utf-8', errors='replace').read().split('AddSkillTemplet_LUA('):
        me = re.search(r'^	m_eID = (\d+),', blk, re.M)
        md = re.search(r'^	m_DeleteSkillIDByUpgrade = (\d+),', blk, re.M)
        if me and md and int(md.group(1)):
            BASE_OF[int(me.group(1))] = int(md.group(1))
    ids = sorted(set(sk) | {900} | set(range(28000, 28237)) | set(range(28600, 28606)))   # every id of switches (c) / (d) and the compares, not only the June tree's
    rec = collections.OrderedDict()
    percase = {}      # (switch, case) -> (target, [events])
    outcome = collections.Counter()

    def absorb(sid, r, res, vname, cls, typ=None, V=None):
        outcome[res.split(':')[0] if res != 'exit' else 'exit'] += 1
        evs = r.events
        for e in evs:
            if e[0] == 'mtr':
                role = 'normal' if e[3] == 1 else 'hyper'
                cb = dict(r.obj['NORM']) if e[3] == 1 else dict(r.obj['HYP'])
                key = (e[1], e[2], role)
                ent = rec.setdefault(key, {'id': None, 'ids': set(), 'cb': cb, 'how': set(), 'cls': set()})
            else:
                key = (e[1], e[2], 'reg')
                ent = rec.setdefault(key, {'id': e[3], 'ids': set(), 'cb': e[4], 'how': set(), 'cls': set()})
            ent['ids'].add(sid)
            ent['how'].add(vname)
            ent['cls'].add(cls)
        lab = sorted(set(('normal ' if e[3] == 1 else 'hyper ') + (e[2] or '?') if e[0] == 'mtr' else 'reg id %s %s' % (e[3], e[2] or '?') for e in evs))
        # which switch the run's case belongs to: the LAST jump-table hit decides what it registered, an earlier hit only routes
        letters = [JT[h[0]][0] for h in r.hits]
        for n_, (tb, idx, tgt) in enumerate(r.hits):
            L_ = JT[tb][0]
            cv = {'a': V, 'b': typ, 'c': sid, 'd': sid}[L_]
            if cv is None or (sid in (900, 28600) and L_ != 'b'):
                continue
            if L_ == 'b' and sid in (900, 28600) and typ is None:
                continue
            last = n_ == len(r.hits) - 1
            cur = percase.setdefault((L_, cv), [tgt, set(), set()])
            if last:
                cur[1].update(lab)
            else:
                cur[2].add('-> switch (%s)' % JT[r.hits[n_ + 1][0]][0])
        if sid in (900, 28600) and typ is None:
            cur = percase.setdefault(('cmp', sid), [0x32b8ea22 if sid == 900 else 0x32b8e95c, set(), set()])
            cur[1].update(lab)
    noev = []
    for sid in ids:
        typs = [sk[sid][1]] if sid in sk else ([12] if sid == 900 else list(range(5, 17)))
        got = False
        for vname, V in ((('0', 0),) if sid not in sk else (('0', 0), ('base', BASE_OF.get(sid, 0)))):
            for typ in typs:
                for cls in (0,):
                    for r, res in explore(sid, typ, V, cls, 0, ins, order):
                        absorb(sid, r, res, vname, cls)
                        got = got or bool(r.events)
        if not got:
            noev.append(sid)
    # switch (a): V = the memo-replaced id 28031..28061 with a templet (32206450 true)
    for V in range(28031, 28062):
        for r, res in explore(V, 6, V, 0, 1, ins, order):
            absorb(V, r, res, 'memo V=%d' % V, 0, 6, V)
    # switch (b): one run per type with the first id of that type (type 5 / 8..11 / 12 have no id switch)
    for typ in range(5, 17):
        sid = next((s for s in sorted(sk) if sk[s][1] == typ), 900 if typ == 12 else 0)
        for r, res in explore(sid, typ, 0, 0, 0, ins, order):
            absorb(sid, r, res, 'type %d' % typ, 0, typ)
    with open(os.path.join(HERE, 'ch0_skill_listing_rows.tsv'), 'w', encoding='utf-8', newline='\n') as o:
        o.write('fn\tcall_va\tstate_id\ttable\tskill_ids\trole\tcallbacks\tlisting_note\n')
        for (a, nm, role), e in rec.items():
            sidv = e['id'] if isinstance(e['id'], int) else '-'
            o.write('%s\t%08x\t%s\t%s\t%s\t%s\t%s\t%s\n' % ('32b8c470', a, sidv, nm, ','.join(map(str, sorted(e['ids']))), role, cbstr(e['cb']),
                                                         'V=' + '/'.join(sorted(e['how']))))
    print('ids', len(ids), 'rows', len(rec), 'outcomes', dict(outcome), 'ids with no event', len(noev))
    print('no-event ids', noev)
    SWN = {'a': '(a) 32b8c5bb V-28031', 'b': '(b) 32b8c7e0 type-5', 'c': '(c) 32b8c875 id-28000', 'd': '(d) 32b8e955 id-28601', 'cmp': 'compare'}
    allcases = []
    for tb, (L_, lo, bt, n) in JT.items():
        m_ = 4
        jt = [struct.unpack_from("<I", exe, tb - B + 4 * i)[0] for i in range({"a": 3, "b": 4, "c": 55, "d": 5}[L_])]
        for k in range(n):
            idx = exe[bt - B + k] if bt else k
            allcases.append((L_, lo + k, jt[idx]))
    allcases += [('cmp', 900, 0x32b8ea22), ('cmp', 28600, 0x32b8e95c)]
    with open(os.path.join(HERE, 'ch0_skill_switches.tsv'), 'w', encoding='utf-8', newline='\n') as o:
        o.write('switch\tcase_value\ttarget_va\tskill_name\tregisters\n')
        for L_, cv, tgt in allcases:
            pc = percase.get((L_, cv))
            reg = '; '.join(sorted(pc[1])) if pc and pc[1] else ''
            if pc and pc[2]:
                reg = ('; '.join(sorted(pc[2])) + ('; ' + reg if reg else ''))
            if L_ == 'b' and tgt == 0x32b8c846:
                reg = '-> id dispatch 32b8c846: switch (c), compare 28600, switch (d)'
            if not reg:
                reg = 'nothing (exit)' if tgt == 0x32b8ea89 else 'no event emulated'
            o.write('%s\t%d\t%08x\t%s\t%s\n' % (SWN[L_], cv, tgt, sk.get(cv, ('',))[0], reg))
    print('switch rows', len(allcases))


main()
