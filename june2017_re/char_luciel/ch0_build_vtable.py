r"""char-luciel CH0: build ch0_vtable_all.tsv (all 547 slots of CX2GULuciel 334f06fc) and ch0_vtable_slots.tsv (the 122 in-range slots).

    python june2017_re/char_luciel/ch0_build_vtable.py          (no Ghidra needed; reads the exe)

Lu/Ciel vtable 334f06fc (547 slots 0x0-0x888; slots 0x880 / 0x884 / 0x888 are Lu/Ciel-only virtuals, the dwords after it are RTTI string bytes);
base = CX2GUUser vtable 334781f4 (544 slots 0x0-0x87c). In-range = the entry lies in the Lu/Ciel class range 32ae4080-32b4e76f.
Member name of a slot, in this order: the sibling class maps (char_chung / char_eve ch0_vtable_slots.tsv: same virtual, same base slot), the
funcmap name of the base function (CX2GUUser:: / CX2GameUnit::), char_chung/ch1_offsets.tsv vslot names, else `?` with a hint (size, first
calls). March declaration = the X2GUUser.h / X2GameUnit.h virtual of that name (CX2GUUser is Lu/Ciel's March base too).
"""
import os, re, struct, collections, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ch0_lu_data as D

HERE, T, B = D.HERE, D.T, D.B
RE = os.path.join(T, 'june2017_re')
LO, HI = D.LO, D.HI
VT, BASE = 0x334f06fc, 0x334781f4
d = open(D.EXE, 'rb').read()


def vt(a, n):
    return [struct.unpack_from('<I', d, a - B + 4 * i)[0] for i in range(n)]


c, u = vt(VT, 547), vt(BASE, 544)

sib = {}   # slot -> (name, source)
for cls in ('char_chung', 'char_eve'):
    p = os.path.join(RE, cls, 'ch0_vtable_slots.tsv')
    for l in open(p, encoding='utf-8').read().splitlines():
        f = l.split('\t')
        if len(f) > 5 and f[0].startswith('0x'):
            nm = f[4]
            if nm and nm != '?' and 'part' not in nm and not nm.startswith('~'):
                sib.setdefault(int(f[0], 16), (nm, cls))
for cls in ('char_chung', 'char_eve', 'char_arme'):
    p = os.path.join(RE, cls, 'ch0_class_map.tsv')
    for l in open(p, encoding='utf-8').read().splitlines()[1:]:
        f = l.split('	')
        if len(f) > 3 and f[1].startswith('override'):
            slots = re.findall(r'0x([0-9a-f]+)', f[1])
            names = f[2].split(' / ')
            if len(slots) == len(names):
                for sl, nm in zip(slots, names):
                    if nm and nm != '?' and 'part' not in nm and not nm.startswith('~'):
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

hdr = {}
for fn in ('X2GUUser.h', 'X2GameUnit.h'):
    for i, l in enumerate(open(os.path.join(T, 'X2Lib', fn), 'rb').read().decode('latin-1').split('\n'), 1):
        m = re.search(r'\bvirtual\b[^;{(]*?([~\w]+)\s*\(', l)
        if m:
            hdr.setdefault(m.group(1), '%s:%d %s' % (fn, i, re.sub(r'\s+', ' ', l.strip().split('//')[0]).strip()))

NEW = {0x880: ('?', 'HINT: Lu/Ciel-only virtual, first of three (0x880 / 0x884 / 0x888); no base entry'),
       0x884: ('?', 'HINT: Lu/Ciel-only virtual, second of three; no base entry'),
       0x888: ('?', 'HINT: Lu/Ciel-only virtual, third of three; called through a vcall stub 32b4b570 ( mov eax,[ecx]; jmp [eax+0x888] )')}


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


def member(slot, base_fn):
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
    if key in hdr:
        return hdr[key]
    return '- (no virtual of that name in X2GUUser.h / X2GameUnit.h)' if name != '?' else '- (name unsettled)'


all_rows = ['slot_off\tluciel_target\tbase_target\tin_range\toverride\tmember_name\tmarch_member']
rows = ['slot_off\tvt_entry\tfn\tbase_fn\tname\tsrc\tmarch']
n_ov = n_in = n_unnamed = 0
for i in range(547):
    s = i * 4
    base_fn = u[i] if i < 544 else None
    inr = LO <= c[i] < HI
    ov = base_fn is None or c[i] != base_fn
    nm, src = NEW[s] if s in NEW else (('~CX2GULuciel (scalar deleting dtor)', 'CONF: sibling class maps slot 0x0; ctor 32b1bd90 / dtor 32b39380') if s == 0 else member(s, base_fn))
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
rows.append('# Lu/Ciel vtable CX2GULuciel %08x: 547 slots (0x0-0x888); in-range entries (target in %08x-%08x): %d (overrides %d, unnamed %d)' % (VT, LO, HI - 1, n_in, n_ov, n_unnamed))
rows.append('# base_fn = CX2GUUser vtable %08x (544 slots); slots 0x880-0x888 have no base entry' % BASE)
open(os.path.join(HERE, 'ch0_vtable_all.tsv'), 'w', newline='\n', encoding='utf-8').write('\n'.join(all_rows) + '\n')
open(os.path.join(HERE, 'ch0_vtable_slots.tsv'), 'w', newline='\n', encoding='utf-8').write('\n'.join(rows) + '\n')
print(n_in, 'in range;', n_ov, 'overrides;', n_unnamed, 'unnamed; named by', collections.Counter(r.split('\t')[5][:22] for r in rows[1:-2]).most_common(6))
