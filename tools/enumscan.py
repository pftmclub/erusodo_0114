#!/usr/bin/env python
"""enumscan.py - find raw integers that are enum values in June-port code (JUNE_ENUM_NAMES_PHASES.md, Phase N0).

    python tools/enumscan.py                       # scan the survey files, write june2017_re/enum_literals.tsv
    python tools/enumscan.py --check FILE...       # exit 1 while a class A or B row remains in those files
    python tools/enumscan.py --verify [N]          # compile-probe N resolved C++ enumerators (default 120)
    python tools/enumscan.py --summary             # per-file / per-class counts of the last tsv
    python tools/enumscan.py --reuse               # reuse the cached cl /P output and enum tables

What it does
  1. C++ enums: cl /P on X2Lib/X2GuEve.cpp (X2Lib's real US_SERVICE defines, so #ifdef / #else duplicates are resolved the way
     the compiler sees them), then a scope-tracking parse of every `enum { ... }` in the output with a small constant evaluator
     (implicit numbering, `A = B + 1`, `X | Y`, `<<`). `--verify` re-checks a sample with june2017_re/enumprobe.py's
     `char (*)[N]` / C2440 trick, because the evaluator is not the compiler.
  2. Lua enums: `NAME = {` tables of the live kom_contents/data036_kom/Enum.lua and June's june_src_fixed/data036_kom/Enum.src.lua,
     read as text (name -> integer, per table).
  3. June code of each file: the live branch of `#ifdef SERV_IRUHADEV_JUNE_*`, functions named *June* / JUNE_*,
     `_CONST_*_JUNE_*` namespaces, `const ... JUNE_* = N`. Code in the #else of SERV_IRUHADEV_JUNE_ENUM_NAMES is the kept literal
     and is skipped, so a renamed site stops appearing.
  4. Every integer literal there (hex or decimal, value >= 100; a `case` label of a switch whose expression names a family
     and a `const ... JUNE_X = N` whose name or comment names a family count at any value, --small adds table rows below 100)
     is given a
     family from its context (switch expression, callee, declaration name, neighbouring comment) and looked up by value.

Classes
  A  the name is already a C++ member                     -> rename the literal
  B  the name is in June's Enum.src.lua / live Enum.lua   -> add the member, then rename
  C  in no Enum                                           -> N1 investigates
  D  lead only: no family could be read from the context, value matches some family (noise is expected; --check ignores D)
  X  a family with no Enum table in this project (STR_ID, item, dungeon, net-error ids): stays a number; --check ignores X
Columns: file line literal hex context family class candidate_name in_cpp in_enumlua in_june note
"""
import argparse
import bisect
import collections
import json
import os
import random
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..'))
RE_DIR = os.path.join(TRUNK, 'june2017_re')
sys.path.insert(0, RE_DIR)

KOM_ROOT = os.environ.get('X2_KOM_CONTENTS', r'F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents')
LIVE_ENUM = os.path.join(KOM_ROOT, 'data036_kom', 'Enum.lua')
JUNE_ENUM = os.path.join(TRUNK, 'june_src_fixed', 'data036_kom', 'Enum.src.lua')
TSV = os.path.join(RE_DIR, 'enum_literals.tsv')
CACHE = os.path.join(TRUNK, '.build', 'enumscan')
PP_TU = 'X2GuEve.cpp'                  # one TU reaches every class header through stdafx.h
ENUM_FLAG = 'SERV_IRUHADEV_JUNE_ENUM_NAMES'
JUNE_FLAG_PREFIX = 'SERV_IRUHADEV_JUNE_'

# the survey files (plan table); X2EffectSet / X2Main are the two clean ones
DEFAULT_FILES = ['X2GuEve', 'X2GUArme_VioletMage', 'X2GUUser', 'X2UISkillTreeNew2015', 'X2SkillTree', 'X2GameUnit',
                 'X2GUNPC', 'X2Game', 'X2UIInventory', 'X2KeyPad', 'X2EffectSet', 'X2Main']

# C++ enum name -> family ; Lua table name -> family (same family names)
CPP_FAMILY = {'SKILL_ID': 'SKILL_ID', 'NPC_UNIT_ID': 'NPC_UNIT_ID', 'UNIT_CLASS': 'UNIT_CLASS',
              'BUFF_TEMPLET_ID': 'BUFF_TEMPLET_ID', 'BUFF_FACTOR_ID': 'BUFF_FACTOR_ID',
              'BUFF_BEHAVIOR_TYPE': 'BUFF_BEHAVIOR_TYPE', 'DAMAGE_TYPE': 'DAMAGE_TYPE',
              'DAMAGE_TRIGGER_TYPE': 'DAMAGE_TRIGGER_TYPE',
              'SKILL_ABILITY_TYPE': 'SKILL_ABILITY_TYPE'}
LUA_FAMILY = dict((k, k) for k in CPP_FAMILY)
LUA_FAMILY.update({'SKILL_ABILITY_TYPE': 'SKILL_ABILITY_TYPE'})
NO_ENUM_FAMILIES = ('STR_ID', 'ITEM_ID', 'DUNGEON_ID', 'NET_ERROR')
SMALL = False                 # --small: values < 100 also in tables (case labels and JUNE_ declarations under a family hint always count)
ID_FAMILIES = ['SKILL_ID', 'NPC_UNIT_ID', 'UNIT_CLASS', 'BUFF_TEMPLET_ID', 'BUFF_FACTOR_ID', 'BUFF_BEHAVIOR_TYPE',
               'DAMAGE_TYPE', 'DAMAGE_TRIGGER_TYPE', 'SKILL_ABILITY_TYPE', 'STATE']


