# June engine remainder - Phase W1c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine W1c review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase W1c done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD` (+ short-lived `_DEBUG`) |
| Log tag | `[JUNE_ENGINE] WORLD` |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Phase W1c - GetLandPosition monster-road bool" "Phase W1c -- Done"
grep -a "^31c7bb00" june2017_re/funcmap.tsv
```

and `june2017_re/w1c_land_position_callers.tsv` (83 rows).

## Task

Review Phase W1c's commit(s) against June's `31c7bb00` and the caller table. What this phase could get wrong:
- the core: the skip's place in the filter chain (`@31c7bbc1`, after the wall skip, before the height test), and that the rest of June's
  function really is March's (`LineToLine` inlined: the degenerate / parallel / general branches, the `|dx|+|dy|+|dz| < r` hit test, the two
  index fallbacks and their tie rules);
- the default: March's 3-arg `GetLandPosition` (and the `unsigned char*` / `unsigned short*` wrappers) now forwards `true`; a March caller
  June does not have keeps March's behaviour that way. Check no overload became ambiguous or silently picked another one (a `float`
  passed where the bool now sits);
- the 10 `false` sites: re-read 2-3 in Ghidra (pick from `31fecb10 @31fed0fb`, `32010440 @32010ff8`, `323eef70 @323f0491`,
  `32561f70 @325621ed`, `32d2db50 @32d2dca1`) and confirm the identity and the pushed bool; the two merged June sites cover two March
  calls each (`X2GUUser.cpp` PhysicProcess start / end, `X2SquareUnit.cpp` PhysicProcess start / end);
- the rows marked medium (Eve `3298a2f0` / `3298c3d5` merged pairs, the path-finder site order); they pass `true` either way.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD --range <first W1c commit>^..<last W1c commit> --with SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD_DEBUG`
(`git log -S SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD` names the commits). Paste its `files:` / `in this flag's files:` / `RESULT:` lines.
Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

If the play-test passed, remove the short-lived `_DEBUG` flag (its `#define` and block) in the same pass and rebuild.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write
"Phase W1c -- Review" into the plan doc, update the W1c STATUS.md row, check `kickoffs/june-engine/W1d.md`, and commit.
