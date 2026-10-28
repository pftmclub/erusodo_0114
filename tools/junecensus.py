#!/usr/bin/env python
"""junecensus.py - census of June-named identifiers and of the March functions June code calls
(JUNE_MARCH_DIVERGENCE_PHASES.md, Phase D0a).

    python tools/junecensus.py                 # scan (per-file cache, ~15 s), write june2017_re/diverge/names.tsv + callees.tsv, print the summary
    python tools/junecensus.py --summary       # the same (the summary is the census table of the plan Context)
    python tools/junecensus.py --check-twins   # June-named functions / members that have a March twin in the same class
    python tools/junecensus.py --file PATH     # every row and every callee site that belongs to one source file
    python tools/junecensus.py --gate          # exit 1 while a June-named identifier remains (plan C5 / D2 gate)
    python tools/junecensus.py --no-scan       # read the last tsv files instead of scanning
    python tools/junecensus.py --rescan        # drop the per-file cache (.build/junecensus) first
    python tools/junecensus.py --jobs N        # worker processes (default: cpu count)

Source set: X2Lib (with Offline/), KTDXLIB, X2 `.cpp` / `.h` / `.inl`. Comments are dropped, so a name is an identifier, never prose.

names.tsv - one row per definition of an identifier that carries `June` / `JUNE_` (never `SERV_IRUHADEV_*`), a June address or a phase tag
  name kind file line scope func flag june_addr twin twin_check funcmap_march defs uses note
  kind        member function | free function | data member | local | constant | global variable | namespace | class | file | lua name |
              unresolved (used, no definition found)
  scope       the class (nested classes joined with ::) or `ns:<namespace>`; empty at file scope
  func        the enclosing function of a local / function-local constant
  flag        innermost SERV_IRUHADEV_* flag at the definition; `!FLAG` = in the branch where the flag is off (the March original)
  june_addr   31..33xxxxxx address from the comments next to the definition, from the name, or from funcmap.tsv by name
  twin        the name with its June part removed, when the same class (or, for a free function, the tree) has that member
  twin_check  yes (funcmap maps the address to that twin) | conflict (funcmap maps it to another function) |
              fm-only (no twin by name, funcmap names a March function) | unmapped (no address or no funcmap row) | no-twin
  funcmap_march  the march_symbol of the funcmap row whose june_va is june_addr
  defs        definition rows sharing this name (a name defined in several classes has several rows)
  uses        occurrences that are not definitions. Locals: inside the same function. Everything else: the whole tree, by name
              (so two rows of one name carry the same number)

callees.tsv - one row per distinct (owner, function) called from June code: the body of a June-named function, a function in a
  `_CONST_*_JUNE_*` namespace, or the live branch of a defined SERV_IRUHADEV_JUNE_* flag (never the #else)
  callee owner owner_n origin sites sites_fn sites_flag files via funcmap_rows funcmap_status hint first_sites
  sites_fn / sites_flag  call sites inside a June-named body / inside the live branch of a defined June flag (sites = both)
  owner       class that declares the function (the root declaring class when a call through a pointer matches several classes);
              empty = free function. owner_n = how many classes the call could be (1 = certain)
  origin      march (a declaration outside every SERV_IRUHADEV flag) | ours (declared only behind our flags)
  funcmap_status  same | hint: <word> | none - from the funcmap.tsv rows whose march_symbol names this function
"""
import argparse
import bisect
import collections
import csv
import json
import multiprocessing
import os
import pickle
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..'))
OUT_DIR = os.path.join(TRUNK, 'june2017_re', 'diverge')
FUNCMAP = os.path.join(TRUNK, 'june2017_re', 'funcmap.tsv')
CACHE_DIR = os.path.join(TRUNK, '.build', 'junecensus')
SRC_DIRS = ['X2Lib', 'KTDXLIB', 'X2']
SHARED_DIRS = ['KncWX2Server/Common']      # shared headers: only the files that name June (event ids, packets)
SRC_EXT = ('.cpp', '.h', '.inl', '.hpp', '.cc', '.c')
SKIP_DIRS = {'debug', 'release', 'ipch', 'us_service', 'us_internal', 'us_test', 'x2tool', '.svn', 'cximage', 'fmod', 'tinyxml'}
SCAN_VERSION = 9            # bump when the per-file result shape changes

FLAG_PREFIX = 'SERV_IRUHADEV_'
JUNE_FLAG_PREFIX = 'SERV_IRUHADEV_JUNE_'
RX_JUNE = re.compile(r'(?i)june')
RX_ADDR = re.compile(r'(?<![0-9A-Fa-f])3[1-3][0-9a-f]{6}(?![0-9A-Fa-f])')
RX_ADDR_NAME = re.compile(r'(?<![0-9])3[1-3][0-9a-f]{6}(?![0-9A-Za-z])')
RX_PHASE_TAG = re.compile(r'(?i)(?:^|_)CH\d+[a-z]\d?(?:_|$)')
RX_CHTAG_ALIAS = re.compile(r'^JUNE_CH\d+[A-Za-z]?\d?_')
RX_MACRO = re.compile(r'^[A-Z][A-Z0-9_]*$')

KEYWORDS = set('''if else for while do switch case default break continue return goto sizeof new delete throw try catch typedef
using namespace template typename class struct union enum public private protected virtual static extern inline const volatile
operator this true false nullptr static_cast dynamic_cast reinterpret_cast const_cast typeid defined __declspec explicit
mutable register friend asm __asm __FUNCTION__ __FILE__ __LINE__ __forceinline __cdecl __stdcall __thiscall'''.split())
NOT_TYPE = set('''return case goto delete new else throw sizeof typedef using namespace public private protected do
if for while switch catch default break continue operator'''.split())
NOT_CALL = set('''if for while switch catch return sizeof static_cast dynamic_cast reinterpret_cast const_cast typeid defined
__declspec throw new delete decltype alignof __FUNCTION__ _T TEXT'''.split())
PRIM_TYPES = set('int float double char short long bool void unsigned signed wchar_t size_t DWORD BYTE WORD UINT'.split())

# ----------------------------------------------------------------------------------------------------------------------
# tokenizer
# ----------------------------------------------------------------------------------------------------------------------
TOK = re.compile(r'''
    (?P<bc>/\*.*?\*/)                                   |
    (?P<lc>//[^\n]*)                                    |
    (?P<pp>(?m:^)[ \t]*\#(?:[^\n\\/]|\\\r?\n|\\.|/(?!/|\*)|/\*.*?\*/)*) |
    (?P<str>(?:L|u8|u|U)?"(?:\\.|[^"\\\n])*")           |
    (?P<chr>L?'(?:\\.|[^'\\\n])*')                      |
    (?P<num>0[xX][0-9a-fA-F]+[uUlL]*|\d[\w.]*)          |
    (?P<id>[A-Za-z_]\w*)                                |
    (?P<op>::|->|\.\.\.|<<=|>>=|[-+*/%&|^<>=!]=|<<|>>|&&|\|\||\+\+|--|[{}()\[\];,.<>=+\-*/%&|^~!?:\#])
''', re.X | re.S)


def read_text(path):
    raw = open(path, 'rb').read()
    try:
        return raw.decode('utf-8-sig') if raw[:3] == b'\xef\xbb\xbf' else raw.decode('utf-8')
    except UnicodeDecodeError:
        return raw.decode('cp949', 'replace')


def defined_flags():
    """every SERV_IRUHADEV_* with a live #define in Always.h (June or not)"""
    try:
        txt = open(os.path.join(TRUNK, 'KTDXLIB', 'Always.h'), 'rb').read().decode('latin-1')
    except OSError:
        return set()
    out = set()
    for m in re.finditer(r'(?m)^[ \t]*#[ \t]*(define|undef)[ \t]+(SERV_IRUHADEV_\w+)', txt):
        if m.group(1) == 'define':
            out.add(m.group(2))
        else:
            out.discard(m.group(2))
    return out


def is_june_name(nm):
    if nm.startswith(FLAG_PREFIX):
        return False
    if RX_JUNE.search(nm):
        return True
    if RX_ADDR_NAME.search(nm) and re.search(r'[a-f]', RX_ADDR_NAME.search(nm).group(0)):
        return True
    return bool(RX_PHASE_TAG.search(nm))


def strip_june(nm):
    """candidate March spelling of a June-named identifier"""
    out = re.sub(r'(?i)june_?', '', nm, count=1)
    return out if out and out != nm else ''


