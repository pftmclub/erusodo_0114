# June engine remainder - Phase R4c - State render split

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (transcribe from Ghidra), then a user play-test; type `/x2-phase june-engine R4c` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R4b; not alongside another session editing the `X2State*.cpp` files it lists |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_STAGE_SPLIT` |
| Log tag | `[JUNE_ENGINE] SPLIT` (`python tools/logs.py --tag JUNE_ENGINE`) |
| Review | `kickoffs/june-engine/R4c-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase R4 -- Done (RE only, 2026-10-18, Opus 5.5)" "Phase R4c - State render split"
```

Then the sweep rows named below (`june2017_re/r4_frame_stage_sweep.tsv`) and the funcmap rows tagged `(R4)`.

## Task

Do "Phase R4c". The rows are `june2017_re/r4_frame_stage_sweep.tsv` area `state` (owner R4c).

- **Split.** For `CX2State` and each March state class, transcribe June's `0x8` / `0xc( pass )` / `0x10( pass )` (and `CX2State`'s
  `0x18`, `31ef76b0`) from Ghidra: gates, dialog handles, pass arguments. March's `OnFrameRender` stays in each `#else`.
- **Earlier ports** inside March's bodies move with their code: `JUNE_TUTORIAL_MODE` and `JUNE_MOVIE_DESCRIPTION` in
  `CX2StateDungeonGame`.
- **In-house extras** (`FIRST_SCREEN_TEST` in `CX2StateLogin` / `CX2StateStartUp`) stay guarded in `0x10`.
- **Plain classes.** `CX2TFieldGame`, `CX2SquareGame` and `CX2WeddingGame` split with their states.
- **Resolve** `g_pMain +0x444` (the second text manager drawn in `0x18`).
- **Until R4d** each state calls its game's March `OnFrameRender` where June calls the game's `0x8`.
- **Size.** This is the plan's largest port. If it does not fit one session, stop after the `CX2State` base and the thunk classes, commit,
  and write `R4c2` for the rest (rule: plan unowned branches without asking).
- **Gate (play-test):** login, server select, character select, village, dungeon room, PvP lobby / room, a dungeon, a PvP match, the
  square, results and loading screens look and behave as before.
