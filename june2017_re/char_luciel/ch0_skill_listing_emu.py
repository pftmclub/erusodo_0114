r"""char-luciel CH0: read SetEquippedSkillFuncBySkillSlotIndex 32b46580 (vslot 0x7b4) from its LISTING (Ghidra cannot decompile it: not
carved) by running each June skill id through the dispatch with a small register/stack interpreter. Adapted from char_chung/ch0_skill_listing_emu.py.

    python june2017_re/char_luciel/ch0_skill_listing_emu.py            (reads ghidra/disasm_32b46580_SetEquippedSkillFunc_capstone.txt)

Arguments (after the prologue, esp-relative at depth 0): [esp+0x20c] = skill id, [esp+0x210] = slot index (0..4), [esp+0x214] = bool (0 = Lu,
1 = Ciel: picks the state-id tables 334ed540 / 334ed554 (Lu normal / hyper) or 334ed568 / 334ed57c (Ciel)). The two 31fd88f0 calls return the
normal (edi) and the hyper (ebp) UserUnitStateData of those ids. 31fca8e0 returns the skill templet (its +4 = m_eType, from NewSkillTempletVer2),
32207210( id ) = m_DeleteSkillIDByUpgrade of the templet or the id itself, 31fdbcc0 returns V (tried as 0 and the id), 32206450 returns 1.
Every other call returns an unknown; a branch on an unknown stops that run, except a branch on a call result (32070d40 al == 1) which is run
both ways. Events per run: mtr = call 32fe9a40 MakeTableReference (first = normal state, second = hyper state); reg = call 31fd89b0
UpdateUserUnitStateData (pushes: callback frame, name, state id).
Callbacks are the dword stores into [edi+off] (normal) / [ebp+off] (hyper) at the end of the run (329cc2c0 copies every normal callback into the
hyper data), or into the local frame relative to the lea behind the first push of a 31fd89b0 call. A callback made by a delegate maker
( 32b4c8b0 / 32b4e4a0 / 329cd4a0 ... : *dest = this; dest[1] = fn ) is the maker's constant. Slots: +0x18 Init, +0x20 StartFuture, +0x28 Start,
+0x30 FrameMoveFuture, +0x38 FrameMove, +0x40 CameraMove, +0x48 EventProcess, +0x50 EndFuture, +0x58 End.
Output: ch0_skill_listing_rows.tsv (one row per registering call site: skill ids that reach it).
"""
import re, sys, os, struct, collections, pickle
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ch0_lu_data as D

HERE, B = D.HERE, D.B
SL = {0x18: 'Init', 0x20: 'StartFuture', 0x28: 'Start', 0x30: 'FrameMoveFuture', 0x38: 'FrameMove', 0x40: 'CameraMove',
      0x48: 'EventProcess', 0x50: 'EndFuture', 0x58: 'End'}
M32 = 0xffffffff
exe = open(D.EXE, 'rb').read()
START, FEND = 0x32b465ad, 0x32b4a831
EXITS = (0x32b4a801, 0x32b4a818)
LISTING = os.path.join(HERE, 'ghidra', 'disasm_32b46580_SetEquippedSkillFunc_capstone.txt')
MAKER_HEAD = bytes.fromhex('8b4424048b4c240889' + '08c74004')   # mov eax,[esp+4]; mov ecx,[esp+8]; mov [eax],ecx; mov [eax+4], imm32; ret


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
    ins, order = {}, []
    for l in open(path).read().splitlines():
        m = re.match(r'([0-9a-f]{8}): (\S+) ?(.*)', l)
        if m:
            a = int(m.group(1), 16)
            if a > FEND:
                break
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


class Fork(Exception):
    pass


