"""March offsetof / sizeof probe (Phase 4a stage 2).

    python offsetprobe.py ITEMS      # run from june2017_re/; ITEMS has one "Type|member" per line
                                     # ("Type|SIZEOF" for sizeof), e.g. CKTDGObject|m_Layer

Compile-only (/Zs) under X2Lib's US_SERVICE defines, with census.py's cl setup and
"#define private public". Each item becomes a pointer initialisation that fails with C2440;
the message names the type 'char (*)[N]', and N - 1 is the offset. No link, no exe.
"""
import os, re, subprocess, sys
sys.path.insert(0, os.getcwd())
import census as C
items = [l.split('|') for l in open(sys.argv[1]).read().split('\n') if l.strip()]
src = ['#define private public', '#define protected public', '#include "stdafx.h"',
       'template<size_t N> struct OffsetIs;', 'static int g_probe = 0;']
for i, (t, m) in enumerate(items):
    expr = 'sizeof(%s)' % t if m == 'SIZEOF' else '((size_t)&(((%s*)0x1000)->%s) - 0x1000)' % (t, m)
    src.append('char (*probe_%d)[%s + 1] = &g_probe; // %s::%s' % (i, expr, t, m))
p = os.path.join(C.TRUNK, 'X2Lib', '_offset_probe.cpp')
open(p, 'w', newline='\r\n').write('\n'.join(src) + '\n')
cmd = [a for a in C.cl_cmd(p, 'x.i') if not a.startswith('/Fi') and a != '/P'] + ['/c', '/Zs', '/EHsc']
r = subprocess.run(cmd, cwd=os.path.join(C.TRUNK, 'X2Lib'), env=C.cl_env(), capture_output=True)
os.remove(p)
out = r.stdout.decode('latin1')
for m in re.finditer(r'_offset_probe\.cpp\((\d+)\) : error C2440.*?char \(\*\)\[(\d+)\]', out):
    ln = int(m.group(1)); n = int(m.group(2)) - 1
    t, mm = items[ln - 6]
    print('%-55s %-28s 0x%x' % (t, mm, n))
if 'C2440' not in out: print(out[-4000:])
