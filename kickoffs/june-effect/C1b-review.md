# June effect completion - C1b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C1b review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C1b done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_NUMERICAL_DATA` |
| Log tag | `[JUNE_FX]` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C1 - Numerical"
```

Also read: `june2017_re/funcmap.tsv` rows the phase cites, and its commits (`git log --oneline --grep="C1b"`).

## Task

Review C1b's commit(s) against the June binary and the plan's C1b section: the override's conditions (index/name and skill id), the x0.01 PHYSIC/MAGIC scaling, which fields it overwrites, the table loader and its degrade path, where APPLIED_SKILL_ID/DAMAGE_SKILL_ID are stored, and census.py's item-1 check now passing.

The plan's model rule is "transcription on Sonnet, then an Opus review"; this is that review.
Hygiene is one command: `python tools/flagcheck.py <FLAG> --range <first C1b commit>^..HEAD`
for each flag the phase used. Paste its `files:` / `in this flag's files:` / `RESULT:` lines, and
explain every OTHER row and deleted line; a FAIL or ERROR is a finding. Check `census.py` moved the
way the phase's Exit says.

Spot-check 2-3 sites directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "C1b - Review" under the phase in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
