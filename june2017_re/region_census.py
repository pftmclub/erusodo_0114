"""Atlas/Elysion region census v1 (ATLAS_PORT_PHASES.md, Phase A0) - the exit metric every Atlas/Elysion phase uses.

    python june2017_re/region_census.py                       # whole report, human readable
    python june2017_re/region_census.py --region atlas        # atlas | elysion | shared (default: all)
    python june2017_re/region_census.py --line AD             # AD AC TW SC AR SL ME REST FLD VIL UI
    python june2017_re/region_census.py --phase A3            # only the rows that phase gates on (v1: a line + check-prefix filter)
    python june2017_re/region_census.py --json out.json       # raw numbers + every open item
    python june2017_re/region_census.py --live                # also compare staged files against the LIVE koms (sizes)
    python june2017_re/region_census.py --tsv                 # write june2017_re/region_census.tsv (one row per open item)

Run from anywhere (paths come from this file).  Needs the KOG luajit (elstools/vendor/luajit) for March registries (BOM source or
0x81 bytecode) and `cl` only through buff_census's cached flag table (no probe is run here).

What the exit metric measures, per region half and per line (the Done gates of every A/E phase read this):

  ids          the REGION SET June defines (derived from June's own files, never typed in), then how much of it March has NOW
               (WORLD_ID / VILLAGE_MAP_ID / LOCAL_MAP_ID / HOUSE_ID / DUNGEON_ID / NPC_UNIT_ID), the collisions of every
               missing value (vs March names at that value, vs the other incoming June IDs, vs shipped relocations, vs the
               C++ end markers), and the ID lists with nil holes.
  npc          readiness per NUI: Enum + NPCTemplet + NPCStat/NewNPCStat (+ its unit script staged).
  registry     June registry key names vs the key literals March's parsers read (OtherPlaceID, NPCMeshName2nd ...).
  bindings     Lua method names the region's June scripts call that no lua_tinker/class_def line in X2Lib/KTDXLIB/X2 spells.
  strings      STR_ID_n names the region files use: defined in StringID_def.lua and with text in general.ess.
  effects      effect-set templets the region references: present in the staged EffectSet.lua or not.
  items        BOSS_DROP_ITEM / field-drop item ids the region uses: present in item.kim or not.
  assets       files the region's Lua names: in March (staged / live) or only in June, with June's size.
  koms         staged files: header (BOM source / 0x81 bytecode), manifest entry, winning mount, live vs staged size.

Two numbers per category (advisor note, A0): the region set (what June defines; reproduces the 2026-10-03 ledger) and
what is missing NOW (the ledger moved after june-effect C3y/C3z/C12 and june-dungeons C3y-1b merged parts of June into March).
June Lua is read from june_src_fixed/ only.  ELYSION_SOLESS.lua is not there: it is scanned as bytes, labelled E0-owned.
"""
import argparse, collections, csv, io, json, os, re, struct, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)
sys.path.insert(0, os.path.join(TRUNK, 'tools'))
sys.path.insert(0, HERE)

import buff_census as BC          # Lua tokenizer / table parser / flag-aware C++ text
import kom                        # staged_names, live_names, inspect
import junekom                    # June kom index + strict decoder

JS = os.path.join(TRUNK, 'june_src_fixed')
KC = kom.KOM_ROOT
GAME = os.path.join(os.path.dirname(KC), 'data')
LJ = os.path.join(TRUNK, 'elstools', 'vendor', 'luajit', 'luajit.exe')
REG = os.path.join(HERE, 'lua_registry_dump.lua')
TSV_OUT = os.path.join(HERE, 'region_census.tsv')

ASCII = lambda s: s.encode('ascii', 'replace').decode('ascii')

# ----------------------------------------------------------------------------------------------------------------------
# Lua helpers
# ----------------------------------------------------------------------------------------------------------------------


def read_text(path):
    return open(path, 'rb').read().decode('utf-8-sig', 'replace')


def run_lj(*args):
    r = subprocess.run([LJ, REG] + list(args), capture_output=True, stdin=subprocess.DEVNULL, timeout=900)
    return r.stdout.decode('utf-8', 'replace').splitlines()


def lj_enum(path, tname):
    out, err = {}, []
    for ln in run_lj('enum', path, tname):
        p = ln.split('\t')
        if p[0] == 'K':
            out[p[1]] = p[2]
        elif p[0] == 'E':
            err.append(p[1] if len(p) > 1 else '')
    return out, err


def lj_calls(path, method):
    rows = []
    for ln in run_lj('calls', path):
        p = ln.split('\t')
        if p[0] != 'C' or p[1] != method:
            continue
        rows.append(dict(kv.partition('=')[::2] for kv in p[2:]))
    return rows


def unq(v):
    """Lua expression text -> bare value: strip quotes."""
    if isinstance(v, BC.Table):
        return v
    v = v.strip()
    if len(v) >= 2 and v[0] in '"\'' and v[-1] == v[0]:
        return v[1:-1]
    return v


def tail(v):
    """`VILLAGE_MAP_ID.VMI_X` -> `VMI_X`; `E:VILLAGE_MAP_ID.VMI_X` -> `VMI_X`; `NUI:NUI_X` -> `NUI_X`."""
    if isinstance(v, BC.Table):
        return v
    v = v.strip()
    if ':' in v and not v.startswith('"'):
        v = v.split(':', 1)[1]
    return v.rsplit('.', 1)[-1]


def method_tables(text, method):
    """Every `X:Method( { ... } )` / `Method({...})` argument table in a Lua source text."""
    kinds, texts = BC.tokenize(text)
    out, i, n = [], 0, len(texts)
    while i < n - 3:
        if kinds[i] == 'i' and texts[i] == method and texts[i + 1] == '(' and texts[i + 2] == '{':
            tab, i = BC._parse_table(kinds, texts, i + 2)
            out.append(tab)
            continue
        i += 1
    return out


def keyed_tables(text, enum):
    """Every `[ENUM.KEY] = { ... }` (nesting skipped) -> OrderedDict KEY -> Table."""
    kinds, texts = BC.tokenize(text)
    out, i, n = collections.OrderedDict(), 0, len(texts)
    while i < n - 6:
        if (texts[i] == '[' and kinds[i] == 'p' and texts[i + 1] == enum and texts[i + 2] == '.' and texts[i + 4] == ']'
                and texts[i + 5] == '=' and texts[i + 6] == '{'):
            key = texts[i + 3]
            tab, i = BC._parse_table(kinds, texts, i + 6)
            out.setdefault(key, tab)
            continue
        i += 1
    return out


def walk(tab):
    """Every (key, value-expression) leaf of a parsed table, depth first."""
    for k, v in tab.items:
        if isinstance(v, BC.Table):
            for x in walk(v):
                yield x
        else:
            yield k, v


def leaves(tab):
    return [v for _, v in walk(tab)]


def positional_refs(t):
    return [tail(v) for v in t.positional()] if isinstance(t, BC.Table) else []


# ----------------------------------------------------------------------------------------------------------------------
# File stores: June source, staged March, live March
# ----------------------------------------------------------------------------------------------------------------------


class Stores(object):
    def __init__(self):
        self.jsrc = {}                         # lower name (.lua) -> path of June .src.lua
        for kd in sorted(os.listdir(JS)):
            d = os.path.join(JS, kd)
            if os.path.isdir(d):
                for f in os.listdir(d):
                    if f.lower().endswith('.src.lua'):
                        self.jsrc.setdefault(f[:-8].lower() + '.lua', os.path.join(d, f))
        self.staged = kom.staged_names()       # lower -> {kom stem: real name}
        self._live = None
        self._june = None

    @property
    def live(self):
        if self._live is None:
            self._live = kom.live_names()
        return self._live

    @property
    def june(self):
        """lower name -> (path, v04, entry): June's own kom index (cheap: headers only)."""
        if self._june is None:
            self._june = {}
            for path in junekom.all_koms()[0]:
                k = junekom.Kom(path)
                for e in k.entries:
                    self._june.setdefault(e['name'].lower(), (path, k.v04, e))
        return self._june

    def staged_path(self, name):
        d = self.staged.get(name.lower())
        if not d:
            return None
        stem, real = sorted(d.items())[0]
        return os.path.join(KC, stem + '_kom', real)

    def jtext(self, name):
        p = self.jsrc.get(name.lower())
        return read_text(p) if p else None


ST = Stores()


def kc(stem, name):
    return os.path.join(KC, stem + '_kom', name)


def js(stem, name):
    base = name[:-8] if name.endswith('.src.lua') else name[:-4] if name.endswith('.lua') else name
    return os.path.join(JS, stem + '_kom', base + '.src.lua')


# ----------------------------------------------------------------------------------------------------------------------
# Region definition (derived from June's own files)
# ----------------------------------------------------------------------------------------------------------------------

