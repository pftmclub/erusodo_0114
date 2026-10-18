"""Skill S3c type sweep: every call to GetSkillTemplet 32206450 / type-by-id 32205ab0 in June's .text followed by a type
compare or a jump table, one line per function. Run from june2017_re/: python s3c_typesweep.py . > out.txt
The function start is the nearest CC CC pad above the call, so a function with no pad is reported under its predecessor
(3220a350 for 3220a940, 32210c70 for 322112d0, ...): s3c_skill_type_readers.tsv carries the corrected start."""
import os, sys, re, struct, capstone
sys.path.insert(0, sys.argv[1])
from disasm_range import load_text, BASE, TV
T = load_text()
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = False
TARGETS = {0x32206450: 'GetSkillTemplet', 0x32205ab0: 'TypeById'}
def fstart(o):
    while o > 1:
        if T[o-1] == 0xcc and T[o-2] == 0xcc: return o
        o -= 1
    return 0
hits = []
for o in range(len(T) - 5):
    if T[o] != 0xe8: continue
    va = o + BASE + TV
    tgt = (va + 5 + struct.unpack_from('<i', T, o+1)[0]) & 0xffffffff
    if tgt in TARGETS: hits.append((va, tgt))
out = {}
for va, tgt in hits:
    o = va - BASE - TV
    ins = list(md.disasm(T[o:o+400], va))[:45]
    if not ins or ins[0].mnemonic != 'call': continue
    typeregs = set(); imms = []; tbl = False
    if tgt == 0x32205ab0: typeregs.add('eax')
    for i in ins[1:]:
        s = i.op_str
        m = re.match(r'(e\w\w), dword ptr \[eax \+ 4\]$', s)
        if i.mnemonic == 'mov' and m and tgt == 0x32206450: typeregs.add(m.group(1)); continue
        m = re.match(r'dword ptr \[eax \+ 4\], (0x[0-9a-f]+|\d+)$', s)
        if i.mnemonic == 'cmp' and m and tgt == 0x32206450: imms.append(int(m.group(1), 0)); continue
        m = re.match(r'(e\w\w), (0x[0-9a-f]+|\d+)$', s)
        if m and m.group(1) in typeregs and i.mnemonic in ('cmp', 'sub', 'add'):
            imms.append((i.mnemonic, int(m.group(1+1), 0)) if i.mnemonic != 'cmp' else int(m.group(2), 0))
        if i.mnemonic == 'jmp' and 'ptr [' in s and '*4' in s.replace(' ', ''): tbl = True
        if i.mnemonic == 'call' and imms: break
    fs = fstart(o) + BASE + TV
    out.setdefault(fs, []).append((va, TARGETS[tgt], imms, tbl))
for fs in sorted(out):
    rows = out[fs]
    flat = [x for r in rows for x in r[2]]
    nums = [x if isinstance(x, int) else x[1] for x in flat]
    interesting = any(isinstance(x, tuple) for x in flat) or any(r[3] for r in rows) or any(0xc <= n <= 0x12 for n in nums) or any(n in (5,6,7) for n in nums)
    if interesting:
        print(f"{fs:08x}", '; '.join(f"{va:08x} {t} {imms}{' TABLE' if tb else ''}" for va, t, imms, tb in rows if imms or tb))
