"""Atlas Phase A11 - asset closure, TET-key census, size check, mount order, pack list (ATLAS_PORT_PHASES.md, "Phase A11").

    python -I june2017_re/atlas_a11_closure.py            # writes atlas_a11_*.tsv next to this file, prints the summary

Read-only.  Reuses region_census (region file set + registry rows), fxdata.embedded_textures (mesh / XET texture names) and
junekom (June's strict decoder).  Region = both halves, as A6's closure was; `atlas` column marks names an Atlas line (AD AC TW
REST FLD UI) quotes.

Outputs
  atlas_a11_closure.tsv   one row per census-named or mesh-embedded asset: where March has it, where June has it, winning mount
  atlas_a11_tet.tsv       one row per (texture, key) pair the region Lua / rows use: TET found, key found, winning mount
  atlas_a11_sizes.tsv     every staged region file June also ships: staged size vs June's index size (short = elstools truncation)
  atlas_a11_packlist.tsv  archive -> staged region files that differ from the live kom (what a pack / copy still owes)
"""
import collections, csv, os, re, struct, sys, tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(os.path.dirname(HERE), 'tools'))
import region_census as RC
import fxdata
import junekom
import kom

ST = RC.ST
ATLAS_LINES = {'AD', 'AC', 'TW', 'REST', 'FLD', 'UI'}
MESH_EXT = ('.x', '.y', '.xet', '.ksm')
TEX_EXT = ('.tga', '.dds', '.bmp', '.png', '.jpg')


def ext(n):
    return os.path.splitext(n)[1].lower()


def rank(stem):
    return RC.mount_rank(stem)


def winner(stems):
    return max(stems, key=rank) if stems else ''


def june_bytes(low):
    """(bytes, error) of June's copy by lowercase name, strict decoder."""
    hit = ST.june.get(low)
    if not hit:
        return None, 'not in June'
    path, v04, e = hit
    with open(path, 'rb') as f:
        f.seek(e['offset'])
        raw = f.read(e['csize'])
    return junekom.decode(e, raw, v04)


def mesh_textures(low, tmpdir):
    """Texture base names a mesh/xet embeds: staged copy first, else June's decoded copy."""
    p = ST.staged_path(low)
    src = 'staged'
    if p is None:
        data, err = june_bytes(low)
        if data is None:
            return None, 'no source (%s)' % err
        p = os.path.join(tmpdir, 'm' + ext(low))
        with open(p, 'wb') as f:
            f.write(data)
        src = 'june'
    try:
        names, fmt = fxdata.embedded_textures(p, lenient=True)
    except Exception as ex:   # noqa
        return None, 'read error %s' % ex
    return names, src


def collect_roots(files, rows):
    refs = collections.defaultdict(set)
    for lines, src, text in RC.all_texts(files, rows):
        for m in set(RC.ASSET_RE.findall(text)):
            refs[m.strip().lower()].update(lines)
    return refs


def where(low):
    st = sorted(ST.staged.get(low, {}))
    lv = sorted(ST.live.get(low, {}))
    loose = low in ST.loose_march
    return st, lv, loose


def stem_variants(low):
    s, e = os.path.splitext(low)
    return [s + x for x in TEX_EXT if s + x != low]


def tet_pairs(files, rows):
    """(texture, key, source, lines) from Lua text: 2-string-arg calls and Name/Key table fields."""
    out = []
    call = re.compile(r'\w+\(\s*(?:[A-Za-z_][\w\.]*\s*,\s*)?["\']([^"\']+\.(?:tga|dds))["\']\s*,\s*["\']([^"\']+)["\']', re.I)
    fields = [('BOSS_GAGE_FACE_TEX', 'BOSS_GAGE_FACE_TEX_PIECE'), ('m_TextureName', 'm_PieceName'),
              ('BATTLE_FIELD_TEXTURE_NAME', 'BATTLE_FIELD_TEXTURE_KEY'), ('NPCTextureName', 'NPCTextureKey'),
              ('TEX_NAME', 'PIECE_NAME')]
    for lines, src, text in RC.all_texts(files, rows):
        for m in call.finditer(text):
            out.append((m.group(1), m.group(2), src, lines))
        for nk, kk in fields:
            ns = re.findall(r'\b%s\s*=\s*["\']([^"\']*)["\']' % re.escape(nk), text)
            ks = re.findall(r'\b%s\s*=\s*["\']([^"\']*)["\']' % re.escape(kk), text)
            for i in range(min(len(ns), len(ks))):
                if ns[i] and ks[i]:
                    out.append((ns[i], ks[i], src, lines))
    return out


def tet_keys(path):
    b = open(path, 'rb').read()
    if b[:4] != b'TET ':
        raise ValueError('not a TET')
    n = struct.unpack_from('<I', b, 8)[0]
    offs = struct.unpack_from('<%dI' % n, b, 12)
    keys = set()
    for o in offs:
        l = struct.unpack_from('<H', b, o)[0]
        keys.add(b[o + 2:o + 2 + 2 * l].decode('utf-16-le').upper())
    return keys


