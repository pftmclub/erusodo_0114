# June effect completion - C3z-3 review - June's dungeon registry rows for the old dungeons

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3z-3 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3z-3 done (its Done note and commit); best after the user's play-test |
| Flag | none (C3z-3 changed no C++) |
| Log tag | `JUNE_FX C3z-3` (no new log lines: ErrorLog `OpenScriptFile` / `XEM_ERROR2` for `DungeonData.lua`, offline_server.log `GAME` lines) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z - old dungeons on June's data (user, 2026-10-10)" "C3z-3 - Done (2026-10-10, Opus 5.5 high; no C++, no flag, no build; data035 + data036 + data065 packed and copied)"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7s. Dungeon-registry class tables only the new classes read (from June effect completion C3z-3)"
```

## Task

Review C3z-3's census, the three Lua edits and the routing. No C++ changed, so there is no flagcheck range.

1. **Re-run the gate:** `python june2017_re/dungeon_registry_census.py`.
   - Expect 217 shared ids, all SAME, 0 March unresolved, and 0 STR_ID missing.
   - A non-SAME row means a peer changed a registry since the commit: say which.
2. **The census reads what the client reads.**
   - Check `dungeon_registry_loader_keys.txt` against `CX2DungeonManager::AddDungeonData_LUA` (`X2DungeonManager.cpp:112-574`).
     Every `#ifdef`'d read it lists must be compiled in. That was settled by key strings in the exe; confirm one with `cl /P`.
   - Check the census's `JUNE_LOADER` set against June's `327e0f80`: 47 XOR-guarded keys (`xorstrings.tsv`) plus 7 plain.
     Confirm `IS_HELL_MODE` really is absent: the decompile has 47 guard blocks, and all 47 are decrypted.
3. **Strict load:** run `june2017_re/dungeon_registry_strict.lua` from `elstools/vendor/luajit`, with DungeonData + Enum +
   DungeonEnum + StringID_def from kom_contents. Expect `OK 248 rows`.
   - Re-run both controls. The pre-C3z-3 Enum stops at row 86; the pre-C3z-3 DungeonEnum stops at row 172.
   - The pre-C3z-3 files are in the `kom_backup_tools/*.20261010_160527` koms; extract them.
4. **The edits are June's and only June's:**
   - Each `--{{ ... June's row` block is byte-equal to its `june_src_fixed` block.
   - Text outside the markers equals the T0 conversion.
   - `UNUSED_DUNGEON_ID` and `DUNGEON_CHECK_CONDITION` equal June's bytecode by value.
5. **Consumers named in the Done note.** Spot-check each:
   - `IsActiveDungeon` (the 92 dropped prerequisites);
   - `m_bHellMode` (`X2LocalMapUI.cpp:1486` / `1548` / `1569`; `CanGoOneSecretDungeonAtLeast` has no caller);
   - `m_mapBossDropItem` (party UI only);
   - `DI_END` (no Lua reader).
6. **Routing:** the C3z-5 / C3z-6 rows and kickoffs exist and match the Done note. Checklist 7s's counts match the census column.
7. **Play-test logs** (if the user played): `python tools/logs.py --grep "DungeonData|XEM_ERROR2|OpenScriptFile"`.
   Expect 0 EXCEPTION and no DungeonData load failure.

Fix clear errors in the same files (pack, then ask before copy). Hand design decisions back to the user. Write "C3z-3 - Review"
into the plan doc, update STATUS, and commit with explicit pathspecs.
