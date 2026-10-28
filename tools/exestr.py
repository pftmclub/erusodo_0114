#!/usr/bin/env python3
"""exestr.py - prove strings are (or are not) inside the built / deployed exe.

    python tools/exestr.py "[JUNE_FX] cast" "Open Mind"      # deployed X2_offline.exe, narrow + UTF-16
    python tools/exestr.py --file strings.txt                # one string per line, '#' comments
    python tools/exestr.py --form wide "Code Recovery"       # only the UTF-16LE form counts
    python tools/exestr.py --absent "[DEBUG_TAG]"            # flag-off proof: must NOT be present
    python tools/exestr.py --exe build "[JUNE_FX]"           # X2/US_SERVICE/x2.exe instead of the deploy

Exit 0 when every string passes, 1 when any fails, 2 on bad input / missing exe.
Run it after `build.py --deploy` printed DEPLOY OK; paste the EXESTR line into the Done note.
"""
import argparse
import hashlib
import os
import sys
import time

TRUNK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME_DIR = os.environ.get("X2_GAME_DIR", r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data")
DEPLOY_NAME = "X2_offline.exe"


def resolve_exe(arg, config):
    if arg in ("deploy", None):
        return os.path.join(GAME_DIR, DEPLOY_NAME)
    if arg == "build":
        return os.path.join(TRUNK, "X2", config, "x2.exe")
    return arg


def forms_for(text, form):
    out = []
    if form in ("any", "narrow"):
        out.append(("narrow", text.encode("cp949", errors="replace")))
    if form in ("any", "wide"):
        out.append(("wide", text.encode("utf-16-le")))
    return out


def scan(data, needle, limit=3):
    offs, pos = [], 0
    count = 0
    while True:
        i = data.find(needle, pos)
        if i < 0:
            break
        count += 1
        if len(offs) < limit:
            offs.append(i)
        pos = i + 1
    return count, offs


def load_strings(args):
    items = list(args.strings)
    if args.file:
        with open(args.file, encoding="utf-8-sig") as f:
            for line in f:
                line = line.rstrip("\r\n")
                if line and not line.lstrip().startswith("#"):
                    items.append(line)
    return items


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("strings", nargs="*", help="strings to look for (quote them)")
    ap.add_argument("--file", help="text file, one string per line")
    ap.add_argument("--exe", default="deploy",
                    help="'deploy' (default, game dir X2_offline.exe), 'build' (X2/<config>/x2.exe) or a path")
    ap.add_argument("--config", default="US_SERVICE", help="config dir for --exe build")
    ap.add_argument("--form", choices=["any", "narrow", "wide"], default="any",
                    help="narrow = cp949/ASCII bytes, wide = UTF-16LE; any = either counts (default)")
    ap.add_argument("--absent", action="store_true", help="pass only if the string is NOT present")
    ap.add_argument("-q", "--quiet", action="store_true", help="print only failures and the summary")
    args = ap.parse_args()

    items = load_strings(args)
    if not items:
        ap.error("give at least one string or --file")

    path = resolve_exe(args.exe, args.config)
    if not os.path.isfile(path):
        print("EXESTR ERROR: exe not found: %s" % path)
        return 2
    with open(path, "rb") as f:
        data = f.read()
    md5 = hashlib.md5(data).hexdigest()
    mtime = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(os.path.getmtime(path)))

    bad = 0
    for text in items:
        hits = []
        for name, needle in forms_for(text, args.form):
            n, offs = scan(data, needle)
            if n:
                hits.append((name, n, offs))
        found = bool(hits)
        ok = (not found) if args.absent else found
        if not ok:
            bad += 1
        if ok and args.quiet:
            continue
        if found:
            detail = "; ".join("%s x%d @%s" % (nm, n, ",".join("0x%x" % o for o in offs)) for nm, n, offs in hits)
        else:
            detail = "not present"
        print("  %-7s %r  (%s)" % ("OK" if ok else ("PRESENT" if args.absent else "MISSING"), text, detail))

    ident = "%s, %d B, dated %s, md5 %s" % (os.path.basename(path), len(data), mtime, md5)
    verb = "absent" if args.absent else "found"
    if bad:
        print("EXESTR FAILED: %d of %d not %s in %s" % (bad, len(items), verb, ident))
        return 1
    print("EXESTR OK: %d of %d %s in %s" % (len(items), len(items), verb, ident))
    return 0


if __name__ == "__main__":
    sys.exit(main())
