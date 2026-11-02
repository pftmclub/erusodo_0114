r"""char-luciel CH0 item 4 helper: list Luciel_Duo / Luciel_Demonio DAMAGE_DATA tables that carry NUMERICAL_DATA_INDEX and no DAMAGE table,
with the APPLIED_SKILL_ID value and the first Lu/Ciel class whose June tree row holds that id. Prints tsv: script line table skill_name skill_id owners."""
import re, collections
D = 'june_src_fixed/data036_kom/'
enum = {}
for l in open(D + 'Enum.src.lua', encoding='utf-8', errors='replace'):
    m = re.match(r'\s*(RNSI_\w+|SI_\w+)\s*=\s*(\d+)', l)
    if m: enum[m.group(1)] = int(m.group(2))
tree = collections.defaultdict(set)
txt = open(D + 'NewSkillTempletVer2.src.lua', encoding='utf-8', errors='replace').read()
for m in re.finditer(r'AddSkillTreeTemplet_LUA\(\{(.*?)\}\)', txt, re.S):
    b = m.group(1)
    s = re.search(r'm_iSkillID = (\d+)', b); c = re.search(r'UC_LUCIEL_(\w+)', b)
    if s and c: tree[int(s.group(1))].add(c.group(1))
ORDER = ['DUO', 'CHILIARCH', 'ROYAL_GUARD', 'DIABLURA', 'DREAD_LORD', 'NOBLESSE', 'DEMONIO']
PH = dict(zip(ORDER, ['CH2a', 'CH2b', 'CH2c', 'CH2d', 'CH3a', 'CH3b', 'CH3c']))
for name in ['Luciel_Duo', 'Luciel_Demonio']:
    L = open(D + name + '.src.lua', encoding='utf-8', errors='replace').read().split('\n')
    top = None
    i = 0
    cur = None
    for n, l in enumerate(L, 1):
        m = re.match(r'^(\w+) = \{', l)
        if m: top = (m.group(1), n)
        if re.match(r'^\tDAMAGE_DATA\w* = \{', l): cur = n
        if 'NUMERICAL_DATA_INDEX' in l:
            seg = '\n'.join(L[cur - 1:cur + 40])
            end = seg.find('\n\t}')
            seg = seg[:end]
            a = re.search(r'APPLIED_SKILL_ID = SKILL_ID\.(\w+)', seg)
            sid = enum.get(a.group(1)) if a else None
            own = [o for o in ORDER if o in tree.get(sid, ())]
            print(name, top[1], top[0], a.group(1) if a else '', sid, ','.join(own),
                  PH[own[0]] if own else '?', 'DAMAGE' in re.findall(r'\bDAMAGE\b', seg) and 'has-DAMAGE' or 'no-DAMAGE', sep='\t')
