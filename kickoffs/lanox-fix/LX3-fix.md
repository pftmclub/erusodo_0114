# lanox-fix - Phase LX3-fix - Geyser Sureyna summon softlock

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort (Opus 5.5 if the logs point at engine RE), then type `/x2-phase lanox-fix LX3-fix` |
| Plan doc | `STATUS.md` block `lanox-fix` (branch `mods/june-buff-port`) |
| Depends on | LX3 diagnostic play-test (Geyser Normal 33290, stage 3, Sureyna past 60% HP) |
| Flag | new `SERV_IRUHADEV_JUNE_<NAME>` for the fix; `SERV_IRUHADEV_GEYSER_SUREYNA_DEBUG` (diagnostic, remove once fixed) |
| Log tag | `SUREYNA` (`python tools/logs.py --grep SUREYNA`) |
| Review | none |

## Read first

- `june_src_fixed/data037_kom/GEYSER_MULSAYRENA.src.lua` :78-215 (INIT_STATE), :1158-1260 (SPECIALATTACK_A_*), :1418-1450 (OUT_STATE).
- Commit `365baf9` (the diagnostic's log sites: X2GUNPC.cpp DoStateEndStart / Lua START, FRAME_MOVE, END / StateChangeForce /
  JuneCreateNPCReqFromNPC; X2Game.cpp CreateNPCReq / CreateNPC / Handler_EGS_NPC_UNIT_CREATE_NOT).

## Task

Expected June flow: A_READY -> (END sets int2 = A_2, int3 = 2, vec3.x = 1.7, timer1 restart) -> OUT_STATE (hidden at y -5000;
FX at timer1 > 1.7; StateChangeForce(int2) at timer1 > 2) -> A_2 (5 x NUI_GEYSER_MULSAYRENA_CALL 1900 at line-map start
positions 2-6; she reappears at (50,0,1340) after AnimEventTimer 0.25) -> A_3 when fewer than 3 minions live -> A_4 -> OUT_STATE
-> A_5 (chair 116,207,-711) -> WAIT.

Read the run's `SUREYNA` lines in `offline_server.log`, `EGS_NPC_UNIT_CREATE` in `offline_packets.log`, and `[LUA_SRC]` /
`NPC Motion Error` in `ErrorLog.txt`. Find the first step where the log leaves that flow:
- `vars` shows timer1 > 1.7 with vec3.x still 1.7 -> OUT_STATE FRAME_MOVE dies before `vec3.x = 9999` (Lua error; read `[LUA_SRC]`).
- StateChangeForce `REFUSED` -> the bounds / cool-time gate.
- A_2 entered, no `request sent` / `CreateNPC ... OK` -> the client create path (host gate, lastKillCheck, SetPosition failure).
- No FRAME_MOVE lines while hidden -> the update path skips her (stop timers in the `OnFrameMove` tick line).
Port June's behaviour for that step (Ghidra, read-only) behind the new flag. Done = a Geyser run whose log walks the whole
flow to A_5 and WAIT, minions spawn, and the stage clears. Then turn `SERV_IRUHADEV_GEYSER_SUREYNA_DEBUG` off.
