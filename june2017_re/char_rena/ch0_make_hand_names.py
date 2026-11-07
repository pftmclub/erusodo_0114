r"""char-rena CH0: write ch0_hand_names.tsv (fn, proposed_name, why) for every class-map row still `new?`. Copy of char_raven/ch0_make_hand_names.py.

    NO_HAND_NAMES=1 python june2017_re/char_rena/ch0_build_class_map.py ; python june2017_re/char_rena/ch0_make_hand_names.py ; python june2017_re/char_rena/ch0_build_class_map.py

A name is proposed only where Chung's method settles it: the constructor and destructor bodies (inside) and the MemoConditionRena vtable entries (RTTI names
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
    '32ca2b60': ('CX2GULire_ElvenRanger', 'inside: X2GULire_ElvenRanger.h:376 constructor (vtable store 32ca2bc4, body 32ca2b60-32ca42e9)'),
    '32ca42f0': ('~CX2GULire_ElvenRanger', 'inside: X2GULire_ElvenRanger.h:383 destructor body (vtable store 32ca431f; the scalar deleting dtor 32cb6c50 calls it, then the delete helper 32fe2300)'),
    '32c69d80': ('?', 'MemoConditionRena vtable 33516854 slot 0 (RTTI .?AVMemoConditionRena@CX2GULire_ElvenRanger@@, type descriptor 3373d730; also the entry of the base vtable 334720bc, so shared with the base memo condition); name by the memo-condition pattern (CH1h / CH4a decision)'),
    '32c6d4d0': ('?', 'MemoConditionRena vtable 33516854 slot 1 (pointer @33516858); name by the memo-condition pattern (CH1h / CH4a decision)'),
    '32c6d4f0': ('?', 'MemoConditionRena vtable 33516854 slot 2 (pointer @3351685c); name by the memo-condition pattern (CH1h / CH4a decision)'),
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
open(os.path.join(HERE, 'ch0_hand_names.tsv'), 'w', encoding='utf-8', newline='\n').write('\n'.join(out) + '\n')
print(len(out) - 1, 'rows')
