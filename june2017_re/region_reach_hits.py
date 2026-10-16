#!/usr/bin/env python
"""Atlas A1a - mechanical inventory of March C++ hits for the Sander/Ranox region tokens.

    python -I region_reach_hits.py   # writes region_reach_hits.tsv next to this script; report to stdout

Read-only against the repo.  The only repo write is the flag cache june2017_re/region_census_flags.tsv
(appended by RC.prime_flags when it probes flags it has never seen).  Source bytes are read as latin-1.

Live/dead comes from our own directive walker (a line-for-line copy of buff_census.active_text's evaluation, plus a parallel
label stack for guard_chain), CROSS-CHECKED against RC.active() per file.  `state` is taken from the walker, not from
"is the active line blank", because active_text blanks every directive line (#define, #pragma) and so would call a token on a
#define line dead.
"""
import os, re, sys, random, collections
import xml.etree.ElementTree as ET

TR = r"F:\elsword stuff\elsword_2014\els_2014\ElswordFiles\ready_and_built\ProjectX2_SVN\ProjectX2_SVN\source\EU_CN_US\Trunk"
SCRATCH = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(TR, 'june2017_re'))
sys.path.insert(0, os.path.join(TR, 'tools'))
os.chdir(os.path.join(TR, 'june2017_re'))
import buff_census as BC          # noqa: E402
import region_census as RC        # noqa: E402

DIRS = ['X2Lib', 'KTDXLIB', 'X2ServerProtocol', 'X2', 'KncWX2Server/Common']
EXTS = ('.cpp', '.h', '.inl')
PROJECTS = {  # name -> (vcxproj relative to Trunk, configuration that matters)
    'X2Lib': ('X2Lib/X2Lib_2010.vcxproj', 'US_SERVICE|Win32'),
    'KTDXLIB': ('KTDXLIB/KTDXLIB_2010.vcxproj', 'US_SERVICE|Win32'),
    'X2ServerProtocol': ('X2ServerProtocol/X2ServerProtocol_2010.vcxproj', 'X2TOOL|Win32'),
    'X2': ('X2/X2_2010.vcxproj', 'US_SERVICE|Win32'),
}

G1 = [r'VMI_SANDER\w*', r'VMI_RANOX\w*', r'VMI_BATTLE_FIELD_SANDER_\w*', r'VMI_BATTLE_FIELD_RANOX_\w*',
      r'LMI_SANDER\w*', r'LMI_RANOX\w*', r'WI_\w*SANDER\w*', r'WI_\w*RANOX\w*',
      r'VMI_BATTLE_FIELD_REST_END', r'VMI_VILLAGE_MAP_END', r'VMI_BATTLE_FIELD_END', r'WI_END', r'NUI_MAX_LIMIT', r'DI_END']
G2 = [r'HI_\w*SANDER\w*', r'HI_\w*RANOX\w*', r'DI_\w*SANDER\w*', r'DI_\w*RANOX\w*']
G1_RE = [re.compile(p, re.A) for p in G1]
G2_RE = [re.compile(p, re.A) for p in G2]
IDENT = re.compile(r'[A-Za-z_]\w*', re.A)
PREFILTER = re.compile(rb'(?:VMI_|LMI_|WI_|NUI_MAX_LIMIT|DI_|HI_)')


def token_group(ident):
    if any(r.fullmatch(ident) for r in G1_RE):
        return 1
    if any(r.fullmatch(ident) for r in G2_RE):
        return 2
    return 0


