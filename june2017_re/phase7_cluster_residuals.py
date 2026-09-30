"""Phase 7 (RANOX_LUA_DECOMPILE_FIX_PLAN.md): cluster the corpus sweep's
`other-ljd` / `reorder` / `upvalue` / `clobber` rows by the SHAPE of their
first divergence, so RANOX_LUA_DECOMPILE_BUG.md's "Residual ljd defects"
section can name a small number of classes instead of listing hundreds of
individual (file, function) rows. Does not fix anything.

    python phase7_cluster_residuals.py --verdicts june2017_re/decompile_verdicts.tsv
        --outdir tmp/phase7_out

Signature: (dominant_raw, labels tuple, side-event text with every number,
mock string and slotN/uvN register name masked). Prints, per signature,
the count, the distinct files it appears in, and one full example (file,
function, seed, first-divergence text) for a human to name and classify.
"""
import argparse
import collections
import csv
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)

NUM = re.compile(r'-?\d+(\.\d+)?')
REG = re.compile(r'\b(slot|uv)\d+\b')
MOCKSTR = re.compile(r'"m[0-9a-f]{6,}"')


def norm_event(e):
    e = MOCKSTR.sub('"s"', e)
    e = REG.sub(r'\1#', e)
    e = NUM.sub('#', e)
    return e


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument('--verdicts', default=os.path.join(HERE, 'decompile_verdicts.tsv'))
    ap.add_argument('--outdir', default=os.path.join(TRUNK, 'tmp', 'phase7_out'))
    ap.add_argument('--verdict-filter', default='other-ljd,reorder,upvalue,clobber')
    args = ap.parse_args(argv)

    wanted = set(args.verdict_filter.split(','))
    rows = [r for r in csv.DictReader(open(args.verdicts, encoding='utf-8'), delimiter='\t')
            if r['scope'] == 'corpus' and r['verdict'] in wanted]
    print('rows matching %s: %d' % (sorted(wanted), len(rows)))

    clusters = collections.defaultdict(list)
    json_cache = {}
    for r in rows:
        flat = '%s__%s' % (r['kom_dir'], r['kom_name'])
        jpath = os.path.join(args.outdir, flat + '.json')
        if jpath not in json_cache:
            json_cache[jpath] = json.load(open(jpath, encoding='utf-8')) if os.path.exists(jpath) else None
        j = json_cache[jpath]
        if j is None:
            clusters[('MISSING-JSON', '', '')].append((r, None))
            continue
        rt = j['results'].get('rt', {})
        fn = r['fn']
        first = None
        if fn == '<main>':
            first = None  # main's 'first' isn't carried in this JSON shape; note separately
        else:
            fr = rt.get('fns', {}).get(fn)
            first = fr.get('first') if fr else None
        if not first:
            clusters[(r['verdict'], r['dominant_raw'], 'NO-FIRST-RECORDED')].append((r, first))
            continue
        s, v, labels, i, xa, xb = first
        sig_event = norm_event(xb[min(len(xb) - 1, len(xb) // 1 - 1)]) if xb else ''
        # Use the actual differing event (last of xb, which first_diff() centers
        # on the divergence index) as the shape key.
        sig_event = norm_event(xb[-1]) if xb else ''
        key = (r['verdict'], tuple(sorted(labels)), sig_event)
        clusters[key].append((r, first))

    print('\ndistinct shapes: %d\n' % len(clusters))
    ordered = sorted(clusters.items(), key=lambda kv: -len(kv[1]))
    for key, items in ordered:
        files = sorted({(r['kom_dir'], r['kom_name']) for r, _ in items})
        print('=' * 100)
        print('shape %r' % (key,))
        print('  %d row(s) across %d file(s)' % (len(items), len(files)))
        ex_r, ex_first = items[0]
        print('  example: %s/%s :: %s' % (ex_r['kom_dir'], ex_r['kom_name'], ex_r['fn']))
        if ex_first:
            s, v, labels, i, xa, xb = ex_first
            print('    seed %d verdict %s labels %s event-index %d' % (s, v, labels, i))
            for e in xa:
                print('    orig | %s' % e)
            for e in xb:
                print('    side | %s' % e)
        print('  files (up to 8): %s' % ', '.join('%s/%s' % fk for fk in files[:8]))

    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
