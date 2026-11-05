r"""char-raven CH0: write ch0_hand_names.tsv (fn, proposed_name, why) for every class-map row still `new?`.

    NO_HAND_NAMES=1 python june2017_re/char_raven/ch0_build_class_map.py ; python june2017_re/char_raven/ch0_make_hand_names.py ; python june2017_re/char_raven/ch0_build_class_map.py

A name is proposed only where Chung's method settles it: the destructor body (inside) and the MemoConditionRaven vtable entries (RTTI names
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
    '32c3e030': ('CanParryingState', 'inside: X2GURaven.cpp:13827 (ch0_rage.tsv)'),
    '32c41170': ('AddRageGuage', 'inside: X2GURaven.cpp:14440 (ch0_rage.tsv)'),
    '32c491a0': ('DoParrying', 'inside: X2GURaven.cpp:13857 (ch0_rage.tsv)'),
    '32c4f750': ('ClearNasodBall', 'inside: X2GURaven.cpp:13662 (ch0_rage.tsv)'),
    '32c4f7e0': ('CreateNasodBall', 'inside: X2GURaven.cpp:13686 (ch0_rage.tsv)'),
    '32c4fd80': ('RestoreNasodBall', 'inside: X2GURaven.cpp:13909 (ch0_rage.tsv)'),
    '32c4ff90': ('ChangePowerRateNasodBall', 'inside: X2GURaven.cpp:14090 (ch0_rage.tsv)'),
    '32c50060': ('AttackNasodBall', 'inside: X2GURaven.cpp:14132 (ch0_rage.tsv)'),
    '32c50330': ('DoFrameMoveNasodBall', 'inside: X2GURaven.cpp:14210 (ch0_rage.tsv)'),
    '32c4dfe0': ('CreateOverHeatObject', 'inside: X2GURaven.cpp:21337 (ch0_overheat.tsv)'),
    '32c4add0': ('_ShowOverheatIcon', 'inside: X2GURaven.cpp:17095 (ch0_overheat.tsv)'),
    '32c4e3c0': ('DeleteOverHeatObject', 'inside: X2GURaven.cpp:21396 (ch0_overheat.tsv)'),
    '32c3ece0': ('CX2GURaven', 'inside: X2GURaven.h:355 constructor (vtable store 32c3ed50, body 32c3ece0-32c409a6); caller 3230f309 is outside the class'),
    '32c54c10': ('~CX2GURaven', 'inside: X2GURaven.h:362 destructor body (ctor store 32c3ed50 / dtor store 32c54c3d; the scalar deleting dtor 32c6a620 calls it, then the delete helper 32fe2300)'),
    '32c69d80': ('?', 'MemoConditionRaven vtable 33511404 slot 0 (RTTI .?AVMemoConditionRaven@CX2GURaven@@; also the entry of the base vtable 334720bc, so shared with the base memo condition); name by the memo-condition pattern (CH1h / CH4a decision)'),
    '32c69f40': ('?', 'MemoConditionRaven vtable 33511404 slot 4 (pointer @33511444); name by the memo-condition pattern (CH1h / CH4a decision)'),
    '32c69f50': ('?', 'MemoConditionRaven vtable 33511404 slot 6 (pointer @3351144c); name by the memo-condition pattern (CH1h / CH4a decision)'),
    '32c63660': ('?', 'user vslot 0x300 (uncarved): 5-byte jmp thunk to 32c5d540 (80 lines, no state registration); read 32c5d540'),
    '32c3c7f0': ('?', 'user vslot 0x73c (uncarved, 13 insns, no calls): the passive delegate pair with 0x748 (plan CH4a); name by the delegate pattern'),
    '32c3c820': ('?', 'user vslot 0x748 (uncarved, 13 insns, no calls): the passive delegate pair with 0x73c (plan CH4a); name by the delegate pattern'),
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
