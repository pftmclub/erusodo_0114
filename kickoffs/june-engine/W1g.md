# June engine remainder - Phase W1g - CX2SimplePathFinder::FollowTargetPathFly as June's (fly-down CanDown gate and the rest)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (map, then port), then type `/x2-phase june-engine W1g` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | W1f (found it, 2026-10-18; needs W1f's 9-arg `CanDown`). Not alongside a session editing `X2SimplePathFinder.*` |
| Flag | `SERV_IRUHADEV_JUNE_PATHFINDER_FLY` (proposed) |
| Log tag | `[JUNE_ENGINE] WORLD` (`python tools/logs.py --tag JUNE_ENGINE`) |
| Review | `kickoffs/june-engine/W1g-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Phase W1f -- Done"
grep -a "32f27b10\|32f27ea2" june2017_re/w1f_candown_callers.tsv june2017_re/funcmap.tsv
```

March code: `CX2SimplePathFinder::FollowTargetPathFly` (`X2Lib/X2SimplePathFinder.cpp:760`); caller `CX2NPCAI::Moving` (June `32f06860`, the
fly branch when the AI's `+0x6d` byte is 1, pathfinder at `ai + 0xa0`).

## Task

**First map, then port.** W1f's caller map found that June's `FollowTargetPathFly` (`32f27b10`) calls `CanDown` where March has none: when the
unit is more than `fDestArriveHeight + 10` above the destination (March's `bFlyDown`, `X2SimplePathFinder.cpp:873-886`), June calls
`CanDown( vCurrPos, LINE_RADIUS, &scratch, &copy of iCurrLineDataIndex, NULL, true, bIsMonster true, bOnLine false, false )` (`@32f27ea2`) and clears the
fly-down when it fails. The function differs elsewhere too (rule 1, no partial update): the head reads the destination line's `+0xf4` / `+0xf8`
(line-group ids), the `UNIT_ROAD` / `JUMP_UP_*` early return is `lineType 4 / 5 / 8` with `rand` into `+0x10`, and the landing is
`GetLandPosition( pos, true, ... )` (`31c7bb00`). Read the whole of `32f27b10` against March's function, write the deltas into funcmap, port them
whole, and add `JUNE_BUGS.md` rows for anything June does oddly.

**Done gate:** build rc=0 and DEPLOY OK. Play-test: a dungeon with a flying monster that follows the player down (the phase names one from the
NPC spawn tables, memory `playtest-dungeon-must-spawn-the-npc`); a short-lived `_DEBUG` line on the gate's first refusals.
