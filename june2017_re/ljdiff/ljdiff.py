"""ljdiff orchestrator (Phases 2a and 2b of RANOX_LUA_DECOMPILE_FIX_PLAN.md).

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

Every compiled side (rt and extras) is traced through its `luajit -bg` twin:
the same source compiled with line info. compile-batch's output is exactly
`luajit -b`'s, and twin_check() proves per prototype that the twin's header,
bytecode, upvalues and constants are byte-identical to it, so the twin runs the
same code and only adds source lines to POISON_READ and the static pass.
--no-twin traces the compile-batch output itself (no lines, no static pass).

    python ljdiff.py                                   # Kenta + MapData + SQUARE_Unit
    python ljdiff.py data037_kom/RAGE_DRYADKENTA.lua   # paths under 14JUN17/data, or absolute
    python ljdiff.py --seeds 8 --extra pre=tmp/kenta_prefix.src.lua data037_kom/RAGE_DRYADKENTA.lua
    python ljdiff.py --twice                           # clean, run, clean, run; byte-compare traces
    python ljdiff.py --diff [--lines] FILES            # run, then diff orig against every other side
    python ljdiff.py diff A.json B.json [--src B.src.lua]   # diff two existing traces
    python ljdiff.py fixloop FILE --fixes FIXES.json   # the fix-rerun loop (Phase 2b)
    python ljdiff.py --fixed ...                       # rt decompiled by ../ljd_fixed.py (Phase 4 fix)

Scratch: <Trunk>/tmp/ljdiff_<dir>__<name>.* (a run deletes and rebuilds only the
files of the June files it was given; don't give other files that prefix).
Traces: tmp/ljdiff_<dir>__<name>.<side>.json; diff reports next to them as
.<side>.diff.txt. Exit status 0 iff every trace has zero harness errors (and,
with --twice, both runs produced byte-identical traces).

Diff verdicts, per (function, seed), strongest first (see classify()):
  harness             either side has a harness error, or the ORIGINAL read an
                      unknown name (the runtime, not the decompile, is wrong)
  dropped-assignment  the side read a slotN it never wrote in this call
                      (POISON_READ), or one only the main chunk wrote (SHADOW_READ)
  clobber             the side read a slotN a nested file-function frame wrote
  upvalue             the side read a uvN (ljd's name for a captured upvalue)
  reorder             same events as a multiset, same outcome, different order
  other-ljd / other   any other difference (other-ljd for rt, the plain
                      decompile; other for an --extra, which may be a port edit)
  pass                identical events and outcome
Per file, the static must-define rows (ljdiff.lua) are matched against every
call's read sites: confirmed (some seed read it unwritten), static-only (none
did; "executed" if the line ran on a safe path, "unreached" if it never ran -
the latter needs --lines to tell apart).
"""
import argparse
import collections
import difflib
import glob
import hashlib
import json
import os
import re
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
LJD_FIXED = os.path.join(RE, 'ljd_fixed.py')  # same CLI as main.py, with the Phase 4 slotworks fix
# --march (LUA_SOURCE_PHASES.md L2): also accept March 0x81 files. Their orig side is the shipped bytecode itself and the
# rt side is decompiled from it with the header swapped to stock (no ljconv). Off by default: the June-only refusal stays.
ACCEPT_MARCH = False
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
import lint_decompile_slots  # noqa: E402

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


def flat_of(path):
    return os.path.basename(os.path.dirname(path)) + '__' + os.path.basename(path)


# ---------------------------------------------------------------- building

def protos(d):
    """(dump flags, [per prototype: (flags/numparams/framesize/sizeuv bytes,
    sizekgc, sizekn, sizebc, bytecode+upvalues+constants bytes)]) of a 0x81
    dump, debug info left out."""
    flags, p = ljconv.uleb(d, 4)
    if not flags & ljconv.BCDUMP_F_STRIP:
        n, p = ljconv.uleb(d, p)
        p += n
    out = []
    while True:
        plen, p = ljconv.uleb(d, p)
        if plen == 0:
            break
        end = p + plen
        q = p + 4
        sizekgc, q = ljconv.uleb(d, q)
        sizekn, q = ljconv.uleb(d, q)
        sizebc, q = ljconv.uleb(d, q)
        sizedbg = 0
        if not flags & ljconv.BCDUMP_F_STRIP:
            sizedbg, q = ljconv.uleb(d, q)
            if sizedbg:
                _, q = ljconv.uleb(d, q)
                _, q = ljconv.uleb(d, q)
        out.append((d[p:p + 4], sizekgc, sizekn, sizebc, d[q:end - sizedbg]))
        p = end
    if p != len(d):
        raise StepError('%d trailing byte(s) after the last prototype' % (len(d) - p))
    return flags, out


def twin_check(stripped, twin):
    """The -bg twin must be the stripped chunk plus debug info, nothing else."""
    a, b = open(stripped, 'rb').read(), open(twin, 'rb').read()
    if a[:4] != ljconv.MAR_MAGIC or b[:4] != ljconv.MAR_MAGIC:
        raise StepError('twin check: %s / %s not both 0x81' % (stripped, twin))
    fa, pa = protos(a)
    fb, pb = protos(b)
    if not fa & ljconv.BCDUMP_F_STRIP or fb & ljconv.BCDUMP_F_STRIP:
        raise StepError('twin check: expected a stripped chunk and a -bg twin')
    if len(pa) != len(pb):
        raise StepError('twin check: %d vs %d prototypes' % (len(pa), len(pb)))
    for i, (x, y) in enumerate(zip(pa, pb)):
        if x != y:
            raise StepError('twin check: prototype %d of %s differs from its -bg twin' % (i, stripped))
    return len(pa)


