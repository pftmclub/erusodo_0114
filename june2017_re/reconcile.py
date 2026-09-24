"""Reconcile the June binary's parser key sets against the keys June's data uses.

    python reconcile.py            # writes reconcile.md; exits non-zero on a failed check

Inputs (all in june2017_re/): june_lua/*.lua + .std.luac + .src.lua (regen.sh),
xorstrings.tsv (xordecrypt.py --all), funcs.txt, and the June binary itself.

Data side. Every table key is extracted twice, and the two sets must be equal
per file, or the run fails:
  * from the .src.lua (a small Lua lexer: constructor keys `K = ...`, and
    `x.K = ...` assignments in code), with its sub-table path;
  * from the bytecode (TSETS string keys, TSETV keys loaded by KSTR, TDUP
    template-table hash keys), via ljd_jun2017.parse_prototype.
ljd is known to mis-structure code-built tables, so the source alone could
silently drop a key. The bytecode alone has no paths. Together they are both
complete and placed. Enum.lua is excluded: its tables are enum members, not keys.

Binary side, per parser function: its decrypted keys (xorstrings.tsv, status
ok) plus the plaintext identifier strings it references directly (sub-table
names such as DAMAGE_DATA are not encrypted). Numbered keys match `%d`
templates and `<plaintext>` + `%s%d`. `K_ULTIMATE` matches when K is passed to a
`"%s"/"%s_ULTIMATE"` wrapper (a function whose own keys include `%s_ULTIMATE`).

Each data key is `both` or `data-only`; each data-only key gets an explanation:
  other-func   the key is decrypted / referenced in a different function (VAs given)
  plain-only   the key exists as a plaintext string in the binary, but no direct
               reference to it was found (it may be reached via a pointer table)
  decoder-miss? a non-ok xorstrings.tsv row is compatible with it
  typo         within edit distance 2 of a key the binary reads (a studio data typo)
  removed-in-June  hand-verified (MANUAL below), with the evidence
  unread       none of the above: the June binary never names it
"""
import collections
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import ljd_jun2017 as LJ  # noqa: E402
import xordecrypt as X    # noqa: E402

LUA = os.path.join(HERE, 'june_lua')
DE_FILES = ['DamageEffect', 'DamageEffect_Character', 'DamageEffect_Dungeon', 'DamageEffect_ETC', 'DamageEffect_INT']
ES_FILES = ['EffectSet', 'EffectSet_INT', 'UIEffectSet', 'UIEffectSet_INT', 'UnitViewerEffectSetTable', 'EffectSet_Description']
OTHER_FILES = ['Elsword_DamageData', 'DamageSound']

# ---------------------------------------------------------------- source side
TOK = re.compile(r'''
    (?P<ws>\s+) | (?P<com>--\[\[.*?\]\]|--[^\n]*) |
    (?P<str>"(?:[^"\\\n]|\\.)*"|'(?:[^'\\\n]|\\.)*') |
    (?P<num>0[xX][0-9a-fA-F]+|\d+\.?\d*(?:[eE][-+]?\d+)?) |
    (?P<name>[A-Za-z_][A-Za-z0-9_]*) |
    (?P<op>==|~=|<=|>=|\.\.\.|\.\.|[-+*/%^#<>=(){}\[\];:,.])
''', re.X | re.S)


def tokens(text):
    out = []
    pos = 0
    while pos < len(text):
        m = TOK.match(text, pos)
        if not m:
            raise SystemExit('lexer stuck at %d: %r' % (pos, text[pos:pos + 40]))
        pos = m.end()
        k = m.lastgroup
        if k in ('ws', 'com'):
            continue
        out.append((k, m.group(k)))
    return out


def norm(name):
    """EFFECT12 -> EFFECT%d, for grouping paths (not for matching keys)."""
    m = re.match(r'^(.*?[A-Za-z_])(\d+)$', name)
    return m.group(1) + '%d' if m else name