class Run:
    def __init__(self, ins, order, sid, typ, V, slot, tm):
        self.ins, self.order = ins, order
        self.next = {a: order[i + 1] for i, a in enumerate(order[:-1])}
        self.regs = {'eax': None, 'ebx': None, 'ecx': None, 'edx': None, 'esi': 'THIS', 'edi': None, 'ebp': None}
        self.mem = {0x20c: sid, 0x210: slot[0], 0x214: slot[1]}
        self.obj = collections.defaultdict(dict)
        self.depth = 0
        self.pushes = []
        self.flags = None
        self.events = []
        self.sid, self.typ, self.V, self.tm = sid, typ, V, tm
        self.mtr = 0
        self.u88f0 = 0
        self.force = {}

    def memref(self, op):
        m = re.match(r'(?:(?:byte|word|dword) ptr )?\[(.*)\]$', op.strip())
        if not m:
            return None
        terms = re.split(r' ([+-]) ', m.group(1))
        sign, base, val, unknown = '+', None, 0, False
        for t in terms:
            if t in ('+', '-'):
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
            elif t == 'esp':
                base = 'esp'
            elif re.fullmatch(r'e[a-z]{2}', t):
                r = self.regs.get(t)
                if isinstance(r, int):
                    val += (1 if sign == '+' else -1) * r
                elif isinstance(r, (str, tuple)):
                    base = r
                else:
                    unknown = True
            else:
                v = imm(t)
                if v is None:
                    unknown = True
                else:
                    val += (1 if sign == '+' else -1) * v
        return None if unknown else (base, val)

    def getv(self, op):
        op = op.strip()
        if op == 'bl':
            v = self.regs.get('ebx')
            return v & 0xff if isinstance(v, int) else None
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
                return exe[o] if op.startswith('byte') else struct.unpack_from('<I', exe, o)[0]
            return None
        if base == 'esp':
            return self.mem.get(off - 4 * self.depth)
        if base == 'OBJ':
            return self.typ if off == 4 else None
        if isinstance(base, tuple):
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
        if not isinstance(a, int) or not isinstance(b, int):
            raise Stop('flag operands unknown at %s' % mn)
        if kind == 'test':
            r = a & b
            zf, sf, cf, of = r == 0, bool(r & 0x80000000), False, False
        else:
            r = (a - b) & M32
            zf, sf, cf = r == 0, bool(r & 0x80000000), a < b
            of = (sg(a) - sg(b)) != sg(r)
        t = {'je': zf, 'jz': zf, 'jne': not zf, 'jnz': not zf, 'ja': not cf and not zf, 'jae': not cf, 'jb': cf, 'jbe': cf or zf,
             'jg': not zf and sf == of, 'jge': sf == of, 'jl': sf != of, 'jle': zf or sf != of, 'js': sf, 'jns': not sf}
        if mn not in t:
            raise Stop('cond ' + mn)
        return t[mn]

    def go(self, start, limit=4000):
        a, steps = start, 0
        while steps < limit:
            steps += 1
            if a in EXITS:
                return 'exit'
            if a not in self.ins:
                raise Stop('off listing %08x' % a)
            mn, op = self.ins[a]
            nxt = self.next.get(a)
            if mn == 'jmp':
                if op.startswith('dword ptr'):
                    return self.stop_jump(a, op)
                a = imm(op)
                continue
            if mn.startswith('j'):
                if a in self.force:
                    c = self.force[a]
                else:
                    try:
                        c = self.cond(mn)
                    except Stop:
                        raise Fork(a)
                a = imm(op) if c else nxt
                continue
            if mn == 'push':
                self.pushes.append(self.getv(op))
                self.depth += 1
            elif mn == 'pop':
                self.depth -= 1
                self.setv(op, None)
            elif mn == 'call':
                self.do_call(a, op)
            elif mn in ('mov', 'movzx'):
                d, s = [x.strip() for x in op.split(',', 1)]
                v = self.getv(s)
                if d == 'bl':
                    self.regs['ebx'] = (v & 0xff) if isinstance(v, int) else None
                elif re.fullmatch(r'[a-d][lh]|[a-d]x', d):
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
                    self.depth = max(0, self.depth - ((imm(s) or 0) // 4 if mn == 'add' else 0))
                else:
                    x, y = self.getv(d), self.getv(s)
                    if isinstance(x, int) and isinstance(y, int):
                        r = (x + y if mn == 'add' else x - y) & M32
                        self.setv(d, r)
                        self.flags = ('cmp', r, 0)
                    else:
                        self.setv(d, None)
                        self.flags = None
            elif mn in ('inc', 'dec'):
                x = self.getv(op)
                if isinstance(x, int):
                    r = (x + (1 if mn == 'inc' else -1)) & M32
                    self.setv(op, r)
                    self.flags = ('cmp', r, 0)
                else:
                    self.setv(op, None)
                    self.flags = None
            elif mn == 'cmp':
                d, s = [x.strip() for x in op.split(',', 1)]
                self.flags = ('cmp', self.getv(d), self.getv(s))
            elif mn == 'test':
                d, s = [x.strip() for x in op.split(',', 1)]
                self.flags = ('test', self.getv(d), self.getv(s))
            elif mn == 'ret':
                return 'ret'
            elif mn == 'nop':
                pass
            elif ',' in op:
                self.setv(op.split(',', 1)[0], None)
            a = nxt
        raise Stop('step limit')

    def do_call(self, a, op):
        t = imm(op)
        ret = None
        if t == 0x32fe9a40:
            nm = self.pushes[-1] if self.pushes else None
            self.mtr += 1
            self.events.append(('mtr', a, cstr(nm) if isinstance(nm, int) else None, self.mtr))
        elif t == 0x31fd89b0:
            p = self.pushes[-3:]
            base = p[0][1] if len(p) == 3 and isinstance(p[0], tuple) else None
            cbs = {}
            if base is not None:
                for k, v in self.mem.items():
                    if (k - base) in SL and isinstance(v, int) and 0x31aa1000 <= v < 0x337ba000:
                        cbs[k - base] = v
            nm = p[1] if len(p) == 3 else None
            self.events.append(('reg', a, cstr(nm) if isinstance(nm, int) else None, p[2] if len(p) == 3 else None, cbs))
        elif t == 0x320775a0:
            c = self.regs.get('ecx')
            if isinstance(c, tuple):
                for k in [k for k in self.mem if c[1] <= k < c[1] + 0x70]:
                    del self.mem[k]
        elif t is not None and exe[t - B:t - B + 13] == MAKER_HEAD and exe[t - B + 17] == 0xc3:
            # delegate maker ( dest, this ): *dest = this; dest[1] = fn; returns dest
            fn = struct.unpack_from('<I', exe, t - B + 13)[0]
            tok = 'PAIR%08x' % fn
            self.obj[tok][0] = 'THIS'
            self.obj[tok][4] = fn
            ret = tok
        elif t == 0x329cc2c0:
            # hyper data = normal data ( every callback )
            src, dst = (self.pushes[-1] if self.pushes else None), self.regs.get('ecx')
            if src == 'NORM' and dst == 'HYP':
                self.obj['HYP'].update(self.obj['NORM'])
        elif t == 0x31fd88f0:
            self.u88f0 += 1
            ret = 'NORM' if self.u88f0 == 1 else 'HYP'
        elif t == 0x31fca8e0:
            ret = 'OBJ'
        elif t == 0x31fdbcc0:
            ret = self.V
        elif t == 0x32206450:
            ret = 1
        elif t == 0x32207210:
            x = self.pushes[-1] if self.pushes else None
            ret = (self.tm.get(x, (0, 0, ''))[1] or x) if isinstance(x, int) else None
        self.depth = 0
        self.pushes = []
        self.regs['eax'] = ret
        self.regs['ecx'] = None
        self.regs['edx'] = None

    def stop_jump(self, a, op):
        m = re.match(r'dword ptr \[(e[a-z]{2})\*4 \+ (0x[0-9a-f]+)\]', op)
        if not m:
            raise Stop('jmp form')
        r = self.regs.get(m.group(1))
        if not isinstance(r, int):
            raise Stop('jmp table index unknown')
        t = int(m.group(2), 16) - B + 4 * r
        return ('goto', struct.unpack_from('<I', exe, t)[0])


def run_id(ins, order, sid, typ, V, slot, tm, force=None):
    """Returns a list of (Run, result): a branch on a call result the interpreter cannot know (32070d40 al == 1) runs both ways."""
    force = dict(force or {})
    r = Run(ins, order, sid, typ, V, slot, tm)
    r.force = force
    a = START
    for _ in range(50):
        try:
            res = r.go(a)
        except Stop as e:
            return [(r, 'stop: %s' % e)]
        except Fork as f:
            ad = f.args[0]
            return run_id(ins, order, sid, typ, V, slot, tm, {**force, ad: True}) + run_id(ins, order, sid, typ, V, slot, tm, {**force, ad: False})
        if isinstance(res, tuple):
            a = res[1]
            continue
        return [(r, res)]
    return [(r, 'loop')]


def cbstr(d):
    return ' '.join('%s=%s' % (SL[o], thunk_res(v)) for o, v in sorted(d.items()) if o in SL and isinstance(v, int) and v)


def listing_consts(ins, order):
    """every immediate (100..40000) compared in the listing: the case values of the dispatch"""
    out = set()
    for a in order:
        mn, op = ins[a]
        m = re.match(r'(?:eax|ecx|edx|dword ptr \[[^\]]*\]), (0x[0-9a-f]+|\d+)$', op)
        if mn == 'cmp' and m:
            v = int(m.group(1), 0)
            if 100 <= v <= 40000:
                out.add(v)
    return out


def main():
    ins, order = load(LISTING)
    tm = D.templets()
    consts = listing_consts(ins, order)
    ids = sorted(set(D.lu_skill_ids(tm)) | {i for i in tm if i < 1000} | consts | {900} | set(range(1, 1000)) | set(range(10000, 10700)) | set(range(30000, 30700)))
    rec = collections.OrderedDict()
    outcome = {}
    for sid in ids:
        typs = [tm[sid][0]] if sid in tm and tm[sid][0] else [5, 6, 7, 12, 13, 14, 15, 16]
        for vname, V in (('0', 0), ('id', sid)):
            for typ in typs:
                for r, res in run_id(ins, order, sid, typ, V, (0, 0), tm):
                    outcome.setdefault((sid, vname), []).append((typ, res, [e[1] for e in r.events]))
                    for e in r.events:
                        if e[0] == 'mtr':
                            role = 'normal' if e[3] == 1 else 'hyper'
                            cb = dict(r.obj['NORM'] if e[3] == 1 else r.obj['HYP'])
                            key = (e[1], e[2], role)
                            ent = rec.setdefault(key, {'id': None, 'ids': set(), 'cb': cb, 'how': set(), 'typ': set()})
                        else:
                            key = (e[1], e[2], 'reg')
                            ent = rec.setdefault(key, {'id': e[3], 'ids': set(), 'cb': e[4], 'how': set(), 'typ': set()})
                        ent['ids'].add(sid)
                        ent['how'].add(vname)
                        ent['typ'].add(typ)
    diff = 0
    for sid in [i for i in ids if i in tm]:
        typs = [tm[sid][0]] if tm[sid][0] else [5, 6, 7, 12, 13, 14, 15, 16]
        for typ in typs:
            a = sorted([(e[1], e[2]) for r, _ in run_id(ins, order, sid, typ, 0, (0, 0), tm) for e in r.events])
            b = sorted([(e[1], e[2]) for r, _ in run_id(ins, order, sid, typ, 0, (4, 1), tm) for e in r.events])
            if a != b:
                diff += 1
    with open(os.path.join(HERE, 'ch0_skill_listing_rows.tsv'), 'w', encoding='utf-8', newline='\n') as o:
        o.write('fn\tcall_va\tstate_id\ttable\tskill_ids\trole\tcallbacks\tlisting_note\n')
        for (a, nm, role), e in rec.items():
            sidv = e['id'] if isinstance(e['id'], int) else '-'
            o.write('32b46580\t%08x\t%s\t%s\t%s\t%s\t%s\tV=%s types=%s\n' % (a, sidv, nm, ','.join(map(str, sorted(e['ids']))), role, cbstr(e['cb']),
                                                                         '/'.join(sorted(e['how'])), ','.join(map(str, sorted(e['typ'])))))
    noev = [sid for sid in ids if not any(ev for v in ('0', 'id') for (_, _, ev) in outcome.get((sid, v), []))]
    print('ids', len(ids), 'rows', len(rec), 'ids with no event', len(noev), 'slot/bool event differences', diff)
    print('no-event outcomes', collections.Counter(res for sid in noev for v in ('0', 'id') for (_, res, _) in outcome.get((sid, v), [])).most_common(8))
    print('stops', collections.Counter(res for sid in ids for v in ('0', 'id') for (_, res, ev) in outcome.get((sid, v), []) if res.startswith('stop')).most_common(10))
    pickle.dump((ids, outcome, noev), open(os.path.join(HERE, '_emu_outcome.pkl'), 'wb'))


if __name__ == '__main__':
    main()
