#!/usr/bin/env python3
"""The core list and `adopt-june` (LUA_SOURCE_PHASES.md Phase L3, Rule 2).  Reached through tools/luasrc.py:

    python tools/luasrc.py core --check                         # the list against kom_contents (exit 1 on any FAIL)
    python tools/luasrc.py core --list [--state pending]
    python tools/luasrc.py core --add data036 X.lua --origin june --why "edit: <phase> <what>" [--state pending]
    python tools/luasrc.py core --set data036 X.lua source|bytecode|pending|"blocked: <reason>"
    python tools/luasrc.py core --classify [--out F.tsv]       # every X.src.lua sidecar against ljconv(June's X.lua), T1
    python tools/luasrc.py adopt-june [<june kom>] X.lua [Y.lua ...] [--dry-run]          # June's bytecode via ljconv (default)
    python tools/luasrc.py adopt-june --list names.txt                                   # a list file (one name per line, or a TSV with `name`)
    python tools/luasrc.py adopt-june --source X.lua [--dry-run]                         # core-list names only: decompile + T1 -> BOM source
    python tools/luasrc.py iter-audit X.lua [Y.lua ...]                                  # pairs/next/foreach sites of a source file

Default mode: tools/junekom.py's strict decode, then june2017_re/ljconv.py (header 0x82 -> 0x81, opcodes renumbered, nothing else),
staged through kom.py stage-june's placement (March's kom if March has the name, else June's), duplicate and long-name rules
(reused, not reimplemented). It REFUSES a core-list name (staging June's bytecode would drop the merge or the edit) and moves a
superseded X.src.lua beside the file to <kom_contents>/../kom_backup_stage_june/<stamp>/ (compile would fold it back over June's file).
--source (core-list names only): ljconv, decompile, T1 against the ljconv output; on T0/T1 BOM + source is staged, with a ledger row,
a bytecode snapshot (the ljconv bytes) and the core row set to `source`. On a fail NOTHING is staged and the core row becomes
`blocked: <reason>` (an L5 / T2 candidate). Every adopted source file gets the iteration-order audit (pairs/next/foreach tables).
Each real run writes june2017_re/june_adopt_<stamp>.tsv (name, kom, state, june_koms, size, shipped_sha1, mode, tier, note).
--compare TSV checks the staged sha1s against an earlier run's shipped_sha1 column (the L3 calibration).

Scratch targets (calibration): set X2_KOM_CONTENTS, X2_LUA_LEDGER and X2_LUA_CORE_LIST before running; nothing else is written
(snapshots and backups follow X2_KOM_CONTENTS' parent). Packing stays a separate step: /elstools pack, ask, /x2-kom-packed copy.
Exit codes: 0 ok, 1 failed, 2 usage, 4 refused (a name was not adopted).  Never launches the game.
"""
import concurrent.futures as cf
import datetime
import glob
import hashlib
import json
import os
import re
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)
if HERE not in sys.path:
    sys.path.insert(0, HERE)

L = None        # tools/luasrc.py's module, set by luasrc._aj() (or on first use when run standalone)
CORE_LIST = os.environ.get("X2_LUA_CORE_LIST", os.path.join(TRUNK, "tools", "data", "lua_core_list.tsv"))
CORE_COLS = ["kom", "name", "origin", "why", "state", "date"]
ORIGINS = ("march", "june", "merged")
STATES = ("source", "bytecode", "pending")
RE_DIR = os.path.join(TRUNK, "june2017_re")
INDEX_CACHE = os.path.join(TRUNK, ".build", "june_lua_index.json")
ITER_FNS = {"next", "pairs", "ipairs", "foreach", "foreachi"}
ORDER_SENSITIVE = {"next", "pairs", "foreach", "foreachi"}
# earlier runs' per-file records: the staged sha1 of every June file shipped as ljconv bytecode
ADOPTED_GLOBS = ("june_npc_lua_port_*.tsv", "june_partial_port_replace_*.tsv", "june_adopt_*.tsv")


def _L():
    global L
    if L is None:
        import luasrc
        L = luasrc
    return L


def p(*a):
    print(*a, flush=True)


def die(code, *a):
    p(*a)
    sys.exit(code)


def _kom():
    import kom
    return kom


def _jk():
    import junekom
    return junekom


