"""Flag hygiene check: does every change under a SERV_IRUHADEV_ flag vanish when the flag is undefined?

Replaces the per-review hand-written unifdef scripts. Byte-level; never decodes source text.

    python tools/flagcheck.py FLAG                      # every commit in the flag's lifetime + uncommitted changes
    python tools/flagcheck.py FLAG --range A..B         # commits in a range (e.g. one phase)
    python tools/flagcheck.py FLAG --commits c1 c2      # specific commits
    python tools/flagcheck.py FLAG --worktree           # only uncommitted changes vs HEAD (incl. new files)
    python tools/flagcheck.py FLAG --worktree --files X2Lib/A.cpp X2Lib/A.h   # only your files (peers share the tree)
    python tools/flagcheck.py FLAG --base REV [--at REV] [FILE ...]
                                                       # doc style: FILE@at with FLAG stripped vs FILE@base
                                                       # (also stripped); --at defaults to the working tree
    python tools/flagcheck.py FLAG --with JUNE_BUFF_DEBUG   # also strip the phase's debug flag
    python tools/flagcheck.py --selftest

FLAG may be given with or without the SERV_IRUHADEV_ prefix.

Per (commit, file) it reports:
  verdict   PASS   strip(new) == strip(old) byte for byte
            PASS~  equal once blank lines and //{{ //}} marker lines are ignored (count shown)
            DEFINE the define file (Always.h): only comments, blank lines and the #define changed
            OTHER  the change is inside other SERV_IRUHADEV_ flags, not this one (named)
            FAIL   something outside every SERV_IRUHADEV_ flag changed (first differing line shown)
            ERROR  a conditional mentioning the flag that this tool cannot evaluate (never a pass)
  -N        deleted lines (house rule: insert-only; deletions must be explained)
  nonascii  inserted lines carrying bytes >= 0x80
  eol!      inserted lines whose line ending differs from the line they were inserted next to
  enc       encoding class old->new (ascii / utf8 / utf8-bom / non-utf8); a change is flagged
  tok!      #else/#endif of this flag without the trailing flag token (house style)
The flag's #define is located and must be in KTDXLIB/Always.h only (X2ServerProtocol .vcxproj
is the documented second home of SERV_IRUHADEV_OFFLINE). Exit 0 only if nothing FAILs or ERRORs.
Non-vacuity: a file whose bytes did not change is never reported as a pass.
"""
import argparse, os, re, subprocess, sys

for _s in (sys.stdout, sys.stderr):  # the Windows console defaults to cp1252
    try:
        _s.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass

TRUNK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CODE_EXT = (".cpp", ".h", ".c", ".hpp", ".inl", ".cxx")
CODE_DIRS = ("X2/", "X2Lib/", "KTDXLIB/", "X2ServerProtocol/", "KncWX2Server/")
DEFINE_HOME = "KTDXLIB/Always.h"
# Flags the user allowed a second #define for, in a header included before Always.h is reachable
# (CLAUDE.md, offline-only override). The second file is checked like Always.h.
SECOND_HOMES = {
    "SERV_IRUHADEV_JUNE_BUFF_FACTOR_WIRE": "KncWX2Server/Common/ServerDefine.h",  # CommonPacket.h precedes Always.h (X2Main.h:4)
    "SERV_IRUHADEV_JUNE_PET_PREVIEW_LOOKUP": "KncWX2Server/Common/ServerDefine.h",  # ClientPacket_Global.h precedes Always.h (ClientPacket.h:11056)
    "SERV_IRUHADEV_JUNE_SKILL_SLOT_10": "KncWX2Server/Common/ServerDefine.h",  # CommonPacket.h precedes Always.h (X2Main.h:4)
    "SERV_IRUHADEV_JUNE_SKILL_ACQUIRE": "KncWX2Server/Common/ServerDefine.h",  # EventID_Client.h / ClientPacket.h precede Always.h
    "SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET": "KncWX2Server/Common/ServerDefine.h",  # EventID_Client.h / ClientPacket.h precede Always.h
    "SERV_IRUHADEV_JUNE_SKILL_PAGE": "KncWX2Server/Common/ServerDefine.h",  # ClientPacket.h precedes Always.h
    "SERV_IRUHADEV_JUNE_SKILL_CLASS_CHANGE": "KncWX2Server/Common/ServerDefine.h",  # ClientPacket.h precedes Always.h
    "SERV_IRUHADEV_JUNE_RANDOM_MISSION": "KncWX2Server/Common/ServerDefine.h",  # EventID_Client.h / ClientPacket.h precede Always.h
    "SERV_IRUHADEV_JUNE_UNSEAL_SKILL_NOT": "KncWX2Server/Common/ServerDefine.h",  # ClientPacket.h precedes Always.h
    "SERV_IRUHADEV_JUNE_RESULT_OFFLINE": "KncWX2Server/Common/ServerDefine.h",  # EventID_Client.h / ClientPacket.h precede Always.h
    "SERV_IRUHADEV_JUNE_DIVERGE_DYING_START": "KncWX2Server/Common/ServerDefine.h",  # ClientPacket.h precedes Always.h
    "SERV_IRUHADEV_JUNE_TRANSCEND_UNIT": "KncWX2Server/Common/ServerDefine.h",  # CommonPacket.h precedes Always.h (X2Main.h:4)
    "SERV_IRUHADEV_JUNE_CHAR_RENA_NF": "KncWX2Server/Common/ServerDefine.h",  # CommonPacket.h precedes Always.h (X2Main.h:4); it defines ADD_RENA_SYSTEM (char-rena CH1q)
    "SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE": "KncWX2Server/Common/ServerDefine.h",  # EventID_Client.h / ClientPacket.h precede Always.h
    "SERV_IRUHADEV_ADMIN_CHEATS": "KncWX2Server/Common/ServerDefine.h",  # EventID_Client.h / ClientPacket.h precede Always.h
    "SERV_IRUHADEV_JUNE_DIVERGE_PVP_OFFICIAL_SKILL": "KncWX2Server/Common/ServerDefine.h",  # ClientPacket.h precedes Always.h
}


