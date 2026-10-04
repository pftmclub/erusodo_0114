# June effect completion - C6b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C6b review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C6b done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SPAWN_INHERIT` |
| Log tag | `[JUNE_FX]` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C6 - Spawner"
python tools/docsection.py JUNE2017_EFFECT_REBUILD_PLAN.md "Phase 10c"
```

Also read: `june2017_re/funcmap.tsv` rows the phase cites, and its commits (`git log --oneline --grep="C6b"`).

## Task

Review C6b's commit(s) against the June binary and the plan's C6b section: the five callees 31f8fdb0, 31f649d0, 31f64c00, 31f65e40, 31f65f90 as CEffect methods, both spawners calling them, no June offset ported, and the JUNE_FX line.

The plan's model rule is "transcription on Sonnet, then an Opus review"; this is that review.
Hygiene is one command: `python tools/flagcheck.py <FLAG> --range <first C6b commit>^..HEAD`
for each flag the phase used. Paste its `files:` / `in this flag's files:` / `RESULT:` lines, and
explain every OTHER row and deleted line; a FAIL or ERROR is a finding. Check `census.py` moved the
way the phase's Exit says.

Spot-check 2-3 sites directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "C6b - Review" under the phase in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