def compile_mar(src, out, twin=True):
    """copy src to out and compile it in place with compile-batch -v mar-2014.
    With twin, also compile src with `luajit -bg` next to it, prove it is the
    same code, and return the twin's path (else out)."""
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
    if not twin:
        return out
    g = out[:-len('.luac')] + '.g.luac'
    rc, so, se = run([LUAJIT, '-bg', os.path.abspath(src), g], LUAJIT_DIR, 300)
    if rc != 0:
        raise StepError('luajit -bg failed on %s: %s' % (src, (so + se).strip()[-400:]))
    twin_check(out, g)
    return g


def decompile(path, table, flat, decompile_timeout=600):
    """June file -> tmp ljd source (Phase 0's no-sibling path). -> src path"""
    d = open(path, 'rb').read()
    if ACCEPT_MARCH and d[:4] == ljconv.MAR_MAGIC:
        stock = ljconv.STD_MAGIC + d[4:]
    else:
        stock, _, _ = ljconv.transcode_stock(d, table)
    stock_path = tmp(flat, 'stock.luac')
    open(stock_path, 'wb').write(stock)
    src = tmp(flat, 'src.lua')
    rc, so, se = run([sys.executable, LJD, '-f', stock_path, '-o', src], os.path.dirname(LJD), decompile_timeout)
    if rc != 0 or not os.path.exists(src):
        raise StepError('ljd failed on %s (rc %d): %s' % (path, rc, (se.strip().splitlines() or [''])[-1]))
    if LJD == LJD_FIXED and 'slotworks fix active' not in se:
        raise StepError('ljd_fixed.py ran on %s without reporting the fix active: %s' % (path, se.strip()[-300:]))
    return src


def build(path, table, extras, twin=True, decompile_timeout=600):
    """-> (flat, {side: chunk_path}, {side: source_path}) for one June file."""
    flat = flat_of(path)
    d = open(path, 'rb').read()
    if ACCEPT_MARCH and d[:4] == ljconv.MAR_MAGIC:
        orig = d
    elif d[:4] != ljconv.JUNE_MAGIC:
        raise StepError('%s: header %s, expected June %s%s' % (path, d[:4].hex(), ljconv.JUNE_MAGIC.hex(),
                                                             '' if ACCEPT_MARCH else ' (March 0x81 needs --march)'))
    else:
        orig, _, _ = ljconv.transcode(d, table)
    sides, srcs = {}, {}
    sides['orig'] = tmp(flat, 'orig.luac')
    open(sides['orig'], 'wb').write(orig)

    src = decompile(path, table, flat, decompile_timeout=decompile_timeout)
    sides['rt'] = compile_mar(src, tmp(flat, 'rt.luac'), twin)
    srcs['rt'] = src

    for name, epath in extras:
        sides[name] = compile_mar(epath, tmp(flat, name + '.luac'), twin)
        srcs[name] = epath
    return flat, sides, srcs


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


def runtime(mode, chunk, out, label, side, seeds, types=TYPES, reentrant=REENTRANT, opts=''):
    rc, so, se = run([LUAJIT, RUNTIME, mode, chunk, out, types, reentrant, str(seeds), label, side, opts],
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
    # Phase 2b: a main-chunk temp read by a function that never wrote it
    ('trace', '"SHADOW_READ slot7"'),
    ('trace', '"GET ENGINE_ENUM.[nil] -> nil"'),
    ('trace', '"fn":"two_flags"'),
    ('trace', '"outcome":"RET true, false"'),
    ('trace', '"outcome":"RET false, true"'),
    # Phase 2b, -bg twin: line-tagged sites, line coverage, the static pass
    ('gtrace', '"POISON_READ slot9 @L'),
    ('gtrace', '"sites":["slot9@L'),
    ('gtrace', '"lines":"'),
    # static rows (tab-separated, JSON-escaped as \u0009)
    ('gtrace', 'poison\\u0009slot9\\u0009L'),
    ('gtrace', 'outer_unset\\u0009slot1\\u0009L'),   # the CLOBBER-unset read, statically
    ('gtrace', 'shadow_main\\u0009slot7\\u0009L'),
    ('gtrace', '\\u0009main-written"'),
    ('gtrace', 'must_loop\\u0009slot2\\u0009L'),     # written on some iterations only
]
SELFTEST_ABSENT = [
    ('learned', 'GetOther'),              # the newer, non-compared node must not be retyped
    ('trace', 'HARNESS_ERROR'),
    ('trace', 'NOT_A_STATE (not'),        # a string naming no state table: silent, not an error
    ('trace', 'REENTER Deferred'),        # deferred rows never re-enter
    ('trace', 'SHADOW_READ slot8'),       # written by the function itself first
    ('gtrace', 'HARNESS_ERROR'),
    ('gtrace', 'must_ok\\u0009slot'),     # written on every path before the read
    ('gtrace', 'must_both\\u0009slot'),   # written on both arms
]


def selftest(seeds):
    clean(['selftest'])
    src = os.path.join(SELFTEST_DIR, 'selftest.lua')
    chunk = tmp('selftest', 'luac')
    rc, so, se = run([LUAJIT, '-b', src, chunk], LUAJIT_DIR, 60)
    if rc != 0:
        raise StepError('luajit -b failed on selftest.lua: %s' % (so + se).strip())
    gchunk = tmp('selftest', 'g.luac')
    rc, so, se = run([LUAJIT, '-bg', src, gchunk], LUAJIT_DIR, 60)
    if rc != 0:
        raise StepError('luajit -bg failed on selftest.lua: %s' % (so + se).strip())
    twin_check(chunk, gchunk)
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
    gtrace_path = tmp('selftest', 'g.json')
    runtime('trace', gchunk, gtrace_path, 'selftest', 'orig', seeds, types, reentrant, 'lines')
    text = {'learned': learned, 'trace': open(trace_path, encoding='utf-8').read(),
            'gtrace': open(gtrace_path, encoding='utf-8').read()}
    fails = ['missing in %s: %r' % (w, s) for w, s in SELFTEST_EXPECT if s not in text[w]]
    fails += ['unexpected in %s: %r' % (w, s) for w, s in SELFTEST_ABSENT if s in text[w]]
    # the twin changes nothing but the line annotations and the new line fields
    a = strip_lines(json.load(open(trace_path, encoding='utf-8')))
    b = strip_lines(json.load(open(gtrace_path, encoding='utf-8')))
    if a != b:
        fails.append('stripped and -bg traces differ beyond line annotations')
    print('selftest trace: %s' % summary)
    for f in fails:
        print('  FAIL ' + f)
    n = len(SELFTEST_EXPECT) + len(SELFTEST_ABSENT) + 1
    print('selftest: %s (%d checks)' % ('OK' if not fails else 'FAILED', n))
    return not fails


