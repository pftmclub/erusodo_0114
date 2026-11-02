r"""char-luciel CH0: build ch0_class_map.tsv, every function start in 32ae4080-32b4e76f incl. uncarved ones, jmp thunks and the thunk tables.
Adapted from char_chung/ch0_build_class_map.py (no March class: every row is new, inside_new = new).

    python june2017_re/char_luciel/ch0_build_vtable.py
    python june2017_re/char_luciel/ch0_build_state_skill.py
    python june2017_re/char_luciel/ch0_build_class_map.py        (no Ghidra needed; callers come from a whole-text rel32 / imm32 scan)

Function starts: Ghidra-carved (funcs.txt) + 16-aligned addresses after int3 padding / ret + call / jmp / pointer targets after int3.
Columns: june_fn, kind, vslot, proposed_name, march_pair, phase, note, carved, callers.
Kinds: override 0x.. / own-virtual 0x.. / registrar / state-fn / thunk / helper / dead / not-luciel. A thunk is a 5-byte `jmp` (or the vcall stub
mov eax,[ecx]; jmp [eax+0x888], a delegate maker: mov eax,[esp+4]; mov ecx,[esp+8]; mov [eax],ecx; mov [eax+4],imm32; ret, a callback shim
push edx; call T; ret, or a this-adjustor sub ecx,imm; jmp T): not transcribed, its phase is the phase of its target. Phases:
 * ctor 32b1bd90, dtor 32b39380, scalar deleting dtor 32b4e660, own virtuals 0x880 / 0x884 / 0x888 and the MemoConditionLuciel ctor 32aeb3f0 CH1a;
   MemoConditionLuciel evaluation 32ae5150 CH4c; registrars 32b43d10 / 32b3e8b0 CH1m; skill setter 32b46580 CH1n.
 * state callbacks: the phase of the state they serve (ch0_state_skill.tsv). A callback used by several states takes the earliest phase.
 * overrides: vslot < OVR_SPLIT CH1k, >= OVR_SPLIT CH1l, except the damage-state getters DU2 calls ( 0x114 0x118 0x458 0x48c 0x490 0x494 ) CH1l,
   InitPassiveSkillState 0x690 and the buff-value setters 0x24c / 0x250 CH4a.
 * helpers: the phase of the earliest referrer ( call, jmp or an immediate pointer in code; a thunk is transparent ). A phase with more than SPLIT
   functions to transcribe is split into <phase>, <phase>2 ... in table order.
"""
import re, os, sys, struct, collections, bisect
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ch0_lu_data as D

HERE, B, LO, HI, T = D.HERE, D.B, D.LO, D.HI, D.T
RE = os.path.join(T, 'june2017_re')
d = open(D.EXE, 'rb').read()
L = 0x1cbbe00
SPLIT = 45
ORDER = ['CH1a', 'CH1b', 'CH1d', 'CH1f', 'CH1g', 'CH1h', 'CH1i', 'CH1k', 'CH1l', 'CH1m', 'CH1n', 'CH1o', 'CH1p',
         'CH2a', 'CH2b', 'CH2c', 'CH2d', 'CH3a', 'CH3b', 'CH3c', 'CH3d', 'CH4a', 'CH4b', 'CH4c', 'UNASSIGNED', 'n/a']
GETTERS = {0x114, 0x118, 0x458, 0x48c, 0x490, 0x494}
CH4_SLOTS = {0x690: 'CH4a', 0x24c: 'CH4a', 0x250: 'CH4a'}
REG = {'32b43d10': 'CH1m', '32b3e8b0': 'CH1m'}
REGN = {'32b3e8b0': ('InitStateCommon', 'registers the common states ( 294 UpdateUserUnitStateData calls ): ids 7-205 and the Lu / Ciel common states'),
        '32b43d10': ('InitState', 'sets +0x8ac8 by unit class ( 37 38 39 200 -> 0, 127 -> 1, 128 -> 2, 129 -> 3 ), +0x7d04 = +0x4088 = 0xd, +0x408c = 0x28, calls 32b3e8b0, 3202b110, 31fe22d0, 32078bc0, registers 65 states')}
