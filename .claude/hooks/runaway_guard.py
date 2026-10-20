"""Stops the stuck-python-REPL disk fill (a background task's .output grew to 8 GB, 2026-10-20).

  bash   PreToolUse on Bash: blocks a bare `python` / `python -` that has no script file or piped stdin,
         and any stdin-fed `python -` / `python - <<EOF` when the call is run_in_background.
  sweep  SessionStart: kills no process; reports task .output files over 200 MB under %TEMP%\\claude.
"""
import glob
import json
import os
import re
import sys

LIMIT = 200 * 1024 * 1024
PY = r"(?:[\w:./\\-]*[\\/])?(?:python[\d.]*|py)(?:\.exe)?"
# a python word that starts a command (after ; && || newline, not after a pipe), then nothing or a lone "-"
BARE = re.compile(r"(?:^|;|&&|\|\||\n)\s*" + PY + r"\s*(?:-\s*)?(?=$|;|&|\||\n|\))", re.I)
STDIN_SCRIPT = re.compile(r"(?:^|;|&&|\|\||\n|\|)\s*" + PY + r"(?:\s+-[A-Za-z]+)*\s+-(?=\s|$)", re.I)


def bash(ev):
    ti = ev.get("tool_input") or {}
    cmd = ti.get("command") or ""
    if BARE.search(cmd):
        why = "a bare `python` with no script or piped input starts an interactive REPL"
    elif ti.get("run_in_background") and STDIN_SCRIPT.search(cmd):
        why = "a stdin-fed `python -` in a background task can fall into the REPL loop"
    else:
        return 0
    sys.stderr.write(
        "BLOCKED by .claude/hooks/runaway_guard.py: %s. Without a real console it prints WinError 123 forever and "
        "fills the task .output file (8 GB per task, 2026-10-20). Write the script to a file in the scratchpad "
        "and run `python -I script.py`.\n" % why)
    return 2


def sweep(_ev):
    root = os.path.join(os.environ.get("LOCALAPPDATA", ""), "Temp", "claude")
    big = []
    for p in glob.glob(os.path.join(root, "*", "*", "tasks", "*.output")):
        try:
            if os.path.getsize(p) > LIMIT:
                big.append((os.path.getsize(p), p))
        except OSError:
            pass
    if big:
        lines = ["%d MB  %s" % (s // 1048576, p) for s, p in sorted(big, reverse=True)]
        print("WARNING: oversized background-task logs (likely a runaway process; find its PID, kill by PID, "
              "delete the file, tell the user):\n" + "\n".join(lines))
    return 0


def main():
    try:
        ev = json.load(sys.stdin)
        mode = sys.argv[1] if len(sys.argv) > 1 else "bash"
        if mode == "bash":
            return bash(ev) if ev.get("tool_name") == "Bash" else 0
        return sweep(ev)
    except Exception:
        return 0


if __name__ == "__main__":
    sys.exit(main())
