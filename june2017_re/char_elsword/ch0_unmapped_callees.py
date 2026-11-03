r"""char-elsword CH0 item 6: callees outside Elsword's class range that funcmap does not map, per phase of the calling function.

    python june2017_re/char_elsword/ch0_unmapped_callees.py > june2017_re/char_elsword/ch0_unmapped_callees.tsv

Callers: every row of ch0_class_map.tsv except thunk / dead / not-Elsword (phase n/a). Each function is disassembled in process (capstone) from
its start to the next row's start; callees = direct `call` targets outside 0x32cf2da0-0x32d2763f, resolved through a 5-byte jmp thunk.
'mapped' = the address appears in funcmap.tsv's first column. 'cited' = the address appears anywhere else in funcmap.tsv or the plan docs.
"""
import os, re, struct, collections, capstone
HERE = os.path.dirname(os.path.abspath(__file__))
RE = os.path.normpath(os.path.join(HERE, '..'))
LO, HI = 0x32cf2da0, 0x32d27640
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
B = 0x31aa0c00
d = open(EXE, 'rb').read()
mapped = set()
fmtext = open(os.path.join(RE, 'funcmap.tsv'), encoding='utf-8', errors='replace').read()
for l in fmtext.split('\n'):
    mapped.update(re.findall(r'\b([0-9a-f]{8})\b', l.split('\t')[0]))
rows = [l.rstrip('\n').split('\t') for l in open(os.path.join(HERE, 'ch0_class_map.tsv'), encoding='utf-8') if l[:1].isalnum() and not l.startswith('june_fn')]
starts = sorted(int(r[0], 16) for r in rows)
nxt = {a: (starts[i + 1] if i + 1 < len(starts) else HI) for i, a in enumerate(starts)}
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
callees = collections.defaultdict(lambda: {'by': set(), 'ph': set()})
for r in rows:
    if r[6] == 'n/a' or r[1] in ('thunk', 'dead', 'not-Elsword'):
        continue
    a = int(r[0], 16)
    code = d[a - B:nxt[a] - B]
    for i in md.disasm(code, a):
        if i.mnemonic == 'call' and i.op_str.startswith('0x'):
            v = int(i.op_str, 16)
            o = v - B
            if 0 <= o < len(d) - 5 and d[o] == 0xe9:
                v = (v + 5 + struct.unpack_from('<i', d, o + 1)[0]) & 0xffffffff
            if LO <= v < HI:
                continue
            c = callees['%08x' % v]
            c['by'].add(r[0])
            c['ph'].add(r[6])
print('callee\tmapped\tcited_elsewhere\tn_callers\tphases\tcallers')
for v, c in sorted(callees.items(), key=lambda kv: (-len(kv[1]['by']), kv[0])):
    cited = 'y' if v in mapped else ('y' if len(re.findall(v, fmtext)) else 'n')
    print('%s\t%s\t%s\t%d\t%s\t%s' % (v, 'y' if v in mapped else 'n', cited, len(c['by']), ' '.join(sorted(c['ph'])), ' '.join(sorted(c['by'])[:6])))
