"""June effect C3z-3 (2026-10-10): March's dungeon registry (data035 DungeonData.lua, data036 DungeonEnum.lua) against June's,
for every dungeon id both register.

How it reads them:
  - March: the staged kom_contents files (BOM source or KOG 0x81 bytecode). June: its own data035/data036 bytecode
    (junekom extract -> june2017_re/june_lua/, ljconv'd to 0x81 in memory). Both RUN in the KOG luajit
    (elstools/vendor/luajit) through june2017_re/dungeon_registry_dump.lua, which serializes every AddDungeonData table.
  - dungeonID names resolve through each side's own DungeonEnum: DUNGEON_ID, and UNUSED_DUNGEON_ID (June registers its
    7 *_SECRET_COMMON rows through that table; March's DungeonEnum has it too since C3z-3).
  - STR_ID_<n> must exist in March's StringID_def.lua (data036) and General.ess; the name column decodes General.ess.

Writes
  june2017_re/dungeon_registry_census.tsv   one row per dungeon id registered by both: fields that differ (March -> June),
                                            fields only one side has, the June fields March's loader does not read, verdict
  june2017_re/dungeon_enum_census.tsv       one row per DUNGEON_ID name in either file: March value, June value, kind
    python june2017_re/dungeon_registry_census.py [--march-dd FILE] [--march-de FILE]   (from the Trunk)
"""
import os, re, sys, subprocess, argparse, collections, tempfile
TRUNK = os.getcwd()
sys.path.insert(0, os.path.join(TRUNK, "tools"))
import kom, luasrc

KC = kom.KOM_ROOT
LJ = os.path.join(TRUNK, "elstools", "vendor", "luajit", "luajit.exe")
DUMP = os.path.join(TRUNK, "june2017_re", "dungeon_registry_dump.lua")
REG = os.path.join(TRUNK, "june2017_re", "lua_registry_dump.lua")
JL = os.path.join(TRUNK, "june2017_re", "june_lua")
OUT = os.path.join(TRUNK, "june2017_re")
GAME = os.path.join(os.path.dirname(KC), "data")

# Keys CX2DungeonManager::AddDungeonData_LUA (X2DungeonManager.cpp:112-574) reads in this build; the file says how its
# #ifdef'd reads were settled.
LOADER_KEYS = os.path.join(OUT, "dungeon_registry_loader_keys.txt")


def run(*args):
    r = subprocess.run([LJ] + list(args), capture_output=True, stdin=subprocess.DEVNULL, timeout=600, cwd=os.path.dirname(LJ))
    return r.stdout.decode("utf-8", "replace").splitlines()


def june_ref(kom_stem, name):
    p = os.path.join(JL, name)
    if not os.path.exists(p):
        subprocess.run([sys.executable, os.path.join(TRUNK, "tools", "junekom.py"), "extract", kom_stem, "--names", name,
                        "--out", os.path.join(OUT, "_c3z3_extract")], check=True)
        os.makedirs(JL, exist_ok=True)
        os.replace(os.path.join(OUT, "_c3z3_extract", kom_stem + "_kom", name), p)
    m, table = luasrc.ljconv_mod()
    out = m.transcode(open(p, "rb").read(), table)[0]
    fd, tmp = tempfile.mkstemp(suffix=".lua")
    os.write(fd, out); os.close(fd)
    return tmp


def enum(path, table, optional=False):
    out = {}
    for ln in run(REG, "enum", path, table):
        p = ln.split("\t")
        if p[0] == "K":
            out[p[1]] = int(float(p[2]))
        elif p[0] == "E":
            if optional and ln.startswith("E\tno table"):
                return out
            raise SystemExit("enum %s: %s" % (path, ln))
    return out


def rows(path):
    calls, fields, errs = {}, collections.defaultdict(dict), []
    for ln in run(DUMP, path):
        p = ln.split("\t")
        if p[0] == "R":
            calls[int(p[1])] = (p[2], p[3])
        elif p[0] == "F":
            fields[int(p[1])][p[2]] = p[3]
        elif p[0] == "E":
            errs.append(ln)
    if errs:
        raise SystemExit("dump %s: %s" % (path, errs))
    return calls, fields


def registry(path, de, unused=None):
    """-> ({dungeon id: (call index, fields)}, [(index, name) unresolved], [titles]). June's own rows also name
    UNUSED_DUNGEON_ID.X (the 7 *_SECRET_COMMON), which resolves in June through that table."""
    calls, fields = rows(path)
    reg, unres, titles = {}, [], []
    for i in sorted(calls):
        meth, ser = calls[i]
        if meth != "AddDungeonData":
            titles.append(ser)
            continue
        f = fields[i]
        nm = f["dungeonID"].strip('"')
        v = None
        if nm.startswith("@DUNGEON_ID."):
            v = de.get(nm[len("@DUNGEON_ID."):])
        elif unused is not None and nm.startswith("@UNUSED_DUNGEON_ID."):
            v = unused.get(nm[len("@UNUSED_DUNGEON_ID."):])
        if v is None:
            unres.append((i, nm))
            continue
        if v in reg:  # AddDungeonData_LUA: a second row for an id fails (XEM_ERROR2), the first wins
            unres.append((i, nm + " (duplicate of row %d)" % reg[v][0]))
            continue
        reg[v] = (i, f)
    return reg, unres, titles


