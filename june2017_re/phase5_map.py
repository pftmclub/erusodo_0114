"""Phase 5 (RANOX_LUA_DECOMPILE_FIX_PLAN.md): map every march2014/**/*.src.lua to
its June2017 original.

Matching is by exact basename only (port `<name>.src.lua` -> June `<name>.lua`).
This is sound here because decompile_corpus.tsv (Phase 0's full corpus inventory)
has 7,328 rows and 7,328 *distinct* names -- verified below, not assumed. So a
name that matches at all matches exactly one (dir, name) in the June corpus;
"ambiguous" is defensive and is not expected to fire.

    python phase5_map.py

Writes:
  june2017_re/phase5_files.tsv   one row per march2014 port file: port_path
      (relative to Trunk), kom_dir, kom_name, map_status, candidates
  june2017_re/phase5_targets.tsv  dir\tname for corpus_run.py's --files, mapped
      rows only (what the sweep actually walks)
Prints a status/count summary.
"""
import csv
import glob
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)
CORPUS_TSV = os.path.join(HERE, 'decompile_corpus.tsv')
MARCH_ROOT = os.path.join(TRUNK, 'march2014')
OUT_FILES = os.path.join(HERE, 'phase5_files.tsv')
OUT_TARGETS = os.path.join(HERE, 'phase5_targets.tsv')


def load_name_index():
    name_idx = {}
    with open(CORPUS_TSV, encoding='utf-8') as fh:
        r = csv.reader(fh, delimiter='\t')
        header = next(r)
        assert header[:2] == ['dir', 'name'], header
        for row in r:
            if not row:
                continue
            d, n = row[0], row[1]
            name_idx.setdefault(n, []).append(d)
    dupes = {n: ds for n, ds in name_idx.items() if len(ds) > 1}
    return name_idx, dupes


def main(argv):
    name_idx, dupes = load_name_index()
    n_corpus_rows = sum(len(v) for v in name_idx.values())
    print('June corpus: %d rows, %d distinct names, %d duplicate name(s)' % (
        n_corpus_rows, len(name_idx), len(dupes)))
    if dupes:
        print('  (ambiguous mapping is possible for: %s)' % ', '.join(sorted(dupes)[:20]))

    port_files = sorted(glob.glob(os.path.join(MARCH_ROOT, '**', '*.src.lua'), recursive=True))
    print('march2014 port files: %d' % len(port_files))

    rows = []
    counts = {}
    seen_basename = {}
    for p in port_files:
        rel = os.path.relpath(p, TRUNK).replace('\\', '/')
        base = os.path.basename(p)
        assert base.endswith('.src.lua'), base
        stem = base[:-len('.src.lua')]
        kom_name = stem + '.lua'
        dup_port = seen_basename.get(stem)
        seen_basename[stem] = rel
        candidates = name_idx.get(kom_name, [])
        if dup_port:
            # Defensive: script's own assumption (unique basenames across
            # march2014) verified false. Report and keep going.
            status = 'duplicate-port-basename'
            kom_dir = ''
        elif len(candidates) == 1:
            status = 'mapped'
            kom_dir = candidates[0]
        elif len(candidates) == 0:
            status = 'no-june-original'
            kom_dir = ''
        else:
            status = 'ambiguous'
            kom_dir = ''
        counts[status] = counts.get(status, 0) + 1
        rows.append({
            'port_path': rel, 'kom_dir': kom_dir, 'kom_name': kom_name,
            'map_status': status, 'candidates': ','.join(candidates),
        })

    with open(OUT_FILES, 'w', encoding='utf-8', newline='\n') as fh:
        w = csv.writer(fh, delimiter='\t', lineterminator='\n')
        w.writerow(['port_path', 'kom_dir', 'kom_name', 'map_status', 'candidates'])
        for r in rows:
            w.writerow([r['port_path'], r['kom_dir'], r['kom_name'], r['map_status'], r['candidates']])

    with open(OUT_TARGETS, 'w', encoding='utf-8', newline='\n') as fh:
        w = csv.writer(fh, delimiter='\t', lineterminator='\n')
        w.writerow(['dir', 'name'])
        mapped = [r for r in rows if r['map_status'] == 'mapped']
        seen = set()
        for r in mapped:
            key = (r['kom_dir'], r['kom_name'])
            if key in seen:
                continue
            seen.add(key)
            w.writerow([r['kom_dir'], r['kom_name']])

    print('status counts: %s' % counts)
    print('wrote %s (%d rows), %s (%d targets)' % (OUT_FILES, len(rows), OUT_TARGETS, len(seen)))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
