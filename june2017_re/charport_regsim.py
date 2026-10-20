"""Per-class path simulation of a June class registrar (InitState / InitStateCommon) decompile (pilot-char-revamp CH1j).

    python charport_regsim.py <decompile.c>      # decompile_function output saved to a file

Flattens the decompile (if / else, switch with fall-through, goto) and walks it once per unit class, taking the branch the class
takes at each test of the unit class (*(iVar6 + 0x38), a register copy of it, or the templet test of InitStateCommon). Prints
per class the UpdateUserUnitStateData (31fd89b0) calls in order (line, state id, table, callbacks; the charport_states slot rules)
and the other calls. The last registration of an id on a class's path is what that class gets. CLASSES below are Aisha's;
another class needs its own list and its own class-test patterns. Unknown conditions are listed at the end (taken as false).
"""
import os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.stdout.reconfigure(encoding='utf-8')
import charport_states as cs


def ind(l):
    return len(l) - len(l.lstrip(' '))


def compile_(L):
    ops = []
    labels = {}
    block_breaks = []
    start = next(k for k, l in enumerate(L) if l.strip() == '{') + 1
    end = max(k for k, l in enumerate(L) if l.strip() == '}' and ind(l) == 0)

    def close_of(i):
        k = ind(L[i])
        for j in range(i + 1, len(L)):
            if ind(L[j]) == k and L[j].strip().startswith('}'):
                return j
        raise Exception('no close %d' % i)

    def extent(k, c):
        t = L[k].strip()
        nk = k + 1
        if t.endswith('{'):
            nk = close_of(k) + 1
            while nk < c and L[nk].strip().startswith('else') and ind(L[nk]) == ind(L[k]):
                nk = close_of(nk) + 1
        return nk

    def block(a, b):
        i = a
        while i < b:
            s = L[i].strip()
            if not s:
                i += 1
                continue
            m = re.match(r'^(LAB_\w+|code_r0x\w+):$', s)
            if m:
                labels[m.group(1)] = len(ops)
                i += 1
                continue
            m = re.match(r'^if (\(.*\)) goto (\w+);$', s)
            if m:
                ops.append(['JF', m.group(1), ('@', len(ops) + 2), i])
                ops.append(['J', m.group(2), i])
                i += 1
                continue
            m = re.match(r'^goto (\w+);$', s)
            if m:
                ops.append(['J', m.group(1), i])
                i += 1
                continue
            if s.startswith('switch(') and s.endswith('{'):
                c = close_of(i)
                sw = ['SW', s[len('switch('):-3], {}, None, i]
                ops.append(sw)
                brk = []
                block_breaks.append(brk)
                k = i + 1
                while k < c:
                    t = L[k].strip()
                    if ind(L[k]) == ind(L[i]) and re.match(r'^(case [^:]+|default):$', t):
                        lab = t[:-1]
                        if lab == 'default':
                            sw[3] = len(ops)
                        else:
                            sw[2][int(lab[5:], 0)] = len(ops)
                        k += 1
                        continue
                    nk = extent(k, c)
                    block(k, nk)
                    k = nk
                block_breaks.pop()
                endpos = len(ops)
                if sw[3] is None:
                    sw[3] = endpos
                for b_ in brk:
                    ops[b_][1] = ('@', endpos)
                i = c + 1
                continue
            if s.startswith('if (') and s.endswith('{'):
                ends = []
                cur = i
                while True:
                    t = L[cur].strip()
                    c = close_of(cur)
                    if t.startswith('if (') or t.startswith('else if ('):
                        cond = t[t.index('('):-2].strip()
                        jf = ['JF', cond, None, cur]
                        ops.append(jf)
                        block(cur + 1, c)
                        ends.append(len(ops))
                        ops.append(['J', None, cur])
                        jf[2] = ('@', len(ops))
                    else:
                        block(cur + 1, c)
                    nxt = c + 1
                    if nxt < len(L) and L[nxt].strip().startswith('else') and ind(L[nxt]) == ind(L[i]):
                        cur = nxt
                        continue
                    break
                for e in ends:
                    ops[e][1] = ('@', len(ops))
                i = nxt
                continue
            if s == 'break;':
                if not block_breaks:
                    raise Exception('break outside switch at %d' % (i + 1))
                block_breaks[-1].append(len(ops))
                ops.append(['J', None, i])
                i += 1
                continue
            if s.startswith('do {') or s.startswith('while') or s.startswith('for ('):
                raise Exception('loop at %d: %s' % (i + 1, s))
            ops.append(('S', i))
            i += 1

    block(start, end)
    return ops, labels


