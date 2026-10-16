"""W0 census: June line-map trigger timelines (TRIGGER_DATA) vs what the March tree holds.

Read-only. Run:  python -I june2017_re/w0_trigger_census.py
Writes june2017_re/w0_trigger_census.tsv (tab separated, header row, LF).

Inputs (all paths below are fixed; a missing input is reported as "not found", never guessed):
  june_src_fixed/**/*.src.lua       June linemap sources (name contains "linemap" and text contains TRIGGER_DATA)
  <GAME>/kom_contents/dataNNN_kom   live staged March tree
  <GAME>/kom_contents_bytecode      pre-mod March bytecode snapshot (only some files)
  tools/data/lua_source_ledger.tsv  converted/adopted source rows
  june2017_re/june_adopt_*.tsv      adopt-june run records (state new/replace, shipped sha1)
  june2017_re/c3y_unreached_*_census.tsv  owner lookup for worlds March lacks
"""
import csv
import glob
import hashlib
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)
GAME = "F:/elsword stuff/elsword_2014/els_2014/237311/24965799"
STAGED = GAME + "/kom_contents"
SNAP = GAME + "/kom_contents_bytecode"
JUNE_SRC = os.path.join(TRUNK, "june_src_fixed")
LEDGER = os.path.join(TRUNK, "tools", "data", "lua_source_ledger.tsv")
OUT = os.path.join(HERE, "w0_trigger_census.tsv")
NF = "not found"

JUNE_ORIG = "F:/elsword stuff/elsword_2014/els_2014/237311/14JUN17/data"
KEYS = [b"AUTO_DISABLE_LINE", b"MOTIONNAME_EFFECTSETNAME", b"TAT_LINE_CHANGE_MOVABLE_DATA"]


# ---------------------------------------------------------------- mini Lua table parser
def strip_comments(t):
    """Remove -- comments (line and long) outside strings."""
    out = []
    i, n = 0, len(t)
    while i < n:
        c = t[i]
        if c in "\"'":
            j = i + 1
            while j < n and t[j] != c:
                if t[j] == "\\":
                    j += 1
                j += 1
            out.append(t[i:j + 1])
            i = j + 1
        elif t.startswith("--", i):
            m = re.match(r"--\[(=*)\[", t[i:])
            if m:
                end = t.find("]" + m.group(1) + "]", i)
                i = n if end < 0 else end + len(m.group(1)) + 2
            else:
                j = t.find("\n", i)
                i = n if j < 0 else j
        else:
            out.append(c)
            i += 1
    return "".join(out)


class P:
    def __init__(self, t, i=0):
        self.t, self.i = t, i

    def ws(self):
        while self.i < len(self.t) and self.t[self.i] in " \t\r\n":
            self.i += 1

    def value(self):
        self.ws()
        t = self.t
        c = t[self.i]
        if c == "{":
            return self.table()
        if c in "\"'":
            j = self.i + 1
            while t[j] != c:
                if t[j] == "\\":
                    j += 1
                j += 1
            s = t[self.i + 1:j]
            self.i = j + 1
            return ("s", s)
        depth = 0
        j = self.i
        while j < len(t):
            ch = t[j]
            if ch in "([":
                depth += 1
            elif ch in ")]":
                depth -= 1
            elif depth == 0 and ch in ",;}":
                break
            j += 1
        raw = t[self.i:j].strip()
        self.i = j
        return ("r", raw)

    def table(self):
        assert self.t[self.i] == "{"
        self.i += 1
        items = []  # list of (key or None, value)
        while True:
            self.ws()
            if self.t[self.i] == "}":
                self.i += 1
                return ("t", items)
            m = re.compile(r"([A-Za-z_][A-Za-z0-9_]*)\s*=(?!=)").match(self.t, self.i)
            if m:
                self.i = m.end()
                items.append((m.group(1), self.value()))
            else:
                items.append((None, self.value()))
            self.ws()
            if self.t[self.i] in ",;":
                self.i += 1


