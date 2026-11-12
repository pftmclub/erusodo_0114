r"""char-elesis CH0: build ch0_class_map.tsv, every function in 0x32b4e770-0x32b90b6f incl. the uncarved overrides and thunk targets. Copy of char_ara/ch0_build_class_map.py.

    NO_HAND_NAMES=1 python june2017_re/char_elesis/ch0_build_class_map.py ; python june2017_re/char_elesis/ch0_make_hand_names.py ; python june2017_re/char_elesis/ch0_build_class_map.py

Function starts: Ghidra-carved (funcs.txt) + 16-aligned addresses after int3 padding / ret + call/jmp/pointer targets after int3.
Kinds: override / registrar / state-fn / helper (phase = earliest caller) / thunk (5-byte jmp, not ported) / dead / not-Elesis.
Override phase by vslot band: < 0x600 CH1h, 0x600-0x7bc CH1i, 0x7c0-0x7ec CH1j, >= 0x7f0 CH1k; exceptions in CH4 / REG and ch0_hand_phases.tsv.
SOFT = blood functions: CH4c unless the caller rule gives an earlier phase (conflicts are printed, never overridden silently).
Columns: june_fn, kind, name_or_states, march_pair, inside_new, proposed_name, phase, note, carved (y = Ghidra has a function at that start; n = listing-only).
Inputs: ch0_vtable_slots.tsv, ch0_state_skill.tsv, ch0_hand_phases.tsv (fn, phase, why, march_pair), ch0_hand_names.tsv.
"""
import capstone
import re, os, struct, collections, bisect
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
HERE = os.path.dirname(os.path.abspath(__file__))
RE = os.path.normpath(os.path.join(HERE, '..'))
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
B = 0x31aa0c00
LO, HI = 0x32b4e770, 0x32b90b70
d = open(EXE, 'rb').read()
L = 0x1cbbe00
ORDER = ['CH1a', 'CH1b', 'CH1c', 'CH1d', 'CH1d2', 'CH1e', 'CH1f', 'CH1f2', 'CH1g', 'CH1g2', 'CH1g3', 'CH1h', 'CH1i', 'CH1j', 'CH1k', 'CH1m', 'CH1n', 'CH1o', 'CH1p', 'CH1q', 'CH1q2', 'CH1',
         'CH2a', 'CH2b0', 'CH2b', 'CH2b2', 'CH2c0', 'CH2c', 'CH2c2', 'CH2d0', 'CH2d', 'CH2d2', 'CH2d3', 'CH3a', 'CH3a2', 'CH3b0', 'CH3b', 'CH3b2', 'CH3c0', 'CH3c', 'CH3c2', 'CH3d', 'CH4a', 'CH4b', 'CH4c', 'UNASSIGNED', 'n/a']
# overrides that leave the vslot band: InitState 0x7a4 CH1m, SetEquippedSkillFunc 0x7b4 CH1n, InitPassiveSkillState 0x690 CH4a,
# the other CH4 / CH1q / CH3b0 / CH4c / CH1o overrides are hand rows (ch0_hand_phases.tsv)
CH4 = {0x690: 'CH4a', 0x7a4: 'CH1m', 0x7b4: 'CH1n'}
REG = {'32b81be0': 'CH1m', '32b864f0': 'CH1m', '32b87a60': 'CH1m'}
SOFT = ['32b87880', '32b734d0', '32b7e3b0', '32b6ff20', '32b7e9a0', '32b7e740', '32b81220']   # blood functions: CH4c unless an earlier caller phase
SOFT_PH = 'CH4c'
REGN = {'32b81be0': 'InitState part 1 (registrations + the hyper row id 23); called by the wrapper 32b87a60', '32b864f0': 'InitState part 2 (registrations); called by the wrapper 32b87a60', '32b87a60': 'InitState wrapper (START / WIN / LOSE roll rows 9 / 11 / 12 + id 130, then the parts + 3202b110)'}
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
for m in re.finditer(rb'(?=(.{2}[\xb9-\xbd]\x32))', d[:L], re.S):
    v = struct.unpack_from('<I', d, m.start())[0]
    if LO <= v < HI:
        ptr[v].append(m.start() + B)


def thunk(a):
    o = a - B
    return 0 <= o < len(d) - 6 and d[o] == 0xe9 and d[o + 5] == 0xcc   # an out-of-image owner (a caller below the range) is no thunk


