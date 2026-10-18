"""Which preprocessor flags X2ServerProtocol's X2TOOL build (the client's one) sees, and what EQUIPPED_SKILL_SLOT_COUNT expands to there.

    python flagprobe_sp.py FLAG [FLAG ...]    # run from june2017_re/; prints ON/OFF per flag, then the macro value

S5 gate (JUNE_SKILL_SYSTEM_PHASES.md Rule 2): a flag that guards CommonPacket.h is defined in KncWX2Server/Common/ServerDefine.h too, and
both projects must see it before the struct sizes can agree. june2017_re/flagprobe.py is the X2Lib half. The probe includes the project's
StdAfx.h and CommonPacket.h; the flag names go in string literals so an empty macro still shows. Output: pp/_flag_probe_sp.i (gitignored).
"""
import os, re, subprocess, sys
sys.path.insert(0, os.getcwd())
import census as C

flags = sys.argv[1:]
body = ['#include "StdAfx.h"', '#include "CommonPacket.h"']
for f in flags:
    body += ['#ifdef %s' % f, 'FLAGON "%s"' % f, '#else', 'FLAGOFF "%s"' % f, '#endif']
body += ['SLOTCOUNT_IS EQUIPPED_SKILL_SLOT_COUNT']
proj = os.path.join(C.TRUNK, 'X2ServerProtocol')
p = os.path.join(proj, '_flag_probe_sp.cpp')
open(p, 'w', newline='\r\n').write('\n'.join(body) + '\n')
out = os.path.join(os.getcwd(), 'pp', '_flag_probe_sp.i')
os.makedirs(os.path.dirname(out), exist_ok=True)
inc = ['./', './FSM', './Socket', '../KncWX2Server/Common'] + [os.path.join(C.TRUNK, s) for s in
      (r'Libs\ExternalLib\DXSDK\Include', r'Libs\ExternalLib\Boost\boost_1_44_0', r'KNCSDK\Include_2010', r'Libs\ExternalLib\sqlite3')]
defs = ['X2TOOL', 'WIN32', 'NDEBUG', '_LIB', '_USE_32BIT_TIME_T', '_HAS_ITERATOR_DEBUGGING=0', 'SERV_IRUHADEV_OFFLINE', 'UNICODE', '_UNICODE']
cmd = [C.VS + r'\VC\bin\cl.exe', '/nologo', '/P', '/Zm600', '/Zc:wchar_t', '/Fi' + out] + ['/I' + i for i in inc] + ['/D' + d for d in defs] + [p]
r = subprocess.run(cmd, cwd=proj, env=C.cl_env(), capture_output=True)
os.remove(p)
if not os.path.exists(out):
    sys.exit('cl /P failed:\n' + r.stdout.decode('latin1', 'replace')[-3000:])
t = open(out, 'rb').read().decode('latin1')
for m in re.finditer(r'FLAG(ON|OFF) "(\w+)"', t):
    print(m.group(1), m.group(2))
for m in re.finditer(r'SLOTCOUNT_IS (\S+)', t):
    print('EQUIPPED_SKILL_SLOT_COUNT =', m.group(1))