HAND_PHASE = {'32b1bd90': ('CH1a', 'CX2GULuciel', 'ctor ( writes 334f06fc at 32b1bde2, calls CX2GUUser ctor 3203e850 with 0x251 )'),
              '32b39380': ('CH1a', '~CX2GULuciel', 'dtor ( rewrites 334f06fc at 32b393ac )'),
              '32aeb3f0': ('CH1a', 'MemoConditionLuciel::MemoConditionLuciel', 'ctor of the nested MemoConditionLuciel ( vtable 334f036c; base ctor 31fc8cd0 )'),
              '32ae5150': ('CH4c', 'MemoConditionLuciel::IsCondition', 'MemoConditionLuciel vtable 334f036c slot 1: 31fd4450( arg ) != 0 ( evaluation logic )'),
              '32ae5170': ('CH4c', '-', 'MemoConditionLuciel vtable 334f036c slot 2: jmp 31fc8cf0 ( thunk )'),
              '32b14bc0': ('CH1p', 'AddCombinationPointOnHit', 'combination-point delegate InitState pushes ( HIT_ADD_COMBINATION_POINT consumer ): CH1p owns combination points')}

# ---- function starts ----
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
jcc = collections.defaultdict(list)
for o in range(0x1000, L - 6):
    if d[o] == 0x0f and 0x80 <= d[o + 1] <= 0x8f:
        t = (o + B + 6 + struct.unpack_from('<i', d, o + 2)[0]) & 0xffffffff
        if LO <= t < HI:
            jcc[t].append(o + B)
    elif d[o] == 0xeb or 0x70 <= d[o] <= 0x7f:
        t = (o + B + 2 + struct.unpack_from('<b', d, o + 1)[0]) & 0xffffffff
        if LO <= t < HI and LO <= o + B < HI:
            jcc[t].append(o + B)
for m in re.finditer(rb'(?=(.{2}[\xae-\xb4]\x32))', d[:L], re.S):
    v = struct.unpack_from('<I', d, m.start())[0]
    if LO <= v < HI:
        ptr[v].append(m.start() + B)


def kind_stub(a):
    """'jmp' (5-byte jmp rel32 + int3), 'vcall' (mov eax,[ecx]; jmp [eax+off]), 'maker' (delegate maker), 'pushcall' (push edx; call rel32; ret: a
    callback shim that forwards edx), 'adjust' (sub ecx, imm32; jmp rel32: a this-adjustor thunk) or None"""
    o = a - B
    if d[o] == 0xe9 and d[o + 5] == 0xcc:
        return 'jmp'
    if d[o:o + 4] == bytes.fromhex('8b01ffa0') and d[o + 8] == 0xcc:
        return 'vcall'
    if d[o:o + 13] == bytes.fromhex('8b4424048b4c240889' + '08c74004') and d[o + 17] == 0xc3:
        return 'maker'
    if d[o:o + 2] == bytes.fromhex('52e8') and d[o + 6] == 0xc3:
        return 'pushcall'
    if d[o:o + 2] == bytes.fromhex('81e9') and d[o + 6] == 0xe9:
        return 'adjust'
    return None


starts = set(carved)
def padded_end(o):
    """the bytes before file offset o, minus the trailing int3 run, end like a function ( ret, ret imm16, jmp rel32 / rel8 / indirect ) and the first
    instructions at o decode"""
    k = o
    while k > 0 and d[k - 1] == 0xcc:
        k -= 1
    if k == o:
        return False
    t = d[max(0, k - 12):k]
    end_ok = (t[-1] == 0xc3 or (t[-3] == 0xc2 and t[-1] == 0) or t[-5] == 0xe9 or t[-2] == 0xeb
              or (t[-2] == 0xff and 0xe0 <= t[-1] <= 0xe7) or (t[-6] == 0xff and t[-5] == 0x24) or (t[-7] == 0xff and t[-6] == 0x25) or (t[-8] == 0xff and t[-7] == 0x24))
    if not end_ok:
        return False
    return True


for a in range(LO, HI, 16):
    o = a - B
    if d[o] != 0xcc and d[o - 1] == 0xcc and padded_end(o):
        starts.add(a)
for t in list(rel) + list(ptr):
    if d[t - B - 1] == 0xcc and d[t - B] != 0xcc:
        starts.add(t)
