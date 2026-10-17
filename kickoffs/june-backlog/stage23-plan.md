# June backlog - Stage 23 planning - offline field rewards stand-in

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase new-plan field-rewards` |
| Output | `OFFLINE_FIELD_REWARDS_PHASES.md` + `kickoffs/field-rewards/` + a `field-rewards` block in `STATUS.md` |
| Branch | `main` |
| Depends on | atlas A6 (done). Runs alongside anything; this session is read-only (no code, no build). |
| Written | 2026-10-17, by atlas A6 (user: "a new phase inside backlog order") |

## Read first

```sh
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 23: offline field rewards stand-in (atlas A6, 2026-10-17)" "Standing rules (every stage, every kickoff)"
python tools/docsection.py ATLAS_PORT_PHASES.md "Phase A6 — Done"
```
Then: `X2Lib/Offline/X2OfflineDropTable.cpp` (`AddBattleFieldNpcDropInfo_LUA` ~:415, `AddBattleFieldStaticDropInfo_LUA` ~:529, the
wildcard key), `X2Lib/Offline/Handlers_Room.cpp` `Handler_EGS_NPC_UNIT_DIE_REQ` (~:3200-3330: EXP / ED / boost / grade), and the
battlefield blocks of `KncWX2Server/ServerResource/US/DropTable.lua` / `StaticDropTable.lua` (rows 40000-40025: their shape by field level).

## Task

1. **Measure:** which field ids have no rows (expect Lanox 40029-40032, Elysion 40035-40038), which monsters spawn there (the NPC tables
   `Ranox_Field_0N.lua`, `Elysion_Field_0N.lua`), and what a real row looks like for the nearest level band (Sander 40022-40025).
   **Dungeons too (atlas A8 review, 2026-10-17):** the Elysion dungeons have no `AddMonsterDropInfo` rows either (the server
   `DropTable.lua` stops at `DungeonID = 33252`; Lanox 33300 / 33310 and AtlasDevelopment 33330-33332 have none, nor do NUIs 2092-2127),
   so a dungeon kill pays 0 EXP / ED / items and the clear bonus is 30 % of 0 (`Handlers_Room.cpp:3312`). Measure and plan both halves:
   fields (`AddBattleFieldNpcDropInfo`) and dungeons (`AddMonsterDropInfo`, keyed `DungeonID` + `MonsterID`, wildcard `DungeonID = 0`).
2. **Look for real data first:** does anything shipped (June client Lua, the server files' dungeon rows for Lanox / Elysion monsters,
   `NpcExpTable`) give per-monster EXP / ED or drop groups for these monsters? Real data beats a stand-in.
3. **Propose the stand-in** (memory `design-missing-june-pieces`): two or three options with what each pays, to the user, before any
   plan phase is code. Options to weigh: derive EXP / ED from `NpcExpTable` by level, copy the Sander band's drop groups, add rows to the
   server Lua (the user packs it: CLAUDE.md server-Lua rule) versus a coded fallback in `X2OfflineDropTable` behind its own flag.
4. Write the plan in the `new-plan` shape (one phase per region or one for both, as the measurement says), log tag, flag name, and mark
   this STATUS row done.
