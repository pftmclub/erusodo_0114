"""Phase 10b audit 1: asset closure of June2017's effect data.

    python assets.py            # ~3 min cold, ~1 min warm (assets_cache/)

Writes ../JUNE2017_ASSET_CLOSURE.md and asset_closure.tsv (one row per node).

Roots: every templet June's effect lua defines, found by EXECUTING the files
(ljconv.py + fxdump.lua, see fxdata.py), not by reading ljd's source:
  - every AddEffectSetTemplet_LUA call (EffectSet*, UIEffectSet*),
  - every table global first defined by a DamageEffect*.lua file,
  - DamageSound.lua's tables and Elsword_DamageData.lua's InsertDamageData_LUA calls.
EffectSet_Description / SetEffectSetVersion carry no references (March compiles
AddEffectSet_Description_LUA out, X2EffectSet.cpp:3380, EFFECT_TOOL).

Edges, each key's target taken from March's own reader (June's where March has none):
  EffectSet EFFECT%d NAME / HYPER_NAME  namespace by that entry's EFFECT_TYPE
                                        (X2EffectSet.cpp:2349, dispatch :203-811; HYPER_NAME :289-700)
  MAIN_EFFECT_NAME[%d|_ULTIMATE]        GameMajor XMeshPlayer, any DEET type (X2DamageEffect.cpp:1427-1447)
  PARTICLE_NAME                         GameMajor particle (X2DamageEffect.cpp:1922/5191/5289/5350,
                                        CUSTOM_HIT_PARTICLE X2DamageManager.cpp:252 -> X2GUUser.cpp:12710)
  DAMAGE_EFFECT_NAME, CREATE_DAMAGE_EFFECT_NAME          damage effect
  CREATE_EFFECT_SET_NAME, EFFECT_SET_NAME                effect set (June 31f64c80 / 321e48e0)
  FILE_NAME, HTD_*                      sound file (X2DamageEffect.cpp:2542 OpenSound)
  particle templet: Texture, Mesh, *Xet, Trace/FinalParticleList (same system, KTDGParticleSystem.cpp:1564)
  mesh templet: MeshName, SLASH_TRACE_TEXTURE, *Xet, AniPackage
  mesh (.x/.y) and .xet files: the texture names they embed (fxdata.embedded_textures)
Templet names resolve case-SENSITIVELY (std::map<wstring> in both engines and
CX2EffectSet; damage effects are Lua globals). File names resolve case-
insensitively (KGCMassFileManager upper-cases them).

Every string value met on the way is either an edge, a known non-reference
(bones, motions, DESC, AniName, ...) or reported as unclassified: the
completeness gate. PARTICLE_DNAME is a studio typo June's binary never reads.

Per file leaf: where it is (June .kom, March live .kom, march2014/update/,
kom_contents staged-only) and whether March's copy differs from June's:
  - exact: both copies readable -> SHA-1;
  - size: manifest sizes differ -> differs;
  - prefix: June's copy is an elstools-truncated algo-2 extraction (see fxdata.Copy):
    equal prefix -> "same-prefix" (tail unreadable), else differs.
Textures with a .tet partner in either build are flagged as atlases.
"""
import collections
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import fxdata as F   # noqa: E402

OUT_MD = os.path.join(F.TRUNK, 'JUNE2017_ASSET_CLOSURE.md')
OUT_TSV = os.path.join(HERE, 'asset_closure.tsv')

ET_NS = {'ET_DAMAGE_EFFECT': 'de', 'ET_MESH_PLAYER_UI_MAJOR': 'mesh.ui.major',
         'ET_MESH_PLAYER_UI_MINOR': 'mesh.ui.minor', 'ET_MESH_PLAYER_GAME_MAJOR': 'mesh.game.major',
         'ET_MESH_PLAYER_GAME_MINOR': 'mesh.game.minor', 'ET_PARTICLE_UI_MAJOR': 'part.ui.major',
         'ET_PARTICLE_UI_MINOR': 'part.ui.minor', 'ET_PARTICLE_GAME_MAJOR': 'part.game.major',
         'ET_PARTICLE_GAME_MINOR': 'part.game.minor'}
