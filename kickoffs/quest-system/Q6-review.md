# June quest system - Phase Q6 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q6 review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q6 done (its Done note and commits), play-test checked |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_GATE` |
| Log tag | `[JUNE_QUEST] GATE` |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Rules" "Phase Q6 - Quest gates: the inert keys and their readers" "Q6 - Done"
python tools/docsection.py ATLAS_PORT_PHASES.md "Deferred to the quest plan"
```

Evidence: `june2017_re/quest_plan/q0a_june_callers.md` (rows 1, 5, 6, 10-13, 15 and Part B).

## Task

Review Q6's commit(s) against June (Ghidra, read-only). Things this phase could get wrong:
- **Parser:** `32501bb0` @32502b6a reads `RequireCompletedQuestID` inside `EnterCondition` (templet `+0x70`, default 0). March's key sits next to `RequireUnitLevel`.
- **Each reader's condition**, against June's decompile (`FUN_<addr>`):
  - tab `3293c6f0` @3293cbe0 (+0x70 < 1 -> open; else complete -> open; else `SetShowEnable( false, false )` and no quest mark; a GM never reaches it);
  - `IsActiveDungeon` `327da190` @327da2ea (only when the location manager is set; `GetLocalMapID` of the row's `m_DungeonID`);
  - warp item `32173120` (`== 0` or complete) and Cobo `324627f0` (`!= 0` and not complete -> the else branch that moves the list index);
  - world map `3225d340` (hide) and field map `3225d970` (`SetEnable`);
  - quest notice `323d4390` (only inside the `RequireClearDungeonID.size() > 0` branch, as June's `size == 0 ||` short-circuit).
- **The data line:** AD Normal `m_iRequireCompleteQuestID = 22218` in data035 `DungeonData.lua` (pack gate PASS, `luasrc.py ledger --check` PASS).
- **The #else paths:** flag-off must equal the Q5a build. `X2UIQuestReceive.cpp` is March's own copy of the notice filter (June has none).
- **Left as is on purpose (user, 2026-10-19):** MapData `REQUIRE_COMPLETE_*` and BattleFieldData `NEED_ONGOING_QUEST_ID` / `BATTLE_FIELD_DIRECTION_INFO` have no stand-in.
  Confirm June's exe really has no reader (strings / XOR table, `q0a_june_callers.md` Part B) before accepting that.

Hygiene is one command: `python tools/flagcheck.py JUNE_QUEST_GATE --range <first Q6 commit>^..HEAD`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines.
Spot-check 3 items in Ghidra; if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`), write "Q6 - Review" into the plan doc,
update the STATUS row, and check that the Q9 kickoff still lists the gate checks.