def learn(built, seeds, log):
    """Original side only, all files, until a round learns nothing."""
    for rnd in range(1, MAX_LEARN_ROUNDS + 1):
        rows = load_types()
        new = 0
        for flat, sides, _ in built:
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


def trace_one(flat, side, chunk, seeds, log, opts=''):
    out = tmp(flat, side + '.json')
    summary = runtime('trace', chunk, out, flat, side, seeds, opts=opts)
    log.append('trace %-40s %-5s %s' % (flat, side, summary))
    return out


def trace(built, seeds, log, opts=''):
    """-> {trace path: (flat, side, totals, ncalls, load_error)}"""
    res = {}
    for flat, sides, _ in built:
        for side, chunk in sides.items():
            out = trace_one(flat, side, chunk, seeds, log, opts if side != 'orig' else '')
            j = json.load(open(out, encoding='utf-8'))
            res[out] = (flat, side, j['totals'], len(j.get('calls', [])), j.get('load_error'))
    return res


def clean(flats):
    for flat in flats:
        for p in glob.glob(os.path.join(TMP, glob.escape(PREFIX + flat) + '.*')):
            os.remove(p)


def traces_digest(flats):
    out = {}
    for flat in flats:
        for p in sorted(glob.glob(os.path.join(TMP, glob.escape(PREFIX + flat) + '.*.json'))):
            out[os.path.basename(p)] = hashlib.sha1(open(p, 'rb').read()).hexdigest()
    return out


def resolve(f):
    p = f if os.path.isabs(f) else os.path.join(JUNE_DATA, f.replace('/', os.sep))
    if not os.path.isfile(p):
        raise SystemExit('no such June file: %s' % p)
    return p


def one_run(files, extras, seeds, twin=True, opts=''):
    log = []
    clean([flat_of(p) for p in files])
    table = ljconv.load_table(ljconv.TABLE_TSV)
    built = [build(p, table, extras, twin) for p in files]
    rounds = learn(built, seeds, log)
    res = trace(built, seeds, log, opts)
    return log, rounds, res, built


# ---------------------------------------------------------------- diffing

ANNOT = re.compile(r' @L\d+')
NUM = re.compile(r'(?<![\w#.])-?\d+(?:\.\d+)?(?:e[-+]?\d+)?')
MOCKSTR = re.compile(r'"s\d+"')
SITE = re.compile(r'^(\w+)@L(\d+)$')
SLOTNAME = re.compile(r'^slot\d+$')


def strip_lines(j):
    """A trace with everything the -bg twin adds removed: @L annotations, the
    per-call sites/lines fields, the static rows and the chunk name."""
    def call(c):
        c = dict(c)
        c.pop('sites', None)
        c.pop('lines', None)
        c['events'] = [ANNOT.sub('', e) for e in c['events']]
        if c.get('fatal'):
            c['fatal'] = ANNOT.sub('', c['fatal'])
        return c
    j = dict(j)
    j.pop('static', None)
    j.pop('chunk', None)
    if 'main' in j:
        j['main'] = call(j['main'])
    if 'calls' in j:
        j['calls'] = [call(c) for c in j['calls']]
    return j


def path_signature(c):
    """Seed-independent shape of one call: its events and outcome with every
    number and mock string masked (so two seeds that took the same branches
    with different mock values collapse), line annotations dropped."""
    evs = [MOCKSTR.sub('"s"', NUM.sub('#', ANNOT.sub('', e))) for e in c['events']]
    evs.append(MOCKSTR.sub('"s"', NUM.sub('#', c['outcome'])))
    return hashlib.sha1('\n'.join(evs).encode('utf-8')).hexdigest()[:12]


def classify(a, b, other_label):
    """-> (verdict, labels) for one (function, seed): a = orig, b = the side."""
    ea = [ANNOT.sub('', e) for e in a['events']]
    eb = [ANNOT.sub('', e) for e in b['events']]
    labels = []
    if a['harness'] or b['harness'] or a['outcome'].startswith('HARNESS') or b['outcome'].startswith('HARNESS'):
        labels.append('harness')
    if a['poison'] or a['upvalue'] or a.get('shadow'):
        labels.append('harness')          # the original has no slotN/uvN globals
    if a['truncated'] or b['truncated']:
        labels.append('truncated')
    if b['poison'] or b.get('shadow'):
        labels.append('dropped-assignment')
    if b['clobber']:
        labels.append('clobber')
    if b['upvalue']:
        labels.append('upvalue')
    same = ea == eb and a['outcome'] == b['outcome']
    if same and not labels:
        return 'pass', []
    if not same and sorted(ea) == sorted(eb) and a['outcome'] == b['outcome']:
        labels.append('reorder')
    elif not same:
        labels.append(other_label)
    order = ['harness', 'dropped-assignment', 'clobber', 'upvalue', 'reorder', other_label, 'truncated']
    return min(labels, key=order.index), sorted(set(labels), key=order.index)


