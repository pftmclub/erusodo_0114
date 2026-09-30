"""corpus_run.py WORKER for Phase 7 (RANOX_LUA_DECOMPILE_FIX_PLAN.md): for one
June original in data037_kom, run ljdiff's own pipeline (as a library, same
reasoning as phase5_worker.py's docstring: corpus_run.py's per-file process
here is for OS-level timeout/resume/crash-isolation, not because ljdiff.py
needs it) with ONLY the FIXED round trip against the June original -- no
`port` side, since Phase 7's scope is the whole data037_kom folder, most of
which has no march2014/ port at all.

    python phase7_worker.py <abs_june_lua_path> --budget SECS --outdir DIR
        [--seeds N] [--hang-dir DIR] [--src-outdir DIR]

Writes:
  <outdir>/<flat>.json   the jsonified diff_traces() result for 'rt', plus the
                          raw trace totals -- INCLUDING the main chunk's raw
                          'sites' (phase5's jsonify_res dropped these, which
                          forced its aggregator to re-open ljdiff's own
                          tmp/ljdiff_<flat>.rt.json; here everything the
                          aggregator needs is in one file, and tmp/ is wiped
                          per file below to bound disk use over 3,105 files)
  <src-outdir>/<name>.src.lua   the fixed decompile text, copied out of tmp/
                          right after build() succeeds -- BEFORE learn/trace,
                          so a later learn/trace failure still leaves the
                          decompile behind for later ports to read.

Prints one TSV line to stdout:
    status  harness_total  rt_dropped  rt_other  rt_unexplained_dropped
    n_fns  out_path

status: ok / STEP_ERROR / TIMEOUT-INTERNAL / CRASH (caught here so
corpus_run.py's own CRASH bucket stays reserved for process death).
rt_dropped/rt_other are summed over every function's verdict counts (seed
counts, not function counts) -- see summary_row() in ljdiff.py.
rt_unexplained_dropped is rt_dropped's sites (dynamic, all functions, PLUS
<main>) minus every site phase5_aggregate.deepcopy_exempt_at_line() explains
as the known, separate deepcopy-hoist ljd defect -- and, coverage-
independently, minus every untagged static must-define row it also explains.
A file where this is > 0 is a Phase 4 regression candidate: STOP the sweep
and report it (see the plan's Phase 7 exit criteria). This mirrors
phase5_aggregate.py's two regression checks, run per-file so corpus_run.py's
own TSV carries a live signal a Monitor can grep for, rather than only
surfacing at the end from phase7_aggregate.py.
"""
import argparse
import collections
import json
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, 'ljdiff'))
import corpus_run  # noqa: E402
import ljdiff as LD  # noqa: E402
import ljconv  # noqa: E402
from phase5_aggregate import deepcopy_exempt_at_line, SLOTNAME_SITE  # noqa: E402

_HANG_FH = None  # keep alive: arm_faulthandler's handle must not be gc'd


def emit(*cols):
    print('\t'.join(str(c) for c in cols))
    sys.stdout.flush()


def jsonify_counter(c):
    return {k: v for k, v in sorted(c.items())}


def jsonify_fn(r):
    return {
        'seeds': r['seeds'],
        'verdicts': jsonify_counter(r['verdicts']),
        'labels': jsonify_counter(r['labels']),
        'cov_orig': len(r['sig_orig']),
        'cov_side': len(r['sig_side']),
        'sites': jsonify_counter(r['sites']),
        'fatal': jsonify_counter(r['fatal']),
        'clobber': jsonify_counter(r['clobber']),
        'divergent_seeds': [list(t) for t in r['divergent']],
        'first': list(r['first']) if r['first'] else None,
        'mean_seed_sim': (sum(r['seed_sim'].values()) / len(r['seed_sim'])) if r['seed_sim'] else None,
    }


def jsonify_res(res, main_sites_raw):
    # diff_traces()'s own res['main'] carries verdict/labels/first only -- the
    # raw per-site POISON_READ/UPVALUE_READ marks for the main chunk live only
    # in the raw trace JSON (tb['main']['sites']), never in diff_traces' own
    # result. phase5_aggregate.py re-opened tmp/ljdiff_<flat>.rt.json for this;
    # here the caller reads that same raw trace once, before this file's tmp/
    # is wiped, and hands the list straight in.
    main = None
    if res.get('main'):
        main = {'verdict': res['main']['verdict'], 'labels': res['main']['labels'],
                'sites': jsonify_counter(collections.Counter(main_sites_raw)) if main_sites_raw else {}}
    return {
        'side': res['side'], 'seeds': res['seeds'], 'other_label': res['other_label'],
        'only_orig': res['only_orig'], 'only_side': res['only_side'], 'load_error': list(res['load_error']),
        'main_values': res['main_values'],
        'main': main,
        'fns': {fn: jsonify_fn(r) for fn, r in res['fns'].items()},
        'static_available': res['static_available'],
        'static': res['static'],
        'dynamic_not_static': res['dynamic_not_static'],
        'lint': res['lint'],
    }


