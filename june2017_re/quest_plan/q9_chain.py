# Q9: write q9_chain.tsv = June's quest chain 22100-22340 (epics + side quests) with sub-quests and dungeon ids.
# usage: python q9_chain.py      (reads june_src_fixed/data036_kom, KncWX2Server/Common/Enum/DungeonEnum.h)
import sys, os, re
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import qlib
TRUNK = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
J = os.path.join(TRUNK, "june_src_fixed", "data036_kom")

def r(v): return v[1].split(".")[-1] if isinstance(v, tuple) else v
def ints(t):
    if isinstance(t, qlib.T): return [x for x in t.arr if isinstance(x, int) and x != 0]
    return [t] if isinstance(t, int) and t else []
def tbl(args):
    for a in args:
        if isinstance(a, qlib.T): return a

dn = {}
for l in open(os.path.join(TRUNK, "KncWX2Server", "Common", "Enum", "DungeonEnum.h"), encoding="cp949", errors="replace"):
    m = re.match(r"\s*(DI_\w+)\s*=\s*(\d+)", l)
    if m: dn[m.group(1)] = int(m.group(2))

q = {}; s = {}
for f in ("FieldQuest", "FieldSubQuest"):
    for name, args in qlib.parse_june(os.path.join(J, f + ".src.lua")):
        t = tbl(args)
        if t is None: continue
        if name.endswith("QuestTemplet_LUA") and "Sub" not in name: q[t.get("m_iID")] = t
        elif name.endswith("SubQuestTemplet_LUA"): s[t.get("m_iID")] = t

out = ["quest_id\ttype\tplay_level\tstart_npc\tend_npc\tafter\tsub_ids\tsub_types\tsub_need\tdungeon_ids\ttitle"]
for qid in sorted(q):
    if not (22100 <= qid <= 22340): continue
    t = q[qid]; subs = ints(t.get("m_vecSubQuest")); types = []; dgs = []; need = []
    for sid in subs:
        st = s.get(sid)
        types.append(r(st.get("m_eClearType"))[4:] if st else "?")
        cc = st.get("m_ClearCondition") if st else None
        n = 1
        if isinstance(cc, qlib.T):
            for k in ("m_iDungeonClearCount", "m_iKillNum", "m_iCollectionItemNum"):
                if isinstance(cc.get(k), int) and cc.get(k) > 0: n = cc.get(k)
        need.append(n)
        if isinstance(cc, qlib.T):
            for d in ints(cc.get("m_eDungeonID")):
                if d not in dgs: dgs.append(d)
    out.append("\t".join(str(x) for x in (qid, r(t.get("m_eQuestType"))[3:], t.get("m_iPlayLevel"), r(t.get("m_eStartNPCID"))[4:],
        r(t.get("m_eEndNPCID"))[4:], ",".join(map(str, ints(t.get("m_iAfterQuestID")))), ",".join(map(str, subs)), ",".join(types), ",".join(map(str, need)),
        ",".join(map(str, dgs)), t.get("m_wstrTitle"))))
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), "q9_chain.tsv")
open(p, "w", encoding="utf-8", newline="\n").write("\n".join(out) + "\n")
print("wrote", p, len(out) - 1, "quests")