LINES = collections.OrderedDict([
    ('AD', ('atlas', 'AtlasDevelopment')), ('AC', ('atlas', 'AtlasCity')), ('TW', ('atlas', 'Tower')),
    ('SC', ('elysion', 'SkyCrossroads')), ('AR', ('elysion', 'Adrian')), ('SL', ('elysion', 'Soless')),
    ('ME', ('elysion', 'Memory'))])
LINE_BY_NAME = {v[1]: k for k, v in LINES.items()}
HALF_OF_LINE = {k: v[0] for k, v in LINES.items()}
HALF_OF_LINE.update({'REST': 'atlas', 'FLD': 'shared', 'VIL': 'elysion', 'UI': 'shared'})


def line_of_name(s):
    """Dungeon / world / unit-script name -> line key or None."""
    u = s.upper()
    for key, (_, nm) in LINES.items():
        if nm.upper() in u:
            return key
    return None


class Region(object):
    """Everything the census derives from June before it looks at March."""
    pass


NUI_RE = re.compile(r'NUI_[A-Z0-9_]+')
WI_RE = re.compile(r'\bWI_[A-Z0-9_]+')
STR_RE = re.compile(r'\bSTR_ID_(\d+)\b')
METHOD_RE = re.compile(r'[A-Za-z_][A-Za-z0-9_]*:([A-Za-z_][A-Za-z0-9_]*)\s*\(')
ASSET_RE = re.compile(r'["\']([A-Za-z0-9_\-\. ]+\.(?:x|y|dds|tga|bmp|png|jpg|xet|tet|ogg|wav|mp3|lua|txt|avi|bik|ksm))["\']', re.I)
EFFECTSET_RE = re.compile(r'["\'](EffectSet_[A-Za-z0-9_]+)["\']')
DAMAGEFX_RE = re.compile(r'["\']((?:DamageEffect|DAMAGE_EFFECT|DAMAGE)_[A-Za-z0-9_]+)["\']')

# Enum tables the region touches: (staged kom stem, file, table); June is june_src_fixed/<stem>_kom/<file>.src.lua
ENUM_TABLES = [('WORLD_ID', 'data036', 'Enum.lua'), ('VILLAGE_MAP_ID', 'data036', 'Enum.lua'),
               ('LOCAL_MAP_ID', 'data036', 'Enum.lua'), ('HOUSE_ID', 'data036', 'Enum.lua'),
               ('NPC_UNIT_ID', 'data036', 'Enum.lua'), ('DUNGEON_ID', 'data036', 'DungeonEnum.lua')]


def load_enums():
    """-> (june, march) : {table: {name: int}}, plus the lua errors met."""
    j, m, errs = {}, {}, []
    for tname, stem, fn in ENUM_TABLES:
        for dst, path in ((j, js(stem, fn)), (m, kc(stem, fn))):
            d, e = lj_enum(path, tname)
            dst[tname] = {k: int(v) for k, v in d.items() if re.match(r'^-?\d+$', v)}
            errs += [(tname, path, x) for x in e]
    return j, m, errs


def derive(J):
    """Region set from June only.  J = June enums.  Returns a Region with ids, per-id line, and the file set."""
    R = Region()
    # --- dungeons: June DungeonData rows whose script is Dungeon_Elysion_*  (Hero rows are out of scope, kept apart)
    dd_text = read_text(js('data035', 'DungeonData'))
    R.dd_rows = {}
    R.di, R.di_hero = collections.OrderedDict(), collections.OrderedDict()
    for t in method_tables(dd_text, 'AddDungeonData'):
        fn = unq(t.get('dataFileName') or '')
        if not fn.lower().startswith('dungeon_elysion_'):
            continue
        did = tail(t.get('dungeonID') or '')
        R.dd_rows[did] = t
        line = line_of_name(fn)
        (R.di_hero if 'HERO' in did else R.di)[did] = dict(id=J['DUNGEON_ID'].get(did), script=fn, line=line)
    # --- villages / rest / lounge: DLG_Map_Village rows named ELYSION
    vil = keyed_tables(read_text(js('data034', 'DLG_Map_Village')), 'VILLAGE_MAP_ID')
    R.vil_rows = collections.OrderedDict((k, t) for k, t in vil.items() if 'ELYSION' in k)
    R.vmi = collections.OrderedDict()
    for k, v in J['VILLAGE_MAP_ID'].items():
        if 'ELYSION' in k:
            line = 'REST' if 'REST' in k else 'VIL' if k == 'VMI_ELYSION' else 'FLD' if 'FIELD' in k else 'VIL'
            R.vmi[k] = dict(id=v, line=line)
    R.lmi = collections.OrderedDict((k, dict(id=v, line='UI')) for k, v in J['LOCAL_MAP_ID'].items() if 'ELYSION' in k)
    # --- battle fields: BattleFieldData rows whose BATTLE_FIELD_ID is an Elysion field, plus the Lanox row with the Atlas portal
    bf_text = read_text(js('data036', 'BattleFieldData'))
    R.bf_rows = collections.OrderedDict()
    R.bf_lanox = None
    for t in method_tables(bf_text, 'AddBattleFieldData'):
        bid = tail(t.get('BATTLE_FIELD_ID') or '')
        if 'ELYSION' in bid:
            R.bf_rows[bid] = t
        if t.get('BATTLE_FIELD_DIRECTION_INFO') is not None and 'ELYSION' in ' '.join(str(x) for x in leaves(t)):
            R.bf_lanox = (bid, t)
    # --- houses: every house in a region village's HouseIDList; npc ids from their rows
    house_rows = keyed_tables(read_text(js('data034', 'DLG_Map_House')), 'HOUSE_ID')
    R.house_rows = house_rows
    R.hi = collections.OrderedDict()
    for vk, t in R.vil_rows.items():
        for h in positional_refs(t.get('HouseIDList')):
            R.hi.setdefault(h, dict(id=J['HOUSE_ID'].get(h), line='REST' if 'REST' in vk else 'VIL', villages=[]))['villages'].append(vk)
    # --- worlds: the worlds the scripts name + the village/rest/field WORLD_IDs + every WI_*ELYSION* name
    R.wi = collections.OrderedDict()

    def addwi(name, line):
        if name in J['WORLD_ID']:
            R.wi.setdefault(name, dict(id=J['WORLD_ID'][name], line=line))

    for did, d in list(R.di.items()) + list(R.di_hero.items()):
        for w in WI_RE.findall(ST.jtext(d['script']) or ''):
            addwi(w, d['line'])
    for vk, t in R.vil_rows.items():
        w = tail(t.get('WORLD_ID') or '')
        addwi(w, 'REST' if 'REST' in vk else 'VIL')
    for bid, t in R.bf_rows.items():
        addwi(tail(t.get('WORLD_ID') or ''), 'FLD')
    for k in J['WORLD_ID']:
        if 'ELYSION' in k:
            addwi(k, 'REST' if 'REST' in k else 'FLD' if ('PVPMAP' in k or 'FIELD' in k) else 'VIL')
        elif re.match(r'^WI_(ATLASDEVELOPMENT|ATLASCITY|TOWER|SKYCROSSROADS|ADRIAN|SOLESS|MEMORY)_', k):
            addwi(k, line_of_name(k))                        # enum families: WI_TOWER_STAGE3 (no script names it)
    # --- per-group file roots -> NUI closure (the line(s) that reach each NUI)
    R.roots = collections.OrderedDict()        # line -> [lua file names]
    for did, d in R.di.items():
        R.roots.setdefault(d['line'], []).append(d['script'])
    R.roots.setdefault('FLD', [])
    for bid, t in R.bf_rows.items():
        for key in ('NPC_TABLE_FILE_NAME', 'LINE_MAP_FILE_NAME'):
            fn = unq(t.get(key) or '')
            if fn:
                R.roots['FLD'].append(fn)
    R.direct_nui = collections.defaultdict(set)           # line -> NUI names named by roots that are not scripts (houses, directions)
    for h, d in R.hi.items():
        row = house_rows.get(h)
        if row is not None:
            for n in positional_refs(row.get('NPC_ID_LIST')):
                R.direct_nui[d['line']].add(n)
    if R.bf_lanox:
        for n in NUI_RE.findall(' '.join(str(x) for x in leaves(R.bf_lanox[1]))):
            R.direct_nui['FLD'].add(n)
    return R


def npc_lua_map():
    """June NPCTemplet: NUI name -> unit script name."""
    out = {}
    for t in method_tables(read_text(js('data037', 'NPCTemplet')), 'AddNPCTemplet'):
        out[tail(t.get('NPC_ID') or '')] = unq(t.get('LUA_FILE_NAME') or '')
    return out