# ---------------------------------------------------------------------------------------------------------------------
# C++ enums
# ---------------------------------------------------------------------------------------------------------------------
TOK_PP = re.compile(r'''
    "(?:\\.|[^"\\\n])*"            |
    '(?:\\.|[^'\\\n])*'            |
    (?P<n>0[xX][0-9a-fA-F]+[uUlL]*|\d+[uUlL]*) |
    (?P<i>[A-Za-z_]\w*)            |
    (?P<o><<|>>|::|[{}();,=+\-*/%|&^~!<>?:\[\]])
''', re.X)


def run_cl_p(out):
    import census as C
    os.makedirs(os.path.dirname(out), exist_ok=True)
    r = subprocess.run(C.cl_cmd(PP_TU, out), cwd=os.path.join(TRUNK, 'X2Lib'), env=C.cl_env(), capture_output=True)
    if r.returncode != 0 or not os.path.exists(out):
        sys.exit('cl /P failed (exit %s):\n%s' % (r.returncode, r.stdout.decode('latin1', 'replace')[-3000:]))


class Evaluator(object):
    """Constant expressions over token lists: ints, enumerators, + - * / % << >> & | ^ ~ ! parens, casts."""

    def __init__(self, lookup):
        self.lookup = lookup

    def eval(self, toks):
        self.t = toks
        self.p = 0
        try:
            v = self.bor()
            return v if self.p == len(self.t) else None
        except (ValueError, IndexError, ZeroDivisionError, KeyError):
            return None

    def peek(self):
        return self.t[self.p] if self.p < len(self.t) else None

    def take(self):
        self.p += 1
        return self.t[self.p - 1]

    def level(self, nxt, ops):
        v = nxt()
        while self.peek() in ops:
            op = self.take()
            w = nxt()
            v = {'|': lambda a, b: a | b, '^': lambda a, b: a ^ b, '&': lambda a, b: a & b,
                 '<<': lambda a, b: a << b, '>>': lambda a, b: a >> b, '+': lambda a, b: a + b,
                 '-': lambda a, b: a - b, '*': lambda a, b: a * b, '/': lambda a, b: int(a / b),
                 '%': lambda a, b: a % b}[op](v, w)
        return v

    def bor(self):
        return self.level(self.bxor, ('|',))

    def bxor(self):
        return self.level(self.band, ('^',))

    def band(self):
        return self.level(self.shift, ('&',))

    def shift(self):
        return self.level(self.add, ('<<', '>>'))

    def add(self):
        return self.level(self.mul, ('+', '-'))

    def mul(self):
        return self.level(self.unary, ('*', '/', '%'))

    def unary(self):
        t = self.peek()
        if t == '-':
            self.take()
            return -self.unary()
        if t == '+':
            self.take()
            return self.unary()
        if t == '~':
            self.take()
            return ~self.unary()
        if t == '!':
            self.take()
            return 0 if self.unary() else 1
        return self.primary()

    def primary(self):
        t = self.take()
        if t == '(':
            # (int) cast / (TYPE)expr : a parenthesised lone identifier that is not a known enumerator is a cast
            if self.peek() is not None and re.match(r'[A-Za-z_]', self.peek()) and self.t[self.p + 1] == ')' \
                    and self.lookup(self.peek()) is None:
                self.p += 2
                return self.unary()
            v = self.bor()
            if self.take() != ')':
                raise ValueError
            return v
        if t in ('static_cast', 'reinterpret_cast', 'const_cast'):
            if self.take() != '<':
                raise ValueError
            while self.take() != '>':
                pass
            if self.take() != '(':
                raise ValueError
            v = self.bor()
            if self.take() != ')':
                raise ValueError
            return v
        if re.match(r'(0[xX][0-9a-fA-F]+|\d+)[uUlL]*$', t):
            return int(re.sub(r'[uUlL]+$', '', t), 0 if t[:2].lower() == '0x' else 10)
        if re.match(r'[A-Za-z_]', t):
            name = t
            while self.peek() == '::':           # A::B::C -> C
                self.take()
                name = self.take()
            v = self.lookup(name)
            if v is None:
                raise KeyError(name)
            return v
        raise ValueError(t)