def is_define_home(path, flags):
    """Always.h for every flag; a SECOND_HOMES file only for the flags it is the second home of."""
    return path.endswith(DEFINE_HOME) or any(path.endswith(SECOND_HOMES[f]) for f in flags if f in SECOND_HOMES)
PREFIX = "SERV_IRUHADEV_"


class Unevaluable(Exception):
    pass


# ------------------------------------------------------------------ git helpers
def git_bytes(*args):
    r = subprocess.run(["git", "-C", TRUNK] + list(args), capture_output=True)
    if r.returncode != 0:
        raise RuntimeError("git %s: %s" % (" ".join(args), r.stderr.decode(errors="replace").strip()))
    return r.stdout


def git(*args):
    return git_bytes(*args).decode("utf-8", errors="replace").strip()


def show(rev, path):
    """File bytes at rev (None if absent). rev None = working tree."""
    if rev is None:
        fp = os.path.join(TRUNK, path)
        return open(fp, "rb").read() if os.path.exists(fp) else None
    r = subprocess.run(["git", "-C", TRUNK, "show", "%s:%s" % (rev, path)], capture_output=True)
    return r.stdout if r.returncode == 0 else None


# ------------------------------------------------------------------ preprocessor strip
DIRECTIVE = re.compile(rb"^[ \t]*#[ \t]*(ifdef|ifndef|if|elif|else|endif)\b(.*)$", re.S)
TOKEN = re.compile(rb"\s*(defined|\(|\)|!|&&|\|\||[A-Za-z_]\w*|\d+[uUlL]*|.)")


def split_lines(data):
    return data.splitlines(keepends=True)


def body(line):
    return line.rstrip(b"\r\n")


def eval_expr(expr, targets):
    """3-valued: True/False/None(unknown). Target flags are undefined; anything else is unknown."""
    expr = re.sub(rb"//.*$", b"", expr)
    expr = re.sub(rb"/\*.*?\*/", b"", expr).strip()
    toks = [t for t in TOKEN.findall(expr) if t.strip()]
    pos = [0]

    def peek():
        return toks[pos[0]] if pos[0] < len(toks) else None

    def take():
        t = peek()
        pos[0] += 1
        return t

    def primary():
        t = take()
        if t is None:
            raise Unevaluable("truncated expression")
        if t == b"!":
            v = primary()
            return None if v is None else (not v)
        if t == b"(":
            v = orexp()
            if take() != b")":
                raise Unevaluable("unbalanced parentheses")
            return v
        if t == b"defined":
            if peek() == b"(":
                take()
                name = take()
                if take() != b")":
                    raise Unevaluable("bad defined()")
            else:
                name = take()
            return False if name and name.decode() in targets else None
        if re.match(rb"\d", t):
            return int(re.match(rb"\d+", t).group()) != 0
        if re.match(rb"[A-Za-z_]", t):
            return False if t.decode() in targets else None  # undefined macro in #if evaluates to 0
        raise Unevaluable("operator %r" % t)

    def andexp():
        v = primary()
        while peek() == b"&&":
            take()
            w = primary()
            v = False if (v is False or w is False) else (None if (v is None or w is None) else True)
        return v

    def orexp():
        v = andexp()
        while peek() == b"||":
            take()
            w = andexp()
            v = True if (v is True or w is True) else (None if (v is None or w is None) else False)
        return v

    v = orexp()
    if peek() is not None:
        raise Unevaluable("unsupported operator %r" % peek())
    return v


