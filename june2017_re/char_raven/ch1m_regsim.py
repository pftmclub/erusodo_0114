"""Per-class path simulation of Raven's registrars (char-raven CH1m): Elsword's ch1m_regsim.py with Raven's class tests.

    python june2017_re/char_raven/ch1m_regsim.py <decompile_function output of a registrar> <fn>   # run from Trunk

Joins Ghidra's split `if (... &&` / `{` lines first, then walks the decompile once per Raven class (4 16 17 25 106 107 115).
Prints fn, class, line, state id, table, callbacks per registration (ch1m_reg_by_class.tsv is this output for the five registrars).
The one non-class test, InitState's `RSI_RWT_LOSE2` compare @261 ( iVar5 == 0 ), is taken as false and written by hand ( LOSE2 FrameMove ).
"""
import os, re, sys
T = os.getcwd()
sys.path.insert(0, os.path.join(T, 'june2017_re'))
sys.stdout.reconfigure(encoding='utf-8')
import charport_regsim as rs
rs.CLASSEXPR = [r'\*\(undefined4 \*\)\(local_68 \+ 0x38\)', r'\*\(int \*\)\(local_68 \+ 0x38\)', r'\*\(undefined4 \*\)\(uVar4 \+ 0x38\)',
                r'\*\(int \*\)\(uVar4 \+ 0x38\)', r'iVar5']
orig = rs.evalcond


def ev(cond, cls, line):
    c = cond.strip()
    if re.fullmatch(r'\((piVar\d+|local_10|piStack_\w+) != \(int \*\)0x0\)', c):
        return False    # shared_ptr release, no registration behind it
    if c == '(iVar5 == 0)':
        return False    # LOSE2 compare @261 or a release count: neither registers
    g = c
    for e in rs.CLASSEXPR:
        g = re.sub(e, 'CLS', g)
    if re.fullmatch(r'[CLS0-9a-fx()=!|& ]+', g) and 'CLS' in g:
        return bool(eval(g.replace('||', ' or ').replace('&&', ' and '), {'CLS': cls}))
    return orig(cond, cls, line)


rs.evalcond = ev
L = open(sys.argv[1], encoding='utf-8', errors='replace').read().split('\n')
for i, l in enumerate(L):
    if l.rstrip().endswith('&&') and l.lstrip().startswith('if ('):
        L[i] = l.rstrip() + ' ' + L[i + 1].strip()
        L[i + 1] = ''
for i, l in enumerate(L):
    if l.lstrip().startswith('if (') and not l.rstrip().endswith('{') and ' goto ' not in l:
        j = i + 1
        while L[j].strip() == '':
            j += 1
        if L[j].strip() == '{':
            L[i] = l.rstrip() + ' {'
            L[j] = ''
ops, labels = rs.compile_(L)
for cls in [4, 16, 17, 25, 106, 107, 115]:
    for r in rs.run(L, ops, labels, cls):
        if r[0] == 'REG':
            print('\t'.join([sys.argv[2], str(cls)] + [str(x) for x in r[1:]]))
for u in sorted(set(rs.UNKNOWN)):
    print('UNKNOWN COND', u)
