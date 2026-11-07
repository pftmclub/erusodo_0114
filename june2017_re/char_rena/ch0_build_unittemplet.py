"""char-rena CH0 item 9b (Copy of char_raven/ch0_build_unittemplet.py): Rena's 7 UnitTemplet rows, June (june_src_fixed UnitTemplet.src.lua) vs March (live data036_kom/UnitTemplet.lua, BOM source),
key by key, with every STR_ID_n checked against the live text table general.ess (id<TAB>text, XOR 0x10, utf-16le) and StringID_def.src.lua (March live and June).
python june2017_re/char_rena/ch0_build_unittemplet.py"""
import os, re
HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..', '..'))
K = 'F:/elsword stuff/elsword_2014/els_2014/237311/24965799/kom_contents/data036_kom/'
GAME = 'F:/elsword stuff/elsword_2014/els_2014/237311/24965799/data/general.ess'

def rows_of(path):
    t = open(path, 'rb').read().decode('utf-8-sig', 'replace').replace('\r\n', '\n')
    out = []
    for b in t.split('g_pUnitManager:AddUnitTemplet(')[1:]:
        d, order = {}, []
        for m in re.finditer(r'^\t(\w+) = (.+?),?\s*$', b, re.M):
            d[m.group(1)] = m.group(2).strip(); order.append(m.group(1))
        out.append((d.get('unitClass', '').split('.')[-1], d, order, t[:t.index(b)].count('\n') + 1))
    return out
jun = {r[0]: r for r in rows_of(os.path.join(TRUNK, 'june_src_fixed/data036_kom/UnitTemplet.src.lua')) if r[0].startswith('UC_LIRE_')}
mar = {r[0]: r for r in rows_of(K + 'UnitTemplet.lua') if r[0].startswith('UC_LIRE_')}
raw = open(GAME, 'rb').read()[2:].decode('utf-16-le')
ess = {}
for ln in raw.split('\r\n'):
    s = ''.join(chr(ord(c) ^ 16) for c in ln)
    if '\t' in s:
        k, v = s.split('\t', 1)
        if k.isdigit(): ess[int(k)] = v
def sdset(p):
    return set(int(x) for x in re.findall(r'^STR_ID_(\d+) = ', open(p, 'rb').read().decode('utf-8-sig', 'replace'), re.M))
sd_m = sdset(K + 'StringID_def.src.lua'); sd_j = sdset(os.path.join(TRUNK, 'june_src_fixed/data036_kom/StringID_def.src.lua'))
ucs = sorted(jun, key=lambda u: jun[u][3])
lines, summary = [], []
for uc in ucs:
    j, m = jun[uc][1], mar.get(uc, (0, {}, [], 0))[1]
    keys = list(jun[uc][2]) + [k for k in m if k not in j]
    same = ch = jo = mo = 0; diffs = []
    for k in keys:
        jv, mv = j.get(k), m.get(k)
        st = 'same' if jv == mv else ('june-only' if mv is None else ('march-only' if jv is None else 'changed'))
        same += st == 'same'; ch += st == 'changed'; jo += st == 'june-only'; mo += st == 'march-only'
        if st != 'same': diffs.append('%s: %s -> %s' % (k, mv or '-', jv or '-'))
        strs = []
        for v in (mv, jv):
            mm = re.fullmatch(r'STR_ID_(\d+)', v or '')
            if mm and int(mm.group(1)) not in [s[0] for s in strs]:
                n = int(mm.group(1)); strs.append((n, 'y' if n in ess else 'n', 'y' if n in sd_m else 'n', 'y' if n in sd_j else 'n', ess.get(n, '')))
        note = ' | '.join('STR_ID_%d general.ess=%s sd_live=%s sd_june=%s "%s"' % (a, b, c, d, e.replace('\t', ' ')[:70]) for a, b, c, d, e in strs)
        lines.append('\t'.join([uc, k, mv or '-', jv or '-', st, note]))
    summary.append('# %s: March keys %d (row at %s), June keys %d (row at %d); same %d, changed %d, june-only %d, march-only %d' % (uc, len(m), mar[uc][3] if uc in mar else 'ABSENT', len(j), jun[uc][3], same, ch, jo, mo))
    for d in diffs: summary.append('#    ' + d)
with open(os.path.join(HERE, 'ch0_unittemplet_rena.tsv'), 'w', encoding='utf-8', newline='\n') as f:
    f.write('# Rena UnitTemplet rows: June june_src_fixed/data036_kom/UnitTemplet.src.lua vs March live kom_contents/data036_kom/UnitTemplet.lua (%d Rena rows March, %d June)\n' % (len(mar), len(jun)))
    f.write('# string table = general.ess (live game-dir text table, %d ids, XOR 0x10 utf-16le); sd_live / sd_june = STR_ID_n defined in StringID_def.src.lua (March live %d ids / June %d ids)\n' % (len(ess), len(sd_m), len(sd_j)))
    f.write('\n'.join(summary) + '\n')
    f.write('unit_class\tkey\tmarch_value\tjune_value\tdiff\tstring_check\n')
    f.write('\n'.join(lines) + '\n')
print('\n'.join(summary))