def parse_cpp_enums(i_path):
    """-> list of dicts {scope, name, members: [(name, value|None)], family}"""
    data = open(i_path, 'rb').read().decode('latin1')
    data = re.sub(r'(?m)^[ \t]*#[^\n]*$', '', data)
    toks = [m.group(0) for m in TOK_PP.finditer(data) if m.group(0)[0] not in '"\'']
    enums = []
    scope = []                 # stack of ('class'|'namespace'|'block', name)
    glob = {}                  # enumerator -> value (last definition wins; scope-local first via 'local')
    ev = Evaluator(lambda n: None)
    i, n = 0, len(toks)
    while i < n:
        t = toks[i]
        if t in ('class', 'struct', 'namespace', 'union') and i + 1 < n and re.match(r'[A-Za-z_]', toks[i + 1]):
            j = i + 1
            name = toks[j]
            j += 1
            if j < n and toks[j] == '{':
                scope.append((t, name))
                i = j + 1
                continue
            if j < n and toks[j] == ':' and t != 'namespace':
                while j < n and toks[j] not in ('{', ';'):
                    j += 1
                if j < n and toks[j] == '{':
                    scope.append((t, name))
                    i = j + 1
                    continue
            i += 1
            continue
        if t == 'enum':
            j = i + 1
            name = ''
            if j < n and re.match(r'[A-Za-z_]', toks[j]):
                name = toks[j]
                j += 1
            if j < n and toks[j] == '{':
                j += 1
                members = []
                local = {}
                cls = '::'.join(s[1] for s in scope if s[0] in ('class', 'struct', 'namespace'))
                lookup = lambda nm: local.get(nm, glob.get(nm))
                ev.lookup = lookup
                prev = -1
                while j < n and toks[j] != '}':
                    if not re.match(r'[A-Za-z_]', toks[j]):
                        j += 1
                        continue
                    mname = toks[j]
                    j += 1
                    val = None
                    if j < n and toks[j] == '=':
                        j += 1
                        expr = []
                        depth = 0
                        while j < n and not (depth == 0 and toks[j] in (',', '}')):
                            depth += (toks[j] == '(') - (toks[j] == ')')
                            expr.append(toks[j])
                            j += 1
                        val = ev.eval(expr)
                    else:
                        val = prev + 1 if prev is not None else None
                    prev = val
                    members.append((mname, val))
                    if val is not None:
                        local[mname] = val
                        glob[mname] = val
                    if j < n and toks[j] == ',':
                        j += 1
                enums.append({'scope': cls, 'name': name, 'members': members})
                i = j + 1
                continue
            i += 1
            continue
        if t == '{':
            scope.append(('block', ''))
        elif t == '}':
            if scope:
                scope.pop()
        i += 1
    return enums


def load_cpp(reuse):
    """-> {family: {value: [qualified names]}}, {qualified name: value}, state tables {scope-lower: {value: [names]}}, stats"""
    ipath = os.path.join(CACHE, PP_TU[:-4] + '.i')
    jpath = os.path.join(CACHE, 'cpp_enums.json')
    if reuse and os.path.exists(jpath):
        enums = json.load(open(jpath))
    else:
        if not (reuse and os.path.exists(ipath)):
            run_cl_p(ipath)
        enums = parse_cpp_enums(ipath)
        json.dump(enums, open(jpath, 'w'))
    fam = collections.defaultdict(lambda: collections.defaultdict(list))
    qual = {}
    states = collections.defaultdict(lambda: collections.defaultdict(list))
    for e in enums:
        f = CPP_FAMILY.get(e['name'])
        is_state = f is None and any(re.match(r'[A-Z]{1,6}SI_', m) for m, _ in e['members'][:6])
        for m, v in e['members']:
            if v is None:
                continue
            q = (e['scope'] + '::' + m) if e['scope'] else m
            qual[q] = v
            if f:
                fam[f][v].append(q)
            elif is_state:
                states[e['scope'].split('::')[-1].lower()][v].append(q)
    return fam, qual, states, enums


# ---------------------------------------------------------------------------------------------------------------------
# Lua enums
# ---------------------------------------------------------------------------------------------------------------------
def load_lua_enum(path):
    """-> {family: {value: [names]}}, {family: {name: value}}"""
    byval = collections.defaultdict(lambda: collections.defaultdict(list))
    byname = collections.defaultdict(dict)
    if not os.path.exists(path):
        return byval, byname
    cur = None
    for line in open(path, 'rb').read().decode('utf-8-sig', 'replace').split('\n'):
        line = re.sub(r'--.*$', '', line).strip()
        m = re.match(r'([A-Za-z_]\w*)\s*=\s*\{\s*$', line)
        if m:
            cur = LUA_FAMILY.get(m.group(1))
            continue
        if line.startswith('}'):
            cur = None
            continue
        if cur:
            m = re.match(r'([A-Za-z_]\w*)\s*=\s*(-?\d+|0[xX][0-9a-fA-F]+)\s*,?$', line)
            if m:
                v = int(m.group(2), 0)
                byval[cur][v].append(m.group(1))
                byname[cur][m.group(1)] = v
    return byval, byname


# ---------------------------------------------------------------------------------------------------------------------
# Source scanner
# ---------------------------------------------------------------------------------------------------------------------
TOK_SRC = re.compile(r'''
    (?P<lc>//[^\n]*)                            |
    (?P<bc>/\*.*?\*/)                           |
    (?P<str>L?"(?:\\.|[^"\\\n])*")              |
    (?P<chr>L?'(?:\\.|[^'\\\n])*')              |
    (?P<pp>^[ \t]*\#(?:[^\n\\]|\\\r?\n|\\.)*)   |
    (?P<num>(?<![\w.])(?:0[xX][0-9a-fA-F]+|\d+)[uUlL]*(?![\w.])) |
    (?P<id>[A-Za-z_]\w*)                        |
    (?P<op>==|!=|<=|>=|&&|\|\||::|->|[{}()\[\];,<>=:?])
''', re.X | re.S | re.M)