KEY_NS = {'MAIN_EFFECT_NAME': 'mesh.game.major', 'MAIN_EFFECT_NAME_ULTIMATE': 'mesh.game.major',
          'PARTICLE_NAME': 'part.game.major',
          'DAMAGE_EFFECT_NAME': 'de', 'CREATE_DAMAGE_EFFECT_NAME': 'de',
          'CREATE_EFFECT_SET_NAME': 'es', 'EFFECT_SET_NAME': 'es',
          'FILE_NAME': 'file'}
NONREF = {'BONE_NAME', 'WEAPON_BONE_NAME', 'RIDING_PET_BONE_NAME', 'POSITION_BONE_NAME', 'TRACE_OWNER_BONE',
          'TRACE_OWNER_BONE_ONLY_ROTATE', 'LOCK_ON_TARGET_BONE_NAME', 'MOTION_NAME', 'DESC',
          'DAMAGE_KEY_NAME', 'DAMAGE_DATA_KEY_NAME', 'VERSION',
          'PARTICLE_DNAME'}   # typo: no read site in the June binary (xorstrings.tsv); its particle never plays
FILE_EXT = re.compile(r'\.(x|y|dds|tga|bmp|png|jpg|jpeg|xet|tet|ogg|wav|mp3)$', re.I)
KIND_BY_EXT = {'.x': 'mesh', '.y': 'mesh', '.xet': 'xet', '.tet': 'tet', '.ogg': 'sound', '.wav': 'sound',
               '.mp3': 'sound'}


def ext_kind(name):
    e = os.path.splitext(name)[1].lower()
    return KIND_BY_EXT.get(e, 'tex' if e in F.TEX_EXT else 'other')


def norm_key(k):
    return re.sub(r'\d+$', '', k)


def lua_edges(root_kind, tab, et_names, out, uncl, path=''):
    """Walk a captured Lua table; append (path, key, ns, value) edges to `out`."""
    t = tab.get('$t', tab)
    etype = t.get('EFFECT_TYPE')
    for k, v in t.items():
        if isinstance(v, dict):
            if '$t' in v:
                lua_edges(root_kind, v, et_names, out, uncl, path + '/' + k)
            continue
        if not isinstance(v, str):
            continue
        key = norm_key(k)
        p = path + '/' + k
        if key == 'NAME' and path == '':
            continue   # the templet's own name
        if root_kind == 'de' and key == 'NAME':
            # A particle sub-table's NAME: June's LoadParticleTable (31f66e60) reads
            # PARTICLE_NAME and never NAME (its decrypted keys, xorstrings.tsv), so
            # June ignores it too. A studio typo, like PARTICLE_DNAME.
            uncl.append((p, key, v, 'june-ignores: 31f66e60 reads PARTICLE_NAME, not NAME'))
            continue
        if root_kind == 'es' and key in ('NAME', 'HYPER_NAME') and path != '':
            ns = ET_NS.get(et_names.get(etype, ''), None) if isinstance(etype, (int, float)) else None
            out.append((p, key, ns or 'es-entry-type:%r' % (et_names.get(etype, etype),), v))
        elif key in KEY_NS:
            out.append((p, key, KEY_NS[key], v))
        elif key.startswith('HTD_'):
            out.append((p, key, 'file', v))
        elif key in NONREF:
            continue
        elif FILE_EXT.search(v):
            out.append((p, key, 'file', v))
            uncl.append((p, key, v, 'file by extension'))
        else:
            uncl.append((p, key, v, ''))


