"""ljdiff orchestrator (Phase 2a of RANOX_LUA_DECOMPILE_FIX_PLAN.md).

Builds, for each June file, the inputs the runtime (ljdiff.lua) compares:
  orig   June 0x82 -> mar-2014 0x81 via ljconv.transcode (the real bytecode)
  rt     June -> ljconv.transcode_stock -> unmodified vendored ljd (Phase 0's
         no-sibling decompile path) -> `elstools compile-batch -v mar-2014` on a
         tmp/ copy (the port pipeline)
  NAME   optional: --extra NAME=PATH.src.lua, compiled the same way (a
         march2014/ port file, or `git show 857dfd7^:...` saved to a file)
then runs ljdiff.lua on them in two phases:
  learn  on the ORIGINAL side only, over every input file, repeated until a
         round learns nothing; new keys are merged into types.tsv
  trace  every side with types.tsv frozen; one JSON trace per side per file

    python ljdiff.py                                   # Kenta + MapData + SQUARE_Unit
    python ljdiff.py data037_kom/RAGE_DRYADKENTA.lua   # paths under 14JUN17/data, or absolute
    python ljdiff.py --seeds 8 --extra pre=tmp/kenta_prefix.src.lua data037_kom/RAGE_DRYADKENTA.lua
    python ljdiff.py --twice                           # clean, run, clean, run; byte-compare traces

Scratch: <Trunk>/tmp/ljdiff_* (deleted and rebuilt on every run; don't give
other files that prefix). Traces: tmp/ljdiff_<dir>__<name>.<side>.json.
Exit status 0 iff every trace has zero harness errors (and, with --twice,
both runs produced byte-identical traces). Diffing the traces is Phase 2b.
"""
import argparse
import glob
import hashlib
import json
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
RE = os.path.dirname(HERE)
TRUNK = os.path.dirname(RE)
TMP = os.path.join(TRUNK, 'tmp')
PREFIX = 'ljdiff_'
JUNE_DATA = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data'
LUAJIT_DIR = os.path.join(TRUNK, 'elstools', 'vendor', 'luajit')
LUAJIT = os.path.join(LUAJIT_DIR, 'luajit.exe')
LJD = os.path.join(TRUNK, 'elstools', 'vendor', 'ljd', 'main.py')
ELSTOOLS = os.path.join(TRUNK, 'elstools', 'elstools.exe')
RUNTIME = os.path.join(HERE, 'ljdiff.lua')
TYPES = os.path.join(HERE, 'types.tsv')
REENTRANT = os.path.join(HERE, 'reentrant.tsv')
# Kenta (the calibration file), MapData (a data036 registry with a known defect),
# SQUARE_Unit (the only June file where simulated engine re-entry fires)
DEFAULT_FILES = ['data037_kom/RAGE_DRYADKENTA.lua', 'data036_kom/MapData.lua', 'data038_kom/SQUARE_Unit.lua']
MAX_LEARN_ROUNDS = 10

sys.path.insert(0, RE)
import ljconv  # noqa: E402

TYPES_HEADER = ('# ljdiff type oracle: learned on the ORIGINAL side only by ljdiff.py, then frozen\n'
                '# and shared by both sides. Generated - do not hand-edit (delete a row to relearn it).\n'
                '# kind: ret (what calling member NAME returns) / field / arg (FN#I) / global.\n'
                '# evidence: arith-* and compare-both are exact; compare is the one node-vs-number\n'
                '# operand pair in the erroring frame; compare-ambiguous picked the newest of several;\n'
                '# conflict = an ambiguous guess later used as an object, pinned back to object.\n'
                'kind\tname\ttype\tevidence\tfirst_seen\n')


class StepError(Exception):
    pass


def run(cmd, cwd, timeout):
    # subprocess.run kills the child by its own handle (PID) on timeout.
    p = subprocess.run(cmd, cwd=cwd, capture_output=True, timeout=timeout)
    return p.returncode, p.stdout.decode('utf-8', 'replace'), p.stderr.decode('utf-8', 'replace')


def tmp(flat, kind):
    return os.path.join(TMP, '%s%s.%s' % (PREFIX, flat, kind))


