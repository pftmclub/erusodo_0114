"""June class state map from Ghidra decompiles (pilot-char-revamp CH0; reusable for Stage 7).

    python charport_states.py init <InitStateCommon va> [<more registration fn va> ...] > reg.tsv
    python charport_states.py skill <SetEquippedSkillFuncBySkillSlotIndex va>      > skill.tsv

Needs Ghidra's HTTP server (decompile_function). Reads the decompile text, not the bytes, so check a surprising row
against the listing (funcdis.py).

init:  every UpdateUserUnitStateData (31fd89b0) call: state id, table name, and the callbacks stored into the local
       UserUnitStateData before it (the Init clears them: inlined as explicit zero stores, or the call `FUN_320775a0();`,
       which Ghidra prints without arguments; CH1b review fix, before it every slot carried into the next state). The local's fields are found by their Ghidra stack
       names: `local_N` / `xStack_N` sit at offset (base - N), base = the local passed as the 3rd argument.
       A name held in a variable (a per-class switch or if / else, a `~` row) is read along its own path: see cmd_init (CH0c).
skill: the skill switch. For every case group: the skill ids, the table names passed to MakeTableReference (32fe9a40)
       for the normal and hyper state, and the callback slots written through the two state pointers (index k = byte
       offset 4k), following goto tails. Extra states a case registers through 31fd89b0 are listed with their ids.
Callback slots (UserUnitStateData, funcmap 320775a0): +0x18 Init, +0x20 StartFuture, +0x28 Start, +0x30 FrameMoveFuture,
+0x38 FrameMove, +0x40 CameraMove, +0x48 EventProcess, +0x50 EndFuture, +0x58 End.
Each value is resolved through a 5-byte `jmp` thunk to the real function (`thunk>target`).
"""
import re, sys, struct, subprocess

EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
BASE = 0x31aa0000 + 0xc00
SLOTS = {0x18: 'Init', 0x20: 'StartFuture', 0x28: 'Start', 0x30: 'FrameMoveFuture', 0x38: 'FrameMove',
         0x40: 'CameraMove', 0x48: 'EventProcess', 0x50: 'EndFuture', 0x58: 'End'}
REG = 'FUN_31fd89b0'
MTR = 'FUN_32fe9a40'
SKIP_CALLS = {'32fe9a40', '31fd89b0', '320775a0', '32073380', '31fd88f0'}
_d = None


def exe():
    global _d
    if _d is None:
        _d = open(EXE, 'rb').read()
    return _d


def resolve(va):
    d = exe()
    o = va - BASE
    if 0 <= o < len(d) - 5 and d[o] == 0xe9:
        t = (va + 5 + struct.unpack('<i', d[o + 1:o + 5])[0]) & 0xffffffff
        return '%08x>%08x' % (va, t)
    return '%08x' % va


def decompile(va):
    return subprocess.run(['curl', '-s', '--noproxy', '127.0.0.1', '-m', '600', '-G',
                           'http://127.0.0.1:8080/decompile_function', '--data-urlencode', 'address=' + va],
                          capture_output=True, text=True).stdout.split('\n')


def fnval(rhs):
    rhs = re.sub(r'^\([^)]*\)\s*', '', rhs.strip().rstrip(';').strip())
    m = re.match(r'^&?(?:LAB|FUN|PTR_FUN|DAT)_([0-9a-f]{8})$', rhs) or re.match(r'^0x(3[0-9a-f]{7})$', rhs)
    return int(m.group(1), 16) if m else None


def sname(tok):
    m = re.match(r's_([A-Za-z0-9_]+?)_([0-9a-f]{8})$', tok.strip())
    if not m:
        return tok.strip()
    d, o = exe(), int(m.group(2), 16) - BASE  # Ghidra cuts long labels: read the string itself
    e = d.find(b'\x00', o)
    return d[o:e].decode('latin-1') if 0 <= o < len(d) and 0 < e - o < 128 else m.group(1)


def slotstr(fields):
    return ' '.join('%s=%s' % (SLOTS[o], resolve(v)) for o, v in sorted(fields.items()) if o in SLOTS and v)