# ----------------------------------------------------------------------------------------------
# the core list
def core_load():
    if not os.path.isfile(CORE_LIST):
        return []
    rows, head = [], None
    for ln in open(CORE_LIST, "rb").read().decode("utf-8").splitlines():
        if not ln.strip() or ln.startswith("#"):
            continue
        cells = ln.split("\t")
        if head is None:
            head = cells
            continue
        rows.append({c: (cells[i] if i < len(cells) else "") for i, c in enumerate(head)})
    for r in rows:
        for c in CORE_COLS:
            r.setdefault(c, "")
    return rows


CORE_HEADER = ("# Core list (LUA_SOURCE_PHASES.md Rule 2): the .lua files that are BOM source instead of June's ljconv bytecode.\n"
               "# why = `kept-merged: <blocker>` (March's C++ and June's file disagree) or `edit: <plan phase> <what>` (a port must edit it).\n"
               "# state = source (BOM text in kom_contents) | pending (should become source) | bytecode (deliberately bytecode) | blocked: <reason> (T1 failed).\n"
               "# Edit with `luasrc.py core --add / --set`; `core --check` keeps it honest.\n")


def core_save(rows):
    out = ["\t".join(CORE_COLS)]
    for r in rows:
        out.append("\t".join(str(r.get(c, "")).replace("\t", " ").replace("\n", " ") for c in CORE_COLS))
    os.makedirs(os.path.dirname(CORE_LIST), exist_ok=True)
    _L().write_atomic(CORE_LIST, (CORE_HEADER + "\n".join(out) + "\n").encode("utf-8"))


def core_key(r):
    return (r["kom"], r["name"].lower())


def core_by_name(rows=None):
    out = {}
    for r in (core_load() if rows is None else rows):
        out.setdefault(r["name"].lower(), []).append(r)
    return out


def core_add(kom, name, origin, why, state="pending", update=False):
    """Add a row (or update an existing one's state/why when update). Returns (row, created)."""
    rows = core_load()
    for r in rows:
        if core_key(r) == (kom, name.lower()):
            if update:
                r["state"], r["date"] = state, datetime.date.today().isoformat()
                if why and why not in r["why"]:
                    r["why"] = r["why"] + "; " + why if r["why"] else why
                core_save(rows)
            return r, False
    r = {"kom": kom, "name": name, "origin": origin, "why": why, "state": state, "date": datetime.date.today().isoformat()}
    rows.append(r)
    core_save(rows)
    return r, True


def core_set_state(kom, name, state, note=""):
    rows = core_load()
    for r in rows:
        if core_key(r) == (kom, name.lower()):
            r["state"], r["date"] = state, datetime.date.today().isoformat()
            if note:
                r["why"] = (r["why"] + "; " + note).strip("; ")
            core_save(rows)
            return True
    return False


def state_ok(s):
    return s in STATES or (s.startswith("blocked:") and len(s) > 9)


