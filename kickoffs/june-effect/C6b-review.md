# June effect completion - C6b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C6b review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C6b done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SPAWN_INHERIT` and `SERV_IRUHADEV_JUNE_CREATE_EFFECT_SET` (RT_CREATE_EFFECT_SET, a second logical change in the same phase) |
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
- **C6b's second flag, `SERV_IRUHADEV_JUNE_CREATE_EFFECT_SET`** (RT_CREATE_EFFECT_SET, `31f85910 @31f86158-31f8672a`): spot-check in Ghidra
  - the trace-unit pointer test at the branch (not the TRACE_UNIT flag), and LIFTTIME only on that branch;
  - the retarget gate: the eight field IDs 40033/40034/40039-40044 (`3266cdc0`), `32481fa0`'s state, and that the boss unit stub only returns NULL;
  - the no-trace path: the mesh must exist, the call's position/rotation/direction (`31c82e40` = `CXMeshInstance::GetDirVec()`, `LOCAL_ROTATE_EFFECT` on), life -1.
- The kickoff's JUNE_FX line asked for the entry rate, the four gate flags and the index per spawn. They are split over `SPAWN_TAIL` (rate, entry flags) and `SPAWN_INHERIT` (gates, `hyper`, `index`).
- If an identification turns out wrong, correct its funcmap row and tell the user; C6c (the buff list) builds on the same rows.

The plan's model rule is "transcription on Sonnet, then an Opus review"; this is that review.
Hygiene is one command: `python tools/flagcheck.py <FLAG> --range df22a13^..HEAD`
for each of the two flags (`6382401` rewrote C6b's own debug line, so the range shows 2 deleted lines for SPAWN_INHERIT; `--base df22a13^ --at HEAD` on the code files shows the net, 0 deleted). Paste its `files:` / `in this flag's files:` / `RESULT:` lines, and
explain every OTHER row and deleted line; a FAIL or ERROR is a finding. Check `census.py` moved the
way the phase's Exit says.

Spot-check 2-3 sites directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "C6b - Review" under the phase in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
