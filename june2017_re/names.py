"""Phase 10b audit 2: the name contract between March's C++ and June's effect data.

    python names.py             # ~40 min: cl /P every X2Lib + KTDXLIB TU (16-way), then compare
    python names.py --resume    # reuse per-TU results a previous (interrupted) run saved
    python names.py --reuse     # development: reuse assets_cache/names_literals.json

Writes ../JUNE2017_NAME_CONTRACT.md and name_contract.tsv.

March side. Every client TU that US_SERVICE builds (X2Lib_2010.vcxproj and
KTDXLIB_2010.vcxproj ClCompile items, minus ExcludedFromBuild for
US_SERVICE|Win32) is preprocessed with cl /P under that project's own
US_SERVICE include path and defines, plus UNICODE/_UNICODE, as census.py does
for its parser TUs. So an #ifdef'd-out call does not count and a stringized
name does. From each .i, every string literal (wide and narrow) whose #line origin is a file
under X2Lib/ or KTDXLIB/ is kept (adjacent literals joined), with its origin
file:line and the text before it on the statement. Each .i is deleted as soon
as it is read.

Rather than enumerate APIs (PlayEffectSet, CreateInstance, CreateSequence, ...),
a literal counts as an effect name when March's own data defines it in some
namespace: effect set (game/UI), damage effect, or one of the eight particle /
mesh-player systems. Names are compared case-sensitively (see assets.py).

For each such name: does June define it in the same namespace?
  kept              yes, in a file March's loader already opens
  kept-new-file     yes, but only in a file March does not load yet
                    (GameMajorParticle24-67, DamageEffect_Character/_Dungeon/_ETC)
  moved             no; June defines the exact name in another namespace
  case-changed      no; June defines it with different letter case
  renamed           no; a June templet in the same namespace has identical content
                    (normalised: the templet's own name removed)
  renamed-candidate no; a June templet in the same namespace references exactly the
                    same assets (evidence given; not proof)
  dropped           none of the above: silently disappears on adoption
Out of scope by construction, and counted so the list says so: names built at
run time (format strings such as L"EffectSet_%s" at effect call sites).
"""
import collections
import json
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import census as C    # noqa: E402  (cl.exe location, env, X2Lib flags)
import fxdata as F    # noqa: E402

OUT_MD = os.path.join(F.TRUNK, 'JUNE2017_NAME_CONTRACT.md')
OUT_TSV = os.path.join(HERE, 'name_contract.tsv')
LIT_CACHE = os.path.join(F.CACHE, 'names_literals.json')
PP_TMP = os.path.join(F.CACHE, 'pp_names')

# KTDXLIB_2010.vcxproj, US_SERVICE|Win32 (lines 1622-1630 and IncludePath 768).
KTDX_INC = ['./', './DXUT', '../X2ServerProtocol', '../X2ServerProtocol/FSM', '../X2ServerProtocol/Socket',
            './CxImage', '../libxml']
KTDX_INC_SOLN = [r'Libs\ExternalLib\DXSDK\Include', r'Libs\ExternalLib\Boost\boost_1_44_0', r'KNCSDK\Include_2010',
                 r'KOGGamePerformanceCheck\include', r'Libs\ExternalLib\freetype-2.4.11\include', r'XTRAP\Client',
                 'Steamworks']
KTDX_DEFS = ['WIN32', 'NDEBUG', '_LIB', 'SERV_COUNTRY_US', 'CLIENT_COUNTRY_US', '_IN_HOUSE_',
             '_HAS_ITERATOR_DEBUGGING=0', '_USE_32BIT_TIME_T', '_CONVERT_VS_2010', 'UNICODE', '_UNICODE']

BONE_CTX = re.compile(r'\w*Bone\w*\s*\(\s*$', re.I)
EFFECT_API = re.compile(r'(PlayEffectSet|CreateInstance\w*|CreateSequence\w*|GetTemplet\w*|EffectSet\w*|'
                        r'DamageEffect\w*|XMeshPlayer\w*|Particle\w*)\s*\(', re.I)


# ------------------------------------------------------------------ March literals
def tus(project):
    path = os.path.join(F.TRUNK, project)
    t = open(path, encoding='utf-8-sig', errors='replace').read()
    out = []
    for inc, _, body in re.findall(r'<ClCompile Include="([^"]+)"\s*(/>|>(.*?)</ClCompile>)', t, re.S):
        if not inc.lower().endswith(('.cpp', '.c')):
            continue
        if re.search(r"ExcludedFromBuild Condition=\"'\$\(Configuration\)\|\$\(Platform\)'=="
                     r"'US_SERVICE\|Win32'\">true", body or ''):
            continue
        out.append(inc)
    return out