def cmd_core_check(verbose=True):
    """-> (fails, warns, infos). Every BOM-text .lua in kom_contents is on the list; every `source` row is BOM text."""
    Lm = _L()
    rows = core_load()
    fails, warns, infos = [], [], []
    seen = {}
    for r in rows:
        k = core_key(r)
        if k in seen:
            fails.append("%s %s: listed twice" % k)
        seen[k] = r
        if r["origin"] not in ORIGINS:
            fails.append("%s %s: origin %r is not one of %s" % (r["kom"], r["name"], r["origin"], "/".join(ORIGINS)))
        if not state_ok(r["state"]):
            fails.append("%s %s: state %r is not source / bytecode / pending / blocked: <reason>" % (r["kom"], r["name"], r["state"]))
        if not r["why"].strip():
            fails.append("%s %s: no reason (a row needs `kept-merged: <blocker>` or `edit: <phase> <what>`)" % (r["kom"], r["name"]))
        elif not re.match(r"(kept-merged|edit)\b", r["why"]):
            warns.append("%s %s: why does not start with kept-merged / edit (%r)" % (r["kom"], r["name"], r["why"][:50]))
    text_files = set()
    for kom in Lm.kom_dirs_with_lua():
        d = Lm.kom_dir(kom)
        for n in Lm.lua_names(d):
            if Lm.kind(Lm.read(os.path.join(d, n))) == "bom-text":
                text_files.add((kom, n.lower()))
                row = seen.get((kom, n.lower()))
                if not row:
                    fails.append("%s %s: BOM text but not on the core list (Rule 2: only core-list files are source; `core --add`)" % (kom, n))
                elif row["state"] != "source":
                    fails.append("%s %s: BOM text, but the core row says %r (set it to `source`)" % (kom, n, row["state"]))
    for r in rows:
        k = core_key(r)
        d = Lm.kom_dir(r["kom"])
        idx = Lm.dir_index(d) if os.path.isdir(d) else {}
        rel = idx.get(r["name"].lower())
        if r["state"] == "source":
            if not rel:
                fails.append("%s %s: row says source, but the file is not in kom_contents" % k)
            elif k not in text_files:
                fails.append("%s %s: row says source, but the file is %s" % (k[0], r["name"], Lm.kind(Lm.read(os.path.join(d, rel)))))
        elif not rel:
            warns.append("%s %s: on the list (%s), not staged in this kom_contents" % (r["kom"], r["name"], r["state"]))
        if r["state"].startswith("blocked"):
            infos.append("blocked: %s %s -> %s" % (r["kom"], r["name"], r["state"][9:].strip()[:140]))
    sidecars = []
    for fn in sorted(os.listdir(Lm.KOM_ROOT)) if os.path.isdir(Lm.KOM_ROOT) else []:
        m = re.fullmatch(r"(data[i]?\d{3})_kom", fn)
        if not m:
            continue
        for rel in Lm.dir_index(os.path.join(Lm.KOM_ROOT, fn)).values():
            if rel.lower().endswith(".src.lua") and (m.group(1), rel[:-8].lower() + ".lua") not in seen:
                sidecars.append("%s %s" % (m.group(1), rel))
    if sidecars:
        infos.append("%d X.src.lua sidecar(s) are not on the list (fine: `compile` still folds an edited one, but a sidecar is not a reason to be source): %s%s" % (
            len(sidecars), "; ".join(sidecars[:6]), " ..." if len(sidecars) > 6 else ""))
    if verbose:
        by = {}
        for r in rows:
            by[r["state"].split(":")[0]] = by.get(r["state"].split(":")[0], 0) + 1
        p("== core list %s: %d row(s)  %s" % (CORE_LIST, len(rows), dict(sorted(by.items()))))
        for i in infos:
            p("  info %s" % i)
        for w in warns[:Lm.SHOW]:
            p("  WARN %s" % w)
        for f in fails[:Lm.SHOW]:
            p("  FAIL %s" % f)
        if len(warns) > Lm.SHOW or len(fails) > Lm.SHOW:
            p("  ... %d warn / %d fail in total" % (len(warns), len(fails)))
        p("CORE CHECK %s: %d FAIL, %d WARN" % ("PASS" if not fails else "FAIL", len(fails), len(warns)))
    return fails, warns, infos


# ----------------------------------------------------------------------------------------------
# June's files
def june_index():
    """lower name -> [[kom path, stem, v04, entry]] for every .lua in every June kom (cached by the koms' sizes)."""
    jk = _jk()
    koms, _skipped = jk.all_koms()
    sig = [[os.path.basename(k), os.path.getsize(k)] for k in koms]
    try:
        c = json.load(open(INDEX_CACHE, "r", encoding="utf-8"))
        if c.get("sig") == sig:
            return c["names"]
    except (OSError, ValueError):
        pass
    names = {}
    for kp in koms:
        k = jk.Kom(kp)
        for e in k.entries:
            if e["name"].lower().endswith(".lua"):
                names.setdefault(e["name"].lower(), []).append([kp, k.stem, k.v04, e])
    os.makedirs(os.path.dirname(INDEX_CACHE), exist_ok=True)
    json.dump({"sig": sig, "names": names}, open(INDEX_CACHE, "w", encoding="utf-8"))
    return names


