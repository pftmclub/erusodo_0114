"""Flag slotN globals read before this file's own decompile ever assigns them.

elstools'/ljd's decompile of these KOG LuaJIT files sometimes drops a real bytecode
assignment and inlines the computing call straight into the `if`/`elseif` condition that
reads it (see RANOX_LUA_DECOMPILE_BUG.md). Because none of the Ranox-ported .src.lua files
declare `local`, a dropped assignment doesn't produce Lua's nil-safe undefined -- every bare
`slotN` compiles under `elstools compile-batch -v mar-2014` to a genuine `_env["slotN"]`
global, so the "missing" value is whatever that global last held. This is a heuristic
read-before-write tracker, not a bytecode-level check -- it flags candidates cheaply; each
hit still needs verifying against real bytecode (RANOX_LUA_DECOMPILE_BUG.md's "How to verify
a candidate") before you trust it as a real bug.

Usage:
    python lint_decompile_slots.py <file.src.lua> [more.src.lua ...]

Zero output for a file means no hits. Known limitations: regex-based, not a real Lua
parser -- multi-line statements, string literals containing `=`, and nested nameless
functions (table-literal callbacks) are not handled and may under- or over-report.
"""
import re
import sys

slot_re = re.compile(r'\bslot(\d+)\b')
assign_re = re.compile(r'^\s*slot(\d+)(\.\w+)?\s*=\s*[^=]')
for_num_re = re.compile(r'^\s*for\s+slot(\d+)\s*=')
for_in_re = re.compile(r'^\s*for\s+((?:slot\d+\s*,\s*)*slot\d+)\s+in\s')
opens_re = re.compile(r'\b(function|if|for|while|do)\b')
closes_re = re.compile(r'\bend\b')


def lint_file(path):
    text = open(path, encoding='utf-8').read()
    lines = text.splitlines()
    # Strip `--` line comments (not block comments) so commented-out code doesn't trip the scan.
    clean = [ln[:ln.find('--')] if '--' in ln else ln for ln in lines]

    func_starts = [(i, re.match(r'^function\s+(\w+)\s*\(([^)]*)\)', clean[i]))
                   for i in range(len(clean))]
    func_starts = [(i, m) for i, m in func_starts if m]

    def scan_function(start_idx, params):
        defined = set()
        for p in params.split(','):
            m = re.match(r'\s*slot(\d+)', p)
            if m:
                defined.add(int(m.group(1)))
        j = start_idx
        block_depth = 0
        issues = []
        while j < len(clean):
            line = clean[j]
            block_depth += len(opens_re.findall(line))

            m_for_num = for_num_re.match(line)
            m_for_in = for_in_re.match(line)
            m_assign = assign_re.match(line)

            if m_for_num:
                defined.add(int(m_for_num.group(1)))
            elif m_for_in:
                for tok in m_for_in.group(1).split(','):
                    mm = re.match(r'\s*slot(\d+)', tok)
                    if mm:
                        defined.add(int(mm.group(1)))
            elif m_assign and '==' not in line[:line.find('=') + 2]:
                eq_pos = line.find('=')
                while eq_pos != -1 and eq_pos > 0 and line[eq_pos - 1] in '=~<>':
                    eq_pos = line.find('=', eq_pos + 1)
                if eq_pos != -1:
                    lhs, rhs = line[:eq_pos], line[eq_pos + 1:]
                    for n_ in [int(x) for x in slot_re.findall(rhs)]:
                        if n_ not in defined:
                            issues.append((j + 1, n_, line.strip()))
                    for n_ in [int(x) for x in slot_re.findall(lhs)]:
                        defined.add(n_)
                else:
                    for n_ in [int(x) for x in slot_re.findall(line)]:
                        if n_ not in defined:
                            issues.append((j + 1, n_, line.strip()))
            else:
                for n_ in [int(x) for x in slot_re.findall(line)]:
                    if n_ not in defined:
                        issues.append((j + 1, n_, line.strip()))

            block_depth -= len(closes_re.findall(line))
            if block_depth <= 0 and j > start_idx:
                break
            j += 1
        return issues

    hits = []
    for idx, m in func_starts:
        issues = scan_function(idx, m.group(2))
        if issues:
            hits.append((m.group(1), idx + 1, issues))
    return hits


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)
    total = 0
    for path in sys.argv[1:]:
        hits = lint_file(path)
        if not hits:
            continue
        if len(sys.argv) > 2:
            print(f'### {path}')
        for name, ln, issues in hits:
            print(f'== {name} (line {ln}) ==')
            for iln, n_, txt in issues:
                print(f'  line {iln}: slot{n_} read before write: {txt}')
                total += 1
    if total:
        print(f'TOTAL_HITS={total}')
        sys.exit(1)
