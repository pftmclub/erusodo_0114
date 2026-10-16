"""UI framework census (measurement 4): does June 2017's DLG_* UI Lua use anything March 2014's KTDGUI parser lacks?

    python june2017_re/ui_dlg_census.py            # writes june2017_re/ui_dlg_census.tsv, prints a summary
    python june2017_re/ui_dlg_census.py --top 60   # longer gap list in the summary

What a dialog script is: KTDGUIDialog.cpp:2323 declares the global `g_pUIDialog` and runs the file; the script then calls
`g_pUIDialog:CreateStatic()` / `:CreateButton()` / `:CreatePicture()` ..., methods on what those return, and passes plain
Lua tables to SetPoint-style calls, which C++ reads key by key (KTDGUIControl.cpp SetPointData_/SetStringData_/
SetTexChangeData_/Move, KTDGUIDialog.cpp Move) with a default for every missing key. So an unknown METHOD is a nil-call
error (the script aborts), an unknown KEY is silently ignored (a feature quietly missing).

Three name kinds are compared (June source text -> March C++ bindings / March DLG bytecode):
  method   `obj:Name(`            March side: lua_tinker::class_def / class_mem in KTDXLIB + X2Lib (name; per-class when the
                                  receiver type can be inferred), March DLG: TGETS constant
  global   `Name(` / `Name.sub(`  March side: lua_tinker::def / decl / class_add, March DLG: GGET constant
  key      `{ NAME = ... }` argument tables of a call. March side: every LUA_GET_VALUE*/lua_tinker::get "KEY" in KTDGUI*.cpp,
                                  March DLG: TSETS / TDUP template key
Classes: bound (UI class binds it / C++ reads it) | bound-nonui (bound, but only on a non-UI class or as a global) |
         march-lua-uses (March DLG uses it, C++ does not bind it - Lua-defined) | lua-builtin | june-defined (a function the
         June file itself defines) | june-only-unbound (the gap list).
Binding guards (#if) are resolved against the built exe: a name whose only bindings are guarded is `bound` only when its string
is present in the deployed exe (in_exe column).

March DLG baseline = the pre-June koms (kom_contents/data001/data034/datai014/datai022/data064 .kom, extracted with elstools),
because kom_contents/dataNNN_kom now holds ljconv'd June scripts that would hide every gap. Bytecode is read with tools/ljbc.py
(exact constants, no regex over bytes).
"""
import argparse, collections, glob, os, re, subprocess, sys, tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.abspath(os.path.join(HERE, ".."))
sys.path.insert(0, os.path.join(TRUNK, "tools"))
import ljbc  # noqa: E402

MARCH = r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799"
KOM_CONTENTS = os.path.join(MARCH, "kom_contents")
EXE = os.path.join(MARCH, "data", "X2_offline.exe")
JUNE = os.path.join(TRUNK, "june_src_fixed")
ELSTOOLS = os.path.join(TRUNK, "elstools", "elstools.exe")
ORIG_KOMS = ["data001", "data034", "data035", "data036", "data037", "data040", "data064", "datai014", "datai022"]
TSV = os.path.join(HERE, "ui_dlg_census.tsv")

LUA_BUILTINS = set("""print pairs ipairs next type tostring tonumber setmetatable getmetatable rawget rawset rawequal select
unpack pcall xpcall error assert require dofile loadfile loadstring load collectgarbage module string table math os io
coroutine debug bit ffi jit""".split())
KEYWORDS = set("and break do else elseif end false for function goto if in local nil not or repeat return then true until while".split())


# ----------------------------------------------------------------------------------------------
# March C++ side
def read_cp949(p):
    return open(p, "rb").read().decode("cp949", "replace")


def strip_cpp_comments(t):
    t = re.sub(r"/\*.*?\*/", lambda m: "\n" * m.group(0).count("\n"), t, flags=re.S)
    return re.sub(r"//[^\n]*", "", t)


def cpp_files():
    out = []
    for sub in ("KTDXLIB", "X2Lib"):
        for ext in ("cpp", "h"):
            out += glob.glob(os.path.join(TRUNK, sub, "*." + ext))
            if sub == "KTDXLIB":
                out += glob.glob(os.path.join(TRUNK, sub, "OnlyGlobal", "*." + ext))
    return sorted(set(out))


UI_CLASS_RE = re.compile(r"^(CKTDGUI\w+|CPictureData|CMovieData|UIStringData|UIPointData)$")


def short_cls(c):
    return c.split("::")[-1]


