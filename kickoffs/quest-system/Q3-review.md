# June quest system - Phase Q3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q3 review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q3 done (its Done note and commits), play-test checked |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_RUNTIME` (+ `_DEBUG`) |
| Log tag | `[JUNE_QUEST] RUN` |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Rules" "Phase Q3 - Quest manager runtime to June" "Q3 - Done (2026-10-19, Sonnet 5.5 high)" "Q0a - Done (2026-10-19)"
```

## Task

Review Q3's commit(s) against June (Ghidra, read-only) and `june2017_re/quest_plan/q0a_qm_summary.md`. Things this phase could get wrong:
- **Chain rule `3232caf0`:** the max over a chain's completion times, the code-event-0x10 minute branch, the local-midnight branch (`T + AllowFewDays * 86400`, midnight of that day), and the three
  places that rebuild the startable map (`3232ef70` callers: create, remove, refresh-daily, SetUnitQuest). Insert-unique on the completion record (`RemoveUnitQuest`, `SetUnitQuest`).
- **Week / month `32322050` / `32321d10`:** weekday arithmetic (Sunday = 1, Wednesday = 4 -> 7 days), the December wrap, `<=` against the server time64.
- **`SetUnitQuest`:** the daily / week / month skip order, the third argument, nothing counted for a purged record.
- **Lists:** `3232fbd0` (QT 2 / 9 at every NPC, dedup only for those, guild gate, QT 5 skip), `323357c0` (QT 8 drop in the non-repeat branch only, chain flag after the class-change loop),
  `3232fd70` (each case of the village switch against the table in the Done note; early returns for Peita and 1008; 1500 adds NPC 10025).
- **`IsForbiddenQuest` `3232e510`:** the QR gate, the QT 9 code events (0x920 type 12, 0x921 class 0x87, 0x923 class 0x88), and the gates left out as server-fed (Done note): re-check each id against June's
  `FieldQuest` and decide whether any is wrong to skip.
- **`MakeSubQuestString` `32337db0`:** cases 0x25 / 0x27 / 0x2b, the clear-count group (0x2c, 0x30), the any-dungeon text order (`42924` then `17688`), the cases that print nothing.
- **The #else paths:** flag-off must equal Q2's build.

Hygiene is one command: `python tools/flagcheck.py JUNE_QUEST_RUNTIME --range <first Q3 commit>^..HEAD --with JUNE_QUEST_RUNTIME_DEBUG`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines.
Spot-check 3 items in Ghidra; if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`), switch `SERV_IRUHADEV_JUNE_QUEST_RUNTIME_DEBUG`
off once the play-test is read, write "Q3 - Review" into the plan doc, update the STATUS row, and check that Q4b / Q6 / Q10 kickoffs are still right.
