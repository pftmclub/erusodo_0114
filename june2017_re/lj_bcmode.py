"""Find and decode the June2017 KOG LuaJIT opcode-mode table (lj_bc_mode[]).

Evidence script for README.md "June LuaJIT opcode table" and ljd_jun2017.py.

lj_bc_mode[] holds one uint16 per opcode, in the VM's own (KOG-permuted)
order: ra | rb<<3 | rc<<7 | mm<<11, with the operand types below. The script
scans unpacked_x2.exe for the 93-entry uint16 array whose (ra,rb,rc) values
are exactly a permutation of stock LuaJIT 2.0's, with JMP at slot 0x54 (a slot
elstools and the data agree on), then prints every slot and the stock opcodes
whose operand types fit it. It exits non-zero unless exactly one table is
found and it agrees with ljd_jun2017.FIX.

    python lj_bcmode.py [path/to/unpacked_x2.exe]
"""
import collections
import sys

EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
M = ['___', 'dst', 'base', 'var', 'rbase', 'uv', 'lit', 'lits', 'pri', 'num', 'str', 'tab',
     'func', 'jump', 'cdata']
# Stock LuaJIT 2.0 BCDEF (lj_bc.h): name ra rb rc, in stock opcode order.
STD = """ISLT var ___ var;ISGE var ___ var;ISLE var ___ var;ISGT var ___ var;ISEQV var ___ var;
ISNEV var ___ var;ISEQS var ___ str;ISNES var ___ str;ISEQN var ___ num;ISNEN var ___ num;
ISEQP var ___ pri;ISNEP var ___ pri;ISTC dst ___ var;ISFC dst ___ var;IST ___ ___ var;
ISF ___ ___ var;MOV dst ___ var;NOT dst ___ var;UNM dst ___ var;LEN dst ___ var;
ADDVN dst var num;SUBVN dst var num;MULVN dst var num;DIVVN dst var num;MODVN dst var num;
ADDNV dst var num;SUBNV dst var num;MULNV dst var num;DIVNV dst var num;MODNV dst var num;
ADDVV dst var var;SUBVV dst var var;MULVV dst var var;DIVVV dst var var;MODVV dst var var;
POW dst var var;CAT dst rbase rbase;KSTR dst ___ str;KCDATA dst ___ cdata;KSHORT dst ___ lits;
KNUM dst ___ num;KPRI dst ___ pri;KNIL base ___ base;UGET dst ___ uv;USETV uv ___ var;
USETS uv ___ str;USETN uv ___ num;USETP uv ___ pri;UCLO rbase ___ jump;FNEW dst ___ func;
TNEW dst ___ lit;TDUP dst ___ tab;GGET dst ___ str;GSET var ___ str;TGETV dst var var;
TGETS dst var str;TGETB dst var lit;TSETV var var var;TSETS var var str;TSETB var var lit;
TSETM base ___ num;CALLM base lit lit;CALL base lit lit;CALLMT base ___ lit;CALLT base ___ lit;
ITERC base lit lit;ITERN base lit lit;VARG base lit lit;ISNEXT base ___ jump;RETM base ___ lit;
RET rbase ___ lit;RET0 rbase ___ lit;RET1 rbase ___ lit;FORI base ___ jump;JFORI base ___ jump;
FORL base ___ jump;IFORL base ___ jump;JFORL base ___ lit;ITERL base ___ jump;
IITERL base ___ jump;JITERL base ___ lit;LOOP rbase ___ jump;ILOOP rbase ___ jump;
JLOOP rbase ___ lit;JMP rbase ___ jump;FUNCF rbase ___ ___;IFUNCF rbase ___ ___;
JFUNCF rbase ___ lit;FUNCV rbase ___ ___;IFUNCV rbase ___ ___;JFUNCV rbase ___ lit;
FUNCC rbase ___ ___;FUNCCW rbase ___ ___"""
std = [x.split() for x in STD.replace('\n', '').split(';')]
IDX = {m: i for i, m in enumerate(M)}
enc = lambda s: IDX[s[1]] | (IDX[s[2]] << 3) | (IDX[s[3]] << 7)
STDLOW = [enc(s) for s in std]
JMP = [s[0] for s in std].index('JMP')


def find(d):
    want = collections.Counter(STDLOW)
    v = STDLOW[JMP]
    hits = []
    for p in range(0, len(d) - 2 * len(std), 2):
        q = p + 2 * JMP
        if d[q] != v & 0xff or (d[q + 1] & 7) != v >> 8:
            continue
        t = [d[p + 2 * i] | (d[p + 2 * i + 1] << 8) for i in range(len(std))]
        if collections.Counter(x & 0x7ff for x in t) == want:
            hits.append((p, t))
    return hits


def main():
    d = open(sys.argv[1] if len(sys.argv) > 1 else EXE, 'rb').read()
    hits = find(d)
    print('tables found: %d at %s' % (len(hits), ', '.join('file+0x%x' % p for p, _ in hits)))
    if len(hits) != 1:
        return 1
    p, t = hits[0]
    names = [s[0] for s in std]
    for i, v in enumerate(t):
        cands = [std[j][0] for j in range(len(std)) if STDLOW[j] == v & 0x7ff]
        print('%02x %04x (%s,%s,%s) mm=%2d fits %s' % (
            i, v, M[v & 7], M[(v >> 3) & 15], M[(v >> 7) & 15], v >> 11, ','.join(cands)))
    # Consistency with the decompile fix: every corrected opcode's slot must
    # carry exactly the operand types of the stock opcode it is mapped to, and
    # VARG must be the only (base,lit,lit) slot in 0x2f-0x31 without __call's mm.
    sys.path.insert(0, __import__('os').path.dirname(__import__('os').path.abspath(__file__)))
    from ljd_jun2017 import FIX
    ok = True
    for kog, stock in sorted(FIX.items()):
        match = (t[kog] & 0x7ff) == STDLOW[stock]
        ok &= match
        print('FIX %02x -> %-6s operand types %s' % (kog, names[stock], 'match' if match else 'MISMATCH'))
    call_mm = t[0x2c] >> 11  # CALL, agreed by elstools and the data
    iters = [k for k in (0x2f, 0x30, 0x31) if t[k] >> 11 == call_mm]
    print('slots in 2f-31 with CALL\'s metamethod (ITERC/ITERN): %s' % ' '.join('%02x' % k for k in iters))
    ok &= iters == [0x2f, 0x30]
    print('consistent with ljd_jun2017.FIX' if ok else 'INCONSISTENT with ljd_jun2017.FIX')
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
