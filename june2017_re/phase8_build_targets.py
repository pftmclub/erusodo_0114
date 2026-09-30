"""Phase 8 (RANOX_LUA_DECOMPILE_FIX_PLAN.md): split the corpus files outside
data037_kom (Phase 7's scope) into the three sweep batches the plan calls
for, reading decompile_corpus.tsv (Phase 0) rather than re-deriving anything:

  functions  protos_complete > 1 and size <= 1MB -- run FULL (learn()), so
             types.tsv gets populated for the fast batch to read.
  fast       protos_complete == 1 and size <= 1MB -- run --fast (no learn()).
             entry_points() is empty for these (ljdiff.lua only discovers
             functions reachable from main-chunk globals, and a file with
             zero nested prototypes has none to discover), confirmed by
             reading ljdiff.lua directly; --fast's calibration check is in
             "Phase 8 result".
  big        size > 1,000,000 bytes (14 files, all in data036_kom per Phase 0's
             own count) -- run FULL if protos_complete > 1 (DamageEffect.lua,
             EffectSet.lua: the known deepcopy-defect files, never treated as
             exempt by assumption), else --fast. Both get a larger
             --budget/--decompile-timeout.

    python phase8_build_targets.py --corpus june2017_re/decompile_corpus.tsv
        --outdir june2017_re

Writes phase8_targets_functions.tsv, phase8_targets_fast.tsv,
phase8_targets_big.tsv (dir, name, protos_complete, size columns) under
--outdir, and prints the count of each plus the overlap check (494 + 3715 +
14 == 4223, the whole non-data037_kom corpus, confirmed against Phase 0's own
7,328-file total minus data037_kom's 3,105).
"""
import argparse
import csv
import os

HERE = os.path.dirname(os.path.abspath(__file__))
BIG_THRESHOLD = 1_000_000


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument('--corpus', default=os.path.join(HERE, 'decompile_corpus.tsv'))
    ap.add_argument('--outdir', default=HERE)
    ap.add_argument('--exclude-dir', default='data037_kom')
    args = ap.parse_args(argv)

    rows = list(csv.DictReader(open(args.corpus, encoding='utf-8'), delimiter='\t'))
    rem = [r for r in rows if r['dir'] != args.exclude_dir]

    big = [r for r in rem if int(r['size']) > BIG_THRESHOLD]
    big_keys = {(r['dir'], r['name']) for r in big}
    rest = [r for r in rem if (r['dir'], r['name']) not in big_keys]
    functions = [r for r in rest if int(r['protos_complete']) > 1]
    fast = [r for r in rest if int(r['protos_complete']) == 1]

    assert len(functions) + len(fast) + len(big) == len(rem), \
        'split lost or duplicated a file: %d + %d + %d != %d' % (len(functions), len(fast), len(big), len(rem))

    def write(name, rs):
        path = os.path.join(args.outdir, name)
        with open(path, 'w', encoding='utf-8', newline='\n') as fh:
            w = csv.writer(fh, delimiter='\t', lineterminator='\n')
            w.writerow(['dir', 'name', 'protos_complete', 'size'])
            for r in sorted(rs, key=lambda r: (r['dir'], r['name'])):
                w.writerow([r['dir'], r['name'], r['protos_complete'], r['size']])
        print('%s: %d row(s) -> %s' % (name, len(rs), path))

    write('phase8_targets_functions.tsv', functions)
    write('phase8_targets_fast.tsv', fast)
    write('phase8_targets_big.tsv', big)
    print('total non-%s: %d (== %d + %d + %d)' % (args.exclude_dir, len(rem), len(functions), len(fast), len(big)))
    print('big files, protos_complete: %s' % sorted((r['name'], r['protos_complete']) for r in big))


if __name__ == '__main__':
    main(None)