# ---------------------------------------------------------------------------------------------------- lexer
def lex(text):
    """-> (raw_lines, region_lines).  region char per raw char: C code, M comment, S string/char literal (quotes included)."""
    raw_lines = text.split('\n')
    regs = []
    state = 'N'               # N normal, B block comment, L line comment continued by a backslash, S/Q string/char continued
    for line in raw_lines:
        n = len(line)
        reg = ['C'] * n
        i = 0
        if state == 'L':
            for k in range(n):
                reg[k] = 'M'
            state = 'L' if line.rstrip('\r').endswith('\\') else 'N'
            regs.append(''.join(reg))
            continue
        while i < n:
            c = line[i]
            if state == 'B':
                reg[i] = 'M'
                if c == '*' and i + 1 < n and line[i + 1] == '/':
                    reg[i + 1] = 'M'
                    i += 2
                    state = 'N'
                    continue
                i += 1
                continue
            if state in ('S', 'Q'):
                q = '"' if state == 'S' else "'"
                reg[i] = 'S'
                if c == '\\' and i + 1 < n:
                    reg[i + 1] = 'S'
                    i += 2
                    continue
                if c == q:
                    state = 'N'
                i += 1
                continue
            # normal
            if c == '/' and i + 1 < n and line[i + 1] == '/':
                for k in range(i, n):
                    reg[k] = 'M'
                state = 'L' if line.rstrip('\r').endswith('\\') else 'N'
                i = n
                break
            if c == '/' and i + 1 < n and line[i + 1] == '*':
                reg[i] = reg[i + 1] = 'M'
                i += 2
                state = 'B'
                continue
            if c == '"' or c == "'":
                reg[i] = 'S'
                state = 'S' if c == '"' else 'Q'
                i += 1
                continue
            i += 1
        # strings do not span lines unless the line ends with a backslash
        if state in ('S', 'Q') and not line.rstrip('\r').endswith('\\'):
            state = 'N'
        regs.append(''.join(reg))
    return raw_lines, regs


def masked(raw_lines, regs, keep):
    """Copy of the lines where every char whose region is not in `keep` is a space."""
    out = []
    for l, r in zip(raw_lines, regs):
        out.append(''.join(ch if rg in keep else ' ' for ch, rg in zip(l, r)))
    return out


# ---------------------------------------------------------------------------------------------------- directive walker
_DIR = re.compile(r'^\s*#\s*(ifdef|ifndef|if|elif|else|endif|define|undef|include|pragma)\b(.*)$')


def norm_expr(e):
    return re.sub(r'\s+', ' ', e.strip())


def is_approx(expr):
    """True when the #if expression has anything but defined-ness: a number other than a lone 0/1, or a relational operator."""
    e = re.sub(r'defined\s*\(\s*\w+\s*\)', ' ', expr)
    e = re.sub(r'defined\s+\w+', ' ', e)
    e = re.sub(r'\b[A-Za-z_]\w*\b', ' ', e)
    e = e.replace('&&', ' ').replace('||', ' ').replace('!', ' ').replace('(', ' ').replace(')', ' ')
    rest = e.split()
    return any(t not in ('0', '1') for t in rest)