def source_keys(path):
    """-> list of (templet, path tuple, key, value_is_table). Path excludes the templet."""
    toks = tokens(open(path, encoding='utf-8', errors='replace').read())
    out = []
    stack = []      # one entry per open '{', '(' : ('tab', name) / ('par', None)
    templet = None
    for i, (k, v) in enumerate(toks):
        prev = toks[i - 1] if i else ('', '')
        prev2 = toks[i - 2] if i > 1 else ('', '')
        if v == '{' and k == 'op':
            if prev[1] == '=' and prev2[0] == 'name':
                name = prev2[1]
            elif prev[1] == '(' and prev2[0] == 'name' and i > 2 and toks[i - 3][1] == ':':
                name = 'call:' + prev2[1]
            else:
                name = '[]'
            tabs = [s for s in stack if s[0] == 'tab']
            if not tabs:
                templet = name
            stack.append(('tab', name))
            continue
        if v in ('(',) and k == 'op':
            stack.append(('par', None))
            continue
        if v in ('}', ')') and k == 'op':
            stack.pop()
            continue
        if k == 'name' and i + 1 < len(toks) and toks[i + 1][1] == '=':
            tabs = [s for s in stack if s[0] == 'tab']
            is_tab = i + 2 < len(toks) and toks[i + 2][1] == '{'
            if stack and stack[-1][0] == 'tab' and prev[1] in ('{', ','):
                path = tuple(norm(s[1]) for s in tabs[1:])
                out.append((templet, path, v, is_tab))
            elif prev[1] == '.' and prev2[0] == 'name':
                out.append((None, ('<code>',), v, is_tab))   # x.K = ... in a helper
    return out


# -------------------------------------------------------------- bytecode side
def bytecode_keys(lua_path):
    root = LJ.parse_prototype(lua_path)
    keys = collections.Counter()
    todo = [root]
    while todo:
        p = todo.pop()
        kc = p.constants.complex_constants
        kstr = {}
        for ins in p.instructions:
            n = ins.name
            if n == 'TSETS':
                keys[kc[ins.CD]] += 1
            elif n == 'TSETV' and ins.CD in kstr:
                keys[kstr[ins.CD]] += 1
            elif n == 'TDUP':
                for key, _ in kc[ins.CD].dictionary:
                    if isinstance(key, str):
                        keys[key] += 1
            if n == 'KSTR':
                kstr[ins.A] = kc[ins.CD]
            elif ins.A_type in (1, 2) and not n.startswith('TSET'):  # T_DST / T_BS write A
                kstr.pop(ins.A, None)
        for c in kc:
            if hasattr(c, 'instructions'):
                todo.append(c)
    return keys


# ---------------------------------------------------------------- binary side
def load_xor():
    ok = collections.defaultdict(set)
    bad = []
    for line in open(os.path.join(HERE, 'xorstrings.tsv'), encoding='utf-8').read().split('\n')[1:]:
        c = line.split('\t')
        if len(c) < 10:
            continue
        f = int(c[1], 16)
        if c[6] == 'ok':
            ok[f].add(c[5])
        else:
            bad.append((f, c[5], c[6]))
    return ok, bad


IDENT = re.compile(r'^[A-Za-z_][A-Za-z0-9_]*(%d|%s)?[A-Za-z0-9_]*$')


def plain_at(d, va):
    if not (X.TEXT_VA <= va < X.TEXT_VA + X.TEXT_RAWSIZE):
        return None
    o = X.raw(va)
    m = re.match(rb'[\x20-\x7e]{2,80}\x00', d[o:o + 81])
    return m.group(0)[:-1].decode() if m else None


