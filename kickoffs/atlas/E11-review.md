# Atlas/Elysion port - Phase E11 review (Memory line)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E11 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | E11 done (its Done note and commit; the data035 / 037 / 040 copy can come before or after) |
| Flag | `SERV_IRUHADEV_ELYSION_PORT` names the Lua blocks; E11 changed no C++ |
| Log tag | `[ELYDBG]`; load errors in `ErrorLog.txt` |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Per-line checklist" "Phase E11 — Memory" "Phase E11 — Done"
python -I june2017_re/region_census.py --line ME --limit 0 --no-probe
```

The boss decompile is `june_src_fixed/data037_kom/ELYSION_M_SOLESS.src.lua` (3875 lines).
Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review E11 against June:
- **Registry rows, verbatim:** diff the three `atlas E11` blocks against `june_src_fixed`: `DungeonData` June :33740-34002 (no allowed difference;
  June has no Memory quest key and no `DI_MEMORY_HERO_*`), `NPCTemplet` 30 rows (two runs, :21523-21702 and :21721-21810, ranges in the block
  header; check :21703-21720 really holds no Memory row), `WorldData` :3438-3462. Grep each new `dungeonID` / `NPC_ID` / `worldID` once.
- **Left out on purpose:** `ELYSION_M_2STAGE_GATE` / `M_HARNIE` / `M_SOLESS2` have no NUI and no June row; confirm no June stage or script spawns
  them (`Motion_M_2STAGE_GATE.x` is unstaged and needed only by `M_2STAGE_GATE`).
- **Unlock chain:** `EXTRA_REQUIRE_DUNGEON_ID = DI_SOLESS_NORMAL` (E10 clear key 33890); MinLevel 87.
- **Adopted files:** the 10 data040 files equal `ljconv(June)` (`june2017_re/june_adopt_20261017_172712.tsv`), manifest `"2"`.
- **Boss:** `IsReadyBreakAttack` (:3553) is only reached from `M_SOLESS`'s own CF (:3006): `MEMORY_1/2`'s `CF_ELYSION_M_MEMORY_BREAK_READY`
  (:1314) is referenced by no state, so the load-order question is moot; re-check that. The `OF_SUN` gauge (:3658, data034 dialog from E6);
  `GetBuffAccumulationCountNow( BTI_DEBUFF_DARK_SOLES_ATTACK ) >= 3` (:3456); the Stage1 SUB_STAGE2 `CC_TIME` (`CLEAR_TIME = 3`, March reads it at
  `X2DungeonSubStage.cpp:1608`). Spot-check 3 engine calls the boss makes against the census binding list.
- **JUNE_BUGS 245** (`LLOW_DIR_CHANGE`): June `32784c00` @32784d93 / @32784eb3 default false. `SHAW` (`M_2STAGE_END` / `M_3STAGE_END` dying states)
  stays a step-9 `keys.file` row: no June string, intent unknowable; confirm.
- **Play-test (if run):** every difficulty enters, Stage0-3 + BossStage load, the OF_SUN gauge and dramatic boxes show, Stage1's timed sub-stage
  clears after 3 s, the boss dies and the result screen shows.

Hygiene: `luasrc.py ledger --check` and `core --check`; if the user has copied, `region_census.py --line ME --live`.
Fix clear transcription errors (repack, ask before copy); hand design decisions back. Write "Phase E11 — Review" into the plan doc, update the E11
STATUS row, check E12's kickoff (E12 is optional; E13 is the asset closure), commit.