def after_ret(a, known):
    """char-elesis CH0 fix: the byte before `a` is a real ret / int3 only if the instructions from the previous known start reach `a`
    exactly and end on ret / int3 / jmp (a byte test took 32caf980 after `cmp eax, ebx` (3b c3) inside InitState 32cadc90 for a start)."""
    i = bisect.bisect_left(known, a) - 1
    if i < 0:
        return True
    p, last, end = known[i], None, None
    for ins in md.disasm(d[p - B:a - B], p):
        last, end = ins.mnemonic, ins.address + ins.size
    if not (end == a and last in ('ret', 'int3', 'jmp')):
        return False
    for o in range(a - B - 0x4000, a - B + 0x4000):   # a block its own function jumps to (Win 32ca7860's tail 32ca78a0) is no start
        op = d[o]
        if (0x70 <= op <= 0x7f or op == 0xeb) and o + 2 + struct.unpack_from('<b', d, o + 1)[0] == a - B:
            return False
        if op == 0x0f and 0x80 <= d[o + 1] <= 0x8f and o + 6 + struct.unpack_from('<i', d, o + 2)[0] == a - B:
            return False
    return True


# char-elesis CH0 fix: every vtable target is a start (InitPassiveSkillState 32ca78b0 is uncarved and follows Win's `ret 4`, so the byte test
# missed it and its body was credited to Win 32ca7860); a 16-aligned address after `ret N` (c2 xx xx) is a candidate too.
VTF = {int(l.split('	')[2], 16) for l in open(os.path.join(HERE, 'ch0_vtable_slots.tsv'), encoding='utf-8').read().splitlines()
       if l and l[0] != '#' and not l.startswith('slot_off')}
KNOWN = sorted(carved | set(rel) | set(ptr) | VTF)
starts = set(carved) | VTF
for a in range(LO, HI, 16):
    o = a - B
    if d[o] != 0xcc and (d[o - 1] in (0xcc, 0xc3) or d[o - 3] == 0xc2):
        if a not in carved and a not in rel and a not in ptr and not after_ret(a, KNOWN):
            continue
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
    slot, fn, name, march = int(f[0], 16), f[2], f[4], f[6]
    ph = CH4.get(slot) or ('CH1h' if slot < 0x600 else 'CH1i' if slot < 0x7c0 else 'CH1j' if slot < 0x7f0 else 'CH1k')
    ins = 'new' if march.startswith('- (') else 'inside'
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
        rows[fn] = [fn, 'registrar', REGN[fn], 'X2GUEl.cpp InitState', 'inside', '-', ph, '']
rows['32b8c470'][1] += ' + registrar'
rows['32b8c470'][6] = 'CH1n'
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
# char-elesis CH0: 32b8c470 / 32b8c470 hand the state setup of most skills to small class functions (call helper( state*, id ): vtable Init, name -> id map, callbacks).
# Each is one state's registration, outlined from the switch case: it goes inside March's SetEquippedSkillFuncBySkillSlotIndex with the phase of its state.
state_phase_by_name = {}
for l in st:
    p_ = l.split('	')
    if len(p_) > 7 and p_[7] not in ('n/a', 'phase') and p_[0].startswith(('ESI_', 'USI_', 'SI_')):
        state_phase_by_name[p_[0]] = p_[7]
import capstone as _cs
_md = _cs.Cs(_cs.CS_ARCH_X86, _cs.CS_MODE_32)
_nxt = {a: (S[i + 1] if i + 1 < len(S) else HI) for i, a in enumerate(S)}
SW_HELPERS = 0
for a in S:
    fn = '%08x' % a
    if fn in rows or thunk(a):
        continue
    names = []
    for ins_ in _md.disasm(d[a - B:min(_nxt[a], a + 0x120) - B], a):
        m_ = re.match(r'^0x(3[0-9a-f]{7})$', ins_.op_str) if ins_.mnemonic == 'push' else None
        if m_:
            o_ = int(m_.group(1), 16) - B
            e_ = d.find(b'\x00', o_)
            nm_ = d[o_:e_].decode('latin-1') if 0 <= o_ < len(d) and 0 < e_ - o_ < 100 else ''
            if re.fullmatch(r'(ESI|USI|SI)_\w+', nm_) and nm_ in state_phase_by_name:
                names.append(nm_)
        if ins_.mnemonic == 'ret':
            break
    names = list(dict.fromkeys(names))
    if names and any(ins_ for ins_ in [1]):
        phs = [state_phase_by_name[n] for n in names]
        ph_ = min(phs, key=ORDER.index)
        callers_ = [x for x, k in rel.get(a, [])]
        if callers_ and all(0x32b8c470 <= x < 0x32b8eabc for x in callers_):
            rows[fn] = [fn, 'helper', 'state registration helper of ' + ' '.join(names), 'X2GUEl.cpp SetEquippedSkillFuncBySkillSlotIndex (case body outlined into a helper)', 'inside', '-', ph_,
                        'called by %s' % ' '.join('%08x' % x for x in callers_[:4])]
            SW_HELPERS += 1
