r"""char-luciel CH0 item 6: callees outside Lu/Ciel's class range that funcmap does not map, per phase of the calling function.

    python june2017_re/char_luciel/ch0_unmapped_callees.py > june2017_re/char_luciel/ch0_unmapped_callees.tsv

Callers: every row of ch0_class_map.tsv of kind override / own-virtual / registrar / state-fn / helper (dead / not-luciel / thunk skipped, phase n/a skipped).
Each is disassembled in process (capstone) from its start to the next row's start; callees = direct `call` targets and direct `jmp` (tail call) targets
outside 0x32ae4080-0x32b4e76f, resolved through a 5-byte jmp thunk (a thunk inside the range that jumps out is followed).
'mapped' = the address appears in funcmap.tsv's first column. 'cited' = the address appears anywhere else in funcmap.tsv.
'other_chars' = the address has a row in Chung's / Eve's / Arme's ch0_unmapped_identified.tsv ( char:confidence ). 'diverge' = the address is in tools/data/june_march_divergent.tsv.
"""
import os, re, struct, collections, capstone
HERE = os.path.dirname(os.path.abspath(__file__))
RE = os.path.normpath(os.path.join(HERE, '..'))
TOOLS = os.path.normpath(os.path.join(RE, '..', 'tools', 'data'))
LO, HI = 0x32ae4080, 0x32b4e770
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
B = 0x31aa0c00
TRANSCRIBED = ('override', 'own-virtual', 'registrar', 'state-fn', 'helper')
d = open(EXE, 'rb').read()
mapped = set()
fmtext = open(os.path.join(RE, 'funcmap.tsv'), encoding='utf-8', errors='replace').read()
for l in fmtext.split('\n'):
    mapped.update(re.findall(r'\b([0-9a-f]{8})\b', l.split('\t')[0]))
other = collections.defaultdict(list)
for ch in ('char_chung', 'char_eve', 'char_arme'):
    p = os.path.join(RE, ch, 'ch0_unmapped_identified.tsv')
    if os.path.exists(p):
        for l in open(p, encoding='utf-8'):
            f = l.rstrip('\n').split('\t')
            if len(f) > 3 and re.fullmatch(r'[0-9a-f]{8}', f[0]):
                other[f[0]].append('%s:%s' % (ch[5:], f[3]))
div = {}
for l in open(os.path.join(TOOLS, 'june_march_divergent.tsv'), encoding='utf-8', errors='replace'):
    if l.startswith('#'):
        continue
    f = l.rstrip('\n').split('\t')
    if len(f) > 3:
        for a in re.findall(r'\b[0-9a-f]{8}\b', f[2]):
            div[a] = '%s/%s' % (f[0], f[3])
def key(ph):
    m = re.fullmatch(r'CH(\d)([a-z])(\d*)', ph)
    return (int(m.group(1)), m.group(2), int(m.group(3) or 0)) if m else (99, ph, 0)
rows = [l.rstrip('\n').split('\t') for l in open(os.path.join(HERE, 'ch0_class_map.tsv'), encoding='utf-8') if re.match(r'[0-9a-f]{8}\t', l)]
starts = sorted(int(r[0], 16) for r in rows)
nxt = {a: (starts[i + 1] if i + 1 < len(starts) else HI) for i, a in enumerate(starts)}
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
callees = collections.defaultdict(lambda: {'by': set(), 'ph': set(), 'byph': collections.defaultdict(set)})
def resolve(v):
    o = v - B
    if 0 <= o < len(d) - 5 and d[o] == 0xe9:
        v = (v + 5 + struct.unpack_from('<i', d, o + 1)[0]) & 0xffffffff
    return v
for r in rows:
    if r[5] == 'n/a' or not r[1].startswith(TRANSCRIBED):
        continue
    a = int(r[0], 16)
    code = d[a - B:nxt[a] - B]
    for i in md.disasm(code, a):
        if i.mnemonic in ('call', 'jmp') and i.op_str.startswith('0x'):
            v = int(i.op_str, 16)
            if i.mnemonic == 'jmp' and LO <= v < HI:
                continue
            v = resolve(v)
            if LO <= v < HI:
                continue
            c = callees['%08x' % v]
            c['by'].add(r[0]); c['ph'].add(r[5]); c['byph'][r[5]].add(r[0])
print('callee\tmapped\tcited_elsewhere\tn_callers\tphases\tcallers\tother_chars\tdiverge')
for v, c in sorted(callees.items(), key=lambda kv: (-len(kv[1]['by']), kv[0])):
    cited = 'y' if v in mapped else ('y' if len(re.findall(v, fmtext)) else 'n')
    print('%s\t%s\t%s\t%d\t%s\t%s\t%s\t%s' % (v, 'y' if v in mapped else 'n', cited, len(c['by']), ' '.join(sorted(c['ph'], key=key)), ' '.join(sorted(c['by'])[:6]), ' '.join(other.get(v, [])) or '-', div.get(v, '-')))
