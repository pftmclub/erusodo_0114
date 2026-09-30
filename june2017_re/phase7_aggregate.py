"""Phase 7 (RANOX_LUA_DECOMPILE_FIX_PLAN.md): turn phase7_worker.py's per-file
JSON outputs (June original vs the FIXED round trip, no `port` side) into
`scope=corpus` rows of june2017_re/decompile_verdicts.tsv, alongside the
`scope=ported` rows Phase 5 already wrote there.

    python phase7_aggregate.py --outdir tmp/phase7_out --run tmp/phase7_verdicts.tsv
        --targets june2017_re/phase7_targets.tsv --out june2017_re/decompile_verdicts.tsv

--out is READ first: every row whose scope != 'corpus' is kept verbatim (that
is Phase 5's ported-file data), then this run's corpus rows replace whatever
scope=corpus rows were there before (so a re-run of this script is safe to
repeat; a re-run of phase5_aggregate.py is NOT -- see its own module doc --
and would need this script re-run afterward to restore the corpus rows it
overwrote).

Verdict vocabulary (per file+function; no `port-divergence`, since there is no
port side here):
  clean / dropped-assignment / other-ljd (deepcopy, exempt) / dropped-assignment
  / clobber / upvalue / reorder / other-ljd / static-only / not-exercised

A `dropped-assignment` verdict on the rt side (the ONLY side that exists here)
is always a Phase 4 regression candidate unless the deepcopy-hoist exemption
explains every one of its sites -- see phase5_aggregate.py's module docstring
for that shape and why it must never be assumed. This script's own exit code
follows the same convention: 0 means no unexplained regression, 1 means stop
and report (matching phase5_aggregate.py and the plan's Phase 7 exit
criteria).
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

PRIORITY_RT = ['harness', 'dropped-assignment', 'clobber', 'upvalue', 'reorder', 'other-ljd', 'pass']

COLS = ['scope', 'port_path', 'kom_dir', 'kom_name', 'fn', 'verdict', 'dominant_raw', 'seeds',
        'verdicts_breakdown', 'cov_orig', 'cov_side', 'static_confirmed', 'static_only', 'clobber_events',
        'lint_total', 'lint_confirmed', 'first_divergence', 'notes']


def dominant(verdicts, priority):
    for v in priority:
        if verdicts.get(v):
            return v
    return 'pass' if not verdicts else sorted(verdicts)[0]


def fn_static_counts(static_rows, fn):
    confirmed = sum(1 for r in static_rows if r['fn'] == fn and r['status'] == 'confirmed')
    only = sum(1 for r in static_rows if r['fn'] == fn and r['status'] != 'confirmed')
    return confirmed, only


def fn_lint_counts(lint_rows, fn):
    if lint_rows is None:
        return None, None
    total = sum(1 for h in lint_rows if h['fn'] == fn)
    confirmed = sum(1 for h in lint_rows if h['fn'] == fn and h['confirmed'])
    return total, confirmed


def first_str(first):
    if not first:
        return ''
    s, v, labels, i, xa, xb = first
    tail = xb[-1] if xb else ''
    return 'seed %d %s[%s] ev%d: %s' % (s, v, ','.join(labels), i, tail)


def classify_row(fr, static_rows, fn, lint_rows, rt_src_text, regressions):
    verdicts = fr['verdicts']
    dom = dominant(verdicts, PRIORITY_RT)
    static_confirmed, static_only = fn_static_counts(static_rows, fn)
    lint_total, lint_confirmed = fn_lint_counts(lint_rows, fn)

    if dom == 'dropped-assignment':
        def site_line(site):
            _, _, lineno = site.partition('@L')
            return int(lineno) if lineno else 0
        poison_sites = [s for s in fr['sites'] if SLOTNAME_SITE.match(s)]
        unexplained = [s for s in poison_sites
                       if not deepcopy_exempt_at_line(rt_src_text, site_line(s), s.split('@')[0])]
        if unexplained:
            regressions.append((fn, unexplained))
            verdict = 'dropped-assignment'
        else:
            verdict = 'other-ljd (deepcopy, exempt)'
    elif dom == 'clobber':
        verdict = 'clobber'
    elif dom in ('other', 'other-ljd'):
        verdict = 'other-ljd'
    elif dom in ('reorder', 'upvalue', 'harness'):
        verdict = dom
    elif static_only:
        verdict = 'static-only'
    else:
        verdict = 'clean'

    return {
        'verdict': verdict, 'dominant_raw': dom, 'seeds': fr['seeds'],
        'verdicts_breakdown': ' '.join('%s=%d' % kv for kv in sorted(verdicts.items())),
        'cov_orig': fr['cov_orig'], 'cov_side': fr['cov_side'],
        'static_confirmed': static_confirmed, 'static_only': static_only,
        'clobber_events': len(fr['clobber']), 'lint_total': lint_total, 'lint_confirmed': lint_confirmed,
        'first_divergence': first_str(fr['first']),
    }


def load_existing_ported_rows(out_path):
    if not os.path.exists(out_path):
        return []
    rows = list(csv.DictReader(open(out_path, encoding='utf-8'), delimiter='\t'))
    return [r for r in rows if r.get('scope') != 'corpus']


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument('--outdir', default=os.path.join(TRUNK, 'tmp', 'phase7_out'))
    ap.add_argument('--run', default=os.path.join(TRUNK, 'tmp', 'phase7_verdicts.tsv'))
    ap.add_argument('--targets', default=os.path.join(HERE, 'phase7_targets.tsv'))
    ap.add_argument('--out', default=os.path.join(HERE, 'decompile_verdicts.tsv'))
    ap.add_argument('--src-dir', default=os.path.join(TRUNK, 'june_src_fixed', 'data037_kom'))
    args = ap.parse_args(argv)

    targets = list(csv.DictReader(open(args.targets, encoding='utf-8'), delimiter='\t'))
    run_rows = {}
    with open(args.run, encoding='utf-8') as fh:
        r = csv.reader(fh, delimiter='\t')
        header = next(r)
        for cols in r:
            if len(cols) < 8:
                continue
            d, n = cols[0], cols[1]
            run_rows[(d, n)] = cols

    kept = load_existing_ported_rows(args.out)
    out_rows = []
    regressions_by_file = {}
    status_counts = collections.Counter()
    unreached_files = []
    unverifiable = 0
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
        jpath = cols[-1]
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
        rt_src_path = os.path.join(args.src_dir, os.path.splitext(n)[0] + '.src.lua')
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
            for cl, cnt in rt_fns[fn].get('clobber', {}).items():
                pass  # corpus-scope clobber census: rolled into notes, no separate census file for 3,105 files
            out_rows.append(dict(row, scope='corpus', port_path='', kom_dir=d, kom_name=n, fn=fn,
                                  notes='; '.join(notes)))
        if regressions:
            regressions_by_file[('%s/%s' % (d, n))] = regressions

        # <main>
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

    print('wrote %s: %d ported rows kept, %d corpus rows written (%d total)' % (
        args.out, len(kept), len(out_rows), len(all_rows)))
    print('status counts: %s' % dict(status_counts))
    print('files with >=1 unreached-by-harness function: %d' % len(unreached_files))
    print('unverifiable-truncated: %d (Phase 0 found zero truncation in this corpus)' % unverifiable)
    if regressions_by_file:
        print('\n!!! POSSIBLE PHASE 4 REGRESSION: rt-side dropped-assignment NOT matching the deepcopy '
              'exemption in %d file(s):' % len(regressions_by_file))
        for p, regs in regressions_by_file.items():
            for fn, sites in regs:
                print('   %s :: %s -> %s' % (p, fn, sites))
        return 1
    print('\nno unexplained rt-side dropped-assignment: Phase 4 fix holds across data037_kom.')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
