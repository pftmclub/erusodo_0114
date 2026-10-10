r"""Class-aware Lua binding census (June effect C3z-4, 2026-10-10).

June side: the two binder string rows of func_strings.tsv (31be67a0 = KTDX engine binder, 31d95080 = X2 binder). Each row reads as
a class name followed by that class's methods / members, in registration order.
March side: every client file that holds a lua_tinker binder, preprocessed with cl /P under the US_SERVICE defines (census.py's
recipe), so a class_def inside a dead #ifdef does not count. class_inh is walked, so a method bound on a base class counts.

Gap = June(C) - March(C and its March ancestors). Reach = every `x:M(` and `x.M(` call in a script a registered dungeon reaches
(c3z1_unbound_reach.py's reach), plus every June source when --all.

    python june2017_re/c3z4_class_census.py [--reuse-pp]   (from the Trunk; ~1 min with cl /P, seconds with --reuse-pp)
writes june2017_re/c3z4_class_gap.tsv (all gaps) and june2017_re/c3z4_unbound.tsv (gaps a reached script calls)
"""
import os, re, csv, sys, subprocess, collections
HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..'))
sys.path.insert(0, HERE)
import census as C   # cl_cmd / cl_env / X2LIB_*

PP = os.path.join(HERE, 'pp')
FILES = ['X2Lib/X2Main.cpp', 'X2Lib/X2EventScene.cpp', 'X2Lib/X2World.cpp', 'X2Lib/X2ItemManager.cpp',
         'X2Lib/X2PVPResultTable.cpp', 'KTDXLIB/KTDXApp.cpp']

def preprocess(reuse):
    out = {}
    procs = []
    for f in FILES:
        src = os.path.join(TRUNK, f)
        i = os.path.join(PP, 'c3z4_' + os.path.basename(f)[:-4] + '.i')
        out[f] = i
        if reuse and os.path.exists(i):
            continue
        cwd = os.path.join(TRUNK, os.path.dirname(f))
        procs.append((f, subprocess.Popen(C.cl_cmd(src, i), cwd=os.path.join(TRUNK, 'X2Lib'), env=C.cl_env(),
                                          stdout=subprocess.PIPE, stderr=subprocess.STDOUT)))
    for f, p in procs:
        o = p.communicate()[0]
        if p.returncode:
            sys.exit('cl /P failed on %s:\n%s' % (f, o.decode('latin1')[-3000:]))
    return out

RX_ADD = re.compile(r'class_add\s*<\s*([\w:]+)\s*>\s*\(\s*\w+\s*,\s*"([^"]+)"')
RX_DEF = re.compile(r'class_(?:def|mem)\s*<\s*([\w:]+)\s*>\s*\(\s*\w+\s*,\s*"([^"]+)"')
RX_FPTR = re.compile(r'class_def\s*<\s*([\w:]+)\s*>\s*\(\s*\w+\s*,\s*"([^"]+)"\s*,\s*&\s*([\w:]+)')
RX_INH = re.compile(r'class_inh\s*<\s*([\w:]+)\s*,\s*([\w:]+)\s*>')
RX_GDEF = re.compile(r'lua_tinker\s*::\s*def\s*\(\s*\w+\s*,\s*"([^"]+)"')

def march(ifiles):
    cname = {}                         # C++ type -> Lua class name
    meth = collections.defaultdict(set)  # C++ type -> names
    inh = {}                           # C++ type -> C++ base
    gdef = set()
    fptr = {}
    texts = []
    for f, i in ifiles.items():
        t = open(i, 'rb').read().decode('latin1')
        texts.append(t)
        for a, b, c in RX_FPTR.findall(t): fptr[(a, b)] = c
        for a, b in RX_ADD.findall(t): cname[a] = b
        for a, b in RX_DEF.findall(t): meth[a].add(b)
        for a, b in RX_INH.findall(t): inh[a] = b
        gdef |= set(RX_GDEF.findall(t))
    # Lua class -> its own names + ancestors'
    def closure(ty):
        s, seen = set(), set()
        while ty and ty not in seen:
            seen.add(ty); s |= meth.get(ty, set()); ty = inh.get(ty)
        return s
    lua = {}
    for ty, nm in cname.items():
        lua[nm] = closure(ty)
    parent = {cname.get(a, a): cname.get(b, b) for a, b in inh.items()}
    # getter return type: the pointer type a bound member function is declared to return, when it maps to one Lua class
    # Read inside the declaring class's own body (the &Class::Func qualifier), so two classes' GetCamera do not mix.
    big = max(texts, key=len)   # X2Main.i carries every X2Lib and KTDXLIB header
    bodies = {}
    def body_of(cls):
        short = cls.split('::')[-1]
        if short in bodies: return bodies[short]
        b = ''
        for m in re.finditer(r'\b(?:class|struct)\s+%s\b[^;{()]*\{' % re.escape(short), big):
            i, depth = m.end(), 1
            while i < len(big) and depth:
                ch = big[i]
                if ch == '{': depth += 1
                elif ch == '}': depth -= 1
                i += 1
            if i - m.end() > len(b): b = big[m.end():i]
        bodies[short] = b
        return b
    ret = {}
    for (ty, nm), fp in fptr.items():
        luacls = cname.get(ty)
        if not luacls: continue
        owner = '::'.join(fp.split('::')[:-1]) or ty
        short = fp.split('::')[-1]
        cands = set(re.findall(r'([\w:]+)\s*(?:const\s*)?[*&]\s*%s\s*\(' % re.escape(short), body_of(owner)))
        lc = {cname.get(c, cname.get(c.split('::')[-1])) for c in cands} - {None}
        if len(lc) == 1:
            ret[(luacls, nm)] = lc.pop()
    return lua, parent, gdef, ret

