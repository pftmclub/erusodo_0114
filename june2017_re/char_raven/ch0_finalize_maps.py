r"""char-raven CH0 items 7 / 8: ch0_rage_draft.tsv / ch0_overheat_draft.tsv (subagent digests) -> ch0_rage.tsv / ch0_overheat.tsv with an owner column.
    python june2017_re/char_raven/ch0_finalize_maps.py
Owner: a June class function (or an address inside one) takes its ch0_class_map.tsv phase; a live March piece with no class function goes to the
map's phase (rage CH1q, overheat CH4c); March-dead rows n/a; left-arm rows by the lead's routing (june-engine R14c owns 3295dfa0 / 329657d0).
"""
import os, re, bisect
HERE = os.path.dirname(os.path.abspath(__file__))
cm = {}
for l in open(os.path.join(HERE, 'ch0_class_map.tsv'), encoding='utf-8', errors='replace'):
    c = l.rstrip('\n').split('\t')
    if len(c) > 6 and re.match(r'^[0-9a-f]{8}$', c[0]):
        cm[int(c[0], 16)] = c[6]
S = sorted(cm)
LO, HI = 0x32c25a20, 0x32c6a640
LEFT = {'329657d0': 'june-engine R14c (inside 3295dfa0, which R14c ports whole)', '3237aa40': 'n/a (UnitTemplet loader; key read, no Raven code)',
        '3201c3a0': 'done (SERV_IRUHADEV_JUNE_DIVERGE_NOT_RENDERING_EQUIP)'}


def owner(addrs, default, piece, live):
    if live.startswith('DEAD') or live.startswith('dead'):
        return 'n/a (dead in March)'
    if piece.startswith('LEFTARM'):
        for a in addrs:
            if a in LEFT:
                return LEFT[a]
        if 'preload' in piece:
            return 'n/a (beginning game / create-unit preload, out of scope)'
        return 'june-engine R14c (locate with 3295dfa0)' if 'ReplaceEmptyToBasic' in piece else 'CH4c (locate June reader)'
    phs = []
    for a in addrs:
        v = int(a, 16)
        if LO <= v < HI:
            i = bisect.bisect_right(S, v) - 1
            if i >= 0:
                phs.append(cm[S[i]])
    phs = [p for p in phs if p not in ('n/a', '')]
    if phs:
        return ' / '.join(dict.fromkeys(phs))
    return default


for src, dst, default in (('ch0_rage_draft.tsv', 'ch0_rage.tsv', 'CH1q'), ('ch0_overheat_draft.tsv', 'ch0_overheat.tsv', 'CH4c')):
    rows = [l.rstrip('\r\n').split('\t') for l in open(os.path.join(HERE, src), encoding='utf-8')]
    out = ['\t'.join(rows[0] + ['owner'])]
    for r in rows[1:]:
        r += [''] * (6 - len(r))
        addrs = re.findall(r'\b(3[0-9a-f]{7})\b', r[3])
        out.append('\t'.join(r + [owner(addrs, default, r[0], r[2])]))
    open(os.path.join(HERE, dst), 'w', encoding='utf-8', newline='\n').write('\n'.join(out) + '\n')
    print(dst, len(out) - 1)
