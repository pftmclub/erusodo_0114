"""Phase 11: stage June's effect data into the user's kom_contents for packing.

    python stage_june_fx.py            # dry run: writes stage_plan.tsv, prints the summary
    python stage_june_fx.py --apply    # copies files, updates _manifest.json, backs up what it replaces
    python stage_june_fx.py --revert --check   # what a revert would do; writes nothing
    python stage_june_fx.py --revert   # undo --apply: restore replaced files from the backup, delete
                                       # added files and their manifest keys, move the .src.lua back

What it stages (JUNE2017_EFFECT_REBUILD_PLAN.md, Phase 11; user's call 2026-10-03: straight
into kom_contents, no throwaway client copy):
  - the 10 June effect lua March opens, transcoded by ljconv.py (Phase 10b route (b));
    June's Enum.lua is NOT staged (March's Phase 11 Enum.lua is the one to keep);
  - every June Game/UI Major/Minor Particle*/XMeshPlayer*.txt (June wins at the same name);
  - from asset_closure.tsv: june-only/missing (add), march-live/differs and
    march-live/truncated-in-march (replace with June's copy).

Placement rules:
  - A name March already ships stays in the kom March ships it in (a second copy in another
    kom raises an _IN_HOUSE_ duplicate MessageBox at mount, KGCMassFileManager::AddMassFile).
  - A new name goes in June's archive number (the Ranox/Lanox convention), which must be one
    the client mounts: data001-226 / datai001-032 (X2Main.cpp:799-845,
    SERV_IRUHADEV_KOM_FILE_COUNT_226).
  - Never overwrite pending work: if March's kom_contents copy does not match the live
    manifest size, or the name is staged somewhere not yet packed (kom_contents outside any
    manifest, or march2014/update/), the row is a conflict and is skipped.

--apply backs every replaced file up to kom_contents_backup_june_fx/<kom>_kom/ (outside
kom_contents, so a pack never sees it), and moves the two Ranox-merged sources
data036_kom/{EffectSet,DamageEffect}.src.lua there too: elstools compile-dir rebuilds X.lua
from any X.src.lua beside it, so leaving them would silently put the merged copies back.
"""
import csv
import json
import os
import re
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import fxdata  # noqa: E402
import ljconv  # noqa: E402

BACKUP = os.path.join(os.path.dirname(fxdata.MAR_STAGE), 'kom_contents_backup_june_fx')
TRIAL_LUA = os.path.join(fxdata.CACHE, 'june_b')   # ljconv output, same bytes as Phase 10b
LUA10 = ['EffectSet', 'EffectSet_INT', 'UIEffectSet', 'UIEffectSet_INT', 'DamageEffect',
         'DamageEffect_Character', 'DamageEffect_Dungeon', 'DamageEffect_ETC', 'DamageEffect_INT',
         'DamageSound']
TXT_RE = re.compile(r'^(Game|UI)(Major|Minor)(Particle|XMeshPlayer)\d*\.txt$', re.I)
MERGED_SRC = ['EffectSet.src.lua', 'DamageEffect.src.lua']
KOM_NUM = re.compile(r'^data(i?)(\d+)$', re.I)


def mounted(kom):
    m = KOM_NUM.match(kom)
    if not m:
        return False
    n = int(m.group(2))
    return 1 <= n <= (32 if m.group(1) else 226)


def lua_paths():
    """The 10 transcoded files; rebuilt with ljconv and checked against the cached ones."""
    src = [os.path.join(HERE, 'june_lua', n + '.lua') for n in LUA10]
    table, _, _ = ljconv.build_table(src)
    ljconv.check_modes(table)
    out = {}
    for f in src:
        b, _, _ = ljconv.transcode(open(f, 'rb').read(), table)
        cached = os.path.join(TRIAL_LUA, os.path.basename(f))
        if not os.path.exists(cached) or open(cached, 'rb').read() != b:
            open(cached, 'wb').write(b)
        out[os.path.basename(f).lower()] = cached
    return out