def compile_mar(src, out):
    """copy src to out and compile it in place with compile-batch -v mar-2014."""
    shutil.copyfile(src, out)
    rc, so, se = run([ELSTOOLS, 'compile-batch', '-q', '-v', 'mar-2014', '-i', out], TRUNK, 300)
    res = None
    for line in so.splitlines():
        try:
            j = json.loads(line)
        except ValueError:
            continue
        if j.get('type') == 'result':
            res = j
    if rc != 0 or not res or res.get('ok') != 1 or res.get('failed'):
        raise StepError('compile-batch failed on %s (rc %d): %s' % (src, rc, (so + se).strip()[-400:]))
    if open(out, 'rb').read(4) != ljconv.MAR_MAGIC:
        raise StepError('compile-batch output for %s is not 0x81 bytecode' % src)


def build(path, table, extras):
    """-> (flat, {side: chunk_path}) for one June file."""
    flat = os.path.basename(os.path.dirname(path)) + '__' + os.path.basename(path)
    d = open(path, 'rb').read()
    if d[:4] != ljconv.JUNE_MAGIC:
        raise StepError('%s: header %s, expected June %s' % (path, d[:4].hex(), ljconv.JUNE_MAGIC.hex()))
    sides = {}
    orig, _, _ = ljconv.transcode(d, table)
    sides['orig'] = tmp(flat, 'orig.luac')
    open(sides['orig'], 'wb').write(orig)

    stock, _, _ = ljconv.transcode_stock(d, table)
    stock_path = tmp(flat, 'stock.luac')
    open(stock_path, 'wb').write(stock)
    src = tmp(flat, 'src.lua')
    rc, so, se = run([sys.executable, LJD, '-f', stock_path, '-o', src], os.path.dirname(LJD), 600)
    if rc != 0 or not os.path.exists(src):
        raise StepError('ljd failed on %s (rc %d): %s' % (path, rc, (se.strip().splitlines() or [''])[-1]))
    sides['rt'] = tmp(flat, 'rt.luac')
    compile_mar(src, sides['rt'])

    for name, epath in extras:
        sides[name] = tmp(flat, name + '.luac')
        compile_mar(epath, sides[name])
    return flat, sides


def load_types():
    rows = {}
    if os.path.exists(TYPES):
        for line in open(TYPES, encoding='utf-8'):
            if line.startswith('#') or line.startswith('kind\t') or not line.strip():
                continue
            c = line.rstrip('\n').split('\t')
            rows[(c[0], c[1])] = c[2:5]
    return rows


def save_types(rows):
    with open(TYPES, 'w', encoding='utf-8', newline='\n') as f:
        f.write(TYPES_HEADER)
        for (kind, name) in sorted(rows):
            f.write('\t'.join([kind, name] + rows[(kind, name)]) + '\n')


def runtime(mode, chunk, out, label, side, seeds, types=TYPES, reentrant=REENTRANT):
    rc, so, se = run([LUAJIT, RUNTIME, mode, chunk, out, types, reentrant, str(seeds), label, side],
                     LUAJIT_DIR, 1800)
    if rc != 0:
        raise StepError('ljdiff.lua %s failed on %s/%s (rc %d): %s' % (
            mode, label, side, rc, (so + se).strip()[-600:]))
    return so.strip()


