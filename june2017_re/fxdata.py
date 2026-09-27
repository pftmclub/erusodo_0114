"""Shared data layer for the Phase 10b audits (assets.py, names.py).

Everything here reads; nothing writes outside assets_cache/ (gitignored).

- Inventories of both builds' .kom archives, from `elstools inspect` manifests
  (the authority for what is packed), with file content taken from the
  extracted *_kom directories only where the extracted file's size equals the
  manifest's. June: 14JUN17/data/*_kom. March: the user's staging copy,
  24965799/kom_contents/*_kom (see README "Phase 10b"). march2014/update/ is
  inventoried separately (files ported but not yet packed).
- Effect lua, both builds, executed with fxdump.lua: June's 14 files through
  ljconv.py first; March's live files as they are (already 0x81).
- Templet text files (*Particle*.txt, *XMeshPlayer*.txt) in each build's own
  loader order, parsed into templets with classified string references.
"""
import collections
import glob
import hashlib
import json
import os
import re
import subprocess
import sys
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..'))
sys.path.insert(0, HERE)

JUNE_DATA = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data'
MAR_DATA = r'F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data'
MAR_STAGE = r'F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents'
UPDATE = os.path.join(TRUNK, 'march2014', 'update')
CACHE = os.path.join(HERE, 'assets_cache')
ELSTOOLS = os.path.join(TRUNK, 'elstools', 'elstools.exe')
LUAJIT = os.path.join(TRUNK, 'elstools', 'vendor', 'luajit', 'luajit.exe')
KOM_RE = re.compile(r'^data(i?)(\d+)\.kom$', re.I)   # the archives the client mounts


# ----------------------------------------------------------------- inventories
class Copy(object):
    """One archive entry. `path` is set only when an extracted file exists whose
    size equals the manifest size. `xpath` is any extracted file of that name:
    for June V.0.4 algo-2 entries elstools 1.0.0 writes 1-255 bytes short of the
    manifest size (proven from DDS headers and MSZIP streams, see README), so
    such a copy is usable for a prefix comparison and for reading names, never
    as the file itself."""
    __slots__ = ('kom', 'name', 'size', 'path', 'xpath', 'algo')

    def __init__(self, kom, name, size, path, xpath, algo):
        self.kom, self.name, self.size, self.path, self.xpath, self.algo = kom, name, size, path, xpath, algo


def inspect(kom_path):
    """elstools inspect, cached by (size, mtime) of the .kom."""
    os.makedirs(os.path.join(CACHE, 'inspect'), exist_ok=True)
    st = os.stat(kom_path)
    tag = '%s_%d_%d.json' % (os.path.basename(os.path.dirname(os.path.dirname(kom_path))) + '_' +
                             os.path.basename(os.path.dirname(kom_path)) + '_' +
                             os.path.basename(kom_path), st.st_size, int(st.st_mtime))
    cp = os.path.join(CACHE, 'inspect', re.sub(r'[^\w.]', '_', tag))
    if not os.path.exists(cp):
        out = subprocess.run([ELSTOOLS, 'inspect', '-q', '-i', kom_path], capture_output=True,
                             check=True).stdout.decode('utf-8', 'replace')
        open(cp, 'w', encoding='utf-8').write(out[out.index('{'):])
    return json.load(open(cp, encoding='utf-8'))


def build_inventory(data_dir, extract_root):
    """{lower name: [Copy]} over every dataNNN.kom / dataiNNN.kom in data_dir,
    plus {lower name: [(kom, name, path)]} for extracted files no manifest lists
    (staged but not packed)."""
    inv = collections.defaultdict(list)
    staged = collections.defaultdict(list)
    for kom in sorted(os.listdir(data_dir)):
        if not KOM_RE.match(kom):
            continue
        stem = kom[:-4]
        man = inspect(os.path.join(data_dir, kom))
        xdir = os.path.join(extract_root, stem + '_kom')
        listing = {}
        if os.path.isdir(xdir):
            for f in os.listdir(xdir):
                listing[f.lower()] = f
        listed = set()
        for e in man['files']:
            low = e['name'].lower()
            listed.add(low)
            path = xpath = None
            if low in listing:
                p = os.path.join(xdir, listing[low])
                if os.path.isfile(p):
                    xpath = p
                    if os.path.getsize(p) == e['size']:
                        path = p
            inv[low].append(Copy(stem, e['name'], e['size'], path, xpath, e['algo']))
        for low, f in listing.items():
            if low not in listed and low != '_manifest.json' and os.path.isfile(os.path.join(xdir, f)):
                staged[low].append((stem, f, os.path.join(xdir, f)))
    return inv, staged