# ----------------------------------------------------------------------------------------------------------------------
# #if evaluation, just enough for SERV_IRUHADEV flags
# ----------------------------------------------------------------------------------------------------------------------
def eval_cond(expr, defs):
    """-> (value True/False/None, june names, positive) for an #if expression; unknown names make the result None"""
    names = re.findall(r'\b(SERV_IRUHADEV_\w+)', expr)
    if not names:
        return None, [], True
    e = expr
    e = re.sub(r'defined\s*\(\s*(\w+)\s*\)', r'defined \1', e)

    def val(nm):
        return 1 if nm in defs else (0 if nm.startswith(FLAG_PREFIX) else None)
    toks = re.findall(r'defined\s+\w+|\w+|&&|\|\||!|\(|\)', e)
    pos = [0]

    def parse_or():
        a = parse_and()
        while pos[0] < len(toks) and toks[pos[0]] == '||':
            pos[0] += 1
            b = parse_and()
            a = 1 if (a == 1 or b == 1) else (None if (a is None or b is None) else 0)
        return a

    def parse_and():
        a = parse_not()
        while pos[0] < len(toks) and toks[pos[0]] == '&&':
            pos[0] += 1
            b = parse_not()
            a = 0 if (a == 0 or b == 0) else (None if (a is None or b is None) else 1)
        return a

    def parse_not():
        if pos[0] < len(toks) and toks[pos[0]] == '!':
            pos[0] += 1
            a = parse_not()
            return None if a is None else 1 - a
        return parse_atom()

    def parse_atom():
        if pos[0] >= len(toks):
            return None
        t = toks[pos[0]]
        pos[0] += 1
        if t == '(':
            a = parse_or()
            if pos[0] < len(toks) and toks[pos[0]] == ')':
                pos[0] += 1
            return a
        if t.startswith('defined'):
            return val(t.split()[1])
        return val(t) if t.startswith(FLAG_PREFIX) else None
    try:
        v = parse_or()
    except Exception:
        v = None
    neg = bool(re.match(r'\s*!\s*(defined|SERV_)', expr)) and '&&' not in expr and '||' not in expr
    return (None if v is None else bool(v)), names, not neg


class Frame(object):
    __slots__ = ('names', 'jb', 'val', 'branch', 'snap', 'ends', 'june')

    def __init__(self, names, jb, val, snap):
        self.names = names          # SERV_IRUHADEV_* names in the condition
        self.jb = jb                # branch (0/1) in which the flag is "on", None for a plain frame
        self.val = val              # True / False / None (flag defined?)
        self.branch = 0
        self.snap = snap            # parser stack at the #if
        self.ends = []              # parser stack at the end of each finished branch
        self.june = any(n.startswith(JUNE_FLAG_PREFIX) for n in names)

    def active(self):
        if self.jb is None or self.val is None:
            return True
        return (self.branch == self.jb) if self.val else (self.branch != self.jb)


