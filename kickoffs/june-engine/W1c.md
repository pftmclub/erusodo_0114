# June engine remainder - Phase W1c - GetLandPosition's monster-road bool

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine W1c` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | W1b (found it, 2026-10-17). Not alongside W2 or atlas E5 (`KTDGLineMap.*`) |
| Flag | `SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD` (proposed; split if the RE finds more than one change) |
| Log tag | `[JUNE_ENGINE] WORLD` (`python tools/logs.py --tag JUNE_ENGINE`) |
| Review | `kickoffs/june-engine/W1c-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase W1b -- Done" "Phase W1c - GetLandPosition monster-road bool"
grep -a "^31c7bb00" june2017_re/funcmap.tsv
```

March code: `KTDXLIB/KTDGLineMap.h/.cpp` (`GetLandPosition`, its three overloads at `KTDGLineMap.h:1158-1195`, the body at `KTDGLineMap.cpp:1069`).

## Task

**First map, then port.** June's `CKTDGLineMap::GetLandPosition` `31c7bb00` takes `( pos, bool, float fRadius, int* pLineIndex )`: the bool lets
`LT_MONSTER_ROAD` (7) lines count; without it they are skipped (users pass false, NPCs true). March's (`KTDGLineMap.cpp:1069`) has no bool and no
monster-road skip.

1. Read June's whole function against March's (both loops, the `lineIndex == -1` fallbacks, the nearest-point pass) and write every delta into the
   funcmap row `31c7bb00` (raise it to the full pseudocode).
2. List every June caller of `31c7bb00` (and of any wrapper overloads) with the bool it passes, and the matching March call sites. Put the list in a
   `.tsv` under `june2017_re/` if it is longer than ten rows.
3. Port June's whole function (rule 1: no partial update), keeping March's signature callable for callers June does not have. Every March call site
   that June has gets June's bool. W1b's `DoTriggerLine` tail (`X2World.cpp`, users false, NPCs true) is one of them.

**Done gate:** build rc=0 and DEPLOY OK; the caller table complete (no caller left with an unknown bool). Play-test: a dungeon with `LT_MONSTER_ROAD`
lines (find one in the June line maps), a user cannot land on one, a monster still walks it.