def mentions(rest, targets):
    names = set(n.decode() for n in re.findall(rb"[A-Za-z_]\w*", rest))
    return names & targets


def scan_comment_state(line, in_comment):
    """Whether a /* */ block comment is still open after this line (strings and // respected)."""
    i, n = 0, len(line)
    quote = None
    while i < n:
        c = line[i:i + 1]
        two = line[i:i + 2]
        if in_comment:
            if two == b"*/":
                in_comment = False
                i += 2
                continue
        elif quote:
            if c == b"\\":
                i += 2
                continue
            if c == quote:
                quote = None
        elif two == b"//":
            break
        elif two == b"/*":
            in_comment = True
            i += 2
            continue
        elif c in (b'"', b"'"):
            quote = c
        i += 1
    return in_comment


def strip(data, targets, tok_flag=None, stats=None):
    """Remove every conditional on `targets` as if they were undefined. Returns bytes.
    Frames: ['T', taking, taken_any, line_no] for target conditionals, ['O'] for others."""
    out = []
    stack = []
    lines = split_lines(data)

    def emitting():
        return all(f[1] for f in stack if f[0] == "T")

    in_comment = False
    for no, line in enumerate(lines, 1):
        starts_in_comment = in_comment
        in_comment = scan_comment_state(body(line), in_comment)
        m = None if starts_in_comment else DIRECTIVE.match(body(line))
        if not m:
            if emitting():
                out.append(line)
            continue
        kw, rest = m.group(1), m.group(2)
        hit = mentions(rest, targets)
        if kw in (b"ifdef", b"ifndef", b"if"):
            if hit and emitting():
                if kw == b"ifdef":
                    v = False
                elif kw == b"ifndef":
                    v = True
                else:
                    v = eval_expr(rest, targets)
                    if v is None:
                        raise Unevaluable("line %d: %s" % (no, body(line).decode("latin-1").strip()))
                stack.append(["T", v, v, no, hit])
            elif hit:  # inside a dropped region: track depth only
                stack.append(["T", False, True, no, hit])
            else:
                stack.append(["O"])
                if emitting():
                    out.append(line)
        elif kw == b"elif":
            if not stack:
                raise Unevaluable("line %d: #elif without #if" % no)
            f = stack[-1]
            if f[0] == "O":
                if hit:
                    raise Unevaluable("line %d: #elif on %s inside a non-flag #if chain" % (no, ",".join(hit)))
                if emitting():
                    out.append(line)
            else:
                if any(f2[0] == "T" and not f2[1] for f2 in stack[:-1]):
                    continue
                v = eval_expr(rest, targets)
                if v is None:
                    raise Unevaluable("line %d: #elif mixes the flag with other macros: %s"
                                      % (no, body(line).decode("latin-1").strip()))
                f[1] = (not f[2]) and v
                f[2] = f[2] or v
        elif kw == b"else":
            if not stack:
                raise Unevaluable("line %d: #else without #if" % no)
            f = stack[-1]
            if f[0] == "O":
                if emitting():
                    out.append(line)
            else:
                if stats is not None and tok_flag and not any(n.encode() in rest for n in f[4]):
                    stats["tok"] += 1
                f[1] = not f[2]
                f[2] = True
        elif kw == b"endif":
            if not stack:
                raise Unevaluable("line %d: #endif without #if" % no)
            f = stack.pop()
            if f[0] == "O":
                if emitting():
                    out.append(line)
            elif stats is not None and tok_flag and not any(n.encode() in rest for n in f[4]):
                stats["tok"] += 1
    if stack:
        raise Unevaluable("unterminated #if (opened at line %s)" % (stack[-1][3] if stack[-1][0] == "T" else "?"))
    return b"".join(out)


