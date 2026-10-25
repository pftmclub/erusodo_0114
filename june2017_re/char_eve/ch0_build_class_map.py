"""char-eve CH0: build ch0_class_map.tsv, every June function in Eve's class range 0x32965dd0-0x329cf86f. Copy of
char_arme/ch0_build_class_map.py with Eve's range, registrars, vtable and phase table.

    python june2017_re/char_eve/ch0_build_class_map.py        (needs Ghidra's HTTP server for xrefs)

Inputs (all in char_eve/): ch0_vtable_slots.tsv (the 69 vtable overrides), ch0_state_skill.tsv (state callbacks, phases),
ch0_hand_phases.tsv (CH0's hand-set rows: fn, phase, why), june2017_re/funcs.txt (Ghidra's function list). Kinds as the pilot's:
override / registrar / state-fn / helper (phase = earliest caller) / thunk (not ported) / dead (no reference in the exe).
Overrides: CH1k (slots < 0x600), CH1l (0x600-0x7bc), CH1l2 (Common*, 0x7c0-0x7ec), CH1l3 (skill-use group, 0x7f0 up), CH1l4 (CommonFrameMove 0x7cc and the helpers only it reaches), CH4a-c for the passive / damage-exception / MP / cool-time slots.
"""
import re, os, struct, subprocess, collections

HERE = os.path.dirname(os.path.abspath(__file__))
RE = os.path.normpath(os.path.join(HERE, '..'))
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
B = 0x31aa0c00
LO, HI = 0x32965dd0, 0x329cf870
d = open(EXE, 'rb').read()
ORDER = ['CH1a', 'CH1b', 'CH1c', 'CH1d', 'CH1e', 'CH1f', 'CH1g', 'CH1h', 'CH1i', 'CH1j', 'CH1j2', 'CH1k', 'CH1l', 'CH1l2', 'CH1l3', 'CH1l4', 'CH1m', 'CH1n',
         'CH1o', 'CH1p', 'CH1q', 'CH1', 'CH2a', 'CH2a2', 'CH2b', 'CH2c', 'CH2c2', 'CH3a', 'CH3b', 'CH3c', 'CH3d', 'CH4a', 'CH4b', 'CH4c', 'CH5', 'n/a']
CH4_SLOTS = {'0x2b8': ('CH4b', 'SetDamageExceptionProcess: passive blocks keyed on RNSI_* (checklist 7b), June-only override'),
             '0x24c': ('CH4a', 'buff-templet specifics'), '0x250': ('CH4a', 'buff-templet specifics (undo)'),
             '0x7ac': ('CH4a', 'passive accel-buff time'), '0x6a0': ('CH4b', 'MP-consume rules'), '0x6a4': ('CH4b', 'cool-time rules'),
             '0x690': ('CH4a', 'InitPassiveSkillState'), '0x428': ('CH4b', 'skill-memo float hook (7an)'),
             '0x73c': ('CH4b', 'skill-memo float hook (7an)')}
REGISTRARS = {'329b8030': ('InitStateCommon', 'CX2GUEve::InitStateCommon (X2GuEve.cpp:673)', 'CH1m'),
              '329ba9c0': ('InitState (vslot 0x7a4: cut-in set index, class-gated state blocks, calls 329b8030 / 3202b110 / 31fe22d0)',
                           'X2GuEve.cpp:1605 InitState + InitStateID :655', 'CH1m'),
              '329c8a00': ('SetEquippedSkillFuncBySkillSlotIndex (vslot 0x7b4; head, memo / type switches; skill cases follow their skills)',
                           'X2GuEve.cpp:2153-3212', 'CH1n')}
REG_RANGES = {}
for a, b, n in ((0x329b8030, 0x329ba9bb, 'InitStateCommon 329b8030'), (0x329ba9c0, 0x329bc45e, 'InitState 329ba9c0'),
                (0x329c8a00, 0x329caab3, 'SetEquippedSkillFunc 329c8a00')):
    for x in range(a, b):
        REG_RANGES[x] = n


def xrefs(a):
    o = subprocess.run(['curl', '-s', '--noproxy', '127.0.0.1', '-m', '60', '-G', 'http://127.0.0.1:8080/xrefs_to',
                        '--data-urlencode', 'address=' + a], capture_output=True, text=True).stdout
    return [int(x, 16) for x in re.findall(r'From ([0-9a-f]{8})', o)]


