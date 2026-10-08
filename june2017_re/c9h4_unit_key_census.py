"""Effect C9h-4: count June-new unit-state keys over March's and June's script corpora.

    python june2017_re/c9h4_unit_key_census.py > june2017_re/c9h4_unit_key_census.tsv

March corpus: kom_contents/data*_kom/*.lua compiled LuaJIT (not .src.lua: only 320 files have one).
Key names are plain string constants there. Header byte 3 tells the build: 0x81 = March, 0x82 = a June
file staged into the March tree (counted as 'june-staged').
June corpus: june_src_fixed/data*_kom/*.src.lua. Family = RidingPet_* / Pet* / NPC-or-unit (rest).
Counts files, not occurrences. Key match is a Lua table key at the start of a field
(`KEY =` or `KEY0 =` ..), so mentions in comments/strings are not counted.
"""
import os, re, sys, glob, hashlib
MARCH = r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents"
JUNE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "june_src_fixed")
KEYS = ["SOUND_PLAY_ALL_OPTION", "SOUND_PLAY_EX", "STATE_SOUND_PLAY", "CUSTOM_STATE_DELETE_SOUND",
        "DELETE_CONDITION", "UPDOWN_CANCEL_AFTER", "DISABLE_GRAVITY_TIME_START", "CAN_PUSH_UNIT_TIME",
        "CAN_PASS_UNIT_TIME", "DELETE_EFFECT_SET_OFF_RIDING", "DELETE_EFFECT_SET_ON_SKILL_STATE_END",
        "DELETE_EFFECT_SET_ON_STAGE_END", "SET_HYPER_EFFECT_SET_CHECK_HYPER_STATE",
        "EFFECT_SET_LIST_EX_APPLIED_SKILL", "EFFECT_SET_LIST_ULTIMATE_EX", "EFFECT_SET_LIST_ULTIMATE_EX_APPLIED_SKILL",
        "EFFECT_SET_LIST_FINAL", "EFFECT_SET_LIST_FINAL_ULTIMATE", "EFFECT_SET_LIST_EX_TRANS_1",
        "EFFECT_SET_LIST_EX_TRANS_2", "EFFECT_SET_LIST_EX_NO_TRANS", "APPLY_RIDINGPET_ANIM_MOVE",
        "TARGET_RANGE", "LIMIT_Z_DEPTH", "DEGREE_RANGE", "VALUE_POS", r"SOUND_PLAY\d+_[A-Z]{2}"]
def fam(n):
    b = os.path.basename(n).lower()
    if b.startswith("ridingpet_"): return "ridingpet"
    if b.startswith("pet"): return "pet"
    return "unit/npc/other"
def scan_bc(root):
    out = {}
    for p in glob.glob(os.path.join(root, "data*_kom", "*.lua")):
        if p.endswith(".src.lua"): continue
        try: d = open(p, "rb").read()
        except OSError: continue
        if d[:3] != b"KL": continue
        out[p] = ("june-staged" if d[3] == 0x82 else "march", d)
    return out
def scan(root):
    out = {}
    for p in glob.glob(os.path.join(root, "data*_kom", "*.src.lua")):
        try: t = open(p, encoding="utf-8", errors="replace").read()
        except OSError: continue
        out[p] = t
    return out
march, june = scan_bc(MARCH), scan(JUNE)
print("key\tcorpus\tfamily\tfiles\texamples")
for k in KEYS:
    pat = re.compile(r"(?m)^\s*(?:\[\")?%s(?:\d*)(?:\"\])?\s*=" % k)
    bpat = re.compile((r"(?<![A-Z_])%s\d*(?![A-Z_])" % k).encode())
    for cname, corp in (("march", march), ("june", june)):
        rows = {}
        for p, t in corp.items():
            if cname == "march":
                c, d = t
                if not bpat.search(d): continue
            else:
                c = cname
                if not pat.search(t): continue
            rows.setdefault((c, fam(p)), []).append(os.path.basename(p).replace(".src.lua", "").replace(".lua", ""))
        for (c, f), names in sorted(rows.items()):
            print(f"{k}\t{c}\t{f}\t{len(names)}\t{', '.join(sorted(names)[:6])}")
