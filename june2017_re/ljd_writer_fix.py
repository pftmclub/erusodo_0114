"""ljd printer fix: long-bracket strings that close on their own content (JUNE_SKILL_SYSTEM_PHASES.md Phase S1).

The vendored ljd prints a string constant with more than two newlines as

    [[ + "\\n" + value + ]]

(elstools/vendor/ljd/ljd/lua/writer.py, Visitor.visit_constant) and never looks at the value. Three
values do not survive that:

  1. the value contains "]]": the long string closes early. June's NewSkillTempletVer2(_LA).lua
     (data036) has skill descriptions in combo notation, "Dragon Stance 1: Dash [X] or Activate with
     XX<down>X[X]]" -> printed "...[X]]]," and luajit stops with "'}' expected ... near ']'"
     (RANOX_LUA_DECOMPILE_BUG.md, "Residual ljd defects (Phase 8)");
  2. the value ends in "]" (or "]=" at a higher level): the closing bracket starts one byte early and
     leaves a stray "]" behind;
  3. the value contains "\\r": a long string folds "\\r", "\\r\\n" and "\\n\\r" to "\\n", and ljd writes
     its output in text mode, so every "\\n" is already "\\r\\n" on disk. The value comes back changed
     with no error at all.

The fix, as Visitor.visit_constant: for 1 and 2 pick the smallest level n >= 1 whose "]" + "="*n + "]"
closes exactly at the end of the value ([==[ ... ]==]); for 3 use ljd's own quoted form, which escapes
"\\r" and "\\n". Every other string prints byte-for-byte as before, so a decompile that was already
correct gets the same text (the self-test checks this).

This module does not edit ljd. ljd_fixed.install_fix() calls install() after the slotworks fix, so
every caller of ljd_fixed (ljd_jun2017.py, ljdiff --fixed, tools/luasrc.py decompile / adopt-june
--source) gets it; ljd_jun2017.py --unfixed still runs plain ljd. install() refuses unless writer.py
is byte-for-byte the file this was written against (WRITER_SHA256; both vendored copies, checked
2026-10-17).

    python ljd_writer_fix.py --selftest     # luajit round trip of the edge cases + "unchanged" check
"""
import hashlib
import io
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
LJD = os.path.normpath(os.path.join(HERE, '..', 'elstools', 'vendor', 'ljd'))
WRITER = os.path.join(LJD, 'ljd', 'lua', 'writer.py')
WRITER_SHA256 = '001f3364112556a1d5dea4a46bd4da16b2f4b3fe034c13698422276774a39ac6'
LUAJIT = os.path.normpath(os.path.join(HERE, '..', 'elstools', 'vendor', 'luajit', 'luajit.exe'))

STATS = {'leveled': 0, 'requoted': 0}
_ORIG = []


def _sha256(path):
    return hashlib.sha256(open(path, 'rb').read()).hexdigest()


def long_level(value):
    """Smallest n such that "]" + "="*n + "]" appended to value closes exactly at the end of value."""
    n = 0
    while True:
        close = ']' + '=' * n + ']'
        if (value + close).find(close) == len(value):
            return n
        n += 1


def install():
    """Replace ljd.lua.writer.Visitor.visit_constant (once per process). Returns the writer module."""
    sha = _sha256(WRITER)
    if sha != WRITER_SHA256:
        raise SystemExit('ljd_writer_fix: refusing to install. %s has SHA-256 %s; the fix was written against %s.'
                         ' Re-derive the fix against the new file before changing WRITER_SHA256.'
                         % (WRITER, sha, WRITER_SHA256))
    if LJD not in [os.path.normpath(os.path.abspath(p)) for p in sys.path]:
        sys.path.insert(0, LJD)
    import ljd.lua.writer as writer
    if os.path.normcase(os.path.abspath(writer.__file__)) != os.path.normcase(WRITER):
        raise SystemExit('ljd_writer_fix: refusing to install. ljd.lua.writer was imported from %s, not %s'
                         % (writer.__file__, WRITER))
    if _ORIG:
        return writer
    nodes = writer.nodes
    orig = writer.Visitor.visit_constant
    _ORIG.append(orig)

    def visit_constant(self, node):
        if node.type != nodes.Constant.T_STRING or node.value.count('\n') <= 2:
            return orig(self, node)
        value = node.value
        if '\r' in value:
            # Defect 3: ljd's quoted branch, verbatim (writer.py visit_constant's else branch).
            STATS['requoted'] += 1
            text = value
            text = text.replace("\\", "\\\\")
            text = text.replace("\t", "\\t")
            text = text.replace("\n", "\\n")
            text = text.replace("\r", "\\r")
            text = text.replace("\"", "\\\"")
            self._write('"' + text + '"')
            return
        n = long_level(value)
        if n == 0:
            return orig(self, node)
        # Defects 1 and 2: the same three writes as ljd, at level n.
        STATS['leveled'] += 1
        self._write('[' + '=' * n + '[')
        self._write("\n")
        self._write(value)
        self._write(']' + '=' * n + ']')

    writer.Visitor.visit_constant = visit_constant
    return writer


