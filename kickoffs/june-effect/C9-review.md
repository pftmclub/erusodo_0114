# June effect completion - C9 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9 done (its Done note and commits) |
| Flag | new `SERV_IRUHADEV_JUNE_*` flag(s), named in the Done note |
| Log tag | `[JUNE_FX]` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9 - The June effect files"
```

Also read: `june2017_re/funcmap.tsv` rows the phase cites, and its commits (`git log --oneline --grep="C9"`).

## Task

Review C9's commit(s) against the June binary and the plan's C9 section: the InsertDamageData_LUA binding, storage and the record apply against 32621080 / 3261a1a0 / 3264a5d0 (flag `SERV_IRUHADEV_JUNE_DAMAGE_DATA_TABLE`), the EffectSet_Description decline (C2's reason), and the staged `Elsword_DamageData.lua` (packed into data036). The UnitViewerEffectSetTable half was deliberately not ported (consumer unidentified): it is C9b, not a finding here. Read the play-test result first: the `JUNE_FX DAMAGE_DATA_LOAD ... rows=N` line in ErrorLog.txt (`python tools/logs.py --tag JUNE_FX`).

The plan's model rule is "transcription on Sonnet, then an Opus review"; this is that review.
Hygiene is one command: `python tools/flagcheck.py <FLAG> --range <first C9 commit>^..HEAD`
for each flag the phase used. Paste its `files:` / `in this flag's files:` / `RESULT:` lines, and
explain every OTHER row and deleted line; a FAIL or ERROR is a finding. Check `census.py` moved the
way the phase's Exit says.

Spot-check 2-3 sites directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "C9 - Review" under the phase in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