def is_class(tok, known, methods):
    # March Lua class names first; otherwise a class-shaped name (underscores allowed: CX2NewBossRaid_Eltrion_MK2_Phase2,
    # CX2SimpleDLG_WithLuaFunc) that March never binds as a method (KTDGUIOKMsgBox / KTDGUIOkAndCancelMsgBox are CX2Main methods)
    if tok in known: return True
    if tok in methods: return False
    return bool(re.match(r'^(CX2|CKTD|KTDG|CX|KTrain|KGC|D3DX)[A-Za-z0-9:_]*$', tok)) and not tok.upper().endswith('_LUA')

def june(known, methods):
    rows = {r[0]: r[2].split('|') for r in csv.reader(open(os.path.join(HERE, 'func_strings.tsv'), encoding='utf-8'), delimiter='\t')}
    cls = collections.OrderedDict()
    for va in ('31be67a0', '31d95080'):
        cur = None
        for tok in rows[va]:
            if is_class(tok, known, methods):
                cur = tok; cls.setdefault(cur, [])
            elif cur:
                cls[cur].append(tok)
    return cls


# Receiver typing. lua_tinker picks a method table from the static C++ type that was pushed, so the gate is per call site:
# the receiver's pushed type, and whether the name is in March's closure of that type.
#   unit scripts    NPC state / event / AI callbacks: (g_pKTDXApp, g_pX2Game, this)  X2GUNPC.cpp:16686, X2GUNPCAI.cpp:81
#   dungeon scripts substage trigger callbacks: (g_pKTDXApp, static_cast<CX2DungeonGame*>(g_pX2Game), this)  X2DungeonSubStage.cpp:431
# A local takes the return type of the getter it was assigned from (March's declaration, or JUNE_RET for June-only getters).
# Control flow is ignored (last assignment wins, in text order); helpers take their argument types from their call sites.
SIG = {'unit': ['CKTDXApp', 'CX2Game', 'CX2GUNPC'], 'dungeon': ['CKTDXApp', 'CX2DungeonGame', 'CX2DungeonSubStage']}
GLOBALS = {'g_pKTDXApp': 'CKTDXApp', 'g_pX2Game': 'CX2Game', 'g_pMain': 'CX2Main', 'g_pData': 'CX2Data', 'g_pWorld': 'CX2World',
           'g_pEffectSet': 'CX2EffectSet', 'g_pDamageManager': 'CX2DamageManager', 'g_pCamera': 'CX2Camera',
           'g_pDungeonManager': 'CX2DungeonManager', 'g_pLineMap': 'CKTDGLineMap', 'g_pMajorParticle': 'CKTDGParticleSystem',
           'g_pMinorParticle': 'CKTDGParticleSystem', 'g_pFontManager': 'CKTDGFontManager', 'g_pDGManager': 'CKTDGManager'}
# June-only getters: the type June's binder pushes (filled as C3z-4 triages them; c3z4_bind_plan.tsv cites the address)
JUNE_RET = {('CX2Game', 'GetHostGameUnit'): 'CX2GUUser', ('CX2GUNPC', 'GetStoredGUUser'): 'CX2GUUser',
            ('CX2Game', 'GetTutorialGuideManager'): 'CX2TutorialGuideManager',
            ('CX2Game', 'GetSimpleDLG'): 'CX2SimpleDLG_WithLuaFunc', ('CX2Game', 'CreateSimpleDLG'): 'CX2SimpleDLG_WithLuaFunc',
            ('CX2Game', 'GetLiveUserUnitRandom'): 'CX2GUUser'}