def cmd_for(project, src, out):
    if project.startswith('X2Lib'):
        return C.cl_cmd(src, out)
    inc = KTDX_INC + [os.path.join(F.TRUNK, s) for s in KTDX_INC_SOLN]
    return ([C.VS + r'\VC\bin\cl.exe', '/nologo', '/P', '/Zm600', '/Zc:wchar_t', '/Fi' + out] +
            ['/I' + i for i in inc] + ['/D' + d for d in KTDX_DEFS] + [src])


LINE = re.compile(r'^\s*#line\s+(\d+)\s+"([^"]+)"|^\s*#\s*(\d+)\s+"([^"]+)"')
WSTR = re.compile(r'L"((?:[^"\\\n]|\\.)*)"')


def literals_in(ipath, keep_roots, base_dir=None):
    """String literals, wide AND narrow, whose origin file is under keep_roots:
    [(value, file, line, context)]. Narrow matters: CreateInstance_LUA and friends take a
    char* name (CreateInstance_LUA( this, "MACHINE_GUN_STRAIGHT", ... ), X2GUUser.cpp).
    cl /P names the TU itself by its bare relative path (#line 1 "X2DamageEffect.cpp")
    while headers get absolute paths, so a relative #line path is resolved against
    base_dir (the project directory cl ran in) before the keep_roots test."""
    text = open(ipath, 'rb').read().decode('latin1')
    out = []
    cur_file, cur_line = None, 0
    keep = False
    pending = None   # a literal run that reached the end of its line: may continue on the next
    # (?<![\w']) keeps u8"/R"/identifier-suffixed quotes and '"' char literals out.
    run = re.compile(r'(?<![\w\'])L?"(?:[^"\\\n]|\\.)*"(?:\s*L?"(?:[^"\\\n]|\\.)*")*')
    cont = re.compile(r'^\s*(L?"(?:[^"\\\n]|\\.)*"(?:\s*L?"(?:[^"\\\n]|\\.)*")*)')
    parts = lambda s: ''.join(re.findall(r'"((?:[^"\\\n]|\\.)*)"', s))
    for raw in text.split('\n'):
        m = LINE.match(raw)
        if m:
            cur_line = int(m.group(1) or m.group(3)) - 1
            cur_file = (m.group(2) or m.group(4)).replace('\\\\', '\\')
            if base_dir and not os.path.isabs(cur_file):
                cur_file = os.path.normpath(os.path.join(base_dir, cur_file))
            low = cur_file.lower().replace('/', '\\')
            keep = any(r in low for r in keep_roots)
            continue
        cur_line += 1
        start = 0
        if pending is not None:
            if not raw.strip():
                continue
            mc = cont.match(raw)
            if mc:   # L"a"  <newline>  L"b": one literal
                pending[0] += parts(mc.group(1))
                start = mc.end()
                if not raw[start:].strip():
                    continue   # the run may go on past this line too
            out.append(tuple(pending))
            pending = None
        if not keep or '"' not in raw or raw.lstrip().startswith('#'):
            continue
        # Join adjacent literals on the line: L"a" L"b" (and L"a" "b").
        for m2 in run.finditer(raw, start):
            item = [parts(m2.group(0)), cur_file, cur_line, raw[max(0, m2.start() - 120):m2.start()].strip()]
            if not raw[m2.end():].strip():
                pending = item
            else:
                out.append(tuple(item))
    if pending is not None:
        out.append(tuple(pending))
    return out


def remove_retrying(path):
    """Delete a fresh .i. Windows Defender scans a newly written 20 MB file and holds it
    open for a moment (WinError 32), so retry; a file that stays locked is left for the
    next run rather than aborting this one. Returns True if it is gone."""
    import time
    for i in range(20):
        try:
            os.remove(path)
            return True
        except FileNotFoundError:
            return True
        except PermissionError:
            time.sleep(0.25 * (i + 1))
    return False