def nui_closure(R, J):
    """line -> {NUI name}: names named by the line's roots, then by the unit scripts those NUIs run (June source)."""
    lua_of = npc_lua_map()
    R.nui_lua = lua_of
    R.nui = collections.OrderedDict()
    unit_files = collections.defaultdict(set)             # lower unit script -> lines
    for line in list(R.roots) + [l for l in R.direct_nui if l not in R.roots]:
        seen, queue = set(), []
        for fn in R.roots.get(line, []):
            for n in NUI_RE.findall(ST.jtext(fn) or ''):
                if n not in seen:
                    seen.add(n); queue.append(n)
        for n in R.direct_nui.get(line, ()):
            if n not in seen:
                seen.add(n); queue.append(n)
        while queue:
            n = queue.pop()
            lf = lua_of.get(n, '')
            if lf:
                unit_files[lf.lower()].add(line)
                for m in NUI_RE.findall(ST.jtext(lf) or ''):
                    if m not in seen:
                        seen.add(m); queue.append(m)
        for n in seen:
            if n in J['NPC_UNIT_ID']:
                R.nui.setdefault(n, dict(id=J['NPC_UNIT_ID'][n], lines=set()))['lines'].add(line)
    R.unit_files = unit_files
    return R


# ----------------------------------------------------------------------------------------------------------------------
# Region file set (June source) and the text each file contributes
# ----------------------------------------------------------------------------------------------------------------------


class Item(object):
    """One census row.  state: OPEN (a gap a phase must close), INFO (baseline fact), OK."""
    __slots__ = ('check', 'line', 'subject', 'state', 'detail')

    def __init__(self, check, line, subject, state, detail=''):
        self.check, self.line, self.subject, self.state, self.detail = check, line, subject, state, detail

    def half(self):
        return HALF_OF_LINE.get(self.line, 'shared')

    def row(self):
        return [self.half(), self.line or '-', self.check, self.subject, self.state, ASCII(str(self.detail))]


def file_set(R, J):
    """-> OrderedDict lower file name -> dict(name, lines:set, via:set, text) for every June source file the region owns.

    Roots: dungeon scripts, the unit scripts of region NUIs, the world scripts + line maps of region worlds, the battle-field
    NPC tables / line maps, and every June source whose own name says Elysion / Elsion / Atlas (the dialogs).  Registry rows
    are added separately (row_texts), they have no file of their own."""
    files = collections.OrderedDict()

    def add(name, line, via):
        low = name.lower()
        e = files.setdefault(low, dict(name=name, lines=set(), via=set(), text=ST.jtext(name)))
        e['lines'].add(line)
        e['via'].add(via)
        return e

    for did, d in R.di.items():
        add(d['script'], d['line'], 'dungeon')
    for lf, lines in R.unit_files.items():
        for l in lines:
            add(lf, l, 'unit')
    wd = {}
    for t in method_tables(read_text(js('data040', 'WorldData')), 'AddWorldData'):
        wd.setdefault(unq(t.get('worldName') or '').strip(), t)
    R.wd_rows = wd
    for w, d in R.wi.items():
        t = wd.get(w)
        if t is None:
            continue
        for key in ('dataFileName', 'dataFileName_Additional'):
            v = t.get(key)
            fns = [unq(x) for x in v.positional()] if isinstance(v, BC.Table) else [unq(v or '')]   # Soless boss: a table of names
            for fn in fns:
                if fn:
                    e = add(fn, d['line'], 'world')
                    for m in ASSET_RE.findall(e['text'] or ''):
                        if m.lower().endswith('.lua') and m.lower() in ST.jsrc:
                            add(m, d['line'], 'linemap')
    for bid, t in R.bf_rows.items():
        for key in ('NPC_TABLE_FILE_NAME', 'LINE_MAP_FILE_NAME'):
            fn = unq(t.get(key) or '')
            if fn:
                add(fn, 'FLD', 'field')
    for low, path in ST.jsrc.items():
        if re.search(r'elysion|elsion|atlas', low) and low not in files:
            add(os.path.basename(path)[:-8] + '.lua', 'UI', 'name')
    return files


def row_texts(R):
    """Registry rows as pseudo text: the leaves of every region row, per line.  -> [(line, source, text)]"""
    out = []

    def rowtext(t):
        return '\n'.join(('%s = %s' % (k, v)) if k else str(v) for k, v in walk(t))

    for did, d in R.di.items():
        out.append((d['line'], 'DungeonData', rowtext(R.dd_rows[did])))
    for k, t in R.vil_rows.items():
        out.append(('REST' if 'REST' in k else 'VIL', 'DLG_Map_Village', rowtext(t)))
    for bid, t in R.bf_rows.items():
        out.append(('FLD', 'BattleFieldData', rowtext(t)))
    if R.bf_lanox:
        out.append(('FLD', 'BattleFieldData(Lanox F04)', rowtext(R.bf_lanox[1])))
    for h, d in R.hi.items():
        row = R.house_rows.get(h)
        if row is not None:
            out.append((d['line'], 'DLG_Map_House', rowtext(row)))
    for w, d in R.wi.items():
        t = R.wd_rows.get(w)
        if t is not None:
            out.append((d['line'], 'WorldData', rowtext(t)))
    return out


# ----------------------------------------------------------------------------------------------------------------------
# C++ side: end markers, string literals, bound Lua names
# ----------------------------------------------------------------------------------------------------------------------

FLAGS_CACHE = os.path.join(HERE, 'region_census_flags.tsv')
CPP_DIRS = ('X2Lib', 'KTDXLIB', 'X2')


_PROBED = set()


def load_flags(probe_needed=None, probe=True):
    """Merge buff_census's cached flag table with ours; probe flags neither knows (cl /P via flagprobe.py, ~45 s, cached).
    A flag is probed at most once per run, resolved or not."""
    cache = BC.load_flag_cache()
    if os.path.exists(FLAGS_CACHE):
        for l in open(FLAGS_CACHE):
            p = l.split()
            if len(p) == 2:
                cache[p[0]] = (p[1] == 'ON')
    if probe_needed and probe:
        todo = sorted(f for f in probe_needed if f not in cache and f not in _PROBED and re.match(r'^[A-Za-z_]\w+$', f))
        if todo:
            _PROBED.update(todo)
            res = BC.run_flag_probe(todo)
            cache.update(res)
            with open(FLAGS_CACHE, 'a', newline='\n') as fh:
                for f, v in sorted(res.items()):
                    fh.write('%s %s\n' % (f, 'ON' if v else 'OFF'))
    return cache


def prime_flags(paths, probe=True):
    """Probe every unknown flag of all `paths` in ONE flagprobe run."""
    unknown = set()
    for p in paths:
        BC.active_text(p, unknown)
    load_flags(unknown, probe)


def active(path, probe=True):
    """Flag-resolved C++ text of one file (flags probed beforehand by prime_flags)."""
    unknown = set()
    return BC.active_text(path, unknown), unknown


END_MARKERS = [('VMI_BATTLE_FIELD_REST_END', 'KncWX2Server/Common/Enum/Enum.h', 'VILLAGE_MAP_ID', 'VILLAGE_MAP_ID'),
               ('VMI_VILLAGE_MAP_END', 'KncWX2Server/Common/Enum/Enum.h', 'VILLAGE_MAP_ID', 'VILLAGE_MAP_ID'),
               ('VMI_BATTLE_FIELD_END', 'KncWX2Server/Common/Enum/Enum.h', 'VILLAGE_MAP_ID', 'VILLAGE_MAP_ID'),
               ('WI_END', 'X2Lib/X2World.h', 'WORLD_ID', 'WORLD_ID'),
               ('NUI_MAX_LIMIT', 'X2Lib/X2UnitManager.h', 'NPC_UNIT_ID', 'NPC_UNIT_ID'),
               ('DI_END', 'KncWX2Server/Common/Enum/DungeonEnum.h', 'DUNGEON_ID', 'DUNGEON_ID')]


def cpp_markers(probe=True):
    """-> {marker: value or None} resolved with the US_SERVICE flag states (probed once, cached)."""
    out = {}
    prime_flags([os.path.join(TRUNK, m[1].replace('/', os.sep)) for m in END_MARKERS], probe)
    for name, rel, cenum, _ in END_MARKERS:
        text, unk = active(os.path.join(TRUNK, rel.replace('/', os.sep)), probe)
        en = BC.parse_cpp_enums(text)
        out[name] = en.get(cenum, {}).get(name)
    return out


def cpp_literals():
    """Every quoted identifier-like string in X2Lib / KTDXLIB / X2 sources (key literals, binding names)."""
    lit = set()
    for d in CPP_DIRS:
        for root, _, fs in os.walk(os.path.join(TRUNK, d)):
            for f in fs:
                if f.endswith(('.cpp', '.h', '.inl')):
                    b = open(os.path.join(root, f), 'rb').read()
                    lit |= {m.decode('latin-1') for m in re.findall(rb'"([A-Za-z_][A-Za-z0-9_%]{2,80})"', b)}
    return lit


