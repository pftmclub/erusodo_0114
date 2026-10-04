"""Print named sections of a big plan doc instead of reading the whole file.

    python tools/docsection.py JUNE_BUFF_PORT_PHASES.md --list            # outline with sizes
    python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Phase 6b"
    python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Phase 5c" --all  # every heading that matches

A name matches a heading when it is a case-insensitive prefix of the heading text
(after the #s), with dashes normalised ("--", en and em dash are the same). An exact
match wins; otherwise the FIRST match is printed and the others are listed, unless
--all. A section runs to the next heading of the same or a higher level, so a ##
section includes its ### subsections; use --no-sub to stop at the first subheading.

If a section body is only a pointer of the form "Moved to <FILE>" (an archived
section), the same heading is printed from that file instead.
"""
import argparse, os, re, sys

for _s in (sys.stdout, sys.stderr):  # the Windows console defaults to cp1252
    try:
        _s.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass

TRUNK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
HEAD = re.compile(r"^(#{1,6})\s+(.*?)\s*$")
MOVED = re.compile(r"Moved to `?([\w./-]+\.md)`?")


def norm(s):
    s = s.replace("—", "-").replace("–", "-").replace("--", "-")
    return re.sub(r"\s+", " ", s).strip().lower()


def load(path):
    fp = path if os.path.isabs(path) else os.path.join(TRUNK, path)
    if not os.path.exists(fp) and os.path.exists(path):
        fp = path
    return fp, open(fp, encoding="utf-8-sig").read().split("\n")


def outline(lines):
    heads = []
    in_code = False
    for i, l in enumerate(lines):
        if l.startswith("```"):
            in_code = not in_code
            continue
        if in_code:
            continue
        m = HEAD.match(l)
        if m:
            heads.append((i, len(m.group(1)), m.group(2)))
    return heads


def section(lines, heads, k, no_sub):
    i, lvl, _ = heads[k]
    end = len(lines)
    for j, l2, _ in heads[k + 1:]:
        if l2 <= lvl or no_sub:
            end = j
            break
    return lines[i:end]


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("doc")
    ap.add_argument("names", nargs="*")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("--all", action="store_true")
    ap.add_argument("--no-sub", action="store_true")
    a = ap.parse_args()
    fp, lines = load(a.doc)
    heads = outline(lines)
    if a.list or not a.names:
        for k, (i, lvl, text) in enumerate(heads):
            size = sum(len(x) + 1 for x in section(lines, heads, k, True))
            print("%5d %6.1fK %s%s" % (i + 1, size / 1024.0, "  " * (lvl - 1), text))
        return 0
    rc = 0
    for name in a.names:
        n = norm(name)
        exact = [k for k, h in enumerate(heads) if norm(h[2]) == n]
        pref = [k for k, h in enumerate(heads) if norm(h[2]).startswith(n)]
        hits = exact or pref
        if not hits:
            print("### NOT FOUND: %r in %s (use --list)" % (name, os.path.basename(fp)))
            rc = 1
            continue
        chosen = hits if a.all else hits[:1]
        if not a.all and len(hits) > 1 and not exact:
            print("<!-- %r also matches: %s -->" % (name, " | ".join(heads[k][2] for k in hits[1:6])))
        for k in chosen:
            body = section(lines, heads, k, a.no_sub)
            text = "\n".join(body)
            m = MOVED.search("\n".join(body[1:6]))
            if m and len(body) < 8:
                tgt = m.group(1)
                tfp, tlines = load(os.path.join(os.path.dirname(fp), tgt))
                th = outline(tlines)
                tk = [x for x, h in enumerate(th) if norm(h[2]) == norm(heads[k][2])]
                if tk:
                    print("<!-- %s (archived in %s) -->" % (heads[k][2], tgt))
                    text = "\n".join(section(tlines, th, tk[0], a.no_sub))
            print("<!-- %s:%d -->" % (os.path.basename(fp), heads[k][0] + 1))
            print(text.rstrip())
            print()
    return rc


if __name__ == "__main__":
    sys.exit(main())
