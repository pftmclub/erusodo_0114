#!/usr/bin/env python3
"""Pack, compile and deploy March .kom archives so nobody types the elstools commands by hand.

    python tools/kom.py pack data036            # kom_contents/data036_kom -> kom_contents/data036_packed.kom
    python tools/kom.py pack 36 5 13            # several at once; data036 / 036 / 36 / data036_kom all work
    python tools/kom.py compile data036         # recompile ONLY stale X.src.lua -> X.lua (bytecode, mar-2014)
    python tools/kom.py copy data036            # kom_contents/data036_packed.kom -> <game dir>/data036.kom
    python tools/kom.py status                  # recently touched archives that need a pack / a copy
    python tools/kom.py status data036          # one archive in detail (entry diff vs the live kom)

pack  : runs `elstools pack -i <dir> -o <packed> -v mar-2014`. NOTE (measured 2026-10-06): the pack is
        DIRECTORY-driven, not manifest-driven - it packs every file in the dir except *.src.lua and
        _manifest.json, and the manifest only supplies each file's algo; a file the manifest does not
        list is packed anyway with algo 2 (wrong for non-lua: the rule is lua 2, everything else 0).
        So before it runs it REFUSES if any such unlisted file exists (--allow-unlisted overrides),
        warns about manifest entries with no file (they are silently left out), and about stale
        X.src.lua (newer than X.lua). After it runs it re-reads the result with `elstools inspect`,
        checks entry set + per-entry algo against dir + manifest, then replaces the old packed kom
        atomically. Prints the entry diff against the LIVE kom.
compile: see above; per-file (compile-batch), never compile-dir, because compile-dir overwrites every
        X.lua that has an X.src.lua beside it, including ones deliberately converted another way.
copy  : refuses while X2_offline.exe runs, if the kom_contents dir is newer than the packed kom, or
        if the packed kom DROPS entries the live kom has (--allow-removed overrides). Backs the
        live kom up to <kom_contents>/../kom_backup_tools/ (newest 5 kept per archive), copies via
        a .part file + atomic replace, verifies size+md5 and the entry count, prints `COPY OK`.

Exit codes: 0 ok, 1 failed, 2 usage, 3 game running, 4 refused (stale pack / dropped entries /
manifest mismatch).  Paths can be overridden with X2_GAME_DIR, X2_KOM_CONTENTS.
Never launches the game.
"""
import argparse, datetime, hashlib, json, os, re, shutil, subprocess, sys, time

for _s in (sys.stdout, sys.stderr):  # the Windows console defaults to cp1252
    try:
        _s.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass

