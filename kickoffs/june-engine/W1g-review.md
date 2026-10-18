# June engine remainder - Phase W1g review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine W1g review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase W1g done (its Done note, commit and play-test) |
| Flag | `SERV_IRUHADEV_JUNE_PATHFINDER_FLY` (+ short-lived `SERV_IRUHADEV_JUNE_PATHFINDER_FLY_DEBUG`) |
| Log tag | `[JUNE_ENGINE] WORLD fly-down` (`python tools/logs.py --grep "WORLD fly-down"`) |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Phase W1g - FollowTargetPathFly as June's" "Phase W1g -- Done"
grep -a "(W1g)" june2017_re/funcmap.tsv
```

March code: `CX2SimplePathFinder::FollowTargetPathFly` (`X2Lib/X2SimplePathFinder.cpp:760`, the W1g block after `bFlyDown = true;`).

## Task

Review Phase W1g's commit against June's `CX2SimplePathFinder::FollowTargetPathFly` `32f27b10`. What this phase could get wrong:
- the gate's pushes at `@32f27e51-32f27ea2`: vector by value (scratch copy of `vCurrPos` at `[ESP+0x84]` after the prologue), radius `[0x33441b9c]` = 30.0,
  out position `&scratch`, in index `&[ESP+0x34]` (= `iCurrLineDataIndex`, stored `@32f27b72`), out NULL, then 1 / 1 / 0 / 0; March's call binds the 9-arg
  `CanDown` (not the 7-arg forwarder) with the same order;
- the flag bytes: `[ESP+0x13]` = bFlyUp, `[ESP+0x12]` = bFlyDown, the fail path `@32f27eb3` clears only bFlyDown; the two y compares (`10.0` double `@33438088`);
- the "rest = March" verdict: re-read the head (`+0xf4` / `+0xf8` = `m_lineGroupID`, types 4 / 5 / 8 = `JUMP_UP_REL` / `JUMP_UP_ABS` / `UNIT_ROAD`,
  `rand` into `+0x10` = `m_MoveState` 0xb / 0x11), the arrival / turn branches (0.3 `@3345993c`, -0.3 `@3348434c`), the four `GetLandPosition( .., true, 30, .. )`
  side checks (`+0x95` monster-allowed early return, `31c8ee10` == / `31cfd6b0` != against `this + 0`), and the final 12-state select. `RET 0x30` = March's
  12 arguments (the unused `fLineEndDetectRange` stays);
- the play-test lines: refusals and passes logged from the fly unit the Done note names.

Hygiene is one command:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_PATHFINDER_FLY --with SERV_IRUHADEV_JUNE_PATHFINDER_FLY_DEBUG --range <W1g commit>^..<W1g commit>`
(`git log -S SERV_IRUHADEV_JUNE_PATHFINDER_FLY` names it). Paste its `files:` / `in this flag's files:` / `RESULT:` lines.
Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Fix clear transcription errors under the same flag; turn `_DEBUG` off (comment the `#define` with the date and reason) and `tools/build.py --deploy`;
hand design decisions back to the user. Write "Phase W1g -- Review" into the plan doc, update the W1g STATUS.md row, and commit.
