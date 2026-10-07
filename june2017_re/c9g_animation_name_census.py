"""june-effect C9g: June AnimationNameTable.lua vs March's hard-coded CX2UnitViewerUI::PlayByMotionType
(X2UnitViewerUI.cpp:4041-5129) vs the animation names present in March's and June's motion files."""
import re, os, glob, collections

TR = r"F:/elsword stuff/elsword_2014/els_2014/ElswordFiles/ready_and_built/ProjectX2_SVN/ProjectX2_SVN/source/EU_CN_US/Trunk"
MK = r"F:/elsword stuff/elsword_2014/els_2014/237311/24965799/kom_contents"
JK = r"F:/elsword stuff/elsword_2014/els_2014/237311/14JUN17/data"

MTYPE = {"UVUMT_WAIT": 0, "UVUMT_WAIT1": 1, "UVVMT_WAIT2": 2, "UVVMT_DOWN_LANDING": 3,
         "UVUMT_READY": 4, "UVUMT_LEVEL_UP": 5, "UVUMT_WAIT_DELETE": 6}
JTYPE = {"UVUMT_WAIT": 0, "UVUMT_WAIT1": 1, "UVUMT_WAIT2": 2, "UVUMT_DOWN_LANDING": 3,
         "UVUMT_READY": 4, "UVUMT_LEVEL_UP": 5, "UVUMT_HABIT": 6}

L = open(TR + "/X2Lib/X2UnitViewerUI.cpp", "rb").read().decode("cp949", "replace").splitlines()
body = L[4040:5129]
march = {}
cur_t = None
cur = []
for i, raw in enumerate(body):
    s = raw.strip()
    if s.startswith("#"):
        continue
    m = re.match(r"case CX2UnitViewerUI::(\w+):", s)
    if m:
        cur_t = MTYPE[m.group(1)]
        cur = []
        continue
    m = re.match(r"case CX2Unit::(UC_\w+):", s)
    if m:
        cur.append(m.group(1))
        s = s[m.end():].strip()
    elif s.startswith("default:"):
        cur = ["DEFAULT"]
        s = s[len("default:"):].strip()
    m = re.search(r'PlayAnim\(\s*L"([^"]+)"\s*,\s*CKTDGXSkinAnim::(\w+)', s)
    if m and cur_t is not None:
        for c in cur:
            march.setdefault((c, cur_t), (m.group(1), m.group(2), 4041 + i))
    if re.search(r"\bbreak\s*;", s):
        cur = []

june = collections.OrderedDict()
for l in open(TR + "/june_src_fixed/data036_kom/AnimationNameTable.src.lua", encoding="utf-8-sig"):
    m = re.match(r'g_pUnitManager:AddAnimationName\(UNIT_CLASS\.(\w+), UNIT_VIEWER_UI_MOTION_TYPE\.(\w+), "([^"]+)"\)', l.strip())
    if m:
        june.setdefault((m.group(1), JTYPE[m.group(2)]), m.group(3))


def motion_names(root):
    names = set()
    for f in glob.glob(root + "/data*_kom/Motion_*.[xX]"):
        d = open(f, "rb").read()
        for t in re.findall(rb"[A-Za-z][A-Za-z0-9_]{3,}", d):
            names.add(t.decode())
    return names


mnames = motion_names(MK)
jnames = motion_names(JK)
hdr = open(TR + "/X2Lib/X2Unit.h", "rb").read().decode("cp949", "replace")


def has(name, pool):
    # names in KSM files can carry up to three trailing junk bytes: accept a prefix hit too
    if name in pool:
        return "yes"
    if any(p.startswith(name) and len(p) - len(name) <= 3 for p in pool):
        return "yes(prefix)"
    low = name.lower()
    for p in pool:
        if p.lower().startswith(low) and len(p) - len(name) <= 3:
            return "case:" + p
    return "no"


JPT = lambda t: "XAP_LOOP" if t <= 2 else "XAP_ONE_WAIT"
out = ["class\ttype\tjune_name\tmarch_name\tmarch_line\tjune_name_in_march_motion\tjune_name_in_june_motion\tmarch_playtype\tjune_playtype\tverdict"]
cnt = collections.Counter()
for (c, t), jn in june.items():
    mm = march.get((c, t))
    mname, mpt, mline = (mm if mm else ("", "", ""))
    inm, inj = has(jn, mnames), has(jn, jnames)
    if not re.search(r"\b%s\b" % c, hdr):
        v = "inert: class not in March"
    elif mname == jn:
        v = "same name"
    elif not mm:
        v = "new row (March used the default)" if inm.startswith("yes") else "new row, motion missing in March"
    elif inm.startswith("yes"):
        v = "renamed, June motion present in March"
    else:
        v = "June renamed the motion: adopt June's motion file"
    if mm and mpt and mpt != JPT(t) and not v.startswith("inert"):
        v += "; play type changes"
    cnt[v] += 1
    out.append("\t".join([c, str(t), jn, mname, str(mline), inm, inj, mpt, JPT(t), v]))
for (c, t), mm in march.items():
    if c != "DEFAULT" and (c, t) not in june:
        v = "March-only row (June uses the default)"
        cnt[v] += 1
        out.append("\t".join([c, str(t), "", mm[0], str(mm[2]), "", "", mm[1], JPT(t), v]))
for (c, t), mm in sorted(march.items()):
    if c == "DEFAULT":
        out.append("\t".join(["DEFAULT", str(t), "", mm[0], str(mm[2]), "", "", mm[1], JPT(t), "March default (June 32375ee0 default is the same name)"]))
open(TR + "/june2017_re/c9g_animation_name_census.tsv", "w", encoding="utf-8", newline="\n").write("\n".join(out) + "\n")
print("june rows", len(june), "march rows", len(march))
for k, v in cnt.most_common():
    print(v, k)
