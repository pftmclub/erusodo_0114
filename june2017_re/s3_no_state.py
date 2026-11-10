"""S3 census generator: does the March class file have a state for every June skill-tree id?
(CHARACTER_PORT_CHECKLIST S3; rebuilds the rows of s3_reform_ids_no_state.tsv for one class family.)

    python june2017_re/s3_no_state.py --family X2GuEve.cpp [-o out.tsv]
    python june2017_re/s3_no_state.py --family X2GUArme_VioletMage.cpp --loose
    python june2017_re/s3_no_state.py --family X2GuEve.cpp --live-lines live.txt
    python june2017_re/s3_no_state.py --family X2GuEve.cpp --preprocess      # exact live branches via cl /P

Input (tree rows).  The rows are read from the existing s3_reform_ids_no_state.tsv (--tsv), filtered on its
`family_cpp_file` column.  The tsv is the only place that holds June's per-class tree (class, id, RNSI name,
templet name, type, master level, row / column) already joined and in the order the checklist cites; June's
SkillTree*.src.lua / NewSkillTempletVer2.src.lua would need a Lua walk plus the same join, and the tsv rows do not
change when a March class file does.  Only the verdict columns are recomputed.  A June-only class (no rows in the
tsv) is listed in JUNE_ONLY_FAMILIES below: its rows (columns 1-10) are read from June's NewSkillTempletVer2.src.lua
tree rows + SKILL_ID names in Enum.src.lua, and a missing March class file counts as empty (every row no_state).

Class text.  --class-file (repeatable; default X2Lib/<family> and the same name with .h) is read as bytes and
decoded latin-1 (CP949 safe); // and /* */ comments are stripped (line numbers kept), strings left alone.

Evidence per tree id (all kinds are computed; the verdict uses the strict ones unless --loose):
  case    `case CX2SkillTree::SI_X:` (SI_X resolved through X2SkillTree.h's SKILL_ID enum, computed the way C++
          does: `= N` / `= OTHER + k` explicit values, otherwise previous + 1), `case <number>:`, and
          `case <const int name>:` (const resolved from `const int NAME = N;` in the class files)
  const   `const int NAME = <id>;` in the class files (a definition only, not proof of a state)
  cmp     `== <id>` / `!= <id>` / `<id> ==` against a tree id (not proof of a state either)
verdict: has_state when a `case` exists for the id, else no_state.  --loose also accepts `const` and `cmp`.
The last column, `evidence`, lists what was found as kind@file:line (first 3), so a false positive can be read off.

Columns 1-20 are the original tsv's (in_cpp_enum = the name is an enumerator of SKILL_ID; march_value_of_name /
march_names_at_same_id = its value / the enumerators with the id's value; cpp_any_token_in_family = the name occurs
as a token in the family files; case_in_X2GUUser_cpp = a case for the id in X2Lib/X2GUUser.cpp;
lookalike_march_name = the name with RNSI_ -> SI_ when that enumerator exists).  Column 21 `evidence` is new.

Live / dead branches.  Default is textual: every occurrence counts, #ifdef or not (and X2SkillTree.h's SKILL_ID enum
is evaluated with a crude defined-set: every uncommented `#define NAME` in Always.h, ServerDefine*.h, X2Define.h
etc., conditionals ignored).  --live-lines FILE restricts the class files to listed ranges (`start-end` or
`file:start-end` per line, 1-based, inclusive; a bare range applies to every class file).  --preprocess runs
`cl /P` (june2017_re/census.py's recipe) on the family .cpp and takes the enum and the class text from the result,
so live branches are exact; it needs the VS2010 toolchain and takes about a minute.
"""
import argparse
import bisect
import csv
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..'))
X2LIB = os.path.join(TRUNK, 'X2Lib')
DEFAULT_TSV = os.path.join(HERE, 's3_reform_ids_no_state.tsv')
SKILL_TREE_H = os.path.join(X2LIB, 'X2SkillTree.h')
USER_CPP = os.path.join(X2LIB, 'X2GUUser.cpp')
DEFINE_FILES = [
    'KTDXLIB/Always.h', 'KTDXLIB/AlwaysButConditionally.h', 'KTDXLIB/OnlyGlobal/Always_US.h',
    'KncWX2Server/Common/ServerDefine.h', 'KncWX2Server/Common/OnlyGlobal/ServerDefine/ServerDefine_Global.h',
    'KncWX2Server/Common/OnlyGlobal/ServerDefine/ServerDefine_US.h', 'X2Lib/X2Define.h',
]
JUNE_LUA_DIR = os.path.join(TRUNK, 'june_src_fixed', 'data036_kom')
JUNE_ONLY_FAMILIES = {          # family_cpp_file -> UNIT_CLASS names, in the order the rows are written
    'X2GULuciel.cpp': ('UC_LUCIEL_DUO', 'UC_LUCIEL_CHILIARCH', 'UC_LUCIEL_ROYAL_GUARD', 'UC_LUCIEL_DIABLURA',
                       'UC_LUCIEL_DREAD_LORD', 'UC_LUCIEL_NOBLESSE', 'UC_LUCIEL_DEMONIO'),
}
# Classes of a family that the tsv has no rows for (appended by --append-missing; the tsv's other rows are never touched).
TSV_MISSING_CLASSES = {
    'X2GUAra.cpp': ('UC_ARA_LITTLE_SPECTER', 'UC_ARA_ASURA'),
}
COLS = ['unit_class_name', 'unit_class_value', 'family_cpp_file', 'skill_id', 'skill_name', 'templet_name',
        'skill_type', 'master_level', 'row', 'column', 'in_cpp_enum', 'cpp_case_in_family', 'verdict',
        'march_value_of_name', 'march_names_at_same_id', 'cpp_any_token_in_family', 'case_in_X2GUUser_cpp',
        'lookalike_march_name', 'lookalike_march_value', 'lookalike_case_in_family']
