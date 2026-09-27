"""Resumable per-file corpus runner. Phase 0 of RANOX_LUA_DECOMPILE_FIX_PLAN.md,
and every later sweep phase, reuses this.

Why per-file subprocesses: ljd (elstools/vendor/ljd) keeps module-level global
state, so ljd_jun2017.py already runs one file per interpreter. This module
generalizes that: it drives a WORKER script, one subprocess per (dir, name)
target, bounds each with a timeout, and appends one TSV row per file so a
killed or interrupted run resumes exactly where it left off.

Process rules this follows (see RANOX_LUA_DECOMPILE_FIX_PLAN.md "Starting
context"):
  - Never kill by image name -- other python.exe processes (MCP servers) run
    on this machine. The backstop kill is `taskkill /F /T /PID <pid>`, which
    is PID-scoped (the /T also kills any children THAT PID spawned, e.g. if a
    worker shells out to elstools.exe).
  - faulthandler: a WORKER is expected to arm its own via arm_faulthandler()
    below, so a hang dumps its stack to a per-file log before this runner's
    backstop timeout fires. That is what makes a `TIMEOUT` row diagnosable
    instead of just "it was slow".
  - Long runs are resumable: one TSV row appended per file, --resume skips
    (dir, name) pairs already present, and a dangling partial line (the
    process was killed mid-write) is dropped before resuming.

Usage:
    python corpus_run.py --worker WORKER.py --root ROOT --out OUT.tsv
        [--files FILES.tsv] [--glob PATTERN] [--timeout SECS] [--workers N]
        [--resume] [--budget-margin SECS] [-- WORKER_ARGS...]

--files FILES.tsv: one row per target, columns "dir\tname" (relative to
    --root). Header optional (a row that doesn't parse as two columns is
    skipped with a warning).
--glob PATTERN: alternative to --files. Matched under --root with
    glob.glob(root + '/' + PATTERN, recursive=True); dir/name are the match's
    dirname (relative to --root) and basename.

WORKER contract -- invoked as:
    python WORKER.py <absolute_input_path> --budget SECS [WORKER_ARGS...]
  - Call arm_faulthandler(budget, hang_log_path) near its own start.
  - Catch every exception ITSELF; print exactly one TSV line to stdout with
    the worker's own columns (free-form, but the first should be a status/
    verdict word) and exit 0. A worker that exits non-zero or prints nothing
    is recorded by this runner as CRASH, not as one of the worker's own
    verdicts -- that distinction matters: a verdict the worker names is a
    confirmed outcome, a CRASH is this runner noticing the worker didn't.

OUT.tsv columns: dir, name, size, wall_seconds, exit_code, timed_out, then
the worker's own stdout line split on tabs (ragged -- different workers may
emit different column counts; consumers should read by position from a
worker-specific offset, not assume a fixed total width).
"""
import argparse
import csv
import glob as glob_mod
import os
import subprocess
import sys
import threading
import time

HERE = os.path.dirname(os.path.abspath(__file__))


def arm_faulthandler(budget, hang_log_path, margin=5):
    """Call this near the top of a WORKER's own main(). If the worker is still
    running budget-margin seconds after this call, its own stack (every
    thread) is dumped to hang_log_path and the process exits -- so a hang is
    diagnosable from the log instead of only showing up as this runner's
    TIMEOUT/backstop-kill row. Returns the open file handle; keep a reference
    (module-level, or the caller's local) so it isn't garbage-collected."""
    import faulthandler
    fh = open(hang_log_path, 'w', encoding='utf-8')
    deadline = max(1, budget - margin)
    faulthandler.dump_traceback_later(deadline, exit=True, file=fh)
    return fh


def _kill_tree(pid):
    """PID-scoped, not image-name-scoped: only this process and whatever it
    spawned. Never `taskkill /IM python.exe` -- other python.exe processes
    (MCP servers) run on this machine."""
    subprocess.run(['taskkill', '/F', '/T', '/PID', str(pid)],
                   capture_output=True)


def _read_done(out_path):
    """(dir, name) keys already present in out_path. If the previous run was
    killed mid-write, its dangling partial last line is truncated OFF THE FILE
    ITSELF first -- not just skipped in memory -- so a resumed run's `open(...,
    'a')` appends a clean new row instead of gluing it onto the fragment."""
    done = set()
    if not os.path.exists(out_path) or os.path.getsize(out_path) == 0:
        return done
    with open(out_path, 'rb+') as fh:
        fh.seek(-1, os.SEEK_END)
        ends_in_newline = fh.read(1) == b'\n'
        if not ends_in_newline:
            fh.seek(0)
            data = fh.read()
            idx = data.rfind(b'\n')
            fh.seek(idx + 1 if idx >= 0 else 0)
            fh.truncate()
    with open(out_path, encoding='utf-8') as fh:
        lines = fh.readlines()
    for line in lines[1:] if lines and lines[0].startswith('dir\t') else lines:
        cols = line.rstrip('\n').split('\t')
        if len(cols) >= 2:
            done.add((cols[0], cols[1]))
    return done


def load_targets(root, files_tsv, glob_pattern):
    targets = []
    if files_tsv:
        with open(files_tsv, encoding='utf-8') as fh:
            for line in fh:
                cols = line.rstrip('\n').split('\t')
                if len(cols) < 2 or cols[0] == 'dir':
                    continue
                targets.append((cols[0], cols[1]))
    else:
        pat = os.path.join(root, glob_pattern)
        for p in glob_mod.glob(pat, recursive=True):
            if not os.path.isfile(p):
                continue
            rel = os.path.relpath(p, root)
            d, n = os.path.split(rel)
            targets.append((d, n))
    return targets


