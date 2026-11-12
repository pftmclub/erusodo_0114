r"""char-elesis CH0: build ch0_vtable_all.tsv (all 544 slots of CX2GUEL 334fa61c) and ch0_vtable_slots.tsv (the in-range overrides).
Copy of char_ara/ch0_build_vtable.py with Elesis's constants (char_ara is one of the sibling maps).

    python june2017_re/char_elesis/ch0_build_vtable.py          (no Ghidra needed; reads the exe)

Elesis vtable 334fa61c (544 slots 0x0-0x87c); base = CX2GUUser vtable 334781f4 (544 slots).
In-range = the entry lies in the Elesis class range 32b4e770-32b90b6f (incl. the uncarved entries).
Member name of a slot, in this order: the sibling class maps (char_raven / chung / eve / luciel ch0_vtable_slots.tsv and the raven / arme / eve / chung / luciel
class maps: same virtual, same base slot), the funcmap name of the base function (CX2GUUser:: / CX2GameUnit::), char_chung/ch1_offsets.tsv vslot names,
else `?` with a hint (size, first calls). March declaration = X2GUEl.h when it declares the virtual, else the X2GUUser.h / X2GameUnit.h virtual.
"""
import os, re, struct, collections, sys

HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..'))
B = 0x31aa0c00
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
RE = os.path.join(T, 'june2017_re')
LO, HI = 0x32b4e770, 0x32b90b70      # inclusive start, exclusive end
VT, BASE = 0x334fa61c, 0x334781f4
d = open(EXE, 'rb').read()


def vt(a, n):
    return [struct.unpack_from('<I', d, a - B + 4 * i)[0] for i in range(n)]


c, u = vt(VT, 544), vt(BASE, 544)

sib = {}   # slot -> (name, source)
for cls in ('char_ara', 'char_raven', 'char_chung', 'char_eve', 'char_luciel', 'char_elsword', 'char_rena'):
    p = os.path.join(RE, cls, 'ch0_vtable_slots.tsv')
    for l in open(p, encoding='utf-8').read().splitlines():
        f = l.split('\t')
        if len(f) > 5 and f[0].startswith('0x'):
            nm = f[4]
            if nm and nm != '?' and 'part' not in nm and not nm.startswith('~') and not re.match(r'^0x[0-9a-f]+$', nm):
                sib.setdefault(int(f[0], 16), (nm, cls))
for cls in ('char_ara', 'char_raven', 'char_chung', 'char_eve', 'char_arme', 'char_luciel', 'char_elsword', 'char_rena'):
    p = os.path.join(RE, cls, 'ch0_class_map.tsv')
    for l in open(p, encoding='utf-8').read().splitlines()[1:]:
        f = l.split('	')
        if len(f) > 3 and f[1].startswith('override'):
            slots = re.findall(r'0x([0-9a-f]+)', f[1])
            names = f[2].split(' / ')
            if len(slots) == len(names):
                for sl, nm in zip(slots, names):
                    if nm and nm != '?' and 'part' not in nm and not nm.startswith('~') and not re.match(r'^0x[0-9a-f]+$', nm):
                        sib.setdefault(int(sl, 16), (nm.strip(), cls))
off_names = {}
for l in open(os.path.join(RE, 'char_chung', 'ch1_offsets.tsv'), encoding='utf-8').read().splitlines():
    f = l.split('\t')
    if len(f) > 2 and f[1] == 'vslot':
        off_names[int(f[0], 16)] = re.sub(r'^June', '', re.sub(r'\(.*', '', f[2]).strip())
fm = {}
for l in open(os.path.join(RE, 'funcmap.tsv'), encoding='utf-8', errors='replace'):
    p = l.split('\t')
    m = re.match(r'^([0-9a-f]{8})$', p[0])
    if m and len(p) > 1 and re.match(r'^CX2(GUUser|GameUnit)::', p[1]):
        fm.setdefault(m.group(1), re.sub(r'\s*\(.*', '', p[1]))

hdr, own = {}, {}
for fn in ('X2GUUser.h', 'X2GameUnit.h', 'X2GUEl.h'):
    for i, l in enumerate(open(os.path.join(T, 'X2Lib', fn), 'rb').read().decode('latin-1').split(chr(10)), 1):
        m = re.search(r'\bvirtual\b[^;{(]*?([~\w]+)\s*\(', l)
        if m:
            (own if fn == 'X2GUEl.h' else hdr).setdefault(m.group(1), '%s:%d %s' % (fn, i, re.sub(r'\s+', ' ', l.strip().split('//')[0]).strip()))