def first_diff(a, b, ctx=2):
    ea = [ANNOT.sub('', e) for e in a['events']] + ['OUTCOME ' + a['outcome']]
    eb = b['events'] + ['OUTCOME ' + b['outcome']]
    ebn = [ANNOT.sub('', e) for e in eb]
    i = 0
    while i < len(ea) and i < len(ebn) and ea[i] == ebn[i]:
        i += 1
    lo = max(0, i - ctx)
    return i, ea[lo:i + ctx + 1], eb[lo:i + ctx + 1]


def main_values(j):
    out = {}
    for line in j.get('main_values') or []:
        m = re.match(r'^(.*?) #([0-9a-f]{8}) = ', line)
        if m:
            out[m.group(1)] = m.group(2)
    return out


def parse_static(j):
    rows = []
    for r in j.get('static') or []:
        c = r.split('\t')
        rows.append({'fn': c[0], 'name': c[1], 'line': int(c[2][1:]), 'pc': int(c[3][2:]),
                     'tags': [t for t in c[4].split(',') if t] if len(c) > 4 else []})
    return rows


def diff_traces(ta, tb, src=None):
    """Orig trace ta vs side trace tb -> result dict (see report())."""
    if ta.get('types_h32') != tb.get('types_h32'):
        raise StepError('refusing to diff: types_h32 %s vs %s (different type oracles)' % (
            ta.get('types_h32'), tb.get('types_h32')))
    other_label = 'other-ljd' if tb['side'] == 'rt' else 'other'
    ca = {(c['fn'], c['seed']): c for c in ta.get('calls', [])}
    cb = {(c['fn'], c['seed']): c for c in tb.get('calls', [])}
    fa = sorted({k[0] for k in ca})
    fb = sorted({k[0] for k in cb})
    res = {'label': ta['label'], 'side': tb['side'], 'seeds': ta['seeds'], 'other_label': other_label,
           'only_orig': sorted(set(fa) - set(fb)), 'only_side': sorted(set(fb) - set(fa)),
           'load_error': (ta.get('load_error'), tb.get('load_error')), 'fns': {}}
    # main chunk
    va, vb = main_values(ta), main_values(tb)
    # the side's main-chunk locals are slotN globals under the no-local
    # convention: expected, reported apart, never a difference by themselves
    only_side = set(vb) - set(va)
    res['main_values'] = {
        'differ': sorted(k for k in set(va) & set(vb) if va[k] != vb[k]),
        'only_orig': sorted(set(va) - set(vb)),
        'only_side': sorted(k for k in only_side if not SLOTNAME.match(k)),
        'slot_globals': sorted(k for k in only_side if SLOTNAME.match(k)), 'n': len(va)}
    if 'main' in ta and 'main' in tb:
        v, labels = classify(ta['main'], tb['main'], other_label)
        res['main'] = {'verdict': v, 'labels': labels,
                       'first': first_diff(ta['main'], tb['main']) if v != 'pass' else None}
    all_sites = collections.Counter()
    all_lines = set()
    for fn in sorted(set(fa) & set(fb)):
        seeds = sorted(s for (f, s) in ca if f == fn and (f, s) in cb)
        r = {'verdicts': collections.Counter(), 'labels': collections.Counter(), 'seeds': len(seeds),
             'sig_orig': set(), 'sig_side': set(), 'sites': collections.Counter(), 'fatal': collections.Counter(),
             'first': None, 'divergent': [], 'clobber': collections.Counter(),
             'seed_hash': {}, 'seed_sim': {}}
        for s in seeds:
            a, b = ca[(fn, s)], cb[(fn, s)]
            v, labels = classify(a, b, other_label)
            r['verdicts'][v] += 1
            for lab in labels:
                r['labels'][lab] += 1
            r['sig_orig'].add(path_signature(a))
            r['sig_side'].add(path_signature(b))
            for st in b.get('sites', []):
                r['sites'][st] += 1
                all_sites[st] += 1
            for cl in b['clobber']:
                r['clobber'][cl] += 1
            if b.get('fatal'):
                r['fatal'][b['fatal']] += 1
            if b.get('lines'):
                all_lines.update(int(x) for x in b['lines'].split(','))
            # line-independent identity of the side's call, and how many of its
            # events line up with the original's (the fix loop's progress measure)
            eb = [ANNOT.sub('', e) for e in b['events']] + ['OUTCOME ' + b['outcome']]
            r['seed_hash'][s] = hashlib.sha1('\n'.join(eb).encode('utf-8')).hexdigest()[:12]
            if v == 'pass':
                r['seed_sim'][s] = 1.0
            else:
                # a ratio, not a count: removing an extra side event is progress too
                ea = [ANNOT.sub('', e) for e in a['events']] + ['OUTCOME ' + a['outcome']]
                r['seed_sim'][s] = difflib.SequenceMatcher(None, ea, eb, autojunk=False).ratio()
            if v != 'pass':
                r['divergent'].append((s, v))
                if r['first'] is None:
                    r['first'] = (s, v, labels) + first_diff(a, b)
        res['fns'][fn] = r
    if tb.get('main', {}).get('lines'):
        all_lines.update(int(x) for x in tb['main']['lines'].split(','))
    # static rows vs the dynamic read sites
    static = parse_static(tb)
    res['has_lines'] = bool(all_lines)
    res['static'] = []
    skeys = set()
    for row in static:
        key = '%s@L%d' % (row['name'], row['line'])
        skeys.add(key)
        if all_sites[key]:
            status = 'confirmed'
        elif not all_lines:
            status = 'static-only'
        elif row['line'] in all_lines:
            status = 'static-only (executed)'
        else:
            status = 'static-only (unreached)'
        res['static'].append(dict(row, status=status, seeds=all_sites[key]))
    res['static_available'] = tb.get('static') is not None
    res['dynamic_not_static'] = sorted(k for k in all_sites if SITE.match(k) and k not in skeys) \
        if res['static_available'] else []
    # the lint on the same source text
    res['lint'] = None
    if src:
        hits = []
        for fn, _, issues in lint_decompile_slots.lint_file(src):
            for line, n, _ in issues:
                hits.append({'fn': fn, 'name': 'slot%d' % n, 'line': line})
        for h in hits:
            key = '%s@L%d' % (h['name'], h['line'])
            h['static'] = key in skeys
            h['confirmed'] = all_sites[key] > 0
            h['executed'] = h['line'] in all_lines if all_lines else None
        res['lint'] = hits
    return res


