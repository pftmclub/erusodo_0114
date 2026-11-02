"""CH0 item 8: does June's CharacterStatTable.src.lua reproduce March's server StatTable.lua rows?  (read-only; writes ch0_stat_fit.tsv)
Hypothesis tested: base / first-job class rows at level L = relative row of the class's UNIT_TYPE x (0.8 + 0.0225 (L-1)) for the four attack/defence
columns and HP x (1 + 0.15 (L-1)); second-job rows: level 1 = 0.8 x SECOND_CLASS row (HP 100%), level >= 2 = the unit type's curve."""
import os, re
HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..', '..'))
K = 'F:/elsword stuff/elsword_2014/els_2014/237311/24965799/kom_contents/data036_kom/'
t = open(os.path.join(TRUNK, 'june_src_fixed/data036_kom/CharacterStatTable.src.lua'), 'rb').read().decode('latin-1').replace('\r\n', '\n')

def tab(name):
    m = re.search(r'^%s = \{\n(.*?)^\}' % name, t, re.S | re.M)
    return {b.split(',')[0].strip().split('.')[-1]: [float(x) for x in b.split(',')[1:]] for b in re.findall(r'\{\s*([^{}]*?)\s*\}', m.group(1), re.S)}

def etbl(path, name):
    s = open(path, 'rb').read().decode('utf-8-sig', 'replace').replace('\r\n', '\n')
    m = re.search(r'^%s = \{\n(.*?)^\}' % name, s, re.S | re.M)
    return {k: int(v) for k, v in re.findall(r'^\t(\w+) = (-?\d+)', m.group(1), re.M)}

ut = tab('CHARACTER_RELATIVE_STAT_TABLE')
sc = tab('SECOND_CLASS_RELATIVE_STAT_TABLE')
cl = etbl(K + 'Enum.lua', 'UNIT_CLASS')
inv = {v: k for k, v in cl.items()}
c2t = {}
for b in open(K + 'UnitTemplet.lua', 'rb').read().decode('utf-8-sig', 'replace').split('AddUnitTemplet(')[1:]:
    a = re.search(r'unitClass = UNIT_CLASS\.(\w+)', b)
    u = re.search(r'unitType = UNIT_TYPE\.(\w+)', b)
    if a and u:
        c2t[a.group(1)] = u.group(1)
# Lu/Ciel templets are June-only
for n in ('UC_LUCIEL_DUO', 'UC_LUCIEL_CHILIARCH', 'UC_LUCIEL_ROYAL_GUARD', 'UC_LUCIEL_DIABLURA', 'UC_LUCIEL_DREAD_LORD', 'UC_LUCIEL_NOBLESSE', 'UC_LUCIEL_DEMONIO'):
    c2t[n] = 'UT_LUCIEL'
st = open(os.path.join(TRUNK, 'KncWX2Server/ServerResource/US/StatTable.lua'), 'rb').read().decode('utf-8-sig', 'replace')
rows = {}
for m in re.finditer(r'^StatTable:SetUnitStat\(\s*(\d+)\s*,\s*(\d+)\s*,\s*\{\s*AtkPhysic\s*=\s*([\d.]+)\s*,\s*AtkMagic\s*=\s*([\d.]+)\s*,\s*DefPhysic\s*=\s*([\d.]+)\s*,\s*DefMagic\s*=\s*([\d.]+)\s*,\s*HP\s*=\s*([\d.]+)', st, re.M):
    rows[(int(m.group(1)), int(m.group(2)))] = [float(m.group(i)) for i in range(3, 8)]

def pred(name, L):
    base = ut[c2t[name]]
    s = sc.get(name)
    row, lvl1 = (s, True) if (s and L == 1) else (base, False)
    A, D, M, DM, H = row           # TDT order: AtkPhysic, DefPhysic, AtkMagic, DefMagic, HP
    f = 0.8 if lvl1 else 0.8 + 0.0225 * (L - 1)
    hp = H if lvl1 else H * (1 + 0.15 * (L - 1))
    return [A * f, M * f, D * f, DM * f, hp]      # StatTable order: AtkPhysic, AtkMagic, DefPhysic, DefMagic, HP

out = ['class\tclass_name\trelative_row_used\tlevels_1_80_within_1\tlevels_81_99_within_1\tlevel1_actual(AtkP,AtkM,DefP,DefM,HP)']
for c in sorted({c for c, l in rows}):
    n = inv[c]
    ok80 = sum(1 for L in range(1, 81) if (c, L) in rows and all(abs(p - x) < 1.0001 for p, x in zip(pred(n, L), rows[(c, L)])))
    n80 = sum(1 for L in range(1, 81) if (c, L) in rows)
    ok99 = sum(1 for L in range(81, 100) if (c, L) in rows and all(abs(p - x) < 1.0001 for p, x in zip(pred(n, L), rows[(c, L)])))
    n99 = sum(1 for L in range(81, 100) if (c, L) in rows)
    out.append('%d\t%s\t%s\t%d/%d\t%d/%d\t%s' % (c, n, 'SECOND_CLASS(L1)+' + c2t[n] if n in sc else c2t[n], ok80, n80, ok99, n99, ','.join('%g' % x for x in rows[(c, 1)])))
out.append('')
out.append('# DERIVED (NOT June data, NOT a live row): the same formula applied to the Lu/Ciel rows of CharacterStatTable.src.lua; StatTable order AtkPhysic,AtkMagic,DefPhysic,DefMagic,HP')
out.append('class\tclass_name\tlevel\tAtkPhysic\tAtkMagic\tDefPhysic\tDefMagic\tHP')
for c, n in ((200, 'UC_LUCIEL_DUO'), (37, 'UC_LUCIEL_CHILIARCH'), (38, 'UC_LUCIEL_ROYAL_GUARD'), (39, 'UC_LUCIEL_DIABLURA'), (127, 'UC_LUCIEL_DREAD_LORD'), (128, 'UC_LUCIEL_NOBLESSE'), (129, 'UC_LUCIEL_DEMONIO')):
    for L in (1, 2, 17, 80):
        out.append('%d\t%s\t%d\t%s' % (c, n, L, '\t'.join('%.1f' % x for x in pred(n, L))))
open(os.path.join(HERE, 'ch0_stat_fit.tsv'), 'w', encoding='utf-8', newline='\n').write('\n'.join(out) + '\n')
print(len(rows), 'live rows;', sum(1 for l in out[1:51] if l.split(chr(9))[3].split('/')[0] == l.split(chr(9))[3].split('/')[1]), 'of 50 classes fit at levels 1-80')