# hint regexes: ordered, first match per hint string wins; a hint string can yield several families (kept in order)
B_ = r'(?<![A-Za-z0-9])'      # prefix boundary that, unlike \b, lets an underscore precede (JUNE_CH1K_NUI_1621)
HINTS = [
    ('STR_ID', re.compile(r'GET_STRING|' + B_ + r'STR_|GetString|szTip|\bMessageBox|GetStr|GET_REPLACED_STRING|GuideDesc|(?i:string_?id)')),
    ('ITEM_ID', re.compile(r'(?i)itemid|item_id|' + B_ + r'ITEM_|iItem|\bGetItemTemplet|\bnItem')),
    ('DUNGEON_ID', re.compile(r'(?i)dungeonid|dungeon_id|' + B_ + r'S?DI_|iDungeon')),
    ('NET_ERROR', re.compile(r'NetError|NET_ERR|' + B_ + r'NEF_')),
    ('UNIT_CLASS', re.compile(r'UNIT_CLASS|' + B_ + r'UC_|(?i:unitclass|eunitclass|iclass\b|classtype|class_?id|\bnclass)')),
    ('BUFF_TEMPLET_ID', re.compile(r'BUFF_TEMPLET_ID|' + B_ + r'BTI_|(?i:\bbti\b|buff_?templet|buffid(?!entity)|\bbtid\b)')),
    ('BUFF_FACTOR_ID', re.compile(r'BUFF_FACTOR_ID|' + B_ + r'BFI_|(?i:\bbfi\b|buff_?factor)')),
    ('BUFF_BEHAVIOR_TYPE', re.compile(r'BUFF_BEHAVIOR_TYPE|' + B_ + r'BBT_|(?i:\bbbt\b|behaviortype)')),
    ('DAMAGE_TRIGGER_TYPE', re.compile(r'DAMAGE_TRIGGER_TYPE|' + B_ + r'DTT_|(?i:triggertype)')),
    ('DAMAGE_TYPE', re.compile(r'DAMAGE_TYPE|(?i:damagetype)')),
    ('NPC_UNIT_ID', re.compile(r'NPC_UNIT_ID|' + B_ + r'NUI_|(?i:\bnpc_?id|\binpc|\benpc|npcunitid|\bnui\b|summonnpc|npc)')),
    ('STATE', re.compile(B_ + r'[A-Z]{1,6}SI_\w')),
    ('SKILL_ABILITY_TYPE', re.compile(r'SKILL_ABILITY_TYPE|' + B_ + r'SA_[A-Z]|(?i:abilitytype|\bsat\b)')),
    ('SKILL_ID', re.compile(r'SKILL_ID|' + B_ + r'(?:RNSI|TSI|SI)_|(?i:skill_?id|\b[ien]?skill_?$)')),
    ('STATE', re.compile(r'(?i:stateid|state_id|nowstate|\bstate\b|statechange|changestate|statestart|stateend|statefn|_state\b)')),
    ('SKILL_ID', re.compile(r'(?:^|_)(?:SA|HA|A|P)_[A-Z]')),           # a bare June skill-name stem (JUNE_CH4C_A_ABM_AURA_POWER_ACCEL)
]


def hint_families(*texts):
    """ordered distinct families named by the given hint strings (earlier strings first)"""
    out = []
    for t in texts:
        if not t:
            continue
        for fam, rx in HINTS:
            if rx.search(t) and fam not in out:
                out.append(fam)
    return out


def comment_names(text):
    """identifiers that look like enum names in a comment, with the family their prefix says"""
    out = []
    for m in re.finditer(r'\b(NUI_\w+|BTI_\w+|BFI_\w+|BBT_\w+|DTT_\w+|UC_\w+|RNSI_\w+|TSI_\w+|SI_\w+|SA_\w+|[A-Z]{1,6}SI_\w+)', text):
        out.append(m.group(1))
    return out


def name_family(nm):
    if nm.startswith('NUI_'):
        return 'NPC_UNIT_ID'
    if nm.startswith('BTI_'):
        return 'BUFF_TEMPLET_ID'
    if nm.startswith('BFI_'):
        return 'BUFF_FACTOR_ID'
    if nm.startswith('BBT_'):
        return 'BUFF_BEHAVIOR_TYPE'
    if nm.startswith('DTT_'):
        return 'DAMAGE_TRIGGER_TYPE'
    if nm.startswith('UC_'):
        return 'UNIT_CLASS'
    if nm.startswith('SA_'):
        return 'SKILL_ABILITY_TYPE'
    if re.match(r'(RNSI|TSI|SI)_', nm):
        return 'SKILL_ID'
    if re.match(r'[A-Z]{1,6}SI_', nm):
        return 'STATE'
    return None


class Lit(object):
    __slots__ = ('line', 'text', 'value', 'ctx', 'hints', 'func', 'name', 'note', 'june', 'family', 'cls', 'cand', 'cpp', 'live_', 'june_')


def read_source(path):
    return open(path, 'rb').read().decode('cp949', 'replace')


