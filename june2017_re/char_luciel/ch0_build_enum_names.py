"""CH0 items 9a: June SKILL_ID names of Lu/Ciel's 195 skill templets vs March X2SkillTree.h (live, from a cl /P stream) and the live core Enum.lua.
python june2017_re/char_luciel/ch0_build_enum_names.py <x2guchung.i from s3_no_state.py --preprocess --keep-i>"""
import os, re, sys
HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, os.path.join(TRUNK, 'june2017_re'))
import s3_no_state as S

ipath = sys.argv[1]
raw = S.read_latin1(ipath)
body = re.sub(r'^#line[^\n]*$', '', S.enum_body(S.strip_comments(raw)), flags=re.M)   # parse_enum drops the item after a #line
live, _ = S.parse_enum(body.split('\n'))
live_by_val = {}
for n, v in live.items():
    live_by_val.setdefault(v, []).append(n)

jenum = S.read_latin1(os.path.join(TRUNK, 'june_src_fixed/data036_kom/Enum.src.lua'))
jlines = jenum.split('\n')
jsk = {}                 # name -> (value, line)
st = next(i for i, l in enumerate(jlines) if l.startswith('SKILL_ID = {'))
for i in range(st + 1, len(jlines)):
    if jlines[i].startswith('}'):
        break
    m = re.match(r'\t(\w+) = (-?\d+),?\s*$', jlines[i])
    if m:
        jsk[m.group(1)] = (int(m.group(2)), i + 1)
jby = {}
for n, (v, ln) in jsk.items():
    jby.setdefault(v, []).append(n)

lpath = 'F:/elsword stuff/elsword_2014/els_2014/237311/24965799/kom_contents/data036_kom/Enum.lua'
ltxt = open(lpath, 'rb').read().decode('utf-8-sig', 'replace').replace('\r\n', '\n')
llines = ltxt.split('\n')
lsk = {}
st = next(i for i, l in enumerate(llines) if l.startswith('SKILL_ID = {'))
for i in range(st + 1, len(llines)):
    if llines[i].startswith('}'):
        break
    m = re.match(r'\t(\w+) = (-?\d+),?\s*$', llines[i])
    if m:
        lsk[m.group(1)] = (int(m.group(2)), i + 1)
lby = {}
for n, (v, ln) in lsk.items():
    lby.setdefault(v, []).append(n)

# the .h: every line that mentions the name, classified
hpath = os.path.join(TRUNK, 'X2Lib/X2SkillTree.h')
hraw = S.read_latin1(hpath).split('\n')
hstrip = S.strip_comments('\n'.join(hraw)).split('\n')
def hsite(name):
    pat = re.compile(r'\b%s\b' % re.escape(name))
    unc = [i + 1 for i, l in enumerate(hstrip) if pat.search(l)]
    anyl = [i + 1 for i, l in enumerate(hraw) if pat.search(l)]
    com = [i for i in anyl if i not in unc]
    return unc, com

# templets
src = S.read_latin1(os.path.join(TRUNK, 'june_src_fixed/data036_kom/NewSkillTempletVer2.src.lua'))
lines = src.split('\n')
templ = {}
for i, l in enumerate(lines):
    m = re.match(r'\tm_eID = (\d+),', l)
    if m:
        v = int(m.group(1))
        if 10000 <= v <= 10699 or 30001 <= v <= 30999:
            templ[v] = i + 1
# templet name and tree membership
tree_ids = set()
for uc in S.JUNE_ONLY_FAMILIES['X2GULuciel.cpp']:
    pass
rows = S.june_only_rows('X2GULuciel.cpp')
tree_ids = {int(r['skill_id']) for r in rows}
tree_cls = {}
for r in rows:
    tree_cls.setdefault(int(r['skill_id']), []).append(r['unit_class_name'][len('UC_LUCIEL_'):])
tname = {int(r['skill_id']): r['templet_name'] for r in rows}

