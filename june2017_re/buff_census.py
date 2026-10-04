"""June 2017 buff/debuff port census (JUNE_BUFF_PORT_PHASES.md, Phase 0).

    python june2017_re/buff_census.py                 # full report
    python june2017_re/buff_census.py --phase 5       # only the types/keys phase 5 owns
    python june2017_re/buff_census.py --no-probe      # skip the cl /EP flag probe (uses the cached flag table)
    python june2017_re/buff_census.py --json out.json # also dump the raw numbers

What it measures (the "exit metric" every buff phase uses): what June's buff data uses that
March's C++ cannot handle, with entry counts, plus the enum diff.

  March side  X2Lib/X2Buff*.cpp  CASE_* macro lists (preprocessor-aware: #ifdef/#else resolved with the
              US_SERVICE flag states probed by flagprobe.py, cached in buff_census_flags.tsv) and the
              quoted key literals; X2Lib/X2BuffFactor.h enums; march2014/data036/Enum.src.lua.
  June side   june_src_fixed/ ONLY (never raw bytecode or a fresh decompile): data036_kom BattleFieldBuff*
              (+ _INT), UnitApplyBuffGroup, BuffTemplet (+ _INT), Enum, and every inline BUFF_FACTOR block
              in every *.src.lua under june_src_fixed.

March's live BattleFieldBuff*.lua exist only as bytecode, so "March supports" always means the C++.
Run from anywhere; paths are resolved from this file. Output is plain ASCII.
"""
import argparse, collections, json, os, re, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)
X2LIB = os.path.join(TRUNK, 'X2Lib')
JUNE = os.path.join(TRUNK, 'june_src_fixed')
J036 = os.path.join(JUNE, 'data036_kom')
MARCH_ENUM = os.path.join(TRUNK, 'march2014', 'data036', 'Enum.src.lua')
FLAGS_TSV = os.path.join(HERE, 'buff_census_flags.tsv')

# --------------------------------------------------------------------------- Lua parsing
_TOK = re.compile(r'''
  (?P<ws>\s+|--\[(?P<eq>=*)\[.*?\](?P=eq)\]|--[^\n]*)
 |(?P<str>"(?:[^"\\\n]|\\.)*"|'(?:[^'\\\n]|\\.)*')
 |(?P<num>0[xX][0-9a-fA-F]+|\d+\.?\d*(?:[eE][+-]?\d+)?|\.\d+)
 |(?P<name>[A-Za-z_][A-Za-z0-9_]*)
 |(?P<p>\.\.\.|\.\.|==|~=|<=|>=|[{}()\[\],;:=.+\-*/%^#<>])
''', re.X | re.S)


def tokenize(text):
    """-> (kinds, texts). kinds: 's' string, 'n' number, 'i' identifier, 'p' punctuation."""
    kinds, texts = [], []
    for m in _TOK.finditer(text):
        k = m.lastgroup
        if k in ('ws', 'eq'):
            continue
        kinds.append({'str': 's', 'num': 'n', 'name': 'i', 'p': 'p'}[k])
        texts.append(m.group(k))
    return kinds, texts


class Table(object):
    """A parsed Lua table constructor: items = [(key|None, value)], value is Table or an expression string."""
    __slots__ = ('items',)

    def __init__(self, items):
        self.items = items

    def keys(self):
        return [k for k, _ in self.items if k is not None]

    def get(self, key, default=None):
        for k, v in self.items:
            if k == key:
                return v
        return default

    def positional(self):
        return [v for k, v in self.items if k is None]


def _parse_value(kinds, texts, i):
    if texts[i] == '{' and kinds[i] == 'p':
        return _parse_table(kinds, texts, i)
    depth, start = 0, i
    while True:
        t, k = texts[i], kinds[i]
        if k == 'p':
            if t in '([{':
                depth += 1
            elif t in ')]':
                depth -= 1
            elif t == '}':
                if depth == 0:
                    break
                depth -= 1
            elif t in ',;' and depth == 0:
                break
        i += 1
    return ''.join(texts[start:i]), i


def _parse_table(kinds, texts, i):
    assert texts[i] == '{'
    i += 1
    items = []
    while not (kinds[i] == 'p' and texts[i] == '}'):
        if kinds[i] == 'p' and texts[i] in ',;':
            i += 1
            continue
        key = None
        if kinds[i] == 'i' and texts[i + 1] == '=' and kinds[i + 1] == 'p':
            key = texts[i]
            i += 2
        elif kinds[i] == 'p' and texts[i] == '[':
            j = i + 1
            while not (texts[j] == ']' and kinds[j] == 'p'):
                j += 1
            if texts[j + 1] == '=':
                key = '[' + ''.join(texts[i + 1:j]) + ']'
                i = j + 2
        val, i = _parse_value(kinds, texts, i)
        items.append((key, val))
    return Table(items), i + 1


def parse_enum_file(path):
    """Top-level `NAME = { A = 1, B = 2 }` assignments -> {table: OrderedDict(member -> int)}."""
    kinds, texts = tokenize(open(path, 'rb').read().decode('latin1'))
    out, i, n = collections.OrderedDict(), 0, len(texts)
    while i < n - 2:
        if kinds[i] == 'i' and texts[i + 1] == '=' and texts[i + 2] == '{' and (i == 0 or texts[i - 1] not in '.:'):
            name = texts[i]
            tab, i = _parse_table(kinds, texts, i + 2)
            members = collections.OrderedDict()
            for k, v in tab.items:
                if k is None or isinstance(v, Table):
                    continue
                try:
                    members[k] = int(v, 0) if re.match(r'^-?(0[xX][0-9a-fA-F]+|\d+)$', v) else int(float(v))
                except ValueError:
                    members[k] = v        # non-numeric: kept as text, reported by the caller
            out[name] = members
        else:
            i += 1
    return out


BUFF_CALLS = ('AddBuffTemplet_LUA', 'AddBuffFactor_LUA', 'AddBuffGroup_LUA', 'AddOfficiallyBuffList_LUA',
              'AddBuffInfo')


def scan_lua(path):
    """-> dict(templets=[Table], factors=[Table], groups=[Table], official=[Table], premium=[Table],
    inline=[Table])  Inline = a keyed `BUFF_FACTOR = {...}` block that is not part of an AddBuffFactor_LUA call."""
    text = open(path, 'rb').read().decode('latin1')
    return scan_text(text)


def scan_text(text):
    out = dict(templets=[], factors=[], groups=[], official=[], premium=[], inline=[])
    bucket = {'AddBuffTemplet_LUA': 'templets', 'AddBuffFactor_LUA': 'factors', 'AddBuffGroup_LUA': 'groups',
              'AddOfficiallyBuffList_LUA': 'official', 'AddBuffInfo': 'premium'}
    kinds, texts = tokenize(text)
    i, n = 0, len(texts)
    while i < n - 3:
        t = texts[i]
        if kinds[i] == 'i':
            if t in bucket and texts[i + 1] == '(' and texts[i + 2] == '{':
                tab, i = _parse_table(kinds, texts, i + 2)
                out[bucket[t]].append(tab)
                continue
            if t == 'BUFF_FACTOR' and texts[i + 1] == '=' and texts[i + 2] == '{':
                tab, i = _parse_table(kinds, texts, i + 2)
                if tab.keys():
                    out['inline'].append(tab)
                continue
        i += 1
    return out


# --------------------------------------------------------------------------- C++ preprocessing
_FLAGS_CACHE = None


def load_flag_cache():
    global _FLAGS_CACHE
    if _FLAGS_CACHE is None:
        _FLAGS_CACHE = {}
        if os.path.exists(FLAGS_TSV):
            for l in open(FLAGS_TSV):
                p = l.split()
                if len(p) == 2:
                    _FLAGS_CACHE[p[0]] = (p[1] == 'ON')
    return _FLAGS_CACHE


