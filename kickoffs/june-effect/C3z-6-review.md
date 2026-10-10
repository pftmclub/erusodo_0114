# June effect completion - C3z-6 review - the emulator's server-side DungeonData semantics

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3z-6 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3z-6 done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_OFFLINE_RELATIVE_NPC_LEVEL`, `SERV_IRUHADEV_OFFLINE_SECRET_POISON` |
| Log tag | `JUNE_FX C3z-6` (unbracketed: `logs.py --grep "JUNE_FX C3z-6"`), plus `GAME     NOTE dungeon` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z-6 - Done (2026-10-10, Opus 5.5 high; 2 flags)"
```

## Task

Review C3z-6's commit against the March server it ports. No Ghidra needed: the source is the server tree.

1. **Relative level:** compare `RelativeMonsterLevelForStage` and `OnDungeonStartGame` (`X2Lib/Offline/Handlers_Room.cpp`)
   line by line with these server lines:
   - `KncWX2Server/CenterServer/DungeonRoom.cpp:3646-3712` (stage load) and `:756-852` (StartGame);
   - `KncWX2Server/CenterServer/RoomUserManager.cpp:6309-6352` (the average).
   Check:
   - the clamp order;
   - no clamp after the +5;
   - the defence clamp uses the ID without difficulty;
   - the extra-stage count increments only for DT_SECRET + DM_SECRET_HELL + ST_EXTRA;
   - the Henir-challenge `= 1` branch (`SERV_HENIR_RENEWAL_2013` off);
   - the `<= 0` fallback.
2. **SecretDungeonData.lua loader:**
   - The borrowed `g_pDungeonManager` is handed back on every path after the bind.
   - A negative level is refused (`XSLDungeonManager.cpp:701-712`).
   - The missing-file line names the file and the fallback 2 (`SecretDungeonData.lua:6`).
3. **Poison:** compare `StartSecretPoison`, `TickSecretPoison` and `ExtraStageSecretPoison` with `DungeonRoom.cpp:758`,
   `1072-1078`, `167-186` and `4147-4166`. Judge whether the one recorded deviation is acceptable: the 19999 s play-clock
   reset is not mirrored on `m_dwPlayStartTick`.
4. **Verdict-only keys:** re-check the Done note's verdicts (`requireLevel`, `m_bForbidEnterDungeon`, `m_bArcadeMode`,
   `m_bSwitchingWithEventInfo`, the `DUNGEON_CHECK_CONDITION` server half) against the server greps it cites.

Hygiene, for each flag in turn (the compound `#if defined(A) || defined(B)` blocks need both stripped):
`python tools/flagcheck.py SERV_IRUHADEV_OFFLINE_RELATIVE_NPC_LEVEL --range <C3z-6 commit>^..HEAD --with SERV_IRUHADEV_OFFLINE_SECRET_POISON`
`python tools/flagcheck.py SERV_IRUHADEV_OFFLINE_SECRET_POISON --range <C3z-6 commit>^..HEAD --with SERV_IRUHADEV_OFFLINE_RELATIVE_NPC_LEVEL`
Paste the `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row; a FAIL or ERROR is a finding.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "C3z-6 - Review" into the plan doc, update the C3z-6 STATUS.md row, and commit.