CLASSEXPR = [r'\*\(undefined4 \*\)\(iVar6 \+ 0x38\)', r'\*\(int \*\)\(iVar6 \+ 0x38\)']
UNKNOWN = []


ALIAS = set()


def evalcond(cond, cls, line):
    for a in ALIAS:
        cond = re.sub(r'\b%s\b' % a, '*(int *)(iVar6 + 0x38)', cond)
    if 'iVar1 = *(int *)(iVar1 + 4)' in cond:   # InitStateCommon: unit-templet class test (Battle Magician / Dimension Witch)
        return not (cls == 0x17 or cls == 0x71)
    c = cond
    for e in CLASSEXPR:
        c = re.sub(e, 'CLS', c)
    m = re.match(r'^\(CLS == (0x[0-9a-f]+|\d+)\)$', c.strip())
    if m:
        return cls == int(m.group(1), 0)
    if 'CLS' not in c and re.search(r'piVar\d+ != \(int \*\)0x0|iVar\d+ == 0|iVar\d+ \+ -1 == 0|< 0x10|0xf <', c):
        return False
    UNKNOWN.append((line + 1, cond))
    return False


def run(L, ops, labels, cls):
    pc, cur, ids, names, out, steps = 0, {}, {}, {}, [], 0
    ALIAS.clear()
    while pc < len(ops) and steps < 200000:
        steps += 1
        op = ops[pc]
        if op[0] == 'S':
            ln = L[op[1]]
            ma = re.match(r'\s*(iVar\d+) = (.*);', ln)
            if ma:
                if re.fullmatch(r'\*\(int \*\)\(iVar6 \+ 0x38\)', ma.group(2).strip()):
                    ALIAS.add(ma.group(1))
                else:
                    ALIAS.discard(ma.group(1))
            cs.apply(ln, cur, ids)
            m = cs.NAMESET.match(ln)
            if m:
                names[m.group(1)] = cs.strat(m.group(2))
            c = cs.REGCALL.search(ln)
            if c:
                base = int(c.group(3), 16)
                sid = c.group(1).strip()
                sid = ids.get(sid, sid)
                try:
                    sid = str(int(sid, 0))
                except ValueError:
                    pass
                nm = c.group(2).strip()
                nm = names.get(nm, '?' + nm) if re.match(r'pcVar\d+$', nm) else cs.strat(nm)
                fields = {base - n: v for n, v in cur.items() if v is not None and base - n > 0}
                out.append(('REG', op[1] + 1, sid, nm, cs.slotstr(fields)))
            else:
                for f in re.findall(r'FUN_([0-9a-f]{8})\(', ln):
                    if f not in ('320775a0', '31fd89b0'):
                        out.append(('CALL', op[1] + 1, f, ln.strip()))
            pc += 1
        elif op[0] == 'J':
            t = op[1]
            pc = t[1] if isinstance(t, tuple) else labels[t]
        elif op[0] == 'JF':
            pc = pc + 1 if evalcond(op[1], cls, op[3]) else op[2][1]
        elif op[0] == 'SW':
            if not any(re.fullmatch(x, op[1]) for x in CLASSEXPR):
                raise Exception('unknown switch %s' % op[1])
            pc = op[2].get(cls, op[3])
    if steps >= 200000:
        out.append(('LOOP?',))
    return out


if __name__ == '__main__':
    L = open(sys.argv[1], encoding='utf-8', errors='replace').read().split('\n')
    ops, labels = compile_(L)
    for cls in [2, 0xe, 0xf, 0x17, 0x68, 0x69, 0x71]:
        print('### class', cls)
        for r in run(L, ops, labels, cls):
            print('\t'.join(str(x) for x in r))
    for u in sorted(set(UNKNOWN)):
        print('UNKNOWN COND', u)