def june_bytes(name, idx):
    """Strict-decoded June bytes of one .lua -> (bytes|None, why, koms). Every June copy must decode and be identical."""
    jk = _jk()
    cands = idx.get(name.lower())
    if not cands:
        return None, "not in any June kom", []
    outs = {}
    for kp, stem, v04, e in cands:
        with open(kp, "rb") as f:
            f.seek(e["offset"])
            raw = f.read(e["csize"])
        out, err = jk.decode(e, raw, v04)
        if out is None:
            return None, "June %s does not decode strictly: %s" % (stem, err), [c[1] for c in cands]
        outs[stem] = out
    if len({hashlib.sha1(o).hexdigest() for o in outs.values()}) > 1:
        return None, "June koms disagree: %s" % ", ".join(sorted(outs)), sorted(outs)
    return next(iter(outs.values())), "", sorted(outs)


def adopted_shas():
    """(kom, lower name) -> {sha1} of June files earlier runs staged as ljconv bytecode."""
    out = {}
    for g in ADOPTED_GLOBS:
        for f in glob.glob(os.path.join(RE_DIR, g)):
            head = None
            for ln in open(f, "rb").read().decode("utf-8", "replace").splitlines():
                if not ln.strip() or ln.startswith("#"):
                    continue
                c = ln.split("\t")
                if head is None:
                    head = c
                    continue
                if "name" not in head or "kom" not in head or "shipped_sha1" not in head:
                    break
                row = dict(zip(head, c))
                if row.get("mode", "bytecode") == "bytecode" and row.get("shipped_sha1"):
                    out.setdefault((row["kom"], row["name"].lower()), set()).add(row["shipped_sha1"])
    return out


_adopted_cache = []


def is_adopted_june(kom, name, data):
    """convert's refusal (L2 review): the file is ljconv'd June bytecode with the March 0x81 header. -> description or ''."""
    if not _adopted_cache:
        _adopted_cache.append(adopted_shas())
    shas = _adopted_cache[0].get((kom, name.lower()))
    if shas and hashlib.sha1(data).hexdigest() in shas:
        return "June's own bytecode (ljconv), staged by an earlier adopt run (june2017_re/june_*port*.tsv); `convert` is for March files. " \
               "A core-list file takes `luasrc.py adopt-june --source %s`; any other June file stays bytecode" % name
    return ""


# ----------------------------------------------------------------------------------------------
# iteration-order audit (L2's finding: text builds table constants with the parser, so pairs/next can walk another order)
def _writes(ljbc, ins):
    op, a, b, c, d = ljbc.fields(ins)
    return op < len(ljbc.OPS) and ljbc.OPS[op][1] == "dst"


def _expr(ljbc, pt, pc, reg, depth=0):
    """What register `reg` holds just before instruction `pc`, as a short source-like string."""
    if depth > 6:
        return "?"
    for q in range(pc - 1, -1, -1):
        ins = pt.bc[q]
        op, a, b, c, d = ljbc.fields(ins)
        name = ljbc.NAMES[op] if op < len(ljbc.NAMES) else "?"
        if name in ("CALL", "CALLM") and a <= reg:
            if a == reg:
                return _expr(ljbc, pt, q, a, depth + 1) + "(...)"
            return "<call result>"
        if name == "KNIL" and a <= reg <= d:
            return "nil"
        if not _writes(ljbc, ins) or a != reg:
            continue
        if name == "GGET":
            return ljbc.show_const(ljbc.kgc_at(pt, d)).strip('"')
        if name == "TGETS":
            return "%s.%s" % (_expr(ljbc, pt, q, b, depth + 1), ljbc.show_const(ljbc.kgc_at(pt, c)).strip('"'))
        if name == "TGETB":
            return "%s[%d]" % (_expr(ljbc, pt, q, b, depth + 1), c)
        if name == "TGETV":
            return "%s[%s]" % (_expr(ljbc, pt, q, b, depth + 1), _expr(ljbc, pt, q, c, depth + 1))
        if name == "MOV":
            return _expr(ljbc, pt, q, d, depth + 1)
        if name == "UGET":
            return "upvalue#%d" % d
        if name in ("TNEW", "TDUP"):
            return "{table constructor}"
        if name == "KSTR":
            return ljbc.show_const(ljbc.kgc_at(pt, d))
        if name == "FNEW":
            return "function"
        return "<%s r%d>" % (name.lower(), reg)
    return "<local r%d>" % reg


