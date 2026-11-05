"""char-raven CH0 item 9d: every state name June registers (ch0_reg.tsv + ch0_skill.tsv) against X2GURaven.h RAVEN_STATE_ID (live branch, from the cl /P stream)
and June's Enum.src.lua RAVEN_STATE_ID. python june2017_re/char_raven/ch0_build_state_enum_names.py <x2guraven.i> -> ch0_state_enum_names.tsv"""
import os, re, sys, csv
HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, os.path.join(TRUNK, 'june2017_re'))
import s3_no_state as S
raw = re.sub(r'^#line[^\n]*$', '', S.read_latin1(sys.argv[1]), flags=re.M)
def enum_of(name):
    m = re.search(r'\benum\s+%s\s*\{' % name, raw)
    return raw[m.end(): raw.find('}', m.end())]
_m = next(m for m in re.finditer(r'enum\s+(\w+)\s*\{([^}]*)\}', raw) if re.search(r'\bUSI_END\b', m.group(2)))
usi, _ = S.parse_enum(_m.group(2).split('\n'))
usi_end = usi.get('USI_END')
ctx = dict(usi)
body = enum_of('RAVEN_STATE_ID')
vals, nxt, order = {}, None, []
for it in body.split(','):
    it = it.strip()
    m = re.match(r'([A-Za-z_]\w*)\s*(?:=\s*(.+))?$', it, re.S)
    if not m: continue
    n, e = m.group(1), m.group(2)
    if e is not None:
        e = e.strip(); mm = re.fullmatch(r'(\w+)\s*\+\s*(\d+)', e)
        v = ctx[mm.group(1)] + int(mm.group(2)) if mm and mm.group(1) in ctx else (int(e) if e.isdigit() else None)
        if v is None: raise SystemExit('cannot resolve ' + n + ' = ' + e)
    else:
        v = nxt
    vals[n] = v; nxt = v + 1; order.append(n)
L = open(os.path.join(TRUNK, 'june_src_fixed/data036_kom/Enum.src.lua'), encoding='utf-8', errors='replace').read().split('\n')
st = next(i for i, l in enumerate(L) if l.startswith('RAVEN_STATE_ID = {'))
jen = {}
for i in range(st + 1, len(L)):
    if L[i].startswith('}'): break
    m = re.match(r'\t(\w+) = (\d+),?\s*$', L[i])
    if m: jen[m.group(1)] = (int(m.group(2)), i + 1)
reg = {}      # name -> (id, where)
for r in csv.reader(open(os.path.join(HERE, 'ch0_reg.tsv'), encoding='utf-8'), delimiter='\t'):
    if r and r[0] != 'fn' and r[2].isdigit(): reg.setdefault(r[3], (int(r[2]), 'reg %s@%s' % (r[0], r[1])))
for r in csv.reader(open(os.path.join(HERE, 'ch0_skill_listing_rows.tsv'), encoding='utf-8'), delimiter='\t'):
    if r and r[0] != 'fn' and r[2].isdigit(): reg.setdefault(r[3], (int(r[2]), 'skill %s' % r[1]))
tables = [m.group(1) for l in open(os.path.join(TRUNK, 'june_src_fixed/data036_kom/RAVEN_FIGHTER.src.lua'), encoding='utf-8', errors='replace') for m in [re.match(r'^(\w+) = \{', l)] if m]
def pref(n):
    for p in ('RSI_', 'RVC_', 'RBM_', 'RRF_', 'RWT_', 'SI_'):
        if n.startswith(p): return p
    return 'other'
out, tally = [], {}
names = list(dict.fromkeys(list(reg) + list(jen) + tables))
for n in names:
    rid = reg.get(n, (None, ''))
    cv, jv = vals.get(n), jen.get(n, (None, ''))[0]
    cpp = 'y' if n in vals else 'n'
    if cpp == 'y' and rid[0] is not None: eq = 'same' if cv == rid[0] else 'DIFF'
    else: eq = ''
    out.append([n, pref(n), str(rid[0]) if rid[0] is not None else '', rid[1], str(jv) if jv is not None else '', str(jen[n][1]) if n in jen else '',
                cpp, str(cv) if cv is not None else '', eq, 'y' if n in tables else 'n'])
    k = (pref(n), cpp, 'registered' if rid[0] is not None else 'not-registered')
    tally[k] = tally.get(k, 0) + 1
with open(os.path.join(HERE, 'ch0_state_enum_names.tsv'), 'w', encoding='utf-8', newline='\n') as f:
    f.write('# RAVEN_STATE_ID: one C++ enum (X2GURaven.h:6-251) holds every June prefix (RSI_, RVC_, RBM_, RRF_, RWT_, SI_); live members via cl /P: %d, first RSI_BASE = USI_END + 1 = %s; June Enum.src.lua RAVEN_STATE_ID: %d names\n' % (len(vals), vals.get('RSI_BASE'), len(jen)))
    f.write('# reg ids = ch0_reg.tsv (InitStateCommon 32c5d660 / InitState 32c66fe0 / 3202b110) + ch0_skill_listing_rows.tsv (SetEquippedSkillFuncBySkillSlotIndex 32c641a0); script_table = a top-level table of RAVEN_FIGHTER.src.lua\n')
    f.write('# tally (prefix, in C++ enum, registered): ' + '; '.join('%s/%s/%s %d' % (a[0], a[1], a[2], b) for a, b in sorted(tally.items())) + '\n')
    f.write('\t'.join(['name', 'prefix', 'june_reg_id', 'june_reg_site', 'june_enum_lua_value', 'june_enum_line', 'cpp_enum_has_name', 'cpp_value', 'cpp_vs_june_reg_id', 'script_table']) + '\n')
    for r in out: f.write('\t'.join(r) + '\n')
c = lambda f: sum(1 for r in out if f(r))
print('rows', len(out), 'USI_END', usi_end, 'RSI_BASE', vals.get('RSI_BASE'))
print('registered', c(lambda r: r[2] != ''), 'registered & in C++ enum', c(lambda r: r[2] != '' and r[6] == 'y'), 'registered not in C++ enum', c(lambda r: r[2] != '' and r[6] == 'n'))
print('value DIFF', c(lambda r: r[8] == 'DIFF'), 'same', c(lambda r: r[8] == 'same'))
print('june Enum.lua names not in C++', c(lambda r: r[4] != '' and r[6] == 'n'))
print(tally)