# ----------------------------------------------------------------------------------------------------------------------
# per-file scan
# ----------------------------------------------------------------------------------------------------------------------
def scan_file(path):
    text = read_text(path)
    defs_flag = scan_file.defs
    nl = [m.start() for m in re.finditer('\n', text)]
    line_of = lambda off: bisect.bisect_left(nl, off) + 1
    toks = []
    comments = collections.defaultdict(list)
    for m in TOK.finditer(text):
        k = m.lastgroup
        if k in ('bc', 'lc'):
            ln = line_of(m.start())
            if k == 'bc':
                end = line_of(m.end())
                for l2 in range(ln, end + 1):
                    comments[l2].append(m.group(0))
            else:
                comments[ln].append(m.group(0))
            continue
        toks.append((k, m.group(0), m.start()))
    n = len(toks)
    rel = os.path.relpath(path, TRUNK).replace('\\', '/')
    is_offline = '/Offline/' in rel or rel.startswith('X2Lib/Offline/')

    res = {'rel': rel, 'defs': [], 'uses': collections.Counter(), 'luses': collections.Counter(), 'funcs': [], 'members': [],
           'classes': {}, 'calls': collections.OrderedDict(), 'lua': [], 'strings': 0, 'includes': [], 'bodies': [], 'occ': collections.Counter()}
    stack = []                      # parser scopes: dicts with ch
    frames = []
    pair = {}
    last_close = [None]             # index of the last ')' closed
    n_calls_cap = 8

    def innermost_flag():
        for f in reversed(frames):
            if f.names:
                nm = [x for x in f.names if x.startswith(JUNE_FLAG_PREFIX)] or f.names
                return ('' if (f.jb is None or f.branch == f.jb) else '!') + nm[0]
        return ''

    def june_live():
        """is the position inside the live branch of a defined June flag (June code)?"""
        for f in frames:
            if f.june and f.jb is not None and f.branch == f.jb and f.val is not False:
                return True
        return False

    def dead_branch():
        """inside the branch of a June frame that is not compiled"""
        for f in frames:
            if f.jb is not None and f.val is not None and not f.active():
                return True
        return False

    def ours_flag_on():
        for f in frames:
            if f.names and f.jb is not None and f.branch == f.jb:
                return True
        return False

    def cls_path():
        return '::'.join(s['name'] for s in stack if s.get('kind') == 'class' and s.get('name'))

    def ns_name():
        for s in reversed(stack):
            if s.get('kind') == 'ns' and s.get('name'):
                return s['name']
        return ''

    def cur_fn():
        for s in stack:
            if s.get('kind') == 'fn':
                return s
        return None

    def in_param_list():
        return bool(stack) and stack[-1]['ch'] == '(' and stack[-1].get('decl')

    def callee_span(i):
        """for the '(' at toks[i]: (full text, first token index, name token index) of what it calls"""
        j = i - 1
        if j < 0:
            return '', i, i
        if toks[j][1] == '>':                       # Foo<T>( ... )
            k, depth = j, 0
            while k > 0:
                if toks[k][1] == '>':
                    depth += 1
                elif toks[k][1] == '<':
                    depth -= 1
                    if depth == 0:
                        break
                k -= 1
            if k > 0 and toks[k - 1][0] == 'id':
                nm = toks[k - 1][1]
                s = k - 1
                while s - 2 >= 0 and toks[s - 1][1] == '::' and toks[s - 2][0] == 'id':
                    nm = toks[s - 2][1] + '::' + nm
                    s -= 2
                return nm, s, k - 1
            return '', i, i
        if toks[j][0] == 'id':
            nm = toks[j][1]
            s = j
            if s - 1 >= 0 and toks[s - 1][1] == '~':
                nm = '~' + nm
                s -= 1
            while s - 2 >= 0 and toks[s - 1][1] == '::' and toks[s - 2][0] == 'id':
                nm = toks[s - 2][1] + '::' + nm
                s -= 2
                if s - 1 >= 0 and toks[s - 1][1] == '~':
                    pass
            return nm, s, j
        if toks[j][0] == 'op':                      # operator==( ... )
            for back in (1, 2, 3):
                if j - back >= 0 and toks[j - back][1] == 'operator':
                    return 'operator', j - back, j - back
        return '', i, i

    def type_ish(t):
        return (t[0] == 'id' and t[1] not in NOT_TYPE) or t[1] in ('*', '&', '>', '~') or (t[0] == 'id' and t[1] in PRIM_TYPES)

    def stmt_start(i):
        j = i - 1
        while j >= 0 and i - j < 90:
            t = toks[j]
            if t[0] != 'pp' and t[1] in (';', '{', '}'):
                return j + 1
            if t[1] == ':' and j >= 1 and toks[j - 1][1] in ('public', 'private', 'protected', 'default'):
                return j + 1
            j -= 1
        return max(j + 1, 0)

    def decl_check(i, boundary_commas, allow_paren=False):
        """is the id at toks[i] declared here? (a type run, then the name, then = ; , [ ) ), or ( for a constructor-style local)"""
        nx = toks[i + 1][1] if i + 1 < n else ''
        if nx not in ('=', ';', ',', '[', ')', ':') and not (allow_paren and nx == '('):
            return False
        j = i - 1
        if j >= 0 and toks[j][1] == '::':
            return False
        if j >= 0 and toks[j][1] == ',' and not boundary_commas and stack and stack[-1]['ch'] == '{':
            # a second declarator: `Type a = 1, bJune = 2;`
            st = stmt_start(i)
            while st < n and toks[st][0] == 'pp':
                st += 1
            if st + 2 < i and toks[st][0] == 'id' and toks[st][1] not in NOT_TYPE and (toks[st + 1][0] == 'id' or toks[st + 1][1] in ('*', '&', '<', '::')):
                return True
            return False
        ntype = 0
        while j >= 0:
            t = toks[j]
            if t[1] == ')' and j in pair and pair[j] >= 1 and toks[pair[j] - 1][1] == '__declspec':
                j = pair[j] - 1
            elif t[0] == 'id':
                if t[1] in NOT_TYPE:
                    return False
                ntype += 1
            elif t[1] in ('::', '*', '&'):
                pass
            elif t[1] == '>':
                depth = 0
                while j >= 0:
                    if toks[j][1] == '>':
                        depth += 1
                    elif toks[j][1] == '<':
                        depth -= 1
                        if depth == 0:
                            break
                    j -= 1
                if j < 0:
                    return False
            elif t[0] == 'pp' or t[1] in (';', '{', '}', '('):
                break
            elif t[1] == ',' and boundary_commas:
                break
            elif t[1] == ':' and j >= 1 and toks[j - 1][1] in ('public', 'private', 'protected', 'default'):
                break
            else:
                return False
            j -= 1
        return ntype >= 1

    def typedef_stmt(i):
        st = stmt_start(i)
        while st < i and toks[st][0] == 'pp':
            st += 1
        return toks[st][1] == 'typedef'

    def addr_near(line, nm):
        m = RX_ADDR.search(nm)
        if m and re.search('[a-f]', m.group(0)):
            return m.group(0)
        for d in (0, -1, 1, -2, 2):
            cm = comments.get(line + d)
            if cm:
                m = RX_ADDR.search(' '.join(cm))
                if m:
                    return m.group(0)
        return ''

    def push_pp_snapshot():
        return list(stack)

    cur_enum_stack = []
    pending_local_const = False
    skipping_dead = False

    trace = os.environ.get('JC_TRACE', '')
    trace_line = int(trace.split(':')[1]) if trace.startswith(rel + ':') else 0
    for i in range(n):
        kd, tx, off = toks[i]
        if trace_line and kd != 'pp' and line_of(off) >= trace_line:
            sys.stderr.write('TRACE %s:%d stack=%s frames=%s\n' % (rel, line_of(off), [(e['ch'], e.get('kind', ''), e.get('name', '')) for e in stack],
                                                                   [(f.names, f.branch) for f in frames]))
            trace_line = 0
        if kd == 'pp':
            body = re.sub(r'/\*.*?\*/|//[^\n]*', ' ', tx.strip()[1:], flags=re.S)
            body = re.sub(r'\\\r?\n', ' ', body)
            body = re.sub(r'\s+', ' ', body).strip()
            m = re.match(r'(ifdef|ifndef|if|elif|else|endif|define|include)\b\s*(.*)', body)
            if not m:
                continue
            d, rest = m.group(1), m.group(2)
            if d in ('ifdef', 'ifndef'):
                nm = rest.split(' ')[0] if rest else ''
                names = [nm] if nm.startswith(FLAG_PREFIX) else []
                if names:
                    v = nm in defs_flag
                    frames.append(Frame(names, 0 if d == 'ifdef' else 1, v, push_pp_snapshot()))
                else:
                    frames.append(Frame([], None, None, push_pp_snapshot()))
            elif d == 'if':
                v, names, positive = eval_cond(rest, defs_flag)
                if names:
                    frames.append(Frame(names, 0 if positive else 1, v, push_pp_snapshot()))
                else:
                    frames.append(Frame([], None, None, push_pp_snapshot()))
            elif d in ('elif', 'else'):
                if frames:
                    f = frames[-1]
                    f.ends.append(list(stack))
                    f.branch += 1
                    stack[:] = f.snap
                    if d == 'elif' and f.names:
                        v, names, positive = eval_cond(rest, defs_flag)
                        if names and f.jb is not None:
                            f.names = list(set(f.names) | set(names))
            elif d == 'endif':
                if frames:
                    f = frames.pop()
                    f.ends.append(list(stack))
                    pick = f.ends[0]
                    if f.jb is not None and f.val is not None and len(f.ends) > 1:
                        want = 0 if (f.val == (f.jb == 0)) else 1
                        pick = f.ends[min(want, len(f.ends) - 1)]
                    stack[:] = pick
                    for id_m in re.finditer(r'(?<![0-9A-Za-z_])[A-Za-z_]\w*', rest):
                        if is_june_name(id_m.group(0)):
                            res['uses'][id_m.group(0)] += 1
            elif d == 'include':
                im = re.search(r'["<]([^">]+)[">]', rest)
                if im and RX_JUNE.search(im.group(1)):
                    res['includes'].append(os.path.basename(im.group(1)))
            elif d == 'define':
                dm = re.match(r'(\w+)(\([^)]*\))?\s*(.*)', rest)
                if dm:
                    nm, repl = dm.group(1), dm.group(3)
                    if is_june_name(nm) and repl.strip() and not nm.startswith(FLAG_PREFIX):
                        res['defs'].append((nm, 'constant', line_of(off), '', '', innermost_flag(), addr_near(line_of(off), nm), 'macro'))
                    for id_m in re.finditer(r'(?<![0-9A-Za-z_])[A-Za-z_]\w*', repl):
                        if is_june_name(id_m.group(0)):
                            res['uses'][id_m.group(0)] += 1
            continue
        if kd == 'str':
            if RX_JUNE.search(tx):
                res['strings'] += 1
            continue
        if kd != 'id' and kd != 'op':
            continue
        if kd == 'op':
            if tx == '(':
                nm, s, ni = callee_span(i)
                ent = {'ch': '(', 'pos': i, 'callee': nm, 'cs': s, 'ni': ni, 'decl': False, 'cand': False}
                top = stack[-1] if stack else None
                scope_ok = (not stack or (top['ch'] == '{' and top.get('kind') in ('class', 'ns', 'extern'))) and not cur_fn()
                if nm and scope_ok and not RX_MACRO.match(nm.split('::')[-1]) and nm.split('::')[-1] not in NOT_CALL:
                    p = toks[s - 1] if s > 0 else ('', '', 0)
                    qual = '::' in nm
                    if type_ish(p) or qual or p[1] in (';', '{', '}', ':') or p[0] == 'pp' or p[0] == '':
                        ent['cand'] = True
                        ent['decl'] = True
                elif nm and scope_ok and RX_MACRO.match(nm.split('::')[-1]) and is_june_name(nm.split('::')[-1]):
                    ent['cand'] = True
                    ent['decl'] = True
                stack.append(ent)
            elif tx == '[':
                stack.append({'ch': '[', 'pos': i})
            elif tx == '{':
                j0 = i - 1
                while j0 >= 0 and toks[j0][0] == 'pp':
                    j0 -= 1
                pv = toks[j0] if j0 >= 0 else ('', '', 0)
                ent = {'ch': '{', 'kind': 'block', 'pos': i, 'name': ''}
                j = j0
                while j >= 0 and (toks[j][1] in ('const', 'volatile', 'override', 'final') or toks[j][0] == 'pp'):
                    j -= 1
                pvj = toks[j] if j >= 0 else ('', '', 0)
                infn = cur_fn() is not None
                head = stmt_start(i)
                htoks = [t[1] if t[0] != 'pp' else ';;' for t in toks[head:i]]
                is_head = ('class' in htoks or 'struct' in htoks or 'union' in htoks) and '(' not in htoks and pvj[1] != ')' and '=' not in htoks
                if infn and not (is_head or ('enum' in htoks and '(' not in htoks)):
                    ent['kind'] = 'init' if pv[1] in ('=',) else 'block'
                elif not infn and 'namespace' in htoks and '(' not in htoks:
                    ent['kind'] = 'ns'
                    k = len(htoks) - 1
                    ent['name'] = htoks[k] if k >= 0 and htoks[k] != 'namespace' else ''
                    if ent['name'] and toks[head + k][0] != 'id':
                        ent['name'] = ''
                elif pv[1] == '"' or (pv[0] == 'str' and 'extern' in htoks):
                    ent['kind'] = 'extern'
                elif 'enum' in htoks and '(' not in htoks:
                    ent['kind'] = 'enum'
                    ids = [t for t in toks[head:i] if t[0] == 'id' and t[1] not in ('enum', 'class', 'struct')]
                    ent['name'] = ids[0][1] if ids else ''
                elif is_head:
                    kw = max(k for k, t in enumerate(htoks) if t in ('class', 'struct', 'union'))
                    ids = []
                    k = kw + 1
                    while k < len(htoks) and htoks[k] not in (':', '{'):
                        if toks[head + k][0] == 'id' and htoks[k] not in ('__declspec',):
                            ids.append(htoks[k])
                        if htoks[k] == '<':
                            break
                        k += 1
                    name = ids[-1] if ids else ''
                    if len(ids) >= 2 and (RX_MACRO.match(ids[0]) or ids[0].endswith('_API')):
                        name = ids[-1]
                    ent['kind'] = 'class'
                    ent['name'] = name
                    bases = []
                    if ':' in htoks[kw:]:
                        k = head + kw + htoks[kw:].index(':') + 1
                        seg = []
                        depth = 0
                        while k < i:
                            t = toks[k]
                            if t[1] == '<':
                                depth += 1
                            elif t[1] == '>':
                                depth -= 1
                            elif t[1] == ',' and depth == 0:
                                if seg:
                                    bases.append(seg[-1])
                                seg = []
                            elif t[0] == 'id' and depth == 0 and t[1] not in ('public', 'private', 'protected', 'virtual'):
                                seg.append(t[1])
                            k += 1
                        if seg:
                            bases.append(seg[-1])
                    if name:
                        full = (cls_path() + '::' + name) if cls_path() else name
                        res['classes'][full] = bases
                        if is_june_name(name):
                            noff = next((toks[head + k2][2] for k2 in range(kw, len(htoks)) if htoks[k2] == name), off)
                            res['defs'].append((name, 'class', line_of(noff), cls_path(), '', innermost_flag(), addr_near(line_of(noff), name), ''))
                elif pvj[1] == ')':
                    close = j
                    # constructor initializer list: walk back to the parameter list
                    guard = 0
                    while guard < 400:
                        guard += 1
                        op = pair.get(close)
                        if op is None:
                            break
                        nm, s, ni = callee_span(op)
                        if s - 1 >= 0 and toks[s - 1][1] == ',' and s - 2 >= 0 and toks[s - 2][1] == ')':
                            close = s - 2
                            continue
                        if s - 1 >= 0 and toks[s - 1][1] == ':' and s - 2 >= 0 and toks[s - 2][1] in (')', 'const'):
                            close = s - 2 if toks[s - 2][1] == ')' else s - 3
                            continue
                        break
                    op = pair.get(close)
                    nm, s, ni = callee_span(op) if op is not None else ('', 0, 0)
                    if nm and nm.split('::')[-1] not in NOT_CALL:
                        parts = nm.split('::')
                        fname = parts[-1]
                        fcls = '::'.join(parts[:-1]) if len(parts) > 1 else (cls_path() or '')
                        ent['kind'] = 'fn'
                        ent['name'] = fname
                        ent['cls'] = fcls
                        ent['ns'] = ns_name() if len(parts) == 1 and not cls_path() else ''
                        ent['line'] = line_of(toks[ni][2])
                        ent['june'] = bool(is_june_name(fname) or (ent['ns'] and is_june_name(ent['ns'])) or
                                           any(s2.get('kind') == 'ns' and is_june_name(s2.get('name', '')) for s2 in stack))
                    else:
                        ent['kind'] = 'block'
                elif pv[1] in ('=', ',', '{', ']'):
                    ent['kind'] = 'init'
                stack.append(ent)
                if ent['kind'] == 'fn':
                    res['bodies'].append((ent['cls'], ent['name'], ent['june'], ent['line']))
            elif tx in (')', ']', '}'):
                want = {')': '(', ']': '[', '}': '{'}[tx]
                while stack and stack[-1]['ch'] != want:
                    stack.pop()
                if stack:
                    ent = stack.pop()
                    if tx == ')':
                        pair[i] = ent['pos']
                        if ent.get('cand'):
                            # classify the declaration / definition that this ')' ends
                            k = i + 1
                            while k < n and (toks[k][1] in ('const', 'volatile', 'override', 'final', 'throw', '=', '0', 'abstract') or toks[k][0] == 'pp'):
                                if toks[k][1] == 'throw' and k + 1 < n and toks[k + 1][1] == '(':
                                    depth = 0
                                    k += 1
                                    while k < n:
                                        if toks[k][1] == '(':
                                            depth += 1
                                        elif toks[k][1] == ')':
                                            depth -= 1
                                            if depth == 0:
                                                break
                                        k += 1
                                k += 1
                            nxt = toks[k][1] if k < n else ''
                            if nxt in ('{', ':') or (nxt == ',' and False):
                                role = 'def'
                            elif nxt == ';':
                                role = 'decl'
                            else:
                                role = ''
                            if role:
                                nm = ent['callee']
                                parts = nm.split('::')
                                fname = parts[-1]
                                fcls = '::'.join(parts[:-1]) if len(parts) > 1 else (cls_path() or '')
                                if fname and fname not in KEYWORDS:
                                    ln = line_of(toks[ent['ni']][2])
                                    in_ns = (len(parts) == 1 and not cls_path())
                                    res['funcs'].append((fcls, fname, role, ln, 'ours' if (is_offline or ours_flag_on()) else ''))
                                    if is_june_name(fname):
                                        scope = fcls if fcls else ('ns:' + ns_name() if ns_name() else '')
                                        kind = 'member function' if fcls else 'free function'
                                        res['defs'].append((fname, kind, ln, scope, '', innermost_flag(), addr_near(ln, fname),
                                                            'decl' if role == 'decl' else ''))
                                        res['uses'][fname] -= 1                 # the name token was counted as a use
                elif tx == '}' and False:
                    pass
            elif tx == ',':
                if stack and stack[-1]['ch'] == '(':
                    stack[-1]['commas'] = stack[-1].get('commas', 0) + 1
            continue
        # ---- an identifier -------------------------------------------------------------------------------------
        nxt = toks[i + 1][1] if i + 1 < n else ''
        # data-member declarations of any name (the member index), and calls from June code
        top = stack[-1] if stack else None
        fn = cur_fn()
        if nxt == '(' and fn and tx not in NOT_CALL and not RX_MACRO.match(tx) and tx not in KEYWORDS:
            p = toks[i - 1] if i > 0 else ('', '', 0)
            decl_local = (p[0] == 'id' and p[1] not in NOT_TYPE) or p[1] in ('*', '&') and False
            if not decl_local and not is_june_name(tx) and not dead_branch():
                jfn = fn.get('june') or any(s2.get('kind') == 'ns' and is_june_name(s2.get('name', '')) for s2 in stack)
                reason = 'fn' if jfn else ('flag' if june_live() else '')
                if reason:
                    via, qual = 'bare', ''
                    if p[1] in ('.', '->'):
                        via = 'obj'
                        if p[1] == '->' and i >= 2 and toks[i - 2][1] == 'this':
                            via = 'bare'
                    elif p[1] == '::':
                        q = []
                        j = i - 1
                        while j - 1 >= 0 and toks[j][1] == '::' and toks[j - 1][0] == 'id':
                            q.insert(0, toks[j - 1][1])
                            j -= 2
                        via, qual = 'qual', '::'.join(q)
                    key = (tx, via, qual, fn.get('cls', ''), fn.get('name', ''), reason)
                    rec = res['calls'].get(key)
                    if rec is None:
                        rec = [0, []]
                        res['calls'][key] = rec
                    rec[0] += 1
                    if len(rec[1]) < n_calls_cap:
                        rec[1].append(line_of(off))
        if not is_june_name(tx):
            # a non-June data member still matters: the twin index
            if top and top['ch'] == '{' and top.get('kind') == 'class' and not fn and nxt in (';', '=', '[', ',') and decl_check(i, False):
                res['members'].append((cls_path(), tx, line_of(off), 'ours' if (is_offline or ours_flag_on()) else ''))
            continue
        # ---- a June-named identifier ---------------------------------------------------------------------------
        res['occ'][tx] += 1
        ln = line_of(off)
        k_ = i - 1
        while k_ >= 0 and toks[k_][0] == 'pp':
            k_ -= 1
        pv = toks[k_] if k_ >= 0 else ('', '', 0)
        k_ = i + 1
        while k_ < n and toks[k_][0] == 'pp':
            k_ += 1
        nxt = toks[k_][1] if k_ < n else ''
        defined = False
        if pv[1] in ('class', 'struct', 'union'):
            defined = True                          # handled at its '{'
        elif pv[1] == 'enum':
            if nxt in ('{', ':', ';'):
                res['defs'].append((tx, 'class', ln, cls_path(), '', innermost_flag(), addr_near(ln, tx), 'enum'))
                defined = True
        elif pv[1] == 'namespace':
            if toks[i - 2][1] != 'using' and nxt == '{':
                res['defs'].append((tx, 'namespace', ln, 'ns:' + ns_name() if ns_name() else '', '', innermost_flag(), addr_near(ln, tx), ''))
                defined = True
        elif nxt == '(' and stack and stack[-1].get('ch') == '(' and stack[-1].get('ni') == i:
            defined = bool(stack[-1].get('cand'))       # recorded when its ')' closes
        elif top and top['ch'] == '(' and top.get('callee') in ('BOOST_TEST_FOREACH', 'BOOST_FOREACH') and fn and pv[1] == ',':
            res['defs'].append((tx, 'local', ln, fn['cls'], fn['name'], innermost_flag(), addr_near(ln, tx), 'foreach'))
            defined = True
        elif top and top['ch'] == '(' and top.get('callee') in ('_ENUM', 'DECL_PACKET', 'DECL_DATA', '_ENUM_ID') and not cur_fn() and nxt == ')':
            if top['callee'] in ('DECL_PACKET', 'DECL_DATA'):
                res['defs'].append((('K' + tx) if top['callee'] == 'DECL_PACKET' else tx, 'class', ln, '', '', innermost_flag(), addr_near(ln, tx), 'packet' if top['callee'] == 'DECL_PACKET' else 'data struct'))
            else:
                res['defs'].append((tx, 'constant', ln, '', '', innermost_flag(), addr_near(ln, tx), 'event id'))
            defined = True
        elif top and top['ch'] == '{' and top.get('kind') == 'enum' and pv[1] in ('{', ',') and nxt in ('=', ',', '}'):
            res['defs'].append((tx, 'constant', ln, cls_path(), '', innermost_flag(), addr_near(ln, tx), 'enum member'))
            defined = True
        elif nxt in (';', '[') and not in_param_list() and typedef_stmt(i):
            res['defs'].append((tx, 'class', ln, cls_path(), fn['name'] if fn else '', innermost_flag(), addr_near(ln, tx), 'typedef'))
            defined = True
        elif nxt == ')' and pv[1] == '*' and k_ >= 0 and toks[i - 2][1] == '(' and k_ + 1 < n and toks[k_ + 1][1] == '(':
            res['defs'].append((tx, 'local' if fn else 'global variable', ln, fn['cls'] if fn else ('ns:' + ns_name() if ns_name() else ''),
                                fn['name'] if fn else '', innermost_flag(), addr_near(ln, tx), 'function pointer'))
            defined = True
        elif nxt in ('=', ';', ',', '[', ')', ':', '(') and pv[1] not in ('.', '->', '::') and (decl_check(i, in_param_list(), fn is not None and nxt == '(')):
            st = stmt_start(i)
            stmt = [t[1] for t in toks[st:i]]
            is_const = 'const' in stmt
            if fn:
                if is_const and RX_MACRO.match(tx):
                    res['defs'].append((tx, 'constant', ln, cls_path() or ('ns:' + ns_name() if ns_name() else ''), fn['name'], innermost_flag(), addr_near(ln, tx), 'local const'))
                else:
                    res['defs'].append((tx, 'local', ln, fn['cls'], fn['name'], innermost_flag(), addr_near(ln, tx), 'param' if in_param_list() else ''))
                defined = True
            elif in_param_list():
                res['defs'].append((tx, 'local', ln, cls_path(), stack[-1].get('callee', ''), innermost_flag(), addr_near(ln, tx), 'param'))
                defined = True
            elif top and top['ch'] == '{' and top.get('kind') == 'class':
                kind = 'constant' if (is_const and ('static' in stmt or RX_MACRO.match(tx))) else 'data member'
                res['defs'].append((tx, kind, ln, cls_path(), '', innermost_flag(), addr_near(ln, tx), ''))
                res['members'].append((cls_path(), tx, ln, 'ours' if (is_offline or ours_flag_on()) else ''))
                defined = True
            elif not stack or (top['ch'] == '{' and top.get('kind') in ('ns', 'extern')):
                kind = 'constant' if is_const else 'global variable'
                res['defs'].append((tx, kind, ln, 'ns:' + ns_name() if ns_name() else '', '', innermost_flag(), addr_near(ln, tx), ''))
                defined = True
        if not defined:
            if fn and decl_check(i, False) is False:
                pass
            # use. Locals are counted per function
            if fn and nxt != '(':
                res['luses'][(tx, fn['cls'], fn['name'])] += 1
            res['uses'][tx] += 1
    # lua-bound names: string literals with June next to lua_tinker registrations
    for m in re.finditer(r'lua_tinker\s*::\s*(?:decl|def|class_def|class_add|set|class_mem|constructor)\b[^;]*?"([^"\n]*)"', text):
        if RX_JUNE.search(m.group(1)):
            res['lua'].append((m.group(1), line_of(m.start())))
    return res