def strip_defines(data, targets):
    """Drop '#define FLAG' lines (Always.h) so the define file can be compared too."""
    return b"".join(l for l in split_lines(data)
                    if not re.match(rb"^[ \t]*#[ \t]*define[ \t]+(%s)\b" % b"|".join(t.encode() for t in targets), l))


def define_file_only(a, b):
    keep = lambda d: [body(l) for l in split_lines(d) if body(l).strip() and not body(l).lstrip().startswith(b"//")]
    return keep(a) == keep(b)


MARKER = re.compile(rb"^[ \t]*(//\{\{|//\}\})")


def loose(data):
    return [body(l) for l in split_lines(data) if body(l).strip() and not MARKER.match(body(l))]


def first_diff(a, b):
    la, lb = split_lines(a), split_lines(b)
    for i in range(max(len(la), len(lb))):
        x = la[i] if i < len(la) else b"<EOF>"
        y = lb[i] if i < len(lb) else b"<EOF>"
        if x != y:
            return i + 1, x, y
    return None


def enc_class(data):
    if data is None:
        return "-"
    if all(b < 0x80 for b in data):
        return "ascii"
    try:
        data.decode("utf-8")
        return "utf8-bom" if data.startswith(b"\xef\xbb\xbf") else "utf8"
    except UnicodeDecodeError:
        return "non-utf8"


def diff_stats(old, new, rev_old, rev_new, path):
    """Deleted lines, inserted non-ASCII lines, EOL mismatches; from git's own diff (raw bytes)."""
    if rev_new is None:
        raw = git_bytes("diff", "--no-color", "--no-ext-diff", "--minimal", "-U0", rev_old, "--", path)
    else:
        raw = git_bytes("diff", "--no-color", "--no-ext-diff", "--minimal", "-U0", rev_old, rev_new, "--", path)
    old_lines = split_lines(old or b"")
    dels = ins = nonascii = eolbad = 0
    anchor_eol = None
    for line in raw.split(b"\n"):
        if line.startswith(b"@@"):
            m = re.match(rb"@@ -(\d+)(?:,(\d+))? \+(\d+)", line)
            a, cnt = int(m.group(1)), int(m.group(2) or 1)
            idx = (a - 1) if cnt == 0 else (a - 1)  # pure insert: line a is the one before
            idx = min(max(idx, 0), len(old_lines) - 1) if old_lines else -1
            anchor_eol = None
            if idx >= 0:
                anchor_eol = b"\r\n" if old_lines[idx].endswith(b"\r\n") else (b"\n" if old_lines[idx].endswith(b"\n") else None)
        elif line.startswith(b"+") and not line.startswith(b"+++"):
            ins += 1
            if any(c >= 0x80 for c in line[1:]):
                nonascii += 1
            eol = b"\r\n" if line.endswith(b"\r") else b"\n"
            if anchor_eol and eol != anchor_eol:
                eolbad += 1
        elif line.startswith(b"-") and not line.startswith(b"---"):
            dels += 1
    return ins, dels, nonascii, eolbad


# ------------------------------------------------------------------ core
def all_iruhadev(*blobs):
    names = set()
    for b in blobs:
        if b:
            names |= set(n.decode() for n in re.findall(rb"SERV_IRUHADEV_\w+", b))
    return names


def classify(path, old, new, flag, extra=()):
    """(verdict, note, tok_missing). Pure function of the bytes; shared by the check and the selftest."""
    if new == old:
        return "SAME", "", 0
    targets = {flag} | set(extra)
    stats = {"tok": 0}
    try:
        s_new = strip(new or b"", targets, flag, stats)
        s_old = strip(old or b"", targets)
    except Unevaluable as e:
        return "ERROR", str(e), 0
    if is_define_home(path, targets):
        s_new, s_old = strip_defines(s_new, targets), strip_defines(s_old, targets)
        if s_new != s_old and define_file_only(s_old, s_new):
            return "DEFINE", "only comment/blank lines and the #define changed", stats["tok"]
    if s_new == s_old:
        return "PASS", "", stats["tok"]
    if loose(s_new) == loose(s_old):
        n = abs(len(split_lines(s_new)) - len(split_lines(s_old)))
        return "PASS~", "differs only by %d blank/marker line(s) outside the flag" % n, stats["tok"]
    others = all_iruhadev(old, new) - targets
    if others:
        try:
            a, b = strip(s_new, others), strip(s_old, others)
            if is_define_home(path, others):
                a, b = strip_defines(a, others), strip_defines(b, others)
                # another flag's block appended to the define file: its comment lines stay after the strip
                if a != b and define_file_only(b, a):
                    return "OTHER", "%s: other flags' comments/defines" % os.path.basename(path), stats["tok"]
            if a == b or loose(a) == loose(b):
                changed = []
                for o in sorted(others):  # o is involved if stripping everything else still leaves a diff
                    rest = others - {o}
                    if loose(strip(s_new, rest)) != loose(strip(s_old, rest)):
                        changed.append(o)
                return "OTHER", "change belongs to: %s" % ", ".join(changed or sorted(others)), stats["tok"]
        except Unevaluable:
            pass
    d = first_diff(s_old, s_new)
    note = ""
    if d:
        note = "flag-off differs at stripped line %d: old %r | new %r" % (d[0], body(d[1])[:100], body(d[2])[:100])
    return "FAIL", note, stats["tok"]


