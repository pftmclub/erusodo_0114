# June quest system - Phase Q8b - The quest window's scenario-movie tab

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q8b` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q8 review (the movie table, the seen list and the player it builds on) |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB` |
| Log tag | `[JUNE_QUEST] MOVIE` (`python tools/logs.py --tag JUNE_QUEST`) |
| Review | `kickoffs/quest-system/Q8b-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Rules" "Starting context" "Phase Q8b - The quest window's scenario-movie tab" "Q8 - Done"
```
Then `june2017_re/funcmap.tsv` rows tagged `(Quest Q8)`, and Q8's code: `CX2Main::June*ClearMovie*` at the end of `X2Lib/X2Main.cpp`
(before `LoadLuaEnum`), and Q5a's tab stub in `X2Lib/X2UIQuestNew.cpp` (cases 41038-41042) and `X2UIQuestNewJune.cpp` `JuneHideMovieTab`.

## Task

The quest window's movie tab. Its strings already say so: string 30615 ends "To rewatch, go to [Quest] window and press [Video] button."
It is part of the quest window (the plan's subject); Q5a hides its controls and logs its five messages.

1. **Identify, in Ghidra** (cite addresses, funcmap rows `(Quest Q8b)`):
   - the tab builder `323ca6d0` (`Button_Movie_%d`, `NewTexture_%d`, `g_pStatic_HorizonLine`, `RadioButton_Area`, `Village`; callers `323d0ca0`, `323e5340`);
   - the list `323dfc00` (`DLG_UI_Common_Texture_ScenarioMovie_01.tga`, `Overlay_Lock`; called from `323e45c0`);
   - `UICustomEventProc` cases 41038-41042 in `323e1bf0`'s tail (TAB / PLAY / AREA_TAB / BUTTON_OVER / BUTTON_OUT); play by id `31d884f0` @323e409b
     (it sets the name from the row with `m_eMovieCategory` = id, sets `+0x80a`, then starts with `31d880b0`, which Q8 ported as `JuneStartClearMovie`);
   - what the list reads per row: `m_wstrMovieTextureName` / `KeyName`, `m_wstrIconTexture*`, `m_wstrDungeonName`, `m_eMovieCategory`, `m_bIntroMovie`;
   - **the seen list `g_pMain +0x84c`** (Q8's `m_vecJuneSeenClearMovie`): every writer besides `31d78bf0` (Q8 calls it from the complete requests; the dungeon
     first clear `324db6b0` / intro `32434710` calls are Stage 26's). Find whether June fills it from a server packet at login. If it does, the offline
     source is a design question for the user (save table, or derive it from completed quests / cleared dungeons): ask, do not invent.
2. **Port the tab whole** behind the flag, with March's Q5a stub kept in the `#else`. If the seen list needs save state, propose the schema rung first.
3. If a branch the tab reaches is not quest UI (for instance the dungeon movies), follow the scope rule: it is Stage 26's.

**Done gate:** the quest window's Video tab lists June's 13 movies by area, the seen ones unlocked; pressing one plays it (with subtitles where
`datai015` has them) and returns to the window. 0 EXCEPTION.
