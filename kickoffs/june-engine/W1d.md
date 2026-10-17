# June engine remainder - Phase W1d - pet re-seat on the master's road type (CX2PETAI::Moving)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine W1d` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | W1c (found it, 2026-10-17). Not alongside a session editing `X2PetAI.cpp` / `X2Pet.cpp` |
| Flag | `SERV_IRUHADEV_JUNE_PET_AI_ROAD_RESEAT` (proposed) |
| Log tag | `[JUNE_ENGINE] WORLD` (`python tools/logs.py --tag JUNE_ENGINE`) |
| Review | `kickoffs/june-engine/W1d-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase W1c -- Done"
grep -a "^32ef6a95\|^32ef68b0" june2017_re/w1c_land_position_callers.tsv june2017_re/funcmap.tsv
```

March code: `CX2PETAI::Moving` (`X2Lib/X2PetAI.cpp:400`), `CX2PET::SetPosition` (`X2Pet.cpp:3129`), `CKTDGLineMap::GetNearOtherLine`.

## Task

**First map, then port.** W1c's caller map found a block in June's `CX2PETAI::Moving` (`32ef68b0`) that March lacks (`@32ef69a0-32ef6ab0`, around the
`GetLandPosition` call `@32ef6a95`, `bMonster` true). In a field or square game (`DAT_33775870` / `DAT_3377586c`) June looks up the pet's master (UID at
`+0xd8/+0xdc` through `323a8dd0` / `323a4dd0`), lands the master's position, calls `31c688c0` (`GetNearOtherLine`, radius `_DAT_33457460`) from the pet's
line (`+0x3c`), and when the pet's line type (`LineData +4`) differs from the master's last-touch line type it re-seats the pet with `326eb090`
(`CX2PET::SetPosition`). Read the whole of `32ef68b0` against March's `Moving` (rule 1: no partial update), write the deltas into funcmap, port them
whole, and add `JUNE_BUGS.md` rows for anything June does oddly (e.g. the line-type compare when the index is out of range).

**Done gate:** build rc=0 and DEPLOY OK. Play-test: in a village with `LT_OTHER_ROAD` lines (W1c's caller table names `CX2TFieldGame::OnFrameMove`'s
OTHER_ROAD block), summon a pet, cross onto the other road; the pet follows onto it (one log line per re-seat).