def check_pair(path, old, new, flag, rev_old, rev_new, extra=()):
    res = {"path": path, "dels": 0, "ins": 0, "nonascii": 0, "eolbad": 0,
           "enc": "%s->%s" % (enc_class(old), enc_class(new))}
    res["verdict"], res["note"], res["tok"] = classify(path, old, new, flag, extra)
    if res["verdict"] == "SAME":
        return res
    try:
        res["ins"], res["dels"], res["nonascii"], res["eolbad"] = diff_stats(old, new, rev_old, rev_new, path)
    except RuntimeError as e:
        res["note"] += " (diff stats unavailable: %s)" % e
    return res


def code_files_changed(rev_old, rev_new):
    if rev_new is None:  # working tree: tracked changes plus new untracked files
        out = "\n".join([git("diff", "--name-only", rev_old), git(
            "ls-files", "--others", "--exclude-standard", "--", *[d.rstrip("/") for d in CODE_DIRS])])
    else:
        out = git("diff", "--name-only", rev_old, rev_new)
    return [f for f in dict.fromkeys(out.splitlines()) if f.lower().endswith(CODE_EXT) and f.startswith(CODE_DIRS)]


def define_sites(flag):
    # git grep exits 1 on no match: a flag whose #define is commented out ( a _DEBUG turned OFF ) has no sites.
    r = subprocess.run(["git", "-C", TRUNK, "grep", "-n", "-E", r"^\s*#\s*define\s+%s\b" % flag, "--", "."], capture_output=True)
    if r.returncode == 1:
        return []
    if r.returncode != 0:
        raise RuntimeError("git grep: %s" % r.stderr.decode(errors="replace").strip())
    return [l for l in r.stdout.decode("utf-8", errors="replace").strip().splitlines() if l]


def print_row(label, r):
    flags = []
    if r["dels"]:
        flags.append("-%d" % r["dels"])
    if r["nonascii"]:
        flags.append("nonascii %d" % r["nonascii"])
    if r["eolbad"]:
        flags.append("eol! %d" % r["eolbad"])
    a, b = r["enc"].split("->")
    if a != b and a != "-":
        flags.append("enc! %s" % r["enc"])
    if r["tok"]:
        flags.append("tok! %d" % r["tok"])
    print("  %-6s %-9s %-52s +%-4d %s%s" % (r["verdict"], label, r["path"], r["ins"], " ".join(flags),
                                          ("  -- " + r["note"]) if r["note"] else ""))


def classify_unflagged(path, old, new):
    """For a changed file that never mentions the flag: is the change inside SOME SERV_IRUHADEV_ flag?"""
    if new == old:
        return "SAME", ""
    others = all_iruhadev(old, new)
    try:
        a, b = strip(new or b"", others), strip(old or b"", others)
        if is_define_home(path, others):
            a, b = strip_defines(a, others), strip_defines(b, others)
            if a != b and define_file_only(b, a):
                return "OTHER", "%s: other flags' comments/defines" % os.path.basename(path)
    except Unevaluable as e:
        return "ERROR", str(e)
    if a == b or loose(a) == loose(b):
        changed = [o for o in sorted(others) if loose(strip(new or b"", others - {o})) != loose(strip(old or b"", others - {o}))]
        return "OTHER", "file does not mention the flag; change belongs to: %s" % ", ".join(changed or sorted(others))
    d = first_diff(b, a)
    note = "UNFLAGGED change in a file that does not mention the flag"
    if d:
        note += " (stripped line %d: old %r | new %r)" % (d[0], body(d[1])[:80], body(d[2])[:80])
    return "FAIL", note