def strip_comments(s):
    out, i, n, mode = [], 0, len(s), None
    while i < n:
        c = s[i]
        if mode is None:
            if c == '/' and i + 1 < n and s[i + 1] == '/':
                while i < n and s[i] != '\n':
                    i += 1
                continue
            if c == '/' and i + 1 < n and s[i + 1] == '*':
                i += 2
                while i + 1 < n and not (s[i] == '*' and s[i + 1] == '/'):
                    if s[i] == '\n':
                        out.append('\n')
                    i += 1
                i += 2
                continue
            if c == '"':
                mode = '"'
            out.append(c)
        else:
            out.append(c)
            if c == '\\' and i + 1 < n:
                out.append(s[i + 1])
                i += 1
            elif c == mode:
                mode = None
        i += 1
    return ''.join(out)


_DIR = re.compile(r'^\s*#\s*(ifdef|ifndef|if|elif|else|endif|define|undef|include|pragma)\b(.*)$')


def conditional_flags(path):
    """Every identifier used in a conditional directive of this file."""
    flags = set()
    for l in strip_comments(open(path, 'rb').read().decode('latin1')).split('\n'):
        m = _DIR.match(l)
        if m and m.group(1) in ('ifdef', 'ifndef', 'if', 'elif'):
            flags.update(re.findall(r'[A-Za-z_]\w*', m.group(2).replace('defined', ' ')))
    return flags - {'defined'}


def active_text(path, unknown):
    """Preprocessor-lite: resolve #ifdef/#ifndef/#if/#else/#endif with the probed flag states, drop #define bodies
    (macro bodies hold `case type_:`, not real cases). Flags absent from the cache are treated OFF and recorded."""
    flags = load_flag_cache()

    def isdef(f):
        if f not in flags:
            unknown.add(f)
        return flags.get(f, False)

    def ev(expr):
        e = expr.strip()
        e = re.sub(r'defined\s*\(\s*(\w+)\s*\)', lambda m: '1' if isdef(m.group(1)) else '0', e)
        e = re.sub(r'defined\s+(\w+)', lambda m: '1' if isdef(m.group(1)) else '0', e)
        e = re.sub(r'\b[A-Za-z_]\w*\b', lambda m: '1' if isdef(m.group(0)) else '0', e)
        e = e.replace('&&', ' and ').replace('||', ' or ').replace('!', ' not ')
        try:
            return bool(eval(e, {}, {}))
        except Exception:
            unknown.add('#if ' + expr.strip())
            return False

    lines = strip_comments(open(path, 'rb').read().decode('latin1')).split('\n')
    out, stack, cont = [], [], False       # stack entries: [parent_active, this_branch_taken, active_now]
    for l in lines:
        if cont:                            # continuation of a #define
            cont = l.rstrip().endswith('\\')
            out.append('')
            continue
        m = _DIR.match(l)
        act = all(s[2] for s in stack)
        if m:
            d, rest = m.groups()
            if d in ('ifdef', 'ifndef', 'if'):
                v = isdef(rest.split()[0]) if d == 'ifdef' else (not isdef(rest.split()[0]) if d == 'ifndef' else ev(rest))
                par = act
                stack.append([par, v, par and v])
            elif d == 'elif':
                s = stack[-1]
                v = (not s[1]) and ev(rest)
                s[1] = s[1] or v
                s[2] = s[0] and v
            elif d == 'else':
                s = stack[-1]
                s[2] = s[0] and not s[1]
                s[1] = True
            elif d == 'endif':
                stack.pop()
            elif d in ('define', 'undef') and l.rstrip().endswith('\\'):
                cont = True
            out.append('')
            continue
        out.append(l if act else '')
    return '\n'.join(out)


def run_flag_probe(flags):
    """Resolve flags through the real US_SERVICE preprocessor (flagprobe.py, ~45 s). Returns {flag: bool}."""
    r = subprocess.run([sys.executable, 'flagprobe.py'] + sorted(flags), cwd=HERE, capture_output=True, text=True)
    res = {}
    for l in r.stdout.splitlines():
        p = l.split()
        if len(p) == 2 and p[0] in ('ON', 'OFF'):
            res[p[1]] = (p[0] == 'ON')
    return res


# --------------------------------------------------------------------------- March C++ model
CPP_FILES = ['X2BuffFactor.cpp', 'X2BuffBehaviorTemplet.cpp', 'X2BuffFinalizerTemplet.cpp',
             'X2BuffDisplayerTemplet.cpp', 'X2BuffTemplet.cpp', 'X2BuffTempletManager.cpp']
CPP_HEADERS = ['X2BuffFactor.h', 'X2BuffBehaviorTemplet.h', 'X2BuffFinalizerTemplet.h',
               'X2BuffDisplayerTemplet.h', 'X2BuffTemplet.h', 'X2BuffTempletManager.h']
_CASE = re.compile(r'\bCASE_(BEHAVIOR|FINALIZER|DISPLAYER)_(FACTOR|TEMPLET)(?:_SKILL)?\s*\(\s*(\w+)\s*,\s*(\w+)')
_PLAIN_CASE = re.compile(r'\bcase\s+(B[BFD]T_\w+)\s*:')
TEMPLET_METHOD = {'BEHAVIOR': 'ParsingBehavior', 'FINALIZER': 'ParsingFinalizer', 'DISPLAYER': 'ParsingDisplayer'}
_STR = re.compile(r'(?<![A-Za-z0-9_])L?"([A-Z][A-Z0-9_]*(?:%d[A-Z0-9_]*)*)"')


def body_of(text, marker):
    """Text of the brace block that follows the first occurrence of `marker`."""
    s = text.find(marker)
    if s < 0:
        return ''
    b = text.find('{', s)
    depth, i = 0, b
    while i < len(text):
        if text[i] == '{':
            depth += 1
        elif text[i] == '}':
            depth -= 1
            if depth == 0:
                return text[b:i + 1]
        i += 1
    return text[b:]


def parse_cpp_enums(text):
    """enum NAME { A, B = 5, C } -> {NAME: OrderedDict}; resolves `X = Y + n` against earlier members."""
    out = collections.OrderedDict()
    for m in re.finditer(r'\benum\s+(\w+)\s*\{(.*?)\}\s*;', text, re.S):
        members, nxt = collections.OrderedDict(), 0
        for part in m.group(2).split(','):
            part = part.strip()
            if not part:
                continue
            mm = re.match(r'^(\w+)\s*(?:=\s*(.+))?$', part, re.S)
            if not mm:
                continue
            name, expr = mm.groups()
            if expr is not None:
                e = expr.strip()
                e = re.sub(r'\b[A-Za-z_]\w*\b', lambda x: str(members[x.group(0)]) if x.group(0) in members else x.group(0), e)
                try:
                    nxt = int(eval(e, {}, {}))
                except Exception:
                    nxt = None
            if nxt is None:
                members[name] = '?'
                nxt = None
                continue
            members[name] = nxt
            nxt += 1
        out[m.group(1)] = members
    return out


_QLIT = re.compile(r'(?<![A-Za-z0-9_])L?"([A-Za-z_][A-Za-z0-9_%]*)"')
_DEF_CACHE = {}


def lit_supported(key, lits):
    """True if `key` is one of the literals, or matches a printf-style key literal such as "SKILL_CATEGORY_%d"."""
    if key in lits:
        return True
    for l in lits:
        if '%' in l and re.match('^' + re.escape(l).replace('%d', r'\d+').replace('%s', r'.+') + '$', key):
            return True
    return False


def find_def(text, qual, name):
    """Body of the definition `qual::name( ... ) { ... }` (calls are skipped: a definition is followed by `{` or `:`)."""
    for m in re.finditer(r'\b' + re.escape(qual) + r'::' + re.escape(name) + r'\s*\(', text):
        i, depth = m.end(), 1
        while i < len(text) and depth:
            depth += {'(': 1, ')': -1}.get(text[i], 0)
            i += 1
        j = i
        while j < len(text) and text[j].isspace():
            j += 1
        if text.startswith('const', j):
            j += 5
            while j < len(text) and text[j].isspace():
                j += 1
        if j < len(text) and text[j] in '{:':
            return body_of(text[m.start():], '{')
    return None


def class_bases(header_texts):
    """{class: [direct public bases]} from `class A : public B, public C`."""
    out = {}
    for t in header_texts:
        for m in re.finditer(r'\b(?:class|struct)\s+(\w+)\s*:\s*([^{;]*)\{', t):
            out[m.group(1)] = re.findall(r'\bpublic\s+(\w+)', m.group(2))
    return out