def strat(tok):
    """A table-name argument: an s_ label, or a raw 0x3....... address (read from the exe)."""
    tok = tok.strip()
    m = re.match(r'^(?:\([^)]*\))?0x(3[0-9a-f]{7})$', tok)
    if m:
        d, o = exe(), int(m.group(1), 16) - BASE
        e = d.find(b'\x00', o)
        return d[o:e].decode('latin-1') if 0 < e - o < 128 else tok
    return sname(tok)


STORE = re.compile(r'\s*(?:local|[a-z]+Stack)_([0-9a-f]+)\s*=\s*(.+);')
IDSET = re.compile(r'\s*(uVar\d+) = (0x[0-9a-f]+|\d+);')
NAMESET = re.compile(r'\s*(pcVar\d+) = (s_\w+|(?:\([^)]*\))?0x3[0-9a-f]{7});')
REGCALL = re.compile(REG + r'\(([^,]+),([^,]+),\s*&?(?:local|[a-z]+Stack)_([0-9a-f]+)')


def ind(ln):
    return len(ln) - len(ln.lstrip(' '))


def apply(ln, cur, ids):
    """One line's effect on the local UserUnitStateData (cur: stack offset -> fn or None) and the id variables."""
    if 'FUN_320775a0(' in ln:          # UserUnitStateData::Init on the local (zeroes +0x14..+0x5c, every slot)
        cur.clear()
    m = STORE.match(ln)
    if m:
        cur[int(m.group(1), 16)] = fnval(m.group(2))
    m = IDSET.match(ln)
    if m:
        ids[m.group(1)] = str(int(m.group(2), 0))


def close_of(L, i):
    """The line of the `}` that closes the block opened on line i (same indent)."""
    k = ind(L[i])
    for j in range(i + 1, len(L)):
        if ind(L[j]) == k and L[j].strip() == '}':
            return j
    return len(L) - 1


def walk(L, a, cur, ids, pc, labels):
    """Follow the path from line a (where pc gets its name) to the 31fd89b0 call that consumes pc: the rest of the branch
    up to break / goto / its closing brace, then the merge, skipping sibling cases and else blocks. Returns
    (call line, id token, cur, ids, flags); flags notes slot stores made inside a nested block (read the listing)."""
    cur, ids, flags = dict(cur), dict(ids), set()
    lvl, i, steps = ind(L[a]), a + 1, 0
    while i < len(L) and steps < 4000:
        steps += 1
        ln, s = L[i], L[i].strip()
        c = REGCALL.search(ln)
        if c and c.group(2).strip() == pc:
            return i, c.group(1).strip(), cur, ids, flags
        g = re.match(r'goto (\w+);$', s)
        if g:
            if g.group(1) not in labels:
                return None
            i = labels[g.group(1)] + 1
            lvl = ind(L[i])
            continue
        if s == 'break;':
            sw = next((j for j in range(i - 1, -1, -1) if L[j].strip().startswith('switch(') and ind(L[j]) < ind(ln)
                       and close_of(L, j) > i), None)
            if sw is None:
                return None
            i, lvl = close_of(L, sw) + 1, ind(L[sw])
            continue
        if s == '}' and ind(ln) < lvl:
            j, lvl = i, ind(ln)
            while j + 1 < len(L) and L[j + 1].strip().startswith('else') and ind(L[j + 1]) == lvl:
                j = close_of(L, j + 1)
            i = j + 1
            continue
        if re.match(r'(case [^:]+|default):$', s):   # fall through into the next case label
            i += 1
            continue
        if STORE.match(ln) and ind(ln) > lvl and fnval(STORE.match(ln).group(2)) is not None:
            flags.add('nested store @%d' % (i + 1))
        apply(ln, cur, ids)
        i += 1
    return None