def run(flag, pairs, extra=(), only=None):
    """pairs: list of (label, rev_old, rev_new(None=worktree), [paths] or None).
    A pair is in scope when any of its changed code files mentions the flag; then EVERY changed
    code file of that pair is checked (a forgotten #ifdef in a flag-free file is a FAIL)."""
    totals = {"PASS": 0, "PASS~": 0, "DEFINE": 0, "OTHER": 0, "FAIL": 0, "ERROR": 0, "SAME": 0}
    dels = nonascii = eolbad = tok = encbad = 0
    fb = flag.encode()
    for label, ro, rn, paths in pairs:
        files = paths if paths is not None else code_files_changed(ro, rn)
        if only:
            files = [f for f in files if f.replace("\\", "/") in only]
        blobs = [(path, show(ro, path), show(rn, path)) for path in files]
        if not any(fb in (o or b"") or fb in (n or b"") for _, o, n in blobs):
            continue
        rows = []
        for path, old, new in blobs:
            if fb not in (new or b"") and fb not in (old or b""):
                v, note = classify_unflagged(path, old, new)
                if v == "SAME":
                    continue
                r = {"path": path, "verdict": v, "note": note, "dels": 0, "ins": 0, "nonascii": 0, "eolbad": 0,
                     "enc": "%s->%s" % (enc_class(old), enc_class(new)), "tok": 0}
                rows.append(r)
                continue
            r = check_pair(path, old, new, flag, ro, rn, extra)
            if r["verdict"] == "SAME":
                continue
            rows.append(r)
        for r in rows:
            print_row(label, r)
            totals[r["verdict"]] += 1
            if r["verdict"] != "OTHER":
                dels += r["dels"]
                nonascii += r["nonascii"]
                eolbad += r["eolbad"]
                tok += r["tok"]
            a, b = r["enc"].split("->")
            if a != b and a != "-":
                encbad += 1
    sites = define_sites(flag)
    print()
    print("define: %s" % ("; ".join(sites) if sites else "NOT FOUND in tracked files"))
    homes = [DEFINE_HOME] + ([SECOND_HOMES[flag]] if flag in SECOND_HOMES else [])
    bad_sites = [s for s in sites if not any(s.startswith(h + ":") for h in homes)]
    if bad_sites:
        print("  WARNING: #define outside %s: %s" % (DEFINE_HOME, bad_sites))
    print("files: %d PASS, %d PASS~ (blank/marker only), %d DEFINE, %d OTHER (other flags), %d FAIL, %d ERROR"
          % (totals["PASS"], totals["PASS~"], totals["DEFINE"], totals["OTHER"], totals["FAIL"], totals["ERROR"]))
    print("in this flag's files: %d deleted line(s), %d non-ASCII inserted line(s), %d EOL-mismatched inserted line(s), "
          "%d #else/#endif without the flag token, %d encoding change(s)" % (dels, nonascii, eolbad, tok, encbad))
    ok = totals["FAIL"] == 0 and totals["ERROR"] == 0 and encbad == 0 and not bad_sites and bool(sites)
    if not sites:
        print("  FAIL: the flag is not #defined anywhere tracked (add it to %s)" % DEFINE_HOME)
    checked = sum(v for k, v in totals.items() if k != "SAME")
    if checked == 0:
        print("RESULT: NOTHING CHECKED (no changed file mentions %s in the given revisions)" % flag)
        return 1
    print("RESULT: %s" % ("PASS" if ok else "FAIL"))
    return 0 if ok else 1


def lifetime_commits(flag):
    first = git("log", "-S", flag, "--reverse", "--format=%H", "HEAD", "--",
                *[d.rstrip("/") for d in CODE_DIRS]).splitlines()
    if not first:
        return []
    return git("rev-list", "--reverse", "%s^..HEAD" % first[0]).splitlines()


