"""Per-class path simulation of Elsword's registrars (char-elsword CH1m): Chung's ch1m_regsim.py with Elsword's class tests.

    python june2017_re/char_elsword/ch1m_regsim.py <decompile_function output of 32d1f8f0 or 32d23ff0> <fn>   # run from Trunk

Joins Ghidra's split `if (... &&` / `{` lines first, then walks the decompile once per Elsword class (1 10 11 22 100 101 112).
Prints fn, class, line, state id, table, callbacks per registration (ch1m_reg_by_class.tsv is this output for both registrars).
"""
import os, re, sys
T = os.getcwd()
sys.path.insert(0, os.path.join(T, 'june2017_re'))
sys.stdout.reconfigure(encoding='utf-8')
import charport_regsim as rs
rs.CLASSEXPR = [r'\*\(undefined4 \*\)\(local_68 \+ 0x38\)', r'\*\(int \*\)\(local_68 \+ 0x38\)', r'\*\(undefined4 \*\)\(uVar4 \+ 0x38\)', r'iVar6']
orig = rs.evalcond
LEARNED = os.environ.get('CH1M_21216', '0') == '1'


def ev(cond, cls, line):
    c = cond.strip()
    if re.fullmatch(r'\(piVar\d+ != \(int \*\)0x0\)', c):
        return False    # shared_ptr release of the combo data, no registration behind it
    if c == '(iVar4 < 1)':
        return not LEARNED     # 32d1f8f0 @1158: GetSkillLevel( 21216 ) < 1 -> ESSI_REVENGE_ATTACK, else the compact counter
    if c == '(iVar6 == 0)':
        return False
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
for cls in [1, 10, 11, 22, 100, 101, 112]:
    for r in rs.run(L, ops, labels, cls):
        if r[0] == 'REG':
            print('\t'.join([sys.argv[2], str(cls)] + [str(x) for x in r[1:]]))
for u in sorted(set(rs.UNKNOWN)):
    print('UNKNOWN COND', u)
