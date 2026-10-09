#!/usr/bin/env python3
"""Keep March client .lua as BOM + UTF-8 source (not bytecode), and prove each conversion.  See LUA_SOURCE_PHASES.md.

    python tools/luasrc.py find data036 X.lua               # exact sources for one file, compile-compared
    python tools/luasrc.py convert data036 X.lua [Y.lua..]  # bytecode X.lua -> BOM + source (T0 or T1 proof only)
    python tools/luasrc.py convert data036 'DLG_*.lua'      # names may be globs
    python tools/luasrc.py convert data036 --dry-run        # whole kom dir: what WOULD convert, writes nothing
    python tools/luasrc.py convert --all --dry-run [--no-decompile]   # every kom dir that has .lua (T0 only: fast)
    python tools/luasrc.py restore data036 X.lua            # put the snapshot bytes back, ledger row -> restored
    python tools/luasrc.py refresh data036 X.lua            # re-record the sha after a deliberate text edit
    python tools/luasrc.py ledger [--check]                 # census; --check = consistency (exit 1 on any FAIL)
  L2:
    python tools/luasrc.py decompile X.lua [-o out.lua]       # March 0x81 or June 0x82 -> BOM source (not proven by itself)
    python tools/luasrc.py check SRC.lua REF.lua [--ref june]  # T0 / T1 / FAIL against shipped (or ljconv'd June) bytecode
    python tools/luasrc.py t2 SRC.lua REF.lua [--out DIR]      # T2 report: globals in raw next order, mock environment
    python tools/luasrc.py selftest                            # seeded edits T1 must reject (and controls it must accept)
    python tools/luasrc.py calibrate [--sets march-t0,june,class] [--seed X --exclude TSV]

Proof tiers (Rule 1): T0 = the source compiles BYTE-IDENTICAL to the shipped bytecode (luajit -b, mar-2014 header).
T1 = identical per prototype, constants equal by value and slot order ignored (tools/ljbc.py). T2 = behaviour only: `t2`
reports, it never passes a file. T3 = unproven, the bytecode ships. Sources tried, in Rule 3's order: the sibling X.src.lua,
a studio original from ClientScript\\ or _ClientScript\\ (every copy of the name is tried), then the ljd decompile (with the
template-layout tidy-up, tools/ljsrc.py), which converts only at T0 or T1. Conversion changes FORMAT only (Rule 4).
A table constant passes T1 with the same key/value pairs in its own table; their storage order is ignored (user, 2026-10-09).

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


def compile_text(text, want_bytes=True, debug=False):
    """Compile source bytes (BOM already stripped). Returns (bytecode|None, error message). Stripped (flags 02) like the shipped
    files; debug=True keeps line info (`-bg`, used only to map a T1 difference back to a source line)."""
    if not os.path.isfile(LUAJIT):
        die(1, "luajit.exe not found at %s" % LUAJIT)
    os.makedirs(TMP, exist_ok=True)
    stem = os.path.join(TMP, uuid.uuid4().hex)
    src, out = stem + ".lua", stem + ".bin"
    try:
        with open(src, "wb") as f:
            f.write(text)
        r = subprocess.run([LUAJIT, "-bg" if debug else "-b", src, out if want_bytes else "NUL"], stdin=subprocess.DEVNULL, capture_output=True,
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
def classify_file(d, name, man, decompile=True):
    """What `convert` would do with one staged file: (state, detail, ref, label, path, text). Rule 3's order: sibling
    .src.lua, studio copies (T0), then the decompile (T0 or T1, proven by the strict T1 check; L2)."""
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
    if decompile:
        tier, text, dwhy = prove_decompile(data)
        if text is not None:
            return "decompiled", tier, data, "decompile", None, text
        why += "; decompile: " + dwhy
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
    tier = st if st in ("T0", "T1") else "T0"
    return new_row(kom, name, a.origin, "shipped March bytecode" if a.origin == "march" else "ljconv(June bytecode)", sha1(ref),
                   {"src.lua": "src.lua", "ClientScript": "ClientScript", "_ClientScript": "_ClientScript"}.get(label, label), tier,
                   sha1(text), a.phase, a.reason + folded + ("; ljd decompile proven %s (L2)" % tier if label == "decompile" else ""))


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
        futs = [ex.submit(classify_file, d, n, man, not a.no_decompile) for kom, d, n, man in work]
        for (kom, d, n, man), fu in zip(work, futs):
            results.append(((kom, d, n), fu.result()))
    refused_explicit = 0
    rows, tsv = [], ["kom\tname\tstate\tsource\tdetail"]
    by_label, by_kom = {}, {}
    for (kom, d, n), (st, detail, ref, label, path, text) in results:
        tsv.append("\t".join([kom, n, st, label or "", detail.replace("\t", " ")]))
        if st in ("t0", "decompiled"):
            tier = "T0" if st == "t0" else detail
            t0_list.append((kom, n))
            key = label if st == "t0" else "decompile %s" % tier
            by_label[key] = by_label.get(key, 0) + 1
            by_kom[kom] = by_kom.get(kom, 0) + 1
            if not a.dry_run:
                rows.append(convert_one(kom, d, n, tier, label, path, text, ref, a))
            if explicit or len(work) <= 20:
                p("  %s %-8s %s  [%s, %s%s]" % ("would convert" if a.dry_run else "CONVERTED", kom, n, label, tier,
                                              "" if a.dry_run else ", snapshot + ledger"))
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
    p("  proven by source: %s;  by kom: %s" % (by_label or "none", dict(sorted(by_kom.items())) or "none"))
    p("  unproven (no T0 source%s): %d  %s" % (" and decompile not tried: --no-decompile" if a.no_decompile else ", and the decompile is not T1",
                                             len(noproof), dict(sorted(nokom.items())) if nokom else ""))
    p("  skipped: already source %d, not March bytecode %d, manifest not \"2\" %d" % (
        skipped["already-source"], skipped["not-march-bytecode"], skipped["manifest"]))
    if a.dry_run:
        p("CONVERT DRY RUN: %d convertible (T0/T1), %d unproven, %d skipped; nothing changed." % (len(t0_list), len(noproof), sum(skipped.values())))
    else:
        p("CONVERT OK: %d converted (T0/T1), %d refused/unproven, %d skipped. Next: kom.py pack %s   (then ask before `copy`)" % (
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


# ----------------------------------------------------------------------------------------------
# L2: decompile, the T1 check, calibration (LUA_SOURCE_PHASES.md Phase L2)
RE_DIR = os.path.join(TRUNK, "june2017_re")
LJD_FIXED = os.path.join(RE_DIR, "ljd_fixed.py")
STD_BC = b"\x1bLJ\x01"
DECOMPILE_TIMEOUT = int(os.environ.get("X2_LJD_TIMEOUT", "900"))
CALIBRATION_TSV = os.path.join(RE_DIR, "lua_source_calibration.tsv")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ljbc  # noqa: E402  (tools/ljbc.py: the dump reader and the T1 comparison)
import ljsrc  # noqa: E402  (tools/ljsrc.py: Lua tokenizer, constructors, the template-layout tidy-up)

_ljconv = []


def ljconv_mod():
    """june2017_re/ljconv.py with its opcode table loaded (once)."""
    if not _ljconv:
        if RE_DIR not in sys.path:
            sys.path.insert(0, RE_DIR)
        import ljconv as m
        _ljconv.append((m, m.load_table(m.TABLE_TSV)))
    return _ljconv[0]


def reference_of(data, ref="auto"):
    """-> (reference 0x81 bytes, label). --ref march: the shipped 0x81 file itself; --ref june: ljconv(June 0x82 bytecode)."""
    k = kind(data)
    if ref == "auto":
        ref = "june" if k == "june-bytecode" else "march"
    if ref == "march":
        if k != "march-bytecode":
            raise ValueError("--ref march needs shipped March bytecode (1B 4B 4C 81); the reference is %s" % k)
        return data, "shipped March bytecode"
    if k != "june-bytecode":
        raise ValueError("--ref june needs June bytecode (1B 4B 4C 82); the reference is %s" % k)
    m, table = ljconv_mod()
    return m.transcode(data, table)[0], "ljconv(June bytecode)"


def decompile_bc(data, timeout=DECOMPILE_TIMEOUT, repair=True):
    """Bytecode -> (source bytes without BOM, or None; error).
    March 0x81: the header is swapped to stock 1B 4C 4A 01. June 0x82: ljconv.transcode_stock, i.e. the bytes ljd_jun2017.py
    feeds ljd from elstools' .std.luac with its FIX slots corrected (`ljconv.py --verify-stock` proves them equal), with no
    .std.luac needed. Both then go through june2017_re/ljd_fixed.py (the vendored ljd; the slotworks fix is baked into both
    vendored copies, sha256 2235d0a1...)."""
    k = kind(data)
    if k == "march-bytecode":
        stock = STD_BC + data[4:]
    elif k == "june-bytecode":
        m, table = ljconv_mod()
        stock = m.transcode_stock(data, table)[0]
    else:
        return None, "not bytecode (%s)" % k
    os.makedirs(TMP, exist_ok=True)
    stem = os.path.join(TMP, uuid.uuid4().hex)
    src, out = stem + ".luac", stem + ".dec.lua"
    try:
        with open(src, "wb") as f:
            f.write(stock)
        r = subprocess.run([sys.executable, LJD_FIXED, "-f", src, "-o", out], stdin=subprocess.DEVNULL, capture_output=True,
                           cwd=TRUNK, timeout=timeout)
        err = r.stderr.decode("utf-8", "replace")
        if r.returncode != 0 or not os.path.isfile(out):
            last = [ln for ln in err.strip().splitlines() if ln.strip()]
            return None, "ljd failed (rc %d): %s" % (r.returncode, last[-1][:300] if last else "no message")
        if "slotworks fix active" not in err:
            return None, "ljd_fixed.py ran without reporting the slotworks fix active"
        text = read(out)
        if repair:
            text = repair_layout(text, data)[0]
        return text, ""
    except subprocess.TimeoutExpired:
        return None, "ljd timed out after %ds" % timeout
    finally:
        for f in (src, out):
            try:
                os.remove(f)
            except OSError:
                pass


def repair_layout(text, data):
    """Best-effort: reorder constant constructor fields so the parsed templates store their pairs in the reference's order
    (tools/ljsrc.py). -> (text, notes). Runs only on a text that passes T1 and returns one that still does."""
    try:
        ref = reference_of(data)[0]
    except ValueError:
        return text, ["no reference"]
    try:
        return ljsrc.repair_layout(text, ref, lambda t, dbg: compile_text(t, True, dbg), ljbc.t1_bytes)
    except (ljsrc.LexError, ljbc.DumpError, RuntimeError, subprocess.TimeoutExpired) as e:
        return text, ["layout repair skipped: %s" % e]


def t1_tier(text, ref):
    """Source bytes (no BOM) against a 0x81 reference. -> (tier, ljbc.T1Result or None, error).
    tier: T0 (byte-identical) | T1 | FAIL (T1 differences) | NOCOMPILE."""
    bc, err = compile_text(text)
    if bc is None:
        return "NOCOMPILE", None, err
    if bc == ref:
        return "T0", ljbc.T1Result(), ""
    r = ljbc.t1_bytes(bc, ref)
    return ("T1" if r.ok else "FAIL"), r, ""


def prove_decompile(ref):
    """Rule 3's last source: decompile the reference and prove the text at T1. -> (tier, text or None, why)."""
    text, err = decompile_bc(ref)
    if text is None:
        return "DECOMPILE-FAIL", None, err
    tier, r, err = t1_tier(text, ref)
    if tier == "NOCOMPILE":
        return tier, None, "the decompile does not compile (%s)" % err
    if tier in ("T0", "T1"):
        return tier, text, "%s: decompile proven (%d prototype(s), %d instruction(s))" % (tier, r.protos, r.ins)
    first = ("%s: %s" % r.diffs[0])[:240] if r.diffs else "?"
    return "FAIL", None, "decompile is not T1: %d difference(s); first %s" % (r.total, first)