def tget(tab, key):
    for k, v in tab[1]:
        if k == key:
            return v
    return None


def sval(v):
    if v is None:
        return None
    return v[1] if v[0] in ("s", "r") else None


def parse_calls(text):
    """All g_pLineMap:AddXxx_LUA( { ... } ) tables -> list of (method, table)."""
    res = []
    for m in re.finditer(r":\s*(\w+_LUA)\s*\(\s*\{", text):
        p = P(text, m.end() - 1)
        try:
            res.append((m.group(1), p.table()))
        except Exception:
            res.append((m.group(1), None))
    return res


def analyse(path):
    raw = open(path, "rb").read().decode("utf-8", "replace")
    text = strip_comments(raw)
    calls = parse_calls(text)
    ntrig = 0
    mesh, line, etc = {}, {}, {}
    auto_disable = potal = movable = effset_rows = 0
    effsets, sounds = [], []
    bad = 0
    for meth, tab in calls:
        if tab is None:
            bad += 1
            continue
        subs = [(k, v) for k, v in tab[1] if k and k.startswith("TRIGGER_DATA") and v[0] == "t"]
        has_td = any(k and k.startswith("TRIGGER_DATA") for k, _ in tab[1])
        if not has_td:
            continue
        ntrig += 1
        # TRIGGER_DATA0 = { TRIGGER_DATA_MESH0 = {...}, ... } : flatten one more level
        flat = []
        for k, v in subs:
            if any(kk and kk.startswith("TRIGGER_DATA_") for kk, _ in v[1]):
                flat.extend((kk, vv) for kk, vv in v[1] if kk and kk.startswith("TRIGGER_DATA_") and vv[0] == "t")
            else:
                flat.append((k, v))
        for k, v in flat:
            act = sval(tget(v, "TRIGGER_ACTION")) or ""
            am = re.search(r"(TAT_[A-Z_0-9]+)", act)
            act = am.group(1) if am else "NO_ACTION(" + k + ")"
            bucket = mesh if act.startswith("TAT_MESH_") else line if act.startswith("TAT_LINE_") else etc
            bucket[act] = bucket.get(act, 0) + 1
            ad = sval(tget(v, "AUTO_DISABLE_LINE"))
            if ad is not None and ad.strip().upper() in ("TRUE", "1"):
                auto_disable += 1
            if act == "TAT_LINE_CHANGE_TYPE":
                if "LT_POTAL" in (sval(tget(v, "CHANGE_LINE_TYPE")) or ""):
                    potal += 1
            if act == "TAT_LINE_CHANGE_MOVABLE_DATA":
                movable += 1
            if act == "TAT_ETC_PLAY_EFFECTSET":
                effset_rows += 1
                effsets.append(sval(tget(v, "MOTIONNAME_EFFECTSETNAME")) or NF)
            if act == "TAT_ETC_PLAY_SOUND":
                s = sval(tget(v, "LINE_SOUND"))
                if s is not None and s not in sounds:
                    sounds.append(s)
    return dict(n=ntrig, mesh=mesh, line=line, etc=etc, auto=auto_disable, potal=potal,
                movable=movable, effset_rows=effset_rows, effsets=effsets, sounds=sounds,
                bad=bad, raw_auto=raw.count("AUTO_DISABLE_LINE"))


def fmt(d):
    return ",".join("%s:%d" % (k, d[k]) for k in sorted(d)) or "-"


# ---------------------------------------------------------------- helpers
def sha1(b):
    return hashlib.sha1(b).hexdigest()


def read(p):
    with open(p, "rb") as f:
        return f.read()


def tsv_rows(path):
    rows = []
    with open(path, "r", encoding="utf-8", errors="replace", newline="") as f:
        for line in f:
            line = line.rstrip("\r\n")
            if not line or line.startswith("#"):
                continue
            rows.append(line.split("\t"))
    return rows


