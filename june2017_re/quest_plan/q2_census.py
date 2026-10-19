# Q2 step 1: old-save census against June's quest data (JUNE_QUEST_SYSTEM_PHASES.md "Phase Q2").
# Reads a COPY of els_db.sql (sqlite backup API into the scratch path given), March's data036 bytecode
# and June's june_src_fixed sources. Writes q2_census.tsv (one row per unit x quest) and prints the counts.
# usage: python q2_census.py <copy_of_save.sqlite> [out.tsv]
import sys, os, sqlite3, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import qlib

TRUNK = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
MARCH = r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents_bytecode\data036_kom"  # March 2014 originals, snapshotted before the Q2 flip
JUNE = os.path.join(TRUNK, "june_src_fixed", "data036_kom")
DI_RANOX_FINALHALL_NORMAL = 33320          # KncWX2Server/Common/Enum/DungeonEnum.h:506
LANOX_CHAIN = range(22160, 22218)          # Atlas decision 6

def r(v):  # ('R','QUEST_TYPE.QT_EPIC') -> 'QT_EPIC'
    return v[1].split(".")[-1] if isinstance(v, tuple) else v

def ints(t):
    if t is None: return []
    if isinstance(t, qlib.T): return [x for x in t.arr if isinstance(x, int) and x != 0]
    return [t] if isinstance(t, int) and t != 0 else []

def tbl(args):
    for a in args:
        if isinstance(a, qlib.T): return a
    return None

def load(calls, fname):
    q = {}; s = {}
    for name, args in calls:
        t = tbl(args)
        if t is None: continue
        if name.endswith("QuestTemplet_LUA") and "Sub" not in name:
            q[t.get("m_iID")] = t
        elif name.endswith("SubQuestTemplet_LUA"):
            s[t.get("m_iID")] = t
    return q, s

def march():
    q, s = {}, {}
    for f in ("FieldQuest.lua", "FieldSubQuest.lua", "AccountQuest.lua"):
        calls, _ = qlib.parse_march(os.path.join(MARCH, f))
        a, b = load(calls, f); q.update(a); s.update(b)
    return q, s

def june():
    q, s = {}, {}
    for f in ("FieldQuest", "FieldSubQuest", "AccountQuest"):
        a, b = load(qlib.parse_june(os.path.join(JUNE, f + ".src.lua")), f); q.update(a); s.update(b)
    return q, s

def subsig(t):
    if t is None: return None
    cc = t.get("m_ClearCondition")
    return (r(t.get("m_eClearType")), qlib.show(cc) if cc is not None else "")

def title(t): return (t.get("m_wstrTitle") or "").strip()

