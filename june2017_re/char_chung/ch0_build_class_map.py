r"""char-chung CH0: build ch0_class_map.tsv, every function in 0x32bd86f0-0x32c25a1f incl. uncarved ones. Copy of char_eve/ch0_build_class_map.py.

    python june2017_re/char_chung/ch0_build_class_map.py        (no Ghidra needed; callers come from a whole-text rel32/imm32 scan)

Function starts: Ghidra-carved (funcs.txt) + 16-aligned addresses after int3 padding / ret + call/jmp/pointer targets after int3.
Kinds: override / registrar / state-fn / helper (phase = earliest caller) / thunk (5-byte jmp, not ported) / dead / not-Chung.
Inputs: ch0_vtable_slots.tsv, ch0_state_skill.tsv, ch0_hand_phases.tsv (fn, phase, why).
"""
import re, os, struct, collections, bisect
HERE = os.path.dirname(os.path.abspath(__file__))
RE = os.path.normpath(os.path.join(HERE, '..'))
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
B = 0x31aa0c00
LO, HI = 0x32bd86f0, 0x32c25a20
d = open(EXE, 'rb').read()
L = 0x1cbbe00
ORDER = ['CH1a', 'CH1b', 'CH1c', 'CH1d', 'CH1d2', 'CH1e', 'CH1f', 'CH1g', 'CH1g2', 'CH1h', 'CH1i', 'CH1j', 'CH1k', 'CH1l', 'CH1l2', 'CH1l3', 'CH1l4',
         'CH1m', 'CH1n', 'CH1o', 'CH1q', 'CH2a', 'CH2b', 'CH3b0', 'CH2c', 'CH2d', 'CH3a', 'CH3a2', 'CH3b', 'CH3c', 'CH3d', 'CH4a', 'CH4b', 'UNASSIGNED', 'n/a']
CH4 = {0x2b8: 'CH4b', 0x690: 'CH4a', 0x6a0: 'CH4b', 0x6a4: 'CH4b', 0x428: 'CH4b', 0x73c: 'CH4b', 0x24c: 'CH4a', 0x250: 'CH4a', 0x7ac: 'CH4a',
       0x7a4: 'CH1m', 0x7b4: 'CH1n', 0xa4: 'CH1o', 0x9c: 'CH1o', 0xa0: 'CH1o', 0x6a8: 'CH1o'}
REG = {'32c12a30': 'CH1m', '32c1d1d0': 'CH1m', '32c22540': 'CH1m'}
REGN = {'32c12a30': 'InitStateCommon (registers the common states, ids 7-205)', '32c1d1d0': 'InitState class-gated part (per-class combo / skill states)',
        '32c22540': 'InitState'}
funcs = {int(m.group(1), 16) for m in re.finditer(r' at ([0-9a-f]{8})\s*$', open(os.path.join(RE, 'funcs.txt')).read(), re.M)}
carved = {x for x in funcs if LO <= x < HI}
rel = collections.defaultdict(list)
ptr = collections.defaultdict(list)
for o in range(0x1000, L - 5):
    b = d[o]
    if b in (0xe8, 0xe9):
        t = (o + B + 5 + struct.unpack_from('<i', d, o + 1)[0]) & 0xffffffff
        if LO <= t < HI:
            rel[t].append((o + B, b))
for m in re.finditer(rb'(?=(.{2}[\xbd-\xc2]\x32))', d[:L], re.S):
    v = struct.unpack_from('<I', d, m.start())[0]
    if LO <= v < HI:
        ptr[v].append(m.start() + B)


def thunk(a):
    o = a - B
    return d[o] == 0xe9 and d[o + 5] == 0xcc


starts = set(carved)
for a in range(LO, HI, 16):
    o = a - B
    if d[o] != 0xcc and d[o - 1] in (0xcc, 0xc3):
        starts.add(a)
for t in list(rel) + list(ptr):
    if d[t - B - 1] == 0xcc and d[t - B] != 0xcc:
        starts.add(t)