def index_kom_dir(root):
    idx = {}
    if not os.path.isdir(root):
        return idx
    for d in sorted(os.listdir(root)):
        full = os.path.join(root, d)
        if d.endswith("_kom") and os.path.isdir(full):
            for f in os.listdir(full):
                if f.lower().endswith(".src.lua"):
                    continue  # sidecar text, not a shipped file
                idx.setdefault(f.lower(), []).append((d, f))
    return idx


def ljconv_of_june(name_lower):
    """ljconv(June bytecode) for a data040 file, or None (with the reason in the 2nd slot)."""
    try:
        sys.path.insert(0, HERE)
        import ljconv  # noqa: E402
        table = ljconv.load_table(os.path.join(HERE, "ljconv_table.tsv"))
    except Exception as e:  # pragma: no cover
        return None, "ljconv import failed: %s" % e
    for root in sorted(glob.glob(JUNE_ORIG + "/*_kom")):
        for f in os.listdir(root):
            if f.lower() == name_lower:
                d = read(os.path.join(root, f))
                if d[:4] != ljconv.JUNE_MAGIC:
                    return None, "June file header %s" % d[:4].hex()
                try:
                    out = ljconv.transcode(d, table)[0]
                except Exception as e:
                    return None, "ljconv failed: %s" % e
                return out, os.path.basename(root)
    return None, "no June original file"