def audit_text(text):
    """Source bytes (no BOM) -> [{line, proto, fn, table, kind, sensitive}] for every pairs/next/foreach/foreachi/ipairs site and every
    ISNEXT / ITERN / ITERC loop. A site is order-sensitive when the walk is a hash walk (pairs, next, foreach, foreachi, or a custom
    iterator, whose order nobody checked); ipairs is index order."""
    Lm = _L()
    import ljbc
    bc, err = Lm.compile_text(text, debug=True)
    if bc is None:
        return None, err
    dump = ljbc.parse(bc)
    sites = []
    for pt in dump.protos:
        used = set()
        for pc, ins in enumerate(pt.bc):
            op, a, b, c, d = ljbc.fields(ins)
            name = ljbc.NAMES[op] if op < len(ljbc.NAMES) else "?"
            line = pt.lines[pc] if pt.lines else 0
            if name in ("ISNEXT", "ITERC"):
                func = a - 3
                call_pc = None
                for q in range(pc - 1, -1, -1):
                    if _writes(ljbc, pt.bc[q]) and ljbc.fields(pt.bc[q])[1] == func or (
                            ljbc.NAMES[ljbc.fields(pt.bc[q])[0]] in ("CALL", "CALLM") and ljbc.fields(pt.bc[q])[1] == func):
                        call_pc = q
                        break
                if call_pc is None:
                    sites.append(dict(line=line, proto=pt.index, fn="?", table="?", kind="loop %s" % name, sensitive=True))
                    continue
                w = ljbc.NAMES[ljbc.fields(pt.bc[call_pc])[0]]
                if w in ("CALL", "CALLM"):
                    used.add(call_pc)
                    fn = _expr(ljbc, pt, call_pc, func)
                    tab = _expr(ljbc, pt, call_pc, func + 1)
                else:
                    fn = _expr(ljbc, pt, pc, func)
                    tab = _expr(ljbc, pt, pc, func + 1)
                base = fn.split(".")[-1].replace("(...)", "")
                sites.append(dict(line=line, proto=pt.index, fn=fn, table=tab, kind="loop %s" % name,
                                  sensitive=(base in ORDER_SENSITIVE) or (base not in ITER_FNS)))
        for pc, ins in enumerate(pt.bc):
            op, a, b, c, d = ljbc.fields(ins)
            name = ljbc.NAMES[op] if op < len(ljbc.NAMES) else "?"
            if name in ("CALL", "CALLM", "CALLT", "CALLMT") and pc not in used:
                fn = _expr(ljbc, pt, pc, a)
                base = fn.split(".")[-1]
                if base in ITER_FNS:
                    sites.append(dict(line=pt.lines[pc] if pt.lines else 0, proto=pt.index, fn=fn, table=_expr(ljbc, pt, pc, a + 1),
                                      kind="call", sensitive=base in ORDER_SENSITIVE))
    return sorted(sites, key=lambda s: (s["line"], s["proto"])), ""


def audit_lines(name, sites):
    sens = [s for s in sites if s["sensitive"]]
    out = ["ITER AUDIT %s: %d site(s), %d order-sensitive" % (name, len(sites), len(sens))]
    for s in sites:
        out.append("  line %s (proto #%d) %s %s(%s)%s" % (s["line"] or "?", s["proto"], s["kind"], s["fn"], s["table"],
                                                         "  <-- order-sensitive" if s["sensitive"] else "  (index order)"))
    return out


def cmd_iter_audit(a):
    Lm = _L()
    bad = 0
    for f in a.files:
        data = Lm.read(f)
        text = Lm.strip_bom(data) if Lm.kind(data) == "bom-text" else data
        sites, err = audit_text(text)
        if sites is None:
            p("  %s: does not compile (%s)" % (f, err)); bad += 1; continue
        for l in audit_lines(os.path.basename(f), sites):
            p(l)
    sys.exit(1 if bad else 0)


# ----------------------------------------------------------------------------------------------
# core --classify: every X.src.lua sidecar against ljconv(June's X.lua)
def _classify_task(item):
    kom, name, sidecar_path = item
    global L
    if L is None:
        import luasrc
        L = luasrc
    idx = june_index()
    data, why, koms = june_bytes(name, idx)
    if data is None:
        return kom, name, "not-in-June" if why.startswith("not in") else "june-undecodable", why
    if data[:4] != L.JUNE_BC:
        return kom, name, "june-not-bytecode", L.kind(data)
    m, table = L.ljconv_mod()
    try:
        ref = m.transcode(data, table)[0]
    except Exception as e:
        return kom, name, "ljconv-refused", str(e)[:120]
    text = L.strip_bom(L.read(sidecar_path))
    tier, r, err = L.t1_tier(text, ref)
    if tier in ("T0", "T1"):
        return kom, name, "same-as-June (%s)" % tier, "sidecar recompiles to ljconv(June) at %s; June's bytecode is equivalent" % tier
    if tier == "NOCOMPILE":
        return kom, name, "sidecar-no-compile", err[:140]
    first = ("%s: %s" % r.diffs[0])[:160] if r and r.diffs else "?"
    return kom, name, "differs-from-June", "%d T1 difference(s) vs ljconv(June); first %s" % (r.total if r else 0, first)


