"""pilot-char-revamp CH0: build ch0_class_map.tsv, every June function in Aisha's class range 0x32cb6e00-0x32cf2fff.

    python june2017_re/char_arme/ch0_build_class_map.py <scratch dir>     (needs Ghidra's HTTP server for xrefs)

Inputs: <S>/ch0_vtable_slots.tsv (the 63 vtable overrides, CH0 subagent), ch0_state_skill.tsv (state callbacks, phases),
june2017_re/funcs.txt (Ghidra's function list). Kinds:
  override   a vtable 0x3351d0b4 slot that differs from CX2GUUser's; March pair = the .h virtual
  registrar  InitState / InitStateCommon / SetEquippedSkillFuncBySkillSlotIndex / the InitState tail
  state-fn   a state callback (thunk target); March pair = March's callback in the same slot of the same table
  helper     anything else with a caller; phase = the earliest phase among its callers
  thunk      a 5-byte jmp (32cf0xxx-32cf2xxx block): not ported, the callback points at the target
  dead       no pointer, call or thunk anywhere in the exe (checked by a whole-file scan for the four found by hand,
             by Ghidra xrefs for the rest)
Phase order: CH1a < ... < CH1g < CH1h < CH1i < CH1 < CH2a < CH2b < CH3a < CH3b < CH4 < CH5. Overrides are CH1h (slots < 0x600) and CH1i
(slots >= 0x600), transcribed as inert June_* members and wired in CH1; helpers follow their earliest caller, except the passive / buff / cool-time / memorize ones (CH4).
"""
import re, sys, os, struct, subprocess, collections

S = sys.argv[1]
HERE = os.path.dirname(os.path.abspath(__file__))
RE = os.path.normpath(os.path.join(HERE, '..'))
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
B = 0x31aa0c00
LO, HI = 0x32cb6e00, 0x32cf3000
d = open(EXE, 'rb').read()
ORDER = ['CH1a', 'CH1b', 'CH1c', 'CH1d', 'CH1e', 'CH1f', 'CH1g', 'CH1h', 'CH1i', 'CH1', 'CH2a', 'CH2b', 'CH3a', 'CH3b', 'CH4', 'CH5', 'n/a']
CH4_SLOTS = {'0x2b8': 'passive blocks keyed on RNSI_* (checklist 7b, D8 Fitness -> Magical Staff)',
             '0x24c': 'buff-templet specifics (ABM / ADW Magical Makeup transform)', '0x250': 'buff-templet specifics (undo)',
             '0x7ac': 'passive accel-buff time', '0x6a0': 'MP-consume rules', '0x6a4': 'cool-time rules',
             '0x690': 'InitPassiveSkillState', '0x428': 'skill-memo float hook', '0x73c': 'skill-memo float hook'}
REGISTRARS = {'32ce77b0': ('InitStateCommon', 'CX2GUArme_VioletMage::InitStateCommon (X2GUArme_VioletMage.cpp:1637)'),
              '32ce1ba0': ('InitState tail (AVP Angor hammer mesh load)', '-'),
              '32cede00': ('SetEquippedSkillFuncBySkillSlotIndex (head, type switch; skill cases follow their skills)',
                           'X2GUArme_VioletMage.cpp:2842-4088'),
              '32cea0c0': ('InitState (cut-in set index, job state blocks, calls 32ce77b0 / 3202b110 / 31fe22d0 / 32ce1ba0)',
                           'X2GUArme_VioletMage.cpp:959 InitState + InitStateID :1623 + InitStateCommonRandom')}
DEAD_BY_HAND = {'32cd5d60': 'June BINDING_CIRCLE_HYPER FrameMove leftover (June dropped the _HYPER table); checklist 7p / 7ai / 7c sites in it never run',
                '32cc7300': 'spec-A socket reader (checklist 7 l.264) never called', '32ce0010': 'alpha-tag site 32ce0615 (7c) never runs',
                '32cce0f0': 'alpha-tag site 32cce1c2 (7c) never runs'}


def xrefs(a):
    o = subprocess.run(['curl', '-s', '--noproxy', '127.0.0.1', '-m', '60', '-G', 'http://127.0.0.1:8080/xrefs_to',
                        '--data-urlencode', 'address=' + a], capture_output=True, text=True).stdout
    return [int(x, 16) for x in re.findall(r'From ([0-9a-f]{8})', o)]