def report(res):
    """Human-readable diff report -> list of lines."""
    L = []
    L.append('ljdiff diff: %s  orig vs %s  (%d seeds)' % (res['label'], res['side'], res['seeds']))
    if any(res['load_error']):
        L.append('LOAD_ERROR orig=%s side=%s' % res['load_error'])
    if res['only_orig'] or res['only_side']:
        L.append('entries only in orig: %s' % (', '.join(res['only_orig']) or '-'))
        L.append('entries only in %s: %s' % (res['side'], ', '.join(res['only_side']) or '-'))
    mv = res['main_values']
    L.append('main_values: %d name(s); differ: %s; only orig: %s; only side: %s; side main-chunk slotN globals '
             '(no-local, expected): %s' % (
                 mv['n'], ', '.join(mv['differ']) or '-', ', '.join(mv['only_orig']) or '-',
                 ', '.join(mv['only_side']) or '-', ', '.join(mv['slot_globals']) or '-'))
    if res.get('main'):
        L.append('main chunk: %s %s' % (res['main']['verdict'], ','.join(res['main']['labels'])))
    L.append('')
    L.append('%-58s %5s %5s %5s  %s' % ('function', 'seeds', 'cov_o', 'cov_s', 'verdicts'))
    for fn, r in sorted(res['fns'].items()):
        vs = ' '.join('%s=%d' % (k, n) for k, n in sorted(r['verdicts'].items()))
        L.append('%-58s %5d %5d %5d  %s' % (fn, r['seeds'], len(r['sig_orig']), len(r['sig_side']), vs))
    single = [fn for fn, r in sorted(res['fns'].items()) if len(r['sig_orig']) == 1]
    L.append('')
    L.append('coverage 1 (one path on the original over every seed): %d function(s)%s' % (
        len(single), (': ' + ', '.join(single)) if single else ''))
    for fn, r in sorted(res['fns'].items()):
        if not r['divergent'] and not r['sites'] and not r['clobber']:
            continue
        L.append('')
        L.append('== %s: %s' % (fn, ' '.join('%s=%d' % kv for kv in sorted(r['verdicts'].items()))))
        L.append('   divergent seeds: %s' % ' '.join('%d:%s' % sv for sv in r['divergent']))
        if r['sites']:
            L.append('   read sites (seeds): %s' % ', '.join('%s(%d)' % kv for kv in sorted(r['sites'].items())))
        if r['fatal']:
            L.append('   fatal: %s' % ', '.join('%s(%d)' % kv for kv in sorted(r['fatal'].items())))
        if r['clobber']:
            L.append('   clobber: %s' % ', '.join('%s(%d)' % kv for kv in sorted(r['clobber'].items())))
        if r['first']:
            s, v, labels, i, xa, xb = r['first']
            L.append('   first divergence: seed %d, %s [%s], event %d' % (s, v, ','.join(labels), i))
            for e in xa:
                L.append('     orig | %s' % e)
            for e in xb:
                L.append('     %4s | %s' % (res['side'][:4], e))
    L.append('')
    if not res['static_available']:
        L.append('static pass: not available (chunk without line info; use the -bg twin)')
    else:
        L.append('static must-define candidates: %d' % len(res['static']))
        for row in res['static']:
            L.append('   %-56s %-6s L%-5d %-24s %s%s' % (row['fn'], row['name'], row['line'], row['status'],
                     ','.join(row['tags']), (' seeds=%d' % row['seeds']) if row['seeds'] else ''))
        L.append('dynamic read sites the static pass missed: %s' % (', '.join(res['dynamic_not_static']) or 'none'))
    if res['lint'] is not None:
        L.append('')
        L.append('lint hits: %d' % len(res['lint']))
        for h in res['lint']:
            L.append('   %-56s %-6s L%-5d static=%s confirmed=%s executed=%s' % (
                h['fn'], h['name'], h['line'], h['static'], h['confirmed'], h['executed']))
    return L


def summary_row(res):
    tot = collections.Counter()
    for r in res['fns'].values():
        tot.update(r['verdicts'])
    return tot


def diff_files(flat, sides, srcs, write=True):
    """orig vs every other side of one built file -> {side: result}"""
    ta = json.load(open(tmp(flat, 'orig.json'), encoding='utf-8'))
    out = {}
    for side in sides:
        if side == 'orig':
            continue
        tb = json.load(open(tmp(flat, side + '.json'), encoding='utf-8'))
        res = diff_traces(ta, tb, srcs.get(side))
        if write:
            with open(tmp(flat, side + '.diff.txt'), 'w', encoding='utf-8', newline='\n') as f:
                f.write('\n'.join(report(res)) + '\n')
        out[side] = res
    return out


# ---------------------------------------------------------------- fix-rerun loop

def fn_span(lines, fn):
    """(first, last) line indices of top-level `function FN(` ... `end`."""
    start = None
    for i, ln in enumerate(lines):
        if start is None and re.match(r'^function %s\(' % re.escape(fn), ln):
            start = i
        elif start is not None and ln.rstrip('\r\n') == 'end':
            return start, i
    raise StepError('fix: function %s not found' % fn)


