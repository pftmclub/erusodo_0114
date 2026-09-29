"""What census.py cannot see: June's character (and other non-effect) scripts.

census.py covers only June's DamageEffect*/EffectSet* files. Character scripts
(Elsword_SwordMan.lua, RAVEN_FIGHTER.lua, ...) feed the same SetDamageDataFromLUA,
so a DamageData key March does not parse is silently defaulted there too. This
script is the check for JUNE2017_CHARACTER_PORT_CHECKLIST.md; rerun it before a
character port and after any phase that changes census.tsv.

    python charport_scan.py <scratch dir>

<scratch dir> receives a copy of every June data036_kom/*.lua plus the
.std.luac/.src.lua elstools writes next to each (the June directory is never
written to). Existing copies are reused. Prints four sections:

  KEYS      open DE.DAMAGE_DATA census rows (to-implement / binary-only /
            unsupported) that a non-effect June script sets, matched by exact
            sub-path under the DAMAGE_DATA* table (source walk; a file is
            path-checked only when its .src.lua key set equals its bytecode's)
  NUMERIC   per character: DAMAGE_DATA* tables that set NUMERICAL_DATA_INDEX > 0
            and have no DAMAGE table, i.e. whose damage only June's numerical
            override supplies
  ENUM      June Enum.lua tables a character script indexes that March's
            Enum/DungeonEnum lack (a missing table aborts the rest of the chunk)
  SKIPPED   files the source tokenizer rejects (their bytecode was still scanned
            by name, see KEYS-BYNAME)
"""
import bisect
import collections
import csv
import glob
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..'))
sys.path.insert(0, HERE)
import reconcile as R  # noqa: E402

JUNE_DIR = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\data036_kom'
ELSTOOLS = os.path.join(TRUNK, 'elstools', 'elstools.exe')
EFFECT = re.compile(r'^(DamageEffect|EffectSet)')
STRLIT = re.compile(r'"(?:[^"\\\n]|\\.)*"')


def prepare(scratch):
    os.makedirs(scratch, exist_ok=True)
    for f in glob.glob(os.path.join(JUNE_DIR, '*.lua')):
        dst = os.path.join(scratch, os.path.basename(f))
        if not os.path.exists(dst):
            shutil.copy2(f, dst)
    todo = sorted(os.path.basename(f) for f in glob.glob(os.path.join(scratch, '*.lua'))
                  if not f.endswith('.src.lua') and not os.path.exists(f[:-4] + '.std.luac'))
    # batches of 20 with relative paths: one long command line fails with WinError 206
    for i in range(0, len(todo), 20):
        args = [ELSTOOLS, 'decompile-batch', '-q', '--timeout-secs', '60']
        for f in todo[i:i + 20]:
            args += ['-i', f]
        subprocess.run(args, cwd=scratch, capture_output=True)


def open_rows():
    rows = [r for r in csv.DictReader(open(os.path.join(HERE, 'census.tsv'), encoding='utf-8'), delimiter='\t')
            if r['root'] == 'DE.DAMAGE_DATA']
    sp = lambda s: () if s == '(top)' else tuple(s.split('/'))
    return {(sp(r['subpath']), r['key']): (r['status'], r['phase']) for r in rows}


def enum_tables(p):
    return set(re.findall(r'(?m)^([A-Z][A-Z0-9_]*)\s*=\s*\{', open(p, encoding='utf-8', errors='replace').read()))


def damage_tables(t):
    for m in re.finditer(r'\bDAMAGE_DATA\w*\s*=\s*\{', t):
        i, d = m.end(), 1
        while d and i < len(t):
            d += (t[i] == '{') - (t[i] == '}')
            i += 1
        yield t[m.end():i - 1]