def scan_file(path):
    """-> list of Lit for the June code of one source file"""
    text = read_source(path)
    nl = [m.start() for m in re.finditer('\n', text)]
    line_of = lambda off: bisect.bisect_left(nl, off) + 1
    toks = []                         # (kind, text, off)
    comments = collections.defaultdict(list)
    for m in TOK_SRC.finditer(text):
        k = m.lastgroup
        if k in ('lc', 'bc'):
            comments[line_of(m.start())].append(m.group(0))
            continue
        toks.append((k, m.group(0), m.start()))
    # flag stack, bracket stack, June state
    flags = []                        # dict(if_j, else_j, branch, skip)
    stack = []                        # dict(ch, callee, commas, sw, fn, ns, kind, args)
    last_closed = None
    lits = []
    n = len(toks)

    def flag_state():
        j = any((f['if_j'] if f['branch'] == 0 else f['else_j']) for f in flags)
        s = any(f['skip'] and f['branch'] == 1 for f in flags)
        return j, s

    def cond_has_june(expr):
        for m in re.finditer(r'(!?)\s*(?:defined\s*\(?\s*)?(SERV_IRUHADEV_JUNE_\w+)', expr):
            if not m.group(1) and not re.search(r'!\s*defined\s*\(?\s*$', expr[:m.start(2)]):
                return True
        return False

    def prev_sig(i, k=1):
        return toks[i - k] if i - k >= 0 else ('', '', 0)

    def operand_text(i, direction):
        """identifier text of the operand that sits next to the operator at toks[i]"""
        out = []
        j = i + direction
        depth = 0
        while 0 <= j < n and len(out) < 12:
            kd, tx, _ = toks[j]
            if kd == 'pp':
                break
            if tx in (')', ']') and direction < 0:
                depth += 1
            elif tx in ('(', '[') and direction < 0:
                if depth == 0:
                    break
                depth -= 1
            elif tx in ('(', '[') and direction > 0:
                depth += 1
            elif tx in (')', ']') and direction > 0:
                if depth == 0:
                    break
                depth -= 1
            elif depth == 0 and tx in ('&&', '||', ';', ',', '{', '}', '?', ':', '==', '!=', '=', 'return', 'case'):
                break
            if kd in ('id', 'str'):
                out.append(tx)
            j += direction
        return ' '.join(out)

    def stmt_lhs(i):
        """the identifier written before the '=' of the statement that holds toks[i] ('' when none)"""
        j = i - 1
        while j >= 0 and i - j < 40 and toks[j][1] not in (';', '{', '}') and toks[j][0] != 'pp':
            if toks[j][1] == '=' and j >= 1 and toks[j - 1][0] == 'id':
                return toks[j - 1][1]
            j -= 1
        return ''

    def callee_of(i):
        """text naming what the '(' at toks[i] calls (template args kept)"""
        j = i - 1
        if j >= 0 and toks[j][1] == '>':
            k = j
            while k > 0 and toks[k][1] != '<' and j - k < 12:
                k -= 1
            inner = ' '.join(t[1] for t in toks[k + 1:j])
            base = toks[k - 1][1] if k > 0 else ''
            return '%s<%s>' % (base, inner)
        if j >= 0 and toks[j][0] == 'id':
            nm = toks[j][1]
            while j - 2 >= 0 and toks[j - 1][1] == '::' and toks[j - 2][0] == 'id':
                nm = toks[j - 2][1] + '::' + nm
                j -= 2
            return nm
        return ''

    for i in range(n):
        kd, tx, off = toks[i]
        if kd == 'pp':
            body = re.sub(r'\s+', ' ', tx.strip()[1:].strip())
            m = re.match(r'(ifdef|ifndef|if|elif|else|endif)\b\s*(.*)', body)
            if not m:
                continue
            d, rest = m.group(1), m.group(2)
            if d in ('ifdef', 'ifndef'):
                nm = rest.split(' ')[0] if rest else ''
                isj = nm.startswith(JUNE_FLAG_PREFIX)
                flags.append({'if_j': isj and d == 'ifdef', 'else_j': isj and d == 'ifndef', 'branch': 0,
                              'skip': nm == ENUM_FLAG and d == 'ifdef'})
            elif d == 'if':
                has = cond_has_june(rest)
                neg = bool(re.match(r'!\s*defined', rest))
                flags.append({'if_j': has and not neg, 'else_j': has and neg, 'branch': 0,
                              'skip': ENUM_FLAG in rest and not neg})
            elif d == 'elif':
                if flags:
                    flags[-1]['branch'] = 1
                    flags[-1]['else_j'] = cond_has_june(rest)
            elif d == 'else':
                if flags:
                    flags[-1]['branch'] = 1
            elif d == 'endif':
                if flags:
                    flags.pop()
            continue
        if kd == 'op':
            if tx in ('(', '['):
                stack.append({'ch': tx, 'callee': callee_of(i) if tx == '(' else '', 'commas': 0, 'pos': i})
            elif tx == '{':
                pv = prev_sig(i)
                ent = {'ch': '{', 'sw': None, 'fn': None, 'ns': None, 'kind': 'block', 'pos': i}
                if pv[1] in ('=', ',', '{', ']') or (pv[1] == ')' and False):
                    ent['kind'] = 'init'
                if pv[1] == ')' or (pv[1] == 'const' and prev_sig(i, 2)[1] == ')'):
                    if last_closed:
                        callee, inner = last_closed
                        if callee == 'switch':
                            ent['sw'] = inner
                        elif callee not in ('if', 'for', 'while', 'catch') and not any(s.get('fn') for s in stack):
                            ent['fn'] = (callee, inner)
                else:
                    mm = prev_sig(i, 2)
                    if mm[1] == 'namespace' and pv[0] == 'id':
                        ent['ns'] = pv[1]
                stack.append(ent)
            elif tx in (')', ']', '}'):
                pops = {')': '(', ']': '[', '}': '{'}[tx]
                while stack and stack[-1]['ch'] != pops:
                    stack.pop()
                if stack:
                    ent = stack.pop()
                    if tx == ')':
                        inner = ' '.join(t[1] for t in toks[ent['pos'] + 1:i] if t[0] in ('id', 'str'))
                        last_closed = (ent['callee'], inner)
            elif tx == ',':
                if stack and stack[-1]['ch'] == '(':
                    stack[-1]['commas'] += 1
            continue
        if kd != 'num':
            continue
        # ---- a numeric literal
        val = int(re.sub(r'[uUlL]+$', '', tx), 0 if tx[:2].lower() == '0x' else 10)
        if val == 0 or val > 0xFFFFFF:
            continue
        in_june_flag, skip = flag_state()
        if skip:
            continue
        funcs = [s['fn'] for s in stack if s.get('fn')]
        fn = funcs[0] if funcs else None
        in_june_fn = bool(fn and re.search(r'(?i)june', fn[0]))
        in_june_ns = any(s.get('ns') and re.search(r'_CONST_.*_JUNE_', s['ns']) for s in stack)
        pv, nx = prev_sig(i), (toks[i + 1] if i + 1 < n else ('', '', 0))
        pv2, pv3 = prev_sig(i, 2), prev_sig(i, 3)
        top = stack[-1] if stack else None
        if top and top['ch'] == '[':
            continue                                    # array size / subscript
        ctx, prim, name = 'other', [], ''
        if pv[1] == 'case':
            ctx = 'case'
            for s in reversed(stack):
                if s.get('sw') is not None:
                    prim = [s['sw']]
                    break
        elif pv[1] in ('==', '!=', '<=', '>=') or (pv[1] in ('<', '>') and nx[1] != '>'):
            ctx = 'cmp'
            prim = [operand_text(i - 1, -1)]
        elif nx[1] in ('==', '!=', '<=', '>=', '<', '>') and nx[1] not in ('>',) or (nx[1] == '>' and pv[1] in ('(', ',', '&&', '||')):
            ctx = 'cmp'
            prim = [operand_text(i + 1, +1)]
        elif pv[1] == '=':
            if pv3[0] == 'id' or pv3[1] in ('>', '*', ']'):
                ctx, name = 'decl', pv2[1]
            else:
                ctx = 'assign'
            prim = [pv2[1]]
        elif pv[1] == 'return':
            ctx = 'ret'
        elif top and top['ch'] == '(':
            ctx = 'arg'
            callee = top['callee']
            prim = [callee]
            if re.search(r'(?i)skill', callee) and not callee.startswith('static_cast'):
                if top['commas'] == 0 or 'ability' not in callee.lower():
                    prim.append('SKILL_ID')                  # June_GetSkillLevel( id ), June_GetPassiveSkillAbilityValue( id, SA_, ... )
            # a lone cast around the literal in a declaration / assignment: the declared name is the hint
            if callee.startswith('static_cast') and nx[1] == ')':
                k = top['pos'] - 1
                while k >= 0 and toks[k][1] != 'static_cast' and top['pos'] - k < 14:
                    k -= 1
                if k >= 2 and toks[k - 1][1] == '=':
                    ctx, name = 'decl', toks[k - 2][1]
                    prim = [callee, name]
        elif top and top['ch'] == '{' and top['kind'] == 'init':
            ctx = 'table'
            # { "NAME", 12 } : the string before the literal names it
            if pv[1] == ',' and pv2[0] == 'str':
                name = pv2[1].strip('"').lstrip('L"')
                prim = [name]
        if ctx == 'other':
            lhs = stmt_lhs(i)
            if lhs:
                prim = [lhs]
        ln = line_of(off)
        cm = ' '.join(comments.get(ln, [])) or ' '.join(comments.get(ln - 1, []))
        fn_hint = ('%s %s' % fn) if fn else ''
        # decide June-ness
        june = in_june_flag or in_june_fn or in_june_ns or (ctx == 'decl' and name.startswith('JUNE_'))
        if not june:
            continue
        L = Lit()
        L.line, L.text, L.value, L.ctx, L.name, L.func, L.june = ln, tx, val, ctx, name, fn[0] if fn else '', True
        L.note = ''
        cn = comment_names(cm)
        L.hints = (prim, cn, fn_hint)
        L.note = ' '.join(cn[:3])
        lits.append(L)
    return lits


