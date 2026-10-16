"""C11 reach table: which June templet exercises each census key, and is it deployed.

    python june2017_re/c11_reach.py [--key KEY ...]

Reads assets_cache/june_fx.json (June's effect files, executed), census.tsv and the
deployed effect files under kom_contents (X2_KOM_CONTENTS overrides the root).
Writes june2017_re/c11_reach.tsv: one row per census key with the number of June
templets that use the folded key (digits -> %d), how many of those are deployed in
the March kom_contents, and up to 3 deployed examples (smallest templet first).
A templet is "deployed" when its name is in a deployed effect file: a token match in
a BOM-source file, a substring match in a bytecode file (strings are stored raw).
No game files are touched.
"""
import csv, json, os, re, sys, collections

HERE = os.path.dirname(os.path.abspath(__file__))
FX = os.path.join(HERE, 'assets_cache', 'june_fx.json')
CENSUS = os.path.join(HERE, 'census.tsv')
OUT = os.path.join(HERE, 'c11_reach.tsv')
KOM = os.environ.get('X2_KOM_CONTENTS', r'F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents')

DEPLOY_FILES = (('data036_kom', 'DamageEffect.lua', 'DE'), ('datai015_kom', 'DamageEffect_INT.lua', 'DE'),
                # C11a: June's split damage-effect files ship beside DamageEffect.lua since the re-adoption
                ('data036_kom', 'DamageEffect_Character.lua', 'DE'), ('data036_kom', 'DamageEffect_Dungeon.lua', 'DE'),
                ('data036_kom', 'DamageEffect_ETC.lua', 'DE'),
                ('data036_kom', 'EffectSet.lua', 'ES'), ('data036_kom', 'UIEffectSet.lua', 'ES'),
                ('datai015_kom', 'EffectSet_INT.lua', 'ES'))
BOM = b'\xef\xbb\xbf'
LJ = b'\x1bKL'


class Deployed(object):
    def __init__(self):
        self.tok = {'DE': set(), 'ES': set()}
        self.blobs = {'DE': [], 'ES': []}
        self.blocks = {'DE': {}, 'ES': {}}  # name -> folded identifier tokens inside that templet (source files only)
        self.report = []
        for kom, fn, fam in DEPLOY_FILES:
            p = os.path.join(KOM, kom, fn)
            if not os.path.exists(p):
                self.report.append('%s/%s missing' % (kom, fn))
                continue
            b = open(p, 'rb').read()
            if b[:4] == LJ + b'\x81':
                self.blobs[fam].append(b)
                self.report.append('%s/%s bytecode %d B' % (kom, fn, len(b)))
            elif b[:3] == BOM:
                self.tok[fam].update(re.findall(rb'[A-Za-z_][A-Za-z0-9_]*', b))
                self._split(fam, b.decode('utf-8', 'replace'))
                self.report.append('%s/%s source %d B' % (kom, fn, len(b)))
            else:
                self.report.append('%s/%s UNKNOWN header' % (kom, fn))

    def _split(self, fam, t):
        ident = re.compile(r'[A-Za-z_][A-Za-z0-9_]*')
        if fam == 'DE':
            chunks = re.split(r'(?m)^(?=[A-Za-z_][A-Za-z0-9_]*\s*=\s*\{)', t)
            for c in chunks:
                m = ident.match(c)
                if m:
                    self.blocks['DE'].setdefault(m.group(0), set()).update(fold(x) for x in ident.findall(c))
        else:
            for c in t.split('AddEffectSetTemplet_LUA('):
                m = re.search(r'NAME\s*=\s*"([^"]+)"', c)
                if m:
                    self.blocks['ES'].setdefault(m.group(1), set()).update(fold(x) for x in ident.findall(c))

    def has_key(self, fam, name, key):
        """True: the deployed source templet contains the key. 'bc': name and key text both occur in a deployed
        bytecode file (cannot be tied to one templet). False otherwise."""
        b = self.blocks[fam].get(name)
        if b is not None:
            return key in b
        nb = name.encode('utf-8', 'replace')
        kb = key.split('%d')[0].encode('utf-8', 'replace')
        if any(nb in x and kb in x for x in self.blobs[fam]):
            return 'bc'
        return False

    def has(self, fam, name):
        nb = name.encode('utf-8', 'replace')
        if nb in self.tok[fam]:
            return True
        return any(nb in b for b in self.blobs[fam])


