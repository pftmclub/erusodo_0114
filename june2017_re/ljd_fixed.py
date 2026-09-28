"""ljd with the slotworks dropped-assignment fix (Phase 4 of RANOX_LUA_DECOMPILE_FIX_PLAN.md).

The fix itself is ljd_slotworks_fix.py, written as slotworks.py code. This
script installs it into the vendored ljd at run time, without editing any ljd
file, the same way ljd_jun2017.py installs its opcode correction:

    install_fix()   execs ljd_slotworks_fix.py inside ljd.ast.slotworks, after
                    checking that the vendored slotworks.py is byte-for-byte
                    the file the fix was written against (SLOTWORKS_SHA256).
                    Any other slotworks.py is refused, loudly.

It also writes the same change as a unified diff, ljd_slotworks.patch, for
the two vendored ljd copies (elstools/vendor/ljd and the one elstools.exe
runs, see elstools/elstools_config.json). Applying that is optional and the
user's call; nothing here applies it.

Usage:
    python ljd_fixed.py -f FILE.luac -o FILE.src.lua [more main.py args]
        ljd's main.py with the fix installed: the entry point for stock
        bytecode (mar-2014 0x81 files with the 1b 4c 4a 01 header swapped in,
        what ljd_oracle/run_oracle.py and ljdiff/ljdiff.py feed ljd; both take
        --fixed to use this). Prints one "ljd_fixed: ..." summary line to stderr.
    python ljd_fixed.py --write-patch    regenerate ljd_slotworks.patch
    python ljd_fixed.py --check-patch    exit 1 if ljd_slotworks.patch is stale
June 0x82 files: `python ljd_jun2017.py --fixed FILE.lua`.

LJD_FIX_LOG=<file> appends one line per vetoed inline (and per exempt one) to
<file>: the caller, the function, the slot, the kept assignment and the read
that needed it.
"""
import ast as pyast
import difflib
import hashlib
import os
import runpy
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
LJD = os.path.normpath(os.path.join(HERE, '..', 'elstools', 'vendor', 'ljd'))
SLOTWORKS = os.path.join(LJD, 'ljd', 'ast', 'slotworks.py')
# elstools/vendor/ljd/ljd/ast/slotworks.py as vendored (byte-identical to
# gitlab.com/znixian/luajit-decompiler master and to the second vendored copy,
# checked 2026-09-28). The fix was written and verified against this file only.
SLOTWORKS_SHA256 = '464b2c45bc6859947904a4417b2d3926543441a5eb10be59084a10230041675c'
FIX = os.path.join(HERE, 'ljd_slotworks_fix.py')
PATCH = os.path.join(HERE, 'ljd_slotworks.patch')
MARKER = '# ==== ljd_fixed: slotworks fix.'
PATCHED_SHA_PREFIX = '# patched slotworks.py SHA-256: '


def _fix_text():
    """-> (line index the fix code starts at, the fix code): everything after MARKER."""
    lines = open(FIX, encoding='utf-8', newline='').read().split('\n')
    for i, line in enumerate(lines):
        if line.startswith(MARKER):
            return i + 1, '\n'.join(lines[i + 1:])
    raise SystemExit('ljd_fixed: %s has no %r line' % (FIX, MARKER))


def _sha256(path):
    return hashlib.sha256(open(path, 'rb').read()).hexdigest()


def install_fix():
    """Install the fix into ljd.ast.slotworks (once per process) and return the module.
    Refuses (SystemExit) unless the vendored slotworks.py is the one it was written against."""
    sha = _sha256(SLOTWORKS)
    if sha != SLOTWORKS_SHA256:
        patched = ''
        if os.path.exists(PATCH):
            for line in open(PATCH, encoding='utf-8'):
                if line.startswith(PATCHED_SHA_PREFIX) and line[len(PATCHED_SHA_PREFIX):].strip() == sha:
                    patched = (' It matches ljd_slotworks.patch applied: the fix is already in'
                               ' slotworks.py, so run ljd without --fixed.')
        raise SystemExit('ljd_fixed: refusing to install. %s has SHA-256 %s; the fix was written against %s.%s'
                         ' Otherwise re-derive the fix against the new file before changing SLOTWORKS_SHA256.'
                         % (SLOTWORKS, sha, SLOTWORKS_SHA256, patched))
    if LJD not in [os.path.normpath(os.path.abspath(p)) for p in sys.path]:
        sys.path.insert(0, LJD)
    import ljd.ast.slotworks as sw
    if os.path.normcase(os.path.abspath(sw.__file__)) != os.path.normcase(SLOTWORKS):
        raise SystemExit('ljd_fixed: refusing to install. ljd.ast.slotworks was imported from %s, not %s'
                         % (sw.__file__, SLOTWORKS))
    if getattr(sw, '_LJDFIX_STATS', None) is None:
        start, text = _fix_text()
        exec(compile('\n' * start + text, FIX, 'exec'), sw.__dict__)
    return sw


