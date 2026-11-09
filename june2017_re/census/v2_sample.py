"""june-census V2: how much of an unowned group is real port work. Decompiles a random sample through GhidraMCP (read-only)
and sorts each function by a text heuristic: serializer (2+ calls into the KSerializer primitives 0x3305xxxx), stl (std:: /
container / _Xlength_error), compiler (small body with operator_delete), trivial (<= 6 statements), logic (the rest).
A hand check of 12 'logic' rows found 7 real gameplay bodies and 5 container / ctor / dtor bodies (SUMMARY.md).

    python -I june2017_re/census/v2_sample.py [--cache DIR]   -> june2017_re/census/v2_sample.tsv
Sample: 300 unowned studio + 100 unknown functions of coverage.tsv (seed 11, population in june_va order).
"""
import collections, csv, os, random, re, sys, urllib.parse, urllib.request

HERE = os.path.dirname(os.path.abspath(__file__))
COV = os.path.join(HERE, 'coverage.tsv')
OUT = os.path.join(HERE, 'v2_sample.tsv')
GHIDRA = 'http://127.0.0.1:8080/decompile_function?'
SER = re.compile(r'FUN_3305[0-3][0-9a-f]{3}\(')
DECL = re.compile(r'\s*(undefined|int|uint|char|float|short|byte|bool|void|code|ushort|longlong|double)[\w\s\*\[\]]*;\s*$')


def decompile(va, cache):
    p = os.path.join(cache, va + '.c') if cache else None
    if p and os.path.exists(p):
        return open(p, encoding='utf-8').read()
    op = urllib.request.build_opener(urllib.request.ProxyHandler({}))
    try:
        t = op.open(GHIDRA + urllib.parse.urlencode({'address': va}), timeout=90).read().decode('utf-8', 'replace')
    except Exception as e:
        t = 'ERR %s' % e
    if p:
        open(p, 'w', encoding='utf-8').write(t)
    return t


def classify(t):
    if t.startswith('ERR') or t.startswith('Error decompiling'):
        return 'error'
    if len(SER.findall(t)) >= 2:
        return 'serializer'
    if re.search(r'std::|_Xlength_error|_Container_base|too_long|invalid_map|_Lockit|_Tree|_Iterator_base', t):
        return 'stl'
    body = [l for l in t.split('\n') if l.strip() and not DECL.match(l) and l.strip() not in ('{', '}')]
    if re.search(r'operator_delete', t) and len(body) - 1 <= 8:
        return 'compiler'
    return 'trivial' if len(body) - 1 <= 6 else 'logic'


def main():
    cache = sys.argv[sys.argv.index('--cache') + 1] if '--cache' in sys.argv else None
    if cache:
        os.makedirs(cache, exist_ok=True)
    rows = [r for r in csv.DictReader((l for l in open(COV, encoding='utf-8') if not l.startswith('#')), delimiter='\t')]
    rows.sort(key=lambda r: int(r['june_va'], 16))
    random.seed(11)
    studio = [r for r in rows if r['bucket'] == 'studio' and not r['owner']]
    unknown = [r for r in rows if r['bucket'] == 'unknown']
    picks = [(r, 'studio') for r in random.sample(studio, 300)] + [(r, 'unknown') for r in random.sample(unknown, 100)]
    out = [(r['june_va'], b, r['size'], classify(decompile(r['june_va'], cache))) for r, b in picks]
    with open(OUT, 'w', encoding='utf-8', newline='\n') as f:
        f.write('june_va\tbucket\tsize\tclass\n')
        for x in out:
            f.write('\t'.join(x) + '\n')
    for b in ('studio', 'unknown'):
        print(b, dict(collections.Counter(x[3] for x in out if x[1] == b)))


if __name__ == '__main__':
    main()
