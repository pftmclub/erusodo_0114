"""Phase 2b calibration (RANOX_LUA_DECOMPILE_FIX_PLAN.md): every number in the
plan's calibration table comes out of this script.

    python calibrate.py [--noise FILE.lua ...] [--upvalue FILE.lua]

Runs, in order (each run rebuilds only its own files' tmp/ljdiff_* scratch):
  1. learning over every calibration file, so the type oracle is settled
     before any comparison (types are name-keyed and shared across files)
  2. the -bg twin check: Kenta's round trip traced stripped and as its twin
     must agree once the twin's line annotations are dropped
  3. Kenta: orig vs rt (fresh decompile), vs pre (the pre-fix port,
     `git show 857dfd7^:...`), vs fix (the working-tree port)
  4. the fix-rerun loop on Kenta and MapData (fixes/*.json)
  5. coverage: distinct path signatures per Kenta function at N = 8..128
  6. the noise check: a lint-clean file with file-scope state must be all pass;
     plus the upvalue file, which shows why captured file-scope locals can't be
  7. lint precision/recall against the fix loop's instances and the static pass
Writes tmp/ljcal_report.txt (and prints it). Exit 0 iff every exit criterion
holds.
"""
import argparse
import collections
import json
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import ljdiff as LD  # noqa: E402

KENTA = 'data037_kom/RAGE_DRYADKENTA.lua'
MAPDATA = 'data036_kom/MapData.lua'
KENTA_FNS = ['RAGE_DRYADKENTA_ATTACK_B_WAIT_FRAME_MOVE', 'RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH_STATE_START',
             'RAGE_DRYADKENTA_COMMON_FRAMEMOVE', 'RAGE_DRYADKENTA_SPECIAL_ATTACK_FIRE_FRAME_MOVE',
             'RAGE_DRYADKENTA_SPECIAL_ATTACK_FIRE_EMOTION_FRAME_MOVE']
PRE_REV = '857dfd7^:march2014/update/data037/RAGE_DRYADKENTA.src.lua'
FIXED = os.path.join(LD.TRUNK, 'march2014', 'update', 'data037', 'RAGE_DRYADKENTA.src.lua')
FIXES = os.path.join(HERE, 'fixes')
REPORT = os.path.join(LD.TMP, 'ljcal_report.txt')
NS = [8, 16, 32, 64, 128]
# The fix loop's leave-one-out needs rare paths: BW-3 (a stale slot4, no unwritten
# read) shows on ~3% of seeds, so 32 seeds miss it about a third of the time.
FIXLOOP_SEEDS = 128
# noise check: lint-clean, decompile trivially correct (tmp/find_noise.py picked them)
NOISE = ['data034_kom/DLG_Map_Select_Window_Instance.lua',     # main chunk writes and reads slot3
         'datai016_kom/EVENT_HALLOWEEN_RESPAWNER_SAND.lua',    # functions read CreateNPCCount, posArray
         'data038_kom/SQUARE_GM_Ariel.lua']                    # 13 functions, read INIT_PHYSIC
UPVALUE = 'data034_kom/DLG_LUCIEL_TRANSFORM_SKILL_SLOT.lua'     # main-chunk locals captured by a function

OUT = []


def say(s=''):
    OUT.append(s)
    print(s)
    sys.stdout.flush()


def short(fn):
    return fn.replace('RAGE_DRYADKENTA_', '')


def verdicts(r):
    return ' '.join('%s=%d' % kv for kv in sorted(r['verdicts'].items()))


def run(files, extras=(), seeds=32, twin=True, opts='lines'):
    log, rounds, res, built = LD.one_run([LD.resolve(f) for f in files], list(extras), seeds, twin, opts)
    for out, (flat, side, t, n, lerr) in res.items():
        if t['harness'] or lerr:
            raise LD.StepError('harness error in %s/%s: %s' % (flat, side, t))
    return built