def cmd_core_classify(out_path, jobs):
    Lm = _L()
    items = []
    for fn in sorted(os.listdir(Lm.KOM_ROOT)):
        m = re.fullmatch(r"(data[i]?\d{3})_kom", fn)
        if not m:
            continue
        d = os.path.join(Lm.KOM_ROOT, fn)
        for rel in sorted(Lm.dir_index(d).values()):
            if rel.lower().endswith(".src.lua"):
                items.append((m.group(1), rel[:-8] + ".lua", os.path.join(d, rel)))
    p("== core --classify: %d sidecar(s) against ljconv(June's file)" % len(items))
    june_index()
    res = []
    with cf.ProcessPoolExecutor(max_workers=jobs) as ex:
        for r in ex.map(_classify_task, items):
            res.append(r)
            p("  %-9s %-46s %s" % (r[0], r[1], r[2]))
    tally = {}
    for r in res:
        tally[r[2].split(" ")[0]] = tally.get(r[2].split(" ")[0], 0) + 1
    p("  %s" % dict(sorted(tally.items())))
    if out_path:
        with open(out_path, "w", encoding="utf-8", newline="\n") as f:
            f.write("kom\tname\tvs_june\tdetail\n" + "\n".join("\t".join(x) for x in res) + "\n")
        p("  written: %s" % out_path)


# ----------------------------------------------------------------------------------------------
# adopt-june
def _looks_like_kom(s):
    return bool(re.fullmatch(r"(?i)(data[i]?)?\d{3}(\.kom)?|datai?\d{3}_kom", s.strip()))


