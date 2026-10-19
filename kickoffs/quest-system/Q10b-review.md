# June quest system - Phase Q10b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q10b review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q10b done (its Done note and commits), play-test checked |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_STORY_PLAYER` |
| Log tag | `[JUNE_QUEST] STORY PLAY` (`python tools/logs.py --grep "STORY PLAY"`) |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Rules" "Phase Q10b - Story slide-show player" "Q10b - Done"
```

Evidence: `june2017_re/quest_plan/q10b_player_map.tsv` (every function, offset and constant the port used), `q10_story_map.tsv` rows "manager+player".

## Task

Review Q10b's commit(s) against June (Ghidra, read-only). Things this phase could get wrong:
- **Start `328c3750`:** the state set (June 0xe / 0xf / 0x15 -> March village / battle field), the already-running return, the black background's
  CreateSequence arguments ( `-1.0` from `334380f0`, trigger time 1.0, bCullCheck 0, layer 100 ), the village / battle-field branches ( which one
  hides objects ), the saved BGM, hide mode 5, the cursor value ( June 0xf = March `XCS_EVENT_SCENE`? check against the old slide show 325c6791 ).
- **OnFrameMove `328c41f0`:** the fade comparisons ( `> 0.1` double ), the camera numbers ( 3349d870 / 33443950 ), the ESC / Enter keys and mouse
  buttons, the 1.5 s no-picture rule ( 33471a1c ), which list index pairs with which cut vector ( `+0x6c` / cut `+0x38` ... `+0x78` / cut `+0x68` ),
  one row a frame vs the Scene_Delete loop, the two keys ( "%I64d_%I64d" and "...BG" ), bNextFrame 0 / 1 and layer 100 / -1, the spread time
  ( Spread_Rate / text length ), the auto-advance max and the Static_Enter 1.0 s rule, the "background still due" hold.
- **AddText `328c27d0`:** width 675.0 x resolution scale, the \n / \r strip, no -1 test after the wrap, Name-null early return, the dummy-pos branch.
- **End `328c3ae0`:** restore order, SetProjection arguments ( near +0x110, far +0x114 ), the village `m_bEnableCameraProcess` and the end message
  ( `GetReplacedString( 40048, "SL", GET_STRING( 40147 ), title )`: which argument is S and which L ), the box position, `+0xabc` cleared.
- **Starts / forced Ends / gates:** ACK @323e5a4a..6d, Remind @323e293d..2963, the party handlers' event ids ( 0x147 / 0x37d via the two byte tables ),
  CX2Game::Release, and the four gates ( 32276284, 322f8326, 32309d80, 32e90649 ): placement next to March's existing checks.
- **Not carried:** June world `+0x05` ( no reader found ) and the write-only fields: confirm no reader with your own scan.
- **The #else paths:** flag-off must equal the pre-Q10b build ( the one `replace` in `X2Game.cpp` keeps the original line in the `#else` ).

Hygiene: `python tools/flagcheck.py JUNE_QUEST_STORY_PLAYER --range <first Q10b commit>^..HEAD`. Paste its `files:` / `in this flag's files:` /
`RESULT:` lines. The one expected FAIL is `X2SlideShotScripted.cpp`'s `#include "StdAfx.h"` line (outside the flag for the PCH, as Q10's new file).
Spot-check 3 items in Ghidra; if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`),
write "Q10b - Review" into the plan doc, update the STATUS row, and check that `kickoffs/quest-system/Q10c.md` still matches.
