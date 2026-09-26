"""Which preprocessor flags X2Lib's US_SERVICE build defines (Phase 4a stage 2).

    python flagprobe.py FLAG [FLAG ...]    # run from june2017_re/; prints ON/OFF per flag

Preprocesses a probe that includes X2Lib's stdafx.h, the census.py way; the flag names go
in string literals, so an empty macro still shows. Output lands in pp/ (gitignored).
"""
import os, re, subprocess, sys
sys.path.insert(0, os.getcwd())
import census as C
flags = sys.argv[1:]
body = ['#include "stdafx.h"']
for f in flags:
    body += ['#ifdef %s' % f, 'FLAGON "%s"' % f, '#else', 'FLAGOFF "%s"' % f, '#endif']
p = os.path.join(C.TRUNK, 'X2Lib', '_flag_probe2.cpp')
open(p, 'w', newline='\r\n').write('\n'.join(body) + '\n')
out = os.path.join(os.getcwd(), 'pp', '_flag_probe2.i')
r = subprocess.run(C.cl_cmd(p, out), cwd=os.path.join(C.TRUNK, 'X2Lib'), env=C.cl_env(), capture_output=True)
os.remove(p)
t = open(out, 'rb').read().decode('latin1')
for m in re.finditer(r'FLAG(ON|OFF) "(\w+)"', t): print(m.group(1), m.group(2))