def show_t1(r, n=12):
    for path, msg in r.diffs[:n]:
        p("    %s: %s" % (path, msg))
    if r.total > n:
        p("    ... %d difference(s) in total" % r.total)


def bytecode_arg(arg):
    """A path, or KOM/NAME (data038/SQUARE_GM_Ariel.lua): the staged file, or its snapshot once the file was converted."""
    if os.path.isfile(arg):
        return read(arg)
    m = re.fullmatch(r"([^/\\:]+)[/\\:](.+)", arg)
    kom = None
    if m and re.fullmatch(r"(?i)(?:data)?i?\d+(?:_kom)?", m.group(1)):
        kom = norm(m.group(1))
    if kom:
        for path in (os.path.join(kom_dir(kom), m.group(2)), os.path.join(snapshot_dir(kom), m.group(2))):
            if os.path.isfile(path) and kind(read(path)) in ("march-bytecode", "june-bytecode"):
                return read(path)
    die(2, "no such bytecode file: %s (give a path, or KOM/NAME like data038/SQUARE_GM_Ariel.lua)" % arg)


def cmd_decompile(a):
    data = bytecode_arg(a.bytecode)
    text, err = decompile_bc(data, a.timeout)
    if text is None:
        die(1, "DECOMPILE FAILED %s: %s" % (a.bytecode, err))
    out = a.out or os.path.join(TRUNK, ".build", "luasrc_dec", os.path.basename(a.bytecode.replace(":", "/")))
    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    write_atomic(out, BOM + text)
    p("DECOMPILED %s -> %s (%d bytes, BOM + UTF-8). Not proven: run `luasrc.py check %s %s`" % (
        a.bytecode, out, len(text) + 3, out, a.bytecode))


