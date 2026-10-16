# June dungeons - C3y-1b - Henir Space's 5 missing boss worlds + the other Sander secret worlds (port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-dungeons C3y-1b` |
| Plan doc | `JUNE_DUNGEONS_PHASES.md` (branch `main`); this phase's text and the earlier Done notes still sit in `JUNE2017_EFFECT_COMPLETION_PHASES.md`, which the commands below read; write the new Done note into `JUNE_DUNGEONS_PHASES.md` |
| Depends on | C3y; can run with C3y-1a (no shared file); not with the port halves of C3y-1c / 2 / 3 / 4 (data036 Enum.lua, data040 WorldData.lua) |
| Flag | a `SERV_IRUHADEV_JUNE_*` flag only if C++ must change (none expected) |
| Log tag | `JUNE_FX C3y-1b` (`python tools/logs.py --grep "JUNE_FX C3y-1b"`) |
| Review | none unless the phase ships code |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3y-1b - port: Henir Space's 5 missing boss worlds + the other Sander secret worlds"
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3y - Done"
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3x - Done"
grep -E "SANDER_SECRET|ATLASDEVELOPMENT_BOSSATGE|TOWER_BOSSATGE|SKYCROSSROADS_BOSSSTAGE|ADRIAN_BOSSSTAGE" june2017_re/c3y_unreached_world_census.tsv
```

## Task

User, 2026-10-11: the 4 atlas / elysion Henir boss worlds are ported here, world-only, next to Sander's 9.

1. **Enum** (data036 `Enum.lua`, core): `WI_SANDER_SECRET_STAGE0-3`, `_BOSS_STAGE`, `_EXTRA_STAGE1-3`, `_EXTRA_BOSS_STAGE` = June's
   614-622, and `WI_ATLASDEVELOPMENT_BOSSATGE` 645, `WI_TOWER_BOSSATGE` 654, `WI_SKYCROSSROADS_BOSSSTAGE` 675, `WI_ADRIAN_BOSSSTAGE` 710 (all free in March; re-check before writing).
2. **WorldData** (data040 `WorldData.lua`, core): June's 13 rows verbatim (key by `worldID`, not `worldName`).
3. **Map files:** `luasrc.py adopt-june` the 13 world scripts and their 13 line maps (names in the world TSV).
4. **Binding audit** of the 13 adopted scripts (C3x's method): every method they call must be bound in March. The 4 Elysion boss-stage
   scripts may need elysion E5 (moving platforms) or E7 ("additional world"). Port a small world-only binding here; otherwise stop
   and report which call needs which E-phase. Never edit June's file to dodge one.
5. **Assets:** after staging, `npc_mesh_census.py --worlds <list>` (inflates MSZIP). C3y's pre-count: Sander 130 named / 112 missing,
   the 4 boss worlds 102 named / 93 missing; both lower bounds. Check overlap with june-asset-port's staged files. Check each target
   kom's dir against the live archive before packing. Pack; ask before every copy.
6. **Gate:** world census: the 13 worlds `same` as June; the C3x CreateWorld log has no line for them. Henir Space stages 22 and 32-35
   load if a run reaches them (the order is not proven; say so in the hand-off rather than promising it).
