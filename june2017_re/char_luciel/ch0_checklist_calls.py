r"""char-luciel CH0 item 4 helper: which Lu/Ciel-range functions (0x32ae4080-0x32b4e76f) call or jump to each engine address.
    python june2017_re/char_luciel/ch0_checklist_calls.py 322a7660 322a8a70 ...
Raw E8/E9 rel32 scan of .text; the owner is the preceding int3-padded function start (a function that follows a ret with no padding is merged)."""
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
from disasm_range import load_text, BASE, TV
T = load_text()
LO, HI = 0x32ae4080, 0x32b4e76f
tg = {int(a, 16): a for a in sys.argv[1:]}
starts = []
o = LO - BASE - TV
end = HI - BASE - TV
prev_pad = True
i = o
fs = []
while i < end:
    if T[i] == 0xcc:
        prev_pad = True
    elif prev_pad:
        fs.append(i + BASE + TV); prev_pad = False
    i += 1
import bisect
for i in range(o, end - 5):
    if T[i] in (0xe8, 0xe9):
        rel = int.from_bytes(T[i + 1:i + 5], 'little', signed=True)
        va = i + BASE + TV
        t = (va + 5 + rel) & 0xffffffff
        if t in tg:
            f = fs[bisect.bisect_right(fs, va) - 1]
            print(tg[t], 'call' if T[i] == 0xe8 else 'jmp', '%x' % va, 'in %x' % f)
