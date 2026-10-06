# June effect completion - C5 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C5 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C5 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_DAMAGEDATA` (extended) |
| Log tag | `[JUNE_FX]` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C5 - 8b, buff-factor"
```

Also read: `june2017_re/funcmap.tsv` rows the phase cites, and its commits (`git log --oneline --grep="C5"`).

## Task

Review C5's commit(s) against the June binary and the plan's C5 section: the ParsingBuffFactorID family's selection order, the DAMAGE_TRIGGER_DATA sub-table and its consumer, TRACE_NEAREST_BONE's consumer 321da350, and that nothing the buff port owns was duplicated.

The plan's model rule is "transcription on Sonnet, then an Opus review"; this is that review.
Hygiene is one command: `python tools/flagcheck.py <FLAG> --range <first C5 commit>^..HEAD`
for each flag the phase used. Paste its `files:` / `in this flag's files:` / `RESULT:` lines, and
explain every OTHER row and deleted line; a FAIL or ERROR is a finding. Check `census.py` moved the
way the phase's Exit says.

Points the C5 Done note asks the review to settle (see its "Open for the review" list):
- the `GetEffectPosition` override branch: `321dceaa` loads the unit from the observable at instance `+0x14` (ctor arg 3 = the trace target, `321df2b7`), not from `+0xd4`'s raw copy;
- `CX2GameUnit::JuneGetNearestCollisionFrameName` is a third copy of `32278580`'s walk (the other two: the buff port's file-`static` in `X2BuffTemplet.cpp`, and `CEffect::JuneGetLockOnAimPos`). Fold them only if you judge it worth an OTHER row in buff-port code;
- the three buff-port callers that still compare `m_eDamageTrigger` instead of `JuneIsAttackTrigger` (listed in the Done note).

Spot-check 2-3 sites directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "C5 - Review" under the phase in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