def worker_init(defs):
    scan_file.defs = defs


def scan_one(path):
    try:
        st = os.stat(path)
        key = '%s|%d|%d|%d' % (path, st.st_mtime_ns, st.st_size, SCAN_VERSION)
        h = re.sub(r'[^0-9A-Za-z]', '_', os.path.relpath(path, TRUNK)) + '.pkl'
        cp = os.path.join(CACHE_DIR, h)
        if os.path.exists(cp):
            try:
                with open(cp, 'rb') as f:
                    k, r = pickle.load(f)
                if k == key:
                    return r
            except Exception:
                pass
        r = scan_file(path)
        with open(cp, 'wb') as f:
            pickle.dump((key, r), f, protocol=2)
        return r
    except Exception as e:                      # one broken file must not lose the census
        return {'rel': os.path.relpath(path, TRUNK).replace('\\', '/'), 'error': repr(e), 'defs': [], 'uses': collections.Counter(),
                'luses': collections.Counter(), 'funcs': [], 'members': [], 'classes': {}, 'calls': {}, 'lua': [], 'strings': 0,
                'includes': [], 'bodies': [], 'occ': collections.Counter()}


def source_files():
    out = []
    for d in SRC_DIRS:
        for root, dirs, files in os.walk(os.path.join(TRUNK, d)):
            dirs[:] = [x for x in dirs if x.lower() not in SKIP_DIRS]
            for f in files:
                if f.lower().endswith(SRC_EXT):
                    out.append(os.path.join(root, f))
    for d in SHARED_DIRS:
        for root, dirs, files in os.walk(os.path.join(TRUNK, d)):
            dirs[:] = [x for x in dirs if x.lower() not in SKIP_DIRS and x.lower() != 'onlyglobal']
            for f in files:
                if f.lower().endswith(SRC_EXT):
                    fp = os.path.join(root, f)
                    try:
                        if re.search(rb'(?i)june', open(fp, 'rb').read()):
                            out.append(fp)
                    except OSError:
                        pass
    return sorted(out)


