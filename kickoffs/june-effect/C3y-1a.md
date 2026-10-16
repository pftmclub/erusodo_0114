# June effect completion - C3y-1a - Hamel secret dungeon rows + the Ranox secret common row (port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3y-1a` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3y |
| Flag | none expected (data only); a `SERV_IRUHADEV_JUNE_*` flag only if C++ must change |
| Log tag | `JUNE_FX C3y-1a` (`python tools/logs.py --grep "JUNE_FX C3y-1a"`) |
| Review | `kickoffs/june-effect/C3y-1a-review.md` if the phase ships code |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3y-1a - port: Hamel secret dungeon rows + the Ranox secret common row"
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3y - Done"
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z-3 - Done (2026-10-10, Opus 5.5 high; no C++, no flag, no build; data035 + data036 + data065 packed and copied)"
grep -E "HAMEL_SECRET|RANOX_SECRET_COMMON" june2017_re/c3y_june_only_dungeon_ids.tsv
```

## Task

1. **DungeonData** (data035 `DungeonData.lua`, core, BOM source): add June's rows for `UNUSED:DI_HAMEL_SECRET_COMMON` 33260,
   `DI_HAMEL_SECRET_HELL` 33261 and `UNUSED:DI_RANOX_SECRET_COMMON` 41270 verbatim from `june_src_fixed/data035_kom/DungeonData.src.lua`,
   between C3z-3's `--{{ Iruha ... June's row` / `--}}` markers. Re-run `dungeon_registry_census.py` and the strict load test
   (`dungeon_registry_strict.lua`): 0 fail(s).
   - `m_eDefaultDungeonLoungeWorldID = WI_HAMEL_SECRET_STAGE_BOSS` names no enum in either build (reads `WI_NONE`): keep it verbatim
     with a `JUNE_BUGS.md` row, unless the strict test fails.
2. **Map button:** June's `DLG_Map_Local_Hamel_Icon_NEW.lua` (June data217) has `HamelSecret` + `_Desc`; March's data034 copy omitted
   them (UI parity Phase 3) and has a `.src.lua` sidecar. Default: take June's own file with `luasrc.py adopt-june` (June asset wins),
   after checking where `stage-june` places it (June data217, March data034) and whether UI parity Phase 3's other March-compat
   omissions in it still apply (each omitted control: does March now have its id / texture?). Only if March's copy must stay is it
   edited, and that needs the user's OK first (CLAUDE.md: a `.lua` with a `.src.lua` is asked about). Check the texture keys.
3. **Assets / items:** `HQ_LoadingDungeon_HARMEL_SECRET1/2.tga`, `DLG_UI_Dungeon_Image09.dds`; entry items 700015 / 113277 in item.kim
   (`/x2-kim`). Stage what March lacks (`kom.py stage-june`), pack, ask before every copy.
4. **Gate:** the registry census shows the 3 ids SAME; the Hamel local map shows the secret button; a Hamel Secret Hell room starts
   and stage 0 loads. The play-test hand-off names a level-70+ character from `els_db.sql`.