S = sorted(starts)


def owner(va):
    i = bisect.bisect_right(S, va) - 1
    return S[i] if i >= 0 else None


rows = {}
vt = [l.split('\t') for l in open(os.path.join(HERE, 'ch0_vtable_slots.tsv'), encoding='utf-8').read().splitlines() if l and l[0] != '#' and not l.startswith('slot_off')]
for f in vt:
    slot, fn, name, march = int(f[0], 16), f[2], f[4], f[8]
    ph = CH4.get(slot) or ('CH1k' if slot < 0x600 else 'CH1l' if slot < 0x7c0 else 'CH1l4' if slot == 0x7cc else 'CH1l2' if slot < 0x7f0 else 'CH1l3')
    ins = 'new' if march.startswith('- (not') or (march.startswith('- (CX2GUChung does not') and name != 'HyperModeFrameMove') else 'inside'
    if name.startswith('SpecialAttackEventProcess part'):
        ins = 'inside'
    prop = '-' if ins == 'inside' else name
    if ins == 'new' and name == '?':
        ins = 'new?'
    if fn in rows:
        rows[fn][1] += ' + 0x%x' % slot
        rows[fn][2] += ' / ' + name
        continue
    rows[fn] = [fn, 'override 0x%x' % slot, name, march, ins, prop, ph, 'base %s' % f[3]]
for fn, ph in REG.items():
    if fn in rows:
        rows[fn][1] += ' + registrar'
        rows[fn][6] = ph
    else:
        rows[fn] = [fn, 'registrar', REGN[fn], 'X2GUChung.cpp InitStateCommon', 'inside', '-', ph, '']
rows['32c1f530'][1] += ' + registrar'
rows['32c1f530'][6] = 'CH1n'
st = open(os.path.join(HERE, 'ch0_state_skill.tsv'), encoding='utf-8').read().split('# part 2')[0].splitlines()
sfn = {}
for l in st:
    p = l.split('\t')
    if len(p) < 10 or p[1] == 'june_script_line' or p[7] == 'n/a':
        continue
    ins = dict(x.split('=', 1) for x in p[8].split(' | ') if '=' in x)
    prp = dict(x.split('=', 1) for x in p[9].split(' | ') if '=' in x)
    for slot, th, tgt in re.findall(r'(\w+)=([0-9a-f]{8})>([0-9a-f]{8})', p[4]):
        if not (LO <= int(tgt, 16) < HI):
            continue
        e = sfn.setdefault(tgt, {'uses': [], 'ph': 'n/a', 'ins': [], 'prop': []})
        e['uses'].append('%s.%s' % (p[0], slot))
        i = ins.get(slot, 'new')
        if i.startswith('inside'):
            e['ins'].append(i)
        elif prp.get(slot, '-') != '-':
            e['prop'].append(prp[slot])
        if p[7] in ORDER and ORDER.index(p[7]) < ORDER.index(e['ph']):
            e['ph'] = p[7]
for fn, e in sfn.items():
    if fn in rows:
        rows[fn][1] += ' + state-fn'
        continue
    u = list(dict.fromkeys(e['uses']))
    rows[fn] = [fn, 'state-fn', ' '.join(u[:3]) + (' +%d' % (len(u) - 3) if len(u) > 3 else ''), ' ; '.join(dict.fromkeys(e['ins'])) or '-',
                'inside' if e['ins'] else 'new', '-' if e['ins'] else (e['prop'][0] if e['prop'] else '?'), e['ph'], '%d states' % len(u)]
hand = {}
for l in open(os.path.join(HERE, 'ch0_hand_phases.tsv')).read().splitlines()[1:]:
    f = l.split('\t')
    hand[f[0]] = (f[1], f[2])