def reader_literals(ctx, qual, name, depth=3, seen=None):
    """Quoted key literals read by `qual::name` (looked up through qual's base classes if it does not define it) and by
    the functions it calls: `Q::f(` resolves in Q, an unqualified `f(` in the caller's class chain, `obj.f(`/`p->f(` is
    skipped. ctx = dict(texts=[...], bases={...}). Depth-limited."""
    seen = seen if seen is not None else set()
    if (qual, name) in seen or depth < 0:
        return set()
    seen.add((qual, name))
    chain, todo = [], [qual]
    while todo:
        c = todo.pop(0)
        if c not in chain:
            chain.append(c)
            todo.extend(ctx['bases'].get(c, []))
    body, owner = None, None
    for c in chain:
        key = (id(ctx['texts']), c, name)
        if key not in _DEF_CACHE:
            b = None
            for t in ctx['texts']:
                b = find_def(t, c, name)
                if b is not None:
                    break
            _DEF_CACHE[key] = b
        if _DEF_CACHE[key] is not None:
            body, owner = _DEF_CACHE[key], c
            break
    if body is None:
        return set()
    out = set(_QLIT.findall(body))
    for cq, cn in set(re.findall(r'(?<![\w.>])((?:\w+::)*)(\w+)\s*\(', body)):
        out |= reader_literals(ctx, cq[:-2].split('::')[-1] if cq else owner, cn, depth - 1, seen)
    return out


def load_march_cpp(unknown):
    m = dict(cases=collections.defaultdict(set), literals=collections.defaultdict(set), enums={}, slice={},
             custom_fn_btis=set(), fn=collections.defaultdict(dict), readers={}, plain=collections.defaultdict(set))
    texts = {}
    for f in CPP_FILES + CPP_HEADERS:
        texts[f] = active_text(os.path.join(X2LIB, f), unknown)
    for f in CPP_FILES:
        for c in _CASE.finditer(texts[f]):
            kind, role, fn, typ = c.groups()
            m['cases'][(kind, role)].add(typ)
            m['fn'][(kind, role)][typ] = fn
        for s in _STR.finditer(texts[f]):
            m['literals'][f].add(s.group(1))
    for f in CPP_HEADERS:
        for s in _STR.finditer(texts[f]):
            m['literals'][f].add(s.group(1))
    # hand-written `case BBT_EMPTY:` labels beside the CASE_* macros are handled types too
    plain_scopes = {('BEHAVIOR', 'TEMPLET'): ('X2BuffBehaviorTemplet.cpp', 'CX2BuffBehaviorTemplet::ParsingCombinationBehavior'),
                    ('BEHAVIOR', 'FACTOR'): ('X2BuffFactor.cpp', 'CX2BuffFactor::ParsingCombinationBehavior'),
                    ('FINALIZER', 'TEMPLET'): ('X2BuffFinalizerTemplet.cpp', 'CX2BuffFinalizerTemplet::ParsingCombinationFinalizer'),
                    ('FINALIZER', 'FACTOR'): ('X2BuffFactor.cpp', 'CX2BuffFactor::ParsingCombinationFinalizer'),
                    ('DISPLAYER', 'TEMPLET'): ('X2BuffDisplayerTemplet.cpp', 'CX2BuffDisplayerTemplet::ParsingCombinationDisplayer')}
    for key, (f, marker) in plain_scopes.items():
        labs = set(_PLAIN_CASE.findall(body_of(texts[f], marker)))
        m['plain'][key] = labs
        m['cases'][key] |= labs
    m['enums'] = parse_cpp_enums(texts['X2BuffFactor.h'])
    for kind, fn in (('BEHAVIOR', 'BUFF_FACTOR::GetBehaviorFactorToLevel'), ('FINALIZER', 'BUFF_FACTOR::GetFinalizerFactorToLevel')):
        m['slice'][kind] = set(re.findall(r'\bcase\s+(B[BF]T_\w+)\s*:', body_of(texts['X2BuffFactor.cpp'], fn)))
    # per-type key readers: the factor-parse function (a CX2BuffFactor member) and the templet class's virtual
    # ParsingBehavior/Finalizer/Displayer, each followed through the calls it makes
    ctx = dict(texts=[texts[f] for f in CPP_FILES], bases=class_bases([texts[f] for f in CPP_HEADERS]))
    m['ctx'] = ctx
    for kind in ('BEHAVIOR', 'FINALIZER', 'DISPLAYER'):
        for role in ('FACTOR', 'TEMPLET'):
            for typ, fn in m['fn'][(kind, role)].items():
                if role == 'FACTOR':
                    lits = reader_literals(ctx, 'CX2BuffFactor', fn)
                else:
                    lits = reader_literals(ctx, fn, TEMPLET_METHOD[kind])
                m['readers'].setdefault((kind, typ, role), set()).update(lits)
    gu = active_text(os.path.join(X2LIB, 'X2GameUnit.cpp'), unknown)
    body = body_of(gu, 'CX2GameUnit::CreateAndInsertCustomFunctionByBuff')
    m['custom_fn_btis'] = set(re.findall(r'\bcase\s+(BTI_\w+)\s*:', body))
    return m


def key_level_sets(m):
    """Per-level supported key literals, taken from the function(s) that parse that level (templet/factor top level) or from
    the files that parse it (behavior/finalizer/displayer, the level-wide view)."""
    lit, ctx = m['literals'], m['ctx']
    mgr = lit['X2BuffTempletManager.cpp']
    return {
        'templet': reader_literals(ctx, 'CX2BuffTemplet', 'ParsingScript') | mgr,
        'factor': reader_literals(ctx, 'CX2BuffFactor', 'ParsingBuffFactor') | mgr,
        'behavior': lit['X2BuffFactor.cpp'] | lit['X2BuffBehaviorTemplet.cpp'] | lit['X2BuffBehaviorTemplet.h'],
        'finalizer': lit['X2BuffFactor.cpp'] | lit['X2BuffFinalizerTemplet.cpp'] | lit['X2BuffFinalizerTemplet.h'],
        'displayer': lit['X2BuffDisplayerTemplet.cpp'] | lit['X2BuffDisplayerTemplet.h'],
    }


# --------------------------------------------------------------------------- June corpus
STRUCT_KEYS = {'COMBINATION', 'BEHAVIOR', 'FINALIZER', 'DISPLAYER', 'BUFF_FACTOR'}
BIND_SOURCES = ['X2Main.cpp', 'X2BuffTempletManager.cpp', 'X2GameUnit.cpp', 'X2GUUser.cpp', 'X2GUNPC.cpp', 'X2DamageManager.cpp']
BIND_PATTERNS = ['AddBuffGroup_LUA', 'AddOfficiallyBuffList_LUA', 'SetMonsterBuffGroupID', 'DeleteNonControlDebuff',
                 'GetBuffAccumulationCountNow', 'ApplyBuffFactor', 'AddRandomBuffList', 'SetPveBalanceBuffGiveScore_LUA',
                 'SendRandomMissionRewardBuffCheat']


def enum_member(expr, table):
    """'BUFF_BEHAVIOR_TYPE.BBT_X' -> 'BBT_X' when the prefix is `table`; else None."""
    p = table + '.'
    return expr[len(p):] if isinstance(expr, str) and expr.startswith(p) else None


def types_in(tab, section, table):
    """(types listed in COMBINATION, sub-table keys) of a BEHAVIOR/FINALIZER/DISPLAYER table."""
    sec = tab.get(section)
    if not isinstance(sec, Table):
        return [], []
    comb = sec.get('COMBINATION')
    listed = [enum_member(v, table) for v in comb.positional()] if isinstance(comb, Table) else []
    listed = [t for t in listed if t]
    subs = [k for k in sec.keys() if k != 'COMBINATION']
    return listed, subs


LEVELS = (('BEHAVIOR', 'BUFF_BEHAVIOR_TYPE', 'behavior'), ('FINALIZER', 'BUFF_FINALIZER_TYPE', 'finalizer'),
          ('DISPLAYER', 'BUFF_DISPLAYER_TYPE', 'displayer'))