# --selftest: selftest/selftest.lua is compiled straight with luajit -b and run
# through learn and trace with its own types file and reentrant.tsv (never the
# shared ones). Each entry must appear somewhere in the named output.
SELFTEST_DIR = os.path.join(HERE, 'selftest')
SELFTEST_EXPECT = [
    ('learned', 'ret\tIsReady\tboolean\tstatic-iseqp'),
    ('learned', 'ret\tGetLevel\tnumber\tcompare\t'),
    ('trace', 'CLOBBER outer_had inner slot1 had'),
    ('trace', 'CLOBBER outer_unset inner slot1 unset'),
    ('trace', 'REENTER RunCallback -> cb'),
    ('trace', 'CLOBBER user cb slot3 had'),
    ('trace', 'POISON_READ slot9'),
    ('trace', '-> true'),                 # seeded __eq said yes on some seed
    ('trace', '"outcome":"RET \\"match\\""'),
    ('trace', '"CALLBACKS[1].run/1 nested"'),
    ('trace', '"fn":"CALLBACKS[1].run","seed":1,"arity":1,"args":"<a0>","outcome":"RET 42"'),
    # state-table re-entry: JUMP's START, then (old state known) JUMP's END and WALK's START
    ('trace', '"REENTER ChangeState -> start_jump"'),
    ('trace', '"REENTER ChangeState -> end_jump"'),
    ('trace', '"CALL a3:Landed() -> '),
    ('trace', '"REENTER ChangeState -> start_walk"'),
    ('trace', '"CLOBBER square start_walk slot4 had"'),
    ('trace', '"unsim":["OpenDialog"]'),
    # main_values: post-main global values, capped text plus full-value hash
    ('trace', '"WALK #'),
    ('trace', '= {[\\"FUNC_STATE_START\\"]=\\"start_walk\\"}"'),
    ('trace', '"DEEP #'),
]
SELFTEST_ABSENT = [
    ('learned', 'GetOther'),              # the newer, non-compared node must not be retyped
    ('trace', 'HARNESS_ERROR'),
    ('trace', 'NOT_A_STATE (not'),        # a string naming no state table: silent, not an error
    ('trace', 'REENTER Deferred'),        # deferred rows never re-enter
]


def selftest(seeds):
    clean()
    chunk = tmp('selftest', 'luac')
    rc, so, se = run([LUAJIT, '-b', os.path.join(SELFTEST_DIR, 'selftest.lua'), chunk], LUAJIT_DIR, 60)
    if rc != 0:
        raise StepError('luajit -b failed on selftest.lua: %s' % (so + se).strip())
    types = tmp('selftest', 'types.tsv')
    reentrant = os.path.join(SELFTEST_DIR, 'reentrant.tsv')
    learned_path = tmp('selftest', 'learned.tsv')
    open(types, 'w').close()
    learned = ''
    for _ in range(MAX_LEARN_ROUNDS):
        runtime('learn', chunk, learned_path, 'selftest', 'orig', seeds, types, reentrant)
        new = open(learned_path, encoding='utf-8').read()
        if not new:
            break
        learned += new
        with open(types, 'a', encoding='utf-8', newline='\n') as f:
            f.write(new)
    trace_path = tmp('selftest', 'json')
    summary = runtime('trace', chunk, trace_path, 'selftest', 'orig', seeds, types, reentrant)
    text = {'learned': learned, 'trace': open(trace_path, encoding='utf-8').read()}
    fails = ['missing in %s: %r' % (w, s) for w, s in SELFTEST_EXPECT if s not in text[w]]
    fails += ['unexpected in %s: %r' % (w, s) for w, s in SELFTEST_ABSENT if s in text[w]]
    print('selftest trace: %s' % summary)
    for f in fails:
        print('  FAIL ' + f)
    print('selftest: %s (%d checks)' % ('OK' if not fails else 'FAILED', len(SELFTEST_EXPECT) + len(SELFTEST_ABSENT)))
    return not fails


def learn(built, seeds, log):
    """Original side only, all files, until a round learns nothing."""
    for rnd in range(1, MAX_LEARN_ROUNDS + 1):
        rows = load_types()
        new = 0
        for flat, sides in built:
            out = tmp(flat, 'learned.tsv')
            summary = runtime('learn', sides['orig'], out, flat, 'orig', seeds)
            log.append('learn r%d %-40s %s' % (rnd, flat, summary))
            if ' harness 0 ' not in summary + ' ':
                raise StepError('harness error while learning %s: %s' % (flat, summary))
            for line in open(out, encoding='utf-8'):
                c = line.rstrip('\n').split('\t')
                if len(c) < 5:
                    continue
                key = (c[0], c[1])
                if key not in rows:
                    rows[key] = c[2:5]
                    new += 1
                elif c[3] == 'conflict' and rows[key][0] != 'object':
                    rows[key] = c[2:5]
                    new += 1
            save_types(rows)       # the next file in this round sees it
        log.append('learn r%d: %d new type(s), %d total' % (rnd, new, len(rows)))
        if new == 0:
            return rnd
    raise StepError('type learning did not converge in %d rounds' % MAX_LEARN_ROUNDS)