def cmd_init(vas):
    """Rows marked '~' come from a name held in a variable (a per-class switch or if / else): one row per name assignment.
    CH0c fix (pilot-char-revamp; the CH1b review found it): a '~' row is read along its own path. The state at the start of
    a case / else is the state at its switch / if, not the previous sibling's. From the name assignment the path takes the
    rest of that branch (up to break, goto or its closing brace), then the stores after the merge, up to the 31fd89b0 call
    that consumes the name; the id is the variable's value on that path. A '?' after the line number means a slot store
    sat in a nested block on that path (both arms applied): check it against the listing (funcdis.py)."""
    print('fn	line	state_id	table	callbacks')
    for fva in vas:
        L = decompile(fva)
        labels = {}
        for i, ln in enumerate(L):
            m = re.match(r'\s*(LAB_[0-9a-f]+|code_r0x[0-9a-f]+):', ln)
            if m:
                labels[m.group(1)] = i
        cur, ids, frames, out = {}, {}, [], []
        for i, ln in enumerate(L):
            s = ln.strip()
            # branch entry: a case / else starts from the state at its switch / if (CH0c)
            while frames and ind(ln) < frames[-1][0]:
                frames.pop()
            if frames and ind(ln) == frames[-1][0]:
                k, kind, scur, sids = frames[-1]
                if (kind == 'switch' and re.match(r'(case [^:]+|default):$', s)) or (kind == 'if' and s.startswith('else')):
                    cur, ids = dict(scur), dict(sids)
                elif s == '}':
                    if not (i + 1 < len(L) and L[i + 1].strip().startswith('else') and ind(L[i + 1]) == k):
                        frames.pop()
            if re.match(r'(switch\(|if \().*\{$', s):
                frames.append((ind(ln), 'switch' if s.startswith('switch') else 'if', dict(cur), dict(ids)))
            apply(ln, cur, ids)
            m = NAMESET.match(ln)
            if m:
                w = walk(L, i, cur, ids, m.group(1), labels)
                if w is None:
                    out.append((i, '%s	~%d!	?	%s	(no consuming 31fd89b0 call found: read the listing)' % (fva, i + 1, strat(m.group(2)))))
                    continue
                cl, sid, wcur, wids, flags = w
                base = int(REGCALL.search(L[cl]).group(3), 16)
                sv = wids.get(sid, sid)
                try:
                    sv = str(int(sv, 0))
                except ValueError:
                    pass
                fields = {base - n: v for n, v in wcur.items() if v is not None and base - n > 0}
                out.append((i, '%s	~%d%s	%s	%s	%s' % (fva, i + 1, '?' if flags else '', sv, strat(m.group(2)), slotstr(fields))))
                continue
            c = REGCALL.search(ln)
            if c and not re.match(r'pcVar\d+$', c.group(2).strip()):
                base = int(c.group(3), 16)
                sv = c.group(1).strip()
                try:
                    sv = str(int(sv, 0))
                except ValueError:
                    pass
                fields = {base - n: v for n, v in cur.items() if v is not None and base - n > 0}
                out.append((i, '%s	%d	%s	%s	%s' % (fva, i + 1, sv, strat(c.group(2).strip()), slotstr(fields))))
        for _, row in sorted(out):
            print(row)


