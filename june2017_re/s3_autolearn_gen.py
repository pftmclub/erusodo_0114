"""June skill system core, Phase S3: generate the auto-learn table and June's SP-by-level table.

The v17 save migration runs from CX2OfflineDB::Open(), before CX2StateLoading loads the skill tree (the same reason v16's
X2OfflineCap99Exp.h exists), so it cannot read g_pData->GetSkillTree(). This script runs June's own files under the vendored
luajit, with a recording stub for g_pSkillTree, and writes what the migration and the save tools need:

  X2Lib/Offline/X2OfflineJuneSkillAutoLearn.h   GENERATED, for the migration and the emulator's seed
  tools/data/june_skill_autolearn.tsv           the same rows, for tools/createchar.py and elsdb.py setchar
  tools/data/june_skill_init_sp.tsv             GetCalcInitSkillPoint( level ) for 1..99

Inputs (nothing is retyped):
  March's core Enum.lua from kom_contents/data036_kom (what the client loads: names -> values, True = 1 / False = 0)
  june_src_fixed/data036_kom/SkillData.src.lua            (CalcLevelUpIncreaseSkillPoint; T1-equal to the shipped bytecode)
  june_src_fixed/data036_kom/NewSkillTempletVer2.src.lua  (templets and tree rows; S1: --check clean, T1)

The auto-learn rule is the stand-in the user accepted on 2026-10-18 (S3 Q1; June's own default list is empty):
  a tree row of the class whose templet costs 0 SP to learn, has master level 1, is not born sealed, is not an "[Enhanced]"
  skill (no m_DeleteSkillIDByUpgrade), and whose m_RequireCharacterLevel[1] (absent = no gate) is <= the unit's level.
Parser rules copied from the C++ loader: a templet id seen twice keeps the first (June 32216320); a tree row overwrites an
earlier row of the same (class, id) (mapSkillTreeTemplet[ id ] = row); a tree row with no templet is skipped.
SP: June GetCalcInitSkillPoint 32205790 = sum over 2..L of lua_tointeger( CalcLevelUpIncreaseSkillPoint( l ) ) (truncation).

    python june2017_re/s3_autolearn_gen.py [--check]
"""
import os
import subprocess
import sys

