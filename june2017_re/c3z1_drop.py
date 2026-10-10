r"""Remove the meshes June's own world scripts name but June no longer ships (June effect C3z-1, user 2026-10-10: "remove all 306").
June rebuilt old Besma Mine / Lake / Dragon Road maps with fewer, merged meshes; its world scripts still name the old pieces, which
fail to load in June (XEM_ERROR109, nothing drawn). March's copies would draw over June's rebuilt meshes. March is crash-safe with a
missing world mesh (CX2WorldObjectMesh::SetXMesh_LUA logs XEM_ERROR109; render and RayHit check the mesh).
    python june2017_re/c3z1_drop.py <list.tsv> [--dry-run]     (list: `name` column; from the Trunk)
Each file is moved to <kom_contents>/../kom_backup_stage_june/<stamp>_c3z1_drop/dataNNN_kom/ and its _manifest.json key is removed by
text (the file's own line ending and indent kept). Restore = move the files back and re-add the keys ("0").
"""
import os, re, sys, csv, json, shutil, datetime
sys.path.insert(0, os.path.join(os.getcwd(), "tools"))
import kom

dry = "--dry-run" in sys.argv
names = [r["name"].lower() for r in csv.DictReader(open(sys.argv[1], encoding="utf-8"), delimiter="\t")]
staged = kom.staged_names()
stamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S") + "_c3z1_drop"
by_kom, missing = {}, []
for n in names:
    d = staged.get(n)
    if not d or len(d) != 1:
        missing.append((n, d)); continue
    (stem, sp), = d.items()
    by_kom.setdefault(stem, []).append(sp)
if missing:
    sys.exit("REFUSED: not in exactly one kom dir: %s" % missing[:10])
for stem, sps in sorted(by_kom.items()):
    d = os.path.join(kom.KOM_ROOT, stem + "_kom")
    mp = os.path.join(d, "_manifest.json")
    raw = open(mp, "rb").read()
    eol = b"\r\n" if b"\r\n" in raw else b"\n"
    lines = raw.split(eol)
    drop = {s.lower() for s in sps}
    keep = [l for l in lines if not (re.match(rb'\s*"', l) and json.loads(b"{" + l.rstrip(b",") + b"}").popitem()[0].lower() in drop)]
    removed = len(lines) - len(keep)
    if removed != len(sps):
        sys.exit("REFUSED: %s: %d manifest keys match, %d files" % (stem, removed, len(sps)))
    # the last entry before the closing brace carries no comma
    i = max(j for j, l in enumerate(keep) if l.strip().startswith(b"}"))
    k = i - 1
    while not keep[k].strip():
        k -= 1
    keep[k] = keep[k].rstrip(b",")
    new = eol.join(keep)
    json.loads(new.decode("utf-8-sig"))
    print("%s: %d file(s) %s, %d manifest key(s)" % (stem, len(sps), "to remove (dry run)" if dry else "removed", removed))
    if dry:
        continue
    bk = os.path.join(os.path.dirname(kom.KOM_ROOT), "kom_backup_stage_june", stamp, stem + "_kom")
    os.makedirs(bk, exist_ok=True)
    shutil.copy2(mp, os.path.join(bk, "_manifest.json.before"))
    for sp in sps:
        shutil.move(os.path.join(d, sp), os.path.join(bk, sp))
    with open(mp + ".part", "wb") as f:
        f.write(new)
    os.replace(mp + ".part", mp)
if not dry:
    print("backup: %s" % os.path.join(os.path.dirname(kom.KOM_ROOT), "kom_backup_stage_june", stamp))
