# June effect completion - C3y - June-only dungeons no registry reaches, and the Sander secret boss world (census + plan)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3y` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3x |
| Flag | none expected (census and planning); a `SERV_IRUHADEV_JUNE_*` flag only if C++ must change |
| Log tag | `JUNE_FX C3y` |
| Review | none unless the phase ships code or data |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3x - Done"
head -20 june2017_re/world_census.tsv
python tools/docsection.py JUNE_UI_PARITY_PHASES.md --list      # then only the sections naming DI_SANDER_SECRET_HELL
```

## Task (routed by C3x, 2026-10-10; unowned June branch, planned per the standing rule)

1. **The 66 June-only dungeon scripts no registry reaches** (C3x kickoff's count; re-measure it). For each, list its
   `DungeonData.lua` / `DungeonEnum.lua` id, its worlds (`world_census.py`'s shape: WorldData row, world script, line map),
   and which plan's content it is (Atlas, Elysion, Hero, events, secret dungeons). Hand each to the owning plan's
   STATUS row; anything with no owner gets a phase row here.
2. **`WI_SANDER_SECRET_BOSS_STAGE`** (Henir Space's fifth missing boss-stage world; the other four are atlas / elysion's).
   No plan owns Sander's secret dungeon (`DI_SANDER_SECRET_HELL` 41141 is June-only, missing in March per JUNE_UI_PARITY).
   Plan its port: enum ids, `WorldData` row, world + line map, monsters, assets.
3. The deliverable is the census TSV and phase rows, not ports. A port that needs `DungeonData.lua` / `DungeonEnum.lua`
   (kept merged, core list) says so in its row.
