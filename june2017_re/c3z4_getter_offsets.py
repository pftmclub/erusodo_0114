r"""Thin June getters bound to Lua: name -> the this-offset its fn reads (June effect C3z-4).
For each binder site (c3z4_binder_sites.sites) disassemble the bound fn's first instructions; a fn that reads [ecx+off]
and returns within ~8 instructions is a getter of that member. Used to name June member offsets (e.g. CX2GUNPC +0x270).
    python june2017_re/c3z4_getter_offsets.py [OFFSET_HEX ...]
"""
import sys, re, os, capstone
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import disasm_range as D, c3z4_binder_sites as B
text = D.load_text(); md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
want = {int(x, 16) for x in sys.argv[1:]}
for name, use, fn, call in B.sites():
    if not fn: continue
    off = D.va_to_raw(fn) - D.TR
    ins = list(md.disasm(text[off:off + 64], fn))[:10]
    s = '; '.join('%s %s' % (i.mnemonic, i.op_str) for i in ins[:8])
    offs = [int(m, 16) for m in re.findall(r'\[ecx \+ 0x([0-9a-f]+)\]', s)]
    if any(i.mnemonic == 'ret' for i in ins[:8]) and offs and (not want or set(offs) & want):
        print('%-36s use %s fn %08x  %s' % (name, use, fn, s))
