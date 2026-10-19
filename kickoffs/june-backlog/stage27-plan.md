# June backlog - Stage 27 planning - June Lua event scenes (EventScene.lua data and the DUO keys)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase new-plan june-lua-scenes` |
| Output | `JUNE_LUA_SCENES_PHASES.md` + `kickoffs/june-lua-scenes/` + a `june-lua-scenes` block in `STATUS.md` |
| Branch | `main` |
| Depends on | nothing. Read-only planning; Ghidra with `unpacked_x2.exe`. |
| Written | 2026-10-19 by quest-system Q8 (scope rule: no quest plays a Lua scene in June - Q0b, re-proved by Q5b - so the data is not the quest plan's) |

## Read first

```sh
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 27: June Lua event scenes (added 2026-10-19, quest-system Q8)" "Standing rules (every stage, every kickoff)"
```
Then `june2017_re/quest_plan/q0b_ui_map.tsv` row `EventScene.lua / EventSceneObject.lua`, and March's parser `X2Lib/X2EventScene.cpp`
(`AddEventSceneObject_LUA` ~:1327, `AddEventScene_LUA` ~:1448, `LoadEventSequence` ~:1259, loaded from `X2Data.cpp` ~:2440 / `X2StateLoading.cpp` ~:875).

## What is known

- June's `EventScene.lua` (2.6 MB) / `EventSceneObject.lua` (data036) use only `AddEventScene_LUA` / `AddEventSceneObject_LUA`, like March's (1.07 MB).
- June's parser reads keys March's does not: `SCENE_OBJECT_DUO_TYPE` in the object parser `325b1f40` and the scene-object-play parser `325b0020`
  (113 uses), `OBJECT_TEXT_TAG_NAME_DUO_TYPE` in the text-play parser `325b0c70` (17 uses). Scene id parser `325b71e0`.
- The same-name rule wants June's two files in March, and "no partial updates" wants the DUO keys ported with them (what a duo type selects at play
  time is not mapped yet: find its readers).
- Who plays Lua scenes in June (not the quest window): find the callers of the Lua `CX2EventScene` play path, and whether any is reachable offline.

## Task

Map the DUO keys end to end (parse -> store -> the play-time reader), list the Lua-scene callers and their offline reach, and write the plan
(adopt both files with `luasrc.py adopt-june`, one flag `SERV_IRUHADEV_JUNE_LUA_SCENE_DUO`, log tag `[JUNE_SCENE]`). Mark this STATUS row done.
