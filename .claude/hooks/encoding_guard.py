"""Claude Code hook: keep the Edit/Write tools away from CP949/ANSI files.

pre  (PreToolUse, Edit|MultiEdit|Write): if the target file exists and is not valid UTF-8,
     block the call (exit 2) and tell the session to use tools/bytepatch.py instead.
post (PostToolUse, Edit|MultiEdit|Write): if the edited file lost a UTF-8 BOM or flipped
     its line endings compared with git HEAD, tell the session (exit 2 = message to Claude).

Fail-open by design: any internal problem exits 0, because this hook runs in every
session in this tree and a bug here must never block ordinary edits.
"""
import json, os, subprocess, sys

TOOLS = ("Edit", "MultiEdit", "Write")


def is_utf8(data):
    try:
        data.decode("utf-8")
        return True
    except UnicodeDecodeError:
        return False


def pre(ev):
    path = (ev.get("tool_input") or {}).get("file_path")
    if not path or not os.path.isfile(path):
        return 0
    with open(path, "rb") as f:
        data = f.read()
    if is_utf8(data):
        return 0
    rel = path
    sys.stderr.write(
        "BLOCKED by .claude/hooks/encoding_guard.py: %s is not UTF-8 (CP949/ANSI bytes, usually Korean comments).\n"
        "The Edit/Write tools re-encode such a file to UTF-8 and corrupt every Korean byte in it.\n"
        "Edit it byte-safely instead (run from the Trunk root):\n"
        "  1. python tools/bytepatch.py show \"%s\" START:END    # exact bytes: \\t, \\r, \\xNN visible\n"
        "  2. Write a patch spec file to your scratchpad with the Write tool (never a bash heredoc):\n"
        "       @@ file <path>\n"
        "       @@ insert_after            (or insert_before / replace; add occurrence=N if the anchor repeats)\n"
        "       @@ anchor\n"
        "       <one or more whole lines, copied exactly; tabs matter>\n"
        "       @@ text\n"
        "       <new lines, ASCII, wrapped in #ifdef SERV_IRUHADEV_...>\n"
        "       @@ end\n"
        "  3. python tools/bytepatch.py apply <spec> --dry-run, then without --dry-run.\n"
        "Full format: python tools/bytepatch.py --help (or the x2-byte-edit skill).\n" % (rel, rel))
    return 2


def git_head_bytes(path):
    d = os.path.dirname(path)
    top = subprocess.run(["git", "-C", d, "rev-parse", "--show-toplevel"], capture_output=True, text=True)
    if top.returncode != 0:
        return None
    root = top.stdout.strip()
    rel = os.path.relpath(path, root).replace("\\", "/")
    r = subprocess.run(["git", "-C", root, "show", "HEAD:%s" % rel], capture_output=True)
    return r.stdout if r.returncode == 0 else None


def post(ev):
    path = (ev.get("tool_input") or {}).get("file_path")
    if not path or not os.path.isfile(path):
        return 0
    with open(path, "rb") as f:
        now = f.read()
    old = git_head_bytes(path)
    if not old:
        return 0
    problems = []
    if old.startswith(b"\xef\xbb\xbf") and not now.startswith(b"\xef\xbb\xbf"):
        problems.append("the UTF-8 BOM that HEAD has is gone (BOMs are load-bearing on server .lua configs)")
    o_crlf, o_lf = old.count(b"\r\n"), old.count(b"\n")
    n_crlf, n_lf = now.count(b"\r\n"), now.count(b"\n")
    if o_lf > 20 and n_lf > 20:
        o_ratio, n_ratio = o_crlf / float(o_lf), n_crlf / float(n_lf)
        if abs(o_ratio - n_ratio) > 0.5:
            problems.append("line endings flipped: HEAD is %d%% CRLF, the file is now %d%% CRLF"
                            % (o_ratio * 100, n_ratio * 100))
    if not is_utf8(old) and is_utf8(now) and any(b >= 0x80 for b in old):
        problems.append("HEAD is CP949/ANSI but the file is now UTF-8: it was re-encoded")
    if not problems:
        return 0
    sys.stderr.write("WARNING from .claude/hooks/encoding_guard.py on %s: %s.\n"
                     "Check `git diff --stat -- \"%s\"`; if the diff is far larger than your edit, restore the file "
                     "(ask the user first) and redo the edit with tools/bytepatch.py.\n"
                     % (path, "; ".join(problems), path))
    return 2


def main():
    try:
        ev = json.load(sys.stdin)
        if ev.get("tool_name") not in TOOLS:
            return 0
        mode = sys.argv[1] if len(sys.argv) > 1 else "pre"
        return pre(ev) if mode == "pre" else post(ev)
    except Exception:
        return 0


if __name__ == "__main__":
    sys.exit(main())
