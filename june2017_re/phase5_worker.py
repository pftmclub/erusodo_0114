"""corpus_run.py WORKER for Phase 5 (RANOX_LUA_DECOMPILE_FIX_PLAN.md): for one
June original that a march2014/ port file maps to (see phase5_map.py), run
ljdiff's own pipeline (as a library -- ljdiff.lua/elstools/ljd are all invoked
by ljdiff.py as subprocesses already, so there is no module-global-state reason
to isolate this in its own interpreter; corpus_run.py's per-file process is
here for OS-level timeout/resume/crash-isolation, not for that) with TWO
extra sides against the June original:
    rt    the Phase 4 FIXED decompile (must show zero dropped-assignment)
    port  the march2014/ port file as it stands (the repair target)

    python phase5_worker.py <abs_june_lua_path> --budget SECS --outdir DIR
        --map MAP_TSV [--seeds N] [--hang-dir DIR]

MAP_TSV is phase5_files.tsv: port_path, kom_dir, kom_name, map_status,
candidates. This worker only ever receives (dir, name) that phase5_map.py
already resolved to exactly one 'mapped' port file (phase5_targets.tsv), so a
lookup miss here is itself a worker-reported error, not a silent skip.

Writes <outdir>/<flat>.json: the jsonified diff_traces() result for each of
'rt' and 'port' (Counters/sets/tuples flattened to plain JSON), plus the raw
trace totals and the port-side lint hits (diff_traces's own lint cross-check,
run on the port .src.lua text).

Prints one TSV line to stdout:
    status  harness_total  rt_dropped  rt_other  port_dropped  port_other
    port_clobber  n_fns  detail

status: ok / STEP_ERROR / TIMEOUT-INTERNAL / CRASH (the last two are caught
here so corpus_run.py's own CRASH bucket stays reserved for process death).
rt_dropped/rt_other etc. are summed over every function's verdict counts
(seed-counts, not function-counts) -- see summary_row() in ljdiff.py.
"""
import argparse
import collections
import csv
import json
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, 'ljdiff'))
import corpus_run  # noqa: E402
import ljdiff as LD  # noqa: E402
import ljconv  # noqa: E402

_HANG_FH = None  # keep alive: arm_faulthandler's handle must not be gc'd


def emit(*cols):
    print('\t'.join(str(c) for c in cols))
    sys.stdout.flush()


def load_map(map_tsv):
    """(kom_dir, kom_name) -> port_path (relative to TRUNK), 'mapped' rows only."""
    out = {}
    with open(map_tsv, encoding='utf-8') as fh:
        r = csv.DictReader(fh, delimiter='\t')
        for row in r:
            if row['map_status'] == 'mapped':
                out[(row['kom_dir'], row['kom_name'])] = row['port_path']
    return out


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


def jsonify_res(res):
    out = {
        'side': res['side'], 'seeds': res['seeds'], 'other_label': res['other_label'],
        'only_orig': res['only_orig'], 'only_side': res['only_side'], 'load_error': list(res['load_error']),
        'main_values': res['main_values'],
        'main': ({'verdict': res['main']['verdict'], 'labels': res['main']['labels']} if res.get('main') else None),
        'fns': {fn: jsonify_fn(r) for fn, r in res['fns'].items()},
        'static_available': res['static_available'],
        'static': res['static'],
        'dynamic_not_static': res['dynamic_not_static'],
        'lint': res['lint'],
    }
    return out


def main(argv):
    global _HANG_FH
    ap = argparse.ArgumentParser()
    ap.add_argument('lua_path')
    ap.add_argument('--budget', type=float, default=600)
    ap.add_argument('--outdir', required=True)
    ap.add_argument('--map', required=True)
    ap.add_argument('--seeds', type=int, default=32)
    ap.add_argument('--hang-dir')
    args = ap.parse_args(argv)

    os.makedirs(args.outdir, exist_ok=True)
    hang_dir = args.hang_dir or args.outdir
    os.makedirs(hang_dir, exist_ok=True)

    d = os.path.basename(os.path.dirname(args.lua_path))
    n = os.path.basename(args.lua_path)
    flat = d + '__' + n
    _HANG_FH = corpus_run.arm_faulthandler(args.budget, os.path.join(hang_dir, flat + '.hang.txt'))

    port_map = load_map(args.map)
    port_rel = port_map.get((d, n))
    if not port_rel:
        emit('STEP_ERROR', 0, 0, 0, 0, 0, 0, 0, 'no mapped port file for %s/%s in %s' % (d, n, args.map))
        return 0
    port_abs = os.path.join(TRUNK, port_rel.replace('/', os.sep))
    if not os.path.isfile(port_abs):
        emit('STEP_ERROR', 0, 0, 0, 0, 0, 0, 0, 'mapped port file missing on disk: %s' % port_abs)
        return 0

    LD.use_fixed_ljd(True)
    try:
        table = ljconv.load_table(ljconv.TABLE_TSV)
        LD.clean([flat])
        flat_built, sides, srcs = LD.build(args.lua_path, table, [('port', port_abs)], twin=True)
        built = [(flat_built, sides, srcs)]
        log = []
        rounds = LD.learn(built, args.seeds, log)
        trace_res = LD.trace(built, args.seeds, log, opts='lines')
        harness_total = sum(t['harness'] for (_, _, t, _, _) in trace_res.values())
        results = LD.diff_files(flat_built, sides, srcs)
    except LD.StepError as e:
        emit('STEP_ERROR', 0, 0, 0, 0, 0, 0, 0, str(e)[:300].replace('\t', ' ').replace('\n', ' | '))
        return 0
    except subprocess.TimeoutExpired as e:
        emit('TIMEOUT-INTERNAL', 0, 0, 0, 0, 0, 0, 0, str(e)[:300])
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
    port_tot = tot('port')
    port_clobber = sum(sum(r['clobber'].values()) for r in results.get('port', {}).get('fns', {}).values()) \
        if 'port' in results else 0
    n_fns = len(results.get('rt', {}).get('fns', {})) if 'rt' in results else 0

    out = {
        'port_path': port_rel, 'kom_dir': d, 'kom_name': n, 'flat': flat_built,
        'rounds': rounds, 'harness_total': harness_total, 'seeds': args.seeds,
        'results': {side: jsonify_res(r) for side, r in results.items()},
    }
    out_path = os.path.join(args.outdir, flat + '.json')
    with open(out_path, 'w', encoding='utf-8') as fh:
        json.dump(out, fh, indent=1, sort_keys=True)

    emit('ok', harness_total,
         rt_tot.get('dropped-assignment', 0), sum(v for k, v in rt_tot.items() if k not in ('pass', 'dropped-assignment')),
         port_tot.get('dropped-assignment', 0), sum(v for k, v in port_tot.items() if k not in ('pass', 'dropped-assignment')),
         port_clobber, n_fns, out_path)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
