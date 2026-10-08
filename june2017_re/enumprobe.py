"""March enum / constant-expression probe (Effect C3d-1).

    python enumprobe.py ITEMS        # run from june2017_re/; ITEMS has one C++ constant expression per line,
                                     # e.g. CX2GUUser::USI_END or CX2GUChung::CSI_A_CIP_ARTILLERY_NOVA_FIRE

Same trick as offsetprobe.py (compile-only /Zs under X2Lib's US_SERVICE defines, C2440 names 'char (*)[N]'),
with the nine class headers included, so class state enums resolve as the compiler sees them (nested #ifdefs
included). Prints each expression's value. Values must be >= -1.
"""
import os, re, subprocess, sys
sys.path.insert(0, os.getcwd())
import census as C
incs = ['X2GUElsword_SwordMan.h','X2GUArme_VioletMage.h','X2GULire_ElvenRanger.h','X2GURaven.h','X2GUEve.h','X2GUChung.h','X2GUAra.h','X2GUEL.h','X2GUAdd.h']
items = [l.strip() for l in open(sys.argv[1]).read().split('\n') if l.strip()]
src = ['#define private public', '#define protected public', '#include "stdafx.h"'] + ['#include "%s"' % h for h in incs] + ['static int g_probe = 0;']
base = len(src)
for i, e in enumerate(items):
    src.append('char (*probe_%d)[(%s) + 2] = &g_probe;' % (i, e))
p = os.path.join(C.TRUNK, 'X2Lib', '_enum_probe_c3d1.cpp')
open(p, 'w', newline='\r\n').write('\n'.join(src) + '\n')
cmd = [a for a in C.cl_cmd(p, 'x.i') if not a.startswith('/Fi') and a != '/P'] + ['/c', '/Zs', '/EHsc']
r = subprocess.run(cmd, cwd=os.path.join(C.TRUNK, 'X2Lib'), env=C.cl_env(), capture_output=True)
os.remove(p)
out = r.stdout.decode('latin1')
for m in re.finditer(r'_enum_probe_c3d1\.cpp\((\d+)\) : error C2440.*?char \(\*\)\[(\d+)\]', out):
    ln = int(m.group(1)); n = int(m.group(2)) - 2
    print('%-60s %d (0x%x)' % (items[ln - base - 1], n, n))
if 'C2440' not in out: print(out[-4000:])