starts.add(LO)
# a 16-aligned address right after a `ret` byte (no int3 padding) is a function start only if a linear sweep from the previous start ends a
# ret exactly there ( a stray 0xc3 inside an instruction is not a boundary )
try:
    import capstone as _cs
    _md = _cs.Cs(_cs.CS_ARCH_X86, _cs.CS_MODE_32)
except Exception:
    _md = None
NOPAD = 0
for a in range(LO, HI, 16):
    o = a - B
    if a in starts or d[o] == 0xcc or d[o - 1] not in (0xc3,):
        continue
    prev = max([x for x in starts if x < a], default=None)
    if prev is None or _md is None:
        continue
    last = None
    ok = False
    for i in _md.disasm(d[prev - B:o + 16], prev):
        if i.address == a:
            ok = last is not None and last.mnemonic in ('ret', 'jmp')
            break
        if i.address > a:
            break
        last = i
    if ok:
        starts.add(a)
        NOPAD += 1
S = sorted(s for s in starts if LO <= s < HI)


def owner(va):
    i = bisect.bisect_right(S, va) - 1
    return S[i] if i >= 0 and va < HI else None


def stub_target(a):
    k = kind_stub(a)
    o = a - B
    if k == 'jmp':
        return (a + 5 + struct.unpack_from('<i', d, o + 1)[0]) & 0xffffffff
    if k == 'pushcall':
        return (a + 6 + struct.unpack_from('<i', d, o + 2)[0]) & 0xffffffff
    if k == 'adjust':
        return (a + 11 + struct.unpack_from('<i', d, o + 7)[0]) & 0xffffffff
    if k == 'maker':
        return struct.unpack_from('<I', d, o + 13)[0]
    if k == 'vcall':
        return ('vslot', struct.unpack_from('<I', d, o + 4)[0])
    return None


# ---- vtable overrides ----
rows = {}
vt = [l.split('\t') for l in open(os.path.join(HERE, 'ch0_vtable_slots.tsv'), encoding='utf-8').read().splitlines() if l and l[0] != '#' and not l.startswith('slot_off')]
ovr = []
for f in vt:
    slot, fn, name, march = int(f[0], 16), f[2], f[4], f[6]
    ovr.append((slot, fn, name, march, f[3]))
