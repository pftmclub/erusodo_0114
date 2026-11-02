r"""char-luciel CH0 item 4: build ch0_checklist_rows.tsv (columns: section checklist_line row luciel_evidence phase note).
    python june2017_re/char_luciel/ch0_build_checklist_rows.py
Numerical-only table rows are computed from the two June scripts (same logic as ch0_numtables.py); every other row is hand-written in
ch0_checklist_rows_hand.py from JUNE2017_CHARACTER_PORT_CHECKLIST.md (line numbers are that file's current lines) and the digests in this folder."""
import re, collections, os, sys
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
D = os.path.join(HERE, '..', '..', 'june_src_fixed', 'data036_kom') + os.sep
R = []


def row(sec, line, text, ev, ph, note=''):
    R.append((sec, line, text, ev, ph, note))


enum = {}
for l in open(D + 'Enum.src.lua', encoding='utf-8', errors='replace'):
    m = re.match(r'\s*(RNSI_\w+|SI_\w+)\s*=\s*(\d+)', l)
    if m:
        enum[m.group(1)] = int(m.group(2))
tree = collections.defaultdict(set)
for m in re.finditer(r'AddSkillTreeTemplet_LUA\(\{(.*?)\}\)', open(D + 'NewSkillTempletVer2.src.lua', encoding='utf-8', errors='replace').read(), re.S):
    s = re.search(r'm_iSkillID = (\d+)', m.group(1))
    c = re.search(r'UC_LUCIEL_(\w+)', m.group(1))
    if s and c:
        tree[int(s.group(1))].add(c.group(1))
ORDER = ['DUO', 'CHILIARCH', 'ROYAL_GUARD', 'DIABLURA', 'DREAD_LORD', 'NOBLESSE', 'DEMONIO']
PH = dict(zip(ORDER, ['CH2a', 'CH2b', 'CH2c', 'CH2d', 'CH3a', 'CH3b', 'CH3c']))


def owner(sid):
    o = [x for x in ORDER if x in tree.get(sid, ())]
    return o, (PH[o[0]] if o else 'UNPLACED')


applied = []
num = []
for name in ['Luciel_Duo', 'Luciel_Demonio']:
    L = open(D + name + '.src.lua', encoding='utf-8', errors='replace').read().split('\n')
    top = None
    cur = None
    for n, l in enumerate(L, 1):
        m = re.match(r'^(\w+) = \{', l)
        if m:
            top = (m.group(1), n)
        if re.match(r'^\tDAMAGE_DATA\w* = \{', l):
            cur = n
        a = re.search(r'APPLIED_SKILL_ID = SKILL_ID\.(\w+)', l)
        if a:
            applied.append((name, n, top[0], a.group(1), enum[a.group(1)]))
        if 'NUMERICAL_DATA_INDEX' in l:
            seg = '\n'.join(L[cur - 1:cur + 40])
            seg = seg[:seg.find('\n\t}')]
            a = re.search(r'APPLIED_SKILL_ID = SKILL_ID\.(\w+)', seg)
            num.append((name, top[1], top[0], n, a.group(1), enum[a.group(1)]))
assert len(num) == 17 and len(applied) == 21, (len(num), len(applied))
unplaced = 0
for name, tl, tn, n, sk, sid in num:
    o, ph = owner(sid)
    if ph == 'UNPLACED':
        unplaced += 1
    row('§1', 'L54' if name == 'Luciel_Duo' else 'L58', 'numerical-only DAMAGE_DATA table %s (%s.lua L%d)' % (tn, name, tl),
        'APPLIED_SKILL_ID %s %d; NUMERICAL_DATA_INDEX at L%d, no DAMAGE table; June tree rows in %s' % (sk, sid, n, ' '.join(x.lower() for x in o)),
        ph, 'NUM use line from the play-test; June vs March damage in MODS.md (March has no Lu/Ciel). Owner = first of 200/37/38/39/127/128/129 whose tree row holds the id.')
byph = collections.OrderedDict()
for name, n, tn, sk, sid in applied:
    byph.setdefault(owner(sid)[1], []).append(sid)
APPLIED_EV = '; '.join('%s: %s' % (k, ','.join(str(x) for x in sorted(set(v)))) for k, v in byph.items())
APPLIED_PH = ' '.join(byph.keys())

import ch0_checklist_rows_hand as H
H.add(row, APPLIED_EV, APPLIED_PH)

rows = [('section', 'checklist_line', 'row', 'luciel_evidence', 'phase', 'note')] + R
with open(os.path.join(HERE, 'ch0_checklist_rows.tsv'), 'w', encoding='utf-8', newline='') as f:
    for r in rows:
        assert all('\t' not in c and '\n' not in c for c in r), r
        f.write('\t'.join(r) + '\n')
print(len(R), 'rows;', unplaced, 'unplaced numerical tables')