def update_inventory():
    """{lower name: [(dir, name, path)]} for march2014/update/<dir>/<file>."""
    inv = collections.defaultdict(list)
    for d in sorted(os.listdir(UPDATE)):
        full = os.path.join(UPDATE, d)
        if not os.path.isdir(full):
            continue
        for f in os.listdir(full):
            if f.lower() in ('assets.txt', '_manifest.json') or not os.path.isfile(os.path.join(full, f)):
                continue
            inv[f.lower()].append((d, f, os.path.join(full, f)))
    return inv


_HASHES = None


def sha1(path):
    """SHA-1 of a file, cached in assets_cache/hashes.json by (path, size, mtime)."""
    global _HASHES
    cp = os.path.join(CACHE, 'hashes.json')
    if _HASHES is None:
        _HASHES = json.load(open(cp)) if os.path.exists(cp) else {}
    st = os.stat(path)
    key = '%s|%d|%d' % (path, st.st_size, int(st.st_mtime))
    h = _HASHES.get(key)
    if h is None:
        hh = hashlib.sha1()
        with open(path, 'rb') as f:
            for blk in iter(lambda: f.read(1 << 20), b''):
                hh.update(blk)
        h = _HASHES[key] = hh.hexdigest()
    return h


def save_hashes():
    if _HASHES is not None:
        os.makedirs(CACHE, exist_ok=True)
        json.dump(_HASHES, open(os.path.join(CACHE, 'hashes.json'), 'w'))


class Build(object):
    def __init__(self, label, data_dir, extract_root):
        self.label = label
        self.inv, self.staged = build_inventory(data_dir, extract_root)

    def file(self, name):
        """Readable path for a packed file (first copy with verified content), or None."""
        for c in self.inv.get(name.lower(), ()):
            if c.path:
                return c.path
        return None


# ----------------------------------------------------------------- effect lua
JUNE_LUA_ORDER = ['Enum', 'DamageSound', 'Elsword_DamageData', 'UIEffectSet', 'UIEffectSet_INT',
                  'EffectSet', 'EffectSet_INT', 'EffectSet_Description', 'UnitViewerEffectSetTable',
                  # FUN_322e2930 opens these five in this order.
                  'DamageEffect', 'DamageEffect_Character', 'DamageEffect_Dungeon',
                  'DamageEffect_ETC', 'DamageEffect_INT']
# X2StateStartUp.cpp:164-172 (INT_RESOURCE_DEVIDE is defined under US_SERVICE),
# X2DamageManager.cpp:17, X2Game.cpp:1650-1652.
MAR_LUA_ORDER = ['Enum', 'DamageSound', 'UIEffectSet', 'EffectSet', 'UIEffectSet_INT', 'EffectSet_INT',
                 'EffectSet_Description', 'DamageEffect', 'DamageEffect_INT']


def run_fxdump(files, out_json):
    p = subprocess.run([LUAJIT, os.path.join(HERE, 'fxdump.lua'), out_json] + files,
                       cwd=os.path.dirname(LUAJIT), capture_output=True)
    if p.returncode != 0:
        raise SystemExit('fxdump failed:\n' + p.stderr.decode('latin1'))
    return json.load(open(out_json, encoding='latin1'))


def june_fx():
    import ljconv
    outdir = os.path.join(CACHE, 'june_b')
    os.makedirs(outdir, exist_ok=True)
    src = [os.path.join(HERE, 'june_lua', n + '.lua') for n in JUNE_LUA_ORDER]
    table, _, _ = ljconv.build_table(src)
    ljconv.check_modes(table)
    files = []
    for f in src:
        out, _, _ = ljconv.transcode(open(f, 'rb').read(), table)
        files.append(os.path.join(outdir, os.path.basename(f)))
        open(files[-1], 'wb').write(out)
    return run_fxdump(files, os.path.join(CACHE, 'june_fx.json'))


def march_fx(mar):
    files = []
    for n in MAR_LUA_ORDER:
        p = mar.file(n + '.lua')
        if p is None:
            raise SystemExit('March %s.lua: no extracted copy matching the live manifest' % n)
        files.append(p)
    return run_fxdump(files, os.path.join(CACHE, 'march_fx.json'))


def fx_templets(fx):
    """(effect sets, damage effects) from a fxdump result, keyed by EXACT name:
    CX2EffectSet keeps templets in a std::map<std::wstring> (X2EffectSet.h:856-858)
    and damage effects are Lua globals, so both lookups are case-sensitive.
    effect sets: {name: [(ns, file, table)]} in call order, ns 'es.game' / 'es.ui'.
    damage effects: {name: (file, table, redefined)} for every table global first
    defined by a DamageEffect*.lua file (final value, as the client reads it)."""
    es = collections.defaultdict(list)
    for c in fx['calls']:
        if c['method'] == 'AddEffectSetTemplet_LUA' and c['args'] and isinstance(c['args'][0], dict):
            t = c['args'][0].get('$t', {})
            name = t.get('NAME')
            if isinstance(name, str):
                ns = 'es.ui' if c['file'].lower().startswith('uieffectset') else 'es.game'
                es[name].append((ns, c['file'], t))
    de = {}
    for name, g in fx['globals'].items():
        if g['file'].lower().startswith('damageeffect') and isinstance(g['value'], dict) and '$t' in g['value']:
            de[name] = (g['file'], g['value']['$t'], g['redefined'])
    return es, de


