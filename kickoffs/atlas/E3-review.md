# Atlas/Elysion port - Phase E3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E3 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | Phase E3 done (its Done note and commit); best after the E3 play-test |
| Flag | `SERV_IRUHADEV_ELYSION_PORT` |
| Log tag | `[ELYDBG]` |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Phase E3 — Field 03 (+ Field 04 check)" "Phase E3 — Done" "Phase A6 — Review (2026-10-17)"
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

E3 is data plus one C++ `case`. Review it against June (`june_src_fixed/`), the staged copies in `24965799\kom_contents\` and the live koms:
- **Verbatim checks** (parse and compare, don't eyeball): the F03 `BattleFieldData` entry vs June `data036_kom/BattleFieldData.src.lua:2158-2206`
  (identical, comments aside); `WorldData` WI 640 vs June `:3200-3204`; `Village[VMI_ELYSION]` START_POS 517 vs June `DLG_Map_Village.src.lua:991-1152`
  (now identical but the two `OtherPlace*` renames); the world map's F03 pin block vs June's `DLG_UI_World_Map_NEW`; `DLG_Map_House` changes are comments only.
- **June files** are byte-exact ljconv output (`june2017_re/june_adopt_20261017_111022.tsv`: sha1, size, `1B 4B 4C 81` header).
- **Field 04 is enum-only:** no `BattleFieldData` / `WorldData` row, scene or dialog for 40038 / WI 641 in June; `FieldWarpByCommand` 43 stays omitted.
- **C++:** `FieldWarpByCommand` case 42 against June's jump table @31d71550 (Ghidra, read-only); that the A7 stand-in at `X2WorldMapUI.cpp:495`
  now passes through (F03 has a name from `GetBattleFieldNameByBattleFieldId`).
- **Census:** `python -I june2017_re/region_census.py --region elysion --line FLD --live` after the copy: staged = live, no F03 file open.
- **Play-test result** (the user's report and `python tools/logs.py --tag ELYDBG`): village -> F03 (pos 517), field monsters spawn with HP and fight, return
  to the village (L0 -> 516), F03 pin on the world map, field-map dialog opens. Look for `CBattleFieldPortalMovingInfoPtr is NULL!`, missing textures, and
  `no reward row` (expected until backlog Stage 23).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_ELYSION_PORT --range <E3 commit>^..<E3 commit> --files X2Lib/X2Main.cpp`.
Fix clear transcription errors under the same flag; hand design decisions back to the user. Write "Phase E3 — Review" into the plan doc, update the E3 STATUS
row, write `kickoffs/atlas/E4.md` if missing (E4 = local map, Elysion page; Opus 5.5 / high), and commit.
