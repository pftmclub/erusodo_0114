"""Run a character plan's phases unattended, one headless `claude -p` session per phase.

    python tools/runphases.py char-ara --dry-run                  # list the steps, models, skips; run nothing
    python tools/runphases.py char-ara                            # CH1a .. (before CH1), then every Opus review
    python tools/runphases.py char-ara --from CH1d --stop-after CH1g
    python tools/runphases.py char-ara --max-usd 25               # per-step budget cap
    python tools/runphases.py --selftest                          # prove `claude -p "/x2-phase status"` works headless

Every step is the `x2-phase` skill in a fresh session: CLAUDE.md, the hooks and the skill's own gates still apply.
This tool only does the loop the user types by hand (`/x2-phase <plan> <id>`), then checks the result itself.

Order: all phases first (table order of the plan's STATUS.md block, from --from up to but not including --until,
default CH1), then every phase flagged "Opus review" gets its review session (reviews deferred to the end, user 2026-11-10).

Model and effort of a step are READ, never defaulted: from the step's own kickoff `Start` line
(kickoffs/<plan>/<id>.md, reviews <id>-review.md), cross-checked with the STATUS row. A mismatch, a missing model or
an unknown model name stops the run before anything is launched.

The list is live: after every step STATUS.md and the kickoffs are re-read, so a phase created by a phase is picked up
(its row must have a kickoff file). --max-steps bounds a phase that keeps spawning phases.

A step passes only if: exit 0 and no error result; the final text has `RUNPHASES DONE <id>`; a new commit
`<plan> Phase <id>` (reviews: `<plan> Phase <id> review`) exists; the plan doc has the `### <id> - Done` / `- Review`
heading; `junediverge.py --check` on the changed client code prints RESULT: PASS (phases only); no owned file is left
dirty. The first failure stops the run (later phases edit the same files); re-run the same command to resume.

Logs: runphases_logs/<stamp>_<plan>_<id>[_review].jsonl (raw stream) and runphases_logs/summary.tsv.
Exit codes: 0 all done, 1 a step failed its gates, 2 usage / preflight refused, 3 interrupted.
"""
import argparse, datetime, json, os, queue, re, shutil, socket, subprocess, sys, threading, time

for _s in (sys.stdout, sys.stderr):  # the Windows console defaults to cp1252
    try:
        _s.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LOGDIR = os.path.join(ROOT, "runphases_logs")
LOCK = os.path.join(ROOT, "runphases.lock")

# the one name -> CLI id table (a model name in a kickoff that is not here stops the run)
MODEL_IDS = {"sonnet 5.5": "claude-sonnet-5-5", "opus 5.5": "claude-opus-5-5", "haiku 5.5": "claude-haiku-5-5"}
EFFORTS = ("low", "medium", "high", "max")
SKIP_STATES = ("n/a", "dropped", "merged", "superseded")
# files these phases edit: dirty copies mean a peer or a failed run is mid-edit
OWNED = ["X2Lib/X2GUAra.*", "X2Lib/X2GUUser.*", "KTDXLIB/Always.h", "X2Lib/X2Gage*"]
CODE_EXT = (".cpp", ".h", ".hpp", ".inl", ".c", ".cc")
CODE_DIRS = ("X2Lib/", "KTDXLIB/", "X2/", "X2ServerProtocol/")


class Stop(Exception):
    pass


