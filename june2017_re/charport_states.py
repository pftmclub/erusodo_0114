"""June class state map from Ghidra decompiles (pilot-char-revamp CH0; reusable for Stage 7).

    python charport_states.py init <InitStateCommon va> [<more registration fn va> ...] > reg.tsv
    python charport_states.py skill <SetEquippedSkillFuncBySkillSlotIndex va>      > skill.tsv

Needs Ghidra's HTTP server (decompile_function). Reads the decompile text, not the bytes, so check a surprising row
against the listing (funcdis.py).

init:  every UpdateUserUnitStateData (31fd89b0) call: state id, table name, and the callbacks stored into the local
       UserUnitStateData before it (the Init clears them: inlined as explicit zero stores, or the call `FUN_320775a0();`,
       which Ghidra prints without arguments; CH1b review fix, before it every slot carried into the next state). The local's fields are found by their Ghidra stack
       names: `local_N` / `xStack_N` sit at offset (base - N), base = the local passed as the 3rd argument.
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
    m = re.match(r'^&?(?:LAB|FUN|PTR_FUN)_([0-9a-f]{8})$', rhs) or re.match(r'^0x(3[0-9a-f]{7})$', rhs)
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


def cmd_init(vas):
    """Rows marked '~' come from a name held in a variable (a per-class switch): one row per name assigned since the
    previous registration, with the callbacks as they stood at that assignment (read the branch to confirm).
    KNOWN DEFECT (pilot-char-revamp CH1b review; CH0c fixes it): Ghidra may print the name assignment before the branch's
    stores, so a '~' row can come out empty, and its id is the last uVar seen (e.g. AVSI_ABM_COMBO_Z4 reads 164, the
    branch sets 0xcb). Never trust a '~' row without the listing."""
    print('fn	line	state_id	table	callbacks')
    for fva in vas:
        L = decompile(fva)
        cur, cand, ids = {}, {}, {}
        for i, ln in enumerate(L):
            if 'FUN_320775a0(' in ln:          # UserUnitStateData::Init on the local (zeroes +0x14..+0x5c, every slot)
                cur = {}
            m = re.match(r'\s*(?:local|[a-z]+Stack)_([0-9a-f]+)\s*=\s*(.+);', ln)
            if m:
                cur[int(m.group(1), 16)] = fnval(m.group(2))
            m = re.match(r'\s*(uVar\d+) = (0x[0-9a-f]+|\d+);', ln)
            if m:
                ids[m.group(1)] = str(int(m.group(2), 0))
            m = re.match(r'\s*(pcVar\d+) = (s_\w+|(?:\([^)]*\))?0x3[0-9a-f]{7});', ln)
            if m:
                cand.setdefault(m.group(1), []).append((i + 1, strat(m.group(2)), dict(cur), dict(ids)))
            c = re.search(REG + r'\(([^,]+),([^,]+),\s*&?(?:local|[a-z]+Stack)_([0-9a-f]+)', ln)
            if c:
                base = int(c.group(3), 16)
                sid, arg = c.group(1).strip(), c.group(2).strip()
                rows = []
                if arg in cand:
                    for (cl, nm, snap, idsnap) in cand.pop(arg):
                        rows.append(('~%d' % cl, idsnap.get(sid, sid), nm, snap))
                else:
                    rows.append((str(i + 1), sid, strat(arg), cur))
                for (lno, sv, nm, snap) in rows:
                    try:
                        sv = str(int(sv, 0))
                    except ValueError:
                        pass
                    fields = {base - n: v for n, v in snap.items() if v is not None and base - n > 0}
                    print('%s	%s	%s	%s	%s' % (fva, lno, sv, nm, slotstr(fields)))


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
