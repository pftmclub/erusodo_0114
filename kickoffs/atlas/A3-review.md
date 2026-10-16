# Atlas/Elysion port - Phase A3 review (region enums, Lua + C++)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas A3 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | A3 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_ELYSION_PORT` |
| Log tag | none (enums; the gate scripts are the evidence) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Phase A3 — Done" "March C++ end markers that Elysion IDs collide with" "A1a reachability summary"
```

## Task

Review A3's commit(s) against June (read-only in Ghidra, `june_src_fixed/data036_kom/Enum.src.lua`, `DungeonEnum.src.lua`):

1. **Values, both halves.** Re-run `python -I june2017_re/atlas_a3_check.py` (must print `A3 CHECK PASS`; it resolves the C++ with the preprocessor's flag state, so it also proves the flag reaches `Enum.h` / `XSLUnitManager.h`). Then spot-check 5 ids by hand against June's `Enum.src.lua`: two NUIs from different lines, `WI_ELYSION_FIELD_PVPMAP` 752, `HI_YUNO`, one `DI_*`.
2. **Sentinels.** June's five C++ markers (REST_END 1009, VILLAGE_MAP_END 20011, BATTLE_FIELD_END 40045, WI_END 779, DI_END 41530): re-read one June compare per marker (the A3 session re-read `32268dd0` = `x-20000 < 11`, `x-30000 < 7`, `x-1000 < 9`; `31efb3a0` = `< 0x30b`; `322b5970` = `x-40000 < 0x2d`). Judge the explicit-value rule (`#ifdef ... = N #else <original line> #endif`; `NUI_MAX_LIMIT` left implicit = 10196) and the Lua twins (`WI_END` 779, `VMI_BATTLE_FIELD_END` 40045).
3. **Consumer audit.** The table in the Done note lists every reader of every bumped marker. Confirm each is a range test or an ASSERT, and that `DI_END` used as a "no dungeon" return value (`X2UIQuest*.cpp`, `X2WorldMapUI.cpp`) cannot collide with a real id (41530 is above every DI in either enum).
4. **`IsVillageField`** (20000-20010 as the literal `20010`, nested under the existing `JUNE_FIELD_MAP` / `RANOX_PORT` blocks) and the **offline seed** (`X2OfflineLuaEnumSeed.h`: 3 markers + 7 rows).
5. **Staging.** `Enum.lua` / `DungeonEnum.lua` are edited in place in `kom_contents/data036_kom/` (not in git; ledger `refresh` done). Check `luasrc.py ledger --check`, that nothing a peer added to `Enum.lua` since 2026-10-11 was lost (diff against the Done note's counts), and that `data036_packed.kom` differs from the live kom by exactly `DungeonEnum.lua`, `Enum.lua` and A2's `ExpTable.lua`.

Hygiene: `python tools/flagcheck.py ELYSION_PORT --range <first A3 commit>^..HEAD` (the 9 EOL-mismatch lines in `Enum.h` are CRLF anchors inside an LF region; cosmetic). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions to the user. Write "Phase A3 -- Review" into the plan doc, update the A3 row in `STATUS.md`, and commit with explicit pathspecs. A4 and A5 then have their kickoffs to write on first use.