def scan_bindings(defined_macros):
    """-> binds: list of dict(kind,cls,name,target,file,line,guard)."""
    pats = [
        ("class_add", re.compile(r'class_add\s*<\s*([\w:]+)\s*>\s*\(\s*[^,]+,\s*"(\w+)"')),
        ("method", re.compile(r'class_def\s*<\s*([\w:]+)\s*>\s*\(\s*[^,]+,\s*"(\w+)"\s*,\s*&?\s*([\w:]+)?')),
        ("member", re.compile(r'class_mem\s*<\s*([\w:]+)\s*>\s*\(\s*[^,]+,\s*"(\w+)"')),
        ("global", re.compile(r'lua_tinker::def\s*\(\s*[^,]+,\s*"(\w+)"')),
        ("global", re.compile(r'lua_tinker::decl\s*\(\s*[^,]+,\s*"(\w+)"')),
    ]
    inh_re = re.compile(r"class_inh\s*<\s*([\w:]+)\s*,\s*([\w:]+)\s*>")
    binds, inh = [], {}
    for f in cpp_files():
        txt = read_cp949(f)
        if "lua_tinker" not in txt:
            continue
        stack = []                     # (active, text)
        for ln, raw in enumerate(strip_cpp_comments(txt).split("\n"), 1):
            s = raw.strip()
            m = re.match(r"#\s*(ifdef|ifndef|if|elif|else|endif)\b\s*(.*)", s)
            if m:
                k, rest = m.group(1), m.group(2)
                if k in ("ifdef", "ifndef", "if"):
                    stack.append((k, rest.strip()))
                elif k == "elif":
                    if stack:
                        stack[-1] = ("if", "(elif) " + rest.strip())
                elif k == "else":
                    if stack:
                        stack[-1] = ("else", stack[-1][0] + " " + stack[-1][1])
                elif k == "endif" and stack:
                    stack.pop()
                continue
            if "lua_tinker" not in s and "class_" not in s:
                continue
            guard = " && ".join("%s %s" % (a, b) for a, b in stack)
            mi = inh_re.search(s)
            if mi:
                inh[short_cls(mi.group(1))] = short_cls(mi.group(2))
            for kind, rx in pats:
                for m in rx.finditer(s):
                    g = m.groups()
                    if kind == "global":
                        binds.append(dict(kind="global", cls="", name=g[0], target="", file=f, line=ln, guard=guard))
                    elif kind == "class_add":
                        binds.append(dict(kind="class", cls=short_cls(g[0]), name=g[1], target="", file=f, line=ln, guard=guard))
                    elif kind == "method":
                        binds.append(dict(kind="method", cls=short_cls(g[0]), name=g[1], target=(g[2] or ""), file=f, line=ln, guard=guard))
                    else:
                        binds.append(dict(kind="member", cls=short_cls(g[0]), name=g[1], target="", file=f, line=ln, guard=guard))
    return binds, inh


def guard_active(stack, defined):
    """Simple #ifdef/#ifndef guards against the macros #define'd somewhere in the headers (an upper bound).
    Other forms are assumed active."""
    for k, rest in stack:
        m = re.match(r"(\w+)", rest)
        if not m:
            continue
        if k == "ifdef" and m.group(1) not in defined:
            return False
        if k == "ifndef" and m.group(1) in defined:
            return False
    return True


KEY_GUARD = {}      # key -> list of (active, guard text), filled by scan_ui_key_reads


def scan_ui_key_reads(defined):
    """Table keys the KTDGUI parsers read: key -> list of 'file:line fn' (also fills KEY_GUARD)."""
    keys = collections.defaultdict(list)
    rx = re.compile(r'(?:LUA_GET_VALUE\w*|LUA_GET_USER_DEFINED_TYPE_VALUE)\s*\(\s*\w+\s*,\s*"(\w+)"|lua_tinker::get\s*<[^>]*>\s*\(\s*[^,]+,\s*"(\w+)"')
    for f in sorted(glob.glob(os.path.join(TRUNK, "KTDXLIB", "KTDGUI*.cpp"))) + [os.path.join(TRUNK, "X2Lib", "X2SlotManager.cpp"), os.path.join(TRUNK, "X2Lib", "X2SlotItem.cpp")]:
        fn = "?"
        stack = []
        for ln, raw in enumerate(strip_cpp_comments(read_cp949(f)).split("\n"), 1):
            mm = re.match(r"#\s*(ifdef|ifndef|if|elif|else|endif)\b\s*(.*)", raw.strip())
            if mm:
                k_, rest_ = mm.group(1), mm.group(2).strip()
                if k_ in ("ifdef", "ifndef", "if"):
                    stack.append((k_, rest_))
                elif k_ in ("elif", "else") and stack:
                    stack[-1] = ("if", "(else-branch) " + rest_)
                elif k_ == "endif" and stack:
                    stack.pop()
                continue
            m = re.match(r"^[\w:<>\*&\s]+?(\w+::~?\w+)\s*\(", raw)
            if m and not raw.startswith((" ", "\t")):
                fn = m.group(1)
            for m in rx.finditer(raw):
                k = m.group(1) or m.group(2)
                keys[k].append("%s:%d %s" % (os.path.basename(f), ln, fn))
                KEY_GUARD.setdefault(k, []).append((guard_active(stack, defined), " && ".join("%s %s" % s for s in stack)))
    return keys


