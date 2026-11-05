import re, csv, json, collections
T = "F:/elsword stuff/elsword_2014/els_2014/ElswordFiles/ready_and_built/ProjectX2_SVN/ProjectX2_SVN/source/EU_CN_US/Trunk/"
L = open(T + 'june_src_fixed/data036_kom/RAVEN_FIGHTER.src.lua', encoding='utf-8', errors='replace').read().split('\n')
# enum value -> name from baseline tsv
rows = list(csv.reader(open(T + 'june2017_re/char_raven/ch0_s3_baseline.txt', encoding='utf-8'), delimiter='\t'))
hdr = [r for r in rows if r and r[0] == 'unit_class_name'][0]
data = [dict(zip(hdr, r)) for r in rows if r and not r[0].startswith('#') and r[0] != 'unit_class_name']
sid = collections.defaultdict(list)
name = {}
typ = {}
for d in data:
    i = int(d['skill_id'])
    sid[i].append(int(d['unit_class_value']))
    name[i] = d['skill_name']
    typ[i] = d['skill_type']
# Enum names for any id (for APPLIED_SKILL_ID that are names)
E = open(T + 'june_src_fixed/data036_kom/Enum.src.lua', encoding='utf-8', errors='replace').read()
i = E.index('\nSKILL_ID = {')
j = E.index('\n}', i)
sk = {}
for m in re.finditer(r'^\s*([A-Za-z_0-9]+)\s*=\s*(-?\d+)', E[i:j], re.M):
    sk.setdefault(m.group(1), int(m.group(2)))
byval = {v: k for k, v in sk.items()}
out = []
for ln, l in enumerate(L, 1):
    if 'NUMERICAL_DATA_INDEX' in l:
        # enclosing table: walk back to the line with smaller indentation that ends with '{'
        ind = len(l) - len(l.lstrip('\t'))
        k = ln - 2
        while k >= 0:
            lk = L[k]
            if lk.strip().endswith('{') and (len(lk) - len(lk.lstrip('\t'))) < ind:
                break
            k -= 1
        tname = L[k].strip().rstrip('{').rstrip('= ').strip()
        # top-level state
        t = k
        while t >= 0 and not re.match(r'^[A-Za-z_0-9]+ = \{', L[t]):
            t -= 1
        top = L[t].split(' ')[0]
        # block extent
        e = ln
        while e < len(L) and not (L[e].strip() in ('}', '},') and (len(L[e]) - len(L[e].lstrip('\t'))) == (len(L[k]) - len(L[k].lstrip('\t')))):
            e += 1
        blk = L[k:e + 1]
        idx = re.search(r'NUMERICAL_DATA_INDEX = (\d+)', l).group(1)
        app = [re.search(r'=\s*(\S+?),?$', x.strip()).group(1) for x in blk if 'APPLIED_SKILL_ID' in x]
        # whole top-level table for APPLIED too
        e2 = t + 1
        while e2 < len(L) and not re.match(r'^\}', L[e2]):
            e2 += 1
        app_top = [re.search(r'=\s*(\S+?),?$', x.strip()).group(1) for x in L[t:e2 + 1] if 'APPLIED_SKILL_ID' in x]
        dam = any(re.match(r'\s*DAMAGE\s*=', x) for x in blk)
        out.append((ln, top, t + 1, tname, k + 1, idx, app, app_top, dam))
for o in out:
    print(o)
json.dump([list(o) for o in out], open(T + 'june2017_re/char_raven/ch0_numtables.json', 'w'))
