r"""char-rena CH0: build ch0_march_live_states.tsv (March's LIVE state registrations in X2GULire_ElvenRanger.cpp). Copy of char_raven/ch0_march_live_states.py.

    cl /E /C on X2GULire_ElvenRanger.cpp (VS2010 at D:\Program Files\VS\Microsoft Visual Studio 10.0\VC\bin\cl.exe, from X2Lib/, in PowerShell
    with PATH = VC\bin;Common7\IDE and INCLUDE = Libs\ExternalLib\DXSDK\Include;Boost\boost_1_44_0;KNCSDK\Include_2010;
    KOGGamePerformanceCheck\include;freetype-2.4.11\include;XTRAP\Client;Steamworks;VC\include;VC\atlmfc\include;Windows SDK v7.0A\Include,
    the -I / -D list of ch0_live_lines.py's docstring, `cmd /c "cl ... > els.i"`) -> els.i
    python june2017_re/char_rena/ch0_march_live_states.py <lire.i>   (s3_no_state.py --family X2GULire_ElvenRanger.cpp --preprocess --keep-i <lire.i>)

Columns: source line of a live MakeTableReference, the table name (<var> when it is a variable), and the live SET_CB_FUNC lines up to
the next MakeTableReference / m_StateList store as Slot=Class::Fn@line (liveness from the preprocessor output, never from reading #ifdefs).
"""
import sys, re, os
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'char_eve'))
import ch0_live_lines as L
live = L.live_set(sys.argv[1], 'X2GULire_ElvenRanger.cpp')
src = open(os.path.join(HERE, '..', '..', 'X2Lib', 'X2GULire_ElvenRanger.cpp'), 'rb').read().decode('latin-1').split('\n')
SL = {'StateInit': 'Init', 'StateStartFuture': 'StartFuture', 'StateStart': 'Start', 'OnFrameMoveFuture': 'FrameMoveFuture',
      'OnFrameMove': 'FrameMove', 'OnCameraMove': 'CameraMove', 'OnEventProcess': 'EventProcess', 'StateEndFuture': 'EndFuture',
      'StateEnd': 'End'}
out, tot = [], 0
prev = {}
for i, l in enumerate(src, 1):
    m = re.search(r'MakeTableReference\(\s*(.+?)\s*,\s*\w+\.stateID', l)
    if not m:
        continue
    tot += 1
    if i not in live:
        continue
    a = m.group(1)
    q = re.match(r'"(\w+)"', a)
    cbs = []
    for k in range(i + 1, min(i + 60, len(src) + 1)):
        s = src[k - 1]
        if 'MakeTableReference' in s or re.search(r'm_StateList\s*\[', s):
            break
        if k in live:
            for f in re.findall(r'\.\s*(\w+)\s*=\s*SET_CB_FUNC\(\s*(\w+)\s*,\s*(\w+)\s*\)', s):
                cbs.append('%s=%s::%s@%d' % (SL.get(f[0], f[0]), f[1], f[2], k))
            for f in re.findall(r'hyperStateData\.(\w+)\s*=\s*normalStateData\.(\w+)\s*;', s):
                if prev.get(SL.get(f[0], f[0])):
                    cbs.append('%s=%s(=normal)' % (SL.get(f[0], f[0]), prev[SL.get(f[0], f[0])]))
    prev = {c.split('=')[0]: c.split('=', 1)[1].split('(=')[0] for c in cbs}
    out.append('%d\t%s\t%s' % (i, q.group(1) if q else '<' + a.replace('.c_str()', '') + '>', ' '.join(cbs)))
open(os.path.join(HERE, 'ch0_march_live_states.tsv'), 'w', newline='\n', encoding='utf-8').write('\n'.join(out) + '\n')
print('MakeTableReference lines %d, live %d' % (tot, len(out)))