BIND_RE = re.compile(r'lua_tinker\s*::\s*(?:class_def|def|decl|set|class_mem)\s*(?:<[^>;]*>)?\s*\(\s*L\s*,\s*"([A-Za-z_][A-Za-z0-9_]*)"')
# GetStoredGUUser was the ledger's unbound control; june-effect C3z-4 bound it (X2Main.cpp:4719), so the unbound control is now
# AddMovableLineData_LUA (E5 owns it)
BIND_CONTROLS = {'SetFlag_LUA': True, 'SetForceInvincible': True, 'GetStoredGUUser': True, 'AddMovableLineData_LUA': False}


def cpp_bound_names(probe=True):
    """Names registered with lua_tinker::class_def/def/decl, flag-resolved.  -> (set, controls_ok, n_files)"""
    names, nfiles, paths = set(), 0, []
    for d in CPP_DIRS:
        for root, _, fs in os.walk(os.path.join(TRUNK, d)):
            for f in fs:
                if f.endswith(('.cpp', '.h', '.inl')):
                    p = os.path.join(root, f)
                    if b'lua_tinker' in open(p, 'rb').read():
                        paths.append(p)
    prime_flags(paths, probe)
    for p in paths:
        text, _ = active(p, probe)
        nfiles += 1
        names |= set(BIND_RE.findall(text))
    ok = all((n in names) == want for n, want in BIND_CONTROLS.items())
    return names, ok, nfiles


# ----------------------------------------------------------------------------------------------------------------------
# Checks.  Each returns a list of Item (OPEN rows only; counts go to the stats dict)
# ----------------------------------------------------------------------------------------------------------------------

LINE_ORDER = ['REST', 'FLD', 'VIL', 'UI', 'AD', 'AC', 'TW', 'SC', 'AR', 'SL', 'ME']
PREFIX_TABLE = {'HI_': 'HOUSE_ID', 'DI_': 'DUNGEON_ID', 'VMI_': 'VILLAGE_MAP_ID', 'NUI_': 'NPC_UNIT_ID', 'WI_': 'WORLD_ID',
                'LMI_': 'LOCAL_MAP_ID'}
OLD_NUI = ['NUI_ADAMS', 'NUI_ARIEL', 'NUI_BILLBOARD', 'NUI_CAMILLA', 'NUI_GLAVE', 'NUI_LURIEL', 'NUI_MU', 'NUI_FIRETEMPLE_GATE',
           'NUI_FIRE_DEVIL_BERSERKER', 'NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE', 'NUI_MOUNT_FIREBIRD_FIRETEMPLE', 'NUI_SECRET_MAG_NOM',
           'NUI_SECRET_MAG_CHARGER', 'NUI_SECRET_SUMMON_MAG_NOM', 'NUI_SECRET_MAGI_OBJECT', 'NUI_SECRET_LAVA_DUMMY']   # ledger: "16 NUIs the region uses already exist in March"
KEY_RENAMES = {'OtherPlaceID': 'BattleFieldId', 'OtherPlaceStartLineIndex': 'BattleFieldStartLineIndex'}  # ledger "Key rename" hint


def jl(lines):
    """Item.line text for one or several lines (canonical order, '+' joined)."""
    ls = [l for l in LINE_ORDER if l in lines]
    return '+'.join(ls) or '-'


def line_in(item_line, want):
    return want is None or want in item_line.split('+')


def check_ids(R, J, M, markers):
    """Region set vs March now, collisions, value mismatches.  -> (items, stats)"""
    items, stats = [], {}
    tables = [('WORLD_ID', 'WI', R.wi), ('VILLAGE_MAP_ID', 'VMI', R.vmi), ('LOCAL_MAP_ID', 'LMI', R.lmi), ('HOUSE_ID', 'HI', R.hi),
              ('DUNGEON_ID', 'DI', R.di), ('NPC_UNIT_ID', 'NUI', R.nui)]
    marker_of = {'WORLD_ID': ['WI_END'], 'VILLAGE_MAP_ID': ['VMI_BATTLE_FIELD_REST_END', 'VMI_VILLAGE_MAP_END', 'VMI_BATTLE_FIELD_END'],
                 'NPC_UNIT_ID': ['NUI_MAX_LIMIT'], 'DUNGEON_ID': ['DI_END']}
    # which marker bounds which value range (the C++ range helpers): value must sit below the marker of its band
    band = {'VMI_BATTLE_FIELD_REST_END': (1000, 2000), 'VMI_VILLAGE_MAP_END': (20000, 29999), 'VMI_BATTLE_FIELD_END': (40000, 49999)}
    relocs = {t: {n: (J[t][n], M[t][n]) for n in J[t] if n in M[t] and J[t][n] != M[t][n]} for t in J}
    stats['relocations'] = {t: sorted(r) for t, r in relocs.items() if r}
    ncoll = nmark = 0
    for t, short, reg in tables:
        mby = collections.defaultdict(list)
        for n, v in M[t].items():
            mby[v].append(n)
        jby = collections.defaultdict(list)
        for n, d in reg.items():
            jby[d['id']].append(n)
        miss = pres = bad = 0
        for n, d in reg.items():
            line = jl(d['lines']) if 'lines' in d else d['line']
            v = d['id']
            if n in M[t]:
                if M[t][n] == v:
                    pres += 1
                else:
                    bad += 1
                    items.append(Item('ids.value', line, n, 'OPEN', '%s: June %s, March %s' % (t, v, M[t][n])))
                continue
            miss += 1
            problems, marks = [], []
            clash = [x for x in mby.get(v, []) if x != n]
            if clash:
                problems.append('March has %s at %d' % (','.join(clash[:3]), v))
            twin = [x for x in jby.get(v, []) if x != n]
            if twin:
                problems.append('incoming twin %s' % ','.join(twin[:3]))
            for rn, (jv, mv) in relocs[t].items():
                if mv == v:
                    problems.append('relocated name %s now at %d (June %d)' % (rn, mv, jv))
            for mk in marker_of.get(t, []):
                mv = markers.get(mk)
                if mv is None:
                    continue
                lo, hi = band.get(mk, (0, 10 ** 9))
                if lo <= v <= hi and v >= mv:
                    marks.append('C++ marker %s=%s <= value' % (mk, mv))
            if problems:
                ncoll += 1
            if marks:
                nmark += 1
            items.append(Item('ids.missing', line, '%s=%s' % (n, v), 'OPEN', t + ('; COLLISION: ' + '; '.join(problems) if problems else '')
                              + ('; MARKER: ' + '; '.join(marks) if marks else '')))
        stats[short] = dict(region=len(reg), present=pres, missing=miss, value_mismatch=bad)
    stats['collisions_among_missing'] = ncoll         # value collisions only (the ledger's "0 collisions")
    stats['cpp_marker_hits'] = nmark                  # missing IDs at or above a C++ end marker (A3 bumps them)
    # Lua-side sentinels the real tables carry (stale = A3 work)
    for t, k in (('WORLD_ID', 'WI_END'), ('VILLAGE_MAP_ID', 'VMI_BATTLE_FIELD_END'), ('VILLAGE_MAP_ID', 'VMI_VILLAGE_MAP_END'),
                 ('VILLAGE_MAP_ID', 'VMI_BATTLE_FIELD_REST_END'), ('DUNGEON_ID', 'DI_END')):
        if k in M[t] and k in J[t] and M[t][k] != J[t][k]:
            items.append(Item('ids.sentinel', 'UI', k, 'OPEN', 'Lua sentinel March %s, June %s' % (M[t][k], J[t][k])))
    return items, stats


def lj_all_calls(path, _cache={}):
    if path not in _cache:
        rows = []
        for ln in run_lj('calls', path):
            p = ln.split('\t')
            if p[0] == 'C':
                rows.append((p[1], dict(kv.partition('=')[::2] for kv in p[2:])))
        _cache[path] = rows
    return _cache[path]


def calls_of(path, *methods):
    return [r for m, r in lj_all_calls(path) if m in methods]


def check_npc(R, M):
    """NPC readiness: Enum + NPCTemplet + unit script staged + stat row.  -> (items, stats)"""
    mt = {tail(r.get('NPC_ID', '')): unq(r.get('LUA_FILE_NAME', '')) for r in calls_of(kc('data037', 'NPCTemplet.lua'), 'AddNPCTemplet')}
    m_stat = set()
    for p in (kc('data037', 'NPCStat.lua'), kc('data036', 'NewNPCStat.lua')):
        if os.path.exists(p):
            m_stat |= {tail(r.get('NPC_ID', '')) for r in calls_of(p, 'AddNPCStat', 'AddExtraNPCStat')}
    j_stat = set()
    for stem, fn in (('data037', 'NPCStat'), ('data036', 'NewNPCStat')):
        t = read_text(js(stem, fn))
        for meth in ('AddNPCStat', 'AddExtraNPCStat'):
            j_stat |= {tail(x.get('NPC_ID') or '') for x in method_tables(t, meth)}
    items, ready, n_new = [], 0, 0
    for n, d in R.nui.items():
        lines = jl(d['lines'])
        gaps = []
        if n not in M['NPC_UNIT_ID']:
            gaps.append('enum')
        if n not in mt:
            gaps.append('NPCTemplet')
        lf = R.nui_lua.get(n, '')
        if lf and lf.lower() not in ST.staged:
            gaps.append('script:%s' % lf)
        if n in j_stat and n not in m_stat:
            gaps.append('stat')
        if gaps:
            items.append(Item('npc.ready', lines, n, 'OPEN', 'missing ' + ','.join(gaps)))
        else:
            ready += 1
    return items, dict(region=len(R.nui), ready=ready, not_ready=len(R.nui) - ready, march_templets=len(mt), march_stat_rows=len(m_stat),
                       june_stat_rows=len(j_stat))