# ----------------------------------------------------------------------------------------------------------------------
# funcmap
# ----------------------------------------------------------------------------------------------------------------------
HINT_WORDS = [
    ('stub', r'\bstub\b'), ('always', r'\balways (?:true|false|0|1|returns)\b|\bis always\b'),
    ('returns', r'\breturns? (?:0|1|false|true|nothing|-1)\b(?! if)'), ('no-op', r'\bno-?op\b'),
    ('differs', r'\bdiffers?\b|\bdifferent\b|\bunlike\b|\bdiverg'), ('none-in-March', r'\bnone in March\b|\bMarch (?:has no|lacks|does not)\b'),
    ('June-form', r'\bJune form\b|\bJune overload\b|\bJune-only\b'), ('dead', r'\bdead\b|\bnever (?:called|true|false)\b'),
    ('not-ported', r'\bnot ported\b'),
]


def load_funcmap():
    rows = []
    if not os.path.exists(FUNCMAP):
        return rows
    with open(FUNCMAP, encoding='utf-8', errors='replace') as f:
        rd = csv.reader(f, delimiter='\t')
        next(rd, None)
        for r in rd:
            if len(r) >= 3:
                rows.append((r[0], r[1], r[2], r[3] if len(r) > 3 else ''))
    return rows


RX_SYM = re.compile(r'((?:[A-Za-z_]\w*::)*)(~?[A-Za-z_]\w*)\s*(?:\(|$|\s|\{|\)|,)')


def symbol_parts(sym):
    """(class, function) pairs named in a march_symbol"""
    out = []
    for m in re.finditer(r'((?:[A-Za-z_]\w*::)+)(~?[A-Za-z_]\w*)', sym):
        out.append((m.group(1)[:-2], m.group(2)))
    return out


def hint_of(*texts):
    t = ' '.join(texts)
    for word, rx in HINT_WORDS:
        if re.search(rx, t, re.I):
            return word
    return ''


