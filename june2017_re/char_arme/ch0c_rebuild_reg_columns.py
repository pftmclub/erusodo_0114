"""pilot-char-revamp CH0c: rewrite the registration-derived columns of the two CH0 maps from a fixed reg.tsv.

    python june2017_re/charport_states.py init 32ce77b0 32cea0c0 3202b110 > <S>/reg.tsv
    python june2017_re/charport_states.py skill 32cede00 > <S>/skill.tsv
    python june2017_re/char_arme/ch0c_rebuild_reg_columns.py <S> [--write]

Why not re-run the builders: ch0_build_state_skill.py needs <S>/march_live_states.tsv (a `cl /P` of X2GUArme_VioletMage.cpp) and
ch0_build_class_map.py needs <S>/ch0_vtable_slots.tsv and Ghidra xrefs, and also rewrites checklist rows. CH0c changes only the
registration rows, so this script recomputes, with the builders' own rules:
  ch0_state_skill.tsv part 1: june_registration (col 3) and june_callbacks (col 5); the per-phase function totals line.
  ch0_class_map.tsv: for every function that is a state callback (old or new map): kind (adds / drops 'state-fn'),
  name_or_states, march_pair, verdict, phase. A function whose phase is CH1a or CH1b (ported) keeps it.
Without --write it prints the changes only.
"""
import re, sys, os, collections, subprocess

S = sys.argv[1]
WRITE = '--write' in sys.argv
HERE = os.path.dirname(os.path.abspath(__file__))
CORE_FN = {'32ce77b0': 'InitStateCommon 32ce77b0', '32cea0c0': 'InitState 32cea0c0', '3202b110': 'CX2GUUser event states 3202b110'}
LO, HI = 0x32cb6e00, 0x32cf3000
ORDER = ['CH1a', 'CH1b', 'CH1c', 'CH1d', 'CH1e', 'CH1f', 'CH1g', 'CH1h', 'CH1i', 'CH1', 'CH2a', 'CH2b', 'CH3a', 'CH3b', 'CH4', 'CH5', 'n/a']
PORTED = ('CH1a', 'CH1b')

# the builder's how / cb accumulation (ch0_build_state_skill.py), registration and skill parts
how, cbs = collections.defaultdict(list), collections.defaultdict(list)
# CH0c: registrations whose table name is a std::string, not a literal (the extractor prints the variable). Read from the
# decompile and the listing: 32ce77b0 id 0x17 (USI_HYPER_MODE) takes its name from 31fdd490 by job; 32cea0c0 ids 9 / 0xb / 0xc
# (USI_START / USI_WIN / USI_LOSE) take the three names the START / WIN / LOSE roll 31fe22d0 fills (apppuStack_2c / _64 / _48).
st_names = [l.split('\t')[0] for l in open(os.path.join(HERE, 'ch0_state_skill.tsv'), encoding='utf-8').read().split('# part 2')[0].split('\n')
            if len(l.split('\t')) >= 8 and l.split('\t')[1] != 'june_script_line']
st_how = {l.split('\t')[0]: l.split('\t')[2] for l in open(os.path.join(HERE, 'ch0_state_skill.tsv'), encoding='utf-8').read().split('# part 2')[0].split('\n')
          if len(l.split('\t')) >= 8}
rolled = [n for n in st_names if 'START/WIN/LOSE roll 31fe22d0' in st_how.get(n, '')]
VARNAME = {('32ce77b0', '23'): ('name from 31fdd490 by job', [n for n in st_names if 'CX2GUUser hyper mode 31fdd490' in st_how.get(n, '')]),
           ('32cea0c0', '9'): ('name from the roll 31fe22d0', [n for n in rolled if re.search(r'_START$', n)]),
           ('32cea0c0', '11'): ('name from the roll 31fe22d0', [n for n in rolled if re.search(r'_WIN2?$', n)]),
           ('32cea0c0', '12'): ('name from the roll 31fe22d0', [n for n in rolled if re.search(r'_LOSE2?$', n)])}
