"""Per-class path simulation of Eve's registrars (char-eve CH1m): the pilot's charport_regsim.py with Eve's class tests.

    python june2017_re/char_eve/ch1m_regsim.py <decompile_function output of 329ba9c0 or 329b8030> <fn>   # run from Trunk

Joins Ghidra's split `if (... &&` / `{` lines first, then walks the decompile once per Eve class (5 18 19 26 108 109 116).
Prints fn, class, line, state id, table, callbacks per registration (ch1m_reg_by_class.tsv is this output for both registrars).
"""
import os, re, sys
T = os.getcwd()
sys.path.insert(0, os.path.join(T, 'june2017_re'))
sys.stdout.reconfigure(encoding='utf-8')
import charport_regsim as rs
rs.CLASSEXPR = [r'\*\(undefined4 \*\)\(local_68 \+ 0x38\)', r'\*\(int \*\)\(local_68 \+ 0x38\)', r'uVar5', r'iVar4']
orig = rs.evalcond


def ev(cond, cls, line):
    c = cond.strip()
    if c == '(*(int *)(param_1[0x219f] + 0xaf0) == 0)':
        return False    # the unit templet is present
    if 'param_1[0x219f] + 0xaf0) != 0' in c and '0x13' in c and '0x6d' in c:
        return cls in (0x13, 0x6d)
    if 'FUN_31c40570' in c or c == '(iVar4 == 0)':
        return False    # WIN2 / LOSE2 string tests, no registration behind them
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
for cls in [5, 18, 19, 26, 108, 109, 116]:
    for r in rs.run(L, ops, labels, cls):
        if r[0] == 'REG':
            print('\t'.join([sys.argv[2], str(cls)] + [str(x) for x in r[1:]]))
for u in sorted(set(rs.UNKNOWN)):
    print('UNKNOWN COND', u)
