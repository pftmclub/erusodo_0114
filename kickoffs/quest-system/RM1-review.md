# June quest system - Phase RM1 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system RM1 review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | RM1 done (its Done note and commits), play-test checked |
| Flag | `SERV_IRUHADEV_JUNE_RANDOM_MISSION` |
| Log tag | `[JUNE_QUEST] RM` (`python tools/logs.py --grep "JUNE_QUEST\] RM"`) |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Rules" "Phase RM1 - Random mission port" "RM1 - Done"
```

Evidence: `june2017_re/quest_plan/rm0_map.tsv` (56 rows) and the `(Quest RM1)` rows in `june2017_re/funcmap.tsv`.

## Task

Review RM1's commit(s) against June (Ghidra, read-only). Things this phase could get wrong:
- **Mission logic:** `AddEvent 32679820` (which types add, which assign, type 3's ceiling), `OnDungeonEnd 326797a0`, the time limit in `32679a50`
  / `326799a0` (accumulator starts at -1.0, the tick order, the failed flip), the manager state changes `32679970` / `32679a20`.
- **Life cycle:** `StageStart @327d1b8a` (state 1 -> 2 + 5 s timer, state 4 -> destroy), `OnFrameMove @327d3bfb` (state >= 2), the three resets
  (`31f06e60` count 3, `327d2240`, `324d8f70` case 12), `RefreshUI 326765f0` and its two callers (`32154820`, `324d2e20` Tab).
- **The raise sites** (12 events): `rm0_map.tsv` rows event 1-12. Re-check the two NPC-damage copies (A user attacker, B owned summon), the sub-stage
  boss test (June sub-stage `+0x2c` = `m_bIsBossStage`, set at ctor `327f09e5`), `+0x8aeb` = `m_bHasBossGage` (written @`327ef788`), event 4's
  gate test (vslot `0x298` = class type 100), event 9's `UpNowMp` deviation (no a2 / a3 flags in March; my gage only), and that Add's hook sits after
  `SetSkillCoolTimeLeft`.
- **UI:** `CreateUI 32678910`, the result `326791c0` (popup only on success), `32677f90`, the text builders `32677020` / `32677740` / `326784b0`
  (string ids, `Sii` order, the colour swap `#C600000` -> `#CED3131`), the tooltip's 30745 arguments (prefix, EXP rate, buff name), fade `326762d0`.
- **Packets:** `31efe2a0` reader order, ACK `324c94d0`, the `AddServerPacket( 0x494, 10 s )`.
- **Emulator stand-in** (a = type, b = goal, c = seconds, +10% EXP, description ids 30677-30688): flagged as not June's; nothing is sent in normal play.
- **The #else paths:** flag-off must equal the Q4c build.

Hygiene: `python tools/flagcheck.py JUNE_RANDOM_MISSION --range <first RM1 commit>^..HEAD`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines.
The expected FAIL is the new files' `#include "StdAfx.h"` line. Spot-check 3 items in Ghidra; if one fails, re-check the whole phase. Fix clear
transcription errors under the same flag (then `tools/build.py --deploy`), write "RM1 - Review" into the plan doc, and update the STATUS row.