def main():
    # ---- June linemap list
    june = {}  # key lower name.lua -> (kom, path, display name)
    for root, _, files in os.walk(JUNE_SRC):
        for f in files:
            if f.lower().endswith(".src.lua") and "linemap" in f.lower():
                p = os.path.join(root, f)
                if b"TRIGGER_DATA" in read(p):
                    name = f[:-8] + ".lua"
                    kom = os.path.basename(root)
                    june.setdefault(name.lower(), []).append((kom, p, name))
    names = sorted(june, key=lambda k: june[k][0][2].lower())

    # ---- June world rows: WI -> file ; files that reference each linemap
    wd_re = re.compile(r'worldName\s*=\s*"(WI_[A-Za-z0-9_]+)"\s*,\s*dataFileName\s*=\s*"([^"]+)"')
    june_wd = {}  # data file lower -> [WI]
    june_texts = {}
    for root, _, files in os.walk(JUNE_SRC):
        for f in files:
            if f.lower().endswith(".src.lua"):
                p = os.path.join(root, f)
                t = read(p).decode("utf-8", "replace")
                june_texts[p] = t.lower()
                for wi, df in wd_re.findall(t):
                    june_wd.setdefault(df.lower(), [])
                    if wi not in june_wd[df.lower()]:
                        june_wd[df.lower()].append(wi)

    # ---- staged March tree
    st_idx = index_kom_dir(STAGED)
    sn_idx = index_kom_dir(SNAP)
    st_wd = {}
    for d in glob.glob(STAGED + "/*_kom/WorldData*.lua"):
        b = read(d)
        if b[:3] == b"\xef\xbb\xbf" or b[:1] != b"\x1b":
            for wi, df in wd_re.findall(b.decode("utf-8", "replace")):
                st_wd.setdefault(df.lower(), [])
                if wi not in st_wd[df.lower()]:
                    st_wd[df.lower()].append(wi)
    sn_wd = {}
    for d in glob.glob(SNAP + "/*_kom/WorldData*.lua"):
        b = read(d)
        if b[:3] == b"\xef\xbb\xbf" or b[:1] != b"\x1b":
            for wi, df in wd_re.findall(b.decode("utf-8", "replace")):
                sn_wd.setdefault(df.lower(), [])
                if wi not in sn_wd[df.lower()]:
                    sn_wd[df.lower()].append(wi)
    # one pass over staged .lua: which files mention each linemap name (lowercased bytes)
    needles = {n: n.encode("ascii", "ignore") for n in names}
    st_refs = {n: [] for n in names}
    for d in sorted(os.listdir(STAGED)):
        full = os.path.join(STAGED, d)
        if not (d.endswith("_kom") and os.path.isdir(full)):
            continue
        for f in os.listdir(full):
            if not f.lower().endswith(".lua") or f.lower().endswith(".src.lua"):
                continue
            b = read(os.path.join(full, f)).lower()
            for n, nb in needles.items():
                if nb in b and f.lower() != n:
                    st_refs[n].append(f)

    # ---- ledger + adopt records
    ledger = {}
    if os.path.exists(LEDGER):
        for r in tsv_rows(LEDGER)[1:]:
            if len(r) >= 12:
                ledger[(r[0], r[1].lower())] = r
    adopt = {}  # name lower -> list of (file, state, sha, mode)
    for ap in sorted(glob.glob(os.path.join(HERE, "june_adopt_*.tsv"))):
        for r in tsv_rows(ap):
            if r[0] == "name" or len(r) < 7:
                continue
            adopt.setdefault(r[0].lower(), []).append((os.path.basename(ap), r[2], r[5], r[6]))

    # ---- owner lookup from the c3y censuses
    dun = []
    p = os.path.join(HERE, "c3y_unreached_dungeon_census.tsv")
    if os.path.exists(p):
        hdr = None
        for r in tsv_rows(p):
            if hdr is None:
                hdr = r
                continue
            dun.append(dict(zip(hdr, r)))
    wor = []
    p = os.path.join(HERE, "c3y_unreached_world_census.tsv")
    if os.path.exists(p):
        hdr = None
        for r in tsv_rows(p):
            if hdr is None:
                hdr = r
                continue
            wor.append(dict(zip(hdr, r)))

    cols = ["file", "june_kom", "n_triggers", "mesh_actions", "line_actions", "etc_actions",
            "auto_disable_line", "potal_retype", "play_effectset_values", "play_sound_values",
            "in_march_kom", "staged_copy_origin", "staged_has_trigger_keys", "june_worlds",
            "march_worlds", "owner", "movable_data_rows", "play_effectset_rows",
            "march_original_worlds", "parse_warnings"]
    rows = []
    for n in names:
        kom, path, disp = june[n][0]
        a = analyse(path)
        warn = []
        if a["bad"]:
            warn.append("%d unparsed AddXxx_LUA call(s)" % a["bad"])
        if a["raw_auto"] != a["auto"]:
            warn.append("AUTO_DISABLE_LINE text count %d vs trigger-row count %d" % (a["raw_auto"], a["auto"]))
        if len(june[n]) > 1:
            warn.append("duplicate June file in %s" % ",".join(x[0] for x in june[n]))

        # June worlds
        refs = []
        for pp, t in june_texts.items():
            if n in t and os.path.basename(pp).lower() != n[:-4] + ".src.lua":
                refs.append(os.path.basename(pp)[:-8] + ".lua")
        jw = []
        for r in sorted(set(refs)):
            if r.lower() in june_wd:
                jw.extend(june_wd[r.lower()])
            else:
                jw.append("file:" + r)
        # staged March
        hits = st_idx.get(n, [])
        in_march = "; ".join("%s/%s" % (d, f) for d, f in hits) if hits else "no"
        origin = "absent"
        keyflags = NF
        mw = []
        mow = []
        if hits:
            d, f = hits[0]
            sp = os.path.join(STAGED, d, f)
            b = read(sp)
            sh = sha1(b)
            km = d[:-4]
            hdr = b[:4].hex()
            how = []
            rec = [x for x in adopt.get(n, []) if x[2] == sh]
            lrow = ledger.get((km, n))
            if rec:
                r0 = rec[-1]
                kind = "june-ljconv" if r0[3] == "bytecode" else "june-source"
                origin = kind
                how.append("sha1 matches %s row (state=%s mode=%s)" % (r0[0], r0[1], r0[3]))
            elif lrow:
                origin = "june-source" if lrow[2] == "june" else "march"
                how.append("ledger origin=%s tier=%s" % (lrow[2], lrow[6]))
            else:
                lj, why = ljconv_of_june(n)
                if lj is not None and sha1(lj) == sh:
                    origin = "june-ljconv"
                    how.append("no adopt row, but bytes equal ljconv(June %s) (sha1 %s)" % (why, sh[:10]))
                else:
                    origin = "march"
                    how.append("no adopt/ledger row matches sha1; header %s; ljconv(June) compare: %s" % (
                        hdr, "differs" if lj is not None else why))
                    if n in adopt:
                        how.append("an adopt row exists for the name but with a different sha1")
            how.append("state_in_adopt=" + (",".join(sorted(set(x[1] for x in adopt.get(n, [])))) or "none"))
            how.append("snapshot=" + ("yes:" + sn_idx[n][0][0] if n in sn_idx else "no"))
            origin += " (" + "; ".join(how) + ")"
            keyflags = ",".join(k.decode() for k in KEYS if k in b) or "none"
            refs_m = sorted(set(st_refs[n]))
            for r in refs_m:
                if r.lower() in st_wd:
                    mw.extend(st_wd[r.lower()])
                else:
                    mw.append("file:" + r)
            for r in refs_m:
                if r.lower() in sn_wd:
                    mow.extend(sn_wd[r.lower()])
        else:
            # no staged copy: still list any staged referrers
            for r in sorted(set(st_refs[n])):
                mw.append(st_wd[r.lower()][0] if r.lower() in st_wd else "file:" + r)
        mw = sorted(set(mw))
        mow = sorted(set(mow))

        # owner
        if hits:
            owner = "already live in March"
        else:
            owners = []
            wis = [w for w in jw if w.startswith("WI_")]
            for w in wis:
                for dr in dun:
                    if w in (dr.get("worlds", "") or "").split(";") and dr.get("owner"):
                        owners.append(dr["owner"])
            for wr in wor:
                if n in (wr.get("line_maps(kc/june)", "") or "").lower() or wr.get("world") in wis:
                    for dr in dun:
                        if dr.get("script") in (wr.get("used_by", "") or "").split(";") and dr.get("owner"):
                            owners.append(dr["owner"])
            owners = sorted(set(owners))
            if owners:
                owner = "; ".join(owners)
            elif disp.lower().startswith(("elysion", "atlas")):
                owner = "atlas (by name prefix; no census row)"
            else:
                owner = "unowned"
        rows.append([
            disp, kom, str(a["n"]), fmt(a["mesh"]), fmt(a["line"]), fmt(a["etc"]),
            str(a["auto"]), str(a["potal"]), "|".join(a["effsets"]) or "-", "|".join(a["sounds"]) or "-",
            in_march, origin, keyflags, ";".join(jw) or NF, ";".join(mw) or ("none" if hits else NF),
            owner, str(a["movable"]), str(a["effset_rows"]), ";".join(mow) or "-", "; ".join(warn) or "-",
        ])

    with open(OUT, "w", encoding="utf-8", newline="") as f:
        f.write("\t".join(cols) + "\n")
        for r in rows:
            f.write("\t".join(x.replace("\t", " ").replace("\n", " ") for x in r) + "\n")

    # summary
    ci = {c: i for i, c in enumerate(cols)}
    print("rows", len(rows))
    for c in ("n_triggers", "auto_disable_line", "potal_retype", "play_effectset_rows", "movable_data_rows"):
        print(c, sum(int(r[ci[c]]) for r in rows))
    print("in_march yes", sum(1 for r in rows if r[ci["in_march_kom"]] != "no"))
    for r in rows:
        if int(r[ci["auto_disable_line"]]) or int(r[ci["potal_retype"]]) or int(r[ci["play_effectset_rows"]]) or int(r[ci["movable_data_rows"]]):
            print("\t".join(r[ci[c]] for c in ("file", "auto_disable_line", "potal_retype", "play_effectset_rows",
                                               "movable_data_rows", "in_march_kom", "owner")))
            print("   origin:", r[ci["staged_copy_origin"]][:160], "| keys:", r[ci["staged_has_trigger_keys"]])
    return 0


if __name__ == "__main__":
    sys.exit(main())
