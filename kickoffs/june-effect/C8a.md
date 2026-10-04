# June effect completion - C8 step 1 - compare the hit paths and SetDamageExceptionProcess overrides (Opus)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C8a` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C2 (the slot-0x2b8 rows) |
| Flag | none (comparison; verdicts into funcmap.tsv and the plan) |
| Log tag | `[JUNE_FX]` in ErrorLog.txt (`python tools/logs.py --tag JUNE_FX`) |
| Review | none (C8b is reviewed) |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C8 - Hit path"
python tools/docsection.py JUNE2017_EFFECT_REBUILD_PLAN.md "Starting context" "Phase 9"
```

Also read: `MODS.md` (SERV_IRUHADEV_JUNE_COLLISION / _DAMAGE_EXCEPTION entries), `june2017_re/funcmap.tsv` slot-0x2b8 rows

The inline prompt below predates `tools/`: wherever it says to build, deploy, check the exe,
make CP949 edits, run a unifdef or read logs by hand, use `tools/build.py --deploy`,
`tools/bytepatch.py`, `tools/flagcheck.py` and `tools/logs.py --tag JUNE_FX` instead (the x2-phase
skill's Tools and Finish lists). Never launch X2_offline.exe; ask the user. Ghidra work uses the
`ghidra` skill; identification verdicts go into `june2017_re/funcmap.tsv`.

## Task

**This session does the Opus half**: confirm each remaining hit path's June counterpart, compare the eight overrides and the NPC override, settle T6's far TEST samples if possible. Write a port list for C8b. No code.

Verbatim kickoff prompt from the plan:

```
June2017 effect completion C8. Read the rebuild plan's Starting context and Phase 9, MODS.md's
SERV_IRUHADEV_JUNE_COLLISION / _DAMAGE_EXCEPTION entries, funcmap.tsv slot-0x2b8 rows,
JUNE2017_EFFECT_COMPLETION_PHASES.md (C8). On Opus: confirm each remaining hit path's June
counterpart and compare the eight SetDamageExceptionProcess overrides. On Sonnet: port, citing
each. CP949 byte-level; build, deploy, ASK before launching; update matrix and MODS.md.
```