def _flatten(tab, prefix=''):
    """Yield (path, key) for every key of `tab` and of the keyed tables nested in it. Positional lists are leaves."""
    for k in tab.keys():
        yield prefix + k, k
        v = tab.get(k)
        if isinstance(v, Table) and v.keys():
            for x in _flatten(v, prefix + k + '.'):
                yield x


def harvest(rec, origin, st):
    """Record one templet/factor/inline entry into the stats. `rec` is the table that carries BEHAVIOR/FINALIZER/...
    (a templet table, or the BUFF_FACTOR sub-table of a factor).
    st['keys'][(level, bare_key)]            -> entries, for the level-wide view the ledger used
    st['tkeys'][(section, type, path)]       -> entries, for the per-type view (path = KEY or PARENT.CHILD)"""
    top = 'templet' if origin == 'templet' else 'factor'
    for k in rec.keys():
        if k in STRUCT_KEYS and not (k == 'DISPLAYER' and top == 'factor'):
            continue
        st['keys'][(top, k)][origin].add(id(rec))
        v = rec.get(k)
        if isinstance(v, Table) and v.keys():                  # e.g. APPLY_CHECK_CONDITION = { CONDITION_0 = ... }
            for path, leaf in _flatten(v):
                st['nested'][(top, k)].add(leaf)
    for section, table, level in LEVELS:
        listed, subs = types_in(rec, section, table)
        for t in set(listed):
            st['types'][section][t][origin].add(id(rec))
        for t in set(subs) - set(listed):
            st['anomaly'].append((origin, section, 'table without COMBINATION entry', t))
        for t in set(listed) - set(subs):
            st['anomaly'].append((origin, section, 'COMBINATION entry without table', t))
        sec = rec.get(section)
        if not isinstance(sec, Table):
            continue
        for t in subs:
            sub = sec.get(t)
            if not isinstance(sub, Table):
                continue
            for path, leaf in _flatten(sub):
                st['keys'][(level, leaf)][origin].add(id(rec))
                st['tkeys'][(section, t, path)][origin].add(id(rec))


def new_stats():
    return dict(keys=collections.defaultdict(lambda: collections.defaultdict(set)),
                types={s: collections.defaultdict(lambda: collections.defaultdict(set)) for s, _, _ in LEVELS},
                tkeys=collections.defaultdict(lambda: collections.defaultdict(set)),
                nested=collections.defaultdict(set), anomaly=[])


def load_june():
    data = {}
    for key, fn in (('templet', 'BattleFieldBuffTemplet'), ('templet_int', 'BattleFieldBuffTemplet_INT'),
                    ('factor', 'BattleFieldBuffFactor'), ('factor_int', 'BattleFieldBuffFactor_INT'),
                    ('group', 'UnitApplyBuffGroup'), ('premium', 'BuffTemplet'), ('premium_int', 'BuffTemplet_INT')):
        data[key] = scan_lua(os.path.join(J036, fn + '.src.lua'))
    return data


def scan_corpus():
    """One pass over every *.src.lua: inline BUFF_FACTOR blocks, binding use counts, IMMUNITY_LIST uses."""
    inline = []                  # (relpath, Table)
    binds = {b: [0, set()] for b in BIND_PATTERNS}
    imm = [0, set()]
    nfiles = 0
    data_files = {fn + '.src.lua' for fn in ('BattleFieldBuffTemplet', 'BattleFieldBuffFactor', 'BattleFieldBuffTemplet_INT',
                                             'BattleFieldBuffFactor_INT', 'UnitApplyBuffGroup', 'BuffTemplet', 'BuffTemplet_INT')}
    for root, _, files in os.walk(JUNE):
        for f in files:
            if not f.endswith('.src.lua'):
                continue
            nfiles += 1
            path = os.path.join(root, f)
            rel = os.path.relpath(path, JUNE).replace('\\', '/')
            text = open(path, 'rb').read().decode('latin1')
            for b in BIND_PATTERNS:
                c = text.count(b)
                if c:
                    binds[b][0] += c
                    binds[b][1].add(rel)
            c = text.count('IMMUNITY_LIST_AT_THIS_STATE')
            if c:
                imm[0] += c
                imm[1].add(rel)
            if 'BUFF_FACTOR' in text and not (f in data_files and os.path.dirname(rel) == 'data036_kom'):
                for tab in scan_text(text)['inline']:
                    inline.append((rel, tab))
    return dict(inline=inline, binds=binds, imm=imm, nfiles=nfiles)


def load_march_data():
    """String constants of March's live buff Lua (bytecode only; no decompile exists). {file: set(strings)} or None.
    Location: $BUFF_CENSUS_KOM_CONTENTS, else the game directory's kom_contents/data036_kom (outside the repo)."""
    base = os.environ.get('BUFF_CENSUS_KOM_CONTENTS') or os.path.normpath(
        os.path.join(TRUNK, *(['..'] * 7), '237311', '24965799', 'kom_contents'))
    d = os.path.join(base, 'data036_kom')
    out = {}
    for fn in ('BattleFieldBuffTemplet.lua', 'BattleFieldBuffFactor.lua', 'BuffTemplet.lua'):
        p = os.path.join(d, fn)
        if os.path.exists(p):
            out[fn] = set(x.decode() for x in re.findall(rb'[A-Za-z_][A-Za-z_0-9]{2,}', open(p, 'rb').read()))
    return out or None


def march_data_flag(md, key, files=('BattleFieldBuffTemplet.lua', 'BattleFieldBuffFactor.lua')):
    if md is None:
        return '?'
    return 'yes' if any(key in md.get(f, ()) for f in files) else 'NO (new in June)'


# --------------------------------------------------------------------------- phases
PHASE_BBT_VALUES = {
    # grouping re-split by Phase 1 (JUNE_BUFF_PORT_PHASES.md "Per-type map"): 5 = transcription-ready
    # ChangeStat types (Sonnet), 5b = stat types needing an Opus pass, 97 moved to 7.
    '5': {76, 85, 86, 87, 96, 108, 109},
    '5b': {78, 79, 80, 81, 82, 83, 84, 88, 92, 93, 106, 107},
    '6': {75, 89, 90, 91, 98, 99, 100, 103},
    '7': {73, 74, 102, 77, 94, 95, 97, 101, 104, 105, 110, 111, 112},
}
PHASE_BFT_VALUES = {'8': {14, 15, 16, 17}}
# Types June's own client never implements: its templet factories and factor-parse switches stop at
# BBT 110 / BFT 16 (Phase 1: 32e80450 @32e804da, 32ec36f0, 32d4bbf0, 32d4c750). They are enum-only in
# the port, so they are reported apart and never count as "not handled".
NOT_IN_JUNE_CLIENT = {'BEHAVIOR': {111, 112}, 'FINALIZER': {17}}
# Keys June's own client never reads at that level, so there is nothing to port (Phase 4a). Templet
# level: absent from ParsingScript 3236a9b0's 17 keys and from all 6,240 decrypted XOR buffers
# (server-side keys, Phase 1 table). Factor level: ParsingBuffFactor 32d4d850 reads BUFF_TEMPLET_ID,
# RATE, IGNORE_REGIST_FOR_RATE, ACCUMULATION_AT_ONE_GO, APPLY_CHECK_CONDITION, BEHAVIOR, FINALIZER only;
# APPLIED_SKILL_ID is read at damage-data level (SetDamageDataFromLUA 32632446) before any BUFF_FACTOR
# table opens, so the 2 copies nested in inline BUFF_FACTOR blocks are ignored by June too.
# Reported apart, never as unread.
NOT_READ_BY_JUNE_CLIENT = {
    'templet': {'EXP_RATE', 'IS_PARTY_BUFF', 'NOT_APPLY_BUFF_WHEN_SAME_BUFF_OWNER', 'DROP_RATE', 'VP_RATE'},
    'templet_prefix': ('LUCKYTIMINGEVENT_',),
    'factor': {'DISPLAYER', 'APPLIED_SKILL_ID'},
}