def reach_map():
    spec = open(os.path.join(HERE, 'c3z1_unbound_reach.py'), encoding='utf-8').read()
    head = spec.split('miss=collections')[0]       # its reach construction, not its name-only census
    g = {'__name__': 'c3z4'}
    cwd = os.getcwd(); os.chdir(TRUNK)
    try: exec(compile(head, 'c3z1_unbound_reach.py', 'exec'), g)
    finally: os.chdir(cwd)
    kind = {}
    for n in g['units']: kind[n] = 'unit'
    for d in g['covered']: kind[d.lower()] = 'dungeon'
    for n in g['worlds']: kind.setdefault(n, 'world')
    return g['reach'], g['src'], kind

RX_FUNC = re.compile(r'^function\s+([\w.:]+)\s*\(([^)]*)\)', re.M)
RX_CALL = re.compile(r'([:.])\s*([A-Za-z_]\w*)\s*\(')
IDCH = set('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_')

def back_expr(t, i, env, rtype):
    """type of the receiver expression that ends just before t[i] (the ':' of a method call), or None."""
    j = i - 1
    while j >= 0 and t[j] in ' \t': j -= 1
    if j < 0: return None
    if t[j] == ')':
        depth = 0
        while j >= 0:
            if t[j] == ')': depth += 1
            elif t[j] == '(':
                depth -= 1
                if depth == 0: break
            j -= 1
        k = j - 1
        while k >= 0 and t[k] in ' \t': k -= 1
        e = k
        while k >= 0 and t[k] in IDCH: k -= 1
        name = t[k + 1:e + 1]
        while k >= 0 and t[k] in ' \t': k -= 1
        if k >= 0 and t[k] == ':' and name:
            rt = back_expr(t, k, env, rtype)
            return rtype(rt, name) if rt else None
        return None
    e = j
    while j >= 0 and t[j] in IDCH: j -= 1
    var = t[j + 1:e + 1]
    if j >= 0 and t[j] == '.': return None
    return env.get(var) or GLOBALS.get(var)

def type_calls(text, kind, ret):
    rtype = lambda r, n: ret.get((r, n)) or JUNE_RET.get((r, n))
    sites = []
    funcs = list(RX_FUNC.finditer(text))
    helper_args = collections.defaultdict(list)
    bodies = []
    for k, m in enumerate(funcs):
        end = funcs[k + 1].start() if k + 1 < len(funcs) else len(text)
        params = [x.strip() for x in m.group(2).split(',') if x.strip()]
        bodies.append((m.group(1), params, m.end(), end))

    def run(a, b, env, fname):
        pos = a
        for ln in text[a:b].split('\n'):
            off = pos; pos += len(ln) + 1
            for c in RX_CALL.finditer(ln):
                if c.group(1) == ':':
                    sites.append((c.group(2), back_expr(text, off + c.start(1), env, rtype), fname))
            for c in re.finditer(r'(?<![:.\w])([A-Za-z_]\w*)\s*\(([^()]*)\)', ln):
                args = [x.strip() for x in c.group(2).split(',')]
                helper_args[c.group(1)].append([env.get(x) or GLOBALS.get(x) for x in args])
            am = re.match(r'\s*(slot\d+)\s*=\s*(.+?)\s*$', ln)
            if not am: continue
            var, rhs = am.group(1), am.group(2)
            if re.fullmatch(r'slot\d+|g_p\w+', rhs):
                env[var] = env.get(rhs) or GLOBALS.get(rhs); continue
            env[var] = None
            if not rhs.endswith(')') or ':' not in rhs: continue
            q = off + ln.rindex(')')        # the outermost last call of the RHS
            depth = 0
            while q > off:
                if text[q] == ')': depth += 1
                elif text[q] == '(':
                    depth -= 1
                    if depth == 0: break
                q -= 1
            k2 = q - 1
            while k2 > off and text[k2] in IDCH: k2 -= 1
            name = text[k2 + 1:q].strip()
            if text[k2] == ':':
                rt = back_expr(text, k2, env, rtype)
                env[var] = rtype(rt, name) if rt else None

    sig = SIG.get(kind)
    # an engine callback is a function the script names in a string (LUA_STATE_START_FUNC = "X", COMMON_FRAME_FUNC = "X",
    # trigger conditions); it gets the script kind's signature. Anything else is a helper (GetUser(slot1, slot2),
    # ALTERA_SECRET_TYPE2_BOSS_DOWN_SMOKE(game, npc, t)): its parameters take their types from its call sites, or stay untyped.
    named = set(re.findall(r'"([A-Za-z_]\w*)"', text))
    called = set(re.findall(r'(?<![:.\w])([A-Za-z_]\w*)\s*\(', re.sub(r'^function\s+[^\n]*', '', text, flags=re.M)))
    called |= {n for n, *_ in bodies} - named
    pending = []
    for name, params, a, b in bodies:
        if sig and len(params) == 3 and '.' not in name and ':' not in name and name not in called:
            run(a, b, dict(zip(params, sig)), name)
        else:
            pending.append((name, params, a, b))
    for rnd in range(2):     # helpers called from helpers: a second round sees the first round's call sites
        out = []
        for name, params, a, b in pending:
            env = {}
            calls = helper_args.get(re.split(r'[.:]', name)[-1], [])
            for i, p in enumerate(params):
                ts = {c[i] if i < len(c) else None for c in calls} - {None}   # typed call sites must agree
                if len(ts) == 1: env[p] = ts.pop()
            if rnd == 0:
                saved = len(sites); run(a, b, env, name); out.append((name, params, a, b, saved, len(sites)))
            else:
                run(a, b, env, name)
        if rnd == 0:
            for name, params, a, b, s0, s1 in reversed(out): del sites[s0:s1]   # drop round-1 sites, round 2 redoes them
    return sites