# ----------------------------------------------------------------- templet .txt files
NS_FILES = {
    # (June loader FUN_32597410 / March X2StateStartUp.cpp:125-172), in load order.
    'part.ui.major': ['UIMajorParticle.txt', 'UIMajorParticle_INT.txt'],
    'part.ui.minor': ['UIMinorParticle.txt'],
    'part.game.major': ['GameMajorParticle.txt', '<GameMajorParticle%d.txt>', 'GameMajorParticle_INT.txt'],
    'part.game.minor': ['GameMinorParticle.txt', 'GameMinorParticle_INT.txt'],
    'mesh.ui.major': ['UIMajorXMeshPlayer.txt', 'UIMajorXMeshPlayer_INT.txt'],
    'mesh.ui.minor': ['UIMinorXMeshPlayer.txt'],
    'mesh.game.major': ['GameMajorXMeshPlayer.txt', 'GameMajorXMeshPlayer_INT.txt'],
    'mesh.game.minor': ['GameMinorXMeshPlayer.txt'],
}
# June: loop from 2 while the file exists (FUN_32597410 @32597677 ESI=2, exit at the
# JZ @32597d5d). March: GameMajorParticle2..23 hard-coded (X2StateStartUp.cpp:130-150).
MAR_PARTICLE_MAX = 23

# Keys whose quoted value is a reference. Keys are matched upper-case: the studio mixes case.
TXT_REFS = {
    'part': {'TEXTURE': 'tex', 'MESH': 'mesh', 'CHANGETEXXET': 'xet', 'MULTITEXXET': 'xet', 'ANIXET': 'xet',
             'TRACEPARTICLELIST': 'nested', 'FINALPARTICLELIST': 'nested'},
    'mesh': {'MESHNAME': 'mesh', 'SLASH_TRACE_TEXTURE': 'tex', 'CHANGETEXXET': 'xet', 'MULTITEXXET': 'xet',
             'ANIXET': 'xet', 'ANIPACKAGE': 'xet'},
}
TXT_NONREFS = {'ANINAME'}   # an animation inside the mesh, not a file
TXT_HEADS = {'part': ('PARTICLESYSTEM', 'EVENTSEQUENCE'), 'mesh': ('XMESHPLAYER', 'TEMPLET')}
TOK = re.compile(r'"([^"\r\n]*)"|([A-Za-z_][A-Za-z0-9_]*)|(-?[0-9.]+)|([{}])|(\S)')


class Templet(object):
    __slots__ = ('ns', 'name', 'file', 'refs', 'unclassified', 'body')

    def __init__(self, ns, name, file):
        self.ns, self.name, self.file = ns, name, file
        self.refs = []           # (key, kind, value)
        self.unclassified = []   # (key, value)
        self.body = []           # token stream, for content comparison


def parse_txt(path, ns):
    kind = ns.split('.')[0]
    head, templ_kw = TXT_HEADS[kind]
    text = open(path, 'rb').read().decode('latin1')
    out = []
    depth = 0
    last_ident = None
    cur = None
    pending_name = None
    for m in TOK.finditer(text):
        s, ident, num, brace, other = m.groups()
        if brace == '{':
            depth += 1
            if depth == 2 and pending_name is not None:
                cur = Templet(ns, pending_name, os.path.basename(path))
                pending_name = None
            last_ident = None
            continue
        if brace == '}':
            if depth == 2 and cur is not None:
                out.append(cur)
                cur = None
            depth -= 1
            last_ident = None
            continue
        if depth == 1:
            if ident is not None:
                last_ident = ident.upper()
            elif s is not None and last_ident == templ_kw:
                pending_name = s
            continue
        if cur is None:
            continue
        cur.body.append(m.group(0))
        if ident is not None:
            last_ident = ident.upper()
        elif s is not None:
            key = last_ident or ''
            k = TXT_REFS[kind].get(key)
            if k:
                cur.refs.append((key, k, s))
            elif key not in TXT_NONREFS:
                cur.unclassified.append((key, s))
    return out


def ns_files(build_label, build, ns):
    files = []
    for f in NS_FILES[ns]:
        if f == '<GameMajorParticle%d.txt>':
            i = 2
            while True:
                if build_label == 'march' and i > MAR_PARTICLE_MAX:
                    break
                n = 'GameMajorParticle%d.txt' % i
                if build.file(n) is None:
                    if build.inv.get(n.lower()):
                        raise SystemExit('%s %s: packed but no verified extracted copy' % (build_label, n))
                    break
                files.append(n)
                i += 1
        else:
            files.append(f)
    return files