# Phase 4b: keys inside a type March handles that June's OWN reader for that origin never reads, so there is
# nothing to port. Key: (section, type, origin kind), where 'factor' covers factor and inline entries (the
# factor-parse function, ParsingCombinationBehavior 32d4bbf0's case) and 'templet' the templet class's
# ParsingBehavior (vtable slot 12). Value: {key: June function}. Reported apart, never as unread.
_ATTR_RATE = ('BBT_CHANGE_ENCHANT_ATTACK_RATE', 'BBT_CHANGE_ATTRIBUTE_BLAZE_ATTACK_RATE', 'BBT_CHANGE_ATTRIBUTE_WATER_ATTACK_RATE',
              'BBT_CHANGE_ATTRIBUTE_NATURE_ATTACK_RATE', 'BBT_CHANGE_ATTRIBUTE_WIND_ATTACK_RATE',
              'BBT_CHANGE_ATTRIBUTE_LIGHT_ATTACK_RATE', 'BBT_CHANGE_ATTRIBUTE_DARK_ATTACK_RATE')
NOT_READ_BY_JUNE_TYPE = {}
for _t in _ATTR_RATE:
    NOT_READ_BY_JUNE_TYPE[('BEHAVIOR', _t, 'factor')] = {'BUFF_RELATION_TYPE': '32d46550'}
    NOT_READ_BY_JUNE_TYPE[('BEHAVIOR', _t, 'templet')] = {'BUFF_RELATION_TYPE': '32e74820'}
NOT_READ_BY_JUNE_TYPE.update({
    ('BEHAVIOR', 'BBT_CHANGE_NOW_HP_PER_SECOND', 'factor'): {'DRAW_TEXT': '32d42030', 'TIME_INTERVAL': '32d42030'},
    ('BEHAVIOR', 'BBT_CHANGE_NOW_HP_PER_SECOND', 'templet'): {'BUFF_RELATION_TYPE': '32e72a00'},
    ('BEHAVIOR', 'BBT_CHANGE_NOW_MP_PER_SECOND', 'factor'): {'BUFF_RELATION_TYPE': '32d44320'},
    ('BEHAVIOR', 'BBT_CHANGE_NOW_MP_PER_SECOND', 'templet'): {'BUFF_RELATION_TYPE': '32e72e20'},
    ('BEHAVIOR', 'BBT_DASH_IMPOSSIBLE', 'factor'): {'BUFF_RELATION_TYPE': '32d45b60 (empty parse)'},
    ('BEHAVIOR', 'BBT_DASH_IMPOSSIBLE', 'templet'): {'BUFF_RELATION_TYPE': 'Identity class, slot 12 = base 32ec5990'},
    ('BEHAVIOR', 'BBT_DOUBLE_ATTACK', 'templet'): {'BUFF_RELATION_TYPE': '32e74c20'},
    ('BEHAVIOR', 'BBT_FROZEN', 'factor'): {'IS_DOWN': '32d45b60 (empty parse)'},
    ('BEHAVIOR', 'BBT_NEVER_MOVE', 'factor'): {'OFFSET_POSITION': '32d45b60 (empty parse)', 'IS_GRAVITY': '32d45b60 (empty parse)'},
    ('BEHAVIOR', 'BBT_RESET_SKILL_COOL_TIME', 'factor'): {'SKILL_TYPE': '32d46fc0'},
    ('BEHAVIOR', 'BBT_REVERSE_LEFT_RIGHT', 'factor'): {'TIME_INTERVAL': '32d45b60 (empty parse)'},
    # Phase 4b-ii: June's only reader of the key, 32ebfd60, is the class's hiding non-virtual template method, with
    # no caller; factory 32ec36f0 case 12 runs the base template 32ec0580 and slot 12 is the base 32ec5990 (true)
    ('FINALIZER', 'BFT_RIDING_ON_OR_NOT', 'templet'): {'FINISH_WHEN_RIDING_ON': '32ec0580 -> base slot 12 32ec5990 (32ebfd60 is uncalled)'},
    ('FINALIZER', 'BFT_RIDING_ON_OR_NOT', 'factor'): {'FINISH_WHEN_RIDING_ON': '32d44230 (empty parse)'},
})


def not_read_by_june_type(section, typ, origin, key):
    kind = 'templet' if origin == 'templet' else 'factor'
    return NOT_READ_BY_JUNE_TYPE.get((section, typ, kind), {}).get(key)


def not_read_by_june(level, key):
    if key in NOT_READ_BY_JUNE_CLIENT.get(level, ()):
        return True
    return level == 'templet' and key.startswith(NOT_READ_BY_JUNE_CLIENT['templet_prefix'])
# keys by owning phase (levels: templet/factor -> 4a, behavior/finalizer/displayer -> 4b)
PHASE_KEY_LEVELS = {'4a': {'templet', 'factor'}, '4b': {'behavior', 'finalizer', 'displayer'}}


def phase_of_type(section, value):
    if section == 'BEHAVIOR':
        for p, vs in PHASE_BBT_VALUES.items():
            if value in vs:
                return str(p)
    if section == 'FINALIZER':
        for p, vs in PHASE_BFT_VALUES.items():
            if value in vs:
                return str(p)
    return '?'


