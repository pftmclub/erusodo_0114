r"""char-elesis CH0: write ch0_hand_names.tsv (fn, proposed_name, why) for every class-map row still `new?`. Copy of char_ara/ch0_make_hand_names.py.

    NO_HAND_NAMES=1 python june2017_re/char_elesis/ch0_build_class_map.py ; python june2017_re/char_elesis/ch0_make_hand_names.py ; python june2017_re/char_elesis/ch0_build_class_map.py

A name is proposed only where Chung's method settles it: the constructor and destructor bodies (inside) and the MemoConditionElesis vtable entries (RTTI names
read from the exe). Every other row stays `?` with a hint (callers, vslot, base function) for the transcribing phase to name by the class pattern.
"""
import os, re
HERE = os.path.dirname(os.path.abspath(__file__))
rows = {}
for l in open(os.path.join(HERE, 'ch0_class_map.tsv'), encoding='utf-8').read().splitlines()[1:]:
    p = l.split('\t')
    if re.fullmatch(r'[0-9a-f]{8}', p[0]):
        rows[p[0]] = p
SPECIAL = {
    '32b70ee0': ('CX2GUEL', 'inside: X2GUEl.h constructor (vtable store 32b70f40; its only caller 32b72489 is inside the unit factory 32b72430)'),
    '32b70990': ('~CX2GUEL', 'inside: X2GUEl.h:255 destructor body (vtable store 32b709ba; the scalar deleting dtor 32b90520 calls it)'),
    '32b642f0': ('?', 'MemoConditionElesis constructor (vtable 334fa4f4 store 32b642fa; RTTI .?AVMemoConditionElesis@CX2GUEL@@, type descriptor 3373d5b8); name by the memo-condition pattern (CH4b decision)'),
    '32b51980': ('?', 'MemoConditionElesis vtable 334fa4f4 slot 1 (RTTI .?AVMemoConditionElesis@CX2GUEL@@); name by the memo-condition pattern (CH4b decision)'),
    '32b519a0': ('?', 'MemoConditionElesis vtable 334fa4f4 slot 2; name by the memo-condition pattern (CH4b decision)'),
}
out = ['fn\tproposed_name\twhy']
for fn, p in sorted(rows.items()):
    if p[4] != 'new?':
        continue
    if fn in SPECIAL:
        out.append('%s\t%s\t%s' % ((fn,) + SPECIAL[fn]))
        continue
    if p[1].startswith('override'):
        out.append('%s\t?\tvirtual %s (base %s); not named by any sibling class map, funcmap or X2GUUser.h; read the body' % (fn, p[1].split()[1], p[7].replace('base ', '')))
        continue
    callers = re.findall(r'[0-9a-f]{8}', p[2])
    names = []
    for c in callers[:4]:
        r = rows.get(c)
        names.append('%s(%s)' % (c, (r[2].split(' / ')[0][:28] if r and r[1].startswith(('override', 'state-fn', 'registrar')) else r[1] if r else '?')))
    out.append('%s\t?\t%s%s' % (fn, p[2] if not callers else 'called by ' + ' '.join(names), ('; ' + p[7]) if p[7] and 'phase unsettled' not in p[7] else ''))
# char-elesis CH0: merge ch0_new_names.tsv (june_fn, phase, proposed_name, confidence, evidence) from the naming pass: a row with a name (not `?`) becomes the
# proposed name; an evidence starting `MARCH HAS` becomes an `inside:` row (March owns the function); a `?` row only gains the description. SPECIAL rows win.
NN = os.path.join(HERE, 'ch0_new_names.tsv')
if os.path.exists(NN):
    nn = {}
    for l in open(NN, encoding='utf-8').read().splitlines()[1:]:
        f = (l.split('\t') + ['', '', '', '', ''])[:5]
        nn[f[0]] = f
    merged = 0
    for i, l in enumerate(out):
        f = l.split('\t')
        if i == 0 or f[0] in SPECIAL or f[0] not in nn:
            continue
        _, _, name, conf, ev = nn[f[0]]
        if name != '?':
            why = ('inside: ' + ev) if ev.startswith('MARCH HAS') else 'named (%s): %s' % (conf, ev)
            out[i] = '%s\t%s\t%s' % (f[0], name, why)
        else:
            out[i] = '%s\t?\t%s; read: %s' % (f[0], f[2], ev)
        merged += 1
    print(merged, 'names merged from ch0_new_names.tsv')
open(os.path.join(HERE, 'ch0_hand_names.tsv'), 'w', encoding='utf-8', newline='\n').write('\n'.join(out) + '\n')
print(len(out) - 1, 'rows')