TRUNK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ELSTOOLS = os.path.join(TRUNK, "elstools", "elstools.exe")
VERSION = "mar-2014"
GAME_DIR = os.environ.get("X2_GAME_DIR", r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data")
KOM_ROOT = os.environ.get("X2_KOM_CONTENTS", r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents")
BACKUP_ROOT = os.path.join(os.path.dirname(KOM_ROOT), "kom_backup_tools")
GAME_EXE = "X2_offline.exe"
KEEP_BACKUPS = 5
SHOW = 15  # max lines of any list


def p(*a):
    print(*a, flush=True)


def die(code, *a):
    p(*a)
    sys.exit(code)


def norm(arg):
    """'data036' / '036' / '36' / 'data036_kom' / 'data036.kom' / 'datai001' -> ('data036', '036')."""
    m = re.fullmatch(r"(?i)(?:data)?(i?)0*(\d+)(?:_kom|_packed|\.kom|_packed\.kom)?", arg.strip())
    if not m:
        die(2, "cannot read %r as an archive name (use data036, 036 or datai001)" % arg)
    num = m.group(2).zfill(3)
    return "data%s%s" % (m.group(1).lower(), num)


def paths(name):
    return {
        "dir": os.path.join(KOM_ROOT, name + "_kom"),
        "packed": os.path.join(KOM_ROOT, name + "_packed.kom"),
        "live": os.path.join(GAME_DIR, name + ".kom"),
    }


def md5(path):
    h = hashlib.md5()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def fmt_time(t):
    return datetime.datetime.fromtimestamp(t).strftime("%Y-%m-%d %H:%M:%S")


def running(image):
    """PIDs of processes with this image name (query only, never kill)."""
    try:
        out = subprocess.run(["tasklist", "/FI", "IMAGENAME eq %s" % image, "/FO", "CSV", "/NH"],
                             capture_output=True, text=True, errors="replace").stdout
    except OSError:
        return []
    pids = []
    for line in out.splitlines():
        cols = [c.strip('"') for c in line.split('","')]
        if len(cols) > 1 and cols[0].lower() == image.lower():
            try:
                pids.append(int(cols[1]))
            except ValueError:
                pass
    return pids


def elstools(args, timeout=1800):
    """Run elstools with stdin closed (a bash/while-read parent once starved it). Returns (rc, stdout)."""
    if not os.path.isfile(ELSTOOLS):
        die(1, "elstools.exe not found at %s" % ELSTOOLS)
    r = subprocess.run([ELSTOOLS] + args, stdin=subprocess.DEVNULL, capture_output=True, text=True,
                       encoding="utf-8", errors="replace", timeout=timeout)
    return r.returncode, r.stdout


def last_result(stdout):
    res = None
    for line in stdout.splitlines():
        line = line.strip()
        if line.startswith("{") and '"type":"result"' in line.replace(" ", ""):
            try:
                res = json.loads(line)
            except ValueError:
                pass
    return res


def inspect(kom):
    rc, out = elstools(["inspect", "-q", "-i", kom])
    try:
        j = json.loads(out)
    except ValueError:
        j = None
    if rc != 0 or not j or "files" not in j:
        die(1, "elstools inspect failed on %s (rc=%s): %s" % (kom, rc, out[-300:]))
    return j


def load_manifest(d):
    mp = os.path.join(d, "_manifest.json")
    if not os.path.isfile(mp):
        die(4, "REFUSED: %s has no _manifest.json (pack is manifest-driven; it would pack nothing)." % d)
    with open(mp, "r", encoding="utf-8-sig") as f:
        return json.load(f)


def content_files(d):
    """Relative names of every file in the kom dir, forward slashes."""
    out = {}
    for root, _dirs, files in os.walk(d):
        for fn in files:
            full = os.path.join(root, fn)
            out[os.path.relpath(full, d).replace(os.sep, "/")] = full
    return out


def newest_content(d):
    """(mtime, name) of the newest file that can change the packed output (not X.src.lua)."""
    best = (0.0, "")
    for rel, full in content_files(d).items():
        if rel.lower().endswith(".src.lua"):
            continue
        t = os.path.getmtime(full)
        if t > best[0]:
            best = (t, rel)
    return best


def stale_sources(d):
    """X.src.lua whose X.lua is missing or older than it (needs compiling)."""
    stale = []
    for rel, full in content_files(d).items():
        if not rel.lower().endswith(".src.lua"):
            continue
        lua = full[:-len(".src.lua")] + ".lua"
        if not os.path.isfile(lua) or os.path.getmtime(lua) < os.path.getmtime(full):
            stale.append(full)
    return sorted(stale)


def lst(items, label, limit=SHOW):
    for it in items[:limit]:
        p("    %s %s" % (label, it))
    if len(items) > limit:
        p("    ... and %d more" % (len(items) - limit))


def entry_map(j):
    return {e["name"]: (e["size"], e["comp"], str(e["algo"])) for e in j["files"]}


def diff(old, new):
    """old/new: entry maps. Returns (added, removed, changed) name lists."""
    added = sorted(set(new) - set(old))
    removed = sorted(set(old) - set(new))
    changed = sorted(n for n in set(old) & set(new) if old[n][:2] != new[n][:2] or old[n][2] != new[n][2])
    return added, removed, changed


def report_diff(label, added, removed, changed):
    p("  vs %s: +%d added, -%d removed, ~%d changed" % (label, len(added), len(removed), len(changed)))
    lst(added, "+")
    lst(removed, "-")
    lst(changed, "~")


# ----------------------------------------------------------------------------------------------
def cmd_pack(name, allow_unlisted=False):
    pa = paths(name)
    d = pa["dir"]
    if not os.path.isdir(d):
        die(2, "no such kom dir: %s" % d)
    man = load_manifest(d)
    files = content_files(d)
    p("== pack %s  (%d manifest entries, %d files in dir)" % (name, len(man), len(files)))

    packable = sorted(n for n in files if n != "_manifest.json" and not n.lower().endswith(".src.lua"))
    missing = sorted(n for n in man if n not in files)
    if missing:
        p("  WARN %d manifest entr%s no file in the dir, so %s NOT packed:" % (len(missing), "y has" if len(missing) == 1 else "ies have", "it is" if len(missing) == 1 else "they are"))
        lst(missing, "!")
    unlisted = [n for n in packable if n not in man]
    if unlisted:
        p("%s %d file(s) are in the dir but NOT in _manifest.json. elstools packs them anyway with algo 2 (rule: lua 2, non-lua 0):" %
          ("  WARN" if allow_unlisted else "REFUSED:", len(unlisted)))
        lst(unlisted, "?")
        if not allow_unlisted:
            die(4, "Add them to _manifest.json (\"name\": \"2\" for .lua, \"0\" otherwise) or move them out; nothing was written. (--allow-unlisted to pack anyway)")
    stale = stale_sources(d)
    if stale:
        p("  WARN %d X.src.lua newer than X.lua (edited after the last compile) - run `kom.py compile %s` first if those edits should ship:" % (len(stale), name))
        lst([os.path.relpath(s, d) for s in stale], "!")

    part = pa["packed"] + ".part.kom"
    if os.path.exists(part):
        os.remove(part)
    t0 = time.time()
    rc, out = elstools(["pack", "-q", "-i", d, "-o", part, "-v", VERSION])
    res = last_result(out)
    if rc != 0 or not res or res.get("failed") or res.get("skipped") or res.get("ok") != len(packable):
        if os.path.exists(part):
            os.remove(part)
        die(1, "PACK FAILED rc=%s result=%s\n%s" % (rc, res, out[-600:]))

    j = inspect(part)
    new = entry_map(j)
    bad = []
    if set(new) != set(packable):
        bad.append("entry set differs from the dir: %s" % sorted(set(new) ^ set(packable))[:SHOW])
    for n, algo in man.items():
        if n in new and new[n][2] != str(algo):
            bad.append("%s: packed algo %s, manifest says %s" % (n, new[n][2], algo))
    if bad:
        os.remove(part)
        p("PACK VERIFY FAILED (old packed kom left untouched):")
        lst(bad, "!")
        sys.exit(1)

    os.replace(part, pa["packed"])
    size = os.path.getsize(pa["packed"])
    p("  packed: %s" % pa["packed"])
    p("  %d entries, %d bytes, algos %s, %.1fs" % (j["file_count"], size, j["algos"], time.time() - t0))
    if os.path.isfile(pa["live"]):
        a, r, c = diff(entry_map(inspect(pa["live"])), new)
        report_diff("live %s.kom" % name, a, r, c)
        if r:
            p("  NOTE: the packed kom drops %d live entr%s; `copy` will refuse without --allow-removed." % (len(r), "y" if len(r) == 1 else "ies"))
    else:
        p("  (no live %s.kom in the game dir - this would be a new archive)" % name)
    p("PACK OK %s: %s" % (name, pa["packed"]))


def cmd_compile(name):
    d = paths(name)["dir"]
    if not os.path.isdir(d):
        die(2, "no such kom dir: %s" % d)
    stale = stale_sources(d)
    p("== compile %s: %d stale X.src.lua" % (name, len(stale)))
    if not stale:
        p("nothing to compile (every X.lua is newer than its X.src.lua).")
        return
    args = ["compile-batch", "-q", "-v", VERSION]
    for s in stale:
        args += ["-i", os.path.abspath(s)]
    rc, out = elstools(args)
    res = last_result(out)
    lst([os.path.relpath(s, d) for s in stale], ">")
    if rc != 0 or not res or res.get("failed"):
        die(1, "COMPILE FAILED rc=%s result=%s\n%s" % (rc, res, out[-800:]))
    p("COMPILE OK %s: %s" % (name, {k: v for k, v in res.items() if k in ("ok", "failed", "skipped")}))
    p("Next: kom.py pack %s" % name)


def cmd_copy(name, allow_removed, force):
    pa = paths(name)
    if not os.path.isfile(pa["packed"]):
        die(4, "REFUSED: %s does not exist - run `kom.py pack %s` first." % (pa["packed"], name))
    pids = running(GAME_EXE)
    if pids:
        die(3, "COPY REFUSED: %s is running (pid %s). Ask the user to close the game, then rerun." % (GAME_EXE, pids))
    if not os.path.isdir(GAME_DIR):
        die(1, "game dir not found: %s" % GAME_DIR)
    p("== copy %s" % name)
    if os.path.isdir(pa["dir"]):
        t, who = newest_content(pa["dir"])
        if t > os.path.getmtime(pa["packed"]) and not force:
            die(4, "COPY REFUSED: %s is newer than the packed kom (%s > %s). Run `kom.py pack %s` first (or --force)." %
                (who, fmt_time(t), fmt_time(os.path.getmtime(pa["packed"])), name))
    live_exists = os.path.isfile(pa["live"])
    if live_exists and os.path.getsize(pa["live"]) == os.path.getsize(pa["packed"]) and md5(pa["live"]) == md5(pa["packed"]):
        die(0, "COPY OK %s: already identical to the live kom (md5 match), nothing copied." % name)

    new_j = inspect(pa["packed"])
    if live_exists:
        a, r, c = diff(entry_map(inspect(pa["live"])), entry_map(new_j))
        report_diff("live %s.kom" % name, a, r, c)
        if r and not allow_removed:
            die(4, "COPY REFUSED: the packed kom drops %d entr%s the live kom has (list above). The kom dir is probably out of step with the live archive; "
                   "rerun with --allow-removed only if the user wants them gone." % (len(r), "y" if len(r) == 1 else "ies"))
        os.makedirs(BACKUP_ROOT, exist_ok=True)
        bak = os.path.join(BACKUP_ROOT, "%s.kom.%s" % (name, datetime.datetime.now().strftime("%Y%m%d_%H%M%S")))
        shutil.copy2(pa["live"], bak)
        old = sorted(f for f in os.listdir(BACKUP_ROOT) if f.startswith(name + ".kom."))
        for f in old[:-KEEP_BACKUPS]:
            os.remove(os.path.join(BACKUP_ROOT, f))
        p("  backup: %s" % bak)
    else:
        p("  NOTE: no live %s.kom existed - creating a new archive; confirm the client actually mounts it." % name)

    part = pa["live"] + ".part"
    try:
        shutil.copy2(pa["packed"], part)
        os.replace(part, pa["live"])  # new inode: also detaches any hard link the old file had
    except OSError as e:
        if os.path.exists(part):
            try:
                os.remove(part)
            except OSError:
                pass
        die(1, "COPY FAILED: %s (game running, a sharing lock, or Defender; the old live kom is untouched or restorable from %s)" % (e, BACKUP_ROOT))

    ok = os.path.getsize(pa["live"]) == os.path.getsize(pa["packed"]) and md5(pa["live"]) == md5(pa["packed"])
    n = inspect(pa["live"])["file_count"]
    if not ok or n != new_j["file_count"]:
        die(1, "COPY VERIFY FAILED: size/md5 match=%s entries live=%s packed=%s" % (ok, n, new_j["file_count"]))
    p("COPY OK %s: %s  %d bytes, %d entries, md5 %s" % (name, pa["live"], os.path.getsize(pa["live"]), n, md5(pa["live"])))


def cmd_status(names, days):
    detail = bool(names)
    if not names:
        names = sorted({norm(f.split("_")[0].split(".")[0]) for f in os.listdir(KOM_ROOT)
                        if re.fullmatch(r"data[i]?\d{3}_kom", f)})
    rows = []
    for name in names:
        pa = paths(name)
        if not os.path.isdir(pa["dir"]):
            rows.append((name, "NO DIR", "")); continue
        t, who = newest_content(pa["dir"])
        packed = os.path.isfile(pa["packed"])
        live = os.path.isfile(pa["live"])
        lt = os.path.getmtime(pa["live"]) if live else 0
        pt = os.path.getmtime(pa["packed"]) if packed else 0
        if packed and t > pt:
            state, note = "PACK NEEDED", "%s newer than packed (%s)" % (who, fmt_time(t))
        elif packed:
            same = live and os.path.getsize(pa["live"]) == os.path.getsize(pa["packed"]) and md5(pa["live"]) == md5(pa["packed"])
            state, note = ("in sync", "") if same else ("COPY NEEDED", "packed %s differs from live" % fmt_time(pt))
        else:
            state, note = "-", ""
        if stale_sources(pa["dir"]):
            note += "  [stale .src.lua: compile]"
        if not detail and (not packed or max(t, pt) < time.time() - days * 86400):
            state, note = "-", ""  # never packed here, or untouched for `days`: not part of the current workflow
        rows.append((name, state, note.strip()))
    shown = [r for r in rows if detail or r[1] not in ("-", "in sync")]
    for n, s, note in shown:
        p("%-9s %-12s %s" % (n, s, note))
    if not detail:
        p("(%d archives scanned: %d need a pack, %d need a copy, %d in sync, %d not in play)" % (
            len(rows), sum(r[1] == "PACK NEEDED" for r in rows), sum(r[1] == "COPY NEEDED" for r in rows),
            sum(r[1] == "in sync" for r in rows), sum(r[1] == "-" for r in rows)))
        p("(only archives with a packed kom touched in the last %g day(s) are listed; --days N widens it, or name an archive)" % days)
    if detail:
        for name in names:
            pa = paths(name)
            if os.path.isfile(pa["packed"]) and os.path.isfile(pa["live"]):
                a, r, c = diff(entry_map(inspect(pa["live"])), entry_map(inspect(pa["packed"])))
                report_diff("live (what `copy` would change)", a, r, c)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    for c in ("pack", "compile", "copy", "status"):
        s = sub.add_parser(c)
        s.add_argument("koms", nargs="*" if c == "status" else "+", help="data036 | 036 | 36 | datai001 ...")
        if c == "copy":
            s.add_argument("--allow-removed", action="store_true", help="copy even if the packed kom drops entries the live kom has")
            s.add_argument("--force", action="store_true", help="copy even if the kom dir is newer than the packed kom")
        if c == "status":
            s.add_argument("--days", type=float, default=2, help="no-arg status: only archives touched within N days (default 2)")
        if c == "pack":
            s.add_argument("--allow-unlisted", action="store_true", help="pack files the manifest does not list (they get algo 2)")
    a = ap.parse_args()
    names = [norm(k) for k in a.koms]
    if a.cmd == "status":
        cmd_status(names, a.days)
        return
    for n in names:
        if a.cmd == "pack":
            cmd_pack(n, a.allow_unlisted)
        elif a.cmd == "compile":
            cmd_compile(n)
        else:
            cmd_copy(n, a.allow_removed, a.force)


if __name__ == "__main__":
    main()
