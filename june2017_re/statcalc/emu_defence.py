"""june-stat-calc D1: June's defence rate and damage rate, run in Unicorn, against the port's expression.

June 32284aa0 (CX2GameUnit::GetFinalDamage):
  @32285d8d-32285dc4  rate = defence / ( (float)( level * 39.35 + 258.65 ) + defence )   ([esp+0x130] level in, rate out)
  @32286068-32286090  t = 1 - rate * [esp+0x3c] (frozen) * [esp+0x18] (skill); t < 0 -> 0      (result [esp+0x14])
The port (X2GameUnit.cpp, SERV_IRUHADEV_JUNE_STATCALC_DEFENCE_CURVE) is transcribed below as `port()`: C float
assignments round to float, x87 intermediates at 53-bit precision (the MSVC runtime's control word 0x027F).
EBX points at a fake DamageData with m_bLvDamage (+0x57c) = 0, so the JNZ @32285dc4 goes on to the damage rate.

Usage: python june2017_re/statcalc/emu_defence.py      (prints one line per mismatch, then the summary line)
"""
import ctypes
import itertools
import math
import struct

from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EBX, UC_X86_REG_FPCW

EXE = r"F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe"
BASE = 0x31aa0000
TEXT_RVA, TEXT_RAW, TEXT_SIZE = 0x1000, 0x400, 0x1cbba00
STACK, DD = 0x10000000, 0x20000000
ESP = STACK + 0x8000


def f32(x):
    return ctypes.c_float(x).value


def div(a, b):
    if b == 0.0:
        if a == 0.0 or math.isnan(a):
            return math.nan
        return math.copysign(math.inf, a) * math.copysign(1.0, b)
    return a / b


D39 = float(f32(39.35))    # June's double 33487178 is (double)39.35f
D258 = float(f32(258.65))  # June's double 33487170 is (double)258.65f


def port(level, defence, frozen, skill):
    """the port's expression, X2GameUnit.cpp GetFinalDamage under SERV_IRUHADEV_JUNE_STATCALC_DEFENCE_CURVE"""
    basic = f32(float(level) * D39 + D258)
    rate = f32(div(defence, basic + defence))
    t = f32(1.0 - rate * frozen * skill)
    if t < 0.0:
        t = 0.0
    return rate, t


def make_emu():
    data = open(EXE, "rb").read()
    mu = Uc(UC_ARCH_X86, UC_MODE_32)
    size = (TEXT_SIZE + 0xfff) & ~0xfff
    mu.mem_map(BASE + TEXT_RVA, size)
    mu.mem_write(BASE + TEXT_RVA, data[TEXT_RAW:TEXT_RAW + TEXT_SIZE])
    mu.mem_map(STACK, 0x10000)
    mu.mem_map(DD, 0x1000)
    return mu


def june(mu, level, defence, frozen, skill):
    mu.mem_write(DD + 0x57c, b"\x00")
    mu.mem_write(ESP + 0x130, struct.pack("<i", level))
    mu.mem_write(ESP + 0x24, struct.pack("<f", defence))
    mu.mem_write(ESP + 0x3c, struct.pack("<f", frozen))
    mu.mem_write(ESP + 0x18, struct.pack("<f", skill))
    mu.mem_write(ESP + 0x14, struct.pack("<f", -12345.0))
    mu.reg_write(UC_X86_REG_ESP, ESP)
    mu.reg_write(UC_X86_REG_EBX, DD)
    mu.reg_write(UC_X86_REG_FPCW, 0x027f)
    mu.emu_start(0x32285d8d, 0x32285dc4)      # stops before the JNZ (m_bLvDamage is 0: it is taken)
    rate = struct.unpack("<f", mu.mem_read(ESP + 0x130, 4))[0]
    mu.emu_start(0x32286068, 0x32286090)
    t = struct.unpack("<f", mu.mem_read(ESP + 0x14, 4))[0]
    return rate, t


def same(a, b):
    return (math.isnan(a) and math.isnan(b)) or struct.pack("<f", a) == struct.pack("<f", b)


def inputs():
    levels = [1, 2, 10, 45, 70, 80, 99]
    defences = [0.0, -0.0, 1.0, 100.0, 946.0, 3694.0, 5128.0, 11329.0, 15836.0, 1e6, -50.0, -1000.0, -1e6, math.inf]
    rates = [0.0, 0.5, 1.0]
    for lv, d, fr, sk in itertools.product(levels, defences, rates, rates):
        yield lv, d, fr, sk
    for lv in levels:  # defence = -(level curve): the divide-by-zero edge
        yield lv, -f32(float(lv) * D39 + D258), 1.0, 1.0


def main():
    mu = make_emu()
    n = bad = 0
    for lv, d, fr, sk in inputs():
        d = f32(d)
        jr, jt = june(mu, lv, d, fr, sk)
        pr, pt = port(lv, d, fr, sk)
        n += 1
        if not (same(jr, pr) and same(jt, pt)):
            bad += 1
            print("MISMATCH lv=%d def=%r frozen=%g skill=%g june=(%r, %r) port=(%r, %r)" % (lv, d, fr, sk, jr, jt, pr, pt))
    print("EMU_DEFENCE %s: %d input sets, %d mismatch(es) (rate and damage rate compared bit for bit, NaN = NaN)"
          % ("OK" if bad == 0 else "FAIL", n, bad))


if __name__ == "__main__":
    main()
