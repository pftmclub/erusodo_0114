# Atlas/Elysion port - Phase A2 review - Level cap 99

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas A2 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | Phase A2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_LEVEL_CAP_99` (+ `SERV_IRUHADEV_EXP_GAUGE_MAX_LEVEL_FIX`, the user's "-1.$%" request) |
| Log tag | `[CAP99]` (`python tools/logs.py --grep CAP99`) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Level cap" "Phase A2" "Phase A2 — Done"
python june2017_re/cap99_gen.py all          # the proofs, no writes (~10 s)
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch.

## Task

Review Phase A2's commit against the plan's three required checks:

1. **Generator proof.** Read `june2017_re/cap99_gen.py`.
   - Proof 1 must reproduce all 4000 live rows exactly.
   - Proof 2: the extrapolation agrees with the studio's commented-out rows for classes 110/111 on 188 of 190 values (AtkMagic L92, off by 1). Judge whether that miss is a float-tie artifact or a method error.
   - Classes 110/111 ship the studio rows. Check that they are copied verbatim.
   - Rerun `--write` and confirm the output is byte-identical (`git diff` empty).
   - Check the BOM, CRLF and no-EOF-newline of `StatTable.lua`, and the tabs of `NpcExpTable.lua`.
2. **Migration formula** (`CX2OfflineDB::MigrateExpToJuneTable`, schema v16) against both tables.
   - Check the index convention: TOTAL(L) reaches L, NEED(L) = TOTAL(L) - TOTAL(L-1).
   - Check the clamp of f to [0, 1), the level-80 case f = 0, `__int64` arithmetic, and `X2OfflineCap99Exp.h` against both tsvs.
   - After the play-test, compare every `[CAP99] migrated unit` line in `offline_server.log` with the Done note's expected table.
3. **Every 80-bound site.** Re-run the sweep:
   - `g_iMaxLevel` / `GetMaxLevel` / `MAX_LEVEL` / `MAX_NPC_LEVEL`;
   - the typedef;
   - the level-up loop's no-row guard;
   - the gauge fix (both branches).

   The two retained non-80 bounds are `X2BuffBehaviorTemplet.cpp:1339` (90 clamp) and `X2UISkillNote.cpp:62` (20..70 loop). Confirm they are still out of scope.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_LEVEL_CAP_99 --range <A2 commit>^..<A2 commit>`. Also run
`git log -SSERV_IRUHADEV_LEVEL_CAP_99 <A2 commit>..HEAD`.
- Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review.
- Explain every OTHER row and every deleted line.
- A FAIL or ERROR is a finding.

Fix clear defects under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "Phase A2 — Review" into the plan doc, update the A2 STATUS.md row, check `kickoffs/atlas/A3.md`, and commit.
