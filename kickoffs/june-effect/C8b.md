# June effect completion - C8 step 2 - port the hit paths and overrides

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C8b` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C8a. (C1b was dropped by D7, 2026-10-06: the `APPLIED_SKILL_ID` hooks read `DamageData::m_iAppliedSkillID`, stored since Phase 8c; port them as June has them.) |
| Flag | `SERV_IRUHADEV_JUNE_COLLISION`, `SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION` |
| Log tag | `[JUNE_FX]` in ErrorLog.txt (`python tools/logs.py --tag JUNE_FX`) |
| Review | `kickoffs/june-effect/C8b-review.md` |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C8 - Hit path"
python tools/docsection.py JUNE2017_EFFECT_REBUILD_PLAN.md "Starting context" "Phase 9"
```

Also read: `MODS.md` (the two flags' entries), `june2017_re/funcmap.tsv`, C8a's port list in the plan

The inline prompt below predates `tools/`: wherever it says to build, deploy, check the exe,
make CP949 edits, run a unifdef or read logs by hand, use `tools/build.py --deploy`,
`tools/bytepatch.py`, `tools/flagcheck.py` and `tools/logs.py --tag JUNE_FX` instead (the x2-phase
skill's Tools and Finish lists). Never launch X2_offline.exe; ask the user. Ghidra work uses the
`ghidra` skill; identification verdicts go into `june2017_re/funcmap.tsv`.

## Task

**This session does the Sonnet half**: port exactly C8a's list, citing each.

Verbatim kickoff prompt from the plan:

```
June2017 effect completion C8. Read the rebuild plan's Starting context and Phase 9, MODS.md's
SERV_IRUHADEV_JUNE_COLLISION / _DAMAGE_EXCEPTION entries, funcmap.tsv slot-0x2b8 rows,
JUNE2017_EFFECT_COMPLETION_PHASES.md (C8). On Opus: confirm each remaining hit path's June
counterpart and compare the eight SetDamageExceptionProcess overrides. On Sonnet: port, citing
each. CP949 byte-level; build, deploy, ASK before launching; update matrix and MODS.md.
```
