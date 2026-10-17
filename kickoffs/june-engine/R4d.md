# June engine remainder - Phase R4d - Game render split

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (transcribe from Ghidra), then a user play-test; type `/x2-phase june-engine R4d` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R4c; not alongside another session editing `X2Game.cpp` or the game-class files it lists |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_GAME_SPLIT` |
| Log tag | `[JUNE_ENGINE] SPLIT` (`python tools/logs.py --tag JUNE_ENGINE`) |
| Review | `kickoffs/june-engine/R4d-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase R4 -- Done (RE only, 2026-10-18, Opus 5.5)" "Phase R4d - Game render split"
```

Then the sweep rows named below (`june2017_re/r4_frame_stage_sweep.tsv`) and the funcmap rows tagged `(R4)`.

## Task

Do "Phase R4d". The rows are `june2017_re/r4_frame_stage_sweep.tsv` area `game` (owner R4d).

- **Split** `CX2Game` (`322d7260` / `322cc8f0` / `3230aa60`) and the March game classes (`CX2PVPGame`, `CX2PVPGameSurvival`,
  `CX2PVPGameTeamDeath`, `CX2DungeonGame`, `CX2BattleFieldGame`). Transcribe each `0x10` UI slot from June.
- **June blocks** already in `CX2Game::OnFrameRender` move into the matching slot: `JUNE_RENDER_ENVBOX`, `ELYSION_SCRIPT_B`,
  `DRAMATIC_MESSAGE` and `SKIP_START`.
- **State callers.** The R4c state slots then call the game's three slots, as June does.
- **Gate (play-test):** dungeon, PvP (team, death match, survival) and battlefield HUDs, the boss intro / dramatic message and the
  skip-start button look as before.