def cmd_check(a):
    src = read(a.source)
    if kind(src) not in ("bom-text", "bare-text"):
        die(2, "%s is not source text (%s)" % (a.source, kind(src)))
    try:
        ref, label = reference_of(bytecode_arg(a.reference), a.ref)
    except ValueError as e:
        die(2, str(e))
    tier, r, err = t1_tier(strip_bom(src), ref)
    p("== check %s against %s (%s)" % (a.source, a.reference, label))
    if tier == "NOCOMPILE":
        die(1, "CHECK FAIL: the source does not compile (%s)" % err)
    if tier == "FAIL":
        show_t1(r, a.show)
        die(1, "CHECK FAIL: not T1 - %d difference(s) over %d prototype(s), %d instruction(s)" % (r.total, r.protos, r.ins))
    p("CHECK %s: %s" % (tier, "byte-identical" if tier == "T0" else "%d prototype(s), %d instruction(s) equal; constants equal by value%s" % (
        r.protos, r.ins, "; %d table constant(s) hold the same pairs in another storage order (pairs() may walk them "
                         "differently; ignored since 2026-10-09)" % r.layout if r.layout else "")))


# ---- T2 report (reports only; it never passes a file)
T2_HARNESS = r"""
local MAXINS = tonumber(arg[3]) or 50000000
local function rd(p) local f = assert(io.open(p, "rb")); local s = f:read("*a"); f:close(); return s end
local STUBS = setmetatable({}, { __mode = "k" })
local function stub(name)
  local s = {}
  STUBS[s] = name
  return setmetatable(s, {
    __index = function(t, k) local c = stub(name .. "." .. tostring(k)); rawset(t, k, c); return c end,
    __call = function(_, ...) return stub(name .. "()") end,
    __add = function() return stub(name .. "+") end, __sub = function() return stub(name .. "-") end,
    __mul = function() return stub(name .. "*") end, __div = function() return stub(name .. "/") end,
    __mod = function() return stub(name .. "%") end, __unm = function() return stub("-" .. name) end,
    __concat = function() return stub(name .. "..") end, __len = function() return 0 end,
  })
end
local function q(v)
  local t = type(v)
  if t == "string" then return string.format("%q", v) end
  if t == "number" then return string.format("%.17g", v) end
  if t == "table" and STUBS[v] then return "<" .. STUBS[v] .. ">" end
  if t == "table" or t == "function" or t == "userdata" or t == "thread" then return "<" .. t .. ">" end
  return tostring(v)
end
local function dump(out, v, path, seen, depth)
  if type(v) ~= "table" or STUBS[v] then return end
  if seen[v] then out[#out + 1] = path .. " = <same as " .. seen[v] .. ">"; return end
  seen[v] = path
  if depth > 40 then out[#out + 1] = path .. " = <too deep>"; return end
  local k, val = next(v)
  while k ~= nil do
    local p = path .. "[" .. q(k) .. "]"
    out[#out + 1] = p .. " = " .. q(val)
    dump(out, val, p, seen, depth + 1)
    k, val = next(v, k)
  end
end
local s = rd(arg[1])
if s:sub(1, 3) == "\239\187\191" then s = s:sub(4) end
local f, err = loadstring(s, "=chunk")
if not f then io.write("LOAD ERROR ", tostring(err), "\n"); return end
local env = setmetatable({}, { __index = function(t, k) return stub(tostring(k)) end })
setfenv(f, env)
local n = 0
debug.sethook(function() n = n + 1; if n * 1000 > MAXINS then error("instruction budget exhausted", 0) end end, "", 1000)
local ok, e = pcall(f)
debug.sethook()
local out = {}
if not ok then out[1] = "MAIN ERROR " .. tostring(e) end
dump(out, env, "_G", {}, 0)
local fh = assert(io.open(arg[2], "wb")); fh:write(table.concat(out, "\n"), "\n"); fh:close()
io.write("OK ", #out, "\n")
"""