_KEYTAB = re.compile(r'^[ \t]+([A-Za-z_][A-Za-z0-9_]{2,})\s*=(?!=)', re.M)


def table_keys(tab, out):
    for k, v in tab.items:
        if k and not k.startswith('['):
            out.add(k)
        if isinstance(v, BC.Table):
            table_keys(v, out)
    return out


def check_keys(R, files, rows, lit):
    """Registry key-name diff and file-key diff: key names the region's June data uses that no C++ literal spells."""
    items, stats = [], {}
    # registry rows
    reg_src = collections.defaultdict(lambda: [set(), set()])      # (registry, key) -> [lines, row ids]
    for did, d in R.di.items():
        for k in table_keys(R.dd_rows[did], set()):
            reg_src[('DungeonData', k)][0].add(d['line'])
    for vk, t in R.vil_rows.items():
        for k in table_keys(t, set()):
            reg_src[('DLG_Map_Village', k)][0].add('REST' if 'REST' in vk else 'VIL')
    for bid, t in R.bf_rows.items():
        for k in table_keys(t, set()):
            reg_src[('BattleFieldData', k)][0].add('FLD')
    for h, d in R.hi.items():
        row = R.house_rows.get(h)
        if row is not None:
            for k in table_keys(row, set()):
                reg_src[('DLG_Map_House', k)][0].add(d['line'])
    for w, d in R.wi.items():
        t = R.wd_rows.get(w)
        if t is not None:
            for k in table_keys(t, set()):
                reg_src[('WorldData', k)][0].add(d['line'])
    unread = 0
    for (reg, k), (lines, _) in sorted(reg_src.items()):
        if BC.lit_supported(k, lit):
            continue
        unread += 1
        hint = (' (March reads %s)' % KEY_RENAMES[k]) if k in KEY_RENAMES else ''
        items.append(Item('keys.registry', jl(lines), '%s.%s' % (reg, k), 'OPEN', 'no C++ literal spells it' + hint))
    stats['registry_unread'] = unread
    # per-file keys
    fk = collections.defaultdict(lambda: [set(), set()])             # (via, key) -> [lines, file names]
    for low, e in files.items():
        if not e['text']:
            continue
        via = sorted(e['via'])[0]
        for k in set(_KEYTAB.findall(e['text'])):
            fk[(via, k)][0].update(e['lines'])
            fk[(via, k)][1].add(e['name'])
    nfk = 0
    for (via, k), (lines, names) in sorted(fk.items()):
        if BC.lit_supported(k, lit) or BC.lit_supported(re.sub(r'\d+$', '', k), lit) or re.sub(r'\d+$', '%d', k) in lit:
            continue                                # numbered keys (CLEAR_COND0, NEXT_STAGE0) are built from a stem literal
        if via == 'name':
            continue                                # dialogs are read by the GUI parser, not by key literals
        if via == 'unit' and not re.match(r'^[A-Z][A-Z0-9_]*$', k):
            continue                                # unit-script sub-tables use CAPS keys; mixed case there is data
        nfk += 1
        items.append(Item('keys.file', jl(lines), '%s:%s' % (via, k), 'OPEN', 'in %d file(s), no C++ literal: %s' % (len(names), ','.join(sorted(names))[:90])))
    stats['file_unread'] = nfk
    return items, stats


LUA_STD = set('format sub find gsub gmatch match len rep lower upper byte char insert remove concat sort getn setn foreach foreachi '
              'unpack pairs ipairs next type tostring tonumber select error assert pcall xpcall setmetatable getmetatable rawget rawset '
              'random randomseed floor ceil abs min max sqrt sin cos tan pow fmod'.split())


def june_bytes(name):
    t = ST.june.get(name.lower())
    if not t:
        return None
    path, v04, e = t
    with open(path, 'rb') as fh:
        fh.seek(e['offset'])
        raw = fh.read(e['csize'])
    out, err = junekom.decode(e, raw, v04)
    return out


_UNIVERSE = []


def method_universe():
    """Every `obj:Name(` name anywhere in june_src_fixed (what counts as method-like when scanning bytecode strings)."""
    if not _UNIVERSE:
        s = set()
        for kd in os.listdir(JS):
            d = os.path.join(JS, kd)
            if os.path.isdir(d):
                for f in os.listdir(d):
                    if f.endswith('.src.lua'):
                        s |= set(METHOD_RE.findall(open(os.path.join(d, f), 'rb').read().decode('utf-8', 'replace')))
        _UNIVERSE.append(s)
    return _UNIVERSE[0]


def check_bindings(files, bound, lit):
    """Lua method names the region's June scripts call that no lua_tinker line spells (name match only: receiver class is A1b's)."""
    uses = collections.defaultdict(lambda: [set(), set()])
    bytecode_only = []
    for low, e in files.items():
        t = e['text']
        if t is None:
            bytecode_only.append(e)
            continue
        for m in set(METHOD_RE.findall(t)):
            if m in LUA_STD or m in bound:
                continue
            uses[m][0].update(e['lines'])
            uses[m][1].add(e['name'])
    # names defined in Lua itself (function X:Name) are not bindings
    deflua = set()
    for low, e in files.items():
        if e['text']:
            deflua |= set(re.findall(r'function\s+[A-Za-z_]\w*\s*[:.]\s*([A-Za-z_]\w*)', e['text']))
    items, n_src = [], 0
    for m, (lines, names) in sorted(uses.items()):
        if m in deflua:
            continue
        n_src += 1
        items.append(Item('bind.unbound', jl(lines), m, 'OPEN',
                          '%d file(s)%s: %s' % (len(names), ', string-only in C++' if m in lit else '', ','.join(sorted(names))[:90])))
    n_byte = 0
    uni = method_universe()
    already = set(uses)
    for e in bytecode_only:
        b = june_bytes(e['name'])
        if b is None:
            items.append(Item('bind.nosource', jl(e['lines']), e['name'], 'INFO',
                              'named by June data but June ships no such file (ledger: Tower S3 -> missing file); nothing to port'))
            continue
        for m in sorted(set(x.decode('latin-1') for x in re.findall(rb'[A-Za-z_][A-Za-z0-9_]{3,}', b))):
            if m in uni and m not in bound and m not in LUA_STD and m not in already and m not in deflua:
                n_byte += 1
                already.add(m)
                items.append(Item('bind.unbound.bytecode', jl(e['lines']), m, 'OPEN',
                                  'inferred from %s bytecode strings (E0 owns the confirmation)' % e['name']))
    return items, dict(bound_names=len(bound), unbound_from_source=n_src, unbound_inferred_bytecode=n_byte,
                       bytecode_only_files=[e['name'] for e in bytecode_only])


def ess_text():
    raw = open(os.path.join(GAME, 'general.ess'), 'rb').read()[2:].decode('utf-16-le')
    out = {}
    for ln in raw.split('\r\n'):
        s = ''.join(chr(ord(c) ^ 16) for c in ln)
        if '\t' in s:
            k, v = s.split('\t', 1)
            if k.isdigit():
                out[int(k)] = v
    return out


def bytecode_text(e):
    """Files june_src_fixed lacks (ELYSION_SOLESS.lua, Elysion_Tower_Stage3.lua): the printable runs of June's own bytecode, each
    wrapped in quotes so the string / asset / effect-set regexes see them like source literals.  Labelled E0-owned in reports."""
    if 'btext' not in e:
        b = june_bytes(e['name'])
        # a constant's length byte is often printable and glues onto the name ("(EffectSet_..."): emit the run and the run minus
        # its first byte (bytecode-name-scan-lookahead-trap, C3z-1 review)
        runs = re.findall(rb'[ -~]{3,120}', b) if b else []
        e['btext'] = '\n'.join('"%s"\n"%s"' % (r.decode('latin-1'), r[1:].decode('latin-1')) for r in runs)
    return e['btext']


def all_texts(files, rows):
    for low, e in files.items():
        if e['text']:
            yield e['lines'], e['name'], e['text']
        else:
            t = bytecode_text(e)
            if t:
                yield e['lines'], e['name'], t
    for line, src, text in rows:
        yield {line}, src, text