class Binary:
    def __init__(self):
        self.d = X.data()
        self.starts = X.load_funcs()
        self.xor_ok, self.xor_bad = load_xor()
        self.xor_index = collections.defaultdict(set)   # key -> funcs decrypting it
        for f, ks in self.xor_ok.items():
            for k in ks:
                self.xor_index[k].add(f)
        self._fn = {}
        self._helper = {}

    def func_of(self, va):
        import bisect
        i = bisect.bisect_right(self.starts, va) - 1
        return self.starts[i] if i >= 0 else None

    def fn(self, f):
        """-> dict(keys=set, plain=[(va, s)], calls=[(va, target)]) for one function."""
        if f in self._fn:
            return self._fn[f]
        from capstone import CS_GRP_CALL
        from capstone.x86 import X86_OP_IMM, X86_OP_MEM
        plain, calls = [], []
        for x in X.disasm(f, X.func_end(self.starts, f)):
            if x.group(CS_GRP_CALL) and x.operands and x.operands[0].type == X86_OP_IMM:
                calls.append((x.address, x.operands[0].imm & 0xffffffff))
                continue
            for op in x.operands:
                v = op.imm & 0xffffffff if op.type == X86_OP_IMM else (
                    op.mem.disp & 0xffffffff if op.type == X86_OP_MEM and op.mem.base == 0 else None)
                s = plain_at(self.d, v) if v is not None else None
                if s:
                    plain.append((x.address, s))
        keys = set(self.xor_ok.get(f, ()))
        keys |= set(s for _, s in plain if IDENT.match(s))
        # <plaintext table name> + "%s%d" in the same function -> NAME%d
        if any(s == '%s%d' for _, s in plain):
            keys |= set(s + '%d' for _, s in plain if IDENT.match(s) and '%' not in s)
        r = {'keys': keys, 'plain': plain, 'calls': calls}
        self._fn[f] = r
        # ... or passed to a small "%s%d" helper, e.g. FUN_31c7b980(name, index, flag):
        # snprintf(buf, 0xff, "%s%d", name, index) then open that table. The name is
        # the last plaintext pushed before the call (EffectSet: "EFFECT" @321e5cef).
        for site, t in calls:
            if t != f and self.is_numbered_helper(t):
                before = [s for va, s in plain if va < site and site - va < 0x20 and IDENT.match(s) and '%' not in s]
                if before:
                    keys.add(before[-1] + '%d')
        return r

    def is_numbered_helper(self, t):
        if t in self._helper:
            return self._helper[t]
        self._helper[t] = False   # break recursion before fn(t)
        r = self.fn(t)
        ok = any(s == '%s%d' for _, s in r['plain']) and len(r['calls']) <= 4
        self._helper[t] = ok
        return ok

    def ultimate_bases(self, f):
        """Keys passed to a "%s"/"%s_ULTIMATE" wrapper: the last plaintext name before the call."""
        r = self.fn(f)
        out = set()
        for site, t in r['calls']:
            if '%s_ULTIMATE' in self.xor_ok.get(t, ()):
                before = [s for va, s in r['plain'] if va < site and site - va < 0x40 and IDENT.match(s)]
                if before:
                    out.add((before[-1], site, t))
        return out

    def plain_refs(self, key):
        """Functions that reference a plaintext copy of `key` (ASCII, NUL-terminated)."""
        d = self.d
        pat = key.encode() + b'\x00'
        hits, refs = [], set()
        i = d.find(pat)
        while i != -1:
            # A hit counts if an instruction references its first byte, whatever
            # precedes it (OR_OPERATION follows junk ending in '1'). An unreferenced
            # hit counts only on a clean boundary, so a mere suffix of a longer
            # string is not reported.
            if X.TEXT_RAW <= i < X.TEXT_RAW + X.TEXT_RAWSIZE:
                va = i - X.TEXT_RAW + X.TEXT_VA
                b = va.to_bytes(4, 'little')
                found = set()
                j = d.find(b, X.TEXT_RAW, X.TEXT_RAW + X.TEXT_RAWSIZE)
                while j != -1:
                    found.add(self.func_of(j - X.TEXT_RAW + X.TEXT_VA))
                    j = d.find(b, j + 1, X.TEXT_RAW + X.TEXT_RAWSIZE)
                if found or d[i - 1] == 0:
                    hits.append(va)
                    refs |= found
            i = d.find(pat, i + 1)
        return hits, refs


def matcher(keys):
    exact = set(k for k in keys if '%' not in k)
    pats = [re.compile('^' + re.escape(k).replace('%d', r'\d+').replace('%s', r'.+') + '$') for k in keys if '%' in k]

    def m(key):
        if key in exact:
            return key
        for p in pats:
            if p.match(key):
                return p.pattern
        return None
    return m


