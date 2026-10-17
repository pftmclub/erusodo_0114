# Atlas/Elysion port - Phase E10 review (Soless line)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E10 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | E10 done (its Done note and commit; the data035 / 037 / 040 copy can come before or after) |
| Flag | `SERV_IRUHADEV_ELYSION_PORT` names the Lua blocks; the one C++ change is under `SERV_IRUHADEV_ELYSION_DEBUG` |
| Log tag | `[ELYDBG]`; load errors in `ErrorLog.txt` |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Per-line checklist" "Phase E10 — Soless" "Phase E10 — Done" "Phase E5 — Review (2026-10-17)"
python -I june2017_re/region_census.py --line SL --limit 0 --no-probe
```

The boss has no decompile: `python tools/ljbc.py list` on `kom_contents/data037_kom/ELYSION_SOLESS.lua`.
Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review E10 against June:
- **Registry rows, verbatim:** diff the three `atlas E10` blocks against `june_src_fixed`: `DungeonData` June :33120-33354 (no allowed difference),
  `NPCTemplet` 45 rows (ranges in the block header), `WorldData` :3405-3437. Grep each new `dungeonID` / `NPC_ID` / `worldID` once. Confirm June has
  no `DI_SOLESS_HERO_*` row and that `SOLESS_EVENT2` / `_A` are spawned by no Soless script.
- **Unlock chain:** `EXTRA_REQUIRE_DUNGEON_ID = DI_ADRIAN_NORMAL` (E9 clear key 33880); Memory names `DI_SOLESS_NORMAL`.
- **Adopted files:** the 13 data040 files equal `ljconv(June)` (`june2017_re/june_adopt_20261017_150655.tsv`), manifest `"2"`.
- **Boss:** spot-check 3 of the "all 105 methods bound" claim and the `GetBuffAccumulationCountNow` compare (> 3) in the `ljbc` listing; the phase-2 sequence
  (`SetShowWorld` / `SetShowAdditionalWorld(0, true)`) against E7's reader; the true / bad ending condition functions.
- **JUNE_BUGS 240-243:** 240 against June `327f3c40` (stops at the first miss); 241 `SetPosion` truly unbound in June (`region_engine.tsv` B40); 242, 243.
- **Code:** `X2DungeonStage.cpp` `LoadReadyNPC` probe: no behaviour change, flag hygiene
  (`python tools/flagcheck.py SERV_IRUHADEV_ELYSION_DEBUG --range <E10 commit>^..<E10 commit> --files X2Lib/X2DungeonStage.cpp`).
- **Play-test fixes (commits `30b389d6`..`48f5194e`, see the Done note's play-test 1-4 bullets):**
  `SERV_IRUHADEV_ELYSION_MOVABLE_LINE_END` (a port-gap fix, not June: June's 31c6fbb0 / 32801ab0 read as drifting, the user saw June stop; JUNE_BUGS 230);
  `CX2Game::Release` order under `ELYSION_SCRIPT_B` (June 323080c0 @323083e5-32308416); `SERV_IRUHADEV_ELYSION_INVINCIBLE_ATTACK_TYPE` (state start
  327ad91e-327ad957, slot 95 3275bbf0, both DamageReact sites) and E7's parser fix (@3278f63c-3278f6a4); the user-chosen stand-in
  `SERV_IRUHADEV_ELYSION_ATTACK_TYPE_STANDIN` (checklist 7x); JUNE_BUGS 244. Range hygiene per flag with `flagcheck.py <FLAG> --range 30b389d6^..48f5194e`.
- **Play-test (if run):** the E5 review's two Stage0 checks (X02 skin mesh after the 7 s ride; does a unit on lines 34-39 ride along), the hidden stage,
  the AWAKEN / HERBAON gauges, phase 2, dramatic boxes, the ending.

Hygiene: `luasrc.py ledger --check` and `core --check`; if the user has copied, `region_census.py --line SL --live`.
Fix clear transcription errors (repack, ask before copy); hand design decisions back. Write "Phase E10 — Review" into the plan doc, update the E10
STATUS row, write `kickoffs/atlas/E11.md` from the template (Opus 5.5 high; Memory boss `ELYSION_M_SOLESS` is in `june_src_fixed/data037_kom/`), commit.