def collect_literals(resume=False):
    """Per-TU results are saved to assets_cache/names_tu/ as each TU finishes, so a run
    that dies part-way loses nothing; --resume reuses them (use only if no source or
    header changed since)."""
    os.makedirs(PP_TMP, exist_ok=True)
    tu_dir = os.path.join(F.CACHE, 'names_tu')
    os.makedirs(tu_dir, exist_ok=True)
    for f in os.listdir(PP_TMP):   # leftovers a previous run could not delete
        remove_retrying(os.path.join(PP_TMP, f))
    jobs = []
    for project in ('X2Lib/X2Lib_2010.vcxproj', 'KTDXLIB/KTDXLIB_2010.vcxproj'):
        pdir = os.path.join(F.TRUNK, os.path.dirname(project))
        for src in tus(project):
            jobs.append((project, pdir, src))
    env = C.cl_env()
    keep_roots = ['\\x2lib\\', '\\ktdxlib\\']
    results = {}
    failures = []
    locked = []

    def one(job):
        project, pdir, src = job
        stem = re.sub(r'[\\/:]', '_', os.path.dirname(project) + '_' + src)
        done = os.path.join(tu_dir, stem + '.json')
        if resume and os.path.exists(done):
            return job, json.load(open(done, encoding='utf-8')), None
        out = os.path.join(PP_TMP, stem + '.i')
        p = subprocess.run(cmd_for(project, src, out), cwd=pdir, env=env, capture_output=True)
        if p.returncode != 0 or not os.path.exists(out):
            remove_retrying(out)
            return job, None, p.stdout.decode('latin1', 'replace')[-600:]
        lits = literals_in(out, keep_roots, pdir)
        if not remove_retrying(out):
            locked.append(out)
        json.dump(lits, open(done, 'w', encoding='utf-8'))
        return job, lits, None

    with ThreadPoolExecutor(max_workers=16) as ex:
        for job, lits, err in ex.map(one, jobs):
            if lits is None:
                failures.append((job[0], job[2], err))
                continue
            for v, f, l, ctx in lits:
                rel = f.replace('/', '\\')
                i = rel.lower().find('\\trunk\\')
                rel = rel[i + 7:] if i >= 0 else rel
                results[(v, rel, l)] = ctx
    if locked:
        print('%d .i file(s) still locked, left in %s for the next run to delete' % (len(locked), PP_TMP))
    lit = [{'value': v, 'file': f, 'line': l, 'ctx': c} for (v, f, l), c in sorted(results.items())]
    data = {'tus': len(jobs), 'failures': failures, 'literals': lit}
    json.dump(data, open(LIT_CACHE, 'w', encoding='utf-8'))
    return data


# ------------------------------------------------------------------ name universes
def universes(build_label, build, fx):
    es, de = F.fx_templets(fx)
    u = {'es': {}, 'de': {}}
    for name, defs in es.items():
        u['es'][name] = defs[-1]
    for name, d in de.items():
        u['de'][name] = d
    tt = F.templets(build_label, build)
    for ns, d in tt.items():
        u[ns] = dict((name, ts[-1]) for name, ts in d.items())
    return u


def lua_norm(tab):
    t = dict(tab.get('$t', tab))
    t.pop('NAME', None)
    return json.dumps(t, sort_keys=True)


def lua_assets(tab, acc=None):
    """The set of string values a Lua templet references (its asset/name signature)."""
    acc = set() if acc is None else acc
    for k, v in tab.get('$t', tab).items():
        if isinstance(v, dict) and '$t' in v:
            lua_assets(v, acc)
        elif isinstance(v, str) and k != 'NAME' and not k.endswith('BONE_NAME'):
            acc.add(v)
    return acc


def content_key(ns, obj):
    if ns == 'es':
        return lua_norm(obj[2])
    if ns == 'de':
        return lua_norm(obj[1])
    return ' '.join(obj.body)


def ref_key(ns, obj):
    if ns == 'es':
        return frozenset(lua_assets(obj[2]))
    if ns == 'de':
        return frozenset(lua_assets(obj[1]))
    return frozenset(v.lower() for _, _, v in obj.refs)


def defining_file(ns, obj):
    if ns == 'es':
        return obj[1]
    if ns == 'de':
        return obj[0]
    return obj.file


