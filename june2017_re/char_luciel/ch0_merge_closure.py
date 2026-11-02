"""Merge the Luciel_Duo and Luciel_Demonio effect closures (same C++ range 32ae4080..32b4e770) into ch0_effect_closure.tsv.

    python june2017_re/char_luciel/ch0_merge_closure.py DUO.tsv DEM.tsv OUT.tsv
Each input is charport_closure.py output for one script with --range 0x32ae4080 0x32b4e770. Rows are unioned (exact duplicates dropped:
the C++ fn:<va> rows are the same in both runs); the summary comment lines are recomputed.
"""
import collections
import sys

duo, dem, out = sys.argv[1:4]
kinds = ['damage-effect', 'effect-set', 'particle', 'mesh-player', 'texture', 'mesh', 'sound', 'other']
stats = ['june-bytes', 'march-copy', 'missing']
rows, seen, heads, extra = [], set(), [], []
for path in (duo, dem):
    first = True
    for line in open(path, encoding='utf-8'):
        line = line.rstrip('\n')
        if first:
            first = False
            hdr = line
            continue
        if line.startswith('#'):
            if line.startswith('# class script') or line.startswith('#   unmatched'):
                (heads if line.startswith('# class') else extra).append(line)
            continue
        p = line.split('\t')
        key = tuple(p)
        if key in seen:
            continue
        seen.add(key)
        rows.append(p)
uniq = {}
for r in rows:
    uniq.setdefault((r[1], r[2].lower() if r[1] in ('texture', 'mesh', 'sound', 'other') else r[2]), r)
with open(out, 'w', encoding='utf-8', newline='\n') as f:
    f.write(hdr + '\n')
    for r in rows:
        f.write('\t'.join(r) + '\n')
    f.write('# total\n')
    for h in heads:
        f.write(h + '\n')
    f.write('# rows %d (union of both scripts, exact duplicates dropped), unique (kind,name) %d\n' % (len(rows), len(uniq)))
    for label, src in (('unique', list(uniq.values())), ('rows', rows)):
        f.write('# by kind (%s)\t%s\n' % (label, '\t'.join('%s=%d' % (k, sum(1 for r in src if r[1] == k)) for k in kinds)))
        f.write('# by march_status (%s)\t%s\n' % (label, '\t'.join('%s=%d' % (s, sum(1 for r in src if r[3] == s)) for s in stats)))
        f.write('# kind x status (%s)\n' % label)
        for k in kinds:
            f.write('#   %s\t%s\n' % (k, '\t'.join('%s=%d' % (s, sum(1 for r in src if r[1] == k and r[3] == s)) for s in stats)))
    und = [r for r in uniq.values() if 'june-undefined' in r[7]]
    f.write('# june-undefined: %d\n' % len(und))
    for e in sorted(set(extra)):
        f.write(e + '\n')
print('rows', len(rows), 'unique', len(uniq), 'missing', sum(1 for r in uniq.values() if r[3] == 'missing'), 'undefined', len(und))
