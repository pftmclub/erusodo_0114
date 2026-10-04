# June effect completion - C8b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C8b review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C8b done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_COLLISION`, `SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION` |
| Log tag | `[JUNE_FX]` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C8 - Hit path"
python tools/docsection.py JUNE2017_EFFECT_REBUILD_PLAN.md "Phase 9"
```

Also read: `june2017_re/funcmap.tsv` rows the phase cites, and its commits (`git log --oneline --grep="C8b"`).

## Task

Review C8b's commit(s) against the June binary and the plan's C8b section: each of UnitToUnit, UnitToEffect, MeshToUnit, CollisionDataToUnit against its June counterpart, every ported SetDamageExceptionProcess override (per class and NPC), and the melee and projectile JUNE_FX HIT lines.

The plan's model rule is "transcription on Sonnet, then an Opus review"; this is that review.
Hygiene is one command: `python tools/flagcheck.py <FLAG> --range <first C8b commit>^..HEAD`
for each flag the phase used. Paste its `files:` / `in this flag's files:` / `RESULT:` lines, and
explain every OTHER row and deleted line; a FAIL or ERROR is a finding. Check `census.py` moved the
way the phase's Exit says.

Spot-check 2-3 sites directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "C8b - Review" under the phase in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
