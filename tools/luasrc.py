#!/usr/bin/env python3
"""Keep March client .lua as BOM + UTF-8 source (not bytecode), and prove each conversion.  See LUA_SOURCE_PHASES.md.

    python tools/luasrc.py find data036 X.lua               # exact sources for one file, compile-compared
    python tools/luasrc.py convert data036 X.lua [Y.lua..]  # bytecode X.lua -> BOM + source (T0 proof only)
    python tools/luasrc.py convert data036 'DLG_*.lua'      # names may be globs
    python tools/luasrc.py convert data036 --dry-run        # whole kom dir: what WOULD convert, writes nothing
    python tools/luasrc.py convert --all --dry-run          # every kom dir that has .lua
    python tools/luasrc.py restore data036 X.lua            # put the snapshot bytes back, ledger row -> restored
    python tools/luasrc.py refresh data036 X.lua            # re-record the sha after a deliberate text edit
    python tools/luasrc.py ledger [--check]                 # census; --check = consistency (exit 1 on any FAIL)

Proof tiers (Rule 1): T0 = the source compiles BYTE-IDENTICAL to the shipped bytecode (luajit -b, mar-2014 header).
T1 / T2 / T3 arrive with L2 and are not offered here. Sources tried, in Rule 3's order: the sibling X.src.lua, then a studio
original from ClientScript\\ or _ClientScript\\ (every copy of the name is tried). Conversion changes FORMAT only (Rule 4).

Files: bytecode snapshots go to <kom_contents>/../kom_contents_bytecode/dataNNN_kom/ (X.lua and any X.src.lua);
the ledger is tools/data/lua_source_ledger.tsv. Overrides: X2_KOM_CONTENTS, X2_KOM_BYTECODE, X2_LUA_LEDGER,
X2_STUDIO_SCRIPT_DIRS (os.pathsep-separated).  Compiles run from .build/luasrc_tmp/ (file to file: stdout pipes add CRLFs).

Exit codes: 0 ok, 1 failed (check FAIL / error), 2 usage, 4 refused (no proof / not convertible).
Never launches the game.  Imported by tools/kom.py for the pack gate and the compile fold.
"""
import argparse, concurrent.futures as cf, datetime, fnmatch, hashlib, json, os, re, shutil, subprocess, sys, uuid

for _s in (sys.stdout, sys.stderr):
    try:
        _s.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass

