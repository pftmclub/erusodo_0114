"""pilot-char-revamp CH0c: check charport_states.py's registration rows against the raw listing (not Ghidra's decompile).

    python june2017_re/funcdis.py --func 32ce77b0 > <S>/dis_32ce77b0.txt        (and 32cea0c0, 3202b110)
    python june2017_re/charport_states.py init 32ce77b0 32cea0c0 3202b110 > <S>/reg.tsv
    python june2017_re/char_arme/ch0c_listing_check.py <S> 32ce77b0 32cea0c0 3202b110

Two readers, both tracking esp depth (push +1; a call resets it: thiscall / stdcall callees pop their arguments; `add esp, N`),
`mov reg, imm` / `xor reg, reg` / `lea reg, [esp + X]`, and dword stores into the stack:
  names  for every push of a table-name literal (an exe string equal to the row's name): back to the block start (the previous
         Init 320775a0 call, the previous 31fd89b0 call, or a jmp / ret), then forward through unconditional jmps (conditional
         ones fall through) to the 31fd89b0 call. Slots are relative to the registrar's local (the `lea ecx, [esp + X]` before
         its Init calls). This reads a per-class '~' row along the path its branch really takes.
  calls  for every 31fd89b0 call: back to the block start, forward to the call; base = the lea behind the first of the three
         pushes. Covers rows whose name is not a pushed literal (a std::string) and registrars with no Init call.
A reg.tsv row passes when one reader gives its id and callbacks. Prints the rows no reader reproduces.
"""
import re, sys, os, collections

EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
B = 0x31aa0c00
SL = {0x18: 'Init', 0x20: 'StartFuture', 0x28: 'Start', 0x30: 'FrameMoveFuture', 0x38: 'FrameMove', 0x40: 'CameraMove',
      0x48: 'EventProcess', 0x50: 'EndFuture', 0x58: 'End'}
exe = open(EXE, 'rb').read()


def cstr(va):
    o = va - B
    e = exe.find(b'\0', o)
    return exe[o:e].decode('latin-1') if 0 <= o < len(exe) and 0 < e - o < 128 else None


def imm(s):
    s = s.strip()
    return int(s, 16) if s.startswith('0x') else (int(s) if s.isdigit() else None)


def load(path):
    ins = []
    for l in open(path).read().splitlines():
        m = re.match(r'([0-9a-f]{8}): (\S+) ?(.*)', l)
        if m:
            ins.append((int(m.group(1), 16), m.group(2), m.group(3)))
    return ins


def block_start(ins, k):
    while k > 0:
        a, mn, op = ins[k - 1]
        if (mn == 'call' and op in ('0x320775a0', '0x31fd89b0')) or mn in ('jmp', 'ret'):
            break
        k -= 1
    return k


def simulate(ins, at, k, stop):
    """Walk from k to the first 31fd89b0 call (following jmp); stop(i) may end early. Returns (call index, pushes, frame stores)."""
    depth, regs, fld, pushes, steps = 0, {}, {}, [], 0
    while k < len(ins) and steps < 600:
        steps += 1
        a, mn, op = ins[k]
        if mn == 'call' and op == '0x31fd89b0':
            return k, pushes, fld
        if mn == 'jmp':
            t = imm(op)
            if t not in at:
                return None
            k = at[t]
            continue
        if mn == 'push':
            depth += 1
            v = imm(op)
            pushes.append(v if v is not None else regs.get(op, op))
        elif mn == 'pop':
            depth -= 1
        elif mn == 'call':
            regs.pop('eax', None)
            depth, pushes = 0, []
        elif mn == 'add' and op.startswith('esp, '):
            depth = max(0, depth - imm(op.split(',')[1]) // 4)
        elif mn in ('mov', 'lea', 'xor'):
            d, s = [x.strip() for x in op.split(',', 1)]
            m = re.match(r'dword ptr \[esp \+ (0x[0-9a-f]+|\d+)\]$', d)
            if mn == 'mov' and m:
                fld[int(m.group(1), 0) - 4 * depth] = imm(s) if imm(s) is not None else regs.get(s)
            elif mn == 'mov' and re.match(r'^e[a-z]{2}$', d):
                regs[d] = imm(s) if imm(s) is not None else regs.get(s)
            elif mn == 'xor' and d == s:
                regs[d] = 0
            elif mn == 'lea':
                m2 = re.match(r'\[esp \+ (0x[0-9a-f]+|\d+)\]$', s)
                regs[d] = ('lea', int(m2.group(1), 0) - 4 * depth) if m2 else None
        k += 1
    return None


def cbstr(fld, base):
    return ' '.join('%s=%08x' % (SL[o - base], v) for o, v in sorted(fld.items()) if (o - base) in SL and isinstance(v, int) and v)


S = sys.argv[1]
want = collections.defaultdict(set)
for l in open(os.path.join(S, 'reg.tsv')).read().splitlines()[1:]:
    f = (l.split('\t') + [''])[:5]
    want[(f[0], f[3])].add((f[2], re.sub(r'>[0-9a-f]{8}', '', f[4])))
got = set()
for fn in sys.argv[2:]:
    ins = load(os.path.join(S, 'dis_%s.txt' % fn))
    at = {a: k for k, (a, _, _) in enumerate(ins)}
    names = {n for (f, n) in want if f == fn}
    for k, (a, mn, op) in enumerate(ins):          # names
        m = re.search(r'0x(33[0-9a-f]{6})', op)
        if not m or cstr(int(m.group(1), 16)) not in names:
            continue
        r = simulate(ins, at, block_start(ins, k), None)
        if r and len(r[1]) >= 3 and isinstance(r[1][-3], tuple):
            got.add((fn, cstr(int(m.group(1), 16)), str(r[1][-1]), cbstr(r[2], r[1][-3][1])))
    for c, (a, mn, op) in enumerate(ins):          # calls
        if mn == 'call' and op == '0x31fd89b0':
            r = simulate(ins, at, block_start(ins, c), None)
            if r and len(r[1]) >= 3 and isinstance(r[1][-3], tuple):
                got.add((fn, '*', str(r[1][-1]), cbstr(r[2], r[1][-3][1])))
ok, bad = 0, []
for (fn, name), vs in want.items():
    for sid, cb in vs:
        if (fn, name, sid, cb) in got or (fn, '*', sid, cb) in got:
            ok += 1
        else:
            bad.append((fn, name, sid, cb))
print('reg.tsv rows reproduced by the listing: %d / %d' % (ok, ok + len(bad)))
for b in bad:
    print('NOT REPRODUCED', *b)
