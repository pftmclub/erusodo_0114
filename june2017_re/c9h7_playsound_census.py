"""Census of June's device-manager PlaySound call sites (june-effect C9h-7).

    python c9h7_playsound_census.py > c9h7_playsound_census.tsv

For every `call 0x31ce0e10` in .text: the containing function (int3-padding bounds), the
4th pushed argument (the auto-release flag: 1 / 0 / a register, resolved when an earlier
`mov reg, imm` / `xor reg, reg` in the function sets it), a UTF-16 literal pushed or loaded
in the 0xa0 bytes before the call (the sound name, when it is a literal), and what the
code does with EAX right after the call (store to a member, keep in a local, pass on, test,
or drop). Linear sweep from the function start: a jump table can desync a few sites, so
read anything surprising with funcdis.py.
"""
import os, re, sys, struct, capstone
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from disasm_range import EXE, BASE, TR, TV, load_text

TARGET = 0x31ce0e10
T = load_text()
D = open(EXE, 'rb').read()
RBASE = 0x31aa0000 + 0xc00          # raw = VA - RBASE for .text and the read-only data after it
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

def va_of(off): return off + BASE + TV
def off_of(va): return va - BASE - TV

def fstart(o):
    while o > 1:
        if T[o-1] == 0xcc and T[o-2] == 0xcc: return o
        o -= 1
    return 0

def wstr(va):
    r = va - RBASE
    if r < 0 or r + 4 > len(D): return None
    s = D[r:r+260]
    out = []
    for i in range(0, len(s) - 1, 2):
        c = s[i] | (s[i+1] << 8)
        if c == 0: break
        if c < 0x20 or c > 0x7e: return None
        out.append(chr(c))
    return ''.join(out) if len(out) >= 4 else None

# settled by hand (C9h-7): the sweep cannot resolve these, read with disasm_range.py
OVERRIDES = {
    0x322d6b13: '1 (eax: cmp eax, 1 / jne guards the push)',
    0x32d94419: '1 (fork: push 1 at 32d943c2 precedes the jcc)',
    0x321a921b: '1 (function bounds desync; push 1 at 321a9210)',
    0x3251094c: 'ebp (unresolved; handle dropped)',
    0x325130c5: 'ebp (unresolved; handle dropped)',
    0x325131b2: 'ebp (unresolved; handle dropped)',
    0x32e2b608: 'ebx (unresolved; handle dropped)',
}

sites = []
i = T.find(b'\xe8')
while i >= 0:
    if i + 5 <= len(T):
        rel = struct.unpack_from('<i', T, i + 1)[0]
        if va_of(i) + 5 + rel == TARGET:
            sites.append(i)
    i = T.find(b'\xe8', i + 1)

print('site\tfunc\targ4\tname\thandle\tafter')
for s in sites:
    f = fstart(s)
    ins = list(md.disasm(T[f:s + 0x40], va_of(f)))
    idx = next((k for k, x in enumerate(ins) if x.address == va_of(s)), None)
    if idx is None:
        print(f'{va_of(s):08x}\t{va_of(f):08x}\t{OVERRIDES.get(va_of(s), "?desync")}\t\tdropped\t'); continue
    # a fork just before the call (`jcc L; <path A pushes>; jmp call; L: <path B pushes>; call`):
    # path A's pushes duplicate path B's, so drop the instructions between the jcc and the jmp
    skip = set()
    for k, x in enumerate(ins[:idx]):
        if x.mnemonic == 'jmp' and re.fullmatch(r'0x[0-9a-f]+', x.op_str) and x.address < int(x.op_str, 16) <= va_of(s):
            nxt = x.address + x.size
            fork = next((y for y in reversed(ins[:k]) if y.mnemonic.startswith('j') and y.mnemonic != 'jmp'
                         and re.fullmatch(r'0x[0-9a-f]+', y.op_str) and int(y.op_str, 16) == nxt), None)
            if fork:
                skip.update(y.address for y in ins[:k + 1] if fork.address < y.address <= x.address)
    pushes = [x for x in ins[:idx] if x.mnemonic == 'push' and x.address not in skip]
    a4 = '?'
    if len(pushes) >= 4:
        p = pushes[-4]
        a4 = p.op_str
        if re.fullmatch(r'e[a-d]x|e[sd]i|ebx|ebp', a4):
            reg = a4
            for x in reversed(ins[:ins.index(p)]):
                if x.mnemonic == 'mov' and x.op_str.startswith(reg + ', ') and re.fullmatch(r'0x[0-9a-f]+|\d+', x.op_str.split(', ')[1]):
                    a4 = f'{reg}={int(x.op_str.split(", ")[1], 0)}'; break
                if x.mnemonic == 'xor' and x.op_str == f'{reg}, {reg}':
                    a4 = f'{reg}=0'; break
                if x.op_str.startswith(reg + ',') or x.op_str.startswith(reg[1:] + ',') or (x.mnemonic in ('pop', 'call') and reg == 'eax'):
                    break
    name = ''
    for x in ins[:idx]:
        if x.address < va_of(s) - 0xa0: continue
        for m in re.findall(r'0x3[0-9a-f]{7}', x.op_str):
            w = wstr(int(m, 16))
            if w and '.' in w: name = w
    # a branch landing between the 4th push and the call: another path pushes its own args
    if len(pushes) >= 4:
        lo, hi = pushes[-4].address, va_of(s)
        for x in ins:
            if x.address in skip: continue
            if x.mnemonic.startswith('j') and re.fullmatch(r'0x[0-9a-f]+', x.op_str):
                t = int(x.op_str, 16)
                if lo < t <= hi and not (lo <= x.address < hi):
                    a4 += ' (merge)'; break
    # what EAX (the handle) does next: first use before it is overwritten
    use, regs = 'dropped', {'eax'}
    for x in ins[idx+1:idx+12]:
        ops = x.op_str
        dst = ops.split(', ')[0] if ', ' in ops else ops
        src = ops.split(', ', 1)[1] if ', ' in ops else ''
        r = next((g for g in regs if re.search(r'\b' + g + r'\b', ops)), None)
        if x.mnemonic == 'mov' and src in regs and dst.startswith('dword ptr'):
            use = ('local ' if re.search(r'\b(esp|ebp)\b', dst) else 'STORE ') + dst; break
        if x.mnemonic == 'push' and ops in regs:
            use = 'pass'; break
        if x.mnemonic in ('test', 'cmp') and r and not dst.startswith(('dword', 'byte')):
            use = 'test'; break
        if x.mnemonic == 'mov' and src in regs and re.fullmatch(r'e[a-d]x|e[sd]i|ebx|ebp', dst):
            if dst == 'ecx':
                use = 'this(method)'; break
            regs.add(dst); continue
        if x.mnemonic in ('call', 'ret', 'jmp') or (x.mnemonic in ('mov', 'xor', 'pop', 'lea', 'movzx', 'movsx') and dst in regs):
            if dst in regs and x.mnemonic not in ('call', 'ret', 'jmp'):
                regs.discard(dst)
                if regs: continue
            break
    a4 = OVERRIDES.get(va_of(s), a4)
    after = ' ; '.join(f'{x.mnemonic} {x.op_str}' for x in ins[idx+1:idx+5])
    print(f'{va_of(s):08x}\t{va_of(f):08x}\t{a4}\t{name}\t{use}\t{after}')