def main():
    J, M, errs = RC.load_enums()
    R = RC.derive(J)
    RC.nui_closure(R, J)
    files = RC.file_set(R, J)
    rows = RC.row_texts(R)
    refs = collect_roots(files, rows)
    tmpdir = tempfile.mkdtemp(prefix='a11_')
    live_sizes = {}

    def live_size(stem, real):
        if stem not in live_sizes:
            p = os.path.join(RC.GAME, stem + '.kom')
            live_sizes[stem] = {x['name'].lower(): x for x in kom.inspect(p)['files']} if os.path.exists(p) else {}
        ent = live_sizes[stem].get(real.lower())
        return None if not ent else ent.get('size', ent.get('usize', ent.get('uncompressed_size')))

    # ---------------- 1. closure
    rows_out = {}   # low -> dict

    def add(low, how, frm, lines):
        r = rows_out.setdefault(low, dict(name=low, how=how, frm=frm, atlas='N', lines=set()))
        r['lines'] |= set(lines)
        if frm < r['frm']:
            r['frm'] = frm      # first referrer = alphabetically smallest, so reruns give the same file
        if r['how'] == 'embedded' and how == 'census':
            r['how'] = 'census'
        if set(lines) & ATLAS_LINES:
            r['atlas'] = 'Y'
        return r

    for low, lines in sorted(refs.items()):
        if ext(low) == '.lua':
            continue
        add(low, 'census', ','.join(sorted(lines)), lines)
    queue = sorted(low for low in rows_out if ext(low) in MESH_EXT)
    seen = set(queue)
    expanded = {}
    while queue:
        low = queue.pop()
        names, src = mesh_textures(low, tmpdir)
        expanded[low] = (names, src)
        if names is None:
            continue
        for n in names:
            nl = n.lower()
            add(nl, 'embedded', low, rows_out[low]['lines'])
            if ext(nl) in MESH_EXT and nl not in seen:
                seen.add(nl)
                queue.append(nl)
    for low, r in rows_out.items():
        st, lv, loose = where(low)
        jn = ST.june.get(low)
        jloose = ST.loose_june.get(low)
        r['staged'] = ','.join(st)
        r['live'] = ','.join(lv)
        r['loose_march'] = 'Y' if loose else ''
        r['june'] = (os.path.basename(jn[0]) if jn else ('loose:' + os.path.basename(os.path.dirname(jloose)) if jloose else ''))
        r['june_size'] = (jn[2].get('size', '') if jn else (os.path.getsize(jloose) if jloose else ''))
        r['win'] = winner(set(st) | set(lv))
        if st or lv or loose:
            r['state'] = 'march-staged' if st and not lv else 'march-live'
            if st and lv and not (set(st) <= set(lv)):
                r['state'] = 'march-staged+live'
        else:
            # same stem, other texture extension (Lanox false-positive class)
            alt = [v for v in stem_variants(low) if where(v)[0] or where(v)[1]]
            if alt:
                r['state'] = 'ext-mismatch(march has %s)' % alt[0]
            elif jn or jloose:
                r['state'] = 'MISSING-june-has'
            else:
                r['state'] = 'absent-june-too'
    for low, (names, src) in expanded.items():
        if names is None:
            rows_out[low]['state'] += ' [no mesh source: %s]' % src

    cols = ['name', 'how', 'frm', 'atlas', 'state', 'win', 'staged', 'live', 'loose_march', 'june', 'june_size']
    with open(os.path.join(HERE, 'atlas_a11_closure.tsv'), 'w', newline='', encoding='utf-8') as f:
        w = csv.writer(f, delimiter='\t', lineterminator='\n')
        w.writerow(cols)
        for low in sorted(rows_out):
            w.writerow([rows_out[low][c] for c in cols])
    c = collections.Counter(r['state'].split(' ')[0].split('(')[0] for r in rows_out.values())
    ca = collections.Counter(r['state'].split(' ')[0].split('(')[0] for r in rows_out.values() if r['atlas'] == 'Y')
    print('CLOSURE rows %d (atlas-quoted %d)' % (len(rows_out), sum(ca.values())))
    print('  all  ', dict(c))
    print('  atlas', dict(ca))
    print('  meshes expanded %d, without a source %d' % (len(expanded), sum(1 for v in expanded.values() if v[0] is None)))
    for low, r in sorted(rows_out.items()):
        if r['state'].startswith(('MISSING', 'ext-mismatch')) or 'no mesh source' in r['state']:
            print('  !!', low, r['state'], 'from', r['frm'], 'june', r['june'])

    # ---------------- 2. TET-key census
    pairs = tet_pairs(files, rows)
    tet_rows, tcache = [], {}
    uniq = collections.OrderedDict()
    srcs = collections.defaultdict(set)
    for tex, key, src, lines in pairs:
        uniq.setdefault((tex.lower(), key.upper()), set()).update(lines)
        srcs[(tex.lower(), key.upper())].add(src)
    for (tex, key), lines in uniq.items():
        tet = os.path.splitext(tex)[0] + '.tet'
        st, lv, _ = where(tet)
        stems = set(st) | set(lv)
        win = winner(stems)
        res, detail = 'NOTET', ''
        if win:
            if tet not in tcache:
                real = ST.staged.get(tet, {}).get(win) or ST.live[tet][win]
                p = os.path.join(RC.KC, win + '_kom', real)
                try:
                    keys = tet_keys(p)
                    sz = os.path.getsize(p)
                    ls = live_size(win, real) if win in ST.live.get(tet, {}) else None
                    tcache[tet] = (keys, '' if ls == sz else ('staged %d B vs live %s B' % (sz, ls)))
                except Exception as ex:   # noqa
                    tcache[tet] = (None, 'unreadable %s' % ex)
            keys, detail = tcache[tet]
            res = 'UNREADABLE' if keys is None else ('OK' if key in keys else 'KEY-MISSING')
        else:
            jn = ST.june.get(tet)
            detail = 'June has it in %s' % os.path.basename(jn[0]) if jn else 'June has no TET either'
        tet_rows.append([tex, key, res, win, detail, ','.join(sorted(lines)), 'Y' if lines & ATLAS_LINES else 'N',
                         ';'.join(sorted(srcs[(tex, key)]))[:120]])
    with open(os.path.join(HERE, 'atlas_a11_tet.tsv'), 'w', newline='', encoding='utf-8') as f:
        w = csv.writer(f, delimiter='\t', lineterminator='\n')
        w.writerow(['texture', 'key', 'result', 'winning_mount', 'detail', 'lines', 'atlas', 'sources'])
        w.writerows(tet_rows)
    ct = collections.Counter(r[2] for r in tet_rows)
    cta = collections.Counter(r[2] for r in tet_rows if r[6] == 'Y')
    print('TET pairs %d: %s ; atlas-quoted %d: %s' % (len(tet_rows), dict(ct), sum(cta.values()), dict(cta)))
    for r in tet_rows:
        if r[2] != 'OK' and r[6] == 'Y':
            print('  !! ATLAS', r)

    # ---------------- 3. size check (every staged region file June also ships)
    short, size_rows = 0, []
    for low, r in sorted(rows_out.items()):
        if not r['staged'] or not r['june'] or r['june'].startswith('loose'):
            continue
        for stem in r['staged'].split(','):
            real = ST.staged[low][stem]
            p = os.path.join(RC.KC, stem + '_kom', real)
            sz = os.path.getsize(p)
            js = r['june_size']
            state = 'same' if sz == js else ('SHORT' if sz < js else 'LONGER')
            if state != 'same':
                short += 1
            size_rows.append([low, stem, sz, js, state])
    with open(os.path.join(HERE, 'atlas_a11_sizes.tsv'), 'w', newline='', encoding='utf-8') as f:
        w = csv.writer(f, delimiter='\t', lineterminator='\n')
        w.writerow(['name', 'staged_kom', 'staged_size', 'june_size', 'state'])
        w.writerows(size_rows)
    print('SIZES staged-and-in-June %d, differing %d' % (len(size_rows), short))
    for r in size_rows:
        if r[4] != 'same':
            print('  !!', r)

    # ---------------- 4. mount order: names in more than one archive
    multi = []
    for low, r in sorted(rows_out.items()):
        stems = sorted(set(filter(None, r['staged'].split(','))) | set(filter(None, r['live'].split(','))), key=rank)
        if len(stems) > 1:
            multi.append((low, stems))
    print('MOUNT names present in >1 archive: %d (winner = highest rank)' % len(multi))
    for low, stems in multi[:40]:
        print('  ', low, stems, '-> wins', winner(stems))

    # ---------------- 5. pack list: staged region files whose live kom entry is absent or differs
    pack = collections.defaultdict(list)
    names_all = set(rows_out) | {k for k in files} | {os.path.splitext(r[0])[0] + '.tet' for r in tet_rows}
    for low in sorted(names_all):
        for stem, real in ST.staged.get(low, {}).items():
            p = os.path.join(RC.KC, stem + '_kom', real)
            if not os.path.exists(os.path.join(RC.GAME, stem + '.kom')):
                pack[stem].append((real, 'NEW archive'))
                continue
            ls = live_size(stem, real)
            if ls is None:
                pack[stem].append((real, 'not in live kom'))
            elif ls != os.path.getsize(p):
                pack[stem].append((real, 'size differs: staged %d, live %s' % (os.path.getsize(p), ls)))
    with open(os.path.join(HERE, 'atlas_a11_packlist.tsv'), 'w', newline='', encoding='utf-8') as f:
        w = csv.writer(f, delimiter='\t', lineterminator='\n')
        w.writerow(['archive', 'file', 'why'])
        for stem in sorted(pack):
            for real, why in pack[stem]:
                w.writerow([stem, real, why])
    print('PACKLIST archives owing a pack/copy: %d, files %d' % (len(pack), sum(len(v) for v in pack.values())))
    for stem in sorted(pack):
        print('  ', stem, len(pack[stem]), pack[stem][:3])


if __name__ == '__main__':
    main()