# ----------------------------------------------------------------------------------------------------------------------
# merge
# ----------------------------------------------------------------------------------------------------------------------
class World(object):
    def __init__(self, results):
        self.results = results
        self.bases = {}
        self.func_sites = collections.defaultdict(list)      # (cls, name) -> [(role, file, line, ours)]
        self.func_owners = collections.defaultdict(set)      # name -> classes
        self.members = collections.defaultdict(set)          # cls -> data member names
        self.march_members = collections.defaultdict(set)    # ... declared outside every SERV_IRUHADEV flag
        for r in results:
            for c, b in r['classes'].items():
                if c not in self.bases or b:
                    self.bases[c] = b
            for (cls, nm, role, ln, ours) in r['funcs']:
                self.func_sites[(cls, nm)].append((role, r['rel'], ln, ours))
                self.func_owners[nm].add(cls)
            for (cls, nm, ln, ours) in r['members']:
                self.members[cls].add(nm)
                if not ours:
                    self.march_members[cls].add(nm)
        self.children = collections.defaultdict(set)
        for c, bs in self.bases.items():
            for b in bs:
                self.children[b].add(c)
        self._anc = {}

    def short(self, c):
        return c.split('::')[-1]

    def ancestors(self, c):
        if c in self._anc:
            return self._anc[c]
        seen, todo = [], [c]
        while todo:
            x = todo.pop(0)
            for b in self.bases.get(x, []) + self.bases.get(self.short(x), []):
                if b not in seen:
                    seen.append(b)
                    todo.append(b)
        self._anc[c] = seen
        return seen

    def find_in_chain(self, cls, name):
        for c in [cls] + self.ancestors(cls):
            if (c, name) in self.func_sites:
                return c
            for k in list(self.func_owners.get(name, ())):
                if k == c or self.short(k) == self.short(c):
                    return k
        return None

    def resolve(self, name, via, qual, encl_cls):
        """-> (owner string, n candidates) or None for an external call"""
        if via == 'qual':
            if qual:
                o = self.find_in_chain(qual.split('::')[-1], name) or self.find_in_chain(qual, name)
                if o is not None:
                    return o, 1
                if '' in self.func_owners.get(name, ()):
                    return '', 1
            return None
        if via == 'bare':
            if encl_cls:
                o = self.find_in_chain(encl_cls, name)
                if o is not None:
                    return o, 1
            if '' in self.func_owners.get(name, ()):
                return '', 1
            return None
        cands = set(self.func_owners.get(name, ()))
        cands.discard('')
        if not cands:
            return ('', 1) if '' in self.func_owners.get(name, ()) else None
        roots = set(c for c in cands if not any(a in cands for a in self.ancestors(c)))
        roots = roots or cands
        return '|'.join(sorted(roots)), len(roots)


def build(results, fmap, jobs_note=''):
    W = World(results)
    # funcmap indexes
    fm_by_addr = {}
    fm_by_fn = collections.defaultdict(list)
    fm_sym_addr = collections.defaultdict(list)
    for (va, sym, status, ev) in fmap:
        for a in RX_ADDR.findall(va):
            fm_by_addr.setdefault(a, []).append((sym, status, ev))
        for (c, f) in symbol_parts(sym):
            fm_by_fn[f].append((c, va, sym, status, ev))
            for a in RX_ADDR.findall(va):
                fm_sym_addr[(c, f)].append(a)
        for f in re.findall(r'(?<![:\w])(~?[A-Za-z_]\w*)(?=\s*(?:\(|$))', sym):
            pass
    # ---------------- names
    all_defs = []
    uses_g = collections.Counter()
    luses = collections.Counter()
    for r in results:
        uses_g.update(r['uses'])
        luses.update(r['luses'])
    seen_rows = set()
    rows = []
    for r in results:
        for (nm, kind, ln, scope, func, flag, addr, note) in r['defs']:
            all_defs.append((r['rel'], nm, kind, ln, scope, func, flag, addr, note))
    decl_of = {}
    for (rel, nm, kind, ln, scope, func, flag, addr, note) in all_defs:
        if kind in ('member function', 'free function') and note == 'decl':
            decl_of.setdefault((scope, nm), (rel, ln, flag, addr))
    defined_fn = set((scope, nm) for (rel, nm, kind, ln, scope, func, flag, addr, note) in all_defs
                     if kind in ('member function', 'free function') and note != 'decl')
    for (rel, nm, kind, ln, scope, func, flag, addr, note) in all_defs:
        if kind in ('member function', 'free function') and note == 'decl' and (scope, nm) in defined_fn:
            continue
        key = (rel, nm, kind, ln, scope, func)
        if key in seen_rows:
            continue
        seen_rows.add(key)
        if not addr:
            for (rel2, ln2, fl2, ad2) in [decl_of.get((scope, nm), ('', 0, '', ''))]:
                addr = ad2
        rows.append([nm, kind, rel, ln, scope, func, flag, addr, note])
    # files
    include_n = collections.Counter()
    for r in results:
        for inc in r['includes']:
            include_n[inc] += 1
    for p in source_files():
        b = os.path.basename(p)
        if RX_JUNE.search(b):
            rows.append([b, 'file', os.path.relpath(p, TRUNK).replace('\\', '/'), 1, '', '', '', '', ''])
    # lua names
    for r in results:
        for (s, ln) in r['lua']:
            rows.append([s, 'lua name', r['rel'], ln, '', '', '', '', ''])
    # unresolved: used, never defined
    defined_names = set(row[0] for row in rows)
    for nm, c in sorted(uses_g.items()):
        if c > 0 and nm not in defined_names and not nm.startswith(FLAG_PREFIX) and is_june_name(nm):
            rows.append([nm, 'unresolved', '', 0, '', '', '', '', ''])
    # counts per name
    defs_n = collections.Counter(row[0] for row in rows)
    out = []
    for (nm, kind, rel, ln, scope, func, flag, addr, note) in rows:
        if kind == 'local':
            uses = luses.get((nm, scope, func), 0)
        elif kind == 'file':
            uses = include_n.get(nm, 0)
        elif kind == 'lua name':
            uses = 0
        else:
            uses = uses_g.get(nm, 0)
        twin_name = strip_june(nm) if kind in ('member function', 'free function', 'data member') else ''
        twin = ''
        if twin_name:
            if kind == 'data member':
                if twin_name in W.march_members.get(scope, ()):
                    twin = twin_name
            elif kind == 'member function':
                if any(st[3] == '' for st in W.func_sites.get((scope, twin_name), ())):
                    twin = twin_name
            else:
                if any(st[3] == '' for st in W.func_sites.get(('', twin_name), ())):
                    twin = twin_name
        fm_sym = ''
        if addr and addr in fm_by_addr:
            fm_sym = ' | '.join(s for (s, st, ev) in fm_by_addr[addr][:2])
        tcheck = ''
        if kind in ('member function', 'free function'):
            fm_parts = []
            if addr and addr in fm_by_addr:
                for (s, st, ev) in fm_by_addr[addr]:
                    fm_parts.extend(symbol_parts(s))
                fm_parts = [(c, f) for (c, f) in fm_parts if not is_june_name(f)]
            if twin:
                if not addr or addr not in fm_by_addr:
                    tcheck = 'unmapped'
                elif any(f == twin for (c, f) in fm_parts):
                    tcheck = 'yes'
                elif fm_parts:
                    tcheck = 'conflict'
                else:
                    tcheck = 'unverified'
            else:
                if fm_parts and any((c, f) in W.func_sites or (W.short(c), f) in W.func_sites for (c, f) in fm_parts):
                    tcheck = 'fm-only'
                else:
                    tcheck = 'no-twin' if (addr and addr in fm_by_addr) or twin_name else 'unmapped'
        n2 = ''
        if RX_CHTAG_ALIAS.match(nm):
            n2 = 'phase-tag'
        elif RX_ADDR_NAME.search(nm) and re.search('[a-f]', RX_ADDR_NAME.search(nm).group(0)):
            n2 = 'address-named'
        elif not RX_JUNE.search(nm):
            n2 = 'rule-form' if (kind == 'namespace' and re.match(r'^_CONST_\w+_CH\w+_$', nm)) else 'phase-tag'
        if kind in ('member function',) and scope and RX_JUNE.search(scope.split('::')[-1]) and                 (nm == scope.split('::')[-1] or nm == '~' + scope.split('::')[-1]):
            n2 = 'ctor-dtor-of-june-class'
        note2 = ';'.join(x for x in (n2, note) if x)
        out.append([nm, kind, rel, ln, scope, func, flag, addr, twin, tcheck, fm_sym, defs_n[nm], uses, note2])
    out.sort(key=lambda r: (r[1], r[2], r[3], r[0]))

    # ---------------- callees
    agg = collections.OrderedDict()
    bodies = 0
    class_names = set(W.short(c) for c in W.bases)
    excluded = {'ctor': set(), 'external': set(), 'external_sites': 0}
    for r in results:
        bodies += sum(1 for b in r['bodies'] if b[2])
        for (name, via, qual, encl_cls, encl_fn, reason), (cnt, lines) in r['calls'].items():
            if name in KEYWORDS or name in NOT_CALL:
                continue
            if name in class_names:
                if reason == 'fn':
                    excluded['ctor'].add(name)
                continue                                   # a constructor call
            rs = W.resolve(name, via, qual, encl_cls)
            if rs is None:
                if reason == 'fn':
                    excluded['external'].add(name)
                    excluded['external_sites'] += cnt
                continue
            owner, ncand = rs
            if name in W.members.get(owner, ()) and (owner, name) not in W.func_sites:
                continue
            k = (owner, name)
            a = agg.get(k)
            if a is None:
                a = {'n': ncand, 'sites': 0, 'files': set(), 'via': set(), 'first': [], 'fn': 0, 'flag': 0}
                agg[k] = a
            a['sites'] += cnt
            a[reason] += cnt
            a['files'].add(r['rel'])
            a['via'].add(via)
            for ln in lines:
                if len(a['first']) < 4:
                    a['first'].append('%s:%d' % (r['rel'], ln))
    crow = []
    for (owner, name), a in agg.items():
        sites = []
        for c in (owner.split('|') if owner else ['']):
            sites.extend(W.func_sites.get((c, name), []))
        origin = 'march' if any(s[3] == '' for s in sites) else ('ours' if sites else 'march')
        # funcmap rows
        want = set(owner.split('|')) if owner else set()
        rel = set()
        for c in list(want):
            rel.add(W.short(c))
            for x in W.ancestors(c) + list(W.children.get(c, ())):
                rel.add(W.short(x))
        hits = []
        for (c, va, sym, status, ev) in fm_by_fn.get(name, []):
            if is_june_name(name):
                continue
            if not owner or W.short(c) in rel or not want:
                hits.append((va, sym, status, ev))
        hint = ''
        for (va, sym, status, ev) in hits:
            h = hint_of(sym, ev) if not re.search(r'(?i)^\s*(?:=|same)', status) else ''
            if h:
                hint = h
                break
        fstat = 'none' if not hits else (('hint: ' + hint) if hint else 'same')
        crow.append([name, owner, a['n'], origin, a['sites'], a['fn'], a['flag'], len(a['files']), ','.join(sorted(a['via'])),
                     ' '.join(h[0].split()[0] for h in hits[:6]), fstat, hint, ' '.join(a['first'])])
    crow.sort(key=lambda r: (-r[4], r[0]))
    W.excluded = {'ctor_names': len(excluded['ctor']), 'external_names': len(excluded['external']),
                  'external_sites': excluded['external_sites']}
    return out, crow, bodies, W


