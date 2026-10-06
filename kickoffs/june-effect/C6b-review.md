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

This review also covers C6a2's identification, which had no review of its own. Check C6b's "C6a2 findings" items against funcmap rows tagged `Effect C6a2` and spot-check them in the binary:
- **The stamps (`321ee600 @321ef639-321ef66d`):**
  - `m_iIndex` and `DamageData::m_iJuneDamageEffectIndex` both come from `EffectSetInstance::m_iDamageEffectIndex`. It defaults to **0**, not -1.
  - `m_iJuneAttackerStateID` comes from `m_iOwnerStateIDOnPlay` (default -1), nested in `SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION`.
- **PlayEffectSet (`321e1e60`):**
  - the vslot `0x248` call sits at `@321e2029`, before the custom-position block;
  - `GetNowStateID` is stored last, at `@321e20ce`;
  - neither appears in the square or pet overloads.
- **The four `m_iJuneDamageEffectIndex` writers:**
  - SimpleInit -1;
  - the SetDamageExceptionProcess tail `@3229971e`, from the base virtual returning -1;
  - the SetDamageDataNext copy `@31f79b87`;
  - the NEXT-parse reset `@31f79ce9`, before SetDamageDataFromLUA.
- **C6a2's negative verdicts:**
  - EffectData `+0x118` has no writer: `321f4690`'s four callers;
  - `ADD_RENA_SYSTEM` is off (`flagprobe.py`), so the first-attack copies stay untouched;
  - `m_iIndex` 0 cannot reach Rena's `GetInstanceByIndex`.
- If an identification turns out wrong, correct its funcmap row and tell the user; C6c (the buff list) builds on the same rows.

The plan's model rule is "transcription on Sonnet, then an Opus review"; this is that review.
Hygiene is one command: `python tools/flagcheck.py <FLAG> --range <first C6b commit>^..HEAD`
for each flag the phase used. Paste its `files:` / `in this flag's files:` / `RESULT:` lines, and
explain every OTHER row and deleted line; a FAIL or ERROR is a finding. Check `census.py` moved the
way the phase's Exit says.

Spot-check 2-3 sites directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "C6b - Review" under the phase in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
