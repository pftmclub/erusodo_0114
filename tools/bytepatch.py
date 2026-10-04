"""Byte-safe edits for CP949/ANSI sources (the Edit/Write tools re-encode them; the hook blocks that).

    python tools/bytepatch.py info  FILE                 # encoding class, EOL counts, high-byte count
    python tools/bytepatch.py show  FILE 120:160         # lines with \\t \\r \\xNN made visible (exact anchor text)
    python tools/bytepatch.py find  FILE "text"          # line numbers containing an ASCII substring
    python tools/bytepatch.py apply SPEC [--dry-run]     # apply a patch spec (below)

Patch spec: a plain text file written with the Write tool (never a heredoc: bash eats
backslashes). No escaping anywhere: block contents are taken literally, line by line.

    @@ file X2Lib/X2GUUser.cpp
    @@ insert_after
    @@ anchor
    	m_bDoubleAttack = false;
    @@ text
    #ifdef SERV_IRUHADEV_FOO
    	DoFoo();
    #endif SERV_IRUHADEV_FOO
    @@ end

  ops:  insert_after   text goes after the anchor's last line
        insert_before  text goes before the anchor's first line
        replace        the anchor lines are replaced by the text (prefer #ifdef/#else over this)
  options on the op line: occurrence=N (required when the anchor matches more than once),
        allow_non_ascii (text is encoded CP949; otherwise non-ASCII text is refused)
  anchor: one or more WHOLE lines, compared exactly (tabs vs spaces matter; use `show`).
        `@@ anchor_hex` instead takes hex bytes of one line without its EOL, for anchors
        that contain Korean bytes.
  Several edits and several `@@ file` sections may share one spec; edits apply to the
  original bytes, so line numbers in error messages refer to the file before the patch.

Line endings: every inserted line gets the EOL of the anchor line it sits next to
(CRLF in CRLF regions, LF in LF-native files such as X2UserSkillTree.cpp). Nothing
outside the edited lines is touched; the result is checked for that before writing.
"""
import argparse, os, re, sys

for _s in (sys.stdout, sys.stderr):  # the Windows console defaults to cp1252
    try:
        _s.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass

TRUNK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def resolve(path):
    if os.path.isabs(path):
        return path
    for base in (os.getcwd(), TRUNK):
        fp = os.path.join(base, path)
        if os.path.exists(fp):
            return fp
    return os.path.join(TRUNK, path)


def lines_of(data):
    """Split on \\n only (a lone \\r stays inside the line, as the compiler sees it)."""
    out, start = [], 0
    while start < len(data):
        i = data.find(b"\n", start)
        if i < 0:
            out.append(data[start:])
            break
        out.append(data[start:i + 1])
        start = i + 1
    return out


def content(line):
    if line.endswith(b"\r\n"):
        return line[:-2]
    if line.endswith(b"\n"):
        return line[:-1]
    return line


def eol(line):
    return b"\r\n" if line.endswith(b"\r\n") else (b"\n" if line.endswith(b"\n") else b"")


def enc_class(data):
    if all(b < 0x80 for b in data):
        return "ascii"
    try:
        data.decode("utf-8")
        return "utf8-bom" if data.startswith(b"\xef\xbb\xbf") else "utf8"
    except UnicodeDecodeError:
        return "non-utf8 (CP949/ANSI)"


def info(data):
    crlf = data.count(b"\r\n")
    lf = data.count(b"\n") - crlf
    high = sum(1 for b in data if b >= 0x80)
    return "%s, %d lines, CRLF %d, LF-only %d, bytes>=0x80: %d, size %d" % (
        enc_class(data), len(lines_of(data)), crlf, lf, high, len(data))


def visible(line):
    s = []
    for b in line:
        if b == 9:
            s.append("\\t")
        elif b == 13:
            s.append("\\r")
        elif b == 10:
            s.append("\\n")
        elif b == 92:
            s.append("\\\\")
        elif b < 32 or b >= 127:
            s.append("\\x%02x" % b)
        else:
            s.append(chr(b))
    return "".join(s)