# ---------------------------------------------------------------------------------------------------------------------
# Resolution
# ---------------------------------------------------------------------------------------------------------------------
class Tables(object):
    def __init__(self, reuse):
        self.cpp, self.cppq, self.cpp_states, self.cpp_enums = load_cpp(reuse)
        self.live, self.live_n = load_lua_enum(LIVE_ENUM)
        self.june, self.june_n = load_lua_enum(JUNE_ENUM)

    def lookup(self, fam, v, stem):
        """-> (cpp names, live names, june names)"""
        if fam == 'STATE':
            st = self.cpp_states.get(('c' + stem).lower())
            if st is None:
                st = {}
            return list(st.get(v, [])), [], []
        return list(self.cpp[fam].get(v, [])), list(self.live[fam].get(v, [])), list(self.june[fam].get(v, []))


def resolve(L, T, stem):
    """fill L.family / cls / candidate / in_* ; return False to drop the literal"""
    prim, cn, fnh = L.hints
    v = L.value
    prim_f = hint_families(*prim)
    com_f = [name_family(c) for c in cn if name_family(c)]
    com_f = [f for i, f in enumerate(com_f) if f not in com_f[:i]]
    fn_f = hint_families(fnh)
    strong = bool(prim_f)
    if L.ctx == 'table' and L.name:
        sf = hint_families(L.name)
        strong = True
        prim_f = sf or prim_f
    small_ok = strong and (L.ctx == 'case' or (L.ctx == 'decl' and L.name.startswith('JUNE_')) or (SMALL and L.ctx == 'table'))
    if v < 100 and not small_ok:
        return False
    cands = prim_f + [f for f in com_f if f not in prim_f]
    if not cands and L.ctx == 'ret':
        cands = [f for f in fn_f]
    L.cpp = L.live_ = L.june_ = []
    chosen = None
    for f in cands:
        if f in NO_ENUM_FAMILIES:
            chosen = f
            break
        cpp, live, june = T.lookup(f, v, stem)
        if cpp or live or june:
            chosen = f
            break
    # a table row names its enumerator: look the name up directly, and report a value mismatch
    if L.ctx == 'table' and L.name and cands and cands[0] not in NO_ENUM_FAMILIES:
        f0 = cands[0]
        for q, qv in T.cppq.items():
            if q.split('::')[-1] == L.name:
                chosen = f0
                break
    if chosen in NO_ENUM_FAMILIES:
        L.family, L.cls, L.cand, L.cpp, L.live_, L.june_ = chosen, 'X', '', [], [], []
        return True
    if chosen is None:
        if not cands:
            # no family from the context: lead only when the value matches somewhere and looks id-like
            if not (L.text[:2].lower() == '0x' or v >= 1000):
                return False
            if L.text[:2].lower() != '0x' and v % 100 == 0:
                return False                                    # round decimals are sizes / timings
            if L.text[:2].lower() == '0x' and v >= 256 and (v & (v - 1)) == 0:
                return False                                    # a lone bit is a flag
            hits = []
            for f in ID_FAMILIES:
                cpp, live, june = T.lookup(f, v, stem)
                if cpp or live or june:
                    hits.append((f, cpp, live, june))
            if not hits:
                return False
            f, cpp, live, june = hits[0]
            L.family, L.cls = '?' if len(hits) > 1 else f, 'D'
            L.cpp = sum((h[1] for h in hits), [])
            L.live_ = sum((h[2] for h in hits), [])
            L.june_ = sum((h[3] for h in hits), [])
            L.cand = (L.cpp or L.june_ or L.live_)[0]
            return True
        L.family, L.cls, L.cand, L.cpp, L.live_, L.june_ = cands[0], 'C', '', [], [], []
        return True
    cpp, live, june = T.lookup(chosen, v, stem)
    L.family, L.cpp, L.live_, L.june_ = chosen, cpp, live, june
    if L.ctx == 'table' and L.name:
        byname = [q for q in T.cppq if q.split('::')[-1] == L.name]
        if byname:
            q = byname[0]
            L.cls, L.cand = 'A', q
            L.cpp = [q] if T.cppq[q] == v else cpp
            if T.cppq[q] != v:
                L.note = ('value differs: %s = %d' % (q, T.cppq[q]) + ' ' + L.note).strip()
            return True
    if cpp:
        L.cls, L.cand = 'A', cpp[0]
    elif june or live:
        L.cls, L.cand = 'B', (june or live)[0]
    else:
        L.cls, L.cand = 'C', ''
    return True