def diffs(built):
    out = {}
    for flat, sides, srcs in built:
        out[flat] = LD.diff_files(flat, sides, srcs)
    return out


def site_set(res, dynamic_only=False):
    """{(line, name)} of a diff result's static rows (optionally only the
    dynamically confirmed ones)"""
    return {(r['line'], r['name']) for r in res['static'] if not dynamic_only or r['status'] == 'confirmed'}


def lint_set(res):
    return {(h['line'], h['name']) for h in res['lint'] or []}


def pr(lint, truth):
    tp = len(lint & truth)
    p = tp / len(lint) if lint else float('nan')
    r = tp / len(truth) if truth else float('nan')
    return tp, len(lint) - tp, len(truth) - tp, p, r


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument('--noise', nargs='+', default=NOISE)
    ap.add_argument('--upvalue', default=UPVALUE)
    args = ap.parse_args(argv)
    os.makedirs(LD.TMP, exist_ok=True)
    ok = True
    pre = os.path.join(LD.TMP, 'ljcal_kenta_prefix.src.lua')
    data = subprocess.run(['git', 'show', PRE_REV], cwd=LD.TRUNK, capture_output=True, check=True).stdout
    open(pre, 'wb').write(data)
    fixed = os.path.join(LD.TMP, 'ljcal_kenta_fixed.src.lua')
    open(fixed, 'wb').write(open(FIXED, 'rb').read())

    # 1 ------------------------------------------------------------ learning
    files = [KENTA, MAPDATA] + args.noise + ([args.upvalue] if args.upvalue else [])
    say('== 1. learning over %s' % ', '.join(files))
    run(files, seeds=32, opts='')
    say('   types.tsv: %d row(s), converged' % len(LD.load_types()))

    # 2 ------------------------------------------------------------ twin
    say('')
    say('== 2. -bg twin: stripped rt trace vs twin rt trace (Kenta, 32 seeds)')
    flat = LD.flat_of(LD.resolve(KENTA))
    run([KENTA], twin=False, opts='')
    a = json.load(open(LD.tmp(flat, 'rt.json'), encoding='utf-8'))
    run([KENTA], twin=True, opts='lines')
    b = json.load(open(LD.tmp(flat, 'rt.json'), encoding='utf-8'))
    same = LD.strip_lines(a) == LD.strip_lines(b)
    nproto = LD.twin_check(LD.tmp(flat, 'rt.luac'), LD.tmp(flat, 'rt.g.luac'))
    say('   prototypes byte-identical apart from debug info: %d' % nproto)
    say('   traces identical once line annotations are dropped: %s' % same)
    ok = ok and same

    # 3 ------------------------------------------------------------ Kenta x3
    say('')
    say('== 3. Kenta: June original vs rt / pre / fix (32 seeds)')
    built = run([KENTA], extras=[('pre', pre), ('fix', fixed)])
    kd = diffs(built)[flat]
    say('   %-44s %-40s %-40s %s' % ('function', 'rt (fresh decompile)', 'pre (pre-fix port)', 'fix (working tree)'))
    for fn in sorted(kd['rt']['fns']):
        say('   %-44s %-40s %-40s %s' % (short(fn), verdicts(kd['rt']['fns'][fn]),
                                         verdicts(kd['pre']['fns'][fn]), verdicts(kd['fix']['fns'][fn])))
    for side in ('rt', 'pre'):
        for fn in KENTA_FNS:
            if not kd[side]['fns'][fn]['verdicts'].get('dropped-assignment'):
                say('   FAIL: %s shows no dropped-assignment on %s' % (short(fn), side))
                ok = False
    for fn in KENTA_FNS:
        if kd['fix']['fns'][fn]['verdicts'].get('dropped-assignment'):
            say('   FAIL: %s still shows dropped-assignment in the fixed port' % short(fn))
            ok = False
    say('   unwritten reads per (function, name), seeds with the read -- rt vs pre:')
    for fn in KENTA_FNS:
        def names(side):
            c = collections.Counter()
            for st, n in kd[side]['fns'][fn]['sites'].items():
                c[st.split('@')[0]] = max(c[st.split('@')[0]], n)
            return c
        r_, p_ = names('rt'), names('pre')
        say('     %-44s rt %-28s pre %s' % (short(fn), ' '.join('%s:%d' % kv for kv in sorted(r_.items())),
                                            ' '.join('%s:%d' % kv for kv in sorted(p_.items()))))
    tot = {s: sum(len(c['poison']) for c in json.load(open(LD.tmp(flat, s + '.json'), encoding='utf-8'))['calls'])
           for s in ('rt', 'pre', 'fix')}
    say('   poison totals (names per call, summed): rt %d, pre %d, fix %d' % (tot['rt'], tot['pre'], tot['fix']))
    say('   residual divergences in the fixed port (each needs a reason):')
    for fn, r in sorted(kd['fix']['fns'].items()):
        if r['divergent']:
            s, v, labels, i, xa, xb = r['first']
            say('     %-44s %s  first: seed %d event %d: orig %r / fix %r' % (
                short(fn), verdicts(r), s, i, xa[min(2, len(xa) - 1)] if xa else '', xb[min(2, len(xb) - 1)] if xb else ''))
    say('   static candidates: rt %d rows / %d sites, pre %d / %d, fix %d / %d' % tuple(
        x for s in ('rt', 'pre', 'fix') for x in (len(kd[s]['static']), len(site_set(kd[s])))))

    # 4 ------------------------------------------------------------ fix loops
    loops = {}
    for f, name in ((KENTA, 'RAGE_DRYADKENTA'), (MAPDATA, 'MapData')):
        say('')
        say('== 4. fix-rerun loop: %s (%d seeds)' % (f, FIXLOOP_SEEDS))
        okf, L, states, fixes = LD.fixloop(LD.resolve(f), os.path.join(FIXES, name + '.json'), FIXLOOP_SEEDS,
                                           'lines')
        for line in L:
            say('   ' + line)
        ok = ok and okf
        loops[f] = (states, fixes)

    # 4b ----------------------------------------------------------- fix at 128
    # The stale-value instances (BW-3 ~3% of seeds) are invisible at 32 seeds, so
    # "the fixed port restores all 12" needs the fixed port at the fix loop's N.
    say('')
    say('== 4b. Kenta: June original vs fix (working tree), %d seeds' % FIXLOOP_SEEDS)
    built = run([KENTA], extras=[('fix', fixed)], seeds=FIXLOOP_SEEDS)
    fd = diffs(built)[flat]['fix']
    for fn in KENTA_FNS:
        r = fd['fns'][fn]
        say('   %-44s %s' % (short(fn), verdicts(r)))
        if r['verdicts'].get('dropped-assignment'):
            say('   FAIL: %s shows dropped-assignment in the fixed port' % short(fn))
            ok = False
    if set(fd['fns']['RAGE_DRYADKENTA_ATTACK_B_WAIT_FRAME_MOVE']['verdicts']) != {'pass'}:
        say('   FAIL: ATTACK_B_WAIT_FRAME_MOVE (all four restorations, BW-3 included) is not all pass')
        ok = False
    say('   static candidates: %d' % len(fd['static']))

    # 5 ------------------------------------------------------------ coverage
    say('')
    say('== 5. coverage: distinct orig path signatures per Kenta function')
    cov = {}
    reach = {}
    detect = {}
    lcov = {}
    for n in NS:
        run([KENTA], seeds=n, opts='lines')
        ta = json.load(open(LD.tmp(flat, 'orig.json'), encoding='utf-8'))
        tb = json.load(open(LD.tmp(flat, 'rt.json'), encoding='utf-8'))
        res = LD.diff_traces(ta, tb)
        cov[n] = {fn: len(r['sig_orig']) for fn, r in res['fns'].items()}
        # lines of the round trip each function executed over all seeds
        lc = collections.defaultdict(set)
        for c in tb['calls']:
            if c.get('lines'):
                lc[c['fn']].update(int(x) for x in c['lines'].split(','))
        lcov[n] = {fn: len(v) for fn, v in lc.items()}
        detect[n] = {fn: res['fns'][fn]['verdicts'].get('dropped-assignment', 0) for fn in KENTA_FNS}
        # COMMON_FRAMEMOVE's elseif body: the 20% dash
        reach[n] = sum(1 for c in ta['calls'] if c['fn'] == 'RAGE_DRYADKENTA_COMMON_FRAMEMOVE'
                       and any('SetFlag_LUA(2, true)' in e for e in c['events']))
    say('   %-44s %s' % ('function', ' '.join('%6s' % ('N=%d' % n) for n in NS)))
    for fn in sorted(cov[NS[0]]):
        say('   %-44s %s' % (short(fn), ' '.join('%6d' % cov[n][fn] for n in NS)))
    say('   lines of the round trip executed per function (union over seeds; saturates where the paths do not):')
    say('   %-44s %s' % ('function', ' '.join('%6s' % ('N=%d' % n) for n in NS)))
    for fn in sorted(cov[NS[0]]):
        say('   %-44s %s' % (short(fn), ' '.join('%6d' % lcov[n].get(fn, 0) for n in NS)))
    say('   seeds reaching COMMON_FRAMEMOVE\'s elseif body (SetFlag_LUA(2, true)) on orig: %s' % (
        ', '.join('N=%d: %d' % (n, reach[n]) for n in NS)))
    say('   seeds with dropped-assignment on rt, the five known functions:')
    for fn in KENTA_FNS:
        say('     %-44s %s' % (short(fn), ' '.join('%6d' % detect[n][fn] for n in NS)))

    # 6 ------------------------------------------------------------ noise
    # Each file must show zero divergences; together they must exercise the
    # main chunk writing (and reading back) its own slotN temps, and functions
    # reading file-scope state. "Main-chunk locals read by functions" as the
    # plan words it cannot be clean: ljd prints such a read as uvN (6b).
    say('')
    say('== 6. noise check (32 seeds): %s' % ', '.join(args.noise))
    nb = run(args.noise)
    dd = diffs(nb)
    any_fs = any_slots = False
    for f in args.noise:
        nflat = LD.flat_of(LD.resolve(f))
        nd = dd[nflat]['rt']
        tot = LD.summary_row(nd)
        tb = json.load(open(LD.tmp(nflat, 'rt.json'), encoding='utf-8'))
        ta = json.load(open(LD.tmp(nflat, 'orig.json'), encoding='utf-8'))
        shadow = sum(len(c.get('shadow', [])) for c in tb['calls'])
        mv = nd['main_values']
        fs = ta.get('filescope') or []
        clean = set(tot) <= {'pass'} and not mv['differ'] and not mv['only_orig'] and not mv['only_side']             and not nd['static'] and not shadow and (not nd.get('main') or nd['main']['verdict'] == 'pass')
        any_fs = any_fs or bool(fs)
        any_slots = any_slots or bool(mv['slot_globals'])
        say('   %s' % f)
        say('     entries %d, calls %d, verdicts %s; main chunk %s; main_values %d (differ %d, only orig %d, '
            'only rt %d)' % (len(nd['fns']), sum(r['seeds'] for r in nd['fns'].values()),
                             ' '.join('%s=%d' % kv for kv in sorted(tot.items())),
                             nd['main']['verdict'] if nd.get('main') else '-', mv['n'], len(mv['differ']),
                             len(mv['only_orig']), len(mv['only_side'])))
        say('     rt main-chunk slotN globals: %s; SHADOW_READs %d; static rows %d; lint hits %d' % (
            ', '.join(mv['slot_globals']) or 'none', shadow, len(nd['static']), len(nd['lint'] or [])))
        say('     file-scope state its functions read: %s' % (', '.join(fs) or 'none'))
        say('     zero divergences: %s' % clean)
        ok = ok and clean
    say('   exercises main-chunk slotN writes: %s; exercises file-scope reads: %s' % (any_slots, any_fs))
    ok = ok and any_fs and any_slots
    if args.upvalue:
        say('')
        say('== 6b. captured file-scope locals: %s' % args.upvalue)
        ub = run([args.upvalue])
        uflat = LD.flat_of(LD.resolve(args.upvalue))
        ud = diffs(ub)[uflat]['rt']
        say('   verdicts %s; lint hits %d; static rows %d (%s)' % (
            ' '.join('%s=%d' % kv for kv in sorted(LD.summary_row(ud).items())), len(ud['lint'] or []),
            len(ud['static']), ', '.join(sorted({'%s@L%d' % (r['name'], r['line']) for r in ud['static']}))))

    # 7 ------------------------------------------------------------ lint
    say('')
    say('== 7. lint precision / recall (unit: unique (line, slot) on the same text)')
    states, fixes = loops[KENTA]
    fresh = states[0]
    truth = site_set(fresh)
    lint = lint_set(fresh)
    tp, fp, fn_, p, r = pr(lint, truth)
    say('   Kenta rt   truth = fix-loop sites (= static pass, all dynamically confirmed): %d; lint %d: '
        'TP %d FP %d FN %d  precision %.2f recall %.2f' % (len(truth), len(lint), tp, fp, fn_, p, r))
    say('     lint misses: %s' % ', '.join('%s@L%d' % (n, l) for l, n in sorted(truth - lint)))
    inst = [f for f in fixes if f.get('kind', 'dropped') == 'dropped']
    found = [f for f in inst if {(l, n) for l, n in f.get('_owned', []) + f.get('_cleared', [])} & lint]
    nosite = [f for f in inst if not f.get('_owned') and not f.get('_cleared')]
    say('     instances: %d dropped writes; lint flags a read of %d; static pass %d; no unwritten read at all '
        '(stale value, harness only): %s' % (len(inst), len(found), len(inst) - len(nosite),
                                            ', '.join(f['id'] for f in nosite) or 'none'))
    say('     lint-missed instances: %s' % ', '.join(f['id'] for f in inst if f not in found))
    pres = kd['pre']
    truth = site_set(pres, dynamic_only=True)
    lint = lint_set(pres)
    tp, fp, fn_, p, r = pr(lint, truth)
    say('   Kenta pre  truth = dynamically confirmed static sites: %d; lint %d: TP %d FP %d FN %d  '
        'precision %.2f recall %.2f; misses %s' % (len(truth), len(lint), tp, fp, fn_, p, r,
                                                   ', '.join('%s@L%d' % (n, l) for l, n in sorted(truth - lint))))
    mstates, mfixes = loops[MAPDATA]
    truth = site_set(mstates[0])
    lint = lint_set(mstates[0])
    tp, fp, fn_, p, r = pr(lint, truth)
    say('   MapData rt truth %d; lint %d: TP %d FP %d FN %d  precision %.2f recall %.2f' % (
        len(truth), len(lint), tp, fp, fn_, p, r))
    say('   (MapData\'s structure defect MD-S and Kenta\'s order defects have no unwritten read: no text lint sees them)')

    say('')
    tj = json.load(open(LD.tmp(flat, 'orig.json'), encoding='utf-8'))
    say('type oracle: types.tsv %d row(s), types_h32 %s (compare later runs under the same oracle, '
        'or re-baseline)' % (len(LD.load_types()), tj['types_h32']))
    say('result: %s' % ('OK' if ok else 'FAILED'))
    with open(REPORT, 'w', encoding='utf-8', newline='\n') as f:
        f.write('\n'.join(OUT) + '\n')
    return 0 if ok else 1


if __name__ == '__main__':
    try:
        sys.exit(main(sys.argv[1:]))
    except (LD.StepError, subprocess.TimeoutExpired) as e:
        print('STEP_ERROR %s' % e)
        sys.exit(2)
