"""CH0 item 9b: Lu/Ciel's 7 June UnitTemplet rows, key by key, with STR_ID_* checked against March's general.ess and StringID_def."""
import os, re
HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..', '..'))
K = 'F:/elsword stuff/elsword_2014/els_2014/237311/24965799/kom_contents/data036_kom/'
GAME = 'F:/elsword stuff/elsword_2014/els_2014/237311/24965799/data/general.ess'

def rows_of(path):
    t = open(path, 'rb').read().decode('utf-8-sig', 'replace').replace('\r\n', '\n')
    out = []
    for b in t.split('g_pUnitManager:AddUnitTemplet(')[1:]:
        d = {}
        order = []
        for m in re.finditer(r'^\t(\w+) = (.+?),?\s*$', b, re.M):
            d[m.group(1)] = m.group(2).strip()
            order.append(m.group(1))
        uc = d.get('unitClass', '').split('.')[-1]
        out.append((uc, d, order, t[:t.index(b)].count('\n') + 1))
    return out

jun = rows_of(os.path.join(TRUNK, 'june_src_fixed/data036_kom/UnitTemplet.src.lua'))
mar = rows_of(K + 'UnitTemplet.lua')
lu = [r for r in jun if r[0].startswith('UC_LUCIEL_')]
other_j = [r for r in jun if not r[0].startswith('UC_LUCIEL_')]
keys_j = set().union(*[set(r[1]) for r in other_j])
keys_m = set().union(*[set(r[1]) for r in mar]) if mar else set()
raw = open(GAME, 'rb').read()[2:].decode('utf-16-le')
ess = {}
for ln in raw.split('\r\n'):
    s = ''.join(chr(ord(c) ^ 16) for c in ln)
    if '\t' in s:
        k, v = s.split('\t', 1)
        if k.isdigit():
            ess[int(k)] = v
sdef = open(K + 'StringID_def.src.lua', 'rb').read().decode('utf-8-sig', 'replace')
sdefs = set(int(x) for x in re.findall(r'^STR_ID_(\d+) = ', sdef, re.M))
june_sd = open(os.path.join(TRUNK, 'june_src_fixed/data036_kom/StringID_def.src.lua'), 'rb').read().decode('utf-8-sig', 'replace')
jsd = set(int(x) for x in re.findall(r'^STR_ID_(\d+) = ', june_sd, re.M))

lu_only = sorted(set().union(*[set(r[1]) for r in lu]) - keys_j)
lu_only_m = sorted(set().union(*[set(r[1]) for r in lu]) - keys_m)
print('march rows', len(mar), 'march Lu rows', sum(1 for r in mar if 'LUCIEL' in r[0]), 'june rows', len(jun), 'lu', len(lu))
print('keys no other June class has:', len(lu_only), lu_only)
print('keys no March live row has:', len(lu_only_m), lu_only_m)
print('ess ids', len(ess), 'max', max(ess), 'StringID_def live ids', len(sdefs), 'max', max(sdefs), 'june', len(jsd), max(jsd))
allkeys = []
for r in lu:
    for k in r[2]:
        if k not in allkeys:
            allkeys.append(k)
with open(os.path.join(HERE, 'ch0_unittemplet_luciel.tsv'), 'w', encoding='utf-8', newline='\n') as f:
    f.write('# UnitTemplet.src.lua Lu/Ciel rows (%s); March live K/data036_kom/UnitTemplet.lua has %d rows, 0 UC_LUCIEL\n' % (', '.join('%s@%d' % (r[0], r[3]) for r in lu), len(mar)))
    f.write('# keys no other June class row has (%d): %s\n' % (len(lu_only), ' '.join(lu_only)))
    f.write('# keys absent from every March live row (%d): %s\n' % (len(lu_only_m), ' '.join(lu_only_m)))
    f.write('# STR check: general.ess = live game-dir text table (id<TAB>text, XOR 0x10, utf-16le); sd_live = STR_ID_n defined in live data036 StringID_def.src.lua; sd_june = defined in June StringID_def.src.lua\n')
    f.write('unit_class\tkey\tjune_value\tonly_lu_ciel_key\tstr_id\tin_general_ess\tsd_live\tsd_june\tmarch_text\n')
    for uc, d, order, ln in lu:
        for k in order:
            v = d[k]
            m = re.fullmatch(r'STR_ID_(\d+)', v)
            sid = int(m.group(1)) if m else None
            t = ess.get(sid, '') if sid is not None else ''
            f.write('%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' % (uc, k, v, 'y' if k in lu_only else 'n',
                    sid if sid is not None else '', ('y' if sid in ess else 'n') if sid is not None else '',
                    ('y' if sid in sdefs else 'n') if sid is not None else '', ('y' if sid in jsd else 'n') if sid is not None else '',
                    t.replace('\t', ' ').replace('\n', '\n')[:90]))