class Walk(object):
    def __init__(self, nc_lines, cache, unknown_sink):
        self.cache = cache
        self.unknown = unknown_sink
        self.frames = []
        self.chain = []        # per line: tuple of (frame id, branch idx)
        self.is_dir = []
        self.guard_frame = None
        self.walk(nc_lines)

    # -- same evaluation as buff_census.active_text
    def isdef(self, f):
        if f not in self.cache:
            self.unknown.add(f)
        return self.cache.get(f, False)

    def ev(self, expr):
        e = expr.strip()
        e = re.sub(r'defined\s*\(\s*(\w+)\s*\)', lambda m: '1' if self.isdef(m.group(1)) else '0', e)
        e = re.sub(r'defined\s+(\w+)', lambda m: '1' if self.isdef(m.group(1)) else '0', e)
        e = re.sub(r'\b[A-Za-z_]\w*\b', lambda m: '1' if self.isdef(m.group(0)) else '0', e)
        e = e.replace('&&', ' and ').replace('||', ' or ').replace('!', ' not ')
        try:
            return bool(eval(e, {}, {}))
        except Exception:
            self.unknown.add('#if ' + expr.strip())
            return False

    def fstate(self, f):
        c = self.cache.get(f)
        return 'UNK' if c is None else ('ON' if c else 'OFF')

    def label(self, kind, rest):
        rest = rest.strip()
        if kind in ('ifdef', 'ifndef'):
            f = rest.split()[0] if rest.split() else '?'
            return '%s %s=%s' % (kind, f, self.fstate(f)), [f], False
        e = norm_expr(rest)
        ids = [x for x in IDENT.findall(e) if x != 'defined']
        approx = is_approx(e)
        st = ','.join('%s=%s' % (x, self.fstate(x)) for x in dict.fromkeys(ids))
        if approx:
            return '%s ?%s' % (kind, e[:90]), ids, True
        return '%s %s%s' % (kind, e[:90], '[%s]' % st if st else ''), ids, False

    def cur_chain(self):
        return tuple((f['id'], len(f['branches']) - 1) for f in self.stack)

    def walk(self, nc):
        self.stack = []
        cont = False
        first_dirs = []          # (line idx, directive, rest) of the first two non-pragma directives
        for idx, l in enumerate(nc):
            self.chain.append(self.cur_chain())
            if cont:
                self.is_dir.append(True)
                cont = l.rstrip().endswith('\\')
                continue
            m = _DIR.match(l)
            if not m:
                self.is_dir.append(False)
                continue
            self.is_dir.append(True)
            d, rest = m.groups()
            if d != 'pragma' and len(first_dirs) < 2:
                first_dirs.append((idx, d, rest.strip()))
            if d in ('ifdef', 'ifndef', 'if'):
                lab, ids, approx = self.label(d, rest)
                v = (self.isdef(rest.split()[0]) if rest.split() else False) if d == 'ifdef' else \
                    ((not self.isdef(rest.split()[0])) if rest.split() else False) if d == 'ifndef' else self.ev(rest)
                f = dict(id=len(self.frames), kind=d, line=idx, branches=[dict(label=lab, flags=ids, approx=approx, kind=d)],
                         taken=bool(v), selected=0 if v else None, uncertain=approx, guard=False)
                self.frames.append(f)
                self.stack.append(f)
            elif d == 'elif':
                if not self.stack:
                    continue
                f = self.stack[-1]
                lab, ids, approx = self.label('elif', rest)
                v = (not f['taken']) and self.ev(rest)
                f['branches'].append(dict(label=lab, flags=ids, approx=approx, kind='elif'))
                f['uncertain'] = f['uncertain'] or approx
                if v:
                    f['selected'] = len(f['branches']) - 1
                f['taken'] = f['taken'] or v
            elif d == 'else':
                if not self.stack:
                    continue
                f = self.stack[-1]
                prior = ';'.join(b['label'] for b in f['branches'])
                f['branches'].append(dict(label='else(%s)' % prior, flags=[x for b in f['branches'] for x in b['flags']],
                                          approx=False, kind='else'))
                if not f['taken']:
                    f['selected'] = len(f['branches']) - 1
                f['taken'] = True
            elif d == 'endif':
                if self.stack:
                    self.stack.pop()
            elif d in ('define', 'undef') and l.rstrip().endswith('\\'):
                cont = True
        # include guard: first directive is #ifndef G and the next one #define G
        if len(first_dirs) == 2 and first_dirs[0][1] == 'ifndef' and first_dirs[1][1] == 'define':
            g1 = first_dirs[0][2].split()[0] if first_dirs[0][2].split() else None
            g2 = first_dirs[1][2].split()[0] if first_dirs[1][2].split() else None
            if g1 and g1 == g2:
                for f in self.frames:
                    if f['line'] == first_dirs[0][0]:
                        f['guard'] = True
                        f['selected'] = 0
                        f['uncertain'] = False
                        f['branches'][0]['label'] = 'guard(%s)' % g1
                        self.guard_frame = f
                        break

    def selected(self, fid, forced):
        if forced and fid in forced:
            return forced[fid]
        return self.frames[fid]['selected']

    def line_alive(self, idx, forced=None):
        return all(self.selected(fid, forced) == b for fid, b in self.chain[idx])

    def state(self, idx):
        """live / dead / ? (approximated #if decides)."""
        ch = self.chain[idx]
        uncertain = False
        for fid, b in ch:
            f = self.frames[fid]
            if f['selected'] != b:
                if not f['uncertain']:
                    return 'dead'
                uncertain = True
            elif f['uncertain']:
                uncertain = True
        return '?' if uncertain else 'live'


