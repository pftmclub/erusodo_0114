#!/usr/bin/env python3
"""Live source-line ranges of one file, read from a cl /E /C preprocessor output.

Usage:  python ch0_live_lines.py <file.i> <source-name> [--src <path>] [--lines]
  <source-name>  basename or path suffix of the file, e.g. X2GuEve.cpp or X2GuEve.h
  --src          the source file itself (default: looked up under X2Lib/ then KTDXLIB/)
  --lines        also print one 'N<TAB>L' (live) or 'N<TAB>-' (not) row per source line

A source line is live when the preprocessor emitted non-blank text for it (build the .i
with -C so live comments count). Blank lines and preprocessor-directive lines are neutral:
they join the live range around them and are trimmed at its edges.
As a module: live_set(i_path, name) -> set of live line numbers; ranges(live, src_lines).
Make the .i from X2Lib/ with VS2010 cl.exe: -nologo -E -C -Zm600, X2Lib_2010.vcxproj US_SERVICE include dirs,
-DWIN32 -DNDEBUG -D_LIB -D_USE_32BIT_TIME_T -D_IN_HOUSE_ -DSERV_COUNTRY_US -DCLIENT_COUNTRY_US
-D_HAS_ITERATOR_DEBUGGING=0 -D_CONVERT_VS_2010 -DUNICODE -D_UNICODE, stdout redirected to the .i.
"""
import os
import re
import sys

LINE_RE = re.compile(r'^#line\s+(\d+)\s+"(.*)"')


def norm(p):
    return p.replace('\\\\', '/').replace('\\', '/').lower()


def matches(path, name):
    p, n = norm(path), norm(name)
    return p == n or p.endswith('/' + n)


def live_set(i_path, name):
    live = set()
    cur_file, cur_line = None, 0
    with open(i_path, 'rb') as f:
        for raw in f:
            m = LINE_RE.match(raw.decode('latin-1'))
            if m:
                cur_line, cur_file = int(m.group(1)), m.group(2)
                continue
            if cur_file is not None and matches(cur_file, name) and raw.strip():
                live.add(cur_line)
            cur_line += 1
    return live


def neutral_flags(src_lines):
    """True per line (index 0 = line 1) for blank lines and preprocessor directive lines."""
    out, cont = [], False
    for ln in src_lines:
        s = ln.strip()
        if cont:
            out.append(True)
            cont = s.endswith(b'\\')
        elif s.startswith(b'#'):
            out.append(True)
            cont = s.endswith(b'\\')
        else:
            out.append(not s)
    return out


def ranges(live, src_lines=None):
    if not live:
        return []
    n = max(live)
    neutral = neutral_flags(src_lines) if src_lines else None
    if src_lines:
        n = max(n, len(src_lines))
    res, start, last = [], None, None
    for i in range(1, n + 1):
        if i in live:
            if start is None:
                start = i
            last = i
        elif neutral is not None and i <= len(neutral) and neutral[i - 1] and start is not None:
            continue  # neutral gap: keep the run open
        else:
            if start is not None:
                res.append((start, last))
                start = None
    if start is not None:
        res.append((start, last))
    return res


def find_src(name):
    here = os.path.dirname(os.path.abspath(__file__))
    trunk = os.path.abspath(os.path.join(here, '..', '..'))
    for d in ('X2Lib', 'KTDXLIB', 'KncWX2Server/Common'):
        p = os.path.join(trunk, d, name)
        if os.path.isfile(p):
            return p
    return None


def main(argv):
    args = [a for a in argv[1:] if not a.startswith('--')]
    src = None
    if '--src' in argv:
        src = argv[argv.index('--src') + 1]
        args = [a for a in args if a != src]
    if len(args) < 2:
        print(__doc__)
        return 2
    i_path, name = args[0], args[1]
    live = live_set(i_path, name)
    src = src or find_src(name)
    src_lines = open(src, 'rb').read().split(b'\n') if src else None
    if src_lines and src_lines[-1] == b'':
        src_lines.pop()
    rs = ranges(live, src_lines)
    print('# file=%s source=%s total_lines=%s live_code_lines=%d ranges=%d' % (
        name, src or 'not found', len(src_lines) if src_lines else 'unknown', len(live), len(rs)))
    for a, b in rs:
        print('%d-%d' % (a, b) if a != b else '%d' % a)
    if '--lines' in argv:
        n = len(src_lines) if src_lines else max(live)
        for i in range(1, n + 1):
            print('%d\t%s' % (i, 'L' if i in live else '-'))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