def scan_all(files, reuse, T=None, dropped=None):
    T = T or Tables(reuse)
    rows = []
    for f in files:
        path = f if os.path.exists(f) else os.path.join(TRUNK, 'X2Lib', f if f.endswith(('.cpp', '.h')) else f + '.cpp')
        stem = os.path.splitext(os.path.basename(path))[0]
        rel = os.path.relpath(path, TRUNK).replace('\\', '/')
        for L in scan_file(path):
            if resolve(L, T, stem):
                rows.append((rel, L))
            elif dropped is not None:
                dropped.append((rel, L))
    return rows


COLS = ['file', 'line', 'literal', 'hex', 'context', 'family', 'class', 'candidate_name', 'in_cpp', 'in_enumlua', 'in_june', 'note']


def row_text(rel, L):
    return '\t'.join([rel, str(L.line), L.text, '0x%x' % L.value, L.ctx, L.family, L.cls, L.cand,
                      '|'.join(L.cpp[:4]), '|'.join(L.live_[:4]), '|'.join(L.june_[:4]), L.note])


def summary(rows_by_file):
    out = []
    for f, rs in rows_by_file.items():
        c = collections.Counter(r[6] for r in rs)
        out.append('%-34s %4d rows   A %-4d B %-4d C %-4d D %-4d X %-4d' % (f, len(rs), c['A'], c['B'], c['C'], c['D'], c['X']))
    return '\n'.join(out)


def read_tsv():
    rows = collections.OrderedDict()
    if not os.path.exists(TSV):
        sys.exit('no %s: run enumscan.py first' % TSV)
    for line in open(TSV, encoding='utf-8').read().split('\n')[1:]:
        if line:
            r = line.split('\t')
            rows.setdefault(r[0], []).append(r)
    return rows


