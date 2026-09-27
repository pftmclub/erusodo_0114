"""Phase 0 step 3: is every June corpus .lua file's on-disk copy trustworthy?
Two independent checks per file, both recorded:
  - manifest: elstools inspect's size for this entry vs. the on-disk size.
  - structure: ljconv.instruction_offsets_tolerant's dump walk.

    python truncation_census.py [--root DIR] [--out FILE.tsv]

Writes one row per file: dir, name, size, manifest_size, manifest_short_by,
manifest_status, walk_status, protos_complete, last_complete_end,
n_instructions, class. `class` is 'clean' (both checks agree nothing is
missing), 'terminator-only' (manifest short by exactly 1 byte AND the walk
ends cleanly at a prototype boundary -- see instruction_offsets_tolerant's
docstring for why that combination, not the walk alone, is what proves it),
'no-manifest-entry', or 'unverifiable-truncated' (including the two
DISAGREE-* cases: manifest and the structural walk disagree with each other).

Default root: the June 14JUN17 reference client, all 21 *_kom folders with
.lua content.
"""
import argparse
import csv
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import fxdata  # noqa: E402
import ljconv  # noqa: E402

DIRS = ['data001', 'data002', 'data014', 'data034', 'data035', 'data036', 'data037', 'data038',
        'data039', 'data040', 'data064', 'data119', 'data217', 'data224', 'datai003', 'datai014',
        'datai015', 'datai016', 'datai022', 'datai024', 'datai030']


def classify(mstatus, mshort, wstatus):
    if mstatus == 'size-match' and wstatus == 'complete':
        return 'clean'
    if mstatus == 'size-short' and mshort == 1 and wstatus == 'no-terminator':
        return 'terminator-only'
    if mstatus == 'no-manifest-entry':
        return 'no-manifest-entry'
    if mstatus == 'size-match' and wstatus != 'complete':
        return 'DISAGREE-manifest-ok-walk-bad'
    if mstatus == 'size-short' and wstatus == 'complete':
        return 'DISAGREE-manifest-short-walk-complete'
    return 'unverifiable-truncated'


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', default=fxdata.JUNE_DATA)
    ap.add_argument('--out', default=os.path.join(HERE, '..', 'tmp', 'trunc_census.tsv'))
    args = ap.parse_args(argv)

    manifests = {}
    for stem in DIRS:
        m = fxdata.inspect(os.path.join(args.root, stem + '.kom'))
        manifests[stem] = {e['name'].lower(): e for e in m['files']}

    rows = []
    counts = {}
    for stem in DIRS:
        kdir = os.path.join(args.root, stem + '_kom')
        man = manifests[stem]
        for f in sorted(os.listdir(kdir)):
            low = f.lower()
            if not low.endswith('.lua') or low.endswith('.src.lua'):
                continue
            p = os.path.join(kdir, f)
            size = os.path.getsize(p)
            me = man.get(low)
            if me is None:
                mstatus, msize, mshort = 'no-manifest-entry', None, None
            else:
                msize = me['size']
                mshort = msize - size
                mstatus = 'size-match' if mshort == 0 else ('size-short' if mshort > 0 else 'size-LONGER')
            d = open(p, 'rb').read()
            r = ljconv.instruction_offsets_tolerant(d, ljconv.JUNE_MAGIC)
            cls = classify(mstatus, mshort, r['status'])
            counts[cls] = counts.get(cls, 0) + 1
            rows.append((stem + '_kom', f, size, msize, mshort, mstatus, r['status'],
                        r['protos_complete'], r['last_complete_end'], len(r['offs']), cls))

    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    with open(args.out, 'w', newline='', encoding='utf-8') as fh:
        w = csv.writer(fh, delimiter='\t')
        w.writerow(['dir', 'name', 'size', 'manifest_size', 'manifest_short_by', 'manifest_status',
                   'walk_status', 'protos_complete', 'last_complete_end', 'n_instructions', 'class'])
        w.writerows(rows)

    print('wrote %s: %d rows' % (args.out, len(rows)))
    for k, v in sorted(counts.items(), key=lambda kv: -kv[1]):
        print('  %-45s %d' % (k, v))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
