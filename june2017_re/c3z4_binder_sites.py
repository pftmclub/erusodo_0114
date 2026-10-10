r"""June binder sites for Lua method names (June effect C3z-4, 2026-10-10).

Each lua_tinker registration in June's two binders (31be67a0 KTDX, 31d95080 X2) decrypts the name onto the stack, then
`mov reg, <fn>` / push reg / push name / push L / call class_def<T> (C3z-1: IsVotingLuto @31dda25e, fn 31e4e9c0).
This reads the first .text immediate after each name's decrypt (use_va) and the first call after it; the call target is the
class_def<T> instantiation, named by majority vote of the June class list (c3z4_class_census.june) over its names.

    python june2017_re/c3z4_binder_sites.py NAME [NAME ...]     -> name, class, use_va, fn, class_def instantiation
"""
import os, sys, csv, re, collections, capstone
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import disasm_range as D

BINDERS = ('31be67a0', '31d95080')

def sites():
    text = D.load_text()
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    lo, hi = D.BASE + D.TV, D.BASE + D.TV + D.TS
    out = []
    for r in csv.DictReader(open(os.path.join(HERE, 'xorstrings.tsv'), encoding='utf-8'), delimiter='\t'):
        if r['func_va'] not in BINDERS or r['status'] != 'ok': continue
        va = int(r['use_va'], 16)
        off = D.va_to_raw(va) - D.TR
        fn = call = None
        for ins in md.disasm(text[off:off + 0x260], va):
            if ins.mnemonic == 'call':
                if fn is not None:
                    try: call = int(ins.op_str, 16)
                    except ValueError: call = ins.op_str
                    break
                continue
            if fn is None and ins.mnemonic in ('mov', 'push'):
                m = re.search(r'0x([0-9a-f]{8})$', ins.op_str)
                if m and lo <= int(m.group(1), 16) < hi and (ins.mnemonic == 'push' or ins.op_str.startswith('e')):
                    fn = int(m.group(1), 16)
        out.append((r['plaintext'], r['use_va'], fn, call))
    return out

def classify(rows):
    import c3z4_class_census as C
    lua, parent, gdef, ret = C.march(C.preprocess(True))
    jcls = C.june(set(lua), set().union(*lua.values()) | gdef)
    by = collections.defaultdict(collections.Counter)
    for c, ms in jcls.items():
        for m in ms: pass
    owner = collections.defaultdict(set)
    for c, ms in jcls.items():
        for m in ms: owner[m].add(c)
    for name, use, fn, call in rows:
        for c in owner.get(name, ()): by[call][c] += 1
    return {k: v.most_common(1)[0][0] for k, v in by.items() if v}

def main():
    want = set(sys.argv[1:])
    rows = sites()
    cls = classify(rows)
    for name, use, fn, call in rows:
        if not want or name in want:
            print('%-40s %-26s use %s fn %s class_def %s' % (name, cls.get(call, '?'), use,
                  '%08x' % fn if fn else '-', '%08x' % call if isinstance(call, int) else call))

if __name__ == '__main__':
    main()