funcs = sorted({int(m.group(1), 16) for m in re.finditer(r' at ([0-9a-f]{8})\s*$', open(os.path.join(RE, 'funcs.txt')).read(), re.M)
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


def is_thunk(va):
    o = va - B
    return d[o] == 0xe9 and d[o + 5] == 0xcc


_rel = None


def rel_index():
    """rel32 call / jmp targets of the whole text, built once (target -> [offset])."""
    global _rel
    if _rel is None:
        import array
        _rel = collections.defaultdict(list)
        L = 0x1cbbe00
        for o in range(0x1000, L - 5):
            if d[o] in (0xe8, 0xe9):
                t = (o + B + 5 + struct.unpack_from('<i', d, o + 1)[0]) & 0xffffffff
                if LO - 0x100000 <= t < HI + 0x100000:
                    _rel[t].append(o)
    return _rel


def whole_file_refs(t):
    """(referencing address, label) for imm32 pointers, rel32 call / jmp, and pointers to a jmp thunk of t."""
    L, out = 0x1cbbe00, []
    p = struct.pack('<I', t)
    i = d.find(p, 0, L)
    while i != -1:
        out.append((i + B, 'ptr@%08x' % (i + B)))
        i = d.find(p, i + 1, L)
    for o in rel_index().get(t, []):
        if d[o] == 0xe9 and d[o + 5] == 0xcc:
            q = struct.pack('<I', o + B)
            j = d.find(q, 0, L)
            while j != -1:
                out.append((j + B, 'thunk %08x @%08x' % (o + B, j + B)))
                j = d.find(q, j + 1, L)
        else:
            out.append((o + B, '%08x' % (o + B)))
    return out


rows = {}
vt = [l.split('\t') for l in open(os.path.join(HERE, 'ch0_vtable_slots.tsv'), encoding='utf-8').read().splitlines()
      if l and not l.startswith('#') and not l.startswith('slot_off')]
for f in vt:
    slot, fn, name, src, march = f[0], f[2], f[4], f[5], f[8] if len(f) > 8 else '-'
    ph, why = CH4_SLOTS.get(slot, (None, ''))
    if not ph:
        ph = REGISTRARS[fn][2] if fn in REGISTRARS else ('CH1k' if int(slot, 16) < 0x600 else ('CH1l' if int(slot, 16) < 0x7c0 else ('CH1l4' if slot == '0x7cc' else ('CH1l2' if int(slot, 16) < 0x7f0 else 'CH1l3'))))
    note = why + ('; name: ' + src.split(':')[0] if src else '')
    verdict = 'pair' if march not in ('-', '') and not march.startswith('- ') else ('june-only (base virtual exists in March)' if name != '?' else 'unnamed')
    rows[fn] = [fn, 'override ' + slot, name, march, verdict, ph, note.strip('; ')]
for fn, (name, march, ph) in REGISTRARS.items():
    if fn not in rows:
        rows[fn] = [fn, 'registrar', name, march, 'differs (June state set)', ph, '']
    else:
        rows[fn][1] += ' + registrar'
        rows[fn][5] = ph
st = open(os.path.join(HERE, 'ch0_state_skill.tsv'), encoding='utf-8').read().split('# part 2')[0].splitlines()
sfn = collections.defaultdict(lambda: {'uses': [], 'march': set(), 'ph': 'n/a'})
for l in st:
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
for fn, e in sfn.items():
    if fn in rows:
        rows[fn][1] += ' + state-fn'
        continue
    march = ' / '.join(sorted(e['march'])) or '-'
    uses = list(dict.fromkeys(e['uses']))
    rows[fn] = [fn, 'state-fn', ' '.join(uses[:4]) + (' +%d' % (len(uses) - 4) if len(uses) > 4 else ''), march,
                'pair' if e['march'] else 'june-only', e['ph'], '']
hand = {}
hp = os.path.join(HERE, 'ch0_hand_phases.tsv')
if os.path.exists(hp):
    for l in open(hp, encoding='utf-8').read().splitlines():
        if l and not l.startswith('#') and not l.startswith('fn\t'):
            f = l.split('\t')
            hand[f[0]] = (f[1], f[2] if len(f) > 2 else '')
for fn, (ph, why) in hand.items():
    if ph == 'dead':
        rows[fn] = [fn, 'dead', rows.get(fn, [fn, '', '-'])[2], '-', 'june-dead-code (no reference in the exe)', 'n/a', why]
pending = [f for f in funcs if '%08x' % f not in rows]
callers = {}
for f in pending:
    if is_thunk(f):
        continue
    callers[f] = [(x, owner(x) if LO <= x < HI else None) for x in xrefs('%08x' % f)]
changed = True
while changed:
    changed = False
    for f, cs in callers.items():
        fn = '%08x' % f
        if fn in hand:
            continue
        phs = [rows['%08x' % c][5] for _, c in cs if c is not None and '%08x' % c in rows and rows['%08x' % c][5] in ORDER]
        ph = min(phs, key=ORDER.index) if phs else None
        if ph and (fn not in rows or ORDER.index(ph) < ORDER.index(rows[fn][5])):
            outside = sorted({'%08x' % x for x, c in cs if c is None})
            rows[fn] = [fn, 'helper', 'called by ' + ' '.join(sorted({'%08x' % c for _, c in cs if c})[:4]), '-', '-', ph,
                        ('also called from outside the class: ' + ' '.join(outside[:4])) if outside else '']
            changed = True
nthunk = 0
for f in pending:
    fn = '%08x' % f
    if fn in rows or fn in hand:
        continue
    if is_thunk(f):
        nthunk += 1
        continue
    cs = callers.get(f, [])
    if not cs:
        refs = whole_file_refs(f)
        reg = [r for r in refs if r[0] in REG_RANGES]
        if reg:
            rows[fn] = [fn, 'state-fn', 'callback set in %s (branch the extractor skips: read it)' % REG_RANGES[reg[0][0]], '-',
                        'june-only?', 'CH1n', 'thunk %s' % reg[0][1]]
        elif refs:
            rows[fn] = [fn, 'helper', 'called from %s' % refs[0][1], '-', '-', 'CH1o', 'reached only through a pointer / thunk: read it']
        else:
            rows[fn] = [fn, 'dead', '-', '-', 'june-dead-code (no pointer, call, jmp or thunk anywhere in the exe)', 'n/a', '']
    else:
        rows[fn] = [fn, 'helper', 'called only from outside the class: ' + ' '.join('%08x' % x for x, _ in cs[:4]), '-', '-', 'CH1o',
                    'outside caller: read it (shared / folded code?)']
for fn, (ph, why) in hand.items():
    if ph == 'dead':
        continue
    if fn not in rows:
        rows[fn] = [fn, 'helper', '-', '-', '-', ph, why]
    else:
        rows[fn][5], rows[fn][6] = ph, why
with open(os.path.join(HERE, 'ch0_class_map.tsv'), 'w', encoding='utf-8', newline='\n') as o:
    o.write('june_fn\tkind\tname_or_states\tmarch_pair\tverdict\tphase\tnote\n')
    for fn in sorted(rows):
        o.write('\t'.join(rows[fn]) + '\n')
    c = collections.Counter((r[1].split(' ')[0], r[5]) for r in rows.values())
    o.write('# totals kind/phase: %s; thunks skipped: %d; functions in range: %d\n' % (
        ', '.join('%s/%s=%d' % (k[0], k[1], v) for k, v in sorted(c.items())), nthunk, len(funcs)))
    pc = collections.Counter(r[5] for r in rows.values())
    o.write('# totals per phase (functions to transcribe): %s\n' % ', '.join('%s=%d' % (k, pc[k]) for k in ORDER if pc[k]))
print(open(os.path.join(HERE, 'ch0_class_map.tsv'), encoding='utf-8').read().split('# totals', 1)[1])

stph = {}
for l in st:
    p = l.split('\t')
    if len(p) >= 8 and p[1] != 'june_script_line':
        stph[p[0]] = p[7].split(' ')[0]
cl = os.path.join(HERE, 'ch0_effect_closure.tsv')
if os.path.exists(cl) and os.environ.get("CH0_NO_CLOSURE") != "1":
    out = []
    for l in open(cl, encoding='utf-8').read().split('\n'):
        p = l.split('\t')
        if len(p) >= 6 and not l.startswith('#') and p[0] != 'state':
            s0 = p[0]
            if s0.startswith('fn:'):
                f = s0[3:].lower().replace('0x', '')
                p[5] = rows[f][5] if f in rows else 'CH1o'
            elif s0.startswith('INIT_'):
                p[5] = 'CH1'
            else:
                p[5] = stph.get(s0, 'n/a')
            l = '\t'.join(p)
        out.append(l)
    open(cl, 'w', encoding='utf-8', newline='\n').write('\n'.join(out))