fm = {}
for l in open(os.path.join(RE, 'funcmap.tsv'), encoding='utf-8', errors='replace'):
    p = l.split('\t')
    for a in re.findall(r'\b([0-9a-f]{8})\b', p[0]):
        fm.setdefault(a, p[1] if len(p) > 1 else '')
name_ph = {}
for l in st:
    p_ = l.split('	')
    if len(p_) > 7 and p_[7] not in ('n/a', 'phase') and p_[0][:1] in 'CUX_p':
        name_ph[p_[0]] = p_[7]
name_sites = []
for n, ph_ in name_ph.items():
    j = d.find(b'\0' + n.encode() + b'\0')
    while j != -1:
        q_ = struct.pack('<I', j + 1 + B)
        k_ = d.find(q_, 0, L)
        while k_ != -1:
            if 0x32c1f530 <= k_ + B < 0x32c22250:
                name_sites.append((k_ + B, n, ph_))
            k_ = d.find(q_, k_ + 1, L)
        j = d.find(b'\0' + n.encode() + b'\0', j + 1)
name_sites.sort()


def nearest_state(x):
    best = None
    for a_, n_, p_ in name_sites:
        if a_ < x:
            best = (p_, n_)
    return best


cand = sorted(a for a in S if '%08x' % a not in rows and not thunk(a))
nthunk = sum(1 for a in S if thunk(a))
callers = {a: [(x, owner(x)) for x, k in rel.get(a, [])] for a in cand}
changed = True
while changed:
    changed = False
    for a in cand:
        fn = '%08x' % a
        if fn in hand:
            continue
        phs = [rows['%08x' % c][6] for x, c in callers[a] if c and c != a and '%08x' % c in rows and rows['%08x' % c][6] in ORDER]
        if phs:
            ph = min(phs, key=ORDER.index)
            if fn not in rows or ORDER.index(ph) < ORDER.index(rows[fn][6]):
                nm = fm.get(fn, '')
                chung = 'CX2GUChung' in nm
                outs = sorted({'%08x' % x for x, c in callers[a] if c is None or not (LO <= x < HI)})
                rows[fn] = [fn, 'helper', 'called by ' + ' '.join(sorted({'%08x' % c for x, c in callers[a] if c and c != a})[:4]), nm.split(' (')[0] if nm else '-',
                            'inside' if chung else 'new?', '-' if chung else '?', ph, ('also called from outside the class: ' + ' '.join(outs[:3])) if outs else '']
                changed = True
for a in cand:
    fn = '%08x' % a
    if fn in rows:
        continue
    if fn in hand:
        rows[fn] = [fn, 'helper', '-', '-', 'new?', '?', hand[fn][0], hand[fn][1]]
        continue
    pt = ptr.get(a, [])
    outside = [x for x, k in rel.get(a, []) if not (LO <= x < HI)]
    inside_c = [x for x, k in rel.get(a, []) if LO <= x < HI]
    fi = d[a - B:a - B + 8]
    if not rel.get(a) and not pt:
        if fi[:2] in (b'\x8b\x4d', b'\x8d\x4d', b'\x8b\x45', b'\x8d\x45'):
            rows[fn] = [fn, 'not-Chung', 'unwind funclet (opens with an ebp-relative load, no reference)', '-', '-', '-', 'n/a', '']
        else:
            rows[fn] = [fn, 'dead', '-', '-', '-', '-', 'n/a', 'no call, jmp or pointer reference in the exe; first bytes %s' % fi.hex()]
    elif not inside_c and outside and not pt:
        rows[fn] = [fn, 'not-Chung', 'called only from outside the range: ' + ' '.join('%08x' % x for x in outside[:3]), '-', '-', '-', 'n/a', 'shared / folded code']
    else:
        thk = [x for x, k in rel.get(a, []) if k == 0xe9 and thunk(owner(x) or 0) and owner(x) not in rows and '%08x' % owner(x) not in rows]
        if thk and not pt:
            tt = owner(thk[0])
            refs = []
            q = struct.pack('<I', tt)
            k = d.find(q, 0, L)
            while k != -1:
                refs.append(k + B)
                k = d.find(q, k + 1, L)
            if not refs:
                rows[fn] = [fn, 'dead', 'only reference is the unreferenced thunk %08x' % tt, '-', '-', '-', 'n/a', 'callback thunk never stored anywhere']
                continue
            nm = nearest_state(refs[0])
            if nm:
                sp, sn = nm
                rows[fn] = [fn, 'state-fn', '%s.? (callback thunk %08x stored @%08x; state = nearest preceding name push in 32c1f530)' % (sn, tt, refs[0]), '-', 'new?', '?', sp,
                            'state inferred by address order, not extracted: check the case']
                continue
        src = 'pointer @%08x' % pt[0] if pt else 'callers with no phased caller ' + ' '.join('%08x' % x for x, k in rel[a][:3])
        rows[fn] = [fn, 'helper', 'reached only through ' + src, '-', 'new?', '?', 'CH1o', 'phase unsettled: read it']