def t2_dump(chunk_bytes, tag):
    """Run one chunk (bytecode or BOM/plain text) in the mock environment. -> (lines, error)."""
    os.makedirs(TMP, exist_ok=True)
    stem = os.path.join(TMP, uuid.uuid4().hex + "_" + tag)
    src, out = stem + ".chunk", stem + ".t2"
    try:
        with open(src, "wb") as f:
            f.write(chunk_bytes)
        r = subprocess.run([LUAJIT, "-", src, out], input=T2_HARNESS.encode(), capture_output=True, cwd=LUAJIT_DIR, env=_env(),
                           timeout=600)
        msg = r.stdout.decode("utf-8", "replace").strip()
        if not msg.startswith("OK"):
            return None, (msg or r.stderr.decode("utf-8", "replace").strip())[:300]
        return read(out).decode("utf-8", "replace").splitlines(), ""
    except subprocess.TimeoutExpired:
        return None, "mock run timed out"
    finally:
        for f in (src, out):
            try:
                os.remove(f)
            except OSError:
                pass


def t2_compare(a, b):
    """-> 'identical' | 'order only' (same lines, other raw next order) | 'content: N line(s) differ'."""
    if a == b:
        return "identical"
    if sorted(a) == sorted(b):
        return "order only"
    sa, sb = set(a), set(b)
    return "content: %d line(s) only in the first, %d only in the second" % (len(sa - sb), len(sb - sa))