def main(apply):
    mar = fxdata.Build('march', fxdata.MAR_DATA, fxdata.MAR_STAGE)
    jun = fxdata.Build('june', fxdata.JUNE_DATA, fxdata.JUNE_DATA)
    upd = fxdata.update_inventory()

    wanted = {}   # lower name -> (source kind, reason)
    lua = lua_paths()
    for low in lua:
        wanted[low] = ('lua', 'June effect lua (ljconv)')
    for low, copies in jun.inv.items():
        if TXT_RE.match(copies[0].name):
            wanted.setdefault(low, ('june', 'June particle/mesh-player .txt'))
    for r in csv.DictReader(open(os.path.join(HERE, 'asset_closure.tsv'), encoding='utf-8'),
                            delimiter='\t'):
        v = (r['loc'], r['verdict'])
        if v in (('june-only', 'missing'), ('march-live', 'differs'),
                 ('march-live', 'truncated-in-march')):
            wanted.setdefault(r['name'].lower(), ('june', 'closure %s/%s' % v))

    rows = []
    for low, (kind, reason) in sorted(wanted.items()):
        jc = jun.inv.get(low, [])
        if kind == 'lua':
            src, srcnote, jname = lua[low], 'ljconv', os.path.basename(lua[low])
            jkom = jc[0].kom if jc else ''
        else:
            if not jc:
                rows.append(dict(action='skip', name=low, kom='', reason=reason,
                                 note='not in any June archive', src='', dst=''))
                continue
            c = next((x for x in jc if x.path), None) or next((x for x in jc if x.xpath), None)
            if c is None:
                rows.append(dict(action='skip', name=jc[0].name, kom=jc[0].kom, reason=reason,
                                 note='June copy not extracted', src='', dst=''))
                continue
            src = c.path or c.xpath
            srcnote = 'exact' if c.path else 'short %d B' % (c.size - os.path.getsize(c.xpath))
            jname, jkom = c.name, c.kom
        mc = mar.inv.get(low, [])
        conflict = []
        if len(mc) > 1:
            conflict.append('March ships it in %d koms (%s)' % (len(mc), ','.join(x.kom for x in mc)))
        if mc:
            m = mc[0]
            kom, dname, action = m.kom, m.name, 'replace'
            if m.path is None:
                conflict.append('kom_contents copy differs from live %s.kom (pending work?)' % kom)
        else:
            kom, dname, action = jkom, jname, 'add'
            if not mounted(kom):
                conflict.append('June archive %s is not mounted by March' % kom)
        for (skom, sname, spath) in mar.staged.get(low, []):
            conflict.append('staged unpacked in %s_kom' % skom)
        for (udir, uname, upath) in upd.get(low, []):
            conflict.append('staged in march2014/update/%s' % udir)
        dst = os.path.join(fxdata.MAR_STAGE, kom + '_kom', dname)
        if action == 'replace' and os.path.exists(dst) and \
                open(dst, 'rb').read() == open(src, 'rb').read():
            action = 'noop'
        rows.append(dict(action='conflict' if conflict else action, name=dname, kom=kom,
                         reason=reason, note='; '.join([srcnote] + conflict), src=src, dst=dst))

    with open(os.path.join(HERE, 'stage_plan.tsv'), 'w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, ['action', 'kom', 'name', 'reason', 'note', 'src', 'dst'],
                           delimiter='\t', lineterminator='\n')
        w.writeheader()
        for r in rows:
            w.writerow(r)
    import collections
    print(collections.Counter((r['action'], r['reason']) for r in rows).most_common())
    print('short June copies:', sum(1 for r in rows if r['note'].startswith('short')
                                    and r['action'] in ('add', 'replace')))
    for r in rows:
        if r['action'] in ('conflict', 'skip'):
            print(' ', r['action'], r['kom'], r['name'], '|', r['note'])
    newdirs = sorted({r['kom'] for r in rows if r['action'] == 'add'
                      and not os.path.isdir(os.path.join(fxdata.MAR_STAGE, r['kom'] + '_kom'))})
    print('new kom_contents dirs needed:', newdirs)
    if apply:
        do_apply(rows)


def algo(n):
    """Manifest value: "2" for .lua and .txt (the live data036.kom stores every .txt as algo 2,
    like .lua: elstools inspect, 2026-10-03), "0" for everything else."""
    return b'2' if n.lower().endswith(('.lua', '.txt')) else b'0'


def manifest_add(d, names):
    """Append keys to <d>/_manifest.json in the file's own style (indent, newline, no trailing
    newline); create the file if the directory is new."""
    p = os.path.join(d, '_manifest.json')
    if os.path.exists(p):
        b = open(p, 'rb').read()
        nl = b'\r\n' if b'\r\n' in b else b'\n'
        bom = b.startswith(b'\xef\xbb\xbf')
        have = {k.lower() for k in json.loads(b.decode('utf-8-sig'))}
        m = re.search(rb'\n([ \t]+)"', b)
        ind = m.group(1) if m else b'  '
        body = b.rstrip()
        assert body.endswith(b'}'), p
        body = body[:-1].rstrip()
        add = [n for n in names if n.lower() not in have]
        if not add:
            return 0
        empty = body.endswith(b'{')
        parts = [(b'' if empty else b',') + nl]
        parts.append((b',' + nl).join(ind + json.dumps(n).encode('utf-8') + b': "' +
                                      algo(n) + b'"'
                                      for n in add))
        out = body + b''.join(parts) + nl + b'}'
        if b.endswith(nl):
            out += nl
        json.loads(out.decode('utf-8-sig'))
        open(p, 'wb').write(out)
        return len(add)
    out = b'{\n' + b',\n'.join(b'  ' + json.dumps(n).encode('utf-8') + b': "' +
                               algo(n) + b'"'
                               for n in names) + b'\n}'
    open(p, 'wb').write(out)
    return len(names)


