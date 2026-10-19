# June quest system - Phase Q4c - Dungeon result ranks offline

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q4c` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q4 review (done). Not at the same time as Q4b or RM1 (all three edit `X2Lib/Offline/`) |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_RESULT_RANK` (+ `_DEBUG` if the phase logs per run) |
| Log tag | `[JUNE_QUEST] RANK` (`python tools/logs.py --grep "JUNE_QUEST] RANK"`) |
| Review | `kickoffs/quest-system/Q4c-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Rules" "Q4 - Review" "Q4 - Review stand-ins (2026-10-19, user answers)"
```

## Why (user, 2026-10-19)

Every dungeon result offline shows rank F for all four categories and the total, whatever the run was. The emulator never computes a
rank: `Handler_EGS_MY_USER_UNIT_INFO_TO_SERVER_REQ` (`X2Lib/Offline/Handlers_Room.cpp` ~1760) says the thresholds are server Lua it does not
reproduce, so `SendDungeonResultData` sends `KDungeonUnitResultInfo`'s default RT_F, and `Handler_EGS_END_GAME_REQ` (~4491) passes
`RT_NONE` to `QuestOnDungeonClear`. Effects: the result screen, the rank EXP bonus, SQT_DUNGEON_RANK sub-quests (58 in June, e.g. 35367
"Clear with Rank SS"), the Q4 review's SQT 44 stand-in (35363 'The Ultimate Road 4/5' needs SS), and any rank-based title mission.

## Task

Compute the ranks the way March's room server does, with its own Lua, and feed them to the screen and to the quest hooks.
1. **Map the server flow** (`KncWX2Server/CenterServer/ResultProcess.cpp` ~1696-2040, the dungeon result; the second copy ~3019-3230 is
   a different room type, say which). For each of combo / technical / damaged / time: which `lua_tinker::call` produces the score,
   how `spRoomUser->GetRank()` turns it into a rank (find `SetRank` / the room-user code), and how `DUNGEON_TOTAL_RANK_RESULT` (or the
   `2*combo + 2*tech + damaged + time) / 6` fallback) makes the total, including the dead-unit RT_F branch and the RT_SS / RT_S clamps.
2. **The Lua is server-only:** `KncWX2Server/ServerResource/US/ResultData_new.lua` (8 functions: `WEIGHT_BY_DUNGEONID`,
   `DUNGEON_TIME_RESULT`, `DUNGEON_DAMAGE_RESULT`, `DUNGEON_COMBO_RESULT`, `DUNGEON_TECHNICAL_RESULT`, `DUNGEON_TOTAL_RANK_RESULT`,
   `SUB_STAGE_*`). It is not in `data036_kom`. Follow CLAUDE.md "server-side Lua the client needs": load it the shipped way
   (`LoadDataFile` + `DoMemory`, `DoMemoryNotEncript` fallback with a note; shape of `X2OfflineStatTable.cpp`), degrade visibly
   (file missing -> ranks stay F, one log line naming the file), and name the file for the user to XOR-pack into data036 when done. Do not
   transcribe its functions into C++ and do not invent thresholds (a Lua function gets no fallback).
3. **Its arguments are server objects** (`pUnitInfo`, `pMonsterManager`). List every method / field the 8 functions call on them, then
   bind a client-side object with exactly those names (lua_tinker, as the server binds them; cite the server binding site), filled from
   what the emulator has: `m_kRoom.m_kPlayResult` (the client's own combo / technical / damage figures), play time, the dungeon's monster
   list (the server's `KDungeonMonsterManager` counts and weights; say where the emulator can get the same numbers, or stop and ask if
   it cannot). Party members offline = 1 (`nEndNumMember`).
4. **Feed the results:** the four ranks + total into `KDungeonUnitResultInfo` in `SendDungeonResultData`; the total rank into
   `QuestOnDungeonClear` instead of RT_NONE; the rank EXP bonus if the server pays one from the total (cite it; the comment at ~1765 says
   it was left out for this reason). Keep the original code in `#else`.
5. **Gate:** a run played well shows a rank above F on the result screen and in the log (`[JUNE_QUEST] RANK combo .. tech .. damaged ..
   time .. total ..`); a run where the unit died shows F (the server's branch); unit 25 (jtLordKnight) holds 35363 with 35365 at 4 / 5, so an
   SS clear in Atlas Development Normal (band 78..84) should log `together 35365 ( quest 35363 ) counted 5 / 5`.

Stop and ask before any number or rule that is not in `ResultData_new.lua` or March's server code.