def ui_return_types():
    """Func name -> UI class it returns a pointer to (from the KTDGUI*.h declarations)."""
    ret = {}
    rx = re.compile(r"([\w:]+)\s*\*\s*(\w+)\s*\(")
    for f in glob.glob(os.path.join(TRUNK, "KTDXLIB", "KTDGUI*.h")):
        for m in rx.finditer(strip_cpp_comments(read_cp949(f))):
            c = short_cls(m.group(1))
            if UI_CLASS_RE.match(c):
                ret.setdefault(m.group(2), c)
    return ret


def defined_in_headers():
    """Every macro #define'd (uncommented) anywhere in KTDXLIB/X2Lib/KncWX2Server/Common headers: an upper bound only."""
    s = set()
    rx = re.compile(r"^\s*#\s*define\s+(\w+)", re.M)
    for sub in (("KTDXLIB",), ("X2Lib",), ("KncWX2Server", "Common")):
        for f in glob.glob(os.path.join(TRUNK, *sub, "**", "*.h"), recursive=True):
            s.update(rx.findall(strip_cpp_comments(read_cp949(f))))
    return s


# ----------------------------------------------------------------------------------------------
# June side (text)
def strip_lua(t):
    """Blank comments and string contents (keep quotes and newlines)."""
    out, i, n = [], 0, len(t)
    while i < n:
        c = t[i]
        if t.startswith("--", i):
            m = re.match(r"--\[(=*)\[", t[i:])
            if m:
                end = t.find("]" + m.group(1) + "]", i)
                end = n if end < 0 else end + len(m.group(1)) + 2
                out.append("\n" * t[i:end].count("\n"))
                i = end
            else:
                j = t.find("\n", i)
                i = n if j < 0 else j
        elif c in "\"'":
            j = i + 1
            while j < n and t[j] != c and t[j] != "\n":
                j += 2 if t[j] == "\\" else 1
            out.append(c + c)
            i = j + 1
        elif c == "[" and re.match(r"\[(=*)\[", t[i:]):
            m = re.match(r"\[(=*)\[", t[i:])
            end = t.find("]" + m.group(1) + "]", i)
            end = n if end < 0 else end + len(m.group(1)) + 2
            out.append('""' + "\n" * t[i:end].count("\n"))
            i = end
        else:
            out.append(c)
            i += 1
    return "".join(out)


CALL_TABLE_RE = re.compile(r"(?:(\w+)\s*:\s*)?(\w+)\s*\(\s*\{")
KEY_RE = re.compile(r"(?:^|[{,;])\s*([A-Za-z_]\w*)\s*=(?!=)")


def table_keys_of_call(s, open_idx):
    """Depth-1 keys of the `{` at open_idx (strings/comments already blanked) and the index after its `}`."""
    depth, i, n = 0, open_idx, len(s)
    top = []
    start = open_idx
    while i < n:
        c = s[i]
        if c == "{":
            if depth == 1:
                top.append(s[start:i].replace("\n", " "))
                start = i
            depth += 1
            if depth == 1:
                start = i + 1
        elif c == "}":
            depth -= 1
            if depth == 1:
                start = i + 1
            if depth == 0:
                top.append(s[start:i].replace("\n", " "))
                return top, i + 1
        i += 1
    return top, n


def defset_names(s):
    out = set(re.findall(r"\blocal\s+function\s+(\w+)", s))
    out |= set(re.findall(r"\bfunction\s+(\w+)", s))
    for m in re.finditer(r"\blocal\s+([\w\s,]+?)(?:=|\n|$)", s):
        out |= set(re.findall(r"\w+", m.group(1)))
    for m in re.finditer(r"\bfor\s+([\w\s,]+?)(?:=|\bin\b)", s):
        out |= set(re.findall(r"\w+", m.group(1)))
    for m in re.finditer(r"\bfunction\s*[\w.:]*\s*\(([^)]*)\)", s):
        out |= set(re.findall(r"\w+", m.group(1)))
    return out