# ------------------------------------------------------------------ spec parsing
def parse_spec(text):
    files, cur, edit, block = [], None, None, None
    for raw in text.split("\n"):
        line = raw[:-1] if raw.endswith("\r") else raw
        if line.startswith("@@ ") or line == "@@":
            words = line[3:].split()
            if not words:
                raise SystemExit("spec: empty @@ line")
            kw = words[0]
            if kw == "file":
                path = line[3:].strip()[len("file"):].strip()
                cur = {"path": path, "edits": []}
                files.append(cur)
                edit, block = None, None
            elif kw in ("insert_after", "insert_before", "replace"):
                if cur is None:
                    raise SystemExit("spec: '@@ %s' before any '@@ file'" % kw)
                edit = {"op": kw, "occurrence": None, "allow_non_ascii": False, "anchor": None,
                        "anchor_hex": None, "text": None}
                for w in words[1:]:
                    if w.startswith("occurrence="):
                        edit["occurrence"] = int(w.split("=", 1)[1])
                    elif w == "allow_non_ascii":
                        edit["allow_non_ascii"] = True
                    else:
                        raise SystemExit("spec: unknown option %r" % w)
                cur["edits"].append(edit)
                block = None
            elif kw in ("anchor", "anchor_hex", "text"):
                if edit is None:
                    raise SystemExit("spec: '@@ %s' outside an edit" % kw)
                edit[kw] = []
                block = kw
            elif kw == "end":
                block = None
            else:
                raise SystemExit("spec: unknown directive %r" % line)
            continue
        if block:
            edit[block].append(line)
    for f in files:
        for e in f["edits"]:
            for k in ("anchor", "anchor_hex", "text"):
                if e[k] is not None:  # drop trailing empty lines produced by the file's final newline
                    while e[k] and e[k][-1] == "" and k != "text":
                        e[k].pop()
            if e["text"] is not None:
                while e["text"] and e["text"][-1] == "":
                    e["text"].pop()
            if not e["anchor"] and not e["anchor_hex"]:
                raise SystemExit("spec: %s edit in %s has no anchor" % (e["op"], f["path"]))
            if e["text"] is None:
                raise SystemExit("spec: %s edit in %s has no text block" % (e["op"], f["path"]))
    return files


def anchor_lines(e):
    if e["anchor_hex"]:
        return [bytes.fromhex("".join(e["anchor_hex"]).replace(" ", ""))]
    out = []
    for l in e["anchor"]:
        try:
            out.append(l.encode("ascii"))
        except UnicodeEncodeError:
            raise SystemExit("anchor line %r has non-ASCII characters: use `show` and an @@ anchor_hex block, "
                             "or pick an ASCII-only line next to it" % l)
    return out


def find_anchor(lines, anc):
    hits = []
    n = len(anc)
    for i in range(len(lines) - n + 1):
        if all(content(lines[i + k]) == anc[k] for k in range(n)):
            hits.append(i)
    return hits


def near_misses(lines, anc):
    norm = lambda b: re.sub(rb"\s+", b" ", b).strip()
    first = norm(anc[0])
    return [i for i, l in enumerate(lines) if norm(content(l)) == first][:5]


