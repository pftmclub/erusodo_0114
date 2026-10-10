# June effect completion - C3y-1b - Sander secret worlds (port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3y-1b` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3y; can run with C3y-1a (no shared file); not with the port halves of C3y-1c / 2 / 3 / 4 (data036 Enum.lua, data040 WorldData.lua) |
| Flag | a `SERV_IRUHADEV_JUNE_*` flag only if C++ must change (none expected) |
| Log tag | `JUNE_FX C3y-1b` (`python tools/logs.py --grep "JUNE_FX C3y-1b"`) |
| Review | none unless the phase ships code |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3y-1b - port: Sander secret worlds (Henir Space stage 22 names the boss one)"
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3y - Done"
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3x - Done"
grep SANDER_SECRET june2017_re/c3y_unreached_world_census.tsv
```

## Task

1. **Enum** (data036 `Enum.lua`, core): `WI_SANDER_SECRET_STAGE0-3`, `_BOSS_STAGE`, `_EXTRA_STAGE1-3`, `_EXTRA_BOSS_STAGE` = June's
   614-622 (free in March; re-check before writing).
2. **WorldData** (data040 `WorldData.lua`, core): June's 9 rows verbatim (key by `worldID`, not `worldName`).
3. **Map files:** `luasrc.py adopt-june` the 9 world scripts and their 9 line maps (names in the world TSV).
4. **Assets:** after staging, `npc_mesh_census.py --worlds <list>` (inflates MSZIP). C3y's pre-count is 130 named, 112 missing
   (73 `.Y`, 33 `.x`, 6 `.ogg`), a lower bound. Check overlap with june-asset-port's staged files. Check each target kom's dir against
   the live archive before packing. Pack; ask before every copy.
5. **Gate:** world census: the 9 worlds `same` as June; the C3x CreateWorld log has no line for them. Henir Space stage 22 loads if a
   run reaches it (the order is not proven; say so in the hand-off rather than promising it).
