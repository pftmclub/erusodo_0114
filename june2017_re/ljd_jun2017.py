"""Decompile June2017 (KOG LuaJIT, header 1b 4b 4c 82) bytecode correctly.

elstools 1.0.0's jun-2014/jun-2017 opcode table has two sub-blocks swapped
(KOG opcodes 0x2f-0x37). Its decompile-batch therefore turns ITERN into USETV,
ISNEXT into USETN and UGET into USETP, and ljd then dies on the first
`for ... in pairs` loop with `assert iterator.opcode in (ITERC, ITERN)`. That
is why DamageEffect*.lua / EffectSet.lua never decompiled.

The true June mapping comes from two independent sources (README.md,
"June LuaJIT opcode table"):
  - the June binary's own lj_bc_mode[] (unpacked_x2.exe file offset
    0x1abfa08): each slot's operand types pin UGET/USETV/USETS/USETN/USETP
    uniquely, and the metamethod field separates VARG (none) from
    ITERC/ITERN (__call);
  - the data: DamageEffect_ETC.lua's first prototype (a recursive deepcopy)
    needs 0x32=ISNEXT (jumps to the loop head), 0x30=ITERN (right before
    ITERL), 0x33=UGET (loads its own upvalue for the recursive call).

This script does not modify elstools. It runs the vendored ljd on the
.std.luac that `elstools decompile-batch` leaves behind and, for each
instruction, reads the ORIGINAL KOG opcode byte at the same file offset (the
two files have identical layout; only the header and opcode bytes differ).
KOG opcodes in FIX are decoded with the corrected mapping; every other opcode
keeps elstools' translation, which the mode table agrees with.

Usage (from june2017_re/):
    python ljd_jun2017.py june_lua/DamageEffect.lua [more.lua ...]
    python ljd_jun2017.py --fixed june_lua/X.lua   # also install ljd_fixed.py's slotworks fix
Needs <stem>.std.luac next to each input (run elstools decompile-batch first;
it writes that file even when its own decompile fails). Writes <stem>.src.lua.
Exits non-zero if any file fails, or if the two files differ anywhere other
than the header and opcode bytes.
"""
import os
import runpy
import sys
import traceback

HERE = os.path.dirname(os.path.abspath(__file__))
LJD = os.path.join(HERE, '..', 'elstools', 'vendor', 'ljd')

# KOG June opcode -> stock LuaJIT 2.0 opcode, for the block elstools gets wrong.
FIX = {
    0x2f: 0x41,  # ITERC
    0x30: 0x42,  # ITERN
    0x31: 0x43,  # VARG
    0x32: 0x44,  # ISNEXT
    0x33: 0x2b,  # UGET
    0x34: 0x2c,  # USETV
    0x35: 0x2d,  # USETS
    0x36: 0x2e,  # USETN
    0x37: 0x2f,  # USETP
}
KOG_MAGIC = b'\x1bKL\x82'


_STATE = {}          # the file currently being read: set by begin() per file
_INSTALLED = [False]


def install_hooks():
    """Install once per process. The hook reads the current file's KOG bytes from
    _STATE, so several files can be parsed in one process (reconcile.py does)."""
    if _INSTALLED[0]:
        return
    _INSTALLED[0] = True
    sys.path.insert(0, LJD)
    import ljd.rawdump.code as code

    orig_read = code.read

    def read(parser):
        state = _STATE
        pos = parser.stream.pos
        kog = state['kog'][pos]
        state['oppos'].add(pos)
        if kog in FIX:
            state['fixed'][kog] = state['fixed'].get(kog, 0) + 1
            # Re-run the original reader on a codeword whose low byte is the
            # corrected stock opcode.
            stream = parser.stream
            real_read_uint = stream.read_uint

            def patched_read_uint(size=4):
                stream.read_uint = real_read_uint
                v = real_read_uint(size)
                return (v & ~0xff) | FIX[kog]
            stream.read_uint = patched_read_uint
        return orig_read(parser)
    code.read = read

    # ljd's --catch_asserts path uses the pre-3.10 traceback signature
    # (etype=/value=/tb=). We don't pass -c; this only keeps the writer alive.
    orig_fe = traceback.format_exception

    def format_exception(*a, **kw):
        if 'etype' in kw:
            return orig_fe(kw['value'])
        return orig_fe(*a, **kw)
    traceback.format_exception = format_exception