def main():
    reuse = '--reuse-pp' in sys.argv
    lua, parent, gdef, ret = march(preprocess(reuse))
    allmarch = set().union(*lua.values()) | gdef
    jcls = june(set(lua), allmarch)
    jby = collections.defaultdict(set)
    for c, ms in jcls.items():
        for m in ms: jby[m].add(c)
    gaps = []
    for c, ms in jcls.items():
        have = lua.get(c)
        for m in ms:
            if have is None or m not in have:
                gaps.append((c, m, 'class-missing' if have is None else ('elsewhere' if m in allmarch else 'absent')))
    reach, src, kind = reach_map()
    site = collections.defaultdict(lambda: collections.defaultdict(lambda: [set(), set()]))   # method -> recv type -> scripts, dungeons
    ntyped = nall = 0
    for n, ds in reach.items():
        if n not in src: continue
        t = open(os.path.join(TRUNK, src[n]), encoding='utf-8', errors='replace').read()
        own = {re.split(r'[.:]', m.group(1))[-1] for m in RX_FUNC.finditer(t)}   # functions the script defines itself
        for meth, rt, fn in type_calls(t, kind.get(n), ret):
            if meth in own: continue
            nall += 1; ntyped += rt is not None
            site[meth][rt or '?'][0].add(n); site[meth][rt or '?'][1].update(ds)
    gapnames = {m for c, m, st in gaps}
    rows = []
    for meth, by in site.items():
        for rt, (s, d) in by.items():
            if rt == '?':
                # untyped, but March binds the name somewhere: only a name June binds on a class March lacks it on is
                # still a candidate (the receiver could be that class); listed as untyped-gap so the gate covers it
                if meth in allmarch:
                    if meth not in gapnames: continue
                    st = 'untyped-gap'
                else:
                    st = 'unbound-untyped'
            else:
                have = lua.get(rt)
                if have is not None and meth in have: continue
                st = 'class-missing' if have is None else ('bound-elsewhere' if meth in allmarch else 'unbound')
            jc = sorted(jby.get(meth, ()))
            if not jc: st += ',not-in-june-binder'
            rows.append((meth, rt, st, jc, s, d))
    rows.sort(key=lambda r: (-len(r[5]), r[0]))
    with open(os.path.join(HERE, 'c3z4_class_gap.tsv'), 'w', encoding='utf-8', newline='\n') as fh:
        fh.write('june_class\tmethod\tmarch\n')
        for c, m, st in gaps: fh.write('%s\t%s\t%s\n' % (c, m, st))
    with open(os.path.join(HERE, 'c3z4_unbound.tsv'), 'w', encoding='utf-8', newline='\n') as fh:
        fh.write('method\treceiver\tstate\tjune_classes\tn_scripts\tn_dungeons\tscripts\tdungeons\n')
        for m, rt, st, jc, s, d in rows:
            fh.write('%s\t%s\t%s\t%s\t%d\t%d\t%s\t%s\n' % (m, rt, st, ';'.join(jc), len(s), len(d), ';'.join(sorted(s)), ';'.join(sorted(d))))
    per = collections.Counter(c for c, m, st in gaps)
    print('June classes', len(jcls), ' March Lua classes', len(lua), ' class gaps', len(gaps), ' getter types', len(ret))
    for c, n in per.most_common(): print('  %-34s gap %3d  (June %d, March %s)' % (c, n, len(jcls[c]), len(lua[c]) if c in lua else '-'))
    print('reached call sites %d, receiver typed %d' % (nall, ntyped))
    print('reached call-site gaps', len(rows), ' (methods %d)' % len({r[0] for r in rows}))
    for m, rt, st, jc, s, d in rows:
        print('  %-36s on %-24s %-30s june=%s scripts %d dungeons %d' % (m, rt, st, ','.join(jc), len(s), len(d)))

if __name__ == '__main__':
    main()