class Closure(object):
    def __init__(self):
        self.jun = F.Build('june', F.JUNE_DATA, F.JUNE_DATA)
        self.mar = F.Build('march', F.MAR_DATA, F.MAR_STAGE)
        self.upd = F.update_inventory()
        self.jfx = F.june_fx()
        self.jes, self.jde = F.fx_templets(self.jfx)
        self.jt = F.templets('june', self.jun)
        self.mt = F.templets('march', self.mar)
        et = self.jfx['globals']['EFFECT_TYPE']['value']['$t']
        self.et_names = dict((v, k) for k, v in et.items())
        self.uncl = []            # (root, path, key, value, note)
        self.dangling = []        # (referrer, key, ns, name, hint)
        self.templet_seen = {}    # (ns, name) -> resolved?
        self.files = collections.OrderedDict()   # lower name -> {'name', 'kind', 'refs': [referrer]}
        self.xerrors = []

    # ---------------------------------------------------------------- resolution
    def resolve(self, ns, name, referrer, key):
        """Queue a named reference. Returns the node key or None if dangling."""
        if ns == 'file':
            self.add_file(name, referrer)
            return
        if ns.startswith('es-entry-type'):
            self.dangling.append((referrer, key, ns, name, 'EFFECT_TYPE unset or unknown: nothing is created'))
            return
        found = self.lookup(ns, name)
        if found:
            if (found, name) not in self.templet_seen:
                self.templet_seen[(found, name)] = True
                self.queue.append((found, name))
            return
        hint = self.hint(ns, name)
        self.dangling.append((referrer, key, ns, name, hint))

    def lookup(self, ns, name):
        if ns == 'es':
            return 'es' if name in self.jes else None
        if ns == 'de':
            return 'de' if name in self.jde else None
        return ns if name in self.jt[ns] else None

    def hint(self, ns, name):
        low = name.lower()
        other = [n for n in self.jt if n != ns and name in self.jt[n]]
        if name in self.jes and ns != 'es':
            other.append('es')
        if name in self.jde and ns != 'de':
            other.append('de')
        pools = [('es', self.jes), ('de', self.jde)] + [(n, self.jt[n]) for n in self.jt]
        case = sorted('%s:%s' % (n, k) for n, d in pools for k in d if k.lower() == low and k != name)
        bits = []
        if other:
            bits.append('defined in ' + ', '.join(sorted(other)) + ' instead')
        if case:
            bits.append('case-only match ' + ', '.join(case[:3]))
        if self.mar_has(ns, name):
            bits.append('March defines it')
        return '; '.join(bits) or 'defined nowhere in June'

    def mar_has(self, ns, name):
        if ns in self.mt:
            return name in self.mt[ns]
        return False

    def add_file(self, name, referrer):
        base = re.split(r'[\\/]', name)[-1]
        low = base.lower()
        f = self.files.get(low)
        if f is None:
            f = self.files[low] = {'name': base, 'kind': ext_kind(base), 'refs': []}
            if f['kind'] in ('mesh', 'xet'):
                self.queue.append(('file', low))
        f['refs'].append(referrer)

    # ---------------------------------------------------------------- walk
    def run(self):
        self.queue = collections.deque()
        roots = []
        for name, defs in self.jes.items():
            for ns, fname, tab in defs:
                roots.append(('es', name, fname, tab))
        for name, (fname, tab, _) in self.jde.items():
            roots.append(('de', name, fname, tab))
        for c in self.jfx['calls']:
            if c['method'] == 'InsertDamageData_LUA' and c['args']:
                roots.append(('dd', c['args'][0]['$t'].get('DAMAGE_KEY_NAME', '?'), c['file'], c['args'][0]))
        for name, g in self.jfx['globals'].items():
            if g['file'] == 'DamageSound.lua' and isinstance(g['value'], dict):
                roots.append(('ds', name, g['file'], g['value']))
        self.roots = roots
        for kind, name, fname, tab in roots:
            edges = []
            uncl = []
            lua_edges('es' if kind == 'es' else kind, tab, self.et_names, edges, uncl)
            ref = '%s:%s' % (kind, name)
            for p, key, v, note in uncl:
                self.uncl.append((ref, p, key, v, note))
            for p, key, ns, v in edges:
                if v == '':
                    continue   # empty name: the engine skips it
                self.resolve(ns, v, ref, key)
            if kind in ('es', 'de'):
                self.templet_seen[(kind, name)] = True
        while self.queue:
            ns, name = self.queue.popleft()
            if ns in ('es', 'de'):
                continue   # every effect set / damage effect is already a root
            if ns == 'file':
                self.expand_file(name)
                continue
            for t in self.jt[ns][name]:
                ref = '%s:%s' % (ns, name)
                for key, kind, v in t.refs:
                    if kind == 'nested':
                        self.resolve(ns, v, ref, key)
                    else:
                        self.add_file(v, ref)
                for key, v in t.unclassified:
                    self.uncl.append((ref, '', key, v, 'templet'))

    def expand_file(self, low):
        c = self.best_june(low)
        if c is None:
            return
        path, exact = (c.path, True) if c.path else (c.xpath, False)
        try:
            # Names only: a short or broken block is read as far as it goes (lenient),
            # whether June's copy is exact or elstools-truncated.
            names, fmt = F.embedded_textures(path, lenient=True)
        except Exception as e:   # noqa: BLE001 - recorded, and the leaf still gets a verdict
            self.xerrors.append((low, str(e)))
            return
        self.files[low]['fmt'] = fmt
        for n in names:
            self.add_file(n, 'file:' + self.files[low]['name'])

    def best_june(self, low):
        cs = self.jun.inv.get(low, [])
        for c in cs:
            if c.path:
                return c
        for c in cs:
            if c.xpath:
                return c
        return None

    # ---------------------------------------------------------------- verdicts
    def compare(self, jc, mc):
        """June copy vs a March copy with readable content. -> (verdict, method)"""
        if mc.path is None:
            return ('unverified', 'March copy not in kom_contents at manifest size')
        if jc.size != mc.size:
            if jc.xpath and 0 < jc.size - mc.size <= 255 and not jc.path:
                a = open(jc.xpath, 'rb').read()
                b = open(mc.path, 'rb').read()
                if a == b:
                    return ('truncated-in-march', 'March copy == elstools-truncated June extraction')
            return ('differs', 'size %d vs %d' % (jc.size, mc.size))
        if jc.path:
            return (('same', 'sha1') if F.sha1(jc.path) == F.sha1(mc.path) else ('differs', 'sha1'))
        if jc.xpath:
            a = open(jc.xpath, 'rb').read()
            b = open(mc.path, 'rb').read(len(a))
            return (('same-prefix', 'June tail unreadable (%d of %d bytes)' % (len(a), jc.size)) if a == b
                    else ('differs', 'prefix'))
        return ('unverified', 'June copy not extracted')

    def verdicts(self):
        tets = set()
        for inv in (self.jun.inv, self.mar.inv):
            for low in inv:
                if low.endswith('.tet'):
                    tets.add(low[:-4])
        mar_users = self.march_users()
        rows = []
        for low, f in self.files.items():
            jcs = self.jun.inv.get(low, [])
            mcs = self.mar.inv.get(low, [])
            stg = self.mar.staged.get(low, [])
            upd = self.upd.get(low, [])
            r = dict(f)
            r['low'] = low
            r['june'] = ' '.join('%s/%d%s' % (c.kom, c.size, '' if c.path else
                                              ('/short%d' % (c.size - os.path.getsize(c.xpath)) if c.xpath else '/noext'))
                                 for c in jcs)
            r['march'] = ' '.join('%s/%d' % (c.kom, c.size) for c in mcs)
            r['staged'] = ' '.join('%s/%d' % (k, os.path.getsize(p)) for k, _, p in stg)
            r['update'] = ' '.join('%s/%d' % (d, os.path.getsize(p)) for d, _, p in upd)
            notes = []
            if len(jcs) > 1:
                hs = set()
                for c in jcs:
                    hs.add(F.sha1(c.path) if c.path else ('size', c.size))
                notes.append('June has %d copies (%s)' % (len(jcs), 'identical' if len(hs) == 1 else 'DIFFERENT'))
            if len(mcs) > 1:
                notes.append('March has %d copies' % len(mcs))
            jbest = self.best_june(low) or (jcs[0] if jcs else None)
            if mcs:
                loc = 'march-live'
                verdict, method = self.compare(jbest, mcs[0]) if jbest else ('march-only', 'not in June')
            elif upd:
                loc, verdict, method = 'update-only', 'pending-pack', ''
            elif stg:
                loc, verdict, method = 'staged-only', 'pending-pack', 'in kom_contents, not in a live .kom'
            elif jcs:
                loc, verdict = 'june-only', 'missing'
                method = 'exact copy available' if any(c.path for c in jcs) else \
                    'extracted copy is short (elstools algo-2 tail loss)'
            else:
                loc, verdict, method = 'nowhere', 'nowhere', ''
            for d, _, p in upd:
                if jbest and os.path.getsize(p) != jbest.size and 0 < jbest.size - os.path.getsize(p) <= 255:
                    notes.append('update/%s copy is %d bytes short of June' % (d, jbest.size - os.path.getsize(p)))
            r['loc'], r['verdict'], r['method'] = loc, verdict, method
            stem = os.path.splitext(low)[0]
            r['atlas'] = 'atlas' if f['kind'] == 'tex' and stem in tets else ''
            r['mar_users'] = mar_users.get(low, 0)
            r['notes'] = '; '.join(notes)
            rows.append(r)
        return rows

    def march_users(self):
        """How many March templets (any namespace) reference each file directly, plus
        March meshes embedding it: the blast radius of replacing March's copy."""
        users = collections.Counter()
        meshes = set()
        for ns, d in self.mt.items():
            for name, ts in d.items():
                t = ts[-1]
                for key, kind, v in t.refs:
                    if kind != 'nested':
                        low = re.split(r'[\\/]', v)[-1].lower()
                        users[low] += 1
                        if kind in ('mesh', 'xet'):
                            meshes.add(low)
        for low in meshes:
            p = self.mar.file(low)
            if not p:
                continue
            try:
                names, _ = F.embedded_textures(p)
            except Exception:   # noqa: BLE001
                continue
            for n in names:
                users[n.lower()] += 1
        return users