def main():
    scratch = sys.argv[1]
    prepare(scratch)
    status = open_rows()
    openkeys = set(k for (s, k), v in status.items() if v[0] in ('to-implement', 'binary-only', 'unsupported'))
    march = enum_tables(os.path.join(TRUNK, 'march2014', 'data036', 'Enum.src.lua')) | \
        enum_tables(os.path.join(TRUNK, 'march2014', 'data036', 'DungeonEnum.src.lua'))
    june = enum_tables(os.path.join(R.LUA, 'Enum.src.lua'))

    byname = collections.defaultdict(collections.Counter)
    exact = collections.defaultdict(collections.Counter)
    numeric, enums, skipped, mismatch = {}, {}, [], []
    for lua in sorted(glob.glob(os.path.join(scratch, '*.lua'))):
        if lua.endswith('.src.lua'):
            continue
        b = os.path.basename(lua)[:-4]
        if EFFECT.match(b):
            continue
        bk = R.bytecode_keys(lua)
        for k in openkeys:
            if bk.get(k):
                byname[k][b] += bk[k]
        src = lua[:-4] + '.src.lua'
        if not os.path.exists(src):
            skipped.append(b + ' (no .src.lua)')
            continue
        try:
            keys = R.source_keys(src)
        except BaseException:          # R.tokens calls sys.exit on some dialog strings
            skipped.append(b)
            continue
        if set(k for _, _, k, _ in keys) != set(bk):
            mismatch.append(b)
            continue
        for templet, path, key, tab in keys:
            idx = [i for i, p in enumerate(path) if p.startswith('DAMAGE_DATA')]
            if not idx:
                continue
            sub = tuple(R.norm(p) for p in path[idx[-1] + 1:])
            st = status.get((sub, R.norm(key))) or status.get((sub, key))
            if st and st[0] in ('to-implement', 'binary-only', 'unsupported'):
                exact[(st[0], st[1], '/'.join(sub) or '(top)', key)][b] += 1
        t = open(src, encoding='utf-8', errors='replace').read()
        n = collections.Counter()
        for body in damage_tables(t):
            if re.search(r'\bNUMERICAL_DATA_INDEX\s*=\s*[1-9]', body):
                n['no DAMAGE table' if not re.search(r'\bDAMAGE\s*=\s*\{', body) else 'has DAMAGE table'] += 1
            else:
                n['no NUMERICAL_DATA_INDEX'] += 1
        if sum(n.values()):
            numeric[b] = n
        tt = STRLIT.sub('""', t)
        st_starts = [m.start() for m in re.finditer(r'(?m)^[A-Za-z_]', tt)]
        miss = collections.Counter()
        for m in re.finditer(r'(?<![\w.])([A-Z][A-Z0-9_]*)\.([A-Za-z_]\w*)', tt):
            if m.group(1) in june and m.group(1) not in march:
                miss[m.group(1)] += 1
        if miss:
            enums[b] = miss

    print('KEYS (exact sub-path under DAMAGE_DATA*)')
    for (s, ph, sub, k), c in sorted(exact.items()):
        print('  %-12s %-10s %-8s %-58s %4d  %s' % (s, ph, sub, k, sum(c.values()),
                                                  '; '.join('%s x%d' % kv for kv in c.most_common())))
    print('KEYS-BYNAME (bytecode, any table; leads only)')
    for k in sorted(byname):
        print('  %-58s %s' % (k, '; '.join('%s x%d' % kv for kv in byname[k].most_common(8))))
    print('NUMERIC (DAMAGE_DATA* tables)')
    for b, n in sorted(numeric.items()):
        if n['no DAMAGE table'] or n['has DAMAGE table']:
            print('  %-28s %s' % (b, dict(n)))
    print('ENUM (June-only Enum.lua tables indexed)')
    for b, m in sorted(enums.items()):
        print('  %-28s %s' % (b, dict(m)))
    print('SKIPPED %s' % skipped)
    print('SRC/BYTECODE MISMATCH (not path-checked) %s' % mismatch)


if __name__ == '__main__':
    main()