def apply_file(f, dry):
    path = resolve(f["path"])
    if not os.path.exists(path):
        raise SystemExit("%s: no such file" % f["path"])
    data = open(path, "rb").read()
    lines = lines_of(data)
    before = info(data)
    plans = []  # (start_idx, end_idx_exclusive, new_lines)
    for n, e in enumerate(f["edits"], 1):
        anc = anchor_lines(e)
        hits = find_anchor(lines, anc)
        if not hits:
            msg = "%s edit %d (%s): anchor not found:\n    %s" % (f["path"], n, e["op"], "\n    ".join(visible(a) for a in anc))
            nm = near_misses(lines, anc)
            if nm:
                msg += "\n  same text with different whitespace at line(s): %s\n  exact bytes there:\n    %s" % (
                    ", ".join(str(i + 1) for i in nm), "\n    ".join(visible(content(lines[i])) for i in nm))
            raise SystemExit(msg)
        if e["occurrence"] is None and len(hits) > 1:
            raise SystemExit("%s edit %d: anchor matches %d times (lines %s); add occurrence=N or a longer anchor"
                             % (f["path"], n, len(hits), ", ".join(str(h + 1) for h in hits[:10])))
        occ = e["occurrence"] or 1
        if occ > len(hits):
            raise SystemExit("%s edit %d: occurrence=%d but only %d match(es)" % (f["path"], n, occ, len(hits)))
        i = hits[occ - 1]
        j = i + len(anc)
        texts = []
        for t in e["text"]:
            if e["allow_non_ascii"]:
                texts.append(t.encode("cp949"))
            else:
                try:
                    texts.append(t.encode("ascii"))
                except UnicodeEncodeError:
                    raise SystemExit("%s edit %d: text line %r is not ASCII (house rule: new comments in ASCII; "
                                     "add allow_non_ascii to encode it as CP949)" % (f["path"], n, t))
        if e["op"] == "insert_after":
            le = eol(lines[j - 1]) or (eol(lines[j - 2]) if j >= 2 else b"\r\n")
            new = [t + le for t in texts]
            if not eol(lines[j - 1]):  # anchor is the last line without EOL: give it one, keep text EOL-less at the end
                new = [le] + [t + le for t in texts[:-1]] + ([texts[-1]] if texts else [])
            plans.append((j, j, new, n, i))
        elif e["op"] == "insert_before":
            le = eol(lines[i]) or b"\r\n"
            plans.append((i, i, [t + le for t in texts], n, i))
        else:  # replace
            les = [eol(lines[k]) for k in range(i, j)]
            le = les[0] or b"\r\n"
            new = [t + le for t in texts]
            if new and not eol(lines[j - 1]):
                new[-1] = texts[-1]
            plans.append((i, j, new, n, i))
    plans.sort(key=lambda p: (p[0], p[1]))
    for a, b in zip(plans, plans[1:]):
        if b[0] < a[1] or (a[0] == b[0] and a[1] == b[1] and a[0] != a[1]):
            raise SystemExit("%s: edits %d and %d overlap" % (f["path"], a[3], b[3]))
    out = list(lines)
    for s, e_, new, n, i in sorted(plans, key=lambda p: (p[0], p[1]), reverse=True):
        out[s:e_] = new
    result = b"".join(out)
    # verify: removing the edited spans from both sides leaves identical bytes
    check, k = [], 0
    for s, e_, new, n, i in sorted(plans, key=lambda p: (p[0], p[1])):
        check.extend(lines[k:s])
        k = e_
    check.extend(lines[k:])
    kept = b"".join(check)
    removed_total = sum(len(b"".join(lines[s:e_])) for s, e_, *_ in plans)
    if len(kept) != len(data) - removed_total:
        raise SystemExit("internal check failed for %s; nothing written" % f["path"])
    for s, e_, new, n, i in sorted(plans, key=lambda p: p[0]):
        verb = {True: "insert", False: "replace"}[s == e_]
        print("  %s edit %d: %s %d line(s) at line %d%s, EOL %s" % (
            f["path"], n, verb, len(new), s + 1, (" replacing %d line(s)" % (e_ - s)) if s != e_ else "",
            "CRLF" if (new and new[0].endswith(b"\r\n")) else "LF"))
        if dry:
            for l in new[:12]:
                print("      + " + visible(content(l)))
            if len(new) > 12:
                print("      + ... %d more" % (len(new) - 12))
    after = info(result)
    if enc_class(result) != enc_class(data) and not (enc_class(data) == "ascii"):
        raise SystemExit("%s: encoding class would change (%s -> %s); nothing written" % (f["path"], enc_class(data), enc_class(result)))
    print("  before: %s" % before)
    print("  after:  %s%s" % (after, "  (dry run, not written)" if dry else ""))
    return path, data, result


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("info"); s.add_argument("file")
    s = sub.add_parser("show"); s.add_argument("file"); s.add_argument("range")
    s = sub.add_parser("find"); s.add_argument("file"); s.add_argument("text")
    s = sub.add_parser("apply"); s.add_argument("spec"); s.add_argument("--dry-run", action="store_true")
    a = ap.parse_args()
    if a.cmd == "info":
        print(info(open(resolve(a.file), "rb").read()))
    elif a.cmd == "show":
        lines = lines_of(open(resolve(a.file), "rb").read())
        m = re.match(r"^(\d+)(?::(\d+))?$", a.range)
        if not m:
            raise SystemExit("range is N or N:M")
        lo = int(m.group(1)); hi = int(m.group(2) or lo)
        for i in range(max(lo, 1), min(hi, len(lines)) + 1):
            print("%6d| %s" % (i, visible(lines[i - 1])))
    elif a.cmd == "find":
        needle = a.text.encode("ascii")
        for i, l in enumerate(lines_of(open(resolve(a.file), "rb").read()), 1):
            if needle in l:
                print("%6d| %s" % (i, visible(content(l))))
    else:
        spec = open(a.spec, "r", encoding="utf-8-sig").read()
        files = parse_spec(spec)
        if not files:
            raise SystemExit("spec has no '@@ file' section")
        planned = [apply_file(f, a.dry_run) for f in files]  # any failure exits before anything is written
        if not a.dry_run:
            for path, data, result in planned:
                if open(path, "rb").read() != data:
                    raise SystemExit("%s changed on disk while planning; nothing written" % path)
            for path, data, result in planned:
                tmp = path + ".bytepatch.tmp"
                with open(tmp, "wb") as fh:
                    fh.write(result)
                os.replace(tmp, path)
        print("done (dry run)" if a.dry_run else "done: %d file(s) written" % len(planned))


if __name__ == "__main__":
    main()