def apply_fix(text, fix):
    """Replace fix['old'] (exact text, LF line breaks; matched against the
    CRLF source) with fix['new'], once, inside fix['fn']'s body. Whole lines
    only. -> (new text, 0-based first line replaced, old lines, new lines)"""
    crlf = '\r\n' in text
    t = text.replace('\r\n', '\n')
    lines = t.split('\n')
    a, b = fn_span(lines, fix['fn'])
    body = '\n'.join(lines[a:b + 1])
    n = body.count(fix['old'])
    if n != 1:
        raise StepError('fix %s: old text found %d times in %s' % (fix['id'], n, fix['fn']))
    i = body.index(fix['old'])
    j = i + len(fix['old'])
    if (i and body[i - 1] != '\n') or (j < len(body) and body[j] != '\n'):
        raise StepError('fix %s: old text must cover whole lines' % fix['id'])
    loc = a + body[:i].count('\n')
    body = body.replace(fix['old'], fix['new'])
    out = '\n'.join(lines[:a] + [body] + lines[b + 1:])
    return (out.replace('\n', '\r\n') if crlf else out), loc, fix['old'].split('\n'), fix['new'].split('\n')


def remap(cur, loc, old, new):
    """Line map after one replacement: cur[i] is the fresh decompile's line
    number of current line i. A replaced line that reappears verbatim keeps its
    number; any other new line inherits the number of the old line at the same
    position (clamped), so a one-line rewrite keeps its line exactly."""
    seg, used, mid = cur[loc:loc + len(old)], set(), []
    for i, nl in enumerate(new):
        m = next((k for k, ol in enumerate(old) if k not in used and ol == nl and nl.strip()), None)
        if m is not None:
            used.add(m)
        mid.append(seg[m] if m is not None else seg[min(i, len(seg) - 1)])
    return cur[:loc] + mid + cur[loc + len(old):]


def revert_fix(text, fix):
    """Undo one fix inside a text where every fix is applied: its new text must
    still be there verbatim, once, inside its function."""
    inv = dict(fix, old=fix['new'], new=fix['old'])
    try:
        return apply_fix(text, inv)
    except StepError as e:
        raise StepError('leave-one-out: cannot revert %s (%s); a later fix must keep its new text verbatim'
                        % (fix['id'], e))


def static_sites(res, fn, lmap):
    """{(fresh-decompile line, name)} of fn's static rows under line map lmap"""
    out = set()
    for row in res['static']:
        if row['fn'] == fn:
            out.add((lmap[row['line'] - 1], row['name']))
    return out


def fn_state(res, fn):
    r = res['fns'].get(fn)
    if not r:
        return None
    names = collections.Counter()
    for st in r['sites']:
        names[st.split('@')[0]] += r['sites'][st]
    fatal = set()
    for f in r['fatal']:
        m = re.search(r'(?:POISON|UPVALUE)_READ (\w+)', f)
        if m:
            fatal.add(m.group(1))
    return {'div': len(r['divergent']), 'verdicts': dict(r['verdicts']), 'sites': dict(r['sites']),
            'names': dict(names), 'fatal': sorted(fatal), 'seed_hash': r['seed_hash'], 'seed_sim': r['seed_sim']}


def detect(state, fix):
    """How the harness sees fix's instance in a state that lacks it."""
    if fix['slot'] in state['names']:
        return 'fatal poison' if fix['slot'] in state['fatal'] else 'poison'
    return 'diverge-only' if state['div'] else 'unobserved'


