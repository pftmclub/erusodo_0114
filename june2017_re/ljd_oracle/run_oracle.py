"""Engine-free ljd oracle (Phase 1 of RANOX_LUA_DECOMPILE_FIX_PLAN.md).

For every cases/<name>.lua and each build (strip = `luajit -b`, g = `luajit -bg`):
  1. compile with the vendored KOG luajit.exe (must emit 1b 4b 4c 81; the flags
     byte must say stripped for `strip` and not for `g`);
  2. swap the header to stock 1b 4c 4a 01 and decompile with the vendored ljd,
     unmodified, in its own process;
  3. copy ljd's text to a tmp/ file and recompile it in place with
     `elstools compile-batch -v mar-2014` (the port pipeline);
  4. run driver.lua on the original and on the round trip with
     cases/<name>.calls.lua, and compare the two traces call by call.

    python run_oracle.py                   # everything, compare to expected.tsv
    python run_oracle.py --case kenta      # one case
    python run_oracle.py --expect fixed    # compare to the post-fix column (Phase 4)
    python run_oracle.py --fixed --expect fixed   # decompile with ../ljd_fixed.py (the Phase 4 fix)
    python run_oracle.py --twice           # clean, run, clean, run; byte-compare all outputs
    python run_oracle.py --observed F.tsv  # also dump every observed verdict/fact (for review)

Per-call verdicts, strongest first (the first differing line is only detail:
the dropped assignment also reorders calls, so a CALL line usually differs
before the poisoned read does):
  HARNESS       the ORIGINAL read an unknown global - a bug in the case/driver
  REPRO_POISON  the round trip read a slotN it never wrote in this call
  REPRO_GLOBAL  the round trip read some other unknown global (a -g local)
  UPVALUE       the round trip read a uvN global (ljd's upvalue name)
  DIVERGE       traces differ with no unknown-global read
  PASS          traces identical
Text facts (kind=text in expected.tsv): `assigns=N`, the number of assignment
statements ljd prints in that function - pins over-conservative inlining.
expected.tsv: expect_now is unmodified ljd's reviewed behaviour; expect_fixed
is what a correct Phase 4 fix must give. '-' means not checked.

All scratch goes to <Trunk>/tmp/ljoracle_*. Exit status 0 iff every row of
expected.tsv matched (and, with --twice, both runs were byte-identical).
"""
import argparse
import csv
import glob
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.abspath(os.path.join(HERE, '..', '..'))
TMP = os.path.join(TRUNK, 'tmp')
PREFIX = 'ljoracle_'
LUAJIT_DIR = os.path.join(TRUNK, 'elstools', 'vendor', 'luajit')
LUAJIT = os.path.join(LUAJIT_DIR, 'luajit.exe')
LJD = os.path.join(TRUNK, 'elstools', 'vendor', 'ljd', 'main.py')
LJD_FIXED = os.path.join(TRUNK, 'june2017_re', 'ljd_fixed.py')  # same CLI as main.py, fix installed
ELSTOOLS = os.path.join(TRUNK, 'elstools', 'elstools.exe')
DRIVER = os.path.join(HERE, 'driver.lua')
CASES = os.path.join(HERE, 'cases')
EXPECTED = os.path.join(HERE, 'expected.tsv')

MAR_MAGIC = b'\x1bKL\x81'
STD_MAGIC = b'\x1bLJ\x01'
BCDUMP_F_STRIP = 0x02
BUILDS = (('strip', '-b'), ('g', '-bg'))
READ_CLASSES = ('POISON_READ', 'GLOBAL_READ', 'UPVALUE_READ')


class StepError(Exception):
    pass


def run(cmd, cwd, timeout):
    p = subprocess.run(cmd, cwd=cwd, capture_output=True, timeout=timeout)
    return p.returncode, p.stdout.decode('utf-8', 'replace'), p.stderr.decode('utf-8', 'replace')


def tmp(case, build, kind):
    return os.path.join(TMP, '%s%s_%s.%s' % (PREFIX, case, build, kind))


