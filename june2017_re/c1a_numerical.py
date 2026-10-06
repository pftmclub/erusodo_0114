"""Effect C1a: June's numerical skill data vs March's skill templets.

Inputs: two dumps written by skilltemplet_dump.lua (March NewSkillTempletVer2.lua from
kom_contents/data036_kom, June's converted with ljconv.py), plus census_completion's June
DAMAGE_DATA* blocks (assets_cache/june_fx.json).

    python c1a_numerical.py MARCH_DUMP.tsv JUNE_DUMP.tsv

Writes, next to this script:
  c1a_numerical_blocks.tsv     one row per block census item 1 counts (no non-zero DAMAGE, a numerical
                               key and a skill ID): the skill June's override resolves, the row it reads
                               at level 1 (value x 0.01 = PHYSIC/MAGIC), and whether March has that skill
  c1a_skilltemplet_diff.tsv    per AddSkillTemplet_LUA key: how many of the shared skills differ
and prints the summary C1a's Done note quotes.

June's lookup, as transcribed from the binary (funcmap rows 3263fe41, 320717e0, 32078060, 320716c0,
32216320 @322186f5): NUMERICAL_DATA_INDEX n > 0 picks NUMERICAL_DATA<n> (1-based; out of range -> a
zero row); else NUMERICAL_DATA_NAME picks the first entry whose m_wstrName matches (none -> zero row).
The level picks row min(level, rows) - 1. A row is (m_Value[i], m_Pvp_Rate[i] or 1.0, m_Tech_Point[i]
or 0, m_Hit_Add_MP[i] or 0), each side array clamped to its last entry, one row per m_Value entry.
"""
import collections
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import census_completion as cc  # noqa: E402


def load(path):
    sk, tree = {}, []
    for line in open(path, 'rb'):
        line = line.rstrip(b'\n')
        if line.startswith(b'SK\t'):
            _, i, rest = line.split(b'\t', 2)
            d = {}
            for kv in rest.split(b'\2'):
                if kv:
                    k, v = kv.split(b'\1', 1)
                    d[k.decode()] = v.decode('latin1')
            sk[int(float(i))] = d
        elif line.startswith(b'TR\t'):
            tree.append(line.decode('latin1'))
    return sk, tree


def arr(s, key):
    m = re.search(key + r'=\{([^{}]*)\}', s)
    return [float(x.split('=')[1]) for x in m.group(1).split(',') if x] if m else []


def numerical(d):
    out, n = [], 1
    while 'NUMERICAL_DATA%d' % n in d:
        s = d['NUMERICAL_DATA%d' % n]
        nm = re.search(r'm_wstrName="([^"]*)"', s)
        out.append((nm.group(1) if nm else '', arr(s, 'm_Value'), arr(s, 'm_Pvp_Rate'),
                    arr(s, 'm_Tech_Point'), arr(s, 'm_Hit_Add_MP')))
        n += 1
    return out


def row_at(entry, level):
    """June 32216320 @32218ac1-32218cd2 builds the rows, 320716c0 clamps the level."""
    _, val, pvp, tech, mp = entry
    if not val:
        return (0.0, 0.0, 0.0, 0.0)
    i = min(level, len(val)) - 1
    pick = lambda a, dflt: dflt if not a else a[min(i, len(a) - 1)]
    return (val[i], pick(pvp, 1.0), pick(tech, 0.0), pick(mp, 0.0))


def main(mar_path, jun_path):
    M, mtree = load(mar_path)
    J, jtree = load(jun_path)
    stat = collections.Counter()
    out = [('file', 'templet', 'block', 'skill_id', 'june_skill_name', 'lookup', 'result',
            'L1_value', 'L1_x0.01', 'L1_pvp_rate', 'L1_tech_point', 'L1_hit_add_mp', 'skill_in_march_templets')]
    for fil, name, k, t in cc.june_damage_blocks():
        if cc._direct_damage(t) or not cc._src_numerical(t):
            continue
        a = t.get('APPLIED_SKILL_ID')
        sid = int(a if cc._truthy(a) else t.get('DAMAGE_SKILL_ID'))
        idx, nm = t.get('NUMERICAL_DATA_INDEX'), t.get('NUMERICAL_DATA_NAME')
        jd = J.get(sid)
        nd = numerical(jd) if jd else []
        if cc._num(idx) and idx > 0:
            look = 'INDEX %d' % idx
            ent = nd[idx - 1] if 1 <= idx <= len(nd) else None
            res = 'ok' if ent else 'index out of range: June writes a zero row'
        else:
            look = 'NAME %s' % nm
            ent = next((e for e in nd if e[0] == nm), None)
            res = 'ok' if ent else 'name not found: June writes a zero row'
        if jd is None:
            res = 'no June templet'
        r = row_at(ent, 1) if ent else (0.0, 0.0, 0.0, 0.0)
        stat['June lookup: ' + res] += 1
        stat['skill in March templets' if sid in M else 'skill NOT in March templets'] += 1
        out.append((fil, name, k, sid, (jd or {}).get('m_wstrName', '').strip('"'), look, res,
                    '%g' % r[0], '%g' % (r[0] * 0.01), '%g' % r[1], '%g' % r[2], '%g' % r[3], sid in M))
    with open(os.path.join(HERE, 'c1a_numerical_blocks.tsv'), 'w', encoding='utf-8', newline='\n') as f:
        for row in out:
            f.write('\t'.join(str(x) for x in row) + '\n')

    shared = set(M) & set(J)
    per_key, changed = collections.Counter(), 0
    for i in shared:
        diff = [kk for kk in set(M[i]) | set(J[i]) if M[i].get(kk) != J[i].get(kk)]
        changed += bool(diff)
        for kk in diff:
            per_key[re.sub(r'\d+$', '#', kk)] += 1
    norm = lambda D: set(re.sub(r'\d+$', '#', kk) for d in D.values() for kk in d)
    with open(os.path.join(HERE, 'c1a_skilltemplet_diff.tsv'), 'w', encoding='utf-8', newline='\n') as f:
        f.write('key\tshared_skills_differing\tnote\n')
        for kk, v in per_key.most_common():
            f.write('%s\t%d\t\n' % (kk, v))
        for kk in sorted(norm(J) - norm(M)):
            f.write('%s\t-\tkey only in June\n' % kk)

    ids = sorted(set(r[3] for r in out[1:]))
    with_num = [i for i, d in J.items() if any(kk.startswith('NUMERICAL_DATA') for kk in d)]
    tree_ids = collections.Counter()
    for line in jtree:
        m = re.search(r'm_iSkillID=(\d+)', line)
        if m:
            tree_ids['reform (>= 20000)' if int(m.group(1)) >= 20000 else 'old (< 20000)'] += 1
    print('blocks', len(out) - 1)
    for kk, v in sorted(stat.items()):
        print('  %5d %s' % (v, kk))
    print('distinct skill ids %d, range %d-%d' % (len(ids), ids[0], ids[-1]))
    print('skill templets: March %d (ids %d-%d), June %d; shared %d, of which differ %d; June-only %d'
          % (len(M), min(M), max(M), len(J), len(shared), changed, len(set(J) - set(M))))
    print('June templets with NUMERICAL_DATA %d, shared with March %d' % (len(with_num), len(set(with_num) & set(M))))
    print('skill-tree rows: March %d, June %d %s' % (len(mtree), len(jtree), dict(tree_ids)))


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
