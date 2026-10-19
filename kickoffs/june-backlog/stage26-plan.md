# June backlog - Stage 26 planning - June AVI movie system (dungeon movies and the movie flag's readers)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase new-plan june-movies` |
| Output | `JUNE_MOVIES_PHASES.md` + `kickoffs/june-movies/` + a `june-movies` block in `STATUS.md` |
| Branch | `main` |
| Depends on | quest-system Q8 review (the table, the player and the flag this stage extends). Read-only planning; Ghidra with `unpacked_x2.exe`. |
| Written | 2026-10-19 by quest-system Q8 (scope rule: the quest movies are Q8's, everything dungeon- and field-driven is here) |

## Read first

```sh
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 26: June AVI movie system (added 2026-10-19, june-gfx G5)" "Standing rules (every stage, every kickoff)"
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Q8 - Done"
```
Then the funcmap rows tagged `(Quest Q8)` (each routed row names this stage) and Q8's `CX2Main::June*ClearMovie*` in `X2Lib/X2Main.cpp`.

## What Q8 left here (June addresses; all of them read `g_pMain +0x80a` or start / stop the movie)

- **Dungeon first clear:** `31d7a150( dungeon, unit, intro )` (row `+0x1c` dungeon list, `+0x68` intro filter, none of the list cleared = `3234f870`);
  the dungeon game's END_GAME handler `324db6b0 @324db86b-324db8b4` flags the clear movie (intro 0), adds the seen id (`31d758c0` -> `31d78bf0`) and
  `AddClearDungeon` (`32356a30`). March's counterpart: `X2StateDungeonGame.cpp:3179-3180` (`IsClearDungeon` / `AddClearDungeon`).
- **Where the flagged clear movie starts:** the field state's OnFrameMove-side `32507840` (vtable `334a1520`; also called from `325090ce`) and the battle
  field's `324bb780` (via `324bbc00`, June event 0x8e): `+0x80a` -> `31d880b0`, failure -> `31d883a0`.
- **Intro movies** (Elder Secret / Henir rows, `m_bIntroMovie = TRUE`): the party manager's dungeon-start path `32434710` (intro 1, parks the request at
  `+0x90`, id `+0xd8` = 0x374 / 0x377) and its resume `32438fa0`, which `31d883a0` calls at its end.
- **The other readers of `+0x80a`** (census: 28 byte patterns in the image, 6 outside the movie code itself):
  `322f7200` CX2Game::KeyProcess head (no game keys while a movie plays outside the dungeon game; June's head also differs on the cash-shop branch);
  `3230ada5` CX2Game render `3230aa60` (the state string waits for the movie); `323b7da1` CX2TFieldGame::OnFrameMove (the field title `323b1320`
  OpenFieldName is reserved in `+0x3c0` and opened only with no movie; March opens it in the ctor); `32d54ff3` CX2BattleFieldGame (same for `+0xcd5` /
  `32d54280`); `32508736` CX2StateField::OnFrameMove `32507d40`; `323aad41` (profession manager: Stage 9). `31d71810`'s callers are June-only (G3 row).
- **`31d87560`'s two bodies:** the state vslot 0xb4 (battle field `324bafc0` sends June event 0x3af; field `3208ddc0` is a no-op) and the user's busy
  emotion 0x11 (`32044a80`, also called by the cash shop `3223d680` and the battle field `324baf40` / `324bb780`). Q8 kept only the call (a log line).
- **Other stops:** `31f06e60 @31f07179` (GAME_START_TIME_COUNT_NOT stops a playing movie) and `32db33e0 @32db392e`.
- **Movie files:** 13 `.avi` in June's `data\movie\` (Q8 asked the user to copy them; check which are in March's `data\movie\`).

## Task

Map each item above against March (cite file:line), decide which are reachable offline, and write the plan in the `new-plan` shape: one flag family
`SERV_IRUHADEV_JUNE_MOVIE_*`, log tag `[JUNE_MOVIE]`, phases small enough to review (first clear + field start; intro + party resume; the readers;
`31d87560`'s bodies). Mark this STATUS row done.