def build_case(case, build, flag):
    """Steps 1-3. Returns (orig_luac, rt_luac, ljd_src)."""
    src = os.path.join(CASES, case + '.lua')
    orig = tmp(case, build, 'orig.luac')
    rc, so, se = run([LUAJIT, flag, src, orig], LUAJIT_DIR, 60)
    if rc != 0:
        raise StepError('luajit %s failed: %s' % (flag, (so + se).strip()))
    d = open(orig, 'rb').read()
    if d[:4] != MAR_MAGIC:
        raise StepError('luajit emitted header %s, expected %s' % (d[:4].hex(), MAR_MAGIC.hex()))
    stripped = bool(d[4] & BCDUMP_F_STRIP)
    if stripped != (build == 'strip'):
        raise StepError('flags byte 0x%02x: stripped=%s for build %s' % (d[4], stripped, build))

    stock = tmp(case, build, 'stock.luac')
    open(stock, 'wb').write(STD_MAGIC + d[4:])
    ljd_src = tmp(case, build, 'src.lua')
    rc, so, se = run([sys.executable, LJD, '-f', stock, '-o', ljd_src], os.path.dirname(LJD), 300)
    if rc != 0 or not os.path.exists(ljd_src):
        raise StepError('ljd failed (rc %d): %s' % (rc, se.strip().splitlines()[-1:] or ''))
    if LJD == LJD_FIXED and 'slotworks fix active' not in se:
        raise StepError('ljd_fixed.py ran without reporting the fix active: %s' % se.strip()[-300:])

    rt = tmp(case, build, 'rt.luac')
    shutil.copyfile(ljd_src, rt)
    rc, so, se = run([ELSTOOLS, 'compile-batch', '-q', '-v', 'mar-2014', '-i', rt], TRUNK, 120)
    res = None
    for line in so.splitlines():
        try:
            j = json.loads(line)
        except ValueError:
            continue
        if j.get('type') == 'result':
            res = j
    if rc != 0 or not res or res.get('ok') != 1 or res.get('failed'):
        raise StepError('compile-batch failed (rc %d): %s' % (rc, (so + se).strip()))
    if open(rt, 'rb').read(4) != MAR_MAGIC:
        raise StepError('compile-batch output is not 0x81 bytecode')
    return orig, rt, ljd_src


def trace(case, build, side, chunk):
    rc, so, se = run([LUAJIT, DRIVER, chunk, os.path.join(CASES, case + '.calls.lua')], LUAJIT_DIR, 120)
    path = tmp(case, build, side + '.trace')
    open(path, 'w', encoding='utf-8', newline='\n').write(so)
    if rc != 0:
        raise StepError('driver failed on %s (rc %d): %s' % (side, rc, (so + se).strip()[-300:]))
    return so


def blocks(text):
    """{id: [lines]} for MAIN and every CALL_BEGIN..CALL_END; '#' lines dropped."""
    out, order, cur = {}, [], None
    for line in text.splitlines():
        if line.startswith('#'):
            continue
        if line == 'MAIN_BEGIN':
            cur = 'MAIN'
        elif line.startswith('CALL_BEGIN '):
            cur = line.split()[1]
        if cur is not None:
            out.setdefault(cur, []).append(line)
            if cur not in order:
                order.append(cur)
        if line == 'MAIN_END' or line.startswith('CALL_END '):
            cur = None
    return order, out


def reads(lines):
    return [l for l in lines if l.split(' ', 1)[0] in READ_CLASSES]


def verdict(a, b):
    if reads(a):
        return 'HARNESS', 'original read: ' + '; '.join(reads(a))
    rb = reads(b)
    first = next((i for i, (x, y) in enumerate(zip(a, b)) if x != y), None)
    if first is None and len(a) != len(b):
        first = min(len(a), len(b))
    diff = ''
    if first is not None:
        diff = 'first diff @%d: orig=%r rt=%r' % (
            first, a[first] if first < len(a) else '<end>', b[first] if first < len(b) else '<end>')
    end = ''
    oa = [l for l in a if l.startswith(('RET', 'ERROR', 'BUDGET'))][-1:]
    ob = [l for l in b if l.startswith(('RET', 'ERROR', 'BUDGET'))][-1:]
    if oa != ob:
        end = 'outcome: orig=%r rt=%r' % ((oa or ['<none>'])[0], (ob or ['<none>'])[0])
    detail = '; '.join(filter(None, [', '.join(sorted(set(rb))), diff, end]))
    kinds = set(l.split(' ', 1)[0] for l in rb)
    if 'POISON_READ' in kinds:
        return 'REPRO_POISON', detail
    if 'GLOBAL_READ' in kinds:
        return 'REPRO_GLOBAL', detail
    if 'UPVALUE_READ' in kinds:
        return 'UPVALUE', detail
    if first is not None:
        return 'DIVERGE', detail
    return 'PASS', ''


ASSIGN = re.compile(r'^\s*(local\s+)?[A-Za-z_]\w*(\s*,\s*[A-Za-z_]\w*)*\s*=(?!=)')


def text_facts(ljd_src):
    """{function name: number of assignment statements in its printed body}."""
    facts, name = {}, None
    for line in open(ljd_src, encoding='utf-8', errors='replace').read().splitlines():
        m = re.match(r'^function ([A-Za-z_]\w*)\(', line)
        if m:
            name = m.group(1)
            facts[name] = 0
            continue
        if name and re.match(r'^end\s*$', line):
            name = None
            continue
        if name and ASSIGN.match(line):
            facts[name] += 1
    return facts


def load_expected():
    rows = {}
    with open(EXPECTED, encoding='utf-8', newline='') as f:
        for r in csv.DictReader((l for l in f if not l.startswith('#')), delimiter='\t'):
            rows[(r['case'], r['build'], r['kind'], r['key'])] = r
    return rows