TRUNK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LUAJIT_DIR = os.path.join(TRUNK, "elstools", "vendor", "luajit")
LUAJIT = os.path.join(LUAJIT_DIR, "luajit.exe")
KOM_ROOT = os.environ.get("X2_KOM_CONTENTS", r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents")
BYTECODE_ROOT = os.environ.get("X2_KOM_BYTECODE", os.path.join(os.path.dirname(KOM_ROOT), "kom_contents_bytecode"))
LEDGER = os.environ.get("X2_LUA_LEDGER", os.path.join(TRUNK, "tools", "data", "lua_source_ledger.tsv"))
_STUDIO = r"F:\elsword stuff\elsword_2014\els_2014\ElswordFiles\Elsword\Elsword"
STUDIO_DIRS = ([d for d in os.environ["X2_STUDIO_SCRIPT_DIRS"].split(os.pathsep) if d] if os.environ.get("X2_STUDIO_SCRIPT_DIRS")
               else [os.path.join(_STUDIO, "ClientScript"), os.path.join(_STUDIO, "_ClientScript")])
TMP = os.path.join(TRUNK, ".build", "luasrc_tmp")
COLS = ["kom", "name", "origin", "reference", "reference_sha1", "source", "tier", "source_sha1", "phase", "date", "reason", "state"]
BOM = b"\xef\xbb\xbf"
MARCH_BC = b"\x1bKL\x81"
JUNE_BC = b"\x1bKL\x82"
JOBS = max(2, min(12, os.cpu_count() or 2))
SHOW = 15
EXC = "EXC"      # ledger tier of a deliberate bytecode exception (Rule 5)


def p(*a):
    print(*a, flush=True)


def die(code, *a):
    p(*a)
    sys.exit(code)


def norm(arg):
    m = re.fullmatch(r"(?i)(?:data)?(i?)0*(\d+)(?:_kom|_packed|\.kom|_packed\.kom)?", arg.strip())
    if not m:
        die(2, "cannot read %r as an archive name (use data036, 036 or datai001)" % arg)
    return "data%s%s" % (m.group(1).lower(), m.group(2).zfill(3))


def kom_dir(kom):
    return os.path.join(KOM_ROOT, kom + "_kom")


def sha1(b):
    return hashlib.sha1(b).hexdigest()


def read(path):
    with open(path, "rb") as f:
        return f.read()


def write_atomic(path, data):
    tmp = path + ".part"
    with open(tmp, "wb") as f:
        f.write(data)
    os.replace(tmp, path)


def strip_bom(b):
    return b[3:] if b[:3] == BOM else b


def kind(b):
    """Classify a .lua payload by its first bytes."""
    if b[:4] == MARCH_BC:
        return "march-bytecode"
    if b[:4] == JUNE_BC:
        return "june-bytecode"
    if b[:1] == b"\x1b":
        return "other-bytecode"
    if b[:3] == BOM:
        return "bom-text"
    if not b:
        return "empty"
    return "bare-text"


# ----------------------------------------------------------------------------------------------
# compiling (vendored LuaJIT 2.0.2, KOG build): file to file, BOM stripped by us, never a stdout pipe
def _env():
    e = os.environ.copy()
    e["LUA_PATH"] = os.path.join(LUAJIT_DIR, "?.lua")
    return e


def compile_text(text, want_bytes=True):
    """Compile source bytes (BOM already stripped). Returns (bytecode|None, error message). Stripped (flags 02) like the shipped files."""
    if not os.path.isfile(LUAJIT):
        die(1, "luajit.exe not found at %s" % LUAJIT)
    os.makedirs(TMP, exist_ok=True)
    stem = os.path.join(TMP, uuid.uuid4().hex)
    src, out = stem + ".lua", stem + ".bin"
    try:
        with open(src, "wb") as f:
            f.write(text)
        r = subprocess.run([LUAJIT, "-b", src, out if want_bytes else "NUL"], stdin=subprocess.DEVNULL, capture_output=True,
                           env=_env(), timeout=600)
        if r.returncode != 0:
            msg = (r.stderr or r.stdout).decode("utf-8", "replace").strip()
            msg = re.sub(r"^luajit:\s*(?:\S.*?\.lua|\S*\.\.\.\S*):(\d+):\s*", r"line \1: ", msg)
            return None, msg.splitlines()[0] if msg else "luajit rc=%d" % r.returncode
        return (read(out) if want_bytes else b""), ""
    except subprocess.TimeoutExpired:
        return None, "luajit timed out"
    finally:
        for f in (src, out):
            try:
                os.remove(f)
            except OSError:
                pass


def utf8_ok(text):
    try:
        text.decode("utf-8")
        return True
    except UnicodeDecodeError:
        return False


# ----------------------------------------------------------------------------------------------
# ledger
def ledger_load():
    if not os.path.isfile(LEDGER):
        return []
    lines = read(LEDGER).decode("utf-8").splitlines()
    if not lines:
        return []
    head = lines[0].split("\t")
    rows = []
    for ln in lines[1:]:
        if not ln.strip():
            continue
        cells = ln.split("\t")
        rows.append({c: (cells[i] if i < len(cells) else "") for i, c in enumerate(head)})
    for r in rows:
        for c in COLS:
            r.setdefault(c, "")
        r["state"] = r["state"] or "live"
    return rows


def ledger_save(rows):
    out = ["\t".join(COLS)]
    for r in rows:
        out.append("\t".join(str(r.get(c, "")).replace("\t", " ").replace("\n", " ") for c in COLS))
    os.makedirs(os.path.dirname(LEDGER), exist_ok=True)
    write_atomic(LEDGER, ("\n".join(out) + "\n").encode("utf-8"))


def ledger_latest(rows):
    """(kom, lower name) -> last LIVE row (a later `restored` row supersedes)."""
    cur = {}
    for r in rows:
        key = (r["kom"], r["name"].lower())
        if r["state"] == "restored":
            cur.pop(key, None)
        else:
            cur[key] = r
    return cur


def exceptions():
    """{(kom, lower name)} of live EXC rows (Rule 5's bytecode exceptions)."""
    return {k for k, r in ledger_latest(ledger_load()).items() if r["tier"] == EXC}


def new_row(kom, name, origin, ref, ref_sha, source, tier, src_sha, phase, reason):
    return {"kom": kom, "name": name, "origin": origin, "reference": ref, "reference_sha1": ref_sha, "source": source,
            "tier": tier, "source_sha1": src_sha, "phase": phase, "date": datetime.date.today().isoformat(), "reason": reason,
            "state": "live"}


# ----------------------------------------------------------------------------------------------
# directory helpers
def dir_index(d):
    """lower relative name -> relative name, for every file in a kom dir."""
    out = {}
    for root, _dirs, files in os.walk(d):
        for fn in files:
            rel = os.path.relpath(os.path.join(root, fn), d).replace(os.sep, "/")
            out[rel.lower()] = rel
    return out


def lua_names(d):
    """Non-sidecar .lua names in a kom dir."""
    return sorted(n for n in dir_index(d).values() if n.lower().endswith(".lua") and not n.lower().endswith(".src.lua"))


def load_manifest(d):
    mp = os.path.join(d, "_manifest.json")
    if not os.path.isfile(mp):
        return {}
    with open(mp, "r", encoding="utf-8-sig") as f:
        return json.load(f)


def kom_dirs_with_lua():
    out = []
    for fn in sorted(os.listdir(KOM_ROOT)):
        m = re.fullmatch(r"(data[i]?\d{3})_kom", fn)
        if m and os.path.isdir(os.path.join(KOM_ROOT, fn)) and lua_names(os.path.join(KOM_ROOT, fn)):
            out.append(m.group(1))
    return out


_studio_cache = None


def studio_index():
    """lower basename -> [full paths], ClientScript before _ClientScript, each sorted."""
    global _studio_cache
    if _studio_cache is None:
        idx = {}
        for root_dir in STUDIO_DIRS:
            if not os.path.isdir(root_dir):
                continue
            for root, _dirs, files in sorted(os.walk(root_dir)):
                for fn in sorted(files):
                    if fn.lower().endswith(".lua") and not fn.lower().endswith(".src.lua"):
                        idx.setdefault(fn.lower(), []).append(os.path.join(root, fn))
        _studio_cache = idx
    return _studio_cache


def candidates(d, name):
    """[(label, path)] in Rule 3's order: sibling .src.lua, then each studio copy."""
    out = []
    side = os.path.join(d, name[:-4] + ".src.lua")
    if os.path.isfile(side):
        out.append(("src.lua", side))
    for path in studio_index().get(os.path.basename(name).lower(), []):
        top = next((os.path.basename(r) for r in STUDIO_DIRS if os.path.abspath(path).lower().startswith(os.path.abspath(r).lower())), "studio")
        out.append((top, path))
    return out


def prove_t0(ref, cand_path):
    """Does this candidate compile byte-identical to ref? -> (ok, why, text)."""
    text = strip_bom(read(cand_path))
    bc, err = compile_text(text)
    if bc is None:
        return False, "does not compile (%s)" % err, text
    if bc == ref:
        return True, "T0: byte-identical (%d bytes)" % len(bc), text
    n = next((i for i in range(min(len(bc), len(ref))) if bc[i] != ref[i]), min(len(bc), len(ref)))
    return False, "compiles, but differs from the shipped bytecode (sizes %d vs %d, first difference at byte %d)" % (len(bc), len(ref), n), text


def find_t0(d, name, ref):
    """First T0 source -> (label, path, text, tried[(label,path,why)]) or (None, None, None, tried)."""
    tried = []
    for label, path in candidates(d, name):
        ok, why, text = prove_t0(ref, path)
        tried.append((label, path, why))
        if ok:
            return label, path, text, tried
    return None, None, None, tried


# ----------------------------------------------------------------------------------------------
# snapshots (kom_contents_bytecode)
def snapshot_dir(kom):
    return os.path.join(BYTECODE_ROOT, kom + "_kom")


def snapshot(kom, d, name):
    """Keep the bytecode (and a sidecar) before overwriting. A snapshot that already holds these bytes is left; a different one is rotated."""
    sd = snapshot_dir(kom)
    os.makedirs(sd, exist_ok=True)
    stamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    made = []
    for rel in (name, name[:-4] + ".src.lua"):
        src = os.path.join(d, rel)
        if not os.path.isfile(src):
            continue
        if rel == name and kind(read(src)) != "march-bytecode":
            continue          # never snapshot a text X.lua as "the bytecode"
        dst = os.path.join(sd, rel)
        if os.path.isfile(dst):
            if read(dst) == read(src):
                continue
            os.replace(dst, dst + ".prev_" + stamp)
        shutil.copy2(src, dst)
        made.append(rel)
    return made


def write_source(d, name, text):
    write_atomic(os.path.join(d, name), BOM + text)


# ----------------------------------------------------------------------------------------------
def classify_file(d, name, man, pool_results=None):
    """What `convert` would do with one staged file: (state, detail, ref, label, path, text)."""
    data = read(os.path.join(d, name))
    k = kind(data)
    if k == "bom-text":
        return "already-source", "BOM text", data, None, None, None
    if k != "march-bytecode":
        return "not-march-bytecode", k, data, None, None, None
    if str(man.get(name, "?")) != "2":
        return "manifest", "manifest value %r (convert only touches \"2\" files)" % man.get(name, "(missing)"), data, None, None, None
    label, path, text, tried = find_t0(d, name, data)
    if label:
        return "t0", label, data, label, path, text
    why = "; ".join("%s: %s" % (lb, w) for lb, _p, w in tried) if tried else "no exact source found (no sibling .src.lua, no studio copy)"
    return "no-proof", why, data, None, None, None


def cmd_find(kom, names):
    d = kom_dir(kom)
    if not os.path.isdir(d):
        die(2, "no such kom dir: %s" % d)
    idx = dir_index(d)
    rc = 0
    for nm in names:
        rel = idx.get(nm.lower())
        if not rel:
            p("== %s %s: not in the dir" % (kom, nm)); rc = 2; continue
        ref = read(os.path.join(d, rel))
        p("== %s %s  (%s, %d bytes)" % (kom, rel, kind(ref), len(ref)))
        if kind(ref) != "march-bytecode":
            p("   the staged file is not March bytecode; there is nothing to prove against.")
            continue
        cands = candidates(d, rel)
        if not cands:
            p("   no candidate source (no sibling .src.lua, no ClientScript / _ClientScript copy).")
            continue
        for label, path in cands:
            ok, why, _t = prove_t0(ref, path)
            p("   %-14s %s  -> %s" % (label, path, why))
    return rc


def convert_one(kom, d, name, st, label, path, text, ref, a):
    """Write one proven conversion. Returns the ledger row."""
    made = snapshot(kom, d, name)
    write_source(d, name, text)
    side = os.path.join(d, name[:-4] + ".src.lua")
    folded = ""
    if label == "src.lua" and os.path.isfile(side):
        os.remove(side)
        folded = "; sidecar folded"
    elif os.path.isfile(side):
        folded = "; unrelated sidecar left"
    return new_row(kom, name, a.origin, "shipped March bytecode" if a.origin == "march" else "ljconv(June bytecode)", sha1(ref),
                   {"src.lua": "src.lua", "ClientScript": "ClientScript", "_ClientScript": "_ClientScript"}.get(label, label), "T0",
                   sha1(text), a.phase, a.reason + folded)


def cmd_convert(a):
    t0_list, skipped = [], {"already-source": 0, "not-march-bytecode": 0, "manifest": 0}
    noproof = []
    explicit = bool(a.names)
    if a.all:
        koms = kom_dirs_with_lua()
    else:
        koms = [a.kom]
    if not os.path.isdir(KOM_ROOT):
        die(1, "kom_contents not found: %s" % KOM_ROOT)
    work = []
    for kom in koms:
        d = kom_dir(kom)
        if not os.path.isdir(d):
            die(2, "no such kom dir: %s" % d)
        man = load_manifest(d)
        names = lua_names(d)
        if explicit:
            sel = []
            for pat in a.names:
                hit = [n for n in names if fnmatch.fnmatchcase(n.lower(), pat.lower())]
                if not hit:
                    die(2, "no .lua matching %r in %s" % (pat, kom))
                sel += hit
            names = sorted(set(sel))
        work += [(kom, d, n, man) for n in names]
    p("== convert %s: %d .lua file(s) %s" % (", ".join(koms) if len(koms) < 4 else "%d koms" % len(koms), len(work),
                                          "(DRY RUN, nothing is written)" if a.dry_run else ""))
    studio_index()   # build once, before the pool
    results = []
    with cf.ThreadPoolExecutor(max_workers=a.jobs) as ex:
        futs = [ex.submit(classify_file, d, n, man) for kom, d, n, man in work]
        for (kom, d, n, man), fu in zip(work, futs):
            results.append(((kom, d, n), fu.result()))
    refused_explicit = 0
    rows, tsv = [], ["kom\tname\tstate\tsource\tdetail"]
    by_label, by_kom = {}, {}
    for (kom, d, n), (st, detail, ref, label, path, text) in results:
        tsv.append("\t".join([kom, n, st, label or "", detail.replace("\t", " ")]))
        if st == "t0":
            t0_list.append((kom, n))
            by_label[label] = by_label.get(label, 0) + 1
            by_kom[kom] = by_kom.get(kom, 0) + 1
            if not a.dry_run:
                rows.append(convert_one(kom, d, n, st, label, path, text, ref, a))
            if explicit or len(work) <= 20:
                p("  %s %-8s %s  [%s%s]" % ("would convert" if a.dry_run else "CONVERTED", kom, n, label, "" if a.dry_run else ", snapshot + ledger"))
        elif st in skipped:
            skipped[st] += 1
            if explicit:
                p("  skip     %-8s %s: %s" % (kom, n, detail))
        else:
            noproof.append((kom, n, detail))
            if explicit:
                refused_explicit += 1
                p("  REFUSED  %-8s %s: %s" % (kom, n, detail))
    if rows:
        ledger_save(ledger_load() + rows)
    if a.plan_out:
        with open(a.plan_out, "w", encoding="utf-8", newline="\n") as f:
            f.write("\n".join(tsv) + "\n")
        p("  plan written: %s" % a.plan_out)
    nokom = {}
    for kom, n, _w in noproof:
        nokom[kom] = nokom.get(kom, 0) + 1
    p("  T0 by source: %s;  by kom: %s" % (by_label or "none", dict(sorted(by_kom.items())) or "none"))
    p("  no T0 proof (needs the L2 decompile tier): %d  %s" % (len(noproof), dict(sorted(nokom.items())) if nokom else ""))
    p("  skipped: already source %d, not March bytecode %d, manifest not \"2\" %d" % (
        skipped["already-source"], skipped["not-march-bytecode"], skipped["manifest"]))
    if a.dry_run:
        p("CONVERT DRY RUN: %d T0 convertible, %d need L2, %d skipped; nothing changed." % (len(t0_list), len(noproof), sum(skipped.values())))
    else:
        p("CONVERT OK: %d converted (T0), %d refused/unproven, %d skipped. Next: kom.py pack %s   (then ask before `copy`)" % (
            len(t0_list), len(noproof), sum(skipped.values()), " ".join(sorted({k for k, _n in t0_list}))))
    if explicit and refused_explicit:
        sys.exit(4)


def cmd_restore(kom, names):
    d = kom_dir(kom)
    sd = snapshot_dir(kom)
    if not os.path.isdir(d):
        die(2, "no such kom dir: %s" % d)
    rows = ledger_load()
    cur = ledger_latest(rows)
    idx = dir_index(d)
    bad = 0
    for nm in names:
        rel = idx.get(nm.lower()) or nm
        snap = os.path.join(sd, rel)
        if not os.path.isfile(snap):
            p("  REFUSED %s: no snapshot at %s" % (rel, snap)); bad += 1; continue
        data = read(snap)
        if kind(data) != "march-bytecode":
            p("  REFUSED %s: the snapshot is not March bytecode (%s)" % (rel, kind(data))); bad += 1; continue
        write_atomic(os.path.join(d, rel), data)
        side = os.path.join(sd, rel[:-4] + ".src.lua")
        if os.path.isfile(side):
            write_atomic(os.path.join(d, rel[:-4] + ".src.lua"), read(side))
        row = cur.get((kom, rel.lower()))
        note = ""
        if row:
            for r in rows:
                if r is not None and r["kom"] == kom and r["name"].lower() == rel.lower() and r["state"] == "live":
                    r["state"] = "restored"
                    r["reason"] = (r["reason"] + "; restored " + datetime.date.today().isoformat()).strip("; ")
            if row["reference_sha1"] and row["reference_sha1"] != sha1(data):
                note = "  (note: snapshot sha1 differs from the ledger's reference sha1)"
        else:
            note = "  (no ledger row for it)"
        p("  RESTORED %s %s: %d bytes from the snapshot%s%s" % (kom, rel, len(data), " + sidecar" if os.path.isfile(side) else "", note))
    ledger_save(rows)
    if bad:
        sys.exit(4)
    p("RESTORE OK: %d file(s). Next: kom.py pack %s" % (len(names), kom))


def cmd_refresh(kom, names):
    d = kom_dir(kom)
    rows = ledger_load()
    cur = ledger_latest(rows)
    idx = dir_index(d)
    bad = 0
    for nm in names:
        rel = idx.get(nm.lower())
        row = cur.get((kom, (rel or nm).lower()))
        if not rel or not row or row["tier"] == EXC:
            p("  REFUSED %s: not a live converted/adopted ledger row" % nm); bad += 1; continue
        data = read(os.path.join(d, rel))
        if kind(data) != "bom-text":
            p("  REFUSED %s: the file is not BOM text (%s)" % (rel, kind(data))); bad += 1; continue
        text = strip_bom(data)
        bc, err = compile_text(text, want_bytes=False)
        if bc is None:
            p("  REFUSED %s: does not parse (%s)" % (rel, err)); bad += 1; continue
        if sha1(text) == row["source_sha1"]:
            p("  unchanged %s" % rel); continue
        row["source_sha1"] = sha1(text)
        row["reason"] = (row["reason"] + "; edited " + datetime.date.today().isoformat()).strip("; ")
        p("  REFRESHED %s %s" % (kom, rel))
    ledger_save(rows)
    if bad:
        sys.exit(4)


def cmd_ledger(check):
    rows = ledger_load()
    cur = ledger_latest(rows)
    tiers, koms = {}, {}
    for r in cur.values():
        tiers[r["tier"]] = tiers.get(r["tier"], 0) + 1
        koms[r["kom"]] = koms.get(r["kom"], 0) + 1
    p("== ledger %s: %d row(s), %d live" % (LEDGER, len(rows), len(cur)))
    p("  live by tier: %s;  by kom: %s;  restored rows: %d" % (dict(sorted(tiers.items())), dict(sorted(koms.items())),
                                                              sum(1 for r in rows if r["state"] == "restored")))
    fails, warns = [], []
    seen_text = set()
    census = {"march-bytecode": 0, "bom-text": 0, "other": 0}
    for kom in kom_dirs_with_lua():
        d = kom_dir(kom)
        man = load_manifest(d)
        for n in lua_names(d):
            data = read(os.path.join(d, n))
            k = kind(data)
            key = (kom, n.lower())
            row = cur.get(key)
            if row and row["tier"] == EXC:
                if sha1(data) != row["reference_sha1"]:
                    fails.append("%s %s: exception row's sha1 no longer matches the file (it changed; re-seed or remove the row)" % (kom, n))
                census["other"] += 1
                continue
            if k == "march-bytecode":
                census["march-bytecode"] += 1
                if row:
                    fails.append("%s %s: ledger row says converted (%s) but the file is bytecode (restored without `restore`?)" % (kom, n, row["tier"]))
            elif k == "bom-text":
                census["bom-text"] += 1
                seen_text.add(key)
                text = strip_bom(data)
                if not row:
                    fails.append("%s %s: BOM text with no ledger row" % (kom, n))
                elif sha1(text) != row["source_sha1"]:
                    fails.append("%s %s: text differs from the ledger sha1 (edited since; run `luasrc.py refresh %s %s`)" % (kom, n, kom, n))
                if str(man.get(n, "?")) != "2":
                    warns.append("%s %s: BOM text but manifest value %r (loaded without decryption)" % (kom, n, man.get(n)))
                if not utf8_ok(text):
                    warns.append("%s %s: not valid UTF-8" % (kom, n))
            else:
                census["other"] += 1
                msg = "%s %s: %s is neither March bytecode nor BOM text and has no exception row (pack %s)" % (
                    kom, n, k, "only warns: manifest 0" if str(man.get(n)) == "0" else "REFUSES it")
                (warns if str(man.get(n)) == "0" else fails).append(msg)
    for key, r in cur.items():
        if r["tier"] != EXC and key not in seen_text and not any(f.startswith("%s %s:" % (r["kom"], r["name"])) for f in fails):
            fails.append("%s %s: ledger row has no BOM-text file in kom_contents" % (r["kom"], r["name"]))
    p("  kom_contents .lua: %s" % census)
    for w in warns[:SHOW]:
        p("  WARN %s" % w)
    for f in fails[:SHOW]:
        p("  FAIL %s" % f)
    if len(warns) > SHOW or len(fails) > SHOW:
        p("  ... %d warn / %d fail in total" % (len(warns), len(fails)))
    if check:
        p("LEDGER CHECK %s: %d FAIL, %d WARN" % ("PASS" if not fails else "FAIL", len(fails), len(warns)))
        if fails:
            sys.exit(1)


# ----------------------------------------------------------------------------------------------
# used by kom.py: pack gate and compile fold
def gate(d, kom, man):
    """Rule 5 on one kom dir. -> (refusals, warnings, notes)."""
    exc = exceptions()
    rows = ledger_latest(ledger_load())
    refusals, warnings, notes = [], [], []
    todo = []
    for n in lua_names(d):
        data = read(os.path.join(d, n))
        k = kind(data)
        algo = str(man.get(n, "?"))
        if (kom, n.lower()) in exc:
            notes.append("%s: ledger exception (kept as %s)" % (n, k))
            continue
        if k == "march-bytecode":
            continue
        if k == "bom-text":
            todo.append((n, data))
            continue
        why = {"june-bytecode": "June 0x82 bytecode (header 1B 4B 4C 82); March's loader cannot read it - run it through ljconv or `luasrc.py convert` it",
               "other-bytecode": "bytecode with an unknown header %s (March reads 1B 4B 4C 81)" % data[:4].hex(),
               "bare-text": "text without the UTF-8 BOM: the binder returns false with no log (KEM_ERROR73 only); `EF BB BF` must lead it",
               "empty": "empty file"}[k]
        if algo == "0":
            warnings.append("%s: manifest \"0\" (user-packed server Lua?) - %s; not validated" % (n, why))
        else:
            refusals.append("%s: %s" % (n, why))
    if todo:
        with cf.ThreadPoolExecutor(max_workers=JOBS) as ex:
            res = list(ex.map(lambda nd: compile_text(strip_bom(nd[1]), want_bytes=False), todo))
        for (n, data), (bc, err) in zip(todo, res):
            if bc is None:
                (warnings if str(man.get(n)) == "0" else refusals).append("%s: BOM text does not parse under luajit (%s)%s" % (
                    n, err, "; manifest \"0\", only a warning" if str(man.get(n)) == "0" else ""))
                continue
            row = rows.get((kom, n.lower()))
            if str(man.get(n)) != "2":
                warnings.append("%s: BOM text but manifest %r - the client would load it without decryption" % (n, man.get(n)))
            if not row:
                warnings.append("%s: BOM text with no ledger row (`luasrc.py ledger --check` fails on it)" % n)
            elif sha1(strip_bom(data)) != row["source_sha1"]:
                warnings.append("%s: differs from its ledger sha1 (edited since conversion; `luasrc.py refresh %s %s` records it)" % (n, kom, n))
            if not utf8_ok(strip_bom(data)):
                warnings.append("%s: not valid UTF-8" % n)
    return refusals, warnings, notes


def fold(d, kom, phase="compile", dry_run=False):
    """kom.py compile: fold every X.src.lua into BOM-source X.lua. -> (folded[(name, how)], refused[(name, why)], skipped[name])"""
    folded, refused, skipped, rows = [], [], [], []
    idx = dir_index(d)
    sides = sorted(r for r in idx.values() if r.lower().endswith(".src.lua"))
    for side_rel in sides:
        name = side_rel[:-8] + ".lua"
        side = os.path.join(d, side_rel)
        lua_rel = idx.get(name.lower())
        lua = os.path.join(d, lua_rel) if lua_rel else os.path.join(d, name)
        name = lua_rel or name
        text = strip_bom(read(side))
        bc, err = compile_text(text)
        if bc is None:
            refused.append((name, "the .src.lua does not compile (%s)" % err))
            continue
        k = kind(read(lua)) if lua_rel else None
        if k == "bom-text" and strip_bom(read(lua)) == text:
            skipped.append(name)             # already folded; the sidecar is just a copy
            continue
        newer = (not lua_rel) or os.path.getmtime(side) > os.path.getmtime(lua)
        if k == "march-bytecode" and bc == read(lua):
            tier, how, ref = "T0", "T0: compiles byte-identical to the bytecode", read(lua)
        elif newer:
            tier = "NEW" if not lua_rel else "EDIT"
            how = ("new file" if not lua_rel else "intended edit: .src.lua is newer than X.lua (no equivalence claimed; tier EDIT)")
            ref = read(lua) if lua_rel else b""
        else:
            refused.append((name, "the .src.lua is OLDER than X.lua (%s) and is not T0 against it - a stale sidecar beside a file that was converted or "
                                  "edited another way; delete the sidecar, or touch it if it really is the intended text" % (k or "missing")))
            continue
        if dry_run:
            folded.append((name, how))
            continue
        if lua_rel:
            snapshot(kom, d, name)
        write_source(d, name, text)
        os.remove(side)
        rows.append(new_row(kom, name, "march", "staged bytecode" if ref else "-", sha1(ref) if ref else "", "src.lua", tier, sha1(text), phase,
                            "kom.py compile fold (%s)" % ("T0" if tier == "T0" else how.split(":")[0])))
        folded.append((name, how))
    if rows:
        ledger_save(ledger_load() + rows)
    return folded, refused, skipped


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("find"); s.add_argument("kom"); s.add_argument("names", nargs="+")
    s = sub.add_parser("convert")
    s.add_argument("kom", nargs="?"); s.add_argument("names", nargs="*")
    s.add_argument("--all", action="store_true", help="every kom dir that has .lua")
    s.add_argument("--dry-run", action="store_true")
    s.add_argument("--origin", choices=("march", "june"), default="march")
    s.add_argument("--phase", default="convert")
    s.add_argument("--reason", default="convert (format change only)")
    s.add_argument("--jobs", type=int, default=JOBS)
    s.add_argument("--plan-out", help="write the per-file verdicts as a TSV")
    s = sub.add_parser("restore"); s.add_argument("kom"); s.add_argument("names", nargs="+")
    s = sub.add_parser("refresh"); s.add_argument("kom"); s.add_argument("names", nargs="+")
    s = sub.add_parser("ledger"); s.add_argument("--check", action="store_true")
    a = ap.parse_args()
    if a.cmd == "ledger":
        cmd_ledger(a.check)
    elif a.cmd == "convert":
        if a.all and a.kom:
            a.names = [a.kom] + a.names
            a.kom = None
        if not a.all and not a.kom:
            die(2, "convert needs a kom (data036) or --all")
        if a.kom:
            a.kom = norm(a.kom)
        if not a.dry_run and a.all and not a.names:
            die(2, "convert --all without --dry-run would rewrite every provable file in every kom; name the files, or run one kom at a time")
        cmd_convert(a)
    else:
        kom = norm(a.kom)
        sys.exit({"find": cmd_find, "restore": cmd_restore, "refresh": cmd_refresh}[a.cmd](kom, a.names) or 0)


if __name__ == "__main__":
    main()
