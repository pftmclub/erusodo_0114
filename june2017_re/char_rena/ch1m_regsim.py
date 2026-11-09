"""Per-class path simulation of Rena's registrars (char-rena CH1m): Raven's ch1m_regsim.py with Rena's class tests.

    python june2017_re/char_rena/ch1m_regsim.py <decompile_function output of a registrar> <fn>   # run from Trunk

Rena's registrars test the class two ways: the unit's class (*(param_1[0x219f] + 0x38)) and the unit templet's class
(*(*(param_1[0x219f] + 0xaf0) + 4)); both are rewritten to the base simulator's class expression first, the null tests of the unit and
the templet are taken as not null, and the `iVarN = <class>,` assignments inside a condition are folded. Then Ghidra's split
`if (... &&` / `{` lines are joined and the decompile is walked once per Rena class (3 12 13 24 102 103 114).
Prints fn, class, line, state id, table, callbacks per registration (ch1m_reg_by_class.tsv is this output for both registrars).
The one non-class test that guards a store, InitState's `LESI_RNW_WIN2` compare ( iVar3 == 0 ), registers nothing.
"""
import os, re, sys
T = os.getcwd()
sys.path.insert(0, os.path.join(T, 'june2017_re'))
sys.stdout.reconfigure(encoding='utf-8')
import charport_regsim as rs
CLS = '*(int *)(iVar6 + 0x38)'
rs.CLASSEXPR = [r'\*\(int \*\)\(iVar6 \+ 0x38\)', r'uVar5']
orig = rs.evalcond
CLASSES = [3, 12, 13, 24, 102, 103, 114]


def ev(cond, cls, line):
    c = cond.strip()
    c = c.replace('(param_1[0x219f] == 0)', 'False')
    c = re.sub(r'\(\*\(int \*\)\(param_1\[0x219f\] \+ 0xaf0\) == 0\)', 'False', c)
    c = re.sub(r'\biVar\d+ == 0\b', 'False', c) if 'iVar6 + 0x38' in c or re.search(r'\*\(int \*\)\(iVar\d+ \+ 4\)', c) else c
    c = re.sub(r'\*\(int \*\)\(iVar\d+ \+ 4\)', 'CLS', c)
    c = c.replace(CLS, 'CLS')
    m = re.search(r'(iVar\d+) = CLS, ', c)
    if m:
        c = c.replace(m.group(0), '')
        c = re.sub(r'\b%s\b' % m.group(1), 'CLS', c)
    for a in rs.ALIAS:
        c = re.sub(r'\b%s\b' % a, 'CLS', c)
    if re.fullmatch(r'[0-9a-fx()=!|&<> ]*', re.sub(r'\b(CLS|False)\b', '', c)) and ('CLS' in c or 'False' in c):
        return bool(eval(c.replace('||', ' or ').replace('&&', ' and ').replace('!=', ' != ').replace('False', ' False '), {'CLS': cls}))
    return orig(cond, cls, line)


rs.evalcond = ev
src = open(sys.argv[1], encoding='utf-8', errors='replace').read()
for a in (r'*(undefined4 *)(param_1[0x219f] + 0x38)', r'*(int *)(param_1[0x219f] + 0x38)',
          r'*(int *)(*(int *)(param_1[0x219f] + 0xaf0) + 4)', r'*(undefined4 *)(*(int *)(param_1[0x219f] + 0xaf0) + 4)'):
    src = src.replace(a, CLS)
L = src.split('\n')
for i, l in enumerate(L):
    j = i + 1
    while l.lstrip().startswith('if (') and (l.rstrip().endswith(('&&', '||')) or l.count('(') > l.count(')')):
        l = l.rstrip() + ' ' + L[j].strip()
        L[j] = ''
        j += 1
    L[i] = l
for i, l in enumerate(L):
    if l.lstrip().startswith('if (') and not l.rstrip().endswith('{') and ' goto ' not in l:
        j = i + 1
        while L[j].strip() == '':
            j += 1
        if L[j].strip() == '{':
            L[i] = l.rstrip() + ' {'
            L[j] = ''
ops, labels = rs.compile_(L)
for cls in CLASSES:
    for r in rs.run(L, ops, labels, cls):
        if r[0] == 'REG':
            print('\t'.join([sys.argv[2], str(cls)] + [str(x) for x in r[1:]]))
for u in sorted(set(rs.UNKNOWN)):
    print('UNKNOWN COND', u)