def cmd_adopt(a):
    Lm = _L()
    km = _kom()
    items = list(a.items)
    june_kom = None
    if items and not os.path.isfile(items[0]) and _looks_like_kom(items[0]) and (len(items) > 1 or a.list):
        try:
            june_kom = Lm.norm(items.pop(0).replace("_kom", ""))
        except SystemExit:
            raise
    names, seen = [], set()
    for it in items + ([a.list] if a.list else []):
        for n in km.read_names(it):
            if n.lower() not in seen:
                seen.add(n.lower())
                names.append(n)
    if not names:
        die(2, "adopt-june needs file names (or --list FILE)")
    core = core_by_name()
    mode = "source" if a.source else "bytecode"
    p("== adopt-june %s%s: %d name(s)%s" % (mode, " (DRY RUN, nothing is written)" if a.dry_run else "", len(names),
                                          " from June %s" % june_kom if june_kom else ""))
    refused, todo = [], []
    for n in names:
        rows = core.get(n.lower())
        if a.source and not rows:
            refused.append((n, "", "not on the core list: --source is for core-list files only (Rule 2). A port that must edit this file adds the row "
                                  "(`luasrc.py core --add <kom> %s --origin june --why \"edit: <phase> <what>\"`); otherwise adopt it without --source" % n))
        elif a.source and rows[0]["why"].startswith("kept-merged") and not a.lifted:
            refused.append((n, rows[0]["kom"], "kept-merged row: --source would replace the merged file with June's whole, which is exactly what the blocker forbids "
                                                "(%s). Pass --lifted once the blocker is gone" % rows[0]["why"][:90].rstrip(" ,;:(") + "..."))
        elif not a.source and rows:
            refused.append((n, rows[0]["kom"], "on the core list (%s; %s): staging June's bytecode would drop that merge/edit. Use --source once the "
                                                "blocker is lifted, or leave it" % (rows[0]["state"].split(":")[0], rows[0]["why"][:90].rstrip(" ,;:(") + "...")))
        else:
            todo.append(n)
    m, table = Lm.ljconv_mod()
    ok_source = {}      # lower name -> dict(tier, ref, text, audit)
    blocked = {}        # lower name -> reason

    def transform(name, data, stem):
        if data[:4] != Lm.JUNE_BC:
            return None, "not June bytecode (%s): nothing to transcode, and a text file here needs a BOM decision, so it is refused" % Lm.kind(data)
        try:
            ref = m.transcode(data, table)[0]
        except Exception as e:
            return None, "ljconv refused: %s" % e
        if not a.source:
            return ref, "ljconv"
        row = next((r for r in core.get(name.lower(), [])), None)
        if row["kom"] != stem:
            return None, "the core list says %s but staging placed it in %s (fix the row or the placement)" % (row["kom"], stem)
        t0 = time.time()
        p("  decompiling %s (%d bytes of ljconv bytecode) ..." % (name, len(ref)))
        tier, text, why = Lm.prove_decompile(ref)
        p("  %s: %s in %.0fs" % (name, tier if text is not None else "no T1 (%s)" % tier, time.time() - t0))
        if text is None:
            blocked[name.lower()] = why
            return None, "T1 %s: %s" % ("FAIL" if tier == "FAIL" else tier, why[:300])
        sites, err = audit_text(text)
        if sites is None:
            blocked[name.lower()] = "decompile proven %s but does not parse under the audit (%s)" % (tier, err)
            return None, blocked[name.lower()]
        ok_source[name.lower()] = dict(tier=tier, ref=ref, text=text, sites=sites)
        return Lm.BOM + text, "source %s" % tier

    for n, stem, why in sorted(refused, key=lambda r: r[0].lower()):
        p("  REFUSED %s%s: %s" % (n, " (" + stem + ")" if stem else "", why))
    res = None
    if todo:
        res = km.cmd_stage_june(todo, a.dry_run, a.long_names, a.jobs, a.plan_out, transform=transform,
                                only_koms={june_kom} if june_kom else None, move_sidecars=True, exit_on_refuse=False,
                                transform_jobs=a.jobs if a.source else 1)
    staged = res["staged"] if res else []
    nrefused = len(refused) + (len(res["refused"]) if res else 0)
    refused = refused + (res["refused"] if res else [])
    # source outcomes
    today = datetime.date.today().isoformat()
    if a.source:
        for s in staged:
            info = ok_source.get(s["name"].lower())
            if not info:
                continue
            for l in audit_lines(s["name"], info["sites"]):
                p(l)
            if a.dry_run:
                continue
            rows = Lm.ledger_load()
            rows.append(Lm.new_row(s["stem"], s["name"], "june", "ljconv(June bytecode)", Lm.sha1(info["ref"]), "decompile", info["tier"],
                                   Lm.sha1(info["text"]), "L3 adopt-june", "adopt-june --source: June %s -> ljconv -> decompile proven %s; %s" % (
                                       ",".join(s["junes"]), info["tier"], "%d iteration site(s), %d order-sensitive" % (
                                           len(info["sites"]), sum(1 for x in info["sites"] if x["sensitive"])))))
            Lm.ledger_save(rows)
            sd = Lm.snapshot_dir(s["stem"])
            os.makedirs(sd, exist_ok=True)
            Lm.write_atomic(os.path.join(sd, s["name"]), info["ref"])
            core_set_state(s["stem"], s["name"], "source", "adopted from June %s (%s)" % (",".join(s["junes"]), info["tier"]))
        for low, why in blocked.items():
            row = next(iter(core.get(low, [])), None)
            if row and not a.dry_run:
                core_set_state(row["kom"], row["name"], "blocked: " + re.sub(r"\s+", " ", why)[:200])
            p("  BLOCKED %s: %s" % (row["name"] if row else low, why[:200]))
    # per-run TSV
    tsv_path = a.tsv_out
    if not tsv_path and not a.dry_run and staged:
        tsv_path = os.path.join(RE_DIR, "june_adopt_%s.tsv" % datetime.datetime.now().strftime("%Y%m%d_%H%M%S"))
    if tsv_path and (staged or blocked):
        lines = ["# adopt-june --%s run %s (LUA_SOURCE_PHASES.md L3)%s" % (mode, today, " DRY RUN" if a.dry_run else ""),
                 "# mode bytecode = ljconv(June bytecode), shipped_sha1 is its sha1; mode source = BOM + decompile proven at `tier`, shipped_sha1 is the staged file's",
                 "name\tkom\tstate\tjune_koms\tsize\tshipped_sha1\tmode\ttier\tnote"]
        for s in staged:
            info = ok_source.get(s["name"].lower())
            lines.append("\t".join([s["name"], s["stem"], s["state"], ",".join(s["junes"]), str(s["size"]), s["sha1"], mode,
                                    info["tier"] if info else "", s["note"]]))
        for n, stem, why in refused:
            lines.append("\t".join([n, stem, "REFUSED", "", "", "", mode, "", re.sub(r"\s+", " ", why)[:300]]))
        os.makedirs(os.path.dirname(tsv_path), exist_ok=True)
        with open(tsv_path, "w", encoding="utf-8", newline="\n") as f:
            f.write("\n".join(lines) + "\n")
        p("  per-run record: %s" % tsv_path)
    # --compare
    rc = 0
    if a.compare:
        want = {}
        head = None
        for ln in open(a.compare, "rb").read().decode("utf-8").splitlines():
            if not ln.strip() or ln.startswith("#"):
                continue
            c = ln.split("\t")
            if head is None:
                head = c
                continue
            row = dict(zip(head, c))
            want[row["name"].lower()] = row
        match, bad = 0, []
        for s in staged:
            w = want.get(s["name"].lower())
            if not w:
                bad.append("%s: not in %s" % (s["name"], os.path.basename(a.compare)))
            elif w["shipped_sha1"] == s["sha1"] and (not w.get("kom") or w["kom"] == s["stem"]):
                match += 1
            else:
                bad.append("%s: sha1 %s vs recorded %s (kom %s vs %s)" % (s["name"], s["sha1"][:10], w["shipped_sha1"][:10], s["stem"], w.get("kom")))
        p("COMPARE %s: %d of %d staged file(s) are byte-identical to %s (and in the same kom)" % (
            "OK" if not bad and staged else "FAIL", match, len(staged), os.path.basename(a.compare)))
        for b in bad[:20]:
            p("    MISMATCH %s" % b)
        rc = 0 if not bad and staged else 1
    allref = {(n, stem) for n, stem, _w in refused}
    p("ADOPT-JUNE %s (%s): %d staged%s, %d refused%s" % (
        "DRY RUN" if a.dry_run else ("OK" if not allref else "PARTIAL"), mode, len(staged),
        " (%d source, %d blocked)" % (len(ok_source), len(blocked)) if a.source else "", len(allref),
        "" if a.dry_run or not staged else ". Next: /elstools pack %s   (then ask before `/x2-kom-packed copy`)" % " ".join(sorted({s["stem"] for s in staged}))))
    if rc:
        sys.exit(rc)
    if allref:
        sys.exit(4)