def ess():
    p = os.path.join(GAME, "general.ess")
    raw = open(p, "rb").read()[2:].decode("utf-16-le")
    out = {}
    for ln in raw.split("\r\n"):
        s = "".join(chr(ord(c) ^ 16) for c in ln)
        if "\t" in s:
            k, v = s.split("\t", 1)
            try:
                out[int(k)] = v
            except ValueError:
                pass
    return out


# June's client loader (AddDungeonData_LUA, unpacked_x2.exe 327e0f80): its 47 XOR-guarded key strings (all decrypted in
# june2017_re/xorstrings.tsv, func 327e0f80; 47 guard blocks in the decompile) plus its 7 plain ones. IS_HELL_MODE is not read.
JUNE_LOADER = set("""dungeonID dungeonName dataFileName requireDungeonID m_RequireItemLevel m_HardRequireItemLevel requireItemID
requireItemCount requireItemID2 requireItemCount2 requireSpirit difficulty MinAdequateMemberCount MaxAdequateMemberCount
FixMaximumDifficulty normalOnly m_fTimeLimit m_bShowNpcKillCount m_bAprilFoolsDungeon m_bNoDarkBackGroundBy60S m_bShowStageTime
m_bLevelEqualized m_bDamageEqualized m_usFixedMembers m_iLimitedPlayTimes m_iLimitedClearTimes m_MinLevel m_MaxLevel m_HardMinLevel
m_HardMaxLevel m_fExtraValue m_SuitableLevelFixValue m_eDefaultDungeonLoungeWorldID m_eDungeonType m_bRelativeMonsterLevel
m_DungeonDescription m_bHideLoadingTip m_iRequireCompleteQuestID m_TextureName m_PieceName m_Explanation m_PopUpOffsetPosX
m_PopUpOffsetPosY m_LoadingScreenFileName m_LoadingScreenFileName2 EXTRA_REQUIRE_DUNGEON_ID additionalTicket DUNGEON_CHECK_CONDITION
BOSS_DROP_ITEM AUTO_DUNGEON_ROOM_TITLE ENDING_SPEECH0 ENDING_SPEECH1""".split())
# The class tables March's loader reads inside BOSS_DROP_ITEM (plus COMMON) and ENDING_SPEECH<n> (X2DungeonManager.cpp:252-552).
MARCH_CLASSES = {"ELSWORD", "AISHA", "RENA", "RAVEN", "EVE", "CHUNG", "ARA", "ELESIS", "ADD"}


def subkeys(ser):
    """Top-level keys of a dump-serialized table '{K=..,K2={..}}'."""
    if not ser.startswith("{"):
        return []
    out, depth, inq, cur = [], 0, False, ""
    for ch in ser[1:-1]:
        if ch == '"':
            inq = not inq
        if not inq:
            if ch == "{":
                depth += 1
            elif ch == "}":
                depth -= 1
            elif ch == "," and depth == 0:
                out.append(cur.split("=", 1)[0]); cur = ""; continue
        cur += ch
    if cur:
        out.append(cur.split("=", 1)[0])
    return out


def nested_unread(f):
    out = []
    for k, v in sorted(f.items()):
        if k == "BOSS_DROP_ITEM":
            out += ["%s.%s" % (k, s) for s in subkeys(v) if s not in MARCH_CLASSES | {"COMMON"}]
        elif k.startswith("ENDING_SPEECH"):
            out += ["%s.%s" % (k, s) for s in subkeys(v) if s not in MARCH_CLASSES]
    return out