def fold(k):
    return re.sub(r'\d+', '%d', k)


def walk(tab, path, out):
    """out: set of (subpath tuple, folded key)"""
    if not isinstance(tab, dict) or '$t' not in tab:
        return
    for k, v in tab['$t'].items():
        if not isinstance(k, str):
            continue
        out.add((path, fold(k)))
        if isinstance(v, dict) and '$t' in v:
            walk(v, path + (fold(k),), out)


def norm_path(root, sub):
    comps = []
    rc = root.split('.', 1)[1] if '.' in root else root
    if rc != 'top':
        comps += rc.split('/')
    comps += [c for c in sub.split('/') if c not in ('(top)', '[]', '')]
    return [c.replace('%d', '') for c in comps]


def build():
    d = json.load(open(FX, encoding='utf-8'))
    idx = collections.defaultdict(list)  # (family, key) -> [(size, name, file, subpath)]
    for name, v in d['globals'].items():
        f = v['file']
        if not f.startswith('DamageEffect'):
            continue
        s = set()
        walk(v['value'], (), s)
        sz = len(json.dumps(v['value']))
        for sp, k in s:
            idx[('DE', k)].append((sz, name, f, '/'.join(sp) or '(top)'))
    for c in d['calls']:
        if c['method'] != 'AddEffectSetTemplet_LUA':
            continue
        t = c['args'][0]
        nm = t['$t'].get('NAME', '?')
        s = set()
        walk(t, (), s)
        sz = len(json.dumps(t))
        for sp, k in s:
            idx[('ES', k)].append((sz, nm, c['file'], '/'.join(sp) or '(top)'))
    return idx


def main():
    a = sys.argv[1:]
    only = set(a[a.index('--key') + 1:]) if '--key' in a else None
    idx = build()
    dep = Deployed()
    print('deployed files:', '; '.join(dep.report))
    rows = list(csv.DictReader(open(CENSUS, encoding='utf-8'), delimiter='\t'))
    out = []
    cache = {}
    for r in rows:
        key = r['key']
        if only and key not in only:
            continue
        hits = idx.get((r['family'], key), [])
        sub = r['subpath']
        want = norm_path(r['root'], sub)
        exact = [h for h in hits if [c.replace('%d', '') for c in h[3].split('/') if c != '(top)'] == want]
        use = exact or hits
        fam = r['family'] if r['family'] in ('DE', 'ES') else None

        def depkind(h):
            if fam is None:
                return False
            k = (fam, h[1], key)
            if k not in cache:
                cache[k] = dep.has_key(fam, h[1], key)
            return cache[k]
        dsrc = [h for h in use if depkind(h) is True] if fam else []
        dbc = [h for h in use if depkind(h) == 'bc'] if fam else []
        ex = '; '.join('%s[%s]' % (h[1], h[2].replace('.lua', '')) for h in sorted(dsrc)[:3])
        if not dsrc and dbc:
            ex = '(bytecode, unverified) ' + '; '.join('%s[%s]' % (h[1], h[2].replace('.lua', '')) for h in sorted(dbc)[:3])
        dhits = dsrc
        out.append([r['family'], r['root'], sub, key, r['status'], r['phase'], r['owner'],
                    r['uses'], str(len(hits)), str(len(use)), str(len(dsrc)), str(len(dbc)), ex])
    if only:
        print('(--key: table not written)')
        for o in out:
            print('\t'.join(o))
        return
    with open(OUT, 'w', encoding='utf-8', newline='') as f:
        w = csv.writer(f, delimiter='\t', lineterminator='\n')
        w.writerow(['family', 'root', 'subpath', 'key', 'status', 'phase', 'owner',
                    'census_uses', 'june_templets_any_path', 'june_templets_path_pref', 'deployed_src', 'deployed_bytecode_maybe', 'deployed_examples'])
        w.writerows(out)
    print('rows', len(out), '->', OUT)
    if only:
        for o in out:
            print('\t'.join(o))


if __name__ == '__main__':
    main()