funcs = sorted({int(m.group(1), 16) for m in re.finditer(r' at ([0-9a-f]{8})\s*$', open(os.path.join(RE, 'funcs.txt')).read(), re.M)
                if LO <= int(m.group(1), 16) < HI})


def owner(va):
    i = 0
    lo, hi = 0, len(funcs) - 1
    while lo <= hi:
        m = (lo + hi) // 2
        if funcs[m] <= va:
            i = m
            lo = m + 1
        else:
            hi = m - 1
    return funcs[i] if funcs and funcs[i] <= va else None


def is_thunk(va):
    o = va - B
    return d[o] == 0xe9 and d[o + 5] == 0xcc


REG_RANGES = {}
for a, b, n in ((0x32ce77b0, 0x32ce9ec5, 'InitStateCommon 32ce77b0'), (0x32cea0c0, 0x32ceba8a, 'InitState 32cea0c0'),
                (0x32cede00, 0x32cf0739, 'SetEquippedSkillFunc 32cede00')):
    for x in range(a, b):
        REG_RANGES[x] = n


def whole_file_refs(t):
    """(referencing address, label) for imm32 pointers, rel32 call / jmp, and pointers to a jmp thunk of t."""
    L, out = 0x1cbbe00, []
    p = struct.pack('<I', t)
    i = d.find(p, 0, L)
    while i != -1:
        out.append((i + B, 'ptr@%08x' % (i + B)))
        i = d.find(p, i + 1, L)
    for o in range(0x1000, L - 5):
        if d[o] in (0xe8, 0xe9) and (o + B + 5 + struct.unpack('<i', d[o + 1:o + 5])[0]) & 0xffffffff == t:
            if d[o] == 0xe9 and d[o + 5] == 0xcc:   # a thunk: who points at it
                q = struct.pack('<I', o + B)
                j = d.find(q, 0, L)
                while j != -1:
                    out.append((j + B, 'thunk %08x @%08x' % (o + B, j + B)))
                    j = d.find(q, j + 1, L)
            else:
                out.append((o + B, '%08x' % (o + B)))
    return out


rows = {}
# overrides
vt = [l.split('\t') for l in open(os.path.join(S, 'ch0_vtable_slots.tsv'), encoding='utf-8').read().splitlines()
      if l and not l.startswith('#') and not l.startswith('slot_off')]
for f in vt:
    slot, fn, name, src, march = f[0], f[2], f[4], f[5], f[8] if len(f) > 8 else '-'
    # CH1h: lifecycle / render / reaction overrides (slots below 0x600); CH1i: skill-flow, riding, equip-view and
    # Common* overrides (0x600 and up). Both are transcribed as inert June_* members and wired by CH1.
    ph = 'CH4' if slot in CH4_SLOTS else ('CH1' if fn in REGISTRARS else ('CH1h' if int(slot, 16) < 0x600 else 'CH1i'))
    note = CH4_SLOTS.get(slot, '') + ('; name: ' + src.split(':')[0] if src else '')
    rows[fn] = [fn, 'override ' + slot, name, march, 'pair' if march not in ('-', '') else ('june-only (base virtual exists in March)' if name != '?' else 'unnamed'), ph, note.strip('; ')]
for fn, (name, march) in REGISTRARS.items():
    if fn not in rows:
        rows[fn] = [fn, 'registrar', name, march, 'differs (June state set)', 'CH1', '']
    else:
        rows[fn][1] += ' + registrar'
        rows[fn][5] = 'CH1'
# state callbacks
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
    rows[fn] = [fn, 'state-fn', ' '.join(e['uses'][:4]) + (' +%d' % (len(e['uses']) - 4) if len(e['uses']) > 4 else ''), march,
                'pair' if e['march'] else 'june-only', e['ph'], '']
# dead by hand
for fn, why in DEAD_BY_HAND.items():
    rows[fn] = [fn, 'dead', rows.get(fn, [fn, '', ''])[2] or '-', '-', 'june-dead-code (no reference in the exe)', 'n/a', why]
# everything else: thunk / helper / dead, by callers
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
        phs = [rows['%08x' % c][5] for _, c in cs if c is not None and '%08x' % c in rows and rows['%08x' % c][5] in ORDER]
        ph = min(phs, key=ORDER.index) if phs else None
        if ph and (fn not in rows or ORDER.index(ph) < ORDER.index(rows[fn][5])):
            outside = sorted({'%08x' % x for x, c in cs if c is None})
            rows[fn] = [fn, 'helper', 'called by ' + ' '.join(sorted({'%08x' % c for _, c in cs if c})[:4]), '-', '-', ph,
                        ('also called from outside the class: ' + ' '.join(outside[:4])) if outside else '']
            changed = True
