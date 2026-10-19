# Q9: put one unit's Lanox / Atlas / Elysion epic chain (quests 22100-22340) at a chosen point, through tools/elsdb.py x
# (backup first, one transaction, integrity check; refuses while X2_offline.exe runs).
#
#   python june2017_re/quest_plan/q9_jump.py <unit> <quest_id> [--open-by-play] [--village ID] [--cleared] [--dry-run | --print-sql]
#
# Result: every chain epic BELOW <quest_id> is complete; <quest_id> is ongoing with its sub-quests at 0 (or, with --open-by-play,
# not ongoing at all (with --cleared its sub-quests are already done: clear_data = the needed count, success = 1, so the Complete button
# works at once; the same trick Q5b / Q8 used by hand), so the village / dungeon / after-quest sweep of the emulator has to open it); all side quests of the range
# are removed; the normal dungeons the finished epics used are marked cleared (the emulator's EXTRA_REQUIRE_DUNGEON_ID gate).
# The unit's level is not touched (use `elsdb.py setchar <unit> --level N`, which also sets the exp). --village sets unit.last_pos (20008 = Lanox village, 1008 = Atlas Station rest area, 20009 = Elysion village).
# The range 22100-22340 of THIS unit is rewritten: only run it on a test unit.
import sys, os, csv, time, subprocess, tempfile, argparse

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.abspath(os.path.join(HERE, "..", ".."))

ap = argparse.ArgumentParser()
ap.add_argument("unit", type=int); ap.add_argument("quest", type=int)
ap.add_argument("--open-by-play", action="store_true")
ap.add_argument("--village", type=int); ap.add_argument("--dry-run", action="store_true"); ap.add_argument("--print-sql", action="store_true"); ap.add_argument("--cleared", action="store_true")
a = ap.parse_args()

rows = list(csv.DictReader(open(os.path.join(HERE, "q9_chain.tsv"), encoding="utf-8"), delimiter="\t"))
by = {int(r["quest_id"]): r for r in rows}
if a.quest not in by or by[a.quest]["type"] != "EPIC":
    sys.exit("quest %d is not an epic of q9_chain.tsv (epics: %s)" % (a.quest, ",".join(str(k) for k in sorted(by) if by[k]["type"] == "EPIC")))
done = [k for k in sorted(by) if by[k]["type"] == "EPIC" and k < a.quest]
now = int(time.time()); U = a.unit
sql = ["DELETE FROM unit_subquest WHERE unit_uid=%d AND quest_id BETWEEN 22100 AND 22340;" % U,
       "DELETE FROM unit_quest WHERE unit_uid=%d AND quest_id BETWEEN 22100 AND 22340;" % U,
       "DELETE FROM unit_quest_complete WHERE unit_uid=%d AND quest_id BETWEEN 22100 AND 22340;" % U]
for k in done:
    sql.append("INSERT INTO unit_quest_complete(unit_uid,quest_id,complete_count,complete_date) VALUES(%d,%d,1,%d);" % (U, k, now))
if not a.open_by_play:
    sql.append("INSERT INTO unit_quest(unit_uid,quest_id,state,progress) VALUES(%d,%d,0,NULL);" % (U, a.quest))
    for sid, need in zip(by[a.quest]["sub_ids"].split(","), by[a.quest]["sub_need"].split(",")):
        sql.append("INSERT INTO unit_subquest(unit_uid,quest_id,sub_id,clear_data,success) VALUES(%d,%d,%s,%s,%d);" % (U, a.quest, sid, need if a.cleared else 0, 1 if a.cleared else 0))
normals = set()
for k in done:
    for d in by[k]["dungeon_ids"].split(","):
        if d: normals.add(int(d) - int(d) % 10)
for d in sorted(normals):
    sql.append("INSERT OR IGNORE INTO unit_dungeon(unit_uid,dungeon_id,clear_count,best_rank,max_score,clear_date) VALUES(%d,%d,1,0,0,%d);" % (U, d, now))
sets = []
if a.village: sets.append("last_pos=%d" % a.village)
if sets: sql.append("UPDATE unit SET %s WHERE unit_uid=%d;" % (",".join(sets), U))

if a.print_sql:
    print("\n".join(sql)); sys.exit(0)
f = tempfile.NamedTemporaryFile("w", suffix=".sql", delete=False, encoding="utf-8"); f.write("\n".join(sql) + "\n"); f.close()
print("unit %d: %d epics complete (22100..%d), %s, %d normal dungeons marked cleared%s" % (U, len(done), done[-1] if done else 0,
      ("quest %d left for the game to open" % a.quest) if a.open_by_play else ("quest %d ongoing, %d sub-quest(s)" % (a.quest, len(by[a.quest]["sub_ids"].split(",")))),
      len(normals), "; " + ",".join(sets) if sets else ""))
cmd = [sys.executable, os.path.join(TRUNK, "tools", "elsdb.py"), "x", "--file", f.name] + (["--dry-run"] if a.dry_run else [])
rc = subprocess.call(cmd); os.unlink(f.name); sys.exit(rc)
