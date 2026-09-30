"""Phase 11: the real GameMajorParticle name-override set from opening June's 24-67.

Every templet name March's existing GameMajorParticle{1,2..23}.txt already define, checked
against every name June's newly-opened GameMajorParticle{24..67}.txt define. Because a later
definition replaces an earlier one by name (KTDGParticleSystem.cpp:1318-1332), and
SERV_IRUHADEV_JUNE_LOADER_GAPS opens 24-67 after 2-23, every shared name here gets June's
version once packed -- not just the 18 names the C++-literal name contract could see
(JUNE2017_NAME_CONTRACT.md only sees names March's own code asks for by literal; a particle
referenced only from EffectSet/DamageEffect *data* is invisible to that census).

Output: particle_override_census.tsv (name, march_file, june_file, verdict).
"""
import sys, os
sys.path.insert(0, os.path.dirname(__file__))
import fxdata as F

MARCH_DIR = r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents\data036_kom"
JUNE_DIR = r"F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\data036_kom"
OUT = os.path.join(os.path.dirname(__file__), 'particle_override_census.tsv')


def load(dir_, lo, hi):
    names = {}
    for i in range(lo, hi + 1):
        fname = 'GameMajorParticle.txt' if i == 1 else 'GameMajorParticle%d.txt' % i
        path = os.path.join(dir_, fname)
        if os.path.exists(path):
            for t in F.parse_txt(path, 'part.game.major'):
                names[t.name] = t
    return names


def main():
    march = load(MARCH_DIR, 1, 23)
    june_new = load(JUNE_DIR, 24, 67)
    shared = sorted(set(march) & set(june_new))
    same = [n for n in shared if march[n].body == june_new[n].body]
    differ = [n for n in shared if march[n].body != june_new[n].body]
    with open(OUT, 'w', encoding='utf-8', newline='\r\n') as f:
        f.write('name\tmarch_file\tjune_file\tverdict\n')
        for n in shared:
            verdict = 'same' if march[n].body == june_new[n].body else 'differ'
            f.write('%s\t%s\t%s\t%s\n' % (n, march[n].file, june_new[n].file, verdict))
    print('March names (1-23):', len(march))
    print('June new-range names (24-67):', len(june_new))
    print('shared (override set):', len(shared), '-- same:', len(same), 'differ:', len(differ))
    print('wrote', OUT)


if __name__ == '__main__':
    main()