for l in open(os.path.join(S, 'reg.tsv')).read().splitlines()[1:]:
    fn, line, sid, name, cb = (l.split('\t') + [''])[:5]
    if re.match(r'p+[a-z]*Var\d+$', name):
        why, names = VARNAME.get((fn, sid), (None, []))
        if not names:
            print('UNRESOLVED variable name: %s' % l)
            continue
        for n in names:
            how[n].append('%s@%s id %s (%s)' % (CORE_FN.get(fn, fn), line, sid, why))
            if cb:
                cbs[n].append(cb)
        continue
    how[name].append('%s@%s id %s' % (CORE_FN.get(fn, fn), line, sid))
    if cb:
        cbs[name].append(cb)
SKILL = collections.defaultdict(list)   # name -> the skill 'how' entries (kept from the map: only the order of entries matters)
for l in open(os.path.join(S, 'skill.tsv')).read().splitlines()[1:]:
    f = l.split('\t')
    fn, line, cases, nt, ht, ncb, hcb, extra = f[:8]
    names = [x for x in nt.split(';') + ht.split(';') if x] + [e.split(':', 1)[1] for e in extra.split()]
    for n in names:
        SKILL[n].append(('SetEquippedSkillFunc 32cede00@%s case %s' % (line, cases), (ncb, hcb)))

sp = os.path.join(HERE, 'ch0_state_skill.tsv')
text = open(sp, encoding='utf-8').read()
part1, rest = text.split('# part 2', 1)
lines = part1.split('\n')
out, changes = [], []
for l in lines:
    p = l.split('\t')
    if len(p) < 8 or p[1] == 'june_script_line':
        out.append(l)
        continue
    name = p[0]
    old_how = p[2].split(' | ')
    # keep every non-registration entry of the old column (roll, hyper mode, skill switch, MANUAL text) in place
    regs = [h for h in old_how if re.match(r'(InitStateCommon|InitState|CX2GUUser event states) [0-9a-f]{8}@', h)]
    others = [h for h in old_how if h not in regs]
    if not regs and not how.get(name):
        out.append(l)
        continue
    new_how = list(dict.fromkeys(how.get(name, []) + others))
    if not how.get(name) and others:   # no registration row at all now: should not happen, report it
        changes.append(('LOST', name, p[2], ''))
    cb = list(cbs.get(name, []))
    for _, (ncb, hcb) in SKILL.get(name, []):
        for c in (ncb, hcb):
            if c and c not in cb:
                cb.append(c)
    nh, nc = ' | '.join(new_how), ' | '.join(cb)
    if p[2] == 'no reference in June C++ (string absent from the exe)':
        nh = p[2]
    if (nh, nc) != (p[2], p[4]):
        changes.append(('STATE', name, (p[2], p[4]), (nh, nc)))
    p[2], p[4] = nh, nc
    out.append('\t'.join(p))
new_part1 = '\n'.join(out)

# state-fn rows of the class map, from the new part 1 (ch0_build_class_map.py's rules)
sfn = collections.defaultdict(lambda: {'uses': [], 'march': set(), 'ph': 'n/a'})
for l in new_part1.split('\n'):
    p = l.split('\t')
    if len(p) < 8 or p[1] == 'june_script_line':
        continue
    mslots = dict(x.split('=', 1) for l2 in p[5].split(' ; ') for x in l2.split(':', 1)[-1].split(',') if '=' in x)
    for slot, th, tgt in re.findall(r'(\w+)=([0-9a-f]{8})>([0-9a-f]{8})', p[4]):
        if not (LO <= int(tgt, 16) < HI):
            continue
        e = sfn[tgt]
        e['uses'].append('%s.%s' % (p[0], slot))
        if slot in mslots:
            e['march'].add(mslots[slot])
        ph = p[7].split(' ')[0]
        if ph in ORDER and ORDER.index(ph) < ORDER.index(e['ph']):
            e['ph'] = ph
