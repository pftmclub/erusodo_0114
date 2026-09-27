"""Merge truncation_census.py's output with corpus_run.py + decompile_worker.py
sweep TSVs into the tracked june2017_re/decompile_corpus.tsv (one row per
corpus file). The sweep TSVs themselves are scratch (tmp/, gitignored) --
this script is what makes the tracked result regenerable from tracked code:
    python truncation_census.py
    python corpus_run.py --worker decompile_worker.py --root <14JUN17 data> \
        --out tmp/decompile_corpus_rest.tsv --files tmp/rest_files.tsv \
        --timeout 90 --workers 8 --resume -- --table ljconv_table.tsv
    python corpus_run.py --worker decompile_worker.py --root <14JUN17 data> \
        --out tmp/decompile_corpus_big.tsv --files tmp/big_files.tsv \
        --timeout 300 --workers 2 --resume -- --table ljconv_table.tsv
    python build_decompile_corpus.py [SWEEP_TSV ...]  # default: the two above
"""
import csv
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..'))
TRUNC_TSV = os.path.join(TRUNK, 'tmp', 'trunc_census.tsv')
DEFAULT_SWEEPS = [os.path.join(TRUNK, 'tmp', 'decompile_corpus_rest.tsv'),
                 os.path.join(TRUNK, 'tmp', 'decompile_corpus_big.tsv')]
OUT = os.path.join(HERE, 'decompile_corpus.tsv')


def load_sweep_row(row):
    d, n, size, wall, exit_code, timed_out = row[:6]
    rest = row[6:]
    cols = ['ljok', 'verdict', 'detail', 'nins', 'fixhits', 'opchash', 'nops']
    vals = {c: (rest[i] if i < len(rest) else '') for i, c in enumerate(cols)}
    vals.update(size=size, wall=wall, exit_code=exit_code, timed_out=timed_out)
    return (d, n), vals


def main(argv):
    sweep_paths = argv or DEFAULT_SWEEPS

    trunc = {}
    with open(TRUNC_TSV, encoding='utf-8') as f:
        for row in csv.DictReader(f, delimiter='\t'):
            trunc[(row['dir'], row['name'])] = row

    sweep = {}
    for path in sweep_paths:
        with open(path, encoding='utf-8') as f:
            rows = list(csv.reader(f, delimiter='\t'))
        for row in rows[1:]:
            if len(row) < 6:
                continue
            k, vals = load_sweep_row(row)
            sweep[k] = vals

    all_keys = sorted(set(trunc) | set(sweep))
    missing = 0
    with open(OUT, 'w', newline='', encoding='utf-8') as fh:
        w = csv.writer(fh, delimiter='\t')
        w.writerow(['dir', 'name', 'size', 'manifest_size', 'manifest_short_by', 'truncated',
                   'walk_status', 'protos_complete', 'last_complete_end', 'opcode_set_hash',
                   'ljconv_ok', 'decompile_verdict', 'wall_seconds', 'detail', 'n_instructions',
                   'n_fix_hits', 'n_distinct_opcodes', 'exit_code', 'timed_out'])
        for k in all_keys:
            t = trunc.get(k, {})
            s = sweep.get(k)
            if s is None:
                missing += 1
                w.writerow([k[0], k[1], t.get('size', ''), t.get('manifest_size', ''),
                           t.get('manifest_short_by', ''), t.get('class', 'NOT-YET-SWEPT'),
                           t.get('walk_status', ''), t.get('protos_complete', ''),
                           t.get('last_complete_end', ''), '', '', 'NOT-YET-SWEPT',
                           '', '', '', '', '', '', ''])
                continue
            w.writerow([k[0], k[1], s['size'], t.get('manifest_size', ''),
                       t.get('manifest_short_by', ''), t.get('class', ''),
                       t.get('walk_status', ''), t.get('protos_complete', ''),
                       t.get('last_complete_end', ''), s['opchash'], s['ljok'], s['verdict'],
                       s['wall'], s['detail'], s['nins'], s['fixhits'], s['nops'],
                       s['exit_code'], s['timed_out']])

    print('wrote %s: %d rows (%d without a sweep row yet)' % (OUT, len(all_keys), missing))
    import collections
    c = collections.Counter()
    for k in all_keys:
        s = sweep.get(k)
        c[s['verdict'] if s else 'NOT-YET-SWEPT'] += 1
    for k, v in c.most_common():
        print('  %-25s %d' % (k, v))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