def stats_line():
    import ljd.ast.slotworks as sw
    s = sw._LJDFIX_STATS
    return ('slotworks fix active (%s): vetoed=%d exempt=%d andor=%d checked=%d unlocated=%d stale_target=%d'
            ' unknown_node=%d'
            % (SLOTWORKS_SHA256[:8], s['vetoed'], s['exempt'], s['andor'], s['checked'], s['unlocated'],
               s['stale_target'], s['unknown_node']))


# ---------------------------------------------------------------- the patch

def patched_slotworks():
    """slotworks.py with the fix spliced in: each function the fix redefines replaces
    the original in place, and everything else in the fix is appended at the end."""
    orig = open(SLOTWORKS, encoding='utf-8', newline='').read()
    _, text = _fix_text()
    olines = orig.split('\n')
    flines = text.split('\n')
    odefs = {n.name: n for n in pyast.parse(orig).body if isinstance(n, pyast.FunctionDef)}
    replaced, appended = {}, []
    prev_end = 0
    for n in pyast.parse(text).body:
        item = flines[prev_end:n.end_lineno]  # the node plus the comments and blank lines above it
        prev_end = n.end_lineno
        if isinstance(n, pyast.FunctionDef) and n.name in odefs:
            if n.decorator_list or odefs[n.name].decorator_list:
                raise SystemExit('ljd_fixed: decorated function %s: splice by hand' % n.name)
            while item and not item[0].strip():
                del item[0]
            replaced[n.name] = item
        else:
            appended.append(item)
    out, i = [], 0
    for name in sorted(replaced, key=lambda k: odefs[k].lineno):
        out += olines[i:odefs[name].lineno - 1] + replaced[name]
        i = odefs[name].end_lineno
    out += olines[i:]
    while out and not out[-1].strip():
        out.pop()
    tail = [l for item in appended for l in item]
    while tail and not tail[0].strip():
        del tail[0]
    out += ['', '', '# ==== ljd_fixed: slotworks fix (june2017_re/ljd_slotworks_fix.py in the ProjectX2 trunk) ====',
            ''] + tail
    return '\n'.join(out).rstrip('\n') + '\n'


def patch_text():
    orig = open(SLOTWORKS, encoding='utf-8', newline='').read()
    new = patched_slotworks()
    diff = difflib.unified_diff(orig.splitlines(True), new.splitlines(True),
                                'a/ljd/ast/slotworks.py', 'b/ljd/ast/slotworks.py', n=3)
    header = [
        '# ljd slotworks dropped-assignment fix (RANOX_LUA_DECOMPILE_FIX_PLAN.md Phase 4).',
        '# Generated by june2017_re/ljd_fixed.py --write-patch from ljd_slotworks_fix.py; do not hand-edit.',
        '# The same change june2017_re/ljd_fixed.py installs at run time. Applying it is optional.',
        '# Apply from an ljd root (the directory holding main.py), e.g. elstools/vendor/ljd:',
        '#     patch -p1 < ljd_slotworks.patch        (or: git apply ljd_slotworks.patch)',
        '# original slotworks.py SHA-256: ' + SLOTWORKS_SHA256,
        PATCHED_SHA_PREFIX + hashlib.sha256(new.encode('utf-8')).hexdigest(),
    ]
    return '\n'.join(header) + '\n' + ''.join(diff)


def main(argv):
    if argv[:1] in (['--write-patch'], ['--check-patch']):
        if _sha256(SLOTWORKS) != SLOTWORKS_SHA256:
            raise SystemExit('ljd_fixed: %s is not the file the fix was written against' % SLOTWORKS)
        text = patch_text()
        if argv[0] == '--write-patch':
            open(PATCH, 'w', encoding='utf-8', newline='\n').write(text)
            print('wrote %s' % PATCH)
            return 0
        current = open(PATCH, encoding='utf-8', newline='').read() if os.path.exists(PATCH) else ''
        print('%s is %s' % (PATCH, 'current' if current == text else 'STALE - run --write-patch'))
        return 0 if current == text else 1

    install_fix()
    main_py = os.path.join(LJD, 'main.py')
    sys.argv = [main_py] + argv
    rc = 0
    try:
        runpy.run_path(main_py, run_name='__main__')
    except SystemExit as e:
        if e.code is None or isinstance(e.code, int):
            rc = e.code or 0
        else:
            sys.stderr.write('%s\n' % e.code)
            rc = 1
    # Only on success: the harnesses report a failure by stderr's last line, which must
    # stay ljd's own error.
    if rc == 0:
        sys.stderr.write('ljd_fixed: %s\n' % stats_line())
    return rc


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