for i, l in enumerate(open(os.path.join(T, 'X2Lib', 'X2GUEl.h'), 'rb').read().decode('latin-1').split(chr(10)), 1):
    m = re.match(r'\s*void (HyperModeFrameMove|COMMON_BUFF_FrameMove)\s*\(', l)
    if m:
        own.setdefault(m.group(1), 'X2GUEl.h:%d %s (no virtual keyword; overrides the CX2GUUser pure virtual)' % (i, l.strip()))


NEW = {}


def hint(va):
    """size and first calls of the function at va (capstone, to the first ret at depth 0)"""
    import capstone
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    o = va - B
    calls, n = [], 0
    for i in md.disasm(d[o:o + 0x400], va):
        n += 1
        if i.mnemonic == 'call' and re.match(r'^0x[0-9a-f]+$', i.op_str) and len(calls) < 3:
            calls.append(i.op_str[2:])
        if i.mnemonic == 'ret':
            break
    return '%d insns; calls %s' % (n, ' '.join(calls) or '-')


PARTS = {0x7f8: 'SpecialAttackEventProcess part: skill lookup (slot 510)', 0x7fc: 'SpecialAttackEventProcess part: skill use (slot 511)',
         0x800: 'SpecialAttackEventProcess part: per-class skill use / cooldown (slot 512)'}   # Chung's NAMES for the same slots (char_chung/ch0_build_vtable.py)


def member(slot, base_fn):
    if slot in PARTS:
        return PARTS[slot], 'CONF: char_chung vtable names, same slot (funcmap 32bd8790 note: slot 510 / 511 / 512 parts of SpecialAttackEventProcess)'
    if slot in sib:
        return sib[slot][0], 'CONF: %s class map %#x' % (sib[slot][1], slot)
    nm = fm.get('%08x' % base_fn) if base_fn else None
    if nm:
        return nm.split('::', 1)[1], 'CONF: funcmap %08x %s' % (base_fn, nm)
    if slot in off_names:
        return off_names[slot], 'CONF: char_chung/ch1_offsets.tsv vslot %#x' % slot
    return '?', ''


def march(name):
    key = name.split(' (')[0]
    if key in own:
        return own[key]
    if key in hdr:
        return '- (CX2GUEL does not declare it in March; %s)' % hdr[key]
    return '- (no virtual of that name in X2GUEl.h / X2GUUser.h / X2GameUnit.h)' if name != '?' else '- (name unsettled)'


all_rows = ['slot_off\telesis_target\tbase_target\tin_range\toverride\tmember_name\tmarch_member']
rows = ['slot_off\tvt_entry\tfn\tbase_fn\tname\tsrc\tmarch']
n_ov = n_in = n_unnamed = 0
for i in range(544):
    s = i * 4
    base_fn = u[i]
    inr = LO <= c[i] < HI
    ov = base_fn is None or c[i] != base_fn
    nm, src = NEW[s] if s in NEW else (('~CX2GUEL (scalar deleting dtor)', 'CONF: sibling class maps slot 0x0; slot-0 dtor 32b90520') if s == 0 else member(s, base_fn))
    if nm == '?' and s not in NEW:
        src = 'HINT: ' + (('base %08x; ' % base_fn if base_fn else '') + hint(c[i]))
    all_rows.append('0x%x\t%08x\t%s\t%d\t%s\t%s\t%s' % (s, c[i], '%08x' % base_fn if base_fn else '-', 1 if inr else 0, 'Y' if ov else '', nm, march(nm)))
    if inr:
        n_in += 1
        if ov:
            n_ov += 1
        if nm == '?':
            n_unnamed += 1
            if s in NEW:
                src = src + '; ' + hint(c[i])
        rows.append('0x%x\t%08x\t%08x\t%s\t%s\t%s\t%s' % (s, VT + s, c[i], '%08x' % base_fn if base_fn else '-', nm, src, march(nm)))
rows.append('# Elesis vtable CX2GUEL %08x: 544 slots (0x0-0x87c); in-range entries (target in %08x-%08x): %d (overrides %d, unnamed %d)' % (VT, LO, HI - 1, n_in, n_ov, n_unnamed))
rows.append('# base_fn = CX2GUUser vtable %08x (544 slots)' % BASE)
open(os.path.join(HERE, 'ch0_vtable_all.tsv'), 'w', newline='\n', encoding='utf-8').write('\n'.join(all_rows) + '\n')
open(os.path.join(HERE, 'ch0_vtable_slots.tsv'), 'w', newline='\n', encoding='utf-8').write('\n'.join(rows) + '\n')
print(n_in, 'in range;', n_ov, 'overrides;', n_unnamed, 'unnamed; named by', collections.Counter(r.split('\t')[5][:22] for r in rows[1:-2]).most_common(6))
