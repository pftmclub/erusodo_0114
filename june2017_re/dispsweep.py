"""Find every instruction in June's .text whose displacement is one of the given offsets.

    python dispsweep.py 6890 6891 6892     # hex member offsets

Prints `addr f=<function start> instruction`. Catches EBP- and ESP-based operands too, so
filter `esp +` hits (locals) by hand. A few hits decode at the wrong start (a `mov esi, 0x6890`
is really a cmp/mov on [reg+0x6890]); read those with funcdis.py. Protected members are read
as `lea reg, [obj+off]` plus XORs, so a field read and a field write look alike: tell them
apart by what follows. (Buff port Phase 7b: the detonation bytes and bInvincible sweeps.)
"""
import os, sys, re, capstone
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from disasm_range import load_text, BASE, TV
T = load_text(); md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
def fstart(o):
    while o > 0:
        if T[o-1] == 0xcc and T[o-2] == 0xcc: return o + BASE + TV
        o -= 1
for disp in [int(x,16) for x in sys.argv[1:]]:
    pat = disp.to_bytes(4,'little')
    i = T.find(pat)
    while i >= 0:
        for back in range(1,8):
            s = i-back
            ins = list(md.disasm(T[s:s+16], s+BASE+TV, 1))
            if ins and ins[0].size >= back+4 and hex(disp) in ins[0].op_str:
                print(f"{ins[0].address:08x} f={fstart(s):08x} {ins[0].mnemonic} {ins[0].op_str}")
                break
        i = T.find(pat, i+1)