def do_apply(rows):
    os.makedirs(BACKUP, exist_ok=True)
    for s in MERGED_SRC:
        p = os.path.join(fxdata.MAR_STAGE, 'data036_kom', s)
        if os.path.exists(p):
            os.makedirs(os.path.join(BACKUP, 'data036_kom'), exist_ok=True)
            shutil.move(p, os.path.join(BACKUP, 'data036_kom', s))
            print('moved out', s)
    added = {}
    n = {'add': 0, 'replace': 0}
    for r in rows:
        if r['action'] not in ('add', 'replace'):
            continue
        d = os.path.dirname(r['dst'])
        os.makedirs(d, exist_ok=True)
        if r['action'] == 'replace':
            bd = os.path.join(BACKUP, r['kom'] + '_kom')
            os.makedirs(bd, exist_ok=True)
            bp = os.path.join(bd, r['name'])
            if not os.path.exists(bp):
                shutil.copy2(r['dst'], bp)
        shutil.copyfile(r['src'], r['dst'])
        n[r['action']] += 1
        if r['action'] == 'add':
            added.setdefault(d, []).append(r['name'])
    m = sum(manifest_add(d, names) for d, names in sorted(added.items()))
    print('applied', n, 'manifest keys added', m)


def manifest_drop(d, names, write):
    """Undo manifest_add: the added keys are a contiguous tail, so cut back to the line before
    the first of them, drop the comma manifest_add put after the old last entry, and close."""
    p = os.path.join(d, '_manifest.json')
    b = open(p, 'rb').read()
    nl = b'\r\n' if b'\r\n' in b else b'\n'
    keys = list(json.loads(b.decode('utf-8-sig')).keys())
    low = {n.lower() for n in names}
    tail = [k for k in keys if k.lower() in low]
    if not tail:
        return 0
    assert keys[-len(tail):] == tail, '%s: added keys are not the manifest tail' % p
    if len(tail) == len(keys):
        return -1   # manifest created by --apply: the caller removes the directory
    m = re.search(re.escape(nl) + rb'[ \t]+' + re.escape(json.dumps(tail[0]).encode('utf-8')) + rb':', b)
    assert m, p
    head = b[:m.start()].rstrip()
    assert head.endswith(b','), p
    out = head[:-1] + nl + b'}'
    if b.endswith(nl):
        out += nl
    assert list(json.loads(out.decode('utf-8-sig')).keys()) == keys[:-len(tail)], p
    if write:
        open(p, 'wb').write(out)
    return len(tail)


def do_revert(write):
    """Put kom_contents back as it was before --apply, from stage_plan.tsv and the backup dir."""
    rows = list(csv.DictReader(open(os.path.join(HERE, 'stage_plan.tsv'), encoding='utf-8'),
                               delimiter='\t'))
    n = collections.Counter()
    added = {}
    for r in rows:
        if r['action'] == 'replace':
            bp = os.path.join(BACKUP, r['kom'] + '_kom', r['name'])
            assert os.path.exists(bp), bp
            if write:
                shutil.copyfile(bp, r['dst'])
            n['restored'] += 1
        elif r['action'] == 'add':
            added.setdefault(os.path.dirname(r['dst']), []).append(r['name'])
    for d, names in sorted(added.items()):
        k = manifest_drop(d, names, write)
        if k == -1:
            if write:
                shutil.rmtree(d)
            n['dirs removed'] += 1
            continue
        n['manifest keys dropped'] += k
        for nm in names:
            p = os.path.join(d, nm)
            if os.path.exists(p):
                if write:
                    os.remove(p)
                n['deleted'] += 1
    for s in MERGED_SRC:
        bp = os.path.join(BACKUP, 'data036_kom', s)
        if os.path.exists(bp):
            if write:
                shutil.move(bp, os.path.join(fxdata.MAR_STAGE, 'data036_kom', s))
            n['src moved back'] += 1
    print('revert' if write else 'revert (check only, nothing written)', dict(n))


if __name__ == '__main__':
    import collections
    if '--revert' in sys.argv:
        do_revert('--check' not in sys.argv)
    else:
        main('--apply' in sys.argv)