# ---------------------------------------------------------------------------------------------------- scope parser
class Scope(object):
    NOT_FUNC_NAMES = {'if', 'for', 'while', 'switch', 'catch', 'sizeof', 'throw', 'noexcept', '__declspec', '__attribute__',
                      'return', 'defined'}
    MACRO_LIKE = re.compile(r'[A-Z][A-Z0-9_]+')

    def __init__(self):
        self.stack = []          # [kind, name]
        self.buf = ''
        self.fdepth = 0

    def copy(self):
        s = Scope()
        s.stack = [list(e) for e in self.stack]
        s.buf = self.buf
        s.fdepth = self.fdepth
        return s

    def qual(self):
        return '::'.join(n for k, n in self.stack if k == 'class' and n)

    def classify(self, s):
        s = re.sub(r'\s+', ' ', s).strip()
        s = re.sub(r'\b(public|private|protected)\s*:', ' ', s).strip()
        if re.search(r'\bnamespace\b', s) or re.match(r'extern\s*"?\s*C"?\s*$', s) or s.replace(' ', '') in ('extern"C"', 'extern""'):
            return 'ns', ''
        m = re.search(r'\benum\b(?:\s+(?:class|struct))?\s*(\w+)?', s)
        if m:
            return 'enum', m.group(1) or ''
        m = re.search(r'\b(class|struct|union)\s+(?:__declspec\s*\([^)]*\)\s*)?(\w+)', s)
        if m and '(' not in s[:m.start()] and not re.search(r'\)\s*(const|volatile)?\s*$', s):
            return 'class', m.group(2)
        if s.endswith('=') or s.endswith(',') or not s:
            return 'block', ''
        if '(' in s:
            return 'func', self.fname(s)
        return 'block', ''

    def fname(self, s):
        groups = []
        depth = 0
        start = None
        for i, ch in enumerate(s):
            if ch == '(':
                if depth == 0:
                    start = i
                depth += 1
            elif ch == ')':
                if depth > 0:
                    depth -= 1
                    if depth == 0 and start is not None:
                        groups.append((start, i))
        cands = []
        for a, b in groups:
            pre = s[:a]
            m = re.search(r'((?:~?\w+(?:\s*<[^<>]*>)?\s*::\s*)*(?:operator\s*(?:\(\s*\)|[^\s\w(]+)|~?\w+))\s*$', pre)
            if not m:
                continue
            nm = re.sub(r'\s+', '', m.group(1))
            last = nm.split('::')[-1]
            if last in self.NOT_FUNC_NAMES:
                continue
            cands.append((nm, last, s[a + 1:b].strip()))
        if not cands:
            return '?'
        for nm, last, arg in cands:
            if not self.MACRO_LIKE.fullmatch(last):
                return self.qualify(nm)
        nm, last, arg = cands[0]
        return '%s(%s)' % (nm, re.sub(r'\s+', ' ', arg)[:60])

    def qualify(self, nm):
        if '::' in nm:
            return nm
        q = self.qual()
        return (q + '::' + nm) if q else nm

    def feed(self, s):
        pos = 0
        for m in re.finditer(r'[{};]', s):
            seg = s[pos:m.start()]
            if self.fdepth == 0:
                self.buf += seg
            ch = m.group(0)
            if ch == '{':
                if self.fdepth > 0:
                    self.stack.append(['block', ''])
                else:
                    kind, name = self.classify(self.buf)
                    self.stack.append([kind, name])
                    if kind == 'func':
                        self.fdepth += 1
                    self.buf = ''
            elif ch == '}':
                if self.stack:
                    k, n = self.stack.pop()
                    if k == 'func':
                        self.fdepth -= 1
                self.buf = ''
            else:
                if self.fdepth == 0:
                    self.buf = ''
            pos = m.end()
        if self.fdepth == 0:
            self.buf += s[pos:] + ' '

    def innermost(self, kind):
        for k, n in reversed(self.stack):
            if k == kind:
                return n
        return None

    def in_kind(self):
        return self.stack[-1][0] if self.stack else None