# ---------------------------------------------------------------------------------------------------------------------
# --verify : the compiler as referee (enumprobe.py's technique)
# ---------------------------------------------------------------------------------------------------------------------
def verify(n, reuse):
    import census as C
    fam, qual, states, enums = load_cpp(reuse)
    pool = []                                    # the families the scan resolves against (not boost / std enumerators)
    for fam_name, d in fam.items():
        pool += [(q, v) for v, qs in d.items() for q in qs if v >= 0]
    for scope_d in states.values():
        pool += [(q, v) for v, qs in scope_d.items() for q in qs if v >= 0]
    rnd = random.Random(20261026)
    sample = rnd.sample(pool, min(n, len(pool)))
    # always include the members the plan names
    for q in ('CX2SkillTree::SI_SA_COMMON_AURA_POWER_ACCEL', 'CX2SkillTree::SI_SA_COMMON_AURA_POWER_ACCEL_UPGRADE'):
        if q in qual and (q, qual[q]) not in sample:
            sample.append((q, qual[q]))
    incs = ['X2GUElsword_SwordMan.h', 'X2GUArme_VioletMage.h', 'X2GULire_ElvenRanger.h', 'X2GURaven.h', 'X2GUEve.h',
            'X2GUChung.h', 'X2GUAra.h', 'X2GUEL.h', 'X2GUAdd.h']
    bad = ok = unprobed = 0
    for c0 in range(0, len(sample), 80):
        chunk = sample[c0:c0 + 80]
        src = ['#define private public', '#define protected public', '#include "stdafx.h"'] + \
              ['#include "%s"' % h for h in incs] + ['static int g_probe = 0;']
        base = len(src)
        for i, (q, v) in enumerate(chunk):
            src.append('char (*probe_%d)[(%s) + 2] = &g_probe;' % (i, q))
        p = os.path.join(TRUNK, 'X2Lib', '_enumscan_probe.cpp')
        open(p, 'w', newline='\r\n').write('\n'.join(src) + '\n')
        cmd = [a for a in C.cl_cmd(p, 'x.i') if not a.startswith('/Fi') and a != '/P'] + ['/c', '/Zs', '/EHsc']
        try:
            r = subprocess.run(cmd, cwd=os.path.join(TRUNK, 'X2Lib'), env=C.cl_env(), capture_output=True)
        finally:
            os.remove(p)
        out = r.stdout.decode('latin1')
        got = {}
        for m in re.finditer(r'_enumscan_probe\.cpp\((\d+)\) : error C2440.*?char \(\*\)\[(\d+)\]', out):
            got[int(m.group(1)) - base - 1] = int(m.group(2)) - 2
        for i, (q, v) in enumerate(chunk):
            if i not in got:
                unprobed += 1
                print('NOT PROBED  %s (parser says %d)' % (q, v))
            elif got[i] != v:
                bad += 1
                print('MISMATCH    %s parser %d compiler %d' % (q, v, got[i]))
            else:
                ok += 1
    print('verify: %d match, %d mismatch, %d not probed (of %d)' % (ok, bad, unprobed, len(sample)))
    return 1 if bad else 0


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('files', nargs='*')
    ap.add_argument('--check', nargs='+', metavar='FILE')
    ap.add_argument('--verify', nargs='?', const=120, type=int, metavar='N')
    ap.add_argument('--summary', action='store_true')
    ap.add_argument('--reuse', action='store_true', help='reuse the cached cl /P output and enum tables')
    ap.add_argument('--small', action='store_true', help='also take values < 100 in tables (e.g. the SA_ key table) when the context names a family')
    ap.add_argument('--dropped', action='store_true', help='also list June literals >= 100 that were dropped (tuning aid)')
    ap.add_argument('--out', default=TSV)
    a = ap.parse_args()
    global SMALL
    SMALL = a.small
    if a.verify is not None:
        return verify(a.verify, a.reuse)
    if a.summary:
        rows = read_tsv()
        print(summary(rows))
        return 0
    if a.check:
        rows = scan_all(a.check, a.reuse)
        bad = [(r, L) for r, L in rows if L.cls in ('A', 'B')]
        for r, L in bad:
            print('%s:%d  %s  %s  class %s  %s' % (r, L.line, L.text, L.family, L.cls, L.cand))
        print('check: %d A/B row(s) remain in %d file(s) (C %d, D %d, X %d ignored)' % (
            len(bad), len(a.check), sum(1 for _, L in rows if L.cls == 'C'), sum(1 for _, L in rows if L.cls == 'D'),
            sum(1 for _, L in rows if L.cls == 'X')))
        return 1 if bad else 0
    files = a.files or DEFAULT_FILES
    dropped = [] if a.dropped else None
    rows = scan_all(files, a.reuse, dropped=dropped)
    for rel, L in (dropped or []):
        if L.value >= 100:
            print('DROPPED %s:%d %s %s hints=%r' % (rel, L.line, L.text, L.ctx, L.hints[0]))
    with open(a.out, 'w', encoding='utf-8', newline='\n') as fh:
        fh.write('\t'.join(COLS) + '\n')
        for rel, L in rows:
            fh.write(row_text(rel, L) + '\n')
    by = collections.OrderedDict()
    for rel, L in rows:
        by.setdefault(rel, []).append(row_text(rel, L).split('\t'))
    print(summary(by))
    print('wrote %s (%d rows)' % (os.path.relpath(a.out, TRUNK), len(rows)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
