"""C3e-1 gate: /Zs /w14244 /w14242 over the TUs, flag on vs off, diff by file:line.
usage: python warndiff.py   (run from june2017_re/). Temporarily comments out the C3e-1 LINE_INDEX16 define for the OFF pass and restores Always.h."""
import os, re, subprocess, sys, shutil, json
sys.path.insert(0, os.getcwd())
import census as C

TUS = ['X2AllyNPCAI.cpp', 'X2BaseUnitViewerUI.cpp', 'X2BattleFieldGame.cpp', 'X2DamageEffect.cpp', 'X2DamageManager.cpp',
       'X2DropItemManager.cpp', 'X2DungeonGame.cpp', 'X2EffectSet.cpp', 'X2GUAdd.cpp', 'X2GUAra.cpp', 'X2GUArme_VioletMage.cpp',
       'X2GUChung.cpp', 'X2GUEL.cpp', 'X2GUElsword_SwordMan.cpp', 'X2GULire_ElvenRanger.cpp', 'X2GUNPC.cpp', 'X2GUNPCAI.cpp',
       'X2GURaven.cpp', 'X2GUUser.cpp', 'X2Game.cpp', 'X2GuEve.cpp', 'X2PartyManager.cpp', 'X2Pet.cpp', 'X2PetAI.cpp',
       'X2PlayGuide.cpp', 'X2RelationshipManager.cpp', 'X2RidingPet.cpp', 'X2SimplePathFinder.cpp', 'X2SquareUnit.cpp',
       'X2State.cpp', 'X2StateBeginning.cpp', 'X2StateField.cpp', 'X2StateServerSelect.cpp', 'X2TFieldGame.cpp',
       'X2TrainingGameUI.cpp', 'X2UDPPacket.cpp', 'X2Unit.cpp']
BATCH = 10
ALWAYS = os.path.join(C.TRUNK, 'KTDXLIB', 'Always.h')
OUT = sys.argv[1] if len(sys.argv) > 1 else '.'


def run_pass(label):
    env = C.cl_env()
    res = {}
    for b in range(0, len(TUS), BATCH):
        procs = []
        for tu in TUS[b:b + BATCH]:
            src = os.path.join(C.TRUNK, 'X2Lib', tu)
            if not os.path.exists(src):
                print('missing', tu); continue
            cmd = [a for a in C.cl_cmd(src, 'x.i') if not a.startswith('/Fi') and a != '/P'] + ['/c', '/Zs', '/EHsc', '/w14244', '/w14242']
            procs.append((tu, subprocess.Popen(cmd, cwd=os.path.join(C.TRUNK, 'X2Lib'), env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)))
        for tu, p in procs:
            out = p.communicate()[0].decode('latin1')
            res[tu] = out
            open(os.path.join(OUT, 'warn_%s_%s.txt' % (label, tu)), 'w', encoding='latin1').write(out)
            print(label, tu, 'rc', p.returncode, len(out), flush=True)
    return res


def parse(res):
    s = {}
    for tu, out in res.items():
        for m in re.finditer(r'^(\S[^\(]*)\((\d+)\) : warning (C4244|C4242): (.*)$', out, re.M):
            s.setdefault((os.path.basename(m.group(1)), int(m.group(2))), set()).add(m.group(4).strip())
    return s


def main():
    on_res = run_pass('on')
    on = parse(on_res)
    errs = sorted(set(l for o in on_res.values() for l in o.splitlines() if ' error C' in l))
    print('ON-pass compile errors: %d' % len(errs))
    for l in errs[:40]:
        print('ERR', l)
    orig = open(ALWAYS, 'rb').read()
    try:
        mod = orig
        for f in (b'SERV_IRUHADEV_JUNE_LINE_INDEX16',):
            mod = mod.replace(b'\r\n#define ' + f + b'\r\n', b'\r\n//#define ' + f + b'\r\n')
        assert mod != orig
        open(ALWAYS, 'wb').write(mod)
        off = parse(run_pass('off'))
    finally:
        open(ALWAYS, 'wb').write(orig)
    new = sorted(k for k in on if k not in off)
    gone = sorted(k for k in off if k not in on)
    print('ON %d sites, OFF %d sites, new %d, gone %d' % (len(on), len(off), len(new), len(gone)))
    for k in new:
        print('NEW', k[0], k[1], ' | '.join(sorted(on[k])))
    for k in gone:
        print('GONE', k[0], k[1], ' | '.join(sorted(off[k])))


main()
