"""Phase 8 (RANOX_LUA_DECOMPILE_FIX_PLAN.md): turn phase8_worker.py's per-file
JSON outputs (June original vs the FIXED round trip, no `port` side -- same
shape as Phase 7) into `scope=corpus` rows of june2017_re/decompile_verdicts.tsv,
for the ~4,200 files outside data037_kom.

    python phase8_aggregate.py --outdir tmp/phase8_out --run tmp/phase8_verdicts.tsv
        --targets tmp/phase8_targets_TAG.tsv --out june2017_re/decompile_verdicts.tsv

--out is READ first, same convention as phase7_aggregate.py, but Phase 8 runs
in THREE batches (functions / fast-path / big files, kept sequential -- see
the plan's Phase 8 "Work" item and RANOX_LUA_DECOMPILE_BUG.md's corpus-run
notes) that split files WITHIN a kom_dir (by protos_complete/size, not by
folder -- confirmed 8 folders appear in both the functions and fast-path
target lists, with zero (dir, name) pairs in common), and each batch's run is
aggregated separately so a later batch's aggregation doesn't have to wait on
an earlier one. So the scoping here is stricter than phase7_aggregate.py's
'everything scope=corpus gets replaced', and MUST be by exact (dir, name)
pair, not by dir alone -- an earlier dir-scoped version of this script
silently dropped a sibling batch's rows for every shared folder the first
time two batches were aggregated back to back; see "Phase 8 result" in the
plan. A row is kept from the existing --out file iff its scope != 'corpus' OR
(its kom_dir, kom_name) is not one of the (dir, name) pairs THIS run's
--targets covers. That preserves Phase 7's data037_kom rows always, and lets
Phase 8's own three batches (and any resumed/re-run subset of them) coexist
and be re-run individually without clobbering each other.

Verdict vocabulary, classify_row(): identical to phase7_aggregate.py (shared
via phase5_aggregate.deepcopy_exempt_at_line()/SLOTNAME_SITE). This script's
own exit code follows the same convention: 0 means no unexplained
dropped-assignment regression, 1 means stop and report.
"""
import argparse
import collections
import csv
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)
sys.path.insert(0, HERE)
from phase5_aggregate import deepcopy_exempt_at_line, SLOTNAME_SITE  # noqa: E402
from phase7_aggregate import (  # noqa: E402
    PRIORITY_RT, COLS, dominant, fn_static_counts, fn_lint_counts, first_str, classify_row,
)