# ------------------------------------------------------------------ selftest
def selftest():
    F = "SERV_IRUHADEV_T"
    ko = b"\xc7\xd1\xb1\xdb"  # CP949 bytes
    base = b"int a;\r\n// " + ko + b"\r\nvoid f()\r\n{\r\n\tg();\r\n}\r\n"
    cases = []
    # 1 insert-only ifdef block
    new = base.replace(b"\tg();\r\n", b"\tg();\r\n#ifdef SERV_IRUHADEV_T\r\n\th();\r\n#endif SERV_IRUHADEV_T\r\n")
    cases.append(("insert", base, new, "PASS"))
    # 2 replace with #else keeping original
    new = base.replace(b"\tg();\r\n", b"#ifdef SERV_IRUHADEV_T\r\n\th();\r\n#else SERV_IRUHADEV_T\r\n\tg();\r\n#endif SERV_IRUHADEV_T\r\n")
    cases.append(("else", base, new, "PASS"))
    # 3 original lost (no #else)
    new = base.replace(b"\tg();\r\n", b"#ifdef SERV_IRUHADEV_T\r\n\th();\r\n#endif SERV_IRUHADEV_T\r\n")
    cases.append(("lost-original", base, new, "FAIL"))
    # 4 ifndef keeps body
    new = base.replace(b"\tg();\r\n", b"#ifndef SERV_IRUHADEV_T\r\n\tg();\r\n#endif\r\n")
    cases.append(("ifndef", base, new, "PASS"))
    # 5 nested other #if inside dropped region
    new = base.replace(b"\tg();\r\n", b"\tg();\r\n#ifdef SERV_IRUHADEV_T\r\n#ifdef X\r\n\tx();\r\n#else\r\n\ty();\r\n#endif\r\n#endif SERV_IRUHADEV_T\r\n")
    cases.append(("nested", base, new, "PASS"))
    # 6 blank line outside the flag
    new = base.replace(b"\tg();\r\n", b"\tg();\r\n\r\n#ifdef SERV_IRUHADEV_T\r\n\th();\r\n#endif SERV_IRUHADEV_T\r\n")
    cases.append(("blank", base, new, "PASS~"))
    # 7 unflagged edit
    new = base.replace(b"\tg();", b"\tg2();")
    new = new.replace(b"int a;", b"int a;\r\n#ifdef SERV_IRUHADEV_T\r\n#endif")
    cases.append(("unflagged", base, new, "FAIL"))
    # 8 #if defined(F) && defined(OTHER) -> 0 regardless
    new = base.replace(b"\tg();\r\n", b"\tg();\r\n#if defined(SERV_IRUHADEV_T) && defined(_IN_HOUSE_)\r\n\th();\r\n#endif\r\n")
    cases.append(("and-unknown", base, new, "PASS"))
    # 9 #if defined(F) || OTHER -> unknown -> ERROR
    new = base.replace(b"\tg();\r\n", b"\tg();\r\n#if defined(SERV_IRUHADEV_T) || defined(_IN_HOUSE_)\r\n\th();\r\n#endif\r\n")
    cases.append(("or-unknown", base, new, "ERROR"))
    # 10 other flag's change only
    new = base.replace(b"\tg();\r\n", b"\tg();\r\n#ifdef SERV_IRUHADEV_T\r\n\th();\r\n#endif SERV_IRUHADEV_T\r\n")
    newer = new.replace(b"int a;\r\n", b"int a;\r\n#ifdef SERV_IRUHADEV_OTHER\r\nint b;\r\n#endif\r\n")
    cases.append(("other-flag", new, newer, "OTHER"))
    # 11 CP949 bytes inside the dropped block and in the #else are preserved exactly
    new = base.replace(b"// " + ko + b"\r\n", b"#ifdef SERV_IRUHADEV_T\r\n// new\r\n#else\r\n// " + ko + b"\r\n#endif\r\n")
    cases.append(("cp949", base, new, "PASS"))
    # 12b directives inside a block comment are not directives
    comment = b"/*#ifdef A\r\n#elif defined B\r\n#else\r\n#endif*/\r\n"
    old_c = base.replace(b"\tg();\r\n", b"\tg();\r\n" + comment)
    new = base.replace(b"\tg();\r\n", b"\tg();\r\n" + comment + b"#ifdef SERV_IRUHADEV_T\r\n\th();\r\n#endif\r\n")
    cases.append(("block-comment", old_c, new, "PASS"))
    # 12 identical file -> SAME (never a pass)
    cases.append(("vacuous", base, base, "SAME"))
    fails = 0
    # scope: a file that never mentions the flag
    for name, o, n, want in (
            ("unflagged-file", base, base.replace(b"\tg();", b"\tg(1);"), "FAIL"),
            ("other-flag-file", base,
             base.replace(b"\tg();\r\n", b"\tg();\r\n#ifdef SERV_IRUHADEV_X\r\n\th();\r\n#endif\r\n"), "OTHER")):
        got = classify_unflagged("X2Lib/u.cpp", o, n)[0]
        ok = got == want
        fails += not ok
        print("  %-4s %-14s want %-6s got %s" % ("ok" if ok else "BAD", name, want, got))
    for name, old, new, want in cases:
        got = classify("X2Lib/t.cpp", old, new, F)[0]
        ok = got == want
        fails += not ok
        print("  %-4s %-14s want %-6s got %s" % ("ok" if ok else "BAD", name, want, got))
    # define files: a later commit appends another flag's block to Always.h (OTHER, not FAIL); a #define in a
    # SECOND_HOMES file counts only for the flag that file is the second home of
    blk = lambda f: b"\r\n////////\r\n// Description: " + f + b"\r\n#define " + f + b"\r\n////////\r\n"
    ah = b"#define SERV_IRUHADEV_A\r\n" + blk(b"SERV_IRUHADEV_T")
    sd = b"#include \"x.h\"\r\n"
    wire = sorted(SECOND_HOMES)[0]
    for name, path, old, new, flag, want in (
            ("define-home", DEFINE_HOME, b"#define SERV_IRUHADEV_A\r\n", ah, F, "DEFINE"),
            ("peer-define", DEFINE_HOME, ah, ah + blk(b"SERV_IRUHADEV_PEER"), F, "OTHER"),
            ("not-2nd-home", SECOND_HOMES[wire], sd, sd + blk(b"SERV_IRUHADEV_T"), F, "FAIL"),
            ("second-home", SECOND_HOMES[wire], sd, sd + blk(wire.encode()), wire, "DEFINE")):
        got = classify(path, old, new, flag)[0]
        ok = got == want
        fails += not ok
        print("  %-4s %-14s want %-6s got %s" % ("ok" if ok else "BAD", name, want, got))
    # tok count
    st = {"tok": 0}
    strip(b"#ifdef SERV_IRUHADEV_T\r\nx\r\n#else\r\ny\r\n#endif\r\n", {F}, F, st)
    ok = st["tok"] == 2
    fails += not ok
    print("  %-4s %-14s want 2      got %d" % ("ok" if ok else "BAD", "token-count", st["tok"]))
    print("SELFTEST: %s" % ("PASS" if not fails else "%d FAILED" % fails))
    return 0 if not fails else 1


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("flag", nargs="?")
    ap.add_argument("files", nargs="*")
    ap.add_argument("--range")
    ap.add_argument("--commits", nargs="+")
    ap.add_argument("--files", nargs="+", dest="files_opt", metavar="FILE",
                    help="limit the check to these files (use this after --commits, which takes several values)")
    ap.add_argument("--worktree", action="store_true")
    ap.add_argument("--base")
    ap.add_argument("--at")
    ap.add_argument("--with", dest="with_", nargs="+", metavar="FLAG2",
                    help="flags whose blocks count as part of this change (e.g. the phase's _DEBUG flag)")
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args()
    if a.selftest:
        return selftest()
    if not a.flag:
        ap.error("FLAG is required")
    flag = a.flag if a.flag.startswith(PREFIX) else PREFIX + a.flag
    a.files = list(a.files or []) + list(a.files_opt or [])
    pairs = []
    if a.base:
        files = a.files or None
        if files is None:
            files = code_files_changed(a.base, a.at) if a.at else code_files_changed(a.base, None)
        pairs.append(("base", a.base, a.at, files))
    else:
        if a.range:
            commits = git("rev-list", "--reverse", a.range).splitlines()
        elif a.commits:
            commits = [git("rev-parse", c) for c in a.commits]
        elif a.worktree:
            commits = []
        else:
            commits = lifetime_commits(flag)
        for c in commits:
            parents = git("rev-list", "--parents", "-n", "1", c).split()
            if len(parents) < 2:
                continue
            pairs.append((c[:7], parents[1], c, None))
        if a.worktree or not (a.range or a.commits):
            if git("status", "--porcelain", "--", *[d.rstrip("/") for d in CODE_DIRS]):
                pairs.append(("worktree", "HEAD", None, None))
    print("flag %s: %d revision pair(s)" % (flag, len(pairs)))
    extra = [x if x.startswith(PREFIX) else PREFIX + x for x in (a.with_ or [])]
    if extra:
        print("treated as part of the same change: %s" % ", ".join(extra))
    only = set(x.replace("\\", "/") for x in a.files) if (a.files and not a.base) else None
    if only:
        print("only these files: %s" % ", ".join(sorted(only)))
    return run(flag, pairs, extra, only)


if __name__ == "__main__":
    sys.exit(main())