def main():
    src = sys.argv[1]; out = sys.argv[2] if len(sys.argv) > 2 else os.path.join(os.path.dirname(os.path.abspath(__file__)), "q2_census.tsv")
    mq, ms = march(); jq, js = june()
    jtitle = collections.defaultdict(list)
    for i, t in jq.items(): jtitle[title(t)].append(i)
    db = sqlite3.connect(src)
    units = {u: (n, c, l) for u, n, c, l in db.execute("select unit_uid, nickname, unit_class, level from unit")}
    prog = collections.defaultdict(dict)
    for u, qid in db.execute("select unit_uid, quest_id from unit_quest"): prog[u][qid] = []
    for u, qid, sid, cd, ok in db.execute("select unit_uid, quest_id, sub_id, clear_data, success from unit_subquest"):
        prog[u].setdefault(qid, []).append((sid, cd, ok))
    done = collections.defaultdict(dict)
    for u, qid, n, d in db.execute("select unit_uid, quest_id, complete_count, complete_date from unit_quest_complete"): done[u][qid] = (n, d)
    lanox = {u for (u,) in db.execute("select unit_uid from unit_dungeon where dungeon_id=? and clear_count>0", (DI_RANOX_FINALHALL_NORMAL,))}

    rows = []; cnt = collections.Counter(); per_unit = collections.defaultdict(collections.Counter)
    def add(u, cat, qid, detail):
        rows.append((u, units.get(u, ("?",))[0], cat, qid, detail)); cnt[cat] += 1; per_unit[u][cat] += 1

    for u in sorted(set(prog) | set(done) | lanox):
        for qid, subs in sorted(prog[u].items()):
            m, j = mq.get(qid), jq.get(qid)
            mt = m or j
            info = "%s '%s'" % (r(mt.get("m_eQuestType")) if mt else "?", title(mt) if mt else "?")
            if j is None:
                cand = [x for x in jtitle.get(title(m), []) if x != qid] if m else []
                add(u, "P_NO_JUNE", qid, info + (" june_same_title=%s" % cand if cand else ""))
                continue
            mv, jv = ints(m.get("m_vecSubQuest")) if m else [], ints(j.get("m_vecSubQuest"))
            changed = []
            if mv != jv: changed.append("subs march=%s june=%s" % (mv, jv))
            for sid, cd, ok in subs:
                if sid not in jv: changed.append("saved sub %d (cd=%d ok=%d) not in June" % (sid, cd, ok))
                elif subsig(ms.get(sid)) != subsig(js.get(sid)): changed.append("sub %d condition differs (%s -> %s)" % (sid, subsig(ms.get(sid))[0] if ms.get(sid) else "-", subsig(js.get(sid))[0]))
            prog_any = any(cd or ok for _, cd, ok in subs)
            if changed: add(u, "P_SUBS_CHANGED", qid, info + (" [has progress]" if prog_any else " [no progress]") + " ; " + " ; ".join(changed))
            else: add(u, "P_KEEP", qid, info)
        for qid in sorted(done[u]):
            m, j = mq.get(qid), jq.get(qid)
            mt = m or j
            info = "%s '%s'" % (r(mt.get("m_eQuestType")) if mt else "?", title(mt) if mt else "?")
            if j is None:
                cand = [x for x in jtitle.get(title(m), []) if x != qid] if m else []
                add(u, "C_NO_JUNE", qid, info + (" june_same_title=%s" % cand if cand else ""))
            else: add(u, "C_KEEP", qid, info)
        # epic position: in-progress epics (March view) and the June epic frontier from the completed set
        ep = [q for q in prog[u] if (mq.get(q) or jq.get(q)) is not None and r((mq.get(q) or jq.get(q)).get("m_eQuestType")) == "QT_EPIC"]
        cep = [q for q in done[u] if (mq.get(q) or jq.get(q)) is not None and r((mq.get(q) or jq.get(q)).get("m_eQuestType")) == "QT_EPIC"]
        add(u, "EPIC", ",".join(map(str, sorted(ep))) or "-", "epics completed=%d (in June %d) ; in-progress %s" % (
            len(cep), sum(1 for q in cep if q in jq), ["%d%s" % (q, "" if q in jq else "*no-june") for q in sorted(ep)]))
        if u in lanox:
            have = sum(1 for q in LANOX_CHAIN if q in done[u]); injune = sum(1 for q in LANOX_CHAIN if q in jq)
            add(u, "LANOX_CLEARED", DI_RANOX_FINALHALL_NORMAL, "chain 22160-22217: %d June quests, %d already complete" % (injune, have))

    with open(out, "w", encoding="utf-8", newline="\n") as f:
        f.write("unit_uid\tnick\tcategory\tquest_id\tdetail\n")
        for row in rows: f.write("\t".join(map(str, row)) + "\n")
    print("march quests %d subs %d ; june quests %d subs %d" % (len(mq), len(ms), len(jq), len(js)))
    print("distinct save ids: %d, not in June: %d" % (
        len({q for u in prog for q in prog[u]} | {q for u in done for q in done[u]}),
        len({q for u in prog for q in prog[u] if q not in jq} | {q for u in done for q in done[u] if q not in jq})))
    for k in ("P_KEEP", "P_SUBS_CHANGED", "P_NO_JUNE", "C_KEEP", "C_NO_JUNE", "EPIC", "LANOX_CLEARED"): print("%-15s %d" % (k, cnt[k]))
    print("wrote", out)

if __name__ == "__main__":
    main()