# --------------------------------------------------------------------------- report
def enum_diff(june, march):
    rows = []
    for t in june:
        if t not in march:
            rows.append((t, len(june[t]), None, None, None, None))
            continue
        jm, mm = june[t], march[t]
        new = [k for k in jm if k not in mm]
        chg = [(k, mm[k], jm[k]) for k in jm if k in mm and mm[k] != jm[k]]
        gone = [k for k in mm if k not in jm]
        rows.append((t, len(jm), len(mm), new, chg, gone))
    return rows


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--phase', help='only report what this phase owns: 2,3,4a,4b,5,5b,6,7,8,9,10,11')
    ap.add_argument('--no-probe', action='store_true', help='never run the ~45 s cl /EP flag probe')
    ap.add_argument('--reprobe', action='store_true', help='re-resolve every flag through cl /EP, ignoring the cache')
    ap.add_argument('--json', help='write the raw numbers to this file')
    ap.add_argument('-v', '--verbose', action='store_true', help='list the entries behind each unsupported key')
    a = ap.parse_args()
    want = (lambda *p: a.phase is None or a.phase in p)

    # -- resolve the preprocessor flags every conditional in the March files needs
    need = set()
    for f in CPP_FILES + CPP_HEADERS + ['X2GameUnit.cpp', 'X2PremiumBuffManager.cpp'] + BIND_SOURCES:
        need |= conditional_flags(os.path.join(X2LIB, f))
    cache = load_flag_cache()
    # Our own SERV_IRUHADEV_* flags change as the port proceeds, so they are NEVER cached: a flag probed OFF before its
    # #define lands would otherwise stay OFF and a finished phase would never clear. By house rule they are plain
    # unconditional `#define`s in KTDXLIB/Always.h, so each run reads them from there (fast); --reprobe asks the compiler.
    ours = sorted(f for f in need if f.startswith('SERV_IRUHADEV_'))
    for f in ours:
        cache.pop(f, None)
    studio = sorted(f for f in need if f not in ours and (f not in cache or a.reprobe))
    if (studio or (a.reprobe and ours)) and not a.no_probe:
        todo = studio + (ours if a.reprobe else [])
        sys.stderr.write('probing %d flag(s) through cl /EP (~45 s)...\n' % len(todo))
        res = run_flag_probe(todo)
        if res:
            cache.update(res)
            with open(FLAGS_TSV, 'w') as fh:                     # persistent cache = studio flags only
                for k in sorted(cache):
                    if not k.startswith('SERV_IRUHADEV_'):
                        fh.write('%s\t%s\n' % (k, 'ON' if cache[k] else 'OFF'))
    if ours and not (a.reprobe and not a.no_probe):
        alw = open(os.path.join(TRUNK, 'KTDXLIB', 'Always.h'), 'rb').read().decode('latin1')
        for f in ours:
            cache[f] = bool(re.search(r'^\s*#\s*define\s+' + f + r'\b', alw, re.M))
    unknown = set()
    march = load_march_cpp(unknown)
    lsets = key_level_sets(march)
    march_data = load_march_data()
    pm = active_text(os.path.join(X2LIB, 'X2PremiumBuffManager.cpp'), unknown)
    march['premium_literals'] = set(_QLIT.findall(pm))

    # -- enums
    j_enum = parse_enum_file(os.path.join(J036, 'Enum.src.lua'))
    m_enum = parse_enum_file(MARCH_ENUM)
    BUFF_TABLES = [t for t in list(j_enum) + list(m_enum) if t.startswith('BUFF_') or t in ('SPA_BUFF_TYPE',)]
    BUFF_TABLES = list(collections.OrderedDict.fromkeys(BUFF_TABLES))
    jb = collections.OrderedDict((t, j_enum[t]) for t in BUFF_TABLES if t in j_enum)
    mb = collections.OrderedDict((t, m_enum[t]) for t in BUFF_TABLES if t in m_enum)

    J = load_june()
    C = scan_corpus()

    st = new_stats()
    counts = {}
    for key, origin in (('templet', 'templet'), ('templet_int', 'templet'), ('factor', 'factor'), ('factor_int', 'factor')):
        tabs = J[key]['templets'] if 'templet' in key else J[key]['factors']
        counts[key] = len(tabs)
        for tab in tabs:
            rec = tab if origin == 'templet' else tab.get('BUFF_FACTOR')
            if isinstance(rec, Table):
                harvest(rec, origin, st)
    for rel, tab in C['inline']:
        harvest(tab, 'inline', st)
    out = collections.OrderedDict()

    def P(s=''):
        print(s)

    P('BUFF CENSUS  (June = june_src_fixed, March = X2Lib C++ with US_SERVICE flags)')
    if unknown:
        P('  NOTE: conditionals with no probed state (treated OFF): %s' % ', '.join(sorted(unknown)))
    P('')

    # ---------------------------------------------------------------- data shape
    if want('3', '4a', '11'):
        P('== June data shape')
        for key in ('templet', 'templet_int', 'factor', 'factor_int', 'group', 'premium', 'premium_int'):
            d = J[key]
            n = {'templet': len(d['templets']), 'templet_int': len(d['templets']), 'factor': len(d['factors']),
                 'factor_int': len(d['factors']), 'group': len(d['groups']), 'premium': len(d['premium']),
                 'premium_int': len(d['premium'])}[key]
            P('  %-12s %5d entries' % (key, n))
        P('  official-list calls in UnitApplyBuffGroup: %d' % len(J['group']['official']))
        ids = [t.get('BUFF_FACTOR_ID') for t in J['factor']['factors']]
        fids = [enum_member(i, 'BUFF_FACTOR_ID') for i in ids]
        vals = [j_enum['BUFF_FACTOR_ID'].get(n) for n in fids]
        contiguous = vals == list(range(1, len(vals) + 1))
        P('  factor ids contiguous 1..N in file order: %s (N=%d)' % (contiguous, len(vals)))
        iv = [j_enum['BUFF_FACTOR_ID'].get(enum_member(t.get('BUFF_FACTOR_ID'), 'BUFF_FACTOR_ID')) for t in J['factor_int']['factors']]
        P('  _INT factor ids: %d entries, %s..%s' % (len(iv), min(iv) if iv else None, max(iv) if iv else None))
        tv = [j_enum['BUFF_TEMPLET_ID'].get(enum_member(t.get('BUFF_TEMPLET_ID'), 'BUFF_TEMPLET_ID')) for t in J['templet']['templets']]
        tiv = [j_enum['BUFF_TEMPLET_ID'].get(enum_member(t.get('BUFF_TEMPLET_ID'), 'BUFF_TEMPLET_ID')) for t in J['templet_int']['templets']]
        gaps = (max(tv) - min(tv) + 1) - len(set(tv))
        P('  templet ids: %d entries (%d distinct), %s..%s, sorted=%s, gaps in range=%d' % (
            len(tv), len(set(tv)), min(tv), max(tv), tv == sorted(tv), gaps))
        P('  _INT templet ids: %d entries, %s..%s' % (len(tiv), min(tiv), max(tiv)))
        have = set(tv)
        runs, st0 = [], None
        for i in range(1, max(tv) + 2):
            if i not in have and st0 is None:
                st0 = i
            if i in have and st0 is not None:
                runs.append('%d-%d' % (st0, i - 1))
                st0 = None
        P('  absent ids in 1..%d form %d run(s): %s' % (max(tv), len(runs), ', '.join(runs)))
        P('  BTI enum members (June, incl. BTI_NONE, excl. _INT) with no templet row: %d' % len(set(j_enum['BUFF_TEMPLET_ID'].values()) - have - {10001 + i for i in range(72)} - set(tiv)))
        # data rows for ids that exist in March's Lua enum but have no March data row is not knowable without a
        # March decompile (bytecode only) -- use the enum: June rows whose id is in March's enum.
        mt, mf = m_enum['BUFF_TEMPLET_ID'], m_enum['BUFF_FACTOR_ID']
        P('  June templet rows whose BTI is in March\'s Lua enum: %d / factor rows whose BFI is: %d' % (
            sum(1 for v in tv if v in set(mt.values())),
            sum(1 for v in vals if v in set(mf.values()))))
        if march_data is not None:
            jn_t = [enum_member(t.get('BUFF_TEMPLET_ID'), 'BUFF_TEMPLET_ID') for t in J['templet']['templets']]
            jn_f = [enum_member(t.get('BUFF_FACTOR_ID'), 'BUFF_FACTOR_ID') for t in J['factor']['factors']]
            P('    of those, no row in March\'s live bytecode (name not among its string constants): %d templets / %d factors' % (
                sum(1 for n in jn_t if n in m_enum['BUFF_TEMPLET_ID'] and n not in march_data['BattleFieldBuffTemplet.lua']),
                sum(1 for n in jn_f if n in m_enum['BUFF_FACTOR_ID'] and n not in march_data['BattleFieldBuffFactor.lua'])))
        else:
            P('    (March bytecode not reachable: set BUFF_CENSUS_KOM_CONTENTS to the kom_contents directory)')
        P('  inline BUFF_FACTOR blocks: %d in %d scripts (of %d .src.lua scanned)' % (
            len(C['inline']), len(set(r for r, _ in C['inline'])), C['nfiles']))
        by = collections.Counter(r.split('/')[0] for r, _ in C['inline'])
        P('    by archive: ' + ', '.join('%s=%d' % kv for kv in sorted(by.items())))
        P('  IMMUNITY_LIST_AT_THIS_STATE: %d uses in %d files' % (C['imm'][0], len(C['imm'][1])))
        P('')

    # ---------------------------------------------------------------- enum diff
    if want('2'):
        P('== Enum diff: June Enum.src.lua vs March march2014/data036/Enum.src.lua')
        P('  %-30s %6s %6s %6s %s' % ('table', 'June', 'March', 'new', 'value-changed'))
        rows = enum_diff(jb, mb)
        for t, nj, nm, new, chg, gone in rows:
            if nm is None:
                P('  %-30s %6d %6s %6s (table absent in March)' % (t, nj, '-', nj))
                continue
            P('  %-30s %6d %6d %6d %d' % (t, nj, nm, len(new), len(chg)))
        for t, nj, nm, new, chg, gone in rows:
            if nm is None:
                continue
            if chg:
                for k, mv, jv in chg:
                    P('    CHANGED  %s.%s  March %s -> June %s' % (t, k, mv, jv))
            if gone:
                P('    MARCH-ONLY %s: %s' % (t, ', '.join('%s=%s' % (k, mb[t][k]) for k in gone)))
        for t in mb:
            if t not in jb:
                P('  (March-only table: %s)' % t)
        for t in jb:
            for k, v in jb[t].items():
                if not isinstance(v, int):
                    P('  non-numeric member %s.%s = %s' % (t, k, v))
        newt = sorted(t for t in jb if t not in mb)
        P('  tables June has and March lacks: %d  %s' % (len(newt), ', '.join(newt)))
        # value-ranges of new BBT / BFT
        for t in ('BUFF_BEHAVIOR_TYPE', 'BUFF_FINALIZER_TYPE'):
            new = [(v, k) for k, v in jb[t].items() if k not in mb[t]]
            P('  new %s: %d  %s' % (t, len(new), ', '.join('%s=%s' % (k, v) for v, k in sorted(new))[:2000]))
        P('')

        # C++ enums vs June / March Lua
        P('== C++ enum (X2BuffFactor.h) vs Lua enums')
        cenum = march['enums']
        for ct, lt, pre in (('BUFF_BEHAVIOR_TYPE', 'BUFF_BEHAVIOR_TYPE', 'BBT_'), ('BUFF_FINALIZER_TYPE', 'BUFF_FINALIZER_TYPE', 'BFT_'),
                            ('BUFF_DISPLAYER_TYPE', 'BUFF_DISPLAYER_TYPE', 'BDT_'), ('BUFF_TEMPLET_ID', 'BUFF_TEMPLET_ID', 'BTI_'),
                            ('BUFF_FACTOR_ID', 'BUFF_FACTOR_ID', 'BFI_')):
            ce = cenum.get(ct, {})
            ce = collections.OrderedDict((k, v) for k, v in ce.items() if k.startswith(pre))
            jm, mm = jb[lt], mb[lt]
            lack_j = [k for k in jm if k.startswith(pre) and k not in ce]
            drift = [(k, v, jm[k]) for k, v in ce.items() if k in jm and jm[k] != v]
            absent = [(k, v) for k, v in ce.items() if k not in jm]
            lack_m_lua = [k for k in mm if k.startswith(pre) and k not in ce]
            P('  %-20s C++ %4d | June %4d | C++ lacks (June) %4d | C++ lacks (March Lua) %4d | value drift vs June %d | C++-only names %d' % (
                ct, len(ce), len(jm), len(lack_j), len(lack_m_lua), len(drift), len(absent)))
            if ct in ('BUFF_BEHAVIOR_TYPE', 'BUFF_FINALIZER_TYPE', 'BUFF_DISPLAYER_TYPE'):
                P('      C++ lacks, already in March Lua: ' + (', '.join('%s=%s' % (k, mm[k]) for k in lack_m_lua) or '-'))
            for k, cv, jv in drift:
                P('      DRIFT   %s  C++ %s  June %s' % (k, cv, jv))
            byval = {v: k for k, v in jm.items()}
            for k, v in absent:
                P('      ABSENT  %s = %s in C++; June name at that value: %s' % (k, v, byval.get(v)))
        P('')

    # ---------------------------------------------------------------- unsupported types
    unsupported = {}
    if want('5', '5b', '6', '7', '8'):
        P('== June BBT / BFT / BDT used by data but not handled by March C++')
        P('   cols: used-by entries = templets/factors/inline blocks that list the type in COMBINATION')
        for section, table, pre, kind in (('BEHAVIOR', 'BUFF_BEHAVIOR_TYPE', 'BBT_', 'BEHAVIOR'), ('FINALIZER', 'BUFF_FINALIZER_TYPE', 'BFT_', 'FINALIZER'),
                                           ('DISPLAYER', 'BUFF_DISPLAYER_TYPE', 'BDT_', 'DISPLAYER')):
            roles = ('FACTOR', 'TEMPLET')
            have = {r: march['cases'].get((kind, r), set()) for r in roles}
            handled = have['FACTOR'] & have['TEMPLET'] if kind != 'DISPLAYER' else have['TEMPLET']
            only_one = (have['FACTOR'] ^ have['TEMPLET']) if kind != 'DISPLAYER' else set()
            slice_set = march['slice'].get(kind)
            used = st['types'][section]
            enum_all = jb[table]
            unsup = []
            for name, v in enum_all.items():
                if name.startswith(pre) and name not in handled and not name.endswith('_NONE'):
                    o = used.get(name, {})
                    tot = len(o.get('templet', ())), len(o.get('factor', ())), len(o.get('inline', ()))
                    unsup.append((v, name, tot))
            enum_only = [u for u in unsup if u[0] in NOT_IN_JUNE_CLIENT.get(section, ())]
            unsup = [u for u in unsup if u[0] not in NOT_IN_JUNE_CLIENT.get(section, ())]
            unsupported[section] = unsup
            used_unsup = [u for u in unsup if sum(u[2]) > 0]
            if enum_only:
                P('  %s: not implemented in the June client either (enum only, excluded below): %s' % (
                    section, ', '.join('%s=%d (data %d)' % (n, v, sum(t)) for v, n, t in sorted(enum_only))))
            P('  %s: June enum %d | March C++ handles %d (factor-parse %d, templet-factory %d) | not handled %d | of those used by data %d | unused by any data %d' % (
                section, len([n for n in enum_all if n.startswith(pre)]), len(handled), len(have['FACTOR']), len(have['TEMPLET']), len(unsup),
                len(used_unsup), len(unsup) - len(used_unsup)))
            if only_one:
                P('    C++ asymmetry (in factor-parse XOR templet-factory): ' + ', '.join(sorted(only_one)))
            if slice_set is not None and kind in ('BEHAVIOR', 'FINALIZER'):
                miss = sorted(handled - slice_set)
                P('    level-slice (BUFF_FACTOR::Get%sFactorToLevel): %d of the handled types have no explicit case (default branch applies): %s' % (
                    kind.title(), len(miss), ', '.join(miss) or '-'))
            P('    %-5s %-46s %7s %7s %7s  %s' % ('val', 'type', 'templ', 'factor', 'inline', 'phase'))
            for v, name, tot in sorted(unsup):
                ph = phase_of_type(section, v)
                if a.phase is not None and a.phase != ph:
                    continue
                P('    %-5s %-46s %7d %7d %7d  %s' % (v, name, tot[0], tot[1], tot[2], ph))
            # types in data that March's *Lua enum* lacks but the data uses -> would be a bug
            for name in used:
                if name not in enum_all:
                    P('    !! data uses %s which June Enum lacks' % name)
        P('')
        anom = collections.Counter((o, s, w) for o, s, w, t in st['anomaly'])
        if anom and a.phase is None:
            P('  data anomalies (COMBINATION vs sub-tables): ' + '; '.join('%s/%s/%s x%d' % (o, s, w, c) for (o, s, w), c in anom.items()))
            P('')

    # ---------------------------------------------------------------- custom functions
    if want('9'):
        P('== BBT_CUSTOM_FUNCTION templets (June) vs CX2GameUnit::CreateAndInsertCustomFunctionByBuff cases')
        cf = []
        for tab in J['templet']['templets'] + J['templet_int']['templets']:
            lst, _ = types_in(tab, 'BEHAVIOR', 'BUFF_BEHAVIOR_TYPE')
            if 'BBT_CUSTOM_FUNCTION' in lst:
                cf.append(enum_member(tab.get('BUFF_TEMPLET_ID'), 'BUFF_TEMPLET_ID'))
        handled = march['custom_fn_btis']
        un = [b for b in cf if b not in handled]
        P('  June custom-function templets: %d | March case labels: %d (%s)' % (len(cf), len(handled), ', '.join(sorted(handled))))
        P('  not handled by March: %d' % len(un))
        for b in un:
            P('    %s' % b)
        P('')

    # ---------------------------------------------------------------- keys
    P_keys = {}
    LEVEL_OF = {'BEHAVIOR': 'behavior', 'FINALIZER': 'finalizer', 'DISPLAYER': 'displayer'}
    TABLE_OF = {'BEHAVIOR': 'BUFF_BEHAVIOR_TYPE', 'FINALIZER': 'BUFF_FINALIZER_TYPE', 'DISPLAYER': 'BUFF_DISPLAYER_TYPE'}

    def type_owner(section, typ):
        """Phase that owns a type: 4b if March handles it, else the phase its enum value is assigned to."""
        if typ in march['cases'].get((section, 'FACTOR'), ()) or typ in march['cases'].get((section, 'TEMPLET'), ()):
            return '4b'
        return phase_of_type(section, jb[TABLE_OF[section]].get(typ, -1))

    if want('4a', '4b', '5', '5b', '6', '7', '8'):
        # owners of a level-wide key = phases of the types it sits under (templet/factor level keys: 4a)
        owners = collections.defaultdict(set)
        for (section, typ, path), origins in st['tkeys'].items():
            owners[(LEVEL_OF[section], path.split('.')[-1])].add(type_owner(section, typ))
        P('== June data keys March never reads: level-wide view (the ledger\'s definition)')
        P('   entry = templet / factor / inline block carrying the key (nested keys counted by bare name);')
        P('   March-supported = key literal (or a "%d" pattern literal) present in the C++ that parses that level;')
        P('   owner = phase of the type(s) the key sits under: 4b = a type March already handles, 5-8 = a type March lacks;')
        P('   in-March-data = the key string is in March\'s compiled BattleFieldBuff*/BuffTemplet bytecode (if reachable)')
        rows = []
        june_unread = []
        for (level, k), origins in st['keys'].items():
            if lit_supported(k, lsets[level]):
                continue
            if re.match(r'^(BBT|BFT|BDT)_', k):
                continue
            if not_read_by_june(level, k):
                june_unread.append((level, k, sum(len(s) for s in origins.values())))
                continue
            n = {o: len(s) for o, s in origins.items()}
            own = sorted(owners.get((level, k), set())) if level in ('behavior', 'finalizer', 'displayer') else ['4a']
            rows.append((level, k, sum(n.values()), n.get('templet', 0), n.get('factor', 0), n.get('inline', 0), ','.join(own)))
        rows.sort(key=lambda r: (r[0], -r[2], r[1]))
        order = ['templet', 'factor', 'behavior', 'finalizer', 'displayer']
        for lv in order:
            sub = [r for r in rows if r[0] == lv and (a.phase is None or a.phase in r[6].split(','))]
            if not sub:
                continue
            P('  %s level: %d key(s), %d entry-uses' % (lv, len(sub), sum(r[2] for r in sub)))
            P('    %-44s %6s %6s %6s %6s  %-6s %s' % ('key', 'total', 'templ', 'factor', 'inline', 'owner', 'in-March-data'))
            for r in sub:
                P('    %-44s %6d %6d %6d %6d  %-6s %s' % (r[1], r[2], r[3], r[4], r[5], r[6], march_data_flag(march_data, r[1])))
            P_keys[lv] = sub
        if june_unread and (a.phase is None or a.phase == '4a'):
            P('  not read by June\'s client either (excluded; see NOT_READ_BY_JUNE_CLIENT): %s' %
              ', '.join('%s.%s x%d' % r for r in sorted(june_unread)))
        P('')

    if want('4b'):
        # ---- per-type view: keys under types March DOES handle that the type's own parse never reads
        P('== Keys inside types March handles that the type\'s parse never reads (phase 4b scope)')
        P('   per-type reader = literals in the CASE_*_FACTOR parse function (factor/inline entries) or the templet class\'s')
        P('   ParsingBehavior/Finalizer/Displayer (templet entries), following the calls they make (Q::f in Q, plain f in the')
        P('   caller\'s class chain); path A.B = key B nested in table A, reported at the first unread component.')
        P('   Literal match, not data flow: spot-check a key in source before acting on it.')
        per_type = collections.OrderedDict()
        june_unread_type = collections.Counter()
        for (section, typ, path), origins in st['tkeys'].items():
            parts = path.split('.')
            for o, ids in origins.items():
                rd = march['readers'].get((section, typ, 'TEMPLET' if o == 'templet' else 'FACTOR'))
                if rd is None:
                    continue                              # type not handled by C++ at all: its keys belong to the new type
                for i in range(len(parts)):
                    if not lit_supported(parts[i], rd):
                        fn = not_read_by_june_type(section, typ, o, parts[i])
                        if fn:
                            june_unread_type[(section, typ, 'templet' if o == 'templet' else 'factor', parts[i], fn)] += len(ids)
                            break
                        pref = '.'.join(parts[:i + 1])
                        per_type.setdefault((section, typ, pref), set()).update((o, x) for x in ids)
                        break
        agg = collections.defaultdict(list)
        for (section, typ, pref), ents in per_type.items():
            agg[(section, typ)].append((pref, len(ents), collections.Counter(o for o, _ in ents)))
        tot_unread = 0
        for (section, typ), lst in sorted(agg.items()):
            # xN(t/f/i) = entry-uses split by origin: templet / factor / inline block
            P('  %-10s %-44s %s' % (section, typ, '  '.join('%s x%d(%d/%d/%d)' % (k, n, c['templet'], c['factor'], c['inline'])
                                                            for k, n, c in sorted(lst, key=lambda x: (-x[1], x[0])))))
            tot_unread += sum(n for _, n, _ in lst)
        P('  (%d type/key pair(s), %d entry-uses)' % (sum(len(v) for v in agg.values()), tot_unread))
        if june_unread_type:
            P('  not read by June\'s own reader for that origin either (excluded; see NOT_READ_BY_JUNE_TYPE):')
            for (s_, t_, k_, key_, fn_), n_ in sorted(june_unread_type.items()):
                P('    %-10s %-44s %-8s %-24s x%-4d June %s' % (s_, t_, k_, key_, n_, fn_))
        P_keys['per_type'] = [[s_, t_, k_, n_] for (s_, t_), v_ in agg.items() for k_, n_, _ in v_]
        P('')

    if want('5', '5b', '6', '7', '8') and (a.phase is not None or a.verbose):
        # ---- every key the data puts under a type March cannot handle yet: the transcription checklist for 5-8
        P('== Keys the data puts under the types March cannot handle yet (path A.B = B nested in A; xN = entries)')
        for section in ('BEHAVIOR', 'FINALIZER'):
            for v, name, tot in sorted(unsupported.get(section, [])):
                if a.phase is not None and a.phase != phase_of_type(section, v):
                    continue
                ents = collections.defaultdict(set)
                for (sec, typ, path), origins in st['tkeys'].items():
                    if sec == section and typ == name:
                        for o, ids in origins.items():
                            ents[path].update((o, x) for x in ids)
                P('  %-5s %-46s %s' % (v, name, '  '.join('%s x%d' % (k, len(e)) for k, e in sorted(ents.items(), key=lambda kv: (-len(kv[1]), kv[0]))) or '(no sub-keys)'))
        P('')

    if want('4a', '4b'):
        nested_lines = sorted((lv, k, sorted(v)) for (lv, k), v in st['nested'].items() if (lv, k) in dict(((r[0], r[1]), 1) for r in rows))
        if nested_lines and want('4a'):
            P('== Sub-keys of the unread top-level keys above')
            for lv, k, v in nested_lines:
                P('  %s.%s: %s' % (lv, k, ', '.join(v)))
            P('')

    # ---------------------------------------------------------------- bindings & premium
    if want('10', '11'):
        P('== June-only Lua bindings: uses in june_src_fixed and whether March source mentions them')
        march_src = ''
        for f in BIND_SOURCES:                              # preprocessed: a binding under an OFF flag is not "present"
            p = os.path.join(X2LIB, f)
            if os.path.exists(p):
                march_src += active_text(p, unknown)
        for b in BIND_PATTERNS:
            cnt, files = C['binds'][b]
            P('  %-34s June %4d use(s) in %3d file(s)   March source: %s' % (b, cnt, len(files), 'present' if b in march_src else 'ABSENT'))
        P('  BUFF_GROUP_ID entries in UnitApplyBuffGroup: %d' % len(J['group']['groups']))
        pk = collections.Counter()
        for src in ('premium', 'premium_int'):
            for tab in J[src]['premium']:
                for k in tab.keys():
                    pk[k] += 1
        P('  premium AddBuffInfo keys (June, %d entries)   C++ reads = literal in X2PremiumBuffManager.cpp; in-March-data = in March BuffTemplet.lua' % sum(
            len(J[s]['premium']) for s in ('premium', 'premium_int')))
        for k, c in sorted(pk.items(), key=lambda kv: -kv[1]):
            P('    %-36s %4d  C++ %-6s in-March-data %s' % (k, c, 'reads' if k in march['premium_literals'] else 'never',
                                                           march_data_flag(march_data, k, files=('BuffTemplet.lua',))))
        P('')

    if a.json:
        dump = dict(
            enum_new={t: len(r[3] or []) for t, r in zip(jb, enum_diff(jb, mb)) if r[2] is not None},
            unsupported_types={s: [(v, n, t) for v, n, t in u] for s, u in unsupported.items()},
            keys={lv: [list(r) for r in rows] for lv, rows in P_keys.items()},
            inline_blocks=len(C['inline']), inline_scripts=len(set(r for r, _ in C['inline'])))
        json.dump(dump, open(a.json, 'w'), indent=1)


if __name__ == '__main__':
    main()
