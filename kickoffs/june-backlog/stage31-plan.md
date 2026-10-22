# June port backlog - Stage 31 planning - June achievement system

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase new-plan june-achievement` |
| Output | plan key `june-achievement` (`kickoffs/june-achievement/`, a `june-achievement` STATUS block) |
| Plan doc | `JUNE_PORT_BACKLOG_ORDER.md` (Stage 31), output `JUNE_ACHIEVEMENT_PHASES.md` (new) |
| Depends on | - (planning is read-only; code waits for its turn in the priority order, after Stage 9) |
| Flag | one family, e.g. `SERV_IRUHADEV_JUNE_ACHIEVEMENT_*` (none in this planning session: no code) |
| Log tag | `JUNE_ACHIEVEMENT` (propose it in the plan) |
| Review | none (a planning session); each planned phase gets its own review kickoff |

## Read first (and nothing else unless you need it)

```sh
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 31: June achievement system (june-transcend planning, 2026-10-22)" "Standing rules (every stage, every kickoff)"
grep -n -i "achievement" june2017_re/xorstrings.tsv
```

## Task

Identify June's achievement system and plan its port. March has none (no `Achievement` in `X2Lib`). Do the RE read-only in Ghidra.
- **The manager.** `CX2AchievementManager` (RTTI `33735a64`). Its binders `AddAchievementData_LUA` / `AddAchievementCondition_LUA` are
  registered in `31d95080` (xorstrings `31e45cae` / `31e45ec9` / `31e4611e`); parsers `325fa910`, `325fdd90`, `32600540`.
- **The data.** `AchievementData.lua` and `AchievementConditionData.lua` (`june_src_fixed/data036_kom/`). Condition types include
  `ACT_DUNGEON_CLEAR` and `ACT_TRANCENDENTAL_CLASS_COUNT` 14 (ids 70-72: 2 / 7 / 10 transcended characters, found by june-transcend
  planning).
- **The UI.** `DLG_Achievement_System_UI.lua`, `DLG_Achievement_System_Piece_Detail_Info.lua`, the achievement buff notice, the
  `ACHIEVEMENT_SCORE_EMBLEM`, sounds `Achievement_List_Renew.ogg` / `Achievement_Mouse_Right_Click.ogg`.
- **The packets.** `ERR_ACHIEVEMENT_00-03`, the admin handlers `EGS_ADMIN_RESET_ACHIEVEMENT_REQ`, `..._ACHIEVEMENT_CONDITION_UPDATE_REQ`,
  `EGS_ACHIEVEMENT_POINT_UPDATE_BY_ADMIN_REQ`, and the live update / reward packets.
- **The rest.** Where conditions are counted (client or server), what rewards are (titles, buffs, items), and the emulator and save it
  needs.

Output:
- `JUNE_ACHIEVEMENT_PHASES.md` (map phase first, Opus review each);
- one kickoff per phase in `kickoffs/june-achievement/`;
- a STATUS block;
- funcmap rows tagged `Achievement S31`.

Open design questions go to the user at the end, each with a recommendation.