def t2_report(text, ref):
    """R = reference bytecode, B = the source compiled to bytecode, S = the source loaded as text (what the client runs
    after conversion). -> dict of the three comparisons plus the dumps."""
    bc, err = compile_text(text)
    dumps = {"R": t2_dump(ref, "R"), "S": t2_dump(BOM + text, "S")}
    dumps["B"] = t2_dump(bc, "B") if bc is not None else (None, "does not compile: %s" % err)
    res = {}
    for x, y, what in (("R", "B", "bytecode vs bytecode"), ("B", "S", "loader effect (T0 files show it too)"),
                       ("R", "S", "what conversion changes")):
        (da, ea), (db, eb) = dumps[x], dumps[y]
        res[x + y] = (what, t2_compare(da, db) if da is not None and db is not None else "not run: %s" % (ea or eb))
    return res, dumps


def cmd_t2(a):
    src = read(a.source)
    try:
        ref, label = reference_of(bytecode_arg(a.reference), a.ref)
    except ValueError as e:
        die(2, str(e))
    res, dumps = t2_report(strip_bom(src), ref)
    p("== T2 report %s against %s (%s). Reports only: T2 never passes a file (Rule 1)." % (a.source, a.reference, label))
    for key in ("RB", "BS", "RS"):
        what, verdict = res[key]
        p("  %s  %-40s %s" % (key, what, verdict))
    for x in ("R", "S"):
        lines, err = dumps[x]
        if lines and lines[0].startswith("MAIN ERROR"):
            p("  note: %s main chunk stopped: %s" % (x, lines[0][11:200]))
    if a.out:
        os.makedirs(a.out, exist_ok=True)
        for x, (lines, err) in dumps.items():
            with open(os.path.join(a.out, "%s.t2.txt" % x), "w", encoding="utf-8", newline="\n") as f:
                f.write("\n".join(lines) + "\n" if lines is not None else "not run: %s\n" % err)
        p("  dumps written to %s (R/B/S.t2.txt)" % a.out)
    p("Blind spots: function bodies are not run (only the main chunk), engine globals are stubs, and a main chunk that "
      "errors is compared up to the error.")


# ---- mutation self-test (LUA_SOURCE_PHASES.md L2 step 5): every seeded change must fail T1, every control must pass
SELFTEST_SEEDS = [("data038", "SQUARE_GM_Ariel.lua"), ("data037", "Soldier_Benders_Thief_Arcade.lua"),
                  ("data037", "Gliter_Great_Rock.lua"), ("data038", "SQUARE_Unit.lua"), ("data036", "IceHeaterPreView.lua"),
                  ("data217", "DLG_Map_Local_Ranox.lua"), ("data035", "Dungeon_El_Forest_Gate_Normal.lua"),
                  ("datai015", "UIEffectSet_INT.lua"),
                  # ljd prints stripped locals as slotN globals (no `local`), so the local-to-global seeds mutate proven T0
                  # studio sources that declare locals.
                  ("data037", "ANCIENT_CRAB.lua", "t0"), ("data035", "Dungeon_Arcade_Ruben.lua", "t0"),
                  # the only March files with upvalues; no provable text, so only their bytecode seeds run
                  ("data037", "Wally9th_Armor.lua", "bc")]


def seed_reference(kom, name):
    """The shipped bytecode of a seed: the staged file, or its snapshot once the file was converted."""
    for path in (os.path.join(kom_dir(kom), name), os.path.join(snapshot_dir(kom), name)):
        if os.path.isfile(path) and kind(read(path)) == "march-bytecode":
            return read(path)
    return None