def cmd_core(a):
    Lm = _L()
    if a.add:
        kom, name = Lm.norm(a.add[0]), a.add[1]
        if not a.why:
            die(2, "core --add needs --why (`kept-merged: <blocker>` or `edit: <plan phase> <what>`)")
        if not state_ok(a.state):
            die(2, "bad --state %r" % a.state)
        r, created = core_add(kom, name, a.origin, a.why, a.state)
        p("%s %s %s (%s, %s)" % ("ADDED" if created else "already on the list:", kom, name, r["origin"], r["state"]))
        return
    if a.set:
        kom, name, st = Lm.norm(a.set[0]), a.set[1], a.set[2]
        if not state_ok(st):
            die(2, "bad state %r (source / bytecode / pending / blocked: <reason>)" % st)
        if not core_set_state(kom, name, st, a.why or ""):
            die(4, "%s %s is not on the core list" % (kom, name))
        p("SET %s %s -> %s" % (kom, name, st))
        return
    if a.classify:
        cmd_core_classify(a.out, a.jobs)
        return
    if a.list:
        rows = [r for r in core_load() if not a.state or r["state"].startswith(a.state)]
        for r in rows:
            p("%-9s %-48s %-7s %-10s %s" % (r["kom"], r["name"], r["origin"], r["state"][:10], r["why"][:110]))
        p("%d row(s)" % len(rows))
        return
    fails, _w, _i = cmd_core_check(True)
    if fails:
        sys.exit(1)
