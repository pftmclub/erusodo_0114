"""Ground-truth x87 compare+branch emulator: JITs the two idioms found at the OnFrameMove
touch-land velocity gate (@31f8a300-31f8a394) and runs them on concrete floats, rather than
hand-deriving C0/C2/C3 truth tables and parity flags by reading the Intel SDM. That hand
derivation is exactly what an earlier pass got wrong for the third comparison (funcmap.tsv row
31f83380 used to say "all three < 0"; it is actually "< 0, < 0, > 0", the third strict and
opposite-signed). Allocates RWX memory and calls raw machine code via ctypes -- this is Python
3.14 64-bit, so the harness uses the Microsoft x64 calling convention (float arg in XMM0), not
the x86-32 one the real client binary uses; only the x87 sequence itself (fldz/fcom/fnstsw/test/
jcc) is copied byte-for-byte from the disassembly, the surrounding calling-convention glue is
throwaway scaffolding to invoke it. Phase 7e, June2017 effect rebuild.

Output for the two masks the touch-land gate uses:
  mask=0x41 (C0|C3), JNE  -- June's first two comparisons (mesh(+0x47c), mesh(+0x550))
  mask=0x05 (C0|C2), JP   -- June's third comparison (mesh(+0x538) = m_vVelocity.y)
"skip"/"1" means the branch that bypasses the touch-land spawn; "continue"/"0" means the gate
stays open. Confirms: continue needs mem < 0 under the first mask, mem > 0 under the second --
opposite polarity, both strict, NaN always skips.
"""
import ctypes

kernel32 = ctypes.windll.kernel32
kernel32.VirtualAlloc.restype = ctypes.c_void_p
kernel32.VirtualAlloc.argtypes = [ctypes.c_void_p, ctypes.c_size_t, ctypes.c_uint32, ctypes.c_uint32]
MEM_COMMIT = 0x1000
MEM_RESERVE = 0x2000
PAGE_EXECUTE_READWRITE = 0x40

def make_func(mask, use_jp):
    # x64 calling convention: float arg comes in XMM0.
    code = bytearray()
    code += bytes([0x48, 0x83, 0xEC, 0x28])                    # sub rsp, 0x28
    code += bytes([0xF3, 0x0F, 0x11, 0x44, 0x24, 0x20])        # movss [rsp+0x20], xmm0
    code += bytes([0xD9, 0xEE])                                 # fldz
    code += bytes([0xD8, 0x54, 0x24, 0x20])                    # fcom dword ptr [rsp+0x20]  (D8 /2)
    code += bytes([0x9B, 0xDF, 0xE0])                           # fwait; fnstsw ax
    code += bytes([0xF6, 0xC4, mask])                           # test ah, mask
    code += bytes([0x7A if use_jp else 0x75, 0x0A])             # jp/jne +10
    code += bytes([0xB8, 0, 0, 0, 0])                           # mov eax,0 (fallthrough = "continue")
    code += bytes([0x48, 0x83, 0xC4, 0x28])                     # add rsp,0x28
    code += bytes([0xC3])                                       # ret
    code += bytes([0xB8, 1, 0, 0, 0])                           # mov eax,1 (branch target = "skip")
    code += bytes([0x48, 0x83, 0xC4, 0x28])                     # add rsp,0x28
    code += bytes([0xC3])                                       # ret
    buf = bytes(code)
    addr = kernel32.VirtualAlloc(None, len(buf), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)
    ctypes.memmove(addr, buf, len(buf))
    FUNC = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_float)
    return FUNC(addr)

f_0x41_jne = make_func(0x41, False)
f_0x05_jp = make_func(0x05, True)

print("value | mask=0x41,JNE (1=skip,0=continue) | mask=0x05,JP (1=skip,0=continue)")
for v in [-1.0, 0.0, 1.0, float('nan')]:
    a = f_0x41_jne(v)
    b = f_0x05_jp(v)
    print(v, a, b)