def dds_expected_size(d):
    """File size a DDS's header implies (128-byte header + every mip of every face)."""
    import struct
    if d[:4] != b'DDS ' or len(d) < 128:
        return None
    h, w = struct.unpack('<II', d[12:20])
    mips = struct.unpack('<I', d[28:32])[0] or 1
    pf_flags, fourcc, bpp = struct.unpack('<I', d[80:84])[0], d[84:88], struct.unpack('<I', d[88:92])[0]
    faces = 6 if struct.unpack('<I', d[112:116])[0] & 0x200 else 1
    blk = {b'DXT1': 8, b'DXT2': 16, b'DXT3': 16, b'DXT4': 16, b'DXT5': 16}.get(fourcc) if pf_flags & 4 else None
    if pf_flags & 4 and blk is None:
        return None   # a FourCC this check does not size
    tot = 0
    for _ in range(mips):
        tot += (max(1, (w + 3) // 4) * max(1, (h + 3) // 4) * blk) if blk else w * h * bpp // 8
        w, h = max(1, w // 2), max(1, h // 2)
    return 128 + tot * faces


def update_truncation(cl):
    """Every march2014/update/ file (closure or not) that is 1-255 bytes short of June's
    manifest size, with the file's own internal evidence and its live March copy."""
    rows = []
    for low, entries in sorted(cl.upd.items()):
        sizes = sorted(set(c.size for c in cl.jun.inv.get(low, [])))
        for d, n, p in entries:
            data = open(p, 'rb').read()
            short = [s for s in sizes if 0 < s - len(data) <= 255]
            if not short or len(data) in sizes:
                continue
            e = dds_expected_size(data)
            if e is not None:
                ev = 'DDS header needs %d, file has %d' % (e, len(data))
            elif data[:4] == b'xof ' and data[8:12] in (b'bzip', b'tzip'):
                try:
                    F.xfile_text(data)
                    ev = 'MSZIP inflates fully (the lost bytes follow the stream)'
                except Exception as ex:   # noqa: BLE001
                    ev = 'MSZIP BROKEN: %s' % str(ex)[:70]
            else:
                ev = 'no internal size check for this format'
            live = cl.mar.inv.get(low, [])
            rows.append({'dir': d, 'name': n, 'size': len(data), 'june': short[0], 'evidence': ev,
                         'live': ' '.join('%s/%d%s' % (c.kom, c.size, ' (TRUNCATED)' if c.size == len(data) else '')
                                          for c in live) or '-'})
    return rows


# ------------------------------------------------------------------------ report
def md_table(rows, cols, heads):
    out = ['| ' + ' | '.join(heads) + ' |', '|' + '---|' * len(heads)]
    for r in rows:
        out.append('| ' + ' | '.join(str(r.get(c, '')).replace('|', '\\|') for c in cols) + ' |')
    return out


def main():
    cl = Closure()
    cl.run()
    rows = cl.verdicts()
    F.save_hashes()

    cols = ['kind', 'name', 'loc', 'verdict', 'method', 'atlas', 'mar_users', 'june', 'march', 'staged',
            'update', 'fmt', 'nrefs', 'first_ref', 'notes']
    with open(OUT_TSV, 'w', encoding='utf-8', newline='\n') as fh:
        fh.write('\t'.join(cols) + '\n')
        for r in sorted(rows, key=lambda r: (r['verdict'], r['kind'], r['low'])):
            r['nrefs'] = len(r['refs'])
            r['first_ref'] = r['refs'][0]
            fh.write('\t'.join(str(r.get(c, '')) for c in cols) + '\n')

    by = collections.Counter((r['loc'], r['verdict']) for r in rows)
    kinds = collections.Counter(r['kind'] for r in rows)
    print('roots %d, templets reached %d, files %d, dangling %d, unclassified %d, file errors %d' % (
        len(cl.roots), len(cl.templet_seen), len(rows), len(cl.dangling), len(cl.uncl), len(cl.xerrors)))
    for k, n in sorted(by.items()):
        print('  %-12s %-20s %d' % (k[0], k[1], n))
    write_md(cl, rows, by, kinds)


def write_md(cl, rows, by, kinds):
    L = []
    w = L.append
    w('# June2017 effect data: asset closure (Phase 10b, audit 1)')
    w('')
    w('Generated by `june2017_re/assets.py`; the full per-node table is `june2017_re/asset_closure.tsv`.')
    w('Plan: `JUNE2017_EFFECT_REBUILD_PLAN.md`, Phase 10b. **Nothing here was copied or packed** - this is')
    w('the list for the user, who packs.')
    w('')
    w('## What was walked')
    w('')
    w('- Roots: %d. Every effect set and damage effect June\'s 14 effect lua files define, found by' % len(cl.roots))
    w('  executing the converted bytecode (route (b), see Phase 10b in the plan), so the code-built')
    w('  `Get*` templets are included. Plus `DamageSound.lua` and `Elsword_DamageData.lua`.')
    w('- Templets reached: %d effect sets / damage effects / particle / mesh-player templets.' % len(cl.templet_seen))
    w('- File nodes: %d (%s).' % (len(rows), ', '.join('%s %d' % kv for kv in sorted(kinds.items()))))
    w('- Meshes (`.x`/`.y`: DirectX `xof` txt/bin/bzip, and KOG `KSM`/`KME`) and `.xet` files were opened')
    w('  and the texture names they embed followed. %d file(s) could not be parsed (listed below).' % len(cl.xerrors))
    unexpl = [u for u in cl.uncl if not u[4].startswith('june-ignores')]
    w('- Completeness gate: %d string value(s) met on the way were neither an edge nor a known' % len(unexpl))
    w('  non-reference; %d more are keys June itself ignores (both listed below).' % (len(cl.uncl) - len(unexpl)))
    w('')
    w('## Verdicts')
    w('')
    w('| where | verdict | files |')
    w('|---|---|---|')
    for (loc, v), n in sorted(by.items()):
        w('| %s | %s | %d |' % (loc, v, n))
    w('')
    w('Meaning:')
    w('')
    w('- **march-live / same**: March\'s live copy is byte-identical to June\'s (SHA-1). Nothing to do.')
    w('- **march-live / same-prefix**: identical as far as June\'s copy can be read. June\'s extraction is')
    w('  truncated by elstools (see *Extractor defect* below), so the last 1-255 bytes are unverified.')
    w('- **march-live / differs**: March ships a different file under the same name. **Dangerous bucket**:')
    w('  swapping in June\'s copy changes every March user of it (`March effect-templet users` is a lower')
    w('  bound, see below). Atlases (a `.tet`')
    w('  partner exists) are flagged and listed first. Flagged, not resolved.')
    w('- **march-live / truncated-in-march**: March\'s live copy equals the elstools-truncated June')
    w('  extraction, i.e. it is a broken port of June\'s file.')
    w('- **update-only / pending-pack**: not in a live archive, but staged in `march2014/update/`.')
    w('- **staged-only / pending-pack**: only in `kom_contents` (not in any live `.kom`).')
    w('- **june-only / missing**: March has no copy anywhere. These are what adoption needs packed.')
    w('- **nowhere**: referenced by June\'s data but present in neither build: June\'s own dangling asset.')
    w('- **march-live / march-only**: March has it, June\'s archives do not (June\'s data still names it).')
    w('')

    def section(title, sel, note=None, limit=None):
        rs = [r for r in rows if sel(r)]
        w('## %s (%d)' % (title, len(rs)))
        w('')
        if note:
            w(note)
            w('')
        if not rs:
            w('None.')
            w('')
            return
        rs.sort(key=lambda r: (r['atlas'] == '', r['kind'], r['low']))
        shown = rs if limit is None else rs[:limit]
        for line in md_table(shown, ['kind', 'name', 'atlas', 'mar_users', 'method', 'june', 'march', 'update',
                                     'notes'],
                             ['kind', 'file', 'atlas', 'March effect-templet users', 'evidence', 'June copies',
                              'March live',
                              'update/', 'notes']):
            w(line)
        if limit is not None and len(rs) > limit:
            w('')
            w('... %d more in `asset_closure.tsv`.' % (len(rs) - limit))
        w('')

    natlas = sum(1 for r in rows if r['atlas'])
    ntet = sum(1 for low in cl.mar.inv if low.endswith('.tet'))
    section('Differs: March ships a different file under the same name', lambda r: r['verdict'] == 'differs',
            'Atlases first. `June copies` marks `/shortN` where elstools extracted June\'s file N bytes short. '
            '**Shared atlases: %d** of the closure\'s textures have a `.tet` partner in either build (March '
            'ships %d `.tet` files)%s. `March effect-templet users` counts March particle / mesh-player '
            'templets naming the file, plus the meshes those templets use that embed it. It is a **lower '
            'bound** on the blast radius, not the whole of it: character, map and UI meshes that embed the '
            'same texture, and non-effect scripts, are not counted, so 0 does not mean safe to swap.' % (
                natlas, ntet, '; the effect data references none of their textures, so no swap in this list '
                'moves an atlas swatch' if natlas == 0 else '; atlas rows are listed first and must be merged, '
                'not swapped (see RANOX_PORT_PLAN.md, DLG_UI_Button17 / Texture71)'))
    section('Truncated in March: live copy is a broken port of June\'s file',
            lambda r: r['verdict'] == 'truncated-in-march')
    section('June-only: needed for adoption, March has no copy', lambda r: r['verdict'] == 'missing',
            'The user packs these (lua as algo 2, everything else algo 0, as the March client reads them). '
            'Rows whose evidence reads *extracted copy is short* have no byte-exact extraction: the copy lacks '
            'its last N bytes (`/shortN`). The user reports such copies work in the March client, and the lost '
            'tail is usually not content (see *Extractor defect*), but they are not byte-identical to June.')
    section('Pending pack: staged but not in a live archive', lambda r: r['verdict'] == 'pending-pack')
    section('Nowhere: referenced by June, present in neither build', lambda r: r['verdict'] == 'nowhere')
    section('Unverified', lambda r: r['verdict'] == 'unverified')

    w('## Extractor defect: elstools truncates June algo-2 entries')
    w('')
    w('For June2017\'s archives (`KOG GC TEAM MASSFILE V.0.4.`), `elstools extract` writes about a quarter')
    w('of the algo-2 entries 1-255 bytes **short** of the manifest size. It does this with every `-v`')
    w('option, and the pre-existing `14JUN17\\data\\*_kom` dirs have the same defect. Proof that it is lost')
    w('data and not padding: a truncated DDS is short of the size its own header\'s dimensions and mip')
    w('count require (e.g. `CHUNG_GIGANTIC_IMPACT_Bottom_Broken_SP.dds` 21996 vs 22000), and a truncated')
    w('bzip `.x`/`.y`\'s last MSZIP block does not inflate to its declared size. Padding with NULs does')
    w('not repair it: the stream then overshoots.')
    w('')
    w('**How much it matters.** Usually little. The user reports that extracted June files work in the')
    w('March client when packed with the right algorithm (lua algo 2, everything else algo 0). What is')
    w('lost is the tail: for a DDS, the smallest mip levels; for a binary `.x`/`.y`, in the samples read,')
    w('mostly the closing-brace tokens (`0b 00`) after the last texture name, i.e. no mesh data. Not always:')
    w('`FireImpactG04.Y` loses the end of a `VertexDuplicationIndices` list and `Spa_Ranox_G01_water01.y`')
    w('210 bytes of a data run. So a short copy is usable, but it is not June\'s file.')
    w('')
    ut = update_truncation(cl)
    nlive = sum(1 for r in ut if 'TRUNCATED' in r['live'])
    w('Files ported from those extractions carry the same short tail. Over all of `march2014/update/` (not')
    w('only this closure), %d files are short of June\'s manifest size; %d of them are live at the' % (len(ut), nlive))
    w('truncated size. Nothing was changed. The file\'s own header is the evidence where the format has one:')
    w('')
    for line in md_table(ut, ['dir', 'name', 'size', 'june', 'evidence', 'live'],
                         ['update/', 'file', 'bytes', 'June manifest', 'internal evidence', 'March live']):
        w(line)
    w('')
    w('## Dangling name references in June\'s own data (%d)' % len(cl.dangling))
    w('')
    w('A name June\'s effect data asks for that June itself does not define under that namespace. These')
    w('are not adoption regressions; they do nothing in June either. Grouped by hint:')
    w('')
    dg = collections.Counter((ns, hint) for _, _, ns, _, hint in cl.dangling)
    w('| namespace | hint | refs |')
    w('|---|---|---|')
    for (ns, hint), n in dg.most_common():
        w('| %s | %s | %d |' % (ns, hint.replace('|', '\\|')[:120], n))
    w('')
    w('<details><summary>All dangling references</summary>')
    w('')
    for ref, key, ns, name, hint in sorted(cl.dangling):
        w('- `%s` %s -> %s `%s`: %s' % (ref, key, ns, name, hint))
    w('')
    w('</details>')
    w('')
    w('## Completeness gate: unclassified string values (%d)' % len(cl.uncl))
    w('')
    if cl.uncl:
        uc = collections.Counter((key, note) for _, _, key, _, note in cl.uncl)
        w('| key | note | count | example |')
        w('|---|---|---|---|')
        ex = {}
        for ref, p, key, v, note in cl.uncl:
            ex.setdefault((key, note), '%s %s = %r' % (ref, p, v))
        for (key, note), n in uc.most_common():
            w('| %s | %s | %d | %s |' % (key, note, n, ex[(key, note)].replace('|', '\\|')[:100]))
    else:
        w('None: every string value was an edge or a known non-reference.')
    w('')
    w('## Files that could not be parsed (%d)' % len(cl.xerrors))
    w('')
    for low, e in cl.xerrors:
        w('- `%s`: %s' % (low, e))
    if not cl.xerrors:
        w('None.')
    w('')
    open(OUT_MD, 'w', encoding='utf-8', newline='\n').write('\n'.join(L))


if __name__ == '__main__':
    main()
