# June dungeon result - Phase R1 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-result R1 review` |
| Plan doc | `JUNE_DUNGEON_RESULT_PHASES.md` (branch `main`) |
| Depends on | R1 done (its Done note, the commit, and the user's play-test of the console opener) |
| Flag | `SERV_IRUHADEV_JUNE_RESULT_WIDGET` (+ `SERV_IRUHADEV_JUNE_RESULT_DEBUG`) |
| Log tag | `[JUNE_RESULT] WIDGET` (`python tools/logs.py --tag JUNE_RESULT`) |

## Read first

```sh
python tools/docsection.py JUNE_DUNGEON_RESULT_PHASES.md "Starting context" "Phase R1 - The GFx widget" "R1 - Done"
```

Then the `(Result R1)` rows at the end of `june2017_re/funcmap.tsv` and the `Widget` block of `june2017_re/result_r0_map.tsv`.

## Task

Review R1's commit against June (read-only in Ghidra). Things this phase could get wrong:
- **to-AS member order, types and names** of `32eeb1d0` (UInt 4 / Number 5 / Boolean 2 / StringW 7; the `category_<rank><category>` index order), `32eeaff0`
  and the bool/trigger initial values (`32eea7e0`: the two retry bools start true, item `iNum` starts 1).
- The **listener**: name `UIDungeonResult`, `gameDataResultFunctionInit`, the five callbacks in table order, the always-true binding-created notice
  (`32eea220`) and G3's `CreateBindingImpl` now walking the listeners.
- `324d60e0` (fill): the six lists and their order (`324ddca0` is the comparator, `324de610` a unique insert), my row, ED cap, exp bar (range =
  `ExpTable(level+1).need`: confirm against `32741030`'s first int64), the retry value (`324d1120`), and the not-ported start block (`31d05fe0` /
  `31d05ec0`, routed to Stage 9: identify what they close).
- The **obtained-item list** and its caller in `UpdateInventorySlotList` (`3286d590 @3286d612-3286d707`): old quantity from the removed item, the
  `ST_E_DEFENCE_QUICK_SLOT` filter, `PT_QUANTITY` delta, and whether any June caller passes the second argument true (the 83 callers are mostly register pushes).
- Flag-off path: the `#else` is March; `flagcheck` below.

Hygiene is one command:
`python tools/flagcheck.py JUNE_RESULT_WIDGET --range <first R1 commit>^..HEAD --with JUNE_RESULT_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. The new `.cpp`'s PCH `#include "StdAfx.h"` line is the expected FAIL; explain every OTHER row.

Spot-check 2-3 items directly in Ghidra; if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then
`tools/build.py --deploy`); hand design decisions back to the user. When the review passes, retire `SERV_IRUHADEV_JUNE_RESULT_DEBUG`'s log lines only if R3 no
longer needs the console opener (R3 uses it for its play-test: keep it until the R3 review). Write "R1 - Review" into the plan doc, update the R1 row in
`STATUS.md`, check `kickoffs/june-result/R3.md` ("Amended by R1"), and commit.
