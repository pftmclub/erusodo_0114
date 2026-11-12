# June dungeons - Phase W3b - every village and rest area against June

> Follow-up to W3 (done 2026-11-12). User report: travelling through Elder and Besma, NPC positions and village layout do not match, some textures are missing.
> The user asked for a full investigation of ALL villages and rest areas, not only those two.

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-dungeons W3b` |
| Plan doc | `JUNE_DUNGEONS_PHASES.md` (board row, "Phase W3b", Done note goes at its end as `## W3b - Done`) (branch `main`) |
| Depends on | W3 (done). Not alongside a session packing `data034`, `data040` or `data101` (atlas / Elysion world phases, the port halves of C3y-1b / 1c / 2 / 3 / 4) |
| Flag | none unless the census routes a C++ gap (then its own flagged phase) |
| Log tag | `[JUNE_ENGINE] WVIL` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_DUNGEONS_PHASES.md "Phase W3b - every village and rest area against June (user 2026-11-12, after W3)" "W3 - Done (2026-11-12, Sonnet 5.5)"
python tools/docsection.py MODS.md "June villages and rest areas, Lanox rest NPCs, field-name banner, Sander sign board (W3, 2026-11-12)" "data101 restored and Velder village assets checked (W3 follow-up, 2026-11-12)"
```

Then the skills `x2-adopt-june`, `elstools`, `x2-kom-packed`, `x2-lua`, `x2-check-logs`, `ghidra`.

## Task

Do "Phase W3b": census EVERY village and rest area (Ruben, Elder, Besma, Altera, Peita, Velder, Hamel, Sander, Ranox, Elysion; rest areas Elder, Besma,
Altera, Peita, Velder, Hamel, Sander, Ranox, Elysion, Atlas Station 1008, Arcade) against June for layout, NPC position and textures, prove each cause, fix
what is unambiguous, route the rest. Evidence and leads are in the plan doc section; do not take them as settled, prove or kill each.

Things W3 did and did not do, so you do not redo or trust them blindly:
- Done by W3: 13 world scripts / line maps adopted from June (`june2017_re/june_adopt_20261112_083532.tsv`), 43 assets staged
  (`june2017_re/w3_village_assets.tsv`), `data101` restored, `DLG_Map_House.lua` `COMMON_NPC_LOCATION` rows synced (9 moved, 29 added), Fieldname banner
  sheets and the Sander field-4 sign board. Backups under `kom_backup_stage_june\20261112_*`.
- Not done: the `Village[...]` blocks (only `HouseIDList` and `START_POS` were looked at, and not changed), the other per-house keys, the map-window Lua, and the
  texture closure of meshes that were already in March.

Method hints:
- Python on this machine fails on Git-Bash `/f/...` paths; use `F:\...` or relative paths from the right directory.
- June raw extract: `F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\dataNNN_kom\`; staged March: `...\237311\24965799\kom_contents\dataNNN_kom\`;
  June readable Lua: `june_src_fixed/data034_kom/*.src.lua`, `june_src_fixed/data040_kom/*.src.lua`.
- `kom.live_names()` / `staged_names()` give March's names; `junekom.Kom` + `junekom.decode` read any kom strictly (`elstools extract` fails on many entries).
- A mesh `.Y` is MSZIP-compressed X: inflate with `june2017_re/fxdata.xfile_text(lenient=True)` before reading texture names
  (see `june2017_re/npc_mesh_census.py`).
- Do not write a loose file into the game directory. Stage, pack, copy.

Done means: the census file `june2017_re/w3b_village_census.tsv` has no unexplained row, every adopted world script's texture closure is at 0 missing, the checks
in the plan doc pass, `ledger --check` and `core --check` PASS, a copy-ready play-test list for every place. Stop and hand a decision to the user if June's
`OtherPlaceID` semantics need a C++ change (say "this needs a design decision") or if a fix would add a June house whose NPC assets are absent.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum is never written as a number in new or touched code. A table whose
rows are the data itself (`.tsv`, a `Village[]` row of ids that already read as names in the file) stays as it is.