def load_existing_rows(out_path, run_pairs):
    """Keep every row except scope=corpus rows whose (kom_dir, kom_name) is
    one of the (dir, name) pairs this run's --targets covers (those get
    replaced by this run's own output below). Must be pair-scoped, not
    dir-scoped: Phase 8's three batches split files WITHIN a kom_dir, so a
    dir-only check would drop a sibling batch's rows for any shared folder."""
    if not os.path.exists(out_path):
        return []
    rows = list(csv.DictReader(open(out_path, encoding='utf-8'), delimiter='\t'))
    return [r for r in rows if not (r.get('scope') == 'corpus' and (r.get('kom_dir'), r.get('kom_name')) in run_pairs)]


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument('--outdir', default=os.path.join(TRUNK, 'tmp', 'phase8_out'))
    ap.add_argument('--run', default=os.path.join(TRUNK, 'tmp', 'phase8_verdicts.tsv'))
    ap.add_argument('--targets', required=True)
    ap.add_argument('--out', default=os.path.join(HERE, 'decompile_verdicts.tsv'))
    ap.add_argument('--src-root', default=os.path.join(TRUNK, 'june_src_fixed'))
    args = ap.parse_args(argv)

    targets = list(csv.DictReader(open(args.targets, encoding='utf-8'), delimiter='\t'))
    run_pairs = {(t['dir'], t['name']) for t in targets}
    run_dirs = {t['dir'] for t in targets}
    run_rows = {}
    with open(args.run, encoding='utf-8') as fh:
        r = csv.reader(fh, delimiter='\t')
        header = next(r)
        for cols in r:
            if len(cols) < 8:
                continue
            d, n = cols[0], cols[1]
            run_rows[(d, n)] = cols

    kept = load_existing_rows(args.out, run_pairs)
    out_rows = []
    regressions_by_file = {}
    status_counts = collections.Counter()
    unreached_files = []
    n_clean_files = 0

    for t in targets:
        d, n = t['dir'], t['name']
        cols = run_rows.get((d, n))
        if cols is None:
            status_counts['MISSING'] += 1
            out_rows.append({'scope': 'corpus', 'kom_dir': d, 'kom_name': n, 'fn': '-', 'verdict': 'not-run',
                              'notes': 'no row in %s' % args.run})
            continue
        status = cols[6]
        status_counts[status] += 1
        if status != 'ok':
            out_rows.append({'scope': 'corpus', 'kom_dir': d, 'kom_name': n, 'fn': '-', 'verdict': status,
                              'notes': ' '.join(cols[7:])[:300]})
            continue
        # corpus_run.py's 6 fixed columns (dir,name,size,wall,exit,timed_out) + phase8_worker's
        # own emit() columns (status,harness,dropped,other,unexplained,n_fns,out_path,...) --
        # out_path is NOT the last column here (unlike phase7_worker), since --fast and the
        # per-stage timings trail it. Index 12 = out_path.
        jpath = cols[12]
        if not os.path.isabs(jpath):
            jpath = os.path.join(TRUNK, jpath)
        if not os.path.exists(jpath):
            out_rows.append({'scope': 'corpus', 'kom_dir': d, 'kom_name': n, 'fn': '-', 'verdict': 'not-run',
                              'notes': 'json missing at ' + jpath})
            continue

        j = json.load(open(jpath, encoding='utf-8'))
        rt = j['results'].get('rt', {})
        rt_fns = rt.get('fns', {})
        rt_static = rt.get('static', [])
        rt_src_path = os.path.join(args.src_root, d, os.path.splitext(n)[0] + '.src.lua')
        rt_src_text = open(rt_src_path, encoding='utf-8', errors='replace').read() if os.path.exists(rt_src_path) else ''

        regressions = []
        static_regressions = []
        for row in rt_static:
            if row['tags']:
                continue
            if not deepcopy_exempt_at_line(rt_src_text, row['line'], row['name']):
                static_regressions.append(row)
        if static_regressions:
            regressions_by_file.setdefault('%s/%s' % (d, n), []).append(
                ('<static>', ['%s@L%d(status=%s,seeds=%d)' % (r['name'], r['line'], r['status'], r['seeds'])
                              for r in static_regressions]))

        file_has_unreached = False
        for fn in sorted(rt_fns):
            row = classify_row(rt_fns[fn], rt_static, fn, None, rt_src_text, regressions)
            notes = []
            if fn in rt.get('only_orig', []):
                notes.append('unreached-by-harness(only in June trace)')
                file_has_unreached = True
            out_rows.append(dict(row, scope='corpus', port_path='', kom_dir=d, kom_name=n, fn=fn,
                                  notes='; '.join(notes)))
        if regressions:
            regressions_by_file[('%s/%s' % (d, n))] = regressions

        rt_main = rt.get('main')
        rt_mv = rt.get('main_values', {})
        main_notes = []
        if rt_main:
            main_notes.append('rt_main=%s' % rt_main['verdict'])
        if rt_mv:
            main_notes.append('main_values differ=%d only_orig=%d only_side=%d(of %d)' % (
                len(rt_mv.get('differ', [])), len(rt_mv.get('only_orig', [])), len(rt_mv.get('only_side', [])),
                rt_mv.get('n', 0)))
        main_verdict = 'clean'
        if rt_main and 'dropped-assignment' in rt_main['labels']:
            main_sites = (rt_main.get('sites') or {}).keys() if isinstance(rt_main.get('sites'), dict) else []
            poison_sites = [s for s in main_sites if SLOTNAME_SITE.match(s)]
            unexplained = [s for s in poison_sites
                           if not deepcopy_exempt_at_line(rt_src_text, int(s.partition('@L')[2] or 0), s.split('@')[0])]
            if unexplained:
                regressions_by_file.setdefault('%s/%s' % (d, n), []).append(('<main>', unexplained))
                main_verdict = 'dropped-assignment'
            else:
                main_verdict = 'other-ljd (deepcopy, exempt)'
        elif rt_main and rt_main['verdict'] not in ('pass',):
            main_verdict = 'other-ljd'
        out_rows.append({'scope': 'corpus', 'port_path': '', 'kom_dir': d, 'kom_name': n, 'fn': '<main>',
                          'verdict': main_verdict, 'dominant_raw': (rt_main or {}).get('verdict', ''),
                          'notes': '; '.join(main_notes)})
        if not rt_fns and main_verdict == 'clean' and not file_has_unreached:
            n_clean_files += 1
        if file_has_unreached:
            unreached_files.append('%s/%s' % (d, n))

    all_rows = kept + out_rows
    with open(args.out, 'w', encoding='utf-8', newline='\n') as fh:
        w = csv.DictWriter(fh, fieldnames=COLS, delimiter='\t', lineterminator='\n')
        w.writeheader()
        for r in all_rows:
            w.writerow({k: r.get(k, '') for k in COLS})

    print('wrote %s: %d row(s) kept from before, %d new corpus row(s) for dirs %s (%d total)' % (
        args.out, len(kept), len(out_rows), sorted(run_dirs), len(all_rows)))
    print('status counts: %s' % dict(status_counts))
    print('files with >=1 unreached-by-harness function: %d' % len(unreached_files))
    if regressions_by_file:
        print('\n!!! POSSIBLE PHASE 4 REGRESSION: rt-side dropped-assignment NOT matching the deepcopy '
              'exemption in %d file(s):' % len(regressions_by_file))
        for p, regs in regressions_by_file.items():
            for fn, sites in regs:
                print('   %s :: %s -> %s' % (p, fn, sites))
        return 1
    print('\nno unexplained rt-side dropped-assignment: Phase 4 fix holds.')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