ids = sorted(templ)
extra = sorted(tree_ids - set(templ))
out = []
unnamed = []
tally = {}
def row(kind, v):
    names = jby.get(v, [])
    skn = [n for n in names]
    n = ([x for x in skn if x.startswith('RNSI_')] or skn or [''])[0]
    ccount = len(skn)
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
        if n in live:
            state = 'live'
        elif unc:
            state = 'DEAD-BRANCH(uncommented in .h line %s, not in cl/P stream)' % unc
        elif com:
            state = 'COMMENTED(line %s)' % com
        else:
            state = 'absent'
    core = ''
    if n:
        core = 'name' if (n in lsk and lsk[n][0] == v) else ('name-diff-value' if n in lsk else 'absent')
    core_val = lby.get(v, [])
    note = []
    if len(skn) > 1:
        note.append('june names at value: ' + '|'.join(skn))
    if cpp_names:
        note.append('march live names at value: ' + '|'.join(cpp_names[:4]))
    if core_val and core != 'name':
        note.append('core Enum.lua names at value: ' + '|'.join(core_val[:4]))
    if not n:
        note.append('NO June SKILL_ID name')
    out.append([kind, n, str(v), str(jsk[n][1]) if n else '', cpp, (str(unc[0]) if unc else ''), state, core,
                (str(lsk[n][1]) if n in lsk else ''), ','.join(sorted(set(tree_cls.get(v, [])))) or 'not-in-tree', '; '.join(note)])
    tally.setdefault((kind, cpp), 0)
    tally[(kind, cpp)] += 1
    tally.setdefault((kind, 'core:' + core), 0)
    tally[(kind, 'core:' + core)] += 1
    if not n:
        unnamed.append((v, kind))
for v in ids:
    row('templet', v)
for v in extra:
    row('tree-only', v)

with open(os.path.join(HERE, 'ch0_enum_names.tsv'), 'w', encoding='utf-8', newline='\n') as f:
    f.write('# Lu/Ciel skill ids: %d templets (52 SI_ 10000-10699 + 143 RNSI_ 30001-30999, NewSkillTempletVer2.src.lua) + %d tree-only ids (tree rows whose id is not one of the 195)\n' % (len(ids), len(extra)))
    f.write('# June names: Enum.src.lua SKILL_ID (%d names); March C++: X2SkillTree.h SKILL_ID live branch via cl /P (%d names); core: live data036_kom/Enum.lua SKILL_ID (%d names)\n' % (len(jsk), len(live), len(lsk)))
    f.write('# cpp_enum: name = same name+value live; lookalike = RNSI_ -> SI_ name live; value-only = other live name at the value; none = no live member at the value. cpp_state: live / DEAD-BRANCH / COMMENTED / absent\n')
    f.write('\t'.join(['kind', 'june_name', 'june_value', 'june_enum_line', 'cpp_enum', 'cpp_h_line', 'cpp_state', 'core_enum_lua', 'core_line', 'tree_classes', 'note']) + '\n')
    for r in out:
        f.write('\t'.join(r) + '\n')
# unnamed ids
with open(os.path.join(HERE, 'ch0_unnamed_ids.tsv'), 'w', encoding='utf-8', newline='\n') as f:
    f.write('id\tkind\ttree_classes\ttemplet_line\ttemplet_name\tlive_enum_other_use\tnote\n')
    for v, kind in unnamed:
        other = []
        for i, l in enumerate(jlines):
            if re.search(r'=\s*%d,?\s*$' % v, l):
                other.append('Enum.src.lua:%d %s' % (i + 1, l.strip()))
        f.write('%d\t%s\t%s\t%s\t%s\t%s\t%s\n' % (v, kind, ','.join(sorted(set(tree_cls.get(v, [])))) or 'not-in-tree', templ.get(v, ''), tname.get(v, ''), ' | '.join(other[:3]), 'no SKILL_ID name in June Enum.src.lua'))
for k in sorted(tally):
    print(k, tally[k])
print('unnamed', len(unnamed))