# (sub-table name, reader functions, evidence). The deepest path component that
# matches a rule picks the reader; otherwise the parent's rule applies.
CE = 0x31f7aab0
DE_RULES = [
    (r'DAMAGE_DATA(_ULTIMATE|_NEXT|_NEXT_ULTIMATE|_CHANGE_TIME.*)?', [0x32632390, 0x31f79c20],
     'plaintext DAMAGE_DATA* refs in CE ctor 31f7b42c/31f7c632/31f7d894/31f7e86e; SetDamageData 32632390'),
    (r'LOCK_ON_DATA(_ULTIMATE)?', [0x31f71930], 'LOCK_ON_DATA refs 31f7e7ab/31f7e7bb -> calls 31f7e7b2/31f7e7c2'),
    (r'BLACKHOLE_DATA(_ULTIMATE)?', [0x31f637e0], 'BLACKHOLE_DATA refs 31f7e7ce/31f7e7e2 -> calls 31f7e7d5/31f7e7e9'),
    (r'SIN_MOVE_DATA', [0x31f64200], 'plaintext SIN_MOVE_DATA @31f6421f'),
    (r'CAMERA%d', [0x31f650f0], 'CAMERA + %s%d @31f6512e'),
    (r'SCREEN%d', [0x31f65730], 'SCREEN + %s%d @31f6577a'),
    (r'SOUND%d', [0x31f73f30], 'SOUND + %s%d @31f73f94'),
    (r'CREATE_DAMAGE_EFFECT%d', [0x31f6ac60], 'CREATE_DAMAGE_EFFECT @31f6acc5'),
    (r'DIE_DAMAGE_EFFECT(_ULTIMATE)?(%d)?', [0x31f68f30, 0x31f6e7e0], 'DIE_DAMAGE_EFFECT[_ULTIMATE] @31f6e834/31f6e824 -> 31f68f30'),
    (r'HIT_DAMAGE_EFFECT(_ULTIMATE)?(%d)?', [0x31f6ceb0, 0x31f6e850], 'HIT_DAMAGE_EFFECT[_ULTIMATE] @31f6e8a4/31f6e894 -> 31f6ceb0'),
    (r'TOUCH_LAND_DAMAGE_EFFECT', [0x31f76a90], 'TOUCH_LAND_DAMAGE_EFFECT @31f76afe'),
    (r'HIT_EFFECT_SET%d', [0x31f64c80], 'HIT_EFFECT_SET + %s%d @31f64cde'),
    (r'DIE_CREATE_NPC%d', [0x31f66230], 'DIE_CREATE_NPC + %s%d @31f66275'),
    (r'AFTER_IMAGE', [0x31f74a80], 'AFTER_IMAGE @31f74a9f'),
    (r'DAMAGE_EFFECT_DIE_CHECK_CONDITION|CONDITION_%d', [0x31f66700, CE],
     'the ctor opens DAMAGE_EFFECT_DIE_CHECK_CONDITION @31f7f94e and its AND_OPERATION / OR_OPERATION '
     'containers @31f7f95f / 31f7f96e, then calls 31f66700 (CONDITION_ + %s%d @31f66755) for each'),
    (r'(START|PASSIVE|TIME|HIT)_PARTICLE%d', [0x31f66e60, 0x31f6a7b0, 0x31f6a940, 0x31f6aad0, 0x31f6cd00],
     'START/PASSIVE/TIME/HIT_PARTICLE + %s%d in wrappers 31f6a7b0/31f6a940/31f6aad0/31f6cd00 -> 31f66e60'),
]
DE_TOP = [CE, 0x31f8ba80, 0x31f8b370]

# Hand-verified verdicts for keys the automatic categories cannot settle.
MANUAL = {
    'ANIMATION_SPEED_UP': ('removed-in-June',
        'March reads it in SetDamageDataFromLUA (X2DamageManager.cpp:360) between IGNORE_DEFENCE and '
        'ATTACK_TRIGGER_TYPE. June 32632390 decrypts NO_BUFF, RATE_MODIFIER, IGNORE_DEFENCE, then the new '
        'IGNORE_DEFENCE_ON_PVP/_RATE/_RATE_ON_PVP/_DECLINE, then ATTACK_TRIGGER_TYPE @3263cebe: same order, '
        'these two gone, no gap in the decrypt-site spacing'),
    'ANIMATION_SPEED_UP_RATE': ('removed-in-June', 'see ANIMATION_SPEED_UP (March X2DamageManager.cpp:361)'),
    'BUFF_FACTOR_RELATIVE_MEMO_ID': ('unread',
        'March reads it only under #ifdef ADD_MEMO_1ST_CLASS (X2DamageManager.cpp:3511). June\'s counterpart '
        '32630d20 decrypts the non-memo neighbours (BUFF_FACTOR_RELATIVE_SKILL_ID, RELATIVE_SKILL_LEARN_CHECK, '
        'BUFF_FACTOR_RELATIVE_MASTER_UNIT) but not this; nothing else in the binary names it. Memo path absent '
        'from June\'s build (removed or compiled out: not settled)'),
    'BUFF_FACTOR_ID_MEMO': ('unread', 'as BUFF_FACTOR_RELATIVE_MEMO_ID (March X2DamageManager.cpp:3526, same #ifdef)'),
}


def edit1(a, b):
    """Damerau-Levenshtein distance, capped: small keys only."""
    if abs(len(a) - len(b)) > 2:
        return 9
    prev2, prev = None, list(range(len(b) + 1))
    for i, ca in enumerate(a, 1):
        cur = [i] + [0] * len(b)
        for j, cb in enumerate(b, 1):
            cur[j] = min(prev[j] + 1, cur[j - 1] + 1, prev[j - 1] + (ca != cb))
            if prev2 is not None and i > 1 and j > 1 and ca == b[j - 2] and a[i - 2] == cb:
                cur[j] = min(cur[j], prev2[j - 2] + 1)
        prev2, prev = prev, cur
    return prev[-1]


_MARCH = None