def trace(built, seeds, log):
    """-> {trace path: totals dict}"""
    res = {}
    for flat, sides in built:
        for side, chunk in sides.items():
            out = tmp(flat, side + '.json')
            summary = runtime('trace', chunk, out, flat, side, seeds)
            j = json.load(open(out, encoding='utf-8'))
            res[out] = (flat, side, j['totals'], len(j.get('calls', [])), j.get('load_error'))
            log.append('trace %-40s %-5s %s' % (flat, side, summary))
    return res


def clean():
    for p in glob.glob(os.path.join(TMP, PREFIX + '*')):
        os.remove(p)


def traces_digest():
    return {os.path.basename(p): hashlib.sha1(open(p, 'rb').read()).hexdigest()
            for p in sorted(glob.glob(os.path.join(TMP, PREFIX + '*.json')))}


def resolve(f):
    p = f if os.path.isabs(f) else os.path.join(JUNE_DATA, f.replace('/', os.sep))
    if not os.path.isfile(p):
        raise SystemExit('no such June file: %s' % p)
    return p


def one_run(files, extras, seeds):
    log = []
    clean()
    table = ljconv.load_table(ljconv.TABLE_TSV)
    built = [build(p, table, extras) for p in files]
    rounds = learn(built, seeds, log)
    res = trace(built, seeds, log)
    return log, rounds, res


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument('files', nargs='*')
    ap.add_argument('--seeds', type=int, default=32)
    ap.add_argument('--extra', action='append', default=[], metavar='NAME=PATH',
                    help='also trace this .src.lua, compiled with compile-batch -v mar-2014')
    ap.add_argument('--twice', action='store_true')
    ap.add_argument('--selftest', action='store_true',
                    help='only run selftest/selftest.lua (CLOBBER, re-entry, poison, learners)')
    args = ap.parse_args(argv)
    os.makedirs(TMP, exist_ok=True)
    if args.selftest:
        try:
            return 0 if selftest(args.seeds) else 1
        except (StepError, subprocess.TimeoutExpired) as e:
            print('STEP_ERROR %s' % e)
            return 2
    files = [resolve(f) for f in (args.files or DEFAULT_FILES)]
    extras = []
    for e in args.extra:
        name, _, p = e.partition('=')
        if not name or not p or name in ('orig', 'rt') or not os.path.isfile(p):
            raise SystemExit('--extra wants NAME=PATH with an existing file and a new side name: %r' % e)
        extras.append((name, os.path.abspath(p)))
    if not os.path.exists(REENTRANT):
        print('warning: %s missing - engine re-entry is not simulated' % REENTRANT)

    try:
        log, rounds, res = one_run(files, extras, args.seeds)
    except (StepError, subprocess.TimeoutExpired) as e:
        print('STEP_ERROR %s' % e)
        return 2
    print('\n'.join(log))
    print('\nlearning converged after %d round(s); types.tsv has %d row(s)' % (rounds, len(load_types())))
    print('%-44s %-5s %6s %7s %6s %7s %7s %6s %7s %5s %5s' % (
        'file', 'side', 'calls', 'harness', 'poison', 'upvalue', 'clobber', 'budget', 'reenter', 'unsim', 'trunc'))
    harness = 0
    for out, (flat, side, t, ncalls, lerr) in sorted(res.items()):
        harness += t['harness']
        print('%-44s %-5s %6d %7d %6d %7d %7d %6d %7d %5d %5d%s' % (
            flat, side, ncalls, t['harness'], t['poison'], t['upvalue'], t['clobber'], t['budget'],
            t['reenter'], t['unsim'], t['truncated'], ('  LOAD_ERROR ' + lerr) if lerr else ''))
    ok = harness == 0
    if args.twice:
        d1 = traces_digest()
        try:
            _, _, _ = one_run(files, extras, args.seeds)
        except (StepError, subprocess.TimeoutExpired) as e:
            print('STEP_ERROR (second run) %s' % e)
            return 2
        d2 = traces_digest()
        same = d1 == d2
        diffs = sorted(k for k in set(d1) | set(d2) if d1.get(k) != d2.get(k))
        print('\ndeterminism: %s (%d trace(s) compared%s)' % (
            'IDENTICAL' if same else 'DIFFERENT', len(d1), '' if same else '; differing: ' + ', '.join(diffs)))
        ok = ok and same
    print('\nresult: %s' % ('OK' if ok else 'HARNESS ERRORS' if harness else 'NONDETERMINISTIC'))
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