def cmd_skill(fva):
    L = decompile(fva)
    labels = {}
    for i, ln in enumerate(L):
        m = re.match(r'\s*(LAB_[0-9a-f]+|code_r0x[0-9a-f]+):', ln)
        if m:
            labels[m.group(1)] = i
    ptrs = []  # state-data pointers from FUN_31fd88f0( id ): first = normal, second = hyper
    for ln in L:
        m = re.match(r'\s*(\w+) = \([^)]*\)FUN_31fd88f0\(([\w\[\]]+)\);', ln)
        if m:
            ptrs.append((m.group(1), m.group(2).split('[')[0]))
    (pn, _), (ph, idh) = ptrs[0], ptrs[1]
    caseln = re.compile(r'\s*(case [^:]+|default):\s*$')
    starts, i = [], 0
    while i < len(L):
        if caseln.match(L[i]):
            vals, j = [], i
            while j < len(L) and caseln.match(L[j]):
                v = re.match(r'\s*case ([^:]+):', L[j])
                vals.append(v.group(1) if v else 'default')
                j += 1
            starts.append((j, vals))
            i = j
        elif re.match(r'\s*if \(iVar\d+ == (0x[0-9a-f]+)\) \{\s*$', L[i]):  # an id tested outside the switch
            starts.append((i + 1, [re.match(r'\s*if \(iVar\d+ == (0x[0-9a-f]+)\)', L[i]).group(1)]))
            i += 1
        else:
            i += 1
    print('fn\tline\tcases\tnormal_table\thyper_table\tnormal_callbacks\thyper_callbacks\textra_states\tcalls')
    for st, vals in starts:
        N, H, var, names, extra, calls, tabs = {}, {}, {}, {}, [], set(), {'N': [], 'H': []}
        i, steps, seen = st, 0, set()
        while i < len(L) and steps < 600:
            ln = L[i]
            steps += 1
            if re.match(r'\s*break;', ln):
                break
            g = re.match(r'\s*goto (\w+);', ln)
            if g:
                if g.group(1) in labels and g.group(1) not in seen and 'LAB_32cf06f8' != g.group(1):
                    seen.add(g.group(1))
                    i = labels[g.group(1)] + 1
                    continue
                break
            if re.match(r'\s*}\s*$', ln) and len(ln) - len(ln.lstrip()) <= 2:
                break
            m = re.match(r'\s*(\w+)\[(0x[0-9a-f]+|\d+)\] = (.+);', ln)
            if m and m.group(1) in (pn, ph):
                k = int(m.group(2), 0) * 4
                rhs = m.group(3).strip()
                tgt = N if m.group(1) == pn else H
                r = re.match(r'(?:\([^)]*\))?(\w+)\[(0x[0-9a-f]+|\d+)\]$', rhs)
                if r and r.group(1) == pn:
                    v = N.get(int(r.group(2), 0) * 4)
                elif rhs in var:
                    v = var[rhs]
                else:
                    v = fnval(rhs)
                if v is not None:
                    tgt[k] = v
            m = re.match(r'\s*((?:puVar|pcVar|pvVar|pp?uVar)\d+) = (.+);', ln)
            if m and m.group(1) not in (pn, ph):
                rhs = m.group(2).strip()
                rr = re.match(r'\([^)]*\)(\w+)\[(0x[0-9a-f]+|\d+)\]$', rhs)
                if rr and rr.group(1) == pn:
                    var[m.group(1)] = N.get(int(rr.group(2), 0) * 4)
                elif rhs.startswith('s_'):
                    names.setdefault(m.group(1), []).append(sname(rhs))  # every branch's name
                else:
                    var[m.group(1)] = fnval(rhs)
            m = re.search(MTR + r'\(([^,]+),([^,]+),', ln)
            if m:
                key = 'H' if m.group(2).strip().startswith(idh) else 'N'
                for nm in names.get(m.group(1).strip(), [sname(m.group(1))]):
                    if nm not in tabs[key]:
                        tabs[key].append(nm)
            m = re.search(REG + r'\(([^,]+),([^,]+),', ln)
            if m:
                sid = m.group(1).strip()
                try:
                    sid = str(int(sid, 0))
                except ValueError:
                    pass
                extra.append('%s:%s' % (sid, sname(m.group(2))))
            for f in re.findall(r'FUN_([0-9a-f]{8})\(', ln):
                if f not in SKIP_CALLS:
                    calls.add(f)
            i += 1
        cv = ','.join(str(int(v, 0)) if re.match(r'^(0x[0-9a-f]+|\d+)$', v) else v for v in vals)
        print('%s\t%d\t%s\t%s\t%s\t%s\t%s\t%s\t%s' % (fva, st, cv, ';'.join(tabs['N']), ';'.join(tabs['H']),
              slotstr(N), slotstr(H), ' '.join(extra), ' '.join(sorted(calls))))


if __name__ == '__main__':
    if sys.argv[1] == 'init':
        cmd_init(sys.argv[2:])
    else:
        cmd_skill(sys.argv[2])
