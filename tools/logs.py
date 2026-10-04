"""Read the game's logs for one phase, and say whether they belong to the deployed exe.

    python tools/logs.py                          # status + packet/offline summary, no grep
    python tools/logs.py --tag JUNE_BUFF          # + every line containing [JUNE_BUFF]
    python tools/logs.py --grep "BBT (75|91)"     # + lines matching a regex (combine with --tag)
    python tools/logs.py --tag JUNE_BUFF --files errorlog,server --max 400

The header answers the questions that used to take three round trips:
  * which exe is deployed (md5, time) and whether it is still what tools/build.py deployed;
  * whether the game is running right now;
  * when each log's run started, and whether that run is AFTER the last deploy
    (the logs are rewritten per launch, so an older run means the user has not
    played the new build yet: say so instead of reading stale lines).
Keep a phase's log check to its own tag (memory: scope play-test log checks to the phase).
"""
import argparse, datetime, hashlib, json, os, re, subprocess, sys

for _s in (sys.stdout, sys.stderr):  # the Windows console defaults to cp1252
    try:
        _s.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass

TRUNK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME_DIR = os.environ.get("X2_GAME_DIR", r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data")
EXE = "X2_offline.exe"
FILES = {
    "errorlog": "ErrorLog.txt",
    "server": "offline_server.log",
    "packets": "offline_packets.log",
    "state": "State.log",
    "openscript": "OpenScriptFile.log",
}
DEFAULT_GREP_FILES = ["errorlog", "server", "packets"]


def read_text(path):
    raw = open(path, "rb").read()
    if raw.startswith(b"\xef\xbb\xbf"):
        raw = raw[3:]
    try:
        return raw.decode("utf-8")
    except UnicodeDecodeError:
        return raw.decode("cp949", errors="replace")


def md5(path):
    h = hashlib.md5()
    with open(path, "rb") as f:
        for c in iter(lambda: f.read(1 << 20), b""):
            h.update(c)
    return h.hexdigest()


def fmt(t):
    return datetime.datetime.fromtimestamp(t).strftime("%Y-%m-%d %H:%M:%S") if t else "?"


def game_running():
    try:
        out = subprocess.run(["tasklist", "/FI", "IMAGENAME eq %s" % EXE, "/FO", "CSV", "/NH"],
                             capture_output=True, text=True, errors="replace").stdout
    except OSError:
        return None
    return EXE.lower() in out.lower()


def run_start(key, path, text):
    """Launch time of the run that wrote this log (logs are rewritten per launch)."""
    mt = os.path.getmtime(path)
    first = text[:200]
    m = re.match(r"(\d{4})/(\d{1,2})/(\d{1,2}) (\d{1,2}):(\d{2}):(\d{2})", first)
    if m:
        return datetime.datetime(*map(int, m.groups())).timestamp()
    m = re.match(r"\[(\d{2}):(\d{2}):(\d{2})", first)
    if m:
        d = datetime.datetime.fromtimestamp(mt)
        t = d.replace(hour=int(m.group(1)), minute=int(m.group(2)), second=int(m.group(3)), microsecond=0)
        if t > d:  # crossed midnight
            t -= datetime.timedelta(days=1)
        return t.timestamp()
    return None


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--tag", help="bracket tag without brackets, e.g. JUNE_BUFF")
    ap.add_argument("--grep", help="regex")
    ap.add_argument("--files", help="comma list of: %s (default %s)" % (",".join(FILES), ",".join(DEFAULT_GREP_FILES)))
    ap.add_argument("--max", type=int, default=250, help="max matching lines per file (tail kept)")
    a = ap.parse_args()

    # ---- deploy status
    shared = os.environ.get("X2_BUILD_STATE", os.path.join(os.environ.get("LOCALAPPDATA", ""), "x2build"))
    man_path = os.path.join(shared, "last_deploy.json")
    man = json.load(open(man_path)) if os.path.exists(man_path) else None
    exe_path = os.path.join(GAME_DIR, EXE)
    deployed_t = None
    if os.path.exists(exe_path):
        cur_md5 = md5(exe_path)
        deployed_t = os.path.getmtime(exe_path)
        line = "exe: %s %s, md5 %s" % (EXE, fmt(deployed_t), cur_md5)
        if man:
            if man.get("md5") == cur_md5:
                line += " = tools/build.py deploy of %s %s%s" % (man.get("branch"), man.get("head"),
                        " (+%d uncommitted)" % len(man["dirty_code_files"]) if man.get("dirty_code_files") else "")
            else:
                line += " (NOT the last tools/build.py deploy, which was md5 %s: deployed by hand or replaced)" % man.get("md5")
        print(line)
    else:
        print("exe: %s is MISSING from the game directory (Defender quarantine?)" % EXE)
    running = game_running()
    print("game: %s" % ("RUNNING now (logs may still be growing)" if running else "not running"))

    # ---- per-log run status
    texts = {}
    stale_any = False
    for key, name in FILES.items():
        path = os.path.join(GAME_DIR, name)
        if not os.path.exists(path):
            print("  %-10s %-20s missing" % (key, name))
            continue
        text = read_text(path)
        texts[key] = text
        start = run_start(key, path, text)
        rel = ""
        if deployed_t and start:
            if start >= deployed_t - 1:
                rel = "run AFTER deploy"
            else:
                rel = "run BEFORE deploy (stale: the new build has not been played)"
                stale_any = True
        print("  %-10s %-20s run started %s, last write %s, %d lines  %s" % (
            key, name, fmt(start), fmt(os.path.getmtime(path)), text.count("\n"), rel))
    if stale_any:
        print("NOTE: at least one log predates the deployed exe; ask the user to play the new build before judging.")

    # ---- standing summary
    pk = texts.get("packets", "")
    if pk:
        un = re.findall(r"^.*\*\*\* UNHANDLED \*\*\*.*$", pk, re.M)
        ex = re.findall(r"^.*EXCEPTION - ROLLED BACK.*$", pk, re.M)
        print("packets: %d UNHANDLED, %d EXCEPTION%s" % (len(un), len(ex), "" if not (un or ex) else ":"))
        for l in (un + ex)[:20]:
            print("   " + l.strip())
    srv = texts.get("server", "")
    census = [l for l in srv.splitlines() if "CENSUS" in l]
    if census:
        print("server: census lines present (clean exit)")

    # ---- grep
    if not (a.tag or a.grep):
        return 0
    pats = []
    if a.tag:
        pats.append(re.compile(re.escape("[%s]" % a.tag)))
    if a.grep:
        pats.append(re.compile(a.grep))
    keys = [k.strip() for k in a.files.split(",")] if a.files else DEFAULT_GREP_FILES
    total = 0
    for key in keys:
        text = texts.get(key)
        if text is None:
            continue
        hits = [(i, l) for i, l in enumerate(text.splitlines(), 1) if all(p.search(l) for p in pats)]
        total += len(hits)
        print("\n== %s: %d matching line(s)%s" % (FILES[key], len(hits),
              " (last %d shown)" % a.max if len(hits) > a.max else ""))
        for i, l in hits[-a.max:]:
            print("%6d| %s" % (i, l.rstrip()))
    if total == 0:
        print("\nno matching lines (is the tag right? did the run reach the code?)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