def fixloop(june_path, fixes_path, seeds, opts):
    """The fix-rerun loop. Apply the curated restorations one at a time to the
    fresh decompile and rerun both sides after each (the ladder: a fatal read
    hides the reads behind it, so each step shows what the last one revealed).
    Then, from the all-fixed text, revert each fix alone and rerun
    (leave-one-out): a fix is a harness-confirmed instance iff its function no
    longer matches the original without it, and the static sites left in that
    state are the ones it owns. Leave-one-out does not depend on fix order."""
    fixes = json.load(open(fixes_path, encoding='utf-8'))['fixes']
    table = ljconv.load_table(ljconv.TABLE_TSV)
    flat = flat_of(june_path)
    clean([flat])
    log = []
    _, sides, srcs = build(june_path, table, [])
    learn([(flat, sides, srcs)], seeds, log)
    trace_one(flat, 'orig', sides['orig'], seeds, log)
    ta = json.load(open(tmp(flat, 'orig.json'), encoding='utf-8'))

    def run_text(side, text):
        src = tmp(flat, side + '.src.lua')
        with open(src, 'w', encoding='utf-8', newline='') as f:
            f.write(text)
        chunk = compile_mar(src, tmp(flat, side + '.luac'))
        trace_one(flat, side, chunk, seeds, log, opts)
        tb = json.load(open(tmp(flat, side + '.json'), encoding='utf-8'))
        tb['side'] = 'rt'        # classify as the plain decompile (other-ljd)
        res = diff_traces(ta, tb, src)
        with open(tmp(flat, side + '.diff.txt'), 'w', encoding='utf-8', newline='\n') as f:
            f.write('\n'.join(report(res)) + '\n')
        return res

    text = open(srcs['rt'], encoding='utf-8', newline='').read()
    lmap = list(range(1, text.count('\n') + 2))
    states, maps = [], []
    for k in range(len(fixes) + 1):
        if k:
            text, loc, old, new = apply_fix(text, fixes[k - 1])
            lmap = remap(lmap, loc, old, new)
        maps.append(lmap)
        states.append(run_text('fix%02d' % k, text))
    final_text, final_map = text, lmap

    L = ['fix-rerun loop: %s, %d fix(es), %d seeds' % (flat, len(fixes), seeds), '',
         'ladder (fixes applied in file order; "seen" is what the harness showed before each one):']
    ok = True
    ladder_all = set()
    for k, fix in enumerate(fixes, 1):
        before, after = states[k - 1], states[k]
        fb, fa = fn_state(before, fix['fn']), fn_state(after, fix['fn'])
        how = detect(fb, fix)
        if how == 'diverge-only' and not any(
                fa['seed_sim'][x] > fb['seed_sim'][x] for x in fa['seed_sim'] if x in fb['seed_sim']):
            how = 'unobserved'   # still diverging, but this fix moved no seed closer
        # nothing outside this function may change (line-free per-seed hashes)
        others = []
        for fn in set(before['fns']) | set(after['fns']):
            sb, sa = fn_state(before, fn), fn_state(after, fn)
            if fn != fix['fn'] and (sb is None or sa is None or sb['seed_hash'] != sa['seed_hash']):
                others.append(fn)
        if others:
            ok = False
        revealed = sorted(set(fa['names']) - set(fb['names']))
        sb_, sa_ = static_sites(before, fix['fn'], maps[k - 1]), static_sites(after, fix['fn'], maps[k])
        if fix.get('kind', 'dropped') == 'dropped':
            ladder_all |= {(fix['fn'], l, n) for l, n in sb_ - sa_}
        fix['_cleared'], fix['_seen'] = sorted(sb_ - sa_), how
        L.append('%2d %-5s %-7s %-50s %-6s seen: %-12s div %2d -> %2d  unwritten %s -> %s%s%s' % (
            k, fix['id'], fix.get('kind', 'dropped'), fix['fn'], fix['slot'], how, fb['div'], fa['div'],
            ','.join(sorted(fb['names'])) or '-', ','.join(sorted(fa['names'])) or '-',
            ('  revealed ' + ','.join(revealed)) if revealed else '',
            ('  CHANGED OTHER FUNCTIONS: ' + ','.join(sorted(others))) if others else ''))
        L.append('      static sites cleared: %s' % (
            ', '.join('%s@L%d' % (n, l) for l, n in sorted(sb_ - sa_)) or '-'))
    L.append('')
    final = states[-1]
    fixed_fns = sorted({f['fn'] for f in fixes})
    L.append('after all fixes:')
    for fn in sorted(final['fns']):
        st = fn_state(final, fn)
        if st['div'] or fn in fixed_fns:
            L.append('   %-58s %s%s' % (fn, ' '.join('%s=%d' % kv for kv in sorted(st['verdicts'].items())),
                                        '  (fixed function)' if fn in fixed_fns else ''))
            if fn in fixed_fns and st['div']:
                ok = False
    L.append('   static candidates left: %d' % len(final['static']))
    if final['static']:
        ok = False

    # leave-one-out
    L.append('')
    L.append('leave-one-out (all fixes but one; order-independent):')
    counts = collections.Counter()
    owned_all = set()
    for k, fix in enumerate(fixes, 1):
        t, loc, old, new = revert_fix(final_text, fix)
        m = remap(final_map, loc, old, new)
        res = run_text('loo%02d' % k, t)
        st = fn_state(res, fix['fn'])
        how = detect(st, fix)
        others = [fn for fn in res['fns'] if fn != fix['fn'] and fn_state(res, fn)['div']]
        if others or not st['div']:
            ok = False
        owned = static_sites(res, fix['fn'], m)
        if fix.get('kind', 'dropped') == 'dropped':
            owned_all |= {(fix['fn'], l, n) for l, n in owned}
        counts[(fix.get('kind', 'dropped'), how)] += 1
        fix['_how'], fix['_owned'] = how, sorted(owned)
        L.append('%2d %-5s %-7s %-50s %-6s %-13s div %2d/%d  owns %s%s' % (
            k, fix['id'], fix.get('kind', 'dropped'), fix['fn'], fix['slot'],
            'NECESSARY' if st['div'] else 'not needed', st['div'], res['seeds'],
            ', '.join('%s@L%d' % (n, l) for l, n in sorted(owned)) or '- (stale value, no unwritten read)',
            ('  OTHER FUNCTIONS DIVERGE: ' + ','.join(sorted(others))) if others else ''))
        L.append('      without it: %s' % how)
    # every static site of the fresh decompile is accounted for by the dropped
    # fixes: owned (leave-one-out re-exposes it) or, when two dropped writes of
    # one slot both have to be missing for the read to be unwritten, cleared on
    # the ladder only ("joint")
    fresh = {(r['fn'], r['line'], r['name']) for r in states[0]['static']}
    acc = owned_all | ladder_all
    fmt3 = lambda st: ', '.join('%s %s@L%d' % (f, n, l) for f, l, n in sorted(st)) or 'none'
    L.append('')
    L.append('fresh decompile static sites: %d unique (line, name); owned (leave-one-out) %d; joint (ladder only) %d: %s'
             % (len(fresh), len(owned_all & fresh), len((fresh & ladder_all) - owned_all),
                fmt3((fresh & ladder_all) - owned_all)))
    L.append('   unaccounted: %s; accounted but not in the fresh decompile: %s' % (fmt3(fresh - acc), fmt3(acc - fresh)))
    if fresh != acc:
        ok = False
    L.append('')
    for kind in sorted({k for k, _ in counts}):
        n = sum(v for (k, _), v in counts.items() if k == kind)
        L.append('%s fixes: %d; seen without it as: %s' % (kind, n, ', '.join(
            '%s %d' % (h, v) for (k, h), v in sorted(counts.items()) if k == kind)))
    L.append('result: %s' % ('OK - each fixed function matches the original on every seed, nothing else '
                             'moved, every fix is necessary, and the dropped fixes account for every static site' if ok else 'NOT CLEAN'))
    return ok, L, states, fixes


# ---------------------------------------------------------------- main

def accept_march(on):
    global ACCEPT_MARCH
    ACCEPT_MARCH = bool(on)


def use_fixed_ljd(fixed):
    global LJD
    if fixed:
        LJD = LJD_FIXED


