# June effect completion - C3z-2 review - June's NPCs for the old dungeons

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3z-2 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3z-2 done (its Done note and commit); best after the user's Druid Forest play-test |
| Flag | none (C3z-2 changed no C++) |
| Log tag | `JUNE_FX C3z-2` (offline_server.log `GAME +NPC` / `stage N loading` lines; ErrorLog `[EXTRA_NPC_STAT] loaded`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z - old dungeons on June's data (user, 2026-10-10)" "C3z-2 - Done (2026-10-10, Opus 5.5 high; no C++, no flag, no build; data036 \`Enum.lua\` + data037 \`NPCTemplet.lua\` packed and copied)"
python tools/docsection.py JUNE_BUGS.md --list     # then only row 165 (grep "^| 165 |")
```

## Task

Review C3z-2's census and the two registry edits. No C++ changed, so there is no flagcheck range.

1. **Re-run the gate:** `python june2017_re/dungeon_npc_census.py`.
   - Expect 0 FAIL, `dungeon_npc_add.tsv` empty, and every EXC row named C3y.
   - A FAIL means a peer changed a dungeon script or a registry since the commit: say which.
2. **The census reads what the client reads.** Spot-check 3 sub-stages against the C++ walkers:
   - `X2Dungeon.cpp` `STAGE_LIST`;
   - `X2DungeonStage.cpp` `SUB_STAGE` / `READY_NPC`;
   - `X2DungeonSubStage.cpp` `New_LoadNPCData`, `ParseNpcGroupTable`, and the `CLEAR_COND` `NPC_ID` loop.

   Pick one `NPC_GROUP_RATE` dungeon (a Henir or Hero row) and one with `SUB_NPC`. Check the dumper's lines against the June source text in `june_src_fixed/`.
3. **Enum.lua** (data036, BOM source):
   - The 142 values equal June's (`lua_registry_dump.lua enum` on both files).
   - Nothing else in the file moved: diff it against the backup `kom_backup_tools/data036.kom.20261010_142222` (extract), or against the ledger's previous sha.
   - Re-check the collision claim against the C++ enum with one `cl /P` probe of `X2UnitManager.h`, implicit members included.
4. **NPCTemplet.lua** (data037):
   - The appended rows are byte-for-byte June's `june_src_fixed` blocks.
   - By value they equal June's own bytecode (junekom extract + ljconv, then `lua_registry_dump.lua calls`).
   - The 2,026 March rows equal the T0 snapshot (`24965799\kom_contents_bytecode\data037_kom\NPCTemplet.lua`) by value.
5. **Bookkeeping:** `luasrc.py ledger --check` and `core --check` PASS. `JUNE_BUGS.md` #165 is right: each named id is in a `READY_NPC` list, and is in neither Enum.
6. **Play-test logs** (if the user played):
   - `python tools/logs.py --grep "\+NPC .*id=16(0[89]|1[0-4])|stage [0-9]+ loading|EXTRA_NPC_STAT\] loaded"`.
   - Expect `nil NPC_ID` 709, not 835.
   - Expect Druid Forest stages to spawn more than the box.
   - 0 EXCEPTION.

Fix clear errors in the same files (pack, then ask before copy). Hand design decisions back to the user; the open DropTable stand-in is one. Write "C3z-2 - Review" into the plan doc, update STATUS, check the `C3z-3.md` kickoff (it now runs after C3z-2), and commit with explicit pathspecs.