NAMES_HDR = ['name', 'kind', 'file', 'line', 'scope', 'func', 'flag', 'june_addr', 'twin', 'twin_check', 'funcmap_march', 'defs', 'uses', 'note']
CALL_HDR = ['callee', 'owner', 'owner_n', 'origin', 'sites', 'sites_fn', 'sites_flag', 'files', 'via', 'funcmap_rows', 'funcmap_status', 'hint', 'first_sites']


def write_tsv(path, hdr, rows):
    with open(path, 'w', encoding='utf-8', newline='') as f:
        f.write('\t'.join(hdr) + '\n')
        for r in rows:
            f.write('\t'.join(str(x).replace('\t', ' ').replace('\n', ' ') for x in r) + '\n')


def read_tsv(path):
    with open(path, encoding='utf-8', newline='') as f:
        rd = csv.reader(f, delimiter='\t')
        hdr = next(rd)
        return hdr, [dict(zip(hdr, r)) for r in rd]


# ----------------------------------------------------------------------------------------------------------------------
# reports
# ----------------------------------------------------------------------------------------------------------------------
def fnclass_label(scope):
    return scope.split('::')[0]


RX_PREFIX = re.compile(r'^(?:June_|June[A-Z0-9]|JUNE_)')


def summary(names, callees, meta=None):
    meta = meta or {}
    L = []
    P = L.append

    def by(rows, key, top=14):
        c = collections.Counter(key(r) for r in rows)
        return ', '.join('%s %d' % (k, v) for k, v in c.most_common(top))

    def dedupe(rows):
        seen, out = set(), []
        for r in rows:
            k = (r['scope'], r['name'])
            if k not in seen:
                seen.add(k)
                out.append(r)
        return out
    P('JUNE CENSUS - names.tsv %d rows, callees.tsv %d rows (a "row" is one definition)' % (len(names), len(callees)))
    P('')
    P('| What | This tool | Plan Context |')
    P('|---|---|---|')
    mf_all = [r for r in names if r['kind'] == 'member function' and 'decl' not in r['note'] and not r['flag'].startswith('!')]
    mf_all = [r for r in mf_all if 'ctor-dtor-of-june-class' not in r['note']]
    mf_all = dedupe(mf_all)
    pref = [r for r in mf_all if RX_PREFIX.match(r['name'])]
    pref_cpp = [r for r in pref if not r['file'].endswith('.h')]
    pref_h = [r for r in pref if r['file'].endswith('.h')]
    P('| June-prefixed member functions, distinct (class, name) | %d (%d defined in .cpp + %d inline in a header) | ~1,505 |' % (len(pref), len(pref_cpp), len(pref_h)))
    P('|   .cpp definitions by class | %s | Eve 568, Aisha 453, X2GUUser 108, adapter 37, X2GameUnit 36, quest 35, skill-tree UI 35, DamageEffect 27, X2Game 23, emblem 20, X2Main 20, X2SkillTree 14, KTDXApp 14 |' % by(pref_cpp, lambda r: fnclass_label(r['scope'])))
    P('| any other June-named member function (June in the middle, e.g. `AddJunePvpReferData_LUA`) | %d | - |' % (len(mf_all) - len(pref)))
    tw = [r for r in pref if r['twin']]
    tw_cpp = [r for r in pref_cpp if r['twin']]
    P('| ... prefixed with a March twin in the same class (.cpp / all) | %d / %d | ~604 |' % (len(tw_cpp), len(tw)))
    P('|   by class (.cpp) | %s | Eve 373, Aisha 200, quest 17, X2GUUser 9 |' % by(tw_cpp, lambda r: fnclass_label(r['scope'])))
    P('| ... twin_check yes / conflict / unverified / unmapped | %s | - |' % ' / '.join(str(sum(1 for r in tw if r['twin_check'] == k)) for k in ('yes', 'conflict', 'unverified', 'unmapped')))
    P('| ... prefixed without a twin (.cpp / all) | %d / %d | ~900 |' % (len(pref_cpp) - len(tw_cpp), len(pref) - len(tw)))
    ff = [r for r in names if r['kind'] == 'free function' and 'decl' not in r['note']]
    P('| June-named free functions (in `_CONST_` namespaces or file scope) | %d | - |' % len(dedupe(ff)))
    dm = [r for r in names if r['kind'] == 'data member' and RX_JUNE.search(r['name'])]
    hm = meta.get('hdr_member_mentions', {})
    P('| data members with June: distinct definitions / `m_*June*` occurrences in headers | %d / %d | ~700 mentions |' % (len(dm), sum(hm.values())))
    P('|   occurrences by header | %s | X2GUUser.h 87, X2GameUnit.h 74, X2GUNPC.h 48, X2GuEve.h 47 |' % ', '.join(
        '%s %d' % (k, v) for k, v in sorted(hm.items(), key=lambda kv: -kv[1])[:8]))
    loc = [r for r in names if r['kind'] == 'local']
    locn = set(r['name'] for r in loc)
    tp = set(n for n in locn if re.match(r'^[a-z]{1,6}June[A-Z0-9_]', n))
    P('| locals and parameters with June: distinct names / definitions / uses | %d / %d / %d | ~400 distinct, ~2,150 uses |' % (len(locn), len(loc), sum(int(r['uses']) for r in loc)))
    P('|   ... of which a type prefix then June (`bJuneMemo`): distinct / their uses | %d / %d | (the Context definition) |' % (
        len(tp), sum(int(r['uses']) for r in loc if r['name'] in tp)))
    cons = [r for r in names if r['kind'] == 'constant' and 'event id' not in r['note']]
    cn = {}
    for r in cons:
        cn.setdefault(r['name'], r)
    alias = [r for r in cons if 'phase-tag' in r['note']]
    aliasn = set(r['name'] for r in alias)
    P('| JUNE constants, not event ids (distinct names / phase-tag aliases) | %d / %d | ~364 / 195 |' % (len(cn), len(aliasn)))
    P('|   phase-tag aliases: definitions + uses, comments excluded | %d + %d = %d | 545 "uses" (a raw text count: %s occurrences incl. comments) |' % (
        len(alias), sum(int(cn[n]['uses']) for n in aliasn), len(alias) + sum(int(cn[n]['uses']) for n in aliasn), meta.get('alias_text_occ', '?')))
    P('| event ids and packet structs named June (`EGS_JUNE_*`, `KEGS_JUNE_*`, shared headers) | %d | not in Context |' % len(set(r['name'] for r in names if 'event id' in r['note'] or 'packet' in r['note'])))
    P('| address-named identifiers | %d | 1 |' % len([r for r in names if 'address-named' in r['note']]))
    nsr = [r for r in names if r['kind'] == 'namespace']
    P('| namespaces: `_CONST_*_JUNE_*_` / other June-named / `_CONST_*_CH*_` (rule form, no June) | %d / %d / %d | 29 |' % (
        len([r for r in nsr if r['name'].startswith('_CONST_') and RX_JUNE.search(r['name'])]),
        len([r for r in nsr if not r['name'].startswith('_CONST_') and RX_JUNE.search(r['name'])]),
        len([r for r in nsr if 'rule-form' in r['note']])))
    P('| Lua-bound names with June | %d | 10 |' % len([r for r in names if r['kind'] == 'lua name']))
    cl = [r for r in names if r['kind'] == 'class' and 'packet' not in r['note']]
    gfx = [r for r in cl if re.search(r'(?i)junegfx|^JUNE_GFX', r['name'])]
    P('| June-named classes and enums: GFx runtime (`JuneGfx*`) / others | %d / %d | `CX2UIQuestNewJune` only |' % (len(gfx), len(cl) - len(gfx)))
    P('| June-named source files | %d | `X2UIQuestNewJune.cpp` |' % len([r for r in names if r['kind'] == 'file']))
    P('')
    fn_any = [c for c in callees if int(c['sites_fn']) > 0]
    fn_mar = [c for c in fn_any if c['origin'] == 'march']
    all_mar = [c for c in callees if c['origin'] == 'march']
    P('| What | This tool | Plan Context |')
    P('|---|---|---|')
    P('| June-named function bodies | %s | ~1,500 |' % meta.get('bodies', '?'))
    P('| distinct functions called from June-named bodies: any origin (march + ours) | %d (%d + %d) | ~1,300 |' % (
        len(fn_any), len(fn_mar), len(fn_any) - len(fn_mar)))
    P('| ... call sites in June-named bodies | %d | ~15,000 |' % sum(int(c['sites_fn']) for c in fn_any))
    ex = meta.get('excluded', {})
    P('| ... names not counted: no project definition (CRT, D3DX, boost, lua_tinker) / constructor-call names | %s / %s (%s call sites) | - |' % (
        ex.get('external_names', '?'), ex.get('ctor_names', '?'), ex.get('external_sites', '?')))
    P('| plus the live branches of defined June flags: distinct march-origin callees overall / their call sites | %d / %d | - |' % (
        len(all_mar), sum(int(c['sites']) for c in all_mar)))
    for label, grp in (('June-named bodies, any origin', fn_any), ('June-named bodies, march origin', fn_mar), ('all June code, march origin', all_mar)):
        withrow = [c for c in grp if c['funcmap_status'] != 'none']
        hint = [c for c in grp if c['funcmap_status'].startswith('hint')]
        P('| ... %s: with a funcmap row / hinting a difference / never compared | %d / %d / %d | 453 / 93 / 847 |' % (label, len(withrow), len(hint), len(grp) - len(withrow)))
    unres = [r for r in names if r['kind'] == 'unresolved']
    if unres:
        P('')
        P('unresolved (used, no definition found in the scanned tree): %d, first: %s' % (len(unres), ' '.join(r['name'] for r in unres[:12])))
    return '\n'.join(L)


