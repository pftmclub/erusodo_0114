"""char-eve CH0 item 6: callees outside Eve's class range that funcmap does not map, per phase of the calling function.

    python june2017_re/char_eve/ch0_unmapped_callees.py > june2017_re/char_eve/ch0_unmapped_callees.tsv

Callers: every row of ch0_class_map.tsv except n/a. Callees: direct `call` targets (funcdis.py) outside 0x32965dd0-0x329cf870,
resolved through a 5-byte jmp thunk. 'mapped' = the address appears in funcmap.tsv's first column.
"""
import os, re, sys, subprocess, collections, struct
HERE = os.path.dirname(os.path.abspath(__file__))
RE = os.path.normpath(os.path.join(HERE, '..'))
LO, HI = 0x32965dd0, 0x329cf870
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
B = 0x31aa0c00
d = open(EXE, 'rb').read()
mapped = set()
for l in open(os.path.join(RE, 'funcmap.tsv'), encoding='utf-8', errors='replace'):
    mapped.update(re.findall(r'\b([0-9a-f]{8})\b', l.split('\t')[0]))
rows = [l.rstrip('\n').split('\t') for l in open(os.path.join(HERE, 'ch0_class_map.tsv'), encoding='utf-8') if l[:1].isalnum() and not l.startswith('june_fn')]
callees = collections.defaultdict(lambda: {'by': set(), 'ph': set()})
for r in rows:
    if r[5] == 'n/a':
        continue
    out = subprocess.run([sys.executable, os.path.join(RE, 'funcdis.py'), '--func', r[0]], capture_output=True, text=True).stdout
    for t in re.findall(r': call 0x([0-9a-f]{8})', out):
        v = int(t, 16)
        o = v - B
        if 0 <= o < len(d) - 5 and d[o] == 0xe9:
            v = (v + 5 + struct.unpack_from('<i', d, o + 1)[0]) & 0xffffffff
        if LO <= v < HI:
            continue
        c = callees['%08x' % v]
        c['by'].add(r[0])
        c['ph'].add(r[5])
print('callee\tmapped\tn_callers\tphases\tcallers')
for v, c in sorted(callees.items(), key=lambda kv: (-len(kv[1]['by']), kv[0])):
    print('%s\t%s\t%d\t%s\t%s' % (v, 'y' if v in mapped else 'n', len(c['by']), ' '.join(sorted(c['ph'])), ' '.join(sorted(c['by'])[:6])))
