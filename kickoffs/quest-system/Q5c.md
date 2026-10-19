# June quest system - Phase Q5c - UPDATE_QUEST_NOT: June's quest-window handler vs March's state handler

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (Ghidra first), then type `/x2-phase quest-system Q5c` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q4e (built), Q5a review |
| Flag | one `SERV_IRUHADEV_JUNE_QUEST_*` flag per logical difference found, or none if equal |
| Log tag | `QUEST` (`python tools/logs.py --grep "QUEST "`) |
| Review | none unless the phase ports more than one difference |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Rules" "Phase Q5c - UPDATE_QUEST_NOT: June's quest-window handler vs March's state handler" "Q4e - Done (2026-10-19, Opus 5.5 high)"
grep -n "323e08c0\|31f09b50\|0x27c" june2017_re/quest_plan/q0b_ui_map.tsv june2017_re/funcmap.tsv
```

## Task

Q5a kept `Handler_EGS_UPDATE_QUEST_NOT` in `CX2State` (March `X2State.cpp:4859`) and called June's quest-window copy `323e08c0` "behaviour equal".
Q4e found it is not equal at the visit cases and ported that one difference (`SERV_IRUHADEV_JUNE_QUEST_VISIT_FLUSH`).

1. Diff `323e08c0` (UI dispatcher `323e5fb0` case 0x27c) line by line against March's handler as it now stands (Q3 / Q4 / Q4e flags on): the
   deserialize, `CheckUpdateSubQuestInst` `3232e310`, `UpdateUnitQuest` `32331a30`, the tutorial-guide part, the out-of-game switch (SQT 0xd-0x12 /
   0x1c / 0x1f UI reset, 0x18 visit) and the in-game switch (per-SQT progress text `322db770`, count read as a short), and the tail
   (`*(g_pMain + 0x8c) == 0xe / 0x15` tests at the end of the decompile).
2. Read June's state copy `31f09b50` (state dispatcher `31f2ab80` case 0x167, vtable slot 0x160) only for what it does that the window copy does not.
3. Port each June difference under its own flag, original in `#else`; or record "equal" per part. Keep June bugs verbatim with a `JUNE_BUGS.md` row.
4. If any evidence settles which of 0x167 / 0x27c June's server sent for quest progress, record it (Q4e took 0x27c).

Done = a per-part table (equal / ported + flag) in the Done note. No new `.lua`, no data change.