def stats_line():
    return 'writer fix active (%s): leveled=%d requoted=%d' % (WRITER_SHA256[:8], STATS['leveled'], STATS['requoted'])


# ---------------------------------------------------------------- self-test

CASES = [
    ('short', 'one line'),
    ('two newlines', 'a\nb\nc'),
    ('plain long', 'line 1\nline 2\nline 3\nline 4'),
    ('leading newline', '\nfirst\nsecond\nthird'),
    ('June combo text', "Swing the spear.\n\n#CFFC000Energy Gain#CX\n-Gain 1 Energy\n"
                        "Dragon Stance 1: Dash [X] or Activate with XX↓X[X]]"),
    (']] in the middle', 'a\nb\nc]]d\ne'),
    ('ends in ]', 'a\nb\nc\nd]'),
    (']] and ]=] both', 'a\nb]]\nc]=]\nd'),
    ('ends in ]=', 'a\nb\nc\nd]]x]='),
    ('CRLF inside', 'a\r\nb\r\nc\r\nd'),
    ('lone CR', 'a\nb\nc\nd\re'),
    ('CR and ]]', 'a\nb\nc\nd]]\r'),
    ('quote and backslash', 'a\n"b"\n\\c\nd\te'),
]


def _render(visit, value):
    """-> the source text ljd writes for one string constant, through the real queue processor."""
    import ljd.lua.writer as writer
    v = writer.Visitor()
    node = writer.nodes.Constant()
    node.type = writer.nodes.Constant.T_STRING
    node.value = value
    visit(v, node)
    out = io.StringIO()
    writer._process_queue(out, v.print_queue, None)
    return out.getvalue()


def _luajit_values(exprs):
    """Run every expression through luajit, as ljd's output reaches it (text mode, so "\\n" -> "\\r\\n").
    -> list of byte strings, or None per expression that does not compile."""
    got = []
    with tempfile.TemporaryDirectory() as d:
        for i, expr in enumerate(exprs):
            src = os.path.join(d, 's%d.lua' % i)
            res = os.path.join(d, 'r%d.bin' % i)
            with open(src, 'w', encoding='utf-8') as f:  # text mode, like ljd main.py
                f.write('local f = assert(io.open(arg[1], "wb"))\nf:write(%s)\nf:close()\n' % expr)
            r = subprocess.run([LUAJIT, src, res], capture_output=True, stdin=subprocess.DEVNULL, timeout=60)
            got.append(open(res, 'rb').read() if r.returncode == 0 and os.path.exists(res) else None)
    return got


def selftest():
    if not os.path.isfile(LUAJIT):
        raise SystemExit('ljd_writer_fix: luajit.exe not found at %s' % LUAJIT)
    writer = install()
    fixed = writer.Visitor.visit_constant
    orig = _ORIG[0]
    new_src = [_render(fixed, v) for _, v in CASES]
    old_src = [_render(orig, v) for _, v in CASES]
    new_val = _luajit_values(new_src)
    old_val = _luajit_values(old_src)
    fails, unchanged, repaired = [], 0, 0
    for (name, value), ns, os_, nv, ov in zip(CASES, new_src, old_src, new_val, old_val):
        want = value.encode('utf-8')
        old_ok = ov == want
        if nv != want:
            fails.append('%s: fixed printer gives %r for %r (source %r)' % (name, nv, want, ns))
        if old_ok and ns != os_:
            fails.append('%s: ljd was already right here, but the fixed printer changed its text (%r -> %r)' % (name, os_, ns))
        if old_ok:
            unchanged += 1
        else:
            repaired += 1
    # The hook must be needed: the cases it exists for really fail under plain ljd.
    for name in (']] in the middle', 'June combo text', 'ends in ]', 'CRLF inside', 'lone CR'):
        i = [c[0] for c in CASES].index(name)
        if old_val[i] == CASES[i][1].encode('utf-8'):
            fails.append('%s: plain ljd round-trips this one, so the case proves nothing' % name)
    for f in fails:
        print('FAIL ' + f)
    print('ljd_writer_fix selftest %s: %d case(s), %d already right in plain ljd (text unchanged), %d repaired; %s'
          % ('FAIL' if fails else 'PASS', len(CASES), unchanged, repaired, stats_line()))
    return 1 if fails else 0


if __name__ == '__main__':
    for _s in (sys.stdout, sys.stderr):
        try:
            _s.reconfigure(encoding='utf-8', errors='backslashreplace')
        except Exception:
            pass
    if sys.argv[1:] != ['--selftest']:
        raise SystemExit('usage: python ljd_writer_fix.py --selftest')
    sys.exit(selftest())