def cmd_check_twins(names):
    rows = [r for r in names if r['kind'] in ('member function', 'free function', 'data member') and r['twin']]
    print('%d June-named definitions have a twin in the same class (merge candidates)' % len(rows))
    print('twin_check: yes = funcmap names the twin, conflict = funcmap maps the June address to another March function, '
          'unverified = funcmap row names no March function, unmapped = no address or no funcmap row')
    print('june name\ttwin\tclass\tkind\tjune_addr\ttwin_check\tfuncmap_march\tfile:line')
    for r in sorted(rows, key=lambda r: (r['scope'], r['name'])):
        print('\t'.join([r['name'], r['twin'], r['scope'], r['kind'], r['june_addr'], r['twin_check'], r['funcmap_march'][:60],
                         '%s:%s' % (r['file'], r['line'])]))


def cmd_file(path, names, callees, results_note=None):
    rel = os.path.relpath(os.path.abspath(path), TRUNK).replace('\\', '/')
    rows = [r for r in names if r['file'] == rel]
    print('%d name rows in %s' % (len(rows), rel))
    for r in rows:
        print('\t'.join([r['line'], r['kind'], r['name'], r['scope'], r['func'], r['flag'], r['june_addr'], r['twin'], r['twin_check']]))
    cs = [c for c in callees if any(s.startswith(rel + ':') for s in c['first_sites'].split())]
    print('')
    print('%d callees with a first site in %s (callees.tsv keeps the first 4 sites)' % (len(cs), rel))
    for c in cs:
        print('\t'.join([c['callee'], c['owner'], c['origin'], c['sites'], c['funcmap_status']]))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--summary', action='store_true', help='print the census table (default action)')
    ap.add_argument('--check-twins', action='store_true')
    ap.add_argument('--file', metavar='PATH')
    ap.add_argument('--gate', action='store_true', help='exit 1 while any June-named identifier remains (plan C5 / D2 gate)')
    ap.add_argument('--no-scan', action='store_true', help='read the last names.tsv / callees.tsv instead of scanning')
    ap.add_argument('--rescan', action='store_true', help='drop the per-file cache first')
    ap.add_argument('--jobs', type=int, default=0)
    a = ap.parse_args()
    npath = os.path.join(OUT_DIR, 'names.tsv')
    cpath = os.path.join(OUT_DIR, 'callees.tsv')
    meta = os.path.join(OUT_DIR, 'census_meta.json')
    bad = []
    if a.no_scan and os.path.exists(npath) and os.path.exists(cpath):
        _, nn = read_tsv(npath)
        _, cc = read_tsv(cpath)
        mt = json.load(open(meta)) if os.path.exists(meta) else {}
    else:
        os.makedirs(OUT_DIR, exist_ok=True)
        os.makedirs(CACHE_DIR, exist_ok=True)
        if a.rescan:
            for f in os.listdir(CACHE_DIR):
                os.remove(os.path.join(CACHE_DIR, f))
        defs = defined_flags()
        files = source_files()
        jobs = a.jobs or os.cpu_count() or 4
        sys.stderr.write('scanning %d files with %d workers (%d IRUHADEV flags defined)\n' % (len(files), jobs, len(defs)))
        with multiprocessing.Pool(jobs, initializer=worker_init, initargs=(defs,)) as pool:
            results = pool.map(scan_one, files, chunksize=4)
        bad = [r for r in results if r.get('error')]
        for r in bad:
            sys.stderr.write('SCAN ERROR %s: %s\n' % (r['rel'], r['error']))
        names, callees, bodies, W = build(results, load_funcmap())
        write_tsv(npath, NAMES_HDR, names)
        write_tsv(cpath, CALL_HDR, callees)
        hm = collections.Counter()
        for r in results:
            if r['rel'].endswith('.h'):
                for nm, c in r['occ'].items():
                    if re.match(r'^m_\w*[Jj]une', nm):
                        hm[os.path.basename(r['rel'])] += c
        alias_names = set(r[0] for r in names if r[1] == 'constant' and 'phase-tag' in str(r[13]))
        alias_text = sum(c for r in results for nm, c in r['occ'].items() if nm in alias_names)
        mt = {'bodies': bodies, 'hdr_member_mentions': hm, 'alias_text_occ': alias_text, 'excluded': W.excluded}
        with open(meta, 'w') as f:
            json.dump(mt, f)
        nn = [dict(zip(NAMES_HDR, [str(x) for x in r])) for r in names]
        cc = [dict(zip(CALL_HDR, [str(x) for x in r])) for r in callees]
    if a.check_twins:
        cmd_check_twins(nn)
    elif a.file:
        cmd_file(a.file, nn, cc)
    else:
        print(summary(nn, cc, mt))
    remain = [r for r in nn if 'rule-form' not in r['note']]
    if a.gate:
        print('GATE: %d June-named identifiers remain (names.tsv rows, rule-form namespaces excluded)' % len(remain))
        return 1 if remain or bad else 0
    return 1 if bad else 0


if __name__ == '__main__':
    multiprocessing.freeze_support()
    try:
        rc = main()
        sys.stdout.flush()
    except BrokenPipeError:             # `| head`
        rc = 0
        try:
            sys.stdout = None
        except Exception:
            pass
    sys.exit(rc)