print('state registration helpers', SW_HELPERS)
hand, hand_pair = {}, {}
for l in open(os.path.join(HERE, 'ch0_hand_phases.tsv')).read().splitlines()[1:]:
    f = l.split('\t') + ['']
    if os.environ.get('NO_HAND_PHASES'):   # rule-only run (to compare the hand rows with the caller rule)
        break
    hand[f[0]] = (f[1], f[2])
    if f[3]:
        hand_pair[f[0]] = f[3]   # char-elesis CH0: the March function the June body goes inside (e.g. one only under force-power flag)
fm = {}
for l in open(os.path.join(RE, 'funcmap.tsv'), encoding='utf-8', errors='replace'):
    p = l.split('\t')
    for a in re.findall(r'\b([0-9a-f]{8})\b', p[0]):
        fm.setdefault(a, p[1] if len(p) > 1 else '')
name_ph = {}
for l in st:
    p_ = l.split('	')
    if len(p_) > 7 and p_[7] not in ('n/a', 'phase') and p_[0].startswith(('ESI_', 'USI_', 'SI_')):
        name_ph[p_[0]] = p_[7]
name_sites = []
for n, ph_ in name_ph.items():
    j = d.find(b'\0' + n.encode() + b'\0')
    while j != -1:
        q_ = struct.pack('<I', j + 1 + B)
        k_ = d.find(q_, 0, L)
        while k_ != -1:
            if 0x32b8c470 <= k_ + B < 0x32b8eabc:
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
CS = sorted(carved)


def owner_c(va):
    """the Ghidra-carved function holding va (a bogus 16-aligned start inside a body would give the wrong owner), else the heuristic owner"""
    i = bisect.bisect_right(CS, va) - 1
    h = owner(va)
    if h is not None and h in VTF and (i < 0 or h > CS[i]):   # char-elesis CH0: an uncarved override between the carved owner and va owns va
        return h
    return CS[i] if i >= 0 and va - CS[i] < 0x2000 and '%08x' % CS[i] in rows else h


def ph_of(c):
    cf = '%08x' % c
    if cf in hand:
        return hand[cf][0]
    return rows[cf][6] if cf in rows else None


SOFT_RULE = {}
for _f in SOFT:
    if _f in rows and not os.environ.get('NO_SOFT'):   # an override / registrar row: min(its own phase, CH4c)
        SOFT_RULE[_f] = rows[_f][6]
        if ORDER.index(SOFT_PH) < ORDER.index(rows[_f][6]):
            rows[_f][6] = SOFT_PH
callers = {a: [(x, owner(x)) for x, k in rel.get(a, [])] + [(x, owner_c(x)) for x in ptr.get(a, []) if LO <= x < HI] for a in cand}   # a code pointer inside the class (push / mov imm32) counts as a caller
changed = True
while changed:
    changed = False
    for a in cand:
        fn = '%08x' % a
        if fn in hand:
            continue
        phs = [p_ for x, c in callers[a] if c and c != a for p_ in [ph_of(c)] if p_ in ORDER]
        if fn in SOFT and not os.environ.get('NO_SOFT'):
            SOFT_RULE[fn] = min(phs, key=ORDER.index) if phs else None
            phs.append(SOFT_PH)
        if phs:
            ph = min(phs, key=ORDER.index)
            if fn not in rows or ORDER.index(ph) < ORDER.index(rows[fn][6]):
                nm = fm.get(fn, '')
                chung = 'CX2GUEL' in nm
                outs = sorted({'%08x' % x for x, c in callers[a] if c is None or not (LO <= x < HI)})
                rows[fn] = [fn, 'helper', 'called by ' + ' '.join(sorted({'%08x' % c for x, c in callers[a] if c and c != a})[:4]), nm.split(' (')[0] if nm else '-',
                            'inside' if chung else 'new?', '-' if chung else '?', ph, ('also called from outside the class: ' + ' '.join(outs[:3])) if outs else '']
                changed = True