class JuneBinary(object):
    """Does June's own C++ still ask for a name? Looks in unpacked_x2.exe as plain UTF-16
    and NUL-terminated ASCII, and in xordecrypt's `ok` rows (xorstrings.tsv). Absent means
    June's call site changed; present means June still asks for it."""
    EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'

    def __init__(self):
        self.d = open(self.EXE, 'rb').read()
        self.xor = set()
        for line in open(os.path.join(HERE, 'xorstrings.tsv'), encoding='latin1'):
            c = line.rstrip('\n').split('\t')
            if len(c) > 6 and c[6] == 'ok':
                self.xor.add(c[5])

    def has(self, s):
        return (self.d.find(s.encode('utf-16le')) >= 0 or self.d.find(s.encode('latin1') + b'\x00') >= 0
                or s in self.xor)


def bone_names(fx):
    """Every value a captured effect table stores under a bone key, in either build."""
    out = set()

    def walk(t):
        for k, v in t.get('$t', t).items():
            if isinstance(v, dict) and '$t' in v:
                walk(v)
            elif isinstance(v, str) and ('BONE' in k.upper()):
                out.add(v)
    for c in fx['calls']:
        for a in c['args']:
            if isinstance(a, dict) and '$t' in a:
                walk(a)
    for g in fx['globals'].values():
        if isinstance(g['value'], dict) and '$t' in g['value']:
            walk(g['value'])
    return out