OUT_COLS = COLS + ['evidence']


def read_latin1(path):
    with open(path, 'rb') as f:
        return f.read().decode('latin-1').replace('\r\n', '\n').replace('\r', '\n')


# ---------------------------------------------------------------- C++ text helpers
_CPP_TOK = re.compile(r'//[^\n]*|/\*.*?\*/|L?"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'', re.S)


def strip_comments(text):
    """Blank // and /* */ comments, keep every newline (line numbers stay valid), leave strings alone."""
    def sub(m):
        s = m.group(0)
        if s.startswith('//') or s.startswith('/*'):
            return re.sub(r'[^\n]', ' ', s)
        return s
    return _CPP_TOK.sub(sub, text)


def parse_int(s):
    s = s.strip().rstrip('uUlL')
    return int(s, 16) if s[:2].lower() == '0x' else int(s)


def eval_expr(expr, env):
    """Evaluate an enumerator / const initialiser: numbers, known names, + - * / << >> | & ( ).  None if unknown."""
    expr = re.sub(r'(?:CX2SkillTree\s*::\s*)', '', expr)
    def name(m):
        n = m.group(0)
        return str(env[n]) if n in env else m.group(0)
    e = re.sub(r'\b[A-Za-z_]\w*\b', name, expr)
    e = re.sub(r'\b(0[xX][0-9a-fA-F]+|\d+)[uUlL]+\b', r'\1', e)
    if not re.fullmatch(r'[\s\d()+\-*/<>|&xXa-fA-F~%^]*', e) or re.search(r'[g-zG-Z_]', e):
        return None
    try:
        return int(eval(e.replace('/', '//'), {'__builtins__': {}}, {}))
    except Exception:
        return None


# ---------------------------------------------------------------- mini preprocessor (enum region, textual mode)
def crude_defines():
    out = set()
    for rel in DEFINE_FILES:
        p = os.path.join(TRUNK, rel)
        if not os.path.exists(p):
            continue
        for line in strip_comments(read_latin1(p)).split('\n'):
            m = re.match(r'\s*#\s*define\s+([A-Za-z_]\w*)', line)
            if m:
                out.add(m.group(1))
    return out


def eval_cond(kind, rest, defs):
    if kind == 'ifdef':
        return rest.split()[0] in defs
    if kind == 'ifndef':
        return rest.split()[0] not in defs
    e = re.sub(r'defined\s*\(\s*(\w+)\s*\)|defined\s+(\w+)', lambda m: '1' if (m.group(1) or m.group(2)) in defs else '0', rest)
    e = re.sub(r'\b[A-Za-z_]\w*\b', lambda m: '1' if m.group(0) in defs else '0', e)
    e = e.replace('&&', ' and ').replace('||', ' or ').replace('!', ' not ')
    try:
        return bool(eval(e, {'__builtins__': {}}, {}))
    except Exception:
        return True


def live_text(lines, defs):
    """Apply #if/#ifdef/#ifndef/#elif/#else/#endif to a list of lines; return the live lines (directives dropped)."""
    out, stack = [], []        # stack entries: [parent_live, taken, live]
    live = True
    for ln in lines:
        m = re.match(r'\s*#\s*(ifdef|ifndef|if|elif|else|endif)\b(.*)', ln)
        if not m:
            if live:
                out.append(ln)
            continue
        kind, rest = m.group(1), m.group(2).strip()
        if kind in ('if', 'ifdef', 'ifndef'):
            c = eval_cond(kind, rest, defs)
            stack.append([live, c])
            live = live and c
        elif kind == 'elif':
            par, taken = stack[-1]
            c = (not taken) and eval_cond('if', rest, defs)
            stack[-1][1] = taken or c
            live = par and c
        elif kind == 'else':
            par, taken = stack[-1]
            live = par and not taken
            stack[-1][1] = True
        elif kind == 'endif':
            par, _ = stack.pop()
            live = par
    return out


def enum_body(text):
    """Text between the braces of the `enum SKILL_ID` that holds SI_NONE (comments already stripped)."""
    for m in re.finditer(r'\benum\s+SKILL_ID\s*\{', text):
        end = text.find('}', m.end())
        if end > 0 and 'SI_NONE' in text[m.end():end]:
            return text[m.end():end]
    return None


def parse_enum(body_lines):
    """-> ordered {name: value}, list of unresolved names.  C++ rule: no initialiser = previous + 1."""
    items = re.sub(r'#\s*pragma[^\n]*', '', '\n'.join(body_lines)).split(',')
    vals, unresolved, nxt = {}, [], 0
    for it in items:
        it = it.strip()
        if not it:
            continue
        m = re.match(r'([A-Za-z_]\w*)\s*(?:=\s*(.+))?$', it, re.S)
        if not m:
            continue
        name, expr = m.group(1), m.group(2)
        if expr is not None:
            v = eval_expr(expr.strip(), vals)
            if v is None:
                unresolved.append(name)
                v = nxt
        else:
            v = nxt
        vals[name] = v
        nxt = v + 1
    return vals, unresolved


def load_enum_textual():
    text = strip_comments(read_latin1(SKILL_TREE_H))
    m = re.search(r'\benum\s+SKILL_ID\s*\{', text)
    start = m.end()
    # braces never occur inside the enum body: the first `}` ends it, even across #if branches
    body = text[start:text.find('}', start)]
    return parse_enum(live_text(body.split('\n'), crude_defines()))


# ---------------------------------------------------------------- --preprocess (cl /P)
def run_preprocess(cpp_path, out_path):
    sys.path.insert(0, HERE)
    import census as C
    cmd = C.cl_cmd(cpp_path, out_path)
    r = subprocess.run(cmd, cwd=X2LIB, env=C.cl_env(), capture_output=True)
    if r.returncode != 0 or not os.path.exists(out_path):
        sys.exit('cl /P failed (exit %s):\n%s' % (r.returncode, r.stdout.decode('latin1', 'replace')[-3000:]))


def split_preprocessed(path, wanted):
    """-> (enum text of the whole stream, {basename: text with line numbers of the original file kept})."""
    raw = read_latin1(path)
    seg_re = re.compile(r'^#line\s+(\d+)\s+"([^"]*)"\s*$', re.M)
    marks = list(seg_re.finditer(raw))
    files = {}
    for i, m in enumerate(marks):
        end = marks[i + 1].start() if i + 1 < len(marks) else len(raw)
        base = os.path.basename(m.group(2).replace('\\\\', '\\')).lower()
        if base not in wanted:
            continue
        # the preprocessor drops blank / directive lines; line numbers are therefore approximate: start + offset
        files.setdefault(base, []).append((int(m.group(1)), raw[m.end() + 1:end]))
    return raw, files


# ---------------------------------------------------------------- scanning a class family
CASE_RE = re.compile(r'\bcase\s+((?:[A-Za-z_]\w*\s*::\s*)*[A-Za-z_]\w*|0[xX][0-9a-fA-F]+|\d+)\s*:(?!:)')
CONST_RE = re.compile(r'\bconst\s+(?:unsigned\s+)?int\s+([A-Za-z_]\w*)\s*=\s*(0[xX][0-9a-fA-F]+|\d+)\s*;')
CMP_RE = re.compile(r'(?:[=!]=\s*(0[xX][0-9a-fA-F]+|\d+)\b)|(?:\b(0[xX][0-9a-fA-F]+|\d+)\s*[=!]=)')


class Source:
    """One scanned file: comment-stripped text, a line-start index, and an optional live-range filter."""
    def __init__(self, name, text, live=None, line_base=None):
        self.name = name
        self.text = strip_comments(text)
        self.starts = [0]
        for m in re.finditer(r'\n', self.text):
            self.starts.append(m.end())
        self.live = live
        self.line_base = line_base      # function(pos) -> reported line, for --preprocess

    def line(self, pos):
        if self.line_base:
            return self.line_base(pos)
        return bisect.bisect_right(self.starts, pos)

    def ok(self, pos):
        if not self.live:
            return True
        ln = self.line(pos)
        return any(a <= ln <= b for a, b in self.live)


def scan_sources(srcs, enum):
    """-> consts {name: value}, cases {value: [(file, line)]}, unresolved case names, cmps {value: [...]}, consts_at {value: [...]}"""
    consts, consts_at = {}, {}
    for s in srcs:
        for m in CONST_RE.finditer(s.text):
            if s.ok(m.start()):
                v = parse_int(m.group(2))
                consts[m.group(1)] = v
                consts_at.setdefault(v, []).append((s.name, s.line(m.start())))
    cases, unresolved, cmps = {}, set(), {}
    for s in srcs:
        for m in CASE_RE.finditer(s.text):
            if not s.ok(m.start()):
                continue
            tok = m.group(1)
            if re.fullmatch(r'0[xX][0-9a-fA-F]+|\d+', tok):
                v = parse_int(tok)
            else:
                base = re.split(r'\s*::\s*', tok)[-1]
                if base in consts:
                    v = consts[base]
                elif base in enum and (tok == base or tok.endswith('SKILL_ID::' + base) or re.match(r'CX2SkillTree\s*::', tok)):
                    v = enum[base]
                else:
                    unresolved.add(tok)
                    continue
            cases.setdefault(v, []).append((s.name, s.line(m.start())))
        for m in CMP_RE.finditer(s.text):
            if s.ok(m.start()):
                v = parse_int(m.group(1) or m.group(2))
                cmps.setdefault(v, []).append((s.name, s.line(m.start())))
    return consts, cases, unresolved, cmps, consts_at


def token_present(name, srcs):
    pat = re.compile(r'\b%s\b' % re.escape(name))
    return any(pat.search(s.text) for s in srcs)


def parse_live_lines(path):
    """-> {file lower or '*': [(a, b)]}"""
    out = {}
    for ln in open(path, encoding='utf-8', errors='replace'):
        ln = ln.split('#')[0].strip()
        if not ln:
            continue
        m = re.fullmatch(r'(?:(.+?):)?\s*(\d+)\s*-\s*(\d+)', ln)
        if not m:
            sys.exit('bad --live-lines row: %r' % ln)
        out.setdefault((m.group(1) or '*').lower(), []).append((int(m.group(2)), int(m.group(3))))
    return out


def lua_table(text, name):
    """-> {key: int} for the top-level `NAME = { KEY = int, ... }` table of Enum.src.lua."""
    m = re.search(r'^%s = \{\n(.*?)^\}' % re.escape(name), text, re.S | re.M)
    return {k: int(v) for k, v in re.findall(r'^\t(\w+) = (-?\d+),?\s*$', m.group(1), re.M)}


def june_only_rows(fam, only=None):
    """Tree rows (columns 1-10 of the tsv) for a June-only family (or the given classes) from June's Lua source."""
    enum = read_latin1(os.path.join(JUNE_LUA_DIR, 'Enum.src.lua'))
    classes = lua_table(enum, 'UNIT_CLASS')
    names = {}
    for n, v in lua_table(enum, 'SKILL_ID').items():
        names.setdefault(v, []).append(n)
    src = read_latin1(os.path.join(JUNE_LUA_DIR, 'NewSkillTempletVer2.src.lua'))
    blocks = src.split('g_pSkillTree:AddSkill')
    templets, tree = {}, {}
    for b in blocks[1:]:
        if b.startswith('Templet_LUA'):
            i = re.search(r'^\tm_eID = (\d+)', b, re.M)
            if i:
                nm = re.search(r'^\tm_wstrName = "(.*)",\s*$', b, re.M)
                ty = re.search(r'^\tm_eType = (\d+)', b, re.M)
                templets[int(i.group(1))] = (nm.group(1) if nm else '', ty.group(1) if ty else '')
        elif b.startswith('TreeTemplet_LUA'):
            g = dict(re.findall(r'^\t(m_\w+) = ([\w.]+)', b, re.M))
            uc = g['m_eUnitClass'].split('.')[-1]
            tree.setdefault(uc, []).append(g)
    rows = []
    for uc in (only or JUNE_ONLY_FAMILIES[fam]):
        for g in tree.get(uc, []):
            sid = int(g['m_iSkillID'])
            cand = names.get(sid, [])
            nm = ([n for n in cand if n.startswith('RNSI_')] or cand or ['not found'])[0]
            tn, ty = templets.get(sid, ('', ''))
            rows.append({'unit_class_name': uc, 'unit_class_value': str(classes[uc]), 'family_cpp_file': fam,
                         'skill_id': str(sid), 'skill_name': nm, 'templet_name': tn, 'skill_type': ty,
                         'master_level': g['m_iMasterLevel'], 'row': g['m_iRowIndex'], 'column': g['m_iColumnIndex']})
    return rows


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--family', required=True, help='tsv family_cpp_file value, e.g. X2GuEve.cpp')
    ap.add_argument('--class-file', action='append', help='class source (repeatable); default X2Lib/<family> and its .h')
    ap.add_argument('--tsv', default=DEFAULT_TSV, help='tree rows (default s3_reform_ids_no_state.tsv)')
    ap.add_argument('--live-lines', help='file of live line ranges (start-end or file:start-end per line)')
    ap.add_argument('--preprocess', action='store_true', help='use cl /P for exact live branches')
    ap.add_argument('--loose', action='store_true', help='const definitions and == compares also count as has_state')
    ap.add_argument('--append-missing', action='store_true', help='append the TSV_MISSING_CLASSES rows of --family to --tsv and exit')
    ap.add_argument('-o', '--out', help='write here instead of stdout')
    ap.add_argument('--keep-i', help='with --preprocess: where to leave the .i file (default: next to -o or temp)')
    a = ap.parse_args()

    fam = a.family
    if a.append_missing:
        add = june_only_rows(fam, TSV_MISSING_CLASSES[fam])
        with open(a.tsv, encoding='utf-8', newline='') as fh:
            have = {r.split('\t')[0] for r in fh.read().split('\n')[1:] if r}
        add = [d for d in add if d['unit_class_name'] not in have]
        with open(a.tsv, 'a', encoding='utf-8', newline='\n') as fh:
            for d in add:
                d = dict(d, in_cpp_enum='n', cpp_case_in_family='n', verdict='no_state', cpp_any_token_in_family='n', case_in_X2GUUser_cpp='n')
                fh.write('\t'.join(d.get(c, '') for c in COLS) + '\n')
        print('appended %d rows' % len(add))
        return
    base_cpp = os.path.join(X2LIB, fam)
    files = a.class_file or [base_cpp, os.path.splitext(base_cpp)[0] + '.h']
    files = [f if os.path.isabs(f) else os.path.join(TRUNK, f) for f in files]
    if fam in JUNE_ONLY_FAMILIES and not a.class_file:
        files = [f for f in files if os.path.exists(f)]      # no March class yet: nothing to scan
    for f in files:
        if not os.path.exists(f):
            sys.exit('class file not found: %s' % f)
    live = parse_live_lines(a.live_lines) if a.live_lines else None

    # tree rows
    rows = []
    with open(a.tsv, encoding='utf-8', errors='replace', newline='') as fh:
        rd = csv.reader(fh, delimiter='\t')
        hdr = next(rd)
        idx = {h: i for i, h in enumerate(hdr)}
        for r in rd:
            if r and not r[0].startswith('#') and r[idx['family_cpp_file']].lower() == fam.lower():
                rows.append(dict(zip(hdr, r)))
    if not rows and fam in JUNE_ONLY_FAMILIES:
        rows = june_only_rows(fam)
    if not rows:
        sys.exit('no rows for family %s in %s (add the family to JUNE_ONLY_FAMILIES or its tree rows to the tsv)' % (fam, a.tsv))

    notes = []
    if a.preprocess:
        out_i = a.keep_i or os.path.join(os.environ.get('TEMP', HERE), 's3_no_state_%s.i' % os.path.splitext(fam)[0])
        run_preprocess(base_cpp, out_i)
        raw, segs = split_preprocessed(out_i, {os.path.basename(f).lower() for f in files})
        enum_txt = enum_body(strip_comments(raw))
        if enum_txt is None:
            sys.exit('SKILL_ID enum not found in %s' % out_i)
        enum, unres = parse_enum(enum_txt.split('\n'))
        srcs = []
        for f in files:
            parts = segs.get(os.path.basename(f).lower(), [])
            text = '\n'.join(p[1] for p in parts)
            srcs.append(Source(os.path.basename(f), text))
        notes.append('mode: cl /P (live branches exact; line numbers are positions in the preprocessed stream)')
    else:
        enum, unres = load_enum_textual()
        srcs = []
        for f in files:
            lv = None
            if live:
                lv = live.get(os.path.basename(f).lower(), []) + live.get('*', [])
            srcs.append(Source(os.path.basename(f), read_latin1(f), lv))
        if live:
            notes.append('mode: textual, restricted to --live-lines ranges')
        else:
            notes.append('mode: textual, every occurrence counts (no live/dead #ifdef resolution in the class files)')
    notes.append('enum SKILL_ID: %d enumerators, %d unresolved initialisers%s' % (len(enum), len(unres), (' (%s)' % ', '.join(unres[:5])) if unres else ''))

    consts, cases, unres_case, cmps, consts_at = scan_sources(srcs, enum)
    user_src = [Source('X2GUUser.cpp', read_latin1(USER_CPP))]
    _, user_cases, _, _, _ = scan_sources(user_src, enum)
    by_val = {}
    for n, v in enum.items():
        by_val.setdefault(v, []).append(n)
    notes.append('verdict: %s' % ('has_state = case OR const OR cmp evidence (--loose)' if a.loose else 'has_state = a case label resolves to the id (strict)'))
    if unres_case:
        notes.append('case labels not resolved (not in enum / consts): %d' % len(unres_case))

    out_rows, n_has = [], 0
    for d in rows:
        sid = parse_int(d['skill_id'])
        name = d['skill_name']
        ev = []
        for kind, src in (('case', cases), ('const', consts_at), ('cmp', cmps)):
            for f, ln in src.get(sid, [])[:3]:
                ev.append('%s@%s:%d' % (kind, f, ln))
        has = sid in cases or (a.loose and (sid in consts_at or sid in cmps))
        n_has += has
        look = name.replace('RNSI_', 'SI_', 1) if name.startswith('RNSI_') else ''
        look = look if look in enum else ''
        d = dict(d)
        d['in_cpp_enum'] = 'y' if name in enum else 'n'
        d['cpp_case_in_family'] = 'y' if sid in cases else 'n'
        d['verdict'] = 'has_state' if has else 'no_state'
        d['march_value_of_name'] = str(enum[name]) if name in enum else ''
        d['march_names_at_same_id'] = '|'.join(by_val.get(sid, []))
        d['cpp_any_token_in_family'] = 'y' if token_present(name, srcs) else 'n'
        d['case_in_X2GUUser_cpp'] = 'y' if sid in user_cases else 'n'
        d['lookalike_march_name'] = look
        d['lookalike_march_value'] = str(enum[look]) if look else ''
        d['lookalike_case_in_family'] = ('y' if enum[look] in cases else 'n') if look else ''
        d['evidence'] = ';'.join(ev)
        out_rows.append([d.get(c, '') for c in OUT_COLS])

    lines = ['# s3_no_state.py --family %s%s' % (fam, ' --loose' if a.loose else '')]
    lines += ['# ' + n for n in notes]
    lines.append('\t'.join(OUT_COLS))
    lines += ['\t'.join(r) for r in out_rows]
    lines.append('# totals\trows=%d\tno_state=%d\thas_state=%d' % (len(out_rows), len(out_rows) - n_has, n_has))
    text = '\n'.join(lines) + '\n'
    if a.out:
        with open(a.out, 'w', encoding='utf-8', newline='\n') as fh:
            fh.write(text)
        print(lines[-1])
    else:
        sys.stdout.write(text)


if __name__ == '__main__':
    main()
