# June quest system - Phase Q10 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q10 review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q10 done (its Done note and commits), play-test checked |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_STORY` |
| Log tag | `[JUNE_QUEST] STORY` (`python tools/logs.py --grep "JUNE_QUEST\] STORY"`) |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Rules" "Phase Q10 - Story book (QT_STORY)" "Q10 - Done"
```

Evidence: `june2017_re/quest_plan/q10_story_map.tsv` (rows "manager+player"), `q10_story_keys.tsv` (both parsers, key by key).

## Task

Review Q10's commit(s) against June (Ghidra, read-only). Things this phase could get wrong:
- **Parsers:** `AddMediaInfo` `326a8e10` and `AddStory` `326a9890` against `q10_story_keys.tsv`: read order, defaults (Title -1, Speaker / Text -1,
  Scene_Duration 7.0 at `334729f0`), the IsCategory skip at @326a920a (MediaGroup and MediaFile not read, the row still appended), `Cut_Scene%d`
  from 0 up to the first gap, the five sorts of `326a8d70` (which list each sorts), first-wins inserts (`326ae600`, `326b0ea0`), the narrow
  `Scene_Delete.Key`, and that June reads no `BlackLayoutMove`.
- **Loader:** `3258c470` @32590879..32590885 (destroy, make, `326a8d40`); `326a8d40` loads `SlideShotScript.lua` only after `MediaInfoScript.lua` loaded.
- **Rebuild `32335f50`:** the completed map (`+0x38`), the templet map (`+0x08`), the server-group test, `326ab090`, the key (`+0x1b4`), the write to
  `g_pData +0x570`, `g_pInstanceData +0xabd` = the argument; the two callers' arguments (`32336e80` @3233728c = 0, `323e57e0` @323e5a43 = 1).
- **Complete ACK `323e57e0` @323e5a03..323e5a6d:** the type test (`+8 == 10`), `326a7d50`, where the block sits (after the end scene, before `323d4390`).
- **Remind `323ccbc0` case 6:** state test (June 0xe..0xf / 0x15; 0xf is the guild agit March lacks), found / not found, the SetShowEnable argument
  order (`PUSH enable; PUSH show`), RequestFocus in both found branches.
- **The #else paths:** flag-off must equal the Q6 build.

Hygiene: `python tools/flagcheck.py JUNE_QUEST_STORY --range <first Q10 commit>^..HEAD`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines.
The one expected FAIL is `X2SlideShotScriptedManager.cpp`'s `#include "StdAfx.h"` line (outside the flag for the PCH, as G3's new file).
Spot-check 3 items in Ghidra; if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`),
write "Q10 - Review" into the plan doc, update the STATUS row, and check that `kickoffs/quest-system/Q10b.md` still matches.