for a in S:
    fn = '%08x' % a
    if thunk(a) and fn not in rows:
        t = (a + 5 + struct.unpack_from('<i', d, a - B + 1)[0]) & 0xffffffff
        rows[fn] = [fn, 'thunk', 'jmp %08x' % t, '-', '-', '-', 'n/a', 'not ported']
for fn, (ph, why) in hand.items():
    if fn in rows and rows[fn][1] != 'thunk':
        rows[fn][6] = ph
        rows[fn][7] = why
# optional ch0_hand_names.tsv (fn, proposed_name, why): a name turns new? into new, or into inside when why starts with 'inside:'
HN_USED = 0
_hp = os.path.join(HERE, 'ch0_hand_names.tsv')
if os.path.exists(_hp):
    for l in open(_hp, encoding='utf-8').read().splitlines()[1:]:
        f = (l.split('	') + ['', '', ''])[:3]
        r_ = rows.get(f[0])
        if r_ and r_[4] == 'new?':
            HN_USED += 1
            if f[2].startswith('inside:'):
                r_[4], r_[5] = 'inside', '-'
                r_[7] = (r_[7] + '; ' if r_[7] else '') + f[2]
            else:
                r_[4], r_[5] = 'new', f[1]
                r_[7] = (r_[7] + '; ' if r_[7] else '') + 'name: ' + f[2]
with open(os.path.join(HERE, 'ch0_class_map.tsv'), 'w', encoding='utf-8', newline='\n') as o:
    o.write('june_fn\tkind\tname_or_states\tmarch_pair\tinside_new\tproposed_name\tphase\tnote\tcarved\n')
    for fn in sorted(rows):
        o.write('\t'.join(rows[fn]) + '\t%s\n' % ('y' if int(fn, 16) in carved else 'n'))
    kc = collections.Counter(r[1].split(' ')[0] for r in rows.values())
    o.write('# totals kind: %s; functions in range %d; carved %d; uncarved %d (thunks %d)\n' % (
        ', '.join('%s=%d' % kv for kv in sorted(kc.items())), len(rows), len(carved), len(rows) - len(carved), nthunk))
    skip = ('thunk', 'dead', 'not-Chung')
    pc = collections.Counter(r[6] for r in rows.values() if r[1] not in skip)
    pi = collections.Counter((r[6], r[4]) for r in rows.values() if r[1] not in skip)
    o.write('# totals per phase (to transcribe): %s\n' % ', '.join('%s=%d' % (k, pc[k]) for k in ORDER if pc[k]))
    o.write('# inside/new per phase: %s\n' % ', '.join('%s %s=%d' % (k[0], k[1], v) for k, v in sorted(pi.items(), key=lambda kv: (ORDER.index(kv[0][0]), kv[0][1]))))
print('hand names applied', HN_USED)
print(open(os.path.join(HERE, 'ch0_class_map.tsv'), encoding='utf-8').read().split('# totals', 1)[1])