cp = os.path.join(HERE, 'ch0_class_map.tsv')
ctext = open(cp, encoding='utf-8').read().split('\n')
cout, cchanges, moves = [], [], []
for l in ctext:
    p = l.split('\t')
    if len(p) < 7 or l.startswith('#') or p[0] == 'june_fn':
        cout.append(l)
        continue
    fn, kind = p[0], p[1]
    was_sfn = kind == 'state-fn' and not p[2].startswith('callback set in')
    now = sfn.get(fn)
    if kind.startswith('override') or kind.startswith('registrar'):
        if now and '+ state-fn' not in kind:
            cchanges.append(('KIND', fn, kind, kind + ' + state-fn'))
            p[1] = kind + ' + state-fn'
        cout.append('\t'.join(p))
        continue
    if not now and not was_sfn and not (kind == 'state-fn'):
        cout.append(l)
        continue
    old = list(p)
    if now:
        p[1] = 'state-fn'
        p[2] = ' '.join(now['uses'][:4]) + (' +%d' % (len(now['uses']) - 4) if len(now['uses']) > 4 else '')
        p[3] = ' / '.join(sorted(now['march'])) or '-'
        p[4] = 'pair' if now['march'] else 'june-only'
        ph = now['ph']
        if old[5] in PORTED:
            if ph != old[5]:
                moves.append((fn, old[5], ph, 'KEPT (ported)'))
            ph = old[5]
        elif ph != old[5]:
            moves.append((fn, old[5], ph, 'moved'))
        p[5] = ph
        if old[2].startswith('callback set in'):
            p[6] = ('CH0c: was "%s" (%s)' % (old[2], old[6])).strip()
        if old[1] == 'helper':
            p[6] = ('CH0c: also a state callback; was helper (%s)%s' % (old[2], ('; ' + old[6]) if old[6] else '')).strip()
    else:
        cchanges.append(('NO-LONGER-STATE-FN', fn, old, 'read by hand'))
    if p != old:
        cchanges.append(('ROW', fn, old, list(p)))
    cout.append('\t'.join(p))

# helpers follow their earliest in-class caller (ch0_build_class_map.py's fixed point), re-run on the new phases.
# Needs Ghidra's HTTP server (xrefs_to). Ported (CH1a / CH1b) and the builder's two hand-set rows keep their phase.
HAND = {'32ce1770', '32cc98e0'}


def xrefs(a):
    o = subprocess.run(['curl', '-s', '--noproxy', '127.0.0.1', '-m', '60', '-G', 'http://127.0.0.1:8080/xrefs_to',
                        '--data-urlencode', 'address=' + a], capture_output=True, text=True).stdout
    return [int(x, 16) for x in re.findall(r'From ([0-9a-f]{8})', o)]


funcs = sorted({int(m.group(1), 16) for m in re.finditer(r' at ([0-9a-f]{8})\s*$', open(os.path.join(HERE, '..', 'funcs.txt')).read(), re.M)
                if LO <= int(m.group(1), 16) < HI})


def owner(va):
    lo, hi, i = 0, len(funcs) - 1, 0
    while lo <= hi:
        m = (lo + hi) // 2
        if funcs[m] <= va:
            i, lo = m, m + 1
        else:
            hi = m - 1
    return funcs[i] if funcs and funcs[i] <= va else None


rows = {}
for l in cout:
    p = l.split('\t')
    if len(p) >= 7 and not l.startswith('#') and p[0] != 'june_fn':
        rows[p[0]] = p
helpers = [fn for fn, p in rows.items() if p[1] == 'helper' and fn not in HAND and p[5] not in PORTED]
callers = {fn: [owner(x) for x in xrefs(fn) if LO <= x < HI] for fn in helpers}
oldph = {fn: rows[fn][5] for fn in helpers}
hset = set(helpers)
base_rows = {}
for l in ctext:
    q = l.split('\t')
    if len(q) >= 7 and not l.startswith('#') and q[0] != 'june_fn':
        base_rows[q[0]] = q


def fixed_point(ph_of):
    """The builder's rule: a helper's phase is the earliest phase among its in-class callers, iterated to a fixed point
    (a helper not yet placed does not count, as in the builder)."""
    got, changed = {}, True
    while changed:
        changed = False
        for fn in helpers:
            phs = []
            for c in callers[fn]:
                c = '%08x' % c if c is not None else None
                if c is None or c == fn or c not in ph_of:
                    continue
                if c in hset:
                    if c in got:
                        phs.append(got[c])
                else:
                    phs.append(ph_of[c])
            phs = [x for x in phs if x in ORDER]
            ph = min(phs, key=ORDER.index) if phs else None
            if ph and (fn not in got or ORDER.index(ph) < ORDER.index(got[fn])):
                got[fn] = ph
                changed = True
    return got