def june_extract(text, ret_types):
    """-> dict of lists: methods[(recv_type|None, name)], globals[name], keys[(ctx, key)], fields[(type,name)], defs set."""
    s = strip_lua(text)
    r = dict(methods=[], globals=[], keys=[], fields=[], defs=set(), consts=[], recvs=[])
    r["defs"] = set(re.findall(r"function\s+([A-Za-z_][\w.:]*)", s))
    vtype = {"g_pUIDialog": "CKTDGUIDialog", "g_pDialogManager": "CKTDGUIDialogManager"}
    for line in s.split("\n"):
        m = re.match(r"\s*(?:local\s+)?(\w+)\s*=\s*(\w+)\s*:\s*(\w+)\s*\(", line)
        if m:
            var, recv, meth = m.groups()
            vtype[var] = ret_types.get(meth) if (meth in ret_types) else None
            vtype[var] = vtype[var] or ("?" + meth)          # remember an unknown-returning method as unresolved
        else:
            m = re.match(r"\s*(?:local\s+)?(\w+)\s*=\s*(\w+)\s*\(", line)
            if m:
                vtype[m.group(1)] = None
        for m in re.finditer(r"(?<![\w.])(\w+)\s*:\s*(\w+)\s*\(", line):
            recv, meth = m.groups()
            t = vtype.get(recv)
            r["methods"].append((t if t and not t.startswith("?") else None, meth))
            r["recvs"].append((recv, meth))
        for m in re.finditer(r"(?<![\w.:])(\w+)\s*\(", line):
            nm = m.group(1)
            if nm in KEYWORDS or re.search(r"function\s+$", line[:m.start()]):
                continue                      # a `function name(` definition is not a call
            r["globals"].append(nm)
        for m in re.finditer(r"(?<![\w.:])([A-Za-z_]\w*)\.([A-Za-z_]\w*)\s*\(", line):
            r["globals"].append(m.group(1) + "." + m.group(2))
        m = re.match(r"\s*(\w+)\.(\w+)\s*=(?!=)", line)
        if m and vtype.get(m.group(1)) and not vtype[m.group(1)].startswith("?"):
            r["fields"].append((vtype[m.group(1)], m.group(2)))
    # external global VALUE reads (enum tables, constants): identifiers never assigned/declared in this file, not called
    local = set(re.findall(r"(?<![\w.])([A-Za-z_]\w*)\s*=(?!=)", s)) | defset_names(s)
    for m in re.finditer(r"(?<![\w.:\"'])([A-Za-z_]\w*)(?:\s*\.\s*([A-Za-z_]\w*))?", s):
        a_, b_ = m.group(1), m.group(2)
        if a_ in KEYWORDS or a_ in local or a_ in LUA_BUILTINS or re.match(r"^(uv|slot)\d+$", a_):
            continue
        rest = s[m.end():m.end() + 3].lstrip()
        if rest.startswith("(") or rest.startswith(":"):
            continue
        r["consts"].append(a_ + ("." + b_ if b_ else ""))
    for m in CALL_TABLE_RE.finditer(s):
        ctx = m.group(2)
        top, _ = table_keys_of_call(s, m.end() - 1)
        for chunk in top:
            for k in KEY_RE.findall(chunk):
                r["keys"].append((ctx, k))
    return r


# ----------------------------------------------------------------------------------------------
# March side (bytecode)
def march_extract(data):
    """-> dict(tgets, gget, gset, keys, kgc) sets from a LuaJIT 0x81 dump."""
    dump = ljbc.parse(data)
    out = dict(tgets=set(), gget=set(), gset=set(), keys=set(), kgc=set())
    names = ljbc.NAMES

    def kstr(pt, idx):
        e = ljbc.kgc_at(pt, idx)
        return e[1].decode("latin-1") if e and e[0] == "str" else None

    for pt in dump.protos:
        for e in pt.kgc:
            if e[0] == "str":
                out["kgc"].add(e[1].decode("latin-1"))
            elif e[0] == "tab":
                for k, _v in e[3]:
                    if k[0] == "s":
                        out["keys"].add(k[1].decode("latin-1"))
        for ins in pt.bc:
            op, a, b, c, d = ljbc.fields(ins)
            nm = names[op] if op < len(names) else ""
            if nm == "TGETS":
                v = kstr(pt, c)
                if v:
                    out["tgets"].add(v)
            elif nm == "TSETS":
                v = kstr(pt, c)
                if v:
                    out["keys"].add(v)
            elif nm == "GGET":
                v = kstr(pt, d)
                if v:
                    out["gget"].add(v)
            elif nm == "GSET":
                v = kstr(pt, d)
                if v:
                    out["gset"].add(v)
    return out


def ensure_orig_koms(workdir):
    for k in ORIG_KOMS:
        dest = os.path.join(workdir, k + "_kom")
        if os.path.isdir(dest) and os.listdir(dest):
            continue
        src = os.path.join(KOM_CONTENTS, k + ".kom")
        if not os.path.isfile(src):
            print("  missing original kom", src)
            continue
        subprocess.run([ELSTOOLS, "-q", "extract", "-i", src, "-o", workdir, "-v", "mar-2014"], stdin=subprocess.DEVNULL,
                       stdout=subprocess.DEVNULL, check=True)


def march_files(workdir):
    """-> list of (label, path). Pristine extracted koms plus kom_contents_bytecode snapshots (originals of converted files)."""
    out, seen = [], set()
    for root in (workdir, os.path.join(MARCH, "kom_contents_bytecode")):
        for p in sorted(glob.glob(os.path.join(root, "data*_kom", "*.lua"))):
            if p.endswith(".src.lua"):
                continue
            b = os.path.basename(p)
            if b in seen:
                continue
            seen.add(b)
            out.append((os.path.basename(os.path.dirname(p)) + "/" + b, p))
    return out


