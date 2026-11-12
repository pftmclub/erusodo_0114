"""char-elesis CH0 item 3: ch0_keys.tsv = plan_charport_keys.tsv + owner + readers. Copy of char_ara/ch0_build_keys.py, trimmed: Ara's hand-written owners are not
carried over (they are a template only), the owner column is `DRAFT` for the caller to decide. Rows that get owner + readers: every june-only / neither /
march-only-site key, plus the keys named in the CH0 brief (MAX_HYPER_MODE_TIME, HYPER_MODE_COUNT, MP_CHARGE_RATE, the EFFECT_SET_LIST_* keys, RELATIVE_SKILL_LEARN_CHECK,
IGNORE_STATE_DEFENCE, IGNORE_DEFENCE_DECLINE, WAY_OF_SWORD_TYPE, CREATE_EFFECT_SET, PLAY_TYPE, TRANSITION).
March readers = a live grep of the quoted key string in X2Lib / KTDXLIB / X2 (*.cpp, *.h, *.inl), file:line, a line that starts with // marked (commented).
python june2017_re/char_elesis/ch0_build_keys.py  (reads ch0_state_skill.tsv for each state's phase; the March script counts are read from the March ELESIS_KNIGHT.lua bytecode)"""
import os, re, csv, bisect, collections
HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..')) + '/'
L = open(T + 'june_src_fixed/data036_kom/ELESIS_KNIGHT.src.lua', encoding='utf-8', errors='replace').read().split('\n')
MS = open('F:/elsword stuff/elsword_2014/els_2014/237311/24965799/kom_contents/data036_kom/ELESIS_KNIGHT.lua', 'rb').read()
tops = [(i + 1, m.group(1)) for i, l in enumerate(L) for m in [re.match(r'^(\w+) = \{', l)] if m]
ln = [t[0] for t in tops]
ph = {}
for r in csv.reader(open(HERE + '/ch0_state_skill.tsv', encoding='utf-8'), delimiter='\t'):
    if len(r) > 7 and r[0] and not r[0].startswith('#') and r[0] != 'state':
        ph[r[0]] = r[7]
EXTRA = ['MAX_HYPER_MODE_TIME', 'HYPER_MODE_COUNT', 'MP_CHARGE_RATE', 'EFFECT_SET_LIST_EX_NO_TRANS', 'EFFECT_SET_LIST_EX_TRANS_1', 'EFFECT_SET_LIST_EX_APPLIED_SKILL',
         'EFFECT_SET_LIST_ULTIMATE_EX_APPLIED_SKILL', 'RELATIVE_SKILL_LEARN_CHECK', 'IGNORE_STATE_DEFENCE', 'IGNORE_DEFENCE_DECLINE', 'WAY_OF_SWORD_TYPE',
         'CREATE_EFFECT_SET', 'PLAY_TYPE', 'TRANSITION']
# live grep of the quoted key in the March client source
SRC = []
for sub in ('X2Lib', 'KTDXLIB', 'X2'):
    for root, _, fs in os.walk(T + sub):
        for f in fs:
            if f.lower().endswith(('.cpp', '.h', '.inl')):
                SRC.append(os.path.join(root, f))
LINES = {}
for p in SRC:
    LINES[p] = open(p, 'rb').read().decode('latin-1').split('\n')


def march_sites(key):
    pat = re.compile(r'"%s"' % re.escape(key))
    out = []
    for p, ls in LINES.items():
        for i, l in enumerate(ls, 1):
            if pat.search(l):
                out.append('%s:%d%s' % (os.path.relpath(p, T).replace(os.sep, '/'), i, ' (commented)' if l.lstrip().startswith('//') else ''))
    return sorted(out)


def states(key):
    c = collections.OrderedDict()
    for i, l in enumerate(L):
        if re.match(r'\s*%s\s*=' % re.escape(key), l):
            s = tops[bisect.bisect_right(ln, i + 1) - 1][1]
            c[s] = c.get(s, 0) + 1
    return c


def mscript(key):
    n = MS.count(key.encode())
    return 'March ELESIS_KNIGHT.lua (bytecode) has the string x%d' % n if n else 'March ELESIS_KNIGHT.lua (bytecode) lacks the string'


rows = list(csv.reader(open(HERE + '/plan_charport_keys.tsv', encoding='utf-8'), delimiter='\t'))
out = [rows[0] + ['owner', 'readers']]
cnt = collections.Counter()
for r in rows[1:]:
    k = r[0]
    own = rd = ''
    if k in EXTRA or r[11] in ('june-only', 'neither', 'march-only-site'):
        ms = march_sites(k)
        sts = '; '.join('%s [%s]' % (s, ph.get(s, 'INIT' if s.startswith('INIT_') else '?')) + (' x%d' % n if n > 1 else '') for s, n in states(k).items())
        own = 'DRAFT'
        rd = 'June: %s; readers %s. March reader: %s. %s. States: %s' % (
            r[6], r[7], ('; '.join(ms[:8]) + (' +%d more' % (len(ms) - 8) if len(ms) > 8 else '')) if ms else 'no March reader (no quoted literal in X2Lib / KTDXLIB / X2)', mscript(k), sts)
        cnt[r[11]] += 1
    out.append(r + [own, rd])
with open(HERE + '/ch0_keys.tsv', 'w', encoding='utf-8', newline='') as f:
    csv.writer(f, delimiter='\t', lineterminator='\n').writerows(out)
print(len(out) - 1, 'rows;', dict(cnt), 'with owner:', sum(1 for r in out[1:] if r[-2]))