def march_reads(key):
    """Quoted-literal hits for key in March's X2Lib/KTDXLIB (a lead only: stringizing
    macros can hide a read, see the plan's 'Dead ends')."""
    global _MARCH
    if _MARCH is None:
        _MARCH = []
        root = os.path.join(HERE, '..')
        for sub in ('X2Lib', 'KTDXLIB'):
            for dp, _, fns in os.walk(os.path.join(root, sub)):
                for fn in fns:
                    if fn.endswith(('.cpp', '.h')):
                        p = os.path.join(dp, fn)
                        _MARCH.append((os.path.relpath(p, root).replace('\\', '/'), open(p, 'rb').read().decode('latin1')))
    pat = '"%s"' % key
    for rel, txt in _MARCH:
        i = txt.find(pat)
        if i != -1:
            return '%s:%d' % (rel, txt.count('\n', 0, i) + 1)
    return None
ES_TOP = [0x321e48e0]


def es_family(B):
    """The EffectSet parser plus its callees (two levels) inside the EffectSet region."""
    fam = set(ES_TOP)
    frontier = list(ES_TOP)
    for _ in range(2):
        nxt = []
        for f in frontier:
            for _, t in B.fn(f)['calls']:
                if 0x321d0000 <= t < 0x32208000 and t not in fam:
                    fam.add(t)
                    nxt.append(t)
        frontier = nxt
    return sorted(fam)


def de_reader(path):
    for comp in reversed(path):
        for pat, funcs, ev in DE_RULES:
            if re.fullmatch(pat, comp):
                return pat, funcs
    return '<CEffect top level>', DE_TOP