def check_strings(files, rows, ess):
    sd = open(kc('data036', 'StringID_def.lua'), 'rb').read()
    defined = {int(x) for x in re.findall(rb'STR_ID_(\d+)', sd)}
    users = collections.defaultdict(set)
    for lines, src, text in all_texts(files, rows):
        for m in STR_RE.findall(text):
            users[int(m)].update(lines)
    items, undef, notext = [], 0, 0
    for sid, lines in sorted(users.items()):
        gaps = []
        if sid not in defined:
            gaps.append('not in StringID_def.lua'); undef += 1
        if not ess.get(sid):
            gaps.append('no text in general.ess'); notext += 1
        if gaps:
            items.append(Item('strings', jl(lines), 'STR_ID_%d' % sid, 'OPEN', '; '.join(gaps)))
    return items, dict(distinct=len(users), undefined=undef, no_text=notext, string_def_count=len(defined))


MEMBER_RE = re.compile(r'(?<![A-Za-z0-9_."\'])([A-Z][A-Z0-9_]{2,})\.([A-Za-z_][A-Za-z0-9_]*)\b')


def top_level_tables(text):
    """`Name = { ... }` at depth 0 -> [(name, Table)]"""
    kinds, texts = BC.tokenize(text)
    out, i, n = [], 0, len(texts)
    while i < n - 2:
        if kinds[i] == 'i' and texts[i + 1] == '=' and texts[i + 2] == '{' and (i == 0 or texts[i - 1] not in '.:'):
            tab, j = BC._parse_table(kinds, texts, i + 2)
            out.append((texts[i], tab))
            i = j
        else:
            i += 1
    return out


def check_members(files, rows, J, M, region_ids):
    """TABLE.MEMBER over every enum table Enum.lua / DungeonEnum.lua define: member unresolved in March (June resolves it) or nil in June too."""
    june_t = collections.OrderedDict()
    for fn in ('Enum', 'DungeonEnum'):
        for t, mem in BC.parse_enum_file(js('data036', fn)).items():
            june_t.setdefault(t, set()).update(mem)
    users = collections.defaultdict(lambda: [set(), set()])
    for lines, src, text in all_texts(files, rows):
        for tb, mb in set(MEMBER_RE.findall(text)):
            if tb in june_t:
                users[(tb, mb)][0].update(lines)
                users[(tb, mb)][1].add(src)
    march_t = {}
    for tb in sorted({t for t, _ in users}):
        d = {}
        for fn in ('Enum.lua', 'DungeonEnum.lua'):
            x, _e = lj_enum(kc('data036', fn), tb)
            d.update(x)
        march_t[tb] = d
    items, miss, jnil = [], 0, 0
    for (tb, mb), (lines, srcs) in sorted(users.items()):
        if mb in march_t[tb]:
            continue
        if tb in region_ids and mb in region_ids[tb]:
            continue                                  # already a check_ids row
        if mb in june_t[tb]:
            miss += 1
            items.append(Item('members.missing', jl(lines), '%s.%s' % (tb, mb), 'OPEN', 'June resolves it, March nil; used by %s' % ','.join(sorted(srcs))[:80]))
        else:
            jnil += 1
            items.append(Item('members.june_nil', jl(lines), '%s.%s' % (tb, mb), 'INFO', 'nil in June too (keep verbatim, JUNE_BUGS.md row); used by %s' % ','.join(sorted(srcs))[:80]))
    return items, dict(tables=len({t for t, _ in users}), pairs=len(users), missing_in_march=miss, june_nil=jnil)


LIST_KEY = re.compile(r'(IDList|_LIST|REQUIRE_DUNGEON_ID|RequireClearDungeonID)$')


def check_lists(R, M):
    """Every element of every ID list resolves, in order (a nil hole truncates the list in Lua)."""
    items, nlists, nholes = [], 0, 0

    def walk_tab(tab, where, line):
        nonlocal nlists, nholes
        for k, v in tab.items:
            if not isinstance(v, BC.Table):
                continue
            if k and LIST_KEY.search(k):
                refs = [tail(x) for x in v.positional() if isinstance(x, str) and re.match(r'^[A-Z_]+\.[A-Z]+_', x.strip())]
                if refs:
                    nlists += 1
                    bad = [(i, r) for i, r in enumerate(refs) if PREFIX_TABLE.get(re.match(r'^[A-Z]+_', r).group(0)) and
                           r not in M[PREFIX_TABLE[re.match(r'^[A-Z]+_', r).group(0)]]]
                    if bad:
                        nholes += 1
                        items.append(Item('lists.hole', line, '%s.%s' % (where, k), 'OPEN',
                                          '%d of %d unresolved, first at %d (%s)' % (len(bad), len(refs), bad[0][0], bad[0][1])))
            walk_tab(v, where, line)

    for did, d in R.di.items():
        walk_tab(R.dd_rows[did], 'DungeonData/' + did, d['line'])
    for vk, t in R.vil_rows.items():
        walk_tab(t, 'DLG_Map_Village/' + vk, 'REST' if 'REST' in vk else 'VIL')
    for h, d in R.hi.items():
        row = R.house_rows.get(h)
        if row is not None:
            walk_tab(row, 'DLG_Map_House/' + h, d['line'])
    lm = ST.jtext('DLG_Map_Local_Elysion.lua')
    if lm:
        for nm, t in top_level_tables(lm):
            walk_tab(BC.Table([(nm, t)]), 'DLG_Map_Local_Elysion', 'UI')
    return items, dict(lists=nlists, with_holes=nholes)


def check_effects(files, rows):
    """Effect-set templets the region names: June's EffectSet defines them, is the staged EffectSet.lua carrying them?"""
    # June's decompiled EffectSet.src.lua does not run under luajit (line 3 `uv0(...)`): read its names as text
    j_names = {unq(t.get('NAME') or '') for t in method_tables(read_text(js('data036', 'EffectSet')), 'AddEffectSetTemplet_LUA')}
    m_names = {unq(r.get('NAME', '')) for r in calls_of(kc('data036', 'EffectSet.lua'), 'AddEffectSetTemplet_LUA')}
    refs = collections.defaultdict(set)
    for lines, src, text in all_texts(files, rows):
        for q in set(re.findall(r'"([^"\n]{3,80})"', text)):
            if q in j_names:
                refs[q].update(lines)
    elys = sorted(n for n in m_names if 'elysion' in n.lower() or 'elsion' in n.lower())
    items = [Item('effects', jl(lines), n, 'OPEN', 'June EffectSet templet, absent from the staged EffectSet.lua')
             for n, lines in sorted(refs.items()) if n not in m_names]
    return items, dict(referenced=len(refs), present=sum(1 for n in refs if n in m_names), missing=len(items),
                       june_templets=len(j_names), march_templets=len(m_names), march_elysion_named=len(elys))


def check_items(R):
    """Item ids the region's DungeonData rows drop: in the staged item.kim (June's since K2)?"""
    sys.path.insert(0, os.path.join(TRUNK, 'tools'))
    import kim as K
    staged, june = K.Kim('staged'), K.Kim('june')
    idx_s, idx_j = staged.index(), june.index()
    refs = collections.defaultdict(set)
    for did, d in R.di.items():
        t = R.dd_rows[did]
        bi = t.get('BOSS_DROP_ITEM')
        if isinstance(bi, BC.Table):
            for k, v in walk(bi):
                if re.match(r'^\d+$', v.strip()):
                    refs[int(v)].add(d['line'])
        for k, v in walk(t):
            if k and re.search(r'ITEM_?ID$', k, re.I) and re.match(r'^\d+$', v.strip()):
                refs[int(v)].add(d['line'])
    items = []
    for iid, lines in sorted(refs.items()):
        if iid not in idx_s:
            items.append(Item('items', jl(lines), str(iid), 'OPEN',
                              'not in staged item.kim' + ('' if iid in idx_j else ' and not in June item.kim either')))
    return items, dict(referenced=len(refs), missing=len(items))


def check_assets(files, rows):
    refs = collections.defaultdict(set)
    for lines, src, text in all_texts(files, rows):
        for m in set(ASSET_RE.findall(text)):
            refs[m.strip().lower()].update(lines)
    items, c = [], collections.Counter()
    known = lambda n: n in ST.staged or n in ST.live or n in ST.june or n in ST.jsrc
    for low, lines in sorted(refs.items()):
        if not known(low) and known(low[1:]):
            continue                                  # bytecode length byte glued onto the name (bytecode_text emits both variants)
        if low in ST.staged or low in ST.live:
            c['in_march'] += 1
        elif low in ST.june:
            c['june_only'] += 1
            items.append(Item('assets.adopt', jl(lines), low, 'OPEN', 'June only, %d B' % ST.june[low][2].get('size', ST.june[low][2].get('usize', 0))))
        elif low in ST.jsrc:
            c['june_lua_source_only'] += 1
            items.append(Item('assets.adopt', jl(lines), low, 'OPEN', 'June Lua (june_src_fixed only), no June kom entry decoded'))
        else:
            c['not_found'] += 1
            items.append(Item('assets.notfound', jl(lines), low, 'OPEN', 'in neither June kom index nor March'))
    return items, dict(refs=len(refs), **c)