def scope_at(walk, ns, forced, idx, col, cache):
    """Scope state at (line idx, col) for the view where the hit's own conditional branches are forced active."""
    key = (idx and 0, tuple(sorted(forced.items())) if forced else ())
    ent = cache.get(key)
    if ent is None:
        ent = {'snaps': {}, 'sc': Scope(), 'next': 0}
        cache[key] = ent
    # advance incrementally up to line idx (lines are queried in ascending order per signature, but be safe)
    if ent['next'] > idx:
        # rebuild from scratch to the target line
        ent = {'snaps': {}, 'sc': Scope(), 'next': 0}
        cache[key] = ent
    sc = ent['sc']
    while ent['next'] < idx:
        i = ent['next']
        if (not walk.is_dir[i]) and walk.line_alive(i, forced):
            sc.feed(ns[i])
        ent['next'] += 1
    snap = sc.copy()
    if (not walk.is_dir[idx]) and walk.line_alive(idx, forced):
        snap.feed(ns[idx][:col])
    return snap


# ---------------------------------------------------------------------------------------------------- vcxproj
def load_projects():
    """-> {normalized lower path: (project, excluded_for_config bool)} for every ClCompile."""
    out = {}
    for name, (rel, cfg) in PROJECTS.items():
        path = os.path.join(TR, rel.replace('/', os.sep))
        tree = ET.parse(path)
        root = tree.getroot()
        for el in root.iter():
            if not el.tag.endswith('ClCompile'):
                continue
            inc = el.get('Include')
            if not inc:
                continue
            excl = False
            for ch in el:
                if ch.tag.endswith('ExcludedFromBuild') and (ch.text or '').strip().lower() == 'true':
                    cond = ch.get('Condition')
                    if cond is None or ("'%s'" % cfg) in cond.replace(' ', '') or cfg in cond:
                        excl = True
            full = os.path.normpath(os.path.join(os.path.dirname(path), inc.replace('\\', os.sep))).lower()
            prev = out.get(full)
            out[full] = (name, excl if prev is None else (excl and prev[1]))
    return out


def defined_names(paths):
    s = set()
    rx = re.compile(rb'^\s*#\s*define\s+(\w+)', re.M)
    for p in paths:
        try:
            s.update(m.decode('latin-1') for m in rx.findall(open(p, 'rb').read()))
        except OSError:
            pass
    return s


def vcx_defs(rel, cfg):
    """Preprocessor definitions of one config in a vcxproj."""
    tree = ET.parse(os.path.join(TR, rel.replace('/', os.sep)))
    names = set()
    for g in tree.getroot().iter():
        if g.tag.endswith('ItemDefinitionGroup'):
            cond = g.get('Condition') or ''
            if cfg in cond:
                for pd in g.iter():
                    if pd.tag.endswith('PreprocessorDefinitions') and pd.text:
                        names.update(x.split('=')[0].strip() for x in pd.text.split(';') if x.strip())
    return names


# ---------------------------------------------------------------------------------------------------- usage
REL = r'(?:==|!=|<=|>=|(?<![-<>=!])<(?![<=])|(?<![-<>=])>(?![>=]))'
REL_BEFORE = re.compile(REL + r'\s*\(?\s*(?:\(\s*[\w:\s\*]+\)\s*)*(?:\w+::)*$')
REL_AFTER = re.compile(r'^\s*\)?\s*' + REL)
DECL_ARR = re.compile(r'^\s*(?:(?:static|const|extern|volatile)\s+)*[\w:<>\*&,]+(?:\s*[\*&]+\s*|\s+)\w+\s*\[\s*(?:[\w:]*::)?$')


def usage_of(ns, pos, ident, region):
    if region == 'S':
        return 'string'
    before, after = ns[:pos], ns[pos + len(ident):]
    if re.search(r'\bcase\s+(?:\w+::)*$', before):
        return 'case'
    if re.search(r'\[\s*(?:\w+::)*$', before) and re.match(r'\s*\]', after):
        return 'array-size' if DECL_ARR.search(before) else 'index'
    rel = bool(REL_BEFORE.search(before) or REL_AFTER.search(after))
    if rel:
        if re.search(r'\bfor\s*\(', before):
            return 'loop-bound'
        return 'compare'
    if re.match(r'\s*(?:\}\s*)?(?:else\s+)?(?:if|while|switch)\b', ns):
        return 'if'
    return 'other'