# ------------------------------------------------------------------ main
def main():
    reuse = '--reuse' in sys.argv
    data = (json.load(open(LIT_CACHE, encoding='utf-8')) if reuse and os.path.exists(LIT_CACHE)
            else collect_literals(resume='--resume' in sys.argv))
    if data['failures']:
        print('cl /P failed for %d TU(s):' % len(data['failures']))
        for proj, src, err in data['failures'][:10]:
            print('  %s %s: %s' % (proj, src, err.strip().splitlines()[-1] if err.strip() else '?'))

    jun = F.Build('june', F.JUNE_DATA, F.JUNE_DATA)
    mar = F.Build('march', F.MAR_DATA, F.MAR_STAGE)
    jfx = F.june_fx()
    mfx = F.march_fx(mar)
    ju = universes('june', jun, jfx)
    mu = universes('march', mar, mfx)
    bones = bone_names(jfx) | bone_names(mfx)
    jbin = JuneBinary()

    march_files = set(n.lower() + '.lua' for n in F.MAR_LUA_ORDER)
    for ns in F.NS_FILES:
        march_files.update(f.lower() for f in F.ns_files('march', mar, ns))
        march_files.update(f.lower() for f in F.NS_FILES[ns] if not f.startswith('<'))

    # Literal occurrences per value.
    occ = collections.defaultdict(list)
    for L in data['literals']:
        occ[L['value']].append(L)

    # Run-time names. The studio builds some names with StringCchPrintf into a buffer
    # (L"Effectset_Swallow_Assault_Shadow0%d", X2GUAra.cpp), so no literal names them.
    # Turn every format literal into a pattern and match it against every name either
    # build defines; each March name it can produce is judged below like a literal,
    # with the format string's site. A format that matches no March name is listed
    # with what June would give it.
    all_march = set(n for ns in mu for n in mu[ns])
    all_june = set(n for ns in ju for n in ju[ns])
    fmt_sites = []
    spec = re.compile(r'%[-+ #0]*\d*(?:\.\d+)?(?:l|h|I64)?([dius])')
    for L in data['literals']:
        v = L['value']
        if '%' not in v or not spec.search(v):
            continue
        pat = ''
        pos = 0
        for m in spec.finditer(v):
            pat += re.escape(v[pos:m.start()]) + (r'\d+' if m.group(1) in 'diu' else r'.+?')
            pos = m.end()
        pat += re.escape(v[pos:])
        if not re.search(r'[A-Za-z]{3}', re.sub(r'\\.|\.\+\?|\\d\+', '', pat)):
            continue   # "%d", "%s_%s": matches anything, names nothing
        rx = re.compile('^' + pat + '$')
        mhits = sorted(n for n in all_march if rx.match(n))
        jhits = sorted(n for n in all_june if rx.match(n))
        if not mhits and not jhits:
            continue
        fmt_sites.append(dict(L, march=mhits, june=jhits))
        for n in mhits:
            occ[n].append(dict(L, value=n, ctx='(built from L"%s") %s' % (v, L['ctx'])))

    # Content indexes of June, per namespace.
    jcontent = {}
    jrefs = {}
    jlower = {}
    for ns, d in ju.items():
        ci = collections.defaultdict(list)
        ri = collections.defaultdict(list)
        li = collections.defaultdict(list)
        for name, obj in d.items():
            ci[content_key(ns, obj)].append(name)
            rk = ref_key(ns, obj)
            if rk:
                ri[rk].append(name)
            li[name.lower()].append(name)
        jcontent[ns], jrefs[ns], jlower[ns] = ci, ri, li

    rows = []
    for value, sites in sorted(occ.items()):
        nss = [ns for ns in mu if value in mu[ns]]
        for ns in nss:
            mobj = mu[ns][value]
            r = {'name': value, 'ns': ns, 'march_file': defining_file(ns, mobj),
                 'sites': sorted(set('%s:%d' % (s['file'], s['line']) for s in sites)),
                 'ctx': sites[0]['ctx'][-90:]}
            if value in ju[ns]:
                jf = defining_file(ns, ju[ns][value])
                r['verdict'] = 'kept' if jf.lower() in march_files else 'kept-new-file'
                r['evidence'] = 'June: %s' % jf
            else:
                other = [n for n in ju if n != ns and value in ju[n]]
                case = jlower[ns].get(value.lower(), [])
                ck = content_key(ns, mobj)
                same = [n for n in jcontent[ns].get(ck, []) if n != value]
                rk = ref_key(ns, mobj)
                cand = [n for n in jrefs[ns].get(rk, []) if n != value] if rk else []
                if other:
                    r['verdict'], r['evidence'] = 'moved', 'June defines it as ' + ', '.join(other)
                elif case:
                    r['verdict'], r['evidence'] = 'case-changed', 'June: ' + ', '.join(case)
                elif same:
                    r['verdict'], r['evidence'] = 'renamed', 'identical content: ' + ', '.join(sorted(same)[:5])
                elif cand:
                    r['verdict'] = 'renamed-candidate'
                    r['evidence'] = 'same %d reference(s) %s: %s' % (
                        len(rk), sorted(rk)[:3], ', '.join(sorted(cand)[:5]))
                else:
                    r['verdict'], r['evidence'] = 'dropped', 'no June templet with this name, content or references'
                # Second, independent source: June's binary. Gone there = June changed
                # its call site (so March's call site must change on adoption); still
                # there = June asks for a name its own data lacks (dead in June too).
                jb = jbin.has(value)
                r['june_binary'] = 'still asked for (dead in June too)' if jb else 'gone (June changed the call)'
                if r['verdict'] in ('case-changed', 'renamed-candidate', 'renamed'):
                    names_ = [x.strip() for x in r['evidence'].split(':', 1)[1].split(',')]
                    asked = [x for x in names_ if jbin.has(x)]
                    if asked:
                        r['june_binary'] += '; June asks for ' + ', '.join(asked[:3])
            rows.append(r)

    # Literals at effect call sites that no build defines: already dead in March.
    # Bone names (Bip01, Dummy01, TRACE_START0, ...) are arguments of the same calls; any
    # value the effect data itself uses as a *BONE_NAME / TRACE_OWNER_BONE is skipped.
    dead = []
    for value, sites in sorted(occ.items()):
        if any(value in mu[ns] for ns in mu) or '%' in value or not value or value in bones:
            continue
        # A literal handed straight to GetBonePos( / *Bone*( is a bone even when it sits
        # inside a CreateInstance call (L"Cannon_Head", X2GUUser.cpp:24853).
        # Parser keywords (token.m_strValue.compare( "NUMPARTICLES" ), KTDGParticleSystem)
        # match EFFECT_API through the word "Particle" but are not names.
        api = [s for s in sites if EFFECT_API.search(s['ctx']) and not BONE_CTX.search(s['ctx'])
               and not re.search(r'compare\s*\(\s*$', s['ctx'])]
        if api and not re.search(r'\.(lua|txt|dds|tga|x|y|ogg|wav|xet)$', value, re.I):
            jn = [ns for ns in ju if value in ju[ns]]
            dead.append({'name': value, 'sites': sorted(set('%s:%d' % (s['file'], s['line']) for s in api)),
                         'ctx': api[0]['ctx'][-90:], 'june': ', '.join(jn),
                         'june_binary': 'asked for' if jbin.has(value) else 'gone'})

    write(rows, dead, fmt_sites, data, jun, mar)
    by = collections.Counter(r['verdict'] for r in rows)
    print('TUs %d (failed %d), literals %d, effect names %d: %s; dead-at-call-site %d, format-string sites %d' % (
        data['tus'], len(data['failures']), len(data['literals']), len(rows), dict(by), len(dead), len(fmt_sites)))


