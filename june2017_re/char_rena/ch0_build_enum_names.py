"""char-rena CH0 item 9a (Copy of char_raven/ch0_build_enum_names.py): June SKILL_ID names of Rena's 130 skill ids vs March X2SkillTree.h (live branch, cl /P stream) and the live core Enum.lua.
python june2017_re/char_rena/ch0_build_enum_names.py <lire.i from s3_no_state.py --family X2GULire_ElvenRanger.cpp --preprocess --keep-i> <s3 tsv from the same run>
(derived from char_luciel/ch0_build_enum_names.py; the ids and classes come from the s3 rows instead of the templet file)"""
import os, re, sys, csv
HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, os.path.join(TRUNK, 'june2017_re'))
import s3_no_state as S

raw = S.read_latin1(sys.argv[1])
body = re.sub(r'^#line[^\n]*$', '', S.enum_body(S.strip_comments(raw)), flags=re.M)
live, _ = S.parse_enum(body.split('\n'))
live_by_val = {}
for n, v in live.items():
    live_by_val.setdefault(v, []).append(n)

def load_enum(path, enc_bom):
    t = open(path, 'rb').read().decode('utf-8-sig', 'replace').replace('\r\n', '\n')
    ls = t.split('\n')
    st = next(i for i, l in enumerate(ls) if l.startswith('SKILL_ID = {'))
    d = {}
    for i in range(st + 1, len(ls)):
        if ls[i].startswith('}'):
            break
        m = re.match(r'\t(\w+) = (-?\d+),?\s*$', ls[i])
        if m:
            d[m.group(1)] = (int(m.group(2)), i + 1)
    return d
jsk = load_enum(os.path.join(TRUNK, 'june_src_fixed/data036_kom/Enum.src.lua'), 0)
lsk = load_enum('F:/elsword stuff/elsword_2014/els_2014/237311/24965799/kom_contents/data036_kom/Enum.lua', 1)
jby, lby = {}, {}
for n, (v, ln) in jsk.items(): jby.setdefault(v, []).append(n)
for n, (v, ln) in lsk.items(): lby.setdefault(v, []).append(n)

hraw = S.read_latin1(os.path.join(TRUNK, 'X2Lib/X2SkillTree.h')).split('\n')
hstrip = S.strip_comments('\n'.join(hraw)).split('\n')
def hsite(name):
    pat = re.compile(r'\b%s\b' % re.escape(name))
    unc = [i + 1 for i, l in enumerate(hstrip) if pat.search(l)]
    anyl = [i + 1 for i, l in enumerate(hraw) if pat.search(l)]
    return unc, [i for i in anyl if i not in unc]

rows = list(csv.reader(open(sys.argv[2], encoding='utf-8'), delimiter='\t'))
hdr = next(x for x in rows if x and x[0] == 'unit_class_name')
data = [dict(zip(hdr, x)) for x in rows if x and not x[0].startswith('#') and x[0] != 'unit_class_name']
cls, typ, tname = {}, {}, {}
for d in data:
    v = int(d['skill_id'])
    cls.setdefault(v, set()).add(d['unit_class_value'])
    typ[v] = d['skill_type']; tname[v] = d['templet_name']
ids = sorted(cls)
out, tally = [], {}
for v in ids:
    names = jby.get(v, [])
    n = ([x for x in names if x.startswith('RNSI_')] or names or [''])[0]
    cpp_names = live_by_val.get(v, [])
    if n and n in live:
        cpp = 'name' if live[n] == v else 'name-diff-value'
    elif n and n.startswith('RNSI_') and 'SI_' + n[5:] in live:
        cpp = 'lookalike'
    elif cpp_names:
        cpp = 'value-only'
    else:
        cpp = 'none'
    unc, com = hsite(n) if n else ([], [])
    state = ''
    if n:
        state = 'live' if n in live else ('DEAD-BRANCH(.h line %s)' % unc if unc else ('COMMENTED(line %s)' % com if com else 'absent'))
    core = ''
    if n:
        core = 'name' if (n in lsk and lsk[n][0] == v) else ('name-diff-value' if n in lsk else 'absent')
    note = []
    if len(names) > 1: note.append('june names at value: ' + '|'.join(names))
    if cpp_names: note.append('march live names at value: ' + '|'.join(cpp_names[:4]))
    if lby.get(v) and core != 'name': note.append('core Enum.lua names at value: ' + '|'.join(lby[v][:4]))
    if not n: note.append('NO June SKILL_ID name')
    out.append(['skill', n, str(v), str(jsk[n][1]) if n else '', cpp, (str(unc[0]) if unc else ''), state, core,
                str(lsk[n][1]) if n in lsk else '', typ[v], ','.join(sorted(cls[v], key=int)), tname[v].replace('\n', ' ').replace('\n', ' '), '; '.join(note)])
    tally[cpp] = tally.get(cpp, 0) + 1
    tally['core:' + core] = tally.get('core:' + core, 0) + 1
with open(os.path.join(HERE, 'ch0_enum_names.tsv'), 'w', encoding='utf-8', newline='\n') as f:
    f.write('# skill ids: %d distinct (Rena tree rows of the s3 run, family X2GULire_ElvenRanger.cpp); June names: Enum.src.lua SKILL_ID (%d names); March C++: X2SkillTree.h SKILL_ID live branch via cl /P (%d names); core: live data036_kom/Enum.lua SKILL_ID (%d names)\n' % (len(ids), len(jsk), len(live), len(lsk)))
    f.write('# tally: ' + ', '.join('%s %d' % (k, tally[k]) for k in sorted(tally)) + '\n')
    f.write('# cpp_enum: name = same name+value live; lookalike = RNSI_ -> SI_ name live; value-only = other live name at the value; none = no live member at the value. cpp_state: live / DEAD-BRANCH / COMMENTED / absent\n')
    f.write('\t'.join(['kind', 'june_name', 'june_value', 'june_enum_line', 'cpp_enum', 'cpp_h_line', 'cpp_state', 'core_enum_lua', 'core_line', 'skill_type', 'tree_classes', 'templet_name', 'note']) + '\n')
    for r in out: f.write('\t'.join(r) + '\n')
print(len(out), tally, 'unnamed', sum(1 for r in out if not r[1]))
