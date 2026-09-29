"""Raw capstone disassembly of an arbitrary VA range in unpacked_x2.exe.

Ghidra's disassemble_function/decompile_function only work on addresses Ghidra has
already carved into a function (get_function_by_address). CEffect::OnFrameMove (31f88790)
has no Ghidra function at all (Phase 7a), so anything inside it -- the touch-land gate,
the die gates -- has to be read this way instead.

Usage: python disasm_range.py <start_va_hex> <end_va_hex> [--x87]

--x87 also prints a best-effort symbolic x87 stack trace (see x87_emulate below): useful
for settling a fcompp/fucomip's operands without hand-tracing pushes across branches.
"""
import capstone, sys

EXE = r"F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe"
BASE = 0x31aa0000
TR = 0x400      # .text raw file offset
TS = 0x1cbba00  # .text raw size
TV = 0x1000     # .text RVA

def va_to_raw(va):
    return TR + (va - BASE - TV)

def load_text():
    with open(EXE, "rb") as f:
        data = f.read()
    return data[TR:TR + TS]

def disasm_range(va_start, va_end):
    text = load_text()
    off_start = va_to_raw(va_start) - TR
    off_end = va_to_raw(va_end) - TR
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = False
    insns = list(md.disasm(text[off_start:off_end + 16], va_start))
    out = []
    for insn in insns:
        if insn.address >= va_end:
            break
        out.append((insn.address, f"{insn.mnemonic} {insn.op_str}".strip()))
    return out

X87_MNEM = {
    "fld", "fld1", "fldz", "fst", "fstp", "fadd", "faddp", "fsub", "fsubp",
    "fsubr", "fsubrp", "fmul", "fmulp", "fdiv", "fdivp", "fdivr", "fdivrp",
    "fcom", "fcomp", "fcompp", "fucom", "fucomp", "fucompp", "fucomi",
    "fucomip", "fcomi", "fcomip", "fchs", "fxch", "fnstsw",
}

def x87_emulate(rows):
    """Best-effort symbolic x87 stack tracker over a linear instruction list
    (no branch following -- run it on one straight-line path at a time). Prints
    the stack (st0 first) after every x87 instruction, and expands FCOMP/FCOMPP/
    FUCOM* pairs with the two symbolic operands compared, so a `TEST AH,mask`
    afterward can be read against a named comparison instead of raw flags."""
    stack = []
    for addr, text in rows:
        mnem = text.split(" ", 1)[0]
        if mnem not in X87_MNEM:
            continue
        arg = text[len(mnem):].strip()
        if mnem in ("fld1",):
            stack.insert(0, "1.0")
        elif mnem in ("fldz",):
            stack.insert(0, "0.0")
        elif mnem == "fld":
            stack.insert(0, f"mem[{arg}]" if arg else "st?")
        elif mnem == "fchs":
            if stack:
                stack[0] = f"-({stack[0]})"
        elif mnem in ("fst", "fstp"):
            val = stack[0] if stack else "?"
            if mnem == "fstp" and stack:
                stack.pop(0)
            print(f"  {addr:08x}: {text:40s} -> {arg or 'st0'} = {val}")
            continue
        elif mnem in ("fcom", "fucom"):
            # Non-popping compare. An earlier version of this emulator only special-cased the
            # popping forms (fcomp/fucomp/fcompp/fucompp), so a non-popping fcom/fucom fell through
            # to the generic stack-dump line below and its comparison was silently invisible -- this
            # hid 2 of the 3 touch-land velocity compares (Phase 7e found this the hard way).
            other = arg if arg else (stack[1] if len(stack) > 1 else "?")
            top = stack[0] if stack else "?"
            print(f"  {addr:08x}: {text:40s} -> compare {top} vs {other} (no pop)")
            continue
        elif mnem in ("fcomp", "fucomp"):
            other = arg if arg else (stack[1] if len(stack) > 1 else "?")
            top = stack[0] if stack else "?"
            print(f"  {addr:08x}: {text:40s} -> compare {top} vs {other}")
            if stack:
                stack.pop(0)
            continue
        elif mnem in ("fcompp", "fucompp"):
            a = stack[0] if stack else "?"
            b = stack[1] if len(stack) > 1 else "?"
            print(f"  {addr:08x}: {text:40s} -> compare {a} vs {b}")
            stack = stack[2:]
            continue
        elif mnem in ("fucomip", "fcomip"):
            a = stack[0] if stack else "?"
            b = arg if arg else (stack[1] if len(stack) > 1 else "?")
            print(f"  {addr:08x}: {text:40s} -> compare {a} vs {b} (pops, sets EFLAGS)")
            if stack:
                stack.pop(0)
            continue
        print(f"  {addr:08x}: {text:40s} -> stack(st0..) = {stack}")

if __name__ == "__main__":
    va_start = int(sys.argv[1], 16)
    va_end = int(sys.argv[2], 16)
    rows = disasm_range(va_start, va_end)
    for addr, text in rows:
        print(f"{addr:08x}: {text}")
    if "--x87" in sys.argv:
        print("\n--- x87 symbolic trace ---")
        x87_emulate(rows)