def _selftest_seed(item):
    kom, name, mode, per_kind = item
    ref = seed_reference(kom, name)
    res = {"seed": "%s %s%s" % (kom, name, {"t0": " (T0 source)", "bc": " (bytecode seeds only)"}.get(mode, "")), "rows": [],
           "error": ""}
    if ref is None:
        res["error"] = "no March bytecode for the seed"
        return res
    text = None
    if mode == "t0":
        label, _path, text, _tried = find_t0(kom_dir(kom), name, ref)
        if not label:
            res["error"] = "no T0 source"
            return res
    elif mode == "decompile":
        text, err = decompile_bc(ref)
        if text is None:
            res["error"] = "decompile failed: %s" % err
            return res
    if text is not None:
        tier, r, err = t1_tier(text, ref)
        if tier not in ("T0", "T1"):
            res["error"] = "the seed's text is not T1 (%s); pick another seed" % (err or "%d difference(s)" % r.total)
            return res
        base, _ = compile_text(text)
    for kind_, where, mtext in (ljsrc.mutants(text, per_kind) if text is not None else ()):
        bc, err = compile_text(mtext)
        if bc is None:
            res["rows"].append((kind_, where, "fail", "invalid", "does not compile"))
            continue
        if bc == base:
            res["rows"].append((kind_, where, "fail", "equivalent", "bytecode unchanged by the edit"))
            continue
        rr = ljbc.t1_bytes(bc, ref)
        res["rows"].append((kind_, where, "fail", "pass" if rr.ok else "fail", "" if rr.ok else "%s: %s" % rr.diffs[0]))
    for kind_, expect, mbytes, against in ljbc.dump_mutants(ref):
        rr = ljbc.t1_bytes(mbytes, against)
        res["rows"].append(("bc:" + kind_, "bytecode", expect, "pass" if rr.ok else "fail", "" if rr.ok else "%s: %s" % rr.diffs[0]))
    return res


def cmd_selftest(a):
    seeds = [(x[0], x[1], x[2] if len(x) > 2 else "decompile") for x in SELFTEST_SEEDS]
    for spec in a.seed or []:
        k, _, n = spec.partition(":")
        mode = "decompile"
        if n.endswith((":t0", ":bc")):
            n, mode = n[:-3], n[-2:]
        seeds.append((norm(k), n, mode))
    p("== selftest: %d seed file(s); source mutants (%d per kind per seed) + bytecode seeds" % (len(seeds), a.per_kind))
    with cf.ThreadPoolExecutor(max_workers=a.jobs) as ex:
        results = list(ex.map(_selftest_seed, [(k, n, m, a.per_kind) for k, n, m in seeds]))
    tally, misses, bad_seeds = {}, [], 0
    for res in results:
        if res["error"]:
            bad_seeds += 1
            p("  SEED SKIPPED %s: %s" % (res["seed"], res["error"]))
            continue
        for kind_, where, expect, got, why in res["rows"]:
            t = tally.setdefault(kind_, {"valid": 0, "ok": 0, "invalid": 0, "equivalent": 0})
            if got in ("invalid", "equivalent"):
                t[got] += 1
                continue
            t["valid"] += 1
            if got == expect:
                t["ok"] += 1
            else:
                misses.append("%s %s %s: expected %s, T1 said %s" % (res["seed"], kind_, where, expect, got))
    p("  %-58s %6s %6s %8s %10s" % ("kind", "valid", "right", "invalid", "equivalent"))
    for kind_ in sorted(tally):
        t = tally[kind_]
        p("  %-58s %6d %6d %8d %10d" % (kind_, t["valid"], t["ok"], t["invalid"], t["equivalent"]))
    for m in misses:
        p("  MISS %s" % m)
    total = sum(t["valid"] for t in tally.values())
    right = sum(t["ok"] for t in tally.values())
    empty = [k for k, t in tally.items() if not t["valid"]]
    want = {"number-constant", "string-constant", "string-byte-flip", "drop-statement", "operator", "constructor-reorder",
            "local-to-global", "swap-string-constants", "swap-number-constants"}
    absent = sorted(want - {k for k, t in tally.items() if t["valid"]})
    ok = not misses and not absent and not bad_seeds
    p("SELFTEST %s: %d/%d seeded edits judged right (%.1f%%)%s%s" % (
        "PASS" if ok else "FAIL", right, total, 100.0 * right / max(1, total),
        "; kinds with no valid mutant: %s" % ", ".join(absent) if absent else "",
        "; %d seed(s) skipped" % bad_seeds if bad_seeds else ""))
    if not ok:
        sys.exit(1)


# ---- calibration
CLASS_SCRIPTS = ["Elsword_SwordMan.lua", "Arme_VioletMage.lua", "Lire_ElvenRanger.lua", "RAVEN_FIGHTER.lua", "Eve_Nasod.lua",
                 "Chung_Iron_Cannon.lua", "Ara_Martial_Artist.lua", "ELESIS_KNIGHT.lua", "Add_NasodRuler.lua"]
JUNE_LUA_DIR = os.path.join(RE_DIR, "june_lua")
_SLOTG = re.compile(r'^"(slot|uv)\d+"$')


