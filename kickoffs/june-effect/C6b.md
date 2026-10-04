# June effect completion - C6 step 2 - port the five tail callees

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C6b` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C6a |
| Flag | `SERV_IRUHADEV_JUNE_SPAWN_INHERIT` |
| Log tag | `[JUNE_FX]` in ErrorLog.txt (`python tools/logs.py --tag JUNE_FX`) |
| Review | `kickoffs/june-effect/C6b-review.md` |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C6 - Spawner"
python tools/docsection.py JUNE2017_EFFECT_REBUILD_PLAN.md "Starting context" "Phase 10c"
```

Also read: `june2017_re/funcmap.tsv` (C6a's rows)

The inline prompt below predates `tools/`: wherever it says to build, deploy, check the exe,
make CP949 edits, run a unifdef or read logs by hand, use `tools/build.py --deploy`,
`tools/bytepatch.py`, `tools/flagcheck.py` and `tools/logs.py --tag JUNE_FX` instead (the x2-phase
skill's Tools and Finish lists). Never launch X2_offline.exe; ask the user. Ghidra work uses the
`ghidra` skill; identification verdicts go into `june2017_re/funcmap.tsv`.

## Task

**This session does step 2 only**, from C6a's verdicts. Under the no-partial-June-updates rule it also corrects March's existing hit and die spawners.

Verbatim kickoff prompt from the plan:

```
June2017 effect completion C6 (= rebuild plan Phase 10c steps 1-2). Read the rebuild plan's
Starting context and Phase 10c, JUNE2017_EFFECT_COMPLETION_PHASES.md (C6), funcmap.tsv. Step 1 on
Opus: identify the CEffect flags, entry fields and invoke-rate gate Phase 10c lists, verdicts into
funcmap.tsv. Step 2 on Sonnet: port the five tail callees as CEffect methods, call them from the
hit and die spawners under SERV_IRUHADEV_JUNE_SPAWN_INHERIT. Never port an offset. CP949
byte-level; build, deploy, ASK before launching; update matrix and MODS.md.
```