def main(argv):
    if argv and argv[0] == 'diff':
        ap = argparse.ArgumentParser(prog='ljdiff.py diff')
        ap.add_argument('a')
        ap.add_argument('b')
        ap.add_argument('--src', help='the side\'s .src.lua, for the lint cross-check')
        args = ap.parse_args(argv[1:])
        ta = json.load(open(args.a, encoding='utf-8'))
        tb = json.load(open(args.b, encoding='utf-8'))
        try:
            print('\n'.join(report(diff_traces(ta, tb, args.src))))
        except StepError as e:
            print('STEP_ERROR %s' % e)
            return 2
        return 0
    if argv and argv[0] == 'fixloop':
        ap = argparse.ArgumentParser(prog='ljdiff.py fixloop')
        ap.add_argument('file')
        ap.add_argument('--fixes', required=True)
        ap.add_argument('--seeds', type=int, default=32)
        ap.add_argument('--no-lines', action='store_true')
        ap.add_argument('--fixed', action='store_true', help='decompile with ljd_fixed.py')
        args = ap.parse_args(argv[1:])
        use_fixed_ljd(args.fixed)
        os.makedirs(TMP, exist_ok=True)
        try:
            ok, L, _, _ = fixloop(resolve(args.file), args.fixes, args.seeds, '' if args.no_lines else 'lines')
        except (StepError, subprocess.TimeoutExpired) as e:
            print('STEP_ERROR %s' % e)
            return 2
        print('\n'.join(L))
        return 0 if ok else 1

    ap = argparse.ArgumentParser()
    ap.add_argument('files', nargs='*')
    ap.add_argument('--seeds', type=int, default=32)
    ap.add_argument('--extra', action='append', default=[], metavar='NAME=PATH',
                    help='also trace this .src.lua, compiled with compile-batch -v mar-2014')
    ap.add_argument('--twice', action='store_true')
    ap.add_argument('--diff', action='store_true', help='diff orig against every other side afterwards')
    ap.add_argument('--lines', action='store_true', help='record per-call line coverage (slower)')
    ap.add_argument('--no-twin', action='store_true', help='trace compile-batch output, not the -bg twin')
    ap.add_argument('--selftest', action='store_true',
                    help='only run selftest/selftest.lua (CLOBBER, re-entry, poison, learners, static pass)')
    ap.add_argument('--fixed', action='store_true',
                    help='decompile the rt side with ljd_fixed.py (the Phase 4 slotworks fix)')
    ap.add_argument('--march', action='store_true',
                    help='also accept March 0x81 files (absolute paths); orig = the shipped bytecode (LUA_SOURCE_PHASES.md L2)')
    args = ap.parse_args(argv)
    use_fixed_ljd(args.fixed)
    accept_march(args.march)
    os.makedirs(TMP, exist_ok=True)
    if args.selftest:
        try:
            return 0 if selftest(args.seeds) else 1
        except (StepError, subprocess.TimeoutExpired) as e:
            print('STEP_ERROR %s' % e)
            return 2
    files = [resolve(f) for f in (args.files or DEFAULT_FILES)]
    flats = [flat_of(p) for p in files]
    extras = []
    for e in args.extra:
        name, _, p = e.partition('=')
        if not name or not p or name in ('orig', 'rt') or not os.path.isfile(p) or not re.match(r'^\w+$', name):
            raise SystemExit('--extra wants NAME=PATH with an existing file and a new side name: %r' % e)
        extras.append((name, os.path.abspath(p)))
    if not os.path.exists(REENTRANT):
        print('warning: %s missing - engine re-entry is not simulated' % REENTRANT)
    opts = 'lines' if args.lines else ''
    twin = not args.no_twin

    try:
        log, rounds, res, built = one_run(files, extras, args.seeds, twin, opts)
    except (StepError, subprocess.TimeoutExpired) as e:
        print('STEP_ERROR %s' % e)
        return 2
    print('\n'.join(log))
    print('\nlearning converged after %d round(s); types.tsv has %d row(s)' % (rounds, len(load_types())))
    print('%-44s %-5s %6s %7s %6s %6s %7s %7s %6s %7s %5s %5s' % (
        'file', 'side', 'calls', 'harness', 'poison', 'shadow', 'upvalue', 'clobber', 'budget', 'reenter',
        'unsim', 'trunc'))
    harness = 0
    for out, (flat, side, t, ncalls, lerr) in sorted(res.items()):
        harness += t['harness']
        print('%-44s %-5s %6d %7d %6d %6d %7d %7d %6d %7d %5d %5d%s' % (
            flat, side, ncalls, t['harness'], t['poison'], t.get('shadow', 0), t['upvalue'], t['clobber'],
            t['budget'], t['reenter'], t['unsim'], t['truncated'], ('  LOAD_ERROR ' + lerr) if lerr else ''))
    ok = harness == 0
    if args.diff:
        print('')
        for flat, sides, srcs in built:
            try:
                results = diff_files(flat, sides, srcs)
            except StepError as e:
                print('STEP_ERROR %s' % e)
                return 2
            for side, r in sorted(results.items()):
                tot = summary_row(r)
                conf = sum(1 for s in r['static'] if s['status'] == 'confirmed')
                print('diff %-40s orig vs %-5s %s | static %d (confirmed %d) -> %s' % (
                    flat, side, ' '.join('%s=%d' % kv for kv in sorted(tot.items())), len(r['static']), conf,
                    os.path.basename(tmp(flat, side + '.diff.txt'))))
    if args.twice:
        d1 = traces_digest(flats)
        try:
            one_run(files, extras, args.seeds, twin, opts)
        except (StepError, subprocess.TimeoutExpired) as e:
            print('STEP_ERROR (second run) %s' % e)
            return 2
        d2 = traces_digest(flats)
        same = d1 == d2
        diffs = sorted(k for k in set(d1) | set(d2) if d1.get(k) != d2.get(k))
        print('\ndeterminism: %s (%d trace(s) compared%s)' % (
            'IDENTICAL' if same else 'DIFFERENT', len(d1), '' if same else '; differing: ' + ', '.join(diffs)))
        ok = ok and same
    print('\nresult: %s' % ('OK' if ok else 'HARNESS ERRORS' if harness else 'NONDETERMINISTIC'))
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