def sh(args, **kw):
    return subprocess.run(args, cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", **kw)


def rel(path):
    try:
        return os.path.relpath(path, ROOT)
    except ValueError:  # another drive (tests)
        return path


def git(*a):
    r = sh(["git"] + list(a))
    if r.returncode != 0:
        raise Stop("git %s failed: %s" % (" ".join(a), r.stderr.strip()[:300]))
    return r.stdout


# ---------------------------------------------------------------- STATUS / kickoffs
def read(path):
    with open(path, "rb") as f:
        return f.read().decode("utf-8", "replace")


def clean_cell(c):
    return c.strip().strip("*`").strip()


def parse_block(status_text, plan):
    """-> (plan_doc, [row dicts in table order]) for the `## <plan>` block."""
    lines = status_text.splitlines()
    start = None
    for i, l in enumerate(lines):
        if re.match(r"^## %s(\s|$)" % re.escape(plan), l):
            start = i
            break
    if start is None:
        raise Stop("no `## %s` block in STATUS.md" % plan)
    end = len(lines)
    for j in range(start + 1, len(lines)):
        if lines[j].startswith("## "):
            end = j
            break
    block = lines[start:end]
    m = re.search(r"Plan `([^`]+\.md)`", "\n".join(block[:4]))
    plan_doc = m.group(1) if m else None
    rows = []
    for l in block:
        if not l.startswith("|"):
            continue
        cells = [c for c in l.strip().strip("|").split("|")]
        if len(cells) < 4:
            continue
        pid = clean_cell(cells[0])
        if pid.lower() in ("phase", "") or set(pid) <= set("-: "):
            continue
        rows.append({"id": pid, "state": clean_cell(cells[1]), "model_cell": cells[2].strip()})
    return plan_doc, rows


def row_range(rows, start_id, until_id):
    ids = [r["id"] for r in rows]
    if start_id not in ids:
        raise Stop("--from %s is not a row of the block" % start_id)
    a = ids.index(start_id)
    b = ids.index(until_id) if until_id in ids else len(ids)
    if b < a:
        raise Stop("--until %s comes before --from %s" % (until_id, start_id))
    return rows[a:b]


def kickoff_model(path):
    """-> (model name, effort) from the kickoff's `| Start |` line, or raise Stop."""
    if not os.path.isfile(path):
        raise Stop("kickoff missing: %s (the phase that added the row, or the phase it reviews, must write it)" % rel(path))
    m = None
    for l in read(path).splitlines():
        if l.startswith("| Start"):
            m = re.search(r"\*\*(Sonnet|Opus|Haiku)\s+([0-9.]+)\s*,\s*(\w+)\*\*", l)
            break
    if not m:
        raise Stop("no `**<Model> <ver>, <effort>**` on the Start line of %s" % rel(path))
    name = ("%s %s" % (m.group(1), m.group(2))).lower()
    eff = m.group(3).lower()
    if name not in MODEL_IDS:
        raise Stop("unknown model name %r in %s (add it to MODEL_IDS)" % (name, rel(path)))
    if eff not in EFFORTS:
        raise Stop("unknown effort %r in %s" % (eff, rel(path)))
    return name, eff


def status_models(model_cell):
    """STATUS 'Sonnet 5.5 / high, Opus review, user play-test' -> (phase model or None, effort or None, has_review)."""
    m = re.match(r"\s*(Sonnet|Opus|Haiku)\s+([0-9.]+)\s*/\s*(\w+)", model_cell)
    phase = ("%s %s" % (m.group(1), m.group(2))).lower() if m else None
    eff = m.group(3).lower() if m else None
    return phase, eff, bool(re.search(r"Opus review", model_cell))


def step_spec(plan, row, kind, kdir):
    pid = row["id"]
    fname = pid + ("-review.md" if kind == "review" else ".md")
    name, eff = kickoff_model(os.path.join(kdir, plan, fname))
    s_model, s_eff, _ = status_models(row["model_cell"])
    if kind == "phase" and s_model and (s_model != name or (s_eff and s_eff != eff)):
        raise Stop("model mismatch for %s: kickoff says %s / %s, STATUS says %s / %s (fix one of them)"
                   % (pid, name, eff, s_model, s_eff))
    return {"id": pid, "kind": kind, "model": MODEL_IDS[name], "model_name": name, "effort": eff}


# ---------------------------------------------------------------- done detection
def subjects(plan):
    return git("log", "main", "--format=%s", "-n", "2000").splitlines()


def is_done(plan, pid, kind, subj):
    review = re.compile(r"^%s Phase %s review\b" % (re.escape(plan), re.escape(pid)))
    phase = re.compile(r"^%s Phase %s(:| )" % (re.escape(plan), re.escape(pid)))
    for s in subj:
        if kind == "review" and review.search(s):
            return True
        if kind == "phase" and phase.search(s) and not review.search(s):
            return True
    return False


def next_step(plan, args, status_path, kdir):
    """Recompute the list from disk: -> (next step or None, listing, plan doc). Phases first, then reviews."""
    plan_doc, rows = parse_block(read(status_path), plan)
    rng = row_range(rows, args.start, args.until)
    subj = subjects(plan)
    live = [r for r in rng if not r["state"].lower().startswith(SKIP_STATES)]
    listing = []
    for r in live:
        done = is_done(plan, r["id"], "phase", subj)
        listing.append(("phase", r, done, None if done else step_spec(plan, r, "phase", kdir)))
    for r in live:
        if status_models(r["model_cell"])[2]:
            listing.append(("review", r, is_done(plan, r["id"], "review", subj), None))
    for kind, r, done, spec in listing:
        if kind == "phase" and not done:
            return spec, listing, plan_doc
    for kind, r, done, spec in listing:
        if kind == "review" and not done and is_done(plan, r["id"], "phase", subj):
            return step_spec(plan, r, "review", kdir), listing, plan_doc
    return None, listing, plan_doc


# ---------------------------------------------------------------- preflight
def pid_alive(pid):
    r = sh(["tasklist", "/FI", "PID eq %d" % pid, "/NH"])
    return str(pid) in r.stdout


def take_lock():
    if os.path.isfile(LOCK):
        try:
            pid = int(read(LOCK).split()[0])
        except Exception:
            pid = 0
        if pid and pid_alive(pid):
            raise Stop("another runphases is running (pid %d, %s)" % (pid, LOCK))
    with open(LOCK, "w") as f:
        f.write("%d %s\n" % (os.getpid(), datetime.datetime.now().isoformat(timespec="seconds")))


def drop_lock():
    try:
        os.remove(LOCK)
    except OSError:
        pass


def dirty_owned():
    out = git("status", "--porcelain", "--", *OWNED)
    return [l for l in out.splitlines() if l.strip()]


def preflight(args):
    br = git("rev-parse", "--abbrev-ref", "HEAD").strip()
    if br != "main":
        raise Stop("branch is %r, the rule is one branch: main" % br)
    d = dirty_owned()
    if d:
        raise Stop("owned files have uncommitted changes (a peer or a failed run is mid-edit):\n  " + "\n  ".join(d))
    if "X2_offline.exe" in sh(["tasklist"]).stdout:
        raise Stop("the game (X2_offline.exe) is running")
    if not args.no_ghidra_check:
        try:
            socket.create_connection(("127.0.0.1", args.ghidra_port), timeout=3).close()
        except OSError:
            raise Stop("Ghidra is not reachable on 127.0.0.1:%d (CH1 phases transcribe from it); start it or pass "
                       "--no-ghidra-check" % args.ghidra_port)
    if not shutil.which("claude"):
        raise Stop("`claude` is not on PATH")


# ---------------------------------------------------------------- one step
def unattended_note(spec, plan):
    pid, kind = spec["id"], spec["kind"]
    done = "RUNPHASES DONE %s%s" % (pid, " review" if kind == "review" else "")
    lines = [
        "UNATTENDED RUN by tools/runphases.py: nobody can answer questions. Do not ask any. Follow the x2-phase skill and CLAUDE.md exactly.",
        "If you reach a decision that is the user's, a step that 'needs a higher model', a gate you cannot make PASS, "
        "or June behaviour you cannot read: commit nothing partial and end your final message with the line "
        "`RUNPHASES STOP %s: <reason in one line>`." % pid,
        "On success end your final message with the exact line `%s`." % done,
        "These steps have no play-test (nothing runs until the CH1 final switch): there is no play-test hand-off; "
        "a probe build replaces the full build, as the kickoff says.",
    ]
    if kind == "phase":
        lines.append("The user ordered every review of this run deferred to the end: treat any 'depends on <phase> review' "
                     "as satisfied and do not stop on it. Do not run or write a review in this step; if the kickoff for this "
                     "phase's review is missing, write it from kickoffs/TEMPLATE-review.md as x2-phase says.")
    else:
        lines.append("This is a review session: read the covered Done notes and commits, run the checks the review kickoff lists, "
                     "fix what is wrong, retire the debug defines, and write the next kickoff if missing, as x2-phase says.")
    return "\n".join(lines)


def run_claude(spec, plan, args, logpath):
    cmd = ["claude", "-p", "/x2-phase %s %s%s" % (plan, spec["id"], " review" if spec["kind"] == "review" else ""),
           "--model", spec["model"], "--effort", spec["effort"],
           "--permission-mode", args.permission_mode,
           "--output-format", "stream-json", "--verbose",
           "--append-system-prompt", unattended_note(spec, plan)]
    if args.max_usd:
        cmd += ["--max-budget-usd", str(args.max_usd)]
    return stream(cmd, logpath, args.heartbeat)


def stream(cmd, logpath, heartbeat):
    """Run, log raw lines, print a heartbeat; -> dict(result, cost, turns, is_error, rc, text)."""
    q = queue.Queue()
    p = subprocess.Popen(cmd, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, stdin=subprocess.DEVNULL,
                         text=True, encoding="utf-8", errors="replace", bufsize=1)

    def pump():
        for line in p.stdout:
            q.put(line)
        q.put(None)
    threading.Thread(target=pump, daemon=True).start()
    out = {"result": "", "cost": 0.0, "turns": 0, "is_error": False, "rc": None, "last_text": "", "subtype": "", "denials": []}
    tools = 0
    last_tool = ""
    t0 = time.time()
    try:
        with open(logpath, "w", encoding="utf-8") as lf:
            while True:
                try:
                    line = q.get(timeout=heartbeat)
                except queue.Empty:
                    print("    ... %ds, %d tool calls, last: %s" % (time.time() - t0, tools, last_tool or "-"), flush=True)
                    continue
                if line is None:
                    break
                lf.write(line)
                lf.flush()
                try:
                    ev = json.loads(line)
                except ValueError:
                    continue
                t = ev.get("type")
                if t == "assistant":
                    for b in (ev.get("message") or {}).get("content") or []:
                        if b.get("type") == "tool_use":
                            tools += 1
                            last_tool = b.get("name", "")
                        elif b.get("type") == "text" and b.get("text", "").strip():
                            out["last_text"] = b["text"]
                elif t == "result":
                    out["result"] = ev.get("result") or ""
                    out["cost"] = float(ev.get("total_cost_usd") or ev.get("cost_usd") or 0)
                    out["turns"] = ev.get("num_turns", 0)
                    out["is_error"] = bool(ev.get("is_error")) or str(ev.get("subtype", "")).startswith("error")
                    out["subtype"] = ev.get("subtype", "")
                    out["denials"] = [d.get("tool_name", "?") + ": " + str((d.get("tool_input") or {}).get("command", ""))[:160]
                                      for d in (ev.get("permission_denials") or [])]
        p.wait()
        out["rc"] = p.returncode
    except KeyboardInterrupt:
        p.terminate()
        try:
            p.wait(10)
        except Exception:
            p.kill()
        raise
    out["tools"] = tools
    return out


# ---------------------------------------------------------------- gates
def check_gates(spec, plan, plan_doc, start_sha, res):
    pid, kind = spec["id"], spec["kind"]
    problems = []
    text = res["result"] or res["last_text"]
    if res["rc"] != 0 or res["is_error"]:
        problems.append("claude exited rc=%s is_error=%s subtype=%s" % (res["rc"], res["is_error"], res["subtype"]))
    marker = "RUNPHASES DONE %s%s" % (pid, " review" if kind == "review" else "")
    if marker not in text:
        stop = re.search(r"RUNPHASES STOP[^\n]*", text)
        problems.append(stop.group(0) if stop else "final message has no `%s`" % marker)
    new_subj = git("log", "%s..HEAD" % start_sha, "--format=%s").splitlines()
    rx = (r"^%s Phase %s review\b" if kind == "review" else r"^%s Phase %s(:| )") % (re.escape(plan), re.escape(pid))
    if not any(re.search(rx, s) and (kind == "review" or " review" not in s.split(":")[0]) for s in new_subj):
        problems.append("no new commit `%s Phase %s%s` (new commits: %s)" % (plan, pid, " review" if kind == "review" else "",
                                                                          "; ".join(new_subj) or "none"))
    if plan_doc and os.path.isfile(os.path.join(ROOT, plan_doc)):
        word = "Review" if kind == "review" else "Done"
        if not re.search(r"^#{2,4} %s - %s\b" % (re.escape(pid), word), read(os.path.join(ROOT, plan_doc)), re.M):
            problems.append("%s has no `### %s - %s` heading" % (plan_doc, pid, word))
    if kind == "phase":
        files = [f for f in git("diff", "--name-only", "%s..HEAD" % start_sha).splitlines()
                 if f.endswith(CODE_EXT) and f.startswith(CODE_DIRS)]
        if files:
            r = sh([sys.executable, os.path.join(ROOT, "tools", "junediverge.py"), "--check"] + files)
            tail = (r.stdout + r.stderr).strip().splitlines()[-1:] or [""]
            if "RESULT: PASS" not in (r.stdout + r.stderr):
                problems.append("junediverge --check on %d file(s): %s" % (len(files), tail[0]))
    d = dirty_owned()
    if d:
        problems.append("owned files left uncommitted: " + ", ".join(x.strip() for x in d))
    return problems


def active_debug_defines():
    p = os.path.join(ROOT, "KTDXLIB", "Always.h")
    try:
        txt = read(p)
    except OSError:
        return []
    return re.findall(r"^\s*#define\s+(SERV_IRUHADEV_JUNE_CHAR_\w*_DEBUG)\b", txt, re.M)


def summary_row(spec, t0, secs, res, sha, verdict):
    os.makedirs(LOGDIR, exist_ok=True)
    p = os.path.join(LOGDIR, "summary.tsv")
    new = not os.path.isfile(p)
    with open(p, "a", encoding="utf-8") as f:
        if new:
            f.write("id\tkind\tmodel\teffort\tstart\tseconds\tcost_usd\tturns\tcommit\tverdict\n")
        f.write("%s\t%s\t%s\t%s\t%s\t%d\t%.2f\t%s\t%s\t%s\n" % (
            spec["id"], spec["kind"], spec["model"], spec["effort"], t0.isoformat(timespec="seconds"), secs,
            res["cost"] if res else 0, res["turns"] if res else "", sha, verdict))


# ---------------------------------------------------------------- selftest
def selftest(args):
    if not shutil.which("claude"):
        raise Stop("`claude` is not on PATH")
    os.makedirs(LOGDIR, exist_ok=True)
    cmd = ["claude", "-p", "/x2-phase status", "--model", MODEL_IDS["sonnet 5.5"], "--effort", "low",
           "--permission-mode", args.permission_mode, "--output-format", "stream-json", "--verbose",
           "--max-budget-usd", "2"]
    res = stream(cmd, os.path.join(LOGDIR, "selftest.jsonl"), args.heartbeat)
    text = res["result"] or res["last_text"]
    ok = res["rc"] == 0 and not res["is_error"] and re.search(r"char-ara", text, re.I) and "Unknown" not in text[:200]
    if res["denials"]:
        print("permission denials in auto mode: %d" % len(res["denials"]))
        for d in res["denials"][:5]:
            print("  denied " + d)
    print("SELFTEST %s: rc=%s tools=%d cost=$%.2f" % ("OK" if ok else "FAIL", res["rc"], res["tools"], res["cost"]))
    print(text[:600])
    return 0 if ok else 1


# ---------------------------------------------------------------- main
def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0], formatter_class=argparse.RawDescriptionHelpFormatter,
                                 epilog=__doc__)
    ap.add_argument("plan", nargs="?", help="STATUS.md block key, e.g. char-ara")
    ap.add_argument("--from", dest="start", default="CH1a", help="first row id (default CH1a)")
    ap.add_argument("--until", default="CH1", help="sentinel row id, exclusive (default CH1: the deploy phase)")
    ap.add_argument("--stop-after", help="stop after this step id (a phase id, or ID:review)")
    ap.add_argument("--max-steps", type=int, default=60)
    ap.add_argument("--max-usd", type=float, help="per-step budget cap (claude --max-budget-usd)")
    ap.add_argument("--permission-mode", default="auto", help="claude --permission-mode (default auto)")
    ap.add_argument("--heartbeat", type=int, default=30, help="seconds between progress lines")
    ap.add_argument("--no-ghidra-check", action="store_true")
    ap.add_argument("--ghidra-port", type=int, default=8080)
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--selftest", action="store_true")
    ap.add_argument("--status", default=os.path.join(ROOT, "STATUS.md"), help="STATUS.md path (tests)")
    ap.add_argument("--kickoffs", default=os.path.join(ROOT, "kickoffs"), help="kickoffs folder (tests)")
    args = ap.parse_args()
    try:
        if args.selftest:
            return selftest(args)
        if not args.plan:
            ap.error("plan is required")
        plan = args.plan
        if args.dry_run:
            step, listing, plan_doc = next_step(plan, args, args.status, args.kickoffs)
            print("plan doc: %s   range: %s .. (before %s)" % (plan_doc, args.start, args.until))
            for kind, r, done, spec in listing:
                if spec is None and not done and kind == "review":
                    try:
                        spec = step_spec(plan, r, "review", args.kickoffs)
                    except Stop as e:
                        spec = {"model_name": "?", "effort": "?", "note": str(e)}
                m = "%s / %s" % (spec["model_name"], spec["effort"]) if spec else "-"
                print("  %-7s %-8s %-8s %s%s" % (kind, r["id"], "DONE" if done else "pending", m,
                                                  ("   <- " + spec["note"]) if spec and spec.get("note") else ""))
            print("next step: %s" % (("%s %s" % (step["kind"], step["id"])) if step else "none (everything done)"))
            return 0
        preflight(args)
        take_lock()
        seen = set()
        n = 0
        t_all = time.time()
        total = 0.0
        results = []
        try:
            while True:
                step, listing, plan_doc = next_step(plan, args, args.status, args.kickoffs)
                for kind, r, done, spec in listing:
                    if r["id"] not in seen:
                        if seen:
                            print("NEW: %s appeared in the block" % r["id"])
                        seen.add(r["id"])
                if step is None:
                    break
                if n >= args.max_steps:
                    raise Stop("--max-steps %d reached" % args.max_steps)
                n += 1
                if step["kind"] == "review" and not os.path.isfile(os.path.join(args.kickoffs, plan, step["id"] + "-review.md")):
                    raise Stop("review kickoff missing: kickoffs/%s/%s-review.md" % (plan, step["id"]))
                d = dirty_owned()
                if d:
                    raise Stop("owned files dirty before %s: %s" % (step["id"], ", ".join(x.strip() for x in d)))
                os.makedirs(LOGDIR, exist_ok=True)
                t0 = datetime.datetime.now()
                stamp = t0.strftime("%Y%m%d_%H%M%S")
                logpath = os.path.join(LOGDIR, "%s_%s_%s%s.jsonl" % (stamp, plan, step["id"], "_review" if step["kind"] == "review" else ""))
                start_sha = git("rev-parse", "HEAD").strip()
                print("[%s] %s %s  (%s, %s)  log %s" % (t0.strftime("%H:%M:%S"), step["kind"].upper(), step["id"],
                                                         step["model_name"], step["effort"], os.path.relpath(logpath, ROOT)), flush=True)
                res = run_claude(step, plan, args, logpath)
                secs = (datetime.datetime.now() - t0).total_seconds()
                total += res["cost"]
                problems = check_gates(step, plan, plan_doc, start_sha, res)
                sha = git("rev-parse", "--short", "HEAD").strip()
                if problems:
                    summary_row(step, t0, secs, res, sha, "FAIL: " + " | ".join(problems)[:300])
                    print("  FAIL %s (%.0fs, $%.2f, %d tool calls, %d denials):" % (step["id"], secs, res["cost"], res["tools"], len(res["denials"])))
                    for dn in res["denials"][:5]:
                        print("    denied " + dn)
                    for pr in problems:
                        print("    - " + pr)
                    print("  final message:\n" + ("\n".join("    " + x for x in (res["result"] or res["last_text"]).strip().splitlines()[-25:])))
                    print("  resume after fixing: python tools/runphases.py %s --from %s" % (plan, args.start))
                    return 1
                dbg = active_debug_defines()
                if res["denials"]:
                    print("  note: %d tool call(s) were denied by permission mode %s (the phase worked around them):" % (len(res["denials"]), args.permission_mode))
                    for dn in res["denials"][:5]:
                        print("    " + dn)
                summary_row(step, t0, secs, res, sha, "PASS" + (" (%d denials)" % len(res["denials"]) if res["denials"] else ""))
                print("  PASS %s (%.0fs, $%.2f, %d tool calls) commit %s%s" % (
                    step["id"], secs, res["cost"], res["tools"], sha,
                    ("   [active debug defines: %s]" % ", ".join(dbg)) if (dbg and step["kind"] == "review") else ""), flush=True)
                results.append((step["kind"], step["id"], secs, res["cost"]))
                key = step["id"] + (":review" if step["kind"] == "review" else "")
                if args.stop_after and args.stop_after in (key, step["id"]) and (":" in args.stop_after or step["kind"] == "phase"):
                    print("stopped after %s (--stop-after)" % key)
                    break
        finally:
            drop_lock()
        print("\nDONE: %d step(s), %.0f min, $%.2f total" % (len(results), (time.time() - t_all) / 60, total))
        for kind, pid, secs, cost in results:
            print("  %-6s %-6s %5.0fs  $%.2f" % (kind, pid, secs, cost))
        dbg = active_debug_defines()
        if dbg:
            print("note: active debug defines still in Always.h: " + ", ".join(dbg))
        return 0
    except Stop as e:
        print("STOP: %s" % e)
        return 2
    except KeyboardInterrupt:
        print("interrupted")
        return 3


if __name__ == "__main__":
    sys.exit(main())
