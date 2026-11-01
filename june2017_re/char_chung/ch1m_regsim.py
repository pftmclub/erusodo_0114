"""Per-class path simulation of Chung's registrars (char-chung CH1m): Eve's ch1m_regsim.py with Chung's class tests.

    python june2017_re/char_chung/ch1m_regsim.py <decompile_function output of 32c12a30 or 32c1d1d0> <fn>   # run from Trunk

Joins Ghidra's split `if (... &&` / `{` lines first, then walks the decompile once per Chung class (6 20 21 27 110 111 117).
InitState 32c22540 is not carved: its four registrations are ch0_reg.tsv's hand rows.
Prints fn, class, line, state id, table, callbacks per registration (ch1m_reg_by_class.tsv is this output for both registrars).
"""
import os, re, sys
T = os.getcwd()
sys.path.insert(0, os.path.join(T, 'june2017_re'))
sys.stdout.reconfigure(encoding='utf-8')
import charport_regsim as rs
rs.CLASSEXPR = [r'uVar5', r'uVar4']
orig = rs.evalcond


def ev(cond, cls, line):
    c = cond.strip()
    if c.startswith('(piVar2 != (int *)0x0)') or c.startswith('(piVar1 != (int *)0x0)'):
        return False    # shared_ptr release of the combo data, no registration behind it
    if c == '(*(int *)(param_1 + 0x867c) == 0)':
        return False    # the unit is present
    if c == '(*(int *)(param_1 + 0x867c) != 0)':
        return True
    if c == '(iVar4 != 0)':
        return True     # 32c12a30 @1543: Back Blast ( 26200 ) equipped; its registration is CH2a's
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
for cls in [6, 20, 21, 27, 110, 111, 117]:
    for r in rs.run(L, ops, labels, cls):
        if r[0] == 'REG':
            print('\t'.join([sys.argv[2], str(cls)] + [str(x) for x in r[1:]]))
for u in sorted(set(rs.UNKNOWN)):
    print('UNKNOWN COND', u)