TRUNK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LUAJIT = os.path.join(TRUNK, "elstools", "vendor", "luajit", "luajit.exe")
KOM_ROOT = os.environ.get("X2_KOM_CONTENTS", r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents")
ENUM = os.path.join(KOM_ROOT, "data036_kom", "Enum.lua")
SKILLDATA = os.path.join(TRUNK, "june_src_fixed", "data036_kom", "SkillData.src.lua")
TEMPLET = os.path.join(TRUNK, "june_src_fixed", "data036_kom", "NewSkillTempletVer2.src.lua")
OUT_H = os.path.join(TRUNK, "X2Lib", "Offline", "X2OfflineJuneSkillAutoLearn.h")
OUT_TSV = os.path.join(TRUNK, "tools", "data", "june_skill_autolearn.tsv")
OUT_SP = os.path.join(TRUNK, "tools", "data", "june_skill_init_sp.tsv")
MAX_LEVEL = 99

DRIVER = r"""
local function slurp(p)
  local f = assert(io.open(p, 'rb')); local s = f:read('*a'); f:close()
  if s:sub(1, 3) == '\239\187\191' then s = s:sub(4) end
  return s
end
local templets, rows = {}, {}
local rec = {}
function rec.AddSkillTemplet_LUA(self, t) templets[#templets + 1] = t end
function rec.AddSkillTreeTemplet_LUA(self, t) rows[#rows + 1] = t end
local sink = setmetatable({}, { __index = function(_, k) return rec[k] or function() end end })
g_pSkillTree = sink
setmetatable(_G, { __index = function(_, k) if type(k) == 'string' and k:sub(1, 3) == 'g_p' then return sink end end })
assert(loadstring(slurp(ENUM_PATH), 'Enum.lua'))()
assert(loadstring(slurp(SKILLDATA_PATH), 'SkillData.lua'))()
assert(loadstring(slurp(TEMPLET_PATH), 'NewSkillTempletVer2.lua'))()
local function num(v) if type(v) == 'number' then return v elseif v == true then return 1 else return 0 end end
for _, t in ipairs(templets) do
  local req = t.m_RequireCharacterLevel
  local req1 = (type(req) == 'table' and req[1] ~= nil) and num(req[1]) or -1
  io.write(string.format('T\t%d\t%d\t%d\t%d\t%d\t%d\n', num(t.m_eID), num(t.m_eType), num(t.m_iRequireLearnSkillPoint),
    num(t.m_bBornSealed), num(t.m_DeleteSkillIDByUpgrade), req1))
end
for _, r in ipairs(rows) do
  io.write(string.format('R\t%d\t%d\t%d\t%d\t%d\n', num(r.m_eUnitClass), num(r.m_iSkillID), num(r.m_iMasterLevel),
    num(r.m_iRowIndex), num(r.m_iColumnIndex)))
end
for l = 2, MAX_LEVEL do
  local v = CalcLevelUpIncreaseSkillPoint(l)
  local i = (v >= 0) and math.floor(v) or math.ceil(v)
  io.write(string.format('S\t%d\t%d\n', l, i))
end
"""

# CX2SkillTree::SKILL_TYPE values that go on the bar (CX2OfflineSkill::SeedDefaultSkills: ST_ACTIVE, ST_SPECIAL_ACTIVE,
# ST_BUFF). Resolved in C++ by name, so the header only carries the type number.


def run_lua():
    def lstr(p):
        return "[==[" + p + "]==]"
    code = ("ENUM_PATH = %s\nSKILLDATA_PATH = %s\nTEMPLET_PATH = %s\nMAX_LEVEL = %d\n" %
            (lstr(ENUM), lstr(SKILLDATA), lstr(TEMPLET), MAX_LEVEL)) + DRIVER
    p = subprocess.run([LUAJIT, "-"], input=code.encode("utf-8"), capture_output=True, cwd=os.path.dirname(LUAJIT))
    if p.returncode != 0:
        sys.exit("luajit failed: " + p.stderr.decode("utf-8", "replace"))
    return p.stdout.decode("ascii").splitlines()


def build():
    lines = run_lua()
    templet = {}
    rows = []          # file order, later rows of the same (class, id) replace earlier ones in place
    row_at = {}
    sp_step = {}
    n_templet_lines = 0
    for ln in lines:
        f = ln.split("\t")
        if f[0] == "T":
            n_templet_lines += 1
            tid = int(f[1])
            if tid not in templet:      # duplicate id keeps the first
                templet[tid] = dict(type=int(f[2]), learn=int(f[3]), sealed=int(f[4]), upg=int(f[5]), req1=int(f[6]))
        elif f[0] == "R":
            key = (int(f[1]), int(f[2]))
            row = dict(cls=key[0], sid=key[1], master=int(f[3]), r=int(f[4]), c=int(f[5]))
            if key in row_at:
                rows[row_at[key]] = row
            else:
                row_at[key] = len(rows)
                rows.append(row)
        elif f[0] == "S":
            sp_step[int(f[1])] = int(f[2])
    init_sp = [0] * (MAX_LEVEL + 1)
    for lv in range(2, MAX_LEVEL + 1):
        init_sp[lv] = init_sp[lv - 1] + sp_step[lv]
    auto = []
    for row in rows:
        t = templet.get(row["sid"])
        if t is None:
            continue
        if t["learn"] != 0 or row["master"] != 1 or t["sealed"] != 0 or t["upg"] != 0:
            continue
        auto.append((row["cls"], row["sid"], max(t["req1"], 0), t["type"], row["r"], row["c"]))
    stats = dict(templet_lines=n_templet_lines, templets=len(templet), rows=len(rows),
                 rows_no_templet=sum(1 for r in rows if r["sid"] not in templet), auto=len(auto),
                 classes=len(set(a[0] for a in auto)))
    return auto, init_sp, stats


def render(auto, init_sp, stats):
    h = []
    h.append("#ifdef SERV_IRUHADEV_JUNE_SKILL_REFORM\r\n")
    h.append("#pragma once\r\n")
    h.append("\r\n")
    h.append("//////////////////////////////////////////////////////////////////////////\r\n")
    h.append("// Author: Iruha\r\n")
    h.append("// Date: 2026-10-18\r\n")
    h.append("// Description: GENERATED by june2017_re/s3_autolearn_gen.py - do not hand-edit. June skill system core, Phase S3.\r\n")
    h.append("//              The v17 migration runs from CX2OfflineDB::Open(), before the skill tree loads, so June's data it needs\r\n")
    h.append("//              is carried here, read out of June's own SkillData / NewSkillTempletVer2 (june_src_fixed, T1-equal to the\r\n")
    h.append("//              shipped bytecode) under March's core Enum.lua. The emulator's seed uses the same rows, and\r\n")
    h.append("//              SERV_IRUHADEV_JUNE_SKILL_DEBUG checks them against the loaded tree ([JUNE_SKILL] MIG autolearn check).\r\n")
    h.append("//              %d templets (%d lines, first wins), %d tree rows (%d without a templet); %d auto-learn rows in %d classes.\r\n"
             % (stats["templets"], stats["templet_lines"], stats["rows"], stats["rows_no_templet"], stats["auto"], stats["classes"]))
    h.append("//////////////////////////////////////////////////////////////////////////\r\n")
    h.append("\r\n")
    h.append("\r\n")
    h.append("namespace X2OfflineJuneSkillAutoLearn\r\n")
    h.append("{\r\n")
    h.append("\t/// June GetCalcInitSkillPoint 32205790 for levels 0..%d: sum over 2..L of CalcLevelUpIncreaseSkillPoint( l ) (l / 10 + 4).\r\n" % MAX_LEVEL)
    h.append("\tstatic const int MAX_LEVEL = %d;\r\n" % MAX_LEVEL)
    h.append("\tstatic const int INIT_SKILL_POINT[ MAX_LEVEL + 1 ] =\r\n")
    h.append("\t{\r\n")
    for i in range(0, MAX_LEVEL + 1, 10):
        chunk = init_sp[i:i + 10]
        h.append("\t\t" + ", ".join("%d" % v for v in chunk) + ("," if i + 10 <= MAX_LEVEL else "") + "\t// %d..\r\n" % i)
    h.append("\t};\r\n")
    h.append("\r\n")
    h.append("\t/// The S3 Q1 stand-in: a 0-SP, master-level-1, not born-sealed, non-[Enhanced] tree row. Tree-file order.\r\n")
    h.append("\tstruct KRow\r\n")
    h.append("\t{\r\n")
    h.append("\t\tint\tm_iUnitClass;\r\n")
    h.append("\t\tint\tm_iSkillID;\r\n")
    h.append("\t\tint\tm_iRequireLevel;\t///< m_RequireCharacterLevel[ 0 ], 0 when the templet has none\r\n")
    h.append("\t\tint\tm_iSkillType;\t\t///< m_eType ( CX2SkillTree::SKILL_TYPE )\r\n")
    h.append("\t};\r\n")
    h.append("\r\n")
    h.append("\tstatic const KRow ROWS[] =\r\n")
    h.append("\t{\r\n")
    for i, a in enumerate(auto):
        h.append("\t\t{ %d, %d, %d, %d }%s\r\n" % (a[0], a[1], a[2], a[3], "," if i < len(auto) - 1 else ""))
    h.append("\t};\r\n")
    h.append("\tstatic const int ROW_COUNT = sizeof( ROWS ) / sizeof( ROWS[0] );\r\n")
    h.append("}\r\n")
    h.append("\r\n")
    h.append("#endif SERV_IRUHADEV_JUNE_SKILL_REFORM\r\n")
    tsv = ["unit_class\tskill_id\trequire_level\tskill_type\trow\tcolumn\n"]
    tsv += ["%d\t%d\t%d\t%d\t%d\t%d\n" % a for a in auto]
    sp = ["level\tinit_sp\n"] + ["%d\t%d\n" % (lv, init_sp[lv]) for lv in range(1, MAX_LEVEL + 1)]
    return "".join(h), "".join(tsv), "".join(sp)


def main():
    check = "--check" in sys.argv[1:]
    auto, init_sp, stats = build()
    outs = dict(zip((OUT_H, OUT_TSV, OUT_SP), render(auto, init_sp, stats)))
    bad = 0
    for path, text in outs.items():
        data = text.encode("ascii")
        if check:
            same = os.path.exists(path) and open(path, "rb").read() == data
            print("%s %s" % ("SAME" if same else "DIFF", os.path.relpath(path, TRUNK)))
            bad += 0 if same else 1
        else:
            with open(path, "wb") as f:
                f.write(data)
            print("wrote %s (%d B)" % (os.path.relpath(path, TRUNK), len(data)))
    print("templets %(templets)d (%(templet_lines)d lines), tree rows %(rows)d (%(rows_no_templet)d without a templet), "
          "auto-learn %(auto)d rows in %(classes)d classes" % stats)
    print("init SP: L2 %d, L10 %d, L80 %d, L99 %d" % (init_sp[2], init_sp[10], init_sp[80], init_sp[99]))
    if check and bad:
        sys.exit(1)


if __name__ == "__main__":
    main()