def _calib_pick(per_dir, seed="L2", exclude=()):
    """Known-T0 March files: [(set, kom, name, ref bytes, exact label)], a deterministic spread over every kom dir that has
    any (the largest file of each dir always included), plus data217's converted files from their snapshots."""
    picks = []
    for kom in kom_dirs_with_lua():
        d = kom_dir(kom)
        man = load_manifest(d)
        names = [n for n in lua_names(d) if str(man.get(n)) == "2" and (kom, n) not in exclude]
        cands = [n for n in names if kind(read(os.path.join(d, n))) == "march-bytecode" and candidates(d, n)]
        if not cands:
            continue
        cands.sort(key=lambda n: hashlib.sha1((seed + kom + n.lower()).encode()).hexdigest())
        biggest = max(cands, key=lambda n: os.path.getsize(os.path.join(d, n)))
        order = ([biggest] if seed == "L2" else []) + [n for n in cands if n != biggest or seed != "L2"]
        got = 0
        for n in order:
            if got >= per_dir.get(kom, per_dir.get("*", 0)):
                break
            ref = read(os.path.join(d, n))
            label, _path, _text, _tried = find_t0(d, n, ref)
            if label:
                picks.append(("march-t0", kom, n, ref, label))
                got += 1
    if seed == "L2":
        for r in ledger_latest(ledger_load()).values():
            if r["kom"] == "data217" and r["tier"] == "T0" and ("data217", r["name"]) not in exclude:
                snap = os.path.join(snapshot_dir("data217"), r["name"])
                if os.path.isfile(snap):
                    picks.append(("march-t0", "data217", r["name"], read(snap), "ledger T0 (%s)" % r["source"]))
    return picks


def _auto_cause(row, r):
    """A first-pass cause from the T1 differences; every miss is then read and confirmed in the calibration TSV."""
    if row["verdict"] == "DECOMPILE-FAIL":
        return "ljd: " + ("timeout" if "timed out" in row["first_diff"] else "crash (%s)" % row["first_diff"][:80])
    if row["verdict"] == "NOCOMPILE":
        return "ljd printer: invalid Lua (%s)" % row["first_diff"][:80]
    if r is None:
        return ""
    texts = " ".join(m for _p, m in r.diffs)
    if re.search(r'only in source \{[^}]*"slot\d+"', texts):
        return "ljd: a local printed as a slotN global (no `local`; stripped bytecode has no names)"
    if re.search(r'"uv\d+"', texts):
        return "ljd: an upvalue printed as a uvN global"
    if "0x4330" in texts:
        return "ljd: a multi-value tail (TSETM count) printed differently"
    return "other: " + (r.diffs[0][1][:100] if r.diffs else "?")


def _calib_one(item):
    set_, kom, name, data, label = item
    t = datetime.datetime.now()
    row = {"set": set_, "kom": kom, "name": name, "bytes": len(data), "exact_source": label}
    try:
        ref = reference_of(data)[0]
    except ValueError as e:
        row.update(verdict="NO-REFERENCE", first_diff=str(e), decompile_s="0")
        return row
    text, err = decompile_bc(data)
    row["decompile_s"] = "%.1f" % (datetime.datetime.now() - t).total_seconds()
    r = None
    if text is None:
        row.update(verdict="DECOMPILE-FAIL", first_diff=err)
    else:
        tier, r, err = t1_tier(text, ref)
        if tier == "NOCOMPILE":
            row.update(verdict="NOCOMPILE", first_diff=err)
        else:
            verdict = tier
            row.update(verdict=verdict, diffs=r.total, reordered_tables=r.layout,
                       first_diff=("%s: %s" % r.diffs[0])[:300] if r.diffs else "")
    if row["verdict"] not in ("T0", "T1"):
        row["cause"] = _auto_cause(row, r)
    return row


CALIB_COLS = ["set", "kom", "name", "bytes", "exact_source", "decompile_s", "verdict", "diffs", "reordered_tables", "first_diff", "cause"]


