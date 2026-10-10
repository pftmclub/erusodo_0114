"""Map June NPC StateData flag bits to their Lua keys (June effect C3j-2 review).

June's LoadStateData (32784c00) builds every key string on the stack from XOR-masked globals,
then calls a getter (32fe9e40 bool, 32fea370 / 32fea820 float, 32fea4e0 int) and the bit setter
32386970( bit, value ). This replays the masking linearly over a funcdis.py listing and prints,
for each setter call, the bit and the key of the getter call before it.

    python june2017_re/funcdis.py --func 32784c00 > c00.txt
    python june2017_re/statedata_keys.py c00.txt [bit ...]     # bits as hex, e.g. 0x1f 4 0x2e

A bit set without a getter of its own (e.g. a setter after a non-key branch) reports the previous
key: read the listing around the address before relying on that row. A stray trailing letter
(0x2e prints NEVER_MOVEM) is a terminator stored from a register the replay does not track.
"""
import re, sys

GETTERS = ('0x32fe9e40', '0x32fea370', '0x32fea820', '0x32fea4e0')
SETTER = '0x32386970'
IMM = re.compile(r'^(0x[0-9a-f]+|\d+)$')


def decode(lines):
    mem, reg, stack, lea, pushed = {}, {}, {}, {}, []
    delta, key, last_push, out = 0, None, None, []
    for ln in lines:
        m = re.match(r'([0-9a-f]+): (\w+) ?(.*)', ln.strip())
        if not m:
            continue
        va, op, ops = m.groups()
        if op == 'push':
            pushed.append(lea.get(ops))
            last_push = ops
            delta -= 4
        elif op == 'pop':
            delta += 4
        elif op in ('add', 'sub') and ops.startswith('esp, '):
            delta += (1 if op == 'add' else -1) * int(ops.split(', ')[1], 0)
        ml = re.match(r'(e[a-d]x|esi|edi|ebp), \[esp \+ (0x[0-9a-f]+)\]', ops)
        if op == 'lea' and ml:
            lea[ml.group(1)] = int(ml.group(2), 16) + delta
        mm = re.match(r'(byte|word|dword) ptr \[(0x[0-9a-f]+)\], (.+)', ops)
        if op == 'mov' and mm:
            size = {'byte': 1, 'word': 2, 'dword': 4}[mm.group(1)]
            addr, src = int(mm.group(2), 16), mm.group(3)
            val = int(src, 0) if IMM.match(src) else reg.get(src)
            if val is not None:
                for i in range(size if IMM.match(src) else 1):
                    mem[addr + i] = (val >> (8 * i)) & 0xff
            continue
        mm = re.match(r'(al|bl|cl|dl), (.+)', ops)
        if mm and op in ('mov', 'xor'):
            r, src = mm.groups()
            mg = re.match(r'byte ptr \[(0x[0-9a-f]+)\]', src)
            if op == 'mov' and mg:
                reg[r] = mem.get(int(mg.group(1), 16), 0)
            elif op == 'mov' and IMM.match(src):
                reg[r] = int(src, 0) & 0xff
            elif op == 'xor' and IMM.match(src):
                reg[r] = reg.get(r, 0) ^ (int(src, 0) & 0xff)
            continue
        mm = re.match(r'byte ptr \[esp \+ (0x[0-9a-f]+)\], (.+)', ops)
        if op == 'mov' and mm:
            src = mm.group(2)
            val = reg.get(src) if src in reg else (int(src, 0) & 0xff if IMM.match(src) else None)
            if val is not None:
                stack[int(mm.group(1), 16) + delta] = val
            continue
        if op == 'call' and ops in GETTERS:
            if pushed and pushed[-1] is not None:
                a, s = pushed[-1], ''
                while a in stack and stack[a] != 0:
                    s += chr(stack[a]) if 32 <= stack[a] < 127 else '?'
                    a += 1
                key = s
            pushed = []
        if op == 'call' and ops == SETTER:
            out.append((va, last_push, key))
    return out


if __name__ == '__main__':
    rows = decode(open(sys.argv[1]).read().splitlines())
    want = {int(b, 0) for b in sys.argv[2:]}
    for va, bit, key in rows:
        if not want or (bit and IMM.match(bit) and int(bit, 0) in want):
            print(va, bit, key)
