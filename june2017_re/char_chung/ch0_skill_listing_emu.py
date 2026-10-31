r"""char-chung CH0: read SetEquippedSkillFunc 32c1f530 from its LISTING (not the decompile) by running each June skill id through the
dispatch with a small register/stack interpreter, and write ch0_skill_listing_rows.tsv.

    python june2017_re/funcdis.py --func 32c1f530 > <scratch>/sw.txt
    python june2017_re/char_chung/ch0_skill_listing_emu.py <scratch>/sw.txt

Inputs known to the interpreter: [esp+0xac] = skill id; 31fca8e0 returns the skill templet (its +4 = skill type, from
s3_reform_ids_no_state.tsv); 31fdbcc0 returns the switch value V (tried as 0, the id, and the base id of m_DeleteSkillIDByUpgrade);
32206450 returns 1. Every other call returns an unknown. A branch on an unknown stops that run. Per run the events are:
  mtr  = call 32fe9a40 MakeTableReference (name = last push; first call = normal state, second = hyper state);
  reg  = call 31fd89b0 UpdateUserUnitStateData (pushes: callback frame, name, state id);
callbacks are the dword stores into [edi+off] (normal state) / [ebp+off] (hyper state; copies from edi are followed) before the call that
consumes them, or into the local frame relative to the lea behind the first push of a 31fd89b0 call. Slots: +0x18 Init, +0x20 StartFuture,
+0x28 Start, +0x30 FrameMoveFuture, +0x38 FrameMove, +0x40 CameraMove, +0x48 EventProcess, +0x50 EndFuture, +0x58 End.
Output rows (one per state registered on a path): fn, call_va, state_id, table, skill_ids (all ids whose run produced the same call),
role, callbacks (slot=thunk>target), listing_note.
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
FN = 0x32c1f530


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
        self.regs = {'eax': None, 'ebx': 'EBX', 'ecx': None, 'edx': None, 'esi': 'THIS', 'edi': 'NORM', 'ebp': 'HYP'}
        self.mem = {0xac: sid}
        self.obj = collections.defaultdict(dict)   # token -> {off: value}
        self.depth = 0
        self.pushes = []
        self.flags = None
        self.events = []
        self.sid, self.typ, self.V, self.cls = sid, typ, V, cls
        self.mtr = 0
        self.fld_reset = {}

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

    def cond(self, mn):
        f = self.flags
        if f is None:
            raise Stop('flags unknown')
        kind, a, b = f
        if kind == 'test':
            a = 1 if isinstance(a, (str, tuple)) else a
            b = 1 if isinstance(b, (str, tuple)) else b
        if a is None or b is None or not isinstance(a, int) or not isinstance(b, int):
            raise Stop('flag operands unknown at %s' % mn)
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
            if a in (0x32c22234, 0x32c2221d):
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
                v = self.getv(op)
                self.pushes.append(v)
                self.depth += 1
            elif mn == 'pop':
                self.depth -= 1
                self.setv(op, None)
            elif mn == 'call':
                t = imm(op)
                if t == 0x32fe9a40:
                    nm = self.pushes[-1] if self.pushes else None
                    self.mtr += 1
                    self.events.append(('mtr', a, cstr(nm) if isinstance(nm, int) else None, self.mtr,
                                        dict(self.obj['NORM']), dict(self.obj['HYP'])))
                elif t == 0x31fd89b0:
                    p = self.pushes[-3:]
                    fr = {k: v for k, v in self.mem.items()}
                    base = p[0][1] if len(p) == 3 and isinstance(p[0], tuple) else None
                    cbs = {}
                    if base is not None:
                        for k, v in fr.items():
                            if (k - base) in SL and isinstance(v, int) and v and 0x32bd0000 <= v < 0x32c30000:
                                cbs[k - base] = v
                    nm = p[1] if len(p) == 3 else None
                    self.events.append(('reg', a, cstr(nm) if isinstance(nm, int) else None, p[2] if len(p) == 3 else None, cbs))
                elif t == 0x320775a0:
                    # clears the local struct whose address is in ecx
                    c = self.regs.get('ecx')
                    if isinstance(c, tuple):
                        for k in [k for k in self.mem if c[1] <= k < c[1] + 0x70]:
                            del self.mem[k]
                self.depth = 0
                self.pushes = []
                ret = None
                if t == 0x31fca8e0:
                    ret = 'OBJ'
                elif t == 0x31fdbcc0:
                    ret = self.V
                elif t == 0x32206450:
                    ret = 1
                elif t == 0x31e4d0c0:
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
    start = 0x32c1f60d
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


def cbstr(d):
    return ' '.join('%s=%s' % (SL[o], thunk_res(v)) for o, v in sorted(d.items()) if o in SL and isinstance(v, int) and v)


def main():
    ins, order = load(sys.argv[1])
    sk = {}
    for l in open(os.path.join(T, 'june2017_re/s3_reform_ids_no_state.tsv'), encoding='utf-8').read().splitlines()[1:]:
        f = l.split('\t')
        if f[2] != 'X2GUChung.cpp':
            continue
        sk[int(f[3])] = (f[4], int(f[6]))
    base_of = {}
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
    ids = sorted(set(sk) | oldids | {900, 26600})
    # call address -> accumulated record
    rec = collections.OrderedDict()
    outcome = {}
    for sid in ids:
        typs = [sk[sid][1]] if sid in sk else ([12] if sid == 900 else [6, 13, 14, 15, 16])
        for ((vname, V), typ), cls in [((vv, tt), cc) for cc in (0, 110) for vv in ((('0', 0), ('id', sid), ('base', base_of.get(sid, 0))) if sid in sk or sid == 900 else (('id', sid),)) for tt in typs]:
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
    # hyper callbacks: stores into HYP at the time of the second mtr call (copies from NORM are followed by reading obj['HYP'])
    with open(os.path.join(HERE, 'ch0_skill_listing_rows.tsv'), 'w', encoding='utf-8', newline='\n') as o:
        o.write('fn\tcall_va\tstate_id\ttable\tskill_ids\trole\tcallbacks\tlisting_note\n')
        for (a, nm, role), e in rec.items():
            sidv = e['id'] if isinstance(e['id'], int) else '-'
            o.write('32c1f530\t%08x\t%s\t%s\t%s\t%s\t%s\t%s\n' % (a, sidv, nm, ','.join(map(str, sorted(e['ids']))), role, cbstr(e['cb']),
                                                                'V=' + '/'.join(sorted(e['how'])) + ('' if e['cls'] == {0, 110} else ' class=%s' % ('110 only' if e['cls'] == {110} else 'not 110'))))
    noev = [sid for sid in ids if not any(outcome.get((sid, v), (0, []))[1] for v in ('0', 'id', 'base'))]
    print('ids', len(ids), 'rows', len(rec), 'ids with no event in any scenario', len(noev), noev)
    print('stops', collections.Counter(outcome.get((s_, v), ('-',))[0] for s_ in noev for v in ('0', 'id', 'base')))


main()