def templets(build_label, build):
    """{ns: {name: [Templet, ...]}} keyed by EXACT name, every definition in load order.
    Both engines keep templets in a std::map<wstring> (case-sensitive) and a later
    definition replaces an earlier one: KTDGXMeshPlayer.cpp:769-778,
    KTDGParticleSystem.cpp:1318-1332. So the LAST entry of each list is the live one."""
    res = {}
    for ns in NS_FILES:
        d = collections.defaultdict(list)
        for f in ns_files(build_label, build, ns):
            p = build.file(f)
            if p is None:
                if build.inv.get(f.lower()):
                    raise SystemExit('%s %s: packed but no verified extracted copy' % (build_label, f))
                continue   # not shipped by this build (e.g. an _INT file)
            for t in parse_txt(p, ns):
                d[t.name].append(t)
        res[ns] = d
    return res


# ----------------------------------------------------------------- file contents
TEX_EXT = ('.dds', '.tga', '.bmp', '.png', '.jpg', '.jpeg')
NAMECH = r'[A-Za-z0-9_\-.()~+\\/ ]'
TEXNAME = re.compile((NAMECH + r'{1,200}?\.(?:dds|tga|bmp|png|jpg|jpeg)(?![A-Za-z0-9])').encode(), re.I)
TEXNAME_U = re.compile(NAMECH + r'{1,200}?\.(?:dds|tga|bmp|png|jpg|jpeg)(?![A-Za-z0-9])', re.I)


def xfile_text(data, lenient=False):
    """DirectX .x body: 'xof 0303' + 'txt '/'bin '/'tzip'/'bzip' + '0032'. The zip
    forms are MSZIP: a DWORD total size, then blocks of (WORD raw size, WORD packed
    size incl. the 2-byte 'CK' signature, 'CK', raw deflate). Each block is a
    continuation of the previous one's 32 KB window."""
    if data[:4] != b'xof ':
        return None
    fmt = data[8:12]
    if fmt in (b'txt ', b'bin '):
        return data[16:]
    if fmt not in (b'tzip', b'bzip'):
        return None
    p = 20
    out = b''
    while p + 4 <= len(data):
        raw = int.from_bytes(data[p:p + 2], 'little')
        packed = int.from_bytes(data[p + 2:p + 4], 'little')
        blk = data[p + 4:p + 4 + packed]
        if blk[:2] != b'CK':
            raise ValueError('MSZIP block without CK at 0x%x' % p)
        d = zlib.decompressobj(-15, zdict=out[-32768:]) if out else zlib.decompressobj(-15)
        try:
            chunk = d.decompress(blk[2:]) + d.flush()
        except zlib.error:
            if not lenient:
                raise
            # A truncated copy: keep whatever inflates before the stream breaks.
            d = zlib.decompressobj(-15, zdict=out[-32768:]) if out else zlib.decompressobj(-15)
            chunk = b''
            try:
                for i in range(2, len(blk), 256):
                    chunk += d.decompress(blk[i:i + 256])
            except zlib.error:
                pass
            out += chunk
            break
        if len(chunk) > raw or (len(chunk) < raw and not lenient):
            raise ValueError('MSZIP block at 0x%x: %d bytes, header says %d' % (p, len(chunk), raw))
        out += chunk
        p += 4 + packed
    return out


def embedded_textures(path, lenient=False):
    """(texture base names a mesh or .xet embeds, original case; format tag).
    .xet: 'XET ' + UTF-16LE names. DirectX .x/.y ('xof 0303'): txt/bin read as is,
    tzip/bzip inflated first. KOG 'KSM '/'KME ' meshes, and anything else, are
    scanned raw: their texture names are plain single-byte strings. `lenient`
    accepts a short final MSZIP block, for reading names out of a truncated copy."""
    data = open(path, 'rb').read()
    names = []
    if data[:4] == b'XET ':
        fmt = 'xet'
        names += [m.group(0) for m in TEXNAME_U.finditer(data.decode('utf-16le', 'replace'))]
        body = data
    elif data[:4] == b'xof ':
        fmt = 'x-' + data[8:12].decode('latin1').strip()
        body = xfile_text(data, lenient)
    else:
        fmt = data[:4].decode('latin1').strip() or 'raw'
        body = data
    names += [m.group(0).decode('latin1') for m in TEXNAME.finditer(body)]
    seen = []
    for n in names:
        base = re.split(r'[\\/]', n)[-1].strip()
        if base and base not in seen:
            seen.append(base)
    return seen, fmt
