"""char-elesis CH0, copy of char_ara/ch0_build_state_enum_names.py: every state name June registers (ch0_reg.tsv + ch0_skill_listing_rows.tsv) against
X2GUEl.h EL_STATE_ID (live branch, from the cl /P stream). June's Enum.src.lua has NO Elesis state enum (no ELESIS_STATE_ID table), so the June enum
columns stay empty; June's ids are the registered ids. Extra column cpp_value_rebased = the C++ value moved from March's USI_END (100) to June's (152),
valid when the member is numbered from ESI_BASE = USI_END + 1 (same relative order assumed, compared with the registered id).
python june2017_re/char_elesis/ch0_build_state_enum_names.py <el.i> -> ch0_state_enum_names.tsv"""
import os, re, sys, csv
HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, os.path.join(TRUNK, 'june2017_re'))
import s3_no_state as S
raw = re.sub(r'^\s*#(?:line|pragma)[^\n]*$', '', S.read_latin1(sys.argv[1]), flags=re.M)   # char-elesis CH0: the enum carries #pragma region lines
def enum_of(name):
    m = re.search(r'\benum\s+%s\s*\{' % name, raw)
    return raw[m.end(): raw.find('}', m.end())]
_m = next(m for m in re.finditer(r'enum\s+(\w+)\s*\{([^}]*)\}', raw) if re.search(r'\bUSI_END\b', m.group(2)))
usi, _ = S.parse_enum(_m.group(2).split('\n'))
usi_end = usi.get('USI_END')
ctx = dict(usi)
body = enum_of('EL_STATE_ID')
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
_lt = '\n'.join(L)
_g = int(re.search(r'^\tGUSI_END = (\d+)', _lt, re.M).group(1))
june_usi_end = _g + int(re.search(r'^\tUSI_END = GAME_UNIT_STATE_ID\.GUSI_END \+ (\d+)', _lt, re.M).group(1))
jen = {}   # no Elesis state enum in June's Enum.src.lua
reg = {}      # name -> (id, where)
for r in csv.reader(open(os.path.join(HERE, 'ch0_reg.tsv'), encoding='utf-8'), delimiter='\t'):
    if r and r[0] != 'fn' and r[2].isdigit(): reg.setdefault(r[3], (int(r[2]), 'reg %s@%s' % (r[0], r[1])))
for r in csv.reader(open(os.path.join(HERE, 'ch0_skill_listing_rows.tsv'), encoding='utf-8'), delimiter='\t'):
    if r and r[0] != 'fn' and r[2].isdigit(): reg.setdefault(r[3], (int(r[2]), 'skill %s' % r[1]))
tables = [m.group(1) for l in open(os.path.join(TRUNK, 'june_src_fixed/data036_kom/ELESIS_KNIGHT.src.lua'), encoding='utf-8', errors='replace') for m in [re.match(r'^(\w+) = \{', l)] if m]
def pref(n):
    for p in ('ESI_', 'USI_', 'SI_'):
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
    rb = (cv + (june_usi_end - usi_end) if cv > usi_end else cv) if cv is not None else None   # members numbered from ESI_BASE move with USI_END; USI-range ids (<= 100) stay
    if cpp == 'y' and rid[0] is not None and rb is not None: eq2 = 'same' if rb == rid[0] else 'DIFF'
    else: eq2 = ''
    out.append([n, pref(n), str(rid[0]) if rid[0] is not None else '', rid[1], str(jv) if jv is not None else '', str(jen[n][1]) if n in jen else '',
                cpp, str(cv) if cv is not None else '', eq, 'y' if n in tables else 'n', str(rb) if rb is not None else '', eq2])
    k = (pref(n), cpp, 'registered' if rid[0] is not None else 'not-registered')
    tally[k] = tally.get(k, 0) + 1
with open(os.path.join(HERE, 'ch0_state_enum_names.tsv'), 'w', encoding='utf-8', newline='\n') as f:
    f.write('# EL_STATE_ID: one C++ enum (X2GUEl.h) holds the ESI_ members; live members via cl /P: %d, ESI_BASE = USI_END + 1 = %s (June USI_END %d, March %d); June Enum.src.lua has no Elesis state enum\n' % (len(vals), vals.get('ESI_BASE'), june_usi_end, usi_end))
    f.write('# reg ids = ch0_reg.tsv (InitState 32b81be0 / 32b864f0 / 32b87a60 / 3202b110) + ch0_skill_listing_rows.tsv (SetEquippedSkillFuncBySkillSlotIndex 32b8c470); script_table = a top-level table of ELESIS_KNIGHT.src.lua\n')
    f.write('# tally (prefix, in C++ enum, registered): ' + '; '.join('%s/%s/%s %d' % (a[0], a[1], a[2], b) for a, b in sorted(tally.items())) + '\n')
    f.write('\t'.join(['name', 'prefix', 'june_reg_id', 'june_reg_site', 'june_enum_lua_value', 'june_enum_line', 'cpp_enum_has_name', 'cpp_value', 'cpp_vs_june_reg_id', 'script_table', 'cpp_value_rebased_to_june_usi_end', 'rebased_vs_june_reg_id']) + '\n')
    for r in out: f.write('\t'.join(r) + '\n')
c = lambda f: sum(1 for r in out if f(r))
print('rows', len(out), 'USI_END', usi_end, 'ESI_BASE', vals.get('ESI_BASE'))
print('registered', c(lambda r: r[2] != ''), 'registered & in C++ enum', c(lambda r: r[2] != '' and r[6] == 'y'), 'registered not in C++ enum', c(lambda r: r[2] != '' and r[6] == 'n'))
print('value DIFF', c(lambda r: r[8] == 'DIFF'), 'same', c(lambda r: r[8] == 'same'))
print('rebased DIFF', c(lambda r: r[11] == 'DIFF'), 'same', c(lambda r: r[11] == 'same'))
print('june Enum.lua names not in C++', c(lambda r: r[4] != '' and r[6] == 'n'))
print(tally)