def strids(path):
    out = set()
    tmp = os.path.join(tempfile.gettempdir(), "c3z3_strdump.lua")
    open(tmp, "w").write('local f = assert(loadfile(arg[1])); local env = {}; setfenv(f, setmetatable(env, {__index=_G})); f()\n'
                         'for k, v in pairs(env) do if type(k)=="string" and k:sub(1,7)=="STR_ID_" then print(k) end end\n')
    for ln in run(tmp, path):
        out.add(ln.strip())
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--march-dd", default=os.path.join(KC, "data035_kom", "DungeonData.lua"))
    ap.add_argument("--march-de", default=os.path.join(KC, "data036_kom", "DungeonEnum.lua"))
    ap.add_argument("--out-suffix", default="")
    a = ap.parse_args()
    loader = set(l.split("\t")[0] for l in open(LOADER_KEYS, encoding="utf-8").read().splitlines() if l and not l.startswith("#"))

    jdd, jde_p = june_ref("data035", "DungeonData.lua"), june_ref("data036", "DungeonEnum.lua")
    m_de, j_de, j_un = enum(a.march_de, "DUNGEON_ID"), enum(jde_p, "DUNGEON_ID"), enum(jde_p, "UNUSED_DUNGEON_ID")
    m_un = enum(a.march_de, "UNUSED_DUNGEON_ID", optional=True)
    m_reg, m_unres, m_titles = registry(a.march_dd, m_de, m_un)
    j_reg, j_unres, j_titles = registry(jdd, j_de, j_un)
    names = ess()
    sids = strids(os.path.join(KC, "data036_kom", "StringID_def.lua"))

    # ---- DungeonEnum census
    with open(os.path.join(OUT, "dungeon_enum_census%s.tsv" % a.out_suffix), "w", encoding="utf-8", newline="\n") as w:
        w.write("name\tmarch\tjune\tjune_unused\tkind\n")
        kinds = collections.Counter()
        for n in sorted(set(m_de) | set(j_de) | set(j_un)):
            mv, jv, ju = m_de.get(n), j_de.get(n), j_un.get(n)
            if mv is not None and jv is not None:
                k = "same" if mv == jv else "VALUE-DIFFERS"
            elif mv is not None:
                k = "march-only (June: unused)" if ju is not None else "march-only"
            elif jv is not None:
                k = "june-only"
            else:
                k = "june-unused-only"
            kinds[k] += 1
            w.write("%s\t%s\t%s\t%s\t%s\n" % (n, "" if mv is None else mv, "" if jv is None else jv, "" if ju is None else ju, k))
    print("DungeonEnum: March %d, June %d (+%d unused); %s" % (len(m_de), len(j_de), len(j_un), dict(kinds)))

    # ---- DungeonData census
    shared = sorted(set(m_reg) & set(j_reg))
    rev_m = {v: k for k, v in m_de.items()}
    verdicts = collections.Counter()
    unread = collections.Counter()
    unread_nested = collections.Counter()
    missing_str = set()
    with open(os.path.join(OUT, "dungeon_registry_census%s.tsv" % a.out_suffix), "w", encoding="utf-8", newline="\n") as w:
        w.write("dungeon_id\tname\tmarch_name\tjune_name\tdiffer\tmarch_only\tjune_only\tjune_unread_june_client_reads"
                "\tjune_unread_no_client_reads\tjune_unread_class_tables\tstr_missing\tverdict\n")
        for did in shared:
            mi, mf = m_reg[did]
            ji, jf = j_reg[did]
            diff = [k for k in sorted(set(mf) & set(jf)) if mf[k] != jf[k]]
            mo = sorted(set(mf) - set(jf))
            jo = sorted(set(jf) - set(mf))
            ju = sorted(k for k in jf if k not in loader)
            for k in ju:
                unread[k] += 1
            sm = sorted(set(s.lstrip("@") for v in jf.values() for s in re.findall(r"@STR_ID_\d+", v)
                            if s.lstrip("@") not in sids or int(s[len("@STR_ID_"):]) not in names))
            missing_str |= set(sm)
            def sname(f):
                m = re.match(r"@STR_ID_(\d+)$", f.get("dungeonName", ""))
                return names.get(int(m.group(1)), "?") if m else f.get("dungeonName", "")
            if not diff and not mo and not jo:
                v = "SAME"
            elif sm:
                v = "BLOCKED: STR_ID missing in March"
            else:
                v = "JUNE"
            verdicts[v] += 1
            fmt = lambda ks, a_, b_: " | ".join("%s: %s -> %s" % (k, a_.get(k, ""), b_.get(k, "")) for k in ks)
            nu = nested_unread(jf)
            for k in nu:
                unread_nested[k.split(".")[1]] += 1
            w.write("\t".join([str(did), rev_m.get(did, "?"), sname(mf), sname(jf), fmt(diff, mf, jf),
                               " | ".join("%s=%s" % (k, mf[k]) for k in mo), " | ".join("%s=%s" % (k, jf[k]) for k in jo),
                               ",".join(k for k in ju if k in JUNE_LOADER), ",".join(k for k in ju if k not in JUNE_LOADER),
                               ",".join(nu), ",".join(sm), v]) + "\n")
    print("DungeonData: March %d rows (%d unresolved), June %d rows (%d unresolved); shared ids %d; March-only %d; June-only %d"
          % (len(m_reg), len(m_unres), len(j_reg), len(j_unres), len(shared), len(set(m_reg) - set(j_reg)), len(set(j_reg) - set(m_reg))))
    print("verdicts", dict(verdicts))
    print("June fields March's loader does not read (rows); June's client reads:",
          dict((k, n) for k, n in unread.items() if k in JUNE_LOADER))
    print("  ... no client reads (server / tool):", dict((k, n) for k, n in unread.items() if k not in JUNE_LOADER))
    print("  ... class tables March has no branch for (rows):", dict(unread_nested))
    print("STR_ID missing in March:", sorted(missing_str))
    print("room titles equal:", m_titles == j_titles)
    print("March unresolved:", m_unres)
    print("March-only ids:", sorted((d, rev_m.get(d)) for d in set(m_reg) - set(j_reg)))
    for p in (jdd, jde_p):
        os.remove(p)


if __name__ == "__main__":
    main()