def write(rows, dead, fmt_sites, data, jun, mar):
    cols = ['verdict', 'ns', 'name', 'evidence', 'june_binary', 'march_file', 'sites']
    with open(OUT_TSV, 'w', encoding='utf-8', newline='\n') as fh:
        fh.write('\t'.join(cols) + '\n')
        for r in sorted(rows, key=lambda r: (r['verdict'], r['ns'], r['name'])):
            fh.write('\t'.join(' '.join(r[c]) if isinstance(r.get(c), list) else str(r.get(c, '')) for c in cols) + '\n')

    L = []
    w = L.append
    by = collections.Counter(r['verdict'] for r in rows)
    w('# June2017 effect data: name contract (Phase 10b, audit 2)')
    w('')
    w('Generated by `june2017_re/names.py`; every row is also in `june2017_re/name_contract.tsv`.')
    w('Plan: `JUNE2017_EFFECT_REBUILD_PLAN.md`, Phase 10b. **Nothing was fixed.** This is the adoption')
    w('punch list: every effect name March\'s C++ asks for by literal, checked against what June\'s files')
    w('define.')
    w('')
    w('## Method')
    w('')
    w('- %d translation units (every `.cpp` X2Lib and KTDXLIB build under `US_SERVICE`) preprocessed with' % data['tus'])
    w('  `cl /P` under each project\'s own `US_SERVICE` flags: `#ifdef`\'d-out calls do not count,')
    w('  stringized names do. %d failed to preprocess%s.' % (
        len(data['failures']), '' if not data['failures'] else ' (listed at the end)'))
    w('- %d distinct string literals, wide and narrow (value, file, line), from X2Lib/KTDXLIB sources.' % len(data['literals']))
    w('  Narrow counts: `CreateInstance_LUA` takes a `char*` name.')
    w('- Every row that is not *kept* is also checked against June\'s own binary (`unpacked_x2.exe` plain')
    w('  strings plus `xorstrings.tsv`): *gone* means June changed that call site, so March\'s must change')
    w('  on adoption; *still asked for* means June\'s code asks for a name June\'s data lacks: dead in June too.')
    w('- The loaded files: every `OpenScriptFile` / `Reset*Particle` / `Reset*XMeshPlayer` call in X2Lib')
    w('  and KTDXLIB opens only the standard `*Particle*.txt` / `*XMeshPlayer*.txt` set. The')
    w('  `data121` `rena_A_trapingArrow_Particle.txt` / `_Xmesh.txt` pair ships in both builds but is')
    w('  opened by neither: no loader names it in March\'s source or in June\'s binary.')
    w('- A literal is an **effect name** when March\'s own loaded data defines it: an effect set,')
    w('  a damage effect, or a templet in one of the eight particle / mesh-player systems. Case-sensitive,')
    w('  as the engine\'s `std::map<std::wstring>` lookups are. No API list is needed, so no call shape')
    w('  is missed.')
    w('- June side: the same namespaces, from June\'s 14 effect lua files (executed, route (b)) and every')
    w('  particle / mesh-player file June\'s loader opens.')
    w('')
    w('## Result')
    w('')
    w('| verdict | names | meaning |')
    w('|---|---|---|')
    meaning = {
        'kept': 'June defines it, in a file March already loads',
        'kept-new-file': 'June defines it, but only in a file March\'s loader does not open yet',
        'moved': 'June defines the exact name in a different namespace',
        'case-changed': 'June defines it with different letter case (the lookup is case-sensitive)',
        'renamed': 'a June templet with identical content exists under another name',
        'renamed-candidate': 'a June templet with the same references exists (not proof)',
        'dropped': 'nothing in June: **disappears silently on adoption**',
    }
    for v in ['kept', 'kept-new-file', 'moved', 'case-changed', 'renamed', 'renamed-candidate', 'dropped']:
        w('| %s | %d | %s |' % (v, by.get(v, 0), meaning[v]))
    w('')
    for v in ['dropped', 'renamed-candidate', 'renamed', 'case-changed', 'moved', 'kept-new-file']:
        rs = [r for r in rows if r['verdict'] == v]
        w('## %s (%d)' % (v, len(rs)))
        w('')
        if not rs:
            w('None.')
            w('')
            continue
        w('| namespace | name | evidence | June\'s binary | March defines it in | March call sites |')
        w('|---|---|---|---|---|---|')
        for r in sorted(rs, key=lambda r: (r['ns'], r['name'])):
            sites = ', '.join('`%s`' % s for s in r['sites'][:4]) + (' +%d' % (len(r['sites']) - 4)
                                                                     if len(r['sites']) > 4 else '')
            w('| %s | `%s` | %s | %s | %s | %s |' % (r['ns'], r['name'], r['evidence'].replace('|', '\\|'),
                                                      r.get('june_binary', '-'), r['march_file'], sites))
        w('')
    w('## Already dead in March (%d)' % len(dead))
    w('')
    w('Literals at an effect-looking call site (`PlayEffectSet`, `CreateInstance`, `CreateSequence`, ...)')
    w('that March\'s own data does not define. Not an adoption regression; listed because the `June`')
    w('column shows where adoption would bring one back to life.')
    w('')
    if dead:
        w('| name | June defines it in | June\'s binary | call sites | context |')
        w('|---|---|---|---|---|')
        for d in dead:
            w('| `%s` | %s | %s | %s | `%s` |' % (d['name'], d['june'] or '-', d['june_binary'],
                                                 ', '.join('`%s`' % s for s in d['sites'][:3]),
                                            d['ctx'].replace('|', '\\|').replace('`', "'")))
    w('')
    w('## Run-time names: format strings (%d)' % len(fmt_sites))
    w('')
    w('Some names are built with `StringCchPrintf` into a buffer, so no literal names them. Every format')
    w('literal was turned into a pattern (`%d` -> digits, `%s` -> text) and matched against every name')
    w('either build defines. The March names each one produces are judged in the tables above like')
    w('literals (their call site is the format string). Formats of pure specifiers (`%s_%s`) name')
    w('nothing and are skipped, so a name assembled from two variables is still out of reach.')
    w('')
    if fmt_sites:
        w('| site | format | March names it matches | June names it matches |')
        w('|---|---|---|---|')
        for s in fmt_sites:
            only_j = [n for n in s['june'] if n not in s['march']]
            w('| `%s:%d` | `%s` | %s | %s |' % (
                s['file'], s['line'], s['value'].replace('|', '\\|'),
                ', '.join('`%s`' % n for n in s['march'][:6]) + (' +%d' % (len(s['march']) - 6) if len(s['march']) > 6 else '') or '-',
                ('%d; June-only: ' % len(s['june']) + ', '.join('`%s`' % n for n in only_j[:4])) if only_j else
                ('%d, same set' % len(s['june']) if s['june'] else 'none')))
    w('')
    w('## Load contract: files June loads that March does not')
    w('')
    w('A June name defined only in one of these is invisible to March until its loader opens the file')
    w('(the `kept-new-file` rows above). From June\'s decrypted strings (`xorstrings.tsv`) and plain wide')
    w('strings, against March\'s loaders:')
    w('')
    w('| June file | June loader | March |')
    w('|---|---|---|')
    jmax = max([int(m.group(1)) for n in jun.inv for m in [re.match(r'gamemajorparticle(\d+)\.txt$', n)] if m])
    w('| `GameMajorParticle24.txt` .. `GameMajorParticle%d.txt` | `FUN_32597410` loops from 2 while the file exists (exit `JZ` @`32597d5d`) | hard-codes 2..23 (`X2StateStartUp.cpp:130-150`, `X2Game.cpp:1615-1636`) |' % jmax)
    w('| `DamageEffect_Character.lua`, `DamageEffect_Dungeon.lua`, `DamageEffect_ETC.lua` | `FUN_322e2930`, between `DamageEffect.lua` and `DamageEffect_INT.lua` | opens only `DamageEffect.lua` + `_INT` (`X2Game.cpp:1650-1652`, `:5473-5482`, `:18851`) |')
    w('| `Elsword_DamageData.lua` | plain wide string in the binary | no loader, and no `InsertDamageData_LUA` binding in X2Lib |')
    w('| `UnitViewerEffectSetTable.lua` | plain wide string in the binary | no loader, and no `AddUnitViewerEffectSet` binding in X2Lib |')
    w('')
    if data['failures']:
        w('## TUs that failed to preprocess')
        w('')
        for proj, src, err in data['failures']:
            w('- `%s` `%s`: `%s`' % (proj, src, (err.strip().splitlines() or ['?'])[-1][:150]))
        w('')
    open(OUT_MD, 'w', encoding='utf-8', newline='\n').write('\n'.join(L))


if __name__ == '__main__':
    main()