# the vslot boundary: the median of the overrides that are left to CH1k / CH1l
free = sorted(s for s, fn, nm, mr, bf in ovr if s not in (0x0, 0x7a4, 0x7b4, 0x880, 0x884, 0x888) and s not in CH4_SLOTS and fn not in HAND_PHASE)
OVR_SPLIT = free[len(free) // 2]
for slot, fn, name, march, bfn in ovr:
    own = slot in (0x880, 0x884, 0x888)
    if fn in rows:
        rows[fn][1] += ' + 0x%x' % slot
        rows[fn][3] += ' / ' + name
        continue
    if slot == 0:
        ph = 'CH1a'
    elif own:
        ph = 'CH1a'
    elif slot in CH4_SLOTS:
        ph = CH4_SLOTS[slot]
    elif slot in GETTERS:
        ph = 'CH1l'
    else:
        ph = 'CH1k' if slot < OVR_SPLIT else 'CH1l'
    kind = ('own-virtual 0x%x' if own else 'override 0x%x') % slot
    note = 'base %s' % bfn if not own else 'Lu/Ciel-only virtual ( no base entry )'
    rows[fn] = [fn, kind, '0x%x' % slot, name, '-' if own or march.startswith('- (') else march, ph, note]
for fn, (ph, nm, why) in HAND_PHASE.items():
    if fn in rows:
        rows[fn][5] = ph
        rows[fn][6] += '; ' + why
    else:
        rows[fn] = [fn, 'helper', '-', nm, '-', ph, why]
for fn, ph in REG.items():
    if fn in rows:
        rows[fn][1] += ' + registrar'
        rows[fn][5] = ph
        rows[fn][6] += '; ' + REGN[fn][1]
    else:
        rows[fn] = [fn, 'registrar', '-', REGN[fn][0], '-', ph, REGN[fn][1]]
rows['32b46580'][1] += ' + registrar'
rows['32b46580'][5] = 'CH1n'
rows['32b46580'][6] += '; skill setter ( not carved in Ghidra; ch0_skill.tsv ): 64 cases'
rows['32b43d10'][3] = 'InitState'
rows['32b3e8b0'][6] = REGN['32b3e8b0'][1]

# ---- state callbacks ----
st = open(os.path.join(HERE, 'ch0_state_skill.tsv'), encoding='utf-8').read().split('# part 2')[0].splitlines()
sfn = collections.OrderedDict()
for l in st:
    p = l.split('\t')
    if len(p) < 10 or p[0] == 'state' or p[7] == 'n/a' or l.startswith('#'):
        continue
    prp = dict(x.split('=', 1) for x in p[9].split(' | ') if '=' in x)
    for slot, th, tgt in re.findall(r'(\w+)=([0-9a-f]{8})>([0-9a-f]{8})', p[4]):
        if not (LO <= int(tgt, 16) < HI):
            continue
        e = sfn.setdefault(tgt, {'uses': [], 'ph': 'n/a', 'prop': []})
        e['uses'].append('%s.%s' % (p[0], slot))
        if prp.get(slot, '-') != '-':
            e['prop'].append(prp[slot])
        if p[7] in ORDER and ORDER.index(p[7]) < ORDER.index(e['ph']):
            e['ph'] = p[7]
for fn, e in sfn.items():
    if kind_stub(int(fn, 16)):
        continue   # a callback that is itself a stub ( vcall stub, shim ): classified as a thunk below
    if fn in rows:
        rows[fn][1] += ' + state-fn'
        continue
    u = list(dict.fromkeys(e['uses']))
    rows[fn] = [fn, 'state-fn', '-', e['prop'][0] if e['prop'] else '?', '-', e['ph'],
                '%d use%s: %s%s' % (len(u), '' if len(u) == 1 else 's', ' '.join(u[:4]), (' +%d more' % (len(u) - 4)) if len(u) > 4 else '')]

# ---- helpers / thunks / dead ----
fm = {}
for l in open(os.path.join(RE, 'funcmap.tsv'), encoding='utf-8', errors='replace'):
    p = l.split('\t')
    for a in re.findall(r'\b([0-9a-f]{8})\b', p[0]):
        fm.setdefault(a, p[1] if len(p) > 1 else '')

try:
    import capstone
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
except Exception:
    md = None


def strs(a):
    out = []
    if md is None:
        return out
    n = 0
    for i in md.disasm(d[a - B:a - B + 0x600], a):
        n += 1
        m = re.search(r'0x(33[0-9a-f]{6})\b', i.op_str)
        if m and i.mnemonic in ('push', 'mov', 'lea'):
            v = int(m.group(1), 16)
            o = v - B
            e = d.find(b'\0\0', o)
            try:
                t = d[o:e + 1].decode('utf-16le') if 0 <= e - o < 200 else ''
                t = t.split('\0')[0]
            except Exception:
                t = ''
            if not (t and all(32 <= ord(c) < 127 for c in t)):
                e = d.find(b'\0', o)
                t = d[o:e].decode('latin-1') if 0 < e - o < 100 and all(32 <= c < 127 for c in d[o:e]) else ''
            if len(t) >= 3 and t not in out:
                out.append(t)
        if i.mnemonic == 'ret' or n > 400:
            break
    return out[:2]


def insn_count(a):
    if md is None:
        return 0
    n = 0
    for i in md.disasm(d[a - B:a - B + 0x800], a):
        n += 1
        if i.mnemonic == 'ret' and n > 1:
            break
    return n


cand = sorted(a for a in S if '%08x' % a not in rows and not kind_stub(a))
STUB = {a: kind_stub(a) for a in S if kind_stub(a) and '%08x' % a not in rows}
STUBT = {a: stub_target(a) for a in STUB}


def referrers(a):
    """owners of every call / jmp site and every immediate reference to a (a data-table pointer counts as outside)"""
    out = []
    for x, k in rel.get(a, []):
        out.append(owner(x) if LO <= x < HI else ('out', x))
    for x in ptr.get(a, []):
        out.append(owner(x) if (LO <= x < HI) else ('out', x))
    return out


def eff_referrers(a, seen=None):
    """referrers of a; a stub that references or targets a is transparent (its own referrers count)"""
    seen = seen if seen is not None else set()
    res = []
    for r in referrers(a):
        if isinstance(r, int) and r in STUB:
            if r not in seen:
                seen.add(r)
                res += eff_referrers(r, seen)
        else:
            res.append(r)
    for w, t in STUBT.items():
        if t == a and w not in seen:
            seen.add(w)
            res += eff_referrers(w, seen)
    return res


EREF = {a: eff_referrers(a) for a in cand}
# checklist 7am: InitState 32b43d10 pushes the ten passive delegate callbacks; CH0 gives them ( and what only they reach ) to CH4a,
# CH1m declares them with "body in CH4a"
DELEGATES = {0x32b14bc0, 0x32b3d380, 0x32b0fc50, 0x32b43590, 0x32b435f0, 0x32b0fce0, 0x32b177b0, 0x32b13c80, 0x32b17f90, 0x32b3e470}


def ref_phase(c, a):
    if c == 0x32b43d10 and a in DELEGATES:
        return 'CH4a'
    return rows['%08x' % c][5]


changed = True
while changed:
    changed = False
    for a in cand:
        fn = '%08x' % a
        phs = [ref_phase(c, a) for c in EREF[a] if isinstance(c, int) and c != a and '%08x' % c in rows and rows['%08x' % c][5] in ORDER
               and rows['%08x' % c][5] not in ('n/a', 'UNASSIGNED')]
        if phs:
            ph = min(phs, key=ORDER.index)
            if fn not in rows or ORDER.index(ph) < ORDER.index(rows[fn][5]):
                nm = fm.get(fn, '')
                outs = sorted({'%08x' % c[1] for c in EREF[a] if isinstance(c, tuple)})
                ins_ = sorted({'%08x' % c for c in EREF[a] if isinstance(c, int) and c != a})
                rows[fn] = [fn, 'helper', '-', '?', '-', ph,
                            'called by %s%s' % (' '.join(ins_[:4]), ('; also referenced from outside the class: ' + ' '.join(outs[:3])) if outs else '')]
                if nm:
                    rows[fn][6] += '; funcmap: ' + nm.split(' (')[0]
                changed = True
UNW = (b'\x8b\x4d', b'\x8d\x4d', b'\x8b\x45', b'\x8d\x45')
for a in cand:
    fn = '%08x' % a
    if fn in rows:
        continue
    er = EREF[a]
    if not er and jcc.get(a):
        ow = sorted({'%08x' % owner(x) for x in jcc[a] if owner(x)})
        rows[fn] = [fn, 'not-luciel', '-', '-', '-', 'n/a', 'cold-path chunk of %s ( reached by a conditional / short jump from %08x, no call ): part of that function, not a function' % (' '.join(ow[:2]), jcc[a][0])]
        continue
    outside = [c[1] for c in er if isinstance(c, tuple)]
    inside_c = [c for c in er if isinstance(c, int) and c != a]
    fi = d[a - B:a - B + 8]
    if not er:
        if fi[:2] in UNW:
            rows[fn] = [fn, 'not-luciel', '-', '-', '-', 'n/a', 'unwind funclet ( opens with an ebp-relative load, no reference )']
        else:
            rows[fn] = [fn, 'dead', '-', '-', '-', 'n/a', 'no call, jmp or pointer reference anywhere in the exe ( also none through a thunk ); first bytes %s' % fi.hex()]
    elif not inside_c and outside and not any(x >= 0x33400000 for x in outside):
        rows[fn] = [fn, 'not-luciel', '-', '-', '-', 'n/a', 'referenced only from outside the range: ' + ' '.join('%08x' % x for x in outside[:3]) + ' ( shared / folded code )']
    elif not inside_c and outside:
        rows[fn] = [fn, 'helper', '-', '?', '-', 'UNASSIGNED', 'referenced only from outside the class code ( data pointer ' + ' '.join('%08x' % x for x in outside[:3]) + ' ): phase unsettled']
    else:
        rows[fn] = [fn, 'helper', '-', '?', '-', 'UNASSIGNED', 'referrers %s have no phase yet' % ' '.join('%08x' % c for c in inside_c[:3])]
# helpers reached only from a shared ( not-luciel ) or dead function are shared / dead too
for _ in range(4):
    for fn, r in list(rows.items()):
        if r[1] == 'helper' and r[5] == 'UNASSIGNED':
            refs = [c for c in EREF.get(int(fn, 16), []) if isinstance(c, int) and c != int(fn, 16)]
            kinds = {rows['%08x' % c][1] for c in refs if '%08x' % c in rows}
            if refs and kinds and kinds <= {'not-luciel', 'dead'}:
                r[1] = 'not-luciel' if 'not-luciel' in kinds else 'dead'
                r[3], r[5] = '-', 'n/a'
                r[6] = 'called only from %s %s' % ('shared code' if r[1] == 'not-luciel' else 'a dead function', ' '.join('%08x' % c for c in refs[:3]))
# stubs
for a in S:
    fn = '%08x' % a
    k = kind_stub(a)
    if k and fn not in rows:
        t = stub_target(a)
        if k == 'vcall':
            rows[fn] = [fn, 'thunk', '-', '-', '-', 'UNASSIGNED', 'vcall stub: jmp [eax+0x%x] ( thunk>vslot 0x%x )' % (t[1], t[1])]
        elif k == 'maker':
            rows[fn] = [fn, 'thunk', '-', '-', '-', 'UNASSIGNED', 'delegate maker ( dest, this ): dest[0] = this, dest[1] = %08x ( thunk>%08x )' % (t, t)]
        elif k == 'pushcall':
            rows[fn] = [fn, 'thunk', '-', '-', '-', 'UNASSIGNED', 'callback shim: push edx; call %08x; ret ( thunk>%08x )' % (t, t)]
        elif k == 'adjust':
            rows[fn] = [fn, 'thunk', '-', '-', '-', 'UNASSIGNED', 'this-adjustor ( sub ecx, imm ): jmp %08x ( thunk>%08x )' % (t, t)]
        else:
            rows[fn] = [fn, 'thunk', '-', '-', '-', 'UNASSIGNED', 'jmp %08x ( thunk>%08x )' % (t, t)]
# thunk phase = the phase of its target ( resolved through chains of thunks )
vphase = {}
for r in rows.values():
    if r[1].startswith(('override', 'own-virtual')):
        for m in re.finditer(r'0x([0-9a-f]+)', r[1]):
            vphase[int(m.group(1), 16)] = r[5]
for _ in range(6):
    for fn, r in rows.items():
        if r[1] != 'thunk':
            continue
        m = re.search(r'thunk>([0-9a-f]{8})', r[6])
        if m:
            tg = m.group(1)
            if tg in rows and rows[tg][5] in ORDER and rows[tg][5] not in ('UNASSIGNED',):
                r[5] = rows[tg][5]
            elif int(tg, 16) < LO or int(tg, 16) >= HI:
                r[5] = 'n/a'
                if 'target outside' not in r[6]:
                    r[6] += '; target outside the Lu/Ciel range ( shared code )'
        m = re.search(r'thunk>vslot 0x([0-9a-f]+)', r[6])
        if m and int(m.group(1), 16) in vphase:
            r[5] = vphase[int(m.group(1), 16)]
# helper hints
for fn, r in rows.items():
    if r[1] == 'helper' and r[3] == '?':
        a = int(fn, 16)
        s_ = strs(a)
        r[6] += '; %d insns' % insn_count(a) + ('; strings: ' + ' | '.join('"%s"' % x for x in s_) if s_ else '')
# the shared ret stubs are not class members
for a in (0x32b4e770, 0x32b4e780):
    rows['%08x' % a] = ['%08x' % a, 'not-luciel', '-', '-', '-', 'n/a', 'shared `ret` stub ( callers in base code 31fca204 / 31fede50 ), after the class range']

# ---- split phases over SPLIT functions ----
skip = ('thunk', 'dead', 'not-luciel')


def tbl_order(fn):
    return int(fn, 16)


GROUP = {}
for ph in ORDER:
    fns = [fn for fn, r in rows.items() if r[5] == ph and r[1] not in skip]
    if len(fns) <= SPLIT + 5 or ph in ('n/a', 'UNASSIGNED'):
        continue
    if ph == 'CH1l':
        first = sorted([fn for fn in fns if rows[fn][1].startswith('override') and int(rows[fn][2].split(' ')[0], 16) in GETTERS], key=tbl_order)
        rest = sorted([fn for fn in fns if fn not in first], key=lambda f: (int(rows[f][2], 16) if rows[f][1].startswith('override') else 0xffff, tbl_order(f)))
        fns = first + rest
    else:
        fns = sorted(fns, key=tbl_order)
    parts = -(-len(fns) // SPLIT)
    size = -(-len(fns) // parts)
    for i, fn in enumerate(fns):
        GROUP[fn] = ph if i // size == 0 else '%s%d' % (ph, i // size + 1)
SPLITS = collections.OrderedDict()
for fn, g in GROUP.items():
    rows[fn][5] = g
    SPLITS.setdefault(re.sub(r'(?<=[a-z])\d+$', '', g), []).append(fn)
# thunks of a split phase follow their target
for _ in range(3):
    for fn, r in rows.items():
        if r[1] == 'thunk':
            m = re.search(r'thunk>([0-9a-f]{8})', r[6])
            if m and m.group(1) in rows and rows[m.group(1)][5] not in ('n/a', 'UNASSIGNED'):
                r[5] = rows[m.group(1)][5]

# ---- hand-decided names ( ch0_hand_names.tsv: fn, name, why ): name replaces proposed_name, '?' keeps it and adds the hint; the phase is never touched ----
HAND = {}
_hp = os.path.join(HERE, 'ch0_hand_names.tsv')
if os.path.exists(_hp):
    for _l in open(_hp, encoding='utf-8').read().splitlines():
        _p = _l.split('\t')
        if len(_p) >= 3 and _p[0] != 'fn':
            HAND[_p[0]] = (_p[1], _p[2])
for _fn, (_nm, _why) in HAND.items():
    if _fn not in rows:
        sys.exit('ch0_hand_names.tsv: %s is not a class-map row' % _fn)
    if _nm != '?':
        rows[_fn][3] = _nm
        rows[_fn][6] += '; name: ' + _why
    else:
        rows[_fn][6] += '; hint: ' + _why

with open(os.path.join(HERE, 'ch0_class_map.tsv'), 'w', encoding='utf-8', newline='\n') as o:
    o.write('june_fn\tkind\tvslot\tproposed_name\tmarch_pair\tphase\tnote\tcarved\tcallers\n')
    for fn in sorted(rows):
        a = int(fn, 16)
        r = rows[fn]
        cl = sorted({'%08x' % x for x, k in rel.get(a, [])})
        pt = ['%08x' % x for x in ptr.get(a, [])]
        cs = ('%d call/jmp: %s' % (len(cl), ' '.join(cl[:4]) + (' ...' if len(cl) > 4 else ''))) if cl else ''
        if pt:
            cs += ('; ' if cs else '') + 'ptr: ' + ' '.join(pt[:3])
        o.write('\t'.join(r[:7]) + '\t%s\t%s\n' % ('y' if a in carved else 'n', cs or '-'))
    kc = collections.Counter(r[1].split(' ')[0] for r in rows.values())
    nthunk = sum(1 for r in rows.values() if r[1] == 'thunk')

    def phkey(k):
        b = re.sub(r'(?<=[a-z])\d+$', '', k)
        return (ORDER.index(b) if b in ORDER else 99, k)
    o.write('# totals kind: %s; functions in range %d; carved %d; uncarved %d ( thunks %d ); override vslot boundary CH1k < 0x%x <= CH1l\n' % (
        ', '.join('%s=%d' % kv for kv in sorted(kc.items())), len(rows), len(carved), len(rows) - len(carved), nthunk, OVR_SPLIT))
    pc = collections.Counter(r[5] for r in rows.values() if r[1] not in skip)
    o.write('# totals per phase ( functions to transcribe ): %s\n' % ', '.join('%s=%d' % (k, pc[k]) for k in sorted(pc, key=phkey)))
    tc = collections.Counter(r[5] for r in rows.values() if r[1] == 'thunk')
    o.write('# totals per phase ( thunks, not transcribed ): %s\n' % ', '.join('%s=%d' % (k, tc[k]) for k in sorted(tc, key=phkey)))
    o.write('# helpers named ? : %d of %d helpers\n' % (sum(1 for r in rows.values() if r[1] == 'helper' and r[3] == '?'), sum(1 for r in rows.values() if r[1] == 'helper')))
    o.write('# state-fn named ? : %d\n' % sum(1 for r in rows.values() if r[1].startswith('state-fn') and r[3] == '?'))
print(open(os.path.join(HERE, 'ch0_class_map.tsv'), encoding='utf-8').read().split('\n# totals', 1)[1])
print('splits', {k: len(v) for k, v in SPLITS.items()})
