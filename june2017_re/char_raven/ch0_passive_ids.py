"""char-raven CH0 item 9c: every CX2SkillTree::SI_* id X2GURaven.cpp / .h read, March value vs June's id for the same skill.
python june2017_re/char_raven/ch0_passive_ids.py <x2guraven.i> <s3 tsv>   -> ch0_passive_ids.tsv
june_id = RNSI_<suffix> if June Enum.src.lua has it, else SI_<suffix> in June. in_tree = that id is one of Raven's 126 June tree ids."""
import os, re, sys, csv
HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, os.path.join(TRUNK, 'june2017_re'))
import s3_no_state as S
raw = S.read_latin1(sys.argv[1])
body = re.sub(r'^#line[^\n]*$', '', S.enum_body(S.strip_comments(raw)), flags=re.M)
live, _ = S.parse_enum(body.split('\n'))
live_tokens = set(re.findall(r'\bSI_\w+', raw))
def load(path):
    t = open(path, 'rb').read().decode('utf-8-sig', 'replace').replace('\r\n', '\n').split('\n')
    st = next(i for i, l in enumerate(t) if l.startswith('SKILL_ID = {')); d = {}
    for i in range(st + 1, len(t)):
        if t[i].startswith('}'): break
        m = re.match(r'\t(\w+) = (-?\d+),?\s*$', t[i])
        if m: d[m.group(1)] = int(m.group(2))
    return d
jun = load(os.path.join(TRUNK, 'june_src_fixed/data036_kom/Enum.src.lua'))
core = load('F:/elsword stuff/elsword_2014/els_2014/237311/24965799/kom_contents/data036_kom/Enum.lua')
jby = {}
for n, v in jun.items(): jby.setdefault(v, []).append(n)
rows = list(csv.reader(open(sys.argv[2], encoding='utf-8'), delimiter='\t'))
hdr = next(x for x in rows if x and x[0] == 'unit_class_name')
tree = {}
for x in rows:
    if x and not x[0].startswith('#') and x[0] != 'unit_class_name':
        d = dict(zip(hdr, x)); tree.setdefault(int(d['skill_id']), set()).add(d['unit_class_value'])
def fkey(n):          # (type, tail) with the class tag dropped: SI_P_RRF_BURNING_NASOD_HAND -> ('P', 'BURNING_NASOD_HAND')
    m = re.match(r'(?:RNSI|SI)_(SA|HA|A|P)_(?:RFF|RF|RST|RBM|ROT|RRF|RWT|RVC|COMMON)_(.+)$', n)
    return (m.group(1), m.group(2)) if m else None
treenames = {}
for tid in tree:
    for nm in jby.get(tid, []):
        if nm.startswith('RNSI_') and fkey(nm): treenames.setdefault(fkey(nm), []).append(nm)
sites = {}
for fn in ('X2GURaven.cpp', 'X2GURaven.h'):
    for i, l in enumerate(open(os.path.join(TRUNK, 'X2Lib', fn), 'rb').read().split(b'\n')):
        s = l.decode('cp949', 'replace')
        code = s.split('//')[0]
        for m in re.finditer(r'\bCX2SkillTree::(SI_\w+)', code):
            sites.setdefault(m.group(1), []).append('%s:%d' % (fn, i + 1))
out, tally = [], {}
for n in sorted(sites, key=lambda n: (live.get(n, 10**9), n)):
    mv = live.get(n)
    suf = n[3:]
    cands = ['RNSI_' + suf, n]
    jn = next((c for c in cands if c in jun), '')
    alias = ''
    if jn == '' and mv is not None:
        for c in jby.get(mv, []):                       # March spelling differs from June's (WOFL / HEDGHOG / ARM): follow the value
            if c.startswith('SI_'):
                alias = c; jn = next((x for x in ('RNSI_' + c[3:], c) if x in jun), '')
                break
    jv = jun.get(jn)
    same_val_names = '|'.join(jby.get(mv, [])[:3]) if mv is not None else ''
    moved = ''
    if jv is None or jv not in tree:
        cand = treenames.get(fkey(n) or ('', ''), []) or treenames.get(fkey(alias) if alias else ('', ''), [])
        if cand: moved = '|'.join('%s=%d' % (c, jun[c]) for c in cand)
    if mv is None: st = 'march-name-not-live-in-cl/P-stream'
    elif jn == '': st = 'NO-JUNE-NAME(value %d is June %s)' % (mv, same_val_names or 'unnamed')
    elif jv == mv: st = 'same'
    else: st = 'DIFFERS' + (' (March spelling %s, June %s)' % (n, alias) if alias else '')
    intree = ('y:' + ','.join(sorted(tree[jv], key=int))) if jv in tree else ('n' if jv is not None else '')
    mv_intree = 'y' if mv in tree else 'n'
    kinds = 'P' if n.startswith('SI_P_') else ('A' if n.startswith('SI_A_') else ('SA' if n.startswith('SI_SA_') else ('HA' if n.startswith('SI_HA_') else 'other')))
    tally[st.split('(')[0]] = tally.get(st.split('(')[0], 0) + 1
    out.append([n, str(mv) if mv is not None else '', jn, str(jv) if jv is not None else '', st, intree, mv_intree, same_val_names, kinds,
                'y' if n in core else 'n', moved, ' '.join(sites[n][:4]) + (' +%d' % (len(sites[n]) - 4) if len(sites[n]) > 4 else '')])
with open(os.path.join(HERE, 'ch0_passive_ids.tsv'), 'w', encoding='utf-8', newline='\n') as f:
    f.write('# SI_* ids named in X2GURaven.cpp / X2GURaven.h code (comments stripped): %d. tally: %s\n' % (len(out), ', '.join('%s %d' % kv for kv in sorted(tally.items()))))
    f.write('# march_value = X2SkillTree.h live enum (cl /P of X2GURaven.cpp); june_name = RNSI_<suffix> else SI_<suffix> in June Enum.src.lua; in_tree_june = June id is one of Raven\'s 126 tree ids (classes); march_value_in_tree = the March value is one of them; core_enum = SI_ name in the live core Enum.lua\n')
    f.write('\t'.join(['march_name', 'march_value', 'june_name', 'june_value', 'status', 'in_tree_june', 'march_value_in_tree', 'june_names_at_march_value', 'kind', 'core_enum_has_march_name', 'june_tree_skill_same_title_other_class_tag', 'march_sites']) + '\n')
    for r in out: f.write('\t'.join(r) + '\n')
print(len(out), tally)
