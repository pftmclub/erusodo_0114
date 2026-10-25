"""Annotated disassembly of June class functions (character port, step 0 of a transcription phase).

    python charport_adis.py eve 32981680 3297acf0            # these functions
    python charport_adis.py eve --phase CH1a --out DIR       # every class-map row of phase CH1a -> DIR/<va>.txt

Why: Ghidra's decompile drops the arguments of every virtual call it cannot type (StateChange( id, 0 ) prints as `()`), prints the
x87 compares as `NAN(f) || 0.0 < f == (f == 0.0)`, and has no function at all for thunks and for a function that follows a `ret` without
int3 padding. This prints the raw listing with the things a transcription needs next to the instruction:
  * `push 0xNN` before a call: the state id name (`char_<key>/ch0_state_skill.tsv`, column "id N");
  * `call 0x...`: the funcmap name; a call into the class range is marked `(class fn)`;
  * `[reg + 0xNNN]`: the member name from `char_<key>/ch1_offsets.tsv` (kind field; InputData bytes from 0x86da are generated);
  * `call/jmp [reg + 0xNNN]`: the virtual slot name from the same table (kind vslot);
  * `[0x33xxxxxx]` after fld/fcomp/...: the float or double value (the width is the operand width, see charport_consts.py);
  * `push 0x33xxxxxx`: the wide string at that address.
Read the compare idioms like this: after `fcomp m` (ST0 = other operand), `test ah, 1` + `jne X` jumps when ST0 < m; `test ah, 5` + `jp X`
jumps when NOT (ST0 < m); `test ah, 0x41` + `jp X` jumps when ST0 > m.
"""
import os, re, struct, sys
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from charport_consts import read_va, func_bounds, md, T

CLASS_RANGE = {'eve': (0x32965dd0, 0x329cf86f)}


def load(key):
    d = os.path.join(HERE, 'char_' + key)
    ids = {}
    p = os.path.join(d, 'ch0_state_skill.tsv')
    if os.path.exists(p):
        for l in open(p, encoding='latin1'):
            c = l.split('\t')
            if len(c) > 2:
                m = re.search(r'\bid (\d+)', c[2])
                if m:
                    ids.setdefault(int(m.group(1)), c[0])
    fields, vslots = {}, {}
    p = os.path.join(d, 'ch1_offsets.tsv')
    if os.path.exists(p):
        for l in open(p, encoding='latin1'):
            c = l.rstrip('\n').split('\t')
            if len(c) >= 3 and c[0].startswith('0x'):
                (fields if c[1] == 'field' else vslots)[int(c[0], 16)] = c[2]
    keys = ['Left', 'Right', 'Up', 'Down', 'Z', 'X', 'C', 'A', 'S', 'D', 'F', 'Q', 'W', 'E', 'R', 'LShift', 'LAlt', 'Space', 'LCtr']
    for i, nm in enumerate(keys):
        for j, pf in enumerate(['pure', 'one', 'pureDouble', 'oneDouble']):
            fields.setdefault(0x86da + i * 4 + j, 'in.' + pf + nm)
    fm = {}
    for l in open(os.path.join(HERE, 'funcmap.tsv'), encoding='latin1'):
        c = l.rstrip('\n').split('\t')
        if len(c) > 1:
            m = re.match(r'^([0-9a-f]{8})\b', c[0])
            if m and m.group(1) not in fm:
                fm[m.group(1)] = c[1][:70]
    return ids, fields, vslots, fm


def wstr(a):
    b = read_va(a, 24)
    if not b:
        return ''
    try:
        t = b.decode('utf-16le').split(chr(0))[0]
    except Exception:
        return ''
    return t if t and all(32 <= ord(c) < 127 for c in t) else ''


def cval(a, w):
    b = read_va(a, w)
    if b is None:
        return '?'
    if w == 8:
        return 'd=%r' % struct.unpack('<d', b)[0]
    return 'f=%r i=%d' % (struct.unpack('<f', b)[0], struct.unpack('<i', b)[0])


def run(va, key, tables):
    ids, fields, vslots, fm = tables
    lo, hi = CLASS_RANGE.get(key, (0, 0))
    o, e = func_bounds(va)
    out, lastpush = [], []

    def sym(op):
        def r(m):
            v = int(m.group(2), 16)
            return m.group(1) + m.group(2) + (('{' + fields[v] + '}') if v in fields else '')
        return re.sub(r'(\[e..\s*\+\s*)(0x[0-9a-f]+)', r, op)

    for i in md.disasm(T[o:e], va):
        note = ''
        if i.mnemonic == 'call':
            m = re.match(r'^0x([0-9a-f]+)$', i.op_str)
            if m:
                note = fm.get(m.group(1), '') or ('(class fn)' if lo <= int(m.group(1), 16) <= hi else '')
            if lastpush:
                note += ' pushes=' + ','.join(lastpush[-3:])
        m = re.search(r'(dword|qword) ptr \[(0x3[0-9a-f]{7})\]', i.op_str)
        if m:
            note += ' ' + cval(int(m.group(2), 16), 8 if m.group(1) == 'qword' else 4)
        m = re.search(r'\[e..\s*\+\s*(0x[0-9a-f]+)\]', i.op_str)
        if m and (i.mnemonic in ('call', 'jmp') or i.mnemonic == 'mov') and int(m.group(1), 16) in vslots and int(m.group(1), 16) not in fields:
            note += ' vslot ' + m.group(1) + ' ' + vslots[int(m.group(1), 16)]
        ws = ''
        if i.mnemonic == 'push':
            m = re.match(r'^(0x[0-9a-f]+|\d+)$', i.op_str)
            if m:
                v = int(m.group(1), 0)
                lastpush.append('%s(%s)' % (m.group(1), ids.get(v, '')))
                if 0x33000000 <= v < 0x34000000:
                    t = wstr(v)
                    if t:
                        ws = ' L"%s"' % t
        out.append('%08x: %-6s %-48s %s' % (i.address, i.mnemonic, sym(i.op_str), (note.strip() + ws)))
        if i.mnemonic == 'call':
            lastpush = []
    return out


def main(argv):
    if len(argv) < 2:
        print(__doc__)
        return
    key = argv[0]
    tables = load(key)
    outdir = None
    vas = []
    i = 1
    while i < len(argv):
        if argv[i] == '--phase':
            ph = argv[i + 1]
            i += 2
            for l in open(os.path.join(HERE, 'char_' + key, 'ch0_class_map.tsv'), encoding='latin1'):
                c = l.rstrip('\n').split('\t')
                if len(c) > 5 and c[5].strip() == ph:
                    vas.append(c[0])
        elif argv[i] == '--out':
            outdir = argv[i + 1]
            i += 2
        else:
            vas.append(argv[i])
            i += 1
    for va in vas:
        text = '#### %s\n%s\n' % (va, '\n'.join(run(int(va, 16), key, tables)))
        if outdir:
            os.makedirs(outdir, exist_ok=True)
            open(os.path.join(outdir, va + '.txt'), 'w', encoding='utf-8').write(text)
        else:
            sys.stdout.write(text)


if __name__ == '__main__':
    main(sys.argv[1:])