# a helper moves only when the rule reproduces its old phase on the old map and the new map changes it
base = fixed_point({fn: q[5] for fn, q in base_rows.items()})
new = fixed_point({fn: q[5] for fn, q in rows.items()})
for fn in helpers:
    b0, n0 = base.get(fn, oldph[fn]), new.get(fn, oldph[fn])
    if b0 == n0:
        continue
    if b0 != oldph[fn]:
        moves.append((fn, oldph[fn], n0, 'NOT moved: the rule gives %s on the old map too (builder order); read by hand' % b0))
        continue
    moves.append((fn, oldph[fn], n0, 'moved (helper)'))
    rows[fn][5] = n0
    cchanges.append(('ROW', fn, [x if i != 5 else oldph[fn] for i, x in enumerate(rows[fn])], list(rows[fn])))
cout = [('\t'.join(rows[l.split('\t')[0]]) if (len(l.split('\t')) >= 7 and not l.startswith('#') and l.split('\t')[0] in rows) else l)
        for l in cout]

for c in changes:
    if c[0] == 'STATE':
        (oh, oc), (nh, nc) = c[2], c[3]
        print('STATE %s\n  how: %s\n   ->  %s\n  cb:  %s\n   ->  %s' % (c[1], oh, nh, oc, nc) if oh != nh else
              'STATE %s\n  cb:  %s\n   ->  %s' % (c[1], oc, nc))
    else:
        print(c)
for c in cchanges:
    if c[0] == 'ROW':
        o, n = c[2], c[3]
        diff = ['%s: %s -> %s' % (k, o[i], n[i]) for i, k in enumerate(['fn', 'kind', 'states', 'march', 'verdict', 'phase', 'note']) if o[i] != n[i]]
        print('MAP %s  %s' % (c[1], ' | '.join(diff)))
    else:
        print('MAP', c)
print('# phase moves:')
for m in moves:
    print('MOVE %s %s -> %s %s' % m)
print('# %d state rows changed, %d map rows changed, %d phase moves' % (len([c for c in changes if c[0] == 'STATE']),
      len([c for c in cchanges if c[0] == 'ROW']), len(moves)))

if WRITE:
    # the distinct-functions-by-phase totals line, from the new map rows (state-fn kind, Aisha range)
    body = [x.split('\t') for x in cout if len(x.split('\t')) >= 7 and not x.startswith('#') and not x.startswith('june_fn')]
    kc = collections.Counter((r[1].split(' ')[0], r[5]) for r in body)
    pc = collections.Counter(r[5] for r in body)
    for j, x in enumerate(cout):   # the builder's two totals lines (ch0_build_class_map.py), thunk / range counts kept
        m = re.match(r'# totals kind/phase: .*?(; thunks skipped: .*)$', x)
        if m:
            cout[j] = '# totals kind/phase: %s%s' % (', '.join('%s/%s=%d' % (k[0], k[1], v) for k, v in sorted(kc.items())), m.group(1))
        elif x.startswith('# totals per phase (functions to transcribe):'):
            cout[j] = '# totals per phase (functions to transcribe): %s' % ', '.join('%s=%d' % (k, pc[k]) for k in ORDER if pc[k])
    open(cp, 'w', encoding='utf-8', newline='\n').write('\n'.join(cout))
    fc = collections.Counter()
    for l in cout:
        p = l.split('\t')
        if len(p) >= 7 and 'state-fn' in p[1] and not l.startswith('#'):
            fc[p[5]] += 1
    rest_l = rest.split('\n')
    rest_l = [('# totals distinct Aisha-range state functions by phase: %s' % ', '.join('%s=%d' % kv for kv in sorted(fc.items()))
               if x.startswith('# totals distinct Aisha-range state functions by phase:') else x) for x in rest_l]
    open(sp, 'w', encoding='utf-8', newline='\n').write(new_part1 + '# part 2' + '\n'.join(rest_l))
    print('written')