for a in cand:
    fn = '%08x' % a
    if fn in rows:
        continue
    if fn in SOFT and not os.environ.get('NO_SOFT'):
        SOFT_RULE[fn] = None
        rows[fn] = [fn, 'helper', 'blood function (no phased caller in the class map)', fm.get(fn, '-'), 'new?', '?', SOFT_PH, '']
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
            rows[fn] = [fn, 'not-Elesis', 'unwind funclet (opens with an ebp-relative load, no reference)', '-', '-', '-', 'n/a', '']
        else:
            rows[fn] = [fn, 'dead', '-', '-', '-', '-', 'n/a', 'no call, jmp or pointer reference in the exe; first bytes %s' % fi.hex()]
    elif not inside_c and outside and not pt:
        rows[fn] = [fn, 'not-Elesis', 'called only from outside the range: ' + ' '.join('%08x' % x for x in outside[:3]), '-', '-', '-', 'n/a', 'shared / folded code']
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
                rows[fn] = [fn, 'state-fn', '%s.? (callback thunk %08x stored @%08x; state = nearest preceding name push in 32b8c470)' % (sn, tt, refs[0]), '-', 'new?', '?', sp,
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
        if fn in hand_pair:
            rows[fn][3], rows[fn][4], rows[fn][5] = hand_pair[fn], 'inside', '-'
        if ph == 'n/a':
            rows[fn][4], rows[fn][5] = '-', '-'
        if why.startswith('helper:') and rows[fn][1] == 'not-Elesis':
            rows[fn][1], rows[fn][2], rows[fn][4], rows[fn][5] = 'helper', 'called only from outside the class (base code)', 'new?', '?'
        if why.startswith('ctor:') and rows[fn][1] == 'not-Elesis':
            rows[fn][1], rows[fn][2], rows[fn][3], rows[fn][4], rows[fn][5] = 'ctor', 'constructor (caller outside the class)', 'X2GUEl.h constructor', 'inside', '-'
# optional ch0_hand_names.tsv (fn, proposed_name, why): a name turns new? into new, or into inside when why starts with 'inside:'
HN_USED = 0
_hp = os.path.join(HERE, 'ch0_hand_names.tsv')
if os.path.exists(_hp) and not os.environ.get('NO_HAND_NAMES'):
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
OUT = os.environ.get('OUTFILE') or os.path.join(HERE, 'ch0_class_map.tsv')
for _f, _r in sorted(SOFT_RULE.items()):
    if rows.get(_f) and rows[_f][6] != SOFT_PH:
        print('SOFT CONFLICT %s: caller rule gives %s (kept), not %s' % (_f, rows[_f][6], SOFT_PH))
with open(OUT, 'w', encoding='utf-8', newline='\n') as o:
    o.write('june_fn\tkind\tname_or_states\tmarch_pair\tinside_new\tproposed_name\tphase\tnote\tcarved\n')
    for fn in sorted(rows):
        o.write('\t'.join(rows[fn]) + '\t%s\n' % ('y' if int(fn, 16) in carved else 'n'))
    kc = collections.Counter(r[1].split(' ')[0] for r in rows.values())
    o.write('# totals kind: %s; functions in range %d; carved %d; uncarved %d (thunks %d)\n' % (
        ', '.join('%s=%d' % kv for kv in sorted(kc.items())), len(rows), len(carved), len(rows) - len(carved), nthunk))
    skip = ('thunk', 'dead', 'not-Elesis')
    pc = collections.Counter(r[6] for r in rows.values() if r[1] not in skip)
    pi = collections.Counter((r[6], r[4]) for r in rows.values() if r[1] not in skip)
    o.write('# totals per phase (to transcribe): %s\n' % ', '.join('%s=%d' % (k, pc[k]) for k in ORDER if pc[k]))
    o.write('# inside/new per phase: %s\n' % ', '.join('%s %s=%d' % (k[0], k[1], v) for k, v in sorted(pi.items(), key=lambda kv: (ORDER.index(kv[0][0]), kv[0][1]))))
    o.write('# thunk tables: 409 slots = 402 jmp thunks (402 distinct state-fn targets, all rows) + 7 delegate adapters (push edx; call <delegate>; ret) at 32b8ecf0 32b8ed00 32b8ffd0 32b8ffe0 32b906d0 32b90ab0 32b90ac0, each pushed by InitPassiveSkillState 32b88300 (@32b88342 @32b88a76 @32b8835e @32b88aac @32b8837a @32b88c21 @32b8903c); delegates 32b52470 32b524b0 32b6fd50 32b6f820 32b7e3b0 32b872d0 32b87880 = CH4a / CH4c (char-elesis CH0 Opus)\n')
    o.write('# overrides: 66 = 65 in range (32 uncarved) + 0x4b0 32c69f70 (folded GetComboZStateID stub, out of range, uncarved)\n')
print('hand names applied', HN_USED)
print(open(OUT, encoding='utf-8').read().split('# totals', 1)[1])
