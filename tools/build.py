"""Build (and optionally deploy) the offline client. Never launches the game.

    python tools/build.py                 # build X2ServerProtocol, KTDXLIB, X2Lib, X2 (US_SERVICE)
    python tools/build.py --deploy        # ...then copy x2.exe -> <game dir>/X2_offline.exe and verify
    python tools/build.py --deploy-only   # deploy the existing artifact without building
    python tools/build.py --deps          # also rebuild luajitLib + luaLib (Release)
    python tools/build.py --config US_INTERNAL   # only when explicitly asked for
    python tools/build.py --dry-run       # print what would run

What it does for you (each item is a rule that used to live in CLAUDE.md prose):
  * msbuild by full path, SolutionDir with forward slashes and a trailing slash,
    /MP on, node reuse off (so no MSBuild.exe lingers and looks like a live build).
  * Project order and configs: luajitLib/luaLib Release (only with --deps or when the
    .lib is missing), X2ServerProtocol X2TOOL, then KTDXLIB, X2Lib, X2 in --config.
  * PCH invalidation per project: if any header that project's PCH can see is newer
    than its .pch, the PCH-creating .cpp is touched first. --pch force|skip overrides.
  * One build at a time across every session AND every worktree (they share one game
    directory): a lock in %LOCALAPPDATA%/x2build/ (override with X2_BUILD_STATE).
    Waits up to --wait minutes (default 30), then exits 75 naming the holder.
  * Coalescing: a run that waited for the lock does not build again if another session's
    full build STARTED after this run was requested and succeeded (it compiled the tree
    with this session's edits already on disk); it deploys that exe instead. Only a
    successful, full (no --only), same-config, same-tree build counts. --no-coalesce or
    --pch force always builds. Edits saved after you ran build.py are not covered.
  * X2 is built with BuildProjectReferences=false when KTDXLIB and X2Lib were built
    successfully earlier in the same run (otherwise its references re-check, as before),
    and afterwards x2.exe must be at least as new as X2/KTDXLIB.lib and X2/X2Lib.lib,
    or the run fails (STALE LINK, exit 4).
  * Refuses to start while a cl.exe/link.exe/lib.exe from someone else is running.
  * Prints only errors (full log in .build/logs/), checks x2.exe is newer than the
    build start, and with --deploy: refuses if X2_offline.exe is running, copies,
    verifies name/size/md5 programmatically, re-checks after 3 s (Defender), and
    writes %LOCALAPPDATA%/x2build/last_deploy.json for tools/logs.py.

Exit codes: 0 ok, 1 build failed, 2 usage, 3 deploy refused (game running),
4 verification failed, 75 lock/compiler busy timeout.
"""
import argparse, datetime, hashlib, json, os, re, shutil, subprocess, sys, time

for _s in (sys.stdout, sys.stderr):  # the Windows console defaults to cp1252
    try:
        _s.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass

TRUNK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MSBUILD = r"C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe"
GAME_DIR = os.environ.get("X2_GAME_DIR", r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data")
DEPLOY_NAME = "X2_offline.exe"
STATE = os.path.join(TRUNK, ".build")  # per-tree: build logs
# Shared by every worktree (they all deploy into the same game directory): lock + deploy manifest.
SHARED = os.environ.get("X2_BUILD_STATE", os.path.join(os.environ.get("LOCALAPPDATA", STATE), "x2build"))
LOCK = os.path.join(SHARED, "build.lock")
MANIFEST = os.path.join(SHARED, "last_deploy.json")
LAST_BUILD = os.path.join(SHARED, "last_build.json")  # what the last lock holder built, for coalescing
LINKED_LIBS = ["X2/KTDXLIB.lib", "X2/X2Lib.lib"]  # x2.exe pulls these in via #pragma comment(lib) in X2.cpp
LOCK_STALE_MIN = 90

# Headers each project's PCH can see (conservative). Dirs are relative to TRUNK.
HDR_ALL = ["KTDXLIB", "X2Lib", "X2ServerProtocol", "KncWX2Server/Common", "X2"]
PROJECTS = [
    # name, vcxproj, config (None = --config), pch-creating source, pch file name, header dirs
    ("luajitLib", "luajitLib/luajitLib.vcxproj", "Release", None, None, None),
    ("luaLib", "luaLib/luaLib_2010.vcxproj", "Release", None, None, None),
    ("X2ServerProtocol", "X2ServerProtocol/X2ServerProtocol_2010.vcxproj", "X2TOOL", None, None, None),
    ("KTDXLIB", "KTDXLIB/KTDXLIB_2010.vcxproj", None, "KTDXLIB/stdafx.cpp", "KTDXLIB/{cfg}/KTDXLIB.pch", ["KTDXLIB"]),
    ("X2Lib", "X2Lib/X2Lib_2010.vcxproj", None, "X2Lib/stdafx.cpp", "X2Lib/{cfg}/X2Lib.pch", HDR_ALL),
    ("X2", "X2/X2_2010.vcxproj", None, "X2/Common/dxstdafx.cpp", "X2/{cfg}/x2.pch", HDR_ALL),
]
DEP_LIBS = {"luajitLib": "KTDXLIB/luajitLib.lib", "luaLib": "KTDXLIB/luaLib.lib"}
CODE_DIRS = ["X2", "X2Lib", "KTDXLIB", "X2ServerProtocol", "KncWX2Server/Common"]


def p(*a):
    print(*a, flush=True)


def tp(rel):
    return os.path.join(TRUNK, rel.replace("/", os.sep))


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


def pid_alive(pid):
    return pid in running_pids_all()


def running_pids_all():
    try:
        out = subprocess.run(["tasklist", "/FO", "CSV", "/NH"], capture_output=True, text=True, errors="replace").stdout
    except OSError:
        return set()
    s = set()
    for line in out.splitlines():
        cols = [c.strip('"') for c in line.split('","')]
        if len(cols) > 1:
            try:
                s.add(int(cols[1]))
            except ValueError:
                pass
    return s


def git(*args):
    r = subprocess.run(["git", "-C", TRUNK] + list(args), capture_output=True, text=True, errors="replace")
    return r.stdout.strip()


# ---------------------------------------------------------------- lock
def acquire_lock(wait_min, label):
    os.makedirs(SHARED, exist_ok=True)
    deadline = time.time() + wait_min * 60
    announced = False
    while True:
        try:
            fd = os.open(LOCK, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
            with os.fdopen(fd, "w") as f:
                json.dump({"pid": os.getpid(), "started": time.time(), "what": label,
                           "tree": TRUNK}, f)
            return True
        except FileExistsError:
            try:
                with open(LOCK) as f:
                    info = json.load(f)
            except Exception:
                info = {}
            age = (time.time() - info.get("started", time.time())) / 60
            if info.get("pid") and (not pid_alive(info["pid"]) or age > LOCK_STALE_MIN):
                p("lock: removing stale lock (pid %s, %.0f min old)" % (info.get("pid"), age))
                try:
                    os.remove(LOCK)
                except OSError:
                    pass
                continue
            if time.time() > deadline:
                p("LOCK TIMEOUT: another session is building (pid %s, '%s', started %.0f min ago)."
                  % (info.get("pid"), info.get("what"), age))
                p("Re-run this command (in the background) once it finishes; do not delete the lock.")
                return False
            if not announced:
                p("lock: waiting for another session's build (pid %s, '%s', %.0f min old)..."
                  % (info.get("pid"), info.get("what"), age))
                announced = True
            time.sleep(5)


def release_lock():
    try:
        with open(LOCK) as f:
            mine = json.load(f).get("pid") == os.getpid()
        if mine:  # remove only after the handle is closed (Windows refuses otherwise)
            os.remove(LOCK)
    except Exception as e:
        p("lock: could not release %s (%s); the next run will treat it as stale." % (LOCK, e))


def wait_for_foreign_compilers(wait_min):
    deadline = time.time() + wait_min * 60
    announced = False
    while True:
        busy = {img: running(img) for img in ("cl.exe", "link.exe", "lib.exe")}
        busy = {k: v for k, v in busy.items() if v}
        if not busy:
            return True
        if time.time() > deadline:
            p("COMPILER BUSY: %s still running outside this script (a session building by hand?)." % busy)
            return False
        if not announced:
            p("waiting: %s running outside the lock (someone building by hand)..." % busy)
            announced = True
        time.sleep(5)


# ---------------------------------------------------------------- pch
def newest_header(dirs):
    best, best_t = None, 0.0
    for d in dirs:
        root = tp(d)
        for dp, dn, fn in os.walk(root):
            dn[:] = [x for x in dn if x not in ("US_SERVICE", "US_INTERNAL", "Release", "Debug", "X2TOOL", "ipch", ".svn")
                     and not x.startswith("Release_")]
            for n in fn:
                if n.lower().endswith((".h", ".hpp", ".inl")):
                    fp = os.path.join(dp, n)
                    try:
                        t = os.path.getmtime(fp)
                    except OSError:
                        continue
                    if t > best_t:
                        best, best_t = fp, t
    return best, best_t


def pch_check(name, src, pch, dirs, cfg, mode, dry):
    if mode == "skip" or not src:
        return
    pch_path = tp(pch.format(cfg=cfg))
    if mode == "force":
        reason = "--pch force"
    elif not os.path.exists(pch_path):
        return  # first build creates it anyway
    else:
        hdr, t = newest_header(dirs)
        if t <= os.path.getmtime(pch_path):
            return
        reason = "%s is newer than %s" % (os.path.relpath(hdr, TRUNK), os.path.basename(pch_path))
    p("pch: %s: touching %s (%s)" % (name, src, reason))
    if not dry:
        os.utime(tp(src), None)


# ---------------------------------------------------------------- build
ERR_RE = re.compile(r"(: (fatal )?error [A-Z]*\d+|error MSB\d+|: error :|LINK : fatal)", re.I)
WARN_RE = re.compile(r": warning [A-Z]*\d+", re.I)


def build_project(name, proj, cfg, logdir, dry, extra=()):
    cmd = [MSBUILD, tp(proj), "-p:Configuration=%s" % cfg, "-p:Platform=Win32",
           "-p:SolutionDir=%s/" % TRUNK.replace("\\", "/"), "-m", "-p:MultiProcessorCompilation=true",
           "-nr:false", "-nologo", "-v:minimal"] + list(extra)
    p("build: %-17s %s%s" % (name, cfg, ("  " + " ".join(extra)) if extra else ""))
    if dry:
        p("   " + " ".join('"%s"' % c if " " in c else c for c in cmd))
        return True
    t0 = time.time()
    log = os.path.join(logdir, "%s.log" % name)
    with open(log, "wb") as lf:
        r = subprocess.run(cmd, cwd=TRUNK, stdout=lf, stderr=subprocess.STDOUT)
    raw = open(log, "rb").read()
    text = raw.decode("utf-8", errors="replace") if b"\xef\xbf\xbd" not in raw else raw.decode("cp949", errors="replace")
    errs, seen = [], set()
    nwarn = 0
    for line in text.splitlines():
        if ERR_RE.search(line):
            key = line.strip()
            if key not in seen:
                seen.add(key)
                errs.append(key)
        elif WARN_RE.search(line):
            nwarn += 1
    dt = time.time() - t0
    if r.returncode != 0 or errs:
        p("   FAILED rc=%d in %.0fs, %d error line(s):" % (r.returncode, dt, len(errs)))
        for e in errs[:40]:
            p("   " + e)
        if len(errs) > 40:
            p("   ... %d more" % (len(errs) - 40))
        if not errs:
            p("   (no error lines matched; tail of the log:)")
            for line in text.splitlines()[-15:]:
                p("   " + line)
        if any("C1083" in e for e in errs):
            p("   hint: C1083 on boost/d3dx9/ImportKncSerializer = SolutionDir problem; this script passes it, so check the include actually exists.")
        p("   full log: %s" % os.path.relpath(log, TRUNK))
        return False
    p("   ok in %.0fs (%d warning line(s))" % (dt, nwarn))
    return True


def write_last_build(rec):
    try:
        os.makedirs(SHARED, exist_ok=True)
        with open(LAST_BUILD, "w") as f:
            json.dump(rec, f, indent=2)
    except OSError as e:
        p("note: could not write %s (%s); the next waiter will build itself." % (LAST_BUILD, e))


def coalesced_build(a, t_req):
    """The last build if it already covers this request (see the docstring), else None."""
    if a.no_coalesce or a.only or a.pch == "force":
        return None
    try:
        with open(LAST_BUILD) as f:
            rec = json.load(f)
    except Exception:
        return None
    if not (rec.get("ok") and rec.get("full") and rec.get("config") == a.config
            and os.path.normcase(rec.get("tree", "")) == os.path.normcase(TRUNK)
            and rec.get("started", 0) >= t_req and (rec.get("deps") or not a.deps)):
        return None
    exe = tp("X2/%s/x2.exe" % a.config)
    if not os.path.exists(exe):
        return None
    # A later build may have relinked the exe since (newer = still includes this request).
    if md5(exe) != rec.get("exe_md5") and os.path.getmtime(exe) < rec["started"]:
        return None
    return rec


def stale_link(cfg):
    """Libs x2.exe should have linked that are missing or newer than it; [] when fine."""
    exe_t = os.path.getmtime(tp("X2/%s/x2.exe" % cfg))
    bad = []
    for lib in LINKED_LIBS:
        if not os.path.exists(tp(lib)):
            bad.append("%s is missing" % lib)
        elif os.path.getmtime(tp(lib)) > exe_t + 2:
            bad.append("%s (%s) is newer than x2.exe (%s)" % (lib, fmt_time(os.path.getmtime(tp(lib))), fmt_time(exe_t)))
    return bad


def dirty_code_files():
    # Not git(): its strip() eats the first line's leading status space, and l[3:] then drops a letter.
    out = subprocess.run(["git", "-C", TRUNK, "status", "--porcelain", "--"] + CODE_DIRS,
                         capture_output=True, text=True, errors="replace").stdout
    return [l[3:] for l in out.splitlines() if l.strip()]


# ---------------------------------------------------------------- deploy
def deploy(cfg, build_start, dry):
    src = tp("X2/%s/x2.exe" % cfg)
    dst = os.path.join(GAME_DIR, DEPLOY_NAME)
    if not os.path.exists(src):
        p("DEPLOY: %s does not exist" % src)
        return 4
    if build_start and not dry and os.path.getmtime(src) < build_start - 2:
        p("note: x2.exe was not relinked by this build (nothing changed); deploying it anyway.")
    pids = running(DEPLOY_NAME)
    if pids:
        p("DEPLOY REFUSED: %s is running (pid %s). Ask the user to close the game, then run:" % (DEPLOY_NAME, pids))
        p("    python tools/build.py --deploy-only")
        return 3
    if dry:
        p("deploy: copy %s -> %s" % (src, dst))
        return 0
    src_md5 = md5(src)
    if os.path.exists(dst) and md5(dst) == src_md5:
        # Same bytes already deployed: do not copy, so the file time (and with it tools/logs.py's
        # "run after deploy" judgement of logs written by these very bytes) stays put.
        p("deploy: %s already has these exact bytes; registering it without copying." % DEPLOY_NAME)
    else:
        shutil.copyfile(src, dst)
        time.sleep(3)  # Defender quarantine shows up as the file vanishing right after the copy
    names = os.listdir(GAME_DIR)
    if DEPLOY_NAME not in names:
        p("DEPLOY FAILED: %s is gone from the game directory right after the copy." % DEPLOY_NAME)
        p("Windows Defender (Bearfoos ML false positive) probably quarantined it: check Protection History;")
        p("the fix is a folder exclusion for the game directory (needs admin, the user's step).")
        return 4
    dst_md5, size, mt = md5(dst), os.path.getsize(dst), os.path.getmtime(dst)
    if dst_md5 != src_md5 or size != os.path.getsize(src):
        p("DEPLOY FAILED: copy differs from x2.exe (md5 %s vs %s)" % (dst_md5, src_md5))
        return 4
    head = git("rev-parse", "--short", "HEAD")
    dirty = dirty_code_files()
    manifest = {"deployed": mt, "deployed_str": fmt_time(mt), "exe": dst, "size": size, "md5": dst_md5,
                "source": "X2/%s/x2.exe" % cfg, "source_mtime": fmt_time(os.path.getmtime(src)),
                "config": cfg, "head": head, "branch": git("rev-parse", "--abbrev-ref", "HEAD"), "tree": TRUNK,
                "dirty_code_files": dirty}
    os.makedirs(SHARED, exist_ok=True)
    with open(MANIFEST, "w") as f:
        json.dump(manifest, f, indent=2)
    p("DEPLOY OK: %s is %s B, dated %s, md5 %s, equal to X2/%s/x2.exe (%s); built at %s %s%s."
      % (DEPLOY_NAME, format(size, ","), manifest["deployed_str"], dst_md5, cfg, manifest["source_mtime"],
         manifest["branch"], head,
         " + %d uncommitted code file(s)" % len(dirty) if dirty else ""))
    p("(Paste that line into the Done note. Do NOT launch the game - ask the user.)")
    return 0


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--config", default="US_SERVICE")
    ap.add_argument("--deploy", action="store_true")
    ap.add_argument("--deploy-only", action="store_true")
    ap.add_argument("--deps", action="store_true", help="also build luajitLib and luaLib")
    ap.add_argument("--only", help="comma list of projects to build (e.g. X2Lib,X2)")
    ap.add_argument("--pch", choices=["auto", "force", "skip"], default="auto")
    ap.add_argument("--wait", type=float, default=30, help="minutes to wait for the lock / foreign compilers")
    ap.add_argument("--no-coalesce", action="store_true",
                    help="always build, even if a build that started after this request already succeeded")
    ap.add_argument("--dry-run", action="store_true")
    a = ap.parse_args()
    t_req = time.time()  # edits on disk before this moment are what this run promises to build

    if a.config not in ("US_SERVICE", "US_INTERNAL"):
        p("--config must be US_SERVICE or US_INTERNAL")
        return 2
    if not os.path.exists(MSBUILD):
        p("msbuild not found at %s" % MSBUILD)
        return 2

    label = "build.py %s" % " ".join(sys.argv[1:])
    if not a.dry_run and not acquire_lock(a.wait, label):
        return 75
    try:
        build_start = None
        rec = None if (a.deploy_only or a.dry_run) else coalesced_build(a, t_req)
        if rec:
            p("COALESCED: '%s' (pid %s) started a full %s build at %s, after this request (%s), and it succeeded"
              % (rec.get("what"), rec.get("pid"), rec["config"], fmt_time(rec["started"]), fmt_time(t_req)))
            p("  at %s; it compiled this tree with your edits on disk, so this run does not build again."
              % rec.get("head"))
            p("  (Pass --no-coalesce to force an own build.)")
            if a.deploy:
                return deploy(a.config, rec["started"], a.dry_run)
            return 0
        if not a.deploy_only:
            if not a.dry_run and not wait_for_foreign_compilers(a.wait):
                return 75
            only = set(x.strip() for x in a.only.split(",")) if a.only else None
            stamp = datetime.datetime.now().strftime("%Y%m%d-%H%M%S")
            logdir = os.path.join(STATE, "logs", stamp)
            if not a.dry_run:
                os.makedirs(logdir, exist_ok=True)
            dirty = dirty_code_files()
            p("HEAD %s on %s; %d uncommitted code file(s)%s" % (
                git("rev-parse", "--short", "HEAD"), git("rev-parse", "--abbrev-ref", "HEAD"), len(dirty),
                (": " + ", ".join(dirty[:12]) + (" ..." if len(dirty) > 12 else "")) if dirty else ""))
            if dirty:
                p("  (other sessions share this tree: anything listed that is not yours is built in too)")
            if not a.dry_run and not os.path.exists(tp("KTDXLIB/sfjpeg.lib")):
                # june-gfx G3: Scaleform's libjpeg 8d under private names (git-ignored *.lib); the client's jpeg.lib is 6b
                p("KTDXLIB/sfjpeg.lib is missing: running tools/sfjpeg.py")
                subprocess.run([sys.executable, tp("tools/sfjpeg.py")])
            build_start = time.time()
            rec = {"pid": os.getpid(), "what": label, "tree": TRUNK, "config": a.config, "started": build_start,
                   "full": not only, "deps": a.deps, "head": git("rev-parse", "--short", "HEAD"), "ok": False}
            built = set()
            for name, proj, cfg, src, pch, dirs in PROJECTS:
                if only and name not in only:
                    continue
                if name in DEP_LIBS and not a.deps and os.path.exists(tp(DEP_LIBS[name])):
                    continue
                cfg = cfg or a.config
                pch_check(name, src, pch, dirs, cfg, a.pch, a.dry_run)
                # Both libs were just built in this run: X2's ProjectReferences would only rebuild
                # them a second time (a peer's mid-build header edit makes that a ~10 min full pass).
                extra = ["-p:BuildProjectReferences=false"] if name == "X2" and {"KTDXLIB", "X2Lib"} <= built else []
                if not build_project(name, proj, cfg, logdir, a.dry_run, extra):
                    p("BUILD FAILED (%s). Nothing was deployed." % name)
                    if not a.dry_run:
                        write_last_build(rec)
                    return 1
                built.add(name)
            if not a.dry_run and (not only or "X2" in only):
                exe = tp("X2/%s/x2.exe" % a.config)
                if not os.path.exists(exe):
                    p("BUILD FAILED: X2/%s/x2.exe missing after a clean build" % a.config)
                    write_last_build(rec)
                    return 4
                bad = stale_link(a.config)
                if bad:
                    p("STALE LINK: x2.exe did not pick up the libraries it links: " + "; ".join(bad))
                    p("  Re-run with --no-coalesce; if it repeats, delete X2/%s/link.*.tlog and build again." % a.config)
                    write_last_build(rec)
                    return 4
                mt = os.path.getmtime(exe)
                p("BUILD OK: X2/%s/x2.exe %s B, %s%s" % (a.config, format(os.path.getsize(exe), ","), fmt_time(mt),
                  "" if mt >= build_start - 2 else " (unchanged: nothing needed relinking)"))
                rec.update(ok=True, finished=time.time(), exe_md5=md5(exe))
                write_last_build(rec)
        if a.deploy or a.deploy_only:
            return deploy(a.config, build_start, a.dry_run)
        return 0
    finally:
        if not a.dry_run:
            release_lock()


if __name__ == "__main__":
    sys.exit(main())
