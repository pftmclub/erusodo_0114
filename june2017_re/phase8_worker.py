"""corpus_run.py WORKER for Phase 8 (RANOX_LUA_DECOMPILE_FIX_PLAN.md): the same
June-vs-FIXED-round-trip comparison as phase7_worker.py, extended to the
~4,200 files outside data037_kom, with two additions:

  --fast      Skip LD.learn() entirely. Only LD.learn() ever calls
              save_types() (ljdiff.py:240, checked before this flag was
              added), so a --fast run never writes june2017_re/ljdiff/
              types.tsv -- it only READS whatever is already there, which is
              why a batch of --fast workers can safely run with --workers >
              1 while a learning batch cannot (phase5_aggregate.py's
              concurrent-save_types() race). --fast is intended for files
              with zero nested prototypes (decompile_corpus.tsv's
              protos_complete == 1): such a file has no entry points
              (ljdiff.lua's entry_points() walks only functions reachable
              from main-chunk globals), so trace()'s per-seed per-entry-point
              loop is already a no-op with or without --fast -- confirmed by
              reading ljdiff.lua directly (run_main() executes exactly once
              regardless of MODE or seed count; the `for _, ep in
              ipairs(eps) do for seed = 1, NSEEDS do` loop below it never
              iterates when eps is empty). The ONLY thing --fast removes is
              LD.learn()'s own extra main-chunk execution(s) and its
              static_learn() type-mining pass -- see "Phase 8 result" in the
              plan for the 20-file check confirming this doesn't change any
              verdict.
  --decompile-timeout SECS
              Threaded into LD.build() -> LD.decompile(), which used to
              hard-code 600s for the `ljd -f` subprocess (ljdiff.py, fixed
              2026-09-30 to accept this as a parameter, default unchanged at
              600). Only the 14 files over 1MB need more; every other target
              uses the default.

Everything else -- the JSON shape, the src-outdir copy-before-trace
ordering, LD.clean() bounding disk use, the unexplained-dropped-sites
regression check via phase5_aggregate.deepcopy_exempt_at_line() -- is
unchanged from phase7_worker.py. One real difference: Phase 8's target set
spans 20 different kom_dir folders (phase7's was single-folder), so the
fixed decompile is written to <src-outdir>/<kom_dir>/<name>.src.lua, not
<src-outdir>/<name>.src.lua -- phase7_worker.py's flat '--src-outdir IS
data037_kom' convention doesn't generalize.

    python phase8_worker.py <abs_june_lua_path> --budget SECS --outdir DIR
        --src-outdir DIR [--seeds N] [--hang-dir DIR] [--fast]
        [--decompile-timeout SECS]

Prints one TSV line to stdout (same columns as phase7_worker.py, plus a
'fast' flag column and per-stage wall times at the end):
    status  harness_total  rt_dropped  rt_other  rt_unexplained_dropped
    n_fns  out_path  fast  t_build  t_learn  t_trace  t_diff
"""
import argparse
import collections
import json
import os
import shutil
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, 'ljdiff'))
import corpus_run  # noqa: E402
import ljdiff as LD  # noqa: E402
import ljconv  # noqa: E402
from phase5_aggregate import deepcopy_exempt_at_line, SLOTNAME_SITE  # noqa: E402
from phase7_worker import jsonify_res, count_unexplained  # noqa: E402

_HANG_FH = None  # keep alive: arm_faulthandler's handle must not be gc'd


def emit(*cols):
    print('\t'.join(str(c) for c in cols))
    sys.stdout.flush()


def main(argv):
    global _HANG_FH
    ap = argparse.ArgumentParser()
    ap.add_argument('lua_path')
    ap.add_argument('--budget', type=float, default=600)
    ap.add_argument('--outdir', required=True)
    ap.add_argument('--seeds', type=int, default=32)
    ap.add_argument('--hang-dir')
    ap.add_argument('--src-outdir', required=True)
    ap.add_argument('--fast', action='store_true')
    ap.add_argument('--decompile-timeout', type=float, default=600)
    args = ap.parse_args(argv)

    os.makedirs(args.outdir, exist_ok=True)
    hang_dir = args.hang_dir or args.outdir
    os.makedirs(hang_dir, exist_ok=True)

    kom_dir = os.path.basename(os.path.dirname(args.lua_path))
    n = os.path.basename(args.lua_path)
    flat = kom_dir + '__' + n
    src_outdir = os.path.join(args.src_outdir, kom_dir)
    os.makedirs(src_outdir, exist_ok=True)
    _HANG_FH = corpus_run.arm_faulthandler(args.budget, os.path.join(hang_dir, flat + '.hang.txt'))

    LD.use_fixed_ljd(True)
    t_build = t_learn = t_trace = t_diff = 0.0
    try:
        table = ljconv.load_table(ljconv.TABLE_TSV)
        LD.clean([flat])
        t0 = time.time()
        flat_built, sides, srcs = LD.build(args.lua_path, table, [], twin=True,
                                            decompile_timeout=args.decompile_timeout)
        t_build = time.time() - t0
        built = [(flat_built, sides, srcs)]

        # Copy the fixed decompile out BEFORE learn/trace, so a downstream
        # failure still leaves it behind for later phases/ports to read.
        rt_src_path = srcs['rt']
        rt_src_text = open(rt_src_path, encoding='utf-8', errors='replace').read()
        dest = os.path.join(src_outdir, os.path.splitext(n)[0] + '.src.lua')
        shutil.copyfile(rt_src_path, dest)

        log = []
        rounds = 0
        if not args.fast:
            t0 = time.time()
            rounds = LD.learn(built, args.seeds, log)
            t_learn = time.time() - t0
        t0 = time.time()
        trace_res = LD.trace(built, args.seeds, log, opts='lines')
        t_trace = time.time() - t0
        harness_total = sum(t['harness'] for (_, _, t, _, _) in trace_res.values())
        t0 = time.time()
        results = LD.diff_files(flat_built, sides, srcs)
        t_diff = time.time() - t0
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
        'kom_dir': kom_dir, 'kom_name': n, 'flat': flat_built,
        'rounds': rounds, 'harness_total': harness_total, 'seeds': args.seeds,
        'fast': args.fast,
        'timing': {'build': t_build, 'learn': t_learn, 'trace': t_trace, 'diff': t_diff},
        'results': {side: jsonify_res(r, main_sites_raw if side == 'rt' else []) for side, r in results.items()},
        'unexplained_dropped_sites': ['%s::%s(seeds=%d)' % u for u in unexplained],
    }
    out_path = os.path.join(args.outdir, flat + '.json')
    with open(out_path, 'w', encoding='utf-8') as fh:
        json.dump(out, fh, indent=1, sort_keys=True)

    LD.clean([flat])  # bound disk use over ~4,200 files' worth of tmp/ljdiff_*.*

    emit('ok', harness_total,
         rt_tot.get('dropped-assignment', 0), sum(v for k, v in rt_tot.items() if k not in ('pass', 'dropped-assignment')),
         len(unexplained), n_fns, out_path, int(args.fast),
         '%.2f' % t_build, '%.2f' % t_learn, '%.2f' % t_trace, '%.2f' % t_diff)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