def cmd_calibrate(a):
    sets = set(a.sets.split(","))
    per = {"*": a.per_dir}
    for spec in a.quota or []:
        k, _, n = spec.partition("=")
        per[norm(k)] = int(n)
    exclude = set()
    for path in a.exclude or []:
        for ln in read(path).decode("utf-8").splitlines():
            if ln.startswith(("#", "set\t")):
                continue
            c = ln.split("\t")
            if len(c) > 2:
                exclude.add((c[1], c[2]))
    picks = []
    if "march-t0" in sets:
        picks += _calib_pick(per, a.seed, exclude)
    if "march-nogt" in sets:
        # kom dirs with no exact source at all: decompile + T1 verdicts without ground truth
        t0_dirs = {x[1] for x in _calib_pick({"*": 1}, "L2")}
        for kom in kom_dirs_with_lua():
            if kom in t0_dirs:
                continue
            d = kom_dir(kom)
            for n in lua_names(d):
                data = read(os.path.join(d, n))
                if kind(data) == "march-bytecode":
                    picks.append(("march-nogt", kom, n, data, "none (no exact source in this dir)"))
    if "june" in sets:
        for f in sorted(os.listdir(JUNE_LUA_DIR)):
            if f.lower().endswith(".lua") and not f.lower().endswith(".src.lua"):
                picks.append(("june", "june_lua", f, read(os.path.join(JUNE_LUA_DIR, f)), "ljconv reference"))
    if "class" in sets:
        for n in CLASS_SCRIPTS:
            picks.append(("class", "data036", n, read(os.path.join(kom_dir("data036"), n)), "none (bytecode only)"))
    p("== calibrate: %d file(s) (%s); decompiling with %d job(s)" % (
        len(picks), ", ".join("%s %d" % (s_, sum(1 for x in picks if x[0] == s_)) for s_ in sorted({x[0] for x in picks})), a.jobs))
    rows = []
    with cf.ThreadPoolExecutor(max_workers=a.jobs) as ex:
        for row in ex.map(_calib_one, picks):
            rows.append(row)
            p("  %-16s %-6s %-8s %s%s" % (row["verdict"], row["set"], row["kom"], row["name"],
                                          "  -- " + row.get("cause", "") if row.get("cause") else ""))
    out = a.out or os.path.join(TRUNK, ".build", "luasrc_calibration.tsv")
    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    with open(out, "w", encoding="utf-8", newline="\n") as f:
        f.write("# luasrc.py calibrate --sets %s --seed %s --per-dir %d %s(%s)\n" % (
            a.sets, a.seed, a.per_dir, "".join("--quota %s " % q for q in a.quota or []), datetime.date.today().isoformat()))
        f.write("\t".join(CALIB_COLS) + "\n")
        for r in rows:
            f.write("\t".join(str(r.get(c, "")).replace("\t", " ").replace("\n", " ") for c in CALIB_COLS) + "\n")
    for s_ in sorted({r["set"] for r in rows}):
        tally = {}
        for r in rows:
            if r["set"] == s_:
                tally[r["verdict"]] = tally.get(r["verdict"], 0) + 1
        p("CALIBRATE %s: %s" % (s_, dict(sorted(tally.items()))))
    p("  -> %s (every non-T0/T1 row carries a first-pass cause; confirm each by reading it)" % out)


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
    s.add_argument("--no-decompile", action="store_true", help="T0 sources only (skip the slower decompile + T1 tier)")
    s = sub.add_parser("restore"); s.add_argument("kom"); s.add_argument("names", nargs="+")
    s = sub.add_parser("refresh"); s.add_argument("kom"); s.add_argument("names", nargs="+")
    s = sub.add_parser("ledger"); s.add_argument("--check", action="store_true")
    s = sub.add_parser("decompile"); s.add_argument("bytecode"); s.add_argument("-o", "--out")
    s.add_argument("--timeout", type=int, default=DECOMPILE_TIMEOUT)
    s = sub.add_parser("check"); s.add_argument("source"); s.add_argument("reference")
    s.add_argument("--ref", choices=("auto", "march", "june"), default="auto"); s.add_argument("--show", type=int, default=12)
    s = sub.add_parser("t2"); s.add_argument("source"); s.add_argument("reference")
    s.add_argument("--ref", choices=("auto", "march", "june"), default="auto"); s.add_argument("--out")
    s = sub.add_parser("selftest"); s.add_argument("--per-kind", type=int, default=3)
    s.add_argument("--seed", action="append", metavar="KOM:NAME[:t0]", help="add a seed file (its decompile must be T1; "
                   ":t0 = mutate its proven T0 studio source instead)")
    s.add_argument("--jobs", type=int, default=JOBS)
    s = sub.add_parser("calibrate"); s.add_argument("--per-dir", type=int, default=20)
    s.add_argument("--quota", action="append", metavar="KOM=N"); s.add_argument("--jobs", type=int, default=JOBS)
    s.add_argument("--sets", default="march-t0,march-nogt,june,class", help="comma list of march-t0, march-nogt, june, class")
    s.add_argument("--seed", default="L2", help="selection seed (another value draws other files; L2 = the phase's own set)")
    s.add_argument("--exclude", action="append", metavar="TSV", help="skip files already listed in a calibration TSV")
    s.add_argument("--out")
    a = ap.parse_args()
    if a.cmd in ("decompile", "check", "calibrate", "t2", "selftest"):
        return {"decompile": cmd_decompile, "check": cmd_check, "calibrate": cmd_calibrate, "t2": cmd_t2,
                "selftest": cmd_selftest}[a.cmd](a)
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
