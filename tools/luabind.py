#!/usr/bin/env python3
"""luabind.py - which Lua methods a June script calls that March never bound.

A June script that calls `obj:Name()` for a Name March did not register with lua_tinker dies at that line at run
time: one deduped ErrorLog line ("can't find 'Name' class variable"), the rest of the function silently skipped.
That is how Oberon's slash (GetIsNowPVPCorrectionGame) went missing. This reads the scripts instead of waiting for a
play-test.

  python tools/luabind.py check NAME [NAME ...]    June script(s) by name (june_src_fixed/*/NAME.src.lua), or a path
  python tools/luabind.py check --dir june_src_fixed/data037_kom   every script in a folder, with a per-name census
  python tools/luabind.py names                    list the names March registers (for a spot check)

Exit 1 when a script calls a method that is neither registered in March nor listed in tools/data/lua_unbound_known.tsv
(columns: name, phase, reason; a name goes there only when a phase decided to leave it absent, and says why).
Names are matched by method name only (the class is not checked), so a method bound on the wrong class is not found;
a registration inside a dead #if is counted as bound. Both are false negatives, never false alarms.
"""
import argparse, collections, csv, glob, os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC_DIRS = ("X2Lib", "KTDXLIB", "X2ServerProtocol")
KNOWN = os.path.join(ROOT, "tools", "data", "lua_unbound_known.tsv")
GAP = os.path.join(ROOT, "june2017_re", "c3z4_class_gap.tsv")

# lua_tinker::class_def<T>( L, "Name", ...), decl( L, "Name", ...), def(...), class_mem(...): the name is the 2nd argument
REG = re.compile(r'(?:class_def|class_mem|class_add|decl|def|set)\s*(?:<[^()]*?>)?\s*\(\s*[\w>.\-*&()]+?\s*,\s*"(\w+)"')
CALL = re.compile(r':(\w+)\s*\(')


def march_names():
    names = set()
    for d in SRC_DIRS:
        for f in glob.glob(os.path.join(ROOT, d, "*.cpp")) + glob.glob(os.path.join(ROOT, d, "*.h")):
            t = open(f, "rb").read().decode("cp949", "replace")
            if "lua_tinker" not in t and "LuaBinder" not in t:
                continue
            names.update(REG.findall(t))
    return names


def known():
    out = {}
    if os.path.exists(KNOWN):
        for row in csv.reader(open(KNOWN, encoding="utf-8"), delimiter="\t"):
            if row and not row[0].startswith("#") and row[0] != "name":
                out[row[0]] = row[1:]
    return out


def gap_notes():
    out = {}
    if os.path.exists(GAP):
        for row in csv.DictReader(open(GAP, encoding="utf-8"), delimiter="\t"):
            out.setdefault(row["method"], []).append("%s:%s" % (row["june_class"], row["march"]))
    return out


def find_script(name):
    if os.path.exists(name):
        return name
    hits = glob.glob(os.path.join(ROOT, "june_src_fixed", "*", name + ".src.lua"))
    hits += glob.glob(os.path.join(ROOT, "june_src_fixed", "*", name))
    return hits[0] if hits else None


def calls_in(path):
    t = open(path, encoding="utf-8-sig", errors="replace").read()
    t = re.sub(r"--\[\[.*?\]\]", "", t, flags=re.S)
    t = re.sub(r"--[^\n]*", "", t)
    uses = collections.defaultdict(list)
    for no, line in enumerate(t.split("\n"), 1):
        for m in CALL.finditer(line):
            uses[m.group(1)].append(no)
    return uses


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("cmd", choices=("check", "names"))
    ap.add_argument("targets", nargs="*", help="script name or path")
    ap.add_argument("--dir", help="check every *.lua in this folder")
    a = ap.parse_args()
    reg = march_names()
    if a.cmd == "names":
        print("\n".join(sorted(reg)))
        print("# %d names" % len(reg), file=sys.stderr)
        return 0

    files = []
    for t in a.targets:
        p = find_script(t)
        if not p:
            print("NOT FOUND: %s (june_src_fixed/*/%s.src.lua)" % (t, t))
            return 2
        files.append(p)
    if a.dir:
        files += sorted(glob.glob(os.path.join(a.dir, "*.lua")))
    if not files:
        ap.error("give a script name or --dir")

    kn, notes = known(), gap_notes()
    new = collections.defaultdict(list)
    bad_files = 0
    for p in files:
        uses = calls_in(p)
        unb = {n: l for n, l in uses.items() if n not in reg and n not in kn}
        if len(files) <= 12 or unb:
            print("%s: %d method(s) called, %d unbound" % (os.path.relpath(p, ROOT), len(uses), len(unb)))
        for n, lines in sorted(unb.items()):
            print("   UNBOUND %-40s line %s   %s" % (n, ",".join(map(str, lines[:4])), " ".join(notes.get(n, ["(not in c3z4_class_gap.tsv)"]))))
            new[n].append(os.path.basename(p))
        bad_files += bool(unb)
    if len(files) > 12:
        print("\n== census: %d unbound name(s) across %d of %d script(s)" % (len(new), bad_files, len(files)))
        for n, fs in sorted(new.items(), key=lambda kv: -len(kv[1]))[:40]:
            print("   %4d script(s)  %s" % (len(fs), n))
    print("RESULT: %s" % ("FAIL, %d unbound name(s): bind each whole from June's function, or add it to tools/data/lua_unbound_known.tsv with the reason" % len(new) if new else "PASS"))
    return 1 if new else 0


if __name__ == "__main__":
    sys.exit(main())