def mount_rank(stem):
    m = re.match(r'(datai?)(\d+)', stem)
    return (1 if m.group(1) == 'datai' else 0, int(m.group(2)))


def check_koms(files, live_sizes=False):
    """Staged region files: header, manifest entry, winning mount; unstaged files are the A-phase's staging list."""
    items, c = [], collections.Counter()
    manifests = {}
    for low, e in files.items():
        name, lines = e['name'], e['lines']
        holders = ST.staged.get(low, {})
        livek = ST.live.get(low, {})
        if not holders:
            c['unstaged'] += 1
            tag = 'staged-nowhere' + ('; live ' + ','.join(sorted(livek)) if livek else '')
            if e['text'] is None:
                tag += '; not in june_src_fixed'
            items.append(Item('koms.unstaged', jl(lines), name, 'OPEN', tag))
            continue
        c['staged'] += 1
        for stem, real in sorted(holders.items()):
            p = os.path.join(KC, stem + '_kom', real)
            if real.lower().endswith('.lua'):
                with open(p, 'rb') as fh:
                    h = fh.read(4)
                kind = 'bytecode81' if h == b'\x1bKL\x81' else 'bom-source' if h[:3] == b'\xef\xbb\xbf' else 'JUNE82' if h == b'\x1bKL\x82' else 'plain-no-bom'
                if kind in ('JUNE82', 'plain-no-bom'):
                    c['bad_header'] += 1
                    items.append(Item('koms.header', jl(lines), '%s/%s' % (stem, real), 'OPEN', 'pack gate refuses: ' + kind))
            if stem not in manifests:
                manifests[stem] = kom.load_manifest(os.path.join(KC, stem + '_kom'))
            want = '2' if real.lower().endswith('.lua') else '0'
            got = manifests[stem].get(real)
            if got is None:
                c['manifest_miss'] += 1
                items.append(Item('koms.manifest', jl(lines), '%s/%s' % (stem, real), 'OPEN', 'no _manifest.json entry'))
            elif str(got) != want:
                c['manifest_wrong'] += 1
                items.append(Item('koms.manifest', jl(lines), '%s/%s' % (stem, real), 'OPEN', 'manifest %s, expected %s' % (got, want)))
        allk = sorted(set(holders) | set(livek), key=mount_rank)
        if len(allk) > 1:
            sizes = {}
            for stem in holders:
                sizes[stem] = os.path.getsize(os.path.join(KC, stem + '_kom', holders[stem]))
            c['multi_mount'] += 1
            if len(set(sizes.values())) > 1:
                items.append(Item('koms.mount', jl(lines), name, 'OPEN',
                                  'staged copies differ in size %s; winner by mount order: %s' % (sizes, allk[-1])))
    return items, dict(c)


# ----------------------------------------------------------------------------------------------------------------------
# Live-kom size check (--live)
# ----------------------------------------------------------------------------------------------------------------------


def check_live(files):
    """Staged region files vs the entry in the live kom of the same name: size differs = repack owed (the pack list)."""
    items, c = [], collections.Counter()
    by_kom = collections.defaultdict(list)
    for low, e in files.items():
        for stem, real in ST.staged.get(low, {}).items():
            if stem in (ST.live.get(low) or {}):
                by_kom[stem].append((e, real))
    for stem, lst in sorted(by_kom.items()):
        live_path = os.path.join(GAME, stem + '.kom')
        if not os.path.exists(live_path):
            continue
        sizes = {x['name'].lower(): x for x in kom.inspect(live_path)['files']}
        for e, real in lst:
            ent = sizes.get(real.lower())
            sp = os.path.join(KC, stem + '_kom', real)
            if not ent:
                continue
            ls = ent.get('size', ent.get('usize', ent.get('uncompressed_size')))
            ss = os.path.getsize(sp)
            if ls != ss:
                c['size_mismatch'] += 1
                items.append(Item('live.size', jl(e['lines']), '%s/%s' % (stem, real), 'OPEN', 'staged %d B, live %s B (repack owed)' % (ss, ls)))
            else:
                c['same'] += 1
    return items, dict(c)


# ----------------------------------------------------------------------------------------------------------------------
# Baseline (the 2026-10-03 ledger) and the CLI
# ----------------------------------------------------------------------------------------------------------------------

LEDGER = collections.OrderedDict([
    ('WI', 41), ('VMI', 7), ('LMI', 1), ('HI', 12), ('DI', 21), ('NUI', 203), ('collisions', 0), ('unbound_names', 65),
    ('effect_sets', 393), ('assets_to_adopt', 2071)])