# hand-set (CH0): class pieces the engine calls for a later phase's feature
for f, (ph, why) in {'32ce1770': ('CH4', 'decide-page ACK class piece (checklist 7ak), callers 3220f4dc / 3220e995 / 328df9db'),
                     '32cc98e0': ('CH1h', 'summon hook called by CX2Game::SetUserSummonedNPCInfo 3230c1a0 @3230c59a (checklist 7 l.266)')}.items():
    if f in rows:
        rows[f][5], rows[f][6] = ph, why
nthunk = 0
for f in pending:
    fn = '%08x' % f
    if fn in rows:
        continue
    if is_thunk(f):
        nthunk += 1
        continue
    cs = callers.get(f, [])
    if not cs:
        refs = whole_file_refs(f)
        reg = [r for r in refs if r[0] in REG_RANGES]
        if reg:
            rows[fn] = [fn, 'state-fn', 'callback set in %s (job branch the extractor skips: read the branch)' % REG_RANGES[reg[0][0]], '-',
                        'june-only?', 'CH1h', 'thunk %s' % reg[0][1]]
        elif refs:
            src = refs[0][1]
            rows[fn] = [fn, 'helper', 'called from %s' % src, '-', '-', 'CH4' if src.startswith('328') else 'CH1h',
                        'outside caller (skill-slot UI, memorize: checklist 7aj)' if src.startswith('328') else '']
        else:
            rows[fn] = [fn, 'dead', '-', '-', 'june-dead-code (no pointer, call, jmp or thunk anywhere in the exe)', 'n/a', '']
    else:
        rows[fn] = [fn, 'helper', 'called only from outside the class (or from an unmapped caller): ' + ' '.join('%08x' % x for x, _ in cs[:4]), '-', '-', 'CH1h', '']
with open(os.path.join(HERE, 'ch0_class_map.tsv'), 'w', encoding='utf-8', newline='\n') as o:
    o.write('june_fn\tkind\tname_or_states\tmarch_pair\tverdict\tphase\tnote\n')
    for fn in sorted(rows):
        o.write('\t'.join(rows[fn]) + '\n')
    o.write('# march-only virtuals (no June override): ParseCommonRandomState (.h:472, June rolls in CX2GUUser 31fe22d0), '
            'ApplyAuraAcceleratorBuffToGameUnit (.h:424): June wins, CH1 drops them; CreateNotEnoughMPEffect (.h:512) is not compiled\n')
    c = collections.Counter((r[1].split(' ')[0], r[5]) for r in rows.values())
    o.write('# totals kind/phase: %s; thunks skipped: %d; functions in range: %d\n' % (
        ', '.join('%s/%s=%d' % (k[0], k[1], v) for k, v in sorted(c.items())), nthunk, len(funcs)))
    pc = collections.Counter(r[5] for r in rows.values())
    o.write('# totals per phase (functions to transcribe): %s\n' % ', '.join('%s=%d' % (k, pc[k]) for k in ORDER if pc[k]))
print(open(os.path.join(HERE, 'ch0_class_map.tsv'), encoding='utf-8').read().split('# march-only')[1])

# fill ch0_effect_closure.tsv's phase column (charport_closure.py leaves '-'): script state -> its phase in
# ch0_state_skill.tsv, fn:<va> -> the class map's phase for that function, INIT_* -> CH1 (the script's own load)
stph = {}
for l in st:
    p = l.split('\t')
    if len(p) >= 8 and p[1] != 'june_script_line':
        stph[p[0]] = p[7].split(' ')[0]
cl = os.path.join(HERE, 'ch0_effect_closure.tsv')
out = []
for l in open(cl, encoding='utf-8').read().split('\n'):
    p = l.split('\t')
    if len(p) >= 6 and not l.startswith('#') and p[0] != 'state':
        s0 = p[0]
        if s0.startswith('fn:'):
            f = s0[3:].lower().replace('0x', '')
            p[5] = rows[f][5] if f in rows else 'CH1h'
        elif s0.startswith('INIT_'):
            p[5] = 'CH1'
        else:
            p[5] = stph.get(s0, 'n/a')
        l = '\t'.join(p)
    out.append(l)
open(cl, 'w', encoding='utf-8', newline='\n').write('\n'.join(out))