def begin(lua_path):
    """Check the pair of files and point the hook at this one. Returns (kog, std bytes)."""
    stem = lua_path[:-4] if lua_path.endswith('.lua') else lua_path
    kog = open(lua_path, 'rb').read()
    stdb = open(stem + '.std.luac', 'rb').read()
    if kog[:4] != KOG_MAGIC:
        raise SystemExit('%s: not June KOG bytecode (header %s)' % (lua_path, kog[:4].hex()))
    if len(kog) != len(stdb):
        raise SystemExit('%s: length differs from %s.std.luac' % (lua_path, stem))
    install_hooks()
    _STATE.clear()
    _STATE.update({'kog': kog, 'oppos': set(), 'fixed': {}})
    return kog, stdb


def parse_prototype(lua_path):
    """Parse a June file's bytecode (opcode-corrected) without decompiling it.
    Returns ljd's root Prototype; sub-prototypes sit in constants.complex_constants.
    Used by reconcile.py to read table keys straight from TSETS / TDUP."""
    stem = lua_path[:-4] if lua_path.endswith('.lua') else lua_path
    begin(lua_path)
    import ljd.rawdump.code as code
    import ljd.rawdump.parser as parser
    from ljd.rawdump.luajit.v2_0.luajit_opcode import _OPCODES
    code.init(_OPCODES)
    _, proto = parser.parse(stem + '.std.luac')
    if proto is None:
        raise SystemExit('%s: ljd could not parse %s.std.luac' % (lua_path, stem))
    return proto


def decompile(lua_path, fixed=False):
    stem = lua_path[:-4] if lua_path.endswith('.lua') else lua_path
    std = stem + '.std.luac'
    out = stem + '.src.lua'
    kog, stdb = begin(lua_path)
    state = _STATE
    if fixed:
        # The dropped-assignment fix (RANOX_LUA_DECOMPILE_FIX_PLAN.md Phase 4); opt-in until Phase 9.
        import ljd_fixed
        ljd_fixed.install_fix()
    sys.argv = ['ljd', '-f', std, '-o', out]
    try:
        runpy.run_path(os.path.join(LJD, 'main.py'), run_name='__main__')
    except SystemExit as e:
        if e.code not in (0, None):
            raise SystemExit('%s: ljd exited %r' % (lua_path, e.code))
    # Layout check: outside the 4-byte header, the files may differ only at
    # instruction opcode bytes.
    bad = [i for i in range(4, len(kog)) if kog[i] != stdb[i] and i not in state['oppos']]
    if bad:
        raise SystemExit('%s: %d non-opcode byte(s) differ, first at 0x%x' % (lua_path, len(bad), bad[0]))
    fixed_ops = ' '.join('%02x:%d' % kv for kv in sorted(state['fixed'].items())) or 'none'
    print('%s: ok, %d instructions, corrected KOG ops %s -> %s%s' % (
        os.path.basename(lua_path), len(state['oppos']), fixed_ops, os.path.basename(out),
        ' [%s]' % sys.modules['ljd_fixed'].stats_line() if fixed else ''))


if __name__ == '__main__':
    fixed = '--fixed' in sys.argv[1:]
    files = [a for a in sys.argv[1:] if a != '--fixed']
    if len(files) != 1:
        # ljd keeps global state per process; run one file per interpreter.
        import subprocess
        rc = 0
        for f in files:
            rc |= subprocess.call([sys.executable, os.path.abspath(__file__)] + ['--fixed'] * fixed + [f])
        sys.exit(rc)
    decompile(files[0], fixed)