def count_unexplained(results, rt_src_text, main_sites_raw):
    """rt-side dropped-assignment sites, dynamic + static, not covered by the
    known deepcopy-hoist exemption. See phase5_aggregate.py's module docstring
    for the exemption's exact shape and why it must never be assumed."""
    unexplained = []
    r = results.get('rt')
    if not r:
        return unexplained
    for fn, fr in r['fns'].items():
        for site, n in fr['sites'].items():
            if not SLOTNAME_SITE.match(site):
                continue
            line = int(site.partition('@L')[2] or 0)
            name = site.split('@')[0]
            if not deepcopy_exempt_at_line(rt_src_text, line, name):
                unexplained.append((fn, site, n))
    for site in main_sites_raw:
        if not SLOTNAME_SITE.match(site):
            continue
        line = int(site.partition('@L')[2] or 0)
        name = site.split('@')[0]
        if not deepcopy_exempt_at_line(rt_src_text, line, name):
            unexplained.append(('<main>', site, 1))
    for row in r.get('static', []):
        if row['tags']:
            continue
        if not deepcopy_exempt_at_line(rt_src_text, row['line'], row['name']):
            unexplained.append((row['fn'], '%s@L%d(static)' % (row['name'], row['line']), 0))
    return unexplained


def main(argv):
    global _HANG_FH
    ap = argparse.ArgumentParser()
    ap.add_argument('lua_path')
    ap.add_argument('--budget', type=float, default=600)
    ap.add_argument('--outdir', required=True)
    ap.add_argument('--seeds', type=int, default=32)
    ap.add_argument('--hang-dir')
    ap.add_argument('--src-outdir', required=True)
    args = ap.parse_args(argv)

    os.makedirs(args.outdir, exist_ok=True)
    os.makedirs(args.src_outdir, exist_ok=True)
    hang_dir = args.hang_dir or args.outdir
    os.makedirs(hang_dir, exist_ok=True)

    d = os.path.basename(os.path.dirname(args.lua_path))
    n = os.path.basename(args.lua_path)
    flat = d + '__' + n
    _HANG_FH = corpus_run.arm_faulthandler(args.budget, os.path.join(hang_dir, flat + '.hang.txt'))

    LD.use_fixed_ljd(True)
    try:
        table = ljconv.load_table(ljconv.TABLE_TSV)
        LD.clean([flat])
        flat_built, sides, srcs = LD.build(args.lua_path, table, [], twin=True)
        built = [(flat_built, sides, srcs)]

        # Copy the fixed decompile out BEFORE learn/trace, so a downstream
        # failure still leaves it behind for later phases/ports to read.
        rt_src_path = srcs['rt']
        rt_src_text = open(rt_src_path, encoding='utf-8', errors='replace').read()
        dest = os.path.join(args.src_outdir, os.path.splitext(n)[0] + '.src.lua')
        shutil.copyfile(rt_src_path, dest)

        log = []
        rounds = LD.learn(built, args.seeds, log)
        trace_res = LD.trace(built, args.seeds, log, opts='lines')
        harness_total = sum(t['harness'] for (_, _, t, _, _) in trace_res.values())
        results = LD.diff_files(flat_built, sides, srcs)
        rt_trace_path = LD.tmp(flat_built, 'rt.json')
        main_sites_raw = json.load(open(rt_trace_path, encoding='utf-8')).get('main', {}).get('sites', []) \
            if os.path.exists(rt_trace_path) else []
    except LD.StepError as e:
        emit('STEP_ERROR', 0, 0, 0, 0, 0, str(e)[:300].replace('\t', ' ').replace('\n', ' | '))
        LD.clean([flat])
        return 0
    except subprocess.TimeoutExpired as e:
        emit('TIMEOUT-INTERNAL', 0, 0, 0, 0, 0, str(e)[:300])
        LD.clean([flat])
        return 0

    def tot(side):
        r = results.get(side)
        if not r:
            return collections.Counter()
        c = collections.Counter()
        for fr in r['fns'].values():
            c.update(fr['verdicts'])
        return c

    rt_tot = tot('rt')
    n_fns = len(results.get('rt', {}).get('fns', {})) if 'rt' in results else 0
    unexplained = count_unexplained(results, rt_src_text, main_sites_raw)

    out = {
        'kom_dir': d, 'kom_name': n, 'flat': flat_built,
        'rounds': rounds, 'harness_total': harness_total, 'seeds': args.seeds,
        'results': {side: jsonify_res(r, main_sites_raw if side == 'rt' else []) for side, r in results.items()},
        'unexplained_dropped_sites': ['%s::%s(seeds=%d)' % u for u in unexplained],
    }
    out_path = os.path.join(args.outdir, flat + '.json')
    with open(out_path, 'w', encoding='utf-8') as fh:
        json.dump(out, fh, indent=1, sort_keys=True)

    LD.clean([flat])  # bound disk use: 3,105 files' worth of tmp/ljdiff_*.* would not fit comfortably

    emit('ok', harness_total,
         rt_tot.get('dropped-assignment', 0), sum(v for k, v in rt_tot.items() if k not in ('pass', 'dropped-assignment')),
         len(unexplained), n_fns, out_path)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