PHASE_SCOPE = {      # phase -> (kinds prefixes, lines or None).  v1: which census rows a phase's Done gate reads.
    'A3': (('ids', 'members', 'lists'), None), 'A4': (('bind', 'keys'), ('AD', 'AC', 'TW')), 'A5': (None, ('REST',)),
    'A6': (None, ('FLD',)), 'A7': (None, ('UI',)), 'A8': (None, ('AD',)), 'A9': (None, ('AC',)), 'A10': (None, ('TW',)),
    'A11': (('assets', 'koms', 'live'), ('REST', 'FLD', 'AD', 'AC', 'TW', 'UI')),
    'E2': (None, ('VIL',)), 'E3': (None, ('FLD',)), 'E4': (None, ('UI',)), 'E5': (('bind', 'keys', 'members'), ('AR', 'SL')),
    'E6': (('bind', 'keys'), ('SL', 'ME')), 'E7': (('bind', 'keys'), ('SC', 'AR', 'SL', 'ME')),
    'E8': (None, ('SC',)), 'E9': (None, ('AR',)), 'E10': (None, ('SL',)), 'E11': (None, ('ME',)),
    'E13': (('assets', 'koms', 'live'), ('VIL', 'SC', 'AR', 'SL', 'ME'))}


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--region', choices=['atlas', 'elysion', 'shared'])
    ap.add_argument('--line', choices=sorted(set(list(LINES) + ['REST', 'FLD', 'VIL', 'UI'])))
    ap.add_argument('--phase', help='A3..A11 / E2..E13: print only the rows that phase gates on (v1: line + check filter)')
    ap.add_argument('--json', metavar='FILE')
    ap.add_argument('--tsv', action='store_true', help='write june2017_re/region_census.tsv')
    ap.add_argument('--live', action='store_true', help='also compare staged files with the live koms (runs elstools inspect)')
    ap.add_argument('--no-probe', action='store_true', help='do not run flagprobe for unknown C++ flags (uses the caches only)')
    ap.add_argument('--limit', type=int, default=12, help='rows listed per check in the text report (default 12; 0 = all)')
    a = ap.parse_args()
    if a.phase and a.phase.upper() not in PHASE_SCOPE:
        ap.error('--phase %s: v1 knows %s' % (a.phase, ' '.join(sorted(PHASE_SCOPE, key=lambda s: (s[0], int(s[1:]))))))
    probe = not a.no_probe

    J, M, enum_errs = load_enums()
    R = derive(J)
    nui_closure(R, J)
    files = file_set(R, J)
    rows = row_texts(R)
    markers = cpp_markers(probe)
    lit = cpp_literals()
    bound, controls_ok, nbfiles = cpp_bound_names(probe)
    ess = ess_text()

    results = collections.OrderedDict()
    stats = collections.OrderedDict()
    for name, fn in (('ids', lambda: check_ids(R, J, M, markers)), ('npc', lambda: check_npc(R, M)),
                     ('keys', lambda: check_keys(R, files, rows, lit)), ('bindings', lambda: check_bindings(files, bound, lit)),
                     ('strings', lambda: check_strings(files, rows, ess)),
                     ('members', lambda: check_members(files, rows, J, M, {t: set(r) for t, r in
                                                                           (('WORLD_ID', R.wi), ('VILLAGE_MAP_ID', R.vmi), ('LOCAL_MAP_ID', R.lmi),
                                                                            ('HOUSE_ID', R.hi), ('DUNGEON_ID', R.di), ('NPC_UNIT_ID', R.nui))})),
                     ('lists', lambda: check_lists(R, M)), ('effects', lambda: check_effects(files, rows)),
                     ('items', lambda: check_items(R)), ('assets', lambda: check_assets(files, rows)),
                     ('koms', lambda: check_koms(files))):
        results[name], stats[name] = fn()
    if a.live:
        results['live'], stats['live'] = check_live(files)
    stats['markers'] = markers
    stats['bind_controls_ok'] = controls_ok
    stats['bound_files'] = nbfiles
    stats['file_set'] = dict(files=len(files), by_via=dict(collections.Counter(v for e in files.values() for v in e['via'])))
    stats['enum_errors'] = [str(x) for x in enum_errs]

    allitems = [it for lst in results.values() for it in lst]

    def keep(it):
        if a.line and not line_in(it.line, a.line):
            return False
        if a.region and it.half() != a.region and not (a.region != 'shared' and it.half() == 'shared' and False):
            return False
        if a.phase:
            kinds, lines = PHASE_SCOPE[a.phase.upper()]
            if kinds and not it.check.startswith(kinds):
                return False
            if lines and not any(line_in(it.line, l) for l in lines):
                return False
        return True

    shown = [it for it in allitems if keep(it)]

    # ---- baseline vs ledger
    base = collections.OrderedDict()
    ids = stats['ids']
    for k in ('WI', 'VMI', 'LMI', 'HI', 'DI'):
        base[k] = ids[k]['region']
    # ledger-equivalent = region IDs March did not have on 2026-10-03 = missing now + adopted since.
    # Pre-existing: the 16 OLD_NUI (ledger list) and the houses that already had a March row; WI/VMI/LMI/DI had none.
    adopted = collections.OrderedDict()
    adopted['NUI'] = sum(1 for n in R.nui if n in M['NPC_UNIT_ID'] and n not in OLD_NUI)
    adopted['WI'] = ids['WI']['present']
    adopted['HI'] = 0
    for k in ('VMI', 'LMI', 'DI'):
        adopted[k] = ids[k]['present']
    preex_hi = ids['HI']['present']
    base['HI'] = ids['HI']['missing']
    base['NUI'] = ids['NUI']['missing'] + adopted['NUI']
    base['WI'] = ids['WI']['missing'] + adopted['WI']
    stats['adopted_since_ledger'] = dict(adopted, HI_preexisting_shared=preex_hi)
    base['collisions'] = ids['collisions_among_missing']
    base['unbound_names'] = stats['bindings']['unbound_from_source'] + stats['bindings']['unbound_inferred_bytecode']
    base['effect_sets'] = stats['effects']['referenced']
    base['assets_to_adopt'] = stats['assets'].get('june_only', 0) + stats['assets'].get('june_lua_source_only', 0)

    BASE_NOTES = {
        'WI': "ledger table rows sum to 40 (shared 7 + AD 4 + AC 4 + TW 5 + SC 4 + AR 5 + SL 6 + ME 5); '41' was an addition slip, 757 is deferred. "
              "4 boss worlds (645 654 675 710) were adopted since by june-dungeons C3y-1b (7b23b392)",
        'NUI': "ledger '201 region NPCs + EVENT_00/01 = 203' counted EVENT_00/01 twice: the 201 already contains 2139/2140. "
               "per-line tallies match Appendix A/B (AD 11, AR 34, ME 30, SL 43+2 shared, FLD 25 = 23+2). 8 adopted since by june-effect C3z-2 (b5f2265c)",
        'HI': 'reproduces: the 12 houses with no March HOUSE_ID (11 shared houses already existed)',
        'VMI': 'reproduces', 'LMI': 'reproduces', 'DI': 'reproduces (Hero rows kept apart: %d)' % len(R.di_hero),
        'collisions': "reproduces for ID values; 9 missing IDs sit at/above the C++ end markers (A3 bumps them, ledger section 'March C++ end markers')",
        'unbound_names': "10-03 count was 65; june-effect C3z-4 bound ~30 of the names the ledger lists (GetStoredGUUser, GetCenterPositionLineIndex, ...); "
                         "census = %d from sources + %d inferred from bytecode strings (ELYSION_SOLESS, Elysion_Tower_Stage3 are not in june_src_fixed)" % (
                             stats['bindings']['unbound_from_source'], stats['bindings']['unbound_inferred_bytecode']),
        'effect_sets': "premise 'staged EffectSet.lua has 0 Elysion entries' is FALSE now: c13f8115 (C11a full June effect re-adoption) staged June's "
                       "%d templets, so all referenced sets are present (missing: %d). Referenced count %d vs ledger 393: the ledger method is not recorded" % (
                           stats['effects']['june_templets'], stats['effects']['missing'], stats['effects']['referenced']),
        'assets_to_adopt': "ledger '>= 2,071' was a lower bound taken before June asset adoption: %d of %d refs are in March now (ledger: 249); "
                           "census refs %d vs ledger 2,383 (the census file set adds %d name-matched dialogs)" % (
                               stats['assets']['in_march'], stats['assets']['refs'], stats['assets']['refs'],
                               stats['file_set']['by_via'].get('name', 0))}
    base_rows = [(k, LEDGER[k], base[k], 'match' if base[k] == LEDGER[k] else 'DIFFERS', BASE_NOTES.get(k, '')) for k in LEDGER]
    stats['baseline'] = [list(r) for r in base_rows]
    out = io.StringIO()
    W = lambda s='': out.write(s + '\n')
    W('=== Atlas/Elysion region census v1 ===  (scope: %s)' % ' '.join(filter(None, [a.region and 'region=' + a.region,
                                                                                    a.line and 'line=' + a.line, a.phase and 'phase=' + a.phase])) or 'all')
    W('region files: %(files)d  by route %(by_via)s' % stats['file_set'])
    W('C++ end markers (US_SERVICE flag state): ' + ', '.join('%s=%s' % kv for kv in markers.items()))
    W('lua_tinker names bound: %d from %d files; calibration %s (SetFlag_LUA, SetForceInvincible, GetStoredGUUser bound; AddMovableLineData_LUA not)' % (
        len(bound), nbfiles, 'OK' if controls_ok else 'FAILED'))
    W()
    W('-- Region sets (June) vs March now')
    W('%-5s %7s %8s %8s %9s' % ('table', 'region', 'present', 'missing', 'mismatch'))
    for k in ('WI', 'VMI', 'LMI', 'HI', 'DI', 'NUI'):
        s = ids[k]
        W('%-5s %7d %8d %8d %9d' % (k, s['region'], s['present'], s['missing'], s['value_mismatch']))
    W('value collisions among the missing region IDs: %d   (separately, %d missing IDs sit at/above a C++ end marker: A3 bumps them)' % (
        ids['collisions_among_missing'], ids['cpp_marker_hits']))
    W('value relocations between June and March (same name, other value): ' + (str(ids['relocations']) or 'none'))
    W()
    W('-- Baseline vs the 2026-10-03 ledger (new-at-ledger-time counts; NUI excludes the 16 NUIs March already had)')
    W('%-16s %8s %8s  %s' % ('item', 'ledger', 'census', '(adopted into March since 10-03 are counted: census = missing now + adopted)'))
    for k, lv, cv, verdict, note in base_rows:
        W('%-16s %8s %8s  %s' % (k, lv, cv, verdict))
        if note:
            W('      ' + ASCII(note))
    W()
    W('-- Check summaries')
    for name, s in stats.items():
        if name in ('ids', 'markers', 'bind_controls_ok', 'bound_files', 'file_set', 'enum_errors'):
            continue
        W('%-9s %s' % (name, json.dumps(s, sort_keys=True)[:400]))
    W()
    by_check = collections.defaultdict(list)
    for it in shown:
        by_check[it.check].append(it)
    W('-- Open items in scope: %d' % sum(1 for it in shown if it.state == 'OPEN'))
    for ck in sorted(by_check):
        rows_ = by_check[ck]
        W('%-22s %5d' % (ck, len(rows_)))
        lim = a.limit or len(rows_)
        for it in rows_[:lim]:
            W('    [%s] %-44s %s' % (it.line, ASCII(it.subject)[:44], ASCII(str(it.detail))[:110]))
        if len(rows_) > lim:
            W('    ... %d more (--limit 0, --tsv or --json)' % (len(rows_) - lim))
    sys.stdout.write(out.getvalue())

    if a.json:
        with open(a.json, 'w', encoding='utf-8') as fh:
            json.dump(dict(stats=stats, baseline=base, ledger=LEDGER, items=[dict(zip(('half', 'line', 'check', 'subject', 'state', 'detail'), it.row())) for it in shown]),
                      fh, indent=1, default=str)
    if a.tsv:
        with open(os.path.join(HERE, 'region_census_baseline.tsv'), 'w', encoding='utf-8', newline='\n') as fh:
            fh.write('metric\tledger_2026_10_03\tcensus\tverdict\tnote\n')
            for r in base_rows:
                fh.write('\t'.join(str(x).replace('\t', ' ') for x in r) + '\n')
        with open(TSV_OUT, 'w', encoding='utf-8', newline='\n') as fh:
            fh.write('half\tline\tcheck\tsubject\tstate\tdetail\n')
            for it in shown:
                fh.write('\t'.join(x.replace('\t', ' ') for x in it.row()) + '\n')
    return 0


if __name__ == '__main__':
    sys.exit(main())