def run_one(worker, root, d, n, timeout, hang_dir, worker_args):
    path = os.path.join(root, d, n)
    size = os.path.getsize(path) if os.path.exists(path) else -1
    flat = ('%s__%s' % (d, n)).replace(os.sep, '_').replace('/', '_')
    hang_log = os.path.join(hang_dir, flat + '.hang.txt')
    err_log = os.path.join(hang_dir, flat + '.err.txt')
    cmd = [sys.executable, worker, path, '--budget', str(timeout),
           '--hang-dir', hang_dir] + worker_args
    t0 = time.time()
    exit_code = None
    timed_out = False
    stdout_line = ''
    stderr_text = ''
    try:
        p = subprocess.Popen(cmd, cwd=HERE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             text=True)
        try:
            out, err = p.communicate(timeout=timeout + 10)
            exit_code = p.returncode
            stdout_line = (out or '').strip().splitlines()[-1] if out and out.strip() else ''
            stderr_text = err or ''
        except subprocess.TimeoutExpired:
            # The backstop: in practice the WORKER's own faulthandler deadline
            # (budget - margin) fires and self-exits first, so this branch is
            # rarely the one that catches a real hang -- see the exit_code!=0
            # check below, which is what usually does.
            timed_out = True
            _kill_tree(p.pid)
            try:
                out, err = p.communicate(timeout=15)
                stdout_line = (out or '').strip().splitlines()[-1] if out and out.strip() else ''
                stderr_text = err or ''
            except Exception:
                pass
            exit_code = -1
    except Exception as e:
        exit_code = -2
        stdout_line = 'CRASH-TO-LAUNCH\t%r' % (e,)
    wall = time.time() - t0
    if exit_code != 0 and not stdout_line:
        # A hang log is created (empty) by arm_faulthandler() on every run, so
        # its mere existence proves nothing -- only a body starting "Timeout ("
        # means IT fired (self-exit, before this function's own backstop).
        hang_text = ''
        if os.path.exists(hang_log):
            try:
                hang_text = open(hang_log, encoding='utf-8', errors='replace').read()
            except Exception:
                pass
        if hang_text.strip().startswith('Timeout ('):
            timed_out = True
            stdout_line = 'TIMEOUT\t%s' % hang_log
        else:
            if stderr_text.strip():
                with open(err_log, 'w', encoding='utf-8', errors='replace') as fh:
                    fh.write(stderr_text)
                last_err = stderr_text.strip().splitlines()[-1]
                stdout_line = 'CRASH\t(exit %r) %s (full stderr: %s)' % (exit_code, last_err, err_log)
            else:
                stdout_line = 'CRASH\t(exit %r, no stdout or stderr)' % (exit_code,)
    extra_cols = stdout_line.split('\t') if stdout_line else ['CRASH']
    return [d, n, str(size), '%.2f' % wall, str(exit_code), str(timed_out)] + extra_cols


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument('--worker', required=True)
    ap.add_argument('--root', required=True)
    ap.add_argument('--out', required=True)
    ap.add_argument('--files')
    ap.add_argument('--glob')
    ap.add_argument('--timeout', type=float, default=60)
    ap.add_argument('--workers', type=int, default=4)
    ap.add_argument('--resume', action='store_true')
    ap.add_argument('--budget-margin', type=float, default=5)
    ap.add_argument('--hang-dir')
    args, worker_args = ap.parse_known_args(argv)
    if worker_args and worker_args[0] == '--':
        worker_args = worker_args[1:]

    if not args.files and not args.glob:
        raise SystemExit('need --files or --glob')

    hang_dir = args.hang_dir or os.path.dirname(os.path.abspath(args.out)) or '.'
    os.makedirs(hang_dir, exist_ok=True)

    targets = load_targets(args.root, args.files, args.glob)
    done = _read_done(args.out) if args.resume else set()
    todo = [(d, n) for d, n in targets if (d, n) not in done]
    print('targets: %d total, %d already done, %d to run' % (len(targets), len(done), len(todo)))

    lock = threading.Lock()
    out_fh = open(args.out, 'a' if args.resume else 'w', newline='', encoding='utf-8')
    writer = csv.writer(out_fh, delimiter='\t')
    if out_fh.tell() == 0:
        writer.writerow(['dir', 'name', 'size', 'wall_seconds', 'exit_code', 'timed_out', 'worker_cols...'])
        out_fh.flush()

    n_done = [0]
    t_start = time.time()

    def worker_task(d, n):
        row = run_one(args.worker, args.root, d, n, args.timeout, hang_dir, worker_args)
        with lock:
            writer.writerow(row)
            out_fh.flush()
            n_done[0] += 1
            if n_done[0] % 25 == 0 or n_done[0] == len(todo):
                elapsed = time.time() - t_start
                print('  %d/%d done (%.0fs elapsed, %.2fs/file avg)' %
                      (n_done[0], len(todo), elapsed, elapsed / max(1, n_done[0])))

    import concurrent.futures
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.workers) as ex:
        futs = [ex.submit(worker_task, d, n) for d, n in todo]
        for f in concurrent.futures.as_completed(futs):
            f.result()  # surface any unexpected exception from worker_task itself

    out_fh.close()
    print('done: %d file(s) run this session, out=%s' % (len(todo), args.out))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