# ---------------------------------------------------------------------------------------------------- main
def main():
    projects = load_projects()
    # collect files with hits
    cand = []
    for d in DIRS:
        for root, _, fs in os.walk(os.path.join(TR, d.replace('/', os.sep))):
            for f in fs:
                if not f.lower().endswith(EXTS):
                    continue
                p = os.path.join(root, f)
                try:
                    b = open(p, 'rb').read()
                except OSError:
                    continue
                if PREFILTER.search(b):
                    cand.append((d, p, b))
    cand.sort(key=lambda t: (DIRS.index(t[0]), os.path.relpath(t[1], TR).lower()))

    # which candidate files actually have a group-1/2 identifier
    files = []
    excluded_cpp = collections.Counter()
    for d, p, b in cand:
        txt = b.decode('latin-1')
        if not any(token_group(m.group(0)) for m in IDENT.finditer(txt)):
            continue
        rel = os.path.relpath(p, TR).replace('\\', '/')
        low = p.lower()
        if d == 'KncWX2Server/Common' and low.endswith('.cpp'):
            ent = projects.get(os.path.normpath(p).lower())
            if ent is None or ent[1]:
                n = sum(1 for m in IDENT.finditer(txt) if token_group(m.group(0)))
                excluded_cpp[rel] = n
                continue
        files.append((d, p, rel, txt))

    paths = [f[1] for f in files]
    print('files with hits:', len(files), 'excluded server .cpp:', dict(excluded_cpp))
    n0 = len(BC.load_flag_cache())
    RC.prime_flags(paths)                       # one cl /P probe for every unknown flag of the hit files
    cache = BC.load_flag_cache()
    print('flag cache size before/after prime:', n0, len(cache))

    kt_defs = defined_names([os.path.join(r, f) for r, _, fs in os.walk(os.path.join(TR, 'KTDXLIB')) for f in fs if f.lower().endswith('.h')])
    kt_defs |= vcx_defs(*PROJECTS['KTDXLIB'])

    rows = []
    report = collections.OrderedDict(linecount_mismatch=[], crosscheck=[], guard_files=[], unknown_post=collections.OrderedDict(),
                                     unknown_chain=collections.OrderedDict(), approx_exprs=collections.Counter(),
                                     strip_vs_lexer=[], dead_func_whatif=0)
    for d, p, rel, txt in files:
        raw_lines, regs = lex(txt)
        nc = masked(raw_lines, regs, 'CS')          # comments blanked, strings kept (for directives)
        ns = masked(raw_lines, regs, 'C')           # comments and strings blanked (for tokens / braces)
        unk_walk = set()
        walk = Walk(nc, cache, unk_walk)
        # --- evaluator cross-check
        act_text, unk_rc = RC.active(p, probe=False)
        act = act_text.split('\n')
        if len(act) != len(raw_lines):
            report['linecount_mismatch'].append((rel, len(raw_lines), len(act)))
        if unk_rc:
            report['unknown_post'][rel] = sorted(unk_rc)
        if walk.guard_frame is not None:
            report['guard_files'].append((rel, walk.guard_frame['branches'][0]['label']))
        mism = []
        if len(act) == len(raw_lines):
            for i in range(len(raw_lines)):
                if walk.is_dir[i] or not nc[i].strip():
                    continue
                wa = walk.line_alive(i)
                ra = bool(act[i].strip())
                if wa != ra:
                    mism.append(i + 1)
        if mism:
            report['crosscheck'].append((rel, len(mism), mism[:6]))
        bc_strip = BC.strip_comments(txt).split('\n')
        scope_cache = {}
        is_h = rel.lower().endswith(('.h', '.inl'))
        proj_name, proj_ok = ('-', '-')
        if not is_h:
            ent = projects.get(os.path.normpath(p).lower())
            proj_ok = 'Y' if (ent is not None and not ent[1]) else 'N'
        for i, raw in enumerate(raw_lines):
            if not PREFILTER.search(raw.encode('latin-1', 'replace')):
                continue
            seen = set()
            for m in IDENT.finditer(raw):
                ident = m.group(0)
                g = token_group(ident)
                if not g:
                    continue
                pos = m.start()
                region = regs[i][pos]
                key = (ident, region)
                if key in seen:
                    continue
                seen.add(key)
                # lexer vs BC.strip_comments disagreement (char literal trap): BC keeps token on the line?
                bc_has = ident in bc_strip[i] if i < len(bc_strip) else False
                if region == 'M' and bc_has:
                    report['strip_vs_lexer'].append((rel, i + 1, ident, 'lexer=comment, BC.strip_comments kept it'))
                elif region != 'M' and not bc_has:
                    report['strip_vs_lexer'].append((rel, i + 1, ident, 'lexer=code/string, BC.strip_comments dropped it'))
                # state
                if region == 'M':
                    state = 'comment'
                else:
                    state = walk.state(i)
                # chain
                labels = []
                for fid, b in walk.chain[i]:
                    f = walk.frames[fid]
                    if f['guard']:
                        continue
                    br = f['branches'][b]
                    lab = br['label']
                    if d == 'KTDXLIB' and not f['guard']:
                        miss = [x for x in br['flags'] if x not in kt_defs and re.match(r'^[A-Za-z_]\w+$', x)]
                        if miss:
                            lab += ' [not in KTDXLIB headers (%s): KTDXLIB sees OFF]' % ','.join(dict.fromkeys(miss))
                    labels.append(lab)
                    if br['approx']:
                        report['approx_exprs'][(rel, br['label'])] += 1
                    for x in br['flags']:
                        if x not in cache and re.match(r'^[A-Za-z_]\w+$', x):
                            report['unknown_chain'].setdefault(x, set()).add(rel)
                chain = ' > '.join(labels)
                # kind
                before = ns[i][:pos]
                after = ns[i][pos + len(ident):]
                lowrel = rel.lower()
                scope = None
                if lowrel == 'x2lib/offline/x2offlineluaenumseed.h':
                    kind = 'seed'
                elif lowrel == 'ktdxlib/always.h':
                    kind = 'flagdef'
                else:
                    kind = 'use'
                    if region == 'C':
                        if re.match(r'^\s*#\s*define\s+' + re.escape(ident) + r'\b', raw):
                            kind = 'def'
                        elif re.match(r'^\s*(?:static\s+)?const(?:expr)?\s+[\w:]+\s+' + re.escape(ident) + r'\s*=', ns[i]):
                            kind = 'def'
                # scope (function / enum context)
                forced = {fid: b for fid, b in walk.chain[i]}
                scope = scope_at(walk, ns, forced, i, pos, scope_cache)
                if kind == 'use' and region == 'C' and scope.in_kind() == 'enum':
                    prevtxt = before.rstrip()
                    if not prevtxt:
                        # line start: previous code line must end with ',' or '{' (or be the enum opener)
                        j = i - 1
                        while j >= 0 and (walk.is_dir[j] or not ns[j].strip()):
                            j -= 1
                        prevtxt = ns[j].rstrip() if j >= 0 else ''
                        prev_ok = (prevtxt.endswith(',') or prevtxt.endswith('{') or prevtxt == '')
                    else:
                        prev_ok = prevtxt.endswith(',') or prevtxt.endswith('{')
                    nxt = after.lstrip()
                    next_ok = (nxt == '' or nxt.startswith(',') or nxt.startswith('}') or (nxt.startswith('=') and not nxt.startswith('==')))
                    if prev_ok and next_ok:
                        kind = 'def'
                # function / usage
                if kind == 'use':
                    use = usage_of(ns[i] if region == 'C' else raw, pos, ident, region) if region != 'M' else '-'
                    fn = scope.innermost('func')
                    if region == 'M':
                        func = '-'
                    elif fn is not None:
                        func = fn
                    else:
                        func = '-'
                    if state == 'dead' and fn is not None:
                        report['dead_func_whatif'] += 1
                else:
                    use, func = '-', '-'
                code = raw.encode('latin-1').decode('cp949', 'replace').replace('\t', ' ').strip()
                code = re.sub(r'\s+', ' ', code)[:160]
                rows.append(dict(file=rel, line=i + 1, col=pos, group=g, token=ident, kind=kind, state=state, in_project=proj_ok,
                                 function=func, usage=use, guard_chain=chain, code=code))
        for x in sorted(unk_walk):
            pass

    # ---- write TSV
    out_tsv = os.path.join(SCRATCH, 'region_reach_hits.tsv')
    hdr = ['file', 'line', 'group', 'token', 'kind', 'state', 'in_project', 'function', 'usage', 'guard_chain', 'code']
    with open(out_tsv, 'w', encoding='utf-8', newline='\n') as fh:
        fh.write('\t'.join(hdr) + '\n')
        for r in rows:
            fh.write('\t'.join(str(r[h]).replace('\t', ' ').replace('\n', ' ') for h in hdr) + '\n')

    # ---- report
    L = []
    L.append('TSV: %s  rows=%d' % (out_tsv, len(rows)))
    cs = collections.Counter((r['kind'], r['state']) for r in rows)
    L.append('rows by kind x state: ' + '; '.join('%s/%s=%d' % (k[0], k[1], v) for k, v in sorted(cs.items())))
    L.append('rows by group: ' + str(dict(collections.Counter(r['group'] for r in rows))))
    byf = collections.OrderedDict()
    for r in rows:
        if r['kind'] == 'use':
            byf.setdefault(r['file'], collections.Counter())[r['state']] += 1
    L.append('use rows by file (live/dead/comment/?):')
    for f, c in byf.items():
        L.append('  %-50s live=%d dead=%d comment=%d ?=%d' % (f, c['live'], c['dead'], c['comment'], c['?']))
    L.append('excluded KncWX2Server/Common .cpp (hits but not a ClCompile in any client vcxproj): ' + str(dict(excluded_cpp)))
    L.append('line-count mismatches RC.active vs raw: ' + (str(report['linecount_mismatch']) or 'none'))
    L.append('walker vs RC.active cross-check mismatches (file, n, first lines): ' + (str(report['crosscheck']) or 'none'))
    L.append('include-guard frames forced active: ' + (str(report['guard_files']) or 'none'))
    L.append('unknown flags after probe, RC.active (per file): ' + (str(dict(report['unknown_post'])) or 'none'))
    L.append('unknown flags inside hit guard chains: ' + (str({k: sorted(v) for k, v in report['unknown_chain'].items()}) or 'none'))
    L.append('approximated #if/#elif in hit chains (state=? where it decides): ' + (str(dict(report['approx_exprs'])) or 'none'))
    L.append('strip_comments vs lexer disagreements: %d %s' % (len(report['strip_vs_lexer']), report['strip_vs_lexer'][:8]))
    L.append('dead use rows whose function came from a forced-branch (what-if) parse: %d' % report['dead_func_whatif'])
    # distinct use+live functions
    fn_first = collections.OrderedDict()
    for r in rows:
        if r['kind'] == 'use' and r['state'] == 'live':
            key = (r['file'], r['function'])
            fn_first.setdefault(key, r['line'])
    L.append('distinct use+live functions: %d' % len(fn_first))
    for (f, fn), ln in fn_first.items():
        L.append('  %s  %s:%d' % (fn, f, ln))
    # spot-check sample
    rnd = random.Random(20261016)
    use_rows = [r for r in rows if r['kind'] == 'use']
    sample = rnd.sample(use_rows, min(5, len(use_rows)))
    L.append('spot-check sample (seed 20261016):')
    for r in sample:
        L.append('  %s:%d [%s %s %s %s] %s' % (r['file'], r['line'], r['token'], r['state'], r['function'], r['usage'], r['code'][:90]))
    print('\n'.join(L))


if __name__ == '__main__':
    main()
