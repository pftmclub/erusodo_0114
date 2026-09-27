"""corpus_run.py WORKER for Phase 0's decompile sweep: June (0x82) -> stock
(1b 4c 4a 01) via ljconv.transcode_stock (no .std.luac sibling needed) ->
unmodified vendored ljd, all in this one process (one file per interpreter,
since ljd keeps module-level global state).

    python decompile_worker.py <abs_lua_path> --budget SECS
        --outdir DIR --table TABLE_TSV [--hang-dir DIR]

Prints one TSV line to stdout:
    ljconv_ok  decompile_verdict  detail  n_instructions  n_fix_hits
    opcode_set_hash  n_distinct_opcodes

ljconv_ok: yes/no (did every opcode in the file resolve through the table).
decompile_verdict: ok / assert / timeout-internal / unverifiable-truncated /
    unmapped-opcode / n-a (when ljconv_ok is no).
detail: free text -- for 'assert', "ExceptionType at ljd/file.py:line"; for
    'unverifiable-truncated', the tolerant walker's status; empty for 'ok'.

Exit code is always 0 once arm_faulthandler is installed and argv parses --
every failure mode is a printed verdict, not a process exit code, so
corpus_run.py's CRASH bucket stays reserved for genuinely unexpected process
death (segfault, the parent's backstop kill, an exception before the verdict
print).
"""
import argparse
import hashlib
import os
import runpy
import sys
import traceback

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
LJD = os.path.join(HERE, '..', 'elstools', 'vendor', 'ljd')

import corpus_run  # noqa: E402
import ljconv  # noqa: E402

_HANG_FH = None  # keep alive: arm_faulthandler's handle must not be gc'd


def emit(*cols):
    print('\t'.join(str(c) for c in cols))
    sys.stdout.flush()


def ljd_frame(tb):
    """Innermost 'file.py:line' inside elstools/vendor/ljd, from a traceback."""
    frames = traceback.extract_tb(tb)
    for fr in reversed(frames):
        if 'ljd' in fr.filename.replace('\\', '/').split('/'):
            return '%s:%d' % (os.path.basename(fr.filename), fr.lineno)
    if frames:
        fr = frames[-1]
        return '%s:%d' % (os.path.basename(fr.filename), fr.lineno)
    return '?'


def main(argv):
    global _HANG_FH
    ap = argparse.ArgumentParser()
    ap.add_argument('lua_path')
    ap.add_argument('--budget', type=float, default=60)
    ap.add_argument('--outdir', default=os.path.join(HERE, '..', 'tmp', 'decompile_sweep'))
    ap.add_argument('--table', default=os.path.join(HERE, 'ljconv_table.tsv'))
    ap.add_argument('--hang-dir')
    args = ap.parse_args(argv)

    os.makedirs(args.outdir, exist_ok=True)
    hang_dir = args.hang_dir or args.outdir
    os.makedirs(hang_dir, exist_ok=True)
    flat = os.path.basename(args.lua_path)
    # Disambiguate same-basename files from different dirs (corpus_run passes
    # an absolute path whose parent dir names the *_kom folder).
    flat = os.path.basename(os.path.dirname(args.lua_path)) + '__' + flat
    _HANG_FH = corpus_run.arm_faulthandler(args.budget, os.path.join(hang_dir, flat + '.hang.txt'))

    d = open(args.lua_path, 'rb').read()

    r = ljconv.instruction_offsets_tolerant(d, ljconv.JUNE_MAGIC)
    if r['status'] != 'complete':
        emit('no', 'unverifiable-truncated', r['status'], 0, 0, '', 0)
        return 0

    offs = r['offs']
    ops = sorted(set(d[o] for o in offs))
    opset_hash = hashlib.sha1(bytes(ops)).hexdigest()[:12]

    table = ljconv.load_table(args.table)
    missing = sorted(set(ops) - set(table))
    if missing:
        emit('no', 'unmapped-opcode', '0x' + ',0x'.join('%02x' % k for k in missing),
             0, 0, opset_hash, len(ops))
        return 0

    try:
        out, nins, fixhits = ljconv.transcode_stock(d, table)
    except ValueError as e:
        emit('no', 'unmapped-opcode', str(e), 0, 0, opset_hash, len(ops))
        return 0

    stock_path = os.path.join(args.outdir, flat + '.stock.luac')
    src_path = os.path.join(args.outdir, flat + '.src.lua')
    open(stock_path, 'wb').write(out)

    sys.path.insert(0, LJD)
    sys.argv = ['ljd', '-f', stock_path, '-o', src_path]
    try:
        runpy.run_path(os.path.join(LJD, 'main.py'), run_name='__main__')
        verdict, detail = 'ok', ''
    except SystemExit as e:
        if e.code in (0, None):
            verdict, detail = 'ok', ''
        else:
            verdict, detail = 'assert', 'ljd exited %r' % (e.code,)
    except RecursionError:
        verdict, detail = 'assert', 'RecursionError'
    except Exception as e:
        tb = sys.exc_info()[2]
        verdict, detail = 'assert', '%s at %s' % (type(e).__name__, ljd_frame(tb))

    emit('yes', verdict, detail, nins, fixhits, opset_hash, len(ops))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
