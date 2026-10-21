"""Compare two June functions instruction by instruction (branch targets made relative): IDENTICAL, or a diff.

    python june2017_re/funccmp.py <va_a> <va_b>

Both start at the given entry and end at the next int3 pair. Use it to prove a June body equals another June body (for example the
32cbe6a0 EventProcess clones) before registering it as one member. (Character port CH3a.)
"""
import sys, re, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import funcann as ann
def norm(va):
    s = va; e = ann.fend(va)
    out = []
    for i in ann.md.disasm(ann.T[ann.off(s):ann.off(e)+16], s):
        if i.address >= e: break
        op = i.op_str
        op = re.sub(r'0x([0-9a-f]{8})\b', lambda m: ("L%d" % (int(m.group(1),16)-s)) if s <= int(m.group(1),16) < e else m.group(0), op)
        out.append(f"{i.mnemonic} {op}")
    return out
a = norm(int(sys.argv[1], 16)); b = norm(int(sys.argv[2], 16))
if a == b: print("IDENTICAL", len(a))
else:
    import difflib
    print("DIFF", len(a), len(b))
    for l in difflib.unified_diff(a, b, lineterm="", n=0): print(l)