def run_all(cases, expect_col, observed=None):
    expected = load_expected()
    lines, ok = [], True
    seen = set()
    summary = []
    for case in cases:
        for build, flag in BUILDS:
            got = {}
            try:
                orig, rt, ljd_src = build_case(case, build, flag)
                oa, ta = blocks(trace(case, build, 'orig', orig))
                ob, tb = blocks(trace(case, build, 'rt', rt))
                if oa != ob:
                    raise StepError('call lists differ: %r vs %r' % (oa, ob))
                for cid in oa:
                    got[('call', cid)] = verdict(ta[cid], tb[cid])
                for fn, n in sorted(text_facts(ljd_src).items()):
                    got[('text', fn)] = ('assigns=%d' % n, '')
            except (StepError, subprocess.TimeoutExpired) as e:
                got[('call', 'BUILD')] = ('STEP_ERROR', str(e))
            counts = {}
            for (kind, key), (v, _) in got.items():
                if kind == 'call':
                    counts[v] = counts.get(v, 0) + 1
            mism = []
            if observed is not None:
                for (kind, key), (v, detail) in sorted(got.items()):
                    observed.append((case, build, kind, key, v, detail))
            for (kind, key), (v, detail) in got.items():
                row = expected.get((case, build, kind, key))
                seen.add((case, build, kind, key))
                want = row[expect_col] if row else None
                if want is None:
                    if kind == 'call':
                        mism.append('  UNEXPECTED-ROW %s %s -> %s %s' % (kind, key, v, detail))
                elif want != '-' and want != v:
                    mism.append('  MISMATCH %s %s: expected %s, got %s  %s' % (kind, key, want, v, detail))
            for (c, b, kind, key), row in expected.items():
                if c == case and b == build and (kind, key) not in got:
                    mism.append('  MISSING %s %s (expected %s)' % (kind, key, row[expect_col]))
            status = 'ok' if not mism else 'FAIL'
            ok &= not mism
            summary.append('%-10s %-6s %-4s %s' % (case, build, status,
                           ' '.join('%s=%d' % kv for kv in sorted(counts.items()))))
            lines.append('== %s / %s: %s' % (case, build, status))
            for (kind, key), (v, detail) in sorted(got.items()):
                if kind == 'call' and v != 'PASS':
                    lines.append('  %-28s %-13s %s' % (key, v, detail))
            lines.extend(mism)
    header = ['case       build  res  call verdicts', '-' * 60]
    report = '\n'.join(header + summary + [''] + lines) + '\n'
    return report, ok


def clean():
    for p in glob.glob(os.path.join(TMP, PREFIX + '*')):
        os.remove(p)


def snapshot():
    return {os.path.basename(p): hashlib.sha1(open(p, 'rb').read()).hexdigest()
            for p in sorted(glob.glob(os.path.join(TMP, PREFIX + '*')))}


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument('--case', action='append')
    ap.add_argument('--expect', choices=('now', 'fixed'), default='now')
    ap.add_argument('--twice', action='store_true')
    ap.add_argument('--observed', help='also write every observed verdict/fact to this TSV')
    ap.add_argument('--fixed', action='store_true', help='decompile with ljd_fixed.py instead of the vendored main.py')
    args = ap.parse_args(argv)
    if args.fixed:
        global LJD
        LJD = LJD_FIXED
    os.makedirs(TMP, exist_ok=True)
    cases = args.case or sorted(os.path.basename(p)[:-4] for p in glob.glob(os.path.join(CASES, '*.lua'))
                                if not p.endswith('.calls.lua'))
    col = 'expect_' + args.expect

    clean()
    observed = [] if args.observed else None
    report, ok = run_all(cases, col, observed)
    sys.stdout.write(report)
    if args.observed:
        with open(args.observed, 'w', encoding='utf-8', newline='\n') as f:
            f.write('case\tbuild\tkind\tkey\tobserved\tdetail\n')
            for r in observed:
                f.write('\t'.join(r) + '\n')
    if args.twice:
        snap1 = snapshot()
        clean()
        report2, ok2 = run_all(cases, col)
        snap2 = snapshot()
        same = report == report2 and snap1 == snap2
        diffs = sorted(k for k in set(snap1) | set(snap2) if snap1.get(k) != snap2.get(k))
        print('\ndeterminism: %s (%d files compared%s)' % (
            'IDENTICAL' if same else 'DIFFERENT', len(snap1),
            '' if same else '; differing: ' + ', '.join(diffs or ['report text'])))
        ok = ok and ok2 and same
    open(os.path.join(TMP, PREFIX + 'report.txt'), 'w', encoding='utf-8', newline='\n').write(report)
    print('\nresult: %s' % ('ALL EXPECTED' if ok else 'MISMATCHES'))
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