# ----------------------------------------------------------------------------------------------
def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--top", type=int, default=40)
    ap.add_argument("--workdir", default=os.path.join(tempfile.gettempdir(), "ui_dlg_census_march_orig"))
    a = ap.parse_args()

    # ---- March C++ surface
    binds, inh = scan_bindings(set())
    key_reads = scan_ui_key_reads(defined_in_headers())
    ret_by_func = ui_return_types()
    ret_types = {}                      # Lua method name -> UI class it returns (through the bind's C++ target, e.g. GetStatic -> GetStatic_LUA)
    for b in binds:
        if b["kind"] == "method" and UI_CLASS_RE.match(b["cls"]):
            fn = b["target"].split("::")[-1]
            if fn in ret_by_func:
                ret_types.setdefault(b["name"], ret_by_func[fn])
    exe = open(EXE, "rb").read() if os.path.isfile(EXE) else b""
    ui_classes = {b["cls"] for b in binds if b["kind"] == "class" and UI_CLASS_RE.match(b["cls"])} | {"UIStringData"}

    def ancestors(c):
        out = [c]
        while c in inh:
            c = inh[c]
            out.append(c)
        return out

    methods_by_cls = collections.defaultdict(dict)       # cls -> name -> [binds]
    for b in binds:
        if b["kind"] in ("method", "member"):
            methods_by_cls[b["cls"]].setdefault(b["name"], []).append(b)
    ui_method_names = collections.defaultdict(list)
    nonui_method_names = collections.defaultdict(list)
    global_names = collections.defaultdict(list)
    for b in binds:
        if b["kind"] in ("method", "member"):
            (ui_method_names if b["cls"] in ui_classes else nonui_method_names)[b["name"]].append(b)
        else:
            global_names[b["name"]].append(b)

    def in_exe(name):
        return bool(exe) and (name.encode() + b"\0") in exe

    def rel(b):
        return "%s:%d" % (os.path.relpath(b["file"], TRUNK).replace("\\", "/"), b["line"])

    def verdict(blist, name):
        """bound / bound-guarded(in exe) / 'unbound-guarded' for a list of bindings."""
        if any(not b["guard"] for b in blist):
            return "bound"
        return "bound" if in_exe(name) else "guarded-not-in-exe"

    n_ui_methods = len(ui_method_names)
    print("March C++: %d UI classes %s; %d UI method/member names; %d non-UI-class names; %d global/class names; %d table keys read in KTDGUI*.cpp + X2SlotManager/X2SlotItem.cpp"
          % (len(ui_classes), sorted(ui_classes), n_ui_methods, len(nonui_method_names), len(global_names), len(key_reads)))

    # ---- June
    jfiles = {}
    for p in sorted(glob.glob(os.path.join(JUNE, "data*_kom", "*.src.lua"))):
        t = open(p, encoding="utf-8", errors="replace").read()
        b = os.path.basename(p)
        if b.startswith("DLG_") or "g_pUIDialog" in t:
            jfiles[os.path.relpath(p, JUNE).replace("\\", "/")] = t
    june = {f: june_extract(t, ret_types) for f, t in jfiles.items()}
    n_dlg = sum(1 for f in june if os.path.basename(f).startswith("DLG_"))
    print("June: %d UI scripts (%d DLG_*, %d other referencing g_pUIDialog)" % (len(june), n_dlg, len(june) - n_dlg))

    # ---- March DLG bytecode (pristine)
    os.makedirs(a.workdir, exist_ok=True)
    ensure_orig_koms(a.workdir)
    mfiles = march_files(a.workdir)
    march = {}
    all_gset, all_keys = set(), set()      # over every pristine March Lua (not just DLGs): global assignments, table field keys
    bad = 0
    for label, p in mfiles:
        d = open(p, "rb").read()
        if d[:4] != ljbc.MAR_MAGIC:
            bad += 1
            continue
        try:
            m = march_extract(d)
        except Exception:
            bad += 1
            continue
        all_gset.update(m["gset"])
        all_keys.update(m["keys"])
        if os.path.basename(p).startswith("DLG_") or "g_pUIDialog" in m["gget"]:
            march[label] = m
    mu = lambda k: set().union(*[m[k] for m in march.values()]) if march else set()
    m_tgets, m_gget, m_keys, m_kgc, m_gset = mu("tgets"), mu("gget"), mu("keys"), mu("kgc"), mu("gset")
    print("March baseline: %d UI bytecode scripts (%d non-0x81/unparsed skipped); %d TGETS, %d GGET, %d keys"
          % (len(march), bad, len(m_tgets), len(m_gget), len(m_keys)))

    # ---- aggregate June usage
    use = collections.defaultdict(lambda: collections.defaultdict(set))   # (kind,name) -> file set
    pair_use = collections.defaultdict(set)                               # (type, name) -> files
    ctx_use = collections.defaultdict(set)                                # (ctx,key) -> files
    june_defs = collections.defaultdict(set)
    for f, r in june.items():
        for t, nme in r["methods"]:
            use[("method", nme)][f] = None
            if t:
                pair_use[(t, nme)].add(f)
        for nme in r["globals"]:
            use[("global", nme)][f] = None
        for ctx, k in r["keys"]:
            use[("key", k)][f] = None
            ctx_use[(ctx, k)].add(f)
        for t, nme in r["fields"]:
            use[("field", nme)][f] = None
        for nme in set(r["consts"]):
            use[("const", nme)][f] = None
        for d in r["defs"]:
            june_defs[d].add(f)

    # global names June UI scripts call that nothing above explains: look for a definition anywhere in June's source (any kom)
    pending = {n for (k, n) in use if k == "global" and n.split(".")[0] not in LUA_BUILTINS
               and n not in global_names and n.split(".")[0] not in global_names and n not in all_gset
               and n.split(".")[0] not in all_gset and n not in june_defs and n.split(".")[0] not in june_defs}
    june_anywhere = collections.defaultdict(set)
    if pending:
        drx = re.compile(r"function\s+([A-Za-z_][\w.:]*)|^\s*([A-Za-z_]\w*)\s*=\s*function", re.M)
        for p in glob.glob(os.path.join(JUNE, "data*_kom", "*.src.lua")):
            tx = open(p, encoding="utf-8", errors="replace").read()
            for m in drx.finditer(tx):
                nm = m.group(1) or m.group(2)
                if nm in pending or nm.split(".")[0] in pending:
                    june_anywhere[nm].add(os.path.relpath(p, JUNE).replace("\\", "/"))

    rows = []
    cstats = collections.Counter()
    for (kind, name), files in sorted(use.items()):
        files = sorted(files)
        cls, note, where = "", "", ""
        base = name.split(".")[0]
        if kind == "method":
            if name in ui_method_names:
                cls = verdict(ui_method_names[name], name)
                where = rel(ui_method_names[name][0])
                if cls == "guarded-not-in-exe":
                    cls = "june-only-unbound"
                    note = "UI binding is #if-guarded and its string is not in the exe"
            elif name in nonui_method_names:
                cls = "bound-nonui"
                where = rel(nonui_method_names[name][0])
            elif name in global_names:
                cls = "bound-nonui"
                where = rel(global_names[name][0])
            elif name in m_tgets:
                cls = "march-lua-uses"
            else:
                cls = "june-only-unbound"
        elif kind == "global":
            if base in LUA_BUILTINS:
                cls = "lua-builtin"
            elif name in global_names or base in global_names:
                bl = global_names.get(name) or global_names[base]
                cls = verdict(bl, base)
                where = rel(bl[0])
                if cls == "guarded-not-in-exe":
                    cls = "june-only-unbound"
                    note = "binding guarded and not in exe"
            elif (name in june_defs or base in june_defs) and set(files) <= (june_defs.get(name) or june_defs[base]):
                cls = "june-defined-same-file"
            elif name in all_gset or base in all_gset:
                cls = "march-lua-defines"
            elif name in june_defs or base in june_defs:
                cls = "june-cross-file-helper"
                note = "defined in " + ", ".join(sorted(june_defs.get(name) or june_defs[base])[:3])
            elif base in m_gget or name in m_gget:
                cls = "march-lua-uses"
            elif (name in june_anywhere) or (base in june_anywhere):
                cls = "june-cross-file-helper"
                note = "defined in non-UI June Lua: " + ", ".join(sorted(june_anywhere.get(name) or june_anywhere[base])[:3])
            else:
                cls = "june-only-unbound"
        elif kind == "key":
            ctxs = collections.Counter({c: len(fs) for (c, k), fs in ctx_use.items() if k == name})
            ui_ctx = [c for c in ctxs if c in ui_method_names or c in nonui_method_names or c in ("AddString", "SetPoint", "Move")]
            if name in key_reads and not any(act for act, _g in KEY_GUARD[name]):
                cls = "june-only-unbound"
                where = key_reads[name][0]
                note = "reader exists but every read sits in an inactive guard: " + KEY_GUARD[name][0][1]
            elif name in key_reads:
                cls = "bound"
                where = key_reads[name][0]
            elif name in m_keys:
                cls = "march-lua-uses"
            elif not ui_ctx:
                cls = "key-in-non-ui-call"
            else:
                cls = "june-only-unbound"
            note = (note + "; " if note else "") + "ctx: " + ", ".join("%s(%d)" % kv for kv in ctxs.most_common(4))
        elif kind == "const":
            a_, _, b_ = name.partition(".")
            if a_ in global_names:
                cls, where = "bound", rel(global_names[a_][0])
            elif a_ in all_gset:
                if not b_ or b_ in all_keys:
                    cls = "march-lua-defines"
                else:
                    cls, note = "const-field-absent", "table %s exists in March Lua, field %s does not" % (a_, b_)
            elif a_ in m_gget:
                cls = "march-lua-uses"
            else:
                cls = "const-table-absent"
        elif kind == "field":
            cls = "bound" if (name in ui_method_names or name in key_reads) else ("march-lua-uses" if name in m_keys else "june-only-unbound")
        loose = "yes" if name in m_kgc or base in m_kgc else ""
        rows.append(dict(kind=kind, name=name, cls=cls, nfiles=len(files), files=files, where=where, note=note, loose=loose))
        cstats[(kind, cls)] += 1

    # ---- type-aware method check
    pair_rows = []
    for (t, nme), files in sorted(pair_use.items()):
        if t not in ui_classes and t not in inh:
            continue
        ok = False
        for c in ancestors(t):
            if nme in methods_by_cls.get(c, {}):
                bl = methods_by_cls[c][nme]
                ok = verdict(bl, nme) == "bound"
                break
        if not ok:
            pair_rows.append((t, nme, len(files), sorted(files)))
    already = {r["name"] for r in rows if r["kind"] == "method" and r["cls"] == "june-only-unbound"}
    for t, nme, n, fs in pair_rows:
        if nme not in already:          # bound by name somewhere else, but not on this receiver's class or its bases
            rows.append(dict(kind="method@" + t, name=nme, cls="june-only-unbound", nfiles=n, files=fs, where="",
                             note="nil-method call on a %s receiver (name is bound on another class)" % t, loose=""))
    n_calls = sum(len(r["methods"]) for r in june.values())
    n_typed = sum(1 for r in june.values() for t, _m in r["methods"] if t)
    # who calls the bound-nonui names: receiver variable names
    recv_of = collections.defaultdict(collections.Counter)
    for f, r in june.items():
        for rv, mth in r["recvs"]:
            recv_of[mth][rv] += 1

    # ---- guesses (from the name only)
    def guess(kind, name):
        n = name.lower()
        words = re.findall(r"[A-Z][a-z]+|[a-z]+|[A-Z]+(?![a-z])|\d+", name)
        if kind == "key":
            return "guess: option key '%s' of a control/point table" % name
        return "guess: %s '%s'" % (kind, " ".join(w.lower() for w in words))

    with open(TSV, "w", encoding="utf-8", newline="\n") as fh:
        fh.write("kind\tname\tclass\tjune_files\ttop5_files\tmarch_binding_or_reader\tnote\tmarch_const_anywhere\n")
        for r in sorted(rows, key=lambda r: (r["cls"] != "june-only-unbound", r["kind"], -r["nfiles"], r["name"])):
            top5 = ", ".join(os.path.basename(f).replace(".src.lua", "") for f in r["files"][:5])
            fh.write("\t".join([r["kind"], r["name"], r["cls"], str(r["nfiles"]), top5, r["where"],
                                r["note"] or (guess(r["kind"], r["name"]) if r["cls"] == "june-only-unbound" else ""), r["loose"]]) + "\n")
        fh.write("\n# type-aware: method called on a receiver whose UI class is known, not bound on it or its bases\n")
        fh.write("# class\tmethod\tjune_files\ttop5_files\n")
        for t, nme, n, fs in sorted(pair_rows, key=lambda x: -x[2]):
            fh.write("#\t%s\t%s\t%d\t%s\n" % (t, nme, n, ", ".join(os.path.basename(f).replace(".src.lua", "") for f in fs[:5])))

    # ---- summary
    print()
    for kind in ("method", "global", "key", "field", "const"):
        print("%-7s " % kind + "  ".join("%s=%d" % (c, n) for (k, c), n in sorted(cstats.items()) if k == kind))
    print("\nCONSTANTS = Lua enum/ID values (data in Enum.lua / string-ID Lua, NOT a KTDGUI framework gap). Compared with the PRISTINE March Lua,")
    print("so this overstates the gap: Enum.lua is a merged core-list file in the live tree.")
    for cl, what in (("const-table-absent", "the whole table is nil in March -> `T.FIELD` is a nil-index script error"),
                     ("const-field-absent", "table exists, field missing -> nil value, script carries on")):
        cm = [r for r in rows if r["cls"] == cl]
        bytab = collections.Counter()
        for r in cm:
            bytab[r["name"].split(".")[0]] += 1
        fl = set(f for r in cm for f in r["files"])
        print("  %s: %d names in %d scripts (%s); by table: %s" % (cl, len(cm), len(fl), what,
              ", ".join("%s=%d" % kv for kv in bytab.most_common(8))))
    sid = collections.Counter(r["cls"] for r in rows if r["kind"] == "const" and re.match(r"^STR_ID_\d+$", r["name"]))
    print("  STR_ID_<n> by class (sanity: some must be march-lua-defines, else March defines them somewhere unscanned): %s" % dict(sid))
    xf = sorted([r for r in rows if r["cls"] == "june-cross-file-helper"], key=lambda r: -r["nfiles"])
    print("\nLUA HELPERS June DLGs call that are defined in ANOTHER June file and not in pristine March Lua: %d" % len(xf))
    for r in xf[:10]:
        print("  %-28s %4d callers  %s" % (r["name"], r["nfiles"], r["note"]))
    print("  type-aware coverage: %d of %d `obj:Method(` calls had a receiver whose UI class was inferred (rest untyped: chained calls,"
          " function params, decompiler slotN/uvN)" % (n_typed, n_calls))
    gu = sorted(n for n, bl in ui_method_names.items() if all(b["guard"] for b in bl) and ("method", n) in
                {(r["kind"], r["name"]) for r in rows})
    print("  June-used UI methods whose every UI binding is #if-guarded (verified by exe string only): %s" % (", ".join(
          "%s[%s]%s" % (n, ui_method_names[n][0]["guard"], "+nonui-too" if n in nonui_method_names else "") for n in gu) or "none"))
    gaps = sorted([r for r in rows if r["cls"] == "june-only-unbound"], key=lambda r: (-r["nfiles"], r["name"]))
    print("\nGAP LIST june-only-unbound: %d names (STR_ID_<n> families collapsed below)" % len(gaps))
    fam = collections.defaultdict(list)
    shown = []
    for r in gaps:
        m = re.match(r"^(STR_ID_|[A-Z]+_STR_ID_)\d+$", r["name"])
        if m:
            fam[m.group(1)].append(r)
        else:
            shown.append(r)
    for r in shown[:a.top]:
        print("  %-6s %-40s %4d  %s" % (r["kind"], r["name"], r["nfiles"],
                                       ", ".join(os.path.basename(f).replace(".src.lua", "") for f in r["files"][:5])))
    for k, rs in fam.items():
        fs = set(f for r in rs for f in r["files"])
        print("  const  %-40s %4d files, %d distinct ids (family)" % (k + "<n>", len(fs), len(rs)))
    for kd, what in (("method", "nil-method call = script error at load"), ("key", "silently ignored key")):
        fs = set(f for r in gaps if r["kind"].split("@")[0] == kd for f in r["files"])
        print("  %s gaps touch %d of %d June UI scripts (%s)" % (kd, len(fs), len(june), what))
    print("  bound-nonui (June calls them; bound only on a non-UI class/global) with their receiver variables:")
    for r in sorted((r for r in rows if r["cls"] == "bound-nonui"), key=lambda r: r["name"]):
        print("    %-34s files=%-3d recv: %s" % (r["name"], r["nfiles"], ", ".join("%s(%d)" % kv for kv in recv_of[r["name"]].most_common(3))))
    print("\nCALIBRATION")
    cls_of = {(r["kind"], r["name"]): r["cls"] for r in rows}
    for kn in (("method", "SetNormalTex"), ("method", "CreateStatic"), ("method", "AddPicture"), ("key", "ADD_SIZE_X"),
               ("key", "LEFT_TOP"), ("global", "D3DXVECTOR2"), ("method", "SetDisableUnderWindow"), ("key", "LINE_BREAK_LENGTH")):
        print("  %-8s %-28s -> %s" % (kn[0], kn[1], cls_of.get(kn, "(not used by June)")))
    bound_any = set(ui_method_names) | set(nonui_method_names) | set(global_names)
    mm = sorted(n for n in m_tgets if re.match(r"^(Set|Add|Create|Get|Move|Change|Request)[A-Z]", n) and n not in bound_any)
    mk = sorted(k for k in m_keys if k not in key_reads)
    print("  March's own DLGs: %d Set/Add/Create/Get/Move-looking TGETS names not bound in C++ (expect ~0): %s"
          % (len(mm), ", ".join(mm[:12])))
    print("  March's own DLGs: %d of %d table keys are not read by KTDGUI*.cpp (data-table keys, expected): e.g. %s"
          % (len(mk), len(m_keys), ", ".join(mk[:10])))
    print("\nbound-nonui methods (named on a non-UI class only) that June calls on a UI receiver / any: %d" % sum(1 for r in rows if r["cls"] == "bound-nonui"))
    print("type-aware misses: %d (class.method pairs)" % len(pair_rows))
    for t, nme, n, fs in sorted(pair_rows, key=lambda x: -x[2])[:20]:
        print("  %-18s %-30s %4d  %s" % (t, nme, n, ", ".join(os.path.basename(f).replace(".src.lua", "") for f in fs[:3])))
    print("\nwrote", TSV)


if __name__ == "__main__":
    main()
