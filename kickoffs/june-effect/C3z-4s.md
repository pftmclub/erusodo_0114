# June effect completion - C3z-4s - June's CX2SimpleDLG_WithLuaFunc (Ranox Secret Hell)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3z-4s` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3z-4 batch 1 |
| Flag | `SERV_IRUHADEV_JUNE_SIMPLE_DLG` |
| Log tag | `JUNE_FX C3z-4` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z-4 - Done (batch 1)"
```

## Task

`secret_scene_dummy.lua` (Ranox Secret Hell only) calls `CreateSimpleDLG` (`322def10`: new 0x14, `326b24a0`, into game `+0xba8`),
`GetSimpleDLG` (`31e4de40`) and the class methods `SetShow_LUA` / `SetNumberInNumberStatic_LUA` / `SetNumberInGageStatic_LUA` /
`SetNumber_LUA` / `GetNumber_LUA` / `CrashAndColorChange_LUA` (`326b1380`, `326b1bb0`, `326b1d40`, `31e4d8a0`, ?, `326b1a10`; the
resolver read `32e4ea40` for `GetNumber_LUA`, outside the group: re-read it). June's class binds 12 methods.

Read the class (ctor `326b24a0`, its dialog file, update and render), port it with its Lua binds and the game member, and the
scene's other unbound calls (`c3z4_bind_plan.tsv`: `GetDyingStart`, `FadeOriginWorldColor_LUA`, `GetNowStateName` are batch 1 / 2).