def main():
    report = []
    fail = []
    data = {}  # file -> list of (templet, path, key, is_tab)
    for grp in (DE_FILES, ES_FILES, OTHER_FILES):
        for stem in grp:
            lua = os.path.join(LUA, stem + '.lua')
            src = source_keys(os.path.join(LUA, stem + '.src.lua'))
            bc = bytecode_keys(lua)
            s_set = set(k for _, _, k, _ in src)
            b_set = set(bc)
            only_s, only_b = sorted(s_set - b_set), sorted(b_set - s_set)
            line = '%s: %d source keys, %d bytecode keys' % (stem, len(s_set), len(b_set))
            if only_s or only_b:
                line += ' -- MISMATCH source-only %s bytecode-only %s' % (only_s[:10], only_b[:10])
                fail.append(stem)
            print(line)
            data[stem] = src
    if fail:
        sys.exit('source/bytecode key sets differ for %s' % ', '.join(fail))
    eq_lines = ['| `%s.lua` | %d | %d |' % (s, len(set(k for _, _, k, _ in data[s])), len(set(k for _, _, k, _ in data[s])))
                for s in data]

    B = Binary()
    esf = es_family(B)
    all_de = sorted(set(DE_TOP) | set(f for _, fs, _ in DE_RULES for f in fs))

    # ---- assign every data key to a scope
    scopes = collections.OrderedDict()   # name -> {'readers', 'evidence', 'keys': {key: [count, example]}}

    def add(scope, readers, ev, key, templet, stem, path=()):
        s = scopes.setdefault(scope, {'readers': readers, 'evidence': ev, 'keys': {}})
        e = s['keys'].setdefault(key, [0, '%s:%s' % (stem, templet), set()])
        e[0] += 1
        e[2].add(path)

    ev_of = dict((p, ev) for p, _, ev in DE_RULES)
    for stem in DE_FILES:
        for templet, path, key, _ in data[stem]:
            if path == ('<code>',):
                add('DamageEffect code-built tables (Get* helpers)', all_de, 'union of every DamageEffect reader', key, '<helper>', stem)
                continue
            pat, readers = de_reader(path)
            add('DamageEffect ' + pat, readers, ev_of.get(pat, 'CEffect ctor 31f7aab0 + %s wrappers 31f8ba80/31f8b370'), key, templet, stem, path)
    for stem in ES_FILES:
        for templet, path, key, _ in data[stem]:
            add('EffectSet (all sub-tables)', esf, 'FUN_321e48e0 + its callees in 0x321d0000-0x32208000', key, templet, stem, path)
    for stem, scope in (('Elsword_DamageData', 'InsertDamageData_LUA'), ('DamageSound', 'DamageSound')):
        for templet, path, key, _ in data[stem]:
            add(scope, [], 'no reader identified yet: every key is looked up binary-wide', key, templet, stem)

    # ---- global helpers for explanations
    for f in all_de + esf:
        B.fn(f)
    tpl_owner = collections.defaultdict(set)
    for k, fs in B.xor_index.items():
        if '%d' in k and IDENT.match(k):
            tpl_owner[k] |= fs
    for f, r in B._fn.items():
        for k in r['keys']:
            if '%d' in k:
                tpl_owner[k].add(f)
    glob_tpl = sorted(tpl_owner)
    glob_m = matcher(glob_tpl)
    bad_pats = []
    for f, txt, st in B.xor_bad:
        known = sum(ch != '?' for ch in txt)
        if '?' in txt and known >= 3 and known * 2 >= len(txt):
            bad_pats.append((f, re.compile('^' + re.escape(txt).replace(r'\?', '.') + '$'), st))

    typo_pool = sorted(k for k in B.xor_index if re.match(r'^[A-Z][A-Z0-9_]+$', k))

    def closure(readers, depth=3):
        seen = set(readers)
        frontier = list(readers)
        for _ in range(depth):
            nxt = []
            for f in frontier:
                for _, t in B.fn(f)['calls']:
                    if t not in seen and X.TEXT_VA <= t < X.TEXT_VA + X.TEXT_RAWSIZE:
                        seen.add(t)
                        nxt.append(t)
            frontier = nxt
        return seen

    def placed(fs, reach, what):
        """other-func only if a naming function is on the scope's call path."""
        on = sorted(f for f in fs if reach is None or f in reach)
        if on:
            return 'other-func', '%s in callee %s' % (what, ', '.join('%08x' % f for f in on[:4]))
        return 'name-collision', '%s only in unrelated %s: not on this scope\'s call path' % (
            what, ', '.join('%08x' % f for f in sorted(fs)[:4]))

    def explain(key, scope_keys, reach):
        if key in MANUAL:
            return MANUAL[key]
        # Pool every function that names the key -- decrypted, via a %d template, or
        # by plaintext reference -- before asking whether one is on the call path.
        # (Stopping at the first source mislabelled BUFF_FACTOR_ID: decrypted only in
        # unrelated functions, but the callee ParsingBuffFactorID may name it too.)
        namers = {}
        for f in B.xor_index.get(key, ()):
            namers.setdefault(f, 'decrypted')
        t = glob_m(key)
        if t:
            for k in glob_tpl:
                if re.match('^' + re.escape(k).replace('%d', r'\d+') + '$', key):
                    for f in tpl_owner[k]:
                        namers.setdefault(f, 'template %s' % k)
        hits, refs = B.plain_refs(key)
        refs.discard(None)
        for f in refs:
            namers.setdefault(f, 'plaintext referenced')
        if namers:
            on = sorted(f for f in namers if reach is None or f in reach)
            if on:
                return 'other-func', 'named in callee %s' % ', '.join('%08x (%s)' % (f, namers[f]) for f in on[:4])
            return 'name-collision', 'named only in unrelated %s: not on this scope\'s call path' % (
                ', '.join('%08x (%s)' % (f, namers[f]) for f in sorted(namers)[:4]))
        if hits:
            return 'plain-only', 'plaintext at ' + ', '.join('%08x' % h for h in hits[:3]) + ', no direct reference found'
        for f, p, st in bad_pats:
            if p.match(key):
                return 'decoder-miss?', 'compatible with non-ok row in %08x (%s)' % (f, st)
        for f, txt, st in B.xor_bad:
            if key in txt:
                return 'decoder-miss?', 'inside non-ok row %r in %08x (%s)' % (txt[:50], f, st)
        # A studio typo in the data: close to a key the binary does read. June
        # ignores it exactly as it ignores any unknown key.
        for pool, where in ((sorted(k for k in scope_keys if '%' not in k), 'this scope'), (typo_pool, 'binary-wide')):
            best = min(((edit1(key.upper(), k), k) for k in pool if abs(len(k) - len(key)) <= 2), default=(9, None))
            limit = 2 if where == 'this scope' else 1
            if best[1] and (best[0] <= limit or key.upper() == best[1]) and len(key) >= 5:
                owners = sorted(B.xor_index.get(best[1], ()))[:3]
                return 'typo', 'data typo of `%s` (%s%s); the binary never reads this spelling' % (
                    best[1], where, ', decrypted in ' + ', '.join('%08x' % f for f in owners) if owners else '')
        m = march_reads(key)
        return 'unread', 'not decrypted, not plaintext, no compatible failed row; March: %s' % (
            'reads it at ' + m if m else 'no quoted literal in X2Lib/KTDXLIB')

    # ---- classify
    group_keys = collections.defaultdict(set)   # scope family -> every data key and sub-table name
    for name, s in scopes.items():
        fam = 'ES' if name.startswith('EffectSet') else 'DE'
        group_keys[fam] |= set(s['keys'])
    for stem in DE_FILES:
        for _, path, _, _ in data[stem]:
            group_keys['DE'] |= set(path)

    results = []
    for name, s in scopes.items():
        keys = set()
        ult = set()
        for f in s['readers']:
            keys |= B.fn(f)['keys']
            for base, site, t in B.ultimate_bases(f):
                ult.add((base, site, t))
        keys |= set(b + '_ULTIMATE' for b, _, _ in ult)
        m = matcher(keys)
        # Scopes with no identified reader (InsertDamageData_LUA, DamageSound) take the
        # whole binary as their call path: their reader is what the lookup finds.
        reach = closure(s['readers']) if s['readers'] else None
        both, only = [], []
        # Sub-tables opened by a callee (not by the readers themselves): their
        # contents belong to that loader's handler, which may be reached indirectly.
        callee_names = {}
        for f in sorted((reach or set()) - set(s['readers'])):
            for kk in B.fn(f)['keys']:
                callee_names.setdefault(kk, f)
        cm = matcher(list(callee_names))
        # Completeness of the readers: every guard is a decoded ok buffer or a
        # non-string static, and no reader has a non-ok row. Then a key none of
        # them names is provably not asked for by this scope's loader.
        complete, acct = True, []
        for f in s['readers']:
            fnx = X.Func(f, X.func_end(B.starts, f))
            rows = fnx.buffers()
            okn = sum(r['status'] == 'ok' for r in rows)
            acct.append('%08x: %d guards = %d ok + %d static' % (f, len(fnx.guards), okn, fnx.statics))
            if okn != len(rows) or len(fnx.guards) != okn + fnx.statics:
                complete = False
        s['acct'] = (complete, acct)
        for k, (n, ex, paths) in sorted(s['keys'].items()):
            hit = m(k)
            if hit:
                both.append((k, n, hit))
                continue
            cat, why = explain(k, keys, reach)
            if cat == 'name-collision':
                for pth in sorted(paths, key=len, reverse=True):
                    opener = next(((c, callee_names.get(c) or callee_names.get(cm(c) or '')) for c in reversed(pth) if cm(c)), None)
                    if opener:
                        cat = 'nested-subtable'
                        why = "under `%s`, opened by callee %08x on this scope's path; %s" % (
                            '/'.join(pth), opener[1], why.replace(": not on this scope's call path", ' (reached indirectly)'))
                        break
            if cat == 'name-collision' and complete and s['readers']:
                cat = 'dead-here'
                why = ("this scope's readers never ask for it (every guard accounted for, see "
                       "the scope header; no callee within 3 levels names it); ") + why.replace(
                       ": not on this scope's call path", ' read it for other sub-tables')
            only.append((k, n, ex, cat, why))
        results.append((name, s, both, only, ult))

    # binary-only: per reader function, names no data key or sub-table (in its family) matches
    bin_only = collections.OrderedDict()
    for fam, funcs in (('DE', all_de), ('ES', esf)):
        dk = group_keys[fam]
        for f in funcs:
            src_x = B.xor_ok.get(f, set())
            lst = []
            for k in sorted(B.fn(f)['keys']):
                if not re.match(r'^[A-Z][A-Z0-9_]*(%d)?[A-Z0-9_]*$', k):
                    continue
                if k in dk:
                    continue
                if k + '%d' in B.fn(f)['keys']:
                    continue  # bare prefix of a numbered table (NAME + "%s%d"), not a key
                if '%d' in k and any(matcher([k])(x) for x in dk):
                    continue
                lst.append((k, 'xor' if k in src_x else 'plain'))
            if lst:
                bin_only['%08x' % f] = lst
    all_bin_only = set(k for v in bin_only.values() for k, _ in v)

    # exit checks
    onechar = []
    for name, s, both, only, ult in results:
        for k, n, ex, cat, why in only:
            if k[:-1] in all_bin_only:
                onechar.append((k, k[:-1], name))
    cats = collections.Counter(cat for r in results for *_, cat, _ in r[3])

    # ---- write reconcile.md
    L = []
    w = L.append
    w('# June2017 key reconciliation (Phase 0)')
    w('')
    w('Generated by `python reconcile.py` from `june_lua/` (decompiled by `ljd_jun2017.py`),')
    w('`xorstrings.tsv` (`xordecrypt.py --all`) and `unpacked_x2.exe`. Do not edit by hand;')
    w('rerun it. `JUNE2017_EFFECT_REBUILD_PLAN.md` Phase 0 defines the terms.')
    w('')
    w('## Checks')
    w('')
    w('- **Source vs. bytecode keys:** equal in every file (the run aborts otherwise). ljd')
    w('  dropped and invented nothing, including in the code-built `Get*` helper tables.')
    w('- **Data-only keys that are a one-character extension of a binary-only key** (the')
    w('  dropped-last-character signature): %s.' % ('**none**' if not onechar else ', '.join('`%s` vs `%s` (%s)' % o for o in onechar)))
    w('- **Data-only explanations:** %s.' % ', '.join('%s %d' % kv for kv in sorted(cats.items())))
    w('')
    w('| File | source keys | bytecode keys |')
    w('|---|---|---|')
    L += eq_lines
    w('')
    w('Explanation categories for data-only keys:')
    w('')
    w('- `other-func`: a **callee** of this scope\'s readers (3 levels of direct calls) names')
    w('  the key, so the loader does read it, one call down.')
    w('- `nested-subtable`: the key sits under a sub-table that a callee opens, e.g.')
    w('  `DAMAGE_DATA/BUFF_FACTOR/BEHAVIOR/...`. It is read by the functions listed, which')
    w('  are reached indirectly (virtual or factory dispatch), not by direct calls.')
    w('- `dead-here`: the key is used in this sub-table, but this sub-table\'s loader provably')
    w('  never asks for it. Every guard in every reader is a decoded `ok` buffer or a')
    w('  non-string static (see each scope\'s "Guard accounting"), none is this key, and no')
    w('  callee names it. Other loaders read the same name for *their* sub-tables. This is')
    w('  data copy-pasted between block types; June ignores it, and so must we.')
    w('- `name-collision`: as `dead-here`, but the readers\' guard accounting is incomplete,')
    w('  so a missed read cannot be ruled out.')
    w('- `plain-only`: the key exists as a plaintext string, but nothing references it')
    w('  directly. A pointer table may reach it.')
    w('- `decoder-miss?`: a non-ok `xorstrings.tsv` row is compatible with the key.')
    w('- `typo`: within edit distance 2 of (or a case variant of) a key the binary reads.')
    w('  It is a studio typo in the data, which June ignores like any unknown key. Keep it')
    w('  verbatim; do not "fix" the data, and do not add a read for it.')
    w('- `removed-in-June`: hand-verified against March\'s read order (`MANUAL` in the script).')
    w('- `unread`: dead data. Nothing in the binary names it: not decrypted in any function,')
    w('  not a plaintext string, and not inside or compatible with any failed decoder row.')
    w('  That is only credible because decoder coverage')
    w('  is 6180/6240 buffers clean with every failure labelled; see `README.md`. The')
    w('  March column is a lead, not proof (stringizing macros can hide a March read).')
    w('')
    w('## Summary')
    w('')
    w('| Scope | Readers | data keys | both | data-only |')
    w('|---|---|---|---|---|')
    for name, s, both, only, ult in results:
        w('| %s | %s | %d | %d | %d |' % (name, ', '.join('`%08x`' % f for f in s['readers'][:6]) + (' …' if len(s['readers']) > 6 else '') or '—',
                                       len(s['keys']), len(both), len(only)))
    w('')
    for name, s, both, only, ult in results:
        w('## %s' % name)
        w('')
        w('Readers: %s. Evidence: %s.' % (', '.join('`%08x`' % f for f in s['readers']) or '—', s['evidence']))
        if s.get('acct') and s['acct'][1]:
            w('')
            w('Guard accounting (%s): %s.' % ('complete' if s['acct'][0] else '**incomplete**', '; '.join(s['acct'][1])))
        if ult:
            w('`_ULTIMATE` variants via wrapper: %s.' % ', '.join('`%s` @%08x→%08x' % u for u in sorted(ult)))
        w('')
        w('Intersection (%d): %s' % (len(both), ' '.join('`%s`' % k for k, _, _ in both)))
        w('')
        if only:
            w('Data-only (%d):' % len(only))
            w('')
            w('| Key | uses | example | category | explanation |')
            w('|---|---|---|---|---|')
            for k, n, ex, cat, why in only:
                w('| `%s` | %d | %s | %s | %s |' % (k, n, ex.replace('|', '/'), cat, why))
            w('')
    w('## Binary-only keys (read by a parser function, used by no data in its family)')
    w('')
    w('`xor` = decrypted key, `plain` = plaintext string the function references.')
    w('')
    for f, lst in bin_only.items():
        w('- `%s`: %s' % (f, ' '.join('`%s`%s' % (k, '' if s == 'xor' else '(plain)') for k, s in lst)))
    w('')
    open(os.path.join(HERE, 'reconcile.md'), 'w', encoding='utf-8', newline='\n').write('\n'.join(L))
    print('reconcile.md: %s; one-char extensions: %d' % (dict(cats), len(onechar)))
    if onechar:
        sys.exit(1)


if __name__ == '__main__':
    main()
